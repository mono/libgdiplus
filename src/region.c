/*
 * Copyright (c) 2004-2005 Ximian
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
 *          Jordi Mas i Hernandez <jordi@ximian.com>, 2004-2005
 *          Sebastien Pouliot  <sebastien@ximian.com>
 *
 */

#include "region-private.h"
#include "general-private.h"
#include "graphics-path-private.h"

/*
	Helper functions
*/

void
gdip_region_init (GpRegion *result)
{
	result->type = RegionTypeInfinite;
	result->cnt = 0;
	result->rects = NULL;
	result->tree = NULL;
	result->bitmap = NULL;
}

GpRegion *
gdip_region_new ()
{
	GpRegion *result;

	result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	if (result)
		gdip_region_init (result);

	return result;
}

static int
gdip_compare_rectf (const void *a, const void *b) {
	const GpRectF *r1 = (GpRectF*)a;
	const GpRectF *r2 = (GpRectF*)b;
	if (r1->Y == r2->Y && r1->X == r2->X)
		return 0;
	if (r1->Y > r2->Y || (r1->Y == r2->Y && r1->X > r2->X))
		return 1;
	return -1;
}

static void
gdip_sort_rect_array (GpRectF* array, int length) {
	qsort (array, length, sizeof (GpRectF), gdip_compare_rectf);
}

// Not a mistake in the name, it is for re-sorting nearly-sorted data.
// Insertion sort.
static void
gdip_sort_rect_array_sorted (GpRectF* array, int length) {
	GpRectF rect;
	GpRectF *i, *j;

	for (i = array + 1; i < array + length; i++) {
		rect = *i;
		for (j = i - 1; j >= array && gdip_compare_rectf (j, &rect) > 0; j--) {
			*(j + 1) = *j;
		}
		*(j + 1) = rect;
	}
}

static GpStatus
gdip_extend_rect_array (GpRectF** srcarray, int* elements, int* capacity) {
	GpRectF *array;
	int newCapacity = -1;

	if (capacity) {
		if (*srcarray == NULL) {
			if (*capacity < 1)
				*capacity = 5; // starting capacity if we're given a size of zero
			newCapacity = *capacity;
		} else if (*elements == *capacity) {
			newCapacity = *elements * 2;
		}
	} else {
		newCapacity = *elements + 1;
	}

	if (newCapacity > 0) {
		array = GdipAlloc (sizeof (GpRectF) * newCapacity);
		if (!array)
			return OutOfMemory;

		memcpy (array, *srcarray, sizeof (GpRectF) * (*elements));

		if (*srcarray)
			GdipFree (*srcarray);

		*srcarray = array;
		if (capacity)
			*capacity = newCapacity;
	}
	return Ok;
}

static GpStatus
gdip_trim_rect_array (GpRectF** srcarray, int elements) {
	GpRectF *array;

	array = GdipAlloc (sizeof (GpRectF) * elements);
	if (!array)
		return OutOfMemory;

	memcpy (array, *srcarray, sizeof (GpRectF) * elements);

	if (*srcarray)
		GdipFree (*srcarray);

	*srcarray = array;
	return Ok;
}

static GpStatus
gdip_add_rect_to_array (GpRectF** srcarray, int* elements, int* capacity, const GpRectF* rect)
{
	GpRectF *next;
	GpStatus status;

	status = gdip_extend_rect_array (srcarray, elements, capacity);
	if (status != Ok)
		return status;

	next = *srcarray;
	next += (*elements);
	memcpy (next, rect, sizeof (GpRectF));

	*elements = *elements + 1;

	return Ok;
}

static GpRectF*
gdip_binsearch_rect_array (GpRectF* array, int elements, const GpRectF* search, int* index)
{
	GpRectF *next;
	int upper = elements, lower = 0, mid;

	while (upper > lower) {
		mid = (upper + lower) / 2;
		next = array + mid;
		if (gdip_compare_rectf (search, next) > 0) {
			lower = mid + 1;
		} else {
			upper = mid;
		}
	}
	next = array + lower;
	if (index)
		*index = lower;
	return next;
}

static GpStatus
gdip_add_rect_to_array_sorted (GpRectF** srcarray, int* elements, int* capacity, const GpRectF* rect)
{
	GpRectF *next;
	GpStatus status;
	int insertAt;

	status = gdip_extend_rect_array (srcarray, elements, capacity);
	if (status != Ok)
		return status;

	next = gdip_binsearch_rect_array (*srcarray, *elements, rect, &insertAt);
	memmove (next + 1, next, sizeof (GpRectF) * (*elements - insertAt));
	memcpy (next, rect, sizeof (GpRectF));

	*elements = *elements + 1;

	return Ok;
}

static BOOL
gdip_is_Point_in_RectF_Visible (float x, float y, GpRectF* rect)
{
	if ((x >= rect->X && x < (rect->X + rect->Width))
		&& (y >= rect->Y && y < (rect->Y + rect->Height)))
		return TRUE;
	else
		return FALSE;
}

static BOOL
gdip_is_Point_in_RectFs_Visible (float x, float y, GpRectF* r, int cnt)
{
	GpRectF* rect = r;
	int i;

	for (i = 0; i < cnt; i++, rect++) {
		if (gdip_is_Point_in_RectF_Visible (x, y, rect)) {
			return TRUE;
		}
	}

	return FALSE;
}

static BOOL
gdip_is_Rect_in_RectF_Visible (float x, float y, float width, float height, GpRectF* rect)
{
	if (rect->Width == 0 || rect->Height == 0)
		return FALSE;

	return x < rect->X + rect->Width && x + width > rect->X && y < rect->Y + rect->Height && y + height > rect->Y;
}

static BOOL
gdip_is_Rect_in_RectFs_Visible (float x, float y, float width, float height, GpRectF* r, int cnt)
{
	GpRectF* rect = r;
	int i;

	for (i = 0; i < cnt; i++, rect++) {
		if (gdip_is_Rect_in_RectF_Visible (x, y, width, height, rect))
			return TRUE;
	}

	return FALSE;
}

static void
gdip_get_bounds (GpRectF *allrects, int allcnt, GpRectF *bound)
{
	float nx, ny, fx, fy;
	int i;
	GpRectF *rect;

	if (allrects == NULL || allcnt == 0) {
		bound->X = bound->Y = bound->Width =  bound->Height = 0;
		return;
	}

	/* Build a rect that contains all the rects inside. Smallest x,y and biggest x,y*/
	nx = allrects->X; ny = allrects->Y;
	fx = allrects->X + allrects->Width; fy = allrects->Y + allrects->Height;

	for (i = 0, rect = allrects; i < allcnt; i++, rect++) {

		if (rect->X < nx)
			nx = rect->X;

		if (rect->Y < ny)
			ny = rect->Y;

		if (rect->X + rect->Width  > fx)
			fx = rect->X + rect->Width;

		if (rect->Y + rect->Height > fy)
			fy = rect->Y + rect->Height;
	}

	bound->X = nx; bound->Y = ny;
	bound->Width = fx - nx; bound->Height = fy - ny;
}

static BOOL
gdip_is_region_empty (const GpRegion *region, BOOL allowNegative)
{
	GpRectF rect;

	if (!region)
		return FALSE;

	switch (region->type) {
	case RegionTypeRect:
		if (!region->rects || (region->cnt == 0))
			return TRUE;

		gdip_get_bounds (region->rects, region->cnt, &rect);
		return gdip_is_rectF_empty (&rect, allowNegative);
	case RegionTypeInfinite:
		return FALSE;
	case RegionTypePath:
		if (!region->tree)
			return TRUE;
		if (region->tree->path) {
			if (region->tree->path->count == 0)
				return TRUE;
			
			// Open paths are empty.
			if (!gdip_path_closed (region->tree->path))
				return TRUE;
		}
		if (region->bitmap && (region->bitmap->Width == 0 || region->bitmap->Height == 0))
			return TRUE;

		return FALSE;
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return FALSE;
	}
}

static BOOL
gdip_is_rect_infinite (const GpRectF *rect)
{
	if (!rect)
		return FALSE;
		
	if (gdip_is_rectF_empty (rect, /* allowNegative */ TRUE))
		return FALSE;

	if (rect->Width >= REGION_INFINITE_LENGTH || rect->Height >= REGION_INFINITE_LENGTH)
		return TRUE;

	return FALSE;
}

BOOL
gdip_is_InfiniteRegion (const GpRegion *region)
{
	switch (region->type) {
	case RegionTypeRect:
		if (region->cnt != 1)
		      return FALSE;
		return gdip_is_rect_infinite (region->rects);
	case RegionTypePath:
		/* FIXME: incomplete and not 100% accurate (curves) - but cover the most common case */
		if (!region->tree || !region->tree->path)
			return FALSE;

		if (gdip_path_closed (region->tree->path) && region->tree->path->count == 4) {
			GpRectF bounds;
			if (GdipGetPathWorldBounds (region->tree->path, &bounds, NULL, NULL) == Ok)
				return gdip_is_rect_infinite (&bounds);
		}
		break;
	case RegionTypeInfinite:
		return TRUE;
	default:
		g_warning ("unknown type 0x%08X", region->type);
		break;
	}
	return FALSE;
}

