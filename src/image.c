/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4; -*-
 *
 * image.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
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
 *   Alexandre Pigolkine(pigolkine@gmx.de)
 *   Sanjay Gupta (gsanjay@novell.com)
 *   Vladimir Vukicevic (vladimir@pobox.com)
 *   Jordi Mas (jordi@ximian.com)
 *   Jonathan Gilbert (logic@deltaq.org)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "gdiplus-private.h"
#include "image-private.h"
#include "imageattributes-private.h"
#include "general-private.h"
#include "graphics-private.h"
#include "matrix.h"

#include "metafile-private.h"
#include "bmpcodec.h"
#include "pngcodec.h"
#include "jpegcodec.h"
#include "gifcodec.h"
#include "tiffcodec.h"
#include "icocodec.h"
#include "emfcodec.h"
#include "wmfcodec.h"

/*
 * format guids
 */
extern GUID gdip_bmp_image_format_guid;
extern GUID gdip_jpg_image_format_guid;
extern GUID gdip_png_image_format_guid;
extern GUID gdip_gif_image_format_guid;
extern GUID gdip_tif_image_format_guid;
extern GUID gdip_wmf_image_format_guid;
extern GUID gdip_emf_image_format_guid;
extern GUID gdip_ico_image_format_guid;
GUID gdip_exif_image_format_guid = {0xb96b3cb2U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
GUID gdip_membmp_image_format_guid = {0xb96b3caaU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

/*
 * encoder param guids
 */
GUID GdipEncoderCompression = {0x0E09D739DU, 0x0CCD4U, 0x44EEU, {0x8E, 0x0BA, 0x3F, 0x0BF, 0x8B, 0x0E4, 0x0FC, 0x58}};
GUID GdipEncoderColorDepth = {0x66087055U, 0x0AD66U, 0x4C7CU, {0x9A, 0x18, 0x38, 0x0A2, 0x31, 0x0B, 0x83, 0x37}};
GUID GdipEncoderSaveFlag = {0x292266FCU, 0x0AC40U, 0x47BFU, {0x8C, 0x0FC, 0x0A8, 0x5B, 0x89, 0x0A6, 0x55, 0x0DE}};
GUID GdipEncoderSaveAsCMYK = {0x0A219BBC9U, 0x0A9DU, 0x4005U, {0x0A3, 0x0EE, 0x3A, 0x42, 0x1B, 0x8B, 0x0B0, 0x6C}};
GUID GdipEncoderImageItems = {0x63875E13U, 0x1F1DU, 0x45ABU, {0x91, 0x95, 0x0A2, 0x9B, 0x60, 0x66, 0x0A6, 0x50}};
GUID GdipEncoderTransformation = {0x8D0EB2D1U, 0x0A58EU, 0x4EA8U, {0x0AA, 0x14, 0x10, 0x80, 0x74, 0x0B7, 0x0B6, 0x0F9}};
GUID GdipEncoderQuality = {0x1D5BE4B5U, 0x0FA4AU, 0x452DU, {0x9C, 0x0DD, 0x5D, 0x0B3, 0x51, 0x5, 0x0E7, 0x0EB}};
GUID GdipEncoderLuminanceTable = {0x0EDB33BCEU, 0x266U, 0x4A77U, {0x0B9, 0x4, 0x27, 0x21, 0x60, 0x99, 0x0E7, 0x17}};
GUID GdipEncoderChrominanceTable = {0x0F2E455DCU, 0x9B3U, 0x4316U, {0x82, 0x60, 0x67, 0x6A, 0x0DA, 0x32, 0x48, 0x1C}};

#define DECODERS_SUPPORTED 8
#define ENCODERS_SUPPORTED 5
static BYTE *g_decoder_list;
static int g_decoders = 0;
static BYTE *g_encoder_list;
static int g_encoders = 0;

static ImageFormat
gdip_image_format_for_format_guid (GDIPCONST GUID *formatGUID)
{
	if (memcmp (formatGUID, &gdip_bmp_image_format_guid, sizeof (GUID)) == 0)
		return BMP;
	if (memcmp (formatGUID, &gdip_jpg_image_format_guid, sizeof (GUID)) == 0)
		return JPEG;
	if (memcmp (formatGUID, &gdip_png_image_format_guid, sizeof (GUID)) == 0)
		return PNG;
	if (memcmp (formatGUID, &gdip_gif_image_format_guid, sizeof (GUID)) == 0)
		return GIF;
	if (memcmp (formatGUID, &gdip_tif_image_format_guid, sizeof (GUID)) == 0)
		return TIF;
	if (memcmp (formatGUID, &gdip_exif_image_format_guid, sizeof (GUID)) == 0)
		return EXIF;
	if (memcmp (formatGUID, &gdip_wmf_image_format_guid, sizeof (GUID)) == 0)
		return WMF;
	if (memcmp (formatGUID, &gdip_emf_image_format_guid, sizeof (GUID)) == 0)
		return EMF;
	if (memcmp (formatGUID, &gdip_ico_image_format_guid, sizeof (GUID)) == 0)
		return ICON;
	if (memcmp (formatGUID, &gdip_membmp_image_format_guid, sizeof (GUID)) == 0)
		return PNG; /* MemoryBmp is saved as PNG */
	return INVALID;
}

/* hack #1 - nonplaceable WMF metafiles are supported by no signature match them in the codecs */
static const BYTE nonplaceable_wmf_sig_pattern[] = { 0x01, 0x00, 0x09, 0x00, 0x00, 0x03 };
static const BYTE nonplaceable_wmf_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static const int nonplaceable_wmf_sig_count = 1;
static const int nonplaceable_wmf_sig_size = 6;

static BOOL
signature_match (char *data, size_t data_size, int size, int count, BYTE* sig_pattern, BYTE* sig_mask)
{
	if (data_size < size)
		return FALSE;

	int sig;
	for (sig = 0; sig < (size * count); sig += size) {
		BOOL match = TRUE;
		int p;
		for (p = 0; ((p < size) && (p < data_size)); p++) {
			BYTE *mask = (BYTE*)(&sig_mask [sig]);
			BYTE *pattern = (BYTE*)(&sig_pattern [sig]);
			if ((data [p] & mask[p]) != pattern[p]) {
				match = FALSE;
				break;
			}
		}
		if (match)
			return TRUE;
	}
	return FALSE;
}

static ImageFormat 
get_image_format (char *sig_read, size_t size_read, ImageFormat *final)
{
	ImageCodecInfo *decoder = (ImageCodecInfo*)g_decoder_list;
	int index;
	/* look at every decoder available, this will depends on how libgdiplus is compiled */
	for (index = 0; index < g_decoders; index++, decoder++) {
		/* for each signature in the codec */
		if (signature_match (sig_read, size_read, decoder->SigSize, decoder->SigCount, (BYTE*)&decoder->SigPattern[0],
			(BYTE*)&decoder->SigMask[0])) {
			*final = gdip_image_format_for_format_guid (&decoder->FormatID);
			return *final;
		}
	}
	/* last chance - GDI+ still detects some files even if they don't match the codec signatures */
	/* [mis-]handle non-placeable metafiles are WMF but reported as EMF (see #81178 for a test case) */
	if (signature_match (sig_read, size_read, nonplaceable_wmf_sig_size, nonplaceable_wmf_sig_count, 
		(BYTE*)&nonplaceable_wmf_sig_pattern[0], (BYTE*)&nonplaceable_wmf_sig_mask[0])) {
		*final = EMF;
		return WMF;
	}
	return INVALID;
}

/* Converts the given interpolation value to cairo_filter_t */
static cairo_filter_t
gdip_get_cairo_filter (InterpolationMode imode)
{
	switch (imode) {
	case InterpolationModeHighQuality:
	case InterpolationModeHighQualityBilinear:
	case InterpolationModeHighQualityBicubic:
		return CAIRO_FILTER_BEST;
	case InterpolationModeNearestNeighbor:
		return CAIRO_FILTER_NEAREST;
	case InterpolationModeBilinear:
		return CAIRO_FILTER_BILINEAR;
	case InterpolationModeBicubic:
		return CAIRO_FILTER_GAUSSIAN;
	case InterpolationModeLowQuality:
		return CAIRO_FILTER_FAST;
	case InterpolationModeDefault:
	default:
		return CAIRO_FILTER_GOOD;
	}
}

void 
gdip_image_init (GpImage *image)
{
	gdip_bitmap_init(image);
}

static void
copy_pixel (BYTE *src, BYTE *trg, int size)
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
	if (!line) {
		return OutOfMemory;
	}

	src = (BYTE *) image->active_bitmap->scan0;
	
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
	if (!line) {
		return OutOfMemory;
	}

	src = (BYTE *) image->active_bitmap->scan0;
	trg = (BYTE *) image->active_bitmap->scan0;
	trg +=  (height-1) * stride;
	
	for (i = 0; i < (height /2); i++, src += stride, trg -= stride) {			
		memcpy (line, trg, stride);	/* Save target line*/
		memcpy (trg, src, stride);	/* Copy src to trg*/
		memcpy (src, line, stride);	/* Copy trg to src*/
	}
	
	GdipFree (line);

	return Ok;
}

GpStatus WINGDIPAPI 
GdipDisposeImage (GpImage *image)
{
	if (!image)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		return gdip_bitmap_dispose (image);
	case ImageTypeMetafile:
		return gdip_metafile_dispose ((GpMetafile*)image);
	default: /* imageUndefined */
		g_warning ("unknown image type couldn't be disposed, ptr = %p, type %d", (void*)image, image->type);
		return Ok;
	}
}

