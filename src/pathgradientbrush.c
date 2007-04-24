/*
 * pathgradientbrush.c
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
 * Authors:
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Ravindra (rkumar@novell.com)
 *
 */

#include "pathgradientbrush-private.h"
#include "graphics-private.h"
#include "graphics-path-private.h"
#include "matrix-private.h"

static GpStatus gdip_pgrad_setup (GpGraphics *graphics, GpBrush *brush);
static GpStatus gdip_pgrad_clone_brush (GpBrush *brush, GpBrush **clonedBrush);
static GpStatus gdip_pgrad_destroy (GpBrush *brush);

static BrushClass pathgradient_vtable = { BrushTypePathGradient,
                                          gdip_pgrad_setup,
                                          gdip_pgrad_clone_brush,
                                          gdip_pgrad_destroy };

static void
gdip_pathgradient_init (GpPathGradient *pg)
{
	gdip_brush_init (&pg->base, &pathgradient_vtable);
	pg->boundary = NULL;
	pg->boundaryColors = (ARGB *) GdipAlloc (sizeof (ARGB));
	*(pg->boundaryColors) = MAKE_ARGB_ARGB(255,255,255,255); /* default boundary color is white */
	pg->boundaryColorsCount = 1; /* one default boundary color */
	pg->focusScales.X = 0.0f;
	pg->focusScales.Y = 0.0f;
	pg->wrapMode = WrapModeClamp;
	cairo_matrix_init_identity (&pg->transform);
	pg->presetColors = (InterpolationColors *) GdipAlloc (sizeof (InterpolationColors));
	pg->presetColors->count = 1;
	pg->presetColors->colors = (ARGB *) GdipAlloc (sizeof (ARGB));
	pg->presetColors->positions = (float *) GdipAlloc (sizeof (float));
	pg->presetColors->colors [0] = MAKE_ARGB_ARGB(0,0,0,0);
	pg->presetColors->positions[0] = 0.0;
	pg->blend = (Blend *) GdipAlloc (sizeof (Blend));
	pg->blend->count = 1;
	pg->blend->factors = (float *) GdipAlloc (sizeof (float));
	pg->blend->positions = (float *) GdipAlloc (sizeof (float));
	pg->blend->factors [0] = 1.0;
	pg->blend->positions[0] = 0.0;
	pg->rectangle.X = 0.0;
	pg->rectangle.Y = 0.0;
	pg->rectangle.Width = 0.0;
	pg->rectangle.Height = 0.0;
	pg->pattern = NULL;
}

static GpPathGradient*
gdip_pathgradient_new (void)
{
	GpPathGradient *result = (GpPathGradient *) GdipAlloc (sizeof (GpPathGradient));

	if (result)
		gdip_pathgradient_init (result);

	return result;
}

