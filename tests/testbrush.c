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

static void test_getBrushType ()
{
    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;

    GdipCreateSolidFill (1, &brush);

    status = GdipGetBrushType (NULL, &brushType);
    assert (status == InvalidParameter);

    status = GdipGetBrushType ((GpBrush *) brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_clone ()
{
    GpStatus status;
    GpSolidFill *brush;
    GpBrush *clone;

    GdipCreateSolidFill (1, &brush);

    status = GdipCloneBrush (NULL, &clone);
    assert (status == InvalidParameter);

    status = GdipCloneBrush ((GpBrush *) brush, NULL);
    assert (status == InvalidParameter);
}

static void test_delete ()
{
    GpStatus status;
    GpSolidFill *brush;
    
    GdipCreateSolidFill (1, &brush);

    status = GdipDeleteBrush ((GpBrush *) brush);
    assert (status == Ok);

    status = GdipDeleteBrush (NULL);
    assert (status == InvalidParameter);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

    test_getBrushType ();
    test_clone ();
    test_delete ();

	GdiplusShutdown (gdiplusToken);
	return 0;
}
