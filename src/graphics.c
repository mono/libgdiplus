/*
 * graphics.c
 *
 * Copyright (c) 2003 Alexandre Pigolkine, Novell Inc.
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
 *   Alexandre Pigolkine (pigolkine@gmx.de)
 *   Duncan Mak (duncan@ximian.com)
 *
 */

#include "gdip.h"
#include "gdip_win32.h"
#include <math.h>
#include <glib.h>

void
gdip_graphics_init (GpGraphics *graphics)
{
	graphics->ct = cairo_create ();
	graphics->copy_of_ctm = cairo_matrix_create ();
	cairo_matrix_set_identity (graphics->copy_of_ctm);
	graphics->hdc = 0;
	graphics->hdc_busy_count = 0;
	graphics->image = 0;
	graphics->type = gtUndefined;
        /* cairo_select_font (graphics->ct, "serif:12"); */
	cairo_select_font (graphics->ct, "serif:12", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
}

GpGraphics *
gdip_graphics_new ()
{
	GpGraphics *result = (GpGraphics *) GdipAlloc (sizeof (GpGraphics));
	gdip_graphics_init (result);
	return result;
}

void
gdip_graphics_attach_bitmap (GpGraphics *graphics, GpBitmap *image)
{
	cairo_set_target_image (graphics->ct, image->data.Scan0, image->cairo_format,
				image->data.Width, image->data.Height, image->data.Stride);
	graphics->image = image;
	graphics->type = gtMemoryBitmap;
}

void 
gdip_graphics_detach_bitmap (GpGraphics *graphics, GpBitmap *image)
{
	printf ("Implement graphics_detach_bitmap");
	/* FIXME: implement me */
}

#define C1 0.552285
static void 
make_ellipse (GpGraphics *graphics, float x, float y, float width, float height)
{
        double rx = width / 2;
        double ry = height / 2;
        double cx = x + rx;
        double cy = y + ry;

        cairo_move_to (graphics->ct, cx + rx, cy);

        /* an approximate of the ellipse by drawing a curve in each
         * quartrant */
        cairo_curve_to (graphics->ct,
                        cx + rx, cy - C1 * ry,
                        cx + C1 * rx, cy - ry,
                        cx, cy - ry);
        
        cairo_curve_to (graphics->ct,
                        cx - C1 * rx, cy - ry,
                        cx - rx, cy - C1 * ry,
                        cx - rx, cy);

        cairo_curve_to (graphics->ct,
                        cx - rx, cy + C1 * ry,
                        cx - C1 * rx, cy + ry,
                        cx, cy + ry);
                
        cairo_curve_to (graphics->ct,
                        cx + C1 * rx, cy + ry,
                        cx + rx, cy + C1 * ry,
                        cx + rx, cy);

        cairo_close_path (graphics->ct);
}

static void
make_polygon (GpGraphics *graphics, GpPointF *points, int count)
{
        int i;
        cairo_move_to (graphics->ct, points [0].X, points [0].Y);

        for (i = 0; i < count; i++)
                cairo_line_to (graphics->ct, points [i].X, points [i].Y);

        /*
         * Draw a line from the last point back to the first point if
         * they're not the same
         */
        if (points [0].X != points [count].X && points [0].Y != points [count].Y)
                cairo_line_to (graphics->ct, points [0].X, points [0].Y);

        cairo_close_path (graphics->ct);
}

static void
make_polygon_from_integers (GpGraphics *graphics, GpPoint *points, int count)
{
        int i;
        cairo_move_to (graphics->ct, points [0].X, points [0].Y);

        for (i = 0; i < count; i++)
                cairo_line_to (graphics->ct, points [i].X, points [i].Y);

        /*
         * Draw a line from the last point back to the first point if
         * they're not the same
         */
        if (points [0].X != points [count].X && points [0].Y != points [count].Y)
                cairo_line_to (graphics->ct, points [0].X, points [0].Y);

        cairo_close_path (graphics->ct);
}

/*
 * Based on the algorithm described in
 *      http://www.stillhq.com/ctpfaq/2002/03/c1088.html#AEN1212
 */
static void
make_pie (GpGraphics *graphics, float x, float y, 
          float width, float height, float startAngle, float sweepAngle)
{
        float rx = width / 2;
        float ry = height / 2;
        int cx = x + rx;
        int cy = y + ry;

        /* angles in radians */        
        float alpha = startAngle * PI / 180;
        float beta = sweepAngle * PI / 180;

        float delta = beta - alpha;
        float bcp = 4.0 / 3 * (1 - cos (delta / 2)) / sin (delta /2);

        float sin_alpha = sin (alpha);
        float sin_beta = sin (beta);
        float cos_alpha = cos (alpha);
        float cos_beta = cos (beta);

        /* move to center */
        cairo_move_to (graphics->ct, cx, cy);

        /* draw pie edge */
        cairo_line_to (graphics->ct,
                       cx + rx * cos_alpha, 
                       cy + ry * sin_alpha);
        
        cairo_curve_to (graphics->ct,
                        cx + rx * (cos_alpha - bcp * sin_alpha),
                        cy + ry * (sin_alpha + bcp * cos_alpha),
                        cx + rx * (cos_beta  + bcp * sin_beta),
                        cy + ry * (sin_beta  - bcp * cos_beta),
                        cx + rx *  cos_beta,
                        cy + ry *  sin_beta);

        /* draws line back to center */
        cairo_close_path (graphics->ct);
}

static void
make_arc (GpGraphics *graphics, float x, float y, float width,
	  float height, float startAngle, float sweepAngle)
{        
        float rx = width / 2;
        float ry = height / 2;
        
        /* center */
        int cx = x + rx;
        int cy = y + ry;

        /* angles in radians */        
        float alpha = startAngle * PI / 180;
        float beta = sweepAngle * PI / 180;

        float delta = beta - alpha;
        float bcp = 4.0 / 3 * (1 - cos (delta / 2)) / sin (delta /2);

        float sin_alpha = sin (alpha);
        float sin_beta = sin (beta);
        float cos_alpha = cos (alpha);
        float cos_beta = cos (beta);

        /* move to pie edge */
        cairo_move_to (graphics->ct,
                       cx + rx * cos_alpha, 
                       cy + ry * sin_alpha);

        cairo_curve_to (graphics->ct,
                        cx + rx * (cos_alpha - bcp * sin_alpha),
                        cy + ry * (sin_alpha + bcp * cos_alpha),
                        cx + rx * (cos_beta  + bcp * sin_beta),
                        cy + ry * (sin_beta  - bcp * cos_beta),
                        cx + rx *  cos_beta,
                        cy + ry *  sin_beta);
}

static cairo_fill_rule_t
convert_fill_mode (GpFillMode fill_mode)
{
        if (fill_mode == FillModeAlternate) 
                return CAIRO_FILL_RULE_EVEN_ODD;
        else
                return CAIRO_FILL_RULE_WINDING;
}


GpStatus 
GdipCreateFromHDC (int hDC, GpGraphics **graphics)
{
	DC* dc = _get_DC_by_HDC (hDC);
	
	/* printf ("GdipCreateFromHDC. in %d, DC %p\n", hDC, dc); */
	if (dc == 0) return NotImplemented;
	
	*graphics = gdip_graphics_new ();
	cairo_set_target_drawable ( (*graphics)->ct, GDIP_display, dc->physDev->drawable);
	_release_hdc (hDC);
	(*graphics)->hdc = (void*)hDC;
	(*graphics)->type = gtX11Drawable;
	/* printf ("GdipCreateFromHDC. graphics %p, ct %p\n", (*graphics), (*graphics)->ct); */
	return Ok;
}

GpStatus 
GdipDeleteGraphics (GpGraphics *graphics)
{
	/* FIXME: attention to surface (image, etc.) */
	/* printf ("GdipDeleteGraphics. graphics %p\n", graphics); */
	cairo_matrix_destroy (graphics->copy_of_ctm);
	cairo_destroy (graphics->ct);
	GdipFree (graphics);
	return Ok;
}

GpStatus 
GdipGetDC (GpGraphics *graphics, int *hDC)
{
	if (graphics->hdc == 0) {
		if (graphics->image != 0) {
			/* Create DC */
			graphics->hdc = gdip_image_create_Win32_HDC (graphics->image);
			if (graphics->hdc != 0) {
				++graphics->hdc_busy_count;
			}
		}
	}
	*hDC = (int)graphics->hdc;
	return Ok;
}

GpStatus 
GdipReleaseDC (GpGraphics *graphics, int hDC)
{
	if (graphics->hdc != (void *)hDC) return InvalidParameter;
	if (graphics->hdc_busy_count > 0) {
		--graphics->hdc_busy_count;
		if (graphics->hdc_busy_count == 0) {
			/* Destroy DC */
			gdip_image_destroy_Win32_HDC (graphics->image, (void*)hDC);
			graphics->hdc = 0;
		}
	}
	return Ok;
}

/* FIXME: the stack implementation is probably not suitable */
#define MAX_GRAPHICS_STATE_STACK 100

GpState saved_stack [MAX_GRAPHICS_STATE_STACK];
int current_stack_pos = 0;

GpStatus 
GdipRestoreGraphics (GpGraphics *graphics, unsigned int graphicsState)
{
	if (graphicsState < MAX_GRAPHICS_STATE_STACK) {
		cairo_matrix_copy (graphics->copy_of_ctm, saved_stack[graphicsState].matrix);
		cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
		current_stack_pos = graphicsState;
	}
	else {
		return InvalidParameter;
	}
	return Ok;
}

GpStatus 
GdipSaveGraphics (GpGraphics *graphics, unsigned int *state)
{
	if (current_stack_pos < MAX_GRAPHICS_STATE_STACK) {
		saved_stack[current_stack_pos].matrix = cairo_matrix_create ();
		cairo_matrix_copy (saved_stack[current_stack_pos].matrix, graphics->copy_of_ctm);
		*state = current_stack_pos;
		++current_stack_pos;
	}
	else {
		return OutOfMemory;
	}
	return Ok;
}

GpStatus
GdipResetWorldTransform (GpGraphics *graphics)
{
        GpStatus s = cairo_matrix_set_identity (graphics->copy_of_ctm);

        if (s != Ok)
                return s;
        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return Ok;
        }
}

