/*
 * pngcodec.c : Contains function definitions for encoding decoding png images
 *
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
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic <vladimir@pobox.com>
 *      Jonathan Gilbert (logic@deltaq.org)
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "win32structs.h"

GUID gdip_png_image_format_guid = {0xb96b3cafU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

#ifdef HAVE_LIBPNG

#include <png.h>
#include "gdiplus-private.h"
#include "pngcodec.h"
#include <setjmp.h>

/* Codecinfo related data*/
static ImageCodecInfo png_codec;
static const WCHAR png_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'P', 'N', 'G', 0}; /* Built-in PNG */
static const WCHAR png_extension[] = {'*', '.', 'P', 'N', 'G', 0}; /* *.PNG */
static const WCHAR png_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'p', 'n', 'g', 0}; /* image/png */
static const WCHAR png_format[] = {'P', 'N', 'G', 0}; /* PNG */
static const BYTE png_sig_pattern[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
static const BYTE png_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


ImageCodecInfo *
gdip_getcodecinfo_png ()
{
        png_codec.Clsid = (CLSID) { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
        png_codec.FormatID = gdip_png_image_format_guid;
        png_codec.CodecName = (const WCHAR*) png_codecname;
        png_codec.DllName = NULL;
        png_codec.FormatDescription = (const WCHAR*) png_format;
        png_codec.FilenameExtension = (const WCHAR*) png_extension;
        png_codec.MimeType = (const WCHAR*) png_mimetype;
        png_codec.Flags = ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin;
        png_codec.Version = 1;
        png_codec.SigCount = 1;
        png_codec.SigSize = 8;
        png_codec.SigPattern = png_sig_pattern;
        png_codec.SigMask = png_sig_mask;

        return &png_codec;
}

#if !defined(HAVE_SIGSETJMP) && !defined(sigsetjmp)
#define sigjmp_buf jmp_buf
#define sigsetjmp(jb, x) setjmp(jb)
#define siglongjmp longjmp
#endif

static void
_gdip_png_stream_read_data (png_structp png_ptr, png_bytep data, png_size_t length)
{
	GetBytesDelegate	getBytesFunc;
	int			bytesRead;
	int			res;

	getBytesFunc = (GetBytesDelegate) png_get_io_ptr (png_ptr);

	/* In png parlance, it is an error to read less than length */
	bytesRead = 0;
	while (bytesRead != length) {
		res = getBytesFunc (data + bytesRead, length - bytesRead, 0);
		if (res <= 0) {
			png_error(png_ptr, "Read failed");
		}
		bytesRead += res;
	}
}

static void
_gdip_png_stream_write_data (png_structp png_ptr, png_bytep data, png_size_t length)
{
    PutBytesDelegate putBytesFunc = (PutBytesDelegate) png_get_io_ptr (png_ptr);
    putBytesFunc (data, length);
}

static void
_gdip_png_stream_flush_data (png_structp png_ptr)
{
    /* nothing */
}

static GpStatus
gdip_load_png_properties (png_structp png_ptr, png_infop info_ptr, png_infop end_ptr, BitmapData *bitmap_data)
{
#if defined(PNG_INCH_CONVERSIONS) && defined(PNG_FLOATING_POINT_SUPPORTED)
	bitmap_data->image_flags |= ImageFlagsHasRealDPI;
	bitmap_data->dpi_horz = png_get_x_pixels_per_inch(png_ptr, info_ptr);
	bitmap_data->dpi_vert = png_get_y_pixels_per_inch(png_ptr, info_ptr);
#elif defined(PNG_pHYs_SUPPORTED)
	{
		int		unit_type;
		png_uint_32	res_x;
		png_uint_32	res_y;
		png_get_pHYs( png_ptr, info_ptr, &res_x, &res_y, &unit_type );
		if (unit_type == PNG_RESOLUTION_METER) {
			bitmap_data->image_flags |= ImageFlagsHasRealDPI;
			bitmap_data->dpi_horz = res_x * 0.0254;
			bitmap_data->dpi_vert = res_y * 0.0254;
		}
	}
#endif
	/* default to screen resolution (if nothing was provided or available) */
	if (bitmap_data->dpi_horz == 0 || bitmap_data->dpi_vert == 0) {
		 bitmap_data->dpi_horz = bitmap_data->dpi_vert = gdip_get_display_dpi ();
	}

#if defined(PNG_iCCP_SUPPORTED)
	{
		png_charp	name;
		png_bytepp	profile;
		png_uint_32	proflen;
		int		compression_type;

		if (png_get_iCCP(png_ptr, info_ptr, &name, &compression_type, profile, &proflen)) {
			gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagICCProfileDescriptor, (BYTE*)name);
			gdip_bitmapdata_property_add_byte(bitmap_data, PropertyTagICCProfile, (BYTE)compression_type);
		}
	}
#endif

#if defined(PNG_gAMA_SUPPORTED)
	{
		double	gamma;

		if (png_get_gAMA(png_ptr, info_ptr, &gamma)) {
			gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagGamma, 100000, gamma * 100000);
		}
	}
#endif

#if defined(PNG_cHRM_SUPPORTED)
	{
		double	white_x;
		double	white_y;
		double	red_x;
		double	red_y;
		double	green_x;
		double	green_y;
		double	blue_x;
		double	blue_y;

		if (png_get_cHRM(png_ptr, info_ptr, &white_x, &white_y, &red_x, &red_y, &green_x, &green_y, &blue_x, &blue_y)) {
			BYTE *buffer;
			guint32		*ptr;

			buffer = GdipAlloc(6 * (sizeof(png_uint_32) + sizeof(png_uint_32)));
			if (buffer != NULL)  {
				ptr = (guint32 *)buffer;

				ptr[0] = (guint32)(red_x * 100000);
				ptr[1] = 1000000;
				ptr[2] = (guint32)(red_y * 100000);
				ptr[3] = 100000;

				ptr[4] = (guint32)(green_x * 100000);
				ptr[5] = 1000000;
				ptr[6] = (guint32)(green_y * 100000);
				ptr[7] = 100000;

				ptr[8] = (guint32)(blue_x * 100000);
				ptr[9] = 100000;
				ptr[10] = (guint32)(blue_y * 100000);
				ptr[11] = 100000;

				gdip_bitmapdata_property_add (bitmap_data, PropertyTagPrimaryChromaticities, 
					6 * (sizeof(guint32) + sizeof(guint32)), PropertyTagTypeRational, buffer);

				ptr[0] = (guint32)(white_x * 100000);
				ptr[1] = 1000000;
				ptr[2] = (guint32)(white_y * 100000);
				ptr[3] = 100000;
				gdip_bitmapdata_property_add (bitmap_data, PropertyTagWhitePoint, 
					2 * (sizeof(guint32) + sizeof(guint32)), PropertyTagTypeRational, buffer);

				GdipFree(buffer);
			}
		}
	}
#endif

#if defined(PNG_pHYs_SUPPORTED)
	{
		int		unit_type;
		png_uint_32	res_x;
		png_uint_32	res_y;

		if (png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type)) {
			gdip_bitmapdata_property_add_byte(bitmap_data, PropertyTagPixelUnit, (BYTE)unit_type);
			gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagPixelPerUnitX, res_x);
			gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagPixelPerUnitY, res_y);
		}
	}
