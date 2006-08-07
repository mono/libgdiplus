/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4; -*-
 *
 * image.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
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
 *   Alexandre Pigolkine(pigolkine@gmx.de)
 *   Sanjay Gupta (gsanjay@novell.com)
 *   Vladimir Vukicevic (vladimir@pobox.com)
 *   Jordi Mas (jordi@ximian.com)
 *   Jonathan Gilbert (logic@deltaq.org)
 */

#include <stdio.h>
#include "gdipImage.h"
#include <cairo-features.h>
#include <math.h>

static GpStatus gdip_flip_x (GpImage *image);
static GpStatus gdip_flip_y (GpImage *image);
cairo_filter_t gdip_get_cairo_filter (InterpolationMode imode);

/*
 * format guids
 */

GUID gdip_membmp_image_format_guid = {0xb96b3caaU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_bmp_image_format_guid = {0xb96b3cabU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_jpg_image_format_guid = {0xb96b3caeU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_png_image_format_guid = {0xb96b3cafU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_gif_image_format_guid = {0xb96b3cb0U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_tif_image_format_guid = {0xb96b3cb1U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_exif_image_format_guid = {0xb96b3cb2U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_wmf_image_format_guid = {0xb96b3cadU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_emf_image_format_guid = {0xb96b3cacU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

/*
 * encoder param guids
 */
GUID GdipEncoderQuality = {0x1d5be4b5U, 0xfa4aU, 0x452dU, {0x9c, 0xdd, 0x5d, 0xb3, 0x51, 0x05, 0xe7, 0xeb}};
GUID GdipEncoderCompression = {0xe09d739dU, 0xccd4U, 0x44eeU, {0x8e, 0xba, 0x3f, 0xbf, 0x8b, 0xe4, 0xfc, 0x58}};

void 
gdip_image_init (GpImage *image)
{
	gdip_bitmap_init(image);
} 


GpStatus 
GdipDisposeImage (GpImage *image)
{
	if (!image)
		return InvalidParameter;

	if (image->type == imageBitmap) {
		gdip_bitmap_dispose(image);
		return Ok;
	}

	if (image->type == imageMetafile) {
		return Ok;
	}

	/* imageUndefined */
	return Ok;
}

/*
	Microsoft GDI+ only supports these pixel formats Format24bppRgb, Format32bppArgb, 
	Format32bppPArgb, Format32bppRgb, Format48bppRgb, Format64bppArgb, Format64bppPArgb
*/
GpStatus 
GdipGetImageGraphicsContext (GpImage *image, GpGraphics **graphics)
{
	GpGraphics	*gfx;
	cairo_surface_t	*surface;
	cairo_pattern_t	*filter;
	
	if ((image  == NULL) || (image->active_bitmap == NULL) || (graphics == NULL)) {
		return InvalidParameter;
	}

	if (image->type != imageBitmap) {
		return NotImplemented;
	}

	surface = cairo_image_surface_create_for_data ((unsigned char *) image->active_bitmap->scan0, image->cairo_format,
				image->active_bitmap->width, image->active_bitmap->height, image->active_bitmap->stride);

	gfx = gdip_graphics_new (surface);
	gfx->dpi_x = gfx->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);

	gfx->image = image;
	gfx->type = gtMemoryBitmap;
	filter = cairo_pattern_create_for_surface (image->surface);
	cairo_pattern_set_filter (filter, gdip_get_cairo_filter (gfx->interpolation));
	cairo_pattern_destroy (filter);
	*graphics = gfx;
	return Ok;
}

#ifdef CAIRO_HAS_PS_SURFACE

#include <cairo-ps.h>

GpStatus 
GdipGetPostScriptGraphicsContext (char* filename, int width, int height, double dpix, double dpiy, GpGraphics **graphics)
{
	GpGraphics *gfx;
	cairo_surface_t *surface;
	
	if (!graphics)
		return InvalidParameter;

	surface = cairo_ps_surface_create (filename, (double) width, (double) height);
	cairo_surface_set_fallback_resolution (surface, dpix, dpiy);

	gfx = gdip_graphics_new (surface);
	gfx->dpi_x = dpix;
	gfx->dpi_y = dpiy;
	cairo_surface_destroy (surface);

	gfx->type = gtPostScript;
	*graphics = gfx;
	return Ok;
}

GpStatus 
GdipGetPostScriptSavePage (GpGraphics* graphics) 
{
	if (!graphics)
		return InvalidParameter;

	cairo_show_page (graphics->ct);
	return Ok;
}

#endif

GpStatus 
GdipDrawImageI (GpGraphics *graphics, GpImage *image, int x, int y)
{
	if (!image) {
		return InvalidParameter;
	}

	return GdipDrawImageRect (graphics, image, x, y, image->active_bitmap->width, image->active_bitmap->height);
}

GpStatus 
GdipDrawImage (GpGraphics *graphics, GpImage *image, float x, float y)
{
	if (!image) {
		return InvalidParameter;
	}

	return GdipDrawImageRect (graphics, image, x, y, image->active_bitmap->width, image->active_bitmap->height);
}

GpStatus
GdipDrawImageRectI (GpGraphics *graphics, GpImage *image, int x, int y, int width, int height)
{
	return GdipDrawImageRect (graphics, image, x, y, width, height);
}

GpStatus
GdipDrawImageRect (GpGraphics *graphics, GpImage *image, float x, float y, float width, float height)
{
	cairo_pattern_t *pattern;
	cairo_pattern_t *org_pattern;

	if ((graphics == NULL) || (image == NULL) || (image->type != imageBitmap)) {
		return InvalidParameter;
	}

	x = gdip_unitx_convgr (graphics, x);
	y = gdip_unity_convgr (graphics, y);
	width = gdip_unitx_convgr (graphics, width);
	height = gdip_unity_convgr (graphics, height);
	
	cairo_new_path(graphics->ct);

	if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
		GpStatus status = OutOfMemory;
		GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb (image);

		if (rgb_bitmap != NULL) {
			status = GdipDrawImageRect(graphics, rgb_bitmap, x, y, width, height);
			GdipDisposeImage(rgb_bitmap);
		}

		return status;
	}

	/* Create a surface for this bitmap if one doesn't exist */
	gdip_bitmap_ensure_surface (image);
	pattern = cairo_pattern_create_for_surface (image->surface);

	cairo_pattern_set_filter (pattern, gdip_get_cairo_filter (graphics->interpolation));

	cairo_translate (graphics->ct, x, y);

	if (width != image->active_bitmap->width || height != image->active_bitmap->height) {
		cairo_scale (graphics->ct, (double) width / image->active_bitmap->width, (double) height / image->active_bitmap->height);
	}
	
	org_pattern = cairo_get_source(graphics->ct);
	cairo_pattern_reference(org_pattern);

	cairo_set_source_surface (graphics->ct, image->surface, 0, 0);
	cairo_identity_matrix (graphics->ct);
	cairo_paint (graphics->ct);
	cairo_set_source(graphics->ct, org_pattern);	

	cairo_pattern_destroy (org_pattern);
	cairo_pattern_destroy (pattern);
	
	return Ok;
}

/* points are upper left, upper right, lower left. fourth point is extrapolated. */
GpStatus
GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, int count)
{
	float		width;
	float		height;
	cairo_pattern_t	*pattern;
	cairo_pattern_t	*org_pattern;
	GpMatrix *matrix = NULL;
	cairo_matrix_t orig_matrix;
	GpRectF tRect;
	
	if ((graphics == NULL) || (image == NULL) || (dstPoints == NULL) || (count != 3)) {
		return InvalidParameter;
	}

	if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
		GpStatus status = OutOfMemory;

		GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb (image);

		if (rgb_bitmap != NULL) {
			status = GdipDrawImagePoints (graphics, rgb_bitmap, dstPoints, count);
			GdipDisposeImage(rgb_bitmap);
		}
		return status;
	}

	cairo_new_path(graphics->ct);

	/* Create a surface for this bitmap if one doesn't exist */
	gdip_bitmap_ensure_surface (image);
	pattern = cairo_pattern_create_for_surface (image->surface);
	cairo_pattern_set_filter (pattern, gdip_get_cairo_filter (graphics->interpolation));

	tRect.X = 0; 
	tRect.Y = 0; 
	tRect.Width = image->active_bitmap->width; 
	tRect.Height = image->active_bitmap->height;
	GdipCreateMatrix3(&tRect, dstPoints, &matrix);
	
	org_pattern = cairo_get_source(graphics->ct);
	cairo_pattern_reference(org_pattern);
	
	cairo_get_matrix(graphics->ct, &orig_matrix);
	cairo_set_matrix (graphics->ct, matrix);
	cairo_set_source_surface (graphics->ct, image->surface, 0, 0);
	
	cairo_paint (graphics->ct);	
	cairo_set_source(graphics->ct, org_pattern);
	cairo_set_matrix (graphics->ct, &orig_matrix);

	GdipDeleteMatrix (matrix);
	cairo_pattern_destroy (org_pattern);
	cairo_pattern_destroy (pattern);
	
	return Ok;
}

GpStatus
GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, int count)
{
	GpPointF	points[3];
	int		i;
	
	if ((dstPoints == NULL) || (count != 3)) {
		return InvalidParameter;
	}

	for (i = 0; i < 3; i++) {
		points[i].X = dstPoints[i].X;
		points[i].Y = dstPoints[i].Y;
	}
	
	return GdipDrawImagePoints (graphics, image, points, 3);
}

GpStatus
GdipDrawImagePointRect (GpGraphics *graphics, GpImage *image,
                        float x, float y,
                        float srcx, float srcy, float srcwidth, float srcheight,
                        GpUnit srcUnit)
{
	if (image == NULL) {
		return InvalidParameter;
	}

	return GdipDrawImageRectRect (graphics, image, x, y, srcwidth, srcheight, srcx, srcy, 
		srcwidth, srcheight, srcUnit, NULL, NULL, NULL);
}

GpStatus
GdipDrawImagePointRectI (GpGraphics *graphics, GpImage *image,
                         int x, int y,
                         int srcx, int srcy, int srcwidth, int srcheight,
                         GpUnit srcUnit)
{
	return GdipDrawImagePointRect (graphics, image, x, y, srcx, srcy, srcwidth, srcheight, srcUnit);
}

GpStatus
GdipDrawImageRectRect (GpGraphics *graphics, GpImage *image,
                       float dstx, float dsty, float dstwidth, float dstheight,
                       float srcx, float srcy, float srcwidth, float srcheight,
                       GpUnit srcUnit,
                       GDIPCONST GpImageAttributes *imageAttributes,
                       DrawImageAbort callback, void *callbackData)
{
	cairo_pattern_t	*pattern;
	cairo_pattern_t	*orig;
	cairo_matrix_t	mat;
	void		*dest;
	void		*org;
	int		org_format;
	bool		allocated = FALSE;
	
	if ((graphics == NULL) || (image == NULL) || (image->type != imageBitmap)) {
		return InvalidParameter;
	}

	cairo_matrix_init (&mat, 1, 0, 0, 1, 0, 0);

	if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
		GpStatus status = OutOfMemory;
		GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb (image);

		if (rgb_bitmap != NULL) {
			status = GdipDrawImageRectRect (graphics, rgb_bitmap,
				dstx, dsty, dstwidth, dstheight,
				srcx, srcy, srcwidth, srcheight,
				srcUnit, imageAttributes, callback, callbackData);
			GdipDisposeImage (rgb_bitmap);
		}

		return status;
	}

	if (srcUnit != UnitPixel && srcUnit != UnitWorld) {
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_x, dstx, &dstx);
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_y, dsty, &dsty);
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_x, dstwidth, &dstwidth);
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_y, dstheight, &dstheight);
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_x, srcx, &srcx);
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_y, srcy, &srcy);
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_x, srcwidth, &dstwidth);
		gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->type, graphics->dpi_y, srcheight, &srcheight);
	}

	org = dest = image->active_bitmap->scan0; 
	org_format = image->active_bitmap->pixel_format;
	gdip_process_bitmap_attributes (image, &dest, (GpImageAttributes *) imageAttributes, &allocated);

	/*  If allocated is true we have a newly allocated and altered Scan0 in dest */
	if (allocated) {
		image->active_bitmap->scan0 = dest;
	}
	
	/* Drop the existing surface if attributes are being applied since the surface might be out-of-date */
	if ((image->surface != NULL) && (imageAttributes != NULL))  {
		cairo_surface_destroy (image->surface);
		image->surface = NULL;
	}

	if (imageAttributes && imageAttributes->wrapmode != WrapModeClamp) {
		float		img_width;
		float		img_height;
		float		posx;
		float		posy;
		bool		flipXOn;
		bool		flipYOn;
		bool		flipX;
		bool		flipY;
		GpBitmap	*cur_image;
		GpBitmap	*imgflipX;
		GpBitmap	*imgflipY;
		GpBitmap	*imgflipXY;

		img_width = image->active_bitmap->width *  (dstwidth / srcwidth);
		img_height = image->active_bitmap->height * (dstheight / srcheight);
		flipXOn = (imageAttributes->wrapmode == WrapModeTileFlipX) ? TRUE: FALSE;
		flipYOn = (imageAttributes->wrapmode == WrapModeTileFlipY) ? TRUE: FALSE;
		flipX = FALSE;
		flipY = FALSE;
		imgflipX = NULL;
		imgflipY = NULL;
		imgflipXY = NULL;
		
		if (imageAttributes->wrapmode == WrapModeTileFlipXY) {
			flipXOn = flipYOn = TRUE;
		}
		
		if (flipXOn) {			
			/* We're ok just cloning the bitmap, we don't need the image data
			 * and we destroy it before we leave this function */
			gdip_bitmap_clone (image, &imgflipX);
			gdip_flip_x (imgflipX);	
			gdip_bitmap_ensure_surface (imgflipX);			
		}
		
		if (flipYOn) {			
			gdip_bitmap_clone (image, &imgflipY);
			gdip_flip_y (imgflipY);	
			gdip_bitmap_ensure_surface (imgflipY);			
		}
		
		if (flipXOn && flipYOn) {			
			gdip_bitmap_clone (image, &imgflipXY);
			gdip_flip_x (imgflipXY);	
			gdip_flip_y (imgflipXY);	
			gdip_bitmap_ensure_surface (imgflipXY);			
		}
		
		gdip_bitmap_ensure_surface (image);

		for (posy = 0; posy < dstheight; posy += img_height) {
			for (posx = 0; posx < dstwidth; posx += img_width) {
				if (flipX && flipY) {
					cur_image = imgflipXY;
				} else {
					if (flipX) {
						cur_image = imgflipX;
					} else {
						if (flipY) {
							cur_image = imgflipY;
						} else {
							cur_image = image;
						}
					}
				}

				cairo_matrix_scale (&mat, srcwidth / dstwidth, srcheight / dstheight);
				cairo_matrix_translate (&mat, srcx - (dstx + posx), srcy - (dsty + posy));

				pattern = cairo_pattern_create_for_surface(cur_image->surface);
				cairo_pattern_set_matrix (pattern, &mat);

				orig = cairo_get_source(graphics->ct);
				cairo_pattern_reference(orig);

				cairo_set_source(graphics->ct, pattern);
				cairo_rectangle (graphics->ct, dstx + posx, dsty + posy, img_width, img_height);
				cairo_fill (graphics->ct);

				cairo_set_source(graphics->ct, orig);

				cairo_matrix_init_identity (&mat);
				cairo_pattern_set_matrix (pattern, &mat);

				cairo_pattern_destroy(orig);
				cairo_pattern_destroy(pattern);

				if (flipXOn) {
					flipX = !flipX;
				}
			
			}
					
			if (flipYOn) {
				flipY = !flipY; 
			}
		}	
		
		if (imgflipX) {
			GdipDisposeImage ((GpImage *) imgflipX);
		}
			
		if (imgflipY) {
			GdipDisposeImage ((GpImage *) imgflipY);
		}

		if (imgflipXY) {
			GdipDisposeImage ((GpImage *) imgflipXY);
		}
	} else {
		cairo_pattern_t *filter;

		gdip_bitmap_ensure_surface (image);
		filter = cairo_pattern_create_for_surface (image->surface);
		cairo_pattern_set_filter (filter, gdip_get_cairo_filter (graphics->interpolation));
		
		cairo_matrix_scale (&mat, srcwidth / dstwidth, srcheight / dstheight);
		cairo_matrix_translate (&mat, srcx - dstx, srcy - dsty);

		pattern = cairo_pattern_create_for_surface(image->surface);
		cairo_pattern_set_matrix (pattern, &mat);

		orig = cairo_get_source(graphics->ct);
		cairo_pattern_reference(orig);

		cairo_set_source(graphics->ct, pattern);
		cairo_rectangle (graphics->ct, dstx, dsty, dstwidth, dstheight);
		cairo_fill (graphics->ct);
		
		cairo_set_source(graphics->ct, orig);
		cairo_pattern_destroy (orig);

		cairo_matrix_init_identity (&mat);
		cairo_pattern_set_matrix (pattern, &mat);
		cairo_pattern_destroy (pattern);
		cairo_pattern_destroy (filter);
	}

	/* The current surface is no longer valid if we had attributes applied */
	if (image->surface && imageAttributes != NULL)  {
		cairo_surface_destroy (image->surface);
		image->surface = NULL;
	}

	if (allocated) {
		image->active_bitmap->scan0 = org;
		image->active_bitmap->pixel_format = org_format;
		GdipFree (dest);
	}
	
	return Ok;
}

