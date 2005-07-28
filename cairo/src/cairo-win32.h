/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2005 Red Hat, Inc
 *
 * This library is free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 * The Original Code is the cairo graphics library.
 *
 * The Initial Developer of the Original Code is Red Hat, Inc.
 *
 * Contributor(s):
 *	Owen Taylor <otaylor@redhat.com>
 */

#ifndef _CAIRO_WIN32_H_
#define _CAIRO_WIN32_H_

#include <cairo.h>

#if CAIRO_HAS_WIN32_SURFACE

#include <windows.h>

CAIRO_BEGIN_DECLS

cairo_surface_t *
cairo_win32_surface_create (HDC hdc);

cairo_font_face_t *
cairo_win32_font_face_create_for_logfontw (LOGFONTW *logfont);

cairo_status_t
cairo_win32_scaled_font_select_font (cairo_scaled_font_t *scaled_font,
				     HDC                  hdc);

void
cairo_win32_scaled_font_done_font (cairo_scaled_font_t *scaled_font);

double
cairo_win32_scaled_font_get_metrics_factor (cairo_scaled_font_t *scaled_font);

CAIRO_END_DECLS

#else  /* CAIRO_HAS_WIN32_SURFACE */
# error Cairo was not compiled with support for the win32 backend
#endif /* CAIRO_HAS_WIN32_SURFACE */

#endif /* _CAIRO_WIN32_H_ */
