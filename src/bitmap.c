/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: t; tab-width: 4; -*-
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
#include "gdip_win32.h"
#include "gdipImage.h"
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



/* Pick an unlikely combination. This pixel color will be interpreted as 'transparent' 
   when mapping Wine pixels onto a cairo image. Need to do this crude hack since there
   is no bitmap alpha component in regular Win32 bitmaps */
#define	UNALTERED_PIXEL		0x00454243

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
	bitmap->cairo_format =  CAIRO_FORMAT_ARGB32; /* We always use this format when drawing into  Cairo. We hardcoded this value here*/
		
	bitmap->internal_format	= Format32bppArgb; /* Real internal format of Scan0 */
	bitmap->hBitmapDC = 0;
	bitmap->hInitialBitmap = 0;
	bitmap->hBitmap = 0;
	bitmap->lockRect.X = bitmap->lockRect.Y = bitmap->lockRect.Height = bitmap->lockRect.Width = 0;
}

void
gdip_bitmap_clone (GpBitmap *bitmap, GpBitmap **clonedbitmap)
{
	*clonedbitmap = (GpBitmap *) GdipAlloc (sizeof (GpBitmap));
	memcpy (*clonedbitmap, bitmap, sizeof (GpBitmap));
}


GpBitmap *
gdip_bitmap_new ()
{
	GpBitmap *result = (GpBitmap *) GdipAlloc (sizeof (GpBitmap));
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
	if ((bitmap->data.Reserved & GBD_OWN_SCAN0) != 0)
		GdipFree (bitmap->data.Scan0);
}


void *
gdip_bitmap_create_Win32_HDC (GpBitmap *bitmap)
{
	void * result = 0;
	void * hdc = CreateCompatibleDC_pfn (0);
	void * hbitmap = 0, * holdbitmap = 0;
	void * hdcDesc = GetDC_pfn (0);
	
	hbitmap = CreateCompatibleBitmap_pfn (hdcDesc, bitmap->data.Width, bitmap->data.Height);
	if (hbitmap != 0) {
		BITMAPINFO	bmi;
		gdip_bitmap_fill_info_header (bitmap, &bmi.bmiHeader);
		SetDIBits_pfn (hdc, hbitmap, 0, bitmap->data.Height, bitmap->data.Scan0, &bmi, 0);
		holdbitmap = SelectObject_pfn (hdc, hbitmap);
		bitmap->hBitmapDC = hdc;
		bitmap->hInitialBitmap = holdbitmap;
		bitmap->hBitmap = hbitmap;
		result = hdc;
	} else {
		DeleteDC_pfn (hdc);
	}
	ReleaseDC_pfn (0, hdcDesc);
	return result;
}

void 
gdip_bitmap_destroy_Win32_HDC (GpBitmap *bitmap, void *hdc)
{
	if (bitmap->hBitmapDC == hdc) {
		
		BITMAPINFO	bmi;
		int res = 0;
		unsigned long *array, *end;
			
		SelectObject_pfn (bitmap->hBitmapDC, bitmap->hInitialBitmap);
			
		gdip_bitmap_fill_info_header (bitmap, &bmi.bmiHeader);
		res = GetDIBits_pfn (bitmap->hBitmapDC, bitmap->hBitmap, 0, bitmap->data.Height, bitmap->data.Scan0, &bmi, 0);

		if (bitmap->cairo_format == CAIRO_FORMAT_ARGB32) {
			array = bitmap->data.Scan0;
			end = array + (bmi.bmiHeader.biSizeImage >> 2);
			while (array < end) {
				if (*array!=UNALTERED_PIXEL) {
					*array |= 0xff000000;
				}
				++array;
			}
		}

		DeleteObject_pfn (bitmap->hBitmap);
		DeleteDC_pfn (bitmap->hBitmapDC);
		bitmap->hBitmapDC = 0;
		bitmap->hInitialBitmap = 0;
		bitmap->hBitmap = 0;
	}
}

