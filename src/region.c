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
#include "graphics-path-private.h"

/*
	Helper functions
*/

static void
gdip_from_Rect_To_RectF (GpRect* rect, GpRectF* rectf)
{
        rectf->X = rect->X;
        rectf->Y = rect->Y;
        rectf->Width = rect->Width;
        rectf->Height = rect->Height;
}

static void
gdip_add_rect_to_array (GpRectF** srcarray, int* elements,  GpRectF* rect)
{
        GpRectF *array, *next;

        array = GdipAlloc (sizeof (GpRectF) * (*elements + 1));
        memcpy (array, *srcarray, sizeof (GpRectF) * (*elements));

        if (*srcarray)
                GdipFree (*srcarray);

        next = array;
        next += (*elements);
        memcpy (next, rect, sizeof (GpRectF));

        *srcarray = array;
        *elements = *elements + 1;
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

/* This internal version doesn't require a Graphic object to work */
static BOOL
gdip_is_region_empty (GpRegion *region)
{
	GpRectF rect;

	if (!region)
		return FALSE;

	if (region->type == RegionTypePath) {
		/* check for an existing, but empty, path list */
		if (!region->tree)
			return TRUE;
		if (region->tree->path)
			return (region->tree->path->count == 0);
		return FALSE;
	}

	if (!region->rects || (region->cnt == 0))
		return TRUE;

	gdip_get_bounds (region->rects, region->cnt, &rect);
	return ((rect.Width == 0) || (rect.Height == 0));
}

static BOOL
gdip_is_rect_infinite (GpRectF *rect)
{
	return (rect && (rect->X == REGION_INFINITE_POSITION) && 
		(rect->Y == REGION_INFINITE_POSITION) &&
		(rect->Width == REGION_INFINITE_LENGTH) && 
		(rect->Height == REGION_INFINITE_LENGTH));
}

BOOL
gdip_is_InfiniteRegion (GpRegion *region)
{
	switch (region->type) {
	case RegionTypeRectF:
		if (region->cnt != 1)
	              return FALSE;
		return gdip_is_rect_infinite (region->rects);
	case RegionTypePath:
		/* FIXME: incomplete and not 100% accurate (curves) - but cover the most common case */
		if (region->tree && region->tree->path && (region->tree->path->count == 4)) {
			GpRectF bounds;
			if (GdipGetPathWorldBounds (region->tree->path, &bounds, NULL, NULL) != Ok)
				return gdip_is_rect_infinite (&bounds);
		}
		break;
	default:
		g_warning ("unknown type %d", region->type);
		break;
	}
	return FALSE;
}

static BOOL
gdip_intersects (GpRectF *rect1, GpRectF *rect2)
{
	if (rect1->X + rect1->Width == rect2->X) {
		return TRUE;
	}

	return (rect1->X < rect2->X + rect2->Width &&
		rect1->X + rect1->Width > rect2->X &&
		rect1->Y < rect2->Y + rect2->Height &&
		rect1->Y + rect1->Height > rect2->Y);
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
gdip_add_rect_to_array_notcontained (GpRectF** srcarray, int* elements,  GpRectF* rect)
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

	gdip_add_rect_to_array (srcarray, elements, rect);
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
	region->type = RegionTypeEmpty;

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

void
gdip_copy_region (GpRegion *source, GpRegion *dest)
{
	dest->type = source->type;

	if (source->rects) {
		dest->cnt = source->cnt;
	        dest->rects = (GpRectF *) GdipAlloc (sizeof (GpRectF) * source->cnt);
        	memcpy (dest->rects, source->rects, sizeof (GpRectF) * source->cnt);
	} else {
		dest->cnt = 0;
		dest->rects = NULL;
	}

	if (source->tree) {
		dest->tree = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));
		gdip_region_copy_tree (source->tree, dest->tree);
	} else {
		dest->tree = NULL;
	}

	if (source->bitmap) {
		dest->bitmap = gdip_region_bitmap_clone (source->bitmap);
	} else {
		dest->bitmap = NULL;
	}
}

/* convert a rectangle-based region to a path based region */
static void 
gdip_region_convert_to_path (GpRegion *region)
{
	int i;
	GpRectF *rect;

	/* no conversion is required for complex regions */
	if (!region || (region->type != RegionTypeRectF))
		return;

	region->type = RegionTypePath;
	region->tree = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));

	GdipCreatePath (FillModeAlternate, &region->tree->path);
	/* all rectangles are converted into a single path */
	for (i = 0, rect = region->rects; i < region->cnt; i++, rect++) {
		GdipAddPathRectangle (region->tree->path, rect->X, rect->Y, rect->Width, rect->Height);
	}

	if (region->rects) {
		GdipFree (region->rects);
		region->rects = NULL;
	}
}

/*
 * Create a region (path-tree) from a path.
 */
static GpStatus
gdip_region_create_from_path (GpRegion *region, GpPath *path)
{
	region->type = RegionTypePath;
	region->tree = (GpPathTree *) GdipAlloc (sizeof (GpPathTree));
	return GdipClonePath (path, &region->tree->path);
}

