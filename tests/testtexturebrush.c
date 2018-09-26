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

const WCHAR bitmapFile[] = {'t', 'e', 's', 't', '.', 'b', 'm', 'p', 0};
const WCHAR wmfFile[] = {'t', 'e', 's', 't', '.', 'w', 'm', 'f', 0};
const WCHAR emfFile[] = {'t', 'e', 's', 't', '.', 'e', 'm', 'f', 0};

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
    assertEqualInt (brushType, BrushTypeTextureFill);

    status = GdipGetTextureWrapMode (brush, &wrapMode);
    assertEqualInt (status, Ok);
    assertEqualInt (wrapMode, expectedWrapMode);

    status = GdipGetTextureImage (brush, &brushImage);
    assertEqualInt (status, Ok);

    GdipGetImageDimension (brushImage, &brushWidth, &brushHeight);
    assertEqualFloat (brushWidth, expectedBrushWidth);
    assertEqualFloat (brushHeight, expectedBrushHeight);

    status = GdipGetTextureTransform (brush, brushTransform);
    assertEqualInt (status, Ok);
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

    GdipLoadImageFromFile (bitmapFile, &image1);

    GdipCreateTexture (image1, WrapModeTile, &brush);

    status = GdipCloneBrush ((GpBrush *) brush, &clone);
    assertEqualInt (status, Ok);
    assert (clone && brush != clone);

    GdipGetBrushType (clone, &brushType);
    assertEqualInt (brushType, BrushTypeTextureFill);

    GdipGetTextureImage ((GpTexture *) clone, &image2);

    GdipGetImageDimension (image2, &brushWidth, &brushHeight);
    assertEqualFloat (brushWidth, 100);
    assertEqualFloat (brushHeight, 68);

    GdipGetTextureWrapMode ((GpTexture *) clone, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTile);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush (clone);
    GdipDisposeImage (image1);
    GdipDisposeImage (image2);
}

static void test_delete ()
{
    GpStatus status;
    GpTexture *brush;
    GpImage *image;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    assertEqualInt (status, Ok);

    GdipDisposeImage (image);
}