#endif

#if defined(PNG_TEXT_SUPPORTED)
	{
		int		num_text;
		png_textp	text_ptr;

		if (png_get_text(png_ptr, info_ptr, &text_ptr, &num_text)) {
			if (num_text > 0) {
				gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagExifUserComment, (BYTE*)text_ptr[0].text);
			}
		}
	}
#endif

	return Ok;
}

static GpStatus 
gdip_load_png_image_from_file_or_stream (FILE *fp, GetBytesDelegate getBytesFunc, GpImage **image)
{
	png_structp	png_ptr = NULL;
	png_infop	info_ptr = NULL;
	png_infop	end_info_ptr = NULL;
	BYTE		*rawdata = NULL;
	GpImage		*result = NULL;
	GpStatus	status = InvalidParameter;
	int		bit_depth;
	int		channels;
	BYTE		color_type;
	int 	num_palette;
	png_colorp	png_palette;

	png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr) {
		goto error;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		/* png detected error occured */
		goto error;
	}

	info_ptr = png_create_info_struct (png_ptr);
	if (info_ptr == NULL) {
		goto error;
	}

	end_info_ptr = png_create_info_struct (png_ptr);
	if (end_info_ptr == NULL) {
		goto error;
	}

	if (fp != NULL) {
		png_init_io (png_ptr, fp);
	} else {
		png_set_read_fn (png_ptr, getBytesFunc, _gdip_png_stream_read_data);
	}

	png_read_png (png_ptr, info_ptr, 0, NULL);

	bit_depth = png_get_bit_depth (png_ptr, info_ptr);
	channels = png_get_channels (png_ptr, info_ptr);
	color_type = png_get_color_type (png_ptr, info_ptr);

	/* 2bpp is a special case (promoted to 32bpp ARGB by MS GDI+) */
	if ((bit_depth <= 8) && (bit_depth != 2) && (channels == 1) && 
		((color_type == PNG_COLOR_TYPE_PALETTE)	|| (color_type == PNG_COLOR_TYPE_GRAY))) {
		int		width;
		int		height;
		int		source_stride;
		int		dest_stride;
		png_bytep	*row_pointers;
		BYTE		*rawptr;
		int		num_colours;
		int		palette_entries;
		ColorPalette	*palette;
		ImageFlags	colourspace_flag;
		int		i;
		int		j;

		width = png_get_image_width (png_ptr, info_ptr);
		height = png_get_image_height (png_ptr, info_ptr);

		source_stride = (width * bit_depth + 7) / 8;
		dest_stride = source_stride;
		gdip_align_stride (dest_stride);

		/* Copy image data. */
		row_pointers = png_get_rows (png_ptr, info_ptr);

		rawdata = GdipAlloc(dest_stride * height);
		for (i=0; i < height; i++) {
			memcpy (rawdata + i * dest_stride, row_pointers[i], source_stride);
		}

		/* Copy palette. */
		num_colours = 1 << bit_depth;

		if (png_get_color_type (png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY) {
			/* A gray-scale image; generate a palette fading from black to white. */
			colourspace_flag = ImageFlagsColorSpaceGRAY;
			palette = gdip_create_greyscale_palette (num_colours);
			palette->Flags = PaletteFlagsGrayScale;
		} else {
			/* Copy the palette data into the GDI+ structure. */
			colourspace_flag = ImageFlagsColorSpaceRGB;

			png_get_PLTE( png_ptr, info_ptr, &png_palette, &num_palette );
			palette_entries = num_colours;
			if (palette_entries > num_palette) {
				palette_entries = num_palette;
			}

			palette = GdipAlloc (sizeof(ColorPalette) + (num_colours - 1) * sizeof(ARGB));
			palette->Flags = 0;
			palette->Count = num_colours;

			for (i=0; i < palette_entries; i++) {
				set_pixel_bgra (&palette->Entries[i], 0,
						png_palette[i].blue,
						png_palette[i].green,
						png_palette[i].red,
						0xFF); /* alpha */
			}
		}

		{
			png_bytep trans_alpha;
			int num_trans;
			png_color_16p trans_color;

			/* Make sure transparency is respected. */
			png_get_tRNS( png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color );
			if (num_trans > 0) {
				palette->Flags |= PaletteFlagsHasAlpha;
				colourspace_flag |= ImageFlagsHasAlpha;

				if (num_trans > num_palette) {
					num_trans = num_palette;
				}

				for (i=0; i < num_trans; i++) {
					set_pixel_bgra(&palette->Entries[i], 0,
							png_palette[i].blue,
							png_palette[i].green,
							png_palette[i].red,
#if PNG_LIBPNG_VER > 10399
							trans_alpha [i]); /* alpha */
#else
							trans[i]); /* alpha */
#endif
				}
			}
		}

		result = gdip_bitmap_new_with_frame (&gdip_image_frameDimension_page_guid, TRUE);
		result->type = ImageTypeBitmap;
		result->active_bitmap->stride = dest_stride;
		result->active_bitmap->width = width;
		result->active_bitmap->height = height;
		result->active_bitmap->scan0 = rawdata;
		result->active_bitmap->reserved = GBD_OWN_SCAN0;

		switch (bit_depth) {
		case 1:
			result->active_bitmap->pixel_format = PixelFormat1bppIndexed;
			result->cairo_format = CAIRO_FORMAT_A1;
			break;
		// note: 2bpp is a special case as the format is "promoted" to PixelFormat32bppARGB / CAIRO_FORMAT_ARGB32
		//       we deal with this later...
		case 4:
			result->active_bitmap->pixel_format = PixelFormat4bppIndexed;
			result->cairo_format = CAIRO_FORMAT_A8;
			break;
		case 8:
			result->active_bitmap->pixel_format = PixelFormat8bppIndexed;
			result->cairo_format = CAIRO_FORMAT_A8;
			break;
		}

		result->active_bitmap->image_flags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | colourspace_flag; /* assigned when the palette is loaded */
		result->active_bitmap->dpi_horz = 0;
		result->active_bitmap->dpi_vert = 0;
		result->active_bitmap->palette = palette;
	}

	/* 2bpp needs to enter here too */
	if (!result) {
		int		width;
		int		height;
		BYTE		bit_depth;
		int		stride;
		int		interlace;
		png_bytep *row_pointers;
		BYTE *rawptr;
		int i, j;

		width = png_get_image_width (png_ptr, info_ptr);
		height = png_get_image_height (png_ptr, info_ptr);

		/* 24 and 32bpp are supported, 48, 64bpp aren't, see http://bugzilla.ximian.com/show_bug.cgi?id=80693 */
		bit_depth = png_get_bit_depth (png_ptr, info_ptr);
		if (bit_depth > 8) {
			g_warning ("PNG images with %dbpp aren't supported by libgdiplus.", channels * bit_depth);
			status = UnknownImageFormat;
			goto error;
		}

		interlace = png_get_interlace_type (png_ptr, info_ptr);

		/* According to the libpng manual, this sequence is equivalent to
		* using the PNG_TRANSFORM_EXPAND flag in png_read_png. */
		if (color_type == PNG_COLOR_TYPE_PALETTE) {
			png_set_palette_to_rgb (png_ptr);
		}

		if ((color_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8)) {
#if PNG_LIBPNG_VER > 10399
			png_set_expand_gray_1_2_4_to_8 (png_ptr);
#else
			png_set_gray_1_2_4_to_8(png_ptr);
#endif
		}

		if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
			png_set_tRNS_to_alpha(png_ptr);
		}

		stride = (width * 4);
		gdip_align_stride (stride);

		row_pointers = png_get_rows (png_ptr, info_ptr);

		rawdata = GdipAlloc (stride * height);
		rawptr = rawdata;

		switch (channels) {
			case 4: {
				for (i = 0; i < height; i++) {
					png_bytep rowp = row_pointers[i];
					for (j = 0; j < width; j++) {
						BYTE a = rowp[3];
						if (a == 0) {
							set_pixel_bgra (rawptr, 0, 0, 0, 0, 0);
						} else {
							BYTE b = rowp[2];
							BYTE g = rowp[1];
							BYTE r = rowp[0];

							set_pixel_bgra (rawptr, 0, b, g, r, a);
						}
						rowp += 4;
						rawptr += 4;
					}
				}
				break;
			}

			case 3: {
				for (i = 0; i < height; i++) {
					png_bytep rowp = row_pointers[i];
					for (j = 0; j < width; j++) {
						set_pixel_bgra (rawptr, 0, rowp[2], rowp[1], rowp[0], 0xff);
						rowp += 3;
						rawptr += 4;
					}
				}
				break;
			}

			case 1:
				for (i = 0; i < height; i++) {
					png_bytep rowp = row_pointers[i];
					if (bit_depth == 2) {
						// 4 pixels for each byte
						for (j = 0; j < (width >> bit_depth); j++) {
							png_byte palette = 0;
							png_byte pix = *rowp++;

							palette = (pix >> 6) & 0x03;
							set_pixel_bgra (rawptr, 0,
								png_palette[palette].blue,
								png_palette[palette].green,
								png_palette[palette].red,
								0xFF); /* alpha */

							palette = (pix >> 4) & 0x03;
							set_pixel_bgra (rawptr, 4,
								png_palette[palette].blue,
								png_palette[palette].green,
								png_palette[palette].red,
								0xFF); /* alpha */

							palette = (pix >> 2) & 0x03;
							set_pixel_bgra (rawptr, 8,
								png_palette[palette].blue,
								png_palette[palette].green,
								png_palette[palette].red,
								0xFF); /* alpha */

							palette = pix & 0x03;
							set_pixel_bgra (rawptr, 12,
								png_palette[palette].blue,
								png_palette[palette].green,
								png_palette[palette].red,
								0xFF); /* alpha */
							rawptr += 16;
						}
					} else {
						for (j = 0; j < width; j++) {
							png_byte pix = *rowp++;
							set_pixel_bgra (rawptr, 0, pix, pix, pix, 0xff);
							rawptr += 4;
						}
					}
				}
				break;
		}

		result = gdip_bitmap_new_with_frame (&gdip_image_frameDimension_page_guid, TRUE);
		result->type = ImageTypeBitmap;

		result->cairo_format = CAIRO_FORMAT_ARGB32;
		result->active_bitmap->stride = stride;
		result->active_bitmap->pixel_format = PixelFormat32bppARGB;
		result->active_bitmap->width = width;
		result->active_bitmap->height = height;
		result->active_bitmap->scan0 = rawdata;
		result->active_bitmap->reserved = GBD_OWN_SCAN0;

		result->surface = cairo_image_surface_create_for_data ((BYTE*)rawdata,
			result->cairo_format,
			result->active_bitmap->width,
			result->active_bitmap->height,
			result->active_bitmap->stride);

		if (channels == 3) {
			result->active_bitmap->pixel_format = PixelFormat24bppRGB;
			result->active_bitmap->image_flags = ImageFlagsColorSpaceRGB;
		} else if (channels == 4) {
			result->active_bitmap->pixel_format = PixelFormat32bppARGB;
			result->active_bitmap->image_flags = ImageFlagsColorSpaceRGB;
		} else if ((channels == 1) && (color_type == PNG_COLOR_TYPE_GRAY)) {
			// doesn't apply to 2bpp images
			result->active_bitmap->pixel_format = PixelFormat8bppIndexed;
			result->active_bitmap->image_flags = ImageFlagsColorSpaceGRAY;
		}

		if (color_type & PNG_COLOR_MASK_ALPHA)
			 result->active_bitmap->image_flags |= ImageFlagsHasAlpha;

		result->active_bitmap->image_flags |= ImageFlagsReadOnly | ImageFlagsHasRealPixelSize;
		result->active_bitmap->dpi_horz = 0;
		result->active_bitmap->dpi_vert = 0;
	}

	gdip_load_png_properties(png_ptr, info_ptr, end_info_ptr, result->active_bitmap);
	png_destroy_read_struct (&png_ptr, &info_ptr, &end_info_ptr);

	*image = result;

	return Ok;

