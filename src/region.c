/*
 * Copyright (c) 2004-2005 Ximian
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
 * Author:
 *          Jordi Mas i Hernandez <jordi@ximian.com>, 2004-2005
 *
 */

#include "gdip.h"


/*
	Helper functions
*/

void
gdip_from_Rect_To_RectF (GpRect* rect, GpRectF* rectf)
{
        rectf->X = rect->X;
        rectf->Y = rect->Y;
        rectf->Width = rect->Width;
        rectf->Height = rect->Height;
}

void
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

BOOL
gdip_is_Point_in_RectF_Visible (float x, float y, GpRectF* rect)
{
        if ((x >= rect->X && x < (rect->X + rect->Width))
                && (y >= rect->Y && y < (rect->Y + rect->Height)))
                return TRUE;
        else
                return FALSE;
}

BOOL
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

bool
gdip_is_InfiniteRegion (GpRegion *region)
{
	if (region->cnt != 1)
              return FALSE;

	if (region->rects->X == -4194304 && region->rects->Y == -4194304 &&
		region->rects->Width == 8388608 &&  region->rects->Height == 8388608)
		return TRUE;

	return FALSE;
}

BOOL
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
BOOL
gdip_contains (GpRectF *rect1, GpRectF *rect2)
{
	return (rect1->X >= rect2->X &&
		rect1->X + rect1->Width <= rect2->X + rect2->Width &&
		rect1->Y >= rect2->Y &&
		rect1->Y + rect1->Height <= rect2->Y + rect2->Height);
}

BOOL
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


