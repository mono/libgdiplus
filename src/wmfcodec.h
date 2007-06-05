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

#ifndef __WMFCODEC_H__
#define __WMFCODEC_H__

#include "gdiplus-private.h"
#include "graphics-private.h"
#include "metafile-private.h"

#define WMF_MIN_RECORD_SIZE	6

#define WMF_CHECK_PARAMS(x)	do { \
		if (params < (x)) goto cleanup; \
	} while (0)

#define RECORDSIZE	0
#define FUNCTION	4
#define WP1		6
#define WP2		8
#define WP3		10
#define WP4		12
#define WP5		14
#define WP6		16
#define WP7		18
#define WP8		20
#define WP9		22
#define WP10		24
#define WP11		26
#define WP12		28
#define WP(y)		(4 + ((y) << 1))

#define METAFILE_RECORD_SAVEDC			0x001E
#define METAFILE_RECORD_SETBKMODE		0x0102
#define METAFILE_RECORD_SETMAPMODE		0x0103
#define METAFILE_RECORD_SETROP2			0x0104
#define METAFILE_RECORD_SETRELABS		0x0105
#define METAFILE_RECORD_SETPOLYFILLMODE		0x0106
#define METAFILE_RECORD_SETSTRETCHBLTMODE	0x0107
#define METAFILE_RECORD_RESTOREDC		0x0127
#define METAFILE_RECORD_SELECTOBJECT		0x012D
#define METAFILE_RECORD_SETTEXTALIGN		0x012E
#define METAFILE_RECORD_DELETEOBJECT		0x01F0
#define METAFILE_RECORD_SETBKCOLOR		0x0201
#define METAFILE_RECORD_SETWINDOWORG		0x020B
#define METAFILE_RECORD_SETWINDOWEXT		0x020C
#define METAFILE_RECORD_LINETO			0x0213
#define METAFILE_RECORD_MOVETO			0x0214
#define METAFILE_RECORD_CREATEPENINDIRECT	0x02FA
#define METAFILE_RECORD_CREATEBRUSHINDIRECT	0x02FC
#define METAFILE_RECORD_POLYGON			0x0324
#define METAFILE_RECORD_POLYLINE		0x0325
#define METAFILE_RECORD_POLYPOLYGON		0x0538
#define METAFILE_RECORD_ARC			0x0817
#define METAFILE_RECORD_STRETCHDIBITS		0x0F43


#define gdip_read_wmf_data	gdip_read_bmp_data


GpStatus gdip_load_wmf_image_from_file (FILE *fp, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_load_wmf_image_from_stream_delegate (dstream_t *loader, GpImage **image) GDIP_INTERNAL;

/* no save functions as the WMF "codec" is a decoder only */

ImageCodecInfo* gdip_getcodecinfo_wmf () GDIP_INTERNAL;

#endif
