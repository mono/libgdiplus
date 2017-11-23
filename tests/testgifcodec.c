#ifdef _WIN32
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

static const char *file = "temp_asset.gif";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'a', 's', 's', 'e', 't', '.', 'g', 'i', 'f', 0};

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

static void test_validData ()
{
  GpImage *image;
  const INT gifFlags = ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly;

  BYTE localColorTable89[]                     = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE globalColorTable89[]                    = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 0, 0, 0, 255, 255, 255, ',', 0, 0, 0, 0, 3, 0, 5, 0, 0, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE localAndGlobalColorTable89[]            = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 0, 0, 0, 255, 255, 255, ',', 0, 0, 0, 0, 3, 0, 5, 0,  B8(10000000), 255, 255, 255, 0, 0, 0, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE localColorTable87[]                     = {'G', 'I', 'F', '8', '7', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE globalColorTable87[]                    = {'G', 'I', 'F', '8', '7', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 0, 0, 0, 255, 255, 255, ',', 0, 0, 0, 0, 3, 0, 5, 0, 0, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE localAndGlobalColorTable87[]            = {'G', 'I', 'F', '8', '7', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 0, 0, 0, 255, 255, 255, ',', 0, 0, 0, 0, 3, 0, 5, 0,  B8(10000000), 255, 255, 255, 0, 0, 0, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE extraData[]                             = {'G', 'I', 'F', '8', '7', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 0, 0, 0, 255, 255, 255, ',', 0, 0, 0, 0, 3, 0, 5, 0,  B8(10000000), 255, 255, 255, 0, 0, 0, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';', ';'};
  BYTE noColorTables[]                         = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, 0, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE emptyExtensionBlock[]                   = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE unknownExtensionBlock87[]               = {'G', 'I', 'F', '8', '7', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 1, 2, 3, 4, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE unknownExtensionBlock89[]               = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 1, 2, 3, 4, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE graphicsControlBlock[]                  = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9, 0x04, 0, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
#if defined(USE_WINDOWS_GDIPLUS)
  BYTE graphicsControlBlockMissingTerminator[] = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9, 0x04, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
#endif
  BYTE severalGraphicsControlBlocks[]          = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9, 0x04, 0, 0, 0, 0, 0, '!', 0xF9, 0x04, 0, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE misplacedGraphicsControlBlock[]         = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, '!', 0xF9, 0x04, B8(11111111), 0, 0, 0, 0, ';'};
  BYTE plainTextControlBlock[]                 = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0x01, 0x0C, 0, 0, 0, 0, 3, 0, 5, 0, 1, 1, 0, 1, 2, 'H', 'I', 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE invalidTextControlBlock[]               = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0x01, 0x0C, 200, 0, 200, 0, 100, 0, 100, 0, 255, 255, 40, 40, 2, 'H', 'I', 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE applicationTextControlBlock[]           = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xFF, 0x0B, '1', '2', '3', '4', '5', '6', '7', '8', 1, 2, 3, 2, 'H', 'I', 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE commentControlBlock[]                   = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xFE, 2, 'H', 'I', 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};

  image = createFile (localColorTable89, sizeof(localColorTable89), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (globalColorTable89, sizeof(globalColorTable89), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (localAndGlobalColorTable89, sizeof(localAndGlobalColorTable89), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (localColorTable87, sizeof(localColorTable87), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (globalColorTable87, sizeof(globalColorTable87), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (localAndGlobalColorTable87, sizeof(localAndGlobalColorTable87), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (extraData, sizeof(extraData), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (noColorTables, sizeof(noColorTables), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (emptyExtensionBlock, sizeof(emptyExtensionBlock), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (unknownExtensionBlock87, sizeof(unknownExtensionBlock87), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (unknownExtensionBlock89, sizeof(unknownExtensionBlock89), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (graphicsControlBlock, sizeof(graphicsControlBlock), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  // FIXME: it appears that GDI+ allows a graphics control extension block without a
  // terminating 0 byte.
#if defined(USE_WINDOWS_GDIPLUS)
  image = createFile (graphicsControlBlockMissingTerminator, sizeof(graphicsControlBlockMissingTerminator), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);
#endif

  image = createFile (severalGraphicsControlBlocks, sizeof(severalGraphicsControlBlocks), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (misplacedGraphicsControlBlock, sizeof(misplacedGraphicsControlBlock), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (plainTextControlBlock, sizeof(plainTextControlBlock), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (invalidTextControlBlock, sizeof(invalidTextControlBlock), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (applicationTextControlBlock, sizeof(applicationTextControlBlock), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);

  image = createFile (commentControlBlock, sizeof(commentControlBlock), Ok);
  verifyImage (image, ImageTypeBitmap, gifRawFormat, PixelFormat8bppIndexed, 0, 0, 3, 5, 3, 5, gifFlags, 0, TRUE);
  GdipDisposeImage (image);
}

static void test_invalidHeader ()
{
  BYTE noScreenWidth87[]        = {'G', 'I', 'F', '8', '7', 'a'};
  BYTE noScreenWidth89[]        = {'G', 'I', 'F', '8', '9', 'a'};
  BYTE shortScreenWidth[]       = {'G', 'I', 'F', '8', '9', 'a', 3};
  BYTE noScreenHeight[]         = {'G', 'I', 'F', '8', '9', 'a', 3, 0};
  BYTE shortScreenHeight[]      = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5};
  BYTE noColorTableFlags[]      = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0};
  BYTE noBackgroundColor[]      = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0};
  BYTE noPixelAspectRatio[]     = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0};
  BYTE noGlobalColorTable[]     = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, B8(10000000), 0, 0};
  BYTE shortGlobalColorTable[]  = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 255};
  BYTE smallGlobalColorTable[]  = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 255, 255, 255};
  BYTE noRecords[]              = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0};
  BYTE unknownChunk[]           = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '_'};
  
  createFile (noScreenWidth87, sizeof(noScreenWidth87), OutOfMemory);
  createFile (noScreenWidth89, sizeof(noScreenWidth89), OutOfMemory);
  createFile (shortScreenWidth, sizeof(shortScreenWidth), OutOfMemory);
  createFile (noScreenHeight, sizeof(noScreenHeight), OutOfMemory);
  createFile (shortScreenHeight, sizeof(shortScreenHeight), OutOfMemory);
  createFile (noColorTableFlags, sizeof(noColorTableFlags), OutOfMemory);
  createFile (noBackgroundColor, sizeof(noBackgroundColor), OutOfMemory);
  createFile (noPixelAspectRatio, sizeof(noPixelAspectRatio), OutOfMemory);
  createFile (noGlobalColorTable, sizeof(noGlobalColorTable), OutOfMemory);
  createFile (shortGlobalColorTable, sizeof(shortGlobalColorTable), OutOfMemory);
  createFile (smallGlobalColorTable, sizeof(smallGlobalColorTable), OutOfMemory);
  createFile (noRecords, sizeof(noRecords), OutOfMemory);
  createFile (unknownChunk, sizeof(unknownChunk), OutOfMemory);
}

static void test_invalidImageRecord ()
{
  BYTE noImageLeft[]            = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ','};
  BYTE shortImageLeft[]         = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0};
  BYTE noImageTop[]             = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0};
  BYTE shortImageTop[]          = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0};
  BYTE noImageWidth[]           = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0};
  BYTE shortImageWidth[]        = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3};
  BYTE noImageHeight[]          = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0};
  BYTE shortImageHeight[]       = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5};
  BYTE noImagePacked[]          = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0};
  BYTE noLocalColorTable[]      = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000)};
  BYTE shortLocalColorTable[]   = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0};
  BYTE smallLocalColorTable[]   = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0};
  BYTE noCodeSizeWithTable[]    = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255};
  BYTE noCodeSizeNoTable[]      = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, 0};
  BYTE noData[]                 = {'G', 'I', 'F', '8', '7', 'a', 3, 0, 5, 0, B8(10000000), 0, 0, 0, 0, 0, 255, 255, 255, ';'};
  BYTE noColorTables[]          = {'G', 'I', 'F', '8', '9', 'a', 10, 0, 11, 0, 0, 0, 1, ';'};
