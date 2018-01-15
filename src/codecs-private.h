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

#ifndef __CODECS_PRIVATE_H__
#define __CODECS_PRIVATE_H__

#include "gdiplus-private.h"
#include "codecs.h"

/* Maximum CODEC signature length, which is the minimum size we need to read from an image file to detect it's type */
#define MAX_CODEC_SIG_LENGTH		44

typedef enum {
	File,
	DStream,
	Memory
} ImageSource;

typedef struct {
	BYTE* ptr;
	int size;
	int pos;
} MemorySource;


static const CLSID gdip_image_frameDimension_page_guid = {0x7462dc86U, 0x6180U, 0x4c7eU, {0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83}};
static const CLSID gdip_image_frameDimension_time_guid = {0x6aedbd6dU, 0x3fb5U, 0x418aU, {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};
static const CLSID gdip_image_frameDimension_resolution_guid = {0x84236f7bU, 0x3bd3U, 0x428fU, {0x8d, 0xab, 0x4e, 0xa1, 0x43, 0x9c, 0xa3, 0x15}};

const EncoderParameter *gdip_find_encoder_parameter (GDIPCONST EncoderParameters *eps, const GUID *guid) GDIP_INTERNAL;

GpStatus initCodecList (void) GDIP_INTERNAL;
void releaseCodecList (void) GDIP_INTERNAL;

extern GUID GdipEncoderCompression;
extern GUID GdipEncoderColorDepth;
extern GUID GdipEncoderSaveFlag;
extern GUID GdipEncoderSaveAsCMYK;
extern GUID GdipEncoderImageItems;
extern GUID GdipEncoderTransformation;
extern GUID GdipEncoderQuality;
extern GUID GdipEncoderLuminanceTable;
extern GUID GdipEncoderChrominanceTable;

#endif
