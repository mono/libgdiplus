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

#include "emfplus.h"

//#define DEBUG_EMFPLUS_ALL
#ifdef DEBUG_EMFPLUS_ALL
#define DEBUG_EMFPLUS
#define DEBUG_EMFPLUS_2
#define DEBUG_EMFPLUS_3
#define DEBUG_EMFPLUS_NOTIMPLEMENTED
#endif


/* http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/Header.html */
static GpStatus
EmfPlusHeader (MetafilePlayContext *context, WORD flags, BYTE* data, int size)
{
#ifdef DEBUG_EMFPLUS
	/* if flags == 1, EmfPlusDual (GDI and GDI+), else EmfPlusOnly (GDI+) */
	printf ("\nEmfPlusHeader flags %X", flags);
	printf ("\n\tData Size %d", GETDW(DWP1));
	printf ("\n\tObject Header %X", GETDW(DWP2));
	printf ("\n\tVersion %d", GETDW(DWP3));
	printf ("\n\tHorizontal Resolution %d", GETDW(DWP4));
	printf ("\n\tVertical Resolution %d", GETDW(DWP5));
#endif
	context->metafile->metafile_header.Type = (flags & 1) ? MetafileTypeEmfPlusDual : MetafileTypeEmfPlusOnly;
	/* ObjectHeader, not Version, is returned to be compatible with GDI+ */
	context->metafile->metafile_header.Version = GETDW(DWP2);
	/* Horizontal and Vertical Resolution aren't reported correctly by GDI+ (generally 0) */
	return Ok;
}

/* http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/EndOfFile.html */
static GpStatus
EmfPlusEndOfFile (MetafilePlayContext *context, WORD flags, BYTE* data, int size)
{
#ifdef DEBUG_EMFPLUS
	printf ("EmfPlusRecordTypeEndOfFile flags %X", flags);
#endif
	return Ok;
}

/* http://www.aces.uiuc.edu/~jhtodd/Metafile/MetafileRecords/FillRects.html */
static GpStatus
EmfPlusFillRects (MetafilePlayContext *context, WORD flags, BYTE* data, int size)
{
	GpStatus status = Ok;
	GpSolidFill *solid = NULL;
	GpBrush *brush = NULL;

#ifdef DEBUG_EMFPLUS
	printf ("EmfPlusRecordTypeFillRects flags %X", flags, size);
	printf ("\n\tColor: 0x%X", GETDW(DWP2));
	printf ("\n\t#rect: %d", GETDW(DWP3));
#endif
	BOOL use_int16 = (flags & 0x4000);
	BOOL use_color = (flags & 0x8000);
	DWORD num = GETDW(DWP3);
	int i = 0;
	int n = 3;

	if (use_color) {
		status = GdipCreateSolidFill (GETDW(DWP2), &solid);
		if (status != Ok)
			return status;
		brush = (GpBrush*)solid;
	} else {
		/* TODO - get brush from index */
	}

	while ((i++ < num) && (status == Ok)) {
		float x, y, w, h;
		if (use_int16) {
			DWORD xy = GETDW(DWP(n));
			n++;
			x = (xy >> 16);
			y = (WORD)xy;
			DWORD wh = GETDW(DWP(n));
			n++;
			w = (wh >> 16);
			h = (WORD)wh;
		} else {
			x = (float) GETDW(DWP(n));
			n++;
			y = (float) GETDW(DWP(n));
			n++;
			w = (float) GETDW(DWP(n));
			n++;
			h = (float) GETDW(DWP(n));
			n++;
		}
#ifdef DEBUG_EMFPLUS_2
		printf ("\n\t\t%d - x %g, y %g, w %g, h %g", i, x, y, w, h);
#endif
		status = GdipFillRectangle (context->graphics, brush, x, y, w, h);
	}

	if (solid)
		GdipDeleteBrush (brush); /* brush == a GpBrush* typecasted solid, if used */
	return status;
}

GpStatus
gdip_metafile_play_emfplus_block (MetafilePlayContext *context, BYTE* data, int length)
{
	GpStatus status = Ok;
	BYTE *end = data + length;
#ifdef DEBUG_EMFPLUS
	int i = 1, j;
#endif

	/* special case to update the header informations (we're not really playing the metafile) */
	if (!context->graphics) {
		DWORD record = GETDW(EMF_FUNCTION);
		WORD func = (WORD)record;
		if (func == EmfPlusRecordTypeHeader) {
			WORD flags = (record >> 16);
			DWORD size = GETDW(EMF_RECORDSIZE);
			return EmfPlusHeader (context, flags, data, size);
		}
		/* no header, continue */
		return Ok;
	}

	/* reality check - each record is, at minimum, 8 bytes long (when size == 0) */
	while (data < end - EMF_MIN_RECORD_SIZE) {
		DWORD record = GETDW(EMF_FUNCTION);
		WORD func = (WORD)record;
		WORD flags = (record >> 16);
		DWORD size = GETDW(EMF_RECORDSIZE);
#ifdef DEBUG_EMFPLUS
		printf ("\n\tEMF+[#%d] size %d ", i++, size);
#endif
		switch (func) {
		case EmfPlusRecordTypeHeader:
			status = EmfPlusHeader (context, flags, data, size);
			break;
		case EmfPlusRecordTypeEndOfFile:
			return EmfPlusEndOfFile (context, flags, data, size);
		case EmfPlusRecordTypeFillRects:
			status = EmfPlusFillRects (context, flags, data, size);
			break;
		default:
			/* unprocessed records, ignore the data */
#ifdef DEBUG_EMFPLUS_NOTIMPLEMENTED
			printf ("Unimplemented_%d (", func);
			for (j = 0; j < params; j++) {
				printf (" %d", GETDW(DWP(j)));
			}
			printf (" )");
#endif
			break;
		}

		if (status != Ok) {
			g_warning ("EMF+ parsing interupted, status %d returned from function %d.", status, func);
			goto cleanup;
		}

		data += size;
	}
cleanup:
	return status;
}
