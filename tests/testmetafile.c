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
    WCHAR *bitmapFilePath;
    GpMetafile *metafile;

    noSuchFilePath = createWchar ("noSuchFile.wmf");
    invalidFilePath = createWchar ("test.ttf");
    bitmapFilePath = createWchar ("test.bmp");

    // Create from WMF file.
    status = GdipCreateMetafileFromFile (wmfFilePath, &metafile);
    assertEqualInt (status, Ok);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    verifyImage(metafile, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 2, 2, 0.017323f, 0.016807f, 0.458334f, 0.444445f, 327683, 0, TRUE);
#endif
    GdipDisposeImage (metafile);

    // Create from EMF file.
    status = GdipCreateMetafileFromFile (emfFilePath, &metafile);
    assertEqualInt (status, Ok);
    verifyMetafile (metafile, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);
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

    status = GdipCreateMetafileFromFile (bitmapFilePath, &metafile);
    assertEqualInt (status, GenericError);
}

static void test_createMetafileFromStream ()
{
    GpStatus status;
    GpMetafile *metafile;
#if !defined(USE_WINDOWS_GDIPLUS)
    int temp = 0;
#endif

    // Negative tests.
    status = GdipCreateMetafileFromStream (NULL, &metafile);
    assertEqualInt (status, InvalidParameter);

    // Libgdiplus does not implement GdipCreateMetafileFromStream.
#if !defined(USE_WINDOWS_GDIPLUS)
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
    HENHMETAFILE hWmfMetafile;
    HENHMETAFILE hEmfMetafile;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
#if defined(USE_WINDOWS_GDIPLUS)
    HDC wmfHdc = CreateMetaFileA (NULL);
    hWmfMetafile = (HENHMETAFILE) CloseMetaFile (wmfHdc);
    CopyMetaFileA ((HMETAFILE) hWmfMetafile, "test.wmf");
#else
    GdipGetHemfFromMetafile (wmfMetafile, &hWmfMetafile);
#endif
    GdipGetHemfFromMetafile (emfMetafile, &hEmfMetafile);

    // Create from a WMF file.
    metafile = (GpMetafile *) 0xCC;
    status = GdipCreateMetafileFromEmf (hWmfMetafile, TRUE, &metafile);
    assertEqualInt (status, GenericError);
    assert (!metafile);

    metafile = (GpMetafile *) 0xCC;
    status = GdipCreateMetafileFromEmf (hWmfMetafile, FALSE, &metafile);
    assertEqualInt (status, GenericError);
    assert (!metafile);

    // Create from a EMF file.
    status = GdipCreateMetafileFromEmf (hEmfMetafile, FALSE, &metafile);
    assertEqualInt (status, Ok);
    verifyMetafile (metafile, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);
    GdipDisposeImage (metafile);

    status = GdipCreateMetafileFromEmf (hEmfMetafile, TRUE, &metafile);
    assertEqualInt (status, Ok);
    verifyMetafile (metafile, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);
    GdipDisposeImage (metafile);

    // Negative tests.
    status = GdipCreateMetafileFromEmf (NULL, TRUE, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreateMetafileFromEmf (hEmfMetafile, TRUE, NULL);
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
    HMETAFILE hWmfMetafile;
    HMETAFILE hEmfMetafile;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
#if defined(USE_WINDOWS_GDIPLUS)
    HDC wmfHdc = CreateMetaFileA (NULL);
    hWmfMetafile = CloseMetaFile (wmfHdc);
    CopyMetaFileA (hWmfMetafile, "test.wmf");
#else
    GdipGetHemfFromMetafile (wmfMetafile, &hWmfMetafile);
#endif
    GdipGetHemfFromMetafile (emfMetafile, (HENHMETAFILE *) &hEmfMetafile);

    // Create from WMF file - no placeable header.
    status = GdipCreateMetafileFromWmf (hWmfMetafile, FALSE, NULL, &metafile);
    assertEqualInt (status, Ok);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    verifyImage(metafile, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 2, 2, 0.017323f, 0.016807f, 0.458334f, 0.444445f, 327683, 0, TRUE);
#endif
    GdipDisposeImage (metafile);

    status = GdipCreateMetafileFromWmf (hWmfMetafile, TRUE, NULL, &metafile);
    assertEqualInt (status, Ok);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    verifyImage(metafile, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 2, 2, 0.017323f, 0.016807f, 0.458334f, 0.444445f, 327683, 0, TRUE);
#endif
    GdipDisposeImage (metafile);

    // Create from EMF file.
    metafile = (GpMetafile *) 0xCC;
    status = GdipCreateMetafileFromWmf (hEmfMetafile, TRUE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, GenericError);
    assert (!metafile);

    metafile = (GpMetafile *) 0xCC;
    status = GdipCreateMetafileFromWmf (hEmfMetafile, FALSE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, GenericError);
    assert (!metafile);

    // Negative tests.
    status = GdipCreateMetafileFromWmf (NULL, TRUE, &wmfPlaceableFileHeader, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipCreateMetafileFromWmf (hWmfMetafile, TRUE, &wmfPlaceableFileHeader, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_getMetafileHeaderFromWmf ()
{
#if !defined(USE_WINDOWS_GDIPLUS)
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    WmfPlaceableFileHeader wmfPlaceableFileHeader;
    MetafileHeader header;
    HMETAFILE hWmfMetafile;
    HMETAFILE hEmfMetafile;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
#if defined(USE_WINDOWS_GDIPLUS)
    HDC wmfHdc = CreateMetaFileA (NULL);
    hWmfMetafile = CloseMetaFile (wmfHdc);
    CopyMetaFileA (hWmfMetafile, "test.wmf");
#else
    GdipGetHemfFromMetafile (wmfMetafile, &hWmfMetafile);
#endif
    GdipGetHemfFromMetafile (emfMetafile, (HENHMETAFILE *) &hEmfMetafile);

    // Get from WMF file.
    status = GdipGetMetafileHeaderFromWmf (hWmfMetafile, &wmfPlaceableFileHeader, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type, 2);
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
    status = GdipGetMetafileHeaderFromWmf (hEmfMetafile, &wmfPlaceableFileHeader, &header);
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

    status = GdipGetMetafileHeaderFromWmf (hWmfMetafile, NULL, &header);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetMetafileHeaderFromWmf (hWmfMetafile, &wmfPlaceableFileHeader, NULL);
    assertEqualInt (status, InvalidParameter);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
#endif
}

static void test_getMetafileHeaderFromEmf ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    HENHMETAFILE hWmfMetafile;
    HENHMETAFILE hEmfMetafile;
    MetafileHeader header;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
#if defined(USE_WINDOWS_GDIPLUS)
    HDC wmfHdc = CreateMetaFileA (NULL);
    hWmfMetafile = (HENHMETAFILE) CloseMetaFile (wmfHdc);
    CopyMetaFileA ((HMETAFILE) hWmfMetafile, "test.wmf");
#else
    GdipGetHemfFromMetafile (wmfMetafile, &hWmfMetafile);
#endif
    GdipGetHemfFromMetafile (emfMetafile, &hEmfMetafile);

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromEmf (hEmfMetafile, &header);
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
    status = GdipGetMetafileHeaderFromEmf (hWmfMetafile, &header);
    assertEqualInt (status, InvalidParameter);

    // Negative tests.
    status = GdipGetMetafileHeaderFromEmf (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromEmf (hWmfMetafile, NULL);
    assertEqualInt (status, InvalidParameter);
#endif

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
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualInt (header.Type, MetafileTypeEmf);
    assertEqualInt (header.Size, 256);
    assertEqualInt (header.Version, 65536);
    assertEqualInt (header.EmfPlusFlags, 0);
    assertEqualFloat (header.DpiX, 95.9999924);
    assertEqualFloat (header.DpiY, 96.0504150);
    assertEqualInt (header.X, -1);
    assertEqualInt (header.Y, -1);
    assertEqualInt (header.Width, 2);
    assertEqualInt (header.Height, 2);
#endif

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromFile (emfFilePath, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type, MetafileTypeEmf);
    assertEqualInt (header.Size, 30284);
    assertEqualInt (header.Version, 65536);
    assertEqualInt (header.EmfPlusFlags, 0);
    assertEqualFloat (header.DpiX, 130.628555f);
    assertEqualFloat (header.DpiY, 134.470581f);
    assertEqualInt (header.X, 0);
    assertEqualInt (header.Y, 0);
    assertEqualInt (header.Width, 100);
    assertEqualInt (header.Height, 100);
    assertEqualInt (header.EmfPlusHeaderSize, 0);

    // Negative tests.
    status = GdipGetMetafileHeaderFromFile (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromFile (wmfFilePath, NULL);
    assertEqualInt (status, InvalidParameter);
#endif
}

static void test_getMetafileHeaderFromStream ()
{
    GpStatus status;
    MetafileHeader header;
#if !defined(USE_WINDOWS_GDIPLUS)
    int temp = 0;
#endif

    // Negative tests.
    status = GdipGetMetafileHeaderFromStream (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    // Libgdiplus does not implement GdipGetMetafileHeaderFromStream.
#if !defined(USE_WINDOWS_GDIPLUS)
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
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    assertEqualInt (header.Type, MetafileTypeEmf);
    assertEqualInt (header.Size, 256);
    assertEqualInt (header.Version, 65536);
    assertEqualInt (header.EmfPlusFlags, 0);
    assertEqualFloat (header.DpiX, 95.9999924);
    assertEqualFloat (header.DpiY, 96.0504150);
    assertEqualInt (header.X, -1);
    assertEqualInt (header.Y, -1);
    assertEqualInt (header.Width, 2);
    assertEqualInt (header.Height, 2);
#endif

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromMetafile (emfMetafile, &header);
    assertEqualInt (status, Ok);
    assertEqualInt (header.Type, MetafileTypeEmf);
    assertEqualInt (header.Size, 30284);
    assertEqualInt (header.Version, 65536);
    assertEqualInt (header.EmfPlusFlags, 0);
    assertEqualFloat (header.DpiX, 130.628555f);
    assertEqualFloat (header.DpiY, 134.470581f);
    assertEqualInt (header.X, 0);
    assertEqualInt (header.Y, 0);
    assertEqualInt (header.Width, 100);
    assertEqualInt (header.Height, 100);
    assertEqualInt (header.EmfPlusHeaderSize, 0);

    // Negative tests.
    status = GdipGetMetafileHeaderFromMetafile (NULL, &header);
    assertEqualInt (status, InvalidParameter);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromMetafile (emfMetafile, NULL);
    assertEqualInt (status, InvalidParameter);
#endif

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
}

static void test_gettHemfFromMetafile ()
{
    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    HENHMETAFILE hEmfMetafile;
    GpMetafile *metafileFromEmf;
    HENHMETAFILE hemf;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);
    GdipGetHemfFromMetafile (emfMetafile, &hEmfMetafile);
    GdipCreateMetafileFromEmf (hEmfMetafile, FALSE, &metafileFromEmf);

    // The definition of HENHMETAFILE is different between libgdiplus and GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
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
#endif

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
    // FIXME: GDI+ returns WrongState.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileDownLevelRasterizationLimit (emfMetafile, &metafileRasterizationLimitDpi);
    assertEqualInt (status, WrongState);
#endif

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

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Get from EMF file.
    // FIXME: GDI+ returns InvalidParameter/WrongState.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, 5);
    assertEqualInt (status, InvalidParameter);
#
    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, 15);
    assertEqualInt (status, WrongState);
#endif

    // Get from WMF file.
    status = GdipSetMetafileDownLevelRasterizationLimit (wmfMetafile, 10);
    assertEqualInt (status, WrongState);

    // Negative tests.
    status = GdipSetMetafileDownLevelRasterizationLimit (NULL, 5);
    assertEqualInt (status, InvalidParameter);

    // FIXME: GDI+ returns WrongState.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, -1);
    assertEqualInt (status, WrongState);
#endif

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

    GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat32bppRGB, NULL, (GpBitmap **) &bitmap);
    GdipGetImageGraphicsContext (bitmap, &graphics);
    GdipGetDC (graphics, &hdc);

    // FIXME: this returns Ok with GDI+.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, NULL, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, Ok);
#endif

    // Negative tests.
    status = GdipRecordMetafile (NULL, EmfTypeEmfPlusDual, &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, (EmfType)(EmfTypeEmfOnly - 1), &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    assertEqualInt (status, InvalidParameter);

    status = GdipRecordMetafile (hdc, (EmfType)(EmfTypeEmfPlusDual + 1), &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
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
    STARTUP;

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

    SHUTDOWN;
    return 0;
}
