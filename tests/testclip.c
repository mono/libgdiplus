#ifdef USE_WINDOWS_GDIPLUS
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#include <Windows.h>
#include <GdiPlus.h>
#else
#include <GdiPlusFlat.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "testhelpers.h"

#define C(func) assert (func == Ok)

#ifdef USE_WINDOWS_GDIPLUS
using namespace Gdiplus;
using namespace DllExports;
#endif

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

	BitmapData bm;
	BitmapData other_bm;
	memset (&bm, 0, sizeof (bm));
	memset (&other_bm, 0, sizeof (other_bm));

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
	assert (is_infinite);

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
	assert (!is_infinite);

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
	C (GdipBitmapLockBits (bitmap, &recti, ImageLockModeRead, PixelFormat32bppARGB, &bm));
	C (GdipBitmapLockBits (other_bitmap, &recti, ImageLockModeRead, PixelFormat32bppARGB, &other_bm));

	assert (bm.Width == other_bm.Width);
	assert (bm.Height == other_bm.Height);
	{
		ARGB *p = (ARGB *) bm.Scan0;
		ARGB *other_p = (ARGB *) other_bm.Scan0;
		for (int i = 0; i < width * height; ++i)
			assert (*p++ == *other_p++);
	}

	C (GdipBitmapUnlockBits (bitmap, &bm));
	C (GdipBitmapUnlockBits (other_bitmap, &other_bm));

	C (GdipDeleteRegion (clip));
	C (GdipDeleteGraphics (graphics));
	C (GdipDisposeImage (bitmap));

	GdipFree (scan0);
}

static void
test_gdip_clip_transform()
{
	GpBitmap *bitmap = 0;
	GpGraphics *graphics;
	GpRegion *clip;
	GpPath *path;
	GpRectF bounds;
	static const int width = 100;
	static const int height = 100;
	BYTE *scan0 = (BYTE*) GdipAlloc (width * height * 4);
	BOOL is_infinite = 0;

	C (GdipCreateBitmapFromScan0 (100, 100, 100 * 4, PixelFormat32bppARGB, scan0, &bitmap));
	C (GdipGetImageGraphicsContext (bitmap, &graphics));

	// Check the clipping region is infinite
	C (GdipCreateRegion(&clip));
	C (GdipGetClip(graphics, clip));
	C (GdipIsInfiniteRegion(clip, graphics, &is_infinite));
	assert (is_infinite);

	// Transform the world
	C (GdipTranslateWorldTransform(graphics, 50.f, 0.f, MatrixOrderAppend));

	// Test setting clip as rectangle
	C (GdipSetClipRect(graphics, 10, 10, 60, 60, CombineModeReplace));
	C (GdipGetClipBounds(graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 60);

	// Test setting clip as path with rectangle
	C (GdipCreatePath(FillModeWinding, &path));
	C (GdipAddPathRectangle(path, 10, 10, 60, 60));
	C (GdipGetPathWorldBounds(path, &bounds, NULL, NULL));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 60);
	C (GdipSetClipPath(graphics, path, CombineModeReplace));
	C (GdipGetClipBounds(graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 60);

	C (GdipDeleteRegion (clip));
	C (GdipDeleteGraphics (graphics));
	C (GdipDisposeImage (bitmap));

	GdipFree (scan0);
}

static void
test_gdip_clip_path ()
{
	GpBitmap *bitmap = 0;
	GpGraphics *graphics;

	C (GdipCreateBitmapFromScan0 (612, 792, 0, PixelFormat32bppARGB, NULL, &bitmap));
	C (GdipBitmapSetResolution (bitmap, 72, 72));
	C (GdipGetImageGraphicsContext (bitmap, &graphics));

	C (GdipSetClipRect (graphics, 0, 0, 612, 792, CombineModeIntersect));

	GpRectF rc = { 100, 100, 50, 50 };
	GpPath *path;
	C (GdipCreatePath (FillModeAlternate, &path));
	{
		C (GdipAddPathRectangle (path, rc.X, rc.Y, rc.Width, rc.Height));

		// This rectangle should be overlapped by the black rectangle
		GpSolidFill *pathBrush;
		C (GdipCreateSolidFill (0xFF00FF00, &pathBrush));
		C (GdipFillPath (graphics, pathBrush, path));
		C (GdipDeleteBrush (pathBrush));

		// Test clipping by GdipSetClipPath (issue #552)
		C (GdipSetClipPath (graphics, path, CombineModeIntersect));

		// Also test clipping by GdipSetClipRegion (issue #547)
		GpRegion *rgn;
		C (GdipCreateRegionPath (path, &rgn));
		C (GdipSetClipRegion (graphics, rgn, CombineModeIntersect));
		C (GdipDeleteRegion (rgn));
	}
	C (GdipDeletePath (path));

	const ARGB expectedColor = 0xFF000000;
	GpSolidFill *brush;
	C (GdipCreateSolidFill (expectedColor, &brush));
	C (GdipFillRectangleI (graphics, brush, rc.X, rc.Y, rc.Width, rc.Height));
	C (GdipDeleteBrush (brush));

	//CLSID png_clsid = { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	//WCHAR *filePath = createWchar ("test_gdip_clip_path.png");
	//C (GdipSaveImageToFile (bitmap, filePath, &png_clsid, NULL));

	GpPoint points[] = {
		{ rc.X, rc.Y },
		{ rc.X + rc.Width / 2, rc.Y + rc.Height / 2 },
		{ rc.X + rc.Width - 1, rc.Y + rc.Height - 1 }
	};
	for (int i = 0; i < sizeof (points) / sizeof (points[0]); i += 2) {
		GpPoint *pt = &points[i];

		ARGB color;
		C (GdipBitmapGetPixel (bitmap, pt->X, pt->Y, &color));
		assertEqualInt (color, expectedColor);
	}

	C (GdipDeleteGraphics (graphics));
	C (GdipDisposeImage (bitmap));
}

int
main(int argc, char**argv)
{
	STARTUP;

	test_gdip_clip();
	test_gdip_clip_transform();
	test_gdip_clip_path ();

	SHUTDOWN;
	return 0;
}
