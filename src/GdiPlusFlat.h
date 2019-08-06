/*
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
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
 *          Sebastien Pouliot  <sebastien@ximian.com>
 */

/*
 * This is *the* public header to be used by C/C++ consumers of GDI+
 * and has the same name as MS provided header for the "flat API"
 */

#ifndef _FLATAPI_H
#define _FLATAPI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * WINGDIPAPI is used to define the visibility of the GDI+ functions. If you're including GdiPlusFlat.h
 * file, you're using the public header, consuming GDI+, so the GDI+ functions are not exported in your library.
 */
#define WINGDIPAPI

/*
 * If you ever need a workaround specific to libgdiplus then use the following define
 */

#define MONO_LIBGDIPLUS	1

/*
 * public view of internal stuff (curious about them, check in *-private.h files ;-)
 */

typedef void GpAdjustableArrowCap;
typedef void GpBitmap;
typedef void GpBrush;
typedef void GpCustomLineCap;
typedef void GpFont;
typedef void GpFontCollection;
typedef void GpFontFamily;
typedef void GpGraphics;
typedef void GpHatch;
typedef void GpImage;
typedef void GpImageAttributes;
typedef void GpLineGradient;
typedef void GpMatrix;
typedef void GpMetafile;
typedef void GpPath;
typedef void GpPathIterator;
typedef void GpPathGradient;
typedef void GpPen;
typedef void GpRegion;
typedef void GpSolidFill;
typedef void GpState;
typedef void GpStringFormat;
typedef void GpTexture;

/*
 * Do not include those headers directly.
 * Their content is public but, unlike this file, the header filenames do NOT match MS filenames!
 */

#include "gdipluspixelformats.h"
#include "gdiplusimaging.h"
#include "gdipenums.h"
#include "gdipstructs.h"

#include "general.h"
#include "adjustablearrowcap.h"
#include "bitmap.h"
#include "brush.h"
#include "customlinecap.h"
#include "font.h"
#include "fontcollection.h"
#include "fontfamily.h"
#include "graphics.h"
#include "graphics-path.h"
#include "graphics-pathiterator.h"
#include "hatchbrush.h"
#include "image.h"
#include "imageattributes.h"
#include "lineargradientbrush.h"
#include "matrix.h"
#include "metafile.h"
#include "pathgradientbrush.h"
#include "pen.h"
#include "region.h"
#include "solidbrush.h"
#include "stringformat.h"
#include "text.h"
#include "texturebrush.h"

#ifdef __cplusplus
}
#endif

#endif
