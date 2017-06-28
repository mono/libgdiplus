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

GpStatus WINGDIPAPI GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTexture2 (GpImage *image, GpWrapMode wrapMode, float x, float y, float width, float height, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTexture2I (GpImage *image, GpWrapMode wrapMode, int x, int y, int width, int height, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture);
GpStatus WINGDIPAPI GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture);
GpStatus WINGDIPAPI GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix);
GpStatus WINGDIPAPI GdipSetTextureTransform (GpTexture *texture, GDIPCONST GpMatrix *matrix);
GpStatus WINGDIPAPI GdipResetTextureTransform (GpTexture *texture);
GpStatus WINGDIPAPI GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipTranslateTextureTransform (GpTexture *texture, float dx, float dy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipScaleTextureTransform (GpTexture *texture, float sx, float sy, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order);
GpStatus WINGDIPAPI GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode);
GpStatus WINGDIPAPI GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode);
GpStatus WINGDIPAPI GdipGetTextureImage (GpTexture *texture, GpImage **image);

#endif
