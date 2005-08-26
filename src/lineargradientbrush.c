/*
 * lineargradientbrush.c
 *
 * Copyright (C) Novell, Inc. 2003-2004.
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

void
gdip_linear_gradient_init (GpLineGradient *linear)
{
	gdip_brush_init (&linear->base, &vtable);
	linear->wrapMode = WrapModeTile;
	GdipCreateMatrix(&linear->matrix);
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
	linear->pattern = NULL;
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
	if (linear->rectangle) {
		newbrush->rectangle = (GpRectF *) GdipAlloc (sizeof (GpRectF));
		memcpy (newbrush->rectangle, linear->rectangle, sizeof (GpRectF));
	} else {
		newbrush->rectangle= NULL;
	}
	newbrush->gammaCorrection = linear->gammaCorrection;
	newbrush->angle = linear->angle;
	newbrush->isAngleScalable = linear->isAngleScalable;
	/* cloned brush needs to have its own pattern */
	newbrush->base.changed = TRUE;
	newbrush->pattern = NULL;
	newbrush->lineColors [0] = linear->lineColors [0];
	newbrush->lineColors [1] = linear->lineColors [1];
	newbrush->points [0] = linear->points [0];
	newbrush->points [1] = linear->points [1];

	newbrush->presetColors = (InterpolationColors *) GdipAlloc (sizeof (InterpolationColors));

	if (newbrush->presetColors == NULL) 
		goto NO_PRESET;

	if (linear->presetColors->count > 0) {
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
	if (linear->blend->count > 0) {
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

	if (linear->rectangle)
		GdipFree (linear->rectangle);
	linear->rectangle = NULL;

	if (linear->blend)
		if (linear->blend->count > 0) {
			GdipFree (linear->blend->factors);
			GdipFree (linear->blend->positions);
		}
	GdipFree (linear->blend);
	linear->blend = NULL;

	if (linear->presetColors)
		if (linear->presetColors->count > 0) {
			GdipFree (linear->presetColors->colors);
			GdipFree (linear->presetColors->positions);
		}
	GdipFree (linear->presetColors);
	linear->presetColors = NULL;

	if (linear->pattern)
		cairo_pattern_destroy (linear->pattern);
	linear->pattern = NULL;

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

		cairo_pattern_add_color_stop_rgba (pattern, offset, 
					      ((sr * (1 - factor)) + (er * factor)) / 255,
					      ((sg * (1 - factor)) + (eg * factor)) / 255,
					      ((sb * (1 - factor)) + (eb * factor)) / 255,
					      ((sa * (1 - factor)) + (ea * factor)) / 255);
	}
}

void
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

void
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

GpStatus
create_tile_linear (cairo_t *ct, GpLineGradient *linear)
{
	cairo_t	*ct2;
	cairo_surface_t *gradient;
	cairo_pattern_t *pat;
	GpMatrix *currMatrix = NULL;
	GpMatrix *tempMatrix = NULL;
	GpRectF *rect = linear->rectangle;
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	/* gradient surface to be created. We are using CAIRO_CONTENT_COLOR_ALPHA, */
	/* because internally this is the format we use everywhere. */
	gradient = cairo_surface_create_similar (cairo_get_target (ct), CAIRO_CONTENT_COLOR_ALPHA,
						 (int) (2 * rect->Width), (int) (2 * rect->Height));

	if (gradient == NULL) {
		return OutOfMemory;
	}

	pat = cairo_pattern_create_linear (linear->points [0].X, linear->points [0].Y,
					   linear->points [1].X, linear->points [1].Y);

	if (pat == NULL) {
		cairo_surface_destroy (gradient);
		return OutOfMemory;
	}

	ct2 = cairo_create(gradient);

	if (linear->blend->count > 1)
		add_color_stops_from_blend (pat, linear->blend, linear->lineColors);
	else if (linear->presetColors->count > 1)
		add_color_stops_from_interpolation_colors (pat, linear->presetColors);
	else
		add_color_stops (pat, linear->lineColors);

	GdipCreateMatrix (&tempMatrix);

	/* rotate the pattern if angle is non-zero */
	if (linear->angle != 0) {
		/* Absolute rotation */
		cairo_matrix_translate (tempMatrix, rect-> Width, rect->Height);
		cairo_matrix_rotate (tempMatrix, linear->angle * DEGTORAD);
		cairo_matrix_translate (tempMatrix, -rect-> Width, -rect->Height);
	}

	/* if rotation angle is scalable, it should be multiplied by the brush matrix */
	if (linear->isAngleScalable) {
		cairo_matrix_multiply (tempMatrix, tempMatrix, linear->matrix);
	}

	/* pattern is transformed wrt to main graphics */
	cairo_matrix_multiply (linear->matrix, linear->matrix, tempMatrix);

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, 2 * rect->Width, 2 * rect->Height);
	cairo_fill (ct2);	

	cairo_pattern_destroy (pat);
	cairo_destroy (ct2);

	linear->pattern = cairo_pattern_create_for_surface (gradient);
	cairo_surface_destroy (gradient);


	return Ok;
}

