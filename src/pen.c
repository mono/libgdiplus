/*
 * pen.c
 * 
 * Copyright (c) 2003 Alexandre Pigolkine
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

#include "gdip.h"
#include "solidbrush.h"

void 
gdip_pen_init (GpPen *pen)
{
        pen->color = 0;
        pen->brush = 0;
        pen->width = 1;
        pen->miter_limit = 10;
        pen->line_join = LineJoinMiter;
	pen->dash_style = DashStyleSolid;
	pen->line_cap = LineCapFlat;
	pen->mode = PenAlignmentCenter;
	pen->dash_offset = 0;
	pen->dash_count = 0;
	pen->own_dash_array = FALSE;
	pen->dash_array = NULL;
	pen->compound_count = 0;
	pen->compound_array = NULL;
	pen->unit = UnitWorld;
        pen->matrix = cairo_matrix_create ();
	pen->changed = TRUE;
}

GpPen*
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
convert_line_cap (GpLineCap cap)
{
        switch (cap) {
        
        case LineCapSquare:
                return CAIRO_LINE_CAP_SQUARE;

        case LineCapRound:
                return CAIRO_LINE_CAP_ROUND;                

        case LineCapFlat:
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
convert_dash_array (float *f, int count)
{
        double *retval = malloc (sizeof (double) * count);
        int i;
        for (i = 0; i < count; i++) {
                retval[i] = (double) f[i];
        }

        return retval;
}

GpStatus 
gdip_pen_setup (GpGraphics *graphics, GpPen *pen)
{
	GpStatus status;
	cairo_matrix_t *product;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	status = gdip_brush_setup (graphics, pen->brush);
	if (status != Ok)
		return status;

	/* Here we use product of pen->matrix and graphics->copy_of_ctm.
	 * This gives us absolute results with respect to graphics. We
	 * do following irrespective of the pen->changed state since we
	 * need to set the product matrix for subsequent stroke operations.
	 */
	product = cairo_matrix_create ();
	cairo_matrix_multiply (product, pen->matrix, graphics->copy_of_ctm);
	cairo_set_matrix (graphics->ct, product);
	cairo_matrix_destroy (product);

	/* Don't need to setup, if pen is not changed */
	/* This leads to a bug when we are using the same */
	/* graphics but with a different pen. See bug #66665 */
	/* Commented following to fix #66665. */
	/*	if (! pen->changed)
		return Ok; */

	if (pen->width <= 0) { /* we draw a pixel wide line if width is <=0 */
		double widthx = 1.0;
		double widthy = 1.0;
		cairo_inverse_transform_distance (graphics->ct, &widthx, &widthy);
		cairo_set_line_width (graphics->ct, widthx);
	} else
		cairo_set_line_width (graphics->ct, (double) pen->width);
	
        cairo_set_miter_limit (graphics->ct, (double) pen->miter_limit);
        cairo_set_line_join (graphics->ct, convert_line_join (pen->line_join));
        cairo_set_line_cap (graphics->ct, convert_line_cap (pen->line_cap));

        if (pen->dash_count > 0) {
                double *dash_array;

                dash_array = convert_dash_array (pen->dash_array, pen->dash_count);
                cairo_set_dash (graphics->ct, dash_array, pen->dash_count, pen->dash_offset);
                free (dash_array);
        } else /* Clear the dashes, if set in previous calls */
		cairo_set_dash (graphics->ct, NULL, 0, 0);

	/* We are done with using all the changes in the pen. */
	/* pen->changed = FALSE; */

	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipCreatePen1(int argb, float width, GpUnit unit, GpPen **pen)
{
        GpStatus s;
	GpSolidFill *solidBrush;

	g_return_val_if_fail (pen != NULL, InvalidParameter);

        *pen = gdip_pen_new ();
	g_return_val_if_fail (*pen != NULL, InvalidParameter);

        (*pen)->color = argb;

        /* FIXME: do unit conversion when setting width */
        (*pen)->width = width;

	s = GdipCreateSolidFill (argb, &solidBrush);
        (*pen)->brush = (GpBrush*) solidBrush;

        return s;
}

