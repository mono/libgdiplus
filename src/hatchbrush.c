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
	hatch->pattern = NULL;
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
draw_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/*
	 * The values used below are obtained by using hit and trial to get
	 * the results similar to that of MS. Not sure, if the results will
	 * be the same with different resolutions.
	 */
	if (hbr->hatchStyle == HatchStyleLightHorizontal)
		hatch_size *= 0.7; 	/* As per the docs lines should be 50% closer than */
					/* horizontal i.e. multiplication factor of 0.5    */
	else if (hbr->hatchStyle == HatchStyleNarrowHorizontal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 75% closer than */
					/* horizontal i.e. multiplication factor of 0.25   */
		line_width *= 1.5; 	/* docs say nothing about line width               */
	}
	else if (hbr->hatchStyle == HatchStyleDarkHorizontal) {
		hatch_size *= 0.6; 	/* As per the docs lines should be 50% closer than */
					/* horizontal i.e. multiplication factor of 0.5    */
		line_width *= 2.0; 	/* As per the docs line width should be twice the  */
					/* width of horizontal i.e. multiplication factor  */
					/* of 2.0                                          */
	}

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw horizontal line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);
		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);
		cairo_stroke (ct);
		cairo_paint (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);

	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_veritcal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/*
	 * The values used below are obtained by hit and trial to get the 
	 * results similar to that of MS.
	 */
	if (hbr->hatchStyle == HatchStyleLightVertical)
		hatch_size *= 0.7; 	/* As per the docs lines should be 50% closer than */
					/* vertical i.e. multiplication factor of 0.5      */
	else if (hbr->hatchStyle == HatchStyleNarrowVertical) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 75% closer than */
					/* vertical i.e. multiplication factor of 0.25     */
		line_width *= 1.5; 	/* docs say nothing about line width               */
	}
	else if (hbr->hatchStyle == HatchStyleDarkVertical) {
		hatch_size *= 0.6; 	/* As per the docs lines should be 50% closer than */
					/* vertical i.e. multiplication factor of 0.5      */
		line_width *= 2.0; 	/* As per the docs line width should be twice the  */
					/* width of vertical i.e. multiplication factor of */
					/* 2.0                                             */
	}

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
                ct = cairo_create (hatch);		

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw vertical line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);
		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);
		cairo_stroke (ct);
		cairo_paint (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_forward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	
	
	/* draw hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
                ct = cairo_create (hatch);		

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw forward diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);
 
		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);
 
		cairo_stroke (ct);
		cairo_paint (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_backward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	
	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
                ct = cairo_create (hatch);
		

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw backward diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, 0, hatch_size / 2.0);

		cairo_move_to (ct, hatch_size, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);

		cairo_stroke (ct);
		cairo_paint (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_cross_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 2;
	double line_width = LINE_WIDTH;
	double dash [] = {1.0}; /* used for drawing dotted grid */

	if (hbr->hatchStyle == HatchStyleSmallGrid)
		hatch_size *= 0.6; /* small grid are placed 50% closer than cross */

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
                ct = cairo_create (hatch);		

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw lines in the foregound */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		if (hbr->hatchStyle == HatchStyleDottedGrid)
			cairo_set_dash (ct, dash, 1, 0);

		/* draw a horizontal line */
		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);

		/* draw a vertical line */
		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);

		cairo_stroke (ct);
		cairo_paint (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_diagonal_cross_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;
	double dash [] = {1.0}; /* used for drawing dotted diamond */

	/* FIXME:
	 * Lines drawn in case of OutlinedDiamond hatch style are not supposed 
	 * to be antialiased as per MS docs. That is the only difference between
	 * DiagonalCross and OutlinedDiamond style. Find a way to stop Cairo 
	 * antialiasing to conform to MS docs.
	 */

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
                ct = cairo_create (hatch);		

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		if (hbr->hatchStyle == HatchStyleDottedDiamond)
			cairo_set_dash (ct, dash, 1, 0);

		/* draw a forward diagonal line */
		cairo_move_to (ct, 0, 0);
		cairo_line_to (ct, hatch_size, hatch_size);

		/* draw a backward diagonal line */
		cairo_move_to (ct, hatch_size, 0);
		cairo_line_to (ct, 0, hatch_size);
		cairo_stroke (ct);
		cairo_paint (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_05_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 2;
	double rad;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((5.0 * hatch_size * hatch_size) / (2 * PI * 100.0));

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw four half circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

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
		
		cairo_paint (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_10_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_wd = HATCH_SIZE + 2;
	double hatch_ht = 5.0;
	double rad;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((10.0 * hatch_wd * hatch_ht) / (2 * PI * 100.0));

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw four half circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

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

		cairo_paint (ct);
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_20_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = 5;
	double rad = 0.5;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((20.0 * hatch_size * hatch_size) / (2 * PI * 100.0));

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw four half circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

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

		cairo_paint (ct);
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_25_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_wd = 5;
	double hatch_ht = 3;
	double rad;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((25.0 * hatch_wd * hatch_ht) / (2 * PI * 100.0));

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw four semi circles in the foreground to make dots */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

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

		cairo_paint (ct);
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_40_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch, *temp;
	cairo_pattern_t *pattern;
	double temp_size = 2;
	double hatch_wd = HATCH_SIZE + 1;
	double hatch_ht = HATCH_SIZE + 1;
	double rad;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* calculate radius using, pi * square (r) = % of hatch area */
	rad = sqrt ((10.0 * hatch_wd * hatch_ht) / (2 * PI * 100.0));

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	

	/* draw a 10% hatch */
	{
		int Rb,Gb,Bb;

		cairo_save (ct);

		temp = cairo_surface_create_similar (cairo_get_target (ct),
						     format, temp_size, temp_size);
		
		if (temp == NULL) {
			cairo_surface_destroy (hatch);
			return OutOfMemory;
		}

		pattern = cairo_get_source (ct);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);

		Rb = (backcolor & 0x00FF0000) >> 16;
		Gb = (backcolor & 0x0000FF00) >> 8;
		Bb = (backcolor & 0x000000FF);
		
		/* draw a 50% hatch */
		{
			int R,G,B;

			cairo_save (ct);
			
			/* hatch is not supposed to be affected by user matrix */
			cairo_identity_matrix (ct);
                        ct = cairo_create (temp);			
			
			/* draw background */
			cairo_set_source_rgb (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);
			
			cairo_rectangle (ct, 0, 0, temp_size, temp_size);
			cairo_fill (ct);
			
			/* draw two diagonal lines in the foreground */
			R = (forecolor & 0x00FF0000) >> 16;
			G = (forecolor & 0x0000FF00) >> 8;
			B = (forecolor & 0x000000FF);
			cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

			cairo_rectangle (ct, 0, 0, temp_size / 2.0, temp_size / 2.0);
			cairo_rectangle (ct, temp_size / 2.0, temp_size / 2.0, temp_size, temp_size);
			cairo_fill (ct);

			cairo_restore (ct);
		} /* 50% hatch*/

		/* set the pattern for the consequent fill or stroke */
		gdip_cairo_set_surface_pattern_with_extend (ct, temp, CAIRO_EXTEND_REPEAT);
		cairo_surface_destroy (temp);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		/* temp hatch is used as background, so we get 50% hatch */
		ct = cairo_create (hatch);
		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* foreground is filled with 10% hatch of background color */
		cairo_set_source_rgb (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);

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

		cairo_paint (ct);		
		
		cairo_restore (ct);
	} /* 10% hatch */

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);
	
	return Ok;
}

