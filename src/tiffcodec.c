/*
 * tiffcodec.c : Contains function definitions for encoding decoding tiff images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include <stdio.h>
#include "tiffcodec.h"

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage *image)
{
	image = (GpImage *) gdip_image_new ();
	image->type = imageBitmap;
	return NotImplemented;
}