GpStatus
GdipDrawImageRectRectI (GpGraphics *graphics, GpImage *image,
                        int dstx, int dsty, int dstwidth, int dstheight,
                        int srcx, int srcy, int srcwidth, int srcheight,
                        GpUnit srcUnit,
                        GDIPCONST GpImageAttributes *imageAttributes,
                        DrawImageAbort callback, void *callbackData)
{
	return GdipDrawImageRectRect (graphics, image, dstx, dsty, dstwidth, dstheight,
					srcx, srcy, srcwidth, srcheight,
					srcUnit,
					imageAttributes, callback, callbackData);
}

GpStatus
GdipDrawImagePointsRect (GpGraphics *graphics, GpImage *image,
                         GDIPCONST GpPointF *points, int count,
                         float srcx, float srcy, float srcwidth, float srcheight,
                         GpUnit srcUnit,
                         GDIPCONST GpImageAttributes *imageAttributes,
                         DrawImageAbort callback, void *callbackData)
{
	return NotImplemented; /* GdipDrawImagePointsRect */
}

GpStatus
GdipDrawImagePointsRectI (GpGraphics *graphics, GpImage *image,
                          GDIPCONST GpPoint *points, int count,
                          int srcx, int srcy, int srcwidth, int srcheight,
                          GpUnit srcUnit,
                          GDIPCONST GpImageAttributes *imageAttributes,
                          DrawImageAbort callback, void *callbackData)
{
	return NotImplemented; /* GdipDrawImagePointsRectI */
}

