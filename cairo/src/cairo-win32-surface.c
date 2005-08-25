/* Cairo - a vector graphics library with display and print output
 *
 * Copyright © 2005 Red Hat, Inc.
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

#include <stdio.h>
#include "cairoint.h"
#include "cairo-win32-private.h"

static const cairo_surface_backend_t cairo_win32_surface_backend;

/**
 * _cairo_win32_print_gdi_error:
 * @context: context string to display along with the error
 * 
 * Helper function to dump out a human readable form of the
 * current error code.
 *
 * Return value: A cairo status code for the error code
 **/
cairo_status_t
_cairo_win32_print_gdi_error (const char *context)
{
    void *lpMsgBuf;
    DWORD last_error = GetLastError ();

    if (!FormatMessageA (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			 FORMAT_MESSAGE_FROM_SYSTEM,
			 NULL,
			 last_error,
			 MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
			 (LPTSTR) &lpMsgBuf,
			 0, NULL)) {
	fprintf (stderr, "%s: Unknown GDI error", context);
    } else {
	fprintf (stderr, "%s: %s", context, (char *)lpMsgBuf);
	
	LocalFree (lpMsgBuf);
    }

    /* We should switch off of last_status, but we'd either return
     * CAIRO_STATUS_NO_MEMORY or CAIRO_STATUS_UNKNOWN_ERROR and there
     * is no CAIRO_STATUS_UNKNOWN_ERROR.
     */

    return CAIRO_STATUS_NO_MEMORY;
}

