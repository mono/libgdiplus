/*
 * pen.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
 * Copyright (C) 2006 Novell, Inc (http://www.novell.com)
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
 *   Alexandre Pigolkine(pigolkine@gmx.de)
 *   Duncan Mak (duncan@ximian.com)
 *   Ravindra (rkumar@novell.com)
 *
 */

#include "pen-private.h"
#include "solidbrush-private.h"
#include "matrix-private.h"
#include "graphics-private.h"
#include "customlinecap-private.h"

static void 
gdip_pen_init (GpPen *pen)
{
        pen->color = 0;
        pen->brush = 0;
	pen->own_brush = FALSE;
        pen->width = 1;
        pen->miter_limit = 10;
        pen->line_join = LineJoinMiter;
	pen->dash_style = DashStyleSolid;
	pen->line_cap = LineCapFlat;
	pen->end_cap = LineCapFlat;		/* ignored, Cairo only support a single start/end line cap */
	pen->dash_cap = DashCapFlat;		/* ignored */
	pen->mode = PenAlignmentCenter;
	pen->dash_offset = 0;
	pen->dash_count = 0;
	pen->own_dash_array = FALSE;
	pen->dash_array = NULL;
	pen->compound_count = 0;
	pen->compound_array = NULL;
	pen->unit = UnitWorld;
	pen->changed = TRUE;
	pen->custom_start_cap = NULL;
	pen->custom_end_cap = NULL;
	cairo_matrix_init_identity (&pen->matrix);
}

static GpPen*
gdip_pen_new (void)
{
        GpPen *result = (GpPen *) GdipAlloc (sizeof (GpPen));

	if (result)
		gdip_pen_init (result);

        return result;
}

static cairo_line_join_t
convert_line_join (GpLineJoin join)
{
        switch (join) {

        case LineJoinMiter:
                return CAIRO_LINE_JOIN_MITER;

        case LineJoinBevel:
                return CAIRO_LINE_JOIN_BEVEL;

        case LineJoinRound:
                return CAIRO_LINE_JOIN_ROUND;
 
        case LineJoinMiterClipped:
        default:
                return CAIRO_LINE_JOIN_MITER;
        }
}

static cairo_line_cap_t
convert_line_cap (GpPen *pen)
{
        switch (pen->line_cap) {

	/* HACK - this keeps SWF (mostly) happy with results very similar to GDI+
	 * (under those specific cases) and also keeps the pen's functionalities 
	 * on par with GDI+
	 */
        case LineCapFlat:
		if (pen->dash_array || (pen->width > 1.0))
	                return CAIRO_LINE_CAP_BUTT;
		else
	                return CAIRO_LINE_CAP_SQUARE;
        
        case LineCapSquare:
                return CAIRO_LINE_CAP_SQUARE;

        case LineCapRound:
                return CAIRO_LINE_CAP_ROUND;                

        case LineCapTriangle:
        case LineCapNoAnchor:
        case LineCapSquareAnchor:
        case LineCapRoundAnchor:
        case LineCapDiamondAnchor:
        case LineCapArrowAnchor:
        case LineCapCustom:
        default:
                return CAIRO_LINE_CAP_BUTT;
        }
}

static double *
convert_dash_array (float *f, double width, int count)
{
        double *retval = GdipAlloc (sizeof (double) * count);
        int i;
        for (i = 0; i < count; i++) {
                retval[i] = (double) f[i] * width;
        }

        return retval;
}