GpStatus
draw_50_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch;
	double hatch_size = 2;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw two rectangles in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size / 2.0, hatch_size / 2.0);
		cairo_rectangle (ct, hatch_size / 2.0, hatch_size / 2.0, hatch_size, hatch_size);
		cairo_fill (ct);		
		cairo_restore (ct);		
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_pattern_set_extend (hbr->pattern, CAIRO_EXTEND_REPEAT);	
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_60_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_wd = 4;
	double hatch_ht = 4;
	double line_width = LINE_WIDTH;
	int temp;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* we need to swap the colors */
	temp = forecolor;
	forecolor = backcolor;
	backcolor = temp;

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw two diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		cairo_move_to (ct, hatch_wd, 0);
		cairo_line_to (ct, 0, hatch_ht);

		cairo_move_to (ct, 0, hatch_ht / 4.0);
		cairo_line_to (ct, hatch_wd - hatch_wd / 4.0, hatch_ht);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_70_percent_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_wd = 4;
	double hatch_ht = 4;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw two diagonal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

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
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_downward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	/* FIXME:
	 * Lines drawn in this hatch are not supposed to be antialiased as per
	 * MS docs. Find a way to stop cairo antialiasing to conform to MS docs.
	 */
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;

	/*
	 * The values used below are obtained by hit and trial to get the 
	 * results similar to that of MS.
	 */
	if (hbr->hatchStyle == HatchStyleLightDownwardDiagonal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 50% closer than */
					/* forward diagonal i.e. multiplication factor of  */
					/* 0.5                                             */
		line_width *= 0.7;
	}
	else if (hbr->hatchStyle == HatchStyleWideDownwardDiagonal)
		line_width *= 2.0; 	/* As per the docs lines should be spaced same as  */
					/* forward diagonal but line width is triple the   */
					/* width of forward diagonal i.e. multiplication   */
					/* factor of 3.0                                   */
	else if (hbr->hatchStyle == HatchStyleDarkDownwardDiagonal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 50% closer than */
					/* forward diagonal i.e. multiplication factor of  */
                                        /* 0.5                                             */
		line_width *= 1.5; 	/* As per the docs line width should be twice the  */
					/* width of forward diagonal i.e. multiplication   */
					/* factor of 2.0                                   */
	}

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw diagonal line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);
 
		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	/* FIXME:
	 * Lines drawn in this hatch are not supposed to be antialiased as per
	 * MS docs. Find a way to stop cairo antialiasing to conform to MS docs.
	 */
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;

	/*
	 * The values used below are obtained by hit and trial to get the 
	 * results similar to that of MS.
	 */
	if (hbr->hatchStyle == HatchStyleLightUpwardDiagonal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 50% closer than */
					/* backward diagonal i.e. multiplication factor of */
					/* 0.5                                             */
		line_width *= 0.7;
	}
	else if (hbr->hatchStyle == HatchStyleWideUpwardDiagonal)
		line_width *= 2.0; 	/* As per the docs lines should be spaced same as  */
					/* backward diagonal but line width is triple the  */
					/* width of backward diagonal i.e. multiplication  */
					/* factor of 3.0                                   */
	else if (hbr->hatchStyle == HatchStyleDarkUpwardDiagonal) {
		hatch_size *= 0.5; 	/* As per the docs lines should be 50% closer than */
					/* backward diagonal i.e. multiplication factor of */
                                        /* 0.5                                             */
		line_width *= 1.5; 	/* As per the docs line width should be twice the  */
					/* width of backward diagonal i.e. multiplication  */
					/* factor of 2.0                                   */
	}

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw diagonal line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		cairo_move_to (ct, hatch_size / 2.0, 0);
		cairo_line_to (ct, 0, hatch_size / 2.0);

		cairo_move_to (ct, hatch_size, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_dashed_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double space = 6;
	double hatch_wd = 4;
	double hatch_ht = hatch_wd + space;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch that has a 45 deg line*/
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw slant line in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		if (hbr->hatchStyle == HatchStyleDashedUpwardDiagonal) {
			/* 45 deg slant line */
			cairo_move_to (ct, hatch_wd, 0);
			cairo_line_to (ct, 0, hatch_wd);
		} else { /* HatchStyleDashedDownwardDiagonal */
			/* -45 deg slant line */
			cairo_move_to (ct, 0, 0);
			cairo_line_to (ct, hatch_wd, hatch_wd);
		}

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_dashed_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double dash_len = 5;
	double line_space = 4;
	double hatch_wd = (2 * dash_len) - 1;
	double hatch_ht = 2 * line_space;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch that has two dashes near upper left and lower right corners */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw horizontal lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

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
        hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_dashed_vertical_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double dash_len = 5;
	double line_space = 4;
	double hatch_ht = (2 * dash_len) - 1;
	double hatch_wd = 2 * line_space;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch that has two dashes near upper left and lower right corners */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* draw vertical lines in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

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
        hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_confetti_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double confetti_size = 1.0;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	if (hbr->hatchStyle == HatchStyleLargeConfetti)
		confetti_size *= 1.5;

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw small rectangles in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		/* draw four rectangles on the left reversed 'L' shape */
		cairo_rectangle (ct, 0, 0, confetti_size, confetti_size);
		cairo_rectangle (ct, hatch_size / 8.0, hatch_size / 4.0, confetti_size, confetti_size);
		cairo_rectangle (ct, 3 * hatch_size / 8.0, hatch_size / 2.0, confetti_size, confetti_size);
		cairo_rectangle (ct, hatch_size / 4.0, 3 * hatch_size / 4.0, confetti_size, confetti_size);

		/* draw four rectangles on the other left reversed 'L' shape */
		cairo_rectangle (ct, hatch_size / 2.0, hatch_size / 8.0, confetti_size, confetti_size);
		cairo_rectangle (ct, 3 * hatch_size / 4.0, 3 * hatch_size / 8.0, confetti_size, confetti_size);
		cairo_rectangle (ct, 7 * hatch_size / 8.0, 5 * hatch_size / 8.0, confetti_size, confetti_size);
		cairo_rectangle (ct, 5 * hatch_size / 8.0, 7 * hatch_size / 8.0, confetti_size, confetti_size);
		cairo_fill (ct);

		cairo_paint (ct);		
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_zigzag_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;
	double dash [] = {0.5};

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw two 'V' shapes in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);
		cairo_set_dash (ct, dash, 1, 0);
		cairo_set_line_width (ct, line_width);

		/* upper 'V' shape */
		cairo_move_to (ct, 0, 0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, 0);

		/* lower 'V' shape */
		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);
		
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_wave_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_wd = HATCH_SIZE + 2;
	double hatch_ht = hatch_wd / 2.0;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_wd, hatch_ht);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_wd, hatch_ht);
		cairo_fill (ct);

		/* set foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* draw a horizontal 'S' shape */
		cairo_move_to (ct, 1.0, 1.0);
		cairo_curve_to (ct, 1.0, 1.0, hatch_wd / 4.0, hatch_ht, hatch_wd / 2.0, hatch_ht / 2.0);
		cairo_curve_to (ct, hatch_wd / 2.0, hatch_ht / 2.0, 3 * hatch_wd / 4.0, 0, hatch_wd - 1.0, hatch_ht - 1.0);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_diagonal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 2;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* set foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);
		cairo_set_line_width (ct, line_width);

		/* draw one full diagonal line and half of another diagonal line */
		cairo_move_to (ct, 0, hatch_size);
		cairo_line_to (ct, hatch_size, 0);
		cairo_move_to (ct, hatch_size / 2.0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size);
		
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_horizontal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 2;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* set foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* draw a 'C' shape */
		cairo_move_to (ct, hatch_size, 0);
		cairo_line_to (ct, 0, 0);
		cairo_line_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);

		/* draw lower connector */
		cairo_move_to (ct, hatch_size / 2.0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_weave_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;
	double dash [] = {0.5};

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* set foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);
		cairo_set_dash (ct, dash, 1, 0);

		/* draw a reversed 'C' shape diagonally with one connector towards
		 * one of the other two vertices of the square and three connectors
		 * towards the fourth vertex.
		 */
		cairo_move_to (ct, 0, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size / 2.0, 0);
		/* this makes up one of the lower connectors also */
		cairo_line_to (ct, hatch_size, hatch_size / 2.0);
		/* complete the diagonal 'C' shape */
		cairo_move_to (ct, 3 * hatch_size / 4.0, hatch_size / 4.0);
		cairo_line_to (ct, 0, hatch_size);

		/* draw one upper connector */
		cairo_move_to (ct, 0, 0);
		cairo_line_to (ct, hatch_size / 4.0, hatch_size / 4.0);

		/* draw lower connectors */
		cairo_move_to (ct, hatch_size / 4.0, 3 * hatch_size / 4.0);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size);

		cairo_move_to (ct, hatch_size, hatch_size);
		cairo_line_to (ct, hatch_size / 2.0, hatch_size / 2.0);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_plaid_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch, *temp;
	cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double temp_size = 2;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	

	/* draw one hatch that has 50% of area as 50% hatch and remaining area
	 * equally covered by each of foreground and background colors.
	 */
	{
		int Rb,Gb,Bb;
		int Rf,Gf,Bf;

		cairo_save (ct);

		temp = cairo_surface_create_similar (cairo_get_target (ct),
						     format, temp_size, temp_size);
		
		if (temp == NULL) {
		  cairo_surface_destroy (hatch);
		  return OutOfMemory;
		}

		pattern = cairo_get_source (ct);
		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		

		Rb = (backcolor & 0x00FF0000) >> 16;
		Gb = (backcolor & 0x0000FF00) >> 8;
		Bb = (backcolor & 0x000000FF);

		Rf = (forecolor & 0x00FF0000) >> 16;
		Gf = (forecolor & 0x0000FF00) >> 8;
		Bf = (forecolor & 0x000000FF);
		
		/* draw a 50% hatch */
		{
			cairo_save (ct);
			
			/* hatch is not supposed to be affected by user matrix */
			cairo_identity_matrix (ct);
			ct = cairo_create (temp);
			
			/* draw background */
			cairo_set_source_rgb (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);
			
			cairo_rectangle (ct, 0, 0, temp_size, temp_size);
			cairo_fill (ct);
			
			/* draw two diagonal lines in the foreground */
			cairo_set_source_rgb (ct, (double) Rf / 255.0, (double) Gf / 255.0, (double) Bf / 255.0);
			
			cairo_rectangle (ct, 0, 0, temp_size / 2.0, temp_size / 2.0);
			cairo_rectangle (ct, temp_size / 2.0, temp_size / 2.0, temp_size, temp_size);
			cairo_fill (ct);

			cairo_restore (ct);
		} /* 50% hatch*/

		/* set the pattern for the consequent fill or stroke */
		gdip_cairo_set_surface_pattern_with_extend (ct, temp, CAIRO_EXTEND_REPEAT);
		cairo_surface_destroy (temp);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		/* temp hatch is used to fill 50% of area */
		ct = cairo_create (hatch);
		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size / 2.0);
		cairo_fill (ct);

		/* draw two rectangle in the foregound to make up 50% of plaid */
		/* foreground is filled with 25% of background color and 25% of foreground color */
		cairo_set_source_rgb (ct, (double) Rf / 255.0, (double) Gf / 255.0, (double) Bf / 255.0);
		cairo_rectangle (ct, 0, hatch_size / 2.0, hatch_size / 2.0, hatch_size / 2.0);
		cairo_fill (ct);

		cairo_set_source_rgb (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);
		cairo_rectangle (ct, hatch_size / 2.0, hatch_size / 2.0, hatch_size / 2.0, hatch_size / 2.0);
		cairo_fill (ct);

		cairo_paint (ct);
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);
	
	return Ok;
}