static GpStatus
gdip_createRegion (GpRegion **region, RegionType type, void *src)
{
        GpRegion *result;
        GpRectF rect;

        result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	result->type = type;
	result->cnt = 0;
        result->rects = NULL;
        result->tree = NULL;
        result->bitmap = NULL;

        switch (type) {
        case RegionTypeRect:
                gdip_from_Rect_To_RectF ((GpRect *)src, &rect);
                gdip_add_rect_to_array (&result->rects, &result->cnt,  &rect);
		result->type = RegionTypeRectF;
                break;
        case RegionTypeRectF:
                gdip_add_rect_to_array (&result->rects, &result->cnt,  (GpRectF *)src);
                break;
        case RegionTypeEmpty:
		GdipSetInfinite (result);
		/* note: GdipSetInfinite converts type to RegionTypeRectF */
                break;
	case RegionTypePath:
		gdip_region_create_from_path (result, (GpPath*)src);
		break;
        default:
		g_warning ("unknown type %d", result->type);
		return NotImplemented;
        }

	*region = result;
	return Ok;
}

/*
	API implementation
*/

// coverity[+alloc : arg-*0]
GpStatus
GdipCreateRegion (GpRegion **region)
{
        if (!region)
                return InvalidParameter;

        return gdip_createRegion (region, RegionTypeEmpty, NULL);
}

// coverity[+alloc : arg-*1]
GpStatus
GdipCreateRegionRect (GDIPCONST GpRectF *rect, GpRegion **region)
{
        if (!region || !rect)
                return InvalidParameter;

        return gdip_createRegion (region, RegionTypeRectF, (void*) rect);
}

// coverity[+alloc : arg-*1]
GpStatus
GdipCreateRegionRectI (GDIPCONST GpRect *rect, GpRegion **region)
{
        if (!region || !rect)
                return InvalidParameter;

        return gdip_createRegion (region, RegionTypeRect, (void*) rect);
}

// coverity[+alloc : arg-*2]
GpStatus
GdipCreateRegionRgnData (GDIPCONST BYTE *regionData, int size, GpRegion **region)
{
	GpRegion *result;

	if (!region || !regionData)
		return InvalidParameter;
	if (size < 8)
		return GenericError;

	result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	memcpy (&result->type, regionData, sizeof (guint32));
	result->cnt = 0;
        result->rects = NULL;
	result->tree = NULL;
        result->bitmap = NULL;

	switch (result->type) {
	case RegionTypeRectF: {
		guint32 count;
		GpRectF *rect;
		int i;

		memcpy (&count, regionData + 4, sizeof (guint32));
		if (count != (size - 8) / sizeof (GpRectF)) {
			GdipFree (result);
			return InvalidParameter;
		}

		for (i = 0, rect = (GpRectF*)(regionData + 8); i < count; i++, rect++)
	                gdip_add_rect_to_array (&result->rects, &result->cnt, rect);
		}
		break;
	case RegionTypePath:
		if (size < 16) {
			GdipFree (result);
			return InvalidParameter;
		}

		result->tree = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		if (!gdip_region_deserialize_tree ((BYTE*)(regionData + 4), (size - 4), result->tree)) {
			GdipFree (result);
			return InvalidParameter;
		}
		break;
	default:
		g_warning ("unknown type %d", result->type);
		GdipFree (result);
		return NotImplemented;
	}

	*region = result;
	return Ok;
}

// coverity[+alloc : arg-*1]
GpStatus
GdipCloneRegion (GpRegion *region, GpRegion **cloneRegion)
{
        GpRegion *result;

        if (!region || !cloneRegion)
                return InvalidParameter;

	result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	if (!result)
		return OutOfMemory;

	gdip_copy_region (region, result);
	*cloneRegion = result;
        return Ok;
}


GpStatus
GdipDeleteRegion (GpRegion *region)
{
        if (!region)
                return InvalidParameter;

	gdip_clear_region (region);
        GdipFree (region);

        return Ok;
}


GpStatus
GdipSetInfinite (GpRegion *region)
{
        GpRectF rect;

        if (!region)
                return InvalidParameter;

	gdip_clear_region (region);
	region->type = RegionTypeRectF;

        rect.X = rect.Y = REGION_INFINITE_POSITION;
        rect.Width = rect.Height = REGION_INFINITE_LENGTH;

        gdip_add_rect_to_array (&region->rects, &region->cnt,  &rect);
        return Ok;
}


GpStatus
GdipSetEmpty (GpRegion *region)
{
        if (!region)
                return InvalidParameter;

	gdip_clear_region (region);
	region->type = RegionTypeRectF;

        return Ok;
}

/* pre-process negative width and height, without modifying the originals, see bug #383878 */
static void
gdip_normalize_rectangle (GpRectF *rect, GpRectF *normalized)
{
	if (rect->Width < 0) {
		normalized->X = rect->X + rect->Width;
		normalized->Width = fabs (rect->Width);
	} else {
		normalized->X = rect->X;
		normalized->Width = rect->Width;
	}

	if (rect->Height < 0) {
		normalized->Y = rect->Y + rect->Height;
		normalized->Height = fabs (rect->Height);
	} else {
		normalized->Y = rect->Y;
		normalized->Height = rect->Height;
	}
}

