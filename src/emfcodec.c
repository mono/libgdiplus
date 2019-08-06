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

#include "emfcodec.h"

//#define DEBUG_EMF_ALL
#ifdef DEBUG_EMF_ALL
#define DEBUG_EMF
#define DEBUG_EMF_2
#define DEBUG_EMF_3
#define DEBUG_EMF_NOTIMPLEMENTED
#endif

#ifdef DEBUG_EMF_NOTIMPLEMENTED
#define NOTIMPLEMENTED(s)			printf(s)
#define NOTIMPLEMENTED1(s,p1)			printf(s, (p1))
#define NOTIMPLEMENTED2(s,p1,p2)		printf(s, (p1), (p2))
#define NOTIMPLEMENTED3(s,p1,p2,p3)		printf(s, (p1), (p2), (p3))
#define NOTIMPLEMENTED4(s,p1,p2,p3,p4)		printf(s, (p1), (p2), (p3), (p4))
#define NOTIMPLEMENTED5(s,p1,p2,p3,p4,p5)	printf(s, (p1), (p2), (p3), (p4), (p5))
#define NOTIMPLEMENTED6(s,p1,p2,p3,p4,p5,p6)	printf(s, (p1), (p2), (p3), (p4), (p5), (p6))
#else
#define NOTIMPLEMENTED(s)
#define NOTIMPLEMENTED1(s,p1)
#define NOTIMPLEMENTED2(s,p1,p2)
#define NOTIMPLEMENTED3(s,p1,p2,p3)
#define NOTIMPLEMENTED4(s,p1,p2,p3,p4)
#define NOTIMPLEMENTED5(s,p1,p2,p3,p4,p5)
#define NOTIMPLEMENTED6(s,p1,p2,p3,p4,p5,p6)
#endif