/* coverity[+alloc : arg-*1] */
GpStatus WINGDIPAPI 
GdipGetImageGraphicsContext (GpImage *image, GpGraphics **graphics)
{
	GpGraphics	*gfx;
	cairo_pattern_t	*filter;
	
	if (!image || !graphics)
		return InvalidParameter;

	/* This function isn't allowed on metafiles - unless we're recording */
	if (image->type == ImageTypeMetafile) {
		GpMetafile *mf = (GpMetafile*)image;
		if (!mf->recording)
			return OutOfMemory;
		*graphics = gdip_metafile_graphics_new (mf);
		return *graphics ? Ok : OutOfMemory;
	}

	if (!image->active_bitmap)
		return InvalidParameter;

	/*
	 * Microsoft GDI+ only supports these pixel formats Format24bppRGB, Format32bppARGB, 
	 * Format32bppPARGB, Format32bppRGB, Format48bppRGB, Format64bppARGB, Format64bppPARGB
	 * but we're limited to 24/32 inside libgdiplus
	 */
	switch (image->active_bitmap->pixel_format) {
	case PixelFormat24bppRGB:
	case PixelFormat32bppARGB:
	case PixelFormat32bppPARGB:
	case PixelFormat32bppRGB:
		break;
	default:
		return OutOfMemory;
	}

	if (gdip_bitmap_ensure_surface (image) == NULL)
		return OutOfMemory;
	
	gfx = gdip_graphics_new (image->surface);
	if (!gfx)
		return OutOfMemory;

	gfx->dpi_x = image->active_bitmap->dpi_horz <= 0 ? gdip_get_display_dpi () : image->active_bitmap->dpi_horz;
	gfx->dpi_y = image->active_bitmap->dpi_vert <= 0 ? gdip_get_display_dpi () : image->active_bitmap->dpi_vert;
	gfx->bounds.Width = image->active_bitmap->width;
	gfx->bounds.Height = image->active_bitmap->height;
	gfx->orig_bounds.Width = image->active_bitmap->width;
	gfx->orig_bounds.Height = image->active_bitmap->height;

	gfx->image = image;
	gfx->type = gtMemoryBitmap;
	filter = cairo_pattern_create_for_surface (image->surface);
	cairo_pattern_set_filter (filter, gdip_get_cairo_filter (gfx->interpolation));
	cairo_pattern_destroy (filter);
	*graphics = gfx;
	return Ok;
}

GpStatus WINGDIPAPI 
GdipDrawImageI (GpGraphics *graphics, GpImage *image, INT x, INT y)
{
	return GdipDrawImage (graphics, image, x, y);
}

GpStatus WINGDIPAPI 
GdipDrawImage (GpGraphics *graphics, GpImage *image, REAL x, REAL y)
{
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!image)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap: {
		ActiveBitmapData *data = image->active_bitmap;
		return GdipDrawImageRect (graphics, image, x, y, data->width, data->height);
	}
	case ImageTypeMetafile: {
		MetafileHeader *metaheader = &((GpMetafile*)image)->metafile_header;
		return GdipDrawImageRect (graphics, image, x, y, metaheader->Width, metaheader->Height);
	}
	default:
		return InvalidParameter;
	}
}

GpStatus WINGDIPAPI
GdipDrawImageRectI (GpGraphics *graphics, GpImage *image, INT x, INT y, INT width, INT height)
{
	return GdipDrawImageRect (graphics, image, x, y, width, height);
}

GpStatus WINGDIPAPI
GdipDrawImageRect (GpGraphics *graphics, GpImage *image, REAL x, REAL y, REAL width, REAL height)
{
	cairo_pattern_t *pattern;
	cairo_pattern_t *org_pattern;
	MetafilePlayContext *metacontext = NULL;
	BOOL need_scaling = FALSE;
	double scaled_width, scaled_height;
	cairo_matrix_t orig_matrix;

	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!image)
		return InvalidParameter;
			
	if (image->type == ImageTypeBitmap) {
		/* check does not apply to metafiles, and it's better be done before converting the image */
		if (gdip_is_overflow(x) || gdip_is_overflow(y))
			return ValueOverflow;

		if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
			GpStatus status = OutOfMemory;
			GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb (image);

			if (rgb_bitmap) {
				status = GdipDrawImageRect (graphics, rgb_bitmap, x, y, width, height);
				GdipDisposeImage (rgb_bitmap);
			}
			return status;
		}
	}

	if ((width == 0) || (height == 0))
		return Ok;

	/* conversion must be done after the recursive call to GdipDrawImageRect to remove the indexed bitmap */
	if (!OPTIMIZE_CONVERSION (graphics)) {
		x = gdip_unitx_convgr (graphics, x);
		y = gdip_unity_convgr (graphics, y);
		width = gdip_unitx_convgr (graphics, width);
		height = gdip_unity_convgr (graphics, height);
	}
	
	cairo_new_path (graphics->ct);

	/* metafile */
	if (image->type == ImageTypeMetafile) {
		GpStatus status;

		metacontext = gdip_metafile_play_setup ((GpMetafile*)image, graphics, x, y, width, height);

		status = gdip_metafile_play (metacontext);

		gdip_metafile_play_cleanup (metacontext);
		return status;
	}

	/* Create a surface for this bitmap if one doesn't exist */
	if (gdip_bitmap_ensure_surface (image) == NULL)
		return OutOfMemory;

	if (width != image->active_bitmap->width || height != image->active_bitmap->height) {
		scaled_width = (double) width / image->active_bitmap->width;
		scaled_height = (double) height / image->active_bitmap->height;
		need_scaling = TRUE;
	}

	/* Use the image->surface as a pattern */
	pattern = cairo_pattern_create_for_surface (image->surface);

	cairo_pattern_set_filter (pattern, gdip_get_cairo_filter (graphics->interpolation));

	cairo_get_matrix (graphics->ct, &orig_matrix);
	cairo_translate (graphics->ct, x, y);

	if (need_scaling)
		cairo_scale (graphics->ct, scaled_width, scaled_height);
	
	org_pattern = cairo_get_source(graphics->ct);
	cairo_pattern_reference(org_pattern);

	cairo_set_source (graphics->ct, pattern);
	cairo_identity_matrix (graphics->ct);
	cairo_paint (graphics->ct);
	cairo_set_source (graphics->ct, org_pattern);	
	cairo_set_matrix (graphics->ct, &orig_matrix);

	cairo_pattern_destroy (org_pattern);
	cairo_pattern_destroy (pattern);

	return Ok;
}

