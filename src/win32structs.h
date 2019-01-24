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

#include <glib.h>

/* public enums and structures that GDI+ reuse from the other Windows API */

#define LF_FACESIZE		32

#define META_SETBKCOLOR              0x0201
#define META_SETBKMODE               0x0102
#define META_SETMAPMODE              0x0103
#define META_SETROP2                 0x0104
#define META_SETRELABS               0x0105
#define META_SETPOLYFILLMODE         0x0106
#define META_SETSTRETCHBLTMODE       0x0107
#define META_SETTEXTCHAREXTRA        0x0108
#define META_SETTEXTCOLOR            0x0209
#define META_SETTEXTJUSTIFICATION    0x020A
#define META_SETWINDOWORG            0x020B
#define META_SETWINDOWEXT            0x020C
#define META_SETVIEWPORTORG          0x020D
#define META_SETVIEWPORTEXT          0x020E
#define META_OFFSETWINDOWORG         0x020F
#define META_SCALEWINDOWEXT          0x0410
#define META_OFFSETVIEWPORTORG       0x0211
#define META_SCALEVIEWPORTEXT        0x0412
#define META_LINETO                  0x0213
#define META_MOVETO                  0x0214
#define META_EXCLUDECLIPRECT         0x0415
#define META_INTERSECTCLIPRECT       0x0416
#define META_ARC                     0x0817
#define META_ELLIPSE                 0x0418
#define META_FLOODFILL               0x0419
#define META_PIE                     0x081A
#define META_RECTANGLE               0x041B
#define META_ROUNDRECT               0x061C
#define META_PATBLT                  0x061D
#define META_SAVEDC                  0x001E
#define META_SETPIXEL                0x041F
#define META_OFFSETCLIPRGN           0x0220
#define META_TEXTOUT                 0x0521
#define META_BITBLT                  0x0922
#define META_STRETCHBLT              0x0B23
#define META_POLYGON                 0x0324
#define META_POLYLINE                0x0325
#define META_ESCAPE                  0x0626
#define META_RESTOREDC               0x0127
#define META_FILLREGION              0x0228
#define META_FRAMEREGION             0x0429
#define META_INVERTREGION            0x012A
#define META_PAINTREGION             0x012B
#define META_SELECTCLIPREGION        0x012C
#define META_SELECTOBJECT            0x012D
#define META_SETTEXTALIGN            0x012E
#define META_CHORD                   0x0830
#define META_SETMAPPERFLAGS          0x0231
#define META_EXTTEXTOUT              0x0a32
#define META_SETDIBTODEV             0x0d33
#define META_SELECTPALETTE           0x0234
#define META_REALIZEPALETTE          0x0035
#define META_ANIMATEPALETTE          0x0436
#define META_SETPALENTRIES           0x0037
#define META_POLYPOLYGON             0x0538
#define META_RESIZEPALETTE           0x0139
#define META_DIBBITBLT               0x0940
#define META_DIBSTRETCHBLT           0x0b41
#define META_DIBCREATEPATTERNBRUSH   0x0142
#define META_STRETCHDIB              0x0f43
#define META_EXTFLOODFILL            0x0548
#define META_SETLAYOUT               0x0149
#define META_DELETEOBJECT            0x01f0
#define META_CREATEPALETTE           0x00f7
#define META_CREATEPATTERNBRUSH      0x01F9
#define META_CREATEPENINDIRECT       0x02FA
#define META_CREATEFONTINDIRECT      0x02FB
#define META_CREATEBRUSHINDIRECT     0x02FC
#define META_CREATEREGION            0x06FF

#define ENHMETA_STOCK_OBJECT    0x80000000

