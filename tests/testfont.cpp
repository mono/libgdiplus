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

static HDC getEmptyHDC ()
{
#if defined(USE_WINDOWS_GDIPLUS)
	return CreateCompatibleDC (NULL);
#else
	return (HDC)1;
#endif
}

static void *readFile (const char *fileName, int *memoryLength)
{
	void *buffer = NULL;
	size_t length;
	size_t read_length;

	FILE *f = fopen (fileName, "rb");
	assert (f && "Expected file to exist.");

	fseek (f, 0, SEEK_END);
	length = ftell (f);

	fseek (f, 0, SEEK_SET);
	buffer = malloc (length);
	assert (buffer && "Expected successful allocation of buffer.");

	read_length = fread (buffer, 1, length, f);
	assert (read_length && "Expected successful read.");
	fclose (f);

	*memoryLength = (int) length;
	return buffer;
}

static void verifyFont (GpFont *font, GpFontFamily *originalFamily, INT expectedStyle, Unit expectedUnit)
{
	GpStatus status;
	GpFontFamily *family;
	WCHAR originalFamilyName[LF_FACESIZE];
	WCHAR familyName[LF_FACESIZE];
	Unit unit;
	INT style;

	assert (font && "Expected font to be initialized.");

	status = GdipGetFamily (font, &family);
	ASSERT_EQ (Ok, status);
	assert (family && family != originalFamily);

	status = GdipGetFamilyName (originalFamily, originalFamilyName, 0);
	ASSERT_EQ (Ok, status);

	status = GdipGetFamilyName (family, familyName, 0);
	ASSERT_EQ (Ok, status);
	assert (!strcmp ((char *) originalFamilyName, (char *) familyName));

	status = GdipGetFontUnit (font, &unit);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedUnit, unit);

	status = GdipGetFontStyle(font, &style);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (expectedStyle, style);

	GdipDeleteFontFamily (family);
}

TEST(Font, GdipNewPrivateFontCollection)
{
	STARTUP

	GpStatus status;
	GpFontCollection *collection;
	INT count;

	status = GdipNewPrivateFontCollection (&collection);
	ASSERT_EQ (Ok, status);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, count);

	// Negative tests.
	status = GdipNewPrivateFontCollection (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePrivateFontCollection (&collection);

	SHUTDOWN
}

