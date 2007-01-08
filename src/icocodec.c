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

#include "icocodec.h"

/* Codecinfo related data*/
static ImageCodecInfo ico_codec;
static const WCHAR ico_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'I', 'C', 'O', 0}; /* Built-in ICO */
static const WCHAR ico_extension[] = {'*','.','I', 'C', 'O', 0}; /* *.ICO */
static const WCHAR ico_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'x', '-', 'i', 'c', 'o', 'n', 0}; /* image/x-icon */
static const WCHAR ico_format[] = {'I', 'C', 'O', 0}; /* ICO */
static const BYTE ico_sig_pattern[] = { 0x00, 0x00, 0x01, 0x00 };
static const BYTE ico_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF };


ImageCodecInfo*
gdip_getcodecinfo_ico ()
{
	ico_codec.Clsid = (CLSID) { 0x557cf407, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	ico_codec.FormatID = gdip_ico_image_format_guid;
	ico_codec.CodecName = (const WCHAR*) ico_codecname;            
	ico_codec.DllName = NULL;
	ico_codec.FormatDescription = (const WCHAR*) ico_format;
	ico_codec.FilenameExtension = (const WCHAR*) ico_extension;
	ico_codec.MimeType = (const WCHAR*) ico_mimetype;
	ico_codec.Flags = Decoder | SupportBitmap | Builtin;
	ico_codec.Version = 1;
	ico_codec.SigCount = 1;
	ico_codec.SigSize = 4;
	ico_codec.SigPattern = ico_sig_pattern;
	ico_codec.SigMask = ico_sig_mask;
	return &ico_codec; 
}

GpStatus 
gdip_load_ico_image_from_file (FILE *fp, GpImage **image)
{
	return NotImplemented;
}

GpStatus 
gdip_load_ico_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return NotImplemented;
}
