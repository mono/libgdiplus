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
 *   Alexandre Pigolkine(pigolkine@gmx.de)
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
	cairo_set_target_drawable ((*graphics)->ct, GDIP_display, dc->physDev->drawable);
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
GdipSaveGraphics(GpGraphics *graphics, unsigned int *state)
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
        gdip_pen_setup (graphics, pen);

        float delta = sweepAngle - startAngle;

        if (delta < 180)
                make_arc (graphics, x, y, width, height, startAngle, sweepAngle);

        else {
                make_arc (graphics, x, y, width, height, startAngle, startAngle + 180);
                make_arc (graphics, x, y, width, height, startAngle + 180, sweepAngle);
        }

        cairo_stroke (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawArcI (GpGraphics *graphics, GpPen *pen, 
                int x, int y, int width, int height, 
                float startAngle, float sweepAngle)
{
        gdip_pen_setup (graphics, pen);

        float delta = sweepAngle - startAngle;

        if (delta < 180)
                make_arc (graphics, x, y, width, height, startAngle, sweepAngle);

        else {
                make_arc (graphics, x, y, width, height, startAngle, startAngle + 180);
                make_arc (graphics, x, y, width, height, startAngle + 180, sweepAngle);
        }

        cairo_stroke (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawBezier (GpGraphics *graphics, GpPen *pen, 
                float x1, float y1, float x2, float y2,
                float x3, float y3, float x4, float y4)
{
        gdip_pen_setup (graphics, pen);        
        
        cairo_move_to (graphics->ct, x1, y1);
        cairo_curve_to (graphics->ct, x2, y2, x3, y3, x4, y4);
        cairo_stroke (graphics->ct);

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

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen,
                GpPoint *points, int count)
{
        int i, j, k;
        
        if (count == 0)
                return Ok;

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

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus 
GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, 
                float x, float y, float width, float height)
{
        gdip_pen_setup (graphics, pen);
        
        make_ellipse (graphics, x, y, width, height);
        cairo_stroke (graphics->ct);

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
	gdip_pen_setup (graphics, pen);

	cairo_move_to (graphics->ct, x1, y1);
	cairo_line_to (graphics->ct, x2, y2);
	cairo_stroke (graphics->ct);

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

GpStatus
GdipDrawPath (GpGraphics *graphics, GpPen *pen, GpPath *path)
{
        int length = path->count;
        int i;

        gdip_pen_setup (graphics, pen);

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

        cairo_stroke (graphics->ct);
        
        return Ok;
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
        gdip_pen_setup (graphics, pen);
        
        make_polygon (graphics, points, count);
        cairo_stroke (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count)
{
        gdip_pen_setup (graphics, pen);
        
        make_polygon_from_integers (graphics, points, count);
        cairo_stroke (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawRectangle (GpGraphics *graphics, GpPen *pen,
                float x, float y, float width, float height)
{
        gdip_pen_setup (graphics, pen);

        cairo_rectangle (graphics->ct, x, y, width, height);
        cairo_stroke (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen,
                int x, int y, int width, int height)
{
        return GdipDrawRectangle (graphics, pen, x, y, width, height);
}

GpStatus
GdipFillEllipse (GpGraphics *graphics, GpBrush *brush,
                float x, float y, float width, float height)
{
        gdip_brush_setup (graphics, brush);
        make_ellipse (graphics, x, y, width, height);
        cairo_fill (graphics->ct);

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
	gdip_brush_setup (graphics, brush);
	cairo_rectangle (graphics->ct, x, y, width, height);
	cairo_fill (graphics->ct);
        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, 
                GpPointF *points, int count, GpFillMode fillMode)
{
        gdip_brush_setup (graphics, brush);
        make_polygon (graphics, points, count);

        cairo_set_fill_rule (
                graphics->ct,
                convert_fill_mode (fillMode));

        cairo_fill (graphics->ct);

        return gdip_get_status (cairo_status (graphics->ct));
}

GpStatus
GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, 
                GpPoint *points, int count, GpFillMode fillMode)
{
        gdip_brush_setup (graphics, brush);
        make_polygon_from_integers (graphics, points, count);

        cairo_set_fill_rule (
                graphics->ct,
                convert_fill_mode (fillMode));
        
        cairo_fill (graphics->ct);

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



static cairo_status_t
_cairo_ft_font_glyph_extents2 (void			*abstract_font,
                              cairo_glyph_t		*glyphs,
                              int			num_glyphs,
			      cairo_text_extents_t	*extents)
{
    int i;
    cairo_ft_font_t *font = abstract_font;
    cairo_point_double_t origin;
    cairo_point_double_t glyph_min, glyph_max;
    cairo_point_double_t total_min, total_max;
    FT_Error error;
    FT_Face face = font->face;
    FT_GlyphSlot glyph = face->glyph;
    FT_Glyph_Metrics *metrics = &glyph->metrics;

    if (num_glyphs == 0)  {
    	extents->x_bearing = 0.0;
	    extents->y_bearing = 0.0;
    	extents->width  = 0.0;
    	extents->height = 0.0;
    	extents->x_advance = 0.0;
    	extents->y_advance = 0.0;

	    return CAIRO_STATUS_SUCCESS;
        }

    origin.x = glyphs[0].x;
    origin.y = glyphs[0].y;

    _install_font_matrix (&font->base.matrix, face);

    for (i = 0; i < num_glyphs; i++)    {
    	error = FT_Load_Glyph (face, glyphs[i].index, FT_LOAD_DEFAULT);
	    /* XXX: What to do in this error case? */
    	if (error)
	        continue;

    	/* XXX: Need to add code here to check the font's FcPattern
               for FC_VERTICAL_LAYOUT and if set get vertBearingX/Y
               instead. This will require that
               cairo_ft_font_create_for_ft_face accept an
               FcPattern. */
    	glyph_min.x = glyphs[i].x + DOUBLE_FROM_26_6 (metrics->horiBearingX);
    	glyph_min.y = glyphs[i].y - DOUBLE_FROM_26_6 (metrics->horiBearingY);
    	glyph_max.x = glyph_min.x + DOUBLE_FROM_26_6 (metrics->width);
    	glyph_max.y = glyph_min.y + DOUBLE_FROM_26_6 (metrics->height);

    	if (i==0) {
	        total_min = glyph_min;
	        total_max = glyph_max;
    	} else {
            total_max.y= glyph_max.y + total_max.y;
            total_max.x= glyph_max.x + total_max.x;

    	} 
    }


    extents->width     = total_max.x;
    extents->height    = total_max.y;
    extents->x_advance = glyphs[i-1].x + DOUBLE_FROM_26_6 (metrics->horiAdvance) - origin.x;
    extents->y_advance = glyphs[i-1].y + 0 - origin.y;

    return CAIRO_STATUS_SUCCESS;
}


static void
_utf8_to_ucs4 (char const *utf8,
               FT_ULong **ucs4,
               size_t *nchars)
{

    int len = 0, step = 0;
    size_t n = 0, alloc = 0;
    FcChar32 u = 0;

    if (ucs4 == NULL || nchars == NULL)
        return;

    len = strlen (utf8);
    alloc = len;
    *ucs4 = malloc (sizeof (FT_ULong) * alloc);
    if (*ucs4 == NULL)
        return;

    while (len && (step = FcUtf8ToUcs4(utf8, &u, len)) > 0)
    {
        if (n == alloc)
        {
            alloc *= 2;
            *ucs4 = realloc (*ucs4, sizeof (FT_ULong) * alloc);
            if (*ucs4 == NULL)
                return;
        }
        (*ucs4)[n++] = u;
        len -= step;
        utf8 += step;
    }
    *nchars = alloc;
}


static void
_install_font_matrix(cairo_matrix_t *matrix, FT_Face face)
{
    cairo_matrix_t normalized;
    double scale_x, scale_y;
    double xx, xy, yx, yy, tx, ty;
    FT_Matrix mat;
    
    _cairo_matrix_compute_scale_factors (matrix, &scale_x, &scale_y);

    cairo_matrix_copy (&normalized, matrix);

    cairo_matrix_scale (&normalized, 1.0 / scale_x, 1.0 / scale_y);
    cairo_matrix_get_affine (&normalized,
                             &xx /* 00 */ , &yx /* 01 */,
                             &xy /* 10 */, &yy /* 11 */,
                             &tx, &ty);

    mat.xx = DOUBLE_TO_16_16(xx);
    mat.xy = -DOUBLE_TO_16_16(xy);
    mat.yx = -DOUBLE_TO_16_16(yx);
    mat.yy = DOUBLE_TO_16_16(yy);

    FT_Set_Transform(face, &mat, NULL);
    FT_Set_Char_Size(face,
		     DOUBLE_TO_26_6(scale_x),
		     DOUBLE_TO_26_6(scale_y),
		     0, 0);
}


static int
_utf8_to_glyphs (cairo_ft_font_t	*font,
		 const unsigned char	*utf8,
		 double			x0,
		 double			y0,
		 cairo_glyph_t		**glyphs,
		 size_t			*nglyphs)
{
    FT_Face face = font->face;
    double x = 0., y = 0.;
    size_t i;
    FT_ULong *ucs4 = NULL;

    _utf8_to_ucs4 (utf8, &ucs4, nglyphs);

    if (ucs4 == NULL)
        return 0;

    *glyphs = (cairo_glyph_t *) malloc ((*nglyphs) * (sizeof (cairo_glyph_t)));
    if (*glyphs == NULL)
    {
        free (ucs4);
        return 0;
    }

    _install_font_matrix (&font->base.matrix, face);

    for (i = 0; i < *nglyphs; i++)
    {
        (*glyphs)[i].index = FT_Get_Char_Index (face, ucs4[i]);
        (*glyphs)[i].x = x0 + x;
        (*glyphs)[i].y = y0 + y;

        FT_Load_Glyph (face, (*glyphs)[i].index, FT_LOAD_DEFAULT);

        x += DOUBLE_FROM_26_6 (face->glyph->advance.x);
        y -= DOUBLE_FROM_26_6 (face->glyph->advance.y);
    }

    free (ucs4);
    return 1;
}




int
gdip_measure_string_widths (GpFont *font,
                                const unsigned char* utf8,
                                float** pwidths, int *nwidths,
                                float* totalwidth, float* totalheight)
{
    cairo_status_t status;
    int i;
    FT_GlyphSlot glyphslot;
    cairo_glyph_t* glyphs = NULL;
    size_t nglyphs;
    *totalwidth = 0;
    *totalheight = 0;
    *nwidths = 0;
    *pwidths = NULL;

    cairo_matrix_t saved;
    cairo_matrix_copy (&saved, (const cairo_matrix_t *)&font->cairofnt->base.matrix);

    cairo_matrix_scale (&font->cairofnt->base.matrix,  font->sizeInPixels,  font->sizeInPixels);

    _utf8_to_glyphs (font->cairofnt, utf8, 0.0, 0.0, &glyphs, &nglyphs);

    *pwidths = malloc (sizeof(float) *nglyphs);
    *nwidths = nglyphs;
    float *pPos = *pwidths;

    if (!nglyphs){
        cairo_matrix_copy (&font->cairofnt->base.matrix, (const cairo_matrix_t *)&saved);
         return 0;
    }
    
    double x, y;
    cairo_ft_font_t* ft = (cairo_ft_font_t *)font->cairofnt;

    glyphslot = ft->face->glyph;
    *totalwidth  = 0;
    *totalheight = 0;

    for (i = 0; i < nglyphs; i++, pPos++)
    {
        FT_Load_Glyph (ft->face, glyphs[i].index, FT_LOAD_DEFAULT);
        FT_Render_Glyph (glyphslot, FT_RENDER_MODE_NORMAL);

        if (glyphslot->bitmap.width == 0){
            glyphslot->bitmap.rows = DOUBLE_FROM_26_6 (ft->face->glyph->advance.y);
            *pPos =  DOUBLE_FROM_26_6 (ft->face->glyph->advance.x);
        }
        else {
            *pPos =  glyphslot->bitmap.width;
        }

        *totalwidth+=*pPos;                              

    }

    printf("width %f\n", *totalwidth);
    cairo_matrix_copy (&font->cairofnt->base.matrix, (const cairo_matrix_t *)&saved);
    return 1;
}


GpStatus 
GdipDrawString (GpGraphics *graphics, const char *stringUnicode,
                int len, GpFont *font, RectF *rc, GpStringFormat *format, GpBrush *brush)
{
        if (!graphics || !stringUnicode || !font || !format)
            return InvalidParameter;
    
        char* string = (char*) g_utf16_to_utf8 ((const gunichar2 *) stringUnicode,
                        (glong)len, NULL, NULL, NULL);

        printf("DrawString: %s, %x, %f\n", string, font, font->sizeInPixels);

        cairo_save (graphics->ct);
    
        if (brush)
            gdip_brush_setup (graphics, brush);    
        else
            cairo_set_rgb_color (graphics->ct, 0., 0., 0.);
            
        cairo_select_font_nondestructive (graphics->ct, font->cairofnt);
        cairo_scale_font (graphics->ct, font->sizeInPixels);

        float width = 0, height = 0;
        float* pwidths = NULL;
        float* pPos;
        int nwidths, i;
        int nGlyp=0;
        float realY = rc->Y + font->sizeInPixels;
        float w = 0, x=rc->X, y=realY;
        int nLast = 0, nLines = 0;
        const gunichar2* pUnicode = (const gunichar2 *)  stringUnicode;
        StringAlignment align;
        StringAlignment lineAlign;

        // Get widths                                            
        gdip_measure_string_widths(font, string, &pwidths, &nwidths, &width, &height);
        printf("RC->x %f, y %f, width->%f, height->%f\n", rc->X, rc->Y, rc->Width, rc->Height);
        printf("width->%f, height->%f\n", width, height);

        pPos = pwidths;
        bool bContinua = TRUE;

        // Determine in which positions the strings have to be drawn
        GpPoint* pPoints = (GpPoint*) malloc(sizeof(GpPointF) * nwidths);
        GpPoint* pPoint = pPoints;
        
        GdipGetStringFormatAlign(format, &align);
        GdipGetStringFormatLineAlign(format, &lineAlign);

        w=*pPos;
        while (1){

            if ((w + *(pPos+1)) >rc->Width ||  bContinua==FALSE) {
                switch(align){
                case StringAlignmentNear:// Left
                    pPoint->X = rc->X;
                    break;
                case StringAlignmentCenter:
                    pPoint->X = rc->X + ((rc->Width+rc->X-w)/2);
                    break;
                case StringAlignmentFar: // Right
                    pPoint->X = rc->X;
                    pPoint->X += (w < rc->Width) ? (rc->Width - w) : 0;
                    break;
                default:
                    break;
                }

                pPoint++;                
                y+=font->sizeInPixels;
                w=0;
                nLines++;
            }

            if (bContinua==FALSE) break;

            pPos++;
            w+=*pPos;
            nGlyp++;

            if (nGlyp >= nwidths) bContinua = FALSE;
        }

        float alignY = realY; // Default, top

        // Determine vertical alignment
        switch(lineAlign){
        case StringAlignmentNear:// Top
            break;
        case StringAlignmentCenter:
            alignY = realY + ((rc->Y+rc->Height - y) /2);
            break;
        case StringAlignmentFar: // Bottom
            alignY = realY + (rc->Y+rc->Height) - y;
            break;
        default:
            break;
        }

        // Setup Y coordinate for every line
        pPoint = pPoints;                
        for (i =0; i < nLines; i++, pPoint++) {            
            pPoint->Y = alignY;
            alignY+=font->sizeInPixels;
        }

        bContinua = TRUE;
        pPos = pwidths;
        w = *pPos;
        nGlyp=0;
        pPoint = pPoints;

        // Draw the strings
        while (1){

            printf("XXX %f %f %f\n",   w, w + *pPos ,rc->Width);

            if ((w + *(pPos+1)) >rc->Width ||  bContinua==FALSE) {

                if (w > rc->Width) *((char *)NULL) = NULL;
                
                char* spiece = (char*)g_utf16_to_utf8 (pUnicode, (glong)nGlyp-nLast, NULL, NULL, NULL);

                printf("piece->%s\n", spiece);

                pUnicode+=(nGlyp-nLast);
                nLast = nGlyp;                
                cairo_move_to (graphics->ct, pPoint->X, pPoint->Y);
                cairo_show_text (graphics->ct, spiece);                       
                g_free(spiece);

                w=0;
                pPoint++;
            }

            if (bContinua==FALSE) break;

            pPos++;
            w+=*pPos;
            nGlyp++;

            if (nGlyp >= nwidths) bContinua = FALSE;
        
        }

        cairo_select_font_nondestructive(graphics->ct, NULL);
        g_free(string);
        free(pwidths);
        free(pPoints);

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

