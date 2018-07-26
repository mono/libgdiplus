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
  GpImage *image;
  GpGraphics *graphics;
  GpStatus status;
  GpPath *path;
  const WCHAR string[] = {'H', 'e', 'l', 'l', 'o', '\0'};
  const WCHAR longString[] = {'H', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e', ',', ' ', 't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'l', 'o', 'n', 'g', ' ', 's', 't', 'r', 'i', 'n', 'g', '.', '\0'};
  const WCHAR emptyString[] = {'\0'};
  const int fontSize = 20;
  GpFontFamily *family;
  GpFont *font;
  GpStringFormat *format;
  RectF layoutRect = {10, 20, 236, 226};
  RectF longLayoutRect = {30, 40, 700, 100};
  RectF rect1, rect2;

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

  dumpPath (path);
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
  
  // Set up Graphics stuff to use MeasureString (copied from testtext.c)
  status = GdipCreateFont (family, fontSize, FontStyleRegular, UnitPixel, &font);
  assertEqualInt (status, Ok);
  status = GdipCreateBitmapFromScan0 (2000, 100, 0, PixelFormat32bppRGB, NULL, (GpBitmap **) &image);
  assertEqualInt (status, Ok);
  status = GdipGetImageGraphicsContext (image, &graphics);
  assertEqualInt (status, Ok);
  
  // Check the path bounds
  GdipSetStringFormatAlign (format, StringAlignmentFar);
  GdipSetStringFormatLineAlign (format, StringAlignmentFar);
  GdipCreatePath (FillModeAlternate, &path);
  status = GdipAddPathString (path, longString, -1, family, 0, fontSize, &longLayoutRect, format);
  assertEqualInt (status, Ok);
  status = GdipGetPathWorldBounds (path, &rect1, NULL, NULL);
  assertEqualInt (status, Ok);
  status = GdipMeasureString (graphics, longString, -1, font, &longLayoutRect, format, &rect2, NULL, NULL);
  assertEqualInt (status, Ok);
  assertSimilarFloat (rect1.X, rect2.X, 10.0);
  assertSimilarFloat (rect1.Y, rect2.Y, 10.0);
  assertSimilarFloat (rect1.X + rect1.Width, rect2.X + rect2.Width, 5.0);
  assertSimilarFloat (rect1.Y + rect1.Height, rect2.Y + rect2.Height, 5.0);
  GdipDeletePath (path);
  
  // Dispose the Graphics stuff
  GdipDeleteGraphics (graphics);
  GdipDeleteFont (font);
  GdipDisposeImage (image);  
  
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
