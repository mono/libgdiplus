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

GpStatus
GdipCreateImageAttributes(GpImageAttributes **imageattr)
{
        if (!imageattr)
                return InvalidParameter;

        GpImageAttributes *result = (GpImageAttributes *) GdipAlloc (sizeof (GpImageAttributes));
      
        *imageattr = result;
        return Ok;        
}


GpStatus
GdipCloneImageAttributes(GDIPCONST GpImageAttributes *imageattr, GpImageAttributes **cloneImageattr)
{
        if (!imageattr || !cloneImageattr)
                return InvalidParameter;

        GpImageAttributes *result = (GpImageAttributes *) GdipAlloc (sizeof (GpImageAttributes));

        memcpy (result, imageattr, sizeof (GpStringFormat));

        *cloneImageattr = result;
        return Ok; 

}

GpStatus
GdipDisposeImageAttributes(GpImageAttributes *imageattr)
{
        if (!imageattr)
                return InvalidParameter;

        GdipFree (imageattr);
        return Ok;
}

GpStatus
GdipSetImageAttributesThreshold(GpImageAttributes *imageattr,  ColorAdjustType type, BOOL enableFlag, float threshold)
{
        return NotImplemented;
}


GpStatus
GdipSetImageAttributesGamma(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, float gamma)
{
        return NotImplemented;
}

GpStatus
GdipSetImageAttributesNoOp(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag)
{
        return NotImplemented;
}


GpStatus
GdipSetImageAttributesColorKeys(GpImageAttributes *imageattr, ColorAdjustType type,  BOOL enableFlag,
        ARGB colorLow, ARGB colorHigh)
{
        return NotImplemented;
}

GpStatus
GdipSetImageAttributesOutputChannelColorProfile(GpImageAttributes *imageattr, ColorAdjustType type,  BOOL enableFlag,
        GDIPCONST WCHAR *colorProfileFilename)

{       
        return NotImplemented;
}

GpStatus
GdipGetImageAttributesAdjustedPalette(GpImageAttributes *imageAttr, ColorPalette *colorPalette, ColorAdjustType colorAdjustType)
{
	return(NotImplemented);
}

GpStatus
GdipSetImageAttributesColorMatrix(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, GDIPCONST GpColorMatrix *colorMatrix, GDIPCONST GpColorMatrix *grayMatrix, GpColorMatrixFlags flags)
{
        return(NotImplemented);
}

GpStatus
GdipSetImageAttributesOutputChannel(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, GpColorChannelFlags channelFlags)
{
        return(NotImplemented);
}

GpStatus
GdipSetImageAttributesRemapTable(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, UINT mapSize, GDIPCONST GpColorMap *map)
{
        return(NotImplemented);
}

GpStatus
GdipSetImageAttributesWrapMode(GpImageAttributes *imageAttr, WrapMode wrap, ARGB argb, BOOL clamp)
{
        return(NotImplemented);
}

