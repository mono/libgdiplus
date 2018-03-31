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

#define ok(expected, error, ...) assert(expected);
#define expect(expected, got) ok((got) == (expected), "Expected %d, got %d\n", (INT)(expected), (INT)(got))
#define expectf_(expected, got, precision) ok(fabs((expected) - (got)) <= (precision), "Expected %f, got %f\n", (expected), (got))
#define expectf(expected, got) expectf_((expected), (got), 0.001)
#define TODO if (0)

#define set_rect_empty(r) (r)->X = (r)->Y = (r)->Width = (r)->Height = 0

static void wine_test_string_functions(void)
{
    GpStatus status;
	GpBitmap *image;
    GpGraphics *graphics;
    GpFontFamily *family;
    GpFont *font;
    RectF rc, char_bounds, bounds;
    GpBrush *brush;
    ARGB color = 0xff000000;
    const WCHAR fontname[] = {'T','a','h','o','m','a',0};
    const WCHAR teststring[] = {'M','M',' ','M','\n','M',0};
    //const WCHAR teststring2[] = {'j',0};
    REAL char_width, char_height;
    INT codepointsfitted, linesfilled;
    GpStringFormat *format;
    CharacterRange ranges[3] = {{0, 1}, {1, 3}, {5, 1}};
    GpRegion *regions[4];
    BOOL region_isempty[4];
    int i;
    //PointF positions[8];
    GpMatrix *identity;

	status = GdipCreateBitmapFromScan0 (400, 400, 0, PixelFormat32bppRGB, NULL, &image);
    expect(Ok, status);

	status = GdipGetImageGraphicsContext (image, &graphics);
    expect(Ok, status);
    ok(graphics != NULL, "Expected graphics to be initialized\n");

    status = GdipCreateFontFamilyFromName(fontname, NULL, &family);
    expect(Ok, status);

    status = GdipCreateFont(family, 10.0, FontStyleRegular, UnitPixel, &font);
    expect(Ok, status);

    status = GdipCreateSolidFill(color, (GpSolidFill**)&brush);
    expect(Ok, status);

    status = GdipCreateStringFormat(0, 0, &format);
    expect(Ok, status);

    rc.X = 0;
    rc.Y = 0;
    rc.Width = 100.0;
    rc.Height = 100.0;

    status = GdipDrawString(NULL, teststring, 6, font, &rc, NULL, brush);
    expect(InvalidParameter, status);

    status = GdipDrawString(graphics, NULL, 6, font, &rc, NULL, brush);
    expect(InvalidParameter, status);

    status = GdipDrawString(graphics, teststring, 6, NULL, &rc, NULL, brush);
    expect(InvalidParameter, status);

    status = GdipDrawString(graphics, teststring, 6, font, NULL, NULL, brush);
    expect(InvalidParameter, status);

    status = GdipDrawString(graphics, teststring, 6, font, &rc, NULL, NULL);
    expect(InvalidParameter, status);

    status = GdipDrawString(graphics, teststring, 6, font, &rc, NULL, brush);
    expect(Ok, status);

    status = GdipMeasureString(NULL, teststring, 6, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(InvalidParameter, status);

    status = GdipMeasureString(graphics, NULL, 6, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(InvalidParameter, status);

    status = GdipMeasureString(graphics, teststring, 6, NULL, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(InvalidParameter, status);

    status = GdipMeasureString(graphics, teststring, 6, font, NULL, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(InvalidParameter, status);

    status = GdipMeasureString(graphics, teststring, 6, font, &rc, NULL, NULL, &codepointsfitted, &linesfilled);
    expect(InvalidParameter, status);

    status = GdipMeasureString(graphics, teststring, 6, font, &rc, NULL, &bounds, NULL, &linesfilled);
    expect(Ok, status);

    status = GdipMeasureString(graphics, teststring, 6, font, &rc, NULL, &bounds, &codepointsfitted, NULL);
    expect(Ok, status);

    status = GdipMeasureString(graphics, teststring, 1, font, &rc, NULL, &char_bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, char_bounds.X);
    expectf(0.0, char_bounds.Y);
    ok(char_bounds.Width > 0, "got %0.2f\n", bounds.Width);
    ok(char_bounds.Height > 0, "got %0.2f\n", bounds.Height);
    expect(1, codepointsfitted);
    expect(1, linesfilled);

    status = GdipMeasureString(graphics, teststring, 2, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    ok(bounds.Width > char_bounds.Width, "got %0.2f, expected at least %0.2f\n", bounds.Width, char_bounds.Width);
    expectf(char_bounds.Height, bounds.Height);
    expect(2, codepointsfitted);
    expect(1, linesfilled);
    char_width = bounds.Width - char_bounds.Width;

    status = GdipMeasureString(graphics, teststring, 6, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    ok(bounds.Width > char_bounds.Width + char_width * 2, "got %0.2f, expected at least %0.2f\n",
       bounds.Width, char_bounds.Width + char_width * 2);
    ok(bounds.Height > char_bounds.Height, "got %0.2f, expected at least %0.2f\n", bounds.Height, char_bounds.Height);
    expect(6, codepointsfitted);
    expect(2, linesfilled);
    char_height = bounds.Height - char_bounds.Height;

    /* Measure the first line. */
    status = GdipMeasureString(graphics, teststring, 4, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expect(4, codepointsfitted);
    expect(1, linesfilled);

    /* Give just enough space to fit the first line. */
    rc.Width = bounds.Width;
    status = GdipMeasureString(graphics, teststring, 5, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expect(5, codepointsfitted);
    expect(1, linesfilled);

    /* Cut off everything after the first space. */
    rc.Width = char_bounds.Width + char_width * 2.1;

    status = GdipMeasureString(graphics, teststring, 6, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf_(char_bounds.Width + char_width, bounds.Width, 0.01);
    expectf_(char_bounds.Height + char_height * 2, bounds.Height, 0.01);
    expect(6, codepointsfitted);
    expect(3, linesfilled);

    /* Cut off everything including the first space. */
    rc.Width = char_bounds.Width + char_width * 1.7;

    status = GdipMeasureString(graphics, teststring, 6, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf_(char_bounds.Width + char_width, bounds.Width, 0.01);
    expectf_(char_bounds.Height + char_height * 2, bounds.Height, 0.01);
    expect(6, codepointsfitted);
    expect(3, linesfilled);

    /* Cut off everything after the first character. */
    rc.Width = char_bounds.Width + char_width * 0.8;

    status = GdipMeasureString(graphics, teststring, 6, font, &rc, NULL, &bounds, &codepointsfitted, &linesfilled);
    expect(Ok, status);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf_(char_bounds.Width, bounds.Width, 0.01);
    expectf_(char_bounds.Height + char_height * 3, bounds.Height, 0.05);
    expect(6, codepointsfitted);
    expect(4, linesfilled);

    for (i = 0; i < 4; i++)
        regions[i] = (GpRegion *)0xdeadbeef;

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, format, 0, regions);
    expect(Ok, status);

    for (i = 0; i < 4; i++)
        ok(regions[i] == (GpRegion *)0xdeadbeef, "expected 0xdeadbeef, got %p\n", regions[i]);

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, format, 3, regions);
    expect(Ok, status);

    for (i = 0; i < 4; i++)
        ok(regions[i] == (GpRegion *)0xdeadbeef, "expected 0xdeadbeef, got %p\n", regions[i]);

    status = GdipSetStringFormatMeasurableCharacterRanges(format, 3, ranges);
    expect(Ok, status);

    rc.X = rc.Y = rc.Width = rc.Height = 0;

    for (i=0; i<4; i++)
    {
        status = GdipCreateRegion(&regions[i]);
        expect(Ok, status);
        status = GdipSetEmpty(regions[i]);
        expect(Ok, status);
    }

    status = GdipMeasureCharacterRanges(NULL, teststring, 6, font, &rc, format, 3, regions);
    expect(InvalidParameter, status);

    status = GdipMeasureCharacterRanges(graphics, NULL, 6, font, &rc, format, 3, regions);
    expect(InvalidParameter, status);

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, NULL, &rc, format, 3, regions);
    expect(InvalidParameter, status);

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, NULL, format, 3, regions);
    expect(InvalidParameter, status);

    if (0)
    {
        /* Crashes on Windows XP */
        status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, NULL, 3, regions);
        expect(InvalidParameter, status);
    }

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, format, 3, NULL);
    expect(InvalidParameter, status);

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, format, 2, regions);
    expect(InvalidParameter, status);

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, format, 3, regions);
    expect(Ok, status);

    for (i = 0; i < 4; i++)
    {
        status = GdipIsEmptyRegion(regions[i], graphics, &region_isempty[i]);
        expect(Ok, status);
    }

    TODO ok(region_isempty[0], "region should be empty\n");
    TODO ok(region_isempty[1], "region should be empty\n");
    TODO ok(region_isempty[2], "region should be empty\n");
    TODO ok(region_isempty[3], "region should be empty\n");

    rc.Width = 100.0;
    rc.Height = 100.0;

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, format, 4, regions);
    TODO expect(Ok, status);

    for (i=0; i<4; i++)
    {
        status = GdipIsEmptyRegion(regions[i], graphics, &region_isempty[i]);
        TODO expect(Ok, status);
    }

    ok(!region_isempty[0], "region shouldn't be empty\n");
    ok(!region_isempty[1], "region shouldn't be empty\n");
    ok(!region_isempty[2], "region shouldn't be empty\n");
    ok(region_isempty[3], "region should be empty\n");

    /* Cut off everything after the first space, and the second line. */
    rc.Width = char_bounds.Width + char_width * 2.1;
    rc.Height = char_bounds.Height + char_height * 0.5;

    status = GdipMeasureCharacterRanges(graphics, teststring, 6, font, &rc, format, 3, regions);
    expect(Ok, status);

    for (i=0; i<4; i++)
    {
        status = GdipIsEmptyRegion(regions[i], graphics, &region_isempty[i]);
        expect(Ok, status);
    }

    ok(!region_isempty[0], "region shouldn't be empty\n");
    ok(!region_isempty[1], "region shouldn't be empty\n");
    ok(region_isempty[2], "region should be empty\n");
    ok(region_isempty[3], "region should be empty\n");

    for (i=0; i<4; i++)
        GdipDeleteRegion(regions[i]);

    status = GdipCreateMatrix(&identity);
    expect(Ok, status);

#if 0
    rc.X = 0;
    rc.Y = 0;
    rc.Width = 0;
    rc.Height = 0;
    memset(positions, 0, sizeof(positions));
    status = GdipMeasureDriverString(NULL, teststring, 6, font, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, &rc);
    expect(InvalidParameter, status);

    status = GdipMeasureDriverString(graphics, NULL, 6, font, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, &rc);
    expect(InvalidParameter, status);

    status = GdipMeasureDriverString(graphics, teststring, 6, NULL, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, &rc);
    expect(InvalidParameter, status);

    status = GdipMeasureDriverString(graphics, teststring, 6, font, NULL,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, &rc);
    expect(InvalidParameter, status);

    status = GdipMeasureDriverString(graphics, teststring, 6, font, positions,
        0x100, identity, &rc);
    expect(Ok, status);

    status = GdipMeasureDriverString(graphics, teststring, 6, font, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        NULL, &rc);
    expect(Ok, status);

    status = GdipMeasureDriverString(graphics, teststring, 6, font, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, NULL);
    expect(InvalidParameter, status);

    rc.X = 0;
    rc.Y = 0;
    rc.Width = 0;
    rc.Height = 0;
    status = GdipMeasureDriverString(graphics, teststring, 6, font, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, &rc);
    expect(Ok, status);

    expectf(0.0, rc.X);
    ok(rc.Y < 0.0, "unexpected Y %0.2f\n", rc.Y);
    ok(rc.Width > 0.0, "unexpected Width %0.2f\n", rc.Width);
    ok(rc.Height > 0.0, "unexpected Y %0.2f\n", rc.Y);

    char_width = rc.Width;
    char_height = rc.Height;

    rc.X = 0;
    rc.Y = 0;
    rc.Width = 0;
    rc.Height = 0;
    status = GdipMeasureDriverString(graphics, teststring, 4, font, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, &rc);
    expect(Ok, status);

    expectf(0.0, rc.X);
    ok(rc.Y < 0.0, "unexpected Y %0.2f\n", rc.Y);
    ok(rc.Width < char_width, "got Width %0.2f, expecting less than %0.2f\n", rc.Width, char_width);
    expectf(char_height, rc.Height);

    rc.X = 0;
    rc.Y = 0;
    rc.Width = 0;
    rc.Height = 0;
    status = GdipMeasureDriverString(graphics, teststring2, 1, font, positions,
        DriverStringOptionsCmapLookup|DriverStringOptionsRealizedAdvance,
        identity, &rc);
    expect(Ok, status);

    expectf(rc.X, 0.0);
    ok(rc.Y < 0.0, "unexpected Y %0.2f\n", rc.Y);
    ok(rc.Width > 0, "unexpected Width %0.2f\n", rc.Width);
    expectf(rc.Height, char_height);
#endif

    GdipDeleteMatrix(identity);
    GdipDeleteStringFormat(format);
    GdipDeleteBrush(brush);
    GdipDeleteFont(font);
    GdipDeleteFontFamily(family);
    GdipDeleteGraphics(graphics);
	GdipDisposeImage(image);
}

static const REAL mm_per_inch = 25.4;
static const REAL point_per_inch = 72.0;

static GpGraphics *create_graphics(REAL res_x, REAL res_y, GpUnit unit, REAL scale, GpImage **image)
{
    GpStatus status;
    union
    {
        GpBitmap *bitmap;
        GpImage *image;
    } u;
    GpGraphics *graphics = NULL;
    REAL res;

    status = GdipCreateBitmapFromScan0(1, 1, 4, PixelFormat24bppRGB, NULL, &u.bitmap);
    expect(Ok, status);

    status = GdipBitmapSetResolution(u.bitmap, res_x, res_y);
    expect(Ok, status);
    status = GdipGetImageHorizontalResolution(u.image, &res);
    expect(Ok, status);
    expectf(res_x, res);
    status = GdipGetImageVerticalResolution(u.image, &res);
    expect(Ok, status);
    expectf(res_y, res);

    status = GdipGetImageGraphicsContext(u.image, &graphics);
    expect(Ok, status);

    *image = u.image;

    status = GdipGetDpiX(graphics, &res);
    expect(Ok, status);
    expectf(res_x, res);
    status = GdipGetDpiY(graphics, &res);
    expect(Ok, status);
    expectf(res_y, res);

    status = GdipSetPageUnit(graphics, unit);
    expect(Ok, status);
    status = GdipSetPageScale(graphics, scale);
    expect(Ok, status);

    return graphics;
}

static REAL units_to_pixels(REAL units, GpUnit unit, REAL dpi)
{
    switch (unit)
    {
    case UnitPixel:
    case UnitDisplay:
        return units;
    case UnitPoint:
        return units * dpi / point_per_inch;
    case UnitInch:
        return units * dpi;
    case UnitDocument:
        return units * dpi / 300.0; /* Per MSDN */
    case UnitMillimeter:
        return units * dpi / mm_per_inch;
    default:
        ok(0, "Unsupported unit: %d\n", unit);
        return 0;
    }
}

static REAL pixels_to_units(REAL pixels, GpUnit unit, REAL dpi)
{
    switch (unit)
    {
    case UnitPixel:
    case UnitDisplay:
        return pixels;
    case UnitPoint:
        return pixels * point_per_inch / dpi;
    case UnitInch:
        return pixels / dpi;
    case UnitDocument:
        return pixels * 300.0 / dpi;
    case UnitMillimeter:
        return pixels * mm_per_inch / dpi;
    default:
        ok(0, "Unsupported unit: %d\n", unit);
        return 0;
    }
}

static REAL units_scale(GpUnit from, GpUnit to, REAL dpi)
{
    REAL pixels = units_to_pixels(1.0, from, dpi);
    return pixels_to_units(pixels, to, dpi);
}

static void wine_test_transform(void)
{
    static const struct test_data
    {
        REAL res_x, res_y, scale;
        GpUnit unit;
        GpPointF in[2], out[2];
    } td[] =
    {
        { 96.0, 96.0, 1.0, UnitPixel, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 100.0, 0.0 }, { 0.0, 100.0 } } },
        { 96.0, 96.0, 1.0, UnitDisplay, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 100.0, 0.0 }, { 0.0, 100.0 } } },
        { 96.0, 96.0, 1.0, UnitInch, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 9600.0, 0.0 }, { 0.0, 9600.0 } } },
        { 123.0, 456.0, 1.0, UnitPoint, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 170.833313, 0.0 }, { 0.0, 633.333252 } } },
        { 123.0, 456.0, 1.0, UnitDocument, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 40.999996, 0.0 }, { 0.0, 151.999985 } } },
        { 123.0, 456.0, 2.0, UnitMillimeter, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 968.503845, 0.0 }, { 0.0, 3590.550781 } } },
        { 196.0, 296.0, 1.0, UnitDisplay, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 100.0, 0.0 }, { 0.0, 100.0 } } },
        { 196.0, 296.0, 1.0, UnitPixel, { { 100.0, 0.0 }, { 0.0, 100.0 } }, { { 100.0, 0.0 }, { 0.0, 100.0 } } },
    };
    GpStatus status;
    GpGraphics *graphics;
    GpImage *image;
    GpPointF ptf[2];
    UINT i;

    for (i = 0; i < sizeof(td)/sizeof(td[0]); i++)
    {
        graphics = create_graphics(td[i].res_x, td[i].res_y, td[i].unit, td[i].scale, &image);
        ptf[0].X = td[i].in[0].X;
        ptf[0].Y = td[i].in[0].Y;
        ptf[1].X = td[i].in[1].X;
        ptf[1].Y = td[i].in[1].Y;
        status = GdipTransformPoints(graphics, CoordinateSpaceDevice, CoordinateSpaceWorld, ptf, 2);
        expect(Ok, status);
        expectf(td[i].out[0].X, ptf[0].X);
        expectf(td[i].out[0].Y, ptf[0].Y);
        expectf(td[i].out[1].X, ptf[1].X);
        expectf(td[i].out[1].Y, ptf[1].Y);
        status = GdipTransformPoints(graphics, CoordinateSpaceWorld, CoordinateSpaceDevice, ptf, 2);
        expect(Ok, status);
        expectf(td[i].in[0].X, ptf[0].X);
        expectf(td[i].in[0].Y, ptf[0].Y);
        expectf(td[i].in[1].X, ptf[1].X);
        expectf(td[i].in[1].Y, ptf[1].Y);
        status = GdipDeleteGraphics(graphics);
        expect(Ok, status);
        status = GdipDisposeImage(image);
        expect(Ok, status);
    }
}

