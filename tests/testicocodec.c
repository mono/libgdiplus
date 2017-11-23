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

#ifdef USE_WINDOWS_GDIPLUS
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"

#if !defined(_WIN32)
#include <unistd.h>
#endif

static const char *file = "temp_asset.ico";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'a', 's', 's', 'e', 't', '.', 'i', 'c', 'o', 0};

static GpImage *createFile (BYTE *buffer, int length, GpStatus expectedStatus)
{
  GpStatus status;
  GpImage *image;
  FILE *f = fopen (file, "w+");
  fwrite ((void *) buffer, sizeof(BYTE), length, f);
  fclose (f);
  
  status = GdipLoadImageFromFile (wFile, &image);
  assertEqualInt (status, expectedStatus);
  
  return image;
}

static void test_invalidHeader ()
{
  BYTE noCount[]   = {0, 0, 1, 0};
  BYTE zeroCount[] = {0, 0, 1, 0, 0, 0};

  createFile (noCount, sizeof(noCount), OutOfMemory);
  createFile (zeroCount, sizeof(zeroCount), OutOfMemory);
}

static void test_invalidEntry ()
{
  BYTE noEntries[] = {0, 0, 1, 0, 1, 0};

  createFile (noEntries, sizeof(noEntries), OutOfMemory);
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

  createFile (noBitmapHeader, sizeof(noBitmapHeader), OutOfMemory);
  createFile (invalidPalette, sizeof(invalidPalette), OutOfMemory);
  createFile (noRGBEntries, sizeof(noRGBEntries), OutOfMemory);
  createFile (fewRGBEntries, sizeof(fewRGBEntries), OutOfMemory);
  createFile (noXorData, sizeof(noXorData), OutOfMemory);
  createFile (noAndData, sizeof(noAndData), OutOfMemory);
  
  // FIXME: this returns Ok with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
  createFile (invalidData, sizeof(invalidData), OutOfMemory);
#endif
}

int
main (int argc, char**argv)
{
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
  
  test_invalidHeader ();
  test_invalidEntry ();
  test_invalidImage ();
  
#if !defined(_WIN32)
  unlink (file);
#else
  DeleteFileA (file);
#endif
  
  GdiplusShutdown (gdiplusToken);
  return 0;
}
