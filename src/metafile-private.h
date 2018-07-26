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

#ifndef __METAFILE_PRIVATE_H__
#define __METAFILE_PRIVATE_H__

#include "gdiplus-private.h"
#include "codecs-private.h"
#include "bitmap-private.h"
#include "matrix-private.h"
#include "bmpcodec.h"
#include "emfcodec.h"
#include "wmfcodec.h"

/*
 *	http://wvware.sourceforge.net/caolan/ora-wmf.html
 */

#define ALDUS_PLACEABLE_METAFILE_KEY	0x9AC6CDD7
#define WMF_TYPE_AND_HEADERSIZE_KEY	0x00090001
#define EMF_EMR_HEADER_KEY		0x1

#define MM_PER_INCH			25.4f

/* object types */
#define METAOBJECT_TYPE_EMPTY	0
#define METAOBJECT_TYPE_PEN	1
#define METAOBJECT_TYPE_BRUSH	2

#define gdip_get_metaheader(image)	(&((GpMetafile*)image)->metafile_header)

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
#define GETDW(x)	(*(DWORD*)(data + (x)))
#else
#define GETDW(x)	(GUINT32_FROM_LE(*(DWORD*)(data + (x))))
#endif

typedef struct {
	void *ptr;
	int type;
} MetaObject;

struct _Metafile {
	GpImage base;
	MetafileHeader metafile_header;
	BOOL delete;
	BYTE *data;
	int length;
	BOOL recording;		/* recording into memory (data), file (fp) or user stream (stream) */
	FILE *fp;
	void *stream;
};

typedef struct {
	GpMetafile *metafile;
	int x, y, width, height;
	int objects_count;
	MetaObject *objects;
	MetaObject created;
	GpGraphics *graphics;
	GpMatrix initial;
	GpMatrix matrix;
	DWORD bk_mode;
	DWORD bk_color;
	float miter_limit;
	int selected_pen;
	int selected_brush;
	int selected_font;
	int selected_palette;
	int map_mode;
	FillMode fill_mode;
	int current_x, current_y;
	/* path related data */
	BOOL use_path;
	GpPath *path;
	int path_x, path_y;
	/* stock objects */
	GpPen *stock_pen_white;
	GpPen *stock_pen_black;
	GpPen *stock_pen_null;
	GpSolidFill *stock_brush_white;
	GpSolidFill *stock_brush_ltgray;
	GpSolidFill *stock_brush_gray;
	GpSolidFill *stock_brush_dkgray;
	GpSolidFill *stock_brush_black;
	GpSolidFill *stock_brush_null;
	/* bitmap representation */
	BYTE *scan0;
} MetafilePlayContext;

typedef struct {
	int num;
	GpPointF *points;
} PointFList;

typedef struct {
	DWORD	cbPixelFormat;
	DWORD	offPixelFormat;
	DWORD	bOpenGL;
} HeaderExtension1;

GpStatus gdip_get_metafile_from (void *pointer, GpMetafile **metafile, ImageSource source) GDIP_INTERNAL;
GpStatus gdip_metafile_clone (GpMetafile *metafile, GpMetafile **clonedmetafile) GDIP_INTERNAL;
GpStatus gdip_metafile_dispose (GpMetafile *metafile) GDIP_INTERNAL;

GpStatus gdip_get_bitmap_from_metafile (GpMetafile *metafile, INT width, INT height, GpImage **thumbnail) GDIP_INTERNAL;

GpStatus gdip_metafile_stop_recording (GpMetafile *metafile) GDIP_INTERNAL;

