/*
 * jpegcodec.h : Contains function declarations for encoding decoding jpeg images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *      Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _JPEGCODEC_H
#define _JPEGCODEC_H
#include "gdip.h"

GpStatus 
gdip_load_jpeg_image_from_file (FILE *fp, GpImage **image);

GpStatus 
gdip_save_jpeg_image_to_file (FILE *fp, GpImage *image);
#endif /* _PNGCODEC_H */
