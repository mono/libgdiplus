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

#include "metafile-private.h"
#include "solidbrush-private.h"
#include "general-private.h"
#include "graphics.h"
#include "graphics-path-private.h"
#include "hatchbrush-private.h"
#include "pen.h"

//#define DEBUG_METAFILE

/* http://wvware.sourceforge.net/caolan/SaveDC.html */
GpStatus
gdip_metafile_SaveDC (MetafilePlayContext *context)
{
#ifdef DEBUG_METAFILE
	printf ("SaveDC");
#endif
	/* TODO */
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/SetBkMode.html */
GpStatus
gdip_metafile_SetBkMode (MetafilePlayContext *context, DWORD bkMode)
{
#ifdef DEBUG_METAFILE
	printf ("SetBkMode %d", bkMode);
#endif
	/* TODO - currently unused elsewhere */
	context->bk_mode = bkMode;
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/SetMapMode.html */
GpStatus
gdip_metafile_SetMapMode (MetafilePlayContext *context, DWORD mode)
{
	GpStatus status;
	float scale;
#ifdef DEBUG_METAFILE
	printf ("SetMapMode %d", mode);
#endif
	context->map_mode = mode;

	switch (mode) {
	case MM_HIENGLISH:
		/* 1 logical unit == 0.001 inch */
		scale = gdip_get_display_dpi () * 0.001;
		break;
	case MM_LOENGLISH:
		/* 1 logical unit == 0.01 inch */
		scale = gdip_get_display_dpi () * 0.01;
		break;
	case MM_HIMETRIC:
		/* 1 logical unit == 0.01 mm */
		scale = gdip_get_display_dpi () / (MM_PER_INCH * 100);
		break;
	case MM_LOMETRIC:
		/* 1 logical unit == 0.1 mm */
		scale = gdip_get_display_dpi () / (MM_PER_INCH * 10);
		break;
	case MM_TWIPS:
		/* 1 logical point == 1/1440 inch (1/20 of a "old" printer point ;-) */
		scale = gdip_get_display_dpi () / 1440;
		break;
	case MM_TEXT:
	default:
		/* 1 logical unit == 1 pixel */
		scale = 1.0f;
		context->map_mode = MM_TEXT;
		break;
	case MM_ISOTROPIC:
	case MM_ANISOTROPIC:
		/* SetWindowExt will calculate the correct ratio */
		return Ok;
	}

	/* this isn't cumulative (and we get a lot of "junk" calls) */
	GdipSetWorldTransform (context->graphics, &context->matrix);
	status = GdipScaleWorldTransform (context->graphics, scale, scale, MatrixOrderPrepend);
#ifdef DEBUG_METAFILE_2
	printf ("\n\tGdipScaleWorldTransform sx %g, sy %g (status %d)", scale, scale, status);
#endif
	return status;
}

/* http://wvware.sourceforge.net/caolan/SetROP2.html */
GpStatus
gdip_metafile_SetROP2 (MetafilePlayContext *context, DWORD rop)
{
#ifdef DEBUG_METAFILE
	printf ("SetROP2 %d", rop);
#endif
	/* TODO */
	return Ok;
}

/* http://www.ousob.com/ng/win2api/ng30859.php */
GpStatus
gdip_metafile_SetRelabs (MetafilePlayContext *context, DWORD mode)
{
#ifdef DEBUG_METAFILE
	printf ("SetRelabs %d", mode);
#endif
	/* TODO */
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/SetPolyFillMode.html */
GpStatus
gdip_metafile_SetPolyFillMode (MetafilePlayContext *context, DWORD fillmode)
{
#ifdef DEBUG_METAFILE
	printf ("SetPolyFillMode %d", fillmode);
#endif
	switch (fillmode) {
	case WINDING:
		context->fill_mode = FillModeWinding;
		break;
	default:
		g_warning ("Unknown fillmode %d, assuming ALTERNATE", fillmode);
		/* fall through */
	case ALTERNATE:
		context->fill_mode = FillModeAlternate;
		break;
	}
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/SetStretchBltMode.html */
GpStatus
gdip_metafile_SetStretchBltMode (MetafilePlayContext *context, int iStretchMode)
{
#ifdef DEBUG_METAFILE
	printf ("SetStretchBltMode %d", iStretchMode);
#endif
	/* TODO */
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/RestoreDC.html */
GpStatus
gdip_metafile_RestoreDC (MetafilePlayContext *context)
{
#ifdef DEBUG_METAFILE
	printf ("RestoreDC");
#endif
	/* TODO */
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/SelectObject.html */
GpStatus
gdip_metafile_SelectObject (MetafilePlayContext *context, DWORD slot)
{
	// EMF have some stock objects that do not requires creation
	if (slot & ENHMETA_STOCK_OBJECT) {
#ifdef DEBUG_METAFILE
		printf ("SelectObject stock #%X", slot);
#endif
		switch (slot - ENHMETA_STOCK_OBJECT) {
		case WHITE_BRUSH:
		case LTGRAY_BRUSH:
		case GRAY_BRUSH:
		case DKGRAY_BRUSH:
		case BLACK_BRUSH:
		case NULL_BRUSH:
			context->selected_brush = slot;
			break;
		case WHITE_PEN:
		case BLACK_PEN:
		case NULL_PEN:
			context->selected_pen = slot;
			break;
		case OEM_FIXED_FONT:
		case ANSI_FIXED_FONT:
		case ANSI_VAR_FONT:
		case SYSTEM_FONT:
		case DEVICE_DEFAULT_FONT:
		case SYSTEM_FIXED_FONT:
			context->selected_font = slot;
			break;
		case DEFAULT_PALETTE:
			context->selected_palette = slot;
			break;
		default:
			return InvalidParameter;
		}
		return Ok;
	}

#ifdef DEBUG_METAFILE
	printf ("SelectObject %d (out of %d slots)", slot, context->objects_count);
#endif
	if (slot >= context->objects_count) {
		g_warning ("SelectObject %d, invalid slot number.", slot);
		return InvalidParameter;
	}

	/* FIXME - what's first ? the created object or what's in the slot ? */
	switch (context->created.type) {
	case METAOBJECT_TYPE_EMPTY:
		/* if nothing is "created" (and not yet selected into a slot) then we "reselect" the object */
		switch (context->objects [slot].type) {
		case METAOBJECT_TYPE_EMPTY:
			g_warning ("SelectObject %d, no created object, slot empty.", slot);
			break;
		case METAOBJECT_TYPE_PEN:
			context->selected_pen = slot;
			break;
		case METAOBJECT_TYPE_BRUSH:
			context->selected_brush = slot;
			break;
		}
		return Ok;
	case METAOBJECT_TYPE_PEN:
		context->selected_pen = slot;
		break;
	case METAOBJECT_TYPE_BRUSH:
		context->selected_brush = slot;
		break;
	}

	context->objects [slot].type = context->created.type;
	context->objects [slot].ptr = context->created.ptr;

	context->created.type = METAOBJECT_TYPE_EMPTY;
	context->created.ptr = NULL;
	return Ok;
}

GpStatus
gdip_metafile_ModifyWorldTransform (MetafilePlayContext *context, XFORM *lpXform, DWORD iMode)
{
	GpMatrix matrix;
	GpMatrixOrder order;

	switch (iMode) {
	case MWT_IDENTITY:
		/* This is a reset and it ignores lpXform in this case */
		/* we can't use GdipResetWorldTransform as we still want the original parameters */
		return GdipSetWorldTransform (context->graphics, &context->matrix);
	case MWT_LEFTMULTIPLY:
		order = MatrixOrderPrepend;
		break;
	case MWT_RIGHTMULTIPLY:
		order = MatrixOrderPrepend;
		break;
	default:
		return InvalidParameter;
	}

	if (!lpXform)
		return InvalidParameter;
	matrix.xx = lpXform->eM11;
	matrix.yx = lpXform->eM12;
	matrix.xy = lpXform->eM21;
	matrix.yy = lpXform->eM22;
	matrix.x0 = lpXform->eDx;
	matrix.y0 = lpXform->eDy;
	return GdipMultiplyWorldTransform (context->graphics, &matrix, order);
}


/* http://wvware.sourceforge.net/caolan/SetTextAlign.html */
GpStatus
gdip_metafile_SetTextAlign (MetafilePlayContext *context, DWORD textalign)
{
#ifdef DEBUG_METAFILE
	printf ("SetTextAlign %d", textalign);
#endif
	/* TODO */
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/DeleteObject.html */
GpStatus
gdip_metafile_DeleteObject (MetafilePlayContext *context, DWORD slot)
{
	GpStatus status = Ok;
	MetaObject *obj;

	if (slot >= context->objects_count) {
		g_warning ("DeleteObject failure");
		return InvalidParameter;
	}

	obj = &context->objects [slot];
	switch (obj->type) {
	case METAOBJECT_TYPE_PEN:
		status = GdipDeletePen ((GpPen*)obj->ptr);
		break;
	case METAOBJECT_TYPE_BRUSH:
		status = GdipDeleteBrush ((GpBrush*)obj->ptr);
		break;
	case METAOBJECT_TYPE_EMPTY:
		break;
	}

#ifdef DEBUG_METAFILE
	printf ("DeleteObject %d (type %d, ptr %p, status %d)", slot, obj->type, obj->ptr, status);
#endif
	obj->type = METAOBJECT_TYPE_EMPTY;
	obj->ptr = NULL;
	return status;
}

/* http://wvware.sourceforge.net/caolan/SetBkColor.html */
GpStatus
gdip_metafile_SetBkColor (MetafilePlayContext *context, DWORD color)
{
#ifdef DEBUG_METAFILE
	printf ("SetBkColor %X", color);
#endif
	/* TODO - currently unused elsewhere */
	context->bk_color = color;
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/SetWindowOrg.html */
GpStatus
gdip_metafile_SetWindowOrg (MetafilePlayContext *context, int x, int y)
{
#ifdef DEBUG_METAFILE
	printf ("SetWindowOrg X: %d, Y: %d", x, y);
#endif
	/* TODO */
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/SetWindowExt.html */
GpStatus
gdip_metafile_SetWindowExt (MetafilePlayContext *context, int height, int width)
{
	GpStatus status;
	float sx, sy;

#ifdef DEBUG_METAFILE
	printf ("SetWindowExt height %d, width %d", height, width);
#endif
	switch (context->map_mode) {
	case MM_ISOTROPIC:
		sx = (float)context->metafile->metafile_header.Width / width;
		sy = (float)context->metafile->metafile_header.Height / height;
		/* keeps ratio to 1:1 */
		if (sx < sy)
			sy = sx;
		break;
	case MM_ANISOTROPIC:
		sx = (float)context->metafile->metafile_header.Width / width;
		sy = (float)context->metafile->metafile_header.Height / height;
		break;
	default:
		/* most cases are handled by SetMapMode */
		return Ok;
	}

	/* this isn't cumulative (and we get a lot of "junk" calls) */
	GdipSetWorldTransform (context->graphics, &context->matrix);
	status = GdipScaleWorldTransform (context->graphics, sx, sy, MatrixOrderPrepend);
#ifdef DEBUG_METAFILE_2
	printf ("\n\tGdipScaleWorldTransform sx %g, sy %g (status %d)", sx, sy, status);
#endif
	return status;
}

/* http://wvware.sourceforge.net/caolan/LineTo.html */
GpStatus
gdip_metafile_LineTo (MetafilePlayContext *context, int x, int y)
{
	GpStatus status;
#ifdef DEBUG_METAFILE
	printf ("LineTo %s%d, %d", context->use_path ? "Path " : " ", x, y);
#endif
	if (context->use_path) {
		status = GdipAddPathLine (context->path, context->current_x, context->current_y, x, y);
	} else {
		GpPen *pen = gdip_metafile_GetSelectedPen (context);
		status = GdipDrawLine (context->graphics, pen, context->current_x, context->current_y, x, y);
	}
	context->current_x = x;
	context->current_y = y;
	return status;
}

/* http://wvware.sourceforge.net/caolan/MoveTo.html */
GpStatus
gdip_metafile_MoveTo (MetafilePlayContext *context, int x, int y)
{
#ifdef DEBUG_METAFILE
	printf ("MoveTo X: %d, Y: %d", x, y);
#endif
	/* seems to always be called before Arc but, according to documentation, 
	   Arc doesn't depend on the current cursor position */
	context->current_x = x;
	context->current_y = y;
	return Ok;
}

GpStatus
gdip_metafile_SetMiterLimit (MetafilePlayContext *context, float eNewLimit, float *peOldLimit)
{
	if (peOldLimit)
		*peOldLimit = context->miter_limit;
	/* GDI+ keeps the miter limit with it's pen, not the context, definition */
	context->miter_limit = eNewLimit;
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/CreatePenIndirect.html */
GpStatus
gdip_metafile_CreatePenIndirect (MetafilePlayContext *context, DWORD style, DWORD width, DWORD color)
{
	LOGBRUSH brush;
	brush.lbStyle = 0;
	brush.lbColor = color;
	brush.lbHatch = 0;
	return gdip_metafile_ExtCreatePen (context, style, width, &brush, 0, NULL);
}

GpStatus
gdip_metafile_ExtCreatePen (MetafilePlayContext *context, DWORD dwPenStyle, DWORD dwWidth, CONST LOGBRUSH *lplb, 
	DWORD dwStyleCount, CONST DWORD *lpStyle)
{
	/* TODO - there's more cases to consider */
	GpStatus status;
	GpPen *pen = NULL;
	GpLineCap line_cap = LineCapRound;
	GpLineJoin line_join = LineJoinRound;
	int s = dwPenStyle & PS_STYLE_MASK;
	DWORD color = lplb->lbColor;

#ifdef DEBUG_METAFILE
	printf ("ExtCreatePenIndirect style %d, width %d, color %X", dwPenStyle, dwWidth, color);
#endif
	if (s == PS_NULL)
		color &= 0x00FFFFFF;
	else
		color |= 0xFF000000;

	status = GdipCreatePen1 (color, dwWidth, UnitPixel, &pen);
	if (status != Ok)
		return status;

	/* The style is always solid for width > 1 */
	if (dwWidth > 1) {
		switch (s) {
		default:
			g_warning ("Invalid pen style %d, style & PS_STYLE_MASK %d", dwPenStyle, s);
			/* fall through */
		case PS_NULL:
		case PS_SOLID:
			break;
		case PS_DASH:
		case PS_DOT:
		case PS_DASHDOT:
		case PS_DASHDOTDOT:
			status = GdipSetPenDashStyle (pen, s);
			if (status != Ok) {
				GdipDeletePen (pen);
				return status;
			}
			break;
		}
	}

	/* at this stage we got a pen, so we won't abort drawing on it's style */
	s = (dwPenStyle & PS_TYPE_MASK);
	if (s == PS_GEOMETRIC) {
		s = (dwPenStyle & PS_ENDCAP_MASK);
		switch (s) {
		default:
			g_warning ("Invalid pen endcap, style %d, (style & PS_ENDCAP_MASK) %d", dwPenStyle, s);
			/* fall through */
		case PS_ENDCAP_ROUND:
			line_cap = LineCapRound;
			break;
		case PS_ENDCAP_SQUARE:
			line_cap = LineCapSquare;
			break;
		case PS_ENDCAP_FLAT:
			line_cap = LineCapFlat;
			break;
		}
		GdipSetPenStartCap (pen, line_cap);
		GdipSetPenEndCap (pen, line_cap);
		
		s = (dwPenStyle & PS_JOIN_MASK);
		switch (s) {
		default:
			g_warning ("Invalid pen join, style %d, (style & PS_JOIN_MASK) %d", dwPenStyle, s);
			/* fall through */
		case PS_JOIN_ROUND:
			line_join = LineJoinRound;
			break;
		case PS_JOIN_BEVEL:
			line_join = LineJoinBevel;
			break;
		case PS_JOIN_MITER:
			line_join = LineJoinMiter;
			break;
		}
		GdipSetPenLineJoin (pen, line_join);
	}

	context->created.type = METAOBJECT_TYPE_PEN;
	context->created.ptr = pen;
	return Ok;
}

/* http://wvware.sourceforge.net/caolan/CreateBrushIndirect.html */
GpStatus
gdip_metafile_CreateBrushIndirect (MetafilePlayContext *context, DWORD style, DWORD color, DWORD hatch)
{
	GpStatus status = Ok;
	void *brush;

	switch (style) {
	case BS_SOLID:
		color |= 0xFF000000;
		status = GdipCreateSolidFill (color, (GpSolidFill **) &brush);
		break;
	case BS_NULL:
		color &= 0x00FFFFFF;
		status = GdipCreateSolidFill (color, (GpSolidFill **) &brush);
		break;
	case BS_HATCHED:
		color |= 0xFF000000;
		status = GdipCreateHatchBrush (hatch, color, 0xFFFFFFFF, (GpHatch **) &brush);
		break;
	default:
		g_warning ("gdip_metafile_CreateBrushIndirect unimplemented style %d", style);
		status = GdipCreateSolidFill (color, (GpSolidFill **) &brush);
		break;
	}

#ifdef DEBUG_METAFILE
	printf ("CreateBrushIndirect style %d, color %X, hatch %d (%p, status %d)", style, color, hatch, brush, status);
#endif
	context->created.type = METAOBJECT_TYPE_BRUSH;
	context->created.ptr = brush;
	return status;
}

/* http://wvware.sourceforge.net/caolan/Arc.html */
GpStatus
gdip_metafile_Arc (MetafilePlayContext *context, int left, int top, int right, int bottom, 
	int xstart, int ystart, int xend, int yend)
{
#ifdef DEBUG_METAFILE
	printf ("Arc left %d, top %d, right %d, bottom %d, xstart %d, ystart %d, xend %d, yend %d", 
		left, top, right, bottom, xstart, ystart, xend, yend);
#endif

	/* Don't draw if the width or height are empty. */
	if ((right - left <= 0) || (bottom - top <= 0))
		return Ok;

	return GdipDrawArc (context->graphics, gdip_metafile_GetSelectedPen (context), left, top, 
		(right - left), (bottom - top), atan2 (ystart, xstart), atan2 (yend, xend));
}

/* http://wvware.sourceforge.net/caolan/Rectangle.html */
GpStatus
gdip_metafile_Rectangle (MetafilePlayContext *context, int bottomRect, int rightRect, int topRect, int leftRect)
{
	GpStatus status;
	int x = min (leftRect, rightRect);
	int y = min (topRect, bottomRect);
	int width = abs (rightRect - leftRect);
	int height = abs (bottomRect - topRect);

#ifdef DEBUG_METAFILE
	printf ("Rectangle bottom %d, right %d, top %d, left %d", 
		bottomRect, rightRect, topRect, leftRect);
#endif

	status = GdipFillRectangleI (context->graphics, gdip_metafile_GetSelectedBrush (context), x, y, width, height);
	if (status != Ok)
		return status;

	return GdipDrawRectangleI (context->graphics, gdip_metafile_GetSelectedPen (context), x, y, width, height);
}

GpStatus
gdip_metafile_SetPixel (MetafilePlayContext *context, DWORD color, int x, int y)
{
	color |= 0xFF000000;

	GpBrush *fill;
	GpStatus status = GdipCreateSolidFill (color, (GpSolidFill **) &fill);
	if (status != Ok)
		return status;

	status = GdipFillRectangle (context->graphics, fill, x, y, 1, 1);
	GdipDeleteBrush (fill);
	return status;
}


/* http://wvware.sourceforge.net/caolan/ora-wmf.html */
GpStatus
gdip_metafile_StretchDIBits (MetafilePlayContext *context, int XDest, int YDest, int nDestWidth, int nDestHeight, 
	int XSrc, int YSrc, int nSrcWidth, int nSrcHeight, CONST void *lpBits, CONST BITMAPINFO *lpBitsInfo, 
	UINT iUsage, DWORD dwRop)
{
	GpStatus status = OutOfMemory;
	GpImage *image = NULL;
	MemorySource ms;
#ifdef DEBUG_METAFILE
	printf ("StretchDIBits\n\t[XDest %d, YDest %d, nDestWidth %d, nDestHeight %d]", XDest, YDest, nDestWidth, nDestHeight);
	printf ("\n\t[XSrc %d, YSrc %d, nSrcWidth %d, nSrcHeight %d]", XSrc, YSrc, nSrcWidth, nSrcHeight);
	printf ("\n\tlpBits %p, lpBitsInfo %p, iUsage %d, dwRop %d", lpBits, lpBitsInfo, iUsage, dwRop);
	printf ("\n\tBITMAPINFO\n\t\tSize: %d", lpBitsInfo->bmiHeader.biSize);
	printf ("\n\t\tWidth: %d", lpBitsInfo->bmiHeader.biWidth);
	printf ("\n\t\tHeight: %d", lpBitsInfo->bmiHeader.biHeight);
	printf ("\n\t\tPlanes: %d", lpBitsInfo->bmiHeader.biPlanes);
	printf ("\n\t\tBitCount: %d", lpBitsInfo->bmiHeader.biBitCount);
	printf ("\n\t\tCompression: %d", lpBitsInfo->bmiHeader.biCompression);
	printf ("\n\t\tSizeImage: %d", lpBitsInfo->bmiHeader.biSizeImage);
	printf ("\n\t\tXPelsPerMeter: %d", lpBitsInfo->bmiHeader.biXPelsPerMeter);
	printf ("\n\t\tYPelsPerMeter: %d", lpBitsInfo->bmiHeader.biXPelsPerMeter);
	printf ("\n\t\tClrUsed: %d", lpBitsInfo->bmiHeader.biClrUsed);
	printf ("\n\t\tClrImportant: %d", lpBitsInfo->bmiHeader.biClrImportant);
#endif
	ms.ptr = (BYTE*)lpBitsInfo;
	if (lpBitsInfo->bmiHeader.biCompression == BI_RGB) {
		// Per the spec, if compression is RGB ImageSize must be ignored (and it should be zero anyway)
		// and calculated according to the following formula.
		ms.size = (floor ((lpBitsInfo->bmiHeader.biWidth * lpBitsInfo->bmiHeader.biPlanes * 
			lpBitsInfo->bmiHeader.biBitCount + 31) / 32) * 4) * abs (lpBitsInfo->bmiHeader.biHeight);
	} else {
		ms.size = lpBitsInfo->bmiHeader.biSizeImage;
	}
	ms.pos = 0;
	status = gdip_read_bmp_image (&ms, &image, Memory);
	if (status == Ok) {
		status = GdipDrawImageRectRect (context->graphics, image, XDest, YDest,
			nDestWidth, nDestHeight, XSrc, YSrc, nSrcWidth, nSrcHeight, UnitPixel, NULL, NULL, NULL);
	}
	if (image)
		GdipDisposeImage (image);
	return status;
}

/*
 * Return the selected GDI+ Pen to draw on the metafile or NULL if none is selected/valid.
 */
GpPen*
gdip_metafile_GetSelectedPen (MetafilePlayContext *context)
{
	GpPen *pen;

	// EMF have some stock objects that do not requires explicit creation
	if (context->selected_pen & ENHMETA_STOCK_OBJECT) {
		switch (context->selected_pen - ENHMETA_STOCK_OBJECT) {
		case WHITE_PEN:
			if (!context->stock_pen_white) {
				if (GdipCreatePen1 (0xFFFFFFFF, 0, UnitPixel, &context->stock_pen_white) != Ok)
					return NULL;
			}
			pen = context->stock_pen_white;
			break;
		case BLACK_PEN:
			if (!context->stock_pen_black) {
				if (GdipCreatePen1 (0xFF000000, 0, UnitPixel, &context->stock_pen_black) != Ok)
					return NULL;
			}
			pen = context->stock_pen_black;
			break;
		case NULL_PEN:
			if (!context->stock_pen_null) {
				if (GdipCreatePen1 (0x00000000, 0, UnitPixel, &context->stock_pen_null) != Ok)
					return NULL;
			}
			pen = context->stock_pen_null;
			break;
		default:
			return NULL;
		}
	} else {
		if ((context->selected_pen < 0) || (context->selected_pen >= context->objects_count)) {
			g_warning ("Invalid pen handle %d [0..%d[", context->selected_pen, context->objects_count);
			return NULL;
		}

		if (context->objects [context->selected_pen].type != METAOBJECT_TYPE_PEN) {
			g_warning ("Wrong object type %d, expected pen (%d)", context->objects [context->selected_pen].type, 
				METAOBJECT_TYPE_PEN);
			return NULL;
		}
#ifdef DEBUG_METAFILE
		printf ("\tusing pen #%d (%p)", context->selected_pen, context->objects [context->selected_pen].ptr);
#endif
		pen = (GpPen*) context->objects [context->selected_pen].ptr;
	}

	/* miter limit was global (i.e. context not pen specific) in GDI */
	GdipSetPenMiterLimit (pen, context->miter_limit);
	return pen;
}

/*
 * Return the selected GDI+ Brush to fill on the metafile or NULL if none is selected/valid.
 */
GpBrush*
gdip_metafile_GetSelectedBrush (MetafilePlayContext *context)
{
	// EMF have some stock objects that do not requires explicit creation
	if (context->selected_brush & ENHMETA_STOCK_OBJECT) {
		switch (context->selected_brush - ENHMETA_STOCK_OBJECT) {
		case WHITE_BRUSH:
			if (!context->stock_brush_white) {
				if (GdipCreateSolidFill (0xFFFFFFFF, &context->stock_brush_white) != Ok)
					return NULL;
			}
			return (GpBrush*)context->stock_brush_white;
		case LTGRAY_BRUSH:
			if (!context->stock_brush_ltgray) {
				if (GdipCreateSolidFill (0xFFBBBBBB, &context->stock_brush_ltgray) != Ok)
					return NULL;
			}
			return (GpBrush*)context->stock_brush_ltgray;
		case GRAY_BRUSH:
			if (!context->stock_brush_gray) {
				if (GdipCreateSolidFill (0xFF888888, &context->stock_brush_gray) != Ok)
					return NULL;
			}
			return (GpBrush*)context->stock_brush_gray;
		case DKGRAY_BRUSH:
			if (!context->stock_brush_dkgray) {
				if (GdipCreateSolidFill (0xFF444444, &context->stock_brush_dkgray) != Ok)
					return NULL;
			}
			return (GpBrush*)context->stock_brush_dkgray;
		case BLACK_BRUSH:
			if (!context->stock_brush_black) {
				if (GdipCreateSolidFill (0xFF000000, &context->stock_brush_black) != Ok)
					return NULL;
			}
			return (GpBrush*)context->stock_brush_black;
		case NULL_BRUSH:
			if (!context->stock_brush_null) {
				if (GdipCreateSolidFill (0x00000000, &context->stock_brush_null) != Ok)
					return NULL;
			}
			return (GpBrush*)context->stock_brush_null;
		default:
			return NULL;
		}
	}

	if ((context->selected_brush < 0) || (context->selected_brush >= context->objects_count)) {
		g_warning ("Invalid brush handle %d [0..%d[", context->selected_brush, context->objects_count);
		return NULL;
	}

	if (context->objects [context->selected_brush].type != METAOBJECT_TYPE_BRUSH) {
		g_warning ("Wrong object type %d, expected brush (%d)", context->objects [context->selected_brush].type, 
			METAOBJECT_TYPE_BRUSH);
		return NULL;
	}

#ifdef DEBUG_METAFILE
	printf ("\tusing brush #%d (%p)", context->selected_brush, context->objects [context->selected_brush].ptr);
#endif
	return (GpBrush*) context->objects [context->selected_brush].ptr;
}

GpStatus
gdip_metafile_PolyBezier (MetafilePlayContext *context, GpPointF *points, int count)
{
	GpStatus status;
#ifdef DEBUG_METAFILE
	printf ("PolyBezier %s count %d", context->use_path ? "Path " : " ", count);
#endif
	if (context->use_path) {
		status = GdipAddPathBeziers (context->path, points, count);
	} else {
		GpPen *pen = gdip_metafile_GetSelectedPen (context);
		return GdipDrawCurve (context->graphics, pen, points, count);
	}
	return status;
}

GpStatus
gdip_metafile_Polygon (MetafilePlayContext *context, GpPointF *points, int count)
{
#ifdef DEBUG_METAFILE
	printf ("Polygon %s count %d", context->use_path ? "Path " : " ", count);
#endif
	GpBrush *brush = gdip_metafile_GetSelectedBrush (context);
	GpStatus status = GdipFillPolygon (context->graphics, brush, points, count, context->fill_mode);
	if (status == Ok) {
		GpPen *pen = gdip_metafile_GetSelectedPen (context);
		status = GdipDrawPolygon (context->graphics, pen, points, count);
	}
	return status;
}

GpStatus
gdip_metafile_BeginPath (MetafilePlayContext *context)
{
	GpStatus status;

	if (context->path) {
		GdipDeletePath (context->path);
		context->path = NULL;
	}
	context->use_path = TRUE;
	status = GdipCreatePath (0, &context->path);
#ifdef DEBUG_METAFILE
	printf ("BeginPath %p", context->path);
#endif
	return status;
}

GpStatus
gdip_metafile_EndPath (MetafilePlayContext *context)
{
#ifdef DEBUG_METAFILE
	printf ("EndPath %p", context->path);
#endif
	context->use_path = FALSE;
	return Ok;
}

GpStatus
gdip_metafile_CloseFigure (MetafilePlayContext *context)
{
#ifdef DEBUG_METAFILE
	printf ("CloseFigure %p", context->path);
#endif
	return GdipClosePathFigures (context->path);
}

GpStatus
gdip_metafile_FillPath (MetafilePlayContext *context)
{
#ifdef DEBUG_METAFILE
	printf ("FillPath %p", context->path);
#endif
	GpBrush* brush = gdip_metafile_GetSelectedBrush (context);
	/* end path if required */
	if (context->use_path)
		gdip_metafile_EndPath (context);
	return GdipFillPath (context->graphics, brush, context->path);
}

GpStatus
gdip_metafile_StrokePath (MetafilePlayContext *context)
{
#ifdef DEBUG_METAFILE
	printf ("StrokePath %p", context->path);
#endif
	GpPen *pen = gdip_metafile_GetSelectedPen (context);
	/* end path if required */
	if (context->use_path)
		gdip_metafile_EndPath (context);
	return GdipDrawPath (context->graphics, pen, context->path);
}

GpStatus
gdip_metafile_StrokeAndFillPath (MetafilePlayContext *context)
{
	GpBrush *brush;
	GpStatus status;
#ifdef DEBUG_METAFILE
	printf ("StrokeAndFillPath %p", context->path);
#endif
	/* end path if required */
	if (context->use_path)
		gdip_metafile_EndPath (context);

	brush = gdip_metafile_GetSelectedBrush (context);
	status = GdipFillPath (context->graphics, brush, context->path);
	if (status == Ok) {
		GpPen *pen = gdip_metafile_GetSelectedPen (context);
		status = GdipDrawPath (context->graphics, pen, context->path);
	}
	return status;
}


static GpMetafile*
gdip_metafile_create ()
{
	GpMetafile* mf = (GpMetafile*) GdipAlloc (sizeof (GpMetafile));
	if (mf) {
		memset (&mf->base, 0, sizeof (GpImage));

		mf->base.type = ImageTypeMetafile;
		mf->delete = FALSE;
		mf->data = NULL;
		mf->length = 0;
		mf->recording = FALSE;
		mf->fp = NULL;
		mf->stream = NULL;
	}
	return mf;
}

GpStatus
gdip_metafile_clone (GpMetafile *metafile, GpMetafile **clonedmetafile)
{
	GpStatus status;
	GpMetafile *mf = gdip_metafile_create ();
	GpImage *base;
	if (!mf)
		return OutOfMemory;

	status = gdip_bitmap_clone (&metafile->base, &base);
	if (status != Ok) {
		GdipFree (mf);
		return status;
	}

	status = gdip_bitmap_setactive (base, NULL, 0);
	if (status != Ok) {
		GdipFree (mf);
		GdipFree (base);
		return status;
	}

	mf->base = *base;
	GdipFree(base);
	base = NULL;

	memcpy (&mf->metafile_header, &metafile->metafile_header, sizeof (MetafileHeader));
	if (metafile->length > 0) {
		mf->data = GdipAlloc (metafile->length);
		if (!mf->data) {
			GdipFree (mf);
			return OutOfMemory;
		}

		memcpy (mf->data, metafile->data, metafile->length);
		mf->length = metafile->length;
	}

	*clonedmetafile = mf;
	return Ok;
}

GpStatus
gdip_metafile_dispose (GpMetafile *metafile)
{
	if (!metafile)
		return InvalidParameter;

	/* TODO deal with "delete" flag */
	metafile->length = 0;
	if (metafile->data) {
		GdipFree (metafile->data);
		metafile->data = NULL;
	}

	if (metafile->recording)
		gdip_metafile_stop_recording (metafile);

	GdipFree (metafile);
	return Ok;
}

GpStatus
gdip_get_bitmap_from_metafile (GpMetafile *metafile, INT width, INT height, GpImage **thumbnail)
{
	if (width <= 0 || height <= 0) {
		switch (metafile->metafile_header.Type) {
		case MetafileTypeWmfPlaceable:
		case MetafileTypeWmf: {
			width = iround (metafile->metafile_header.Width / 1000.0f * gdip_get_display_dpi());
			height = iround (metafile->metafile_header.Height / 1000.0f * gdip_get_display_dpi());
			break;
		}
		case MetafileTypeEmf:
		case MetafileTypeEmfPlusOnly:
		case MetafileTypeEmfPlusDual: {
			width = metafile->metafile_header.Width;
			height = metafile->metafile_header.Height;
			break;
		}
		default:
			return GenericError;
		}
	}

	return GdipGetImageThumbnail ((GpImage *) metafile, width, height, thumbnail, NULL, NULL);
}

GpStatus
gdip_metafile_stop_recording (GpMetafile *metafile)
{
	/* TODO save current stuff */

	if (metafile->fp) {
		fclose (metafile->fp);
		metafile->fp = NULL;
	}
	if (metafile->stream) {
		/* it's not ours to close, just forget about it */
		metafile->stream = NULL;
	}
	/* we cannot open a new graphics instance on this metafile - recording is over */
	metafile->recording = FALSE;
	return Ok;
}

MetafilePlayContext*
gdip_metafile_play_setup (GpMetafile *metafile, GpGraphics *graphics, int x, int y, int width, int height)
{
	int i;
	MetaObject *obj;
	MetafilePlayContext *context;
	float scaleX;
	float scaleY;
	float translateX;
	float translateY;
	/* metafiles always render as 32bppRgb */

	if (!metafile || !graphics)
		return NULL;

	context = GdipAlloc (sizeof (MetafilePlayContext));
	if (!context) {
		return NULL;
	}

	context->metafile = metafile;
	context->graphics = graphics;
	context->use_path = FALSE;
	context->path = NULL;

	/* keep a copy for clean up */
	GdipGetWorldTransform (graphics, &context->initial);
#ifdef DEBUG_METAFILE
	MetafileHeader *mh = &metafile->metafile_header;
	GpMatrix *m = &context->initial;
	g_warning ("\nMetafileHeader X %d, Y %d, W %d, H %d", mh->X, mh->Y, mh->Width, mh->Height);
	g_warning ("\n\tinitial matrix %g %g %g %g %g %g", m->xx, m->yx, m->xy, m->yy, m->x0, m->y0);
#endif
	context->x = x;
	context->y = y;
	context->width = width;
	context->height = height;
	/* and keep an adjusted copy for providing "resets" */
	scaleX = (float) width / (float) metafile->metafile_header.Width;
	scaleY = (float) height / (float) metafile->metafile_header.Height;
	GdipScaleWorldTransform (graphics, scaleX, scaleY, MatrixOrderPrepend);
	translateX = -metafile->metafile_header.X + x / scaleX;
	translateY = -metafile->metafile_header.Y + y / scaleY;
	GdipTranslateWorldTransform (graphics, translateX, translateY, MatrixOrderPrepend);
	GdipGetWorldTransform (graphics, &context->matrix);

	/* defaults */
	context->fill_mode = FillModeAlternate;
	switch (context->metafile->metafile_header.Type) {
		case MetafileTypeWmfPlaceable:
		case MetafileTypeWmf:
			gdip_metafile_SetMapMode (context, MM_TWIPS);
			break;
		case MetafileTypeEmf:
		case MetafileTypeEmfPlusOnly:
		case MetafileTypeEmfPlusDual:
			gdip_metafile_SetMapMode (context, MM_TEXT);
			break;
		default:
			GdipFree (context);
			return NULL;
	}

	context->miter_limit = 10.0f;
	context->selected_pen =  ENHMETA_STOCK_OBJECT + BLACK_PEN;
	context->selected_brush = ENHMETA_STOCK_OBJECT + WHITE_BRUSH;
	context->selected_font = -1;
	context->selected_palette = -1;

	/* Create* functions store the object here */
	context->created.type = METAOBJECT_TYPE_EMPTY;
	context->created.ptr = NULL;

	/* stock objects */
	context->stock_pen_white = NULL;
	context->stock_pen_black = NULL;
	context->stock_pen_null = NULL;
	context->stock_brush_white = NULL;
	context->stock_brush_ltgray = NULL;
	context->stock_brush_gray = NULL;
	context->stock_brush_dkgray = NULL;
	context->stock_brush_black = NULL;
	context->stock_brush_null = NULL;

	/* SelectObject | DeleteObject works on this array */
	switch (context->metafile->metafile_header.Type) {
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
		context->objects_count = metafile->metafile_header.Header.Wmf.mtNoObjects;
		break;
	case MetafileTypeEmf:
	case MetafileTypeEmfPlusOnly:
	case MetafileTypeEmfPlusDual:
		context->objects_count = metafile->metafile_header.Header.Emf.nHandles + 1; /* 0 is reserved */
		break;
	default:
		GdipFree (context);
		return NULL;
	}
	context->objects = (MetaObject*) GdipAlloc (context->objects_count * sizeof (MetaObject));
	if (!context->objects) {
		GdipFree (context);
		return NULL;
	}
	obj = context->objects;
	for (i = 0; i < context->objects_count; i++) {
		obj->type = METAOBJECT_TYPE_EMPTY;
		obj->ptr = NULL;
		obj++;
	}

	return context;
}

GpStatus
gdip_metafile_play (MetafilePlayContext *context)
{
	if (!context || !context->metafile)
		return InvalidParameter;

	switch (context->metafile->metafile_header.Type) {
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
		return gdip_metafile_play_wmf (context);
	case MetafileTypeEmf:
		return gdip_metafile_play_emf (context);
	case MetafileTypeEmfPlusOnly:
	case MetafileTypeEmfPlusDual:
		return gdip_metafile_play_emf (context);
	default:
		g_warning ("Invalid metafile format %d", context->metafile->metafile_header.Type);
		break;
	}
	return NotImplemented;
}

GpStatus
gdip_metafile_play_cleanup (MetafilePlayContext *context)
{
	if (!context)
		return InvalidParameter;

	GdipSetWorldTransform (context->graphics, &context->initial);
	context->graphics = NULL;
	if (context->path) {
		GdipDeletePath (context->path);
		context->path = NULL;
	}
	context->created.type = METAOBJECT_TYPE_EMPTY;
	context->created.ptr = NULL;
	if (context->objects) {
		int i;
		MetaObject *obj = context->objects;
		/* free each object */
		for (i = 0; i < context->objects_count; i++) {
			gdip_metafile_DeleteObject (context, i);
			obj++;
		}
		GdipFree (context->objects);
		context->objects = NULL;
	}

	context->selected_pen = -1;
	context->selected_brush = -1;
	context->selected_font = -1;
	context->selected_font = -1;
	context->selected_palette = -1;

	/* stock objects */
	if (context->stock_pen_white)
		GdipDeletePen (context->stock_pen_white);
	if (context->stock_pen_black)
		GdipDeletePen (context->stock_pen_black);
	if (context->stock_pen_null)
		GdipDeletePen (context->stock_pen_null);
	if (context->stock_brush_white)
		GdipDeleteBrush ((GpBrush*)context->stock_brush_white);
	if (context->stock_brush_ltgray)
		GdipDeleteBrush ((GpBrush*)context->stock_brush_ltgray);
	if (context->stock_brush_gray)
		GdipDeleteBrush ((GpBrush*)context->stock_brush_gray);
	if (context->stock_brush_dkgray)
		GdipDeleteBrush ((GpBrush*)context->stock_brush_dkgray);
	if (context->stock_brush_black)
		GdipDeleteBrush ((GpBrush*)context->stock_brush_black);
	if (context->stock_brush_null)
		GdipDeleteBrush ((GpBrush*)context->stock_brush_null);

	GdipFree (context);
	return Ok;
}

static void
WmfPlaceableFileHeaderLE (WmfPlaceableFileHeader *wmfPlaceableFileHeader)
{
#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	/* header->Key is already adjusted */
	wmfPlaceableFileHeader->Hmf = GUINT16_FROM_LE (wmfPlaceableFileHeader->Hmf);
	wmfPlaceableFileHeader->BoundingBox.Left = GUINT16_FROM_LE (wmfPlaceableFileHeader->BoundingBox.Left);
	wmfPlaceableFileHeader->BoundingBox.Top = GUINT16_FROM_LE (wmfPlaceableFileHeader->BoundingBox.Top);
	wmfPlaceableFileHeader->BoundingBox.Right = GUINT16_FROM_LE (wmfPlaceableFileHeader->BoundingBox.Right);
	wmfPlaceableFileHeader->BoundingBox.Bottom = GUINT16_FROM_LE (wmfPlaceableFileHeader->BoundingBox.Bottom);
	wmfPlaceableFileHeader->Inch = GUINT16_FROM_LE (wmfPlaceableFileHeader->Inch);
	wmfPlaceableFileHeader->Reserved = GUINT32_FROM_LE (wmfPlaceableFileHeader->Reserved);
	wmfPlaceableFileHeader->Checksum = GUINT16_FROM_LE (wmfPlaceableFileHeader->Checksum);
#endif
}

static void
MetafileHeaderLE (MetafileHeader *header)
{
#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	header->Header.Wmf.mtType = GUINT16_FROM_LE (header->Header.Wmf.mtType);
	header->Header.Wmf.mtHeaderSize = GUINT16_FROM_LE (header->Header.Wmf.mtHeaderSize);
	header->Header.Wmf.mtVersion = GUINT16_FROM_LE (header->Header.Wmf.mtVersion);
	header->Header.Wmf.mtSize = GUINT32_FROM_LE (header->Header.Wmf.mtSize);
	header->Header.Wmf.mtNoObjects = GUINT16_FROM_LE (header->Header.Wmf.mtNoObjects);
	header->Header.Wmf.mtMaxRecord = GUINT32_FROM_LE (header->Header.Wmf.mtMaxRecord);
	header->Header.Wmf.mtNoParameters = GUINT16_FROM_LE (header->Header.Wmf.mtNoParameters);
#endif
}

static void
EnhMetaHeaderLE (ENHMETAHEADER3 *emf)
{
#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	/* emf->iType is already adjusted */
	emf->nSize = GUINT32_FROM_LE (emf->nSize);
	emf->rclBounds.left = GUINT32_FROM_LE (emf->rclBounds.left);
	emf->rclBounds.top = GUINT32_FROM_LE (emf->rclBounds.top);
	emf->rclBounds.right = GUINT32_FROM_LE (emf->rclBounds.right);
	emf->rclBounds.bottom = GUINT32_FROM_LE (emf->rclBounds.bottom);
	emf->rclFrame.left = GUINT32_FROM_LE (emf->rclFrame.left);
	emf->rclFrame.top = GUINT32_FROM_LE (emf->rclFrame.top);
	emf->rclFrame.right = GUINT32_FROM_LE (emf->rclFrame.right);
	emf->rclFrame.bottom = GUINT32_FROM_LE (emf->rclFrame.bottom);
	emf->dSignature = GUINT32_FROM_LE (emf->dSignature);
	emf->nVersion = GUINT32_FROM_LE (emf->nVersion);
	emf->nBytes = GUINT32_FROM_LE (emf->nBytes);
	emf->nRecords = GUINT32_FROM_LE (emf->nRecords);
	emf->nHandles = GUINT32_FROM_LE (emf->nHandles);

	emf->sReserved = GUINT32_FROM_LE (emf->sReserved);
	emf->nDescription = GUINT32_FROM_LE (emf->nDescription);
	emf->offDescription = GUINT32_FROM_LE (emf->offDescription);
	emf->nPalEntries = GUINT32_FROM_LE (emf->nPalEntries);
	emf->szlDevice.cx = GUINT32_FROM_LE (emf->szlDevice.cx);
	emf->szlDevice.cy = GUINT32_FROM_LE (emf->szlDevice.cy);
	emf->szlMillimeters.cx = GUINT32_FROM_LE (emf->szlMillimeters.cx);
	emf->szlMillimeters.cy = GUINT32_FROM_LE (emf->szlMillimeters.cy);
#endif
}

static GpStatus
combine_headers (GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, MetafileHeader *header)
{
	if (wmfPlaceableFileHeader) {
		header->Type = MetafileTypeWmfPlaceable;

		header->X = min (wmfPlaceableFileHeader->BoundingBox.Right, wmfPlaceableFileHeader->BoundingBox.Left);
		header->Y = min (wmfPlaceableFileHeader->BoundingBox.Top, wmfPlaceableFileHeader->BoundingBox.Bottom);
		header->Width = abs (wmfPlaceableFileHeader->BoundingBox.Right - wmfPlaceableFileHeader->BoundingBox.Left);
		header->Height= abs (wmfPlaceableFileHeader->BoundingBox.Bottom - wmfPlaceableFileHeader->BoundingBox.Top);

		/* The units of a metafile are twips (1/20 of a point). The Inch field contains the number
		*  of twips per inch used to represent the image. There are 1440 twips per inch by default.
		*  Use the default if the placeable header has no twips per inch specified. */
		if (wmfPlaceableFileHeader->Inch == 0)
			header->DpiX = 1440;
		else
			header->DpiX = wmfPlaceableFileHeader->Inch;
	} else {
		header->Type = MetafileTypeWmf;
		header->X = 0;
		header->Y = 0;
		/* TODO: GDI+ uses screen resolution for non-placeable metafiles */
		header->Width = 1280;
		header->Height= 1024;
		header->DpiX = gdip_get_display_dpi ();
	}

	header->DpiY = header->DpiX;
	header->Size = header->Header.Wmf.mtSize * 2;
	header->Version = header->Header.Wmf.mtVersion;
	header->EmfPlusFlags = 0;
	header->EmfPlusHeaderSize = 0;
	header->LogicalDpiX = 0;
	header->LogicalDpiY = 0;
	return Ok;
}

/* Handle the record based headers that can exists in EMF files */
/* Note: GDI+ doesn't report correctly all stuff within the headers :| */
static GpStatus
update_emf_header (MetafileHeader *header, BYTE* data, int length)
{
	GpStatus status = Ok;
	MetafilePlayContext context;
	GpMetafile mf;
	if (length < sizeof (DWORD))
		return Ok;

	DWORD *func = (DWORD*)data;

	switch (*func) {
	case EMR_HEADER:
		g_warning ("TODO - EMR_HEADER. Not common, need test case :)");
		break;
	case EMR_GDICOMMENT:
		/* this could be an embedded EmfPlusRecordTypeHeader */
		context.metafile = &mf;
		context.graphics = NULL; /* special case where we're not playing the metafile */
		status = GdiComment (&context, data, length);
		if (status == Ok) {
			header->Type = mf.metafile_header.Type;
			header->Version = mf.metafile_header.Version; /* GDI+ seems to report the object header */
			/* Horizontal and Vertical Logical DPI are available but not reported */
		}
		break;
	}
	return status;
}

static GpStatus
gdip_read_emf_header_optionals (ENHMETAHEADER3 *header, void *pointer, ImageSource source)
{
	/* This algorithm is taken from the specification: https://msdn.microsoft.com/en-us/library/cc230635.aspx. */
	const int HeaderRecordSize = sizeof (ENHMETAHEADER3);
	const int HeaderExtension1Size = sizeof (ENHMETAHEADER3) + sizeof (HeaderExtension1);

	/* Initialize header size to minimum. */
	int headerSize = HeaderRecordSize;

	/* Valid header record size? */
	if (header->nSize >= HeaderRecordSize)
	{
		/* Set HeaderSize to header record size. */
		headerSize = header->nSize;

		/* Valid description values? If so, set HeaderSize to description offset. */
		if (header->offDescription >= HeaderRecordSize && (header->offDescription + header->nDescription * 2) <= header->nSize)
			headerSize = header->offDescription;

		/* Header big enough to contain an extension? */
		if (headerSize >= HeaderExtension1Size)
		{
			/* Match GDI+ behaviour where missing header data is set to 0. */
			HeaderExtension1 extension;
			memset (&extension, 0, sizeof (HeaderExtension1));
			gdip_read_emf_data (pointer, (BYTE *) &extension, sizeof (HeaderExtension1), source);

			/* Valid pixel format values? */
			if (extension.offPixelFormat >= HeaderExtension1Size && (extension.offPixelFormat + extension.cbPixelFormat) <= header->nSize)
			{
				/* Pixel format before description? If so, set HeaderSize to pixel format offset. */
				if (extension.offPixelFormat > header->offDescription)
					headerSize = extension.offPixelFormat;
			}
		}
	}

	header->nSize = headerSize;
	return Ok;
}

static GpStatus
gdip_get_metafileheader_from (void *pointer, MetafileHeader *header, ImageSource source)
{
	int size;
	DWORD key;
	GpStatus status = NotImplemented;
	WmfPlaceableFileHeader aldus_header;
	ENHMETAHEADER3 *emf;

	/* peek at first DWORD to select the right format */
	size = sizeof (DWORD);
	if (gdip_read_wmf_data (pointer, (void*)&key, size, source) != size)
		return OutOfMemory;

#if G_BYTE_ORDER != G_LITTLE_ENDIAN
	key = GUINT32_FROM_LE (key);
#endif
	switch (key) {
	case ALDUS_PLACEABLE_METAFILE_KEY:
		aldus_header.Key = key;
		size = sizeof (WmfPlaceableFileHeader) - size;
		if (gdip_read_wmf_data(pointer, (BYTE*)(&aldus_header) + sizeof(DWORD), size, source) != size)
			return OutOfMemory;
#ifdef DEBUG_METAFILE
g_warning ("ALDUS_PLACEABLE_METAFILE key %d, hmf %d, L %d, T %d, R %d, B %d, inch %d, reserved %d, checksum %d", 
	aldus_header.Key, aldus_header.Hmf, aldus_header.BoundingBox.Left, aldus_header.BoundingBox.Top,
	aldus_header.BoundingBox.Right, aldus_header.BoundingBox.Bottom, aldus_header.Inch, aldus_header.Reserved,
	aldus_header.Checksum);
#endif
		size = sizeof (METAHEADER);
		if (gdip_read_wmf_data (pointer, (BYTE*)&header->Header.Wmf, size, source) != size)
			return OutOfMemory;

		WmfPlaceableFileHeaderLE (&aldus_header);
		MetafileHeaderLE (header);

		if (header->Header.Wmf.mtType != 1 && header->Header.Wmf.mtType != 2)
			return OutOfMemory;
		if (header->Header.Wmf.mtHeaderSize != 9)
			return OutOfMemory;
		if (header->Header.Wmf.mtVersion != 0x0100 && header->Header.Wmf.mtVersion != 0x0300)
			return OutOfMemory;

		status = combine_headers (&aldus_header, header);
		break;
	case WMF_TYPE_AND_HEADERSIZE_KEY:
		memcpy (&header->Header.Wmf, &key, size);

		size = sizeof (METAHEADER) - size;
		if (gdip_read_wmf_data (pointer, (BYTE*)(&header->Header.Wmf) + sizeof(DWORD), size, source) != size)
			return InvalidParameter;

		MetafileHeaderLE (header);
		status = combine_headers (NULL, header);
		break;
	case EMF_EMR_HEADER_KEY:
		emf = &header->Header.Emf;
		emf->iType = key;

		/* Match GDI+ behaviour where missing header data is set to 0. */
		size = sizeof (ENHMETAHEADER3) - size;
		memset ((BYTE *) emf + sizeof (DWORD), 0, size);
		gdip_read_emf_data (pointer, (BYTE *) emf + sizeof (DWORD), size, source);

		EnhMetaHeaderLE (&header->Header.Emf);

#ifdef DEBUG_METAFILE
g_warning ("EMF HEADER iType %d, nSize %d, Bounds L %d, T %d, R %d, B %d, Frame L %d, T %d, R %d, B %d, signature %X, version %d, bytes %d, records %d, handles %d, reserved %d, description %d, %d, palentries %d, device %d, %d, millimeters %d, %d", 
	emf->iType, emf->nSize, 
	emf->rclBounds.left, emf->rclBounds.top, emf->rclBounds.right, emf->rclBounds.bottom,
	emf->rclFrame.left, emf->rclFrame.top, emf->rclFrame.right, emf->rclFrame.bottom,
	emf->dSignature, emf->nVersion, emf->nBytes, emf->nRecords, emf->nHandles,
	emf->sReserved, emf->nDescription, emf->offDescription, emf->nPalEntries,
	emf->szlDevice.cx, emf->szlDevice.cy, emf->szlMillimeters.cx, emf->szlMillimeters.cy);
#endif
		if ((emf->iType != 1) || (emf->dSignature != 0x464D4520))
			return OutOfMemory;
		if (emf->nRecords < 2)
			return OutOfMemory;
		if (emf->nHandles == 0)
			return OutOfMemory;
		if (emf->nBytes < emf->nSize || emf->nBytes & 3)
			return OutOfMemory;
		if (emf->szlDevice.cx == 0 || emf->szlDevice.cy == 0)
			return OutOfMemory;
		if (emf->szlMillimeters.cx == 0 || emf->szlMillimeters.cy == 0)
			return OutOfMemory;

		header->Type = MetafileTypeEmf;

		/* Convert millimetres to inches to get the DPI for each dimension. */
		header->DpiX = emf->szlDevice.cx / (emf->szlMillimeters.cx / MM_PER_INCH);
		header->DpiY = emf->szlDevice.cy / (emf->szlMillimeters.cy / MM_PER_INCH);

		/* The bounding box of the metafile is derived from rclFrame, not rclBounds.
		 * We need to perform some unit conversions from 100s of millimetres to pixels. */
		REAL inchLeft = emf->rclFrame.left / (MM_PER_INCH * 100);
		REAL inchTop = emf->rclFrame.top / (MM_PER_INCH * 100);
		REAL inchRight = emf->rclFrame.right / (MM_PER_INCH * 100);
		REAL inchBottom = emf->rclFrame.bottom / (MM_PER_INCH * 100);
		
		header->X = iround (inchLeft * header->DpiX);
		header->Y = iround (inchTop * header->DpiY);

		/* The frame is inclusive, so we need to add 1. */
		header->Width = iround ((inchRight - inchLeft) * header->DpiX) + 1;
		header->Height = iround ((inchBottom - inchTop) * header->DpiY) + 1;

		header->Size = emf->nBytes;
		header->Version = emf->nVersion;
		/* We need to check for the EmfHeader record (can't be done at this stage) but some files still returns
		 * invalid values. E.g. Files with "strange" bounds also have insanely large EmfPlusHeaderSize and logical
		 * DPI values (with MS GDI+) */
		header->EmfPlusFlags = 0;
		header->EmfPlusHeaderSize = 0;
		header->LogicalDpiX = 0;
		header->LogicalDpiY = 0;

		status = gdip_read_emf_header_optionals (emf, pointer, source);
		break;
	default:
		status = OutOfMemory;
	}

#ifdef DEBUG_METAFILE
g_warning ("METAHEADER type %d, header %d, version %d, size %d, #obj %d, max rec %d, #params %d",
	header->Header.Wmf.mtType, header->Header.Wmf.mtHeaderSize, header->Header.Wmf.mtVersion,
	header->Header.Wmf.mtSize, header->Header.Wmf.mtNoObjects, header->Header.Wmf.mtMaxRecord,
	header->Header.Wmf.mtNoParameters);
#endif
	return status;
}

GpStatus
gdip_get_metafile_from (void *pointer, GpMetafile **metafile, ImageSource source)
{
	BOOL adjust_emf_headers = FALSE;
	GpStatus status = OutOfMemory;
	GpMetafile *mf = gdip_metafile_create ();
	if (!mf)
		goto error;

	/* decode metafile header */
	status = gdip_get_metafileheader_from (pointer, &mf->metafile_header, source);
	if (status != Ok)
		goto error;

	switch (mf->metafile_header.Type) {
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
		mf->base.image_format = WMF;
		/* note: mtSize is in WORD, mtSize contains the METAHEADER, mf->length is in bytes */
		mf->length = mf->metafile_header.Header.Wmf.mtSize * 2 - sizeof (METAHEADER);

		/* The file size has to be enough for an EOF record. */
		if (mf->length == 0)
		{
			if (mf->metafile_header.Header.Wmf.mtMaxRecord != 3)
			{
				status = OutOfMemory;
				goto error;
			}
		}
		else if (mf->length <= 4)
		{
			status = OutOfMemory;
			goto error;
		}

		break;
	case MetafileTypeEmf:
	case MetafileTypeEmfPlusOnly:
	case MetafileTypeEmfPlusDual:
		mf->base.image_format = EMF;
		mf->length = mf->metafile_header.Header.Emf.nBytes - mf->metafile_header.Header.Emf.nSize;
		adjust_emf_headers = TRUE;
		break;
	default:
		status = GenericError;
		goto error;
	}

	mf->data = (BYTE*) GdipAlloc (mf->length);
	if (!mf->data)
		goto error;

	/* Copy the data into memory for playback later. To match GDI+ behaviour, we don't validate that there is
	 * as much data as the header says. Instead, if the data length is invalid and there is no EOF record before
	 * we run out of space in the buffer playback will fail. */
	mf->length = gdip_read_wmf_data (pointer, (void *) mf->data, mf->length, source);

	if (adjust_emf_headers) {
		/* if the first EMF record is an EmfHeader (or an Header inside a Comment) then we have extra data to extract */
		status = update_emf_header (&mf->metafile_header, mf->data, mf->length);
		if (status != Ok)
			goto error;
	}

	*metafile = mf;
	return Ok;
error:
	if (mf)
		gdip_metafile_dispose (mf);
	*metafile = NULL;
	return status;
}

/* public (GDI+) functions */

GpStatus
GdipCreateMetafileFromFile (GDIPCONST WCHAR *file, GpMetafile **metafile)
{
	FILE *fp;
	char *file_name;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!file || !metafile)
		return InvalidParameter;

	file_name = (char *) utf16_to_utf8 ((const gunichar2 *)file, -1);
	if (!file_name)
		return InvalidParameter;

	fp = fopen (file_name, "rb");
	if (!fp) {
		GdipFree (file_name);
		return GenericError;
	}

	/* Match GDI+ behaviour by either returning Ok or GenericError. */
	status = gdip_get_metafile_from (fp, metafile, File);
	if (status != Ok)
		status = GenericError;

	fclose (fp);
	GdipFree (file_name);

	return status;
}

/*
 * GdipCreateMetafileFromStream will never be implemented, as 'stream' is a COM IStream ...
 */
GpStatus
GdipCreateMetafileFromStream (void *stream, GpMetafile **metafile)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!stream || !metafile)
		return InvalidParameter;

	return NotImplemented;
}
/*
 * instead we'll use delegates to load the metafile header with this function
 */
GpStatus GdipCreateMetafileFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	GpMetafile **metafile)
{
	GpStatus status = InvalidParameter;
	dstream_t *loader;

	if (!metafile)
		return InvalidParameter;

	loader = dstream_input_new (getBytesFunc, seekFunc);
	if (loader) {
		status = gdip_get_metafile_from (loader, metafile, DStream);
		dstream_free (loader);
	}
	return status;
}

GpStatus
GdipCreateMetafileFromEmf (HENHMETAFILE hEmf, BOOL deleteEmf, GpMetafile **metafile)
{
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!hEmf || !metafile)
		return InvalidParameter;

	switch (((GpMetafile *) hEmf)->metafile_header.Type) {
	case MetafileTypeEmf:
	case MetafileTypeEmfPlusOnly:
	case MetafileTypeEmfPlusDual:
		status = gdip_metafile_clone ((GpMetafile*)hEmf, metafile);
		if (status != Ok)
			return status;

		(*metafile)->delete = deleteEmf;
		return Ok;
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
	default:
		*metafile = NULL;
		return GenericError;
	}
}

GpStatus
GdipCreateMetafileFromWmf (HMETAFILE hWmf, BOOL deleteWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, GpMetafile **metafile)
{
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!hWmf || !metafile)
		return InvalidParameter;

	status = gdip_metafile_clone ((GpMetafile*)hWmf, metafile);
	if (status != Ok)
		return status;

	switch ((*metafile)->metafile_header.Type) {
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
		if (wmfPlaceableFileHeader) {
			status = GdipGetMetafileHeaderFromWmf (hWmf, wmfPlaceableFileHeader, &(*metafile)->metafile_header);
			if (status != Ok) {
				gdip_metafile_dispose (*metafile);
				*metafile = NULL;
				return status;
			}
		}

		(*metafile)->delete = deleteWmf;
		return Ok;
	case MetafileTypeEmf:
	case MetafileTypeEmfPlusOnly:
	case MetafileTypeEmfPlusDual:
	default:
		gdip_metafile_dispose (*metafile);
		*metafile = NULL;
		return GenericError;
	}
}

GpStatus
GdipGetMetafileHeaderFromWmf (HMETAFILE hWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, MetafileHeader *header)
{
	GpMetafile *mf;

	if (!hWmf || !wmfPlaceableFileHeader || !header)
		return InvalidParameter;

	mf = (GpMetafile*)hWmf;
	memcpy (header, &mf->metafile_header, sizeof (MetafileHeader));
	return combine_headers (wmfPlaceableFileHeader, header);
}

GpStatus
GdipGetMetafileHeaderFromEmf (HENHMETAFILE hEmf, MetafileHeader *header)
{
	GpMetafile* metafile = (GpMetafile*)hEmf;

	if (!metafile || !header)
		return InvalidParameter;

	switch (metafile->metafile_header.Type) {
	case MetafileTypeEmf:
	case MetafileTypeEmfPlusOnly:
	case MetafileTypeEmfPlusDual:
		return GdipGetMetafileHeaderFromMetafile (metafile, header);
	/* you can get a HENHMETAFILE from a WMF metafile but it seems (unit tests) that you can't use it as such */
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
	default:
		return InvalidParameter;
	}
}

GpStatus
GdipGetMetafileHeaderFromFile (GDIPCONST WCHAR *filename, MetafileHeader *header)
{
	FILE *fp;
	char *file_name;
	GpStatus status = InvalidParameter;

	if (!filename || !header)
		return InvalidParameter;

	file_name = (char *) utf16_to_utf8 ((const gunichar2 *)filename, -1);
	if (!file_name)
		return InvalidParameter;
	
	fp = fopen (file_name, "rb");
	if (fp) {
		status = gdip_get_metafileheader_from (fp, header, File);
		/* if EMF check for additional header record */
		if (header->Type == MetafileTypeEmf) {
			/* look for more details, possibly upgrading the metafile to MetafileTypeEmfPlusOnly or EMFPLUSDUAL */
			/* TODO */
		}
		fclose (fp);
	}
	GdipFree (file_name);
	return status;
}

/*
 * GdipGetMetafileHeaderFromStream will never be implemented, as 'stream' is a COM IStream ...
 */
GpStatus
GdipGetMetafileHeaderFromStream (void *stream, MetafileHeader *header)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!stream || !header)
		return InvalidParameter;

	return NotImplemented;
}
/*
 * instead we'll use delegates to load the metafile header with this function
 */
GpStatus 
GdipGetMetafileHeaderFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	MetafileHeader *header)
{
	GpStatus status = InvalidParameter;
	dstream_t *loader;

	if (!header)
		return status;

	loader = dstream_input_new (getBytesFunc, seekFunc);
	if (loader) {
		status = gdip_get_metafileheader_from (loader, header, DStream);
		/* if EMF check for additional header record */
		if (header->Type == MetafileTypeEmf) {
			/* look for more details, possibly upgrading the metafile to MetafileTypeEmfPlusOnly or EMFPLUSDUAL */
			/* TODO */
		}
		dstream_free (loader);
	}
	return status;
}

GpStatus
GdipGetMetafileHeaderFromMetafile (GpMetafile *metafile, MetafileHeader *header)
{
	if (!metafile || !header)
		return InvalidParameter;

	memcpy (header, &metafile->metafile_header, sizeof (MetafileHeader));
	return Ok;
}

GpStatus
GdipGetHemfFromMetafile (GpMetafile *metafile, HENHMETAFILE *hEmf)
{
	if (!metafile || !hEmf)
		return InvalidParameter;

	*hEmf = (HENHMETAFILE)metafile;
	return Ok;
}

GpStatus
GdipGetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT *metafileRasterizationLimitDpi)
{
	if (!metafile || !metafileRasterizationLimitDpi)
		return InvalidParameter;

	switch (metafile->metafile_header.Type) {
	case MetafileTypeEmf:
	case MetafileTypeEmfPlusDual:
		/* TODO */
		*metafileRasterizationLimitDpi = 0;
		return Ok;
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
	case MetafileTypeEmfPlusOnly:
	default:
		return WrongState;
	}
}

