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

GpStatus GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture);
GpStatus GdipCreateTexture2 (GpImage *image, GpWrapMode wrapMode, float x, float y, float width, float height, GpTexture **texture);
GpStatus GdipCreateTexture2I (GpImage *image, GpWrapMode wrapMode, int x, int y, int width, int height, GpTexture **texture);
GpStatus GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture);
GpStatus GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture);
GpStatus GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix);
GpStatus GdipSetTextureTransform (GpTexture *texture, GDIPCONST GpMatrix *matrix);
GpStatus GdipResetTextureTransform (GpTexture *texture);
GpStatus GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipTranslateTextureTransform (GpTexture *texture, float dx, float dy, GpMatrixOrder order);
GpStatus GdipScaleTextureTransform (GpTexture *texture, float sx, float sy, GpMatrixOrder order);
GpStatus GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order);
GpStatus GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode);
GpStatus GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode);
GpStatus GdipGetTextureImage (GpTexture *texture, GpImage **image);

#endif
