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

static void test_clone ()
{
    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;
    GpBrush *clone;

    ARGB solidColor;

    GdipCreateSolidFill (1, &brush);

    status = GdipCloneBrush ((GpBrush *) brush, &clone);
    assert (status == Ok);
    assert (clone && brush != clone);

    GdipGetBrushType (clone, &brushType);
    assert (brushType == BrushTypeSolidColor);

    GdipGetSolidFillColor ((GpSolidFill *) clone, &solidColor);
    assert (solidColor == 1);

    GdipDeleteBrush ((GpBrush *) brush);
    GdipDeleteBrush (clone);
}

static void test_createSolidFill ()
{
    GpStatus status;
    GpSolidFill *brush;
    GpBrushType brushType;

    status = GdipCreateSolidFill (1, &brush);
    assert (status == Ok);
    assert (brush && "Expected the brush to be initialized.");

    status = GdipGetBrushType ((GpBrush *) brush, &brushType);
    assert (status == Ok);
    assert (brushType == BrushTypeSolidColor);

    status = GdipCreateSolidFill (1, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_getSolidFillColor ()
{
    GpStatus status;
    GpSolidFill *brush;
    ARGB color;

    GdipCreateSolidFill (1, &brush);

    status = GdipGetSolidFillColor (brush, &color);
    assert (status == Ok);
    assert (color == 1);

    status = GdipGetSolidFillColor (NULL, &color);
    assert (status == InvalidParameter);

    status = GdipGetSolidFillColor (brush, NULL);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

static void test_setSolidFillColor ()
{
    GpStatus status;
    GpSolidFill *brush;
    ARGB color;

    GdipCreateSolidFill (1, &brush);

    status = GdipSetSolidFillColor (brush, 2);
    assert (status == Ok);

    GdipGetSolidFillColor (brush, &color);
    assert (color == 2);

    status = GdipSetSolidFillColor (NULL, 1);
    assert (status == InvalidParameter);

    GdipDeleteBrush ((GpBrush *) brush);
}

int
main (int argc, char**argv)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

    test_clone ();
    test_createSolidFill ();
    test_getSolidFillColor ();
    test_setSolidFillColor ();

    GdiplusShutdown (gdiplusToken);
    return 0;
}
