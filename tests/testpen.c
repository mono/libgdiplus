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

static GpImage * getImage ()
{
	GpStatus status;
	GpBitmap *image;
	status = GdipCreateBitmapFromScan0 (100, 68, 0, PixelFormat32bppRGB, NULL, &image);
	assertEqualInt (status, Ok);

	return (GpImage *)image;
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
	assertEqualInt (status, Ok);
	assertEqualFloat (width, expectedWidth);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, expectedUnit);

	status = GdipGetPenFillType (pen, &type);
	assertEqualInt (status, Ok);
	assertEqualInt (type, expectedType);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	assertEqualInt (status, Ok);
	assertEqualInt (lineJoin, LineJoinMiter);

	status = GdipGetPenMode (pen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, PenAlignmentCenter);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);

	status = GdipGetBrushType (brush, &brushType);
	assertEqualInt (status, Ok);
	assertEqualInt (brushType, (BrushType)expectedType);

	status = GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (status, Ok);
	assertEqualInt (startCap, LineCapFlat);

	status = GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (status, Ok);
	assertEqualInt (endCap, LineCapFlat);

	status = GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCap, DashCapFlat);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleSolid);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assertEqualInt (status, Ok);
	assert (!customStartCap);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assertEqualInt (status, Ok);
	assert (!customEndCap);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assertEqualInt (status, Ok);
	assertEqualFloat (miterLimit, 10);

	status = GdipGetPenTransform (pen, transform);
	assertEqualInt (status, Ok);
	verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 0);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashOffset, 0);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assertEqualInt (status, Ok);
	assertEqualInt (compoundCount, 0);

	GdipDeleteMatrix (transform);
	GdipDeleteBrush (brush);
}

static void verifySolidBrushPen (GpPen *pen, ARGB expectedColor, REAL expectedWidth, Unit expectedUnit)
{
	GpStatus status;
	ARGB color;

	verifyPen (pen, expectedWidth, expectedUnit, PenTypeSolidColor);

	status = GdipGetPenColor (pen, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, expectedColor);
}

static void verifyNonSolidBrushPen (GpPen *pen, REAL expectedWidth, Unit expectedUnit, PenType expectedType)
{
	GpStatus status;
	ARGB color;

	verifyPen (pen, expectedWidth, expectedUnit, expectedType);

	status = GdipGetPenColor (pen, &color);
	assertEqualInt (status, InvalidParameter);
}

static void test_createPen1 ()
{
	GpStatus status;
	GpPen *pen;

	status = GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, 10, UnitWorld);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 0, UnitPixel, &pen);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, 0, UnitPixel);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, -1, UnitPoint, &pen);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, -1, UnitPoint);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 10, UnitInch, &pen);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, 10, UnitInch);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 10, UnitDocument, &pen);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, 10, UnitDocument);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (0x00000001, 10, UnitMillimeter, &pen);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, 10, UnitMillimeter);
	GdipDeletePen (pen);

#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreatePen1 (0x00000001, 10, UnitCairoPoint, &pen);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, 10, UnitCairoPoint);
	GdipDeletePen (pen);
#endif

	// Negative tests.
	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, (Unit)(UnitWorld - 1), &pen);
	assertEqualInt (status, InvalidParameter);
	assert (pen == (GpPen *) 0xCC);

	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, UnitDisplay, &pen);
	assertEqualInt (status, InvalidParameter);
	assert (pen == (GpPen *) 0xCC);

#if defined(USE_WINDOWS_GDIPLUS)
	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, (Unit)(UnitMillimeter + 1), &pen);
	assertEqualInt (status, InvalidParameter);
	assert (pen == (GpPen *) 0xCC);
#else
	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, (Unit)(UnitCairoPoint + 1), &pen);
	assertEqualInt (status, InvalidParameter);
	assert (pen == (GpPen *) 0xCC);
#endif

	pen = (GpPen *) 0xCC;
	status = GdipCreatePen1 (0x00000001, 10, UnitWorld, NULL);
	assertEqualInt (status, InvalidParameter);
	assert (pen == (GpPen *) 0xCC);
}

static GpPointF threePoints[3] =
{
	{3, 13},
	{1, 2},
	{5, 6}
};

