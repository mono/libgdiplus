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

#include <stdio.h>
#include "bmpcodec.h"
#include "gdip.h"
#include "gdipImage.h"
#include "dstream.h"


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
        bmp_codec.Flags = Encoder | Decoder | SupportBitmap | Builtin;
        bmp_codec.Version = 1;
        bmp_codec.SigCount = 1;
        bmp_codec.SigSize = 2;
        bmp_codec.SigPattern = bmp_sig_pattern;
        bmp_codec.SigMask = bmp_sig_mask;

        return &bmp_codec; 
}

int
gdip_get_pixelformat (WORD bitcount)
{
        switch (bitcount) {
        case 32:
                return Format32bppRgb;
        case 24:
                return Format24bppRgb;
        case 8:
                return Format8bppIndexed;
        case 4:
                return Format4bppIndexed;
	case 1:
		return Format1bppIndexed;
        default:
                return 0;
        }  
}

GpStatus
gdip_get_bmp_pixelformat (int bitCount, int compression, PixelFormat *dest)
{
	switch (compression)
	{
		case BI_RLE4:
		{
			if (bitCount != 4)
				return InvalidParameter;
			*dest = Format4bppIndexed;
			break;
		}
		case BI_RLE8:
		{
			if (bitCount != 8)
				return InvalidParameter;
			*dest = Format8bppIndexed;
			break;
		}
		default:
		{
			*dest = gdip_get_pixelformat (bitCount);
			if (0 == *dest) /* invalid pixel format */
				return InvalidParameter;
			break;
		}
	}

	return Ok;
}

void 
gdip_bitmap_fill_info_header (GpBitmap *bitmap, PBITMAPINFOHEADER bmi)
{
	PixelFormat format = bitmap->active_bitmap->pixel_format;

	memset (bmi, 0, sizeof (BITMAPINFOHEADER));
#ifdef WORDS_BIGENDIAN
	bmi->biSize = GUINT32_FROM_LE (sizeof (BITMAPINFOHEADER));
	bmi->biWidth = GULONG_FROM_LE (bitmap->active_bitmap->width);
	bmi->biHeight = GULONG_FROM_LE (bitmap->active_bitmap->height);
	bmi->biPlanes = GUINT16_FROM_LE (1);
	if (format != Format24bppRgb)
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
	if (format != Format24bppRgb)
		bmi->biBitCount = gdip_get_pixel_format_bpp (bitmap->active_bitmap->pixel_format);
	else
		bmi->biBitCount = 24;

	bmi->biCompression = BI_RGB;
	bmi->biSizeImage =  0; /* Many tools expect this may be set to zero for BI_RGB bitmaps */
	bmi->biXPelsPerMeter = (int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100));
	bmi->biYPelsPerMeter = (int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100)); /* 1 meter is = 39.37 */       
#endif
}                                                           

GpStatus 
gdip_load_bmp_image_from_file (FILE *fp, GpImage **image)
{
        return gdip_read_bmp_image_from_file_stream ( (void*)fp, image, TRUE);
}

GpStatus 
gdip_load_bmp_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return gdip_read_bmp_image_from_file_stream ((void *)loader, image, FALSE);
}