static BOOL
gdip_intersects (const GpRectF *rect1, const GpRectF *rect2)
{
	return (rect1->X < rect2->X + rect2->Width &&
		rect1->X + rect1->Width > rect2->X &&
		rect1->Y < rect2->Y + rect2->Height &&
		rect1->Y + rect1->Height > rect2->Y);
}

static BOOL
gdip_intersects_or_touches (GpRectF *rect1, GpRectF *rect2)
{
	return (rect1->X <= rect2->X + rect2->Width &&
		rect1->X + rect1->Width >= rect2->X &&
		rect1->Y <= rect2->Y + rect2->Height &&
		rect1->Y + rect1->Height >= rect2->Y);
}

/* Is source contained in target ? */
static BOOL
gdip_contains (GpRectF *rect1, GpRectF *rect2)
{
	return (rect1->X >= rect2->X &&
		rect1->X + rect1->Width <= rect2->X + rect2->Width &&
		rect1->Y >= rect2->Y &&
		rect1->Y + rect1->Height <= rect2->Y + rect2->Height);
}

static BOOL
gdip_add_rect_to_array_notcontained (GpRectF** srcarray, int* elements, int* capacity,  GpRectF* rect)
{
	int i;
	GpRectF* rectarray = *srcarray;

	if (rect->Height <= 0 || rect->Width <= 0)
		return FALSE;

	for (i = 0; i < *elements; i++, rectarray++) {
		if (gdip_contains (rect, rectarray) == TRUE) {
			return FALSE;
		}
	}

	gdip_add_rect_to_array (srcarray, elements, capacity, rect);
	return TRUE;
}


static BOOL
gdip_equals (GpRectF *rect1, GpRectF *rect2)
{
	if (!rect1)
		return (rect2 == NULL);

	return (rect1->X == rect2->X &&
		rect1->Width == rect2->Width &&
		rect1->Y == rect2->Y &&
		rect1->Height == rect2->Height);
}

BOOL
gdip_is_Point_in_RectF_inclusive (float x, float y, GpRectF* rect)
{
	if ((x >= rect->X && x <= (rect->X + rect->Width))
		&& (y >= rect->Y && y <= (rect->Y + rect->Height)))
		return TRUE;
	else
		return FALSE;
}

/* Finds a rect that has the lowest x and y after the src rect provided */
static BOOL
gdip_getlowestrect (GpRectF *rects, int cnt, GpRectF* src, GpRectF* rslt)
{
	int i;
	GpRectF *current;
	GpRectF *lowest = NULL;

	for (i = 0, current = rects; i < cnt; i++, current++) {
		if (current->Width <= 0 || current->Height <= 0)
			continue;

		if (current->Y > src->Y ||
			(current->Y == src->Y && current->X > src->X)) {
			if (lowest == NULL) {
				lowest = current;
			}
			else {
				if (current->Y < lowest->Y ||
					(current->Y == lowest->Y && current->X < lowest->X)) {
						lowest = current;
				}
			}
		}
	}

	if (lowest == NULL) {
		return FALSE;
	}

	rslt->X = lowest->X; rslt->Y = lowest->Y;
	rslt->Width = lowest->Width; rslt->Height = lowest->Height;
	return TRUE;
}

void 
gdip_clear_region (GpRegion *region)
{
	region->type = RegionTypeInfinite;

	if (region->rects) {
		GdipFree (region->rects);
		region->rects = NULL;
	}

	if (region->tree) {
		gdip_region_clear_tree (region->tree);
		GdipFree (region->tree);
		region->tree = NULL;
	}

	if (region->bitmap) {
		gdip_region_bitmap_free (region->bitmap);
		region->bitmap = NULL;
	}

	region->cnt = 0;
}

GpStatus
gdip_copy_region (GpRegion *source, GpRegion *dest)
{
	GpStatus status;

	dest->type = source->type;

	if (source->rects) {
		dest->cnt = source->cnt;
		dest->rects = (GpRectF *) GdipAlloc (sizeof (GpRectF) * source->cnt);
		if (!dest->rects)
			return OutOfMemory;

		memcpy (dest->rects, source->rects, sizeof (GpRectF) * source->cnt);
	} else {
		dest->cnt = 0;
		dest->rects = NULL;
	}

	if (source->tree) {
		dest->tree = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));
		if (!dest->tree)
			return OutOfMemory;

		status = gdip_region_copy_tree (source->tree, dest->tree);
		if (status != Ok)
			return status;
	} else {
		dest->tree = NULL;
	}

	if (source->bitmap) {
		dest->bitmap = gdip_region_bitmap_clone (source->bitmap);
	} else {
		dest->bitmap = NULL;
	}

	return Ok;
}

/* convert a rectangle-based region to a path based region */
static GpStatus
gdip_region_convert_to_path (GpRegion *region)
{
	GpStatus status;

	/* no conversion is required for complex regions */
	if (!region || (region->type == RegionTypePath))
		return Ok;

	region->tree = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));
	if (!region->tree)
		return OutOfMemory;

	status = GdipCreatePath (FillModeAlternate, &region->tree->path);
	if (status != Ok)
		return status;

	switch (region->type) {
	case RegionTypeRect:
	case RegionTypeInfinite: {
		/* all rectangles are converted into a single path */
		for (int i = 0; i < region->cnt; i++) {
			RectF normalized;
			gdip_normalize_rectangle (&region->rects[i], &normalized);
			GdipAddPathRectangle (region->tree->path, normalized.X, normalized.Y, normalized.Width, normalized.Height);
		}

		break;
	}
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}

	if (region->rects) {
		GdipFree (region->rects);
		region->cnt = 0;
		region->rects = NULL;
	}

	region->type = RegionTypePath;
	return Ok;
}

/*
 * Create a region (path-tree) from a path.
 */
static GpStatus
gdip_region_create_from_path (GpRegion *region, GpPath *path)
{
	region->type = RegionTypePath;
	region->tree = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));
	if (!region->tree)
		return OutOfMemory;

	return GdipClonePath (path, &region->tree->path);
}

/*
	API implementation
*/

// coverity[+alloc : arg-*0]
GpStatus WINGDIPAPI
GdipCreateRegion (GpRegion **region)
{
	GpRegion *result;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!region)
		return InvalidParameter;
	
	result = gdip_region_new ();
	if (!result)
		return OutOfMemory;

	/* GdipSetInfinite handles setting region->type. */
	status = GdipSetInfinite (result);
	if (status != Ok) {
		GdipDeleteRegion (result);
		return status;
	}

	*region = result;
	return Ok;
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCreateRegionRect (GDIPCONST GpRectF *rect, GpRegion **region)
{
	GpRegion *result;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!region || !rect)
		return InvalidParameter;

	result = gdip_region_new ();
	if (!result)
		return OutOfMemory;

	result->type = RegionTypeRect;
	status = gdip_add_rect_to_array (&result->rects, &result->cnt, NULL, rect);
	if (status != Ok) {
		GdipDeleteRegion (result);
		return status;
	}

	*region = result;
	return Ok;
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCreateRegionRectI (GDIPCONST GpRect *rect, GpRegion **region)
{
	GpRectF rectF;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!region || !rect)
		return InvalidParameter;
	
	gdip_RectF_from_Rect (rect, &rectF);
	return GdipCreateRegionRect (&rectF, region);
}

