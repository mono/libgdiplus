/*
 * bmpcodec.h : Contains function declarations for encoding decoding bmp images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _BMPCODEC_H
#define _BMPCODEC_H
#include "gdip.h"

GpStatus 
gdip_load_bmp_image_from_file (FILE *fp, GpImage **image);

GpStatus 
gdip_save_bmp_image_to_file (FILE *fp, GpImage *image);
#endif /* _BMPCODEC_H */
