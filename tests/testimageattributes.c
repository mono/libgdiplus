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
#include <string.h>
#include "testhelpers.h"

static void test_createImageAttributes ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	status = GdipCreateImageAttributes (&attributes);
	assertEqualInt (status, Ok);
	assert (attributes && "Expected attributes to be initialized.");

	// Negative tests.
	status = GdipCreateImageAttributes (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_cloneImageAttributes ()
{
	GpStatus status;
	GpImageAttributes *attributes;
	GpImageAttributes *clonedAttributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipCloneImageAttributes (attributes, &clonedAttributes);
	assertEqualInt (status, Ok);
	assert (attributes && attributes != clonedAttributes);

	// Negative tests.
	status = GdipCloneImageAttributes (NULL, &clonedAttributes);
	assertEqualInt (status, InvalidParameter);

	status = GdipCloneImageAttributes (attributes, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_disposeImageAttributes ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipDisposeImageAttributes (attributes);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDisposeImageAttributes (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_setImageAttributesToIdentity ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeDefault);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeBitmap);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeBrush);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypePen);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeText);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesToIdentity (NULL, ColorAdjustTypeBitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeCount);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeAny);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesToIdentity (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_resetImageAttributes ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeDefault);
	assertEqualInt (status, Ok);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeBitmap);
	assertEqualInt (status, Ok);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeBrush);
	assertEqualInt (status, Ok);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypePen);
	assertEqualInt (status, Ok);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeText);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipResetImageAttributes (NULL, ColorAdjustTypeBitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeCount);
	assertEqualInt (status, InvalidParameter);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeAny);
	assertEqualInt (status, InvalidParameter);

	status = GdipResetImageAttributes (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1));
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesColorMatrix ()
{
	GpStatus status;
	GpImageAttributes *attributes;
	ColorMatrix colorMatrix;
	ColorMatrix grayMatrix;

	memset ((void *) &colorMatrix, 0, sizeof (ColorMatrix));
	memset ((void *) &grayMatrix, 0, sizeof (ColorMatrix));

	GdipCreateImageAttributes (&attributes);

	// Enable - ColorMatrixFlagsDefault, null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	assertEqualInt (status, Ok);

	// Enable - ColorMatrixFlagsDefault, non-null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault);
	assertEqualInt (status, Ok);

	// Enable - ColorMatrixFlagsSkipGrays, null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, Ok);

	// Enable - ColorMatrixFlagsSkipGrays, non-null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, Ok);

	// Enable - ColorMatrixFlagsAltGray.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsAltGray);
	assertEqualInt (status, Ok);

	// Disable.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsAltGray);
	assertEqualInt (status, Ok);

	// Re-enable.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesColorMatrix (NULL, ColorAdjustTypeBitmap, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (NULL, ColorAdjustTypeBitmap, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeCount, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeCount, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeAny, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeAny, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, (ColorMatrixFlags)(ColorMatrixFlagsAltGray + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, &colorMatrix, &grayMatrix, (ColorMatrixFlags)(ColorMatrixFlagsAltGray + 1));
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, NULL, &grayMatrix, ColorMatrixFlagsDefault);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, NULL, &grayMatrix, ColorMatrixFlagsDefault);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsAltGray);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsSkipGrays);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	assertEqualInt (status, Ok);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesThreshold ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Positive value.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, 100);
	assertEqualInt (status, Ok);

	// Zero value.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, 0);
	assertEqualInt (status, Ok);

	// Negative value.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, -100);
	assertEqualInt (status, Ok);

	// Disabled.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, FALSE, 0);
	assertEqualInt (status, Ok);

	// Re-enable.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, 100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesThreshold (NULL, ColorAdjustTypeDefault, TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesThreshold (NULL, ColorAdjustTypeDefault, FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeCount, TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeCount, FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeAny, TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeAny, FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesThreshold (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesThreshold (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesGamma ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Positive value.
	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, 100);
	assertEqualInt (status, Ok);

	// Disabled.
	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, FALSE, 10);
	assertEqualInt (status, Ok);

	// Re-enable.
	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, 100);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesGamma (NULL, ColorAdjustTypeDefault, TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (NULL, ColorAdjustTypeDefault, FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeCount, TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeCount, FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeAny, TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeAny, FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, 0);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, FALSE, 0);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, FALSE, -1);
	assertEqualInt (status, Ok);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesNop ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Enabled.
	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeDefault, TRUE);
	assertEqualInt (status, Ok);

	// Disabled.
	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeDefault, FALSE);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesNoOp (NULL, ColorAdjustTypeDefault, TRUE);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesNoOp (NULL, ColorAdjustTypeDefault, FALSE);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeCount, TRUE);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeCount, FALSE);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeAny, TRUE);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeAny, FALSE);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesNoOp (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesNoOp (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesColorKeys ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Enabled - greater.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x02030405);
	assertEqualInt (status, Ok);
	
	// Enabled - equal.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01020304);
	assertEqualInt (status, Ok);
	
	// Enabled - lower alpha.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x00020304);
	assertEqualInt (status, Ok);

	// Disabled.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x02030405);
	assertEqualInt (status, Ok);
	
	// Re-enable.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x02030405);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesColorKeys (NULL, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x02030405);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, (ColorAdjustType)(ColorAdjustTypeDefault - 1), TRUE, 0x01020304, 0x02030405);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeCount, TRUE, 0x01020304, 0x02030405);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeAny, TRUE, 0x01020304, 0x02030405);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 0x01020304, 0x02030405);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01010405);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x01010405);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01020205);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x01020205);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01020303);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x01020303);
	assertEqualInt (status, Ok);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesOutputChannel ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Enable - ColorChannelFlagsC.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsC);
	assertEqualInt (status, Ok);

	// Enable - ColorChannelFlagsM.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsM);
	assertEqualInt (status, Ok);

	// Enable - ColorChannelFlagsY.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsY);
	assertEqualInt (status, Ok);

	// Enable - ColorChannelFlagsK.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsK);
	assertEqualInt (status, Ok);

	// Disabled.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, FALSE, ColorChannelFlagsY);
	assertEqualInt (status, Ok);

	// Re-enable.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsC);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesOutputChannel (NULL, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsK);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, (ColorAdjustType)(ColorAdjustTypeDefault - 1), TRUE, ColorChannelFlagsK);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeCount, TRUE, ColorChannelFlagsK);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeAny, TRUE, ColorChannelFlagsK);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, ColorChannelFlagsK);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, ColorChannelFlagsK);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsLast);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, (ColorChannelFlags)(ColorChannelFlagsC - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, (ColorChannelFlags)(ColorChannelFlagsLast + 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, FALSE, (ColorChannelFlags)(ColorChannelFlagsC - 1));
	assertEqualInt (status, Ok);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesOutputChannelColorProfile ()
{
	GpStatus status;
	GpImageAttributes *attributes;
	WCHAR *colorProfileName = createWchar ("AdobeRGB1998.icc");
	WCHAR *noSuchFileName = createWchar ("Name");
	WCHAR *invalidFileName = createWchar ("test.bmp");

	GdipCreateImageAttributes (&attributes);

	// Enabled.
	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, colorProfileName);
	//assertEqualInt (status, Ok);

	// Disabled.
	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, FALSE, colorProfileName);
	assertEqualInt (status, Ok);

	// Re-enable.
	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, colorProfileName);
	//assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesOutputChannelColorProfile (NULL, ColorAdjustTypeDefault, TRUE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (NULL, ColorAdjustTypeDefault, FALSE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeCount, TRUE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeCount, FALSE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeAny, TRUE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeAny, FALSE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, colorProfileName);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, NULL);
	assertEqualInt (status, Win32Error);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, FALSE, NULL);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, noSuchFileName);
	assertEqualInt (status, OutOfMemory);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, FALSE, noSuchFileName);
	assertEqualInt (status, Ok);

	GdipDisposeImageAttributes (attributes);
	freeWchar (colorProfileName);
	freeWchar (noSuchFileName);
	freeWchar (invalidFileName);
}

