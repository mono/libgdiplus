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

	C (GdipDeleteGraphics (graphics));

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
	C (GdipDeletePath (path));
	C (GdipDeletePath (other_path));
	C (GdipDisposeImage (bitmap));
	C (GdipDisposeImage (other_bitmap));

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
	C (GdipDeletePath (path));
	C (GdipDisposeImage (bitmap));

	GdipFree (scan0);
}

static void
test_gdip_clip_containers()
{
	GpBitmap *bitmap = 0;
	GpGraphics *graphics;
	GpRegion *clip;
	GpRectF bounds;
	static const int width = 100;
	static const int height = 100;
	BYTE *scan0 = (BYTE*) GdipAlloc (width * height * 4);
	BOOL is_infinite = 0;
	GraphicsContainer container1, container2;
	ARGB colour;

	C (GdipCreateBitmapFromScan0 (100, 100, 100 * 4, PixelFormat32bppARGB, scan0, &bitmap));
	C (GdipGetImageGraphicsContext (bitmap, &graphics));

	// Check the clipping region is infinite
	C (GdipCreateRegion (&clip));
	C (GdipGetClip (graphics, clip));
	C (GdipIsInfiniteRegion (clip, graphics, &is_infinite));
	assert (is_infinite);

	C (GdipGraphicsClear (graphics, 0xFFFF0000));

	// Test setting clip as rectangle
	C (GdipSetClipRect (graphics, 10, 10, 60, 60, CombineModeReplace));
	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 60);

	C (GdipBeginContainer2 (graphics, &container1));
	assert (container1);

	C (GdipGetClip (graphics, clip));
	C (GdipIsInfiniteRegion (clip, graphics, &is_infinite));
	assert (is_infinite);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 60);

	C (GdipGraphicsClear (graphics, 0xFF00FF00));

	C (GdipSetClipRect (graphics, 20, 0, 40, 80, CombineModeReplace));
	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 0);
	assert (bounds.Width == 40);
	assert (bounds.Height == 80);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 10);
	assert (bounds.Width == 40);
	assert (bounds.Height == 60);

	C (GdipGraphicsClear (graphics, 0xFF0000FF));

	C (GdipBeginContainer2 (graphics, &container2));
	assert (container2);

	C (GdipGetClip (graphics, clip));
	C (GdipIsInfiniteRegion (clip, graphics, &is_infinite));
	assert (is_infinite);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 10);
	assert (bounds.Width == 40);
	assert (bounds.Height == 60);

	C (GdipSetClipRect (graphics, -20, 20, 140, 20, CombineModeReplace));
	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == -20);
	assert (bounds.Y == 20);
	assert (bounds.Width == 140);
	assert (bounds.Height == 20);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 20);
	assert (bounds.Width == 40);
	assert (bounds.Height == 20);

	C (GdipGraphicsClear (graphics, 0xFF00FFFF));

	C (GdipEndContainer (graphics, container2));

	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 0);
	assert (bounds.Width == 40);
	assert (bounds.Height == 80);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 10);
	assert (bounds.Width == 40);
	assert (bounds.Height == 60);

	C (GdipEndContainer (graphics, container1));

	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 60);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 60);

	C (GdipBitmapGetPixel (bitmap, 30, 5, &colour)); // Drawn with no clipping
	assert (colour == 0xFFFF0000);

	C (GdipBitmapGetPixel (bitmap, 15, 15, &colour)); // Drawn inside container1, clipping set outside (10, 10, 60, 60)
	assert (colour == 0xFF00FF00);

	C (GdipBitmapGetPixel (bitmap, 30, 15, &colour)); // Drawn inside container1, clipping set inside (20, 10, 40, 60)
	assert (colour == 0xFF0000FF);

	C (GdipBitmapGetPixel (bitmap, 30, 30, &colour)); // Drawn inside container2, clipping set inside (20, 20, 40, 20)
	assert (colour == 0xFF00FFFF);

	C (GdipDeleteRegion (clip));
	C (GdipDeleteGraphics (graphics));
	C (GdipDisposeImage (bitmap));

	GdipFree (scan0);
}

