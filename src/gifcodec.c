/* -*- Mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*-
 *
 * gifcodec.c : Contains function definitions for encoding decoding gif images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "gifcodec.h"

#ifdef HAVE_LIBUNGIF

#include <gif_lib.h>

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
    GifFileType *gif = NULL;
    GifRecordType rectype;
    int res;

    GpBitmap *img = NULL;
    ColorMapObject *pal = NULL;
    guchar *pixels = NULL;
    guchar *readptr, *writeptr;

    int i, j;

    gif = DGifOpenFileHandle (fileno (fp));
    if (gif == NULL)
        goto error;

    res = DGifSlurp (gif);
    if (res != GIF_OK)
        goto error;

    img = gdip_bitmap_new ();
    img->image.type = imageBitmap;
    img->image.graphics = 0;
    img->image.width = gif->SWidth;
    img->image.height = gif->SHeight;

    /* Note that Cairo/libpixman does not have support for indexed
     * images, so we expand gifs out to 32bpp argb.
     * This is unfortunate.
     */
#if 0
    /* Copy the palette over, if there is one */
    if (gif->SColorMap != NULL) {
        ColorPalette *pal = g_malloc (sizeof(ColorPalette) + sizeof(ARGB) * gif->SColorMap->ColorCount);
        pal->Flags = 0;
        pal->Count = gif->SColorMap->ColorCount;
        for (i = 0; i < gif->SColorMap->ColorCount; i++) {
            pal->Entries[i] = MAKE_ARGB_RGB(gif->SColorMap->Colors[i].Red,
                                            gif->SColorMap->Colors[i].Green,
                                            gif->SColorMap->Colors[i].Blue);
        }

        img->image.palette = pal;
    }
    img->image.pixFormat = Format8bppIndexed;
#endif

    pal = gif->SColorMap;

    img->cairo_format = CAIRO_FORMAT_ARGB32;
    img->image.pixFormat = Format32bppArgb;

    img->data.PixelFormat = img->image.pixFormat;
    img->data.Width = img->image.width;
    img->data.Height = img->image.height;
    img->data.Stride = img->data.Width * 4;

    pixels = GdipAlloc (img->data.Stride * img->data.Height);

    readptr = gif->SavedImages->RasterBits;
    writeptr = pixels;
    for (i = 0; i < img->image.width * img->image.height; i++) {
        guchar pix = *readptr++;
        if (pal) {
            *writeptr++ = pal->Colors[pix].Blue;
            *writeptr++ = pal->Colors[pix].Green;
            *writeptr++ = pal->Colors[pix].Red;
        } else {
            *writeptr++ = pix;
            *writeptr++ = pix;
            *writeptr++ = pix;
        }
        *writeptr++ = 255;  /* A */
    }

    img->data.Scan0 = pixels;
    img->data.Reserved = GBD_OWN_SCAN0;

    img->image.surface = cairo_surface_create_for_image (pixels, img->cairo_format,
                                                   img->image.width, img->image.height,
                                                   img->data.Stride);
    img->image.horizontalResolution = gdip_get_display_dpi ();
    img->image.verticalResolution = gdip_get_display_dpi ();
    img->image.propItems = NULL;
    img->image.palette = NULL;

    *image = (GpImage *) img;
    return Ok;
  error:
    if (pixels)
        GdipFree (pixels);
    if (img)
        gdip_bitmap_dispose (img);
    if (gif)
        DGifCloseFile (gif);
    *image = NULL;
    return InvalidParameter;
}

GpStatus 
gdip_save_gif_image_to_file (FILE *fp, GpImage *image)
{
    return NotImplemented;
}

#else

/* No libgif */

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
    *image = NULL;
    return NotImplemented;
}

GpStatus 
gdip_save_gif_image_to_file (FILE *fp, GpImage *image)
{
    return NotImplemented;
}

#endif
