/*
 * hatchbrush.c
 * 
 * Copyright (C) 2004 Novell, Inc.
 * 
 * Authors:
 *   Ravindra (rkumar@novell.com)
 *
 */
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
draw_backward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height,cairo_format_t format)
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
draw_cross_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);
	
	/* draw one hatch */
	{
		cairo_save (ct);

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
draw_diagonal_cross_hatch (cairo_t *ct, int forecolor, int backcolor, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);

	g_return_val_if_fail (hatch != NULL, OutOfMemory);

	cairo_surface_set_repeat (hatch, 1);

	/* draw one hatch */
	{
		cairo_save (ct);

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
draw_percentage_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, int percentage)
{
	return NotImplemented;
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
draw_dashed_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_dashed_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_dashed_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_dashed_vertical_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
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
draw_diagonal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_horizontal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
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
draw_dotted_grid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_dotted_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
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
draw_sphere_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_small_grid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_small_checker_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_large_checker_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_outlined_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
}

GpStatus
draw_solid_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	return NotImplemented;
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
		return draw_cross_hatch (ct, forecol, backcol, format);

	case HatchStyleDiagonalCross:
		return draw_diagonal_cross_hatch (ct, forecol, backcol, format);

	case HatchStyle05Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 5);

	case HatchStyle10Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 10);

	case HatchStyle20Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 20);

	case HatchStyle25Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 25);

	case HatchStyle30Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 30);

	case HatchStyle40Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 40);

	case HatchStyle50Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 50);

	case HatchStyle60Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 60);

	case HatchStyle70Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 70);

	case HatchStyle75Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 75);

	case HatchStyle80Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 80);

	case HatchStyle90Percent:
		return draw_percentage_hatch (ct, forecol, backcol, width, height, 90);

	case HatchStyleLightDownwardDiagonal:
	case HatchStyleDarkDownwardDiagonal:
	case HatchStyleWideDownwardDiagonal:
		return draw_downward_diagonal_hatch (ct, forecol, backcol, width, height, format, hatch->hatchStyle);

	case HatchStyleLightUpwardDiagonal:
	case HatchStyleDarkUpwardDiagonal:
	case HatchStyleWideUpwardDiagonal:
		return draw_upward_diagonal_hatch (ct, forecol, backcol, width, height, format, hatch->hatchStyle);

	case HatchStyleDashedDownwardDiagonal:
		return draw_dashed_down_diagonal_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDashedUpwardDiagonal:
		return draw_dashed_upward_diagonal_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDashedHorizontal:
		return draw_dashed_horizontal_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDashedVertical:
		return draw_dashed_vertical_hatch (ct, forecol, backcol, width, height);

	case HatchStyleSmallConfetti:
		return draw_small_confetti_hatch (ct, forecol, backcol, width, height);

	case HatchStyleLargeConfetti:
		return draw_large_confetti_hatch (ct, forecol, backcol, width, height);

	case HatchStyleZigZag:
		return draw_zigzag_hatch (ct, forecol, backcol, width, height);

	case HatchStyleWave:
		return draw_wave_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDiagonalBrick:
		return draw_diagonal_brick_hatch (ct, forecol, backcol, width, height);

	case HatchStyleHorizontalBrick:
		return draw_horizontal_brick_hatch (ct, forecol, backcol, width, height);

	case HatchStyleWeave:
		return draw_weave_hatch (ct, forecol, backcol, width, height);

	case HatchStylePlaid:
		return draw_plaid_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDivot:
		return draw_divot_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDottedGrid:
		return draw_dotted_grid_hatch (ct, forecol, backcol, width, height);

	case HatchStyleDottedDiamond:
		return draw_dotted_diamond_hatch (ct, forecol, backcol, width, height);

	case HatchStyleShingle:
		return draw_shingle_hatch (ct, forecol, backcol, width, height);

	case HatchStyleTrellis:
		return draw_trellis_hatch (ct, forecol, backcol, width, height);

	case HatchStyleSphere:
		return draw_sphere_hatch (ct, forecol, backcol, width, height);

	case HatchStyleSmallGrid:
		return draw_small_grid_hatch (ct, forecol, backcol, width, height);

	case HatchStyleSmallCheckerBoard:
		return draw_small_checker_hatch (ct, forecol, backcol, width, height);

	case HatchStyleLargeCheckerBoard:
		return draw_large_checker_hatch (ct, forecol, backcol, width, height);

	case HatchStyleOutlinedDiamond:
		return draw_outlined_diamond_hatch (ct, forecol, backcol, width, height);

	case HatchStyleSolidDiamond:
	/* case HatchStyleMax: */
		return draw_solid_diamond_hatch (ct, forecol, backcol, width, height);

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
