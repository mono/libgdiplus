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
#include "gdip_win32.h"

GpStatus
gdip_load_bmp_image_from_file (FILE *fp, GpImage **image);

GpStatus
gdip_load_bmp_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                          SeekDelegate seeknFunc,
                                          GpImage **image);

GpStatus
gdip_save_bmp_image_to_file (FILE *fp, GpImage *image);

GpStatus
gdip_save_bmp_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                        GpImage *image);

ImageCodecInfo *
gdip_getcodecinfo_bmp ();

#endif /* _BMPCODEC_H */