static void test_createTexture ()
{
    GpStatus status;
    GpImage *bitmapImage;
    GpImage *wmfImage;
    GpImage *emfImage;
    GpTexture *brush;

    GdipLoadImageFromFile (bitmapFile, &bitmapImage);
    GdipLoadImageFromFile (wmfFile, &wmfImage);
    GdipLoadImageFromFile (emfFile, &emfImage);

    // Bitmap image - WrapModeTile.
    status = GdipCreateTexture (bitmapImage, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipX.
    status = GdipCreateTexture (bitmapImage, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipY.
    status = GdipCreateTexture (bitmapImage, WrapModeTileFlipY, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipXY.
    status = GdipCreateTexture (bitmapImage, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeClamp.
    status = GdipCreateTexture (bitmapImage, WrapModeClamp, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTile.
    status = GdipCreateTexture (wmfImage, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipX.
    status = GdipCreateTexture (wmfImage, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipY.
    status = GdipCreateTexture (wmfImage, WrapModeTileFlipY, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipXY.
    status = GdipCreateTexture (wmfImage, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeClamp.
    status = GdipCreateTexture (wmfImage, WrapModeClamp, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTile.
    status = GdipCreateTexture (emfImage, WrapModeTile, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipX.
    status = GdipCreateTexture (emfImage, WrapModeTileFlipX, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipY.
    status = GdipCreateTexture (emfImage, WrapModeTileFlipY, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipXY.
    status = GdipCreateTexture (emfImage, WrapModeTileFlipXY, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeClamp.
    status = GdipCreateTexture (emfImage, WrapModeClamp, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (NULL, WrapModeClamp, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (NULL, (GpWrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (bitmapImage, (GpWrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (wmfImage, (GpWrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (emfImage, (GpWrapMode)(WrapModeClamp + 1), &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (bitmapImage, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (wmfImage, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (emfImage, WrapModeClamp, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);
}

static void test_createTexture2 ()
{
    GpStatus status;
    GpImage *bitmapImage;
    GpImage *wmfImage;
    GpImage *emfImage;
    GpTexture *brush;

    GdipLoadImageFromFile (bitmapFile, &bitmapImage);
    GdipLoadImageFromFile (wmfFile, &wmfImage);
    GdipLoadImageFromFile (emfFile, &emfImage);

    // Bitmap image - WrapModeTile.
    status = GdipCreateTexture2 (bitmapImage, WrapModeTile, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipX.
    status = GdipCreateTexture2 (bitmapImage, WrapModeTileFlipX, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipY.
    status = GdipCreateTexture2 (bitmapImage, WrapModeTileFlipY, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipXY.
    status = GdipCreateTexture2 (bitmapImage, WrapModeTileFlipXY, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - clamp.
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 5, 6, 7, 8, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTile, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTile, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTileFlipX, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTileFlipY, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTileFlipXY, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - clamp, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTile, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTile, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTileFlipX, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTileFlipY, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTileFlipXY, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeClamp, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 50, 60, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 0, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, -1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 1, 0, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 1, -1, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (wmfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (emfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, -1, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, -1, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, -1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 0, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, 0, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, -1, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 101, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 1, 0, 100, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 100, 69, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 1, 100, 68, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 0, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, -1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, 0, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, -1, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);
}

static void test_createTexture2I ()
{
    GpStatus status;
    GpImage *bitmapImage;
    GpImage *wmfImage;
    GpImage *emfImage;
    GpTexture *brush;

    GdipLoadImageFromFile (bitmapFile, &bitmapImage);
    GdipLoadImageFromFile (wmfFile, &wmfImage);
    GdipLoadImageFromFile (emfFile, &emfImage);

    // Bitmap image - WrapModeTile.
    status = GdipCreateTexture2I (bitmapImage, WrapModeTile, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipX.
    status = GdipCreateTexture2I (bitmapImage, WrapModeTileFlipX, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipXY.
    status = GdipCreateTexture2I (bitmapImage, WrapModeTileFlipXY, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipY.
    status = GdipCreateTexture2I (bitmapImage, WrapModeTileFlipY, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - clamp.
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 5, 6, 7, 8, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTile, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTile, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTileFlipX, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTileFlipY, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTileFlipXY, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - clamp, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTile, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTile, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTileFlipX, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipX, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTileFlipY, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTileFlipXY, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - tile, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTile, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeClamp, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 50, 60, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 0, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, -1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 1, 0, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 1, -1, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (wmfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (emfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, -1, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, -1, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, -1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 0, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 1, 0, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 1, -1, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 101, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 1, 0, 100, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 100, 69, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, 68, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, 68, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 1, 100, 68, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 0, 68, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, -1, 68, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, 0, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, -1, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);
}

static void test_createTextureIA ()
{
    GpStatus status;
    GpImage *bitmapImage;
    GpImage *wmfImage;
    GpImage *emfImage;
    GpImageAttributes *attributes;
    GpImageAttributes *customAttributes;
    GpImageAttributes *customAttributesClamp;
    GpImageAttributes *customAttributesInvalidWrap;
    GpTexture *brush;

    GdipLoadImageFromFile (bitmapFile, &bitmapImage);
    GdipLoadImageFromFile (wmfFile, &wmfImage);
    GdipLoadImageFromFile (emfFile, &emfImage);
    GdipCreateImageAttributes (&attributes);
    GdipCreateImageAttributes (&customAttributes);
    GdipCreateImageAttributes (&customAttributesClamp);
    GdipCreateImageAttributes (&customAttributesInvalidWrap);
    GdipSetImageAttributesWrapMode (customAttributes, WrapModeTileFlipXY, 0xFF00FF00, FALSE);
    GdipSetImageAttributesWrapMode (customAttributesClamp, WrapModeTileFlipXY, 0xFF00FF00, TRUE);
    GdipSetImageAttributesWrapMode (customAttributesInvalidWrap, (WrapMode)(WrapModeClamp + 1), 0xFF00FF00, TRUE);

    // Bitmap image - full size.
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - custom size.
    status = GdipCreateTextureIA (bitmapImage, NULL, 5, 6, 7, 8, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with default wrap mode.
    status = GdipCreateTextureIA (bitmapImage, attributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode.
    status = GdipCreateTextureIA (bitmapImage, customAttributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIA (bitmapImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - full.
    status = GdipCreateTextureIA (wmfImage, NULL, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - small size.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTextureIA (wmfImage, NULL, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTextureIA (wmfImage, NULL, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with default wrap mode.
    status = GdipCreateTextureIA (wmfImage, attributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode.
    status = GdipCreateTextureIA (wmfImage, customAttributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIA (wmfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIA (emfImage, NULL, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 50, 60, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTextureIA (emfImage, NULL, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTextureIA (emfImage, NULL, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with default wrap mode.
    status = GdipCreateTextureIA (emfImage, attributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode.
    status = GdipCreateTextureIA (emfImage, customAttributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIA (emfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, NULL, 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 0, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, -1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 1, 0, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 1, -1, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (wmfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (emfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    status = GdipCreateTextureIA (bitmapImage, NULL, -1, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, -1, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, -1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 0, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 1, 0, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 1, -1, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 101, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 1, 0, 100, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 100, 69, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 1, 100, 68, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 0, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, -1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 0, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, -1, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);
    GdipDisposeImageAttributes (attributes);
    GdipDisposeImageAttributes (customAttributes);
    GdipDisposeImageAttributes (customAttributesClamp);
    GdipDisposeImageAttributes (customAttributesInvalidWrap);
}

static void test_createTextureIAI ()
{
    GpStatus status;
    GpImage *bitmapImage;
    GpImage *wmfImage;
    GpImage *emfImage;
    GpImageAttributes *attributes;
    GpImageAttributes *customAttributes;
    GpImageAttributes *customAttributesClamp;
    GpImageAttributes *customAttributesInvalidWrap;
    GpTexture *brush;

    GdipLoadImageFromFile (bitmapFile, &bitmapImage);
    GdipLoadImageFromFile (wmfFile, &wmfImage);
    GdipLoadImageFromFile (emfFile, &emfImage);
    GdipCreateImageAttributes (&attributes);
    GdipCreateImageAttributes (&customAttributes);
    GdipCreateImageAttributes (&customAttributesClamp);
    GdipCreateImageAttributes (&customAttributesInvalidWrap);
    GdipSetImageAttributesWrapMode (customAttributes, WrapModeTileFlipXY, 0xFF00FF00, FALSE);
    GdipSetImageAttributesWrapMode (customAttributesClamp, WrapModeTileFlipXY, 0xFF00FF00, TRUE);
    GdipSetImageAttributesWrapMode (customAttributesInvalidWrap, (WrapMode)(WrapModeClamp + 1), 0xFF00FF00, TRUE);

    // Bitmap image - full size.
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 100, 68, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - custom size.
    status = GdipCreateTextureIAI (bitmapImage, NULL, 5, 6, 7, 8, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with default wrap mode.
    status = GdipCreateTextureIAI (bitmapImage, attributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode.
    status = GdipCreateTextureIAI (bitmapImage, customAttributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIAI (bitmapImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - full.
    status = GdipCreateTextureIAI (wmfImage, NULL, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - small size.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTextureIAI (wmfImage, NULL, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTextureIAI (wmfImage, NULL, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with default wrap mode.
    status = GdipCreateTextureIAI (wmfImage, attributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode.
    status = GdipCreateTextureIAI (wmfImage, customAttributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIAI (wmfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIAI (emfImage, NULL, 0, 0, 100, 100, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 50, 60, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTextureIAI (emfImage, NULL, -10, -10, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTextureIAI (emfImage, NULL, 2000, 2000, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 0, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 2, 0, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, -1, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 2, -1, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with default wrap mode.
    status = GdipCreateTextureIAI (emfImage, attributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode.
    status = GdipCreateTextureIAI (emfImage, customAttributes, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIAI (emfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    assertEqualInt (status, Ok);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, NULL, 0, 0, 1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 0, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, -1, 2, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 1, 0, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 1, -1, &brush);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (wmfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (emfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    status = GdipCreateTextureIAI (bitmapImage, NULL, -1, 0, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, -1, 1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, -1, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 0, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 1, 0, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 1, -1, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 101, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 1, 0, 100, 2, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 100, 69, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 1, 100, 68, &brush);
    assertEqualInt (status, OutOfMemory);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 0, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, -1, 2, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 0, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, -1, NULL);
    assertEqualInt (status, InvalidParameter);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);
    GdipDisposeImageAttributes (attributes);
    GdipDisposeImageAttributes (customAttributes);
    GdipDisposeImageAttributes (customAttributesClamp);
    GdipDisposeImageAttributes (customAttributesInvalidWrap);
}

static void test_getTextureImage ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpImage *brushImage1;
    GpImage *brushImage2;

    GdipLoadImageFromFile (bitmapFile, &image);

    GdipCreateTexture (image, WrapModeTile, &brush);

    status = GdipGetTextureImage (brush, &brushImage1);
    assertEqualInt (status, Ok);
    assert (brushImage1 != image && "The texture image should be a clone.");

    status = GdipGetTextureImage (brush, &brushImage2);
    assertEqualInt (status, Ok);
    assert (brushImage2 != brushImage1 && "The texture image should be a clone.");

    // Negative tests.
    status = GdipGetTextureImage (NULL, &brushImage1);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetTextureImage (brush, NULL);
    assertEqualInt (status, InvalidParameter);

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

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);

    // Negative tests.
    status = GdipGetTextureWrapMode (NULL, &wrapMode);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetTextureWrapMode (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
}

static void test_setTextureWrapMode ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpWrapMode wrapMode;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);

    // WrapModeTileFlipX.
    status = GdipSetTextureWrapMode (brush, WrapModeTileFlipX);
    assertEqualInt (status, Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipX);

    // WrapModeTileFlipY.
    status = GdipSetTextureWrapMode (brush, WrapModeTileFlipY);
    assertEqualInt (status, Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipY);

    // WrapModeTileFlipXY.
    status = GdipSetTextureWrapMode (brush, WrapModeTileFlipXY);
    assertEqualInt (status, Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeTileFlipXY);

    // WrapModeClamp.
    status = GdipSetTextureWrapMode (brush, WrapModeClamp);
    assertEqualInt (status, Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeClamp);

    // Invalid WrapMode - nop.
    status = GdipSetTextureWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    assertEqualInt (status, Ok);

    GdipGetTextureWrapMode (brush, &wrapMode);
    assertEqualInt (wrapMode, WrapModeClamp);

    // Negative tests.
    status = GdipSetTextureWrapMode (NULL, wrapMode);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
}

static void test_getTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *transform;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix (&transform);

    // Negative tests.
    status = GdipGetTextureTransform (NULL, transform);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetTextureTransform (brush, NULL);
    assertEqualInt (status, InvalidParameter);

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
    GpMatrix *nonInvertibleMatrix;
    GpMatrix *transform;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

    status = GdipSetTextureTransform (brush, matrix);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Negative tests.
    status = GdipSetTextureTransform (NULL, transform);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetTextureTransform (brush, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetTextureTransform (brush, nonInvertibleMatrix);
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);
}

static void test_resetTextureTransform ()
{
    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *matrix;
    GpMatrix *transform;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &matrix);
    GdipCreateMatrix (&transform);

    GdipSetTextureTransform (brush, matrix);

    status = GdipResetTextureTransform (brush);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipResetTextureTransform (NULL);
    assertEqualInt (status, InvalidParameter);

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

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix2 (2, 3, 4, 5, 6, 7, &matrix);
    GdipCreateMatrix2 (123, 24, 82, 16, 47, 30, &nonInvertibleMatrix);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, MatrixOrderAppend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix.
    status = GdipMultiplyTextureTransform (brush, NULL, MatrixOrderAppend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - negative.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Invalid MatrixOrder - positive.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Negative tests.
    status = GdipMultiplyTextureTransform (NULL, matrix, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipMultiplyTextureTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

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

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipTranslateTextureTransform (brush, 5, 6, MatrixOrderAppend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipTranslateTextureTransform (brush, 5, 6, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslateTextureTransform (NULL, 1, 1, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipTranslateTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipTranslateTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

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

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipScaleTextureTransform (brush, 0.5, 0.75, MatrixOrderAppend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipScaleTextureTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScaleTextureTransform (NULL, 1, 1, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipScaleTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipScaleTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

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

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix2 (1, 2, 3, 4, 5, 6, &originalTransform);
    GdipCreateMatrix (&transform);

    // MatrixOrderAppend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipRotateTextureTransform (brush, 90, MatrixOrderAppend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipRotateTextureTransform (brush, 90, MatrixOrderPrepend);
    assertEqualInt (status, Ok);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotateTextureTransform (NULL, 90, MatrixOrderAppend);
    assertEqualInt (status, InvalidParameter);

    status = GdipRotateTextureTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    assertEqualInt (status, InvalidParameter);

    status = GdipRotateTextureTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    assertEqualInt (status, InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);
}

int
main (int argc, char**argv)
{
    STARTUP;

    test_clone ();
    test_delete ();
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

    SHUTDOWN;
    return 0;
}