// coverity[+alloc : arg-*2]
GpStatus WINGDIPAPI
GdipCreateRegionRgnData (GDIPCONST BYTE *regionData, INT size, GpRegion **region)
{
	GpRegion *result;
	RegionHeader header;
	DWORD type;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!region || !regionData || size < 0)
		return InvalidParameter;
	
	/* Read and validate the region data header. */
	if (size < sizeof (RegionHeader))
		return GenericError;

	memcpy (&header, regionData, sizeof (RegionHeader));
	if (header.size < 8 || header.checksum != gdip_crc32 (regionData + 8, size - 8) || (header.magic & 0xfffff000) != 0xdbc01000) {
		return GenericError;
	}

	regionData += sizeof (RegionHeader);
	size -= sizeof (RegionHeader);
	
	/* Now read the rest of the data. */
	result = gdip_region_new ();
	if (!result)
		return OutOfMemory;

	result->cnt = 0;
	result->rects = NULL;
	result->tree = NULL;
	result->bitmap = NULL;

	// Read the type.
	memcpy (&type, regionData, sizeof (DWORD));
	regionData += sizeof (DWORD);
	size -= sizeof (DWORD);

	switch (type) {
	case RegionDataRect:
		result->type = RegionTypeRect;
		if (header.size < sizeof (DWORD) * 3 + sizeof (GpRectF)) {
			GdipFree (result);
			return GenericError;
		}

		GpRectF rect;
		memcpy (&rect, regionData, sizeof (GpRectF));
		gdip_add_rect_to_array (&result->rects, &result->cnt, NULL, &rect);

		break;
	case RegionDataPath:
		result->type = RegionTypePath;
		if (size < 16) {
			GdipFree (result);
			return InvalidParameter;
		}

		result->tree = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!result->tree) {
			GdipFree (result);
			return OutOfMemory;
		}

		if (!gdip_region_deserialize_tree ((BYTE *) regionData, size, result->tree)) {
			GdipFree (result);
			return InvalidParameter;
		}
		break;
	case RegionDataEmptyRect: {
		result->type = RegionTypeRect;
		
		break;
	}
	case RegionDataInfiniteRect: {
		result->type = RegionTypeInfinite;

		GpRectF rect = {REGION_INFINITE_POSITION, REGION_INFINITE_POSITION, REGION_INFINITE_LENGTH, REGION_INFINITE_LENGTH};
		gdip_add_rect_to_array (&result->rects, &result->cnt, NULL, &rect);
		
		break;
	}
	default:
		g_warning ("unknown type 0x%08X", result->type);
		GdipFree (result);
		return NotImplemented;
	}

	*region = result;
	return Ok;
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCloneRegion (GpRegion *region, GpRegion **cloneRegion)
{
	GpRegion *result;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!region || !cloneRegion)
		return InvalidParameter;

	result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	if (!result)
		return OutOfMemory;

	status = gdip_copy_region (region, result);
	if (status != Ok) {
		GdipFree (result);
		return status;
	}

	*cloneRegion = result;
	return Ok;
}

GpStatus WINGDIPAPI
GdipDeleteRegion (GpRegion *region)
{
	if (!region)
		return InvalidParameter;

	gdip_clear_region (region);
	GdipFree (region);

	return Ok;
}


GpStatus WINGDIPAPI
GdipSetInfinite (GpRegion *region)
{
	GpRectF rect;

	if (!region)
		return InvalidParameter;

	gdip_clear_region (region);
	region->type = RegionTypeInfinite;

	rect.X = rect.Y = REGION_INFINITE_POSITION;
	rect.Width = rect.Height = REGION_INFINITE_LENGTH;

	return gdip_add_rect_to_array (&region->rects, &region->cnt, NULL, &rect);
}


GpStatus WINGDIPAPI
GdipSetEmpty (GpRegion *region)
{
	if (!region)
		return InvalidParameter;

	gdip_clear_region (region);
	region->type = RegionTypeRect;

	return Ok;
}

/* Exclude */
static GpStatus
gdip_combine_exclude (GpRegion *region, GpRectF *rtrg, int cntt)
{
	GpRectF *allsrcrects = NULL, *rects = NULL;
	GpRectF *alltrgrects = NULL, *rect, *rectop, *recttrg;
	int allsrccnt = 0, allsrccap, cnt = 0, cap, i, n, alltrgcnt = 0, alltrgcap;
	GpRectF current, rslt, newrect;
	BOOL storecomplete;
	GpStatus status;

	/* Create the list of source rectangles to process, it will contain splitted ones later */
	allsrccap = region->cnt * 2;
	cap = allsrccap;
	for (i = 0, rect = region->rects; i < region->cnt; i++, rect++) {
		status = gdip_add_rect_to_array (&allsrcrects, &allsrccnt, &allsrccap, rect);
		if (status != Ok) {
			if (allsrcrects) {
				GdipFree (allsrcrects);
			}

			return status;
		}
	}

	/* Create the list of target rectangles to process, it will contain splitted ones later */
	alltrgcap = cntt;
	for (i = 0, rect = rtrg; i < cntt; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		status = gdip_add_rect_to_array (&alltrgrects, &alltrgcnt, &alltrgcap, &normal);
		if (status != Ok) {
			if (alltrgrects) {
				GdipFree (alltrgrects);
			}
			if (allsrcrects) {
				GdipFree(allsrcrects);
			}
			return status;
		}
	}

	/* Init current with the first element in the array */
	current.X = REGION_INFINITE_POSITION - 1;
	current.Y = REGION_INFINITE_POSITION - 1;
	current.Width = 0; current.Height = 0;

	while (gdip_getlowestrect (allsrcrects, allsrccnt, &current, &rslt)) {
		current.X = rslt.X; current.Y = rslt.Y;
		current.Width = rslt.Width; current.Height = rslt.Height;
		storecomplete = TRUE;

		/* Current rect with lowest y and X against the target ones */
		for (i = 0, recttrg = alltrgrects; i < alltrgcnt; i++, recttrg++) {

			if (gdip_intersects (&current, recttrg) == FALSE
				|| gdip_equals (&current, recttrg) == TRUE ||
				recttrg->Height < 0 || recttrg->Width < 0) {
				continue;
			}

			/* Once a rect is splitted, we do not want to take into account anymore */
			for (rectop = allsrcrects, n = 0; n < allsrccnt; n++, rectop++) {
				if (gdip_equals (&current, rectop)) {
					rectop->X = 0; rectop->Y = 0;
					rectop->Width = 0; rectop->Height = 0;
					break;
				}
			}

			/* Result rect */
			newrect.Y = current.Y;
			if (current.Y >= recttrg->Y) {  /* Our rect intersects in the upper part with another rect */
				newrect.Height = MIN (recttrg->Y + recttrg->Height - current.Y, current.Height);
				if (newrect.Height < 0)
					newrect.Height = current.Height;

				if (current.X >= recttrg->X) { /* Hit from behind */
					newrect.X = recttrg->X + recttrg->Width;
					newrect.Width = MAX (current.X + current.Width - newrect.X, 0);
				}
				else {
					newrect.X = current.X;
					newrect.Width = MAX (recttrg->X - current.X, 0);
				}
			}
			else {
				newrect.Height = MIN (recttrg->Y - current.Y, current.Height);
				newrect.X = current.X;
				newrect.Width = current.Width;
			}

			gdip_add_rect_to_array_notcontained (&rects, &cnt, &cap, &newrect);

			/* What's left to process from the source region */
			if (current.Y >= recttrg->Y) {  /* Our rect intersects in the upper part with another rect */
				/* A whole part from the top has been taken*/
				if (recttrg->X <= current.X && recttrg->X + recttrg->Width  >= current.X + current.Width)
					rslt.Y = recttrg->Y + recttrg->Height;
				else
					rslt.Y = newrect.Y + newrect.Height;

				rslt.Height = current.Y  + current.Height - rslt.Y;
			}
			else {
				rslt.Y = recttrg->Y;
				rslt.Height = current.Y + current.Height - recttrg->Y;
			}

			rslt.X = current.X;
			rslt.Width = current.Width;

			if (rslt.Height > 0 && rslt.Width > 0) {
				status = gdip_add_rect_to_array (&allsrcrects, &allsrccnt, &allsrccap, &rslt);
				if (status != Ok) {
					GdipFree (allsrcrects);
					GdipFree (alltrgrects);

					return status;
				}
			}

			/* Special case where our rect is hit and split in two parts IIIUIII */
			if (recttrg->X >= current.X && recttrg->X + recttrg->Width  <= current.X + current.Width) {
				/* Generate extra right rect, keep previous values of Y and Height */
				newrect.Width = current.X + current.Width - (recttrg->X + recttrg->Width);
				newrect.X = recttrg->X + recttrg->Width;
				gdip_add_rect_to_array_notcontained (&rects, &cnt, &cap, &newrect);
			}

			storecomplete = FALSE;
			break;
		}

		/* don't include a rectangle identical to the excluded one! */
		if (storecomplete && !gdip_equals (rtrg, &current)) {
			gdip_add_rect_to_array_notcontained (&rects, &cnt, &cap, &current);
		}
	}

	gdip_trim_rect_array (&rects, cnt);

	GdipFree (allsrcrects);
	GdipFree (alltrgrects);
	if (region->rects)
		GdipFree (region->rects);

	region->rects = rects;
	region->cnt = cnt;

	return Ok;
}


