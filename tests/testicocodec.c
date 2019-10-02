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
#include "testhelpers.h"

static const char *file = "temp_asset.ico";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'a', 's', 's', 'e', 't', '.', 'i', 'c', 'o', 0};
GpImage *image;

#define createFile(buffer, expectedStatus) \
{ \
	GpStatus status; \
	FILE *f = fopen (file, "wb+"); \
	assert (f); \
	fwrite ((void *) buffer, sizeof (BYTE), sizeof (buffer), f); \
	fclose (f); \
 \
	status = GdipLoadImageFromFile (wFile, &image); \
	assertEqualInt (status, expectedStatus); \
}

static void test_invalidHeader ()
{
  BYTE noCount[]   = {0, 0, 1, 0};
  BYTE zeroCount[] = {0, 0, 1, 0, 0, 0};

  createFile (noCount, OutOfMemory);
  createFile (zeroCount, OutOfMemory);
}

static void test_invalidEntry ()
{
  BYTE noEntries[] = {0, 0, 1, 0, 1, 0};

  createFile (noEntries, OutOfMemory);
}

static void test_invalidImage ()
{
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
}

static void test_getPixel ()
{
  WCHAR *unis;
  GpBitmap *bitmap;
  GpStatus status;
  PixelFormat pixel_format;
  ARGB color;

  unis = createWchar ("test3.ico");
  status = GdipCreateBitmapFromFile (unis, &bitmap);
  assertEqualInt (status, Ok);
  freeWchar (unis);
  status = GdipGetImagePixelFormat (bitmap, &pixel_format);
  assertEqualInt (status, Ok);
  assertEqualInt (pixel_format, PixelFormat32bppARGB);
  status = GdipBitmapGetPixel (bitmap, 0, 0, &color);
  assertEqualInt (status, Ok);
  assertEqualInt (color, 0x879EE532u);
  GdipDisposeImage (bitmap);
}

int
main (int argc, char**argv)
{
	STARTUP;

  test_invalidHeader ();
  test_invalidEntry ();
  test_invalidImage ();
  test_getPixel ();

  deleteFile (file);

  SHUTDOWN;
  return 0;
}
