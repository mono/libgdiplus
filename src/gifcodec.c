/* 
 * gifcodec.c : Contains function definitions for encoding decoding gif images
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
 *	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Jordi Mas (jordi@ximian.com)
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

/* giflib declares this incorrectly as EgifOpen */
extern GifFileType *EGifOpen(void *userData, OutputFunc writeFunc);

/* Data structure used for callback */
typedef struct
{
	GetBytesDelegate getBytesFunc;
	SeekDelegate seekFunc;
	
} gif_callback_data;

/* Codecinfo related data*/
static ImageCodecInfo gif_codec;
static const WCHAR gif_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'G', 'I', 'F', ' ', 'C', 
        'o', 'd', 'e', 'c',   0}; /* Built-in GIF Codec */
static const WCHAR gif_extension[] = {'*', '.', 'G', 'I', 'F',0}; /* *.GIF */
static const WCHAR gif_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'g', 'i', 'f', 0}; /* image/gif */
static const WCHAR gif_format[] = {'G', 'I', 'F', 0}; /* GIF */

GpStatus  gdip_load_gif_image (void *stream, GpImage **image, bool from_file);
GpStatus  gdip_save_gif_image (void *stream, GpImage *image, bool from_file);

ImageCodecInfo *
gdip_getcodecinfo_gif ()
{                        
	gif_codec.Clsid = (CLSID) { 0x557cf402, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	gif_codec.FormatID = gdip_gif_image_format_guid;
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

/* Read callback function for the gif libbrary*/
static int 
gdip_gif_inputfunc (GifFileType *gif, GifByteType *data, int len) 
{
	int read = 0;	
	gif_callback_data *gcd = (gif_callback_data*) gif->UserData;
	
	read = gcd->getBytesFunc (data, len, 0);
	return read;
}

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_load_gif_image (fp, image, TRUE);
}

GpStatus
gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           SeekDelegate seekFunc,
                                           GpImage **image)
{
	gif_callback_data gif_data;
	
	gif_data.getBytesFunc = getBytesFunc;
	gif_data.seekFunc = seekFunc;
	
	return gdip_load_gif_image (&gif_data, image, FALSE);	
}