GpStatus
gdip_pgrad_clone_brush (GpBrush *brush, GpBrush **clonedBrush)
{
	GpPathGradient *pgbrush;
	GpPathGradient *newbrush;

	if (!brush || !clonedBrush)
		return InvalidParameter;

	newbrush = (GpPathGradient *) GdipAlloc (sizeof (GpPathGradient));
	if (!newbrush)
		return OutOfMemory;

	pgbrush = (GpPathGradient *) brush;

	newbrush->base = pgbrush->base;
	if (pgbrush->boundary) {
		GdipClonePath (pgbrush->boundary, &newbrush->boundary);
	} else {
		newbrush->boundary = NULL;
	}

	newbrush->boundaryColors = GdipAlloc (sizeof(ARGB) * pgbrush->boundaryColorsCount);
	if (!newbrush->boundaryColors)
		goto NO_BOUNDARY_COLORS;

	memcpy (newbrush->boundaryColors, pgbrush->boundaryColors, sizeof(ARGB) * pgbrush->boundaryColorsCount);
	newbrush->boundaryColorsCount = pgbrush->boundaryColorsCount;
	newbrush->focusScales = pgbrush->focusScales;
	newbrush->center = pgbrush->center;
	newbrush->centerColor = pgbrush->centerColor;
	newbrush->wrapMode = pgbrush->wrapMode;
	gdip_cairo_matrix_copy (&newbrush->transform, &pgbrush->transform);

	newbrush->rectangle.X = pgbrush->rectangle.X;
	newbrush->rectangle.Y = pgbrush->rectangle.Y;
	newbrush->rectangle.Width = pgbrush->rectangle.Width;
	newbrush->rectangle.Height = pgbrush->rectangle.Height;

	newbrush->presetColors = (InterpolationColors *) GdipAlloc (sizeof (InterpolationColors));

	if (newbrush->presetColors == NULL) 
		goto NO_PRESET;

	newbrush->presetColors->count = pgbrush->presetColors->count;
	if (pgbrush->presetColors->count > 0) {
		newbrush->presetColors->colors = (ARGB *) GdipAlloc (pgbrush->presetColors->count * sizeof (ARGB));
		if (newbrush->presetColors->colors == NULL) 
			goto NO_PRESET_COLORS;
		memcpy (newbrush->presetColors->colors, pgbrush->presetColors->colors, 
			pgbrush->presetColors->count * sizeof (ARGB));
		newbrush->presetColors->positions = (float *) GdipAlloc (pgbrush->presetColors->count * sizeof (float));
		if (newbrush->presetColors->positions == NULL)
			goto NO_PRESET_POSITIONS;
		memcpy (newbrush->presetColors->positions, pgbrush->presetColors->positions, 
			pgbrush->presetColors->count * sizeof (float));
	} else {
		memcpy (newbrush->presetColors, pgbrush->presetColors, sizeof (InterpolationColors));
	}

	newbrush->blend = (Blend *) GdipAlloc (sizeof (Blend));
	if (newbrush->blend == NULL)
		goto NO_BLEND;

	newbrush->blend->count = pgbrush->blend->count;
	if (pgbrush->blend->count > 0) {
		newbrush->blend->factors = (float *) GdipAlloc (pgbrush->blend->count * sizeof (float));
		if (newbrush->blend->factors == NULL) 
			goto NO_BLEND_FACTORS;
		memcpy (newbrush->blend->factors, pgbrush->blend->factors, pgbrush->blend->count * sizeof (ARGB));
		newbrush->blend->positions = (float *) GdipAlloc (pgbrush->blend->count * sizeof (float));
		if (newbrush->blend->positions == NULL)
			goto NO_BLEND_POSITIONS;
		memcpy (newbrush->blend->positions, pgbrush->blend->positions, pgbrush->blend->count * sizeof (float));
	} else {
		memcpy (newbrush->blend, pgbrush->blend, sizeof (Blend));
	}
	goto SUCCESS;
    
NO_BLEND_POSITIONS:
	GdipFree (newbrush->blend->factors);
NO_BLEND_FACTORS:
	GdipFree (newbrush->blend);
NO_BLEND:
NO_PRESET_POSITIONS:
	GdipFree (newbrush->presetColors->colors);
NO_PRESET_COLORS:
	GdipFree (newbrush->presetColors);
NO_PRESET:
	GdipFree (newbrush->boundaryColors);
NO_BOUNDARY_COLORS:
	GdipFree (newbrush);
	return OutOfMemory;

SUCCESS:
	/* Let the clone to create its own pattern */
	newbrush->base.changed = TRUE;
	newbrush->pattern = NULL;

	*clonedBrush = (GpBrush *) newbrush;    

	return Ok;
}