/*
 * These two will never be implemented, as 'stream' is a COM IStream
 */
GpStatus
GdipLoadImageFromStream (void *stream, GpImage **image)
{
	return NotImplemented; /* GdipLoadImageFromStream - not supported */
}

GpStatus
GdipSaveImageToStream (GpImage *image, void *stream, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
	return NotImplemented; /* GdipSaveImageToStream - not supported */
}

GpStatus 
GdipLoadImageFromFile (GDIPCONST WCHAR *file, GpImage **image)
{
	FILE		*fp = 0;
	GpImage		*result = 0;
	GpStatus	status = 0;
	ImageFormat	format;
	char		*file_name = NULL;
	char		format_peek[10];
	int		format_peek_sz;
	
	if ((image == NULL) || (file == NULL)) {
		return InvalidParameter;
	}
	
	file_name = (char *) ucs2_to_utf8 ((const gunichar2 *)file, -1);
	if (file_name == NULL) {
		*image = NULL;
		return InvalidParameter;
	}
	
	fp = fopen(file_name, "rb");
	GdipFree (file_name);
	if (fp == NULL) {
		return InvalidParameter;
	}
	
	format_peek_sz = fread (format_peek, 1, 10, fp);
	format = get_image_format (format_peek, format_peek_sz);
	fseek (fp, 0, SEEK_SET);
	
	switch (format) {
		case BMP: {
			status = gdip_load_bmp_image_from_file (fp, &result);
			if (result != NULL) {
				result->image_format = BMP;
			}
			break;
		}

		case TIF: {
			status = gdip_load_tiff_image_from_file (fp, &result);
			if (result != NULL) {
				result->image_format = TIF;
			}
			break;
		}

		case GIF: {
			status = gdip_load_gif_image_from_file (fp, &result);
			if (result != NULL) {
				result->image_format = GIF;
			}
			break;
		}

		case PNG: {
			status = gdip_load_png_image_from_file (fp, &result);
			if (result != NULL) {
				result->image_format = PNG;
			}
			break;
		}

		case JPEG: {
			status = gdip_load_jpeg_image_from_file (fp, &result);
			if (result != NULL) {
				result->image_format = JPEG;
			}
			break;
		}

		case EXIF:
		case WMF:
		case EMF:
		case ICON: {
			status = NotImplemented;
			break;
		}

		default: {
			status = OutOfMemory;
			break;
		}
	}
	
	fclose (fp);
	
	*image = result;
	if (status != Ok) {
		*image = NULL;
	} else if (result && !result->active_bitmap) {
		/* If the codec didn't set the active bitmap we will */
		gdip_bitmap_setactive(result, NULL, 0);
	}
	
	return status;
}

ImageFormat 
gdip_get_imageformat_from_codec_clsid (CLSID *encoderCLSID)
{
	int numEncoders, size, cnt;
    	ImageCodecInfo *encoders, *encoder;

    	GdipGetImageEncodersSize (&numEncoders, &size);

    	if (numEncoders == 0)
        	return INVALID;

    	encoders = GdipAlloc (size);
    
    	GdipGetImageEncoders (numEncoders, size, encoders);

    	for (cnt = 0, encoder = encoders; cnt < numEncoders; encoder++) {
	       	if (memcmp (&encoder->Clsid, encoderCLSID, sizeof (GUID)) == 0) {
			ImageFormat ifmt = gdip_image_format_for_format_guid (&encoder->FormatID);
            		GdipFree (encoders);
			return ifmt;
        	}
    	}

    	GdipFree (encoders);
    	return INVALID;
}

GpStatus
GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
	FILE *fp = 0;
	GpStatus status = 0;
	char *file_name;
	ImageFormat format;
	
	if (!image || !file || !encoderCLSID)
		return InvalidParameter;

	if (image->type != imageBitmap)
		return InvalidParameter;
	
	format = gdip_get_imageformat_from_codec_clsid ( (CLSID *)encoderCLSID);
	if (format == INVALID)
		return UnknownImageFormat;
	
	file_name = (char *) ucs2_to_utf8 ((const gunichar2 *)file, -1);
	if (file_name == NULL)
		return InvalidParameter;
	
	if (format == GIF) { /* gif library has to open the file itself*/
		status = gdip_save_gif_image_to_file ((unsigned char *)file_name, image);
		GdipFree (file_name);
		return status;
	} else if (format == TIF) { 
		/* tif library has to open the file itself or seeking will fail when saving multi-page images*/
		status = gdip_save_tiff_image_to_file ((unsigned char *)file_name, image, params);
		GdipFree (file_name);
		return status;
	}
	
	if ((fp = fopen(file_name, "wb")) == NULL) {
		GdipFree (file_name);
		return GenericError;
	}
		
	GdipFree (file_name);
	
	switch (format) {
		case BMP:
			status = gdip_save_bmp_image_to_file (fp, image);
			break;
		case PNG:
			status = gdip_save_png_image_to_file (fp, image, params);
			break;
		case JPEG:
			status = gdip_save_jpeg_image_to_file (fp, image, params);
			break;
		default:
			status = NotImplemented;
			break;
	}
	
	fclose (fp);
	
	return status;
}

