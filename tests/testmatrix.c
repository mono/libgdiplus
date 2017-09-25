#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(_WIN32)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
#else
#include <GdiPlusFlat.h>
#endif

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int floatsEqual(float v1, float v2)
{
		return fabs (v1 - v2) < 0.0001;
}

void verifyMatrix (GpMatrix *matrix, REAL e1, REAL e2, REAL e3, REAL e4, REAL e5, REAL e6)
{
		float elements[6];
		GdipGetMatrixElements (matrix, elements);

	if (!floatsEqual (elements[0], e1) ||
			!floatsEqual (elements[1], e2) ||
			!floatsEqual (elements[2], e3) ||
			!floatsEqual (elements[3], e4) ||
			!floatsEqual (elements[4], e5) ||
			!floatsEqual (elements[5], e6)) {

			printf ("Expected matrices to be equal\n");
			printf ("Actual:   %f, %f, %f, %f, %f, %f\n", e1, e2, e3, e4, e5, e6);
			printf ("Expected: %f, %f, %f, %f, %f, %f\n\n", elements[0], elements[1], elements[2], elements[3], elements[4], elements[5]);

			assert (FALSE);
	}
}

static void test_createMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	status = GdipCreateMatrix (&matrix);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);

	// Negative tests.
	status = GdipCreateMatrix (NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_createMatrix2 ()
{
	GpStatus status;
	GpMatrix *matrix;

	status = GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_createMatrix3 ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpRectF rect = {5, 6, 8, 10};
	GpPointF points[3];

	points[0].X = 12;
	points[0].Y = 14;
	points[1].X = -10;
	points[1].Y = 20;
	points[2].X = 8;
	points[2].Y = 0;

	status = GdipCreateMatrix3 (&rect, points, &matrix);
	assert (status == Ok);
	verifyMatrix (matrix, -2.75f, 0.75f, -0.4f, -1.4f, 28.15f, 18.65f);

	// Negative tests.
	status = GdipCreateMatrix3 (NULL, points, &matrix);
	assert (status == InvalidParameter);

	status = GdipCreateMatrix3 (&rect, NULL, &matrix);
	assert (status == InvalidParameter);

	status = GdipCreateMatrix3 (&rect, points, NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_createMatrix3I ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpRect rect = {5, 6, 8, 10};
	GpPoint points[3];

	points[0].X = 12;
	points[0].Y = 14;
	points[1].X = -10;
	points[1].Y = 20;
	points[2].X = 8;
	points[2].Y = 0;

	status = GdipCreateMatrix3I (&rect, points, &matrix);
	assert (status == Ok);
	verifyMatrix (matrix, -2.75f, 0.75f, -0.4f, -1.4f, 28.15f, 18.65f);

	// Negative tests.
	status = GdipCreateMatrix3I (NULL, points, &matrix);
	assert (status == InvalidParameter);

	status = GdipCreateMatrix3I (&rect, NULL, &matrix);
	assert (status == InvalidParameter);

	status = GdipCreateMatrix3I (&rect, points, NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_cloneMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *clonedMatrix;

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);

	status = GdipCloneMatrix (matrix, &clonedMatrix);
	assert (status == Ok);
	assert (clonedMatrix && clonedMatrix != matrix);
	verifyMatrix (clonedMatrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipCloneMatrix (NULL, &clonedMatrix);
	assert (status == InvalidParameter);

	status = GdipCloneMatrix (matrix, NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (clonedMatrix);
}

static void test_deleteMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	status = GdipDeleteMatrix (matrix);
	assert (status == Ok);

	// Negative tests.
	status = GdipDeleteMatrix (NULL);
	assert (status == InvalidParameter);
}

static void test_setMatrixElements ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	status = GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipSetMatrixElements (NULL, 1, 2, 3, 4, 5, 6);
	assert (status == InvalidParameter);
}

static void test_multiplyMatrix ()
{
	GpStatus status;
	GpMatrix *originalTransform;
	GpMatrix *matrix;
	GpMatrix *nonInvertibleMatrix;

	GdipCreateMatrix (&originalTransform);
	GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &matrix);
	GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (originalTransform, 1, 2, 3, 4, 5, 6);
	status = GdipMultiplyMatrix (originalTransform, matrix, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (originalTransform, 10, 13, 22, 29, 40, 52);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (originalTransform, 1, 2, 3, 4, 5, 6);
	status = GdipMultiplyMatrix (originalTransform, matrix, MatrixOrderPrepend);
	assert (status == Ok);
	verifyMatrix (originalTransform, 11, 16, 19, 28, 32, 46);

	// Non invertible matrix.
	GdipSetMatrixElements (originalTransform, 1, 2, 3, 4, 5, 6);
	status = GdipMultiplyMatrix (originalTransform, nonInvertibleMatrix, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (originalTransform, 287, 56, 697, 136, 1154, 246);

	// Negative tests.
	status = GdipMultiplyMatrix (NULL, matrix, MatrixOrderAppend);
	assert (status == InvalidParameter);

	status = GdipMultiplyMatrix (originalTransform, NULL, MatrixOrderAppend);
	assert (status == InvalidParameter);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, MatrixOrderAppend);
	assert (status == ObjectBusy);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, (MatrixOrder)(MatrixOrderPrepend - 1));
	assert (status == ObjectBusy);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, (MatrixOrder)(MatrixOrderAppend + 1));
	assert (status == ObjectBusy);

	status = GdipMultiplyMatrix (originalTransform, NULL, (MatrixOrder)(MatrixOrderPrepend - 1));
	assert (status == InvalidParameter);

	status = GdipMultiplyMatrix (originalTransform, NULL, (MatrixOrder)(MatrixOrderAppend + 1));
	assert (status == InvalidParameter);

	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
}