GpStatus
GdipSetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT metafileRasterizationLimitDpi)
{
	if (!metafile)
		return InvalidParameter;

	switch (metafile->metafile_header.Type) {
	case MetafileTypeEmf:
	case MetafileTypeEmfPlusDual:
		// values less than 10 left resolution unchanged (from GDI+ documentation)
		if (metafileRasterizationLimitDpi >= 10) {
			/* TODO */
		}
		return Ok;
	case MetafileTypeWmfPlaceable:
	case MetafileTypeWmf:
	case MetafileTypeEmfPlusOnly:
	default:
		return WrongState;
	}
}

GpStatus
GdipPlayMetafileRecord (GDIPCONST GpMetafile *metafile, EmfPlusRecordType recordType, UINT flags, UINT dataSize, GDIPCONST BYTE* data)
{
	if (!metafile || (dataSize && !data))
		return InvalidParameter;

	/* TODO */
	return NotImplemented;
}

GpStatus
GdipRecordMetafile (HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, MetafileFrameUnit frameUnit, 
	GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	GpMetafile *mf;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!referenceHdc || !frameRect || !metafile)
		return InvalidParameter;

	if ((type < EmfTypeEmfOnly) || (type > EmfTypeEmfPlusDual))
		return InvalidParameter;

	if ((frameUnit < MetafileFrameUnitPixel) || (frameUnit > MetafileFrameUnitGdi))
		return InvalidParameter;

	if (((frameRect->Width == 0) || (frameRect->Height == 0)) && (frameUnit != MetafileFrameUnitGdi))
		return GenericError;

	mf = gdip_metafile_create ();
	if (!mf)
		return OutOfMemory;

	mf->metafile_header.X = frameRect->X;
	mf->metafile_header.Y = frameRect->Y;
	mf->metafile_header.Width = frameRect->Width;
	mf->metafile_header.Height = frameRect->Height;
	mf->metafile_header.Size = 0;
	mf->metafile_header.Type = (MetafileType)type;
	mf->recording = TRUE;

	/* TODO - more stuff here! */

	*metafile = mf;
	return Ok;
}

