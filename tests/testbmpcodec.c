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

static WCHAR file[] = {'t', 'e', 'm', 'p', '.', 'b', 'm', 'p', 0};

static void createFile (BYTE *buffer, int length, GpStatus expectedStatus)
{
	GpStatus status;
	GpImage *image;
	FILE *f = fopen ("temp.bmp", "w+");
	fwrite ((void *) buffer, sizeof(BYTE), length, f);
	fclose (f);
	
	status = GdipLoadImageFromFile (file, &image);
	assertEqualInt (status, expectedStatus);
	
	if (expectedStatus == Ok)
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
	BYTE hasImageData[] =            {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE hasExtraImageData[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0, 1, 2, 3, 4};
	
	BYTE planesNotOne[] =            {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE invalidBitsPerPixel[] =     {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
#if defined(USE_WINDOWS_GDIPLUS)
	BYTE rle4Non4Bpp[] =             {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 2, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	BYTE rle8Non8Bpp[] =             {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
#endif
	BYTE bitfieldsNon16Bpp[] =       {0x42, 0x4D, 62, 0, 0, 0, 0, 0, 0, 0x3E, 0, 0, 0, 0, 40, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 32, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 0, 128, 0, 0, 0};
	
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
	createFile (hasImageData, sizeof(hasImageData), Ok);
	createFile (hasExtraImageData, sizeof(hasExtraImageData), Ok);
	
	createFile (planesNotOne, sizeof(planesNotOne), Ok);
	createFile (invalidBitsPerPixel, sizeof(invalidBitsPerPixel), OutOfMemory);
	// FIXME: this should not fail with InvalidParameter in libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	createFile (rle4Non4Bpp, sizeof(rle4Non4Bpp), Ok);
	createFile (rle8Non8Bpp, sizeof(rle8Non8Bpp), Ok);
#endif
	createFile (bitfieldsNon16Bpp, sizeof(bitfieldsNon16Bpp), OutOfMemory);
}

int
main (int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	
	test_invalidHeader ();
	
	GdiplusShutdown (gdiplusToken);
	return 0;
}
