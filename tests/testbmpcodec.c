#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(_WIN32)
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
	fwrite ((void *) buffer, sizeof(BYTE), length, f);
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

static void test_invalidHeader ()
{
	BYTE noHeader[] = 	 						 {0x42, 0x4D};
	BYTE shortHeader[] = 	 					 {0x42, 0x4D, 13, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0};
	BYTE noHeaderSize[] =     	 		 {0x42, 0x4D, 14, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0};
	BYTE zeroHeaderSize[] =          {0x42, 0x4D, 18, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE newFormatNoWidth[] =	       {0x42, 0x4D, 18, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0};
	BYTE newFormatNoHeight[] =	     {0x42, 0x4D, 22, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0};
	BYTE os2FormatNoWidthHeight[] =	 {0x42, 0x4D, 18, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 12, 0, 0, 0};
	BYTE noColorPlanes[] =	         {0x42, 0x4D, 26, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0};
	BYTE noBitsPerPixel[] =	         {0x42, 0x4D, 28, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
	BYTE noCompression[] =	         {0x42, 0x4D, 30, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0};
	BYTE noSize[] =	                 {0x42, 0x4D, 34, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
	BYTE noHorizontalResolution[] =  {0x42, 0x4D, 38, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0};
	BYTE noVerticalResolution[] =    {0x42, 0x4D, 42, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0};
	BYTE noColors[] =                {0x42, 0x4D, 46, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE noImportantColors[] =       {0x42, 0x4D, 50, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE noColorEntries[] =          {0x42, 0x4D, 54, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	BYTE notEnoughColorEntries[] =   {0x42, 0x4D, 58, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE noImageData[] =             {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0};
#endif
	BYTE noImageDataBigSize[] =      {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0};
	BYTE missingImageDataBigSize[] = {0x42, 0x4D, 70, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0, 0, 0, 0, 0};
	BYTE hasImageData1bpp[] =        {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasImageData4bpp[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 4, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasImageData8bpp[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 8, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasImageData16bpp[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasImageData32bpp[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasImageData64bpp[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 64, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasExtraImageData[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0, 1, 2, 3, 4};
	
	BYTE planesNotOne[] =            {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE invalidBitsPerPixel[] =     {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE rle4Non4Bpp[] =             {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE rle8Non8Bpp[] =             {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE bitfieldsNon16Bpp[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE bitfields16Bpp[] =          {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 16, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
#endif

	createFile (noHeader, sizeof(noHeader), OutOfMemory);
	createFile (shortHeader, sizeof(shortHeader), OutOfMemory);
	createFile (noHeaderSize, sizeof(noHeaderSize), OutOfMemory);
	createFile (zeroHeaderSize, sizeof(zeroHeaderSize), OutOfMemory);
	createFile (newFormatNoWidth, sizeof(newFormatNoWidth), OutOfMemory);
	createFile (newFormatNoHeight, sizeof(newFormatNoHeight), OutOfMemory);
	createFile (os2FormatNoWidthHeight, sizeof(os2FormatNoWidthHeight), OutOfMemory);
	createFile (noColorPlanes, sizeof(noColorPlanes), OutOfMemory);
	createFile (noBitsPerPixel, sizeof(noBitsPerPixel), OutOfMemory);
	createFile (noCompression, sizeof(noCompression), OutOfMemory);
	createFile (noSize, sizeof(noSize), OutOfMemory);
	createFile (noHorizontalResolution, sizeof(noHorizontalResolution), OutOfMemory);
	createFile (noVerticalResolution, sizeof(noVerticalResolution), OutOfMemory);
	createFile (noColors, sizeof(noColors), OutOfMemory);
	createFile (noImportantColors, sizeof(noImportantColors), OutOfMemory);
	createFile (noColorEntries, sizeof(noColorEntries), OutOfMemory);
	createFile (notEnoughColorEntries, sizeof(notEnoughColorEntries), OutOfMemory);
	// FIXME: this should fail with OutOfMemory in libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFile (noImageData, sizeof(noImageData), OutOfMemory);
#endif
	createFile (noImageDataBigSize, sizeof(noImageDataBigSize), OutOfMemory);
	createFileSuccess (missingImageDataBigSize, sizeof(missingImageDataBigSize), PixelFormat1bppIndexed);
	createFileSuccess (hasImageData1bpp, sizeof(hasImageData1bpp), PixelFormat1bppIndexed);
	createFile (hasImageData4bpp, sizeof(hasImageData4bpp), OutOfMemory);
	createFile (hasImageData8bpp, sizeof(hasImageData8bpp), OutOfMemory);
	createFileSuccess (hasImageData16bpp, sizeof(hasImageData16bpp), PixelFormat32bppRGB);
	createFileSuccess (hasImageData32bpp, sizeof(hasImageData32bpp), PixelFormat32bppRGB);
	createFileSuccess (hasImageData64bpp, sizeof(hasImageData64bpp), PixelFormat64bppARGB);
	createFileSuccess (hasExtraImageData, sizeof(hasExtraImageData), PixelFormat1bppIndexed);
	
	createFileSuccess (planesNotOne, sizeof(planesNotOne), PixelFormat1bppIndexed);
	createFile (invalidBitsPerPixel, sizeof(invalidBitsPerPixel), OutOfMemory);
	createFile (bitfieldsNon16Bpp, sizeof(bitfieldsNon16Bpp), OutOfMemory);
	
	// FIXME: this appears to return Ok with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFile (bitfields16Bpp, sizeof(bitfields16Bpp), OutOfMemory);
#endif
	createFileSuccess (rle4Non4Bpp, sizeof(rle4Non4Bpp), PixelFormat32bppRGB);
	createFileSuccess (rle8Non8Bpp, sizeof(rle8Non8Bpp), PixelFormat32bppRGB);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	
	test_invalidHeader ();

#if !defined(_WIN32)
	unlink (file);
#else
	DeleteFile (file);
#endif
	
	GdiplusShutdown (gdiplusToken);
	return 0;
}
