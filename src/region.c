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
        
        printf ("add_rect_to_array %u, %f, %f, %f, %f,\n", *elements, rect->X, rect->Y, rect->Width, rect->Height); 
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
/*                        printf ("gdip_is_Point_in_RectFs %f, %f TRUE\n", x,y);*/
                        return TRUE;
                }
        }

       /* printf ("gdip_is_Point_in_RectFs %f, %f FALSE\n", x,y); */
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
                if (gdip_is_Point_in_RectF_inclusive (x, y, rect)) {
/*                        printf ("gdip_is_Point_in_RectFs %f, %f TRUE\n", x,y);*/
                        return TRUE;
                }
        }

       /* printf ("gdip_is_Point_in_RectFs %f, %f FALSE\n", x,y); */
        return FALSE;
}



GpStatus
gdip_createRegion (GpRegion **region, RegionType type, void *src)
{
        GpRegion *result;
        GpRectF rect;

        result = (GpRegion *) GdipAlloc (sizeof (GpRegion));

        printf ("createregion type %u\n", type);

        switch (type) {
        case RegionTypeRect:
                result->rects = NULL;
                result->cnt = 0;               
                gdip_from_Rect_To_RectF ((GpRect *)src, &rect);
                gdip_add_rect_to_array (&result->rects, &result->cnt,  &rect);
                printf ("created region %x\n", result);
                break;
        default:
                return NotImplemented;
                
        }

	*region = result;

	return Ok;

}


GpStatus
GdipCreateRegion (GpRegion **region)
{
        GpRegion *result;

        /*TODO: act as clone region*/
        result = (GpRegion *) GdipAlloc (sizeof (GpRegion));
        result->rects = NULL;
        result->cnt = 0;
        *region = result;
      
	return Ok;              
}


GpStatus
GdipCreateRegionRect (GDIPCONST GpRectF *rect, GpRegion **region)
{
        GpStatus status;

        status = gdip_createRegion (region, RegionTypeRectF, (void*) rect);

        if (status != Ok)
                return status;

        return Ok;
}


GpStatus
GdipCreateRegionRectI (GDIPCONST GpRect *rect, GpRegion **region)
{
        GpStatus status;

        status = gdip_createRegion (region, RegionTypeRect, (void*) rect);

        if (status != Ok)
                return status;

        return Ok;
}


GpStatus
GdipCreateRegionPath (GpPath *path, GpRegion **region)
{

        return NotImplemented;
}


GpStatus
GdipCloneRegion (GpRegion *region, GpRegion **cloneRegion)
{
        GpRegion *result = (GpRegion *) GdipAlloc (sizeof (GpRegion));

        memcpy (result, region, sizeof (GpRegion));

        /* TODO: Copy src allocated memory*/

        *cloneRegion = result;
        return Ok; 
        
}



GpStatus
GdipDeleteRegion (GpRegion *region)
{
        if (!region)
                return InvalidParameter;

        free (region->rects);
        GdipFree (region);
        
        return NotImplemented;
}


GpStatus
GdipSetInfinite (GpRegion *region)
{

        return NotImplemented;
}


GpStatus
GdipSetEmpty (GpRegion *region)
{

        return NotImplemented;
}


GpStatus
GdipCombineRegionRect (GpRegion *region, GDIPCONST GpRectF *rect, CombineMode combineMode)
{

        return NotImplemented;
}

