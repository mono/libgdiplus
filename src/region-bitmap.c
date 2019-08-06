/*
 * Copyright (C) 2006-2007 Novell, Inc (http://www.novell.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "region-private.h"
#include "graphics-path-private.h"
#include "graphics-cairo-private.h"

// #define DEBUG_REGION

#ifdef DEBUG_REGION

/*
 * Debugging helpers
 */

static void
display32 (BYTE *shape, int width, int height)
{
	int i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			printf ("%s", (shape [(i*width + j) * 4] == 0) ? "." : "X");
		}
		printf ("\n");
	}
	printf ("\n");
}

void
display (char* message, GpRegionBitmap *bitmap)
{
	int i = 0, j = 0, k;

	printf ("\n%s\n\tbitmap X: %d, Y: %d, Width: %d, Height %d, Mask %p\n", message,
		bitmap->X, bitmap->Y, bitmap->Width, bitmap->Height, bitmap->Mask);
	if (!bitmap->Mask)
		return;

	while (i < SHAPE_SIZE(bitmap)) {
		BYTE b = bitmap->Mask [i++];
		for (k = 0; k < 8; k++) {
			if (j++ == bitmap->Width) {
				j = 1;
				printf ("\n");
			}
			printf ("%s", ((b & (1 << k)) == 0) ? "." : "X");
		}
	}
	printf ("\n");
}

#endif


/* Helpers */


/*
 * rect_union:
 * @bitmap1: a GpRegionBitmap
 * @bitmap2: a GpRegionBitmap
 * @rect: a pointer to a GpRect
 *
 * Calculate a rectangle, @rect, that contains both @bitmap1 and @bitmap2
 * rectangles.
 */
static void
rect_union (GpRegionBitmap *bitmap1, GpRegionBitmap *bitmap2, GpRect *rect)
{
	int max_x_1 = bitmap1->X + bitmap1->Width;
	int max_x_2 = bitmap2->X + bitmap2->Width;
	int max_y_1 = bitmap1->Y + bitmap1->Height;
	int max_y_2 = bitmap2->Y + bitmap2->Height;

	rect->X = (bitmap1->X < bitmap2->X) ? bitmap1->X : bitmap2->X;
	rect->Y = (bitmap1->Y < bitmap2->Y) ? bitmap1->Y : bitmap2->Y;
	rect->Width = ((max_x_1 > max_x_2) ? max_x_1 : max_x_2) - rect->X;
	rect->Height = ((max_y_1 > max_y_2) ? max_y_1 : max_y_2) - rect->Y;
}


/*
 * rect_intersect:
 * @bitmap1: a GpRegionBitmap
 * @bitmap2: a GpRegionBitmap
 * @rect: a pointer to a GpRect
 *
 * Calculate a rectangle, @rect, that represent the area shared by both 
 * @bitmap1 and @bitmap2 rectangles.
 */
static void
rect_intersect (GpRegionBitmap *bitmap1, GpRegionBitmap *bitmap2, GpRect *rect)
{
	rect->X = (bitmap1->X > bitmap2->X) ? bitmap1->X : bitmap2->X;
	rect->Y = (bitmap1->Y > bitmap2->Y) ? bitmap1->Y : bitmap2->Y;
	rect->Width = (((bitmap1->X + bitmap1->Width) < (bitmap2->X + bitmap2->Width)) ? 
		(bitmap1->X + bitmap1->Width) : (bitmap2->X + bitmap2->Width)) - rect->X;
	rect->Height = (((bitmap1->Y + bitmap1->Height) < (bitmap2->Y + bitmap2->Height)) ? 
		(bitmap1->Y + bitmap1->Height) : (bitmap2->Y + bitmap2->Height)) - rect->Y;
}


/*
 * rect_adjust_horizontal:
 * @x: a pointer to an integer
 * @width: a pointer to an integer
 *
 * Adjust the @x and @width values so that they both are multiples of 32
 * and still encompass, at least, the same data as their original value.
 * The value 32 is chosen to match CAIRO_STRIDE_ALIGNMENT and allow direct
 * use of cairo surfaces.
 */
static void
rect_adjust_horizontal (int *x, int *width)
{
	/* ensure that X is a multiple of 8 */
	int i = (*x & 31);
	if (i > 0) {
		/* reduce X to be a multiple of 8*/
		*x -= i;
		/* but keep the "true" Width constant */
		*width += i;
	}
	/* ensure that Width is a multiple of 8 */
	i = (*width & 31);
	if (i > 0) {
		*width += (32 - i);
	}
}


