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

static GpImage * getImage ()
{
	GpStatus status;
	GpBitmap *image;
	status = GdipCreateBitmapFromScan0 (100, 68, 0, PixelFormat32bppRGB, NULL, &image);
	assert (status == Ok);

	return (GpImage *) image;
}

static void verifyPen (GpPen *pen, REAL expectedWidth, Unit expectedUnit, PenType expectedType)
{
	assert (pen && "Expected the pen to be initalized.");

	GpStatus status;
	REAL width;
	GpUnit unit;
	PenType type;
	LineJoin lineJoin;
	PenAlignment mode;
	GpBrush *brush;
	BrushType brushType;
	LineCap startCap;
	LineCap endCap;
	DashCap dashCap;
	DashStyle dashStyle;
	GpCustomLineCap *customStartCap;
	GpCustomLineCap *customEndCap;
	REAL miterLimit;
	GpMatrix *transform;
	INT dashCount;
	REAL dashOffset;
	INT compoundCount;

	GdipCreateMatrix (&transform);

	status = GdipGetPenWidth (pen, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (expectedWidth, width);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedUnit, unit);

	status = GdipGetPenFillType (pen, &type);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedType, type);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineJoinMiter, lineJoin);

	status = GdipGetPenMode (pen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PenAlignmentCenter, mode);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);

	status = GdipGetBrushType (brush, &brushType);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((BrushType)expectedType, brushType);

	status = GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapFlat, startCap);

	status = GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapFlat, endCap);

	status = GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashCapFlat, dashCap);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleSolid, dashStyle);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	ASSERT_EQ (Ok, status);
	assert (!customStartCap);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	ASSERT_EQ (Ok, status);
	assert (!customEndCap);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, miterLimit);

	status = GdipGetPenTransform (pen, transform);
	ASSERT_EQ (Ok, status);
	verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, dashCount);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, dashOffset);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, compoundCount);

	GdipDeleteMatrix (transform);
	GdipDeleteBrush (brush);
}

static void verifySolidBrushPen (GpPen *pen, ARGB expectedColor, REAL expectedWidth, Unit expectedUnit)
{
	GpStatus status;
	ARGB color;

	verifyPen (pen, expectedWidth, expectedUnit, PenTypeSolidColor);

	status = GdipGetPenColor (pen, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedColor, color);
}

static void verifyNonSolidBrushPen (GpPen *pen, REAL expectedWidth, Unit expectedUnit, PenType expectedType)
{
	GpStatus status;
	ARGB color;

	verifyPen (pen, expectedWidth, expectedUnit, expectedType);

	status = GdipGetPenColor (pen, &color);
	ASSERT_EQ (InvalidParameter, status);
}

TEST(Pen, GdipCreatePen1)
{
	STARTUP

	GpStatus status;
	GpPen *pen;

	status = GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, 10, UnitWorld);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 0, UnitPixel, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, 0, UnitPixel);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, -1, UnitPoint, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, -1, UnitPoint);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 10, UnitInch, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, 10, UnitInch);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 10, UnitDocument, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, 10, UnitDocument);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 10, UnitMillimeter, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, 10, UnitMillimeter);
	GdipDeletePen (pen);

#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreatePen1 (0x00000001, 10, UnitCairoPoint, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, 10, UnitCairoPoint);
	GdipDeletePen (pen);
#endif

	// Negative tests.
	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, (Unit)(UnitWorld - 1), &pen);
	ASSERT_EQ (InvalidParameter, status);
	assert (pen == (GpPen *) 0xCC);

	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, UnitDisplay, &pen);
	ASSERT_EQ (InvalidParameter, status);
	assert (pen == (GpPen *) 0xCC);

#if defined(USE_WINDOWS_GDIPLUS)
	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, (Unit)(UnitMillimeter + 1), &pen);
	ASSERT_EQ (InvalidParameter, status);
	assert (pen == (GpPen *) 0xCC);
#else
	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, (Unit)(UnitCairoPoint + 1), &pen);
	ASSERT_EQ (InvalidParameter, status);
	assert (pen == (GpPen *) 0xCC);
#endif

	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, UnitWorld, NULL);
	ASSERT_EQ (InvalidParameter, status);
	assert (pen == (GpPen *) 0xCC);

	SHUTDOWN
}

static GpPointF threePoints[3] =
{
	{3, 13},
	{1, 2},
	{5, 6}
};