GpStatus
draw_divot_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = 10.0;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	
	
	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* set foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_set_line_width (ct, line_width);

		/* draw two angle brackets in the foreground */
		cairo_move_to (ct, hatch_size / 8.0, hatch_size / 8.0);
		cairo_line_to (ct, 3 * hatch_size / 8.0, hatch_size / 4.0);
		cairo_line_to (ct, hatch_size / 8.0, 3 * hatch_size / 8.0);

		cairo_move_to (ct, 7 * hatch_size / 8.0, 7 * hatch_size / 8.0);
		cairo_line_to (ct, 5 * hatch_size / 8.0, 3 * hatch_size / 4.0);
		cairo_line_to (ct, 7 * hatch_size / 8.0, 5 * hatch_size / 8.0);

		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_solid_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw two triangles in the foreground with left and right sides as their bases */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		/* draw left triangle */
		cairo_move_to (ct, 0, 0.5);
		cairo_line_to (ct, hatch_size / 2.0 - 0.5, hatch_size / 2.0);
		cairo_line_to (ct, 0, hatch_size - 0.5);
		cairo_close_path (ct);
		cairo_fill (ct);

		/* draw right triangle */
		cairo_move_to (ct, hatch_size, 0.5);
		cairo_line_to (ct, hatch_size / 2.0 + 0.5, hatch_size / 2.0);
		cairo_line_to (ct, hatch_size, hatch_size - 0.5);
		cairo_close_path (ct);
		cairo_fill (ct);

		cairo_paint (ct);
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_shingle_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = HATCH_SIZE + 1;
	double line_width = LINE_WIDTH;
	double dash [] = {1.0};

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	

	/* draw one hatch */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw two curves in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);
		cairo_set_dash (ct, dash, 1, 0);
		cairo_set_line_width (ct, line_width);

		/* draw a curve thru left top, center and right top corners */
		cairo_move_to (ct, 0, 0);
		cairo_line_to (ct, hatch_size / 4.0, hatch_size / 4.0);
		cairo_curve_to (ct, hatch_size / 4.0, hatch_size / 4.0, hatch_size / 2.0, hatch_size / 2.0, 3 * hatch_size / 4.0, hatch_size / 4.0);
		cairo_line_to (ct, hatch_size, 0);

		/* draw a quarter circle thru center and right bottom corner */
		cairo_arc (ct, 0, hatch_size, hatch_size, -PI/4.0, 0);
		
		cairo_stroke (ct);

		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_trellis_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = 4;
	int temp;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	/* swap the colors */
	temp = backcolor;
	backcolor = forecolor;
	forecolor = temp;

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	
	
	/* draw one hatch that has two colored squares at upper left and lower right corners */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw rectangles in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		/* upper left rectangle */
		cairo_rectangle (ct, 0, hatch_size / 4.0, hatch_size / 2.0, hatch_size / 4.0);

		/* lower right rectangle */
		cairo_rectangle (ct, hatch_size / 2.0, 3 * hatch_size / 4.0, hatch_size / 2.0, hatch_size / 4.0);
		cairo_fill (ct);

		cairo_paint (ct);
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
	hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_sphere_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = 12;
	double line_width = LINE_WIDTH;
	double rad = hatch_size / 4.0 + 0.5;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	
	
	/* draw one hatch */
	{
		int Rb,Gb,Bb;
		int Rf,Gf,Bf;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		Rb = (backcolor & 0x00FF0000) >> 16;
		Gb = (backcolor & 0x0000FF00) >> 8;
		Bb = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw half circles in the foreground */
		Rf = (forecolor & 0x00FF0000) >> 16;
		Gf = (forecolor & 0x0000FF00) >> 8;
		Bf = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) Rf / 255.0, (double) Gf / 255.0, (double) Bf / 255.0);

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
		cairo_set_source_rgb (ct, (double) Rb / 255.0, (double) Gb / 255.0, (double) Bb / 255.0);
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
        hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