GpStatus
create_tile_flipX_linear (cairo_t *ct, GpLineGradient *linear)
{
	cairo_surface_t *gradient;
	cairo_pattern_t *pat;
	GpMatrix *tempMatrix = NULL;
	GpMatrix *currMatrix = NULL;
	GpRectF *rect = linear->rectangle;
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	/* gradient surface to be created. We are using CAIRO_CONTENT_COLOR_ALPHA, */
	/* because internally this is the format we use everywhere. */
	gradient = cairo_surface_create_similar (cairo_get_target (ct), CAIRO_CONTENT_COLOR_ALPHA,
						 (int) (2 * rect->Width), (int) (2 * rect->Height));

	if (gradient == NULL) {
		
		return OutOfMemory;
	}

	pat = cairo_pattern_create_linear (linear->points [0].X, linear->points [0].Y,
					   linear->points [1].X, linear->points [1].Y);

	if (pat == NULL) {
		cairo_surface_destroy (gradient);
		
		return OutOfMemory;
	}
	
	GdipCreateMatrix (&tempMatrix);
	cairo_save (ct);
	{
		ct = cairo_create (gradient);

		if (linear->blend->count > 1)
			add_color_stops_from_blend (pat, linear->blend, linear->lineColors);
		else if (linear->presetColors->count > 1)
			add_color_stops_from_interpolation_colors (pat, linear->presetColors);
		else
			add_color_stops (pat, linear->lineColors);

		cairo_identity_matrix (ct);
		cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);
		cairo_set_source (ct, pat);

		/* left part */
		cairo_rectangle (ct, 0, 0, rect->Width, 2 * rect->Height);
		cairo_fill (ct);
		
		/* right part */
		cairo_translate (ct, rect->Width, 0);
		cairo_rectangle (ct, 0, 0, rect->Width, 2 * rect->Height);
		cairo_matrix_translate (tempMatrix, rect->Width - 1, 0);
		cairo_matrix_scale (tempMatrix, -1.0, 1.0);
		cairo_pattern_set_matrix (pat, tempMatrix);
		cairo_fill (ct);

		cairo_pattern_destroy (pat);
	}
	cairo_restore (ct);

	cairo_matrix_init_identity (tempMatrix);
	/* rotate the pattern if angle is non-zero */
	if (linear->angle != 0) {
		/* Absolute rotation */
		cairo_matrix_translate (tempMatrix, rect-> Width, rect->Height);
		cairo_matrix_rotate (tempMatrix, linear->angle * DEGTORAD);
		cairo_matrix_translate (tempMatrix, -rect-> Width, -rect->Height);
	}
	
	/* if rotation angle is scalable, it should be multiplied by the brush matrix */
	if (linear->isAngleScalable) {
		cairo_matrix_multiply (tempMatrix, tempMatrix, linear->matrix);
	}
	
	linear->pattern = cairo_pattern_create_for_surface (gradient);
	cairo_surface_destroy (gradient);

	/* pattern is transformed wrt to main graphics */
	GdipCreateMatrix (&currMatrix);
	cairo_get_matrix (ct, currMatrix);
	cairo_matrix_multiply (tempMatrix, tempMatrix, currMatrix);
	cairo_set_matrix (ct, tempMatrix);
	GdipDeleteMatrix (currMatrix);
	
	

	return Ok;
}

