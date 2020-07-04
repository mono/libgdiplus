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

static void verifyStringFormat (GpStringFormat *format, INT expectedAttributes, StringTrimming expectedTrimming)
{
	GpStatus status;
	INT flags;
	StringAlignment align;
	LANGID language;
	StringDigitSubstitute digitSubstitution;
	StringAlignment lineAlign;
	INT hotkeyPrefix;
	StringTrimming trimming;
	INT tabStopCount;
	REAL firstTabOffset;
	REAL tabStops[1];
	INT measurableCharacterRangeCount;

	status = GdipGetStringFormatFlags (format, &flags);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedAttributes, flags);

	status = GdipGetStringFormatAlign (format, &align);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (StringAlignmentNear, align);

	status = GdipGetStringFormatDigitSubstitution (format, &language, &digitSubstitution);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, language);
	ASSERT_EQ (StringDigitSubstituteUser, digitSubstitution);

	status = GdipGetStringFormatLineAlign (format, &lineAlign);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (StringAlignmentNear, lineAlign);

	status = GdipGetStringFormatHotkeyPrefix (format, &hotkeyPrefix);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (HotkeyPrefixNone, hotkeyPrefix);

	status = GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedTrimming, trimming);

	status = GdipGetStringFormatTabStopCount (format, &tabStopCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, tabStopCount);

	status = GdipGetStringFormatTabStops (format, 0, &firstTabOffset, tabStops);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, firstTabOffset);

	status = GdipGetStringFormatMeasurableCharacterRangeCount (format, &measurableCharacterRangeCount);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, measurableCharacterRangeCount);
}

