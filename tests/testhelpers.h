#include <assert.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(_WIN32)
#include <unistd.h>
#endif

#define STARTUP \
    ULONG_PTR gdiplusToken; \
    GdiplusStartupInput gdiplusStartupInput; \
    gdiplusStartupInput.GdiplusVersion = 1; \
    gdiplusStartupInput.DebugEventCallback = NULL; \
    gdiplusStartupInput.SuppressBackgroundThread = FALSE; \
    gdiplusStartupInput.SuppressExternalCodecs = FALSE; \
    GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL); \

#define SHUTDOWN GdiplusShutdown (gdiplusToken);

#if !defined(__has_attribute)
#define __has_attribute(x) 0
#endif

#if __has_attribute(used) || defined(__GNUC__)
#define ATTRIBUTE_USED __attribute__((used))
#else
#define ATTRIBUTE_USED
#endif

#define ARRAY_SIZE(x) sizeof (x) / sizeof (*x)

ATTRIBUTE_USED static BOOL floatsEqual(float v1, float v2)
{
    if (isnan (v1))
        return isnan (v2);

    if (isinf (v1))
        return isinf (v2);

    return fabs (v1 - v2) < 0.0001;
}

#if !defined(USE_WINDOWS_GDIPLUS)
#define createWchar(c) g_utf8_to_utf16 (c, -1, NULL, NULL, NULL)
#define freeWchar(c) g_free(c)
#define wcharFromChar(c) createWchar(c)
#define charFromWchar(c) g_utf16_to_utf8 (c, -1, NULL, NULL, NULL)
#define freeChar(c) g_free(c)
#else
ATTRIBUTE_USED static WCHAR* wcharFromChar(const char *c)
{
    size_t length = strlen (c);

    WCHAR *wc = (WCHAR *)malloc ((length + 1) * sizeof(WCHAR));
    for (size_t i = 0; i < length; i++) {
        wc[i] = (WCHAR) c[i];
    }
    wc[length] = 0;

    return wc;
}

ATTRIBUTE_USED static char* charFromWchar(const wchar_t *wc)
{
    size_t length = lstrlenW (wc);

    char *c = (char *)malloc ((length + 1) * sizeof(char));
    for (size_t i = 0; i < length; i++) {
        c[i] = (char) wc[i];
    }
    c[length] = 0;

    return c;
}

#define createWchar(c) (WCHAR *) L ##c
#define freeWchar(c)
#define freeChar(c) free(c);
#endif

ATTRIBUTE_USED static void printFailure(const char *file, const char *function, int line)
{
    fprintf (stderr, "Assertion failure: file %s in %s, line %d\n", file, function, line);
}

ATTRIBUTE_USED static void assertEqualIntImpl (INT actual, INT expected, const char *message, const char *file, const char *function, int line)
{
    if (actual != expected)
    {
        if (message)
            fprintf (stderr, "%s\n", message);

        printFailure (file, function, line);
        fprintf (stderr, "Expected: %d\n", expected);
        fprintf (stderr, "Actual:   %d\n", actual);
        abort();
    }
}

#define assertEqualInt(actual, expected) assertEqualIntImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertEqualFloatImpl (REAL actual, REAL expected, const char *message, const char *file, const char *function, int line)
{
    if (!floatsEqual (actual, expected))
    {
        if (message)
            fprintf (stderr, "%s\n", message);

        printFailure (file, function, line);
        fprintf (stderr, "Expected: %f\n", expected);
        fprintf (stderr, "Actual:   %f\n", actual);
        abort ();
    }
}

#define assertEqualFloat(actual, expected) assertEqualFloatImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertSimilarFloatImpl (REAL actual, REAL expected, REAL tolerance, const char *message, const char *file, const char *function, int line)
{
    if (fabs(actual - expected) > fabs(tolerance)) {
        if (message)
            fprintf (stderr, "%s\n", message);

        printFailure (file, function, line);
        fprintf (stderr, "Expected %f +/- %f\n", expected, tolerance);
        fprintf (stderr, "Actual:   %f\n", actual);
        abort ();
    }
}