GpStatus 
GdipCreateBitmapFromScan0 (int width, int height, int stride, int format, void *scan0, GpBitmap **bitmap)
{
	GpBitmap *result = 0;
	bool own_scan0 = FALSE;
	
	result = gdip_bitmap_new ();
	gdip_bitmap_init (result);	

	/* We are going to allocate a buffer for our internal format*/
	if (scan0 == NULL) {
	
		if (stride == 0) {
			int bpp  = gdip_get_pixel_format_components (result->internal_format);
			bpp = bpp * gdip_get_pixel_format_depth (result->internal_format);
			stride = (bpp * width) / 8;
			stride = (stride + 3) & ~3;		
		}	
		
		scan0 = GdipAlloc (stride * height);
		memset (scan0, 0, stride * height);
		own_scan0 = TRUE;
	}
	else {
		/*
			TODO: If Scan0 is allocated it may contain data. If the pixelformat!=32
			it has to be converted and more likely re-allocated since we need at least
			and extra byte per pixel. In this case, when we do not allocate the buffer
			we will need to keep two buffers, one in 32bbps and the Scan0 allocated by
			
		*/		
		GdipFree (result);
		return NotImplemented;	
	}
	
	result->data.Width = result->image.width = width;
	result->data.Height = result->image.height = height;
	result->data.PixelFormat = result->image.pixFormat = format;
	result->data.Scan0 = scan0;
	result->data.Stride = stride;

	if (own_scan0)
		result->data.Reserved |= GBD_OWN_SCAN0;

	*bitmap = result;

	return Ok;
}

GpStatus
GdipCreateBitmapFromGraphics (int width, int height, GpGraphics *graphics, GpBitmap **bitmap)
{
	GpBitmap 		*result = 0;
	int 			bmpSize = 0;
	int 			stride = width;
	unsigned long	*ptr;
	unsigned char	*endptr;
	
	if (!graphics || !bitmap)
		return InvalidParameter;

	/*
	 * FIXME: should get the stride based on the format of the graphics object.
	 */
	fprintf (stderr, "GdipCreateBitmapFromGraphics: This routine has not been checked for stride size\n");
	while (stride % 4)
		stride++;
	
	stride *= 4;
		
	bmpSize = stride * height;
	result = gdip_bitmap_new ();
	gdip_bitmap_init (result);
	
	result->data.Width = result->image.width = width;
	result->data.Height = result->image.height = height;
	result->data.PixelFormat = result->image.pixFormat = Format32bppArgb;
	result->data.Stride = stride;

	result->data.Scan0 = GdipAlloc (bmpSize);

	ptr=result->data.Scan0;
	endptr=result->data.Scan0+bmpSize;
	while ((unsigned char *)ptr<endptr) {
		ptr[0]=UNALTERED_PIXEL;
		ptr++;
	}

	result->data.Reserved |= GBD_OWN_SCAN0;
	*bitmap = result;
	return Ok;
}