GpStatus 
GdipGetImageBounds (GpImage *image, GpRectF *rect, GpUnit *unit)
{
	if ((image == NULL) || (rect == NULL) || (unit == NULL)) {
		return InvalidParameter;
	}

	rect->X = 0;
	rect->Y = 0;
	rect->Height = image->active_bitmap->height;
	rect->Width = image->active_bitmap->width;
	*unit = UnitPixel;

	return Ok;
}

GpStatus 
GdipGetImageDimension (GpImage *image, float *width, float *height)
{
	if ((image == NULL) || (width == NULL) || (height == NULL)) {
		return InvalidParameter;
	}

	*width = image->active_bitmap->width;
	*height = image->active_bitmap->height;

	return Ok;
}

GpStatus 
GdipGetImageType (GpImage *image, ImageType *type)
{
	if ((image == NULL) || (type == NULL)) {
		return InvalidParameter;
	}

	*type = image->type;
	return Ok;
}

GpStatus 
GdipGetImageWidth (GpImage *image, UINT *width)
{
	if ((image == NULL) || (width == NULL)) {
		return InvalidParameter;
	}

	*width = image->active_bitmap->width;
	return Ok;
}

GpStatus 
GdipGetImageHeight (GpImage *image, UINT *height)
{
	if ((image == NULL) || (height == NULL)) {
		return InvalidParameter;
	}

	*height = image->active_bitmap->height;
	return Ok;
}

GpStatus 
GdipGetImageHorizontalResolution (GpImage *image, float *resolution)
{
	if ((image == NULL) || (resolution == NULL)) {
		return InvalidParameter;
	}
	
	*resolution = image->active_bitmap->dpi_horz;
	return Ok;
}

GpStatus 
GdipGetImageVerticalResolution (GpImage *image, float *resolution)
{
	if ((image == NULL) || (resolution == NULL)) {
		return InvalidParameter;
	}

	*resolution = image->active_bitmap->dpi_vert;
	return Ok;
}

GpStatus 
GdipGetImageFlags (GpImage *image, UINT *flags)
{
	if ((image == NULL) || (flags == NULL)) {
		return InvalidParameter;
	}

	*flags = image->active_bitmap->image_flags;
	return Ok;
}

GpStatus GdipGetImageRawFormat (GpImage *image, GUID *format)
{
	if ((image == NULL) || (format == NULL)) {
		return InvalidParameter;
	}
	
	switch (image->image_format) {
        	case BMP:
	        	memcpy(format, &gdip_bmp_image_format_guid, 16);
			return Ok;

		case TIF:
            		memcpy(format, &gdip_tif_image_format_guid, 16);
			return Ok;

		case GIF:
	        	memcpy(format, &gdip_gif_image_format_guid, 16);
			return Ok;

		case PNG:
	        	memcpy(format, &gdip_png_image_format_guid, 16);
			return Ok;

		case JPEG:
            		memcpy(format, &gdip_jpg_image_format_guid, 16);
			return Ok;

		case EXIF:
	        	memcpy(format, &gdip_exif_image_format_guid, 16);
			return Ok;

		case WMF:
            		memcpy(format, &gdip_wmf_image_format_guid, 16);
			return Ok;

		case EMF:
	        	memcpy(format, &gdip_emf_image_format_guid, 16);
			return Ok;

		case MEMBMP:
	        	memcpy(format, &gdip_membmp_image_format_guid, 16);
			return Ok;

        	default:
            		return InvalidParameter;
    	}
}

GpStatus 
GdipGetImagePixelFormat (GpImage *image, PixelFormat *format)
{
	if ((image == NULL) || (format == NULL)) {
		return InvalidParameter;
	}

	*format = image->active_bitmap->pixel_format;
	return Ok;
}

GpStatus 
GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count)
{
	if ((image == NULL) || (count == NULL)) {
		return InvalidParameter;
	}

	*count = image->num_of_frames;
	return Ok;
}

GpStatus 
GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count)
{
	int	i;
	int	countReturn;

	if ((image == NULL) || (dimensionGUID == NULL)) {
		return InvalidParameter;
	}

	countReturn = image->num_of_frames;
	
	if (countReturn > count) {
		countReturn = count;
	}

	for (i = 0; i < countReturn; i++) {
		dimensionGUID[i] = image->frames[i].frame_dimension;
	}
	return Ok;
}

GpStatus
GdipImageGetFrameCount(GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count)
{
	int i;

	if ((image == NULL) || (dimensionGUID == NULL) || (count == NULL)) {
		return InvalidParameter;
	}

	for (i = 0; i < image->num_of_frames; i++){
		if (memcmp(dimensionGUID, &(image->frames[i].frame_dimension), sizeof(CLSID)) == 0) {
			*count = image->frames[i].count;
			return Ok;
		}
	}

        return InvalidParameter;
}

GpStatus
GdipImageSelectActiveFrame(GpImage *image, GDIPCONST GUID *dimensionGUID, UINT index)
{
        if ((image == NULL) || (dimensionGUID == NULL) || (index <0)) {
                return InvalidParameter;
	}

	return gdip_bitmap_setactive(image, dimensionGUID, index);
}

void copy_pixel (BYTE *src, BYTE *trg, int size)
{
	memcpy (trg, src, size);
}

static GpStatus
gdip_flip_x (GpImage *image)
{
	BYTE	*src;
	BYTE	*line;
	int	stride;
	int	width;
	int	height;
	int	pixel_size;
	int	i;
	int	j;
	
	stride = image->active_bitmap->stride;
	width = image->active_bitmap->width;
	height = image->active_bitmap->height;
	pixel_size = gdip_get_pixel_format_components (image->active_bitmap->pixel_format) * gdip_get_pixel_format_depth (image->active_bitmap->pixel_format) / 8;
	line = GdipAlloc (stride);
	src = (BYTE *) image->active_bitmap->scan0;

	if (line == NULL) {
		return OutOfMemory;
	}
	
	for (i = 0; i < height; i++, src += stride) {
		memcpy (line, src, stride);	/* Save original line */

		for  (j = 0; j < width; j++) {
			copy_pixel(&line[(width - j - 1) * pixel_size], &src[j * pixel_size], pixel_size);
		}
	}
	
	GdipFree (line);

	return Ok;
}

static GpStatus
gdip_flip_y (GpImage *image)
{
	BYTE	*src;
	BYTE	*trg;
	BYTE	*line;
	int	stride;
	int	height;
	int	i;
	
	stride = image->active_bitmap->stride;
	height = image->active_bitmap->height;
	line = GdipAlloc (stride);
	src = (BYTE *) image->active_bitmap->scan0;
	trg = (BYTE *) image->active_bitmap->scan0;
	trg +=  (height-1) * stride;

	if (line == NULL) {
		return OutOfMemory;
	}
	
	for (i = 0; i < (height /2); i++, src += stride, trg -= stride) {			
		memcpy (line, trg, stride);	/* Save target line*/
		memcpy (trg, src, stride);	/* Copy src to trg*/
		memcpy (src, line, stride);	/* Copy trg to src*/
	}
	
	GdipFree (line);

	return Ok;
}

