#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(USE_WINDOWS_LIBGDIPLUS)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus")
#else
#include <GdiPlusFlat.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

static HDC getEmptyHDC ()
{
#if defined(USE_WINDOWS_LIBGDIPLUS)
	return CreateCompatibleDC (NULL);
#else
	return (HDC)1;
#endif
}

static void assertStringEqual (WCHAR *actual, const char *expected)
{
	int i = 0;
	while (TRUE) {
		if (expected[i] == '\0') {
			assert (actual[i] == '\0');
			break;
		}

		assert (expected[i] == (char)actual[i]);
		i++;
	}
}

static void *readFile (const char *fileName, int *memoryLength)
{
	void *buffer = NULL;
	long length;
	long read_length;

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

static void test_newPrivateFontCollection ()
{
	GpStatus status;
	GpFontCollection *collection;
	INT count;

	status = GdipNewPrivateFontCollection (&collection);
	assert (status == Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assert (status == Ok);
	assert (count == 0);

	// Negative tests.
	status = GdipNewPrivateFontCollection (NULL);
	assert (status == InvalidParameter);

	GdipDeletePrivateFontCollection (&collection);
}

static void test_deletePrivateFontCollection ()
{
	GpStatus status;
	GpFontCollection *collection;

	GdipNewPrivateFontCollection(&collection);

	status = GdipDeletePrivateFontCollection (&collection);
	assert (status == Ok);
	assert (!collection && "Expected the pointer to be set to NULL.");

	// Negative tests.
	status = GdipDeletePrivateFontCollection (NULL);
	assert (status == InvalidParameter);
}

static void test_getFontCollectionFamilyCount ()
{
	GpStatus status;
	GpFontCollection *collection;
	INT count;

	GdipNewPrivateFontCollection(&collection);

	// Negative tests.
	status = GdipGetFontCollectionFamilyCount (NULL, &count);
	assert (status == InvalidParameter);

	status = GdipGetFontCollectionFamilyCount (collection, NULL);
	assert (status == InvalidParameter);

	GdipDeletePrivateFontCollection (&collection);
}

static void test_getFontCollectionFamilyList ()
{
	GpStatus status;
	WCHAR *fontFile;
	GpFontCollection *collection;
	GpFontFamily *families[2] = {NULL, NULL};
	INT numFound;

	fontFile = g_utf8_to_utf16 ("test.ttf", -1, NULL, NULL, NULL);
	GdipNewPrivateFontCollection(&collection);

	//Empty list.
	status = GdipGetFontCollectionFamilyList (collection, 10, families, &numFound);
	assert (status == Ok);
	assert (numFound == 0);

	status = GdipGetFontCollectionFamilyList (collection, 0, families, &numFound);
	assert (status == Ok);
	assert (numFound == 0);

	status = GdipGetFontCollectionFamilyList (collection, -1, families, &numFound);
	assert (status == Ok);
	assert (numFound == 0);

	// Non empty list.
	GdipPrivateAddFontFile (collection, fontFile);
	status = GdipGetFontCollectionFamilyList (collection, 0, families, &numFound);
	assert (status == Ok);
	assert (numFound == 0);
	assert (families[0] == NULL);
	assert (families[1] == NULL);

	// Negative tests.
	status = GdipGetFontCollectionFamilyList (NULL, 10, families, &numFound);
	assert (status == InvalidParameter);

	status = GdipGetFontCollectionFamilyList (collection, 10, NULL, &numFound);
	assert (status == InvalidParameter);

	status = GdipGetFontCollectionFamilyList (collection, 10, families, NULL);
	assert (status == InvalidParameter);

	GdipDeletePrivateFontCollection (&collection);
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
	ttfFile = g_utf8_to_utf16 ("test.ttf", -1, NULL, NULL, NULL);
	otfFile = g_utf8_to_utf16 ("test.otf", -1, NULL, NULL, NULL);
	invalidFile = g_utf8_to_utf16 ("test.bmp", -1, NULL, NULL, NULL);
	noSuchFile = g_utf8_to_utf16 ("noSuchFile.ttf", -1, NULL, NULL, NULL);

	// Valid TTF file.
	status = GdipPrivateAddFontFile (collection, ttfFile);
	assert (status == Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assert (status == Ok);
	assert (count == 1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assert (status == Ok);
	assert (numFound == 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assert (status == Ok);
	assertStringEqual (name, "Code New Roman");

	GdipDeletePrivateFontCollection (&collection);

	// Valid OTF file.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddFontFile (collection, otfFile);
	assert (status == Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assert (status == Ok);
	assert (count == 1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assert (status == Ok);
	assert (numFound == 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assert (status == Ok);
	assertStringEqual (name, "Code New Roman");

	GdipDeletePrivateFontCollection (&collection);

	// Invalid file.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddFontFile (collection, invalidFile);
	assert (status == Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assert (status == Ok);
	assert (count == 0);

	// Negative tests.
	status = GdipPrivateAddFontFile (NULL, ttfFile);
	assert (status == InvalidParameter);

	status = GdipPrivateAddFontFile (collection, NULL);
	assert (status == InvalidParameter);

	status = GdipPrivateAddFontFile (collection, noSuchFile);
	assert (status == FileNotFound);

	GdipDeletePrivateFontCollection (&collection);
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
	assert (status == Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assert (status == Ok);
	assert (count == 1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assert (status == Ok);
	assert (numFound == 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assert (status == Ok);
	assertStringEqual (name, "Code New Roman");

	free (memory);
	GdipDeletePrivateFontCollection (&collection);

	// Valid OTF file.
	GdipNewPrivateFontCollection (&collection);
	memory = readFile ("test.otf", &memoryLength);

	status = GdipPrivateAddMemoryFont (collection, memory, memoryLength);
	assert (status == Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assert (status == Ok);
	assert (count == 1);

	status = GdipGetFontCollectionFamilyList (collection, 2, families, &numFound);
	assert (status == Ok);
	assert (numFound == 1);

	status = GdipGetFamilyName (families[0], name, 0);
	assert (status == Ok);
	assertStringEqual (name, "Code New Roman");

	free (memory);
	GdipDeletePrivateFontCollection (&collection);

	// Invalid memory is ignored.
	GdipNewPrivateFontCollection (&collection);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, 1);
	assert (status == Ok);

	status = GdipGetFontCollectionFamilyCount (collection, &count);
	assert (status == Ok);
	assert (count == 0);

	// Negative tests.
	status = GdipPrivateAddMemoryFont (NULL, &invalidMemory, 1);
	assert (status == InvalidParameter);

	status = GdipPrivateAddMemoryFont (collection, NULL, 1);
	assert (status == InvalidParameter);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, -1);
	assert (status == InvalidParameter);

	status = GdipPrivateAddMemoryFont (collection, &invalidMemory, 0);
	assert (status == InvalidParameter);

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
	assert (status == Ok);
	assert (collection1 && "Expected collection to be initialized.");

	status = GdipGetFontCollectionFamilyCount (collection1, &count1);
	assert (status == Ok);
	assert (count1 >= 0);

	status = GdipNewInstalledFontCollection (&collection2);
	assert (status == Ok);
	assert (collection1 == collection2 && "Expected the FontCollecion to be a singleton.");

	status = GdipGetFontCollectionFamilyCount (collection2, &count2);
	assert (status == Ok);
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
	assert (status == InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_LIBGDIPLUS)
	status = GdipCreateFontFromDC (hdc, NULL);
	assert (status == InvalidParameter);
#endif
}


static void test_createFontFromLogfontA ()
{
	GpStatus status;
	GpFont *font;
	LOGFONTA logfont;
	HDC hdc;
	INT style;
	Unit unit;
	GpFontFamily *family;

	hdc = getEmptyHDC ();

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
	strcpy (logfont.lfFaceName, "Times New Roman");

	status = GdipCreateFontFromLogfontA (hdc, &logfont, &font);
	assert (status == Ok);

	GdipGetFontStyle (font, &style);
	assert (style == 15);

	GdipGetFontUnit(font, &unit);
	assert (unit == UnitWorld);

	status = GdipGetFamily (font, &family);
	// FIXME: this fails with libgdiplus.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	assert (status == Ok);
	assert (family);
#endif

	// Negative tests.
	status = GdipCreateFontFromLogfontA (NULL, &logfont, &font);
	assert (status == InvalidParameter);

	status = GdipCreateFontFromLogfontA (hdc, NULL, &font);
	assert (status == InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_LIBGDIPLUS)
	status = GdipCreateFontFromLogfontA (hdc, &logfont, NULL);
	assert (status == InvalidParameter);
#endif

	GdipDeleteFont (font);
#if defined(USE_WINDOWS_LIBGDIPLUS)
	GdipDeleteFontFamily (family);
#endif	
}

static void test_createFontFromLogfontW ()
{
	GpStatus status;
	GpFont *font;
	LOGFONTW logfont;
	WCHAR *fontName;
	HDC hdc;
	INT style;
	Unit unit;
	GpFontFamily *family;

	fontName = g_utf8_to_utf16 ("Times New Roman", -1, NULL, NULL, NULL);
	hdc = getEmptyHDC ();

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
	memcpy ((void *) logfont.lfFaceName, (void *) fontName, sizeof(WCHAR) + 15 + 1);

	status = GdipCreateFontFromLogfontW (hdc, &logfont, &font);
	assert (status == Ok);

	GdipGetFontStyle (font, &style);
	assert (style == 15);

	GdipGetFontUnit(font, &unit);
	assert (unit == UnitWorld);

	status = GdipGetFamily (font, &family);
	// FIXME: this fails with libgdiplus.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	assert (status == Ok);
	assert (family);
#endif

	// Negative tests.
	status = GdipCreateFontFromLogfontW (NULL, &logfont, &font);
	assert (status == InvalidParameter);

	status = GdipCreateFontFromLogfontW (hdc, NULL, &font);
	assert (status == InvalidParameter);

	// This causes a null pointer dereference in GDI+.
#if !defined(USE_WINDOWS_LIBGDIPLUS)
	status = GdipCreateFontFromLogfontW (hdc, &logfont, NULL);
	assert (status == InvalidParameter);
#endif

	GdipDeleteFont (font);
#if defined(USE_WINDOWS_LIBGDIPLUS)
	GdipDeleteFontFamily (family);
#endif
}

static void test_createFont ()
{
	GpStatus status;
	GpFont *font;
	GpFontFamily *originalFamily;
	GpFontFamily *family;
	WCHAR originalFamilyName[32];
	WCHAR familyName[32];
	Unit unit;
	INT style;

	GdipGetGenericFontFamilySansSerif (&originalFamily);
	GdipCreateFont (originalFamily, 10, 10, UnitPixel, &font);

	status = GdipGetFamily (font, &family);
	assert (status == Ok);
	assert (family && family != originalFamily);

	GdipGetFamilyName (originalFamily, originalFamilyName, 0);
	GdipGetFamilyName (family, familyName, 0);
	assert (!strcmp ((char *) originalFamilyName, (char *) familyName));

	GdipGetFontUnit (font, &unit);
	assert (unit == UnitPixel);

	GdipGetFontStyle(font, &style);
	assert (style == 10);

	// Negative tests.
	status = GdipCreateFont (NULL, 10, 10, UnitPixel, &font);
	assert (status == InvalidParameter);

	status = GdipCreateFont (family, 10, 10, UnitPixel, NULL);
	assert (status == InvalidParameter);

	// FIXME: there are several places that indirectly call GdipCreateFont in
	// libgdiplus but allow negative em sizes on GDI+ because the GDI+
	// implementation does not call GdipCreateFont. An example is GdipAddPathString.
	// A fix for this will need to carefully account for places where this
	// API is called.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	status = GdipCreateFont (family, -1, 10, UnitPixel, &font);
	assert (status == InvalidParameter);

	status = GdipCreateFont (family, 0, 10, UnitPixel, &font);
	assert (status == InvalidParameter);
#endif

	status = GdipCreateFont (family, 10, 10, UnitDisplay, &font);
	assert (status == InvalidParameter);

	status = GdipCreateFont (family, 10, 10, (Unit)(UnitWorld - 1), &font);
	assert (status == InvalidParameter);

#if defined(USE_WINDOWS_LIBGDIPLUS)
	status = GdipCreateFont (family, 10, 10, (Unit)(UnitMillimeter + 1), &font);
	assert (status == InvalidParameter);
#else
	status = GdipCreateFont (family, 10, 10, (Unit)(UnitCairoPoint + 1), &font);
	assert (status == InvalidParameter);
#endif

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
	GdipDeleteFontFamily (family);
}

static void test_deleteFont ()
{
	GpStatus status;
	GpFontFamily *family;
	GpFont *font;

	GdipGetGenericFontFamilySansSerif (&family);

	GdipCreateFont (family, 10, 10, UnitPixel, &font);

	status = GdipDeleteFont (font);
	assert (status == Ok);

	// Negative tests.
	status = GdipDeleteFont (NULL);
	assert (status == InvalidParameter);

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
	assert (status == InvalidParameter);

	status = GdipGetFamily (font, NULL);
	assert (status == InvalidParameter);

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
	assert (status == InvalidParameter);

	status = GdipGetFontStyle (font, NULL);
	assert (status == InvalidParameter);

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
	assert (status == Ok);

	// Negative tests.
	status = GdipGetFontSize (NULL, &size);
	assert (status == InvalidParameter);

	status = GdipGetFontSize (font, NULL);
	assert (status == InvalidParameter);

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
	assert (status == InvalidParameter);

	status = GdipGetFontUnit (font, NULL);
	assert (status == InvalidParameter);

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
	assert (status == Ok);
	// FIXME: this returns a different value with libgdiplus.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	assert (floatsEqual (height, 11.3183594));
#endif

	// Negative tests.
	status = GdipGetFontHeight (NULL, NULL, &height);
	assert (status == InvalidParameter);

	status = GdipGetFontHeight (font, NULL, NULL);
	assert (status == InvalidParameter);

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
	assert (status == Ok);
	// FIXME: this returns a different value with libgdiplus.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	assert (floatsEqual (height, 11.3183594));
#endif

	// Negative tests.
	status = GdipGetFontHeightGivenDPI (NULL, 10, &height);
	assert (status == InvalidParameter);

	status = GdipGetFontHeightGivenDPI (font, 10, NULL);
	assert (status == InvalidParameter);

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
	assert (status == Ok);
	assert (logfont.lfHeight == -10);
	assert (logfont.lfWidth == 0);
	assert (logfont.lfEscapement == 0);
	assert (logfont.lfOrientation == 0);
	assert (logfont.lfWeight == 400);
	assert (logfont.lfCharSet == 0);
	assert (logfont.lfOutPrecision == 0);
	assert (logfont.lfClipPrecision == 0);
	assert (logfont.lfQuality == 0);
	assert (logfont.lfPitchAndFamily == 0);
	assertStringEqual (originalFamilyName, logfont.lfFaceName);

	// Negative tests.
	status = GdipGetLogFontA (NULL, graphics, &logfont);
	assert (status == InvalidParameter);

	status = GdipGetLogFontA (font, NULL, &logfont);
	assert (status == InvalidParameter);

	status = GdipGetLogFontA (font, graphics, NULL);
	assert (status == InvalidParameter);

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
	assert (status == Ok);
	assert (logfont.lfHeight == -10);
	assert (logfont.lfWidth == 0);
	assert (logfont.lfEscapement == 0);
	assert (logfont.lfOrientation == 0);
	assert (logfont.lfWeight == 400);
	assert (logfont.lfCharSet == 0);
	assert (logfont.lfOutPrecision == 0);
	assert (logfont.lfClipPrecision == 0);
	assert (logfont.lfQuality == 0);
	assert (logfont.lfPitchAndFamily == 0);
	assert (!strcmp ((char *) originalFamilyName, (char *) logfont.lfFaceName));

	// Negative tests.
	status = GdipGetLogFontW (NULL, graphics, &logfont);
	assert (status == InvalidParameter);

	status = GdipGetLogFontW (font, NULL, &logfont);
	assert (status == InvalidParameter);

	status = GdipGetLogFontW (font, graphics, NULL);
	assert (status == InvalidParameter);

	GdipDeleteFont (font);
	GdipDeleteFontFamily (originalFamily);
	GdipDisposeImage ((GpImage *) image);
}

static const WCHAR Tahoma[] = {'T', 'a', 'h', 'o', 'm', 'a', 0};
static const WCHAR CodeNewRoman[] = {'C', 'o', 'd', 'e', ' ', 'N', 'e', 'w', ' ', 'R', 'o', 'm', 'a', 'n', 0};
static const WCHAR NoSuchFont[] = {'A', 'B', 'C', 0};

static void verifyFontFamily (GpFontFamily *family, const char *expectedName, UINT16 expectedCellAscent, UINT16 expectedCellDescent, UINT16 expectedLineSpacing)
{
	GpStatus status;
	WCHAR name[LF_FACESIZE];
	UINT16 emHeight;
	UINT16 cellAscent;
	UINT16 cellDescent;
	UINT16 lineSpacing;

	status = GdipGetFamilyName (family, name, 1);
	assert (status == Ok);
	assertStringEqual (name, expectedName);

	status = GdipGetEmHeight (family, FontStyleRegular, &emHeight);
	assert (status == Ok);
	assert (emHeight == 2048);

	status = GdipGetCellAscent (family, FontStyleRegular, &cellAscent);
	assert (status == Ok);
	assert (cellAscent == expectedCellAscent);

	status = GdipGetCellDescent (family, FontStyleRegular, &cellDescent);
	assert (status == Ok);
	assert (cellDescent == expectedCellDescent);

	status = GdipGetLineSpacing (family, FontStyleRegular, &lineSpacing);
	assert (status == Ok);
	assert (lineSpacing == expectedLineSpacing);

	GdipDeleteFontFamily (family);
}

static void test_createFontFamilyFromName ()
{
	GpStatus status;
	GpFontFamily *family;
	GpFontCollection *collection;
	WCHAR *fontName;

	GdipNewPrivateFontCollection (&collection);
	fontName = g_utf8_to_utf16 ("test.ttf", -1, NULL, NULL, NULL);
	GdipPrivateAddFontFile (collection, fontName);

	status = GdipCreateFontFamilyFromName (Tahoma, NULL, &family);
	assert (status == Ok);
	verifyFontFamily (family, "Tahoma", 2049, 423, 2472);

	// FIXME: it appears that GdipCreateFontFamilyFromName fails if
	// GdipGetFontCollectionCount is not called first.
	// This needs to be investigated.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	INT x;
	GdipGetFontCollectionFamilyCount (collection, &x);
#endif

	status = GdipCreateFontFamilyFromName (CodeNewRoman, collection, &family);
	assert (status == Ok);
	verifyFontFamily (family, "Code New Roman", 2059, 430, 2489);

	// Negative tests.
	status = GdipCreateFontFamilyFromName (NULL, collection, &family);
	assert (status == InvalidParameter);

	status = GdipCreateFontFamilyFromName (CodeNewRoman, collection, NULL);
	assert (status == InvalidParameter);

	// FIXME: Libgdiplus does not validate that the font family exists
	// if the collection is NULL.
#if defined(USE_WINDOWS_LIBGDIPLUS)
	status = GdipCreateFontFamilyFromName (NoSuchFont, NULL, &family);
	assert (status == FontFamilyNotFound);
#endif

	status = GdipCreateFontFamilyFromName (Tahoma, collection, &family);
	assert (status == FontFamilyNotFound);

	status = GdipCreateFontFamilyFromName (NoSuchFont, collection, &family);
	assert (status == FontFamilyNotFound);

	GdipDeletePrivateFontCollection (&collection);
}

static void test_cloneFontFamily ()
{
	GpStatus status;
	GpFontFamily *family;
	GpFontFamily *clonedFamily;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);
	
	status = GdipCloneFontFamily (family, &clonedFamily);
	assert (status == Ok);
	verifyFontFamily (clonedFamily, "Tahoma", 2049, 423, 2472);

	// Negative tests.
	status = GdipCloneFontFamily (NULL, &clonedFamily);
	assert (status == InvalidParameter);

	status = GdipCloneFontFamily (family, NULL);
	assert (status == InvalidParameter);
	
	GdipDeleteFontFamily (family);
}

static void test_deleteFontFamily ()
{
	GpStatus status;
	GpFontFamily *family;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);
	
	status = GdipDeleteFontFamily (family);
	assert (status == Ok);
	
	// Negative tests.
	status = GdipDeleteFontFamily (NULL);
	assert (status == InvalidParameter);
}

static void test_getGenericFontFamilySansSerif ()
{
	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilySansSerif  (&family1);
	assert (status == Ok);
	assert (family1);

	status = GdipGetGenericFontFamilySansSerif (&family2);
	assert (status == Ok);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilySansSerif (NULL);
	assert (status == InvalidParameter);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);
}

static void test_getGenericFontFamilySerif ()
{
	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilySerif  (&family1);
	assert (status == Ok);
	assert (family1);

	status = GdipGetGenericFontFamilySerif  (&family2);
	assert (status == Ok);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilySerif (NULL);
	assert (status == InvalidParameter);

	GdipDeleteFontFamily (family1);
	GdipDeleteFontFamily (family2);
}

static void test_getGenericFontFamilyMonospace ()
{
	GpStatus status;
	GpFontFamily *family1;
	GpFontFamily *family2;

	status = GdipGetGenericFontFamilyMonospace  (&family1);
	assert (status == Ok);
	assert (family1);

	status = GdipGetGenericFontFamilyMonospace  (&family2);
	assert (status == Ok);
	assert (family2 && family1 == family2);

	// Negative tests.
	status = GdipGetGenericFontFamilyMonospace (NULL);
	assert (status == InvalidParameter);

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
	assert (status == Ok);
	assertStringEqual(name, "Tahoma");

	// Negative tests.
	status = GdipGetFamilyName (NULL, name, 1);
	assert (status == InvalidParameter);

	status = GdipGetFamilyName (family, NULL, 1);
	assert (status == Ok);

	status = GdipGetFamilyName (NULL, NULL, 1);
	assert (status == InvalidParameter);

	GdipDeleteFontFamily (family);
}

static void test_isStyleAvailable ()
{
	GpStatus status;
	GpFontFamily *family;
	BOOL isStyleAvailable;

	GdipCreateFontFamilyFromName (Tahoma, NULL, &family);

	status = GdipIsStyleAvailable (family, FontStyleBold, &isStyleAvailable);
	assert (status == Ok);
	assert (isStyleAvailable == TRUE);

	status = GdipIsStyleAvailable (family, -1, &isStyleAvailable);
	assert (status == Ok);
	assert (isStyleAvailable == TRUE);

	status = GdipIsStyleAvailable (family, FontStyleStrikeout + 1, &isStyleAvailable);
	assert (status == Ok);
	assert (isStyleAvailable == TRUE);

	// Negative tests.
	status = GdipIsStyleAvailable (NULL, FontStyleBold, &isStyleAvailable);
	assert (status == InvalidParameter);

	status = GdipIsStyleAvailable (family, FontStyleBold, NULL);
	assert (status == InvalidParameter);

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
	assert (status == InvalidParameter);

	status = GdipGetEmHeight (family, FontStyleBold, NULL);
	assert (status == InvalidParameter);

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
	assert (status == InvalidParameter);

	status = GdipGetCellAscent (family, FontStyleBold, NULL);
	assert (status == InvalidParameter);

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
	assert (status == InvalidParameter);

	status = GdipGetCellDescent (family, FontStyleBold, NULL);
	assert (status == InvalidParameter);

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
	assert (status == InvalidParameter);

	status = GdipGetLineSpacing (family, FontStyleBold, NULL);
	assert (status == InvalidParameter);

	GdipDeleteFontFamily (family);
}

int
main(int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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

	GdiplusShutdown(gdiplusToken);
	return 0;
}
