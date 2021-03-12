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

static WCHAR wmfFilePath[] = {'t', 'e', 's', 't', '.', 'w', 'm', 'f', 0};
static WCHAR emfFilePath[] = {'t', 'e', 's', 't', '.', 'e', 'm', 'f', 0};

TEST(Metafile, GdipCreateMetafileFromFile)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    verifyImage(metafile, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 2, 2, 0.017323f, 0.016807f, 0.458334f, 0.444445f, 327683, 0, TRUE);
#endif
    GdipDisposeImage (metafile);

    // Create from EMF file.
    status = GdipCreateMetafileFromFile (emfFilePath, &metafile);
    ASSERT_EQ (Ok, status);
    verifyMetafile (metafile, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);
    GdipDisposeImage (metafile);

    // Negative tests.
    status = GdipCreateMetafileFromFile (NULL, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreateMetafileFromFile (noSuchFilePath, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreateMetafileFromFile (noSuchFilePath, &metafile);
    ASSERT_EQ (GenericError, status);

    status = GdipCreateMetafileFromFile (invalidFilePath, &metafile);
    ASSERT_EQ (GenericError, status);

    status = GdipCreateMetafileFromFile (bitmapFilePath, &metafile);
    ASSERT_EQ (GenericError, status);

    freeWchar (noSuchFilePath);
    freeWchar (invalidFilePath);
    freeWchar (bitmapFilePath);

    SHUTDOWN
}

TEST(Metafile, GdipCreateMetafileFromStream)
{
    STARTUP

    GpStatus status;
    GpMetafile *metafile;
#if !defined(USE_WINDOWS_GDIPLUS)
    int temp = 0;
#endif

    // Negative tests.
    status = GdipCreateMetafileFromStream (NULL, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    // Libgdiplus does not implement GdipCreateMetafileFromStream.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipCreateMetafileFromStream (&temp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreateMetafileFromStream (&temp, &metafile);
    ASSERT_EQ (NotImplemented, status);
#endif

SHUTDOWN
}

TEST(Metafile, GdipCreateMetafileFromEmf)
{
    STARTUP

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
    ASSERT_EQ (GenericError, status);
    assert (!metafile);

    metafile = (GpMetafile *) 0xCC;
    status = GdipCreateMetafileFromEmf (hWmfMetafile, FALSE, &metafile);
    ASSERT_EQ (GenericError, status);
    assert (!metafile);

    // Create from a EMF file.
    status = GdipCreateMetafileFromEmf (hEmfMetafile, FALSE, &metafile);
    ASSERT_EQ (Ok, status);
    verifyMetafile (metafile, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);
    GdipDisposeImage (metafile);

    status = GdipCreateMetafileFromEmf (hEmfMetafile, TRUE, &metafile);
    ASSERT_EQ (Ok, status);
    verifyMetafile (metafile, emfRawFormat, 0, 0, 100, 100, 1944.444336f, 1888.888794f);
    GdipDisposeImage (metafile);

    // Negative tests.
    status = GdipCreateMetafileFromEmf (NULL, TRUE, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreateMetafileFromEmf (hEmfMetafile, TRUE, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);

    SHUTDOWN
}

TEST(Metafile, GdipCreateMetafileFromWmf)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    verifyImage(metafile, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 2, 2, 0.017323f, 0.016807f, 0.458334f, 0.444445f, 327683, 0, TRUE);
#endif
    GdipDisposeImage (metafile);

    status = GdipCreateMetafileFromWmf (hWmfMetafile, TRUE, NULL, &metafile);
    ASSERT_EQ (Ok, status);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    verifyImage(metafile, ImageTypeMetafile, emfRawFormat, PixelFormat32bppRGB, 0, 0, 2, 2, 0.017323f, 0.016807f, 0.458334f, 0.444445f, 327683, 0, TRUE);
#endif
    GdipDisposeImage (metafile);

    // Create from EMF file.
    metafile = (GpMetafile *) 0xCC;
    status = GdipCreateMetafileFromWmf (hEmfMetafile, TRUE, &wmfPlaceableFileHeader, &metafile);
    ASSERT_EQ (GenericError, status);
    assert (!metafile);

    metafile = (GpMetafile *) 0xCC;
    status = GdipCreateMetafileFromWmf (hEmfMetafile, FALSE, &wmfPlaceableFileHeader, &metafile);
    ASSERT_EQ (GenericError, status);
    assert (!metafile);

    // Negative tests.
    status = GdipCreateMetafileFromWmf (NULL, TRUE, &wmfPlaceableFileHeader, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipCreateMetafileFromWmf (hWmfMetafile, TRUE, &wmfPlaceableFileHeader, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);

    SHUTDOWN
}

TEST(Metafile, GdipGetMetafileHeaderFromWmf)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, header.Type);
    ASSERT_EQ (68142, header.Size);
    ASSERT_EQ (768, header.Version);
    ASSERT_EQ (0, header.EmfPlusFlags);

    // FIXME: these values constantly fluctuate with libgdiplus - potential UB?
#if defined(USE_WINDOWS_GDIPLUS)
    ASSERT_EQ (0, header.DpiX);
    ASSERT_EQ (0, header.DpiY);
    ASSERT_EQ (0, header.X);
    ASSERT_EQ (0, header.Y);
    ASSERT_EQ (0, header.Width);
    ASSERT_EQ (0, header.Height);
#endif

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromWmf (hEmfMetafile, &wmfPlaceableFileHeader, &header);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (2, header.Type );
    ASSERT_EQ (0, header.Size);
    ASSERT_EQ ( 108, header.Version);
    ASSERT_EQ (0, header.EmfPlusFlags);
    // FIXME: these values constantly fluctuate with libgdiplus - potential UB?
#if defined(USE_WINDOWS_GDIPLUS)
    assert (header.DpiX > 0);
    assert (header.DpiY > 0);
    ASSERT_EQ (0, header.X);
    ASSERT_EQ (0, header.Y);
    ASSERT_EQ (0, header.Width);
    ASSERT_EQ (0, header.Height);
#endif

    // Negative tests.
    status = GdipGetMetafileHeaderFromWmf (NULL, &wmfPlaceableFileHeader, &header);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetMetafileHeaderFromWmf (hWmfMetafile, NULL, &header);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetMetafileHeaderFromWmf (hWmfMetafile, &wmfPlaceableFileHeader, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
#endif

SHUTDOWN
}

TEST(Metafile, GdipGetMetafileHeaderFromEmf)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (3, header.Type);
    ASSERT_EQ (30284, header.Size);
    ASSERT_EQ (65536, header.Version);
    ASSERT_EQ (0, header.EmfPlusFlags);
    assert (header.DpiX > 0);
    assert (header.DpiY > 0);
    ASSERT_EQ (0, header.X);
    ASSERT_EQ (0, header.Y);
    ASSERT_EQ (100, header.Width);
    ASSERT_EQ (100, header.Height);
    ASSERT_EQ (0, header.EmfPlusHeaderSize);

    // Get from WMF file.
    status = GdipGetMetafileHeaderFromEmf (hWmfMetafile, &header);
    ASSERT_EQ (InvalidParameter, status);

    // Negative tests.
    status = GdipGetMetafileHeaderFromEmf (NULL, &header);
    ASSERT_EQ (InvalidParameter, status);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromEmf (hWmfMetafile, NULL);
    ASSERT_EQ (InvalidParameter, status);
#endif

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);

    SHUTDOWN
}

