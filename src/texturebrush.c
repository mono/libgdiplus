/*
 * texturebrush.c
 * 
 * Copyright (C) 2004 Novell, Inc.
 * 
 * Authors:
 *   Ravindra (rkumar@novell.com)
 *
 */
#include "gdip.h"
#include "texturebrush.h"

/*
 * we have a single copy of vtable for
 * all instances of texturebrush.
 */

static BrushClass vtable = { BrushTypeTextureFill, 
			     gdip_texture_setup, 
			     gdip_texture_clone,
			     gdip_texture_destroy };

void 
gdip_texture_init (GpTexture *texture)
{
	gdip_brush_init (&texture->base, &vtable);
	texture->wrapMode = WrapModeTile;
	texture->matrix = cairo_matrix_create ();
}

GpTexture*
gdip_texture_new (void)
{
        GpTexture *result = (GpTexture *) GdipAlloc (sizeof (GpTexture));

        gdip_texture_init (result);

        return result;
}

void
gdip_texture_setup (GpGraphics *graphics, GpBrush *brush)
{
	cairo_t *ct;
	GpBitmap *bmp;
	GpTexture *texture = (GpTexture *) brush;
	GpImage *img = texture->image;

	if (img->type == imageBitmap)
		bmp = (GpBitmap *) img;
	else 
		return;

	ct = graphics->ct;

	switch (texture->wrapMode) {

	case WrapModeTile:
		draw_tile_texture (ct, bmp, texture->matrix);
		break;

	case WrapModeTileFlipX:
		draw_tile_flipX_texture (ct, bmp, texture->matrix);
		break;

	case WrapModeTileFlipY:
		draw_tile_flipY_texture (ct, bmp, texture->matrix);
		break;

	case WrapModeTileFlipXY:
		draw_tile_flipXY_texture (ct, bmp, texture->matrix);
		break;

	case WrapModeClamp:
		draw_clamp_texture (ct, bmp, texture->matrix);
		break;

	default:
		break;
	}
}

void
gdip_texture_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpTexture *result = (GpTexture *) GdipAlloc (sizeof (GpTexture));
	GpTexture *texture = (GpTexture *) brush;

	result->base = texture->base;
	result->wrapMode = texture->wrapMode;
	result->image = texture->image;
	result->matrix = cairo_matrix_create ();
	cairo_matrix_copy (result->matrix, texture->matrix);

	*clonedBrush = (GpBrush *) result;
}

void
gdip_texture_destroy (GpBrush *brush)
{
	GpTexture *texture = (GpTexture *) brush;
	if (texture->matrix != NULL)
		cairo_matrix_destroy (texture->matrix);

	GdipFree (texture);
}

GpStatus
GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture)
{
        *texture = gdip_texture_new ();
        (*texture)->wrapMode = wrapMode;
        (*texture)->image = image;

        return Ok; 
}

GpStatus
GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	matrix = texture->matrix;
	return Ok;
}

GpStatus
GdipSetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	texture->matrix = matrix;
	return Ok;
}

GpStatus
GdipResetTextureTransform (GpTexture *texture)
{
	cairo_status_t status = cairo_matrix_set_identity (texture->matrix);
	return gdip_get_status (status);
}

GpStatus
GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order)
{
	return GdipMultiplyMatrix (texture->matrix, matrix, order);
}

GpStatus
GdipTranslateTextureTransform (GpTexture *texture, float dx, float dy, GpMatrixOrder order)
{
	return GdipTranslateMatrix (texture->matrix, dx, dy, order);
}

GpStatus
GdipScaleTextureTransform (GpTexture *texture, float sx, float sy, GpMatrixOrder order)
{
	return GdipScaleMatrix (texture->matrix, sx, sy, order);
}

GpStatus
GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order)
{
	return GdipRotateMatrix (texture->matrix, angle, order);
}

GpStatus
GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode)
{
	texture->wrapMode = wrapMode;
	return Ok;
}

GpStatus
GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode)
{
	*wrapMode = texture->wrapMode;
	return Ok;
}

GpStatus
GdipGetTextureImage (GpTexture *texture, GpImage **image)
{
	*image = texture->image;
	return Ok;
}

/*
 * MonoTODO: Implement these when ImageAttributes is implemented. We also
 * need Cairo to support image clipping for these.
 *
 */