GpStatus
GdipSetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
        graphics->copy_of_ctm = matrix;
        cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
        return Ok;
}

GpStatus 
GdipGetWorldTransform (GpGraphics *graphics, GpMatrix *matrix)
{
        cairo_current_matrix (graphics->ct, matrix);
        return Ok;
}

GpStatus
GdipMultiplyWorldTransform (GpGraphics *graphics, GpMatrix *matrix, GpMatrixOrder order)
{
        Status s = GdipMultiplyMatrix (graphics->copy_of_ctm, matrix, order);
        
        if (s != Ok)
                return s;

        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return Ok;
        }
}

GpStatus 
GdipRotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order)
{
	GpStatus s = GdipRotateMatrix (graphics->copy_of_ctm, angle, order);

        if (s != Ok)
                return s;
        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return Ok;
        }
}

GpStatus 
GdipTranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order)
{
        GpStatus s = GdipTranslateMatrix (graphics->copy_of_ctm, dx, dy, order);

        if (s != Ok) 
                return s;
        else {
                cairo_set_matrix (graphics->ct, graphics->copy_of_ctm);
                return Ok;
        }
}

GpStatus
GdipDrawArc (GpGraphics *graphics, GpPen *pen, 
	     float x, float y, float width, float height, 
	     float startAngle, float sweepAngle)
{
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);

        float delta = sweepAngle - startAngle;

        if (delta < 180)
                make_arc (graphics, x, y, width, height, startAngle, sweepAngle);
        else {
                make_arc (graphics, x, y, width, height, startAngle, startAngle + 180);
                make_arc (graphics, x, y, width, height, startAngle + 180, sweepAngle);
        }

        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawArcI (GpGraphics *graphics, GpPen *pen, 
	      int x, int y, int width, int height, 
	      float startAngle, float sweepAngle)
{
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);

        float delta = sweepAngle - startAngle;

        if (delta < 180)
                make_arc (graphics, x, y, width, height, startAngle, sweepAngle);

        else {
                make_arc (graphics, x, y, width, height, startAngle, startAngle + 180);
                make_arc (graphics, x, y, width, height, startAngle + 180, sweepAngle);
        }

        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawBezier (GpGraphics *graphics, GpPen *pen, 
                float x1, float y1, float x2, float y2,
                float x3, float y3, float x4, float y4)
{
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);        
        
        cairo_move_to (graphics->ct, x1, y1);
        cairo_curve_to (graphics->ct, x2, y2, x3, y3, x4, y4);
        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, 
			  int x1, int y1, int x2, int y2,
			  int x3, int y3, int x4, int y4)
{
        return GdipDrawBezier (graphics, pen,
			       x1, y1, x2, y2, x3, y3, x4, y4);
}

