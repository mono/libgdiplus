/*
 * Copyright (c) 2004-2005 Ximian
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *	Jordi Mas i Hernandez <jordi@ximian.com>, 2004-2005
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 */

#include "imageattributes-private.h"
#include "bitmap-private.h"
#include "general-private.h"

static void
gdip_init_image_attribute (GpImageAttribute* attr)
{
	attr->flags = 0;
	attr->colormap = NULL;
	attr->colormap_elem = 0;
	attr->gamma_correction = 0.0f;
	attr->key_colorlow = 0;
	attr->key_colorhigh = 0;
	attr->colormatrix = NULL;
	attr->graymatrix = NULL;
	attr->colormatrix_flags = ColorMatrixFlagsDefault;
	attr->threshold = 0;
	attr->outputchannel_flags = 0;
	attr->colorprofile_filename = NULL;
}

static void
gdip_dispose_image_attribute (GpImageAttribute* attr)
{
	if (attr->colormap) {
		GdipFree (attr->colormap);
		attr->colormap = NULL;
	}

	if (attr->colormatrix) {
		GdipFree (attr->colormatrix);
		attr->colormatrix = NULL;
	}

	if (attr->graymatrix) {
		GdipFree (attr->graymatrix);
		attr->graymatrix = NULL;
	}

	if (attr->colorprofile_filename) {
		GdipFree (attr->colorprofile_filename);
		attr->colorprofile_filename = NULL;
	}
}

static GpImageAttribute*
gdip_get_image_attribute (GpImageAttributes* attr, ColorAdjustType type)
{
	switch (type)	{
	case ColorAdjustTypeDefault:
		return &attr->def;
	case ColorAdjustTypeBitmap:
		return &attr->bitmap;
	case ColorAdjustTypeBrush:
		return &attr->brush;
	case ColorAdjustTypePen:
		return &attr->pen;
	case ColorAdjustTypeText:
		return &attr->text;
	default:
		return NULL;
	}
}