/*
 * alloc_bitmap_memory:
 * @size: the size of the required allocation
 * @clear: a BOOL
 *
 * Allocate the alpha (1bpp) memory required for storing a bitmap and return
 * a pointer to this memory. @clear decides if the memory will be zeroized
 * after being allocated. NULL can be returned if too much memory is 
 * requested (very large region) or if the memory couldn't be allocated (low
 * memory).
 */
static BYTE*
alloc_bitmap_memory (int size, BOOL clear)
{
	BYTE *buffer;

	if ((size < 1) || (size > REGION_MAX_BITMAP_SIZE)) {
		g_warning ("Requested %d bytes. Maximum size for region is %d bytes.", 
			size, REGION_MAX_BITMAP_SIZE);
		return NULL;
	}

	buffer = (BYTE*) GdipAlloc (size);
	if (!buffer)
		return NULL;

	if (clear)
		memset (buffer, 0, size);

	return buffer;
}


/*
 * alloc_bitmap_with_buffer:
 * @x: an integer representing the X coordinate of the bitmap
 * @y: an integer representing the Y coordinate of the bitmap
 * @width: an integer representing the Width of the bitmap
 * @height: an integer representing the Height of the bitmap
 * @buffer: a byte array of the bitmap data
 *
 * Allocate and return a new GpRegionBitmap structure using the supplied 
 * @buffer.
 *
 * Notes:
 * - The allocated structure must be freed using gdip_region_bitmap_free.
 * - The bitmap @x and @width MUST BE multiple of 8.
 * - The supplied @buffer MUST match the supplied width and height parameters.
 */
static GpRegionBitmap*
alloc_bitmap_with_buffer (int x, int y, int width, int height, BYTE *buffer)
{
	GpRegionBitmap *result = (GpRegionBitmap*) GdipAlloc (sizeof (GpRegionBitmap));
	if (!result) {
		return NULL;
	}

	result->X = x;
	result->Y = y;
	result->Width = width;
	result->Height = height;
	result->Mask = buffer;
	result->reduced = FALSE; /* bitmap size isn't optimal wrt contents */

	return result;
}


/*
 * alloc_bitmap:
 * @x: an integer representing the X coordinate of the bitmap
 * @y: an integer representing the Y coordinate of the bitmap
 * @width: an integer representing the Width of the bitmap
 * @height: an integer representing the Height of the bitmap
 *
 * Allocate and return a new GpRegionBitmap structure.
 *
 * Notes:
 * - The allocated structure must be freed using gdip_region_bitmap_free.
 * - The bitmap @x and @width will be adjusted to a multiple of 8.
 */
static GpRegionBitmap*
alloc_bitmap (int x, int y, int width, int height)
{
	BYTE *buffer;
	int size;

	/* ensure X and Width are multiple of 8 */
	rect_adjust_horizontal (&x, &width);

	size = (width * height >> 3); /* 1 bit per pixel */
	buffer = alloc_bitmap_memory (size, TRUE);

	return alloc_bitmap_with_buffer (x, y, width, height, buffer);
}


/*
 * alloc_merged_bitmap:
 * @bitmap1: a GpRegionBitmap
 * @bitmap2: a GpRegionBitmap
 *
 * Allocate and return a new GpRegionBitmap that covers the total area 
 * (single rectangle) of both @bitmap1 and @bitmap2.
 *
 * Notes:
 * - The allocated structure must be freed using gdip_region_bitmap_free.
 */
static GpRegionBitmap*
alloc_merged_bitmap (GpRegionBitmap *bitmap1, GpRegionBitmap *bitmap2)
{
	GpRect rect;

	rect_union (bitmap1, bitmap2, &rect);
	return alloc_bitmap (rect.X, rect.Y, rect.Width, rect.Height);
}


/*
 * alloc_intersected_bitmap:
 * @bitmap1: a GpRegionBitmap
 * @bitmap2: a GpRegionBitmap
 *
 * Allocate and return a new GpRegionBitmap that covers only the shared 
 * rectangle area of both @bitmap1 and @bitmap2.
 *
 * Notes:
 * - The allocated structure must be freed using gdip_region_bitmap_free.
 * - The bitmap width will be adjusted to a multiple of 8.
 */
static GpRegionBitmap*
alloc_intersected_bitmap (GpRegionBitmap *bitmap1, GpRegionBitmap *bitmap2)
{
	GpRect rect;

	rect_intersect (bitmap1, bitmap2, &rect);
	return alloc_bitmap (rect.X, rect.Y, rect.Width, rect.Height);
}


/*
 * gdip_region_bitmap_clone:
 * @bitmap: a GpRegionBitmap
 *
 * Allocate and return new GpRegionBitmap containing a copy of @bitmap.
 *
 * Note: the allocated structure must be freed using gdip_region_bitmap_free.
 */
