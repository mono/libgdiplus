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
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __TEXT_METAFILE_PRIVATE_H__
#define __TEXT_METAFILE_PRIVATE_H__

#include "gdiplus-private.h"

GpStatus metafile_DrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, INT length, GDIPCONST GpFont *font,
	GDIPCONST RectF *rc, GDIPCONST GpStringFormat *format, GpBrush *brush) GDIP_INTERNAL;

#endif