static GpStatus
gdip_rotate_orthogonal_flip_x (GpImage *image, int angle, BOOL flip_x)
{
	BYTE	*rotated;
	BYTE	*source;
	BYTE	*target;
	int	x, y;
	int	source_stride, source_height, source_width, source_pixel_delta, source_interscan_delta;
	int	target_stride, target_height, target_width, target_pixel_delta, target_interscan_delta;
	int	initial_source_offset, initial_target_offset;
	int	components;
	int	depth;
	int	pixel_size;

	components = gdip_get_pixel_format_components (image->active_bitmap->pixel_format);
	depth = gdip_get_pixel_format_depth (image->active_bitmap->pixel_format);
	pixel_size = components * depth / 8;

	source_stride = image->active_bitmap->stride;
	source_width = image->active_bitmap->width;
	source_height = image->active_bitmap->height;

	source_pixel_delta = pixel_size;
	source_interscan_delta = source_stride - source_width * pixel_size;
	initial_source_offset = 0;

	if (angle == 180) {
		target_width = source_width;
		target_height = source_height;
	} else {
		target_height = source_width; /* swap width & height here */
		target_width = source_height;
	}

	target_stride = target_width * pixel_size;
	target_stride = (target_stride + 3) & ~3;

	switch (angle) {
		case 90: {
			target_pixel_delta = target_stride;
			if (flip_x) {
				target_interscan_delta = +pixel_size - target_stride * target_height;
				initial_target_offset = 0;
			} else {
				target_interscan_delta = -pixel_size - target_stride * target_height;
				initial_target_offset = (target_width - 1) * pixel_size;
			}
			break;
		}

		case 180: {
			if (flip_x) {
				target_pixel_delta = +pixel_size;
				target_interscan_delta = -target_width * pixel_size - target_stride; 
				initial_target_offset = (target_height - 1) * target_stride;
			} else {
				target_pixel_delta = -pixel_size;
				target_interscan_delta = +target_width * pixel_size - target_stride;
				initial_target_offset = (target_height - 1) * target_stride + (target_width - 1) * pixel_size;
			}
			break;
		}

		case 270: {
			target_pixel_delta = -target_stride;
			if (flip_x) {
				target_interscan_delta = target_stride * target_height - pixel_size;
				initial_target_offset = (target_height - 1) * target_stride + (target_width - 1) * pixel_size;
			} else {
				target_interscan_delta = target_stride * target_height + pixel_size;
				initial_target_offset = (target_height - 1) * target_stride;
			}
			break;
		}

		default: {
			if (flip_x) {
				return gdip_flip_x (image);
			} else {
				return Ok;
			}
		}
	}

	rotated = GdipAlloc (target_height * target_stride);

	if (rotated == NULL) {
		return OutOfMemory;
	}

	source = initial_source_offset + (BYTE *) image->active_bitmap->scan0;
	target = initial_target_offset + rotated;

	for (y = 0; y < source_height;
             y++,
             source += source_interscan_delta,
             target += target_interscan_delta) {
		for (x = 0; x < source_width;
                     x++,
                     source += source_pixel_delta,
                     target += target_pixel_delta) {
				copy_pixel (source, target, pixel_size);
		}
	}

	image->active_bitmap->stride = target_stride;
	image->active_bitmap->height = target_height;
	image->active_bitmap->width = target_width;

	if ((image->active_bitmap->reserved & GBD_OWN_SCAN0) != 0) {
		GdipFree(image->active_bitmap->scan0);
	}

	image->active_bitmap->scan0 = rotated;
	image->active_bitmap->reserved |= GBD_OWN_SCAN0;	

	if (image->surface != NULL) {
		cairo_surface_destroy (image->surface);
		image->surface = NULL;
	}

	return Ok;
}

static GpStatus
gdip_rotate_flip_packed_indexed (GpImage *image, PixelFormat pixel_format, int angle, BOOL flip_x)
{
	BYTE		*rotated;
	StreamingState	scan[8];
	BOOL		scan_valid[8];
	int		x;
	int		y;
	int		i;
	int		bits_per_pixel;
	int		pixels_per_byte;
	int		source_width;
	int		source_height;
	BOOL		aspect_inversion;
	int		target_width;
	int		target_height;
	int		target_scan_size;
	int		target_stride;

	bits_per_pixel = gdip_get_pixel_format_depth (pixel_format);
	pixels_per_byte = 8 / bits_per_pixel;

	source_width = image->active_bitmap->width;
	source_height = image->active_bitmap->height;

	/* Swap the width & height if needed */
	aspect_inversion = ((angle % 180) != 0);

	target_width = aspect_inversion ? source_height : source_width;
	target_height = aspect_inversion ? source_width : source_height;

	target_scan_size = (target_width + pixels_per_byte - 1) / pixels_per_byte;
	target_stride = (target_scan_size + 3) & ~3;

	if ((angle == 180) && flip_x) {
		return gdip_flip_y(image);
	}

	rotated = GdipAlloc (target_height * target_stride);
	if (rotated == NULL) {
		return OutOfMemory;
	}

	if (aspect_inversion == FALSE) {
		int x_alignment = ((source_width - 1) % pixels_per_byte) + 1 - pixels_per_byte;
		int x_step = pixels_per_byte;

		StreamingState stream;

		BOOL target_starts_at_bottom = (angle == 180);

		int target_x_offset = target_scan_size - 1;
		int target_y_offset = target_starts_at_bottom ? (target_height - 1) * target_stride : 0;
		int target_y_offset_delta = target_starts_at_bottom ? -target_stride : +target_stride;

		Status status = gdip_init_pixel_stream (&stream, image->active_bitmap, 0, 0, image->active_bitmap->width, image->active_bitmap->height);

		if (status != Ok) {
			GdipFree (rotated);
			return status;
		}

		for (y = 0; y < source_height; y++, target_y_offset += target_y_offset_delta) {
			BYTE *target = rotated + target_x_offset + target_y_offset;

			for (x = x_alignment; x < source_width; x += x_step) {
				int byte = 0;
				for (i = 0; i < pixels_per_byte; i++) {
					int index = x + i;

					if ((index >= 0) && (index < source_width)) {
						byte |= (gdip_pixel_stream_get_next (&stream) << 8);
					}

					byte >>= bits_per_pixel;
				}

				*target = byte;
				target--;
			}
		}
	} else {
		int y_alignment = ((source_height - 1) % pixels_per_byte) + 1 - pixels_per_byte;
		int y_step = pixels_per_byte;
		int byte_column = 0;

		int target_delta;

		BOOL target_starts_at_bottom = (angle == 270);
		BOOL target_starts_at_left = (target_starts_at_bottom ^ flip_x);

		if (target_starts_at_left) {
			y_alignment = 0;
		}

		if (target_starts_at_bottom) {
			target_delta = -target_stride;
		} else {
			target_delta = +target_stride;
		}

		for (y = y_alignment; y < source_height; y += y_step) {
			BYTE *target;

			for (i=0; i < pixels_per_byte; i++) {
				int scan_index = y + i;

				scan_valid[i] = ((scan_index >= 0) && (scan_index < source_height));

				if (scan_valid[i]) {
					Status status = gdip_init_pixel_stream (&scan[i], image->active_bitmap, 0, scan_index, source_width, 1);

					if (status != Ok) {
						GdipFree (rotated);
						return status;
					}
				}
			}

			target = rotated;

			if (target_starts_at_left) {
				target += byte_column;
			} else {
				target += target_scan_size - byte_column - 1;
			}

			if (target_starts_at_bottom) {
				target += (target_height - 1) * target_stride;
			}

			for (x = 0; x < source_width; x++) {
				int byte = 0;

				if (target_starts_at_left) {
					for (i=0; i < pixels_per_byte; i++) {
						byte <<= bits_per_pixel;
						if (scan_valid[i])
							byte |= gdip_pixel_stream_get_next (&scan[i]);
					}
				} else {
					for (i = pixels_per_byte - 1; i >= 0; i--) {
						byte <<= bits_per_pixel;
						if (scan_valid[i])
							byte |= gdip_pixel_stream_get_next (&scan[i]);
					}
				}

				*target = byte;
				target += target_delta;
			}

			byte_column++;
		}
	}

	image->active_bitmap->stride = target_stride;
	image->active_bitmap->height = target_height;
	image->active_bitmap->width = target_width;

	if ((image->active_bitmap->reserved & GBD_OWN_SCAN0) != 0) {
		GdipFree(image->active_bitmap->scan0);
	}

	image->active_bitmap->scan0 = rotated;
	image->active_bitmap->reserved |= GBD_OWN_SCAN0;	

	/* It shouldn't be possible for an indexed image to have one,
	 * but if it does, it needs to be killed. */
	if (image->surface != NULL) {
		cairo_surface_destroy (image->surface);
		image->surface = NULL;
	}

	return Ok;
}

