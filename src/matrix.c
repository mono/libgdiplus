/**
 * matrix.c
 *
 * Author: Duncan Mak (duncan@ximian.com)
 * 
 * Copyright (C) Novell, Inc. 2003.
 *
 **/

#include <math.h>
#include "gdip.h"

GpStatus 
GdipCreateMatrix (GpMatrix **matrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        *matrix = cairo_matrix_create ();

	g_return_val_if_fail (*matrix != NULL, OutOfMemory);

        return gdip_get_status (
                cairo_matrix_set_affine (*matrix, 1, 0, 0, 1, 0, 0));
}

GpStatus
GdipCreateMatrix2 (float m11, float m12, float m21, float m22, float dx, float dy, GpMatrix **matrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        *matrix = cairo_matrix_create ();
	g_return_val_if_fail (*matrix != NULL, OutOfMemory);

        return gdip_get_status (
                cairo_matrix_set_affine (
                        *matrix, m11, m12, m21, m22, dx, dy));
}

GpStatus
GdipCreateMatrix3 (const GpRectF *rect, const GpPointF *dstplg, GpMatrix **matrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        *matrix = cairo_matrix_create ();
	g_return_val_if_fail (*matrix != NULL, OutOfMemory);

        double m11 = rect->X;
        double m12 = rect->Y;
        double m21 = rect->Width;
        double m22 = rect->Height;
        double dx = dstplg->X;
        double dy = dstplg->Y;

        Status s = cairo_matrix_set_affine (
                        *matrix, m11, m12, m21, m22, dx, dy);

        return gdip_get_status (s);
}

GpStatus
GdipCreateMatrix3I (const GpRect *rect, const GpPoint *dstplg, GpMatrix **matrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        *matrix = cairo_matrix_create ();
	g_return_val_if_fail (*matrix != NULL, OutOfMemory);

        double m11 = rect->X;
        double m12 = rect->Y;
        double m21 = rect->Width;
        double m22 = rect->Height;
        double dx = dstplg->X;
        double dy = dstplg->Y;
        
        Status s = cairo_matrix_set_affine (
                        *matrix, m11, m12, m21, m22, dx, dy);

        return gdip_get_status (s);
}

GpStatus
GdipCloneMatrix (GpMatrix *matrix, GpMatrix **cloneMatrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (cloneMatrix != NULL, InvalidParameter);

        *cloneMatrix = cairo_matrix_create ();
	g_return_val_if_fail (*cloneMatrix != NULL, OutOfMemory);
        
        return gdip_get_status (
                cairo_matrix_copy (*cloneMatrix, matrix));
}

GpStatus
GdipDeleteMatrix (GpMatrix *matrix)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	cairo_matrix_destroy (matrix);

        return Ok;
}

GpStatus
GdipSetMatrixElements (GpMatrix *matrix, float m11, float m12, float m21, float m22, float dx, float dy)
{
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

        return gdip_get_status (
                cairo_matrix_set_affine (matrix, m11, m12, m21, m22, dx, dy));
}

GpStatus 
GdipGetMatrixElements (GpMatrix *matrix, float *matrixOut)
{
        double a, b, c, d, tx, ty;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (matrixOut != NULL, InvalidParameter);
        
        cairo_matrix_get_affine (matrix, &a, &b, &c, &d, &tx, &ty);
        
        matrixOut[0] = (float) a;
        matrixOut[1] = (float) b;
        matrixOut[2] = (float) c;
        matrixOut[3] = (float) d;
        matrixOut[4] = (float) tx;
        matrixOut[5] = (float) ty;

        return Ok;
}

GpStatus
GdipMultiplyMatrix (GpMatrix *matrix, GpMatrix *matrix2, GpMatrixOrder order)
{
        cairo_status_t status;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (matrix2 != NULL, InvalidParameter);

        if (order == MatrixOrderAppend)
                status = cairo_matrix_multiply (matrix, matrix, matrix2);

        else if (order == MatrixOrderPrepend)
                status = cairo_matrix_multiply (matrix, matrix2, matrix);
        
        else
                return GenericError;

        return gdip_get_status (status);
}

static GpMatrix *
set_translate (float offsetX, float offsetY)
{
        GpMatrix *matrix = cairo_matrix_create ();
        cairo_matrix_set_affine (matrix, 1, 0, 0, 1, offsetX, offsetY);
        return matrix;
}

