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
gdip_tiff_read (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)((gdip_tiff_clientData *) clientData)->getBytesFunc (buffer, size, 0);
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
	((gdip_tiff_clientData *) clientData)->closeFunc ();
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
	GpBitmap *bitmap = (GpBitmap *) image;	
	unsigned char *buf = NULL;
	int i, linebytes;
	guint32 *r32 = NULL;
	size_t npixels;
					
	if (!tiff)
		return InvalidParameter;		
		
	TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, bitmap->data.Width);  
	TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, bitmap->data.Height); 
	TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, 4); /* Hardcoded 32bbps*/
	TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, 8);  
	TIFFSetField (tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);   
	TIFFSetField (tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); 
	TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	linebytes =  bitmap->data.Stride;     	
	
	if (TIFFScanlineSize (tiff) < linebytes) 
		buf =(unsigned char *)_TIFFmalloc (linebytes); 
	else 
		buf = (unsigned char *)_TIFFmalloc (TIFFScanlineSize (tiff)); 

	TIFFSetField (tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize (tiff, linebytes)); 

	/* We store data in ABGR format, but tiff stores it in ARGB, 
	 * so convert before writing it. 
	 */	 
	npixels = bitmap->image.width * bitmap->image.height;
	r32 = bitmap->data.Scan0;
	/* flip bytes */
	for (i = 0; i < npixels; i++) {
		*r32 = (*r32 & 0xff000000) | ((*r32 & 0x00ff0000) >> 16) |
				(*r32 & 0x0000ff00) | ((*r32 & 0x000000ff) << 16);
		r32++;
	}
	
	/*write data*/
	for (i = 0; i < bitmap->data.Height; i++) {
		if (TIFFWriteScanline (tiff, bitmap->data.Scan0 + i *bitmap->data.Stride, i, 0) < 0) 
			break;
	}
		
	/* image data might be reused again, so convert back to ABGR*/
	r32 = bitmap->data.Scan0;
	for (i = 0; i < npixels; i++) {
		*r32 = (*r32 & 0xff000000) | ((*r32 & 0x00ff0000) >> 16) |
				(*r32 & 0x0000ff00) | ((*r32 & 0x000000ff) << 16);
		r32++;
	}
	
	TIFFClose (tiff); 
		
	if (buf) 
		_TIFFfree (buf);

	return Ok;
}

GpStatus 
gdip_load_tiff_image (TIFF *tif, GpImage **image)
{
	GpBitmap *img = NULL;
	char *raster = NULL;

	if (tif) {
		TIFFRGBAImage tifimg;
		char emsg[1024];

		if (TIFFRGBAImageBegin (&tifimg, tif, 0, emsg)) {
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
			raster = GdipAlloc (npixels * sizeof (guint32));
			if (raster != NULL) {
				/* Problem: the raster data returned here has the origin at bottom left,
				* not top left.  The TIFF guys must be in cahoots with the OpenGL folks.
				*
				* Then, to add insult to injury, it's in ARGB format, not ABGR.
				*/
				if (TIFFRGBAImageGet (&tifimg, (uint32*) raster, tifimg.width, tifimg.height)) { 
					guchar *onerow = GdipAlloc (img->data.Stride);
					guint32 *r32 = (guint32*)raster;
					int i;
				
					/* flip raster */
					for (i = 0; i < tifimg.height / 2; i++) {
						memcpy (onerow, raster + (img->data.Stride * i), img->data.Stride);
						memcpy (raster + (img->data.Stride * i),
							raster + (img->data.Stride * (tifimg.height - i - 1)),
								img->data.Stride);
						memcpy (raster + (img->data.Stride * (tifimg.height - i - 1)),
								onerow, img->data.Stride);
					}
					/* flip bytes */

					for (i = 0; i < npixels; i++) {
						*r32 = (*r32 & 0xff000000) | ((*r32 & 0x00ff0000) >> 16) |
								(*r32 & 0x0000ff00) | ((*r32 & 0x000000ff) << 16);
						r32++;
					}
				
					img->data.Scan0 = raster;
					GdipFree (onerow);
					img->data.Reserved = GBD_OWN_SCAN0;
				
					img->image.surface = cairo_surface_create_for_image (raster,
											img->cairo_format,
											img->image.width,
											img->image.height,
											img->data.Stride);
					img->image.horizontalResolution = 0;
					img->image.verticalResolution = 0;
				
					img->image.imageFlags = ImageFlagsReadOnly |
								ImageFlagsHasRealPixelSize |
								ImageFlagsColorSpaceRGB;
				
					img->image.propItems = NULL;
					img->image.palette = NULL;
				}
			} else {
				goto error;
			}
		
			TIFFRGBAImageEnd (&tifimg);
		} else {
			goto error;
		}
	
		TIFFClose (tif);
	} else {
		goto error;
	}

	*image = (GpImage *) img;
	return Ok;

error:
	
	if (raster)
		GdipFree (raster);
		
	if (img)
		gdip_bitmap_dispose (img);
		
	if (tif)
		TIFFClose (tif);
		
	*image = NULL;
	return InvalidParameter;
}

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image)
{
	TIFF *tif = NULL;
	
	tif = TIFFFdOpen (fileno (fp), "lose.tif", "r");
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
	
	tif = TIFFClientOpen("lose.tif", "r", &clientData, gdip_tiff_read, 
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
	
	tiff = TIFFClientOpen("lose.tif", "w", &clientData, gdip_tiff_read, 
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
	return NotImplemented;
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
	return NotImplemented;
}

GpStatus 
gdip_save_tiff_image_to_file (FILE *fp, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return NotImplemented;
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
    return NotImplemented;
}

#endif
