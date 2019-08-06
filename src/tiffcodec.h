/*
 * tiffcodec.h : Contains function declarations for encoding decoding tiff images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *      Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) 2003-2004, 2007 Novell, Inc (http://www.novell.com)
 */

#ifndef _TIFFCODEC_H
#define _TIFFCODEC_H

#include "bitmap-private.h"
#include "bmpcodec.h"

GpStatus gdip_load_tiff_image_from_file (FILE *fp, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_load_tiff_image_from_stream_delegate (GetBytesDelegate getBytesFunc, PutBytesDelegate putBytesFunc,
	SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_save_tiff_image_to_file (unsigned char *filename, GpImage *image, GDIPCONST EncoderParameters *params) GDIP_INTERNAL;

GpStatus gdip_save_tiff_image_to_stream_delegate (GetBytesDelegate getBytesFunc, PutBytesDelegate putBytesFunc,
	SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, GpImage *image, 
	GDIPCONST EncoderParameters *params) GDIP_INTERNAL;

ImageCodecInfo* gdip_getcodecinfo_tiff () GDIP_INTERNAL;

GpStatus gdip_fill_encoder_parameter_list_tiff (EncoderParameters *buffer, UINT size) GDIP_INTERNAL;

typedef struct
{
  UINT count;
  EncoderParameter compression;
  EncoderParameter colorDepth;
  EncoderParameter saveFlag;
  EncoderParameter saveAsCYMK;
  LONG compressionData[5];
  LONG colorDepthData[5];
  LONG saveFlagValue;
  LONG saveAsCYMKValue;
} TiffEncoderParameters;

#endif /* _TIFFCODEC_H */