GpStatus
gdip_pgrad_destroy (GpBrush *brush)
{
	/* a. the NULL check for brush is done by the caller, GdipDeleteBrush */
	/* b. brush itself is freed by the caller */

	GpPathGradient *pgbrush = (GpPathGradient *) brush;

	if (pgbrush->boundary) {
		GdipDeletePath (pgbrush->boundary);
		pgbrush->boundary = NULL;
	}

	if (pgbrush->boundaryColors) {
		GdipFree (pgbrush->boundaryColors);
		pgbrush->boundaryColors = NULL;
	}

	if (pgbrush->pattern) {
		cairo_pattern_destroy (pgbrush->pattern);
		pgbrush->pattern = NULL;
	}

	if (pgbrush->blend) {
		if (pgbrush->blend->count > 0) {
			GdipFree (pgbrush->blend->factors);
			GdipFree (pgbrush->blend->positions);
		}
		GdipFree (pgbrush->blend);
		pgbrush->blend = NULL;
	}
	
	if (pgbrush->presetColors) {
		if (pgbrush->presetColors->count > 0) {
			GdipFree (pgbrush->presetColors->colors);
			GdipFree (pgbrush->presetColors->positions);
		}
		GdipFree (pgbrush->presetColors);
		pgbrush->presetColors = NULL;
	}

	return Ok;
}

static void
add_color_stops_from_blend (cairo_pattern_t *pattern, Blend *blend, ARGB color1, ARGB color2)
{
	int index;
	double sr, sg, sb, sa;
	double er, eg, eb, ea;
	double offset, factor;

	sa = (color1 >> 24) & 0xFF;
	sr = (color1 >> 16) & 0xFF;
	sg = (color1 >> 8) & 0xFF;
	sb = color1 & 0xFF;

	ea = (color2 >> 24) & 0xFF;
	er = (color2 >> 16) & 0xFF;
	eg = (color2 >> 8) & 0xFF;
	eb = color2 & 0xFF;

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

GpStatus
gdip_pgrad_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpPathGradient *pgbrush;
	GpStatus status;

	if (!graphics || !brush)
		return InvalidParameter;

	pgbrush = (GpPathGradient *) brush;

	if (pgbrush->boundary == NULL)
		return Ok;              /* do nothing */

	/* We create the new pattern for brush, if the brush is changed
	 * or if pattern has not been created yet.
	 */
	if (pgbrush->base.changed || !pgbrush->pattern) {
		cairo_pattern_t *pat;
		float r = MIN (pgbrush->rectangle.Width / 2, pgbrush->rectangle.Height / 2);
		int i;

		/* destroy the existing pattern */
		if (pgbrush->pattern) {
			cairo_pattern_destroy (pgbrush->pattern);
			pgbrush->pattern = NULL;
		}

		/* FIXME: To fully implement this function we need cairo to support path gradients.
		 * Right now we have radial gradient which can be used, in some cases, to get the right effect.
		 */

		pat = cairo_pattern_create_radial (pgbrush->center.X, pgbrush->center.Y, 0.0f,
			pgbrush->center.X, pgbrush->center.Y, r);
		status = gdip_get_pattern_status (pat);
		if (status != Ok)
			return status;

		cairo_pattern_set_matrix (pat, &pgbrush->transform);

		if ((pgbrush->blend->count > 1) && (pgbrush->boundaryColorsCount > 0)) {
			/* FIXME: blending done using the a radial shape (not the path shape) */
			add_color_stops_from_blend (pat, pgbrush->blend, pgbrush->boundaryColors[0], pgbrush->centerColor);
		} else if (pgbrush->presetColors->count > 1) {
			/* FIXME: copied from lineargradiantbrush, most probably not right */
			add_color_stops_from_interpolation_colors (pat, pgbrush->presetColors);
		} else {
			cairo_pattern_add_color_stop_rgba (pat, 0.0f,
				ARGB_RED_N (pgbrush->centerColor),
				ARGB_GREEN_N (pgbrush->centerColor),
				ARGB_BLUE_N (pgbrush->centerColor),
				ARGB_ALPHA_N (pgbrush->centerColor));

			/* if a single other boundary color is present, then we can do the a real radial */
			if (pgbrush->boundaryColorsCount == 1) {
				ARGB c = pgbrush->boundaryColors[0];
				cairo_pattern_add_color_stop_rgba (pat, 1.0f,
					ARGB_RED_N (c), ARGB_GREEN_N (c), ARGB_BLUE_N (c), ARGB_ALPHA_N (c));
			} else {
				/* FIXME: otherwise we (solid-)fill with the centerColor */
			}
		}

		pgbrush->pattern = pat;
	}

	cairo_set_source (graphics->ct, pgbrush->pattern);
	return gdip_get_status (cairo_status (graphics->ct));
}

