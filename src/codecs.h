/*
 * codecs.h
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

#ifndef __CODECS_H__
#define __CODECS_H__

#include "win32structs.h"
#include "gdipenums.h"

typedef struct {
	GUID	Guid;
	ULONG	NumberOfValues;
	ULONG	Type;
	VOID*	Value;
} EncoderParameter;

typedef struct {
	UINT Count;
	EncoderParameter Parameter[1];
} EncoderParameters;

typedef struct {
        CLSID Clsid;
        GUID  FormatID;
        const WCHAR* CodecName;
        const WCHAR* DllName;
        const WCHAR* FormatDescription;
        const WCHAR* FilenameExtension;
        const WCHAR* MimeType;
        DWORD Flags;
        DWORD Version;
        DWORD SigCount;
        DWORD SigSize;
        const BYTE* SigPattern;
        const BYTE* SigMask;
} ImageCodecInfo;

GpStatus GdipGetImageDecodersSize (UINT *numDecoders, UINT *size);
GpStatus GdipGetImageDecoders (UINT numDecoders, UINT size, ImageCodecInfo *decoders);
GpStatus GdipGetImageEncodersSize (UINT *numEncoders, UINT *size);
GpStatus GdipGetImageEncoders (UINT numEncoders, UINT size, ImageCodecInfo *encoders);

#endif