static cairo_status_t
_create_dc_and_bitmap (cairo_win32_surface_t *surface,
		       HDC                    original_dc,
		       cairo_format_t         format,
		       int                    width,
		       int                    height,
		       char                 **bits_out,
		       int                   *rowstride_out)
{
    cairo_status_t status;

    BITMAPINFO *bitmap_info = NULL;
    struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[2];
    } bmi_stack;
    void *bits;

    int num_palette = 0;	/* Quiet GCC */
    int i;

    surface->dc = NULL;
    surface->bitmap = NULL;

    switch (format) {
    case CAIRO_FORMAT_ARGB32:
    case CAIRO_FORMAT_RGB24:
	num_palette = 0;
	break;
	
    case CAIRO_FORMAT_A8:
	num_palette = 256;
	break;
	
    case CAIRO_FORMAT_A1:
	num_palette = 2;
	break;
    }

    if (num_palette > 2) {
	bitmap_info = malloc (sizeof (BITMAPINFOHEADER) + num_palette * sizeof (RGBQUAD));
	if (!bitmap_info)
	    return CAIRO_STATUS_NO_MEMORY;
    } else {
	bitmap_info = (BITMAPINFO *)&bmi_stack;
    }

    bitmap_info->bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
    bitmap_info->bmiHeader.biWidth = width;
    bitmap_info->bmiHeader.biHeight = - height; /* top-down */
    bitmap_info->bmiHeader.biSizeImage = 0;
    bitmap_info->bmiHeader.biXPelsPerMeter = 72. / 0.0254; /* unused here */
    bitmap_info->bmiHeader.biYPelsPerMeter = 72. / 0.0254; /* unused here */
    bitmap_info->bmiHeader.biPlanes = 1;
    
    switch (format) {
    case CAIRO_FORMAT_ARGB32:
    case CAIRO_FORMAT_RGB24:
	bitmap_info->bmiHeader.biBitCount = 32;
	bitmap_info->bmiHeader.biCompression = BI_RGB;
	bitmap_info->bmiHeader.biClrUsed = 0;	/* unused */
	bitmap_info->bmiHeader.biClrImportant = 0;
	break;
	
    case CAIRO_FORMAT_A8:
	bitmap_info->bmiHeader.biBitCount = 8;
	bitmap_info->bmiHeader.biCompression = BI_RGB;
	bitmap_info->bmiHeader.biClrUsed = 256;
	bitmap_info->bmiHeader.biClrImportant = 0;

	for (i = 0; i < 256; i++) {
	    bitmap_info->bmiColors[i].rgbBlue = i;
	    bitmap_info->bmiColors[i].rgbGreen = i;
	    bitmap_info->bmiColors[i].rgbRed = i;
	    bitmap_info->bmiColors[i].rgbReserved = 0;
	}
	
	break;
	
    case CAIRO_FORMAT_A1:
	bitmap_info->bmiHeader.biBitCount = 1;
	bitmap_info->bmiHeader.biCompression = BI_RGB;
	bitmap_info->bmiHeader.biClrUsed = 2;
	bitmap_info->bmiHeader.biClrImportant = 0;

	for (i = 0; i < 2; i++) {
	    bitmap_info->bmiColors[i].rgbBlue = i * 255;
	    bitmap_info->bmiColors[i].rgbGreen = i * 255;
	    bitmap_info->bmiColors[i].rgbRed = i * 255;
	    bitmap_info->bmiColors[i].rgbReserved = 0;
	    break;
	}
    }

    surface->dc = CreateCompatibleDC (original_dc);
    if (!surface->dc)
	goto FAIL;

    surface->bitmap = CreateDIBSection (surface->dc,
			                bitmap_info,
			                DIB_RGB_COLORS,
			                &bits,
			                NULL, 0);
    if (!surface->bitmap)
	goto FAIL;

    surface->saved_dc_bitmap = SelectObject (surface->dc,
					     surface->bitmap);
    if (!surface->saved_dc_bitmap)
	goto FAIL;
    
    if (bitmap_info && num_palette > 2)
	free (bitmap_info);

    if (bits_out)
	*bits_out = bits;

    if (rowstride_out) {
	/* Windows bitmaps are padded to 32-bit (dword) boundaries */
	switch (format) {
	case CAIRO_FORMAT_ARGB32:
	case CAIRO_FORMAT_RGB24:
	    *rowstride_out = 4 * width;
	    break;
	    
	case CAIRO_FORMAT_A8:
	    *rowstride_out = (width + 3) & ~3;
	    break;
	
	case CAIRO_FORMAT_A1:
	    *rowstride_out = ((width + 31) & ~31) / 8;
	    break;
	}
    }

    return CAIRO_STATUS_SUCCESS;

 FAIL:
    status = _cairo_win32_print_gdi_error ("_create_dc_and_bitmap");
    
    if (bitmap_info && num_palette > 2)
	free (bitmap_info);

    if (surface->saved_dc_bitmap) {
	SelectObject (surface->dc, surface->saved_dc_bitmap);
	surface->saved_dc_bitmap = NULL;
    }
    
    if (surface->bitmap) {
	DeleteObject (surface->bitmap);
	surface->bitmap = NULL;
    }
    
    if (surface->dc) {
 	DeleteDC (surface->dc);
	surface->dc = NULL;
    }
 
    return status;
}

static cairo_surface_t *
_cairo_win32_surface_create_for_dc (HDC             original_dc,
				    cairo_format_t  format,
				    int	            width,
				    int	            height)
{
    cairo_status_t status;
    cairo_win32_surface_t *surface;
    char *bits;
    int rowstride;

    surface = malloc (sizeof (cairo_win32_surface_t));
    if (surface == NULL) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return &_cairo_surface_nil;
    }

    status = _create_dc_and_bitmap (surface, original_dc, format,
				    width, height,
				    &bits, &rowstride);
    if (status)
	goto FAIL;

    surface->image = cairo_image_surface_create_for_data (bits, format,
							  width, height, rowstride);
    if (surface->image->status) {
	status = CAIRO_STATUS_NO_MEMORY;
	goto FAIL;
    }
    
    surface->format = format;
    
    surface->clip_rect.x = 0;
    surface->clip_rect.y = 0;
    surface->clip_rect.width = width;
    surface->clip_rect.height = height;

    surface->set_clip = 0;
    surface->saved_clip = NULL;
    
    _cairo_surface_init (&surface->base, &cairo_win32_surface_backend);

    return (cairo_surface_t *)surface;

 FAIL:
    if (surface->bitmap) {
	SelectObject (surface->dc, surface->saved_dc_bitmap);
  	DeleteObject (surface->bitmap);
        DeleteDC (surface->dc);
    }
    if (surface)
	free (surface);

    if (status == CAIRO_STATUS_NO_MEMORY) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return &_cairo_surface_nil;
    } else {
	_cairo_error (status);
	return &_cairo_surface_nil;
    }
}

