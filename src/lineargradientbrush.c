/*
 * lineargradientbrush.c
 *
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004 Novell, Inc. http://www.novell.com
 */

#include "gdip.h"
#include "lineargradientbrush.h"

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

struct _Blend {
	float *factors;
	float *positions;
	int count;
};

struct _InterpolationColors {
	ARGB* colors;
	float *positions;
	int count;
};
 
void
gdip_linear_gradient_init (GpLineGradient *linear)
{
	gdip_brush_init (&linear->base, &vtable);
	linear->wrapMode = WrapModeTile;
	linear->matrix = cairo_matrix_create ();
	linear->rectangle = NULL;
	linear->gammaCorrection = FALSE;
	linear->angle = 0.0;
	linear->isAngleScalable = FALSE;
	linear->presetColors = (InterpolationColors *) GdipAlloc (sizeof (InterpolationColors));
	linear->presetColors->count = 0;
	linear->blend = (Blend *) GdipAlloc (sizeof (Blend));
	linear->blend->count = 1;
	linear->blend->factors = (float *) GdipAlloc (sizeof (float));
	linear->blend->positions = (float *) GdipAlloc (sizeof (float));
	linear->blend->factors [0] = 1.0;
	linear->blend->positions[0] = 0.0;
}

GpLineGradient*
gdip_linear_gradient_new (void)
{
	GpLineGradient *result = (GpLineGradient *) GdipAlloc (sizeof (GpLineGradient));

	if (result)
		gdip_linear_gradient_init (result);

	return result;
}

GpStatus
gdip_linear_gradient_clone_brush (GpBrush *brush, GpBrush **clonedBrush)
{
	GpLineGradient *linear;
	GpLineGradient *newbrush;

	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (clonedBrush != NULL, InvalidParameter);

	linear = (GpLineGradient *) brush;
	newbrush = (GpLineGradient *) GdipAlloc (sizeof (GpLineGradient));

	g_return_val_if_fail (newbrush != NULL, OutOfMemory);

	newbrush->base = linear->base;
	newbrush->wrapMode = linear->wrapMode;
	GdipCloneMatrix (linear->matrix, &newbrush->matrix);
	newbrush->rectangle = linear->rectangle;
	newbrush->gammaCorrection = linear->gammaCorrection;
	newbrush->angle = linear->angle;
	newbrush->isAngleScalable = linear->isAngleScalable;
	newbrush->lineColors [0] = linear->lineColors [0];
	newbrush->lineColors [1] = linear->lineColors [1];
	newbrush->points [0] = linear->points [0];
	newbrush->points [1] = linear->points [1];

	newbrush->presetColors = (InterpolationColors *) GdipAlloc (sizeof (InterpolationColors));

	if (newbrush->presetColors == NULL) 
		goto NO_PRESET;

	if (linear->presetColors->count != -1) {
		newbrush->presetColors->colors = (ARGB *) GdipAlloc (linear->presetColors->count * sizeof (ARGB));
		if (newbrush->presetColors->colors == NULL) 
			goto NO_PRESET_COLORS;
		memcpy (newbrush->presetColors->colors, linear->presetColors->colors, 
			linear->presetColors->count * sizeof (ARGB));
		newbrush->presetColors->positions = (float *) GdipAlloc (linear->presetColors->count * sizeof (float));
		if (newbrush->presetColors->positions == NULL)
			goto NO_PRESET_POSITIONS;
		memcpy (newbrush->presetColors->positions, linear->presetColors->positions, 
			linear->presetColors->count * sizeof (float));
	} else {
		memcpy (newbrush->presetColors, linear->presetColors, sizeof (InterpolationColors));
	}
	
	newbrush->blend = (Blend *) GdipAlloc (sizeof (Blend));
	if (newbrush->blend == NULL)
		goto NO_BLEND;
	if (linear->blend->count != -1) {
		newbrush->blend->factors = (float *) GdipAlloc (linear->blend->count * sizeof (float));
		if (newbrush->blend->factors == NULL) 
			goto NO_BLEND_FACTORS;
		memcpy (newbrush->blend->factors, linear->blend->factors, linear->blend->count * sizeof (ARGB));
		newbrush->blend->positions = (float *) GdipAlloc (linear->blend->count * sizeof (float));
		if (newbrush->blend->positions == NULL)
			goto NO_BLEND_POSITIONS;
		memcpy (newbrush->blend->positions, linear->blend->positions, linear->blend->count * sizeof (float));
	} else {
		memcpy (newbrush->blend, linear->blend, sizeof (Blend));
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
	GdipDeleteMatrix (newbrush->matrix);
	GdipFree (newbrush);
	return OutOfMemory;

 SUCCESS:
	*clonedBrush = (GpBrush *) newbrush;

	return Ok;
}

GpStatus
gdip_linear_gradient_destroy (GpBrush *brush)
{
	GpLineGradient *linear;

	g_return_val_if_fail (brush != NULL, InvalidParameter);

	linear = (GpLineGradient *) brush;

	if (linear->matrix)
		cairo_matrix_destroy (linear->matrix);
	if (linear->rectangle)
		GdipFree (linear->rectangle);
	if (linear->blend->count > 0) {
		GdipFree (linear->blend->factors);
		GdipFree (linear->blend->positions);
	}
	GdipFree (linear->blend);

	if (linear->presetColors->count > 0) {
		GdipFree (linear->presetColors->colors);
		GdipFree (linear->presetColors->positions);
	}
	GdipFree (linear->presetColors);

	GdipFree (linear);

	return Ok;
}

void
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

		cairo_pattern_add_color_stop (pattern, offset, 
					      ((sr * factor) + (er * (1 - factor))) / 255,
					      ((sg * factor) + (eg * (1 - factor))) / 255,
					      ((sb * factor) + (eb * (1 - factor))) / 255,
					      ((sa * factor) + (ea * (1 - factor))) / 255);
	}
}

