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
#include <math.h>

void
gdip_init_image_attribute (GpImageAttribute* attr)
{
	attr->colormap = NULL;
	attr->colormap_elem = 0;
	attr->gamma_correction = 0.0f;
	attr->key_colorlow = 0;
	attr->key_colorhigh = 0;
	attr->key_enabled = FALSE;	
}

void
gdip_dispose_image_attribute (GpImageAttribute* attr)
{
	if (attr->colormap)
		free (attr->colormap);
}


GpImageAttribute*
gdip_get_image_attribute (GpImageAttributes* attr, ColorAdjustType type)
{
	switch (type)	{
	case ColorAdjustTypeDefault:
		return &attr->def;
	case ColorAdjustTypeBitmap:
		return &attr->bitmap;
	case ColorAdjustTypeBrush:
		return &attr->brush;
	case ColorAdjustTypePen:
		return &attr->pen;
	case ColorAdjustTypeText:
		return &attr->text;	
	default:
		return NULL;
	}
}

void
gdip_process_bitmap_attributes (GpBitmap *bitmap, void **dest, GpImageAttributes* attr, bool *allocated)
{ 
	GpImageAttribute *imgattr;
	void *scan0;
	GpBitmap bmpdest;
	int x,y, cnt;
	ARGB color;	
	
	*allocated = FALSE;
	
	if (!bitmap || !dest || !attr) 
		return;
	
	imgattr = gdip_get_image_attribute (attr, ColorAdjustTypeBitmap);
	
	if (!imgattr)	
		return;		
	
	/* Fail into default*/
	if (!imgattr->colormap_elem || !imgattr->gamma_correction || !imgattr->key_enabled) 
		imgattr = gdip_get_image_attribute (attr, ColorAdjustTypeDefault);
		
	if (imgattr->colormap_elem || imgattr->gamma_correction || imgattr->key_enabled) {
		scan0 = malloc (bitmap->data.Stride * bitmap->data.Height);
		memcpy (scan0, bitmap->data.Scan0, bitmap->data.Stride * bitmap->data.Height);
		*dest = scan0;
		memcpy (&bmpdest, bitmap, sizeof (GpBitmap));
		bmpdest.data.Scan0 = scan0;
		*allocated = TRUE;
	}
	
	/* 	
		We use get/set pixel instead of direct buffer manipulation because it's a good way of keeping the pixel 
		logic in a single place with a very litte penalty in performance	
	*/	
	
	/* Color mapping */
	if (imgattr->colormap_elem) {	
		for (y = 0; y <bitmap->data.Height; y++) {	
			for (x = 0; x <bitmap->data.Width; x++) {
				GpColorMap* clrmap = imgattr->colormap;
				int alpha;
				ARGB colorold;		
				
				GdipBitmapGetPixel (&bmpdest, x, y, &color);
				
				for (cnt = 0; cnt < imgattr->colormap_elem; cnt++, clrmap++) {
				  
					if (color == clrmap->oldColor.Color) {						
						color = clrmap->newColor.Color;						
						break;
					}
				}
							
				GdipBitmapSetPixel (&bmpdest, x, y, color);
			}	
		}	
	}
	
	/* Gamma correction */
	if (imgattr->gamma_correction) {
		for (y = 0; y <bitmap->data.Height; y++) {	
			for (x = 0; x <bitmap->data.Width; x++) {
			
				BYTE r,g,b,a;					
				
				GdipBitmapGetPixel (&bmpdest, x, y, &color);		
			
				a = (color & 0xff000000) >> 24;
				r = (color & 0x00ff0000) >> 16;
				g = (color & 0x0000ff00) >> 8;
				b = (color & 0x000000ff);
				
				/* FIXME: This is not the right gamma GDI + correction algorithm */
				
				/*
				r = (int) powf (r, (1 / imgattr->gamma_correction));			
				g = (int) powf (g, (1 / imgattr->gamma_correction));			
				b = (int) powf (b, (1 / imgattr->gamma_correction));			
				a = (int) powf (a, (1 / imgattr->gamma_correction));*/
				
				color = b | (g  << 8) | (r << 16) | (a << 24);
					
				GdipBitmapSetPixel (&bmpdest, x, y, color);
			}	
		}
		
	}	
	
	/* Apply transparency range */
	if (imgattr->key_enabled) {
		for (y = 0; y <bitmap->data.Height; y++) {	
			for (x = 0; x <bitmap->data.Width; x++) {
				
				GdipBitmapGetPixel (&bmpdest, x, y, &color);					
				
				if (color >= imgattr->key_colorlow && color <= imgattr->key_colorhigh)
					color = color & 0x00ffffff; /* Alpha = 0 */
					
				GdipBitmapSetPixel (&bmpdest, x, y, color);
			}	
		}
	
	}
}


GpStatus
GdipCreateImageAttributes (GpImageAttributes **imageattr)
{
	GpImageAttributes *result;

	if (!imageattr)
		return InvalidParameter;

	result = (GpImageAttributes *) GdipAlloc (sizeof (GpImageAttributes));
	gdip_init_image_attribute (&result->def);
	gdip_init_image_attribute (&result->bitmap);
	gdip_init_image_attribute (&result->brush);
	gdip_init_image_attribute (&result->pen);
	gdip_init_image_attribute (&result->text);
	result->color = 0;
	result->wrapmode = WrapModeClamp;
      
	*imageattr = result;
	return Ok;        
}


