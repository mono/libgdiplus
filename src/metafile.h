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

/* function prototypes */

GpStatus WINGDIPAPI GdipCreateMetafileFromWmf (HMETAFILE hWmf, BOOL deleteWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, GpMetafile **metafile);

GpStatus WINGDIPAPI GdipCreateMetafileFromEmf (HENHMETAFILE hEmf, BOOL deleteEmf, GpMetafile **metafile);

GpStatus WINGDIPAPI GdipCreateMetafileFromFile (GDIPCONST WCHAR *file, GpMetafile **metafile);

GpStatus WINGDIPAPI GdipCreateMetafileFromWmfFile (GDIPCONST WCHAR *file, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, GpMetafile **metafile);

GpStatus WINGDIPAPI GdipCreateMetafileFromStream (void *stream, GpMetafile **metafile);
GpStatus WINGDIPAPI GdipCreateMetafileFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc,
	GpMetafile **metafile);

GpStatus WINGDIPAPI GdipGetMetafileHeaderFromWmf (HMETAFILE hWmf, GDIPCONST WmfPlaceableFileHeader *wmfPlaceableFileHeader, MetafileHeader *header);

GpStatus WINGDIPAPI GdipGetMetafileHeaderFromEmf (HENHMETAFILE hEmf, MetafileHeader *header);

GpStatus WINGDIPAPI GdipGetMetafileHeaderFromFile (GDIPCONST WCHAR *filename, MetafileHeader *header);

GpStatus WINGDIPAPI GdipGetMetafileHeaderFromStream (void *stream, MetafileHeader *header);
GpStatus WINGDIPAPI GdipGetMetafileHeaderFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc,
	MetafileHeader *header);

GpStatus WINGDIPAPI GdipGetMetafileHeaderFromMetafile (GpMetafile *metafile, MetafileHeader *header);

GpStatus WINGDIPAPI GdipGetHemfFromMetafile (GpMetafile *metafile, HENHMETAFILE *hEmf);

GpStatus WINGDIPAPI GdipGetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT *metafileRasterizationLimitDpi);

GpStatus WINGDIPAPI GdipSetMetafileDownLevelRasterizationLimit (GpMetafile *metafile, UINT metafileRasterizationLimitDpi);

GpStatus WINGDIPAPI GdipPlayMetafileRecord (GDIPCONST GpMetafile *metafile, EmfPlusRecordType recordType, UINT flags, UINT dataSize, GDIPCONST BYTE* data);

GpStatus WINGDIPAPI GdipRecordMetafile (HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect, MetafileFrameUnit frameUnit,
	GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus WINGDIPAPI GdipRecordMetafileI (HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect, MetafileFrameUnit frameUnit,
	GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus WINGDIPAPI GdipRecordMetafileFileName (GDIPCONST WCHAR *fileName, HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect,
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus WINGDIPAPI GdipRecordMetafileFileNameI (GDIPCONST WCHAR *fileName, HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect,
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus WINGDIPAPI GdipRecordMetafileStream (void /* IStream */ *stream, HDC referenceHdc, EmfType type, GDIPCONST GpRectF *frameRect,
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);
GpStatus WINGDIPAPI GdipRecordMetafileStreamI (void /* IStream */ *stream, HDC referenceHdc, EmfType type, GDIPCONST GpRect *frameRect,
	MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description, GpMetafile **metafile);

/* extra public (exported) functions in libgdiplus to replace the IStream (COM-based) ones available on Windows */

GpStatus WINGDIPAPI GdipRecordMetafileFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, HDC referenceHdc,
	EmfType type, GDIPCONST GpRectF *frameRect, MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description,
	GpMetafile **metafile);
GpStatus WINGDIPAPI GdipRecordMetafileFromDelegateI_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, HDC referenceHdc,
	EmfType type, GDIPCONST GpRect *frameRect, MetafileFrameUnit frameUnit, GDIPCONST WCHAR *description,
	GpMetafile **metafile);

#endif
