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

#define LF_FACESIZE		32

/* SetBkMode */
#define TRANSPARENT		1
#define OPAQUE			2

/* SetMapMode */
#define MM_TEXT			1
#define MM_LOMETRIC		2
#define MM_HIMETRIC		3
#define MM_LOENGLISH		4
#define MM_HIENGLISH		5
#define MM_TWIPS		6
#define MM_ISOTROPIC		7
#define MM_ANISOTROPIC		8

/* CreatePenIndirect */
#define PS_NULL			0x00000005
#define PS_STYLE_MASK		0x0000000F
#define PS_ENDCAP_ROUND		0x00000000
#define PS_ENDCAP_SQUARE	0x00000100
#define PS_ENDCAP_FLAT		0x00000200
#define PS_ENDCAP_MASK		0x00000F00
#define PS_JOIN_ROUND		0x00000000
#define PS_JOIN_BEVEL		0x00001000
#define PS_JOIN_MITER		0x00002000
#define PS_JOIN_MASK		0x0000F000

/* CreateBrushIndirect */
#define BS_SOLID		0
#define BS_NULL			1
#define BS_HATCHED		2
#define BS_PATTERN		3
#define BS_INDEXED		4

/* SetPolyFillMode */
#define ALTERNATE		1
#define WINDING			2

/* SetRelabs */
#define ABSOLUTE		1
#define RELATIVE		2

/* ModifyWorldTransform */
#define MWT_IDENTITY		1
#define MWT_LEFTMULTIPLY	2
#define MWT_RIGHTMULTIPLY	3

typedef int LANGID;
typedef int INT;
typedef guint16 WCHAR; /* 16-bits unicode */
typedef guint32 UINT;
typedef guint32 ARGB;
typedef guint32 UINT32;
typedef gint32 PROPID;
typedef guint32 ULONG_PTR; /* not a pointer! */
typedef float REAL;

typedef gpointer HBITMAP;
typedef gpointer HDC;
typedef gpointer HENHMETAFILE;
typedef gpointer HFONT;
typedef gpointer HICON;
typedef gpointer HINSTANCE;
typedef gpointer HMETAFILE;
typedef gpointer HPALETTE;

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

typedef struct {
	float eM11;
	float eM12;
	float eM21;
	float eM22;
	float eDx;
	float eDy;
} XFORM;

typedef struct {
	LONG	x;
	LONG	y;
} POINT;

typedef DWORD COLORREF;

typedef struct {
	UINT		lopnStyle;
	POINT		lopnWidth;
	COLORREF	lopnColor;
} LOGPEN;

typedef struct {
	UINT		lbStyle;
	COLORREF	lbColor;
	LONG		lbHatch;
} LOGBRUSH;

typedef struct {
	int	left;
	int	top;
	int	right;
	int	bottom;
} RECT, RECTL;

typedef struct {
	int	cx;
	int	cy; 
} SIZE, SIZEL;

typedef struct {
	SHORT	Left;
	SHORT	Top;
	SHORT	Right;
	SHORT	Bottom;
} PWMFRect16;

#ifndef __GNUC__
	#pragma pack(2)
#endif

typedef struct
#ifdef __GNUC__
	 __attribute__ ((packed))
#endif
{
	WORD	mtType;			/* 1 for disk, 0 for memory */
	WORD	mtHeaderSize;
	WORD	mtVersion;
	DWORD	mtSize;
	WORD	mtNoObjects;
	DWORD	mtMaxRecord;
	WORD	mtNoParameters;
} METAHEADER;

typedef struct
#ifdef __GNUC__
	 __attribute__ ((packed))
#endif
{
	DWORD		Key;
	SHORT		Hmf;
	PWMFRect16	BoundingBox;
	SHORT		Inch;
	DWORD		Reserved;
	SHORT		Checksum;
} WmfPlaceableFileHeader;

#ifndef __GNUC__
	#pragma pack()
#endif

typedef struct {
	DWORD	iType;
	DWORD	nSize;
	RECTL	rclBounds;
	RECTL	rclFrame;
	DWORD	dSignature;
	DWORD	nVersion;
	DWORD	nBytes;
	DWORD	nRecords;
	WORD	nHandles;
	WORD	sReserved;
	DWORD	nDescription;
	DWORD	offDescription;
	DWORD	nPalEntries;
	SIZEL	szlDevice;
	SIZEL	szlMillimeters;
} ENHMETAHEADER3;

typedef struct {
	int	Type;
	int	Size;
	int	Version;
	int	EmfPlusFlags;
	float	DpiX;
	float	DpiY;
	int	X;
	int	Y;
	int	Width;
	int	Height;
	union {
		METAHEADER	WmfHeader;
		ENHMETAHEADER3	EmfHeader;
	};
	int	EmfPlusHeaderSize;
	int	LogicalDpiX;
	int	LogicalDpiY;
} MetafileHeader;

#endif
