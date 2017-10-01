/*
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
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

#ifndef __IMAGEATTRIBUTES_H__
#define __IMAGEATTRIBUTES_H__

/* ImageAttributes public API (only!) */

GpStatus WINGDIPAPI GdipCreateImageAttributes (GpImageAttributes **imageattr);
GpStatus WINGDIPAPI GdipCloneImageAttributes (GDIPCONST GpImageAttributes *imageattr, GpImageAttributes **cloneImageattr);
GpStatus WINGDIPAPI GdipDisposeImageAttributes (GpImageAttributes *imageattr);

GpStatus WINGDIPAPI GdipSetImageAttributesToIdentity (GpImageAttributes *imageattr, ColorAdjustType type);
GpStatus WINGDIPAPI GdipResetImageAttributes (GpImageAttributes *imageattr, ColorAdjustType type);

GpStatus WINGDIPAPI GdipGetImageAttributesAdjustedPalette(GpImageAttributes *imageattr, ColorPalette *colorPalette, 
	ColorAdjustType colorAdjustType); 

GpStatus WINGDIPAPI GdipSetImageAttributesColorKeys (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, ARGB colorLow,
	ARGB colorHigh);
GpStatus WINGDIPAPI GdipSetImageAttributesColorMatrix (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, 
	GDIPCONST ColorMatrix* colorMatrix, GDIPCONST ColorMatrix* grayMatrix, ColorMatrixFlags flags);
GpStatus WINGDIPAPI GdipSetImageAttributesGamma (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, REAL gamma);
GpStatus WINGDIPAPI GdipSetImageAttributesNoOp (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag);
GpStatus WINGDIPAPI GdipSetImageAttributesOutputChannel (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, 
	ColorChannelFlags channelFlags);
GpStatus WINGDIPAPI GdipSetImageAttributesOutputChannelColorProfile (GpImageAttributes *imageattr, ColorAdjustType type,  BOOL enableFlag,
        GDIPCONST WCHAR *colorProfileFilename);
GpStatus WINGDIPAPI GdipSetImageAttributesRemapTable (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, UINT mapSize, 
	GDIPCONST ColorMap *map);
GpStatus WINGDIPAPI GdipSetImageAttributesThreshold (GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, REAL threshold);
GpStatus WINGDIPAPI GdipSetImageAttributesWrapMode (GpImageAttributes *imageattr, WrapMode wrap, ARGB argb, BOOL clamp); 
GpStatus WINGDIPAPI GdipSetImageAttributesICMMode (GpImageAttributes *imageAttr, BOOL on);
GpStatus WINGDIPAPI GdipSetImageAttributesCachedBackground (GpImageAttributes *imageattr, BOOL enableFlag);

#endif
