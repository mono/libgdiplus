/*
 * tiffcodec.h : Contains function declarations for encoding decoding tiff images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *      Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _TIFFCODEC_H
#define _TIFFCODEC_H
#include "gdip.h"

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image);

GpStatus
gdip_load_tiff_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
					   PutBytesDelegate putBytesFunc,
                                           SeekDelegate seekFunc,
					   CloseDelegate closeFunc,
					   SizeDelegate sizeFunc,
                                           GpImage **image);

GpStatus 
gdip_save_tiff_image_to_file (FILE *fp, GpImage *image,
			      GDIPCONST EncoderParameters *params);

GpStatus
gdip_save_tiff_image_to_stream_delegate (GetBytesDelegate getBytesFunc,
					 PutBytesDelegate putBytesFunc,
                                         SeekDelegate seekFunc,
					 CloseDelegate closeFunc,
					 SizeDelegate sizeFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params);

ImageCodecInfo *
gdip_getcodecinfo_tiff ();

#endif /* _TIFFCODEC_H */
