/**
 * matrix.c
 *
 * Copyright (C) Novell, Inc. 2003-2004.
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

#include <math.h>
#include "gdip.h"

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

GpStatus 
GdipCreateMatrix (GpMatrix **matrix)
{
	g_return_val_if_fail  (matrix != NULL, InvalidParameter);
	
	*matrix = GdipAlloc (sizeof(GpMatrix));
	cairo_matrix_init (*matrix, 1, 0, 0, 1, 0, 0);
	
	return Ok;
}

GpStatus
GdipCreateMatrix2 (float m11, float m12, float m21, float m22, float dx, float dy, GpMatrix **matrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	*matrix = GdipAlloc (sizeof(GpMatrix));
	cairo_matrix_init (*matrix, m11, m12, m21, m22, dx, dy);
	return Ok;
}

GpStatus
GdipCreateMatrix3 (const GpRectF *rect, const GpPointF *dstplg, GpMatrix **matrix)
{
	double m11, m12, m21, m22, dx, dy;

	g_return_val_if_fail (rect != NULL, InvalidParameter);
	g_return_val_if_fail (dstplg != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	m11 = rect->X;
	m12 = rect->Y;
	m21 = rect->Width;
	m22 = rect->Height;
	dx = dstplg->X;
	dy = dstplg->Y;

	*matrix = GdipAlloc (sizeof (GpMatrix));
	cairo_matrix_init (*matrix, m11, m12, m21, m22, dx, dy);

	return Ok;
}

GpStatus
GdipCreateMatrix3I (const GpRect *rect, const GpPoint *dstplg, GpMatrix **matrix)
{
	double m11, m12, m21, m22, dx, dy;

	g_return_val_if_fail (rect != NULL, InvalidParameter);
	g_return_val_if_fail (dstplg != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	m11 = rect->X;
	m12 = rect->Y;
	m21 = rect->Width;
	m22 = rect->Height;
	dx = dstplg->X;
	dy = dstplg->Y;

	*matrix = GdipAlloc (sizeof (GpMatrix));
	cairo_matrix_init (*matrix, m11, m12, m21, m22, dx, dy);

        return Ok;
}

GpStatus
GdipCloneMatrix (GpMatrix *matrix, GpMatrix **cloneMatrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (cloneMatrix != NULL, InvalidParameter);

	*cloneMatrix = GdipAlloc (sizeof (GpMatrix));
        gdip_cairo_matrix_copy (*cloneMatrix, matrix);
	
	return Ok;
}

GpStatus
GdipDeleteMatrix (GpMatrix *matrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	GdipFree (matrix);
        return Ok;
}

GpStatus
GdipSetMatrixElements (GpMatrix *matrix, float m11, float m12, float m21, float m22, float dx, float dy)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
       
	cairo_matrix_init (matrix, m11, m12, m21, m22, dx, dy);
	
	return Ok;
}

GpStatus 
GdipGetMatrixElements (GpMatrix *matrix, float *matrixOut)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (matrixOut != NULL, InvalidParameter);

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
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (matrix2 != NULL, InvalidParameter);

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
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        return gdip_get_status (
                cairo_matrix_invert (matrix));
}

GpStatus
GdipTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count)
{
        int i;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);
	if (count < 1)
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

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);
	if (count < 1)
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

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);
	if (count < 1)
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
        
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);
	if (count < 1)
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
GdipIsMatrixInvertible (GpMatrix *matrix, int *result)
{
        cairo_status_t status;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (result != NULL, InvalidParameter);

	status = cairo_matrix_invert (matrix);

        if (status == CAIRO_STATUS_INVALID_MATRIX)
                *result = 0;
	else
	        *result = 1;

        return Ok;
}

static bool
matrix_equals (GpMatrix *x, GpMatrix *y)
{
	if ((x->xx != y->xx) || (x->yx != y->yx) || (x->xy != y->xy) ||
	    (x->yy != y->yy) || (x->x0 != y->x0) || (x->y0 != y->y0))
	  return FALSE;
	
	return TRUE;
	
}

GpStatus
GdipIsMatrixIdentity (GpMatrix *matrix, int *result)
{
	Status s;
        GpMatrix identity;

        cairo_matrix_init_identity (&identity);

        s = GdipIsMatrixEqual (matrix, &identity, result);

        return s;
}

GpStatus
GdipIsMatrixEqual (GpMatrix *matrix, GpMatrix *matrix2, int *result)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (matrix2 != NULL, InvalidParameter);
	g_return_val_if_fail (result != NULL, InvalidParameter);

        *result = matrix_equals (matrix, matrix2);
        return Ok;
}
