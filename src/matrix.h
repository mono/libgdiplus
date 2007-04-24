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

#ifndef __MATRIX_H__
#define __MATRIX_H__

GpStatus GdipCreateMatrix (GpMatrix **matrix);
GpStatus GdipCreateMatrix2 (float m11, float m12, float m21, float m22, float dx, float dy, GpMatrix **matrix);
GpStatus GdipCreateMatrix3 (GDIPCONST GpRectF *rect, GDIPCONST GpPointF *dstplg, GpMatrix **matrix);
GpStatus GdipCreateMatrix3I (GDIPCONST GpRect *rect, GDIPCONST GpPoint *dstplg, GpMatrix **matrix);
GpStatus GdipCloneMatrix (GpMatrix *matrix, GpMatrix **cloneMatrix);
GpStatus GdipDeleteMatrix (GpMatrix *matrix);
GpStatus GdipSetMatrixElements (GpMatrix *matrix, float m11, float m12, float m21, float m22, float dx, float dy);
GpStatus GdipMultiplyMatrix (GpMatrix *matrix, GpMatrix *matrix2, GpMatrixOrder order);
GpStatus GdipTranslateMatrix (GpMatrix *matrix, float offsetX, float offsetY, GpMatrixOrder order);
GpStatus GdipScaleMatrix (GpMatrix *matrix, float scaleX, float scaleY, GpMatrixOrder order);
GpStatus GdipRotateMatrix (GpMatrix *matrix, float angle, GpMatrixOrder order);
GpStatus GdipShearMatrix (GpMatrix *matrix, float shearX, float shearY, GpMatrixOrder order);
GpStatus GdipInvertMatrix (GpMatrix *matrix);
GpStatus GdipTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count);
GpStatus GdipTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count);
GpStatus GdipVectorTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count);
GpStatus GdipVectorTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count);
GpStatus GdipGetMatrixElements (GpMatrix *matrix, float *matrixOut);
GpStatus GdipIsMatrixInvertible (GpMatrix *matrix, BOOL *result);
GpStatus GdipIsMatrixIdentity (GpMatrix *matrix, BOOL *result);
GpStatus GdipIsMatrixEqual (GpMatrix *matrix, GpMatrix *matrix2, BOOL *result);

#endif
