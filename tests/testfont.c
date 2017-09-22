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
#include <wchar.h>

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

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
	FILE *f = fopen (fileName, "rb");
	assert (f && "Expected file to exist.");

	fseek (f, 0, SEEK_END);
	length = ftell (f);

	fseek (f, 0, SEEK_SET);
	buffer = malloc (length);
	assert (buffer && "Expected successful allocation of buffer.");

	fread (buffer, 1, length, f);
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

	GdiplusShutdown(gdiplusToken);
	return 0;
}