GpStatus
create_tile_flipY_linear (cairo_t *ct, GpLineGradient *linear)
{
	cairo_surface_t *gradient;
	cairo_pattern_t *pat;
	GpMatrix *tempMatrix = NULL;
	GpMatrix *currMatrix = NULL;
	GpRectF *rect = linear->rectangle;
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	/* gradient surface to be created. We are using CAIRO_CONTENT_COLOR_ALPHA, */
	/* because internally this is the format we use everywhere. */
	gradient = cairo_surface_create_similar (cairo_get_target (ct), CAIRO_CONTENT_COLOR_ALPHA,
						 (int) (2 * rect->Width), (int) (2 * rect->Height));

	if (gradient == NULL) {
		
		return OutOfMemory;
	}

	pat = cairo_pattern_create_linear (linear->points [0].X, linear->points [0].Y,
					   linear->points [1].X, linear->points [1].Y);

	if (pat == NULL) {
		cairo_surface_destroy (gradient);
		
		return OutOfMemory;
	}

	GdipCreateMatrix (&tempMatrix);	
	cairo_save (ct);
	{
		ct = cairo_create (gradient);

		if (linear->blend->count > 1)
			add_color_stops_from_blend (pat, linear->blend, linear->lineColors);
		else if (linear->presetColors->count > 1)
			add_color_stops_from_interpolation_colors (pat, linear->presetColors);
		else
			add_color_stops (pat, linear->lineColors);

		cairo_identity_matrix (ct);
		cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);
		cairo_set_source (ct, pat);

		/* upper part */
		cairo_rectangle (ct, 0, 0, 2 * rect->Width, rect->Height);
		cairo_fill (ct);

		/* lower part */
		cairo_translate (ct, 0, rect->Height);
		cairo_rectangle (ct, 0, 0, 2 * rect->Width, rect->Height);
		cairo_matrix_translate (tempMatrix, 0, rect->Height - 1);
		cairo_matrix_scale (tempMatrix, 1.0, -1.0);
		cairo_pattern_set_matrix (pat, tempMatrix);
		cairo_fill (ct);

		cairo_pattern_destroy (pat);
	}
	cairo_restore (ct);

	cairo_matrix_init_identity (tempMatrix);
	/* rotate the pattern if angle is non-zero */
	if (linear->angle != 0) {
		/* Absolute rotation */
		cairo_matrix_translate (tempMatrix, rect-> Width, rect->Height);
		cairo_matrix_rotate (tempMatrix, linear->angle * DEGTORAD);
		cairo_matrix_translate (tempMatrix, -rect-> Width, -rect->Height);
	}
	
	/* if rotation angle is scalable, it should be multiplied by the brush matrix */
	if (linear->isAngleScalable) {
		cairo_matrix_multiply (tempMatrix, tempMatrix, linear->matrix);
	}
	
	linear->pattern = cairo_pattern_create_for_surface (gradient);
	cairo_surface_destroy (gradient);

	/* pattern is transformed wrt to main graphics */
	GdipCreateMatrix (&currMatrix);
	cairo_get_matrix (ct, currMatrix);
	cairo_matrix_multiply (tempMatrix, tempMatrix, currMatrix);
	cairo_set_matrix (ct, tempMatrix);
	GdipDeleteMatrix (currMatrix);	
	

	return Ok;
}

