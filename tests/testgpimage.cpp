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

#include <gtest/gtest.h>
#include "testhelpers.h"

static GpImage* getImage (const char* fileName)
{
	GpStatus status;
	WCHAR *wFileName = wcharFromChar (fileName);
	GpImage *image;

	status = GdipLoadImageFromFile (wFileName, &image);
	assert (status == Ok);

	freeWchar (wFileName);

	return image;
}

TEST(Image, GdipLoadImageFromStream)
{
	STARTUP

	GpStatus status;
	GpImage *image;

	// Negative tests.
	status = GdipLoadImageFromStream (NULL, &image);
	ASSERT_EQ (InvalidParameter, status);

#if !defined(USE_WINDOWS_GDIPLUS)
	int temp = 0;

	status = GdipLoadImageFromStream (&temp, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipLoadImageFromStream (&temp, &image);
	ASSERT_EQ (NotImplemented, status);
#endif

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFile)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	// Negative tests.
	status = GdipLoadImageFromFile (NULL, &image);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipLoadImageFromFile (noSuchFile, &image);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipLoadImageFromFile (invalidFile, &image);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipLoadImageFromFile (noSuchFile, NULL);
	ASSERT_EQ (InvalidParameter, status);

	freeWchar (noSuchFile);
	freeWchar (invalidFile);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromStreamICM)
{
	STARTUP

	GpStatus status;
	GpImage *image;

	// Negative tests.
	status = GdipLoadImageFromStreamICM (NULL, &image);
	ASSERT_EQ (InvalidParameter, status);

#if !defined(USE_WINDOWS_GDIPLUS)
	int temp = 0;

	status = GdipLoadImageFromStreamICM (&temp, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipLoadImageFromStreamICM (&temp, &image);
	ASSERT_EQ (NotImplemented, status);
#endif

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFileICM)
{
	STARTUP

	GpStatus status;
	GpImage *image;
	WCHAR *noSuchFile = createWchar ("noSuchFile.bmp");
	WCHAR *invalidFile = createWchar ("test.ttf");

	// Negative tests.
	status = GdipLoadImageFromFileICM (NULL, &image);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipLoadImageFromFileICM (noSuchFile, &image);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipLoadImageFromFileICM (invalidFile, &image);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipLoadImageFromFileICM (noSuchFile, NULL);
	ASSERT_EQ (InvalidParameter, status);

	freeWchar (noSuchFile);
	freeWchar (invalidFile);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFileTif)
{
	STARTUP

	GpImage *image = getImage ("test.tif");

	verifyBitmap (image, tifRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 19, TRUE);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFileGif)
{
	STARTUP

	GpImage *image = getImage ("test.gif");

	verifyBitmap (image, gifRawFormat, PixelFormat8bppIndexed, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 4, TRUE);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFilePng)
{
	STARTUP

	GpImage *image = getImage ("test.png");

	verifyBitmap (image, pngRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5, TRUE);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFileJpg)
{
	STARTUP

	GpImage *image = getImage ("test.jpg");

	verifyBitmap (image, jpegRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 2, TRUE);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFileIcon)
{
	STARTUP

	GpImage *image = getImage ("test.ico");

	verifyBitmap (image, icoRawFormat, PixelFormat32bppARGB, 48, 48, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsHasAlpha | ImageFlagsReadOnly, 0, TRUE);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFileWmf)
{
	STARTUP

	GpImage *image = getImage ("test.wmf");

	verifyMetafile (image, wmfRawFormat, -4008, -3378, 8016, 6756, 20360.638672f, 17160.2383f);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipLoadImageFromFileEmf)
{
	STARTUP

	GpImage *image = getImage ("test.emf");

	verifyMetafile (image, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipCloneImage)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpgImage = getImage ("test.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	GpImage *clonedImage;

	// ImageTypeBitmap - bmp.
	status = GdipCloneImage (bitmapImage, &clonedImage);
	ASSERT_EQ (Ok, status);
	assert (clonedImage && clonedImage != bitmapImage);
	verifyBitmap (clonedImage, bmpRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 0, TRUE);
	GdipDisposeImage (clonedImage);

	// ImageTypeBitmap - jpg.
	status = GdipCloneImage (jpgImage, &clonedImage);
	ASSERT_EQ (Ok, status);
	assert (clonedImage && clonedImage != jpgImage);
	verifyBitmap (clonedImage, jpegRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 2, TRUE);
	GdipDisposeImage (clonedImage);

	// ImageTypeMetafile.
	status = GdipCloneImage (metafileImage, &clonedImage);
	ASSERT_EQ (Ok, status);
	assert (clonedImage && clonedImage != metafileImage);
	verifyMetafile (clonedImage, wmfRawFormat, -4008, -3378, 8016, 6756, 20360.638672f, 17160.238281f);
	GdipDisposeImage (clonedImage);

	// Negative tests.
	status = GdipCloneImage (NULL, &clonedImage);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCloneImage (bitmapImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpgImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipDisposeImage)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");

	// ImageTypeBitmap.
	status = GdipDisposeImage (bitmapImage);
	ASSERT_EQ (Ok, status);

	// ImageTypeMetafile.
	status = GdipDisposeImage (metafileImage);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDisposeImage (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(Image, GdipGetImageGraphicsContext)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *gifImage = getImage ("test.gif");
	GpImage *metafileImage = getImage ("test.wmf");
	GpGraphics *graphics;

	// ImageTypeBitmap - PixelFormat24Bpp.
	status = GdipGetImageGraphicsContext (bitmapImage, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics);
	GdipDeleteGraphics (graphics);

	// FIXME: libgdiplus doesn't support PixelFormat8bppIndexed.
#if defined(USE_WINDOWS_GDIPLUS)
	// ImageTypeBitmap - PixelFormat8bppIndexed.
	status = GdipGetImageGraphicsContext (gifImage, &graphics);
	ASSERT_EQ (Ok, status);
	assert (graphics);
	GdipDeleteGraphics (graphics);
#endif

	// ImageTypeMetafile - not recording.
	status = GdipGetImageGraphicsContext (metafileImage, &graphics);
	ASSERT_EQ (OutOfMemory, status);

	// Negative tests.
	status = GdipGetImageGraphicsContext (NULL, &graphics);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageGraphicsContext (bitmapImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (gifImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetImageBounds)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	GpRectF bounds;
	Unit unit;

	// Negative tests.
	status = GdipGetImageBounds (NULL, &bounds, &unit);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageBounds (image, NULL, &unit);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageBounds (image, &bounds, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageDimension)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	REAL dimensionWidth;
	REAL dimensionHeight;

	// Negative tests.
	status = GdipGetImageDimension (NULL, &dimensionWidth, &dimensionHeight);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageDimension (image, NULL, &dimensionHeight);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageDimension (image, &dimensionWidth, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageType)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	ImageType type;

	// Negative tests.
	status = GdipGetImageType (NULL, &type);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageType (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageWidth)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT width;

	// Negative tests.
	status = GdipGetImageWidth (NULL, &width);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageWidth (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageHeight)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT height;

	// Negative tests.
	status = GdipGetImageHeight (NULL, &height);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageHeight (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageHorizontalResolution)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	REAL horizontalResolution;

	// Negative tests.
	status = GdipGetImageHorizontalResolution (NULL, &horizontalResolution);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageHorizontalResolution (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageVerticalResolution)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	REAL verticalResolution;

	// Negative tests.
	status = GdipGetImageVerticalResolution (NULL, &verticalResolution);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageVerticalResolution (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageFlags)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT flags;

	// Negative tests.
	status = GdipGetImageFlags (NULL, &flags);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageFlags (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImageRawFormat)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	GUID rawFormat;

	// Negative tests.
	status = GdipGetImageRawFormat (NULL, &rawFormat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageRawFormat (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetImagePixelFormat)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	PixelFormat pixelFormat;

	// Negative tests.
	status = GdipGetImagePixelFormat (NULL, &pixelFormat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePixelFormat (image, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

static BOOL callback (void *callbackData) { return TRUE; }

TEST(Image, GdipGetImageThumbnail)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *wmfImage = getImage ("test.wmf");
	GpImage *emfImage = getImage ("test.emf");
	GpImage *thumbImage;

	// ImageTypeBitmap - non zero width and height.
	status = GdipGetImageThumbnail (bitmapImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 10, 10, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - width > height.
	status = GdipGetImageThumbnail (bitmapImage, 20, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 20, 10, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - height > width.
	status = GdipGetImageThumbnail (bitmapImage, 10, 20, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 10, 20, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - zero width and height.
	status = GdipGetImageThumbnail (bitmapImage, 0, 0, &thumbImage, NULL, NULL);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 120, 120, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - non zero width and height.
	status = GdipGetImageThumbnail (wmfImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 10, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - width > height.
	status = GdipGetImageThumbnail (wmfImage, 20, 10, &thumbImage, NULL, NULL);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 20, 10, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - height > width.
	status = GdipGetImageThumbnail (wmfImage, 10, 20, &thumbImage, NULL, NULL);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 20, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - non zero width and height.
	status = GdipGetImageThumbnail (emfImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 10, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - width > height.
	status = GdipGetImageThumbnail (emfImage, 20, 10, &thumbImage, NULL, NULL);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 20, 10, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - height > width.
	status = GdipGetImageThumbnail (emfImage, 10, 20, &thumbImage, NULL, NULL);
	ASSERT_EQ (Ok, status);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 20, ImageFlagsHasAlpha, 0, TRUE);
	GdipDisposeImage (thumbImage);

	// Negative tests.
	status = GdipGetImageThumbnail (NULL, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageThumbnail (bitmapImage, 10, 10, NULL, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageThumbnail (bitmapImage, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetImageThumbnail (wmfImage, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetImageThumbnail (emfImage, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetImageThumbnail (NULL, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageThumbnail (bitmapImage, 0, 10, NULL, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageThumbnail (bitmapImage, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetImageThumbnail (wmfImage, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetImageThumbnail (emfImage, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipGetImageThumbnail (NULL, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageThumbnail (bitmapImage, 10, 0, NULL, (GetThumbnailImageAbort) callback, (void *) 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (wmfImage);
	GdipDisposeImage (emfImage);

	SHUTDOWN
}

TEST(Image, GdipGetEncoderParameterListSize)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT size;

	status = GdipGetEncoderParameterListSize (image, &bmpEncoderClsid, &size);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetEncoderParameterListSize (image, &tifEncoderClsid, &size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (is_32bit() ? 164 : 184, size);

	status = GdipGetEncoderParameterListSize (image, &gifEncoderClsid, &size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (is_32bit() ? 64 : 80, size);

	status = GdipGetEncoderParameterListSize (image, &pngEncoderClsid, &size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (is_32bit() ? 32 : 40, size);

	status = GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, &size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (is_32bit() ? 172 : 200, size);

	status = GdipGetEncoderParameterListSize (image, &icoEncoderClsid, &size);
	ASSERT_EQ (FileNotFound, status);

	status = GdipGetEncoderParameterListSize (image, &wmfEncoderClsid, &size);
	ASSERT_EQ (FileNotFound, status);

	status = GdipGetEncoderParameterListSize (image, &emfEncoderClsid, &size);
	ASSERT_EQ (FileNotFound, status);

	// Negative tests.
	status = GdipGetEncoderParameterListSize (NULL, &emfEncoderClsid, &size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterListSize (image, NULL, &size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterListSize (image, &emfEncoderClsid, NULL);
	ASSERT_EQ (FileNotFound, status);

	status = GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipGetEncoderParameterList)
{
	STARTUP

	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT tiffSize;
	UINT gifSize;
	UINT pngSize;
	UINT jpegSize;
	EncoderParameters buffer;
	EncoderParameters *parameters;
	GUID compression = {0x0E09D739D, 0x0CCD4, 0x44EE, {0x8E, 0x0BA, 0x3F, 0x0BF, 0x8B, 0x0E4, 0x0FC, 0x58}};
	GUID colorDepth = {0x66087055, 0x0AD66, 0x4C7C, {0x9A, 0x18, 0x38, 0x0A2, 0x31, 0x0B, 0x83, 0x37}};
	GUID saveFlag = {0x292266FC, 0x0AC40, 0x47BF, {0x8C, 0x0FC, 0x0A8, 0x5B, 0x89, 0x0A6, 0x55, 0x0DE}};
	GUID saveAsCMYK = {0x0A219BBC9, 0x0A9D, 0x4005, {0x0A3, 0x0EE, 0x3A, 0x42, 0x1B, 0x8B, 0x0B0, 0x6C}};
	GUID imageItems = {0x63875E13, 0x1F1D, 0x45AB, {0x91, 0x95, 0x0A2, 0x9B, 0x60, 0x66, 0x0A6, 0x50}};
	GUID transformation = {0x8D0EB2D1, 0x0A58E, 0x4EA8, {0x0AA, 0x14, 0x10, 0x80, 0x74, 0x0B7, 0x0B6, 0x0F9}};
	GUID quality = {0x1D5BE4B5, 0x0FA4A, 0x452D, {0x9C, 0x0DD, 0x5D, 0x0B3, 0x51, 0x5, 0x0E7, 0x0EB}};
	GUID luminanceTable = {0x0EDB33BCE, 0x266, 0x4A77, {0x0B9, 0x4, 0x27, 0x21, 0x60, 0x99, 0x0E7, 0x17}};
	GUID chrominanceTable = {0x0F2E455DC, 0x9B3, 0x4316, {0x82, 0x60, 0x67, 0x6A, 0x0DA, 0x32, 0x48, 0x1C}};

	// TIFF encoder.
	GdipGetEncoderParameterListSize (image, &tifEncoderClsid, &tiffSize);
	parameters = (EncoderParameters *) malloc (tiffSize);

	status = GdipGetEncoderParameterList (image, &tifEncoderClsid, tiffSize, parameters);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (4, parameters->Count);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &compression, sizeof (GUID)) == 0);
	ASSERT_EQ (5, parameters->Parameter[0].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeLong, parameters->Parameter[0].Type);
	ASSERT_EQ (EncoderValueCompressionLZW, ((LONG *) parameters->Parameter[0].Value)[0]);
	ASSERT_EQ (EncoderValueCompressionCCITT3, ((LONG *) parameters->Parameter[0].Value)[1]);
	ASSERT_EQ (EncoderValueCompressionRle, ((LONG *) parameters->Parameter[0].Value)[2]);
	ASSERT_EQ (EncoderValueCompressionCCITT4, ((LONG *) parameters->Parameter[0].Value)[3]);
	ASSERT_EQ (EncoderValueCompressionNone, ((LONG *) parameters->Parameter[0].Value)[4]);

	assert (memcmp ((void *) &parameters->Parameter[1].Guid, (void *) &colorDepth, sizeof (GUID)) == 0);
	ASSERT_EQ (5, parameters->Parameter[1].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeLong, parameters->Parameter[1].Type);
	ASSERT_EQ (1, ((LONG *) parameters->Parameter[1].Value)[0]);
	ASSERT_EQ (4, ((LONG *) parameters->Parameter[1].Value)[1]);
	ASSERT_EQ (8, ((LONG *) parameters->Parameter[1].Value)[2]);
	ASSERT_EQ (24, ((LONG *) parameters->Parameter[1].Value)[3]);
	ASSERT_EQ (32, ((LONG *) parameters->Parameter[1].Value)[4]);

	assert (memcmp ((void *) &parameters->Parameter[2].Guid, (void *) &saveFlag, sizeof (GUID)) == 0);
	ASSERT_EQ (1, parameters->Parameter[2].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeLong, parameters->Parameter[2].Type);
	ASSERT_EQ (EncoderValueMultiFrame, ((LONG *) parameters->Parameter[2].Value)[0]);

	assert (memcmp ((void *) &parameters->Parameter[3].Guid, (void *) &saveAsCMYK, sizeof (GUID)) == 0);
	ASSERT_EQ (1, parameters->Parameter[3].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeLong, parameters->Parameter[3].Type);
	ASSERT_EQ (1, ((LONG *) parameters->Parameter[3].Value)[0]);

	free (parameters);

	// GIF encoder.
	GdipGetEncoderParameterListSize (image, &gifEncoderClsid, &gifSize);
	parameters = (EncoderParameters *) malloc (gifSize);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, gifSize, parameters);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, parameters->Count);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &imageItems, sizeof (GUID)) == 0);
	ASSERT_EQ (0, parameters->Parameter[0].NumberOfValues);
	ASSERT_EQ ((EncoderParameterValueType) 9, parameters->Parameter[0].Type);
	assert (!parameters->Parameter[0].Value);

	assert (memcmp ((void *) &parameters->Parameter[1].Guid, (void *) &saveFlag, sizeof (GUID)) == 0);
	ASSERT_EQ (1, parameters->Parameter[1].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeLong, parameters->Parameter[1].Type);
	ASSERT_EQ (EncoderValueMultiFrame, ((LONG *) parameters->Parameter[1].Value)[0]);

	free (parameters);

	// PNG encoder.
	GdipGetEncoderParameterListSize (image, &pngEncoderClsid, &pngSize);
	parameters = (EncoderParameters *) malloc (pngSize);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, pngSize, parameters);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, parameters->Count);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &imageItems, sizeof (GUID)) == 0);
	ASSERT_EQ (0, parameters->Parameter[0].NumberOfValues);
	ASSERT_EQ ((EncoderParameterValueType) 9, parameters->Parameter[0].Type);
	assert (!parameters->Parameter[0].Value);

	free (parameters);

	// JPEG encoder.
	GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, &jpegSize);
	parameters = (EncoderParameters *) malloc (jpegSize);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, jpegSize, parameters);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (5, parameters->Count);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &transformation, sizeof (GUID)) == 0);
	ASSERT_EQ (5, parameters->Parameter[0].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeLong, parameters->Parameter[0].Type);
	ASSERT_EQ (EncoderValueTransformRotate90, ((LONG *) parameters->Parameter[0].Value)[0]);
	ASSERT_EQ (EncoderValueTransformRotate180, ((LONG *) parameters->Parameter[0].Value)[1]);
	ASSERT_EQ (EncoderValueTransformRotate270, ((LONG *) parameters->Parameter[0].Value)[2]);
	ASSERT_EQ (EncoderValueTransformFlipHorizontal, ((LONG *) parameters->Parameter[0].Value)[3]);
	ASSERT_EQ (EncoderValueTransformFlipVertical, ((LONG *) parameters->Parameter[0].Value)[4]);

	assert (memcmp ((void *) &parameters->Parameter[1].Guid, (void *) &quality, sizeof (GUID)) == 0);
	ASSERT_EQ (1, parameters->Parameter[1].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeLongRange, parameters->Parameter[1].Type);
	ASSERT_EQ (0, ((LONG *) parameters->Parameter[1].Value)[0]);
	ASSERT_EQ (100, ((LONG *) parameters->Parameter[1].Value)[1]);

	assert (memcmp ((void *) &parameters->Parameter[2].Guid, (void *) &luminanceTable, sizeof (GUID)) == 0);
	ASSERT_EQ (0, parameters->Parameter[2].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeShort, parameters->Parameter[2].Type);
	assert (!parameters->Parameter[2].Value);

	assert (memcmp ((void *) &parameters->Parameter[3].Guid, (void *) &chrominanceTable, sizeof (GUID)) == 0);
	ASSERT_EQ (0, parameters->Parameter[3].NumberOfValues);
	ASSERT_EQ (EncoderParameterValueTypeShort, parameters->Parameter[3].Type);
	assert (!parameters->Parameter[3].Value);

	assert (memcmp ((void *) &parameters->Parameter[4].Guid, (void *) &imageItems, sizeof (GUID)) == 0);
	ASSERT_EQ (0, parameters->Parameter[4].NumberOfValues);
	ASSERT_EQ ((EncoderParameterValueType) 9, parameters->Parameter[4].Type);
	assert (!parameters->Parameter[4].Value);

	free (parameters);

	// Negative tests.
	status = GdipGetEncoderParameterList (NULL, &emfEncoderClsid, 100, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, NULL, 100, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &emfEncoderClsid, 100, NULL);
	ASSERT_EQ (FileNotFound, status);

	status = GdipGetEncoderParameterList (image, &tifEncoderClsid, 100, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, 100, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, 100, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, 100, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, 0, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, -1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &tifEncoderClsid, tiffSize - 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &tifEncoderClsid, tiffSize + 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, gifSize - 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, gifSize + 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, pngSize - 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, pngSize + 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, jpegSize - 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, jpegSize + 1, &buffer);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEncoderParameterList (image, &bmpEncoderClsid, 0, &buffer);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetEncoderParameterList (image, &icoEncoderClsid, 0, &buffer);
	ASSERT_EQ (FileNotFound, status);

	status = GdipGetEncoderParameterList (image, &wmfEncoderClsid, 0, &buffer);
	ASSERT_EQ (FileNotFound, status);

	status = GdipGetEncoderParameterList (image, &emfEncoderClsid, 0, &buffer);
	ASSERT_EQ (FileNotFound, status);

	GdipDisposeImage (image);

	SHUTDOWN
}

TEST(Image, GdipImageGetFrameDimensionsCount)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT count;

	status = GdipImageGetFrameDimensionsCount (bitmapImage, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	status = GdipImageGetFrameDimensionsCount (metafileImage, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	// Negative tests.
	status = GdipImageGetFrameDimensionsCount (NULL, &count);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageGetFrameDimensionsCount (metafileImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipImageGetFrameDimensionsList)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GUID dimensions[1];

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, 1);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipImageGetFrameDimensionsList (NULL, dimensions, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageGetFrameDimensionsList (bitmapImage, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, 0);
	ASSERT_EQ (Win32Error, status);

	status = GdipImageGetFrameDimensionsList (metafileImage, dimensions, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, -1);
	ASSERT_EQ (Win32Error, status);

	status = GdipImageGetFrameDimensionsList (metafileImage, dimensions, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageGetFrameDimensionsList (bitmapImage, dimensions, 2);
	ASSERT_EQ (Win32Error, status);

	status = GdipImageGetFrameDimensionsList (metafileImage, dimensions, 2);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipImageGetFrameCount)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GUID pageDimension = {0x7462dc86, 0x6180, 0x4c7e, {0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83}};
	GUID timeDimension = {0x6aedbd6d, 0x3fb5, 0x418a, {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};
	UINT count;

	// Bitmap - page dimension.
	status = GdipImageGetFrameCount (bitmapImage, &pageDimension, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	// Metafile - null dimension.
	count = -1;
	status = GdipImageGetFrameCount (metafileImage, NULL, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	// Metafile - no such dimension.
	count = -1;
	status = GdipImageGetFrameCount (metafileImage, &emfEncoderClsid, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	// Negative tests.
	status = GdipImageGetFrameCount (NULL, &pageDimension, &count);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageGetFrameCount (bitmapImage, NULL, &count);
	ASSERT_EQ (Win32Error, status);

	status = GdipImageGetFrameCount (bitmapImage, &pageDimension, NULL);
	ASSERT_EQ (Win32Error, status);

	status = GdipImageGetFrameCount (metafileImage, &pageDimension, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageGetFrameCount (bitmapImage, &timeDimension, &count);
	ASSERT_EQ (Win32Error, status);

	status = GdipImageGetFrameCount (bitmapImage, &emfEncoderClsid, &count);
	ASSERT_EQ (Win32Error, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipImageSelectActiveFrame)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	GUID pageDimension = {0x7462dc86, 0x6180, 0x4c7e, {0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83}};
	GUID timeDimension = {0x6aedbd6d, 0x3fb5, 0x418a, {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};

	// Bitmap - page dimension.
	status = GdipImageSelectActiveFrame (bitmapImage, &pageDimension, 0);
	ASSERT_EQ (Ok, status);

	// Metafile - page dimension.
	status = GdipImageSelectActiveFrame (metafileImage, &pageDimension, 100);
	ASSERT_EQ (Ok, status);

	// Metafile - time dimension.
	status = GdipImageSelectActiveFrame (metafileImage, &timeDimension, 100);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipImageSelectActiveFrame (NULL, &pageDimension, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageSelectActiveFrame (bitmapImage, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageSelectActiveFrame (metafileImage, NULL, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageSelectActiveFrame (bitmapImage, &pageDimension, 4);
	ASSERT_EQ (Win32Error, status);

	status = GdipImageSelectActiveFrame (metafileImage, &pageDimension, 200);
	ASSERT_EQ (Ok, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipImageForceValidation)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");

	// ImageTypeBitmap.
	status = GdipImageForceValidation (bitmapImage);
	ASSERT_EQ (Ok, status);

	// ImageTypeMetafile.
	status = GdipImageForceValidation (metafileImage);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipImageForceValidation (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipImageRotateFlip)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");

	// RotateNoneFlipNone.
	status = GdipImageRotateFlip (bitmapImage, RotateNoneFlipNone);
	ASSERT_EQ (Ok, status);

	// Rotate90FlipNone.
	status = GdipImageRotateFlip (bitmapImage, Rotate90FlipNone);
	ASSERT_EQ (Ok, status);

	// Rotate180FlipNone.
	status = GdipImageRotateFlip (bitmapImage, Rotate180FlipNone);
	ASSERT_EQ (Ok, status);

	// Rotate270FlipNone.
	status = GdipImageRotateFlip (bitmapImage, Rotate270FlipNone);
	ASSERT_EQ (Ok, status);

	// RotateNoneFlipX.
	status = GdipImageRotateFlip (bitmapImage, RotateNoneFlipX);
	ASSERT_EQ (Ok, status);

	// Rotate90FlipX.
	status = GdipImageRotateFlip (bitmapImage, Rotate90FlipX);
	ASSERT_EQ (Ok, status);

	// Rotate180FlipX.
	status = GdipImageRotateFlip (bitmapImage, Rotate180FlipX);
	ASSERT_EQ (Ok, status);

	// Rotate270FlipX.
	status = GdipImageRotateFlip (bitmapImage, Rotate270FlipX);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipImageRotateFlip (NULL, RotateNoneFlipNone);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageRotateFlip (metafileImage, Rotate270FlipX);
	ASSERT_EQ (NotImplemented, status);

	status = GdipImageRotateFlip (bitmapImage, (RotateFlipType)(RotateNoneFlipNone - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipImageRotateFlip (bitmapImage, (RotateFlipType)(Rotate270FlipX + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetImagePalette)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, palette->Count);

	// Empty palette - larger size.
	palette->Count = 100;
	status = GdipGetImagePalette (bitmapImage, palette, size + 1);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, palette->Count);

	// Empty palette - negative size.
	palette->Count = 100;
	status = GdipGetImagePalette (bitmapImage, palette, -1);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, palette->Count);

	// Negative tests.
	status = GdipGetImagePalette (NULL, palette, size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePalette (bitmapImage, NULL, size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePalette (metafileImage, NULL, size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePalette (metafileImage, palette, size);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetImagePalette (metafileImage, palette, -1);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetImagePalette (bitmapImage, palette, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePalette (metafileImage, palette, 0);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetImagePalette (bitmapImage, palette, size - 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePalette (metafileImage, palette, size - 1);
	ASSERT_EQ (NotImplemented, status);

	// Non empty palette - setup.
	nonEmptyPalette->Count = 10;
	nonEmptyPalette->Flags = 1;
	nonEmptyPalette->Entries[0] = 2;
	status = GdipSetImagePalette (bitmapImage, nonEmptyPalette);
	ASSERT_EQ (Ok, status);

	GdipGetImagePaletteSize (bitmapImage, &size);

	// Non empty palette - same size.
	palette->Count = 100;
	palette->Flags = 100;
	palette->Entries[0] = 100;

	status = GdipGetImagePalette (bitmapImage, palette, size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, palette->Count);
	ASSERT_EQ (1, palette->Flags);
	ASSERT_EQ (2, palette->Entries[0]);

	// Non empty palette - larger size.
	palette->Count = 100;
	palette->Flags = 100;
	palette->Entries[0] = 100;

	status = GdipGetImagePalette (bitmapImage, palette, size + 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePalette (bitmapImage, palette, size - 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipSetImagePalette)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetImagePaletteSize (bitmapImage, &size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (48, size);

	status = GdipGetImagePalette (bitmapImage, resultPalette, size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, resultPalette->Count);
	ASSERT_EQ (1, resultPalette->Flags);
	ASSERT_EQ (2, resultPalette->Entries[0]);

	// Set with large count.
	palette->Count = 256;
	palette->Flags = 1;
	palette->Entries[0] = 20;
	status = GdipSetImagePalette (bitmapImage, palette);
	ASSERT_EQ (Ok, status);

	status = GdipGetImagePaletteSize (bitmapImage, &size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1032, size);

	status = GdipGetImagePalette (bitmapImage, resultPalette, size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (256, resultPalette->Count);
	ASSERT_EQ (1, resultPalette->Flags);
	ASSERT_EQ (20, resultPalette->Entries[0]);

	// Negative tests.
	status = GdipSetImagePalette (NULL, palette);
	ASSERT_EQ (InvalidParameter, status);

	palette->Count = 10;
	status = GdipSetImagePalette (metafileImage, palette);
	ASSERT_EQ (NotImplemented, status);

	palette->Count = 0;
	status = GdipSetImagePalette (bitmapImage, palette);
	ASSERT_EQ (InvalidParameter, status);

	palette->Count = -1;
	status = GdipSetImagePalette (bitmapImage, palette);
	ASSERT_EQ (InvalidParameter, status);

	palette->Count = 257;
	status = GdipSetImagePalette (bitmapImage, palette);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImagePalette (bitmapImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImagePalette (metafileImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetImagePaletteSize)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	INT size;

	status = GdipGetImagePaletteSize (bitmapImage, &size);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (12, (int) sizeof(ColorPalette));
	ASSERT_EQ (12, size);

	// Negative tests.
	status = GdipGetImagePaletteSize (NULL, &size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePaletteSize (bitmapImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImagePaletteSize (metafileImage, &size);
	ASSERT_EQ (GenericError, status);

	status = GdipGetImagePaletteSize (metafileImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetPropertyCount)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT count;

	// ImageTypeBitmap.
	status = GdipGetPropertyCount (bitmapImage, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, count);

	// ImageTypeMetafile.
	status = GdipGetPropertyCount (metafileImage, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, count);

	// Negative tests.
	status = GdipGetPropertyCount (NULL, &count);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyCount (bitmapImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetPropertyIdList)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID list[2];

	// ImageTypeBitmap.
	status = GdipGetPropertyIdList (bitmapImage, 0, list);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipGetPropertyIdList (NULL, 0, list);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyIdList (metafileImage, 0, list);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetPropertyIdList (bitmapImage, 0, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyIdList (metafileImage, 0, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyIdList (bitmapImage, 1, list);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyIdList (metafileImage, 1, list);
	ASSERT_EQ (NotImplemented, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetPropertyItemSize)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID prop = 100;
	UINT size;

	// Negative tests.
	status = GdipGetPropertyItemSize (NULL, prop, &size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyItemSize (bitmapImage, prop, &size);
	ASSERT_EQ (PropertyNotFound, status);

	status = GdipGetPropertyItemSize (metafileImage, prop, &size);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetPropertyItemSize (bitmapImage, prop, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyItemSize (metafileImage, prop, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetPropertyItem)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID prop = 100;
	PropertyItem propertyItem;

	// Negative tests.
	status = GdipGetPropertyItem (NULL, prop, 1, &propertyItem);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyItem (bitmapImage, prop, 1, &propertyItem);
	ASSERT_EQ (PropertyNotFound, status);

	status = GdipGetPropertyItem (metafileImage, prop, 1, &propertyItem);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetPropertyItem (bitmapImage, prop, 1, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertyItem (metafileImage, prop, 1, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetPropertySize)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT totalBufferSize;
	UINT numProperties;

	status = GdipGetPropertySize (bitmapImage, &totalBufferSize, &numProperties);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, totalBufferSize);
	ASSERT_EQ (0, numProperties);

	// Negative tests.
	status = GdipGetPropertySize (NULL, &totalBufferSize, &numProperties);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertySize (metafileImage, &totalBufferSize, &numProperties);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetPropertySize (bitmapImage, NULL, &numProperties);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertySize (metafileImage, NULL, &numProperties);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertySize (bitmapImage, &totalBufferSize, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetPropertySize (metafileImage, &totalBufferSize, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipGetAllPropertyItems)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PropertyItem propertyItems[2];

	// Negative tests.
	status = GdipGetAllPropertyItems (NULL, 0, 0, propertyItems);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAllPropertyItems (bitmapImage, 0, 0, propertyItems);
	ASSERT_EQ (GenericError, status);

	status = GdipGetAllPropertyItems (metafileImage, 0, 0, propertyItems);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetAllPropertyItems (bitmapImage, 0, 0, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAllPropertyItems (metafileImage, 0, 0, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAllPropertyItems (bitmapImage, 1, 0, propertyItems);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAllPropertyItems (metafileImage, 1, 0, propertyItems);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetAllPropertyItems (bitmapImage, 0, 1, propertyItems);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetAllPropertyItems (metafileImage, 0, 1, propertyItems);
	ASSERT_EQ (NotImplemented, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}

TEST(Image, GdipRemovePropertyItem)
{
	STARTUP

	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID prop = 100;

	// Negative tests.
	status = GdipRemovePropertyItem (NULL, prop);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipRemovePropertyItem (bitmapImage, prop);
	ASSERT_EQ (GenericError, status);

	status = GdipRemovePropertyItem (metafileImage, prop);
	ASSERT_EQ (NotImplemented, status);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
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
	ASSERT_EQ (Ok, status);

	status = GdipGetPropertyItemSize (image, propertyItem1.id, &propertySize);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((int) sizeof(PropertyItem), propertySize);

	status = GdipGetPropertyItem (image, propertyItem1.id, propertySize, &resultPropertyItem);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, resultPropertyItem.id);
	ASSERT_EQ (0, resultPropertyItem.length);
	ASSERT_EQ (11, resultPropertyItem.type);

	status = GdipGetPropertyCount (image, &numProperties);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, numProperties);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((int) sizeof(PropertyItem), totalBufferSize);
	ASSERT_EQ (1, numProperties);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, propertyIds[0]);
	ASSERT_EQ (-1, propertyIds[1]);

	// Set another new property.
	status = GdipSetPropertyItem (image, &propertyItem2);
	ASSERT_EQ (Ok, status);

	status = GdipGetPropertyItemSize (image, propertyItem2.id, &propertySize);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((int) sizeof(PropertyItem), propertySize);

	status = GdipGetPropertyItem (image, propertyItem2.id, propertySize, &resultPropertyItem);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (11, resultPropertyItem.id);
	ASSERT_EQ (0, resultPropertyItem.length);
	ASSERT_EQ (12, resultPropertyItem.type);

	status = GdipGetPropertyCount (image, &numProperties);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, numProperties);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((int) sizeof(PropertyItem) * 2, totalBufferSize);
	ASSERT_EQ (2, numProperties);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, propertyIds[0]);
	ASSERT_EQ (11, propertyIds[1]);

	// Override an existing property.
	status = GdipSetPropertyItem (image, &propertyItem3);
	ASSERT_EQ (Ok, status);

	status = GdipGetPropertyItemSize (image, propertyItem3.id, &propertySize);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((int) sizeof(PropertyItem), propertySize);

	status = GdipGetPropertyItem (image, propertyItem3.id, propertySize, &resultPropertyItem);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, resultPropertyItem.id);
	ASSERT_EQ (0, resultPropertyItem.length);
	ASSERT_EQ (9, resultPropertyItem.type);

	status = GdipGetPropertyCount (image, &numProperties);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2, numProperties);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ ((int) sizeof(PropertyItem) * 2, totalBufferSize);
	ASSERT_EQ (2, numProperties);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (10, propertyIds[0]);
	ASSERT_EQ (11, propertyIds[1]);
}

TEST(Image, GdipSetPropertyItem)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipSetPropertyItem (gifImage, &propertyItem);
	ASSERT_EQ (Ok, status);

	status = GdipSetPropertyItem (pngImage, &propertyItem);
	ASSERT_EQ (Ok, status);

	status = GdipSetPropertyItem (jpgImage, &propertyItem);
	ASSERT_EQ (Ok, status);

	status = GdipSetPropertyItem (icoImage, &propertyItem);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetPropertyItem (NULL, &propertyItem);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPropertyItem (metafileImage, &propertyItem);
	ASSERT_EQ (NotImplemented, status);

	status = GdipSetPropertyItem (bmpImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetPropertyItem (metafileImage, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImage (bmpImage);
	GdipDisposeImage (tifImage);
	GdipDisposeImage (gifImage);
	GdipDisposeImage (pngImage);
	GdipDisposeImage (jpgImage);
	GdipDisposeImage (icoImage);
	GdipDisposeImage (metafileImage);

	SHUTDOWN
}