#define assertSimilarFloat(actual, expected, tolerance) assertSimilarFloatImpl (actual, expected, tolerance, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static BOOL stringsEqual (const WCHAR *actual, const char *expected)
{
    int i = 0;
    while (TRUE) {
        if (expected[i] == '\0') {
            return actual[i] == '\0';
        }

        if (expected[i] != (char)actual[i])
            return FALSE;

        i++;
    }

    return TRUE;
}

ATTRIBUTE_USED static void assertEqualStringImpl(const WCHAR *actual, const char * expected, const char *message, const char *file, const char *function, int line)
{
    if (!stringsEqual (actual, expected))
    {
        char *actualA = charFromWchar (actual);

        if (message)
            fprintf (stderr, "%s\n", message);

        printFailure (file, function, line);
        fprintf (stderr, "Expected: %s\n", expected);
        fprintf (stderr, "Actual:   %s\n", actualA);

        freeChar (actualA);
        abort();
    }
}

#define assertEqualString(actual, expected) assertEqualStringImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertEqualRectImpl (GpRectF actual, GpRectF expected, const char *message, const char *file, const char *function, int line)
{
    if (!floatsEqual (actual.X, expected.X) || !floatsEqual (actual.Y, expected.Y) || !floatsEqual (actual.Width, expected.Width) || !floatsEqual (actual.Height, expected.Height))
    {
        if (message)
            fprintf (stderr, "%s\n", message);

        printFailure (file, function, line);
        fprintf (stderr, "Expected: {%g, %g, %g, %g}\n", expected.X, expected.Y, expected.Width, expected.Height);
        fprintf (stderr, "Actual:   {%g, %g, %g, %g}\n", actual.X, actual.Y, actual.Width, actual.Height);
        abort ();
    }
}

#define assertEqualRect(actual, expected) assertEqualRectImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertEqualPointImpl (Point actual, Point expected, const char *message, const char *file, const char *function, int line)
{
	if (actual.X != expected.X || actual.X != expected.X)
	{
		if (message)
			fprintf(stderr, "%s\n", message);

		printFailure(file, function, line);
		fprintf(stderr, "Expected: {%d, %d}\n", expected.X, expected.Y);
		fprintf(stderr, "Actual:   {%d, %d}\n", actual.X, actual.Y);
		abort();
	}
}

#define assertEqualPoint(actual, expected) assertEqualPointImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertEqualPointsImpl (const Point *actual, const Point *expected, INT count, const char *file, const char *function, int line)
{
	for (int i = 0; i < count; i++)
	{
		char iChar = i + '0';
		assertEqualPointImpl (actual[i], expected[i], &iChar, file, function, line);
	}
}

#define assertEqualPoints(actual, expected, count) assertEqualPointsImpl (actual, expected, count, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertEqualPointFImpl (PointF actual, PointF expected, const char *message, const char *file, const char *function, int line)
{
	if (!floatsEqual(actual.X, expected.X) || !floatsEqual(actual.Y, expected.Y))
	{
		if (message)
			fprintf(stderr, "%s\n", message);

		printFailure(file, function, line);
		fprintf(stderr, "Expected: {%g, %g}\n", expected.X, expected.Y);
		fprintf(stderr, "Actual:   {%g, %g}\n", actual.X, actual.Y);
		abort();
	}
}

#define assertEqualPointF(actual, expected) assertEqualPointFImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertEqualPointsFImpl (const PointF *actual, const PointF *expected, INT count, const char *file, const char *function, int line)
{
	for (int i = 0; i < count; i++)
	{
		char iChar = i + '0';
		assertEqualPointFImpl (actual[i], expected[i], &iChar, file, function, line);
	}
}

#define assertEqualPointsF(actual, expected, count) assertEqualPointsFImpl (actual, expected, count, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void dumpBytes (const BYTE *bytes, int length)
{
    printf ("%u\n", length);
    for (int i = 0; i < length; i++) {
        printf ("0x%02X", bytes[i]);
        if (i != length - 1) {
            printf (", ");
        }
    }

    printf("\n\n");
}