GpStatus
create_tile_flipXY_linear (cairo_t *ct, GpLineGradient *linear)
{
	cairo_surface_t *gradient;
	cairo_pattern_t *pat;
	GpMatrix *tempMatrix = NULL;
	GpMatrix *currMatrix = NULL;

	GpRectF *rect = linear->rectangle;
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	/* gradient surface to be created. We are using CAIRO_CONTENT_COLOR_ALPHA, */
	/* because internally this is the format we use everywhere. */
	gradient = cairo_surface_create_similar (cairo_get_target (ct), CAIRO_CONTENT_COLOR_ALPHA,
						 (int) (2 * rect->Width), (int) (2 * rect->Height));
	
	if (gradient == NULL) {
		
		return OutOfMemory;
	}

	pat = cairo_pattern_create_linear (linear->points [0].X, linear->points [0].Y,
					   linear->points [1].X, linear->points [1].Y);

	if (pat == NULL) {
		cairo_surface_destroy (gradient);
		
		return OutOfMemory;
	}

	GdipCreateMatrix (&tempMatrix);	
	cairo_save (ct);
	{
		ct = cairo_create (gradient);

		if (linear->blend->count > 1)
			add_color_stops_from_blend (pat, linear->blend, linear->lineColors);
		else if (linear->presetColors->count > 1)
			add_color_stops_from_interpolation_colors (pat, linear->presetColors);
		else
			add_color_stops (pat, linear->lineColors);

		cairo_identity_matrix (ct);
		cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);
		cairo_set_source (ct, pat);

		/* Draw upper left part */
		cairo_rectangle (ct, 0, 0, rect->Width, rect->Height);
		cairo_fill (ct);

		/* Draw lower left part */
		cairo_translate (ct, 0, rect->Height);
		cairo_rectangle (ct, 0, 0, rect->Width, rect->Height);
		/* Not sure if this is a bug, but using rect->Height - 1
		 * avoids the seam.
		 */
		cairo_matrix_translate (tempMatrix, 0, rect->Height - 1);
		/* scale in -Y direction to flip along Y */
		cairo_matrix_scale (tempMatrix, 1.0, -1.0);
		cairo_pattern_set_matrix (pat, tempMatrix);
		cairo_fill (ct);

		/* Draw upper right part */
		cairo_translate (ct, rect->Width, -rect->Height);
		cairo_rectangle (ct, 0, 0, rect->Width, rect->Height);
		/* Reset the pattern matrix and do fresh transformation */
		cairo_matrix_init_identity (tempMatrix);
		/* Not sure if this is a bug, but using rect->Width - 1
		 * avoids the seam.
		 */
		cairo_matrix_translate (tempMatrix, rect->Width - 1, 0);
		/* scale in -X direction to flip along X */
		cairo_matrix_scale (tempMatrix, -1.0, 1.0);
		cairo_pattern_set_matrix (pat, tempMatrix);
		cairo_fill (ct);

		/* Draw lower right part */
		cairo_translate (ct, 0, rect->Height);
		cairo_rectangle (ct, 0, 0, rect->Width, rect->Height);
		/* Not sure if this is a bug, but using rect->Height - 1
		 * avoids the seam.
		 */
		cairo_matrix_translate (tempMatrix, 0, rect->Height - 1);
		/* scale in -Y direction to flip along Y */
		cairo_matrix_scale (tempMatrix, 1.0, -1.0);
		cairo_pattern_set_matrix (pat, tempMatrix);
		cairo_fill (ct);

		cairo_pattern_destroy (pat);
	}
	cairo_restore (ct);

	cairo_matrix_init_identity (tempMatrix);
	/* rotate the pattern if angle is non-zero */
	if (linear->angle != 0) {
		/* Absolute rotation */
		cairo_matrix_translate (tempMatrix, rect-> Width, rect->Height);
		cairo_matrix_rotate (tempMatrix, linear->angle * DEGTORAD);
		cairo_matrix_translate (tempMatrix, -rect-> Width, -rect->Height);
	}
	
	/* if rotation angle is scalable, it should be multiplied by the brush matrix */
	if (linear->isAngleScalable) {
		cairo_matrix_multiply (tempMatrix, tempMatrix, linear->matrix);
	}
	
	linear->pattern = cairo_pattern_create_for_surface (gradient);
	cairo_surface_destroy (gradient);

	/* pattern is transformed wrt to main graphics */
	GdipCreateMatrix (&currMatrix);
	cairo_get_matrix (ct, currMatrix);
	cairo_matrix_multiply (tempMatrix, tempMatrix, currMatrix);
	cairo_set_matrix (ct, tempMatrix);       
	GdipDeleteMatrix (currMatrix);

	return Ok;
}

GpStatus
gdip_linear_gradient_setup (GpGraphics *graphics, GpBrush *brush)
{
	cairo_t *ct;
	GpLineGradient *linear;
	cairo_pattern_t *pattern;
	GpMatrix *product = NULL;
	GpStatus status = Ok;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	ct = graphics->ct;
	g_return_val_if_fail (ct != NULL, InvalidParameter);

	linear = (GpLineGradient *) brush;

	/* We create the new pattern for brush, if the brush is changed
	 * or if pattern has not been created yet.
	 */
	if (linear->base.changed || (linear->pattern) == NULL) {

		/* destroy the existing pattern */
		if (linear->pattern)
			cairo_pattern_destroy (linear->pattern);

		switch (linear->wrapMode) {

		case WrapModeTile:
			status = create_tile_linear (ct, linear);
			break;

		case WrapModeTileFlipX:
			status = create_tile_flipX_linear (ct, linear);
			break;

		case WrapModeTileFlipY:
			status = create_tile_flipY_linear (ct, linear);
			break;

		case WrapModeTileFlipXY:
			status = create_tile_flipXY_linear (ct, linear);
			break;

		default:
			status = InvalidParameter;
		}
	}

	if (status == Ok) {
		if (linear->pattern != NULL) {
			/* Use both the matrices */
			GdipCreateMatrix (&product);
			cairo_matrix_multiply (product, linear->matrix, graphics->copy_of_ctm);
			cairo_matrix_invert (product);
			cairo_pattern_set_matrix (linear->pattern, product);
			cairo_pattern_set_extend (linear->pattern, CAIRO_EXTEND_REPEAT);
			cairo_set_source (ct, linear->pattern);

			status = gdip_get_status (cairo_status (ct));
		}
		else
			status = GenericError;
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

	brush->base.changed = TRUE;

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
GdipSetLineGammaCorrection (GpLineGradient *brush, BOOL useGammaCorrection)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	brush->gammaCorrection = useGammaCorrection;
	brush->base.changed = TRUE;

	return Ok;
}

GpStatus
GdipGetLineGammaCorrection (GpLineGradient *brush, BOOL *useGammaCorrection)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (useGammaCorrection != NULL, InvalidParameter);

	*useGammaCorrection = brush->gammaCorrection;

	return Ok;
}

