#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#define GDIPVER 0x0110

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

static const char *file = "temp_img.png";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'i', 'm', 'g', '.', 'p', 'n', 'g', 0};
GpImage *image;

#define createFile(buffer, expectedStatus) \
{ \
	GpStatus status; \
	FILE *f = fopen (file, "wb+"); \
	assert (f); \
	fwrite ((void *) buffer, sizeof (BYTE), sizeof (buffer), f); \
	fclose (f); \
 \
	status = GdipLoadImageFromFile (wFile, &image); \
	assertEqualInt (status, expectedStatus); \
}

static GpImage* getImageImpl (const char* fileName, const char* message, const char* file, const char* function, int line)
{
	GpStatus status;
	WCHAR *wFileName = wcharFromChar (fileName);
	GpImage *image;

	status = GdipLoadImageFromFile (wFileName, &image);
	assertEqualIntImpl (status, Ok, message, file, function, line);

	freeWchar (wFileName);

	return image;
}

#define getImage(fileName) getImageImpl (fileName, NULL, __FILE__, __func__, __LINE__)

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

#if !defined(USE_WINDOWS_GDIPLUS)
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

static void test_loadImageFromFileTif ()
{
	GpImage *image = getImage ("test.tif");

	verifyBitmap (image, tifRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 19);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileGif ()
{
	GpImage *image = getImage ("test.gif");

	verifyBitmap (image, gifRawFormat, PixelFormat8bppIndexed, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 4);

	GdipDisposeImage (image);
}

static void test_loadImageFromFilePng ()
{
	GpStatus status;
	UINT numOfProperty;
	UINT size;
	PropertyItem *propertyItem = (PropertyItem *) malloc(1024);

	{
		GpImage *image = getImage ("test.png");

		verifyBitmap (image, pngRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5);

		status = GdipGetPropertyCount (image, &numOfProperty);
		assertEqualInt (status, Ok);
		assertEqualInt (numOfProperty, 5);

		PROPID *list = (PROPID *) malloc (numOfProperty * sizeof (PROPID));
		status = GdipGetPropertyIdList (image, numOfProperty, list);
		assertEqualInt (status, Ok);
		assertEqualInt (list[0], PropertyTagGamma);
		assertEqualInt (list[1], PropertyTagDateTime);
		assertEqualInt (list[2], PropertyTagPixelUnit);
		assertEqualInt (list[3], PropertyTagPixelPerUnitX);
		assertEqualInt (list[4], PropertyTagPixelPerUnitY);

		status = GdipGetPropertyItemSize (image, PropertyTagGamma, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 8);

		status = GdipGetPropertyItem (image, PropertyTagGamma, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagGamma);
		assertEqualInt (propertyItem->length, 8);
		assertEqualInt (propertyItem->type, PropertyTagTypeRational);
		// 2.2
		assertEqualInt (*((INT *)propertyItem->value), 100000);
		assertEqualInt (*((INT *)propertyItem->value + 1), 45455);

		status = GdipGetPropertyItemSize (image, PropertyTagDateTime, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 20);

		status = GdipGetPropertyItem (image, PropertyTagDateTime, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagDateTime);
		assertEqualInt (propertyItem->length, 20);
		assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
		assertEqualInt (strcmp("2004:06:11 05:44:09", (char *)propertyItem->value), 0);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelUnit, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 1);

		status = GdipGetPropertyItem (image, PropertyTagPixelUnit, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelUnit);
		assertEqualInt (propertyItem->length, 1);
		assertEqualInt (propertyItem->type, PropertyTagTypeByte);
		assertEqualInt (*((BYTE*) propertyItem->value), 1);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitX, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitX, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitX);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 2834);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitY, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitY, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitY);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 2834);

		GdipDisposeImage (image);
		free (list);
	}

	{
		GpImage *image = getImage ("test-gsa.png");

		verifyBitmap (image, pngRawFormat, PixelFormat32bppARGB, 16, 16, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsHasAlpha | ImageFlagsReadOnly, 3);

		status = GdipGetPropertyCount (image, &numOfProperty);
		assertEqualInt (status, Ok);
		assertEqualInt (numOfProperty, 3);

		PROPID *list = (PROPID *) malloc (numOfProperty * sizeof (PROPID));
		status = GdipGetPropertyIdList (image, numOfProperty, list);
		assertEqualInt (status, Ok);
		assertEqualInt (list[0], PropertyTagPixelUnit);
		assertEqualInt (list[1], PropertyTagPixelPerUnitX);
		assertEqualInt (list[2], PropertyTagPixelPerUnitY);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelUnit, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 1);

		status = GdipGetPropertyItem (image, PropertyTagPixelUnit, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelUnit);
		assertEqualInt (propertyItem->length, 1);
		assertEqualInt (propertyItem->type, PropertyTagTypeByte);
		assertEqualInt (*((BYTE*) propertyItem->value), 1);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitX, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitX, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitX);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitY, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitY, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitY);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		GdipDisposeImage (image);
		free (list);
	}

	{
		GpImage *image = getImage ("test-trns.png");

		verifyBitmap (image, pngRawFormat, WINDOWS_GDIPLUS ? PixelFormat32bppARGB : PixelFormat4bppIndexed, 24, 24, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsHasAlpha | ImageFlagsReadOnly, 3);

		status = GdipGetPropertyCount (image, &numOfProperty);
		assertEqualInt (status, Ok);
		assertEqualInt (numOfProperty, 3);

		PROPID *list = (PROPID *) malloc (numOfProperty * sizeof (PROPID));
		status = GdipGetPropertyIdList (image, numOfProperty, list);
		assertEqualInt (status, Ok);
		assertEqualInt (list[0], PropertyTagPixelUnit);
		assertEqualInt (list[1], PropertyTagPixelPerUnitX);
		assertEqualInt (list[2], PropertyTagPixelPerUnitY);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelUnit, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 1);

		status = GdipGetPropertyItem (image, PropertyTagPixelUnit, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelUnit);
		assertEqualInt (propertyItem->length, 1);
		assertEqualInt (propertyItem->type, PropertyTagTypeByte);
		assertEqualInt (*((BYTE*) propertyItem->value), 1);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitX, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitX, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitX);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitY, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitY, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitY);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		GdipDisposeImage (image);
		free (list);
	}

	{
		BYTE srgbChunk[] = {
			/* Signature */ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
			/* IHDR */      0x00, 0x00, 0x00, 0x0D, 'I', 'H', 'D', 'R', 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x04, 0x03, 0x00, 0x00, 0x01, 0x28, 0x2D, 0x63, 0xE6,
			/* sRGB */      0x00, 0x00, 0x00, 0x01, 's', 'R', 'G', 'B', 0x00, 0xAE, 0xCE, 0x1C, 0xE9,
			/* PLTE */      0x00, 0x00, 0x00, 0x0C, 'P', 'L', 'T', 'E', 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xC1, 0x7F, 0x62, 0xD1,
			/* IDAT */      0x00, 0x00, 0x00, 0x0C, 'I', 'D', 'A', 'T', 0x18, 0xD3, 0x63, 0xC0, 0x06, 0x18, 0x18, 0x00, 0x00, 0x17, 0x00, 0x01, 0x47, 0xB7, 0x91, 0x37,
			/* IEND */      0x00, 0x00, 0x00, 0x00, 'I', 'E', 'N', 'D', 0xAE, 0x42, 0x60, 0x82
		};

		createFile (srgbChunk, Ok);
		verifyBitmap (image, pngRawFormat, PixelFormat4bppIndexed, 6, 4, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5);

		status = GdipGetPropertyCount (image, &numOfProperty);
		assertEqualInt (status, Ok);
		assertEqualInt (numOfProperty, 5);

		PROPID *list = (PROPID *) malloc (numOfProperty * sizeof (PROPID));
		status = GdipGetPropertyIdList (image, numOfProperty, list);
		assertEqualInt (status, Ok);
		assertEqualInt (list[0], PropertyTagSRGBRenderingIntent);
		assertEqualInt (list[1], PropertyTagGamma);
		assertEqualInt (list[2], PropertyTagPixelUnit);
		assertEqualInt (list[3], PropertyTagPixelPerUnitX);
		assertEqualInt (list[4], PropertyTagPixelPerUnitY);

		status = GdipGetPropertyItemSize (image, PropertyTagSRGBRenderingIntent, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 1);

		status = GdipGetPropertyItem (image, PropertyTagSRGBRenderingIntent, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagSRGBRenderingIntent);
		assertEqualInt (propertyItem->length, 1);
		assertEqualInt (propertyItem->type, PropertyTagTypeByte);
		assertEqualInt (*((BYTE *)propertyItem->value), 0);

		status = GdipGetPropertyItemSize (image, PropertyTagGamma, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 8);

		status = GdipGetPropertyItem (image, PropertyTagGamma, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagGamma);
		assertEqualInt (propertyItem->length, 8);
		assertEqualInt (propertyItem->type, PropertyTagTypeRational);
		// 2.2
		assertEqualInt (*((INT *)propertyItem->value), 100000);
		assertEqualInt (*((INT *)propertyItem->value + 1), 45455);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelUnit, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 1);

		status = GdipGetPropertyItem (image, PropertyTagPixelUnit, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelUnit);
		assertEqualInt (propertyItem->length, 1);
		assertEqualInt (propertyItem->type, PropertyTagTypeByte);
		assertEqualInt (*((BYTE*) propertyItem->value), 1);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitX, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitX, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitX);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitY, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitY, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitY);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		GdipDisposeImage (image);
		free (list);
		deleteFile (file);
	}

	// GDI+ reads this as sRGB anyway.
