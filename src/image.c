/*
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
 *
 */

#include <stdio.h>
#include "gdipImage.h"
#include <math.h>

void 
gdip_image_init(GpImage *image)
{
	image->type = imageUndefined;
	image->surface = 0;
	image->graphics = 0;
	image->imageFlags = 0;
	image->height = 0;
	image->width = 0;
	image->horizontalResolution = gdip_get_display_dpi ();
	image->palette = 0;
	image->pixFormat = Format32bppArgb;
	image->propItems = 0;
	image->verticalResolution = gdip_get_display_dpi ();
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
	printf("GdipDrawImageI. %p (type %d), %p, (%d,%d)\n", graphics, graphics->type, image, x, y);
	return NotImplemented;
}

GpStatus 
GdipDrawImageRectI (GpGraphics *graphics, GpImage *image, int x, int y, int width, int height)
{
	GpGraphics *image_graphics = 0;
	cairo_surface_t *image_surface = 0;
	
	if (!graphics || !image)
		return InvalidParameter;

	if (image->type != imageBitmap)
		return InvalidParameter;
	
	/* printf("GdipDrawImageRectI. %p (type %d), %p, (%d,%d) (%d,%d)\n", graphics, graphics->type, image, x, y, width, height); */
	
	GdipGetImageGraphicsContext (image, &image_graphics);
	if (image_graphics == 0) {
		printf("GdipDrawImageRectI. Error : cannot get graphics\n");
		return GenericError;
	}
	image_surface = cairo_current_target_surface (image_graphics->ct);
	if (image_surface == 0) {
		printf("GdipDrawImageRectI. Error : cannot get surface\n");
		return GenericError;
	}
	cairo_move_to (graphics->ct, x, y);
	cairo_set_pattern (graphics->ct, image_surface);
	cairo_rectangle (graphics->ct, x, y, width, height);
	cairo_fill (graphics->ct);
	
	return Ok;
}


/* GpStatus GdipLoadImageFromStream (IStream *stream, GpImage **image); */
/* GpStatus GdipSaveImageToStream (GpImage *image, IStream *stream, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params); */

GpStatus 
GdipLoadImageFromFile (GDIPCONST WCHAR *file, GpImage **image)
{
	FILE *fp = 0;
	GpImage *result = 0;
	GpStatus status = 0;
	ImageFormat format;
	unsigned char *file_name;

	if (!image || !file)
		return InvalidParameter;
 
	file_name = (unsigned char *) g_utf16_to_utf8 ((const gunichar2 *)file, -1, NULL, NULL, NULL);
	/*printf ("image.c, file name is %s \n", file_name);*/
	if ((fp = fopen(file_name, "rb")) == NULL) 
		return FileNotFound;

	/*printf ("came outof fopen, file pointer is not null \n");*/
	format = get_image_format (fp);
	
	switch (format) {
		case BMP:
			status = gdip_load_bmp_image_from_file (fp, result);
			if (status != Ok)
			{	
				fclose(fp);
				return status;
			}
			break;
		case TIFF:
			status = gdip_load_tiff_image_from_file (fp, result);
			if (status != Ok)
			{	
				fclose(fp);
				return status;
			}
			break;
		case GIF:
			status = gdip_load_gif_image_from_file (fp, result);
			if (status != Ok)
			{	
				fclose(fp);
				return status;
			}
			break;
		case PNG:
			status = gdip_load_png_image_from_file (fp, result);
			if (status != Ok)
			{	
				fclose(fp);
				return status;
			}
			break;
		case JPEG:
			status = gdip_load_jpeg_image_from_file (fp, result);
			if (status != Ok)
			{	
				fclose(fp);
				return status;
			}
			break;
		case EXIF:
			/*printf ("read EXIF \n");*/
		case WMF:
		case EMF:
		case ICON:			
		default:
			return NotImplemented | InvalidParameter;
	}
				
	fclose (fp);
	*image = result;
	return Ok;
}

GpStatus
GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params)
{
	FILE *fp = 0;
	GpStatus status = 0;
	unsigned char *file_name;
	
	if (!image || !file || !encoderCLSID || !params)
		return InvalidParameter;
   
	file_name = (unsigned char *) g_utf16_to_utf8 ((const gunichar2 *)file, -1, NULL, NULL, NULL);
	if ((fp = fopen(file_name, "wb")) == NULL)
	        return GenericError;

	return NotImplemented;
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
	return NotImplemented;
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
                return NotImplemented;
        case imageUndefined:
		break;
	default:
		break;
	}
        
	return Ok;
}


ImageFormat 
get_image_format (FILE *file)
{
	int index, sig_len, sig_num, inner_index;
	size_t size_read;
	char sig_read[10];
	char png[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, '\0'};
	char *signature[]  = { "BM", "MM", "II", "GIF", png, "\xff\xd8", "\xff\xd8\xff\xe1", "", "", ""};
	/*printf ("came in get_image_format \n");*/
	size_read = fread (sig_read, sizeof(char), 10, file);
	if (size_read != 10)
		return INVALID;
	/*printf("sig_read is %s \n", sig_read);*/
	/*printf ("size read is %d \n", size_read); */
	for (index = 0; index < 10; index ++) {
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
} 


int
gdip_getpixel_formatsize(PixelFormat pixfmt)
{
        int result = 0;
                                           
        switch (pixfmt) {
        case Format16bppArgb1555:
        case Format16bppGrayScale:
        case Format16bppRgb555:
        case Format16bppRgb565:
                result = 16;
                break;
        case Format1bppIndexed:
                result = 1;
                break;
        case Format24bppRgb:
                result = 24;
                break;
        case Format32bppArgb:
        case Format32bppPArgb:
        case Format32bppRgb:
                result = 32;
                break;
        case Format48bppRgb:
                result = 48;
                break;
        case Format4bppIndexed:
                result = 4;
                break;
        case Format64bppArgb:
        case Format64bppPArgb:
                result = 64;
                break;
        case Format8bppIndexed:
                result = 8;
                break;
        default:
                break;
        }

        return result;
}


