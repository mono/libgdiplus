/*
 * pngcodec.c : Contains function definitions for encoding decoding png images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic <vladimir@pobox.com>
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include <config.h>
#include <png.h>

#include "pngcodec.h"

#include <setjmp.h>

#ifndef HAVE_SIGSETJMP
#define sigjmp_buf jmp_buf
#define sigsetjmp(jb, x) setjmp(jb)
#define siglongjmp longjmp
#endif

GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage **image)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_infop end_info_ptr = NULL;
    guchar *rawdata = NULL;
    GpBitmap *img = NULL;

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

    png_init_io (png_ptr, fp);
    png_read_png(png_ptr, info_ptr, 0, NULL);

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

        /* Don't even bother with interlaced png files, ugh */
        if (interlace != 0)
            goto error;

        stride = width * 4;

        row_pointers = png_get_rows (png_ptr, info_ptr);
        rawdata = GdipAlloc (stride * height);
        rawptr = rawdata;

        for (i = 0; i < height; i++) {
            if (channels == 4) {
                memcpy (rawptr, row_pointers[i], stride);
                rawptr += stride;
            } else if (channels == 3) {
                png_bytep rowp = row_pointers[i];
                for (j = 0; j < width; j++) {
                    *rawptr++ = *rowp++; /* r */
                    *rawptr++ = *rowp++; /* g */
                    *rawptr++ = *rowp++; /* b */
                    *rawptr++ = 255; /* a */
                }
            } else if (channels == 1) {
                png_bytep rowp = row_pointers[i];
                for (j = 0; j < width; j++) {
                    png_byte pix = *rowp++;
                    *rawptr++ = pix;
                    *rawptr++ = pix;
                    *rawptr++ = pix;
                    *rawptr++ = 255;
                }
            }
        }

        png_destroy_read_struct (&png_ptr, &info_ptr, &end_info_ptr);

        img = gdip_bitmap_new ();
        img->image.type = imageBitmap;
        img->image.graphics = 0;
        img->image.width = width;;
        img->image.height = height;;

        img->image.pixFormat = Format32bppArgb;
        img->cairo_format = CAIRO_FORMAT_ARGB32;

        img->data.Stride = stride;
        img->data.PixelFormat = img->image.pixFormat;
        img->data.Width = width;
        img->data.Height = height;
        img->data.Scan0 = rawdata;
        img->data.Reserved = GBD_OWN_SCAN0;
    }

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

    *image = NULL;
    return InvalidParameter;
}

GpStatus 
gdip_save_png_image_to_file (FILE *fp, GpImage *image)
{
    return NotImplemented;
}
