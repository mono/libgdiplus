/*
 * bitmap.c
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
 *   Alexandre Pigolkine (pigolkine@gmx.de)
 *   Vladimir Vukicevic (vladimir@pobox.com)
 *   Jordi Mas (jordi@ximian.com)
 */

#include <glib.h>
#include "gdip.h"
#include "gdipImage.h"
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
	Those are the only pixel formats that we really support	
*/
BOOL
gdip_is_a_supported_pixelformat (PixelFormat fmt)
{
	switch (fmt) {
	
 	case Format24bppRgb:
	case Format32bppArgb:
	case Format32bppPArgb:
	case Format32bppRgb:
		return TRUE;
	default:
		return FALSE;
	}
}

void 
gdip_bitmap_init (GpBitmap *bitmap)
{
	gdip_image_init (&bitmap->image);

	bitmap->image.type = imageBitmap;
	bitmap->data.Width = 0;
	bitmap->data.Height = 0;
	bitmap->data.Stride = 0;
	bitmap->data.PixelFormat = Format32bppArgb;
	bitmap->data.Scan0 = 0;
	bitmap->data.Reserved = 0;
	bitmap->data.ByteCount = 0;
	bitmap->data.Bytes = NULL;
	bitmap->data.Left = 0;
	bitmap->data.Top = 0;
	bitmap->cairo_format = CAIRO_FORMAT_ARGB32; 
		
	bitmap->hBitmapDC = 0;
	bitmap->hInitialBitmap = 0;
	bitmap->hBitmap = 0;
}



void
gdip_bitmap_clone (GpBitmap *bitmap, GpBitmap **clonedbitmap)
{
	GpBitmap *result = (GpBitmap *) GdipAlloc (sizeof (GpBitmap));	
	memcpy (result, bitmap, sizeof (GpBitmap));
	
	result->data.Scan0 = malloc (bitmap->data.Stride * bitmap->data.Height);
	memcpy (result->data.Scan0, bitmap->data.Scan0, bitmap->data.Stride * bitmap->data.Height);
	*clonedbitmap = result;

	if (bitmap->data.ByteCount > 0 && bitmap->data.Bytes != NULL){
		result->data.Bytes = malloc (bitmap->data.ByteCount);
		if (result->data.Bytes == NULL)
			bitmap->data.ByteCount = 0;
		else
			memcpy (result->data.Bytes, bitmap->data.Bytes, bitmap->data.ByteCount);
	}
	
	result->image.surface = NULL;
	
	/*TODO: We should also copy palette info when we support it*/
}


GpBitmap *
gdip_bitmap_new ()
{
	GpBitmap *result = (GpBitmap *) GdipAlloc (sizeof (GpBitmap));

	if (result)
		gdip_bitmap_init (result);

	return result;
}

/*
 * This should only be called from GdipDisposeImage, and it should *not* free
 * the structure, that one is freed by GdipDisposeImage
 */
void 
gdip_bitmap_dispose (GpBitmap *bitmap)
{
	if (((bitmap->data.Reserved & GBD_OWN_SCAN0) != 0) && bitmap->data.Scan0 != NULL) {
		GdipFree (bitmap->data.Scan0);
		bitmap->data.Scan0 = NULL;
	}

	if ((bitmap->data.ByteCount > 0) && (bitmap->data.Bytes != NULL)) {
		GdipFree (bitmap->data.Bytes);
		bitmap->data.Bytes = NULL;
	}
}


