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
 *	Frederik Carlier <frederik.carlier@quamotion.mobi>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

/*
 * NOTE: This is a private header files and everything is subject to changes.
 */

#ifndef __GRAPHICS_PRIVATE_H__
#define __GRAPHICS_PRIVATE_H__

#include "gdiplus-private.h"

#include "imageattributes-private.h"
#include "pen-private.h"
#include "region-private.h"
#include "metafile.h"

#if CAIRO_VERSION < CAIRO_VERSION_ENCODE(1,6,0)
	#define CAIRO_AA_OFFSET_X		1
	#define CAIRO_AA_OFFSET_Y		0.5
#else
	#define CAIRO_AA_OFFSET_X		0.5
	#define CAIRO_AA_OFFSET_Y		0.5
#endif

#define CURVE_MIN_TERMS			1
#define CURVE_MAX_TERMS			7

#define DEFAULT_TEXT_CONTRAST		4

/* Recursion depth of the flattening algorithm */
#define FLATTEN_RECURSION_LIMIT		10

/* not 100% identical to MS GDI+ which varies a little from int and float, but still around 0x40000000 */
#define GDIP_MAX_COORD			1073741824
#define gdip_is_overflow(a)		(((a) < -GDIP_MAX_COORD) || ((a) > GDIP_MAX_COORD))

typedef enum {
        CURVE_OPEN,
        CURVE_CLOSE 
} _CurveType;

typedef enum {
	gtUndefined,
	gtX11Drawable,
	gtMemoryBitmap,
	gtOSXDrawable,
	gtPostScript
} GraphicsType;

typedef enum {
	GraphicsBackEndInvalid	= -1,
	GraphicsBackEndCairo	= 0,
	GraphicsBackEndMetafile	= 1
} GraphicsBackEnd;

typedef struct {
	cairo_matrix_t		matrix;
	cairo_matrix_t		previous_matrix;
	GpRegion*		clip;
	cairo_matrix_t		clip_matrix;
	CompositingMode    	composite_mode;
	CompositingQuality 	composite_quality;
	InterpolationMode 	interpolation;
	GpUnit			page_unit;
	float			scale;
	SmoothingMode		draw_mode;
	TextRenderingHint 	text_mode;
	PixelOffsetMode 	pixel_mode;
	int			org_x;
	int			org_y;
	int			text_contrast;
} GpState;

typedef enum {
	GraphicsStateValid = 0,
	GraphicsStateBusy = 1
} GraphicsInternalState;

typedef struct _Graphics {
	GraphicsBackEnd		backend;
	/* cairo-specific stuff */
	cairo_t			*ct;
	GpMatrix		*copy_of_ctm;
	cairo_matrix_t		previous_matrix;
#if defined(HAVE_X11)
#ifdef CAIRO_HAS_XLIB_SURFACE
	Display			*display;
	Drawable		drawable;
#endif
#endif
	void			*image;
	int			type; 
	GpPen*			last_pen;	/* caching pen and brush to avoid unnecessary sets */
	GpBrush*		last_brush;
	float			aa_offset_x;
	float			aa_offset_y;
	/* metafile-specific stuff */
	EmfType			emf_type;
	GpMetafile		*metafile;
	cairo_surface_t		*metasurface;	/* bogus surface to satisfy some API calls */
	/* common-stuff */
	GpRegion*		clip;
	GpMatrix*		clip_matrix;
	GpRect			bounds;
	GpRect			orig_bounds;
	GpUnit			page_unit;
	float			scale;
	InterpolationMode	interpolation;
	SmoothingMode		draw_mode;
	TextRenderingHint	text_mode;
	GpState*		saved_status;
	int			saved_status_pos;
	CompositingMode		composite_mode;
	CompositingQuality	composite_quality;
	PixelOffsetMode		pixel_mode;
	int			render_origin_x;
	int			render_origin_y;
	float			dpi_x;
	float			dpi_y;
	int			text_contrast;
	GraphicsInternalState		state;
#ifdef CAIRO_HAS_QUARTZ_SURFACE
	void		*cg_context;
#endif
} Graphics;

float gdip_unit_conversion (Unit from, Unit to, float dpi, GraphicsType type, float nSrc) GDIP_INTERNAL;

void gdip_set_cairo_clipping (GpGraphics *graphics) GDIP_INTERNAL;

GpGraphics* gdip_graphics_new (cairo_surface_t *surface) GDIP_INTERNAL;
GpGraphics* gdip_metafile_graphics_new (GpMetafile *metafile) GDIP_INTERNAL;

BOOL gdip_is_scaled (GpGraphics *graphics) GDIP_INTERNAL;

/* prototypes for cairo wrappers to deal with coordonates limits, unit conversion and antialiasing) */
void gdip_cairo_rectangle (GpGraphics *graphics, double x, double y, double width, double height, BOOL antialiasing) GDIP_INTERNAL;
void gdip_cairo_move_to (GpGraphics *graphics, double x, double y, BOOL convert_units, BOOL antialiasing) GDIP_INTERNAL;
void gdip_cairo_line_to (GpGraphics *graphics, double x, double y, BOOL convert_units, BOOL antialiasing) GDIP_INTERNAL;
void gdip_cairo_curve_to (GpGraphics *graphics, double x1, double y1, double x2, double y2, double x3, double y3, 
	BOOL convert_units, BOOL antialiasing) GDIP_INTERNAL;

void gdip_cairo_set_matrix (GpGraphics *graphics, GpMatrix *matrixPageUnits) GDIP_INTERNAL;

#ifdef CAIRO_HAS_QUARTZ_SURFACE

#if __i386__
typedef float CGFloat;
#else
typedef double CGFloat;
#endif

// For the Quartz backend to function we need a few structures and function declarations.
// Unfortunately including the headers causes conflicts with internal types.  This must
// be kept in sync with any changes that might happen (albeit unlikely) to apples structures
struct CGPoint {
	CGFloat x;
	CGFloat y;
};

typedef struct CGPoint CGPoint;

struct CGSize {
	CGFloat width;
	CGFloat height;
};

typedef struct CGSize CGSize;

struct CGRect {
   CGPoint origin;
   CGSize size;
};

typedef struct CGRect CGRect;

void *CGBitmapContextCreateImage (void *context);
void CGContextDrawImage (void *context, CGRect rect, void *image);
void CGImageRelease (void *image);
void *cairo_quartz_surface_get_cg_context(cairo_surface_t *surface);
cairo_surface_t *cairo_quartz_surface_create (int format, int width, int height);
cairo_surface_t *cairo_quartz_surface_create_for_cg_context (void *ctx, int width, int height);
#endif

/* include public API */
#include "image.h"
#include "graphics.h"

#endif