GpStatus
GdipCloneImageAttributes (GDIPCONST GpImageAttributes *imageattr, GpImageAttributes **cloneImageattr)
{
	GpImageAttributes *result;

	if (!imageattr || !cloneImageattr)
		return InvalidParameter;

	result = (GpImageAttributes *) GdipAlloc (sizeof (GpImageAttributes));

	memcpy (result, imageattr, sizeof (GpStringFormat));

	*cloneImageattr = result;
	return Ok; 

}

GpStatus
GdipDisposeImageAttributes (GpImageAttributes *imageattr)
{
	if (!imageattr)
		return InvalidParameter;
		
	gdip_dispose_image_attribute (&imageattr->def);
	gdip_dispose_image_attribute (&imageattr->bitmap);
	gdip_dispose_image_attribute (&imageattr->brush);
	gdip_dispose_image_attribute (&imageattr->pen);
	gdip_dispose_image_attribute (&imageattr->text);

	GdipFree (imageattr);
	return Ok;
}

GpStatus
GdipSetImageAttributesThreshold ( GpImageAttributes *imageattr,  ColorAdjustType type, BOOL enableFlag, float threshold)
{
	return NotImplemented;
}


GpStatus  
GdipSetImageAttributesGamma (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, float gamma)
{
	GpImageAttribute *imgattr;
		
	if (!imageattr)
		return InvalidParameter;
		
	imgattr = gdip_get_image_attribute (imageattr, type);
	
	if (!imgattr)
		return InvalidParameter;
			
	if (enableFlag)	
		imgattr->gamma_correction = gamma;
	else
		imgattr->gamma_correction = 0.0f;

	return Ok;
}

GpStatus
GdipSetImageAttributesNoOp (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag)
{	
	GpImageAttribute *imgattr;
		
	if (!imageattr)
		return InvalidParameter;
		
	imgattr = gdip_get_image_attribute (imageattr, type);
	
	if (!imgattr)
		return InvalidParameter;	
		
	imgattr->no_op = enableFlag;
	return Ok;
}


GpStatus
GdipSetImageAttributesColorKeys (GpImageAttributes *imageattr, ColorAdjustType type,  BOOL enableFlag,
        ARGB colorLow, ARGB colorHigh)
{
	GpImageAttribute *imgattr;
		
	if (!imageattr)
		return InvalidParameter;
		
	imgattr = gdip_get_image_attribute (imageattr, type);
	
	if (!imgattr)
		return InvalidParameter;	

	imgattr->key_colorlow = colorLow;
	imgattr->key_colorhigh = colorHigh;
	imgattr->key_enabled = enableFlag;
	
	return Ok;
}

GpStatus
GdipSetImageAttributesOutputChannelColorProfile (GpImageAttributes *imageattr, ColorAdjustType type,  BOOL enableFlag,
        GDIPCONST WCHAR *colorProfileFilename)

{       
	return NotImplemented;
}


GpStatus 
GdipSetImageAttributesRemapTable (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, UINT mapSize, GDIPCONST GpColorMap *map)
{
	GpImageAttribute *imgattr;
	
	if (!imageattr || !map)
		return InvalidParameter;
		
	imgattr = gdip_get_image_attribute (imageattr, type);
	
	if (!imgattr)
		return InvalidParameter;	
		
	if (!enableFlag)  {	/* Acts as clean */			
		free (imgattr->colormap);
		imgattr->colormap = NULL;
		imgattr->colormap_elem = 0;
		return Ok;
	}
	
	if (imgattr->colormap) 
		free (imgattr->colormap);
		
	/* Copy colormap table*/	
	imgattr->colormap =  malloc (mapSize);
	memcpy (imgattr->colormap, map, mapSize * sizeof (GpColorMap));
	imgattr->colormap_elem = mapSize;
	
	return Ok;	
}

/*
	According to Microsoft documentation:
	clamp: This parameter has no effect in Microsoft® Windows® GDI+ version 1.0
*/ 
GpStatus 
GdipSetImageAttributesWrapMode (GpImageAttributes *imageattr, WrapMode wrap, ARGB argb, BOOL clamp)
{
	if (!imageattr)
		return InvalidParameter;	
	
	imageattr->wrapmode = wrap;
	imageattr->color = argb;	
	return Ok;
}


GpStatus 
GdipGetImageAttributesAdjustedPalette (GpImageAttributes *imageattr, ColorPalette *colorPalette, ColorAdjustType type)
{
	return NotImplemented;
}


GpStatus 
GdipSetImageAttributesColorMatrix (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag,  GpColorMatrix* colorMatrix,
	GpColorMatrix* grayMatrix, GpColorMatrixFlags flags)
{
	static int	called = 0;

	if (!called) {
		printf("NOT IMPLEMENTED YET: GdipSetImageAttributesColorMatrix (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag,  GpColorMatrix* colorMatrix, GpColorMatrix* grayMatrix, GpColorMatrixFlags flags)\n");
	}
	//return NotImplemented;
	return Ok;
}
	
GpStatus 
GdipSetImageAttributesOutputChannel (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, GpColorChannelFlags channelFlags)
{
	return NotImplemented;
}

