#if defined(USE_WINDOWS_GDIPLUS)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus")
#else
#include <GdiPlusFlat.h>
#endif

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <gtest/gtest.h>
#include "testhelpers.h"

TEST(Matrix, GdipCreateMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	status = GdipCreateMatrix (&matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);

	// Negative tests.
	status = GdipCreateMatrix (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipCreateMatrix2)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	status = GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);
	GdipDeleteMatrix (matrix);

	// Negative tests.
	status = GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(Matrix, GdipCreateMatrix3)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -2.75f, 0.75f, -0.4f, -1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&rect, rectPoints, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&rect, zeroPoints, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&negativeRect, points, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 2.75f, -0.75f, 0.4f, 1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3 (&negativeRect, zeroPoints, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	// Negative tests.
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (NULL, points, &matrix);
	ASSERT_EQ (InvalidParameter, status);
	assert (matrix == (GpMatrix *) 0xCC);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (&rect, NULL, &matrix);
	ASSERT_EQ (InvalidParameter, status);
	assert (matrix == (GpMatrix *) 0xCC);

	status = GdipCreateMatrix3 (&rect, points, NULL);
	ASSERT_EQ (InvalidParameter, status);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (&zeroWidthRect, points, &matrix);
	ASSERT_EQ (OutOfMemory, status);
	assert (!matrix);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3 (&zeroHeightRect, points, &matrix);
	ASSERT_EQ (OutOfMemory, status);
	assert (!matrix);

	SHUTDOWN
}

TEST(Matrix, GdipCreateMatrix3I)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -2.75f, 0.75f, -0.4f, -1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&rect, rectPoints, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&rect, zeroPoints, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&negativeRect, points, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 2.75f, -0.75f, 0.4f, 1.4f, 28.15f, 18.65f);
	GdipDeleteMatrix (matrix);

	status = GdipCreateMatrix3I (&negativeRect, zeroPoints, &matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);
	GdipDeleteMatrix (matrix);

	// Negative tests.
	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (NULL, points, &matrix);
	ASSERT_EQ (InvalidParameter, status);
	assert (matrix == (GpMatrix *) 0xCC);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (&rect, NULL, &matrix);
	ASSERT_EQ (InvalidParameter, status);
	assert (matrix == (GpMatrix *) 0xCC);

	status = GdipCreateMatrix3I (&rect, points, NULL);
	ASSERT_EQ (InvalidParameter, status);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (&zeroWidthRect, points, &matrix);
	ASSERT_EQ (OutOfMemory, status);
	assert (!matrix);

	matrix = (GpMatrix *) 0xCC;
	status = GdipCreateMatrix3I (&zeroHeightRect, points, &matrix);
	ASSERT_EQ (OutOfMemory, status);
	assert (!matrix);

	SHUTDOWN
}

