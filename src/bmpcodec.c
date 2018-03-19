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
static const WCHAR bmp_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'B', 'M', 'P', 0}; /* Built-in BMP */
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
gdip_get_bmp_pixelformat (BITMAPV5HEADER *bih, PixelFormat *dest)
{
	int bitCount = bih->bV5BitCount;
	int compression = bih->bV5Compression;

	switch (compression) {
	case BI_BITFIELDS:
		if (bitCount != 16)
			return OutOfMemory;
		/* note: incomplete at this stage */
		*dest = PixelFormat16bppRGB565;
		break;
	default:
		switch (bitCount) {
		case 64:
			*dest = PixelFormat64bppARGB;
			break;
		case 32:
			*dest = PixelFormat32bppRGB;
			break;
		case 24:
			*dest = PixelFormat24bppRGB;
			break;
		case 16:
			/* MS produce such files (i.e. bad header) for storing ImageList bitmaps, see bug #80797 */
			*dest = PixelFormat16bppRGB565;
			break;
		case 8:
			*dest = PixelFormat8bppIndexed;
			break;
		case 4:
			*dest = PixelFormat4bppIndexed;
			break;
		case 1:
			*dest = PixelFormat1bppIndexed;
			break;
		default:
			return OutOfMemory;
		}  
	}

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

static GpStatus
gdip_read_bmp_rle_8bit (void *pointer, BYTE *scan0, BOOL upsidedown, int stride, int scanWidth, int scanCount, ImageSource source)
{
	BYTE code;
	int bytes_read;

	int col_offset = 0;
	int row_offset = (upsidedown ? (scanCount - 1) * stride : 0);
	int row_delta = (upsidedown ? -stride : +stride);
	int rows_remaining = scanCount;
	BOOL new_row = FALSE;

	if (scanWidth > stride)
		return InvalidParameter;

	while ((rows_remaining > 0)
	    || ((row_offset == 0) && (col_offset < scanWidth))) {
		bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

		if (bytes_read < 1)
			return GenericError; /* TODO?: Add an "unexpected end of file" error code */

		if (code == 0) { /* RLE escape code */
			bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

			if (bytes_read < 1)
				return GenericError; /* TODO?: Add an "unexpected end of file" error code */

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
					return Ok;
				}
				case 2: /* jump forward (dx, dy) coordinates */
				{
					BYTE dx, dy;

					bytes_read  = gdip_read_bmp_data (pointer, &dx, 1, source);
					bytes_read += gdip_read_bmp_data (pointer, &dy, 1, source);

					if (bytes_read < 2)
						return GenericError; /* TODO?: Add an "unexpected end of file" error code */

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

						bytes_read = gdip_read_bmp_data (
							pointer,
							&scan0[row_offset + col_offset],
							bytes_to_read_this_scan,
							source);

						if (bytes_read < bytes_to_read_this_scan)
							return GenericError; /* TODO?: Add an "unexpected end of file" error code */

						col_offset += bytes_read;
						bytes_to_read -= bytes_read;

						if (col_offset >= scanWidth) 
						{
							col_offset = 0;
							row_offset += row_delta;
							rows_remaining--;

							if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
								return Ok;

							new_row = TRUE;
						}
						else
							new_row = FALSE;
					}

					if (pad_byte_present) {
						bytes_read = gdip_read_bmp_data(pointer, &code, 1, source);

						if (bytes_read < 1)
							return GenericError; /* TODO?: Add an "unexpected end of file" error code */
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
				return GenericError; /* TODO?: Add an "unexpected end of file" error code */

			while (run_length > 0) {
				int bytes_to_run_this_scan = scanWidth - col_offset;

				if (bytes_to_run_this_scan > run_length)
					bytes_to_run_this_scan = run_length;

				memset (scan0 + row_offset + col_offset, pixel_value, bytes_to_run_this_scan);

				col_offset += bytes_to_run_this_scan;
				run_length -= bytes_to_run_this_scan;

				if (col_offset >= scanWidth) 
				{
					col_offset = 0;
					row_offset += row_delta;
					rows_remaining--;

					if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
						return Ok;

					new_row = TRUE;
				}
				else
					new_row = FALSE;
			}
		}
	}

	return Ok;
}

static GpStatus
gdip_read_bmp_rle_4bit (void *pointer, BYTE *scan0, BOOL upsidedown, int stride, int scanWidth, int scanCount, ImageSource source)
{
	BYTE code;
	int bytes_read;

	int col_offset = 0;
	int row_offset = (upsidedown ? (scanCount - 1) * stride : 0);
	int row_delta = (upsidedown ? -stride : +stride);
	int rows_remaining = scanCount;
	BOOL new_row = FALSE;

	if ((scanWidth & 1) != 0)
		scanWidth++;

	if (scanWidth > stride * 2)
		return InvalidParameter;

	while (rows_remaining > 0) {
		bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

		if (bytes_read < 1)
			return GenericError; /* TODO?: Add an "unexpected end of file" error code */

		if (code == 0) { /* RLE escape code */
			bytes_read = gdip_read_bmp_data (pointer, &code, 1, source);

			if (bytes_read < 1)
				return GenericError; /* TODO?: Add an "unexpected end of file" error code */

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
					return Ok;
				}
				case 2: /* jump forward (dx, dy) coordinates */
				{
					BYTE dx, dy;

					bytes_read  = gdip_read_bmp_data (pointer, &dx, 1, source);
					bytes_read += gdip_read_bmp_data (pointer, &dy, 1, source);

					if (bytes_read < 2)
						return GenericError; /* TODO?: Add an "unexpected end of file" error code */

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

							bytes_read = gdip_read_bmp_data (pointer, &pixels, 1, source);

							if (bytes_read < 1)
								return GenericError; /* TODO?: Add an "unexpected end of file" error code */

							same_row_pixel = (pixels >> 4) & 0x0F;
							next_row_pixel =  pixels       & 0x0F;

							if ((col_offset & 1) != 0) {
								BYTE old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
								scan0[row_offset + col_offset / 2] = (old_pixel & 0xF0) | same_row_pixel;
							}
							else
								scan0[row_offset + col_offset / 2] = same_row_pixel << 4;

							col_offset = 1;
							row_offset += row_delta;
							rows_remaining--;

							if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
								return Ok;

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

							bytes_read = gdip_read_bmp_data (
								pointer,
								&scan0[row_offset + col_offset / 2],
								bytes_to_read_this_scan,
								source);

							if (bytes_read < bytes_to_read_this_scan)
								return GenericError; /* TODO?: Add an "unexpected end of file" error code */

							col_offset += bytes_read * 2;
							bytes_to_read -= bytes_read;

							new_row = FALSE;
						}
						else {
							/* bad alignment; nybble-swapping will be required */
							BYTE last_high_nybble = 0xF0 & scan0[row_offset + col_offset / 2];

							int bytes_to_read_this_scan = (scanWidth - col_offset) / 2;

							if (bytes_to_read_this_scan > bytes_to_read)
								bytes_to_read_this_scan = bytes_to_read;

							while (bytes_to_read_this_scan >= 0) {
								BYTE pixels;

								bytes_read = gdip_read_bmp_data (pointer, &pixels, 1, source);

								if (bytes_read < 1)
									return GenericError; /* TODO?: Add an "unexpected end of file" error code */

								scan0[row_offset + col_offset / 2] = last_high_nybble | (pixels >> 4);

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
								return Ok;

							new_row = TRUE;
						}
					}

					if ((pixels_to_read & 1) != 0) {
						/* half of a byte remains to be inserted into the correct nybble */
						BYTE pixel;

						bytes_read = gdip_read_bmp_data (pointer, &pixel, 1, source);

						if (bytes_read < 1)
							return GenericError; /* TODO?: Add an "unexpected end of file" error code */

						pixel >>= 4; /* the last pixel is in the high nybble */

						if ((col_offset & 1) != 0) {
							BYTE old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
							scan0[row_offset + col_offset / 2] = (old_pixel & 0xF0) | pixel;
						}
						else
							scan0[row_offset + col_offset / 2] = pixel << 4;

						col_offset++;

						if (col_offset >= scanWidth) {
							col_offset = 0;
							row_offset += row_delta;
							rows_remaining--;

							if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
								return Ok;

							new_row = TRUE;
						}
						else
							new_row = FALSE;
					}

					if (pad_byte_present) {
						bytes_read = gdip_read_bmp_data(pointer, &code, 1, source);

						if (bytes_read < 1)
							return GenericError; /* TODO?: Add an "unexpected end of file" error code */
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
				return GenericError; /* TODO?: Add an "unexpected end of file" error code */

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

					if ((col_offset & 1) != 0) {
						BYTE old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
						scan0[row_offset + col_offset / 2] = (old_pixel & 0xF0) | same_row_pixel;
					}
					else
						scan0[row_offset + col_offset / 2] = same_row_pixel << 4;

					col_offset = 1;
					row_offset += row_delta;
					rows_remaining--;

					if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
						return Ok;

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

					/* make sure we're byte-aligned; if we're not, we need to store a nybble first */
					if ((col_offset & 1) != 0) {
						BYTE old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
						scan0[row_offset + col_offset / 2] = (old_pixel & 0xF0) | (pixel_values & 0x0F);

						col_offset++;
					}

					bytes_to_run_this_scan = (scanWidth - col_offset) / 2;

					if (bytes_to_run_this_scan > run_length)
						bytes_to_run_this_scan = run_length;

					memset (scan0 + row_offset + col_offset / 2, pixel_values, bytes_to_run_this_scan);

					col_offset += bytes_to_run_this_scan * 2;
					run_length -= bytes_to_run_this_scan;

					if (col_offset >= scanWidth) {
						col_offset = 0;
						row_offset += row_delta;
						rows_remaining--;

						if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
							return Ok;

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

				if ((col_offset & 1) != 0) {
					BYTE old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
					scan0[row_offset + col_offset / 2] = (old_pixel & 0xF0) | pixel;
				}
				else
					scan0[row_offset + col_offset / 2] = pixel << 4;

				col_offset++;

				if (col_offset >= scanWidth) {
					col_offset = 0;
					row_offset += row_delta;
					rows_remaining--;

					if (rows_remaining <= 0) /* more data than expected -- let's not make this a fatal error */
						return Ok;

					new_row = TRUE;
				}
				else
					new_row = FALSE;
			}
		}
	}

	return Ok;
}

GpStatus
gdip_read_BITMAPINFOHEADER (void *pointer, ImageSource source, BITMAPV5HEADER *bmi, BOOL *os2format, BOOL *upsidedown)
{
	DWORD dw = 0;
	BYTE *data_read = (BYTE*)&dw;
	int size = sizeof (DWORD);
	int size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;

	DWORD headerSize = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
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
		*os2format = TRUE;

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
		bmi->bV5Width = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

		dw = 0;
		size_read = gdip_read_bmp_data (pointer, data_read, size, source);
		if (size_read < size)
			return OutOfMemory;
		bmi->bV5Height = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

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
	if (*os2format) {
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
	bmi->bV5Compression = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

 	/* If the height is negative then the bitmap is sideup. */
	if (bmi->bV5Height < 0) {
		*upsidedown = FALSE;
		bmi->bV5Height = -bmi->bV5Height;
	}

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5SizeImage = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5XPelsPerMeter = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5YPelsPerMeter = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5ClrUsed = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5ClrImportant = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	/* We've finished reading the BITMAPINFOHEADER but later versions are larger. */
	if (bmi->bV5Size == sizeof (BITMAPINFOHEADER))
		return Ok;

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5RedMask = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5GreenMask = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5BlueMask = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, source);
	if (size_read < size)
		return OutOfMemory;
	bmi->bV5AlphaMask = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	/* We don't use any information in BITMAPV4HEADER or BITMAPV5HEADER, so there is no point going through all of
	 * the other fields. This leaves the rest of the structure uninitialized. */
	size = headerSize - sizeof (BITMAPV3HEADER);
	if (size > 0) {
		while (size > sizeof (DWORD)) {
			if (gdip_read_bmp_data (pointer, data_read, sizeof (DWORD), source) != sizeof (DWORD))
				return OutOfMemory;
			size -= sizeof (DWORD);
		}
		if (size > 0) {
			if (gdip_read_bmp_data (pointer, data_read, size, source) != size)
				return OutOfMemory;
		}
	}

    return Ok;
}

/* For use with in-memory bitmaps, where the BITMAPFILEHEADER doesn't exists */
GpStatus 
gdip_read_bmp_image (void *pointer, GpImage **image, ImageSource source)
{
	BITMAPV5HEADER bmi;
	GpBitmap	*result = NULL;
	BYTE		*pixels = NULL;
	int		i;
	PixelFormat	format;
	int		colours;
	BOOL		os2format = FALSE;
	BOOL		upsidedown = TRUE;
	int		size_read;
	BYTE		*data_read = NULL;
	int		line;
	int		loop;
	long		index;
	GpStatus	status;
	ARGB red_mask = 0;
	ARGB green_mask = 0;
	ARGB blue_mask = 0;
	int red_shift = 0;
	unsigned long long int size;

	status = gdip_read_BITMAPINFOHEADER (pointer, source, &bmi, &os2format, &upsidedown);
	if (status != Ok)
		goto error;

	colours = (bmi.bV5ClrUsed == 0 && bmi.bV5BitCount <= 8) ? (1 << bmi.bV5BitCount) : bmi.bV5ClrUsed;

	status = gdip_get_bmp_pixelformat (&bmi, &format);
	if (status != Ok) {
		 /* bit count mismatch */
		goto error;
	}

	/* for 16bbp images we need to be more precise */
	if (format == PixelFormat16bppRGB565) {
		/* Newer versions of BITMAPINFOHEADER contains R, G and B masks.*/
		if (bmi.bV5Size >= sizeof (BITMAPV3HEADER)) {
			red_mask = bmi.bV5RedMask;
			green_mask = bmi.bV5GreenMask;
			blue_mask = bmi.bV5BlueMask;
		} else if (bmi.bV5Size == sizeof (BITMAPINFOHEADER)) {
			/* The BITMAPINFOHEADER must be followed by R, G and B masks. */
			int size = sizeof (RGBQUAD);
			size_read = gdip_read_bmp_data (pointer, (void*)&red_mask, size, source);
			if (size_read != size) {
				status = OutOfMemory;
				goto error;
			}
			size_read = gdip_read_bmp_data (pointer, (void*)&green_mask, size, source);
			if (size_read != size) {
				status = OutOfMemory;
				goto error;
			}
			size_read = gdip_read_bmp_data (pointer, (void*)&blue_mask, size, source);
			if (size_read != size) {
				status = OutOfMemory;
				goto error;
			}
		}

		if ((red_mask == 0x7C00) && (green_mask == 0x3E0) && (blue_mask == 0x1F)) {
			/* five red bits, five green bits and five blue bits (0x7FFF) */
			red_shift = 10;
		} else if ((red_mask == 63488) && (green_mask == 2016) && (blue_mask == 31)) {
			/* five red bits, six green bits and five blue bits (0xFFFF) */
			red_shift = 11;
		} else {
			/* MS produce such files (i.e. missing masks) for storing ImageList bitmaps, see bug #80797 */
			red_mask = 0x7C00;
			green_mask = 0x3E0;
			blue_mask = 0x1F;
			red_shift = 10;
		}

		/* note: CAIRO_FORMAT_RGB16_565 is deprecated so we're promoting the bitmap to 32RGB */
		/* why 32bpp when 24 would be enough ? because MS GDI+ loads them as such, but can't display them (empty) */
		format = PixelFormat32bppRGB;
		/* 16bbp bitmap don't seems reversed like their height indicates */
		upsidedown = FALSE;
	}

	result = gdip_bitmap_new_with_frame (NULL, TRUE);
	if (!result) {
		status = OutOfMemory;
		goto error;
	}

	result->type = ImageTypeBitmap;
	result->image_format = BMP;
	result->active_bitmap->pixel_format = format;
	result->active_bitmap->width = bmi.bV5Width;
	result->active_bitmap->height = bmi.bV5Height;

	/* biWidth and biHeight are LONG (32 bits signed integer) */
	size = bmi.bV5Width;

	switch (result->active_bitmap->pixel_format) {
	case PixelFormat1bppIndexed:
		result->active_bitmap->stride = (size + 7) / 8;
		break;
	case PixelFormat4bppIndexed:
		result->active_bitmap->stride = (size + 1) / 2;
		break;
	case PixelFormat8bppIndexed:
		result->active_bitmap->stride = size;
		break;
	default:
		/* For other types, we assume 32 bit and translate into 32 bit from source format */
		result->active_bitmap->pixel_format = PixelFormat32bppRGB;
		/* fall-thru */
	case PixelFormat24bppRGB:
	case PixelFormat64bppARGB:
		/* stride is a (signed) _int_ and once multiplied by 4 it should hold a value that can be allocated by GdipAlloc
		 * this effectively limits 'width' to 536870911 pixels */
		size *= 4;
		if (size > G_MAXINT32) {
			status = InvalidParameter;
			goto error;
		}
		result->active_bitmap->stride = size;
		break;
	}

	/* Ensure 32bits alignment */
	gdip_align_stride (result->active_bitmap->stride);
 
	if (colours) {
		int palette_entries = colours;

		if (result->active_bitmap->pixel_format == PixelFormat4bppIndexed) {
			palette_entries = 256;
		}

		result->active_bitmap->palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * palette_entries);
		if (result->active_bitmap->palette == NULL) {
			status = OutOfMemory;
			goto error;
		}
		result->active_bitmap->palette->Flags = 0;
		result->active_bitmap->palette->Count = palette_entries;

		/* Read optional colour table */
		size = (os2format) ? 3 /* RGBTRIPLE */ : 4 /* RGBquads */;
		data_read = (BYTE*) GdipAlloc(size);
		if (data_read == NULL) {
			status = OutOfMemory;
			goto error;
		}
		for (i = 0; i < colours; i++) {
			size_read = gdip_read_bmp_data (pointer, data_read, size, source);
			if (size_read < size) {
				status = OutOfMemory;
				goto error;
			}

			set_pixel_bgra (result->active_bitmap->palette->Entries, i * 4,
				(data_read[0] & 0xFF),		/* B */
				(data_read[1] & 0xFF),		/* G */
				(data_read[2] & 0xFF),		/* R */
				0xFF);				/* Alpha */
		}
		GdipFree(data_read);
		data_read = NULL;
	}

	size = result->active_bitmap->stride;
	/* ensure total 'size' does not overflow an integer and fits inside our 2GB limit */
	size *= result->active_bitmap->height;
	if (size > G_MAXINT32) {
		status = OutOfMemory;
		goto error;
	}
	pixels = GdipAlloc (size);
	if (pixels == NULL) {
		status = OutOfMemory;
		goto error;
	}

	if (gdip_is_an_indexed_pixelformat (format) && ((bmi.bV5Compression == BI_RLE4) || (bmi.bV5Compression == BI_RLE8))) {
		switch (bmi.bV5Compression) {
			case BI_RLE4:
				gdip_read_bmp_rle_4bit (pointer, pixels, upsidedown, result->active_bitmap->stride, result->active_bitmap->width, result->active_bitmap->height, source);
				break;
			case BI_RLE8:
				gdip_read_bmp_rle_8bit (pointer, pixels, upsidedown, result->active_bitmap->stride, result->active_bitmap->width, result->active_bitmap->height, source);
				break;
		}
	} else {
		/* Size contains the size of the lines on disk */
		switch (bmi.bV5BitCount) {
			case 1: {
				size = ((result->active_bitmap->width + 31) & ~31) / 8;
				loop = result->active_bitmap->width / 8;
				/* we assume 0s for 1 bit, only setting 1s */
				memset(pixels, 0, result->active_bitmap->stride * result->active_bitmap->height);
				break;
			}

			case 4: {
				size = ((bmi.bV5BitCount * result->active_bitmap->width + 31) & ~31) / 8;
				loop = ((result->active_bitmap->width + 1) &~1) / 2;
				break;
			}

			case 8: {
				size = (((bmi.bV5BitCount * result->active_bitmap->width) + 31) & ~31) / 8;
				loop = result->active_bitmap->width;
				break;
			}

			default: {
				size = (((bmi.bV5BitCount * result->active_bitmap->width) + 31) & ~31) / 8;
				loop = (bmi.bV5BitCount * result->active_bitmap->width) / 8;
				break;
			}
		}

		data_read = (BYTE*) GdipAlloc(size);
		if (data_read == NULL) {
			status = OutOfMemory;
			goto error;
		}

		for (i = 0; i < result->active_bitmap->height; i++){ 
			if (upsidedown) {
				line = result->active_bitmap->height - i - 1;
			} else {
				line = i;
			}

			size_read = gdip_read_bmp_data (pointer, data_read, size, source);

			if (size_read < size) {
				/* special case for missing data in the last line */
				if (line == result->active_bitmap->height - 1) {
					/* MS produce such files for storing ImageList bitmaps, see bug #80797 */
					int missing_size = size - size_read;
					memset (data_read + size_read, 0, missing_size);
				} else {
					status = OutOfMemory;
					goto error;
				}
			}

			switch(bmi.bV5BitCount) {
				case 1:
				case 4:
				case 8:
					memcpy(pixels + line * result->active_bitmap->stride, data_read, size);
					continue;

				case 16: {
					int src = 0;
					int dest = 0;
					SHORT *pix = (SHORT*) data_read;

					index = (line * result->active_bitmap->stride);
					while (src < loop) {
						BYTE r = ((*pix & red_mask) >> (red_shift - 3));
						BYTE g = ((*pix & green_mask) >> (red_shift - 8));
						BYTE b = (*pix & blue_mask) << 3;
						set_pixel_bgra (pixels, index + dest, b, g, r, 0xff);
						dest += 4;
						src += 2;
						pix++;
					}
					continue;
				}

				case 24:
				case 32: {
					int src = 0;
					int dest = 0;
					int skip = (bmi.bV5BitCount >> 3);

					index = (line * result->active_bitmap->stride);
					while (src < loop) {
						set_pixel_bgra(pixels, index + dest, data_read[src+0], data_read[src+1], data_read[src+2], 0xff);
						dest += 4;
						src += skip;
					}
					continue;
				}
			}
		}

		GdipFree(data_read);
		data_read = NULL;
	}

	result->active_bitmap->scan0 = pixels;
	result->active_bitmap->reserved = GBD_OWN_SCAN0;
	result->active_bitmap->image_flags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | ImageFlagsColorSpaceRGB;
	if (bmi.bV5XPelsPerMeter != 0 && bmi.bV5YPelsPerMeter != 0)
		result->active_bitmap->image_flags |= ImageFlagsHasRealDPI;

	*image = result;
	return Ok;

error:
	if (data_read != NULL) {
		GdipFree(data_read);
	}

	if (pixels != NULL) {
		GdipFree(pixels);
	}

	if (result != NULL) {
		gdip_bitmap_dispose(result);
	}

	return status;
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
		return size;
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
	BitmapData		*activebmp;
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