/*
	Complement: the part of the second region not shared with the first region.
	Scans the region to be combined and store the rects not present in the region
*/
static GpStatus
gdip_combine_complement (GpRegion *region, GpRectF *rtrg, int cntt)
{
	GpRegion regsrc;
	GpRectF* trg, *rect;
	GpRectF* allsrcrects = NULL;
	int allsrccnt = 0, i,  trgcnt, allsrccap;
	GpStatus status;

	/* Create the list of source rectangles to process */
	allsrccap = cntt;
	for (i = 0, rect = rtrg; i < cntt; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		status = gdip_add_rect_to_array (&allsrcrects, &allsrccnt, &allsrccap, &normal);
		if (status != Ok) {
			goto error;
		}
	}

	regsrc.rects = allsrcrects;
	regsrc.cnt = allsrccnt;
	trg = region->rects;
	trgcnt = region->cnt;

	status = gdip_combine_exclude (&regsrc, trg, trgcnt);
	if (status != Ok) {
		goto error;
	}

	if ((regsrc.rects != allsrcrects) || (regsrc.cnt != allsrccnt)) {
		if (region->rects)
			GdipFree (region->rects);

		region->rects = regsrc.rects;
		region->cnt = regsrc.cnt;
	}

	return Ok;

error:
	if (allsrcrects)
		GdipFree (allsrcrects);
	
	return status;
}


/* Union */
static GpStatus
gdip_combine_union (GpRegion *region, GpRectF *rtrg, int cnttrg)
{
	GpRectF *allrects = NULL, *rects = NULL;
	GpRectF *recttrg, *rect, *rectop, *current;
	int allcnt = 0, allcap, cnt = 0, cap = 0, currentIndex = -1, i, n;
	GpRectF rslt, newrect;
	BOOL storecomplete, contained, needsort;
	GpStatus status;

	/* All the src and trg rects in a single array*/
	allcap = (region->cnt + cnttrg) * 2;
	cap = allcap;
	for (i = 0, rect = region->rects; i < region->cnt; i++, rect++) {
		status = gdip_add_rect_to_array (&allrects, &allcnt, &allcap, rect);
		if (status != Ok) {
			if (allrects)
				GdipFree (allrects);

			return status;
		}
	}

	for (i = 0, rect = rtrg; i < cnttrg; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		gdip_add_rect_to_array (&allrects, &allcnt, &allcap, &normal);
	}

	if (allcnt == 0) {
		GdipFree (allrects);
		return Ok;
	}

	gdip_sort_rect_array(allrects, allcnt);

	for (currentIndex = 0; currentIndex < allcnt; currentIndex++) {
		current = allrects + currentIndex;

		if (current->Width <= 0 || current->Height <= 0) {
			continue;
		}

		storecomplete = TRUE;

		/* Current rect with lowest y and X againt the stored ones */
		for (i = currentIndex + 1; i < allcnt; i++) {
			recttrg = allrects + i;

			needsort = FALSE;

			// If it is positioned after the bottom-right corner of current, no useful rectangles can be found (due to sorting).
			if (recttrg->Y > current->Y + current->Height ||
				(recttrg->Y == current->Y + current->Height && recttrg->X > current->X + current->Width)) {
				break;
			}

			/* If it has lower coordinates or negative / zero size it has been already processed */
			if (recttrg->Height <= 0 || recttrg->Width <= 0 ||
				current->Y > recttrg->Y ||
				(current->Y == recttrg->Y && current->X > recttrg->X)) {
				continue;
			}

			if (gdip_intersects_or_touches (current, recttrg) == FALSE
				|| gdip_equals (current, recttrg) == TRUE) {
				continue;
			}

			if (gdip_contains  (recttrg, current) == TRUE) {
				continue;
			}

			/* Our rect intersects in the lower part with another rect */
			newrect.Y = current->Y;
			newrect.X = current->X;
			if (current->Y == recttrg->Y) {
				newrect.Width = MAX (current->X + current->Width, recttrg->X + recttrg->Width) - newrect.X;
				newrect.Height = MIN (current->Height, recttrg->Height);
			}
			else {
				newrect.Width = current->Width;
				newrect.Height = recttrg->Y - current->Y;
			}

			/* If it's contained inside, get the > height */
			if (recttrg->X == current->X && (recttrg->Width == current->Width ||
				(recttrg->Y == current->Y && recttrg->Width > current->Width))) {

				newrect.Height = recttrg->Y + recttrg->Height - current->Y;
			} else if (recttrg->X >= current->X && recttrg->X + recttrg->Width <= current->X + current->Width) {
				newrect.Height = current->Height;
			}

			gdip_add_rect_to_array_notcontained (&rects, &cnt, &cap, &newrect);

			/* Push what's left from the current the rect in the list of rects to process
			 if it's already not contained in other rects except the current (we just split from there) */
			rslt.X = current->X;
			rslt.Y = newrect.Y + newrect.Height;
			rslt.Width = current->Width;
			rslt.Height = current->Height - newrect.Height;

			if (rslt.Height > 0 && rslt.Width > 0) {
				contained = FALSE;
				for (rectop = allrects + currentIndex + 1, n = currentIndex + 1; n < allcnt; n++, rectop++) {
					// Rectangles before currentIndex have been processed and will be empty. They cannot contain anything.
					if (gdip_contains (&rslt, rectop)) {
						contained = TRUE;
						break;
					} else if (gdip_compare_rectf (rectop, &rslt) > 0) {
						break; // Not going to find one containing it after this.
					}
				}

				if (contained == FALSE) {
					status = gdip_add_rect_to_array_sorted (&allrects, &allcnt, &allcap,  &rslt);
					if (status != Ok) {
						GdipFree (allrects);
						return status;
					}

					// Must get recttrg in the new array in case adding rslt above had to increase the array capacity.
					recttrg = allrects + i;
				}
			}

			/* If both we at the same Y when take into account the X also to process the following
			   that exceeds the X also */
			if (recttrg->Y == current->Y) {
				recttrg->Height -= newrect.Height;
				if (recttrg->Height > 0) {
					recttrg->Y += newrect.Height;
					needsort = TRUE; // Modified Y, re-sort.
				}
			} else if (recttrg->X >= current->X && recttrg->X + recttrg->Width <= current->X + current->Width) {
				/* If it's contained inside, get the > height  */
				recttrg->Height = recttrg->Y + recttrg->Height - (newrect.Y + newrect.Height);
				if (recttrg->Height > 0) {
					recttrg->Y = newrect.Y + newrect.Height;
					needsort = TRUE; // Modified Y, re-sort.
				}
			}

			if (needsort == TRUE)
				gdip_sort_rect_array_sorted (allrects, allcnt);

			storecomplete = FALSE;
			break;
		}

		if (storecomplete) {
			gdip_add_rect_to_array_notcontained (&rects, &cnt, &cap, current);
		}
	}

	GdipFree (allrects);
	if (region->rects)
		GdipFree (region->rects);

	gdip_trim_rect_array (&rects, cnt);
	region->rects = rects;
	region->cnt = cnt;

	return Ok;
}

/* Intersect */
static GpStatus
gdip_combine_intersect (GpRegion *region, GpRectF *rtrg, int cnttrg)
{
	GpRectF *rectsrc;
	int src, trg;
	GpRectF rectcur;
	GpRegion regunion;
	GpRectF *recttrg;
	GpStatus status;

	regunion.rects = NULL;
	regunion.cnt = 0;

	for (rectsrc = region->rects, src = 0; src < region->cnt; src++, rectsrc++) {
		for (recttrg = rtrg, trg = 0; trg < cnttrg; trg++, recttrg++) {
			/* normalize */
			GpRectF normal;
			gdip_normalize_rectangle (recttrg, &normal);

			/* Intersects With */
			if ((rectsrc->X >= normal.X + normal.Width) || (rectsrc->X + rectsrc->Width <= normal.X) ||
				(rectsrc->Y >= normal.Y + normal.Height) || (rectsrc->Y + rectsrc->Height <= normal.Y)) {
				continue;
			}
			/* Area that intersects */
			rectcur.X = rectsrc->X > normal.X ? rectsrc->X : normal.X;
			rectcur.Y = rectsrc->Y > normal.Y ? rectsrc->Y : normal.Y;
			rectcur.Width = rectsrc->X + rectsrc->Width < normal.X + normal.Width ?
				rectsrc->X + rectsrc->Width - rectcur.X : normal.X + normal.Width - rectcur.X;

			rectcur.Height = rectsrc->Y + rectsrc->Height < normal.Y + normal.Height ?
				rectsrc->Y + rectsrc->Height - rectcur.Y : normal.Y + normal.Height - rectcur.Y;

			/* Combine with previous areas that intersect with rect */
			status = gdip_combine_union (&regunion, &rectcur, 1);
			if (status != Ok)
				return status;
		}
	}

	if (region->rects)
		GdipFree (region->rects);

	region->rects = regunion.rects;
	region->cnt = regunion.cnt;

	return Ok;
}