/* Exclude */
static void
gdip_combine_exclude (GpRegion *region, GpRectF *rtrg, int cntt)
{
	GpRectF *allsrcrects = NULL, *rects = NULL;
	GpRectF *alltrgrects = NULL, *rect, *rectop, *recttrg;
        int allsrccnt = 0, cnt = 0, i, n, alltrgcnt = 0;
	GpRectF current, rslt, newrect;
	BOOL storecomplete;

	/* Create the list of source rectangles to process, it will contain splitted ones later */
        for (i = 0, rect = region->rects; i < region->cnt; i++, rect++)
                gdip_add_rect_to_array (&allsrcrects, &allsrccnt, rect);

	/* Create the list of target rectangles to process, it will contain splitted ones later */
        for (i = 0, rect = rtrg; i < cntt; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		gdip_add_rect_to_array (&alltrgrects, &alltrgcnt, &normal);
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

			gdip_add_rect_to_array_notcontained (&rects, &cnt, &newrect);

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
				gdip_add_rect_to_array (&allsrcrects, &allsrccnt,  &rslt);
			}

			/* Special case where our rect is hit and split in two parts IIIUIII */
			if (recttrg->X >= current.X && recttrg->X + recttrg->Width  <= current.X + current.Width) {
				/* Generate extra right rect, keep previous values of Y and Height */
				newrect.Width = current.X + current.Width - (recttrg->X + recttrg->Width);
				newrect.X = recttrg->X + recttrg->Width;
				gdip_add_rect_to_array_notcontained (&rects, &cnt, &newrect);
			}

			storecomplete = FALSE;
			break;
		}

		/* don't include a rectangle identical to the excluded one! */
		if (storecomplete && !gdip_equals (rtrg, &current)) {
			gdip_add_rect_to_array_notcontained (&rects, &cnt,  &current);
		}
	}

	GdipFree (allsrcrects);
	GdipFree (alltrgrects);
	if (region->rects)
		GdipFree (region->rects);

        region->rects = rects;
        region->cnt = cnt;
}


/*
	Complement: the part of the second region not shared with the first region.
	Scans the region to be combined and store the rects not present in the region
*/
static void
gdip_combine_complement (GpRegion *region, GpRectF *rtrg, int cntt)
{
	GpRegion regsrc;
	GpRectF* trg, *rect;
	GpRectF* allsrcrects = NULL;
        int allsrccnt = 0, i,  trgcnt;

	/* Create the list of source rectangles to process */
        for (i = 0, rect = rtrg; i < cntt; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		gdip_add_rect_to_array (&allsrcrects, &allsrccnt, &normal);
	}

	regsrc.rects = allsrcrects;
	regsrc.cnt = allsrccnt;
	trg = region->rects;
	trgcnt = region->cnt;

	gdip_combine_exclude (&regsrc, trg, trgcnt);

	if ((regsrc.rects != allsrcrects) || (regsrc.cnt != allsrccnt)) {
		if (region->rects)
			GdipFree (region->rects);

		region->rects = regsrc.rects;
		region->cnt = regsrc.cnt;
	}
}


/* Union */
static void
gdip_combine_union (GpRegion *region, GpRectF *rtrg, int cnttrg)
{
	GpRectF *allrects = NULL, *rects = NULL;
	GpRectF *recttrg, *rect, *rectop;
        int allcnt = 0, cnt = 0, i, n;
	GpRectF current, rslt, newrect;
	BOOL storecomplete, contained;

        /* All the src and trg rects in a single array*/
        for (i = 0, rect = region->rects; i < region->cnt; i++, rect++)
                gdip_add_rect_to_array (&allrects, &allcnt,  rect);

        for (i = 0, rect = rtrg; i < cnttrg; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		gdip_add_rect_to_array (&allrects, &allcnt, &normal);
	}

        if (allcnt == 0) {
                GdipFree (allrects);
                return;
        }

	/* Init current with the first element in the array */
	current.X = REGION_INFINITE_POSITION;
	current.Y = REGION_INFINITE_POSITION;
	current.Width = 0; current.Height = 0;

	while (gdip_getlowestrect (allrects, allcnt, &current, &rslt)) {

		current.X = rslt.X; current.Y = rslt.Y;
		current.Width = rslt.Width; current.Height = rslt.Height;
		storecomplete = TRUE;

		/* Current rect with lowest y and X againt the stored ones */
		for (i = 0, recttrg = allrects; i < allcnt; i++, recttrg++) {

			/* If it has lower coordinates it has been already processed */
			if (current.Y > recttrg->Y ||
				(current.Y == recttrg->Y && current.X > recttrg->X)) {
				continue;
			}

			if (gdip_intersects (&current, recttrg) == FALSE
				|| gdip_equals (&current, recttrg) == TRUE ||
				recttrg->Height < 0 || recttrg->Width < 0) {
				continue;
			}

			if (gdip_contains  (recttrg, &current) == TRUE) {
				continue;
			}

			/* Once a rect is splitted, we do not want to take into account anymore */
			for (rectop = allrects, n = 0; n < allcnt; n++, rectop++) {
				if (gdip_equals (&current, rectop)) {
					rectop->X = 0; rectop->Y = 0;
					rectop->Width = 0; rectop->Height = 0;
					break;
				}
			}

			/* Our rect intersects in the lower part with another rect */
			newrect.Y = current.Y;
			if (current.Y == recttrg->Y) {
				newrect.X = MIN (current.X, recttrg->X);
				newrect.Width = MAX (current.X + current.Width, recttrg->X + recttrg->Width) - newrect.X;
				newrect.Height = MIN (current.Height, recttrg->Height);
			}
			else {
				newrect.X = current.X;
				newrect.Width = current.Width;
				newrect.Height = recttrg->Y - current.Y;

				/* If it's contained inside, get the > height */
				if (recttrg->X >= current.X && recttrg->X + recttrg->Width <= current.X + current.Width)
					newrect.Height = MAX (current.Height, recttrg->Height);
			}

			gdip_add_rect_to_array_notcontained (&rects, &cnt, &newrect);

			/* Push what's left from the current the rect in the list of rects to process
			 if it's already not contained in other rects except the current (we just split from there) */
			rslt.X = current.X;
			rslt.Y = newrect.Y + newrect.Height;
			rslt.Width = current.Width;
			rslt.Height = current.Height - newrect.Height;

			contained = FALSE;
			for (rectop = allrects, n = 0; n < allcnt; n++, rectop++) {
				if (gdip_equals (rectop, &current)) /* If it's contained with the current does not count */
					continue;

				if (gdip_contains (&rslt, rectop)) {
					contained = TRUE;
					break;
				}
			}

			if (contained == FALSE && rslt.Height > 0 && rslt.Width > 0) {
				gdip_add_rect_to_array (&allrects, &allcnt,  &rslt);
				recttrg = allrects;
			}

			/* If both we at the same Y when take into account the X also to process the following
			   that exceeds the X also */
			if (recttrg->Y == current.Y) {
				if (recttrg->Height <= current.Height) { /* Takes all part */
					recttrg->Width = recttrg->X + recttrg->Width - (recttrg->X + recttrg->Width);
					recttrg->X = newrect.X + newrect.Width;
					recttrg->Y += newrect.Height;
					recttrg->Height -= newrect.Height;
				} else {
					if (newrect.X + newrect.Width >= recttrg->X + recttrg->Width) {
						recttrg->Y += newrect.Height;
						recttrg->Height -= newrect.Height;
					}
				}
			}

			/* If it's contained inside, get the > height  */
			if (recttrg->X >= current.X && recttrg->X + recttrg->Width <= current.X + current.Width) {
				recttrg->Height = MAX (recttrg->Y + recttrg->Height - (current.Y + current.Height), 0);
				recttrg->Y = newrect.Y + newrect.Height;
			}

			storecomplete = FALSE;
			break;
		}

		if (storecomplete) {
			gdip_add_rect_to_array_notcontained (&rects, &cnt,  &current);
		}

	}

	GdipFree (allrects);
	if (region->rects)
		GdipFree (region->rects);

        region->rects = rects;
        region->cnt = cnt;
}

