#include <assert.h>
#include <float.h>
#include <math.h>

int floatsEqual(float v1, float v2)
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

        printf ("Expected matrices to be equal\n");
        printf ("Actual:   %f, %f, %f, %f, %f, %f\n", e1, e2, e3, e4, e5, e6);
        printf ("Expected: %f, %f, %f, %f, %f, %f\n\n", elements[0], elements[1], elements[2], elements[3], elements[4], elements[5]);

        assert (FALSE);
    }
}

#if !defined(USE_WINDOWS_LIBGDIPLUS)
#define createWchar(c) g_utf8_to_utf16 (c, -1, NULL, NULL, NULL);
#define freeWchar(c) g_free(c)
#else
#define createWchar(c) L ##c
#define freeWchar(c)
#endif
