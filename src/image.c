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
 */

#include <stdio.h>
#include "gdipImage.h"
#include <math.h>
#include "gdip_win32.h"

static char *guid_to_string_hack (GDIPCONST CLSID *clsid);
void gdip_FlipX (GpImage *image);
void gdip_rotate_180_FlipX (GpImage *image);
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
gdip_image_init(GpImage *image)
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

void *
gdip_image_create_Win32_HDC (GpImage *image)
{
	void *result = 0;
	switch (image->type) {
		case imageBitmap:
			result = gdip_bitmap_create_Win32_HDC ((GpBitmap *) image);
			break;
		case imageMetafile:
			break;
		case imageUndefined:
			break;
		default:
			break;
	}
	return result;
}

void 
gdip_image_destroy_Win32_HDC (GpImage *image, void *hdc)
{
	if (!image || !hdc)
		return;
	
	switch (image->type) {
		case imageBitmap:
			gdip_bitmap_destroy_Win32_HDC ((GpBitmap *) image, hdc);
			break;
		case imageMetafile:
			break;
		case imageUndefined:
			break;
		default:
			break;
	}
}

GpStatus 
GdipDisposeImage (GpImage *image)
{
	if (!image)
		return InvalidParameter;

	cairo_surface_destroy (image->surface);
	image->surface = NULL;
	if (image->frameDimensionList != NULL){
		int i=0, j=0;
		int count = image->frameDimensionCount;
		for (i=0; i<count; i++){
			int dataCount =0;
			dataCount = image->frameDimensionList->count;
			BitmapData *data;
			data = image->frameDimensionList->frames;
			for (j = 0; j < dataCount; j++){
				if (data [j].Scan0){
					GdipFree (data [j].Scan0);
					data [j].Scan0 = NULL;
				}
				if ((data [j].ByteCount) > 0 && (data [j].Bytes != NULL)){
					GdipFree (data [j].Bytes);
					data [j].ByteCount = 0;
					data [j].Bytes = NULL;
				}

			}
			GdipFree (image->frameDimensionList->frames);
		}
		GdipFree (image->frameDimensionList);
	}

	/* Nothing more to be done here... We have already
	 * cleaned the memory while looping in FrameDimension List
	 * and hence we dont need to do anything in gdip_bitmap_dispose()
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
	if (!image || !graphics)
		return InvalidParameter;

	gfx = gdip_graphics_new ();
	if (image->type == imageBitmap) {
		gdip_graphics_attach_bitmap (gfx, (GpBitmap *) image);
	}
	else if (image->type == imageMetafile) {
	}

	*graphics = gfx;

	return Ok;
}

GpStatus 
GdipDrawImageI (GpGraphics *graphics, GpImage *image, int x, int y)
{
	return GdipDrawImageRect (graphics, image, x, y, image->width, image->height);
}

GpStatus 
GdipDrawImage (GpGraphics *graphics, GpImage *image, float x, float y)
{
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
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (image != NULL, InvalidParameter);
	g_return_val_if_fail (image->type == imageBitmap, InvalidParameter);
	
	/* Always to re-attach the image */	
	if (image->surface)  {
		cairo_surface_destroy (image->surface);
		image->surface = NULL;
	}

	/* Create a surface for this bitmap if one doesn't exist */    
	gdip_bitmap_ensure_surface ((GpBitmap*) image);
	cairo_surface_set_filter (((GpBitmap*) image)->image.surface, gdip_get_cairo_filter (graphics->interpolation));

	gdip_cairo_set_surface_pattern (graphics->ct, image->surface);
	cairo_translate (graphics->ct, x, y);
	cairo_rectangle (graphics->ct, 0, 0, width, height);
	
	if (width != image->width || height != image->height) {
		cairo_scale (graphics->ct,
				(double) width / image->width,
				(double) height / image->height);
		cairo_fill (graphics->ct);
	} else 
		cairo_fill (graphics->ct);
	
	cairo_default_matrix (graphics->ct);

	return Ok;
}

