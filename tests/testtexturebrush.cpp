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
    ASSERT_EQ (BrushTypeTextureFill, brushType);

    status = GdipGetTextureWrapMode (brush, &wrapMode);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (expectedWrapMode, wrapMode);

    status = GdipGetTextureImage (brush, &brushImage);
    ASSERT_EQ (Ok, status);

    GdipGetImageDimension (brushImage, &brushWidth, &brushHeight);
    ASSERT_FLOAT_EQ_NAN (expectedBrushWidth, brushWidth);
    ASSERT_FLOAT_EQ_NAN (expectedBrushHeight, brushHeight);

    status = GdipGetTextureTransform (brush, brushTransform);
    ASSERT_EQ (Ok, status);
    verifyMatrix (brushTransform, 1, 0, 0, 1, 0, 0);

    GdipDisposeImage (brushImage);
    GdipDeleteMatrix (brushTransform);
}

TEST(TextureBrush, GdipCloneBrush)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    assert (clone && brush != clone);

    GdipGetBrushType (clone, &brushType);
    ASSERT_EQ (BrushTypeTextureFill, brushType);

    GdipGetTextureImage ((GpTexture *) clone, &image2);

    GdipGetImageDimension (image2, &brushWidth, &brushHeight);
    ASSERT_FLOAT_EQ_NAN (100, brushWidth);
    ASSERT_FLOAT_EQ_NAN (68, brushHeight);

    GdipGetTextureWrapMode ((GpTexture *) clone, &wrapMode);
    ASSERT_EQ (WrapModeTile, wrapMode);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush (clone);
    GdipDisposeImage (image1);
    GdipDisposeImage (image2);

    SHUTDOWN
}

TEST(TextureBrush, GdipDeleteBrush)
{
    STARTUP

    GpStatus status;
    GpTexture *brush;
    GpImage *image;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    ASSERT_EQ (Ok, status);

    GdipDisposeImage (image);

    SHUTDOWN
}

