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
        return NotImplemented;
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

}

GpStatus 
gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image)
{
        return NotImplemented;
}

