/*
 * pngcodec.h : Contains function declarations for encoding decoding png images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _PNGCODEC_H
#define _PNGCODEC_H
#include "gdip.h"

typedef struct {
	unsigned int width;
	unsigned int height;
	byte bit_depth;
	byte color_type;
	byte compression_method;
	byte filter_method;
	byte interlace_method;
} PNGHeader;

GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage *image);

GpStatus 
gdip_save_png_image_to_file (FILE *fp, GpImage *image);

GpStatus
gdip_png_handle_error (GpImage *image, GpStatus status);

GpStatus
gdip_read_png_idat_chunk (FILE *fp, int length, PNGHeader *header, GpImage *image, unsigned int crc);

GpStatus
gdip_read_png_iend_chunk (FILE *fp, int length, PNGHeader *header, unsigned int crc);

GpStatus
gdip_read_png_plte_chunk (FILE *fp, int length, PNGHeader *header, GpImage *image, unsigned int crc);

unsigned int gdip_png_get_uint (byte *bytep);


#endif /* _PNGCODEC_H */
