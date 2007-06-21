#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "GdiPlusFlat.h"

static int status_counter = 0;

#define CHECK_STATUS(x) do { if (status != Ok) { printf ("status[%d] == %d!\n", status_counter++, status); if(x) { exit(-1); } } else { printf ("status[%d] == Ok\n", status_counter++); } } while (0)

int
main (int argc, char **argv)
{
    GpBitmap *bitmap;
    BitmapData d, q;
    Rect r;
    int i, j;
    unsigned long *lptr;
    unsigned char *cptr;
    BYTE *scan0 = (BYTE*) GdipAlloc(10 * 10 * 4);
    GpStatus status = GdipCreateBitmapFromScan0 (10, 10, 10 * 4, PixelFormat32bppARGB, scan0, &bitmap);
    CHECK_STATUS(1);

    printf ("Full rectangle, no format conversion, read only\n");
    r.X = 0; r.Y = 0;
    r.Width = 10; r.Height = 10;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &d);
    CHECK_STATUS(1);

    printf ("Attempt to re-lock (should fail)\n");
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &q);
    CHECK_STATUS(0);

    printf ("Unlock\n");
    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);

/*    lptr = (unsigned long *) bitmap->active_bitmap->scan0;
    for (j = 0; j < 10; j++) {
        for (i = 0; i < 10; i++) {
            *lptr++ = j | j << 8 | i << 16 | i << 24;
        }
    }
*/
    memset (&d, 0x00, sizeof (BitmapData));
    printf ("\nHalf rectangle, no format conversion, read only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &d);
    CHECK_STATUS(1);

    for (j = 0; j < 5; j++) {
        lptr = (unsigned long *) d.Scan0 + j * d.Stride;
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%08lx ", *lptr++);
        }
        printf ("\n");
    }
    printf ("Modifying (setting to 0xff)\n");
    memset (d.Scan0, 0xff, d.Stride * d.Height);
    printf ("Unlocking\n");
    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);
/*    printf ("Original data after unlock (shouldn't be 0xffffffff): 0x%08x\n",
            ((unsigned long *)(bitmap->active_bitmap->scan0))[55]);
    if (((unsigned long *)(bitmap->active_bitmap->scan0))[55] == 0xffffffff)
        printf ("==> FAIL!\n");
*/
    memset (&d, 0x00, sizeof (BitmapData));
    printf ("\nHalf rectangle, 32bpp ARGB -> 32bpp RGB, read only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppRGB, &d);
    CHECK_STATUS(1);

    lptr = (unsigned long *) d.Scan0;
    for (j = 0; j < 5; j++) {
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%08lx ", *lptr++);
        }
        printf ("\n");
    }

    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);

    memset (&d, 0x00, sizeof (BitmapData));
    printf ("\nHalf rectangle, 32bpp ARGB -> 24bpp RGB, read/write only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead | ImageLockModeWrite, PixelFormat24bppRGB, &d);
    CHECK_STATUS(1);

    for (j = 0; j < 5; j++) {
        cptr = (unsigned char *) (d.Scan0 + (j * d.Stride));
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%02x%02x%02x ", cptr[0], cptr[1], cptr[2]);
            cptr += 3;
        }
        printf ("\n");
    }

    printf ("Modifying (setting to 0xaabbcc)\n");
    for (j = 0; j < 5; j++) {
        cptr = (unsigned char *) (d.Scan0 + (j * d.Stride));
        for (i = 0; i < 5; i++) {
            *cptr++ = 0xcc;
            *cptr++ = 0xbb;
            *cptr++ = 0xaa;
        }
    }

    status = GdipBitmapUnlockBits (bitmap, &d);
    CHECK_STATUS(1);

/*    printf ("Original data after Unlock (should be all 0xffaabbcc):\n");
    for (j = 5; j < 10; j++) {
        lptr = (unsigned long *) (bitmap->active_bitmap->Scan0 + j * bitmap->active_bitmap->Stride) + 5;
        printf ("%d: ", j);
        for (i = 5; i < 10; i++) {
            printf ("%08x ", *lptr++);
        }
        printf ("\n");
    }
*/
    GdipFree (scan0);
    return 0;
}