GpRegionBitmap*
gdip_region_bitmap_clone (GpRegionBitmap *bitmap)
{
	BYTE *buffer;
	int size = (bitmap->Width * bitmap->Height >> 3); /* 1 bit per pixel */

	if (size > 0) {
		buffer = alloc_bitmap_memory (size, FALSE);
		if (buffer)
			memcpy (buffer, bitmap->Mask, size);
	} else {
		buffer = NULL;
	}
	return alloc_bitmap_with_buffer (bitmap->X, bitmap->Y, bitmap->Width, bitmap->Height, buffer);
}


/*
 * empty_bitmap:
 * @bitmap: a GpRegionBitmap
 *
 * Clear and, if required, free the mask of @bitmap. Note that the allocated 
 * GpRegionBitmap structure MUST still be freed using gdip_region_bitmap_free.
 */
static void
empty_bitmap (GpRegionBitmap *bitmap)
{
	bitmap->X = 0;
	bitmap->Y = 0;
	bitmap->Width = 0;
	bitmap->Height = 0;

	if (bitmap->Mask) {
		GdipFree (bitmap->Mask);
		bitmap->Mask = NULL;
	}
}


/*
 * gdip_region_bitmap_free:
 * @bitmap: a GpRegionBitmap
 *
 * Free the region bitmap @bitmap.
 */
void
gdip_region_bitmap_free (GpRegionBitmap *bitmap)
{
	empty_bitmap (bitmap);
	GdipFree (bitmap);
}


/*
 * gdip_region_bitmap_from_tree:
 * @tree: a GpPathTree
 *
 * Return a new GpRegionBitmap containing the bitmap recomposed from the 
 * @tree.
 *
 * Note: the allocated structure must be freed using gdip_region_bitmap_free.
 */
static GpRegionBitmap*
gdip_region_bitmap_from_tree (GpPathTree *tree)
{
	GpRegionBitmap *result;

	if (!tree)
		return NULL;

	/* each item has... */
	if (tree->path) {
		/* (a) only a path (the most common case) */
		result = gdip_region_bitmap_from_path (tree->path);
	} else {
		/* (b) two items with an binary operation */
		GpRegionBitmap *bitmap1 = gdip_region_bitmap_from_tree (tree->branch1);
		GpRegionBitmap *bitmap2 = gdip_region_bitmap_from_tree (tree->branch2);

		result = gdip_region_bitmap_combine (bitmap1, bitmap2, tree->mode);

		if (bitmap1)
			gdip_region_bitmap_free (bitmap1);
		if (bitmap2)
			gdip_region_bitmap_free (bitmap2);
	}
	return result;
}


/*
 * gdip_region_bitmap_ensure:
 * @region: a GpRegion
 *
 * Ensure the @region bitmap is available (as it isn't created until it is 
 * actually needed).
 */
void
gdip_region_bitmap_ensure (GpRegion *region)
{
	/* we already have the bitmap */
	if (region->bitmap)
		return;

	/* redraw the bitmap from the original path + all other operations/paths */
	region->bitmap = gdip_region_bitmap_from_tree (region->tree);
}


/*
 * gdip_region_bitmap_invalidate:
 * @region: a GpRegion
 *
 * Invalidate (and free) the bitmap (if any) associated with @region. The 
 * bitmap will need to be re-created before begin used.
 */
void
gdip_region_bitmap_invalidate (GpRegion *region)
{
	/* it's possible that the bitmap hasn't yet been created (e.g. if
	   a rectangle region has just been converted to a path region) */
	if (!region->bitmap)
		return;

	empty_bitmap (region->bitmap);
	region->bitmap = NULL;
}

/*
 * gdip_region_bitmap_to_cairo_surface
 * @bitmap: a GpRegionBitmap
 *
 * Create a cairo mask surface for the given region bitmap. Caller is
 * responsible for calling cairo_surface_destroy on the returned surface.
 */
cairo_surface_t *
gdip_region_bitmap_to_cairo_surface (GpRegionBitmap *bitmap)
{
	return cairo_image_surface_create_for_data (bitmap->Mask, CAIRO_FORMAT_A1, bitmap->Width, bitmap->Height, bitmap->Width >> 3);
}

/*
 * gdip_region_bitmap_from_path:
 * @path: a GpPath
 *
 * Return a new GpRegionBitmap containing the bitmap representing the @path.
 * NULL will be returned if the bitmap cannot be created (e.g. too big).
 *
 * Note: the allocated structure must be freed using gdip_region_bitmap_free.
 */
