#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

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

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "testhelpers.h"

static void verifyCustomLineCap (GpCustomLineCap *cap, LineCap expectedBaseCap, REAL expectedBaseInset)
{
	assert (cap && "Expected cap to be initialized.");

	GpStatus status;
	LineCap baseCap;
	REAL baseInset;
	LineCap startCap;
	LineCap endCap;
	LineJoin strokeJoin;
	REAL widthScale;
	CustomLineCapType type;

	status = GdipGetCustomLineCapBaseCap (cap, &baseCap);
	assertEqualInt (status, Ok);
	assertEqualInt (baseCap, expectedBaseCap);

	status = GdipGetCustomLineCapBaseInset (cap, &baseInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (baseInset, expectedBaseInset);

	status = GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	assertEqualInt (status, Ok);
	assertEqualInt (startCap, LineCapFlat);
	assertEqualInt (endCap, LineCapFlat);

	status = GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (status, Ok);
	assertEqualInt (strokeJoin, LineJoinMiter);

	status = GdipGetCustomLineCapType (cap, &type);
	assertEqualInt (status, Ok);
	assertEqualInt (type, CustomLineCapTypeDefault);

	status = GdipGetCustomLineCapWidthScale (cap, &widthScale);
	assertEqualInt (status, Ok);
	assertEqualFloat (widthScale, 1);
}

static void test_createCustomLineCap ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);

	status = GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);
	assertEqualInt (status, Ok);
	verifyCustomLineCap (cap, LineCapFlat, 10);
	GdipDeleteCustomLineCap (cap);

	status = GdipCreateCustomLineCap (NULL, strokePath, LineCapCustom, 0, &cap);
	assertEqualInt (status, Ok);
	verifyCustomLineCap (cap, LineCapFlat, 0);
	GdipDeleteCustomLineCap (cap);

	status = GdipCreateCustomLineCap (fillPath, NULL, (LineCap)(LineCapFlat - 1), -1, &cap);
	assertEqualInt (status, Ok);
	verifyCustomLineCap (cap, LineCapFlat, -1);
	GdipDeleteCustomLineCap (cap);

	status = GdipCreateCustomLineCap (fillPath, strokePath, (LineCap)(LineCapCustom + 1), 100, &cap);
	assertEqualInt (status, Ok);
	verifyCustomLineCap (cap, LineCapFlat, 100);
	GdipDeleteCustomLineCap (cap);

	// Negative tests.
	cap = (GpCustomLineCap *) 0xCC;
	status = GdipCreateCustomLineCap (NULL, NULL, LineCapRoundAnchor, 10, &cap);
	assertEqualInt (status, InvalidParameter);
	assert (cap == (GpCustomLineCap *) 0xCC);

	status = GdipCreateCustomLineCap (fillPath, fillPath, LineCapRoundAnchor, 10, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
}

static void test_deleteCustomLineCap ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	status = GdipDeleteCustomLineCap (cap);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDeleteCustomLineCap (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
}