TEST(Metafile, GdipGetMetafileHeaderFromFile)
{
    STARTUP

    GpStatus status;
    MetafileHeader header;

    // Get from WMF file.
    status = GdipGetMetafileHeaderFromFile (wmfFilePath, &header);
    ASSERT_EQ (Ok, status);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    ASSERT_EQ (MetafileTypeEmf, header.Type);
    ASSERT_EQ (256, header.Size);
    ASSERT_EQ (65536, header.Version);
    ASSERT_EQ (0, header.EmfPlusFlags);
    ASSERT_FLOAT_EQ_NAN (95.9999924, header.DpiX);
    ASSERT_FLOAT_EQ_NAN (96.0504150, header.DpiY);
    ASSERT_EQ (-1, header.X);
    ASSERT_EQ (-1, header.Y);
    ASSERT_EQ (2, header.Width);
    ASSERT_EQ (2, header.Height);
#endif

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromFile (emfFilePath, &header);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (MetafileTypeEmf, header.Type);
    ASSERT_EQ (30284, header.Size);
    ASSERT_EQ (65536, header.Version);
    ASSERT_EQ (0, header.EmfPlusFlags);
    ASSERT_FLOAT_EQ_NAN (130.628555f, header.DpiX);
    ASSERT_FLOAT_EQ_NAN (134.470581f, header.DpiY);
    ASSERT_EQ (0, header.X);
    ASSERT_EQ (0, header.Y);
    ASSERT_EQ (100, header.Width);
    ASSERT_EQ (100, header.Height);
    ASSERT_EQ (0, header.EmfPlusHeaderSize);

    // Negative tests.
    status = GdipGetMetafileHeaderFromFile (NULL, &header);
    ASSERT_EQ (InvalidParameter, status);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromFile (wmfFilePath, NULL);
    ASSERT_EQ (InvalidParameter, status);
#endif

SHUTDOWN
}