GpStatus 
GdipCreateBitmapFromScan0 (int width, int height, int stride, int format, void *scan0, GpBitmap **bitmap)
{
	GpBitmap *result = 0;
        bool own_scan0 = FALSE;
	int cairo_format = 0;
	
	switch (format) {
	case Format24bppRgb:
		cairo_format = CAIRO_FORMAT_RGB24;  
		break;
	case Format32bppArgb:
	case Format32bppRgb:
	case Format32bppPArgb:
		cairo_format = CAIRO_FORMAT_ARGB32;
		break;
	case Format8bppIndexed:
		cairo_format = CAIRO_FORMAT_A8;        
		break;
	case Format1bppIndexed:
		cairo_format = CAIRO_FORMAT_A1;
		break;
	default:
		*bitmap = NULL;
		return NotImplemented;
	}

	result = gdip_bitmap_new ();
	if (result == NULL)
		return OutOfMemory;
			
	result->data.Width = result->image.width = width;
	result->data.Height = result->image.height = height;
	result->data.PixelFormat = result->image.pixFormat = format;
	result->cairo_format = cairo_format;

	if (stride == 0) {
		int bpp  = gdip_get_pixel_format_components (format);
		bpp = bpp * gdip_get_pixel_format_depth (format);
		stride = (bpp * width) / 8;
		stride += (stride + (sizeof(pixman_bits_t)-1));
		stride &= ~(sizeof(pixman_bits_t)-1);
	}

	if (scan0 == NULL) {
		scan0 = GdipAlloc (stride * height);
		if (scan0 == NULL)
			return OutOfMemory;
		
		memset (scan0, 0, stride * height);
		own_scan0 = TRUE;
	}
	
	
	result->data.Scan0 = scan0;
	result->data.Stride = stride;
	
	if (own_scan0)
		result->data.Reserved |= GBD_OWN_SCAN0;

	/* Initialize the image frames */
	if (result->image.frameDimensionCount == 0) {
		result->image.frameDimensionCount = 1;
			result->image.frameDimensionList = (FrameInfo *) GdipAlloc (sizeof (FrameInfo));
			result->image.frameDimensionList[0].count = 1; /*multiple frames are already taken care of in respectic codecs*/
			memcpy (&(result->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
			result->image.frameDimensionList[0].frames = &(((GpBitmap *) result)->data);
		}

	*bitmap = result;

	return Ok;
}

GpStatus
GdipCreateBitmapFromGraphics (int width, int height, GpGraphics *graphics, GpBitmap **bitmap)
{
	GpBitmap 		*result = 0;
	int 			bmpSize = 0;
	int 			stride = width;
	/*
	 * FIXME: should get the stride based on the format of the graphics object.
	 */
	stride *= 4;

	while (stride % sizeof(pixman_bits_t))
		stride++;
	
	bmpSize = stride * height;
	result = gdip_bitmap_new ();
	result->data.Width = result->image.width = width;
	result->data.Height = result->image.height = height;
	result->data.PixelFormat = result->image.pixFormat = Format32bppArgb;
	result->data.Stride = stride;

	result->data.Scan0 = GdipAlloc (bmpSize);
	if (result->data.Scan0 == NULL){
		GdipFree (result);
		return OutOfMemory;
	}

	result->data.Reserved |= GBD_OWN_SCAN0;
	*bitmap = result;
	return Ok;
}

GpStatus
GdipCreateBitmapFromHBITMAP(void *hbm, void *hpal, GpBitmap** bitmap)
{
	return(NotImplemented);
}

GpStatus
GdipCreateHBITMAPFromBitmap(GpBitmap* bitmap, void **hbmReturn, unsigned long background)
{
	return(NotImplemented);
}

GpStatus
GdipCreateBitmapFromHICON(void *hicon, GpBitmap** bitmap)
{
	return(NotImplemented);
}

GpStatus
GdipCreateHICONFromBitmap(GpBitmap* bitmap, void **hbmReturn)
{
	return(NotImplemented);
}

GpStatus
GdipCreateBitmapFromResource(void *hInstance, GDIPCONST WCHAR *lpBitmapName, GpBitmap** bitmap)
{
	return(NotImplemented);
}

GpStatus
GdipCloneBitmapAreaI (int x, int y, int width, int height, PixelFormat format,
					  GpBitmap *original, GpBitmap **bitmap)
{
	GpBitmap *result = 0;
	GdipBitmapData bd;
	Rect sr = { x, y, width, height };
	Rect dr = { 0, 0, width, height };
	GpStatus st;

	g_return_val_if_fail (original != NULL, InvalidParameter);
	g_return_val_if_fail (bitmap != NULL, InvalidParameter);

	g_return_val_if_fail (x + width <= original->data.Width, InvalidParameter);
	g_return_val_if_fail (y + height <= original->data.Height, InvalidParameter);

	bd.Scan0 = NULL;
	bd.PixelFormat = format;
	
	st = gdip_bitmap_clone_data_rect (&original->data, &sr, &bd, &dr);
	if (st != Ok)
		return st;

	result = gdip_bitmap_new ();
	if (result == NULL){
		GdipFree (bd.Scan0);
		return OutOfMemory;
	}
	
	result->cairo_format = original->cairo_format;
	memcpy (&result->data, &bd, sizeof (GdipBitmapData));
	result->image.pixFormat = format;	/* this is only used by image codecs */
	result->image.format = original->image.format;
	result->image.height = result->data.Height;
	result->image.width = result->data.Width;
		
	*bitmap = result;
	return Ok;
}

GpStatus
GdipCloneBitmapArea (float x, float y, float w, float h, PixelFormat format,
					 GpBitmap *original, GpBitmap **bitmap)
{
	return GdipCloneBitmapAreaI ((int) x, (int) y, (int) w, (int) h, format,
								 original, bitmap);
}

static void
gdip_copy_strides (void *dst, int dstStride, void *src, int srcStride, int realBytes, int height)
{
	int i;
	for (i = 0; i < height; i++) {
		memcpy (dst, src, realBytes);
		dst += dstStride;
		src += srcStride;
	}
}

/*
 * Copy srcRect region in srcData to destRect region in destData.  No conversion is done.  Assumes
 * BitmapData is straight from a GpBitmap, i.e. its format is always PixelFormat32bppArgb.  src and
 * dest rects must be the same width/height.
 */

GpStatus
gdip_bitmap_clone_data_rect (GdipBitmapData *srcData, Rect *srcRect, GdipBitmapData *destData, Rect *destRect)
{
	int dest_components, dest_deph; 
	
	g_return_val_if_fail (srcData != NULL, InvalidParameter);
	g_return_val_if_fail (srcRect != NULL, InvalidParameter);
	g_return_val_if_fail (destData != NULL, InvalidParameter);
	g_return_val_if_fail (destRect != NULL, InvalidParameter);

	g_return_val_if_fail (srcRect->Width == destRect->Width, InvalidParameter);
	g_return_val_if_fail (srcRect->Height == destRect->Height, InvalidParameter);
	
	if (!gdip_is_a_supported_pixelformat (srcData->PixelFormat) ||
		!gdip_is_a_supported_pixelformat (destData->PixelFormat))
			return NotImplemented;
	
	dest_components = gdip_get_pixel_format_components (destData->PixelFormat);	
	dest_deph = gdip_get_pixel_format_depth (destData->PixelFormat);

	
	if (destData->Scan0 == NULL) {
		byte *data;

		destData->Stride = (((( destRect->Width * dest_components * dest_deph) /8)  + (sizeof(pixman_bits_t)-1)) & ~(sizeof(pixman_bits_t)-1));

		data = GdipAlloc (destData->Stride * destRect->Height);
		if (data == NULL) {
			return OutOfMemory;
		}
		
		destData->Scan0 = data;
		destData->Width = destRect->Width;
		destData->Height = destRect->Height;
		destData->PixelFormat = srcData->PixelFormat;
		destData->Reserved = GBD_OWN_SCAN0;
	}

	gdip_copy_strides (destData->Scan0, destData->Stride,
		srcData->Scan0 + (srcData->Stride * srcRect->Y) + (gdip_get_pixel_format_components (srcData->PixelFormat) 
		* srcRect->X), srcData->Stride,   destRect->Width * dest_components,  destRect->Height);

	return Ok;
}

/*
 * According to GDI+ testing,
 * RGB[A] PixelFormats can be converted to/from any other RGB[A] pixel format, with or without alpha.
 * We should support all of these:
 *
 * 32bpp argb - 16bpp argb 1555
 * 32bpp argb - 16bpp rgb 555
 * 32bpp argb - 16bpp rgb 565
 * 32bpp argb - 24bpp rgb 888
 * 32bpp argb - 32bpp Pargb
 * 32bpp argb - 32bpp rgb
 *
 * Others can only be converted to itself. (i.e. 16bppGrayScale, 1bppIndexed, 4bppIndexed, 8bppIndexed)
 *
 */

static int
gdip_is_pixel_format_conversion_valid (PixelFormat src, PixelFormat dest)
{
	if (src == dest)
		return 1;

	/* non-GDI supported formats can't be converted */
	if (!(src & PixelFormatGDI))
		return 0;

	if (src & PixelFormatIndexed)
		return 0;

	/* These are the RGB formats */
	if ((src & PixelFormatGDI) && !(src & PixelFormatExtended)) {
		/* all of these should be supported, but we only report the
		 * ones we really can do for now
		 */
		/* We can't handle converting to/from the 565/555/1555 ones */
		if ((src & 0xff00) == 16 || (dest & 0xff00) == 16)
			return 0;
		return 1;
	}

	return 0;
}

/**
 * srcData - input data
 * srcRect - rectangle of input data to place in destData
 * destData - where to place output; only the PixelFormat field is needed,
 *            which specifies the output type.
 * destRect - destination rectangle in output.
 */

GpStatus
gdip_bitmap_change_rect_pixel_format (GdipBitmapData *srcData, Rect *srcRect, GdipBitmapData *destData, Rect *destRect)
{
	gpointer outBuffer = NULL;
	gint outStride = 0;
	PixelFormat srcFormat, destFormat;

	g_return_val_if_fail (srcData != NULL, InvalidParameter);
	g_return_val_if_fail (srcRect != NULL, InvalidParameter);
	g_return_val_if_fail (destData != NULL, InvalidParameter);
	g_return_val_if_fail (destRect != NULL, InvalidParameter);

	g_return_val_if_fail (srcRect->Width <= destRect->Width, InvalidParameter);
	g_return_val_if_fail (srcRect->Height <= destRect->Height, InvalidParameter);

	srcFormat = srcData->PixelFormat;
	destFormat = destData->PixelFormat;

	if (!gdip_is_pixel_format_conversion_valid (srcFormat, destFormat))
		return InvalidParameter;

	/* If the pixel formats are the same, our job is easy. */
	if (srcFormat == destFormat) {
		int bitsPerPixel = gdip_get_pixel_format_bpp (srcFormat);
		int bytesPerPixel = bitsPerPixel / 8;

		g_return_val_if_fail (bitsPerPixel == 16 || bitsPerPixel == 24 || bitsPerPixel == 32, InvalidParameter);

		if (destData->Scan0 == NULL) {
			outStride = bytesPerPixel * destRect->Width;
			while (outStride % sizeof(pixman_bits_t))
				outStride++;		/* dword-align each row */

			/* Allocate the output buffer */
			outBuffer = GdipAlloc (outStride * destRect->Height);
			if (outBuffer == NULL)
				return OutOfMemory;
			
			destData->Width = destRect->Width;
			destData->Height = destRect->Height;
			destData->Stride = outStride;
			destData->Scan0 = outBuffer;
			destData->Reserved = GBD_OWN_SCAN0;
		} else {
			/* Don't touch the output data, just get the portion of the rect
			 * that we're writing to
			 */
			outStride = destData->Stride;
			outBuffer = destData->Scan0 + (destRect->Y * destData->Stride) + (destRect->X * bytesPerPixel);
		}

		/* Then let gdip_copy_strides do our work for us */
		gdip_copy_strides
			(outBuffer, outStride,
			 srcData->Scan0 + (srcRect->Y * srcData->Stride) + (srcRect->X * bytesPerPixel), srcData->Stride,
			 bytesPerPixel * destRect->Width,
			 destRect->Height);
	} else {
		/* We need to convert pixel formats */
		gboolean convert_24_to_32 = 0;
		gboolean convert_32_to_24 = 0;
		gboolean add_alpha = 0;

		int dest_skip = 0;
		int src_skip = 0;

		int srcBitsPerPixel = gdip_get_pixel_format_bpp (srcFormat);
		int srcBytesPerPixel = srcBitsPerPixel / 8;
		int destBitsPerPixel = gdip_get_pixel_format_bpp  (destFormat);
		int destBytesPerPixel = destBitsPerPixel / 8;

		if (destData->Scan0 == NULL) {
			outStride = destBytesPerPixel * destRect->Width;
			while (outStride % sizeof(pixman_bits_t))
				outStride++;		/* dword-align each row */

			/* Allocate the output buffer */
			outBuffer = GdipAlloc (outStride * destRect->Height);
			if (outBuffer == NULL)
				return OutOfMemory;
			
			destData->Width = destRect->Width;
			destData->Height = destRect->Height;
			destData->Stride = outStride;
			destData->Scan0 = outBuffer;
			destData->Reserved = GBD_OWN_SCAN0;
		} else {
			/* Don't touch the output data, just get the portion of the rect
			 * that we're writing to
			 */
			outStride = destData->Stride;
			outBuffer = destData->Scan0 + (destRect->Y * destData->Stride) + (destRect->X * destBytesPerPixel);
		}

		/* First, figure out the conversion type */
		if (gdip_get_pixel_format_bpp (srcFormat) == 32 && gdip_get_pixel_format_bpp (destFormat) == 24)
			convert_32_to_24 = 1;
		else if (gdip_get_pixel_format_bpp (srcFormat) == 24 && gdip_get_pixel_format_bpp (destFormat) == 32)
			convert_24_to_32 = 1;

		if (!(srcFormat & PixelFormatAlpha) && (destFormat & PixelFormatAlpha))
			add_alpha = 1;

		if (convert_32_to_24) src_skip = 1;
		if (convert_24_to_32 && !add_alpha) dest_skip = 1;

		if (!convert_32_to_24 && !convert_24_to_32) {
			/* Okay, well.  The formats aren't identical, but they're the same size --
			 * i.e. we might be going 32bppRgb <-> 32bppArgb.  We can always copy strides,
			 * and then go through and add alpha...
			 */
			gdip_copy_strides
				(outBuffer,
				 outStride,
				 srcData->Scan0 + (srcRect->Y * srcData->Stride) + (srcRect->X * srcBytesPerPixel),
				 srcData->Stride,
				 destBytesPerPixel * destRect->Width,
				 destRect->Height);
			if (add_alpha) {
				unsigned int *ptr = (unsigned int *) outBuffer;
				int i;
				for (i = 0; i < destRect->Height * (outStride / sizeof(unsigned int)); i++)
					*ptr++ |= 0xff000000; /* XXX TODO: check what value windows sets alpha to */
			}
		} else {
			/* We need to do 24->32 or 32->24 conversion */
			unsigned char *srcPtr, *destPtr;
			int i, j;

			for (j = 0; j < destRect->Height; j++) {
				srcPtr = srcData->Scan0 + ((srcRect->Y + j) * srcData->Stride) + (srcRect->X * srcBytesPerPixel);
				destPtr = outBuffer + (j * outStride);
				for (i = 0; i < destRect->Width; i++) {
					*destPtr++ = *srcPtr++;
					*destPtr++ = *srcPtr++;
					*destPtr++ = *srcPtr++;
					srcPtr += src_skip;
					if (add_alpha) *destPtr++ = 0xff;
					else destPtr += dest_skip;
				}
			}
		}
	}

	return Ok;
}


/* Format24bppRgb is internally stored by Cairo as a four bytes. Convert it to 3-byte (RGB) */	
int
gdip_from_ARGB_to_RGB (BYTE *src, int width, int height, int stride, BYTE **dest, int* dest_stride)
{
	int x, y, len, r, g, b, a;
	BYTE *result;
	BYTE *pos_src, *pos_dest;
	int src_components = 4; /* ARGB */
	int dest_components = 3; /* RGB */
	
	*dest_stride = dest_components * 8;
	*dest_stride = (*dest_stride * width) / 8;
	*dest_stride = (*dest_stride + (sizeof(pixman_bits_t)-1)) & ~(sizeof(pixman_bits_t)-1);		
	
	result = GdipAlloc (*dest_stride * height);
	if (result == NULL)
		return OutOfMemory;
	
	memset (result, 0, *dest_stride * height);
	
	for (y = 0, pos_src = src, pos_dest = result; y < height; y++, pos_src += stride, pos_dest += *dest_stride) {		
		for (x = 0; x < width; x++) {
			pos_dest[0 + (dest_components * x)] = pos_src[0 + (src_components * x)];
			pos_dest[1 + (dest_components * x)] = pos_src[1 + (src_components * x)];
			pos_dest[2 + (dest_components * x)] = pos_src[2 + (src_components * x)];
		}
	}
	
	*dest = result;
	return Ok;
}


/* Format24bppRgb is internally stored by Cairo as a four bytes. Convert it to 3-byte (RGB) */	
int
gdip_from_RGB_to_ARGB (BYTE *src, int width, int height, int stride, BYTE **dest, int* dest_stride)
{
	int x, y, len, r, g, b, a;
	BYTE *result;
	BYTE *pos_src, *pos_dest;
	int src_components = 3; /* RGB */
	int dest_components = 4; /* ARGB */
	
	*dest_stride = dest_components * 8;
	*dest_stride = (*dest_stride * width) / 8;
	*dest_stride = (*dest_stride + (sizeof(pixman_bits_t)-1)) & ~(sizeof(pixman_bits_t)-1);		
	
	result = GdipAlloc (*dest_stride * height);
	if (result == NULL)
		return OutOfMemory;
	
	memset (result, 0, *dest_stride * height);
	
	for (y = 0, pos_src = src, pos_dest = result; y < height; y++, pos_src += stride, pos_dest += *dest_stride) {		
		for (x = 0; x < width; x++) {
			pos_dest[0 + (dest_components * x)] = pos_src[0 + (src_components * x)];
			pos_dest[1 + (dest_components * x)] = pos_src[1 + (src_components * x)];
			pos_dest[2 + (dest_components * x)] = pos_src[2 + (src_components * x)];
			pos_dest[3 + (dest_components * x)] = 0xff;
		}
	}
	
	*dest = result;
	return Ok;
}


/* Microsoft GDI+ returns BitmapLock buffers in BGR (not RGB) */	
GpStatus 
GdipBitmapLockBits (GpBitmap *bitmap, Rect *srcRect, int flags, int format, GdipBitmapData *result)
{
	int src_stride;
	BYTE *src_scan0, *pos;
	BOOL allocated = FALSE;

	g_return_val_if_fail (bitmap != NULL, InvalidParameter);
	g_return_val_if_fail (srcRect != NULL, InvalidParameter);
	g_return_val_if_fail (flags != 0, InvalidParameter);
	g_return_val_if_fail (result != NULL, InvalidParameter);

	/* There is no way to set ImageLockModeUserInputBuf with S.D, so we don't
	 * support it
	 */
	if (flags & ImageLockModeUserInputBuf)
		return NotImplemented;

	/* Is this bitmap already locked? */
	if (bitmap->data.Reserved & GBD_LOCKED)
		return InvalidParameter;
		
	if (!bitmap || !srcRect || !result)
		return InvalidParameter;
		
	if (gdip_is_a_supported_pixelformat (format) == FALSE)
		return NotImplemented;	
	
	if (format == Format24bppRgb) {
		if (gdip_from_ARGB_to_RGB (bitmap->data.Scan0, bitmap->data.Width, bitmap->data.Height, 
					   bitmap->data.Stride, &src_scan0, &src_stride) != Ok)
			return OutOfMemory;
			
		allocated = TRUE;		
	}
	else {
		src_stride = bitmap->data.Stride;
		src_scan0 =  bitmap->data.Scan0;		
	}
		
	
	/* Special case -- the entire image is requested, with
	 * no format translation.
	 */
	if (srcRect->X == 0 && srcRect->Width == bitmap->data.Width &&
	    srcRect->Y == 0 && srcRect->Height == bitmap->data.Height &&
	    format == bitmap->data.PixelFormat)  {   

	    	result->Width = bitmap->data.Width;
		result->Height = bitmap->data.Height;
		result->Stride = src_stride;
		result->PixelFormat = format;
		result->Scan0 = GdipAlloc (src_stride * bitmap->data.Height);
		if (result->Scan0 == NULL)
			return OutOfMemory;

		if (allocated)
			GdipFree (src_scan0);
		
		memcpy (result->Scan0, src_scan0, src_stride * bitmap->data.Height);
		
	} else {
		/* the user wants a subrect and/or wants a pixel format conversion */
		GdipBitmapData convert;
		Rect destRect = {0, 0, srcRect->Width, srcRect->Height};
		GdipBitmapData src;

		convert.PixelFormat = format;
		convert.Scan0 = NULL;
		
		memcpy (&src, &bitmap->data, sizeof (GdipBitmapData));
		src.Scan0 = src_scan0;
		src.Stride = src_stride;
		
		if (gdip_bitmap_change_rect_pixel_format (&src, srcRect, &convert, &destRect) != Ok) 
			return InvalidParameter;
		
		result->Width = convert.Width;
		result->Height = convert.Height;
		result->Stride = convert.Stride;
		result->PixelFormat = convert.PixelFormat;
		result->Reserved = convert.Reserved;
		
		if (flags == ImageLockModeRead)
			result->Reserved &= GBD_READ_ONLY;
			
		result->Scan0 = convert.Scan0;	
	}

	if (flags == ImageLockModeRead)
		result->Reserved |= GBD_READ_ONLY;
	else
		result->Reserved &= ~GBD_READ_ONLY;	

		
	result->Reserved |= GBD_LOCKED;
	result->Reserved |= GBD_OWN_SCAN0;
	bitmap->data.Reserved |= GBD_LOCKED;
	
	if (allocated)
		GdipFree (src_scan0);
	
	return Ok;
}

GpStatus 
GdipBitmapUnlockBits (GpBitmap *bitmap, GdipBitmapData *bitmap_data)
{
	int src_stride;
	BYTE *src_scan0, *pos;
	BOOL allocated = FALSE;
	
	g_return_val_if_fail (bitmap != NULL, InvalidParameter);
	g_return_val_if_fail (bitmap_data != NULL, InvalidParameter);

	/* Make sure the bitmap is locked when the unlock happens */
	if (!(bitmap->data.Reserved & GBD_LOCKED))
		return InvalidParameter;
		
	if (bitmap_data->PixelFormat == Format24bppRgb) {
		if (gdip_from_RGB_to_ARGB (bitmap_data->Scan0, bitmap_data->Width, bitmap_data->Height, 
					   bitmap_data->Stride, &src_scan0, &src_stride) != Ok)
			return OutOfMemory;
	
		allocated = TRUE;		
	}
	else {
		src_stride = bitmap_data->Stride;
		src_scan0 =  bitmap_data->Scan0;	
	}
	
	/* Special case -- the entire image was requested, with
	 * no format translation, do direct copy
	 */
	if (bitmap->data.Width == bitmap_data->Width &&
	    bitmap->data.Height == bitmap_data->Height &&
	    bitmap->data.PixelFormat == bitmap_data->PixelFormat)  {   
	
		memcpy (bitmap->data.Scan0, src_scan0, bitmap->data.Stride * bitmap->data.Height);
		
	} else {
		/* the user requested a subrect and/or wants a pixel format conversion */
		GdipBitmapData convert_src;
		Rect rect = {0, 0, bitmap_data->Width, bitmap_data->Height};
		
		memcpy (&convert_src, bitmap_data, sizeof (GdipBitmapData));
		convert_src.Scan0 = src_scan0;
		convert_src.Stride = src_stride;
	
		if (gdip_bitmap_change_rect_pixel_format (&convert_src, &rect, &bitmap->data, &rect) != Ok) 
			return InvalidParameter;
			
	}

	if (bitmap_data->Reserved & GBD_OWN_SCAN0)
		GdipFree (bitmap_data->Scan0);

	/* unlock the bitmap */
	bitmap->data.Reserved &= ~GBD_LOCKED;
		
	if (allocated)
		GdipFree (src_scan0);
	
	return Ok;
}

GpStatus
GdipBitmapSetPixel (GpBitmap *bitmap, int x, int y, ARGB color)
{
	GdipBitmapData *data = &bitmap->data;
	unsigned char *v;
	
	if (bitmap == 0) 
		return InvalidParameter;
	
	if (x < 0 || x > data->Width)
		return InvalidParameter;

	if (y < 0 || y > data->Height)
		return InvalidParameter;
		
	/* BMP Locked */
	if (bitmap->data.Reserved & GBD_LOCKED)
		return InvalidParameter;

	v = (unsigned char *)(data->Scan0) + y * data->Stride;
	switch (data->PixelFormat) {
		case Format24bppRgb:
			v += x * 4;
#ifdef WORDS_BIGENDIAN
			v [3] = color & 0xff;
			v [2] = (color >> 8) & 0xff;
			v [1] = (color >> 16) & 0xff;
			v [0] = 0xff;
#else
			v [0] = color & 0xff;
			v [1] = (color >> 8) & 0xff;
			v [2] = (color >> 16) & 0xff;
			v [3] = 0xff;
#endif
			break;
			
		case Format32bppArgb:
		case Format32bppPArgb:
			v += x * 4;
#ifdef WORDS_BIGENDIAN
			v [3] = color & 0xff;
			v [2] = (color >> 8) & 0xff;
			v [1] = (color >> 16) & 0xff;
			v [0] = color >> 24;
#else
			v [0] = color & 0xff;
			v [1] = (color >> 8) & 0xff;
			v [2] = (color >> 16) & 0xff;
			v [3] = color >> 24;
#endif
			break;
			
		case Format32bppRgb:
			v += x * 4;
#ifdef WORDS_BIGENDIAN
			v [3] = color & 0xff;
			v [2] = (color >> 8) & 0xff;
			v [1] = (color >> 16) & 0xff;
			v [0] = 0xff;
#else
			v [0] = color & 0xff;
			v [1] = (color >> 8) & 0xff;
			v [2] = (color >> 16) & 0xff;
			v [3] = 0xff;
#endif
			break;
		default:
			return NotImplemented;
	} 
	return Ok;		
}

GpStatus
GdipBitmapGetPixel (GpBitmap *bitmap, int x, int y, ARGB *color)
{
	GdipBitmapData *data = &bitmap->data;
	unsigned char *v;
	
	if (bitmap == NULL || color == NULL) 
		return InvalidParameter;

	if (x < 0 || x > data->Width)
		return InvalidParameter;

	if (y < 0 || y > data->Height)
		return InvalidParameter;
		
	/* BMP Locked */
	if (bitmap->data.Reserved & GBD_LOCKED)
		return InvalidParameter;
		
	v = ((unsigned char *)data->Scan0) + y * data->Stride;
	switch (data->PixelFormat) {
		case Format24bppRgb:
			v += x * 4;
			*color = (v [0]) | (v [1] << 8) | (v [2] << 16) | (0xff << 24);
			break;
		case Format32bppArgb:
		case Format32bppPArgb:
			v += x * 4;
			*color = (v [0]) | (v [1] << 8) | (v [2] << 16) | (v [3] << 24);
			break;
		case Format32bppRgb:
			v += x * 4;
			*color = (v [0]) | (v [1] << 8) | (v [2] << 16)| (0xff << 24);
			break;
		default:
			return NotImplemented;
	} 
	
	return Ok;
}

GpStatus
GdipBitmapSetResolution (GpBitmap *bitmap, float xdpi, float ydpi)
{
	g_return_val_if_fail (bitmap != NULL, InvalidParameter);

	bitmap->image.horizontalResolution = xdpi;
	bitmap->image.verticalResolution = ydpi;

	bitmap->image.imageFlags |= ImageFlagsHasRealDPI;

	return Ok;
}

cairo_surface_t *
gdip_bitmap_ensure_surface (GpBitmap *bitmap)
{

	if (bitmap->image.surface == NULL &&
		bitmap->data.Scan0 != NULL)	{		
		
		switch (bitmap->data.PixelFormat) {
		case Format24bppRgb:
			bitmap->image.surface = cairo_surface_create_for_image
				((char *)bitmap->data.Scan0,
				 CAIRO_FORMAT_RGB24,
				 bitmap->data.Width,
				 bitmap->data.Height,
				 bitmap->data.Stride);
			break;
		case Format32bppArgb:
		case Format32bppRgb:
		case Format32bppPArgb:
			bitmap->image.surface = cairo_surface_create_for_image
				((char *)bitmap->data.Scan0,
				 CAIRO_FORMAT_ARGB32,
				 bitmap->data.Width,
				 bitmap->data.Height,
				 bitmap->data.Stride);
			break;
		default:
			g_warning ("gdip_bitmap_ensure_surface: Unable to create a surface for raw bitmap data of format 0x%08x", bitmap->data.PixelFormat);
			break;
		}
	}

	return bitmap->image.surface;
}