/* points are upper left, upper right, lower left. fourth point is extrapolated. */
GpStatus
GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, int count)
{
	float x, y, width, height;
	
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (image != NULL, InvalidParameter);
	g_return_val_if_fail (dstPoints != NULL, InvalidParameter);
	g_return_val_if_fail (count != 3, InvalidParameter);
	
	x = dstPoints[0].X;
	y = dstPoints[0].Y;
	width = dstPoints[1].X - dstPoints[0].X;
	height = dstPoints[2].Y - dstPoints[2].Y;
	
	return GdipDrawImageRect (graphics, image, x, y, width, height);
	}

GpStatus
GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, int count)
{
	GpPointF points[3];
	int i;
	
	g_return_val_if_fail (dstPoints != NULL, InvalidParameter);
	g_return_val_if_fail (count != 3, InvalidParameter);
	
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
	cairo_matrix_t *mat = NULL;
	void* dest, *org;
	bool allocated = FALSE;
	GpBitmap *bitmap = (GpBitmap *) image;
	
	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (image != NULL, InvalidParameter);
	g_return_val_if_fail (image->type == imageBitmap, InvalidParameter);
    
	if (srcUnit != UnitPixel && srcUnit != UnitWorld) {
		gdip_unitConversion(srcUnit, UnitPixel, dstx, &dstx);
		gdip_unitConversion(srcUnit, UnitPixel, dsty, &dsty);
		gdip_unitConversion(srcUnit, UnitPixel, dstwidth, &dstwidth);
		gdip_unitConversion(srcUnit, UnitPixel, dstheight, &dstheight);
		gdip_unitConversion(srcUnit, UnitPixel, srcx, &srcx);
		gdip_unitConversion(srcUnit, UnitPixel, srcy, &srcy);
		gdip_unitConversion(srcUnit, UnitPixel, srcwidth, &dstwidth);
		gdip_unitConversion(srcUnit, UnitPixel, srcheight, &srcheight);
	}

	org = dest = bitmap->data.Scan0; 
	gdip_process_bitmap_attributes (bitmap,  &dest,  (GpImageAttributes *) imageAttributes, &allocated);

	/* If there was a new Scan0 allocated, it means that we need it to change
	it's contents in order to apply the imageatributtes*/    
	
	if (allocated) 				
		bitmap->data.Scan0 = dest;
	
	/* Always to re-attach the image */	
	if (bitmap->image.surface)  {
		cairo_surface_destroy (bitmap->image.surface);
		bitmap->image.surface = NULL;
	}

	if (imageAttributes && imageAttributes->wrapmode != WrapModeClamp) {
		float img_x = srcx - dstx, img_y = srcy - dsty;
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
			gdip_FlipX ((GpImage *) imgflipX);	
			gdip_bitmap_ensure_surface (imgflipX);			
		}
		
		if (flipYOn) {			
			gdip_bitmap_clone (bitmap, &imgflipY);
			gdip_rotate_180_FlipX ((GpImage *) imgflipY);	
			gdip_bitmap_ensure_surface (imgflipY);			
		}
		
		if (flipXOn && flipYOn) {			
			gdip_bitmap_clone (bitmap, &imgflipXY);
			gdip_FlipX ((GpImage *)imgflipXY);	
			gdip_rotate_180_FlipX ((GpImage *) imgflipXY);	
			gdip_bitmap_ensure_surface (imgflipXY);			
		}
		
		/* Create a surface for this bitmap if one doesn't exist */
		gdip_bitmap_ensure_surface ((GpBitmap*) image);
		cairo_surface_set_filter (((GpBitmap*) image)->image.surface, gdip_get_cairo_filter (graphics->interpolation));

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
				
				mat = cairo_matrix_create ();
				cairo_matrix_scale (mat, srcwidth / dstwidth, srcheight / dstheight);
				cairo_matrix_translate (mat, srcx - (dstx + posx), srcy - (dsty + posy));
				cairo_surface_set_matrix (cur_image->image.surface, mat);	
				gdip_cairo_set_surface_pattern (graphics->ct, cur_image->image.surface);
				
				cairo_rectangle (graphics->ct, dstx + posx, dsty + posy, img_width, img_height);
				cairo_fill (graphics->ct);
				
				cairo_matrix_set_identity (mat);
				cairo_surface_set_matrix (cur_image->image.surface, mat);
				cairo_matrix_destroy (mat);			
				
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
		cairo_surface_set_filter (((GpBitmap*) image)->image.surface, gdip_get_cairo_filter (graphics->interpolation));
		
		mat = cairo_matrix_create ();
		cairo_matrix_scale (mat, srcwidth / dstwidth, srcheight / dstheight);
		cairo_matrix_translate (mat, srcx - dstx, srcy - dsty);
		cairo_surface_set_matrix (image->surface, mat);
		
		gdip_cairo_set_surface_pattern (graphics->ct, image->surface);
		
		cairo_rectangle (graphics->ct, dstx, dsty, dstwidth, dstheight);
		cairo_fill (graphics->ct);
		
		cairo_matrix_set_identity (mat);
		cairo_surface_set_matrix (image->surface, mat);
		
		/* unref */
		cairo_matrix_destroy (mat);
	}
	
	if (allocated) {
		bitmap->data.Scan0 = org;
		free (dest);	
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
	unsigned char *file_name = NULL;
	char format_peek[10];
	int format_peek_sz;
	
	GError *err = NULL;
	
	if (!image || !file)
		return InvalidParameter;
	
	file_name = (unsigned char *) g_utf16_to_utf8 ((const gunichar2 *)file, -1, NULL, NULL, &err);
	if (file_name == NULL || err != NULL) {
		*image = NULL;
		return InvalidParameter;
		}
	
	fp = fopen(file_name, "rb");
	g_free (file_name);
	if (fp == NULL)
		return FileNotFound;
	
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

ImageFormat 
gdip_get_imageformat_from_codec_clsid (CLSID *encoderCLSID)
{
	int numEncoders, size, cnt;
    	ImageCodecInfo *encoders, *encoder;

    	GdipGetImageEncodersSize (&numEncoders, &size);

    	if (numEncoders == 0)
        	return INVALID;

    	encoders = malloc (size);
    
    	GdipGetImageEncoders (numEncoders, size, encoders);

    	for (cnt = 0, encoder = encoders; cnt < numEncoders; encoder++) {
	       	if (memcmp (&encoder->Clsid, encoderCLSID, sizeof (GUID)) == 0) {
            		free (encoders);
            		return gdip_image_format_for_format_guid (&encoder->FormatID);
        	}
    	}

    	free (encoders);
    	return INVALID;
}

GpStatus
GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
	FILE *fp = 0;
	GpStatus status = 0;
	unsigned char *file_name;
	GError *err = NULL;
	ImageFormat format;
	
	if (image->type != imageBitmap)
		return InvalidParameter;
	
	if (!image || !file || !encoderCLSID)
		return InvalidParameter;
	
	format = gdip_get_imageformat_from_codec_clsid ( (CLSID *)encoderCLSID);
	if (format == INVALID)
		return UnknownImageFormat;
	
	file_name = (unsigned char *) g_utf16_to_utf8 ((const gunichar2 *)file, -1, NULL, NULL, &err);
	if (file_name == NULL || err != NULL) 
		return InvalidParameter;
	
	if (format == GIF) { /* gif library has to open the file itself*/
		status = gdip_save_gif_image_to_file (file_name, image);
		g_free (file_name);
		return status;
	}
	
	if ((fp = fopen(file_name, "wb")) == NULL)
		return GenericError;
		
	g_free (file_name);
	
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
	byte* guid;

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
	if (!image)
                return InvalidParameter;

	*count = image->frameDimensionCount;
	return Ok;
}

