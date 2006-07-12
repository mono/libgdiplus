#ifndef CAIRO_TEST_DIRECTFB_H_
#define CAIRO_TEST_DIRECTFB_H_

/* Two functions: One for a real window, one for a bitmap */

#include <cairo.h>

CAIRO_BEGIN_DECLS

extern cairo_surface_t *
create_directfb_surface (cairo_test_t* test, cairo_format_t format,
                     void **closure);

extern void
cleanup_directfb (void* closure);

extern cairo_surface_t *
create_directfb_bitmap_surface (cairo_test_t* test, cairo_format_t format,
                            void **closure);

CAIRO_END_DECLS

#endif