void
add_color_stops_from_interpolation_colors (cairo_pattern_t *pattern, InterpolationColors *presetColors)
{
	int index;
	double r, g, b, a;
	double offset;
	ARGB color;

	for (index = 0; index < presetColors->count; index++) {
		color = presetColors->colors [index];
		a = (color >> 24) & 0xFF;
		r = (color >> 16) & 0xFF;
		g = (color >> 8) & 0xFF;
		b = color & 0xFF;
		offset = presetColors->positions [index];

		cairo_pattern_add_color_stop (pattern, offset, r / 255, g / 255, b / 255, a / 255);
	}
}

void
add_color_stops (cairo_pattern_t *pattern, ARGB *colors)
{
	double r, g, b, a;

	a = (colors[0] >> 24) & 0xFF;
	r = (colors[0] >> 16) & 0xFF;
	g = (colors[0] >> 8) & 0xFF;
	b = colors[0] & 0xFF;

	/* set start color */
	cairo_pattern_add_color_stop (pattern, 0.0, r / 255, g / 255, b / 255, a / 255);

	a = (colors[1] >> 24) & 0xFF;
	r = (colors[1] >> 16) & 0xFF;
	g = (colors[1] >> 8) & 0xFF;
	b = colors[1] & 0xFF;

	/* set end color */
	cairo_pattern_add_color_stop (pattern, 1.0, r / 255, g / 255, b / 255, a / 255);
}