/* Intersect */
static void
gdip_combine_intersect (GpRegion *region, GpRectF *rtrg, int cnttrg)
{
	GpRectF *rectsrc;
	int src, trg;
	GpRectF rectcur;
	GpRegion regunion;
	GpRectF *recttrg;

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
			gdip_combine_union (&regunion, &rectcur, 1);
		}
	}

	if (region->rects)
		GdipFree (region->rects);

	region->rects = regunion.rects;
	region->cnt = regunion.cnt;
}

/* Xor */
static void
gdip_combine_xor (GpRegion *region, GpRectF *recttrg, int cnttrg)
{
        GpRegion *rgnsrc;  /* All rectangles of both regions*/
        GpRegion *rgntrg;  /* Only the ones that intersect*/
        GpRectF *allrects = NULL, *rect;
        int allcnt = 0, i;

        /* All the src and trg rects in a single array*/
        for (i = 0, rect = region->rects; i < region->cnt; i++, rect++)
                gdip_add_rect_to_array (&allrects, &allcnt,  rect);

	for (i = 0, rect = recttrg; i < cnttrg; i++, rect++) {
		/* normalize */
		GpRectF normal;
		gdip_normalize_rectangle (rect, &normal);
		gdip_add_rect_to_array (&allrects, &allcnt, &normal);
	}

	rgnsrc = (GpRegion *) GdipAlloc (sizeof (GpRegion));
	rgnsrc->type = RegionTypeRectF;
        rgnsrc->cnt = allcnt;
        rgnsrc->rects = allrects;

        GdipCloneRegion (region, &rgntrg);
        gdip_combine_intersect (rgntrg, recttrg, cnttrg);
	/* exclude the intersecting rectangles (if any) */
	if (rgntrg->cnt > 0)
		gdip_combine_exclude (rgnsrc, rgntrg->rects, rgntrg->cnt);

        if (region->rects)
                GdipFree (region->rects);

        region->rects = rgnsrc->rects;
        region->cnt = rgnsrc->cnt;

        GdipFree (rgnsrc);
        GdipDeleteRegion (rgntrg);
}


GpStatus
GdipCombineRegionRect (GpRegion *region, GDIPCONST GpRectF *rect, CombineMode combineMode)
{
        if (!region || !rect)
                return InvalidParameter;

	/* allow the current region to "revert" to a simple RegionTypeRect if possible */
	if (combineMode == CombineModeReplace)
		GdipSetEmpty (region);

	/* Union with infinity is a no-op (still an infinite region) */
	if ((combineMode == CombineModeUnion) && gdip_is_InfiniteRegion (region))
		return Ok;

	if (region->type == RegionTypePath) {
		GpPath *path = NULL;
		GpStatus status;

		/* Convert GpRectF to GpPath and use GdipCombineRegionPath */
		status = GdipCreatePath (FillModeAlternate, &path);
		if (status != Ok) {
			if (path)
				GdipDeletePath (path);
			return status;
		}
		GdipAddPathRectangle (path, rect->X, rect->Y, rect->Width, rect->Height);
		status = GdipCombineRegionPath (region, path, combineMode);
		GdipDeletePath (path);
		return status;
	}

	/* region is rectangle-based */
        switch (combineMode) {
        case CombineModeExclude:
                gdip_combine_exclude (region, (GpRectF *) rect, 1);
                break;
        case CombineModeComplement:
                gdip_combine_complement (region, (GpRectF *) rect, 1);
                break;
        case CombineModeIntersect:
                gdip_combine_intersect (region, (GpRectF *) rect, 1);
                break;
        case CombineModeUnion:
                gdip_combine_union (region, (GpRectF *) rect, 1);
                break;
        case CombineModeXor:
                gdip_combine_xor (region, (GpRectF *) rect, 1);
                break;
	case CombineModeReplace: /* Used by Graphics clipping */
		gdip_add_rect_to_array (&region->rects, &region->cnt, (GpRectF *)rect);
		break;
        default:
               return NotImplemented;
        }

        return Ok;
}


