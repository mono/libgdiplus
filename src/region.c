/*
 * Copyright (c) 2004 Ximian
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
 *          Jordi Mas i Hernandez <jordi@ximian.com>, 2004
 *
 */

#include "gdip.h"
#include "gdip_win32.h"

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

        array = malloc (sizeof (GpRectF) * (*elements + 1));
        memcpy (array, *srcarray, sizeof (GpRectF) * (*elements));

        if (*srcarray)
                free (*srcarray);

        next = array;
        next += (*elements);
        memcpy (next, rect, sizeof (GpRectF));        
        
        *srcarray = array;
        *elements = *elements + 1;           
}


BOOL
gdip_is_Point_in_RectF (float x, float y, GpRectF* rect)
{
        if ((x > rect->X && x < (rect->X + rect->Width))
                && (y > rect->Y && y < (rect->Y + rect->Height)))
                return TRUE;
        else
                return FALSE;
}

BOOL
gdip_is_Point_in_RectFs (float x, float y, GpRectF* r, int cnt)
{
        GpRectF* rect = r;
        int i;
        
        for (i = 0; i < cnt; i++, rect++) {
                if (gdip_is_Point_in_RectF (x, y, rect)) {
                        return TRUE;
                }
        }

        return FALSE;
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

BOOL
gdip_is_Point_in_RectFs_inclusive (float x, float y, GpRectF* r, int cnt)
{
        GpRectF* rect = r;
        int i;

        for (i = 0; i < cnt; i++, rect++) {
                if (gdip_is_Point_in_RectF_inclusive (x, y, rect)) 
                        return TRUE;                
        }

        return FALSE;
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
                break;
       
        default:
                GdipFree (result);
                return NotImplemented;
                
        }

	*region = result;   
	return Ok;              
}


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
        result->rects = (GpRectF *) malloc (sizeof (GpRectF) * region->cnt);
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
                free (region->rects);
                
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
                free (region->rects);
        
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

/* Complement*/
void
gdip_combine_complement (GpRegion *region, GpRectF *recttrg, int cnttrg)
{       
        GpRectF recthit;
        float x, y, hitx, hity, posx, posy;
        BOOL endrect, clean;
        GpRectF *rects = NULL, *rect;
        int cnt = 0, i, reg, step;
        BOOL found, done;
                
        /*
           Complement: the part of the second region not shared with the first region.
           Scans the region to be combined and store the rects not present in the region
        */  
        
        for (step = 0, rect = recttrg; step < cnttrg; step++, rect++) {

                x = y = 0;

                while (y < rect->Height + 1) {

                        /* First point of a rectangle candidate*/
                        if ((gdip_is_Point_in_RectFs (x + rect->X, y + rect->Y, region->rects, region->cnt) == FALSE) &&
                                gdip_is_Point_in_RectFs (x + rect->X, y + rect->Y, rects, cnt) == FALSE) {

                                endrect = FALSE;
                                hitx = recthit.X = x + rect->X;
                                hity = recthit.Y = y + rect->Y;
                                recthit.Height = recthit.Width = 0;
                
                                /* Try to get a rectangle going forward and then down*/

                                /* Go Down */
                                while (hity < (rect->Y + rect->Height +1) && endrect == FALSE) {

                                        clean = TRUE;

                                        /* Go from the beg to be sure that we have not a mach before*/
                                        if (recthit.Width) {
                                                for (i = rect->X; i < recthit.X ; i++) {
                                                        if (gdip_is_Point_in_RectFs_inclusive (i, hity, region->rects, region->cnt) == FALSE) {
                                                                clean = FALSE;
                                                                break;
                                                        }
                                                }
                                         }

                                        if (clean == FALSE)
                                                break;

                                        /* Go forward */
                                        while (hitx < rect->X + rect->Width +1) {
                                                if (gdip_is_Point_in_RectFs (hitx, hity, region->rects, region->cnt) == FALSE &&
                                                        gdip_is_Point_in_RectFs (hitx, hity, rects, cnt) == FALSE){

                                                        if (recthit.Height == 0) /* First row, it's a rect all the rest are equal*/
                                                                recthit.Width++;

                                                        hitx++;
                                                }
                                                else
                                                        break;
                                        }

                                        if (recthit.Height == 0) {
                                                hity++;
                                                recthit.Height++;
                                        }
                                        else {
                                                done = FALSE;

                                                if (hitx - recthit.X == recthit.Width) {
                                                        hity++;
                                                        recthit.Height++;
                                                        done = TRUE;

                                                }
                                                /* Smaller rect after first line*/
                                                if (!done && ((hitx - recthit.X < recthit.Width) && (hitx - recthit.X > 0)
                                                        && recthit.Height == 1))  {
                                                        recthit.Width = hitx - recthit.X;
                                                        hity++;
                                                        recthit.Height++;
                                                        done = TRUE;
                                                }

                                                if (!done) {

                                                        if (hitx - recthit.X >= recthit.Width)
                                                                recthit.Height++;

                                                        endrect = TRUE; /* Line length does not match previous one*/
                                                }
                                        }
                                        
                                hitx = x + rect->X;
                                
                                }  

                                if (recthit.Height)
                                        recthit.Height--;

                                if (recthit.Width)
                                        recthit.Width--;
          
                                found = TRUE;
                                /* Is this already contained */
                                for (posy = 0; posy < recthit.Height + 1; posy++) {

                                        for (posx = 0; posx < recthit.Width + 1; posx++) {
                                                if (gdip_is_Point_in_RectFs_inclusive (recthit.X + posx , recthit.Y + posy, rects, cnt) == FALSE) {
                                                        found = FALSE;
                                                 break;
                                                }
                                        }
                                }

                                if (found == FALSE && recthit.Width > 0 && recthit.Height > 0)
                                        gdip_add_rect_to_array (&rects, &cnt,  &recthit);
                               
                        }  /* end if*/

                        if (x < rect->Width +1)
                                x++;
                        else {
                                y++;
                                x = 0;
                        }
                }  
        }
        
        if (region->rects)
                free (region->rects);
        
        region->rects = rects;
        region->cnt = cnt;
}