TEST(Font, GdipDeletePrivateFontCollection)
{
	STARTUP

	GpStatus status;
	GpFontCollection *collection;

	GdipNewPrivateFontCollection(&collection);

	status = GdipDeletePrivateFontCollection (&collection);
	ASSERT_EQ (Ok, status);
	assert (!collection && "Expected the pointer to be set to NULL.");

	// Negative tests.
	status = GdipDeletePrivateFontCollection (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(Font, GdipGetFontCollectionFamilyCount)
{
	STARTUP

	GpStatus status;
	GpFontCollection *collection;
	INT count;

	GdipNewPrivateFontCollection(&collection);

	// Negative tests.
	status = GdipGetFontCollectionFamilyCount (NULL, &count);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontCollectionFamilyCount (collection, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePrivateFontCollection (&collection);

	SHUTDOWN
}

TEST(Font, GdipGetFontCollectionFamilyList)
{
	STARTUP

	GpStatus status;
	WCHAR *fontFile;
	GpFontCollection *collection;
	GpFontFamily *families[2] = {NULL, NULL};
	INT numFound;

	fontFile = createWchar ("test.ttf");
	GdipNewPrivateFontCollection(&collection);

	//Empty list.
	status = GdipGetFontCollectionFamilyList (collection, 10, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, numFound);

	status = GdipGetFontCollectionFamilyList (collection, 0, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, numFound);

	status = GdipGetFontCollectionFamilyList (collection, -1, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, numFound);

	// Non empty list.
	GdipPrivateAddFontFile (collection, fontFile);
	status = GdipGetFontCollectionFamilyList (collection, 0, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, numFound);
	assert (families[0] == NULL);
	assert (families[1] == NULL);

	// Negative tests.
	status = GdipGetFontCollectionFamilyList (NULL, 10, families, &numFound);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontCollectionFamilyList (collection, 10, NULL, &numFound);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontCollectionFamilyList (collection, 10, families, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePrivateFontCollection (&collection);
	freeWchar (fontFile);

	SHUTDOWN
}

TEST(Font, GdipPrivateAddFontFile)
{
	STARTUP

	GpStatus status;
	GpFontCollection *collection;
	WCHAR *ttfFile;
	WCHAR *otfFile;
	WCHAR *invalidFile;
	WCHAR *noSuchFile;
	INT count;
	GpFontFamily *families[1];
	INT numFound;
	WCHAR name[LF_FACESIZE];

	GdipNewPrivateFontCollection (&collection);
	ttfFile = createWchar ("test.ttf");
	otfFile = createWchar ("test.otf");
	invalidFile = createWchar ("test.bmp");
	noSuchFile = createWchar ("noSuchFile.ttf");

	// Valid TTF file.
	status = GdipPrivateAddFontFile (collection, ttfFile);
	ASSERT_EQ (Ok, status);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, numFound);

	status = GdipGetFamilyName (families[0], name, 0);
	ASSERT_EQ (Ok, status);
	assert (stringsEqual (name, "Code New Roman"));

	// This causes a crash in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	GdipDeleteFontFamily (families[0]);
#endif
	GdipDeletePrivateFontCollection (&collection);

	// Valid OTF file.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddFontFile (collection, otfFile);
	ASSERT_EQ (Ok, status);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, numFound);

	status = GdipGetFamilyName (families[0], name, 0);
	ASSERT_EQ (Ok, status);
	assert (stringsEqual (name, "Code New Roman"));

	// This causes a crash in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	GdipDeleteFontFamily (families[0]);
#endif
	GdipDeletePrivateFontCollection (&collection);

	// Invalid file.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddFontFile (collection, invalidFile);
	ASSERT_EQ (Ok, status);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, count);

	// Negative tests.
	status = GdipPrivateAddFontFile (NULL, ttfFile);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipPrivateAddFontFile (collection, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipPrivateAddFontFile (collection, noSuchFile);
	ASSERT_EQ (FileNotFound, status);

	GdipDeletePrivateFontCollection (&collection);
	freeWchar (ttfFile);
	freeWchar (otfFile);
	freeWchar (invalidFile);
	freeWchar (noSuchFile);

	SHUTDOWN
}

TEST(Font, GdipPrivateAddMemoryFont)
{
	STARTUP

	GpStatus status;
	GpFontCollection *collection;
	void *memory;
	INT memoryLength;
	long invalidMemory = 1;
	INT count;
	GpFontFamily *families[1];
	INT numFound;
	WCHAR name[LF_FACESIZE];

	GdipNewPrivateFontCollection (&collection);
	memory = readFile ("test.ttf", &memoryLength);

	// Valid TTF file.
	status = GdipPrivateAddMemoryFont (collection, memory, memoryLength);
	ASSERT_EQ (Ok, status);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, numFound);

	status = GdipGetFamilyName (families[0], name, 0);
	ASSERT_EQ (Ok, status);
	assert (stringsEqual (name, "Code New Roman"));

	free (memory);
	// This causes a crash in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	GdipDeleteFontFamily (families[0]);
#endif
	GdipDeletePrivateFontCollection (&collection);

	// Valid OTF file.
	GdipNewPrivateFontCollection (&collection);
	memory = readFile ("test.otf", &memoryLength);

	status = GdipPrivateAddMemoryFont (collection, memory, memoryLength);
	ASSERT_EQ (Ok, status);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, count);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (1, numFound);

	status = GdipGetFamilyName (families[0], name, 0);
	ASSERT_EQ (Ok, status);
	assert (stringsEqual (name, "Code New Roman"));

	free (memory);
	// This causes a crash in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	GdipDeleteFontFamily (families[0]);
#endif
	GdipDeletePrivateFontCollection (&collection);

	// Invalid memory is ignored.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, 1);
	ASSERT_EQ (Ok, status);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (0, count);

	// Negative tests.
	status = GdipPrivateAddMemoryFont (NULL, &invalidMemory, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipPrivateAddMemoryFont (collection, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, -1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, 0);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeletePrivateFontCollection (&collection);

	SHUTDOWN
}

TEST(Font, GdipNewInstalledFontCollection)
{
	STARTUP

	GpStatus status;
	GpFontCollection *collection1;
	GpFontCollection *collection2;
	INT count1;
	INT count2;

	status = GdipNewInstalledFontCollection (&collection1);
	ASSERT_EQ (Ok, status);
	assert (collection1 && "Expected collection to be initialized.");

	status = GdipGetFontCollectionFamilyCount (collection1, &count1);
	ASSERT_EQ (Ok, status);
	assert (count1 >= 0);

	status = GdipNewInstalledFontCollection (&collection2);
	ASSERT_EQ (Ok, status);
	assert (collection1 == collection2 && "Expected the FontCollecion to be a singleton.");

	status = GdipGetFontCollectionFamilyCount (collection2, &count2);
	ASSERT_EQ (Ok, status);
	assert (count1 == count2 && "Expected the FontCollection count to remain constant over multiple calls.");

	SHUTDOWN
}

TEST(Font, GdipCreateFontFromDC)
{
	STARTUP

	GpStatus status;
	GpFont *font;
	HDC hdc;

	hdc = getEmptyHDC ();

	// Negative tests.
	status = GdipCreateFontFromDC (NULL, &font);
	ASSERT_EQ (InvalidParameter, status);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFromDC (hdc, NULL);
	ASSERT_EQ (InvalidParameter, status);
#endif

SHUTDOWN
}

TEST(Font, GdipCreateFontFromLogfontA)
{
	STARTUP

	GpStatus status;
	GpFont *font;
	LOGFONTA logfont;
	HDC hdc;
	GpFontFamily *nativeFamily;
	WCHAR nativeFamilyNameW[LF_FACESIZE];
	char *nativeFamilyNameA;
	INT style;
	Unit unit;
	GpFontFamily *family;
	WCHAR familyName[LF_FACESIZE];

	hdc = getEmptyHDC ();
	GdipGetGenericFontFamilySansSerif (&nativeFamily);
	GdipGetFamilyName (nativeFamily, nativeFamilyNameW, 0);
	nativeFamilyNameA = charFromWchar (nativeFamilyNameW);

	logfont.lfHeight = 10;
	logfont.lfWidth = 11;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 1;
	logfont.lfWeight = 700;
	logfont.lfItalic = TRUE;
	logfont.lfUnderline = TRUE;
	logfont.lfStrikeOut = TRUE;
	logfont.lfCharSet = 0;
	logfont.lfOutPrecision = 1;
	logfont.lfClipPrecision = 2;
	logfont.lfQuality = 4;
	logfont.lfPitchAndFamily = 0x50;
	strcpy (logfont.lfFaceName, nativeFamilyNameA);

	status = GdipCreateFontFromLogfontA (hdc, &logfont, &font);
	ASSERT_EQ (Ok, status);

	GdipGetFontStyle (font, &style);
	ASSERT_EQ (15, style);

	GdipGetFontUnit(font, &unit);
	ASSERT_EQ (UnitWorld, unit);

	status = GdipGetFamily (font, &family);
	ASSERT_EQ (Ok, status);
	GdipGetFamilyName (family, familyName, 0);
	assert (stringsEqual (familyName, nativeFamilyNameA));

	// Negative tests.
	status = GdipCreateFontFromLogfontA (NULL, &logfont, &font);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFontFromLogfontA (hdc, NULL, &font);
	ASSERT_EQ (InvalidParameter, status);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFromLogfontA (hdc, &logfont, NULL);
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);
	GdipDeleteFontFamily (nativeFamily);
	freeChar (nativeFamilyNameA);

	SHUTDOWN
}

