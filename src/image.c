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
#include <math.h>

static GpStatus gdip_flip_x (GpImage *image);
static GpStatus gdip_flip_y (GpImage *image);
cairo_filter_t gdip_get_cairo_filter (InterpolationMode imode);

/*
 * format guids
 */

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
	image->type = imageUndefined;
	image->surface = 0;
	image->imageFlags = 0;
	image->height = 0;
	image->width = 0;
	image->palette = 0;
	image->pixFormat = Format32bppArgb;
	image->propItems = 0;
	image->horizontalResolution = 0;
	image->verticalResolution = 0;
	image->format = 0;
	image->frameDimensionCount = 0;
	image->frameDimensionList = NULL;
} 


GpStatus 
GdipDisposeImage (GpImage *image)
{
	int i = 0, j = 0, count = 0, dataCount = 0;
	BitmapData *data;

	if (!image)
		return InvalidParameter;

	if (image->surface)
		cairo_surface_destroy (image->surface);
	image->surface = NULL;

	count = image->frameDimensionCount;

	if (count > 0 && image->frameDimensionList != NULL) {
		for (i = 0; i < count; i++) {
			dataCount = image->frameDimensionList [i].count;
			data = image->frameDimensionList [i].frames;
				for (j = 0; j < dataCount; j++) {
					if (((data [j].Reserved & GBD_OWN_SCAN0) != 0) && data [j].Scan0 != NULL) {
						GdipFree (data [j].Scan0);
						data [j].Scan0 = NULL;
					}
					if ((data [j].ByteCount) > 0 && (data [j].Bytes != NULL)) {
						GdipFree (data [j].Bytes);
						data [j].ByteCount = 0;
						data [j].Bytes = NULL;
					}
				}
		}
		GdipFree (image->frameDimensionList);
	}
	
	/* Nothing more to be done here... We have already
	 * cleaned the memory while looping in FrameDimension List
	 * and hence we dont need to do anything in gdip_bitmap_dispose ()
	 */
	 GdipFree (image);

	 return Ok;
}

/*
	Microsoft GDI+ only supports these pixel formats Format24bppRgb, Format32bppArgb, 
	Format32bppPArgb, Format32bppRgb, Format48bppRgb, Format64bppArgb, Format64bppPArgb
*/
GpStatus 
GdipGetImageGraphicsContext (GpImage *image, GpGraphics **graphics)
{
	GpGraphics *gfx;
	cairo_surface_t *surface;
	GpBitmap* bmp = (GpBitmap *) image;
	
	if (!image || !graphics)
		return InvalidParameter;

	if (image->type != imageBitmap)
		return NotImplemented;	
	
	surface = cairo_image_surface_create_for_data ((unsigned char *) bmp->data.Scan0, bmp->cairo_format,
				bmp->data.Width, bmp->data.Height, bmp->data.Stride);

	gfx = gdip_graphics_new (surface);
	gfx->dpi_x = gfx->dpi_y = gdip_get_display_dpi ();
	cairo_surface_destroy (surface);

	gfx->image = bmp;
	gfx->type = gtMemoryBitmap;	
	cairo_pattern_set_filter (cairo_pattern_create_for_surface (bmp->image.surface), gdip_get_cairo_filter (gfx->interpolation));
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
	cairo_ps_surface_set_dpi (surface, dpix, dpiy);

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
	if (!image)
		return InvalidParameter;

	return GdipDrawImageRect (graphics, image, x, y, image->width, image->height);
}

GpStatus 
GdipDrawImage (GpGraphics *graphics, GpImage *image, float x, float y)
{
	if (!image)
		return InvalidParameter;

	return GdipDrawImageRect (graphics, image, x, y, image->width, image->height);
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
	
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (image != NULL, InvalidParameter);
	g_return_val_if_fail (image->type == imageBitmap, InvalidParameter);

	x = gdip_unitx_convgr (graphics, x);
	y = gdip_unity_convgr (graphics, y);
	width = gdip_unitx_convgr (graphics, width);
	height = gdip_unity_convgr (graphics, height);
	
	cairo_new_path(graphics->ct);

	if (gdip_is_an_indexed_pixelformat (((GpBitmap*) image)->data.PixelFormat)) {
		/* Unable to create a surface for the bitmap; it is an indexed image.
		 * Instead, it will first be converted to 32-bit RGB.
		 */
		GpStatus status = OutOfMemory;

		GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb ((GpBitmap *) image);

		if (rgb_bitmap != NULL) {
			status = GdipDrawImageRect(graphics, (GpImage *)rgb_bitmap, x, y, width, height);
			GdipDisposeImage((GpImage *)rgb_bitmap);
		}

		return status;
	}

	/* Create a surface for this bitmap if one doesn't exist */
	gdip_bitmap_ensure_surface ((GpBitmap *) image);
	pattern = cairo_pattern_create_for_surface (((GpBitmap*) image)->image.surface);
	cairo_pattern_set_filter (pattern,
				  gdip_get_cairo_filter (graphics->interpolation));

	cairo_translate (graphics->ct, x, y);

	if (width != image->width || height != image->height) {
		cairo_scale (graphics->ct,
				(double) width / image->width,
				(double) height / image->height);
	}
	
	cairo_set_source_surface (graphics->ct, image->surface, 0, 0);
	cairo_identity_matrix (graphics->ct);
	cairo_paint (graphics->ct);
	
	cairo_pattern_destroy (pattern);
	
	return Ok;
}

