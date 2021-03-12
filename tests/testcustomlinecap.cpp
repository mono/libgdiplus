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
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedBaseCap, baseCap);

	status = GdipGetCustomLineCapBaseInset (cap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (expectedBaseInset, baseInset);

	status = GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapFlat, startCap);
	ASSERT_EQ (LineCapFlat, endCap);

	status = GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineJoinMiter, strokeJoin);

	status = GdipGetCustomLineCapType (cap, &type);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (CustomLineCapTypeDefault, type);

	status = GdipGetCustomLineCapWidthScale (cap, &widthScale);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, widthScale);
}

TEST(CustomLineCap, GdipCreateCustomLineCap)
{
	STARTUP

	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);

	status = GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);
	ASSERT_EQ (Ok, status);
	verifyCustomLineCap (cap, LineCapFlat, 10);
	GdipDeleteCustomLineCap (cap);

	status = GdipCreateCustomLineCap (NULL, strokePath, LineCapCustom, 0, &cap);
	ASSERT_EQ (Ok, status);
	verifyCustomLineCap (cap, LineCapFlat, 0);
	GdipDeleteCustomLineCap (cap);

	status = GdipCreateCustomLineCap (fillPath, NULL, (LineCap)(LineCapFlat - 1), -1, &cap);
	ASSERT_EQ (Ok, status);
	verifyCustomLineCap (cap, LineCapFlat, -1);
	GdipDeleteCustomLineCap (cap);

	status = GdipCreateCustomLineCap (fillPath, strokePath, (LineCap)(LineCapCustom + 1), 100, &cap);
	ASSERT_EQ (Ok, status);
	verifyCustomLineCap (cap, LineCapFlat, 100);
	GdipDeleteCustomLineCap (cap);

	// Negative tests.
	cap = (GpCustomLineCap *) 0xCC;
	status = GdipCreateCustomLineCap (NULL, NULL, LineCapRoundAnchor, 10, &cap);
	ASSERT_EQ (InvalidParameter, status);
	assert (cap == (GpCustomLineCap *) 0xCC);

	status = GdipCreateCustomLineCap (fillPath, fillPath, LineCapRoundAnchor, 10, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);

	SHUTDOWN
}

TEST(CustomLineCap, GdipDeleteCustomLineCap)
{
	STARTUP

	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	status = GdipDeleteCustomLineCap (cap);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDeleteCustomLineCap (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);

	SHUTDOWN
}