GpStatus 
GdipDrawBeziers (GpGraphics *graphics, GpPen *pen,
		 GpPointF *points, int count)
{
        int i, j, k;
        
        if (count == 0)
                return Ok;

	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);
        
        cairo_move_to (graphics->ct, points [0].X, points [0].Y);

        for (i = 0; i < count - 3; i += 3) {
                j = i + 1;
                k = i + 2;
                cairo_curve_to (graphics->ct,
                                points [i].X, points [i].Y,
                                points [j].X, points [j].Y,
                                points [k].X, points [k].Y);
        }

        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen,
		  GpPoint *points, int count)
{
        int i, j, k;
        
        if (count == 0)
                return Ok;

	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);

        cairo_move_to (graphics->ct, points [0].X, points [0].Y);

        for (i = 0; i < count - 3; i += 3) {
                j = i + 1;
                k = i + 2;
                cairo_curve_to (graphics->ct,
                                points [i].X, points [i].Y,
                                points [j].X, points [j].Y,
                                points [k].X, points [k].Y);
        }

        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, 
		 float x, float y, float width, float height)
{
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);
        
        make_ellipse (graphics, x, y, width, height);
        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawEllipseI (GpGraphics *graphics, GpPen *pen,
		  int x, int y, int width, int height)
{
        return GdipDrawEllipse (graphics, pen, x, y, width, height);
}