/* points are upper left, upper right, lower left. fourth point is extrapolated. */
GpStatus
GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, int count)
{
	float width, height;
	cairo_pattern_t *pattern;
	
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (image != NULL, InvalidParameter);
	g_return_val_if_fail (dstPoints != NULL, InvalidParameter);
	g_return_val_if_fail (count == 3, InvalidParameter);

	if (gdip_is_an_indexed_pixelformat (((GpBitmap*) image)->data.PixelFormat)) {
		/* Unable to create a surface for the bitmap; it is an indexed image.
		 * Instead, it will first be converted to 32-bit RGB.
		 */
		GpStatus status = OutOfMemory;

		GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb ((GpBitmap *) image);

		if (rgb_bitmap != NULL) {
			status = GdipDrawImagePoints (graphics, (GpImage *)rgb_bitmap, dstPoints, count);
			GdipDisposeImage((GpImage *)rgb_bitmap);
		}
		return status;
	}

	cairo_new_path(graphics->ct);

	width = dstPoints[1].X > dstPoints[0].X ? dstPoints[1].X - dstPoints[0].X : dstPoints[0].X - dstPoints[1].X;
	height = dstPoints[2].Y > dstPoints[0].Y ? dstPoints[2].Y - dstPoints[0].Y : dstPoints[0].Y - dstPoints[2].Y;

	/* Create a surface for this bitmap if one doesn't exist */
	gdip_bitmap_ensure_surface ((GpBitmap *) image);
	pattern = cairo_pattern_create_for_surface (((GpBitmap*) image)->image.surface);
	cairo_pattern_set_filter (pattern,
				  gdip_get_cairo_filter (graphics->interpolation));

	cairo_translate (graphics->ct, dstPoints[0].X, dstPoints[0].Y);

	/*
		TODO: Implement rotation
	*/

	cairo_scale (graphics->ct,
		(double) width / image->width,
		(double) height / image->height);
	
	cairo_set_source_surface (graphics->ct, image->surface, 0, 0);
	cairo_identity_matrix (graphics->ct);
	cairo_paint (graphics->ct);	
	cairo_pattern_destroy (pattern);
	
	return Ok;
}

