/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * tiffcodec.c : Contains function definitions for encoding decoding tiff images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include <stdio.h>
#include "tiffcodec.h"

#ifdef HAVE_LIBTIFF

/* Thankfully, libtiff sucks far less than libjpeg */
#include <tiffio.h>

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image)
{
    GpBitmap *img = NULL;
    TIFF *tif = NULL;
    guint32 *raster = NULL;

    tif = TIFFFdOpen(fileno (fp), "lose.tif", "r");
    if (tif) {
        TIFFRGBAImage tifimg;
        char emsg[1024];

        if (TIFFRGBAImageBegin(&tifimg, tif, 0, emsg)) {
            size_t npixels;

            img = gdip_bitmap_new ();
            img->image.type = imageBitmap;
            img->image.graphics = 0;
            img->image.width = tifimg.width;
            img->image.height = tifimg.height;
            /* libtiff expands stuff out to ARGB32 for us if we use this interface */
            img->image.pixFormat = Format32bppArgb;
            img->cairo_format = CAIRO_FORMAT_ARGB32;
            img->data.Stride = tifimg.width * 4;
            img->data.PixelFormat = img->image.pixFormat;
            img->data.Width = img->image.width;
            img->data.Height = img->image.height;

            npixels = tifimg.width * tifimg.height;
            /* Note that we don't use _TIFFmalloc */
            raster = (guint32*) GdipAlloc (npixels * sizeof (guint32));
            if (raster != NULL) {
                if (TIFFRGBAImageGet(&tifimg, raster, tifimg.width, tifimg.height)) { 
                    img->data.Scan0 = raster;
                    img->data.Reserved = GBD_OWN_SCAN0;
                }
            } else {
                goto error;
            }

            TIFFRGBAImageEnd(&tifimg);
        } else {
            goto error;
        }

        TIFFClose(tif);
    } else {
        goto error;
    }

    *image = img;
    return Ok;

  error:

    if (raster)
        GdipFree (raster);
    if (img)
        gdip_bitmap_dispose (img);
    if (tif)
        TIFFClose(tif);
    *image = NULL;
    return InvalidParameter;
}

GpStatus 
gdip_save_tiff_image_to_file (FILE *fp, GpImage *image)
{
    return NotImplemented;
}

#else

/* no libtiff */

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image)
{
    *image = NULL;
    return NotImplemented;
}

GpStatus 
gdip_save_tiff_image_to_file (FILE *fp, GpImage *image)
{
    return NotImplemented;
}

#endif
