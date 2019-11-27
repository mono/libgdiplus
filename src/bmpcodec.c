/*
 * Copyright (c) 2004 Ximian
 * Copyright (C) 2003-2004,2007 Novell, Inc (http://www.novell.com)
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
 * bmpcodec.c : Contains function definitions for encoding decoding bmp images
 *
 * Authors:
 *      Jordi Mas i Hernandez (jordi@ximian.com)
 *      Sanjay Gupta (gsanjay@novell.com)
 *      Mark Steele (ms@rapidsys.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Useful documentation about bitmaps
 *
 *      http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/bitmaps_4v1h.asp
 *      http://www.csdn.net/Dev/Format/windows/Bmp.html
 *      http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html
 *
 *      Header structure
 *              BITMAPFILEHEADER
 *              BITMAPINFOHEADER or BITMAPV4HEADER or BITMAPV5HEADER or BITMAPCOREHEADER
 *              RGBQUADS or RGBTRIPLE (optional)
 *              Bitmap data
 */

#include "gdiplus-private.h"
#include "bmpcodec.h"

GUID gdip_bmp_image_format_guid = {0xb96b3cabU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

/* Codecinfo related data*/
static ImageCodecInfo bmp_codec;
static const WCHAR bmp_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'B', 'M', 'P', ' ', 'C', 'o', 'd', 'e', 'c', 0}; /* Built-in BMP Codec */
static const WCHAR bmp_extension[] = {'*','.','B', 'M', 'P',';', '*','.', 'D','I', 'B',';', '*','.', 'R', 'L', 'E',0}; /* *.BMP;*.DIB;*.RLE */
static const WCHAR bmp_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'b', 'm', 'p', 0}; /* image/bmp */
static const WCHAR bmp_format[] = {'B', 'M', 'P', 0}; /* BMP */
static const BYTE bmp_sig_pattern[] = { 0x42, 0x4D };
static const BYTE bmp_sig_mask[] = { 0xFF, 0xFF };


ImageCodecInfo *
gdip_getcodecinfo_bmp ()
{
	bmp_codec.Clsid = (CLSID) { 0x557cf400, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	bmp_codec.FormatID = gdip_bmp_image_format_guid;
	bmp_codec.CodecName = (const WCHAR*) bmp_codecname;            
	bmp_codec.DllName = NULL;
	bmp_codec.FormatDescription = (const WCHAR*) bmp_format;
	bmp_codec.FilenameExtension = (const WCHAR*) bmp_extension;
	bmp_codec.MimeType = (const WCHAR*) bmp_mimetype;
	bmp_codec.Flags = ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin;
	bmp_codec.Version = 1;
	bmp_codec.SigCount = 1;
	bmp_codec.SigSize = 2;
	bmp_codec.SigPattern = bmp_sig_pattern;
	bmp_codec.SigMask = bmp_sig_mask;

	return &bmp_codec; 
}

static GpStatus
gdip_get_bmp_stride (PixelFormat format, INT width, INT *strideResult, BOOL cairoHacks)
{
	INT stride;
	/* stride is a (signed) _int_ and once multiplied by 4 it should hold a value that can be allocated by GdipAlloc
		* this effectively limits 'width' to 536870911 pixels */
	unsigned long long int widthBuff = width;

	switch (format) {
	case PixelFormat1bppIndexed:
		stride = (width + 7) / 8;
		break;
	case PixelFormat4bppIndexed:
		stride = (width + 1) / 2;
		break;
	case PixelFormat8bppIndexed:
		stride = width;
		break;
	case PixelFormat16bppRGB555:
	case PixelFormat16bppRGB565:
		widthBuff *= 2;
		if (widthBuff > G_MAXINT32)
			return InvalidParameter;
		
		stride = widthBuff;
		break;
	case PixelFormat24bppRGB:
		widthBuff *= cairoHacks ? 4 : 3;
		if (widthBuff > G_MAXINT32)
			return InvalidParameter;
		
		stride = widthBuff;
		break;
	default:
		widthBuff *= 4;
		if (widthBuff > G_MAXINT32)
			return InvalidParameter;

		stride = widthBuff;
		break;
	}

	/* Ensure 32bits alignment */
	gdip_align_stride (stride);
	*strideResult = stride;
	return Ok;
}

static GpStatus
gdip_get_bmp_pixelformat (const BITMAPV5HEADER *bih, PixelFormat *sourceFormatResult, PixelFormat *conversionFormatResult)
{
	PixelFormat sourceFormat;
	PixelFormat conversionFormat;

	if (bih->bV5Compression == BI_BITFIELDS) {
		if (bih->bV5BitCount != 16)
			return OutOfMemory;

		if ((bih->bV5RedMask == 0x7C00) && (bih->bV5GreenMask == 0x3E0) && (bih->bV5BlueMask == 0x1F))
			sourceFormat = PixelFormat16bppRGB555;
		else if ((bih->bV5RedMask == 0xF800) && (bih->bV5GreenMask == 0x7E0) && (bih->bV5BlueMask == 0x1F))
			sourceFormat = PixelFormat16bppRGB565;
		else
			sourceFormat = PixelFormat16bppRGB555;

		conversionFormat = PixelFormat32bppRGB;
	} else {
		switch (bih->bV5BitCount) {
		case 64:
			sourceFormat = PixelFormat64bppARGB;
			conversionFormat = sourceFormat;
			break;
		case 32:
			sourceFormat = PixelFormat32bppRGB;
			conversionFormat = sourceFormat;
			break;
		case 24:
			sourceFormat = PixelFormat24bppRGB;
			conversionFormat = sourceFormat;
			break;
		case 16:
			sourceFormat = PixelFormat16bppRGB555;
			conversionFormat = PixelFormat32bppRGB;
			break;
		case 8:
			sourceFormat = PixelFormat8bppIndexed;
			conversionFormat = sourceFormat;
			break;
		case 4:
			sourceFormat = PixelFormat4bppIndexed;
			conversionFormat = sourceFormat;
			break;
		case 1:
			sourceFormat = PixelFormat1bppIndexed;
			conversionFormat = sourceFormat;
			break;
		default:
			return OutOfMemory;
		}
	}

	*sourceFormatResult = sourceFormat;
	*conversionFormatResult = conversionFormat;
	return Ok;
}

static void 
gdip_bitmap_fill_info_header (GpBitmap *bitmap, PBITMAPINFOHEADER bmi)
{
	PixelFormat format = bitmap->active_bitmap->pixel_format;

	memset (bmi, 0, sizeof (BITMAPINFOHEADER));
#ifdef WORDS_BIGENDIAN
	bmi->biSize = GUINT32_FROM_LE (sizeof (BITMAPINFOHEADER));
	bmi->biWidth = GULONG_FROM_LE (bitmap->active_bitmap->width);
	bmi->biHeight = GULONG_FROM_LE (bitmap->active_bitmap->height);
	bmi->biPlanes = GUINT16_FROM_LE (1);
	if (format != PixelFormat24bppRGB)
		bmi->biBitCount = GUINT16_FROM_LE (gdip_get_pixel_format_bpp (bitmap->active_bitmap->pixel_format));
	else
		bmi->biBitCount = GUINT16_FROM_LE (24);
	bmi->biCompression = GUINT32_FROM_LE (BI_RGB);
	bmi->biSizeImage =  GUINT32_FROM_LE (0); /* Many tools expect this may be set to zero for BI_RGB bitmaps */
	bmi->biXPelsPerMeter = GULONG_FROM_LE ((int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100)));
	bmi->biYPelsPerMeter = GULONG_FROM_LE ((int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100))); /* 1 meter is = 39.37 */       
