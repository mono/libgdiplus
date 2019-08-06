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

#ifndef __BITMAP_H__
#define __BITMAP_H__

GpStatus WINGDIPAPI GdipCreateBitmapFromStream (void *stream, GpBitmap **bitmap);
GpStatus WINGDIPAPI GdipCreateBitmapFromFile (GDIPCONST WCHAR* filename, GpBitmap **bitmap);
GpStatus WINGDIPAPI GdipCreateBitmapFromStreamICM (void *stream, GpBitmap **bitmap);
GpStatus WINGDIPAPI GdipCreateBitmapFromFileICM (GDIPCONST WCHAR* filename, GpBitmap **bitmap);

GpStatus WINGDIPAPI GdipCreateBitmapFromScan0 (INT width, INT height, INT stride, PixelFormat format, BYTE* scan0, GpBitmap **bitmap);
GpStatus WINGDIPAPI GdipCreateBitmapFromGraphics (INT width, INT height, GpGraphics *target, GpBitmap **bitmap);

GpStatus WINGDIPAPI GdipCreateBitmapFromHBITMAP (HBITMAP hbm, HPALETTE hpal, GpBitmap** bitmap);
GpStatus WINGDIPAPI GdipCreateHBITMAPFromBitmap (GpBitmap* bitmap, HBITMAP *hbmReturn, ARGB background);

GpStatus WINGDIPAPI GdipCreateBitmapFromHICON (HICON hicon, GpBitmap** bitmap);
GpStatus WINGDIPAPI GdipCreateHICONFromBitmap (GpBitmap* bitmap, HICON *hbmReturn);

GpStatus WINGDIPAPI GdipCreateBitmapFromResource (HINSTANCE hInstance, GDIPCONST WCHAR *lpBitmapName, GpBitmap** bitmap);

GpStatus WINGDIPAPI GdipBitmapLockBits (GpBitmap *bitmap, GDIPCONST Rect *rect, UINT flags, PixelFormat format, BitmapData *lockedBitmapData);
GpStatus WINGDIPAPI GdipBitmapUnlockBits (GpBitmap *bitmap, BitmapData *lockedBitmapData);

GpStatus WINGDIPAPI GdipBitmapSetPixel (GpBitmap *bitmap, INT x, INT y, ARGB color);
GpStatus WINGDIPAPI GdipBitmapGetPixel (GpBitmap *bitmap, INT x, INT y, ARGB *color);

GpStatus WINGDIPAPI GdipCloneBitmapArea (REAL x, REAL y, REAL width, REAL height, PixelFormat format, GpBitmap *srcBitmap, GpBitmap **dstBitmap);
GpStatus WINGDIPAPI GdipCloneBitmapAreaI (INT x, INT y, INT width, INT height, PixelFormat format, GpBitmap *srcBitmap, GpBitmap **dstBitmap);

GpStatus WINGDIPAPI GdipBitmapSetResolution (GpBitmap *bitmap, REAL xdpi, REAL ydpi);


/* missing API
	GdipCreateBitmapFromDirectDrawSurface
	GdipCreateBitmapFromGdiDib
*/

#endif
