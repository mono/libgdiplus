#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"
#include "../config.h"

#define ok(expected, ...) if (!(expected)) { printf(__VA_ARGS__); assert(expected); }
#define expect(expected, got) ok((got) == (expected), "Expected %d, got %d\n", (INT)(expected), (INT)(got))
#define expectf_(expected, got, precision) ok(fabs((expected) - (got)) <= (precision), "Expected %f, got %f\n", (expected), (got))
#define expectf(expected, got) expectf_((expected), (got), 0.001)
#define set_rect_empty(r) (r)->X = (r)->Y = (r)->Width = (r)->Height = 0

#ifdef USE_PANGO_RENDERING

static void test_measure_string(void)
{
	GpStringFormat *format;
	GpImage *image;
	GpGraphics *graphics;
	GpFontFamily *family;
	GpFont *font;
	GpStatus status;
	GpRectF rect, bounds, saved_bounds;
	const WCHAR teststring1[] = { 'M', '\n', '\n', 'M', 0 };
	int glyphs;
	int lines;

	status = GdipCreateStringFormat (0, 0, &format);
	expect (Ok, status);
	status = GdipGetGenericFontFamilySansSerif (&family);
	expect (Ok, status);
	status = GdipCreateFont (family, 10, FontStyleRegular, UnitPixel, &font);
	expect (Ok, status);
	status = GdipCreateBitmapFromScan0 (400, 400, 0, PixelFormat32bppRGB, NULL, &image);
	expect (Ok, status);
	status = GdipGetImageGraphicsContext (image, &graphics);
	expect (Ok, status);
	ok (graphics != NULL, "Expected graphics to be initialized\n");

	rect.X = 5.0;
	rect.Y = 5.0;
	rect.Width = 200.0;
	rect.Height = 200.0;
	set_rect_empty (&bounds);
	status = GdipMeasureString (graphics, teststring1, 1, font, &rect, format, &bounds, &glyphs, &lines);
	expect (Ok, status);
	expectf (5.0, bounds.X);
	expectf (5.0, bounds.Y);
	expect (1, glyphs);
	expect (1, lines);

	// New lines count as fitted code points
	set_rect_empty (&bounds);
	status = GdipMeasureString (graphics, teststring1, 2, font, &rect, format, &bounds, &glyphs, &lines);
	expect (Ok, status);
	expect (2, glyphs);
	expect (1, lines);
	saved_bounds = bounds;

	// Multiple new lines are counted as code points and lines
	set_rect_empty (&bounds);
	status = GdipMeasureString (graphics, teststring1, 3, font, &rect, format, &bounds, &glyphs, &lines);
	expect (Ok, status);
	expect (3, glyphs);
	expect (2, lines);

	// ...and lastly check the correct results for adding another character past the last new line
	set_rect_empty (&bounds);
	status = GdipMeasureString (graphics, teststring1, 4, font, &rect, format, &bounds, &glyphs, &lines);
	expect (Ok, status);
	expect (4, glyphs);
	expect (3, lines);

	// Attempt to fit 3 glyphs / 2 lines into a bounding box from 2 glyphs / 1 line
	rect = saved_bounds;
	set_rect_empty (&bounds);
	status = GdipMeasureString (graphics, teststring1, 3, font, &rect, format, &bounds, &glyphs, &lines);
	expect (Ok, status);
	expect (2, glyphs);
	expect (1, lines);

	GdipDeleteGraphics (graphics);
	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);
	GdipDeleteStringFormat (format);
	GdipDisposeImage (image);
}