/* Xor */
static GpStatus
gdip_combine_xor (GpRegion *region, GpRectF *recttrg, int cnttrg)
{
	GpRegion *rgnsrc = NULL;  /* All rectangles of both regions*/
	GpRegion *rgntrg = NULL;  /* Only the ones that intersect*/
	GpRectF *allrects = NULL, *rect;
	int allcnt = 0, allcap, i;
	GpStatus status;

	/* All the src and trg rects in a single array*/
	allcap = region->cnt + cnttrg;
	for (i = 0, rect = region->rects; i < region->cnt; i++, rect++) {
		status = gdip_add_rect_to_array (&allrects, &allcnt, &allcap, rect);
		if (status != Ok)
			goto error;
	}

	for (i = 0, rect = recttrg; i < cnttrg; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		gdip_add_rect_to_array (&allrects, &allcnt, &allcap, &normal);
	}

	rgnsrc = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	if (!rgnsrc) {
		status = OutOfMemory;
		goto error;
	}

	rgnsrc->type = RegionTypeRect;
	rgnsrc->cnt = allcnt;
	rgnsrc->rects = allrects;

	status = GdipCloneRegion (region, &rgntrg);
	if (status != Ok)
		goto error;

	status = gdip_combine_intersect (rgntrg, recttrg, cnttrg);
	if (status != Ok)
		goto error;

	/* exclude the intersecting rectangles (if any) */
	if (rgntrg->cnt > 0) {
		status = gdip_combine_exclude (rgnsrc, rgntrg->rects, rgntrg->cnt);
		if (status != Ok)
			goto error;
	}

	if (region->rects)
		GdipFree (region->rects);

	region->rects = rgnsrc->rects;
	region->cnt = rgnsrc->cnt;

	GdipFree (rgnsrc);
	GdipDeleteRegion (rgntrg);

	return Ok;

error:
	if (allrects)
		GdipFree (allrects);

	GdipFree (rgnsrc);
	GdipDeleteRegion (rgntrg);

	return status;
}

GpStatus WINGDIPAPI
GdipCombineRegionRect (GpRegion *region, GDIPCONST GpRectF *rect, CombineMode combineMode)
{
	if (!region || !rect)
		return InvalidParameter;

	if (combineMode == CombineModeReplace) {
		GdipSetEmpty (region);
		return gdip_add_rect_to_array (&region->rects, &region->cnt, NULL, (GpRectF *)rect);
	}

	GpRectF normalized;
	gdip_normalize_rectangle (rect, &normalized);

	BOOL infinite = gdip_is_InfiniteRegion (region);
	BOOL empty = gdip_is_region_empty (region, /* allowNegative */ TRUE);
	BOOL rectEmpty = gdip_is_rectF_empty (&normalized, /* allowNegative */ FALSE);

	if (rectEmpty) {
		switch (combineMode) {
		case CombineModeUnion:
		case CombineModeXor:
		case CombineModeExclude:
			/* The union of the empty region and X is X */
			/* The xor of the empty region and X is X */
			/* Everything is outside the empty region */
			if (empty)
				return GdipSetEmpty (region);
			if (infinite)
				return GdipSetInfinite (region);

			return Ok;
		case CombineModeIntersect:
		case CombineModeComplement:
			/* The empty region does not intersect with anything */
			/* Nothing is inside the empty region */
			return GdipSetEmpty (region);
		default:
			break;
		}
	}

	if (infinite) {
		switch (combineMode) {
		case CombineModeIntersect: {
			/* The intersection of the infinite region with X is X */
			GdipSetEmpty (region);
			return gdip_add_rect_to_array (&region->rects, &region->cnt, NULL, &normalized);
		}
		case CombineModeUnion:
			/* The union of the infinite region and X is the infinite region */
			return GdipSetInfinite (region);
		case CombineModeComplement:
			/* Nothing is outside the infinite region */
			return GdipSetEmpty (region);
		default:
			break;
		}
	} else if (empty) {
		switch (combineMode) {
		case CombineModeIntersect:
		case CombineModeExclude:
			/* The empty region does not intersect with anything */
			/* Nothing to exclude */
			return GdipSetEmpty (region);
		case CombineModeUnion:
		case CombineModeXor:
		case CombineModeComplement:
			/* The union of the empty region and X is X */
			/* The XOR of the empty region and X is X */
			/* Everything is outside the empty region */
			GdipSetEmpty (region);
			return gdip_add_rect_to_array (&region->rects, &region->cnt, NULL, &normalized);
		default:
			break;
		}
	}

	switch (region->type) {
	case RegionTypeRect:
	case RegionTypeInfinite: {
		region->type = RegionTypeRect;
		switch (combineMode) {
		case CombineModeExclude:
			return gdip_combine_exclude (region, &normalized, 1);
		case CombineModeComplement:
			return gdip_combine_complement (region, &normalized, 1);
		case CombineModeIntersect:
			return gdip_combine_intersect (region, &normalized, 1);
		case CombineModeUnion:
			return gdip_combine_union (region, &normalized, 1);
		case CombineModeXor:
			return gdip_combine_xor (region, &normalized, 1);
		case CombineModeReplace: /* Used by Graphics clipping */
			return gdip_add_rect_to_array (&region->rects, &region->cnt, NULL, &normalized);
		default:
			return NotImplemented;
		}
	}
	case RegionTypePath: {
		/* Convert GpRectF to GpPath and use GdipCombineRegionPath */
		GpPath *path;
		GpStatus status = GdipCreatePath (FillModeAlternate, &path);
		if (status != Ok)
			return status;

		status = GdipAddPathRectangle (path, normalized.X, normalized.Y, normalized.Width, normalized.Height);
		if (status != Ok) {
			GdipDeletePath (path);
			return status;
		}

		status = GdipCombineRegionPath (region, path, combineMode);
		GdipDeletePath (path);
		return status;
	}
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}
}


GpStatus WINGDIPAPI
GdipCombineRegionRectI (GpRegion *region, GDIPCONST GpRect *recti, CombineMode combineMode)
{
	GpRectF rect;

	if (!region || !recti)
		return InvalidParameter;

	gdip_RectF_from_Rect ((GpRect *) recti, &rect);

	return GdipCombineRegionRect (region, (GDIPCONST GpRectF *) &rect, combineMode);
}

/* Exclude path from infinite region */
static BOOL
gdip_combine_exclude_from_infinite (GpRegion *region, GpPath *path)
{
	/*
	 * We combine the path with the infinite region's, then reverse it.
	 */
	GpPath *region_path;
	GpStatus status;
	
	if (path->count == 0)
		return TRUE;

	if (region->type != RegionTypePath) {
		status = gdip_region_convert_to_path (region);
		if (status != Ok)
			return FALSE;
	}
	
	g_assert (region->tree->path);
	region_path = region->tree->path;
	status = GdipClonePath (path, &region->tree->path);
	if (status != Ok) {
		region->tree->path = region_path;
		return FALSE;
	}
	status = GdipAddPathPath (region->tree->path, region_path, FALSE);
	if (status != Ok) {
		GdipDeletePath (region->tree->path);
		region->tree->path = region_path;
		return FALSE;
	}
	status = GdipReversePath (region->tree->path);
	if (status != Ok) {
		GdipDeletePath (region->tree->path);
		region->tree->path = region_path;
		return FALSE;
	}
	return TRUE;
}

