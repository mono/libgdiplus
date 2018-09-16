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

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __FONT_PRIVATE_H__
#define __FONT_PRIVATE_H__

#include "gdiplus-private.h"

#ifdef USE_PANGO_RENDERING
	#include <pango/pangofc-font.h>
	#include <pango/pangofc-fontmap.h>
	#include <pango/pangocairo.h>
#endif

struct _Font {
	float			sizeInPixels;
	FontStyle		style;
	unsigned char*		face;
	GpFontFamily		*family;
	float			emSize;
	GpUnit			unit;
#ifdef USE_PANGO_RENDERING
	PangoFontDescription	*pango;
#else
	cairo_font_face_t	*cairofnt;
#endif
};

GpStatus gdip_create_font_without_validation (GDIPCONST GpFontFamily *family, REAL emSize, INT style, Unit unit, GpFont **font);
void gdip_font_clear_pattern_cache (void) GDIP_INTERNAL;
void gdip_delete_system_fonts (void) GDIP_INTERNAL;

#ifdef USE_PANGO_RENDERING

PangoFontDescription* gdip_get_pango_font_description (GpFont *font);

#else

cairo_font_face_t* gdip_get_cairo_font_face (GpFont *font);

#endif

#include "font.h"

#endif
