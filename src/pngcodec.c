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

byte png_IHDR[5] = { 73,  72,  68,  82, '\0'};
byte png_IDAT[5] = { 73,  68,  65,  84, '\0'};
byte png_IEND[5] = { 73,  69,  78,  68, '\0'};
byte png_PLTE[5] = { 80,  76,  84,  69, '\0'};
byte png_bKGD[5] = { 98,  75,  71,  68, '\0'};
byte png_cHRM[5] = { 99,  72,  82,  77, '\0'};
byte png_gAMA[5] = {103,  65,  77,  65, '\0'};
byte png_hIST[5] = {104,  73,  83,  84, '\0'};
byte png_iCCP[5] = {105,  67,  67,  80, '\0'};
byte png_pHYs[5] = {112,  72,  89, 115, '\0'};
byte png_sBIT[5] = {115,  66,  73,  84, '\0'};
byte png_sPLT[5] = {115,  80,  76,  84, '\0'};
byte png_sRGB[5] = {115,  82,  71,  66, '\0'};
byte png_tEXt[5] = {116,  69,  88, 116, '\0'};
byte png_tIME[5] = {116,  73,  77,  69, '\0'};
byte png_tRNS[5] = {116,  82,  78,  83, '\0'};
byte png_zTXt[5] = {122,  84,  88, 116, '\0'};

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
	byte bit_depth;
	byte color_type;
	int num_channels = 0;
	int pixel_depth = 0;
	bool idat_read = FALSE;
	bool plte_read = FALSE;
	bool reading = TRUE;
	PNGHeader header;
	GpStatus status = Ok;

	image = (GpImage *) gdip_image_new ();
	if (image == NULL)
		return OutOfMemory;

	image->type = imageBitmap;
	seek = fseek(fp, 8, SEEK_SET);
	if (seek) 
		return gdip_png_handle_error (image, InvalidParameter);
	
	size_read = fread (chunk_length, sizeof(byte), 4, fp); /*read length of IHDR chunk*/
	if (size_read != 4)
		return gdip_png_handle_error (image, InvalidParameter);

	length = ((unsigned int)(*chunk_length) << 24) +
      ((unsigned int)(*(chunk_length + 1)) << 16) +
      ((unsigned int)(*(chunk_length + 2)) << 8) +
      (unsigned int)(*(chunk_length + 3));

	if (length != 13) /*length of header chunk should be 13*/
		return gdip_png_handle_error (image, InvalidParameter);

	size_read = fread (chunk_name, sizeof(byte), 4, fp);
	if (size_read != 4)
		return gdip_png_handle_error (image, InvalidParameter);

	if (memcmp(chunk_name, png_IHDR, 4) != 0)
		return gdip_png_handle_error (image, InvalidParameter);

	/*read 13 bytes of image header*/
	/*chunk_data = new chunk_data[13];*/
	size_read = fread (chunk_data, sizeof(byte), 13, fp);
	if (size_read != 13)
		return gdip_png_handle_error (image, InvalidParameter);

	header.width = image->width = ((unsigned int)(*chunk_data) << 24) +
      ((unsigned int)(*(chunk_data + 1)) << 16) +
      ((unsigned int)(*(chunk_data + 2)) << 8) +
      (unsigned int)(*(chunk_data + 3));

	header.height = image->height = ((unsigned int)(*chunk_data + 4) << 24) +
      ((unsigned int)(*(chunk_data + 5)) << 16) +
      ((unsigned int)(*(chunk_data + 6)) << 8) +
      (unsigned int)(*(chunk_data + 7));

	/* Read bit depth valid values are 1, 2, 4, 8, and 16 */
	bit_depth = *(chunk_data + 8);
	switch (bit_depth) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
			break;
		default:
			return gdip_png_handle_error (image, InvalidParameter);
	}
	
	header.bit_depth = bit_depth;
	/* Read color type valid values are 0, 2, 3, 4, and 6 */
	color_type = *(chunk_data + 9);
	switch (color_type) {
		case 0:
			image->imageFlags |= 0x40; /*ImageFlagsColorSpaceGRAY*/
			num_channels = 1;
			if (bit_depth == 16)
				image->pixFormat = Format16bppGrayScale;
			break;
		case 2:
			if (bit_depth != 8 || bit_depth != 16)
				return gdip_png_handle_error (image, InvalidParameter);
			image->imageFlags |= 0x10;  /*ImageFlagsColorSpaceRGB*/
			num_channels = 3;
			if (bit_depth == 8)
				image->pixFormat = Format24bppRgb;
			else
				image->pixFormat = Format48bppRgb;
			break;
		case 3:
			if (bit_depth == 16)
				return gdip_png_handle_error (image, InvalidParameter);
			num_channels = 1;
			break;
		case 4: /*image has alpha channel*/
			if (bit_depth != 8 || bit_depth != 16)
				return gdip_png_handle_error (image, InvalidParameter);
			image->imageFlags |= 0x02; /*ImageFlagsHasAlpha*/
			num_channels = 2; /*Grayscale image with an alpha channel*/
			image->pixFormat = Alpha;
			break;
		case 6: /*image is rgb and has alpha channel*/
			if (bit_depth != 8 || bit_depth != 16)
				return gdip_png_handle_error (image, InvalidParameter);
			image->imageFlags |= 0x10;  /*ImageFlagsColorSpaceRGB*/
			image->imageFlags |= 0x02;  /*ImageFlagsHasAlpha*/
			num_channels = 4;
			if (bit_depth == 8)
				image->pixFormat = Format32bppArgb;
			else
				image->pixFormat = Format64bppArgb;
			break;
		default:
			return gdip_png_handle_error (image, InvalidParameter);
	}
	
	header.color_type = color_type;

	pixel_depth = num_channels * bit_depth;	

	header.compression_method = *(chunk_data + 10);
	header.filter_method = *(chunk_data + 11);
	header.interlace_method = *(chunk_data + 12);

	while (reading && status == Ok ){
		size_read = fread (chunk_length, sizeof(byte), 4, fp); /*read length of next chunk*/
		if (size_read != 4)
			return gdip_png_handle_error (image, InvalidParameter);

		length = ((unsigned int)(*chunk_length) << 24) +
		((unsigned int)(*(chunk_length + 1)) << 16) +
		((unsigned int)(*(chunk_length + 2)) << 8) +
		(unsigned int)(*(chunk_length + 3));

		size_read = fread (chunk_name, sizeof(byte), 4, fp);
		if (size_read != 4)
			return gdip_png_handle_error (image, InvalidParameter);

		if (memcmp(chunk_name, png_IDAT, 4) == 0){
			if (color_type == 3 && plte_read == FALSE) /*we came to read idat chunk
				without reading plte chunk for a palette based image, signal error */
				return gdip_png_handle_error (image, InvalidParameter);
			status = gdip_read_png_idat_chunk(fp, length, &header, image);
			idat_read = TRUE;
		}

		if (memcmp(chunk_name, png_IEND, 4) == 0){
			if (idat_read == FALSE)/* We are reading IEND chunk without reading IDAT
									chunk, signal error*/
				return gdip_png_handle_error (image, InvalidParameter);
			status = gdip_read_png_iend_chunk(fp, length, &header);
			reading = FALSE;
		}

		if (memcmp(chunk_name, png_PLTE, 4) == 0){
			if (idat_read) /*we have read idat chunk already, plte chunk is out of place*/
				return gdip_png_handle_error (image, InvalidParameter);
			status = gdip_read_png_plte_chunk(fp, length, &header, image);
			plte_read = TRUE;
		}
	}
		
	return NotImplemented;
}

GpStatus
gdip_png_handle_error(GpImage *image, GpStatus status)
{
	GdipFree (image);
	return status;
}

GpStatus
gdip_read_png_idat_chunk(FILE *fp, int length, PNGHeader *header, GpImage *image)
{
	return NotImplemented;
}

GpStatus
gdip_read_png_iend_chunk(FILE *fp, int length, PNGHeader *header)
{
	/*everything has been read, nothing to be done more*/
	return Ok;
}

GpStatus
gdip_read_png_plte_chunk(FILE *fp, int length, PNGHeader *header, GpImage *image)
{
	return NotImplemented;
}


