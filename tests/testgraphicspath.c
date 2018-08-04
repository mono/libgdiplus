#ifdef _WIN32
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
#include "testhelpers.h"
#if !defined(USE_WINDOWS_GDIPLUS)
#include "../config.h"
#endif

#define verifyPath(path, expectedFillMode) \
{ \
  GpStatus status; \
  FillMode fillMode; \
 \
  status = GdipGetPathFillMode (path, &fillMode); \
  assertEqualInt (status, Ok); \
  assertEqualInt (fillMode, expectedFillMode); \
}

// A debug helper that simply saves the path out to an image for viewing and comparison with GDI+.
ATTRIBUTE_USED static void dumpPath (GpPath *path)
{
  GpStatus status;
  GpImage *image;
  GpGraphics *graphics;
  GpSolidFill *brush;
  WCHAR *fileName = createWchar ("test-image.bmp");

  GdipCreateBitmapFromScan0 (256, 256, 0, PixelFormat32bppARGB, NULL, (GpBitmap **) &image);
  GdipGetImageGraphicsContext (image, &graphics);
  GdipCreateSolidFill (0xFF0000FF, &brush);

  GdipFillPath (graphics, brush, path);
  status = GdipSaveImageToFile (image, fileName, &bmpEncoderClsid, NULL);
  assertEqualInt (status, Ok);

  int systemResult = system ("test-image.bmp");
  (void) systemResult;

  GdipDisposeImage (image);
  GdipDeleteGraphics (graphics);
  GdipDeleteBrush ((GpBrush *) brush);
  freeWchar (fileName);
}

static void test_createPath ()
{
  GpStatus status;
  GpPath *path;

  // FillModeAlternate.
  status = GdipCreatePath (FillModeAlternate, &path);
  assertEqualInt (status, Ok);
  verifyPath (path, FillModeAlternate);

  GdipDeletePath (path);

  // FillModeWinding.
  status = GdipCreatePath (FillModeWinding, &path);
  assertEqualInt (status, Ok);
  verifyPath (path, FillModeWinding);

  GdipDeletePath (path);

  // Invalid FillMode.
  status = GdipCreatePath ((FillMode)(FillModeWinding + 1), &path);
  assertEqualInt (status, Ok);
  verifyPath (path, (FillMode)(FillModeWinding + 1));

  GdipDeletePath (path);

  // Negative tests.
  status = GdipCreatePath (FillModeWinding, NULL);
  assertEqualInt (status, InvalidParameter);
}

