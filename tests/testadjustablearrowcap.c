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
#include "testhelpers.h"

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

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
	assertEqualInt (status, Ok);
	assertEqualFloat (height, expectedHeight);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, expectedWidth);

	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	assertEqualInt (status, Ok);
	assert (isFilled == expectedIsFilled);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (middleInset, 0);

	status = GdipGetCustomLineCapType ((GpCustomLineCap *) cap, &capType);
	assertEqualInt (status, Ok);
	assertEqualInt (capType, CustomLineCapTypeAdjustableArrow);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	assertEqualInt (status, Ok);
	assertEqualInt (baseCap, LineCapTriangle);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (baseInset, (expectedWidth == 0 ? 0 : (expectedHeight / expectedWidth)));

	status = GdipGetCustomLineCapStrokeCaps ((GpCustomLineCap *) cap, &startCap, &endCap);
	assertEqualInt (status, Ok);
	assertEqualInt (startCap, LineCapFlat);
	assertEqualInt (endCap, LineCapFlat);

	status = GdipGetCustomLineCapStrokeJoin ((GpCustomLineCap *) cap, &strokeJoin);
	assertEqualInt (status, Ok);
	assertEqualInt (strokeJoin, LineJoinMiter);

	status = GdipGetCustomLineCapWidthScale ((GpCustomLineCap *) cap, &widthScale);
	assertEqualInt (status, Ok);
	assertEqualFloat (widthScale, 1);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_createAdjustableArrowCap ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;

	status = GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, 10, 11, TRUE);

	status = GdipCreateAdjustableArrowCap (0, 0, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, 0, 0, FALSE);

	status = GdipCreateAdjustableArrowCap (2, 0, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, 2, 0, FALSE);

	status = GdipCreateAdjustableArrowCap (0, 2, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, 0, 2, FALSE);

	status = GdipCreateAdjustableArrowCap (-1, -2, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, -1, -2, FALSE);

	status = GdipCreateAdjustableArrowCap (NAN, -2, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, NAN, -2, FALSE);

	status = GdipCreateAdjustableArrowCap (1, NAN, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, 1, NAN, FALSE);

	status = GdipCreateAdjustableArrowCap (-INFINITY, 1, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, -INFINITY, 1, FALSE);

	status = GdipCreateAdjustableArrowCap (1, INFINITY, FALSE, &cap);
	assertEqualInt (status, Ok);
	verifyArrowCap (cap, 1, INFINITY, FALSE);

	// Negative tests.
	status = GdipCreateAdjustableArrowCap (10, 11, TRUE, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_cloneAdjustableArrowCap ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	GpCustomLineCap *clonedCap;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipCloneCustomLineCap ((GpCustomLineCap *) cap, &clonedCap);
	assertEqualInt (status, Ok);
	assert (clonedCap && clonedCap != cap);
	verifyArrowCap ((GpAdjustableArrowCap *) clonedCap, 10, 11, TRUE);

	// Negative tests.
	status = GdipCloneCustomLineCap (NULL, &clonedCap);
	assertEqualInt (status, InvalidParameter);

	// This causes a null pointer reference in GDI+.
#if !defined(_WIN32)
	status = GdipCloneCustomLineCap (cap, NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_deleteAdjustableArrowCap ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDeleteCustomLineCap (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_setAdjustableArrowCapHeight ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL height;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Positive value.
	status = GdipSetAdjustableArrowCapHeight (cap, 30);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	assertEqualInt (status, Ok);
	assertEqualFloat (height, 30);

	// Zero.
	status = GdipSetAdjustableArrowCapHeight (cap, 0);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	assertEqualInt (status, Ok);
	assertEqualFloat (height, 0);

	// Negative value.
	status = GdipSetAdjustableArrowCapHeight (cap, -30);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	assertEqualInt (status, Ok);
	assertEqualFloat (height, -30);

	// Negative tests.
	status = GdipSetAdjustableArrowCapHeight (NULL, 30);
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAdjustableArrowCapHeight (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_setAdjustableArrowCapWidth ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL width;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Positive value.
	status = GdipSetAdjustableArrowCapWidth (cap, 30);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, 30);

	// Zero.
	status = GdipSetAdjustableArrowCapWidth (cap, 0);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, 0);

	// Negative value.
	status = GdipSetAdjustableArrowCapWidth (cap, -30);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, -30);

	// Negative tests.
	status = GdipSetAdjustableArrowCapWidth (NULL, 30);
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAdjustableArrowCapWidth (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_setAdjustableArrowCapMiddleInset ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL middleInset;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Positive value.
	status = GdipSetAdjustableArrowCapMiddleInset (cap, 30);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (middleInset, 30);

	// Zero.
	status = GdipSetAdjustableArrowCapMiddleInset (cap, 0);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (middleInset, 0);

	// Negative value.
	status = GdipSetAdjustableArrowCapMiddleInset (cap, -30);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (middleInset, -30);

	// Negative tests.
	status = GdipSetAdjustableArrowCapMiddleInset (NULL, 30);
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
}

static void test_setAdjustableArrowCapFillState ()
{
	GpStatus status;
	GpAdjustableArrowCap *cap;
	BOOL isFilled;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipSetAdjustableArrowCapFillState (cap, FALSE);
	assertEqualInt (status, Ok);

	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	assertEqualInt (status, Ok);
	assert (isFilled == FALSE);

	// Negative tests.
	status = GdipSetAdjustableArrowCapFillState (NULL, 30);
	assertEqualInt (status, InvalidParameter);

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
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAdjustableArrowCapFillState (cap, NULL);
	assertEqualInt (status, InvalidParameter);

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
