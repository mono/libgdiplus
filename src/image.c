/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: nil; -*-
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
 *
 */

#include <stdio.h>
#include "gdipImage.h"
#include <math.h>

static char *clsid_to_string_hack (GDIPCONST CLSID *clsid);

void 
gdip_image_init(GpImage *image)
{
	image->type = imageUndefined;
	image->surface = 0;
	image->graphics = 0;
	image->imageFlags = 0;
	image->height = 0;
	image->width = 0;
	image->palette = 0;
	image->pixFormat = Format32bppArgb;
	image->propItems = 0;
	image->horizontalResolution = 0;
	image->verticalResolution = 0;
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

	switch (image->type){
	case imageBitmap:
		gdip_bitmap_dispose ((GpBitmap *) image);
		break;
	case imageMetafile:
                break;
	case imageUndefined:
		break;
	default:
		break;
	}
	cairo_surface_destroy (image->surface);
	image->surface = 0;
	GdipFree (image);
	
	return Ok;
}

GpStatus 
GdipGetImageGraphicsContext (GpImage *image, GpGraphics **graphics)
{
	if (!image || !graphics)
		return InvalidParameter;

	if (image->graphics == 0) {
		image->graphics = gdip_graphics_new ();
		if (image->type == imageBitmap) {
			gdip_graphics_attach_bitmap (image->graphics, (GpBitmap *) image);
		}
		else if (image->type == imageMetafile) {
		}
	}
	*graphics = image->graphics;
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
    g_return_val_if_fail (image->surface != NULL, InvalidParameter);

    if (!graphics || !image)
        return InvalidParameter;

    if (image->type != imageBitmap)
        return InvalidParameter;
    
    cairo_move_to (graphics->ct, x, y);
    gdip_cairo_set_surface_pattern (graphics->ct, image->surface);
    cairo_rectangle (graphics->ct, x, y, width, height);

    if (width != image->width || height != image->height) {
        cairo_scale (graphics->ct,
                     (double) width / image->width,
                     (double) height / image->height);
        cairo_fill (graphics->ct);
        cairo_default_matrix (graphics->ct);
    } else {
        cairo_fill (graphics->ct);
    }

    return Ok;
}

GpStatus
GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, int count)
{
    return NotImplemented; /* GdipDrawImagePoints */
}

GpStatus
GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, int count)
{
    return NotImplemented; /* GdipDrawImagePointsI */
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
    return NotImplemented; /* GdipDrawImageRectRect */
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
            break;
        case TIFF:
            status = gdip_load_tiff_image_from_file (fp, &result);
            break;
        case GIF:
            status = gdip_load_gif_image_from_file (fp, &result);
            break;
        case PNG:
            status = gdip_load_png_image_from_file (fp, &result);
            break;
        case JPEG:
            status = gdip_load_jpeg_image_from_file (fp, &result);
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
        *image = result;
    }

    return status;
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

    format = gdip_image_format_for_clsid (encoderCLSID);
    if (format == INVALID)
        return UnknownImageFormat;
   
    file_name = (unsigned char *) g_utf16_to_utf8 ((const gunichar2 *)file, -1, NULL, NULL, &err);
    if (file_name == NULL || err != NULL) {
        return InvalidParameter;
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
        default:
            status = NotImplemented;
            break;
    }

    fclose (fp);
    
    return status;
}

/* GpStatus GdipSaveAddImage (GpImage *image, GpImage *imageNew, GDIPCONST EncoderParameters *params); */

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
        switch (image->type){
	case imageBitmap:
		gdip_bitmap_dispose ((GpBitmap *) image);
		break;
	case imageMetafile:
		break;
	case imageUndefined:
		break;
	default:
		break;
	}
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

/* GpStatus GdipGetImageRawFormat (GpImage *image, GUID *format); */

GpStatus 
GdipGetImagePixelFormat (GpImage *image, PixelFormat *format)
{
	if (!image || !format)
		return InvalidParameter;

	*format = image->pixFormat;
	return Ok;
}

