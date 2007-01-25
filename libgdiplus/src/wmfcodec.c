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

#include "wmfcodec.h"

/* Codecinfo related data*/
static ImageCodecInfo wmf_codec;
static const WCHAR wmf_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'W', 'M', 'F', 0}; /* Built-in WMF */
static const WCHAR wmf_extension[] = {'*','.','W', 'M', 'F', 0}; /* *.WMF */
static const WCHAR wmf_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'x', '-', 'w', 'm', 'f', 0}; /* image/x-wmf */
static const WCHAR wmf_format[] = {'W', 'M', 'F', 0}; /* WMF */
static const BYTE wmf_sig_pattern[] = { 0xD7, 0xCD, 0xC6, 0x9A };
static const BYTE wmf_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF };


ImageCodecInfo*
gdip_getcodecinfo_wmf ()
{
	wmf_codec.Clsid = (CLSID) { 0x557cf404, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	wmf_codec.FormatID = gdip_wmf_image_format_guid;
	wmf_codec.CodecName = (const WCHAR*) wmf_codecname;    
	wmf_codec.DllName = NULL;
	wmf_codec.FormatDescription = (const WCHAR*) wmf_format;
	wmf_codec.FilenameExtension = (const WCHAR*) wmf_extension;
	wmf_codec.MimeType = (const WCHAR*) wmf_mimetype;
	wmf_codec.Flags = Decoder | SupportBitmap | Builtin;
	wmf_codec.Version = 1;
	wmf_codec.SigCount = 1;
	wmf_codec.SigSize = 4;
	wmf_codec.SigPattern = wmf_sig_pattern;
	wmf_codec.SigMask = wmf_sig_mask;
	return &wmf_codec;
}

GpStatus 
gdip_load_wmf_image_from_file (FILE *fp, GpImage **image)
{
	return NotImplemented;
}

GpStatus 
gdip_load_wmf_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return NotImplemented;
}