GpStatus
GdipCombineRegionRectI (GpRegion *region, GDIPCONST GpRect *recti, CombineMode combineMode)
{
        GpRectF rect;

        if (!region || !recti)
                return InvalidParameter;

        gdip_from_Rect_To_RectF ((GpRect *) recti, &rect);

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

	if (region->type == RegionTypeRectF)
		gdip_region_convert_to_path (region);
	
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

GpStatus
GdipCombineRegionPath (GpRegion *region, GpPath *path, CombineMode combineMode)
{
	GpRegionBitmap *path_bitmap, *result;

	if (!region || !path)
		return InvalidParameter;

	/* special case #1 - replace */
	if (combineMode == CombineModeReplace) {
		gdip_clear_region (region);
		gdip_region_create_from_path (region, path);
		return Ok;
	}

	/* special case #2 - the region is empty */
	if (gdip_is_region_empty (region)) {
		switch (combineMode) {
		case CombineModeComplement:
		case CombineModeUnion:
		case CombineModeXor:
			/* this is like "adding" the path */
			gdip_clear_region (region);
			gdip_region_create_from_path (region, path);
			break;
		default:
			/* Intersect and Exclude are no-op on an empty region */
			break;
		}
		return Ok;
	}

	/* special case #3 - the region is infinite */
	if (gdip_is_InfiniteRegion (region)) {
		/* additional shortcuts are possible if path is empty */
		BOOL empty = (path->count == 0);

		switch (combineMode) {
		case CombineModeUnion:
			/* Union with infinity is a no-op (still an infinite region) */
			return Ok;
		case CombineModeComplement:
			/* Complement of infinity is empty - whatever the path is */
			gdip_clear_region (region);
			region->type = RegionTypeRectF;
			return Ok;
		case CombineModeIntersect:
			/* Intersection with infinity is the path itself */
			gdip_clear_region (region);
			if (empty)
				region->type = RegionTypeRectF;
			else
				gdip_region_create_from_path (region, path);
			return Ok;
		case CombineModeExclude:
			if (gdip_combine_exclude_from_infinite (region, path))
				return Ok;
			break;
		default:
			/* Xor must be treated as a "normal" case unless the path is empty */
			if (empty)
				return Ok;
			break;
		}
	}

	if (region->type == RegionTypeRectF)
		gdip_region_convert_to_path (region);

	/* make sure the region's bitmap is available */
	gdip_region_bitmap_ensure (region);
	g_assert (region->bitmap);

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
		region->tree->branch1->path = region->tree->path;
		region->tree->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
	} else {
		/* move the current base tree into branch1 of a new tree ... */
		GpPathTree* tmp = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		tmp->branch1 = region->tree;
		tmp->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		region->tree = tmp;
	}
	/* ... and clone the specified path into branch2 */
	region->tree->mode = combineMode;
	region->tree->path = NULL;
	GdipClonePath (path, &region->tree->branch2->path);
	return Ok;
}


static GpStatus
gdip_combine_pathbased_region (GpRegion *region1, GpRegion *region2, CombineMode combineMode)
{
	GpRegionBitmap *result;
	GpPathTree* tmp;

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
		region1->tree->branch1->path = region1->tree->path;
		region1->tree->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
	} else {
		/* move the current base tree into branch1 of a new tree ... */
		GpPathTree* tmp = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		tmp->branch1 = region1->tree;
		tmp->branch2 = (GpPathTree*) GdipAlloc (sizeof (GpPathTree));
		region1->tree = tmp;
	}

	region1->tree->mode = combineMode;
	region1->tree->path = NULL;

	/* add a copy of region2 tree into region1 tree */
	if (region2->tree->path) {
		GdipClonePath (region2->tree->path, &region1->tree->branch2->path);
	} else {
		gdip_region_copy_tree (region2->tree, region1->tree->branch2);
	}
	return Ok;
}


