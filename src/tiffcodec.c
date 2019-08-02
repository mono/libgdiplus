/* 
 *
 * tiffcodec.c : Contains function definitions for encoding decoding tiff images
 *
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Jordi Mas (jordi@ximian.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#include "config.h"
#include "codecs-private.h"
#include "tiffcodec.h"

GUID gdip_tif_image_format_guid = {0xb96b3cb1U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

#ifdef HAVE_LIBTIFF

/* Thankfully, libtiff sucks far less than libjpeg */
#include <tiffio.h>

#ifdef HAVE_BYTESWAP_H
#include <byteswap.h>
#endif

#ifndef TIFFTAG_EXIFIFD
#define	TIFFTAG_EXIFIFD	34665
#endif


/* Codecinfo related data*/
static ImageCodecInfo tiff_codec;
static const WCHAR tiff_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'T', 'I', 'F', 'F', ' ', 'C', 'o', 'd', 'e', 'c', 0}; /* Built-in TIFF Codec */
static const WCHAR tiff_extension[] = {'*', '.', 'T', 'I', 'F',';', '*', '.', 'T', 'I', 'F','F', 0}; /* *.TIF;*.TIFF */
static const WCHAR tiff_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 't', 'i', 'f', 'f', 0}; /* image/tiff */
static const WCHAR tiff_format[] = {'T', 'I', 'F', 'F', 0}; /* TIFF */
static const BYTE tiff_sig_pattern[] = { 0x49, 0x49, 0x4D, 0x4D };
static const BYTE tiff_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF };

/*Wrapper functions and client data strucutre for delegate functions from StreamHelper class of 
System.Drawing Namespace*/
typedef struct {
	GetBytesDelegate getBytesFunc;
	PutBytesDelegate putBytesFunc;
	SeekDelegate seekFunc;
	CloseDelegate closeFunc;
	SizeDelegate sizeFunc;
} gdip_tiff_clientData;

static tsize_t 
gdip_tiff_fileread (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)fread(buffer, 1, size, (FILE*)clientData);
}

static tsize_t 
gdip_tiff_filewrite (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)fwrite (buffer, 1, size, (FILE*)clientData);
}

static toff_t 
gdip_tiff_fileseek (thandle_t clientData, toff_t offSet, int whence)
{
	int seek_ok = fseek ((FILE*)clientData, offSet, whence);
	if (seek_ok == 0)
		return offSet;

	return -1;
}

static int 
gdip_tiff_fileclose (thandle_t clientData)
{
	/* This is a TIFF cleanup function; but we own the FILE* and close it in image.c so this is a null op */
	return 0;
}

static toff_t 
gdip_tiff_filesize (thandle_t clientData)
{
	long ret;
	long cur_pos = ftell ((FILE*)clientData);
	fseek ((FILE*)clientData, 0, SEEK_END);
	ret = ftell ((FILE*)clientData);
	fseek ((FILE*)clientData, cur_pos, SEEK_SET);
	return (toff_t)ret;
}

static int
gdip_tiff_filedummy_map (thandle_t clientData, tdata_t *phase, toff_t* size)
{
	return 0;
}

static void
gdip_tiff_filedummy_unmap (thandle_t clientData, tdata_t base, toff_t size)
{
}

static tsize_t 
gdip_tiff_read (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)((gdip_tiff_clientData *) clientData)->getBytesFunc (buffer, size, 0);
}

static tsize_t 
gdip_tiff_read_none (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return 0;
}

static tsize_t 
gdip_tiff_write (thandle_t clientData, tdata_t buffer, tsize_t size)
{
	return (tsize_t)((gdip_tiff_clientData *) clientData)->putBytesFunc (buffer, size);
}

static toff_t 
gdip_tiff_seek (thandle_t clientData, toff_t offSet, int whence)
{
	return (toff_t)((gdip_tiff_clientData *) clientData)->seekFunc (offSet, whence);
}

static int 
gdip_tiff_close (thandle_t clientData)
{
	/* We should not close the user provided streams */
	/* ((gdip_tiff_clientData *) clientData)->closeFunc (); */
	return 1;
}

static toff_t 
gdip_tiff_size (thandle_t clientData)
{
	return (toff_t)((gdip_tiff_clientData *) clientData)->sizeFunc ();
}

static int
gdip_tiff_dummy_map (thandle_t clientData, tdata_t *phase, toff_t* size)
{
	return 0;
}

static void
gdip_tiff_dummy_unmap (thandle_t clientData, tdata_t base, toff_t size)
{
}

