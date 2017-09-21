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
	test_multiplyPenTransform ();
	test_translatePenTransform ();
	test_scalePenTransform ();
	test_rotatePenTransform ();
	test_deletePen ();

	GdiplusShutdown (gdiplusToken);
	return 0;
}