GpStatus WINGDIPAPI
GdipCombineRegionPath (GpRegion *region, GpPath *path, CombineMode combineMode)
{
	GpRegionBitmap *path_bitmap, *result;
	GpStatus status;

	if (!region || !path)
		return InvalidParameter;

	if (combineMode == CombineModeReplace) {
		gdip_clear_region (region);
		return gdip_region_create_from_path (region, path);
	}
	
	BOOL infinite = gdip_is_InfiniteRegion (region);
	BOOL empty = gdip_is_region_empty (region, /* allowNegative */ TRUE);
	BOOL pathEmpty = path->count == 0;

	if (pathEmpty) {
		switch (combineMode) {
		case CombineModeUnion:
		case CombineModeXor:
		case CombineModeExclude:
			/* The union of the empty region and X is X */
			/* The xor of the empty region and X is X */
			/* Everything is outside the empty region */
			if (empty)
				return GdipSetEmpty (region);

			return Ok;
		case CombineModeIntersect:
		case CombineModeComplement:
			/* The empty region does not intersect with anything */
			/* Nothing is inside the empty region */
			return GdipSetEmpty (region);
		default:
			break;
		}
	}

	if (infinite) {
		switch (combineMode) {
		case CombineModeIntersect:
			/* The intersection of the infinite region with X is X */
			GdipSetEmpty (region);
			return gdip_region_create_from_path (region, path);
		case CombineModeUnion:
			/* The union of the infinite region and X is the infinite region */
			return GdipSetInfinite (region);
		case CombineModeComplement:
			/* Nothing is outside the infinite region */
			return GdipSetEmpty (region);
		case CombineModeExclude:
			if (gdip_combine_exclude_from_infinite (region, path))
				return Ok;

			break;
		default:
			break;
		}
	} else if (empty) {
		switch (combineMode) {
		case CombineModeIntersect:
		case CombineModeExclude:
			/* The empty region does not intersect with anything */
			/* Nothing to exclude */
			return GdipSetEmpty (region);
		case CombineModeUnion:
		case CombineModeXor:
		case CombineModeComplement:
			/* The union of the empty region and X is X */
			/* The XOR of the empty region and X is X */
			/* Everything is outside the empty region */
			GdipSetEmpty (region);
			return gdip_region_create_from_path (region, path);
		default:
			break;
		}
	}

	if (region->type != RegionTypePath) {
		status = gdip_region_convert_to_path (region);
		if (status != Ok)
			return status;
	}

	/* make sure the region's bitmap is available */
	gdip_region_bitmap_ensure (region);
	if (!region->bitmap)
		return OutOfMemory;

	/* create a bitmap for the path to combine into the region */
	path_bitmap = gdip_region_bitmap_from_path (path);

	result = gdip_region_bitmap_combine (region->bitmap, path_bitmap, combineMode);
	gdip_region_bitmap_free (path_bitmap);
	if (!result)
		return NotImplemented;

	gdip_region_bitmap_free (region->bitmap);
	region->bitmap = result;

	/* add a copy of path into region1 tree */
	if (region->tree->path) {
		/* move the existing path into a new tree (branch1) ... */
		region->tree->branch1 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!region->tree->branch1)
			return OutOfMemory;

		region->tree->branch1->path = region->tree->path;
		region->tree->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!region->tree->branch2)
			return OutOfMemory;
	} else {
		/* move the current base tree into branch1 of a new tree ... */
		GpPathTree* tmp = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!tmp)
			return OutOfMemory;

		tmp->branch1 = region->tree;
		tmp->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!tmp->branch2) {
			GdipFree (tmp);
			return OutOfMemory;
		}

		region->tree = tmp;
	}
	/* ... and clone the specified path into branch2 */
	region->tree->mode = combineMode;
	region->tree->path = NULL;

	return GdipClonePath (path, &region->tree->branch2->path);
}


static GpStatus
gdip_combine_pathbased_region (GpRegion *region1, GpRegion *region2, CombineMode combineMode)
{
	GpRegionBitmap *result;

	/* if not available, construct the bitmaps for both regions */
	gdip_region_bitmap_ensure (region1);
	gdip_region_bitmap_ensure (region2);
	if (!region1->bitmap || !region2->bitmap)
		return OutOfMemory;

	result = gdip_region_bitmap_combine (region1->bitmap, region2->bitmap, combineMode);
	if (!result)
		return NotImplemented;
	gdip_region_bitmap_free (region1->bitmap);
	region1->bitmap = result;

	/* re-structure region1 to allow adding a copy of region2 inside it */
	if (region1->tree->path) {
		region1->tree->branch1 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!region1->tree->branch1)
			return OutOfMemory;

		region1->tree->branch1->path = region1->tree->path;
		region1->tree->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!region1->tree->branch2)
			return OutOfMemory;
	} else {
		/* move the current base tree into branch1 of a new tree ... */
		GpPathTree* tmp = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!tmp)
			return OutOfMemory;

		tmp->branch1 = region1->tree;
		tmp->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!tmp->branch2) {
			GdipFree (tmp);
			return OutOfMemory;
		}

		region1->tree = tmp;
	}

	region1->tree->mode = combineMode;
	region1->tree->path = NULL;

	/* add a copy of region2 tree into region1 tree */
	if (region2->tree->path) {
		return GdipClonePath (region2->tree->path, &region1->tree->branch2->path);
	} else {
		return gdip_region_copy_tree (region2->tree, region1->tree->branch2);
	}
}


GpStatus WINGDIPAPI
GdipCombineRegionRegion (GpRegion *region, GpRegion *region2, CombineMode combineMode)
{
	GpStatus status;

	if (!region || !region2)
		return InvalidParameter;

	if (combineMode == CombineModeReplace) {
		GdipSetEmpty (region);
		return gdip_copy_region (region2, region);
	}

	BOOL region1Empty = gdip_is_region_empty (region, /* allowNegative */ TRUE);
	BOOL region1Infinite = gdip_is_InfiniteRegion (region);
	BOOL region2Empty = gdip_is_region_empty (region2, /* allowNegative */ combineMode != CombineModeIntersect || region->type != RegionTypeInfinite);
	BOOL region2Infinite = gdip_is_InfiniteRegion (region2);

	switch (combineMode) {
	case CombineModeUnion:
		if (region1Infinite || region2Infinite) {
			/* The union of X with the infinite region is infinite */
			return GdipSetInfinite (region);
		}
		if (region1Empty) {
			/* The union of the empty region and X is X */
			GdipSetEmpty (region);
			if (!region2Empty)
				return gdip_copy_region (region2, region);
			
			return Ok;
		}
		if (region2Empty) {
			/* The union of the empty region and X is X */
			return Ok;
		}
		
		break;
	case CombineModeIntersect:
		if (region1Empty || region2Empty) {
			/* Nothing intersects with the empty region */
			return GdipSetEmpty (region);
		}
		if (region1Infinite) {
			/* Everything intersects with the infinite region */
			GdipSetEmpty (region);
			return gdip_copy_region (region2, region);
		}
		if (region2Infinite) {
			/* Everything intersects with the infinite region */
			return Ok;
		}

		break;
	case CombineModeExclude:
		if (region1Empty) {
			/* Nothing is outside the empty region */
			return GdipSetEmpty (region);
		}
		if (region2Empty) {
			/* Everything is outside the empty region */
			return Ok;
		}
		if (region1Infinite) {
			if ((region2->type == RegionTypePath) && region2->tree && region2->tree->path &&
				gdip_combine_exclude_from_infinite (region, region2->tree->path))
				return Ok;
		}

		break;
	case CombineModeXor:
		if (region2Empty) {
			/* The XOR of the empty region and X is X */
			if (region1Empty) {
				return GdipSetEmpty (region);
			}

			return Ok;
		}
		if (region1Empty) {
			/* The XOR of the empty region and X is X */
			GdipSetEmpty (region);
			return gdip_copy_region (region2, region);
		}
		if (region1Infinite && region2Infinite) {
			/* The XOR of the infinite region and the infinite region is X */
			return GdipSetEmpty (region);
		}

		break;
	case CombineModeComplement:
		if (region1Infinite || region2Empty) {
			/* Nothing is outside the infinite region */
			/* Nothing is inside the empty region */
			return GdipSetEmpty (region);
		}
		if (region1Empty) {
			/* Anything is outside of the empty region */
			if (region2Infinite) {
				return GdipSetInfinite (region);
			}

			GdipSetEmpty (region);
			return gdip_copy_region (region2, region);
		}

		break;
	default:
		break;
	}

	if (region->type == RegionTypePath) {
		status = gdip_region_convert_to_path (region2);
		if (status != Ok)
			return status;

		return gdip_combine_pathbased_region (region, region2, combineMode);
	} else if (region2->type == RegionTypePath) {
		status = gdip_region_convert_to_path (region);
		if (status != Ok)
			return status;

		return gdip_combine_pathbased_region (region, region2, combineMode);
	}

	/* at this stage we are sure that BOTH region and region2 are rectangle 
	 * based, so we can use the old rectangle-based code to combine regions
	 */
	region->type = RegionTypeRect;
	switch (combineMode) {
	case CombineModeExclude:
		return gdip_combine_exclude (region, region2->rects, region2->cnt);
	case CombineModeComplement:
		return gdip_combine_complement (region, region2->rects, region2->cnt);
	case CombineModeIntersect:
		return gdip_combine_intersect (region, region2->rects, region2->cnt);
	case CombineModeUnion:
		return gdip_combine_union (region, region2->rects, region2->cnt);
	case CombineModeXor:
		return gdip_combine_xor (region, region2->rects, region2->cnt);
	default:
		return NotImplemented;
	}
}

GpStatus WINGDIPAPI
GdipGetRegionBounds (GpRegion *region, GpGraphics *graphics, GpRectF *rect)
{
	if (!region || !graphics || !rect)
		return InvalidParameter;

	switch (region->type) {
	case RegionTypeRect:
	case RegionTypeInfinite:
		gdip_get_bounds (region->rects , region->cnt, rect);
		break;
	case RegionTypePath: {
		GpRect bounds;

		/* optimisation for simple path */
		if (region->tree->path)
			return GdipGetPathWorldBounds (region->tree->path, rect, NULL, NULL);

		gdip_region_bitmap_ensure (region);
		if (!region->bitmap)
			return OutOfMemory;

		/* base the bounds on the reduced bitmap of the paths */
		gdip_region_bitmap_get_smallest_rect (region->bitmap, &bounds);

		/* small loss of precision when converting the bitmap coord to float */
		rect->X = bounds.X;
		rect->Y = bounds.Y;
		rect->Width = bounds.Width;
		rect->Height = bounds.Height;
		break;
	}
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}

	return Ok;
}