static void
test_gdip_clip_visible()
{
	GpBitmap *bitmap = 0;
	GpGraphics *graphics;
	GpRegion *clip;
	GpRectF bounds;
	static const int width = 100;
	static const int height = 100;
	BYTE *scan0 = (BYTE*) GdipAlloc (width * height * 4);
	BOOL is_infinite = 0;
	GraphicsContainer container;
	BOOL contains;

	C (GdipCreateBitmapFromScan0 (100, 100, 100 * 4, PixelFormat32bppARGB, scan0, &bitmap));
	C (GdipGetImageGraphicsContext (bitmap, &graphics));

	// Check the clipping region is infinite
	C (GdipCreateRegion (&clip));
	C (GdipGetClip (graphics, clip));
	C (GdipIsInfiniteRegion (clip, graphics, &is_infinite));
	assert (is_infinite);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 0);
	assert (bounds.Y == 0);
	assert (bounds.Width == 100);
	assert (bounds.Height == 100);

	C (GdipIsVisiblePoint (graphics, -5, 30, &contains));
	assert (!contains);

	C (GdipIsVisiblePoint (graphics, 150, 30, &contains));
	assert (!contains);

	C (GdipIsVisibleRect (graphics, 100, 60, 150, 150, &contains));
	assert (!contains);

	C (GdipIsVisibleRect (graphics, -50, 5, 50, 100, &contains));
	assert (!contains);

	// Test setting clip as rectangle
	C (GdipSetClipRect (graphics, 10, 10, 60, 120, CombineModeReplace));
	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 120);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 90);

	C (GdipIsVisiblePoint (graphics, 15, 99, &contains));
	assert (contains);

	C (GdipIsVisiblePoint (graphics, 5, 30, &contains));
	assert (!contains);

	C (GdipIsVisiblePoint (graphics, 20, 101, &contains));
	assert (!contains);

	C (GdipIsVisibleRect (graphics, 60, 60, 150, 150, &contains));
	assert (contains);

	C (GdipIsVisibleRect (graphics, 5, 5, 10, 5, &contains));
	assert (!contains);

	C (GdipIsVisibleRect (graphics, 0, 101, 100, 30, &contains));
	assert (!contains);

	C (GdipBeginContainer2 (graphics, &container));
	assert (container);

	C (GdipGetClip (graphics, clip));
	C (GdipIsInfiniteRegion (clip, graphics, &is_infinite));
	assert (is_infinite);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 10);
	assert (bounds.Y == 10);
	assert (bounds.Width == 60);
	assert (bounds.Height == 90);

	C (GdipSetClipRect (graphics, 20, -20, 40, 140, CombineModeReplace));
	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == -20);
	assert (bounds.Width == 40);
	assert (bounds.Height == 140);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 10);
	assert (bounds.Width == 40);
	assert (bounds.Height == 90);

	C (GdipIsVisiblePoint (graphics, 30, 30, &contains));
	assert (contains);

	C (GdipIsVisiblePoint (graphics, 15, 30, &contains));
	assert (!contains);

	C (GdipIsVisibleRect (graphics, 10, 5, 20, 15, &contains));
	assert (contains);

	C (GdipIsVisibleRect (graphics, 30, 20, 20, 70, &contains));
	assert (contains);

	C (GdipIsVisibleRect (graphics, 60, 60, 150, 150, &contains));
	assert (!contains);

	C (GdipSetClipRect (graphics, 30, 20, 20, 20, CombineModeExclude));
	C (GdipGetClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == -20);
	assert (bounds.Width == 40);
	assert (bounds.Height == 140);

	C (GdipGetVisibleClipBounds (graphics, &bounds));
	assert (bounds.X == 20);
	assert (bounds.Y == 10);
	assert (bounds.Width == 40);
	assert (bounds.Height == 90);

	C (GdipIsVisiblePoint (graphics, 29, 30, &contains));
	assert (contains);

	C (GdipIsVisiblePoint (graphics, 31, 30, &contains));
	assert (!contains);

	C (GdipIsVisibleRect (graphics, 35, 25, 20, 10, &contains));
	assert (contains);

	C (GdipIsVisibleRect (graphics, 31, 21, 18, 18, &contains));
	assert (!contains);

	C (GdipEndContainer (graphics, container));

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
	test_gdip_clip_containers ();
	test_gdip_clip_visible ();
	test_gdip_clip_path ();

	SHUTDOWN;
	return 0;
}