GpStatus
gdip_read_bmp_rle_8bit (void *pointer, byte *scan0, bool upsidedown, int stride, int scanWidth, int scanCount, bool useFile)
{
	byte code;
	int bytes_read;

	int col_offset = 0;
	int row_offset = (upsidedown ? (scanCount - 1) * stride : 0);
	int row_delta = (upsidedown ? -stride : +stride);
	int rows_remaining = scanCount;
	bool new_row = FALSE;

	if (scanWidth > stride)
		return InvalidParameter;

	while ((rows_remaining > 0)
	    || ((row_offset == 0) && (col_offset < scanWidth))) {
		bytes_read = gdip_read_bmp_data (pointer, &code, 1, useFile);

		if (bytes_read < 1)
			return GenericError; /* TODO?: Add an "unexpected end of file" error code */

		if (code == 0) { /* RLE escape code */
			bytes_read = gdip_read_bmp_data (pointer, &code, 1, useFile);

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
					byte dx, dy;

					bytes_read  = gdip_read_bmp_data (pointer, &dx, 1, useFile);
					bytes_read += gdip_read_bmp_data (pointer, &dy, 1, useFile);

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
					bool pad_byte_present = ((code & 1) != 0);
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
							useFile);

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
						bytes_read = gdip_read_bmp_data(pointer, &code, 1, useFile);

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
			byte pixel_value;

			bytes_read = gdip_read_bmp_data(pointer, &pixel_value, 1, useFile);

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

GpStatus
gdip_read_bmp_rle_4bit (void *pointer, byte *scan0, bool upsidedown, int stride, int scanWidth, int scanCount, bool useFile)
{
	byte code;
	int bytes_read;

	int col_offset = 0;
	int row_offset = (upsidedown ? (scanCount - 1) * stride : 0);
	int row_delta = (upsidedown ? -stride : +stride);
	int rows_remaining = scanCount;
	bool new_row = FALSE;

	if ((scanWidth & 1) != 0)
		scanWidth++;

	if (scanWidth > stride * 2)
		return InvalidParameter;

	while (rows_remaining > 0) {
		bytes_read = gdip_read_bmp_data (pointer, &code, 1, useFile);

		if (bytes_read < 1)
			return GenericError; /* TODO?: Add an "unexpected end of file" error code */

		if (code == 0) { /* RLE escape code */
			bytes_read = gdip_read_bmp_data (pointer, &code, 1, useFile);

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
					byte dx, dy;

					bytes_read  = gdip_read_bmp_data (pointer, &dx, 1, useFile);
					bytes_read += gdip_read_bmp_data (pointer, &dy, 1, useFile);

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
					bool pad_byte_present = ((bytes_of_data & 1) != 0);

					int bytes_to_read = pixels_to_read / 2; /* leave off the last pixel for now */

					/* wrap rows properly, even though they are inverted in memory */
					while (bytes_to_read > 0) {
						if ((scanWidth - col_offset) == 1) {
							/* special case: a pair of pixels is split across two rows. */
							byte pixels, same_row_pixel, next_row_pixel;

							bytes_read = gdip_read_bmp_data (pointer, &pixels, 1, useFile);

							if (bytes_read < 1)
								return GenericError; /* TODO?: Add an "unexpected end of file" error code */

							same_row_pixel = (pixels >> 4) & 0x0F;
							next_row_pixel =  pixels       & 0x0F;

							if ((col_offset & 1) != 0) {
								byte old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
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
								useFile);

							if (bytes_read < bytes_to_read_this_scan)
								return GenericError; /* TODO?: Add an "unexpected end of file" error code */

							col_offset += bytes_read * 2;
							bytes_to_read -= bytes_read;

							new_row = FALSE;
						}
						else {
							/* bad alignment; nybble-swapping will be required */
							byte last_high_nybble = 0xF0 & scan0[row_offset + col_offset / 2];

							int bytes_to_read_this_scan = (scanWidth - col_offset) / 2;

							if (bytes_to_read_this_scan > bytes_to_read)
								bytes_to_read_this_scan = bytes_to_read;

							while (bytes_to_read_this_scan >= 0) {
								byte pixels;

								bytes_read = gdip_read_bmp_data (pointer, &pixels, 1, useFile);

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
						byte pixel;

						bytes_read = gdip_read_bmp_data (pointer, &pixel, 1, useFile);

						if (bytes_read < 1)
							return GenericError; /* TODO?: Add an "unexpected end of file" error code */

						pixel >>= 4; /* the last pixel is in the high nybble */

						if ((col_offset & 1) != 0) {
							byte old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
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
						bytes_read = gdip_read_bmp_data(pointer, &code, 1, useFile);

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

			byte pixel_values;
			byte inverted_pixel_values;

			bytes_read = gdip_read_bmp_data(pointer, &pixel_values, 1, useFile);

			if (bytes_read < 1)
				return GenericError; /* TODO?: Add an "unexpected end of file" error code */

			inverted_pixel_values = ((pixel_values & 0x0F) << 4) | ((pixel_values & 0xF0) >> 4);

			if ((col_offset & 1) != 0) {
				byte temp = inverted_pixel_values;
				inverted_pixel_values = pixel_values;
				pixel_values = temp;
			}

			while (run_length > 0) {
				if ((scanWidth - col_offset) == 1) {
					/* special case: a pair of pixels is split across two rows. */
					byte same_row_pixel = (pixel_values >> 4) & 0x0F;
					byte next_row_pixel =  pixel_values       & 0x0F;

					if ((col_offset & 1) != 0) {
						byte old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
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
						byte temp = inverted_pixel_values;
						inverted_pixel_values = pixel_values;
						pixel_values = temp;
					}
				}
				else {
					int bytes_to_run_this_scan;

					/* make sure we're byte-aligned; if we're not, we need to store a nybble first */
					if ((col_offset & 1) != 0) {
						byte old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
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
							byte temp = inverted_pixel_values;
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
				byte pixel = pixel_values >> 4; /* the last pixel is in the high nybble */

				if ((col_offset & 1) != 0) {
					byte old_pixel = 0xF0 & scan0[row_offset + col_offset / 2];
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
gdip_read_BITMAPINFOHEADER (void *pointer, BITMAPINFOHEADER *bmi, bool useFile, BOOL *os2format, BOOL *upsidedown)
{
	DWORD dw = 0;
	byte *data_read = (byte*)&dw;
	int size = sizeof (DWORD);
	int size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;

	bmi->biSize = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
	if (bmi->biSize > BITMAPCOREHEADER_SIZE){   /* New Windows headers can be bigger */ 
		dw = 0;
		size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
		if (size_read < size)
			return InvalidParameter;
		bmi->biWidth = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

		dw = 0;
		size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
		if (size_read < size)
			return InvalidParameter;
		bmi->biHeight = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
 	} else if (bmi->biSize == BITMAPCOREHEADER_SIZE) {
		/* Old OS/2 format. Width and Height fields are WORDs instead of DWORDS */
		dw = 0;
		size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
		if (size_read < size)
			return InvalidParameter;
		bmi->biWidth = (data_read[1]<<8 | data_read[0]);
		bmi->biHeight = (data_read[3]<<8 | data_read[2]);
		*os2format = TRUE;
	} else {
		return UnknownImageFormat;
	}

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;
	bmi->biPlanes = (data_read[1]<<8 | data_read[0]); 
	bmi->biBitCount = (data_read[3]<<8 | data_read[2]); 

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;
	bmi->biCompression = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	if (bmi->biHeight < 0) { /* Negative height indicates that the bitmap is sideup*/
		upsidedown = FALSE;
		bmi->biHeight = -bmi->biHeight;
	}

	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;
	bmi->biSizeImage = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;
	bmi->biXPelsPerMeter = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
	
	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;
	bmi->biYPelsPerMeter = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;
	bmi->biClrUsed = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	dw = 0;
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size)
		return InvalidParameter;
	bmi->biClrImportant = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	return Ok;
}

GpStatus 
gdip_read_bmp_image_from_file_stream (void *pointer, GpImage **image, bool useFile)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmi;
	GpBitmap	*result = NULL;
	guchar		*pixels = NULL;
	int		i;
	PixelFormat	format;
	int		colours;
	BOOL		os2format = FALSE;
	BOOL		upsidedown = TRUE;
	int		size;
	int		size_read;
	byte		*data_read;
	int		line;
	int		loop;
	long		index;
	GpStatus	status;
		
	size = sizeof(bmfh);
	data_read = (byte*) GdipAlloc(size);
	if (data_read == NULL) {
		status = OutOfMemory;
		goto error;
	}
	memset (data_read, 0, size);
	size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
	if (size_read < size) {
		status = InvalidParameter;
		goto error;
	}
	
	bmfh.bfType = ((data_read[1]<<8)|data_read[0]);
	if (bmfh.bfType != BFT_BITMAP) {
		status = UnknownImageFormat;
		goto error;
	}

	bmfh.bfSize = (data_read[5]<<24 | data_read[4]<<16 | data_read[3]<<8 | data_read[2]);
	bmfh.bfReserved1 = ((data_read[7]<<8)|data_read[6]);
	bmfh.bfReserved1 = ((data_read[9]<<8)|data_read[8]);
	bmfh.bfOffBits = (data_read[13]<<24 | data_read[12]<<16 | data_read[11]<<8 | data_read[10]);
	GdipFree (data_read);
	data_read = NULL;

	status = gdip_read_BITMAPINFOHEADER (pointer, &bmi, useFile, &os2format, &upsidedown);
	if (status != Ok)
		goto error;

	colours = (bmi.biClrUsed == 0 && bmi.biBitCount <= 8) ? (1 << bmi.biBitCount) : bmi.biClrUsed;

	status = gdip_get_bmp_pixelformat (bmi.biBitCount, bmi.biCompression, &format);

	if (status != Ok) {
		 /* bit count mismatch */
		goto error;
	}
        
	result = gdip_bitmap_new_with_frame (NULL, TRUE);
	result->type = imageBitmap;
	result->image_format = BMP;
	result->active_bitmap->pixel_format = format;
	result->active_bitmap->width = bmi.biWidth;
	result->active_bitmap->height = bmi.biHeight;

	switch (result->active_bitmap->pixel_format) {
		case Format1bppIndexed: result->active_bitmap->stride = (result->active_bitmap->width + 7) / 8; break;
		case Format4bppIndexed: result->active_bitmap->stride = (result->active_bitmap->width + 1) / 2; break;
		case Format8bppIndexed: result->active_bitmap->stride =  result->active_bitmap->width;          break;
		case Format24bppRgb: result->active_bitmap->stride = result->active_bitmap->width * 4;		break;
		default:
			/* For other types, we assume 32 bit and translate into 32 bit from source format */
			result->active_bitmap->pixel_format = Format32bppRgb;
			result->active_bitmap->stride = result->active_bitmap->width * 4;
			break;
	}

	/* Ensure pixman_bits_t alignment */
	result->active_bitmap->stride += (sizeof(pixman_bits_t) - 1);
	result->active_bitmap->stride &= ~(sizeof(pixman_bits_t) - 1);
 
	if (colours) {
		int palette_entries = colours;

		if (result->active_bitmap->pixel_format == Format4bppIndexed) {
			palette_entries = 256;
		}

		result->active_bitmap->palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * palette_entries);
		if (result->active_bitmap->palette == NULL) {
			status = OutOfMemory;
			goto error;
		}
		result->active_bitmap->palette->Flags = 0;
		result->active_bitmap->palette->Count = palette_entries;

		/* Read optional colour table*/
		if (os2format) {
			size = sizeof(byte)*3;	/* RGBTRIPLE */
		} else {
			size = sizeof(byte)*4;	/* RGBquads */
		}
		data_read = (byte*) GdipAlloc(size);
		if (data_read == NULL) {
			status = OutOfMemory;
			goto error;
		}
		for (i = 0; i < colours; i++) {
			size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);
			if (size_read < size) {
				status = InvalidParameter;
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

	pixels = GdipAlloc (result->active_bitmap->stride * result->active_bitmap->height);
	if (pixels == NULL) {
		status = OutOfMemory;
		goto error;
	}

	if ((bmi.biCompression == BI_RLE4) || (bmi.biCompression == BI_RLE8)) {
		switch (bmi.biCompression) {
			case BI_RLE4:
				gdip_read_bmp_rle_4bit (pointer, pixels, upsidedown, result->active_bitmap->stride, result->active_bitmap->width, result->active_bitmap->height, useFile);
				break;
			case BI_RLE8:
				gdip_read_bmp_rle_8bit (pointer, pixels, upsidedown, result->active_bitmap->stride, result->active_bitmap->width, result->active_bitmap->height, useFile);
				break;
		}
	} else {
		/* Size contains the size of the lines on disk */
		switch (bmi.biBitCount) {
			case 1: {
				size = ((result->active_bitmap->width + 31) & ~31) / 8;
				loop = result->active_bitmap->width / 8;
				/* we assume 0s for 1 bit, only setting 1s */
				memset(pixels, 0, result->active_bitmap->stride * result->active_bitmap->height);
				break;
			}

			case 4: {
				size = ((bmi.biBitCount * result->active_bitmap->width + 31) & ~31) / 8;
				loop = ((result->active_bitmap->width + 1) &~1) / 2;
				break;
			}

			case 8: {
				size = (((bmi.biBitCount * result->active_bitmap->width) + 31) & ~31) / 8;
				loop = result->active_bitmap->width;
				break;
			}

			default: {
				size = (((bmi.biBitCount * result->active_bitmap->width) + 31) & ~31) / 8;
				loop = (bmi.biBitCount * result->active_bitmap->width) / 8; 
				break;
			}
		}

		data_read = (byte*) GdipAlloc(size);
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

			size_read = gdip_read_bmp_data (pointer, data_read, size, useFile);

			if (size_read < size) {
				status = InvalidParameter;
				goto error;
			}

			switch(bmi.biBitCount) {
				case 1:
				case 4:
				case 8: memcpy(pixels + line * result->active_bitmap->stride, data_read, size);
					continue;

				case 24: {
					int src = 0;
					int dest = 0;

					index = (line * result->active_bitmap->stride);
					while (src < loop) {
						set_pixel_bgra(pixels, index + dest, data_read[src+0], data_read[src+1], data_read[src+2], 0xff);
						dest += 4;
						src += 3;
					}
					continue;
				}

				case 32: {
#ifndef WORDS_BIGENDIAN
					memcpy (pixels + line * result->active_bitmap->stride, data_read, loop);
#else
					int	src;
					int	dest;

					src = 0;
					dest = 0;

					while (src < loop) {
						index = (line * result->active_bitmap->stride);
						set_pixel_bgra(pixels, index+dest, data_read[src+0], data_read[src+1], data_read[src+2], data_read[src+3]);
						dest += 4;
						src += 4;
					}
#endif
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

	return InvalidParameter;

}

int 
gdip_read_bmp_data (void *pointer, byte *data, int size, bool useFile)
{
	if (useFile) {
		return fread (data, 1, size, (FILE*) pointer);
	} else {
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
}

GpStatus 
gdip_save_bmp_image_to_file (FILE *fp, GpImage *image)
{
	return gdip_save_bmp_image_to_file_stream ( (void *)fp, image, TRUE);
}

GpStatus 
gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image)
{	
        return gdip_save_bmp_image_to_file_stream ( (void *)putBytesFunc, image, FALSE);
}

GpStatus 
gdip_save_bmp_image_to_file_stream (void *pointer, GpImage *image, bool useFile)
{
	BITMAPFILEHEADER	bmfh;
        BITMAPINFOHEADER	bmi;
        int			bitmapLen;
        int			i;
        ARGB			color;
        int			colours = 0;
	unsigned char		*entries;
	int			palette_entries;
	BitmapData		*activebmp;
	byte			*scan0;

	activebmp = image->active_bitmap;
	if (activebmp->pixel_format != Format24bppRgb) {
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

#ifdef WORDS_BIGENDIAN
	bmfh.bfReserved1 = bmfh.bfReserved2 = GUINT16_FROM_LE (0);
	bmfh.bfType = GUINT16_FROM_LE (BFT_BITMAP);
	bmfh.bfOffBits = GUINT32_FROM_LE (14 + 40 + colours * 4);
	bmfh.bfSize = GUINT32_FROM_LE (bmfh.bfOffBits + bitmapLen);
#else
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfType = BFT_BITMAP;
	bmfh.bfOffBits = (14 + 40 + colours * 4);
	bmfh.bfSize = (bmfh.bfOffBits + bitmapLen);
#endif
	gdip_write_bmp_data (pointer, (byte *)&bmfh, sizeof (bmfh), useFile);
        
	gdip_bitmap_fill_info_header (image, &bmi);
	gdip_write_bmp_data (pointer, (byte *)&bmi, sizeof (bmi), useFile);

	if (colours) {
		int idx;

		palette_entries = activebmp->palette->Count;

		if (activebmp->pixel_format == Format4bppIndexed) {
			palette_entries = 16;
		}

		entries = (unsigned char *) GdipAlloc (palette_entries*4);
		if (entries == NULL)
			return OutOfMemory;

		idx = 0;
		for (i = 0; i < palette_entries; i++) {
			color = activebmp->palette->Entries[i];
#ifdef WORDS_BIGENDIAN
			entries [idx] =  color & 0xff;
			entries [idx + 1] = (color >> 8) & 0xff;
			entries [idx + 2] = (color >> 16) & 0xff;
			entries [idx + 3] = color >> 24;
			idx += 4;
#else
			*((guint32 *) entries + i) = color;
#endif
		}
		gdip_write_bmp_data (pointer, entries, palette_entries * 4, useFile);
		GdipFree (entries);
	}

	scan0 = activebmp->scan0;
	if (activebmp->pixel_format == Format24bppRgb) {
		int width = activebmp->width;
		int height = activebmp->height;
		int mystride;
		int k;
		byte *current_line;

		/* rows need to be padded up to the next multiple of 4 */
		mystride = width * 3;
		mystride += 3;
		mystride &= ~3;
		current_line = (byte *) GdipAlloc (mystride);
		memset (current_line, 0, mystride); /* Zero padding at the end if needed */
		for (i = height - 1; i >= 0; i--) {
			byte *ptr;
			uint32_t *iptr;

			iptr = (uint32_t *) (scan0 + i * activebmp->stride);
			ptr = current_line;
			for (k = 0; k < width; k++) {
				uint32_t color = *iptr++;
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
		int	j;
		guchar	*row_pointer = GdipAlloc (activebmp->width * 4);

		if (row_pointer == NULL) {
			return OutOfMemory;
		}

		for (i = activebmp->height -1; i >= 0; i--) {
			for (j = 0; j < activebmp->width; j++) {
				row_pointer[j*4] = *((guchar *)scan0 + (activebmp->stride * i) + (j*4) + 3); 
				row_pointer[j*4+1] = *((guchar *)scan0 + (activebmp->stride * i) + (j*4) + 2); 
				row_pointer[j*4+2] = *((guchar *)scan0 + (activebmp->stride * i) + (j*4) + 1); 
				row_pointer[j*4+3] = *((guchar *)scan0 + (activebmp->stride * i) + (j*4) + 0); 
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

void 
gdip_write_bmp_data (void *pointer, byte *data, int size, bool useFile)
{
	if (useFile)
		fwrite (data, 1, size, (FILE*) pointer);
	else 	
		((PutBytesDelegate)(pointer))(data, size);	
}


