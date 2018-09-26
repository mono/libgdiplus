#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(USE_WINDOWS_GDIPLUS)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
#else
#include <GdiPlusFlat.h>
#endif

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"

static void test_createMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	status = GdipCreateMatrix (&matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);

	// Negative tests.
	status = GdipCreateMatrix (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_createMatrix2 ()
{
	GpStatus status;
	GpMatrix *matrix;

	status = GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	GdipDeleteMatrix (matrix);

	// Negative tests.
	status = GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_createMatrix3 ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpRectF rect = {5, 6, 8, 10};
	GpRectF negativeRect = {-5, -6, -8, -10};
	GpRectF zeroWidthRect = {1, 2, 0, 4};
	GpRectF zeroHeightRect = {1, 2, 3, 0};
	GpPointF points[3] = {
		{12, 14},
		{-10, 20},
		{8, 0}
	};
	GpPointF rectPoints[3] = {
		{5, 6},
		{13, 6},
		{5, 16},
	};
	GpPointF zeroPoints[3] = {
		{0, 0},
		{0, 0},
		{0, 0}
	};

	status = GdipCreateMatrix3 (&rect, points, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -2.75f, 0.75f, -0.4f, -1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&rect, rectPoints, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&rect, zeroPoints, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&negativeRect, points, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 2.75f, -0.75f, 0.4f, 1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&negativeRect, zeroPoints, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	// Negative tests.
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (NULL, points, &matrix);
	assertEqualInt (status, InvalidParameter);
	assert (matrix == (GpMatrix *) 0xCC);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (&rect, NULL, &matrix);
	assertEqualInt (status, InvalidParameter);
	assert (matrix == (GpMatrix *) 0xCC);

	status = GdipCreateMatrix3 (&rect, points, NULL);
	assertEqualInt (status, InvalidParameter);
	
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (&zeroWidthRect, points, &matrix);
	assertEqualInt (status, OutOfMemory);
	assert (!matrix);
	
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (&zeroHeightRect, points, &matrix);
	assertEqualInt (status, OutOfMemory);
	assert (!matrix);
}

static void test_createMatrix3I ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpRect rect = {5, 6, 8, 10};
	GpRect negativeRect = {-5, -6, -8, -10};
	GpRect zeroWidthRect = {1, 2, 0, 4};
	GpRect zeroHeightRect = {1, 2, 3, 0};
	GpPoint points[3] = {
		{12, 14},
		{-10, 20},
		{8, 0}
	};
	GpPoint rectPoints[3] = {
		{5, 6},
		{13, 6},
		{5, 16},
	};
	GpPoint zeroPoints[3] = {
		{0, 0},
		{0, 0},
		{0, 0}
	};

	status = GdipCreateMatrix3I (&rect, points, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -2.75f, 0.75f, -0.4f, -1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&rect, rectPoints, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&rect, zeroPoints, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&negativeRect, points, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 2.75f, -0.75f, 0.4f, 1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&negativeRect, zeroPoints, &matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	// Negative tests.
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (NULL, points, &matrix);
	assertEqualInt (status, InvalidParameter);
	assert (matrix == (GpMatrix *) 0xCC);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (&rect, NULL, &matrix);
	assertEqualInt (status, InvalidParameter);
	assert (matrix == (GpMatrix *) 0xCC);

	status = GdipCreateMatrix3I (&rect, points, NULL);
	assertEqualInt (status, InvalidParameter);
	
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (&zeroWidthRect, points, &matrix);
	assertEqualInt (status, OutOfMemory);
	assert (!matrix);
	
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (&zeroHeightRect, points, &matrix);
	assertEqualInt (status, OutOfMemory);
	assert (!matrix);
}

static void test_cloneMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *clonedMatrix;

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);

	status = GdipCloneMatrix (matrix, &clonedMatrix);
	assertEqualInt (status, Ok);
	assert (clonedMatrix && clonedMatrix != matrix);
	verifyMatrix (clonedMatrix, 1, 2, 3, 4, 5, 6);
	GdipDeleteMatrix (clonedMatrix);

	// Negative tests.
	clonedMatrix = (GpMatrix *) 0xCC;
	status = GdipCloneMatrix (NULL, &clonedMatrix);
	assertEqualInt (status, InvalidParameter);
	assert (clonedMatrix == (GpMatrix *) 0xCC);

	status = GdipCloneMatrix (matrix, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_deleteMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	status = GdipDeleteMatrix (matrix);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDeleteMatrix (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_setMatrixElements ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	status = GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipSetMatrixElements (NULL, 1, 2, 3, 4, 5, 6);
	assertEqualInt (status, InvalidParameter);
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
	assertEqualInt (status, Ok);
	verifyMatrix (originalTransform, 10, 13, 22, 29, 40, 52);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (originalTransform, 1, 2, 3, 4, 5, 6);
	status = GdipMultiplyMatrix (originalTransform, matrix, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (originalTransform, 11, 16, 19, 28, 32, 46);

	// Non invertible matrix.
	GdipSetMatrixElements (originalTransform, 1, 2, 3, 4, 5, 6);
	status = GdipMultiplyMatrix (originalTransform, nonInvertibleMatrix, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (originalTransform, 287, 56, 697, 136, 1154, 246);

	// Negative tests.
	status = GdipMultiplyMatrix (NULL, matrix, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipMultiplyMatrix (originalTransform, NULL, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, MatrixOrderAppend);
	assertEqualInt (status, ObjectBusy);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, ObjectBusy);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, ObjectBusy);

	status = GdipMultiplyMatrix (originalTransform, NULL, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipMultiplyMatrix (originalTransform, NULL, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 10, 12);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 0, 0, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, -5, -6, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 0, 0);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 5, 6, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 28, 40);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 0, 0, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, -5, -6, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, -18, -28);

	// Negative tests.
	status = GdipTranslateMatrix (NULL, 1, 2, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipTranslateMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipTranslateMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0.5, 1.5, 1.5, 3, 2.5, 4.5);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 1, 1, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0, 0, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -1, -1, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -1, -2, -3, -4, -5, -6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -0.5, -0.75, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -0.5, -1.5, -1.5, -3, -2.5, -4.5);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0.5, 0.75, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0.5, 1, 2.25, 3, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 1, 1, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0, 0, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0, 0, 0, 0, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -1, -1, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -1, -2, -3, -4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -0.5, -0.75, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -0.5, -1, -2.25, -3, 5, 6);

	// Negative value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -0.5, -0.75, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -0.5, -1.5, -1.5, -3, -2.5, -4.5);

	// Negative tests.
	status = GdipScaleMatrix (NULL, 1, 2, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipScaleMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipScaleMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_rotateMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 630, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -3, -4, 1, 2, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 360, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 90, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -2, 1, -4, 3, -6, 5);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 0, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -90, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 2, -1, 4, -3, 6, -5);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -360, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -630, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 630, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -3, -4, 1, 2, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 360, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 90, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -90, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -3, -4, 1, 2, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -360, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -630, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);

	// Negative tests.
	status = GdipRotateMatrix (NULL, 45, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipRotateMatrix (matrix, 45, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipRotateMatrix (matrix, 45, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 3, 4, 7, 10, 11, 16);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 0, 0, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, -1, -2, MatrixOrderAppend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -1, 0, -1, -2, -1, -4);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 1, 2, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 7, 10, 4, 6, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 0, 0, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, -1, -2, MatrixOrderPrepend);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -5, -6, 2, 2, 5, 6);

	// Negative tests.
	status = GdipShearMatrix (NULL, 1, 2, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipShearMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipShearMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_invertMatrix ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *nonInvertibleMatrix;

	GdipCreateMatrix (&matrix);
	GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
	
	// Identity matrix.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 0, 0);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);
	
	// Translate only matrix.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 2, 3);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 0, 0, 1, -2, -3);
	
	// Scale only matrix.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 0, 0);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0.5f, 0, 0, 0.25f, 0, 0);
	
	// Shear only matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 0, 0);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -0.2f, 0.4f, 0.6f, -0.2f, 0, 0);
	
	// Translate and scale only matrix.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 2, 3);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 0.5f, 0, 0, 0.25f, -1, -0.75f);
	
	// Translate and shear only matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 2, 3);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -0.2f, 0.4f, 0.6f, -0.2f, -1.4f, -0.2f);
	
	// Scale and shear only matrix.
	GdipSetMatrixElements (matrix, 2, 2, 3, 4, 0, 0);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 2, -1, -1.5, 1, 0, 0);

	// Complex matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, -2, 1, 1.5f, -0.5f, 1, -2);

	status = GdipInvertMatrix (matrix);
	assertEqualInt (status, Ok);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipInvertMatrix (NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipInvertMatrix (nonInvertibleMatrix);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
}