#else
	bmi->biSize = sizeof (BITMAPINFOHEADER);
	bmi->biWidth = bitmap->active_bitmap->width;
	bmi->biHeight = bitmap->active_bitmap->height;
	bmi->biPlanes = 1;
	if (format != PixelFormat24bppRGB)
		bmi->biBitCount = gdip_get_pixel_format_bpp (bitmap->active_bitmap->pixel_format);
	else
		bmi->biBitCount = 24;

	bmi->biCompression = BI_RGB;
	bmi->biSizeImage =  0; /* Many tools expect this may be set to zero for BI_RGB bitmaps */
	bmi->biXPelsPerMeter = (int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100));
	bmi->biYPelsPerMeter = (int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100)); /* 1 meter is = 39.37 */       
#endif
}                                                           

static void
gdip_read_bmp_rle_8bit (void *pointer, BYTE *scan0, BOOL upsidedown, int stride, int scanWidth, int scanCount, ImageSource source)
{
	BYTE code;
	int bytes_read;

	int col_offset = 0;
	int row_offset = (scanCount - 1) * stride;
	int row_delta = -stride;
	int rows_remaining = scanCount;
	int size = scanCount * stride;
	BOOL new_row = FALSE;

	if (!upsidedown)
		return; /* top to bottom images can't be compressed */

	if (scanWidth > stride)
		return;

	while ((rows_remaining > 0)
	    || ((row_offset == 0) && (col_offset < scanWidth))) {
		bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

		if (bytes_read < 1)
			return; /* TODO?: Add an "unexpected end of file" error code */

		if (code == 0) { /* RLE escape code */
			bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

			if (bytes_read < 1)
				return; /* TODO?: Add an "unexpected end of file" error code */

			switch (code)
			{
				case 0: /* skip remainder of scan */
				{
					if (new_row)
						new_row = FALSE;
					else {
						row_offset += row_delta;
						rows_remaining--;
						col_offset = 0;
					}
					break;
				}
				case 1: /* skip remainder of image -- in other words, we're finished :-) */
				{
					return;
				}
				case 2: /* jump forward (dx, dy) coordinates */
				{
					BYTE dx, dy;

					bytes_read  = gdip_read_bmp_data (pointer, &dx, 1, source);
					bytes_read += gdip_read_bmp_data (pointer, &dy, 1, source);

					if (bytes_read < 2)
						return; /* TODO?: Add an "unexpected end of file" error code */

					/* not really sure how to handle the case where the X delta goes
					 * past the end of the scan. in the interest of not crashing,
					 * let's wrap it back around.
					 */
					col_offset = (col_offset + dx) % scanWidth;
					row_offset -= dy * stride; /* BMPs go from bottom to top */

					new_row = FALSE;
					break;
				}
				default: /* an uncompressed section, 'code' pixels wide */
				{
					/* uncompressed sections must be an even number of bytes long,
					 * even if they are an odd number of *pixels* long.
					 */
					BOOL pad_byte_present = ((code & 1) != 0);
					int bytes_to_read = code;

					/* wrap rows properly, even though they are inverted in memory */
					while (bytes_to_read > 0) {
						int bytes_to_read_this_scan = scanWidth - col_offset;

						if (bytes_to_read_this_scan > bytes_to_read)
							bytes_to_read_this_scan = bytes_to_read;

						int pixel_index = row_offset + col_offset;

						if (pixel_index < 0 || pixel_index >= size)
							return;

						bytes_read = gdip_read_bmp_data (
							pointer,
							&scan0[pixel_index],
							bytes_to_read_this_scan,
							source);

						if (bytes_read < bytes_to_read_this_scan)
							return; /* TODO?: Add an "unexpected end of file" error code */

						col_offset += bytes_read;
						bytes_to_read -= bytes_read;

						if (col_offset >= scanWidth) 
						{
							col_offset = 0;
							row_offset += row_delta;
							rows_remaining--;

							if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
								return;

							new_row = TRUE;
						}
						else
							new_row = FALSE;
					}

					if (pad_byte_present) {
						bytes_read = gdip_read_bmp_data(pointer, &code, 1, source);

						if (bytes_read < 1)
							return; /* TODO?: Add an "unexpected end of file" error code */
					}

					break;
				}
			}
		}
		else {
			/* we have a run of length 'code'. the colour of the run is the next byte in the file. */
			int run_length = code;
			BYTE pixel_value;

			bytes_read = gdip_read_bmp_data(pointer, &pixel_value, 1, source);

			if (bytes_read < 1)
				return; /* TODO?: Add an "unexpected end of file" error code */

			while (run_length > 0) {
				int bytes_to_run_this_scan = scanWidth - col_offset;

				if (bytes_to_run_this_scan > run_length)
					bytes_to_run_this_scan = run_length;

				int pixel_index = row_offset + col_offset;

				if (pixel_index < 0 || pixel_index >= size)
					return;

				if (bytes_to_run_this_scan < 0 || pixel_index + bytes_to_run_this_scan > size)
					return;

				memset (scan0 + pixel_index, pixel_value, bytes_to_run_this_scan);

				col_offset += bytes_to_run_this_scan;
				run_length -= bytes_to_run_this_scan;

				if (col_offset >= scanWidth) 
				{
					col_offset = 0;
					row_offset += row_delta;
					rows_remaining--;

					if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
						return;

					new_row = TRUE;
				}
				else
					new_row = FALSE;
			}
		}
	}

	return;
}

