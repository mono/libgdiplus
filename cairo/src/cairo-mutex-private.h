/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2002 University of Southern California
 * Copyright © 2005 Red Hat, Inc.
 * Copyright © 2007 Mathias Hasselmann
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
 * The Initial Developer of the Original Code is University of Southern
 * California.
 *
 * Contributor(s):
 *	Carl D. Worth <cworth@cworth.org>
 *	Mathias Hasselmann <mathias.hasselmann@gmx.de>
 */

#ifndef CAIRO_MUTEX_PRIVATE_H
#define CAIRO_MUTEX_PRIVATE_H

CAIRO_BEGIN_DECLS

#if HAVE_PTHREAD_H /*********************************************************/

# include <pthread.h>

  typedef pthread_mutex_t cairo_mutex_t;

# define CAIRO_MUTEX_INITIALIZE() /* no-op */
# define CAIRO_MUTEX_LOCK(name) pthread_mutex_lock (&name)
# define CAIRO_MUTEX_UNLOCK(name) pthread_mutex_unlock (&name)
# define CAIRO_MUTEX_INIT(mutex) do {				\
    pthread_mutex_t tmp_mutex = PTHREAD_MUTEX_INITIALIZER;      \
    memcpy (mutex, &tmp_mutex, sizeof (tmp_mutex));             \
} while (0)
# define CAIRO_MUTEX_FINI(mutex) pthread_mutex_destroy (mutex)
# define CAIRO_MUTEX_NIL_INITIALIZER PTHREAD_MUTEX_INITIALIZER

#elif defined CAIRO_HAS_WIN32_SURFACE /**************************************/

/* We require Windows 2000 features. Although we don't use them here, things
 * should still work if this header file ends up being the one to include
 * windows.h into a source file, so: */
# if !defined(WINVER) || (WINVER < 0x0500)
#  define WINVER 0x0500
# endif

# if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0500)
#  define _WIN32_WINNT 0x0500
# endif

# define WIN32_LEAN_AND_MEAN
# include <windows.h>

  typedef CRITICAL_SECTION cairo_mutex_t;

# define CAIRO_MUTEX_LOCK(name) EnterCriticalSection (&name)
# define CAIRO_MUTEX_UNLOCK(name) LeaveCriticalSection (&name)
# define CAIRO_MUTEX_INIT(mutex) InitializeCriticalSection (mutex)
# define CAIRO_MUTEX_FINI(mutex) DeleteCriticalSection (mutex)
# define CAIRO_MUTEX_NIL_INITIALIZER { NULL, 0, 0, NULL, NULL, 0 }

#elif defined __OS2__ /******************************************************/

# define INCL_BASE
# define INCL_PM
# include <os2.h>

  typedef HMTX cairo_mutex_t;

# define CAIRO_MUTEX_LOCK(name) DosRequestMutexSem(name, SEM_INDEFINITE_WAIT)
# define CAIRO_MUTEX_UNLOCK(name) DosReleaseMutexSem(name)
# define CAIRO_MUTEX_INIT(mutex) DosCreateMutexSem (NULL, mutex, 0L, FALSE)
# define CAIRO_MUTEX_FINI(mutex) do {				\
    if (0 != (mutex)) {						\
        DosCloseMutexSem (*(mutex));				\
        (mutex) = 0;						\
    }								\
} while (0)
# define CAIRO_MUTEX_NIL_INITIALIZER 0

#elif defined CAIRO_HAS_BEOS_SURFACE /***************************************/

  typedef void* cairo_mutex_t;

  cairo_private void _cairo_beos_lock(cairo_mutex_t*);
  cairo_private void _cairo_beos_unlock(cairo_mutex_t*);

/* the real initialization takes place in a global constructor */
# define CAIRO_MUTEX_LOCK(name) _cairo_beos_lock (&name)
# define CAIRO_MUTEX_UNLOCK(name) _cairo_beos_unlock (&name)

# error "XXX: Someone who understands BeOS needs to add definitions for" \
        "     cairo_mutex_t, CAIRO_MUTEX_INIT, and CAIRO_MUTEX_FINI," \
        "     to cairoint.h"

# define CAIRO_MUTEX_INIT(mutex) ???
# define CAIRO_MUTEX_FINI(mutex) ???
# define CAIRO_MUTEX_NIL_INITIALIZER {}

#else /**********************************************************************/

# define CAIRO_MUTEX_LOCK(name)
# define CAIRO_MUTEX_UNLOCK(name)

#endif  /********************************************************************/

#ifndef CAIRO_MUTEX_DECLARE
#define CAIRO_MUTEX_DECLARE(name) extern cairo_mutex_t name;
#endif

#include "cairo-mutex-list-private.h"

#undef CAIRO_MUTEX_DECLARE
#undef CAIRO_MUTEX_EXTERNAL

#ifndef CAIRO_MUTEX_INITIALIZE

#define CAIRO_MUTEX_INITIALIZE() do { \
    if (!_cairo_mutex_initialized) \
        _cairo_mutex_initialize(); \
} while(0)

cairo_private extern cairo_bool_t _cairo_mutex_initialized;
cairo_private void _cairo_mutex_initialize(void);

#endif

CAIRO_END_DECLS

#endif





