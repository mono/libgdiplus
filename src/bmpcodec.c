/*
 * bmpcodec.c : Contains function definitions for encoding decoding bmp images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include <stdio.h>
#include "bmpcodec.h"

GpStatus 
gdip_load_png_bmp_from_file (FILE *fp, GpImage *image)
{
	image = (GpImage *) gdip_image_new ();
	image->type = imageBitmap;
	return NotImplemented;
}