GpStatus 
GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count)
{
	if (!image || !dimensionGUID)
		return InvalidParameter;
	int i;
	int countReturn = image->frameDimensionCount;

	GUID guid [count];
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
	if (!image || !dimensionGUID)
		return InvalidParameter;
	int i;
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
        if (!image || !dimensionGUID || (index <0))
                return InvalidParameter;
	
	int i=0;
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

void copy_pixel (BYTE *src, BYTE *trg, int components)
{
	memcpy (trg, src, components);
}

void
gdip_rotate_180_FlipX (GpImage *image)
{
	BYTE *src, *trg, *line;
	int stride, height, i;	
	GpBitmap *bitmap = (GpBitmap *) image;
	
	stride = bitmap->data.Stride;
	height = bitmap->data.Height;
	line = malloc (stride);		
	src = (BYTE *) bitmap->data.Scan0;
	trg = (BYTE *) bitmap->data.Scan0;
	trg +=  (height-1) * stride;
	
	for (i = 0; i < (height /2); i++, src += stride, trg -= stride) {			
		memcpy (line, trg, stride);	/* Save target line*/		
		memcpy (trg, src, stride);	/* Copy src to trg*/		
		memcpy (src, line, stride);	/* Copy trg to src*/		
	}
	
	free (line);
}

void
gdip_rotate_90 (GpImage *image)
{
	BYTE *src, *trg, *rotated;
	int stride, height, line, x;	
	GpBitmap *bitmap = (GpBitmap *) image;
	int components = gdip_get_pixel_format_components (bitmap->data.PixelFormat);		         
	 
	bitmap = (GpBitmap *) image;	
        stride = bitmap->data.Stride;
	height = bitmap->data.Height;
	rotated = malloc (stride * height);	
	src = (BYTE *) bitmap->data.Scan0;
	
	for (line = 0; line < height; line++, src += stride) {		
		for (x = 0; x < bitmap->data.Width; x++) {		
			trg = rotated;	
			trg += x * stride;
			trg += (bitmap->data.Width - 1  - line) *components;		
			copy_pixel (src + (x*components), trg, components);			
		}	
	}	
	
	memcpy (bitmap->data.Scan0, rotated, stride * height);	
	free (rotated);
}

void
gdip_rotate_180 (GpImage *image)
{
	BYTE *src, *trg, *rotated;
	int stride, height, line, x;	
	GpBitmap *bitmap = (GpBitmap *) image;
	int components = gdip_get_pixel_format_components (bitmap->data.PixelFormat);		         
	 
	bitmap = (GpBitmap *) image;	
        stride = bitmap->data.Stride;
	height = bitmap->data.Height;
	rotated = malloc (stride * height);	
	src = (BYTE *) bitmap->data.Scan0;
	
	for (line = 0; line < height; line++, src += stride) {		
		for (x = 0; x < bitmap->data.Width; x++) {		
			trg = rotated;	
			trg += (bitmap->data.Height - 1  - line) * stride;
			trg += (bitmap->data.Width - 1  - x) * components;		
			copy_pixel (src + (x*components), trg, components);			
		}	
	}	
	
	memcpy (bitmap->data.Scan0, rotated, stride * height);	
	free (rotated);
}

void
gdip_rotate_270 (GpImage *image)
{
	BYTE *src, *trg, *rotated;
	int stride, height, line, x;	
	GpBitmap *bitmap = (GpBitmap *) image;
	int components = gdip_get_pixel_format_components (bitmap->data.PixelFormat);		         
	 
	bitmap = (GpBitmap *) image;	
        stride = bitmap->data.Stride;
	height = bitmap->data.Height;
	rotated = malloc (stride * height);
	
	src = (BYTE *) bitmap->data.Scan0;
	
	for (line = 0; line < height; line++, src += stride) {		
		for (x = 0; x < bitmap->data.Width; x++) {
		
			trg = rotated;	
			trg += (height-1-x)  * stride;
			trg += line * components;	
			copy_pixel (src + (x*components), trg, components);			
		}	
	}	
	
	memcpy (bitmap->data.Scan0, rotated, stride * height);	
	free (rotated);
}

void
gdip_FlipX (GpImage *image)
{
	BYTE *src, *trg, *rotated;
	int stride, height, line, x;	
	GpBitmap *bitmap = (GpBitmap *) image;
	int components = gdip_get_pixel_format_components (bitmap->data.PixelFormat);		         
	 
	bitmap = (GpBitmap *) image;	
        stride = bitmap->data.Stride;
	height = bitmap->data.Height;
	rotated = malloc (stride * height);
	
	src = (BYTE *) bitmap->data.Scan0;
	
	for (line = 0; line < height; line++, src += stride) {		
		for (x = 0; x < bitmap->data.Width; x++) {
		
			trg = rotated;	
			trg += (bitmap->data.Width-1-x)  * components;
			trg += line * stride;	
			copy_pixel (src + (x*components), trg, components);			
		}	
	}	
	
	memcpy (bitmap->data.Scan0, rotated, stride * height);	
	free (rotated);
}

GpStatus 
GdipImageRotateFlip (GpImage *image, RotateFlipType type)
{
	if (!image)
		return InvalidParameter;

	switch (type) {
                case RotateNoneFlipNone:
			break;
		case Rotate90FlipNone:
			gdip_rotate_90 (image);
			break;
		case Rotate180FlipNone:
			gdip_rotate_180 (image);
			break;
		case Rotate270FlipNone:	
			gdip_rotate_270 (image);
			break;
		case RotateNoneFlipX:
			gdip_FlipX (image);
			break;
		case Rotate90FlipX:
			gdip_rotate_90 (image);
			gdip_FlipX (image);
			break;
		case Rotate180FlipX:
			gdip_rotate_180_FlipX (image);
			break;
		case Rotate270FlipX:
			gdip_rotate_270 (image);
			gdip_FlipX (image);
			break;			
		default:
			return NotImplemented;
	}	
	
	return Ok;
}

GpStatus 
GdipGetImagePalette (GpImage *image, ColorPalette *palette, int size)
{
	return NotImplemented;
}

GpStatus 
GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette)
{
	return Ok;
}