GpRegionBitmap*
gdip_region_bitmap_from_path (GpPath *path)
{
	GpRect bounds;
	GpRegionBitmap *bitmap;
	int i, idx;
	int length = path->count;
	unsigned long long int size;
	cairo_surface_t *surface = NULL;
	cairo_t *cr = NULL;

	/* empty path == empty bitmap */
	if (length == 0)
		return alloc_bitmap_with_buffer (0, 0, 0, 0, NULL);

	/* get the limits of the bitmap we need to allocate */
	if (GdipGetPathWorldBoundsI (path, &bounds, NULL, NULL) != Ok)
		return NULL;

	/* ensure X and Width are multiple of 8 */
	rect_adjust_horizontal (&bounds.X, &bounds.Width);

	/* an empty width or height is valid, even if no bitmap can be produced */
	if ((bounds.Width == 0) || (bounds.Height == 0))
		return alloc_bitmap_with_buffer (bounds.X, bounds.Y, bounds.Width, bounds.Height, NULL);

	/* replay the path list and the operations to reconstruct the bitmap */
	size = (unsigned long long int)(bounds.Width >> 3) * bounds.Height;
	if ((size < 1) || (size > REGION_MAX_BITMAP_SIZE)) {
		g_warning ("Path conversion requested %llu bytes (%d x %d). Maximum size is %d bytes.",
			size, bounds.Width, bounds.Height, REGION_MAX_BITMAP_SIZE);
		return NULL;
	}

	bitmap = alloc_bitmap (bounds.X, bounds.Y, bounds.Width, bounds.Height);
	if (bitmap == NULL)
		return NULL;

	surface = gdip_region_bitmap_to_cairo_surface (bitmap);
	cr = cairo_create (surface);

	idx = 0;
	for (i = 0; i < length; ++i) {
		GpPointF pt = path->points[i];
		BYTE type = path->types[i];
		GpPointF pts [3];
		/* mask the bits so that we get only the type value not the other flags */
		switch (type & PathPointTypePathTypeMask) {
		case PathPointTypeStart:
			cairo_move_to (cr, pt.X - bounds.X, pt.Y - bounds.Y);
			break;
		case PathPointTypeLine:
			cairo_line_to (cr, pt.X - bounds.X, pt.Y - bounds.Y);
			break;
		case PathPointTypeBezier:
			/* make sure we only add at most 3 points to pts */
			if (idx < 3) {
				pts [idx] = pt;
				idx ++;
			}
			/* once we've added 3 pts, we can draw the curve */
			if (idx == 3) {
				cairo_curve_to (cr, pts [0].X - bounds.X, pts [0].Y - bounds.Y, 
					pts [1].X - bounds.X, pts [1].Y - bounds.Y, 
					pts [2].X - bounds.X, pts [2].Y - bounds.Y);
				idx = 0;
			}
			break;
		}

		/* close the subpath */
		if (type & PathPointTypeCloseSubpath)
			cairo_close_path (cr);
	}

	cairo_clip (cr);
	cairo_set_source_rgba (cr, 1, 1, 1, 1);
	cairo_paint (cr);
	cairo_destroy (cr);

	cairo_surface_destroy (surface);

	return bitmap;
}


/*
 * gdip_region_bitmap_get_smallest_rect:
 * @bitmap: a GpRegionBitmap
 * @rect: a pointer to a GpRect
 *
 * Return the minimal used space in the bitmap inside @rect.
 */
void
gdip_region_bitmap_get_smallest_rect (GpRegionBitmap *bitmap, GpRect *rect)
{
	int first_y = bitmap->Height + 1;	/* empty (top) lines */
	int last_y = -1;			/* empty (bottom) lines */
	int first_x = bitmap->Width + 1;	/* empty (left) columns */
	int last_x = -1;			/* empty (right) columns */
	int i = 0;
	int original_size = SHAPE_SIZE(bitmap);
	int x = 0, y = 0;
	int k;

	while (i < original_size) {
		if (bitmap->Mask [i] != 0) {
			for (k = 0; k < 8; k++) {
				if ((bitmap->Mask [i] & (1 << k)) != 0) {
					if (x < first_x)
						first_x = x;
					if (x > last_x)
						last_x = x;
					if (y < first_y)
						first_y = y;
					if (y > last_y)
						last_y = y;				
				}
				x++;
			}
		} else {
			x += 8;
		}
		i++;		
		if (x == bitmap->Width) {
			x = 0;
			y++;
		}
	}

	/* did we found some bits ? */
	if ((last_x == -1) && (last_y == -1) && (first_x == bitmap->Width + 1) && (first_y == bitmap->Height + 1)) {
		rect->X = rect->Y = rect->Width = rect->Height = 0;
	} else {
		// convert to pixel values
		rect->X = bitmap->X + first_x;
		rect->Y = bitmap->Y + first_y;
		rect->Width = last_x - first_x + 1;
		rect->Height = last_y - first_y + 1;
	}
}


