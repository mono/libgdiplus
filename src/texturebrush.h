/*
 * texturebrush.h
 *
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004,2006-2007 Novell, Inc. http://www.novell.com
 */

/*
 * NOTE: This header file contains ONLY public definitions and MUST match the GDI+ definitions.
 */

#ifndef __TEXTUREBRUSH_H__
#define __TEXTUREBRUSH_H__

GpStatus WINGDIPAPI GdipCreateTexture (GpImage *image, GpWrapMode wrapmode, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTexture2 (GpImage *image, GpWrapMode wrapmode, REAL x, REAL y, REAL width, REAL height, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTexture2I (GpImage *image, GpWrapMode wrapmode, INT x, INT y, INT width, INT height, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, REAL x, REAL y, REAL width, REAL height, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, INT x, INT y, INT width, INT height, GpTexture **texture);
GpStatus WINGDIPAPI GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipSetTextureTransform (GpTexture *texture, GDIPCONST GpMatrix *matrix);
GpStatus WINGDIPAPI GdipResetTextureTransform (GpTexture *texture);
GpStatus WINGDIPAPI GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipTranslateTextureTransform (GpTexture *texture, REAL dx, REAL dy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipScaleTextureTransform (GpTexture *texture, REAL sx, REAL sy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipRotateTextureTransform (GpTexture *texture, REAL angle, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapmode);
GpStatus WINGDIPAPI GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapmode);
GpStatus WINGDIPAPI GdipGetTextureImage (GpTexture *texture, GpImage **image);

#endif