GpStatus
GdipDrawLine (GpGraphics *graphics, GpPen *pen,
	      float x1, float y1, float x2, float y2)
{
	cairo_save (graphics->ct);

	gdip_pen_setup (graphics, pen);

	cairo_move_to (graphics->ct, x1, y1);
	cairo_line_to (graphics->ct, x2, y2);
	cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawLineI (GpGraphics *graphics, GpPen *pen, 
	       int x1, int y1, int x2, int y2)
{
        return GdipDrawLine (graphics, pen, x1, y1, x2, y2);
}

GpStatus 
GdipDrawLines (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count)
{
        GpStatus s;
        int i, j;

        for (i = 0; i < count - 1; i++) {
                j = i + 1;
                s = GdipDrawLine (graphics, pen, 
				  points [i].X, points [i].Y,
				  points [j].X, points [j].Y);

                if (s != Ok) return s;
        }

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawLinesI (GpGraphics *graphics, GpPen *pen,
                GpPoint *points, int count)
{
        GpStatus s;
        int i, j;

        for (i = 0; i < count - 1; i++) {
                j = i + 1;
                s = GdipDrawLineI (graphics, pen, 
				   points [i].X, points [i].Y,
				   points [j].X, points [j].Y);

                if (s != Ok) return s;
        }

        return Ok;
}

static GpStatus
gdip_plot_path (GpGraphics *graphics, GpPath *path)
{
        int length = path->count;
        int i;

        for (i = 0; i < length; ++i) {
                GpPointF pt = g_array_index (path->points, GpPointF, i);
                byte type = g_array_index (path->types, byte, i);
                GpPointF pts [3];
                int idx = 0;
                
                switch (type) {
                case PathPointTypeStart:
                        cairo_move_to (graphics->ct, pt.X, pt.Y);
                        break;

                case PathPointTypeLine:
                        cairo_line_to (graphics->ct, pt.X, pt.Y);
                        break;

                case PathPointTypeBezier:
                        if (idx < 3) {
                                pts [idx] = pt;
                                idx ++;

                        } else {
                                cairo_curve_to (graphics->ct,
                                                pts [0].X, pts [0].Y,
                                                pts [1].X, pts [1].Y,
                                                pts [2].X, pts [2].Y);
                                idx = 0;
                        }
                        break;
                        
                default:
                        return NotImplemented;
                }
        }
	return Ok;
}

GpStatus
GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path)
{
	GpStatus status;
	
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);

	status = gdip_plot_path (graphics, path);

        cairo_stroke (graphics->ct);
       	cairo_restore (graphics->ct);

        return status;
}

