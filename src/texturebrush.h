/*
 * texturebrush.h
 *
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004 Novell, Inc.
 */

#ifndef _TEXTURE_H
#define _TEXTURE_H

#include "brush.h"

typedef enum {
	WrapModeTile = 0,
	WrapModeTileFlipX = 1,
	WrapModeTileFlipY = 2,
	WrapModeTileFlipXY = 3,
	WrapModeClamp = 4
} GpWrapMode, WrapMode;

/* Structures */

typedef struct _Texture GpTexture;

typedef struct _Texture {
	GpBrush base;
	GpImage *image;
	GpWrapMode wrapMode;
} Texture;

/* Texture brush */
GpTexture *gdip_texture_new (void);
void gdip_texture_init (GpTexture *brush);
void gdip_texture_setup (GpGraphics *graphics, GpBrush *brush);
void gdip_texture_clone (GpBrush *brush, GpBrush **clonedBrush);
void gdip_texture_destroy (GpBrush *brush);

GpStatus GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture);
GpStatus GdipCreateTexture2 (GpImage *image, GpWrapMode wrapMode, float x, float y, float width, float height, GpTexture **texture);
GpStatus GdipCreateTexture2I (GpImage *image, GpWrapMode wrapMode, int x, int y, int width, int height, GpTexture **texture);
// Implement these when implementing ImageAttributes.
// GpStatus GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture);
// GpStatus GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture);
GpStatus GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix);
GpStatus GdipSetTextureTransform (GpTexture *texture, GpMatrix *matrix);
GpStatus GdipResetTextureTransform (GpTexture *texture);
GpStatus GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipTranslateTextureTransform (GpTexture *texture, float dx, float dy, GpMatrixOrder order);
GpStatus GdipScaleTextureTransform (GpTexture *texture, float sx, float sy, GpMatrixOrder order);
GpStatus GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order);
GpStatus GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode);
GpStatus GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode);
GpStatus GdipGetTextureImage (GpTexture *texture, GpImage **image);

#endif /* _TEXTURE_H */
