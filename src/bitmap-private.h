/*
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
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __BITMAP_PRIVATE_H__
#define __BITMAP_PRIVATE_H__

#include "gdiplus-private.h"
#include "image-private.h"

#define COLOR_PALETTE_FLAGS_DEFAULT	0
#define COLOR_PALETTE_FLAGS_HAS_ALPHA	1
#define COLOR_PALETTE_FLAGS_GREYSCALE	2
#define COLOR_PALETTE_FLAGS_HALFTONE	4

/* LockMode (reserved) flags */
#define GBD_OWN_SCAN0			(1<<8)
#define GBD_WRITE_OK			(1<<9)
#define GBD_LOCKED			(1<<10)
#define GBD_TRUE24BPP			(1<<11)

#ifdef WORDS_BIGENDIAN
#define set_pixel_bgra(pixel,index,b,g,r,a) do { \
		((unsigned char *)(pixel))[index]   = a; \
		((unsigned char *)(pixel))[index+1] = r; \
		((unsigned char *)(pixel))[index+2] = g; \
		((unsigned char *)(pixel))[index+3] = b; \
	} while (0)
#else
#define set_pixel_bgra(pixel,index,b,g,r,a) do { \
		((unsigned char *)(pixel))[index]   = b; \
		((unsigned char *)(pixel))[index+1] = g; \
		((unsigned char *)(pixel))[index+2] = r; \
		((unsigned char *)(pixel))[index+3] = a; \
	} while (0)
#endif
#define get_pixel_bgra(color, b, g, r, a) do { \
		a = ((color & 0xff000000) >> 24); \
		r = ((color & 0x00ff0000) >> 16); \
		g = ((color & 0x0000ff00) >> 8); \
		b = (color & 0x000000ff); \
	} while(0)

/* This structure is mirrored in System.Drawing.Imaging.BitmapData.
   Any changes here must also be made to BitmapData.cs */
typedef struct {
	unsigned int	width;
	unsigned int	height;
	int		stride;
	PixelFormat		pixel_format;
	BYTE 		*scan0;
	UINT_PTR	reserved;
	/* the rest of the structure isn't part of MS GDI+ definition */
	ColorPalette	*palette;
	int		property_count;		/* Number of properties */
	PropertyItem 	*property;		/* Properties associated with image */

	float 		dpi_horz;		/* */
	float 		dpi_vert;		/* */
	ImageFlags	image_flags;		/* Alpha, ColorSpace, etc. */

	unsigned int	left;			/* left display coordinate of frame */
	unsigned int	top;			/* top display coordinate of frame */
	unsigned int	x;			/* LockBits: left coordinate of locked rectangle */
	unsigned int	y;			/* LockBits: top coordinate of locked rectangle */

	int		transparent;		/* Index of transparent color (<24bit only) */
} ActiveBitmapData;

typedef struct {
	int		count;			/* Number of bitmaps contained in this frame */
	ActiveBitmapData	*bitmap;		/* Bitmaps for this frame */
	GUID		frame_dimension;	/* GUID describing the frame type */
} FrameData;

typedef struct _Image {
	/* Image Description */
	ImageType     	type;			/* Undefined, Bitmap, MetaFile */
	ImageFormat     image_format;		/* BMP, TIF, GIF, PNG, etc.	*/
	/* Image Data */
	int		num_of_frames;		/* Number of frames */
	FrameData	*frames;		/* Array of frames (Page, Time, Resolution) for the image */
	/* Tracking of active image */
	int		active_frame;		/* Index of frame currently used */
	int		active_bitmap_no;	/* Index of active bitmap in current frame */
	ActiveBitmapData	*active_bitmap;		/* Pointer to active frame/bitmap; DO NOT free() */
	/* Internal fields */
	int             cairo_format;
	cairo_surface_t *surface;
} GpBitmap;


void gdip_bitmap_init (GpBitmap *bitmap) GDIP_INTERNAL;

