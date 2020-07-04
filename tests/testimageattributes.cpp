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

TEST(ImageAttributes, GdipCreateImageAttributes)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	status = GdipCreateImageAttributes (&attributes);
	ASSERT_EQ (Ok, status);
	assert (attributes && "Expected attributes to be initialized.");

	// Negative tests.
	status = GdipCreateImageAttributes (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipCloneImageAttributes)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;
	GpImageAttributes *clonedAttributes;
	ColorMatrix colorMatrix;
	ColorMatrix grayMatrix;
	WCHAR *colorProfileName = createWchar ("AdobeRGB1998.icc");
	ColorMap remapTable[2] = {
		{ {123}, {234} },
		{ {345}, {678} }
	};

	memset ((void *) &colorMatrix, 0, sizeof (ColorMatrix));
	memset ((void *) &grayMatrix, 0, sizeof (ColorMatrix));

	// Simple.
	GdipCreateImageAttributes (&attributes);

	status = GdipCloneImageAttributes (attributes, &clonedAttributes);
	ASSERT_EQ (Ok, status);
	assert (attributes && attributes != clonedAttributes);

	GdipDisposeImageAttributes (clonedAttributes);
	GdipDisposeImageAttributes (attributes);

	// Complex.
	GdipCreateImageAttributes (&attributes);
	GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault);
	GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeBitmap, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault);
	GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeBrush, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault);
	GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypePen, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault);
	GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeText, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault);
	GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, 100);
	GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeBitmap, TRUE, 101);
	GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeBrush, TRUE, 102);
	GdipSetImageAttributesThreshold (attributes, ColorAdjustTypePen, TRUE, 103);
	GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeText, TRUE, 104);
	GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, 100);
	GdipSetImageAttributesGamma (attributes, ColorAdjustTypeBitmap, TRUE, 101);
	GdipSetImageAttributesGamma (attributes, ColorAdjustTypeBrush, TRUE, 102);
	GdipSetImageAttributesGamma (attributes, ColorAdjustTypePen, TRUE, 103);
	GdipSetImageAttributesGamma (attributes, ColorAdjustTypeText, TRUE, 104);
	GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x02030406);
	GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeBitmap, TRUE, 0x01020305, 0x02030407);
	GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeBrush, TRUE, 0x01020306, 0x02030408);
	GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypePen, TRUE, 0x01020307, 0x02030409);
	GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeText, TRUE, 0x01020308, 0x02030400);
	GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsC);
	GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeBitmap, TRUE, ColorChannelFlagsM);
	GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeBrush, TRUE, ColorChannelFlagsY);
	GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypePen, TRUE, ColorChannelFlagsK);
	GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeText, TRUE, ColorChannelFlagsC);
	GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, colorProfileName);
	GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeBitmap, TRUE, colorProfileName);
	GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeBrush, TRUE, colorProfileName);
	GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypePen, TRUE, colorProfileName);
	GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeText, TRUE, colorProfileName);
	GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 1, remapTable);
	GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeBitmap, TRUE, 1, remapTable);
	GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeBrush, TRUE, 1, remapTable);
	GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypePen, TRUE, 1, remapTable);
	GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeText, TRUE, 1, remapTable);
	GdipSetImageAttributesWrapMode (attributes, WrapModeTile, 10, TRUE);
	GdipSetImageAttributesCachedBackground (attributes, TRUE);

	status = GdipCloneImageAttributes (attributes, &clonedAttributes);
	ASSERT_EQ (Ok, status);
	assert (attributes && attributes != clonedAttributes);

	GdipDisposeImageAttributes (clonedAttributes);
	GdipDisposeImageAttributes (attributes);

	// Negative tests.
	GdipCreateImageAttributes (&attributes);

	clonedAttributes = (GpImageAttributes *) 0xCC;
	status = GdipCloneImageAttributes (NULL, &clonedAttributes);
	ASSERT_EQ (InvalidParameter, status);
	assert (clonedAttributes == (GpImageAttributes *) 0xCC);

	status = GdipCloneImageAttributes (attributes, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);
	freeWchar (colorProfileName);

	SHUTDOWN
}