GpStatus
GdipCreateTexture2 (GpImage *image, GpWrapMode wrapMode, float x, float y, float width, float height, GpTexture **texture)
{
        return NotImplemented;
}

GpStatus
GdipCreateTexture2I (GpImage *image, GpWrapMode wrapMode, int x, int y, int width, int height, GpTexture **texture)
{
        return NotImplemented;
}

GpStatus
GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture)
{
        return NotImplemented;
}

GpStatus
GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture)
{
        return NotImplemented;
}
 
/* texture internal functions */
void
draw_tile_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix)
{
	cairo_surface_t *texture;

	texture = cairo_surface_create_for_image (bitmap->data.Scan0, bitmap->cairo_format,
				bitmap->data.Width, bitmap->data.Height, bitmap->data.Stride);

	cairo_surface_set_matrix (texture, matrix);
	cairo_surface_set_repeat (texture, 1);
	cairo_set_pattern (ct, texture);

	cairo_surface_destroy (texture);
}

void
draw_tile_flipX_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	GpMatrix *matrixCopy = cairo_matrix_create ();
	cairo_matrix_copy (matrixCopy, matrix);

	/* Original image surface */
	original = cairo_surface_create_for_image (bitmap->data.Scan0, bitmap->cairo_format,
				bitmap->data.Width, bitmap->data.Height, bitmap->data.Stride);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, bitmap->cairo_format,
				2 * bitmap->data.Width, bitmap->data.Height);

	cairo_save (ct);

	/* Draw left part of the texture */
	cairo_set_target_surface (ct, texture);
	cairo_surface_set_matrix (original, matrixCopy);
	cairo_show_surface (ct, original, bitmap->data.Width, bitmap->data.Height);

	/* Draw right part of the texture */
	cairo_matrix_translate (matrixCopy, 2 * bitmap->data.Width, 0);
	/* scale in -X direction to flip along X */
	cairo_matrix_scale (matrixCopy, -1.0, 1.0);
	cairo_surface_set_matrix (original, matrixCopy);
	cairo_show_surface (ct, original, bitmap->data.Width, bitmap->data.Height);

	cairo_restore (ct);
	
	cairo_surface_set_repeat (texture, 1);
	cairo_set_pattern (ct, texture);

	cairo_matrix_destroy (matrixCopy);
	cairo_surface_destroy (original);
	cairo_surface_destroy (texture);
}

void
draw_tile_flipY_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	GpMatrix *matrixCopy = cairo_matrix_create ();
	cairo_matrix_copy (matrixCopy, matrix);

	/* Original image surface */
	original = cairo_surface_create_for_image (bitmap->data.Scan0, bitmap->cairo_format,
				bitmap->data.Width, bitmap->data.Height, bitmap->data.Stride);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, bitmap->cairo_format,
				bitmap->data.Width, 2 * bitmap->data.Height);

	cairo_save (ct);

	/* Draw upper part of the texture */
	cairo_set_target_surface (ct, texture);
	cairo_surface_set_matrix (original, matrixCopy);
	cairo_show_surface (ct, original, bitmap->data.Width, bitmap->data.Height);

	/* Draw lower part of the texture */
	cairo_matrix_translate (matrixCopy, 0, 2 * bitmap->data.Height);
	/* scale in -Y direction to flip along Y */
	cairo_matrix_scale (matrixCopy, 1.0, -1.0);
	cairo_surface_set_matrix (original, matrixCopy);
	cairo_show_surface (ct, original, bitmap->data.Width, bitmap->data.Height);

	cairo_restore (ct);
	
	cairo_surface_set_repeat (texture, 1);
	cairo_set_pattern (ct, texture);

	cairo_matrix_destroy (matrixCopy);
	cairo_surface_destroy (original);
	cairo_surface_destroy (texture);
}

void
draw_tile_flipXY_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix)
{
	/* NotImplemented */
}

void
draw_clamp_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix)
{
	cairo_surface_t *texture;

	texture = cairo_surface_create_for_image (bitmap->data.Scan0, bitmap->cairo_format,
				bitmap->data.Width, bitmap->data.Height, bitmap->data.Stride);

	cairo_surface_set_matrix (texture, matrix);
	cairo_set_pattern (ct, texture);

	cairo_surface_destroy (texture);
}