GpStatus
GdipRecordMetafileI (HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect, MetafileFrameUnit frameUnit, 
	GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	GpRectF rect;

	if (!frameRect)
		return InvalidParameter;

	rect.X = frameRect->X;
	rect.Y = frameRect->Y;
	rect.Width = frameRect->Width;
	rect.Height = frameRect->Height;
	return GdipRecordMetafile (referenceHdc, type, (GDIPCONST GpRectF*) &rect, frameUnit, description, metafile);
}

GpStatus
GdipRecordMetafileFileName (GDIPCONST WCHAR *fileName, HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	GpStatus status;
	GpMetafile *mf = NULL;
	char *file_name;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!fileName)
		return InvalidParameter;

	file_name = (char *) utf16_to_utf8 ((const gunichar2 *)fileName, -1);
	if (!file_name) {
		*metafile = NULL;
		return InvalidParameter;
	}

	status = GdipRecordMetafile (referenceHdc, type, frameRect, frameUnit, description, &mf);
	if (status != Ok) {
		GdipFree (file_name);
		return status;
	}

	/* yep, an existing file is overwritten */
	mf->fp = fopen (file_name, "wb");

	GdipFree (file_name);
	*metafile = mf;
	return Ok;
}

GpStatus
GdipRecordMetafileFileNameI (GDIPCONST WCHAR *fileName, HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	GpRectF rect;

	if (!frameRect)
		return InvalidParameter;

	rect.X = frameRect->X;
	rect.Y = frameRect->Y;
	rect.Width = frameRect->Width;
	rect.Height = frameRect->Height;
	return GdipRecordMetafileFileName (fileName, referenceHdc, type, (GDIPCONST GpRectF*) &rect, frameUnit, description, metafile);
}