/* points are upper left, upper right, lower left. fourth point is extrapolated. */
GpStatus WINGDIPAPI
GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, INT count)
{
	cairo_pattern_t	*pattern;
	cairo_pattern_t	*org_pattern;
	GpMatrix *matrix = NULL;
	cairo_matrix_t orig_matrix;
	GpRectF tRect;
	MetafilePlayContext *metacontext = NULL;

	if (!graphics || !dstPoints || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!image || (count != 3 && count != 4))
		return InvalidParameter;
	if (count == 4)
		return NotImplemented;

	cairo_new_path (graphics->ct);

	if (image->type == ImageTypeBitmap) {
		if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
			GpStatus status = OutOfMemory;
			GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb (image);
			if (rgb_bitmap) {
				status = GdipDrawImagePoints (graphics, rgb_bitmap, dstPoints, count);
				GdipDisposeImage(rgb_bitmap);
			}
			return status;
		}
		tRect.Width = image->active_bitmap->width; 
		tRect.Height = image->active_bitmap->height;
	} else {
		MetafileHeader *metaheader = &((GpMetafile*)image)->metafile_header;
		tRect.Width = metaheader->Width; 
		tRect.Height = metaheader->Height;
	}

	tRect.X = 0; 
	tRect.Y = 0; 
	GdipCreateMatrix3 (&tRect, dstPoints, &matrix);

	/* metafile */
	if (image->type == ImageTypeMetafile) {
		GpStatus status;
		metacontext = gdip_metafile_play_setup ((GpMetafile*)image, graphics, tRect.X, tRect.Y, tRect.Width, tRect.Height);

		cairo_get_matrix (graphics->ct, &orig_matrix);
		gdip_cairo_set_matrix (graphics, matrix);
		status = gdip_metafile_play (metacontext);
		GdipDeleteMatrix (matrix);

		gdip_metafile_play_cleanup (metacontext);
		return status;
	}

	/* Create a surface for this bitmap if one doesn't exist */
	if (gdip_bitmap_ensure_surface (image) == NULL) {
		GdipDeleteMatrix(matrix);
		return OutOfMemory;
	}

	pattern = cairo_pattern_create_for_surface (image->surface);
	cairo_pattern_set_filter (pattern, gdip_get_cairo_filter (graphics->interpolation));

	org_pattern = cairo_get_source(graphics->ct);
	cairo_pattern_reference(org_pattern);
	
	cairo_get_matrix(graphics->ct, &orig_matrix);
	gdip_cairo_set_matrix (graphics, matrix);
	cairo_set_source_surface (graphics->ct, image->surface, 0, 0);
	
	cairo_paint (graphics->ct);	
	cairo_set_source(graphics->ct, org_pattern);
	cairo_set_matrix (graphics->ct, &orig_matrix);

	GdipDeleteMatrix (matrix);
	cairo_pattern_destroy (org_pattern);
	cairo_pattern_destroy (pattern);

	return Ok;
}

