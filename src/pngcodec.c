/*
 * pngcodec.c : Contains function definitions for encoding decoding png images
 *
 * Copyright (C) Novell, Inc. 2003-2004.
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

#ifdef HAVE_LIBPNG

#include <png.h>
#include "pngcodec.h"


#include <setjmp.h>


/* Codecinfo related data*/
static ImageCodecInfo png_codec;
static const WCHAR png_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'P', 'N', 'G', 0}; /* Built-in PNG */
static const WCHAR png_extension[] = {'*', '.', 'P', 'N', 'G', 0}; /* *.PNG */
static const WCHAR png_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'p', 'n', 'g', 0}; /* image/png */
static const WCHAR png_format[] = {'P', 'N', 'G', 0}; /* PNG */


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
        png_codec.Flags = Encoder | Decoder | SupportBitmap | Builtin;
        png_codec.Version = 1;
        png_codec.SigCount = 0;
        png_codec.SigSize = 0;
        png_codec.SigPattern = 0;
        png_codec.SigMask = 0;

        return &png_codec;
}

#if !defined(HAVE_SIGSETJMP) && !defined(sigsetjmp)
#define sigjmp_buf jmp_buf
#define sigsetjmp(jb, x) setjmp(jb)
#define siglongjmp longjmp
#endif

struct gdip_stream_png_source {
    GetBytesDelegate getBytesFunc;
    PutBytesDelegate putBytesFunc;
};
typedef struct gdip_stream_png_source *gdip_stream_png_source_ptr;

void
_gdip_png_stream_read_data (png_structp png_ptr, png_bytep data, png_size_t length)
{
    gdip_stream_png_source_ptr src = (gdip_stream_png_source_ptr) png_get_io_ptr (png_ptr);
    int bytesRead = 0;

    /* In png parlance, it is an error to read less than length */
    while (bytesRead != length) {
        int res;
        res = src->getBytesFunc (data + bytesRead, length - bytesRead, 0);
        if (res < 0)
            png_error(png_ptr, "Read failed");
        bytesRead += res;
    }
}

void
_gdip_png_stream_write_data (png_structp png_ptr, png_bytep data, png_size_t length)
{
    gdip_stream_png_source_ptr src = (gdip_stream_png_source_ptr) png_get_io_ptr (png_ptr);
    src->putBytesFunc (data, length);
}

void
_gdip_png_stream_flush_data (png_structp png_ptr)
{
    /* nothing */
}

