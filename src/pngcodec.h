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

GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage *image);

GpStatus 
gdip_save_png_image_to_file (FILE *fp, GpImage *image);
#endif /* _PNGCODEC_H */