#if defined(USE_WINDOWS_GDIPLUS)
	{
		BYTE invalidSrgbChunk[] = {
			/* Signature */ 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
			/* IHDR */      0x00, 0x00, 0x00, 0x0D, 'I', 'H', 'D', 'R', 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x04, 0x04, 0x03, 0x00, 0x00, 0x01, 0x28, 0x2D, 0x63, 0xE6,
			/* sRGB */      0x00, 0x00, 0x00, 0x01, 's', 'R', 'G', 'B', 0xFF, 0xAE, 0xCE, 0x1C, 0xE9,
			/* PLTE */      0x00, 0x00, 0x00, 0x0C, 'P', 'L', 'T', 'E', 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xC1, 0x7F, 0x62, 0xD1,
			/* IDAT */      0x00, 0x00, 0x00, 0x0C, 'I', 'D', 'A', 'T', 0x18, 0xD3, 0x63, 0xC0, 0x06, 0x18, 0x18, 0x00, 0x00, 0x17, 0x00, 0x01, 0x47, 0xB7, 0x91, 0x37,
			/* IEND */      0x00, 0x00, 0x00, 0x00, 'I', 'E', 'N', 'D', 0xAE, 0x42, 0x60, 0x82
		};
		createFile (invalidSrgbChunk, Ok);
		verifyBitmap (image, pngRawFormat, PixelFormat4bppIndexed, 6, 4, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 5);

		status = GdipGetPropertyCount (image, &numOfProperty);
		assertEqualInt (status, Ok);
		assertEqualInt (numOfProperty, 5);

		PROPID *list = (PROPID *) malloc (numOfProperty * sizeof (PROPID));
		status = GdipGetPropertyIdList (image, numOfProperty, list);
		assertEqualInt (status, Ok);
		assertEqualInt (list[0], PropertyTagSRGBRenderingIntent);
		assertEqualInt (list[1], PropertyTagGamma);
		assertEqualInt (list[2], PropertyTagPixelUnit);
		assertEqualInt (list[3], PropertyTagPixelPerUnitX);
		assertEqualInt (list[4], PropertyTagPixelPerUnitY);

		status = GdipGetPropertyItemSize (image, PropertyTagSRGBRenderingIntent, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 1);

		status = GdipGetPropertyItem (image, PropertyTagSRGBRenderingIntent, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagSRGBRenderingIntent);
		assertEqualInt (propertyItem->length, 1);
		assertEqualInt (propertyItem->type, PropertyTagTypeByte);
		assertEqualInt (*((BYTE *)propertyItem->value), 0);

		status = GdipGetPropertyItemSize (image, PropertyTagGamma, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 8);

		status = GdipGetPropertyItem (image, PropertyTagGamma, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagGamma);
		assertEqualInt (propertyItem->length, 8);
		assertEqualInt (propertyItem->type, PropertyTagTypeRational);
		// 2.2
		assertEqualInt (*((INT *)propertyItem->value), 100000);
		assertEqualInt (*((INT *)propertyItem->value + 1), 45455);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelUnit, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 1);

		status = GdipGetPropertyItem (image, PropertyTagPixelUnit, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelUnit);
		assertEqualInt (propertyItem->length, 1);
		assertEqualInt (propertyItem->type, PropertyTagTypeByte);
		assertEqualInt (*((BYTE*) propertyItem->value), 1);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitX, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitX, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitX);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		status = GdipGetPropertyItemSize (image, PropertyTagPixelPerUnitY, &size);
		assertEqualInt (status, Ok);
		assertEqualInt (size, sizeof (PropertyItem) + 4);

		status = GdipGetPropertyItem (image, PropertyTagPixelPerUnitY, size, propertyItem);
		assertEqualInt (status, Ok);
		assertEqualInt (propertyItem->id, PropertyTagPixelPerUnitY);
		assertEqualInt (propertyItem->length, 4);
		assertEqualInt (propertyItem->type, PropertyTagTypeLong);
		assertEqualInt (*((LONG*) propertyItem->value), 0);

		GdipDisposeImage (image);
		free (list);
		deleteFile (file);
	}
#endif

	free (propertyItem);
}

