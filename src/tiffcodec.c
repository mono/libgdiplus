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
	// This is a TIFF cleanup function; but we own the FILE* and close it in image.c so this is a null op
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
	GpBitmap *bitmap = (GpBitmap *) image;	
	int i, j, k, l, linebytes;
	guint32 *r32 = NULL;
	size_t npixels;
	int dimensionCount = 0;
	int frameCount = 0;
	BitmapData data;
	int currPage = 0; /*First page wud already have been set */
	int totalPages = 0;
					
	if (!tiff)
		return InvalidParameter;		

	dimensionCount = image->frameDimensionCount; 
	for (j = 0; j < dimensionCount; j++)
		totalPages += image->frameDimensionList [j].count;
	
	for (j = 0; j < dimensionCount; j++) {
                frameCount = image->frameDimensionList [j].count;
                for (k = 0; k < frameCount; k++) {
			if (k > 0 || j > 0) {
				TIFFCreateDirectory (tiff);
			}
			
			if (totalPages > 1) {
				TIFFSetField (tiff, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
				TIFFSetField (tiff, TIFFTAG_PAGENUMBER, currPage++, totalPages);
			}
			data = image->frameDimensionList [j].frames [k];
			TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, data.Width);  
			TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, data.Height); 
			TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, 8);  
			TIFFSetField (tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);  
			TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			TIFFSetField (tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);   
			TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, 4); /* Hardcoded 32bbps*/

			linebytes =  data.Stride;     	
    			TIFFSetField (tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize (tiff, linebytes)); 

			TIFFSetField (tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG); 
			TIFFSetField (tiff, TIFFTAG_EXTRASAMPLES, EXTRASAMPLE_UNSPECIFIED);
				
			{
				guchar *row_pointer = GdipAlloc (image->width * 4);

				for (i = 0; i < image->height; i++) {
					for (l = 0; l < image->width; l++) {
#ifdef WORDS_BIGENDIAN
						row_pointer[l*4+0] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 1);
						row_pointer[l*4+1] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 2);
						row_pointer[l*4+2] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 3);
						row_pointer[l*4+3] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 0);
#else
						row_pointer[l*4+0] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 2);
						row_pointer[l*4+1] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 1);
						row_pointer[l*4+2] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 0);
						row_pointer[l*4+3] = *((guchar *)data.Scan0 + (data.Stride * i) + (l*4) + 3);
#endif
					}
					TIFFWriteScanline (tiff, row_pointer, i, 0);
				}
				GdipFree (row_pointer);
			}
			TIFFWriteDirectory (tiff);			
		}
	}
				
	TIFFClose (tiff); 		
	return Ok;
}

GpStatus 
gdip_load_tiff_image (TIFF *tif, GpImage **image)
{
	GpBitmap *img = NULL;
	char *raster = NULL;
	int dirCount = 0;
	int j=0;
	BitmapData data;

	data.ByteCount = data.Top = data.Left = 0;
	data.Bytes = NULL;

	if (tif) {

		TIFFRGBAImage tifimg;
		char emsg[1024];

		/*Count the no of image frames present in input*/
		do {
			dirCount++;
		} while (TIFFReadDirectory (tif));
		
		img = gdip_bitmap_new ();
		img->image.type = imageBitmap;
		/* libtiff expands stuff out to ARGB32 for us if we use this interface */
		img->image.pixFormat = Format32bppArgb;
		img->cairo_format = CAIRO_FORMAT_ARGB32;

		img->image.frameDimensionCount = 1;
		img->image.frameDimensionList = (FrameInfo *) GdipAlloc (sizeof (FrameInfo));
		img->image.frameDimensionList [0].count = dirCount;
		memcpy (&(img->image.frameDimensionList [0].frameDimension), 
				&gdip_image_frameDimension_page_guid, sizeof (CLSID));
		img->image.frameDimensionList [0].frames = (BitmapData *)
						GdipAlloc (sizeof (BitmapData) * dirCount);

		/*loop within all the directories and extract the frame info*/
		for (j = 0; j < dirCount; j++) {
			if (!TIFFSetDirectory (tif, j))
				goto error;
			
			if (TIFFRGBAImageBegin (&tifimg, tif, 0, emsg)) {
				size_t npixels;

				data.Stride = tifimg.width * 4;
				data.PixelFormat = img->image.pixFormat;
				data.Width = tifimg.width;
				data.Height = tifimg.height;
				data.Reserved = GBD_OWN_SCAN0;
			
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
						guchar *onerow = GdipAlloc (data.Stride);
						guint32 *r32 = (guint32*)raster;
						int i;
				
						/* flip raster */
						for (i = 0; i < tifimg.height / 2; i++) {
							memcpy (onerow, raster + (data.Stride * i), data.Stride);
							memcpy (raster + (data.Stride * i), 
									raster + (data.Stride * (tifimg.height - i - 1)),
													data.Stride);
							memcpy (raster + (data.Stride * (tifimg.height - i - 1)),
												onerow, data.Stride);
						}
						/* flip bytes */

						for (i = 0; i < npixels; i++) {
							*r32 = (*r32 & 0xff000000) | ((*r32 & 0x00ff0000) >> 16) |
									(*r32 & 0x0000ff00) | ((*r32 & 0x000000ff) << 16);
							r32++;
						}	
						
						GdipFree (onerow);

						data.Scan0 = (byte *)raster;
						img->image.frameDimensionList [0].frames [j] = data;
					} else {
						goto error;
					} /*end if (TIFFRGBAImageGet */					
				} else {
					goto error;
				} /*end if (raster != NULL) */
				
				TIFFRGBAImageEnd (&tifimg);				
			} else {
				goto error;
			} /*end if (TIFFRGBAImageBegin())*/

		} /*End For Loop for reading directories */
		
		TIFFClose (tif);
	
		img->data = img->image.frameDimensionList [0].frames [0];
		img->image.width = img->data.Width;
		img->image.height = img->data.Height;
		img->image.surface = cairo_surface_create_for_image ((char *)img->data.Scan0, 
						img->cairo_format, img->image.width, 
						img->image.height, img->data.Stride);
		img->image.horizontalResolution = 0;
		img->image.verticalResolution = 0;
			
		img->image.imageFlags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | 
								ImageFlagsColorSpaceRGB;
				
		img->image.propItems = NULL;
		img->image.palette = NULL;

		*image = (GpImage *) img;
		return Ok;
	} else {
		goto error;
	} /*end if (tif) */

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
