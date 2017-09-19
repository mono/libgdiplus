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

static void verifyTexture (GpTexture *brush, GpWrapMode expectedWrapMode, REAL expectedBrushWidth, REAL expectedBrushHeight)
{
    assert (brush && "Expected the brush to be initialized.");

    GpStatus status;
    GpBrushType brushType;
    GpWrapMode wrapMode;
    GpImage *brushImage;
    REAL brushWidth;
    REAL brushHeight;
    GpMatrix *brushTransform;

    GdipCreateMatrix (&brushTransform);

    status = GdipGetBrushType (brush, &brushType);
    assert (brushType == BrushTypeTextureFill);

    status = GdipGetTextureWrapMode (brush, &wrapMode);
    assert (status == Ok);
    assert (wrapMode == expectedWrapMode);

    status = GdipGetTextureImage (brush, &brushImage);
    assert (status == Ok);

    GdipGetImageDimension (brushImage, &brushWidth, &brushHeight);
    assert (brushWidth == expectedBrushWidth);
    assert (brushHeight == expectedBrushHeight);

    status = GdipGetTextureTransform (brush, brushTransform);
    assert (status == Ok);
    verifyMatrix (brushTransform, 1, 0, 0, 1, 0, 0);

    GdipDisposeImage (brushImage);
    GdipDeleteMatrix (brushTransform);
}

static void test_clone ()
{
    GpStatus status;
    GpTexture *brush;
    GpBrushType brushType;
    GpBrush *clone;

    GpImage *image1;
    GpImage *image2;
    REAL brushWidth;
    REAL brushHeight;
    GpWrapMode wrapMode;

    image1 = getImage ();

    GdipCreateTexture (image1, WrapModeTile, &brush);

    status = GdipCloneBrush ((GpBrush *) brush, &clone);
    assert (status == Ok);
    assert (clone && brush != clone);

    GdipGetBrushType (clone, &brushType);
    assert (brushType == BrushTypeTextureFill);

    GdipGetTextureImage ((GpTexture *) clone, &image2);

    GdipGetImageDimension (image2, &brushWidth, &brushHeight);
    assert (brushWidth == 100);
    assert (brushHeight == 68);

    GdipGetTextureWrapMode ((GpTexture *) clone, &wrapMode);
    assert (wrapMode == WrapModeTile);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush (clone);
    GdipDisposeImage (image1);
    GdipDisposeImage (image2);
}

