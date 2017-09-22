#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(_WIN32)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
#else
#include <GdiPlusFlat.h>
#endif

#ifdef WIN32
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
    assert (status == Ok);

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
	assert (status == Ok);
	assert (floatsEqual (width, expectedWidth));

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == expectedUnit);

	status = GdipGetPenFillType (pen, &type);
	assert (status == Ok);
	assert (type == expectedType);

	status = GdipGetPenLineJoin (pen, &lineJoin);
	assert (status == Ok);
	assert (lineJoin == LineJoinMiter);

	status = GdipGetPenMode (pen, &mode);
	assert (status == Ok);
	assert (mode == PenAlignmentCenter);

	status = GdipGetPenBrushFill (pen, &brush);
	assert (status == Ok);

	status = GdipGetBrushType (brush, &brushType);
	assert (status == Ok);
	assert (brushType == (BrushType)expectedType);
	
	status = GdipGetPenStartCap (pen, &startCap);
	assert (status == Ok);
	assert (startCap == LineCapFlat);

	status = GdipGetPenEndCap (pen, &endCap);
	assert (status == Ok);
	assert (endCap == LineCapFlat);

	status = GdipGetPenDashCap197819 (pen, &dashCap);
	assert (status == Ok);
	assert (dashCap == DashCapFlat);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleSolid);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assert (status == Ok);
	assert (!customStartCap);
	
	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assert (status == Ok);
	assert (!customEndCap);

	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assert (status == Ok);
	assert (miterLimit == 10);

	status = GdipGetPenTransform (pen, transform);
	assert (status == Ok);
	verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 0);

	status = GdipGetPenDashOffset (pen, &dashOffset);
	assert (status == Ok);
	assert (dashOffset == 0);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assert (status == Ok);
	assert (compoundCount == 0);

	GdipDeleteMatrix (transform);
	GdipDeleteBrush (brush);
}

static void verifySolidBrushPen (GpPen *pen, ARGB expectedColor, REAL expectedWidth, Unit expectedUnit)
{
	GpStatus status;
	ARGB color;

	verifyPen (pen, expectedWidth, expectedUnit, PenTypeSolidColor);

	status = GdipGetPenColor (pen, &color);
	assert (status == Ok);
	assert (color == expectedColor);
}

static void verifyNonSolidBrushPen (GpPen *pen, REAL expectedWidth, Unit expectedUnit, PenType expectedType)
{
	GpStatus status;
	ARGB color;

	verifyPen (pen, expectedWidth, expectedUnit, expectedType);

	status = GdipGetPenColor (pen, &color);
	assert (status == InvalidParameter);
}

static void test_createPen1 ()
{
	GpStatus status;
	GpPen *pen;

	status = GdipCreatePen1 (1, 10, UnitWorld, &pen);
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, 10, UnitWorld);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (1, 0, UnitPixel, &pen);
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, 0, UnitPixel);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (1, -1, UnitPoint, &pen);
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, -1, UnitPoint);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (1, 10, UnitInch, &pen);
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, 10, UnitInch);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (1, 10, UnitDocument, &pen);
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, 10, UnitDocument);
	GdipDeletePen (pen);

	status = GdipCreatePen1 (1, 10, UnitMillimeter, &pen);
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, 10, UnitMillimeter);
	GdipDeletePen (pen);

#if !defined(_WIN32)
	status = GdipCreatePen1 (1, 10, UnitCairoPoint, &pen);
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, 10, UnitCairoPoint);
	GdipDeletePen (pen);
#endif

	// Negative tests.
	status = GdipCreatePen1 (1, 10, (Unit)(UnitWorld - 1), &pen);
	assert (status == InvalidParameter);

	status = GdipCreatePen1 (1, 10, UnitDisplay, &pen);
	assert (status == InvalidParameter);

	status = GdipCreatePen1 (1, 10, (Unit)(8), &pen);
	assert (status == InvalidParameter);

	status = GdipCreatePen1 (1, 10, UnitWorld, NULL);
	assert (status == InvalidParameter);
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
	assert (status == Ok);
	verifySolidBrushPen (pen, 1, 10, UnitWorld);
	GdipDeletePen (pen);

	// HatchFill/UnitPixel
	status = GdipCreatePen2 (hatchBrush, 0, UnitPixel, &pen);
	assert (status == Ok);
	verifyNonSolidBrushPen (pen, 0, UnitPixel, PenTypeHatchFill);
	GdipDeletePen (pen);

	// TextureFill/UnitPoint
	status = GdipCreatePen2 (textureBrush, -1, UnitPoint, &pen);
	assert (status == Ok);
	verifyNonSolidBrushPen (pen, -1, UnitPoint, PenTypeTextureFill);
	GdipDeletePen (pen);

	// PathGradient/UnitInch.
	status = GdipCreatePen2 (pathGradientBrush, 10, UnitInch, &pen);
	assert (status == Ok);
	verifyNonSolidBrushPen (pen, 10, UnitInch, PenTypePathGradient);
	GdipDeletePen (pen);

	// LineGradient/UnitDocument.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitDocument, &pen);
	assert (status == Ok);
	verifyNonSolidBrushPen (pen, 10, UnitDocument, PenTypeLinearGradient);
	GdipDeletePen (pen);

	// LinearGradient/UnitDocument.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitMillimeter, &pen);
	assert (status == Ok);
	verifyNonSolidBrushPen (pen, 10, UnitMillimeter, PenTypeLinearGradient);
	GdipDeletePen (pen);

