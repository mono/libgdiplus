/*
 * Copyright © 2000 SuSE, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of SuSE not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  SuSE makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SuSE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SuSE
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, SuSE, Inc.
 */

#include "pixman-xserver-compat.h"

pixman_image_t *
pixman_image_create (pixman_format_t	*format,
	       int	width,
	       int	height)
{
    pixman_image_t	*image;
    FbPixels	*pixels;

    pixels = FbPixelsCreate (width, height, format->depth);
    if (pixels == NULL)
	return NULL;
    
    image = pixman_image_createForPixels (pixels, format);
    if (image == NULL) {
	FbPixelsDestroy (pixels);
	return NULL;
    }

    image->owns_pixels = 1;

    return image;
}
slim_hidden_def(pixman_image_create);

pixman_image_t *
pixman_image_create_for_data (FbBits *data, pixman_format_t *format, int width, int height, int bpp, int stride)
{
    pixman_image_t	*image;
    FbPixels	*pixels;

    pixels = FbPixelsCreateForData (data, width, height, format->depth, bpp, stride);
    if (pixels == NULL)
	return NULL;

    image = pixman_image_createForPixels (pixels, format);
    if (image == NULL) {
	FbPixelsDestroy (pixels);
	return NULL;
    }

    image->owns_pixels = 1;

    return image;
}

pixman_image_t *
pixman_image_createForPixels (FbPixels	*pixels,
			pixman_format_t	*format)
{
    pixman_image_t		*image;

    image = malloc (sizeof (pixman_image_t));
    if (!image)
    {
	return NULL;
    }

    image->pixels = pixels;
    image->image_format = *format;
    image->format_code = format->format_code;
/* XXX: What's all this about?
    if (pDrawable->type == DRAWABLE_PIXMAP)
    {
	++((PixmapPtr)pDrawable)->refcnt;
	image->pNext = 0;
    }
    else
    {
	image->pNext = GetPictureWindow(((WindowPtr) pDrawable));
	SetPictureWindow(((WindowPtr) pDrawable), image);
    }
*/

    pixman_image_init (image);

    return image;
}

void
pixman_image_init (pixman_image_t *image)
{
    image->refcnt = 1;
    image->repeat = 0;
    image->graphicsExposures = 0;
    image->subWindowMode = ClipByChildren;
    image->polyEdge = PolyEdgeSharp;
    image->polyMode = PolyModePrecise;
    /* 
     * In the server this was 0 because the composite clip list
     * can be referenced from a window (and often is)
     */
    image->freeCompClip = 0;
    image->freeSourceClip = 0;
    image->clientClipType = CT_NONE;
    image->componentAlpha = 0;
    image->compositeClipSource = 0;

    image->alphaMap = 0;
    image->alphaOrigin.x = 0;
    image->alphaOrigin.y = 0;

    image->clipOrigin.x = 0;
    image->clipOrigin.y = 0;
    image->clientClip = 0;

    image->dither = 0L;

    image->stateChanges = (1 << (CPLastBit+1)) - 1;
/* XXX: What to lodge here?
    image->serialNumber = GC_CHANGE_SERIAL_BIT;
*/

    image->pCompositeClip = pixman_region_create();
    pixman_region_union_rect (image->pCompositeClip, image->pCompositeClip,
			0, 0, image->pixels->width, image->pixels->height);
    image->freeCompClip = 1;

    image->pSourceClip = pixman_region_create ();
    pixman_region_union_rect (image->pSourceClip, image->pSourceClip,
			0, 0, image->pixels->width, image->pixels->height);
    image->freeSourceClip = 1;
    
    image->transform = NULL;

    image->filter = PIXMAN_FILTER_NEAREST;
    image->filter_params = 0;
    image->filter_nparams = 0;


    image->owns_pixels = 0;
}

void
pixman_image_set_component_alpha (pixman_image_t	*image,
				  int		component_alpha)
{
    if (image)
	image->componentAlpha = component_alpha;
}
slim_hidden_def(pixman_image_set_component_alpha);

int
pixman_image_set_transform (pixman_image_t		*image,
		     pixman_transform_t	*transform)
{
    static const pixman_transform_t	identity = { {
	{ xFixed1, 0x00000, 0x00000 },
	{ 0x00000, xFixed1, 0x00000 },
	{ 0x00000, 0x00000, xFixed1 },
    } };

    if (transform && memcmp (transform, &identity, sizeof (pixman_transform_t)) == 0)
	transform = 0;
    
    if (transform)
    {
	if (!image->transform)
	{
	    image->transform = malloc (sizeof (pixman_transform_t));
	    if (!image->transform)
		return 1;
	}
	*image->transform = *transform;
    }
    else
    {
	if (image->transform)
	{
	    free (image->transform);
	    image->transform = 0;
	}
    }
    return 0;
}

