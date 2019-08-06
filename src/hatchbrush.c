/*
 * hatchbrush.c
 * 
 * Copyright (C) 2004,2006-2007 Novell, Inc. http://www.novell.com
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
 *   Ravindra (rkumar@novell.com)
 *
 */

/* FIXME - match hatchbrush use the RenderOrigin stored in graphics */

#include "hatchbrush-private.h"
#include "general-private.h"
#include "graphics-private.h"

static GpStatus gdip_hatch_setup (GpGraphics *graphics, GpBrush *brush);
static GpStatus gdip_hatch_clone (GpBrush *brush, GpBrush **clonedBrush);
static GpStatus gdip_hatch_destroy (GpBrush *brush);

/*
 * We keep a single copy of vtable for
 * all instances of hatchbrush.
 */

static BrushClass vtable = { BrushTypeHatchFill, 
			     gdip_hatch_setup, 
			     gdip_hatch_clone,
			     gdip_hatch_destroy };

static void 
gdip_hatch_init (GpHatch *hatch)
{
	gdip_brush_init (&hatch->base, &vtable);
	hatch->backColor = 0;
	hatch->pattern = NULL;
}

static GpHatch*
gdip_hatch_new (void)
{
	GpHatch *result = (GpHatch *) GdipAlloc (sizeof (GpHatch));

	if (result)
		gdip_hatch_init (result);

	return result;
}

/* helper functions */

static cairo_t*
create_hatch_context (cairo_surface_t *hatch, cairo_line_cap_t line_cap, BOOL antialias)
{
	cairo_t *ct = cairo_create (hatch);
	if (cairo_status (ct) != CAIRO_STATUS_SUCCESS) {
		cairo_destroy (ct);
		return NULL;
	}

	cairo_set_line_cap (ct, line_cap);
	if (!antialias)
		cairo_set_antialias (ct, CAIRO_ANTIALIAS_NONE);
	return ct;
}

static void
set_color (cairo_t *ct, int color, BOOL alpha)
{
	int A = (color & 0xFF000000) >> 24;
	int R = (color & 0x00FF0000) >> 16;
	int G = (color & 0x0000FF00) >> 8;
	int B = (color & 0x000000FF);

	if (alpha) {
		cairo_set_source_rgba (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0, (double) A / 255.0);
	} else {
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);
	}
}

static void
draw_background (cairo_t *ct, int backcolor, int width, int height, BOOL alpha)
{
	set_color (ct, backcolor, alpha);
	cairo_rectangle (ct, 0, 0, width, height);
	cairo_fill (ct);
}

