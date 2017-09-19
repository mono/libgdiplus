#include <assert.h>
#include <float.h>
#include <math.h>

static int floatsEqual(float v1, float v2)
{
    return fabs (v1 - v2) < 0.0001;
}

static void verifyMatrix (GpMatrix *matrix, REAL e1, REAL e2, REAL e3, REAL e4, REAL e5, REAL e6)
{
    float elements[6];
    GdipGetMatrixElements (matrix, elements);

    printf ("Actual:   %f, %f, %f, %f, %f, %f\n", e1, e2, e3, e4, e5, e6);
    printf ("Expected: %f, %f, %f, %f, %f, %f\n\n", elements[0], elements[1], elements[2], elements[3], elements[4], elements[5]);

    assert (floatsEqual(elements[0], e1));
    assert (floatsEqual(elements[1], e2));
    assert (floatsEqual(elements[2], e3));
    assert (floatsEqual(elements[3], e4));
    assert (floatsEqual(elements[4], e5));
    assert (floatsEqual(elements[5], e6));
}