// PixelFormat32bppARGB.
ARGB
gdip_getpixel_32bppARGB (BYTE *scan, INT x)
{
	ARGB pixel = ((ARGB *) scan)[x];
	return pixel;
}

void
gdip_setpixel_32bppARGB (BYTE *scan, INT x, BYTE a, BYTE r, BYTE g, BYTE b)
{
	set_pixel_bgra (scan, x * 4, b, g, r, a);
}

// PixelFormat16bppRGB555.
ARGB
gdip_getpixel_16bppRGB555 (BYTE *scan, INT x)
{
	WORD pixel = ((WORD *) scan)[x];
	return ((pixel & 0x1F) >> 2) | 8 * ((pixel & 0x1F) | 8 * (((((pixel >> 5) & 0x1F) | (((pixel >> 10) & 0x1C) << 8)) & 0xFFFFFFFC) | 32 * (((pixel >> 5) & 0x1F) | ((((pixel >> 10) & 0x1F) | 0xFFFFFFE0) << 8))));
}

// PixelFormat16bppRGB565.
ARGB
gdip_getpixel_16bppRGB565 (BYTE *scan, INT x)
{
	WORD pixel = ((WORD *) scan)[x];
	return ((pixel & 0x1F) >> 2) | 8 * ((pixel & 0x1F) | 2 * (((((pixel >> 5) & 0x3F) | (((pixel >> 11) & 0xFFFFFFFC) << 10)) & 0xFFFFFFF0) | ((((pixel >> 5) & 0x3F) | ((((pixel >> 11)) | 0xFFFFFFE0) << 9)) << 6)));
}