/* The values used below are obtained by hit and trial to get the results similar to that of MS. */
static const double hatches_const[][3] = {
	/* HatchStyleHorizontal */		{ HATCH_SIZE, HATCH_SIZE, LINE_WIDTH },
	/* HatchStyleVertical */		{ HATCH_SIZE, HATCH_SIZE, LINE_WIDTH },
	/* HatchStyleForwardDiagonal */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleBackwardDiagonal */	{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleCross */			{ HATCH_SIZE + 2.0, HATCH_SIZE + 2.0, LINE_WIDTH },
	/* HatchStyleDiagonalCross */		{ HATCH_SIZE, HATCH_SIZE, LINE_WIDTH },
	/* HatchStyle05Percent */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyle10Percent */		{ HATCH_SIZE + 1.0, 4.0, LINE_WIDTH },
	/* HatchStyle20Percent */		{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyle25Percent */		{ 4.0, 2.0, LINE_WIDTH },
	/* HatchStyle30Percent */		{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyle40Percent */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyle50Percent */		{ 2.0, 2.0, LINE_WIDTH },
	/* HatchStyle60Percent */		{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyle70Percent */		{ 4.0, 2.0, LINE_WIDTH },
	/* HatchStyle75Percent */		{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyle80Percent */		{ HATCH_SIZE + 1.0, 4.0, LINE_WIDTH },
	/* HatchStyle90Percent */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleLightDownwardDiagonal */	{ (HATCH_SIZE + 1.0) * 0.5, (HATCH_SIZE + 1.0) * 0.5, LINE_WIDTH },
	/* HatchStyleLightUpwardDiagonal */	{ (HATCH_SIZE + 1.0) * 0.5, (HATCH_SIZE + 1.0) * 0.5, LINE_WIDTH },
	/* HatchStyleDarkDownwardDiagonal */	{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyleDarkUpwardDiagonal */	{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyleWideDownwardDiagonal */	{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH * 2.0 },
	/* HatchStyleWideUpwardDiagonal */	{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH * 2.0 },
	/* HatchStyleLightVertical */		{ HATCH_SIZE * 0.7, HATCH_SIZE * 0.7, LINE_WIDTH },
	/* HatchStyleLightHorizontal */		{ HATCH_SIZE * 0.7, HATCH_SIZE * 0.7, LINE_WIDTH },
	/* HatchStyleNarrowVertical */		{ 2.0, 2.0, LINE_WIDTH },
	/* HatchStyleNarrowHorizontal */	{ 2.0, 2.0, LINE_WIDTH },
	/* HatchStyleDarkVertical */		{ HATCH_SIZE * 0.6, HATCH_SIZE * 0.6, LINE_WIDTH * 2.0 },
	/* HatchStyleDarkHorizontal */		{ HATCH_SIZE * 0.6, HATCH_SIZE * 0.6, LINE_WIDTH * 2.0 },
	/* HatchStyleDashedDownwardDiagonal */	{ 4.0, 8.0, LINE_WIDTH },
	/* HatchStyleDashedUpwardDiagonal */	{ 4.0, 8.0, LINE_WIDTH },
	/* HatchStyleDashedHorizontal */	{ 8.0, 8.0, LINE_WIDTH },
	/* HatchStyleDashedVertical */		{ 8.0, 8.0, LINE_WIDTH },
	/* HatchStyleSmallConfetti */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleLargeConfetti */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleZigZag */			{ HATCH_SIZE + 1.0, 4.0, LINE_WIDTH },
	/* HatchStyleWave */			{ HATCH_SIZE + 2.0, (HATCH_SIZE + 2.0) / 2.0, LINE_WIDTH },
	/* HatchStyleDiagonalBrick */		{ HATCH_SIZE + 0.0, HATCH_SIZE + 0.0, LINE_WIDTH },
	/* HatchStyleHorizontalBrick */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleWeave */			{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStylePlaid */			{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleDivot */			{ 9.0, 9.0, LINE_WIDTH },
	/* HatchStyleDottedGrid */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleDottedDiamond */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleShingle */			{ HATCH_SIZE + 1.0, HATCH_SIZE - 1.0, LINE_WIDTH },
	/* HatchStyleTrellis */			{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyleSphere */			{ 9.0, 9.0, LINE_WIDTH },
	/* HatchStyleSmallGrid */		{ (HATCH_SIZE + 2.0) * 0.6, (HATCH_SIZE + 2.0) * 0.6, LINE_WIDTH },
	/* HatchStyleSmallCheckerBoard */	{ 4.0, 4.0, LINE_WIDTH },
	/* HatchStyleLargeCheckerBoard */	{ 8.0, 8.0, LINE_WIDTH },
	/* HatchStyleOutlinedDiamond */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH },
	/* HatchStyleSolidDiamond */		{ HATCH_SIZE + 1.0, HATCH_SIZE + 1.0, LINE_WIDTH }
};

/*
 * functions to draw different hatch styles.
 */

static cairo_status_t
draw_horizontal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw horizontal line in the foreground color */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	cairo_move_to (ct2, 0, hatch_size / 2.0);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_vertical_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw vertical line in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	cairo_move_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_forward_diagonal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, TRUE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw forward diagonal lines in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	cairo_move_to (ct2, 0, hatch_size / 2.0);
	cairo_line_to (ct2, hatch_size / 2.0, hatch_size);
	cairo_move_to (ct2, hatch_size / 2.0, 0);
	cairo_line_to (ct2, hatch_size, hatch_size / 2.0);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_backward_diagonal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, TRUE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw backward diagonal lines in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	cairo_move_to (ct2, hatch_size / 2.0, 0);
	cairo_line_to (ct2, 0, hatch_size / 2.0 );
	cairo_move_to (ct2, hatch_size, hatch_size / 2.0);
	cairo_line_to (ct2, hatch_size / 2.0, hatch_size);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_cross_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	double dash [] = {1.0}; /* used for drawing dotted grid */
	BOOL dotted = (hbr->hatchStyle == HatchStyleDottedGrid);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, dotted ? CAIRO_LINE_CAP_BUTT : CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw lines in the foregound */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	if (hbr->hatchStyle == HatchStyleDottedGrid)
		cairo_set_dash (ct2, dash, 1, 0);
	/* draw a horizontal line */
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	/* draw a vertical line */
	cairo_move_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_diagonal_cross_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, TRUE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw diagonal lines in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* draw a forward diagonal line */
	cairo_move_to (ct2, 0, 0);
	cairo_line_to (ct2, hatch_size, hatch_size);
	/* draw a backward diagonal line */
	cairo_move_to (ct2, hatch_size, 0);
	cairo_line_to (ct2, 0, hatch_size);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_percent_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_wd = gdip_hatch_get_width (hbr);
	double hatch_ht = gdip_hatch_get_height (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* some patterns requires to reverse the colors */
	switch (hbr->hatchStyle) {
	case HatchStyle05Percent:
	case HatchStyle10Percent:
	case HatchStyle20Percent:
	case HatchStyle25Percent:
		draw_background (ct2, hbr->backColor, hatch_wd, hatch_ht, hbr->alpha);
		set_color (ct2, hbr->foreColor, hbr->alpha);
		break;
	default:
		draw_background (ct2, hbr->foreColor, hatch_wd, hatch_ht, hbr->alpha);
		set_color (ct2, hbr->backColor, hbr->alpha);
	}
	/* draw four dots */
	/* left middle */
	gdip_cairo_pixel (ct2, 0, hatch_ht / 2.0);
	/* top middle */
	gdip_cairo_pixel (ct2, hatch_wd / 2.0, 0);
	/* bottom middle */
	gdip_cairo_pixel (ct2, hatch_wd / 2.0, hatch_ht);
	/* right middle */
	gdip_cairo_pixel (ct2, hatch_wd, hatch_ht / 2.0);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_30_percent_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* HatchStyle30Percent and HatchStyle60Percent are identical, but the colors are reversed */
	if (hbr->hatchStyle == HatchStyle60Percent) {
		draw_background (ct2, hbr->foreColor, hatch_size, hatch_size, hbr->alpha);
		set_color (ct2, hbr->backColor, hbr->alpha);
	} else {
		draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
		set_color (ct2, hbr->foreColor, hbr->alpha);
	}
	/* draw two diagonal lines in the foreground */
	cairo_set_line_width (ct2, line_width);
	/* near top left corner */
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	/* near top right corner */
	cairo_move_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* simulate the dots by drawing lines in background color */
	if (hbr->hatchStyle == HatchStyleDottedDiamond) {
		double i;
		set_color (ct2, hbr->backColor, hbr->alpha);
		for (i = 0.0; i <= hatch_size; i += 2.0) {
			cairo_move_to (ct2, i, 0.0);
			cairo_line_to (ct2, i, hatch_size);
		}
		cairo_stroke (ct2);
	}
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_40_percent_hatch (cairo_t *ct, cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	cairo_pattern_t *pattern;
	cairo_t	*ct2;
	double temp_size = 2;
	/* calculate radius using, pi * square (r) = % of hatch area */
	double rad = sqrt ((10.0 * hatch_size * hatch_size) / (2 * PI * 100.0));

	cairo_surface_t *temp = cairo_surface_create_similar (cairo_get_target (ct), 
		CAIRO_CONTENT_COLOR_ALPHA, temp_size, temp_size);
	if (cairo_surface_status (temp) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy (temp);
		return CAIRO_STATUS_NO_MEMORY;
	}
		
	/* draw a 50% hatch */
	ct2 = create_hatch_context (temp, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL) {
		cairo_surface_destroy (temp);
		return CAIRO_STATUS_NO_MEMORY;
	}
	/* draw background */
	draw_background (ct2, hbr->backColor, temp_size, temp_size, hbr->alpha);
	/* draw two diagonal lines in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_rectangle (ct2, 0, 0, temp_size / 2.0, temp_size / 2.0);
	cairo_rectangle (ct2, temp_size / 2.0, temp_size / 2.0, temp_size, temp_size);
	cairo_fill (ct2);

	/* set the pattern for the consequent fill or stroke */
	pattern = cairo_pattern_create_for_surface (temp);
	if (cairo_pattern_status (pattern) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy (temp);
		cairo_destroy (ct2);
		cairo_pattern_destroy (pattern);
		return CAIRO_STATUS_NO_MEMORY;
	}
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	/* Clean up 50% */
	cairo_surface_destroy (temp);
	cairo_destroy (ct2);
	/* temp hatch is used as background, so we get 50% hatch */
	ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL) {
		cairo_pattern_destroy (pattern);
		return CAIRO_STATUS_NO_MEMORY;
	}
	cairo_set_source (ct2, pattern);
	cairo_rectangle (ct2, 0, 0, hatch_size, hatch_size);
	cairo_fill (ct2);
	/* foreground is filled with 10% hatch of background color */
	set_color (ct2, hbr->backColor, hbr->alpha);
	/* 0.5 is added for pixel adjustment. It's ugly but it makes output better. */
	/* draw four semi circles in the foregound to make dots */
	/* left middle */
	cairo_arc (ct2, 0, hatch_size / 2.0, rad, - PI / 2.0, PI / 2.0);
	cairo_fill (ct2);
	/* top middle */
	cairo_arc (ct2, hatch_size / 2.0, 0, rad, 0, PI);
	cairo_fill (ct2);
	/* bottom middle */
	cairo_arc (ct2, hatch_size / 2.0, hatch_size, rad, - PI, 0);
	cairo_fill (ct2);
	/* right middle */
	cairo_arc (ct2, hatch_size, hatch_size / 2.0, rad, PI / 2.0, - PI / 2.0);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	cairo_pattern_destroy (pattern);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_50_percent_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw two rectangles in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_rectangle (ct2, 0, 0, hatch_size / 2.0, hatch_size / 2.0);
	cairo_rectangle (ct2, hatch_size / 2.0, hatch_size / 2.0, hatch_size, hatch_size);
	cairo_fill (ct2);		
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_downward_diagonal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw diagonal line in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_move_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_dark_diagonal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw diagonal line in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	if (hbr->hatchStyle == HatchStyleDarkDownwardDiagonal) {
		cairo_rectangle (ct2, 0, 0, 2, 1);
		cairo_rectangle (ct2, 1, 1, 2, 1);
		cairo_rectangle (ct2, 2, 2, 2, 1);
		cairo_rectangle (ct2, 0, 3, 1, 1);
		cairo_rectangle (ct2, 3, 3, 1, 1);
	} else {
		cairo_rectangle (ct2, 1, 3, 2, 1);
		cairo_rectangle (ct2, 2, 2, 2, 1);
		cairo_rectangle (ct2, 3, 1, 1, 1);
		cairo_rectangle (ct2, 0, 1, 1, 1);
		cairo_rectangle (ct2, 0, 0, 2, 1);
	}
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_upward_diagonal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw diagonal line in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	cairo_move_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_move_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_dashed_diagonal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_wd = gdip_hatch_get_width (hbr);
	double hatch_ht = gdip_hatch_get_height (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_wd, hatch_ht, hbr->alpha);
	/* draw slant line in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	if (hbr->hatchStyle == HatchStyleDashedUpwardDiagonal) {
		/* 45 deg slant line */
		cairo_move_to (ct2, hatch_wd + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
		cairo_line_to (ct2, CAIRO_AA_OFFSET_X, hatch_wd + CAIRO_AA_OFFSET_Y);
	} else { /* HatchStyleDashedDownwardDiagonal */
		/* -45 deg slant line */
		cairo_move_to (ct2, CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
		cairo_line_to (ct2, hatch_wd, hatch_wd - CAIRO_AA_OFFSET_Y);
	}
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_dashed_horizontal_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_wd = gdip_hatch_get_width (hbr);
	double hatch_ht = gdip_hatch_get_height (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	double line_space = 4;
	double dash_len = 4;
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_wd, hatch_ht, hbr->alpha);
	/* draw horizontal lines in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* upper left dash */
	cairo_move_to (ct2, 0, line_space / 2 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, dash_len, line_space / 2 + CAIRO_AA_OFFSET_Y);
	/* lower right dash */
	cairo_move_to (ct2, hatch_wd - dash_len + CAIRO_AA_OFFSET_X, hatch_ht - line_space / 2 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_wd, hatch_ht - line_space / 2 + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_dashed_vertical_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_wd = gdip_hatch_get_width (hbr);
	double hatch_ht = gdip_hatch_get_height (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	double line_space = 4;
	double dash_len = 4;
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_wd, hatch_ht, hbr->alpha);
	/* draw vertical lines in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* upper left dash */
	cairo_move_to (ct2, line_space / 2 + CAIRO_AA_OFFSET_X, 0);
	cairo_line_to (ct2, line_space / 2 + CAIRO_AA_OFFSET_X, dash_len - CAIRO_AA_OFFSET_Y);
	/* lower right dash */
	cairo_move_to (ct2, hatch_wd - line_space / 2 + CAIRO_AA_OFFSET_X, hatch_ht - dash_len + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_wd - line_space / 2 + CAIRO_AA_OFFSET_X, hatch_ht);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_confetti_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double confetti_size = (hbr->hatchStyle == HatchStyleLargeConfetti) ? 1.5 : 1.0;
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw small rectangles in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	/* draw four rectangles on the left reversed 'L' shape */
	cairo_rectangle (ct2, 0, 0, confetti_size, confetti_size);
	cairo_rectangle (ct2, hatch_size / 8.0, hatch_size / 4.0, confetti_size, confetti_size);
	cairo_rectangle (ct2, 3 * hatch_size / 8.0, hatch_size / 2.0, confetti_size, confetti_size);
	cairo_rectangle (ct2, hatch_size / 4.0, 3 * hatch_size / 4.0, confetti_size, confetti_size);
	/* draw four rectangles on the other left reversed 'L' shape */
	cairo_rectangle (ct2, hatch_size / 2.0, hatch_size / 8.0, confetti_size, confetti_size);
	cairo_rectangle (ct2, 3 * hatch_size / 4.0, 3 * hatch_size / 8.0, confetti_size, confetti_size);
	cairo_rectangle (ct2, 7 * hatch_size / 8.0, 5 * hatch_size / 8.0, confetti_size, confetti_size);
	cairo_rectangle (ct2, 5 * hatch_size / 8.0, 7 * hatch_size / 8.0, confetti_size, confetti_size);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_zigzag_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	set_color (ct2, hbr->foreColor, hbr->alpha);
	/* draw two diagonal lines in the foreground */
	cairo_set_line_width (ct2, line_width);
	/* first half*/
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	/* second half */
	cairo_move_to (ct2, hatch_size - 1 + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, 1 - CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_wave_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_wd = gdip_hatch_get_width (hbr);
	double hatch_ht = gdip_hatch_get_height (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_wd, hatch_ht, hbr->alpha);
	/* set foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* draw a horizontal 'S' shape */
	cairo_move_to (ct2, 1.0 + CAIRO_AA_OFFSET_X, 1.0 + CAIRO_AA_OFFSET_Y);
	cairo_curve_to (ct2, 1.0 + CAIRO_AA_OFFSET_X, 1.0 + CAIRO_AA_OFFSET_Y, hatch_wd / 4.0 + CAIRO_AA_OFFSET_X, hatch_ht + CAIRO_AA_OFFSET_Y, hatch_wd / 2.0 + CAIRO_AA_OFFSET_X, hatch_ht / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_curve_to (ct2, hatch_wd / 2.0 + CAIRO_AA_OFFSET_X, hatch_ht / 2.0 + CAIRO_AA_OFFSET_Y, 3 * hatch_wd / 4.0 + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y, hatch_wd - 1.0 + CAIRO_AA_OFFSET_X, hatch_ht - 1.0 + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_diagonal_brick_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* set foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* draw one full diagonal line and half of another diagonal line */
	gdip_cairo_pixel (ct2, 0, 0);
	cairo_fill (ct2);
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_move_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_horizontal_brick_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* set foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* draw a 'C' shape */
	cairo_move_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	/* draw lower connector */
	cairo_move_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_weave_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	double dash [] = {0.5};
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* set foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	cairo_set_dash (ct2, dash, 1, 0);
	/* draw a reversed 'C' shape diagonally with one connector towards
	 * one of the other two vertices of the square and three connectors
	 * towards the fourth vertex.
	 */
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	/* this makes up one of the lower connectors also */
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	/* complete the diagonal 'C' shape */
	cairo_move_to (ct2, 3 * hatch_size / 4.0 + CAIRO_AA_OFFSET_X, hatch_size / 4.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	/* draw one upper connector */
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 4.0 + CAIRO_AA_OFFSET_X, hatch_size / 4.0 + CAIRO_AA_OFFSET_Y);
	/* draw lower connectors */
	cairo_move_to (ct2, hatch_size / 4.0 + CAIRO_AA_OFFSET_X, 3 * hatch_size / 4.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_move_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_plaid_hatch (cairo_t *ct, cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double temp_size = 2;
	cairo_t *ct2;
	cairo_pattern_t *pattern;
	/* draw one hatch that has 50% of area as 50% hatch and remaining area
	 * equally covered by each of foreground and background colors.
	 */
	cairo_surface_t *temp = cairo_surface_create_similar (cairo_get_target (ct), 
		CAIRO_CONTENT_COLOR_ALPHA, temp_size, temp_size);
	if (cairo_surface_status (temp) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy (temp);
		return CAIRO_STATUS_NO_MEMORY;
	}

	/* draw a 50% hatch */
	ct2 = create_hatch_context (temp, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL) {
		cairo_surface_destroy (temp);
		return CAIRO_STATUS_NO_MEMORY;
	}
	/* draw background */
	draw_background (ct2, hbr->backColor, temp_size, temp_size, hbr->alpha);
	/* draw two diagonal lines in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_rectangle (ct2, 0, 0, temp_size / 2.0, temp_size / 2.0);
	cairo_rectangle (ct2, temp_size / 2.0, temp_size / 2.0, temp_size, temp_size);
	cairo_fill (ct2);

	/* create the pattern for the consequent fill or stroke */
	pattern = cairo_pattern_create_for_surface (temp);
	if (cairo_pattern_status (pattern) != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy (temp);
		cairo_destroy (ct2);
		cairo_pattern_destroy (pattern);
		return CAIRO_STATUS_NO_MEMORY;
	}
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	/* Clean up the 50% objects */
	cairo_surface_destroy (temp);
	cairo_destroy (ct2);

	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL) {
		cairo_pattern_destroy (pattern);
		return CAIRO_STATUS_NO_MEMORY;
	}
	/* temp hatch is used to fill 50% of area */
	cairo_set_source(ct2, pattern);
	cairo_rectangle (ct2, 0, 0, hatch_size, hatch_size / 2.0);
	cairo_fill (ct2);
	/* draw two rectangle in the foregound to make up 50% of plaid */
	/* foreground is filled with 25% of background color and 25% of foreground color */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_rectangle (ct2, 0, hatch_size / 2.0, hatch_size / 2.0, hatch_size / 2.0);
	cairo_fill (ct2);
	set_color (ct2, hbr->backColor, hbr->alpha);
	cairo_rectangle (ct2, hatch_size / 2.0, hatch_size / 2.0, hatch_size / 2.0, hatch_size / 2.0);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	cairo_pattern_destroy (pattern);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_divot_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	double x, y;
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* set foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* draw two angle brackets in the foreground */
	x = hatch_size / 8.0 + CAIRO_AA_OFFSET_X;
	y = hatch_size / 8.0 + CAIRO_AA_OFFSET_Y;
	gdip_cairo_pixel (ct2, x, y);
	gdip_cairo_pixel (ct2, x + 1, y + 1);
	gdip_cairo_pixel (ct2, x, y + 2);
	x += 5;
	y += 6;
	gdip_cairo_pixel (ct2, x, y);
	gdip_cairo_pixel (ct2, x - 1, y - 1);
	gdip_cairo_pixel (ct2, x, y - 2);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_solid_diamond_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_wd = gdip_hatch_get_width (hbr);
	double hatch_ht = gdip_hatch_get_height (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_BUTT, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_wd, hatch_ht, hbr->alpha);
	/* draw two triangles in the foreground with left and right sides as their bases */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	/* draw left triangle */
	cairo_move_to (ct2, -0.5, 0.5);
	cairo_line_to (ct2, hatch_wd / 2.0 - 0.5, hatch_ht / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, 0, hatch_ht);
	cairo_close_path (ct2);
	cairo_fill (ct2);
	/* draw right triangle */
	cairo_move_to (ct2, hatch_wd, 0.5);
	cairo_line_to (ct2, hatch_wd / 2.0 + 0.5, hatch_ht / 2.0);
	cairo_line_to (ct2, hatch_wd, hatch_ht);
	cairo_close_path (ct2);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_shingle_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	double dash [] = {1.0};
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw two curves in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	cairo_set_dash (ct2, dash, 1, 0);
	cairo_set_line_width (ct2, line_width);
	/* draw a curve thru left top, center and right top corners */
	cairo_move_to (ct2, CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size / 4.0 + CAIRO_AA_OFFSET_X, hatch_size / 4.0 + CAIRO_AA_OFFSET_Y);
	cairo_curve_to (ct2, hatch_size / 4.0 + CAIRO_AA_OFFSET_X, hatch_size / 4.0 + CAIRO_AA_OFFSET_Y, hatch_size / 2.0 + CAIRO_AA_OFFSET_X, hatch_size / 2.0 + CAIRO_AA_OFFSET_Y, 3 * hatch_size / 4.0 + CAIRO_AA_OFFSET_X, hatch_size / 4.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size + CAIRO_AA_OFFSET_X, CAIRO_AA_OFFSET_Y);
	/* draw a quarter circle thru center and right bottom corner */
	cairo_arc (ct2, CAIRO_AA_OFFSET_X, hatch_size + CAIRO_AA_OFFSET_Y, hatch_size, -PI/4.0, 0);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_trellis_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* note: we swap the colors (easier to draw this way) */
	/* draw background */
	draw_background (ct2, hbr->foreColor, hatch_size, hatch_size, hbr->alpha);
	/* draw rectangles in the foreground */
	set_color (ct2, hbr->backColor, hbr->alpha);
	/* upper left rectangle */
	cairo_rectangle (ct2,0, hatch_size / 4.0, hatch_size / 2.0, hatch_size / 4.0);
	/* lower right rectangle */
	cairo_rectangle (ct2, hatch_size / 2.0, 3 * hatch_size / 4.0, hatch_size / 2.0, hatch_size / 4.0);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_sphere_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	double line_width = gdip_hatch_get_line_width (hbr);
	double rad = hatch_size / 4.0 + 0.5;
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw half circles in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	/* left middle */
	cairo_arc (ct2, 0, hatch_size / 2.0, rad, - PI / 2.0, PI / 2.0);
	cairo_fill (ct2);
	/* top middle */
	cairo_arc (ct2, hatch_size / 2.0, 0, rad, 0, PI);
	cairo_fill (ct2);
	/* bottom middle */
	cairo_arc (ct2, hatch_size / 2.0, hatch_size, rad, - PI, 0);
	cairo_fill (ct2);
	/* right middle */
	cairo_arc (ct2, hatch_size, hatch_size / 2.0, rad, PI / 2.0, - PI / 2.0);
	cairo_fill (ct2);
	/* draw lines to mark sphere shining */
	set_color (ct2, hbr->backColor, hbr->alpha);
	cairo_set_line_width (ct2, line_width);
	/* first line */
	cairo_move_to (ct2, hatch_size - 1.0, hatch_size / 2.0 - rad / 2.0 + CAIRO_AA_OFFSET_Y);
	cairo_line_to (ct2, hatch_size - rad / 2.0 + CAIRO_AA_OFFSET_X, hatch_size / 2.0 - rad / 2.0 + CAIRO_AA_OFFSET_Y);
	/* second line */
	cairo_move_to (ct2, hatch_size / 2.0 - rad / 4.0, hatch_size - rad / 2.0);
	cairo_line_to (ct2, hatch_size / 2.0 + rad / 4.0 - 1.0, hatch_size - rad / 2.0);
	cairo_stroke (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
draw_checker_hatch (cairo_surface_t *hatch, GpHatch *hbr)
{
	double hatch_size = gdip_hatch_get_width (hbr);
	/* draw one hatch that has two colored squares at upper left and lower right corners */
	/* hatch is not supposed to be affected by user matrix (so we create a new context) */
	cairo_t *ct2 = create_hatch_context (hatch, CAIRO_LINE_CAP_SQUARE, FALSE);
	if (ct2 == NULL)
		return CAIRO_STATUS_NO_MEMORY;
	/* draw background */
	draw_background (ct2, hbr->backColor, hatch_size, hatch_size, hbr->alpha);
	/* draw squares in the foreground */
	set_color (ct2, hbr->foreColor, hbr->alpha);
	/* upper left square */
	cairo_rectangle (ct2, 0, 0, hatch_size / 2.0, hatch_size / 2.0);
	/* lower right square */
	cairo_rectangle (ct2, hatch_size / 2.0, hatch_size / 2.0, hatch_size, hatch_size);
	cairo_fill (ct2);
	/* clean up */
	cairo_destroy (ct2);
	return CAIRO_STATUS_SUCCESS;
}

GpStatus
gdip_hatch_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpHatch *hbr;
	cairo_t *ct;
	cairo_status_t status;

	if (!graphics || !brush)
		return InvalidParameter;

	ct = graphics->ct;
	if (!ct)
		return InvalidParameter;

	/* We create the new pattern for brush, if the brush is changed
	 * or if pattern has not been created yet.
	 */
	hbr = (GpHatch *) brush;
	if (hbr->base.changed || (hbr->pattern) == NULL) {
		cairo_surface_t *hatch;
		cairo_antialias_t alias;
		int width = gdip_hatch_get_width (hbr);
		int height = gdip_hatch_get_height (hbr);

		hbr->alpha = (graphics->composite_mode == CompositingModeSourceOver);
		
		/* destroy the existing pattern */
		if (hbr->pattern)
			cairo_pattern_destroy (hbr->pattern);

		hatch = cairo_surface_create_similar (cairo_get_target (ct), CAIRO_CONTENT_COLOR_ALPHA, width, height);
		status = cairo_surface_status (hatch);
		if (status != CAIRO_STATUS_SUCCESS) {
			cairo_surface_destroy (hatch);
			return gdip_get_status (status);
		}

		alias = cairo_get_antialias (ct);
		cairo_set_antialias(ct, CAIRO_ANTIALIAS_NONE);

		switch (hbr->hatchStyle) {

		/* case HatchStyleMin: */
		case HatchStyleHorizontal:
		case HatchStyleLightHorizontal:
		case HatchStyleNarrowHorizontal:
		case HatchStyleDarkHorizontal:
			status = draw_horizontal_hatch (hatch, hbr);
			break;

		case HatchStyleVertical:
		case HatchStyleLightVertical:
		case HatchStyleNarrowVertical:
		case HatchStyleDarkVertical:
			status = draw_vertical_hatch (hatch, hbr);
			break;

		case HatchStyleForwardDiagonal:
			status = draw_forward_diagonal_hatch (hatch, hbr);
			break;

		case HatchStyleBackwardDiagonal:
			status = draw_backward_diagonal_hatch (hatch, hbr);
			break;

		/* case HatchStyleCross: */
		case HatchStyleLargeGrid:
		case HatchStyleSmallGrid:
		case HatchStyleDottedGrid:
			status = draw_cross_hatch (hatch, hbr);
			break;

		case HatchStyleDiagonalCross:
			status = draw_diagonal_cross_hatch (hatch, hbr);
			break;

		case HatchStyle05Percent:
		case HatchStyle10Percent:
		case HatchStyle20Percent:
		case HatchStyle25Percent:
		case HatchStyle70Percent: /* same as 25% but colors will be reversed (not a typo for 75) */
		case HatchStyle75Percent: /* same as 20% but colors will be reversed (not a typo for 80) */
		case HatchStyle80Percent: /* same as 10% but colors will be reversed (not a typo for 90) */
		case HatchStyle90Percent: /* same as 5% but colors will be reversed (not a typo) */
			status = draw_percent_hatch (hatch, hbr);
			break;

		case HatchStyle30Percent:
		case HatchStyle60Percent:	/* same as 30% but colors will be reversed (not a typo for 70) */
		case HatchStyleOutlinedDiamond:	/* hack: perfect width */
		case HatchStyleDottedDiamond:	/* hack */
			status = draw_30_percent_hatch (hatch, hbr);
			break;

		case HatchStyle40Percent:
			status = draw_40_percent_hatch (ct, hatch, hbr);
			break;

		case HatchStyle50Percent:
			status = draw_50_percent_hatch (hatch, hbr);
			break;

		case HatchStyleLightDownwardDiagonal:
		case HatchStyleWideDownwardDiagonal:
			status = draw_downward_diagonal_hatch (hatch, hbr);
			break;

		case HatchStyleDarkUpwardDiagonal:
		case HatchStyleDarkDownwardDiagonal:
			status = draw_dark_diagonal_hatch (hatch, hbr);
			break;

		case HatchStyleLightUpwardDiagonal:
		case HatchStyleWideUpwardDiagonal:
			status = draw_upward_diagonal_hatch (hatch, hbr);
			break;

		case HatchStyleDashedDownwardDiagonal:
		case HatchStyleDashedUpwardDiagonal:
			status = draw_dashed_diagonal_hatch (hatch, hbr);
			break;

		case HatchStyleDashedHorizontal:
			status = draw_dashed_horizontal_hatch (hatch, hbr);
			break;

		case HatchStyleDashedVertical:
			status = draw_dashed_vertical_hatch (hatch, hbr);
			break;

		case HatchStyleSmallConfetti:
		case HatchStyleLargeConfetti:
			status = draw_confetti_hatch (hatch, hbr);
			break;

		case HatchStyleZigZag:
			status = draw_zigzag_hatch (hatch, hbr);
			break;

		case HatchStyleWave:
			status = draw_wave_hatch (hatch, hbr);
			break;

		case HatchStyleDiagonalBrick:
			status = draw_diagonal_brick_hatch (hatch, hbr);
			break;

		case HatchStyleHorizontalBrick:
			status = draw_horizontal_brick_hatch (hatch, hbr);
			break;

		case HatchStyleWeave:
			status = draw_weave_hatch (hatch, hbr);
			break;

		case HatchStylePlaid:
			status = draw_plaid_hatch (ct, hatch, hbr);
			break;

		case HatchStyleDivot:
			status = draw_divot_hatch (hatch, hbr);
			break;

		case HatchStyleShingle:
			status = draw_shingle_hatch (hatch, hbr);
			break;

		case HatchStyleTrellis:
			status = draw_trellis_hatch (hatch, hbr);
			break;

		case HatchStyleSphere:
			status = draw_sphere_hatch (hatch, hbr);
			break;

		case HatchStyleSmallCheckerBoard:
		case HatchStyleLargeCheckerBoard:
			status = draw_checker_hatch (hatch, hbr);
			break;

		/* case HatchStyleMax: */
		case HatchStyleSolidDiamond:
			status = draw_solid_diamond_hatch (hatch, hbr);
			break;

		default:
			status = CAIRO_STATUS_INVALID_RESTORE; /* will be converted into InvalidParameter */
			break;
		}

		cairo_set_antialias (ct, alias);
		if (status != CAIRO_STATUS_SUCCESS) {
			cairo_surface_destroy (hatch);
			return gdip_get_status (status);
		}

		/* create and verity the pattern created from the surface */
			hbr->pattern = cairo_pattern_create_for_surface (hatch);
		status = cairo_pattern_status (hbr->pattern);
		if (status != CAIRO_STATUS_SUCCESS) {
			cairo_pattern_destroy (hbr->pattern);
			hbr->pattern = NULL;
			cairo_surface_destroy (hatch);
			return gdip_get_status (status);
		}
 
		/* finally set the pattern into the context and destroy the hatch surface */
		cairo_pattern_set_extend (hbr->pattern, CAIRO_EXTEND_REPEAT);
		cairo_surface_destroy (hatch);
	}

	cairo_set_source (ct, hbr->pattern);
	return gdip_get_status (cairo_status (ct));
}

GpStatus
gdip_hatch_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpHatch *result;
	GpHatch *hatch;

	if (!brush || !clonedBrush)
		return InvalidParameter;

	result = gdip_hatch_new ();
	if (!result) {
		*clonedBrush = NULL;
		return OutOfMemory;
	}

	hatch = (GpHatch *) brush;

	result->base = hatch->base;
	result->hatchStyle = hatch->hatchStyle;
	result->foreColor = hatch->foreColor;
	result->backColor = hatch->backColor;
	result->base.changed = TRUE;
	result->pattern = NULL;

	*clonedBrush = (GpBrush *) result;
	return Ok;
}

GpStatus
gdip_hatch_destroy (GpBrush *brush)
{
	GpHatch *hatch = (GpHatch *) brush;

	if (hatch->pattern) {
		cairo_pattern_destroy (hatch->pattern);
		hatch->pattern = NULL;
	}

	return Ok;
}

// coverity[+alloc : arg-*3]
GpStatus
GdipCreateHatchBrush (GpHatchStyle hatchstyle, ARGB forecol, ARGB backcol, GpHatch **brush)
{
	GpHatch *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!brush)
		return InvalidParameter;

	if (hatchstyle < HatchStyleMin || hatchstyle > HatchStyleMax)
		return InvalidParameter;

	result = gdip_hatch_new ();
	if (!result)
		return OutOfMemory;

	result->hatchStyle = hatchstyle;
	result->foreColor = forecol;
	result->backColor = backcol;

	*brush = result;
	return Ok; 
}

GpStatus
GdipGetHatchStyle (GpHatch *brush, GpHatchStyle *hatchstyle)
{
	if (!brush || !hatchstyle)
		return InvalidParameter;

	*hatchstyle = brush->hatchStyle;
	return Ok;
}

GpStatus
GdipGetHatchForegroundColor (GpHatch *brush, ARGB *forecol)
{
	if (!brush || !forecol)
		return InvalidParameter;

	*forecol = brush->foreColor;
	return Ok;
}

GpStatus
GdipGetHatchBackgroundColor (GpHatch *brush, ARGB *backcol)
{
	if (!brush || !backcol)
		return InvalidParameter;

	*backcol = brush->backColor;
	return Ok;
}
