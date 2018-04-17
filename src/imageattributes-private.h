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

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __IMAGEATTRIBUTES_PRIVATE_H__
#define __IMAGEATTRIBUTES_PRIVATE_H__

#include "gdiplus-private.h"

typedef enum {
	ImageAttributeFlagsNoOp = 0x01,
	ImageAttributeFlagsColorMatrixEnabled = 0x02,
	ImageAttributeFlagsThresholdEnabled = 0x04,
	ImageAttributeFlagsGammaEnabled = 0x08,
	ImageAttributeFlagsColorKeysEnabled = 0x10,
	ImageAttributeFlagsColorRemapTableEnabled = 0x20,
	ImageAttributeFlagsOutputChannelEnabled = 0x40,
	ImageAttributeFlagsGrayMatrixEnabled = 0x80,
	ImageAttributeFlagsOutputChannelColorProfileEnabled = 0x100
} ImageAttributeFlags;

typedef struct {
	ImageAttributeFlags flags;
	ColorMap *colormap;
	int colormap_elem;
	float gamma_correction;
	ARGB key_colorlow;
	ARGB key_colorhigh;
	ColorMatrix *colormatrix;
	ColorMatrix *graymatrix;
	ColorMatrixFlags colormatrix_flags;
	float threshold; // Not implemented.
	ColorChannelFlags outputchannel_flags; // Not implemented.
	char *colorprofile_filename; // Not implemented.
} GpImageAttribute;

typedef struct _ImageAttributes {
	GpImageAttribute def;
	GpImageAttribute bitmap;
	GpImageAttribute brush;
	GpImageAttribute pen;
	GpImageAttribute text;
	/* Globals */
	WrapMode wrapmode;
	ARGB color;
} ImageAttributes;

#include "imageattributes.h"

#endif
