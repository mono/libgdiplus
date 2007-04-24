/**
 * matrix.c
 *
 * Copyright (C) Novell, Inc. 2003-2004, 2007.
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
 * Author: Duncan Mak (duncan@ximian.com)
 * 
 *
 **/

#include "matrix-private.h"

/*
	GDI+ matrix takes 6 elements arranged in 3 rows by 2 columns. The identity matrix is
	
	[1, 0]					[1, 0, 0]
	[0, 1] that is a simplification of 	[0, 1, 0] 
	[0, 0] 					[0, 0, 1]

	Point v1, v2	Matrix: [m11, m12]
				[m21, m22]
				[m31, m32]
	
	Calcutation of X, Y using the previous matrix

	X = v1 * m11 + v2 * m12 + m31
	Y = v1 * m21 + v2 * m22 + m32

	M31 and M32 are used to do matrix translations

*/


/* private functions */


/*
 * In System.Drawing it is often impossible to specify a 'null' matrix. 
 * Instead we supply an empty matrix (i.e. new Matrix ()). However this
 * "empty" matrix can cause a lot of extra calculation in libgdiplus
 * (e.g. invalidating the bitmap) unless we consider it as a special case.
 */
BOOL
gdip_is_matrix_empty (GpMatrix* matrix)
{
	if (!matrix)
		return TRUE;

	/* compare the matrix elements with the empty (no-op) version */
	return (gdip_near_one (matrix->xx) && gdip_near_zero (matrix->yx) &&
		gdip_near_zero (matrix->xy) && gdip_near_one (matrix->yy) &&
		gdip_near_zero (matrix->x0) && gdip_near_zero (matrix->y0));
}


BOOL
gdip_is_matrix_a_translation (GpMatrix *matrix)
{
	if (!matrix)
		return TRUE;

	return ((matrix->xx == 1.0f) && (matrix->yx == 0.0f) &&
		(matrix->xy == 0.0f) && (matrix->yy == 1.0f));
}

/* this helper function avoid GpMatrix allocation and reduce code duplication */
GpStatus
gdip_matrix_init_from_rect_3points (GpMatrix *matrix, const GpRectF *rect, const GpPointF *dstplg)
{
	GpPointF *p, *p0, *p1, *p2;
	double m11, m12, m21, m22;

	if (!matrix || !rect || !dstplg)
		return InvalidParameter;
	if ((rect->Width == 0) || (rect->Height == 0))
		return OutOfMemory;

	p = (GpPointF*) dstplg;
	p0 = p++;
	p1 = p++;
	p2 = p;

	m11 = (p1->X - p0->X) / rect->Width;
	m12 = (p1->Y - p0->Y) / rect->Width;
	m21 = (p2->X - p0->X) / rect->Height;
	m22 = (p2->Y - p0->Y) / rect->Height;

	cairo_matrix_init (matrix, m11, m12, m21, m22, p0->X, p0->Y);
	cairo_matrix_translate (matrix, -rect->X, -rect->Y);
	return Ok;
}

/* public (exported) functions */

// coverity[+alloc : arg-*0]
GpStatus 
GdipCreateMatrix (GpMatrix **matrix)
{
	GpMatrix *result;

	if (!matrix)
		return InvalidParameter;
	
	result = GdipAlloc (sizeof (GpMatrix));
	if (!result)
		return OutOfMemory;

	cairo_matrix_init_identity (result);

	*matrix = result;
	return Ok;
}

// coverity[+alloc : arg-*6]
GpStatus
GdipCreateMatrix2 (float m11, float m12, float m21, float m22, float dx, float dy, GpMatrix **matrix)
{
	GpMatrix *result;

	if (!matrix)
		return InvalidParameter;

	result = GdipAlloc (sizeof (GpMatrix));
	if (!result)
		return OutOfMemory;

	cairo_matrix_init (result, m11, m12, m21, m22, dx, dy);

	*matrix = result;
	return Ok;
}

// coverity[+alloc : arg-*2]
GpStatus
GdipCreateMatrix3 (const GpRectF *rect, const GpPointF *dstplg, GpMatrix **matrix)
{
	GpMatrix *result;
	GpStatus status;

	if (!rect || !dstplg || !matrix)
		return InvalidParameter;

	result = GdipAlloc (sizeof (GpMatrix));
	if (!result)
		return OutOfMemory;

	status = gdip_matrix_init_from_rect_3points (result, rect, dstplg);
	if (status == Ok)
		*matrix = result;
	else
		GdipFree (result);

	return status;
}

// coverity[+alloc : arg-*2]
GpStatus
GdipCreateMatrix3I (const GpRect *rect, const GpPoint *dstplg, GpMatrix **matrix)
{
	GpRectF r;
	GpPointF pts[3];

	if (!rect || !dstplg || !matrix)
		return InvalidParameter;

	r.X = rect->X;
	r.Y = rect->Y;
	r.Width = rect->Width;
	r.Height = rect->Height;

	pts [0].X = dstplg [0].X;
	pts [0].Y = dstplg [0].Y;
	pts [1].X = dstplg [1].X;
	pts [1].Y = dstplg [1].Y;
	pts [2].X = dstplg [2].X;
	pts [2].Y = dstplg [2].Y;

        return GdipCreateMatrix3 (&r, (GpPointF*)&pts, matrix);
}

// coverity[+alloc : arg-*1]
GpStatus
GdipCloneMatrix (GpMatrix *matrix, GpMatrix **cloneMatrix)
{
	GpMatrix *result;

	if (!matrix || !cloneMatrix)
		return InvalidParameter;

	result = GdipAlloc (sizeof (GpMatrix));
	if (!result)
		return OutOfMemory;

        gdip_cairo_matrix_copy (result, matrix);
	
	*cloneMatrix = result;
	return Ok;
}

