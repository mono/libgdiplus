/*
 * Copyright (c) 2004 Ximian
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
 * bmpcodec.c : Contains function definitions for encoding decoding bmp images
 *
 * Authors:
 *      Jordi Mas i Hernandez (jordi@ximian.com)
 *      Sanjay Gupta (gsanjay@novell.com)
 *      Mark Steele (ms@rapidsys.com)
 *
 * Useful documentation about bitmaps
 *
 *      http://msdn.microsoft.com/library/default.asp?url=/library/en-us/gdi/bitmaps_4v1h.asp
 *      http://www.csdn.net/Dev/Format/windows/Bmp.html
 *      http://www.fortunecity.com/skyscraper/windows/364/bmpffrmt.html
 *
 *      Header structure
 *              BITMAPFILEHEADER
 *              BITMAPINFOHEADER or BITMAPV4HEADER or BITMAPV5HEADER or BITMAPCOREHEADER
 *              RGBQUADS or RGBTRIPLE (optional)
 *              Bitmap data
 */

#include <stdio.h>
#include "bmpcodec.h"
#include "gdip.h"


/* Codecinfo related data*/
static ImageCodecInfo bmp_codec;
static const WCHAR bmp_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'B', 'M', 'P', 0}; /* Built-in BMP */
static const WCHAR bmp_extension[] = {'*','.','B', 'M', 'P',';', '*','.', 'D','I', 'B',';', '*','.', 'R', 'L', 'E',0}; /* *.BMP;*.DIB;*.RLE */
static const WCHAR bmp_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'b', 'm', 'p', 0}; /* image/bmp */
static const WCHAR bmp_format[] = {'B', 'M', 'P', 0}; /* BMP */


