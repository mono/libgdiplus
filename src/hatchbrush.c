/*
 * hatchbrush.c
 * 
 * Copyright (C) 2004 Novell, Inc. http://www.novell.com
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

#include <math.h>
#include "gdip.h"
#include "hatchbrush.h"

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

void 
gdip_hatch_init (GpHatch *hatch)
{
	gdip_brush_init (&hatch->base, &vtable);
	hatch->backColor = 0;
}

GpHatch*
gdip_hatch_new (void)
{
        GpHatch *result = (GpHatch *) GdipAlloc (sizeof (GpHatch));

	if (result)
	        gdip_hatch_init (result);

        return result;
}

/*
 * functions to draw different hatch styles.
 */
#define HATCH_SIZE 7
#define LINE_WIDTH 1

GpStatus
draw_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/*
	 * The values used below are obtained by using hit and trial to get
	 * the results similar to that of MS. Not sure, if the results will
	 * be the same with different resolutions.
	 */
	if (hatchStyle == HatchStyleLightHorizontal)
		hatch_size *= 0.7; 	/* As per the docs lines should be 50% closer than */
					/* horizontal i.e. multiplication factor of 0.5    */
	else if (hatchStyle == HatchStyleNarrowHorizontal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 75% closer than */
					/* horizontal i.e. multiplication factor of 0.25   */
		line_width *= 1.5; 	/* docs say nothing about line width               */
	}
	else if (hatchStyle == HatchStyleDarkHorizontal) {
		hatch_size *= 0.6; 	/* As per the docs lines should be 50% closer than */
					/* horizontal i.e. multiplication factor of 0.5    */
		line_width *= 2.0; 	/* As per the docs line width should be twice the  */
					/* width of horizontal i.e. multiplication factor  */
					/* of 2.0                                          */
	}

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw horizontal line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);
		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_veritcal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/*
	 * The values used below are obtained by hit and trial to get the 
	 * results similar to that of MS.
	 */
	if (hatchStyle == HatchStyleLightVertical)
		hatch_size *= 0.7; 	/* As per the docs lines should be 50% closer than */
					/* vertical i.e. multiplication factor of 0.5      */
	else if (hatchStyle == HatchStyleNarrowVertical) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 75% closer than */
					/* vertical i.e. multiplication factor of 0.25     */
		line_width *= 1.5; 	/* docs say nothing about line width               */
	}
	else if (hatchStyle == HatchStyleDarkVertical) {
		hatch_size *= 0.6; 	/* As per the docs lines should be 50% closer than */
					/* vertical i.e. multiplication factor of 0.5      */
		line_width *= 2.0; 	/* As per the docs line width should be twice the  */
					/* width of vertical i.e. multiplication factor of */
					/* 2.0                                             */
	}

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw vertical line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);
		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_forward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/* start hatching from bottom left corner */
	double x1 = 0.0, y1 = height;
	double x2 = 0.0, y2 = height;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, width, height);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);
	
	/* draw hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, width, height);
		cairo_fill (ct);

		/* draw forward diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* FIXME:
		 * This way of drawing hatch is very slow, but its results are better
		 * than the other way of drawing hatch. We do not get proper results 
		 * here by using other way. See, draw_horizontal_hatch function, where
		 * we draw a very small hatch and we repeat it on a surface.
		 */
		while ((x2 < width) || (y1 > 0)) {
			cairo_move_to (ct, x1, y1);
			cairo_line_to (ct, x2, y2);
			x2 += hatch_size;
			y1 -= hatch_size;
		}

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_backward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/* start hatching from top left corner */
	double x1 = 0.0, y1 = 0.0;
	double x2 = 0.0, y2 = 0.0;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, width, height);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* draw hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, width, height);
		cairo_fill (ct);

		/* draw backward diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* FIXME:
		 * This way of drawing hatch is very slow, but its results are better
		 * than the other way of drawing hatch. We do not get proper results 
		 * here by using other way. See, draw_horizontal_hatch function, where
		 * we draw a very small hatch and we repeat it on a surface.
		 */
		while ((x1 < width) || (y2 < height)) {
			cairo_move_to (ct, x1, y1);
			cairo_line_to (ct, x2, y2);
			x1 += hatch_size;
			y2 += hatch_size;
		}

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_cross_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE + 2;
	double line_width = LINE_WIDTH;
	double dash [] = {1.0}; /* used for drawing dotted grid */

	if (hatchStyle == HatchStyleSmallGrid)
		hatch_size *= 0.6; /* small grid are placed 50% closer than cross */

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw lines in the foregound */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		if (hatchStyle == HatchStyleDottedGrid)
			cairo_set_dash (ct, dash, 1, 0);

		/* draw a horizontal line */
		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);

		/* draw a vertical line */
		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_diagonal_cross_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;
	double dash [] = {1.0}; /* used for drawing dotted diamond */

	/* FIXME:
	 * Lines drawn in case of OutlinedDiamond hatch style are not supposed 
	 * to be antialiased as per MS docs. That is the only difference between
	 * DiagonalCross and OutlinedDiamond style. Find a way to stop Cairo 
	 * antialiasing to conform to MS docs.
	 */

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		if (hatchStyle == HatchStyleDottedDiamond)
			cairo_set_dash (ct, dash, 1, 0);

		/* draw a forward diagonal line */
		cairo_move_to (ct, 0, 0);
		cairo_line_to (ct, hatch_size, hatch_size);

		/* draw a backward diagonal line */
		cairo_move_to (ct, hatch_size, 0);
		cairo_line_to (ct, 0, hatch_size);
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_05_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE + 2;
	double line_width = LINE_WIDTH;
	double rad;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((5.0 * hatch_size * hatch_size) / (2 * PI * 100.0));

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw four half circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* left middle */
		cairo_arc (ct, 0, hatch_size / 2.0, rad, - PI / 2.0, PI / 2.0);
		cairo_fill (ct);

		/* top middle */
		cairo_arc (ct, hatch_size / 2.0, 0, rad, 0, PI);
		cairo_fill (ct);

		/* bottom middle */
		cairo_arc (ct, hatch_size / 2.0, hatch_size, rad, - PI, 0);
		cairo_fill (ct);

		/* right middle */
		cairo_arc (ct, hatch_size, hatch_size / 2.0, rad, PI / 2.0, - PI / 2.0);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_10_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_wd = HATCH_SIZE + 2;
	double hatch_ht = 5.0;
	double line_width = LINE_WIDTH;
	double rad;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((10.0 * hatch_wd * hatch_ht) / (2 * PI * 100.0));

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw four half circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* left middle */
		cairo_arc (ct, 0, hatch_ht / 2.0, rad, - PI / 2.0, PI / 2.0);
		cairo_fill (ct);

		/* top middle */
		cairo_arc (ct, hatch_wd / 2.0, 0, rad, 0, PI);
		cairo_fill (ct);

		/* bottom middle */
		cairo_arc (ct, hatch_wd / 2.0, hatch_ht, rad, - PI, 0);
		cairo_fill (ct);

		/* right middle */
		cairo_arc (ct, hatch_wd, hatch_ht / 2.0, rad, PI / 2.0, - PI / 2.0);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_20_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = 5;
	double line_width = LINE_WIDTH;
	double rad = 0.5;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((20.0 * hatch_size * hatch_size) / (2 * PI * 100.0));

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw four half circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* left middle */
		cairo_arc (ct, 0, hatch_size / 2.0, rad, - PI / 2.0, PI / 2.0);
		cairo_fill (ct);

		/* top middle */
		cairo_arc (ct, hatch_size / 2.0, 0, rad, 0, PI);
		cairo_fill (ct);

		/* bottom middle */
		cairo_arc (ct, hatch_size / 2.0, hatch_size, rad, - PI, 0);
		cairo_fill (ct);

		/* right middle */
		cairo_arc (ct, hatch_size, hatch_size / 2.0, rad, PI / 2.0, - PI / 2.0);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_25_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_wd = 5;
	double hatch_ht = 3;
	double line_width = LINE_WIDTH;
	double rad;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((25.0 * hatch_wd * hatch_ht) / (2 * PI * 100.0));

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw four semi circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* left middle */
		cairo_arc (ct, 0, hatch_ht / 2.0, rad, - PI / 2.0, PI / 2.0);
		cairo_fill (ct);

		/* top middle */
		cairo_arc (ct, hatch_wd / 2.0, 0, rad, 0, PI);
		cairo_fill (ct);

		/* bottom middle */
		cairo_arc (ct, hatch_wd / 2.0, hatch_ht, rad, - PI, 0);
		cairo_fill (ct);

		/* right middle */
		cairo_arc (ct, hatch_wd, hatch_ht / 2.0, rad, PI / 2.0, - PI / 2.0);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_40_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch, *temp;
	double temp_size = 2;
	double hatch_wd = HATCH_SIZE + 1;
	double hatch_ht = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;
	double rad;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((10.0 * hatch_wd * hatch_ht) / (2 * PI * 100.0));

	cairo_surface_set_repeat (hatch, 1);

	/* draw a 10% hatch */
	{
		cairo_save (ct);

		temp = cairo_surface_create_similar (cairo_current_target_surface (ct),
						     format, temp_size, temp_size);
		
		g_return_val_if_fail (temp != NULL, OutOfMemory);
		
		cairo_surface_set_repeat (temp, 1);

		int Rb = (backcolor & 0x00FF0000) >> 16;
		int Gb = (backcolor & 0x0000FF00) >> 8;
		int Bb = (backcolor & 0x000000FF);
		
		/* draw a 50% hatch */
		{
			cairo_save (ct);
			
			/* hatch is not supposed to be affected by user matrix */
			cairo_identity_matrix (ct);
			cairo_set_target_surface (ct, temp);
			
			/* draw background */
			cairo_set_rgb_color (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);
			
			cairo_rectangle (ct, 0, 0, temp_size, temp_size);
			cairo_fill (ct);
			
			/* draw two diagonal lines in the foreground */
			int R = (forecolor & 0x00FF0000) >> 16;
			int G = (forecolor & 0x0000FF00) >> 8;
			int B = (forecolor & 0x000000FF);
			cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);
			
			cairo_rectangle (ct, 0, 0, temp_size / 2.0, temp_size / 2.0);
			cairo_fill (ct);
			
			cairo_rectangle (ct, temp_size / 2.0, temp_size / 2.0, temp_size, temp_size);
			cairo_fill (ct);

			cairo_restore (ct);
		}

		/* set the pattern for the consequent fill or stroke */
		gdip_cairo_set_surface_pattern (ct, temp);
		cairo_surface_destroy (temp);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		/* temp hatch is used as background, so we get 50% hatch */
		cairo_set_target_surface (ct, hatch);
		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* foreground is filled with 10% hatch of background color */
		cairo_set_rgb_color (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);

		/* 0.5 is added for pixel adjustment. It's ugly but it makes output better. */
		/* draw four semi circles in the foregound to make dots */
		/* left middle */
		cairo_arc (ct, 0, hatch_ht / 2.0 + 0.5, rad, - PI / 2.0, PI / 2.0);
		cairo_fill (ct);

		/* top middle */
		cairo_arc (ct, hatch_wd / 2.0 + 0.5, 0, rad, 0, PI);
		cairo_fill (ct);

		/* bottom middle */
		cairo_arc (ct, hatch_wd / 2.0 + 0.5, hatch_ht, rad, - PI, 0);
		cairo_fill (ct);

		/* right middle */
		cairo_arc (ct, hatch_wd, hatch_ht / 2.0 + 0.5, rad, PI / 2.0, - PI / 2.0);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);
	
	return Ok;
}