/* CombineIntersect */
void
gdip_combine_intersect (GpRegion *region, GpRectF *recttrg, int cnttrg)
{
        GpRectF recthit;
        float x, y, hitx, hity;
        BOOL endrect, clean;
        GpRectF *rects = NULL;
        int cnt = 0, i, reg;
        GpRectF *rect = recttrg;                                                     

        /*
           Intersect: the part of the second region shared with the first region.
           Scans the region to be combined and store the rects not present in the region
        */

        for (reg = 0; reg < cnttrg; reg++, rect++) {

                x = y = 0;     

                while (y < rect->Height) {

                        /* First point of a rectangle candidate*/
                        if ((gdip_is_Point_in_RectFs (x + rect->X, y + rect->Y, region->rects, region->cnt) == TRUE) &&
                                gdip_is_Point_in_RectFs_inclusive (x + rect->X, y + rect->Y, rects, cnt) == FALSE) {

                                endrect = FALSE;
                                hitx = recthit.X = x + rect->X;
                                hity = recthit.Y = y + rect->Y;
                                recthit.Height = recthit.Width = 0;
                             
                                /* Try to get a rectangle going forward and then down*/

                                /* Go Down */
                                while (hity < (rect->Y + rect->Height) && endrect == FALSE) {

                                        clean = TRUE;

                                        /* Go from the beg to be sure that we have not a mach before*/
                                        if (recthit.Width) {
                                        for (i = rect->X; i < recthit.X ; i++)
                                                if (gdip_is_Point_in_RectFs (i, hity, region->rects, region->cnt) == TRUE) {
                                                        clean = FALSE;
                                                        break;
                                                }
                                        }

                                        if (clean == FALSE)
                                                break;

                                        /* Go forward */
                                        while (hitx < rect->X + rect->Width) {
                                                if (gdip_is_Point_in_RectFs (hitx, hity, region->rects, region->cnt) == TRUE) {

                                                        if (recthit.Height == 0) /* First row, it's a rect all the rest are equal*/
                                                                recthit.Width++;

                                                        hitx++;
                                                }
                                                else
                                                        break;
                                        }

                                        if (recthit.Height == 0) {
                                                hity++;
                                                recthit.Height++;
                                        }
                                        else {
                                                if (hitx - recthit.X == recthit.Width) {
                                                        hity++;
                                                        recthit.Height++;
                                                }
                                                else
                                                        endrect = TRUE; /* Line length does not match previous one*/
                                        }


                                        hitx = x + rect->X;
                                }  /* while */

                                /*Store hitrect as a part of this region*/
                                gdip_add_rect_to_array (&rects, &cnt,  &recthit);

                        }  /* end if*/

                        if (x < rect->Width)
                                x++;
                        else {
                                y++;
                                x = 0;
                        }
                }
        } /*for */  

        if (region->rects)        
                free (region->rects);
                
        region->rects = rects;
        region->cnt = cnt;
}

