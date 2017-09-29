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

static void verifyImage (GpImage *image, ImageType expectedType, PixelFormat expectedPixelFormat, REAL expectedX, REAL expectedY, UINT expectedWidth, UINT expectedHeight, REAL expectedDimensionWidth, REAL expectedDimensionHeight)
{
	GpStatus status;
	ImageType type;
	PixelFormat pixelFormat;
	UINT width;
	UINT height;
	GpRectF bounds;
	GpUnit unit;
	REAL dimensionWidth;
	REAL dimensionHeight;

	status = GdipGetImageType (image, &type);
	assertEqualInt (status, Ok);
	assertEqualInt (type, expectedType);

	status = GdipGetImagePixelFormat (image, &pixelFormat);
	assertEqualInt (status, Ok);
	assertEqualInt (pixelFormat, expectedPixelFormat);

	status = GdipGetImageWidth (image, &width);
	assertEqualInt (status, Ok);
	assertEqualInt (width, expectedWidth);

	status = GdipGetImageHeight (image, &height);
	assertEqualInt (status, Ok);
	assertEqualInt (height, expectedHeight);

	status = GdipGetImageBounds (image, &bounds, &unit);
	assertEqualInt (status, Ok);
	assertEqualFloat (bounds.X, expectedX);
	assertEqualFloat (bounds.Y, expectedY);
	assertEqualFloat (bounds.Width, (REAL)expectedWidth);
	assertEqualFloat (bounds.Height, (REAL)expectedHeight);
	assertEqualInt (unit, UnitPixel);

	// Libgdiplus and GDI+ have different exact degrees of accuracy.
	// Typically they differ by +-0.02.
	// This is an acceptable difference.
	status = GdipGetImageDimension (image, &dimensionWidth, &dimensionHeight);
	assertEqualInt (status, Ok);
	assert (fabsf (dimensionWidth - expectedDimensionWidth) <= 0.05);
	assert (fabsf (dimensionHeight - expectedDimensionHeight) <= 0.05);
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

	verifyImage (image, ImageTypeBitmap, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileTif ()
{
	GpImage *image = getImage ("test.tif");

	verifyImage (image, ImageTypeBitmap, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileGif ()
{
	GpImage *image = getImage ("test.gif");

	verifyImage (image, ImageTypeBitmap, PixelFormat8bppIndexed, 0, 0, 100, 68, 100, 68);

	GdipDisposeImage (image);
}

static void test_loadImageFromFilePng ()
{
	GpImage *image = getImage ("test.png");

	verifyImage (image, ImageTypeBitmap, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileJpg ()
{
	GpImage *image = getImage ("test.jpg");

	verifyImage (image, ImageTypeBitmap, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileIcon ()
{
	GpImage *image = getImage ("test.ico");

	verifyImage (image, ImageTypeBitmap, PixelFormat32bppARGB, 0, 0, 48, 48, 48, 48);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileWmf ()
{
	GpImage *image = getImage ("test.wmf");

	verifyImage (image, ImageTypeMetafile, PixelFormat32bppRGB, -4008, -3378, 8016, 6756, 20360.64f, 17160.24f);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileEmf ()
{
	GpImage *image = getImage ("test.emf");

	// FIXME: libgdiplus is totally wrong with the size of this image.
#if defined(USE_WINDOWS_GDIPLUS)
	verifyImage (image, ImageTypeMetafile, PixelFormat32bppRGB, 0, 0, 100, 100, 1944.44f, 1888.88f);
#endif

	GdipDisposeImage (image);
}

static void test_cloneImage ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GpImage *clonedImage;

	// ImageTypeBitmap.
	status = GdipCloneImage (bitmapImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != bitmapImage);
	verifyImage (clonedImage, ImageTypeBitmap, PixelFormat24bppRGB, 0, 0, 100, 68, 100, 68);
	GdipDisposeImage (clonedImage);

	// ImageTypeMetafile.
	status = GdipCloneImage (metafileImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != metafileImage);
	verifyImage (clonedImage, ImageTypeMetafile, PixelFormat32bppRGB, -4008, -3378, 8016, 6756, 20360.64f, 17160.24f);
	GdipDisposeImage (clonedImage);

	// Negative tests.
	status = GdipCloneImage (NULL, &clonedImage);
	assertEqualInt (status, InvalidParameter);

	status = GdipCloneImage (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
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
	verifyImage (thumbImage, ImageTypeBitmap, PixelFormat32bppPARGB, 0, 0, 10, 10, 10, 10);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - zero width and height.
	status = GdipGetImageThumbnail (bitmapImage, 0, 0, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyImage (thumbImage, ImageTypeBitmap, PixelFormat32bppPARGB, 0, 0, 120, 120, 120, 120);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - non zero width and height.
	status = GdipGetImageThumbnail (metafileImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyImage (thumbImage, ImageTypeBitmap, PixelFormat32bppARGB, 0, 0, 10, 10, 10, 10);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - zero width and height.
	status = GdipGetImageThumbnail (metafileImage, 0, 0, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyImage (thumbImage, ImageTypeBitmap, PixelFormat32bppARGB, 0, 0, 120, 120, 120, 120);
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
	assertEqualInt (size, 164);
#endif

	status = GdipGetEncoderParameterListSize (image, &gifEncoderClsid, &size);
// FIXME: this returns FileNotFound with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	assertEqualInt (size, 64);
#endif

	status = GdipGetEncoderParameterListSize (image, &pngEncoderClsid, &size);
// FIXME: this returns FileNotFound with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (status, Ok);
	assertEqualInt (size, 32);
#endif

	status = GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, &size);
	assertEqualInt (status, Ok);
// FIXME: this returns 44 with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualInt (size, 172);
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
	test_rotateFlip ();
	test_forceValidation ();

	GdiplusShutdown (gdiplusToken);
	return 0;
}
