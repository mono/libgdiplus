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

#define BITMAPINFOHEADER_SIZE   40
#define BITMAPCOREHEADER_SIZE   12

#define BFT_BITMAP 0x4d42

typedef struct {
	DWORD 	bV3Size;
	LONG  	bV3Width;
	LONG  	bV3Height;
	WORD 	bV3Planes;
	WORD 	bV3BitCount;
	DWORD 	bV3Compression;
	DWORD 	bV3SizeImage;
	LONG  	bV3XPelsPerMeter;
	LONG  	bV3YPelsPerMeter;
	DWORD 	bV3ClrUsed;
	DWORD 	bV3ClrImportant;
	DWORD	bV3RedMask;
	DWORD	bV3GreenMask;
	DWORD	bV3BlueMask;
	DWORD	bV3AlphaMask;
} BITMAPV3HEADER, *PBITMAPV3HEADER;

#include "gdiplus-private.h"

GpStatus gdip_read_bmp_image (void *pointer, GpImage **image, ImageSource source) GDIP_INTERNAL;
GpStatus gdip_load_bmp_image_from_file (FILE *fp, GpImage **image) GDIP_INTERNAL;
GpStatus gdip_load_bmp_image_from_stream_delegate (dstream_t *loader, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_save_bmp_image_to_file (FILE *fp, GpImage *image) GDIP_INTERNAL;
GpStatus gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image) GDIP_INTERNAL;

ImageCodecInfo *gdip_getcodecinfo_bmp () GDIP_INTERNAL;

/* helper functions / shared with ICOn codec */
GpStatus gdip_read_BITMAPINFOHEADER (void *pointer, ImageSource source, BITMAPV5HEADER *bmi, BOOL *os2format, 
	BOOL *upsidedown) GDIP_INTERNAL;
int gdip_read_bmp_data (void *pointer, BYTE *data, int size, ImageSource source) GDIP_INTERNAL;

#endif /* _BMPCODEC_H */