GpStatus
GdipDeleteMatrix (GpMatrix *matrix)
{
	if (!matrix)
		return InvalidParameter;

	GdipFree (matrix);
        return Ok;
}

GpStatus
GdipSetMatrixElements (GpMatrix *matrix, float m11, float m12, float m21, float m22, float dx, float dy)
{
	if (!matrix)
		return InvalidParameter;
       
	cairo_matrix_init (matrix, m11, m12, m21, m22, dx, dy);
	
	return Ok;
}

GpStatus 
GdipGetMatrixElements (GpMatrix *matrix, float *matrixOut)
{
	if (!matrix || !matrixOut)
		return InvalidParameter;

        matrixOut[0] = (float) matrix->xx;
        matrixOut[1] = (float) matrix->yx;
        matrixOut[2] = (float) matrix->xy;
        matrixOut[3] = (float) matrix->yy;
        matrixOut[4] = (float) matrix->x0;
        matrixOut[5] = (float) matrix->y0;

        return Ok;
}

GpStatus
GdipMultiplyMatrix (GpMatrix *matrix, GpMatrix *matrix2, GpMatrixOrder order)
{
	if (!matrix || !matrix2)
		return InvalidParameter;

        if (order == MatrixOrderAppend)
                cairo_matrix_multiply (matrix, matrix, matrix2);
        else if (order == MatrixOrderPrepend)
                cairo_matrix_multiply (matrix, matrix2, matrix);        
        else
                return InvalidParameter;

	return Ok;
}

GpStatus
GdipTranslateMatrix (GpMatrix *matrix, float offsetX, float offsetY, GpMatrixOrder order)
{
        cairo_matrix_t tmp;
	GpStatus s;

        cairo_matrix_init (&tmp, 1, 0, 0, 1, offsetX, offsetY);
	s = GdipMultiplyMatrix (matrix, &tmp, order);

        return s;
}

GpStatus
GdipScaleMatrix (GpMatrix *matrix, float scaleX, float scaleY, GpMatrixOrder order)
{
        GpMatrix tmp;
	GpStatus s;

        cairo_matrix_init (&tmp, scaleX, 0, 0, scaleY, 0, 0);
	s = GdipMultiplyMatrix (matrix, &tmp, order);        

        return s;
}

GpStatus
GdipRotateMatrix (GpMatrix *matrix, float angle, GpMatrixOrder order)
{
	cairo_matrix_t tmp;
	GpStatus s;
        float rad = angle * DEGTORAD;

        cairo_matrix_init (&tmp, cos (rad), sin (rad), -sin (rad), cos (rad), 0, 0);
	s = GdipMultiplyMatrix (matrix, &tmp, order);


        return s;
}

GpStatus
GdipShearMatrix (GpMatrix *matrix, float shearX, float shearY, GpMatrixOrder order)
{
        cairo_matrix_t tmp;
	GpStatus s;

        cairo_matrix_init (&tmp, 1, shearY, shearX, 1, 0, 0);
	s = GdipMultiplyMatrix (matrix, &tmp, order);

        return s;
}

GpStatus
GdipInvertMatrix (GpMatrix *matrix)
{
	if (!matrix)
		return InvalidParameter;

        return gdip_get_status (
                cairo_matrix_invert (matrix));
}

GpStatus
GdipTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count)
{
        int i;

	if (!matrix || !pts || (count < 1))
		return InvalidParameter;
        
        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                cairo_matrix_transform_point (matrix, &x, &y);

                pts->X = (float) x;
                pts->Y = (float) y;
        }

        return Ok;
}

GpStatus
GdipTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count)
{
        int i;

	if (!matrix || !pts || (count < 1))
		return InvalidParameter;
        
        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                cairo_matrix_transform_point (matrix, &x, &y);

                pts->X = (int) x;
                pts->Y = (int) y;
        }

        return Ok;
}

GpStatus
GdipVectorTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count)
{
        int i;

	if (!matrix || !pts || (count < 1))
		return InvalidParameter;

        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                cairo_matrix_transform_distance (matrix, &x, &y);

                pts->X = (float) x;
                pts->Y = (float) y;
        }

        return Ok;
}

GpStatus
GdipVectorTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count)
{
        int i;
        
	if (!matrix || !pts || (count < 1))
		return InvalidParameter;

        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                cairo_matrix_transform_distance (matrix, &x, &y);

                pts->X = (int) x;
                pts->Y = (int) y;
        }

        return Ok;
}

GpStatus 
GdipIsMatrixInvertible (GpMatrix *matrix, BOOL *result)
{
	cairo_status_t status;
	cairo_matrix_t copy;

	if (!matrix || !result)
		return InvalidParameter;

	gdip_cairo_matrix_copy (&copy, matrix);
	status = cairo_matrix_invert (&copy);

        *result = (status != CAIRO_STATUS_INVALID_MATRIX);
        return Ok;
}

GpStatus
GdipIsMatrixIdentity (GpMatrix *matrix, BOOL *result)
{
	if (!matrix || !result)
		return InvalidParameter;

	/* note: we CAN'T use GdipIsMatrixEqual - as the precision is different */
        *result = gdip_is_matrix_empty (matrix);
        return Ok;
}

GpStatus
GdipIsMatrixEqual (GpMatrix *matrix, GpMatrix *matrix2, BOOL *result)
{
	if (!matrix || !matrix2 || !result)
		return InvalidParameter;

        *result = ((matrix->xx == matrix2->xx) && (matrix->yx == matrix2->yx) && (matrix->xy == matrix2->xy) &&
		(matrix->yy == matrix2->yy) && (matrix->x0 == matrix2->x0) && (matrix->y0 == matrix2->y0));
        return Ok;
}