static void
gdip_read_bmp_rle_4bit (void *pointer, BYTE *scan0, BOOL upsidedown, int stride, int scanWidth, int scanCount, ImageSource source)
{
	BYTE code;
	int bytes_read;

	int col_offset = 0;
	int row_offset = (scanCount - 1) * stride;
	int row_delta = -stride;
	int rows_remaining = scanCount;
	int size = scanCount * stride;
	BOOL new_row = FALSE;

	if (!upsidedown)
		return; /* top to bottom images can't be compressed */

	if ((scanWidth & 1) != 0)
		scanWidth++;

	if (scanWidth > stride * 2)
		return;

	while (rows_remaining > 0) {
		bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

		if (bytes_read < 1)
			return; /* TODO?: Add an "unexpected end of file" error code */

		if (code == 0) { /* RLE escape code */
			bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

			if (bytes_read < 1)
				return; /* TODO?: Add an "unexpected end of file" error code */

			switch (code)
			{
				case 0: /* skip remainder of scan */
				{
					if (new_row)
						new_row = FALSE;
					else {
						row_offset += row_delta;
						rows_remaining--;
						col_offset = 0;
					}
					break;
				}
				case 1: /* skip remainder of image -- in other words, we're finished :-) */
				{
					return;
				}
				case 2: /* jump forward (dx, dy) coordinates */
				{
					BYTE dx, dy;

					bytes_read  = gdip_read_bmp_data (pointer, &dx, 1, source);
					bytes_read += gdip_read_bmp_data (pointer, &dy, 1, source);

					if (bytes_read < 2)
						return; /* TODO?: Add an "unexpected end of file" error code */

					/* not really sure how to handle the case where the X delta goes
					 * past the end of the scan. in the interest of not crashing,
					 * let's wrap it back around.
					 */
					col_offset = (col_offset + dx) % scanWidth;
					row_offset -= dy * stride; /* BMPs go from bottom to top */

					new_row = FALSE;

					break;
				}
				default: /* an uncompressed section, 'code' pixels wide */
				{
					int pixels_to_read = code;
					int bytes_of_data = (pixels_to_read + 1) / 2;

					/* uncompressed sections must be an even number of bytes long,
					 * even if they are an odd number of *pixels* long.
					 */
					BOOL pad_byte_present = ((bytes_of_data & 1) != 0);

					int bytes_to_read = pixels_to_read / 2; /* leave off the last pixel for now */

					/* wrap rows properly, even though they are inverted in memory */
					while (bytes_to_read > 0) {
						if ((scanWidth - col_offset) == 1) {
							/* special case: a pair of pixels is split across two rows. */
							BYTE pixels, same_row_pixel, next_row_pixel;
							int pixel_index = row_offset + col_offset / 2;

							if (pixel_index < 0 || pixel_index >= size)
								return;

							bytes_read = gdip_read_bmp_data (pointer, &pixels, 1, source);

							if (bytes_read < 1)
								return; /* TODO?: Add an "unexpected end of file" error code */

							same_row_pixel = (pixels >> 4) & 0x0F;
							next_row_pixel =  pixels       & 0x0F;

							if ((col_offset & 1) != 0) {
								BYTE old_pixel = 0xF0 & scan0[pixel_index];
								scan0[pixel_index] = (old_pixel & 0xF0) | same_row_pixel;
							}
							else
								scan0[pixel_index] = same_row_pixel << 4;

							col_offset = 1;
							row_offset += row_delta;
							rows_remaining--;

							if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
								return;

							if (row_offset < 0 || row_offset >= size)
								return;

							scan0[row_offset] = next_row_pixel << 4;

							new_row = FALSE;
						}
						else if ((col_offset & 1) == 0) {
							/* alignment is good; we can read pairs of pixels as bytes.
							 * if there are an odd number of pixels in a scan, though,
							 * then the last pixel will need to be special-cased. also,
							 * if the scan width is odd, then a byte will be split
							 * across a row ending. I don't know if this is in the spec,
							 * but it is the most resistant to crashing.
							 */
							int bytes_to_read_this_scan = (scanWidth - col_offset) / 2;

							if (bytes_to_read_this_scan > bytes_to_read)
								bytes_to_read_this_scan = bytes_to_read;

							int pixel_index = row_offset + col_offset / 2;

							if (pixel_index < 0 || pixel_index >= size)
								return;

							bytes_read = gdip_read_bmp_data (
								pointer,
								&scan0[pixel_index],
								bytes_to_read_this_scan,
								source);

							if (bytes_read < bytes_to_read_this_scan)
								return; /* TODO?: Add an "unexpected end of file" error code */

							col_offset += bytes_read * 2;
							bytes_to_read -= bytes_read;

							new_row = FALSE;
						}
						else {
							/* bad alignment; nybble-swapping will be required */
							int pixel_index = row_offset + col_offset / 2;

							if (pixel_index < 0 || pixel_index >= size)
								return;

							BYTE last_high_nybble = 0xF0 & scan0[pixel_index];

							int bytes_to_read_this_scan = (scanWidth - col_offset) / 2;

							if (bytes_to_read_this_scan > bytes_to_read)
								bytes_to_read_this_scan = bytes_to_read;

							while (bytes_to_read_this_scan >= 0) {
								BYTE pixels;

								bytes_read = gdip_read_bmp_data (pointer, &pixels, 1, source);

								if (bytes_read < 1)
									return; /* TODO?: Add an "unexpected end of file" error code */

								pixel_index = row_offset + col_offset / 2;

								if (pixel_index < 0 || pixel_index >= size)
									return;

								scan0[pixel_index] = last_high_nybble | (pixels >> 4);

								last_high_nybble = (pixels << 4) & 0xF0;

								col_offset += 2; /* two pixels processed */
								bytes_to_read_this_scan--;
							}

							new_row = FALSE;
						}

						if (col_offset >= scanWidth) {
							col_offset = 0;
							row_offset += row_delta;
							rows_remaining--;

							if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
								return;

							new_row = TRUE;
						}
					}

					if ((pixels_to_read & 1) != 0) {
						/* half of a byte remains to be inserted into the correct nybble */
						BYTE pixel;
						int pixel_index = row_offset + col_offset / 2;

						if (pixel_index < 0 || pixel_index >= size)
							return;

						bytes_read = gdip_read_bmp_data (pointer, &pixel, 1, source);

						if (bytes_read < 1)
							return; /* TODO?: Add an "unexpected end of file" error code */

						pixel >>= 4; /* the last pixel is in the high nybble */

						if ((col_offset & 1) != 0) {
							BYTE old_pixel = 0xF0 & scan0[pixel_index];
							scan0[pixel_index] = (old_pixel & 0xF0) | pixel;
						}
						else
							scan0[pixel_index] = pixel << 4;

						col_offset++;

						if (col_offset >= scanWidth) {
							col_offset = 0;
							row_offset += row_delta;
							rows_remaining--;

							if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
								return;

							new_row = TRUE;
						}
						else
							new_row = FALSE;
					}

					if (pad_byte_present) {
						bytes_read = gdip_read_bmp_data(pointer, &code, 1, source);

						if (bytes_read < 1)
							return; /* TODO?: Add an "unexpected end of file" error code */
					}

					break;
				}
			}
		}
		else {
			/* we have a run of length 'code'. the colour of the run is the next byte in the file.
			 * something weird is happening here in 4-bit land, though; a byte stores two pixels.
			 * what happens is rather odd: the run is actually of two alternating colours (which
			 * may, of course, be the same, but are not required to be). we need to make sure that
			 * the colours end up in the right nybbles of the output bytes.
			 */
			int run_pixels = code;
			int run_length = run_pixels / 2;

			BYTE pixel_values;
			BYTE inverted_pixel_values;

			bytes_read = gdip_read_bmp_data(pointer, &pixel_values, 1, source);

			if (bytes_read < 1)
				return; /* TODO?: Add an "unexpected end of file" error code */

			inverted_pixel_values = ((pixel_values & 0x0F) << 4) | ((pixel_values & 0xF0) >> 4);

			if ((col_offset & 1) != 0) {
				BYTE temp = inverted_pixel_values;
				inverted_pixel_values = pixel_values;
				pixel_values = temp;
			}

			while (run_length > 0) {
				if ((scanWidth - col_offset) == 1) {
					/* special case: a pair of pixels is split across two rows. */
					BYTE same_row_pixel = (pixel_values >> 4) & 0x0F;
					BYTE next_row_pixel =  pixel_values       & 0x0F;

					int pixel_index = row_offset + col_offset / 2;

					if (pixel_index < 0 || pixel_index >= size)
						return;

					if ((col_offset & 1) != 0) {
						BYTE old_pixel = 0xF0 & scan0[pixel_index];
						scan0[pixel_index] = (old_pixel & 0xF0) | same_row_pixel;
					}
					else
						scan0[pixel_index] = same_row_pixel << 4;

					col_offset = 1;
					row_offset += row_delta;
					rows_remaining--;

					if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
						return;

					if (row_offset < 0 || row_offset >= size)
						return;

					scan0[row_offset] = next_row_pixel << 4;

					new_row = FALSE;

					if ((scanWidth & 1) != 0) {
						/* if the width of the scan is odd, then the nybbles swap
						 * places each time they cross from one row to the next
						 */
						BYTE temp = inverted_pixel_values;
						inverted_pixel_values = pixel_values;
						pixel_values = temp;
					}
				}
				else {
					int bytes_to_run_this_scan;
					int pixel_index = row_offset + col_offset / 2;

					if (pixel_index < 0 || pixel_index >= size)
						return;

					/* make sure we're byte-aligned; if we're not, we need to store a nybble first */
					if ((col_offset & 1) != 0) {
						BYTE old_pixel = 0xF0 & scan0[pixel_index];
						scan0[pixel_index] = (old_pixel & 0xF0) | (pixel_values & 0x0F);

						col_offset++;
					}

					bytes_to_run_this_scan = (scanWidth - col_offset) / 2;

					if (bytes_to_run_this_scan > run_length)
						bytes_to_run_this_scan = run_length;

					if (bytes_to_run_this_scan < 0 || pixel_index + bytes_to_run_this_scan > size)
						return;

					memset (scan0 + pixel_index, pixel_values, bytes_to_run_this_scan);

					col_offset += bytes_to_run_this_scan * 2;
					run_length -= bytes_to_run_this_scan;

					if (col_offset >= scanWidth) {
						col_offset = 0;
						row_offset += row_delta;
						rows_remaining--;

						if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
							return;

						new_row = TRUE;

						if ((scanWidth & 1) != 0) {
							/* if the width of the scan is odd, then the nybbles swap
							 * places each time they cross from one row to the next
							 */
							BYTE temp = inverted_pixel_values;
							inverted_pixel_values = pixel_values;
							pixel_values = temp;
						}
					}
					else
						new_row = FALSE;
				}
			}

			if ((run_pixels & 1) != 0) {
				/* half of a byte remains to be inserted into the correct nybble */
				BYTE pixel = pixel_values >> 4; /* the last pixel is in the high nybble */
				int pixel_index = row_offset + col_offset / 2;

				if (pixel_index < 0 || pixel_index >= size)
					return;

				if ((col_offset & 1) != 0) {
					BYTE old_pixel = 0xF0 & scan0[pixel_index];
					scan0[pixel_index] = (old_pixel & 0xF0) | pixel;
				}
				else
					scan0[pixel_index] = pixel << 4;

				col_offset++;

				if (col_offset >= scanWidth) {
					col_offset = 0;
					row_offset += row_delta;
					rows_remaining--;

					if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
						return;

					new_row = TRUE;
				}
				else
					new_row = FALSE;
			}
		}
	}

	return;
}