TEST(Font, GdipCreateFontFromLogfontW)
{
	STARTUP

	GpStatus status;
	GpFont *font;
	LOGFONTW logfont;
	HDC hdc;
	GpFontFamily *nativeFamily;
	WCHAR nativeFamilyNameW[LF_FACESIZE];
	char *nativeFamilyNameA;
	INT style;
	Unit unit;
	GpFontFamily *family;
	WCHAR familyName[LF_FACESIZE];

	hdc = getEmptyHDC ();
	GdipGetGenericFontFamilySansSerif (&nativeFamily);
	GdipGetFamilyName (nativeFamily, nativeFamilyNameW, 0);
	nativeFamilyNameA = charFromWchar (nativeFamilyNameW);

	logfont.lfHeight = 10;
	logfont.lfWidth = 11;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 1;
	logfont.lfWeight = 700;
	logfont.lfItalic = TRUE;
	logfont.lfUnderline = TRUE;
	logfont.lfStrikeOut = TRUE;
	logfont.lfCharSet = 0;
	logfont.lfOutPrecision = 1;
	logfont.lfClipPrecision = 2;
	logfont.lfQuality = 4;
	logfont.lfPitchAndFamily = 0x50;
	memcpy ((void *) logfont.lfFaceName, (void *) nativeFamilyNameW, LF_FACESIZE * sizeof (WCHAR));

	status = GdipCreateFontFromLogfontW (hdc, &logfont, &font);
	ASSERT_EQ (Ok, status);

	GdipGetFontStyle (font, &style);
	ASSERT_EQ (15, style);

	GdipGetFontUnit(font, &unit);
	ASSERT_EQ (UnitWorld, unit);

	status = GdipGetFamily (font, &family);
	ASSERT_EQ (Ok, status);
	GdipGetFamilyName (family, familyName, 0);
	assertEqualString (familyName, nativeFamilyNameA);

	// Negative tests.
	status = GdipCreateFontFromLogfontW (NULL, &logfont, &font);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFontFromLogfontW (hdc, NULL, &font);
	ASSERT_EQ (InvalidParameter, status);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFromLogfontW (hdc, &logfont, NULL);
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);
	GdipDeleteFontFamily (nativeFamily);
	freeChar (nativeFamilyNameA);

	SHUTDOWN
}

