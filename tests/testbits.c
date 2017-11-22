#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "GdiPlusFlat.h"

static int status_counter = 0;

#define CHECK_STATUS(x) do { if (status != Ok) { printf ("status[%d] == %d!\n", status_counter++, status); if(x) { exit(-1); } } else { printf ("status[%d] == Ok\n", status_counter++); } } while (0)

/* FIXME:
 * libgdiplus version is larger than it should be. The library writes
 * to the extended fields, overwriting other data if the non-extended
 * version is used. This causes the failure of this test because other
 * parts of the stack are unexpectedly overwritten.
 */
typedef union {
    BitmapData origBitmapData;
    /* This structure is from src/bitmap-private.h */
    struct {
	unsigned int	width;
	unsigned int	height;
	int		stride;
	int		pixel_format;
	BYTE 		*scan0;
	unsigned int	reserved;
	/* the rest of the structure isn't part of MS GDI+ definition */
	ColorPalette	*palette;
	int		property_count;		/* Number of properties */
	PropertyItem 	*property;		/* Properties associated with image */

	float 		dpi_horz;		/* */
	float 		dpi_vert;		/* */
	ImageFlags	image_flags;		/* Alpha, ColorSpace, etc. */

	unsigned int	left;			/* left display coordinate of frame */
	unsigned int	top;			/* top display coordinate of frame */
	unsigned int	x;			/* LockBits: left coordinate of locked rectangle */
	unsigned int	y;			/* LockBits: top coordinate of locked rectangle */

	int		transparent;		/* Index of transparent color (<24bit only) */
    } extendedBitmapData;
} IncompatibleBitmapData;

int
main (int argc, char **argv)
{
    GpBitmap *bitmap;
    IncompatibleBitmapData d, q;
    Rect r;
    int i, j;
    unsigned char *cptr;
    BYTE *scan0 = (BYTE*) GdipAlloc(10 * 10 * 4);
    GpStatus status = GdipCreateBitmapFromScan0 (10, 10, 10 * 4, PixelFormat32bppARGB, scan0, &bitmap);
    CHECK_STATUS(1);

    printf ("Full rectangle, no format conversion, read only\n");
    r.X = 0; r.Y = 0;
    r.Width = 10; r.Height = 10;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &d.origBitmapData);
    CHECK_STATUS(1);

    printf ("Attempt to re-lock (should fail)\n");
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &q.origBitmapData);
    CHECK_STATUS(0);

    printf ("Unlock\n");
    status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
    CHECK_STATUS(1);

    memset (&d, 0x00, sizeof (BitmapData));
    printf ("\nHalf rectangle, no format conversion, read only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppARGB, &d.origBitmapData);
    CHECK_STATUS(1);

    for (j = 0; j < 5; j++) {
        cptr = (unsigned char *) d.origBitmapData.Scan0 + j * d.origBitmapData.Stride;
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%04x ", *cptr++);
        }
        printf ("\n");
    }
    printf ("Modifying (setting to 0xff)\n");
    memset (d.origBitmapData.Scan0, 0xff, d.origBitmapData.Stride * d.origBitmapData.Height);
    printf ("Unlocking\n");
    status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
    CHECK_STATUS(1);

    memset (&d, 0x00, sizeof (BitmapData));
    printf ("\nHalf rectangle, 32bpp ARGB -> 32bpp RGB, read only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead, PixelFormat32bppRGB, &d.origBitmapData);
    CHECK_STATUS(1);

    for (j = 0; j < 5; j++) {
        cptr = (unsigned char *) d.origBitmapData.Scan0 + j * d.origBitmapData.Stride;
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%04x ", *cptr++);
        }
        printf ("\n");
    }

    status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
    CHECK_STATUS(1);

    memset (&d, 0x00, sizeof (BitmapData));
    printf ("\nHalf rectangle, 32bpp ARGB -> 24bpp RGB, read/write only\n");
    r.X = 5; r.Y = 5;
    r.Width = 5; r.Height = 5;
    status = GdipBitmapLockBits (bitmap, &r, ImageLockModeRead | ImageLockModeWrite, PixelFormat24bppRGB, &d.origBitmapData);
    CHECK_STATUS(1);

    for (j = 0; j < 5; j++) {
        cptr = ((unsigned char *)d.origBitmapData.Scan0) + (j * d.origBitmapData.Stride);
        printf ("%d: ", j);
        for (i = 0; i < 5; i++) {
            printf ("%02x%02x%02x ", cptr[0], cptr[1], cptr[2]);
            cptr += 3;
        }
        printf ("\n");
    }

    printf ("Modifying (setting to 0xaabbcc)\n");
    for (j = 0; j < 5; j++) {
        cptr = ((unsigned char *)d.origBitmapData.Scan0) + (j * d.origBitmapData.Stride);
        for (i = 0; i < 5; i++) {
            *cptr++ = 0xcc;
            *cptr++ = 0xbb;
            *cptr++ = 0xaa;
        }
    }

    status = GdipBitmapUnlockBits (bitmap, &d.origBitmapData);
    CHECK_STATUS(1);

    GdipFree (scan0);
    return 0;
}
