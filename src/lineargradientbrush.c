/*
 * lineargradientbrush.c
 *
 * Copyright (C) 2003-2004,2007 Novell, Inc. http://www.novell.com
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
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 */

#include "brush-private.h"
#include "lineargradientbrush-private.h"
#include "graphics-private.h"
#include "matrix-private.h"

static GpStatus gdip_linear_gradient_setup (GpGraphics *graphics, GpBrush *brush);
static GpStatus gdip_linear_gradient_clone_brush (GpBrush *brush, GpBrush **clonedBrush);
static GpStatus gdip_linear_gradient_destroy (GpBrush *brush);

/*
 * we have a single copy of vtable for
 * all instances of lineargradient brush.
 */

static BrushClass vtable = { BrushTypeLinearGradient,
			     gdip_linear_gradient_setup,
			     gdip_linear_gradient_clone_brush,
			     gdip_linear_gradient_destroy };

static GpStatus
gdip_linear_gradient_init (GpLineGradient *linear)
{
	gdip_brush_init (&linear->base, &vtable);
	linear->wrapMode = WrapModeTile;
	cairo_matrix_init_identity (&linear->matrix);
	linear->rectangle.X = linear->rectangle.Y = linear->rectangle.Width = linear->rectangle.Height = 0.0f;
	linear->gammaCorrection = FALSE;
	linear->angle = 0.0;
	linear->isAngleScalable = FALSE;
	linear->presetColors = (InterpolationColors *) GdipAlloc (sizeof (InterpolationColors));
	if (!linear->presetColors) {
		return OutOfMemory;
	}

	linear->presetColors->count = 0;
	linear->blend = (Blend *) GdipAlloc (sizeof (Blend));
	if (!linear->blend) {
		GdipFree (linear->presetColors);
		return OutOfMemory;
	}

	linear->blend->count = 1;
	linear->blend->factors = (float *) GdipAlloc (sizeof (float));
	if (!linear->blend->factors) {
		GdipFree (linear->presetColors);
		GdipFree (linear->blend);
		return OutOfMemory;
	}

	linear->blend->positions = (float *) GdipAlloc (sizeof (float));
	if (!linear->blend->positions) {
		GdipFree (linear->presetColors);
		GdipFree (linear->blend->factors);
		GdipFree (linear->blend);
		return OutOfMemory;
	}

	linear->blend->factors [0] = 1.0;
	linear->blend->positions[0] = 0.0;
	linear->pattern = NULL;

	return Ok;
}

static GpLineGradient*
gdip_linear_gradient_new (void)
{
	GpLineGradient *result = (GpLineGradient *) GdipAlloc (sizeof (GpLineGradient));

	if (result) {
		if (gdip_linear_gradient_init (result) == Ok)
			return result;

		GdipFree (result);
	}

	return NULL;
}

GpStatus
gdip_linear_gradient_clone_brush (GpBrush *brush, GpBrush **clonedBrush)
{
	GpLineGradient *linear;
	GpLineGradient *newbrush;

	if (!brush || !clonedBrush)
		return InvalidParameter;

	newbrush = gdip_linear_gradient_new ();
	if (!newbrush)
		return OutOfMemory;

	linear = (GpLineGradient *) brush;

	newbrush->base = linear->base;
	newbrush->wrapMode = linear->wrapMode;
	gdip_cairo_matrix_copy (&newbrush->matrix, &linear->matrix);
	memcpy (&newbrush->rectangle, &linear->rectangle, sizeof (GpRectF));
	newbrush->gammaCorrection = linear->gammaCorrection;
	newbrush->angle = linear->angle;
	newbrush->isAngleScalable = linear->isAngleScalable;
	/* cloned brush needs to have its own pattern */
	newbrush->base.changed = TRUE;
	newbrush->pattern = NULL;
	newbrush->lineColors [0] = linear->lineColors [0];
	newbrush->lineColors [1] = linear->lineColors [1];
	newbrush->points [0].X = linear->points [0].X;
	newbrush->points [0].Y = linear->points [0].Y;
	newbrush->points [1].X = linear->points [1].X;
	newbrush->points [1].Y = linear->points [1].Y;

	newbrush->presetColors = (InterpolationColors *) GdipAlloc (sizeof (InterpolationColors));

	if (newbrush->presetColors == NULL)
		goto failure;

	newbrush->presetColors->count = linear->presetColors->count;
	if (linear->presetColors->count > 0) {
		newbrush->presetColors->colors = (ARGB *) GdipAlloc (linear->presetColors->count * sizeof (ARGB));
		if (newbrush->presetColors->colors == NULL) 
			goto failure;

		memcpy (newbrush->presetColors->colors, linear->presetColors->colors, 
			linear->presetColors->count * sizeof (ARGB));

		newbrush->presetColors->positions = (float *) GdipAlloc (linear->presetColors->count * sizeof (float));
		if (!newbrush->presetColors->positions)
			goto failure;

		memcpy (newbrush->presetColors->positions, linear->presetColors->positions, 
			linear->presetColors->count * sizeof (float));
	} else {
		memcpy (newbrush->presetColors, linear->presetColors, sizeof (InterpolationColors));
	}
	
	newbrush->blend = (Blend *) GdipAlloc (sizeof (Blend));
	if (!newbrush->blend)
		goto failure;

	newbrush->blend->count = linear->blend->count;
	if (linear->blend->count > 0) {
		newbrush->blend->factors = (float *) GdipAlloc (linear->blend->count * sizeof (float));
		if (!newbrush->blend->factors)
			goto failure;

		memcpy (newbrush->blend->factors, linear->blend->factors, linear->blend->count * sizeof (ARGB));

		newbrush->blend->positions = (float *) GdipAlloc (linear->blend->count * sizeof (float));
		if (!newbrush->blend->positions)
			goto failure;

		memcpy (newbrush->blend->positions, linear->blend->positions, linear->blend->count * sizeof (float));
	} else {
		memcpy (newbrush->blend, linear->blend, sizeof (Blend));
	}

	*clonedBrush = (GpBrush *) newbrush;
	return Ok;

failure:
	GdipDeleteBrush ((GpBrush *) newbrush);
	*clonedBrush = NULL;
	return OutOfMemory;
}

