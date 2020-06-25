#include "general-private.h"
#include "bitmap-private.h"
#include "graphics-private.h"
#include "cachedbitmap-private.h"

GpStatus WINGDIPAPI GdipCreateCachedBitmap (GpBitmap *bitmap, GpGraphics *graphics, GpCachedBitmap **cachedBitmap)
{
    cairo_surface_t *surface;
    GpCachedBitmap *newCachedBitmap;
    cairo_status_t status;

    if (!bitmap || !graphics || !cachedBitmap)
        return InvalidParameter;
    if (bitmap->type != ImageTypeBitmap)
        return InvalidParameter;

    surface = cairo_surface_create_similar_image(bitmap->surface, bitmap->cairo_format, bitmap->active_bitmap->width, bitmap->active_bitmap->height);

	status = cairo_surface_status (surface);
    if (status != CAIRO_STATUS_SUCCESS) {
        cairo_surface_destroy (surface);
        return gdip_get_status (status);
    }

    newCachedBitmap = GdipAlloc (sizeof (GpCachedBitmap));
    if (!newCachedBitmap) 
        return OutOfMemory;

    newCachedBitmap->surface = surface;

    *cachedBitmap = newCachedBitmap;
    return Ok;
}

GpStatus WINGDIPAPI GdipDeleteCachedBitmap (GpCachedBitmap *cachedBitmap)
{
    if (!cachedBitmap) 
        return InvalidParameter;

    cairo_surface_destroy(cachedBitmap->surface);
    GdipFree(cachedBitmap);

    return Ok;
}