ImageCodecInfo *
gdip_getcodecinfo_tiff ()
{
	tiff_codec.Clsid = (CLSID) { 0x557cf405, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	tiff_codec.FormatID = gdip_tif_image_format_guid;
	tiff_codec.CodecName = (const WCHAR*) tiff_codecname;
	tiff_codec.DllName = NULL;
	tiff_codec.FormatDescription = (const WCHAR*) tiff_format;
	tiff_codec.FilenameExtension = (const WCHAR*) tiff_extension;
	tiff_codec.MimeType = (const WCHAR*) tiff_mimetype;
	tiff_codec.Flags = ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin;
	tiff_codec.Version = 1;
	tiff_codec.SigCount = 2;
	tiff_codec.SigSize = 2;
	tiff_codec.SigPattern = tiff_sig_pattern;
	tiff_codec.SigMask = tiff_sig_mask;

	/* Mute the lib */
	TIFFSetErrorHandler(NULL);
	TIFFSetWarningHandler(NULL);
	
	return &tiff_codec;
}

static GpStatus
gdip_load_tiff_properties (TIFF *tiff, ActiveBitmapData *bitmap_data)
{
	BYTE *text;
	uint32	i;
	uint16	s;
	uint16	s2;
	double	d;
	float	f;
	uint16	samples_per_pixel;
	uint16	bits_per_sample;
	uint16	planar_configuration;
	uint32	image_length;
	uint16	strips_per_image;
	uint32	rows_per_strip;
	uint32	tile_length;
	uint32	tile_width;
	uint16 compression = 0;

	samples_per_pixel = 0;
	bits_per_sample = 0;
	planar_configuration = 0;
	image_length = 0;
	strips_per_image = 0;
	rows_per_strip = 0;
	tile_length = 0;
	tile_width = 0;
	i = 0;
	s = 0;
	s2 = 0;

	if (TIFFGetField(tiff, TIFFTAG_ARTIST, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagArtist, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE, &bits_per_sample)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagBitsPerSample, bits_per_sample);
	}

	{
		uint16 *rmap;
		uint16 *gmap;
		uint16 *bmap;

		if (TIFFGetField(tiff, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap)) {
			BYTE *buffer;
			uint16		*ptr;

			if ((rmap != NULL) && (gmap != NULL) && (bmap != NULL)) {
				buffer = GdipAlloc (3 * bits_per_sample * sizeof (uint16));
				if (buffer != NULL)  {
					ptr = (uint16 *)buffer;

					for (i = 0; i < bits_per_sample; i++) {
						ptr[0] = rmap[i];
						ptr[1] = gmap[i];
						ptr[2] = bmap[i];
						ptr += 3;
					}
					gdip_bitmapdata_property_add(bitmap_data, PropertyTagColorMap, 
						3 * bits_per_sample * sizeof(uint16), PropertyTagTypeShort, buffer);
					GdipFree(buffer);
				}
			}
		}
	}

	if (TIFFGetField(tiff, TIFFTAG_COMPRESSION, &s)) {
		compression = s;
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagCompression, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_COPYRIGHT, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagCopyright, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_DATETIME, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagDateTime, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_DOCUMENTNAME, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagDocumentName, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_DOTRANGE, &s, &s2)) {
		gdip_bitmapdata_property_add_srational(bitmap_data, PropertyTagDotRange, s, s2);
	}

	{
		uint16	count;
		uint16	*samples;

		if (TIFFGetField(tiff, TIFFTAG_EXTRASAMPLES, &count, &samples)) {
			if ((count > 0) && (samples != NULL)) {
				gdip_bitmapdata_property_add(bitmap_data, PropertyTagExtraSamples, count * sizeof(uint16), 
					PropertyTagTypeShort, samples);
			}
		}
	}

	if (TIFFGetField(tiff, TIFFTAG_FILLORDER, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagFillOrder, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_GROUP3OPTIONS, &i, &i)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagT4Option, i);
	}

	if (TIFFGetField(tiff, TIFFTAG_GROUP4OPTIONS, &i)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagT6Option, i);
	}

	if (TIFFGetField(tiff, TIFFTAG_HALFTONEHINTS, &s, &s2)) {
		gdip_bitmapdata_property_add_srational(bitmap_data, PropertyTagHalftoneHints, s, s2);
	}

	if (TIFFGetField(tiff, TIFFTAG_HOSTCOMPUTER, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagHostComputer, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_IMAGEDESCRIPTION, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagImageDescription, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &i)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagImageWidth, i);
	}

	if (TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &image_length)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagImageHeight, image_length);
	}

	if (TIFFGetField(tiff, TIFFTAG_INKNAMES, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagInkNames, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_INKSET, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagInkSet, s);
	}


#ifdef NotImplemented
	/* Don't know how this property should be stored, datatype is void */
	{
		uint32	count;
		void	*tables;

		if (TIFFGetField(tiff, TIFFTAG_JPEGTABLES, &count, &tables)) {
			gdip_bitmapdata_property_add(bitmap_data, PropertyTagJPEGTables, text);
		}
	}