#define EMR_HEADER                      1
#define EMR_POLYBEZIER                  2
#define EMR_POLYGON                     3
#define EMR_POLYLINE                    4
#define EMR_POLYBEZIERTO                5
#define EMR_POLYLINETO                  6
#define EMR_POLYPOLYLINE                7
#define EMR_POLYPOLYGON                 8
#define EMR_SETWINDOWEXTEX              9
#define EMR_SETWINDOWORGEX              10
#define EMR_SETVIEWPORTEXTEX            11
#define EMR_SETVIEWPORTORGEX            12
#define EMR_SETBRUSHORGEX               13
#define EMR_EOF                         14
#define EMR_SETPIXELV                   15
#define EMR_SETMAPPERFLAGS              16
#define EMR_SETMAPMODE                  17
#define EMR_SETBKMODE                   18
#define EMR_SETPOLYFILLMODE             19
#define EMR_SETROP2                     20
#define EMR_SETSTRETCHBLTMODE           21
#define EMR_SETTEXTALIGN                22
#define EMR_SETCOLORADJUSTMENT          23
#define EMR_SETTEXTCOLOR                24
#define EMR_SETBKCOLOR                  25
#define EMR_OFFSETCLIPRGN               26
#define EMR_MOVETOEX                    27
#define EMR_SETMETARGN                  28
#define EMR_EXCLUDECLIPRECT             29
#define EMR_INTERSECTCLIPRECT           30
#define EMR_SCALEVIEWPORTEXTEX          31
#define EMR_SCALEWINDOWEXTEX            32
#define EMR_SAVEDC                      33
#define EMR_RESTOREDC                   34
#define EMR_SETWORLDTRANSFORM           35
#define EMR_MODIFYWORLDTRANSFORM        36
#define EMR_SELECTOBJECT                37
#define EMR_CREATEPEN                   38
#define EMR_CREATEBRUSHINDIRECT         39
#define EMR_DELETEOBJECT                40
#define EMR_ANGLEARC                    41
#define EMR_ELLIPSE                     42
#define EMR_RECTANGLE                   43
#define EMR_ROUNDRECT                   44
#define EMR_ARC                         45
#define EMR_CHORD                       46
#define EMR_PIE                         47
#define EMR_SELECTPALETTE               48
#define EMR_CREATEPALETTE               49
#define EMR_SETPALETTEENTRIES           50
#define EMR_RESIZEPALETTE               51
#define EMR_REALIZEPALETTE              52
#define EMR_EXTFLOODFILL                53
#define EMR_LINETO                      54
#define EMR_ARCTO                       55
#define EMR_POLYDRAW                    56
#define EMR_SETARCDIRECTION             57
#define EMR_SETMITERLIMIT               58
#define EMR_BEGINPATH                   59
#define EMR_ENDPATH                     60
#define EMR_CLOSEFIGURE                 61
#define EMR_FILLPATH                    62
#define EMR_STROKEANDFILLPATH           63
#define EMR_STROKEPATH                  64
#define EMR_FLATTENPATH                 65
#define EMR_WIDENPATH                   66
#define EMR_SELECTCLIPPATH              67
#define EMR_ABORTPATH                   68

#define EMR_GDICOMMENT                  70
#define EMR_FILLRGN                     71
#define EMR_FRAMERGN                    72
#define EMR_INVERTRGN                   73
#define EMR_PAINTRGN                    74
#define EMR_EXTSELECTCLIPRGN            75
#define EMR_BITBLT                      76
#define EMR_STRETCHBLT                  77
#define EMR_MASKBLT                     78
#define EMR_PLGBLT                      79
#define EMR_SETDIBITSTODEVICE           80
#define EMR_STRETCHDIBITS               81
#define EMR_EXTCREATEFONTINDIRECTW      82
#define EMR_EXTTEXTOUTA                 83
#define EMR_EXTTEXTOUTW                 84
#define EMR_POLYBEZIER16                85
#define EMR_POLYGON16                   86
#define EMR_POLYLINE16                  87
#define EMR_POLYBEZIERTO16              88
#define EMR_POLYLINETO16                89
#define EMR_POLYPOLYLINE16              90
#define EMR_POLYPOLYGON16               91
#define EMR_POLYDRAW16                  92
#define EMR_CREATEMONOBRUSH             93
#define EMR_CREATEDIBPATTERNBRUSHPT     94
#define EMR_EXTCREATEPEN                95
#define EMR_POLYTEXTOUTA                96
#define EMR_POLYTEXTOUTW                97

#define EMR_SETICMMODE                  98
#define EMR_CREATECOLORSPACE            99
#define EMR_SETCOLORSPACE              100
#define EMR_DELETECOLORSPACE           101
#define EMR_GLSRECORD                  102
#define EMR_GLSBOUNDEDRECORD           103
#define EMR_PIXELFORMAT                104

#define EMR_RESERVED_105               105
#define EMR_RESERVED_106               106
#define EMR_RESERVED_107               107
#define EMR_RESERVED_108               108
#define EMR_RESERVED_109               109
#define EMR_RESERVED_110               110
#define EMR_COLORCORRECTPALETTE        111
#define EMR_SETICMPROFILEA             112
#define EMR_SETICMPROFILEW             113
#define EMR_ALPHABLEND                 114
#define EMR_SETLAYOUT                  115
#define EMR_TRANSPARENTBLT             116
#define EMR_RESERVED_117               117
#define EMR_GRADIENTFILL               118
#define EMR_RESERVED_119               119
#define EMR_RESERVED_120               120
#define EMR_COLORMATCHTOTARGETW        121
#define EMR_CREATECOLORSPACEW          122