void
pixman_image_set_repeat (pixman_image_t	*image,
		  int		repeat)
{
    if (image)
	image->repeat = repeat;
}
slim_hidden_def(pixman_image_set_repeat);

void
pixman_image_set_filter (pixman_image_t	*image,
		  pixman_filter_t	filter)
{
    if (image)
	image->filter = filter;
}

int
pixman_image_get_width (pixman_image_t	*image)
{
    return image->pixels->width;
}

int
pixman_image_get_height (pixman_image_t	*image)
{
    return image->pixels->height;
}

int
pixman_image_get_depth (pixman_image_t	*image)
{
    return image->pixels->depth;
}

int
pixman_image_get_stride (pixman_image_t	*image)
{
    return image->pixels->stride;
}

pixman_format_t *
pixman_image_get_format (pixman_image_t	*image)
{
    return &image->image_format;
}

FbBits *
pixman_image_get_data (pixman_image_t	*image)
{
    return image->pixels->data;
}

void
pixman_image_destroy (pixman_image_t *image)
{
    pixman_image_destroyClip (image);

    if (image->freeCompClip) {
	pixman_region_destroy (image->pCompositeClip);
	image->pCompositeClip = NULL;
    }
    
    if (image->freeSourceClip) {
	pixman_region_destroy (image->pSourceClip);
	image->pSourceClip = NULL;
    }

    if (image->owns_pixels) {
	FbPixelsDestroy (image->pixels);
	image->pixels = NULL;
    }

    if (image->transform) {
	free (image->transform);
	image->transform = NULL;
    }

    free (image);
}
slim_hidden_def(pixman_image_destroy);

void
pixman_image_destroyClip (pixman_image_t *image)
{
    switch (image->clientClipType) {
    case CT_NONE:
	return;
    case CT_PIXMAP:
	pixman_image_destroy (image->clientClip);
	break;
    default:
	pixman_region_destroy (image->clientClip);
	break;
    }
    image->clientClip = NULL;
    image->clientClipType = CT_NONE;
}    

int
pixman_image_set_clip_region (pixman_image_t	*image,
			      pixman_region16_t	*region)
{
    pixman_image_destroyClip (image);
    if (region) {
	image->clientClip = pixman_region_create ();
	pixman_region_copy (image->clientClip, region);
	image->clientClipType = CT_REGION;
    }
    
    if (image->freeCompClip)
	pixman_region_destroy (image->pCompositeClip);
    image->pCompositeClip = pixman_region_create();
    pixman_region_union_rect (image->pCompositeClip, image->pCompositeClip,
			      0, 0, image->pixels->width, image->pixels->height);
    image->freeCompClip = 1;
    if (region) {
	pixman_region_translate (image->pCompositeClip,
				 - image->clipOrigin.x,
				 - image->clipOrigin.y);
	pixman_region_intersect (image->pCompositeClip,
				 image->pCompositeClip,
				 region);
	pixman_region_translate (image->pCompositeClip,
				 image->clipOrigin.x,
				 image->clipOrigin.y);
    }
    
    image->stateChanges |= CPClipMask;
    return 0;
}

#define BOUND(v)	(int16_t) ((v) < MINSHORT ? MINSHORT : (v) > MAXSHORT ? MAXSHORT : (v))

static __inline int
FbClipImageReg (pixman_region16_t	*region,
		pixman_region16_t	*clip,
		int		dx,
		int		dy)
{
    if (pixman_region_num_rects (region) == 1 &&
	pixman_region_num_rects (clip) == 1)
    {
	pixman_box16_t *pRbox = pixman_region_rects (region);
	pixman_box16_t *pCbox = pixman_region_rects (clip);
	int	v;

	if (pRbox->x1 < (v = pCbox->x1 + dx))
	    pRbox->x1 = BOUND(v);
	if (pRbox->x2 > (v = pCbox->x2 + dx))
	    pRbox->x2 = BOUND(v);
	if (pRbox->y1 < (v = pCbox->y1 + dy))
	    pRbox->y1 = BOUND(v);
	if (pRbox->y2 > (v = pCbox->y2 + dy))
	    pRbox->y2 = BOUND(v);
	if (pRbox->x1 >= pRbox->x2 ||
	    pRbox->y1 >= pRbox->y2)
	{
	    pixman_region_empty (region);
	}
    }
    else
    {
	pixman_region_translate (region, dx, dy);
	pixman_region_intersect (region, clip, region);
	pixman_region_translate (region, -dx, -dy);
    }
    return 1;
}
		  