GpStatus
gdip_linear_gradient_destroy (GpBrush *brush)
{
	/* a. the NULL check for brush is done by the caller, GdipDeleteBrush */
	/* b. brush itself is freed by the caller */

	GpLineGradient *linear = (GpLineGradient *) brush;

	if (linear->blend) {
		if (linear->blend->count > 0) {
			GdipFree (linear->blend->factors);
			GdipFree (linear->blend->positions);
		}
		GdipFree (linear->blend);
		linear->blend = NULL;
	}

	if (linear->presetColors) {
		if (linear->presetColors->count > 0) {
			GdipFree (linear->presetColors->colors);
			GdipFree (linear->presetColors->positions);
		}
		GdipFree (linear->presetColors);
		linear->presetColors = NULL;
	}

	if (linear->pattern) {
		cairo_pattern_destroy (linear->pattern);
		linear->pattern = NULL;
	}

	return Ok;
}

static void
add_color_stops_from_blend (cairo_pattern_t *pattern, Blend *blend, ARGB *colors)
{
	int index;
	double sr, sg, sb, sa;
	double er, eg, eb, ea;
	double offset, factor;

	sa = (colors[0] >> 24) & 0xFF;
	sr = (colors[0] >> 16) & 0xFF;
	sg = (colors[0] >> 8) & 0xFF;
	sb = colors[0] & 0xFF;

	ea = (colors[1] >> 24) & 0xFF;
	er = (colors[1] >> 16) & 0xFF;
	eg = (colors[1] >> 8) & 0xFF;
	eb = colors[1] & 0xFF;

	for (index = 0; index < blend->count; index++) {
		factor = blend->factors [index];
		offset = blend->positions [index];

		cairo_pattern_add_color_stop_rgba (pattern, offset, 
					      ((sr * (1 - factor)) + (er * factor)) / 255,
					      ((sg * (1 - factor)) + (eg * factor)) / 255,
					      ((sb * (1 - factor)) + (eb * factor)) / 255,
					      ((sa * (1 - factor)) + (ea * factor)) / 255);
	}
}

static void
add_color_stops_from_interpolation_colors (cairo_pattern_t *pattern, InterpolationColors *presetColors)
{
	int index;
	double r, g, b, a;
	double offset;
	ARGB color;

	/* MS accecpts positions above 1.0 also. Cairo assumes the values above 1.0 as 1.0
	 * and values below 0 are assumed as 0. So we get different results if any of the
	 * offset values is out of [0.0, 1.0].
	 */
	for (index = 0; index < presetColors->count; index++) {
		color = presetColors->colors [index];
		a = (color >> 24) & 0xFF;
		r = (color >> 16) & 0xFF;
		g = (color >> 8) & 0xFF;
		b = color & 0xFF;
		offset = presetColors->positions [index];

		cairo_pattern_add_color_stop_rgba (pattern, offset, r / 255, g / 255, b / 255, a / 255);
	}
}

static void
add_color_stops (cairo_pattern_t *pattern, ARGB *colors)
{
	double r, g, b, a;

	a = (colors[0] >> 24) & 0xFF;
	r = (colors[0] >> 16) & 0xFF;
	g = (colors[0] >> 8) & 0xFF;
	b = colors[0] & 0xFF;

	/* set start color */
	cairo_pattern_add_color_stop_rgba (pattern, 0.0, r / 255, g / 255, b / 255, a / 255);

	a = (colors[1] >> 24) & 0xFF;
	r = (colors[1] >> 16) & 0xFF;
	g = (colors[1] >> 8) & 0xFF;
	b = colors[1] & 0xFF;

	/* set end color */
	cairo_pattern_add_color_stop_rgba (pattern, 1.0, r / 255, g / 255, b / 255, a / 255);
}