static void test_setImageAttributesRemapTable ()
{
	GpStatus status;
	GpImageAttributes *attributes;
	ColorMap remapTable[2] = {
		{ {123}, {234} },
		{ {345}, {678} }
	};

	GdipCreateImageAttributes (&attributes);

	// Enable.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 1, remapTable);
	assertEqualInt (status, Ok);
	
	// Enable - larger.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 2, remapTable);
	assertEqualInt (status, Ok);
	
	// Enable - Same.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 2, remapTable);
	assertEqualInt (status, Ok);
	
	// Enable - Smaller.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 1, remapTable);
	assertEqualInt (status, Ok);

	// Disable.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, FALSE, 1, remapTable);
	assertEqualInt (status, Ok);

	// Re-enable.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 2, remapTable);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesRemapTable (NULL, ColorAdjustTypeDefault, TRUE, 1, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (NULL, ColorAdjustTypeDefault, FALSE, 1, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeCount, TRUE, 2, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeCount, FALSE, 2, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeAny, TRUE, 2, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeAny, FALSE, 2, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 2, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, 2, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 0, remapTable);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, FALSE, 0, remapTable);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 1, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, FALSE, 1, NULL);
	assertEqualInt (status, Ok);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesWrapMode ()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// WrapModeTile, enable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeTile, 10, TRUE);
	assertEqualInt (status, Ok);

	// WrapModeClamp, enable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeClamp, 10, TRUE);
	assertEqualInt (status, Ok);

	// WrapModeTile, disable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeClamp, 10, FALSE);
	assertEqualInt (status, Ok);

	// WrapModeClamp, disable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeClamp, 10, FALSE);
	assertEqualInt (status, Ok);

	// Invalid WrapMode - negative value.
	status = GdipSetImageAttributesWrapMode (attributes, (WrapMode)(WrapModeTile - 1), 10, TRUE);
	assertEqualInt (status, Ok);

	// Invalid WrapMode - positive value.
	status = GdipSetImageAttributesWrapMode (attributes, (WrapMode)(WrapModeClamp + 1), 10, TRUE);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesWrapMode (NULL, WrapModeClamp, 10, FALSE);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesICMMode ()
{
	// This causes a linker error. Could be a bug in the GDI+ headers, or that
	// GdipSetImageAttributesICMMode has been obsoleted.
#if !defined(USE_WINDOWS_GDIPLUS)
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipSetImageAttributesICMMode (attributes, TRUE);
	assertEqualInt (status, Ok);

	status = GdipSetImageAttributesICMMode (attributes, FALSE);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesICMMode (NULL, FALSE);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
#endif
}

static void test_getImageAttributesAdjustedPalette ()
{
	GpStatus status;
	GpImageAttributes *attributes;
	BYTE buffer[1040];
	ColorPalette *colorPalette = (ColorPalette *) buffer;

	GdipCreateImageAttributes (&attributes);
	colorPalette->Count = 1;
	colorPalette->Flags = 0;
	colorPalette->Entries[0] = 1;

	// FIXME: GdipGetImageAttributesAdjustedPalette is not implemented in libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeBitmap);
	assertEqualInt (status, Ok);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeBrush);
	assertEqualInt (status, Ok);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypePen);
	assertEqualInt (status, Ok);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeText);
	assertEqualInt (status, Ok);