TEST(Font, GdipCreateFont)
{
	STARTUP

	GpStatus status;
	GpFont *font;
	GpFontFamily *family;

	GdipGetGenericFontFamilySansSerif (&family);

	// UnitPixel.
	status = GdipCreateFont (family, 10, 10, UnitPixel, &font);
	ASSERT_EQ (Ok, status);
	verifyFont (font, family, 10, UnitPixel);

	// Negative tests.
	status = GdipCreateFont (NULL, 10, 10, UnitPixel, &font);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFont (family, 10, 10, UnitPixel, NULL);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFont (family, -1, 10, UnitPixel, &font);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFont (family, 0, 10, UnitPixel, &font);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFont (family, 10, 10, UnitDisplay, &font);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFont (family, 10, 10, (Unit)(UnitWorld - 1), &font);
	ASSERT_EQ (InvalidParameter, status);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFont (family, 10, 10, (Unit)(UnitMillimeter + 1), &font);
	ASSERT_EQ (InvalidParameter, status);
#else
	status = GdipCreateFont (family, 10, 10, (Unit)(UnitCairoPoint + 1), &font);
	ASSERT_EQ (InvalidParameter, status);
#endif

	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipCloneFont)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	GpFont *font;
	GpFont *clonedFont;

	GdipGetGenericFontFamilySansSerif (&family);
	GdipCreateFont (family, 10, 10, UnitPixel, &font);

	status = GdipCloneFont (font, &clonedFont);
	ASSERT_EQ (Ok, status);
	assert (clonedFont && font != clonedFont);
	verifyFont (clonedFont, family, 10, UnitPixel);

	// Negative tests.
	status = GdipCloneFont (NULL, &clonedFont);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCloneFont (font, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFont (clonedFont);
	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipDeleteFont)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	GpFont *font;

	GdipGetGenericFontFamilySansSerif (&family);

	GdipCreateFont (family, 10, 10, UnitPixel, &font);

	status = GdipDeleteFont (font);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDeleteFont (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipGetFamily)
{
	STARTUP

	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	GpFontFamily *family;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	// Negative tests.
	status = GdipGetFamily (NULL, &family);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFamily (font, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);

	SHUTDOWN
}