GpStatus
draw_50_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = 2;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw two rectangles in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size / 2.0, hatch_size / 2.0);
		cairo_fill (ct);

		cairo_rectangle (ct, hatch_size / 2.0, hatch_size / 2.0, hatch_size, hatch_size);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_60_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_wd = 4;
	double hatch_ht = 4;
	double line_width = LINE_WIDTH;
	int temp;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* we need to swap the colors */
	temp = forecolor;
	forecolor = backcolor;
	backcolor = temp;

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw two diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		cairo_move_to (ct, hatch_wd, 0);
		cairo_line_to (ct, 0, hatch_ht);

		cairo_move_to (ct, 0, hatch_ht / 4.0);
		cairo_line_to (ct, hatch_wd - hatch_wd / 4.0, hatch_ht);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_70_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_wd = 4;
	double hatch_ht = 4;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw two diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* near top left corner */
		cairo_move_to (ct, 0, 0);
		cairo_line_to (ct, hatch_wd, hatch_ht);

		/* near top right corner */
		cairo_move_to (ct, hatch_wd, 0);
		cairo_line_to (ct, 0, hatch_ht);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_downward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format, GpHatchStyle hatchStyle)
{
	/* FIXME:
	 * Lines drawn in this hatch are not supposed to be antialiased as per
	 * MS docs. Find a way to stop cairo antialiasing to conform to MS docs.
	 */
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/* start hatching from bottom left corner */
	double x1 = 0.0, y1 = height;
	double x2 = 0.0, y2 = height;

	/*
	 * The values used below are obtained by hit and trial to get the 
	 * results similar to that of MS.
	 */
	if (hatchStyle == HatchStyleLightDownwardDiagonal) {
		hatch_size *= 0.7; 	/* As per the docs lines should be 50% closer than */
					/* forward diagonal i.e. multiplication factor of  */
					/* 0.5                                             */
		line_width *= 0.7;
	}
	else if (hatchStyle == HatchStyleWideDownwardDiagonal)
		line_width *= 2.0; 	/* As per the docs lines should be spaced same as  */
					/* forward diagonal but line width is triple the   */
					/* width of forward diagonal i.e. multiplication   */
					/* factor of 3.0                                   */
	else if (hatchStyle == HatchStyleDarkDownwardDiagonal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 50% closer than */
					/* forward diagonal i.e. multiplication factor of  */
                                        /* 0.5                                             */
		/*line_width *= 2.0;*/ 	/* As per the docs line width should be twice the  */
					/* width of forward diagonal i.e. multiplication   */
					/* factor of 2.0                                   */
	}

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, width, height);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* draw hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, width, height);
		cairo_fill (ct);

		/* draw diagonal line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* FIXME:
		 * This way of drawing hatch is very slow, but its results are better
		 * than the other way of drawing hatch. We do not get proper results 
		 * here by using other way. See, draw_horizontal_hatch function, where
		 * we draw a very small hatch and we repeat it on a surface.
		 */
		while ((x2 < width) || (y1 > 0)) {
			cairo_move_to (ct, x1, y1);
			cairo_line_to (ct, x2, y2);
			x2 += hatch_size;
			y1 -= hatch_size;
		}

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format, GpHatchStyle hatchStyle)
{
	/* FIXME:
	 * Lines drawn in this hatch are not supposed to be antialiased as per
	 * MS docs. Find a way to stop cairo antialiasing to conform to MS docs.
	 */
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/* start hatching from top left corner */
	double x1 = 0.0, y1 = 0.0;
	double x2 = 0.0, y2 = 0.0;

	/*
	 * The values used below are obtained by hit and trial to get the 
	 * results similar to that of MS.
	 */
	if (hatchStyle == HatchStyleLightUpwardDiagonal) {
		hatch_size *= 0.7; 	/* As per the docs lines should be 50% closer than */
					/* backward diagonal i.e. multiplication factor of */
					/* 0.5                                             */
		line_width *= 0.7;
	}
	else if (hatchStyle == HatchStyleWideUpwardDiagonal)
		line_width *= 2.0; 	/* As per the docs lines should be spaced same as  */
					/* backward diagonal but line width is triple the  */
					/* width of backward diagonal i.e. multiplication  */
					/* factor of 3.0                                   */
	else if (hatchStyle == HatchStyleDarkUpwardDiagonal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 50% closer than */
					/* backward diagonal i.e. multiplication factor of */
                                        /* 0.5                                             */
		/*line_width *= 2.0;*/ 	/* As per the docs line width should be twice the  */
					/* width of backward diagonal i.e. multiplication  */
					/* factor of 2.0                                   */
	}

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, width, height);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);
	
	/* draw hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, width, height);
		cairo_fill (ct);

		/* draw diagonal line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* FIXME:
		 * This way of drawing hatch is very slow, but its results are better
		 * than the other way of drawing hatch. We do not get proper results 
		 * here by using other way. See, draw_horizontal_hatch function, where
		 * we draw a very small hatch and we repeat it on a surface.
		 */
		while ((x1 < width) || (y2 < height)) {
			cairo_move_to (ct, x1, y1);
			cairo_line_to (ct, x2, y2);
			x1 += hatch_size;
			y2 += hatch_size;
		}

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_dashed_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double space = 6;
	double hatch_wd = 4;
	double hatch_ht = hatch_wd + space;

	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch that has a 45 deg line*/
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw slant line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		if (hatchStyle == HatchStyleDashedUpwardDiagonal) {
			/* 45 deg slant line */
			cairo_move_to (ct, hatch_wd, 0);
			cairo_line_to (ct, 0, hatch_wd);
		} else { // HatchStyleDashedDownwardDiagonal
			/* -45 deg slant line */
			cairo_move_to (ct, 0, 0);
			cairo_line_to (ct, hatch_wd, hatch_wd);
		}

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_dashed_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double dash_len = 5;
	double line_space = 4;
	double hatch_wd = (2 * dash_len) - 1;
	double hatch_ht = 2 * line_space;

	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch that has two dashes near upper left and lower right corners */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw horizontal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* upper left dash */
		cairo_move_to (ct, 0, line_space / 2);
		cairo_line_to (ct, dash_len, line_space / 2);

		/* lower right dash */
		cairo_move_to (ct, hatch_wd - dash_len, hatch_ht - line_space / 2);
		cairo_line_to (ct, hatch_wd, hatch_ht - line_space / 2);
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_dashed_vertical_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double dash_len = 5;
	double line_space = 4;
	double hatch_ht = (2 * dash_len) - 1;
	double hatch_wd = 2 * line_space;

	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch that has two dashes near upper left and lower right corners */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw vertical lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* upper left dash */
		cairo_move_to (ct, line_space / 2, 0);
		cairo_line_to (ct, line_space / 2, dash_len);

		/* lower right dash */
		cairo_move_to (ct, hatch_wd - line_space / 2, hatch_ht - dash_len);
		cairo_line_to (ct, hatch_wd - line_space / 2, hatch_ht);
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_small_confetti_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_large_confetti_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_zigzag_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_wave_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_diagonal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	return NotImplemented;
}