/*
 * is_worth_shrinking:
 * @original_size: the original size of a bitmap
 * @new_size: the _potential_ new size of the bitmap (if shrinked)
 *
 * Decide if the current bitmap, based on it's current size, is worth 
 * shrinking to a lesser size.
 *
 * Note: Many binary operations (e.g. intersection) can greatly reduce the
 * size of the final bitmap.
 */
static BOOL
is_worth_shrinking (int original_size, int new_size)
{
	/* FIXME - we can do better than checking if we "save" 4kb */
	return ((original_size - new_size) > 4096);
}


/*
 * gdip_region_bitmap_shrink:
 * @bitmap: a GpRegionBitmap
 * @always_shrink: a BOOL
 *
 * Shrink the @bitmap if either @always_shrink is TRUE, or if it is decided
 * to be worth the CPU time (see is_worth_shrinking).
 *
 * Reducing the bitmap size permit (a) to reduce the memory footprint and
 * (b) makes it more likely to apply certain optimizations using rectangle
 * intersections.
 *
 * Notes: 
 * 1.	we don't call this after an union (because the result will never be
 *	smaller) but other operations can result in a smaller bitmap.
 * 2.	we keep the bitmap width in multiple of 8 - it's simpler and faster
 */
void
gdip_region_bitmap_shrink (GpRegionBitmap *bitmap, BOOL always_shrink)
{
	int original_size, new_size;
	BOOL can_be_reduced;
	GpRect rect;

	/* bitmap (a) was already shrinked, or (b) is empty */
	if (bitmap->reduced || !bitmap->Mask)
		return;

	gdip_region_bitmap_get_smallest_rect (bitmap, &rect);

	if ((rect.Width == 0) || (rect.Height == 0)) {
		/* no, the the bitmap is empty */
		empty_bitmap (bitmap);
		return;
	}

	/* ensure X and Width are multiple of 8 */
	rect_adjust_horizontal (&rect.X, &rect.Width);

	original_size = SHAPE_SIZE(bitmap);
	new_size = (rect.Height * rect.Width) >> 3; /* bits->bytes */
	can_be_reduced = (new_size < original_size);

	/* shrink if:
	 * a. the caller asked for it (and there is a size change)
	 * b. the caller didn't ask for it but "we" decided it's worth it
	 */
	if ((always_shrink && can_be_reduced) || is_worth_shrinking (original_size, new_size)) {
		/* reallocate a new bitmap buffer */
		BYTE *new_mask = alloc_bitmap_memory (new_size, FALSE);
		int new_width, new_height;
		int y;

		int old_width_byte, new_width_byte;

		BYTE* newline = NULL;
		BYTE* oldline = NULL;

		if (!new_mask)
			return;

		new_width = rect.Width;
		new_height = rect.Height;

		old_width_byte = bitmap->Width >> 3;
		new_width_byte = new_width >> 3;

		newline = new_mask;
		oldline = bitmap->Mask + ((rect.Y - bitmap->Y) * old_width_byte) + ((rect.X - bitmap->X) >> 3);
		/* copy the interesting portion in the new bitmap */
		for (y = 0; y < new_height; y++) {
			memcpy (newline, oldline, new_width_byte);
			newline += new_width_byte;
			oldline += old_width_byte;
		}

		/* replace current data */
		bitmap->X = rect.X;
		bitmap->Y = rect.Y;
		bitmap->Width = rect.Width;
		bitmap->Height = rect.Height;
		GdipFree (bitmap->Mask);
		bitmap->Mask = new_mask;
		bitmap->reduced = TRUE;
	}
}


/*
 * is_point_visible:
 * @bitmap: a GpRegionBitmap
 * @x: the horizontal position
 * @y: the vertical position
 *
 * Return TRUE if the @x,@y point is set on the bitmap.
 *
 * Note: No bounds check are done this internal shared function.
 */
static BOOL
is_point_visible (GpRegionBitmap *bitmap, int x, int y)
{
	int pixel, pos, mask;

	/* is the pixel set ? */
	x -= bitmap->X;
	y -= bitmap->Y;

	pixel = (y * bitmap->Width + x);
	pos = (pixel >> 3);
	mask = (pixel & 7);

	return ((bitmap->Mask [pos] & (1 << mask)) != 0);
}


/*
 * gdip_region_bitmap_is_point_visible:
 * @bitmap: a GpRegionBitmap
 * @x: the horizontal position
 * @y: the vertical position
 *
 * Return TRUE if the @x,@y point is set on the bitmap.
 *
 * Note: Using a bitmap reduce the precision to integers.
 */