TEST(Font, GdipGetFontStyle)
{
	STARTUP

	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	INT style;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	// Negative tests.
	status = GdipGetFontStyle (NULL, &style);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontStyle (font, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);

	SHUTDOWN
}

TEST(Font, GdipGetFontSize)
{
	STARTUP

	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	REAL size;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	status = GdipGetFontSize (font, &size);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipGetFontSize (NULL, &size);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontSize (font, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);

	SHUTDOWN
}

TEST(Font, GdipGetFontUnit)
{
	STARTUP

	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	Unit unit;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	// Negative tests.
	status = GdipGetFontUnit (NULL, &unit);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontUnit (font, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);

	SHUTDOWN
}

TEST(Font, GdipGetFontHeight)
{
	STARTUP

	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	REAL height;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	status = GdipGetFontHeight (font, NULL, &height);
	ASSERT_EQ (Ok, status);
	// FIXME: this returns a different value with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (height, 11.3183594f);
#endif

	// Negative tests.
	status = GdipGetFontHeight (NULL, NULL, &height);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontHeight (font, NULL, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);

	SHUTDOWN
}

TEST(Font, GdipGetFontHeightGivenDPI)
{
	STARTUP

	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	REAL height;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	status = GdipGetFontHeightGivenDPI (font, 10, &height);
	ASSERT_EQ (Ok, status);
	// FIXME: this returns a different value with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (height, 11.3183594f);
#endif

	// Negative tests.
	status = GdipGetFontHeightGivenDPI (NULL, 10, &height);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFontHeightGivenDPI (font, 10, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);

	SHUTDOWN
}

TEST(Font, GdipGetLogfontA)
{
	STARTUP

	GpStatus status;
	GpBitmap *image;
	GpGraphics *graphics;
	GpFontFamily *originalFamily;
	WCHAR originalFamilyName[32];
	GpFont *font;
	LOGFONTA logfont;

	GdipCreateBitmapFromScan0 (100, 68, 0, PixelFormat32bppRGB, NULL, &image);
	GdipGetImageGraphicsContext (image, &graphics);
	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);
	GdipGetFamilyName (originalFamily, originalFamilyName, 0);

	status = GdipGetLogFontA (font, graphics, &logfont);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (-10, logfont.lfHeight);
	ASSERT_EQ (0, logfont.lfWidth);
	ASSERT_EQ (0, logfont.lfEscapement);
	ASSERT_EQ (0, logfont.lfOrientation);
	ASSERT_EQ (400, logfont.lfWeight);
	ASSERT_EQ (0, logfont.lfCharSet);
	ASSERT_EQ (0, logfont.lfOutPrecision);
	ASSERT_EQ (0, logfont.lfClipPrecision);
	ASSERT_EQ (0, logfont.lfQuality);
	ASSERT_EQ (0, logfont.lfPitchAndFamily);
	assert (stringsEqual (originalFamilyName, logfont.lfFaceName));

	// Negative tests.
	status = GdipGetLogFontA (NULL, graphics, &logfont);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetLogFontA (font, NULL, &logfont);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetLogFontA (font, graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
	GdipDeleteGraphics (graphics);
	GdipDisposeImage ((GpImage *) image);

	SHUTDOWN
}

TEST(Font, GdipGetLogfontW)
{
	STARTUP

	GpStatus status;
	GpBitmap *image;
	GpGraphics *graphics;
	GpFontFamily *originalFamily;
	WCHAR originalFamilyName[32];
	GpFont *font;
	LOGFONTW logfont;

	GdipCreateBitmapFromScan0 (100, 68, 0, PixelFormat32bppRGB, NULL, &image);
	GdipGetImageGraphicsContext (image, &graphics);
	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);
	GdipGetFamilyName (originalFamily, originalFamilyName, 0);

	status = GdipGetLogFontW (font, graphics, &logfont);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (-10, logfont.lfHeight);
	ASSERT_EQ (0, logfont.lfWidth);
	ASSERT_EQ (0, logfont.lfEscapement);
	ASSERT_EQ (0, logfont.lfOrientation);
	ASSERT_EQ (400, logfont.lfWeight);
	ASSERT_EQ (0, logfont.lfCharSet);
	ASSERT_EQ (0, logfont.lfOutPrecision);
	ASSERT_EQ (0, logfont.lfClipPrecision);
	ASSERT_EQ (0, logfont.lfQuality);
	ASSERT_EQ (0, logfont.lfPitchAndFamily);
	assert (!strcmp ((char *) originalFamilyName, (char *) logfont.lfFaceName));

	// Negative tests.
	status = GdipGetLogFontW (NULL, graphics, &logfont);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetLogFontW (font, NULL, &logfont);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetLogFontW (font, graphics, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
	GdipDeleteGraphics (graphics);
	GdipDisposeImage ((GpImage *) image);

	SHUTDOWN
}