GpStatus
gdip_read_BITMAPINFOHEADER (void *pointer, ImageSource source, BITMAPV5HEADER *bmi, BOOL *upsidedown)
{
	DWORD dw = 0;
	BYTE *data_read = (BYTE*)&dw;
	int size = sizeof (DWORD);
	int size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;

	DWORD headerSize = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
	switch (headerSize) {
	case BITMAPCOREHEADER_SIZE:
		bmi->bV5Size = headerSize;

		/* Old OS/2 format. Width and Height fields are WORDs instead of DWORDS */
		dw = 0;
		size_read = gdip_read_bmp_data (pointer, data_read, size, source);
		if (size_read < size)
			return OutOfMemory;
		bmi->bV5Width = (data_read[1]<<8 | data_read[0]);
		bmi->bV5Height = (data_read[3]<<8 | data_read[2]);

		break;
	case sizeof (BITMAPINFOHEADER):
	case sizeof (BITMAPV3HEADER):
	case sizeof (BITMAPV4HEADER):
	case sizeof (BITMAPV5HEADER):
		bmi->bV5Size = headerSize;

		dw = 0;
		size_read = gdip_read_bmp_data (pointer, data_read, size, source);
		if (size_read < size)
			return OutOfMemory;
		bmi->bV5Width = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

		// Width can't be negative (as opposed to Height where that indicates a top-down image)
		if (bmi->bV5Width <= 0)
			return OutOfMemory;

		dw = 0;
		size_read = gdip_read_bmp_data (pointer, data_read, size, source);
		if (size_read < size)
			return OutOfMemory;
		bmi->bV5Height = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

		if (bmi->bV5Height == 0)
			return OutOfMemory;

		break;
	default:
		/* This is an unknown or invalid header. */
		return OutOfMemory;
	}

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5Planes = (data_read[1]<<8 | data_read[0]);
	bmi->bV5BitCount = (data_read[3]<<8 | data_read[2]);

	/* The OS/2 format doesn't have any of these other fields */
	if (bmi->bV5Size == BITMAPCOREHEADER_SIZE) {
		bmi->bV5Compression = 0;
		bmi->bV5SizeImage = 0;
		bmi->bV5XPelsPerMeter = 0;
		bmi->bV5YPelsPerMeter = 0;
		bmi->bV5ClrUsed = 0;
		bmi->bV5ClrImportant = 0;

		return Ok;
	}

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5Compression = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

 	/* If the height is negative then the bitmap is sideup. */
	if (bmi->bV5Height < 0) {
		*upsidedown = FALSE;
		bmi->bV5Height = -bmi->bV5Height;
	}

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5SizeImage = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5XPelsPerMeter = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5YPelsPerMeter = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5ClrUsed = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5ClrImportant = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	/* We've finished reading the BITMAPINFOHEADER but later versions are larger. */
	if (bmi->bV5Size == sizeof (BITMAPINFOHEADER)) {
		// A 16bpp BITMAPINFOHEADER BI_BITFIELDS image is followed by a red, green and blue mask.
		if (bmi->bV5BitCount != 16 || bmi->bV5Compression != BI_BITFIELDS)
			return Ok;
	}

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5RedMask = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5GreenMask = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5BlueMask = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	if (bmi->bV5Size == sizeof (BITMAPINFOHEADER))
		return Ok;

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5AlphaMask = ((guint32)data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	/* We don't use any information in BITMAPV4HEADER or BITMAPV5HEADER, so there is no point going through all of
	 * the other fields. This leaves the rest of the structure uninitialized. */
	size = headerSize - sizeof (BITMAPV3HEADER);
	if (size > 0) {
		while (size > sizeof (DWORD)) {
			if (gdip_read_bmp_data (pointer, data_read, sizeof (DWORD), source) != sizeof (DWORD))
				return OutOfMemory;
			size -= sizeof (DWORD);
		}
		if (gdip_read_bmp_data (pointer, data_read, size, source) != size)
			return OutOfMemory;
	}

    return Ok;
}

static GpStatus
gdip_readbmp_palette (void *pointer, ImageSource source, const BITMAPV5HEADER *bmi, ColorPalette **result)
{
	UINT numberOfColors = bmi->bV5ClrUsed;
	if (bmi->bV5BitCount <= 8) {
		int defaultNumberOfColors = 1 << bmi->bV5BitCount;
		// A color count of 0 means use the default. Also use the default color count
		// if the bitmap has specified an unsupported number of colors (i.e. greater
		// than the default).
		if (bmi->bV5ClrUsed == 0 || bmi->bV5ClrUsed > defaultNumberOfColors)
			numberOfColors = defaultNumberOfColors;
	}

	// Nothing to do.
	if (numberOfColors == 0)
		return Ok;

	BYTE buffer[4];
	INT colorEntrySize = bmi->bV5Size == BITMAPCOREHEADER_SIZE ? 3 : 4;
	unsigned long long int palette_size = (unsigned long long int)sizeof (ColorPalette) + sizeof (ARGB) * numberOfColors;

	/* ensure total 'palette_size' does not overflow an integer and fits inside our 2GB limit */
	if (palette_size > G_MAXINT32) {
		return OutOfMemory;
	}

	ColorPalette *palette = GdipAlloc (palette_size);
	if (!palette)
		return OutOfMemory;
	
	palette->Flags = 0;
	palette->Count = numberOfColors;

	for (int i = 0; i < palette->Count; i++) {
		int size_read = gdip_read_bmp_data (pointer, buffer, colorEntrySize, source);
		if (size_read < colorEntrySize) {
			GdipFree (palette);
			return OutOfMemory;
		}

		set_pixel_bgra (palette->Entries, i * 4, buffer[0], buffer[1], buffer[2], 0xFF);
	}

	*result = palette;
	return Ok;
}

static GpStatus
gdip_read_bmp_scans (void *pointer, BYTE *pixels, BOOL upsidedown, PixelFormat format, INT srcStride, INT destStride, INT width, INT height, ImageSource source)
{
	BYTE *scan = (BYTE *) GdipAlloc (srcStride);
	if (!scan)
		return OutOfMemory;

	for (int y = 0; y < height; y++) {
		int currentLine = upsidedown ? height - y - 1 : y;
		int size_read = gdip_read_bmp_data (pointer, scan, srcStride, source);
		if (size_read < srcStride) {
			GdipFree (scan);
			return OutOfMemory;
		}

		BYTE *destScan = pixels + currentLine * destStride;
		switch (format) {
			case PixelFormat1bppIndexed:
			case PixelFormat4bppIndexed:
			case PixelFormat8bppIndexed:
				memcpy (destScan, scan, srcStride);
				continue;
			case PixelFormat16bppRGB555: {
				for (int x = 0; x < width; x++) {
					ARGB argb = gdip_getpixel_16bppRGB555 (scan, x);

					BYTE a = (argb & 0xFF000000) >> 24;
					BYTE r = (argb & 0x00FF0000) >> 16;
					BYTE g = (argb & 0x0000FF00) >> 8;
					BYTE b = (argb & 0x000000FF);
					gdip_setpixel_32bppARGB (destScan, x, a, r, g, b);
				}
				continue;
			}
			case PixelFormat16bppRGB565: {
				for (int x = 0; x < width; x++) {
					ARGB argb = gdip_getpixel_16bppRGB565 (scan, x);

					BYTE a = (argb & 0xFF000000) >> 24;
					BYTE r = (argb & 0x00FF0000) >> 16;
					BYTE g = (argb & 0x0000FF00) >> 8;
					BYTE b = (argb & 0x000000FF);
					gdip_setpixel_32bppARGB (destScan, x, a, r, g, b);
				}
				continue;
			}
			case PixelFormat24bppRGB: {
				for (int x = 0; x < width; x++) {
					gdip_setpixel_32bppARGB (destScan, x, 0xFF, scan[x * 3 + 2], scan[x * 3 + 1], scan[x * 3]);
				}
				continue;
			}
			case PixelFormat32bppRGB: {
				for (int x = 0; x < width; x++) {
					gdip_setpixel_32bppARGB (destScan, x, 0xFF, scan[x * 4 + 2], scan[x * 4 + 1], scan[x * 4]);
				}
				continue;
			}
			default:
				GdipFree(scan);
				return NotImplemented;
		}
	}

	GdipFree(scan);
	return Ok;
}

static GpStatus
gdip_read_bmp_indexed (void *pointer, BYTE *pixels, BOOL upsidedown, PixelFormat format, INT stride, INT width, INT height, ImageSource source)
{
	if (upsidedown)
		return gdip_read_bmp_scans (pointer, pixels, upsidedown, format, stride, stride, width, height, source);

	int size_read = gdip_read_bmp_data (pointer, pixels, stride * height, source);
	if (size_read < stride)
		return OutOfMemory;

	return Ok;
}

/* For use with in-memory bitmaps, where the BITMAPFILEHEADER doesn't exists */
GpStatus 
gdip_read_bmp_image (void *pointer, GpImage **image, ImageSource source)
{
	BITMAPV5HEADER bmi;
	GpBitmap	*result;
	BYTE		*pixels;
	PixelFormat	originalFormat;
	INT	originalStride;
	BOOL		upsidedown = TRUE;
	GpStatus	status;
	unsigned long long int size;

	status = gdip_read_BITMAPINFOHEADER (pointer, source, &bmi, &upsidedown);
	if (status != Ok)
		return status;

	result = gdip_bitmap_new_with_frame (NULL, TRUE);
	if (!result)
		return OutOfMemory;

	status = gdip_get_bmp_pixelformat (&bmi, &originalFormat, &result->active_bitmap->pixel_format);
	if (status != Ok) {
		gdip_bitmap_dispose (result);
		return status;
	}

	status = gdip_get_bmp_stride (result->active_bitmap->pixel_format, bmi.bV5Width, &result->active_bitmap->stride, /* cairoHacks */ TRUE);
	if (status != Ok) {
		gdip_bitmap_dispose (result);
		return status;
	}

	status = gdip_get_bmp_stride (originalFormat, bmi.bV5Width, &originalStride, /* cairoHacks */ FALSE);
	if (status != Ok) {
		gdip_bitmap_dispose (result);
		return status;
	}

	result->type = ImageTypeBitmap;
	result->image_format = BMP;
	result->active_bitmap->width = bmi.bV5Width;
	result->active_bitmap->height = bmi.bV5Height;
 
 	status = gdip_readbmp_palette (pointer, source, &bmi, &result->active_bitmap->palette);
	if (status != Ok) {
		gdip_bitmap_dispose (result);
		return status;
	}

	/* ensure total 'size' does not overflow an integer and fits inside our 2GB limit */
	size = (unsigned long long int)result->active_bitmap->stride * result->active_bitmap->height;
	if (size > G_MAXINT32) {
		gdip_bitmap_dispose (result);
		return OutOfMemory;
	}

	pixels = GdipAlloc (size);
	if (!pixels) {
		gdip_bitmap_dispose (result);
		return OutOfMemory;
	}

	if (gdip_is_an_indexed_pixelformat (result->active_bitmap->pixel_format)) {
		if (bmi.bV5Compression == BI_RLE4)
			gdip_read_bmp_rle_4bit (pointer, pixels, upsidedown, result->active_bitmap->stride, result->active_bitmap->width, result->active_bitmap->height, source);
		else if (bmi.bV5Compression == BI_RLE8)
			gdip_read_bmp_rle_8bit (pointer, pixels, upsidedown, result->active_bitmap->stride, result->active_bitmap->width, result->active_bitmap->height, source);
		else {
			status = gdip_read_bmp_indexed (pointer, pixels, upsidedown, originalFormat, originalStride, result->active_bitmap->width, result->active_bitmap->height, source);
			if (status != Ok) {
				gdip_bitmap_dispose (result);
				return status;
			}
		}
	} else {
		status = gdip_read_bmp_scans (pointer, pixels, upsidedown, originalFormat, originalStride, result->active_bitmap->stride, result->active_bitmap->width, result->active_bitmap->height, source);
		if (status != Ok) {
			gdip_bitmap_dispose (result);
			return status;
		}
	}

	result->active_bitmap->scan0 = pixels;
	result->active_bitmap->reserved = GBD_OWN_SCAN0;
	result->active_bitmap->image_flags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | ImageFlagsColorSpaceRGB;
	if (bmi.bV5XPelsPerMeter != 0 && bmi.bV5YPelsPerMeter != 0)
		result->active_bitmap->image_flags |= ImageFlagsHasRealDPI;

	*image = result;
	return Ok;
}

/* BMP read from files have a BITMAPFILEHEADER but this isn't the case for the GDI API
 * (e.g. displaying a bitmap) */
static void
BitmapFileHeaderFromLE (BITMAPFILEHEADER *bitmapFileHeader)
{
#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	bitmapFileHeader->bfType = GUINT16_FROM_LE (bitmapFileHeader->bfType);
	bitmapFileHeader->bfSize = GUINT32_FROM_LE (bitmapFileHeader->bfSize);
	bitmapFileHeader->bfReserved1 = GUINT16_FROM_LE (bitmapFileHeader->bfReserved1);
	bitmapFileHeader->bfReserved2 = GUINT16_FROM_LE (bitmapFileHeader->bfReserved2);
	bitmapFileHeader->bfOffBits = GUINT32_FROM_LE (bitmapFileHeader->bfOffBits);
#endif
}

static GpStatus 
gdip_read_bmp_image_from_file_stream (void *pointer, GpImage **image, ImageSource source)
{
	BITMAPFILEHEADER bmfh;
	int size_read;

	size_read = gdip_read_bmp_data (pointer, (BYTE *) &bmfh, sizeof (bmfh), source);
	if (size_read < sizeof (bmfh)) {
		return OutOfMemory;
	}

	BitmapFileHeaderFromLE (&bmfh);
	if (bmfh.bfType != BFT_BITMAP) {
		return UnknownImageFormat;
	}

	return gdip_read_bmp_image (pointer, image, source);
}

GpStatus 
gdip_load_bmp_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_read_bmp_image_from_file_stream ((void*)fp, image, File);
}