ImageCodecInfo *
gdip_getcodecinfo_bmp ()
{
        bmp_codec.Clsid = (CLSID) { 0x557cf400, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
        bmp_codec.FormatID = (CLSID) { 0xb96b3cab, 0x0728, 0x11d3, { 0x9d, 0x7b, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
        bmp_codec.CodecName = (const WCHAR*) bmp_codecname;            
        bmp_codec.DllName = NULL;
        bmp_codec.FormatDescription = (const WCHAR*) bmp_format;
        bmp_codec.FilenameExtension = (const WCHAR*) bmp_extension;
        bmp_codec.MimeType = (const WCHAR*) bmp_mimetype;
        bmp_codec.Flags = Encoder | Decoder | SupportBitmap | Builtin;
        bmp_codec.Version = 1;
        bmp_codec.SigCount = 0;
        bmp_codec.SigSize = 0;
        bmp_codec.SigPattern = 0;
        bmp_codec.SigMask = 0;

        return &bmp_codec; 
}

int
gdip_get_pixelformat (WORD bitcount)
{
        switch (bitcount) {
        case 32:
                return Format32bppArgb;
        case 24:
                return Format24bppRgb;
        case 8:
                return Format8bppIndexed;
        case 4:
                return Format4bppIndexed;
        default:
                return 0;
        }            
}


void 
gdip_bitmap_fill_info_header (GpBitmap *bitmap, PBITMAPINFOHEADER bmi)
{
	int  bitmapLen = bitmap->data.Stride * bitmap->data.Height;
	memset (bmi, 0, sizeof (BITMAPINFOHEADER));
	bmi->biSize = sizeof (BITMAPINFOHEADER);
	bmi->biWidth = bitmap->data.Width;
	bmi->biHeight = bitmap->data.Height;
	bmi->biPlanes = 1;
	bmi->biBitCount = 32; /*PIXEL_FORMAT_BPP (bitmap->image.pixFormat); */
	bmi->biCompression = BI_RGB;
	bmi->biSizeImage =  0; /* Many tools expect this may be set to zero for BI_RGB bitmaps */
	bmi->biXPelsPerMeter = (int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100));
	bmi->biYPelsPerMeter = (int) (0.5f + ((gdip_get_display_dpi() * 3937) / 100)); /* 1 meter is = 39.37 */       
}                                                           

void 
gdip_bitmap_save_bmp (const char *name, GpBitmap *bitmap)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmi;
	int  bitmapLen = bitmap->data.Stride * bitmap->data.Height;
	FILE *fp;
	
	bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
	bmfh.bfType = BFT_BITMAP;
	bmfh.bfOffBits = (14 + 40 + 0 * 4);
	bmfh.bfSize = (bmfh.bfOffBits + bitmapLen);
	fp = fopen (name, "w+b");
	fwrite (&bmfh, 1, sizeof (bmfh), fp);
	gdip_bitmap_fill_info_header (bitmap, &bmi);
	bmi.biHeight = -bmi.biHeight;
	fwrite (&bmi, 1, sizeof (bmi), fp);
	fwrite (bitmap->data.Scan0, 1, bitmapLen, fp);
	fclose (fp);
}

GpStatus 
gdip_load_bmp_image_from_file (FILE *fp, GpImage **image)
{
        BITMAPFILEHEADER bmfh;
        BITMAPINFOHEADER bmi;
        GpBitmap *img = NULL;
        guchar *pixels = NULL, *linep = NULL;
        int i, j, format, colours;
        BOOL os2format = FALSE, upsidedown = TRUE;
        byte b,g,r,a;

        memset (&bmi, 0, sizeof(bmi));
        fread(&bmfh, sizeof(bmfh), 1, fp);
        
        if (bmfh.bfType != BFT_BITMAP)
                return UnknownImageFormat;

        fread(&bmi.biSize, sizeof (DWORD), 1, fp);

        if (bmi.biSize >= sizeof (bmi))   /* New Windows headers can be bigger */     
                fread (&bmi.biWidth, sizeof (bmi) - sizeof (DWORD), 1, fp);
        else  {

                if (bmi.biSize ==  BITMAPCOREHEADER_SIZE) {

                        /* Old OS/2 format. Some fields are WORDs instead of DWORDS */
                        fread (&bmi.biWidth, sizeof (WORD), 1, fp);
                        fread (&bmi.biHeight, sizeof (WORD), 1, fp);
                        fread (&bmi.biPlanes, sizeof (WORD), 1, fp);
                        fread (&bmi.biBitCount, sizeof (WORD), 1, fp);
                        os2format = TRUE;
                }
                else
                        return UnknownImageFormat;
        }

        if (bmi.biCompression == BI_RLE4 || bmi.biCompression == BI_RLE8)
                return NotImplemented; /* We do not support RLE for now*/

        if (bmi.biHeight < 0) { /* Negative height indicates that the bitmap is sideup*/
                upsidedown = FALSE;
                bmi.biHeight =  -bmi.biHeight;
        }

        colours =  (bmi.biClrUsed == 0 && bmi.biBitCount <= 8) ? (1 << bmi.biBitCount) : bmi.biClrUsed;
        
        format = gdip_get_pixelformat (bmi.biBitCount);
                                               
        if (format == 0)
                return NotImplemented; /* Unsuported pixel format*/

        img = gdip_bitmap_new ();
        img->image.pixFormat = format;
        img->image.type = imageBitmap;
        img->image.graphics = 0;
        img->image.width = bmi.biWidth;
        img->image.height = bmi.biHeight;

        img->data.PixelFormat = img->image.pixFormat;
        img->data.Width = img->image.width;
        img->data.Height = img->image.height;
        img->data.Stride = (bmi.biBitCount * img->image.width) / 8;
        img->data.Stride = (img->data.Stride + 3) & ~3;
     
        if (colours){
                
                img->image.palette = g_malloc (sizeof(ColorPalette) + sizeof(ARGB) * colours);
                img->image.palette->Flags = 0;
                img->image.palette->Count = colours;
                       
                /* Read optional colour table*/
                if (os2format) {  /* RGBTRIPLE */
                        for (i = 0; i < colours; i++) {
                                fread(&b, 1, 1, fp);
                                fread(&g, 1, 1, fp);
                                fread(&r, 1, 1, fp);
                                img->image.palette->Entries[i] = (((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff));
                        }
                }
                else { /* RGBSquads */
                        for (i = 0; i < colours; i++) {
                                fread(&b, 1, 1, fp);
                                fread(&g, 1, 1, fp);
                                fread(&r, 1, 1, fp);
                                fread(&a, 1, 1, fp);
                                img->image.palette->Entries[i] = (((a&0xff) << 24)  | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff));                       

                        }

                }
        }
        else
                img->image.palette = NULL;                                                                      
       
        pixels = GdipAlloc (img->data.Stride * img->data.Height);

        if (upsidedown) { 
                for (i = img->data.Height - 1; i >= 0; i--) 
                        fread(pixels + i*img->data.Stride, img->data.Stride, 1, fp);
        }
        else {
                for (i = 0; i < img->data.Height; i++) 
                        fread(pixels + i*img->data.Stride, img->data.Stride, 1, fp);
        }
	
	/* Cairo stores internally RGB24 as RGB32 */	
	if (bmi.biBitCount == 24) {
		BYTE *dest;
		int dest_stride;
	
		gdip_from_RGB_to_ARGB (pixels, img->image.width, img->image.height, img->data.Stride, &dest, &dest_stride);
		
		GdipFree (pixels);
		pixels = dest;	
		img->data.Stride = dest_stride;
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

        *image = (GpImage *) img;

        return Ok;
}

GpStatus 
gdip_load_bmp_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                          SeekDelegate seeknFunc,
                                          GpImage **image)
{
	BITMAPFILEHEADER bmfh;
        BITMAPINFOHEADER bmi;
        GpBitmap *img = NULL;
        guchar *pixels = NULL, *linep = NULL;
        int i, j, format, colours;
        BOOL os2format = FALSE, upsidedown = TRUE;
        int size, size_read;
	byte* data_read;
		
	size = sizeof(bmfh);
	data_read = (byte*) GdipAlloc(size);
	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	
	bmfh.bfType = ((data_read[1]<<8)|data_read[0]);
	if (bmfh.bfType != BFT_BITMAP )
		return UnknownImageFormat;

	bmfh.bfSize = (data_read[5]<<24 | data_read[4]<<16 | data_read[3]<<8 | data_read[2]);
	bmfh.bfReserved1 = ((data_read[7]<<8)|data_read[6]);
	bmfh.bfReserved1 = ((data_read[9]<<8)|data_read[8]);
	bmfh.bfOffBits = (data_read[13]<<24 | data_read[12]<<16 | data_read[11]<<8 | data_read[10]);

	GdipFree(data_read);

	size = sizeof(DWORD);
	data_read = (byte*)GdipAlloc(size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biSize = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

	if (bmi.biSize > BITMAPCOREHEADER_SIZE){   /* New Windows headers can be bigger */ 
		memset (data_read, 0, size);
		size_read = getBytesFunc(data_read, size, 0);
		if (size_read < size)
			return InvalidParameter;
		bmi.biWidth = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);

		memset (data_read, 0, size);
		size_read = getBytesFunc(data_read, size, 0);
		if (size_read < size)
			return InvalidParameter;
		bmi.biHeight = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
 	}
        else  {
		if (bmi.biSize ==  BITMAPCOREHEADER_SIZE) {
			/* Old OS/2 format. Width and Height fields are WORDs instead of DWORDS */
                        memset (data_read, 0, size);
			size_read = getBytesFunc(data_read, size, 0);
			if (size_read < size)
				return InvalidParameter;
			bmi.biWidth = (data_read[1]<<8 | data_read[0]);
			bmi.biHeight = (data_read[3]<<8 | data_read[2]);
                        os2format = TRUE;
                }
                else
                        return UnknownImageFormat;
        }

	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biPlanes = (data_read[1]<<8 | data_read[0]); 
	bmi.biBitCount = (data_read[3]<<8 | data_read[2]); 

	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biCompression = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
        if (bmi.biCompression == BI_RLE4 || bmi.biCompression == BI_RLE8)
                return NotImplemented; /* We do not support RLE for now*/

        if (bmi.biHeight < 0) { /* Negative height indicates that the bitmap is sideup*/
                upsidedown = FALSE;
                bmi.biHeight =  -bmi.biHeight;
        }

	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biSizeImage = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biXPelsPerMeter = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
	
	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biYPelsPerMeter = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biClrUsed = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	memset (data_read, 0, size);
	size_read = getBytesFunc(data_read, size, 0);
	if (size_read < size)
		return InvalidParameter;
	bmi.biClrImportant = (data_read[3]<<24 | data_read[2]<<16 | data_read[1]<<8 | data_read[0]);
		
	colours =  (bmi.biClrUsed == 0 && bmi.biBitCount <= 8) ? (1 << bmi.biBitCount) : bmi.biClrUsed;
        
        format = gdip_get_pixelformat (bmi.biBitCount);
                                               
        if (format == 0)
                return NotImplemented; /* Unsuported pixel format*/

        img = gdip_bitmap_new ();
        img->image.pixFormat = format;
        img->image.type = imageBitmap;
        img->image.graphics = 0;
        img->image.width = bmi.biWidth;
        img->image.height = bmi.biHeight;

        img->data.PixelFormat = img->image.pixFormat;
        img->data.Width = img->image.width;
        img->data.Height = img->image.height;
        img->data.Stride = (bmi.biBitCount * img->image.width) / 8;
        img->data.Stride = (img->data.Stride + 3) & ~3;
     
        if (colours){               
                img->image.palette = g_malloc (sizeof(ColorPalette) + sizeof(ARGB) * colours);
                img->image.palette->Flags = 0;
                img->image.palette->Count = colours;
                       
                /* Read optional colour table*/
                if (os2format) {  /* RGBTRIPLE */
			size = sizeof(byte)*3;
			data_read = (byte*) GdipAlloc(size);
                        for (i = 0; i < colours; i++) {
				memset(data_read, 0, size);
				size_read = getBytesFunc(data_read, size, 0);
				if (size_read < size)
					return InvalidParameter;
				img->image.palette->Entries[i] = (((data_read[2]&0xff)<<16) | 
					((data_read[1]&0xff)<<8) | (data_read[0]&0xff));
                        }
			GdipFree(data_read);
                }
                else { /* RGBSquads */
			size = sizeof(byte)*4;
			data_read = (byte*) GdipAlloc(size);
                        for (i = 0; i < colours; i++) {
				memset(data_read, 0, size);
                                size_read = getBytesFunc(data_read, size, 0);
				if (size_read < size)
					return InvalidParameter;
                                img->image.palette->Entries[i] = (((data_read[3]&0xff)<<24)  | 
					((data_read[2]&0xff)<<16) | ((data_read[1]&0xff)<<8) | 
						(data_read[0]& 0xff));                       
                        }
			GdipFree(data_read);
                }
        }
        else
                img->image.palette = NULL;                                                                      
       
        pixels = GdipAlloc (img->data.Stride * img->data.Height);

	size = img->data.Stride;
	data_read = (byte*) GdipAlloc(size);
        if (upsidedown) { 
		for (i = img->data.Height - 1; i >= 0; i--){ 
			memset(data_read, 0, size);
			size_read = getBytesFunc(data_read, size, 0);
			if (size_read < size)
				return InvalidParameter;                        
			memcpy(pixels + i*size, data_read, size);			
		}
        }
        else {
		for (i = 0; i < img->data.Height; i++) {
			memset(data_read, 0, size);
			size_read = getBytesFunc(data_read, size, 0);
			if (size_read < size)
				return InvalidParameter;                        			
                        memcpy(pixels + i*size, data_read, size);			
		}
        }

	GdipFree(data_read);
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

        *image = (GpImage *) img;

        return Ok;
}

GpStatus 
gdip_save_bmp_image_to_file (FILE *fp, GpImage *image)
{
        BITMAPFILEHEADER bmfh;
        BITMAPINFOHEADER bmi;
        GpBitmap *bitmap = (GpBitmap *) image;
        int bitmapLen = bitmap->data.Stride * bitmap->data.Height;
        int i;
        byte b;
        ARGB color;
        int colours = 0;

        if (bitmap->image.palette)
                colours = bitmap->image.palette->Count;

        bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
        bmfh.bfType = BFT_BITMAP;
        bmfh.bfSize = (bmfh.bfOffBits + bitmapLen);

        bmfh.bfOffBits = (14 + 40 + colours * 4);
        fwrite (&bmfh, 1, sizeof (bmfh), fp);
        gdip_bitmap_fill_info_header (bitmap, &bmi);

        fwrite (&bmi, 1, sizeof (bmi), fp);

        if (colours) {

                /* Write palette on disk on BGRA*/
                for (i = 0; bitmap->image.palette->Count; i++) {
                        color = bitmap->image.palette->Entries[i];
                        b =  color & 0xff;
                        fwrite (&b, 1, 1, fp);
                        b = (color >> 8) & 0xff;
                        fwrite (&b, 1, 1, fp);
                        b = (color >> 16) & 0xff;
                        fwrite (&b, 1, 1, fp);
                        b = color >> 24;
                        fwrite (&b, 1, 1, fp);
                }
        }
        
        /* Writes bitmap upside down. Many tools can only process bmp stored this way*/        
        for (i = bitmap->data.Height - 1; i >= 0; i--)
                fwrite (bitmap->data.Scan0 + i *bitmap->data.Stride, bitmap->data.Stride, 1, fp);
        return Ok;
}

GpStatus 
gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image)
{
        return NotImplemented;
}