static void test_createPen2 ()
{
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
	assertEqualInt (status, Ok);
	verifySolidBrushPen (pen, 1, 10, UnitWorld);
	GdipDeletePen (pen);

	// HatchFill/UnitPixel
	status = GdipCreatePen2 (hatchBrush, 0, UnitPixel, &pen);
	assertEqualInt (status, Ok);
	verifyNonSolidBrushPen (pen, 0, UnitPixel, PenTypeHatchFill);
	GdipDeletePen (pen);

	// TextureFill/UnitPoint
	status = GdipCreatePen2 (textureBrush, -1, UnitPoint, &pen);
	assertEqualInt (status, Ok);
	verifyNonSolidBrushPen (pen, -1, UnitPoint, PenTypeTextureFill);
	GdipDeletePen (pen);

	// PathGradient/UnitInch.
	status = GdipCreatePen2 (pathGradientBrush, 10, UnitInch, &pen);
	assertEqualInt (status, Ok);
	verifyNonSolidBrushPen (pen, 10, UnitInch, PenTypePathGradient);
	GdipDeletePen (pen);

	// LineGradient/UnitDocument.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitDocument, &pen);
	assertEqualInt (status, Ok);
	verifyNonSolidBrushPen (pen, 10, UnitDocument, PenTypeLinearGradient);
	GdipDeletePen (pen);

	// LinearGradient/UnitDocument.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitMillimeter, &pen);
	assertEqualInt (status, Ok);
	verifyNonSolidBrushPen (pen, 10, UnitMillimeter, PenTypeLinearGradient);
	GdipDeletePen (pen);

#if !defined(USE_WINDOWS_GDIPLUS)
	// LinearGradient/UnitCairoPoint.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitCairoPoint, &pen);
	assertEqualInt (status, Ok);
	verifyNonSolidBrushPen (pen, 10, UnitCairoPoint, PenTypeLinearGradient);
	GdipDeletePen (pen);
#endif

	// Negative tests.
	status = GdipCreatePen2 (NULL, 10, UnitPixel, &pen);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreatePen2 (solidBrush, 10, UnitDisplay, &pen);
	assertEqualInt (status, InvalidParameter);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreatePen2 (solidBrush, 10, (Unit)(UnitMillimeter + 1), &pen);
	assertEqualInt (status, InvalidParameter);
#else
	status = GdipCreatePen2 (solidBrush, 10, (Unit)(UnitCairoPoint + 1), &pen);
	assertEqualInt (status, InvalidParameter);
#endif

	status = GdipCreatePen2 (solidBrush, 10, UnitPixel, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteBrush ((GpBrush *) solidBrush);
	GdipDeleteBrush ((GpBrush *) hatchBrush);
	GdipDisposeImage (image);
	GdipDeleteBrush ((GpBrush *) textureBrush);
	GdipDeleteBrush ((GpBrush *) pathGradientBrush);
	GdipDeleteBrush ((GpBrush *) lineGradientBrush);
}

static void test_clone ()
{
	GpStatus status;
	GpPen *pen;
	GpPen *clone;

	GdipCreatePen1 (0x00000001, 10, UnitDocument, &pen);

	status = GdipClonePen (pen, &clone);
	assertEqualInt (status, Ok);
	verifySolidBrushPen (clone, 1, 10, UnitDocument);

	// Negative tests.
	status = GdipClonePen (NULL, &clone);
	assertEqualInt (status, InvalidParameter);

	status = GdipClonePen (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeletePen (clone);
}

static void test_getPenWidth ()
{
	GpStatus status;
	GpPen *pen;
	REAL width;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenWidth (NULL, &width);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenWidth (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenWidth ()
{
	GpStatus status;
	GpPen *pen;
	REAL width;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Positive value.
	status = GdipSetPenWidth (pen, 10);
	assertEqualInt (status, Ok);

	status = GdipGetPenWidth (pen, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, 10);

	// Same value.
	status = GdipSetPenWidth (pen, 10);
	assertEqualInt (status, Ok);

	status = GdipGetPenWidth (pen, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, 10);

	// Zero value.
	status = GdipSetPenWidth (pen, 0);
	assertEqualInt (status, Ok);

	status = GdipGetPenWidth (pen, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, 0);

	// Negative value.
	status = GdipSetPenWidth (pen, -1);
	assertEqualInt (status, Ok);

	status = GdipGetPenWidth (pen, &width);
	assertEqualInt (status, Ok);
	assertEqualFloat (width, -1);

	// Negative tests.
	status = GdipSetPenWidth (NULL, 10);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenUnit ()
{
	GpStatus status;
	GpPen *pen;
	Unit unit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenUnit (NULL, &unit);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenUnit (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenUnit ()
{
	GpStatus status;
	GpPen *pen;
	Unit unit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// UnitWorld.
	status = GdipSetPenUnit (pen, UnitWorld);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitWorld);

	// Same unit.
	status = GdipSetPenUnit (pen, UnitWorld);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitWorld);

	// UnitPixel.
	status = GdipSetPenUnit (pen, UnitPixel);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitPixel);

	// UnitPoint.
	status = GdipSetPenUnit (pen, UnitPoint);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitPoint);

	// UnitInch.
	status = GdipSetPenUnit (pen, UnitInch);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitInch);

	// UnitDocument.
	status = GdipSetPenUnit (pen, UnitDocument);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitDocument);

	// UnitMillimeter.
	status = GdipSetPenUnit (pen, UnitMillimeter);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitMillimeter);

#if !defined(USE_WINDOWS_GDIPLUS)
	// UnitCairoPoint.
	status = GdipSetPenUnit (pen, UnitCairoPoint);
	assertEqualInt (status, Ok);

	status = GdipGetPenUnit (pen, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, UnitCairoPoint);
#endif

	// Negative tests.
	status = GdipSetPenUnit (NULL, UnitWorld);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenUnit (pen, (Unit)(UnitWorld -1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenUnit (pen, UnitDisplay);
	assertEqualInt (status, InvalidParameter);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipSetPenUnit (pen, (Unit)(UnitMillimeter + 1));
	assertEqualInt (status, InvalidParameter);
#else
	status = GdipSetPenUnit (pen, (Unit)(UnitCairoPoint + 1));
	assertEqualInt (status, InvalidParameter);
#endif

	GdipDeletePen (pen);
}

static void test_setPenLineCap197819 ()
{
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
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, LineCapArrowAnchor);

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, LineCapDiamondAnchor);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapRound);

	// Custom values.
	status = GdipSetPenLineCap197819 (pen, LineCapCustom, LineCapCustom, DashCapTriangle);
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, LineCapCustom);

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, LineCapCustom);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapTriangle);

	// Invalid values - negative.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapFlat - 1), (LineCap)(LineCapFlat - 1), (DashCap)(DashCapFlat - 1));
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, (LineCap)(LineCapFlat - 1));

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, (LineCap)(LineCapFlat - 1));

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapFlat);

	// Invalid values - positive.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapCustom + 1), (LineCap)(LineCapCustom + 1), (DashCap)(DashCapTriangle + 1));
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, (LineCap)(LineCapCustom + 1));

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, (LineCap)(LineCapCustom + 1));

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapFlat);

	// Invalid values - one.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapCustom + 1), (LineCap)(LineCapCustom + 1), (DashCap)(1));
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, (LineCap)(LineCapCustom + 1));

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, (LineCap)(LineCapCustom + 1));

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapFlat);

	// Set when there is already a custom cap.
	GdipSetPenCustomStartCap (pen, penCustomLineCap);
	GdipSetPenCustomEndCap (pen, penCustomLineCap);

	status = GdipSetPenLineCap197819 (pen, LineCapNoAnchor, LineCapFlat, DashCapRound);
	assertEqualInt (status, Ok);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assertEqualInt (status, Ok);

	status = GdipGetCustomLineCapBaseInset (customStartCap, &startCapBaseInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (startCapBaseInset, 10);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assertEqualInt (status, Ok);

	status = GdipGetCustomLineCapBaseInset (customEndCap, &endCapBaseInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (startCapBaseInset, 10);

	// Negative tests.
	status = GdipSetPenLineCap197819 (NULL, LineCapArrowAnchor, LineCapDiamondAnchor, DashCapFlat);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeletePath (path);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customStartCap);
	GdipDeleteCustomLineCap (customEndCap);
}

