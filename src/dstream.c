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

#include "gdiplus-private.h"
#include "dstream.h"

struct _dstream_pvt {
	GetBytesDelegate read;
	SeekDelegate seek;

	BYTE *buffer;
	int allocated;
	int position;
	int used;

	/* EXIF APP1 buffer */
	BYTE *exif_buffer;
	unsigned int exif_datasize;
	BOOL keep_exif_buffer;
};

/* dstream_t */
dstream_t *
dstream_input_new (GetBytesDelegate read, SeekDelegate seek)
{
	dstream_t *st;

	st = GdipAlloc (sizeof (dstream_t));
	if (st == NULL)
		return NULL;

	st->pvt = GdipAlloc (sizeof (dstream_private));
	if (st->pvt == NULL) {
		GdipFree (st);
		return NULL;
	}

	memset (st->pvt, 0, sizeof (dstream_private));
	st->pvt->read = read;
	st->pvt->seek = seek;
	return st;
}

void
dstream_free (dstream_t *st)
{
	if (st) {
		dstream_private *loader = st->pvt;

		if (loader->buffer)
			GdipFree (loader->buffer);
		if (loader->exif_buffer)
			GdipFree (loader->exif_buffer);
		memset (loader, 0, sizeof (dstream_t));
		GdipFree (loader);
		GdipFree (st);
	}
}

static int
read_from_buffer (dstream_private *loader, void *buffer, int size)
{
	int nbytes = loader->used - loader->position;

	if (nbytes > size)
		nbytes = size;
	
	memcpy (buffer, loader->buffer + loader->position, nbytes);
	loader->position += nbytes;
	return nbytes;
}

static void
fill_buffer (dstream_private *loader)
{
	int offset;
	int nbytes = loader->used - loader->position;

	if (nbytes > 0)
		return;

	if (loader->allocated == 0) {
		loader->allocated = 65536;
		loader->buffer = GdipAlloc (loader->allocated);
		if (loader->buffer == NULL) {
			/* what? */
			return;
		}
	}

	offset = 0;
	do {
		nbytes = loader->read (loader->buffer + offset, loader->allocated - offset, 0);
		if (nbytes > 0)
			offset += nbytes;
	} while (nbytes > 0 && ((loader->allocated - offset) > 0));

	if (offset != 0) {
		loader->position = 0;
		loader->used = offset;
	}

	/* First chunk read */
	if (nbytes > 0 && loader->keep_exif_buffer && loader->exif_buffer == NULL) {
		loader->exif_buffer = GdipAlloc (offset);
		if (!loader->exif_buffer)
			return;

		loader->exif_datasize = offset;
		memcpy (loader->exif_buffer, loader->buffer, offset);
	}
}

int
dstream_read (dstream_t *st, BYTE *buffer, int size, char peek)
{
	int nbytes;
	int offset;
	dstream_private *loader = st->pvt;

	offset = 0;
	do {
		fill_buffer (loader);
		nbytes = read_from_buffer (loader, buffer + offset, size);
		offset += nbytes;
		size -= nbytes;
	} while (size > 0 && nbytes > 0);

	return offset;
}

void
dstream_skip (dstream_t *st, int nbytes)
{
	dstream_private *loader = st->pvt;

	if (loader->seek != NULL) {
		loader->seek (nbytes, SEEK_CUR);
	} else {
		int remain = loader->used - loader->position;
		if (remain > 0) {
			if (nbytes >= remain) {
				nbytes -= remain;
				loader->used = 0;
				loader->position = 0;
			} else {
				loader->position += nbytes;
				nbytes = 0;
			}
		}

		/* 'read' ignores reads into a NULL buffer */
		while (nbytes > 0) {
			nbytes -= loader->read (NULL, nbytes, 0);
		}
	}

	/* Invalidate buffer */
	loader->used = 0;
	loader->position = 0;
}

void
dstream_keep_exif_buffer (dstream_t *st)
{
	dstream_private *loader = st->pvt;
	loader->keep_exif_buffer = TRUE;
}

void
dstream_get_exif_buffer (dstream_t *st, BYTE **ptr, unsigned int *length)
{
	dstream_private *loader = st->pvt;

	if (loader->keep_exif_buffer) {
		*ptr = loader->exif_buffer;
		*length = loader->exif_datasize;
	} else {
		*ptr = NULL;
		*length = 0;
	}
}
