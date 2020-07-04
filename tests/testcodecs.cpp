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

#define verifyImageCodecInfo(codec, expectedClsid, expectedFormatID, expectedCodecName, expectedFormatDescription, expectedFilenameExtension, expectedMimeType, expectedFlags, expectedVersion, expectedSigCount, expectedSigPattern, expectedSigMask) \
{ \
    assertEqualGuid (codec.Clsid, expectedClsid); \
    assertEqualGuid (codec.FormatID, expectedFormatID); \
    assertEqualString (codec.CodecName, expectedCodecName); \
    assert (codec.DllName == NULL); \
    assertEqualString (codec.FormatDescription, expectedFormatDescription); \
    assertEqualString (codec.FilenameExtension, expectedFilenameExtension); \
    assertEqualString (codec.MimeType, expectedMimeType); \
    ASSERT_EQ (expectedFlags, codec.Flags); \
    ASSERT_EQ (expectedVersion, codec.Version); \
    ASSERT_EQ (expectedSigCount, codec.SigCount); \
    ASSERT_EQ (sizeof (expectedSigPattern) / expectedSigCount, codec.SigSize); \
    assertEqualBytes (codec.SigPattern, expectedSigPattern, codec.SigSize); \
    assertEqualBytes (codec.SigMask, expectedSigMask, codec.SigSize); \
}

TEST(Codecs, GdipGetImageDecodersSize)
{
    STARTUP

    GpStatus status;
    UINT numDecoders;
    UINT size;

    status = GdipGetImageDecodersSize (&numDecoders, &size);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (8, numDecoders);
    //ASSERT_EQ (8 * sizeof (ImageCodecInfo), size);

    // Negative tests.
    // FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetImageDecodersSize (&numDecoders, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetImageDecodersSize (NULL, &size);
    ASSERT_EQ (InvalidParameter, status);
#endif

    SHUTDOWN
}