BOOL
gdip_region_bitmap_is_point_visible (GpRegionBitmap *bitmap, int x, int y)
{
	/* is this an empty bitmap ? */
	if ((bitmap->Width == 0) || (bitmap->Height == 0))
		return FALSE;

	/* is the point inside the bitmap ? */
	if ((x < bitmap->X) || (x >= bitmap->X + bitmap->Width))
		return FALSE;
	if ((y < bitmap->Y) || (y >= bitmap->Y + bitmap->Height))
		return FALSE;

	return is_point_visible (bitmap, x, y);
}


/*
 * gdip_region_bitmap_is_point_visible:
 * @bitmap: a GpRegionBitmap
 * @rect: a pointer to a GpRect
 *
 * Return TRUE is _any_ part of @rect is inside the region.
 */
BOOL
gdip_region_bitmap_is_rect_visible (GpRegionBitmap *bitmap, GpRect *rect)
{
	int x, y;

	/* is this an empty bitmap ? */
	if ((bitmap->Width == 0) || (bitmap->Height == 0))
		return FALSE;

	/* quick intersection checks */
	if (bitmap->X >= rect->X + rect->Width)
		return FALSE;
	if (bitmap->X + bitmap->Width <= rect->X)
		return FALSE;
	if (bitmap->Y >= rect->Y + rect->Height)
		return FALSE;
	if (bitmap->Y + bitmap->Height <= rect->Y)
		return FALSE;

	/* TODO - optimize */
	for (y = rect->Y; y < rect->Y + rect->Height; y++) {
		for (x = rect->X; x < rect->X + rect->Width; x++) {
			if (gdip_region_bitmap_is_point_visible (bitmap, x, y))
				return TRUE;
		}
	}

	return FALSE;
}


/*
 * get_buffer_pos:
 * @shape: a GpRegionBitmap
 * @x: the horizontal position
 * @y: the vertical position
 *
 * Return the index, inside the @shape buffer, corresponding to the @x,@y 
 * point.
 */
static int
get_buffer_pos (GpRegionBitmap *shape, int x, int y)
{
	/* check for out of bounds */
	if ((x < shape->X) || (x >= shape->X + shape->Width))
		return -1;
	if ((y < shape->Y) || (y >= shape->Y + shape->Height))
		return -1;

	x -= shape->X;
	y -= shape->Y;
	return ((y * shape->Width + x) >> 3);
}


/*
 * get_byte:
 * @shape: a GpRegionBitmap
 * @x: the horizontal position
 * @y: the vertical position
 *
 * Return the byte, from the @shape buffer, corresponding to the @x,@y point.
 * Note that this byte contains 8 pixels.
 */
static int
get_byte (GpRegionBitmap *shape, int x, int y)
{
	/* out of bounds == empty (no pixel) */
	int pos = get_buffer_pos (shape, x, y);
	return (pos == -1) ? 0 : shape->Mask [pos];
}


/*
 * Process a single line for gdip_region_bitmap_get_scans.
 */
static BOOL
process_line (GpRegionBitmap *bitmap, int y, int *x, int *w)
{
	int pos = *x;
	*x = -1;
	*w = -1;

	while (pos < bitmap->X + bitmap->Width) {
		BOOL visible = gdip_region_bitmap_is_point_visible (bitmap, pos, y);
		if (*x == -1) {
			if (visible) {
				*x += pos + 1;
			}
		} else {
			if (!visible) {
				*w = pos - *x;
				return TRUE;
			}
		}
		pos++;
	}

	/* end of line - have we started a rect ? */
	if (*x != -1) {
		*w = pos - *x;
		return TRUE;
	}

	return FALSE;
}


/*
 * gdip_region_bitmap_get_scans:
 * @bitmap: a GpRegionBitmap
 * @rect: a pointer to an array of GpRectF
 *
 * Convert the scan lines of the bitmap into an array of GpRectF. The return
 * value represents the actual number of GpRectF entries that were generated.
 */