static void test_addPathString ()
{
  GpStatus status;
  GpPath *path;
  const WCHAR string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
  const WCHAR emptyString[] = {'\0'};
  GpFontFamily *family;
  GpStringFormat *format;
  RectF layoutRect = {10, 20, 236, 226};

  GdipGetGenericFontFamilySerif (&family);
  GdipCreateStringFormat (0, 0, &format);

  // Valid size.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, string, 5, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Smaller size.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, string, 3, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Minus one length - not empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, string, -1, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Minus one length - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, emptyString, -1, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero length - not empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, string, 0, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero length - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, emptyString, 0, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // No such style.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, string, 5, family, -1, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero emSize - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, emptyString, 0, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative emSize - non-empty string.
  RectF largeLayoutRect = {100, 200, 236, 226};
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, string, 5, family, 0, -72, &largeLayoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative emSize - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, emptyString, 0, family, 0, -72, &largeLayoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero layout rect width.
  GdipCreatePath (FillModeAlternate, &path);

  RectF zeroWidth = {0, 0, 0, 100};
  status = GdipAddPathString (path, string, 5, family, 0, 72, &zeroWidth, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero layout rect height.
  GdipCreatePath (FillModeAlternate, &path);

  RectF zeroHeight = {0, 0, 100, 0};
  status = GdipAddPathString (path, string, 5, family, 0, 72, &zeroHeight, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative layout rect width.
  GdipCreatePath (FillModeAlternate, &path);

  RectF negativeWidth = {0, 0, -100, 100};
  status = GdipAddPathString (path, string, 5, family, 0, 72, &negativeWidth, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative layout rect height.
  GdipCreatePath (FillModeAlternate, &path);

  RectF negativeHeight = {0, 0, 100, -100};
  status = GdipAddPathString (path, string, 5, family, -1, 72, &negativeHeight, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Null format.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathString (path, string, 5, family, 0, 72, &layoutRect, NULL);
  assertEqualInt (status, Ok);

  // Negative tests.
  status = GdipAddPathString (NULL, string, 5, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, NULL, 5, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, string, -2, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, string, 5, NULL, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, string, 5, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, GenericError);

  status = GdipAddPathString (NULL, string, 5, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, NULL, 5, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, string, -2, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, string, 5, NULL, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, string, 5, family, 0, 0, NULL, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathString (path, string, 5, family, 0, 72, NULL, format);
  assertEqualInt (status, InvalidParameter);

  GdipDeletePath (path);
  GdipDeleteFontFamily (family);
  GdipDeleteStringFormat (format);
}

static void test_addPathStringI ()
{
  GpStatus status;
  GpPath *path;
  const WCHAR string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
  const WCHAR emptyString[] = {'\0'};
  GpFontFamily *family;
  GpStringFormat *format;
  Rect layoutRect = {10, 20, 236, 226};

  GdipGetGenericFontFamilySerif (&family);
  GdipCreateStringFormat (0, 0, &format);

  // Valid size.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, string, 5, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Minus one length - not empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, string, -1, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  // Minus one length - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, emptyString, -1, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero length - not empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, string, 0, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero length - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, emptyString, 0, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // No such style.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, string, 5, family, -1, 72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero emSize - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, emptyString, 0, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative emSize - non-empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, string, 5, family, 0, -72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative emSize - empty string.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, emptyString, 0, family, 0, -72, &layoutRect, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero layout rect width.
  GdipCreatePath (FillModeAlternate, &path);

  Rect zeroWidth = {0, 0, 0, 100};
  status = GdipAddPathStringI (path, string, 5, family, 0, 72, &zeroWidth, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Zero layout rect height.
  GdipCreatePath (FillModeAlternate, &path);

  Rect zeroHeight = {0, 0, 100, 0};
  status = GdipAddPathStringI (path, string, 5, family, 0, 72, &zeroHeight, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative layout rect width.
  GdipCreatePath (FillModeAlternate, &path);

  Rect negativeWidth = {0, 0, -100, 100};
  status = GdipAddPathStringI (path, string, 5, family, 0, 72, &negativeWidth, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Negative layout rect height.
  GdipCreatePath (FillModeAlternate, &path);

  Rect negativeHeight = {0, 0, 100, -100};
  status = GdipAddPathStringI (path, string, 5, family, -1, 72, &negativeHeight, format);
  assertEqualInt (status, Ok);

  GdipDeletePath (path);

  // Null format.
  GdipCreatePath (FillModeAlternate, &path);

  status = GdipAddPathStringI (path, string, 5, family, 0, 72, &layoutRect, NULL);
  assertEqualInt (status, Ok);

  // Negative tests.
  status = GdipAddPathStringI (NULL, string, 5, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, NULL, 5, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, string, -2, family, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, string, 5, NULL, 0, 72, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, string, 5, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, GenericError);

  status = GdipAddPathStringI (NULL, string, 5, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, NULL, 5, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, string, -2, family, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, string, 5, NULL, 0, 0, &layoutRect, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, string, 5, family, 0, 0, NULL, format);
  assertEqualInt (status, InvalidParameter);

  status = GdipAddPathStringI (path, string, 5, family, 0, 72, NULL, format);
  assertEqualInt (status, InvalidParameter);

  GdipDeletePath (path);
  GdipDeleteFontFamily (family);
  GdipDeleteStringFormat (format);
}

int
main (int argc, char**argv)
{
  STARTUP;

  test_createPath ();
  test_addPathString ();
  test_addPathStringI ();

  SHUTDOWN;
  return 0;
}