GpStatus 
gdip_load_gif_image (void *stream, GpImage **image, bool from_file)
{
	GifFileType *gif = NULL;
	GifRecordType *rectype = NULL;
	int res;
	
	GpBitmap *img = NULL;
	ColorMapObject *pal = NULL;
	ColorMapObject *localPalObj = NULL;
	ColorPalette *localPal = NULL;
	guchar *pixels = NULL;
	guchar *readptr, *writeptr;
	int i, j, l, pixelLength;
	int imgCount;
	int extBlockCount;
	bool pageDimensionFound = FALSE;
	bool timeDimensionFound = FALSE;
	int pageDimensionCount = 0;
	int timeDimensionCount = 0;
	bool timeDimension = FALSE;
	int timeCounter = 0;
	int pageCounter = 0;
	BitmapData data;
	GifImageDesc imgDesc;

	if (from_file) 
		gif = DGifOpenFileHandle (fileno ((FILE*)stream));
	else 		
		gif = DGifOpen (stream, &gdip_gif_inputfunc);
	
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
	 * present in the image. 
	 * I am assuming that if there is an ExtensionBlock strucutre present in SavedImage structure 
	 * which has its Function member set to value 249 (Graphic Control Block) 
	 * then only its an animated gif.
	 */
	imgCount = gif->ImageCount;
	for (i=0; i<imgCount; i++) {
		SavedImage si = gif->SavedImages[i];
		extBlockCount = si.ExtensionBlockCount;
		for (l =0; l<extBlockCount; l++) {
			ExtensionBlock eb = si.ExtensionBlocks[l];
			if (eb.Function == 249){
				if (!timeDimensionFound)
					timeDimensionFound = TRUE;
				timeDimensionCount++;
				break; /*there can be only one Graphic Control Extension before ImageData*/
			}			
		}
	}
	
	if (timeDimensionCount < imgCount){
		pageDimensionFound = TRUE;
		pageDimensionCount = imgCount - timeDimensionCount;
	}
		
	if (pageDimensionFound && timeDimensionFound){
		img->image.frameDimensionCount = 2;
		img->image.frameDimensionList = (FrameInfo *) GdipAlloc (sizeof (FrameInfo)*2);
		img->image.frameDimensionList[0].count = pageDimensionCount;
		memcpy (&(img->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
		img->image.frameDimensionList[0].frames = (BitmapData *) GdipAlloc (sizeof (BitmapData)*pageDimensionCount);
		img->image.frameDimensionList[1].count = timeDimensionCount;
		memcpy (&(img->image.frameDimensionList[1].frameDimension), &gdip_image_frameDimension_time_guid, sizeof (CLSID));
		img->image.frameDimensionList[1].frames = (BitmapData *) GdipAlloc (sizeof (BitmapData)*timeDimensionCount);
	} else if (pageDimensionFound) {
		img->image.frameDimensionCount = 1;
		img->image.frameDimensionList = (FrameInfo *) GdipAlloc (sizeof (FrameInfo));
		img->image.frameDimensionList[0].count = pageDimensionCount;
		memcpy (&(img->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_page_guid, sizeof (CLSID));
		img->image.frameDimensionList[0].frames = (BitmapData *) GdipAlloc (sizeof (BitmapData)*pageDimensionCount);
	} else if (timeDimensionFound) {
		img->image.frameDimensionCount = 1;
		img->image.frameDimensionList = (FrameInfo *) GdipAlloc (sizeof (FrameInfo));
		img->image.frameDimensionList[0].count = timeDimensionCount;
		memcpy (&(img->image.frameDimensionList[0].frameDimension), &gdip_image_frameDimension_time_guid, sizeof (CLSID));
		img->image.frameDimensionList[0].frames = (BitmapData *) GdipAlloc (sizeof (BitmapData)*timeDimensionCount);
	}

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

	/* 
	 * Now populate the frames associated with each frame dimension
	 */
	imgCount = gif->ImageCount;
	for (i=0; i<imgCount; i++) {
		SavedImage si = gif->SavedImages[i];
		extBlockCount = si.ExtensionBlockCount;
		for (l =0; l<extBlockCount; l++) {
			ExtensionBlock eb = si.ExtensionBlocks[l];
			if (eb.Function == 249){
				timeDimension = TRUE;
				break; /*there can be only one Graphic Control Extension before ImageData*/
			}			
		}
		
		/* copy the local color map if there is one*/
		localPalObj= si.ImageDesc.ColorMap;
		if (localPalObj != NULL) {
			localPal = g_malloc (sizeof(ColorPalette) + sizeof(ARGB) * localPalObj->ColorCount);
			localPal->Flags = 0;
			localPal->Count = localPalObj->ColorCount;
			for (j = 0; j < localPalObj->ColorCount; j++) {
				localPal->Entries[j] = MAKE_ARGB_RGB(localPalObj->Colors[j].Red,
								localPalObj->Colors[j].Green,
								localPalObj->Colors[j].Blue);
			}
		}

                data.PixelFormat = img->image.pixFormat;
		imgDesc = si.ImageDesc;
		data.Width = imgDesc.Width;
		data.Height = imgDesc.Height;
		data.Stride = data.Width * 4;
	
		pixels = GdipAlloc (data.Stride * data.Height);
	
		readptr = si.RasterBits;
		writeptr = pixels;
		pixelLength = data.Width * data.Height;
		/*
		 * While loading images, local color map if present takes precedence
		 * over global color map.
		 */
		for (j = 0; j < pixelLength; j++) {
			guchar pix = *readptr++;
			if (localPalObj) {
				*writeptr++ = localPalObj->Colors[pix].Blue;
				*writeptr++ = localPalObj->Colors[pix].Green;
				*writeptr++ = localPalObj->Colors[pix].Red;
			} else if (pal) {
				*writeptr++ = pal->Colors[pix].Blue;
				*writeptr++ = pal->Colors[pix].Green;
				*writeptr++ = pal->Colors[pix].Red;
			} else {
				*writeptr++ = pix;
				*writeptr++ = pix;
				*writeptr++ = pix;
			}
			*writeptr++ = 255; /* A */
		}

		data.Scan0 = pixels;
		data.Reserved = GBD_OWN_SCAN0;

		if (pageDimensionFound && timeDimensionFound){
			if (timeDimension)
				img->image.frameDimensionList [1].frames [timeCounter ++] = data;
			else
				img->image.frameDimensionList [0].frames [pageCounter ++] = data;
		} else if (pageDimensionFound){
			img->image.frameDimensionList[0].frames [pageCounter++] = data;
		} else if (timeDimensionFound){
			img->image.frameDimensionList[0].frames [timeCounter++] = data;
		}
		
		/*printf("\n gifcodec.c pageCounter is %d", pageCounter);
		printf("\n gifcodec.c timeCounter is %d", timeCounter);*/
	}
	
	img->data = img->image.frameDimensionList[0].frames[0];
	
	img->image.surface = cairo_surface_create_for_image (img->data.Scan0, img->cairo_format,
							img->image.width, img->image.height,
							img->data.Stride);
	img->image.imageFlags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | ImageFlagsColorSpaceRGB;
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

/* Write callback function for the gif libbrary*/
static int 
gdip_gif_outputfunc (GifFileType *gif,  const GifByteType *data, int len) 
{
	int written = 0;	
	PutBytesDelegate putBytesFunc = (PutBytesDelegate) gif->UserData;
	
	written = putBytesFunc ((void *)data, len);
	return written;
}


GpStatus 
gdip_save_gif_image_to_file (unsigned char *filename, GpImage *image)
{
	return gdip_save_gif_image ( (void *)filename, image, TRUE);
}

GpStatus
gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)
{
	return gdip_save_gif_image ( (void *)putBytesFunc, image, FALSE);
}

GpStatus 
gdip_save_gif_image (void *stream, GpImage *image, bool from_file)
{
	GifFileType *fp;
	int i, row, x, y, size;
	ColorMapObject *color_map;
	GpBitmap *bitmap = (GpBitmap *) image;
	GifByteType *red, *green, *blue, *pixels;
	GifByteType *ptr_red, *ptr_green, *ptr_blue, *ptr_pixels;
	ARGB color;	
	int cmap_size = 256;	
	ColorMapObject* cmap = NULL;	
	BOOL error = FALSE;
	
	if (!stream) 
		return InvalidParameter;
		
	if (from_file)
		fp = EGifOpenFileName ((unsigned char *) stream, 0);
	else
		fp = EGifOpen (stream, gdip_gif_outputfunc);
		
	if (!fp)
		return FileNotFound;
		
	size = bitmap->data.Height * bitmap->data.Width;
	ptr_red = red = malloc (sizeof (GifByteType)* size);
	ptr_green = green = malloc (sizeof (GifByteType)* size);
	ptr_blue = blue = malloc (sizeof (GifByteType)* size);
	ptr_pixels = pixels = malloc (sizeof (GifByteType)* bitmap->data.Height * bitmap->data.Stride);
	cmap = MakeMapObject (cmap_size, 0);	
	
	for (y = 0; y <bitmap->data.Height; y++) {	
		for (x = 0; x <bitmap->data.Width; x++) {
		
			GdipBitmapGetPixel (bitmap, x, y, &color);		
			
			*ptr_red++ = (color & 0x00ff0000) >> 16;
			*ptr_green++ = (color & 0x0000ff00) >> 8;
			*ptr_blue++ =  (color & 0x000000ff);
		}	
	}	

	if (QuantizeBuffer (bitmap->data.Width, bitmap->data.Height, &cmap_size,
		red, green, blue, pixels, cmap->Colors) == GIF_ERROR) 
		error = TRUE;
	
	if (EGifPutScreenDesc (fp, bitmap->data.Width, bitmap->data.Height,
		8, 0, cmap) == GIF_ERROR) 
		error = TRUE;
	
	if (EGifPutImageDesc (fp, 0, 0, bitmap->data.Width, bitmap->data.Height,
		FALSE, NULL) == GIF_ERROR) 
		error = TRUE;
	
	for (i = 0;  i < bitmap->data.Height;  ++i) {
		if (EGifPutLine (fp, ptr_pixels, bitmap->data.Width) == GIF_ERROR) {
			error = TRUE;
			break;
		}
		ptr_pixels += bitmap->data.Width;
	}

	EGifCloseFile (fp);	
	free (red);
	free (green);
	free (blue);	
	free (pixels);		
	free (cmap);

	return (error == FALSE) ? Ok : GenericError;
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
gdip_save_gif_image_to_file (unsigned char *filename, GpImage *image)
{
	return NotImplemented;
}

GpStatus
gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)
{
	return NotImplemented;
}

GpStatus
gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           SeekDelegate seekFunc,
                                           GpImage **image)
{
	*image = NULL;
	return NotImplemented;
}

#endif