static GpPointF
gdip_get_center (GDIPCONST GpPointF *points, int count)
{
	/* Center is the mean of all the points. */
	int i;
	GpPointF center = {0.0, 0.0};

	for (i = 0; i < count; i++) {
		center.X += points[i].X;
		center.Y += points[i].Y;
	}

	center.X /= count;
	center.Y /= count;

	return center;
}

static BOOL
gdip_has_non_empty_color (GDIPCONST ARGB *colors, int count)
{
	int i;
	ARGB *c = (ARGB*)colors;

	for (i = 0; i < count; i++, c++) {
		if (*c)
			return TRUE;
	}

	return FALSE;
}

static void
gdip_rect_expand_by (GpRectF *rect, GpPointF *point)
{
	/* This method is somewhat stupid, because GpRect is x,y width,height,
	* instead of x0,y0 x1,y1.
	*/
	float x0 = rect->X;
	float y0 = rect->Y;
	float x1 = x0 + rect->Width;
	float y1 = y0 + rect->Height;

	if (point->X < x0)
		x0 = point->X;
	else if (point->X > x1)
		x1 = point->X;

	if (point->Y < y0)
		y0 = point->Y;
	else if (point->Y > y1)
		y1 = point->Y;

	rect->X = x0;
	rect->Y = y0;
	rect->Width = (x1 - x0);
	rect->Height = (y1 - y0);
}

/* coverity[+alloc : arg-*3] */
GpStatus
GdipCreatePathGradient (GDIPCONST GpPointF *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient)
{
	int i;
	GpPathGradient *gp;
	GpPath *gppath = NULL;
	GpStatus status;
	GpPointF point;

	if (!polyGradient)
		return InvalidParameter;

	/* this match MS GDI+ behaviour */
	if (!points || (count < 2))
		return OutOfMemory;

	status = GdipCreatePath (FillModeAlternate, /* ??? */ &gppath);
	if (status != Ok) {
		if (gppath)
			GdipDeletePath (gppath);
		return status;
	}

	GdipAddPathLine2 (gppath, points, count);

	gp = gdip_pathgradient_new ();
	gp->boundary = gppath;
	gp->wrapMode = wrapMode;
	gp->center = gdip_get_center (points, count);
	gp->centerColor = MAKE_ARGB_ARGB(255,0,0,0); /* black center color */
    
	/* set the bounding rectangle */
	point = g_array_index (gppath->points, GpPointF, 0);
	/* set the first point as the edge of the rectangle */
	gp->rectangle.X = point.X;
	gp->rectangle.Y = point.Y;
	for (i = 1; i < gppath->count; i++) {
		point = g_array_index (gppath->points, GpPointF, i);
		gdip_rect_expand_by (&gp->rectangle, &point);
	}

	*polyGradient = gp;

	return Ok;
}

/* coverity[+alloc : arg-*3] */
GpStatus
GdipCreatePathGradientI (GDIPCONST GpPoint *points, int count, GpWrapMode wrapMode, GpPathGradient **polyGradient)
{
	int i;
	GpStatus result;
	GpPointF *newPoints;

	if (!polyGradient)
		return InvalidParameter;

	/* this match MS GDI+ behaviour */
	if (!points || (count < 2))
		return OutOfMemory;

	newPoints = GdipAlloc (sizeof (GpPointF) * count);
	if (!newPoints)
		return OutOfMemory;

	for (i = 0; i < count; i++) {
		newPoints[i].X = points[i].X;
		newPoints[i].Y = points[i].Y;
	}
	result = GdipCreatePathGradient (newPoints, count, wrapMode, polyGradient);

	GdipFree (newPoints);
	return result;
}

