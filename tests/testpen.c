#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#include <GdiPlusFlat.h>

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <float.h>

static void test_createPen1 ()
{
	GpStatus status;
	GpPen *pen;

	status = GdipCreatePen1 (1, 10, UnitWorld, &pen);
	assert (status == Ok);
	assert (pen);

	status = GdipCreatePen1 (1, 10, UnitWorld, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_width ()
{
	GpStatus status;
	GpPen *pen;
	REAL width;

	status = GdipCreatePen1 (1, 10, UnitWorld, &pen);
	assert (status == Ok);

	status = GdipGetPenWidth (pen, &width);
	assert (status == Ok);
	assert (width == 10);

	status = GdipGetPenWidth (NULL, &width);
	assert (status == InvalidParameter);

	status = GdipGetPenWidth (pen, NULL);
	assert (status == InvalidParameter);

	status = GdipSetPenWidth (pen, -1);
	assert (status == Ok);

	status = GdipGetPenWidth (pen, &width);
	assert (status == Ok);
	assert (width == -1);

	status = GdipSetPenWidth (NULL, 10);
	assert (status == InvalidParameter);

	GdipDeletePen(pen);
}

static void test_multiplyTransform ()
{
	GpStatus status;
	GpPen *pen;
	GpMatrix *invertibleMatrix;
	GpMatrix *nonInvertibleMatrix;

	status = GdipCreatePen1 (1, 10, UnitWorld, &pen);
	assert (status == Ok);

	status = GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &invertibleMatrix);
	assert (status == Ok);

	status = GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
	assert (status == Ok);

	status = GdipMultiplyPenTransform (pen, invertibleMatrix, MatrixOrderAppend);
	assert (status == Ok);

	status = GdipMultiplyPenTransform (pen, invertibleMatrix, MatrixOrderPrepend);
	assert (status == Ok);

	status = GdipMultiplyPenTransform (pen, invertibleMatrix, -1);
	assert (status == Ok);

	status = GdipMultiplyPenTransform (pen, invertibleMatrix, 2);
	assert (status == Ok);

	status = GdipMultiplyPenTransform (pen, NULL, MatrixOrderAppend);
	assert (status == InvalidParameter);

	status = GdipMultiplyPenTransform (pen, nonInvertibleMatrix, MatrixOrderAppend);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteMatrix (invertibleMatrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
}

static void test_delete()
{
	GpStatus status;
	GpPen *pen;

	status = GdipCreatePen1 (1, 10, UnitWorld, &pen);
	assert (status == Ok);

	status = GdipDeletePen (pen);
	assert (status == Ok);
	assert (pen);

	status = GdipDeletePen (NULL);
	assert (status == InvalidParameter);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	test_createPen1 ();
	test_width ();
	test_multiplyTransform ();
	test_delete ();

	GdiplusShutdown (gdiplusToken);
	return 0;
}