GpStatus
GdipCloneBitmapAreaI (int x, int y, int width, int height, PixelFormat format,
					  GpBitmap *original, GpBitmap **bitmap)
{
	GpBitmap *result = 0;
	Rect sr = { x, y, width, height };
	Rect dr = { 0, 0, width, height };
	GpStatus st;
	
	g_return_val_if_fail (original != NULL, InvalidParameter);
	g_return_val_if_fail (bitmap != NULL, InvalidParameter);

	g_return_val_if_fail (x + width <= original->data.Width, InvalidParameter);
	g_return_val_if_fail (y + height <= original->data.Height, InvalidParameter);

	result = gdip_bitmap_new ();
	gdip_bitmap_init (result);
	
	result->image.pixFormat  = result->data.PixelFormat = format;
	result->image.format = original->image.format;	/* this is only used by image codecs */
	
	st = gdip_bitmap_clone_bitmap_rect (original, &sr, result, &dr);
	if (st != Ok) {
		GdipDisposeImage ((GpImage *)result);
		return st;
	} 

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
gdip_bitmap_clone_bitmap_rect (GpBitmap *src, Rect *srcRect, GpBitmap *dest, Rect *destRect)
{
	int dest_components, dest_deph; 
		
	g_return_val_if_fail (src != NULL, InvalidParameter);
	g_return_val_if_fail (srcRect != NULL, InvalidParameter);
	g_return_val_if_fail (dest != NULL, InvalidParameter);
	g_return_val_if_fail (destRect != NULL, InvalidParameter);

	g_return_val_if_fail (srcRect->Width == destRect->Width, InvalidParameter);
	g_return_val_if_fail (srcRect->Height == destRect->Height, InvalidParameter);
	
	dest_components = gdip_get_pixel_format_components (dest->internal_format);	
	dest_deph = gdip_get_pixel_format_depth (dest->internal_format);
	
 	if (dest->data.Scan0 == NULL) {
		dest->data.Stride = (((( destRect->Width * dest_components * dest_deph) /8)  + 3) & ~3);
		dest->data.Scan0 = GdipAlloc (dest->data.Stride * destRect->Height);
		dest->data.Width = destRect->Width;
		dest->data.Height = destRect->Height;
		dest->data.Reserved = GBD_OWN_SCAN0;
 	} 
	
	/* Since the buffers internally are 32bbp, we always copy strides*/
	
	gdip_copy_strides (dest->data.Scan0, dest->data.Stride,
		src->data.Scan0 + (src->data.Stride * srcRect->Y) + (gdip_get_pixel_format_components (src->internal_format) 
		* srcRect->X), src->data.Stride,  destRect->Width * dest_components,  destRect->Height);
		
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
		int bitsPerPixel = PIXEL_FORMAT_BPP(srcFormat);
		int bytesPerPixel = bitsPerPixel / 8;

		g_return_val_if_fail (bitsPerPixel == 16 || bitsPerPixel == 24 || bitsPerPixel == 32, InvalidParameter);

		if (destData->Scan0 == NULL) {
			outStride = bytesPerPixel * destRect->Width;
			while (outStride % 4)
				outStride++;		/* dword-align each row */

			/* Allocate the output buffer */
			outBuffer = GdipAlloc (outStride * destRect->Height);

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

		int srcBitsPerPixel = PIXEL_FORMAT_BPP(srcFormat);
		int srcBytesPerPixel = srcBitsPerPixel / 8;
		int destBitsPerPixel = PIXEL_FORMAT_BPP(destFormat);
		int destBytesPerPixel = destBitsPerPixel / 8;

		if (destData->Scan0 == NULL) {
			outStride = destBytesPerPixel * destRect->Width;
			while (outStride % 4)
				outStride++;		/* dword-align each row */

			/* Allocate the output buffer */
			outBuffer = GdipAlloc (outStride * destRect->Height);

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
		if (PIXEL_FORMAT_BPP(srcFormat) == 32 && PIXEL_FORMAT_BPP(destFormat) == 24)
			convert_32_to_24 = 1;
		else if (PIXEL_FORMAT_BPP(srcFormat) == 24 && PIXEL_FORMAT_BPP(destFormat) == 32)
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
				g_assert (PIXEL_FORMAT_BPP(destFormat) == 32);
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

GpStatus 
GdipBitmapLockBits (GpBitmap *bitmap, Rect *srcRect, int flags, int format, GdipBitmapData *result)
{
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

	bitmap->data.Reserved |= GBD_LOCKED;
	bitmap->lockRect = *srcRect;

	/* Special case -- the entire image is requested, with
	 * no format translation.
	 */
	if (srcRect->X == 0 && srcRect->Width == bitmap->data.Width &&
	    srcRect->Y == 0 && srcRect->Height == bitmap->data.Height &&
	    format == bitmap->internal_format)
	{
		/* if the image is read-only, then we just give the
		 * user a copy of our internal buffer, and expect them
		 * not to modify it.
		 */
		if (flags == ImageLockModeRead) {
			*result = bitmap->data;
			result->Reserved |= GBD_READ_ONLY;
			result->Reserved &= ~GBD_OWN_SCAN0;
		} else {
			/* It's either Write-only or R/W, so we must allocate
			 * a new buffer (to comply with LockBits semantics)
			 */
			*result = bitmap->data;
			result->Scan0 = GdipAlloc (bitmap->data.Stride * bitmap->data.Height);
			memcpy (result->Scan0, bitmap->data.Scan0, bitmap->data.Stride * bitmap->data.Height);
			result->Reserved |= GBD_OWN_SCAN0;
			result->Reserved &= ~GBD_READ_ONLY;
		}

		return Ok;
	} else {
		/* the user wants a subrect and/or wants a pixel format conversion */
		GdipBitmapData convert;
		convert.PixelFormat = format;
		convert.Scan0 = NULL;
		Rect destRect = {0, 0, srcRect->Width, srcRect->Height};

		/* TODO: Take into account internal_format*/
		if (gdip_bitmap_change_rect_pixel_format (&(bitmap->data), srcRect, &convert, &destRect) != Ok) {
			/* Unlock before return */
			bitmap->data.Reserved &= ~GBD_LOCKED;
			return InvalidParameter;
		}

		result->Width = convert.Width;
		result->Height = convert.Height;
		result->Stride = convert.Stride;
		result->PixelFormat = convert.PixelFormat;
		result->Reserved = convert.Reserved;
		if (flags == ImageLockModeRead)
			result->Reserved &= GBD_READ_ONLY;
		result->Scan0 = convert.Scan0;
	}

	return Ok;
}

GpStatus 
GdipBitmapUnlockBits (GpBitmap *bitmap, GdipBitmapData *bitmap_data)
{
	g_return_val_if_fail (bitmap != NULL, InvalidParameter);
	g_return_val_if_fail (bitmap_data != NULL, InvalidParameter);

	/* Make sure the bitmap is locked when the unlock happens */
	if (!(bitmap->data.Reserved & GBD_LOCKED))
		return InvalidParameter;

	/* Put the data back where it came from, if it's a copy, and if it's not read-only */
	if (!(bitmap_data->Reserved & GBD_READ_ONLY) &&
		bitmap_data->Reserved & GBD_OWN_SCAN0)
	{
		Rect r = {0, 0, bitmap_data->Width, bitmap_data->Height};
		if (gdip_bitmap_change_rect_pixel_format (bitmap_data, &r, &(bitmap->data), &(bitmap->lockRect))) {
			/* uhh */
			g_warning ("gdip_bitmap_change_rect_pixel_format failed in UnlockBits!");
		}
	}

	if (bitmap_data->Reserved & GBD_OWN_SCAN0)
		GdipFree (bitmap_data->Scan0);

	/* unlock the bitmap */
	bitmap->data.Reserved &= ~GBD_LOCKED;

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

	v = (unsigned char *)(data->Scan0) + y * data->Stride;
	
	/* Pixels are stored always as 32ARGB*/
	v += x * 4;
	v [0] = color & 0xff;
	v [1] = (color >> 8) & 0xff;
	v [2] = (color >> 16) & 0xff;
	v [3] = color >> 24;
	 
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

	v = ((unsigned char *)data->Scan0) + y * data->Stride;
	
	/* Pixels are stored always a 32ARGB*/
	v += x * 4;
	*color = (v [0]) | (v [1] << 8) | (v [2] << 16) | (v [3] << 24);
	
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
		bitmap->data.Scan0 != NULL)
	{
		if (bitmap->data.PixelFormat == Format32bppArgb) {
			bitmap->cairo_format = CAIRO_FORMAT_ARGB32;
			bitmap->image.surface = cairo_surface_create_for_image
				(bitmap->data.Scan0,
				 CAIRO_FORMAT_ARGB32,
				 bitmap->data.Width,
				 bitmap->data.Height,
				 bitmap->data.Stride);
		} else {
			g_warning ("gdip_bitmap_ensure_surface: Unable to create a surface for raw bitmap data of format 0x%08x", bitmap->data.PixelFormat);
		}
	}

	return bitmap->image.surface;
}