GpStatus 
gdip_load_bmp_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return gdip_read_bmp_image_from_file_stream ((void*)loader, image, DStream);
}

int 
gdip_read_bmp_data (void *pointer, BYTE *data, int size, ImageSource source)
{
	switch (source) {
	case File:
		return fread (data, 1, size, (FILE*) pointer);
	case DStream: {
		/* Streams are not required to return the number of bytes
		   requested, they could return less yet our code seems to assume
		   it will always get what it's asking for; lets loop until we
		   get what was requested or we get an error */
		int got;
		int total;
		dstream_t *loader;

		loader = (dstream_t *) pointer;
		total = 0;

		do {
			got = dstream_read (loader, data + total, size - total, 0);
			if (got < 1) {  /*  0 = end of stream, -1 = error */
				return total;
			}
			total += got;
		} while (total < size);

		return total;
	}
	case Memory: {
		MemorySource *ms = (MemorySource*)pointer;
		int len = (ms->pos + size < ms->size) ? size : ms->size - ms->pos;
		if (len > 0) {
			memcpy (data, ms->ptr + ms->pos, len);
			ms->pos += len;
		}
		return len;
	}
	default:
		return -1;
	}
}

static void 
gdip_write_bmp_data (void *pointer, BYTE *data, int size, BOOL useFile)
{
	if (useFile)
		fwrite (data, 1, size, (FILE*) pointer);
	else 	
		((PutBytesDelegate)(pointer))(data, size);	
}

