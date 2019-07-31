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
#include "general-private.h"

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
gdip_is_matrix_empty (const GpMatrix* matrix)
{
	if (!matrix)
		return TRUE;

	/* compare the matrix elements with the empty (no-op) version */
	return (gdip_near_one (matrix->xx) && gdip_near_zero (matrix->yx) &&
		gdip_near_zero (matrix->xy) && gdip_near_one (matrix->yy) &&
		gdip_near_zero (matrix->x0) && gdip_near_zero (matrix->y0));
}

/* GDI+ is more restrictive with matrices which contain boundary values (NaN, Infinity) than
   cairo. This helper function helps you detect these special cases. */
BOOL
gdip_is_matrix_with_boundary_values(const GpMatrix* matrix)
{
	if (!matrix)
		return TRUE;

	return isnan(matrix->xx) || isnan(matrix->xy) || isnan(matrix->yx) || isnan(matrix->yy) || isnan(matrix->x0) || isnan(matrix->y0)
		|| isinf(matrix->xx) || isinf(matrix->xy) || isinf(matrix->yx) || isinf(matrix->yy) || isinf(matrix->x0) || isinf(matrix->y0);
}

/* GDI+ maps values between [ FLT_MAX, Infinity [ to FLT_MAX, instead of Infinity */
REAL
gdip_double_to_float(double value)
{
	if (value != INFINITY && value > FLT_MAX)
		return (REAL)FLT_MAX;

	if (value != -INFINITY && value < -FLT_MAX)
		return (REAL)-FLT_MAX;

	return (REAL)value;
}