GpStatus
GdipGetLinePresetBlendCount (GpLineGradient *brush, int *count)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (count != NULL, InvalidParameter);

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

	brush->base.changed = TRUE;
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

	/* If count is less than 2, we are not in a proper state 
	 * to return presetblend property.
	 */
	if (brush->presetColors->count < 2)
		return WrongState;
	
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
	brush->base.changed = TRUE;
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

	gdip_cairo_matrix_copy(matrix, &brush->matrix);

	return Ok;
}

GpStatus
GdipSetLineTransform (GpLineGradient *brush, GDIPCONST GpMatrix *matrix)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	gdip_cairo_matrix_copy(&brush->matrix, matrix);
	brush->base.changed = TRUE;
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
	brush->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipSetLineLinearBlend (GpLineGradient *brush, float focus, float scale)
{
	float *blends;
	float *positions;
	int count = 3;

	g_return_val_if_fail (brush != NULL, InvalidParameter);

	if (focus == 0 || focus == 1) {
		count = 2;
	}

	if (brush->blend->count != count) {
		blends = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (blends != NULL, OutOfMemory);

		positions = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (positions != NULL, OutOfMemory);

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

/* This is defined in general.c */
float gdip_erf (float x, float std, float mean);

GpStatus
GdipSetLineSigmaBlend (GpLineGradient *brush, float focus, float scale)
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

	g_return_val_if_fail (brush != NULL, InvalidParameter);

	if (focus == 0 || focus == 1) {
		count = 256;
	}

	if (brush->blend->count != count) {
		blends = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (blends != NULL, OutOfMemory);

		positions = (float *) GdipAlloc (count * sizeof (float));
		g_return_val_if_fail (positions != NULL, OutOfMemory);

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

GpStatus
GdipMultiplyLineTransform (GpLineGradient *brush, GpMatrix *matrix, GpMatrixOrder order)
{
	GpStatus status;

	g_return_val_if_fail (brush != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	if ((status = GdipMultiplyMatrix (brush->matrix, matrix, order)) == Ok)
		brush->base.changed = TRUE;
	return status;
}

GpStatus
GdipResetLineTransform (GpLineGradient *brush)
{
	GpStatus s;
	cairo_status_t status;
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	cairo_matrix_init_identity (brush->matrix);
       
	//s = gdip_get_status (status);
	//if (s == Ok)
		brush->base.changed = TRUE;
	//return s;
	return Ok;
}

GpStatus
GdipRotateLineTransform (GpLineGradient *brush, float angle, GpMatrixOrder order)
{
	GpStatus status;
	GpRectF *rect;

	g_return_val_if_fail (brush != NULL, InvalidParameter);

	rect = brush->rectangle;

	/* Absolute rotation */
	if ((status = GdipTranslateMatrix (brush->matrix, rect->Width, rect->Height, order)) == Ok)
		if ((status = GdipRotateMatrix (brush->matrix, angle, order)) == Ok)
			if ((status = GdipTranslateMatrix (brush->matrix, -rect->Width, -rect->Height, order)) == Ok)
				brush->base.changed = TRUE;
	return status;
}

GpStatus
GdipScaleLineTransform (GpLineGradient *brush, float sx, float sy, GpMatrixOrder order)
{
	GpStatus status;
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	if ((status = GdipScaleMatrix (brush->matrix, sx, sy, order)) == Ok)
		brush->base.changed = TRUE;
	return status;
}

GpStatus
GdipTranslateLineTransform (GpLineGradient *brush, float dx, float dy, GpMatrixOrder order)
{
	GpStatus status;
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	if ((status = GdipTranslateMatrix (brush->matrix, dx, dy, order)) == Ok)
		brush->base.changed = TRUE;
	return status;
}