GpStatus WINGDIPAPI
GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, INT count)
{
	GpStatus status;
	GpPointF *pointsF;

	if (!dstPoints || count < 0)
		return InvalidParameter;

	pointsF = convert_points (dstPoints, count);
	if (!pointsF)
		return OutOfMemory;

	status = GdipDrawImagePoints (graphics, image, pointsF, count);

	GdipFree (pointsF);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawImagePointRect (GpGraphics *graphics, GpImage *image, REAL x, REAL y, REAL srcx, REAL srcy,
	REAL srcwidth, REAL srcheight, GpUnit srcUnit)
{
	return GdipDrawImageRectRect (graphics, image, x, y, srcwidth, srcheight, srcx, srcy, 
		srcwidth, srcheight, srcUnit, NULL, NULL, NULL);
}

GpStatus WINGDIPAPI
GdipDrawImagePointRectI (GpGraphics *graphics, GpImage *image, INT x, INT y, INT srcx, INT srcy,
	INT srcwidth, INT srcheight, GpUnit srcUnit)
{
	return GdipDrawImagePointRect (graphics, image, x, y, srcx, srcy, srcwidth, srcheight, srcUnit);
}

GpStatus WINGDIPAPI
GdipDrawImageRectRect (GpGraphics *graphics, GpImage *image,
                       REAL dstx, REAL dsty, REAL dstwidth, REAL dstheight,
                       REAL srcx, REAL srcy, REAL srcwidth, REAL srcheight,
                       GpUnit srcUnit,
                       GDIPCONST GpImageAttributes *imageAttributes,
                       DrawImageAbort callback, void *callbackData)
{
	GpStatus status;
	cairo_pattern_t	*pattern;
	cairo_pattern_t	*orig;
	cairo_matrix_t	mat;
	void		*dest;
	void		*org;
	int		org_format;
	cairo_surface_t *org_surface;
	BOOL		allocated = FALSE;
	cairo_surface_t	*original = NULL;
	
	if (!graphics)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!image)
		return InvalidParameter;

	switch (srcUnit) {
	case UnitPixel:
		break;
	case UnitPoint:
	case UnitInch:
	case UnitDocument:
	case UnitMillimeter:
		if (graphics->type != gtPostScript)
			return NotImplemented; /* GDI+ returns the same */
		break;
	case UnitWorld:
	case UnitDisplay:
	default:
		return InvalidParameter;
	}

	if (image->type == ImageTypeBitmap) {
		if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
			GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb (image);
			if (!rgb_bitmap)
				return OutOfMemory;

			status = GdipDrawImageRectRect (graphics, rgb_bitmap,
				dstx, dsty, dstwidth, dstheight,
				srcx, srcy, srcwidth, srcheight,
				srcUnit, imageAttributes, callback, callbackData);
			GdipDisposeImage (rgb_bitmap);

			return status;
		}
	} else {
		/* metafile support */
		return NotImplemented;
	}

	/* see OPTIMIZE_CONVERSION in general.h */
	if (srcUnit != UnitPixel && graphics->type != gtPostScript) {
		dstx = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_x, graphics->type, dstx);
		dsty = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_y, graphics->type, dsty);
		dstwidth = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_x, graphics->type, dstwidth);
		dstheight = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_y, graphics->type, dstheight);
		srcx = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_x, graphics->type, srcx);
		srcy = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_y, graphics->type, srcy);
		srcwidth = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_x, graphics->type, srcwidth);
		srcheight = gdip_unit_conversion (srcUnit, UnitCairoPoint, graphics->dpi_y, graphics->type, srcheight);
	}

	org = dest = image->active_bitmap->scan0; 
	org_format = image->active_bitmap->pixel_format;
	org_surface = image->surface;
	status = gdip_process_bitmap_attributes (image, &dest, (GpImageAttributes *) imageAttributes, &allocated);
	if (status != Ok)
		return status;

	/*  If allocated is true we have a newly allocated and altered Scan0 in dest */
	if (allocated) {
		image->active_bitmap->scan0 = dest;
		image->surface = NULL;
	}

	cairo_matrix_init (&mat, 1, 0, 0, 1, 0, 0);

	if (imageAttributes && imageAttributes->wrapmode != WrapModeClamp) {
		float		posx;
		float		posy;
		BOOL		flipXOn = (imageAttributes->wrapmode == WrapModeTileFlipX);
		BOOL		flipYOn = (imageAttributes->wrapmode == WrapModeTileFlipY);
		BOOL		flipX = FALSE;
		BOOL		flipY = FALSE;
		GpBitmap	*imgflipX = NULL;
		GpBitmap	*imgflipY = NULL;
		GpBitmap	*imgflipXY = NULL;

		float img_width = image->active_bitmap->width *  (dstwidth / srcwidth);
		float img_height = image->active_bitmap->height * (dstheight / srcheight);
		cairo_surface_t *cur_surface;

		if (imageAttributes->wrapmode == WrapModeTileFlipXY) {
			flipXOn = flipYOn = TRUE;
		}
		
		if (flipXOn) {			
			/* We're ok just cloning the bitmap, we don't need the image data
			 * and we destroy it before we leave this function */
			status = gdip_bitmap_clone (image, &imgflipX);
			if (status != Ok) {
				gdip_bitmap_dispose(imgflipX);
				return status;
			}

			status = gdip_flip_x (imgflipX);
			if (status != Ok) {
				gdip_bitmap_dispose (imgflipX);
				return status;
			}

			gdip_bitmap_ensure_surface (imgflipX);
		}
		
		if (flipYOn) {			
			status = gdip_bitmap_clone (image, &imgflipY);
			if (status != Ok) {
				gdip_bitmap_dispose(imgflipX);
				gdip_bitmap_dispose(imgflipY);
				return status;
			}

			status = gdip_flip_y (imgflipY);
			if (status != Ok) {
				gdip_bitmap_dispose (imgflipX);
				gdip_bitmap_dispose (imgflipY);
				return status;
			}

			gdip_bitmap_ensure_surface (imgflipY);			
		}
		
		if (flipXOn && flipYOn) {			
			status = gdip_bitmap_clone (image, &imgflipXY);
			if (status != Ok) {
				gdip_bitmap_dispose(imgflipX);
				gdip_bitmap_dispose(imgflipY);
				gdip_bitmap_dispose(imgflipXY);
				return status;
			}

			status = gdip_flip_x (imgflipXY);
			if (status != Ok) {
				gdip_bitmap_dispose(imgflipX);
				gdip_bitmap_dispose(imgflipY);
				gdip_bitmap_dispose(imgflipXY);
				return status;
			}

			status = gdip_flip_y (imgflipXY);
			if (status != Ok) {
				gdip_bitmap_dispose(imgflipX);
				gdip_bitmap_dispose(imgflipY);
				gdip_bitmap_dispose(imgflipXY);
				return status;
			}

			gdip_bitmap_ensure_surface (imgflipXY);			
		}
		
		gdip_bitmap_ensure_surface (image);

		for (posy = 0; posy < dstheight; posy += img_height) {
			for (posx = 0; posx < dstwidth; posx += img_width) {
				if (flipX && flipY) {
					cur_surface = imgflipXY->surface;
				} else {
					if (flipX) {
						cur_surface = imgflipX->surface;
					} else {
						if (flipY) {
							cur_surface = imgflipY->surface;
						} else {
							cur_surface = image->surface;
						}
					}
				}

				cairo_matrix_translate (&mat, srcx, srcy);
				cairo_matrix_scale (&mat, srcwidth / dstwidth, srcheight / dstheight);
				cairo_matrix_translate (&mat, - (dstx + posx), - (dsty + posy));

				pattern = cairo_pattern_create_for_surface(cur_surface);
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
		original = image->surface;

		filter = cairo_pattern_create_for_surface (original);
		cairo_pattern_set_filter (filter, gdip_get_cairo_filter (graphics->interpolation));

		cairo_matrix_translate (&mat, srcx, srcy);

		if (!gdip_near_zero(srcwidth - dstwidth) || !gdip_near_zero(srcheight - dstheight))
			cairo_matrix_scale (&mat, srcwidth / dstwidth, srcheight / dstheight);

		cairo_matrix_translate (&mat, -dstx, -dsty);

		pattern = cairo_pattern_create_for_surface(original);
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

	if (allocated) {
		gdip_bitmap_invalidate_surface (image);
		image->active_bitmap->scan0 = org;
		image->active_bitmap->pixel_format = org_format;
		image->surface = org_surface;
		// NOTE: dest is freed by gdip_bitmap_invalidate_surface above
	}
	
	return Ok;
}

GpStatus WINGDIPAPI
GdipDrawImageRectRectI (GpGraphics *graphics, GpImage *image, INT dstx, INT dsty, INT dstwidth, INT dstheight,
	INT srcx, INT srcy, INT srcwidth, INT srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes,
	DrawImageAbort callback, void *callbackData)
{
	return GdipDrawImageRectRect (graphics, image, dstx, dsty, dstwidth, dstheight,
					srcx, srcy, srcwidth, srcheight,
					srcUnit,
					imageAttributes, callback, callbackData);
}

GpStatus WINGDIPAPI
GdipDrawImagePointsRect (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *points, INT count, REAL srcx, REAL srcy,
	REAL srcwidth, REAL srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback,
	void *callbackData)
{
	GpRectF rect;
	GpStatus status;
	GpMatrix *matrix = NULL;
	cairo_matrix_t orig_matrix;

	if (!graphics || !points || count <= 0)
		return InvalidParameter;
	if (graphics->state == GraphicsStateBusy)
		return ObjectBusy;
	if (!image || (count != 3 && count != 4))
		return InvalidParameter;

	switch (srcUnit) {
	case UnitPixel:
		break;
	case UnitPoint:
	case UnitInch:
	case UnitDocument:
	case UnitMillimeter:
		if (graphics->type != gtPostScript)
			return NotImplemented; /* GDI+ returns the same */
		break;
	case UnitWorld:
	case UnitDisplay:
	default:
		return InvalidParameter;
	}

	if (count == 4)
		return NotImplemented;

	rect.X = 0; 
	rect.Y = 0; 
	if (image->type == ImageTypeBitmap) {
		rect.Width = image->active_bitmap->width; 
		rect.Height = image->active_bitmap->height;
	} else {
		MetafileHeader *metaheader = &((GpMetafile*)image)->metafile_header;
		rect.Width = metaheader->Width; 
		rect.Height = metaheader->Height;
	}

	status = GdipCreateMatrix3 (&rect, points, &matrix);
	if (status != Ok) {
		GdipDeleteMatrix (matrix);
		return status;
	}

	cairo_get_matrix (graphics->ct, &orig_matrix);
	gdip_cairo_set_matrix (graphics, matrix);
	status = GdipDrawImageRectRect (graphics, image, rect.X, rect.Y, rect.Width, rect.Height, srcx, srcy, 
		srcwidth, srcheight, srcUnit, imageAttributes, callback, callbackData);
	cairo_set_matrix (graphics->ct, &orig_matrix);
	
	GdipDeleteMatrix (matrix);
	return status;
}

GpStatus WINGDIPAPI
GdipDrawImagePointsRectI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *points, INT count, INT srcx, INT srcy,
	INT srcwidth, INT srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback,
	void *callbackData)
{
	GpPointF *pointsF;

	if (!points || count < 0)
		return InvalidParameter;

	pointsF = convert_points (points, count);
	if (!pointsF)
		return OutOfMemory;

	GpStatus status = GdipDrawImagePointsRect(graphics, image, pointsF, count, srcx, srcy, srcwidth, srcheight,
		srcUnit, imageAttributes, callback, callbackData);
	GdipFree(pointsF);
	return status;
}

/*
 * These two will never be implemented, as 'stream' is a COM IStream
 */
GpStatus WINGDIPAPI
GdipLoadImageFromStream (void *stream, GpImage **image)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!stream || !image)
		return InvalidParameter;

	return NotImplemented; /* GdipLoadImageFromStream - not supported */
}

GpStatus WINGDIPAPI
GdipLoadImageFromStreamICM (void *stream, GpImage **image)
{
	return GdipLoadImageFromStream (stream, image);	
}

GpStatus WINGDIPAPI
GdipSaveImageToStream (GpImage *image, void *stream, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
	if (!stream || !image)
		return InvalidParameter;

	return NotImplemented; /* GdipSaveImageToStream - not supported */
}