GpStatus
GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, int count)
{
	GpPointF points[3];
	int i;
	
	g_return_val_if_fail (dstPoints != NULL, InvalidParameter);
	g_return_val_if_fail (count == 3, InvalidParameter);
	
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
	return NotImplemented; /* GdipDrawImagePointRect */
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
	cairo_pattern_t *pattern;
	cairo_matrix_t mat;
	void* dest, *org;
	bool allocated = FALSE;
	GpBitmap *bitmap = (GpBitmap *) image;
	
	cairo_matrix_init (&mat, 1, 0, 0, 1, 0, 0);

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (image != NULL, InvalidParameter);
	g_return_val_if_fail (image->type == imageBitmap, InvalidParameter);

	if (gdip_is_an_indexed_pixelformat (((GpBitmap *)image)->data.PixelFormat)) {
                /* Unable to create a surface for the bitmap; it is an indexed image.
                 * Instead, it will first be converted to 32-bit RGB.
                 */
		GpStatus status = OutOfMemory;

		GpBitmap *rgb_bitmap = gdip_convert_indexed_to_rgb ((GpBitmap *)image);

		if (rgb_bitmap != NULL) {
			status = GdipDrawImageRectRect (graphics, (GpImage *)rgb_bitmap,
				dstx, dsty, dstwidth, dstheight,
				srcx, srcy, srcwidth, srcheight,
				srcUnit, imageAttributes, callback, callbackData);
			GdipDisposeImage ((GpImage *)rgb_bitmap);
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

	org = dest = bitmap->data.Scan0; 
	gdip_process_bitmap_attributes (bitmap,  &dest,  (GpImageAttributes *) imageAttributes, &allocated);

	/* If there was a new Scan0 allocated, it means that we need it to change
	it's contents in order to apply the imageatributtes*/    
	
	if (allocated) 				
		bitmap->data.Scan0 = dest;
	
	/* Re-attach the image if the image has attribues */
	if ((bitmap->image.surface && imageAttributes != NULL) || bitmap->image.surface == NULL)  {
		cairo_surface_destroy (bitmap->image.surface);
		bitmap->image.surface = NULL;
	}

	if (imageAttributes && imageAttributes->wrapmode != WrapModeClamp) {
		float img_width = bitmap->data.Width *  (dstwidth / srcwidth) , img_height = bitmap->data.Height * (dstheight / srcheight);
		float posx, posy;
		bool flipXOn = (imageAttributes->wrapmode == WrapModeTileFlipX) ? TRUE: FALSE;
		bool flipYOn = (imageAttributes->wrapmode == WrapModeTileFlipY) ? TRUE: FALSE;
		bool flipX = FALSE, flipY = FALSE;
		GpBitmap *cur_image, *imgflipX = NULL, *imgflipY = NULL, *imgflipXY = NULL;
		
		if (imageAttributes->wrapmode == WrapModeTileFlipXY) 
			flipXOn = flipYOn = TRUE;
		
		if (flipXOn) {			
			gdip_bitmap_clone (bitmap, &imgflipX);
			gdip_flip_x ((GpImage *) imgflipX);	
			gdip_bitmap_ensure_surface (imgflipX);			
		}
		
		if (flipYOn) {			
			gdip_bitmap_clone (bitmap, &imgflipY);
			gdip_flip_y ((GpImage *) imgflipY);	
			gdip_bitmap_ensure_surface (imgflipY);			
		}
		
		if (flipXOn && flipYOn) {			
			gdip_bitmap_clone (bitmap, &imgflipXY);
			gdip_flip_x ((GpImage *)imgflipXY);	
			gdip_flip_y ((GpImage *) imgflipXY);	
			gdip_bitmap_ensure_surface (imgflipXY);			
		}
		
		/* Create a surface for this bitmap if one doesn't exist */
		gdip_bitmap_ensure_surface ((GpBitmap*) image);
		cairo_pattern_set_filter (cairo_pattern_create_for_surface(((GpBitmap*) image)->image.surface), gdip_get_cairo_filter (graphics->interpolation));

		for (posy = 0; posy < dstheight; posy += img_height) {
		
			for (posx = 0; posx < dstwidth; posx += img_width) {
			
				if (flipX && flipY)
					cur_image = imgflipXY;				
				else			
					if (flipX) 
						cur_image = imgflipX;				
					else
						if (flipY) 
							cur_image = imgflipY;									 
						else 
							cur_image = bitmap;
				

				cairo_matrix_scale (&mat, srcwidth / dstwidth, srcheight / dstheight);
				cairo_matrix_translate (&mat, srcx - (dstx + posx), srcy - (dsty + posy));

				pattern = cairo_pattern_create_for_surface(cur_image->image.surface);
				cairo_pattern_set_matrix (pattern, &mat);
				cairo_set_source(graphics->ct, pattern);
				
				cairo_rectangle (graphics->ct, dstx + posx, dsty + posy, img_width, img_height);
				cairo_fill (graphics->ct);
				
				cairo_matrix_init_identity (&mat);
				cairo_pattern_set_matrix (pattern, &mat);

				cairo_pattern_destroy(pattern);
				
				if (flipXOn)
					flipX = !flipX; 					
			
			}
					
			if (flipYOn)
				flipY = !flipY; 
		}	
		
		if (imgflipX)
			GdipDisposeImage ((GpImage *) imgflipX);	
			
		if (imgflipY)
			GdipDisposeImage ((GpImage *) imgflipY);
			
		if (imgflipXY)
			GdipDisposeImage ((GpImage *) imgflipXY);
		
	}
	else  {		
		/* Create a surface for this bitmap if one doesn't exist */
		gdip_bitmap_ensure_surface ((GpBitmap*) image);
		cairo_pattern_set_filter (cairo_pattern_create_for_surface (((GpBitmap*) image)->image.surface) , gdip_get_cairo_filter (graphics->interpolation));
		
		cairo_matrix_scale (&mat, srcwidth / dstwidth, srcheight / dstheight);
		cairo_matrix_translate (&mat, srcx - dstx, srcy - dsty);

		pattern = cairo_pattern_create_for_surface(image->surface);
		cairo_pattern_set_matrix (pattern, &mat);
		cairo_set_source(graphics->ct, pattern);
		
		cairo_rectangle (graphics->ct, dstx, dsty, dstwidth, dstheight);
		cairo_fill (graphics->ct);
		
		cairo_matrix_init_identity (&mat);
		cairo_pattern_set_matrix (pattern, &mat);
		cairo_pattern_destroy (pattern);
	}

	/* The default surface is no longer valid*/
	if (bitmap->image.surface && imageAttributes != NULL)  {
		cairo_surface_destroy (bitmap->image.surface);
		bitmap->image.surface = NULL;
	}
	
	if (allocated) {
		bitmap->data.Scan0 = org;
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
	FILE *fp = 0;
	GpImage *result = 0;
	GpStatus status = 0;
	ImageFormat format;
	char *file_name = NULL;
	char format_peek[10];
	int format_peek_sz;
	
	if (!image || !file)
		return InvalidParameter;
	
	file_name = (char *) ucs2_to_utf8 ((const gunichar2 *)file, -1);
	if (file_name == NULL) {
		*image = NULL;
		return InvalidParameter;
	}
	
	fp = fopen(file_name, "rb");
	GdipFree (file_name);
	if (fp == NULL)
		return InvalidParameter;
	
	format_peek_sz = fread (format_peek, 1, 10, fp);
	format = get_image_format (format_peek, format_peek_sz);
	fseek (fp, 0, SEEK_SET);
	
	switch (format) {
		case BMP:
			status = gdip_load_bmp_image_from_file (fp, &result);
			if (result != NULL)
				result->format = BMP;
			break;
		case TIF:
			status = gdip_load_tiff_image_from_file (fp, &result);
			if (result != NULL)
				result->format = TIF;
			break;
		case GIF:
			status = gdip_load_gif_image_from_file (fp, &result);
			if (result != NULL)
				result->format = GIF;
			break;
		case PNG:
			status = gdip_load_png_image_from_file (fp, &result);
			if (result != NULL)
				result->format = PNG;
			break;
		case JPEG:
			status = gdip_load_jpeg_image_from_file (fp, &result);
			if (result != NULL)
				result->format = JPEG;
			break;
		case EXIF:
		case WMF:
		case EMF:
		case ICON:
		default:
			status = NotImplemented;
	}
	
	fclose (fp);
	
	if (status != Ok) {
		*image = NULL;
	} else {
		if (result->frameDimensionCount == 0) {
			result->frameDimensionCount = 1;
			result->frameDimensionList = (FrameInfo *) GdipAlloc (sizeof (FrameInfo));
			result->frameDimensionList[0].count = 1; /*multiple frames are already taken care of in respectic codecs*/
			memcpy (&(result->frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
			result->frameDimensionList[0].frames = &(((GpBitmap *) result)->data);
		}
		*image = result;
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
            		GdipFree (encoders);
            		return gdip_image_format_for_format_guid (&encoder->FormatID);
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
	
	if (image->type != imageBitmap)
		return InvalidParameter;
	
	if (!image || !file || !encoderCLSID)
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
	}
	
	if ((fp = fopen(file_name, "wb")) == NULL)
		return GenericError;
		
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
		case TIF:
			status = gdip_save_tiff_image_to_file (fp, image, params);
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
	if (!image || !rect || !unit)           
		return InvalidParameter;

	rect->X = 0;
	rect->Y = 0;
	rect->Height = image->height;
	rect->Width = image->width;
	*unit = UnitPixel;

	return Ok;
}

GpStatus 
GdipGetImageDimension (GpImage *image, float *width, float *height)
{
	if (!image || !width || !height)
		return InvalidParameter;

	*width = image->width;
	*height = image->height;

	return Ok;
}

GpStatus 
GdipGetImageType (GpImage *image, ImageType *type)
{
	if (!image || !type)
		return InvalidParameter;

	*type = image->type;	
	return Ok;
}

GpStatus 
GdipGetImageWidth (GpImage *image, UINT *width)
{
	if (!image || !width)
		return InvalidParameter;

	*width = image->width;
	return Ok;
}

GpStatus 
GdipGetImageHeight (GpImage *image, UINT *height)
{
	if (!image || !height)
		return InvalidParameter;

	*height = image->height;
	return Ok;
}

GpStatus 
GdipGetImageHorizontalResolution (GpImage *image, float *resolution)
{
	if (!image || !resolution)
		return InvalidParameter;
	
	*resolution = image->horizontalResolution;
	return Ok;
}

GpStatus 
GdipGetImageVerticalResolution (GpImage *image, float *resolution)
{
	if (!image || !resolution)
		return InvalidParameter;

	*resolution = image->verticalResolution;
	return Ok;
}

GpStatus 
GdipGetImageFlags (GpImage *image, UINT *flags)
{
	if (!image || !flags)
		return InvalidParameter;

	*flags = image->imageFlags;
	return Ok;
}

GpStatus GdipGetImageRawFormat (GpImage *image, GUID *format)
{
	if (!image || !format)
		return InvalidParameter;
	
	switch (image->format) {
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
        	default:
            		return InvalidParameter;
    	}
}

GpStatus 
GdipGetImagePixelFormat (GpImage *image, PixelFormat *format)
{
	if (!image || !format)
		return InvalidParameter;

	*format = image->pixFormat;
	return Ok;
}

GpStatus 
GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count)
{
	if (!image || !count)
                return InvalidParameter;

	*count = image->frameDimensionCount;
	return Ok;
}

GpStatus 
GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count)
{
	int i;
	int countReturn;
	GUID guid [count];

	if (!image || !dimensionGUID)
		return InvalidParameter;

	countReturn = image->frameDimensionCount;
	
	if (countReturn < count)
		countReturn = count;
	for (i=0; i<countReturn; i++){
		guid [i] = image->frameDimensionList[i].frameDimension;
	}

	memcpy (dimensionGUID, guid, sizeof (CLSID)*countReturn);
	return Ok;
}

GpStatus
GdipImageGetFrameCount(GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count)
{
	int i;

	if (!image || !dimensionGUID || !count)
		return InvalidParameter;

	for (i=0; i<image->frameDimensionCount; i++){
		if (memcmp(dimensionGUID, &(image->frameDimensionList[i].frameDimension), sizeof(CLSID)) == 0) {
			*count = image->frameDimensionList[i].count;
			return Ok;
		}
	}

        return InvalidParameter;
}

GpStatus
GdipImageSelectActiveFrame(GpImage *image, GDIPCONST GUID *dimensionGUID, UINT index)
{
	int i=0;

        if (!image || !dimensionGUID || (index <0))
                return InvalidParameter;
		
	for (i=0; i<image->frameDimensionCount; i++) {
		if (memcmp(dimensionGUID, &(image->frameDimensionList[i].frameDimension), sizeof(CLSID)) == 0) {
			if (image->frameDimensionList[i].count >= index){
				GpBitmap *bitmap = (GpBitmap *) image;
				bitmap->data = image->frameDimensionList[i].frames[index];
				/*printf ("\n image.c selected frame is %d returning ok", index);*/
				return Ok;
			} else {
				return InvalidParameter;
				/*printf ("\n image.c not selected frame is %d and returning error", index);*/
			}
		}
	}
        return InvalidParameter;        
}

void copy_pixel (BYTE *src, BYTE *trg, int size)
{
	memcpy (trg, src, size);
}

static GpStatus
gdip_flip_x (GpImage *image)
{
	BYTE *src, *line;
	int stride, width, height, pixel_size, i, j;
	GpBitmap *bitmap = (GpBitmap *) image;
	
	stride = bitmap->data.Stride;
	width = bitmap->data.Width;
	height = bitmap->data.Height;
	pixel_size = gdip_get_pixel_format_components (bitmap->data.PixelFormat) * gdip_get_pixel_format_depth (bitmap->data.PixelFormat) / 8;
	line = GdipAlloc (stride);
	src = (BYTE *) bitmap->data.Scan0;

	if (line == NULL)
		return OutOfMemory;
	
	for (i = 0; i < height; i++, src += stride) {			
		memcpy (line, src, stride);	/* Save original line*/

		for  (j = 0; j < width; j++)
			copy_pixel(&line[(width - j - 1) * pixel_size], &src[j * pixel_size], pixel_size);
	}
	
	GdipFree (line);

	return Ok;
}

static GpStatus
gdip_flip_y (GpImage *image)
{
	BYTE *src, *trg, *line;
	int stride, height, i;	
	GpBitmap *bitmap = (GpBitmap *) image;
	
	stride = bitmap->data.Stride;
	height = bitmap->data.Height;
	line = GdipAlloc (stride);		
	src = (BYTE *) bitmap->data.Scan0;
	trg = (BYTE *) bitmap->data.Scan0;
	trg +=  (height-1) * stride;

	if (line == NULL)
		return OutOfMemory;
	
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
	GpBitmap *bitmap = (GpBitmap *) image;

	BYTE *rotated;

	BYTE *source, *target;
	int x, y;
	int source_stride, source_height, source_width, source_pixel_delta, source_interscan_delta;
	int target_stride, target_height, target_width, target_pixel_delta, target_interscan_delta;
	int initial_source_offset, initial_target_offset;

	int components = gdip_get_pixel_format_components (bitmap->data.PixelFormat);
	int depth = gdip_get_pixel_format_depth (bitmap->data.PixelFormat);

	int pixel_size = components * depth / 8;

	source_stride = bitmap->data.Stride;
	source_width = bitmap->data.Width;
	source_height = bitmap->data.Height;

	source_pixel_delta = pixel_size;
	source_interscan_delta = source_stride - source_width * pixel_size;
	initial_source_offset = 0;

	if (angle == 180) {
		target_width = source_width;
		target_height = source_height;
	}
	else {
		target_height = source_width; /* swap width & height here */
		target_width = source_height;
	}

	target_stride = target_width * pixel_size;
	target_stride = (target_stride + 3) & ~3;

	switch (angle)
	{
		case 90:
			target_pixel_delta = target_stride;
			if (flip_x) {
				target_interscan_delta = +pixel_size - target_stride * target_height;
				initial_target_offset = 0;
			}
			else {
				target_interscan_delta = -pixel_size - target_stride * target_height;
				initial_target_offset = (target_width - 1) * pixel_size;
			}
			break;
		case 180:
			if (flip_x) {
				target_pixel_delta = +pixel_size;
				target_interscan_delta = -target_width * pixel_size - target_stride; 
				initial_target_offset = (target_height - 1) * target_stride;
			}
			else {
				target_pixel_delta = -pixel_size;
				target_interscan_delta = +target_width * pixel_size - target_stride;
				initial_target_offset = (target_height - 1) * target_stride + (target_width - 1) * pixel_size;
			}
			break;
		case 270:
			target_pixel_delta = -target_stride;
			if (flip_x) {
				target_interscan_delta = target_stride * target_height - pixel_size;
				initial_target_offset = (target_height - 1) * target_stride + (target_width - 1) * pixel_size;
			}
			else {
				target_interscan_delta = target_stride * target_height + pixel_size;
				initial_target_offset = (target_height - 1) * target_stride;
			}
			break;
		default:
			if (flip_x)
				return gdip_flip_x (image);
			else
				return Ok;
	}

	rotated = GdipAlloc (target_height * target_stride);

	if (rotated == NULL)
		return OutOfMemory;

	source = initial_source_offset + (BYTE *) bitmap->data.Scan0;
	target = initial_target_offset + rotated;

	for (y = 0; y < source_height;
             y++,
             source += source_interscan_delta,
             target += target_interscan_delta)
		for (x = 0; x < source_width;
                     x++,
                     source += source_pixel_delta,
                     target += target_pixel_delta)
			copy_pixel (source, target, pixel_size);

	bitmap->data.Stride = target_stride;
	bitmap->data.Height = image->height = target_height;
	bitmap->data.Width = image->width = target_width;

	if ((bitmap->data.Reserved & GBD_OWN_SCAN0) != 0) {
		int k, j; 

		void *old_scan0 = bitmap->data.Scan0;

		for (j = 0; j < image->frameDimensionCount; j++) {
			for (k = 0; k < image->frameDimensionList [j].count; k++) {
				if (image->frameDimensionList[j].frames[k].Scan0 == old_scan0)  {
					image->frameDimensionList[j].frames[k].Scan0 = rotated;
					image->frameDimensionList[j].frames[k].Stride = target_stride;
					image->frameDimensionList[j].frames[k].Height = image->height = target_height;
					image->frameDimensionList[j].frames[k].Width = image->width = target_width;
				}
			}
		}

		GdipFree (old_scan0);
	}

	bitmap->data.Scan0 = rotated;
	bitmap->data.Reserved |= GBD_OWN_SCAN0;	

	if (image->surface != NULL) {
		cairo_surface_destroy (image->surface);
		image->surface = NULL;
	}

	return Ok;
}

static GpStatus
gdip_rotate_flip_packed_indexed (GpImage *image, PixelFormat pixel_format, int angle, BOOL flip_x)
{
	GpBitmap *bitmap = (GpBitmap *)image;
	BitmapData *data = &bitmap->data;
	BYTE *rotated;
	StreamingState scan[8];
	BOOL scan_valid[8];
	int x, y, i;

	int bits_per_pixel = gdip_get_pixel_format_depth (pixel_format);
	int pixels_per_byte = 8 / bits_per_pixel;

	int source_width = data->Width;
	int source_height = data->Height;

	/* Swap the width & height if needed */
	BOOL aspect_inversion = ((angle % 180) != 0);

	int target_width = aspect_inversion ? source_height : source_width;
	int target_height = aspect_inversion ? source_width : source_height;

	int target_scan_size = (target_width + pixels_per_byte - 1) / pixels_per_byte;
	int target_stride = (target_scan_size + 3) & ~3;

	if ((angle == 180) && flip_x)
		return gdip_flip_y(image);

	rotated = GdipAlloc (target_height * target_stride);

	if (rotated == NULL)
		return OutOfMemory;

	if (aspect_inversion == FALSE) {
		int x_alignment = ((source_width - 1) % pixels_per_byte) + 1 - pixels_per_byte;
		int x_step = pixels_per_byte;

		StreamingState stream;

		BOOL target_starts_at_bottom = (angle == 180);

		int target_x_offset = target_scan_size - 1;
		int target_y_offset = target_starts_at_bottom ? (target_height - 1) * target_stride : 0;
		int target_y_offset_delta = target_starts_at_bottom ? -target_stride : +target_stride;

		Status status = gdip_init_pixel_stream (&stream, data, 0, 0, data->Width, data->Height);

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

					if ((index >= 0) && (index < source_width))
						byte |= (gdip_pixel_stream_get_next (&stream) << 8);
					byte >>= bits_per_pixel;
				}

				*target = byte;
				target--;
			}
		}
	}
	else {
		int y_alignment = ((source_height - 1) % pixels_per_byte) + 1 - pixels_per_byte;
		int y_step = pixels_per_byte;
		int byte_column = 0;

		int target_delta;

		BOOL target_starts_at_bottom = (angle == 270);
		BOOL target_starts_at_left = (target_starts_at_bottom ^ flip_x);

		if (target_starts_at_left)
			y_alignment = 0;

		if (target_starts_at_bottom)
			target_delta = -target_stride;
		else
			target_delta = +target_stride;

		for (y = y_alignment; y < source_height; y += y_step) {
			BYTE *target;

			for (i=0; i < pixels_per_byte; i++) {
				int scan_index = y + i;

				scan_valid[i] = ((scan_index >= 0) && (scan_index < source_height));

				if (scan_valid[i]) {
					Status status = gdip_init_pixel_stream (&scan[i], data, 0, scan_index, source_width, 1);

					if (status != Ok) {
						GdipFree (rotated);
						return status;
					}
				}
			}

			target = rotated;

			if (target_starts_at_left)
				target += byte_column;
			else
				target += target_scan_size - byte_column - 1;

			if (target_starts_at_bottom)
				target += (target_height - 1) * target_stride;

			for (x = 0; x < source_width; x++) {
				int byte = 0;

				if (target_starts_at_left)
					for (i=0; i < pixels_per_byte; i++)
					{
						byte <<= bits_per_pixel;
						if (scan_valid[i])
							byte |= gdip_pixel_stream_get_next (&scan[i]);
					}
				else
					for (i = pixels_per_byte - 1; i >= 0; i--)
					{
						byte <<= bits_per_pixel;
						if (scan_valid[i])
							byte |= gdip_pixel_stream_get_next (&scan[i]);
					}

				*target = byte;
				target += target_delta;
			}

			byte_column++;
		}
	}

	bitmap->data.Stride = target_stride;
	bitmap->data.Height = image->height = target_height;
	bitmap->data.Width = image->width = target_width;

	if ((bitmap->data.Reserved & GBD_OWN_SCAN0) != 0) {
		int k, j; 

		void *old_scan0 = bitmap->data.Scan0;

		for (j = 0; j < image->frameDimensionCount; j++) {
			for (k = 0; k < image->frameDimensionList [j].count; k++) {
				if (image->frameDimensionList[j].frames[k].Scan0 == old_scan0)  {
					image->frameDimensionList[j].frames[k].Scan0 = rotated;
					image->frameDimensionList[j].frames[k].Stride = target_stride;
					image->frameDimensionList[j].frames[k].Height = image->height = target_height;
					image->frameDimensionList[j].frames[k].Width = image->width = target_width;
				}
			}
		}

		GdipFree (old_scan0);
	}

	bitmap->data.Scan0 = rotated;
	bitmap->data.Reserved |= GBD_OWN_SCAN0;	

	/* It shouldn't be possible for an indexed image to have one,
	 * but if it does, it needs to be killed.
	 */
	if (image->surface != NULL) {
		cairo_surface_destroy (image->surface);
		image->surface = NULL;
	}

	return Ok;
}

