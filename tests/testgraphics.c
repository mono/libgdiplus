#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#include <windows.h>
#include <GdiPlus.h>
#else
#include <GdiPlusFlat.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

static void test_flush()
{
	gunichar2 *filePath;
	GpStatus status;
	GpImage *image;
	GpGraphics *graphics;

	filePath = g_utf8_to_utf16("test.bmp", -1, NULL, NULL, NULL);
	status = GdipLoadImageFromFile(filePath, &image);
	assert(status == Ok);

	status = GdipGetImageGraphicsContext(image, &graphics);
	assert(status == Ok);

	status = GdipFlush(NULL, FlushIntentionFlush);
	assert(status == InvalidParameter);

	status = GdipFlush(graphics, FlushIntentionFlush);
	assert(status == Ok);

	status = GdipFlush(graphics, (FlushIntention)-1);
	assert(status == Ok);

// Libgdiplus does not yet match this GDI+ behaviour.
#if 0
	HDC hdc;
	status = GdipGetDC(graphics, &hdc);

	status = GdipFlush(graphics, FlushIntention::FlushIntentionSync);
	expect(ObjectBusy, status);

	GdipReleaseDC(graphics, hdc);
#endif

	GdipDisposeImage(image);
	GdipDeleteGraphics(graphics);
}

int
main(int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	test_flush();

	GdiplusShutdown(gdiplusToken);
	return 0;
}