static void wine_test_GdipMeasureString(void)
{
    static const struct test_data
    {
        REAL res_x, res_y, page_scale;
        GpUnit unit;
    } td[] =
    {
        { 200.0, 200.0, 1.0, UnitPixel }, /* base */
        { 200.0, 200.0, 2.0, UnitPixel },
        { 200.0, 200.0, 1.0, UnitDisplay },
        { 200.0, 200.0, 2.0, UnitDisplay },
        { 200.0, 200.0, 1.0, UnitInch },
        { 200.0, 200.0, 2.0, UnitInch },
        { 200.0, 600.0, 1.0, UnitPoint },
        { 200.0, 600.0, 2.0, UnitPoint },
        { 200.0, 600.0, 1.0, UnitDocument },
        { 200.0, 600.0, 2.0, UnitDocument },
        { 200.0, 600.0, 1.0, UnitMillimeter },
        { 200.0, 600.0, 2.0, UnitMillimeter },
        { 200.0, 600.0, 1.0, UnitDisplay },
        { 200.0, 600.0, 2.0, UnitDisplay },
        { 200.0, 600.0, 1.0, UnitPixel },
        { 200.0, 600.0, 2.0, UnitPixel },
    };
    static const WCHAR tahomaW[] = { 'T','a','h','o','m','a',0 };
    static const WCHAR string[] = { '1','2','3','4','5','6','7',0 };
    GpStatus status;
    GpGraphics *graphics;
    GpFontFamily *family;
    GpFont *font;
    GpStringFormat *format;
    RectF bounds, rc;
    REAL base_cx = 0, base_cy = 0, height;
    INT chars, lines;
    LOGFONTW lf;
    UINT i;
    REAL font_size;
    GpUnit font_unit, unit;

    status = GdipCreateStringFormat(0, 0, &format);
    expect(Ok, status);
    status = GdipCreateFontFamilyFromName(tahomaW, NULL, &family);
    expect(Ok, status);

    /* font size in pixels */
    status = GdipCreateFont(family, 100.0, FontStyleRegular, UnitPixel, &font);
    expect(Ok, status);
    status = GdipGetFontSize(font, &font_size);
    expect(Ok, status);
    expectf(100.0, font_size);
    status = GdipGetFontUnit(font, &font_unit);
    expect(Ok, status);
    expect(UnitPixel, font_unit);

    for (i = 0; i < sizeof(td)/sizeof(td[0]); i++)
    {
        GpImage *image;

        graphics = create_graphics(td[i].res_x, td[i].res_y, td[i].unit, td[i].page_scale, &image);

        lf.lfHeight = 0xdeadbeef;
        status = GdipGetLogFontW(font, graphics, &lf);
        expect(Ok, status);
        height = units_to_pixels(font_size, td[i].unit, td[i].res_y);
        if (td[i].unit != UnitDisplay)
            height *= td[i].page_scale;
        TODO ok(-lf.lfHeight == (LONG)(height + 0.5), "%u: expected %d (%f), got %d\n",
           i, (LONG)(height + 0.5), height, lf.lfHeight);

        height = font_size + 2.0 * font_size / 6.0;

        set_rect_empty(&rc);
        set_rect_empty(&bounds);
        status = GdipMeasureString(graphics, string, -1, font, &rc, format, &bounds, &chars, &lines);
        expect(Ok, status);

        if (i == 0)
        {
            base_cx = bounds.Width;
            base_cy = bounds.Height;
        }

        expectf(0.0, bounds.X);
        expectf(0.0, bounds.Y);
        TODO expectf_(height, bounds.Height, height / 100.0);
        expectf_(bounds.Height / base_cy, bounds.Width / base_cx, 0.1);
        expect(7, chars);
        expect(1, lines);

        /* make sure it really fits */
        bounds.Width += 1.0;
        bounds.Height += 1.0;
        rc = bounds;
        rc.X = 50.0;
        rc.Y = 50.0;
        set_rect_empty(&bounds);
        status = GdipMeasureString(graphics, string, -1, font, &rc, format, &bounds, &chars, &lines);
        expect(Ok, status);
        TODO expectf(50.0, bounds.X);
        TODO expectf(50.0, bounds.Y);
        TODO expectf_(height, bounds.Height, height / 100.0);
        expectf_(bounds.Height / base_cy, bounds.Width / base_cx, 0.1);
        expect(7, chars);
        expect(1, lines);

        status = GdipDeleteGraphics(graphics);
        expect(Ok, status);

        status = GdipDisposeImage(image);
        expect(Ok, status);
    }

    GdipDeleteFont(font);

    /* font size in logical units */
    /* UnitPoint = 3, UnitInch = 4, UnitDocument = 5, UnitMillimeter = 6 */
    for (unit = 3; unit <= 6; unit++)
    {
        /* create a font which final height is 100.0 pixels with 200 dpi device */
        /* height + 2 * (height/6) = 100 => height = 100 * 3 / 4 => 75 */
        height = pixels_to_units(75.0, unit, 200.0);
        status = GdipCreateFont(family, height, FontStyleRegular, unit, &font);
        expect(Ok, status);
        status = GdipGetFontSize(font, &font_size);
        expect(Ok, status);
        expectf(height, font_size);
        status = GdipGetFontUnit(font, &font_unit);
        expect(Ok, status);
        expect(unit, font_unit);

        for (i = 0; i < sizeof(td)/sizeof(td[0]); i++)
        {
            REAL unit_scale;
            GpImage *image;

            graphics = create_graphics(td[i].res_x, td[i].res_y, td[i].unit, td[i].page_scale, &image);

            lf.lfHeight = 0xdeadbeef;
            status = GdipGetLogFontW(font, graphics, &lf);
            expect(Ok, status);
            if (td[i].unit == UnitDisplay || td[i].unit == UnitPixel)
                height = units_to_pixels(font_size, font_unit, td[i].res_x);
            else
                height = units_to_pixels(font_size, font_unit, td[i].res_y);
            /*trace("%.1f font units = %f pixels with %.1f dpi, page_scale %.1f\n", font_size, height, td[i].res_y, td[i].page_scale);*/
            TODO ok(-lf.lfHeight == (LONG)(height + 0.5), "%u: expected %d (%f), got %d\n",
               i, (LONG)(height + 0.5), height, lf.lfHeight);

            if (td[i].unit == UnitDisplay || td[i].unit == UnitPixel)
                unit_scale = units_scale(font_unit, td[i].unit, td[i].res_x);
            else
                unit_scale = units_scale(font_unit, td[i].unit, td[i].res_y);
            /*trace("%u: %d to %d, %.1f dpi => unit_scale %f\n", i, font_unit, td[i].unit, td[i].res_y, unit_scale);*/
            height = (font_size + 2.0 * font_size / 6.0) * unit_scale;
            if (td[i].unit != UnitDisplay)
                height /= td[i].page_scale;
            /*trace("%u: %.1f font units = %f units with %.1f dpi, page_scale %.1f\n", i, font_size, height, td[i].res_y, td[i].page_scale);*/

            set_rect_empty(&rc);
            set_rect_empty(&bounds);
            status = GdipMeasureString(graphics, string, -1, font, &rc, format, &bounds, &chars, &lines);
            expect(Ok, status);

            if (i == 0)
            {
                base_cx = bounds.Width;
                base_cy = bounds.Height;
            }

            expectf(0.0, bounds.X);
            TODO expectf(0.0, bounds.Y);
            TODO expectf_(height, bounds.Height, height / 85.0);
            expectf_(bounds.Height / base_cy, bounds.Width / base_cx, 0.1);
            expect(7, chars);
            expect(1, lines);

            /* make sure it really fits */
            bounds.Width += 1.0;
            bounds.Height += 1.0;
            rc = bounds;
            rc.X = 50.0;
            rc.Y = 50.0;
            set_rect_empty(&bounds);
            status = GdipMeasureString(graphics, string, -1, font, &rc, format, &bounds, &chars, &lines);
            expect(Ok, status);
            TODO expectf(50.0, bounds.X);
            TODO expectf(50.0, bounds.Y);
            TODO expectf_(height, bounds.Height, height / 85.0);
            expectf_(bounds.Height / base_cy, bounds.Width / base_cx, 0.1);
            expect(7, chars);
            expect(1, lines);

            /* verify the result */
            height = units_to_pixels(bounds.Height, td[i].unit, td[i].res_x);
            if (td[i].unit != UnitDisplay)
                height *= td[i].page_scale;
            /*trace("%u: unit %u, %.1fx%.1f dpi, scale %.1f, height %f, pixels %f\n",
                  i, td[i].unit, td[i].res_x, td[i].res_y, td[i].page_scale, bounds.Height, height);*/
            TODO expectf_(100.0, height, 1.1);

            status = GdipDeleteGraphics(graphics);
            expect(Ok, status);

            status = GdipDisposeImage(image);
            expect(Ok, status);
        }

        GdipDeleteFont(font);
    }

    /* Font with units = UnitWorld */
    for (i = 0; i < sizeof(td)/sizeof(td[0]); i++)
    {
        GpPointF pt = {0.0, 100.0};
        GpImage* image;
        REAL expected_width, expected_height;

        graphics = create_graphics(td[i].res_x, td[i].res_y, td[i].unit, td[i].page_scale, &image);

        status = GdipTransformPoints(graphics, CoordinateSpaceWorld, CoordinateSpaceDevice, &pt, 1);
        expect(Ok, status);

        status = GdipCreateFont(family, pt.Y, FontStyleRegular, UnitWorld, &font);
        expect(Ok, status);

        status = GdipGetFontUnit(font, &font_unit);
        expect(Ok, status);
        expect(UnitWorld, font_unit);

        lf.lfHeight = 0xdeadbeef;
        status = GdipGetLogFontW(font, graphics, &lf);
        expect(Ok, status);
        TODO ok(lf.lfHeight == -100, "%u: expected -100, got %d\n", i, lf.lfHeight);

        set_rect_empty(&rc);
        set_rect_empty(&bounds);
        status = GdipMeasureString(graphics, string, -1, font, &rc, format, &bounds, &chars, &lines);
        expect(Ok, status);

        if (i == 0)
        {
            base_cx = bounds.Width;
            base_cy = bounds.Height;
        }

        pt.X = 1.0;
        pt.Y = 1.0;

        status = GdipTransformPoints(graphics, CoordinateSpaceWorld, CoordinateSpaceDevice, &pt, 1);
        expect(Ok, status);

        /* height is constant in device space, width is proportional to height in world space */
        expected_width = base_cx * pt.Y;
        expected_height = base_cy * pt.Y;

        TODO ok(fabs(expected_width - bounds.Width) <= 0.001, "%u: expected %f, got %f\n", i, expected_width, bounds.Width);
        TODO ok(fabs(expected_height - bounds.Height) <= 0.001, "%u: expected %f, got %f\n", i, expected_height, bounds.Height);

        GdipDeleteGraphics(graphics);
        GdipDisposeImage(image);
        GdipDeleteFont(font);
    }

    GdipDeleteFontFamily(family);
    GdipDeleteStringFormat(format);
}

