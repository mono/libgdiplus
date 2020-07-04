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

static const char *file = "temp_asset.ico";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'a', 's', 's', 'e', 't', '.', 'i', 'c', 'o', 0};
static GpImage *image;

#define createFile(buffer, expectedStatus) \
{ \
	GpStatus status; \
	FILE *f = fopen (file, "wb+"); \
	assert (f); \
	fwrite ((void *) buffer, sizeof (BYTE), sizeof (buffer), f); \
	fclose (f); \
 \
	status = GdipLoadImageFromFile (wFile, &image); \
	ASSERT_EQ (expectedStatus, status); \
}

TEST(ICOCodec, InvalidHeader)
{
  STARTUP

  BYTE noCount[]   = {0, 0, 1, 0};
  BYTE zeroCount[] = {0, 0, 1, 0, 0, 0};

  createFile (noCount, OutOfMemory);
  createFile (zeroCount, OutOfMemory);

  deleteFile (file);
  SHUTDOWN
}

TEST(ICOCodec, InvalidEntry)
{
  STARTUP

  BYTE noEntries[] = {0, 0, 1, 0, 1, 0};

  createFile (noEntries, OutOfMemory);

  deleteFile (file);
  SHUTDOWN
}

TEST(ICOCodec, InvalidImage)
{
  STARTUP

  BYTE noBitmapHeader[] = {0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 0, 0, 0};
  BYTE invalidPalette[] = {0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4};
  BYTE noRGBEntries[]   = {0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BYTE fewRGBEntries[]  = {0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BYTE noXorData[]      = {0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  BYTE noAndData[]      = {0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4};

#if defined(USE_WINDOWS_GDIPLUS)
  BYTE invalidData[]    = {0, 0, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0, 0, 0, 0, 0};
#endif

  createFile (noBitmapHeader, OutOfMemory);
  createFile (invalidPalette, OutOfMemory);
  createFile (noRGBEntries, OutOfMemory);
  createFile (fewRGBEntries, OutOfMemory);
  createFile (noXorData, OutOfMemory);
  createFile (noAndData, OutOfMemory);

  // FIXME: this returns Ok with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
  createFile (invalidData, OutOfMemory);
#endif

deleteFile (file);
SHUTDOWN
}

TEST(ICOCodec, IetPixel)
{
  STARTUP

  WCHAR *unis;
  GpBitmap *bitmap;
  GpStatus status;
  PixelFormat pixel_format;
  ARGB color;

  unis = createWchar ("test3.ico");
  status = GdipCreateBitmapFromFile (unis, &bitmap);
  ASSERT_EQ (Ok, status);
  freeWchar (unis);

  status = GdipGetImagePixelFormat (bitmap, &pixel_format);
  ASSERT_EQ (Ok, status);
  ASSERT_EQ (PixelFormat32bppARGB, pixel_format);

  status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
  ASSERT_EQ (Ok, status);
  assertEqualARGB (0x879EE532, color);
  GdipDisposeImage (bitmap);

  deleteFile (file);
  SHUTDOWN
}