int
gdip_region_bitmap_get_scans (GpRegionBitmap *bitmap, GpRectF *rect)
{
	if (!bitmap || !bitmap->Mask)
		return 0;

	GpRect actual;
	int x, y, w;
	int n = 0;
	
	actual.X = REGION_INFINITE_POSITION;
	actual.Width = REGION_INFINITE_LENGTH;
	/* for each line in the bitmap */
	for (y = bitmap->Y; y < bitmap->Y + bitmap->Height; y++) {
		/* until we processed the whole line */
		x = bitmap->X;
		while (process_line (bitmap, y, &x, &w)) {
			/* FIXME - we only look at the last rectangle but we could check all
				rectangles in the previous line (and retain perfect rendering
				with, possibly, less rectangle. We could also allow non exact
				match for X and Width (e.g. +/- 1 pixel). MS doesn't seems to
				return perfect rectangles for all shapes. */

			/* if position (X) and Width are identical to previous rectangle */
			if ((x == actual.X) && (w == actual.Width)) {
				/* then augment it's Height by one */
				if (rect && (n > 0)) {
					rect [n - 1].Height++;
				}
			} else {
				actual.X = x;
				actual.Y = y;
				actual.Width = w;
				actual.Height = 1;

				if (rect) {
					rect [n].X = actual.X;
					rect [n].Y = actual.Y;
					rect [n].Width = actual.Width;
					rect [n].Height = actual.Height;
				}
				n++;
			}
			/* continue on the same line */
			x += w + 1;
		}
	}
	return n;
}


/*
 * Binary operators helper functions
 */


/* 
 * bitmap_intersect:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 *
 * This function checks if the rectangle containing @shape1 intersect with
 * the rectangle containing @shape2. It is used to optimize certain code
 * path in the binary operations.
 */
static BOOL
bitmap_intersect (GpRegionBitmap *shape1, GpRegionBitmap *shape2)
{
	return ((shape1->X < shape2->X + shape2->Width) &&
		(shape1->X + shape1->Width > shape2->X) &&
		(shape1->Y < shape2->Y + shape2->Height) &&
		(shape1->Y + shape1->Height > shape2->Y));
}


/* 
 * gdip_region_bitmap_compare:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 *
 * This function checks if the data inside @shape1 is identical to the data
 * inside @shape2 - even if their respective rectangles are different.
 */
BOOL
gdip_region_bitmap_compare (GpRegionBitmap *shape1, GpRegionBitmap *shape2)
{
	GpRect rect;
	int x, y;

	/* if the rectangles containing shape1 and shape2 DO NOT
	   intersect, then there is no possible intersection */
	if (!bitmap_intersect (shape1, shape2))
		return FALSE;

	rect_union (shape1, shape2, &rect);
	for (y = rect.Y; y < rect.Y + rect.Height; y++) {
		for (x = rect.X; x < rect.X + rect.Width; x += 8) {
			if (get_byte (shape1, x, y) != get_byte (shape2, x, y))
				return FALSE;
		}
	}

	return TRUE;
}


/*
 * Binary operators on bitmap regions
 *
 * Notes
 * - All operations requires the bitmap x origin and it's width to be multiple
 *   of 8.
 */


/*
 * gdip_region_bitmap_union:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 *
 * Return a new bitmap containing the union of the two specified region 
 * bitmaps.
 */
static GpRegionBitmap*
gdip_region_bitmap_union (GpRegionBitmap *shape1, GpRegionBitmap *shape2)
{
	GpRegionBitmap *op = alloc_merged_bitmap (shape1, shape2);
	int x, y;

	for (y = op->Y; y < op->Y + op->Height; y++) {
		int p = get_buffer_pos (op, op->X, y);
		for (x = op->X; x < op->X + op->Width; x += 8) {
			op->Mask [p++] = get_byte (shape1, x, y) | get_byte (shape2, x, y);
		}
	}

	/* no need to call reduce_bitmap (it will never shrink, 
	   unless the original bitmap were oversized) */
	return op;
}


/*
 * gdip_region_bitmap_intersection:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 *
 * Return a new bitmap containing the intersection of the two specified region
 * bitmaps.
 */
static GpRegionBitmap*
gdip_region_bitmap_intersection (GpRegionBitmap *shape1, GpRegionBitmap *shape2)
{
	GpRegionBitmap *op;
	int x, y;

	/* if the rectangles containing shape1 and shape2 DO NOT
	   intersect, then there is no possible intersection */
	if (!bitmap_intersect (shape1, shape2))
		return alloc_bitmap_with_buffer (0, 0, 0, 0, NULL);

	/* the bitmap size cannot be bigger than a rectangle intersection of
	   both bitmaps */
	op = alloc_intersected_bitmap (shape1, shape2);

	for (y = op->Y; y < op->Y + op->Height; y++) {
		int p = get_buffer_pos (op, op->X, y);
		for (x = op->X; x < op->X + op->Width; x += 8) {
			op->Mask [p++] = get_byte (shape1, x, y) & get_byte (shape2, x, y);
		}
	}

	/* reduce bitmap size - if it make sense */
	gdip_region_bitmap_shrink (op, FALSE);
	return op;
}


/*
 * gdip_region_bitmap_exclude:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 *
 * Return a new bitmap containing the first shape minus the second shape.
 */