/* Exclude */
void
gdip_combine_exclude (GpRegion *region, GpRectF *recttrg, int cnttrg)
{
        GpRectF recthit;
        float x, y, hitx, hity, posx, posy;
        BOOL endrect, clean;
        GpRectF *rects = NULL, *rect;
        int cnt = 0, i, reg, step;
        BOOL found, done;

        /*
           Exclude: the part of the first region not shared with the second region.
        */

        for (step = 0, rect = region->rects; step < region->cnt; step++, rect++) {

                x = y = 0;
         
                while (y < rect->Height + 1) {

                        /* First point of a rectangle candidate*/
                        if ((gdip_is_Point_in_RectFs (x + rect->X, y + rect->Y, recttrg, cnttrg) == FALSE) &&
                                gdip_is_Point_in_RectFs (x + rect->X, y + rect->Y, rects, cnt) == FALSE) {

                                endrect = FALSE;
                                hitx = recthit.X = x + rect->X;
                                hity = recthit.Y = y + rect->Y;
                                recthit.Height = recthit.Width = 0;

                                /* Try to get a rectangle going forward and then down*/

                                /* Go Down */
                                while (hity < (rect->Y + rect->Height +1) && endrect == FALSE) {

                                        clean = TRUE;

                                        /* Go from the beg to be sure that we have not a mach before*/
                                        if (recthit.Width) {
                                                for (i = rect->X; i < recthit.X ; i++) {
                                                        if (gdip_is_Point_in_RectFs_inclusive (i, hity, recttrg, cnttrg) == FALSE) {
                                                                clean = FALSE;
                                                                break;
                                                        }
                                                }
                                         }

                                        if (clean == FALSE)
                                                break;

                                        /* Go forward */
                                        while (hitx < rect->X + rect->Width +1) {
                                                if (gdip_is_Point_in_RectFs (hitx, hity, recttrg, cnttrg) == FALSE &&
                                                        gdip_is_Point_in_RectFs (hitx, hity, rects, cnt) == FALSE){

                                                        if (recthit.Height == 0) /* First row, it's a rect all the rest are equal*/
                                                                recthit.Width++;

                                                        hitx++;
                                                }
                                                else
                                                        break;
                                        }

                                        if (recthit.Height == 0) {
                                                hity++;
                                                recthit.Height++;
                                        }
                                        else {
                                                done = FALSE;

                                                if (hitx - recthit.X == recthit.Width) {
                                                        hity++;
                                                        recthit.Height++;
                                                        done = TRUE;

                                                }
                                                /* Smaller rect after first line*/
                                                if (!done && ((hitx - recthit.X < recthit.Width) && (hitx - recthit.X > 0)
                                                        && recthit.Height == 1))  {
                                                        recthit.Width = hitx - recthit.X;
                                                        hity++;
                                                        recthit.Height++;
                                                        done = TRUE;
                                                }

                                                if (!done) {

                                                        if (hitx - recthit.X >= recthit.Width)
                                                                recthit.Height++;

                                                        endrect = TRUE; /* Line length does not match previous one*/
                                                }
                                        }

                                hitx = x + rect->X;

                                }

                                if (recthit.Height)
                                        recthit.Height--;

                                if (recthit.Width)
                                        recthit.Width--;

                                found = TRUE;
                                /* Is this already contained */
                                for (posy = 0; posy < recthit.Height + 1; posy++) {

                                        for (posx = 0; posx < recthit.Width + 1; posx++) {
                                                if (gdip_is_Point_in_RectFs_inclusive (recthit.X + posx , recthit.Y + posy, rects, cnt) == FALSE) {
                                                        found = FALSE;
                                                 break;
                                                }
                                        }
                                }

                                if (found == FALSE && recthit.Width > 1 && recthit.Height > 1)
                                        gdip_add_rect_to_array (&rects, &cnt,  &recthit);

                        }  /* end if*/

                        if (x < rect->Width +1)
                                x++;
                        else {
                                y++;
                                x = 0;
                        }
                }
        }

        if (region->rects)
                free (region->rects);

        region->rects = rects;
        region->cnt = cnt;
}
                                            


