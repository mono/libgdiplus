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

#define verifyPath(path, expectedFillMode) \
{ \
  GpStatus status; \
  FillMode fillMode; \
 \
  status = GdipGetPathFillMode (path, &fillMode); \
  assertEqualInt (status, Ok); \
  assertEqualInt (fillMode, expectedFillMode); \
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

int
main (int argc, char**argv)
{
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

  test_createPath ();

  GdiplusShutdown (gdiplusToken);
  return 0;
}