#if !defined(_WIN32)
// LinearGradient/UnitCairoPoint.
	status = GdipCreatePen2 (lineGradientBrush, 10, UnitCairoPoint, &pen);
	assert (status == Ok);
	verifyNonSolidBrushPen (pen, 10, UnitCairoPoint, PenTypeLinearGradient);
	GdipDeletePen (pen);
#endif

	// Negative tests.
	status = GdipCreatePen1 (1, 10, (Unit)(UnitWorld - 1), &pen);
	assert (status == InvalidParameter);

	status = GdipCreatePen1 (1, 10, UnitDisplay, &pen);
	assert (status == InvalidParameter);

	status = GdipCreatePen1 (1, 10, (Unit)(8), &pen);
	assert (status == InvalidParameter);

	status = GdipCreatePen1 (1, 10, UnitWorld, NULL);
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitDocument, &pen);

	status = GdipClonePen (pen, &clone);
	assert (status == Ok);
	verifySolidBrushPen (clone, 1, 10, UnitDocument);

	// Negative tests.
	status = GdipClonePen (NULL, &clone);
	assert (status == InvalidParameter);

	status = GdipClonePen (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
	GdipDeletePen (clone);
}

static void test_getPenWidth ()
{
	GpStatus status;
	GpPen *pen;
	REAL width;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	status = GdipGetPenWidth (NULL, &width);
	assert (status == InvalidParameter);

	status = GdipGetPenWidth (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenWidth ()
{
	GpStatus status;
	GpPen *pen;
	REAL width;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	status = GdipSetPenWidth (pen, -1);
	assert (status == Ok);

	status = GdipGetPenWidth (pen, &width);
	assert (status == Ok);
	assert (width == -1);

	status = GdipSetPenWidth (NULL, 10);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenUnit ()
{
	GpStatus status;
	GpPen *pen;
	Unit unit;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	status = GdipGetPenUnit (NULL, &unit);
	assert (status == InvalidParameter);

	status = GdipGetPenUnit (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenUnit ()
{
	GpStatus status;
	GpPen *pen;
	Unit unit;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// UnitWorld.
	status = GdipSetPenUnit (pen, UnitWorld);
	assert (status == Ok);

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == UnitWorld);

	// UnitPixel.
	status = GdipSetPenUnit (pen, UnitPixel);
	assert (status == Ok);

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == UnitPixel);

	// UnitPoint.
	status = GdipSetPenUnit (pen, UnitPoint);
	assert (status == Ok);

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == UnitPoint);

	// UnitInch.
	status = GdipSetPenUnit (pen, UnitInch);
	assert (status == Ok);

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == UnitInch);

	// UnitDocument.
	status = GdipSetPenUnit (pen, UnitDocument);
	assert (status == Ok);

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == UnitDocument);

	// UnitMillimeter.
	status = GdipSetPenUnit (pen, UnitMillimeter);
	assert (status == Ok);

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == UnitMillimeter);

#if !defined(_WIN32)
	// UnitCairoPoint.
	status = GdipSetPenUnit (pen, UnitCairoPoint);
	assert (status == Ok);

	status = GdipGetPenUnit (pen, &unit);
	assert (status == Ok);
	assert (unit == UnitCairoPoint);
