/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * jpegcodec.c : Contains function definitions for encoding decoding jpeg images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *      Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include <config.h>

#include <stdio.h>
#include "jpegcodec.h"

#ifdef HAVE_LIBJPEG

#include <setjmp.h>
#include <jpeglib.h>

#ifndef HAVE_SIGSETJMP
#define sigjmp_buf jmp_buf
#define sigsetjmp(jb, x) setjmp(jb)
#define siglongjmp longjmp
#endif

#define JPEG_READ_BUFFER_SIZE	65536

struct gdip_jpeg_source_mgr {
    struct jpeg_source_mgr parent;

    FILE *infp;
    JOCTET *buf;
};
typedef struct gdip_jpeg_source_mgr *gdip_jpeg_source_mgr_ptr;

struct gdip_jpeg_error_mgr {
    struct jpeg_error_mgr parent;
    sigjmp_buf setjmp_buffer;
    GError **error;
};
typedef struct gdip_jpeg_error_mgr *gdip_jpeg_error_mgr_ptr;

static void
_gdip_jpeg_error_exit (j_common_ptr cinfo)
{
    gdip_jpeg_error_mgr_ptr err = (gdip_jpeg_error_mgr_ptr) cinfo->err;
    char buffer[JMSG_LENGTH_MAX];

    (* cinfo->err->format_message) (cinfo, buffer);

    /* There is some very bizzare stuff happening with error
     * reporting.  These are supposed to be fatal errors,
     * but libjpeg goes through a bunch of these calls and then
     * -finishes uncompressing the image-.  I have no idea
     * what's going on here.
     * -- FIXME
     */
#if 0
    printf ("jpeg error: %d %s\n", err->parent.msg_code, buffer);
    siglongjmp (err->setjmp_buffer, 1);
#endif
}

static void
_gdip_jpeg_output_message (j_common_ptr cinfo)
{
    /* shh. */
}

static void
_gdip_source_stdio_init (j_decompress_ptr cinfo)
{
    /* nothing */
}

static bool
_gdip_source_stdio_fill_input_buffer (j_decompress_ptr cinfo)
{
    gdip_jpeg_source_mgr_ptr src = (gdip_jpeg_source_mgr_ptr) cinfo->src;
    size_t nb;

    nb = fread (src->buf, 1, JPEG_READ_BUFFER_SIZE, src->infp);

    if (nb <= 0) {
        /* this is a hack learned from gdk-pixbuf */
        /* insert fake EOI marker, to try to salvage image
         * in case of malformed/incomplete input
         */
        src->buf[0] = (JOCTET) 0xFF;
        src->buf[1] = (JOCTET) JPEG_EOI;
        nb = 2;
    }

    src->parent.next_input_byte = src->buf;
    src->parent.bytes_in_buffer = nb;

    return TRUE;
}

static void
_gdip_source_stdio_skip_input_data (j_decompress_ptr cinfo, long skipbytes)
{
    gdip_jpeg_source_mgr_ptr src = (gdip_jpeg_source_mgr_ptr) cinfo->src;

    if (skipbytes > 0) {
        if (skipbytes > (long) src->parent.bytes_in_buffer) {
            skipbytes -= (long) src->parent.bytes_in_buffer;
            fseek (src->infp, skipbytes, SEEK_CUR);

            (void) _gdip_source_stdio_fill_input_buffer (cinfo);
        } else {
            src->parent.next_input_byte += (size_t) skipbytes;
            src->parent.bytes_in_buffer -= (size_t) skipbytes;
        }
    }
}

static void
_gdip_source_stdio_term (j_decompress_ptr cinfo)
{
    /* nothing */
}