static void test_setPenStartCap ()
{
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
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, LineCapArrowAnchor);

	// Same value.
	status = GdipSetPenStartCap (pen, LineCapArrowAnchor);
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, LineCapArrowAnchor);

	// Custom values.
	status = GdipSetPenStartCap (pen, LineCapCustom);
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, LineCapCustom);

	// Set when there is already a custom cap.
	GdipSetPenCustomStartCap (pen, penCustomLineCap);
	assertEqualInt (status, Ok);

	status = GdipSetPenStartCap (pen, LineCapNoAnchor);
	assertEqualInt (status, Ok);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assertEqualInt (status, Ok);
	assert (!customStartCap && "Expected the pen's custom start cap to be cleared.");

	// Invalid values - negative.
	status = GdipSetPenStartCap (pen, (LineCap)(LineCapFlat - 1));
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, (LineCap)(LineCapFlat - 1));

	// Invalid values - positive.
	status = GdipSetPenStartCap (pen, (LineCap)(LineCapCustom + 1));
	assertEqualInt (status, Ok);

	GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (startCap, (LineCap)(LineCapCustom + 1));

	// Negative tests.
	status = GdipSetPenStartCap (NULL, LineCapArrowAnchor);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customStartCap);
}

static void test_setPenEndCap ()
{
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
	assertEqualInt (status, Ok);

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, LineCapArrowAnchor);

	// Same value.
	status = GdipSetPenEndCap (pen, LineCapArrowAnchor);
	assertEqualInt (status, Ok);

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, LineCapArrowAnchor);

	// Custom values.
	status = GdipSetPenEndCap (pen, LineCapCustom);
	assertEqualInt (status, Ok);

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, LineCapCustom);

	// Set when there is already a custom cap.
	GdipSetPenCustomEndCap (pen, penCustomLineCap);

	status = GdipSetPenEndCap (pen, LineCapNoAnchor);
	assertEqualInt (status, Ok);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assertEqualInt (status, Ok);
	assert (!customEndCap && "Expected the pen's custom end cap to be cleared.");

	// Invalid values - negative.
	status = GdipSetPenEndCap (pen, (LineCap)(LineCapFlat - 1));
	assertEqualInt (status, Ok);

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, (LineCap)(LineCapFlat - 1));

	// Invalid values - positive.
	status = GdipSetPenEndCap (pen, (LineCap)(LineCapCustom + 1));
	assertEqualInt (status, Ok);

	GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (endCap, (LineCap)(LineCapCustom + 1));

	// Negative tests.
	status = GdipSetPenEndCap (NULL, LineCapArrowAnchor);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customEndCap);
}