GpStatus gdip_metafile_play_emf (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_play_wmf (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_play_emfplus_block (MetafilePlayContext *context, BYTE* data, int length) GDIP_INTERNAL;

MetafilePlayContext* gdip_metafile_play_setup (GpMetafile *metafile, GpGraphics *graphics, int x, int y, int width, 
	int height) GDIP_INTERNAL;
GpStatus gdip_metafile_play (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_play_cleanup (MetafilePlayContext *context) GDIP_INTERNAL;

GpPen* gdip_metafile_GetSelectedPen (MetafilePlayContext *context) GDIP_INTERNAL;
GpBrush* gdip_metafile_GetSelectedBrush (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus GdiComment (MetafilePlayContext *context, BYTE* data, DWORD size) GDIP_INTERNAL;

GpStatus gdip_metafile_SaveDC (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_SetBkMode (MetafilePlayContext *context, DWORD bkMode) GDIP_INTERNAL;
GpStatus gdip_metafile_SetMapMode (MetafilePlayContext *context, DWORD mode) GDIP_INTERNAL;
GpStatus gdip_metafile_SetROP2 (MetafilePlayContext *context, DWORD rop) GDIP_INTERNAL;
GpStatus gdip_metafile_SetRelabs (MetafilePlayContext *context, DWORD mode) GDIP_INTERNAL;
GpStatus gdip_metafile_SetPolyFillMode (MetafilePlayContext *context, DWORD mode) GDIP_INTERNAL;
GpStatus gdip_metafile_SetStretchBltMode (MetafilePlayContext *context, int iStretchMode) GDIP_INTERNAL;
GpStatus gdip_metafile_RestoreDC (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_SelectObject (MetafilePlayContext *context, DWORD slot) GDIP_INTERNAL;
GpStatus gdip_metafile_ModifyWorldTransform (MetafilePlayContext *context, XFORM *lpXform, DWORD iMode) GDIP_INTERNAL;
GpStatus gdip_metafile_SetTextAlign (MetafilePlayContext *context, DWORD textalign) GDIP_INTERNAL;
GpStatus gdip_metafile_DeleteObject (MetafilePlayContext *context, DWORD slot) GDIP_INTERNAL;
GpStatus gdip_metafile_SetBkColor (MetafilePlayContext *context, DWORD color) GDIP_INTERNAL;
GpStatus gdip_metafile_SetWindowOrg (MetafilePlayContext *context, int x, int y) GDIP_INTERNAL;
GpStatus gdip_metafile_SetWindowExt (MetafilePlayContext *context, int height, int width) GDIP_INTERNAL;
GpStatus gdip_metafile_LineTo (MetafilePlayContext *context, int x, int y) GDIP_INTERNAL;
GpStatus gdip_metafile_MoveTo (MetafilePlayContext *context, int x, int y) GDIP_INTERNAL;
GpStatus gdip_metafile_SetMiterLimit (MetafilePlayContext *context, float eNewLimit, float *peOldLimit) GDIP_INTERNAL;
GpStatus gdip_metafile_CreatePenIndirect (MetafilePlayContext *context, DWORD style, DWORD width, DWORD color) GDIP_INTERNAL;
GpStatus gdip_metafile_ExtCreatePen (MetafilePlayContext *context, DWORD dwPenStyle, DWORD dwWidth, CONST LOGBRUSH *lplb,
	DWORD dwStyleCount, CONST DWORD *lpStyle) GDIP_INTERNAL;
GpStatus gdip_metafile_CreateBrushIndirect (MetafilePlayContext *context, DWORD style, DWORD color, DWORD hatch) GDIP_INTERNAL;
GpStatus gdip_metafile_Arc (MetafilePlayContext *context, int left, int top, int right, int bottom, 
	int xstart, int ystart, int xend, int yend) GDIP_INTERNAL;
GpStatus gdip_metafile_Rectangle (MetafilePlayContext *context, int bottomRect, int rightRect, int topRect, int leftRect) GDIP_INTERNAL;
GpStatus gdip_metafile_SetPixel (MetafilePlayContext *context, DWORD color, int x, int y) GDIP_INTERNAL;
GpStatus gdip_metafile_StretchDIBits (MetafilePlayContext *context, int XDest, int YDest, int nDestWidth, int nDestHeight, 
	int XSrc, int YSrc, int nSrcWidth, int nSrcHeight, CONST void *lpBits, CONST BITMAPINFO *lpBitsInfo, 
	UINT iUsage, DWORD dwRop) GDIP_INTERNAL;
GpStatus gdip_metafile_PolyBezier (MetafilePlayContext *context, GpPointF *points, int count) GDIP_INTERNAL;
GpStatus gdip_metafile_Polygon (MetafilePlayContext *context, GpPointF *points, int count) GDIP_INTERNAL;
GpStatus gdip_metafile_BeginPath (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_EndPath (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_CloseFigure (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_FillPath (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_StrokePath (MetafilePlayContext *context) GDIP_INTERNAL;
GpStatus gdip_metafile_StrokeAndFillPath (MetafilePlayContext *context) GDIP_INTERNAL;

#include "metafile.h"

#endif