static void test_loadImageFromFileJpg ()
{
	GpImage *image = getImage ("test.jpg");

	verifyBitmap (image, jpegRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 2);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileIcon ()
{
	GpImage *image = getImage ("test.ico");

	verifyBitmap (image, icoRawFormat, PixelFormat32bppARGB, 48, 48, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsHasAlpha | ImageFlagsReadOnly, 0);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileWmf ()
{
	GpImage *image = getImage ("test.wmf");

	verifyMetafile (image, wmfRawFormat, -4008, -3378, 8016, 6756, 20360.638672f, 17160.2383f);

	GdipDisposeImage (image);
}

static void test_loadImageFromFileEmf ()
{
	GpImage *image = getImage ("test.emf");

	verifyMetafile (image, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);

	GdipDisposeImage (image);
}

static void test_cloneImage ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpegImage = getImage ("test.jpg");
	GpImage *jpegImageWithProperties = getImage ("nature24bits.jpg");
	GpImage *memoryImage;
	GdipCreateBitmapFromScan0 (1, 1, 0, PixelFormat32bppARGB, NULL, (GpBitmap **) &memoryImage);
	GpImage *metafileImage = getImage ("test.wmf");
	GpImage *clonedImage;
	UINT totalBufferSize;
	UINT numProperties;
	PropertyItem *propertyItems = (PropertyItem *) malloc (1024);

	// ImageTypeBitmap - bmp.
	status = GdipCloneImage (bitmapImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != bitmapImage);
	verifyBitmap (clonedImage, bmpRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 0);
	GdipDisposeImage (clonedImage);

	// ImageTypeBitmap - jpg.
	status = GdipCloneImage (jpegImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != jpegImage);
	verifyBitmap (clonedImage, jpegRawFormat, PixelFormat24bppRGB, 100, 68, ImageFlagsColorSpaceRGB | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 2);
	GdipDisposeImage (clonedImage);

	// ImageTypeBitmap - jpg with properties.
	status = GdipCloneImage (jpegImageWithProperties, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != jpegImageWithProperties);
	verifyBitmap (clonedImage, jpegRawFormat, PixelFormat24bppRGB, 110, 100, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 3);

	status = GdipGetPropertySize (jpegImageWithProperties, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 3 + 29 + 128 + 128);
	assertEqualInt (numProperties, 3);

	status = GdipGetAllPropertyItems (jpegImageWithProperties, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	PropertyItem *propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 29);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assertEqualInt (strcmp ("LEAD Technologies Inc. V1.01", (char *) propertyItem->value), 0);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected1[] = {
		0x16, 0x00, 0x17, 0x00, 0x1F, 0x00, 0x3E, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x17,
		0x00, 0x1B, 0x00, 0x22, 0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x1F,
		0x00, 0x22, 0x00, 0x49, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x3E,
		0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected2[] = {
		0x15, 0x00, 0x0E, 0x00, 0x0D, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x43, 0x00, 0x50, 0x00, 0x0F,
		0x00, 0x0F, 0x00, 0x12, 0x00, 0x19, 0x00, 0x22, 0x00, 0x4C, 0x00, 0x4F, 0x00, 0x48, 0x00, 0x12,
		0x00, 0x11, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4B, 0x00, 0x5B, 0x00, 0x49, 0x00, 0x12,
		0x00, 0x16, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x43, 0x00, 0x72, 0x00, 0x69, 0x00, 0x51, 0x00, 0x17,
		0x00, 0x1D, 0x00, 0x30, 0x00, 0x49, 0x00, 0x59, 0x00, 0x8F, 0x00, 0x87, 0x00, 0x65, 0x00, 0x1F,
		0x00, 0x2E, 0x00, 0x48, 0x00, 0x54, 0x00, 0x6A, 0x00, 0x89, 0x00, 0x95, 0x00, 0x79, 0x00, 0x40,
		0x00, 0x54, 0x00, 0x66, 0x00, 0x72, 0x00, 0x87, 0x00, 0x9F, 0x00, 0x9E, 0x00, 0x85, 0x00, 0x5F,
		0x00, 0x79, 0x00, 0x7D, 0x00, 0x81, 0x00, 0x93, 0x00, 0x84, 0x00, 0x87, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);
	GdipDisposeImage (clonedImage);

	// ImageTypeBitmap - memory bitmap.
	status = GdipCloneImage (memoryImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != memoryImage);
	verifyBitmap (clonedImage, memoryBmpRawFormat, PixelFormat32bppARGB, 1, 1, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (clonedImage);

	// ImageTypeMetafile.
	status = GdipCloneImage (metafileImage, &clonedImage);
	assertEqualInt (status, Ok);
	assert (clonedImage && clonedImage != metafileImage);
	verifyMetafile (clonedImage, wmfRawFormat, -4008, -3378, 8016, 6756, 20360.638672f, 17160.238281f);
	GdipDisposeImage (clonedImage);

	// Negative tests.
	status = GdipCloneImage (NULL, &clonedImage);
	assertEqualInt (status, InvalidParameter);

	status = GdipCloneImage (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (jpegImageWithProperties);
	GdipDisposeImage (memoryImage);
	GdipDisposeImage (metafileImage);
	free (propertyItems);
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
	GpImage *wmfImage = getImage ("test.wmf");
	GpImage *emfImage = getImage ("test.emf");
	GpImage *thumbImage;

	// ImageTypeBitmap - non zero width and height.
	status = GdipGetImageThumbnail (bitmapImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 10, 10, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - width > height.
	status = GdipGetImageThumbnail (bitmapImage, 20, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 20, 10, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - height > width.
	status = GdipGetImageThumbnail (bitmapImage, 10, 20, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 10, 20, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeBitmap - zero width and height.
	status = GdipGetImageThumbnail (bitmapImage, 0, 0, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppPARGB, 120, 120, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - non zero width and height.
	status = GdipGetImageThumbnail (wmfImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 10, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - width > height.
	status = GdipGetImageThumbnail (wmfImage, 20, 10, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 20, 10, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - height > width.
	status = GdipGetImageThumbnail (wmfImage, 10, 20, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 20, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - non zero width and height.
	status = GdipGetImageThumbnail (emfImage, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 10, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - width > height.
	status = GdipGetImageThumbnail (emfImage, 20, 10, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 20, 10, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// ImageTypeMetafile - height > width.
	status = GdipGetImageThumbnail (emfImage, 10, 20, &thumbImage, NULL, NULL);
	assertEqualInt (status, Ok);
	verifyBitmap (thumbImage, memoryBmpRawFormat, PixelFormat32bppARGB, 10, 20, ImageFlagsHasAlpha, 0);
	GdipDisposeImage (thumbImage);

	// Negative tests.
	status = GdipGetImageThumbnail (NULL, 10, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageThumbnail (bitmapImage, 10, 10, NULL, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageThumbnail (bitmapImage, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetImageThumbnail (wmfImage, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetImageThumbnail (emfImage, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetImageThumbnail (NULL, 0, 10, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageThumbnail (bitmapImage, 0, 10, NULL, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageThumbnail (bitmapImage, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetImageThumbnail (wmfImage, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetImageThumbnail (emfImage, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, OutOfMemory);

	status = GdipGetImageThumbnail (NULL, 10, 0, &thumbImage, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageThumbnail (bitmapImage, 10, 0, NULL, (GetThumbnailImageAbort) callback, (void *) 1);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (wmfImage);
	GdipDisposeImage (emfImage);
}

static void test_getEncoderParameterListSize ()
{
	GpStatus status;
	GpImage *image = getImage ("test.bmp");
	UINT size;

	status = GdipGetEncoderParameterListSize (image, &bmpEncoderClsid, &size);
	assertEqualInt (status, NotImplemented);

	status = GdipGetEncoderParameterListSize (image, &tifEncoderClsid, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, (is_32bit() ? 164 : 184));

	status = GdipGetEncoderParameterListSize (image, &gifEncoderClsid, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, (is_32bit() ? 64 : 80));

	status = GdipGetEncoderParameterListSize (image, &pngEncoderClsid, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, (is_32bit() ? 32 : 40));

	status = GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, (is_32bit() ? 172 : 200));

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
	assertEqualInt (status, Ok);
	assertEqualInt (parameters->Count, 4);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &compression, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[0].NumberOfValues, 5);
	assertEqualInt (parameters->Parameter[0].Type, EncoderParameterValueTypeLong);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[0], EncoderValueCompressionLZW);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[1], EncoderValueCompressionCCITT3);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[2], EncoderValueCompressionRle);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[3], EncoderValueCompressionCCITT4);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[4], EncoderValueCompressionNone);

	assert (memcmp ((void *) &parameters->Parameter[1].Guid, (void *) &colorDepth, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[1].NumberOfValues, 5);
	assertEqualInt (parameters->Parameter[1].Type, EncoderParameterValueTypeLong);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[0], 1);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[1], 4);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[2], 8);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[3], 24);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[4], 32);

	assert (memcmp ((void *) &parameters->Parameter[2].Guid, (void *) &saveFlag, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[2].NumberOfValues, 1);
	assertEqualInt (parameters->Parameter[2].Type, EncoderParameterValueTypeLong);
	assertEqualInt (((LONG *) parameters->Parameter[2].Value)[0], EncoderValueMultiFrame);

	assert (memcmp ((void *) &parameters->Parameter[3].Guid, (void *) &saveAsCMYK, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[3].NumberOfValues, 1);
	assertEqualInt (parameters->Parameter[3].Type, EncoderParameterValueTypeLong);
	assertEqualInt (((LONG *) parameters->Parameter[3].Value)[0], 1);

	free (parameters);

	// GIF encoder.
	GdipGetEncoderParameterListSize (image, &gifEncoderClsid, &gifSize);
	parameters = (EncoderParameters *) malloc (gifSize);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, gifSize, parameters);
	assertEqualInt (status, Ok);
	assertEqualInt (parameters->Count, 2);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &imageItems, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[0].NumberOfValues, 0);
	assertEqualInt (parameters->Parameter[0].Type, (EncoderParameterValueType) 9);
	assert (!parameters->Parameter[0].Value);

	assert (memcmp ((void *) &parameters->Parameter[1].Guid, (void *) &saveFlag, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[1].NumberOfValues, 1);
	assertEqualInt (parameters->Parameter[1].Type, EncoderParameterValueTypeLong);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[0], EncoderValueMultiFrame);

	free (parameters);

	// PNG encoder.
	GdipGetEncoderParameterListSize (image, &pngEncoderClsid, &pngSize);
	parameters = (EncoderParameters *) malloc (pngSize);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, pngSize, parameters);
	assertEqualInt (status, Ok);
	assertEqualInt (parameters->Count, 1);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &imageItems, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[0].NumberOfValues, 0);
	assertEqualInt (parameters->Parameter[0].Type, (EncoderParameterValueType) 9);
	assert (!parameters->Parameter[0].Value);

	free (parameters);

	// JPEG encoder.
	GdipGetEncoderParameterListSize (image, &jpegEncoderClsid, &jpegSize);
	parameters = (EncoderParameters *) malloc (jpegSize);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, jpegSize, parameters);
	assertEqualInt (status, Ok);
	assertEqualInt (parameters->Count, 5);

	assert (memcmp ((void *) &parameters->Parameter[0].Guid, (void *) &transformation, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[0].NumberOfValues, 5);
	assertEqualInt (parameters->Parameter[0].Type, EncoderParameterValueTypeLong);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[0], EncoderValueTransformRotate90);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[1], EncoderValueTransformRotate180);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[2], EncoderValueTransformRotate270);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[3], EncoderValueTransformFlipHorizontal);
	assertEqualInt (((LONG *) parameters->Parameter[0].Value)[4], EncoderValueTransformFlipVertical);

	assert (memcmp ((void *) &parameters->Parameter[1].Guid, (void *) &quality, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[1].NumberOfValues, 1);
	assertEqualInt (parameters->Parameter[1].Type, EncoderParameterValueTypeLongRange);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[0], 0);
	assertEqualInt (((LONG *) parameters->Parameter[1].Value)[1], 100);

	assert (memcmp ((void *) &parameters->Parameter[2].Guid, (void *) &luminanceTable, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[2].NumberOfValues, 0);
	assertEqualInt (parameters->Parameter[2].Type, EncoderParameterValueTypeShort);
	assert (!parameters->Parameter[2].Value);

	assert (memcmp ((void *) &parameters->Parameter[3].Guid, (void *) &chrominanceTable, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[3].NumberOfValues, 0);
	assertEqualInt (parameters->Parameter[3].Type, EncoderParameterValueTypeShort);
	assert (!parameters->Parameter[3].Value);

	assert (memcmp ((void *) &parameters->Parameter[4].Guid, (void *) &imageItems, sizeof (GUID)) == 0);
	assertEqualInt (parameters->Parameter[4].NumberOfValues, 0);
	assertEqualInt (parameters->Parameter[4].Type, (EncoderParameterValueType) 9);
	assert (!parameters->Parameter[4].Value);

	free (parameters);

	// Negative tests.
	status = GdipGetEncoderParameterList (NULL, &emfEncoderClsid, 100, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, NULL, 100, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &emfEncoderClsid, 100, NULL);
	assertEqualInt (status, FileNotFound);

	status = GdipGetEncoderParameterList (image, &tifEncoderClsid, 100, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, 100, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, 100, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, 100, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, 0, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, -1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &tifEncoderClsid, tiffSize - 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &tifEncoderClsid, tiffSize + 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, gifSize - 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &gifEncoderClsid, gifSize + 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, pngSize - 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &pngEncoderClsid, pngSize + 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, jpegSize - 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &jpegEncoderClsid, jpegSize + 1, &buffer);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEncoderParameterList (image, &bmpEncoderClsid, 0, &buffer);
	assertEqualInt (status, NotImplemented);

	status = GdipGetEncoderParameterList (image, &icoEncoderClsid, 0, &buffer);
	assertEqualInt (status, FileNotFound);

	status = GdipGetEncoderParameterList (image, &wmfEncoderClsid, 0, &buffer);
	assertEqualInt (status, FileNotFound);

	status = GdipGetEncoderParameterList (image, &emfEncoderClsid, 0, &buffer);
	assertEqualInt (status, FileNotFound);

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
	GpImage *jpegImage = getImage ("nature24bits.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT count;

	// ImageTypeBitmap.
	status = GdipGetPropertyCount (bitmapImage, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);

	status = GdipGetPropertyCount (jpegImage, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 3);

	// ImageTypeMetafile.
	status = GdipGetPropertyCount (metafileImage, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count, 0);

	// Negative tests.
	count = 0xCC;
	status = GdipGetPropertyCount (NULL, &count);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (0xCC, count);

	count = 0xCC;
	status = GdipGetPropertyCount (bitmapImage, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (0xCC, count);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertyIdList ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpegImage = getImage ("nature24bits.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID list[4] = {1, 2, 3, 4};

	// ImageTypeBitmap.
	status = GdipGetPropertyIdList (bitmapImage, 0, list);
	assertEqualInt (status, Ok);

	status = GdipGetPropertyIdList(jpegImage, 3, list);
	assertEqualInt (status, Ok);
	assertEqualInt (list[0], PropertyTagExifUserComment);
	assertEqualInt (list[1], PropertyTagChrominanceTable);
	assertEqualInt (list[2], PropertyTagLuminanceTable);
	assertEqualInt (list[3], 4);

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

	status = GdipGetPropertyIdList (jpegImage, 0, list);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyIdList (jpegImage, 1, list);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyIdList (jpegImage, 4, list);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyIdList (metafileImage, 1, list);
	assertEqualInt (status, NotImplemented);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertyItemSize ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpegImage = getImage ("nature24bits.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT size;

	status = GdipGetPropertyItemSize (jpegImage, PropertyTagExifUserComment, &size);
	assertEqualInt (size, sizeof (PropertyItem) + 29);

	status = GdipGetPropertyItemSize (jpegImage, PropertyTagChrominanceTable, &size);
	assertEqualInt (size, sizeof (PropertyItem) + 128);

	status = GdipGetPropertyItemSize (jpegImage, PropertyTagLuminanceTable, &size);
	assertEqualInt (size, sizeof (PropertyItem) + 128);

	// Negative tests.
	size = 0xCC;
	status = GdipGetPropertyItemSize (NULL, 0, &size);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (size, 0xCC);

	size = 0xCC;
	status = GdipGetPropertyItemSize (bitmapImage, 100, &size);
	assertEqualInt (status, PropertyNotFound);
	assertEqualInt (size, 0xCC);

	size = 0xCC;
	status = GdipGetPropertyItemSize (bitmapImage, 0, &size);
	assertEqualInt (status, PropertyNotFound);

	size = 0xCC;
	status = GdipGetPropertyItemSize (bitmapImage, -100, &size);
	assertEqualInt (status, PropertyNotFound);
	assertEqualInt (size, 0xCC);

	size = 0xCC;
	status = GdipGetPropertyItemSize (metafileImage, 100, &size);
	assertEqualInt (status, NotImplemented);
	assertEqualInt (size, 0xCC);

	size = 0xCC;
	status = GdipGetPropertyItemSize (metafileImage, 0, &size);
	assertEqualInt (status, NotImplemented);
	assertEqualInt (size, 0xCC);

	size = 0xCC;
	status = GdipGetPropertyItemSize (metafileImage, -100, &size);
	assertEqualInt (status, NotImplemented);
	assertEqualInt (size, 0xCC);

	status = GdipGetPropertyItemSize (bitmapImage, 0, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyItemSize (metafileImage, 0, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (metafileImage);
}

static void test_getPropertyItem ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpegImage = getImage ("nature24bits.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	PropertyItem *propertyItem = (PropertyItem *)malloc(1024);
	UINT size;

	status = GdipGetPropertyItemSize (jpegImage, PropertyTagExifUserComment, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, sizeof (PropertyItem) + 29);

	status = GdipGetPropertyItem (jpegImage, PropertyTagExifUserComment, size, propertyItem);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 29);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assertEqualInt (strcmp ("LEAD Technologies Inc. V1.01", (char *) propertyItem->value), 0);

	status = GdipGetPropertyItemSize (jpegImage, PropertyTagChrominanceTable, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, sizeof (PropertyItem) + 128);

	status = GdipGetPropertyItem (jpegImage, PropertyTagChrominanceTable, size, propertyItem);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected1[] = {
		0x16, 0x00, 0x17, 0x00, 0x1F, 0x00, 0x3E, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x17,
		0x00, 0x1B, 0x00, 0x22, 0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x1F,
		0x00, 0x22, 0x00, 0x49, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x3E,
		0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);

	status = GdipGetPropertyItemSize (jpegImage, PropertyTagLuminanceTable, &size);
	assertEqualInt (status, Ok);
	assertEqualInt (size, sizeof (PropertyItem) + 128);

	status = GdipGetPropertyItem (jpegImage, PropertyTagLuminanceTable, size, propertyItem);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected2[] = {
		0x15, 0x00, 0x0E, 0x00, 0x0D, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x43, 0x00, 0x50, 0x00, 0x0F,
		0x00, 0x0F, 0x00, 0x12, 0x00, 0x19, 0x00, 0x22, 0x00, 0x4C, 0x00, 0x4F, 0x00, 0x48, 0x00, 0x12,
		0x00, 0x11, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4B, 0x00, 0x5B, 0x00, 0x49, 0x00, 0x12,
		0x00, 0x16, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x43, 0x00, 0x72, 0x00, 0x69, 0x00, 0x51, 0x00, 0x17,
		0x00, 0x1D, 0x00, 0x30, 0x00, 0x49, 0x00, 0x59, 0x00, 0x8F, 0x00, 0x87, 0x00, 0x65, 0x00, 0x1F,
		0x00, 0x2E, 0x00, 0x48, 0x00, 0x54, 0x00, 0x6A, 0x00, 0x89, 0x00, 0x95, 0x00, 0x79, 0x00, 0x40,
		0x00, 0x54, 0x00, 0x66, 0x00, 0x72, 0x00, 0x87, 0x00, 0x9F, 0x00, 0x9E, 0x00, 0x85, 0x00, 0x5F,
		0x00, 0x79, 0x00, 0x7D, 0x00, 0x81, 0x00, 0x93, 0x00, 0x84, 0x00, 0x87, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);

	// Negative tests.
	status = GdipGetPropertyItem (NULL, 100, 1, propertyItem);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyItem (bitmapImage, 100, 1, propertyItem);
	assertEqualInt (status, PropertyNotFound);

	status = GdipGetPropertyItem (bitmapImage, 0, 1, propertyItem);
	assertEqualInt (status, PropertyNotFound);

	status = GdipGetPropertyItem (bitmapImage, -100, 1, propertyItem);
	assertEqualInt (status, PropertyNotFound);

	status = GdipGetPropertyItem (metafileImage, 100, 1, propertyItem);
	assertEqualInt (status, NotImplemented);

	status = GdipGetPropertyItem (metafileImage, 0, 1, propertyItem);
	assertEqualInt (status, NotImplemented);

	status = GdipGetPropertyItem (metafileImage, -100, 1, propertyItem);
	assertEqualInt (status, NotImplemented);

	status = GdipGetPropertyItem (jpegImage, PropertyTagExifUserComment, -1, propertyItem);
	assertEqualInt (status, GenericError);

	status = GdipGetPropertyItem (jpegImage, PropertyTagExifUserComment, 0, propertyItem);
	assertEqualInt (status, GenericError);

	status = GdipGetPropertyItem (jpegImage, PropertyTagExifUserComment, 1, propertyItem);
	assertEqualInt (status, GenericError);

	status = GdipGetPropertyItem (jpegImage, PropertyTagExifUserComment, sizeof (PropertyItem) + 29 - 1, propertyItem);
	assertEqualInt (status, GenericError);

	status = GdipGetPropertyItem (jpegImage, PropertyTagExifUserComment, sizeof (PropertyItem) + 29 + 1, propertyItem);
	assertEqualInt (status, GenericError);

	status = GdipGetPropertyItem (bitmapImage, 100, 1, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetPropertyItem (metafileImage, 100, 1, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (metafileImage);
	free (propertyItem);
}

static void test_getPropertySize ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpegImage = getImage ("nature24bits.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	UINT totalBufferSize;
	UINT numProperties;

	status = GdipGetPropertySize (bitmapImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, 0);
	assertEqualInt (numProperties, 0);

	status = GdipGetPropertySize (jpegImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 3 + 29 + 128 + 128);
	assertEqualInt (numProperties, 3);

	// Negative tests.
	totalBufferSize = 0xCC;
	numProperties = 0xCC;
	status = GdipGetPropertySize (NULL, &totalBufferSize, &numProperties);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (totalBufferSize, 0xCC);
	assertEqualInt (numProperties, 0xCC);

	totalBufferSize = 0xCC;
	numProperties = 0xCC;
	status = GdipGetPropertySize (metafileImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, NotImplemented);
	assertEqualInt (totalBufferSize, 0xCC);
	assertEqualInt (numProperties, 0xCC);

	totalBufferSize = 0xCC;
	numProperties = 0xCC;
	status = GdipGetPropertySize (bitmapImage, NULL, &numProperties);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (totalBufferSize, 0xCC);
	assertEqualInt (numProperties, 0xCC);

	totalBufferSize = 0xCC;
	numProperties = 0xCC;
	status = GdipGetPropertySize (metafileImage, NULL, &numProperties);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (totalBufferSize, 0xCC);
	assertEqualInt (numProperties, 0xCC);

	totalBufferSize = 0xCC;
	numProperties = 0xCC;
	status = GdipGetPropertySize (bitmapImage, &totalBufferSize, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (totalBufferSize, 0xCC);
	assertEqualInt (numProperties, 0xCC);

	totalBufferSize = 0xCC;
	numProperties = 0xCC;
	status = GdipGetPropertySize (metafileImage, &totalBufferSize, NULL);
	assertEqualInt (status, InvalidParameter);
	assertEqualInt (totalBufferSize, 0xCC);
	assertEqualInt (numProperties, 0xCC);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (metafileImage);
}

static void test_getAllPropertyItems ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpegImage = getImage ("nature24bits.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	PropertyItem *propertyItems = (PropertyItem *) malloc (1024);
	UINT totalBufferSize;
	UINT numProperties;

	status = GdipGetPropertySize (jpegImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 3 + 29 + 128 + 128);
	assertEqualInt (numProperties, 3);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	PropertyItem *propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 29);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assertEqualInt (strcmp ("LEAD Technologies Inc. V1.01", (char *) propertyItem->value), 0);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected1[] = {
		0x16, 0x00, 0x17, 0x00, 0x1F, 0x00, 0x3E, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x17,
		0x00, 0x1B, 0x00, 0x22, 0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x1F,
		0x00, 0x22, 0x00, 0x49, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x3E,
		0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected2[] = {
		0x15, 0x00, 0x0E, 0x00, 0x0D, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x43, 0x00, 0x50, 0x00, 0x0F,
		0x00, 0x0F, 0x00, 0x12, 0x00, 0x19, 0x00, 0x22, 0x00, 0x4C, 0x00, 0x4F, 0x00, 0x48, 0x00, 0x12,
		0x00, 0x11, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4B, 0x00, 0x5B, 0x00, 0x49, 0x00, 0x12,
		0x00, 0x16, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x43, 0x00, 0x72, 0x00, 0x69, 0x00, 0x51, 0x00, 0x17,
		0x00, 0x1D, 0x00, 0x30, 0x00, 0x49, 0x00, 0x59, 0x00, 0x8F, 0x00, 0x87, 0x00, 0x65, 0x00, 0x1F,
		0x00, 0x2E, 0x00, 0x48, 0x00, 0x54, 0x00, 0x6A, 0x00, 0x89, 0x00, 0x95, 0x00, 0x79, 0x00, 0x40,
		0x00, 0x54, 0x00, 0x66, 0x00, 0x72, 0x00, 0x87, 0x00, 0x9F, 0x00, 0x9E, 0x00, 0x85, 0x00, 0x5F,
		0x00, 0x79, 0x00, 0x7D, 0x00, 0x81, 0x00, 0x93, 0x00, 0x84, 0x00, 0x87, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);

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

	status = GdipGetAllPropertyItems (jpegImage, -1, numProperties, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (jpegImage, 0, numProperties, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize - 1, numProperties, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize + 1, numProperties, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, -1, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, 0, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, numProperties - 1, propertyItems);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, numProperties + 1, propertyItems);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (metafileImage);
	GdipDisposeImage (jpegImage);
	free (propertyItems);
}

static void test_removePropertyItem ()
{
	GpStatus status;
	GpImage *bitmapImage = getImage ("test.bmp");
	GpImage *jpegImage = getImage ("nature24bits.jpg");
	GpImage *metafileImage = getImage ("test.wmf");
	PROPID propertyIds[2];
	UINT totalBufferSize;
	UINT numProperties;
	PropertyItem *propertyItems = (PropertyItem *) malloc (1024);

	// Remove middle.
	status = GdipRemovePropertyItem (jpegImage, PropertyTagChrominanceTable);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 2 + 29 + 128);
	assertEqualInt (numProperties, 2);

	status = GdipGetPropertyIdList (jpegImage, numProperties, propertyIds);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyIds[0], PropertyTagExifUserComment);
	assertEqualInt (propertyIds[1], PropertyTagLuminanceTable);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	PropertyItem *propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 29);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assertEqualInt (strcmp ("LEAD Technologies Inc. V1.01", (char *) propertyItem->value), 0);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected[] = {
		0x15, 0x00, 0x0E, 0x00, 0x0D, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x43, 0x00, 0x50, 0x00, 0x0F,
		0x00, 0x0F, 0x00, 0x12, 0x00, 0x19, 0x00, 0x22, 0x00, 0x4C, 0x00, 0x4F, 0x00, 0x48, 0x00, 0x12,
		0x00, 0x11, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4B, 0x00, 0x5B, 0x00, 0x49, 0x00, 0x12,
		0x00, 0x16, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x43, 0x00, 0x72, 0x00, 0x69, 0x00, 0x51, 0x00, 0x17,
		0x00, 0x1D, 0x00, 0x30, 0x00, 0x49, 0x00, 0x59, 0x00, 0x8F, 0x00, 0x87, 0x00, 0x65, 0x00, 0x1F,
		0x00, 0x2E, 0x00, 0x48, 0x00, 0x54, 0x00, 0x6A, 0x00, 0x89, 0x00, 0x95, 0x00, 0x79, 0x00, 0x40,
		0x00, 0x54, 0x00, 0x66, 0x00, 0x72, 0x00, 0x87, 0x00, 0x9F, 0x00, 0x9E, 0x00, 0x85, 0x00, 0x5F,
		0x00, 0x79, 0x00, 0x7D, 0x00, 0x81, 0x00, 0x93, 0x00, 0x84, 0x00, 0x87, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected, (BYTE *) propertyItem->value, propertyItem->length);

	status = GdipRemovePropertyItem (jpegImage, PropertyTagChrominanceTable);
	assertEqualInt (status, PropertyNotFound);

	status = GdipGetPropertyItem (jpegImage, PropertyTagChrominanceTable, sizeof (PropertyItem) + 128, propertyItems);
	assertEqualInt (status, PropertyNotFound);

	// Remove first.
	status = GdipRemovePropertyItem (jpegImage, PropertyTagExifUserComment);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) + 128);
	assertEqualInt (numProperties, 1);

	status = GdipGetPropertyIdList (jpegImage, numProperties, propertyIds);
	assertEqualInt (status, Ok);
	assertEqualInt (propertyIds[0], PropertyTagLuminanceTable);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	propertyItem = propertyItems;
	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected, (BYTE *) propertyItem->value, propertyItem->length);

	status = GdipRemovePropertyItem (jpegImage, PropertyTagExifUserComment);
	assertEqualInt (status, PropertyNotFound);

	status = GdipGetPropertyItem (jpegImage, PropertyTagExifUserComment, sizeof (PropertyItem) + 29, propertyItems);
	assertEqualInt (status, PropertyNotFound);

	// Remove last.
	status = GdipRemovePropertyItem (jpegImage, PropertyTagLuminanceTable);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImage, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, 0);
	assertEqualInt (numProperties, 0);

	status = GdipGetPropertyIdList (jpegImage, numProperties, propertyIds);
	assertEqualInt (status, Ok);

	status = GdipGetAllPropertyItems (jpegImage, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, GenericError);

	status = GdipRemovePropertyItem (jpegImage, PropertyTagLuminanceTable);
	assertEqualInt (status, GenericError);

	status = GdipGetPropertyItem (jpegImage, PropertyTagLuminanceTable, sizeof (PropertyItem) + 128, propertyItems);
	assertEqualInt (status, PropertyNotFound);

	// Negative tests.
	status = GdipRemovePropertyItem (NULL, 100);
	assertEqualInt (status, InvalidParameter);

	status = GdipRemovePropertyItem (bitmapImage, 100);
	assertEqualInt (status, GenericError);

	status = GdipRemovePropertyItem (bitmapImage, 0);
	assertEqualInt (status, GenericError);

	status = GdipRemovePropertyItem (bitmapImage, -100);
	assertEqualInt (status, GenericError);

	status = GdipRemovePropertyItem (metafileImage, 100);
	assertEqualInt (status, NotImplemented);

	status = GdipRemovePropertyItem (metafileImage, 0);
	assertEqualInt (status, NotImplemented);

	status = GdipRemovePropertyItem (metafileImage, -100);
	assertEqualInt (status, NotImplemented);

	GdipDisposeImage (bitmapImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (metafileImage);
	free (propertyItems);
}

static void setPropertyItemForImageImpl (GpImage *image, const char* message, const char* file, const char* function, int line)
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
	assertEqualIntImpl (status, Ok, message, file, function, line);

	status = GdipGetPropertyItemSize (image, propertyItem1.id, &propertySize);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (propertySize, (int) sizeof(PropertyItem), message, file, function, line);

	status = GdipGetPropertyItem (image, propertyItem1.id, propertySize, &resultPropertyItem);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.id, 10, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.length, 0, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.type, 11, message, file, function, line);

	status = GdipGetPropertyCount (image, &numProperties);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (numProperties, 1, message, file, function, line);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (totalBufferSize, (int) sizeof(PropertyItem), message, file, function, line);
	assertEqualIntImpl (numProperties, 1, message, file, function, line);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (propertyIds[0], 10, message, file, function, line);
	assertEqualIntImpl (propertyIds[1], -1, message, file, function, line);

	// Set another new property.
	status = GdipSetPropertyItem (image, &propertyItem2);
	assertEqualIntImpl (status, Ok, message, file, function, line);

	status = GdipGetPropertyItemSize (image, propertyItem2.id, &propertySize);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (propertySize, (int) sizeof(PropertyItem), message, file, function, line);

	status = GdipGetPropertyItem (image, propertyItem2.id, propertySize, &resultPropertyItem);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.id, 11, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.length, 0, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.type, 12, message, file, function, line);

	status = GdipGetPropertyCount (image, &numProperties);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (numProperties, 2, message, file, function, line);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (totalBufferSize, (int) sizeof(PropertyItem) * 2, message, file, function, line);
	assertEqualIntImpl (numProperties, 2, message, file, function, line);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (propertyIds[0], 10, message, file, function, line);
	assertEqualIntImpl (propertyIds[1], 11, message, file, function, line);

	// Override an existing property.
	status = GdipSetPropertyItem (image, &propertyItem3);
	assertEqualIntImpl (status, Ok, message, file, function, line);

	status = GdipGetPropertyItemSize (image, propertyItem3.id, &propertySize);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (propertySize, (int) sizeof(PropertyItem), message, file, function, line);

	status = GdipGetPropertyItem (image, propertyItem3.id, propertySize, &resultPropertyItem);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.id, 10, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.length, 0, message, file, function, line);
	assertEqualIntImpl (resultPropertyItem.type, 9, message, file, function, line);

	status = GdipGetPropertyCount (image, &numProperties);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (numProperties, 2, message, file, function, line);

	numProperties = -1;
	status = GdipGetPropertySize (image, &totalBufferSize, &numProperties);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (totalBufferSize, (int) sizeof(PropertyItem) * 2, message, file, function, line);
	assertEqualIntImpl (numProperties, 2, message, file, function, line);

	propertyIds[1] = -1;
	status = GdipGetPropertyIdList (image, numProperties, propertyIds);
	assertEqualIntImpl (status, Ok, message, file, function, line);
	assertEqualIntImpl (propertyIds[0], 10, message, file, function, line);
	assertEqualIntImpl (propertyIds[1], 11, message, file, function, line);
}

#define setPropertyItemForImage(image) setPropertyItemForImageImpl (image, NULL, __FILE__, __func__, __LINE__)

static void test_setPropertyItem()
{
	GpStatus status;
	GpImage *bmpImage = getImage ("test.bmp");
	GpImage *tifImage = getImage ("test.tif");
	GpImage *gifImage = getImage ("test.gif");
	GpImage *pngImage = getImage ("test.png");
	GpImage *jpegImage = getImage ("test.jpg");
	GpImage *jpegImageWithProperties = getImage ("nature24bits.jpg");
	GpImage *icoImage = getImage ("test.ico");
	GpImage *metafileImage = getImage ("test.wmf");
	GpImage *memoryImage;
	GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat32bppARGB, NULL, (GpBitmap **)&memoryImage);
	PropertyItem custom = {10, 0, 11, NULL};
	UINT totalBufferSize;
	UINT numProperties;
	PropertyItem *propertyItems = (PropertyItem *) malloc (1024);

	setPropertyItemForImage (bmpImage);

	status = GdipSetPropertyItem (tifImage, &custom);
	assertEqualInt (status, Ok);

	status = GdipSetPropertyItem (gifImage, &custom);
	assertEqualInt (status, Ok);

	status = GdipSetPropertyItem (pngImage, &custom);
	assertEqualInt (status, Ok);

	status = GdipSetPropertyItem (jpegImage, &custom);
	assertEqualInt (status, Ok);

	setPropertyItemForImage (icoImage);

	setPropertyItemForImage (memoryImage);

	// Change item - shorten.
	char text1[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};
	custom.id = PropertyTagExifUserComment;
	custom.type = PropertyTagTypeASCII;
	custom.length = sizeof (text1);
	custom.value = text1;

	status = GdipSetPropertyItem (jpegImageWithProperties, &custom);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImageWithProperties, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 3 + 12 + 128 + 128);
	assertEqualInt (numProperties, 3);

	status = GdipGetAllPropertyItems (jpegImageWithProperties, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	PropertyItem *propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 12);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assertEqualInt (strcmp ("Hello World", (char *) propertyItem->value), 0);
	assert (text1 != propertyItem->value);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected1[] = {
		0x16, 0x00, 0x17, 0x00, 0x1F, 0x00, 0x3E, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x17,
		0x00, 0x1B, 0x00, 0x22, 0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x1F,
		0x00, 0x22, 0x00, 0x49, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x3E,
		0x00, 0x57, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82,
		0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	BYTE expected2[] = {
		0x15, 0x00, 0x0E, 0x00, 0x0D, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x43, 0x00, 0x50, 0x00, 0x0F,
		0x00, 0x0F, 0x00, 0x12, 0x00, 0x19, 0x00, 0x22, 0x00, 0x4C, 0x00, 0x4F, 0x00, 0x48, 0x00, 0x12,
		0x00, 0x11, 0x00, 0x15, 0x00, 0x1F, 0x00, 0x34, 0x00, 0x4B, 0x00, 0x5B, 0x00, 0x49, 0x00, 0x12,
		0x00, 0x16, 0x00, 0x1D, 0x00, 0x26, 0x00, 0x43, 0x00, 0x72, 0x00, 0x69, 0x00, 0x51, 0x00, 0x17,
		0x00, 0x1D, 0x00, 0x30, 0x00, 0x49, 0x00, 0x59, 0x00, 0x8F, 0x00, 0x87, 0x00, 0x65, 0x00, 0x1F,
		0x00, 0x2E, 0x00, 0x48, 0x00, 0x54, 0x00, 0x6A, 0x00, 0x89, 0x00, 0x95, 0x00, 0x79, 0x00, 0x40,
		0x00, 0x54, 0x00, 0x66, 0x00, 0x72, 0x00, 0x87, 0x00, 0x9F, 0x00, 0x9E, 0x00, 0x85, 0x00, 0x5F,
		0x00, 0x79, 0x00, 0x7D, 0x00, 0x81, 0x00, 0x93, 0x00, 0x84, 0x00, 0x87, 0x00, 0x82, 0x00
	};
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);

	// Change item - lengthen.
	char text2[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', ' ', 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', ' ', 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};
	custom.id = PropertyTagExifUserComment;
	custom.type = PropertyTagTypeASCII;
	custom.length = sizeof (text2);
	custom.value = text2;

	status = GdipSetPropertyItem (jpegImageWithProperties, &custom);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImageWithProperties, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 3 + 36 + 128 + 128);
	assertEqualInt (numProperties, 3);

	status = GdipGetAllPropertyItems (jpegImageWithProperties, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 36);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assertEqualInt (strcmp ("Hello World Hello World Hello World", (char *) propertyItem->value), 0);
	assert (text2 != propertyItem->value);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);

	// Change item - same length.
	char text3[] = {'Y', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', ' ', 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', ' ', 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '\0'};
	custom.id = PropertyTagExifUserComment;
	custom.type = PropertyTagTypeASCII;
	custom.length = sizeof (text3);
	custom.value = text3;

	status = GdipSetPropertyItem (jpegImageWithProperties, &custom);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImageWithProperties, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 3 + 36 + 128 + 128);
	assertEqualInt (numProperties, 3);

	status = GdipGetAllPropertyItems (jpegImageWithProperties, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 36);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assertEqualInt (strcmp ("Yello World Hello World Hello World", (char *) propertyItem->value), 0);
	assert (text3 != propertyItem->value);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);

	// Change item - zero length.
	custom.id = PropertyTagExifUserComment;
	custom.type = PropertyTagTypeASCII;
	custom.length = 0;
	custom.value = text3;

	status = GdipSetPropertyItem (jpegImageWithProperties, &custom);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImageWithProperties, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 3 + 128 + 128);
	assertEqualInt (numProperties, 3);

	status = GdipGetAllPropertyItems (jpegImageWithProperties, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 0);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);

	// Add item.
	char text4[] = {'N', 'e', 'w', '\0'};
	custom.id = 1;
	custom.type = 10;
	custom.length = sizeof (text4);
	custom.value = text4;

	status = GdipSetPropertyItem (jpegImageWithProperties, &custom);
	assertEqualInt (status, Ok);

	status = GdipGetPropertySize (jpegImageWithProperties, &totalBufferSize, &numProperties);
	assertEqualInt (status, Ok);
	assertEqualInt (totalBufferSize, sizeof (PropertyItem) * 4 + 128 + 128 + 4);
	assertEqualInt (numProperties, 4);

	status = GdipGetAllPropertyItems (jpegImageWithProperties, totalBufferSize, numProperties, propertyItems);
	assertEqualInt (status, Ok);

	propertyItem = propertyItems;
	assertEqualInt (propertyItem->id, PropertyTagExifUserComment);
	assertEqualInt (propertyItem->length, 0);
	assertEqualInt (propertyItem->type, PropertyTagTypeASCII);
	assert (text3 != propertyItem->value);
	propertyItem++;

	assertEqualInt (propertyItem->id, PropertyTagChrominanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected1, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (status, Ok);
	assertEqualInt (propertyItem->id, PropertyTagLuminanceTable);
	assertEqualInt (propertyItem->length, 128);
	assertEqualInt (propertyItem->type, PropertyTagTypeShort);
	assertEqualBytes (expected2, (BYTE *) propertyItem->value, propertyItem->length);
	propertyItem++;

	assertEqualInt (propertyItem->id, 1);
	assertEqualInt (propertyItem->length, 4);
	assertEqualInt (propertyItem->type, 10);
	assertEqualInt (strcmp ("New", (char *) propertyItem->value), 0);
	assert (text4 != propertyItem->value);

	// Negative tests.
	status = GdipSetPropertyItem (NULL, &custom);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPropertyItem (metafileImage, &custom);
	assertEqualInt (status, NotImplemented);

	status = GdipSetPropertyItem (bmpImage, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetPropertyItem (metafileImage, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImage (bmpImage);
	GdipDisposeImage (tifImage);
	GdipDisposeImage (gifImage);
	GdipDisposeImage (pngImage);
	GdipDisposeImage (jpegImage);
	GdipDisposeImage (jpegImageWithProperties);
	GdipDisposeImage (icoImage);
	GdipDisposeImage (metafileImage);
	GdipDisposeImage (memoryImage);
	free (propertyItems);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_loadImageFromStream ();
	test_loadImageFromFile ();
	test_loadImageFromStreamICM ();
	test_loadImageFromFileICM ();
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

	SHUTDOWN;
	return 0;
}