BOOL
gdip_is_matrix_a_translation (const GpMatrix *matrix)
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
GpStatus WINGDIPAPI
GdipCreateMatrix (GpMatrix **matrix)
{
	GpMatrix *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

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
GpStatus WINGDIPAPI
GdipCreateMatrix2 (REAL m11, REAL m12, REAL m21, REAL m22, REAL dx, REAL dy, GpMatrix **matrix)
{
	GpMatrix *result;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

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
GpStatus WINGDIPAPI
GdipCreateMatrix3 (GDIPCONST GpRectF *rect, GDIPCONST GpPointF *dstplg, GpMatrix **matrix)
{
	GpMatrix *result;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!rect || !dstplg || !matrix)
		return InvalidParameter;

	result = GdipAlloc (sizeof (GpMatrix));
	if (!result)
		return OutOfMemory;

	status = gdip_matrix_init_from_rect_3points (result, rect, dstplg);
	if (status != Ok) {
		GdipFree (result);
		*matrix = NULL;
		return status;
	}
	
	*matrix = result;
	return status;
}

// coverity[+alloc : arg-*2]
GpStatus WINGDIPAPI
GdipCreateMatrix3I (GDIPCONST GpRect *rect, GDIPCONST GpPoint *dstplg, GpMatrix **matrix)
{
	GpRectF rectf;
	GpPointF pts[3];

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!rect || !dstplg || !matrix)
		return InvalidParameter;

	gdip_RectF_from_Rect (rect, &rectf);

	pts [0].X = dstplg [0].X;
	pts [0].Y = dstplg [0].Y;
	pts [1].X = dstplg [1].X;
	pts [1].Y = dstplg [1].Y;
	pts [2].X = dstplg [2].X;
	pts [2].Y = dstplg [2].Y;

	return GdipCreateMatrix3 (&rectf, (GpPointF*)&pts, matrix);
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
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

GpStatus WINGDIPAPI
GdipDeleteMatrix (GpMatrix *matrix)
{
	if (!matrix)
		return InvalidParameter;

	GdipFree (matrix);
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetMatrixElements (GpMatrix *matrix, REAL m11, REAL m12, REAL m21, REAL m22, REAL dx, REAL dy)
{
	if (!matrix)
		return InvalidParameter;

	cairo_matrix_init (matrix, m11, m12, m21, m22, dx, dy);
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetMatrixElements (GDIPCONST GpMatrix *matrix, REAL *matrixOut)
{
	if (!matrix || !matrixOut)
		return InvalidParameter;

	matrixOut[0] = gdip_double_to_float (matrix->xx);
	matrixOut[1] = gdip_double_to_float (matrix->yx);
	matrixOut[2] = gdip_double_to_float (matrix->xy);
	matrixOut[3] = gdip_double_to_float (matrix->yy);
	matrixOut[4] = gdip_double_to_float (matrix->x0);
	matrixOut[5] = gdip_double_to_float (matrix->y0);

	return Ok;
}

GpStatus WINGDIPAPI
GdipMultiplyMatrix (GpMatrix *matrix, GpMatrix *matrix2, GpMatrixOrder order)
{
	if (!matrix || !matrix2)
		return InvalidParameter;

	if (matrix == matrix2)
		return ObjectBusy;

	switch (order) {
	case MatrixOrderAppend:
		cairo_matrix_multiply (matrix, matrix, matrix2);
		break;
	case MatrixOrderPrepend:
		cairo_matrix_multiply (matrix, matrix2, matrix);
		break;
	default:
		return InvalidParameter;
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipTranslateMatrix (GpMatrix *matrix, REAL offsetX, REAL offsetY, GpMatrixOrder order)
{
	cairo_matrix_t tmp;
	cairo_matrix_init_translate (&tmp, offsetX, offsetY);
	return GdipMultiplyMatrix (matrix, &tmp, order);
}

GpStatus WINGDIPAPI
GdipScaleMatrix (GpMatrix *matrix, REAL scaleX, REAL scaleY, GpMatrixOrder order)
{
	cairo_matrix_t tmp;
	cairo_matrix_init_scale (&tmp, scaleX, scaleY);
	return GdipMultiplyMatrix (matrix, &tmp, order);        
}

GpStatus WINGDIPAPI
GdipRotateMatrix (GpMatrix *matrix, REAL angle, GpMatrixOrder order)
{
	cairo_matrix_t tmp;
	cairo_matrix_init_rotate (&tmp, angle * DEGTORAD);
	return GdipMultiplyMatrix (matrix, &tmp, order);
}

GpStatus WINGDIPAPI
GdipShearMatrix (GpMatrix *matrix, REAL shearX, REAL shearY, GpMatrixOrder order)
{
	cairo_matrix_t tmp;
	cairo_matrix_init (&tmp, 1, shearY, shearX, 1, 0, 0);
	return GdipMultiplyMatrix (matrix, &tmp, order);
}

GpStatus WINGDIPAPI
GdipInvertMatrix (GpMatrix *matrix)
{
	if (!matrix || gdip_is_matrix_with_boundary_values(matrix))
		return InvalidParameter;

	return gdip_get_status (cairo_matrix_invert (matrix));
}

GpStatus WINGDIPAPI
GdipTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, INT count)
{
	if (!matrix || !pts || count <= 0)
		return InvalidParameter;

	for (int i = 0; i < count; i++, pts++) {
		double x = pts->X;
		double y = pts->Y;
		cairo_matrix_transform_point (matrix, &x, &y);

		pts->X = (REAL) x;
		pts->Y = (REAL) y;
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, INT count)
{
	if (count < 0)
		return OutOfMemory;
	if (!matrix || !pts || count == 0)
		return InvalidParameter;

	for (int i = 0; i < count; i++, pts++) {
		double x = pts->X;
		double y = pts->Y;
		cairo_matrix_transform_point (matrix, &x, &y);

		pts->X = iround (x);
		pts->Y = iround (y);
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipVectorTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, INT count)
{
	if (!matrix || !pts || count <= 0)
		return InvalidParameter;

	for (int i = 0; i < count; i++, pts++) {
		double x = pts->X;
		double y = pts->Y;
		cairo_matrix_transform_distance (matrix, &x, &y);

		pts->X = (REAL) x;
		pts->Y = (REAL) y;
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipVectorTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, INT count)
{
	if (!matrix || !pts || count <= 0)
		return InvalidParameter;

	for (int i = 0; i < count; i++, pts++) {
		double x = pts->X;
		double y = pts->Y;
		cairo_matrix_transform_distance (matrix, &x, &y);

		pts->X = iround (x);
		pts->Y = iround (y);
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipIsMatrixInvertible (GDIPCONST GpMatrix *matrix, BOOL *result)
{
	cairo_status_t status;
	cairo_matrix_t copy;

	if (!matrix || !result)
		return InvalidParameter;

	if (gdip_is_matrix_with_boundary_values (matrix))
	{
		*result = FALSE;
		return Ok;
	}

	gdip_cairo_matrix_copy (&copy, matrix);
	status = cairo_matrix_invert (&copy);

	*result = (status == CAIRO_STATUS_SUCCESS);
	return Ok;
}

GpStatus WINGDIPAPI
GdipIsMatrixIdentity (GDIPCONST GpMatrix *matrix, BOOL *result)
{
	if (!matrix || !result)
		return InvalidParameter;

	// We can't use GdipIsMatrixEqual as the precision is different.
	*result = gdip_is_matrix_empty (matrix);
	return Ok;
}

GpStatus WINGDIPAPI
GdipIsMatrixEqual (GDIPCONST GpMatrix *matrix, GDIPCONST GpMatrix *matrix2, BOOL *result)
{
	if (!matrix || !matrix2 || !result)
		return InvalidParameter;

	*result = ((matrix->xx == matrix2->xx) && (matrix->yx == matrix2->yx) && (matrix->xy == matrix2->xy) &&
		(matrix->yy == matrix2->yy) && (matrix->x0 == matrix2->x0) && (matrix->y0 == matrix2->y0));
	return Ok;
}
