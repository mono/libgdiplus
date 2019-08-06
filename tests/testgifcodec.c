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

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include "testhelpers.h"

static const char *file = "temp_asset.gif";
static WCHAR wFile[] = {'t', 'e', 'm', 'p', '_', 'a', 's', 's', 'e', 't', '.', 'g', 'i', 'f', 0};
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

#define createFileSuccess(buffer, expectedWidth, expectedHeight) \
{ \
  createFile(buffer, Ok); \
  verifyBitmap (image, gifRawFormat, PixelFormat8bppIndexed, 3, 5, ImageFlagsColorSpaceRGB | ImageFlagsHasRealDPI | ImageFlagsHasRealPixelSize | ImageFlagsReadOnly, 0, TRUE); \
  GdipDisposeImage (image); \
}

static void test_validData ()
{
  GpImage *image;

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

  createFileSuccess (localColorTable89, 3, 5);
  createFileSuccess (globalColorTable89, 3, 5);
  createFileSuccess (localAndGlobalColorTable89, 3, 5);
  createFileSuccess (localColorTable87, 3, 5);
  createFileSuccess (globalColorTable87, 3, 5);
  createFileSuccess (localAndGlobalColorTable87, 3, 5);
  createFileSuccess (extraData, 3, 5);
  createFileSuccess (noColorTables, 3, 5);
  createFileSuccess (emptyExtensionBlock, 3, 5);
  createFileSuccess (unknownExtensionBlock87, 3, 5);
  createFileSuccess (unknownExtensionBlock89, 3, 5);
  createFileSuccess (graphicsControlBlock, 3, 5);

  // FIXME: it appears that GDI+ allows a graphics control extension block without a
  // terminating 0 byte.
#if defined(USE_WINDOWS_GDIPLUS)
  createFileSuccess (graphicsControlBlockMissingTerminator, 3, 5);
#endif

  createFileSuccess (severalGraphicsControlBlocks, 3, 5);
  createFileSuccess (misplacedGraphicsControlBlock, 3, 5);
  createFileSuccess (plainTextControlBlock, 3, 5);
  createFileSuccess (invalidTextControlBlock, 3, 5);
  createFileSuccess (applicationTextControlBlock, 3, 5);
  createFileSuccess (commentControlBlock, 3, 5);
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

  createFile (noScreenWidth87, OutOfMemory);
  createFile (noScreenWidth89, OutOfMemory);
  createFile (shortScreenWidth, OutOfMemory);
  createFile (noScreenHeight, OutOfMemory);
  createFile (shortScreenHeight, OutOfMemory);
  createFile (noColorTableFlags, OutOfMemory);
  createFile (noBackgroundColor, OutOfMemory);
  createFile (noPixelAspectRatio, OutOfMemory);
  createFile (noGlobalColorTable, OutOfMemory);
  createFile (shortGlobalColorTable, OutOfMemory);
  createFile (smallGlobalColorTable, OutOfMemory);
  createFile (noRecords, OutOfMemory);
  createFile (unknownChunk, OutOfMemory);
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

  createFile (noImageLeft, OutOfMemory);
  createFile (shortImageLeft, OutOfMemory);
  createFile (noImageTop, OutOfMemory);
  createFile (shortImageTop, OutOfMemory);
  createFile (noImageWidth, OutOfMemory);
  createFile (shortImageWidth, OutOfMemory);
  createFile (noImageHeight, OutOfMemory);
  createFile (shortImageHeight, OutOfMemory);
  createFile (noImagePacked, OutOfMemory);
  createFile (noLocalColorTable, OutOfMemory);
  createFile (shortLocalColorTable, OutOfMemory);
  createFile (smallLocalColorTable, OutOfMemory);
  createFile (noCodeSizeWithTable, OutOfMemory);
  createFile (noCodeSizeNoTable, OutOfMemory);
  createFile (noData, OutOfMemory);
  createFile (noColorTables, OutOfMemory);

  // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
  createFile (noImageDataWithTable, OutOfMemory);
  createFile (noImageDataTerminator, OutOfMemory);
  createFile (noCompressedData, OutOfMemory);
  createFile (notEnoughImageData, OutOfMemory);
  createFile (noTerminator, OutOfMemory);
  createFile (invalidCompression, OutOfMemory);
#endif

  createFile (zeroHeaderWidth, OutOfMemory);
  createFile (zeroHeaderHeight, OutOfMemory);
  // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
  createFile (invalidImageLeft, OutOfMemory);
  createFile (invalidImageTop, OutOfMemory);
#endif
  createFile (zeroImageWidth, OutOfMemory);
  createFile (zeroImageHeight, OutOfMemory);

  // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
  createFile (invalidImageWidth, OutOfMemory);
  createFile (invalidImageHeight, OutOfMemory);
  createFile (invalidImageLeftWidth, OutOfMemory);
  createFile (invalidImageTopHeight, OutOfMemory);
#endif
}

static void test_invalidExtensionRecord ()
{
  BYTE noExtensionLabel[]              = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!'};
  BYTE noExtensionSize[]               = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9};
  BYTE noExtensionContent[]            = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9, 1, 0, ';'};
  BYTE multipleGraphicsControlBlocks[] = {'G', 'I', 'F', '8', '9', 'a', 3, 0, 5, 0, 0, 0, 0, '!', 0xF9, 0x04, 0, 0, 0, 0, 0x04, 0, 0, 0, 0, 0, ',', 0, 0, 0, 0, 3, 0, 5, 0, B8(10000000), 0, 0, 0, 255, 255, 255, 0x02, 0x06, 0x84, 0x03, 0x81, 0x9a, 0x06, 0x05, 0x00, ';'};

  createFile (noExtensionLabel, OutOfMemory);
  createFile (noExtensionSize, OutOfMemory);
  createFile (noExtensionContent, OutOfMemory);
  createFile (multipleGraphicsControlBlocks, OutOfMemory);
}

int
main (int argc, char**argv)
{
	STARTUP;

  test_validData ();
  test_invalidHeader ();
  test_invalidImageRecord ();
  test_invalidExtensionRecord ();

  deleteFile (file);

  SHUTDOWN;
  return 0;
}
