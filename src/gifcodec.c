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

#include <stdio.h>
#include "gifcodec.h"

#ifdef HAVE_LIBGIF

#include <gif_lib.h>

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
    GifFileType *gif = NULL;
    GifRecordType rectype;
    int res;

    GpBitmap *img = NULL;
    ColorPalette *pal = NULL;
    guchar *pixptr, *pixels = NULL;

    gif = DGifOpenFileHandle (fileno (fp));
    if (gif == NULL) {
        goto error;
    }

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
        for (int i = 0; i < gif->SColorMap->ColorCount; i++) {
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

    img->data.Stride = gif->SWidth * 4;
    img->data.PixelFormat = img->image.pixFormat;
    img->data.Width = img->image.width;
    img->data.Height = img->image.height;

    pixels = GdipAlloc (img->data.Stride * img->data.height);
    pixptr = pixels;

    do {
        res = DGifGetRecordType (gif, &rectype);
    } while (res == GIF_OK && rectype != IMAGE_DESC_RECORD_TYPE);

    if (res != GIF_OK)
        goto error;

    res = DGifGetImageDesc (gif);
    if (res != GIF_OK)
        goto error;

    for (int i = 0; i < img->image.height; i++) {
        guchar *readptr, *writeptr;
        res = DGifGetLine(gif, pixptr, img->data.Width);
        if (res != GIF_OK)
            goto error;
        /* Explode the line from indexed data into real data */
        readptr = pixptr + img->image.width - 1;
        writeptr = pixptr + (img->data.width - 1) * 4;
        for (int j = 0; j < img->image.width; j++) {
            guchar pix = *readptr--;
            writeptr[0] = 255;  /* A */
            if (pal) {
                writeptr[1] = pal->Colors[pix].Blue;
                writeptr[2] = pal->Colors[pix].Green;
                writeptr[3] = pal->Colors[pix].Red;
            } else {
                writeptr[1] = pix;
                writeptr[2] = pix;
                writeptr[3] = pix;
            }
            writeptr -= 4;
        }

        pixptr += img->data.Stride;
    }

    *image = img;
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