TEST(Metafile, GdipGetMetafileHeaderFromStream)
{
    STARTUP

    GpStatus status;
    MetafileHeader header;
#if !defined(USE_WINDOWS_GDIPLUS)
    int temp = 0;
#endif

    // Negative tests.
    status = GdipGetMetafileHeaderFromStream (NULL, &header);
    ASSERT_EQ (InvalidParameter, status);

    // Libgdiplus does not implement GdipGetMetafileHeaderFromStream.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromStream (&temp, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetMetafileHeaderFromStream (&temp, &header);
    ASSERT_EQ (NotImplemented, status);
#endif

SHUTDOWN
}

TEST(Metafile, GdipGetMetafileHeaderFromMetafile)
{
    STARTUP

    GpStatus status;
    GpMetafile *wmfMetafile;
    GpMetafile *emfMetafile;
    MetafileHeader header;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Get from WMF file.
    status = GdipGetMetafileHeaderFromMetafile (wmfMetafile, &header);
    ASSERT_EQ (Ok, status);
    // FIXME: GDI+ converts wmf files to emf.
#if defined(USE_WINDOWS_GDIPLUS)
    ASSERT_EQ (MetafileTypeEmf, header.Type);
    ASSERT_EQ (256, header.Size);
    ASSERT_EQ (65536, header.Version);
    ASSERT_EQ (0, header.EmfPlusFlags);
    ASSERT_FLOAT_EQ_NAN (95.9999924, header.DpiX);
    ASSERT_FLOAT_EQ_NAN (96.0504150, header.DpiY);
    ASSERT_EQ (-1, header.X);
    ASSERT_EQ (-1, header.Y);
    ASSERT_EQ (2, header.Width);
    ASSERT_EQ (2, header.Height);
#endif

    // Get from EMF file.
    status = GdipGetMetafileHeaderFromMetafile (emfMetafile, &header);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (MetafileTypeEmf, header.Type);
    ASSERT_EQ (30284, header.Size);
    ASSERT_EQ (65536, header.Version);
    ASSERT_EQ (0, header.EmfPlusFlags);
    ASSERT_FLOAT_EQ_NAN (130.628555f, header.DpiX);
    ASSERT_FLOAT_EQ_NAN (134.470581f, header.DpiY);
    ASSERT_EQ (0, header.X);
    ASSERT_EQ (0, header.Y);
    ASSERT_EQ (100, header.Width);
    ASSERT_EQ (100, header.Height);
    ASSERT_EQ (0, header.EmfPlusHeaderSize);

    // Negative tests.
    status = GdipGetMetafileHeaderFromMetafile (NULL, &header);
    ASSERT_EQ (InvalidParameter, status);

    // This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetMetafileHeaderFromMetafile (emfMetafile, NULL);
    ASSERT_EQ (InvalidParameter, status);
#endif

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);

    SHUTDOWN
}

TEST(Metafile, GdipGetHemfFromMetafile)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
    assert (hemf == wmfMetafile);

    // Get from EMF file.
    status = GdipGetHemfFromMetafile (emfMetafile, &hemf);
    ASSERT_EQ (Ok, status);
    assert (hemf == emfMetafile);

    // Get from EMF file from EMF file.
    status = GdipGetHemfFromMetafile (metafileFromEmf, &hemf);
    ASSERT_EQ (Ok, status);
    assert (hemf == metafileFromEmf);

    status = GdipGetHemfFromMetafile (metafileFromEmf, &hemf);
    ASSERT_EQ (Ok, status);
    assert (hemf == metafileFromEmf);