GpStatus
GdipDrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, 
	     float width, float height, float startAngle, float sweepAngle)
{
        gdip_pen_setup (graphics, pen);

        float delta = sweepAngle - startAngle;

        if (delta < 180)
                make_pie (graphics, x, y, width, height, startAngle, sweepAngle);
        else {
                make_pie (graphics, x, y, width, height, startAngle, startAngle + 180);
                make_pie (graphics, x, y, width, height, startAngle + 180, sweepAngle);
        }

        cairo_stroke (graphics->ct);

        cairo_close_path (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawPieI (GpGraphics *graphics, GpPen *pen, int x, int y, 
	      int width, int height, float startAngle, float sweepAngle)
{
        gdip_pen_setup (graphics, pen);
        
        float delta = sweepAngle - startAngle;
        
        if (delta < 180)
                make_pie (graphics, x, y, width, height, startAngle, sweepAngle);
        else {
                make_pie (graphics, x, y, width, height, startAngle, startAngle + 180);
                make_pie (graphics, x, y, width, height, startAngle + 180, sweepAngle);
        }

        cairo_stroke (graphics->ct);

        cairo_close_path (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count)
{
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);
        
        make_polygon (graphics, points, count);
        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count)
{
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);
        
        make_polygon_from_integers (graphics, points, count);
        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawRectangle (GpGraphics *graphics, GpPen *pen,
		   float x, float y, float width, float height)
{
	cairo_save (graphics->ct);

        gdip_pen_setup (graphics, pen);

        cairo_rectangle (graphics->ct, x, y, width, height);
        cairo_stroke (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen,
		    int x, int y, int width, int height)
{
        return GdipDrawRectangle (graphics, pen, x, y, width, height);
}

GpStatus
GdipDrawClosedCurveI ()
{
	printf ("GdipDrawClosedCurveI not implemented\n");
	return Ok;
}

GpStatus
GdipDrawCurveI ()
{
	printf ("GdipDrawCurveI not implemented\n");
	return Ok;
}

GpStatus
GdipDrawCurve2I ()
{
	printf ("GdipDrawCurve2I not implemented\n");
	return Ok;
}


/*
 * Fills
 */
GpStatus
GdipFillEllipse (GpGraphics *graphics, GpBrush *brush,
		 float x, float y, float width, float height)
{
	cairo_save (graphics->ct);

        gdip_brush_setup (graphics, brush);
        make_ellipse (graphics, x, y, width, height);
        cairo_fill (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush,
		  int x, int y, int width, int height)
{
        return GdipFillEllipse (graphics, brush, x, y, width, height);
}

GpStatus 
GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, 
		   float x, float y, float width, float height)
{
	cairo_save (graphics->ct);

	gdip_brush_setup (graphics, brush);
	cairo_rectangle (graphics->ct, x, y, width, height);
	cairo_fill (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, 
		 GpPointF *points, int count, GpFillMode fillMode)
{
	cairo_save (graphics->ct);

        gdip_brush_setup (graphics, brush);
        make_polygon (graphics, points, count);

        cairo_set_fill_rule (
                graphics->ct,
                convert_fill_mode (fillMode));

        cairo_fill (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path)
{
	GpStatus status;
	
	cairo_save (graphics->ct);

	gdip_brush_setup (graphics, brush);

	status = gdip_plot_path (graphics, path);
	
	cairo_fill (graphics->ct);
	cairo_restore (graphics->ct);

	if (status != Ok)
		return status;
	
	return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, 
		  GpPoint *points, int count, GpFillMode fillMode)
{
	cairo_save (graphics->ct);

        gdip_brush_setup (graphics, brush);
        make_polygon_from_integers (graphics, points, count);

        cairo_set_fill_rule (
                graphics->ct,
                convert_fill_mode (fillMode));
        
        cairo_fill (graphics->ct);

	cairo_restore (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count)
{
        return GdipFillPolygon (graphics, brush, points, count, FillModeAlternate);
}

GpStatus
GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count)
{
        return GdipFillPolygonI (graphics, brush, points, count, FillModeAlternate);
}

GpStatus
GdipFillClosedCurveI ()
{
	printf ("GdipFillClosedCurveI not implemented\n");
	return Ok;
}

GpStatus
GdipFillPie ()
{
	printf ("GdipFillPie not implemented\n");
	return Ok;
}

int
gdip_measure_string_widths (GDIPCONST GpFont *font,
			    const unsigned char *utf8,
			    float **widths_glyphs, int *num_widths,
			    float *total_width, float *total_height)
{
	cairo_status_t status;
	FT_GlyphSlot glyphslot;
	cairo_glyph_t *glyphs = NULL;
	cairo_matrix_t saved;
	cairo_ft_font_t *ft;
	size_t nglyphs;
	int i = 0;
	float *ppos;
	double x, y;

	*total_width = 0;
	*total_height = 0;
	*num_widths = 0;
	*widths_glyphs = NULL;

	ft = (cairo_ft_font_t *)font->cairofnt;

	cairo_matrix_copy (&saved, (const cairo_matrix_t *)&ft->base.matrix);
	cairo_matrix_scale (&ft->base.matrix, font->sizeInPixels, font->sizeInPixels);
	gdpi_utf8_to_glyphs (font->cairofnt, utf8, 0.0, 0.0, &glyphs, &nglyphs);
	cairo_matrix_copy (&font->cairofnt->base.matrix, (const cairo_matrix_t *)&saved);

	if (!nglyphs)
		return 1;

	*widths_glyphs = malloc (sizeof (float) *nglyphs);
	*num_widths = nglyphs;
	ppos = *widths_glyphs;

	if (!nglyphs)
		return 0;

	for (; i < nglyphs-1; i++, ppos++){
		*ppos = glyphs[i+1].x - glyphs[i].x;
		*total_width+= *ppos;
	}

	*ppos = DOUBLE_FROM_26_6 (ft->face->glyph->advance.x);
	*total_width+= *ppos;

	return 1;
}

/*
        Processes hotkeys and tabs and creates a new string and its GpGlyphsDetails information
*/
void
gdip_prepareString(GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpStringFormat *format,
                            char **outstring, GpGlyphsDetails **glyphs_details, int *current_glyph_details)
{
        char *src_str, *trg_str, *string;
        GpGlyphsDetails *cur_glyphs_details;
        float *tabStops = format->tabStops;
        int tabs = format->numtabStops;
        bool hotkey_found = FALSE;
        int chars = 0, len, i;
        bool format_char = FALSE;

        string = src_str = (char*) g_utf16_to_utf8 ((const gunichar2 *) stringUnicode,
					  (glong)length, NULL, NULL, NULL);

        len = (int) strlen (string); /* We want the num of non-coded utf-8 chars*/
        trg_str = *outstring = (char *) malloc (len+1);

        *glyphs_details = cur_glyphs_details = (GpGlyphsDetails *) malloc (sizeof(GpGlyphsDetails) * (len + 1));

        if (format->hotkeyPrefix == HotkeyPrefixNone)
                hotkey_found = TRUE;

        if (len) {
                cur_glyphs_details->is_hotkey = FALSE;
                cur_glyphs_details->tab_distance = 0;
        }

        for (i = 0; i < len ; i++, src_str++){

                if (format_char == FALSE) {
                        cur_glyphs_details->is_hotkey = FALSE;
                        cur_glyphs_details->tab_distance = 0;
                }

                if (*src_str == '\t'){
                        cur_glyphs_details->is_hotkey = FALSE;
                        if (tabs) {
                                cur_glyphs_details->tab_distance = *tabStops;
                                tabStops++;
                                tabs--;
                        }
                        else
                                cur_glyphs_details->tab_distance = 0;

                        format_char = TRUE;
                        continue;
                }

                if (hotkey_found == FALSE && (*src_str == '&') && (i+1 < len) && (*(src_str+1) != '&')) {
                        hotkey_found = TRUE;
                        cur_glyphs_details->tab_distance = 0;

                        if (format->hotkeyPrefix == HotkeyPrefixShow)
                                cur_glyphs_details->is_hotkey = TRUE;
                        else
                                cur_glyphs_details->is_hotkey = FALSE;

                        format_char = TRUE;
                        continue;
                }

                *trg_str = *src_str;
                trg_str++;
                chars++;
                cur_glyphs_details++;
                format_char = FALSE;
        }

        cur_glyphs_details->is_hotkey = FALSE;
        cur_glyphs_details->tab_distance = 0;

        *trg_str = 0;
        *current_glyph_details = chars;
        g_free (string);
}

void
gdip_measure_string_pos (char* string, int len, GpGlyphsDetails *glyphs_details, GDIPCONST GpFont *font,
        GDIPCONST RectF *rc,  GDIPCONST GpStringFormat *format,  RectF *boundingBox,
        int *codepointsFitted, int *linesFilled,
        GpLinePointF **pPoints)
{
        float width = 0, height = 0;
        float *widths_glyphs = NULL;
        float *ppos;
        int num_widths = 0, i, current_glyph, max = 0, lines = 0;
        float realY = rc->Y + font->sizeInPixels;
        float w = 0, x=rc->X, y=realY;
        StringAlignment align, lineAlign;
        GpGlyphsDetails *cur_glyphs_details;
        GpLinePointF *pPoint = NULL;
        float alignY = realY;

        /* Get widths */
        gdip_measure_string_widths (font, string, &widths_glyphs, &num_widths, &width, &height);

        /* Determine in which positions the strings have to be drawn */
        if (pPoints)
                *pPoints = pPoint = (GpLinePointF*) malloc (sizeof (GpLinePointF) * num_widths);

        GdipGetStringFormatAlign (format, &align);
        GdipGetStringFormatLineAlign (format, &lineAlign);

        cur_glyphs_details = glyphs_details; cur_glyphs_details++;
        for (ppos = widths_glyphs, current_glyph = 1, w = 0 ; current_glyph < num_widths+1; ppos++, current_glyph++, cur_glyphs_details++) {
		w += *ppos;

                if (w > max)
                        max = w;

		if ((current_glyph+1 < num_widths+1) && (w + *(ppos+1)) < rc->Width)
                        continue;

                if (pPoints) {

        		switch (align){
		        case StringAlignmentNear: /* left */
			        pPoint->X = rc->X;
	        		break;
        		case StringAlignmentCenter:
		        	pPoint->X = rc->X + ( (rc->Width - w)/2);
	        		break;
        		case StringAlignmentFar: /* Right */
        			pPoint->X = rc->X;
		        	pPoint->X += rc->Width - w;
	        		break;
        		default:
		        	break;
	        	}
                        pPoint->width = w;
                        pPoint++;
                }

		lines++;
                y += font->sizeInPixels;

		/* Cannot fit more text */
		if (y - rc->Y + font->sizeInPixels >= rc->Height)
			break;

		w = 0;
        }

        if (pPoints) {
                alignY = realY; /* Default, top */

                switch (lineAlign){
                case StringAlignmentNear: /* Top */
                        break;
                case StringAlignmentCenter:
                        alignY = realY + ( (realY+rc->Height - y) /2);
        		break;
                case StringAlignmentFar: /* Bottom */
        		alignY = realY + (realY +rc->Height) - y;
	        	break;
                default:
	        	break;
                }

                /* Setup Y coordinate for every line */
                pPoint = *pPoints;
                for (i = 0; i < lines; i++, pPoint++) {
		        pPoint->Y = alignY;
        		alignY += font->sizeInPixels;
                }
        }

	if (boundingBox) {
		boundingBox->X = rc->X;
		boundingBox->Y = rc->Y;
		boundingBox->Width = max;
		boundingBox->Height = y;
	}

	if (linesFilled) *linesFilled = lines;
	if (codepointsFitted) *codepointsFitted = current_glyph;
}


GpStatus
GdipMeasureString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int len, GDIPCONST GpFont *font, GDIPCONST RectF *rc,
			    GDIPCONST GpStringFormat *fmt,  RectF *boundingBox, int *codepointsFitted, int *linesFilled)
{
	float width = 0, height = 0;
	int nLines = 0;
	int nMax = 0;
	char *string;
        GpGlyphsDetails *glyphs_details = NULL;
        int current_glyph_details;
        int length;
        GpStringFormat *deffmt = NULL;
        GpStringFormat *format = (GpStringFormat *)fmt;

        if (!format) {
                GdipStringFormatGetGenericDefault (&deffmt);
                format = deffmt;
        }

	if (!graphics || !font || !rc)
		return InvalidParameter;

        gdip_prepareString (stringUnicode, len, format, &string, &glyphs_details, &current_glyph_details);

	/* Get widths */
        length = (int) g_utf8_strlen (string, -1);

        gdip_measure_string_pos (string, len, glyphs_details, font, rc,  format,  boundingBox,
                codepointsFitted, linesFilled, (GpLinePointF**) NULL);

        if (!deffmt)
                GdipDeleteStringFormat(deffmt);

	g_free (string);
        free (glyphs_details);
}

void
gdip_draw_linestring(GpGraphics *graphics, GpLinePointF* point, char *str, GpBrush *brush, GpFont *font,
        GpGlyphsDetails *glyphs_details)
{
        char *pos = str;
        int i;
        GpGlyphsDetails *cur_glyphs_detailsPos = glyphs_details;

        cairo_move_to (graphics->ct, point->X, point->Y);
        cairo_show_text (graphics->ct, str);

        if ((font->style & FontStyleUnderline)==FontStyleUnderline)
                gdip_font_drawunderline (graphics, brush, point->X, point->Y, point->width);

        if ((font->style & FontStyleStrikeout)==FontStyleStrikeout)
                gdip_font_drawstrikeout (graphics, brush, point->X, point->Y, point->width);
}

GpStatus
GdipDrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode,
                int lenght, GpFont *font, RectF *rc, GpStringFormat *fmt, GpBrush *brush)
{
        cairo_matrix_t saved;
        float width = 0, height = 0;
        float *widths_glyphs = NULL;
        float *ppos, w;
        int num_widths, i, current_glyph, nLast = 0;
        float realY = rc->Y + font->sizeInPixels;
        const gunichar2 *pUnicode = (const gunichar2 *)  stringUnicode;
	char *string;
	GpLinePointF *pPoints, *pPoint;
        GpStringFormat *deffmt = NULL;
        GpStringFormat *format = fmt;
        GpGlyphsDetails *glyphs_details;
        int current_glyph_details;
        int lines, line_count, len;
        char *curString;
        GpGlyphsDetails *cur_glyphs_details;
        const gunichar2 *unicode_string;

        if (!graphics || !stringUnicode || !font)
		return InvalidParameter;

        if (!format) {
                GdipStringFormatGetGenericDefault (&deffmt);
                format = deffmt;
        }

        gdip_prepareString (stringUnicode, lenght, format, &string, &glyphs_details, &current_glyph_details);
        len = (int) g_utf8_strlen (string,-1);

        if (brush)
		gdip_brush_setup (graphics, brush);
        else
                cairo_set_rgb_color (graphics->ct, 0., 0., 0.);

        cairo_save (graphics->ct);
        cairo_set_font (graphics->ct, (cairo_font_t*) font->cairofnt);

        /* Save font matrix */
        cairo_matrix_copy (&saved, (const cairo_matrix_t *)&font->cairofnt->base.matrix);

        /* Get string widths */
        gdip_measure_string_widths (font, string, &widths_glyphs, &num_widths, &width, &height);

        if (!rc->Width) {

                GpLinePointF point;

                point.X = rc->X;
                point.Y = rc->Y;
                point.width = rc->Width;
                
		cairo_scale_font (graphics->ct, font->sizeInPixels);
                gdip_draw_linestring(graphics, &point, string, brush, font, glyphs_details);

		g_free (string);

                if (!deffmt)
                        GdipDeleteStringFormat(deffmt);

		cairo_matrix_copy (&font->cairofnt->base.matrix, (const cairo_matrix_t *)&saved);
		cairo_restore (graphics->ct);
		return gdip_get_status (cairo_status (graphics->ct));
        }

        gdip_measure_string_pos (string, len, glyphs_details, font, rc, format, (RectF *) NULL, (int *) NULL,
                &lines, &pPoints);

        /* Draw lines*/
        cairo_scale_font (graphics->ct, font->sizeInPixels);

        pPoint = pPoints;
        ppos = widths_glyphs;
        pUnicode =  unicode_string = g_utf8_to_utf16 ((const gchar *)string, -1, NULL, NULL,NULL);

        cur_glyphs_details = glyphs_details; cur_glyphs_details++;
        for (w=0, line_count = 0, current_glyph=1, cur_glyphs_details = glyphs_details; current_glyph < num_widths+1; ppos++, current_glyph++, cur_glyphs_details++) {

                /* Draw hotkey line*/
                if (cur_glyphs_details->is_hotkey) {
                        gdip_font_drawunderline (graphics, brush,
                                w + rc->X, rc->Y + font->sizeInPixels, *ppos);
                }

                w += *ppos;

                if (((current_glyph+1 < num_widths+1) && (w + *(ppos+1)) <rc->Width))
                        continue;

                curString = (char *) g_utf16_to_utf8 (pUnicode, (glong)current_glyph-nLast, NULL, NULL, NULL);
                gdip_draw_linestring (graphics, pPoint, curString, brush, font,  glyphs_details);

                g_free (curString);

                pUnicode += (current_glyph-nLast);
                nLast = current_glyph;

                line_count++;

                if (line_count > lines)
                        break;

                w = 0;
                pPoint++;
        }

        cairo_matrix_copy (&font->cairofnt->base.matrix, (const cairo_matrix_t *)&saved);

        g_free( (void*)unicode_string);
        g_free (string);
        free (widths_glyphs);
        free (pPoints);
        free (glyphs_details);        
                
        if (!deffmt) GdipDeleteStringFormat(deffmt);

        cairo_restore (graphics->ct);
        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipSetRenderingOrigin (GpGraphics *graphics, int x, int y)
{
        cairo_move_to (graphics->ct, x, y);
        cairo_close_path (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipGetRenderingOrigin (GpGraphics *graphics, int *x, int *y)
{
        double cx, cy;
        cairo_current_point (graphics->ct, &cx, &cy);

        *x = (int) cx;
        *y = (int) cy;

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipGetDpiX (GpGraphics *graphics, float *dpi)
{
	*dpi = gdip_get_display_dpi ();    
}

GpStatus 
GdipGetDpiY (GpGraphics *graphics, float *dpi)
{
	*dpi = gdip_get_display_dpi ();  
}

