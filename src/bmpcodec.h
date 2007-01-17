/*
 * bmpcodec.h : Contains function declarations for encoding decoding bmp images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *      Mark Steele (ms@rapidsys.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _BMPCODEC_H
#define _BMPCODEC_H
#include "gdip.h"
#include "dstream.h"

#define BI_RGB           0
#define BI_RLE8          1
#define BI_RLE4          2
#define BI_BITFIELDS     3

#define BITMAPINFOHEADER_SIZE   40
#define BITMAPCOREHEADER_SIZE   12

#ifdef __GNUC__
#    pragma pack(2)
#endif
typedef struct
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER, *LPBITMAPFILEHEADER;
#ifdef __GNUC__
#    pragma pack()
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

GpStatus
gdip_load_bmp_image_from_file (FILE *fp, GpImage **image);

GpStatus
gdip_load_bmp_image_from_stream_delegate (dstream_t *loader, GpImage **image);

GpStatus
gdip_save_bmp_image_to_file (FILE *fp, GpImage *image);

GpStatus
gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image);

ImageCodecInfo *
gdip_getcodecinfo_bmp ();

void
gdip_bitmap_save_bmp (const char *name, GpBitmap *bitmap);

void 
gdip_bitmap_fill_info_header (GpBitmap *bitmap, PBITMAPINFOHEADER bmi);

GpStatus 
gdip_save_bmp_image_to_file_stream (void *pointer, GpImage *image, bool useFile);

void
gdip_write_bmp_data (void *pointer, byte *data, int size, bool useFile);

GpStatus 
gdip_read_bmp_image_from_file_stream (void *pointer, GpImage **image, bool useFile);

int
gdip_read_bmp_data (void *pointer, byte *data, int size, bool useFile);

/* helper functions / shared with ICOn codec */
GpStatus gdip_read_BITMAPINFOHEADER (void *pointer, BITMAPINFOHEADER *bmi, bool useFile, BOOL *os2format, BOOL *upsidedown);
int gdip_read_bmp_data (void *pointer, byte *data, int size, bool useFile);

#endif /* _BMPCODEC_H */
