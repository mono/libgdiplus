// Authors:
// Francis Fisher (frankie@terrorise.me.uk)
//
// (C) Francis Fisher 2013
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "graphics-private.h"
#include "graphics-cairo-private.h"

#include <cairo-features.h>

#define	NO_CAIRO_AA

#define MAX_GRAPHICS_STATE_STACK 512

#define SRCCOPY             (DWORD)0x00CC0020

BOOL
BitBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, HDC hdcSrc, int nXSrc, int nYSrc, DWORD dwRop)
{
	if (dwRop != SRCCOPY)
		return FALSE;
	GpGraphics *src = (GpGraphics *) hdcSrc;
	GpGraphics *dest = (GpGraphics *) hdcDest;

	cairo_t *cr = dest->ct;
	cairo_surface_t *src_surface = cairo_get_target(src->ct);

	cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
	cairo_set_source_surface (cr, src_surface, nXDest - nXSrc, nYDest - nYSrc);
	cairo_rectangle (cr, nXDest, nYDest, nWidth, nHeight);
	cairo_fill (cr);

	return TRUE;
}