GpStatus
create_tile_linear (cairo_t *ct, GpLineGradient *linear, cairo_pattern_t **pattern)
{
	cairo_pattern_t *pat;
	cairo_save (ct);
	
	pat = cairo_pattern_create_linear (linear->points [0].X, linear->points [0].Y,
					   linear->points [1].X, linear->points [1].Y);

	g_return_val_if_fail (pat != NULL, OutOfMemory);

	if (linear->blend->count > 1)
		add_color_stops_from_blend (pat, linear->blend, linear->lineColors);
	else if (linear->presetColors->count > 1)
		add_color_stops_from_interpolation_colors (pat, linear->presetColors);
	else
		add_color_stops (pat, linear->lineColors);

	cairo_set_pattern (ct, pat);
	cairo_rectangle (ct, linear->rectangle->X, linear->rectangle->Y, linear->rectangle->Width, 
			 linear->rectangle->Height);

	if (linear->angle != 0)
		cairo_rotate (ct, linear->angle);

	cairo_fill (ct);

	cairo_pattern_destroy (pat);

	*pattern = cairo_pattern_create_for_surface (cairo_current_target_surface (ct));

	cairo_restore (ct);

	return Ok;
}

GpStatus
create_tile_flipX_linear (cairo_t *ct, GpLineGradient *linear, cairo_pattern_t **pattern)
{
	return NotImplemented;
}

GpStatus
create_tile_flipY_linear (cairo_t *ct, GpLineGradient *linear, cairo_pattern_t **pattern)
{
	return NotImplemented;
}

GpStatus
create_tile_flipXY_linear (cairo_t *ct, GpLineGradient *linear, cairo_pattern_t **pattern)
{
	return NotImplemented;
}

GpStatus
gdip_linear_gradient_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpLineGradient *linear;
	cairo_pattern_t *pattern;
	cairo_t *ct;
	GpStatus status;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	ct = graphics->ct;

	g_return_val_if_fail (ct != NULL, InvalidParameter);

	linear = (GpLineGradient *) brush;

	switch (linear->wrapMode) {

	case WrapModeTile:
		status = create_tile_linear (ct, linear, &pattern);

	case WrapModeTileFlipX:
		status = create_tile_flipX_linear (ct, linear, &pattern);

	case WrapModeTileFlipY:
		status = create_tile_flipY_linear (ct, linear, &pattern);

	case WrapModeTileFlipXY:
		status = create_tile_flipXY_linear (ct, linear, &pattern);

	default:
		status = InvalidParameter;
	}

	if (status == Ok) {
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_pattern_set_matrix (pattern, linear->matrix);
		cairo_set_pattern (ct, pattern);
		cairo_pattern_destroy (pattern);
	}

	return status;
}

void
gdip_set_rect (GpRectF *rect, float x1, float y1, float x2, float y2)
{
	float x, y, width, height;

	if (rect) {
		if (x1 == x2) {
			rect->Height = (y1 > y2) ? (y1 - y2) : (y2 - y1);
			rect->Width = rect->Height;
			rect->X = x1 - (rect->Height) / 2.0;
			rect->Y = (y1 < y2) ? y1 : y2;
		} 
		else if (y1 == y2) {
			rect->Width = (x1 > x2) ? (x1 - x2) : (x2 - x1);
			rect->Height = rect->Width;
			rect->X = (x1 < x2) ? x1 : x2;
			rect->Y = y1 - (rect->Width) / 2.0;
		}
		else {
			rect->Width = (x1 > x2) ? (x1 - x2) : (x2 - x1);
			rect->Height = (y1 > y2) ? (y1 - y2) : (y2 - y1);
			rect->X = (x1 < x2) ? x1 : x2;
			rect->Y = (y1 < y2) ? y1 : y2;
		}
	}
}