GpStatus 
gdip_load_png_image_from_file_or_stream (FILE *fp,
                                         GetBytesDelegate getBytesFunc,
                                         GpImage **image)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_infop end_info_ptr = NULL;
    guchar *rawdata = NULL;
    GpBitmap *img = NULL;
    gdip_stream_png_source_ptr pngsrc = NULL;

    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING,
                                      NULL, NULL, NULL);

    if (!png_ptr)
        goto error;

    if (setjmp(png_jmpbuf(png_ptr))) {
        /* png detected error occured */
        goto error;
    }

    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr)
        goto error;

    end_info_ptr = png_create_info_struct (png_ptr);
    if (!end_info_ptr)
        goto error;

    if (fp != NULL) {
        png_init_io (png_ptr, fp);
    } else {
        pngsrc = GdipAlloc(sizeof(struct gdip_stream_png_source));
        pngsrc->getBytesFunc = getBytesFunc;
        pngsrc->putBytesFunc = NULL;

        png_set_read_fn (png_ptr, pngsrc, _gdip_png_stream_read_data);
    }

    png_read_png(png_ptr, info_ptr, 0, NULL);

    if ((png_get_bit_depth (png_ptr, info_ptr) <= 8)
     && (png_get_channels (png_ptr, info_ptr) == 1)
     && ((png_get_color_type (png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE)
      || (png_get_color_type (png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY))) {
	int width;
	int height;
	int bit_depth;
	int source_stride, dest_stride;
	png_bytep *row_pointers;
	guchar *rawptr;
	int num_colours;
	int palette_entries;
	ColorPalette *palette;
	ImageFlags colourspace_flag;
	int i, j;

	width = png_get_image_width (png_ptr, info_ptr);
	height = png_get_image_height (png_ptr, info_ptr);
	bit_depth = png_get_bit_depth (png_ptr, info_ptr);

	source_stride = (width * bit_depth + 7) / 8;
	dest_stride = (source_stride + sizeof(pixman_bits_t) - 1) & ~(sizeof(pixman_bits_t) - 1);

	/* Copy image data. */
	row_pointers = png_get_rows (png_ptr, info_ptr);

	if (bit_depth == 2) { /* upsample to 4bpp */
		dest_stride = ((width + 1) / 2 + sizeof(pixman_bits_t) - 1) & ~(sizeof(pixman_bits_t) - 1);

		rawdata = GdipAlloc(dest_stride * height);
		for (i=0; i < height; i++) {
			png_bytep row = row_pointers[i];
			rawptr = rawdata + i * dest_stride;

			for (j=0; j < source_stride; j++) {
				int four_pixels = row[j];

				int first_two = 0x0F & (four_pixels >> 4);
				int second_two = 0x0F & four_pixels;

				first_two = (first_two & 0x03) | ((first_two & 0x0C) << 2);
				second_two = (second_two & 0x03) | ((second_two & 0x0C) << 2);

				rawptr[j * 2 + 0] = first_two;
				rawptr[j * 2 + 1] = second_two;
			}
		}
	}
	else {
		rawdata = GdipAlloc(dest_stride * height);
		for (i=0; i < height; i++)
			memcpy(rawdata + i * dest_stride, row_pointers[i], source_stride);
	}

	/* Copy palette. */
	num_colours = 1 << bit_depth;
	if (bit_depth == 4)
		num_colours = 256;

	palette = GdipAlloc (sizeof(ColorPalette) + num_colours * sizeof(ARGB));

	palette->Flags = 0;
	palette->Count = num_colours;

        if (png_get_color_type (png_ptr, info_ptr) == PNG_COLOR_TYPE_GRAY) {
		/* A gray-scale image; generate a palette fading from black to white. */
        	colourspace_flag = ImageFlagsColorSpaceGRAY;
		palette->Flags = PaletteFlagsGrayScale;

		for (i=0; i < num_colours; i++) {
			int intensity = i * 255 / (num_colours - 1);

			set_pixel_bgra (&palette->Entries[i], 0,
				intensity,
				intensity,
				intensity,
				0xFF); /* alpha */
		}
	}
	else {
		/* Copy the palette data into the GDI+ structure. */
		colourspace_flag = ImageFlagsColorSpaceRGB;

		palette_entries = num_colours;
		if (palette_entries > info_ptr->num_palette)
			palette_entries = info_ptr->num_palette;

		for (i=0; i < palette_entries; i++)
			set_pixel_bgra (&palette->Entries[i], 0,
				info_ptr->palette[i].blue,
				info_ptr->palette[i].green,
				info_ptr->palette[i].red,
				0xFF); /* alpha */
	}

	/* Make sure transparency is respected. */
	if (info_ptr->num_trans > 0) {
		palette->Flags |= PaletteFlagsHasAlpha;

		for (i=0; i < info_ptr->num_trans; i++) {
			int transparent_index = info_ptr->trans[i];

			if (transparent_index < num_colours)
				palette->Entries[i] = 0; /* 0 has an alpha value of 0x00 */
		}
	}

        png_destroy_read_struct (&png_ptr, &info_ptr, &end_info_ptr);

        img = gdip_bitmap_new ();
        img->image.type = imageBitmap;
        img->image.width = width;
        img->image.height = height;

        img->cairo_format = CAIRO_FORMAT_ARGB32;
        img->data.Stride = dest_stride;
        img->data.Width = width;
        img->data.Height = height;
        img->data.Scan0 = rawdata;
        img->data.Reserved = GBD_OWN_SCAN0;

	switch (bit_depth)
	{
		case 1: img->image.pixFormat = img->data.PixelFormat = Format1bppIndexed; img->cairo_format = CAIRO_FORMAT_A1; break;
		case 4: img->image.pixFormat = img->data.PixelFormat = Format4bppIndexed; img->cairo_format = CAIRO_FORMAT_A8; break;
		case 8: img->image.pixFormat = img->data.PixelFormat = Format8bppIndexed; img->cairo_format = CAIRO_FORMAT_A8; break;
	}

        img->image.imageFlags =
	    colourspace_flag | /* assigned when the palette is loaded */
            ImageFlagsReadOnly |
            ImageFlagsHasRealPixelSize;
        img->image.horizontalResolution = 0;
        img->image.verticalResolution = 0;
        img->image.propItems = NULL;
        img->image.palette = palette;
    }
    else {
        int width;
        int height;
        guchar bit_depth;
        guchar color_type;
        int channels;
        int stride;
        int interlace;
        png_bytep *row_pointers;
        guchar *rawptr;
        int i, j;

        width = png_get_image_width (png_ptr, info_ptr);
        height = png_get_image_height (png_ptr, info_ptr);
        bit_depth = png_get_bit_depth (png_ptr, info_ptr);
        color_type = png_get_color_type (png_ptr, info_ptr);
        channels = png_get_channels (png_ptr, info_ptr);
        interlace = png_get_interlace_type (png_ptr, info_ptr);

	/* According to the libpng manual, this sequence is equivalent to
	 * using the PNG_TRANSFORM_EXPAND flag in png_read_png.
	 */
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb (png_ptr);
	if ((color_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8))
		png_set_gray_1_2_4_to_8(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

        stride = width * 4;

        while (stride % sizeof(pixman_bits_t))
                stride++;


        row_pointers = png_get_rows (png_ptr, info_ptr);
        rawdata = GdipAlloc (stride * height);
        rawptr = rawdata;

	switch (channels) {

	case 4:
		for (i = 0; i < height; i++) {
			png_bytep rowp = row_pointers[i];
			for (j = 0; j < width; j++) {
				set_pixel_bgra (rawptr, 0, rowp[2], rowp[1], rowp[0], rowp[3]);
				rowp += 4;
				rawptr += 4;
			}
		}
		break;

	case 3:
		for (i = 0; i < height; i++) {
			png_bytep rowp = row_pointers[i];
			for (j = 0; j < width; j++) {
				set_pixel_bgra (rawptr, 0, rowp[2], rowp[1], rowp[0], 0xff);
				rowp += 3;
				rawptr += 4;
			}
		}
		break;

	case 1:
		for (i = 0; i < height; i++) {
			png_bytep rowp = row_pointers[i];
			for (j = 0; j < width; j++) {
				png_byte pix = *rowp++;
				set_pixel_bgra (rawptr, 0, pix, pix, pix, 0xff);
				rawptr += 4;
			}
		}
		break;
        }

        png_destroy_read_struct (&png_ptr, &info_ptr, &end_info_ptr);

        img = gdip_bitmap_new ();
        img->image.type = imageBitmap;
        img->image.width = width;
        img->image.height = height;

        img->cairo_format = CAIRO_FORMAT_ARGB32;
        img->data.Stride = stride;
        img->data.PixelFormat = Format32bppArgb;
        img->data.Width = width;
        img->data.Height = height;
        img->data.Scan0 = rawdata;
        img->data.Reserved = GBD_OWN_SCAN0;

        img->image.surface = cairo_image_surface_create_for_data ((unsigned char *)rawdata,
                                                             img->cairo_format,
                                                             img->image.width,
                                                             img->image.height,
                                                             img->data.Stride);
        if (channels == 3) {
            img->image.pixFormat = Format24bppRgb;
            img->image.imageFlags = ImageFlagsColorSpaceRGB;
        } else if (channels == 4) {
            img->image.pixFormat = Format32bppArgb;
            img->image.imageFlags = ImageFlagsColorSpaceRGB;
        } else if (channels == 1) {
            img->image.pixFormat = Format8bppIndexed;
            img->image.imageFlags = ImageFlagsColorSpaceGRAY;
        }
        img->image.imageFlags |=
            ImageFlagsReadOnly |
            ImageFlagsHasRealPixelSize;
        img->image.horizontalResolution = 0;
        img->image.verticalResolution = 0;
        img->image.propItems = NULL;
        img->image.palette = NULL;
    }

    if (pngsrc)
        GdipFree (pngsrc);

    *image = (GpImage *) img;

    return Ok;

  error:
    if (rawdata)
        GdipFree (rawdata);

    if (png_ptr) {
        png_destroy_read_struct (&png_ptr,
                                 info_ptr ? &info_ptr : (png_infopp) NULL,
                                 end_info_ptr ? &end_info_ptr : (png_infopp) NULL);
    }

    if (pngsrc)
        GdipFree (pngsrc);

    *image = NULL;
    return InvalidParameter;
}


GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage **image)
{
    return gdip_load_png_image_from_file_or_stream (fp, NULL, image);
}

GpStatus
gdip_load_png_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                          SeekDelegate seeknFunc,
                                          GpImage **image)
{
    return gdip_load_png_image_from_file_or_stream (NULL, getBytesFunc, image);
}