#endif

	// Negative tests.
	status = GdipSetPenUnit (NULL, UnitWorld);
	assert (status == InvalidParameter);

	status = GdipSetPenUnit (pen, (Unit)(UnitWorld -1));
	assert (status == InvalidParameter);

	status = GdipSetPenUnit (pen, UnitDisplay);
	assert (status == InvalidParameter);

	status = GdipSetPenUnit (pen, (Unit)(8));
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);
	
	// Valid values.
	status = GdipSetPenLineCap197819 (pen, LineCapArrowAnchor, LineCapDiamondAnchor, DashCapRound);

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == LineCapArrowAnchor);

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == LineCapDiamondAnchor);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapRound);

	// Custom values.
	status = GdipSetPenLineCap197819 (pen, LineCapCustom, LineCapCustom, DashCapTriangle);

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == LineCapCustom);

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == LineCapCustom);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapTriangle);

	// Invalid values - negative.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapFlat - 1), (LineCap)(LineCapFlat - 1), (DashCap)(DashCapFlat - 1));

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == (LineCap)(LineCapFlat - 1));

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == (LineCap)(LineCapFlat - 1));

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapFlat);

	// Invalid values - positive.
	status = GdipSetPenLineCap197819 (pen, (LineCap)(LineCapCustom + 1), (LineCap)(LineCapCustom + 1), (DashCap)(DashCapTriangle + 1));

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == (LineCap)(LineCapCustom + 1));

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == (LineCap)(LineCapCustom + 1));

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapFlat);

	// Set when there is already a custom cap.
	GdipSetPenCustomStartCap (pen, penCustomLineCap);
	GdipSetPenCustomEndCap (pen, penCustomLineCap);

	status = GdipSetPenLineCap197819 (pen, LineCapNoAnchor, LineCapFlat, DashCapRound);
	assert (status == Ok);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assert (status == Ok);

	status = GdipGetCustomLineCapBaseInset (customStartCap, &startCapBaseInset);
	assert (status == Ok);
	assert (startCapBaseInset == 10);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assert (status == Ok);

	status = GdipGetCustomLineCapBaseInset (customEndCap, &endCapBaseInset);
	assert (status == Ok);
	assert (startCapBaseInset == 10);

	// Negative tests.
	status = GdipSetPenLineCap197819 (NULL, LineCapArrowAnchor, LineCapDiamondAnchor, DashCapFlat);
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	// Valid values.
	status = GdipSetPenStartCap (pen, LineCapArrowAnchor);

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == LineCapArrowAnchor);

	// Custom values.
	status = GdipSetPenStartCap (pen, LineCapCustom);

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == LineCapCustom);

	// Set when there is already a custom cap.
	GdipSetPenCustomStartCap (pen, penCustomLineCap);

	status = GdipSetPenStartCap (pen, LineCapNoAnchor);
	assert (status == Ok);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assert (status == Ok);
	assert (!customStartCap && "Expected the pen's custom start cap to be cleared.");

	// Invalid values - negative.
	status = GdipSetPenStartCap (pen, (LineCap)(LineCapFlat - 1));
	assert (status == Ok);

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == (LineCap)(LineCapFlat - 1));

	// Invalid values - positive.
	status = GdipSetPenStartCap (pen, (LineCap)(LineCapCustom + 1));
	assert (status == Ok);

	GdipGetPenStartCap (pen, &startCap);
	assert (startCap == (LineCap)(LineCapCustom + 1));

	// Negative tests.
	status = GdipSetPenStartCap (NULL, LineCapArrowAnchor);
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	// Valid values.
	status = GdipSetPenEndCap (pen, LineCapArrowAnchor);
	assert (status == Ok);

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == LineCapArrowAnchor);

	// Custom values.
	status = GdipSetPenEndCap (pen, LineCapCustom);
	assert (status == Ok);

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == LineCapCustom);

	// Set when there is already a custom cap.
	GdipSetPenCustomEndCap (pen, penCustomLineCap);

	status = GdipSetPenEndCap (pen, LineCapNoAnchor);
	assert (status == Ok);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assert (status == Ok);
	assert (!customEndCap && "Expected the pen's custom end cap to be cleared.");

	// Invalid values - negative.
	status = GdipSetPenEndCap (pen, (LineCap)(LineCapFlat - 1));
	assert (status == Ok);

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == (LineCap)(LineCapFlat - 1));

	// Invalid values - positive.
	status = GdipSetPenEndCap (pen, (LineCap)(LineCapCustom + 1));
	assert (status == Ok);

	GdipGetPenEndCap (pen, &endCap);
	assert (endCap == (LineCap)(LineCapCustom + 1));

	// Negative tests.
	status = GdipSetPenEndCap (NULL, LineCapArrowAnchor);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customEndCap);
}