BOOL
gdip_equals (GpRectF *rect1, GpRectF *rect2)
{
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
BOOL
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

GpStatus
gdip_createRegion (GpRegion **region, RegionType type, void *src)
{
        GpRegion *result;
        GpRectF rect;

        result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
        result->rects = NULL;
        result->cnt = 0;

        switch (type) {
        case RegionTypeRect:
                gdip_from_Rect_To_RectF ((GpRect *)src, &rect);
                gdip_add_rect_to_array (&result->rects, &result->cnt,  &rect);
                break;
        case RegionTypeRectF:
                gdip_add_rect_to_array (&result->rects, &result->cnt,  (GpRectF *)src);
                break;
        case RegionTypeEmpty:
		GdipSetInfinite (result);
                break;

        default:
                GdipFree (result);
                return NotImplemented;

        }

	*region = result;
	return Ok;
}

/*
	API implementation
*/

GpStatus
GdipCreateRegion (GpRegion **region)
{
        if (!region)
                return InvalidParameter;

        return gdip_createRegion (region, RegionTypeEmpty, NULL);
}


GpStatus
GdipCreateRegionRect (GDIPCONST GpRectF *rect, GpRegion **region)
{
        if (!region || !rect)
                return InvalidParameter;

        return gdip_createRegion (region, RegionTypeRectF, (void*) rect);
}


GpStatus
GdipCreateRegionRectI (GDIPCONST GpRect *rect, GpRegion **region)
{
        if (!region || !rect)
                return InvalidParameter;

        return gdip_createRegion (region, RegionTypeRect, (void*) rect);
}


GpStatus
GdipCloneRegion (GpRegion *region, GpRegion **cloneRegion)
{
        GpRegion *result;

        if (!region || !cloneRegion)
                return InvalidParameter;

        result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
        memcpy (result, region, sizeof (GpRegion));
        result->rects = (GpRectF *) GdipAlloc (sizeof (GpRectF) * region->cnt);
        memcpy (result->rects, region->rects, sizeof (GpRectF) * region->cnt);
        *cloneRegion = result;

        return Ok;
}


GpStatus
GdipDeleteRegion (GpRegion *region)
{
        if (!region)
                return InvalidParameter;

        if (region->rects)
                GdipFree (region->rects);

        GdipFree (region);

        return Ok;
}


GpStatus
GdipSetInfinite (GpRegion *region)
{
        GpRectF rect;

        if (!region)
                return InvalidParameter;

        GdipSetEmpty (region);

        rect.X = rect.Y = -4194304;
        rect.Width = rect.Height= 8388608;

        gdip_add_rect_to_array (&region->rects, &region->cnt,  &rect);
        return Ok;
}


GpStatus
GdipSetEmpty (GpRegion *region)
{
        if (!region)
                return InvalidParameter;

        if (region->rects)
                GdipFree (region->rects);

        region->rects = NULL;
        region->cnt = 0;

        return Ok;
}

void
gdip_get_bounds (GpRectF *allrects, int allcnt, GpRectF *bound)
{
        float nx, ny, fx, fy, posx, posy;
        float x = 0, y = 0, hitx, hity, step;
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




/* Exclude */
void
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
        for (i = 0, rect = rtrg; i < cntt; i++, rect++)
                gdip_add_rect_to_array (&alltrgrects, &alltrgcnt, rect);

	/* Init current with the first element in the array */
	current.X = -1; current.Y = -1;
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

		if (storecomplete) {
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
void
gdip_combine_complement (GpRegion *region, GpRectF *rtrg, int cntt)
{
	GpRegion regsrc;
	GpRectF* trg, *rect;
	GpRectF* allsrcrects = NULL;
        int allsrccnt = 0, i,  trgcnt;

	/* Create the list of source rectangles to process */
        for (i = 0, rect = rtrg; i < cntt; i++, rect++)
                gdip_add_rect_to_array (&allsrcrects, &allsrccnt, rect);

	regsrc.rects = allsrcrects;
	regsrc.cnt = allsrccnt;
	trg = region->rects;
	trgcnt = region->cnt;

	gdip_combine_exclude (&regsrc, trg, trgcnt);


	if (region->rects == allsrcrects) {
		GdipFree (region->rects); }
	else {
		region->rects = regsrc.rects;
		region->cnt = regsrc.cnt;
	}
}


/* Union */
void
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

        for (i = 0, rect = rtrg; i < cnttrg; i++, rect++)
                gdip_add_rect_to_array (&allrects, &allcnt,  rect);

        if (allcnt == 0) {
                GdipFree (allrects);
                return;
        }

	/* Init current with the first element in the array */
	current.X = -1; current.Y = -1;
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
void
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
			/* Intersects With */
			if (rectsrc->X >= recttrg->X + recttrg->Width ||
				rectsrc->X + rectsrc->Width <= recttrg->X ||
				rectsrc->Y >= recttrg->Y + recttrg->Height ||
				rectsrc->Y + rectsrc->Height <= recttrg->Y) { /* BUG, TODO: Re-test*/
				continue;
			}
			/* Area that intersects */
			rectcur.X = rectsrc->X > recttrg->X ? rectsrc->X : recttrg->X;
			rectcur.Y = rectsrc->Y > recttrg->Y ? rectsrc->Y : recttrg->Y;
			rectcur.Width = rectsrc->X + rectsrc->Width < recttrg->X + recttrg->Width ?
				rectsrc->X + rectsrc->Width - rectcur.X : recttrg->X + recttrg->Width - rectcur.X;

			rectcur.Height = rectsrc->Y + rectsrc->Height < recttrg->Y + recttrg->Height ?
				rectsrc->Y + rectsrc->Height - rectcur.Y : recttrg->Y + recttrg->Height - rectcur.Y;

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
void
gdip_combine_xor (GpRegion *region, GpRectF *recttrg, int cnttrg)
{
        GpRegion *rgnsrc;  /* All rectangles of both regions*/
        GpRegion *rgntrg;  /* Only the ones that intersect*/
        GpRectF *allrects = NULL, *rect;
        int allcnt = 0, i;

        /* All the src and trg rects in a single array*/
        for (i = 0, rect = region->rects; i < region->cnt; i++, rect++)
                gdip_add_rect_to_array (&allrects, &allcnt,  rect);

        for (i = 0, rect = recttrg; i < cnttrg; i++, rect++)
                gdip_add_rect_to_array (&allrects, &allcnt,  rect);

        gdip_createRegion (&rgnsrc, RegionTypeEmpty, NULL);
        rgnsrc->cnt = allcnt;
        rgnsrc->rects = allrects;

        GdipCloneRegion (region, &rgntrg);
        gdip_combine_intersect (rgntrg, recttrg, cnttrg);

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

	if (gdip_is_InfiniteRegion (region))
		return Ok;

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


GpStatus
GdipCombineRegionPath (GpRegion *region, GpPath *path, CombineMode combineMode)
{
        return NotImplemented;
}


GpStatus
GdipCombineRegionRegion (GpRegion *region,  GpRegion *region2, CombineMode combineMode)
{
        if (!region || !region2)
                return InvalidParameter;

	if (gdip_is_InfiniteRegion (region) || gdip_is_InfiniteRegion(region2))
		return Ok;

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

        gdip_get_bounds (region->rects , region->cnt, rect);

        return Ok;
}


GpStatus
GdipIsEmptyRegion (GpRegion *region, GpGraphics *graphics, BOOL *result)
{
        GpRectF rect;

        if (!region || !graphics || !result)
                return InvalidParameter;

        gdip_get_bounds (region->rects, region->cnt, &rect);

        if (rect.Width ==0  || rect.Height ==0)
                *result = TRUE;
        else
                *result = FALSE;

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

        *result = gdip_is_Point_in_RectFs_Visible (x, y, region->rects, region->cnt);

        return Ok;
}


GpStatus
GdipIsVisibleRegionPointI (GpRegion *region, int x, int y, GpGraphics *graphics, BOOL *result)
{
        float xf, yf;

        if (!region || !result)
                return InvalidParameter;

        xf = x;
        yf = y;

        return GdipIsVisibleRegionPoint (region, xf, yf, graphics, result);
}


GpStatus
GdipIsVisibleRegionRect (GpRegion *region, float x, float y, float width, float height, GpGraphics *graphics, BOOL *result)
{
        BOOL found = FALSE;
        float posy, posx;
        GpRectF recthit;

        if (!region || !result)
                return InvalidParameter;

        if (width ==0 || height ==0) {
                *result = FALSE;
                return Ok;
        }

        recthit.X = x; recthit.Y = y;
        recthit.Width = width; recthit.Height = height;

        /* Any point of intersection ?*/
        for (posy = 0; posy < recthit.Height && found == FALSE; posy++) {

                for (posx = 0; posx < recthit.Width ; posx++) {
                        if (gdip_is_Point_in_RectFs_Visible (recthit.X + posx , recthit.Y + posy, region->rects, region->cnt) == TRUE) {
                                found = TRUE;
                                break;
                        }
		}
        }

        *result = found;
        return Ok;
}


GpStatus
GdipIsVisibleRegionRectI (GpRegion *region, int x, int y, int width, int height, GpGraphics *graphics, BOOL *result)
{
        float fx, fy, fw, fh;

        if (!region || !result)
                return InvalidParameter;

        fx = x; fy = y;
        fh = height; fw = width;

        return GdipIsVisibleRegionRect (region, fx, fy, fw, fh, graphics, result);
}


GpStatus
GdipGetRegionScansCount (GpRegion *region, int* count, GpMatrix* matrix)
{
        if (!region || !count)
                return InvalidParameter;

        *count = region->cnt;
        return Ok;
}

GpStatus
GdipGetRegionScans (GpRegion *region, GpRectF* rects, int* count, GpMatrix* matrix)
{
        if (!region || !rects|| !count)
                return InvalidParameter;

        memcpy (rects, region->rects, sizeof (GpRectF) * *count);
        *count = region->cnt;
        return Ok;
}

GpStatus
GdipIsEqualRegion (GpRegion *region, GpRegion *region2, GpGraphics *graphics, BOOL *result)
{
        int i;
        GpRectF *rectsrc, *recttrg;

        if (!region || !region2 || !graphics || !result)
                return InvalidParameter;

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
        int i;
        GpRectF *rect;

        if (!region)
                return InvalidParameter;

        if (region->rects) {

                for (i = 0, rect=region->rects ; i < region->cnt; i++, rect++) {
                        rect->X += dx;
                        rect->Y += dy;
                }
        }

        return Ok;
}

GpStatus
GdipTranslateRegionI (GpRegion *region, int dx, int dy)
{
        float fx, fy;

        if (!region)
                return InvalidParameter;

        fx = dx; fy = dy;
        return  GdipTranslateRegion (region, fx, fy);
}

GpStatus
GdipTransformRegion (GpRegion *region, GpMatrix *matrix)
{
        return NotImplemented;
}

GpStatus
GdipCreateRegionPath (GpPath *path, GpRegion **region)
{
        return NotImplemented;
}


GpStatus
GdipGetRegionDataSize (GpRegion *region, UINT * bufferSize)
{
        return NotImplemented;
}


GpStatus
GdipGetRegionData(GpRegion *region, BYTE * buffer, UINT bufferSize, UINT *sizeFilled)
{
        return NotImplemented;
}