static const WCHAR Tahoma[] = {'T', 'a', 'h', 'o', 'm', 'a', 0};
static const WCHAR CodeNewRoman[] = {'C', 'o', 'd', 'e', ' ', 'N', 'e', 'w', ' ', 'R', 'o', 'm', 'a', 'n', 0};
static const WCHAR NoSuchFont[] = {'A', 'B', 'C', 0};

static void verifyFontFamily (GpFontFamily *family, const char *expectedName, UINT16 expectedCellAscent, UINT16 expectedCellDescent, UINT16 expectedLineSpacing, const char *alternateExpectedName, UINT16 alternateCellAscent, UINT16 alternateCellDescent, UINT16 alternateLineSpacing)
{
	GpStatus status;
	WCHAR name[LF_FACESIZE];
	UINT16 emHeight;
	UINT16 cellAscent;
	UINT16 cellDescent;
	UINT16 lineSpacing;

	status = GdipGetFamilyName (family, name, 1);
	ASSERT_EQ (Ok, status);
	assert (stringsEqual (name, expectedName) || stringsEqual (name, alternateExpectedName));

	status = GdipGetEmHeight (family, FontStyleRegular, &emHeight);
	ASSERT_EQ (Ok, status);
	ASSERT_EQ (2048, emHeight);

	status = GdipGetCellAscent (family, FontStyleRegular, &cellAscent);
	ASSERT_EQ (Ok, status);
	assert (cellAscent == expectedCellAscent || cellAscent == alternateCellAscent);

	status = GdipGetCellDescent (family, FontStyleRegular, &cellDescent);
	ASSERT_EQ (Ok, status);
	assert (cellDescent == expectedCellDescent || cellDescent == alternateCellDescent);

	status = GdipGetLineSpacing (family, FontStyleRegular, &lineSpacing);
	ASSERT_EQ (Ok, status);
	assert (lineSpacing == expectedLineSpacing || lineSpacing == alternateLineSpacing);

	GdipDeleteFontFamily (family);
}

TEST(Font, GdipCreateFontFamilyFromName)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	GpFontCollection *collection;
	WCHAR *fontName;

	GdipNewPrivateFontCollection (&collection);
	fontName = createWchar ("test.ttf");
	GdipPrivateAddFontFile (collection, fontName);

	status = GdipCreateFontFamilyFromName (Tahoma, NULL, &family);
	ASSERT_EQ (Ok, status);
	verifyFontFamily (family, "Tahoma", 2049, 423, 2472, "DejaVu Sans", 1901, 483, 2384);

	status = GdipCreateFontFamilyFromName (CodeNewRoman, collection, &family);
	ASSERT_EQ (Ok, status);
	verifyFontFamily (family, "Code New Roman", 1884, 514, 2398, "Code New Roman", 1884, 514, 2398);

	// Negative tests.
	status = GdipCreateFontFamilyFromName (NULL, collection, &family);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCreateFontFamilyFromName (CodeNewRoman, collection, NULL);
	ASSERT_EQ (InvalidParameter, status);

	// FIXME: Libgdiplus does not validate that the font family exists
	// if the collection is NULL.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFamilyFromName (NoSuchFont, NULL, &family);
	ASSERT_EQ (FontFamilyNotFound, status);
#endif

	status = GdipCreateFontFamilyFromName (Tahoma, collection, &family);
	ASSERT_EQ (FontFamilyNotFound, status);

	status = GdipCreateFontFamilyFromName (NoSuchFont, collection, &family);
	ASSERT_EQ (FontFamilyNotFound, status);

	GdipDeletePrivateFontCollection (&collection);
	freeWchar (fontName);

	SHUTDOWN
}