void
gdip_process_bitmap_attributes (GpBitmap *bitmap, void **dest, GpImageAttributes* attr, BOOL *allocated)
{
	GpStatus;
	GpImageAttribute *imgattr, *def;
	GpImageAttribute *colormap, *gamma, *trans, *cmatrix;
	GpBitmap *bmpdest;
	ARGB color;
	BYTE *color_p = (BYTE*) &color;

	*allocated = FALSE;
	bmpdest = NULL;

	if (!bitmap || !dest || !attr)
		return;

	imgattr = gdip_get_image_attribute (attr, ColorAdjustTypeBitmap);

	if (!imgattr)
		return;

	def = gdip_get_image_attribute (attr, ColorAdjustTypeDefault);
	if (imgattr->flags & ImageAttributeFlagsColorRemapTableEnabled) {
		colormap = imgattr;
	} else {
		colormap = def;
	}

	if (imgattr->flags & ImageAttributeFlagsGammaEnabled) {
		gamma = imgattr;
	} else {
		gamma = def;
	}

	if (imgattr->flags & ImageAttributeFlagsColorKeysEnabled) {
		trans = imgattr;
	} else {
		trans = def;
	}

	if (imgattr->flags & ImageAttributeFlagsColorKeysEnabled && imgattr->colormatrix) {
		cmatrix = imgattr;
	} else {
		cmatrix = def;
	}

	if ((colormap->flags & ImageAttributeFlagsColorRemapTableEnabled) || (gamma->flags & ImageAttributeFlagsGammaEnabled) || (trans->flags & ImageAttributeFlagsColorKeysEnabled) ||
	    ((cmatrix->flags & ImageAttributeFlagsColorMatrixEnabled) && cmatrix->colormatrix != NULL)) {
		bmpdest = gdip_bitmap_new_with_frame (NULL, FALSE);
		if (!bmpdest)
			return;

		bitmap->active_bitmap->pixel_format = PixelFormat32bppARGB;
		status = gdip_bitmapdata_clone (bitmap->active_bitmap, &bmpdest->frames[0].bitmap, 1)
		if (status != Ok) {
			gdip_bitmap_dispose (bmpdest);
			return;
		}

		bmpdest->frames[0].count = 1;
		gdip_bitmap_setactive(bmpdest, NULL, 0);
		*dest = bmpdest->active_bitmap->scan0;
		*allocated = TRUE;
	}

	/*
		We use get/set pixel instead of direct buffer manipulation because it's a good way of keeping the pixel
		logic in a single place
	*/

	/* Color mapping */
	if (colormap->flags & ImageAttributeFlagsColorRemapTableEnabled) {
		for (int y = 0; y <bitmap->active_bitmap->height; y++) {
			for (int x = 0; x <bitmap->active_bitmap->width; x++) {
				ColorMap* clrmap = colormap->colormap;

				GdipBitmapGetPixel (bmpdest, x, y, &color);

				for (int cnt = 0; cnt < colormap->colormap_elem; cnt++, clrmap++) {

					if (color == clrmap->oldColor.Argb) {
						color = clrmap->newColor.Argb;
						GdipBitmapSetPixel (bmpdest, x, y, color);
						break;
					}
				}
			}
		}
	}

	/* Gamma correction */
	if (gamma->flags & ImageAttributeFlagsGammaEnabled) {
		for (int y = 0; y < bitmap->active_bitmap->height; y++) {
			for (int x = 0; x < bitmap->active_bitmap->width; x++) {

				BYTE r,g,b,a;

				GdipBitmapGetPixel (bmpdest, x, y, &color);

				a = (color & 0xff000000) >> 24;
				r = (color & 0x00ff0000) >> 16;
				g = (color & 0x0000ff00) >> 8;
				b = (color & 0x000000ff);

				/* FIXME: This is not the right gamma GDI + correction algorithm */

				/*
				r = (int) powf (r, (1 / gamma->gamma_correction));
				g = (int) powf (g, (1 / gamma->gamma_correction));
				b = (int) powf (b, (1 / gamma->gamma_correction));
				a = (int) powf (a, (1 / gamma->gamma_correction));*/

				color = b | (g  << 8) | (r << 16) | (a << 24);

				GdipBitmapSetPixel (bmpdest, x, y, color);
			}
		}

	}

	/* Apply transparency range */
	if (trans->flags & ImageAttributeFlagsColorKeysEnabled) {
		for (int y = 0; y < bitmap->active_bitmap->height; y++) {
			for (int x = 0; x < bitmap->active_bitmap->width; x++) {

				GdipBitmapGetPixel (bmpdest, x, y, &color);

				if (color >= trans->key_colorlow && color <= trans->key_colorhigh) {
					color = color & 0x00ffffff; /* Alpha = 0 */
					GdipBitmapSetPixel (bmpdest, x, y, color);
				}
			}
		}
	}

	/* Apply Color Matrix */
	if (cmatrix->flags & ImageAttributeFlagsColorMatrixEnabled && cmatrix->colormatrix) {
		BitmapData *data = bmpdest->active_bitmap;
		BYTE *v = ((BYTE*)data->scan0);
		ARGB *scan;
		ColorMatrixFlags flags = cmatrix->colormatrix_flags;
		ColorMatrix *cm;
		BOOL bmpdest_is_premultiplied = !gdip_bitmap_format_needs_premultiplication (bmpdest);

		for (int y = 0; y < data->height; y++) {
			scan = (ARGB*) v;
			for (int x = 0; x < data->width; x++) {
				BYTE r,g,b,a;
				int r_new,g_new,b_new,a_new;

				get_pixel_bgra (*scan, b, g, r, a);

				/* by default the matrix applies to all colors, including grays */
				if ((flags != ColorMatrixFlagsDefault) && (b == g) && (b == r)) {
					if (flags == ColorMatrixFlagsSkipGrays) {
						/* does not apply */
						scan++;
						continue;
					}
					/* ColorMatrixFlagsAltGray */
					cm = cmatrix->graymatrix;
				} else {
					cm = cmatrix->colormatrix;
				}

				a_new = (r * cm->m[0][3] + g * cm->m[1][3] + b * cm->m[2][3] + a * cm->m[3][3] + (255 * cm->m[4][3]));
				if (a_new == 0 && bmpdest_is_premultiplied) {
					/* 100% transparency, don't waste time computing other values (pre-mul will always be 0) */
					*scan++ = 0;
				} else {
					r_new = (r * cm->m[0][0] + g * cm->m[1][0] + b * cm->m[2][0] + a * cm->m[3][0] + (255 * cm->m[4][0]));
					g_new = (r * cm->m[0][1] + g * cm->m[1][1] + b * cm->m[2][1] + a * cm->m[3][1] + (255 * cm->m[4][1]));
					b_new = (r * cm->m[0][2] + g * cm->m[1][2] + b * cm->m[2][2] + a * cm->m[3][2] + (255 * cm->m[4][2]));

					if (bmpdest_is_premultiplied && a != (BYTE) a_new && a < 0xff && a != 0) {
						/* reverse previous pre-multiplication if necessary */
						r_new = r_new * 255 / a;
						g_new = g_new * 255 / a;
						b_new = b_new * 255 / a;
					}

					r = (r_new > 0xff) ? 0xff : (BYTE) r_new;
					g = (g_new > 0xff) ? 0xff : (BYTE) g_new;
					b = (b_new > 0xff) ? 0xff : (BYTE) b_new;

					/* remember that Cairo use pre-multiplied alpha, e.g. 50% red == 0x80800000 not 0x80ff0000 */
					if (bmpdest_is_premultiplied && a != (BYTE) a_new && a_new < 0xff) {
						/* apply new pre-multiplication */
						a = (BYTE) a_new;
						r = pre_multiplied_table [r][a];
						g = pre_multiplied_table [g][a];
						b = pre_multiplied_table [b][a];
					} else {
						a = (BYTE) a_new;
					}

					set_pixel_bgra (color_p, 0, b, g, r, a);
					*scan++ = color;
				}
			}
			v += data->stride;
		}
	}

	if (bmpdest != NULL) {
		bmpdest->active_bitmap->scan0 = NULL;
		gdip_bitmap_dispose (bmpdest);
	}
}