TEST(ImageAttributes, disposeImageAttributes)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipDisposeImageAttributes (attributes);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDisposeImageAttributes (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesToIdentity)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeDefault);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeBitmap);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeBrush);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypePen);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeText);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesToIdentity (NULL, ColorAdjustTypeBitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeCount);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesToIdentity (attributes, ColorAdjustTypeAny);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesToIdentity (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipResetImageAttributes)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeDefault);
	ASSERT_EQ (Ok, status);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeBitmap);
	ASSERT_EQ (Ok, status);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeBrush);
	ASSERT_EQ (Ok, status);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypePen);
	ASSERT_EQ (Ok, status);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeText);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipResetImageAttributes (NULL, ColorAdjustTypeBitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeCount);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipResetImageAttributes (attributes, ColorAdjustTypeAny);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipResetImageAttributes (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesColorMatrix)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;
	ColorMatrix colorMatrix;
	ColorMatrix grayMatrix;

	memset ((void *) &colorMatrix, 0, sizeof (ColorMatrix));
	memset ((void *) &grayMatrix, 0, sizeof (ColorMatrix));

	GdipCreateImageAttributes (&attributes);

	// Enable - ColorMatrixFlagsDefault, null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	ASSERT_EQ (Ok, status);

	// Enable - ColorMatrixFlagsDefault, non-null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsDefault);
	ASSERT_EQ (Ok, status);

	// Enable - ColorMatrixFlagsSkipGrays, null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (Ok, status);

	// Enable - ColorMatrixFlagsSkipGrays, non-null gray matrix.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (Ok, status);

	// Enable - ColorMatrixFlagsAltGray.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsAltGray);
	ASSERT_EQ (Ok, status);

	// Disable.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsAltGray);
	ASSERT_EQ (Ok, status);

	// Re-enable.
	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesColorMatrix (NULL, ColorAdjustTypeBitmap, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (NULL, ColorAdjustTypeBitmap, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeCount, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeCount, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeAny, TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeAny, FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, &colorMatrix, &grayMatrix, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, &grayMatrix, (ColorMatrixFlags)(ColorMatrixFlagsAltGray + 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, &colorMatrix, &grayMatrix, (ColorMatrixFlags)(ColorMatrixFlagsAltGray + 1));
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, NULL, &grayMatrix, ColorMatrixFlagsDefault);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, NULL, &grayMatrix, ColorMatrixFlagsDefault);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsAltGray);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, TRUE, &colorMatrix, NULL, ColorMatrixFlagsSkipGrays);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesColorMatrix (attributes, ColorAdjustTypeDefault, FALSE, &colorMatrix, NULL, ColorMatrixFlagsDefault);
	ASSERT_EQ (Ok, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesThreshold)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Positive value.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, 100);
	ASSERT_EQ (Ok, status);

	// Zero value.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, 0);
	ASSERT_EQ (Ok, status);

	// Negative value.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, -100);
	ASSERT_EQ (Ok, status);

	// Disabled.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, FALSE, 0);
	ASSERT_EQ (Ok, status);

	// Re-enable.
	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeDefault, TRUE, 100);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesThreshold (NULL, ColorAdjustTypeDefault, TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesThreshold (NULL, ColorAdjustTypeDefault, FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeCount, TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeCount, FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeAny, TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesThreshold (attributes, ColorAdjustTypeAny, FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesThreshold (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesThreshold (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesGamma)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Positive value.
	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, 100);
	ASSERT_EQ (Ok, status);

	// Disabled.
	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, FALSE, 10);
	ASSERT_EQ (Ok, status);

	// Re-enable.
	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, 100);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesGamma (NULL, ColorAdjustTypeDefault, TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (NULL, ColorAdjustTypeDefault, FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeCount, TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeCount, FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeAny, TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeAny, FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, 0);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, TRUE, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, FALSE, 0);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesGamma (attributes, ColorAdjustTypeDefault, FALSE, -1);
	ASSERT_EQ (Ok, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesNoOp)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Enabled.
	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeDefault, TRUE);
	ASSERT_EQ (Ok, status);

	// Disabled.
	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeDefault, FALSE);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesNoOp (NULL, ColorAdjustTypeDefault, TRUE);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesNoOp (NULL, ColorAdjustTypeDefault, FALSE);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeCount, TRUE);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeCount, FALSE);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeAny, TRUE);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesNoOp (attributes, ColorAdjustTypeAny, FALSE);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesNoOp (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesNoOp (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesColorKeys)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Enabled - greater.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x02030405);
	ASSERT_EQ (Ok, status);
	
	// Enabled - equal.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01020304);
	ASSERT_EQ (Ok, status);
	
	// Enabled - lower alpha.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x00020304);
	ASSERT_EQ (Ok, status);

	// Disabled.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x02030405);
	ASSERT_EQ (Ok, status);
	
	// Re-enable.
	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x02030405);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesColorKeys (NULL, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x02030405);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, (ColorAdjustType)(ColorAdjustTypeDefault - 1), TRUE, 0x01020304, 0x02030405);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeCount, TRUE, 0x01020304, 0x02030405);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeAny, TRUE, 0x01020304, 0x02030405);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 0x01020304, 0x02030405);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01010405);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x01010405);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01020205);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x01020205);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, TRUE, 0x01020304, 0x01020303);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesColorKeys (attributes, ColorAdjustTypeDefault, FALSE, 0x01020304, 0x01020303);
	ASSERT_EQ (Ok, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesOutputChannel)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// Enable - ColorChannelFlagsC.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsC);
	ASSERT_EQ (Ok, status);

	// Enable - ColorChannelFlagsM.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsM);
	ASSERT_EQ (Ok, status);

	// Enable - ColorChannelFlagsY.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsY);
	ASSERT_EQ (Ok, status);

	// Enable - ColorChannelFlagsK.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsK);
	ASSERT_EQ (Ok, status);

	// Disabled.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, FALSE, ColorChannelFlagsY);
	ASSERT_EQ (Ok, status);

	// Re-enable.
	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsC);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesOutputChannel (NULL, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsK);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, (ColorAdjustType)(ColorAdjustTypeDefault - 1), TRUE, ColorChannelFlagsK);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeCount, TRUE, ColorChannelFlagsK);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeAny, TRUE, ColorChannelFlagsK);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, ColorChannelFlagsK);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, ColorChannelFlagsK);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, ColorChannelFlagsLast);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, (ColorChannelFlags)(ColorChannelFlagsC - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, TRUE, (ColorChannelFlags)(ColorChannelFlagsLast + 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannel (attributes, ColorAdjustTypeDefault, FALSE, (ColorChannelFlags)(ColorChannelFlagsC - 1));
	ASSERT_EQ (Ok, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesOutputChannelColorProfile)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;
	WCHAR *colorProfileName = createWchar ("AdobeRGB1998.icc");
	WCHAR *noSuchFileName = createWchar ("Name");
	WCHAR *invalidFileName = createWchar ("test.bmp");

	GdipCreateImageAttributes (&attributes);

	// Enabled.
	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, colorProfileName);
	//ASSERT_EQ (Ok, status);

	// Disabled.
	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, FALSE, colorProfileName);
	ASSERT_EQ (Ok, status);

	// Re-enable.
	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, colorProfileName);
	//ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesOutputChannelColorProfile (NULL, ColorAdjustTypeDefault, TRUE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (NULL, ColorAdjustTypeDefault, FALSE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeCount, TRUE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeCount, FALSE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeAny, TRUE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeAny, FALSE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, colorProfileName);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, NULL);
	ASSERT_EQ (Win32Error, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, FALSE, NULL);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, TRUE, noSuchFileName);
	ASSERT_EQ (OutOfMemory, status);

	status = GdipSetImageAttributesOutputChannelColorProfile (attributes, ColorAdjustTypeDefault, FALSE, noSuchFileName);
	ASSERT_EQ (Ok, status);

	GdipDisposeImageAttributes (attributes);
	freeWchar (colorProfileName);
	freeWchar (noSuchFileName);
	freeWchar (invalidFileName);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesRemapTable)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;
	ColorMap remapTable[2] = {
		{ {123}, {234} },
		{ {345}, {678} }
	};

	GdipCreateImageAttributes (&attributes);

	// Enable.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 1, remapTable);
	ASSERT_EQ (Ok, status);
	
	// Enable - larger.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 2, remapTable);
	ASSERT_EQ (Ok, status);
	
	// Enable - Same.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 2, remapTable);
	ASSERT_EQ (Ok, status);
	
	// Enable - Smaller.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 1, remapTable);
	ASSERT_EQ (Ok, status);

	// Disable.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, FALSE, 1, remapTable);
	ASSERT_EQ (Ok, status);

	// Re-enable.
	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 2, remapTable);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesRemapTable (NULL, ColorAdjustTypeDefault, TRUE, 1, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (NULL, ColorAdjustTypeDefault, FALSE, 1, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeCount, TRUE, 2, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeCount, FALSE, 2, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeAny, TRUE, 2, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeAny, FALSE, 2, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), TRUE, 2, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, (ColorAdjustType)(ColorAdjustTypeAny + 1), FALSE, 2, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 0, remapTable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, FALSE, 0, remapTable);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, TRUE, 1, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetImageAttributesRemapTable (attributes, ColorAdjustTypeDefault, FALSE, 1, NULL);
	ASSERT_EQ (Ok, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesWrapMode)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// WrapModeTile, enable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeTile, 10, TRUE);
	ASSERT_EQ (Ok, status);

	// WrapModeClamp, enable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeClamp, 10, TRUE);
	ASSERT_EQ (Ok, status);

	// WrapModeTile, disable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeClamp, 10, FALSE);
	ASSERT_EQ (Ok, status);

	// WrapModeClamp, disable clamp.
	status = GdipSetImageAttributesWrapMode (attributes, WrapModeClamp, 10, FALSE);
	ASSERT_EQ (Ok, status);

	// Invalid WrapMode - negative value.
	status = GdipSetImageAttributesWrapMode (attributes, (WrapMode)(WrapModeTile - 1), 10, TRUE);
	ASSERT_EQ (Ok, status);

	// Invalid WrapMode - positive value.
	status = GdipSetImageAttributesWrapMode (attributes, (WrapMode)(WrapModeClamp + 1), 10, TRUE);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesWrapMode (NULL, WrapModeClamp, 10, FALSE);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesICMMode)
{
	STARTUP

	// This causes a linker error. Could be a bug in the GDI+ headers, or that
	// GdipSetImageAttributesICMMode has been obsoleted.
#if !defined(USE_WINDOWS_GDIPLUS)
	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	status = GdipSetImageAttributesICMMode (attributes, TRUE);
	ASSERT_EQ (Ok, status);

	status = GdipSetImageAttributesICMMode (attributes, FALSE);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesICMMode (NULL, FALSE);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);