GpStatus 
GdipImageRotateFlip (GpImage *image, RotateFlipType type)
{
	int	angle;
	BOOL	flip_x;

	if (image == NULL) {
		return InvalidParameter;
	}

	angle = flip_x = 0;

	switch (type) {
                case RotateNoneFlipNone:/* equivalent to Rotate180FlipXY */
			return Ok;

		case Rotate90FlipNone:	/* equivalent to Rotate270FlipXY */
			angle = 90;
			break;

		case RotateNoneFlipXY:	/* equivalent to Rotate180FlipNone */
			angle = 180;
			break;

		case Rotate90FlipXY:	/* equivalent to Rotate270FlipNone */
			angle = 270;
			break;

		case RotateNoneFlipX:	/* equivalent to Rotate180FlipY */
			flip_x = TRUE;
			break;

		case Rotate90FlipX:	/* equivalent to Rotate270FlipY */
			angle = 90;
			flip_x = TRUE;
			break;

		case RotateNoneFlipY:	/* equivalent to Rotate180FlipX */
			return gdip_flip_y (image);

		case Rotate90FlipY:	/* equivalent to Rotate270FlipX */
			angle = 270;
			flip_x = TRUE;
			break;

		default:
			return NotImplemented;
	}	
	
	if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format) && (gdip_get_pixel_format_depth (image->active_bitmap->pixel_format) < 8)) {
		return gdip_rotate_flip_packed_indexed (image, image->active_bitmap->pixel_format, angle, flip_x);
	} else {
		return gdip_rotate_orthogonal_flip_x (image, angle, flip_x);
	}
}

GpStatus 
GdipGetImagePalette (GpImage *image, ColorPalette *palette, int size)
{
	int palette_entries;
	int bytes_needed;

	if ((image == NULL) || (palette == NULL) || (image->active_bitmap->palette == NULL)) {
		return InvalidParameter;
	}

	palette_entries = image->active_bitmap->palette->Count;

	if ((image->type == imageBitmap) && (image->active_bitmap->pixel_format == Format4bppIndexed)) {
		palette_entries = 16;
	}

	bytes_needed = palette_entries * sizeof(ARGB) + sizeof(ColorPalette) - sizeof(ARGB);

	if (bytes_needed > size) {
		return InvalidParameter;
	}

	memcpy(palette, image->active_bitmap->palette, bytes_needed);
	return Ok;
}

GpStatus 
GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette)
{
	int entries_to_copy;

	if ((image == NULL) || (palette == NULL) || (image->active_bitmap->palette == NULL)) {
		return InvalidParameter;
	}

	entries_to_copy = image->active_bitmap->palette->Count;
	if (entries_to_copy > palette->Count) {
		entries_to_copy = palette->Count;
	}

	memcpy(image->active_bitmap->palette->Entries, palette->Entries, entries_to_copy * sizeof(ARGB));
	return Ok;
}

GpStatus 
GdipGetImagePaletteSize (GpImage *image, int* size)
{
        int palette_entries;

        if ((image == NULL) || (size == NULL) || (image->active_bitmap->palette == NULL)) {
                return InvalidParameter;
	}

        palette_entries = image->active_bitmap->palette->Count;

        if ((image->type == imageBitmap) && (image->active_bitmap->pixel_format == Format4bppIndexed)) {
                palette_entries = 16;
	}

        *size = palette_entries * sizeof(ARGB) + sizeof(ColorPalette) - sizeof(ARGB);
	return Ok;
}

GpStatus 
GdipGetPropertyCount (GpImage *image, UINT *propertyNumber)
{
	if ((image == NULL) || (propertyNumber == NULL)) {
		return InvalidParameter;
	}

	*propertyNumber = image->active_bitmap->property_count;

	return Ok;
}

GpStatus 
GdipGetPropertyIdList (GpImage *image, UINT propertyNumber, PROPID *list)
{
	int	i;

	if ((list == NULL) || (propertyNumber != image->active_bitmap->property_count)) {
		return InvalidParameter;
	}
	for (i = 0; i < propertyNumber; i++) {
		list[i] = image->active_bitmap->property[i].id;
	}

	return Ok;
}

GpStatus 
GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size)
{
	int	index;

	if ((image == NULL) || (size == NULL)) {
		return InvalidParameter;
	}

	if (gdip_bitmapdata_property_find_id(image->active_bitmap, propID, &index) != Ok) {
		return PropertyNotFound;
	}

	*size = sizeof(PropertyItem) + image->active_bitmap->property[index].length;

	return Ok;
}

GpStatus 
GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem *buffer)
{
	int	index;
	int	size_needed;

	if ((image == NULL) || (buffer == NULL)) {
		return InvalidParameter;
	}

	if (gdip_bitmapdata_property_find_id(image->active_bitmap, propID, &index) != Ok) {
		return PropertyNotFound;
	}

	if (size != (sizeof(PropertyItem) + image->active_bitmap->property[index].length)) {
		return InvalidParameter;
	}

	memcpy(buffer, &image->active_bitmap->property[index], sizeof(PropertyItem));
	buffer->value = (unsigned char *)buffer + sizeof(PropertyItem);
	memcpy(buffer->value, image->active_bitmap->property[index].value, buffer->length);

	return Ok;
}

GpStatus 
GdipGetPropertySize (GpImage *image, UINT *totalBufferSize, UINT *numProperties)
{
	UINT	size;
	int	i;

	if ((image == NULL) || (totalBufferSize == NULL) || (numProperties == NULL)) {
		return InvalidParameter;
	}

	*numProperties = image->active_bitmap->property_count;

	size = image->active_bitmap->property_count * sizeof(PropertyItem);
	for (i = 0; i < image->active_bitmap->property_count; i++) {
		size += image->active_bitmap->property[i].length;
	}

	*totalBufferSize = size;
	return Ok;
}

GpStatus 
GdipRemovePropertyItem (GpImage *image, PROPID propID)
{
	if (image == NULL) {
		return InvalidParameter;
	}

	return gdip_bitmapdata_property_remove_id(image->active_bitmap, propID);
}