#endif

	if (TIFFGetField(tiff, TIFFTAG_JPEGQUALITY, &i)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagJPEGQuality, i);
	}

	if (TIFFGetField(tiff, TIFFTAG_MAKE, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagEquipMake, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_MAXSAMPLEVALUE, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagMaxSampleValue, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_MINSAMPLEVALUE, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagMinSampleValue, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_MODEL, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagEquipModel, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_ORIENTATION, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagOrientation, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_PAGENAME, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagPageName, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_PAGENUMBER, &s, &s2)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagPageNumber, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_PHOTOMETRIC, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagPhotometricInterp, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_PLANARCONFIG, &planar_configuration)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagPlanarConfig, planar_configuration);
	}

	if (compression == COMPRESSION_ADOBE_DEFLATE) {
		if (TIFFGetField(tiff, TIFFTAG_PREDICTOR, &s)) {
			gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagPredictor, s);
		}
	}

	{
		float *chromacities = NULL;	/* 6-entry array */

		if (TIFFGetField(tiff, TIFFTAG_PRIMARYCHROMATICITIES, &chromacities) && (chromacities != NULL)) {
			BYTE *buffer;
			uint32		*ptr;

			buffer = GdipAlloc(6 * (sizeof(uint32) + sizeof(uint32)));
			if (buffer != NULL)  {
				ptr = (uint32 *)buffer;

				for (i = 0; i < 6; i++) {
					ptr[0] = (uint32)(chromacities[i] * 1000000);
					ptr[1] = 1000000;
					ptr += 2;
				}
				gdip_bitmapdata_property_add (bitmap_data, PropertyTagPrimaryChromaticities, 
					6 * (sizeof(uint32) + sizeof(uint32)), PropertyTagTypeRational, buffer);
				GdipFree(buffer);
			}
		}
	}

	if (TIFFGetField(tiff, TIFFTAG_RESOLUTIONUNIT, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagResolutionUnit, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_ROWSPERSTRIP, &rows_per_strip)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagRowsPerStrip, rows_per_strip);
	}

	if (TIFFGetField(tiff, TIFFTAG_SAMPLEFORMAT, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagSampleFormat, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagSamplesPerPixel, samples_per_pixel);
	}

	if (samples_per_pixel > 0) {
		float *ref_blackwhite;	/* 2 * SamplesPerPixel array */

		if (TIFFGetField(tiff, TIFFTAG_REFERENCEBLACKWHITE, &ref_blackwhite)) {
			BYTE *buffer;
			uint32		*ptr;

			buffer = GdipAlloc(2 * samples_per_pixel * (sizeof(uint32) + sizeof(uint32)));
			if (buffer != NULL)  {
				ptr = (uint32 *)buffer;

				for (i = 0; i < 2 * samples_per_pixel; i++) {
					ptr[0] = (uint32)(ref_blackwhite[i] * 1000000);
					ptr[1] = 1000000;
					ptr += 2;
				}
				gdip_bitmapdata_property_add (bitmap_data, PropertyTagREFBlackWhite, 
					6 * (sizeof(uint32) + sizeof(uint32)), PropertyTagTypeRational, buffer);
				GdipFree(buffer);
			}
		}
	}


	if (TIFFGetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, &d)) {
		gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagSMaxSampleValue, d * 1000000, 1000000);
	}

	if (TIFFGetField(tiff, TIFFTAG_SMINSAMPLEVALUE, &d)) {
		gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagSMinSampleValue, d * 1000000, 1000000);
	}

	if (TIFFGetField(tiff, TIFFTAG_SOFTWARE, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagSoftwareUsed, text);
	}

	if ((rows_per_strip != 0) && (planar_configuration != 0)) {
		uint32	*bytecounts;
		uint32	*offsets;
		int	count;

		strips_per_image = floor ((image_length + rows_per_strip - 1) / rows_per_strip);

		if (planar_configuration == 1) {
			count = strips_per_image;
		} else {
			count = samples_per_pixel * strips_per_image;
		}

		if (TIFFGetField(tiff, TIFFTAG_STRIPBYTECOUNTS, &bytecounts)) {
			gdip_bitmapdata_property_add(bitmap_data, PropertyTagStripBytesCount, 
				count * sizeof(uint32), PropertyTagTypeLong, bytecounts);
		}

		if (TIFFGetField(tiff, TIFFTAG_STRIPOFFSETS, &offsets)) {
			gdip_bitmapdata_property_add(bitmap_data, PropertyTagStripOffsets, 
				count * sizeof(uint32), PropertyTagTypeLong, offsets);
		}
	}

	if (TIFFGetField(tiff, TIFFTAG_SUBFILETYPE, &i)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagNewSubfileType, i);
	}

	if (TIFFGetField(tiff, TIFFTAG_TARGETPRINTER, &text)) {
		gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagTargetPrinter, text);
	}

	if (TIFFGetField(tiff, TIFFTAG_THRESHHOLDING, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagThreshHolding, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_TILEWIDTH, &tile_width)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagTileWidth, tile_width);
	}

	if (TIFFGetField(tiff, TIFFTAG_TILELENGTH, &tile_length)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagTileLength, tile_length);
	}

	if ((planar_configuration != 0) && (tile_width != 0) && (tile_length != 0)) {
		uint32	*byte_counts;
		uint32	*offsets;
		uint32	tiles_across;
		uint32	tiles_down;
		int	tiles_per_image;
		int	count;

		tiles_across = (image_length + tile_width - 1) / tile_width;
		tiles_down = (image_length + tile_length - 1) / tile_length;
		tiles_per_image = tiles_across * tiles_down;

		if (planar_configuration == 1) {
			count = tiles_per_image;
		} else {
			count = samples_per_pixel * tiles_per_image;
		}

		if (TIFFGetField(tiff, TIFFTAG_TILEBYTECOUNTS, &byte_counts)) {
			gdip_bitmapdata_property_add(bitmap_data, PropertyTagTileByteCounts, count * sizeof(uint32), 
				PropertyTagTypeLong, byte_counts);
		}

		if (TIFFGetField(tiff, TIFFTAG_TILEOFFSETS, &offsets)) {
			gdip_bitmapdata_property_add(bitmap_data, PropertyTagTileOffset, count * sizeof(uint32), 
				PropertyTagTypeLong, offsets);
		}
	}

	if (samples_per_pixel == 1) {
		uint16	*sample;

		if (TIFFGetField(tiff, TIFFTAG_TRANSFERFUNCTION, &sample)) {
			gdip_bitmapdata_property_add (bitmap_data, PropertyTagTransferFuncition, 
				(1 << bits_per_sample) * (ULONG) sizeof(uint16), PropertyTagTypeShort, sample);
		}
	} else if (samples_per_pixel == 3) {
		uint16	*r;
		uint16	*g;
		uint16	*b;

		if (TIFFGetField(tiff, TIFFTAG_TRANSFERFUNCTION, &r, &g, &b)) {
			BYTE *buffer;
			uint16		*ptr;

			buffer = GdipAlloc(3 * (1 << samples_per_pixel) *  sizeof(uint16));
			if (buffer != NULL)  {
				ptr = (uint16 *)buffer;

				for (i = 0; i < 1 << bits_per_sample; i++) {
					ptr[i] = r[i];
					ptr[i + 1] = g[i];
					ptr[i + 2] = b[i];
				}
				gdip_bitmapdata_property_add (bitmap_data, PropertyTagTransferFuncition, 
					3 * (1 << samples_per_pixel) * sizeof(uint16), PropertyTagTypeShort, buffer);
				GdipFree(buffer);
			}
		}
	}

	{
		float *whitepoints;

		if (TIFFGetField(tiff, TIFFTAG_WHITEPOINT, &whitepoints)) {
			BYTE *buffer;
			uint32		*ptr;

			buffer = GdipAlloc(2 * (sizeof(uint32) + sizeof(uint32)));
			if (buffer != NULL)  {
				ptr = (uint32 *)buffer;

				ptr[0] = whitepoints[0] * 1000000;
				ptr[1] = 1000000;

				ptr[2] = whitepoints[1] * 1000000;
				ptr[3] = 1000000;

				gdip_bitmapdata_property_add (bitmap_data, PropertyTagTransferFuncition, 
					2 * (sizeof(uint32) + sizeof(uint32)), PropertyTagTypeRational, buffer);
				GdipFree(buffer);
			}

			gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagWhitePoint, whitepoints[0] * 1000000, 1000000);
		}
	}

	if (TIFFGetField(tiff, TIFFTAG_XPOSITION, &f)) {
		gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagXPosition, f * 1000000, 1000000);
	}

	if (TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &f)) {
		gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagXResolution, f, 1);
	}

	{
		float	*coefficients;

		if (TIFFGetField(tiff, TIFFTAG_YCBCRCOEFFICIENTS, &coefficients)) {
			BYTE buffer[sizeof(uint32) * 6];
			uint32		*ptr;

			ptr = (uint32 *)&buffer;
			ptr[0] = (uint32)(coefficients[0] * 1000000);
			ptr[1] = 1000000;
			ptr[2] = (uint32)(coefficients[1] * 1000000);
			ptr[3] = 1000000;
			ptr[4] = (uint32)(coefficients[2] * 1000000);
			ptr[5] = 1000000;
			
			gdip_bitmapdata_property_add(bitmap_data, PropertyTagYCbCrCoefficients, sizeof(uint32) * 6, 
				PropertyTagTypeRational, buffer);
		}
	}

	if (TIFFGetField(tiff, TIFFTAG_YCBCRPOSITIONING, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagYCbCrPositioning, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_YCBCRSUBSAMPLING, &s, &s2)) {
		gdip_bitmapdata_property_add_srational(bitmap_data, PropertyTagYCbCrSubsampling, s, s2);
	}

	if (TIFFGetField(tiff, TIFFTAG_YPOSITION, &f)) {
		gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagYPosition, f * 1000000, 1000000);
	}

	if (TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &f)) {
		gdip_bitmapdata_property_add_rational(bitmap_data, PropertyTagYResolution, f, 1);
	}