GpStatus
GdipCreatePen2 (GpBrush *brush, float width, GpUnit unit, GpPen **pen)
{
        int color;
        GpStatus s;
        GpBrushType type;

	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        *pen = gdip_pen_new ();
	g_return_val_if_fail (*pen != NULL, OutOfMemory);

        /* FIXME: do unit conversion when setting width */
        (*pen)->width = width;
	(*pen)->brush = brush;

        s = GdipGetBrushType (brush, &type);
	if (s != Ok)
		return s;

        switch (type) {

        case BrushTypeSolidColor:
                s = GdipGetSolidFillColor ((GpSolidFill*) brush, &color);
		if (s != Ok)
			return s;
                (*pen)->color = color;
                return Ok;

        case BrushTypeHatchFill:
        case BrushTypeTextureFill:
        case BrushTypePathGradient:
        case BrushTypeLinearGradient:
		return Ok;

        default:
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

GpStatus 
GdipClonePen (GpPen *pen, GpPen **clonepen)
{
        GpPen *result;
        GpMatrix *matrix;               /* copy of pen->matrix */
        float *dashes;                  /* copy off pen->dash_array */
        float *compound_array = NULL;   /* copy off pen->compound_array */

	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (clonepen != NULL, InvalidParameter);

	/* we make a copy of dash array only if it is owned by pen, i.e. it is not
	 * our global array.
	 */
	if (pen->dash_count > 0 && pen->own_dash_array) {
		dashes = (float *) GdipAlloc (pen->dash_count * sizeof (float));
		g_return_val_if_fail (dashes != NULL, OutOfMemory);
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

        if (GdipCloneMatrix (pen->matrix, &matrix) != Ok) {
		if (pen->dash_count > 0)
			GdipFree (dashes);
		if (pen->compound_count > 0)
			GdipFree (compound_array);

		return OutOfMemory;
	}

	result = gdip_pen_new ();
	if (result == NULL) {
		GdipDeleteMatrix (matrix);
		if (pen->dash_count > 0)
			GdipFree (dashes);
		if (pen->compound_count > 0)
			GdipFree (compound_array);

		return OutOfMemory;
	}

        result->color = pen->color;
	result->brush = pen->brush;
        result->width = pen->width;
        result->miter_limit = pen->miter_limit;
        result->line_join = pen->line_join;
	result->dash_style = pen->dash_style;
        result->line_cap = pen->line_cap;
	result->mode = pen->mode;
        result->dash_offset = pen->dash_offset;
	result->dash_count = pen->dash_count;
	result->own_dash_array = pen->own_dash_array;
	result->dash_array = dashes;
	result->compound_count = pen->compound_count;
	result->compound_array = compound_array;
	result->unit = pen->unit;
        result->matrix = matrix;
	result->changed = pen->changed;

        *clonepen = result;

        return Ok;
}       

GpStatus 
GdipDeletePen (GpPen *pen)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        if (pen->matrix != NULL)
                cairo_matrix_destroy (pen->matrix);
	pen->matrix = NULL;

        if (pen->dash_count != 0 && pen->own_dash_array) {
                GdipFree (pen->dash_array);
		pen->dash_count = 0;
		pen->dash_array = NULL;
	}

        if (pen->compound_count != 0)
                GdipFree (pen->compound_array);
	pen->compound_array = NULL;

        GdipFree (pen);

	return Ok;
}

GpStatus
GdipSetPenWidth (GpPen *pen, float width)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        pen->width = width;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenWidth (GpPen *pen, float *width)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (width != NULL, InvalidParameter);

        *width = pen->width;
        return Ok;
}

GpStatus
GdipSetPenBrushFill (GpPen *pen, GpBrush *brush)
{
        GpStatus s;
        int color;
        GpBrushType type;

	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

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

        pen->brush = brush;
	pen->changed = TRUE;

        return Ok;
}

GpStatus
GdipGetPenBrushFill (GpPen *pen, GpBrush **brush)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

        *brush = pen->brush;

        return Ok;
}