TEST(StringFormat, GdipCreateStringFormat)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;

	// Positive values.
	status = GdipCreateStringFormat (10, 11, &format);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (format, 10, StringTrimmingCharacter);
	GdipDeleteStringFormat (format);

	// Zero values.
	status = GdipCreateStringFormat (0, 0, &format);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (format, 0, StringTrimmingCharacter);
	GdipDeleteStringFormat (format);

	// Negative values.
	status = GdipCreateStringFormat (-1, -2, &format);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (format, -1, StringTrimmingCharacter);
	GdipDeleteStringFormat (format);

	// Negative tests.
	status = GdipCreateStringFormat (10, 11, NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(StringFormat, getGenericDefault)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format1;
	GpStringFormat *format2;
	StringAlignment lineAlign;

	// Should be a singleton.
	status = GdipStringFormatGetGenericDefault (&format1);
	ASSERT_EQ (Ok, status);
	assert (format1);
	verifyStringFormat (format1, 0, StringTrimmingCharacter);

	status = GdipStringFormatGetGenericDefault (&format2);
	ASSERT_EQ (Ok, status);
	assert (format2);
	verifyStringFormat (format2, 0, StringTrimmingCharacter);
	assert (format1 == format2);

	// Should not be deleted.
	status = GdipDeleteStringFormat (format2);
	ASSERT_EQ (Ok, status);

	status = GdipStringFormatGetGenericDefault (&format2);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (format2, 0, StringTrimmingCharacter);
	assert (format1 == format2);

	// Should be mutable.
	status = GdipSetStringFormatLineAlign (format1, StringAlignmentFar);
	ASSERT_EQ (Ok, status);

	status = GdipSetStringFormatLineAlign (format1, StringAlignmentFar);
	ASSERT_EQ (Ok, status);

	status = GdipGetStringFormatLineAlign (format1, &lineAlign);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (StringAlignmentFar, lineAlign);

	// Set back to the default.
	status = GdipSetStringFormatLineAlign (format1, StringAlignmentNear);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipStringFormatGetGenericDefault (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(StringFormat, getGenericTypographic)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format1;
	GpStringFormat *format2;
	StringAlignment lineAlign;

	// Should be a singleton.
	status = GdipStringFormatGetGenericTypographic (&format1);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (format1, StringFormatFlagsNoFitBlackBox | StringFormatFlagsLineLimit | StringFormatFlagsNoClip, StringTrimmingNone);

	status = GdipStringFormatGetGenericTypographic (&format2);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (format2, StringFormatFlagsNoFitBlackBox | StringFormatFlagsLineLimit | StringFormatFlagsNoClip, StringTrimmingNone);
	assert (format1 == format2);

	// Should not be deleted.
	status = GdipDeleteStringFormat (format2);
	ASSERT_EQ (Ok, status);

	status = GdipStringFormatGetGenericTypographic (&format2);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (format2, StringFormatFlagsNoFitBlackBox | StringFormatFlagsLineLimit | StringFormatFlagsNoClip, StringTrimmingNone);
	assert (format1 == format2);

	// Should be mutable.
	status = GdipSetStringFormatLineAlign (format1, StringAlignmentFar);
	ASSERT_EQ (Ok, status);

	status = GdipGetStringFormatLineAlign (format1, &lineAlign);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (StringAlignmentFar, lineAlign);

	// Set back to the default.
	status = GdipSetStringFormatLineAlign (format1, StringAlignmentNear);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipStringFormatGetGenericTypographic (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(StringFormat, deleteStringFormat)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;

	GdipCreateStringFormat (10, 11, &format);

	status = GdipDeleteStringFormat (format);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDeleteStringFormat (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(StringFormat, GdipCloneStringFormat)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	GpStringFormat *clonedFormat;

	GdipCreateStringFormat (10, 11, &format);

	status = GdipCloneStringFormat (format, &clonedFormat);
	ASSERT_EQ (Ok, status);
	verifyStringFormat (clonedFormat, 10, StringTrimmingCharacter);
	GdipDeleteStringFormat (clonedFormat);

	// Negative tests.
	status = GdipCloneStringFormat (NULL, &clonedFormat);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCloneStringFormat (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatAlign)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	StringAlignment align;

	GdipCreateStringFormat (10, 11, &format);

	// StringAlignmentCenter.
	status = GdipSetStringFormatAlign (format, StringAlignmentNear);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatAlign (format, &align);
	ASSERT_EQ (StringAlignmentNear, align);

	// Same.
	status = GdipSetStringFormatAlign (format, StringAlignmentNear);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatAlign (format, &align);
	ASSERT_EQ (StringAlignmentNear, align);

	// StringAlignmentCenter.
	status = GdipSetStringFormatAlign (format, StringAlignmentCenter);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatAlign (format, &align);
	ASSERT_EQ (StringAlignmentCenter, align);

	// StringAlignmentFar.
	status = GdipSetStringFormatAlign (format, StringAlignmentFar);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatAlign (format, &align);
	ASSERT_EQ (StringAlignmentFar, align);

	// Negative tests.
	status = GdipSetStringFormatAlign (NULL, StringAlignmentCenter);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatAlign (format, (StringAlignment)(StringAlignmentNear - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatAlign (format, (StringAlignment)(StringAlignmentFar + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatAlign)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	StringAlignment align;

	GdipCreateStringFormat (10, 11, &format);

	// Negative tests.
	status = GdipGetStringFormatAlign (NULL, &align);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatAlign (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatLineAlign)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	StringAlignment lineAlign;

	GdipCreateStringFormat (10, 11, &format);

	// StringAlignmentNear.
	status = GdipSetStringFormatLineAlign (format, StringAlignmentNear);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatLineAlign (format, &lineAlign);
	ASSERT_EQ (StringAlignmentNear, lineAlign);

	// Same.
	status = GdipSetStringFormatLineAlign (format, StringAlignmentNear);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatLineAlign (format, &lineAlign);
	ASSERT_EQ (StringAlignmentNear, lineAlign);

	// StringAlignmentCenter.
	status = GdipSetStringFormatLineAlign (format, StringAlignmentCenter);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatLineAlign (format, &lineAlign);
	ASSERT_EQ (StringAlignmentCenter, lineAlign);

	// StringAlignmentFar.
	status = GdipSetStringFormatLineAlign (format, StringAlignmentFar);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatLineAlign (format, &lineAlign);
	ASSERT_EQ (StringAlignmentFar, lineAlign);

	// Negative tests.
	status = GdipSetStringFormatLineAlign (NULL, StringAlignmentCenter);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatLineAlign (format, (StringAlignment)(StringAlignmentNear - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatLineAlign (format, (StringAlignment)(StringAlignmentFar + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatLineAlign)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	StringAlignment lineAlign;

	GdipCreateStringFormat (10, 11, &format);

	// Negative tests.
	status = GdipGetStringFormatLineAlign (NULL, &lineAlign);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatLineAlign (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatTrimming)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	StringTrimming trimming;

	GdipCreateStringFormat (10, 11, &format);

	// StringTrimmingNone
	status = GdipSetStringFormatTrimming (format, StringTrimmingNone);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (StringTrimmingNone, trimming);

	// Same.
	status = GdipSetStringFormatTrimming (format, StringTrimmingNone);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (StringTrimmingNone, trimming);

	// StringTrimmingEllipsisCharacter.
	status = GdipSetStringFormatTrimming (format, StringTrimmingCharacter);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (StringTrimmingCharacter, trimming);

	// StringTrimmingWord.
	status = GdipSetStringFormatTrimming (format, StringTrimmingWord);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (StringTrimmingWord, trimming);

	// StringTrimmingEllipsisCharacter.
	status = GdipSetStringFormatTrimming (format, StringTrimmingEllipsisCharacter);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (StringTrimmingEllipsisCharacter, trimming);

	// StringTrimmingEllipsisWord.
	status = GdipSetStringFormatTrimming (format, StringTrimmingEllipsisWord);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (StringTrimmingEllipsisWord, trimming);

	// StringTrimmingEllipsisPath.
	status = GdipSetStringFormatTrimming (format, StringTrimmingEllipsisPath);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTrimming (format, &trimming);
	ASSERT_EQ (StringTrimmingEllipsisPath, trimming);

	// Negative tests.
	status = GdipSetStringFormatTrimming (NULL, StringTrimmingEllipsisCharacter);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatTrimming (format, (StringTrimming)(StringTrimmingNone - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatTrimming (format, (StringTrimming)(StringTrimmingEllipsisPath + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatTrimming)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	StringTrimming trimming;

	GdipCreateStringFormat (10, 11, &format);

	// Negative tests.
	status = GdipGetStringFormatTrimming (NULL, &trimming);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatTrimming (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatHotkeyPrefix)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	INT hotkeyPrefix;

	GdipCreateStringFormat (10, 11, &format);

	// HotkeyPrefixNone.
	status = GdipSetStringFormatHotkeyPrefix (format, HotkeyPrefixNone);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatHotkeyPrefix (format, &hotkeyPrefix);
	ASSERT_EQ (HotkeyPrefixNone, hotkeyPrefix);

	// Same.
	status = GdipSetStringFormatHotkeyPrefix (format, HotkeyPrefixNone);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatHotkeyPrefix (format, &hotkeyPrefix);
	ASSERT_EQ (HotkeyPrefixNone, hotkeyPrefix);

	// HotkeyPrefixShow.
	status = GdipSetStringFormatHotkeyPrefix (format, HotkeyPrefixShow);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatHotkeyPrefix (format, &hotkeyPrefix);
	ASSERT_EQ (HotkeyPrefixShow, hotkeyPrefix);

	// HotkeyPrefixHide.
	status = GdipSetStringFormatHotkeyPrefix (format, HotkeyPrefixHide);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatHotkeyPrefix (format, &hotkeyPrefix);
	ASSERT_EQ (HotkeyPrefixHide, hotkeyPrefix);

	// Negative tests.
	status = GdipSetStringFormatHotkeyPrefix (NULL, HotkeyPrefixNone);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatHotkeyPrefix (format, (HotkeyPrefix)(HotkeyPrefixNone - 1));
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatHotkeyPrefix (format, (HotkeyPrefix)(HotkeyPrefixHide + 1));
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatHotkeyPrefix)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	INT hotkeyPrefix;

	GdipCreateStringFormat (10, 11, &format);

	// Negative tests.
	status = GdipGetStringFormatHotkeyPrefix (NULL, &hotkeyPrefix);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatHotkeyPrefix (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatTabStops)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	REAL firstTabOffset;
	REAL tabStops1[4] = {1, 0, 2, 3};
	REAL tabStops2[4] = {3, 2, 0, 1};
	REAL oneTabStop[1] = {0};
	REAL negativeValueInTabStops[4] = {1, -1};
	REAL formatTabStops[4];

	GdipCreateStringFormat (10, 11, &format);

	// Count of 4.
	status = GdipSetStringFormatTabStops (format, 10, 4, tabStops1);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTabStops (format, 4, &firstTabOffset, formatTabStops);
	ASSERT_FLOAT_EQ_NAN (10, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (1, formatTabStops[0]);
	ASSERT_FLOAT_EQ_NAN (0, formatTabStops[1]);
	ASSERT_FLOAT_EQ_NAN (2, formatTabStops[2]);
	ASSERT_FLOAT_EQ_NAN (3, formatTabStops[3]);

	// Count of 4 again.
	status = GdipSetStringFormatTabStops (format, FLT_MAX, 4, tabStops2);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTabStops (format, 4, &firstTabOffset, formatTabStops);
	ASSERT_FLOAT_EQ_NAN (FLT_MAX, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (3, formatTabStops[0]);
	ASSERT_FLOAT_EQ_NAN (2, formatTabStops[1]);
	ASSERT_FLOAT_EQ_NAN (0, formatTabStops[2]);
	ASSERT_FLOAT_EQ_NAN (1, formatTabStops[3]);

	// Count of 1.
	status = GdipSetStringFormatTabStops (format, 0, 1, oneTabStop);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTabStops (format, 1, &firstTabOffset, formatTabStops);
	ASSERT_FLOAT_EQ_NAN (0, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (0, formatTabStops[0]);

	// Count of 0.
	status = GdipSetStringFormatTabStops (format, 10, 0, oneTabStop);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTabStops (format, 1, &firstTabOffset, formatTabStops);
	ASSERT_FLOAT_EQ_NAN (0, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (0, formatTabStops[0]);

	// Count of -1.
	status = GdipSetStringFormatTabStops (format, 10, -1, oneTabStop);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTabStops (format, 1, &firstTabOffset, formatTabStops);
	ASSERT_FLOAT_EQ_NAN (0, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (0, formatTabStops[0]);

	// Count of 0, invalid first tab offset.
	status = GdipSetStringFormatTabStops (format, -1, 0, oneTabStop);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatTabStops (format, 1, &firstTabOffset, formatTabStops);
	ASSERT_FLOAT_EQ_NAN (0, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (0, formatTabStops[0]);

	// Negative tests,
	status = GdipSetStringFormatTabStops (NULL, 10, -1, oneTabStop);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatTabStops (format, 10, -1, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatTabStops (format, -1, 10, oneTabStop);
	ASSERT_EQ (NotImplemented, status);

	// Negative value in tab stops.
	formatTabStops[0] = 10;
	status = GdipSetStringFormatTabStops (format, 40, 4, negativeValueInTabStops);
	ASSERT_EQ (NotImplemented, status);

	status = GdipGetStringFormatTabStops (format, 4, &firstTabOffset, formatTabStops);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (0, formatTabStops[0]);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatTabStops)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	REAL firstTabOffset = 10;
	REAL tabStops[4] = {1, 2, 3, 4};
	REAL formatTabStops[5] = {-1, -2, -3, -4, -5};

	GdipCreateStringFormat (10, 11, &format);

	// Count of 0 - still overwrites the firstTabOffset value.
	status = GdipGetStringFormatTabStops (format, 0, &firstTabOffset, formatTabStops);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (0, firstTabOffset);

	// Count less than the number of tab stops.
	GdipSetStringFormatTabStops (format, 10, 4, tabStops);

	status = GdipGetStringFormatTabStops (format, 2, &firstTabOffset, formatTabStops);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (1, formatTabStops[0]);
	ASSERT_FLOAT_EQ_NAN (2, formatTabStops[1]);
	ASSERT_FLOAT_EQ_NAN (-3, formatTabStops[2]);
	ASSERT_FLOAT_EQ_NAN (-4, formatTabStops[3]);
	ASSERT_FLOAT_EQ_NAN (-5, formatTabStops[4]);

	// Count greater than the number of tab stops.
	GdipSetStringFormatTabStops (format, 10, 4, tabStops);

	status = GdipGetStringFormatTabStops (format, 5, &firstTabOffset, formatTabStops);
	ASSERT_EQ (Ok, status);
	ASSERT_FLOAT_EQ_NAN (10, firstTabOffset);
	ASSERT_FLOAT_EQ_NAN (1, formatTabStops[0]);
	ASSERT_FLOAT_EQ_NAN (2, formatTabStops[1]);
	ASSERT_FLOAT_EQ_NAN (3, formatTabStops[2]);
	ASSERT_FLOAT_EQ_NAN (4, formatTabStops[3]);
	ASSERT_FLOAT_EQ_NAN (-5, formatTabStops[4]);

	// Negative tests,
	status = GdipGetStringFormatTabStops (NULL, 0, &firstTabOffset, formatTabStops);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatTabStops (NULL, -1, &firstTabOffset, formatTabStops);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatTabStops (format, 0, NULL, formatTabStops);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatTabStops (format, -1, NULL, formatTabStops);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatTabStops (format, 0, &firstTabOffset, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatTabStops (format, -1, &firstTabOffset, NULL);
	ASSERT_EQ (InvalidParameter, status);

	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipGetStringFormatTabStops (format, -1, &firstTabOffset, formatTabStops);
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatTabStopCount)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	INT tabStopCount;

	GdipCreateStringFormat (10, 11, &format);

	// Negative tests.
	status = GdipGetStringFormatTabStopCount (NULL, &tabStopCount);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatTabStopCount (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatDigitSubstitution)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	LANGID language;
	StringDigitSubstitute substitute;

	GdipCreateStringFormat (10, 11, &format);

	// StringDigitSubstituteUser.
	status = GdipSetStringFormatDigitSubstitution (format, 10, StringDigitSubstituteUser);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (10, language);
	ASSERT_EQ (StringDigitSubstituteUser, substitute);

	// Same.
	status = GdipSetStringFormatDigitSubstitution (format, 10, StringDigitSubstituteUser);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (10, language);
	ASSERT_EQ (StringDigitSubstituteUser, substitute);

	// Different language, same substitute.
	status = GdipSetStringFormatDigitSubstitution (format, 12, StringDigitSubstituteUser);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (12, language);
	ASSERT_EQ (StringDigitSubstituteUser, substitute);

	// Same language, different substitute.
	status = GdipSetStringFormatDigitSubstitution (format, 12, StringDigitSubstituteNone);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (12, language);
	ASSERT_EQ (StringDigitSubstituteNone, substitute);

	// StringDigitSubstituteNone.
	status = GdipSetStringFormatDigitSubstitution (format, 10, StringDigitSubstituteNone);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (10, language);
	ASSERT_EQ (StringDigitSubstituteNone, substitute);

	// StringDigitSubstituteNational.
	status = GdipSetStringFormatDigitSubstitution (format, 10, StringDigitSubstituteNational);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (10, language);
	ASSERT_EQ (StringDigitSubstituteNational, substitute);

	// StringDigitSubstituteTraditional.
	status = GdipSetStringFormatDigitSubstitution (format, 10, StringDigitSubstituteTraditional);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (10, language);
	ASSERT_EQ (StringDigitSubstituteTraditional, substitute);

	// Invalid value - negative.
	status = GdipSetStringFormatDigitSubstitution (format, 0, (StringDigitSubstitute)(StringDigitSubstituteUser - 1));
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (0, language);
	ASSERT_EQ ((StringDigitSubstitute)(StringDigitSubstituteUser - 1), substitute);

	// Invalid value - positive.
	status = GdipSetStringFormatDigitSubstitution (format, 0, (StringDigitSubstitute)(StringDigitSubstituteTraditional + 1));
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatDigitSubstitution (format, &language, &substitute);
	ASSERT_EQ (0, language);
	ASSERT_EQ ((StringDigitSubstitute)(StringDigitSubstituteTraditional + 1), substitute);

	// Negative tests.
	status = GdipSetStringFormatDigitSubstitution (NULL, 0, StringDigitSubstituteNational);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatDigitSubstitution)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	LANGID language;
	StringDigitSubstitute substitute;

	GdipCreateStringFormat (10, 11, &format);

	// Language null.
	status = GdipGetStringFormatDigitSubstitution (format, &language, NULL);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, language);

	// Substitution null.
	status = GdipGetStringFormatDigitSubstitution (format, NULL, &substitute);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (StringDigitSubstituteUser, substitute);

	// Both null.
	status = GdipGetStringFormatDigitSubstitution (format, NULL, NULL);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipGetStringFormatDigitSubstitution (NULL, &language, &substitute);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatFlags)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	INT flags;

	GdipCreateStringFormat (10, 11, &format);

	// Custom flags.
	status = GdipSetStringFormatFlags (format, 1234);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatFlags (format, &flags);
	ASSERT_EQ (1234, flags);

	// Same.
	status = GdipSetStringFormatFlags (format, 1234);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatFlags (format, &flags);
	ASSERT_EQ (1234, flags);

	// Zero.
	status = GdipSetStringFormatFlags (format, 0);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatFlags (format, &flags);
	ASSERT_EQ (0, flags);

	// Negative.
	status = GdipSetStringFormatFlags (format, -1234);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatFlags (format, &flags);
	ASSERT_EQ (-1234, flags);

	// Negative tests.
	status = GdipSetStringFormatFlags (NULL, 1234);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatFlags)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	INT flags;

	GdipCreateStringFormat (10, 11, &format);

	// Negative tests.
	status = GdipGetStringFormatFlags (NULL, &flags);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatFlags (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, getStringFormatMeasurableCharacterRangeCount)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	INT measurableCharacterRangeCount;

	GdipCreateStringFormat (10, 11, &format);

	// Negative tests.
	status = GdipGetStringFormatMeasurableCharacterRangeCount (NULL, &measurableCharacterRangeCount);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetStringFormatMeasurableCharacterRangeCount (format, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}

TEST(StringFormat, setStringFormatMeasurableCharacterRanges)
{
	STARTUP

	GpStatus status;
	GpStringFormat *format;
	CharacterRange ranges[32];
	INT measurableCharacterRangeCount;

	GdipCreateStringFormat (10, 11, &format);

	// Count of 1.
	status = GdipSetStringFormatMeasurableCharacterRanges (format, 1, ranges);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatMeasurableCharacterRangeCount (format, &measurableCharacterRangeCount);
	ASSERT_EQ (1, measurableCharacterRangeCount);

	// Count of 1 again.
	status = GdipSetStringFormatMeasurableCharacterRanges (format, 1, ranges);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatMeasurableCharacterRangeCount (format, &measurableCharacterRangeCount);
	ASSERT_EQ (1, measurableCharacterRangeCount);

	// Count of 0.
	GdipSetStringFormatMeasurableCharacterRanges (format, 3, ranges);

	status = GdipSetStringFormatMeasurableCharacterRanges (format, 0, ranges);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatMeasurableCharacterRangeCount (format, &measurableCharacterRangeCount);
	ASSERT_EQ (0, measurableCharacterRangeCount);

	// Count of -1.
	GdipSetStringFormatMeasurableCharacterRanges (format, 3, ranges);

	status = GdipSetStringFormatMeasurableCharacterRanges (format, -1, ranges);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatMeasurableCharacterRangeCount (format, &measurableCharacterRangeCount);
	ASSERT_EQ (0, measurableCharacterRangeCount);

	// Count of 32.
	status = GdipSetStringFormatMeasurableCharacterRanges (format, 32, ranges);
	ASSERT_EQ (Ok, status);

	GdipGetStringFormatMeasurableCharacterRangeCount (format, &measurableCharacterRangeCount);
	ASSERT_EQ (32, measurableCharacterRangeCount);

	// Negative tests.
	status = GdipSetStringFormatMeasurableCharacterRanges (NULL, 0, ranges);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatMeasurableCharacterRanges (format, 0, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipSetStringFormatMeasurableCharacterRanges (format, 33, ranges);
	ASSERT_EQ (ValueOverflow, status);

	GdipDeleteStringFormat (format);

	SHUTDOWN
}
