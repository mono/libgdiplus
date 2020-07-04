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
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (expectedHeight, height);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (expectedWidth, width);

	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	ASSERT_EQ (Ok, status);
	assert (isFilled == expectedIsFilled);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, middleInset);

	status = GdipGetCustomLineCapType ((GpCustomLineCap *) cap, &capType);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CustomLineCapTypeAdjustableArrow, capType);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (expectedWidth == 0 ? 0 : (expectedHeight / expectedWidth), baseInset);

	status = GdipGetCustomLineCapStrokeCaps ((GpCustomLineCap *) cap, &startCap, &endCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapFlat, startCap);
	ASSERT_EQ (LineCapFlat, endCap);

	status = GdipGetCustomLineCapStrokeJoin ((GpCustomLineCap *) cap, &strokeJoin);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineJoinMiter, strokeJoin);

	status = GdipGetCustomLineCapWidthScale ((GpCustomLineCap *) cap, &widthScale);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, widthScale);
}

TEST(AdjustableArrowCap, GdipCreateAdjustableArrowCap)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;

	status = GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, 10, 11, TRUE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (0, 0, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, 0, 0, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (2, 0, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, 2, 0, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (0, 2, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, 0, 2, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (-1, -2, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, -1, -2, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (NAN, -2, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, NAN, -2, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (1, NAN, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, 1, NAN, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (-INFINITY, 1, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, -INFINITY, 1, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	status = GdipCreateAdjustableArrowCap (1, INFINITY, FALSE, &cap);
	ASSERT_EQ (Ok, status);
	verifyArrowCap (cap, 1, INFINITY, FALSE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	// Negative tests.
	status = GdipCreateAdjustableArrowCap (10, 11, TRUE, NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipCloneCustomLineCap)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	GpCustomLineCap *clonedCap;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipCloneCustomLineCap ((GpCustomLineCap *) cap, &clonedCap);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (clonedCap && clonedCap != cap);
	verifyArrowCap ((GpAdjustableArrowCap *) clonedCap, 10, 11, TRUE);
	GdipDeleteCustomLineCap ((GpCustomLineCap *) clonedCap);

	// Negative tests.
	status = GdipCloneCustomLineCap (NULL, &clonedCap);
	ASSERT_EQ (InvalidParameter, status);

	// This causes a null pointer reference in GDI+.
#if !defined(_WIN32)
	status = GdipCloneCustomLineCap (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipDeleteCustomLineCap)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	status = GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDeleteCustomLineCap (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipSetAdjustableArrowCapHeight)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL height;
	REAL baseInset;
	LineCap baseCap;

	GdipCreateAdjustableArrowCap (150, 3, TRUE, &cap);

	// Positive value.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapHeight (cap, 30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (30, height);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, baseInset);

	// Same value.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapHeight (cap, 30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (30, height);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (120, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapRound, baseCap);

	// Zero.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapHeight (cap, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, height);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Negative value.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapHeight (cap, -30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapHeight (cap, &height);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-30, height);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-10, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Negative tests.
	status = GdipSetAdjustableArrowCapHeight (NULL, 30);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipGetAdjustableArrowCapHeight)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL height;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapHeight (NULL, &height);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAdjustableArrowCapHeight (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipSetAdjustableArrowCapWidth)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL width;
	REAL baseInset;
	LineCap baseCap;

	GdipCreateAdjustableArrowCap (150, 3, TRUE, &cap);

	// Positive value.
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	status = GdipSetAdjustableArrowCapWidth (cap, 30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (30, width);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (5, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Same value.
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	status = GdipSetAdjustableArrowCapWidth (cap, 30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (30, width);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (120, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapRound, baseCap);

	// Zero.
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	status = GdipSetAdjustableArrowCapWidth (cap, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, width);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Negative value.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	status = GdipSetAdjustableArrowCapWidth (cap, -30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapWidth (cap, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-30, width);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-5, baseInset);

	// Negative tests.
	status = GdipSetAdjustableArrowCapWidth (NULL, 30);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipGetAdjustableArrowCapWidth)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL width;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapWidth (NULL, &width);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAdjustableArrowCapWidth (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipSetAdjustableArrowCapMiddleInset)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL middleInset;
	REAL baseInset;
	LineCap baseCap;

	GdipCreateAdjustableArrowCap (150, 3, TRUE, &cap);

	// Positive value.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapMiddleInset (cap, 30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (30, middleInset);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (50, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Same value.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapMiddleInset (cap, 30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (30, middleInset);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (120, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapRound, baseCap);

	// Zero.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapMiddleInset (cap, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, middleInset);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (50, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Negative value.
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapMiddleInset (cap, -30);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, &middleInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-30, middleInset);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (50, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Negative tests.
	status = GdipSetAdjustableArrowCapMiddleInset (NULL, 30);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipGetAdjustableArrowCapMiddleInset)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	REAL middleInset;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapMiddleInset (NULL, &middleInset);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAdjustableArrowCapMiddleInset (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipSetAdjustableArrowCapFillState)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	BOOL isFilled;
	REAL baseInset;
	LineCap baseCap;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// FALSE
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapFillState (cap, FALSE);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	ASSERT_EQ (Ok, status);
	ASSERT_FALSE (isFilled);

	// TRUE
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapFillState (cap, TRUE);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isFilled);

	// Same value
	GdipSetCustomLineCapBaseInset ((GpCustomLineCap *) cap, 120);
	GdipSetCustomLineCapBaseCap ((GpCustomLineCap *) cap, LineCapRound);
	status = GdipSetAdjustableArrowCapFillState (cap, TRUE);
	ASSERT_EQ (Ok, status);

	status = GdipGetAdjustableArrowCapFillState (cap, &isFilled);
	ASSERT_EQ (Ok, status);
	ASSERT_TRUE (isFilled);

	status = GdipGetCustomLineCapBaseInset ((GpCustomLineCap *) cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (120, baseInset);

	status = GdipGetCustomLineCapBaseCap ((GpCustomLineCap *) cap, &baseCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapRound, baseCap);

	// Negative tests.
	status = GdipSetAdjustableArrowCapFillState (NULL, 30);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}

TEST(AdjustableArrowCap, GdipGetAdjustableArrowCapFillState)
{
	STARTUP

	GpStatus status;
	GpAdjustableArrowCap *cap;
	BOOL isFilled;

	GdipCreateAdjustableArrowCap (10, 11, TRUE, &cap);

	// Negative tests.
	status = GdipGetAdjustableArrowCapFillState (NULL, &isFilled);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAdjustableArrowCapFillState (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteCustomLineCap ((GpCustomLineCap *) cap);

	SHUTDOWN
}