/* coverity[+alloc : arg-*1] */
GpStatus
GdipCreatePathGradientFromPath (GDIPCONST GpPath *path, GpPathGradient **polyGradient)
{
	int i, count;
	GpPathGradient *gp;
	GpPointF *points;

	/* this match MS GDI+ behaviour */
	if (!path || (path->count < 2))
		return OutOfMemory;
	if (!polyGradient)
		return InvalidParameter;

	gp = gdip_pathgradient_new ();
	GdipClonePath ((GpPath*) path, &(gp->boundary));
	GdipGetPointCount ((GpPath*) path, &count);
	points = (GpPointF*) GdipAlloc (count * sizeof (GpPointF));
	GdipGetPathPoints ((GpPath*) path, points, count);
	gp->center = gdip_get_center (points, count);
	gp->centerColor = MAKE_ARGB_ARGB(255,255,255,255); /* white center color */

	/* set the bounding rectangle */
	/* set the first point as the edge of the rectangle */
	gp->rectangle.X = points [0].X;
	gp->rectangle.Y = points [0].Y;
	for (i = 1; i < count; i++) {
		gdip_rect_expand_by (&gp->rectangle, &points[i]);
	}

	*polyGradient = gp;

	GdipFree (points);
	return Ok;
}

GpStatus
GdipGetPathGradientCenterColor (GpPathGradient *brush, ARGB *colors)
{
	if (!brush || !colors)
		return InvalidParameter;

	*colors = brush->centerColor;
	return Ok;
}