static GpStatus 
gdip_save_bmp_image_to_file_stream (void *pointer, GpImage *image, BOOL useFile)
{
	BITMAPFILEHEADER	bmfh;
	BITMAPINFOHEADER	bmi;
	int			bitmapLen;
	int			i;
	ARGB			color;
	int			colours = 0;
	ARGB			*entries;
	int			palette_entries;
	ActiveBitmapData		*activebmp;
	BYTE			*scan0;

	activebmp = image->active_bitmap;
	if (activebmp->pixel_format != PixelFormat24bppRGB) {
		bitmapLen = activebmp->stride * activebmp->height;
	} else {
		bitmapLen = activebmp->width * 3;
		bitmapLen += 3;
		bitmapLen &= ~3;
		bitmapLen *= activebmp->height;
	}

	if (activebmp->palette) {
			colours = activebmp->palette->Count;
	}

	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfType = BFT_BITMAP;
	bmfh.bfOffBits = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER) + colours * sizeof (RGBQUAD);
	bmfh.bfSize = (bmfh.bfOffBits + bitmapLen);
	BitmapFileHeaderFromLE (&bmfh);

	gdip_write_bmp_data (pointer, (BYTE *) &bmfh, sizeof (bmfh), useFile);
	gdip_bitmap_fill_info_header (image, &bmi);
	gdip_write_bmp_data (pointer, (BYTE*) &bmi, sizeof (bmi), useFile);

	if (colours) {
		palette_entries = activebmp->palette->Count;

		if (activebmp->pixel_format == PixelFormat4bppIndexed) {
			palette_entries = 16;
		}

		entries = (ARGB *) GdipAlloc (palette_entries * sizeof (ARGB));
		if (entries == NULL)
			return OutOfMemory;

		for (i = 0; i < palette_entries; i++) {
			color = activebmp->palette->Entries[i];
#if G_BYTE_ORDER == G_LITTLE_ENDIAN
			*(entries + i) = color;
#else
			*(entries + i) = GUINT32_FROM_LE (color);
#endif
		}
		gdip_write_bmp_data (pointer, (BYTE *) entries, palette_entries * sizeof (ARGB), useFile);
		GdipFree (entries);
	}

	scan0 = activebmp->scan0;
	if (activebmp->pixel_format == PixelFormat24bppRGB) {
		int width = activebmp->width;
		int height = activebmp->height;
		int mystride;
		int k;
		BYTE *current_line;

		/* rows need to be padded up to the next multiple of 4 */
		mystride = width * 3;
		mystride += 3;
		mystride &= ~3;
		current_line = (BYTE*) GdipAlloc (mystride);
		if (!current_line) {
			return OutOfMemory;
		}

		memset (current_line, 0, mystride); /* Zero padding at the end if needed */
		for (i = height - 1; i >= 0; i--) {
			BYTE *ptr;
			guint32 *iptr;

			iptr = (guint32 *) (scan0 + i * activebmp->stride);
			ptr = current_line;
			for (k = 0; k < width; k++) {
				guint32 color = *iptr++;
				*ptr++ = (color & 0x000000ff);
				*ptr++ = ((color & 0x0000ff00) >> 8);
				*ptr++ = ((color & 0x00ff0000) >> 16);
			}
			gdip_write_bmp_data (pointer, current_line, mystride, useFile);
		}
		GdipFree (current_line);
		return Ok;
	}

	/* Writes bitmap upside down. Many tools can only process bmp stored this way*/        