GpStatus 
gdip_pen_setup (GpGraphics *graphics, GpPen *pen)
{
	GpStatus status;
	cairo_matrix_t product;
	double widthx;

	if (!graphics || !pen)
		return InvalidParameter;

	status = gdip_brush_setup (graphics, pen->brush);
	if (status != Ok)
		return status;

	cairo_matrix_init_identity (&product);

	/* Here we use product of pen->matrix and graphics->copy_of_ctm.
	 * This gives us absolute results with respect to graphics. We
	 * do following irrespective of the pen->changed state since graphics
	 * has its own matrix and we need to multiply that with pen->matrix
	 * every time we perform stroke operations. Graphics matrix gets
	 * reset to its own state after stroking.
	 */
	cairo_matrix_multiply (&product, &pen->matrix, graphics->copy_of_ctm);
	/* Pen scaling by 0 are supported by MS GDI+ but would error in Cairo, see bug #338233 */
	if (gdip_near_zero (product.xx) || gdip_near_zero (product.yy)) {
		/* *both* X and Y are affected if either is 0 */
		product.xx = product.yy = 0.0001f;
	}
	cairo_set_matrix (graphics->ct, &product);

	/* Don't need to setup, if pen is the same as the cached pen and
	 * it is not changed. Just comparing pointers may not be sufficient
	 * to say that the pens are same. It is possible to have different
	 * pen on the same memory, but probability is very low. We would
	 * need a function to check the equality of the pens in that case.
	 */
	if (pen == graphics->last_pen && !pen->changed)
		return Ok;

	if (pen->width < 1.0) { /* we draw a pixel wide line if width is < 1.0 */
		double widthy = 1.0;
		widthx = 1.0;

		cairo_device_to_user_distance (graphics->ct, &widthx, &widthy);
	} else {
		widthx = (double) pen->width;
	}
	cairo_set_line_width (graphics->ct, widthx);

        cairo_set_miter_limit (graphics->ct, (double) pen->miter_limit);
        cairo_set_line_join (graphics->ct, convert_line_join (pen->line_join));
        cairo_set_line_cap (graphics->ct, convert_line_cap (pen));

        if (pen->dash_count > 0) {
                double *dash_array;

		/* note: pen->width may be different from what was used to 
		   call cairo_set_line_width, e.g. 0.0 (#78742) */
                dash_array = convert_dash_array (pen->dash_array, widthx, pen->dash_count);
                cairo_set_dash (graphics->ct, dash_array, pen->dash_count, pen->dash_offset);
                GdipFree (dash_array);
        } else /* Clear the dashes, if set in previous calls */
		cairo_set_dash (graphics->ct, NULL, 0, 0);

	/* We are done with using all the changes in the pen. */
	pen->changed = FALSE;
	graphics->last_pen = pen;

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
gdip_pen_draw_custom_start_cap (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2)
{
	if (!graphics || !pen)
		return InvalidParameter;
	
	if (pen->custom_start_cap) {
		/* Draw the end cap */
		gdip_linecap_draw (graphics, pen, pen->custom_start_cap, x1, y1, x2, y2);
	}

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
gdip_pen_draw_custom_end_cap (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2)
{
	if (!graphics || !pen)
		return InvalidParameter;
	
	if (pen->custom_end_cap) {
		/* Draw the end cap */
		gdip_linecap_draw (graphics, pen, pen->custom_end_cap, x1, y1, x2, y2);
	}

	return gdip_get_status (cairo_status (graphics->ct));
}

// coverity[+alloc : arg-*3]
GpStatus 
GdipCreatePen1 (ARGB argb, float width, GpUnit unit, GpPen **pen)
{
        GpStatus s;
	GpSolidFill *solidBrush = NULL;
	GpPen *result;

	if (!pen)
		return InvalidParameter;

	*pen = result = gdip_pen_new ();
	if (!result)
		return OutOfMemory;

	result->color = argb;
	/* FIXME: do unit conversion when setting width */
	result->width = width;

	s = GdipCreateSolidFill (argb, &solidBrush);
	if (s != Ok) {
		if (solidBrush)
			GdipDeleteBrush ((GpBrush*) solidBrush);
		GdipFree (result);
		*pen = NULL;
		return s;
	}

	result->brush = (GpBrush*) solidBrush;
	result->own_brush = TRUE;
	return Ok;
}

// coverity[+alloc : arg-*3]
GpStatus
GdipCreatePen2 (GpBrush *brush, float width, GpUnit unit, GpPen **pen)
{
        ARGB color;
        GpStatus s;
        GpBrushType type;
	GpPen *result;

	if (!brush || !pen)
		return InvalidParameter;

        *pen = result = gdip_pen_new ();
	if (!result)
		return OutOfMemory;

        /* FIXME: do unit conversion when setting width */
        result->width = width;

	/* the user supplied brush can be disposed, we must clone it to ensure
	 * it's valid when we need to set the pen */
	s = GdipCloneBrush (brush, &result->brush);
	if (s != Ok) {
		GdipFree (result);
		*pen = NULL;
		return s;
	}

	result->own_brush = TRUE;

        s = GdipGetBrushType (brush, &type);
	if (s != Ok) {
		GdipFree (result);
		*pen = NULL;
		return s;
	}

        switch (type) {

        case BrushTypeSolidColor:
                s = GdipGetSolidFillColor ((GpSolidFill*) brush, &color);
		if (s != Ok) {
			GdipFree (result);
			*pen = NULL;
			return s;
		}
                result->color = color;
		*pen = result;
                return Ok;

        case BrushTypeHatchFill:
        case BrushTypeTextureFill:
        case BrushTypePathGradient:
        case BrushTypeLinearGradient:
		*pen = result;
		return Ok;

        default:
		GdipFree (result);
		*pen = NULL;
                return GenericError;
        }
}

static float *
clone_dash_array (float *clone, float *array, int size)
{
        int i;

        for (i = 0; i < size; i++)
                clone [i] = array [i];

        return clone;
}

// coverity[+alloc : arg-*1]
GpStatus 
GdipClonePen (GpPen *pen, GpPen **clonepen)
{
        GpPen *result;
        float *dashes;                  /* copy off pen->dash_array */
        float *compound_array = NULL;   /* copy off pen->compound_array */
	GpCustomLineCap *custom_start_cap = NULL;
	GpCustomLineCap *custom_end_cap = NULL;

	if (!pen || !clonepen)
		return InvalidParameter;

	/* we make a copy of dash array only if it is owned by pen, i.e. it is not
	 * our global array.
	 */

	if (pen->dash_count > 0 && pen->own_dash_array) {
		dashes = (float *) GdipAlloc (pen->dash_count * sizeof (float));
		if (!dashes)
			return OutOfMemory;
		clone_dash_array (dashes, pen->dash_array, pen->dash_count);
	} else
		dashes = pen->dash_array;

	if (pen->compound_count > 0) {
		compound_array = (float *) GdipAlloc (pen->compound_count * sizeof (float));
		if (compound_array == NULL) {
			if (pen->dash_count > 0)
				GdipFree (dashes);
			return OutOfMemory;
		}
		clone_dash_array (compound_array, pen->compound_array, pen->compound_count);
	}

	if (pen->custom_start_cap != NULL) {
		GpStatus status = GdipCloneCustomLineCap (pen->custom_start_cap, &custom_start_cap);
		if (status != Ok) {
			if (custom_start_cap)
				GdipDeleteCustomLineCap (custom_start_cap);
			if (compound_array != NULL)
				GdipFree (compound_array);
			if (pen->dash_count > 0)
				GdipFree (dashes);
			return OutOfMemory;
		}
	}

	if (pen->custom_end_cap != NULL) {
		GpStatus status = GdipCloneCustomLineCap (pen->custom_end_cap, &custom_end_cap);
		if (status != Ok) {
			if (custom_end_cap)
				GdipDeleteCustomLineCap (custom_end_cap);
			if (custom_start_cap)
				GdipDeleteCustomLineCap (custom_start_cap);
			if (compound_array != NULL)
				GdipFree (compound_array);
			if (pen->dash_count > 0)
				GdipFree (dashes);
			return OutOfMemory;
		}
	}

	result = gdip_pen_new ();
	if (result == NULL) {
		if (pen->dash_count > 0)
			GdipFree (dashes);
		if (pen->compound_count > 0)
			GdipFree (compound_array);

		return OutOfMemory;
	}

	if (pen->own_brush) {
		GpSolidFill* solidBrush;
		GpSolidFill* oldbrush = (GpSolidFill*)pen->brush;
		GdipCreateSolidFill (oldbrush->color, &solidBrush);
		result->brush = (GpBrush*) solidBrush;
	} else {
		result->brush = pen->brush;
	}

        result->own_brush = pen->own_brush;
	result->color = pen->color;
        result->width = pen->width;
        result->miter_limit = pen->miter_limit;
        result->line_join = pen->line_join;
	result->dash_style = pen->dash_style;
        result->line_cap = pen->line_cap;
	result->end_cap = pen->end_cap;
	result->mode = pen->mode;
        result->dash_offset = pen->dash_offset;
	result->dash_count = pen->dash_count;
	result->own_dash_array = pen->own_dash_array;
	result->dash_array = dashes;
	result->compound_count = pen->compound_count;
	result->compound_array = compound_array;
	result->unit = pen->unit;
	gdip_cairo_matrix_copy (&result->matrix, &pen->matrix);
	result->changed = pen->changed;
	result->custom_start_cap = custom_start_cap;
	result->custom_end_cap = custom_end_cap;

        *clonepen = result;

        return Ok;
}       

GpStatus 
GdipDeletePen (GpPen *pen)
{
	if (!pen)
		return InvalidParameter;

        if (pen->dash_count != 0 && pen->own_dash_array) {
                GdipFree (pen->dash_array);
		pen->dash_count = 0;
		pen->dash_array = NULL;
	}

	if (pen->own_brush && pen->brush) {
		GdipDeleteBrush (pen->brush);
		pen->brush = NULL;
	}

        if (pen->compound_count != 0) {
                GdipFree (pen->compound_array);
		pen->compound_array = NULL;
		pen->compound_count = 0;
	}

	if (pen->custom_start_cap != NULL) {
		GdipDeleteCustomLineCap (pen->custom_start_cap);
		pen->custom_start_cap = NULL;
	}

	if (pen->custom_end_cap != NULL) {
		GdipDeleteCustomLineCap (pen->custom_end_cap);
		pen->custom_end_cap = NULL;
	}

        GdipFree (pen);
	return Ok;
}

GpStatus
GdipSetPenWidth (GpPen *pen, float width)
{
	if (!pen)
		return InvalidParameter;

        pen->width = width;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenWidth (GpPen *pen, float *width)
{
	if (!pen || !width)
		return InvalidParameter;

        *width = pen->width;
        return Ok;
}

GpStatus
GdipSetPenBrushFill (GpPen *pen, GpBrush *brush)
{
        GpStatus s;
        ARGB color;
        GpBrushType type;

	if (!pen || !brush)
		return InvalidParameter;

        s = GdipGetBrushType (brush, &type);

	if (s != Ok)
		return s;

        if (type == BrushTypeSolidColor) {
        	s = GdipGetSolidFillColor ((GpSolidFill*) brush, &color);
	        if (s != Ok)
	                return s;
        	pen->color = color;
        }
	else
		pen->color = 0;

	if (pen->own_brush && pen->brush)
		GdipDeleteBrush (pen->brush);

        pen->brush = brush;
	pen->changed = TRUE;
	pen->own_brush = FALSE;

        return Ok;
}

GpStatus
GdipGetPenBrushFill (GpPen *pen, GpBrush **brush)
{
	if (!pen)
		return InvalidParameter;

	return GdipCloneBrush (pen->brush, brush);
}

GpStatus
GdipGetPenFillType (GpPen *pen, GpPenType *type)
{
	if (!pen || !type)
		return InvalidParameter;

	if (pen->brush != NULL)
		return GdipGetBrushType (pen->brush, (GpBrushType *) type);

	*type = PenTypeSolidColor;

	return Ok;
}

GpStatus
GdipSetPenColor (GpPen *pen, ARGB argb)
{
	if (!pen)
		return InvalidParameter;

	pen->changed = pen->changed ? TRUE : (pen->color != argb);
	pen->color = argb;

	if (pen->changed && pen->brush && (pen->brush->vtable->type == BrushTypeSolidColor))
		return GdipSetSolidFillColor ((GpSolidFill*)pen->brush, argb);

	return Ok;
}

GpStatus
GdipGetPenColor (GpPen *pen, ARGB *argb)
{
	if (!pen || !argb)
		return InvalidParameter;

        *argb = pen->color;
        return Ok;
}

GpStatus
GdipSetPenMiterLimit (GpPen *pen, float miterLimit)
{
	if (!pen)
		return InvalidParameter;

	if (miterLimit < 1.0f)
		miterLimit = 1.0f;

	pen->changed = pen->changed ? TRUE : (pen->miter_limit != miterLimit);
        pen->miter_limit = miterLimit;
        return Ok;
}

GpStatus
GdipGetPenMiterLimit (GpPen *pen, float *miterLimit)
{
	if (!pen || !miterLimit)
		return InvalidParameter;

        *miterLimit = pen->miter_limit;
        return Ok;
}

GpStatus
GdipSetPenLineJoin (GpPen *pen, GpLineJoin lineJoin)
{
	if (!pen)
		return InvalidParameter;

	pen->changed = pen->changed ? TRUE : (pen->line_join != lineJoin);
        pen->line_join = lineJoin;
        return Ok;
}

GpStatus
GdipGetPenLineJoin (GpPen *pen, GpLineJoin *lineJoin)
{
	if (!pen || !lineJoin)
		return InvalidParameter;

        *lineJoin = pen->line_join;
        return Ok;
}

GpStatus
GdipSetPenLineCap197819 (GpPen *pen, GpLineCap startCap, GpLineCap endCap, GpDashCap dashCap)
{
	if (!pen)
		return InvalidParameter;

	/* FIXME:
	 * Cairo supports only one cap for a line. We use startcap for that.
	 * Use end cap and dash cap when Cairo supports different caps for the
	 * line ends and dashcap.
	 */

	pen->line_cap = startCap;
	pen->end_cap = endCap;
	/* any invalid value is changed to DashCapFlat */
	if ((dashCap == DashCapRound) || (dashCap == DashCapTriangle))
		pen->dash_cap = dashCap;
	else
		pen->dash_cap = DashCapFlat; /* default */

	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipSetPenMode (GpPen *pen, GpPenAlignment penMode)
{
	if (!pen)
		return InvalidParameter;

	pen->changed = pen->changed ? TRUE : (penMode != pen->mode);
	pen->mode = penMode;
	return Ok;
}

GpStatus
GdipGetPenMode (GpPen *pen, GpPenAlignment *penMode)
{
	if (!pen || !penMode)
		return InvalidParameter;

        *penMode = pen->mode;
        return Ok;
}

GpStatus
GdipGetPenUnit (GpPen *pen, GpUnit *unit)
{
	if (!pen || !unit)
		return InvalidParameter;

        *unit = pen->unit;
        return Ok;
}

GpStatus
GdipSetPenUnit (GpPen *pen, GpUnit unit)
{
	if (!pen)
		return InvalidParameter;

        pen->unit = unit;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipSetPenTransform (GpPen *pen, GpMatrix *matrix)
{
	GpStatus status;
	BOOL invertible;

	if (!pen || !matrix)
		return InvalidParameter;

	/* the matrix MUST be invertible to be used */
	status = GdipIsMatrixInvertible ((GpMatrix*) matrix, &invertible);
	if (!invertible || (status != Ok))
		return InvalidParameter;

	gdip_cairo_matrix_copy (&pen->matrix, matrix);
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenTransform (GpPen *pen, GpMatrix *matrix)
{
	if (!pen || !matrix)
		return InvalidParameter;

	gdip_cairo_matrix_copy(matrix, &pen->matrix);
        return Ok;
}

GpStatus
GdipResetPenTransform (GpPen *pen)
{
	if (!pen)
		return InvalidParameter;

	cairo_matrix_init_identity (&pen->matrix);

	pen->changed = TRUE;

	return Ok;
}

GpStatus
GdipMultiplyPenTransform (GpPen *pen, GpMatrix *matrix, GpMatrixOrder order)
{
	GpStatus status;
	BOOL invertible;

	if (!pen || !matrix)
		return InvalidParameter;

	/* the matrix MUST be invertible to be used */
	status = GdipIsMatrixInvertible ((GpMatrix*) matrix, &invertible);
	if (!invertible || (status != Ok))
		return InvalidParameter;

	/* invalid GpMatrixOrder values are computed as MatrixOrderAppend (i.e. no error) */
	if (order != MatrixOrderPrepend)
		order = MatrixOrderAppend;

	status = GdipMultiplyMatrix (&pen->matrix, matrix, order);
	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipTranslatePenTransform (GpPen *pen, float dx, float dy, GpMatrixOrder order)
{
	GpStatus status;

	if (!pen)
		return InvalidParameter;

	status = GdipTranslateMatrix (&pen->matrix, dx, dy, order);

	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipScalePenTransform (GpPen *pen, float sx, float sy, GpMatrixOrder order)
{
	GpStatus status;

	if (!pen)
		return InvalidParameter;

        status = GdipScaleMatrix (&pen->matrix, sx, sy, order);

	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipRotatePenTransform (GpPen *pen, float angle, GpMatrixOrder order)
{
	GpStatus status;

	if (!pen)
		return InvalidParameter;

	status = GdipRotateMatrix (&pen->matrix, angle, order);

	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipGetPenDashStyle (GpPen *pen, GpDashStyle *dashStyle)
{
	if (!pen || !dashStyle)
		return InvalidParameter;

        *dashStyle = pen->dash_style;
        return Ok;
}

static float Dot []  = { 1.0, 1.0 };
static float Dash []  = { 3.0, 1.0 };
static float DashDot [] = { 3.0, 1.0, 1.0, 1.0 };
static float DashDotDot [] = { 3.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

GpStatus
GdipSetPenDashStyle (GpPen *pen, GpDashStyle dashStyle)
{
	if (!pen)
		return InvalidParameter;

	switch (dashStyle) {
	case DashStyleSolid:
		pen->dash_array = NULL;
		pen->dash_count = 0;
		break;

	case DashStyleDashDot:
		pen->dash_array = DashDot;
		pen->dash_count = 4;
		break;

	case DashStyleDashDotDot:
		pen->dash_array = DashDotDot;
		pen->dash_count = 6;
		break;

	case DashStyleDot:
		pen->dash_array = Dot;
		pen->dash_count = 2;
		break;

	case DashStyleDash:
		pen->dash_array = Dash;
		pen->dash_count = 2;
		break;

	case DashStyleCustom:
		/* we keep the current assigned value when switching to Custom */
		/* other special stuff happens in System.Drawing (but not here) */
		break;

	default:
		return GenericError;
        }

	pen->dash_style = dashStyle;
	pen->changed = TRUE;
	return Ok;
}

GpStatus
GdipGetPenDashOffset (GpPen *pen, float *offset)
{
	if (!pen || !offset)
		return InvalidParameter;

        *offset = pen->dash_offset;
        return Ok;
}

GpStatus
GdipSetPenDashOffset (GpPen *pen, float offset)
{
	if (!pen)
		return InvalidParameter;

	pen->dash_offset = offset;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenDashCount (GpPen *pen, int *count)
{
	if (!pen || !count)
		return InvalidParameter;

        *count = pen->dash_count;
        return Ok;
}

/*
 * This is the DashPattern property in Pen.
 */
GpStatus
GdipGetPenDashArray (GpPen *pen, float *dash, int count)
{
	if (!pen || !dash)
		return InvalidParameter;
	if (count == 0)
		return OutOfMemory;
	if (count != pen->dash_count)
		return InvalidParameter;

	memcpy (dash, pen->dash_array, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetPenDashArray (GpPen *pen, GDIPCONST float *dash, int count)
{
	float *dash_array;

	if (!pen || !dash || (count <= 0))
		return InvalidParameter;

	if (pen->dash_count != count || pen->own_dash_array == FALSE) {
		dash_array = (float *) GdipAlloc (count * sizeof (float));
		if (!dash_array)
			return OutOfMemory;

		/* free the existing values, if we own them */
		if (pen->dash_count != 0 && pen->own_dash_array == TRUE)
			GdipFree (pen->dash_array);
		pen->dash_array = dash_array;
		pen->dash_count = count;
		pen->own_dash_array = TRUE;
	}

	memcpy (pen->dash_array, dash, pen->dash_count * sizeof (float));
	pen->dash_style = DashStyleCustom;
	pen->changed = TRUE;

	return Ok;
}

/*
 * MonoTODO: Find out what the difference is between CompoundArray and DashArray
 */
GpStatus
GdipGetPenCompoundCount (GpPen *pen, int *count)
{
	if (!pen || !count)
		return InvalidParameter;

	*count = pen->compound_count;
	return Ok;
}

GpStatus
GdipSetPenCompoundArray (GpPen *pen, const float *compound, int count)
{
	float *compound_array;

	if (!pen || !compound || (count <= 0))
		return InvalidParameter;

	if (pen->compound_count != count) {
		compound_array = (float *) GdipAlloc (count * sizeof (float));
		if (!compound_array)
			return OutOfMemory;

		/* free the existing values */
		if (pen->compound_count != 0)
			GdipFree (pen->compound_array);
		pen->compound_array = compound_array;
		pen->compound_count = count;
	}

	memcpy (pen->compound_array, compound, pen->compound_count * sizeof (float));

	return Ok;
}

GpStatus
GdipGetPenCompoundArray (GpPen *pen, float *compound, int count)
{
	if (!pen || !compound || (count != pen->compound_count))
		return InvalidParameter;

	memcpy (compound, pen->compound_array, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetPenStartCap (GpPen *pen, GpLineCap startCap)
{
	if (!pen)
		return InvalidParameter;

	pen->line_cap = startCap;
	pen->changed = TRUE;
	return Ok;
}

GpStatus
GdipGetPenStartCap (GpPen *pen, GpLineCap *startCap)
{
	if (!pen || !startCap)
		return InvalidParameter;

	*startCap = pen->line_cap;

	return Ok;
}

/* MonoTODO - ignored (always same as start cap) */
GpStatus
GdipSetPenEndCap (GpPen *pen, GpLineCap endCap)
{
	if (!pen)
		return InvalidParameter;

	/*
	 * This is currently ignored, as Cairo does not support
	 * having a different start and end Cap
	 */
	pen->end_cap = endCap;
	pen->changed = TRUE;

	return Ok;
}

/* MonoTODO - ignored (always same as start cap) */
GpStatus
GdipGetPenEndCap (GpPen *pen, GpLineCap *endCap)
{
	if (!pen || !endCap)
		return InvalidParameter;

	*endCap = pen->end_cap;

	return Ok;
}

/* MonoTODO - ignored */
GpStatus
GdipSetPenDashCap197819 (GpPen *pen, GpDashCap dashCap)
{
	if (!pen)
		return InvalidParameter;

	pen->dash_cap = dashCap;
	return Ok;
}

/* MonoTODO - ignored */
GpStatus
GdipGetPenDashCap197819 (GpPen *pen, GpDashCap *dashCap)
{
	if (!pen || !dashCap)
		return InvalidParameter;

	*dashCap = pen->dash_cap;
	return Ok;
}

GpStatus
GdipSetPenCustomStartCap (GpPen *pen, GpCustomLineCap *customCap)
{
	if (!pen)
		return InvalidParameter;
	
	return GdipCloneCustomLineCap (customCap, &pen->custom_start_cap);
}

GpStatus
GdipGetPenCustomStartCap (GpPen *pen, GpCustomLineCap **customCap)
{
	if (!pen || !customCap)
		return InvalidParameter;

	return GdipCloneCustomLineCap (pen->custom_start_cap, customCap);
}

GpStatus
GdipSetPenCustomEndCap (GpPen *pen, GpCustomLineCap *customCap)
{
	if (!pen)
		return InvalidParameter;
	
	return GdipCloneCustomLineCap (customCap, &pen->custom_end_cap);
}

GpStatus
GdipGetPenCustomEndCap (GpPen *pen, GpCustomLineCap **customCap)
{
	if (!pen || !customCap)
		return InvalidParameter;

	return GdipCloneCustomLineCap (pen->custom_end_cap, customCap);
}