GpStatus 
gdip_load_jpeg_image_from_file (FILE *fp, GpImage **image)
{
    GpBitmap *img = NULL;
    struct jpeg_decompress_struct cinfo;
    struct gdip_jpeg_error_mgr jerr;
    gdip_jpeg_source_mgr_ptr src = NULL;

    guchar *destbuf = NULL, *destptr = NULL;
    guchar *lines[4] = {NULL, NULL, NULL, NULL};

    int stride;

    img = gdip_bitmap_new ();

    cinfo.err = jpeg_std_error ((struct jpeg_error_mgr *) &jerr);
    jerr.parent.error_exit = _gdip_jpeg_error_exit;
    jerr.parent.output_message = _gdip_jpeg_output_message;

    if (sigsetjmp (jerr.setjmp_buffer, 1)) {
        /* Error occured during decompression */
        jpeg_destroy_decompress (&cinfo);

        if (destbuf != NULL)
            g_free (destbuf);

        gdip_bitmap_dispose (img);
        *image = NULL;
        return GenericError;
    }

    jpeg_create_decompress (&cinfo);

    cinfo.src = (struct jpeg_source_mgr *)
        (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT, sizeof (struct gdip_jpeg_source_mgr));
    src = (gdip_jpeg_source_mgr_ptr) cinfo.src;
    src->buf = (JOCTET *)
        (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT, JPEG_READ_BUFFER_SIZE * sizeof(JOCTET));
    src->parent.init_source = _gdip_source_stdio_init;
    src->parent.fill_input_buffer = _gdip_source_stdio_fill_input_buffer;
    src->parent.skip_input_data = _gdip_source_stdio_skip_input_data;
    src->parent.resync_to_restart = jpeg_resync_to_restart;
    src->parent.bytes_in_buffer = 0;
    src->parent.next_input_byte = NULL;

    src->infp = fp;
    
    jpeg_read_header (&cinfo, TRUE);

    cinfo.do_fancy_upsampling = FALSE;
    cinfo.do_block_smoothing = FALSE;
    jpeg_start_decompress (&cinfo);

    img->image.type = imageBitmap;
    img->image.graphics = 0;
    img->image.width = cinfo.output_width;
    img->image.height = cinfo.output_height;

    if (cinfo.out_color_space != JCS_GRAYSCALE &&
        cinfo.out_color_space != JCS_RGB)
    {
        g_warning ("Unsupported JPEG color space: %d", cinfo.out_color_space);
        jpeg_destroy_decompress (&cinfo);
        gdip_bitmap_dispose (img);
        *image = NULL;
        return InvalidParameter;
    }

    if (cinfo.out_color_components == 1) {
        img->image.pixFormat = Alpha;
        img->cairo_format = CAIRO_FORMAT_A8;
        img->data.Stride = cinfo.out_color_components * cinfo.output_width;
        img->data.PixelFormat = img->image.pixFormat;
    } else if (cinfo.out_color_components == 3) {
        /* We need to expand the data out to 32bpp for cairo */
        img->image.pixFormat = Format32bppArgb;
        img->cairo_format = CAIRO_FORMAT_ARGB32;
        img->data.Stride = 4 * cinfo.output_width;
        img->data.PixelFormat = img->image.pixFormat;
    } else if (cinfo.out_color_components == 4) {
        img->image.pixFormat = Format32bppArgb;
        img->cairo_format = CAIRO_FORMAT_ARGB32;
        img->data.Stride = cinfo.out_color_components * cinfo.output_width;
        img->data.PixelFormat = img->image.pixFormat;
    } else {
        g_warning ("Unsupported JPEG out_color_components: %d", cinfo.out_color_components);
        jpeg_destroy_decompress (&cinfo);
        gdip_bitmap_dispose (img);
        *image = NULL;
        return InvalidParameter;
    }

    img->data.Width = img->image.width;
    img->data.Height = img->image.height;

    stride = img->data.Stride;

    destbuf = GdipAlloc (stride * cinfo.output_height);
    destptr = destbuf;

    img->data.Scan0 = destbuf;
    img->data.Reserved = GBD_OWN_SCAN0;

    jpeg_start_decompress (&cinfo);

    while (cinfo.output_scanline < cinfo.output_height) {
        int i;
        for (i = 0; i < cinfo.rec_outbuf_height; i++) {
            lines[i] = destptr;
            destptr += stride;
        }

        jpeg_read_scanlines (&cinfo, lines, cinfo.rec_outbuf_height);

        /* if we have RGB data, we need to expand this out to ARGB */
        if (cinfo.out_color_components == 3) {
            for (i = 0; i < cinfo.rec_outbuf_height; i++) {
                int j;
                guchar *inptr, *outptr;
                JOCTET r, g, b;

                inptr = lines[i] + (img->image.width) * 3 - 1;
                outptr = lines[i] + stride - 1;
                for (j = 0; j < img->image.width; j++) {
                    /* Note the swapping of R and B, to get ARGB from what
                     * looks like BGR data.
                     * We really need to do some tests to see what this stuff
                     * looks like inside windows.
                     */
                    r = *inptr--;
                    g = *inptr--;
                    b = *inptr--;
                    *outptr-- = 255;
                    *outptr-- = b;
                    *outptr-- = g;
                    *outptr-- = r;
                }
            }
        }
    }

    img->image.surface = cairo_surface_create_for_image (destbuf, img->cairo_format,
                                                   img->image.width, img->image.height,
                                                   stride);
    img->image.horizontalResolution = gdip_get_display_dpi ();
    img->image.verticalResolution = gdip_get_display_dpi ();
    img->image.propItems = NULL;
    img->image.palette = NULL;

    *image = (GpImage *) img;
    return Ok;
}

GpStatus 
gdip_save_jpeg_image_to_file (FILE *fp, GpImage *image)
{
    return NotImplemented;
}

#else

/* No libjpeg */
GpStatus
gdip_load_jpeg_image_from_file (FILE *fp, GpImage **image)
{
    *image = NULL;
    return NotImplemented;
}

GpStatus 
gdip_save_jpeg_image_to_file (FILE *fp, GpImage *image)
{
    return NotImplemented;
}

#endif