TEST(CustomLineCap, GdipCloneCustomLineCap)
{
	STARTUP

	GpStatus status;
	GpCustomLineCap *cap;
	GpPath *fillPath;
	GpPath *strokePath;
	GpCustomLineCap *clonedCap;

	GdipCreatePath (FillModeAlternate, &fillPath);
	GdipCreatePath (FillModeAlternate, &strokePath);
	GdipCreateCustomLineCap (fillPath, strokePath, LineCapDiamondAnchor, 10, &cap);

	status = GdipCloneCustomLineCap (cap, &clonedCap);
	ASSERT_EQ (Ok, status);
	verifyCustomLineCap (clonedCap, LineCapFlat, 10);
	GdipDeleteCustomLineCap (clonedCap);

	// Negative tests.
	status = GdipCloneCustomLineCap (NULL, &clonedCap);
	ASSERT_EQ (InvalidParameter, status);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCloneCustomLineCap (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipGetCustomLineCapType)
{
	STARTUP

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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCustomLineCapType (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipSetCustomLineCapStrokeCaps)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	ASSERT_EQ (LineCapFlat, startCap);
	ASSERT_EQ (LineCapSquare, endCap);

	// LineCapSquare/LineCapFlat.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapSquare, LineCapFlat);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	ASSERT_EQ (LineCapSquare, startCap);
	ASSERT_EQ (LineCapFlat, endCap);

	// LineCapRound/LineCapTriangle.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapRound, LineCapTriangle);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	ASSERT_EQ (LineCapRound, startCap);
	ASSERT_EQ (LineCapTriangle, endCap);

	// LineCapTriangle/LineCapRound.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapTriangle, LineCapRound);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	ASSERT_EQ (LineCapTriangle, startCap);
	ASSERT_EQ (LineCapRound, endCap);

	// Same value.
	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapTriangle, LineCapRound);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	ASSERT_EQ (LineCapTriangle, startCap);
	ASSERT_EQ (LineCapRound, endCap);

	// Negative tests.
	status = GdipSetCustomLineCapStrokeCaps (NULL, LineCapArrowAnchor, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, (LineCap)(LineCapFlat - 1), LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, (LineCap)(LineCapTriangle + 1), LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapNoAnchor, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapSquareAnchor, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapRoundAnchor, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapDiamondAnchor, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapArrowAnchor, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapCustom, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapAnchorMask, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, (LineCap)(LineCapCustom + 1), LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, (LineCap)(LineCapFlat - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, (LineCap)(LineCapTriangle + 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapNoAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapSquareAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapRoundAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapDiamondAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapArrowAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapCustom);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, LineCapAnchorMask);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapStrokeCaps (cap, LineCapFlat, (LineCap)(LineCapCustom + 1));
	ASSERT_EQ (InvalidParameter, status);

	// Make sure nothing changed.
	GdipGetCustomLineCapStrokeCaps (cap, &startCap, &endCap);
	ASSERT_EQ (LineCapTriangle, startCap);
	ASSERT_EQ (LineCapRound, endCap);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipGetCustomLineCapStrokeCaps)
{
	STARTUP

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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCustomLineCapStrokeCaps (cap, NULL, &endCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCustomLineCapStrokeCaps (cap, &startCap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipSetCustomLineCapStrokeJoin)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ (LineJoinMiter, strokeJoin);

	// LineJoinMiter.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinBevel);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ (LineJoinBevel, strokeJoin);

	// LineJoinRound.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinRound);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ (LineJoinRound, strokeJoin);

	// LineJoinMiterClipped.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinMiterClipped);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ (LineJoinMiterClipped, strokeJoin);

	// Same value.
	status = GdipSetCustomLineCapStrokeJoin (cap, LineJoinMiterClipped);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ (LineJoinMiterClipped, strokeJoin);

	// Invalid value - negative.
	status = GdipSetCustomLineCapStrokeJoin (cap, (LineJoin)(LineJoinMiter - 1));
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ ((LineJoin)(LineJoinMiter - 1), strokeJoin);

	// Invalid value - positive.
	status = GdipSetCustomLineCapStrokeJoin (cap, (LineJoin)(LineJoinMiterClipped + 1));
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapStrokeJoin (cap, &strokeJoin);
	ASSERT_EQ ((LineJoin)(LineJoinMiterClipped + 1), strokeJoin);

	// Negative tests.
	status = GdipSetCustomLineCapStrokeJoin (NULL, LineJoinMiterClipped);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipGetCustomLineCapStrokeJoin)
{
	STARTUP

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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCustomLineCapStrokeJoin (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipSetCustomLineCapBaseCap)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	ASSERT_EQ (LineCapFlat, baseCap);

	// LineCapSquare.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapSquare);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	ASSERT_EQ (LineCapSquare, baseCap);

	// LineCapRound.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapRound);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	ASSERT_EQ (LineCapRound, baseCap);

	// LineCapTriangle.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapTriangle);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Same value.
	status = GdipSetCustomLineCapBaseCap (cap, LineCapTriangle);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	ASSERT_EQ (LineCapTriangle, baseCap);

	// Negative tests.
	status = GdipSetCustomLineCapBaseCap (NULL, LineCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, (LineCap)(LineCapTriangle + 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapNoAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapSquareAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapRoundAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapDiamondAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapArrowAnchor);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapCustom);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, LineCapAnchorMask);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetCustomLineCapBaseCap (cap, (LineCap)(LineCapCustom + 1));
	ASSERT_EQ (InvalidParameter, status);

	// Make sure nothing changed.
	GdipGetCustomLineCapBaseCap (cap, &baseCap);
	ASSERT_EQ (LineCapTriangle, baseCap);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipGetCustomLineCapBaseCap)
{
	STARTUP

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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCustomLineCapBaseCap (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipSetCustomLineCapBaseInset)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	ASSERT_FLOAT_EQ_NAN (2, baseInset);

	// Same value.
	status = GdipSetCustomLineCapBaseInset (cap, 2);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	ASSERT_FLOAT_EQ_NAN (2, baseInset);

	// Zero value.
	status = GdipSetCustomLineCapBaseInset (cap, 0);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	ASSERT_FLOAT_EQ_NAN (0, baseInset);

	// Negative value.
	status = GdipSetCustomLineCapBaseInset (cap, -1);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapBaseInset (cap, &baseInset);
	ASSERT_FLOAT_EQ_NAN (-1, baseInset);

	// Negative tests.
	status = GdipSetCustomLineCapBaseInset (NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipGetCustomLineCapBaseInset)
{
	STARTUP

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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCustomLineCapBaseInset (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipSetCustomLineCapWidthScale)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	ASSERT_FLOAT_EQ_NAN (2, widthScale);

	// Same value.
	status = GdipSetCustomLineCapWidthScale (cap, 2);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	ASSERT_FLOAT_EQ_NAN (2, widthScale);

	// Zero value.
	status = GdipSetCustomLineCapWidthScale (cap, 0);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	ASSERT_FLOAT_EQ_NAN (0, widthScale);

	// Negative value.
	status = GdipSetCustomLineCapWidthScale (cap, -1);
	ASSERT_EQ (Ok, status);

	GdipGetCustomLineCapWidthScale (cap, &widthScale);
	ASSERT_FLOAT_EQ_NAN (-1, widthScale);

	// Negative tests.
	status = GdipSetCustomLineCapWidthScale (NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}

TEST(CustomLineCap, GdipGetCustomLineCapWidthScale)
{
	STARTUP

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
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCustomLineCapWidthScale (cap, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePath (fillPath);
	GdipDeletePath (strokePath);
	GdipDeleteCustomLineCap (cap);

	SHUTDOWN
}