static void test_setPenDashCap197819 ()
{
	GpStatus status;
	GpPen *pen;
	DashCap dashCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Round.
	status = GdipSetPenDashCap197819 (pen, DashCapRound);
	assertEqualInt (status, Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapRound);

	// Same value.
	status = GdipSetPenDashCap197819 (pen, DashCapRound);
	assertEqualInt (status, Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapRound);

	// Triangle.
	status = GdipSetPenDashCap197819 (pen, DashCapTriangle);
	assertEqualInt (status, Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapTriangle);

	// Flat.
	status = GdipSetPenDashCap197819 (pen, DashCapFlat);
	assertEqualInt (status, Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapFlat);

	// Invalid values - negative.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(DashCapFlat - 1));
	assertEqualInt (status, Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapFlat);

	// Invalid values - positive.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(DashCapTriangle + 1));
	assertEqualInt (status, Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapFlat);

	// Invalid values - one.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(1));
	assertEqualInt (status, Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assertEqualInt (dashCap, DashCapFlat);

	// Negative tests.
	status = GdipSetPenDashCap197819 (NULL, DashCapFlat);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenStartCap ()
{
	GpStatus status;
	GpPen *pen;
	LineCap startCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenStartCap (NULL, &startCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenStartCap (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenEndCap ()
{
	GpStatus status;
	GpPen *pen;
	LineCap endCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenEndCap (NULL, &endCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenEndCap (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashCap197819 ()
{
	GpStatus status;
	GpPen *pen;
	DashCap dashCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashCap197819 (NULL, &dashCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashCap197819 (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenLineJoin ()
{
	GpStatus status;
	GpPen *pen;
	LineJoin lineJoin;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// LineJoinBevel.
	status = GdipSetPenLineJoin (pen, LineJoinBevel);
	assertEqualInt (status, Ok);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	assertEqualInt (status, Ok);
	assertEqualInt (lineJoin, LineJoinBevel);

	// Same value.
	status = GdipSetPenLineJoin (pen, LineJoinBevel);
	assertEqualInt (status, Ok);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	assertEqualInt (status, Ok);
	assertEqualInt (lineJoin, LineJoinBevel);

	// Invalid value - negative.
	status = GdipSetPenLineJoin (pen, (LineJoin)(LineJoinMiter - 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	assertEqualInt (status, Ok);
	assertEqualInt (lineJoin, (LineJoin)(LineJoinMiter - 1));

	// Invalid value - positive.
	status = GdipSetPenLineJoin (pen, (LineJoin)(LineJoinMiterClipped + 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	assertEqualInt (status, Ok);
	assertEqualInt (lineJoin, (LineJoin)(LineJoinMiterClipped + 1));

	// Negative tests.
	status = GdipSetPenLineJoin (NULL, LineJoinBevel);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenLineJoin ()
{
	GpStatus status;
	GpPen *pen;
	LineJoin lineJoin;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenLineJoin (NULL, &lineJoin);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenLineJoin (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenCustomStartCap ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetPenStartCap (pen, &startCap);
	assertEqualInt (status, Ok);
	assertEqualInt (startCap, LineCapCustom);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assertEqualInt (status, Ok);
	assert (customStartCap && customStartCap != penCustomLineCap);

	status = GdipGetCustomLineCapBaseInset (customStartCap, &baseInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (baseInset, 10);

	// Negative tests.
	status = GdipSetPenCustomStartCap (NULL, penCustomLineCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCustomStartCap (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customStartCap);
}

static void test_getPenCustomStartCap ()
{
	GpStatus status;
	GpPen *pen;
	GpCustomLineCap *customStartCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCustomStartCap (NULL, &customStartCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCustomStartCap (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenCustomEndCap ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetPenEndCap (pen, &endCap);
	assertEqualInt (status, Ok);
	assertEqualInt (endCap, LineCapCustom);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assertEqualInt (status, Ok);
	assert (customEndCap && customEndCap != penCustomLineCap);

	status = GdipGetCustomLineCapBaseInset (customEndCap, &baseInset);
	assertEqualInt (status, Ok);
	assertEqualFloat (baseInset, 10);

	// Negative tests.
	status = GdipSetPenCustomEndCap (NULL, penCustomLineCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCustomEndCap (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customEndCap);
}

static void test_getPenCustomEndCap ()
{
	GpStatus status;
	GpPen *pen;
	GpCustomLineCap *customEndCap;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCustomEndCap (NULL, &customEndCap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCustomEndCap (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenMiterLimit ()
{
	GpStatus status;
	GpPen *pen;
	REAL miterLimit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Positive.
	status = GdipSetPenMiterLimit (pen, 100);
	assertEqualInt (status, Ok);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assertEqualInt (status, Ok);
	assertEqualFloat (miterLimit, 100);

	// Same value.
	status = GdipSetPenMiterLimit (pen, 100);
	assertEqualInt (status, Ok);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assertEqualInt (status, Ok);
	assertEqualFloat (miterLimit, 100);

	// Zero.
	status = GdipSetPenMiterLimit (pen, 0);
	assertEqualInt (status, Ok);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assertEqualInt (status, Ok);
	assertEqualFloat (miterLimit, 1);

	// Negative.
	status = GdipSetPenMiterLimit (pen, -100);
	assertEqualInt (status, Ok);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assertEqualInt (status, Ok);
	assertEqualFloat (miterLimit, 1);

	// Negative tests.
	status = GdipSetPenMiterLimit (NULL, 10);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenMiterLimit ()
{
	GpStatus status;
	GpPen *pen;
	REAL miterLimit;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenMiterLimit (NULL, &miterLimit);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenMiterLimit (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenTransform ()
{
	GpStatus status;
	GpPen *pen;
	GpMatrix *transform;

		GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix (&transform);

		// Negative tests.
	status = GdipGetPenTransform (NULL, transform);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenTransform (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteMatrix (transform);
}

static void test_setPenTransform ()
{
	GpStatus status;
	GpPen *pen;
	GpMatrix *matrix;
	GpMatrix *transform;

		GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
	GdipCreateMatrix (&transform);

	status = GdipSetPenTransform (pen, matrix);
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	assert (transform != matrix && "Expected new matrix to be a clone.");
	verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

	// Changing the original transform should not modify the pen's transform.
	GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

	// Negative tests.
	status = GdipSetPenTransform (NULL, transform);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenTransform (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (transform);
}

static void test_multiplyPenTransform ()
{
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
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipMultiplyPenTransform (pen, matrix, MatrixOrderPrepend);
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

	// Null matrix.
	status = GdipMultiplyPenTransform (pen, NULL, MatrixOrderAppend);
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

	// Invalid MatrixOrder - negative.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipMultiplyPenTransform (pen, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
		assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

	// Invalid MatrixOrder - positive.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipMultiplyPenTransform (pen, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

	// Negative tests.
	status = GdipMultiplyPenTransform (NULL, matrix, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipMultiplyPenTransform (pen, nonInvertibleMatrix, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (matrix);
	GdipDeleteMatrix (nonInvertibleMatrix);
	GdipDeleteMatrix (transform);
}

static void test_translatePenTransform ()
{
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
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipTranslatePenTransform (pen, 5, 6, MatrixOrderPrepend);
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

	// Negative tests.
	status = GdipTranslatePenTransform (NULL, 1, 1, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipTranslatePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipTranslatePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (transform);
}

static void test_scalePenTransform ()
{
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
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipScalePenTransform (pen, 0.5, 0.75, MatrixOrderPrepend);
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

	// Negative tests.
	status = GdipScalePenTransform (NULL, 1, 1, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipScalePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipScalePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (transform);
}

static void test_rotatePenTransform ()
{
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
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

	// MatrixOrderPrepend.
	GdipSetPenTransform (pen, originalTransform);

	status = GdipRotatePenTransform (pen, 90, MatrixOrderPrepend);
	assertEqualInt (status, Ok);

	GdipGetPenTransform (pen, transform);
	verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

	// Negative tests.
	status = GdipRotatePenTransform (NULL, 90, MatrixOrderAppend);
	assertEqualInt (status, InvalidParameter);

	status = GdipRotatePenTransform (pen, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipRotatePenTransform (pen, 90, (MatrixOrder)(MatrixOrderAppend + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteMatrix (originalTransform);
	GdipDeleteMatrix (transform);
}

static void test_setPenMode ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (pen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, PenAlignmentInset);

	// Normal pen - same mode.
	status = GdipSetPenMode (pen, PenAlignmentInset);
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (pen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, PenAlignmentInset);

	// Normal pen - PenAlignmentCenter.
	status = GdipSetPenMode (pen, PenAlignmentCenter);
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (pen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, PenAlignmentCenter);

	// Normal pen - negative.
	status = GdipSetPenMode (pen, (PenAlignment)(PenAlignmentCenter - 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (pen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, (PenAlignment)(PenAlignmentCenter - 1));

	// Normal pen - positive.
	status = GdipSetPenMode (pen, (PenAlignment)(PenAlignmentInset + 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (pen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, (PenAlignment)(PenAlignmentInset + 1));

	// Compound array pen - PenAlignmentCenter.
	status = GdipSetPenMode (compoundArrayPen, PenAlignmentCenter);
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, PenAlignmentCenter);

	// Compound array pen - same mode.
	status = GdipSetPenMode (compoundArrayPen, PenAlignmentCenter);
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, PenAlignmentCenter);

	// Compound array pen - negative.
	status = GdipSetPenMode (compoundArrayPen, (PenAlignment)(PenAlignmentCenter - 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, (PenAlignment)(PenAlignmentCenter - 1));

	// Compound array pen - positive.
	status = GdipSetPenMode (compoundArrayPen, (PenAlignment)(PenAlignmentInset + 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenMode (compoundArrayPen, &mode);
	assertEqualInt (status, Ok);
	assertEqualInt (mode, (PenAlignment)(PenAlignmentInset + 1));

	// Negative tests.
	status = GdipSetPenMode (NULL, PenAlignmentCenter);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenMode (compoundArrayPen, PenAlignmentInset);
	assertEqualInt (status, NotImplemented);

	GdipDeletePen (pen);
	GdipDeletePen (compoundArrayPen);
}

static void test_getPenMode ()
{
	GpStatus status;
	GpPen *pen;
	PenAlignment mode;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenMode (NULL, &mode);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenMode (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenColor ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetPenColor (pen, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x01020304);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);

	status = GdipGetBrushType (brush, &brushType);
	assertEqualInt (status, Ok);
	assertEqualInt (brushType, BrushTypeSolidColor);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x01020304);

	GdipDeleteBrush (brush);

	// Same color.
	status = GdipSetPenColor (pen, 0x01020304);
	assertEqualInt (status, Ok);

	status = GdipGetPenColor (pen, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x01020304);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);

	status = GdipGetBrushType (brush, &brushType);
	assertEqualInt (status, Ok);
	assertEqualInt (brushType, BrushTypeSolidColor);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x01020304);

	GdipDeleteBrush (brush);

	// Different color.
	status = GdipSetPenColor (pen, 0x12345678);
	assertEqualInt (status, Ok);

	status = GdipGetPenColor (pen, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x12345678);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);

	status = GdipGetBrushType (brush, &brushType);
	assertEqualInt (status, Ok);
	assertEqualInt (brushType, BrushTypeSolidColor);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x12345678);

	GdipDeleteBrush (brush);

	// Custom brush - different color.
	status = GdipSetPenBrushFill (pen, hatchBrush);
	assertEqualInt (status, Ok);

	status = GdipSetPenColor (pen, 0x12345678);
	assertEqualInt (status, Ok);

	status = GdipGetPenColor (pen, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x12345678);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);

	status = GdipGetBrushType (brush, &brushType);
	assertEqualInt (status, Ok);
	assertEqualInt (brushType, BrushTypeSolidColor);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x12345678);

	GdipDeleteBrush (brush);

	// Custom brush - same color.
	status = GdipSetPenBrushFill (pen, hatchBrush);
	assertEqualInt (status, Ok);

	status = GdipSetPenColor (pen, 0x00000011);
	assertEqualInt (status, Ok);

	status = GdipGetPenColor (pen, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x00000011);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);

	status = GdipGetBrushType (brush, &brushType);
	assertEqualInt (status, Ok);
	assertEqualInt (brushType, BrushTypeSolidColor);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &color);
	assertEqualInt (status, Ok);
	assertEqualInt (color, 0x00000011);

	GdipDeleteBrush (brush);

	// Negative tests.
	status = GdipSetPenColor (NULL, 0x01020304);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteBrush ((GpBrush *) hatchBrush);
}

static void test_getPenColor ()
{
	GpStatus status;
	GpPen *pen;
	ARGB color;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenColor (NULL, &color);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenColor (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenFillType ()
{
	GpStatus status;
	GpPen *pen;
	PenType type;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenFillType (NULL, &type);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenFillType (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenBrushFill ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);
	assert (brush && brush != solidBrush && "Expected new brush to be a clone.");

	status = GdipGetPenColor (pen, &penColor);
	assertEqualInt (status, Ok);
	assertEqualInt (penColor, 100);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &brushColor);
	assertEqualInt (status, Ok);
	assertEqualInt (brushColor, 100);
	GdipDeleteBrush (brush);

	// Non SolidFill.
	status = GdipSetPenBrushFill (pen, textureBrush);
	assertEqualInt (status, Ok);

	status = GdipGetPenBrushFill (pen, &brush);
	assertEqualInt (status, Ok);
	assert (brush && brush != textureBrush && "Expected new brush to be a clone.");

	status = GdipGetTextureWrapMode ((GpTexture *) brush, &brushWrapMode);
	assertEqualInt (status, Ok);
	assertEqualInt (brushWrapMode, WrapModeTileFlipX);

	// Negative tests.
	status = GdipSetPenBrushFill (NULL, solidBrush);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenBrushFill (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteBrush ((GpBrush *) solidBrush);
	GdipDisposeImage (image);
	GdipDeleteBrush ((GpBrush *) textureBrush);
}

static void test_getPenBrushFill ()
{
	GpStatus status;
	GpPen *pen;
	GpBrush *brush;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenBrushFill (NULL, &brush);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenBrushFill (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashStyle ()
{
	GpStatus status;
	GpPen *pen;
	DashStyle dashStyle;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashStyle (NULL, &dashStyle);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashStyle (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenDashStyle ()
{
	GpStatus status;
	GpPen *pen;
	DashStyle dashStyle;
	REAL dashArray[6];
	INT dashCount;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// DashStyleDashDot.
	status = GdipSetPenDashStyle (pen, DashStyleDashDot);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleDashDot);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 4);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashArray[0], 3);
	assertEqualFloat (dashArray[1], 1);
	assertEqualFloat (dashArray[2], 1);
	assertEqualFloat (dashArray[3], 1);

	// DashStyleSolid.
	status = GdipSetPenDashStyle (pen, DashStyleSolid);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleSolid);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 0);

	// DashStyleDashDotDot.
	status = GdipSetPenDashStyle (pen, DashStyleDashDotDot);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleDashDotDot);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 6);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashArray[0], 3);
	assertEqualFloat (dashArray[1], 1);
	assertEqualFloat (dashArray[2], 1);
	assertEqualFloat (dashArray[3], 1);
	assertEqualFloat (dashArray[4], 1);
	assertEqualFloat (dashArray[5], 1);

	// DashStyleDot.
	status = GdipSetPenDashStyle (pen, DashStyleDot);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleDot);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 2);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashArray[0], 1);
	assertEqualFloat (dashArray[1], 1);

	// DashStyleDash.
	status = GdipSetPenDashStyle (pen, DashStyleDash);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleDash);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 2);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashArray[0], 3);
	assertEqualFloat (dashArray[1], 1);

	// DashStyleCustom.
	status = GdipSetPenDashStyle (pen, DashStyleCustom);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleCustom);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 2);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashArray[0], 3);
	assertEqualFloat (dashArray[1], 1);

	// Invalid value - negative.
	status = GdipSetPenDashStyle (pen, (DashStyle)(DashStyleSolid - 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleCustom);

	// Invalid value - positive.
	status = GdipSetPenDashStyle (pen, (DashStyle)(DashStyleCustom + 1));
	assertEqualInt (status, Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assertEqualInt (status, Ok);
	assertEqualInt (dashStyle, DashStyleCustom);

	// Negative tests.
	status = GdipSetPenDashStyle (NULL, DashStyleDash);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashOffset ()
{
	GpStatus status;
	GpPen *pen;
	REAL dashOffset;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashOffset (NULL, &dashOffset);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashOffset (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenDashOffset ()
{
	GpStatus status;
	GpPen *pen;
	REAL dashOffset;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Positive.
	status = GdipSetPenDashOffset (pen, 100);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashOffset, 100);

	// Same value.
	status = GdipSetPenDashOffset (pen, 100);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashOffset, 100);

	// Zero.
	status = GdipSetPenDashOffset (pen, 0);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashOffset, 0);

	// Negative.
	status = GdipSetPenDashOffset (pen, -100);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashOffset, -100);

	// Negative tests.
	status = GdipSetPenDashOffset (NULL, 100);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashCount ()
{
	GpStatus status;
	GpPen *pen;
	INT dashCount;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashCount (NULL, &dashCount);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashCount (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenDashArray ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 1);

	status = GdipGetPenDashArray (pen, dashes, dashCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashes[0], 1);

	// Two dashes.
	status = GdipSetPenDashArray (pen, twoDashes, 2);
	assertEqualInt (status, Ok);

	status = GdipGetPenDashCount (pen, &dashCount);
	assertEqualInt (status, Ok);
	assertEqualInt (dashCount, 2);

	status = GdipGetPenDashArray (pen, dashes, dashCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashes[0], 1);
	assertEqualFloat (dashes[1], 2);

	// Negative tests.
	status = GdipSetPenDashArray (NULL, twoDashes, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenDashArray (pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenDashArray (pen, allZero, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenDashArray (pen, negative, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenDashArray (pen, dashes, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenDashArray (pen, dashes, -1);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashArray ()
{
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
	assertEqualInt (status, Ok);

	// Small count.
	status = GdipGetPenDashArray (penWithDashes, dashes, 1);
	assertEqualInt (status, Ok);
	assertEqualFloat (dashes[0], 1);
	assertEqualFloat (dashes[1], -1);
	assertEqualFloat (dashes[2], -2);

	// Negative tests.
	status = GdipGetPenDashArray (NULL, dashes, 2);
	assertEqualInt (status, InvalidParameter);

	// Negative tests - no dashes.
	status = GdipGetPenDashArray (penWithNoDashes, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashArray (penWithNoDashes, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashArray (penWithNoDashes, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashArray (penWithNoDashes, dashes, 0);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetPenDashArray (penWithNoDashes, dashes, -1);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetPenDashArray (penWithNoDashes, dashes, 1);
	assertEqualInt (status, InvalidParameter);

	// Negative tests - has dashes.
	status = GdipGetPenDashArray (NULL, dashes, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenDashArray (penWithDashes, dashes, 3);
	assertEqualInt (status, InvalidParameter);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetPenDashArray (penWithDashes, dashes, -1);
	assertEqualInt (status, OutOfMemory);
#endif

	GdipDeletePen (penWithNoDashes);
	GdipDeletePen (penWithDashes);
}

static void test_getPenCompoundCount ()
{
	GpStatus status;
	GpPen *pen;
	INT compoundCount;

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCompoundCount (NULL, &compoundCount);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCompoundCount (pen, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenCompoundArray ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assertEqualInt (status, Ok);
	assertEqualInt (compoundCount, 2);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount - 1);
	assertEqualInt (status, Ok);
	assertEqualFloat (compounds[0], 1);
	assertEqualFloat (compounds[1], 0);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (compounds[0], 1);
	assertEqualFloat (compounds[1], 1);

	// Two compounds - all 0.
	status = GdipSetPenCompoundArray (pen, allZero, 2);
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assertEqualInt (status, Ok);
	assertEqualInt (compoundCount, 2);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (compounds[0], 0);
	assertEqualFloat (compounds[1], 0);

	// Four compounds - all one.
	status = GdipSetPenCompoundArray (pen, fourCompounds, 4);
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assertEqualInt (status, Ok);
	assertEqualInt (compoundCount, 4);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (compounds[0], 1);
	assertEqualFloat (compounds[1], 1);
	assertEqualFloat (compounds[2], 1);
	assertEqualFloat (compounds[3], 1);

	// Ascending order from 0 to 1.
	status = GdipSetPenCompoundArray (pen, ascendingOrderZeroToOne, 6);
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assertEqualInt (status, Ok);
	assertEqualInt (compoundCount, 6);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (compounds[0], 0);
	assertEqualFloat (compounds[1], 0.2f);
	assertEqualFloat (compounds[2], 0.2f);
	assertEqualFloat (compounds[3], 0.4f);
	assertEqualFloat (compounds[4], 0.5f);
	assertEqualFloat (compounds[5], 1);

	// Ascending order from greater than 0 to less than 1.
	status = GdipSetPenCompoundArray (pen, ascendingOrderGreaterThanZeroToLessThanOne, 4);
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assertEqualInt (status, Ok);
	assertEqualInt (compoundCount, 4);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assertEqualInt (status, Ok);
	assertEqualFloat (compounds[0], 0.2f);
	assertEqualFloat (compounds[1], 0.2f);
	assertEqualFloat (compounds[2], 0.4f);
	assertEqualFloat (compounds[3], 0.5f);

	// Negative tests.
	status = GdipSetPenCompoundArray (NULL, fourCompounds, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, firstLessThanZero, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, firstGreaterThanOne, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, secondLessThanZero, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, secondGreaterThanOne, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, secondLessThanFirst, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (pen, thirdLessThanSecond, 4);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (insetPen, fourCompounds, 2);
	assertEqualInt (status, NotImplemented);

	status = GdipSetPenCompoundArray (insetPen, firstLessThanZero, 2);
	assertEqualInt (status, NotImplemented);

	status = GdipSetPenCompoundArray (insetPen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (insetPen, fourCompounds, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPenCompoundArray (insetPen, fourCompounds, 1);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePen (pen);
	GdipDeletePen (insetPen);
}

static void test_getPenCompoundArray ()
{
	GpStatus status;
	GpPen *pen;
	REAL compounds[3];
	REAL twoCompounds[2] = {1, 1};

	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	// Empty compounds.
	status = GdipGetPenCompoundArray (pen, compounds, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCompoundArray (pen, compounds, 0);
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundArray (pen, compounds, -1);
	assertEqualInt (status, Ok);

	// Non-empty compounds.
	status = GdipSetPenCompoundArray (pen, twoCompounds, 2);
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundArray (pen, compounds, 3);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCompoundArray (pen, compounds, 0);
	assertEqualInt (status, Ok);

	status = GdipGetPenCompoundArray (pen, compounds, -1);
	assertEqualInt (status, Ok);

	GdipDeletePen (pen);

	// Negative tests.
	GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);

	status = GdipGetPenCompoundArray (NULL, compounds, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCompoundArray (NULL, compounds, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCompoundArray (pen, NULL, 2);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCompoundArray (pen, NULL, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPenCompoundArray (pen, compounds, 1);
	assertEqualInt (status, InvalidParameter);
}

static void test_deletePen ()
{
	GpStatus status;
	GpPen *pen;

	status = GdipCreatePen1 (0x00000001, 10, UnitWorld, &pen);
	assertEqualInt (status, Ok);

	status = GdipDeletePen (pen);
	assertEqualInt (status, Ok);
	assert (pen);

	// Negative tests.
	status = GdipDeletePen (NULL);
	assertEqualInt (status, InvalidParameter);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_createPen1 ();
	test_createPen2 ();
	test_clone ();
	test_getPenWidth ();
	test_setPenWidth ();
	test_setPenUnit ();
	test_getPenUnit ();
	test_setPenLineCap197819 ();
	test_setPenStartCap ();
	test_setPenEndCap ();
	test_setPenDashCap197819 ();
	test_getPenStartCap ();
	test_getPenEndCap ();
	test_getPenDashCap197819 ();
	test_setPenLineJoin ();
	test_getPenLineJoin ();
	test_setPenCustomStartCap ();
	test_getPenCustomStartCap ();
	test_setPenCustomEndCap ();
	test_getPenCustomEndCap ();
	test_setPenMiterLimit ();
	test_getPenMiterLimit ();
	test_setPenMode ();
	test_getPenMode ();
	test_getPenTransform ();
	test_setPenTransform ();
	test_multiplyPenTransform ();
	test_translatePenTransform ();
	test_scalePenTransform ();
	test_rotatePenTransform ();
	test_setPenColor ();
	test_getPenColor ();
	test_getPenFillType ();
	test_setPenBrushFill ();
	test_getPenBrushFill ();
	test_getPenDashStyle ();
	test_setPenDashStyle ();
	test_getPenDashOffset ();
	test_setPenDashOffset ();
	test_getPenDashCount ();
	test_setPenDashArray ();
	test_getPenDashArray ();
	test_getPenCompoundCount ();
	test_setPenCompoundArray ();
	test_getPenCompoundArray ();
	test_deletePen ();

	SHUTDOWN;
	return 0;
}