#endif

SHUTDOWN
}

TEST(ImageAttributes, GdipGetImageAttributesAdjustedPalette)
{
	STARTUP

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
	ASSERT_EQ (Ok, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeBrush);
	ASSERT_EQ (Ok, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypePen);
	ASSERT_EQ (Ok, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeText);
	ASSERT_EQ (Ok, status);
#endif

	// Negative tests.
	status = GdipGetImageAttributesAdjustedPalette (NULL, colorPalette, ColorAdjustTypeBitmap);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, NULL, ColorAdjustTypeBitmap);
	ASSERT_EQ (InvalidParameter, status);

	colorPalette->Count = 0;
	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeBitmap);
	ASSERT_EQ (InvalidParameter, status);
	colorPalette->Count = 1;

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, (ColorAdjustType)(ColorAdjustTypeDefault - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeDefault);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeCount);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, ColorAdjustTypeAny);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetImageAttributesAdjustedPalette (attributes, colorPalette, (ColorAdjustType)(ColorAdjustTypeAny - 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}

TEST(ImageAttributes, GdipSetImageAttributesCachedBackground)
{
	STARTUP

	GpStatus status;
	GpImageAttributes *attributes;

	GdipCreateImageAttributes (&attributes);

	// True.
	status = GdipSetImageAttributesCachedBackground (attributes, TRUE);
	ASSERT_EQ (Ok, status);
	
	// Same.
	status = GdipSetImageAttributesCachedBackground (attributes, TRUE);
	ASSERT_EQ (Ok, status);

	// False.
	status = GdipSetImageAttributesCachedBackground (attributes, FALSE);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipSetImageAttributesCachedBackground (NULL, TRUE);
	ASSERT_EQ (InvalidParameter, status);

	GdipDisposeImageAttributes (attributes);

	SHUTDOWN
}