GpStatus WINGDIPAPI
GdipIsEmptyRegion (GpRegion *region, GpGraphics *graphics, BOOL *result)
{
	if (!region || !graphics || !result)
		return InvalidParameter;

	*result = gdip_is_region_empty (region, /* allowNegative */ TRUE);
	return Ok;
}


GpStatus WINGDIPAPI
GdipIsInfiniteRegion (GpRegion *region, GpGraphics *graphics, BOOL *result)
{
	if (!region || !graphics || !result)
		return InvalidParameter;

	*result = gdip_is_InfiniteRegion (region);
	return Ok;
}


GpStatus WINGDIPAPI
GdipIsVisibleRegionPoint (GpRegion *region, float x, float y, GpGraphics *graphics, BOOL *result)
{
	if (!region || !result)
		return InvalidParameter;

	switch (region->type) {
	case RegionTypeRect:
	case RegionTypeInfinite:
		*result = gdip_is_Point_in_RectFs_Visible (x, y, region->rects, region->cnt);
		break;
	case RegionTypePath:
		gdip_region_bitmap_ensure (region);
		g_assert (region->bitmap);

		*result = gdip_region_bitmap_is_point_visible (region->bitmap, x, y);
		break;
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}

	return Ok;
}


GpStatus WINGDIPAPI
GdipIsVisibleRegionPointI (GpRegion *region, int x, int y, GpGraphics *graphics, BOOL *result)
{
	return GdipIsVisibleRegionPoint (region, x, y, graphics, result);
}


GpStatus WINGDIPAPI
GdipIsVisibleRegionRect (GpRegion *region, float x, float y, float width, float height, GpGraphics *graphics, BOOL *result)
{
	if (!region || !result)
		return InvalidParameter;

	if (width == 0 || height == 0) {
		*result = FALSE;
		return Ok;
	}

	switch (region->type) {
	case RegionTypeRect:
	case RegionTypeInfinite:
		*result = gdip_is_Rect_in_RectFs_Visible (x, y, width, height, region->rects, region->cnt);
		break;
	case RegionTypePath: {
		GpRect rect = {x, y, width, height};

		gdip_region_bitmap_ensure (region);
		g_assert (region->bitmap);

		*result = gdip_region_bitmap_is_rect_visible (region->bitmap, &rect);
		break;
	}
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}

	return Ok;
}


GpStatus WINGDIPAPI
GdipIsVisibleRegionRectI (GpRegion *region, int x, int y, int width, int height, GpGraphics *graphics, BOOL *result)
{
	return GdipIsVisibleRegionRect (region, x, y, width, height, graphics, result);
}


