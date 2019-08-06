/*
 * gifcodec.h : Contains function declarations for encoding decoding gif images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) 2003-2004, 2007 Novell, Inc (http://www.novell.com)
 */

#ifndef _GIFCODEC_H
#define _GIFCODEC_H

#include "bitmap-private.h"
#include "bmpcodec.h"

GpStatus gdip_load_gif_image_from_file (FILE *fp, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc, SeekDelegate seekFunc, 
	GpImage **image) GDIP_INTERNAL;
					   
GpStatus gdip_save_gif_image_to_file (unsigned char *filename, GpImage *image) GDIP_INTERNAL;

GpStatus gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image, 
	GDIPCONST EncoderParameters *params) GDIP_INTERNAL;

ImageCodecInfo *gdip_getcodecinfo_gif () GDIP_INTERNAL;

GpStatus gdip_fill_encoder_parameter_list_gif (EncoderParameters *buffer, UINT size) GDIP_INTERNAL;

typedef struct
{
  UINT count;
  EncoderParameter imageItems;
  EncoderParameter saveFlag;
  LONG saveFlagValue;
} GifEncoderParameters;

#endif /* _GIFCODEC_H */