GpStatus 
GdipImageRotateFlip (GpImage *image, RotateFlipType type)
{
	int angle;
	BOOL flip_x;
	PixelFormat pixel_format;

	if (!image)
		return InvalidParameter;

	pixel_format = ((GpBitmap *)image)->data.PixelFormat;


	angle = flip_x = 0;

	switch (type) {
                case RotateNoneFlipNone: /* equivalent to Rotate180FlipXY */
			return Ok;
		case Rotate90FlipNone: /* equivalent to Rotate270FlipXY */
			angle = 90;
			break;
		case RotateNoneFlipXY: /* equivalent to Rotate180FlipNone */
			angle = 180;
			break;
		case Rotate90FlipXY: /* equivalent to Rotate270FlipNone */
			angle = 270;
			break;
		case RotateNoneFlipX: /* equivalent to Rotate180FlipY */
			flip_x = TRUE;
			break;
		case Rotate90FlipX: /* equivalent to Rotate270FlipY */
			angle = 90;
			flip_x = TRUE;
			break;
		case RotateNoneFlipY: /* equivalent to Rotate180FlipX */
			return gdip_flip_y (image);
		case Rotate90FlipY: /* equivalent to Rotate270FlipX */
			angle = 270;
			flip_x = TRUE;
			break;			
		default:
			return NotImplemented;
	}	
	
	if (gdip_is_an_indexed_pixelformat (pixel_format)
         && (gdip_get_pixel_format_depth (pixel_format) < 8))
		return gdip_rotate_flip_packed_indexed (image, pixel_format, angle, flip_x);
	else
		return gdip_rotate_orthogonal_flip_x (image, angle, flip_x);
}