TEST(Codecs, GdipGetImageDecoders)
{
    STARTUP

    GpStatus status;
    UINT numDecoders;
    UINT size;
    ImageCodecInfo *codecs;

    GdipGetImageDecodersSize (&numDecoders, &size);
    codecs = (ImageCodecInfo *) malloc (size);

    status = GdipGetImageDecoders (numDecoders, size, codecs);
    ASSERT_EQ (Ok, status);

    BYTE bmpSigPattern[] = {0x42, 0x4D};
    BYTE bmpSigMask[] = {0xFF, 0xFF};
    verifyImageCodecInfo (codecs[0], bmpEncoderClsid, bmpRawFormat, "Built-in BMP Codec", "BMP", "*.BMP;*.DIB;*.RLE", "image/bmp", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, bmpSigPattern, bmpSigMask);

    BYTE jpegSigPattern[] = {0xFF, 0xD8};
    BYTE jpegSigMask[] = {0xFF, 0xFF};
    verifyImageCodecInfo (codecs[1], jpegEncoderClsid, jpegRawFormat, "Built-in JPEG Codec", "JPEG", "*.JPG;*.JPEG;*.JPE;*.JFIF", "image/jpeg", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, jpegSigPattern, jpegSigMask);

    BYTE gifSigPattern[] = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x47, 0x49, 0x46, 0x38, 0x37, 0x61};
    BYTE gifSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[2], gifEncoderClsid, gifRawFormat, "Built-in GIF Codec", "GIF", "*.GIF", "image/gif", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 2, gifSigPattern, gifSigMask);

    BYTE emfSigPattern[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x45, 0x4D, 0x46};
    BYTE emfSigMask[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[3], emfEncoderClsid, emfRawFormat, "Built-in EMF Codec", "EMF", "*.EMF", "image/x-emf", ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, emfSigPattern, emfSigMask);

    BYTE wmfSigPattern[] = {0xD7, 0xCD, 0xC6, 0x9A};
    BYTE wmfSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[4], wmfEncoderClsid, wmfRawFormat, "Built-in WMF Codec", "WMF", "*.WMF", "image/x-wmf", ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, wmfSigPattern, wmfSigMask);

    BYTE tiffSigPattern[] = {0x49, 0x49, 0x4D, 0x4D};
    BYTE tiffSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[5], tifEncoderClsid, tifRawFormat, "Built-in TIFF Codec", "TIFF", "*.TIF;*.TIFF", "image/tiff", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 2, tiffSigPattern, tiffSigMask);

    BYTE pngSigPattern[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    BYTE pngSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[6], pngEncoderClsid, pngRawFormat, "Built-in PNG Codec", "PNG", "*.PNG", "image/png", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, pngSigPattern, pngSigMask);

    BYTE icoSigPattern[] = {0x00, 0x00, 0x01, 0x00};
    BYTE icoSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[7], icoEncoderClsid, icoRawFormat, "Built-in ICO Codec", "ICO", "*.ICO", "image/x-icon", ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, icoSigPattern, icoSigMask);

    // Negative tests.
    status = GdipGetImageDecoders (0, size, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageDecoders (numDecoders - 1, size, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageDecoders (numDecoders + 1, size, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageDecoders (numDecoders, 0, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageDecoders (numDecoders, size - 1, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageDecoders (numDecoders, size + 1, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageDecoders (numDecoders, size, NULL);
    ASSERT_EQ (GenericError, status);

    free (codecs);

    SHUTDOWN
}

TEST(Codecs, GdipGetImageEncodersSize)
{
    STARTUP

    GpStatus status;
    UINT numEncoders;
    UINT size;

    status = GdipGetImageEncodersSize (&numEncoders, &size);
    ASSERT_EQ (Ok, status);
    ASSERT_EQ (numEncoders, 5);
    //ASSERT_EQ (5 * sizeof (ImageCodecInfo), size);

    // Negative tests.
    // FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetImageEncodersSize (&numEncoders, NULL);
    ASSERT_EQ (InvalidParameter, status);

    status = GdipGetImageEncodersSize (NULL, &size);
    ASSERT_EQ (InvalidParameter, status);
#endif

    SHUTDOWN
}

TEST(Codecs, GdipGetImageEncoders)
{
    STARTUP

    GpStatus status;
    UINT numEncoders;
    UINT size;
    ImageCodecInfo *codecs;

    GdipGetImageEncodersSize (&numEncoders, &size);
    codecs = (ImageCodecInfo *) malloc (size);

    status = GdipGetImageEncoders (numEncoders, size, codecs);
    ASSERT_EQ (Ok, status);

    BYTE bmpSigPattern[] = {0x42, 0x4D};
    BYTE bmpSigMask[] = {0xFF, 0xFF};
    verifyImageCodecInfo (codecs[0], bmpEncoderClsid, bmpRawFormat, "Built-in BMP Codec", "BMP", "*.BMP;*.DIB;*.RLE", "image/bmp", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, bmpSigPattern, bmpSigMask);

    BYTE jpegSigPattern[] = {0xFF, 0xD8};
    BYTE jpegSigMask[] = {0xFF, 0xFF};
    verifyImageCodecInfo (codecs[1], jpegEncoderClsid, jpegRawFormat, "Built-in JPEG Codec", "JPEG", "*.JPG;*.JPEG;*.JPE;*.JFIF", "image/jpeg", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, jpegSigPattern, jpegSigMask);

    BYTE gifSigPattern[] = {0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x47, 0x49, 0x46, 0x38, 0x37, 0x61};
    BYTE gifSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[2], gifEncoderClsid, gifRawFormat, "Built-in GIF Codec", "GIF", "*.GIF", "image/gif", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 2, gifSigPattern, gifSigMask);

    BYTE tiffSigPattern[] = {0x49, 0x49, 0x4D, 0x4D};
    BYTE tiffSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[3], tifEncoderClsid, tifRawFormat, "Built-in TIFF Codec", "TIFF", "*.TIF;*.TIFF", "image/tiff", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 2, tiffSigPattern, tiffSigMask);

    BYTE pngSigPattern[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    BYTE pngSigMask[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    verifyImageCodecInfo (codecs[4], pngEncoderClsid, pngRawFormat, "Built-in PNG Codec", "PNG", "*.PNG", "image/png", ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin, 1, 1, pngSigPattern, pngSigMask);

    // Negative tests.
    status = GdipGetImageEncoders (0, size, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageEncoders (numEncoders - 1, size, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageEncoders (numEncoders + 1, size, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageEncoders (numEncoders, 0, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageEncoders (numEncoders, size - 1, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageEncoders (numEncoders, size + 1, codecs);
    ASSERT_EQ (GenericError, status);

    status = GdipGetImageEncoders (numEncoders, size, NULL);
    ASSERT_EQ (GenericError, status);

    free (codecs);

    SHUTDOWN
}