#ifdef NotImplemented
	/* Not sure what type the data is */
	{
		uint32	count;
		void	*profile_data;

		if (TIFFGetField(tiff, TIFFTAG_ICCPROFILE, &count, &profile_data)) {
			gdip_bitmapdata_property_add(bitmap_data, PropertyTagICCProfile, count * sizeof(void *), , profile_data);
		}
	}
#endif

	/* EXIF section */
	if (TIFFGetField(tiff, TIFFTAG_EXIFIFD, &i)) {
		gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagExifIFD, i);
	}

	return Ok;
}

static GpStatus
gdip_save_tiff_properties (TIFF *tiff, ActiveBitmapData *bitmap_data, int samples_per_pixel, int bits_per_sample)
{
	int		index;
	guint32		i;
	guint32		l;
	int		j;
	uint16		s;
	uint16		s2;
	double		d;
	float		f;

	i = 0;
	s = 0;
	s2 = 0;
	d = 0;

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagArtist, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_ARTIST, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagCopyright, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_COPYRIGHT, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagDateTime, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_DATETIME, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagDocumentName, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_DOCUMENTNAME, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagDotRange, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		gdip_property_get_short(2, bitmap_data->property[index].value, &s2);
		TIFFSetField(tiff, TIFFTAG_DOTRANGE, s, s2);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagExtraSamples, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_EXTRASAMPLES, bitmap_data->property[index].length, bitmap_data->property[index].value);
	} else {
		TIFFSetField (tiff, TIFFTAG_EXTRASAMPLES, 1, EXTRASAMPLE_UNSPECIFIED);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagFillOrder, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_FILLORDER, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagT4Option, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &l);
		TIFFSetField(tiff, TIFFTAG_GROUP3OPTIONS, l);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagT6Option, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &l);
		TIFFSetField(tiff, TIFFTAG_GROUP4OPTIONS, l);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagHalftoneHints, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		gdip_property_get_short(2, bitmap_data->property[index].value, &s2);
		TIFFSetField(tiff, TIFFTAG_HALFTONEHINTS, s, s2);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagHostComputer, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_HOSTCOMPUTER, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagImageDescription, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_IMAGEDESCRIPTION, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagInkNames, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_INKNAMES, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagInkSet, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_INKSET, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagJPEGQuality, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &l);
		TIFFSetField(tiff, TIFFTAG_JPEGQUALITY, l);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagEquipMake, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_MAKE, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagMaxSampleValue, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_MAXSAMPLEVALUE, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagMinSampleValue, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_MINSAMPLEVALUE, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagEquipModel, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_MODEL, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagOrientation, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_ORIENTATION, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagPageName, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_PAGENAME, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagPhotometricInterp, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagPredictor, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_PREDICTOR, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagPrimaryChromaticities, &index) == Ok) {
		float	chromacities[6];
		for (j = 0; j < 6; j++) {
			gdip_property_get_long(j * 8, bitmap_data->property[index].value, &i);
			chromacities[j] = (float)i / 1000000;
		}

		TIFFSetField(tiff, TIFFTAG_PRIMARYCHROMATICITIES, &chromacities);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagResolutionUnit, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_RESOLUTIONUNIT, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagSampleFormat, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, s);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagREFBlackWhite, &index) == Ok) {
		float *ref_blackwhite;	/* 2 * SamplesPerPixel array */

		ref_blackwhite = GdipAlloc(2 * samples_per_pixel * sizeof(float));
		if (ref_blackwhite != NULL) {
			for (j = 0; j < (2 * samples_per_pixel); j++) {
				gdip_property_get_long(j * 8, bitmap_data->property[index].value, &i);
				ref_blackwhite[j] =  (float)i / 1000000;
			}
			TIFFSetField(tiff, TIFFTAG_REFERENCEBLACKWHITE, &ref_blackwhite);
			GdipFree(ref_blackwhite);
		}
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagSMaxSampleValue, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		d = (double)i / 10000000;
		TIFFSetField(tiff, TIFFTAG_SMAXSAMPLEVALUE, d);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagSMinSampleValue, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		d = (double)i / 10000000;
		TIFFSetField(tiff, TIFFTAG_SMINSAMPLEVALUE, d);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagSoftwareUsed, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_SOFTWARE, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagNewSubfileType, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_SUBFILETYPE, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagNewSubfileType, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &l);
		TIFFSetField(tiff, TIFFTAG_SUBFILETYPE, l);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagTargetPrinter, &index) == Ok) {
		TIFFSetField(tiff, TIFFTAG_TARGETPRINTER, bitmap_data->property[index].value);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagThreshHolding, &index) == Ok) {
		gdip_property_get_short(0, bitmap_data->property[index].value, &s);
		TIFFSetField(tiff, TIFFTAG_THRESHHOLDING, s);
	}


	if (samples_per_pixel == 1) {
		if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagTransferFuncition, &index) == Ok) {
			TIFFSetField(tiff, TIFFTAG_TRANSFERFUNCTION, bitmap_data->property[index].value);
		}
	} else if (samples_per_pixel == 3) {
		if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagTransferFuncition, &index) == Ok) {
			uint16	*rmap;
			uint16	*gmap;
			uint16	*bmap;

			rmap = GdipAlloc ((1 << samples_per_pixel) * (ULONG) sizeof (uint16));
			gmap = GdipAlloc ((1 << samples_per_pixel) * (ULONG) sizeof (uint16));
			bmap = GdipAlloc ((1 << samples_per_pixel) * (ULONG) sizeof (uint16));

			if ((rmap != NULL) && (gmap != NULL) && (bmap != NULL)) {
				for (j = 0; j < 1 << bits_per_sample; j++) {
					gdip_property_get_short((j * 3), bitmap_data->property[index].value, &rmap[j]);
					gdip_property_get_short((j * 3) + 1, bitmap_data->property[index].value, &gmap[j]);
					gdip_property_get_short((j * 3) + 2, bitmap_data->property[index].value, &bmap[j]);
				}
				TIFFSetField(tiff, TIFFTAG_TRANSFERFUNCTION, rmap, gmap, bmap);
			}
			if (rmap != NULL) {
				GdipFree(rmap);
			}

			if (gmap != NULL) {
				GdipFree(gmap);
			}

			if (bmap != NULL) {
				GdipFree(bmap);
			}
		}
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagXPosition, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		f = (float)i / 10000000;
		TIFFSetField(tiff, TIFFTAG_XPOSITION, f);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagXResolution, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		f = (float)i;
		TIFFSetField(tiff, TIFFTAG_XRESOLUTION, f);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagYCbCrCoefficients, &index) == Ok) {
		float	coefficients[3];

		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		coefficients[0] = (float)i / 1000000;

		gdip_property_get_long(8, bitmap_data->property[index].value, &i);
		coefficients[1] = (float)i / 1000000;

		gdip_property_get_long(16, bitmap_data->property[index].value, &i);
		coefficients[2] = (float)i / 1000000;

		TIFFSetField(tiff, TIFFTAG_YCBCRCOEFFICIENTS, coefficients);
	}

	if (TIFFGetField(tiff, TIFFTAG_YCBCRPOSITIONING, &s)) {
		gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagYCbCrPositioning, s);
	}

	if (TIFFGetField(tiff, TIFFTAG_YCBCRSUBSAMPLING, &s, &s2)) {
		gdip_bitmapdata_property_add_srational(bitmap_data, PropertyTagYCbCrSubsampling, s, s2);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagYPosition, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		f = (float)i / 1000000;
		TIFFSetField(tiff, TIFFTAG_YPOSITION, f);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagYResolution, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		f = (float)i;
		TIFFSetField(tiff, TIFFTAG_YRESOLUTION, f);
	}

	if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagExifIFD, &index) == Ok) {
		gdip_property_get_long(0, bitmap_data->property[index].value, &i);
		TIFFSetField(tiff, TIFFTAG_EXIFIFD, d);
	}

	return Ok;
}