TEST(Matrix, GdipCloneMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *clonedMatrix;

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);

	status = GdipCloneMatrix (matrix, &clonedMatrix);
	ASSERT_EQ (Ok, status);
	assert (clonedMatrix && clonedMatrix != matrix);
	verifyMatrix (clonedMatrix, 1, 2, 3, 4, 5, 6);
	GdipDeleteMatrix (clonedMatrix);

	// Negative tests.
	clonedMatrix = (GpMatrix *) 0xCC;
	status = GdipCloneMatrix (NULL, &clonedMatrix);
	ASSERT_EQ (InvalidParameter, status);
	assert (clonedMatrix == (GpMatrix *) 0xCC);

	status = GdipCloneMatrix (matrix, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipDeleteMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	status = GdipDeleteMatrix (matrix);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDeleteMatrix (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(Matrix, GdipSetMatrixElements)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	status = GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipSetMatrixElements (NULL, 1, 2, 3, 4, 5, 6);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipMultiplyMatrix)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	verifyMatrix (originalTransform, 10, 13, 22, 29, 40, 52);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (originalTransform, 1, 2, 3, 4, 5, 6);
	status = GdipMultiplyMatrix (originalTransform, matrix, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (originalTransform, 11, 16, 19, 28, 32, 46);

	// Non invertible matrix.
	GdipSetMatrixElements (originalTransform, 1, 2, 3, 4, 5, 6);
	status = GdipMultiplyMatrix (originalTransform, nonInvertibleMatrix, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (originalTransform, 287, 56, 697, 136, 1154, 246);

	// Negative tests.
	status = GdipMultiplyMatrix (NULL, matrix, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipMultiplyMatrix (originalTransform, NULL, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, MatrixOrderAppend);
	ASSERT_EQ (ObjectBusy, status);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (ObjectBusy, status);

	status = GdipMultiplyMatrix (originalTransform, originalTransform, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (ObjectBusy, status);

	status = GdipMultiplyMatrix (originalTransform, NULL, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipMultiplyMatrix (originalTransform, NULL, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);

	SHUTDOWN
}

TEST(Matrix, GdipTranslateMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 5, 6, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 10, 12);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 0, 0, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, -5, -6, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 0, 0);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 5, 6, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 28, 40);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, 0, 0, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipTranslateMatrix (matrix, -5, -6, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, -18, -28);

	// Negative tests.
	status = GdipTranslateMatrix (NULL, 1, 2, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTranslateMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTranslateMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipScaleMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0.5, 0.75, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 1, 1, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0, 0, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0, 0, 0, 0, 0, 0);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -1, -1, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -1, -2, -3, -4, -5, -6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -0.5, -0.75, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -0.5, -1.5, -1.5, -3, -2.5, -4.5);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0.5, 0.75, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0.5, 1, 2.25, 3, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 1, 1, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, 0, 0, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0, 0, 0, 0, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -1, -1, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -1, -2, -3, -4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -0.5, -0.75, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -0.5, -1, -2.25, -3, 5, 6);

	// Negative value.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipScaleMatrix (matrix, -0.5, -0.75, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -0.5, -1.5, -1.5, -3, -2.5, -4.5);

	// Negative tests.
	status = GdipScaleMatrix (NULL, 1, 2, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipScaleMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipScaleMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipRotateMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 630, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -3, -4, 1, 2, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 360, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 90, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -2, 1, -4, 3, -6, 5);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 0, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -90, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 2, -1, 4, -3, 6, -5);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -360, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -630, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 630, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -3, -4, 1, 2, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 360, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, 90, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -90, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -3, -4, 1, 2, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -360, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipRotateMatrix (matrix, -630, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 3, 4, -1, -2, 5, 6);

	// Negative tests.
	status = GdipRotateMatrix (NULL, 45, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipRotateMatrix (matrix, 45, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipRotateMatrix (matrix, 45, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipShearMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;

	GdipCreateMatrix (&matrix);

	// MatrixOrderAppend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 1, 2, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 3, 4, 7, 10, 11, 16);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 0, 0, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, -1, -2, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -1, 0, -1, -2, -1, -4);

	// MatrixOrderPrepend.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 1, 2, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 7, 10, 4, 6, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, 0, 0, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipShearMatrix (matrix, -1, -2, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -5, -6, 2, 2, 5, 6);

	// Negative tests.
	status = GdipShearMatrix (NULL, 1, 2, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipShearMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipShearMatrix (matrix, 1, 2, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipInvertMatrix)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *nonInvertibleMatrix;

	GdipCreateMatrix (&matrix);
	GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);

	// Identity matrix.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 0, 0);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 0, 0, 1, 0, 0);

	// Translate only matrix.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 2, 3);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 0, 0, 1, -2, -3);

	// Scale only matrix.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 0, 0);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0.5f, 0, 0, 0.25f, 0, 0);

	// Shear only matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 0, 0);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -0.2f, 0.4f, 0.6f, -0.2f, 0, 0);

	// Translate and scale only matrix.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 2, 3);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 0.5f, 0, 0, 0.25f, -1, -0.75f);

	// Translate and shear only matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 2, 3);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -0.2f, 0.4f, 0.6f, -0.2f, -1.4f, -0.2f);

	// Scale and shear only matrix.
	GdipSetMatrixElements (matrix, 2, 2, 3, 4, 0, 0);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 2, -1, -1.5, 1, 0, 0);

	// Complex matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, -2, 1, 1.5f, -0.5f, 1, -2);

	status = GdipInvertMatrix (matrix);
	ASSERT_EQ (Ok, status);
	verifyMatrix (matrix, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipInvertMatrix (NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipInvertMatrix (nonInvertibleMatrix);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);

	SHUTDOWN
}

TEST(Matrix, GdipTransformMatrixPoints)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, points[0].X);
	ASSERT_FLOAT_EQ_NAN (2, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (3, points[1].X);
	ASSERT_FLOAT_EQ_NAN (4, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (5, points[2].X);
	ASSERT_FLOAT_EQ_NAN (6, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (3, points[0].X);
	ASSERT_FLOAT_EQ_NAN (5, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (5, points[1].X);
	ASSERT_FLOAT_EQ_NAN (7, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (7, points[2].X);
	ASSERT_FLOAT_EQ_NAN (9, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (2, points[0].X);
	ASSERT_FLOAT_EQ_NAN (8, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (6, points[1].X);
	ASSERT_FLOAT_EQ_NAN (16, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (10, points[2].X);
	ASSERT_FLOAT_EQ_NAN (24, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (7, points[0].X);
	ASSERT_FLOAT_EQ_NAN (4, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (15, points[1].X);
	ASSERT_FLOAT_EQ_NAN (10, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (23, points[2].X);
	ASSERT_FLOAT_EQ_NAN (16, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (4, points[0].X);
	ASSERT_FLOAT_EQ_NAN (11, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (8, points[1].X);
	ASSERT_FLOAT_EQ_NAN (19, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (12, points[2].X);
	ASSERT_FLOAT_EQ_NAN (27, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (9, points[0].X);
	ASSERT_FLOAT_EQ_NAN (7, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (17, points[1].X);
	ASSERT_FLOAT_EQ_NAN (13, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (25, points[2].X);
	ASSERT_FLOAT_EQ_NAN (19, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (8, points[0].X);
	ASSERT_FLOAT_EQ_NAN (10, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (18, points[1].X);
	ASSERT_FLOAT_EQ_NAN (22, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (28, points[2].X);
	ASSERT_FLOAT_EQ_NAN (34, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (12, points[0].X);
	ASSERT_FLOAT_EQ_NAN (16, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (20, points[1].X);
	ASSERT_FLOAT_EQ_NAN (28, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (28, points[2].X);
	ASSERT_FLOAT_EQ_NAN (40, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipTransformMatrixPoints (matrix, points, 1);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1.5, points[0].X);
	ASSERT_FLOAT_EQ_NAN (4.5, points[0].Y);

	// Negative tests.
	status = GdipTransformMatrixPoints (NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (NULL, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (matrix, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (matrix, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (matrix, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (matrix, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPoints (matrix, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipTransformMatrixPointsI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, points[0].X);
	ASSERT_EQ (2, points[0].Y);
	ASSERT_EQ (3, points[1].X);
	ASSERT_EQ (4, points[1].Y);
	ASSERT_EQ (5, points[2].X);
	ASSERT_EQ (6, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (3, points[0].X);
	ASSERT_EQ (5, points[0].Y);
	ASSERT_EQ (5, points[1].X);
	ASSERT_EQ (7, points[1].Y);
	ASSERT_EQ (7, points[2].X);
	ASSERT_EQ (9, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, points[0].X);
	ASSERT_EQ (8, points[0].Y);
	ASSERT_EQ (6, points[1].X);
	ASSERT_EQ (16, points[1].Y);
	ASSERT_EQ (10, points[2].X);
	ASSERT_EQ (24, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (7, points[0].X);
	ASSERT_EQ (4, points[0].Y);
	ASSERT_EQ (15, points[1].X);
	ASSERT_EQ (10, points[1].Y);
	ASSERT_EQ (23, points[2].X);
	ASSERT_EQ (16, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (4, points[0].X);
	ASSERT_EQ (11, points[0].Y);
	ASSERT_EQ (8, points[1].X);
	ASSERT_EQ (19, points[1].Y);
	ASSERT_EQ (12, points[2].X);
	ASSERT_EQ (27, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (9, points[0].X);
	ASSERT_EQ (7, points[0].Y);
	ASSERT_EQ (17, points[1].X);
	ASSERT_EQ (13, points[1].Y);
	ASSERT_EQ (25, points[2].X);
	ASSERT_EQ (19, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (8, points[0].X);
	ASSERT_EQ (10, points[0].Y);
	ASSERT_EQ (18, points[1].X);
	ASSERT_EQ (22, points[1].Y);
	ASSERT_EQ (28, points[2].X);
	ASSERT_EQ (34, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (12, points[0].X);
	ASSERT_EQ (16, points[0].Y);
	ASSERT_EQ (20, points[1].X);
	ASSERT_EQ (28, points[1].Y);
	ASSERT_EQ (28, points[2].X);
	ASSERT_EQ (40, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipTransformMatrixPointsI (matrix, points, 1);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, points[0].X);
	ASSERT_EQ (5, points[0].Y);

	// Negative tests.
	status = GdipTransformMatrixPointsI (NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipTransformMatrixPointsI (NULL, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipTransformMatrixPointsI (NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPointsI (NULL, points, -1);
	ASSERT_EQ (OutOfMemory, status);

#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipTransformMatrixPointsI (matrix, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipTransformMatrixPointsI (matrix, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPointsI (matrix, NULL, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipTransformMatrixPointsI (matrix, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTransformMatrixPointsI (matrix, points, -1);
	ASSERT_EQ (OutOfMemory, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipVectorTransformMatrixPoints)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, points[0].X);
	ASSERT_FLOAT_EQ_NAN (2, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (3, points[1].X);
	ASSERT_FLOAT_EQ_NAN (4, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (5, points[2].X);
	ASSERT_FLOAT_EQ_NAN (6, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, points[0].X);
	ASSERT_FLOAT_EQ_NAN (2, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (3, points[1].X);
	ASSERT_FLOAT_EQ_NAN (4, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (5, points[2].X);
	ASSERT_FLOAT_EQ_NAN (6, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (2, points[0].X);
	ASSERT_FLOAT_EQ_NAN (8, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (6, points[1].X);
	ASSERT_FLOAT_EQ_NAN (16, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (10, points[2].X);
	ASSERT_FLOAT_EQ_NAN (24, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (7, points[0].X);
	ASSERT_FLOAT_EQ_NAN (4, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (15, points[1].X);
	ASSERT_FLOAT_EQ_NAN (10, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (23, points[2].X);
	ASSERT_FLOAT_EQ_NAN (16, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (2, points[0].X);
	ASSERT_FLOAT_EQ_NAN (8, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (6, points[1].X);
	ASSERT_FLOAT_EQ_NAN (16, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (10, points[2].X);
	ASSERT_FLOAT_EQ_NAN (24, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (7, points[0].X);
	ASSERT_FLOAT_EQ_NAN (4, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (15, points[1].X);
	ASSERT_FLOAT_EQ_NAN (10, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (23, points[2].X);
	ASSERT_FLOAT_EQ_NAN (16, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (8, points[0].X);
	ASSERT_FLOAT_EQ_NAN (10, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (18, points[1].X);
	ASSERT_FLOAT_EQ_NAN (22, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (28, points[2].X);
	ASSERT_FLOAT_EQ_NAN (34, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPoints (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (7, points[0].X);
	ASSERT_FLOAT_EQ_NAN (10, points[0].Y);
	ASSERT_FLOAT_EQ_NAN (15, points[1].X);
	ASSERT_FLOAT_EQ_NAN (22, points[1].Y);
	ASSERT_FLOAT_EQ_NAN (23, points[2].X);
	ASSERT_FLOAT_EQ_NAN (34, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipVectorTransformMatrixPoints (matrix, points, 1);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1.5, points[0].X);
	ASSERT_FLOAT_EQ_NAN (4.5, points[0].Y);

	// Negative tests.
	status = GdipVectorTransformMatrixPoints (NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (NULL, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (matrix, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (matrix, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (matrix, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (matrix, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPoints (matrix, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipVectorTransformMatrixPointsI)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, points[0].X);
	ASSERT_EQ (2, points[0].Y);
	ASSERT_EQ (3, points[1].X);
	ASSERT_EQ (4, points[1].Y);
	ASSERT_EQ (5, points[2].X);
	ASSERT_EQ (6, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate only matrix.
	GdipCreateMatrix2 (1, 0, 0, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, points[0].X);
	ASSERT_EQ (2, points[0].Y);
	ASSERT_EQ (3, points[1].X);
	ASSERT_EQ (4, points[1].Y);
	ASSERT_EQ (5, points[2].X);
	ASSERT_EQ (6, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, points[0].X);
	ASSERT_EQ (8, points[0].Y);
	ASSERT_EQ (6, points[1].X);
	ASSERT_EQ (16, points[1].Y);
	ASSERT_EQ (10, points[2].X);
	ASSERT_EQ (24, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (7, points[0].X);
	ASSERT_EQ (4, points[0].Y);
	ASSERT_EQ (15, points[1].X);
	ASSERT_EQ (10, points[1].Y);
	ASSERT_EQ (23, points[2].X);
	ASSERT_EQ (16, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and scale only matrix.
	GdipCreateMatrix2 (2, 0, 0, 4, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, points[0].X);
	ASSERT_EQ (8, points[0].Y);
	ASSERT_EQ (6, points[1].X);
	ASSERT_EQ (16, points[1].Y);
	ASSERT_EQ (10, points[2].X);
	ASSERT_EQ (24, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Translate and shear only matrix.
	GdipCreateMatrix2 (1, 2, 3, 1, 2, 3, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (7, points[0].X);
	ASSERT_EQ (4, points[0].Y);
	ASSERT_EQ (15, points[1].X);
	ASSERT_EQ (10, points[1].Y);
	ASSERT_EQ (23, points[2].X);
	ASSERT_EQ (16, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Scale and shear only matrix.
	GdipCreateMatrix2 (2, 2, 3, 4, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (8, points[0].X);
	ASSERT_EQ (10, points[0].Y);
	ASSERT_EQ (18, points[1].X);
	ASSERT_EQ (22, points[1].Y);
	ASSERT_EQ (28, points[2].X);
	ASSERT_EQ (34, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Complex matrix.
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	points[0].X = 1;
	points[0].Y = 2;
	points[1].X = 3;
	points[1].Y = 4;
	points[2].X = 5;
	points[2].Y = 6;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 3);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (7, points[0].X);
	ASSERT_EQ (10, points[0].Y);
	ASSERT_EQ (15, points[1].X);
	ASSERT_EQ (22, points[1].Y);
	ASSERT_EQ (23, points[2].X);
	ASSERT_EQ (34, points[2].Y);

	GdipDeleteMatrix (matrix);

	// Rounding.
	GdipCreateMatrix2 (1.5, 0, 0, 1.5, 0, 0, &matrix);
	points[0].X = 1;
	points[0].Y = 3;

	status = GdipVectorTransformMatrixPointsI (matrix, points, 1);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, points[0].X);
	ASSERT_EQ (5, points[0].Y);

	// Negative tests.
	status = GdipVectorTransformMatrixPointsI (NULL, points, 3);
	ASSERT_EQ (InvalidParameter, status);

	// This causes a null pointer dereference in GDI+.
	status = GdipVectorTransformMatrixPointsI (NULL, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPointsI (NULL, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPointsI (NULL, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPointsI (matrix, NULL, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPointsI (matrix, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPointsI (matrix, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPointsI (matrix, points, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipVectorTransformMatrixPointsI (matrix, points, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipGetMatrixElements)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;
	REAL elements[6];

	GdipCreateMatrix (&matrix);

	// Negative tests.
	status = GdipGetMatrixElements (NULL, elements);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetMatrixElements (matrix, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipIsMatrixInvertible)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;
	BOOL isInvertible;

	GdipCreateMatrix (&matrix);

	// Identity matrix.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Translate only.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 2, 3);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Scale only.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Shear only.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Translate and scale only.
	GdipSetMatrixElements (matrix, 2, 0, 0, 4, 2, 3);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Translate and shear only.
	GdipSetMatrixElements (matrix, 1, 2, 3, 1, 2, 3);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Scale and shear only.
	GdipSetMatrixElements (matrix, 2, 2, 3, 4, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Complex matrix.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isInvertible);

	// Not invertible.
	GdipSetMatrixElements (matrix, 123, 24, 82, 16, 47, 30);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isInvertible);

	// Empty.
	GdipSetMatrixElements (matrix, 0, 0, 0, 0, 0, 0);
	status = GdipIsMatrixInvertible (matrix, &isInvertible);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isInvertible);

	// Negative tests.
	status = GdipIsMatrixInvertible (NULL, &isInvertible);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipIsMatrixInvertible (matrix, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipIsMatrixIdentity)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;
	BOOL isIdentity;

	GdipCreateMatrix (&matrix);

	// Exactly identity.
	GdipSetMatrixElements (matrix, 1, 0, 0, 1, 0, 0);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isIdentity);

	// Close to identity.
	GdipSetMatrixElements (matrix, 0.9999f, -0.0001f, 0.0001f, 1.0001f, 0, 0);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isIdentity);

	// Less close to identity.
	GdipSetMatrixElements (matrix, 0.9998f, -0.0001f, 0.0001f, 1.0001f, 0, 0);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isIdentity);

	// Not identity.
	GdipSetMatrixElements (matrix, 1, 2, 3, 4, 5, 6);
	status = GdipIsMatrixIdentity (matrix, &isIdentity);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isIdentity);

	// Negative tests.
	status = GdipIsMatrixIdentity (NULL, &isIdentity);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipIsMatrixIdentity (matrix, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);

	SHUTDOWN
}

TEST(Matrix, GdipIsMatrixEqual)
{
	STARTUP

	GpStatus status;
	GpMatrix *matrix;
	GpMatrix *other;
	BOOL isEqual;

	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &other);

	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEqual);

	status = GdipIsMatrixEqual (matrix, matrix, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isEqual);

	GdipSetMatrixElements (other, 2, 2, 3, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEqual);

	GdipSetMatrixElements (other, 1, 3, 3, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEqual);

	GdipSetMatrixElements (other, 1, 2, 4, 4, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEqual);

	GdipSetMatrixElements (other, 1, 2, 3, 5, 5, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEqual);

	GdipSetMatrixElements (other, 1, 2, 3, 4, 6, 6);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEqual);

	GdipSetMatrixElements (other, 1, 2, 3, 4, 5, 7);
	status = GdipIsMatrixEqual (matrix, other, &isEqual);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isEqual);

	// Negative tests.
	status = GdipIsMatrixEqual (NULL, other, &isEqual);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipIsMatrixEqual (matrix, NULL, &isEqual);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipIsMatrixEqual (matrix, other, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (other);

	SHUTDOWN
}