static void wine_test_measure_string(void)
{
    static const WCHAR tahomaW[] = { 'T','a','h','o','m','a',0 };
    static const WCHAR string[] = { 'A','0','1',0 };
    GpStringFormat *format;
    CharacterRange range;
    GpRegion *region;
	GpImage *image;
    GpGraphics *graphics;
    GpFontFamily *family;
    GpFont *font;
    GpStatus status;
    RectF bounds, rect;
    REAL width, height, width_1, width_2;
    REAL margin_x, margin_y, width_rgn, height_rgn;
    int lines, glyphs;

    status = GdipCreateStringFormat(StringFormatFlagsNoWrap, 0, &format);
    expect(Ok, status);
    expect(Ok, status);

    status = GdipCreateRegion(&region);
    expect(Ok, status);

    status = GdipCreateFontFamilyFromName(tahomaW, NULL, &family);
    expect(Ok, status);

	status = GdipCreateBitmapFromScan0 (400, 400, 0, PixelFormat32bppRGB, NULL, &image);
    expect(Ok, status);

	status = GdipGetImageGraphicsContext (image, &graphics);
    expect(Ok, status);
    ok(graphics != NULL, "Expected graphics to be initialized\n");

    status = GdipCreateFont(family, 20, FontStyleRegular, UnitPixel, &font);
    expect(Ok, status);

    margin_x = 20.0 / 6.0;
    margin_y = 20.0 / 8.0;

    set_rect_empty(&rect);
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    TODO expect(3, glyphs);
    expect(1, lines);
    TODO expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    width = bounds.Width;
    height = bounds.Height;

    set_rect_empty(&rect);
    rect.Height = height / 2.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    TODO expect(3, glyphs);
    expect(1, lines);
    TODO expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf(width, bounds.Width);
    TODO expectf(height / 2.0, bounds.Height);

    range.First = 0;
    range.Length = 3;
    status = GdipSetStringFormatMeasurableCharacterRanges(format, 1, &range);
    expect(Ok, status);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 32000.0;
    rect.Height = 32000.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    TODO expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    TODO expectf_(5.0 + margin_x, bounds.X, 1.0);
    TODO expectf(5.0, bounds.Y);
    TODO expectf_(width - margin_x*2.0, bounds.Width, 1.0);
    TODO expectf_(height - margin_y, bounds.Height, 1.0);

    width_rgn = bounds.Width;
    height_rgn = bounds.Height;

    range.First = 0;
    range.Length = 1;
    status = GdipSetStringFormatMeasurableCharacterRanges(format, 1, &range);
    expect(Ok, status);

    set_rect_empty(&rect);
    rect.Width = 32000.0;
    rect.Height = 32000.0;
    status = GdipMeasureCharacterRanges(graphics, string, 1, font, &rect, format, 1, &region);
    expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    TODO expectf_(margin_x, bounds.X, 1.0);
    expectf(0.0, bounds.Y);
    TODO ok(bounds.Width < width_rgn / 2.0, "width of 1 glyph is wrong\n");
    TODO expectf(height_rgn, bounds.Height);
    width_1 = bounds.Width;

    range.First = 0;
    range.Length = 3;
    status = GdipSetStringFormatMeasurableCharacterRanges(format, 1, &range);
    expect(Ok, status);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 0.0;
    rect.Height = 0.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    TODO expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    TODO expectf(0.0, bounds.X);
    TODO expectf(0.0, bounds.Y);
    TODO expectf(0.0, bounds.Width);
    TODO expectf(0.0, bounds.Height);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = width_rgn / 2.0;
    rect.Height = 32000.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    TODO expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    TODO expectf_(5.0 + margin_x, bounds.X, 1.0);
    TODO expectf(5.0, bounds.Y);
    TODO expectf_(width_1, bounds.Width, 1.0);
    TODO expectf_(height - margin_y, bounds.Height, 1.0);

    status = GdipSetStringFormatFlags(format, StringFormatFlagsNoWrap | StringFormatFlagsNoClip);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 0.0;
    rect.Height = 0.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    TODO expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    TODO expectf_(5.0 + margin_x, bounds.X, 1.0);
    TODO expectf(5.0, bounds.Y);
    TODO expectf(width_rgn, bounds.Width);
    TODO expectf(height_rgn, bounds.Height);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = width_rgn / 2.0;
    rect.Height = 32000.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    TODO expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    TODO expectf_(5.0 + margin_x, bounds.X, 1.0);
    TODO expectf(5.0, bounds.Y);
    TODO expectf_(width_1, bounds.Width, 1.0);
    TODO expectf(height_rgn, bounds.Height);

    set_rect_empty(&rect);
    rect.Height = height / 2.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(3, glyphs);
    expect(1, lines);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf_(width, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    set_rect_empty(&rect);
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, 1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(1, glyphs);
    expect(1, lines);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    ok(bounds.Width < width / 2.0, "width of 1 glyph is wrong\n");
    expectf(height, bounds.Height);
    width_1 = bounds.Width;

    set_rect_empty(&rect);
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, 2, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(2, glyphs);
    expect(1, lines);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    ok(bounds.Width < width, "width of 2 glyphs is wrong\n");
    ok(bounds.Width > width_1, "width of 2 glyphs is wrong\n");
    expectf(height, bounds.Height);
    width_2 = bounds.Width;

    set_rect_empty(&rect);
    rect.Width = width / 2.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(1, glyphs);
    expect(1, lines);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf_(width_1, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    set_rect_empty(&rect);
    rect.Height = height;
    rect.Width = width - 0.05;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(2, glyphs);
    expect(1, lines);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf_(width_2, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    set_rect_empty(&rect);
    rect.Height = height;
    rect.Width = width_2 - 0.05;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(1, glyphs);
    expect(1, lines);
    expectf(0.0, bounds.X);
    expectf(0.0, bounds.Y);
    expectf_(width_1, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    /* Default (Near) alignment */
    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = width * 2.0;
    rect.Height = height * 2.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(3, glyphs);
    expect(1, lines);
    expectf(5.0, bounds.X);
    expectf(5.0, bounds.Y);
    expectf_(width, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 32000.0;
    rect.Height = 32000.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    expectf_(5.0 + margin_x, bounds.X, 1.0);
    expectf(5.0, bounds.Y);
    expectf_(width - margin_x*2.0, bounds.Width, 1.0);
    expectf_(height - margin_y, bounds.Height, 1.0);

    width_rgn = bounds.Width;
    height_rgn = bounds.Height;

    /* Center alignment */
    GdipSetStringFormatAlign(format, StringAlignmentCenter);
    GdipSetStringFormatLineAlign(format, StringAlignmentCenter);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = width * 2.0;
    rect.Height = height * 2.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(3, glyphs);
    expect(1, lines);
    expectf_(5.0 + width/2.0, bounds.X, 0.01);
    expectf(5.0 + height/2.0, bounds.Y);
    expectf_(width, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 0.0;
    rect.Height = 0.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(3, glyphs);
    expect(1, lines);
    expectf_(5.0 - width/2.0, bounds.X, 0.01);
    expectf(5.0 - height/2.0, bounds.Y);
    expectf_(width, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = width_rgn * 2.0;
    rect.Height = height_rgn * 2.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    expectf_(5.0 + width_rgn/2.0, bounds.X, 1.0);
    expectf_(5.0 + height_rgn/2.0, bounds.Y, 1.0);
    expectf_(width_rgn, bounds.Width, 1.0);
    expectf_(height_rgn, bounds.Height, 1.0);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 0.0;
    rect.Height = 0.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    expectf_(5.0 - width_rgn/2.0, bounds.X, 1.0);
    expectf_(5.0 - height_rgn/2.0, bounds.Y, 1.0);
    expectf_(width_rgn, bounds.Width, 1.0);
    expectf_(height_rgn, bounds.Height, 1.0);

    /* Far alignment */
    GdipSetStringFormatAlign(format, StringAlignmentFar);
    GdipSetStringFormatLineAlign(format, StringAlignmentFar);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = width * 2.0;
    rect.Height = height * 2.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(3, glyphs);
    expect(1, lines);
    expectf_(5.0 + width, bounds.X, 0.01);
    expectf(5.0 + height, bounds.Y);
    expectf_(width, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 0.0;
    rect.Height = 0.0;
    set_rect_empty(&bounds);
    status = GdipMeasureString(graphics, string, -1, font, &rect, format, &bounds, &glyphs, &lines);
    expect(Ok, status);
    expect(3, glyphs);
    expect(1, lines);
    expectf_(5.0 - width, bounds.X, 0.01);
    expectf(5.0 - height, bounds.Y);
    expectf_(width, bounds.Width, 0.01);
    expectf(height, bounds.Height);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = width_rgn * 2.0;
    rect.Height = height_rgn * 2.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    expectf_(5.0 + width_rgn, bounds.X, 2.0);
    expectf_(5.0 + height_rgn, bounds.Y, 1.0);
    expectf_(width_rgn, bounds.Width, 1.0);
    expectf_(height_rgn, bounds.Height, 1.0);

    rect.X = 5.0;
    rect.Y = 5.0;
    rect.Width = 0.0;
    rect.Height = 0.0;
    status = GdipMeasureCharacterRanges(graphics, string, -1, font, &rect, format, 1, &region);
    expect(Ok, status);
    set_rect_empty(&bounds);
    status = GdipGetRegionBounds(region, graphics, &bounds);
    expect(Ok, status);
    expectf_(5.0 - width_rgn, bounds.X, 2.0);
    expectf_(5.0 - height_rgn, bounds.Y, 1.0);
    expectf_(width_rgn, bounds.Width, 1.0);
    expectf_(height_rgn, bounds.Height, 1.0);

    status = GdipDeleteFont(font);
    expect(Ok, status);

    status = GdipDeleteGraphics(graphics);
    expect(Ok, status);

    GdipDeleteFontFamily(family);
    GdipDeleteRegion(region);
    GdipDeleteStringFormat(format);
	GdipDisposeImage(image);
}

static void wine_test_measured_extra_space(void)
{
    static const WCHAR tahomaW[] = { 'T','a','h','o','m','a',0 };
    static const WCHAR string[2] = { 'W','W' };
    GpStringFormat *format;
	GpImage *image;
    GpGraphics *graphics;
    GpFontFamily *family;
    GpFont *font;
    GpStatus status;
    GpUnit gfx_unit, font_unit;
    RectF bounds_1, bounds_2, rect;
    REAL margin, font_size, dpi;

    status = GdipCreateStringFormat(0, 0, &format);
    expect(Ok, status);

    status = GdipCreateFontFamilyFromName(tahomaW, NULL, &family);
    expect(Ok, status);
    
	status = GdipCreateBitmapFromScan0 (400, 400, 0, PixelFormat32bppRGB, NULL, &image);
    expect(Ok, status);

	status = GdipGetImageGraphicsContext (image, &graphics);
    expect(Ok, status);
    ok(graphics != NULL, "Expected graphics to be initialized\n");

    status = GdipGetDpiX(graphics, &dpi);
    expect(Ok, status);

    /* UnitPixel = 2, UnitPoint = 3, UnitInch = 4, UnitDocument = 5, UnitMillimeter = 6 */
    /* UnitPixel as a font base unit is not tested because it differs in behaviour */
    for (font_unit = 3; font_unit <= 6; font_unit++)
    {
        status = GdipCreateFont(family, 1234.0, FontStyleRegular, font_unit, &font);
        expect(Ok, status);

        status = GdipGetFontSize(font, &font_size);
        expect(Ok, status);
        font_size = units_to_pixels(font_size, font_unit, dpi);
        /*trace("font size/6 = %f pixels\n", font_size / 6.0);*/

        /* UnitPixel = 2, UnitPoint = 3, UnitInch = 4, UnitDocument = 5, UnitMillimeter = 6 */
        for (gfx_unit = 2; gfx_unit <= 6; gfx_unit++)
        {
            status = GdipSetPageUnit(graphics, gfx_unit);
            expect(Ok, status);

            /* bounds.width of 1 glyph: [margin]+[width]+[margin] */
            set_rect_empty(&rect);
            set_rect_empty(&bounds_1);
            status = GdipMeasureString(graphics, string, 1, font, &rect, format, &bounds_1, NULL, NULL);
            expect(Ok, status);
            /* bounds.width of 2 identical glyphs: [margin]+[width]+[width]+[margin] */
            set_rect_empty(&rect);
            set_rect_empty(&bounds_2);
            status = GdipMeasureString(graphics, string, 2, font, &rect, format, &bounds_2, NULL, NULL);
            expect(Ok, status);

            /* margin = [bounds.width of 1] - [bounds.width of 2] / 2*/
            margin = units_to_pixels(bounds_1.Width - bounds_2.Width / 2.0, gfx_unit, dpi);
            /*trace("margin %f pixels\n", margin);*/
            TODO expectf_(font_size / 6.0, margin, font_size / 100.0);
        }

        GdipDeleteFont(font);
    }

    GdipDeleteGraphics(graphics);
    GdipDeleteFontFamily(family);
    GdipDeleteStringFormat(format);
	GdipDisposeImage(image);
}

int
main (int argc, char**argv)
{
	STARTUP;

	wine_test_string_functions ();
	wine_test_transform ();
	wine_test_GdipMeasureString ();
	TODO wine_test_measure_string ();
	wine_test_measured_extra_space ();

	SHUTDOWN;
	return 0;
}
