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

#include "emfcodec.h"

/* Codecinfo related data*/
static ImageCodecInfo emf_codec;
static const WCHAR emf_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'E', 'M', 'F', 0}; /* Built-in EMF */
static const WCHAR emf_extension[] = {'*','.','E', 'M', 'F', 0}; /* *.EMF */
static const WCHAR emf_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'x', '-', 'e', 'm', 'f', 0}; /* image/x-emf */
static const WCHAR emf_format[] = {'E', 'M', 'F', 0}; /* EMF */
static const BYTE emf_sig_pattern[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x45, 0x4D, 0x46 };
static const BYTE emf_sig_mask[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

ImageCodecInfo*
gdip_getcodecinfo_emf ()
{
	emf_codec.Clsid = (CLSID) { 0x557cf403, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	emf_codec.FormatID = gdip_emf_image_format_guid;
	emf_codec.CodecName = (const WCHAR*) emf_codecname;    
	emf_codec.DllName = NULL;
	emf_codec.FormatDescription = (const WCHAR*) emf_format;
	emf_codec.FilenameExtension = (const WCHAR*) emf_extension;
	emf_codec.MimeType = (const WCHAR*) emf_mimetype;
	emf_codec.Flags = Decoder | SupportBitmap | Builtin;
	emf_codec.Version = 1;
	emf_codec.SigCount = 1;
	emf_codec.SigSize = 44;
	emf_codec.SigPattern = emf_sig_pattern;
	emf_codec.SigMask = emf_sig_mask;
	return &emf_codec;
}

GpStatus
gdip_metafile_play_emf (MetafilePlayContext *context)
{
	return NotImplemented;
}

GpStatus 
gdip_load_emf_image_from_file (FILE *fp, GpImage **image)
{
	return NotImplemented;
}

GpStatus 
gdip_load_emf_image_from_stream_delegate (dstream_t *loader, GpImage **image)
{
	return NotImplemented;
}