/*TODO Handle TIFF Encoder Parameters*/
static GpStatus 
gdip_save_tiff_image (TIFF* tiff, GpImage *image, GDIPCONST EncoderParameters *params)
{
	int		frame;
	int		x;
	int		y;
	int		i;
	int		num_of_pages;
	int		page;
	ActiveBitmapData	*bitmap_data;
	BYTE		*pixbuf;
	int		samples_per_pixel;
	int		bits_per_sample;
	unsigned long long int size;

	if (tiff == NULL) {
		return InvalidParameter;
	}

	/* Count all pages, we need to know ahead */
	num_of_pages = 0;
	for (frame = 0; frame < image->num_of_frames; frame++) {
		num_of_pages += image->frames[frame].count;
		for (i = 0; i < image->frames[frame].count; i++) {
			if (gdip_is_an_indexed_pixelformat (image->frames[frame].bitmap[i].pixel_format)) {
				return NotImplemented; /* FIXME? */
			}
		}
	}

	page = 0;
	for (frame = 0; frame < image->num_of_frames; frame++) {
		for (i = 0; i < image->frames[frame].count; i++) {
			bitmap_data = &image->frames[frame].bitmap[i];

			if (num_of_pages > 1) {
				if ((frame > 0) && (i > 0)) {
					TIFFCreateDirectory(tiff);
				}

				TIFFSetField (tiff, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
				TIFFSetField (tiff, TIFFTAG_PAGENUMBER, page, num_of_pages);
			}

			if (((bitmap_data->pixel_format & PixelFormatAlpha) != 0) || (bitmap_data->pixel_format == PixelFormat32bppRGB)) {
				samples_per_pixel = 4;
				bits_per_sample = 8;
			} else {
				samples_per_pixel = 3;
				bits_per_sample = 8;
			}
			gdip_save_tiff_properties(tiff, bitmap_data, samples_per_pixel, bits_per_sample);

			TIFFSetField (tiff, TIFFTAG_SAMPLESPERPIXEL, samples_per_pixel);
			TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH, bitmap_data->width);
			TIFFSetField (tiff, TIFFTAG_IMAGELENGTH, bitmap_data->height);
			TIFFSetField (tiff, TIFFTAG_BITSPERSAMPLE, bits_per_sample);
			TIFFSetField (tiff, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
			TIFFSetField (tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
			TIFFSetField (tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
			TIFFSetField (tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize (tiff, bitmap_data->stride));
			TIFFSetField (tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

			size = (unsigned long long int)bitmap_data->width * samples_per_pixel;
			if (size > G_MAXINT32) {
				goto error;
			}

			pixbuf = GdipAlloc (size);
			if (pixbuf == NULL) {
				goto error;
			}
			if (samples_per_pixel == 4) {
				for (y = 0; y < bitmap_data->height; y++) {
					for (x = 0; x < bitmap_data->width; x++) {
#ifdef WORDS_BIGENDIAN
						pixbuf[x*4+0] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 1);
						pixbuf[x*4+1] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 2);
						pixbuf[x*4+2] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 3);
						pixbuf[x*4+3] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 0);
#else
						pixbuf[x*4+0] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 2);
						pixbuf[x*4+1] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 1);
						pixbuf[x*4+2] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 0);
						pixbuf[x*4+3] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 3);
#endif
					}
					TIFFWriteScanline (tiff, pixbuf, y, 0);
				}
			} else {
				for (y = 0; y < bitmap_data->height; y++) {
					for (x = 0; x < bitmap_data->width; x++) {
#ifdef WORDS_BIGENDIAN
						pixbuf[x*3+0] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 1);
						pixbuf[x*3+1] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 2);
						pixbuf[x*3+2] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 3);