/* coverity[+alloc : arg-*0] */
GpStatus WINGDIPAPI
GdipCreateImageAttributes (GpImageAttributes **imageattr)
{
	GpImageAttributes *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!imageattr)
		return InvalidParameter;

	result = (GpImageAttributes *) GdipAlloc (sizeof (GpImageAttributes));
	if (!result) {
		*imageattr = NULL;
		return OutOfMemory;
	}

	gdip_init_image_attribute (&result->def);
	gdip_init_image_attribute (&result->bitmap);
	gdip_init_image_attribute (&result->brush);
	gdip_init_image_attribute (&result->pen);
	gdip_init_image_attribute (&result->text);
	result->color = 0x00000000;
	result->wrapmode = WrapModeClamp;

	*imageattr = result;
	return Ok;
}

/* coverity[+alloc : arg-*1] */
GpStatus WINGDIPAPI
GdipCloneImageAttributes (GDIPCONST GpImageAttributes *imageattr, GpImageAttributes **cloneImageattr)
{
	GpImageAttributes *result;

	if (!imageattr || !cloneImageattr)
		return InvalidParameter;

	result = (GpImageAttributes *) GdipAlloc (sizeof (GpImageAttributes));
	if (!result) {
		*cloneImageattr = NULL;
		return OutOfMemory;
	}

	memcpy (result, imageattr, sizeof (GpImageAttributes));

	*cloneImageattr = result;
	return Ok;

}