static GpStatus
create_tile_linear (GpGraphics *graphics, cairo_t *ct, GpLineGradient *linear)
{
	GpStatus status;
	cairo_pattern_t *pat;
	cairo_matrix_t matrix;

	if (!graphics || !ct || !linear)
		return InvalidParameter;

	gdip_cairo_matrix_copy (&matrix, &linear->matrix);
	status = GdipInvertMatrix (&matrix);
	if (status != Ok)
		return status;

	pat = cairo_pattern_create_linear (linear->points [0].X, linear->points [0].Y, linear->points [1].X, linear->points [1].Y);
	status = gdip_get_pattern_status (pat);
	if (status != Ok)
		return status;

	cairo_pattern_set_matrix (pat, &matrix);

	if (linear->blend->count > 1)
		add_color_stops_from_blend (pat, linear->blend, linear->lineColors);
	else if (linear->presetColors->count > 1)
		add_color_stops_from_interpolation_colors (pat, linear->presetColors);
	else
		add_color_stops (pat, linear->lineColors);

	linear->pattern = pat;

	return Ok;
}

GpStatus
gdip_linear_gradient_setup (GpGraphics *graphics, GpBrush *brush)
{
	cairo_t *ct;
	GpLineGradient *linear;
	GpStatus status = Ok;

	if (!graphics || !brush)
		return InvalidParameter;

	ct = graphics->ct;
	if (!ct)
		return InvalidParameter;

	linear = (GpLineGradient *) brush;

	/* We create the new pattern for brush, if the brush is changed
	 * or if pattern has not been created yet.
	 */
	if (linear->base.changed || !linear->pattern) {

		/* destroy the existing pattern */
		if (linear->pattern) {
			cairo_pattern_destroy (linear->pattern);
			linear->pattern = NULL;
		}

		if (linear->wrapMode == WrapModeClamp)
			return InvalidParameter;
			
		status = create_tile_linear (graphics, ct, linear);

		if ((status == Ok) && linear->pattern) {
			
			switch (linear->wrapMode) {

			case WrapModeTile:
			case WrapModeTileFlipY:
				cairo_pattern_set_extend (linear->pattern, CAIRO_EXTEND_REPEAT);
				break;

			case WrapModeTileFlipX:
			case WrapModeTileFlipXY:
				cairo_pattern_set_extend (linear->pattern, CAIRO_EXTEND_REFLECT);
				break;
			default :
				return InvalidParameter; // we will never get here but I hate warnings!
			}
			
			cairo_set_source (ct, linear->pattern);
			return gdip_get_status (cairo_status (ct));
		}
	}

	if ((status == Ok) && linear->pattern) {
		cairo_set_source (ct, linear->pattern);
		return gdip_get_status (cairo_status (ct));
	}

	return status;
}

static void
gdip_linear_gradient_setup_initial_matrix (GpLineGradient *linear)
{
	float cosAngle, sinAngle, absCosAngle, absSinAngle;
	float transX, transY, wRatio, hRatio, slope, rectRight, rectBottom;
	GpPointF pts[3];
	GpRectF *rectf = &linear->rectangle;

	cosAngle = cos (linear->angle);
	sinAngle = sin (linear->angle);
	absCosAngle = fabs (cosAngle);
	absSinAngle = fabs (sinAngle);

	cairo_matrix_init_identity (&linear->matrix);

	transX = rectf->X + (rectf->Width / 2.0f);
	transY = rectf->Y + (rectf->Height / 2.0f);

	wRatio = (absCosAngle * rectf->Width + absSinAngle * rectf->Height) / rectf->Width;
	hRatio = (absSinAngle * rectf->Width + absCosAngle * rectf->Height) / rectf->Height;

	cairo_matrix_translate (&linear->matrix, transX, transY);
	cairo_matrix_rotate (&linear->matrix, linear->angle);
	cairo_matrix_scale (&linear->matrix, wRatio, hRatio);
	cairo_matrix_translate (&linear->matrix, -transX, -transY);

	if (linear->isAngleScalable && !gdip_near_zero (cosAngle) && !gdip_near_zero (sinAngle)) {
		rectRight = rectf->X + rectf->Width;
		rectBottom = rectf->Y + rectf->Height;
		
		pts[0].X = rectf->X;
		pts[0].Y = rectf->Y;
		pts[1].X = rectRight;
		pts[1].Y = rectf->Y;
		pts[2].X = rectf->X;
		pts[2].Y = rectBottom;

		GdipTransformMatrixPoints (&linear->matrix, pts, 3);

		if (sinAngle > 0 && cosAngle > 0) {
			slope = -1.0f / ((rectf->Width / rectf->Height) * tan (linear->angle));
			pts[0].Y = (slope * (pts[0].X - rectf->X)) + rectf->Y;
			pts[1].X = ((pts[1].Y - rectBottom) / slope) + rectRight;
			pts[2].X = ((pts[2].Y - rectf->Y) / slope) + rectf->X;
		} else if (sinAngle > 0 && cosAngle < 0) {
			slope = -1.0f / ((rectf->Width / rectf->Height) * tan (linear->angle - PI / 2));
			pts[0].X = ((pts[0].Y - rectBottom) / slope) + rectRight;
			pts[1].Y = (slope * (pts[1].X - rectRight)) + rectBottom;
			pts[2].Y = (slope * (pts[2].X - rectf->X)) + rectf->Y;
		} else if (sinAngle < 0 && cosAngle < 0) {
			slope = -1.0f / (((rectf->Width / rectf->Height) * tan (linear->angle)));
			pts[0].Y = (slope * (pts[0].X - rectRight)) + rectBottom;
			pts[1].X = ((pts[1].Y - rectf->Y) / slope) + rectf->X;
			pts[2].X = ((pts[2].Y - rectBottom) / slope) + rectRight;
		} else {
			slope = -1.0f / ((rectf->Width / rectf->Height) * tan (linear->angle - 3 * PI / 2));
			pts[0].X = ((pts[0].Y - rectf->Y) / slope) + rectf->X;
			pts[1].Y = (slope * (pts[1].X - rectf->X)) + rectf->Y;
			pts[2].Y = (slope * (pts[2].X - rectRight)) + rectBottom;
		}

		gdip_matrix_init_from_rect_3points (&linear->matrix, rectf, (GpPointF*)&pts);
	}
}