GUID gdip_emf_image_format_guid = {0xb96b3cacU, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

/* Codecinfo related data*/
static ImageCodecInfo emf_codec;
static const WCHAR emf_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'E', 'M', 'F', ' ', 'C', 'o', 'd', 'e', 'c', 0}; /* Built-in EMF Codec */
static const WCHAR emf_extension[] = {'*','.','E', 'M', 'F', 0}; /* *.EMF */
static const WCHAR emf_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'x', '-', 'e', 'm', 'f', 0}; /* image/x-emf */
static const WCHAR emf_format[] = {'E', 'M', 'F', 0}; /* EMF */
static const BYTE emf_sig_pattern[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x45, 0x4D, 0x46 };
static const BYTE emf_sig_mask[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

ImageCodecInfo*
gdip_getcodecinfo_emf ()
{
	emf_codec.Clsid = (CLSID) { 0x557cf403, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	emf_codec.FormatID = gdip_emf_image_format_guid;
	emf_codec.CodecName = (const WCHAR*) emf_codecname;    
	emf_codec.DllName = NULL;
	emf_codec.FormatDescription = (const WCHAR*) emf_format;
	emf_codec.FilenameExtension = (const WCHAR*) emf_extension;
	emf_codec.MimeType = (const WCHAR*) emf_mimetype;
	emf_codec.Flags = ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin;
	emf_codec.Version = 1;
	emf_codec.SigCount = 1;
	emf_codec.SigSize = 44;
	emf_codec.SigPattern = emf_sig_pattern;
	emf_codec.SigMask = emf_sig_mask;
	return &emf_codec;
}

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
#define GETDW(x)	(*(DWORD*)(data + (x)))
#define GETFLOAT(x)	(*(float*)(data + (x)))
#else
#define GETDW(x)	(GUINT32_FROM_LE(*(DWORD*)(data + (x))))
#define GETFLOAT(x)	((float)GETDW(x))
#endif

static DWORD
GetColor (DWORD color)
{
	BYTE *p = (BYTE*)&color;
	BYTE temp = p[0];
	p[0] = p[2];
	p[2] = temp;
	return color;
}

static GpStatus
PolyBezier (MetafilePlayContext *context, BYTE *data, int len, BOOL compact)
{
	DWORD num;
	GpPointF *points, *pt;
	GpStatus status;
	int p, n = 0;
	RECTL bounds;
	bounds.left = GETDW(DWP(n));
	n++;
	bounds.top = GETDW(DWP(n));
	n++;
	bounds.right = GETDW(DWP(n));
	n++;
	bounds.bottom = GETDW(DWP(n));
	n++;

	/* make sure we're not reading more data than what's available in this record */
	num = GETDW(DWP(n));
	n++;
	if (compact) {
		/* len = bounds (4 * DWORD) + num (DWORD) + 2 * num * (x WORD + y WORD) */
		if (num > ((len - 5 * sizeof (DWORD)) >> 2))
			return InvalidParameter;
	} else {
		/* len = bounds (4 * DWORD) + num (DWORD) + 2 * num * (x DWORD + y DWORD) */
		if (num > ((len - 5 * sizeof (DWORD)) >> 3))
			return InvalidParameter;
	}

#ifdef DEBUG_EMF
	printf ("PolyBezier%s bounds [%d, %d, %d, %d] with %d points", (compact ? "16" : ""), 
		bounds.left, bounds.top, bounds.right, bounds.bottom, num);
#else
	(void) bounds; // Avoid an unused variable warning.
#endif

	/* we need to supply the current x,y position */
	points = (GpPointF*) GdipAlloc ((num + 1) * sizeof (GpPointF));
	if (!points)
		return OutOfMemory;

	pt = points;
	pt->X = context->current_x;
	pt->Y = context->current_y;
#ifdef DEBUG_EMF_2
	printf ("\n\tmoveto to %g,%g", pt->X, pt->Y);
#endif
	context->path_x = pt->X;
	context->path_y = pt->Y;
	pt++;
	for (p = 0; p < num; p++, pt++) {
		if (compact) {
			DWORD xy = GETDW(DWP(n));
			n++;
			pt->X = (xy & 0x0000FFFF);
			pt->Y = (xy >> 16);
		} else {
			pt->X = GETDW(DWP(n));
			n++;
			pt->Y = GETDW(DWP(n));
			n++;
		}
#ifdef DEBUG_EMF_2
		printf ("\n\tbezier to %g,%g", pt->X, pt->Y);
#endif
	}
	/* keep last point of the path so we can close the figure as GDI does (line) */
	pt--;
	context->current_x = pt->X;
	context->current_y = pt->Y;

	/* PolyBezier doesn't use the current point (GDI wise) but metafiles seems different */
	status = gdip_metafile_PolyBezier (context, points, num + 1);

	GdipFree (points);
	return status;
}

/* the structure is different from WMF (16 or 32bits, RECTL bounds) */
static GpStatus
Polygon (MetafilePlayContext *context, BYTE *data, int len, BOOL compact)
{
	DWORD num;
	GpPointF *points, *pt;
	GpStatus status;
	int p, n = 0;
	RECTL bounds;
	bounds.left = GETDW(DWP(n));
	n++;
	bounds.top = GETDW(DWP(n));
	n++;
	bounds.right = GETDW(DWP(n));
	n++;
	bounds.bottom = GETDW(DWP(n));
	n++;

	/* make sure we're not reading more data than what's available in this record */
	num = GETDW(DWP(n));
	n++;
	if (compact) {
		/* len = bounds (4 * DWORD) + num (DWORD) + 2 * num * (x WORD + y WORD) */
		if (num > ((len - 5 * sizeof (DWORD)) >> 2))
			return InvalidParameter;
	} else {
		/* len = bounds (4 * DWORD) + num (DWORD) + 2 * num * (x DWORD + y DWORD) */
		if (num > ((len - 5 * sizeof (DWORD)) >> 3))
			return InvalidParameter;
	}

#ifdef DEBUG_EMF
	printf ("Polygon%s bounds [%d, %d, %d, %d] with %d points", (compact ? "16" : ""), 
		bounds.left, bounds.top, bounds.right, bounds.bottom, num);
#else
	(void) bounds; // Avoid an unused variable warning.
#endif

	points = (GpPointF*) GdipAlloc (num * sizeof (GpPointF));
	if (!points)
		return OutOfMemory;

	for (p = 0, pt = points; p < num; p++, pt++) {
		if (compact) {
			DWORD xy = GETDW(DWP(n));
			n++;
			pt->X = (xy & 0x0000FFFF);
			pt->Y = (xy >> 16);
		} else {
			pt->X = GETDW(DWP(n));
			n++;
			pt->Y = GETDW(DWP(n));
			n++;
		}
#ifdef DEBUG_EMF_2
		printf ("\n\tpoly to %g,%g", pt->X, pt->Y);
#endif
	}

	status = gdip_metafile_Polygon (context, points, num);

	GdipFree (points);
	return status;
}

/* the structure is different from WMF */
static GpStatus
PolyPolygon (MetafilePlayContext *context, BYTE *data, BOOL compact)
{
	GpStatus status = Ok;
	int poly_num;
	int n = 0;
	RECTL bounds;
	bounds.left = GETDW(DWP(n));
	n++;
	bounds.top = GETDW(DWP(n));
	n++;
	bounds.right = GETDW(DWP(n));
	n++;
	bounds.bottom = GETDW(DWP(n));
	n++;

	/* variable number of polygons */
	poly_num = GETDW(DWP(n));
	n++;

	/* total number of points (in all polygons)*/
	// int total = GETDW(DWP(n));
	n++;
	int i, j;
	PointFList *list = GdipAlloc (poly_num * sizeof (PointFList));
	if (!list)
		return OutOfMemory;

	PointFList *current = list;
#ifdef DEBUG_EMF
	printf ("PolyPolygon%s bounds [%d, %d, %d, %d] with %d polygons", (compact ? "16" : ""), 
		bounds.left, bounds.top, bounds.right, bounds.bottom, poly_num);
#else
	(void) bounds; // Avoid an unused variable warning.
#endif

	/* read size of each polygon and allocate the required memory */
	for (i = 0; i < poly_num; i++) {
		current->num = GETDW(DWP(n));
		n++;
		current->points = (GpPointF*) GdipAlloc (current->num * sizeof (GpPointF));
		if (!current->points) {
			for (j = 0; j < i; j++) {
				GdipFree (list[j].points);
			}

			GdipFree (list);
			return OutOfMemory;
		}

#ifdef DEBUG_EMF_2
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
			if (compact) {
				DWORD xy = GETDW(DWP(n));
				n++;
				pt->X = (xy & 0x0000FFFF);
				pt->Y = (xy >> 16);
			} else {
				pt->X = GETDW(DWP(n));
				n++;
				pt->Y = GETDW(DWP(n));
				n++;
			}
#ifdef DEBUG_EMF_3
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

/* http://wvware.sourceforge.net/caolan/ora-wmf.html */
GpStatus
GdiComment (MetafilePlayContext *context, BYTE* data, DWORD size)
{
#ifdef DEBUG_EMF
	printf ("GdiComment record size %d", size);
#endif
	DWORD length = GETDW(DWP1);
	if (length >= 4) {
		DWORD header = GETDW(DWP2);
		if ((header == 0x2B464D45) && (size >= 8)) {
#ifdef DEBUG_EMF_2
			printf (", EMF+ length %d", length);
#endif
			/* move past func, size, comment length, remove EMF+ comment header */
			data += sizeof (DWORD) * 4;
			/* remove EMF+ header from length */
			length -= sizeof (DWORD);
			/* play the EMF+ comment-embedded block */
			return gdip_metafile_play_emfplus_block (context, data, length);
		}
	}

#ifdef DEBUG_EMF_NOTIMPLEMENTED
	int i;
	printf (" - ");
	for (i = 0; i < size; i++)
		printf ("%2X ", *data++);
#endif
	return Ok;
}

static GpStatus
ExtCreatePen (MetafilePlayContext *context, BYTE *data, int size)
{
	LOGBRUSH lb;
#ifdef DEBUG_EMF
	printf ("EMR_EXTCREATEPEN");
#endif
	if ((GETDW(DWP2) != size) || (GETDW(DWP3) != 0) || (GETDW(DWP4) != size) || (GETDW(DWP5) != 0))
		return InvalidParameter;

	lb.lbStyle = GETDW(DWP8);
	lb.lbColor = GetColor(GETDW(DWP9));
	lb.lbHatch = GETDW(DWP10);
	return gdip_metafile_ExtCreatePen (context, GETDW(DWP6), GETDW(DWP7), &lb, GETDW(DWP11), NULL);
}

static GpStatus
ModifyWorldTransform (MetafilePlayContext *context, float eM11, float eM12, float eM21, float eM22, 
	float eDx, float eDy, DWORD iMode)
{
	XFORM xf;
	xf.eM11 = eM11;
	xf.eM12 = eM12;
	xf.eM21 = eM21;
	xf.eM22 = eM22;
	xf.eDx  = eDx;
	xf.eDy  = eDy;
	return gdip_metafile_ModifyWorldTransform (context, &xf, iMode);
}

/*
 * This is very similar in design to the WMF parser, the biggest changes being...
 *
 * - The EMF records are based on 32bits units (unlike WMF records which are 16bits);
 * - The order the size and function parameter is reversed (from WMF);
 * - The size parameter is represented in bytes (not in WORD like WMF);
 * - Minimum record size is 8 bytes (function DWORD + size DWORD);
 * - There are now a record types to start (1) and end (14) the metafiles;
 */
GpStatus
gdip_metafile_play_emf (MetafilePlayContext *context)
{
	GpStatus status = Ok;
	GpMetafile *metafile = context->metafile;
	BYTE *data = metafile->data;
	BYTE *end = data + metafile->length;
#ifdef DEBUG_EMF
	int i = 1, j;
#endif
	/* check for empty or recording metafile */
	if (!data)
		return Ok;

	/* reality check - each record is, at minimum, 8 bytes long (when size == 0) */
	while (data < end - EMF_MIN_RECORD_SIZE) {
		/* record */
		DWORD func = GETDW(EMF_FUNCTION);
		DWORD size = GETDW(EMF_RECORDSIZE);
		int params = (size - EMF_MIN_RECORD_SIZE) / sizeof (DWORD);
#ifdef DEBUG_EMF
		printf ("\n[#%d] size %d ", i++, size);
#endif
		switch (func) {
		case EMR_POLYBEZIER:
			status = PolyBezier (context, data, size - EMF_MIN_RECORD_SIZE, FALSE);
			break;
		case EMR_POLYGON:
			status = Polygon (context, data, size - EMF_MIN_RECORD_SIZE, FALSE);
			break;
		case EMR_POLYPOLYGON:
			status = PolyPolygon (context, data, FALSE);
			break;
		case EMR_SETWINDOWEXTEX:
			EMF_CHECK_PARAMS(2);
			status = gdip_metafile_SetWindowExt (context, GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_SETWINDOWORGEX:
			EMF_CHECK_PARAMS(2);
			status = gdip_metafile_SetWindowOrg (context, GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_SETVIEWPORTEXTEX:
			EMF_CHECK_PARAMS(2);
			NOTIMPLEMENTED2("EMR_SETVIEWPORTEXTEX not implemented %d, %d", GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_SETVIEWPORTORGEX:
			EMF_CHECK_PARAMS(2);
			NOTIMPLEMENTED2("EMR_SETVIEWPORTORGEX not implemented %d, %d", GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_SETBRUSHORGEX:
			EMF_CHECK_PARAMS(2);
			NOTIMPLEMENTED2("EMR_SETBRUSHORGEX not implemented %d, %d", GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_EOF:
			EMF_CHECK_PARAMS(3);
#ifdef DEBUG_EMF
			printf ("EndOfRecord %d %d %d", GETDW(DWP1), GETDW(DWP2), GETDW(DWP3));
#endif
			status = Ok;
			/* rest of the metafile can contains other stuff than records (e.g. palette information) */
			data += size;
			goto cleanup;
		case EMR_SETMAPMODE:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetMapMode (context, GETDW(DWP1));
			break;
		case EMR_SETBKMODE:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetBkMode (context, GETDW(DWP1));
			break;
		case EMR_SETPOLYFILLMODE:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetPolyFillMode (context, GETDW(DWP1));
			break;
		case EMR_SETROP2:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetROP2 (context, GETDW(DWP1));
			break;
		case EMR_SETSTRETCHBLTMODE:
			NOTIMPLEMENTED("EMR_SETSTRETCHBLTMODE not implemented");
			break;
		case EMR_SETTEXTALIGN:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetTextAlign (context, GETDW(DWP1));
			break;
		case EMR_SETTEXTCOLOR:
			EMF_CHECK_PARAMS(1);
			NOTIMPLEMENTED1("EMR_SETTEXTCOLOR %d not implemented", GETDW(DWP1));
			break;
		case EMR_MOVETOEX:
			EMF_CHECK_PARAMS(2);
			status = gdip_metafile_MoveTo (context, GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_INTERSECTCLIPRECT:
			EMF_CHECK_PARAMS(4);
			NOTIMPLEMENTED4("EMR_INTERSECTCLIPRECT %d, %d, %d, %d not implemented", GETDW(DWP1), GETDW(DWP1), GETDW(DWP3), GETDW(DWP4));
			break;
		case EMR_SAVEDC:
			EMF_CHECK_PARAMS(0);
			NOTIMPLEMENTED("EMR_SAVEDC not implemented");
			break;
		case EMR_RESTOREDC:
			EMF_CHECK_PARAMS(1);
			NOTIMPLEMENTED1("EMR_RESTOREDC %d not implemented", GETDW(DWP1));
			break;
		case EMR_SETWORLDTRANSFORM:
			EMF_CHECK_PARAMS(6);
			NOTIMPLEMENTED6("EMR_SETWORLDTRANSFORM %d not implemented", GETDW(DWP1), GETDW(DWP2), GETDW(DWP3), GETDW(DWP4), GETDW(DWP5), GETDW(DWP6));
			break;
		case EMR_MODIFYWORLDTRANSFORM:
			EMF_CHECK_PARAMS(7);
			status = ModifyWorldTransform (context, GETFLOAT(DWP1), GETFLOAT(DWP2), GETFLOAT(DWP3), 
				GETFLOAT(DWP4), GETFLOAT(DWP5), GETFLOAT(DWP6), GETDW(DWP7));
			break;
		case EMR_SELECTOBJECT:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_SelectObject (context, GETDW(DWP1));
			break;
		case EMR_CREATEPEN:
			EMF_CHECK_PARAMS(5);
			status = gdip_metafile_CreatePenIndirect (context, GETDW(DWP2), GETDW(DWP3), GETDW(DWP4));
			break;
		case EMR_CREATEBRUSHINDIRECT:
			EMF_CHECK_PARAMS(4);
			/* 4 parameters provided, only 3 required in LOGBRUSH structure used in CreateBrushIndirect */
			status = gdip_metafile_CreateBrushIndirect (context, GETDW(DWP4), GetColor(GETDW(DWP3)), GETDW(DWP2));
			break;
		case EMR_DELETEOBJECT:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_DeleteObject (context, GETDW(DWP1));
			break;
		case EMR_LINETO:
			EMF_CHECK_PARAMS(2);
			status = gdip_metafile_LineTo (context, GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_SETMITERLIMIT:
			EMF_CHECK_PARAMS(1);
			status = gdip_metafile_SetMiterLimit (context, GETDW(DWP1), NULL);
			break;
		case EMR_BEGINPATH:
			EMF_CHECK_PARAMS(0);
			status = gdip_metafile_BeginPath (context);
			break;
		case EMR_ENDPATH:
			EMF_CHECK_PARAMS(0);
			status = gdip_metafile_EndPath (context);
			break;
		case EMR_CLOSEFIGURE:
			EMF_CHECK_PARAMS(0);
			status = gdip_metafile_CloseFigure (context);
			break;
		case EMR_FILLPATH:
			EMF_CHECK_PARAMS(4);
			/* TODO - deal with all parameters, we have what looks like a rectangle (bounds?) */
			status = gdip_metafile_FillPath (context);
			break;
		case EMR_STROKEANDFILLPATH:
			EMF_CHECK_PARAMS(4);
			/* TODO - deal with all parameters, we have what looks like a rectangle (bounds?) */
			status = gdip_metafile_StrokeAndFillPath (context);
			break;
		case EMR_STROKEPATH:
			EMF_CHECK_PARAMS(4);
			/* TODO - deal with all parameters, we have what looks like a rectangle (bounds?) */
			status = gdip_metafile_StrokePath (context);
			break;
		case EMR_SELECTCLIPPATH:
			EMF_CHECK_PARAMS(1);
			NOTIMPLEMENTED1("EMR_SELECTCLIPPATH %d not implemented", GETDW(DWP1));
			break;
		case EMR_GDICOMMENT:
			EMF_CHECK_PARAMS(1); /* record contains at least the size of the comment */
			status = GdiComment (context, data, size);
			break;
		case EMR_EXTSELECTCLIPRGN:
			EMF_CHECK_PARAMS(2);
			NOTIMPLEMENTED2("EMR_EXTSELECTCLIPRGN %d, %d not implemented", GETDW(DWP1), GETDW(DWP2));
			break;
		case EMR_EXTCREATEFONTINDIRECTW:
			NOTIMPLEMENTED("EMR_EXTCREATEFONTINDIRECTW");
			break;
		case EMR_EXTTEXTOUTA:
			NOTIMPLEMENTED("EMR_EXTTEXTOUTA");
			break;
		case EMR_EXTTEXTOUTW:
			NOTIMPLEMENTED("EMR_EXTTEXTOUTW");
			break;
		case EMR_POLYGON16:
			status = Polygon (context, data, size - EMF_MIN_RECORD_SIZE, TRUE);
			break;
		case EMR_POLYBEZIERTO16:
			status = PolyBezier (context, data, size - EMF_MIN_RECORD_SIZE, TRUE);
			break;
		case EMR_POLYPOLYGON16:
			status = PolyPolygon (context, data, TRUE);
			break;
		case EMR_EXTCREATEPEN:
			EMF_CHECK_PARAMS(11);
			status = ExtCreatePen (context, data, size);
			break;
		default:
			/* unprocessed records, ignore the data */
#ifdef DEBUG_EMF_NOTIMPLEMENTED
			printf ("Unimplemented_%d (", func);
			for (j = 0; j < params; j++) {
				printf (" %d", GETDW(DWP(j)));
			}
			printf (" )");
#endif
			break;
		}

		if (status != Ok) {
			g_warning ("Parsing interupted, status %d returned from function %d.", status, func);
			goto cleanup;
		}

		data += size;
	}
cleanup:
	return status;
}

GpStatus 
gdip_load_emf_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_get_metafile_from ((void*)fp, (GpMetafile**)image, File);
}

GpStatus 
gdip_load_emf_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return gdip_get_metafile_from ((void *)loader, (GpMetafile**)image, DStream);
}