GpStatus 
GdipGetImagePalette (GpImage *image, ColorPalette *palette, int size)
{
	int palette_entries;
	int bytes_needed;

	if ((image == NULL) || (palette == NULL))
		return InvalidParameter;

	if (image->palette == NULL)
		return InvalidParameter;

	palette_entries = image->palette->Count;

	if ((image->type == imageBitmap) && (((GpBitmap *)image)->data.PixelFormat == Format4bppIndexed))
		palette_entries = 16;

	bytes_needed = palette_entries * sizeof(ARGB) + sizeof(ColorPalette) - sizeof(ARGB);

	if (bytes_needed > size)
		return InvalidParameter;

	memcpy(palette, image->palette, bytes_needed);
	return Ok;
}

GpStatus 
GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette)
{
	int entries_to_copy;

	if ((image == NULL) || (palette == NULL))
		return InvalidParameter;

	if (image->palette == NULL)
		return InvalidParameter;

	entries_to_copy = image->palette->Count;
	if (entries_to_copy > palette->Count)
		entries_to_copy = palette->Count;

	memcpy(image->palette->Entries, palette->Entries, entries_to_copy * sizeof(ARGB));
	return Ok;
}

GpStatus 
GdipGetImagePaletteSize (GpImage *image, int* size)
{
        int palette_entries;

        if ((image == NULL) || (size == NULL))
                return InvalidParameter;

        if (image->palette == NULL)
                return InvalidParameter;

        palette_entries = image->palette->Count;

        if ((image->type == imageBitmap) && (((GpBitmap *)image)->data.PixelFormat == Format4bppIndexed))
                palette_entries = 16;

        *size = palette_entries * sizeof(ARGB) + sizeof(ColorPalette) - sizeof(ARGB);
	return Ok;
}