static GpStatus
get_transformed_region (GpRegion *region, GpMatrix *matrix, GpRegion **result)
{
	GpStatus status;
	GpRegion *work;

	if (gdip_is_matrix_empty (matrix)) {
		*result = region;
		return Ok;
	}

	/* The matrix doesn't affect the original region - only the result */
	status = GdipCloneRegion (region, &work);
	if (status != Ok)
		return status;

	/* If required convert into a path-based region */
	if (work->type != RegionTypePath) {
		status = gdip_region_convert_to_path (work);
		if (status != Ok) {
			GdipDeleteRegion (work);
			return status;
		}
	}

	/* Transform all the paths */
	status = gdip_region_transform_tree (work->tree, matrix);
	if (status != Ok) {
		GdipDeleteRegion (work);
		return status;
	}

	/* Any existing bitmap has been invalidated */
	gdip_region_bitmap_invalidate (work);

	*result = work;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetRegionScansCount (GpRegion *region, UINT *count, GpMatrix *matrix)
{
	GpStatus status;
	INT countResult;

	if (!region || !matrix || !count)
		return InvalidParameter;
	
	status = GdipGetRegionScans (region, NULL, &countResult, matrix);
	if (status != Ok)
		return status;

	*count = countResult;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetRegionScans (GpRegion *region, GpRectF* rects, INT* count, GpMatrix* matrix)
{
	GpStatus status;
	GpRegion *work;

	if (!region || !matrix || !count)
		return InvalidParameter;

	status = get_transformed_region (region, matrix, &work);
	if (status != Ok)
		return status;

	if (gdip_is_region_empty (work, /* allowNegative */ TRUE)) {
		*count = 0;
	} else if (gdip_is_InfiniteRegion (work)) {
		if (rects) {
			rects->X = REGION_INFINITE_POSITION;
			rects->Y = REGION_INFINITE_POSITION;
			rects->Width = REGION_INFINITE_LENGTH;
			rects->Height = REGION_INFINITE_LENGTH;
		}

		*count = 1;
	} else {
		switch (work->type) {
		case RegionTypeRect:
			if (rects) {
				for (int i = 0; i < work->cnt; i++) {
					GpRectF rect = work->rects[i];

					INT origX = iround ((rect.X * 16.0f));
					INT origY = iround ((rect.Y * 16.0f));
					INT origMaxX = iround (((rect.Width + rect.X) * 16.0f));
					INT origMaxY = iround (((rect.Height + rect.Y) * 16.0f));

					INT x = (origX + 15) >> 4;
					INT y = (origY + 15) >> 4;
					INT maxX = (origMaxX + 15) >> 4;
					INT maxY = (origMaxY + 15) >> 4;

					rects[i].X = x;
					rects[i].Y = y;
					rects[i].Width = maxX - x;
					rects[i].Height = maxY - y;
				}
			}

			*count = work->cnt;
			break;
		case RegionTypePath:
			/* ensure the bitmap is usable */
			gdip_region_bitmap_ensure (work);
			*count = gdip_region_bitmap_get_scans (work->bitmap, rects);
			break;
		default:
			g_warning ("unknown type 0x%08X", region->type);
			if (work != region)
				GdipDeleteRegion (work);

			return NotImplemented;
		}
	}

	/* Delete the clone */
	if (work != region)
		GdipDeleteRegion (work);
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetRegionScansI (GpRegion *region, GpRect *rects, INT *count, GpMatrix *matrix)
{
	GpStatus status;
	GpRectF *rectsF = NULL;
	UINT scansCount;

	if (!region || !count || !matrix)
		return InvalidParameter;

	if (rects) {
		status = GdipGetRegionScansCount (region, &scansCount, matrix);
		if (status != Ok)
			return status;

		rectsF = malloc (scansCount * sizeof (GpRectF));
		if (!rectsF)
			return OutOfMemory;
	} else {
		rectsF = NULL;
	}

	status = GdipGetRegionScans (region, rectsF, count, matrix);
	if (status != Ok) {
		if (rectsF)
			free(rectsF);
		return status;
	}
		
	if (rects) {
		for (int i = 0; i < scansCount; i++)
			gdip_Rect_from_RectF (&rectsF[i], &rects[i]);
	}

	if (rectsF)
		free(rectsF);
	
	return Ok;
}


GpStatus WINGDIPAPI
GdipIsEqualRegion (GpRegion *region, GpRegion *region2, GpGraphics *graphics, BOOL *result)
{
	int i;
	GpRectF *rectsrc, *recttrg;
	GpStatus status;

	if (!region || !region2 || !graphics || !result)
		return InvalidParameter;

	/* quick case: same pointer == same region == equals */
	if (region == region2) {
		*result = TRUE;
		return Ok;
	}
	
	BOOL region1Infinite = gdip_is_InfiniteRegion (region);
	BOOL region1Empty = gdip_is_region_empty (region, /* allowNegative */ TRUE);
	BOOL region2Infinite = gdip_is_InfiniteRegion (region2);
	BOOL region2Empty = gdip_is_region_empty (region2, /* allowNegative */ TRUE);

	if (region1Infinite || region2Infinite) {
		*result = region1Infinite == region2Infinite;
		return Ok;
	}
	if (region1Empty || region2Empty) {
		*result = region1Empty == region2Empty;
		return Ok;
	}

	if ((region->type == RegionTypePath) || (region2->type == RegionTypePath)) {
		/* if required convert one region to a path based region */
		if (region->type != RegionTypePath) {
			status = gdip_region_convert_to_path (region);
			if (status != Ok)
				return status;
		}

		gdip_region_bitmap_ensure (region);
		g_assert (region->bitmap);

		if (region2->type != RegionTypePath) {
			status = gdip_region_convert_to_path (region2);
			if (status != Ok)
				return status;
		}

		gdip_region_bitmap_ensure (region2);
		g_assert (region2->bitmap);

		*result = gdip_region_bitmap_compare (region->bitmap, region2->bitmap);
		return Ok;
	}

	/* rectangular-based region quality test */
	if (region->cnt != region2->cnt) {
		*result = FALSE;
		return Ok;
	}

	for (i = 0, rectsrc = region->rects, recttrg = region2->rects; i < region->cnt; i++, rectsrc++, recttrg++) {
		if (rectsrc->X != recttrg->X || rectsrc->Y != recttrg->Y ||
				rectsrc->Width != recttrg->Width || rectsrc->Height != recttrg->Height) {
			*result = FALSE;
			return Ok;
		}
	}

	*result = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipTranslateRegion (GpRegion *region, float dx, float dy)
{
	if (!region)
		return InvalidParameter;

	// Infinite regions cannot be transformed.
	if (region->type == RegionTypeInfinite)
		return Ok;

	switch (region->type) {
	case RegionTypeRect: {
		int i;
		GpRectF *rect;
		for (i = 0, rect = region->rects ; i < region->cnt; i++, rect++) {
			rect->X += dx;
			rect->Y += dy;
		}

		break;
	}
	case RegionTypePath:
		gdip_region_translate_tree (region->tree, dx, dy);
		if (region->bitmap) {
			region->bitmap->X += dx;
			region->bitmap->Y += dy;
		}

		break;
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipTranslateRegionI (GpRegion *region, int dx, int dy)
{
	return GdipTranslateRegion (region, dx, dy);
}

/* this call doesn't exists in GDI+ */
static GpStatus
ScaleRegion (GpRegion *region, float sx, float sy)
{
	g_assert (region);
	g_assert (region->type == RegionTypeRect && region->rects);

	for (int i = 0; i < region->cnt; i++) {
		region->rects[i].X *= sx;
		region->rects[i].Y *= sy;
		region->rects[i].Width *= sx;
		region->rects[i].Height *= sy;
	}

	return Ok;
}

GpStatus WINGDIPAPI
GdipTransformRegion (GpRegion *region, GpMatrix *matrix)
{
	GpStatus status = Ok;

	if (!region || !matrix)
		return InvalidParameter;

	// Infinite and empty regions cannot be transformed.
	if (region->type == RegionTypeInfinite || ((region->cnt == 0) && (region->type == RegionTypeRect)))
		return Ok;

	// Nothing to do.
	if (gdip_is_matrix_empty (matrix))
		return Ok;

	BOOL isSimpleMatrix = (matrix->xy == 0) && (matrix->yx == 0);
	BOOL matrixHasTranslate = (matrix->x0 != 0) || (matrix->y0 != 0);
	BOOL matrixHasScale = (matrix->xx != 1) || (matrix->yy != 1);

	/* try to avoid heavy stuff (e.g. conversion to path, invalidating 
	 * bitmap...) if the transform is:
	 * - a translation + scale operations (for rectangle ebased region)
	 * - only to do a scale operation (for a rectangle based region)
	 * - only to do a simple translation (for both rectangular and bitmap based regions)
	 */
	if (region->type == RegionTypeRect) {
		if (isSimpleMatrix) {
			if (matrixHasScale)
				ScaleRegion (region, matrix->xx, matrix->yy);
			if (matrixHasTranslate)
				GdipTranslateRegion (region, matrix->x0, matrix->y0);

			return Ok;
		}
	} else if (isSimpleMatrix && !matrixHasScale) {
		GdipTranslateRegion (region, matrix->x0, matrix->y0);
		return Ok;
	}

	/* most matrix operations would change the rectangles into path so we always preempt this */
	if (region->type != RegionTypePath) {
		status = gdip_region_convert_to_path (region);
		if (status != Ok) {
			gdip_region_bitmap_invalidate (region);

			return status;
		}
	}

	/* apply the same transformation matrix to all paths */
	status = gdip_region_transform_tree (region->tree, matrix);

	/* invalidate the bitmap so it will get re-created on the next gdip_region_bitmap_ensure call */
	gdip_region_bitmap_invalidate (region);

	return status;
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCreateRegionPath (GpPath *path, GpRegion **region)
{
	GpRegion *result;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!region || !path)
		return InvalidParameter;

	result = gdip_region_new ();
	if (!result)
		return OutOfMemory;

	status = gdip_region_create_from_path (result, path);
	if (status != Ok) {
		GdipDeleteRegion (result);
		return status;
	}

	*region = result;
	return Ok;
}


/*
 * The internal data representation for RegionData depends on the type of region.
 *
 * Type 1 (RegionTypeRect), variable size
 *	guint32 RegionType	Always 0x10000000
 *	guint32 Count		0-2^32
 *	GpRectF[Count] Points
 *
 * Type 2 (RegionTypePath), variable size
 *	guint32 RegionType	Always 0x10000001
 *	GpPathTree tree
 *
 * Type 3 (RegionTypeInfinite)
 *	guint32 RegionType	Always 0x10000003.
 *
 * where GpPathTree is
 *	guint32 Tag		1 = Path, 2 = Tree
 *	data[n]
 *
 *	where data is for tag 1 (Path)
 *		guint32 Count		0-2^32
 *		GpFillMode FillMode	
 *		guint8[Count] Types	
 *		GpPointF[Count] Points
 *	or
 *	where data is for tag 2 (Tree)
 *		guint32 Operation	see CombineMode
 *		guint32 Size1		0-2^32
 *		byte[Size1]		branch #1
 *		guint32 Size2		0-2^32
 *		byte[Size2]		branch #2
 */

GpStatus WINGDIPAPI
GdipGetRegionDataSize (GpRegion *region, UINT *bufferSize)
{
	if (!region || !bufferSize)
		return InvalidParameter;

	*bufferSize = sizeof (RegionHeader);

	switch (region->type) {
	case RegionTypeRect:
		*bufferSize += sizeof (DWORD) + region->cnt * sizeof (GpRectF);
		break;
	case RegionTypePath:
		/* regiontype, tree */
		*bufferSize += sizeof (DWORD) + gdip_region_get_tree_size (region->tree);
		break;
	case RegionTypeInfinite:
		// Only one DWORD.
		*bufferSize += sizeof (DWORD);
		break;
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}
	return Ok;
}


GpStatus WINGDIPAPI
GdipGetRegionData (GpRegion *region, BYTE *buffer, UINT bufferSize, UINT *sizeFilled)
{
	GpStatus status;
	UINT size;
	UINT filled = 0;
	RegionHeader header;
	header.combiningOps = 0;

	if (!region || !buffer || !bufferSize)
		return InvalidParameter;

	status = GdipGetRegionDataSize (region, &size);
	if (status != Ok)
		return status;
	if (size > bufferSize)
		return InsufficientBuffer;

	/* Write the region header at the end, as we need to calculate a checksum based off all the data. */
	filled += sizeof (RegionHeader);

	switch (region->type) {
	case RegionTypeRect: {
		DWORD type;

		if (region->cnt) {
			type = RegionDataRect;
			memcpy (buffer + filled, &type, sizeof (DWORD));
			filled += sizeof (DWORD);

			memcpy (buffer + filled, region->rects, region->cnt * sizeof (GpRectF));
			filled += region->cnt * sizeof (GpRectF);
		} else {
			type = RegionDataEmptyRect;
			memcpy (buffer + filled, &type, sizeof (DWORD));

			filled += sizeof (DWORD);
		}

		break;
	}
	case RegionTypePath: {
		DWORD type = RegionDataPath;
		memcpy (buffer + filled, &type, sizeof (DWORD));
		filled += sizeof (DWORD);

		if (!gdip_region_serialize_tree (region->tree, buffer + filled, bufferSize - filled, &filled))
			return InsufficientBuffer;
		break;
	}
	case RegionTypeInfinite: {
		DWORD type = RegionDataInfiniteRect;
		memcpy (buffer + filled, &type, sizeof (DWORD));
		filled += sizeof (DWORD);
		break;
	}
	default:
		g_warning ("unknown type 0x%08X", region->type);
		return NotImplemented;
	}

	/* Write the header at the start of the buffer. */
	header.size = filled - 8;
	header.magic = 0xdbc01002;
	header.combiningOps = 0;
	memcpy (buffer, &header, sizeof (RegionHeader));

	/* Finally, write the checksum. */
	header.checksum = gdip_crc32 (buffer + 8, filled - 8);
	memcpy (buffer + 4, &header.checksum, sizeof (DWORD));

	if (sizeFilled)
		*sizeFilled = filled;

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetRegionHRgn (GpRegion *region, GpGraphics *graphics, HRGN *hRgn)
{
	if (!region || !graphics || !hRgn)
		return InvalidParameter;

	/* infinite region returns NULL */
	if (gdip_is_InfiniteRegion (region)) {
		*hRgn = NULL;
		return Ok;
	}

	/* calling GdipGetRegionHRgn multiple times returns a different HRNG value
	   (i.e. each to be freed separately) */
	return GdipCloneRegion (region, (GpRegion**)hRgn);
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCreateRegionHrgn (HRGN hRgn, GpRegion **region)
{
	if (!hRgn || !region)
		return InvalidParameter;

	return GdipCloneRegion ((GpRegion*) hRgn, region);
}