error:
	/* coverity[dead_error_line] */
	if (rawdata) {
		GdipFree (rawdata);
	}

	if (png_ptr) {
		png_destroy_read_struct (&png_ptr, info_ptr ? &info_ptr : (png_infopp) NULL, end_info_ptr ? &end_info_ptr : (png_infopp) NULL);
	}

	*image = NULL;
	return status;
}


GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_load_png_image_from_file_or_stream (fp, NULL, image);
}

GpStatus
gdip_load_png_image_from_stream_delegate (GetBytesDelegate getBytesFunc, SeekDelegate seeknFunc, GpImage **image)
{
	return gdip_load_png_image_from_file_or_stream (NULL, getBytesFunc, image);
}

static GpStatus 
gdip_save_png_image_to_file_or_stream (FILE *fp, PutBytesDelegate putBytesFunc, GpImage *image, GDIPCONST EncoderParameters *params)
{
	png_structp	png_ptr = NULL;
	png_infop	info_ptr = NULL;
	int		i;
	int		bit_depth;
	int		color_type;

	png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		goto error;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		/* png detected error occured */
		goto error;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		goto error;
	}

	if (fp != NULL) {
		png_init_io (png_ptr, fp);
	} else {
		png_set_write_fn (png_ptr, putBytesFunc, _gdip_png_stream_write_data, _gdip_png_stream_flush_data);
	}

	switch (image->active_bitmap->pixel_format) {
		case PixelFormat32bppARGB:
		case PixelFormat32bppPARGB:
		case PixelFormat32bppRGB:
			color_type = PNG_COLOR_TYPE_RGB_ALPHA;
			bit_depth = 8;
			break;

		case PixelFormat24bppRGB:
			color_type = PNG_COLOR_TYPE_RGB; /* FIXME - we should be able to write grayscale PNGs */
			bit_depth = 8;
			break;

		case PixelFormat8bppIndexed:
			color_type = PNG_COLOR_TYPE_PALETTE;
			bit_depth = 8;
			break;

		case PixelFormat4bppIndexed:
			color_type = PNG_COLOR_TYPE_PALETTE;
			bit_depth = 4;
			break;

		case PixelFormat1bppIndexed:
			color_type = PNG_COLOR_TYPE_PALETTE;
			bit_depth = 1;
			break;

		/* We're not going to even try to save these images, for now */
		case PixelFormat64bppARGB:
		case PixelFormat64bppPARGB:
		case PixelFormat48bppRGB:
		case PixelFormat16bppARGB1555:
		case PixelFormat16bppGrayScale:
		case PixelFormat16bppRGB555:
		case PixelFormat16bppRGB565:
		default:
			color_type = -1;
			bit_depth = -1;
			break;
	}

	if (bit_depth == -1) {
		goto error;
	}

	png_set_IHDR (png_ptr, info_ptr, image->active_bitmap->width, image->active_bitmap->height, bit_depth, color_type,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
		png_color palette[256];

		int palette_entries = image->active_bitmap->palette->Count;
		if (image->active_bitmap->pixel_format == PixelFormat4bppIndexed) {
			palette_entries = 16;
		}

		for (i=0; i < palette_entries; i++) {
			ARGB entry = image->active_bitmap->palette->Entries[i];

			int dummy;

			get_pixel_bgra(entry, palette[i].blue, palette[i].green, palette[i].red, dummy);
		}

		png_set_PLTE (png_ptr, info_ptr, palette, palette_entries);
	}

	png_set_filter (png_ptr, 0, PNG_NO_FILTERS);
	png_set_sRGB_gAMA_and_cHRM (png_ptr, info_ptr, PNG_sRGB_INTENT_PERCEPTUAL);
	png_write_info (png_ptr, info_ptr);

