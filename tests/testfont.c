#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	assertEqualInt (status, Ok);
	assert (family && family != originalFamily);

	status = GdipGetFamilyName (originalFamily, originalFamilyName, 0);
	assertEqualInt (status, Ok);

	status = GdipGetFamilyName (family, familyName, 0);
	assertEqualInt (status, Ok);
	assert (!strcmp ((char *) originalFamilyName, (char *) familyName));

	status = GdipGetFontUnit (font, &unit);
	assertEqualInt (status, Ok);
	assertEqualInt (unit, expectedUnit);

	status = GdipGetFontStyle(font, &style);
	assertEqualInt (status, Ok);
	assertEqualInt (style, expectedStyle);

	GdipDeleteFontFamily (family);
}

static void test_newPrivateFontCollection ()
{
	GpStatus status;
	GpFontCollection *collection;
	INT count;

	status = GdipNewPrivateFontCollection (&collection);
	assertEqualInt (status, Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count,  0);

	// Negative tests.
	status = GdipNewPrivateFontCollection (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePrivateFontCollection (&collection);
}

static void test_deletePrivateFontCollection ()
{
	GpStatus status;
	GpFontCollection *collection;

	GdipNewPrivateFontCollection(&collection);

	status = GdipDeletePrivateFontCollection (&collection);
	assertEqualInt (status, Ok);
	assert (!collection && "Expected the pointer to be set to NULL.");

	// Negative tests.
	status = GdipDeletePrivateFontCollection (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_getFontCollectionFamilyCount ()
{
	GpStatus status;
	GpFontCollection *collection;
	INT count;

	GdipNewPrivateFontCollection(&collection);

	// Negative tests.
	status = GdipGetFontCollectionFamilyCount (NULL, &count);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontCollectionFamilyCount (collection, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePrivateFontCollection (&collection);
}

static void test_getFontCollectionFamilyList ()
{
	GpStatus status;
	WCHAR *fontFile;
	GpFontCollection *collection;
	GpFontFamily *families[2] = {NULL, NULL};
	INT numFound;

	fontFile = createWchar ("test.ttf");
	GdipNewPrivateFontCollection(&collection);

	//Empty list.
	status = GdipGetFontCollectionFamilyList (collection, 10, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 0);

	status = GdipGetFontCollectionFamilyList (collection, 0, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 0);

	status = GdipGetFontCollectionFamilyList (collection, -1, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 0);

	// Non empty list.
	GdipPrivateAddFontFile (collection, fontFile);
	status = GdipGetFontCollectionFamilyList (collection, 0, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 0);
	assert (families[0] == NULL);
	assert (families[1] == NULL);

	// Negative tests.
	status = GdipGetFontCollectionFamilyList (NULL, 10, families, &numFound);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontCollectionFamilyList (collection, 10, NULL, &numFound);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontCollectionFamilyList (collection, 10, families, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePrivateFontCollection (&collection);
	freeWchar (fontFile);
}

static void test_privateAddFontFile ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count,  1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assertEqualInt (status, Ok);
	assert (stringsEqual (name, "Code New Roman"));

	GdipDeletePrivateFontCollection (&collection);

	// Valid OTF file.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddFontFile (collection, otfFile);
	assertEqualInt (status, Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count,  1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assertEqualInt (status, Ok);
	assert (stringsEqual (name, "Code New Roman"));

	GdipDeletePrivateFontCollection (&collection);

	// Invalid file.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddFontFile (collection, invalidFile);
	assertEqualInt (status, Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count,  0);

	// Negative tests.
	status = GdipPrivateAddFontFile (NULL, ttfFile);
	assertEqualInt (status, InvalidParameter);

	status = GdipPrivateAddFontFile (collection, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipPrivateAddFontFile (collection, noSuchFile);
	assertEqualInt (status, FileNotFound);

	GdipDeletePrivateFontCollection (&collection);
	freeWchar (ttfFile);
	freeWchar (otfFile);
	freeWchar (invalidFile);
	freeWchar (noSuchFile);
}

static void test_privateAddMemoryFont ()
{
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
	assertEqualInt (status, Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count,  1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assertEqualInt (status, Ok);
	assert (stringsEqual (name, "Code New Roman"));

	free (memory);
	GdipDeletePrivateFontCollection (&collection);

	// Valid OTF file.
	GdipNewPrivateFontCollection (&collection);
	memory = readFile ("test.otf", &memoryLength);

	status = GdipPrivateAddMemoryFont (collection, memory, memoryLength);
	assertEqualInt (status, Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count,  1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assertEqualInt (status, Ok);
	assertEqualInt (numFound, 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assertEqualInt (status, Ok);
	assert (stringsEqual (name, "Code New Roman"));

	free (memory);
	GdipDeletePrivateFontCollection (&collection);

	// Invalid memory is ignored.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, 1);
	assertEqualInt (status, Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assertEqualInt (status, Ok);
	assertEqualInt (count,  0);

	// Negative tests.
	status = GdipPrivateAddMemoryFont (NULL, &invalidMemory, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipPrivateAddMemoryFont (collection, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, -1);
	assertEqualInt (status, InvalidParameter);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, 0);
	assertEqualInt (status, InvalidParameter);

	GdipDeletePrivateFontCollection (&collection);
}

static void test_newInstalledFontCollection ()
{
	GpStatus status;
	GpFontCollection *collection1;
	GpFontCollection *collection2;
	INT count1;
	INT count2;

	status = GdipNewInstalledFontCollection (&collection1);
	assertEqualInt (status, Ok);
	assert (collection1 && "Expected collection to be initialized.");

	status = GdipGetFontCollectionFamilyCount (collection1, &count1);
	assertEqualInt (status, Ok);
	assert (count1 >= 0);

	status = GdipNewInstalledFontCollection (&collection2);
	assertEqualInt (status, Ok);
	assert (collection1 == collection2 && "Expected the FontCollecion to be a singleton.");

	status = GdipGetFontCollectionFamilyCount (collection2, &count2);
	assertEqualInt (status, Ok);
	assert (count1 == count2 && "Expected the FontCollection count to remain constant over multiple calls.");
}

static void test_createFontFromDC ()
{
	GpStatus status;
	GpFont *font;
	HDC hdc;

	hdc = getEmptyHDC ();

	// Negative tests.
	status = GdipCreateFontFromDC (NULL, &font);
	assertEqualInt (status, InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFromDC (hdc, NULL);
	assertEqualInt (status, InvalidParameter);
#endif
}


static void test_createFontFromLogfontA ()
{
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
	assertEqualInt (status, Ok);

	GdipGetFontStyle (font, &style);
	assertEqualInt (style, 15);

	GdipGetFontUnit(font, &unit);
	assertEqualInt (unit, UnitWorld);

	status = GdipGetFamily (font, &family);
	assertEqualInt (status, Ok);
	GdipGetFamilyName (family, familyName, 0);
	assert (stringsEqual (familyName, nativeFamilyNameA));

	// Negative tests.
	status = GdipCreateFontFromLogfontA (NULL, &logfont, &font);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFontFromLogfontA (hdc, NULL, &font);
	assertEqualInt (status, InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFromLogfontA (hdc, &logfont, NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);
	GdipDeleteFontFamily (nativeFamily);
	freeChar (nativeFamilyNameA);
}

static void test_createFontFromLogfontW ()
{
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
	assertEqualInt (status, Ok);

	GdipGetFontStyle (font, &style);
	assertEqualInt (style, 15);

	GdipGetFontUnit(font, &unit);
	assertEqualInt (unit, UnitWorld);

	status = GdipGetFamily (font, &family);
	assertEqualInt (status, Ok);
	GdipGetFamilyName (family, familyName, 0);
	assertEqualString (familyName, nativeFamilyNameA);

	// Negative tests.
	status = GdipCreateFontFromLogfontW (NULL, &logfont, &font);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFontFromLogfontW (hdc, NULL, &font);
	assertEqualInt (status, InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFromLogfontW (hdc, &logfont, NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);
	GdipDeleteFontFamily (nativeFamily);
	freeChar (nativeFamilyNameA);
}

static void test_createFont ()
{
	GpStatus status;
	GpFont *font;
	GpFontFamily *family;

	GdipGetGenericFontFamilySansSerif (&family);

	// UnitPixel.
	status = GdipCreateFont (family, 10, 10, UnitPixel, &font);
	assertEqualInt (status, Ok);
	verifyFont (font, family, 10, UnitPixel);

	// Negative tests.
	status = GdipCreateFont (NULL, 10, 10, UnitPixel, &font);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFont (family, 10, 10, UnitPixel, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFont (family, -1, 10, UnitPixel, &font);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFont (family, 0, 10, UnitPixel, &font);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFont (family, 10, 10, UnitDisplay, &font);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFont (family, 10, 10, (Unit)(UnitWorld - 1), &font);
	assertEqualInt (status, InvalidParameter);

#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFont (family, 10, 10, (Unit)(UnitMillimeter + 1), &font);
	assertEqualInt (status, InvalidParameter);
#else
	status = GdipCreateFont (family, 10, 10, (Unit)(UnitCairoPoint + 1), &font);
	assertEqualInt (status, InvalidParameter);
#endif

	GdipDeleteFont (font);
	GdipDeleteFontFamily (family);
}

static void test_cloneFont ()
{
	GpStatus status;
	GpFontFamily *family;
	GpFont *font;
	GpFont *clonedFont;

	GdipGetGenericFontFamilySansSerif (&family);
	GdipCreateFont (family, 10, 10, UnitPixel, &font);

	status = GdipCloneFont (font, &clonedFont);
	assertEqualInt (status, Ok);
	assert (clonedFont && font != clonedFont);
	verifyFont (clonedFont, family, 10, UnitPixel);

	// Negative tests.
	status = GdipCloneFont (NULL, &clonedFont);
	assertEqualInt (status, InvalidParameter);

	status = GdipCloneFont (font, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFont (clonedFont);
}

static void test_deleteFont ()
{
	GpStatus status;
	GpFontFamily *family;
	GpFont *font;

	GdipGetGenericFontFamilySansSerif (&family);

	GdipCreateFont (family, 10, 10, UnitPixel, &font);

	status = GdipDeleteFont (font);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDeleteFont (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_getFamily ()
{
	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	GpFontFamily *family;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	// Negative tests.
	status = GdipGetFamily (NULL, &family);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFamily (font, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
}

static void test_getFontStyle ()
{
	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	INT style;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	// Negative tests.
	status = GdipGetFontStyle (NULL, &style);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontStyle (font, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
}

static void test_getFontSize ()
{
	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	REAL size;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	status = GdipGetFontSize (font, &size);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipGetFontSize (NULL, &size);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontSize (font, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
}

static void test_getFontUnit ()
{
	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	Unit unit;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	// Negative tests.
	status = GdipGetFontUnit (NULL, &unit);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontUnit (font, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
}

static void test_getFontHeight ()
{
	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	REAL height;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	status = GdipGetFontHeight (font, NULL, &height);
	assertEqualInt (status, Ok);
	// FIXME: this returns a different value with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (height, 11.3183594f);
#endif

	// Negative tests.
	status = GdipGetFontHeight (NULL, NULL, &height);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontHeight (font, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
}

static void test_getFontHeightGivenDPI ()
{
	GpStatus status;
	GpFontFamily *originalFamily;
	GpFont *font;
	REAL height;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	status = GdipGetFontHeightGivenDPI (font, 10, &height);
	assertEqualInt (status, Ok);
	// FIXME: this returns a different value with libgdiplus.
#if defined(USE_WINDOWS_GDIPLUS)
	assertEqualFloat (height, 11.3183594f);
#endif

	// Negative tests.
	status = GdipGetFontHeightGivenDPI (NULL, 10, &height);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFontHeightGivenDPI (font, 10, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
}

static void test_getLogfontA ()
{
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
	assertEqualInt (status, Ok);
	assertEqualInt (logfont.lfHeight, -10);
	assertEqualInt (logfont.lfWidth, 0);
	assertEqualInt (logfont.lfEscapement, 0);
	assertEqualInt (logfont.lfOrientation, 0);
	assertEqualInt (logfont.lfWeight, 400);
	assertEqualInt (logfont.lfCharSet, 0);
	assertEqualInt (logfont.lfOutPrecision, 0);
	assertEqualInt (logfont.lfClipPrecision, 0);
	assertEqualInt (logfont.lfQuality, 0);
	assertEqualInt (logfont.lfPitchAndFamily, 0);
	assert (stringsEqual (originalFamilyName, logfont.lfFaceName));

	// Negative tests.
	status = GdipGetLogFontA (NULL, graphics, &logfont);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetLogFontA (font, NULL, &logfont);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetLogFontA (font, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
	GdipDisposeImage ((GpImage *) image);
}

static void test_getLogfontW ()
{
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
	assertEqualInt (status, Ok);
	assertEqualInt (logfont.lfHeight, -10);
	assertEqualInt (logfont.lfWidth, 0);
	assertEqualInt (logfont.lfEscapement, 0);
	assertEqualInt (logfont.lfOrientation, 0);
	assertEqualInt (logfont.lfWeight, 400);
	assertEqualInt (logfont.lfCharSet, 0);
	assertEqualInt (logfont.lfOutPrecision, 0);
	assertEqualInt (logfont.lfClipPrecision, 0);
	assertEqualInt (logfont.lfQuality, 0);
	assertEqualInt (logfont.lfPitchAndFamily, 0);
	assert (!strcmp ((char *) originalFamilyName, (char *) logfont.lfFaceName));

	// Negative tests.
	status = GdipGetLogFontW (NULL, graphics, &logfont);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetLogFontW (font, NULL, &logfont);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetLogFontW (font, graphics, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
	GdipDisposeImage ((GpImage *) image);
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
	assertEqualInt (status, Ok);
	assert (stringsEqual (name, expectedName) || stringsEqual (name, alternateExpectedName));

	status = GdipGetEmHeight (family, FontStyleRegular, &emHeight);
	assertEqualInt (status, Ok);
	assertEqualInt (emHeight, 2048);

	status = GdipGetCellAscent (family, FontStyleRegular, &cellAscent);
	assertEqualInt (status, Ok);
	assert (cellAscent == expectedCellAscent || cellAscent == alternateCellAscent);

	status = GdipGetCellDescent (family, FontStyleRegular, &cellDescent);
	assertEqualInt (status, Ok);
	assert (cellDescent == expectedCellDescent || cellDescent == alternateCellDescent);

	status = GdipGetLineSpacing (family, FontStyleRegular, &lineSpacing);
	assertEqualInt (status, Ok);
	assert (lineSpacing == expectedLineSpacing || lineSpacing == alternateLineSpacing);

	GdipDeleteFontFamily (family);
}

static void test_createFontFamilyFromName ()
{
	GpStatus status;
	GpFontFamily *family;
	GpFontCollection *collection;
	WCHAR *fontName;

	GdipNewPrivateFontCollection (&collection);
	fontName = createWchar ("test.ttf");
	GdipPrivateAddFontFile (collection, fontName);

	status = GdipCreateFontFamilyFromName (Tahoma, NULL, &family);
	assertEqualInt (status, Ok);
	verifyFontFamily (family, "Tahoma", 2049, 423, 2472, "DejaVu Sans", 1901, 483, 2384);

	status = GdipCreateFontFamilyFromName (CodeNewRoman, collection, &family);
	assertEqualInt (status, Ok);
	verifyFontFamily (family, "Code New Roman", 1884, 514, 2398, "Code New Roman", 1884, 514, 2398);

	// Negative tests.
	status = GdipCreateFontFamilyFromName (NULL, collection, &family);
	assertEqualInt (status, InvalidParameter);

	status = GdipCreateFontFamilyFromName (CodeNewRoman, collection, NULL);
	assertEqualInt (status, InvalidParameter);

	// FIXME: Libgdiplus does not validate that the font family exists
	// if the collection is NULL.
#if defined(USE_WINDOWS_GDIPLUS)
	status = GdipCreateFontFamilyFromName (NoSuchFont, NULL, &family);
	assertEqualInt (status, FontFamilyNotFound);
#endif

	status = GdipCreateFontFamilyFromName (Tahoma, collection, &family);
	assertEqualInt (status, FontFamilyNotFound);

	status = GdipCreateFontFamilyFromName (NoSuchFont, collection, &family);
	assertEqualInt (status, FontFamilyNotFound);

	GdipDeletePrivateFontCollection (&collection);
	freeWchar (fontName);
}

static void test_cloneFontFamily ()
{
	GpStatus status;
	GpFontFamily *family;
	GpFontFamily *clonedFamily;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipCloneFontFamily (family, &clonedFamily);
	assertEqualInt (status, Ok);
	verifyFontFamily (clonedFamily, "Tahoma", 2049, 423, 2472, "DejaVu Sans", 1901, 483, 2384);

	// Negative tests.
	status = GdipCloneFontFamily (NULL, &clonedFamily);
	assertEqualInt (status, InvalidParameter);

	status = GdipCloneFontFamily (family, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_deleteFontFamily ()
{
	GpStatus status;
	GpFontFamily *family;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipDeleteFontFamily (family);
	assertEqualInt (status, Ok);

	// Negative tests.
	status = GdipDeleteFontFamily (NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_getGenericFontFamilySansSerif ()
{
	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilySansSerif  (&family1);
	assertEqualInt (status, Ok);
	assert (family1);

	status = GdipGetGenericFontFamilySansSerif (&family2);
	assertEqualInt (status, Ok);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilySansSerif (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);
}

static void test_getGenericFontFamilySerif ()
{
	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilySerif  (&family1);
	assertEqualInt (status, Ok);
	assert (family1);

	status = GdipGetGenericFontFamilySerif  (&family2);
	assertEqualInt (status, Ok);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilySerif (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);
}

static void test_getGenericFontFamilyMonospace ()
{
	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilyMonospace  (&family1);
	assertEqualInt (status, Ok);
	assert (family1);

	status = GdipGetGenericFontFamilyMonospace  (&family2);
	assertEqualInt (status, Ok);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilyMonospace (NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);
}

static void test_getFamilyName ()
{
	GpStatus status;
	GpFontFamily *family;
	WCHAR name[LF_FACESIZE];

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipGetFamilyName (family, name, 57920);
	assertEqualInt (status, Ok);
	assert (stringsEqual(name, "Tahoma") || stringsEqual (name, "DejaVu Sans"));

	// Negative tests.
	status = GdipGetFamilyName (NULL, name, 1);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetFamilyName (family, NULL, 1);
	assertEqualInt (status, Ok);

	status = GdipGetFamilyName (NULL, NULL, 1);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_isStyleAvailable ()
{
	GpStatus status;
	GpFontFamily *family;
	BOOL isStyleAvailable;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipIsStyleAvailable (family, FontStyleBold, &isStyleAvailable);
	assertEqualInt (status, Ok);
	assert (isStyleAvailable);

	status = GdipIsStyleAvailable (family, -1, &isStyleAvailable);
	assertEqualInt (status, Ok);
	assert (isStyleAvailable);

	status = GdipIsStyleAvailable (family, FontStyleStrikeout + 1, &isStyleAvailable);
	assertEqualInt (status, Ok);
	assert (isStyleAvailable);

	// Negative tests.
	status = GdipIsStyleAvailable (NULL, FontStyleBold, &isStyleAvailable);
	assertEqualInt (status, InvalidParameter);

	status = GdipIsStyleAvailable (family, FontStyleBold, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_getEmHeight ()
{
	GpStatus status;
	GpFontFamily *family;
	UINT16 emHeight;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetEmHeight (NULL, FontStyleBold, &emHeight);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetEmHeight (family, FontStyleBold, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_getCellAscent ()
{
	GpStatus status;
	GpFontFamily *family;
	UINT16 cellAscent;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetCellAscent (NULL, FontStyleBold, &cellAscent);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCellAscent (family, FontStyleBold, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_getCellDescent ()
{
	GpStatus status;
	GpFontFamily *family;
	UINT16 cellDescent;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetCellDescent (NULL, FontStyleBold, &cellDescent);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetCellDescent (family, FontStyleBold, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_getLineSpacing ()
{
	GpStatus status;
	GpFontFamily *family;
	UINT16 lineSpacing;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	// Negative tests.
	status = GdipGetLineSpacing (NULL, FontStyleBold, &lineSpacing);
	assertEqualInt (status, InvalidParameter);

	status = GdipGetLineSpacing (family, FontStyleBold, NULL);
	assertEqualInt (status, InvalidParameter);

	GdipDeleteFontFamily (family);
}

int
main(int argc, char**argv)
{
	STARTUP;

	test_newPrivateFontCollection ();
	test_deletePrivateFontCollection ();
	test_getFontCollectionFamilyCount ();
	test_getFontCollectionFamilyList ();
	test_privateAddMemoryFont ();
	test_privateAddFontFile ();
	test_newInstalledFontCollection ();
	test_createFontFromDC ();
	test_createFontFromLogfontA ();
	test_createFontFromLogfontW ();
	test_createFont ();
	test_cloneFont ();
	test_deleteFont ();
	test_getFamily ();
	test_getFontStyle ();
	test_getFontSize ();
	test_getFontUnit ();
	test_getFontHeight ();
	test_getFontHeightGivenDPI ();
	test_getLogfontA ();
	test_getLogfontW ();
	test_createFontFamilyFromName ();
	test_cloneFontFamily ();
	test_deleteFontFamily ();
	test_getGenericFontFamilySansSerif ();
	test_getGenericFontFamilySerif ();
	test_getGenericFontFamilyMonospace ();
	test_getFamilyName ();
	test_isStyleAvailable ();
	test_getEmHeight ();
	test_getCellAscent ();
	test_getCellDescent ();
	test_getLineSpacing ();

	SHUTDOWN;
	return 0;
}
