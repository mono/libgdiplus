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

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_EXPAND, NULL);

    {
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

        stride = width * 4;

        row_pointers = png_get_rows (png_ptr, info_ptr);
        rawdata = GdipAlloc (stride * height);
        rawptr = rawdata;

	switch (channels) {

	case 4:
		for (i = 0; i < height; i++) {
			memcpy (rawptr, row_pointers[i], stride);
			rawptr += stride;
		}
		break;

	case 3:
		for (i = 0; i < height; i++) {
			png_bytep rowp = row_pointers[i];
			for (j = 0; j < width; j++) {
				*rawptr++ = *rowp++;
				*rawptr++ = *rowp++;
				*rawptr++ = *rowp++;
				*rawptr++ = 255; /* a */
			}
		}
		break;

	case 1:
		for (i = 0; i < height; i++) {
			png_bytep rowp = row_pointers[i];
			for (j = 0; j < width; j++) {
				png_byte pix = *rowp++;
				*rawptr++ = pix;
				*rawptr++ = pix;
				*rawptr++ = pix;
				*rawptr++ = 255;
			}
		}
		break;
        }

        png_destroy_read_struct (&png_ptr, &info_ptr, &end_info_ptr);

        img = gdip_bitmap_new ();
        img->image.type = imageBitmap;
        img->image.graphics = 0;
        img->image.width = width;
        img->image.height = height;

        img->cairo_format = CAIRO_FORMAT_ARGB32;
        img->data.Stride = stride;
        img->data.PixelFormat = Format32bppArgb;
        img->data.Width = width;
        img->data.Height = height;
        img->data.Scan0 = rawdata;
        img->data.Reserved = GBD_OWN_SCAN0;

        img->image.surface = cairo_surface_create_for_image (rawdata,
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

    int i;

    int bit_depth;
    int color_type;

    png_bytep *row_pointers = NULL;

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
        /* We're not going to even try to save these images, for now */
        case Format64bppArgb:
        case Format64bppPArgb:
        case Format48bppRgb:
        case Format16bppArgb1555:
        case Format16bppGrayScale:
        case Format16bppRgb555:
        case Format16bppRgb565:
        case Format4bppIndexed:
        case Format1bppIndexed:
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
        case Format8bppIndexed:
            color_type = PNG_COLOR_TYPE_RGB; /* XXX - we should be able to write grayscale PNGs */
            break;
        case Format64bppArgb:
        case Format64bppPArgb:
        case Format48bppRgb:
        case Format16bppArgb1555:
        case Format16bppGrayScale:
        case Format16bppRgb555:
        case Format16bppRgb565:
        case Format4bppIndexed:
        case Format1bppIndexed:
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

    row_pointers = GdipAlloc(sizeof (png_bytep) * image->height);

    for (i = 0; i < image->height; i++)
        row_pointers[i] = bitmap->data.Scan0 + (bitmap->data.Stride * i);

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

    png_write_rows (png_ptr, row_pointers, image->height);
    png_write_end (png_ptr, NULL);

    png_destroy_write_struct (&png_ptr, &info_ptr);

    GdipFree (row_pointers);

    if (pngsrc)
        GdipFree (pngsrc);

    return Ok;
error:
    if (png_ptr)
        png_destroy_write_struct (&png_ptr, info_ptr ? &info_ptr : NULL);
    if (row_pointers)
        GdipFree(row_pointers);
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
	return NotImplemented;
}

GpStatus
gdip_load_png_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                          SeekDelegate seeknFunc,
                                          GpImage **image)
{
	*image = NULL;
	return NotImplemented;
}


GpStatus 
gdip_save_png_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return NotImplemented;
}


GpStatus
gdip_save_png_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image, GDIPCONST EncoderParameters *params)
{

	return NotImplemented;
}


ImageCodecInfo *
gdip_getcodecinfo_png ()
{
	return NULL;
}


#endif

