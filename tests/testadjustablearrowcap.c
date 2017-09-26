#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(_WIN32)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus")
#else
#include <GdiPlusFlat.h>
#endif

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

static int floatsEqual (float v1, float v2)
{
	if (isnan (v1))
		return isnan (v2);

	if (isinf (v1))
		return isinf (v2);

	return fabs (v1 - v2) < 0.0001;
}

static void verifyArrowCap (GpAdjustableArrowCap *cap, REAL expectedHeight, REAL expectedWidth, BOOL expectedIsFilled)
{
	assert (cap && "Expected cap to be initialized.");

	GpStatus status;
	REAL height;
	REAL width;
	BOOL isFilled;
	REAL middleInset;
	CustomLineCapType capType;
	LineCap baseCap;
	REAL baseInset;
	LineCap startCap;
	LineCap endCap;
	LineJoin strokeJoin;
	REAL widthScale;

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	assert (status == Ok);
	assert (floatsEqual (height, expectedHeight));

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	assert (status == Ok);
	assert (floatsEqual (width, expectedWidth));

	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	assert (status == Ok);
	assert (isFilled == expectedIsFilled);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	assert (status == Ok);
	assert (middleInset == 0);

	status = GdipGetCustomLineCapType ((GpCustomLineCap *) cap, &capType);
	assert (status == Ok);
	assert (capType == CustomLineCapTypeAdjustableArrow);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	assert (status == Ok);
	assert (baseCap == LineCapTriangle);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	assert (status == Ok);
	if (expectedWidth == 0)
		assert (baseInset == 0);
	else
		assert (floatsEqual (baseInset, (expectedHeight / expectedWidth)));

	status = GdipGetCustomLineCapStrokeCaps ((GpCustomLineCap *) cap, &startCap, &endCap);
	assert (status == Ok);
	assert (startCap == LineCapFlat);
	assert (endCap == LineCapFlat);

	status = GdipGetCustomLineCapStrokeJoin ((GpCustomLineCap *) cap, &strokeJoin);
	assert (status == Ok);
	assert (strokeJoin == LineJoinMiter);

	status = GdipGetCustomLineCapWidthScale ((GpCustomLineCap *) cap, &widthScale);
	assert (status == Ok);
	assert (widthScale == 1);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_createAdjustableArrowCap ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;

	status = GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, 10, 11, TRUE);

	status = GdipCreateAdjustableArrowCap (0, 0, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, 0, 0, FALSE);

	status = GdipCreateAdjustableArrowCap (2, 0, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, 2, 0, FALSE);

	status = GdipCreateAdjustableArrowCap (0, 2, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, 0, 2, FALSE);

	status = GdipCreateAdjustableArrowCap (-1, -2, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, -1, -2, FALSE);

	status = GdipCreateAdjustableArrowCap (NAN, -2, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, NAN, -2, FALSE);

	status = GdipCreateAdjustableArrowCap (1, NAN, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, 1, NAN, FALSE);

	status = GdipCreateAdjustableArrowCap (-INFINITY, 1, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, -INFINITY, 1, FALSE);

	status = GdipCreateAdjustableArrowCap (1, INFINITY, FALSE, &cap);
	assert (status == Ok);
	verifyArrowCap (cap, 1, INFINITY, FALSE);

	// Negative tests.
	status = GdipCreateAdjustableArrowCap (10, 11, TRUE, NULL);
	assert (status == InvalidParameter);
}

static void test_cloneAdjustableArrowCap ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	GpCustomLineCap *clonedCap;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipCloneCustomLineCap ((GpCustomLineCap *) cap, &clonedCap);
	assert (status == Ok);
	assert (clonedCap && clonedCap != cap);
	verifyArrowCap ((GpAdjustableArrowCap *) clonedCap, 10, 11, TRUE);

	// Negative tests.
	status = GdipCloneCustomLineCap (NULL, &clonedCap);
	assert (status == InvalidParameter);

	// This causes a null pointer reference in GDI+.
#if !defined(_WIN32)
	status = GdipCloneCustomLineCap (cap, NULL);
	assert (status == InvalidParameter);
#endif

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_deleteAdjustableArrowCap ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
	assert (status == Ok);

	// Negative tests.
	status = GdipDeleteCustomLineCap (NULL);
	assert (status == InvalidParameter);
}

static void test_setAdjustableArrowCapHeight ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL height;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipSetAdjustableArrowCapHeight (cap, 30);
	assert (status == Ok);
	
	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	assert (status == Ok);
	assert (height == 30);

	// Negative tests.
	status = GdipSetAdjustableArrowCapHeight (NULL, 30);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_getAdjustableArrowCapHeight ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL height;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapHeight (NULL, &height);
	assert (status == InvalidParameter);

	status = GdipGetAdjustableArrowCapHeight (cap, NULL);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_setAdjustableArrowCapWidth ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL width;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipSetAdjustableArrowCapWidth (cap, 30);
	assert (status == Ok);
	
	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	assert (status == Ok);
	assert (width == 30);

	// Negative tests.
	status = GdipSetAdjustableArrowCapWidth (NULL, 30);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_getAdjustableArrowCapWidth ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL width;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapWidth (NULL, &width);
	assert (status == InvalidParameter);

	status = GdipGetAdjustableArrowCapWidth (cap, NULL);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_setAdjustableArrowCapMiddleInset ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL middleInset;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipSetAdjustableArrowCapMiddleInset (cap, 30);
	assert (status == Ok);
	
	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	assert (status == Ok);
	assert (middleInset == 30);

	// Negative tests.
	status = GdipSetAdjustableArrowCapMiddleInset (NULL, 30);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_getAdjustableArrowCapMiddleInset ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL middleInset;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapMiddleInset (NULL, &middleInset);
	assert (status == InvalidParameter);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, NULL);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_setAdjustableArrowCapFillState ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	BOOL isFilled;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipSetAdjustableArrowCapFillState (cap, FALSE);
	assert (status == Ok);
	
	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	assert (status == Ok);
	assert (isFilled == FALSE);

	// Negative tests.
	status = GdipSetAdjustableArrowCapFillState (NULL, 30);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_getAdjustableArrowCapFillState ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	BOOL isFilled;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapFillState (NULL, &isFilled);
	assert (status == InvalidParameter);

	status = GdipGetAdjustableArrowCapFillState (cap, NULL);
	assert (status == InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

int
main(int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	test_createAdjustableArrowCap ();
	test_cloneAdjustableArrowCap ();
	test_deleteAdjustableArrowCap ();
	test_setAdjustableArrowCapHeight ();
	test_getAdjustableArrowCapHeight ();
	test_setAdjustableArrowCapWidth ();
	test_getAdjustableArrowCapWidth ();
	test_setAdjustableArrowCapMiddleInset ();
	test_getAdjustableArrowCapMiddleInset ();
	test_setAdjustableArrowCapFillState ();
	test_getAdjustableArrowCapFillState ();

	GdiplusShutdown(gdiplusToken);
	return 0;
}
