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

/*
 * we have a single copy of vtable for
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

        gdip_hatch_init (result);

        return result;
}

void
gdip_hatch_setup (GpGraphics *graphics, GpBrush *brush)
{
	GpHatch *hatch = (GpHatch *) brush;
	GpImage *img;
	GpBitmap *bmp;
	cairo_t *ct;
	unsigned int width;
	unsigned int height;
	int forecol = hatch->foreColor;
	int backcol = hatch->backColor;
	cairo_format_t format;

	img = graphics->image;
	if (img->type == imageBitmap) {
		bmp = (GpBitmap *) img;
		width = bmp->data.Width;
		height = bmp->data.Height;
		format = bmp->data.PixelFormat;
	} else 
		return;

	ct = graphics->ct;

#define HATCH_SIZE 7
#define LINE_WIDTH 1

	switch (hatch->hatchStyle) {

	/* case HatchStyleMin: */
	case HatchStyleHorizontal:
	case HatchStyleLightHorizontal:
	case HatchStyleNarrowHorizontal:
	case HatchStyleDarkHorizontal:
		draw_horizontal_hatch (ct, forecol, backcol, width, height, format, hatch->hatchStyle);
		break;

	case HatchStyleVertical:
	case HatchStyleLightVertical:
	case HatchStyleNarrowVertical:
	case HatchStyleDarkVertical:
		draw_veritcal_hatch (ct, forecol, backcol, width, height, format, hatch->hatchStyle);
		break;

	case HatchStyleForwardDiagonal:
		draw_forward_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleBackwardDiagonal:
		draw_backward_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	/* case HatchStyleCross: */
	case HatchStyleLargeGrid:
		draw_cross_hatch (ct, forecol, backcol, width, height, format);
		break;

	case HatchStyleDiagonalCross:
		draw_diagonal_cross_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyle05Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 5);
		break;

	case HatchStyle10Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 10);
		break;

	case HatchStyle20Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 20);
		break;

	case HatchStyle25Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 25);
		break;

	case HatchStyle30Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 30);
		break;

	case HatchStyle40Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 40);
		break;

	case HatchStyle50Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 50);
		break;

	case HatchStyle60Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 60);
		break;

	case HatchStyle70Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 70);
		break;

	case HatchStyle75Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 75);
		break;

	case HatchStyle80Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 80);
		break;

	case HatchStyle90Percent:
		draw_percentage_hatch (ct, forecol, backcol, width, height, 90);
		break;

	case HatchStyleLightDownwardDiagonal:
		draw_light_down_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleLightUpwardDiagonal:
		draw_light_upward_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDarkDownwardDiagonal:
		draw_dark_down_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDarkUpwardDiagonal:
		draw_dark_upward_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleWideDownwardDiagonal:
		draw_wide_down_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleWideUpwardDiagonal:
		draw_wide_upward_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDashedDownwardDiagonal:
		draw_dashed_down_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDashedUpwardDiagonal:
		draw_dashed_upward_diagonal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDashedHorizontal:
		draw_dashed_horizontal_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDashedVertical:
		draw_dashed_vertical_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleSmallConfetti:
		draw_small_confetti_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleLargeConfetti:
		draw_large_confetti_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleZigZag:
		draw_zigzag_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleWave:
		draw_wave_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDiagonalBrick:
		draw_diagonal_brick_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleHorizontalBrick:
		draw_horizontal_brick_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleWeave:
		draw_weave_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStylePlaid:
		draw_plaid_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDivot:
		draw_divot_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDottedGrid:
		draw_dotted_grid_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleDottedDiamond:
		draw_dotted_diamond_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleShingle:
		draw_Shingle_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleTrellis:
		draw_trellis_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleSphere:
		draw_sphere_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleSmallGrid:
		draw_small_grid_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleSmallCheckerBoard:
		draw_small_checker_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleLargeCheckerBoard:
		draw_large_checker_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleOutlinedDiamond:
		draw_outlined_diamond_hatch (ct, forecol, backcol, width, height);
		break;

	case HatchStyleSolidDiamond:
	/* case HatchStyleMax: */
		draw_solid_diamond_hatch (ct, forecol, backcol, width, height);
		break;

	default:
		break;
	}
}

void
gdip_hatch_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpHatch *result = (GpHatch *) GdipAlloc (sizeof (GpHatch));
	GpHatch *hatch = (GpHatch *) brush;

	result->base = hatch->base;
	result->hatchStyle = hatch->hatchStyle;
	result->foreColor = hatch->foreColor;
	result->backColor = hatch->backColor;

	*clonedBrush = (GpBrush *) result;
}

void
gdip_hatch_destroy (GpBrush *brush)
{
	GdipFree (brush);
}

GpStatus
GdipCreateHatchBrush (GpHatchStyle hatchstyle, int forecolor, int backcolor, GpHatch **brush)
{
        *brush = gdip_hatch_new ();
        (*brush)->hatchStyle = hatchstyle;
        (*brush)->foreColor = forecolor;
        (*brush)->backColor = backcolor;

        return Ok; 
}

GpStatus
GdipGetHatchStyle (GpHatch *brush, GpHatchStyle *hatchstyle)
{
	*hatchstyle = brush->hatchStyle;
	return Ok;
}

GpStatus
GdipGetHatchForegroundColor (GpHatch *brush, int *forecolor)
{
	*forecolor = brush->foreColor;
	return Ok;
}

GpStatus
GdipGetHatchBackgroundColor (GpHatch *brush, int *backcolor)
{
	*backcolor = brush->backColor;
	return Ok;
}

/*
 * functions to draw different hatches
 */
void
draw_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	/*
	 * the values used below are obtained after the lots of hit and trial
	 * to get the results similar to that of .net. I'm not sure if results
	 * will be same with different resolutions
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

		/* draw line */
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

	/* fill the whole surface with horizontal lines */
	cairo_set_pattern (ct, hatch);
	cairo_surface_destroy (hatch);
}

void draw_veritcal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format, GpHatchStyle hatchStyle)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

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

		/* draw line */
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

	/* fill the whole surface with horizontal lines */
	cairo_set_pattern (ct, hatch);
	cairo_surface_destroy (hatch);
}

void draw_forward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_backward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_cross_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format)
{
	cairo_surface_t *hatch;
	double hatch_size = HATCH_SIZE;
	double line_width = LINE_WIDTH;

	hatch = cairo_surface_create_similar (cairo_current_target_surface (ct),
					      format, hatch_size, hatch_size);
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

		/* draw line */
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

	/* fill the whole surface with horizontal lines */
	cairo_set_pattern (ct, hatch);
	cairo_surface_destroy (hatch);
}

void draw_diagonal_cross_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_percentage_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, int percentage)
{
	/* NotImplemented */
}

void draw_light_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_light_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dark_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dark_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_wide_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_wide_upward_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dashed_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dashed_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dashed_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dashed_vertical_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_small_confetti_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_large_confetti_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_zigzag_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_wave_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_diagonal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_horizontal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_weave_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_plaid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_divot_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dotted_grid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_dotted_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_Shingle_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_trellis_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_sphere_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_small_grid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_small_checker_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_large_checker_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_outlined_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}

void draw_solid_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height)
{
	/* NotImplemented */
}