GpStatus WINGDIPAPI
GdipDisposeImageAttributes (GpImageAttributes *imageattr)
{
	if (!imageattr)
		return InvalidParameter;

	gdip_dispose_image_attribute (&imageattr->def);
	gdip_dispose_image_attribute (&imageattr->bitmap);
	gdip_dispose_image_attribute (&imageattr->brush);
	gdip_dispose_image_attribute (&imageattr->pen);
	gdip_dispose_image_attribute (&imageattr->text);

	GdipFree (imageattr);
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesToIdentity (GpImageAttributes *imageattr, ColorAdjustType type)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	gdip_dispose_image_attribute (imgattr);
	gdip_init_image_attribute (imgattr);
	return Ok;
}

GpStatus WINGDIPAPI
GdipResetImageAttributes (GpImageAttributes *imageattr, ColorAdjustType type)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	gdip_dispose_image_attribute (imgattr);
	gdip_init_image_attribute (imgattr);
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesThreshold ( GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, REAL threshold)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag) {
		imgattr->threshold = threshold;
		imgattr->flags |= ImageAttributeFlagsThresholdEnabled;
	}
	else
		imgattr->flags |= ~ImageAttributeFlagsThresholdEnabled;

	return Ok;
}


GpStatus WINGDIPAPI
GdipSetImageAttributesGamma (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, REAL gamma)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag) {
		if (gamma <= 0)
			return InvalidParameter;

		imgattr->gamma_correction = gamma;
		imgattr->flags |= ImageAttributeFlagsGammaEnabled;
	}
	else
		imgattr->flags |= ~ImageAttributeFlagsGammaEnabled;

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesNoOp (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag)
		imgattr->flags |= ImageAttributeFlagsNoOp;
	else
		imgattr->flags |= ~ImageAttributeFlagsNoOp;

	return Ok;
}


GpStatus WINGDIPAPI
GdipSetImageAttributesColorKeys (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, ARGB colorLow, ARGB colorHigh)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag) {
		BYTE rLow = (colorLow >> 16) & 0xFF;
		BYTE gLow = (colorLow >> 8) & 0xFF;
		BYTE bLow = colorLow & 0xFF;

		BYTE rHigh = (colorHigh >> 16) & 0xFF;
		BYTE gHigh = (colorHigh >> 8) & 0xFF;
		BYTE bHigh = colorHigh & 0xFF;

		if (rLow > rHigh || gLow > gHigh || bLow > bHigh)
			return InvalidParameter;

		imgattr->key_colorlow = colorLow;
		imgattr->key_colorhigh = colorHigh;
		imgattr->flags|= ImageAttributeFlagsColorKeysEnabled;
	}
	else
		imgattr->flags|= ~ImageAttributeFlagsColorKeysEnabled;

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesOutputChannelColorProfile (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, GDIPCONST WCHAR *colorProfileFilename)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag) {
		if (!colorProfileFilename)
			return Win32Error;

		char *utf8 = ucs2_to_utf8 (colorProfileFilename, -1);
		if (!utf8)
			return OutOfMemory;

		FILE *fileHandle = fopen (utf8, "rb");
		if (!fileHandle) {
			GdipFree (utf8);
			return OutOfMemory;
		}

		fclose (fileHandle);

		if (imgattr->colorprofile_filename)
			GdipFree (imgattr->colorprofile_filename);

		imgattr->colorprofile_filename = utf8;
		imgattr->flags |= ImageAttributeFlagsOutputChannelColorProfileEnabled;
	}
	else
		imgattr->flags |= ~ImageAttributeFlagsOutputChannelColorProfileEnabled;

	return Ok;
}