GpStatus 
GdipGetPropertyCount (GpImage *image, UINT *propertyNumber)
{
	/*
	 * We currently only return 1 fake property 0xdeadbeee
	 */
	return 1;
}

GpStatus 
GdipGetPropertyIdList (GpImage *image, UINT propertyNumber, PROPID *list)
{
	return NotImplemented;
}

GpStatus 
GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size)
{
	*size = 16;
	return Ok;
}

GpStatus 
GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem buffer)
{
	if (propID != 0xdeadbeee)
		return InvalidParameter;

	return Ok;
}

GpStatus 
GdipGetPropertySize (GpImage *image, UINT *bufferSize, UINT *propertyNumbers)
{
	if (!image || !bufferSize || !propertyNumbers)
		return InvalidParameter;

	*bufferSize = 32;
	*propertyNumbers = 1;
	return Ok;
}

GpStatus 
GdipRemovePropertyItem (GpImage *image, PROPID propID)
{
	/* Not implemented */
	return Ok;
}

GpStatus 
GdipSetPropertyItem(GpImage *image, GDIPCONST PropertyItem *item)
{
	/* Not implemented */
	return Ok;
}

void
gdip_image_clone (GpImage* image, GpImage* clonedImage)
{
	int i = 0, j = 0, dataCount = 0;
	BitmapData *data, *clonedData;

	clonedImage->surface = NULL;

	if (image->frameDimensionCount) {
		clonedImage->frameDimensionCount = image->frameDimensionCount;
		clonedImage->frameDimensionList = GdipAlloc (sizeof (FrameInfo) * image->frameDimensionCount);

		for (i = 0; i < image->frameDimensionCount; i++) {
			clonedImage->frameDimensionList[i].count = image->frameDimensionList[i].count;
			memcpy (&clonedImage->frameDimensionList[i].frameDimension, 
				&image->frameDimensionList[i].frameDimension, sizeof (GUID));

			dataCount = image->frameDimensionList[i].count;
			data = image->frameDimensionList[i].frames;
			clonedImage->frameDimensionList[i].frames = GdipAlloc (sizeof (BitmapData) * dataCount);
			clonedData = clonedImage->frameDimensionList[i].frames;
			/* Copy all BitmapData */
			memcpy (clonedImage->frameDimensionList[i].frames, 
				image->frameDimensionList[i].frames,  sizeof (BitmapData) * dataCount);

			for (j = 0; j < dataCount; j++) {
				if (data[j].Scan0) {
					clonedData[j].Scan0 = GdipAlloc (data[j].Stride * data[j].Height);
					memcpy (clonedData[j].Scan0, data[j].Scan0, data[j].Stride * data[j].Height);				
				}
				if ((data[j].ByteCount) > 0 && (data[j].Bytes != NULL)) {				
					clonedData[j].Bytes = GdipAlloc (data[j].ByteCount);
					memcpy (clonedData[j].Bytes, data[j].Bytes, data[j].ByteCount);
				}
			}
		}
	}
}

