#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(_WIN32)
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
printf("%c%c%c%c%c%c%c%c%c%c%c%c\n", (char)actual[0], (char)actual[1], (char)actual[2], (char)actual[3], (char)actual[4], (char)actual[5], (char)actual[6], (char)actual[7], (char)actual[8], (char)actual[9], (char)actual[10], (char)actual[11]);
printf("%c%c%c%c%c%c%c%c%c%c%c%c\n", expected[0], expected[1], expected[2], expected[3], expected[4], expected[5], expected[6], expected[7], expected[8], expected[9], expected[10], expected[11]);
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
	WCHAR *fontFile = L"test.ttf";
	GpFontCollection *collection;
	GpFontFamily *families[2] = {NULL, NULL};
	INT numFound;

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
	WCHAR *ttfFile = L"test.ttf";
	WCHAR *otfFile = L"test.otf";
	WCHAR *invalidFile = L"test.bmp";
	WCHAR *noSuchFile = L"noSuchFile.ttf";
	INT count;
	GpFontFamily *families[1];
	INT numFound;
	WCHAR name[LF_FACESIZE];

	GdipNewPrivateFontCollection (&collection);

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
	WCHAR *fontName = L"Times New Roman";
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

	GdiplusShutdown(gdiplusToken);
	return 0;
}