GpStatus
GdipGetPenFillType (GpPen *pen, GpPenType *type)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (type != NULL, InvalidParameter);

	if (pen->brush != NULL)
	  return GdipGetBrushType (pen->brush, (GpBrushType *) type);

	*type = PenTypeSolidColor;

	return Ok;
}

GpStatus
GdipSetPenColor (GpPen *pen, int argb)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        pen->color = argb;
	pen->changed = TRUE;

        return Ok;
}

GpStatus
GdipGetPenColor (GpPen *pen, int *argb)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (argb != NULL, InvalidParameter);

        *argb = pen->color;
        return Ok;
}

GpStatus
GdipSetPenMiterLimit (GpPen *pen, float miterLimit)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        pen->miter_limit = miterLimit;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenMiterLimit (GpPen *pen, float *miterLimit)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (miterLimit != NULL, InvalidParameter);

        *miterLimit = pen->miter_limit;
        return Ok;
}

GpStatus
GdipSetPenLineJoin (GpPen *pen, GpLineJoin lineJoin)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        pen->line_join = lineJoin;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenLineJoin (GpPen *pen, GpLineJoin *lineJoin)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (lineJoin != NULL, InvalidParameter);

        *lineJoin = pen->line_join;
        return Ok;
}

GpStatus
GdipSetPenLineCap197819 (GpPen *pen, GpLineCap startCap, GpLineCap endCap, GpDashCap dashCap)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	/* FIXME:
	 * Cairo supports only one cap for a line. We use startcap for that.
	 * Use end cap and dash cap when Cairo supports different caps for the
	 * line ends and dashcap.
	 */
	pen->line_cap = startCap;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipSetPenMode (GpPen *pen, GpPenAlignment penMode)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        pen->mode = penMode;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenMode (GpPen *pen, GpPenAlignment *penMode)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (penMode != NULL, InvalidParameter);

        *penMode = pen->mode;
        return Ok;
}

GpStatus
GdipGetPenUnit (GpPen *pen, GpUnit *unit)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (unit != NULL, InvalidParameter);

        *unit = pen->unit;
        return Ok;
}

GpStatus
GdipSetPenUnit (GpPen *pen, GpUnit unit)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        pen->unit = unit;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipSetPenTransform (GpPen *pen, GDIPCONST GpMatrix *matrix)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        *(pen->matrix) = *matrix;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenTransform (GpPen *pen, GpMatrix *matrix)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        *matrix = *(pen->matrix);
        return Ok;
}

GpStatus
GdipResetPenTransform (GpPen *pen)
{
	cairo_status_t status;
	GpStatus s;
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	status = cairo_matrix_set_identity (pen->matrix);
	s = gdip_get_status (status);

	if (s == Ok)
		pen->changed = TRUE;

	return s;
}

GpStatus
GdipMultiplyPenTransform (GpPen *pen, GpMatrix *matrix, GpMatrixOrder order)
{
	GpStatus status;
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	status = GdipMultiplyMatrix (pen->matrix, matrix, order);

	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipTranslatePenTransform (GpPen *pen, float dx, float dy, GpMatrixOrder order)
{
	GpStatus status;
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	status = GdipTranslateMatrix (pen->matrix, dx, dy, order);

	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipScalePenTransform (GpPen *pen, float sx, float sy, GpMatrixOrder order)
{
	GpStatus status;
	g_return_val_if_fail (pen != NULL, InvalidParameter);

        status = GdipScaleMatrix (pen->matrix, sx, sy, order);

	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipRotatePenTransform (GpPen *pen, float angle, GpMatrixOrder order)
{
	GpStatus status;
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	status = GdipRotateMatrix (pen->matrix, angle, order);

	if (status == Ok)
		pen->changed = TRUE;

	return status;
}

GpStatus
GdipGetPenDashStyle (GpPen *pen, GpDashStyle *dashStyle)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (dashStyle != NULL, InvalidParameter);

        *dashStyle = pen->dash_style;
        return Ok;
}

static float Custom [] = { 1.0 };
static float Dot []  = { 1.0, 1.0 };
static float Dash []  = { 3.0, 1.0 };
static float DashDot [] = { 3.0, 1.0, 1.0, 1.0 };
static float DashDotDot [] = { 3.0, 1.0, 1.0, 1.0, 1.0, 1.0 };

GpStatus
GdipSetPenDashStyle (GpPen *pen, GpDashStyle dashStyle)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

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
		pen->dash_array = Custom;
		pen->dash_count = 1;
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
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (offset != NULL, InvalidParameter);

        *offset = pen->dash_offset;
        return Ok;
}

GpStatus
GdipSetPenDashOffset (GpPen *pen, float offset)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	pen->dash_offset = offset;
	pen->changed = TRUE;
        return Ok;
}

