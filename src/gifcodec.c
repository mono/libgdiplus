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
#include "gdipImage.h"
#include "gifcodec.h"

#ifdef HAVE_LIBUNGIF

#include <gif_lib.h>


/* Codecinfo related data*/
static ImageCodecInfo gif_codec;
static const WCHAR gif_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'G', 'I', 'F', ' ', 'C', 
        'o', 'd', 'e', 'c',   0}; /* Built-in GIF Codec */
static const WCHAR gif_extension[] = {'*', '.', 'G', 'I', 'F',0}; /* *.GIF */
static const WCHAR gif_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'g', 'i', 'f', 0}; /* image/gif */
static const WCHAR gif_format[] = {'G', 'I', 'F', 0}; /* GIF */

ImageCodecInfo *
gdip_getcodecinfo_gif ()
{                        
        gif_codec.Clsid = (CLSID) { 0x557cf402, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
        gif_codec.FormatID = (CLSID) { 0xb96b3cb0, 0x0728, 0x11d3, { 0x9d, 0x7b, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
        gif_codec.CodecName = (const WCHAR*) gif_codecname;
        gif_codec.DllName = NULL;
        gif_codec.FormatDescription = (const WCHAR*) gif_format;
        gif_codec.FilenameExtension = (const WCHAR*) gif_extension;
        gif_codec.MimeType = (const WCHAR*) gif_mimetype;
        gif_codec.Flags = Encoder | Decoder | SupportBitmap | Builtin;
        gif_codec.Version = 1;
        gif_codec.SigCount = 0;
        gif_codec.SigSize = 0;
        gif_codec.SigPattern = 0;
        gif_codec.SigMask = 0;

        return &gif_codec;
}

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
    GifFileType *gif = NULL;
    GifRecordType *rectype = NULL;
    int res;

    GpBitmap *img = NULL;
    ColorMapObject *pal = NULL;
    guchar *pixels = NULL;
    guchar *readptr, *writeptr;
    int i, j;
/*	bool pageDimensionFound = FALSE;
	bool timeDimensionFound = FALSE;
	int pageDimensionCount = 0;
	int timeDimensionCount = 0;*/

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

	/* ImageCount member of GifFileType structure gives info about the total no of frames
	 * present in the image. Presently i m just attaching all the images as various pages
	 * of an image, but need to find about the way to discover whether some images in 
	 * SavedImage [] member of GifFileType structure belongs to frames of animated image.
	 * My understanding is both kind of frames can be present. 
	 * TODO : Read the extension block and determine whether its an animated gif
	 */

	img->image.frameDimensionCount = 1;
	img->image.frameDimensionList = (FrameDimensionInfo *) GdipAlloc (sizeof (FrameDimensionInfo));
	img->image.frameDimensionList[0].count = gif->ImageCount;
	memcpy (&(img->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
		
	
	/*if (pageDimensionFound && timeDimensionFound){
		img->image.frameDimensionList = (FrameDimensionInfo *) GdipAlloc (sizeof (FrameDimensionInfo)*2);
		img->image.frameDimensionList[0].count = pageDimensionCount;
		memcpy (&(img->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
		img->image.frameDimensionList[1].count = timeDimensionCount;
		memcpy (&(img->image.frameDimensionList[1].frameDimension), &gdip_image_frameDimension_time_guid, sizeof (CLSID));
	} else if (pageDimensionFound) {
		img->image.frameDimensionList = (FrameDimensionInfo *) GdipAlloc (sizeof (FrameDimensionInfo));
		img->image.frameDimensionList[0].count = pageDimensionCount;
		memcpy (&(img->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
	} else if (timeDimensionFound) {
		img->image.frameDimensionList = (FrameDimensionInfo *) GdipAlloc (sizeof (FrameDimensionInfo));
		img->image.frameDimensionList[0].count = timeDimensionCount;
		memcpy (&(img->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_time_guid, sizeof (CLSID));
	}*/

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

    img->image.pixFormat = Format32bppArgb;

    img->cairo_format = CAIRO_FORMAT_ARGB32;
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

ImageCodecInfo *
gdip_getcodecinfo_gif ()
{
        return NULL;
}

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