/*GpStatus GdipGetImageThumbnail (GpImage *image, UINT width, UINT height, GpImage **thumbImage, GetThumbnailImageAbort callback, VOID* callBackData);*/
/* GpStatus GetEncoderParameterListSize (GpImage *image, GDIPCONST CLSID *encoderCLSID, UINT *size); */
/* GpStatus GetEncoderParameterList (GpImage *image, GDIPCONST CLSID *encoderCLSID, UINT size, EncoderParameters *buffer); */

GpStatus 
GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count)
{
	return NotImplemented;
}

/* GpStatus GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count); */

GpStatus
GdipImageGetFrameCount(GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count)
{
        if (!image || !dimensionGUID || !count)
                return InvalidParameter;

        *count = 1;
        return Ok;

}

GpStatus
GdipImageSelectActiveFrame(GpImage *image, GDIPCONST GUID *dimensionGUID, UINT index)
{
        if (!image || !dimensionGUID)
                return InvalidParameter;

        return Ok;
        
}

GpStatus 
GdipImageRotateFlip (GpImage *image, RotateFlipType type)
{
	return NotImplemented;
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
GdipRemoveProperyItem (GpImage *image, PROPID propID)
{
	return NotImplemented;
}

GpStatus 
GdipSetProperyItem (GpImage *image, GDIPCONST PropertyItem *item)
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
					return TIFF;
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
            result = 4;
            break;
        case Format16bppRgb555:
        case Format16bppRgb565:
        case Format24bppRgb:
        case Format32bppRgb:
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
                                 SeekDelegate seekFunc,
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
            break;
        case PNG:
            status = gdip_load_png_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
            break;
        case BMP:
            status = gdip_load_bmp_image_from_stream_delegate (getBytesFunc, seekFunc, &result);
            break;
        case TIFF:
        case GIF:
        default:
            status = NotImplemented;
    }

    if (status != Ok) {
        *image = NULL;
    } else {
        *image = result;
    }

    return status;
}

GpStatus
GdipSaveImageToDelegate_linux (GpImage *image, PutBytesDelegate putBytesFunc, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
    GpStatus status = 0;
    ImageFormat format;

    if (image->type != imageBitmap)
        return InvalidParameter;

    if (!image || !encoderCLSID)
        return InvalidParameter;

    format = gdip_image_format_for_clsid (encoderCLSID);
    if (format == INVALID)
        return UnknownImageFormat;
   
    switch (format) {
        case PNG:
            status = gdip_save_png_image_to_stream_delegate (putBytesFunc, image, params);
            break;
        case JPEG:
            status = gdip_save_jpeg_image_to_stream_delegate (putBytesFunc, image, params);
            break;
        default:
            status = NotImplemented;
            break;
    }

    return status;
}


/*
 * encoder clsid's
 */

CLSID gdip_bmp_image_format_guid = {0xb96b3cabU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
CLSID gdip_jpg_image_format_guid = {0xb96b3caeU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
CLSID gdip_png_image_format_guid = {0xb96b3cafU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
CLSID gdip_gif_image_format_guid = {0xb96b3cb0U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
CLSID gdip_tif_image_format_guid = {0xb96b3cb1U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

ImageFormat
gdip_image_format_for_clsid (GDIPCONST CLSID *encoderCLSID)
{
    if (memcmp(encoderCLSID, &gdip_bmp_image_format_guid, sizeof(CLSID)) == 0)
        return BMP;
    if (memcmp(encoderCLSID, &gdip_jpg_image_format_guid, sizeof(CLSID)) == 0)
        return JPEG;
    if (memcmp(encoderCLSID, &gdip_png_image_format_guid, sizeof(CLSID)) == 0)
        return PNG;
    if (memcmp(encoderCLSID, &gdip_gif_image_format_guid, sizeof(CLSID)) == 0)
        return GIF;
    if (memcmp(encoderCLSID, &gdip_tif_image_format_guid, sizeof(CLSID)) == 0)
        return TIFF;

    return INVALID;
}

static char *
clsid_to_string_hack (GDIPCONST CLSID *clsid)
{
    static char buf[1024];
    snprintf (buf, 1024, "%08lx-%04x-%04x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
              clsid->Data1, clsid->Data2, clsid->Data3,
              clsid->Data4[0], clsid->Data4[1], clsid->Data4[2], clsid->Data4[3], clsid->Data4[4], clsid->Data4[5], clsid->Data4[6], clsid->Data4[7]);
    return buf;
}
