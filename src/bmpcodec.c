/*
 * bmpcodec.c : Contains function definitions for encoding decoding bmp images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *      Mark Steele (ms@rapidsys.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include <stdio.h>
#include "bmpcodec.h"


GpStatus 
gdip_load_bmp_image_from_file (FILE *fp, GpImage **image)
{
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmi;
    GpBitmap *img = NULL;
    guchar *pixels = NULL, *linep = NULL;
    int i, j;

    fread(&bmfh, sizeof(bmfh), 1, fp);
    if (bmfh.bfType != BFT_BITMAP)
        return UnknownImageFormat;

    fread(&bmi, sizeof(bmi), 1, fp);

    fseek(fp, bmfh.bfOffBits, SEEK_SET);

    img = gdip_bitmap_new ();
    img->image.type = imageBitmap;
    img->image.graphics = 0;
    img->image.width = bmi.biWidth;
    img->image.height = bmi.biHeight;

    img->cairo_format = CAIRO_FORMAT_ARGB32;
    img->image.pixFormat = Format32bppArgb;

    img->data.PixelFormat = img->image.pixFormat;
    img->data.Width = img->image.width;
    img->data.Height = img->image.height;
    img->data.Stride = img->data.Width * 4;

    pixels = GdipAlloc (img->data.Stride * img->data.Height);

    for (i = img->data.Height - 1; i >= 0; i--) {
        fread(pixels + i*img->data.Stride, img->data.Stride, 1, fp);
    }

    /* set alpha */
    for (i = 0; i < img->data.Height; i++) {
        linep = pixels + img->data.Stride * i;
        for (j = 1; j < img->data.Width; j++)
            linep[j*4-1] = 0xff;
    }

    img->data.Scan0 = pixels;
    img->data.Reserved = GBD_OWN_SCAN0;
    img->image.surface = cairo_surface_create_for_image (pixels,
                                                         img->cairo_format,
                                                         img->image.width,
                                                         img->image.height,
                                                         img->data.Stride);
    img->image.imageFlags =
        ImageFlagsReadOnly |
        ImageFlagsHasRealPixelSize |
        ImageFlagsColorSpaceRGB;
    img->image.horizontalResolution = 0;
    img->image.verticalResolution = 0;
    img->image.propItems = NULL;
    img->image.palette = NULL;

    *image = (GpImage *) img;

    return Ok;
}

GpStatus 
gdip_load_bmp_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                          SeekDelegate seeknFunc,
                                          GpImage **image)
{
    return NotImplemented;
}

GpStatus 
gdip_save_bmp_image_to_file (FILE *fp, GpImage *image)
{
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmi;
    GpBitmap *bitmap = (GpBitmap *) image;
    int  bitmapLen = bitmap->data.Stride * bitmap->data.Height;
         
    bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
    bmfh.bfType = BFT_BITMAP;
    bmfh.bfOffBits = (14 + 40 + 0 * 4);
    bmfh.bfSize = (bmfh.bfOffBits + bitmapLen);

    fwrite (&bmfh, 1, sizeof (bmfh), fp);
    gdip_bitmap_fill_info_header (bitmap, &bmi);
    bmi.biHeight = -bmi.biHeight;
    fwrite (&bmi, 1, sizeof (bmi), fp);
    fwrite (bitmap->data.Scan0, 1, bitmapLen, fp);
}

GpStatus 
gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image)
{
    return NotImplemented;
}