#if 0
	if ((image->active_bitmap->pixel_format == Format24bppRGB) || (image->active_bitmap->pixel_format == Format32bppRGB)) {
		png_set_filler (png_ptr, 0, PNG_FILLER_AFTER);
	} else if (image->active_bitmap->pixel_format == Format8bppIndexed) {
		png_set_filler (png_ptr, 0, PNG_FILLER_AFTER);
	}
#endif

	png_set_bgr(png_ptr);

	if (gdip_is_an_indexed_pixelformat (image->active_bitmap->pixel_format)) {
		for (i = 0; i < image->active_bitmap->height; i++) {
			png_write_row (png_ptr, image->active_bitmap->scan0 + i * image->active_bitmap->stride);
		}
	} else if (image->active_bitmap->pixel_format == PixelFormat24bppRGB) {
		int j;
		BYTE *row_pointer = GdipAlloc (image->active_bitmap->width * 3);
		for (i = 0; i < image->active_bitmap->height; i++) {
			for (j = 0; j < image->active_bitmap->width; j++) {
#ifdef WORDS_BIGENDIAN
				row_pointer[j*3  ] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 3);
				row_pointer[j*3+1] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 2);
				row_pointer[j*3+2] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 1);
#else
				row_pointer[j*3  ] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 0);
				row_pointer[j*3+1] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 1);
				row_pointer[j*3+2] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 2);