GpStatus
GdipCloneImage(GpImage *image, GpImage **cloneImage)
{
	if (!image || !cloneImage)
		return InvalidParameter;

	switch (image->type){
		case imageBitmap:
                	gdip_bitmap_clone ((GpBitmap *) image, (GpBitmap **) cloneImage);
			gdip_image_clone (image, *cloneImage);
			break;
		case imageMetafile:
        	        return NotImplemented; /* GdipCloneImage - imageMetafile */
        	case imageUndefined:
			break;
		default:
			break;
	}
        
	return Ok;
}

/*  FIXME - this function makes me uneasy. */
ImageFormat 
get_image_format (char *sig_read, size_t size_read)
{
	int index;
	char png[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, '\0'};
	char *signature[]  = { "BM", "MM", "II", "GIF", png, "\xff\xd8", "\xff\xd8\xff\xe1", "", "", ""};

	if (size_read < 10)
			return INVALID;

	for (index = 0; index < size_read; index ++) {
		if ((signature[index][0] == sig_read[0]) && (signature[index][1] == sig_read[1])) {
			switch (index) {
				case 0 :	
					return BMP;
				case 1:
				case 2:
					return TIF;
				case 3:
					if (signature[index][2] == sig_read[2]) 
						return GIF;
					else
						return INVALID;
				case 4:
					if (strncmp(signature[index], sig_read, 8) == 0) 
						return PNG;						
					else
						return INVALID;					
				case 5:		
				case 6: if (strncmp(sig_read + 2, "\xff\e1", 2) == 0)
						if (strncmp(sig_read + 6, "Exif", 4) == 0)
							return EXIF;
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
	int result = 0;
	                                           
	switch (pixfmt) {
		case Format16bppArgb1555:
		case Format16bppGrayScale:
		case Format16bppRgb555:
		case Format16bppRgb565:
		case Format24bppRgb:
		case Format32bppArgb:
		case Format32bppPArgb:
		case Format32bppRgb:
			result = 8;
			break;
		case Format48bppRgb:
		case Format64bppArgb:
		case Format64bppPArgb:
			result = 16;
			break;
		case Format8bppIndexed:
			result = 8;
			break;
		case Format4bppIndexed:
			result = 4;
			break;
		case Format1bppIndexed:
			result = 1;
			break;
		default:
			break;
	}

	return result;
}

int
gdip_get_pixel_format_components(PixelFormat pixfmt)
{
	int result = 0;
						
	switch (pixfmt) {
		case Format16bppArgb1555:
		case Format32bppArgb:
		case Format32bppPArgb:
		case Format64bppArgb:
		case Format64bppPArgb:
		case Format24bppRgb:	/* Cairo uses for bytes for 24BPP*/
		case Format32bppRgb:	/* Cairo uses for bytes for 32BPP*/
			result = 4;
			break;
		case Format16bppRgb555:
		case Format16bppRgb565: 
		case Format48bppRgb:
			result = 3;
			break;
		case Format16bppGrayScale:
		case Format8bppIndexed:
		case Format4bppIndexed:
		case Format1bppIndexed:
			result = 1;
			break;
		default:
			break;
	}
	
	return result;
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
		case JPEG:
			status = gdip_load_jpeg_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL)
				result->format = JPEG;
			break;
		case PNG:
			status = gdip_load_png_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL)
				result->format = PNG;
			break;
		case BMP:
			status = gdip_load_bmp_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL)
				result->format = BMP;
			break;
		case TIF:
			status = gdip_load_tiff_image_from_stream_delegate (getBytesFunc, putBytesFunc,
								seekFunc, closeFunc, sizeFunc, &result);
			if (result != NULL)
				result->format = TIF;
			break;
		case GIF:
			status = gdip_load_gif_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
			if (result != NULL)
				result->format = GIF;            
			break;
		default:
			printf ("type: %d Not implemented\n", format);
			status = NotImplemented;
			break;
	}

	if (status != Ok) {
		*image = NULL;
	} else {
		if (result->frameDimensionCount == 0){
			result->frameDimensionCount = 1;
			result->frameDimensionList = (FrameInfo *) GdipAlloc (sizeof (FrameInfo));
			result->frameDimensionList[0].count = 1; /*multiple frames are already taken care of in respectic codecs*/
			memcpy (&(result->frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
			result->frameDimensionList[0].frames = &(((GpBitmap *) result)->data);
		}
		*image = result;
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

    	if (image->type != imageBitmap)
        	return InvalidParameter;

    	if (!image || !encoderCLSID)
        	return InvalidParameter;

    	format = gdip_get_imageformat_from_codec_clsid ((CLSID *)encoderCLSID);
    	if (format == INVALID)
        	return UnknownImageFormat;

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
	if (!allItems || (totalBufferSize < 16))
		return InvalidParameter;

	allItems [0].id = 0xdeadbeee;
	allItems [0].length = 4;
	/* A byte */
	allItems [0].type = 1;
	allItems [0].value = &(allItems [0].id);
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