/* coverity[+alloc : arg-*1] */
GpStatus WINGDIPAPI 
GdipLoadImageFromFile (GDIPCONST WCHAR *file, GpImage **image)
{
	FILE		*fp = NULL;
	GpImage		*result = NULL;
	GpStatus	status = Ok;
	ImageFormat	format, public_format;
	char		*file_name = NULL;
	char		format_peek[MAX_CODEC_SIG_LENGTH];
	int		format_peek_sz;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;
	
	if (!image || !file)
		return InvalidParameter;
	
	file_name = (char *) utf16_to_utf8 ((const gunichar2 *)file, -1);
	if (!file_name) {
		*image = NULL;
		return InvalidParameter;
	}
	
	fp = fopen (file_name, "rb");
	if (!fp) {
		GdipFree (file_name);
		return OutOfMemory;
	}
	
	format_peek_sz = fread (format_peek, 1, MAX_CODEC_SIG_LENGTH, fp);
	format = get_image_format (format_peek, format_peek_sz, &public_format);
	fseek (fp, 0, SEEK_SET);
	
	switch (format) {
	case BMP:
		status = gdip_load_bmp_image_from_file (fp, &result);
		break;
	case TIF:
		status = gdip_load_tiff_image_from_file (fp, &result);
		break;
	case GIF:
		status = gdip_load_gif_image_from_file (fp, &result);
		break;
	case PNG:
		status = gdip_load_png_image_from_file (fp, &result);
		break;
	case JPEG:
		status = gdip_load_jpeg_image_from_file (fp, file_name, &result);
		break;
	case ICON:
		status = gdip_load_ico_image_from_file (fp, &result);
		break;
	case WMF:
		status = gdip_load_wmf_image_from_file (fp, &result);
		break;
	case EMF:
		status = gdip_load_emf_image_from_file (fp, &result);
		break;
	case EXIF:
		status = NotImplemented;
		break;
	default:
		status = OutOfMemory;
		break;
	}

	if (result && (status == Ok))
		result->image_format = public_format;
	
	fclose (fp);
	GdipFree (file_name);
	
	*image = result;
	if (status != Ok) {
		*image = NULL;
	} else if (result && (result->type == ImageTypeBitmap) && !result->active_bitmap) {
		/* If the codec didn't set the active bitmap we will */
		gdip_bitmap_setactive (result, NULL, 0);
	}
	
	return status;
}

/* Note: use only for encoders (there's more decoders than encoders) */
static ImageFormat 
gdip_get_imageformat_from_codec_clsid (CLSID *encoderCLSID)
{
	GpStatus status;
	UINT numEncoders, size, cnt;
	ImageCodecInfo *encoders, *encoder;

	status = GdipGetImageEncodersSize (&numEncoders, &size);
	if ((status != Ok) ||(numEncoders == 0))
		return INVALID;

	encoders = GdipAlloc (size);

	status = GdipGetImageEncoders (numEncoders, size, encoders);
	if (status != Ok) {
		GdipFree(encoders);
		return INVALID;
	}

	for (cnt = 0, encoder = encoders; cnt < numEncoders; cnt++, encoder++) {
		if (memcmp (&encoder->Clsid, encoderCLSID, sizeof (GUID)) == 0) {
		ImageFormat ifmt = gdip_image_format_for_format_guid (&encoder->FormatID);
				GdipFree (encoders);
		return ifmt;
		}
	}

	GdipFree (encoders);
	return INVALID;
}

