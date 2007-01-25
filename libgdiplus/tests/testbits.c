#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gdip.h"

static int status_counter = 0;

#define CHECK_STATUS(x) do { if (status != Ok) { printf ("status[%d] == %d!\n", status_counter++, status); if(x) { exit(-1); } } else { printf ("status[%d] == Ok\n", status_counter++); } } while (0)

int
main (int argc, char **argv)
{
    GpBitmap *bitmap;
    GdipBitmapData d, q;
    GpStatus status;
    Rect r;
    int i, j;
    unsigned long *lptr;
    unsigned char *cptr;

    status = GdipCreateBitmapFromScan0 (10, 10, 10 * 4, Format32bppArgb, GdipAlloc(10 * 10 * 4), &bitmap);
    CHECK_STATUS(1);

    printf ("Full rectangle, no format conversion, read only\n");
    r.X = 0; r.Y = 0;
    r.Width = 10; r.Height = 10;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, Format32bppArgb, &d);
    CHECK_STATUS(1);

    printf ("Attempt to re-lock (should fail)\n");
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, Format32bppArgb, &q);
    CHECK_STATUS(0);

    printf ("Unlock\n");
    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);

    lptr = (unsigned long *) bitmap->active_bitmap->scan0;
    for (j = 0; j < 10; j++) {
        for (i = 0; i < 10; i++) {
            *lptr++ = j | j << 8 | i << 16 | i << 24;
        }
    }

    memset (&d, 0x00, sizeof(GdipBitmapData));
    printf ("\nHalf rectangle, no format conversion, read only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, Format32bppArgb, &d);
    CHECK_STATUS(1);

    for (j = 0; j < 5; j++) {
        lptr = (unsigned long *) d.scan0 + j * d.stride;
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%08x ", *lptr++);
        }
        printf ("\n");
    }
    printf ("Modifying (setting to 0xff)\n");
    memset (d.scan0, 0xff, d.stride * d.height);
    printf ("Unlocking\n");
    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);
    printf ("Original data after unlock (shouldn't be 0xffffffff): 0x%08x\n",
            ((unsigned long *)(bitmap->active_bitmap->scan0))[55]);
    if (((unsigned long *)(bitmap->active_bitmap->scan0))[55] == 0xffffffff)
        printf ("==> FAIL!\n");

    memset (&d, 0x00, sizeof(GdipBitmapData));
    printf ("\nHalf rectangle, 32bpp ARGB -> 32bpp RGB, read only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, Format32bppRgb, &d);
    CHECK_STATUS(1);

    lptr = (unsigned long *) d.scan0;
    for (j = 0; j < 5; j++) {
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%08x ", *lptr++);
        }
        printf ("\n");
    }

    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);

    memset (&d, 0x00, sizeof(GdipBitmapData));
    printf ("\nHalf rectangle, 32bpp ARGB -> 24bpp RGB, read/write only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead | ImageLockModeWrite, Format24bppRgb, &d);
    CHECK_STATUS(1);

    for (j = 0; j < 5; j++) {
        cptr = (unsigned char *) (d.scan0 + (j * d.stride));
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%02x%02x%02x ", cptr[0], cptr[1], cptr[2]);
            cptr += 3;
        }
        printf ("\n");
    }

    printf ("Modifying (setting to 0xaabbcc)\n");
    for (j = 0; j < 5; j++) {
        cptr = (unsigned char *) (d.scan0 + (j * d.stride));
        for (i = 0; i < 5; i++) {
            *cptr++ = 0xcc;
            *cptr++ = 0xbb;
            *cptr++ = 0xaa;
        }
    }

    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);

    printf ("Original data after Unlock (should be all 0xffaabbcc):\n");
    for (j = 5; j < 10; j++) {
        lptr = (unsigned long *) (bitmap->active_bitmap->scan0 + j * bitmap->active_bitmap->stride) + 5;
        printf ("%d: ", j);
        for (i = 5; i < 10; i++) {
            printf ("%08x ", *lptr++);
        }
        printf ("\n");
    }

    return 0;
}
