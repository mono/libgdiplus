#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "GdiPlusFlat.h"

static int status_counter = 0;

#define CHECK_STATUS(x) do { if (status != Ok) { printf ("status[%d] == %d!\n", status_counter++, status); if(x) { exit(-1); } } else { printf ("status[%d] == Ok\n", status_counter++); } } while (0)
#define CHECK_ASSERT(x) do { if (!(x)) { printf ("check %s at %s:%d failed\n", #x, __FILE__, __LINE__); exit(-1); } else { printf("check %s at %s:%d passed\n", #x, __FILE__, __LINE__); }  } while (0)
 
CLSID png_clsid = { 0x557cf406, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };

int
main (int argc, char **argv)
{
    GpImage *img;
    gunichar2 *unis;
    GpBitmap *bitmap;
    GpStatus status;
    int original_palette_size;
    int reloaded_palette_size;
    ColorPalette *original_palette;
    ColorPalette *reloaded_palette;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // PNG resave should preserve the palette transparency. Let's test it
    // by loading a PNG file and its palette, then resaving it and loading
    // it again for comparison.
    unis = g_utf8_to_utf16 ("test-trns.png", -1, NULL, NULL, NULL);
    status = GdipLoadImageFromFile (unis, &img);
    CHECK_STATUS(1);
    g_free (unis);

    status = GdipGetImagePaletteSize (img, &original_palette_size);
    CHECK_STATUS(1);
    CHECK_ASSERT(original_palette_size > 0);
    original_palette = malloc (original_palette_size);
    GdipGetImagePalette (img, original_palette, original_palette_size);
    CHECK_STATUS(1);

    unis = g_utf8_to_utf16 ("test-trns-resave.png", -1, NULL, NULL, NULL);
    status = GdipSaveImageToFile (img, unis, &png_clsid, NULL);
    CHECK_STATUS(1);
    GdipDisposeImage (img);
    status = GdipLoadImageFromFile (unis, &img);
    CHECK_STATUS(1);
    g_free (unis);

    status = GdipGetImagePaletteSize (img, &reloaded_palette_size);
    CHECK_STATUS(1);
    CHECK_ASSERT(reloaded_palette_size > 0);
    CHECK_ASSERT(reloaded_palette_size == original_palette_size);
    reloaded_palette = malloc (reloaded_palette_size);
    GdipGetImagePalette (img, reloaded_palette, reloaded_palette_size);
    CHECK_STATUS(1);

    CHECK_ASSERT(memcmp (original_palette, reloaded_palette, original_palette_size) == 0);

    GdipDisposeImage (img);
    img = NULL;
    unlink ("test-trns-resave.png");
    free (original_palette);
    free (reloaded_palette);

    return 0;
}