#endif /* WORDS_BIGENDIAN */
			}
			png_write_row (png_ptr, row_pointer);
		}
		GdipFree (row_pointer);
	} else {
#ifdef WORDS_BIGENDIAN
		int j;
		BYTE *row_pointer = GdipAlloc (image->active_bitmap->width * 4);

		for (i = 0; i < image->active_bitmap->height; i++) {
			for (j = 0; j < image->active_bitmap->width; j++) {
				row_pointer[j*4]   = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 3);
				row_pointer[j*4+1] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 2);
				row_pointer[j*4+2] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 1);
				row_pointer[j*4+3] = *((BYTE*)image->active_bitmap->scan0 + (image->active_bitmap->stride * i) + (j*4) + 0);
			}
			png_write_row (png_ptr, row_pointer);
		}
		GdipFree (row_pointer);
#else
		for (i = 0; i < image->active_bitmap->height; i++) {
			png_write_row (png_ptr, image->active_bitmap->scan0 + (image->active_bitmap->stride * i));
		}
#endif
	}

	png_write_end (png_ptr, NULL);

	png_destroy_write_struct (&png_ptr, &info_ptr);

	return Ok;

error:
	if (png_ptr) {
		png_destroy_write_struct (&png_ptr, info_ptr ? &info_ptr : NULL);
	}
	return GenericError;
}

GpStatus 
gdip_save_png_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return gdip_save_png_image_to_file_or_stream (fp, NULL, image, params);
}

GpStatus
gdip_save_png_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return gdip_save_png_image_to_file_or_stream (NULL, putBytesFunc, image, params);
}

#else

#include "pngcodec.h"

GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

GpStatus
gdip_load_png_image_from_stream_delegate (GetBytesDelegate getBytesFunc, SeekDelegate seeknFunc, GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}


GpStatus 
gdip_save_png_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return UnknownImageFormat;
}


GpStatus
gdip_save_png_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image, GDIPCONST EncoderParameters *params)
{

	return UnknownImageFormat;
}


ImageCodecInfo *
gdip_getcodecinfo_png ()
{
	return NULL;
}
#endif	/* HAVE_LIBPNG */

