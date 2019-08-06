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

#ifndef __GDIPSTRUCTS_H__
#define __GDIPSTRUCTS_H__

#include "win32structs.h"

#define GDIPCONST const

/* public structures defined by GDI+ (i.e. you can safely access their members) */

typedef struct {
	int First;
	int Length;
} CharacterRange;

typedef struct {
	ARGB Argb;
} Color;

typedef struct {
	Color oldColor;
	Color newColor;
} ColorMap;

typedef struct {
	float m[5][5];
} ColorMatrix;

typedef struct {
        int X, Y;
} Point, GpPoint;

typedef struct {
        float X, Y;
} PointF, GpPointF;

typedef struct {
        int Count;
        PointF *Points;
        BYTE *Types;
} PathData, GpPathData;

typedef struct {
	int X, Y, Width, Height;
} Rect, GpRect;

typedef struct {
	float X, Y, Width, Height;
} RectF, GpRectF;

/* Callbacks */
typedef BOOL (*ImageAbort) (void *);
typedef ImageAbort DrawImageAbort;
typedef ImageAbort GetThumbnailImageAbort;

typedef BOOL (*EnumerateMetafileProc) (EmfPlusRecordType, UINT, UINT, const BYTE*, VOID*);

#endif