#define EMR_MIN                          1
#define EMR_MAX                         97

#define GDICOMMENT_IDENTIFIER           0x43494447
#define GDICOMMENT_WINDOWS_METAFILE     0x80000001
#define GDICOMMENT_BEGINGROUP           0x00000002
#define GDICOMMENT_ENDGROUP             0x00000003
#define GDICOMMENT_MULTIFORMATS         0x40000004
#define EPS_SIGNATURE                   0x46535045
#define GDICOMMENT_UNICODE_STRING       0x00000040
#define GDICOMMENT_UNICODE_END          0x0000008

#define WHITE_BRUSH         0
#define LTGRAY_BRUSH        1
#define GRAY_BRUSH          2
#define DKGRAY_BRUSH        3
#define BLACK_BRUSH         4
#define NULL_BRUSH          5
#define HOLLOW_BRUSH        NULL_BRUSH
#define WHITE_PEN           6
#define BLACK_PEN           7
#define NULL_PEN            8
#define OEM_FIXED_FONT      10
#define ANSI_FIXED_FONT     11
#define ANSI_VAR_FONT       12
#define SYSTEM_FONT         13
#define DEVICE_DEFAULT_FONT 14
#define DEFAULT_PALETTE     15
#define SYSTEM_FIXED_FONT   16
#define DEFAULT_GUI_FONT    17
#define DC_BRUSH            18
#define DC_PEN              19
#define STOCK_LAST          19

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
#define PS_SOLID         0x00000000
#define PS_DASH          0x00000001
#define PS_DOT           0x00000002
#define PS_DASHDOT       0x00000003
#define PS_DASHDOTDOT    0x00000004
#define PS_NULL          0x00000005
#define PS_INSIDEFRAME   0x00000006
#define PS_USERSTYLE     0x00000007
#define PS_ALTERNATE     0x00000008
#define PS_STYLE_MASK    0x0000000F

#define PS_ENDCAP_ROUND	0x00000000
#define PS_ENDCAP_SQUARE	0x00000100
#define PS_ENDCAP_FLAT		0x00000200
#define PS_ENDCAP_MASK		0x00000F00

#define PS_JOIN_ROUND		0x00000000
#define PS_JOIN_BEVEL		0x00001000
#define PS_JOIN_MITER		0x00002000
#define PS_JOIN_MASK		0x0000F000

#define PS_COSMETIC		0x00000000
#define PS_GEOMETRIC     	0x00010000
#define PS_TYPE_MASK     	0x000F0000

/* CreateBrushIndirect */
#define BS_SOLID		0
#define BS_NULL		1
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

/* Compression */
#define BI_RGB           0
#define BI_RLE8          1
#define BI_RLE4          2
#define BI_BITFIELDS     3

typedef float REAL;

#if defined(WIN32)
#define NOGDI
#include <Windows.h>
#else

typedef int INT;
typedef guint16 WCHAR; /* 16-bits unicode */
typedef guint32 UINT;
typedef guint16 UINT16;
typedef guint32 UINT32;
typedef gint32 PROPID;
typedef guint32 ULONG_PTR; /* not a pointer! */

typedef gpointer HBITMAP;
typedef gpointer HDC;
typedef gpointer HENHMETAFILE;
typedef gpointer HFONT;
typedef gpointer HICON;
typedef gpointer HINSTANCE;
typedef gpointer HMETAFILE;
typedef gpointer HPALETTE;
typedef gpointer HWND;

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
typedef guint64  DWORDLONG;
typedef gpointer PVOID;
typedef gpointer LPVOID;
typedef gboolean BOOL;
typedef guint32 *LPDWORD;
typedef gint16 SHORT;
typedef guint16 USHORT;
typedef gint32 LONG;
typedef guint32 ULONG;
typedef gint32 *PLONG;
typedef guint64 LONGLONG;
typedef guint64 UINT_PTR;
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

typedef WORD LANGID;

typedef struct {
	DWORD Data1;
	WORD  Data2;
	WORD  Data3;
	BYTE  Data4 [8];
} GUID, Guid, CLSID;
#endif

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

#if !defined(WIN32)
typedef struct {
	LONG	x;
	LONG	y;
} POINT;
#endif

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

