/*
 * gifcodec.h : Contains function declarations for encoding decoding gif images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _GIFCODEC_H
#define _GIFCODEC_H
#include "gdip.h"

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage *image);

GpStatus 
gdip_save_gif_image_to_file (FILE *fp, GpImage *image);
#endif /* _GIFCODEC_H */
