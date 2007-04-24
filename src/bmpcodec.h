/*
 * bmpcodec.h : Contains function declarations for encoding decoding bmp images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *      Mark Steele (ms@rapidsys.com)
 *
 * Copyright (C) 2003-2004, 2007 Novell, Inc (http://www.novell.com)
 */

#ifndef _BMPCODEC_H
#define _BMPCODEC_H

#include "bitmap-private.h"
//#include "codecs-private.h"
#include "dstream.h"

#define BI_RGB           0
#define BI_RLE8          1
#define BI_RLE4          2
#define BI_BITFIELDS     3

#define BITMAPINFOHEADER_SIZE   40
#define BITMAPCOREHEADER_SIZE   12

#ifndef __GNUC__
	#pragma pack(2)
#endif
typedef struct
#ifdef __GNUC__
	 __attribute__ ((packed))
#endif
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER, *LPBITMAPFILEHEADER;
#ifndef __GNUC__
	#pragma pack()
#endif

#define BFT_BITMAP 0x4d42


typedef struct {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;

typedef struct {
    DWORD 	biSize;
    LONG  	biWidth;
    LONG  	biHeight;
    WORD 	biPlanes;
    WORD 	biBitCount;
    DWORD 	biCompression;
    DWORD 	biSizeImage;
    LONG  	biXPelsPerMeter;
    LONG  	biYPelsPerMeter;
    DWORD 	biClrUsed;
    DWORD 	biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;

typedef long FXPT2DOT30;

typedef struct {
	FXPT2DOT30	ciexyzX;
	FXPT2DOT30	ciexyzY;
	FXPT2DOT30	ciexyzZ;
} CIEXYZ;

typedef struct {
	CIEXYZ	ciexyzRed;
	CIEXYZ	ciexyzGreen;
	CIEXYZ	ciexyzBlue;
} CIEXYZTRIPLE;

typedef struct {
	DWORD 	bV4Size;
	LONG  	bV4Width;
	LONG  	bV4Height;
	WORD 	bV4Planes;
	WORD 	bV4BitCount;
	DWORD 	bV4Compression;
	DWORD 	bV4SizeImage;
	LONG  	bV4XPelsPerMeter;
	LONG  	bV4YPelsPerMeter;
	DWORD 	bV4ClrUsed;
	DWORD 	bV4ClrImportant;
	DWORD	bV4RedMask;
	DWORD	bV4GreenMask;
	DWORD	bV4BlueMask;
	DWORD	bV4AlphaMask;
	DWORD	bV4CSType;
	CIEXYZTRIPLE	bV4Endpoints;
	DWORD	bV4GammaRed;
	DWORD	bV4GammaGreen;
	DWORD	bV4GammaBlue;
} BITMAPV4HEADER, *PBITMAPV4HEADER;

typedef struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD	bmiColors[1];
} BITMAPINFO, *PBITMAPINFO, *LPBITMAPINFO;

#include "gdiplus-private.h"

GpStatus gdip_read_bmp_image (void *pointer, GpImage **image, ImageSource source) GDIP_INTERNAL;
GpStatus gdip_load_bmp_image_from_file (FILE *fp, GpImage **image) GDIP_INTERNAL;
GpStatus gdip_load_bmp_image_from_stream_delegate (dstream_t *loader, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_save_bmp_image_to_file (FILE *fp, GpImage *image) GDIP_INTERNAL;
GpStatus gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image) GDIP_INTERNAL;

ImageCodecInfo *gdip_getcodecinfo_bmp () GDIP_INTERNAL;

/* helper functions / shared with ICOn codec */
GpStatus gdip_read_BITMAPINFOHEADER (void *pointer, BITMAPINFOHEADER *bmi, ImageSource source, BOOL *os2format, 
	BOOL *upsidedown) GDIP_INTERNAL;
int gdip_read_bmp_data (void *pointer, BYTE *data, int size, ImageSource source) GDIP_INTERNAL;

#endif /* _BMPCODEC_H */
