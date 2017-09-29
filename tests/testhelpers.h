#include <assert.h>
#include <float.h>
#include <math.h>

BOOL floatsEqual (float v1, float v2)
{
	if (isnan (v1))
		return isnan (v2);

	if (isinf (v1))
		return isinf (v2);

	return fabs (v1 - v2) < 0.0001;
}

void verifyMatrix (GpMatrix *matrix, REAL e1, REAL e2, REAL e3, REAL e4, REAL e5, REAL e6)
{
    float elements[6];
    GdipGetMatrixElements (matrix, elements);

    if (!floatsEqual (elements[0], e1) ||
        !floatsEqual (elements[1], e2) ||
        !floatsEqual (elements[2], e3) ||
        !floatsEqual (elements[3], e4) ||
        !floatsEqual (elements[4], e5) ||
        !floatsEqual (elements[5], e6)) {

        fprintf (stderr, "Expected matrices to be equal\n");
        fprintf (stderr, "Expected: %f, %f, %f, %f, %f, %f\n", e1, e2, e3, e4, e5, e6);
        fprintf (stderr, "Actual:   %f, %f, %f, %f, %f, %f\n\n", elements[0], elements[1], elements[2], elements[3], elements[4], elements[5]);

        abort ();
    }
}

#if !defined(USE_WINDOWS_LIBGDIPLUS)
#define createWchar(c) g_utf8_to_utf16 (c, -1, NULL, NULL, NULL);
#define freeWchar(c) g_free(c)
#define wcharFromChar(c) createWchar(c)
#else
static WCHAR* wcharFromChar(const char *c)
{
	size_t length = strlen (c);

	WCHAR *wc = (WCHAR *)malloc((length + 1) * sizeof(WCHAR *));
	swprintf (wc, length + 1, L"%hs", c);
	wc[length] = 0;

	return wc;
}

#define createWchar(c) L ##c
#define freeWchar(c)
#endif

#define assertEqualInt(actual, expected) assertEqualIntImpl (actual, expected, __FILE__, __LINE__)
void assertEqualIntImpl (INT actual, INT expected, const char *file, INT line)
{
    if (actual != expected)
    {
        fprintf (stderr, "Assertion failed on line %d in %s\n", line, file);
        fprintf (stderr, "Expected: %d\n", expected);
        fprintf (stderr, "Actual:   %d\n", actual);

        abort ();
    }
}

#define assertEqualFloat(actual, expected) assertEqualFloatImpl (actual, expected, __FILE__, __LINE__)
void assertEqualFloatImpl (REAL actual, REAL expected, const char *file, INT line)
{
    if (!floatsEqual (actual, expected))
    {
        fprintf (stderr, "Assertion failed on line %d in %s\n", line, file);
        fprintf (stderr, "Expected: %f\n", expected);
        fprintf (stderr, "Actual:   %f\n", actual);

        abort ();
    }
}

static CLSID bmpEncoderClsid = { 0x557cf400, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
static CLSID tifEncoderClsid = { 0x557cf405, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
static CLSID gifEncoderClsid = { 0x557cf402, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
static CLSID pngEncoderClsid = { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
static CLSID jpegEncoderClsid = { 0x557cf401, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
static CLSID icoEncoderClsid = { 0x557cf407, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
static CLSID wmfEncoderClsid = { 0x557cf404, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
static CLSID emfEncoderClsid = { 0x557cf403, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