GpStatus WINGDIPAPI
GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
	FILE *fp = 0;
	GpStatus status = 0;
	char *file_name;
	ImageFormat format;
	
	if (!image || !file || !encoderCLSID)
		return InvalidParameter;

	if (image->type != ImageTypeBitmap)
		return InvalidParameter;
	
	format = gdip_get_imageformat_from_codec_clsid ( (CLSID *)encoderCLSID);
	if (format == INVALID)
		return UnknownImageFormat;
	
	file_name = (char *) utf16_to_utf8 ((const gunichar2 *)file, -1);
	if (file_name == NULL)
		return InvalidParameter;
	
	gdip_bitmap_flush_surface (image);
	
	if (format == GIF) { /* gif library has to open the file itself*/
		status = gdip_save_gif_image_to_file ((BYTE*)file_name, image);
		GdipFree (file_name);
		return status;
	} else if (format == TIF) { 
		/* tif library has to open the file itself or seeking will fail when saving multi-page images*/
		status = gdip_save_tiff_image_to_file ((BYTE*)file_name, image, params);
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
		case ICON:
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

GpStatus WINGDIPAPI 
GdipGetImageBounds (GpImage *image, GpRectF *rect, GpUnit *unit)
{
	if (!image || !rect || !unit)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		rect->X = 0;
		rect->Y = 0;
		rect->Height = image->active_bitmap->height;
		rect->Width = image->active_bitmap->width;
		*unit = UnitPixel;
		break;
	case ImageTypeMetafile: {
		MetafileHeader *metaheader = gdip_get_metaheader(image);
		rect->X = metaheader->X;
		rect->Y = metaheader->Y;
		rect->Height = metaheader->Height;
		rect->Width = metaheader->Width;
		*unit = UnitPixel;
		break;
	}
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImageDimension (GpImage *image, REAL *width, REAL *height)
{
	if (!image || !width || !height)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		*width = image->active_bitmap->width;
		*height = image->active_bitmap->height;
		break;
	case ImageTypeMetafile: {
		MetafileHeader *metaheader = gdip_get_metaheader(image);

		// The width and height values are returned in 0.01 millimeter units.
		*width = gdip_unit_conversion (UnitPixel, UnitMillimeter, metaheader->DpiX, gtMemoryBitmap, metaheader->Width) * 100;
		*height = gdip_unit_conversion (UnitPixel, UnitMillimeter, metaheader->DpiY, gtMemoryBitmap, metaheader->Height) * 100;

		break;
	}
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImageType (GpImage *image, ImageType *type)
{
	if (!image || !type)
		return InvalidParameter;

	*type = image->type;
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImageWidth (GpImage *image, UINT *width)
{
	if (!image || !width)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		*width = image->active_bitmap->width;
		break;
	case ImageTypeMetafile:
		*width = gdip_get_metaheader(image)->Width;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImageHeight (GpImage *image, UINT *height)
{
	if (!image || !height)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		*height = image->active_bitmap->height;
		break;
	case ImageTypeMetafile:
		*height = gdip_get_metaheader(image)->Height;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImageHorizontalResolution (GpImage *image, REAL *resolution)
{
	if (!image || !resolution)
		return InvalidParameter;
	
	switch (image->type) {
	case ImageTypeBitmap:
		*resolution = image->active_bitmap->dpi_horz;
		break;
	case ImageTypeMetafile:
		*resolution = gdip_get_metaheader(image)->DpiX;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImageVerticalResolution (GpImage *image, REAL *resolution)
{
	if (!image || !resolution)
		return InvalidParameter;
	
	switch (image->type) {
	case ImageTypeBitmap:
		*resolution = image->active_bitmap->dpi_vert;
		break;
	case ImageTypeMetafile:
		*resolution = gdip_get_metaheader(image)->DpiY;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImageFlags (GpImage *image, UINT *flags)
{
	if (!image || !flags)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		*flags = image->active_bitmap->image_flags;
		break;
	case ImageTypeMetafile:
		*flags = ImageFlagsUndocumented | ImageFlagsReadOnly | ImageFlagsHasAlpha | ImageFlagsScalable;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetImageRawFormat (GpImage *image, GUID *format)
{
	if (!image || !format)
		return InvalidParameter;
	
	switch (image->image_format) {
	case BMP:
		memcpy (format, &gdip_bmp_image_format_guid, sizeof (GUID));
		break;
	case TIF:
		memcpy (format, &gdip_tif_image_format_guid, sizeof (GUID));
		break;
	case GIF:
		memcpy (format, &gdip_gif_image_format_guid, sizeof (GUID));
		break;
	case PNG:
		memcpy (format, &gdip_png_image_format_guid, sizeof (GUID));
		break;
	case JPEG:
		memcpy (format, &gdip_jpg_image_format_guid, sizeof (GUID));
		break;
	case EXIF:
		memcpy (format, &gdip_exif_image_format_guid, sizeof (GUID));
		break;
	case WMF:
		memcpy (format, &gdip_wmf_image_format_guid, sizeof (GUID));
		break;
	case EMF:
		memcpy (format, &gdip_emf_image_format_guid, sizeof (GUID));
		break;
	case MEMBMP:
		memcpy (format, &gdip_membmp_image_format_guid, sizeof (GUID));
		break;
	case ICON:
		memcpy (format, &gdip_ico_image_format_guid, sizeof (GUID));
		break;
	default:
		return InvalidParameter;
		}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImagePixelFormat (GpImage *image, PixelFormat *format)
{
	if (!image || !format)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		*format = image->active_bitmap->pixel_format;
		break;
	case ImageTypeMetafile:
		*format = PixelFormat32bppRGB;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count)
{
	if (!image || !count)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		*count = image->num_of_frames;
		break;
	case ImageTypeMetafile:
		*count = 1; /* always one for metafiles */
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count)
{
	if (!image || !dimensionGUID)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap: {
		if (count < 1 || count > image->num_of_frames)
			return Win32Error;

		int i;
		int countReturn = image->num_of_frames;
	
		if (countReturn > count)
			countReturn = count;

		for (i = 0; i < countReturn; i++)
			dimensionGUID[i] = image->frames[i].frame_dimension;
		break;
	}
	case ImageTypeMetafile:
		if (count != 1)
			return InvalidParameter;

		dimensionGUID[0] = gdip_image_frameDimension_page_guid;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI
GdipImageGetFrameCount (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count)
{
	if (!image)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap: {
		if (!dimensionGUID || !count)
			return Win32Error;

		int i;
		for (i = 0; i < image->num_of_frames; i++){
			if (memcmp (dimensionGUID, &(image->frames[i].frame_dimension), sizeof (CLSID)) == 0) {
				*count = image->frames[i].count;
				return Ok;
			}
		}

		return Win32Error;
	}
	case ImageTypeMetafile:
		if (!count)
			return InvalidParameter;

		/* note: an empty GUID also returns 1 */
		*count = 1;
		return Ok;
	default:
		break;
	}
	return InvalidParameter;
}

GpStatus WINGDIPAPI
GdipImageSelectActiveFrame (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT index)
{
	if (!image || !dimensionGUID)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		return gdip_bitmap_setactive (image, dimensionGUID, index);
	case ImageTypeMetafile:
		/* any index value or GUID returns Ok */
		return Ok;
	default:
		return InvalidParameter;
	}
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

	unsigned long long int size = (unsigned long long int)target_height * target_stride;
	if (size > G_MAXINT32)
		return OutOfMemory;

	rotated = GdipAlloc (size);

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

	gdip_bitmap_flush_surface (image);
	gdip_bitmap_invalidate_surface (image);

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

	unsigned long long int size = (unsigned long long int)target_height * target_stride;
	if (size > G_MAXINT32)
		return OutOfMemory;

	rotated = GdipAlloc (size);
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

		GpStatus status = gdip_init_pixel_stream (&stream, image->active_bitmap, 0, 0, image->active_bitmap->width, image->active_bitmap->height);

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
					GpStatus status = gdip_init_pixel_stream (&scan[i], image->active_bitmap, 0, scan_index, source_width, 1);

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
	gdip_bitmap_flush_surface (image);
	gdip_bitmap_invalidate_surface (image);

	return Ok;
}

GpStatus WINGDIPAPI 
GdipImageRotateFlip (GpImage *image, RotateFlipType type)
{
	int	angle;
	BOOL	flip_x;

	if (!image)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

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
			return InvalidParameter;
	}	
	
	if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format) && (gdip_get_pixel_format_depth (image->active_bitmap->pixel_format) < 8)) {
		return gdip_rotate_flip_packed_indexed (image, image->active_bitmap->pixel_format, angle, flip_x);
	} else {
		return gdip_rotate_orthogonal_flip_x (image, angle, flip_x);
	}
}

GpStatus WINGDIPAPI 
GdipGetImagePalette (GpImage *image, ColorPalette *palette, INT size)
{
	int bytes_needed;

	if (!image || !palette)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	if (!image->active_bitmap->palette || image->active_bitmap->palette->Count == 0) {
		if (size >= 0 && size < sizeof(ColorPalette))
			return InvalidParameter;

		palette->Count = 0;
		if (image->active_bitmap->palette) {
			palette->Flags = image->active_bitmap->palette->Flags;
		}
		else {
			palette->Flags = 0;
		}

		return Ok;
	}

	GdipGetImagePaletteSize (image, &bytes_needed);
	if (bytes_needed != size)
		return InvalidParameter;

	memcpy(palette, image->active_bitmap->palette, bytes_needed);
	return Ok;
}

GpStatus WINGDIPAPI 
GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette)
{
	int size;
	ColorPalette *result;

	if (!image || !palette || palette->Count == 0 || palette->Count > 256)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	size = (palette->Count - 1) * sizeof(ARGB) + sizeof(ColorPalette);
	if (!image->active_bitmap->palette || (palette->Count != image->active_bitmap->palette->Count)) {
		result = (ColorPalette *) GdipAlloc (size);
		if (!result)
			return OutOfMemory;

		if (image->active_bitmap->palette)
			GdipFree (image->active_bitmap->palette);
		image->active_bitmap->palette = result;
	}

	memcpy (image->active_bitmap->palette, palette, size);
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetImagePaletteSize (GpImage *image, INT* size)
{
	int palette_entries;

	if (!image || !size)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return GenericError;

	palette_entries = (image->active_bitmap->palette) ? image->active_bitmap->palette->Count : 0;

	if (palette_entries == 0)
		*size = sizeof(ColorPalette);
	else
		*size = (palette_entries - 1) * sizeof(ARGB) + sizeof(ColorPalette);

	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetPropertyCount (GpImage *image, UINT *propertyNumber)
{
	if (!image || !propertyNumber)
		return InvalidParameter;

	switch (image->type) {
	case ImageTypeBitmap:
		*propertyNumber = image->active_bitmap->property_count;
		break;
	case ImageTypeMetafile:
		*propertyNumber = 0;
		break;
	default:
		return InvalidParameter;
	}
	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetPropertyIdList (GpImage *image, UINT propertyNumber, PROPID *list)
{
	int	i;

	if (!image || !list)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	if (propertyNumber != image->active_bitmap->property_count)
		return InvalidParameter;

	for (i = 0; i < propertyNumber; i++) {
		list[i] = image->active_bitmap->property[i].id;
	}

	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size)
{
	int	index;

	if (!image || !size)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	if (gdip_bitmapdata_property_find_id(image->active_bitmap, propID, &index) != Ok) {
		return PropertyNotFound;
	}

	*size = sizeof(PropertyItem) + image->active_bitmap->property[index].length;

	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem *buffer)
{
	int	index;

	if (!image || !buffer)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	if (gdip_bitmapdata_property_find_id(image->active_bitmap, propID, &index) != Ok) {
		return PropertyNotFound;
	}

	if (size != (sizeof(PropertyItem) + image->active_bitmap->property[index].length)) {
		return InvalidParameter;
	}

	memcpy(buffer, &image->active_bitmap->property[index], sizeof(PropertyItem));
	buffer->value = (BYTE*)buffer + sizeof(PropertyItem);
	memcpy(buffer->value, image->active_bitmap->property[index].value, buffer->length);

	return Ok;
}

GpStatus WINGDIPAPI 
GdipGetPropertySize (GpImage *image, UINT *totalBufferSize, UINT *numProperties)
{
	UINT	size;
	int	i;

	if (!image || !totalBufferSize || !numProperties)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	*numProperties = image->active_bitmap->property_count;

	size = image->active_bitmap->property_count * sizeof(PropertyItem);
	for (i = 0; i < image->active_bitmap->property_count; i++) {
		size += image->active_bitmap->property[i].length;
	}

	*totalBufferSize = size;
	return Ok;
}

GpStatus WINGDIPAPI 
GdipRemovePropertyItem (GpImage *image, PROPID propID)
{
	if (!image)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	return gdip_bitmapdata_property_remove_id(image->active_bitmap, propID);
}

GpStatus WINGDIPAPI 
GdipSetPropertyItem(GpImage *image, GDIPCONST PropertyItem *item)
{
	int	i;

	if (!image || !item)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	switch(image->image_format) {
		case BMP:
		case TIF:
		case GIF:
		case JPEG:
		case PNG:
		case ICON: {
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

GpStatus WINGDIPAPI
GdipCloneImage (GpImage *image, GpImage **cloneImage)
{
	if (!image || !cloneImage)
		return InvalidParameter;

	switch (image->type){
	case ImageTypeBitmap:
		gdip_bitmap_clone (image, cloneImage);
		gdip_bitmap_setactive(*cloneImage, NULL, 0);
		return Ok;

	case ImageTypeMetafile:
		return gdip_metafile_clone ((GpMetafile*)image, (GpMetafile**)cloneImage);

	case ImageTypeUnknown:
	default:
		return Ok;
	}
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
		case PixelFormat16bppARGB1555:
		case PixelFormat16bppGrayScale:
		case PixelFormat16bppRGB555:
		case PixelFormat16bppRGB565:
		case PixelFormat24bppRGB:
		case PixelFormat32bppARGB:
		case PixelFormat32bppPARGB:
		case PixelFormat32bppRGB:
			return 8;		/* FIXME - shouldn't this be 32? - pdb */

		case PixelFormat48bppRGB:
		case PixelFormat64bppARGB:
		case PixelFormat64bppPARGB:
			return 16;

		case PixelFormat8bppIndexed:
			return 8;

		case PixelFormat4bppIndexed:
			return 4;

		case PixelFormat1bppIndexed:
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
		case PixelFormat16bppARGB1555:
		case PixelFormat32bppARGB:
		case PixelFormat32bppPARGB:
		case PixelFormat64bppARGB:
		case PixelFormat64bppPARGB:
		case PixelFormat32bppRGB:	/* Cairo uses four bytes for 32BPP*/
		case PixelFormat24bppRGB:
			return 4;

		case PixelFormat16bppRGB555:
		case PixelFormat16bppRGB565: 
		case PixelFormat48bppRGB:
			return 3;

		case PixelFormat16bppGrayScale:
		case PixelFormat8bppIndexed:
		case PixelFormat4bppIndexed:
		case PixelFormat1bppIndexed:
			return 1;

		default:
			break;
	}
	
	return 0;
}

GpStatus WINGDIPAPI
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
	ImageFormat format, public_format;
	dstream_t *loader = NULL;
	
	BYTE format_peek[MAX_CODEC_SIG_LENGTH];
	int format_peek_sz;
	
	format_peek_sz = getHeaderFunc (format_peek, MAX_CODEC_SIG_LENGTH);
	format = get_image_format ((char *)format_peek, format_peek_sz, &public_format);
	
	switch (format) {
	case JPEG:
		loader = dstream_input_new (getBytesFunc, seekFunc);
		status = gdip_load_jpeg_image_from_stream_delegate (loader, &result);
		break;
	case PNG:
		status = gdip_load_png_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
		break;
	case BMP:
		loader = dstream_input_new (getBytesFunc, seekFunc);
		status = gdip_load_bmp_image_from_stream_delegate (loader, &result);
		break;
	case TIF:
		status = gdip_load_tiff_image_from_stream_delegate (getBytesFunc, putBytesFunc,
			seekFunc, closeFunc, sizeFunc, &result);
		break;
	case GIF:
		status = gdip_load_gif_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
		break;
	case ICON:
		loader = dstream_input_new (getBytesFunc, seekFunc);
		status = gdip_load_ico_image_from_stream_delegate (loader, &result);
		break;
	case EMF:
		loader = dstream_input_new (getBytesFunc, seekFunc);
		status = gdip_load_emf_image_from_stream_delegate (loader, &result);
		break;
	case WMF:
		loader = dstream_input_new (getBytesFunc, seekFunc);
		status = gdip_load_wmf_image_from_stream_delegate (loader, &result);
		break;
	default:
		/* NotImplemented looks better but this matchs MS behavior */
		status = InvalidParameter;
		break;
	}

	if (result && (status == Ok))
		result->image_format = public_format;

	dstream_free (loader);
	*image = result;
	if (status != Ok) {
		*image = NULL;
	} else if (result && (result->type == ImageTypeBitmap) && !result->active_bitmap) {
		/* If the codec didn't set the active bitmap we will */
		gdip_bitmap_setactive(result, NULL, 0);
	}
	
	return status;
}

GpStatus WINGDIPAPI
GdipSaveImageToDelegate_linux (GpImage *image, GetBytesDelegate getBytesFunc, PutBytesDelegate putBytesFunc,
	SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, GDIPCONST CLSID *encoderCLSID,
	GDIPCONST EncoderParameters *params)
{
	if (!image || !encoderCLSID || (image->type != ImageTypeBitmap))
		return InvalidParameter;

	gdip_bitmap_flush_surface (image);

	switch (gdip_get_imageformat_from_codec_clsid ((CLSID *)encoderCLSID)) {
	case ICON:
	case BMP:
		return gdip_save_bmp_image_to_stream_delegate (putBytesFunc, image);
	case PNG:
		return gdip_save_png_image_to_stream_delegate (putBytesFunc, image, params);
	case JPEG:
		return gdip_save_jpeg_image_to_stream_delegate (putBytesFunc, image, params);
	case GIF:
		return gdip_save_gif_image_to_stream_delegate (putBytesFunc, image, params);
	case TIF:
		return gdip_save_tiff_image_to_stream_delegate (getBytesFunc, putBytesFunc,
			seekFunc, closeFunc, sizeFunc, image, params);
	case INVALID:
		return UnknownImageFormat;
	default:
		return NotImplemented;
	}
}

GpStatus
initCodecList (void)
{
	BYTE *dpos, *epos;
	
	g_decoder_list = dpos = GdipAlloc (sizeof (ImageCodecInfo) * DECODERS_SUPPORTED);
	if (!g_decoder_list)
		return OutOfMemory;

	g_encoder_list = epos = GdipAlloc (sizeof (ImageCodecInfo) * ENCODERS_SUPPORTED);
	if (!g_decoder_list) {
		GdipFree (g_decoder_list);
		g_decoder_list = NULL;
		return OutOfMemory;
	}
	
	/* BMP codec (encoder+decoder) - built-in */
	memcpy (dpos, gdip_getcodecinfo_bmp (), sizeof (ImageCodecInfo));
	dpos += sizeof (ImageCodecInfo);
	g_decoders++;
	memcpy (epos, gdip_getcodecinfo_bmp (), sizeof (ImageCodecInfo));
	epos += sizeof (ImageCodecInfo);
	g_encoders++;
	
	/* JPEG codec (encoder+decoder) */
	if (gdip_getcodecinfo_jpeg ()) {
		memcpy (dpos, gdip_getcodecinfo_jpeg(), sizeof (ImageCodecInfo));
		dpos += sizeof (ImageCodecInfo);
		g_decoders++;
		memcpy (epos, gdip_getcodecinfo_jpeg(), sizeof (ImageCodecInfo));
		epos += sizeof (ImageCodecInfo);
		g_encoders++;
	}
	
	/* GIF codec (encoder+decoder) */
	if (gdip_getcodecinfo_gif ()) {
		memcpy (dpos, gdip_getcodecinfo_gif (), sizeof (ImageCodecInfo));
		dpos += sizeof (ImageCodecInfo);
		g_decoders++;
		memcpy (epos, gdip_getcodecinfo_gif (), sizeof (ImageCodecInfo));
		epos += sizeof (ImageCodecInfo);
		g_encoders++;
	}

	/* EMF codec (decoder-only) */
	if (gdip_getcodecinfo_emf ()) {
		memcpy (dpos, gdip_getcodecinfo_emf (), sizeof (ImageCodecInfo));
		dpos += sizeof (ImageCodecInfo);
		g_decoders++;
	}

	/* WMF codec (decoder-only) */
	if (gdip_getcodecinfo_wmf ()) {
		memcpy (dpos, gdip_getcodecinfo_wmf (), sizeof (ImageCodecInfo));
		dpos += sizeof (ImageCodecInfo);
		g_decoders++;
	}
	
	/* TIFF codec (encoder+decoder) */
	if (gdip_getcodecinfo_tiff ()) {
		memcpy (dpos, gdip_getcodecinfo_tiff (), sizeof (ImageCodecInfo));
		dpos += sizeof (ImageCodecInfo);
		g_decoders++;
		memcpy (epos, gdip_getcodecinfo_tiff (), sizeof (ImageCodecInfo));
		epos += sizeof (ImageCodecInfo);
		g_encoders++;
	}
	
	/* PNG codec (encoder+decoder) */
	if (gdip_getcodecinfo_png ()) {
		memcpy (dpos, gdip_getcodecinfo_png (), sizeof (ImageCodecInfo));
		dpos += sizeof (ImageCodecInfo);
		g_decoders++;
		memcpy (epos, gdip_getcodecinfo_png (), sizeof (ImageCodecInfo));
		epos += sizeof (ImageCodecInfo);
		g_encoders++;
	}

	/* ICO codec (decoder-only) - built-in */
	memcpy (dpos, gdip_getcodecinfo_ico (), sizeof (ImageCodecInfo));
	dpos += sizeof (ImageCodecInfo);
	g_decoders++;

	return Ok;
}

void releaseCodecList (void)
{
	if (g_decoder_list) {
		GdipFree (g_decoder_list);
		g_decoder_list = NULL;
		g_decoders = 0;
	}
	if (g_encoder_list) {
		GdipFree (g_encoder_list);
		g_encoder_list = NULL;
		g_encoders = 0;
	}
}

GpStatus WINGDIPAPI
GdipGetImageDecodersSize (UINT *numDecoders, UINT *size)
{       
	if (!numDecoders || !size)
		return InvalidParameter;
	
	*numDecoders = g_decoders;
	*size = sizeof (ImageCodecInfo) * g_decoders;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetImageDecoders (UINT numDecoders, UINT size, ImageCodecInfo *decoders)
{
	if (!decoders || (numDecoders != g_decoders) || (size != sizeof (ImageCodecInfo) * g_decoders))
		return GenericError;

	memcpy (decoders, g_decoder_list, size);
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetImageEncodersSize (UINT *numEncoders, UINT *size)
{
	if (!numEncoders || !size)
		return InvalidParameter;
	
	*numEncoders = g_encoders;
	*size = sizeof (ImageCodecInfo) * g_encoders;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetImageEncoders (UINT numEncoders, UINT size, ImageCodecInfo *encoders)
{
	if (!encoders || (numEncoders != g_encoders) || (size != sizeof (ImageCodecInfo) * g_encoders))
		return GenericError;

	memcpy (encoders, g_encoder_list, size);
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetAllPropertyItems (GpImage *image, UINT totalBufferSize, UINT numProperties, PropertyItem *allItems)
{
	int		size;
	int		i;
	BYTE *ptr;

	if (!image || !allItems)
		return InvalidParameter;

	/* GDI+ doesn't support this for metafiles */
	if (image->type != ImageTypeBitmap)
		return NotImplemented;

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

	if (numProperties == 0) {
		return GenericError;
	}

	ptr = (BYTE*)allItems;
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

GpStatus WINGDIPAPI
GdipGetEncoderParameterListSize (GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT *size)
{
	ImageFormat fmt;

	if (!image || !clsidEncoder)
		return InvalidParameter;

	fmt = gdip_get_imageformat_from_codec_clsid ((CLSID *) clsidEncoder);

	switch (fmt) {
		case TIF:
			if (!size)
				return InvalidParameter;

			*size = sizeof (TiffEncoderParameters);
			return Ok;
		case GIF:
			if (!size)
				return InvalidParameter;

			*size = sizeof (GifEncoderParameters);
			return Ok;
		case PNG:
			if (!size)
				return InvalidParameter;

			*size = sizeof (PngEncoderParameters);
			return Ok;
		case JPEG:
			if (!size)
				return InvalidParameter;

			*size = sizeof (JpegEncoderParameters);
			return Ok;
		case BMP:
			*size = 0;
			return NotImplemented;
		default:
			break;
	}

	return FileNotFound;
}

GpStatus WINGDIPAPI
GdipGetEncoderParameterList (GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT size, EncoderParameters *buffer)
{
	ImageFormat fmt;

	if (!image || !clsidEncoder)
		return InvalidParameter;

	fmt = gdip_get_imageformat_from_codec_clsid ((CLSID *) clsidEncoder);

	switch (fmt) {
		case TIF:
			return gdip_fill_encoder_parameter_list_tiff (buffer, size);
		case GIF:
			return gdip_fill_encoder_parameter_list_gif (buffer, size);
		case PNG:
			return gdip_fill_encoder_parameter_list_png (buffer, size);
		case JPEG:
			return gdip_fill_encoder_parameter_list_jpeg (buffer, size);
		case BMP:
			return NotImplemented;
		default:
			break;
	}

	return FileNotFound;
}

GpStatus WINGDIPAPI
GdipGetImageThumbnail (GpImage *image, UINT thumbWidth, UINT thumbHeight, GpImage **thumbImage, GetThumbnailImageAbort callback, VOID *callbackData)
{
	GpStatus status;
	PixelFormat format;
	GpImage *result;
	GpGraphics *graphics;

	if (!image || !thumbImage)
		return InvalidParameter;

	if (!thumbWidth && !thumbHeight)
		thumbWidth = thumbHeight = 120;
	else if (!thumbWidth || !thumbHeight)
		return OutOfMemory;

	switch (image->type) {
	case ImageTypeBitmap:
		format = PixelFormat32bppPARGB;
		break;
	case ImageTypeMetafile:
		format = PixelFormat32bppARGB;
		break;
	default:
		return InvalidParameter;
	}

	status = GdipCreateBitmapFromScan0 (thumbWidth, thumbHeight, 0, format, NULL, (GpBitmap **) &result);
	if (status != Ok)
		return status;

	status = GdipGetImageGraphicsContext (result, &graphics);
	if (status != Ok) {
		GdipDisposeImage (result);
		return status;
	}

	status = GdipDrawImageRectI (graphics, image, 0, 0, thumbWidth, thumbHeight);
	if (status != Ok) {
		GdipDisposeImage (result);
		GdipDeleteGraphics (graphics);
		return status;
	}

	GdipDeleteGraphics (graphics);
	*thumbImage = result;
	return Ok;
}

/* coverity[+alloc : arg-*1] */
GpStatus WINGDIPAPI
GdipLoadImageFromFileICM (GDIPCONST WCHAR* filename, GpImage **image)
{
	return GdipLoadImageFromFile (filename, image);
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

GpStatus WINGDIPAPI
GdipSaveAdd (GpImage *image, GDIPCONST EncoderParameters* encoderParams)
{
	if (!image || !encoderParams)
		return InvalidParameter;
	return NotImplemented;
}

GpStatus WINGDIPAPI 
GdipSaveAddImage (GpImage *image, GpImage *imageNew, GDIPCONST EncoderParameters *params)
{
	if (!image || !imageNew || !params)
		return InvalidParameter;
	return NotImplemented;
}

GpStatus WINGDIPAPI
GdipImageForceValidation (GpImage *image)
{
	if (!image)
		return InvalidParameter;

	return Ok;
}