/* Complement*/
void
gdip_combine_complement (GpRegion *region, GpRectF *rect)
{       
        GpRectF recthit;
        float x = 0, y = 0, hitx, hity;
        BOOL endrect, clean;
        GpRectF *rects = NULL;
        int cnt = 0, i;                                 
                
        /*
           Complement: the part not shared with the region.
           Scans the region to be combined and store the rects not present in the region
        */

        while (y < rect->Height) {

                /* First point of a rectangle candidate*/
                if ((gdip_is_Point_in_RectFs (x + rect->X, y + rect->Y, region->rects, region->cnt) == FALSE) &&
                     gdip_is_Point_in_RectFs_inclusive (x + rect->X, y + rect->Y, rects, cnt) == FALSE) {

                        endrect = FALSE;
                        hitx = recthit.X = x + rect->X;
                        hity = recthit.Y = y + rect->Y;
                        recthit.Height = recthit.Width = 0;
                        printf ("point 0->%f, %f\n", hitx, hity);

                        /* Try to get a rectangle going forward and then down*/

                        /* Go Down */
                        while (hity < (rect->Y + rect->Height) && endrect == FALSE) {

                                clean = TRUE;

                                /* Go from the beg to be sure that we have not a mach before*/
                                if (recthit.Width) {
                                for (i = rect->X; i < recthit.X ; i++)
                                        if (gdip_is_Point_in_RectFs (i, hity, region->rects, region->cnt) == FALSE) {
                                                 clean = FALSE;
                                                 printf("hit at %f\n", i);
                                                 break;
                                        }
                                 }

                                if (clean == FALSE)
                                        break;

                                /* Go forward */
                                while (hitx < rect->X + rect->Width) {
                                        if (gdip_is_Point_in_RectFs (hitx, hity, region->rects, region->cnt) == FALSE) {

                                                if (recthit.Height == 0) /* First row, it's a rect all the rest are equal*/
                                                        recthit.Width++;

                                                hitx++;
                                        }
                                                else
                                                        break;
                                }

                               
                                printf("point 2 %f(x), %f(y), %f, %f(w), %f(h)\n", hitx, hity, hitx - recthit.X, recthit.Width, recthit.Height);
                                

                                if (recthit.Height == 0) {
                                        hity++;
                                        recthit.Height++;
                                        printf ("line len: %f\n", recthit.Width);
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

                        printf ("store %f, %f, %f, %f\n", recthit.X, recthit.Y, recthit.Width, recthit.Height);
                        /*Store hitrect as a part of this region*/
                        gdip_add_rect_to_array (&rects, &cnt,  &recthit);

                }  /* end if*/

                /*printf("main\n");*/
                if (x < rect->Width)
                        x++;
                else {
                        y++;
                        x = 0;
                }
        }


        free (region->rects);
        region->rects = rects;
        region->cnt = cnt;
}


/* Exclude */
void
gdip_combine_exclude (GpRegion *region, GpRectF *rect)
{
        GpRectF recthit;
        float x = 0, y = 0, hitx, hity;
        BOOL endrect, clean, found;
        GpRectF *rects = NULL, *regrect;
        int cnt = 0, i, n, posx, posy;

        /*
           Complement: the part not shared with the region.
           Scans the region to be combined and store the rects not present in the region
        */

        printf("rects: %u\n", region->cnt);
        for (n = 0, regrect = region->rects; n <  region->cnt; regrect++, n++) {

        while (y < regrect->Height) {

                /* First point of a rectangle candidate*/
                if ((gdip_is_Point_in_RectF_inclusive (x + regrect->X, y + regrect->Y, rect) == FALSE) &&
                     gdip_is_Point_in_RectFs (x + regrect->X, y + regrect->Y, rects, cnt) == FALSE) {

                        endrect = FALSE;
                        hitx = recthit.X = x + regrect->X;
                        hity = recthit.Y = y + regrect->Y;
                        recthit.Height = recthit.Width = 0;
                        /*
                        printf ("point 0->%f, %f\n", hitx, hity);*/

                        /* Try to get a rectangle going forward and then down*/

                        /* Go Down */
                        while (hity < (regrect->Y + regrect->Height) && endrect == FALSE) {

                                clean = TRUE;

                                /* Go from the beg to be sure that we have not a mach before*/
                                if (recthit.Width) {
                                for (i = regrect->X; i < recthit.X ; i++)
                                        if (gdip_is_Point_in_RectF_inclusive (i, hity, rect) == FALSE) {
                                                 clean = FALSE;
                                                 printf("hit at %u\n", i); 
                                                 break;
                                        }
                                 }

                                if (clean == FALSE)
                                        break;

                                /* Go forward */
                                while (hitx < regrect->X + regrect->Width) {
                                        if (gdip_is_Point_in_RectF_inclusive (hitx, hity, rect) == FALSE) {

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
                                        /*printf ("line len: %f\n", recthit.Width);*/
                                }
                                else {
                                        if (hitx - recthit.X == recthit.Width) {
                                                hity++;
                                                recthit.Height++;
                                        }
                                        else {
                                                endrect = TRUE; /* Line length does not match previous one*/
                                                /*printf ("endred->%f\n", recthit.Height);  */
                                        }
                                }

                                /*
                                printf("point 2 %f(x), %f(y), %f, %f(w), %f(h)\n", hitx, hity, hitx - recthit.X, recthit.Width, recthit.Height);
                                */

                                hitx = x + regrect->X;
                        }  /* while */

                        found = TRUE;
                        /* Is this already contained */
                        for (posy = 0; posy < recthit.Height; posy++) {

                              for (posx = 0; posx < recthit.Width; posx++) {
                                   if (gdip_is_Point_in_RectFs_inclusive (recthit.X + posx , recthit.Y + posy, rects, cnt) == FALSE) {
                                        found = FALSE;
                                        break;
                                   }
                              }     
                        }

                        if (found == FALSE) {                        
                                printf ("store %f, %f, %f, %f\n", recthit.X, recthit.Y, recthit.Width, recthit.Height);
                                /*Store hitrect as a part of this region*/
                                gdip_add_rect_to_array (&rects, &cnt,  &recthit);
                        }

                }  /* end if*/

                /*printf("main\n");*/
                if (x < regrect->Width)
                        x++;
                else {
                        y++;
                        x = 0;
                }
        }

}
        free (region->rects);
        region->rects = rects;
        region->cnt = cnt;
}

GpStatus
GdipCombineRegionRectI (GpRegion *region, GDIPCONST GpRect *recti, CombineMode combineMode)
{
        GpRectF rect;

        if (!region || !recti)
                return InvalidParameter;

        printf ("GdipCombineRegionRectI %x %x %u\n", region, recti, combineMode);

   /*     if (combineMode != CombineModeComplement)
               return NotImplemented; */

        printf("rect->x=%u, y=%u, w=%u, h=%u\n", recti->X, recti->Y, recti->Width, recti->Height);

        gdip_from_Rect_To_RectF ((GpRect *) recti, &rect);
        
        switch (combineMode) {
        case CombineModeExclude:
                gdip_combine_exclude (region, &rect);
                break;
        case CombineModeComplement:
                gdip_combine_complement (region, &rect);
                break;
        default:
               return NotImplemented;
                                    
        }

        return Ok;
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

        printf ("GdipCombineRegionRegion %x %x %u\n", region, region2, combineMode);
        
        switch (combineMode) {
        case CombineModeExclude:
                break;
        case CombineModeComplement:
                break;
        default:
               return NotImplemented;

        }

        return Ok;
}


GpStatus
GdipTransformRegion (GpRegion *region, GpMatrix *matrix)
{

        return NotImplemented;
}


GpStatus
GdipGetRegionBounds (GpRegion *region, GpGraphics *graphics, GpRectF *rect)
{

        return NotImplemented;
}


GpStatus
GdipIsEmptyRegion (GpRegion *region, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsInfiniteRegion (GpRegion *region, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionPoint (GpRegion *region, float x, float y, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionPointI (GpRegion *region, int x, int y, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionRect (GpRegion *region, float x, float y, float width, float height, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
}


GpStatus
GdipIsVisibleRegionRectI (GpRegion *region, int x, int y, int width, int height, GpGraphics *graphics, BOOL *result)
{

        return NotImplemented;
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

        printf ("GdipGetRegionScans %u\n", *count);

        memcpy (rects, region->rects, sizeof (GpRectF) * *count);
        *count = region->cnt;
        return Ok;              
}