GpStatus
GdipCreateLineBrushI (GDIPCONST GpPoint *point1, GDIPCONST GpPoint *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpPointF *pt1, *pt2;
	GpStatus status;

	g_return_val_if_fail (point1 != NULL, InvalidParameter);
	g_return_val_if_fail (point2 != NULL, InvalidParameter);

	pt1 = (GpPointF *) GdipAlloc (sizeof (GpPointF));
	g_return_val_if_fail (pt1 != NULL, OutOfMemory);

	pt2 = (GpPointF *) GdipAlloc (sizeof (GpPointF));

	if (pt2 == NULL) {
		GdipFree (pt1);
		return OutOfMemory;
	}

	pt1->X = point1->X; 
	pt1->Y = point1->Y;
	pt2->X = point2->X; 
	pt2->Y = point2->Y;

	status = GdipCreateLineBrush (pt1, pt2, color1, color2, wrapMode, lineGradient);

	GdipFree (pt1);
	GdipFree (pt2);

	return status;
}

GpStatus
GdipCreateLineBrush (GDIPCONST GpPointF *point1, GDIPCONST GpPointF *point2, ARGB color1, ARGB color2, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpLineGradient *linear;
	GpRectF *rectf;

	g_return_val_if_fail (point1 != NULL, InvalidParameter);
	g_return_val_if_fail (point2 != NULL, InvalidParameter);
	g_return_val_if_fail (lineGradient != NULL, InvalidParameter);

	rectf = (GpRectF *) GdipAlloc (sizeof (GpRectF));
	g_return_val_if_fail (rectf != NULL, OutOfMemory);

	linear = gdip_linear_gradient_new ();
	if (linear == NULL) {
		GdipFree (rectf);
		return OutOfMemory;
	}

	gdip_set_rect (rectf, point1->X, point1->Y, point2->X, point2->Y);

	linear->wrapMode = wrapMode;
	linear->lineColors [0] = color1;
	linear->lineColors [1] = color2;
	linear->points [0] = *point1;
	linear->points [1] = *point2;
	linear->rectangle = rectf;

	*lineGradient = linear;

	return Ok;
}

GpStatus
GdipCreateLineBrushFromRectI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpRectF *rectf;
	GpStatus status;

	g_return_val_if_fail (rect != NULL, InvalidParameter);

	rectf = (GpRectF *) GdipAlloc (sizeof (GpRectF));
	g_return_val_if_fail (rectf != NULL, OutOfMemory);

	rectf->X = rect->X;
	rectf->Y = rect->Y;
	rectf->Width = rect->Width;
	rectf->Height = rect->Height;

	status = GdipCreateLineBrushFromRect (rectf, color1, color2, mode, wrapMode, lineGradient);

	GdipFree (rectf);

	return status;
}

GpStatus
GdipCreateLineBrushFromRect (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, LinearGradientMode mode, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpLineGradient *linear;
	float angle = 0.0;

	switch (mode) {

	case LinearGradientModeHorizontal:
		angle = 0.0;
		break;

	case LinearGradientModeVertical:
		angle = 90.0;
		break;

	case LinearGradientModeForwardDiagonal:
		angle = 45.0;
		break;

	case LinearGradientModeBackwardDiagonal:
		angle = 135.0;
		break;
	}

	/* FIXME: Check whether angle has to be scalable or 
	 * not in case of lineargradient mode. 
	 */
	return GdipCreateLineBrushFromRectWithAngle (rect, color1, color2, angle, FALSE, 
						     wrapMode, lineGradient);
}

GpStatus
GdipCreateLineBrushFromRectWithAngleI (GDIPCONST GpRect *rect, ARGB color1, ARGB color2, float angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpRectF *rectf;

	g_return_val_if_fail (rect != NULL, InvalidParameter);

	rectf = (GpRectF *) GdipAlloc (sizeof (GpRectF));
	g_return_val_if_fail (rectf != NULL, OutOfMemory);

	rectf->X = rect->X;
	rectf->Y = rect->Y;
	rectf->Width = rect->Width;
	rectf->Height = rect->Height;

	return GdipCreateLineBrushFromRectWithAngle (rectf, color1, color2, angle, 
						     isAngleScalable, wrapMode, lineGradient);
}