GpStatus
GdipSetPathGradientCenterColor (GpPathGradient *brush, ARGB colors)
{
	if (!brush)
		return InvalidParameter;

	brush->centerColor = colors;
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipGetPathGradientSurroundColorsWithCount (GpPathGradient *brush, ARGB *colors, int *count)
{
	int i;

	if (!brush || !colors || !count)
		return InvalidParameter;

	for (i = 0; i < *count && i < brush->boundaryColorsCount; i++) {
		colors[i] = brush->boundaryColors[i];
	}

	*count = i;
	return Ok;
}

GpStatus
GdipSetPathGradientSurroundColorsWithCount (GpPathGradient *brush, GDIPCONST ARGB *colors, int *count)
{
	if (!brush || !colors || !count)
		return InvalidParameter;

	if ((*count < 1) || (*count > brush->boundary->count))
		return InvalidParameter;

	/* at least one of the colors must be non-zero (i.e. not empty) */
	if (!gdip_has_non_empty_color (colors, *count))
		return Ok;

	if (*count != brush->boundaryColorsCount) {
		GdipFree (brush->boundaryColors);
		brush->boundaryColors = (ARGB *) GdipAlloc (sizeof(ARGB) * (*count));
	}

	memcpy (brush->boundaryColors, colors, sizeof (ARGB) * (*count));
	brush->boundaryColorsCount = *count;
	return Ok;
}

GpStatus
GdipGetPathGradientCenterPoint (GpPathGradient *brush, GpPointF *point)
{
	if (!brush || !point)
		return InvalidParameter;

	point->X = brush->center.X;
	point->Y = brush->center.Y;
	return Ok;
}

GpStatus
GdipSetPathGradientCenterPoint (GpPathGradient *brush, GDIPCONST GpPointF *point)
{
	if (!brush || !point)
		return InvalidParameter;
	
	brush->center.X = point->X;
	brush->center.Y = point->Y;
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipGetPathGradientRect (GpPathGradient *brush, GpRectF *rect)
{
	if (!brush || !rect)
		return InvalidParameter;
	
	memcpy (rect, &brush->rectangle, sizeof (GpRectF));
	return Ok;
}

GpStatus
GdipGetPathGradientSurroundColorCount (GpPathGradient *brush, int *count)
{
	if (!brush || !count)
		return InvalidParameter;
	
	*count = brush->boundaryColorsCount;
	return Ok;
}

GpStatus
GdipGetPathGradientBlendCount (GpPathGradient *brush, int *count)
{
	if (!brush || !count)
		return InvalidParameter;
	
	/* If count is less than 1, we are not in a proper state 
	 * to return blend property. By default, we have one blend
	 * set. Therefore, count of 1 is acceptible.
	 */
	if (brush->blend->count < 1)
		return WrongState;
	
	*count = brush->blend->count;
	
	return Ok;
}

GpStatus
GdipGetPathGradientBlend (GpPathGradient *brush, float *blend, float *positions, int count)
{
	if (!brush || !blend || !positions || (brush->blend->count != count))
		return InvalidParameter;

	/* If count is less than 1, we are not in a proper state 
	 * to return blend property. By default, we have one blend
	 * set. Therefore, count of 1 is acceptible.
	 */
	if (brush->blend->count < 1)
		return WrongState;

	memcpy (blend, brush->blend->factors, count * sizeof (float));
	memcpy (positions, brush->blend->positions, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetPathGradientBlend (GpPathGradient *brush, GDIPCONST float *blend, GDIPCONST float *positions, int count)
{
	float *blendFactors;
	float *blendPositions;
	int index;

	if (!brush || !blend || !positions || (count < 2))
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

	for (index = 0; index < count; index++) {
		brush->blend->factors [index] = blend [index];
		brush->blend->positions [index] = positions [index];
	}

	brush->blend->count = count;

	/* we clear the preset colors when setting the blend */
	if (brush->presetColors->count != 1) {
		GdipFree (brush->presetColors->colors);
		GdipFree (brush->presetColors->positions);
		brush->presetColors->count = 1;
		brush->presetColors->colors = (ARGB *) GdipAlloc (sizeof (ARGB));
		brush->presetColors->positions = (float *) GdipAlloc (sizeof (float));
	}
	brush->presetColors->colors [0] = MAKE_ARGB_ARGB(0,0,0,0);
	brush->presetColors->positions[0] = 0.0;

	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipGetPathGradientPresetBlendCount (GpPathGradient *brush, int *count)
{
	if (!brush)
		return InvalidParameter;

	*count = brush->presetColors->count;
	return Ok;
}

GpStatus
GdipGetPathGradientPresetBlend (GpPathGradient *brush, ARGB *blend, float *positions, int count)
{
	if (!brush || !blend || !positions || (brush->presetColors->count != count) || (count < 2))
		return InvalidParameter;

	memcpy (blend, brush->presetColors->colors, count * sizeof (ARGB));
	memcpy (positions, brush->presetColors->positions, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetPathGradientPresetBlend (GpPathGradient *brush, GDIPCONST ARGB *blend, GDIPCONST float *positions, int count)
{
	ARGB *blendColors;
	float *blendPositions;
	int index;

	if (!brush || !blend || !positions || (count < 2))
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

	for (index = 0; index < count; index++) {
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

GpStatus
GdipSetPathGradientSigmaBlend (GpPathGradient *brush, float focus, float scale)
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

	if (!brush)
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
	if (brush->presetColors->count != 1) {
		GdipFree (brush->presetColors->colors);
		GdipFree (brush->presetColors->positions);
		brush->presetColors->count = 1;
		brush->presetColors->colors = (ARGB *) GdipAlloc (sizeof (ARGB));
		brush->presetColors->positions = (float *) GdipAlloc (sizeof (float));
	}
	brush->presetColors->colors [0] = MAKE_ARGB_ARGB(0,0,0,0);
	brush->presetColors->positions[0] = 0.0;

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

GpStatus
GdipSetPathGradientLinearBlend (GpPathGradient *brush, float focus, float scale)
{
	float *blends;
	float *positions;
	int count = 3;

	if (!brush)
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
	if (brush->presetColors->count != 1) {
		GdipFree (brush->presetColors->colors);
		GdipFree (brush->presetColors->positions);
		brush->presetColors->count = 1;
		brush->presetColors->colors = (ARGB *) GdipAlloc (sizeof (ARGB));
		brush->presetColors->positions = (float *) GdipAlloc (sizeof (float));
	}
	brush->presetColors->colors [0] = MAKE_ARGB_ARGB(0,0,0,0);
	brush->presetColors->positions[0] = 0.0;

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

GpStatus
GdipGetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode *wrapMode)
{
	if (!brush || !wrapMode)
		return InvalidParameter;

	*wrapMode = brush->wrapMode;
	return Ok;
}

GpStatus
GdipSetPathGradientWrapMode (GpPathGradient *brush, GpWrapMode wrapMode)
{
	if (!brush)
		return InvalidParameter;

	brush->wrapMode = wrapMode;
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipGetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix)
{
	if (!brush || !matrix)
		return InvalidParameter;

	/* If presetcolors are set, we are not in a proper state 
	 * to return transform property.
	 */
	if (brush->presetColors->count >= 2)
		return WrongState;

	gdip_cairo_matrix_copy (matrix, &brush->transform);
	return Ok;
}

GpStatus
GdipSetPathGradientTransform (GpPathGradient *brush, GpMatrix *matrix)
{
	GpStatus status;
	BOOL invertible;

	if (!brush || !matrix)
		return InvalidParameter;

	/* the matrix MUST be invertible to be used */
	status = GdipIsMatrixInvertible ((GpMatrix*) matrix, &invertible);
	if (!invertible || (status != Ok))
		return InvalidParameter;

	gdip_cairo_matrix_copy (&brush->transform, matrix);
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipResetPathGradientTransform (GpPathGradient *brush)
{
	if (!brush)
		return InvalidParameter;

	cairo_matrix_init_identity (&brush->transform);
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipMultiplyPathGradientTransform (GpPathGradient *brush, GDIPCONST GpMatrix *matrix, GpMatrixOrder order)
{
	GpStatus status;
	BOOL invertible;
	cairo_matrix_t mat;

	if (!brush || !matrix)
		return InvalidParameter;

	/* the matrix MUST be invertible to be used */
	status = GdipIsMatrixInvertible ((GpMatrix*) matrix, &invertible);
	if (!invertible || (status != Ok))
		return InvalidParameter;

	if (order == MatrixOrderPrepend)
		cairo_matrix_multiply (&mat, matrix, &brush->transform);
	else if (order == MatrixOrderAppend)
		cairo_matrix_multiply (&mat, &brush->transform, matrix);

	gdip_cairo_matrix_copy (&brush->transform, &mat);
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipTranslatePathGradientTransform (GpPathGradient *brush, float dx, float dy, GpMatrixOrder order)
{
	GpStatus status;

	if (!brush)
		return InvalidParameter;

	if ((status = GdipTranslateMatrix (&brush->transform, dx, dy, order)) == Ok)
		brush->base.changed = TRUE;
	return status;
}

GpStatus
GdipScalePathGradientTransform (GpPathGradient *brush, float sx, float sy, GpMatrixOrder order)
{
	GpStatus status;

	if (!brush)
		return InvalidParameter;

	if ((status = GdipScaleMatrix (&brush->transform, sx, sy, order)) == Ok)
		brush->base.changed = TRUE;
	return status;
}

GpStatus
GdipRotatePathGradientTransform (GpPathGradient *brush, float angle, GpMatrixOrder order)
{
	GpStatus status;

	if (!brush)
		return InvalidParameter;

	if ((status = GdipRotateMatrix (&brush->transform, angle, order)) == Ok)
		brush->base.changed = TRUE;
	return status;
}

GpStatus
GdipGetPathGradientFocusScales (GpPathGradient *brush, float *xScale, float *yScale)
{
	if (!brush || !xScale || !yScale)
		return InvalidParameter;

	*xScale = brush->focusScales.X;
	*yScale = brush->focusScales.Y;
	return Ok;
}

GpStatus
GdipSetPathGradientFocusScales (GpPathGradient *brush, float xScale, float yScale)
{
	if (!brush)
		return InvalidParameter;

	brush->focusScales.X = xScale;
	brush->focusScales.Y = yScale;
	brush->base.changed = TRUE;
	return Ok;
}