TEST(Pen, GdipCreatePen2)
{
	STARTUP

	GpStatus status;
	GpSolidFill *solidBrush;
	GpHatch *hatchBrush;
	GpImage *image;
	GpTexture *textureBrush;
	GpPathGradient *pathGradientBrush;
	GpRectF rect = {1, 2, 3, 4};
	GpLineGradient *lineGradientBrush;

	GpPen *pen;

	GdipCreateSolidFill (1, &solidBrush);
	GdipCreateHatchBrush (HatchStyle05Percent, 1, 2, &hatchBrush);
	image = getImage ();
	GdipCreateTexture (image, WrapModeTile, &textureBrush);
	GdipCreatePathGradient (threePoints, 3, WrapModeTileFlipX, &pathGradientBrush);
	GdipCreateLineBrushFromRect (&rect, 1, 2, LinearGradientModeBackwardDiagonal, WrapModeTileFlipX, &lineGradientBrush);

	// SolidBrush/UnitWorld.
	status = GdipCreatePen2 (solidBrush, 10, UnitWorld, &pen);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (pen, 1, 10, UnitWorld);
	GdipDeletePen (pen);

	// HatchFill/UnitPixel
	status = GdipCreatePen2 (hatchBrush, 0, UnitPixel, &pen);
	ASSERT_EQ (Ok, status);
	verifyNonSolidBrushPen (pen, 0, UnitPixel, PenTypeHatchFill);
	GdipDeletePen (pen);

	// TextureFill/UnitPoint
	status = GdipCreatePen2 (textureBrush, -1, UnitPoint, &pen);
	ASSERT_EQ (Ok, status);
	verifyNonSolidBrushPen (pen, -1, UnitPoint, PenTypeTextureFill);
	GdipDeletePen (pen);

	// PathGradient/UnitInch.
	status = GdipCreatePen2 (pathGradientBrush, 10, UnitInch, &pen);
	ASSERT_EQ (Ok, status);
	verifyNonSolidBrushPen (pen, 10, UnitInch, PenTypePathGradient);
	GdipDeletePen (pen);

	// LineGradient/UnitDocument.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitDocument, &pen);
	ASSERT_EQ (Ok, status);
	verifyNonSolidBrushPen (pen, 10, UnitDocument, PenTypeLinearGradient);
	GdipDeletePen (pen);

	// LinearGradient/UnitDocument.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitMillimeter, &pen);
	ASSERT_EQ (Ok, status);
	verifyNonSolidBrushPen (pen, 10, UnitMillimeter, PenTypeLinearGradient);
	GdipDeletePen (pen);

#if !defined(USE_WINDOWS_GDIPLUS)
	// LinearGradient/UnitCairoPoint.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitCairoPoint, &pen);
	ASSERT_EQ (Ok, status);
	verifyNonSolidBrushPen (pen, 10, UnitCairoPoint, PenTypeLinearGradient);
	GdipDeletePen (pen);
#endif

	// Negative tests.
	status = GdipCreatePen2 (NULL, 10, UnitPixel, &pen);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreatePen2 (solidBrush, 10, UnitDisplay, &pen);
	ASSERT_EQ (InvalidParameter, status);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreatePen2 (solidBrush, 10, (Unit)(UnitMillimeter + 1), &pen);
	ASSERT_EQ (InvalidParameter, status);
#else
	status = GdipCreatePen2 (solidBrush, 10, (Unit)(UnitCairoPoint + 1), &pen);
	ASSERT_EQ (InvalidParameter, status);
#endif

	status = GdipCreatePen2 (solidBrush, 10, UnitPixel, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteBrush ((GpBrush *) solidBrush);
	GdipDeleteBrush ((GpBrush *) hatchBrush);
	GdipDisposeImage (image);
	GdipDeleteBrush ((GpBrush *) textureBrush);
	GdipDeleteBrush ((GpBrush *) pathGradientBrush);
	GdipDeleteBrush ((GpBrush *) lineGradientBrush);

	SHUTDOWN
}

TEST(Pen, GdipClone)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpPen *clone;

	GdipCreatePen1 (0x00000001, 10, UnitDocument, &pen);

	status = GdipClonePen (pen, &clone);
	ASSERT_EQ (Ok, status);
	verifySolidBrushPen (clone, 1, 10, UnitDocument);

	// Negative tests.
	status = GdipClonePen (NULL, &clone);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipClonePen (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeletePen (clone);

	SHUTDOWN
}

TEST(Pen, getPenWidth)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL width;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenWidth (NULL, &width);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenWidth (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenWidth)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL width;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Positive value.
	status = GdipSetPenWidth (pen, 10);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenWidth (pen, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, width);

	// Same value.
	status = GdipSetPenWidth (pen, 10);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenWidth (pen, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, width);

	// Zero value.
	status = GdipSetPenWidth (pen, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenWidth (pen, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, width);

	// Negative value.
	status = GdipSetPenWidth (pen, -1);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenWidth (pen, &width);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-1, width);

	// Negative tests.
	status = GdipSetPenWidth (NULL, 10);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenUnit)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	Unit unit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenUnit (NULL, &unit);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenUnit (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenUnit)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	Unit unit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// UnitWorld.
	status = GdipSetPenUnit (pen, UnitWorld);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitWorld, unit);

	// Same unit.
	status = GdipSetPenUnit (pen, UnitWorld);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitWorld, unit);

	// UnitPixel.
	status = GdipSetPenUnit (pen, UnitPixel);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitPixel, unit);

	// UnitPoint.
	status = GdipSetPenUnit (pen, UnitPoint);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitPoint, unit);

	// UnitInch.
	status = GdipSetPenUnit (pen, UnitInch);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitInch, unit);

	// UnitDocument.
	status = GdipSetPenUnit (pen, UnitDocument);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitDocument, unit);

	// UnitMillimeter.
	status = GdipSetPenUnit (pen, UnitMillimeter);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitMillimeter, unit);

