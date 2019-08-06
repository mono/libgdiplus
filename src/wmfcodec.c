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

#include "wmfcodec.h"

/* Codecinfo related data*/
GUID gdip_wmf_image_format_guid = {0xb96b3cadU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};
static ImageCodecInfo wmf_codec;
static const WCHAR wmf_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'W', 'M', 'F', ' ', 'C', 'o', 'd', 'e', 'c', 0}; /* Built-in WMF Codec */
static const WCHAR wmf_extension[] = {'*','.','W', 'M', 'F', 0}; /* *.WMF */
static const WCHAR wmf_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'x', '-', 'w', 'm', 'f', 0}; /* image/x-wmf */
static const WCHAR wmf_format[] = {'W', 'M', 'F', 0}; /* WMF */
static const BYTE wmf_sig_pattern[] = { 0xD7, 0xCD, 0xC6, 0x9A };
static const BYTE wmf_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF };

//#define DEBUG_WMF_ALL
#ifdef DEBUG_WMF_ALL
#define DEBUG_WMF
#define DEBUG_WMF_2
#define DEBUG_WMF_3
#define DEBUG_WMF_NOTIMPLEMENTED
#endif

ImageCodecInfo*
gdip_getcodecinfo_wmf ()
{
	wmf_codec.Clsid = (CLSID) { 0x557cf404, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	wmf_codec.FormatID = gdip_wmf_image_format_guid;
	wmf_codec.CodecName = (const WCHAR*) wmf_codecname;    
	wmf_codec.DllName = NULL;
	wmf_codec.FormatDescription = (const WCHAR*) wmf_format;
	wmf_codec.FilenameExtension = (const WCHAR*) wmf_extension;
	wmf_codec.MimeType = (const WCHAR*) wmf_mimetype;
	wmf_codec.Flags = ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin;
	wmf_codec.Version = 1;
	wmf_codec.SigCount = 1;
	wmf_codec.SigSize = 4;
	wmf_codec.SigPattern = wmf_sig_pattern;
	wmf_codec.SigMask = wmf_sig_mask;
	return &wmf_codec;
}

#if DEBUG_WMF
#define GetParam(x,y)	GetWORD((6 + ((x) << 1)), (y))

static WORD
GetWORD (int position, BYTE *data)
{
	WORD *value = (WORD*)(data + position);
#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	return GUINT16_FROM_LE (*value);
#else
	return *value;
#endif
}
#endif

static DWORD
GetColor (WORD w1, WORD w2)
{
	DWORD color = (w2 << 16) | w1;
	BYTE *p = (BYTE*)&color;
	BYTE temp = p[0];
	p[0] = p[2];
	p[2] = temp;
	return color;
}

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
#define GETW(x)		(*(WORD*)(data + (x)))
#else
#define GETW(x)		(GUINT16_FROM_LE(*(WORD*)(data + (x))))
#endif

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
#define GETS(x)		(*(SHORT*)(data + (x)))
#else
#define GETS(x)		(GINT16_FROM_LE(*(SHORT*)(data + (x))))
#endif

/* http://wvware.sourceforge.net/caolan/Polygon.html */
static GpStatus
Polygon (MetafilePlayContext *context, BYTE *data, int len)
{
	GpPointF *points, *pt;
	GpStatus status;
	int p;
	/* variable number of parameters */
	SHORT num = GETS(WP1);

	/* len (in WORDs) = num (WORD) + num * (x WORD + y WORD) */
	if (num > len + 1)
		return InvalidParameter;

#ifdef DEBUG_WMF
	printf ("Polygon %d points", num);
#endif
	points = (GpPointF*) GdipAlloc (num * sizeof (GpPointF));
	if (!points)
		return OutOfMemory;

	int n = 2;
	for (p = 0, pt = points; p < num; p++, pt++) {
		pt->X = GETS(WP(n));
		n++;
		pt->Y = GETS(WP(n));
		n++;
#ifdef DEBUG_WMF
		printf ("\n\tpoly to %g,%g", pt->X, pt->Y);
#endif
	}

	status = gdip_metafile_Polygon (context, points, num);

	GdipFree (points);
	return status;
}

/* http://wvware.sourceforge.net/caolan/Polyline.html */
static GpStatus
Polyline (MetafilePlayContext *context, BYTE *data)
{
	GpStatus status;
	int p;
	/* variable number of parameters */
	SHORT num = GETS(WP1);

#ifdef DEBUG_WMF
	printf ("Polyline %d points", num);
#endif
	SHORT x1 = GETS(WP2);
	SHORT y1 = GETS(WP3);
	int n = 4;
	for (p = 1; p < num; p++) {
		SHORT x2 = GETS(WP(n));
		n++;
		SHORT y2 = GETS(WP(n));
		n++;
#ifdef DEBUG_WMF_2
		printf ("\n\tdraw from %d,%d to %d,%d", x1, y1, x2, y2);
#endif
		GpPen *pen = gdip_metafile_GetSelectedPen (context);
		status = GdipDrawLine (context->graphics, pen, x1, y1, x2, y2);
		if (status != Ok)
			return status;

		x1 = x2;
		y1 = y2;
	}
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/PolyPolygon.html */
/* storage isn't very efficient, # of polygons, size of each polygon, data for each polygon */
static GpStatus
PolyPolygon (MetafilePlayContext *context, BYTE *data)
{
	GpStatus status = Ok;
	/* variable number of parameters */
	int poly_num = GETW(WP1);
	int i, j;
	PointFList *list = GdipAlloc (poly_num * sizeof (PointFList));
	if (!list) {
		return OutOfMemory;
	}

	PointFList *current = list;
#ifdef DEBUG_WMF
	printf ("PolyPolygon has %d polygons", poly_num);
#endif
	int n = 2;
	/* read size of each polygon and allocate the required memory */
	for (i = 0; i < poly_num; i++) {
		current->num = GETW(WP(n));
		n++;
		current->points = (GpPointF*) GdipAlloc (current->num * sizeof (GpPointF));
		if (!current->points) {
			for (j = 0; j < i; j++) {
				GdipFree (list[j].points);
			}

			GdipFree (list);
			return OutOfMemory;
		}

#ifdef DEBUG_WMF_2
		printf ("\n\tSub Polygon #%d has %d points", i, current->num);
#endif
		current++;
	}

	/* read the points for each polygons */
	current = list;
	for (i = 0; i < poly_num; i++) {
		GpPointF *pt = current->points;
		int p;
		for (p = 0; p < current->num; p++) {
			pt->X = GETW(WP(n));
			n++;
			pt->Y = GETW(WP(n));
			n++;
#ifdef DEBUG_WMF_3
			printf ("\n\t\tpoly to %g,%g", pt->X, pt->Y);
#endif
			pt++;
		}

		GpStatus s = gdip_metafile_Polygon (context, current->points, current->num);
		if (s != Ok)
			status = s;

		/* free points */
		GdipFree (current->points);
		current++;
	}

	/* all points were freed, after being drawn, so we just have to free the polygon list*/
	GdipFree (list);
	return status;
}

GpStatus
gdip_metafile_play_wmf (MetafilePlayContext *context)
{
	GpStatus status = Ok;
	GpMetafile *metafile = context->metafile;
	BYTE *data = metafile->data;
	BYTE *end = data + metafile->length;
#ifdef DEBUG_WMF
	int i = 1, j;
#endif
	/* reality check - each record is, at minimum, 6 bytes long (4 size + 2 function) */
	while (data < end - WMF_MIN_RECORD_SIZE) {
		DWORD size = GETDW(RECORDSIZE);
		WORD func = GETW(FUNCTION);
		int params = size - (WMF_MIN_RECORD_SIZE / sizeof (WORD));
#ifdef DEBUG_WMF
		printf ("\n[#%d] size %d ", i++, size);
#endif
		/* reality check - enough data available to read all parameters ? (params is in WORD) */
		if ((params << 1) > (long)(end - data)) {
			status = InvalidParameter;
			goto cleanup;
		}

		/* Notes:
		 * - The previous check doesn't mean we have all required parameters (only the one encoded)
		 * - sometimes there are extra (undocumented?, buggy?) parameters for some functions
		 */
		switch (func) {
		case META_SAVEDC:
			WMF_CHECK_PARAMS(0);
			status = gdip_metafile_SaveDC (context);
			break;
		case META_SETBKMODE:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetBkMode (context, GETW(WP1));
			break;
		case META_SETMAPMODE:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetMapMode (context, GETW(WP1));
			break;
		case META_SETROP2:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetROP2 (context, GETW(WP1));
			break;
		case META_SETRELABS:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetRelabs (context, GETW(WP1));
			break;
		case META_SETPOLYFILLMODE:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetPolyFillMode (context, GETW(WP1));
			break;
		case META_SETSTRETCHBLTMODE:
			WMF_CHECK_PARAMS(1); /* 2 but second is unused (32bits?) */
			status = gdip_metafile_SetStretchBltMode (context, GETW(WP1));
			break;
		case META_RESTOREDC:
			WMF_CHECK_PARAMS(0);
			status = gdip_metafile_RestoreDC (context);
			break;
		case META_SELECTOBJECT:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_SelectObject (context, GETW(WP1));
			break;
		case META_SETTEXTALIGN:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetTextAlign (context, GETW(WP1));
			break;
		case META_DELETEOBJECT:
			WMF_CHECK_PARAMS(1);
			status = gdip_metafile_DeleteObject (context, GETW(WP1));
			break;
		case META_SETBKCOLOR:
			WMF_CHECK_PARAMS(2);
			status = gdip_metafile_SetBkColor (context, GetColor (GETW(WP1), GETW(WP2)));
			break;
		case META_SETWINDOWORG:
			WMF_CHECK_PARAMS(2);
			status = gdip_metafile_SetWindowOrg (context, GETS(WP1), GETS(WP2));
			break;
		case META_SETWINDOWEXT:
			WMF_CHECK_PARAMS(2);
			status = gdip_metafile_SetWindowExt (context, GETS(WP1), GETS(WP2));
			break;
		case META_LINETO:
			WMF_CHECK_PARAMS(2);
			status = gdip_metafile_LineTo (context, GETS(WP1), GETS(WP2));
			break;
		case META_MOVETO:
			WMF_CHECK_PARAMS(2);
			status = gdip_metafile_MoveTo (context, GETS(WP1), GETS(WP2));
			break;
		case META_CREATEPENINDIRECT:
			/* note: documented with only 4 parameters, LOGPEN use a POINT to specify width, so y (3) is unused) */
			WMF_CHECK_PARAMS(5);
			status = gdip_metafile_CreatePenIndirect (context, GETW(WP1), GETW(WP2), GetColor (GETW(WP4), GETW(WP5)));
			break;
		case META_CREATEBRUSHINDIRECT:
			WMF_CHECK_PARAMS(4);
			status = gdip_metafile_CreateBrushIndirect (context, GETW(WP1), GetColor (GETW(WP2), GETW(WP3)), GETW(WP4));
			break;
		case META_POLYGON:
			status = Polygon (context, data, params);
			break;
		case META_POLYLINE:
			status = Polyline (context, data);
			break;
		case META_POLYPOLYGON:
			status = PolyPolygon (context, data);
			break;
		case META_ARC:
			WMF_CHECK_PARAMS(8);
			status = gdip_metafile_Arc (context, GETS(WP1), GETS(WP2), GETS(WP3), GETS(WP4), GETS(WP5), GETS(WP6),
				GETS(WP7), GETS(WP8));
			break;
		case META_RECTANGLE:
			WMF_CHECK_PARAMS (4);
			status = gdip_metafile_Rectangle (context, GETS (WP1), GETS (WP2), GETS (WP3), GETS (WP4));
			break;
		case META_SETPIXEL:
			WMF_CHECK_PARAMS (4);
			status = gdip_metafile_SetPixel (context, GetColor (GETW (WP1), GETW (WP2)), GETW (WP4), GETW (WP3));
			break;
		case META_STRETCHDIB: {
			WMF_CHECK_PARAMS(14);
			BITMAPINFO *bmi = (BITMAPINFO*) (data + 14 * sizeof (WORD));
			void* bits = (void*) (bmi + GETDW(WP12));
			status = gdip_metafile_StretchDIBits (context, GETS(WP11), GETS(WP10), GETS(WP9), GETS(WP8), GETS(WP7), 
				GETS(WP6), GETS(WP5), GETS(WP4), bits, bmi, GETW(WP3), GETDW(WP1));
			break;
		}
		case META_DIBSTRETCHBLT: {
			WMF_CHECK_PARAMS(12);
			BITMAPINFO *bmi = (BITMAPINFO*) (data + 13 * sizeof (WORD));
			void* bits = (void*) (bmi + GETDW(WP11));
			status = gdip_metafile_StretchDIBits (context, GETS(WP10), GETS(WP9), GETS(WP8), GETS(WP7), GETS(WP6), 
				GETS(WP5), GETS(WP4), GETS(WP3), bits, bmi, 0, GETDW(WP1));
			break;
		}
		default:
			/* unprocessed records, ignore the data */
			/* 3 for size (DWORD) == 2 * SHORT + function == 1 SHORT */
#ifdef DEBUG_WMF
			printf ("Unimplemented_%X (", func);
			for (j = 0; j < params; j++) {
				printf (" %d", GetParam (j, data));
			}
			printf (" )");
#endif
			break;
		}

		if (status != Ok) {
			g_warning ("Parsing interupted, status %d returned from function %d.", status, func);
			goto cleanup;
		}

		data += size * 2;
	}
cleanup:
	return status;
}

GpStatus 
gdip_load_wmf_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_get_metafile_from ((void*)fp, (GpMetafile**)image, File);
}

GpStatus 
gdip_load_wmf_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return gdip_get_metafile_from ((void *)loader, (GpMetafile**)image, DStream);
}