GpStatus
GdipTranslateMatrix (GpMatrix *matrix, float offsetX, float offsetY, GpMatrixOrder order)
{
        GpMatrix *tmp = set_translate (offsetX, offsetY);
        GpStatus s = GdipMultiplyMatrix (matrix, tmp, order);
        GdipDeleteMatrix (tmp);

        return s;
}

static GpMatrix *
set_scale (float scaleX, float scaleY)
{
        GpMatrix *matrix = cairo_matrix_create ();
        cairo_matrix_set_affine (matrix, scaleX, 0, 0, scaleY, 0, 0);
        return matrix;
}

GpStatus
GdipScaleMatrix (GpMatrix *matrix, float scaleX, float scaleY, GpMatrixOrder order)
{
        GpMatrix *tmp = set_scale (scaleX, scaleY);
        GpStatus s = GdipMultiplyMatrix (matrix, tmp, order);
        GdipDeleteMatrix (tmp);

        return s;
}

static GpMatrix *
set_rotate (float angle)
{
        float rad = angle * DEGTORAD;
        GpMatrix *matrix = cairo_matrix_create ();
        cairo_matrix_set_affine (matrix, cos (rad), sin (rad), -sin (rad), cos (rad), 0, 0);

        return matrix;
}

GpStatus
GdipRotateMatrix (GpMatrix *matrix, float angle, GpMatrixOrder order)
{
        GpMatrix *tmp = set_rotate (angle);
        GpStatus s = GdipMultiplyMatrix (matrix, tmp, order);
        GdipDeleteMatrix (tmp);

        return s;
}

static GpMatrix *
set_shear (float shearX, float shearY)
{
        GpMatrix *matrix = cairo_matrix_create ();
        cairo_matrix_set_affine (matrix, 1, shearX, shearY, 1, 0, 0);
        return matrix;
}

GpStatus
GdipShearMatrix (GpMatrix *matrix, float shearX, float shearY, GpMatrixOrder order)
{
        GpMatrix *tmp = set_shear (shearX, shearY);
        GpStatus s = GdipMultiplyMatrix (matrix, tmp, order);
        GdipDeleteMatrix (tmp);

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
        cairo_status_t status;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);
        
        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                status = cairo_matrix_transform_point (matrix, &x, &y);
                if (status != CAIRO_STATUS_SUCCESS)
                        return gdip_get_status (status);

                pts->X = (float) x;
                pts->Y = (float) y;
        }

        return Ok;
}

GpStatus
GdipTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count)
{
        int i;
        cairo_status_t status;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);
        
        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                status = cairo_matrix_transform_point (matrix, &x, &y);
                if (status != CAIRO_STATUS_SUCCESS)
                        return gdip_get_status (status);
                pts->X = (int) x;
                pts->Y = (int) y;
        }

        return Ok;
}

GpStatus
GdipVectorTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count)
{
        int i;
        cairo_status_t status;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);
        
        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                status = cairo_matrix_transform_distance (matrix, &x, &y);
                if (status != CAIRO_STATUS_SUCCESS)
                        return gdip_get_status (status);
                pts->X = (float) x;
                pts->Y = (float) y;
        }

        return Ok;
}

GpStatus
GdipVectorTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count)
{
        int i;
        cairo_status_t status;
        
	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (pts != NULL, InvalidParameter);

        for (i = 0; i < count; i++, pts++) {
                double x = pts->X;
                double y = pts->Y;
                status = cairo_matrix_transform_distance (matrix, &x, &y);
                if (status != CAIRO_STATUS_SUCCESS)
                        return gdip_get_status (status);

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

        *result = 1;
        return Ok;
}

static bool
matrix_equals (GpMatrix *x, GpMatrix *y)
{
        double ax, bx, cx, dx, ex, fx;
        double ay, by, cy, dy, ey, fy;

        cairo_matrix_get_affine (x, &ax, &bx, &cx, &dx, &ex, &fx);
        cairo_matrix_get_affine (y, &ay, &by, &cy, &dy, &ey, &fy);

        if ((ax != ay) || (bx != by) || (cx != cy) ||
            (dx != dy) || (ex != ey) || (fx != fy))
                return FALSE;

        return TRUE;
}

GpStatus
GdipIsMatrixIdentity (GpMatrix *matrix, int *result)
{
	Status s;
        GpMatrix *identity = cairo_matrix_create ();

	g_return_val_if_fail (identity != NULL, OutOfMemory);
        cairo_matrix_set_identity (identity);

        s = GdipIsMatrixEqual (matrix, identity, result);
        GdipDeleteMatrix (identity);

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