TEST(TextureBrush, GdipCreateTexture)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipX.
    status = GdipCreateTexture (bitmapImage, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipY.
    status = GdipCreateTexture (bitmapImage, WrapModeTileFlipY, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipXY.
    status = GdipCreateTexture (bitmapImage, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeClamp.
    status = GdipCreateTexture (bitmapImage, WrapModeClamp, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 68);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTile.
    status = GdipCreateTexture (wmfImage, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipX.
    status = GdipCreateTexture (wmfImage, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipY.
    status = GdipCreateTexture (wmfImage, WrapModeTileFlipY, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipXY.
    status = GdipCreateTexture (wmfImage, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeClamp.
    status = GdipCreateTexture (wmfImage, WrapModeClamp, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTile.
    status = GdipCreateTexture (emfImage, WrapModeTile, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipX.
    status = GdipCreateTexture (emfImage, WrapModeTileFlipX, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipY.
    status = GdipCreateTexture (emfImage, WrapModeTileFlipY, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipXY.
    status = GdipCreateTexture (emfImage, WrapModeTileFlipXY, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeClamp.
    status = GdipCreateTexture (emfImage, WrapModeClamp, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);
    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (NULL, WrapModeClamp, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (NULL, (GpWrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (bitmapImage, (GpWrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (wmfImage, (GpWrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (emfImage, (GpWrapMode)(WrapModeClamp + 1), &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (bitmapImage, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (wmfImage, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture (emfImage, WrapModeClamp, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);

    SHUTDOWN
}

TEST(TextureBrush, GdipCreateTexture2)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipX.
    status = GdipCreateTexture2 (bitmapImage, WrapModeTileFlipX, 0, 0, 100, 68, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipY.
    status = GdipCreateTexture2 (bitmapImage, WrapModeTileFlipY, 0, 0, 100, 68, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipXY.
    status = GdipCreateTexture2 (bitmapImage, WrapModeTileFlipXY, 0, 0, 100, 68, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - clamp.
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 5, 6, 7, 8, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTile, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTile, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTileFlipX, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTileFlipY, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeTileFlipXY, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - clamp, non zero.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTexture2 (wmfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTile, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTile, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTileFlipX, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTileFlipY, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeTileFlipXY, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeClamp, non zero.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 50, 60, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTexture2 (emfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 0, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, -1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 1, 0, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (NULL, WrapModeClamp, 0, 0, 1, -1, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (wmfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (emfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, -1, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, -1, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, -1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 0, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, 0, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, -1, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 101, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 1, 0, 100, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 100, 69, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 1, 100, 68, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 0, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, -1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, 0, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2 (bitmapImage, WrapModeClamp, 0, 0, 1, -1, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);

    SHUTDOWN
}

TEST(TextureBrush, GdipCreateTexture2I)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipX.
    status = GdipCreateTexture2I (bitmapImage, WrapModeTileFlipX, 0, 0, 100, 68, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipXY.
    status = GdipCreateTexture2I (bitmapImage, WrapModeTileFlipXY, 0, 0, 100, 68, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - WrapModeTileFlipY.
    status = GdipCreateTexture2I (bitmapImage, WrapModeTileFlipY, 0, 0, 100, 68, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - clamp.
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 5, 6, 7, 8, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTile, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTile, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTileFlipX, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTileFlipY, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeTileFlipXY, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - clamp, non zero.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTexture2I (wmfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTile, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTile, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipX, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTileFlipX, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipX, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipY, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTileFlipY, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeTileFlipXY, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTileFlipXY, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - tile, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeTile, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - WrapModeClamp, non zero.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 50, 60, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTexture2I (emfImage, WrapModeClamp, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 0, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, -1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 1, 0, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (NULL, WrapModeClamp, 0, 0, 1, -1, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (wmfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (emfImage, (WrapMode)(WrapModeClamp + 1), 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, -1, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, -1, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, -1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 0, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 1, 0, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 1, -1, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 101, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 1, 0, 100, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 0, 100, 69, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, 68, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, 68, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, (WrapMode)(WrapModeClamp + 1), 0, 1, 100, 68, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 0, 68, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, -1, 68, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, 0, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTexture2I (bitmapImage, WrapModeClamp, 0, 1, 100, -1, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);

    SHUTDOWN
}

TEST(TextureBrush, GdipCreateTextureIA)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - custom size.
    status = GdipCreateTextureIA (bitmapImage, NULL, 5, 6, 7, 8, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with default wrap mode.
    status = GdipCreateTextureIA (bitmapImage, attributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode.
    status = GdipCreateTextureIA (bitmapImage, customAttributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIA (bitmapImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - full.
    status = GdipCreateTextureIA (wmfImage, NULL, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - small size.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTextureIA (wmfImage, NULL, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTextureIA (wmfImage, NULL, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTextureIA (wmfImage, NULL, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with default wrap mode.
    status = GdipCreateTextureIA (wmfImage, attributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode.
    status = GdipCreateTextureIA (wmfImage, customAttributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIA (wmfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIA (emfImage, NULL, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 50, 60, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTextureIA (emfImage, NULL, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTextureIA (emfImage, NULL, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTextureIA (emfImage, NULL, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with default wrap mode.
    status = GdipCreateTextureIA (emfImage, attributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode.
    status = GdipCreateTextureIA (emfImage, customAttributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIA (emfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, NULL, 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 0, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, -1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 1, 0, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (NULL, customAttributesInvalidWrap, 0, 0, 1, -1, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (wmfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (emfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    status = GdipCreateTextureIA (bitmapImage, NULL, -1, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, -1, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, -1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 0, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 1, 0, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 1, -1, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 101, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 1, 0, 100, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 100, 69, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 1, 100, 68, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, NULL, 0, 0, 1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 0, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, -1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 0, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIA (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, -1, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);
    GdipDisposeImageAttributes (attributes);
    GdipDisposeImageAttributes (customAttributes);
    GdipDisposeImageAttributes (customAttributesClamp);
    GdipDisposeImageAttributes (customAttributesInvalidWrap);

    SHUTDOWN
}

TEST(TextureBrush, GdipCreateTextureIAI)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 68);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - custom size.
    status = GdipCreateTextureIAI (bitmapImage, NULL, 5, 6, 7, 8, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 7, 8);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with default wrap mode.
    status = GdipCreateTextureIAI (bitmapImage, attributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode.
    status = GdipCreateTextureIAI (bitmapImage, customAttributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Bitmap image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIAI (bitmapImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - full.
    status = GdipCreateTextureIAI (wmfImage, NULL, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - small size.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - negative x and y.
    status = GdipCreateTextureIAI (wmfImage, NULL, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - large x and y.
    status = GdipCreateTextureIAI (wmfImage, NULL, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero width.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, zero height.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative width.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image, negative height.
    status = GdipCreateTextureIAI (wmfImage, NULL, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 770, 649);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with default wrap mode.
    status = GdipCreateTextureIAI (wmfImage, attributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode.
    status = GdipCreateTextureIAI (wmfImage, customAttributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Wmf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIAI (wmfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIAI (emfImage, NULL, 0, 0, 100, 100, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - non zero.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 50, 60, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 50, 60);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - negative x and y.
    status = GdipCreateTextureIAI (emfImage, NULL, -10, -10, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - large x and y.
    status = GdipCreateTextureIAI (emfImage, NULL, 2000, 2000, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero width.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 0, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, zero height.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 2, 0, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative width.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, -1, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image, negative height.
    status = GdipCreateTextureIAI (emfImage, NULL, 1, 1, 2, -1, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTile, 100, 100);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with default wrap mode.
    status = GdipCreateTextureIAI (emfImage, attributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeClamp, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode.
    status = GdipCreateTextureIAI (emfImage, customAttributes, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Emf image - attributes with custom wrap mode and clamp.
    status = GdipCreateTextureIAI (emfImage, customAttributesClamp, 0, 0, 2, 2, &brush);
    ASSERT_EQ (Ok, status);
    verifyTexture (brush, WrapModeTileFlipXY, 2, 2);

    GdipDeleteBrush ((GpBrush *) brush);

    // Negative tests.
    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, NULL, 0, 0, 1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 0, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, -1, 2, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 1, 0, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (NULL, customAttributesInvalidWrap, 0, 0, 1, -1, &brush);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (wmfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (emfImage, customAttributesInvalidWrap, 0, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    status = GdipCreateTextureIAI (bitmapImage, NULL, -1, 0, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, -1, 1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, -1, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 0, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 1, 0, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 1, -1, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 101, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 1, 0, 100, 2, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 100, 69, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 1, 100, 68, &brush);
    ASSERT_EQ (OutOfMemory, status);
    assert (brush == NULL);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, NULL, 0, 0, 1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 0, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, -1, 2, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, 0, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    brush = (GpTexture *) 0xCC;
    status = GdipCreateTextureIAI (bitmapImage, customAttributesInvalidWrap, 0, 0, 1, -1, NULL);
    ASSERT_EQ (InvalidParameter, status);
    assert (brush == (GpTexture *) 0xCC);

    GdipDisposeImage (bitmapImage);
    GdipDisposeImage (wmfImage);
    GdipDisposeImage (emfImage);
    GdipDisposeImageAttributes (attributes);
    GdipDisposeImageAttributes (customAttributes);
    GdipDisposeImageAttributes (customAttributesClamp);
    GdipDisposeImageAttributes (customAttributesInvalidWrap);

    SHUTDOWN
}

TEST(TextureBrush, GdipGetTextureImage)
{
    STARTUP

    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpImage *brushImage1;
    GpImage *brushImage2;

    GdipLoadImageFromFile (bitmapFile, &image);

    GdipCreateTexture (image, WrapModeTile, &brush);

    status = GdipGetTextureImage (brush, &brushImage1);
    ASSERT_EQ (Ok, status);
    assert (brushImage1 != image && "The texture image should be a clone.");

    status = GdipGetTextureImage (brush, &brushImage2);
    ASSERT_EQ (Ok, status);
    assert (brushImage2 != brushImage1 && "The texture image should be a clone.");

    // Negative tests.
    status = GdipGetTextureImage (NULL, &brushImage1);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetTextureImage (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDisposeImage (brushImage1);
    GdipDisposeImage (brushImage2);

    SHUTDOWN
}

TEST(TextureBrush, GdipGetTextureWrapMode)
{
    STARTUP

    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpWrapMode wrapMode;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);

    // Negative tests.
    status = GdipGetTextureWrapMode (NULL, &wrapMode);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetTextureWrapMode (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);

    SHUTDOWN
}

TEST(TextureBrush, GdipSetTextureWrapMode)
{
    STARTUP

    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpWrapMode wrapMode;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);

    // WrapModeTileFlipX.
    status = GdipSetTextureWrapMode (brush, WrapModeTileFlipX);
    ASSERT_EQ (Ok, status);

    GdipGetTextureWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipX, wrapMode);

    // WrapModeTileFlipY.
    status = GdipSetTextureWrapMode (brush, WrapModeTileFlipY);
    ASSERT_EQ (Ok, status);

    GdipGetTextureWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipY, wrapMode);

    // WrapModeTileFlipXY.
    status = GdipSetTextureWrapMode (brush, WrapModeTileFlipXY);
    ASSERT_EQ (Ok, status);

    GdipGetTextureWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeTileFlipXY, wrapMode);

    // WrapModeClamp.
    status = GdipSetTextureWrapMode (brush, WrapModeClamp);
    ASSERT_EQ (Ok, status);

    GdipGetTextureWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeClamp, wrapMode);

    // Invalid WrapMode - nop.
    status = GdipSetTextureWrapMode (brush, (WrapMode)(WrapModeClamp + 1));
    ASSERT_EQ (Ok, status);

    GdipGetTextureWrapMode (brush, &wrapMode);
    ASSERT_EQ (WrapModeClamp, wrapMode);

    // Negative tests.
    status = GdipSetTextureWrapMode (NULL, wrapMode);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);

    SHUTDOWN
}

TEST(TextureBrush, GdipGetTextureTransform)
{
    STARTUP

    GpStatus status;
    GpImage *image;
    GpTexture *brush;
    GpMatrix *transform;

    GdipLoadImageFromFile (bitmapFile, &image);
    GdipCreateTexture (image, WrapModeTile, &brush);
    GdipCreateMatrix (&transform);

    // Negative tests.
    status = GdipGetTextureTransform (NULL, transform);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetTextureTransform (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(TextureBrush, GdipSetTextureTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    assert (transform != matrix && "Expected new matrix to be a clone.");
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Changing the original transform should not modify the brush's transform.
    GdipSetMatrixElements (matrix, 2, 3, 4, 5, 6, 7);
    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 5, 6);

    // Negative tests.
    status = GdipSetTextureTransform (NULL, transform);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetTextureTransform (brush, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipSetTextureTransform (brush, nonInvertibleMatrix);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(TextureBrush, GdipResetTextureTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 0, 0, 1, 0, 0);

    // Negative tests.
    status = GdipResetTextureTransform (NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(TextureBrush, GdipMultiplyTextureTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Null matrix.
    status = GdipMultiplyTextureTransform (brush, NULL, MatrixOrderAppend);
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 11, 16, 19, 28, 32, 46);

    // Invalid MatrixOrder - negative.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Invalid MatrixOrder - positive.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipMultiplyTextureTransform (brush, matrix, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 10, 13, 22, 29, 40, 52);

    // Negative tests.
    status = GdipMultiplyTextureTransform (NULL, matrix, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipMultiplyTextureTransform (brush, nonInvertibleMatrix, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (matrix);
    GdipDeleteMatrix (nonInvertibleMatrix);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(TextureBrush, GdipTranslateTextureTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 10, 12);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipTranslateTextureTransform (brush, 5, 6, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 1, 2, 3, 4, 28, 40);

    // Negative tests.
    status = GdipTranslateTextureTransform (NULL, 1, 1, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipTranslateTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipTranslateTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(TextureBrush, GdipScaleTextureTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1.5, 1.5, 3, 2.5, 4.5);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipScaleTextureTransform (brush, 0.5, 0.75, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 0.5, 1, 2.25, 3, 5, 6);

    // Negative tests.
    status = GdipScaleTextureTransform (NULL, 1, 1, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipScaleTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipScaleTextureTransform (brush, 1, 1, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}

TEST(TextureBrush, GdipRotateTextureTransform)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, -2, 1, -4, 3, -6, 5);

    // MatrixOrderPrepend.
    GdipSetTextureTransform (brush, originalTransform);

    status = GdipRotateTextureTransform (brush, 90, MatrixOrderPrepend);
    ASSERT_EQ (Ok, status);

    GdipGetTextureTransform (brush, transform);
    verifyMatrix (transform, 3, 4, -1, -2, 5, 6);

    // Negative tests.
    status = GdipRotateTextureTransform (NULL, 90, MatrixOrderAppend);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRotateTextureTransform (brush, 90, (MatrixOrder)(MatrixOrderPrepend - 1));
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRotateTextureTransform (brush, 90, (MatrixOrder)(MatrixOrderAppend + 1));
    ASSERT_EQ (InvalidParameter, status);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDisposeImage (image);
    GdipDeleteMatrix (originalTransform);
    GdipDeleteMatrix (transform);

    SHUTDOWN
}
