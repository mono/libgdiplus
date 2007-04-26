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

typedef enum {
	MetafileTypeInvalid,
	MetafileTypeWmf,
	MetafileTypeWmfPlaceable,
	MetafileTypeEmf,
	MetafileTypeEmfPlusOnly,
	MetafileTypeEmfPlusDual
} MetafileType;

typedef enum {
	EmfTypeEmfOnly		= MetafileTypeEmf,
	EmfTypeEmfPlusOnly	= MetafileTypeEmfPlusOnly,
	EmfTypeEmfPlusDual	= MetafileTypeEmfPlusDual
} EmfType;

typedef enum {
	MetafileFrameUnitPixel		= UnitPixel,
	MetafileFrameUnitPoint		= UnitPoint,
	MetafileFrameUnitInch		= UnitInch,
	MetafileFrameUnitDocument	= UnitDocument,
	MetafileFrameUnitMillimeter	= UnitMillimeter,
	MetafileFrameUnitGdi
} MetafileFrameUnit;

typedef enum {
	EmfRecordTypeGdiComment = 70,
	EmfPlusRecordTypeHeader = 16385,
	EmfPlusRecordTypeEndOfFile = 16386,
	EmfPlusRecordTypeClear = 16393,
	EmfPlusRecordTypeFillRects = 16394,
} EmfPlusRecordType;

typedef BOOL (*EnumerateMetafileProc) (EmfPlusRecordType, UINT, UINT, const BYTE*, void*);

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

GpStatus GdipRecordMetafile (HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, MetafileFrameUnit frameUnit, 
	GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus GdipRecordMetafileI (HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect, MetafileFrameUnit frameUnit, 
	GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus GdipRecordMetafileFileName (GDIPCONST WCHAR *fileName, HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus GdipRecordMetafileFileNameI (GDIPCONST WCHAR *fileName, HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus GdipRecordMetafileStream (void /* IStream */ *stream, HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, 
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus GdipRecordMetafileStreamI (void /* IStream */ *stream, HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect,
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);

/* extra public (exported) functions in libgdiplus to replace the IStream (COM-based) ones available on Windows */

GpStatus GdipRecordMetafileFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, HDC referenceHdc, 
	EmfType type, GDIPCONST GpRectF *frameRect, MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, 
	GpMetafile **metafile);
GpStatus GdipRecordMetafileFromDelegateI_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, HDC referenceHdc, 
	EmfType type, GDIPCONST GpRect *frameRect, MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, 
	GpMetafile **metafile);

#endif
