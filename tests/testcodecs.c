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

#define verifyImageCodecInfo(codec, expectedClsid, expectedFormatID, expectedCodecName, expectedFormatDescription, expectedFilenameExtension, expectedMimeType, expectedFlags, expectedVersion, expectedSigCount, expectedSigPattern, expectedSigMask) \
{ \
    assertEqualGuid (codec.Clsid, expectedClsid); \
    assertEqualGuid (codec.FormatID, expectedFormatID); \
    assertEqualString (codec.CodecName, expectedCodecName); \
    assert (codec.DllName == NULL); \
    assertEqualString (codec.FormatDescription, expectedFormatDescription); \
    assertEqualString (codec.FilenameExtension, expectedFilenameExtension); \
    assertEqualString (codec.MimeType, expectedMimeType); \
    assertEqualInt (codec.Flags, expectedFlags); \
    assertEqualInt (codec.Version, expectedVersion); \
    assertEqualInt (codec.SigCount, expectedSigCount); \
    assertEqualInt (codec.SigSize, sizeof (expectedSigPattern) / expectedSigCount); \
    assertEqualBytes (codec.SigPattern, expectedSigPattern, codec.SigSize); \
    assertEqualBytes (codec.SigMask, expectedSigMask, codec.SigSize); \
}

static void test_getImageDecodersSize ()
{
    GpStatus status;
    UINT numDecoders;
    UINT size;

    status = GdipGetImageDecodersSize (&numDecoders, &size);
    assertEqualInt (status, Ok);
    assertEqualInt (numDecoders, 8);
    //assertEqualInt (size, 8 * sizeof (ImageCodecInfo));

    // Negative tests.
    // FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetImageDecodersSize (&numDecoders, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetImageDecodersSize (NULL, &size);
    assertEqualInt (status, InvalidParameter);
#endif
}

static void test_getImageDecoders ()
{
    GpStatus status;
    UINT numDecoders;
    UINT size;
    ImageCodecInfo *codecs;

    GdipGetImageDecodersSize (&numDecoders, &size);
    codecs = (ImageCodecInfo *) malloc (size);

    status = GdipGetImageDecoders (numDecoders, size, codecs);
    assertEqualInt (status, Ok);

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
    assertEqualInt (status, GenericError);

    status = GdipGetImageDecoders (numDecoders - 1, size, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageDecoders (numDecoders + 1, size, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageDecoders (numDecoders, 0, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageDecoders (numDecoders, size - 1, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageDecoders (numDecoders, size + 1, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageDecoders (numDecoders, size, NULL);
    assertEqualInt (status, GenericError);

    free (codecs);
}

static void test_getImageEncodersSize ()
{
    GpStatus status;
    UINT numEncoders;
    UINT size;

    status = GdipGetImageEncodersSize (&numEncoders, &size);
    assertEqualInt (status, Ok);
    assertEqualInt (numEncoders, 5);
    //assertEqualInt (size, 5 * sizeof (ImageCodecInfo));

    // Negative tests.
    // FIXME: this causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
    status = GdipGetImageEncodersSize (&numEncoders, NULL);
    assertEqualInt (status, InvalidParameter);

    status = GdipGetImageEncodersSize (NULL, &size);
    assertEqualInt (status, InvalidParameter);
#endif
}

static void test_getImageEncoders ()
{
    GpStatus status;
    UINT numEncoders;
    UINT size;
    ImageCodecInfo *codecs;

    GdipGetImageEncodersSize (&numEncoders, &size);
    codecs = (ImageCodecInfo *) malloc (size);

    status = GdipGetImageEncoders (numEncoders, size, codecs);
    assertEqualInt (status, Ok);

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
    assertEqualInt (status, GenericError);

    status = GdipGetImageEncoders (numEncoders - 1, size, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageEncoders (numEncoders + 1, size, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageEncoders (numEncoders, 0, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageEncoders (numEncoders, size - 1, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageEncoders (numEncoders, size + 1, codecs);
    assertEqualInt (status, GenericError);

    status = GdipGetImageEncoders (numEncoders, size, NULL);
    assertEqualInt (status, GenericError);

    free (codecs);
}

int
main (int argc, char**argv)
{
	STARTUP;

    test_getImageDecodersSize ();
    test_getImageDecoders ();
    test_getImageEncodersSize ();
    test_getImageEncoders ();

    SHUTDOWN;
	return 0;
}