GpStatus
draw_horizontal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_wd = HATCH_SIZE + 2;
	double hatch_ht = HATCH_SIZE + 2;

	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* set foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* draw a reversed 'C' shape */
		cairo_move_to (ct, 0, 0);
		cairo_line_to (ct, hatch_wd, 0);
		cairo_line_to (ct, hatch_wd, hatch_ht / 2.0);
		cairo_line_to (ct, 0, hatch_ht / 2.0);
		/* draw lower connector */
		cairo_move_to (ct, hatch_wd / 2.0, hatch_ht / 2.0);
		cairo_line_to (ct, hatch_wd / 2.0, hatch_ht);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_weave_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_plaid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_divot_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_solid_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw two triangles in the foreground with left and right sides as their bases */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* draw left triangle */
		cairo_move_to (ct, 0, 0.5);
		cairo_line_to (ct, hatch_size / 2.0 - 0.5, hatch_size / 2.0);
		cairo_line_to (ct, 0, hatch_size - 0.5);
		cairo_line_to (ct, 0, 0.5);
		cairo_fill (ct);

		/* draw right triangle */
		cairo_move_to (ct, hatch_size, 0.5);
		cairo_line_to (ct, hatch_size / 2.0 + 0.5, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size - 0.5);
		cairo_line_to (ct, hatch_size, 0.5);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_shingle_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_trellis_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_sphere_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = 12;
	double line_width = LINE_WIDTH;
	double rad = hatch_size / 4.0 + 0.5;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch that has two colored squares at upper left and lower right corners */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int Rb = (backcolor & 0x00FF0000) >> 16;
		int Gb = (backcolor & 0x0000FF00) >> 8;
		int Bb = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw half circles in the foreground */
		int Rf = (forecolor & 0x00FF0000) >> 16;
		int Gf = (forecolor & 0x0000FF00) >> 8;
		int Bf = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) Rf / 255.0, (double) Gf / 255.0, (double) Bf / 255.0);

		/* left middle */
		cairo_arc (ct, 0, hatch_size / 2.0, rad, - PI / 2.0, PI / 2.0);
		cairo_fill (ct);

		/* top middle */
		cairo_arc (ct, hatch_size / 2.0, 0, rad, 0, PI);
		cairo_fill (ct);

		/* bottom middle */
		cairo_arc (ct, hatch_size / 2.0, hatch_size, rad, - PI, 0);
		cairo_fill (ct);

		/* right middle */
		cairo_arc (ct, hatch_size, hatch_size / 2.0, rad, PI / 2.0, - PI / 2.0);
		cairo_fill (ct);

		/* draw lines to mark sphere shining */
		cairo_set_rgb_color (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);
		cairo_set_line_width (ct, line_width);

		/* left middle */
		cairo_move_to (ct, 0, hatch_size / 2.0 - rad / 2.0);
		cairo_line_to (ct, rad / 2.0, hatch_size / 2.0 - rad / 2.0);

		/* right middle */
		cairo_move_to (ct, hatch_size, hatch_size / 2.0 - rad / 2.0);
		cairo_line_to (ct, hatch_size - rad / 2.0, hatch_size / 2.0 - rad / 2.0);

		/* bottom middle */
		cairo_move_to (ct, hatch_size / 2.0 - rad / 2.0, hatch_size - rad / 2.0);
		cairo_line_to (ct, hatch_size / 2.0 + rad / 2.0, hatch_size - rad / 2.0);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_checker_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double hatch_size = 5.0;

	if (hatchStyle == HatchStyleLargeCheckerBoard)
		hatch_size *= 2.0;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch that has two colored squares at upper left and lower right corners */
	{
		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		cairo_set_target_surface (ct, hatch);

		/* draw background */
		int R = (backcolor & 0x00FF0000) >> 16;
		int G = (backcolor & 0x0000FF00) >> 8;
		int B = (backcolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw squares in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_rgb_color (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		/* upper left square */
		cairo_rectangle (ct, 0, 0, hatch_size / 2.0, hatch_size / 2.0);

		/* lower right square */
		cairo_rectangle (ct, hatch_size / 2.0, hatch_size / 2.0, hatch_size, hatch_size);
		cairo_fill (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        gdip_cairo_set_surface_pattern (ct, hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
gdip_hatch_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpHatch *hatch;
	GpImage *img;
	GpBitmap *bmp;
	cairo_t *ct;
	unsigned int width;
	unsigned int height;
	int forecol;
	int backcol;
	cairo_format_t format;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	img = graphics->image;

	g_return_val_if_fail (img != NULL, InvalidParameter);

	hatch = (GpHatch *) brush;
	forecol = hatch->foreColor;
	backcol = hatch->backColor;

	if (img->type == imageBitmap) {
		bmp = (GpBitmap *) img;
		width = bmp->data.Width;
		height = bmp->data.Height;
		format = bmp->data.PixelFormat;
	} else 
		return NotImplemented;

	ct = graphics->ct;

	g_return_val_if_fail (ct != NULL, InvalidParameter);

	switch (hatch->hatchStyle) {

	/* case HatchStyleMin: */
	case HatchStyleHorizontal:
	case HatchStyleLightHorizontal:
	case HatchStyleNarrowHorizontal:
	case HatchStyleDarkHorizontal:
		return draw_horizontal_hatch (ct, forecol, backcol, format, hatch->hatchStyle);

	case HatchStyleVertical:
	case HatchStyleLightVertical:
	case HatchStyleNarrowVertical:
	case HatchStyleDarkVertical:
		return draw_veritcal_hatch (ct, forecol, backcol, format, hatch->hatchStyle);

	case HatchStyleForwardDiagonal:
		return draw_forward_diagonal_hatch (ct, forecol, backcol, width, height, format);

	case HatchStyleBackwardDiagonal:
		return draw_backward_diagonal_hatch (ct, forecol, backcol, width, height, format);

	/* case HatchStyleCross: */
	case HatchStyleLargeGrid:
	case HatchStyleSmallGrid:
	case HatchStyleDottedGrid:
		return draw_cross_hatch (ct, forecol, backcol, format, hatch->hatchStyle);

	case HatchStyleDiagonalCross:
	case HatchStyleDottedDiamond:
	case HatchStyleOutlinedDiamond:
		return draw_diagonal_cross_hatch (ct, forecol, backcol, format, hatch->hatchStyle);

	case HatchStyle05Percent:
		return draw_05_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle10Percent:
		return draw_10_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle20Percent:
		return draw_20_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle25Percent:
		return draw_25_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle30Percent:
		return draw_70_percent_hatch (ct, backcol, forecol, format);

	case HatchStyle40Percent:
		return draw_40_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle50Percent:
		return draw_50_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle60Percent:
		return draw_60_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle70Percent:
		return draw_70_percent_hatch (ct, forecol, backcol, format);

	case HatchStyle75Percent:
		return draw_25_percent_hatch (ct, backcol, forecol, format);

	case HatchStyle80Percent:
		return draw_20_percent_hatch (ct, backcol, forecol, format);

	case HatchStyle90Percent:
		return draw_10_percent_hatch (ct, backcol, forecol, format);

	case HatchStyleLightDownwardDiagonal:
	case HatchStyleDarkDownwardDiagonal:
	case HatchStyleWideDownwardDiagonal:
		return draw_downward_diagonal_hatch (ct, forecol, backcol, width, height, format, hatch->hatchStyle);

	case HatchStyleLightUpwardDiagonal:
	case HatchStyleDarkUpwardDiagonal:
	case HatchStyleWideUpwardDiagonal:
		return draw_upward_diagonal_hatch (ct, forecol, backcol, width, height, format, hatch->hatchStyle);

	case HatchStyleDashedDownwardDiagonal:
	case HatchStyleDashedUpwardDiagonal:
		return draw_dashed_diagonal_hatch (ct, forecol, backcol, format, hatch->hatchStyle);

	case HatchStyleDashedHorizontal:
		return draw_dashed_horizontal_hatch (ct, forecol, backcol, format);

	case HatchStyleDashedVertical:
		return draw_dashed_vertical_hatch (ct, forecol, backcol, format);

	case HatchStyleSmallConfetti:
		return draw_small_confetti_hatch (ct, forecol, backcol, width, height);

	case HatchStyleLargeConfetti:
		return draw_large_confetti_hatch (ct, forecol, backcol, width, height);

	case HatchStyleZigZag:
		return draw_zigzag_hatch (ct, forecol, backcol, width, height);

	case HatchStyleWave:
		return draw_wave_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDiagonalBrick:
		return draw_diagonal_brick_hatch (ct, forecol, backcol, format);

	case HatchStyleHorizontalBrick:
		return draw_horizontal_brick_hatch (ct, forecol, backcol, format);

	case HatchStyleWeave:
		return draw_weave_hatch (ct, forecol, backcol, width, height);

	case HatchStylePlaid:
		return draw_plaid_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDivot:
		return draw_divot_hatch (ct, forecol, backcol, width, height);

	case HatchStyleShingle:
		return draw_shingle_hatch (ct, forecol, backcol, width, height);

	case HatchStyleTrellis:
		return draw_trellis_hatch (ct, forecol, backcol, width, height);

	case HatchStyleSphere:
	  return draw_sphere_hatch (ct, forecol, backcol, format);

	case HatchStyleSmallCheckerBoard:
	case HatchStyleLargeCheckerBoard:
		return draw_checker_hatch (ct, forecol, backcol, format, hatch->hatchStyle);

	/* case HatchStyleMax: */
	case HatchStyleSolidDiamond:
		return draw_solid_diamond_hatch (ct, forecol, backcol, format);

	default:
		return InvalidParameter;
	}
}

GpStatus
gdip_hatch_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpHatch *result;
	GpHatch *hatch;

	g_return_val_if_fail (brush != NULL, InvalidParameter);

	hatch = (GpHatch *) brush;
	result = (GpHatch *) GdipAlloc (sizeof (GpHatch));
	
	g_return_val_if_fail (result != NULL, OutOfMemory);

	result->base = hatch->base;
	result->hatchStyle = hatch->hatchStyle;
	result->foreColor = hatch->foreColor;
	result->backColor = hatch->backColor;

	*clonedBrush = (GpBrush *) result;

        return Ok;
}

GpStatus
gdip_hatch_destroy (GpBrush *brush)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	GdipFree (brush);

	return Ok;
}

GpStatus
GdipCreateHatchBrush (GpHatchStyle hatchstyle, int forecolor, int backcolor, GpHatch **brush)
{
        *brush = gdip_hatch_new ();

	g_return_val_if_fail (brush != NULL, OutOfMemory);

        (*brush)->hatchStyle = hatchstyle;
        (*brush)->foreColor = forecolor;
        (*brush)->backColor = backcolor;

        return Ok; 
}

GpStatus
GdipGetHatchStyle (GpHatch *brush, GpHatchStyle *hatchstyle)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	*hatchstyle = brush->hatchStyle;
	return Ok;
}

GpStatus
GdipGetHatchForegroundColor (GpHatch *brush, int *forecolor)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	*forecolor = brush->foreColor;
	return Ok;
}

GpStatus
GdipGetHatchBackgroundColor (GpHatch *brush, int *backcolor)
{
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	*backcolor = brush->backColor;
	return Ok;
}
