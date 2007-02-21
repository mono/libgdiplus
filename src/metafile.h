/*
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
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
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */

#ifndef __METAFILE_H__
#define __METAFILE_H__

#include <stdio.h>
#include "gdip.h"
#include "gdipImage.h"
#include "dstream.h"
#include "brush.h"
#include "solidbrush.h"

/*
 *	http://wvware.sourceforge.net/caolan/ora-wmf.html
 */

/* definitions */

#define ALDUS_PLACEABLE_METAFILE_KEY	0x9AC6CDD7
#define WMF_TYPE_AND_HEADERSIZE_KEY	0x00090001
#define EMF_EMR_HEADER_KEY		0x1

/* this has to do with 25.4mm in an inch (but why is it multiplied by 100 ?) */
#define METAFILE_DIMENSION_FACTOR	2540

/* match System.Drawing.Imaging.MetafileType */
#define METAFILETYPE_INVALID		0
#define METAFILETYPE_WMF		1
#define METAFILETYPE_WMFPLACEABLE	2
#define METAFILETYPE_EMF		3
#define METAFILETYPE_EMFPLUSONLY	4
#define METAFILETYPE_EMFPLUSDUAL	5

/* object types */
#define METAOBJECT_TYPE_EMPTY	0
#define METAOBJECT_TYPE_PEN	1
#define METAOBJECT_TYPE_BRUSH	2

/* SetBkMode */
#define TRANSPARENT		1
#define OPAQUE			2

/* CreatePenIndirect */
#define PS_NULL			0x00000005
#define PS_STYLE_MASK		0x0000000F
#define PS_ENDCAP_ROUND		0x00000000
#define PS_ENDCAP_SQUARE	0x00000100
#define PS_ENDCAP_FLAT		0x00000200
#define PS_ENDCAP_MASK		0x00000F00
#define PS_JOIN_MASK		0x0000F000


#pragma pack(2)

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
	WORD	mtType;			/* 1 for disk, 0 for memory */
	WORD	mtHeaderSize;
	WORD	mtVersion;
	DWORD	mtSize;
	WORD	mtNoObjects;
	DWORD	mtMaxRecord;
	WORD	mtNoParameters;
} METAHEADER;

typedef struct {
	DWORD	iType;
	DWORD	nSize;
	RECTL	rclBounds;
	RECTL	rclFrame;
	DWORD	dSignature;
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
	SHORT	Left;
	SHORT	Top;
	SHORT	Right;
	SHORT	Bottom;
} PWMFRect16;

typedef struct {
	DWORD		Key;
	SHORT		Hmf;
	PWMFRect16	BoundingBox;
	SHORT		Inch;
	DWORD		Reserved;
	SHORT		Checksum;
} WmfPlaceableFileHeader;

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

typedef struct {
	void *ptr;
	int type;
} MetaObject;

typedef struct {
	GpImage base;
	MetafileHeader metafile_header;
	BOOL delete;
	BYTE *data;
	int length;
} GpMetafile;

typedef struct {
	GpMetafile *metafile;
	int x, y, width, height;
	MetaObject *objects;
	MetaObject created;
	GpGraphics *graphics;
	GpPen *selected_pen;
	GpBrush *selected_brush;
	BYTE *scan0;
} MetafilePlayContext;

typedef struct {
	int num;
	GpPointF *points;
} PointFList;

typedef gpointer HMETAFILE;
typedef gpointer HENHMETAFILE;

typedef enum {
	WmfRecordType = 0
} EmfPlusRecordType;

#pragma pack()

/* function prototypes */

GpStatus GdipCreateMetafileFromWmf (HMETAFILE hWmf, BOOL deleteWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, GpMetafile **metafile);

GpStatus GdipCreateMetafileFromEmf (HENHMETAFILE hEmf, BOOL deleteEmf, GpMetafile **metafile);

GpStatus GdipCreateMetafileFromFile (GDIPCONST WCHAR *file, GpMetafile **metafile);

GpStatus GdipCreateMetafileFromWmfFile (GDIPCONST WCHAR *file, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, GpMetafile **metafile);

GpStatus GdipCreateMetafileFromStream (void *stream, GpMetafile **metafile);
GpStatus GdipCreateMetafileFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	GpMetafile **metafile);

GpStatus GdipGetMetafileHeaderFromWmf (HMETAFILE hWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, MetafileHeader *header);

GpStatus GdipGetMetafileHeaderFromEmf (HENHMETAFILE hEmf, MetafileHeader *header);

GpStatus GdipGetMetafileHeaderFromFile (GDIPCONST WCHAR *filename, MetafileHeader *header);

GpStatus GdipGetMetafileHeaderFromStream (void *stream, MetafileHeader *header);
GpStatus GdipGetMetafileHeaderFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, 
	MetafileHeader *header);

GpStatus GdipGetMetafileHeaderFromMetafile (GpMetafile *metafile, MetafileHeader *header);

GpStatus GdipGetHemfFromMetafile (GpMetafile *metafile, HENHMETAFILE *hEmf);

GpStatus GdipGetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT *metafileRasterizationLimitDpi);

GpStatus GdipSetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT metafileRasterizationLimitDpi);

GpStatus GdipPlayMetafileRecord (GDIPCONST GpMetafile *metafile, EmfPlusRecordType recordType, UINT flags, UINT dataSize, GDIPCONST BYTE* data);

/* internal functions */

#define gdip_get_metaheader(image)	(&((GpMetafile*)image)->metafile_header)

GpStatus gdip_get_metafile_from (void *pointer, GpMetafile **metafile, bool useFile);
GpStatus gdip_metafile_clone (GpMetafile *metafile, GpMetafile **clonedmetafile);
GpStatus gdip_metafile_dispose (GpMetafile *metafile);

GpStatus gdip_metafile_play_emf (MetafilePlayContext *context);
GpStatus gdip_metafile_play_wmf (MetafilePlayContext *context);

MetafilePlayContext* gdip_metafile_play_setup (GpMetafile *metafile, GpGraphics *graphics, int x, int y, int width, int height);
GpStatus gdip_metafile_play (MetafilePlayContext *context);
GpStatus gdip_metafile_play_cleanup (MetafilePlayContext *context);


#endif