// coverity[+alloc : arg-*5]
GpStatus WINGDIPAPI
GdipCreateLineBrushI (GDIPCONST GpPoint *point1, GDIPCONST GpPoint *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpPointF p1, p2;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!point1 || !point2 || !lineGradient || wrapMode == WrapModeClamp)	
		return InvalidParameter;

	p1.X = point1->X;
	p1.Y = point1->Y;
	p2.X = point2->X;
	p2.Y = point2->Y;

	return GdipCreateLineBrush (&p1, &p2, color1, color2, wrapMode, lineGradient);
}

// coverity[+alloc : arg-*5]
GpStatus WINGDIPAPI
GdipCreateLineBrush (GDIPCONST GpPointF *point1, GDIPCONST GpPointF *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpLineGradient *linear;
	BOOL xFlipped = FALSE;
	BOOL yFlipped = FALSE;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!point1 || !point2 || !lineGradient || wrapMode == WrapModeClamp)
		return InvalidParameter;

	// For GDI+ compat. A zero-length gradient isn't well-defined.
	if (point1->X == point2->X && point1->Y == point2->Y)
		return OutOfMemory;

	linear = gdip_linear_gradient_new ();
	if (!linear)
		return OutOfMemory;

	linear->wrapMode = wrapMode;
	linear->lineColors [0] = color1;
	linear->lineColors [1] = color2;
	linear->isAngleScalable = FALSE;

	linear->rectangle.Width = point2->X - point1->X;
	linear->rectangle.Height = point2->Y - point1->Y;
	linear->rectangle.X = linear->rectangle.Width < 0 ? point2->X : point1->X;
	linear->rectangle.Y = linear->rectangle.Height < 0 ? point2->Y : point1->Y;

	if (linear->rectangle.Width < 0) {
		linear->rectangle.Width = -linear->rectangle.Width;
		xFlipped = TRUE;
	}

	if (linear->rectangle.Height < 0) {
		linear->rectangle.Height = -linear->rectangle.Height;
		yFlipped = TRUE;
	}

	if (linear->rectangle.Height == 0) {
		linear->rectangle.Height = linear->rectangle.Width;
		linear->rectangle.Y = linear->rectangle.Y - (linear->rectangle.Height / 2.0f);
		linear->angle = xFlipped ? 180 : 0;
	}

	else if (linear->rectangle.Width == 0) {
		linear->rectangle.Width = linear->rectangle.Height;
		linear->rectangle.X = linear->rectangle.X - (linear->rectangle.Width / 2.0f);
		linear->angle = yFlipped ? 270 : 90;
	}

	else {
		float slope = linear->rectangle.Height / linear->rectangle.Width;
		float newAngleRad = atan (slope);
		float newAngle = (newAngleRad / (DEGTORAD));

		if (xFlipped)
			newAngle = 180 - newAngle;

		if (yFlipped)
			newAngle = 360 - newAngle;

		linear->angle = newAngle;
	}

	linear->points [0].X = linear->rectangle.X;
	linear->points [0].Y = linear->rectangle.Y;
	linear->points [1].X = linear->rectangle.X + linear->rectangle.Width;
	linear->points [1].Y = linear->rectangle.Y;

	linear->angle = linear->angle * DEGTORAD;

	gdip_linear_gradient_setup_initial_matrix (linear);

	*lineGradient = linear;
	return Ok;
}