ATTRIBUTE_USED static void assertEqualBytesImpl (const BYTE *actual, const BYTE *expected, int length, const char *message, const char *file, const char *function, int line)
{
    for (int i = 0; i < length; i++)
    {
        if (actual[i] != expected[i])
        {
            if (message)
                fprintf (stderr, "%s\n", message);

            printFailure (file, function, line);
            fprintf (stderr, "Expected[%d]: 0x%02X\n", i, (BYTE) expected[i]);
            fprintf (stderr, "Actual[%d]:   0x%02X\n", i, (BYTE) actual[i]);
            fprintf (stderr, "-- Actual --\n");
            dumpBytes (actual, length);

            abort();
        }
    }
}

#define assertEqualBytes(actual, expected, length) assertEqualBytesImpl (actual, expected, length, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void assertEqualGuidImpl (GUID actual, GUID expected, const char *message, const char *file, const char *function, int line)
{
    if (memcmp ((void *) &actual, (void *) &expected, sizeof (GUID)) != 0)
    {
        if (message)
            fprintf (stderr, "%s\n", message);

        printFailure (file, function, line);
        fprintf (stderr, "Expected: {%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n", (unsigned long) expected.Data1, expected.Data2, expected.Data3, expected.Data4[0], expected.Data4[1], expected.Data4[2], expected.Data4[3], expected.Data4[4], expected.Data4[5], expected.Data4[6], expected.Data4[7]);
        fprintf (stderr, "Actual:   {%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}\n", (unsigned long) actual.Data1, actual.Data2, actual.Data3, actual.Data4[0], actual.Data4[1], actual.Data4[2], actual.Data4[3], actual.Data4[4], actual.Data4[5], actual.Data4[6], actual.Data4[7]);
        abort();
    }
}

#define assertEqualGuid(actual, expected) assertEqualGuidImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void verifyMatrixImpl(GpMatrix *matrix, REAL e1, REAL e2, REAL e3, REAL e4, REAL e5, REAL e6, const char *file, const char *function, int line)
{
    GpStatus status;
    REAL elements[6];

    status = GdipGetMatrixElements(matrix, elements);
    assertEqualInt (status, Ok);

    if (!floatsEqual (elements[0], e1) ||
        !floatsEqual (elements[1], e2) ||
        !floatsEqual (elements[2], e3) ||
        !floatsEqual (elements[3], e4) ||
        !floatsEqual (elements[4], e5) ||
        !floatsEqual (elements[5], e6)) {

        printFailure (file, function, line);
        fprintf (stderr, "Expected: %f, %f, %f, %f, %f, %f\n", e1, e2, e3, e4, e5, e6);
        fprintf (stderr, "Actual:   %f, %f, %f, %f, %f, %f\n\n", elements[0], elements[1], elements[2], elements[3], elements[4], elements[5]);

        abort ();
    }
}

#define verifyMatrix(matrix, e1, e2, e3, e4, e5, e6) verifyMatrixImpl (matrix, e1, e2, e3, e4, e5, e6, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void verifyRegionImpl(GpRegion *region, float expectedX, float expectedY, float expectedWidth, float expectedHeight, BOOL expectedIsEmpty, BOOL expectedIsInfinite, const char *file, const char *function, int line)
{
    GpImage *image;
    GpGraphics *graphics;
    GpStatus status;
    GpRectF bounds;
    BOOL isEmpty;
    BOOL isInfinite;
    RectF expectedBounds = {expectedX, expectedY, expectedWidth, expectedHeight};

	GdipCreateBitmapFromScan0 (10, 10, 0, PixelFormat32bppRGB, NULL, (GpBitmap **) &image);
	GdipGetImageGraphicsContext (image, &graphics);

    status = GdipGetRegionBounds (region, graphics, &bounds);
    assertEqualIntImpl (status, Ok, NULL, file, function, line);
    assertEqualRectImpl (bounds, expectedBounds, "Bounds", file, function, line);

    status = GdipIsEmptyRegion (region, graphics, &isEmpty);
    assertEqualIntImpl (status, Ok, NULL, file, function, line);
    assertEqualIntImpl (isEmpty, expectedIsEmpty, "IsEmpty", file, function, line);

    status = GdipIsInfiniteRegion (region, graphics, &isInfinite);
    assertEqualIntImpl (status, Ok, NULL, file, function, line);
    assertEqualIntImpl (isInfinite, expectedIsInfinite, "IsInfinite", file, function, line);

	GdipDisposeImage (image);
	GdipDeleteGraphics (graphics);
}