static GpRegionBitmap*
gdip_region_bitmap_exclude (GpRegionBitmap *shape1, GpRegionBitmap *shape2)
{
	GpRegionBitmap *op;
	int x, y;

	/* if the rectangles containing shape1 and shape2 DO NOT
	   intersect, then the result is identical shape1 */
	if (!bitmap_intersect (shape1, shape2))
		return gdip_region_bitmap_clone (shape1);

	/* the new bitmap size cannot be bigger than shape1 */
	op = alloc_bitmap (shape1->X, shape1->Y, shape1->Width, shape1->Height);

	for (y = op->Y; y < op->Y + op->Height; y++) {
		int p = get_buffer_pos (op, op->X, y);
		for (x = op->X; x < op->X + op->Width; x += 8) {
			BYTE b1 = get_byte (shape1, x, y);
			op->Mask [p++] = b1 - (b1 & get_byte (shape2, x, y));
		}
	}

	/* reduce bitmap size - if it make sense */
	gdip_region_bitmap_shrink (op, FALSE);
	return op;
}


/*
 * gdip_region_bitmap_complement:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 *
 * Return a new bitmap containing the second shape minus the first shape.
 */
static GpRegionBitmap*
gdip_region_bitmap_complement (GpRegionBitmap *shape1, GpRegionBitmap *shape2)
{
	GpRegionBitmap *op;
	int x, y;

	/* if the rectangles containing shape1 and shape2 DO NOT
	   intersect, then the result is identical shape2 */
	if (!bitmap_intersect (shape1, shape2))
		return gdip_region_bitmap_clone (shape2);

	/* the new bitmap size cannot be bigger than shape2 */
	op = alloc_bitmap (shape2->X, shape2->Y, shape2->Width, shape2->Height);

	for (y = op->Y; y < op->Y + op->Height; y++) {
		int p = get_buffer_pos (op, op->X, y);
		for (x = op->X; x < op->X + op->Width; x += 8) {
			BYTE b2 = get_byte (shape2, x, y);
			op->Mask [p++] = b2 - (b2 & get_byte (shape1, x, y));
		}
	}

	/* reduce bitmap size - if it make sense */
	gdip_region_bitmap_shrink (op, FALSE);
	return op;
}


/*
 * gdip_region_bitmap_xor:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 *
 * Return a new bitmap containing the exclusive-or of the two specified region
 * bitmaps.
 */
static GpRegionBitmap*
gdip_region_bitmap_xor (GpRegionBitmap *shape1, GpRegionBitmap *shape2)
{
	GpRegionBitmap *op;
	int x, y;

	/* if the rectangles containing shape1 and shape2 DO NOT intersect,
	   then the result is identical an union of shape1 and shape2. Code is
	   almost similar but no reduction is required for an union. */
	if (!bitmap_intersect (shape1, shape2))
		return gdip_region_bitmap_union (shape1, shape2);

	/* the new bitmap is potentially as big as the two merged bitmaps */
	op = alloc_merged_bitmap (shape1, shape2);

	for (y = op->Y; y < op->Y + op->Height; y++) {
		int p = get_buffer_pos (op, op->X, y);
		for (x = op->X; x < op->X + op->Width; x += 8) {
			op->Mask [p++] = get_byte (shape1, x, y) ^ get_byte (shape2, x, y);
		}
	}

	/* reduce bitmap size - if it make sense */
	gdip_region_bitmap_shrink (op, FALSE);
	return op;
}


/*
 * gdip_region_bitmap_combine:
 * @shape1: a GpRegionBitmap
 * @shape2: a GpRegionBitmap
 * @combineMode: the binary operator to apply between the two shapes
 *
 * Return a new GpRegionBitmap containing a new bitmap resulting from applying
 * the @combineMode to @shape1 and @shape2 bitmaps.
 */
GpRegionBitmap*
gdip_region_bitmap_combine (GpRegionBitmap *bitmap1, GpRegionBitmap* bitmap2, CombineMode combineMode)
{
	if (!bitmap1 || !bitmap2)
		return NULL;

	switch (combineMode) {
	case CombineModeComplement:
		return gdip_region_bitmap_complement (bitmap1, bitmap2);
	case CombineModeExclude:
		return gdip_region_bitmap_exclude (bitmap1, bitmap2);
	case CombineModeIntersect:
		return gdip_region_bitmap_intersection (bitmap1, bitmap2);
	case CombineModeUnion:
		return gdip_region_bitmap_union (bitmap1, bitmap2);
	case CombineModeXor:
		return gdip_region_bitmap_xor (bitmap1, bitmap2);
	default:
		g_warning ("Unkown combine mode specified (%d)", combineMode);
		return NULL;
	}
}
