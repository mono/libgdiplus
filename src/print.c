/*
 * print.c
 * 
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
 *   Jordi Mas (jordi@ximian.com)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#include <cairo-features.h>

#ifdef CAIRO_HAS_PS_SURFACE

#include "graphics-private.h"
#include <cairo-ps.h>

/*
 * NOTE: The functions in this file aren't part of GDI+ and can only be called 
 * from Mono's System.Drawing.dll on non-Windows operating systems
 */

/* coverity[+alloc : arg-*5] */
GpStatus 
GdipGetPostScriptGraphicsContext (char* filename, int width, int height, double dpix, double dpiy, GpGraphics **graphics)
{
	GpGraphics *gfx;
	cairo_surface_t *surface;
	
	if (!graphics)
		return InvalidParameter;

	surface = cairo_ps_surface_create (filename, (double) width, (double) height);
	cairo_surface_set_fallback_resolution (surface, dpix, dpiy);

	gfx = gdip_graphics_new (surface);
	gfx->dpi_x = dpix;
	gfx->dpi_y = dpiy;
	cairo_surface_destroy (surface);

	gfx->type = gtPostScript;
	*graphics = gfx;
	return Ok;
}

GpStatus 
GdipGetPostScriptSavePage (GpGraphics* graphics) 
{
	if (!graphics)
		return InvalidParameter;

	cairo_show_page (graphics->ct);
	return Ok;
}

#endif