GpStatus 
GdipSetPropertyItem(GpImage *image, GDIPCONST PropertyItem *item)
{
	int	i;

	if ((image == NULL) || (item == NULL)) {
		return InvalidParameter;
	}

	switch(image->image_format) {
		case TIF:
		case JPEG:
		case PNG: {
			break;
		}

		default: {
			return PropertyNotSupported;
		}
	}

	/* Check if it exists and we need to replace */
	if (gdip_bitmapdata_property_find_id(image->active_bitmap, item->id, &i) != Ok) {
		/* We're adding a new item */
		return gdip_bitmapdata_property_add(image->active_bitmap, item->id, item->length, item->type, item->value);
	}

	/* We're replacing an existing item */
	if (item->length > image->active_bitmap->property[i].length) {
		/* We need to allocate more space for our value */
		if (image->active_bitmap->property[i].value != NULL) {
			GdipFree(image->active_bitmap->property[i].value);
		}

		image->active_bitmap->property[i].value = GdipAlloc(item->length);
		if (image->active_bitmap->property[i].value == NULL) {
			/* We already freed the old value, remove the entry */
			gdip_bitmapdata_property_remove_index(image->active_bitmap, i);
			return OutOfMemory;
		}
	} else if (item->length == 0) {
		/* Just drop the old value */
		if (image->active_bitmap->property[i].value != NULL) {
			GdipFree(image->active_bitmap->property[i].value);
			image->active_bitmap->property[i].value = NULL;
		}
	}

	image->active_bitmap->property[i].id = item->id;
	image->active_bitmap->property[i].length = item->length;
	image->active_bitmap->property[i].type = item->type;
	if (item->length > 0) {
		memcpy(image->active_bitmap->property[i].value, item->value, item->length);
	}

	return Ok;
}

void
gdip_image_clone (GpImage* image, GpImage** clonedImage)
{
	gdip_bitmap_clone(image, clonedImage);
}

GpStatus
GdipCloneImage(GpImage *image, GpImage **cloneImage)
{
	if ((image == NULL) || (cloneImage == NULL)) {
		return InvalidParameter;
	}

	switch (image->type){
		case imageBitmap: {
                	gdip_bitmap_clone (image, cloneImage);
			gdip_bitmap_setactive(*cloneImage, NULL, 0);
			return Ok;
		}

		case imageMetafile: {
        	        return NotImplemented; /* GdipCloneImage - imageMetafile */
		}

        	case imageUndefined: {
			return Ok;
		}

		default: {
			return Ok;
		}
	}
        
	return Ok;
}

/*  FIXME - this function makes me uneasy. */
ImageFormat 
get_image_format (char *sig_read, size_t size_read)
{
	int	index;
	char	png[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, '\0'};
	char	*signature[]  = { "BM", "MM", "II", "GIF", png, "\xff\xd8", "\xff\xd8\xff\xe1", "", "", ""};

	if (size_read < 10) {
		return INVALID;
	}

	for (index = 0; index < size_read; index ++) {
		if ((signature[index][0] == sig_read[0]) && (signature[index][1] == sig_read[1])) {
			switch (index) {
				case 0 :	
					return BMP;

				case 1:
				case 2:
					return TIF;

				case 3:
					if (signature[index][2] == sig_read[2]) {
						return GIF;
					} else {
						return INVALID;
					}
				case 4:
					if (strncmp(signature[index], sig_read, 8) == 0) {
						return PNG;						
					} else {
						return INVALID;
					}

				case 5:		
				case 6: if (strncmp(sig_read + 2, "\xff\e1", 2) == 0) {
						if (strncmp(sig_read + 6, "Exif", 4) == 0) {
							return EXIF;
						}
					}
					return JPEG;

				case 7:
				case 8:
				case 9:
				default:
					return INVALID;
			}	
		}
	}
	return INVALID;
} 

int 
gdip_get_pixel_format_bpp (PixelFormat pixfmt)
{
	return gdip_get_pixel_format_depth (pixfmt) * gdip_get_pixel_format_components (pixfmt);
}

int
gdip_get_pixel_format_depth(PixelFormat pixfmt)
{
	switch (pixfmt) {
		case Format16bppArgb1555:
		case Format16bppGrayScale:
		case Format16bppRgb555:
		case Format16bppRgb565:
		case Format24bppRgb:
		case Format32bppArgb:
		case Format32bppPArgb:
		case Format32bppRgb:
			return 8;		/* FIXME - shouldn't this be 32? - pdb */

		case Format48bppRgb:
		case Format64bppArgb:
		case Format64bppPArgb:
			return 16;

		case Format8bppIndexed:
			return 8;

		case Format4bppIndexed:
			return 4;

		case Format1bppIndexed:
			return 1;

		default:
			break;
	}

	return 0;
}

int
gdip_get_pixel_format_components(PixelFormat pixfmt)
{
	switch (pixfmt) {
		case Format16bppArgb1555:
		case Format32bppArgb:
		case Format32bppPArgb:
		case Format64bppArgb:
		case Format64bppPArgb:
		case Format32bppRgb:	/* Cairo uses four bytes for 32BPP*/
		case Format24bppRgb:
			return 4;

		case Format16bppRgb555:
		case Format16bppRgb565: 
		case Format48bppRgb:
			return 3;

		case Format16bppGrayScale:
		case Format8bppIndexed:
		case Format4bppIndexed:
		case Format1bppIndexed:
			return 1;

		default:
			break;
	}
	
	return 0;
}

GpStatus
GdipLoadImageFromDelegate_linux (GetHeaderDelegate getHeaderFunc,
								 GetBytesDelegate getBytesFunc,
								 PutBytesDelegate putBytesFunc,
                                 SeekDelegate seekFunc,
								 CloseDelegate closeFunc,
								 SizeDelegate sizeFunc,
                                 GpImage **image)
{
	GpImage *result = 0;
	GpStatus status = 0;
	ImageFormat format;
	
	byte format_peek[10];
	int format_peek_sz;
	
	format_peek_sz = getHeaderFunc (format_peek, 10);
	format = get_image_format ((char *)format_peek, format_peek_sz);
	
	switch (format) {
		case JPEG: {
			status = gdip_load_jpeg_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL) {
				result->image_format = JPEG;
			}
			break;
		}

		case PNG: {
			status = gdip_load_png_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL) {
				result->image_format = PNG;
			}
			break;
		}

		case BMP: {
			status = gdip_load_bmp_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL) {
				result->image_format = BMP;
			}
			break;
		}

		case TIF: {
			status = gdip_load_tiff_image_from_stream_delegate (getBytesFunc, putBytesFunc,
								seekFunc, closeFunc, sizeFunc, &result);
			if (result != NULL) {
				result->image_format = TIF;
			}
			break;
		}

		case GIF: {
			status = gdip_load_gif_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL) {
				result->image_format = GIF;
			}
			break;
		}

		default: {
			printf ("type: %d Not implemented\n", format);
			status = NotImplemented;
			break;
		}
	}

	*image = result;
	if (status != Ok) {
		*image = NULL;
	} else if (result && !result->active_bitmap) {
		/* If the codec didn't set the active bitmap we will */
		gdip_bitmap_setactive(result, NULL, 0);
	}
	
	return status;
}

GpStatus
GdipSaveImageToDelegate_linux (GpImage *image, GetBytesDelegate getBytesFunc,
                                          PutBytesDelegate putBytesFunc,
					  SeekDelegate seekFunc,
					  CloseDelegate closeFunc,
					  SizeDelegate sizeFunc,
					  GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
	GpStatus status = 0;
    	ImageFormat format;

    	if ((image == NULL) || (encoderCLSID == NULL) || (image->type != imageBitmap)) {
        	return InvalidParameter;
	}

    	format = gdip_get_imageformat_from_codec_clsid ((CLSID *)encoderCLSID);
    	if (format == INVALID) {
        	return UnknownImageFormat;
	}

    	switch (format) {
		case BMP:
	    		status = gdip_save_bmp_image_to_stream_delegate (putBytesFunc, image);
	    		break;

        	case PNG:
            		status = gdip_save_png_image_to_stream_delegate (putBytesFunc, image, params);
            		break;

        	case JPEG:
            		status = gdip_save_jpeg_image_to_stream_delegate (putBytesFunc, image, params);
            		break;

	        case GIF:
        	    	status = gdip_save_gif_image_to_stream_delegate (putBytesFunc, image, params);
            		break;

		case TIF:
	    		status = gdip_save_tiff_image_to_stream_delegate (getBytesFunc, putBytesFunc,
								seekFunc, closeFunc, sizeFunc, image, params);
	    		break;

	        default:
        		status = NotImplemented;
	            	break;
    	}

    	return status;
}