static __inline int
FbClipImageSrc (pixman_region16_t	*region,
		pixman_image_t		*image,
		int		dx,
		int		dy)
{
    /* XXX what to do with clipping from transformed pictures? */
    if (image->transform)
	return 1;
    if (image->repeat)
    {
	/* XXX no source clipping */
	if (image->compositeClipSource &&
	    image->clientClipType != CT_NONE)
	{
	    pixman_region_translate (region, 
			   dx - image->clipOrigin.x,
			   dy - image->clipOrigin.y);
	    pixman_region_intersect (region, image->clientClip, region);
	    pixman_region_translate (region, 
			   - (dx - image->clipOrigin.x),
			   - (dy - image->clipOrigin.y));
	}
	return 1;
    }
    else
    {
	pixman_region16_t   *clip;

	if (image->compositeClipSource)
	    clip = image->pCompositeClip;
	else
	    clip = image->pSourceClip;
	return FbClipImageReg (region,
			       clip,
			       dx,
			       dy);
    }
    return 1;
}

/* XXX: Need to decide what to do with this
#define NEXT_VAL(_type) (vlist ? (_type) *vlist++ : (_type) ulist++->val)

#define NEXT_PTR(_type) ((_type) ulist++->ptr)

int
pixman_image_change (pixman_image_t		*image,
	       Mask		vmask,
	       unsigned int	*vlist,
	       DevUnion		*ulist,
	       int		*error_value)
{
    BITS32		index2;
    int			error = 0;
    BITS32		maskQ;
    
    maskQ = vmask;
    while (vmask && !error)
    {
	index2 = (BITS32) lowbit (vmask);
	vmask &= ~index2;
	image->stateChanges |= index2;
	switch (index2)
	{
	case CPRepeat:
	    {
		unsigned int	newr;
		newr = NEXT_VAL(unsigned int);
		if (newr <= xTrue)
		    image->repeat = newr;
		else
		{
		    *error_value = newr;
		    error = BadValue;
		}
	    }
	    break;
	case CPAlphaMap:
	    {
		pixman_image_t *iAlpha;
		
		iAlpha = NEXT_PTR(pixman_image_t *);
		if (iAlpha)
		    iAlpha->refcnt++;
		if (image->alphaMap)
		    pixman_image_destroy ((void *) image->alphaMap);
		image->alphaMap = iAlpha;
	    }
	    break;
	case CPAlphaXOrigin:
	    image->alphaOrigin.x = NEXT_VAL(int16_t);
	    break;
	case CPAlphaYOrigin:
	    image->alphaOrigin.y = NEXT_VAL(int16_t);
	    break;
	case CPClipXOrigin:
	    image->clipOrigin.x = NEXT_VAL(int16_t);
	    break;
	case CPClipYOrigin:
	    image->clipOrigin.y = NEXT_VAL(int16_t);
	    break;
	case CPClipMask:
	    {
		pixman_image_t	    *mask;
		int	    clipType;

		mask = NEXT_PTR(pixman_image_t *);
		if (mask) {
		    clipType = CT_PIXMAP;
		    mask->refcnt++;
		} else {
		    clipType = CT_NONE;
		}
		error = pixman_image_change_clip (image, clipType,
					   (void *)mask, 0);
		break;
	    }
	case CPGraphicsExposure:
	    {
		unsigned int	newe;
		newe = NEXT_VAL(unsigned int);
		if (newe <= xTrue)
		    image->graphicsExposures = newe;
		else
		{
		    *error_value = newe;
		    error = BadValue;
		}
	    }
	    break;
	case CPSubwindowMode:
	    {
		unsigned int	news;
		news = NEXT_VAL(unsigned int);
		if (news == ClipByChildren || news == IncludeInferiors)
		    image->subWindowMode = news;
		else
		{
		    *error_value = news;
		    error = BadValue;
		}
	    }
	    break;
	case CPPolyEdge:
	    {
		unsigned int	newe;
		newe = NEXT_VAL(unsigned int);
		if (newe == PolyEdgeSharp || newe == PolyEdgeSmooth)
		    image->polyEdge = newe;
		else
		{
		    *error_value = newe;
		    error = BadValue;
		}
	    }
	    break;
	case CPPolyMode:
	    {
		unsigned int	newm;
		newm = NEXT_VAL(unsigned int);
		if (newm == PolyModePrecise || newm == PolyModeImprecise)
		    image->polyMode = newm;
		else
		{
		    *error_value = newm;
		    error = BadValue;
		}
	    }
	    break;
	case CPDither:
	    image->dither = NEXT_VAL(unsigned long);
	    break;
	case CPComponentAlpha:
	    {
		unsigned int	newca;

		newca = NEXT_VAL (unsigned int);
		if (newca <= xTrue)
		    image->componentAlpha = newca;
		else
		{
		    *error_value = newca;
		    error = BadValue;
		}
	    }
	    break;
	default:
	    *error_value = maskQ;
	    error = BadValue;
	    break;
	}
    }
    return error;
}
*/