#if !defined(USE_WINDOWS_GDIPLUS)
  BYTE noImageDataWithTable[]   = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0};
  BYTE noCompressedData[]       = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06};
  BYTE notEnoughImageData[]     = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84};
  BYTE noImageDataTerminator[]  = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, };
  BYTE noTerminator[]           = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00};
  BYTE invalidCompression[]     = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, ';'};
#endif
  BYTE zeroHeaderWidth[]        = {'G', 'I', 'F', '8', '9', 'a', 0, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE zeroHeaderHeight[]       = {'G', 'I', 'F', '8', '9', 'a', 0, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
#if !defined(USE_WINDOWS_GDIPLUS)
  BYTE invalidImageLeft[]       = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 3, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE invalidImageTop[]        = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 3, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
#endif
  BYTE zeroImageWidth[]         = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 0, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE zeroImageHeight[]        = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 0, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
#if !defined(USE_WINDOWS_GDIPLUS)
  BYTE invalidImageWidth[]      = {'G', 'I', 'F', '8', '9', 'a', 2, 0, 5, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE invalidImageHeight[]     = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 4, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE invalidImageLeftWidth[]  = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 1, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
  BYTE invalidImageTopHeight[]  = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, ',', 0, 0, 1, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};
#endif

  createFile (noImageLeft, sizeof(noImageLeft), OutOfMemory);
  createFile (shortImageLeft, sizeof(shortImageLeft), OutOfMemory);
  createFile (noImageTop, sizeof(noImageTop), OutOfMemory);
  createFile (shortImageTop, sizeof(shortImageTop), OutOfMemory);
  createFile (noImageWidth, sizeof(noImageWidth), OutOfMemory);
  createFile (shortImageWidth, sizeof(shortImageWidth), OutOfMemory);
  createFile (noImageHeight, sizeof(noImageHeight), OutOfMemory);
  createFile (shortImageHeight, sizeof(shortImageHeight), OutOfMemory);
  createFile (noImagePacked, sizeof(noImagePacked), OutOfMemory);
  createFile (noLocalColorTable, sizeof(noLocalColorTable), OutOfMemory);
  createFile (shortLocalColorTable, sizeof(shortLocalColorTable), OutOfMemory);
  createFile (smallLocalColorTable, sizeof(smallLocalColorTable), OutOfMemory);
  createFile (noCodeSizeWithTable, sizeof(noCodeSizeWithTable), OutOfMemory);
  createFile (noCodeSizeNoTable, sizeof(noCodeSizeNoTable), OutOfMemory);
  createFile (noData, sizeof(noData), OutOfMemory);
  createFile (noColorTables, sizeof(noColorTables), OutOfMemory);

  // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
  createFile (noImageDataWithTable, sizeof(noImageDataWithTable), OutOfMemory);
  createFile (noImageDataTerminator, sizeof(noImageDataTerminator), OutOfMemory);
  createFile (noCompressedData, sizeof(noCompressedData), OutOfMemory);
  createFile (notEnoughImageData, sizeof(notEnoughImageData), OutOfMemory);
  createFile (noTerminator, sizeof(noTerminator), OutOfMemory);
  createFile (invalidCompression, sizeof(invalidCompression), OutOfMemory);
#endif

  createFile (zeroHeaderWidth, sizeof(zeroHeaderWidth), OutOfMemory);
  createFile (zeroHeaderHeight, sizeof(zeroHeaderHeight), OutOfMemory);
  // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
  createFile (invalidImageLeft, sizeof(invalidImageLeft), OutOfMemory);
  createFile (invalidImageTop, sizeof(invalidImageTop), OutOfMemory);
#endif
  createFile (zeroImageWidth, sizeof(zeroImageWidth), OutOfMemory);
  createFile (zeroImageHeight, sizeof(zeroImageHeight), OutOfMemory);

  // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
  createFile (invalidImageWidth, sizeof(invalidImageWidth), OutOfMemory);
  createFile (invalidImageHeight, sizeof(invalidImageHeight), OutOfMemory);
  createFile (invalidImageLeftWidth, sizeof(invalidImageLeftWidth), OutOfMemory);
  createFile (invalidImageTopHeight, sizeof(invalidImageTopHeight), OutOfMemory);
#endif
}

static void test_invalidExtensionRecord ()
{
  BYTE noExtensionLabel[]              = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!'};
  BYTE noExtensionSize[]               = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9};
  BYTE noExtensionContent[]            = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9, 1, 0, ';'};
  BYTE multipleGraphicsControlBlocks[] = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9, 0x04, 0, 0, 0, 0, 0x04, 0, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};

  createFile (noExtensionLabel, sizeof(noExtensionLabel), OutOfMemory);
  createFile (noExtensionSize, sizeof(noExtensionSize), OutOfMemory);
  createFile (noExtensionContent, sizeof(noExtensionContent), OutOfMemory);
  createFile (multipleGraphicsControlBlocks, sizeof(multipleGraphicsControlBlocks), OutOfMemory);
}

int
main (int argc, char**argv)
{
  GdiplusStartupInput gdiplusStartupInput;
  ULONG_PTR gdiplusToken;
  GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
  
  test_validData ();
  test_invalidHeader ();
  test_invalidImageRecord ();
  test_invalidExtensionRecord ();
  
#if !defined(_WIN32)
  unlink (file);
#else
  DeleteFileA (file);
#endif
  
  GdiplusShutdown (gdiplusToken);
  return 0;
}