static cairo_surface_t *
_cairo_win32_surface_create_similar (void	    *abstract_src,
				     cairo_content_t content,
				     int	     width,
				     int	     height)
{
    cairo_win32_surface_t *src = abstract_src;
    cairo_format_t format = _cairo_format_from_content (content);

    return _cairo_win32_surface_create_for_dc (src->dc, format, width, height);
}

/**
 * _cairo_win32_surface_create_dib:
 * @format: format of pixels in the surface to create 
 * @width: width of the surface, in pixels
 * @height: height of the surface, in pixels
 * 
 * Creates a device-independent-bitmap surface not associated with
 * any particular existing surface or device context. The created
 * bitmap will be unititialized.
 * 
 * Return value: the newly created surface, or %NULL if it couldn't
 *   be created (probably because of lack of memory)
 **/
cairo_surface_t *
_cairo_win32_surface_create_dib (cairo_format_t format,
				 int	        width,
				 int	        height)
{
    return _cairo_win32_surface_create_for_dc (NULL, format, width, height);
}

static cairo_status_t
_cairo_win32_surface_finish (void *abstract_surface)
{
    cairo_win32_surface_t *surface = abstract_surface;

    if (surface->image)
	cairo_surface_destroy (surface->image);

    if (surface->saved_clip) {
	DeleteObject (surface->saved_clip);
    }

    /* If we created the Bitmap and DC, destroy them */
    if (surface->bitmap) {
	SelectObject (surface->dc, surface->saved_dc_bitmap);
  	DeleteObject (surface->bitmap);
        DeleteDC (surface->dc);
    }

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_win32_surface_get_subimage (cairo_win32_surface_t  *surface,
				   int                     x,
				   int                     y,
				   int                     width,
				   int                     height,
				   cairo_win32_surface_t **local_out)
{
    cairo_win32_surface_t *local;
    cairo_status_t status;
    cairo_content_t content = _cairo_content_from_format (surface->format);

    local = 
	(cairo_win32_surface_t *) _cairo_win32_surface_create_similar (surface,
								       content,
								       width,
								       height);
    if (local->base.status)
	return CAIRO_STATUS_NO_MEMORY;
    
    if (!BitBlt (local->dc, 
		 0, 0,
		 width, height,
		 surface->dc,
		 x, y,
		 SRCCOPY))
	goto FAIL;

    *local_out = local;
    
    return CAIRO_STATUS_SUCCESS;

 FAIL:
    status = _cairo_win32_print_gdi_error ("_cairo_win32_surface_get_subimage");

    if (local)
	cairo_surface_destroy (&local->base);

    return status;
}

static cairo_status_t
_cairo_win32_surface_acquire_source_image (void                    *abstract_surface,
					   cairo_image_surface_t  **image_out,
					   void                   **image_extra)
{
    cairo_win32_surface_t *surface = abstract_surface;
    cairo_win32_surface_t *local = NULL;
    cairo_status_t status;
        
    if (surface->image) {
	*image_out = (cairo_image_surface_t *)surface->image;
	*image_extra = NULL;

	return CAIRO_STATUS_SUCCESS;
    }

    status = _cairo_win32_surface_get_subimage (abstract_surface, 0, 0,
						surface->clip_rect.width,
						surface->clip_rect.height, &local);
    if (status)
	return status;

    *image_out = (cairo_image_surface_t *)local->image;
    *image_extra = local;

    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_win32_surface_release_source_image (void                   *abstract_surface,
					   cairo_image_surface_t  *image,
					   void                   *image_extra)
{
    cairo_win32_surface_t *local = image_extra;
    
    if (local)
	cairo_surface_destroy ((cairo_surface_t *)local);
}

static cairo_status_t
_cairo_win32_surface_acquire_dest_image (void                    *abstract_surface,
					 cairo_rectangle_t       *interest_rect,
					 cairo_image_surface_t  **image_out,
					 cairo_rectangle_t       *image_rect,
					 void                   **image_extra)
{
    cairo_win32_surface_t *surface = abstract_surface;
    cairo_win32_surface_t *local = NULL;
    cairo_status_t status;
    RECT clip_box;
    int x1, y1, x2, y2;
        
    if (surface->image) {
	image_rect->x = 0;
	image_rect->y = 0;
	image_rect->width = surface->clip_rect.width;
	image_rect->height = surface->clip_rect.height;

	*image_out = (cairo_image_surface_t *)surface->image;
	*image_extra = NULL;

	return CAIRO_STATUS_SUCCESS;
    }

    if (GetClipBox (surface->dc, &clip_box) == ERROR)
	return _cairo_win32_print_gdi_error ("_cairo_win3_surface_acquire_dest_image");
    
    x1 = clip_box.left;
    x2 = clip_box.right;
    y1 = clip_box.top;
    y2 = clip_box.bottom;
    
    if (interest_rect->x > x1)
	x1 = interest_rect->x;
    if (interest_rect->y > y1)
	y1 = interest_rect->y;
    if (interest_rect->x + interest_rect->width < x2)
	x2 = interest_rect->x + interest_rect->width;
    if (interest_rect->y + interest_rect->height < y2)
	y2 = interest_rect->y + interest_rect->height;
    
    if (x1 >= x2 || y1 >= y2) {
	*image_out = NULL;
	*image_extra = NULL;
	
	return CAIRO_STATUS_SUCCESS;
    }
	
    status = _cairo_win32_surface_get_subimage (abstract_surface, 
						x1, y1, x2 - x1, y2 - y1,
						&local);
    if (status)
	return status;

    *image_out = (cairo_image_surface_t *)local->image;
    *image_extra = local;
    
    image_rect->x = x1;
    image_rect->y = y1;
    image_rect->width = x2 - x1;
    image_rect->height = y2 - y1;

    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_win32_surface_release_dest_image (void                   *abstract_surface,
					 cairo_rectangle_t      *interest_rect,
					 cairo_image_surface_t  *image,
					 cairo_rectangle_t      *image_rect,
					 void                   *image_extra)
{
    cairo_win32_surface_t *surface = abstract_surface;
    cairo_win32_surface_t *local = image_extra;
    
    if (!local)
	return;

    if (!BitBlt (surface->dc,
		 image_rect->x, image_rect->y,
		 image_rect->width, image_rect->height,
		 local->dc,
		 0, 0,
		 SRCCOPY))
	_cairo_win32_print_gdi_error ("_cairo_win32_surface_release_dest_image");

    cairo_surface_destroy ((cairo_surface_t *)local);
}

#if !defined(AC_SRC_OVER)
#define AC_SRC_OVER                 0x00
#pragma pack(1)
typedef struct {
    BYTE   BlendOp;
    BYTE   BlendFlags;
    BYTE   SourceConstantAlpha;
    BYTE   AlphaFormat;
}BLENDFUNCTION;
#pragma pack()
#endif

/* for compatibility with VC++ 6 */
#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA                0x01
#endif

typedef BOOL (WINAPI *cairo_alpha_blend_func_t) (HDC hdcDest,
						 int nXOriginDest,
						 int nYOriginDest,
						 int nWidthDest,
						 int hHeightDest,
						 HDC hdcSrc,
						 int nXOriginSrc,
						 int nYOriginSrc,
						 int nWidthSrc,
						 int nHeightSrc,
						 BLENDFUNCTION blendFunction);

static cairo_int_status_t
_composite_alpha_blend (cairo_win32_surface_t *dst,
			cairo_win32_surface_t *src,
			int                    alpha,
			int                    src_x,
			int                    src_y,
			int                    dst_x,
			int                    dst_y,
			int                    width,
			int                    height)
{
    static unsigned alpha_blend_checked = FALSE;
    static cairo_alpha_blend_func_t alpha_blend = NULL;

    BLENDFUNCTION blend_function;

    /* Check for AlphaBlend dynamically to allow compiling on
     * MSVC 6 and use on older windows versions
     */
    if (!alpha_blend_checked) {
	OSVERSIONINFO os;
    
	os.dwOSVersionInfoSize = sizeof (os);
	GetVersionEx (&os);
	
	/* If running on Win98, disable using AlphaBlend()
	 * to avoid Win98 AlphaBlend() bug */
	if (VER_PLATFORM_WIN32_WINDOWS != os.dwPlatformId ||
	    os.dwMajorVersion != 4 || os.dwMinorVersion != 10)
	{
	    HMODULE msimg32_dll = LoadLibrary ("msimg32");
	    
	    if (msimg32_dll != NULL)
		alpha_blend = (cairo_alpha_blend_func_t)GetProcAddress (msimg32_dll,
									"AlphaBlend");
	}
	    
	alpha_blend_checked = TRUE;
    }

    if (alpha_blend == NULL)
	return CAIRO_INT_STATUS_UNSUPPORTED;
    
    blend_function.BlendOp = AC_SRC_OVER;
    blend_function.BlendFlags = 0;
    blend_function.SourceConstantAlpha = alpha;
    blend_function.AlphaFormat = src->format == CAIRO_FORMAT_ARGB32 ? AC_SRC_ALPHA : 0;

    if (!alpha_blend (dst->dc,
		      dst_x, dst_y,
		      width, height,
		      src->dc,
		      src_x, src_y,
		      width, height,
		      blend_function))
	return _cairo_win32_print_gdi_error ("_cairo_win32_surface_composite");
    
    return CAIRO_STATUS_SUCCESS;
}

static cairo_int_status_t
_cairo_win32_surface_composite (cairo_operator_t	operator,
				cairo_pattern_t       	*pattern,
				cairo_pattern_t		*mask_pattern,
				void			*abstract_dst,
				int			src_x,
				int			src_y,
				int			mask_x,
				int			mask_y,
				int			dst_x,
				int			dst_y,
				unsigned int		width,
				unsigned int		height)
{
    cairo_win32_surface_t *dst = abstract_dst;
    cairo_win32_surface_t *src;
    cairo_surface_pattern_t *src_surface_pattern;
    int alpha;
    int integer_transform;
    int itx, ity;

    if (pattern->type != CAIRO_PATTERN_SURFACE ||
	pattern->extend != CAIRO_EXTEND_NONE)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (mask_pattern) {
	/* FIXME: When we fully support RENDER style 4-channel
	 * masks we need to check r/g/b != 1.0.
	 */
	if (mask_pattern->type != CAIRO_PATTERN_SOLID)
	    return CAIRO_INT_STATUS_UNSUPPORTED;

	alpha = ((cairo_solid_pattern_t *)mask_pattern)->color.alpha_short >> 8;
    } else {
        alpha = 255;
    }

    src_surface_pattern = (cairo_surface_pattern_t *)pattern;
    src = (cairo_win32_surface_t *)src_surface_pattern->surface;

    if (src->base.backend != dst->base.backend)
	return CAIRO_INT_STATUS_UNSUPPORTED;
    
    integer_transform = _cairo_matrix_is_integer_translation (&pattern->matrix, &itx, &ity);
    if (!integer_transform)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (alpha == 255 &&
	src->format == dst->format &&
	(operator == CAIRO_OPERATOR_SOURCE ||
	 (src->format == CAIRO_FORMAT_RGB24 && operator == CAIRO_OPERATOR_OVER))) {
	
	if (!BitBlt (dst->dc,
		     dst_x, dst_y,
		     width, height,
		     src->dc,
		     src_x + itx, src_y + ity,
		     SRCCOPY))
	    return _cairo_win32_print_gdi_error ("_cairo_win32_surface_composite");

	return CAIRO_STATUS_SUCCESS;
	
    } else if (integer_transform &&
	       (src->format == CAIRO_FORMAT_RGB24 || src->format == CAIRO_FORMAT_ARGB32) &&
	       dst->format == CAIRO_FORMAT_RGB24 &&
	       operator == CAIRO_OPERATOR_OVER) {

	return _composite_alpha_blend (dst, src, alpha,
				       src_x + itx, src_y + ity,
				       dst_x, dst_y, width, height);
    }
    
    return CAIRO_INT_STATUS_UNSUPPORTED;
}

/* This big function tells us how to optimize operators for the
 * case of solid destination and constant-alpha source
 *
 * NOTE: This function needs revisiting if we add support for
 *       super-luminescent colors (a == 0, r,g,b > 0)
 */
static enum { DO_CLEAR, DO_SOURCE, DO_NOTHING, DO_UNSUPPORTED }
categorize_solid_dest_operator (cairo_operator_t operator,
				unsigned short   alpha)
{
    enum { SOURCE_TRANSPARENT, SOURCE_LIGHT, SOURCE_SOLID, SOURCE_OTHER } source;

    if (alpha >= 0xff00)
	source = SOURCE_SOLID;
    else if (alpha < 0x100)
	source = SOURCE_TRANSPARENT;
    else
	source = SOURCE_OTHER;
    
    switch (operator) {
    case CAIRO_OPERATOR_CLEAR:    /* 0                 0 */
    case CAIRO_OPERATOR_OUT:      /* 1 - Ab            0 */
	return DO_CLEAR;
	break;
	
    case CAIRO_OPERATOR_SOURCE:   /* 1                 0 */
    case CAIRO_OPERATOR_IN:       /* Ab                0 */
	return DO_SOURCE;
	break;

    case CAIRO_OPERATOR_OVER:     /* 1            1 - Aa */
    case CAIRO_OPERATOR_ATOP:     /* Ab           1 - Aa */
	if (source == SOURCE_SOLID)
	    return DO_SOURCE;
	else if (source == SOURCE_TRANSPARENT)
	    return DO_NOTHING;
	else
	    return DO_UNSUPPORTED;
	break;
	
    case CAIRO_OPERATOR_DEST_OUT: /* 0            1 - Aa */
    case CAIRO_OPERATOR_XOR:      /* 1 - Ab       1 - Aa */
	if (source == SOURCE_SOLID)
	    return DO_CLEAR;
	else if (source == SOURCE_TRANSPARENT)
	    return DO_NOTHING;
	else
	    return DO_UNSUPPORTED;
    	break;
	
    case CAIRO_OPERATOR_DEST:     /* 0                 1 */
    case CAIRO_OPERATOR_DEST_OVER:/* 1 - Ab            1 */
    case CAIRO_OPERATOR_SATURATE: /* min(1,(1-Ab)/Aa)  1 */
	return DO_NOTHING;
	break;

    case CAIRO_OPERATOR_DEST_IN:  /* 0                Aa */
    case CAIRO_OPERATOR_DEST_ATOP:/* 1 - Ab           Aa */
	if (source == SOURCE_SOLID)
	    return DO_NOTHING;
	else if (source == SOURCE_TRANSPARENT)
	    return DO_CLEAR;
	else
	    return DO_UNSUPPORTED;
	break;
	
    case CAIRO_OPERATOR_ADD:	  /* 1                1 */
	if (source == SOURCE_TRANSPARENT)
	    return DO_NOTHING;
	else
	    return DO_UNSUPPORTED;
	break;
    }	

    ASSERT_NOT_REACHED;
    return DO_UNSUPPORTED;
}

static cairo_int_status_t
_cairo_win32_surface_fill_rectangles (void			*abstract_surface,
				      cairo_operator_t		operator,
				      const cairo_color_t	*color,
				      cairo_rectangle_t		*rects,
				      int			num_rects)
{
    cairo_win32_surface_t *surface = abstract_surface;
    cairo_status_t status;
    COLORREF new_color;
    HBRUSH new_brush;
    int i;

    /* If we have a local image, use the fallback code; it will be as fast
     * as calling out to GDI.
     */
    if (surface->image)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    /* Optimize for no destination alpha (surface->pixman_image is non-NULL for all
     * surfaces with alpha.)
     */
    switch (categorize_solid_dest_operator (operator, color->alpha_short)) {
    case DO_CLEAR:
	new_color = RGB (0, 0, 0);
	break;	
    case DO_SOURCE:
	new_color = RGB (color->red_short >> 8, color->green_short >> 8, color->blue_short >> 8);
	break;
    case DO_NOTHING:
	return CAIRO_STATUS_SUCCESS;
    case DO_UNSUPPORTED:
    default:
	return CAIRO_INT_STATUS_UNSUPPORTED;
    }
    
    new_brush = CreateSolidBrush (new_color);
    if (!new_brush)
	return _cairo_win32_print_gdi_error ("_cairo_win32_surface_fill_rectangles");
    
    for (i = 0; i < num_rects; i++) {
	RECT rect;

	rect.left = rects[i].x;
	rect.top = rects[i].y;
	rect.right = rects[i].x + rects[i].width;
	rect.bottom = rects[i].y + rects[i].height;

	if (!FillRect (surface->dc, &rect, new_brush))
	    goto FAIL;
    }

    DeleteObject (new_brush);
    
    return CAIRO_STATUS_SUCCESS;

 FAIL:
    status = _cairo_win32_print_gdi_error ("_cairo_win32_surface_fill_rectangles");
    
    DeleteObject (new_brush);
    
    return status;
}

static cairo_int_status_t
_cairo_win32_surface_set_clip_region (void              *abstract_surface,
				      pixman_region16_t *region)
{
    cairo_win32_surface_t *surface = abstract_surface;
    cairo_status_t status;

    /* If we are in-memory, then we set the clip on the image surface
     * as well as on the underlying GDI surface.
     */
    if (surface->image) {
	unsigned int serial;

	serial = _cairo_surface_allocate_clip_serial (surface->image);
	_cairo_surface_set_clip_region (surface->image, region, serial);
    }

    /* The semantics we want is that any clip set by cairo combines
     * is intersected with the clip on device context that the
     * surface was created for. To implement this, we need to
     * save the original clip when first setting a clip on surface.
     */

    if (region == NULL) {
	/* Clear any clip set by cairo, return to the original */
	
	if (surface->set_clip) {
	    if (SelectClipRgn (surface->dc, surface->saved_clip) == ERROR)
		return _cairo_win32_print_gdi_error ("_cairo_win32_surface_set_clip_region");

	    if (surface->saved_clip) {
		DeleteObject (surface->saved_clip);
		surface->saved_clip = NULL;
	    }

	    surface->set_clip = 0;
	}
	    
	return CAIRO_STATUS_SUCCESS;
    
    } else {
	pixman_box16_t *boxes = pixman_region_rects (region);
	int num_boxes = pixman_region_num_rects (region);
	pixman_box16_t *extents = pixman_region_extents (region);
	RGNDATA *data;
	size_t data_size;
	RECT *rects;
	int i;
	HRGN gdi_region;

	/* Create a GDI region for the cairo region */

	data_size = sizeof (RGNDATAHEADER) + num_boxes * sizeof (RECT);
	data = malloc (data_size);
	if (!data)
	    return CAIRO_STATUS_NO_MEMORY;
	rects = (RECT *)data->Buffer;

	data->rdh.dwSize = sizeof (RGNDATAHEADER);
	data->rdh.iType = RDH_RECTANGLES;
	data->rdh.nCount = num_boxes;
	data->rdh.nRgnSize = num_boxes * sizeof (RECT);
	data->rdh.rcBound.left = extents->x1;
	data->rdh.rcBound.top = extents->y1;
	data->rdh.rcBound.right = extents->x2;
	data->rdh.rcBound.bottom = extents->y2;

	for (i = 0; i < num_boxes; i++) {
	    rects[i].left = boxes[i].x1;
	    rects[i].top = boxes[i].y1;
	    rects[i].right = boxes[i].x2;
	    rects[i].bottom = boxes[i].y2;
	}

	gdi_region = ExtCreateRegion (NULL, data_size, data);
	free (data);
	
	if (!gdi_region)
	    return CAIRO_STATUS_NO_MEMORY;

	if (surface->set_clip) {
	    /* Combine the new region with the original clip */
	    
	    if (surface->saved_clip) {
		if (CombineRgn (gdi_region, gdi_region, surface->saved_clip, RGN_AND) == ERROR)
		    goto FAIL;
	    }

	    if (SelectClipRgn (surface->dc, gdi_region) == ERROR)
		goto FAIL;
		
	} else {
	    /* Save the the current region */

	    surface->saved_clip = CreateRectRgn (0, 0, 0, 0);
	    if (!surface->saved_clip) {
		goto FAIL;	    }

	    /* This function has no error return! */
	    if (GetClipRgn (surface->dc, surface->saved_clip) == 0) { /* No clip */
		DeleteObject (surface->saved_clip);
		surface->saved_clip = NULL;
	    }
		
	    if (ExtSelectClipRgn (surface->dc, gdi_region, RGN_AND) == ERROR)
		goto FAIL;

	    surface->set_clip = 1;
	}

	DeleteObject (gdi_region);
	return CAIRO_STATUS_SUCCESS;

    FAIL:
	status = _cairo_win32_print_gdi_error ("_cairo_win32_surface_set_clip_region");
	DeleteObject (gdi_region);
	return status;
    }
}

static cairo_int_status_t
_cairo_win32_surface_get_extents (void		    *abstract_surface,
				  cairo_rectangle_t *rectangle)
{
    cairo_win32_surface_t *surface = abstract_surface;
    RECT clip_box;

    if (GetClipBox (surface->dc, &clip_box) == ERROR)
	return _cairo_win32_print_gdi_error ("_cairo_win3_surface_acquire_dest_image");

    rectangle->x = clip_box.left;
    rectangle->y = clip_box.top;
    rectangle->width  = clip_box.right  - clip_box.left;
    rectangle->height = clip_box.bottom - clip_box.top;

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_win32_surface_flush (void *abstract_surface)
{
    return _cairo_surface_reset_clip (abstract_surface);
}

cairo_surface_t *
cairo_win32_surface_create (HDC hdc)
{
    cairo_win32_surface_t *surface;
    RECT rect;

    /* Try to figure out the drawing bounds for the Device context
     */
    if (GetClipBox (hdc, &rect) == ERROR) {
	_cairo_win32_print_gdi_error ("cairo_win32_surface_create");
	/* XXX: Can we make a more reasonable guess at the error cause here? */
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return &_cairo_surface_nil;
    }
    
    surface = malloc (sizeof (cairo_win32_surface_t));
    if (surface == NULL) {
	_cairo_error (CAIRO_STATUS_NO_MEMORY);
	return &_cairo_surface_nil;
    }

    surface->image = NULL;
    surface->format = CAIRO_FORMAT_RGB24;
    
    surface->dc = hdc;
    surface->bitmap = NULL;
    surface->saved_dc_bitmap = NULL;
    
    surface->clip_rect.x = rect.left;
    surface->clip_rect.y = rect.top;
    surface->clip_rect.width = rect.right - rect.left;
    surface->clip_rect.height = rect.bottom - rect.top;

    surface->set_clip = 0;
    surface->saved_clip = NULL;

    _cairo_surface_init (&surface->base, &cairo_win32_surface_backend);

    return (cairo_surface_t *)surface;
}

/**
 * _cairo_surface_is_win32:
 * @surface: a #cairo_surface_t
 * 
 * Checks if a surface is an #cairo_win32_surface_t
 * 
 * Return value: True if the surface is an win32 surface
 **/
int
_cairo_surface_is_win32 (cairo_surface_t *surface)
{
    return surface->backend == &cairo_win32_surface_backend;
}

static const cairo_surface_backend_t cairo_win32_surface_backend = {
    _cairo_win32_surface_create_similar,
    _cairo_win32_surface_finish,
    _cairo_win32_surface_acquire_source_image,
    _cairo_win32_surface_release_source_image,
    _cairo_win32_surface_acquire_dest_image,
    _cairo_win32_surface_release_dest_image,
    NULL, /* clone_similar */
    _cairo_win32_surface_composite,
    _cairo_win32_surface_fill_rectangles,
    NULL, /* composite_trapezoids */
    NULL, /* copy_page */
    NULL, /* show_page */
    _cairo_win32_surface_set_clip_region,
    NULL, /* intersect_clip_path */
    _cairo_win32_surface_get_extents,
    NULL, /* show_glyphs */
    NULL, /* fill_path */
    NULL, /* get_font_options */
    _cairo_win32_surface_flush,
    NULL  /* mark_dirty_rectangle */
};
