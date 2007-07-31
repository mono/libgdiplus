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

#define C(func) assert(func == Ok)

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif


struct bm 
{
	BitmapData data;
	char spoil1[128]; // Space eaten at by the real BitmapData ...
};


static void
test_gdip_clip()
{
	GpBitmap *bitmap = 0;
	GpBitmap *other_bitmap = 0;
	GpGraphics *graphics;
	GpRegion *clip;
	GpPath *path;
	GpPath *other_path;
	static const int width = 100;
	static const int height = 100;
	BYTE *scan0 = (BYTE*) GdipAlloc (width * height * 4);
	BOOL is_infinite = 0;
	GpRectF rect;
	GpRect recti;

	struct bm bm;
	struct bm other_bm;

	rect.X = 0.;
	rect.Y = 0.;
	rect.Width = width;
	rect.Height = height;
	recti.X = 0;
	recti.Y = 0;
	recti.Width = width;
	recti.Height = height;

	// Create bitmap

	C (GdipCreateBitmapFromScan0 (width, height, width * 4, PixelFormat32bppARGB, scan0, &bitmap));

	// Get graphics

	C (GdipGetImageGraphicsContext (bitmap, &graphics));

	// Check the clipping region is infinite

	C (GdipCreateRegion(&clip));

	C (GdipGetClip (graphics, clip));
	C (GdipIsInfiniteRegion(clip, graphics, &is_infinite));
	assert(is_infinite);

	// Create a path

	C (GdipCreatePath (FillModeAlternate, &path));
	C (GdipAddPathEllipse (path, 0, 0, 3, 2));
	C (GdipAddPathBezier (path, 30, 30, 70, 175, 210, 220, 20, 60));
	C (GdipAddPathRectangle (path, 200, 200, 60, 60));
	C (GdipAddPathArc (path, 0, 0, 150, 120, 30, 120));
	C (GdipAddPathEllipse (path, 50, 50, 50, 100));

	// Save the infinite clip
	C (GdipGetClip (graphics, clip));
	// Set the path as the clipping region
	C (GdipSetClipPath (graphics, path, CombineModeReplace));
	// Clear the clipped image (foreground)
	C (GdipGraphicsClear (graphics, 0x8000ff00));
	// Reset the clip
	C (GdipResetClip(graphics));

	// Clone the image for future use
	C (GdipCloneBitmapArea (0., 0., (float)width, (float)height, PixelFormat32bppARGB, bitmap, &other_bitmap));

	// Set the path as the clipping region
	C (GdipSetClipPath (graphics, path, CombineModeExclude));
	C (GdipGetClip (graphics, clip));
	C (GdipIsInfiniteRegion (clip, graphics, &is_infinite));
	assert(!is_infinite);

	// Clear the clipped image (background)
	C (GdipGraphicsClear (graphics, 0x80ff0000));

	// Now onto the second image

	// Get the graphics
	C (GdipGetImageGraphicsContext (other_bitmap, &graphics));
	// Get the infinite clip
	C (GdipGetClip (graphics, clip));
	// Dup the path
	C (GdipClonePath (path, &other_path));
	// Add the bounds to it
	C (GdipGetRegionBounds (clip, graphics, &rect));
	C (GdipAddPathRectangles (other_path, &rect, 1));
	// Reverse the path
	C (GdipReversePath (other_path));

	// Clip using the reversed path
	C (GdipSetClipPath (graphics, other_path, CombineModeReplace));
	// Clear the clipped image (background)
	C (GdipGraphicsClear (graphics, 0x80ff0000));

	// Compare the two images
	C (GdipBitmapLockBits (bitmap, &recti, ImageLockModeRead, PixelFormat32bppARGB, &bm.data));
	C (GdipBitmapLockBits (other_bitmap, &recti, ImageLockModeRead, PixelFormat32bppARGB, &other_bm.data));

	assert (bm.data.Width == other_bm.data.Width);
	assert (bm.data.Height == other_bm.data.Height);
	{
		int *p = (int*)bm.data.Scan0;
		int *other_p = (int*)other_bm.data.Scan0;
		int i;
		for (i = 0; i < width * height; ++i)
			assert(*p++ == *other_p++);
	}

	C (GdipBitmapUnlockBits (bitmap, &bm.data));
	C (GdipBitmapUnlockBits (other_bitmap, &other_bm.data));

	C (GdipDeleteRegion (clip));
	C (GdipDeleteGraphics (graphics));
	C (GdipDisposeImage (bitmap));

	GdipFree (scan0);
}

int
main(int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	test_gdip_clip();

	GdiplusShutdown(gdiplusToken);
	return 0;
}
