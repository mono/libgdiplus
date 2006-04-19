/* 
 *
 * tiffcodec.c : Contains function definitions for encoding decoding tiff images
 *
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
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Jordi Mas (jordi@ximian.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>
#endif

#include "gdipImage.h"
#include "tiffcodec.h"

#ifdef HAVE_LIBTIFF

/* Thankfully, libtiff sucks far less than libjpeg */
#include <tiffio.h>

/* Codecinfo related data*/
static ImageCodecInfo tiff_codec;
static const WCHAR tiff_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'T', 'I', 'F', 'F',
        0}; /* Built-in TIFF */
static const WCHAR tiff_extension[] = {'*', '.', 'T', 'I', 'F',';', '*', '.', 'T', 'I', 'F','F', 0}; /* *.TIF;*.TIFF */
static const WCHAR tiff_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 't', 'i', 'f', 'f', 0}; /* image/gif */
static const WCHAR tiff_format[] = {'T', 'I', 'F', 'F', 0}; /* TIFF */

/*Wrapper functions and client data strucutre for delegate functions from StreamHelper class of 
System.Drawing Namespace*/
typedef struct {
	GetBytesDelegate getBytesFunc;
        PutBytesDelegate putBytesFunc;
	SeekDelegate seekFunc;
        CloseDelegate closeFunc;
        SizeDelegate sizeFunc;
} gdip_tiff_clientData;

tsize_t 
gdip_tiff_fileread (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)fread(buffer, 1, size, (FILE*)clientData);
}

tsize_t 
gdip_tiff_fileread_none (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return 0;
}

tsize_t 
gdip_tiff_filewrite (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)fwrite (buffer, 1, size, (FILE*)clientData);
}

toff_t 
gdip_tiff_fileseek (thandle_t clientData, toff_t offSet, int whence)
{
	int seek_ok = fseek ((FILE*)clientData, offSet, whence);
	if (seek_ok == 0)
		return offSet;

	return -1;
}

int 
gdip_tiff_fileclose (thandle_t clientData)
{
	/* This is a TIFF cleanup function; but we own the FILE* and close it in image.c so this is a null op */
	return 0;
}

toff_t 
gdip_tiff_filesize (thandle_t clientData)
{
	long cur_pos = ftell ((FILE*)clientData);
	fseek ((FILE*)clientData, 0, SEEK_END);
	long ret = ftell ((FILE*)clientData);
	fseek ((FILE*)clientData, cur_pos, SEEK_SET);
	return (toff_t)ret;
}

int
gdip_tiff_filedummy_map (thandle_t clientData, tdata_t *phase, toff_t* size)
{
	return 0;
}

void
gdip_tiff_filedummy_unmap (thandle_t clientData, tdata_t base, toff_t size)
{
}

tsize_t 
gdip_tiff_read (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)((gdip_tiff_clientData *) clientData)->getBytesFunc (buffer, size, 0);
}

tsize_t 
gdip_tiff_read_none (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return 0;
}

tsize_t 
gdip_tiff_write (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)((gdip_tiff_clientData *) clientData)->putBytesFunc (buffer, size);
}

toff_t 
gdip_tiff_seek (thandle_t clientData, toff_t offSet, int whence)
{
	return (toff_t)((gdip_tiff_clientData *) clientData)->seekFunc (offSet, whence);
}

int 
gdip_tiff_close (thandle_t clientData)
{
	/* We should not close the user provided streams */
	/* ((gdip_tiff_clientData *) clientData)->closeFunc (); */
	return 1;
}

toff_t 
gdip_tiff_size (thandle_t clientData)
{
	return (toff_t)((gdip_tiff_clientData *) clientData)->sizeFunc ();
}

int
gdip_tiff_dummy_map (thandle_t clientData, tdata_t *phase, toff_t* size)
{
	return 0;
}

void
gdip_tiff_dummy_unmap (thandle_t clientData, tdata_t base, toff_t size)
{
}

