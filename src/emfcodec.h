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

#define EMR_HEADER			1
#define EMR_POLYBEZIER			2
#define EMR_POLYGON			3
#define EMR_POLYPOLYGON			8
#define EMR_SETWINDOWEXTEX		9
#define EMR_SETWINDOWORGEX		10
#define EMR_SETVIEWPORTEXTEX		11
#define EMR_SETVIEWPORTORGEX		12
#define EMR_SETBRUSHORGEX		13
#define EMR_EOF				14
#define EMR_SETMAPMODE			17
#define EMR_SETBKMODE			18
#define EMR_SETPOLYFILLMODE		19
#define EMR_SETROP2			20
#define EMR_SETSTRETCHBLTMODE		21
#define EMR_SETTEXTALIGN		22
#define EMR_SETTEXTCOLOR		24
#define EMR_MOVETOEX			27
#define EMR_INTERSECTCLIPRECT		30
#define EMR_SAVEDC			33
#define EMR_RESTOREDC			34
#define EMR_SETWORLDTRANSFORM		35
#define EMR_MODIFYWORLDTRANSFORM	36
#define EMR_SELECTOBJECT		37
#define EMR_CREATEPEN			38
#define EMR_CREATEBRUSHINDIRECT		39
#define EMR_DELETEOBJECT		40
#define EMR_LINETO			54
#define EMR_SETMITERLIMIT		58
#define EMR_BEGINPATH			59
#define EMR_ENDPATH			60
#define EMR_CLOSEFIGURE			61
#define EMR_FILLPATH			62
#define EMR_STROKEANDFILLPATH		63
#define EMR_STROKEPATH			64
#define EMR_SELECTCLIPPATH		67
#define EMR_GDICOMMENT			70
#define EMR_EXTSELECTCLIPRGN		75
#define EMR_EXTCREATEFONTINDIRECTW	82
#define EMR_EXTTEXTOUTA			83
#define EMR_EXTTEXTOUTW			84
#define EMR_POLYGON16			86
#define EMR_POLYBEZIERTO16		88
#define EMR_POLYPOLYGON16		91
#define EMR_EXTCREATEPEN		95

/* some old GDI cruft we need to consider... */

#define ENHMETA_STOCK_OBJECT	0x80000000
#define WHITE_BRUSH		0
#define LTGRAY_BRUSH		1
#define GRAY_BRUSH		2
#define DKGRAY_BRUSH		3
#define BLACK_BRUSH		4
#define NULL_BRUSH		5
#define WHITE_PEN		6
#define BLACK_PEN		7
#define NULL_PEN		8
#define OEM_FIXED_FONT		10
#define ANSI_FIXED_FONT		11
#define ANSI_VAR_FONT		12
#define SYSTEM_FONT		13
#define DEVICE_DEFAULT_FONT	14
#define DEFAULT_PALETTE		15
#define SYSTEM_FIXED_FONT	16


#define gdip_read_emf_data	gdip_read_bmp_data

GpStatus gdip_load_emf_image_from_file (FILE *fp, GpImage **image) GDIP_INTERNAL;

GpStatus gdip_load_emf_image_from_stream_delegate (dstream_t *loader, GpImage **image) GDIP_INTERNAL;

/* no save functions as the EMF "codec" is a decoder only */

ImageCodecInfo* gdip_getcodecinfo_emf () GDIP_INTERNAL;

#endif
