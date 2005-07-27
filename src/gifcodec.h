/*
 * gifcodec.h : Contains function declarations for encoding decoding gif images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _GIFCODEC_H
#define _GIFCODEC_H
#include "gdip.h"

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image);

GpStatus
gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           SeekDelegate seekFunc,
                                           GpImage **image);
					   
GpStatus 
gdip_save_gif_image_to_file (unsigned char *filename, GpImage *image);


GpStatus
gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params);


ImageCodecInfo *
gdip_getcodecinfo_gif ();

#endif /* _GIFCODEC_H */
