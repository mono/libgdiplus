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
 */

#ifndef __DSTREAM_H
#define __DSTREAM_H

#include "win32structs.h"
#include "image.h"

typedef struct _dstream_pvt dstream_private;
typedef struct _dstream dstream_t;
struct _dstream {
	dstream_private *pvt;
};

dstream_t *dstream_input_new (GetBytesDelegate read, SeekDelegate seek) GDIP_INTERNAL;
int dstream_read (dstream_t *loader, BYTE *buffer, int size, char peek) GDIP_INTERNAL;
void dstream_skip (dstream_t *loader, int nbytes) GDIP_INTERNAL;
void dstream_free (dstream_t *loader) GDIP_INTERNAL;
void dstream_keep_exif_buffer (dstream_t *loader) GDIP_INTERNAL;
void dstream_get_exif_buffer (dstream_t *loader, unsigned char **ptr, unsigned int *length) GDIP_INTERNAL;

#endif