GpBitmap *gdip_bitmap_new (void) GDIP_INTERNAL;
GpBitmap *gdip_bitmap_new_with_frame (const GUID *dimension, BOOL add_bitmapdata) GDIP_INTERNAL;
FrameData *gdip_frame_add(GpBitmap *bitmap, const GUID *dimension) GDIP_INTERNAL;
ActiveBitmapData *gdip_frame_add_bitmapdata(FrameData *frame) GDIP_INTERNAL;
GpStatus gdip_bitmap_dispose (GpBitmap *bitmap) GDIP_INTERNAL;
GpStatus gdip_bitmap_clone (GpBitmap *bitmap, GpBitmap **clonedbitmap) GDIP_INTERNAL;
GpStatus gdip_bitmap_setactive (GpBitmap *bitmap, const GUID *dimension, int index) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_clone (ActiveBitmapData *src, ActiveBitmapData **dest, int count) GDIP_INTERNAL;
ColorPalette *gdip_palette_clone(ColorPalette *original) GDIP_INTERNAL;
GpStatus gdip_property_get_short (int offset, void *value, unsigned short *result) GDIP_INTERNAL;
GpStatus gdip_property_get_long (int offset, void *value, guint32 *result) GDIP_INTERNAL;
GpStatus gdip_property_get_srational (int offset, void *value, unsigned short *numerator, unsigned short *denominator) GDIP_INTERNAL;
GpStatus gdip_property_get_rational (int offset, void *value, guint32 *numerator, guint32 *denominator) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_add (ActiveBitmapData *bitmap_data, PROPID id, ULONG length, WORD type, VOID *value) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_add_byte (ActiveBitmapData *bitmap_data, PROPID id, BYTE value) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_add_short (ActiveBitmapData *bitmap_data, PROPID id, unsigned short value) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_add_long (ActiveBitmapData *bitmap_data, PROPID id, guint32 value) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_add_ASCII (ActiveBitmapData *bitmap_data, PROPID id, unsigned char *value) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_add_rational (ActiveBitmapData *bitmap_data, PROPID id, guint32 numerator, guint32 denominator) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_add_srational (ActiveBitmapData *bitmap_data, PROPID id, unsigned short numerator, unsigned short denominator) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_remove_id (ActiveBitmapData *bitmap_data, PROPID id) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_remove_index (ActiveBitmapData *bitmap_data, int index) GDIP_INTERNAL;
GpStatus gdip_bitmapdata_property_find_id (ActiveBitmapData *bitmap_data, PROPID id, int *index) GDIP_INTERNAL;

cairo_surface_t* gdip_bitmap_ensure_surface (GpBitmap *bitmap) GDIP_INTERNAL;
void gdip_bitmap_flush_surface (GpBitmap *bitmap) GDIP_INTERNAL;
void gdip_bitmap_invalidate_surface (GpBitmap *bitmap) GDIP_INTERNAL;
GpBitmap* gdip_convert_indexed_to_rgb (GpBitmap *bitmap) GDIP_INTERNAL;

BOOL gdip_bitmap_format_needs_premultiplication (GpBitmap *bitmap) GDIP_INTERNAL;
BYTE* gdip_bitmap_get_premultiplied_scan0 (GpBitmap *bitmap) GDIP_INTERNAL;
void gdip_bitmap_get_premultiplied_scan0_inplace (GpBitmap *bitmap, BYTE *premul) GDIP_INTERNAL;
void gdip_bitmap_get_premultiplied_scan0_reverse (GpBitmap *bitmap, BYTE *premul) GDIP_INTERNAL;

GpStatus gdip_process_bitmap_attributes (GpBitmap *bitmap, void **dest, GpImageAttributes* attr, BOOL *allocated) GDIP_INTERNAL;

ColorPalette* gdip_create_greyscale_palette (int num_colors) GDIP_INTERNAL;

typedef struct {
	Rect		region;
	int		x, y;			/* the offset of the next byte that will be loaded, once the buffer is depleted */
	unsigned short	buffer;
	int		p;			/* index of pixel within 'buffer' that was returned by the last call to gdip_pixel_stream_get_next () */
	int		one_pixel_mask;
	int		one_pixel_shift;
	int		pixels_per_byte;	/* a negative value is used to indicate a count of bytes per pixel for depths of more than 8 bits */
	ActiveBitmapData	*data;
	BYTE		*scan;
} StreamingState;

GpStatus gdip_init_pixel_stream (StreamingState *state, ActiveBitmapData *data, int x, int y, int w, int h) GDIP_INTERNAL;
unsigned int gdip_pixel_stream_get_next (StreamingState *state) GDIP_INTERNAL;

#include "bitmap.h"

#endif