static void test_transformMatrixPoints ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPointF points[3];
	
	// Identity matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 1);
	assertEqualFloat (points[0].Y, 2);
	assertEqualFloat (points[1].X, 3);
	assertEqualFloat (points[1].Y, 4);
	assertEqualFloat (points[2].X, 5);
	assertEqualFloat (points[2].Y, 6);
	
	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 3);
	assertEqualFloat (points[0].Y, 5);
	assertEqualFloat (points[1].X, 5);
	assertEqualFloat (points[1].Y, 7);
	assertEqualFloat (points[2].X, 7);
	assertEqualFloat (points[2].Y, 9);
	
	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 2);
	assertEqualFloat (points[0].Y, 8);
	assertEqualFloat (points[1].X, 6);
	assertEqualFloat (points[1].Y, 16);
	assertEqualFloat (points[2].X, 10);
	assertEqualFloat (points[2].Y, 24);
	
	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 7);
	assertEqualFloat (points[0].Y, 4);
	assertEqualFloat (points[1].X, 15);
	assertEqualFloat (points[1].Y, 10);
	assertEqualFloat (points[2].X, 23);
	assertEqualFloat (points[2].Y, 16);
	
	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 4);
	assertEqualFloat (points[0].Y, 11);
	assertEqualFloat (points[1].X, 8);
	assertEqualFloat (points[1].Y, 19);
	assertEqualFloat (points[2].X, 12);
	assertEqualFloat (points[2].Y, 27);
	
	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 9);
	assertEqualFloat (points[0].Y, 7);
	assertEqualFloat (points[1].X, 17);
	assertEqualFloat (points[1].Y, 13);
	assertEqualFloat (points[2].X, 25);
	assertEqualFloat (points[2].Y, 19);
	
	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 8);
	assertEqualFloat (points[0].Y, 10);
	assertEqualFloat (points[1].X, 18);
	assertEqualFloat (points[1].Y, 22);
	assertEqualFloat (points[2].X, 28);
	assertEqualFloat (points[2].Y, 34);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 12);
	assertEqualFloat (points[0].Y, 16);
	assertEqualFloat (points[1].X, 20);
	assertEqualFloat (points[1].Y, 28);
	assertEqualFloat (points[2].X, 28);
	assertEqualFloat (points[2].Y, 40);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipTransformMatrixPoints (matrix, points, 1);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 1.5);
	assertEqualFloat (points[0].Y, 4.5);

	// Negative tests.
	status = GdipTransformMatrixPoints (NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipTransformMatrixPoints (NULL, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipTransformMatrixPoints (NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipTransformMatrixPoints (NULL, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformMatrixPoints (matrix, points, -1);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_transformMatrixPointsI ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPoint points[3];
	
	// Identity matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 1);
	assertEqualInt (points[0].Y, 2);
	assertEqualInt (points[1].X, 3);
	assertEqualInt (points[1].Y, 4);
	assertEqualInt (points[2].X, 5);
	assertEqualInt (points[2].Y, 6);
	
	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 3);
	assertEqualInt (points[0].Y, 5);
	assertEqualInt (points[1].X, 5);
	assertEqualInt (points[1].Y, 7);
	assertEqualInt (points[2].X, 7);
	assertEqualInt (points[2].Y, 9);
	
	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 2);
	assertEqualInt (points[0].Y, 8);
	assertEqualInt (points[1].X, 6);
	assertEqualInt (points[1].Y, 16);
	assertEqualInt (points[2].X, 10);
	assertEqualInt (points[2].Y, 24);
	
	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 7);
	assertEqualInt (points[0].Y, 4);
	assertEqualInt (points[1].X, 15);
	assertEqualInt (points[1].Y, 10);
	assertEqualInt (points[2].X, 23);
	assertEqualInt (points[2].Y, 16);
	
	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 4);
	assertEqualInt (points[0].Y, 11);
	assertEqualInt (points[1].X, 8);
	assertEqualInt (points[1].Y, 19);
	assertEqualInt (points[2].X, 12);
	assertEqualInt (points[2].Y, 27);
	
	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 9);
	assertEqualInt (points[0].Y, 7);
	assertEqualInt (points[1].X, 17);
	assertEqualInt (points[1].Y, 13);
	assertEqualInt (points[2].X, 25);
	assertEqualInt (points[2].Y, 19);
	
	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 8);
	assertEqualInt (points[0].Y, 10);
	assertEqualInt (points[1].X, 18);
	assertEqualInt (points[1].Y, 22);
	assertEqualInt (points[2].X, 28);
	assertEqualInt (points[2].Y, 34);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 12);
	assertEqualInt (points[0].Y, 16);
	assertEqualInt (points[1].X, 20);
	assertEqualInt (points[1].Y, 28);
	assertEqualInt (points[2].X, 28);
	assertEqualInt (points[2].Y, 40);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipTransformMatrixPointsI (matrix, points, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 2);
	assertEqualInt (points[0].Y, 5);

	// Negative tests.
	status = GdipTransformMatrixPointsI (NULL, points, 3);
	assertEqualInt (status, InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipTransformMatrixPointsI (NULL, NULL, 3);
	assertEqualInt (status, InvalidParameter);
#endif
	
	status = GdipTransformMatrixPointsI (NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipTransformMatrixPointsI (NULL, points, -1);
	assertEqualInt (status, OutOfMemory);
	
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipTransformMatrixPointsI (matrix, NULL, 3);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipTransformMatrixPointsI (matrix, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformMatrixPointsI (matrix, NULL, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipTransformMatrixPointsI (matrix, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipTransformMatrixPointsI (matrix, points, -1);
	assertEqualInt (status, OutOfMemory);

	GdipDeleteMatrix (matrix);
}

static void test_vectorTransformMatrixPoints ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPointF points[3];
	
	// Identity matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 1);
	assertEqualFloat (points[0].Y, 2);
	assertEqualFloat (points[1].X, 3);
	assertEqualFloat (points[1].Y, 4);
	assertEqualFloat (points[2].X, 5);
	assertEqualFloat (points[2].Y, 6);
	
	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 1);
	assertEqualFloat (points[0].Y, 2);
	assertEqualFloat (points[1].X, 3);
	assertEqualFloat (points[1].Y, 4);
	assertEqualFloat (points[2].X, 5);
	assertEqualFloat (points[2].Y, 6);
	
	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 2);
	assertEqualFloat (points[0].Y, 8);
	assertEqualFloat (points[1].X, 6);
	assertEqualFloat (points[1].Y, 16);
	assertEqualFloat (points[2].X, 10);
	assertEqualFloat (points[2].Y, 24);
	
	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 7);
	assertEqualFloat (points[0].Y, 4);
	assertEqualFloat (points[1].X, 15);
	assertEqualFloat (points[1].Y, 10);
	assertEqualFloat (points[2].X, 23);
	assertEqualFloat (points[2].Y, 16);
	
	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 2);
	assertEqualFloat (points[0].Y, 8);
	assertEqualFloat (points[1].X, 6);
	assertEqualFloat (points[1].Y, 16);
	assertEqualFloat (points[2].X, 10);
	assertEqualFloat (points[2].Y, 24);
	
	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 7);
	assertEqualFloat (points[0].Y, 4);
	assertEqualFloat (points[1].X, 15);
	assertEqualFloat (points[1].Y, 10);
	assertEqualFloat (points[2].X, 23);
	assertEqualFloat (points[2].Y, 16);
	
	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 8);
	assertEqualFloat (points[0].Y, 10);
	assertEqualFloat (points[1].X, 18);
	assertEqualFloat (points[1].Y, 22);
	assertEqualFloat (points[2].X, 28);
	assertEqualFloat (points[2].Y, 34);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 7);
	assertEqualFloat (points[0].Y, 10);
	assertEqualFloat (points[1].X, 15);
	assertEqualFloat (points[1].Y, 22);
	assertEqualFloat (points[2].X, 23);
	assertEqualFloat (points[2].Y, 34);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipVectorTransformMatrixPoints (matrix, points, 1);
	assertEqualInt (status, Ok);
	assertEqualFloat (points[0].X, 1.5);
	assertEqualFloat (points[0].Y, 4.5);

	// Negative tests.
	status = GdipVectorTransformMatrixPoints (NULL, points, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipVectorTransformMatrixPoints (NULL, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipVectorTransformMatrixPoints (NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipVectorTransformMatrixPoints (NULL, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPoints (matrix, points, -1);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
}

static void test_vectorTransformMatrixPointsI ()
{
	GpStatus status;
	GpMatrix *matrix;
	GpPoint points[3];
	
	// Identity matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 1);
	assertEqualInt (points[0].Y, 2);
	assertEqualInt (points[1].X, 3);
	assertEqualInt (points[1].Y, 4);
	assertEqualInt (points[2].X, 5);
	assertEqualInt (points[2].Y, 6);
	
	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 1);
	assertEqualInt (points[0].Y, 2);
	assertEqualInt (points[1].X, 3);
	assertEqualInt (points[1].Y, 4);
	assertEqualInt (points[2].X, 5);
	assertEqualInt (points[2].Y, 6);
	
	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 2);
	assertEqualInt (points[0].Y, 8);
	assertEqualInt (points[1].X, 6);
	assertEqualInt (points[1].Y, 16);
	assertEqualInt (points[2].X, 10);
	assertEqualInt (points[2].Y, 24);
	
	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 7);
	assertEqualInt (points[0].Y, 4);
	assertEqualInt (points[1].X, 15);
	assertEqualInt (points[1].Y, 10);
	assertEqualInt (points[2].X, 23);
	assertEqualInt (points[2].Y, 16);
	
	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 2);
	assertEqualInt (points[0].Y, 8);
	assertEqualInt (points[1].X, 6);
	assertEqualInt (points[1].Y, 16);
	assertEqualInt (points[2].X, 10);
	assertEqualInt (points[2].Y, 24);
	
	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 7);
	assertEqualInt (points[0].Y, 4);
	assertEqualInt (points[1].X, 15);
	assertEqualInt (points[1].Y, 10);
	assertEqualInt (points[2].X, 23);
	assertEqualInt (points[2].Y, 16);
	
	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 8);
	assertEqualInt (points[0].Y, 10);
	assertEqualInt (points[1].X, 18);
	assertEqualInt (points[1].Y, 22);
	assertEqualInt (points[2].X, 28);
	assertEqualInt (points[2].Y, 34);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 7);
	assertEqualInt (points[0].Y, 10);
	assertEqualInt (points[1].X, 15);
	assertEqualInt (points[1].Y, 22);
	assertEqualInt (points[2].X, 23);
	assertEqualInt (points[2].Y, 34);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 1);
	assertEqualInt (status, Ok);
	assertEqualInt (points[0].X, 2);
	assertEqualInt (points[0].Y, 5);

	// Negative tests.
	status = GdipVectorTransformMatrixPointsI (NULL, points, 3);
	assertEqualInt (status, InvalidParameter);

	// This causes a null pointer dereference in GDI+.
	status = GdipVectorTransformMatrixPointsI (NULL, NULL, 3);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipVectorTransformMatrixPointsI (NULL, points, 0);
	assertEqualInt (status, InvalidParameter);
	
	status = GdipVectorTransformMatrixPointsI (NULL, points, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, NULL, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, points, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipVectorTransformMatrixPointsI (matrix, points, -1);
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, InvalidParameter);

	status = GdipGetMatrixElements (matrix, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_isMatrixInvertible ()
{
	GpStatus status;
	GpMatrix *matrix;
	BOOL isInvertible;

	GdipCreateMatrix (&matrix);
	
	// Identity matrix.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);
	
	// Translate only.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 2, 3);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);
	
	// Scale only.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);
	
	// Shear only.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);
	
	// Translate and scale only.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 2, 3);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);
	
	// Translate and shear only.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 2, 3);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);
	
	// Scale and shear only.
	GdipSetMatrixElements (matrix, 2, 2, 3, 4, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);

	// Complex matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == TRUE);
	
	// Not invertible.
	GdipSetMatrixElements (matrix, 123, 24, 82, 16, 47, 30);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == FALSE);
	
	// Empty.
	GdipSetMatrixElements (matrix, 0, 0, 0, 0, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	assertEqualInt (status, Ok);
	assert (isInvertible == FALSE);

	// Negative tests.
	status = GdipIsMatrixInvertible (NULL, &isInvertible);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsMatrixInvertible (matrix, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
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
	assertEqualInt (status, Ok);
	assert (isIdentity == TRUE);

	// Close to identity.
	GdipSetMatrixElements (matrix, 0.9999f, -0.0001f, 0.0001f, 1.0001f, 0, 0);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	assertEqualInt (status, Ok);
	assert (isIdentity == TRUE);

	// Less close to identity.
	GdipSetMatrixElements (matrix, 0.9998f, -0.0001f, 0.0001f, 1.0001f, 0, 0);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	assertEqualInt (status, Ok);
	assert (isIdentity == TRUE);

	// Not identity.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	assertEqualInt (status, Ok);
	assert (isIdentity == FALSE);

	// Negative tests.
	status = GdipIsMatrixIdentity (NULL, &isIdentity);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsMatrixIdentity (matrix, NULL);
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, Ok);
	assert (isEqual == TRUE);

	status = GdipIsMatrixEqual (matrix, matrix, &isEqual);
	assertEqualInt (status, Ok);
	assert (isEqual == TRUE);

	GdipSetMatrixElements (other, 2, 2, 3, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assertEqualInt (status, Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 3, 3, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assertEqualInt (status, Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 4, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assertEqualInt (status, Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 3, 5, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assertEqualInt (status, Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 3, 4, 6, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assertEqualInt (status, Ok);
	assert (isEqual == FALSE);

	GdipSetMatrixElements (other, 1, 2, 3, 4, 5, 7);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	assertEqualInt (status, Ok);
	assert (isEqual == FALSE);

	// Negative tests.
	status = GdipIsMatrixEqual (NULL, other, &isEqual);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsMatrixEqual (matrix, NULL, &isEqual);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsMatrixEqual (matrix, other, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (other);
}

int
main (int argc, char**argv)
{
	STARTUP;

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

	SHUTDOWN;
	return 0;
}
