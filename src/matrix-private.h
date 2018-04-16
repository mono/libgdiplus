/*
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
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __MATRIX_PRIVATE_H__
#define __MATRIX_PRIVATE_H__

#include "gdiplus-private.h"

typedef cairo_matrix_t GpMatrix;

#define gdip_matrix_get_x_translation(matrix)	(matrix->x0)
#define gdip_matrix_get_y_translation(matrix)	(matrix->y0)
#define gdip_matrix_get_x_scale(matrix)		(matrix->xx)
#define gdip_matrix_get_y_scale(matrix)		(matrix->yy)
#define gdip_matrix_reverse_order(order)	((order == MatrixOrderPrepend) ? MatrixOrderAppend : MatrixOrderPrepend)
#define gdip_cairo_matrix_copy(m1,m2)		memcpy (m1, m2, sizeof (cairo_matrix_t))

BOOL gdip_is_matrix_a_translation (const GpMatrix *matrix) GDIP_INTERNAL;
BOOL gdip_is_matrix_empty (const GpMatrix* matrix) GDIP_INTERNAL;
GpStatus gdip_matrix_init_from_rect_3points (GpMatrix *matrix, const GpRectF *rect, const GpPointF *dstplg) GDIP_INTERNAL;

#include "matrix.h"

#endif