#if !defined(WIN32)
typedef struct {
	LONG	left;
	LONG	top;
	LONG	right;
	LONG	bottom;
} RECT, RECTL;

typedef struct {
	LONG    cx;
	LONG    cy;
} SIZE, SIZEL;
#endif

typedef struct {
	SHORT	Left;
	SHORT	Top;
	SHORT	Right;
	SHORT	Bottom;
} PWMFRect16;

typedef struct {
	BYTE rgbBlue;
	BYTE rgbGreen;
	BYTE rgbRed;
	BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;

typedef LONG FXPT2DOT30;

typedef struct {
	FXPT2DOT30	ciexyzX;
	FXPT2DOT30	ciexyzY;
	FXPT2DOT30	ciexyzZ;
} CIEXYZ;

typedef struct {
	CIEXYZ	ciexyzRed;
	CIEXYZ	ciexyzGreen;
	CIEXYZ	ciexyzBlue;
} CIEXYZTRIPLE;

typedef struct {
	DWORD 	biSize;
	LONG  	biWidth;
	LONG  	biHeight;
	WORD 	biPlanes;
	WORD 	biBitCount;
	DWORD 	biCompression;
	DWORD 	biSizeImage;
	LONG  	biXPelsPerMeter;
	LONG  	biYPelsPerMeter;
	DWORD 	biClrUsed;
	DWORD 	biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;

typedef struct {
	DWORD 	bV4Size;
	LONG  	bV4Width;
	LONG  	bV4Height;
	WORD 	bV4Planes;
	WORD 	bV4BitCount;
	DWORD 	bV4Compression;
	DWORD 	bV4SizeImage;
	LONG  	bV4XPelsPerMeter;
	LONG  	bV4YPelsPerMeter;
	DWORD 	bV4ClrUsed;
	DWORD 	bV4ClrImportant;
	DWORD	bV4RedMask;
	DWORD	bV4GreenMask;
	DWORD	bV4BlueMask;
	DWORD	bV4AlphaMask;
	DWORD	bV4CSType;
	CIEXYZTRIPLE	bV4Endpoints;
	DWORD	bV4GammaRed;
	DWORD	bV4GammaGreen;
	DWORD	bV4GammaBlue;
} BITMAPV4HEADER, *PBITMAPV4HEADER;

typedef struct {
	DWORD        bV5Size;
	LONG         bV5Width;
	LONG         bV5Height;
	WORD         bV5Planes;
	WORD         bV5BitCount;
	DWORD        bV5Compression;
	DWORD        bV5SizeImage;
	LONG         bV5XPelsPerMeter;
	LONG         bV5YPelsPerMeter;
	DWORD        bV5ClrUsed;
	DWORD        bV5ClrImportant;
	DWORD        bV5RedMask;
	DWORD        bV5GreenMask;
	DWORD        bV5BlueMask;
	DWORD        bV5AlphaMask;
	DWORD        bV5CSType;
	CIEXYZTRIPLE bV5Endpoints;
	DWORD        bV5GammaRed;
	DWORD        bV5GammaGreen;
	DWORD        bV5GammaBlue;
	DWORD        bV5Intent;
	DWORD        bV5ProfileData;
	DWORD        bV5ProfileSize;
	DWORD        bV5Reserved;
} BITMAPV5HEADER, *PBITMAPV5HEADER;

typedef struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD	bmiColors[1];
} BITMAPINFO, *PBITMAPINFO, *LPBITMAPINFO;

#ifndef __GNUC__
	#pragma pack(2)
#endif

typedef struct
#ifdef __GNUC__
	 __attribute__ ((packed))
#endif
{
	WORD	mtType;			/* 1 for memory, 2 for disk */
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

typedef enum MetafileType
{
	MetafileTypeInvalid,
	MetafileTypeWmf,
	MetafileTypeWmfPlaceable,
	MetafileTypeEmf,
	MetafileTypeEmfPlusOnly,
	MetafileTypeEmfPlusDual
} MetafileType;

typedef struct {
	MetafileType	Type;
	UINT	Size;
	UINT	Version;
	UINT	EmfPlusFlags;
	REAL	DpiX;
	REAL	DpiY;
	INT	X;
	INT	Y;
	INT	Width;
	INT	Height;
	union {
		METAHEADER	Wmf;
		ENHMETAHEADER3	Emf;
	} Header;
	INT	EmfPlusHeaderSize;
	INT	LogicalDpiX;
	INT	LogicalDpiY;
} MetafileHeader;

#endif