GpStatus
GdipCombineRegionRegion (GpRegion *region,  GpRegion *region2, CombineMode combineMode)
{
        if (!region || !region2)
                return InvalidParameter;

	/* special case to deal with copying empty and infinity regions */
	/* CombineModeReplace is used by Graphics clipping */
	if (combineMode == CombineModeReplace) {
		GdipSetEmpty (region);
		gdip_copy_region (region2, region);
		return Ok;
	} else if (gdip_is_region_empty (region)) {
		switch (combineMode) {
		case CombineModeIntersect:
		case CombineModeExclude:
			/* Intersect and Exclude are no-op on an empty region */
			return Ok;
		default:
			/* for Complement, Union and Xor this is normal processing */
			break;
		}
	} else if (gdip_is_InfiniteRegion (region)) {
		/* additional shortcuts are possible if path is empty */
		BOOL empty = gdip_is_region_empty (region2);

		switch (combineMode) {
		case CombineModeUnion:
			/* Union with infinity is a no-op (still an infinite region) */
			return Ok;
		case CombineModeComplement:
			/* Complement of infinity is empty - whatever the path is */
			gdip_clear_region (region);
			region->type = RegionTypeRectF;
			return Ok;
		case CombineModeIntersect:
			/* Intersection with infinity is the path itself (like an Union with Empty) */
			gdip_clear_region (region);
			region->type = RegionTypeRectF;
			if (empty)
				return Ok;
			combineMode = CombineModeUnion; 
			break;
		case CombineModeExclude:
			if (empty)
				return Ok;
			if ((region2->type == RegionTypePath) && region2->tree && region2->tree->path &&
				gdip_combine_exclude_from_infinite (region, region2->tree->path))
				return Ok;
			break;
		default:
			/* Xor must be treated as a "normal" case unless the path is empty */
			if (empty)
				return Ok;
		}
	}

	if (region->type == RegionTypePath) {
		gdip_region_convert_to_path (region2);
		return gdip_combine_pathbased_region (region, region2, combineMode);
	} else if (region2->type == RegionTypePath) {
		gdip_region_convert_to_path (region);
		return gdip_combine_pathbased_region (region, region2, combineMode);
	}

	/* at this stage we are sure that BOTH region and region2 are rectangle 
	 * based, so we can use the old rectangle-based code to combine regions
	 */
        switch (combineMode) {
        case CombineModeExclude:
                gdip_combine_exclude (region, region2->rects, region2->cnt);
                break;
        case CombineModeComplement:
                gdip_combine_complement (region, region2->rects, region2->cnt);
                break;
        case CombineModeIntersect:
                gdip_combine_intersect (region, region2->rects, region2->cnt);
                break;
        case CombineModeUnion:
                gdip_combine_union (region, region2->rects, region2->cnt);
                break;
        case CombineModeXor:
                gdip_combine_xor (region, region2->rects, region2->cnt);
                break;
        default:
               return NotImplemented;
        }

        return Ok;
}

GpStatus
GdipGetRegionBounds (GpRegion *region, GpGraphics *graphics, GpRectF *rect)
{
        if (!region || !graphics || !rect)
                return InvalidParameter;

	if (region->type == RegionTypePath) {
		GpRect bounds;

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
	} else {
	        gdip_get_bounds (region->rects , region->cnt, rect);
	}

        return Ok;
}


GpStatus
GdipIsEmptyRegion (GpRegion *region, GpGraphics *graphics, BOOL *result)
{
        if (!region || !graphics || !result)
                return InvalidParameter;

	*result = gdip_is_region_empty (region);
        return Ok;
}


GpStatus
GdipIsInfiniteRegion (GpRegion *region, GpGraphics *graphics, BOOL *result)
{
      if (!region || !graphics || !result)
                return InvalidParameter;

      *result = gdip_is_InfiniteRegion (region);
      return Ok;
}


GpStatus
GdipIsVisibleRegionPoint (GpRegion *region, float x, float y, GpGraphics *graphics, BOOL *result)
{
        if (!region || !result)
                return InvalidParameter;

	if (region->type == RegionTypePath) {
		gdip_region_bitmap_ensure (region);
		g_assert (region->bitmap);

		*result = gdip_region_bitmap_is_point_visible (region->bitmap, x, y);
	} else {
		*result = gdip_is_Point_in_RectFs_Visible (x, y, region->rects, region->cnt);
	}

        return Ok;
}


GpStatus
GdipIsVisibleRegionPointI (GpRegion *region, int x, int y, GpGraphics *graphics, BOOL *result)
{
        return GdipIsVisibleRegionPoint (region, x, y, graphics, result);
}


GpStatus
GdipIsVisibleRegionRect (GpRegion *region, float x, float y, float width, float height, GpGraphics *graphics, BOOL *result)
{
        BOOL found = FALSE;

        if (!region || !result)
                return InvalidParameter;

        if (width ==0 || height ==0) {
                *result = FALSE;
                return Ok;
        }

	if (region->type == RegionTypePath) {
		GpRect rect;

		rect.X = x;
		rect.Y = y;
		rect.Width = width;
		rect.Height = height;
		
		gdip_region_bitmap_ensure (region);
		g_assert (region->bitmap);

		found = gdip_region_bitmap_is_rect_visible (region->bitmap, &rect);
	} else {
		float posy, posx;
		GpRectF recthit;

		recthit.X = x;
		recthit.Y = y;
		recthit.Width = width;
		recthit.Height = height;

		/* Any point of intersection ?*/
		for (posy = 0; posy < recthit.Height && found == FALSE; posy++) {
			for (posx = 0; posx < recthit.Width ; posx++) {
				if (gdip_is_Point_in_RectFs_Visible (recthit.X + posx , recthit.Y + posy, region->rects, region->cnt) == TRUE) {
					found = TRUE;
					break;
				}
			}
		}
	}

        *result = found;
        return Ok;
}


GpStatus
GdipIsVisibleRegionRectI (GpRegion *region, int x, int y, int width, int height, GpGraphics *graphics, BOOL *result)
{
        return GdipIsVisibleRegionRect (region, x, y, width, height, graphics, result);
}


