/*
 * Copyright (C) 2006-2007 Novell, Inc (http://www.novell.com)
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
 *          Sebastien Pouliot  <sebastien@ximian.com>
 */

#ifndef __GENERAL_H__
#define __GENERAL_H__

#include "win32structs.h"

/* Startup / shutdown */
typedef struct {
	UINT32	GdiplusVersion;			/* must be 1 or 2 */
	void*	DebugEventCallback;		/* ignored in libgdiplus */
	BOOL	SuppressBackgroundThread;	/* ignored in libgdiplus */
	BOOL	SuppressExternalCodecs;		/* ignored in libgdiplus */
} GdiplusStartupInput;

typedef struct {
	void*	NotificationHook;		/* ignored in libgdiplus */
	void*	NotificationUnhook;		/* ignored in libgdiplus */
} GdiplusStartupOutput;

GpStatus WINGDIPAPI GdiplusStartup (ULONG_PTR *token, const GdiplusStartupInput *input, GdiplusStartupOutput *output);
void WINGDIPAPI GdiplusShutdown (ULONG_PTR token);

/* Memory / public API */
WINGDIPAPI void*  GdipAlloc (size_t size);
WINGDIPAPI void GdipFree (void *ptr);

/* Notification API */
GpStatus WINGDIPAPI GdiplusNotificationHook (ULONG_PTR *token);
void WINGDIPAPI GdiplusNotificationUnhook (ULONG_PTR token);

/* libgdiplus-specific API, useful for quirking buggy behavior in older versions */
WINGDIPAPI char* GetLibgdiplusVersion ();

#endif