GpStatus WINGDIPAPI
GdipSetImageAttributesRemapTable (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, UINT mapSize, GDIPCONST ColorMap *map)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag) {
		if (mapSize == 0 || !map)
			return InvalidParameter;

		/* Copy colormap table */
		int size = mapSize * sizeof (ColorMap);
		ColorMap *newColorMap = GdipAlloc (size);
		if (!newColorMap)
			return OutOfMemory;

		if (imgattr->colormap)
			GdipFree (imgattr->colormap);

		imgattr->colormap = newColorMap;
		memcpy (imgattr->colormap, map, size);
		imgattr->colormap_elem = mapSize;
		imgattr->flags |= ImageAttributeFlagsColorRemapTableEnabled;
	} else
		imgattr->flags |= ~ImageAttributeFlagsColorRemapTableEnabled;

	return Ok;
}

/*
	According to Microsoft documentation:
	clamp: This parameter has no effect in Microsoft® Windows® GDI+ version 1.0
*/
GpStatus WINGDIPAPI
GdipSetImageAttributesWrapMode (GpImageAttributes *imageattr, WrapMode wrap, ARGB argb, BOOL clamp)
{
	if (!imageattr)
		return InvalidParameter;

	imageattr->wrapmode = wrap;
	imageattr->color = argb;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesICMMode (GpImageAttributes *imageAttr, BOOL on)
{
	if (!imageAttr)
		return InvalidParameter;

	// This has no effect in GDI+.
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetImageAttributesAdjustedPalette (GpImageAttributes *imageattr, ColorPalette *colorPalette, ColorAdjustType type)
{
	if (!imageattr || !colorPalette || !colorPalette->Count || type == ColorAdjustTypeDefault)
		return InvalidParameter;

	if (type >= ColorAdjustTypeCount)
		return InvalidParameter;

	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesColorMatrix (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag,
	GDIPCONST ColorMatrix* colorMatrix, GDIPCONST ColorMatrix* grayMatrix, ColorMatrixFlags flags)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag) {
		if (!colorMatrix || flags > ColorMatrixFlagsAltGray) {
			return InvalidParameter;
		}

		if (!imgattr->colormatrix) {
			imgattr->colormatrix = GdipAlloc (sizeof (ColorMatrix));
			if (!imgattr->colormatrix)
				return OutOfMemory;
		}

		if (flags == ColorMatrixFlagsAltGray) {
			if (!grayMatrix)
				return InvalidParameter;

			if (!imgattr->graymatrix) {
				imgattr->graymatrix = GdipAlloc (sizeof (ColorMatrix));
				if (!imgattr->graymatrix)
					return OutOfMemory;
			}

			memcpy (imgattr->graymatrix, grayMatrix, sizeof (ColorMatrix));
			imgattr->flags |= ImageAttributeFlagsGrayMatrixEnabled;
		}

		memcpy (imgattr->colormatrix, colorMatrix, sizeof (ColorMatrix));
		imgattr->colormatrix_flags = flags;
		imgattr->flags |= ImageAttributeFlagsColorMatrixEnabled;
	} else {
		imgattr->flags |= ~(ImageAttributeFlagsColorMatrixEnabled | ImageAttributeFlagsGrayMatrixEnabled);
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesOutputChannel (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, ColorChannelFlags channelFlags)
{
	GpImageAttribute *imgattr;

	if (!imageattr)
		return InvalidParameter;

	imgattr = gdip_get_image_attribute (imageattr, type);

	if (!imgattr)
		return InvalidParameter;

	if (enableFlag) {
		if (channelFlags >= ColorChannelFlagsLast)
			return InvalidParameter;

		imgattr->outputchannel_flags = channelFlags;
		imgattr->flags |= ImageAttributeFlagsOutputChannelEnabled;
	} else
		imgattr->flags |= ~ImageAttributeFlagsOutputChannelEnabled;

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetImageAttributesCachedBackground (GpImageAttributes *imageattr, BOOL enableFlag)
{
	if (!imageattr)
		return InvalidParameter;

	// This has no effect in GDI+.
	return Ok;
}