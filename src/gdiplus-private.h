/*
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
 *          Sebastien Pouliot  <sebastien@ximian.com>
 */

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __LIBGDIPLUS_H__
#define __LIBGDIPLUS_H__

#include <stdio.h>

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <glib.h>
#include <ft2build.h>
#include FT_TRUETYPE_TABLES_H
#ifndef _MSC_VER
#include <pthread.h>
#include <unistd.h>

#endif

#include "config.h"

#if HAVE_VISIBILITY_HIDDEN
	#define GDIP_INTERNAL __attribute__((visibility ("hidden")))
#else
	#define GDIP_INTERNAL
#endif

#ifdef _MSC_VER
#define WINGDIPAPI __declspec( dllexport )
#else
#define WINGDIPAPI
#endif

#include <cairo/cairo.h>
#ifdef CAIRO_HAS_FT_FONT
	#include <cairo/cairo-ft.h>
#endif

#if defined(HAVE_X11)
#ifdef CAIRO_HAS_XLIB_SURFACE
	#include <cairo/cairo-xlib.h>
#endif
#endif

#if defined(HAVE_X11)
#include <X11/Xlib.h>
#endif

#include "win32structs.h"
#include "gdipenums.h"
#include "gdipluspixelformats.h"
#include "gdiplusimaging.h"
#include "gdipstructs.h"
#include "general-private.h"

typedef struct _AdjustableArrowCap GpAdjustableArrowCap;
typedef struct _Brush GpBrush;
typedef struct _CustomLineCap GpCustomLineCap;
typedef struct _Font GpFont;
typedef struct _FontCollection GpFontCollection;
typedef struct _FontFamily GpFontFamily;
typedef struct _Graphics GpGraphics;
typedef struct _Hatch GpHatch;
typedef struct _Image GpImage;
typedef struct _ImageAttributes GpImageAttributes;
typedef struct _LineGradient GpLineGradient;
typedef struct _Metafile GpMetafile;
typedef struct _Path GpPath;
typedef struct _PathIterator GpPathIterator;
typedef struct _PathGradient GpPathGradient;
typedef struct _Pen GpPen;
typedef struct _Region GpRegion;
typedef struct _SolidFill GpSolidFill;
typedef struct _StringFormat GpStringFormat;
typedef struct _Texture GpTexture;

#endif