static void test_cloneCustomLineCap ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	GpCustomLineCap *clonedCap;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	status = GdipCloneCustomLineCap (cap, &clonedCap);
	assertEqualInt (status, Ok);
	verifyCustomLineCap (clonedCap, LineCapFlat, 10);
	GdipDeleteCustomLineCap (clonedCap);

	// Negative tests.
	status = GdipCloneCustomLineCap (NULL, &clonedCap);
	assertEqualInt (status, InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCloneCustomLineCap (cap, NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_getCustomLineCapType ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	CustomLineCapType type;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Negative tests.
	status = GdipGetCustomLineCapType (NULL, &type);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCustomLineCapType (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_setCustomLineCapStrokeCaps ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	LineCap startCap;
	LineCap endCap;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// LineCapFlat/LineCapSquare.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapSquare);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	assertEqualInt (startCap, LineCapFlat);
	assertEqualInt (endCap, LineCapSquare);

	// LineCapSquare/LineCapFlat.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapSquare, LineCapFlat);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	assertEqualInt (startCap, LineCapSquare);
	assertEqualInt (endCap, LineCapFlat);

	// LineCapRound/LineCapTriangle.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapRound, LineCapTriangle);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	assertEqualInt (startCap, LineCapRound);
	assertEqualInt (endCap, LineCapTriangle);

	// LineCapTriangle/LineCapRound.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapTriangle, LineCapRound);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	assertEqualInt (startCap, LineCapTriangle);
	assertEqualInt (endCap, LineCapRound);

	// Same value.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapTriangle, LineCapRound);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	assertEqualInt (startCap, LineCapTriangle);
	assertEqualInt (endCap, LineCapRound);

	// Negative tests.
	status = GdipSetCustomLineCapStrokeCaps (NULL, LineCapArrowAnchor, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, (LineCap)(LineCapTriangle + 1), LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapNoAnchor, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapSquareAnchor, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapRoundAnchor, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapDiamondAnchor, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapArrowAnchor, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapCustom, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapAnchorMask, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, (LineCap)(LineCapCustom + 1), LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, (LineCap)(LineCapTriangle + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapNoAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapSquareAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapRoundAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapDiamondAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapArrowAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapCustom);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapAnchorMask);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, (LineCap)(LineCapCustom + 1));
	assertEqualInt (status, InvalidParameter);

	// Make sure nothing changed.
	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	assertEqualInt (startCap, LineCapTriangle);
	assertEqualInt (endCap, LineCapRound);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_getCustomLineCapStrokeCaps ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	LineCap startCap;
	LineCap endCap;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Negative tests.
	status = GdipGetCustomLineCapStrokeCaps (NULL, &startCap, &endCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCustomLineCapStrokeCaps (cap, NULL, &endCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCustomLineCapStrokeCaps (cap, &startCap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_setCustomLineCapStrokeJoin ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	LineJoin strokeJoin;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// LineJoinMiter.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinMiter);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (strokeJoin, LineJoinMiter);

	// LineJoinMiter.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinBevel);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (strokeJoin, LineJoinBevel);

	// LineJoinRound.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinRound);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (strokeJoin, LineJoinRound);

	// LineJoinMiterClipped.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinMiterClipped);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (strokeJoin, LineJoinMiterClipped);

	// Same value.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinMiterClipped);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (strokeJoin, LineJoinMiterClipped);

	// Invalid value - negative.
	status = GdipSetCustomLineCapStrokeJoin (cap, (LineJoin)(LineJoinMiter - 1));
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (strokeJoin, (LineJoin)(LineJoinMiter - 1));

	// Invalid value - positive.
	status = GdipSetCustomLineCapStrokeJoin (cap, (LineJoin)(LineJoinMiterClipped + 1));
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	assertEqualInt (strokeJoin, (LineJoin)(LineJoinMiterClipped + 1));

	// Negative tests.
	status = GdipSetCustomLineCapStrokeJoin (NULL, LineJoinMiterClipped);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_getCustomLineCapStrokeJoin ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	LineJoin strokeJoin;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Negative tests.
	status = GdipGetCustomLineCapStrokeJoin (NULL, &strokeJoin);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCustomLineCapStrokeJoin (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_setCustomLineCapBaseCap ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	LineCap baseCap;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// LineCapFlat.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapFlat);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	assertEqualInt (baseCap, LineCapFlat);

	// LineCapSquare.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapSquare);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	assertEqualInt (baseCap, LineCapSquare);

	// LineCapRound.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapRound);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	assertEqualInt (baseCap, LineCapRound);

	// LineCapTriangle.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapTriangle);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	assertEqualInt (baseCap, LineCapTriangle);

	// Same value.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapTriangle);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	assertEqualInt (baseCap, LineCapTriangle);

	// Negative tests.
	status = GdipSetCustomLineCapBaseCap (NULL, LineCapFlat);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, (LineCap)(LineCapTriangle + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapNoAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapSquareAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapRoundAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapDiamondAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapArrowAnchor);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapCustom);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapAnchorMask);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetCustomLineCapBaseCap (cap, (LineCap)(LineCapCustom + 1));
	assertEqualInt (status, InvalidParameter);

	// Make sure nothing changed.
	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	assertEqualInt (baseCap, LineCapTriangle);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_getCustomLineCapBaseCap ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	LineCap baseCap;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Negative tests.
	status = GdipGetCustomLineCapBaseCap (NULL, &baseCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCustomLineCapBaseCap (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_setCustomLineCapBaseInset ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	REAL baseInset;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Positive value.
	status = GdipSetCustomLineCapBaseInset (cap, 2);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	assertEqualFloat (baseInset, 2);

	// Same value.
	status = GdipSetCustomLineCapBaseInset (cap, 2);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	assertEqualFloat (baseInset, 2);

	// Zero value.
	status = GdipSetCustomLineCapBaseInset (cap, 0);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	assertEqualFloat (baseInset, 0);

	// Negative value.
	status = GdipSetCustomLineCapBaseInset (cap, -1);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	assertEqualFloat (baseInset, -1);

	// Negative tests.
	status = GdipSetCustomLineCapBaseInset (NULL, 1);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_getCustomLineCapBaseInset ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	REAL baseInset;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Negative tests.
	status = GdipGetCustomLineCapBaseInset (NULL, &baseInset);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCustomLineCapBaseInset (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_setCustomLineCapWidthScale ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	REAL widthScale;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Positive value.
	status = GdipSetCustomLineCapWidthScale (cap, 2);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	assertEqualFloat (widthScale, 2);

	// Same value.
	status = GdipSetCustomLineCapWidthScale (cap, 2);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	assertEqualFloat (widthScale, 2);

	// Zero value.
	status = GdipSetCustomLineCapWidthScale (cap, 0);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	assertEqualFloat (widthScale, 0);

	// Negative value.
	status = GdipSetCustomLineCapWidthScale (cap, -1);
	assertEqualInt (status, Ok);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	assertEqualFloat (widthScale, -1);

	// Negative tests.
	status = GdipSetCustomLineCapWidthScale (NULL, 1);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

static void test_getCustomLineCapWidthScale ()
{
	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	REAL widthScale;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	// Negative tests.
	status = GdipGetCustomLineCapWidthScale (NULL, &widthScale);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCustomLineCapWidthScale (cap, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);
}

int
main(int argc, char**argv)
{
	STARTUP;

	test_createCustomLineCap ();
	test_deleteCustomLineCap ();
	test_cloneCustomLineCap ();
	test_getCustomLineCapType ();
	test_setCustomLineCapStrokeCaps ();
	test_getCustomLineCapStrokeCaps ();
	test_setCustomLineCapStrokeJoin ();
	test_getCustomLineCapStrokeJoin ();
	test_setCustomLineCapBaseCap ();
	test_getCustomLineCapBaseCap ();
	test_setCustomLineCapBaseInset ();
	test_getCustomLineCapBaseInset ();
	test_setCustomLineCapWidthScale ();
	test_getCustomLineCapWidthScale ();

	SHUTDOWN;
	return 0;
}
