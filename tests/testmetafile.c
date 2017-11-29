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

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"

static WCHAR wmfFilePath[] = {'t', 'e', 's', 't', '.', 'w', 'm', 'f', 0};
static WCHAR emfFilePath[] = {'t', 'e', 's', 't', '.', 'e', 'm', 'f', 0};

static void test_createMetafileFromFile ()
{
    GpStatus status;
    WCHAR *noSuchFilePath;
    WCHAR *invalidFilePath;
    GpMetafile *metafile;

    noSuchFilePath = g_utf8_to_utf16 ("noSuchFile.wmf", -1, NULL, NULL, NULL);
    invalidFilePath = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);

    // Create from WMF file.
    status = GdipCreateMetafileFromFile (wmfFilePath, &metafile);
    assertEqualInt (status, Ok);
    verifyImage (metafile, ImageTypeMetafile, wmfRawFormat, PixelFormat32bppRGB, -4008, -3378, 8016, 6756, 20360.638672f, 17160.2383f, 327683, 0, TRUE);
    GdipDisposeImage (metafile);

    // Create from EMF file.
    status = GdipCreateMetafileFromFile (emfFilePath, &metafile);
    assertEqualInt (status, Ok);
    verifyImage (metafile, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 100, 100, 1944.444336f, 1888.888794f, 327683, 0, TRUE);
    GdipDisposeImage (metafile);

    // Negative tests.
    status = GdipCreateMetafileFromFile (NULL, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreateMetafileFromFile (noSuchFilePath, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreateMetafileFromFile (noSuchFilePath, &metafile);
    assertEqualInt (status, GenericError);

    status = GdipCreateMetafileFromFile (invalidFilePath, &metafile);
    assertEqualInt (status, GenericError);
}

static void test_createMetafileFromStream ()
{
    GpStatus status;
    GpMetafile *metafile;
#if defined(USE_WINDOWS_GDIPLUS)
    int temp = 0;
#endif
    
    // Negative tests.
    status = GdipCreateMetafileFromStream (NULL, &metafile);
    assertEqualInt (status, InvalidParameter);

    // Libgdiplus does not implement GdipCreateMetafileFromStream.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipCreateMetafileFromStream (&temp, NULL);
    assertEqualInt (status, InvalidParameter);
    
    status = GdipCreateMetafileFromStream (&temp, &metafile);
    assertEqualInt (status, NotImplemented);
#endif
}

static void test_createMetafileFromEmf ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    GpMetafile *metafile;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Create from a WMF file.
    status = GdipCreateMetafileFromEmf (wmfMetafile, TRUE, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);

    status = GdipCreateMetafileFromEmf (wmfMetafile, FALSE, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);

    // Create from a EMF file.
    status = GdipCreateMetafileFromEmf (emfMetafile, TRUE, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);

    status = GdipCreateMetafileFromEmf (emfMetafile, FALSE, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);

    // Negative tests.
    status = GdipCreateMetafileFromEmf (NULL, TRUE, &metafile);
    assertEqualInt (status, InvalidParameter);
    
    status = GdipCreateMetafileFromEmf (emfMetafile, TRUE, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_createMetafileFromWmf ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    WmfPlaceableFileHeader wmfPlaceableFileHeader;
    GpMetafile *metafile;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Create from WMF file.
    status = GdipCreateMetafileFromWmf (wmfMetafile, TRUE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);
    
    status = GdipCreateMetafileFromWmf (wmfMetafile, FALSE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);
    
    // Create from EMF file.
    status = GdipCreateMetafileFromWmf (emfMetafile, TRUE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);
    
    status = GdipCreateMetafileFromWmf (emfMetafile, FALSE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, Ok);
    GdipDisposeImage (metafile);

    // Negative tests.
    status = GdipCreateMetafileFromWmf (NULL, TRUE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, InvalidParameter);
    
    status = GdipCreateMetafileFromWmf (wmfMetafile, TRUE, NULL, &metafile);
    assertEqualInt (status, InvalidParameter);
    
    status = GdipCreateMetafileFromWmf (wmfMetafile, TRUE, &wmfPlaceableFileHeader, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_getMetafileHeaderFromWmf ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    WmfPlaceableFileHeader wmfPlaceableFileHeader;
    MetafileHeader header;
    
    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Get from WMF file.
    status = GdipGetMetafileHeaderFromWmf (wmfMetafile, &wmfPlaceableFileHeader, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type , 2);
    assertEqualInt (header.Size, 68142);
    assertEqualInt (header.Version, 768);
    assertEqualInt (header.EmfPlusFlags, 0);

    // FIXME: these values constantly fluctuate with libgdiplus - potential UB?
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualInt (header.DpiX, 0);
    assertEqualInt (header.DpiY, 0);
    assertEqualInt (header.X, 0);
    assertEqualInt (header.Y, 0);
    assertEqualInt (header.Width, 0);
    assertEqualInt (header.Height, 0);
#endif
    
    // Get from EMF file.
    status = GdipGetMetafileHeaderFromWmf (emfMetafile, &wmfPlaceableFileHeader, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type , 2);
    assertEqualInt (header.Size, 0);
    assertEqualInt (header.Version,  108);
    assertEqualInt (header.EmfPlusFlags, 0);
    // FIXME: these values constantly fluctuate with libgdiplus - potential UB?
#if defined(USE_WINDOWS_GDIPLUS)
    assert (header.DpiX > 0);
    assert (header.DpiY > 0);
    assertEqualInt (header.X, 0);
    assertEqualInt (header.Y, 0);
    assertEqualInt (header.Width, 0);
    assertEqualInt (header.Height, 0);
#endif

    // Negative tests.
    status = GdipGetMetafileHeaderFromWmf (NULL, &wmfPlaceableFileHeader, &header);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetMetafileHeaderFromWmf (wmfMetafile, NULL, &header);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetMetafileHeaderFromWmf (wmfMetafile, &wmfPlaceableFileHeader, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_getMetafileHeaderFromEmf ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    MetafileHeader header;
    
    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromEmf (emfMetafile, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type, 3);
    assertEqualInt (header.Size, 30284);
    assertEqualInt (header.Version, 65536);
    assertEqualInt (header.EmfPlusFlags, 0);
    assert (header.DpiX > 0);
    assert (header.DpiY > 0);
    assertEqualInt (header.X, 0);
    assertEqualInt (header.Y, 0);
    assertEqualInt (header.Width, 100);
    assertEqualInt (header.Height, 100);
    assertEqualInt (header.EmfPlusHeaderSize, 0);
    
    // Get from WMF file.
    status = GdipGetMetafileHeaderFromEmf (wmfMetafile, &header);
    assertEqualInt (status, InvalidParameter);

    // Negative tests.
    status = GdipGetMetafileHeaderFromEmf (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetMetafileHeaderFromEmf (wmfMetafile, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_getMetafileHeaderFromFile ()
{
    GpStatus status;
    MetafileHeader header;
    
    // Get from WMF file.
    status = GdipGetMetafileHeaderFromFile (wmfFilePath, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type, 2);
    assertEqualInt (header.Size, 68142);
    assertEqualInt (header.Version, 768);
    assertEqualInt (header.EmfPlusFlags, 0);
    assert (header.DpiX >= 0);
    assert (header.DpiY >= 0);
    assertEqualInt (header.X, -4008);
    assertEqualInt (header.Y, -3378);
    assertEqualInt (header.Width, 8016);
    assertEqualInt (header.Height, 6756);

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromFile (emfFilePath, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Size, 30284);
    assertEqualInt (header.Version, 65536);
    assertEqualInt (header.EmfPlusFlags, 0);
    assert (header.DpiX > 0);
    assert (header.DpiY > 0);
    assertEqualInt (header.X, 0);
    assertEqualInt (header.Y, 0);
    assertEqualInt (header.Width, 100);
    assertEqualInt (header.Height, 100);
    assertEqualInt (header.EmfPlusHeaderSize, 0);

    // Negative tests.
    status = GdipGetMetafileHeaderFromFile (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetMetafileHeaderFromFile (wmfFilePath, NULL);
    assertEqualInt (status, InvalidParameter);
}

static void test_getMetafileHeaderFromStream ()
{
    GpStatus status;
    MetafileHeader header;
#if defined(USE_WINDOWS_GDIPLUS)
    int temp = 0;
#endif
    
    // Negative tests.
    status = GdipGetMetafileHeaderFromStream (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    // Libgdiplus does not implement GdipGetMetafileHeaderFromStream.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromStream (&temp, NULL);
    assertEqualInt (status, InvalidParameter);
    
    status = GdipGetMetafileHeaderFromStream (&temp, &header);
    assertEqualInt (status, NotImplemented);
#endif
}

static void test_getMetafileHeaderFromMetafile ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    MetafileHeader header;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
    
    // Get from WMF file.
    status = GdipGetMetafileHeaderFromMetafile (wmfMetafile, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type, 2);
    assertEqualInt (header.Size, 68142);
    assertEqualInt (header.Version, 768);
    assertEqualInt (header.EmfPlusFlags, 0);
    assert (header.DpiX >= 0);
    assert (header.DpiY >= 0);
    assertEqualInt (header.X, -4008);
    assertEqualInt (header.Y, -3378);
    assertEqualInt (header.Width, 8016);
    assertEqualInt (header.Height, 6756);

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromMetafile (emfMetafile, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Size, 30284);
    assertEqualInt (header.Version, 65536);
    assertEqualInt (header.EmfPlusFlags, 0);
    assert (header.DpiX > 0);
    assert (header.DpiY > 0);
    assertEqualInt (header.X, 0);
    assertEqualInt (header.Y, 0);
    assertEqualInt (header.Width, 100);
    assertEqualInt (header.Height, 100);
    assertEqualInt (header.EmfPlusHeaderSize, 0);

    // Negative tests.
    status = GdipGetMetafileHeaderFromMetafile (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetMetafileHeaderFromMetafile (emfMetafile, NULL);
    assertEqualInt (status, InvalidParameter);
    
    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_gettHemfFromMetafile ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    GpMetafile *metafileFromEmf;
    HENHMETAFILE hemf;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
    GdipCreateMetafileFromEmf (emfMetafile, FALSE, &metafileFromEmf);

    // Get from WMF file.
    status = GdipGetHemfFromMetafile (wmfMetafile, &hemf);
    assertEqualInt (status, Ok);
    assert (hemf == wmfMetafile);

    // Get from EMF file.
    status = GdipGetHemfFromMetafile (emfMetafile, &hemf);
    assertEqualInt (status, Ok);
    assert (hemf == emfMetafile);

    // Get from EMF file from EMF file.
    status = GdipGetHemfFromMetafile (metafileFromEmf, &hemf);
    assertEqualInt (status, Ok);
    assert (hemf == metafileFromEmf);

    status = GdipGetHemfFromMetafile (metafileFromEmf, &hemf);
    assertEqualInt (status, Ok);
    assert (hemf == metafileFromEmf);

    // Negative tests.
    status = GdipGetHemfFromMetafile (NULL, &hemf);
    assertEqualInt (status, InvalidParameter);
    
    status = GdipGetHemfFromMetafile (NULL, &hemf);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
    GdipDisposeImage (metafileFromEmf);
}

static void test_getMetafileDownLevelRasterizationLimit ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    UINT metafileRasterizationLimitDpi;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Get from EMF file.
    status = GdipGetMetafileDownLevelRasterizationLimit (emfMetafile, &metafileRasterizationLimitDpi);
    assertEqualInt (status, Ok);
    assertEqualInt (metafileRasterizationLimitDpi, 0);

    // Get from WMF file.
    status = GdipGetMetafileDownLevelRasterizationLimit (wmfMetafile, &metafileRasterizationLimitDpi);
    assertEqualInt (status, WrongState);

    // Negative tests.
    status = GdipGetMetafileDownLevelRasterizationLimit (NULL, &metafileRasterizationLimitDpi);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetMetafileDownLevelRasterizationLimit (emfMetafile, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_setMetafileDownLevelRasterizationLimit ()
{
    GpStatus status;
    GpMetafile *emfMetafile;
    GpMetafile *wmfMetafile;
    UINT metafileRasterizationLimitDpi;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Get from EMF file.
    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, 5);
    assertEqualInt (status, Ok);

    status = GdipGetMetafileDownLevelRasterizationLimit (emfMetafile, &metafileRasterizationLimitDpi);
    assertEqualInt (status, Ok);
    assertEqualInt (metafileRasterizationLimitDpi, 0);

    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, 15);
    assertEqualInt (status, Ok);

    status = GdipGetMetafileDownLevelRasterizationLimit (emfMetafile, &metafileRasterizationLimitDpi);
    assertEqualInt (status, Ok);
    assertEqualInt (metafileRasterizationLimitDpi, 0);

    // Get from WMF file.
    status = GdipSetMetafileDownLevelRasterizationLimit (wmfMetafile, 10);
    assertEqualInt (status, WrongState);

    // Negative tests.
    status = GdipSetMetafileDownLevelRasterizationLimit (NULL, 5);
    assertEqualInt (status, InvalidParameter);

    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, -1);
    assertEqualInt (status, Ok);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_playMetafileRecord ()
{
    GpStatus status;
    GpMetafile *emfMetafile;
    GpMetafile *wmfMetafile;
    BYTE data[] = {1};

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Negative tests.
    status = GdipPlayMetafileRecord (NULL, EmfPlusRecordTypeClear, 0, 1, data);
    assertEqualInt (status, InvalidParameter);

    status = GdipPlayMetafileRecord (emfMetafile, EmfPlusRecordTypeClear, 0, 1, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_recordMetafile ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    GpImage *bitmap;
    GpGraphics *graphics;
    HDC hdc;
    GpRectF rect = {5, 10, 30, 40};
    GpRectF zeroWidth = {0, 0, 0, 100};
    GpRectF zeroHeight = {0, 0, 100, 0};
    GpMetafile *metafile;
    
    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
    
    GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat32bppRGB, NULL, &bitmap);
    GdipGetImageGraphicsContext (bitmap, &graphics);
    GdipGetDC (graphics, &hdc);

    // Negative tests.
    status = GdipRecordMetafile (NULL, EmfTypeEmfPlusDual, &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, (EmfType)(EmfTypeEmfOnly - 1), &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, (EmfType)(EmfTypeEmfPlusDual + 1), &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &rect, (MetafileFrameUnit)(MetafileFrameUnitPixel - 1), wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &rect, (MetafileFrameUnit)(MetafileFrameUnitGdi + 1), wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &zeroWidth, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, GenericError);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &zeroHeight, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, GenericError);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &rect, MetafileFrameUnitPixel, wmfFilePath, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
    GdipDisposeImage (bitmap);
    GdipDeleteGraphics (graphics);
}

int
main (int argc, char**argv)
{
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

    test_createMetafileFromFile ();
    test_createMetafileFromStream ();
    test_createMetafileFromEmf ();
    test_createMetafileFromWmf ();
    test_getMetafileHeaderFromWmf ();
    test_getMetafileHeaderFromEmf ();
    test_getMetafileHeaderFromFile ();
    test_getMetafileHeaderFromStream ();
    test_getMetafileHeaderFromMetafile ();
    test_gettHemfFromMetafile ();
    test_getMetafileDownLevelRasterizationLimit ();
    test_setMetafileDownLevelRasterizationLimit ();
    test_playMetafileRecord ();
    test_recordMetafile ();

    GdiplusShutdown (gdiplusToken);
    return 0;
}