ImageCodecInfo *
gdip_getcodecinfo_tiff ()
{
	tiff_codec.Clsid = (CLSID) { 0x557cf405, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	tiff_codec.FormatID = gdip_tif_image_format_guid;
	tiff_codec.CodecName = (const WCHAR*) tiff_codecname;
	tiff_codec.DllName = NULL;
	tiff_codec.FormatDescription = (const WCHAR*) tiff_format;
	tiff_codec.FilenameExtension = (const WCHAR*) tiff_extension;
	tiff_codec.MimeType = (const WCHAR*) tiff_mimetype;
	tiff_codec.Flags = Encoder | Decoder | SupportBitmap | Builtin;
	tiff_codec.Version = 1;
	tiff_codec.SigCount = 0;
	tiff_codec.SigSize = 0;
	tiff_codec.SigPattern = 0;
	tiff_codec.SigMask = 0;
	
	return &tiff_codec;
}

/*TODO Handle TIFF Encoder Parameters*/
GpStatus 
gdip_save_tiff_image (TIFF* tiff, GpImage *image, GDIPCONST EncoderParameters *params)
{
	int		frame;
	int		x;
	int		y;
	int		i;
	int		num_of_pages;
	int		page;
	BitmapData	*bitmap_data;
	guchar		*pixbuf;

	if (tiff == NULL) {
		return InvalidParameter;
	}

	/* Count all pages, we need to know ahead */
	num_of_pages = 0;
	for (frame = 0; frame < image->num_of_frames; frame++) {
		num_of_pages += image->frames[frame].count;
		for (i = 0; i < image->frames[frame].count; i++) {
			if (gdip_is_an_indexed_pixelformat (image->frames[frame].bitmap[i].pixel_format)) {
				return NotImplemented; /* FIXME? */
			}
		}
	}

	page = 0;
	for (frame = 0; frame < image->num_of_frames; frame++) {
		for (i = 0; i < image->frames[frame].count; i++) {
			bitmap_data = &image->frames[frame].bitmap[i];

			if (num_of_pages > 1) {
				if ((frame > 0) && (i > 0)) {
					TIFFCreateDirectory(tiff);
				}

				TIFFSetField (tiff, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
				TIFFSetField (tiff, TIFFTAG_PAGENUMBER, page, num_of_pages);
			}

			TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, bitmap_data->width);  
			TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, bitmap_data->height); 
			TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, 8);  
			TIFFSetField (tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);  
			TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			TIFFSetField (tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);   
			TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, 4); /* FIXME - Hardcoded 32bbps*/
    			TIFFSetField (tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize (tiff, bitmap_data->stride)); 
			TIFFSetField (tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); 
			TIFFSetField (tiff, TIFFTAG_EXTRASAMPLES, EXTRASAMPLE_UNSPECIFIED);

printf("TIFFCODEC.C: NEED TO SAVE PROPERTIES AS WELL\n");

			pixbuf = GdipAlloc (bitmap_data->width * 4);
			for (y = 0; y < bitmap_data->height; y++) {
				for (x = 0; x < bitmap_data->width; x++) {
#ifdef WORDS_BIGENDIAN
					pixbuf[x*4+0] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 1);
					pixbuf[x*4+1] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 2);
					pixbuf[x*4+2] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 3);
					pixbuf[x*4+3] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 0);
#else
					pixbuf[x*4+0] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 2);
					pixbuf[x*4+1] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 1);
					pixbuf[x*4+2] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 0);
					pixbuf[x*4+3] = *((guchar *)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 3);
#endif
				}
				TIFFWriteScanline (tiff, pixbuf, y, 0);
			}
			GdipFree(pixbuf);
			TIFFWriteDirectory (tiff);
		}	
		page++;
	}
	TIFFClose (tiff); 		
	return Ok;
}

GpStatus 
gdip_load_tiff_image (TIFF *tiff, GpImage **image)
{
	int		i;
	char		error_message[1024];
	int		num_of_pages;
	GpImage		*result;
	int		page;
	TIFFRGBAImage	tiff_image;
	FrameData	*frame;
	BitmapData	*bitmap_data;
	size_t		num_of_pixels;
	char		*pixbuf;
	char		*pixbuf_row;
	guint32		*pixbuf_ptr;

	if (tiff == NULL) {
		*image = NULL;
		TIFFClose(tiff);
		return InvalidParameter;
	}

	pixbuf_row = NULL;
	pixbuf = NULL;

	num_of_pages = 0;
	do {
		num_of_pages++;
	} while (TIFFReadDirectory(tiff));

	result = gdip_bitmap_new();
	result->type = imageBitmap;
	frame = gdip_frame_add(result, &gdip_image_frameDimension_page_guid);

	for (page = 0; page < num_of_pages; page++) {
		bitmap_data = gdip_frame_add_bitmapdata(frame);
		if (bitmap_data == NULL) {
			goto error;
		}

		if (!TIFFSetDirectory(tiff, page)) {
			goto error;
		}

		if (!TIFFRGBAImageBegin(&tiff_image, tiff, 0, error_message)) {
			/* Can we use the error message somewhere? */
			goto error;
		}

		bitmap_data->stride = tiff_image.width * 4;
		bitmap_data->pixel_format = Format32bppArgb;
		bitmap_data->width = tiff_image.width;
		bitmap_data->height = tiff_image.height;
		bitmap_data->reserved = GBD_OWN_SCAN0;

		num_of_pixels = tiff_image.width * tiff_image.height;
		pixbuf = GdipAlloc(num_of_pixels * sizeof(guint32));
		if (pixbuf == NULL) {
			goto error;
		}

		/* Flip the image. TIFF has its origin at bottom left, and is in ARGB instead of ABGR */
		if (!TIFFRGBAImageGet(&tiff_image, (uint32 *)pixbuf, tiff_image.width, tiff_image.height)) {
			goto error;
		}

		pixbuf_row = GdipAlloc(bitmap_data->stride);
		if (pixbuf_row == NULL) {
			goto error;
		}

		/* First, flip rows */
		for (i = 0; i < tiff_image.height / 2; i++) {
			memcpy(pixbuf_row, pixbuf + (bitmap_data->stride * i), bitmap_data->stride);
			memcpy(pixbuf + (bitmap_data->stride * i), pixbuf + (bitmap_data->stride * (tiff_image.height - i - 1)), bitmap_data->stride);
			memcpy(pixbuf + (bitmap_data->stride * (tiff_image.height - i - 1)), pixbuf_row, bitmap_data->stride);
		}

		/* Now flip from ARGB to ABGR */
		pixbuf_ptr = (guint32 *)pixbuf;
		for (i = 0; i < num_of_pixels; i++) {
			*pixbuf_ptr =	(*pixbuf_ptr & 0xff000000) | 
					((*pixbuf_ptr & 0x00ff0000) >> 16) |
					(*pixbuf_ptr & 0x0000ff00) | 
					((*pixbuf_ptr & 0x000000ff) << 16);
			pixbuf_ptr++;
		}
		GdipFree(pixbuf_row);
		bitmap_data->scan0 = (byte *)pixbuf;
		pixbuf_row = NULL;
		pixbuf = NULL;
	}

	gdip_bitmap_setactive(result, &gdip_image_frameDimension_page_guid, 0);

	TIFFClose(tiff);

	*image = result;
	return Ok;

error:
	if (pixbuf_row != NULL) {
		GdipFree(pixbuf_row);
	}

	if (pixbuf != NULL) {
		GdipFree(pixbuf);
	}

	if (result != NULL) {
		gdip_bitmap_dispose(result);
	}

	TIFFClose(tiff);

	return OutOfMemory;
}

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image)
{
	TIFF *tif = NULL;
	
	tif = TIFFClientOpen("lose.tif", "r", (thandle_t) fp, gdip_tiff_fileread, 
				gdip_tiff_filewrite, gdip_tiff_fileseek, gdip_tiff_fileclose, 
				gdip_tiff_filesize, gdip_tiff_filedummy_map, gdip_tiff_filedummy_unmap);
	return gdip_load_tiff_image (tif, image);
}