static void test_translateMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 5, 6, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 10, 12);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 5, 6, MatrixOrderPrepend);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 28, 40);

	// Zero value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 0, 0, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipTranslateMatrix (NULL, 1, 2, MatrixOrderAppend);
	assert (status == InvalidParameter);

	status = GdipTranslateMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	assert (status == InvalidParameter);

	status = GdipTranslateMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_scaleMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0.5, 0.75, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0.5, 0.75, MatrixOrderPrepend);
	assert (status == Ok);
	verifyMatrix (matrix, 0.5, 1, 2.25, 3, 5, 6);

	// Zero value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0, 0, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix,  0, 0, 0, 0, 0, 0);

	// Negative value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -0.5, -0.75, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, -0.5, -1.5, -1.5, -3, -2.5, -4.5);

	// Negative tests.
	status = GdipScaleMatrix (NULL, 1, 2, MatrixOrderAppend);
	assert (status == InvalidParameter);

	status = GdipScaleMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	assert (status == InvalidParameter);

	status = GdipScaleMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_rotateMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 90, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, -2, 1, -4, 3, -6, 5);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 90, MatrixOrderPrepend);
	assert (status == Ok);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);

	// Negative value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -90, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, 2, -1, 4, -3, 6, -5);

	// Large value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 630, MatrixOrderPrepend);
	assert (status == Ok);
	verifyMatrix (matrix, -3, -4, 1, 2, 5, 6);

	// Negative tests.
	status = GdipRotateMatrix (NULL, 45, MatrixOrderAppend);
	assert (status == InvalidParameter);

	status = GdipRotateMatrix (matrix, 45, (MatrixOrder)(MatrixOrderPrepend - 1));
	assert (status == InvalidParameter);

	status = GdipRotateMatrix (matrix, 45, (MatrixOrder)(MatrixOrderAppend + 1));
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_shearMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 1, 2, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, 3, 4, 7, 10, 11, 16);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 1, 2, MatrixOrderPrepend);
	assert (status == Ok);
	verifyMatrix (matrix, 7, 10, 4, 6, 5, 6);

	// Zero value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 0, 0, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, -1, -2, MatrixOrderAppend);
	assert (status == Ok);
	verifyMatrix (matrix, -1, 0, -1, -2, -1, -4);

	// Negative tests.
	status = GdipShearMatrix (NULL, 1, 2, MatrixOrderAppend);
	assert (status == InvalidParameter);

	status = GdipShearMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	assert (status == InvalidParameter);

	status = GdipShearMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_invertMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *nonInvertibleMatrix;

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);

	status = GdipInvertMatrix (matrix);
	assert (status == Ok);
	verifyMatrix (matrix, -2, 1, 1.5, -0.5, 1, -2);

	status = GdipInvertMatrix (matrix);
	assert (status == Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipInvertMatrix (NULL);
	assert (status == InvalidParameter);
	
	status = GdipInvertMatrix (nonInvertibleMatrix);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
}

static void test_transformMatrixPoints ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPointF points[3];

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);

	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assert (status == Ok);
	assert (points[0].X == 12);
	assert (points[0].Y == 16);
	assert (points[1].X == 20);
	assert (points[1].Y == 28);
	assert (points[2].X == 28);
	assert (points[2].Y == 40);

	// Negative tests.
	status = GdipTransformMatrixPoints (NULL, points, 3);
	assert (status == InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, NULL, 3);
	assert (status == InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, points, 0);
	assert (status == InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, points, -1);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_transformMatrixPointsI ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPoint points[3];

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);

	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assert (status == Ok);
	assert (points[0].X == 12);
	assert (points[0].Y == 16);
	assert (points[1].X == 20);
	assert (points[1].Y == 28);
	assert (points[2].X == 28);
	assert (points[2].Y == 40);

	// Negative tests.
	status = GdipTransformMatrixPointsI (NULL, points, 3);
	assert (status == InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_LIBGDIPLUS)
	status = GdipTransformMatrixPointsI (matrix, NULL, 3);
	assert (status == InvalidParameter);
#endif

	status = GdipTransformMatrixPointsI (matrix, points, 0);
	assert (status == InvalidParameter);

	status = GdipTransformMatrixPointsI (matrix, points, -1);
	assert (status == OutOfMemory);

	GdipDeleteMatrix (matrix);
}