GpStatus 
GdipGetImagePaletteSize (GpImage *image, int* size)
{
	return NotImplemented;
}

GpStatus 
GdipGetPropertyCount (GpImage *image, UINT *propertyNumber)
{
	return NotImplemented;
}

GpStatus 
GdipGetPropertyIdList (GpImage *image, UINT propertyNumber, PROPID *list)
{
	return NotImplemented;
}

GpStatus 
GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size)
{
	return NotImplemented;
}

GpStatus 
GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem buffer)
{
	return NotImplemented;
}

GpStatus 
GdipGetPropertySize (GpImage *image, UINT *bufferSize, UINT *propertyNumbers)
{
	return NotImplemented;
}

GpStatus 
GdipRemovePropertyItem (GpImage *image, PROPID propID)
{
	return NotImplemented;
}

GpStatus 
GdipSetPropertyItem(GpImage *image, GDIPCONST PropertyItem *item)
{
	return NotImplemented; /* GdipSetPropertyItem */
}

GpStatus
GdipCloneImage(GpImage *image, GpImage **cloneImage)
{
	if (!image || !cloneImage)
		return InvalidParameter;

	switch (image->type){
		case imageBitmap:
                	gdip_bitmap_clone ((GpBitmap *) image, (GpBitmap **) cloneImage);
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
	int index, sig_len, sig_num, inner_index;
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
GdipLoadImageFromDelegate_linux (GetBytesDelegate getBytesFunc,
				 PutBytesDelegate putBytesFunc,
                                 SeekDelegate seekFunc,
				 CloseDelegate closeFunc,
				 SizeDelegate sizeFunc,
                                 GpImage **image)
{
	GpImage *result = 0;
	GpStatus status = 0;
	ImageFormat format;
	
	char format_peek[10];
	int format_peek_sz;
	
	format_peek_sz = getBytesFunc (format_peek, 10, 1);
	format = get_image_format (format_peek, format_peek_sz);
	
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

static char *
guid_to_string_hack (GDIPCONST GUID *guid)
{
	static char buf[1024];
	snprintf (buf, 1024, "%08lx-%04x-%04x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
		  guid->Data1, guid->Data2, guid->Data3,
		  guid->Data4[0], guid->Data4[1], guid->Data4[2], guid->Data4[3], guid->Data4[4], guid->Data4[5], guid->Data4[6], guid->Data4[7]);
	return buf;
}

#define CODECS_SUPPORTED 5
static BYTE *g_codeclist;
static int g_codecs = 0; /* Available codecs */

void initCodecList (void)
{
	BYTE *pos;
	
	g_codeclist = pos = malloc (sizeof (ImageCodecInfo) * CODECS_SUPPORTED);
	
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
		free (g_codeclist);
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
	return(NotImplemented);
}

GpStatus
GdipGetEncoderParameterListSize(GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT *size)
{
	g_return_val_if_fail (clsidEncoder != NULL, InvalidParameter);
	g_return_val_if_fail (size != NULL, InvalidParameter);

	ImageFormat fmt = gdip_get_imageformat_from_codec_clsid ((CLSID *) clsidEncoder);

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
	g_return_val_if_fail (clsidEncoder != NULL, InvalidParameter);
	g_return_val_if_fail (buffer != NULL, InvalidParameter);

	ImageFormat fmt = gdip_get_imageformat_from_codec_clsid ((CLSID *) clsidEncoder);

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
	return (GdipLoadImageFromFileICM(filename, image));
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