static float
get_angle_from_linear_gradient_mode (LinearGradientMode mode)
{
	switch (mode) {
	case LinearGradientModeVertical:
		return 90.0;
	case LinearGradientModeForwardDiagonal:
		return 45.0;
	case LinearGradientModeBackwardDiagonal:
		return 135.0;
	case LinearGradientModeHorizontal:
	default:
		return 0;
	}
}

// coverity[+alloc : arg-*5]
GpStatus WINGDIPAPI
GdipCreateLineBrushFromRectI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpRectF rectf;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!rect || !lineGradient)
		return InvalidParameter;

	if (mode < LinearGradientModeHorizontal || mode > LinearGradientModeBackwardDiagonal) {
		*lineGradient = NULL;
		return OutOfMemory;
	}

	gdip_RectF_from_Rect (rect, &rectf);
	return GdipCreateLineBrushFromRect (&rectf, color1, color2, mode, wrapMode, lineGradient);
}

// coverity[+alloc : arg-*5]
GpStatus WINGDIPAPI
GdipCreateLineBrushFromRect (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!rect || !lineGradient)
		return InvalidParameter;

	if (mode < LinearGradientModeHorizontal || mode > LinearGradientModeBackwardDiagonal) {
		*lineGradient = NULL;
		return OutOfMemory;
	}

	return GdipCreateLineBrushFromRectWithAngle (rect, color1, color2,
		get_angle_from_linear_gradient_mode (mode), TRUE, wrapMode, lineGradient);
}

// coverity[+alloc : arg-*6]
GpStatus WINGDIPAPI
GdipCreateLineBrushFromRectWithAngleI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, REAL angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpRectF rectf;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!rect || !lineGradient)
		return InvalidParameter;

	gdip_RectF_from_Rect (rect, &rectf);
	return GdipCreateLineBrushFromRectWithAngle (&rectf, color1, color2, angle, 
						     isAngleScalable, wrapMode, lineGradient);
}