GpStatus 
gdip_save_tiff_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{	
	TIFF* tiff;
	
	tiff = TIFFFdOpen (fileno (fp), "lose.tif", "w");

	if (!tiff)
		return FileNotFound;		
		
	return gdip_save_tiff_image (tiff, image, params);
}

GpStatus
gdip_load_tiff_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           PutBytesDelegate putBytesFunc,
                                           SeekDelegate seekFunc,
					   CloseDelegate closeFunc,
					   SizeDelegate sizeFunc,
                                           GpImage **image)
{
	TIFF *tif = NULL;
	gdip_tiff_clientData clientData;
	
	clientData.getBytesFunc = getBytesFunc;
	clientData.putBytesFunc = putBytesFunc;
	clientData.seekFunc = seekFunc;
	clientData.closeFunc = closeFunc;
	clientData.sizeFunc = sizeFunc;
	
	tif = TIFFClientOpen("lose.tif", "r", (thandle_t) &clientData, gdip_tiff_read, 
				gdip_tiff_write, gdip_tiff_seek, gdip_tiff_close, 
				gdip_tiff_size, gdip_tiff_dummy_map, gdip_tiff_dummy_unmap);
	
	return gdip_load_tiff_image (tif, image);
}

GpStatus
gdip_save_tiff_image_to_stream_delegate (GetBytesDelegate getBytesFunc,
					 PutBytesDelegate putBytesFunc,
                                         SeekDelegate seekFunc,
					 CloseDelegate closeFunc,
					 SizeDelegate sizeFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)
{
	TIFF* tiff;
	gdip_tiff_clientData clientData;
	
	clientData.getBytesFunc = getBytesFunc;
	clientData.putBytesFunc = putBytesFunc;
	clientData.seekFunc = seekFunc;
	clientData.closeFunc = closeFunc;
	clientData.sizeFunc = sizeFunc;
	
	tiff = TIFFClientOpen("lose.tif", "w", (thandle_t) &clientData, gdip_tiff_read_none, 
			gdip_tiff_write, gdip_tiff_seek, gdip_tiff_close, 
			gdip_tiff_size, gdip_tiff_dummy_map, gdip_tiff_dummy_unmap);
	if (!tiff)
		return InvalidParameter;		
		
	return gdip_save_tiff_image (tiff, image, params);
}

#else

/* no libtiff */

ImageCodecInfo *
gdip_getcodecinfo_tiff ()
{
	return NULL;
}

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

GpStatus
gdip_load_tiff_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           PutBytesDelegate putBytesFunc,
                                           SeekDelegate seekFunc,
					   CloseDelegate closeFunc,
					   SizeDelegate sizeFunc,
                                           GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

GpStatus 
gdip_save_tiff_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return UnknownImageFormat;
}

GpStatus
gdip_save_tiff_image_to_stream_delegate (GetBytesDelegate getBytesFunc,
					 PutBytesDelegate putBytesFunc,
                                         SeekDelegate seekFunc,
					 CloseDelegate closeFunc,
					 SizeDelegate sizeFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)
{
    return UnknownImageFormat;
}

#endif
