/*
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
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
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "gdiplus-private.h"
#include "icocodec.h"

GUID gdip_ico_image_format_guid = {0xb96b3cb5U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

/* Codecinfo related data*/
static ImageCodecInfo ico_codec;
static const WCHAR ico_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'I', 'C', 'O', ' ', 'C', 'o', 'd', 'e', 'c', 0}; /* Built-in ICO */
static const WCHAR ico_extension[] = {'*','.','I', 'C', 'O', 0}; /* *.ICO */
static const WCHAR ico_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'x', '-', 'i', 'c', 'o', 'n', 0}; /* image/x-icon */
static const WCHAR ico_format[] = {'I', 'C', 'O', 0}; /* ICO */
static const BYTE ico_sig_pattern[] = { 0x00, 0x00, 0x01, 0x00 };
static const BYTE ico_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF };


ImageCodecInfo*
gdip_getcodecinfo_ico ()
{
	ico_codec.Clsid = (CLSID) { 0x557cf407, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	ico_codec.FormatID = gdip_ico_image_format_guid;
	ico_codec.CodecName = (const WCHAR*) ico_codecname;            
	ico_codec.DllName = NULL;
	ico_codec.FormatDescription = (const WCHAR*) ico_format;
	ico_codec.FilenameExtension = (const WCHAR*) ico_extension;
	ico_codec.MimeType = (const WCHAR*) ico_mimetype;
	ico_codec.Flags = ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin;
	ico_codec.Version = 1;
	ico_codec.SigCount = 1;
	ico_codec.SigSize = 4;
	ico_codec.SigPattern = ico_sig_pattern;
	ico_codec.SigMask = ico_sig_mask;
	return &ico_codec; 
}

static BYTE
get_ico_data (BYTE *data, int x, int y, int bpp, int line_length)
{
	BYTE result = 0;
	BYTE *line_data = data + y * line_length;

	switch (bpp) {
	case 1:
		result = (line_data [x >> 3] >> (7 - (x & 7))) & 0x01;
		break;
	case 4:
		result = line_data [x >> 1];
		if ((x & 1) == 1)
			result &= 0x0F;
		else
			result >>= 4;
		break;
	case 8:
		result = line_data [x];
		break;
	}
	return result;
}

static BOOL
read_ICONDIRENTRY (void *pointer, ICONDIRENTRY *entry, ImageSource source, BOOL decode)
{
	if (gdip_read_ico_data (pointer, (void*)entry, sizeof (ICONDIRENTRY), source) != sizeof (ICONDIRENTRY))
		return FALSE;
#if WORDS_BIGENDIAN
	if (decode) {
		BYTE *b;
		/* entry->bWidth, bHeight, bColorCount, bReserved are all BYTE, no change required */
		b = (BYTE*)&entry->wPlanes;
		entry->wPlanes = (b[1] << 8) | b[0]; 
		b = (BYTE*)&entry->wBitCount;
		entry->wBitCount = (b[1] << 8) | b[0];
		b = (BYTE*)&entry->dwBytesInRes;
		entry->dwBytesInRes = ((guint32)b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
		b = (BYTE*)&entry->dwImageOffset;
		entry->dwImageOffset = ((guint32)b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
	}
#endif
	return TRUE;
}

static GpStatus
gdip_read_ico_image_from_file_stream (void *pointer, GpImage **image, ImageSource source)
{
	GpStatus status = OutOfMemory;
	GpBitmap *result = NULL;
	BYTE *pixels = NULL;
	WORD w, count;
	void *p = &w;
	BYTE *b = (BYTE*)&w;
	ICONDIRENTRY entry;
	int i, pos;
	BOOL upsidedown = TRUE;
	BITMAPV5HEADER bih;
	int palette_entries = -1;
	ARGB *colors = NULL;
	int x, y;
	int line_xor_length, xor_size;
	int line_and_length, and_size;
	BYTE *xor_data = NULL, *and_data = NULL;

	/* WORD ICONDIR.idReserved / reversed, MUST be 0 */
	if (gdip_read_ico_data (pointer, p, sizeof (WORD), source) != sizeof (WORD))
		goto error;
	if (w != 0)
		goto error;

	/* WORD ICONDIR.idType / resource type, MUST be 1 for icons */
	if (gdip_read_ico_data (pointer, p, sizeof (WORD), source) != sizeof (WORD))
		goto error;
	i = (b[1] << 8 | b[0]);
	if (i != 1)
		goto error;

	/* WORD ICONDIR.idCount / number of icons, must be greater than 0 */
	if (gdip_read_ico_data (pointer, p, sizeof (WORD), source) != sizeof (WORD))
		goto error;
	count = (b[1] << 8 | b[0]); 
	if (count < 1)
		goto error;

	pos = 6;
	/*
	 * NOTE: it looks like (from unit tests) that we get the last icon 
	 * (e.g. it can return the 16 pixel version, instead of the 32 or 48 pixels available in the same file)
	 */ 
	for (i = 0; i < count - 1; i++) {
		if (!read_ICONDIRENTRY (pointer, &entry, source, FALSE))
			goto error;
		pos += sizeof (ICONDIRENTRY);
	}
	/* last one is important, so we must decode (endianess) it's values */
	if (!read_ICONDIRENTRY (pointer, &entry, source, TRUE))
		goto error;
	pos += sizeof (ICONDIRENTRY);

	while (pos < entry.dwImageOffset) {
		if (gdip_read_ico_data (pointer, p, sizeof (WORD), source) != sizeof (WORD))
			goto error;
		pos += sizeof (WORD);
	}

	/* BITMAPINFOHEADER */
	status = gdip_read_BITMAPINFOHEADER (pointer, source, &bih, &upsidedown);
	if (status != Ok)
		goto error;
	
	result = gdip_bitmap_new_with_frame (NULL, TRUE);
	if (!result) {
		status = OutOfMemory;
		goto error;
	}

	result->type = ImageTypeBitmap;
	result->image_format = ICON;
	result->active_bitmap->pixel_format = PixelFormat32bppARGB; /* icons are always promoted to 32 bbp */
	result->active_bitmap->width = entry.bWidth;
	result->active_bitmap->height = entry.bHeight;
	result->active_bitmap->stride = result->active_bitmap->width * 4;
	/* Ensure 32bits alignment */
	gdip_align_stride (result->active_bitmap->stride);
	result->active_bitmap->dpi_horz = 96.0f;
	result->active_bitmap->dpi_vert = 96.0f;

	switch (bih.bV5BitCount) {
	case 1:
	case 4:
	case 8:
		/* support 2, 16 and 256 colors icons, with palettes, no compression */
		if (bih.bV5Compression == 0)
			palette_entries = 1 << bih.bV5BitCount;
		break;
	case 24:
		/* support 24bits + alpha bitmap, this is not documented anywhere but Windows accept them as valid */
	case 32:
		/* support 24bits + 8 bits alpha (aka "XP" icons), no palette, no compression */
		if (bih.bV5Compression == 0)
			palette_entries = 0;
		break;
	}

	if (palette_entries < 0) {
		status = OutOfMemory;
		goto error;
	}

	/*
	 * Strangely, even if we're supplying a 32bits ARGB image, 
	 * the icon's palette is also supplied with the image.
	 */
	result->active_bitmap->palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * palette_entries);
	if (result->active_bitmap->palette == NULL) {
		status = OutOfMemory;
		goto error;
	}
	result->active_bitmap->palette->Flags = 0;
	result->active_bitmap->palette->Count = palette_entries;

	for (i = 0; i < palette_entries; i++) {
		/* colors are stored as R, G, B and reserved (always 0) */
		BYTE color[4]; 
		void *p = &color;

		if (gdip_read_ico_data (pointer, p, 4, source) < 4) {
			status = OutOfMemory;
			goto error;
		}

		set_pixel_bgra (result->active_bitmap->palette->Entries, i * 4,
			(color[0] & 0xFF),		/* B */
			(color[1] & 0xFF),		/* G */
			(color[2] & 0xFF),		/* R */
			0xFF);				/* Alpha */
	}

	/*
	 * Let's build the 32bpp ARGB bitmap from the icon's XOR and AND bitmaps
	 * notes:
	 * - XORBitmap can be a 1, 4 or 8 bpp bitmap
	 * - ANDBitmap is *always* a monochrome (1bpp) bitmap
	 * - in every case each line is padded to 32 bits boundary
	 */
	unsigned long long int size = (unsigned long long int)result->active_bitmap->stride * result->active_bitmap->height;
	if (size > G_MAXINT32) {
		status = OutOfMemory;
		goto error;
	}
	pixels = GdipAlloc (size);
	if (pixels == NULL) {
		status = OutOfMemory;
		goto error;
	}

	result->active_bitmap->scan0 = pixels;
	result->active_bitmap->reserved = GBD_OWN_SCAN0;
	result->active_bitmap->image_flags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | ImageFlagsColorSpaceRGB | ImageFlagsHasAlpha;

	line_xor_length = (((bih.bV5BitCount * entry.bWidth + 31) & ~31) >> 3);
	xor_size = line_xor_length * entry.bHeight;
	xor_data = (BYTE*) GdipAlloc (xor_size);
	if (!xor_data) {
		status = OutOfMemory;
		goto error;
	}
	if (gdip_read_ico_data (pointer, xor_data, xor_size, source) < xor_size) {
		status = OutOfMemory;
		goto error;
	}

	line_and_length = (((entry.bWidth + 31) & ~31) >> 3);
	and_size = line_and_length * entry.bHeight;
	and_data = (BYTE*) GdipAlloc (and_size);
	if (!and_data) {
		status = OutOfMemory;
		goto error;
	}
	if (gdip_read_ico_data (pointer, and_data, and_size, source) < and_size) {
		status = OutOfMemory;
		goto error;
	}

	colors = result->active_bitmap->palette->Entries;
	for (y = 0; y < entry.bHeight; y++) {
		for (x = 0; x < entry.bWidth; x++) {
			ARGB color;
			if (palette_entries > 0) {
				color = colors [get_ico_data (xor_data, x, y, bih.bV5BitCount, line_xor_length)];
				if (get_ico_data (and_data, x, y, 1, line_and_length) == 1)
					color &= 0x00FFFFFF;
			} else if (bih.bV5BitCount == 24) {
				/* take 1bpp alpha from the and_data */
				if (get_ico_data (and_data, x, y, 1, line_and_length) == 1) {
					color = 0;
				} else {
					BYTE *line_data = xor_data + y * line_xor_length + x * 3;
					color = 0xFF000000 | (line_data [0] | line_data [1] << 8| line_data [2] << 16);
				}
			} else {
				BYTE *line_data = xor_data + y * line_xor_length + x * 4;
				/* ARGB to BRGA */
				color = (line_data [0] | line_data [1] << 8 | line_data [2] << 16 | (guint32)line_data [3] << 24);
			}
			/* image is reversed (y) */
			GdipBitmapSetPixel (result, x, entry.bHeight - y - 1, color);
		}
	}

	GdipFree (xor_data);
	GdipFree (and_data);

	*image = result;
	return Ok;

error:
	if (result)
		GdipDisposeImage (result);
	if (xor_data)
		GdipFree (xor_data);
	if (and_data)
		GdipFree (and_data);

	return status;
}

GpStatus 
gdip_load_ico_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_read_ico_image_from_file_stream ((void*)fp, image, File);
}

GpStatus 
gdip_load_ico_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return gdip_read_ico_image_from_file_stream ((void *)loader, image, DStream);
}