TEST(Font, GdipCloneFontFamily)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	GpFontFamily *clonedFamily;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipCloneFontFamily (family, &clonedFamily);
	ASSERT_EQ (Ok, status);
	verifyFontFamily (clonedFamily, "Tahoma", 2049, 423, 2472, "DejaVu Sans", 1901, 483, 2384);

	// Negative tests.
	status = GdipCloneFontFamily (NULL, &clonedFamily);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipCloneFontFamily (family, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipDeleteFontFamily)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipDeleteFontFamily (family);
	ASSERT_EQ (Ok, status);

	// Negative tests.
	status = GdipDeleteFontFamily (NULL);
	ASSERT_EQ (InvalidParameter, status);

	SHUTDOWN
}

TEST(Font, GdipGetGenericFontFamilySansSerif)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilySansSerif  (&family1);
	ASSERT_EQ (Ok, status);
	assert (family1);

	status = GdipGetGenericFontFamilySansSerif (&family2);
	ASSERT_EQ (Ok, status);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilySansSerif (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);

	SHUTDOWN
}

TEST(Font, GdipGetGenericFontFamilySerif)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilySerif  (&family1);
	ASSERT_EQ (Ok, status);
	assert (family1);

	status = GdipGetGenericFontFamilySerif  (&family2);
	ASSERT_EQ (Ok, status);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilySerif (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);

	SHUTDOWN
}

TEST(Font, GdipGetGenericFontFamilyMonospace)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilyMonospace  (&family1);
	ASSERT_EQ (Ok, status);
	assert (family1);

	status = GdipGetGenericFontFamilyMonospace  (&family2);
	ASSERT_EQ (Ok, status);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilyMonospace (NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);

	SHUTDOWN
}

TEST(Font, GdipGetFamilyName)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	WCHAR name[LF_FACESIZE];

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipGetFamilyName (family, name, 57920);
	ASSERT_EQ (Ok, status);
	assert (stringsEqual(name, "Tahoma") || stringsEqual (name, "DejaVu Sans"));

	// Negative tests.
	status = GdipGetFamilyName (NULL, name, 1);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetFamilyName (family, NULL, 1);
	ASSERT_EQ (Ok, status);

	status = GdipGetFamilyName (NULL, NULL, 1);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, isStyleAvailable)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	BOOL isStyleAvailable;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipIsStyleAvailable (family, FontStyleBold, &isStyleAvailable);
	ASSERT_EQ (Ok, status);
	assert (isStyleAvailable);

	status = GdipIsStyleAvailable (family, -1, &isStyleAvailable);
	ASSERT_EQ (Ok, status);
	assert (isStyleAvailable);

	status = GdipIsStyleAvailable (family, FontStyleStrikeout + 1, &isStyleAvailable);
	ASSERT_EQ (Ok, status);
	assert (isStyleAvailable);

	// Negative tests.
	status = GdipIsStyleAvailable (NULL, FontStyleBold, &isStyleAvailable);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipIsStyleAvailable (family, FontStyleBold, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipGetEmHeight)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	UINT16 emHeight;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetEmHeight (NULL, FontStyleBold, &emHeight);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetEmHeight (family, FontStyleBold, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipGetCellAscent)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	UINT16 cellAscent;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetCellAscent (NULL, FontStyleBold, &cellAscent);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCellAscent (family, FontStyleBold, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipGetCellDescent)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	UINT16 cellDescent;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetCellDescent (NULL, FontStyleBold, &cellDescent);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetCellDescent (family, FontStyleBold, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}

TEST(Font, GdipGetLineSpacing)
{
	STARTUP

	GpStatus status;
	GpFontFamily *family;
	UINT16 lineSpacing;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetLineSpacing (NULL, FontStyleBold, &lineSpacing);
	ASSERT_EQ (InvalidParameter, status);

	status = GdipGetLineSpacing (family, FontStyleBold, NULL);
	ASSERT_EQ (InvalidParameter, status);

	GdipDeleteFontFamily (family);

	SHUTDOWN
}
