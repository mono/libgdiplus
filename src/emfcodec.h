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

#ifndef __EMFCODEC_H__
#define __EMFCODEC_H__

#include "gdiplus-private.h"
#include "metafile-private.h"
#include "graphics-private.h"

/*
 * Some interesting links...
 * http://wvware.sourceforge.net/caolan/ora-wmf.html
 * http://www.undocprint.org/formats/winspool/emf
 */

#define EMF_MIN_RECORD_SIZE	8

#define EMF_CHECK_PARAMS(x)	do { \
		if (params < (x)) goto cleanup; \
	} while (0)

#define EMF_FUNCTION	0
#define EMF_RECORDSIZE	4
#define DWP1		8
#define DWP2		12
#define DWP3		16
#define DWP4		20
#define DWP5		24
#define DWP6		28
#define DWP7		32
#define DWP8		36
#define DWP9		40
#define DWP10		44
#define DWP11		48
#define DWP(y)		(8 + ((y) << 2))


#define gdip_read_emf_data	gdip_read_bmp_data

GpStatus gdip_load_emf_image_from_file (FILE *fp, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_load_emf_image_from_stream_delegate (dstream_t *loader, GpImage **image) GDIP_INTERNAL;

/* no save functions as the EMF "codec" is a decoder only */

ImageCodecInfo* gdip_getcodecinfo_emf () GDIP_INTERNAL;

#endif