GpStatus
GdipGetRegionScansCount (GpRegion *region, int* count, GpMatrix* matrix)
{
	GpRegion *work = NULL;
	GpStatus status;

        if (!region || !count)
                return InvalidParameter;

	/* apply any user supplied matrix transformation */
	if (!gdip_is_matrix_empty (matrix)) {
		int i;

		/* the matrix doesn't affect the original region - only the result */
		status = GdipCloneRegion (region, &work);
		if (status != Ok) {
			if (work)
				GdipDeleteRegion (work);
			return status;
		}

		/* if required convert into a path-based region */
		if (work->type != RegionTypePath)
			gdip_region_convert_to_path (work);

		/* transform all the paths */
		status = gdip_region_transform_tree (work->tree, matrix);
		if (status != Ok) {
			GdipDeleteRegion (work);
			return status;
		}
		/* note: any existing bitmap has been invalidated */
		gdip_region_bitmap_invalidate (work);
	} else {
		work = region;
	}

	if (work->type == RegionTypePath) {
		/* ensure the bitmap is usable */
		gdip_region_bitmap_ensure (work);

		/* check if region is too large to render */
		if (work->bitmap)
			*count = gdip_region_bitmap_get_scans (work->bitmap, NULL, -1);
		else
			*count = 0;
	} else {
	        *count = work->cnt;
	}

	/* delete the clone */
	if (work != region)
		GdipDeleteRegion (work);
	return Ok;
}

GpStatus
GdipGetRegionScans (GpRegion *region, GpRectF* rects, int* count, GpMatrix* matrix)
{
	GpRegion *work = NULL;
	GpStatus status;

        if (!region || !rects|| !count)
                return InvalidParameter;

	/* apply any user supplied matrix transformation */
	if (!gdip_is_matrix_empty (matrix)) {
		int i;

		/* the matrix doesn't affect the original region - only the result */
		status = GdipCloneRegion (region, &work);
		if (status != Ok) {
			if (work)
				GdipDeleteRegion (work);
			return status;
		}

		/* if required convert into a path-based region */
		if (work->type != RegionTypePath)
			gdip_region_convert_to_path (work);

		/* transform all the paths */
		status = gdip_region_transform_tree (work->tree, matrix);
		if (status != Ok) {
			GdipDeleteRegion (work);
			return status;
		}
		/* note: any existing bitmap has been invalidated */
		gdip_region_bitmap_invalidate (work);
	} else {
		work = region;
	}

	if (region->type == RegionTypePath) {
		/* ensure the bitmap is usable */
		gdip_region_bitmap_ensure (work);

		/* check if region is too large to render */
		if (work->bitmap)
			*count = gdip_region_bitmap_get_scans (work->bitmap, rects, *count);
		else
			*count = 0;
	} else {
	        memcpy (rects, work->rects, sizeof (GpRectF) * *count);
	        *count = work->cnt;
	}

	/* delete the clone */
	if (work != region)
		GdipDeleteRegion (work);
	return Ok;
}