ImageFormat
gdip_image_format_for_format_guid (GDIPCONST GUID *formatGUID)
{
	if (memcmp(formatGUID, &gdip_bmp_image_format_guid, sizeof(GUID)) == 0)
		return BMP;
	if (memcmp(formatGUID, &gdip_jpg_image_format_guid, sizeof(GUID)) == 0)
		return JPEG;
	if (memcmp(formatGUID, &gdip_png_image_format_guid, sizeof(GUID)) == 0)
		return PNG;
	if (memcmp(formatGUID, &gdip_gif_image_format_guid, sizeof(GUID)) == 0)
		return GIF;
	if (memcmp(formatGUID, &gdip_tif_image_format_guid, sizeof(GUID)) == 0)
		return TIF;
	if (memcmp(formatGUID, &gdip_exif_image_format_guid, sizeof(GUID)) == 0)
		return EXIF;
	if (memcmp(formatGUID, &gdip_wmf_image_format_guid, sizeof(GUID)) == 0)
		return WMF;
	if (memcmp(formatGUID, &gdip_emf_image_format_guid, sizeof(GUID)) == 0)
		return EMF;
	if (memcmp(formatGUID, &gdip_membmp_image_format_guid, sizeof(GUID)) == 0)
		return PNG; /* MemoryBmp is saved as PNG */

	return INVALID;
}


#define CODECS_SUPPORTED 5
static BYTE *g_codeclist;
static int g_codecs = 0; /* Available codecs */

void initCodecList (void)
{
	BYTE *pos;
	
	g_codeclist = pos = GdipAlloc (sizeof (ImageCodecInfo) * CODECS_SUPPORTED);
	
	/* BMP codec - built-in */
	memcpy (pos, gdip_getcodecinfo_bmp(), sizeof (ImageCodecInfo));
	pos += sizeof (ImageCodecInfo);
	g_codecs++;
	
	/* JPEG codec */
	if (gdip_getcodecinfo_jpeg ()) {
		memcpy (pos, gdip_getcodecinfo_jpeg(), sizeof (ImageCodecInfo));
		pos += sizeof (ImageCodecInfo);
		g_codecs++;
	}
	
	/* GIF codec */
	if (gdip_getcodecinfo_gif ()) {
		memcpy (pos, gdip_getcodecinfo_gif (), sizeof (ImageCodecInfo));
		pos += sizeof (ImageCodecInfo);
		g_codecs++;
	}
	
	/* TIFF codec */
	if (gdip_getcodecinfo_tiff ()) {
		memcpy (pos, gdip_getcodecinfo_tiff (), sizeof (ImageCodecInfo));
		pos += sizeof (ImageCodecInfo);
		g_codecs++;
	}
	
	/* PNG codec */
	if (gdip_getcodecinfo_png ()) {
		memcpy (pos, gdip_getcodecinfo_png (), sizeof (ImageCodecInfo));
		pos += sizeof (ImageCodecInfo);
		g_codecs++;
	}
}

void releaseCodecList (void)
{
	if (g_codeclist)
		GdipFree (g_codeclist);
}

GpStatus
GdipGetImageDecodersSize (int *numDecoders, int *size)
{       
	if (!numDecoders || !size)
		return InvalidParameter;
	
	*numDecoders = g_codecs;
	*size = sizeof (ImageCodecInfo) * g_codecs;
	return Ok;
}

GpStatus
GdipGetImageDecoders (int numDecoders, int size, ImageCodecInfo *decoders)
{
	if (!decoders)
		return InvalidParameter;
					
	memcpy (decoders, g_codeclist, size);
	return Ok;
}

GpStatus
GdipGetImageEncodersSize (int *numEncoders, int *size)
{
	if (!numEncoders || !size)
		return InvalidParameter;
	
	*numEncoders = g_codecs;
	*size = sizeof (ImageCodecInfo) * g_codecs;
	return Ok;
}

GpStatus
GdipGetImageEncoders (UINT numEncoders, UINT size, ImageCodecInfo *encoders)
{
	if (!encoders)
		return InvalidParameter;
	
	memcpy (encoders, g_codeclist, size);
	return Ok;
}

GpStatus
GdipGetAllPropertyItems(GpImage *image, UINT totalBufferSize, UINT numProperties, PropertyItem *allItems)
{
	int		size;
	int		i;
	unsigned char	*ptr;

	if ((image == NULL) || (allItems == NULL)) {
		return InvalidParameter;
	}

	if (numProperties != image->active_bitmap->property_count) {
		return InvalidParameter;
	}

	size = image->active_bitmap->property_count * sizeof(PropertyItem);
	for (i = 0; i < numProperties; i++) {
		size += image->active_bitmap->property[i].length;
	}

	if (size != totalBufferSize) {
		return InvalidParameter;
	}

	ptr = (unsigned char *)allItems;
	ptr += totalBufferSize;

	/* Copy the PropertyItem array */
	memcpy(allItems, image->active_bitmap->property, sizeof(PropertyItem) * numProperties);

	/* Copy the values and fix up the value pointers in the PropertyItem array to point to them */
	for (i = 0; i < numProperties; i++) {
		if (allItems[i].value != NULL) {
			ptr -= allItems[i].length;
			memcpy(ptr, allItems[i].value, allItems[i].length);	/* Use the copied pointer, less indirection than image->active_bitmap->property[i].value */
			allItems[i].value = ptr;
		}
	}

	return Ok;
}

GpStatus
GdipGetEncoderParameterListSize(GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT *size)
{
	ImageFormat fmt;

	g_return_val_if_fail (clsidEncoder != NULL, InvalidParameter);
	g_return_val_if_fail (size != NULL, InvalidParameter);

	fmt = gdip_get_imageformat_from_codec_clsid ((CLSID *) clsidEncoder);

	switch (fmt) {
		case JPEG:
			*size = gdip_get_encoder_parameter_list_size_jpeg ();
			return Ok;
		case TIF:
			*size = 0;
			return NotImplemented;
		default:
			break;
	}

	return NotImplemented;
}

GpStatus
GdipGetEncoderParameterList(GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT size, EncoderParameters *buffer)
{
	ImageFormat fmt;

	g_return_val_if_fail (clsidEncoder != NULL, InvalidParameter);
	g_return_val_if_fail (buffer != NULL, InvalidParameter);

	fmt = gdip_get_imageformat_from_codec_clsid ((CLSID *) clsidEncoder);

	switch (fmt) {
		case JPEG:
			return gdip_fill_encoder_parameter_list_jpeg (buffer, size);
		case TIF:
			return NotImplemented;
		default:
			break;
	}

	return NotImplemented;
}

GpStatus
GdipGetImageThumbnail(GpImage *image, UINT thumbWidth, UINT thumbHeight, GpImage **thumbImage, GetThumbnailImageAbort callback, VOID *callbackData)
{
	/* We don't really need to implement this; we've got it handled in System.Drawing directly */
	return NotImplemented;
}

GpStatus
GdipLoadImageFromFileICM(GDIPCONST WCHAR* filename, GpImage **image)
{
	return (GdipLoadImageFromFile (filename, image));
}

const EncoderParameter *
gdip_find_encoder_parameter (GDIPCONST EncoderParameters *eps, const GUID *guid)
{
	int i;
	
	for (i = 0; i < eps->Count; i++) {
	if (memcmp (&(eps->Parameter[i].Guid), guid, sizeof(GUID)) == 0)
		return &(eps->Parameter[i]);
	}
	
	return NULL;
}

/*
	GDI+ 1.0 only supports multiple frames on an image for the
	tiff format
*/

GpStatus
GdipSaveAdd(GpImage *image, EncoderParameters* encoderParams)
{
	return NotImplemented;
}

GpStatus 
GdipSaveAddImage (GpImage *image, GpImage *imageNew, EncoderParameters *params)
{
	return NotImplemented;
}