/* Union */
void
gdip_combine_union (GpRegion *region, GpRectF *recttrg, int cnttrg)
{
        GpRectF *allrects = NULL, *rects = NULL;
        GpRectF *rect, rectwhole, recthit;
        int allcnt = 0, cnt = 0, i;
        float posx, posy;
        float x = 0, y = 0, hitx, hity, step;
        BOOL endrect, clean, found, done;
    
        /* All the src and trg rects in a single array*/
        for (i = 0, rect = region->rects; i < region->cnt; i++, rect++) 
                gdip_add_rect_to_array (&allrects, &allcnt,  rect);

        for (i = 0, rect = recttrg; i < cnttrg; i++, rect++)
                gdip_add_rect_to_array (&allrects, &allcnt,  rect);

        if (allcnt == 0) {
                free (allrects);
                return;
        }

        gdip_get_bounds (allrects, allcnt, &rectwhole);
    
        while (y < rectwhole.Height+1) {

                /* First point of a rectangle candidate*/
                if ((gdip_is_Point_in_RectFs_inclusive (x + rectwhole.X, y + rectwhole.Y, allrects, allcnt) == TRUE) &&
                        gdip_is_Point_in_RectFs (x + rectwhole.X, y + rectwhole.Y, rects, cnt) == FALSE) {

                        endrect = FALSE;
                        hitx = recthit.X = x + rectwhole.X;
                        hity = recthit.Y = y + rectwhole.Y;
                        recthit.Height = recthit.Width = 0;  
        
                        /* Try to get a rectangle going forward and then down*/

                        /* Go Down */
                        while (hity < (rectwhole.Y + rectwhole.Height +1) /* Pixel vs. rect*/ && endrect == FALSE) {   
                                clean = TRUE;   
                                     
                                /* Go from the beg to be sure that we have not a mach before*/
                                if (recthit.Width)      {
                                        for (step = rectwhole.X; step < recthit.X ; step++) {
                                                if (gdip_is_Point_in_RectFs_inclusive (step, hity, allrects, allcnt) == TRUE) {
                                                        clean = FALSE;
                                                        break;
                                                }
                                        }

                                        if (clean == FALSE)
                                                break;
                                }

                                /* Go forward */
                                while (hitx < rectwhole.X + rectwhole.Width +1) /* Pixel vs. rect*/ {
        
                                        if (gdip_is_Point_in_RectFs_inclusive (hitx, hity, allrects, allcnt) == TRUE &&
                                                gdip_is_Point_in_RectFs (hitx, hity, rects, cnt) == FALSE){

                                                if (recthit.Height == 0) {/* First row, it's a rect all the rest are equal*/
                                                        recthit.Width++;
                                                }

                                                 hitx++;                                                       
                                        }
                                        else
                                                break;

                                }

                                    
                                if (recthit.Height == 0) {
                                        hity++;
                                        recthit.Height++;                                        
                                }
                                else {
                                        done = FALSE;
                                        
                                        if (hitx - recthit.X == recthit.Width) {
                                                hity++;
                                                recthit.Height++;
                                                done = TRUE;
                                        
                                        }
                                        /* Smaller rect after first line*/
                                        if (!done && ((hitx - recthit.X < recthit.Width) && (hitx - recthit.X > 0)
                                                && recthit.Height == 1))  {
                                                recthit.Width = hitx - recthit.X;
                                                hity++;
                                                recthit.Height++;
                                                done = TRUE;
                                        }

                                        if (!done) {
                                                                       
                                                if (hitx - recthit.X >= recthit.Width)
                                                        recthit.Height++;
                                                        
                                                endrect = TRUE; /* Line length does not match previous one*/                                                
                                        }                                        
                                }                                        
                                        
                                hitx = x + rectwhole.X;

                        } /* while (hity < (rectwhole.Y + rectwhole.Height) && endrect == FALSE) {   */

                        found = TRUE;
                        
                        if (recthit.Height)
                                recthit.Height--;

                        if (recthit.Width)
                                recthit.Width--;
                                                                             
                        /* Is this already contained */
                        for (posy = 0; posy < recthit.Height+1; posy++) {

                                for (posx = 0; posx < recthit.Width +1; posx++) {
                                        if (gdip_is_Point_in_RectFs_inclusive (recthit.X + posx , recthit.Y + posy, rects, cnt) == FALSE) {
                                                found = FALSE;
                                                break;
                                        }
                                }
                        }      
                        
                        if (found == FALSE) 
                                gdip_add_rect_to_array (&rects, &cnt,  &recthit);
                                     
                        
                }  /*if ((gdip_is_Point_in_RectFs_i...*/
                
                if (x < rectwhole.Width +1)
                        x++;
                else {
                        y++;
                        x = 0;
                }
        }  /* End while (y < rectwhole.Height)*/
   
        if (region->rects)
                free (region->rects);

        region->rects = rects;
        region->cnt = cnt;        
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
                free (region->rects);

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

      if (region->cnt != 1) {
              *result = FALSE;
              return Ok;
      }

      if (region->rects->X == -4194304 && region->rects->Y == -4194304 &&
                region->rects->Width == 8388608 &&  region->rects->Height == 8388608)
                *result = TRUE;
      else
                *result = FALSE;

      return Ok;
}


GpStatus
GdipIsVisibleRegionPoint (GpRegion *region, float x, float y, GpGraphics *graphics, BOOL *result)
{

        if (!region || !result)
                return InvalidParameter;

        *result = gdip_is_Point_in_RectFs_inclusive (x, y, region->rects, region->cnt);

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
        for (posy = 0; posy < recthit.Height+1; posy++) {

                for (posx = 0; posx < recthit.Width +1; posx++) {
                        if (gdip_is_Point_in_RectFs_inclusive (recthit.X + posx , recthit.Y + posy, region->rects, region->cnt) == TRUE) {
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


