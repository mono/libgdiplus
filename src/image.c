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
 *
 */

#include "gdip.h"
#include "gdipImage.h"
#include <math.h>

void 
gdip_image_init (GpImage *image)
{
	image->type = imageUndefined;
	image->surface = 0;
	image->graphics = 0;
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
	}
	return result;
}

void 
gdip_image_destroy_Win32_HDC (GpImage *image, void *hdc)
{
	switch (image->type) {
	case imageBitmap:
		gdip_bitmap_destroy_Win32_HDC ((GpBitmap *) image, hdc);
		break;
	case imageMetafile:
		break;
	case imageUndefined:
		break;
	}
}

GpStatus 
GdipDisposeImage (GpImage *image)
{
	switch (image->type){
	case imageBitmap:
		gdip_bitmap_dispose ((GpBitmap *) image);
		break;
	case imageMetafile:
		break;
	case imageUndefined:
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
	return NotImplemented;
}

/* GpStatus GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params); */
/* GpStatus GdipSaveAddImage (GpImage *image, GpImage *imageNew, GDIPCONST EncoderParameters *params); */

GpStatus 
GdipGetImageBounds (GpImage *image, GpRectF *rect, GpUnit *unit)
{
	return NotImplemented;
}

GpStatus 
GdipGetImageDimension (GpImage *image, float *width, float *height)
{
	return NotImplemented;
}

GpStatus 
GdipGetImageType (GpImage *image, ImageType *type)
{
	return NotImplemented;
}

GpStatus 
GdipGetImageWidth (GpImage *image, UINT *width)
{
	return NotImplemented;
}

GpStatus 
GdipGetImageHeight (GpImage *image, UINT *heigth)
{
	return NotImplemented;
}

GpStatus 
GdipGetImageHorizontalResolution (GpImage *image, float *resolution)
{
	return NotImplemented;
}

GpStatus 
GdipGetImageVerticalResolution (GpImage *image, float *resolution)
{
	return NotImplemented;
}

GpStatus 
GdipGetImageFlags (GpImage *image, UINT *flags)
{
	return NotImplemented;
}

/* GpStatus GdipGetImageRawFormat (GpImage *image, GUID *format); */

GpStatus 
GdipGetImagePixelFormat (GpImage *image, PixelFormat *format)
{
	return NotImplemented;
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
/* GpStatus GdipImageGetFrameCount (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count); */
/* GpStatus GdipImageSelectActiveFrame (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT index); */

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
	return NotImplemented;
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


