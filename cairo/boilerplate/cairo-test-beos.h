#ifndef CAIRO_TEST_BEOS_H_
#define CAIRO_TEST_BEOS_H_

/* Two functions: One for a real window, one for a bitmap */

#include <cairo.h>

CAIRO_BEGIN_DECLS

extern cairo_surface_t *
create_beos_surface (const char              *name,
                     cairo_content_t          content,
                     int                      width,
                     int                      height,
                     cairo_boilerplate_mode_t mode,
                     void                   **closure);

extern void
cleanup_beos (void* closure);

extern cairo_surface_t *
create_beos_bitmap_surface (const char              *name,
                            cairo_content_t          content,
                            int                      width,
                            int                      height,
                            cairo_boilerplate_mode_t mode,
                            void                   **closure);

extern void
cleanup_beos_bitmap (void* closure);

CAIRO_END_DECLS

#endif