#define verifyRegion(region, expectedX, expectedY, expectedWidth, expectedHeight, expectedIsEmpty, expectedIsInfinite) \
	verifyRegionImpl (region, expectedX, expectedY, expectedWidth, expectedHeight, expectedIsEmpty, expectedIsInfinite, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static GpRectF infiniteScans[] = {{-4194304, -4194304, 8388608, 8388608}};

ATTRIBUTE_USED static GpRectF *emptyScans = NULL;

ATTRIBUTE_USED static void verifyRegionScansImpl (GpRegion *region, RectF *expectedScans, INT expectedCount, const char *file, const char *function, int line)
{
	GpStatus status;
	GpMatrix *matrix;
	INT count;
	GpRectF *scans;
	GdipCreateMatrix (&matrix);

	status = GdipGetRegionScans (region, NULL, &count, matrix);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	assertEqualIntImpl (count, expectedCount / sizeof (GpRectF), "ScansCount", file, function, line);
	scans = (GpRectF *) malloc (count * sizeof (GpRectF));

	status = GdipGetRegionScans (region, scans, &count, matrix);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
#if 0
	for (int i = 0; i < count; i++)
	{
		printf("{%.0f, %.0f, %.0f, %.0f}", scans[i].X, scans[i].Y, scans[i].Width, scans[i].Height);
		if (i != count - 1)
			printf (",");

		printf("\n");
	}
	printf("\n");
#endif
	for (int i = 0; i < count; i++)
	{
		char iChar[] = {'S', 'c', 'a', 'n', ' ', (char) (i + '0'), 0};
		assertEqualRectImpl (scans[i], expectedScans[i], iChar, file, function, line);
	}

	GdipDeleteMatrix (matrix);
}

#define verifyRegionScans(region, expectedScans, expectedCount) \
	verifyRegionScansImpl (region, expectedScans, expectedCount, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static BYTE infiniteRegionData[] = {
	/* --RegionHeader-- */
	/* Size */          0x0C, 0x00, 0x00, 0x00,
	/* Checksum */      0x9B, 0x34, 0x22, 0xA3,
	/* Magic */         0x02, 0x10, 0xC0, 0xDB,
	/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,

	/* -- Entry -- */
	/* Type */ 0x03, 0x00, 0x00, 0x10
};

ATTRIBUTE_USED static BYTE emptyRegionData[] = {
	/* --RegionHeader-- */
	/* Size */          0x0C, 0x00, 0x00, 0x00,
	/* Checksum */      0xFE, 0x53, 0x9E, 0x1B,
	/* Magic */         0x02, 0x10, 0xC0, 0xDB,
	/* Combining Ops */ 0x00, 0x00, 0x00, 0x00,
		
	/* -- Entry -- */
	/* Type */ 0x02, 0x00, 0x00, 0x10
};

ATTRIBUTE_USED static void verifyRegionDataImpl (GpRegion *region, BYTE *expected, INT expectedCount, const char *file, const char *function, int line)
{
	BYTE buffer[1024];

	GpStatus status;
	UINT dataSize;
	UINT sizeFilled;
	status = GdipGetRegionDataSize (region, &dataSize);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	assertEqualIntImpl (dataSize, expectedCount, "Region Data Count", file, function, line);

	status = GdipGetRegionData (region, buffer, dataSize, &sizeFilled);
	assertEqualIntImpl (status, Ok, NULL, file, function, line);
	assertEqualBytesImpl (buffer, expected, expectedCount, "Region Data", file, function, line);
	assertEqualIntImpl (sizeFilled, expectedCount, "Region Data Size Filled", file, function, line);
}

#define verifyRegionData(region, expectedData) \
	verifyRegionDataImpl (region, expectedData, sizeof (expectedData), __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void verifyEmptyRegionImpl (GpRegion *region, const char *file, const char *function, int line)
{
    verifyRegionImpl (region, 0, 0, 0, 0, TRUE, FALSE, file, function, line);
    verifyRegionScansImpl (region, emptyScans, sizeof (emptyScans), file, function, line);
	verifyRegionDataImpl (region, emptyRegionData, sizeof (emptyRegionData), file, function, line);
}

#define verifyEmptyRegion(region) \
	verifyEmptyRegionImpl (region, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static void verifyInfiniteRegionImpl (GpRegion *region, const char *file, const char *function, int line)
{
    verifyRegionImpl (region, -4194304.0f, -4194304.0f, 8388608.0f, 8388608.0f, FALSE, TRUE, file, function, line);
    verifyRegionScansImpl (region, infiniteScans, sizeof (infiniteScans), file, function, line);
	verifyRegionDataImpl (region, infiniteRegionData, sizeof (infiniteRegionData), file, function, line);
}

#define verifyInfiniteRegion(region) \
	verifyInfiniteRegionImpl (region, __FILE__, __func__, __LINE__)

ATTRIBUTE_USED static CLSID bmpEncoderClsid = { 0x557cf400, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static CLSID tifEncoderClsid = { 0x557cf405, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static CLSID gifEncoderClsid = { 0x557cf402, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static CLSID pngEncoderClsid = { 0x557cf406, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static CLSID jpegEncoderClsid = { 0x557cf401, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static CLSID icoEncoderClsid = { 0x557cf407, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static CLSID wmfEncoderClsid = { 0x557cf404, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static CLSID emfEncoderClsid = { 0x557cf403, 0x1a04, 0x11d3,{ 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };

ATTRIBUTE_USED static GUID memoryBmpRawFormat = { 0xb96b3caaU, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID bmpRawFormat = { 0xb96b3cabU, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID tifRawFormat = { 0xb96b3cb1U, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID gifRawFormat = { 0xb96b3cb0U, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID pngRawFormat = { 0xb96b3cafU, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID jpegRawFormat = { 0xb96b3caeU, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID icoRawFormat = { 0xb96b3cb5U, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID wmfRawFormat = { 0xb96b3cadU, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };
ATTRIBUTE_USED static GUID emfRawFormat = { 0xb96b3cacU, 0x0728U, 0x11d3U,{ 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e } };

ATTRIBUTE_USED static BOOL is_32bit()
{
    return sizeof(int *) == 4;
}

#if defined(USE_WINDOWS_GDIPLUS)
#define WINDOWS_GDIPLUS 1
#else
#define WINDOWS_GDIPLUS 0
#endif

#define verifyBitmap(image, expectedRawFormat, expectedPixelFormat, expectedWidth, expectedHeight, expectedFlags, expectedPropertyCount, checkFlags) \
    verifyImage((GpBitmap *) image, ImageTypeBitmap, expectedRawFormat, expectedPixelFormat, 0, 0, expectedWidth, expectedHeight, (REAL)expectedWidth, (REAL)expectedHeight, (REAL)expectedWidth, (REAL)expectedHeight, expectedFlags, expectedPropertyCount, checkFlags)

#define verifyMetafile(image, expectedRawFormat, expectedX, expectedY, expectedWidth, expectedHeight, expectedDimensionWidth, expectedDimensionHeight) \
    verifyImage(image, ImageTypeMetafile, expectedRawFormat, PixelFormat32bppRGB, expectedX, expectedY, expectedWidth, expectedHeight, (REAL)expectedWidth, (REAL)expectedHeight, expectedDimensionWidth, expectedDimensionHeight, 327683, 0, TRUE)

#define verifyImage(image, expectedType, expectedRawFormat, expectedPixelFormat, expectedX, expectedY, expectedWidth, expectedHeight, expectedBoundsWidth, expectedBoundsHeight, expectedDimensionWidth, expectedDimensionHeight, expectedFlags, expectedPropertyCount, checkFlags) \
{ \
    GpStatus status; \
    ImageType type; \
    GUID rawFormat; \
    PixelFormat pixelFormat; \
    UINT width; \
    UINT height; \
    GpRectF bounds; \
    GpUnit unit; \
    REAL dimensionWidth; \
    REAL dimensionHeight; \
    UINT flags; \
    UINT propertyCount; \
 \
    status = GdipGetImageType (image, &type); \
    assertEqualInt (status, Ok); \
    assertEqualInt (type, expectedType); \
 \
    status = GdipGetImageRawFormat (image, &rawFormat); \
    assertEqualInt (status, Ok); \
    assertEqualGuid (rawFormat, expectedRawFormat); \
 \
    status = GdipGetImagePixelFormat (image, &pixelFormat); \
    assertEqualInt (status, Ok); \
    assertEqualInt (pixelFormat, expectedPixelFormat); \
 \
    status = GdipGetImageWidth (image, &width); \
    assertEqualInt (status, Ok); \
    assertEqualInt (width, expectedWidth); \
 \
    status = GdipGetImageHeight (image, &height); \
    assertEqualInt (status, Ok); \
    assertEqualInt (height, expectedHeight); \
 \
    status = GdipGetImageBounds (image, &bounds, &unit); \
    assertEqualInt (status, Ok); \
    assertEqualFloat (bounds.X, expectedX); \
    assertEqualFloat (bounds.Y, expectedY); \
    assertEqualFloat (bounds.Width, expectedBoundsWidth); \
    assertEqualFloat (bounds.Height, expectedBoundsHeight); \
    assertEqualInt (unit, UnitPixel); \
 \
    /* Libgdiplus and GDI+ have different exact degrees of accuracy. */ \
    /* Typically they differ by +-0.02. */ \
    /* This is an acceptable difference. */ \
    status = GdipGetImageDimension (image, &dimensionWidth, &dimensionHeight); \
    assertEqualInt (status, Ok); \
    if (fabsf (dimensionWidth - expectedDimensionWidth) > 0.05) \
        assertEqualFloat (dimensionWidth, expectedDimensionWidth); \
    if (fabsf (dimensionHeight - expectedDimensionHeight) > 0.05) \
        assertEqualFloat (dimensionHeight, expectedDimensionHeight); \
 \
    /* FIXME: libgdiplus and GDI+ have different results for bitmap images. */ \
    if (checkFlags || WINDOWS_GDIPLUS) \
    { \
        status = GdipGetImageFlags (image, &flags); \
        assertEqualInt (status, Ok); \
        assertEqualInt (flags, (expectedFlags)); \
    } \
 \
    status = GdipGetPropertyCount (image, &propertyCount); \
    assertEqualInt (status, Ok); \
    /* FIXME: libgdiplus returns 0 for each image. */ \
    if (WINDOWS_GDIPLUS) \
    { \
        assertEqualInt (propertyCount, expectedPropertyCount); \
    } \
}


ATTRIBUTE_USED static void assertEqualARGBImpl (ARGB actual, ARGB expected, const char *message, const char *file, const char *function, int line)
{
    if (actual != expected)
    {
        if (message)
            fprintf (stderr, "%s\n", message);

        printFailure (file, function, line);
        fprintf (stderr, "Expected: 0x%08X\n", expected);
        fprintf (stderr, "Actual:   0x%08X\n", actual);
        abort();
    }
}

#define assertEqualARGB(actual, expected) assertEqualARGBImpl (actual, expected, NULL, __FILE__, __func__, __LINE__)

#define verifyPixels(image, expectedPixels) \
{ \
    GpStatus status; \
    UINT width; \
    UINT height; \
    GdipGetImageWidth ((GpBitmap *) image, &width); \
    GdipGetImageHeight ((GpBitmap *) image, &height); \
 \
    for (UINT y = 0; y < height; y++) \
    { \
        for (UINT x = 0; x < width; x++) \
        { \
            ARGB expected = expectedPixels[x + y * width]; \
            ARGB actual; \
            status = GdipBitmapGetPixel ((GpBitmap *) image, x, y, &actual); \
            assertEqualInt (status, Ok); \
            if (actual != expected) \
            { \
                fprintf (stderr, "Pixel [%u, %u]\n", x, y); \
                fprintf (stderr, "Expected: 0x%08X\n", expected); \
                fprintf (stderr, "Actual:   0x%08X\n", actual);   \
                dumpPixels (image); \
                assert (expected == actual); \
            } \
        } \
    } \
}

#define verifyPalette(image, flags, entries) \
{ \
    GpStatus status; \
    INT size; \
    ColorPalette *palette; \
 \
    status = GdipGetImagePaletteSize (image, &size); \
    assertEqualInt (status, Ok); \
 \
    palette = (ColorPalette *) GdipAlloc (size); \
    status = GdipGetImagePalette (image, palette, size); \
    assertEqualInt (status, Ok); \
 \
    assertEqualInt (palette->Flags, flags); \
    assertEqualInt (palette->Count, (int) (sizeof (entries) / sizeof (ARGB))); \
 \
    for (UINT i = 0; i < palette->Count; i++) \
    { \
        if (palette->Entries[i] != entries[i]) \
        { \
            fprintf (stderr, "Index [%u]\n", i); \
            fprintf (stderr, "Expected: 0x%08X\n", entries[i]); \
            fprintf (stderr, "Actual:   0x%08X\n", palette->Entries[i]);   \
            dumpPalette (palette); \
            assert (entries[i] == palette->Entries[i]); \
        } \
    } \
 \
    GdipFree (palette); \
}

#define verifyNoPalette(image, flags) \
{ \
    GpStatus status; \
    INT size; \
    ColorPalette *palette; \
 \
    status = GdipGetImagePaletteSize (image, &size); \
    assertEqualInt (status, Ok); \
 \
    palette = (ColorPalette *) GdipAlloc (size); \
    memset (palette, 0, sizeof (ColorPalette)); \
    palette->Entries[0] = 0x123; \
    status = GdipGetImagePalette (image, palette, size); \
    assertEqualInt (status, Ok); \
    assertEqualInt (size, sizeof (ColorPalette)); \
 \
    assertEqualInt (palette->Flags, flags); \
    assertEqualInt (palette->Count, 0); \
    assertEqualInt (palette->Entries[0], 0x123); \
 \
    GdipFree (palette); \
}

#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+ ((x&0x000000F0LU)?2:0)               \
+ ((x&0x00000F00LU)?4:0)               \
+ ((x&0x0000F000LU)?8:0)               \
+ ((x&0x000F0000LU)?16:0)              \
+ ((x&0x00F00000LU)?32:0)              \
+ ((x&0x0F000000LU)?64:0)              \
+ ((x&0xF0000000LU)?128:0)

#define B8(d) ((BYTE)B8__(HEX__(d)))

// A utility for dumping pixel arrays to the console for debugging purposes.
ATTRIBUTE_USED static void dumpPixels (GpImage *image)
{
    UINT width;
    UINT height;
    GdipGetImageWidth (image, &width);
    GdipGetImageHeight (image, &height);

    for (UINT y = 0; y < height; y++)
    {
        for (UINT x = 0; x < width; x++)
        {
            ARGB pixel;
            GdipBitmapGetPixel ((GpBitmap *) image, x, y, &pixel);
            printf ("0x%08X", pixel);
            if (x != width - 1)
            {
                printf(", ");
            }
            else if (y != height - 1)
            {
                printf(",");
            }
        }

        printf("\n");
    }
}

ATTRIBUTE_USED static void deleteFile(const char *file)
{
#if !defined(_WIN32)
    unlink (file);
#else
    remove (file);
#endif
}

ATTRIBUTE_USED static void dumpPalette (ColorPalette *palette)
{
    for (UINT i = 0; i < palette->Count; i++)
    {
        printf ("0x%08X", palette->Entries[i]);
        if (i != palette->Count - 1)
        {
            printf(", ");
        }
    }

    printf("\n");
}

// A utility for dumping an image and displaying it for debugging purposes.
ATTRIBUTE_USED static void dumpImage (GpImage *image)
{
	const WCHAR *wImage = createWchar("test-image.png");
	GpStatus status = GdipSaveImageToFile (image, wImage, &pngEncoderClsid, NULL);
	assertEqualInt (status, Ok);
	int systemResult = system ("test-image.png");
	(void) systemResult;
}
