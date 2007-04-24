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

#ifndef __WIN32STRUCTS_H__
#define __WIN32STRUCTS_H__

#include "glib.h"

/* public enums and structures that GDI+ reuse from the other Windows API */

#define LF_FACESIZE 32

typedef int LANGID;
typedef int INT;
typedef guint16 UINT16;
typedef guint16 WCHAR; /* 16-bits unicode */
typedef guint32 UINT;
typedef guint32 ARGB;
typedef guint32 UINT32;
typedef gint32 PROPID;
typedef guint32 ULONG_PTR; /* not a pointer! */
typedef float REAL;

typedef void* HBITMAP;
typedef void* HFONT;
typedef void* HICON;
typedef void* HPALETTE;
typedef void* HINSTANCE;

/* mono/io-layer/uglify.h also has these typedefs.
 * To avoid a dependency on mono we have copied all
 * the required stuff here. We don't include our defs
 * if uglify.h is included somehow.
 */
#ifndef _WAPI_UGLIFY_H_		/* to avoid conflict with uglify.h */

typedef const gunichar2 *LPCTSTR;
typedef gunichar2 *LPTSTR;
typedef guint8 BYTE;
typedef guint8 *LPBYTE;
typedef guint16 WORD;
typedef guint32 DWORD;
typedef gpointer PVOID;
typedef gpointer LPVOID;
typedef gboolean BOOL;
typedef guint32 *LPDWORD;
typedef gint16 SHORT;
typedef gint32 LONG;
typedef guint32 ULONG;
typedef gint32 *PLONG;
typedef guint64 LONGLONG;
typedef gunichar2 TCHAR;
typedef size_t SIZE_T;

typedef gpointer HANDLE;
typedef gpointer *LPHANDLE;
typedef guint32 SOCKET;
typedef gpointer HMODULE;
typedef gpointer HRGN;
#define CONST const
#define VOID void

#define IN
#define OUT
#define WINAPI

#endif 		/* to avoid conflict with uglify.h */

typedef struct {
	DWORD Data1;
	WORD  Data2;
	WORD  Data3;
	BYTE  Data4 [8];
} GUID, Guid, CLSID;

typedef struct {
	LONG lfHeight;
	LONG lfWidth;
	LONG lfEscapement;
	LONG lfOrientation;
	LONG lfWeight;
	BYTE lfItalic;
	BYTE lfUnderline;
	BYTE lfStrikeOut;
	BYTE lfCharSet;
	BYTE lfOutPrecision;
	BYTE lfClipPrecision;
	BYTE lfQuality;
	BYTE lfPitchAndFamily;
	char lfFaceName[LF_FACESIZE];
} LOGFONTA;

typedef struct {
	LONG lfHeight;
	LONG lfWidth;
	LONG lfEscapement;
	LONG lfOrientation;
	LONG lfWeight;
	BYTE lfItalic;
	BYTE lfUnderline;
	BYTE lfStrikeOut;
	BYTE lfCharSet;
	BYTE lfOutPrecision;
	BYTE lfClipPrecision;
	BYTE lfQuality;
	BYTE lfPitchAndFamily;
	WCHAR lfFaceName[LF_FACESIZE];
} LOGFONTW;

#endif