static void test_vectorTransformMatrixPoints ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPointF points[3];

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assert (status == Ok);
	assert (points[0].X == 7);
	assert (points[0].Y == 10);
	assert (points[1].X == 15);
	assert (points[1].Y == 22);
	assert (points[2].X == 23);
	assert (points[2].Y == 34);

	// Negative tests.
	status = GdipVectorTransformMatrixPoints (NULL, points, 3);
	assert (status == InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, NULL, 3);
	assert (status == InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, points, 0);
	assert (status == InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, points, -1);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_vectorTransformMatrixPointsI ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPoint points[3];

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assert (status == Ok);
	assert (points[0].X == 7);
	assert (points[0].Y == 10);
	assert (points[1].X == 15);
	assert (points[1].Y == 22);
	assert (points[2].X == 23);
	assert (points[2].Y == 34);

	// Negative tests.
	status = GdipVectorTransformMatrixPointsI (NULL, points, 3);
	assert (status == InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, NULL, 3);
	assert (status == InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, points, 0);
	assert (status == InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, points, -1);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_getMatrixElements ()
{
	GpStatus status;
	GpMatrix *matrix;
	REAL elements[6];

	GdipCreateMatrix (&matrix);

	// Negative tests.
	status = GdipGetMatrixElements (NULL, elements);
	assert (status == InvalidParameter);

	status = GdipGetMatrixElements (matrix, NULL);
	assert (status == InvalidParameter);
}

static void test_isMatrixInvertible ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *nonInvertibleMatrix;
	BOOL isInvertible;

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);

	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assert (status == Ok);
	assert (isInvertible == TRUE);

	status = GdipIsMatrixInvertible (nonInvertibleMatrix, &isInvertible);
	assert (status == Ok);
	assert (isInvertible == FALSE);

	// Negative tests.
	status = GdipIsMatrixInvertible (NULL, &isInvertible);
	assert (status == InvalidParameter);
	
	status = GdipIsMatrixInvertible (matrix, NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
}

static void test_isMatrixIdentity ()
{
	GpStatus status;
	GpMatrix *matrix;
	BOOL isIdentity;

	GdipCreateMatrix (&matrix);
	
	// Exactly identity.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 0, 0);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	assert (status == Ok);
	assert (isIdentity == TRUE);

	// Close to identity.
	GdipSetMatrixElements (matrix, 0.9999f, -0.0001f, 0.0001f, 1.0001f, 0, 0);	
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	assert (status == Ok);
	assert (isIdentity == TRUE);

	// Less close to identity.
	// FIXME: it appears that GDI+ is less precise than GDI+.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	GdipSetMatrixElements (matrix, 0.9998f, -0.0001f, 0.0001f, 1.0001f, 0, 0);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	assert (status == Ok);
	assert (isIdentity == TRUE);
#endif

	// Not identity.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	assert (status == Ok);
	assert (isIdentity == FALSE);

	// Negative tests.
	status = GdipIsMatrixIdentity (NULL, &isIdentity);
	assert (status == InvalidParameter);
	
	status = GdipIsMatrixIdentity (matrix, NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_isMatrixEqual ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *other;
	BOOL isEqual;

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &other);
	
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assert (status == Ok);
	assert (isEqual == TRUE);

	status = GdipIsMatrixEqual (matrix, matrix, &isEqual);
	assert (status == Ok);
	assert (isEqual == TRUE);

	GdipSetMatrixElements (other, 2, 2, 3, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assert (status == Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 3, 3, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assert (status == Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 4, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assert (status == Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 3, 5, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assert (status == Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 3, 4, 6, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assert (status == Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 3, 4, 5, 7);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assert (status == Ok);
	assert (isEqual == FALSE);

	// Negative tests.
	status = GdipIsMatrixEqual (NULL, other, &isEqual);
	assert (status == InvalidParameter);
	
	status = GdipIsMatrixEqual (matrix, NULL, &isEqual);
	assert (status == InvalidParameter);
	
	status = GdipIsMatrixEqual (matrix, other, NULL);
	assert (status == InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (other);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	test_createMatrix ();
	test_createMatrix2 ();
	test_createMatrix3 ();
	test_createMatrix3I ();
	test_cloneMatrix ();
	test_deleteMatrix ();
	test_setMatrixElements ();
	test_multiplyMatrix ();
	test_translateMatrix ();
	test_scaleMatrix ();
	test_rotateMatrix ();
	test_shearMatrix ();
	test_invertMatrix ();
	test_transformMatrixPoints ();
	test_transformMatrixPointsI ();
	test_vectorTransformMatrixPoints ();
	test_vectorTransformMatrixPointsI ();
	test_getMatrixElements ();
	test_isMatrixInvertible ();
	test_isMatrixIdentity ();
	test_isMatrixEqual ();

	GdiplusShutdown (gdiplusToken);
	return 0;
}
