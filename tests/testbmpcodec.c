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

#ifdef USE_WINDOWS_GDIPLUS
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"

#if !defined(_WIN32)
#include <unistd.h>
#endif

static const char *file = "temp_asset.bmp";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'a', 's', 's', 'e', 't', '.', 'b', 'm', 'p', 0};

static GpImage *createFile (BYTE *buffer, int length, GpStatus expectedStatus)
{
	GpStatus status;
	GpImage *image;
	FILE *f = fopen (file, "w+");
	fwrite ((void *) buffer, sizeof (BYTE), length, f);
	fclose (f);
	
	status = GdipLoadImageFromFile (wFile, &image);
	assertEqualInt (status, expectedStatus);
	
	return image;
}

static void createFileSuccess (BYTE *buffer, int length, PixelFormat expectedFormat)
{
	PixelFormat format;
	GpImage *image = createFile (buffer, length, Ok);
	
	GdipGetImagePixelFormat (image, &format);
	assertEqualInt (format, expectedFormat);
	
	GdipDisposeImage (image);
}

static void test_validImage1bppOS2Header ()
{
	BYTE image1x1[] = {
		'B', 'M', 36, 0, 0, 0, 0, 0, 0, 0, 0x20, 0, 0, 0,
		12, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0,
		255, 0, 0, 0, 255, 0,
		B8(10000000), B8(00000000), B8(00000000), 0
	};
	BYTE image24x4[] = {
		'B', 'M', 48, 0, 0, 0, 0, 0, 0, 0, 0x20, 0, 0, 0,
		12, 0, 0, 0, 24, 0, 4, 0, 1, 0, 1, 0,
		255, 0, 0, 0, 255, 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(00100111), B8(00000000), B8(00000000), 0,
		B8(11111111), B8(11111111), B8(11111111), 0
	};

	createFileSuccess (image1x1, sizeof (image1x1), PixelFormat1bppIndexed);
	createFileSuccess (image24x4, sizeof (image24x4), PixelFormat1bppIndexed);
}

static void test_validImage1bppBitmapInfoHeader ()
{
	BYTE image1x1[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255,
		B8(10000000), 0, 0, 0
	};
	BYTE image24x4CustomSizedPalette[] = {
		'B', 'M', 82, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 24, 0, 0, 0, 4, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 255, 255, 255, 255,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(00100111), B8(00000000), B8(00000000), 0,
		B8(11111111), B8(11111111), B8(11111111), 0
	};
	BYTE image1x1NegativeHeight[] = {
		'B', 'M', 66, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 255, 255, 255, 255, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255,
		B8(1000000), 0, 0, 0
	};
	BYTE image24x4NegativeHeight[] = {
		'B', 'M', 82, 0, 0, 0, 0, 0, 0, 0, 0x42, 0, 0, 0,
		40, 0, 0, 0, 24, 0, 0, 0, 252, 255, 255, 255, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0,
		255, 0, 0, 255, 0, 255, 0, 255, 255, 255, 255, 255,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(10000000), B8(00000000), B8(00000000), 0,
		B8(00100111), B8(00000000), B8(00000000), 0,
		B8(11111111), B8(11111111), B8(11111111), 0
	};

	createFileSuccess (image1x1, sizeof (image1x1), PixelFormat1bppIndexed);
	createFileSuccess (image24x4CustomSizedPalette, sizeof (image24x4CustomSizedPalette), PixelFormat1bppIndexed);
	createFileSuccess (image1x1NegativeHeight, sizeof (image1x1NegativeHeight), PixelFormat1bppIndexed);
	createFileSuccess (image24x4NegativeHeight, sizeof (image24x4NegativeHeight), PixelFormat1bppIndexed);
}

static void test_validImage ()
{
	BYTE hasImageData16bpp[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		128, 0, 0, 0
	};
	BYTE hasImageData32bpp[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		128, 0, 0, 0
	};
	BYTE hasImageData64bpp[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 64, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		128, 0, 0, 0
	};
	BYTE hasExtraImageData[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		128, 0, 0, 0, 1, 2, 3, 4
	};
	BYTE planesNotOne[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		128, 0, 0, 0
	};
	BYTE rle4Non4Bpp[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		128, 0, 0, 0
	};
	BYTE rle8Non8Bpp[] = {
		'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0,
		40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 255, 255, 255, 0,
		128, 0, 0, 0
	};

	
	createFileSuccess (hasImageData16bpp, sizeof (hasImageData16bpp), PixelFormat32bppRGB);
	createFileSuccess (hasImageData32bpp, sizeof (hasImageData32bpp), PixelFormat32bppRGB);
	createFileSuccess (hasImageData64bpp, sizeof (hasImageData64bpp), PixelFormat64bppARGB);
	createFileSuccess (hasExtraImageData, sizeof (hasExtraImageData), PixelFormat1bppIndexed);
	createFileSuccess (planesNotOne, sizeof (planesNotOne), PixelFormat1bppIndexed);
	createFileSuccess (rle4Non4Bpp, sizeof (rle4Non4Bpp), PixelFormat32bppRGB);
	createFileSuccess (rle8Non8Bpp, sizeof (rle8Non8Bpp), PixelFormat32bppRGB);

}