#if !defined(USE_WINDOWS_GDIPLUS)
	// UnitCairoPoint.
	status = GdipSetPenUnit (pen, UnitCairoPoint);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenUnit (pen, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (UnitCairoPoint, unit);
#endif

	// Negative tests.
	status = GdipSetPenUnit (NULL, UnitWorld);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenUnit (pen, (Unit)(UnitWorld -1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenUnit (pen, UnitDisplay);
	ASSERT_EQ (InvalidParameter, status);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipSetPenUnit (pen, (Unit)(UnitMillimeter + 1));
	ASSERT_EQ (InvalidParameter, status);
#else
	status = GdipSetPenUnit (pen, (Unit)(UnitCairoPoint + 1));
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenLineCap197819)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	LineCap startCap;
	LineCap endCap;
	DashCap dashCap;
	GpPath *path;
	GpCustomLineCap *penCustomLineCap;
	GpCustomLineCap *customStartCap;
	REAL startCapBaseInset;
	GpCustomLineCap *customEndCap;
	REAL endCapBaseInset;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	// Valid values.
	status = GdipSetPenLineCap197819 (pen, LineCapArrowAnchor, LineCapDiamondAnchor, DashCapRound);
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ (LineCapArrowAnchor, startCap);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ (LineCapDiamondAnchor, endCap);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapRound, dashCap);

	// Custom values.
	status = GdipSetPenLineCap197819 (pen, LineCapCustom, LineCapCustom, DashCapTriangle);
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ (LineCapCustom, startCap);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ (LineCapCustom, endCap);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapTriangle, dashCap);

	// Invalid values - negative.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapFlat - 1), (LineCap)(LineCapFlat - 1), (DashCap)(DashCapFlat - 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ ((LineCap)(LineCapFlat - 1), startCap);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ ((LineCap)(LineCapFlat - 1), endCap);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapFlat, dashCap);

	// Invalid values - positive.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapCustom + 1), (LineCap)(LineCapCustom + 1), (DashCap)(DashCapTriangle + 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ ((LineCap)(LineCapCustom + 1), startCap);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ ((LineCap)(LineCapCustom + 1), endCap);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapFlat, dashCap);

	// Invalid values - one.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapCustom + 1), (LineCap)(LineCapCustom + 1), (DashCap)(1));
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ ((LineCap)(LineCapCustom + 1), startCap);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ ((LineCap)(LineCapCustom + 1), endCap);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapFlat, dashCap);

	// Set when there is already a custom cap.
	GdipSetPenCustomStartCap (pen, penCustomLineCap);
	GdipSetPenCustomEndCap (pen, penCustomLineCap);

	status = GdipSetPenLineCap197819 (pen, LineCapNoAnchor, LineCapFlat, DashCapRound);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	ASSERT_EQ (Ok, status);

	status = GdipGetCustomLineCapBaseInset (customStartCap, &startCapBaseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, startCapBaseInset);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	ASSERT_EQ (Ok, status);

	status = GdipGetCustomLineCapBaseInset (customEndCap, &endCapBaseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, startCapBaseInset);

	// Negative tests.
	status = GdipSetPenLineCap197819 (NULL, LineCapArrowAnchor, LineCapDiamondAnchor, DashCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeletePath (path);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customStartCap);
	GdipDeleteCustomLineCap (customEndCap);

	SHUTDOWN
}

