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
#include <zlib.h>

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

size_t png_size_byte = sizeof(byte);

GpStatus 
gdip_load_png_image_from_file (FILE *fp, GpImage *image)
{
	size_t size_read;
	int seek;
	unsigned int length;
	unsigned int crc_obtained;
	unsigned int crc_calculated;
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
	bool can_read_plte = FALSE;
	PNGHeader header;
	GpStatus status = Ok;
	
/*	image = (GpImage *) gdip_image_new (); */
	if (image == NULL)
		return OutOfMemory;

	image->type = imageBitmap;
	seek = fseek(fp, 8, SEEK_SET);
	if (seek) 
		return gdip_png_handle_error (image, InvalidParameter);
	
	size_read = fread (chunk_length, png_size_byte, 4, fp); /*read length of IHDR chunk*/
	if (size_read != 4)
		return gdip_png_handle_error (image, InvalidParameter);

	length = gdip_png_get_uint (chunk_length);

	if (length != 13) /*length of header chunk should be 13*/
		return gdip_png_handle_error (image, InvalidParameter);

	size_read = fread (chunk_name, png_size_byte, 4, fp);
	if (size_read != 4)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_calculated = crc32(0L, Z_NULL, 0);

	if (memcmp(chunk_name, png_IHDR, 4) != 0)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_calculated = crc32(crc_calculated, chunk_name, 4);

	/*read 13 bytes of image header*/
	/*chunk_data = new chunk_data[13];*/
	size_read = fread (chunk_data, png_size_byte, 13, fp);
	if (size_read != 13)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_calculated = crc32(crc_calculated, chunk_data, size_read);

	size_read = fread (chunk_crc, png_size_byte, 4, fp); /*read the crc*/
	if (size_read != 4)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_obtained = gdip_png_get_uint(chunk_crc);

	if (crc_calculated != crc_obtained)
		return gdip_png_handle_error(image, InvalidParameter);

	header.width = image->width = gdip_png_get_uint (chunk_data);

	header.height = image->height = gdip_png_get_uint (chunk_data+4);

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
			can_read_plte = TRUE;
			break;
		case 3:
			if (bit_depth == 16)
				return gdip_png_handle_error (image, InvalidParameter);
			num_channels = 1;
			can_read_plte = TRUE;
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
			can_read_plte = TRUE;
			break;
		default:
			return gdip_png_handle_error (image, InvalidParameter);
	}
	
	header.color_type = color_type;

	pixel_depth = num_channels * bit_depth;	

	header.compression_method = *(chunk_data + 10);
	if (header.compression_method != 0) /*only deflate/inflate compression is defined*/
		return gdip_png_handle_error (image, InvalidParameter);

	header.filter_method = *(chunk_data + 11);
	if (header.filter_method != 0) /*only adaptive filtering with five basic filter types is defined */
		return gdip_png_handle_error (image, InvalidParameter);

	header.interlace_method = *(chunk_data + 12);
	switch (header.interlace_method){
		case 0: /*no interlace method*/
		case 1: /*Adam7 interlace*/
			break;
		default:
            return gdip_png_handle_error (image, InvalidParameter);
	}

	while (reading ){
		size_read = fread (chunk_length, png_size_byte, 4, fp); /*read length of next chunk*/
		if (size_read != 4)
			return gdip_png_handle_error (image, InvalidParameter);

		length = gdip_png_get_uint (chunk_length);

		size_read = fread (chunk_name, png_size_byte, 4, fp);
		if (size_read != 4)
			return gdip_png_handle_error (image, InvalidParameter);

		crc_calculated = crc32 (0L, Z_NULL, 0); /*reinitialize the crc*/
		crc_calculated = crc32 (crc_calculated, chunk_name, 4); /*calculate CRC for the chunk name*/

		if (memcmp(chunk_name, png_IDAT, 4) == 0){
			if (color_type == 3 && plte_read == FALSE) /*we came to read idat chunk
				without reading plte chunk for a palette based image, signal error */
				return gdip_png_handle_error (image, InvalidParameter);
			status = gdip_read_png_idat_chunk (fp, length, &header, image, crc_calculated);
			if (status != Ok)
				return status;
			idat_read = TRUE;
		}

		if (memcmp(chunk_name, png_IEND, 4) == 0){
			if (idat_read == FALSE)/* We are reading IEND chunk without reading IDAT
									chunk, signal error*/
				return gdip_png_handle_error (image, InvalidParameter);
			status = gdip_read_png_iend_chunk (fp, length, &header, crc_calculated);
			if (status != Ok)
				return gdip_png_handle_error (image, status);
			reading = FALSE;
		}

		if (memcmp(chunk_name, png_PLTE, 4) == 0){
			if (idat_read) /*we have read idat chunk already, plte chunk is out of place*/
				return gdip_png_handle_error (image, InvalidParameter);
			if (length > 768 || length%3 != 0 ) /*plte chunk can have only 256(2^8) entries
				corresponding to maximum bit depth of 8. Each entry is 3 bytes one each for RGB */
				return gdip_png_handle_error (image, InvalidParameter);
			if (can_read_plte){
				status = gdip_read_png_plte_chunk (fp, length, &header, image, crc_calculated);
				if (status != Ok)
					return status;
				plte_read = TRUE;
			} else 
				return gdip_png_handle_error (image, status);
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
gdip_read_png_idat_chunk(FILE *fp, int length, PNGHeader *header, GpImage *image, unsigned int crc)
{
	byte *chunk_data;
	byte chunk_crc[4];
	int size_read;
	unsigned int crc_calculated;
	unsigned int crc_obtained;

	size_read = fread (chunk_data, png_size_byte, length, fp);
	if (size_read != length)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_calculated = crc32(crc, chunk_data, size_read);

	size_read = fread (chunk_crc, png_size_byte, 4, fp); /*read the crc*/
	if (size_read != 4)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_obtained = gdip_png_get_uint(chunk_crc);

	if (crc_calculated != crc_obtained)
		return gdip_png_handle_error(image, InvalidParameter);

	return NotImplemented;
}

GpStatus
gdip_read_png_iend_chunk(FILE *fp, int length, PNGHeader *header, unsigned int crc)
{
	byte chunk_crc[4];
	int size_read;
	unsigned int crc_obtained;

	size_read = fread (chunk_crc, png_size_byte, 4, fp); /*read the crc*/
	if (size_read != 4)
		return InvalidParameter;

	crc_obtained = gdip_png_get_uint(chunk_crc);

	if (crc != crc_obtained)
		return InvalidParameter;
	
	return Ok;
}

GpStatus
gdip_read_png_plte_chunk(FILE *fp, int length, PNGHeader *header, GpImage *image, unsigned int crc)
{
	byte *chunk_data;
	byte chunk_crc[4];
	byte palette_entry[3];
	int size_read;
	unsigned int crc_calculated;
	unsigned int crc_obtained;
	ARGB *palette_entries;
	int num_entries, i;
	
	size_read = fread (chunk_data, png_size_byte, length, fp);
	if (size_read != length)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_calculated = crc32(crc, chunk_data, size_read);

	size_read = fread (chunk_crc, png_size_byte, 4, fp); /*read the crc*/
	if (size_read != 4)
		return gdip_png_handle_error (image, InvalidParameter);

	crc_obtained = gdip_png_get_uint(chunk_crc);

	if (crc_calculated != crc_obtained)
		return gdip_png_handle_error(image, InvalidParameter);

	num_entries = length/3;
	image->palette->Count = num_entries;
	image->palette->Entries = (ARGB*) malloc(num_entries*sizeof(ARGB));
	for (i=0; i<num_entries; i++, chunk_data+=3) {
		image->palette->Entries[i] = (ARGB)(((unsigned int) (*chunk_data) << 16) +
								((unsigned int) *(chunk_data+1) << 8) + 
								((unsigned int) *(chunk_data+2))) ;
	}
	/*image->palette->Entries = &palette_entries;*/
	
	return Ok;
}

unsigned int gdip_png_get_uint (byte *bytep)
{
	return ((unsigned int)(*bytep) << 24) + 
			((unsigned int)(*(bytep + 1)) << 16) +
			((unsigned int)(*(bytep + 2)) << 8) +
			(unsigned int)(*(bytep + 3));
}