static void test_createTexture ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;

    image = getImage ();

    status = GdipCreateTexture (image, WrapModeClamp, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeClamp, 100, 68);

    status = GdipCreateTexture (NULL, WrapModeClamp, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateTexture (image, (GpWrapMode)(WrapModeTile - 1), &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture (image, (GpWrapMode)(WrapModeClamp + 1), &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture (image, WrapModeClamp, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
}

static void test_createTexture2 ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;

    image = getImage ();

    status = GdipCreateTexture2 (image, WrapModeTile, 0, 0, 100, 68, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTexture2 (image, WrapModeClamp, 5, 6, 7, 8, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeClamp, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 1, 2, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateTexture2 (image, (WrapMode)(WrapModeTile - 1), 0, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, -1, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, -1, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, 0, -1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, 0, 0, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, 0, 1, 0, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, 0, 1, -1, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, 0, 101, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 1, 0, 100, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, 0, 100, 69, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2 (image, WrapModeClamp, 0, 1, 100, 68, &brush);
    assert (status == OutOfMemory);
}

static void test_createTexture2I ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;

    image = getImage ();

    status = GdipCreateTexture2I (image, WrapModeTile, 0, 0, 100, 68, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTexture2I (image, WrapModeClamp, 5, 6, 7, 8, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeClamp, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 1, 2, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateTexture2I (image, (WrapMode)(WrapModeTile - 1), 0, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, -1, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, -1, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, 0, -1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, 0, 0, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, 0, 1, 0, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, 0, 1, -1, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, 0, 101, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 1, 0, 100, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, 0, 100, 69, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTexture2I (image, WrapModeClamp, 0, 1, 100, 68, &brush);
    assert (status == OutOfMemory);
}

static void test_createTextureIA ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;

    image = getImage ();

    status = GdipCreateTextureIA (image, NULL, 0, 0, 100, 68, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTextureIA (image, NULL, 5, 6, 7, 8, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeTile, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTextureIA (NULL, NULL, 0, 0, 1, 2, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateTextureIA (image, NULL, -1, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, -1, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, 0, -1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, 0, 0, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, 0, 1, 0, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, 0, 1, -1, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, 0, 101, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 1, 0, 100, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, 0, 100, 69, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIA (image, NULL, 0, 1, 100, 68, &brush);
    assert (status == OutOfMemory);
}

static void test_createTextureIAI ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;

    image = getImage ();

    status = GdipCreateTextureIAI (image, NULL, 0, 0, 100, 68, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTextureIAI (image, NULL, 5, 6, 7, 8, &brush);
    assert (status == Ok);
    verifyTexture (brush, WrapModeTile, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    status = GdipCreateTextureIAI (NULL, NULL, 0, 0, 1, 2, &brush);
    assert (status == InvalidParameter);

    status = GdipCreateTextureIAI (image, NULL, -1, 0, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, -1, 1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, 0, -1, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, 0, 0, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, 0, 1, 0, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, 0, 1, -1, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, 0, 101, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 1, 0, 100, 2, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, 0, 100, 69, &brush);
    assert (status == OutOfMemory);

    status = GdipCreateTextureIAI (image, NULL, 0, 1, 100, 68, &brush);
    assert (status == OutOfMemory);
}

static void test_getTextureImage ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpImage *brushImage1;
    GpImage *brushImage2;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);

    status = GdipGetTextureImage (brush, &brushImage1);
    assert (status == Ok);
    assert (brushImage1 != image && "The texture image should be a clone.");

    status = GdipGetTextureImage (brush, &brushImage2);
    assert (status == Ok);
    assert (brushImage2 != brushImage1 && "The texture image should be a clone.");

    status = GdipGetTextureImage (NULL, &brushImage1);
    assert (status == InvalidParameter);

    status = GdipGetTextureImage (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDisposeImage (brushImage1);
    GdipDisposeImage (brushImage2);
}

static void test_getTextureWrapMode ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpWrapMode wrapMode;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);

    status = GdipGetTextureWrapMode (NULL, &wrapMode);
    assert (status == InvalidParameter);

    status = GdipGetTextureWrapMode (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
}

static void test_setTextureWrapMode ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpWrapMode wrapMode;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);

    status = GdipSetTextureWrapMode (brush, WrapModeTileFlipX);
    assert (status == Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipX);

    // Setting to an invalid WrapMode is a nop.
    status = GdipSetTextureWrapMode (brush, (WrapMode)(WrapModeTile - 1));
    assert (status == Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipX);

    status = GdipSetTextureWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    assert (status == Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assert (wrapMode == WrapModeTileFlipX);

    // Negative tests.
    status = GdipSetTextureWrapMode (NULL, wrapMode);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
}

static void test_getTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *transform;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix (&transform);

    status = GdipGetTextureTransform (NULL, transform);
    assert (status == InvalidParameter);

    status = GdipGetTextureTransform (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (transform);
}

static void test_setTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *matrix;
    GpMatrix *transform;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    status = GdipSetTextureTransform (brush, matrix);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Negative tests.
    status = GdipSetTextureTransform (NULL, transform);
    assert (status == InvalidParameter);

    status = GdipSetTextureTransform (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (transform);
}

static void test_resetTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *matrix;
    GpMatrix *transform;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    GdipSetTextureTransform (brush, matrix);

    status = GdipResetTextureTransform (brush);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    status = GdipResetTextureTransform (NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (matrix);
}

static void test_multiplyTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *originalTransform;
    GpMatrix *matrix;
    GpMatrix *nonInvertibleMatrix;
    GpMatrix *transform;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix.
    status = GdipMultiplyTextureTransform (brush, NULL, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - this produces garbage data.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == Ok);

    status = GdipMultiplyTextureTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == Ok);

    // Negative tests.
    status = GdipMultiplyTextureTransform (NULL, matrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipMultiplyTextureTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);
}

static void test_translateTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *originalTransform;
    GpMatrix *transform;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipTranslateTextureTransform (brush, 5, 6, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipTranslateTextureTransform (brush, 5, 6, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslateTextureTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipTranslateTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipTranslateTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_scaleTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *originalTransform;
    GpMatrix *transform;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipScaleTextureTransform (brush, 0.5, 0.75, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipScaleTextureTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScaleTextureTransform (NULL, 1, 1, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipScaleTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipScaleTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

static void test_rotateTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *originalTransform;
    GpMatrix *transform;

    image = getImage ();

    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipRotateTextureTransform (brush, 90, MatrixOrderAppend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipRotateTextureTransform (brush, 90, MatrixOrderPrepend);
    assert (status == Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotateTextureTransform (NULL, 90, MatrixOrderAppend);
    assert (status == InvalidParameter);

    status = GdipRotateTextureTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    assert (status == InvalidParameter);

    status = GdipRotateTextureTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

int
main (int argc, char**argv)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

    test_clone ();
    test_createTexture ();
    test_createTexture2 ();
    test_createTexture2I ();
    test_createTextureIA ();
    test_createTextureIAI ();
    test_getTextureImage ();
    test_getTextureWrapMode ();
    test_setTextureWrapMode ();
    test_getTextureTransform ();
    test_setTextureTransform ();
    test_resetTextureTransform ();
    test_multiplyTextureTransform ();
    test_translateTextureTransform ();
    test_scaleTextureTransform ();
    test_rotateTextureTransform ();

    GdiplusShutdown (gdiplusToken);
    return 0;
}