TEST(Pen, setPenStartCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	LineCap startCap;
	GpPath *path;
	GpCustomLineCap *penCustomLineCap;
	GpCustomLineCap *customStartCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	// Valid values.
	status = GdipSetPenStartCap (pen, LineCapArrowAnchor);
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ (LineCapArrowAnchor, startCap);

	// Same value.
	status = GdipSetPenStartCap (pen, LineCapArrowAnchor);
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ (LineCapArrowAnchor, startCap);

	// Custom values.
	status = GdipSetPenStartCap (pen, LineCapCustom);
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ (LineCapCustom, startCap);

	// Set when there is already a custom cap.
	GdipSetPenCustomStartCap (pen, penCustomLineCap);
	ASSERT_EQ (Ok, status);

	status = GdipSetPenStartCap (pen, LineCapNoAnchor);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	ASSERT_EQ (Ok, status);
	assert (!customStartCap && "Expected the pen's custom start cap to be cleared.");

	// Invalid values - negative.
	status = GdipSetPenStartCap (pen, (LineCap)(LineCapFlat - 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ ((LineCap)(LineCapFlat - 1), startCap);

	// Invalid values - positive.
	status = GdipSetPenStartCap (pen, (LineCap)(LineCapCustom + 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ ((LineCap)(LineCapCustom + 1), startCap);

	// Negative tests.
	status = GdipSetPenStartCap (NULL, LineCapArrowAnchor);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeletePath (path);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customStartCap);

	SHUTDOWN
}

TEST(Pen, setPenEndCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	LineCap endCap;
	GpPath *path;
	GpCustomLineCap *penCustomLineCap;
	GpCustomLineCap *customEndCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	// Valid values.
	status = GdipSetPenEndCap (pen, LineCapArrowAnchor);
	ASSERT_EQ (Ok, status);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ (LineCapArrowAnchor, endCap);

	// Same value.
	status = GdipSetPenEndCap (pen, LineCapArrowAnchor);
	ASSERT_EQ (Ok, status);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ (LineCapArrowAnchor, endCap);

	// Custom values.
	status = GdipSetPenEndCap (pen, LineCapCustom);
	ASSERT_EQ (Ok, status);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ (LineCapCustom, endCap);

	// Set when there is already a custom cap.
	GdipSetPenCustomEndCap (pen, penCustomLineCap);

	status = GdipSetPenEndCap (pen, LineCapNoAnchor);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	ASSERT_EQ (Ok, status);
	assert (!customEndCap && "Expected the pen's custom end cap to be cleared.");

	// Invalid values - negative.
	status = GdipSetPenEndCap (pen, (LineCap)(LineCapFlat - 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ ((LineCap)(LineCapFlat - 1), endCap);

	// Invalid values - positive.
	status = GdipSetPenEndCap (pen, (LineCap)(LineCapCustom + 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ ((LineCap)(LineCapCustom + 1), endCap);

	// Negative tests.
	status = GdipSetPenEndCap (NULL, LineCapArrowAnchor);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeletePath (path);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customEndCap);

	SHUTDOWN
}

TEST(Pen, setPenDashCap197819)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	DashCap dashCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Round.
	status = GdipSetPenDashCap197819 (pen, DashCapRound);
	ASSERT_EQ (Ok, status);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapRound, dashCap);

	// Same value.
	status = GdipSetPenDashCap197819 (pen, DashCapRound);
	ASSERT_EQ (Ok, status);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapRound, dashCap);

	// Triangle.
	status = GdipSetPenDashCap197819 (pen, DashCapTriangle);
	ASSERT_EQ (Ok, status);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapTriangle, dashCap);

	// Flat.
	status = GdipSetPenDashCap197819 (pen, DashCapFlat);
	ASSERT_EQ (Ok, status);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapFlat, dashCap);

	// Invalid values - negative.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(DashCapFlat - 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapFlat, dashCap);

	// Invalid values - positive.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(DashCapTriangle + 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapFlat, dashCap);

	// Invalid values - one.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(1));
	ASSERT_EQ (Ok, status);

	GdipGetPenDashCap197819 (pen, &dashCap);
	ASSERT_EQ (DashCapFlat, dashCap);

	// Negative tests.
	status = GdipSetPenDashCap197819 (NULL, DashCapFlat);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenStartCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	LineCap startCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenStartCap (NULL, &startCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenStartCap (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenEndCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	LineCap endCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenEndCap (NULL, &endCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenEndCap (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenDashCap197819)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	DashCap dashCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashCap197819 (NULL, &dashCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashCap197819 (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenLineJoin)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	LineJoin lineJoin;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// LineJoinBevel.
	status = GdipSetPenLineJoin (pen, LineJoinBevel);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineJoinBevel, lineJoin);

	// Same value.
	status = GdipSetPenLineJoin (pen, LineJoinBevel);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineJoinBevel, lineJoin);

	// Invalid value - negative.
	status = GdipSetPenLineJoin (pen, (LineJoin)(LineJoinMiter - 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((LineJoin)(LineJoinMiter - 1), lineJoin);

	// Invalid value - positive.
	status = GdipSetPenLineJoin (pen, (LineJoin)(LineJoinMiterClipped + 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((LineJoin)(LineJoinMiterClipped + 1), lineJoin);

	// Negative tests.
	status = GdipSetPenLineJoin (NULL, LineJoinBevel);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenLineJoin)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	LineJoin lineJoin;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenLineJoin (NULL, &lineJoin);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenLineJoin (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenCustomStartCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpPath *path;
	GpCustomLineCap *penCustomLineCap;
	GpCustomLineCap *customStartCap;
	REAL baseInset;
	LineCap startCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	status = GdipSetPenCustomStartCap (pen, penCustomLineCap);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenStartCap (pen, &startCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapCustom, startCap);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	ASSERT_EQ (Ok, status);
	assert (customStartCap && customStartCap != penCustomLineCap);

	status = GdipGetCustomLineCapBaseInset (customStartCap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, baseInset);

	// Negative tests.
	status = GdipSetPenCustomStartCap (NULL, penCustomLineCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCustomStartCap (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeletePath (path);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customStartCap);

	SHUTDOWN
}

TEST(Pen, getPenCustomStartCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpCustomLineCap *customStartCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCustomStartCap (NULL, &customStartCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCustomStartCap (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenCustomEndCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpPath *path;
	GpCustomLineCap *penCustomLineCap;
	GpCustomLineCap *customEndCap;
	REAL baseInset;
	LineCap endCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	status = GdipSetPenCustomEndCap (pen, penCustomLineCap);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenEndCap (pen, &endCap);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (LineCapCustom, endCap);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	ASSERT_EQ (Ok, status);
	assert (customEndCap && customEndCap != penCustomLineCap);

	status = GdipGetCustomLineCapBaseInset (customEndCap, &baseInset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, baseInset);

	// Negative tests.
	status = GdipSetPenCustomEndCap (NULL, penCustomLineCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCustomEndCap (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeletePath (path);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customEndCap);

	SHUTDOWN
}

TEST(Pen, getPenCustomEndCap)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpCustomLineCap *customEndCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCustomEndCap (NULL, &customEndCap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCustomEndCap (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenMiterLimit)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL miterLimit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Positive.
	status = GdipSetPenMiterLimit (pen, 100);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (100, miterLimit);

	// Same value.
	status = GdipSetPenMiterLimit (pen, 100);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (100, miterLimit);

	// Zero.
	status = GdipSetPenMiterLimit (pen, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, miterLimit);

	// Negative.
	status = GdipSetPenMiterLimit (pen, -100);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, miterLimit);

	// Negative tests.
	status = GdipSetPenMiterLimit (NULL, 10);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenMiterLimit)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL miterLimit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenMiterLimit (NULL, &miterLimit);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenMiterLimit (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenTransform)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpMatrix *transform;

		GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix (&transform);

		// Negative tests.
	status = GdipGetPenTransform (NULL, transform);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenTransform (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Pen, setPenTransform)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpMatrix *matrix;
	GpMatrix *transform;

		GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	GdipCreateMatrix (&transform);

	status = GdipSetPenTransform (pen, matrix);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	assert (transform != matrix && "Expected new matrix to be a clone.");
	verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

	// Changing the original transform should not modify the pen's transform.
	GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipSetPenTransform (NULL, transform);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenTransform (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Pen, multiplyPenTransform)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpMatrix *originalTransform;
	GpMatrix *matrix;
	GpMatrix *nonInvertibleMatrix;
	GpMatrix *transform;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
	GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &matrix);
	GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
	GdipCreateMatrix (&transform);

	// MatrixOrderAppend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipMultiplyPenTransform (pen, matrix, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipMultiplyPenTransform (pen, matrix, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

	// Null matrix.
	status = GdipMultiplyPenTransform (pen, NULL, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

	// Invalid MatrixOrder - negative.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipMultiplyPenTransform (pen, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
		ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

	// Invalid MatrixOrder - positive.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipMultiplyPenTransform (pen, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

	// Negative tests.
	status = GdipMultiplyPenTransform (NULL, matrix, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipMultiplyPenTransform (pen, nonInvertibleMatrix, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Pen, translatePenTransform)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpMatrix *originalTransform;
	GpMatrix *transform;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
	GdipCreateMatrix (&transform);

	// MatrixOrderAppend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipTranslatePenTransform (pen, 5, 6, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipTranslatePenTransform (pen, 5, 6, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

	// Negative tests.
	status = GdipTranslatePenTransform (NULL, 1, 1, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTranslatePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipTranslatePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Pen, scalePenTransform)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpMatrix *originalTransform;
	GpMatrix *transform;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
	GdipCreateMatrix (&transform);

	// MatrixOrderAppend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipScalePenTransform (pen, 0.5, 0.75, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipScalePenTransform (pen, 0.5, 0.75, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

	// Negative tests.
	status = GdipScalePenTransform (NULL, 1, 1, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipScalePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipScalePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Pen, rotatePenTransform)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpMatrix *originalTransform;
	GpMatrix *transform;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
	GdipCreateMatrix (&transform);

	// MatrixOrderAppend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipRotatePenTransform (pen, 90, MatrixOrderAppend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipRotatePenTransform (pen, 90, MatrixOrderPrepend);
	ASSERT_EQ (Ok, status);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

	// Negative tests.
	status = GdipRotatePenTransform (NULL, 90, MatrixOrderAppend);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipRotatePenTransform (pen, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipRotatePenTransform (pen, 90, (MatrixOrder)(MatrixOrderAppend + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (transform);

	SHUTDOWN
}

TEST(Pen, setPenMode)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpPen *compoundArrayPen;
	REAL twoCompounds[2] = {1, 1};
	PenAlignment mode;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreatePen1 (0x00000001, 10, UnitWorld, &compoundArrayPen);
	GdipSetPenCompoundArray (compoundArrayPen, twoCompounds, 2);

	// Normal pen - PenAlignmentInset.
	status = GdipSetPenMode (pen, PenAlignmentInset);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (pen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PenAlignmentInset, mode);

	// Normal pen - same mode.
	status = GdipSetPenMode (pen, PenAlignmentInset);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (pen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PenAlignmentInset, mode);

	// Normal pen - PenAlignmentCenter.
	status = GdipSetPenMode (pen, PenAlignmentCenter);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (pen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PenAlignmentCenter, mode);

	// Normal pen - negative.
	status = GdipSetPenMode (pen, (PenAlignment)(PenAlignmentCenter - 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (pen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((PenAlignment)(PenAlignmentCenter - 1), mode);

	// Normal pen - positive.
	status = GdipSetPenMode (pen, (PenAlignment)(PenAlignmentInset + 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (pen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((PenAlignment)(PenAlignmentInset + 1), mode);

	// Compound array pen - PenAlignmentCenter.
	status = GdipSetPenMode (compoundArrayPen, PenAlignmentCenter);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PenAlignmentCenter, mode);

	// Compound array pen - same mode.
	status = GdipSetPenMode (compoundArrayPen, PenAlignmentCenter);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (PenAlignmentCenter, mode);

	// Compound array pen - negative.
	status = GdipSetPenMode (compoundArrayPen, (PenAlignment)(PenAlignmentCenter - 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((PenAlignment)(PenAlignmentCenter - 1), mode);

	// Compound array pen - positive.
	status = GdipSetPenMode (compoundArrayPen, (PenAlignment)(PenAlignmentInset + 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((PenAlignment)(PenAlignmentInset + 1), mode);

	// Negative tests.
	status = GdipSetPenMode (NULL, PenAlignmentCenter);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenMode (compoundArrayPen, PenAlignmentInset);
	ASSERT_EQ (NotImplemented, status);

	GdipDeletePen (pen);
	GdipDeletePen (compoundArrayPen);

	SHUTDOWN
}

TEST(Pen, getPenMode)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	PenAlignment mode;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenMode (NULL, &mode);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenMode (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenColor)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	ARGB color;
	GpBrush *brush;
	GpBrushType brushType;
	GpBrush *hatchBrush;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateHatchBrush (HatchStyle05Percent, 0x00000011, 0x00000022, (GpHatch **) &hatchBrush);

	// Solid fill.
	status = GdipSetPenColor (pen, 0x01020304);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenColor (pen, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x01020304, color);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);

	status = GdipGetBrushType (brush, &brushType);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (BrushTypeSolidColor, brushType);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x01020304, color);

	GdipDeleteBrush (brush);

	// Same color.
	status = GdipSetPenColor (pen, 0x01020304);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenColor (pen, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x01020304, color);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);

	status = GdipGetBrushType (brush, &brushType);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (BrushTypeSolidColor, brushType);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x01020304, color);

	GdipDeleteBrush (brush);

	// Different color.
	status = GdipSetPenColor (pen, 0x12345678);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenColor (pen, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x12345678, color);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);

	status = GdipGetBrushType (brush, &brushType);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (BrushTypeSolidColor, brushType);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x12345678, color);

	GdipDeleteBrush (brush);

	// Custom brush - different color.
	status = GdipSetPenBrushFill (pen, hatchBrush);
	ASSERT_EQ (Ok, status);

	status = GdipSetPenColor (pen, 0x12345678);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenColor (pen, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x12345678, color);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);

	status = GdipGetBrushType (brush, &brushType);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (BrushTypeSolidColor, brushType);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x12345678, color);

	GdipDeleteBrush (brush);

	// Custom brush - same color.
	status = GdipSetPenBrushFill (pen, hatchBrush);
	ASSERT_EQ (Ok, status);

	status = GdipSetPenColor (pen, 0x00000011);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenColor (pen, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x00000011, color);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);

	status = GdipGetBrushType (brush, &brushType);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (BrushTypeSolidColor, brushType);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0x00000011, color);

	GdipDeleteBrush (brush);

	// Negative tests.
	status = GdipSetPenColor (NULL, 0x01020304);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteBrush ((GpBrush *) hatchBrush);

	SHUTDOWN
}

TEST(Pen, getPenColor)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	ARGB color;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenColor (NULL, &color);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenColor (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenFillType)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	PenType type;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenFillType (NULL, &type);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenFillType (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenBrushFill)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpSolidFill *solidBrush;
	GpImage *image;
	GpTexture *textureBrush;
	GpBrush *brush;
	ARGB penColor;
	ARGB brushColor;
	WrapMode brushWrapMode;

	image = getImage ();
	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateSolidFill (100, &solidBrush);
	GdipCreateTexture (image, WrapModeTileFlipX, &textureBrush);

	// SolidFill.
	status = GdipSetPenBrushFill (pen, solidBrush);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);
	assert (brush && brush != solidBrush && "Expected new brush to be a clone.");

	status = GdipGetPenColor (pen, &penColor);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (100, penColor);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &brushColor);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (100, brushColor);
	GdipDeleteBrush (brush);

	// Non SolidFill.
	status = GdipSetPenBrushFill (pen, textureBrush);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenBrushFill (pen, &brush);
	ASSERT_EQ (Ok, status);
	assert (brush && brush != textureBrush && "Expected new brush to be a clone.");

	status = GdipGetTextureWrapMode ((GpTexture *) brush, &brushWrapMode);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (WrapModeTileFlipX, brushWrapMode);

	// Negative tests.
	status = GdipSetPenBrushFill (NULL, solidBrush);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenBrushFill (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeleteBrush ((GpBrush *) solidBrush);
	GdipDisposeImage (image);
	GdipDeleteBrush ((GpBrush *) textureBrush);
	GdipDeleteBrush ((GpBrush *) brush);

	SHUTDOWN
}

TEST(Pen, getPenBrushFill)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpBrush *brush;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenBrushFill (NULL, &brush);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenBrushFill (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenDashStyle)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	DashStyle dashStyle;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashStyle (NULL, &dashStyle);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashStyle (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenDashStyle)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	DashStyle dashStyle;
	REAL dashArray[6];
	INT dashCount;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// DashStyleDashDot.
	status = GdipSetPenDashStyle (pen, DashStyleDashDot);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleDashDot, dashStyle);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (4, dashCount);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (3, dashArray[0]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[1]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[2]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[3]);

	// DashStyleSolid.
	status = GdipSetPenDashStyle (pen, DashStyleSolid);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleSolid, dashStyle);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, dashCount);

	// DashStyleDashDotDot.
	status = GdipSetPenDashStyle (pen, DashStyleDashDotDot);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleDashDotDot, dashStyle);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (6, dashCount);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (3, dashArray[0]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[1]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[2]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[3]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[4]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[5]);

	// DashStyleDot.
	status = GdipSetPenDashStyle (pen, DashStyleDot);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleDot, dashStyle);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, dashCount);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[0]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[1]);

	// DashStyleDash.
	status = GdipSetPenDashStyle (pen, DashStyleDash);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleDash, dashStyle);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, dashCount);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (3, dashArray[0]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[1]);

	// DashStyleCustom.
	status = GdipSetPenDashStyle (pen, DashStyleCustom);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleCustom, dashStyle);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, dashCount);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (3, dashArray[0]);
	ASSERT_FLOAT_EQ_NAN (1, dashArray[1]);

	// Invalid value - negative.
	status = GdipSetPenDashStyle (pen, (DashStyle)(DashStyleSolid - 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleCustom, dashStyle);

	// Invalid value - positive.
	status = GdipSetPenDashStyle (pen, (DashStyle)(DashStyleCustom + 1));
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (DashStyleCustom, dashStyle);

	// Negative tests.
	status = GdipSetPenDashStyle (NULL, DashStyleDash);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenDashOffset)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL dashOffset;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashOffset (NULL, &dashOffset);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashOffset (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenDashOffset)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL dashOffset;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Positive.
	status = GdipSetPenDashOffset (pen, 100);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (100, dashOffset);

	// Same value.
	status = GdipSetPenDashOffset (pen, 100);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (100, dashOffset);

	// Zero.
	status = GdipSetPenDashOffset (pen, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, dashOffset);

	// Negative.
	status = GdipSetPenDashOffset (pen, -100);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (-100, dashOffset);

	// Negative tests.
	status = GdipSetPenDashOffset (NULL, 100);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenDashCount)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	INT dashCount;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashCount (NULL, &dashCount);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashCount (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenDashArray)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL twoDashes[2] = {1, 2};
	REAL allZero[2] = {0, 0};
	REAL negative[2] = {2, -1};
	INT dashCount;
	REAL dashes[3];

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// One dash.
	status = GdipSetPenDashArray (pen, twoDashes, 1);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, dashCount);

	status = GdipGetPenDashArray (pen, dashes, dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, dashes[0]);

	// Two dashes.
	status = GdipSetPenDashArray (pen, twoDashes, 2);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenDashCount (pen, &dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, dashCount);

	status = GdipGetPenDashArray (pen, dashes, dashCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, dashes[0]);
	ASSERT_FLOAT_EQ_NAN (2, dashes[1]);

	// Negative tests.
	status = GdipSetPenDashArray (NULL, twoDashes, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenDashArray (pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenDashArray (pen, allZero, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenDashArray (pen, negative, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenDashArray (pen, dashes, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenDashArray (pen, dashes, -1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, getPenDashArray)
{
	STARTUP

	GpStatus status;
	GpPen *penWithNoDashes;
	GpPen *penWithDashes;
	REAL dashes[3] = {0, -1, -2};
	REAL penDashes[2] = {1, 2};

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &penWithNoDashes);
	GdipCreatePen1 (0x00000001, 10, UnitWorld, &penWithDashes);
	GdipSetPenDashArray (penWithDashes, penDashes, 2);

	// Zero count.
	status = GdipGetPenDashArray (penWithDashes, dashes, 0);
	ASSERT_EQ (Ok, status);

	// Small count.
	status = GdipGetPenDashArray (penWithDashes, dashes, 1);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, dashes[0]);
	ASSERT_FLOAT_EQ_NAN (-1, dashes[1]);
	ASSERT_FLOAT_EQ_NAN (-2, dashes[2]);

	// Negative tests.
	status = GdipGetPenDashArray (NULL, dashes, 2);
	ASSERT_EQ (InvalidParameter, status);

	// Negative tests - no dashes.
	status = GdipGetPenDashArray (penWithNoDashes, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashArray (penWithNoDashes, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashArray (penWithNoDashes, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashArray (penWithNoDashes, dashes, 0);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetPenDashArray (penWithNoDashes, dashes, -1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetPenDashArray (penWithNoDashes, dashes, 1);
	ASSERT_EQ (InvalidParameter, status);

	// Negative tests - has dashes.
	status = GdipGetPenDashArray (NULL, dashes, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenDashArray (penWithDashes, dashes, 3);
	ASSERT_EQ (InvalidParameter, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetPenDashArray (penWithDashes, dashes, -1);
	ASSERT_EQ (OutOfMemory, status);
#endif

	GdipDeletePen (penWithNoDashes);
	GdipDeletePen (penWithDashes);

	SHUTDOWN
}

TEST(Pen, getPenCompoundCount)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	INT compoundCount;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCompoundCount (NULL, &compoundCount);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCompoundCount (pen, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, setPenCompoundArray)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	GpPen *insetPen;
	REAL fourCompounds[4] = {1, 1, 1, 1};
	REAL allZero[2] = {0, 0};
	INT compoundCount;
	REAL compounds[6] = {0, 0, 0, 0};
	REAL ascendingOrderZeroToOne[6] = {0, 0.2f, 0.2f, 0.4f, 0.5f, 1};
	REAL ascendingOrderGreaterThanZeroToLessThanOne[4] = {0.2f, 0.2f, 0.4f, 0.5f};
	REAL firstLessThanZero[2] = {-0.01f, 0};
	REAL firstGreaterThanOne[2] = {1.01f, 0};
	REAL secondLessThanZero[2] = {0, -0.01f};
	REAL secondGreaterThanOne[2] = {0, 1.01f};
	REAL secondLessThanFirst[2] = {0.5f, 0.4f};
	REAL thirdLessThanSecond[4] = {0.5f, 0.6f, 0.5f, 0.7f};

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreatePen1 (0x00000001, 10, UnitWorld, &insetPen);
	GdipSetPenMode (insetPen, PenAlignmentInset);

	// Two compounds - all 1.
	status = GdipSetPenCompoundArray (pen, fourCompounds, 2);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, compoundCount);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount - 1);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, compounds[0]);
	ASSERT_FLOAT_EQ_NAN (0, compounds[1]);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, compounds[0]);
	ASSERT_FLOAT_EQ_NAN (1, compounds[1]);

	// Two compounds - all 0.
	status = GdipSetPenCompoundArray (pen, allZero, 2);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, compoundCount);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, compounds[0]);
	ASSERT_FLOAT_EQ_NAN (0, compounds[1]);

	// Four compounds - all one.
	status = GdipSetPenCompoundArray (pen, fourCompounds, 4);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (4, compoundCount);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (1, compounds[0]);
	ASSERT_FLOAT_EQ_NAN (1, compounds[1]);
	ASSERT_FLOAT_EQ_NAN (1, compounds[2]);
	ASSERT_FLOAT_EQ_NAN (1, compounds[3]);

	// Ascending order from 0 to 1.
	status = GdipSetPenCompoundArray (pen, ascendingOrderZeroToOne, 6);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (6, compoundCount);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, compounds[0]);
	ASSERT_FLOAT_EQ_NAN (0.2, compounds[1]);
	ASSERT_FLOAT_EQ_NAN (0.2, compounds[2]);
	ASSERT_FLOAT_EQ_NAN (0.4, compounds[3]);
	ASSERT_FLOAT_EQ_NAN (0.5, compounds[4]);
	ASSERT_FLOAT_EQ_NAN (1, compounds[5]);

	// Ascending order from greater than 0 to less than 1.
	status = GdipSetPenCompoundArray (pen, ascendingOrderGreaterThanZeroToLessThanOne, 4);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (4, compoundCount);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0.2, compounds[0]);
	ASSERT_FLOAT_EQ_NAN (0.2, compounds[1]);
	ASSERT_FLOAT_EQ_NAN (0.4, compounds[2]);
	ASSERT_FLOAT_EQ_NAN (0.5, compounds[3]);

	// Negative tests.
	status = GdipSetPenCompoundArray (NULL, fourCompounds, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, fourCompounds, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, firstLessThanZero, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, firstGreaterThanOne, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, secondLessThanZero, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, secondGreaterThanOne, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, secondLessThanFirst, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (pen, thirdLessThanSecond, 4);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (insetPen, fourCompounds, 2);
	ASSERT_EQ (NotImplemented, status);

	status = GdipSetPenCompoundArray (insetPen, firstLessThanZero, 2);
	ASSERT_EQ (NotImplemented, status);

	status = GdipSetPenCompoundArray (insetPen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (insetPen, fourCompounds, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPenCompoundArray (insetPen, fourCompounds, 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);
	GdipDeletePen (insetPen);

	SHUTDOWN
}

TEST(Pen, getPenCompoundArray)
{
	STARTUP

	GpStatus status;
	GpPen *pen;
	REAL compounds[3];
	REAL twoCompounds[2] = {1, 1};

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Empty compounds.
	status = GdipGetPenCompoundArray (pen, compounds, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCompoundArray (pen, compounds, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundArray (pen, compounds, -1);
	ASSERT_EQ (Ok, status);

	// Non-empty compounds.
	status = GdipSetPenCompoundArray (pen, twoCompounds, 2);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundArray (pen, compounds, 3);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCompoundArray (pen, compounds, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetPenCompoundArray (pen, compounds, -1);
	ASSERT_EQ (Ok, status);

	GdipDeletePen (pen);

	// Negative tests.
	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	status = GdipGetPenCompoundArray (NULL, compounds, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCompoundArray (NULL, compounds, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCompoundArray (pen, NULL, 2);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCompoundArray (pen, NULL, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPenCompoundArray (pen, compounds, 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePen (pen);

	SHUTDOWN
}

TEST(Pen, deletePen)
{
	STARTUP

	GpStatus status;
	GpPen *pen;

	status = GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	ASSERT_EQ (Ok, status);

	status = GdipDeletePen (pen);
	ASSERT_EQ (Ok, status);
	assert (pen);

	// Negative tests.
	status = GdipDeletePen (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}