static void tet_setPenDashCap197819 ()
{
	GpStatus status;
	GpPen *pen;
	DashCap dashCap;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Round.
	status = GdipSetPenDashCap197819 (pen, DashCapRound);
	assert (status == Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapRound);

	// Triangle.
	status = GdipSetPenDashCap197819 (pen, DashCapTriangle);
	assert (status == Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapTriangle);

	// Flat.
	status = GdipSetPenDashCap197819 (pen, DashCapFlat);
	assert (status == Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapFlat);

	// Invalid values - negative.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(DashCapFlat - 1));
	assert (status == Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapFlat);

	// Invalid values - positive.
	status = GdipSetPenDashCap197819 (pen, (DashCap)(DashCapTriangle + 1));
	assert (status == Ok);

	GdipGetPenDashCap197819 (pen, &dashCap);
	assert (dashCap == DashCapFlat);

	// Negative tests.
	status = GdipSetPenDashCap197819 (NULL, DashCapFlat);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenStartCap ()
{
	GpStatus status;
	GpPen *pen;
	LineCap startCap;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenStartCap (NULL, &startCap);
	assert (status == InvalidParameter);

	status = GdipGetPenStartCap (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenEndCap ()
{
	GpStatus status;
	GpPen *pen;
	LineCap endCap;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenEndCap (NULL, &endCap);
	assert (status == InvalidParameter);

	status = GdipGetPenEndCap (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashCap197819 ()
{
	GpStatus status;
	GpPen *pen;
	DashCap dashCap;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashCap197819 (NULL, &dashCap);
	assert (status == InvalidParameter);

	status = GdipGetPenDashCap197819 (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenLineJoin ()
{
	GpStatus status;
	GpPen *pen;
	LineJoin lineJoin;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// LineJoinBevel.
	status = GdipSetPenLineJoin (pen, LineJoinBevel);
	assert (status == Ok);
	
	status = GdipGetPenLineJoin (pen, &lineJoin);
	assert (status == Ok);
	assert (lineJoin == LineJoinBevel);
	
	// Invalid value - negative.
	status = GdipSetPenLineJoin (pen, (LineJoin)(LineJoinMiter - 1));
	assert (status == Ok);
	
	status = GdipGetPenLineJoin (pen, &lineJoin);
	assert (status == Ok);
	assert (lineJoin == (LineJoin)(LineJoinMiter - 1));
	
	// Invalid value - positive.
	status = GdipSetPenLineJoin (pen, (LineJoin)(LineJoinMiterClipped + 1));
	assert (status == Ok);
	
	status = GdipGetPenLineJoin (pen, &lineJoin);
	assert (status == Ok);
	assert (lineJoin == (LineJoin)(LineJoinMiterClipped + 1));

	// Negative tests.
	status = GdipSetPenLineJoin (NULL, LineJoinBevel);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenLineJoin ()
{
	GpStatus status;
	GpPen *pen;
	LineJoin lineJoin;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenLineJoin (NULL, &lineJoin);
	assert (status == InvalidParameter);

	status = GdipGetPenLineJoin (pen, NULL);
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	status = GdipSetPenCustomStartCap (pen, penCustomLineCap);
	assert (status == Ok);

	status = GdipGetPenStartCap (pen, &startCap);
	assert (status == Ok);
	assert (startCap == LineCapCustom);

	status = GdipGetPenCustomStartCap (pen, &customStartCap);
	assert (status == Ok);
	assert (customStartCap && customStartCap != penCustomLineCap);

	status = GdipGetCustomLineCapBaseInset (customStartCap, &baseInset);
	assert (status == Ok);
	assert (baseInset == 10);

	// Negative tests.
	status = GdipSetPenCustomStartCap (NULL, penCustomLineCap);
	assert (status == InvalidParameter);

	status = GdipSetPenCustomStartCap (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customStartCap);
}

static void test_getPenCustomStartCap ()
{
	GpStatus status;
	GpPen *pen;
	GpCustomLineCap *customStartCap;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCustomStartCap (NULL, &customStartCap);
	assert (status == InvalidParameter);

	status = GdipGetPenCustomStartCap (pen, NULL);
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitWorld, &pen);
	GdipCreatePath (FillModeAlternate, &path);
	GdipCreateCustomLineCap (path, NULL, LineCapDiamondAnchor, 10, &penCustomLineCap);

	status = GdipSetPenCustomEndCap (pen, penCustomLineCap);
	assert (status == Ok);

	status = GdipGetPenEndCap (pen, &endCap);
	assert (status == Ok);
	assert (endCap == LineCapCustom);

	status = GdipGetPenCustomEndCap (pen, &customEndCap);
	assert (status == Ok);
	assert (customEndCap && customEndCap != penCustomLineCap);

	status = GdipGetCustomLineCapBaseInset (customEndCap, &baseInset);
	assert (status == Ok);
	assert (baseInset == 10);

	// Negative tests.
	status = GdipSetPenCustomEndCap (NULL, penCustomLineCap);
	assert (status == InvalidParameter);

	status = GdipSetPenCustomEndCap (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
	GdipDeleteCustomLineCap (penCustomLineCap);
	GdipDeleteCustomLineCap (customEndCap);
}

static void test_getPenCustomEndCap ()
{
	GpStatus status;
	GpPen *pen;
	GpCustomLineCap *customEndCap;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCustomEndCap (NULL, &customEndCap);
	assert (status == InvalidParameter);

	status = GdipGetPenCustomEndCap (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenMiterLimit ()
{
	GpStatus status;
	GpPen *pen;
	REAL miterLimit;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Positive.
	status = GdipSetPenMiterLimit (pen, 100);
	assert (status == Ok);
	
	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assert (status == Ok);
	assert (miterLimit == 100);

	// Zero.
	status = GdipSetPenMiterLimit (pen, 0);
	assert (status == Ok);
	
	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assert (status == Ok);
	assert (miterLimit == 1);

	// Negative.
	status = GdipSetPenMiterLimit (pen, -100);
	assert (status == Ok);
	
	status = GdipGetPenMiterLimit (pen, &miterLimit);
	assert (status == Ok);
	assert (miterLimit == 1);

	// Negative tests.
	status = GdipSetPenMiterLimit (NULL, 10);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenMiterLimit ()
{
	GpStatus status;
	GpPen *pen;
	REAL miterLimit;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenMiterLimit (NULL, &miterLimit);
	assert (status == InvalidParameter);

	status = GdipGetPenMiterLimit (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenTransform ()
{
    GpStatus status;
    GpPen *pen;
    GpMatrix *transform;

		GdipCreatePen1 (1, 10, UnitWorld, &pen);
    GdipCreateMatrix (&transform);

		// Negative tests.
    status = GdipGetPenTransform (NULL, transform);
    assert (status == InvalidParameter);

    status = GdipGetPenTransform (pen, NULL);
    assert (status == InvalidParameter);

    GdipDeletePen (pen);
    GdipDeleteMatrix (transform);
}

static void test_setPenTransform ()
{
    GpStatus status;
    GpPen *pen;
    GpMatrix *matrix;
    GpMatrix *transform;

		GdipCreatePen1 (1, 10, UnitWorld, &pen);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    status = GdipSetPenTransform (pen, matrix);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the pen's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Negative tests.
    status = GdipSetPenTransform (NULL, transform);
    assert (status == InvalidParameter);

    status = GdipSetPenTransform (pen, NULL);
    assert (status == InvalidParameter);

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

    GdipCreatePen1 (1, 10, UnitWorld, &pen);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipMultiplyPenTransform (pen, matrix, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipMultiplyPenTransform (pen, matrix, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix.
    status = GdipMultiplyPenTransform (pen, NULL, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - this produces garbage data.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipMultiplyPenTransform (pen, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == Ok);

    status = GdipMultiplyPenTransform (pen, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == Ok);

    // Negative tests.
    status = GdipMultiplyPenTransform (NULL, matrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipMultiplyPenTransform (pen, nonInvertibleMatrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

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

    GdipCreatePen1 (1, 10, UnitWorld, &pen);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipTranslatePenTransform (pen, 5, 6, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipTranslatePenTransform (pen, 5, 6, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslatePenTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipTranslatePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipTranslatePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

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

    GdipCreatePen1 (1, 10, UnitWorld, &pen);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipScalePenTransform (pen, 0.5, 0.75, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipScalePenTransform (pen, 0.5, 0.75, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScalePenTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipScalePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipScalePenTransform (pen, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

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

    GdipCreatePen1 (1, 10, UnitWorld, &pen);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipRotatePenTransform (pen, 90, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetPenTransform (pen, originalTransform);

    status = GdipRotatePenTransform (pen, 90, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetPenTransform (pen, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotatePenTransform (NULL, 90, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipRotatePenTransform (pen, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipRotatePenTransform (pen, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

    GdipDeletePen (pen);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_setPenMode ()
{
	GpStatus status;
	GpPen *pen;
	PenAlignment mode;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// PenAlignmentInset.
	status = GdipSetPenMode (pen, PenAlignmentInset);
	assert (status == Ok);
	
	status = GdipGetPenMode (pen, &mode);
	assert (status == Ok);
	assert (mode == PenAlignmentInset);
	
	// Invalid value - negative.
	status = GdipSetPenMode (pen, (PenAlignment)(PenAlignmentCenter - 1));
	assert (status == Ok);
	
	status = GdipGetPenMode (pen, &mode);
	assert (status == Ok);
	assert (mode == (PenAlignment)(PenAlignmentCenter - 1));
	
	// Invalid value - positive.
	status = GdipSetPenMode (pen, (PenAlignment)(PenAlignmentInset + 1));
	assert (status == Ok);
	
	status = GdipGetPenMode (pen, &mode);
	assert (status == Ok);
	assert (mode == (PenAlignment)(PenAlignmentInset + 1));

	// Negative tests.
	status = GdipSetPenMode (NULL, PenAlignmentCenter);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenMode ()
{
	GpStatus status;
	GpPen *pen;
	PenAlignment mode;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenMode (NULL, &mode);
	assert (status == InvalidParameter);

	status = GdipGetPenMode (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenColor ()
{
	GpStatus status;
	GpPen *pen;
	ARGB color;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	status = GdipSetPenColor (pen, 100);
	assert (status == Ok);
	
	status = GdipGetPenColor (pen, &color);
	assert (status == Ok);
	assert (color == 100);

	// Negative tests.
	status = GdipSetPenColor (NULL, 100);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenColor ()
{
	GpStatus status;
	GpPen *pen;
	ARGB color;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenColor (NULL, &color);
	assert (status == InvalidParameter);

	status = GdipGetPenColor (pen, NULL);
	assert (status == InvalidParameter);

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
	GdipCreatePen1 (1, 10, UnitWorld, &pen);
	GdipCreateSolidFill (100, &solidBrush);
	GdipCreateTexture (image, WrapModeTileFlipX, &textureBrush);

	// SolidFill.
	status = GdipSetPenBrushFill (pen, solidBrush);
	assert (status == Ok);
	
	status = GdipGetPenBrushFill (pen, &brush);
	assert (status == Ok);
	assert (brush && brush != solidBrush && "Expected new brush to be a clone.");
	
	status = GdipGetPenColor (pen, &penColor);
	assert (status == Ok);
	assert (penColor == 100);

	status = GdipGetSolidFillColor ((GpSolidFill *) brush, &brushColor);
	assert (status == Ok);
	assert (brushColor == 100);
	GdipDeleteBrush (brush);

	// Non SolidFill.
	status = GdipSetPenBrushFill (pen, textureBrush);
	assert (status == Ok);
	
	status = GdipGetPenBrushFill (pen, &brush);
	assert (status == Ok);
	assert (brush && brush != textureBrush && "Expected new brush to be a clone.");

	status = GdipGetTextureWrapMode ((GpTexture *) brush, &brushWrapMode);
	assert (status == Ok);
	assert (brushWrapMode == WrapModeTileFlipX);

	// Negative tests.
	status = GdipSetPenBrushFill (NULL, solidBrush);
	assert (status == InvalidParameter);

	status = GdipSetPenBrushFill (pen, NULL);
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenBrushFill (NULL, &brush);
	assert (status == InvalidParameter);

	status = GdipGetPenBrushFill (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashStyle ()
{
	GpStatus status;
	GpPen *pen;
	DashStyle dashStyle;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashStyle (NULL, &dashStyle);
	assert (status == InvalidParameter);

	status = GdipGetPenDashStyle (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenDashStyle ()
{
	GpStatus status;
	GpPen *pen;
	DashStyle dashStyle;
	REAL dashArray[6];
	INT dashCount;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// DashStyleDashDot.
	status = GdipSetPenDashStyle (pen, DashStyleDashDot);
	assert (status == Ok);
	
	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleDashDot);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 4);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assert (status == Ok);
	assert (dashArray[0] == 3);
	assert (dashArray[1] == 1);
	assert (dashArray[2] == 1);
	assert (dashArray[3] == 1);

	// DashStyleSolid.
	status = GdipSetPenDashStyle (pen, DashStyleSolid);
	assert (status == Ok);
	
	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleSolid);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 0);

	// DashStyleDashDotDot.
	status = GdipSetPenDashStyle (pen, DashStyleDashDotDot);
	assert (status == Ok);
	
	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleDashDotDot);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 6);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assert (status == Ok);
	assert (dashArray[0] == 3);
	assert (dashArray[1] == 1);
	assert (dashArray[2] == 1);
	assert (dashArray[3] == 1);
	assert (dashArray[4] == 1);
	assert (dashArray[5] == 1);

	// DashStyleDot.
	status = GdipSetPenDashStyle (pen, DashStyleDot);
	assert (status == Ok);
	
	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleDot);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 2);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assert (status == Ok);
	assert (dashArray[0] == 1);
	assert (dashArray[1] == 1);

	// DashStyleDash.
	status = GdipSetPenDashStyle (pen, DashStyleDash);
	assert (status == Ok);
	
	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleDash);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 2);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assert (status == Ok);
	assert (dashArray[0] == 3);
	assert (dashArray[1] == 1);

	// DashStyleCustom.
	status = GdipSetPenDashStyle (pen, DashStyleCustom);
	assert (status == Ok);
	
	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleCustom);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 2);

	status = GdipGetPenDashArray (pen, dashArray, dashCount);
	assert (status == Ok);
	assert (dashArray[0] == 3);
	assert (dashArray[1] == 1);

	// Invalid value - negative.
	status = GdipSetPenDashStyle (pen, (DashStyle)(DashStyleSolid - 1));
	assert (status == Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleCustom);

	// Invalid value - positive.
	status = GdipSetPenDashStyle (pen, (DashStyle)(DashStyleCustom + 1));
	assert (status == Ok);

	status = GdipGetPenDashStyle (pen, &dashStyle);
	assert (status == Ok);
	assert (dashStyle == DashStyleCustom);

	// Negative tests.
	status = GdipSetPenDashStyle (NULL, DashStyleDash);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashOffset ()
{
	GpStatus status;
	GpPen *pen;
	REAL dashOffset;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashOffset (NULL, &dashOffset);
	assert (status == InvalidParameter);

	status = GdipGetPenDashOffset (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenDashOffset ()
{
	GpStatus status;
	GpPen *pen;
	REAL dashOffset;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Positive.
	status = GdipSetPenDashOffset (pen, 100);
	assert (status == Ok);
	
	status = GdipGetPenDashOffset (pen, &dashOffset);
	assert (status == Ok);
	assert (dashOffset == 100);

	// Zero.
	status = GdipSetPenDashOffset (pen, 0);
	assert (status == Ok);
	
	status = GdipGetPenDashOffset (pen, &dashOffset);
	assert (status == Ok);
	assert (dashOffset == 0);

	// Negative.
	status = GdipSetPenDashOffset (pen, -100);
	assert (status == Ok);
	
	status = GdipGetPenDashOffset (pen, &dashOffset);
	assert (status == Ok);
	assert (dashOffset == -100);

	// Negative tests.
	status = GdipSetPenDashOffset (NULL, 100);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashCount ()
{
	GpStatus status;
	GpPen *pen;
	INT dashCount;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashCount (NULL, &dashCount);
	assert (status == InvalidParameter);

	status = GdipGetPenDashCount (pen, NULL);
	assert (status == InvalidParameter);

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

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// One dash.
	status = GdipSetPenDashArray (pen, twoDashes, 1);
	assert (status == Ok);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 1);

	status = GdipGetPenDashArray (pen, dashes, dashCount);
	assert (status == Ok);
	assert (dashes[0] == 1);

	// Two dashes.
	status = GdipSetPenDashArray (pen, twoDashes, 2);
	assert (status == Ok);

	status = GdipGetPenDashCount (pen, &dashCount);
	assert (status == Ok);
	assert (dashCount == 2);

	status = GdipGetPenDashArray (pen, dashes, dashCount);
	assert (status == Ok);
	assert (dashes[0] == 1);
	assert (dashes[1] == 2);

	// Negative tests.
	status = GdipSetPenDashArray (NULL, twoDashes, 2);
	assert (status == InvalidParameter);

	status = GdipSetPenDashArray (pen, NULL, 2);
	assert (status == InvalidParameter);

	status = GdipSetPenDashArray (pen, allZero, 2);
	assert (status == InvalidParameter);

	status = GdipSetPenDashArray (pen, negative, 2);
	assert (status == InvalidParameter);

	status = GdipSetPenDashArray (pen, dashes, 0);
	assert (status == InvalidParameter);

	status = GdipSetPenDashArray (pen, dashes, -1);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenDashArray ()
{
	GpStatus status;
	GpPen *pen;
	REAL dashes[3];

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenDashArray (NULL, dashes, 2);
	assert (status == InvalidParameter);

	status = GdipGetPenDashArray (pen, NULL, 2);
	assert (status == InvalidParameter);

	status = GdipGetPenDashArray (pen, dashes, 0);
	assert (status == OutOfMemory);

	status = GdipGetPenDashArray (pen, dashes, -1);
	assert (status == OutOfMemory);

	status = GdipGetPenDashArray (pen, dashes, 1);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenCompoundCount ()
{
	GpStatus status;
	GpPen *pen;
	INT compoundCount;

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCompoundCount (NULL, &compoundCount);
	assert (status == InvalidParameter);

	status = GdipGetPenCompoundCount (pen, NULL);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_setPenCompoundArray ()
{
	GpStatus status;
	GpPen *pen;
	REAL fourCompounds[4] = {1, 1, 1, 1};
	REAL allZero[2] = {0, 0};
	INT compoundCount;
	REAL compounds[4] = {0, 0, 0, 0};

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Two compounds - all 1.
	status = GdipSetPenCompoundArray (pen, fourCompounds, 2);
	assert (status == Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assert (status == Ok);
	assert (compoundCount == 2);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount - 1);
	assert (status == Ok);
	assert (compounds[0] == 1);
	assert (compounds[1] == 0);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assert (status == Ok);
	assert (compounds[0] == 1);
	assert (compounds[1] == 1);

	// Two compounds - all 0.
	status = GdipSetPenCompoundArray (pen, allZero, 2);
	assert (status == Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assert (status == Ok);
	assert (compoundCount == 2);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assert (status == Ok);
	assert (compounds[0] == 0);
	assert (compounds[1] == 0);

	// Four compounds.
	status = GdipSetPenCompoundArray (pen, fourCompounds, 4);
	assert (status == Ok);

	status = GdipGetPenCompoundCount (pen, &compoundCount);
	assert (status == Ok);
	assert (compoundCount == 4);

	status = GdipGetPenCompoundArray (pen, compounds, compoundCount);
	assert (status == Ok);
	assert (compounds[0] == 1);
	assert (compounds[1] == 1);
	assert (compounds[2] == 1);
	assert (compounds[3] == 1);

	// Negative tests.
	status = GdipSetPenCompoundArray (NULL, fourCompounds, 2);
	assert (status == InvalidParameter);

	status = GdipSetPenCompoundArray (pen, NULL, 2);
	assert (status == InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 3);
	assert (status == InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 1);
	assert (status == InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, 0);
	assert (status == InvalidParameter);

	status = GdipSetPenCompoundArray (pen, fourCompounds, -1);
	assert (status == InvalidParameter);

	GdipDeletePen (pen);
}

static void test_getPenCompoundArray ()
{
	GpStatus status;
	GpPen *pen;
	REAL compounds[3];
	REAL twoCompounds[2] = {1, 1};

	GdipCreatePen1 (1, 10, UnitWorld, &pen);

	// Negative tests.
	status = GdipGetPenCompoundArray (NULL, compounds, 2);
	assert (status == InvalidParameter);

	status = GdipGetPenCompoundArray (NULL, compounds, -1);
	assert (status == InvalidParameter);

	status = GdipGetPenCompoundArray (pen, NULL, 2);
	assert (status == InvalidParameter);

	status = GdipGetPenCompoundArray (pen, NULL, -1);
	assert (status == InvalidParameter);

	status = GdipGetPenCompoundArray (pen, compounds, 1);
	assert (status == InvalidParameter);

	// Empty compounds.
	status = GdipGetPenCompoundArray (pen, compounds, 1);
	assert (status == InvalidParameter);

	status = GdipGetPenCompoundArray (pen, compounds, 0);
	assert (status == Ok);

	status = GdipGetPenCompoundArray (pen, compounds, -1);
	assert (status == Ok);

	// Non-empty compounds.
	status = GdipSetPenCompoundArray (pen, twoCompounds, 2);
	assert (status == Ok);

	status = GdipGetPenCompoundArray (pen, compounds, 3);
	assert (status == InvalidParameter);

	status = GdipGetPenCompoundArray (pen, compounds, 0);
	assert (status == Ok);

	status = GdipGetPenCompoundArray (pen, compounds, -1);
	assert (status == Ok);

	GdipDeletePen (pen);
}

static void test_deletePen ()
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
	test_createPen2 ();
	test_clone ();
	test_getPenWidth ();
	test_setPenWidth ();
	test_setPenUnit ();
	test_getPenUnit ();
	test_setPenLineCap197819 ();
	test_setPenStartCap ();
	test_setPenEndCap ();
	tet_setPenDashCap197819 ();
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

	GdiplusShutdown (gdiplusToken);
	return 0;
}