// coverity[+alloc : arg-*6]
GpStatus
GdipCreateLineBrushFromRectWithAngle (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, REAL angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpLineGradient *linear;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!rect || !lineGradient || wrapMode == WrapModeClamp)
		return InvalidParameter;

	if (rect->Width == 0.0 || rect->Height == 0.0) {
		*lineGradient = NULL;
		return OutOfMemory;
	}

	linear = gdip_linear_gradient_new ();
	if (!linear)
		return OutOfMemory;

	linear->wrapMode = wrapMode;
	linear->lineColors [0] = color1;
	linear->lineColors [1] = color2;
	linear->angle = fmod (angle, 360) * DEGTORAD;
	linear->isAngleScalable = isAngleScalable;

	linear->points [0].X = rect->X;
	linear->points [0].Y = rect->Y;
	linear->points [1].X = rect->X + rect->Width + 1;
	linear->points [1].Y = rect->Y;
	memcpy (&linear->rectangle, rect, sizeof (GpRectF));

	gdip_linear_gradient_setup_initial_matrix (linear);

	*lineGradient = linear;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineBlendCount (GpLineGradient *brush, INT *count)
{
	if (!brush || !count)
		return InvalidParameter;

	*count = brush->blend->count;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLineBlend (GpLineGradient *brush, GDIPCONST REAL *blend, GDIPCONST REAL *positions, INT count)
{
	float *blendFactors;
	float *blendPositions;

	if (!brush || !blend || !positions || count <= 0)
		return InvalidParameter;

	if (count >= 2 && (positions[0] != 0.0f || positions[count - 1] != 1.0f))
		return InvalidParameter;

	if (brush->blend->count != count) {
		blendFactors = (float *) GdipAlloc (count * sizeof (float));
		if (!blendFactors)
			return OutOfMemory;

		blendPositions = (float *) GdipAlloc (count * sizeof (float));
		if (!blendPositions) {
			GdipFree (blendFactors);
			return OutOfMemory;
		}

		/* free the existing values */
		if (brush->blend->count != 0) {
			GdipFree (brush->blend->factors);
			GdipFree (brush->blend->positions);
		}

		brush->blend->factors = blendFactors;
		brush->blend->positions = blendPositions;
	}

	for (int index = 0; index < count; index++) {
		brush->blend->factors [index] = blend [index];
		brush->blend->positions [index] = positions [index];
	}

	brush->blend->count = count;

	/* we clear the preset colors when setting the blend */
	if (brush->presetColors->count != 0) {
		GdipFree (brush->presetColors->colors);
		GdipFree (brush->presetColors->positions);
		brush->presetColors->count = 0;
	}

	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineBlend (GpLineGradient *brush, REAL *blend, REAL *positions, INT count)
{
	if (!brush || !blend || !positions || count <= 0)
		return InvalidParameter;
		
	if (count < brush->blend->count)
		return InsufficientBuffer;

	/* If count is less than 1, we are not in a proper state 
	 * to return blend property. By default, we have one blend
	 * set. Therefore, count of 1 is acceptible.
	 */
	if (brush->blend->count < 1)
		return WrongState;
	
	memcpy (blend, brush->blend->factors, brush->blend->count * sizeof (float));

	// Don't copy anything to positions if the count is one, as positions requires at least 2 values in the array.
	if (brush->blend->count > 1)
		memcpy (positions, brush->blend->positions, brush->blend->count * sizeof (float));

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLineGammaCorrection (GpLineGradient *brush, BOOL useGammaCorrection)
{
	if (!brush)
		return InvalidParameter;

	brush->gammaCorrection = useGammaCorrection;
	brush->base.changed = TRUE;

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineGammaCorrection (GpLineGradient *brush, BOOL *useGammaCorrection)
{
	if (!brush || !useGammaCorrection)
		return InvalidParameter;

	*useGammaCorrection = brush->gammaCorrection;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLinePresetBlendCount (GpLineGradient *brush, INT *count)
{
	if (!brush || !count)
		return InvalidParameter;

	*count = brush->presetColors->count;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLinePresetBlend (GpLineGradient *brush, GDIPCONST ARGB *blend, GDIPCONST REAL *positions, INT count)
{
	ARGB *blendColors;
	float *blendPositions;

	if (!brush || !blend || !positions || count < 2 || positions[0] != 0.0f || positions[count - 1] != 1.0f)
		return InvalidParameter;

	if (brush->presetColors->count != count) {
		blendColors = (ARGB *) GdipAlloc (count * sizeof (ARGB));
		if (!blendColors)
			return OutOfMemory;

		blendPositions = (float *) GdipAlloc (count * sizeof (float));
		if (!blendPositions) {
			GdipFree (blendColors);
			return OutOfMemory;
		}

		/* free the existing values */
		if (brush->presetColors->count != 0) {
			GdipFree (brush->presetColors->colors);
			GdipFree (brush->presetColors->positions);
		}

		brush->presetColors->colors = blendColors;
		brush->presetColors->positions = blendPositions;
	}

	for (int index = 0; index < count; index++) {
		brush->presetColors->colors [index] = blend [index];
		brush->presetColors->positions [index] = positions [index];
	}

	brush->presetColors->count = count;

	/* we clear the blend when setting preset colors */
	if (brush->blend->count != 0) {
		GdipFree (brush->blend->factors);
		GdipFree (brush->blend->positions);
		brush->blend->count = 0;
	}

	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLinePresetBlend (GpLineGradient *brush, ARGB *blend, REAL *positions, INT count)
{
	if (!brush || !blend || !positions || count < 2)
		return InvalidParameter;

	if (brush->presetColors->count == 0)
		return GenericError;

	/* If count is less than 2, we are not in a proper state 
	 * to return presetblend property.
	 */
	if (brush->presetColors->count < 2)
		return WrongState;
	
	memcpy (blend, brush->presetColors->colors, count * sizeof (ARGB));
	memcpy (positions, brush->presetColors->positions, count * sizeof (float));

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLineColors (GpLineGradient *brush, ARGB color1, ARGB color2)
{
	if (!brush)
		return InvalidParameter;

	brush->lineColors[0] = color1;
	brush->lineColors[1] = color2;
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineColors (GpLineGradient *brush, ARGB *colors)
{
	if (!brush || !colors)
		return InvalidParameter;

	colors [0] = brush->lineColors[0];
	colors [1] = brush->lineColors[1];

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineRectI (GpLineGradient *brush, GpRect *rect)
{
	if (!brush || !rect)
		return InvalidParameter;

	gdip_Rect_from_RectF (&brush->rectangle, rect);
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineRect (GpLineGradient *brush, GpRectF *rect)
{
	if (!brush || !rect)
		return InvalidParameter;

	memcpy (rect, &brush->rectangle, sizeof (GpRectF));
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineTransform (GpLineGradient *brush, GpMatrix *matrix)
{
	if (!brush || !matrix)
		return InvalidParameter;

	gdip_cairo_matrix_copy (matrix, &brush->matrix);
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLineTransform (GpLineGradient *brush, GDIPCONST GpMatrix *matrix)
{
	BOOL invertible;

	if (!brush || !matrix)
		return InvalidParameter;

	/* the matrix MUST be invertible to be used */
	GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible)
		return InvalidParameter;

	gdip_cairo_matrix_copy (&brush->matrix, matrix);
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetLineWrapMode (GpLineGradient *brush, GpWrapMode *wrapMode)
{
	if (!brush || !wrapMode)
		return InvalidParameter;

	*wrapMode = brush->wrapMode;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLineWrapMode (GpLineGradient *brush, GpWrapMode wrapMode)
{
	if (!brush || (wrapMode == WrapModeClamp))
		return InvalidParameter;

	if (wrapMode > WrapModeClamp)
		return Ok;

	brush->wrapMode = wrapMode;
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLineLinearBlend (GpLineGradient *brush, REAL focus, REAL scale)
{
	float *blends;
	float *positions;
	int count = 3;

	if (!brush || focus < 0 || focus > 1 || scale < 0 || scale > 1)
		return InvalidParameter;

	if (focus == 0 || focus == 1) {
		count = 2;
	}

	if (brush->blend->count != count) {
		blends = (float *) GdipAlloc (count * sizeof (float));
		if (!blends)
			return OutOfMemory;

		positions = (float *) GdipAlloc (count * sizeof (float));
		if (!positions) {
			GdipFree (blends);
			return OutOfMemory;
		}

		/* free the existing values */
		if (brush->blend->count != 0) {
			GdipFree (brush->blend->factors);
			GdipFree (brush->blend->positions);
		}

		brush->blend->factors = blends;
		brush->blend->positions = positions;
	}

	/* we clear the preset colors when setting the blend */
	if (brush->presetColors->count != 0) {
		GdipFree (brush->presetColors->colors);
		GdipFree (brush->presetColors->positions);
		brush->presetColors->count = 0;
	}

	/* set the blend colors */
	if (focus == 0) {
		brush->blend->positions [0] = focus;
		brush->blend->factors [0] = scale;
		brush->blend->positions [1] = 1;
		brush->blend->factors [1] = 0;
	}

	else if (focus == 1) {
		brush->blend->positions [0] = 0;
		brush->blend->factors [0] = 0;
		brush->blend->positions [1] = focus;
		brush->blend->factors [1] = scale;
	}

	else {
		brush->blend->positions [0] = 0;
		brush->blend->factors [0] = 0;
		brush->blend->positions [1] = focus;
		brush->blend->factors [1] = scale;
		brush->blend->positions [2] = 1;
		brush->blend->factors [2] = 0;
	}

	brush->blend->count = count;
	brush->base.changed = TRUE;

	return Ok;
}

GpStatus WINGDIPAPI
GdipSetLineSigmaBlend (GpLineGradient *brush, REAL focus, REAL scale)
{
	float *blends;
	float *positions;
	float pos = 0.0;
	int count = 511; /* total no of samples */
	int index;
	float sigma;
	float mean;
	float fall_off_len = 2.0; /* curve fall off length in terms of SIGMA */
	float delta; /* distance between two samples */

	/* we get a curve not starting from 0 and not ending at 1.
	 * so we subtract the starting value and divide by the curve
	 * height to make it fit in the 0 to scale range
	 */
	float curve_bottom;
	float curve_top;
	float curve_height;

	if (!brush || focus < 0 || focus > 1 || scale < 0 || scale > 1)
		return InvalidParameter;

	if (focus == 0 || focus == 1) {
		count = 256;
	}

	if (brush->blend->count != count) {
		blends = (float *) GdipAlloc (count * sizeof (float));
		if (!blends)
			return OutOfMemory;

		positions = (float *) GdipAlloc (count * sizeof (float));
		if (!positions) {
			GdipFree (blends);
			return OutOfMemory;
		}

		/* free the existing values */
		if (brush->blend->count != 0) {
			GdipFree (brush->blend->factors);
			GdipFree (brush->blend->positions);
		}

		brush->blend->factors = blends;
		brush->blend->positions = positions;
	}

	/* we clear the preset colors when setting the blend */
	if (brush->presetColors->count != 0) {
		GdipFree (brush->presetColors->colors);
		GdipFree (brush->presetColors->positions);
		brush->presetColors->count = 0;
	}

	/* Set the blend colors. We use integral of the Normal Distribution,
	 * i.e. Cumulative Distribution Function (CFD).
	 *
	 * Normal distribution:
	 *
	 * y (x) = (1 / sqrt (2 * PI * sq (sigma))) * exp (-sq (x - mu)/ (2 * sq (sigma)))
	 *
	 * where, y = height of normal curve, 
	 *        sigma = standard deviation
	 *        mu = mean
	 * OR
	 * y (x) = peak * exp ( - z * z / 2)
	 * where, z = (x - mu) / sigma
	 *
	 * In this curve, peak would occur at mean i.e. for x = mu. This results in
	 * a peak value of peak = (1 / sqrt (2 * PI * sq (sigma))).
	 *
	 * Cumulative distribution function:
	 * Ref: http://mathworld.wolfram.com/NormalDistribution.html
	 *
	 * D (x) = (1 / 2) [1 + erf (z)]
	 * where, z = (x - mu) / (sigma * sqrt (2))
	 *
	 */
	if (focus == 0) {
		/* right part of the curve with a complete fall in fall_off_len * SIGMAs */
		sigma = 1.0 / fall_off_len;
		mean = 0.5;
		delta = 1.0 / 255.0;

		curve_bottom = 0.5 * (1.0 - gdip_erf (1.0, sigma, mean));
		curve_top = 0.5 * (1.0 - gdip_erf (focus, sigma, mean));
		curve_height = curve_top - curve_bottom;

		/* set the start */
		brush->blend->positions [0] = focus;
		brush->blend->factors [0] = scale;

		for (index = 1, pos = delta; index < 255; index++, pos += delta) {
			brush->blend->positions [index] = pos;
			brush->blend->factors [index] = (scale / curve_height) * 
				(0.5 * (1.0 - gdip_erf (pos, sigma, mean)) - curve_bottom);
		}

		/* set the end */
		brush->blend->positions [count - 1] = 1.0;
		brush->blend->factors [count - 1] = 0.0;
	}

	else if (focus == 1) {
		/* left part of the curve with a complete rise in fall_off_len * SIGMAs */
		sigma = 1.0 / fall_off_len;
		mean = 0.5;
		delta = 1.0 / 255.0;

		curve_bottom = 0.5 * (1.0 + gdip_erf (0.0, sigma, mean));
		curve_top = 0.5 * (1.0 + gdip_erf (focus, sigma, mean));
		curve_height = curve_top - curve_bottom;

		/* set the start */
		brush->blend->positions [0] = 0.0;
		brush->blend->factors [0] = 0.0;

		for (index = 1, pos = delta; index < 255; index++, pos += delta) {
			brush->blend->positions [index] = pos;
			brush->blend->factors [index] = (scale / curve_height) * 
				(0.5 * (1.0 + gdip_erf (pos, sigma, mean)) - curve_bottom);
		}

		/* set the end */
		brush->blend->positions [count - 1] = focus;
		brush->blend->factors [count - 1] = scale;
	}

	else {
		/* left part of the curve with a complete fall in fall_off_len * SIGMAs */
		sigma = focus / (2 * fall_off_len);
		mean = focus / 2.0;
		delta = focus / 255.0;

		/* set the start */
		brush->blend->positions [0] = 0.0;
		brush->blend->factors [0] = 0.0;

		curve_bottom = 0.5 * (1.0 + gdip_erf (0.0, sigma, mean));
		curve_top = 0.5 * (1.0 + gdip_erf (focus, sigma, mean));
		curve_height = curve_top - curve_bottom;

		for (index = 1, pos = delta; index < 255; index++, pos += delta) {
			brush->blend->positions [index] = pos;
			brush->blend->factors [index] = (scale / curve_height) * 
				(0.5 * (1.0 + gdip_erf (pos, sigma, mean)) - curve_bottom);
		}

		brush->blend->positions [index] = focus;
		brush->blend->factors [index] = scale;

		/* right part of the curve with a complete fall in fall_off_len * SIGMAs */
		sigma = (1.0 - focus) / (2 * fall_off_len);
		mean = (1.0 + focus) / 2.0;
		delta = (1.0 - focus) / 255.0;

		curve_bottom = 0.5 * (1.0 - gdip_erf (1.0, sigma, mean));
		curve_top = 0.5 * (1.0 - gdip_erf (focus, sigma, mean));
		curve_height = curve_top - curve_bottom;

		index ++;
		pos = focus + delta;

		for (; index < 510; index++, pos += delta) {
			brush->blend->positions [index] = pos;
			brush->blend->factors [index] = (scale / curve_height) * 
				(0.5 * (1.0 - gdip_erf (pos, sigma, mean)) - curve_bottom);
		}

		/* set the end */
		brush->blend->positions [count - 1] = 1.0;
		brush->blend->factors [count - 1] = 0.0;
	}

	brush->blend->count = count;
	brush->base.changed = TRUE;

	return Ok;
}

GpStatus WINGDIPAPI
GdipMultiplyLineTransform (GpLineGradient *brush, GpMatrix *matrix, GpMatrixOrder order)
{
	BOOL invertible;

	if (!brush)
		return InvalidParameter;

	if (!matrix)
		return Ok;

	/* the matrix MUST be invertible to be used */
	GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible)
		return InvalidParameter;

	if (order == MatrixOrderPrepend)
		cairo_matrix_multiply (&brush->matrix, matrix, &brush->matrix);
	else
		cairo_matrix_multiply (&brush->matrix, &brush->matrix, matrix);       

	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipResetLineTransform (GpLineGradient *brush)
{
	if (!brush)
		return InvalidParameter;

	cairo_matrix_init_identity (&brush->matrix);

	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipRotateLineTransform (GpLineGradient *brush, REAL angle, GpMatrixOrder order)
{
	GpStatus status;

	if (!brush)
		return InvalidParameter;

	status = GdipRotateMatrix (&brush->matrix, angle, order);
	if (status != Ok)
		return status;

	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipScaleLineTransform (GpLineGradient *brush, REAL sx, REAL sy, GpMatrixOrder order)
{
	GpStatus status;

	if (!brush)
		return InvalidParameter;

	status = GdipScaleMatrix (&brush->matrix, sx, sy, order);
	if (status != Ok)
		return status;

	brush->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipTranslateLineTransform (GpLineGradient *brush, REAL dx, REAL dy, GpMatrixOrder order)
{
	GpStatus status;

	if (!brush)
		return InvalidParameter;

	status = GdipTranslateMatrix (&brush->matrix, dx, dy, order);
	if (status != Ok)
		return status;

	brush->base.changed = TRUE;
	return Ok;
}