GpStatus
GdipCreateLineBrushFromRectWithAngle (GDIPCONST GpRectF *rect, ARGB color1, ARGB color2, float angle, BOOL isAngleScalable, GpWrapMode wrapMode, GpLineGradient **lineGradient)
{
	GpLineGradient *linear;
	GpRectF *rectf;

	g_return_val_if_fail (rect != NULL, InvalidParameter);
	g_return_val_if_fail (lineGradient != NULL, InvalidParameter);

	rectf = (GpRectF *) GdipAlloc (sizeof (GpRectF));
	g_return_val_if_fail (rectf != NULL, OutOfMemory);

	linear = gdip_linear_gradient_new ();
	if (linear == NULL) {
		GdipFree (rectf);
		return OutOfMemory;
	}

	memcpy (rectf, rect, sizeof (GpRectF));

	linear->wrapMode = wrapMode;
	linear->lineColors [0] = color1;
	linear->lineColors [1] = color2;
	linear->angle = angle;
	linear->isAngleScalable = isAngleScalable;
	linear->points [0].X = rect->X;
	linear->points [0].Y = rect->Y + rect->Height / 2;
	linear->points [1].X = rect->X + rect->Width;
	linear->points [1].Y = linear->points [0].Y;
	linear->rectangle = rectf; 

	*lineGradient = linear;

	return Ok;
}

GpStatus
GdipGetLineBlendCount (GpLineGradient *brush, int *count)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

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
GdipSetLineBlend (GpLineGradient *brush, GDIPCONST float *blend, GDIPCONST float *positions, int count)
{
	float *blendFactors;
	float *blendPositions;
	int index;

	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (blend != NULL, InvalidParameter);
	g_return_val_if_fail (positions != NULL, InvalidParameter);
	g_return_val_if_fail (count >= 2, InvalidParameter);

	if (brush->blend->count != count) {
		blendFactors = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (blendFactors != NULL, OutOfMemory);

		blendPositions = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (blendPositions != NULL, OutOfMemory);

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
	if (brush->presetColors->count != 0) {
		GdipFree (brush->presetColors->colors);
		GdipFree (brush->presetColors->positions);
		brush->presetColors->count = 0;
	}

	return Ok;
}

GpStatus
GdipGetLineBlend (GpLineGradient *brush, float *blend, float *positions, int count)
{
	float *blendFactors;
	float *blendPositions;

	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (blend != NULL, InvalidParameter);
	g_return_val_if_fail (positions != NULL, InvalidParameter);
	g_return_val_if_fail (brush->blend->count == count, InvalidParameter);
	
	memcpy (blend, brush->blend->factors, count * sizeof (float));
	memcpy (positions, brush->blend->positions, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetLineGammaCorrection (GpLineGradient *brush, BOOL useGammaCorrection)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	brush->gammaCorrection = useGammaCorrection;

	return Ok;
}

GpStatus
GdipGetLineGammaCorrection (GpLineGradient *brush, BOOL *useGammaCorrection)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	*useGammaCorrection = brush->gammaCorrection;

	return Ok;
}

GpStatus
GdipGetLinePresetBlendCount (GpLineGradient *brush, int *count)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	/* If count is less than 2, we are not in a proper state 
	 * to return presetblend property.
	 */
	if (brush->presetColors->count < 2)
		return WrongState;

	*count = brush->presetColors->count;

	return Ok;
}