/* XXX: Do we need this?
int
SetPictureClipRects (PicturePtr	pPicture,
		     int	xOrigin,
		     int	yOrigin,
		     int	nRect,
		     xRectangle	*rects)
{
    ScreenPtr		pScreen = pPicture->pDrawable->pScreen;
    PictureScreenPtr	ps = GetPictureScreen(pScreen);
    pixman_region16_t		*clientClip;
    int			result;

    clientClip = RECTS_TO_REGION(pScreen,
				 nRect, rects, CT_UNSORTED);
    if (!clientClip)
	return 1;
    result =(*ps->ChangePictureClip) (pPicture, CT_REGION, 
				      (void *) clientClip, 0);
    if (result == 0)
    {
	pPicture->clipOrigin.x = xOrigin;
	pPicture->clipOrigin.y = yOrigin;
	pPicture->stateChanges |= CPClipXOrigin|CPClipYOrigin|CPClipMask;
	pPicture->serialNumber |= GC_CHANGE_SERIAL_BIT;
    }
    return result;
}
*/

int
FbComputeCompositeRegion (pixman_region16_t	*region,
			  pixman_image_t	*iSrc,
			  pixman_image_t	*iMask,
			  pixman_image_t	*iDst,
			  int16_t		xSrc,
			  int16_t		ySrc,
			  int16_t		xMask,
			  int16_t		yMask,
			  int16_t		xDst,
			  int16_t		yDst,
			  uint16_t	width,
			  uint16_t	height)
{
    int		v;
    int x1, y1, x2, y2;

    /* XXX: This code previously directly set the extents of the
       region here. I need to decide whether removing that has broken
       this. Also, it might be necessary to just make the pixman_region16_t
       data structure transparent anyway in which case I can just put
       the code back. */
    x1 = xDst;
    v = xDst + width;
    x2 = BOUND(v);
    y1 = yDst;
    v = yDst + height;
    y2 = BOUND(v);
    /* Check for empty operation */
    if (x1 >= x2 ||
	y1 >= y2)
    {
	pixman_region_empty (region);
	return 1;
    }
    /* clip against src */
    if (!FbClipImageSrc (region, iSrc, xDst - xSrc, yDst - ySrc))
    {
	pixman_region_destroy (region);
	return 0;
    }
    if (iSrc->alphaMap)
    {
	if (!FbClipImageSrc (region, iSrc->alphaMap,
			     xDst - (xSrc + iSrc->alphaOrigin.x),
			     yDst - (ySrc + iSrc->alphaOrigin.y)))
	{
	    pixman_region_destroy (region);
	    return 0;
	}
    }
    /* clip against mask */
    if (iMask)
    {
	if (!FbClipImageSrc (region, iMask, xDst - xMask, yDst - yMask))
	{
	    pixman_region_destroy (region);
	    return 0;
	}	
	if (iMask->alphaMap)
	{
	    if (!FbClipImageSrc (region, iMask->alphaMap,
				 xDst - (xMask + iMask->alphaOrigin.x),
				 yDst - (yMask + iMask->alphaOrigin.y)))
	    {
		pixman_region_destroy (region);
		return 0;
	    }
	}
    }
    if (!FbClipImageReg (region, iDst->pCompositeClip, 0, 0))
    {
	pixman_region_destroy (region);
	return 0;
    }
    if (iDst->alphaMap)
    {
	if (!FbClipImageReg (region, iDst->alphaMap->pCompositeClip,
			     -iDst->alphaOrigin.x,
			     -iDst->alphaOrigin.y))
	{
	    pixman_region_destroy (region);
	    return 0;
	}
    }
    return 1;
}

int
miIsSolidAlpha (pixman_image_t *src)
{
    char	line[1];
    
    /* Alpha-only */
    if (PICT_FORMAT_TYPE (src->format_code) != PICT_TYPE_A)
	return 0;
    /* repeat */
    if (!src->repeat)
	return 0;
    /* 1x1 */
    if (src->pixels->width != 1 || src->pixels->height != 1)
	return 0;
    line[0] = 1;
    /* XXX: For the next line, fb has:
	(*pScreen->GetImage) (src->pixels, 0, 0, 1, 1, ZPixmap, ~0L, line);
       Is the following simple assignment sufficient?
    */
    line[0] = src->pixels->data[0];
    switch (src->pixels->bpp) {
    case 1:
	return (uint8_t) line[0] == 1 || (uint8_t) line[0] == 0x80;
    case 4:
	return (uint8_t) line[0] == 0xf || (uint8_t) line[0] == 0xf0;
    case 8:
	return (uint8_t) line[0] == 0xff;
    default:
	return 0;
    }
}
