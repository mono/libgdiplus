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
	GpTexture *texture = (GpTexture *) brush;
	texture->image = graphics->image;
	cairo_set_pattern (graphics->ct, texture->image->surface);
	cairo_set_fill_rule (graphics->ct, CAIRO_FILL_RULE_WINDING);
}

void
gdip_texture_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpTexture *result = (GpTexture *) GdipAlloc (sizeof (GpTexture));
	GpTexture *texture = (GpTexture *) brush;

	result->base = texture->base;
	result->wrapMode = texture->wrapMode;
	result->image = texture->image;

	*clonedBrush = (GpBrush *) result;
}

void
gdip_texture_destroy (GpBrush *brush)
{
	GdipFree (brush);
}

GpStatus
GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture)
{
        *texture = gdip_texture_new ();
        (*texture)->wrapMode = wrapMode;
	cairo_surface_set_matrix (image->surface, cairo_matrix_create ());
        (*texture)->image = image;

        return Ok; 
}

GpStatus
GdipCreateTexture2 (GpImage *image, GpWrapMode wrapMode, float x, float y, float width, float height, GpTexture **texture)
{
	return GdipCreateTexture2I (image, wrapMode, (int) x, (int) y, (int) width, (int) height, texture);

}

GpStatus
GdipCreateTexture2I (GpImage *image, GpWrapMode wrapMode, int x, int y, int width, int height, GpTexture **texture)
{
        *texture = gdip_texture_new ();
        (*texture)->wrapMode = wrapMode;
	/*
	 * Followin API is yet to be implemented in cairo
	 * cairo_surface_clip_rectangle (image->surface, x, y, width, height);
	 */
	cairo_surface_set_matrix (image->surface, cairo_matrix_create ());
        (*texture)->image = image;

        return Ok; 
}

GpStatus
GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	cairo_surface_get_matrix (texture->image->surface, matrix);
	return Ok;
}

GpStatus
GdipSetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	cairo_surface_set_matrix (texture->image->surface, matrix);
	return Ok;
}

GpStatus
GdipResetTextureTransform (GpTexture *texture)
{
	GpMatrix *matrix = cairo_matrix_create ();
	cairo_surface_set_matrix (texture->image->surface, matrix);
	cairo_matrix_destroy (matrix);

	return Ok;
}

GpStatus
GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order)
{
	GpMatrix *current_matrix = cairo_matrix_create ();

	cairo_surface_get_matrix (texture->image->surface, current_matrix);

	Status s = GdipMultiplyMatrix (current_matrix, matrix, order);
        if (s == Ok)
		cairo_surface_set_matrix (texture->image->surface, current_matrix);

	cairo_matrix_destroy (current_matrix);
	return s;
}

GpStatus
GdipTranslateTextureTransform (GpTexture *texture, float dx, float dy, GpMatrixOrder order)
{
	GpMatrix *current_matrix = cairo_matrix_create ();

	cairo_surface_get_matrix (texture->image->surface, current_matrix);

	Status s = GdipTranslateMatrix (current_matrix, dx, dy, order);
        if (s == Ok)
		cairo_surface_set_matrix (texture->image->surface, current_matrix);

	cairo_matrix_destroy (current_matrix);
	return s;
}

GpStatus
GdipScaleTextureTransform (GpTexture *texture, float sx, float sy, GpMatrixOrder order)
{
	GpMatrix *current_matrix = cairo_matrix_create ();

	cairo_surface_get_matrix (texture->image->surface, current_matrix);

	Status s = GdipScaleMatrix (current_matrix, sx, sy, order);
        if (s == Ok)
		cairo_surface_set_matrix (texture->image->surface, current_matrix);

	cairo_matrix_destroy (current_matrix);
	return s;
}

GpStatus
GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order)
{
	GpMatrix *current_matrix = cairo_matrix_create ();

	cairo_surface_get_matrix (texture->image->surface, current_matrix);

	Status s = GdipRotateMatrix (current_matrix, angle, order);
        if (s == Ok)
		cairo_surface_set_matrix (texture->image->surface, current_matrix);

	cairo_matrix_destroy (current_matrix);
	return s;
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
 * MonoTODO: Implement these when ImageAttributes is implemented.
 *
 * Texture brush functions
 *
GpStatus
GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture)
{
}

GpStatus
GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture)
{
}
 */
