/*
 * hatchbrush.h
 *
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004 Novell, Inc.
 */

#ifndef _HATCHBRUSH_H
#define _HATCHBRUSH_H

#include "brush.h"

typedef enum {
	HatchStyleHorizontal = 0,
	HatchStyleVertical   = 1,
	HatchStyleForwardDiagonal  = 2,
	HatchStyleBackwardDiagonal = 3,
	HatchStyleCross = 4,
	HatchStyleDiagonalCross = 5,
	HatchStyle05Percent = 6,
	HatchStyle10Percent = 7,
	HatchStyle20Percent = 8,
	HatchStyle25Percent = 9,
	HatchStyle30Percent = 10,
	HatchStyle40Percent = 11,
	HatchStyle50Percent = 12,
	HatchStyle60Percent = 13,
	HatchStyle70Percent = 14,
	HatchStyle75Percent = 15,
	HatchStyle80Percent = 16,
	HatchStyle90Percent = 17,
	HatchStyleLightDownwardDiagonal = 18,
	HatchStyleLightUpwardDiagonal   = 19,
	HatchStyleDarkDownwardDiagonal  = 20,
	HatchStyleDarkUpwardDiagonal    = 21,
	HatchStyleWideDownwardDiagonal  = 22,
	HatchStyleWideUpwardDiagonal    = 23,
	HatchStyleLightVertical    = 24,
	HatchStyleLightHorizontal  = 25,
	HatchStyleNarrowVertical   = 26,
	HatchStyleNarrowHorizontal = 27,
	HatchStyleDarkVertical     = 28,
	HatchStyleDarkHorizontal   = 29,
	HatchStyleDashedDownwardDiagonal = 30,
	HatchStyleDashedUpwardDiagonal   = 31,
	HatchStyleDashedHorizontal = 32,
	HatchStyleDashedVertical   = 33,
	HatchStyleSmallConfetti = 34,
	HatchStyleLargeConfetti = 35,
	HatchStyleZigZag = 36,
	HatchStyleWave   = 37,
	HatchStyleDiagonalBrick   = 38,
	HatchStyleHorizontalBrick = 39,
	HatchStyleWeave = 40,
	HatchStylePlaid = 41,
	HatchStyleDivot = 42,
	HatchStyleDottedGrid    = 43,
	HatchStyleDottedDiamond = 44,
	HatchStyleShingle = 45,
	HatchStyleTrellis = 46,
	HatchStyleSphere  = 47,
	HatchStyleSmallGrid = 48,
	HatchStyleSmallCheckerBoard = 49,
	HatchStyleLargeCheckerBoard = 50,
	HatchStyleOutlinedDiamond = 51,
	HatchStyleSolidDiamond    = 52,
	HatchStyleLargeGrid = HatchStyleCross,
	HatchStyleMin = HatchStyleHorizontal,
	HatchStyleMax = HatchStyleSolidDiamond
} GpHatchStyle, HatchStyle;

/* Structures */

typedef struct _Hatch GpHatch;

typedef struct _Hatch {
	GpBrush base;
	GpHatchStyle hatchStyle;
	int foreColor;
	int backColor;
} Hatch;

/* Hatch brush */
GpHatch *gdip_hatch_new (void);
void gdip_hatch_init (GpHatch *hatch);
void gdip_hatch_setup (GpGraphics *graphics, GpBrush *brush);
void gdip_hatch_clone (GpBrush *brush, GpBrush **clonedBrush);
void gdip_hatch_destroy (GpBrush *brush);

GpStatus GdipCreateHatchBrush (GpHatchStyle hatchstyle, int forecolor, int backcolor, GpHatch **brush);
GpStatus GdipGetHatchStyle (GpHatch *brush, GpHatchStyle *hatchstyle);
GpStatus GdipGetHatchForegroundColor (GpHatch *brush, int *forecolor);
GpStatus GdipGetHatchBackgroundColor (GpHatch *brush, int *backcolor);

// functions to draw different hatches

void draw_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format, GpHatchStyle hatchstyle);
void draw_veritcal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, cairo_format_t format, GpHatchStyle hatchstyle);
void draw_forward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_backward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_cross_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_diagonal_cross_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_percentage_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height, int percentage);
void draw_light_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_light_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dark_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dark_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_wide_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_wide_upward_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dashed_down_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dashed_upward_diagonal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dashed_horizontal_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dashed_vertical_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_small_confetti_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_large_confetti_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_zigzag_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_wave_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_diagonal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_horizontal_brick_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_weave_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_plaid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_divot_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dotted_grid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_dotted_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_Shingle_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_trellis_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_sphere_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_small_grid_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_small_checker_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_large_checker_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_outlined_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);
void draw_solid_diamond_hatch (cairo_t *ct, int forecolor, int backcolor, int width, int height);

#endif /* _HATCHBRUSH_H */
