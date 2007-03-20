#ifndef CAIRO_TEST_DIRECTFB_H_
#define CAIRO_TEST_DIRECTFB_H_

/* Two functions: One for a real window, one for a bitmap */

#include <cairo.h>

CAIRO_BEGIN_DECLS

extern void
cleanup_directfb (void* closure);

extern cairo_surface_t *
create_directfb_surface (const char			 *name,
		     cairo_content_t		  content,
		     int			  width,
		     int			  height,
		     cairo_boilerplate_mode_t	  mode,
		     void			**closure); 

CAIRO_END_DECLS

#endif