draw_checker_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format, GpHatch *hbr)
{
	cairo_surface_t *hatch; cairo_pattern_t *pattern;
	double hatch_size = 5.0;

	if (hbr->hatchStyle == HatchStyleLargeCheckerBoard)
		hatch_size *= 2.0;

	hatch = cairo_surface_create_similar (cairo_get_target (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	pattern = cairo_get_source (ct);
	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);	
	
	/* draw one hatch that has two colored squares at upper left and lower right corners */
	{
		int R,G,B;

		cairo_save (ct);

		/* hatch is not supposed to be affected by user matrix */
		cairo_identity_matrix (ct);
		ct = cairo_create (hatch);

		/* draw background */
		R = (backcolor & 0x00FF0000) >> 16;
		G = (backcolor & 0x0000FF00) >> 8;
		B = (backcolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		cairo_rectangle (ct, 0, 0, hatch_size, hatch_size);
		cairo_fill (ct);

		/* draw squares in the foreground */
		R = (forecolor & 0x00FF0000) >> 16;
		G = (forecolor & 0x0000FF00) >> 8;
		B = (forecolor & 0x000000FF);
		cairo_set_source_rgb (ct, (double) R / 255.0, (double) G / 255.0, (double) B / 255.0);

		/* upper left square */
		cairo_rectangle (ct, 0, 0, hatch_size / 2.0, hatch_size / 2.0);

		/* lower right square */
		cairo_rectangle (ct, hatch_size / 2.0, hatch_size / 2.0, hatch_size, hatch_size);
		cairo_fill (ct);

		cairo_paint (ct);
		
		cairo_restore (ct);
	}

	/* set the pattern for the consequent fill or stroke */
        hbr->pattern = cairo_pattern_create_for_surface (hatch);
	cairo_surface_destroy (hatch);

	return Ok;
}

GpStatus
gdip_hatch_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpHatch *hbr;
	cairo_t *ct;
	int forecol;
	int backcol;
	cairo_format_t format = CAIRO_CONTENT_COLOR_ALPHA;
	GpStatus status = Ok;

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	hbr = (GpHatch *) brush;
	forecol = hbr->foreColor;
	backcol = hbr->backColor;

	ct = graphics->ct;

	g_return_val_if_fail (ct != NULL, InvalidParameter);

	/* We create the new pattern for brush, if the brush is changed
	 * or if pattern has not been created yet.
	 */
	if (hbr->base.changed || (hbr->pattern) == NULL) {

		/* destroy the existing pattern */
		if (hbr->pattern)
			cairo_pattern_destroy (hbr->pattern);

		switch (hbr->hatchStyle) {

		/* case HatchStyleMin: */
		case HatchStyleHorizontal:
		case HatchStyleLightHorizontal:
		case HatchStyleNarrowHorizontal:
		case HatchStyleDarkHorizontal:
			status = draw_horizontal_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleVertical:
		case HatchStyleLightVertical:
		case HatchStyleNarrowVertical:
		case HatchStyleDarkVertical:
			status = draw_veritcal_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleForwardDiagonal:
			status = draw_forward_diagonal_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleBackwardDiagonal:
			status = draw_backward_diagonal_hatch (ct, forecol, backcol, format, hbr);
			break;

		/* case HatchStyleCross: */
		case HatchStyleLargeGrid:
		case HatchStyleSmallGrid:
		case HatchStyleDottedGrid:
			status = draw_cross_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleDiagonalCross:
		case HatchStyleDottedDiamond:
		case HatchStyleOutlinedDiamond:
			status = draw_diagonal_cross_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle05Percent:
			status = draw_05_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle10Percent:
			status = draw_10_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle20Percent:
			status = draw_20_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle25Percent:
			status = draw_25_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle30Percent:
			status = draw_70_percent_hatch (ct, backcol, forecol, format, hbr);
			break;

		case HatchStyle40Percent:
			status = draw_40_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle50Percent:
			status = draw_50_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle60Percent:
			status = draw_60_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle70Percent:
			status = draw_70_percent_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyle75Percent:
			status = draw_25_percent_hatch (ct, backcol, forecol, format, hbr);
			break;

		case HatchStyle80Percent:
			status = draw_20_percent_hatch (ct, backcol, forecol, format, hbr);
			break;

		case HatchStyle90Percent:
			status = draw_10_percent_hatch (ct, backcol, forecol, format, hbr);
			break;

		case HatchStyleLightDownwardDiagonal:
		case HatchStyleDarkDownwardDiagonal:
		case HatchStyleWideDownwardDiagonal:
			status = draw_downward_diagonal_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleLightUpwardDiagonal:
		case HatchStyleDarkUpwardDiagonal:
		case HatchStyleWideUpwardDiagonal:
			status = draw_upward_diagonal_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleDashedDownwardDiagonal:
		case HatchStyleDashedUpwardDiagonal:
			status = draw_dashed_diagonal_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleDashedHorizontal:
			status = draw_dashed_horizontal_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleDashedVertical:
			status = draw_dashed_vertical_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleSmallConfetti:
			status = draw_confetti_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleLargeConfetti:
			status = draw_confetti_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleZigZag:
			status = draw_zigzag_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleWave:
			status = draw_wave_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleDiagonalBrick:
			status = draw_diagonal_brick_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleHorizontalBrick:
			status = draw_horizontal_brick_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleWeave:
			status = draw_weave_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStylePlaid:
			status = draw_plaid_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleDivot:
			status = draw_divot_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleShingle:
			status = draw_shingle_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleTrellis:
			status = draw_trellis_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleSphere:
		  	status = draw_sphere_hatch (ct, forecol, backcol, format, hbr);
			break;

		case HatchStyleSmallCheckerBoard:
		case HatchStyleLargeCheckerBoard:
			status = draw_checker_hatch (ct, forecol, backcol, format, hbr);
			break;

		/* case HatchStyleMax: */
		case HatchStyleSolidDiamond:
			status = draw_solid_diamond_hatch (ct, forecol, backcol, format, hbr);
			break;

		default:
			status = InvalidParameter;
		}
	}

	if (status == Ok) {
		if (hbr->pattern == NULL)
			status = GenericError;
		else {
			cairo_set_source (ct, hbr->pattern);
			status = gdip_get_status (cairo_status (ct));
		}
	}

	return status;
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
	result->base.changed = TRUE;

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