#else
						pixbuf[x*3+0] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 2);
						pixbuf[x*3+1] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 1);
						pixbuf[x*3+2] = *((BYTE*)bitmap_data->scan0 + (bitmap_data->stride * y) + (x*4) + 0);
#endif
					}
					TIFFWriteScanline (tiff, pixbuf, y, 0);
				}
			}
			GdipFree(pixbuf);
			TIFFWriteDirectory (tiff);
			page++;
		}	
	}
	TIFFClose (tiff);
	return Ok;

error:
	TIFFClose (tiff);
	return OutOfMemory;
}


static GpStatus 
gdip_load_tiff_image (TIFF *tiff, GpImage **image)
{
	int		i;
	char		error_message[1024];
	int		num_of_pages;
	GpImage		*result;
	int		page;
	TIFFRGBAImage	tiff_image;
	FrameData	*frame;
	ActiveBitmapData	*bitmap_data;
	char		*pixbuf;
	char		*pixbuf_row;
	guint32		*pixbuf_ptr;
	guint16		samples_per_pixel;
	float		dpi;

	if (tiff == NULL) {
		*image = NULL;
		/* we cannot call TIFFClose(tiff); with a NULL value since it will crash - bnc #569940 */
		return OutOfMemory;
	}

	result = NULL;
	pixbuf_row = NULL;
	pixbuf = NULL;
	memset (&tiff_image, 0, sizeof (TIFFRGBAImage));

	num_of_pages = TIFFNumberOfDirectories(tiff);

	/* Handle cases where there are too many directories or there is a infinite loop in the directory structure.
	 * This relies on libtiff returning 65535 in the error case, which has been the case since v4.0.4 released in 2015. */
	if (num_of_pages >= 65535)
		goto error;

	result = gdip_bitmap_new();
	if (!result)
		goto error;

	result->type = ImageTypeBitmap;
	frame = gdip_frame_add(result, &gdip_image_frameDimension_page_guid);
	if (!frame)
		goto error;

	for (page = 0; page < num_of_pages; page++) {
		unsigned long long int size;

		bitmap_data = gdip_frame_add_bitmapdata(frame);
		if (bitmap_data == NULL) {
			goto error;
		}

		if (!TIFFSetDirectory(tiff, page)) {
			goto error;
		}

		gdip_load_tiff_properties(tiff, bitmap_data);

		if (!TIFFRGBAImageBegin (&tiff_image, tiff, 0, error_message)) {
			goto error;
		}

		if (TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel)) {
			if (samples_per_pixel != 4) {
				bitmap_data->pixel_format = PixelFormat24bppRGB;
			} else {
				bitmap_data->pixel_format = PixelFormat32bppARGB;
				bitmap_data->image_flags |= ImageFlagsHasAlpha;
			}
		}

		if (TIFFGetField(tiff, TIFFTAG_XRESOLUTION, &dpi)) {
			bitmap_data->dpi_horz = dpi;
		} else {
			bitmap_data->dpi_horz = 0;
		}

		if (TIFFGetField(tiff, TIFFTAG_YRESOLUTION, &dpi)) {
			bitmap_data->dpi_vert = dpi;
		} else {
			bitmap_data->dpi_vert = 0;
		}

		if (bitmap_data->dpi_horz && bitmap_data->dpi_vert)
			bitmap_data->image_flags |= ImageFlagsHasRealDPI;

		/* width and height are uint32, but TIFF uses 32 bits offsets (so it's real size limit is 4GB),
		 * however libtiff uses signed int (int32 not uint32) as offsets so we limit ourselves to 2GB */
		size = tiff_image.width;
		/* stride is a (signed) _int_ and once multiplied by 4 it should hold a value that can be allocated by GdipAlloc
		 * this effectively limits 'width' to 536870911 pixels */
		size *= sizeof (guint32);
		if (size > G_MAXINT32)
			goto error;
		bitmap_data->stride = size;
		bitmap_data->width = tiff_image.width;
		bitmap_data->height = tiff_image.height;
		bitmap_data->reserved = GBD_OWN_SCAN0;
		bitmap_data->image_flags |= ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly;

		/* ensure total 'size' does not overflow an integer and fits inside our 2GB limit */
		size *= tiff_image.height;
		if (size > G_MAXINT32)
			goto error;
		pixbuf = GdipAlloc (size);
		if (pixbuf == NULL) {
			goto error;
		}

		/* Flip the image. TIFF has its origin at bottom left, and is in ARGB instead of ABGR */
		if (!TIFFRGBAImageGet(&tiff_image, (uint32 *)pixbuf, tiff_image.width, tiff_image.height)) {
			goto error;
		}

		pixbuf_row = GdipAlloc(bitmap_data->stride);
		if (pixbuf_row == NULL) {
			goto error;
		}

		/* First, flip rows */
		for (i = 0; i < tiff_image.height / 2; i++) {
			memcpy(pixbuf_row, pixbuf + (bitmap_data->stride * i), bitmap_data->stride);
			memcpy(pixbuf + (bitmap_data->stride * i), pixbuf + (bitmap_data->stride * (tiff_image.height - i - 1)), bitmap_data->stride);
			memcpy(pixbuf + (bitmap_data->stride * (tiff_image.height - i - 1)), pixbuf_row, bitmap_data->stride);
		}

		/* Now flip from ARGB to ABGR processing one pixel (4 bytes) at the time */
		pixbuf_ptr = (guint32 *)pixbuf;
		for (i = 0; i < (size >> 2); i++) {
			*pixbuf_ptr =	(*pixbuf_ptr & 0xff000000) | 
					((*pixbuf_ptr & 0x00ff0000) >> 16) |
					(*pixbuf_ptr & 0x0000ff00) | 
					((*pixbuf_ptr & 0x000000ff) << 16);
			pixbuf_ptr++;
		}
		GdipFree(pixbuf_row);
		bitmap_data->scan0 = (BYTE*) pixbuf;
		pixbuf_row = NULL;
		pixbuf = NULL;

		TIFFRGBAImageEnd (&tiff_image);
	}

	gdip_bitmap_setactive(result, &gdip_image_frameDimension_page_guid, 0);

	TIFFClose(tiff);

	*image = result;
	return Ok;

