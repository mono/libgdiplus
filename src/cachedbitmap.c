#include "general-private.h"
#include "bitmap-private.h"
#include "graphics-private.h"
#include "cachedbitmap-private.h"

GpStatus WINGDIPAPI 
GdipCreateCachedBitmap (GpBitmap *bitmap, GpGraphics *graphics, GpCachedBitmap **cachedBitmap)
{
	cairo_t *ct;
	cairo_surface_t *surface;
	GpCachedBitmap *newCachedBitmap;
	cairo_status_t status;

	if (!bitmap || !graphics || !cachedBitmap)
		return InvalidParameter;
	if (bitmap->type != ImageTypeBitmap)
		return InvalidParameter;

	gdip_bitmap_ensure_surface (bitmap);

	surface = cairo_surface_create_similar (bitmap->surface, CAIRO_CONTENT_COLOR_ALPHA, bitmap->active_bitmap->width, bitmap->active_bitmap->height);

	ct = cairo_create (surface);

	cairo_set_source_surface (ct, bitmap->surface, 0, 0);
	cairo_paint (ct);

	cairo_destroy (ct);

	status = cairo_surface_status (surface);
	if (status != CAIRO_STATUS_SUCCESS) {
		cairo_surface_destroy (surface);
		return gdip_get_status (status);
	}

	newCachedBitmap = GdipAlloc (sizeof (GpCachedBitmap));
	if (!newCachedBitmap) 
	{
		cairo_surface_destroy(surface);
		return OutOfMemory;
	}

	newCachedBitmap->surface = surface;
	*cachedBitmap = newCachedBitmap;

	return Ok;
}

GpStatus WINGDIPAPI 
GdipDeleteCachedBitmap (GpCachedBitmap *cachedBitmap)
{
	if (!cachedBitmap) 
		return InvalidParameter;

	cairo_surface_destroy (cachedBitmap->surface);
	GdipFree (cachedBitmap);

	return Ok;
}