#endif

	// Negative tests.
	status = GdipGetImageAttributesAdjustedPalette (NULL, colorPalette, ColorAdjustTypeBitmap);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageAttributesAdjustedPalette (attributes, NULL, ColorAdjustTypeBitmap);
	assertEqualInt (status, InvalidParameter);

	colorPalette->Count = 0;
	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeBitmap);
	assertEqualInt (status, InvalidParameter);
	colorPalette->Count = 1;

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, (ColorAdjustType)(ColorAdjustTypeDefault - 1));
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeDefault);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeCount);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeAny);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, (ColorAdjustType)(ColorAdjustTypeAny - 1));
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

static void test_setImageAttributesCachedBackground()
{
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// True.
	status = GdipSetImageAttributesCachedBackground (attributes, TRUE);
	assertEqualInt (status, Ok);
	
	// Same.
	status = GdipSetImageAttributesCachedBackground (attributes, TRUE);
	assertEqualInt (status, Ok);

	// False.
	status = GdipSetImageAttributesCachedBackground (attributes, FALSE);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipSetImageAttributesCachedBackground (NULL, TRUE);
	assertEqualInt (status, InvalidParameter);

	GdipDisposeImageAttributes (attributes);
}

int
main (int argc, char**argv)
{
	STARTUP;

	test_createImageAttributes ();
	test_cloneImageAttributes ();
	test_disposeImageAttributes ();
	test_setImageAttributesToIdentity ();
	test_resetImageAttributes ();
	test_setImageAttributesColorMatrix ();
	test_setImageAttributesThreshold ();
	test_setImageAttributesGamma ();
	test_setImageAttributesNop ();
	test_setImageAttributesColorKeys ();
	test_setImageAttributesOutputChannel ();
	test_setImageAttributesOutputChannelColorProfile ();
	test_setImageAttributesRemapTable ();
	test_setImageAttributesWrapMode ();
	test_setImageAttributesICMMode ();
	test_getImageAttributesAdjustedPalette ();
	test_setImageAttributesCachedBackground ();

	SHUTDOWN;
	return 0;
}