GpStatus
GdipIsEqualRegion (GpRegion *region, GpRegion *region2, GpGraphics *graphics, BOOL *result)
{
        int i;
        GpRectF *rectsrc, *recttrg;

        if (!region || !region2 || !graphics || !result)
                return InvalidParameter;

	/* quick case: same pointer == same region == equals */
	if (region == region2) {
		*result = TRUE;
		return Ok;
	}

	if ((region->type == RegionTypePath) || (region2->type == RegionTypePath)) {
		/* if required convert one region to a path based region */
		if (region->type != RegionTypePath)
			gdip_region_convert_to_path (region);
		gdip_region_bitmap_ensure (region);
		g_assert (region->bitmap);

		if (region2->type != RegionTypePath)
			gdip_region_convert_to_path (region2);

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

GpStatus
GdipTranslateRegion (GpRegion *region, float dx, float dy)
{
        if (!region)
                return InvalidParameter;

	/* can't transforman infinite region to anything else than an infinite region 
	 * (even if you scale it by half it's still infinite ;-) see unit tests
	 */
	if (gdip_is_InfiniteRegion (region))
		return Ok;

	if (region->type == RegionTypePath) {
		gdip_region_translate_tree (region->tree, dx, dy);
		/* any existing bitmap is still valid _if_ we update it's origin */
		if (region->bitmap) {
			region->bitmap->X += dx;
			region->bitmap->Y += dy;
		}
	} else if ((region->type == RegionTypeRectF) && region->rects) {
	        int i;
	        GpRectF *rect;
                for (i = 0, rect = region->rects ; i < region->cnt; i++, rect++) {
                        rect->X += dx;
                        rect->Y += dy;
                }
        }

        return Ok;
}

GpStatus
GdipTranslateRegionI (GpRegion *region, int dx, int dy)
{
        return GdipTranslateRegion (region, dx, dy);
}

/* this call doesn't exists in GDI+ */
static GpStatus
ScaleRegion (GpRegion *region, float sx, float sy)
{
	if (!region)
		return InvalidParameter;

	if ((region->type == RegionTypeRectF) && region->rects) {
	        int i;
	        GpRectF *rect;
                for (i = 0, rect = region->rects ; i < region->cnt; i++, rect++) {
                        rect->X *= sx;
                        rect->Y *= sy;
                        rect->Width *= sx;
                        rect->Height *= sy;
                }
        }

        return Ok;
}

GpStatus
GdipTransformRegion (GpRegion *region, GpMatrix *matrix)
{
	GpStatus status = Ok;

	if (!region || !matrix)
		return InvalidParameter;

	/* no transformation to do on an empty region */
	if ((region->cnt == 0) && (region->type == RegionTypeRectF))
		return Ok;

	/* don't (possibly) convert to a bitmap if the matrix is empty (a no-op) */
	if (gdip_is_matrix_empty (matrix))
		return Ok;

	/* can't transforman infinite region to anything else than an infinite region 
	 * (even if you scale it by half it's still infinite ;-) see unit tests
	 */
	if (gdip_is_InfiniteRegion (region))
		return Ok;

	/* try to avoid heavy stuff (e.g. conversion to path, invalidating 
	 * bitmap...) if the transform is:
	 * - a translation + scale operations (for rectangle ebased region)
	 * - only to do a scale operation (for a rectangle based region)
	 * - only to do a simple translation (for both rectangular and bitmap based regions)
	 */
	if ((matrix->xy == 0.0f) && (matrix->yx == 0.0f)) {
		BOOL s = (((matrix->xx != 1.0f) || (matrix->yy != 1.0f)) && (region->type == RegionTypeRectF));
		BOOL t = ((matrix->x0 != 0.0f) || (matrix->yx != 0.0f));
		if (s) {
			status = ScaleRegion (region, 
				gdip_matrix_get_x_scale (matrix), 
				gdip_matrix_get_y_scale (matrix));
		}
		if (t && (status == Ok)) {
			status = GdipTranslateRegion (region, 
				gdip_matrix_get_x_translation (matrix), 
				gdip_matrix_get_y_translation (matrix));
		}

		/* return now if we could optimize the transform (to avoid bitmaps) */
		if (t || s)
			return status;
	}

	/* most matrix operations would change the rectangles into path so we always preempt this */
	if (region->type != RegionTypePath)
		gdip_region_convert_to_path (region);

	/* apply the same transformation matrix to all paths */
	status = gdip_region_transform_tree (region->tree, matrix);

	/* invalidate the bitmap so it will get re-created on the next gdip_region_bitmap_ensure call */
	gdip_region_bitmap_invalidate (region);

        return status;
}

// coverity[+alloc : arg-*1]
GpStatus
GdipCreateRegionPath (GpPath *path, GpRegion **region)
{
        if (!region || !path)
                return InvalidParameter;

        return gdip_createRegion (region, RegionTypePath, (void*) path);
}


/*
 * The internal data representation for RegionData depends on the type of region.
 *
 * Type 0 (RegionTypeEmpty)
 *	Note: There is no type 0. RegionTypeEmpty are converted into 
 *	RegionTypeRectF (type 2) when a region is created.
 *
 * Type 1 (RegionTypeRect)
 *	Note: There is no type 1. RegionTypeRect are converted into 
 *	RegionTypeRectF (type 2) when a region is created.
 *
 * Type 2 (RegionTypeRectF), variable size
 *	guint32 RegionType	Always 2
 *	guint32 Count		0-2^32
 *	GpRectF[Count] Points
 *
 * Type 3 (RegionTypePath), variable size
 *	guint32 RegionType	Always 3
 *	GpPathTree tree
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

GpStatus
GdipGetRegionDataSize (GpRegion *region, UINT *bufferSize)
{
	if (!region || !bufferSize)
		return InvalidParameter;

	switch (region->type) {
	case RegionTypeRectF:
		*bufferSize = (sizeof (guint32) * 2) + (region->cnt * sizeof (GpRectF));
		break;
	case RegionTypePath:
		/* regiontype, tree */
		*bufferSize = sizeof (guint32) + gdip_region_get_tree_size (region->tree);
		break;
	default:
		g_warning ("unknown type %d", region->type);
		return NotImplemented;
	}
	return Ok;
}


GpStatus
GdipGetRegionData (GpRegion *region, BYTE *buffer, UINT bufferSize, UINT *sizeFilled)
{
	GpStatus status;
	UINT size;
	int len;

	if (!region || !buffer || !sizeFilled)
		return InvalidParameter;

	status = GdipGetRegionDataSize (region, &size);
	if (status != Ok)
		return status;
	if (size > bufferSize)
		return InsufficientBuffer;

	/* type of region */
	len = sizeof (guint32);
	memcpy (buffer, &region->type, len);
	buffer += len;
	*sizeFilled += len;

	switch (region->type) {
	case RegionTypeRectF:
		/* count (# rectangles) */
		memcpy (buffer, &region->cnt, len);
		buffer += len;
		*sizeFilled += len;
		/* rectangles */
		len = sizeof (GpRectF) * (region->cnt);
		memcpy (buffer, region->rects, len);
		*sizeFilled += len;
		break;
	case RegionTypePath:
		bufferSize -= len;
		if (!gdip_region_serialize_tree (region->tree, buffer, bufferSize, sizeFilled))
			return InsufficientBuffer;
		break;
	default:
		g_warning ("unknown type %d", region->type);
		return NotImplemented;
	}

	return Ok;
}

GpStatus
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
GpStatus
GdipCreateRegionHrgn (HRGN hRgn, GpRegion **region)
{
	if (!hRgn || !region)
		return InvalidParameter;

	return GdipCloneRegion ((GpRegion*) hRgn, region);
}
