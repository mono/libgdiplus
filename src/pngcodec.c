/*
 * pngcodec.c : Contains function definitions for encoding decoding png images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include <stdio.h>
#include "pngcodec.h"

GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage *image)
{
	size_t size_read;
	int seek;
	unsigned int length;
	byte chunk_length[4];
	byte chunk_name[4];
	byte *chunk_data;
	byte chunk_crc[4];

	byte IHDR[] = { 73,  72,  68,  82, '\0'}; /*png header*/

	image = (GpImage *) gdip_image_new ();
	image->type = imageBitmap;
	seek = fseek(fp, 8, SEEK_SET);
	if (seek)
		return GenericError;
	
	size_read = fread (chunk_length, sizeof(byte), 4, fp); /*read length of IHDR chunk*/
	if (size_read != 4)
		return InvalidParameter;
	
	length = ((unsigned int)(*chunk_length) << 24) +
      ((unsigned int)(*(chunk_length + 1)) << 16) +
      ((unsigned int)(*(chunk_length + 2)) << 8) +
      (unsigned int)(*(chunk_length + 3));

	if (length != 13)  /*length of header chunk should be 13*/
		return InvalidParameter;

	size_read = fread (chunk_name, sizeof(byte), 4, fp);
	if (size_read != 4)
		return InvalidParameter;

	if (strncmp(chunk_name, IHDR, 4) != 0)
		return InvalidParameter;

	/*read 13 bytes of image header*/
	/*chunk_data = new chunk_data[13];*/
	size_read = fread (chunk_data, sizeof(byte), 13, fp);
	if (size_read != 13)
		return InvalidParameter;

	image->width = ((unsigned int)(*chunk_data) << 24) +
      ((unsigned int)(*(chunk_data + 1)) << 16) +
      ((unsigned int)(*(chunk_data + 2)) << 8) +
      (unsigned int)(*(chunk_data + 3));

	image->height = ((unsigned int)(*chunk_data + 4) << 24) +
      ((unsigned int)(*(chunk_data + 5)) << 16) +
      ((unsigned int)(*(chunk_data + 6)) << 8) +
      (unsigned int)(*(chunk_data + 7));

	
	return NotImplemented;
}