static void test_measure_string_alignment(void)
{
	GpStringFormat *format;
	GpImage *image;
	GpGraphics *graphics;
	GpFontFamily *family;
	GpFont *font;
	GpStatus status;
	GpRectF rect, bounds;
	const WCHAR teststring1[] = { 'M', 0 };
	INT i;
	static const struct test_data
	{
		INT flags;
		StringAlignment alignment, line_alignment;
		REAL x_xx, x_x0;
		REAL y_yy, y_y0;
		REAL right_xx, right_x0;
		REAL bottom_yy, bottom_y0;
	} td[] =
	{
		{ 0, StringAlignmentNear, StringAlignmentNear, 0, 0, 0, 0, -1.0, 0, -1.0, 0 },
		{ 0, StringAlignmentCenter, StringAlignmentNear, -0.5, 100, 0, 0, 0.5, 100, -1.0, 0 },
		{ 0, StringAlignmentFar, StringAlignmentNear, -1.0, 200, 0, 0, 0, 200, -1.0, 0 },
		{ 0, StringAlignmentNear, StringAlignmentCenter, 0, 0, -0.5, 50, -1.0, 0, 0.5, 50 },
		{ 0, StringAlignmentCenter, StringAlignmentCenter, -0.5, 100, -0.5, 50, 0.5, 100, 0.5, 50 },
		{ 0, StringAlignmentFar, StringAlignmentCenter, -1.0, 200, -0.5, 50, 0, 200, 0.5, 50 },
		{ 0, StringAlignmentNear, StringAlignmentFar, 0, 0, -1.0, 100, -1.0, 0, 0, 100 },
		{ 0, StringAlignmentCenter, StringAlignmentFar, -0.5, 100, -1.0, 100, 0.5, 100, 0, 100 },
		{ 0, StringAlignmentFar, StringAlignmentFar, -1.0, 200, -1.0, 100, 0, 200, 0, 100 },

		{ StringFormatFlagsDirectionVertical, StringAlignmentNear, StringAlignmentNear, 0, 0, 0, 0, -1.0, 0, -1.0, 0 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentNear, StringAlignmentCenter, -0.5, 100, 0, 0, 0.5, 100, -1.0, 0 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentNear, StringAlignmentFar, -1.0, 200, 0, 0, 0, 200, -1.0, 0 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentCenter, StringAlignmentNear, 0, 0, -0.5, 50, 1.0, 0, 0.5, 50 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentCenter, StringAlignmentCenter, -0.5, 100, -0.5, 50, 0.5, 100, 0.5, 50 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentCenter, StringAlignmentFar, -1.0, 200, -0.5, 50, 0, 200, 0.5, 50 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentFar, StringAlignmentNear, 0, 0, -1.0, 100, -1.0, 0, 0, 100 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentFar, StringAlignmentCenter, -0.5, 100, -1.0, 100, 0.5, 100, 0, 100 },
		{ StringFormatFlagsDirectionVertical, StringAlignmentFar, StringAlignmentFar, -1.0, 200, -1.0, 100, 0, 200, 0, 100 },

		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentFar, StringAlignmentNear, 0, 0, 0, 0, -1.0, 0, -1.0, 0 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentCenter, StringAlignmentNear, -0.5, 100, 0, 0, 0.5, 100, -1.0, 0 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentNear, StringAlignmentNear, -1.0, 200, 0, 0, 0, 200, -1.0, 0 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentFar, StringAlignmentCenter, 0, 0, -0.5, 50, -1.0, 0, 0.5, 50 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentCenter, StringAlignmentCenter, -0.5, 100, -0.5, 50, 0.5, 100, 0.5, 50 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentNear, StringAlignmentCenter, -1.0, 200, -0.5, 50, 0, 200, 0.5, 50 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentFar, StringAlignmentFar, 0, 0, -1.0, 100, -1.0, 0, 0, 100 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentCenter, StringAlignmentFar, -0.5, 100, -1.0, 100, 0.5, 100, 0, 100 },
		{ StringFormatFlagsDirectionRightToLeft, StringAlignmentNear, StringAlignmentFar, -1.0, 200, -1.0, 100, 0, 200, 0, 100 }
	};


	status = GdipCreateStringFormat (0, 0, &format);
	expect (Ok, status);
	status = GdipGetGenericFontFamilySansSerif (&family);
	expect (Ok, status);
	status = GdipCreateFont (family, 10, FontStyleRegular, UnitPixel, &font);
	expect (Ok, status);
	status = GdipCreateBitmapFromScan0 (400, 400, 0, PixelFormat32bppRGB, NULL, &image);
	expect (Ok, status);
	status = GdipGetImageGraphicsContext (image, &graphics);
	expect (Ok, status);
	ok (graphics != NULL, "Expected graphics to be initialized\n");

	for (i = 0; i < sizeof(td) / sizeof(td[0]); i++) {
		GdipSetStringFormatFlags (format, td[i].flags);
		GdipSetStringFormatAlign (format, td[i].alignment);
		GdipSetStringFormatLineAlign (format, td[i].line_alignment);

		rect.X = 5.0;
		rect.Y = 10.0;
		rect.Width = 200.0;
		rect.Height = 100.0;
		set_rect_empty (&bounds);
		status = GdipMeasureString (graphics, teststring1, 1, font, &rect, format, &bounds, NULL, NULL);
		expect (Ok, status);
		expectf_ (td[i].x_x0 + td[i].x_xx * bounds.Width + 5.0, bounds.X, 0.6);
		expectf_ (td[i].y_y0 + td[i].y_yy * bounds.Height + 10.0, bounds.Y, 0.6);
		expectf_ (td[i].right_x0 + td[i].right_xx * bounds.Width + 5.0, bounds.X + bounds.Width, 0.6);
		expectf_ (td[i].bottom_y0 + td[i].bottom_yy * bounds.Height + 10.0, bounds.Y + bounds.Height, 0.6);
	}

	GdipDeleteGraphics (graphics);
	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);
	GdipDeleteStringFormat (format);
	GdipDisposeImage (image);
}

#endif

int
main (int argc, char**argv)
{
	STARTUP;

#ifdef USE_PANGO_RENDERING
	test_measure_string ();
	test_measure_string_alignment ();
#endif

	SHUTDOWN;
	return 0;
}