static void test_invalidFileHeader ()
{
	BYTE shortSignature[] = {'B'};
	BYTE noImageSize[]    = {'B', 'M'};
	BYTE shortImageSize[] = {'B', 'M', 5, 0, 0};
	BYTE noReserved1[]    = {'B', 'M', 6, 0, 0, 0};
	BYTE shortReserved1[] = {'B', 'M', 7, 0, 0, 0, 0};
	BYTE noReserved2[]    = {'B', 'M', 8, 0, 0, 0, 0, 0};
	BYTE shortReserved2[] = {'B', 'M', 9, 0, 0, 0, 0, 0, 0};
	BYTE noOffset[]       = {'B', 'M', 10, 0, 0, 0, 0, 0, 0, 0};
	BYTE shortOffset[]    = {'B', 'M', 13, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0};

	createFile (shortSignature, sizeof (shortSignature), OutOfMemory);
	createFile (noImageSize, sizeof (noImageSize), OutOfMemory);
	createFile (shortImageSize, sizeof (shortImageSize), OutOfMemory);
	createFile (noReserved1, sizeof (noReserved1), OutOfMemory);
	createFile (shortReserved1, sizeof (shortReserved1), OutOfMemory);
	createFile (noReserved2, sizeof (noReserved2), OutOfMemory);
	createFile (shortReserved2, sizeof (shortReserved2), OutOfMemory);
	createFile (noOffset, sizeof (noOffset), OutOfMemory);
	createFile (shortOffset, sizeof (shortOffset), OutOfMemory);
}

static void test_invalidHeader ()
{
	BYTE noHeader[]               = {'B', 'M', 14, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0};
	BYTE zeroHeaderSize[]         = {'B', 'M', 18, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE newFormatNoWidth[]       = {'B', 'M', 18, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0};
	BYTE newFormatNoHeight[]      = {'B', 'M', 22, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0};
	BYTE os2FormatNoWidthHeight[] =	{'B', 'M', 18, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 12, 0, 0, 0};
	BYTE noColorPlanes[]          = {'B', 'M', 26, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
	BYTE noBitsPerPixel[]         = {'B', 'M', 28, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
	BYTE noCompression[]          = {'B', 'M', 30, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
	BYTE noSize[]                 = {'B', 'M', 34, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
	BYTE noHorizontalResolution[] = {'B', 'M', 38, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0};
	BYTE noVerticalResolution[]   = {'B', 'M', 42, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0};
	BYTE noColors[]               = {'B', 'M', 46, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE noImportantColors[]      = {'B', 'M', 50, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	createFile (noHeader, sizeof (noHeader), OutOfMemory);
	createFile (zeroHeaderSize, sizeof (zeroHeaderSize), OutOfMemory);
	createFile (newFormatNoWidth, sizeof (newFormatNoWidth), OutOfMemory);
	createFile (newFormatNoHeight, sizeof (newFormatNoHeight), OutOfMemory);
	createFile (os2FormatNoWidthHeight, sizeof (os2FormatNoWidthHeight), OutOfMemory);
	createFile (noColorPlanes, sizeof (noColorPlanes), OutOfMemory);
	createFile (noBitsPerPixel, sizeof (noBitsPerPixel), OutOfMemory);
	createFile (noCompression, sizeof (noCompression), OutOfMemory);
	createFile (noSize, sizeof (noSize), OutOfMemory);
	createFile (noHorizontalResolution, sizeof (noHorizontalResolution), OutOfMemory);
	createFile (noVerticalResolution, sizeof (noVerticalResolution), OutOfMemory);
	createFile (noColors, sizeof (noColors), OutOfMemory);
	createFile (noImportantColors, sizeof (noImportantColors), OutOfMemory);
}

static void test_invalidImageData ()
{
	BYTE noColorEntries[]         = {'B', 'M', 54, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE notEnoughColorEntries[]  = {'B', 'M', 58, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE noImageData[]            = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0};
#endif
	BYTE noImageDataBigSize[]     = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0};
	BYTE hasImageData4bpp[]       = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasImageData8bpp[]       = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE invalidBitsPerPixel[]    = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE bitfieldsNon16Bpp[]      = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE bitfields16Bpp[]         = {'B', 'M', 62, 0, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
#endif

	createFile (noColorEntries, sizeof (noColorEntries), OutOfMemory);
	createFile (notEnoughColorEntries, sizeof (notEnoughColorEntries), OutOfMemory);
	// FIXME: this should fail with OutOfMemory in libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFile (noImageData, sizeof (noImageData), OutOfMemory);
#endif
	createFile (noImageDataBigSize, sizeof (noImageDataBigSize), OutOfMemory);
	createFile (hasImageData4bpp, sizeof (hasImageData4bpp), OutOfMemory);
	createFile (hasImageData8bpp, sizeof (hasImageData8bpp), OutOfMemory);
	createFile (invalidBitsPerPixel, sizeof (invalidBitsPerPixel), OutOfMemory);
	createFile (bitfieldsNon16Bpp, sizeof (bitfieldsNon16Bpp), OutOfMemory);
	
	// FIXME: this returns Ok with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFile (bitfields16Bpp, sizeof (bitfields16Bpp), OutOfMemory);
#endif
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	
	test_validImage1bppOS2Header ();
	test_validImage1bppBitmapInfoHeader ();
	test_validImage ();
	test_invalidFileHeader ();
	test_invalidHeader ();
	test_invalidImageData ();

#if !defined(_WIN32)
	unlink (file);
#else
	DeleteFileA (file);
#endif
	
	GdiplusShutdown (gdiplusToken);
	return 0;
}
