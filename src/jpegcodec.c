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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "jpegcodec.h"
#include "gdipImage.h"

#ifdef HAVE_LIBJPEG

#include <setjmp.h>
#include <jpeglib.h>

#if !defined(HAVE_SIGSETJMP) && !defined(sigsetjmp)
#define sigjmp_buf jmp_buf
#define sigsetjmp(jb, x) setjmp(jb)
#define siglongjmp longjmp
#endif

#define JPEG_BUFFER_SIZE	65536

struct gdip_stdio_jpeg_source_mgr {
    struct jpeg_source_mgr parent;

    FILE *infp;
    JOCTET *buf;
};
typedef struct gdip_stdio_jpeg_source_mgr *gdip_stdio_jpeg_source_mgr_ptr;

struct gdip_stream_jpeg_source_mgr {
    struct jpeg_source_mgr parent;
    GetBytesDelegate getBytesFunc;
    SeekDelegate seekFunc;

    JOCTET *buf;
};
typedef struct gdip_stream_jpeg_source_mgr *gdip_stream_jpeg_source_mgr_ptr;

struct gdip_stream_jpeg_dest_mgr {
    struct jpeg_destination_mgr parent;

    PutBytesDelegate putBytesFunc;

    JOCTET *buf;
};
typedef struct gdip_stream_jpeg_dest_mgr *gdip_stream_jpeg_dest_mgr_ptr;

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
_gdip_source_dummy_init (j_decompress_ptr cinfo)
{
    /* nothing */
}