GpStatus
GdipSetLinePresetBlend (GpLineGradient *brush, GDIPCONST ARGB *blend, GDIPCONST float *positions, int count)
{
	ARGB *blendColors;
	float *blendPositions;
	int index;

	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (blend != NULL, InvalidParameter);
	g_return_val_if_fail (positions != NULL, InvalidParameter);
	g_return_val_if_fail (count >= 2, InvalidParameter);

	if (brush->presetColors->count != count) {
		blendColors = (ARGB *) GdipAlloc (count * sizeof (ARGB));
		g_return_val_if_fail (blendColors != NULL, OutOfMemory);

		blendPositions = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (blendPositions != NULL, OutOfMemory);

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

	return Ok;
}

GpStatus
GdipGetLinePresetBlend (GpLineGradient *brush, ARGB *blend, float *positions, int count)
{
	float *blendColors;
	float *blendPositions;

	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (blend != NULL, InvalidParameter);
	g_return_val_if_fail (positions != NULL, InvalidParameter);
	g_return_val_if_fail (brush->presetColors->count == count, InvalidParameter);
	
	memcpy (blend, brush->presetColors->colors, count * sizeof (ARGB));
	memcpy (positions, brush->presetColors->positions, count * sizeof (float));

	return Ok;
}

GpStatus
GdipSetLineColors (GpLineGradient *brush, ARGB color1, ARGB color2)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	brush->lineColors[0] = color1;
	brush->lineColors[1] = color2;

	return Ok;
}

GpStatus
GdipGetLineColors (GpLineGradient *brush, ARGB *colors)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (colors != NULL, InvalidParameter);

	colors [0] = brush->lineColors[0];
	colors [1] = brush->lineColors[1];

	return Ok;
}

GpStatus
GdipGetLineRectI (GpLineGradient *brush, GpRect *rect)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	rect->X = (int) brush->rectangle->X;
	rect->Y = (int) brush->rectangle->Y;
	rect->Width = (int) brush->rectangle->Width;
	rect->Height = (int) brush->rectangle->Height;

	return Ok;
}

GpStatus
GdipGetLineRect (GpLineGradient *brush, GpRectF *rect)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	memcpy (rect, brush->rectangle, sizeof (GpRectF));

	return Ok;
}

GpStatus
GdipGetLineTransform (GpLineGradient *brush, GpMatrix *matrix)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	/* If presetcolors are set, we are not in a proper state 
	 * to return transform property.
	 */
	if (brush->presetColors->count >= 2)
		return WrongState;

	*matrix = *(brush->matrix);

	return Ok;
}

GpStatus
GdipSetLineTransform (GpLineGradient *brush, GDIPCONST GpMatrix *matrix)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	*(brush->matrix) = *matrix;
	return Ok;
}

GpStatus
GdipGetLineWrapMode (GpLineGradient *brush, GpWrapMode *wrapMode)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (wrapMode != NULL, InvalidParameter);

	*wrapMode = brush->wrapMode;
	return Ok;
}

GpStatus
GdipSetLineWrapMode (GpLineGradient *brush, GpWrapMode wrapMode)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (wrapMode != WrapModeClamp, InvalidParameter);

	brush->wrapMode = wrapMode;
	return Ok;
}

GpStatus
GdipSetLineLinearBlend (GpLineGradient *brush, float focus, float scale)
{
	return NotImplemented;
}

GpStatus
GdipSetLineSigmaBlend (GpLineGradient *brush, float focus, float scale)
{
	return NotImplemented;
}

GpStatus
GdipMultiplyLineTransform (GpLineGradient *brush, GpMatrix *matrix, GpMatrixOrder order)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	return GdipMultiplyMatrix (brush->matrix, matrix, order);
}

GpStatus
GdipResetLineTransform (GpLineGradient *brush)
{
	cairo_status_t status;
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	status = cairo_matrix_set_identity (brush->matrix);
	return gdip_get_status (status);
}

GpStatus
GdipRotateLineTransform (GpLineGradient *brush, float angle, GpMatrixOrder order)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	return GdipRotateMatrix (brush->matrix, angle, order);
}

GpStatus
GdipScaleLineTransform (GpLineGradient *brush, float sx, float sy, GpMatrixOrder order)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	return GdipScaleMatrix (brush->matrix, sx, sy, order);
}

GpStatus
GdipTranslateLineTransform (GpLineGradient *brush, float dx, float dy, GpMatrixOrder order)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	return GdipTranslateMatrix (brush->matrix, dx, dy, order);
}