error:
	/* coverity[dead_error_line] */
	if (pixbuf_row != NULL) {
		GdipFree(pixbuf_row);
	}

	if (pixbuf != NULL) {
		GdipFree(pixbuf);
	}

	if (result != NULL) {
		gdip_bitmap_dispose(result);
	}

	TIFFRGBAImageEnd (&tiff_image);
	TIFFClose(tiff);

	return OutOfMemory;
}

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image)
{
	TIFF *tif = NULL;
	
	tif = TIFFClientOpen("<stream>", "r", (thandle_t) fp, gdip_tiff_fileread, 
				gdip_tiff_filewrite, gdip_tiff_fileseek, gdip_tiff_fileclose, 
				gdip_tiff_filesize, gdip_tiff_filedummy_map, gdip_tiff_filedummy_unmap);
	return gdip_load_tiff_image (tif, image);
}

GpStatus 
gdip_save_tiff_image_to_file (BYTE *filename, GpImage *image, GDIPCONST EncoderParameters *params)
{	
	TIFF* tiff;
	
	tiff = TIFFOpen((char *)filename, "w");

	if (!tiff)
		return FileNotFound;		
		
	return gdip_save_tiff_image (tiff, image, params);
}

GpStatus
gdip_load_tiff_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
					PutBytesDelegate putBytesFunc,
					SeekDelegate seekFunc,
					CloseDelegate closeFunc,
					SizeDelegate sizeFunc,
					GpImage **image)
{
	TIFF *tif = NULL;
	gdip_tiff_clientData clientData;
	
	clientData.getBytesFunc = getBytesFunc;
	clientData.putBytesFunc = putBytesFunc;
	clientData.seekFunc = seekFunc;
	clientData.closeFunc = closeFunc;
	clientData.sizeFunc = sizeFunc;
	
	tif = TIFFClientOpen("<stream>", "r", (thandle_t) &clientData, gdip_tiff_read, 
				gdip_tiff_write, gdip_tiff_seek, gdip_tiff_close, 
				gdip_tiff_size, gdip_tiff_dummy_map, gdip_tiff_dummy_unmap);
	
	return gdip_load_tiff_image (tif, image);
}