static bool
_gdip_source_stdio_fill_input_buffer (j_decompress_ptr cinfo)
{
    gdip_stdio_jpeg_source_mgr_ptr src = (gdip_stdio_jpeg_source_mgr_ptr) cinfo->src;
    size_t nb;

    nb = fread (src->buf, 1, JPEG_BUFFER_SIZE, src->infp);

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
    gdip_stdio_jpeg_source_mgr_ptr src = (gdip_stdio_jpeg_source_mgr_ptr) cinfo->src;

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

static bool
_gdip_source_stream_fill_input_buffer (j_decompress_ptr cinfo)
{
    gdip_stream_jpeg_source_mgr_ptr src = (gdip_stream_jpeg_source_mgr_ptr) cinfo->src;
    size_t nb;

    nb = src->getBytesFunc (src->buf, JPEG_BUFFER_SIZE, 0);
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
_gdip_source_stream_skip_input_data (j_decompress_ptr cinfo, long skipbytes)
{
    gdip_stream_jpeg_source_mgr_ptr src = (gdip_stream_jpeg_source_mgr_ptr) cinfo->src;

    if (skipbytes > 0) {
        if (skipbytes > (long) src->parent.bytes_in_buffer) {
            skipbytes -= (long) src->parent.bytes_in_buffer;

            if (src->seekFunc != NULL) {
                src->seekFunc (skipbytes, SEEK_CUR);
            } else {
                /* getBytes knows how to "read" into a NULL buffer */
                while (skipbytes >= 0)
                    skipbytes -= src->getBytesFunc (NULL, skipbytes, 0);
            }

            (void) _gdip_source_stream_fill_input_buffer (cinfo);
        } else {
            src->parent.next_input_byte += (size_t) skipbytes;
            src->parent.bytes_in_buffer -= (size_t) skipbytes;
        }
    }
}

static void
_gdip_source_dummy_term (j_decompress_ptr cinfo)
{
    /* nothing */
}

static void
_gdip_dest_stream_init (j_compress_ptr cinfo)
{
    gdip_stream_jpeg_dest_mgr_ptr dest = (gdip_stream_jpeg_dest_mgr_ptr) cinfo->dest;
    dest->parent.next_output_byte = dest->buf;
    dest->parent.free_in_buffer = JPEG_BUFFER_SIZE;
}

static BOOL
_gdip_dest_stream_empty_output_buffer (j_compress_ptr cinfo)
{
    gdip_stream_jpeg_dest_mgr_ptr dest = (gdip_stream_jpeg_dest_mgr_ptr) cinfo->dest;

    dest->putBytesFunc (dest->buf, JPEG_BUFFER_SIZE);

    dest->parent.next_output_byte = dest->buf;
    dest->parent.free_in_buffer = JPEG_BUFFER_SIZE;

    return TRUE;
}

static void
_gdip_dest_stream_term (j_compress_ptr cinfo)
{
    gdip_stream_jpeg_dest_mgr_ptr dest = (gdip_stream_jpeg_dest_mgr_ptr) cinfo->dest;

    dest->putBytesFunc (dest->buf, JPEG_BUFFER_SIZE - dest->parent.free_in_buffer);
}


GpStatus
gdip_load_jpeg_image_internal (struct jpeg_source_mgr *src,
                               GpImage **image)
{
    GpBitmap *img = NULL;
    struct jpeg_decompress_struct cinfo;
    struct gdip_jpeg_error_mgr jerr;

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
    cinfo.src = src;

    jpeg_read_header (&cinfo, TRUE);

    cinfo.do_fancy_upsampling = FALSE;
    cinfo.do_block_smoothing = FALSE;

    img->image.type = imageBitmap;
    img->image.graphics = 0;
    img->image.width = cinfo.image_width;
    img->image.height = cinfo.image_height;

    if (cinfo.jpeg_color_space != JCS_GRAYSCALE &&
        cinfo.jpeg_color_space != JCS_RGB &&
        cinfo.jpeg_color_space != JCS_YCbCr)
    {
        g_warning ("Unsupported JPEG color space: %d", cinfo.jpeg_color_space);
        jpeg_destroy_decompress (&cinfo);
        gdip_bitmap_dispose (img);
        *image = NULL;
        return InvalidParameter;
    }

    if (cinfo.num_components == 1) {
        img->image.pixFormat = Format8bppIndexed;
        img->image.imageFlags = ImageFlagsColorSpaceGRAY;
    } else if (cinfo.num_components == 3) {
        img->image.pixFormat = Format24bppRgb;
        img->image.imageFlags = ImageFlagsColorSpaceRGB;
    } else {
        /* default to this; we assume libjpeg will be able to give us RGB */
        img->image.pixFormat = Format24bppRgb;
        img->image.imageFlags = ImageFlagsColorSpaceRGB;
    }

    img->cairo_format = CAIRO_FORMAT_ARGB32;
    img->data.Stride = 4 * cinfo.image_width;
    img->data.PixelFormat = Format32bppArgb;

    img->data.Width = img->image.width;
    img->data.Height = img->image.height;

    stride = img->data.Stride;

    /* Request cairo-compat output */
    cinfo.out_color_space = JCS_RGB;
    cinfo.out_color_components = 3;
    cinfo.output_components = 3;

    jpeg_start_decompress (&cinfo);

    destbuf = GdipAlloc (stride * cinfo.output_height);
    destptr = destbuf;

    while (cinfo.output_scanline < cinfo.output_height) {
        int i;
        int nlines;
        for (i = 0; i < cinfo.rec_outbuf_height; i++) {
            lines[i] = destptr;
            destptr += stride;
        }

        nlines = jpeg_read_scanlines (&cinfo, lines, cinfo.rec_outbuf_height);
        for (i = 0; i < nlines; i++) {
            int j;
            guchar *inptr, *outptr;
            JOCTET r, g, b;

            inptr = lines[i] + (img->image.width) * 3 - 1;
            outptr = lines[i] + stride - 1;
            for (j = 0; j < img->image.width; j++) {
                /* Note the swapping of R and B, to get ARGB from what
                 * looks like BGR data.
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

    jpeg_destroy_decompress (&cinfo);

    img->data.Scan0 = destbuf;
    img->data.Reserved = GBD_OWN_SCAN0;

    img->image.surface = cairo_surface_create_for_image (destbuf, img->cairo_format,
                                                   img->image.width, img->image.height,
                                                   stride);
    img->image.horizontalResolution = 0;
    img->image.verticalResolution = 0;
    /* win32 returns this as PartiallyScalable and ColorSpaceYCBCR; we
     * just return it as RGB (or Grayscale).
     */
    img->image.imageFlags |=
        ImageFlagsReadOnly |
        ImageFlagsHasRealPixelSize;
    img->image.propItems = NULL;
    img->image.palette = NULL;

    *image = (GpImage *) img;
    return Ok;
}

GpStatus 
gdip_load_jpeg_image_from_file (FILE *fp, GpImage **image)
{
    GpStatus st;

    gdip_stdio_jpeg_source_mgr_ptr src;

    src = (gdip_stdio_jpeg_source_mgr_ptr) GdipAlloc (sizeof (struct gdip_stdio_jpeg_source_mgr));
    src->buf = GdipAlloc (JPEG_BUFFER_SIZE * sizeof(JOCTET));

    src->parent.init_source = _gdip_source_dummy_init;
    src->parent.fill_input_buffer = _gdip_source_stdio_fill_input_buffer;
    src->parent.skip_input_data = _gdip_source_stdio_skip_input_data;
    src->parent.resync_to_restart = jpeg_resync_to_restart;
    src->parent.term_source = _gdip_source_dummy_term;
    src->parent.bytes_in_buffer = 0;
    src->parent.next_input_byte = NULL;

    src->infp = fp;

    st = gdip_load_jpeg_image_internal ((struct jpeg_source_mgr *) src, image);
    GdipFree (src->buf);
    GdipFree (src);

    return st;
}

GpStatus
gdip_load_jpeg_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           SeekDelegate seekFunc,
                                           GpImage **image)
{
    GpStatus st;

    gdip_stream_jpeg_source_mgr_ptr src;

    src = (gdip_stream_jpeg_source_mgr_ptr) GdipAlloc (sizeof (struct gdip_stream_jpeg_source_mgr));
    src->buf = GdipAlloc (JPEG_BUFFER_SIZE * sizeof(JOCTET));

    src->parent.init_source = _gdip_source_dummy_init;
    src->parent.fill_input_buffer = _gdip_source_stream_fill_input_buffer;
    src->parent.skip_input_data = _gdip_source_stream_skip_input_data;
    src->parent.resync_to_restart = jpeg_resync_to_restart;
    src->parent.term_source = _gdip_source_dummy_init;
    src->parent.bytes_in_buffer = 0;
    src->parent.next_input_byte = NULL;

    src->getBytesFunc = getBytesFunc;
    src->seekFunc = seekFunc;

    st = gdip_load_jpeg_image_internal ((struct jpeg_source_mgr *) src, image);
    GdipFree (src->buf);
    GdipFree (src);

    return st;
}

GpStatus
gdip_save_jpeg_image_internal (FILE *fp,
                               PutBytesDelegate putBytesFunc,
                               GpImage *image,
                               GDIPCONST EncoderParameters *params)
{
    GpBitmap *bitmap = (GpBitmap *) image;
    struct jpeg_compress_struct cinfo;
    struct gdip_jpeg_error_mgr jerr;
    gdip_stream_jpeg_dest_mgr_ptr dest = NULL;
    JOCTET *scanline = NULL;
    int need_argb_conversion = 0;

    /* Verify that we can support this pixel format */
    switch (image->pixFormat) {
        case Format32bppArgb:
        case Format32bppPArgb:
        case Format32bppRgb:
        case Format24bppRgb:
        case Format8bppIndexed: /* assume this is grayscale */
            break;
        default:
            return InvalidParameter;
            break;
    }

    cinfo.err = jpeg_std_error ((struct jpeg_error_mgr *) &jerr);
    jerr.parent.error_exit = _gdip_jpeg_error_exit;
    jerr.parent.output_message = _gdip_jpeg_output_message;

    if (sigsetjmp (jerr.setjmp_buffer, 1)) {
        /* Error occured */
        jpeg_destroy_compress (&cinfo);

        if (dest != NULL) {
            GdipFree (dest->buf);
            GdipFree (dest);
        }
        if (scanline != NULL && need_argb_conversion)
            GdipFree (scanline);

        return GenericError;
    }

    jpeg_create_compress (&cinfo);

    if (fp != NULL) {
        jpeg_stdio_dest (&cinfo, fp);
    } else {
        dest = GdipAlloc (sizeof (struct gdip_stream_jpeg_dest_mgr));
        dest->parent.init_destination = _gdip_dest_stream_init;
        dest->parent.empty_output_buffer = _gdip_dest_stream_empty_output_buffer;
        dest->parent.term_destination = _gdip_dest_stream_term;

        dest->putBytesFunc = putBytesFunc;
        dest->buf = GdipAlloc (JPEG_BUFFER_SIZE);

        cinfo.dest = (struct jpeg_destination_mgr *) dest;
    }

    cinfo.image_width = image->width;
    cinfo.image_height = image->height;
    if (gdip_get_pixel_format_components (image->pixFormat) == 3) {
        cinfo.in_color_space = JCS_RGB;
        cinfo.input_components = 3;
        if (image->pixFormat == Format24bppRgb)
            need_argb_conversion = 1;
        else
            need_argb_conversion = 0;
    } else if (gdip_get_pixel_format_components (image->pixFormat) == 4) {
        cinfo.in_color_space = JCS_RGB;
        cinfo.input_components = 3;
        need_argb_conversion = 1;
    } else if (gdip_get_pixel_format_components (image->pixFormat) == 1) {
        cinfo.in_color_space = JCS_RGB;
        cinfo.input_components = 1;
        need_argb_conversion = 1; /* do this conversion */
    }

    jpeg_set_defaults (&cinfo);

    if (gdip_get_pixel_format_components (image->pixFormat) == 1) {
        jpeg_set_colorspace (&cinfo, JCS_GRAYSCALE);
    }

    /* should handle encoding params here */

    jpeg_start_compress (&cinfo, TRUE);

    if (need_argb_conversion) {
        guchar *inptr, *outptr;
        int i, j;

        scanline = GdipAlloc (bitmap->data.Stride);

        for (i = 0; i < bitmap->data.Height; i++) {
            inptr = bitmap->data.Scan0 + (i * bitmap->data.Stride);
            outptr = scanline;

            for (j = 0; j < bitmap->data.Width; j++) {
                *outptr++ = inptr[2]; /* R */
                *outptr++ = inptr[1]; /* G */
                *outptr++ = inptr[0]; /* B */
                inptr += 4;        /* skip RGB+A */
            }

            jpeg_write_scanlines (&cinfo, &scanline, 1);
        }

        GdipFree (scanline);
    } else {
        int i;
        for (i = 0; i < bitmap->data.Height; i++) {
            scanline = bitmap->data.Scan0 + (i * bitmap->data.Stride);
            jpeg_write_scanlines (&cinfo, &scanline, 1);
        }
    }

    jpeg_finish_compress (&cinfo);
    jpeg_destroy_compress (&cinfo);

    if (dest != NULL) {
        GdipFree (dest->buf);
        GdipFree (dest);
    }

    return Ok;
}

GpStatus 
gdip_save_jpeg_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
    return gdip_save_jpeg_image_internal (fp, NULL, image, params);
}

GpStatus
gdip_save_jpeg_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)

{
    return gdip_save_jpeg_image_internal (NULL, putBytesFunc, image, params);
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
gdip_save_jpeg_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
    return NotImplemented;
}

GpStatus
gdip_load_jpeg_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           SeekDelegate seeknFunc,
                                           GpImage **image)
{
    *image = NULL;
    return NotImplemented;
}

GpStatus
gdip_save_jpeg_image_to_stream_delegate (utBytesDelegate putBytesFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)
{
    return NotImplemented;
}

#endif