#endif

    // Negative tests.
    status = GdipGetHemfFromMetafile (NULL, &hemf);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetHemfFromMetafile (NULL, &hemf);
    ASSERT_EQ (InvalidParameter, status);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
    GdipDisposeImage (metafileFromEmf);

    SHUTDOWN
}

TEST(Metafile, GdipGetMetafileDownLevelRasterizationLimit)
{
    STARTUP

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
    ASSERT_EQ (WrongState, status);
#endif

    // Get from WMF file.
    status = GdipGetMetafileDownLevelRasterizationLimit (wmfMetafile, &metafileRasterizationLimitDpi);
    ASSERT_EQ (WrongState, status);

    // Negative tests.
    status = GdipGetMetafileDownLevelRasterizationLimit (NULL, &metafileRasterizationLimitDpi);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetMetafileDownLevelRasterizationLimit (emfMetafile, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);

    SHUTDOWN
}

TEST(Metafile, GdipSetMetafileDownLevelRasterizationLimit)
{
    STARTUP

    GpStatus status;
    GpMetafile *emfMetafile;
    GpMetafile *wmfMetafile;

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Get from EMF file.
    // FIXME: GDI+ returns InvalidParameter/WrongState.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, 5);
    ASSERT_EQ (InvalidParameter, status);
#
    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, 15);
    ASSERT_EQ (WrongState, status);
#endif

    // Get from WMF file.
    status = GdipSetMetafileDownLevelRasterizationLimit (wmfMetafile, 10);
    ASSERT_EQ (WrongState, status);

    // Negative tests.
    status = GdipSetMetafileDownLevelRasterizationLimit (NULL, 5);
    ASSERT_EQ (InvalidParameter, status);

    // FIXME: GDI+ returns WrongState.
#if defined(USE_WINDOWS_GDIPLUS)
    status = GdipSetMetafileDownLevelRasterizationLimit (emfMetafile, -1);
    ASSERT_EQ (WrongState, status);
#endif

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);

    SHUTDOWN
}

TEST(Metafile, GdipPlayMetafileRecord)
{
    STARTUP

    GpStatus status;
    GpMetafile *emfMetafile;
    GpMetafile *wmfMetafile;
    BYTE data[] = {1};

    GdipCreateMetafileFromFile (wmfFilePath, &wmfMetafile);
    GdipCreateMetafileFromFile (emfFilePath, &emfMetafile);

    // Negative tests.
    status = GdipPlayMetafileRecord (NULL, EmfPlusRecordTypeClear, 0, 1, data);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipPlayMetafileRecord (emfMetafile, EmfPlusRecordTypeClear, 0, 1, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);

    SHUTDOWN
}

TEST(Metafile, GdipRecordMetafile)
{
    STARTUP

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
    ASSERT_EQ (Ok, status);
#endif

    // Negative tests.
    status = GdipRecordMetafile (NULL, EmfTypeEmfPlusDual, &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRecordMetafile (hdc, (EmfType)(EmfTypeEmfOnly - 1), &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRecordMetafile (hdc, (EmfType)(EmfTypeEmfPlusDual + 1), &rect, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &rect, (MetafileFrameUnit)(MetafileFrameUnitPixel - 1), wmfFilePath, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &rect, (MetafileFrameUnit)(MetafileFrameUnitGdi + 1), wmfFilePath, &metafile);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &zeroWidth, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    ASSERT_EQ (GenericError, status);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &zeroHeight, MetafileFrameUnitPixel, wmfFilePath, &metafile);
    ASSERT_EQ (GenericError, status);

    status = GdipRecordMetafile (hdc, EmfTypeEmfPlusDual, &rect, MetafileFrameUnitPixel, wmfFilePath, NULL);
    ASSERT_EQ (InvalidParameter, status);

    GdipReleaseDC (graphics, hdc);
    GdipDisposeImage (wmfMetafile);
    GdipDisposeImage (emfMetafile);
    GdipDisposeImage (bitmap);
    GdipDeleteGraphics (graphics);

    SHUTDOWN
}