GpStatus
gdip_save_tiff_image_to_stream_delegate (GetBytesDelegate getBytesFunc,
					PutBytesDelegate putBytesFunc,
					SeekDelegate seekFunc,
					CloseDelegate closeFunc,
					SizeDelegate sizeFunc,
					GpImage *image,
					GDIPCONST EncoderParameters *params)
{
	TIFF* tiff;
	gdip_tiff_clientData clientData;
	
	clientData.getBytesFunc = getBytesFunc;
	clientData.putBytesFunc = putBytesFunc;
	clientData.seekFunc = seekFunc;
	clientData.closeFunc = closeFunc;
	clientData.sizeFunc = sizeFunc;
	
	tiff = TIFFClientOpen("<stream>", "w", (thandle_t) &clientData, gdip_tiff_read_none, 
			gdip_tiff_write, gdip_tiff_seek, gdip_tiff_close, 
			gdip_tiff_size, gdip_tiff_dummy_map, gdip_tiff_dummy_unmap);
	if (!tiff)
		return InvalidParameter;		
		
	return gdip_save_tiff_image (tiff, image, params);
}

#else

/* no libtiff */

#include "image.h"

ImageCodecInfo *
gdip_getcodecinfo_tiff ()
{
	return NULL;
}

GpStatus 
gdip_load_tiff_image_from_file (FILE *fp, GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

GpStatus
gdip_load_tiff_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
					PutBytesDelegate putBytesFunc,
					SeekDelegate seekFunc,
					CloseDelegate closeFunc,
					SizeDelegate sizeFunc,
					GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

GpStatus 
gdip_save_tiff_image_to_file (BYTE *filename, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return UnknownImageFormat;
}

GpStatus
gdip_save_tiff_image_to_stream_delegate (GetBytesDelegate getBytesFunc,
					PutBytesDelegate putBytesFunc,
					SeekDelegate seekFunc,
					CloseDelegate closeFunc,
					SizeDelegate sizeFunc,
					GpImage *image,
					GDIPCONST EncoderParameters *params)
{
    return UnknownImageFormat;
}
#endif

GpStatus
gdip_fill_encoder_parameter_list_tiff (EncoderParameters *buffer, UINT size)
{
	TiffEncoderParameters *tiffBuffer = (TiffEncoderParameters *) buffer;

	if (!buffer || size != sizeof (TiffEncoderParameters))
		return InvalidParameter;

	tiffBuffer->count = 4;

	tiffBuffer->compression.Guid = GdipEncoderCompression;
	tiffBuffer->compression.NumberOfValues = 5;
	tiffBuffer->compression.Type = EncoderParameterValueTypeLong;
	tiffBuffer->compressionData[0] = EncoderValueCompressionLZW;
	tiffBuffer->compressionData[1] = EncoderValueCompressionCCITT3;
	tiffBuffer->compressionData[2] = EncoderValueCompressionRle;
	tiffBuffer->compressionData[3] = EncoderValueCompressionCCITT4;
	tiffBuffer->compressionData[4] = EncoderValueCompressionNone;
	tiffBuffer->compression.Value = &tiffBuffer->compressionData;

	tiffBuffer->colorDepth.Guid = GdipEncoderColorDepth;
	tiffBuffer->colorDepth.NumberOfValues = 5;
	tiffBuffer->colorDepth.Type = EncoderParameterValueTypeLong;
	tiffBuffer->colorDepthData[0] = 1;
	tiffBuffer->colorDepthData[1] = 4;
	tiffBuffer->colorDepthData[2] = 8;
	tiffBuffer->colorDepthData[3] = 24;
	tiffBuffer->colorDepthData[4] = 32;
	tiffBuffer->colorDepth.Value = &tiffBuffer->colorDepthData;

	tiffBuffer->saveFlag.Guid = GdipEncoderSaveFlag;
	tiffBuffer->saveFlag.NumberOfValues = 1;
	tiffBuffer->saveFlag.Type = EncoderParameterValueTypeLong;
	tiffBuffer->saveFlagValue = EncoderValueMultiFrame;
	tiffBuffer->saveFlag.Value = &tiffBuffer->saveFlagValue;

	tiffBuffer->saveAsCYMK.Guid = GdipEncoderSaveAsCMYK;
	tiffBuffer->saveAsCYMK.NumberOfValues = 1;
	tiffBuffer->saveAsCYMK.Type = EncoderParameterValueTypeLong;
	tiffBuffer->saveAsCYMKValue = 1;
	tiffBuffer->saveAsCYMK.Value = &tiffBuffer->saveAsCYMKValue;

	return Ok;
}