GpStatus
GdipGetPenDashCount (GpPen *pen, int *count)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (count != NULL, InvalidParameter);

        *count = pen->dash_count;
        return Ok;
}

/*
 * This is the DashPattern property in Pen.
 */
GpStatus
GdipGetPenDashArray (GpPen *pen, float *dash, int count)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (dash != NULL, InvalidParameter);
	g_return_val_if_fail (count == pen->dash_count, InvalidParameter);

	memcpy (dash, pen->dash_array, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetPenDashArray (GpPen *pen, GDIPCONST float *dash, int count)
{
	float *dash_array;

	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (dash != NULL, InvalidParameter);
	g_return_val_if_fail (count > 0, InvalidParameter);

	if (pen->dash_count != count || pen->own_dash_array == FALSE) {
		dash_array = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (dash_array != NULL, OutOfMemory);

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
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (count != NULL, InvalidParameter);

	*count = pen->compound_count;
	return Ok;
}

GpStatus
GdipSetPenCompoundArray (GpPen *pen, const float *compound, int count)
{
	float *compound_array;

	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (compound != NULL, InvalidParameter);
	g_return_val_if_fail (count > 0, InvalidParameter);

	if (pen->compound_count != count) {
		compound_array = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (compound_array != NULL, OutOfMemory);

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
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (compound != NULL, InvalidParameter);
	g_return_val_if_fail (count == pen->compound_count, InvalidParameter);

	memcpy (compound, pen->compound_array, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetPenStartCap (GpPen *pen, GpLineCap startCap)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);

	pen->line_cap = startCap;
	pen->changed = TRUE;
	return Ok;
}

GpStatus
GdipGetPenStartCap (GpPen *pen, GpLineCap *startCap)
{
	g_return_val_if_fail (pen != NULL, InvalidParameter);
	g_return_val_if_fail (startCap != NULL, InvalidParameter);

	*startCap = pen->line_cap;

	return Ok;
}

GpStatus
GdipSetPenEndCap (GpPen *pen, GpLineCap endCap)
{
	return NotImplemented;
}

GpStatus
GdipGetPenEndCap (GpPen *pen, GpLineCap *endCap)
{
	return NotImplemented;
}

GpStatus
GdipSetPenDashCap197819 (GpPen *pen, GpDashCap dashCap)
{
	return NotImplemented;
}

GpStatus
GdipGetPenDashCap197819 (GpPen *pen, GpDashCap *dashCap)
{
	return NotImplemented;
}

GpStatus
GdipSetPenCustomStartCap (GpPen *pen, GpCustomLineCap *customCap)
{
	return NotImplemented;
}

GpStatus
GdipGetPenCustomStartCap (GpPen *pen, GpCustomLineCap **customCap)
{
	return NotImplemented;
}

GpStatus
GdipSetPenCustomEndCap (GpPen *pen, GpCustomLineCap *customCap)
{
	return NotImplemented;
}

GpStatus
GdipGetPenCustomEndCap (GpPen *pen, GpCustomLineCap **customCap)
{
	return NotImplemented;
}
