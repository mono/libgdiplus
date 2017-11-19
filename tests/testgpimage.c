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

static GpImage* getImage (const char* fileName) {
	GpStatus status;
	WCHAR *wFileName = wcharFromChar (fileName);
	GpImage *image;

	status = GdipLoadImageFromFile (wFileName, &image);
	assertEqualInt (status, Ok);

	freeWchar (wFileName);

	return image;
}

static void test_loadImageFromStream ()
{
	GpStatus status;
	GpImage *image;

	// Negative tests.
	status = GdipLoadImageFromStream (NULL, &image);
	assertEqualInt (status, InvalidParameter);

#if !defined(USE_WINDOWS_GDIPLUS)
	int temp = 0;

	status = GdipLoadImageFromStream (&temp, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipLoadImageFromStream (&temp, &image);
	assertEqualInt (status, NotImplemented);
#endif
}

static void test_loadImageFromFile ()
{
	GpStatus status;
	GpImage *image;
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	// Negative tests.
	status = GdipLoadImageFromFile (NULL, &image);
	assertEqualInt (status, InvalidParameter);

	status = GdipLoadImageFromFile (noSuchFile, &image);
	assertEqualInt (status, OutOfMemory);

	status = GdipLoadImageFromFile (invalidFile, &image);
	assertEqualInt (status, OutOfMemory);

	status = GdipLoadImageFromFile (noSuchFile, NULL);
	assertEqualInt (status, InvalidParameter);

	freeWchar (noSuchFile);
	freeWchar (invalidFile);
}

static void test_loadImageFromStreamICM ()
{
	GpStatus status;
	GpImage *image;

	// Negative tests.
	status = GdipLoadImageFromStreamICM (NULL, &image);
	assertEqualInt (status, InvalidParameter);

#if !defined(_WIN32)
	int temp = 0;

	status = GdipLoadImageFromStreamICM (&temp, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipLoadImageFromStreamICM (&temp, &image);
	assertEqualInt (status, NotImplemented);
#endif
}

static void test_loadImageFromFileICM ()
{
	GpStatus status;
	GpImage *image;
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	// Negative tests.
	status = GdipLoadImageFromFileICM (NULL, &image);
	assertEqualInt (status, InvalidParameter);

	status = GdipLoadImageFromFileICM (noSuchFile, &image);
	assertEqualInt (status, OutOfMemory);

	status = GdipLoadImageFromFileICM (invalidFile, &image);
	assertEqualInt (status, OutOfMemory);

	status = GdipLoadImageFromFileICM (noSuchFile, NULL);
	assertEqualInt (status, InvalidParameter);

	freeWchar (noSuchFile);
	freeWchar (invalidFile);
}

static void test_loadImageFromFileBmp ()
{
	GpImage *image = getImage ("test.bmp");

	// FIXME: bmp image flags are wrong in libgdiplus.
	verifyImage (image, ImageTypeBitmap, bmpRawFormat, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68, 77840, 0, FALSE);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileTif ()
{
	GpImage *image = getImage ("test.tif");

	verifyImage (image, ImageTypeBitmap, tifRawFormat, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68, 77840, 19, TRUE);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileGif ()
{
	GpImage *image = getImage ("test.gif");

	// FIXME: gif image flags are wrong in libgdiplus.
	verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 100, 68, 100, 68, 77840, 4, FALSE);

	GdipDisposeImage (image);
}

static void test_loadImageFromFilePng ()
{
	GpImage *image = getImage ("test.png");

	verifyImage (image, ImageTypeBitmap, pngRawFormat, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68, 77840, 5, TRUE);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileJpg ()
{
	GpImage *image = getImage ("test.jpg");

	// FIXME: jpg image flags are wrong in libgdiplus.
	verifyImage (image, ImageTypeBitmap, jpegRawFormat, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68, 73744, 2, FALSE);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileIcon ()
{
	GpImage *image = getImage ("test.ico");

	verifyImage (image, ImageTypeBitmap, icoRawFormat, PixelFormat32bppARGB, 0, 0, 48, 48, 48, 48, 73746, 0, TRUE);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileWmf ()
{
	GpImage *image = getImage ("test.wmf");

	verifyImage (image, ImageTypeMetafile, wmfRawFormat, PixelFormat32bppRGB, -4008, -3378, 8016, 6756, 20360.64f, 17160.24f, 327683, 0, TRUE);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileEmf ()
{
	GpImage *image = getImage ("test.emf");

	verifyImage (image, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 100, 100, 1944.44f, 1888.88f, 327683, 0, TRUE);

	GdipDisposeImage (image);
}

static void test_cloneImage ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpgImage = getImage ("test.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	GpImage *clonedImage;

	// ImageTypeBitmap - bmp.
	status = GdipCloneImage (bitmapImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != bitmapImage);
	// FIXME: bmp image flags are wrong in libgdiplus.
	verifyImage (clonedImage, ImageTypeBitmap, bmpRawFormat, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68, 77840, 0, FALSE);
	GdipDisposeImage (clonedImage);

	// ImageTypeBitmap - jpg.
	status = GdipCloneImage (jpgImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != jpgImage);
	// FIXME: jpg image flags are wrong in libgdiplus.
	verifyImage (clonedImage, ImageTypeBitmap, jpegRawFormat, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68, 73744, 2, FALSE);
	GdipDisposeImage (clonedImage);

	// ImageTypeMetafile.
	status = GdipCloneImage (metafileImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != metafileImage);
	verifyImage (clonedImage, ImageTypeMetafile, wmfRawFormat, PixelFormat32bppRGB, -4008, -3378, 8016, 6756, 20360.64f, 17160.24f, 327683, 0, TRUE);
	GdipDisposeImage (clonedImage);

	// Negative tests.
	status = GdipCloneImage (NULL, &clonedImage);
	assertEqualInt (status, InvalidParameter);

	status = GdipCloneImage (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpgImage);
	GdipDisposeImage (metafileImage);
}

static void test_disposeImage ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");

	// ImageTypeBitmap.
	status = GdipDisposeImage (bitmapImage);
	assertEqualInt (status, Ok);

	// ImageTypeMetafile.
	status = GdipDisposeImage (metafileImage);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDisposeImage (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_getImageGraphicsContext ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *gifImage = getImage ("test.gif");
	GpImage *metafileImage = getImage ("test.wmf");
	GpGraphics *graphics;

	// ImageTypeBitmap - PixelFormat24Bpp.
	status = GdipGetImageGraphicsContext (bitmapImage, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics);
	GdipDeleteGraphics (graphics);

	// FIXME: libgdiplus doesn't support PixelFormat8bppIndexed.
#if defined(USE_WINDOWS_GDIPLUS)
	// ImageTypeBitmap - PixelFormat8bppIndexed.
	status = GdipGetImageGraphicsContext (gifImage, &graphics);
	assertEqualInt (status, Ok);
	assert (graphics);
	GdipDeleteGraphics (graphics);
#endif

	// ImageTypeMetafile - not recording.
	status = GdipGetImageGraphicsContext (metafileImage, &graphics);
	assertEqualInt (status, OutOfMemory);

	// Negative tests.
	status = GdipGetImageGraphicsContext (NULL, &graphics);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageGraphicsContext (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (gifImage);
	GdipDisposeImage (metafileImage);
}

static void test_getImageBounds ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	GpRectF bounds;
	Unit unit;

	// Negative tests.
	status = GdipGetImageBounds (NULL, &bounds, &unit);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageBounds (image, NULL, &unit);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageBounds (image, &bounds, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageDimension ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	REAL dimensionWidth;
	REAL dimensionHeight;

	// Negative tests.
	status = GdipGetImageDimension (NULL, &dimensionWidth, &dimensionHeight);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageDimension (image, NULL, &dimensionHeight);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageDimension (image, &dimensionWidth, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageType ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	ImageType type;

	// Negative tests.
	status = GdipGetImageType (NULL, &type);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageType (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageWidth ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT width;

	// Negative tests.
	status = GdipGetImageWidth (NULL, &width);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageWidth (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageHeight ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT height;

	// Negative tests.
	status = GdipGetImageHeight (NULL, &height);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageHeight (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageHorizontalResolution ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	REAL horizontalResolution;

	// Negative tests.
	status = GdipGetImageHorizontalResolution (NULL, &horizontalResolution);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageHorizontalResolution (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageVerticalResolution ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	REAL verticalResolution;

	// Negative tests.
	status = GdipGetImageVerticalResolution (NULL, &verticalResolution);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageVerticalResolution (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageFlags ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT flags;

	// Negative tests.
	status = GdipGetImageFlags (NULL, &flags);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageFlags (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImageRawFormat ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	GUID rawFormat;

	// Negative tests.
	status = GdipGetImageRawFormat (NULL, &rawFormat);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageRawFormat (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getImagePixelFormat ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	PixelFormat pixelFormat;

	// Negative tests.
	status = GdipGetImagePixelFormat (NULL, &pixelFormat);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePixelFormat (image, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static BOOL callback (void *callbackData) { return TRUE; }

static void test_getImageThumbnail ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GpImage *thumbImage;

#if defined(USE_WINDOWS_GDIPLUS)
	// ImageTypeBitmap - non zero width and height.
	status = GdipGetImageThumbnail (bitmapImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyImage (thumbImage, ImageTypeBitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 0, 0, 10, 10, 10, 10, 2, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - zero width and height.
	status = GdipGetImageThumbnail (bitmapImage, 0, 0, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyImage (thumbImage, ImageTypeBitmap, memoryBmpRawFormat, PixelFormat32bppPARGB, 0, 0, 120, 120, 120, 120, 2, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - non zero width and height.
	status = GdipGetImageThumbnail (metafileImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyImage (thumbImage, ImageTypeBitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 0, 0, 10, 10, 10, 10, 2, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - zero width and height.
	status = GdipGetImageThumbnail (metafileImage, 0, 0, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyImage (thumbImage, ImageTypeBitmap, memoryBmpRawFormat, PixelFormat32bppARGB, 0, 0, 120, 120, 120, 120, 2, 0, TRUE);
	GdipDisposeImage (thumbImage);
#else
	status = GdipGetImageThumbnail (metafileImage, 0, 0, &thumbImage, NULL, NULL);
	assert (status == NotImplemented);
#endif

	// Negative tests.
	status = GdipGetImageThumbnail (NULL, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageThumbnail (bitmapImage, 10, 10, NULL, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getEncoderParameterListSize ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT size;

	status = GdipGetEncoderParameterListSize (image, &bmpEncoderClsid, &size);
	assertEqualInt (status, NotImplemented);

	status = GdipGetEncoderParameterListSize (image, &tifEncoderClsid, &size);
	// FIXME: this returns NotImplemented with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	assertEqualInt (size, is_32bit() ? 164 : 184);
#endif

	status = GdipGetEncoderParameterListSize (image, &gifEncoderClsid, &size);
	// FIXME: this returns FileNotFound with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	assertEqualInt (size, is_32bit() ? 64 : 80);
#endif

	status = GdipGetEncoderParameterListSize (image, &pngEncoderClsid, &size);
	// FIXME: this returns FileNotFound with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	assertEqualInt (size, is_32bit() ? 32 : 40);
#endif

	status = GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, &size);
	assertEqualInt (status, Ok);
	// FIXME: this returns 44 with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (size, is_32bit() ? 172 : 200);
#endif

	status = GdipGetEncoderParameterListSize (image, &icoEncoderClsid, &size);
	assertEqualInt (status, FileNotFound);

	status = GdipGetEncoderParameterListSize (image, &wmfEncoderClsid, &size);
	assertEqualInt (status, FileNotFound);

	status = GdipGetEncoderParameterListSize (image, &emfEncoderClsid, &size);
	assertEqualInt (status, FileNotFound);

	// Negative tests.
	status = GdipGetEncoderParameterListSize (NULL, &emfEncoderClsid, &size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterListSize (image, NULL, &size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterListSize (image, &emfEncoderClsid, NULL);
	assertEqualInt (status, FileNotFound);

	status = GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getEncoderParameterList ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	EncoderParameters buffer;
	UINT size;

	GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, &size);

	// Negative tests.
	status = GdipGetEncoderParameterList (NULL, &emfEncoderClsid, 100, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, NULL, 100, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &emfEncoderClsid, 100, NULL);
	assertEqualInt (status, FileNotFound);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, 100, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, 0, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, -1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, size - 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (image);
}

static void test_getFrameDimensionsCount ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT count;

	status = GdipImageGetFrameDimensionsCount (bitmapImage, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	status = GdipImageGetFrameDimensionsCount (metafileImage, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	// Negative tests.
	status = GdipImageGetFrameDimensionsCount (NULL, &count);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageGetFrameDimensionsCount (metafileImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getFrameDimensionsList ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GUID dimensions[1];

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, 1);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipImageGetFrameDimensionsList (NULL, dimensions, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageGetFrameDimensionsList (bitmapImage, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, 0);
	assertEqualInt (status, Win32Error);

	status = GdipImageGetFrameDimensionsList (metafileImage, dimensions, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, -1);
	assertEqualInt (status, Win32Error);

	status = GdipImageGetFrameDimensionsList (metafileImage, dimensions, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, 2);
	assertEqualInt (status, Win32Error);

	status = GdipImageGetFrameDimensionsList (metafileImage, dimensions, 2);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getFrameCount ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GUID pageDimension = {0x7462dc86, 0x6180, 0x4c7e, {0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83}};
	GUID timeDimension = {0x6aedbd6d, 0x3fb5, 0x418a, {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};
	UINT count;

	// Bitmap - page dimension.
	status = GdipImageGetFrameCount (bitmapImage, &pageDimension, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	// Metafile - null dimension.
	count = -1;
	status = GdipImageGetFrameCount (metafileImage, NULL, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	// Metafile - no such dimension.
	count = -1;
	status = GdipImageGetFrameCount (metafileImage, &emfEncoderClsid, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 1);

	// Negative tests.
	status = GdipImageGetFrameCount (NULL, &pageDimension, &count);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageGetFrameCount (bitmapImage, NULL, &count);
	assertEqualInt (status, Win32Error);

	status = GdipImageGetFrameCount (bitmapImage, &pageDimension, NULL);
	assertEqualInt (status, Win32Error);

	status = GdipImageGetFrameCount (metafileImage, &pageDimension, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageGetFrameCount (bitmapImage, &timeDimension, &count);
	assertEqualInt (status, Win32Error);

	status = GdipImageGetFrameCount (bitmapImage, &emfEncoderClsid, &count);
	assertEqualInt (status, Win32Error);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_selectActiveFrame ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GUID pageDimension = {0x7462dc86, 0x6180, 0x4c7e, {0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83}};
	GUID timeDimension = {0x6aedbd6d, 0x3fb5, 0x418a, {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};

	// Bitmap - page dimension.
	status = GdipImageSelectActiveFrame (bitmapImage, &pageDimension, 0);
	assertEqualInt (status, Ok);

	// Metafile - page dimension.
	status = GdipImageSelectActiveFrame (metafileImage, &pageDimension, 100);
	assertEqualInt (status, Ok);

	// Metafile - time dimension.
	status = GdipImageSelectActiveFrame (metafileImage, &timeDimension, 100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipImageSelectActiveFrame (NULL, &pageDimension, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageSelectActiveFrame (bitmapImage, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageSelectActiveFrame (metafileImage, NULL, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageSelectActiveFrame (bitmapImage, &pageDimension, 4);
	assertEqualInt (status, Win32Error);

	status = GdipImageSelectActiveFrame (metafileImage, &pageDimension, 200);
	assertEqualInt (status, Ok);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_forceValidation ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");

	// ImageTypeBitmap.
	status = GdipImageForceValidation (bitmapImage);
	assertEqualInt (status, Ok);

	// ImageTypeMetafile.
	status = GdipImageForceValidation (metafileImage);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipImageForceValidation (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_rotateFlip ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");

	// RotateNoneFlipNone.
	status = GdipImageRotateFlip (bitmapImage, RotateNoneFlipNone);
	assertEqualInt (status, Ok);

	// Rotate90FlipNone.
	status = GdipImageRotateFlip (bitmapImage, Rotate90FlipNone);
	assertEqualInt (status, Ok);

	// Rotate180FlipNone.
	status = GdipImageRotateFlip (bitmapImage, Rotate180FlipNone);
	assertEqualInt (status, Ok);

	// Rotate270FlipNone.
	status = GdipImageRotateFlip (bitmapImage, Rotate270FlipNone);
	assertEqualInt (status, Ok);

	// RotateNoneFlipX.
	status = GdipImageRotateFlip (bitmapImage, RotateNoneFlipX);
	assertEqualInt (status, Ok);

	// Rotate90FlipX.
	status = GdipImageRotateFlip (bitmapImage, Rotate90FlipX);
	assertEqualInt (status, Ok);

	// Rotate180FlipX.
	status = GdipImageRotateFlip (bitmapImage, Rotate180FlipX);
	assertEqualInt (status, Ok);

	// Rotate270FlipX.
	status = GdipImageRotateFlip (bitmapImage, Rotate270FlipX);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipImageRotateFlip (NULL, RotateNoneFlipNone);
	assertEqualInt (status, InvalidParameter);

	status = GdipImageRotateFlip (metafileImage, Rotate270FlipX);
	assertEqualInt (status, NotImplemented);

	status = GdipImageRotateFlip (bitmapImage, (RotateFlipType)(RotateNoneFlipNone - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipImageRotateFlip (bitmapImage, (RotateFlipType)(Rotate270FlipX + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getImagePalette ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	INT size;
	BYTE buffer1[1040];
	ColorPalette *palette = (ColorPalette *)buffer1;

	BYTE buffer2[1040];
	ColorPalette *nonEmptyPalette = (ColorPalette *)buffer2;

	GdipGetImagePaletteSize (bitmapImage, &size);

	// Empty palette - same size.
	status = GdipGetImagePalette (bitmapImage, palette, size);
	assertEqualInt (status, Ok);
	assertEqualInt (palette->Count, 0);

	// Empty palette - larger size.
	palette->Count = 100;
	status = GdipGetImagePalette (bitmapImage, palette, size + 1);
	assertEqualInt (status, Ok);
	assertEqualInt (palette->Count, 0);

	// Empty palette - negative size.
	palette->Count = 100;
	status = GdipGetImagePalette (bitmapImage, palette, -1);
	assertEqualInt (status, Ok);
	assertEqualInt (palette->Count, 0);

	// Negative tests.
	status = GdipGetImagePalette (NULL, palette, size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePalette (bitmapImage, NULL, size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePalette (metafileImage, NULL, size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePalette (metafileImage, palette, size);
	assertEqualInt (status, NotImplemented);

	status = GdipGetImagePalette (metafileImage, palette, -1);
	assertEqualInt (status, NotImplemented);

	status = GdipGetImagePalette (bitmapImage, palette, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePalette (metafileImage, palette, 0);
	assertEqualInt (status, NotImplemented);

	status = GdipGetImagePalette (bitmapImage, palette, size - 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePalette (metafileImage, palette, size - 1);
	assertEqualInt (status, NotImplemented);

	// Non empty palette - setup.
	nonEmptyPalette->Count = 10;
	nonEmptyPalette->Flags = 1;
	nonEmptyPalette->Entries[0] = 2;
	status = GdipSetImagePalette (bitmapImage, nonEmptyPalette);
	assertEqualInt (status, Ok);

	GdipGetImagePaletteSize (bitmapImage, &size);

	// Non empty palette - same size.
	palette->Count = 100;
	palette->Flags = 100;
	palette->Entries[0] = 100;

	status = GdipGetImagePalette (bitmapImage, palette, size);
	assertEqualInt (status, Ok);
	assertEqualInt (palette->Count, 10);
	assertEqualInt (palette->Flags, 1);
	assertEqualInt (palette->Entries[0], 2);

	// Non empty palette - larger size.
	palette->Count = 100;
	palette->Flags = 100;
	palette->Entries[0] = 100;

	status = GdipGetImagePalette (bitmapImage, palette, size + 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePalette (bitmapImage, palette, size - 1);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_setImagePalette ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	INT size;
	BYTE buffer1[1040];
	ColorPalette *palette = (ColorPalette *)buffer1;
	BYTE buffer2[1040];
	ColorPalette *resultPalette = (ColorPalette *)buffer2;

	GdipGetImagePaletteSize (bitmapImage, &size);

	// Set with positive count.
	palette->Count = 10;
	palette->Flags = 1;
	palette->Entries[0] = 2;
	status = GdipSetImagePalette (bitmapImage, palette);
	assertEqualInt (status, Ok);

	status = GdipGetImagePaletteSize (bitmapImage, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, 48);

	status = GdipGetImagePalette (bitmapImage, resultPalette, size);
	assertEqualInt (status, Ok);
	assertEqualInt (resultPalette->Count, 10);
	assertEqualInt (resultPalette->Flags, 1);
	assertEqualInt (resultPalette->Entries[0], 2);

	// Set with large count.
	palette->Count = 256;
	palette->Flags = 1;
	palette->Entries[0] = 20;
	status = GdipSetImagePalette (bitmapImage, palette);
	assertEqualInt (status, Ok);

	status = GdipGetImagePaletteSize (bitmapImage, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, 1032);

	status = GdipGetImagePalette (bitmapImage, resultPalette, size);
	assertEqualInt (status, Ok);
	assertEqualInt (resultPalette->Count, 256);
	assertEqualInt (resultPalette->Flags, 1);
	assertEqualInt (resultPalette->Entries[0], 20);

	// Negative tests.
	status = GdipSetImagePalette (NULL, palette);
	assertEqualInt (status, InvalidParameter);

	palette->Count = 10;
	status = GdipSetImagePalette (metafileImage, palette);
	assertEqualInt (status, NotImplemented);

	palette->Count = 0;
	status = GdipSetImagePalette (bitmapImage, palette);
	assertEqualInt (status, InvalidParameter);

	palette->Count = -1;
	status = GdipSetImagePalette (bitmapImage, palette);
	assertEqualInt (status, InvalidParameter);

	palette->Count = 257;
	status = GdipSetImagePalette (bitmapImage, palette);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImagePalette (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImagePalette (metafileImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getImagePaletteSize ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	INT size;

	status = GdipGetImagePaletteSize (bitmapImage, &size);
	assertEqualInt (status, Ok);
	assertEqualInt ((int) sizeof(ColorPalette), 12);
	assertEqualInt (size, 12);

	// Negative tests.
	status = GdipGetImagePaletteSize (NULL, &size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePaletteSize (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImagePaletteSize (metafileImage, &size);
	assertEqualInt (status, GenericError);

	status = GdipGetImagePaletteSize (metafileImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertyCount ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT count;

	// ImageTypeBitmap.
	status = GdipGetPropertyCount (bitmapImage, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);

	// ImageTypeMetafile.
	status = GdipGetPropertyCount (metafileImage, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);

	// Negative tests.
	status = GdipGetPropertyCount (NULL, &count);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyCount (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertyIdList ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID list[2];

	// ImageTypeBitmap.
	status = GdipGetPropertyIdList (bitmapImage, 0, list);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipGetPropertyIdList (NULL, 0, list);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyIdList (metafileImage, 0, list);
	assertEqualInt (status, NotImplemented);

	status = GdipGetPropertyIdList (bitmapImage, 0, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyIdList (metafileImage, 0, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyIdList (bitmapImage, 1, list);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyIdList (metafileImage, 1, list);
	assertEqualInt (status, NotImplemented);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertyItemSize ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID prop = 100;
	UINT size;

	// Negative tests.
	status = GdipGetPropertyItemSize (NULL, prop, &size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyItemSize (bitmapImage, prop, &size);
	assertEqualInt (status, PropertyNotFound);

	status = GdipGetPropertyItemSize (metafileImage, prop, &size);
	assertEqualInt (status, NotImplemented);

	status = GdipGetPropertyItemSize (bitmapImage, prop, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyItemSize (metafileImage, prop, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertyItem ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID prop = 100;
	PropertyItem propertyItem;

	// Negative tests.
	status = GdipGetPropertyItem (NULL, prop, 1, &propertyItem);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyItem (bitmapImage, prop, 1, &propertyItem);
	assertEqualInt (status, PropertyNotFound);

	status = GdipGetPropertyItem (metafileImage, prop, 1, &propertyItem);
	assertEqualInt (status, NotImplemented);

	status = GdipGetPropertyItem (bitmapImage, prop, 1, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyItem (metafileImage, prop, 1, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertySize ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT totalBufferSize;
	UINT numProperties;

	status = GdipGetPropertySize (bitmapImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, 0);
	assertEqualInt (numProperties, 0);

	// Negative tests.
	status = GdipGetPropertySize (NULL, &totalBufferSize, &numProperties);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertySize (metafileImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, NotImplemented);

	status = GdipGetPropertySize (bitmapImage, NULL, &numProperties);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertySize (metafileImage, NULL, &numProperties);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertySize (bitmapImage, &totalBufferSize, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertySize (metafileImage, &totalBufferSize, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_getAllPropertyItems ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PropertyItem propertyItems[2];

	// Negative tests.
	status = GdipGetAllPropertyItems (NULL, 0, 0, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (bitmapImage, 0, 0, propertyItems);
	assertEqualInt (status, GenericError);

	status = GdipGetAllPropertyItems (metafileImage, 0, 0, propertyItems);
	assertEqualInt (status, NotImplemented);

	status = GdipGetAllPropertyItems (bitmapImage, 0, 0, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (metafileImage, 0, 0, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (bitmapImage, 1, 0, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (metafileImage, 1, 0, propertyItems);
	assertEqualInt (status, NotImplemented);

	status = GdipGetAllPropertyItems (bitmapImage, 0, 1, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (metafileImage, 0, 1, propertyItems);
	assertEqualInt (status, NotImplemented);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void test_removePropertyItem ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID prop = 100;

	// Negative tests.
	status = GdipRemovePropertyItem (NULL, prop);
	assertEqualInt (status, InvalidParameter);

	status = GdipRemovePropertyItem (bitmapImage, prop);
	assertEqualInt (status, GenericError);

	status = GdipRemovePropertyItem (metafileImage, prop);
	assertEqualInt (status, NotImplemented);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
}

static void setPropertyItemForImage (GpImage *image)
{
	GpStatus status;

	INT temp = 1;
	PropertyItem propertyItem1 = {10, 0, 11, &temp};
	PropertyItem propertyItem2 = {11, 0, 12, NULL};
	PropertyItem propertyItem3 = {10, 0, 9, NULL};

	UINT propertySize;
	PropertyItem resultPropertyItem;
	UINT numProperties;
	UINT totalBufferSize;
	PROPID propertyIds[2];

	// Set new property.
	status = GdipSetPropertyItem (image, &propertyItem1);
	assertEqualInt (status, Ok);

	status = GdipGetPropertyItemSize (image, propertyItem1.id, &propertySize);
	assertEqualInt (status, Ok);
	assertEqualInt (propertySize, (int) sizeof(PropertyItem));

	status = GdipGetPropertyItem (image, propertyItem1.id, propertySize, &resultPropertyItem);
	assertEqualInt (status, Ok);
	assertEqualInt (resultPropertyItem.id, 10);
	assertEqualInt (resultPropertyItem.length, 0);
	assertEqualInt (resultPropertyItem.type, 11);

	status = GdipGetPropertyCount (image, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (numProperties, 1);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, (int) sizeof(PropertyItem));
	assertEqualInt (numProperties, 1);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyIds[0], 10);
	assertEqualInt (propertyIds[1], -1);

	// Set another new property.
	status = GdipSetPropertyItem (image, &propertyItem2);
	assertEqualInt (status, Ok);

	status = GdipGetPropertyItemSize (image, propertyItem2.id, &propertySize);
	assertEqualInt (status, Ok);
	assertEqualInt (propertySize, (int) sizeof(PropertyItem));

	status = GdipGetPropertyItem (image, propertyItem2.id, propertySize, &resultPropertyItem);
	assertEqualInt (status, Ok);
	assertEqualInt (resultPropertyItem.id, 11);
	assertEqualInt (resultPropertyItem.length, 0);
	assertEqualInt (resultPropertyItem.type, 12);

	status = GdipGetPropertyCount (image, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (numProperties, 2);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, (int) sizeof(PropertyItem) * 2);
	assertEqualInt (numProperties, 2);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyIds[0], 10);
	assertEqualInt (propertyIds[1], 11);

	// Override an existing property.
	status = GdipSetPropertyItem (image, &propertyItem3);
	assertEqualInt (status, Ok);

	status = GdipGetPropertyItemSize (image, propertyItem3.id, &propertySize);
	assertEqualInt (status, Ok);
	assertEqualInt (propertySize, (int) sizeof(PropertyItem));

	status = GdipGetPropertyItem (image, propertyItem3.id, propertySize, &resultPropertyItem);
	assertEqualInt (status, Ok);
	assertEqualInt (resultPropertyItem.id, 10);
	assertEqualInt (resultPropertyItem.length, 0);
	assertEqualInt (resultPropertyItem.type, 9);

	status = GdipGetPropertyCount (image, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (numProperties, 2);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, (int) sizeof(PropertyItem) * 2);
	assertEqualInt (numProperties, 2);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyIds[0], 10);
	assertEqualInt (propertyIds[1], 11);
}

static void test_setPropertyItem()
{
	GpStatus status;
	GpImage *bmpImage = getImage ("test.bmp");
	GpImage *tifImage = getImage ("test.tif");
	GpImage *gifImage = getImage ("test.gif");
	GpImage *pngImage = getImage ("test.png");
	GpImage *jpgImage = getImage ("test.jpg");
	GpImage *icoImage = getImage ("test.ico");
	GpImage *metafileImage = getImage ("test.wmf");
	PropertyItem propertyItem = {10, 0, 11, NULL};

	setPropertyItemForImage (bmpImage);

	status = GdipSetPropertyItem (tifImage, &propertyItem);
	assertEqualInt (status, Ok);

	status = GdipSetPropertyItem (gifImage, &propertyItem);
	assertEqualInt (status, Ok);

	status = GdipSetPropertyItem (pngImage, &propertyItem);
	assertEqualInt (status, Ok);

	status = GdipSetPropertyItem (jpgImage, &propertyItem);
	assertEqualInt (status, Ok);

	status = GdipSetPropertyItem (icoImage, &propertyItem);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetPropertyItem (NULL, &propertyItem);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPropertyItem (metafileImage, &propertyItem);
	assertEqualInt (status, NotImplemented);

	status = GdipSetPropertyItem (bmpImage, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPropertyItem (metafileImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bmpImage);
	GdipDisposeImage (tifImage);
	GdipDisposeImage (gifImage);
	GdipDisposeImage (pngImage);
	GdipDisposeImage (jpgImage);
	GdipDisposeImage (icoImage);
	GdipDisposeImage (metafileImage);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	test_loadImageFromStream ();
	test_loadImageFromFile ();
	test_loadImageFromStreamICM ();
	test_loadImageFromFileICM ();
	test_loadImageFromFileBmp ();
	test_loadImageFromFileTif ();
	test_loadImageFromFileGif ();
	test_loadImageFromFilePng ();
	test_loadImageFromFileJpg ();
	test_loadImageFromFileIcon ();
	test_loadImageFromFileWmf ();
	test_loadImageFromFileEmf ();
	test_cloneImage ();
	test_disposeImage ();
	test_getImageGraphicsContext ();
	test_getImageBounds ();
	test_getImageDimension ();
	test_getImageType ();
	test_getImageWidth ();
	test_getImageHeight ();
	test_getImageHorizontalResolution ();
	test_getImageVerticalResolution ();
	test_getImageFlags ();
	test_getImageRawFormat ();
	test_getImagePixelFormat ();
	test_getImageThumbnail ();
	test_getEncoderParameterListSize ();
	test_getEncoderParameterList ();
	test_getFrameDimensionsCount ();
	test_getFrameDimensionsList ();
	test_getFrameCount ();
	test_selectActiveFrame ();
	test_forceValidation ();
	test_rotateFlip ();
	test_getImagePalette ();
	test_setImagePalette ();
	test_getImagePaletteSize ();
	test_getPropertyCount ();
	test_getPropertyIdList ();
	test_getPropertyItemSize ();
	test_getPropertyItem ();
	test_getPropertySize ();
	test_getAllPropertyItems ();
	test_removePropertyItem ();
	test_setPropertyItem ();

	GdiplusShutdown (gdiplusToken);
	return 0;
}