GpStatus 
gdip_save_png_image_to_file_or_stream (FILE *fp,
                                       PutBytesDelegate putBytesFunc,
                                       GpImage *image,
                                       GDIPCONST EncoderParameters *params)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    GpBitmap *bitmap = (GpBitmap *) image;
    gdip_stream_png_source_ptr pngsrc = NULL;

    int i, j;

    int bit_depth;
    int color_type;

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING,
                                       NULL, NULL, NULL);
    if (!png_ptr)
        goto error;

    if (setjmp(png_jmpbuf(png_ptr))) {
        /* png detected error occured */
        goto error;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        goto error;

    if (fp != NULL) {
        png_init_io (png_ptr, fp);
    } else {
        pngsrc = GdipAlloc(sizeof(struct gdip_stream_png_source));
        pngsrc->getBytesFunc = NULL;
        pngsrc->putBytesFunc = putBytesFunc;

        png_set_write_fn (png_ptr, pngsrc, _gdip_png_stream_write_data, _gdip_png_stream_flush_data);
    }

    switch (image->pixFormat) {
        case Format32bppArgb:
        case Format32bppPArgb:
        case Format32bppRgb:
        case Format24bppRgb:
        case Format8bppIndexed:
            bit_depth = 8;
            break;
	case Format4bppIndexed:
	    bit_depth = 4;
	    break;
	case Format1bppIndexed:
	    bit_depth = 1;
	    break;
        /* We're not going to even try to save these images, for now */
        case Format64bppArgb:
        case Format64bppPArgb:
        case Format48bppRgb:
        case Format16bppArgb1555:
        case Format16bppGrayScale:
        case Format16bppRgb555:
        case Format16bppRgb565:
        default:
            bit_depth = -1;
            break;
    }
    if (bit_depth == -1)
        goto error;

    switch (image->pixFormat) {
        case Format32bppArgb:
        case Format32bppPArgb:
            color_type = PNG_COLOR_TYPE_RGB_ALPHA;
            break;
           
            break;
        case Format32bppRgb:
        case Format24bppRgb:
            color_type = PNG_COLOR_TYPE_RGB; /* XXX - we should be able to write grayscale PNGs */
            break;
        case Format8bppIndexed:
        case Format4bppIndexed:
        case Format1bppIndexed:
 	    color_type = PNG_COLOR_TYPE_PALETTE;
	    break;
        case Format64bppArgb:
        case Format64bppPArgb:
        case Format48bppRgb:
        case Format16bppArgb1555:
        case Format16bppGrayScale:
        case Format16bppRgb555:
        case Format16bppRgb565:
        default:
            color_type = -1;
            break;
    }
    if (color_type == -1)
        goto error;

    png_set_IHDR (png_ptr, info_ptr,
                  image->width,
                  image->height,
                  bit_depth,
                  color_type,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);

    if (gdip_is_an_indexed_pixelformat (bitmap->data.PixelFormat)) {
	png_color palette[256];

	int palette_entries = image->palette->Count;
	if (bitmap->data.PixelFormat == Format4bppIndexed)
		palette_entries = 16;

	for (i=0; i < palette_entries; i++) {
		ARGB entry = image->palette->Entries[i];

		int dummy;

		get_pixel_bgra(entry,
			palette[i].blue,
			palette[i].green,
			palette[i].red,
			dummy);
	}

	png_set_PLTE (png_ptr, info_ptr, palette, palette_entries);
    }

    png_write_info (png_ptr, info_ptr);

    if (image->pixFormat != bitmap->data.PixelFormat) {
        if (image->pixFormat == Format24bppRgb ||
            image->pixFormat == Format32bppRgb)
        {
            png_set_filler (png_ptr, 0, PNG_FILLER_AFTER);
        } else if (image->pixFormat == Format8bppIndexed)
        {
            png_set_filler (png_ptr, 0, PNG_FILLER_AFTER);
        }
    }

    png_set_bgr(png_ptr);

    if (gdip_is_an_indexed_pixelformat (bitmap->data.PixelFormat)) {
	for (i = 0; i < image->height; i++) {
		png_write_row (png_ptr, bitmap->data.Scan0 + i * bitmap->data.Stride);
	}
    }
    else {
#ifdef WORDS_BIGENDIAN
	guchar *row_pointer = GdipAlloc (image->width * 4);

	for (i = 0; i < image->height; i++) {
		for (j = 0; j < image->width; j++) {
			row_pointer[j*4] = *((guchar *)bitmap->data.Scan0 + (bitmap->data.Stride * i) + (j*4) + 3);
			row_pointer[j*4+1] = *((guchar *)bitmap->data.Scan0 + (bitmap->data.Stride * i) + (j*4) + 2);
			row_pointer[j*4+2] = *((guchar *)bitmap->data.Scan0 + (bitmap->data.Stride * i) + (j*4) + 1);
			row_pointer[j*4+3] = *((guchar *)bitmap->data.Scan0 + (bitmap->data.Stride * i) + (j*4) + 0);
		}
		png_write_row (png_ptr, row_pointer);
	}
	GdipFree (row_pointer);
#else
	for (i = 0; i < image->height; i++) {
		png_write_row (png_ptr, bitmap->data.Scan0 + (bitmap->data.Stride * i));
	}
#endif
    }

    png_write_end (png_ptr, NULL);

    png_destroy_write_struct (&png_ptr, &info_ptr);

    if (pngsrc)
        GdipFree (pngsrc);

    return Ok;
error:
    if (png_ptr)
        png_destroy_write_struct (&png_ptr, info_ptr ? &info_ptr : NULL);
    if (pngsrc)
        GdipFree (pngsrc);

    return GenericError;
}

GpStatus 
gdip_save_png_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
    return gdip_save_png_image_to_file_or_stream (fp, NULL, image, params);
}

GpStatus
gdip_save_png_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image,
                                        GDIPCONST EncoderParameters *params)
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
gdip_load_png_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                          SeekDelegate seeknFunc,
                                          GpImage **image)
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
gdip_save_png_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image, GDIPCONST EncoderParameters *params)
{

	return UnknownImageFormat;
}


ImageCodecInfo *
gdip_getcodecinfo_png ()
{
	return NULL;
}


#endif