/*
 * GdipRecordMetafileStream and GdipRecordMetafileStreamI will never be implemented, as 'stream' is a COM IStream ...
 */
GpStatus
GdipRecordMetafileStream (void /* IStream */ *stream, HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	return NotImplemented;
}

GpStatus
GdipRecordMetafileStreamI (void /* IStream */ *stream, HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	return NotImplemented;
}
/*
 * instead we'll use delegates to create the metafile header with these functions
 */
GpStatus 
GdipRecordMetafileFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	GpStatus status;

	if (!putBytesFunc)
		return InvalidParameter;

	status = GdipRecordMetafile (referenceHdc, type, frameRect, frameUnit, description, metafile);
	if (status != Ok)
		return status;

	/* TODO - keep delegates around to write stuff */

	return Ok;
}

GpStatus 
GdipRecordMetafileFromDelegateI_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile)
{
	GpRectF rect;

	if (!frameRect)
		return InvalidParameter;

	rect.X = frameRect->X;
	rect.Y = frameRect->Y;
	rect.Width = frameRect->Width;
	rect.Height = frameRect->Height;
	return GdipRecordMetafileFromDelegate_linux (getHeaderFunc, getBytesFunc, putBytesFunc, seekFunc, closeFunc, sizeFunc,
		referenceHdc, type, (GDIPCONST GpRectF*) &rect, frameUnit, description, metafile);
}