#ifdef WORDS_BIGENDIAN
	if (gdip_is_an_indexed_pixelformat (activebmp->pixel_format) == FALSE) {
		int j;
		BYTE *row_pointer = GdipAlloc (activebmp->width * 4);

		if (row_pointer == NULL) {
			return OutOfMemory;
		}

		for (i = activebmp->height -1; i >= 0; i--) {
			for (j = 0; j < activebmp->width; j++) {
				row_pointer[j*4] = *((BYTE*)scan0 + (activebmp->stride * i) + (j*4) + 3); 
				row_pointer[j*4+1] = *((BYTE*)scan0 + (activebmp->stride * i) + (j*4) + 2); 
				row_pointer[j*4+2] = *((BYTE*)scan0 + (activebmp->stride * i) + (j*4) + 1); 
				row_pointer[j*4+3] = *((BYTE*)scan0 + (activebmp->stride * i) + (j*4) + 0); 
			}
			gdip_write_bmp_data (pointer, row_pointer, activebmp->stride, useFile);
		}
		GdipFree (row_pointer);
	}
	else
#endif /* WORDS_BIGENDIAN */
	for (i = activebmp->height - 1; i >= 0; i--) {
		gdip_write_bmp_data (pointer, scan0 + i * activebmp->stride, activebmp->stride, useFile);
	}

	return Ok;
}

GpStatus 
gdip_save_bmp_image_to_file (FILE *fp, GpImage *image)
{
	return gdip_save_bmp_image_to_file_stream ( (void *)fp, image, TRUE);
}

GpStatus 
gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image)
{	
	return gdip_save_bmp_image_to_file_stream ( (void *)putBytesFunc, image, FALSE);
}
