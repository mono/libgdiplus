/*
 * texturebrush.c
 * 
 * Authors:
 *   Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004 Novell, Inc. http://www.novell.com
 */

#include "gdip.h"
#include "texturebrush.h"

static GpStatus gdip_texture_setup (GpGraphics *graphics, GpBrush *brush);
static GpStatus gdip_texture_clone (GpBrush *brush, GpBrush **clonedBrush);
static GpStatus gdip_texture_destroy (GpBrush *brush);

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
	texture->rectangle = NULL;
}

GpTexture*
gdip_texture_new (void)
{
        GpTexture *result = (GpTexture *) GdipAlloc (sizeof (GpTexture));

	if (result)
	        gdip_texture_init (result);

        return result;
}

/* 
 * functions to create different wrapmodes.
 */
GpStatus
draw_tile_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix, GpRect *rect)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	GpMatrix *tempMatrix;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	tempMatrix = cairo_matrix_create ();

	g_return_val_if_fail (tempMatrix != NULL, OutOfMemory);

	/* Original image surface */
	original = bitmap->image.surface;

	if (original == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return InvalidParameter;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, bitmap->cairo_format,
						rect->Width, rect->Height);

	if (texture == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return OutOfMemory;
	}

	cairo_save (ct);

	/* Draw the texture */
	cairo_set_target_surface (ct, texture);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	cairo_restore (ct);

	cairo_surface_set_repeat (texture, 1);
	cairo_matrix_copy (tempMatrix, matrix);
	cairo_matrix_invert (tempMatrix);
	cairo_surface_set_matrix (texture, tempMatrix);
	gdip_cairo_set_surface_pattern (ct, texture);

	cairo_matrix_destroy (tempMatrix);
	cairo_surface_destroy (texture);

	return Ok;
}

GpStatus
draw_tile_flipX_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix, GpRect *rect)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	GpMatrix *tempMatrix;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	tempMatrix = cairo_matrix_create ();

	g_return_val_if_fail (tempMatrix != NULL, OutOfMemory);

	/* Original image surface */
	original = bitmap->image.surface;

	if (original == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return InvalidParameter;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, bitmap->cairo_format,
						2 * rect->Width, rect->Height);

	if (texture == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return OutOfMemory;
	}

	cairo_save (ct);

	/* Draw left part of the texture */
	cairo_set_target_surface (ct, texture);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	/* Draw right part of the texture */
	cairo_matrix_translate (tempMatrix, 2 * rect->Width, 0);
	/* scale in -X direction to flip along X */
	cairo_matrix_scale (tempMatrix, -1.0, 1.0);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	cairo_restore (ct);

	cairo_surface_set_repeat (texture, 1);
	cairo_matrix_copy (tempMatrix, matrix);
	cairo_matrix_invert (tempMatrix);
	cairo_surface_set_matrix (texture, tempMatrix);
	gdip_cairo_set_surface_pattern (ct, texture);

	cairo_matrix_destroy (tempMatrix);
	cairo_surface_destroy (texture);

	return Ok;
}

GpStatus
draw_tile_flipY_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix, GpRect *rect)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	GpMatrix *tempMatrix;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	tempMatrix = cairo_matrix_create ();

	g_return_val_if_fail (tempMatrix != NULL, OutOfMemory);

	/* Original image surface */
	original = bitmap->image.surface;

	if (original == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return InvalidParameter;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, bitmap->cairo_format,
						rect->Width, 2 * rect->Height);

	if (texture == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return OutOfMemory;
	}

	cairo_save (ct);

	/* Draw upper part of the texture */
	cairo_set_target_surface (ct, texture);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	/* Draw lower part of the texture */
	cairo_matrix_translate (tempMatrix, 0, 2 * rect->Height);
	/* scale in -Y direction to flip along Y */
	cairo_matrix_scale (tempMatrix, 1.0, -1.0);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	cairo_restore (ct);

	cairo_surface_set_repeat (texture, 1);
	cairo_matrix_copy (tempMatrix, matrix);
	cairo_matrix_invert (tempMatrix);
	cairo_surface_set_matrix (texture, tempMatrix);
	gdip_cairo_set_surface_pattern (ct, texture);

	cairo_matrix_destroy (tempMatrix);
	cairo_surface_destroy (texture);

	return Ok;
}

GpStatus
draw_tile_flipXY_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix, GpRect *rect)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	GpMatrix *tempMatrix;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	tempMatrix = cairo_matrix_create ();

	g_return_val_if_fail (tempMatrix != NULL, OutOfMemory);

	/* Original image surface */
	original = bitmap->image.surface;

	if (original == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return InvalidParameter;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, bitmap->cairo_format,
						2 * rect->Width, 2 * rect->Height);

	if (texture == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return OutOfMemory;
	}

	cairo_save (ct);

	/* Draw upper left part of the texture */
	cairo_set_target_surface (ct, texture);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	/* Draw lower left part of the texture */
	cairo_matrix_translate (tempMatrix, 0, 2 * rect->Height);
	/* scale in -Y direction to flip along Y */
	cairo_matrix_scale (tempMatrix, 1.0, -1.0);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	/* Draw upper right part of the texture */
	cairo_matrix_translate (tempMatrix, 2 * rect->Width, 0);
	/* scale in -X direction to flip along X */
	cairo_matrix_scale (tempMatrix, -1.0, 1.0);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	/* Draw lower right part of the texture */
	cairo_matrix_translate (tempMatrix, 0, 2 * rect->Height);
	/* scale in -Y direction to flip along Y */
	cairo_matrix_scale (tempMatrix, 1.0, -1.0);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	cairo_restore (ct);

	cairo_surface_set_repeat (texture, 1);
	cairo_matrix_copy (tempMatrix, matrix);
	cairo_matrix_invert (tempMatrix);
	cairo_surface_set_matrix (texture, tempMatrix);
	gdip_cairo_set_surface_pattern (ct, texture);

	cairo_matrix_destroy (tempMatrix);
	cairo_surface_destroy (texture);

	return Ok;
}

GpStatus
draw_clamp_texture (cairo_t *ct, GpBitmap *bitmap, GpMatrix *matrix, GpRect *rect)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	GpMatrix *tempMatrix;

	g_return_val_if_fail (matrix != NULL, InvalidParameter);
	g_return_val_if_fail (rect != NULL, InvalidParameter);

	tempMatrix = cairo_matrix_create ();

	g_return_val_if_fail (tempMatrix != NULL, OutOfMemory);

	/* Original image surface */
	original = bitmap->image.surface;

	if (original == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return InvalidParameter;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, bitmap->cairo_format,
						rect->Width, rect->Height);

	if (texture == NULL) {
		cairo_matrix_destroy (tempMatrix);
		return OutOfMemory;
	}

	cairo_save (ct);

	/* Draw the texture */
	cairo_set_target_surface (ct, texture);
	cairo_surface_set_matrix (original, tempMatrix);
	cairo_show_surface (ct, original, rect->Width, rect->Height);

	cairo_restore (ct);

	cairo_matrix_copy (tempMatrix, matrix);
	cairo_matrix_invert (tempMatrix);
	cairo_surface_set_matrix (texture, tempMatrix);
	gdip_cairo_set_surface_pattern (ct, texture);

	cairo_matrix_destroy (tempMatrix);
	cairo_surface_destroy (texture);

	return Ok;
}

GpStatus
gdip_texture_setup (GpGraphics *graphics, GpBrush *brush)
{
	cairo_t *ct;
	GpBitmap *bmp;
	GpTexture *texture; 
	GpImage *img; 

	g_return_val_if_fail (graphics != NULL, InvalidParameter);
	g_return_val_if_fail (brush != NULL, InvalidParameter);

	texture = (GpTexture *) brush;
	img = texture->image;

	g_return_val_if_fail (img != NULL, InvalidParameter);

	if (img->type == imageBitmap)
		bmp = (GpBitmap *) img;
	else 
		return NotImplemented;

	ct = graphics->ct;

	g_return_val_if_fail (ct != NULL, InvalidParameter);

	switch (texture->wrapMode) {

	case WrapModeTile:
		return draw_tile_texture (ct, bmp, texture->matrix, texture->rectangle);

	case WrapModeTileFlipX:
		return draw_tile_flipX_texture (ct, bmp, texture->matrix, texture->rectangle);

	case WrapModeTileFlipY:
		return draw_tile_flipY_texture (ct, bmp, texture->matrix, texture->rectangle);

	case WrapModeTileFlipXY:
		return draw_tile_flipXY_texture (ct, bmp, texture->matrix, texture->rectangle);

	case WrapModeClamp:
		return draw_clamp_texture (ct, bmp, texture->matrix, texture->rectangle);

	default:
		return InvalidParameter;
	}
}

GpStatus
gdip_texture_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpTexture *result;
	GpTexture *texture;

	g_return_val_if_fail (brush != NULL, InvalidParameter);

	result = (GpTexture *) GdipAlloc (sizeof (GpTexture));

	g_return_val_if_fail (result != NULL, OutOfMemory);

	texture = (GpTexture *) brush;
	result->base = texture->base;
	result->wrapMode = texture->wrapMode;
	result->image = texture->image;
	result->matrix = cairo_matrix_create ();

	if (result->matrix == NULL) {
		GdipFree (result);
		return OutOfMemory;
	}

	cairo_matrix_copy (result->matrix, texture->matrix);
	result->rectangle = (GpRect *) malloc (sizeof (GpRect));

	if (result->rectangle == NULL) {
		cairo_matrix_destroy (result->matrix);
		GdipFree (result);
		return OutOfMemory;
	}

	memcpy (result->rectangle, texture->rectangle, sizeof (GpRect));

	*clonedBrush = (GpBrush *) result;

	return Ok;
}

GpStatus
gdip_texture_destroy (GpBrush *brush)
{
	GpTexture *texture;

	g_return_val_if_fail (brush != NULL, InvalidParameter);

	texture = (GpTexture *) brush;

	if (texture->matrix != NULL)
		cairo_matrix_destroy (texture->matrix);

	if (texture->rectangle != NULL)
		GdipFree (texture->rectangle);

	GdipFree (texture);

	return Ok;
}

GpStatus
GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture)
{
	GpBitmap *bitmap;
	cairo_t *ct;
	cairo_surface_t *imageSurface;

	g_return_val_if_fail (image != NULL, InvalidParameter);

	if (image->type == imageBitmap)
		bitmap = (GpBitmap *) image;
	else
		return NotImplemented;

	imageSurface = cairo_surface_create_for_image (bitmap->data.Scan0,
					bitmap->cairo_format, bitmap->data.Width,
					bitmap->data.Height, bitmap->data.Stride);

	g_return_val_if_fail (imageSurface != NULL, OutOfMemory);

	/* texture surface to be used by brush */
	image->surface = imageSurface;

	*texture = gdip_texture_new ();

	if (*texture == NULL) {
		cairo_surface_destroy (imageSurface);
		return OutOfMemory;
	}

	(*texture)->wrapMode = wrapMode;
	(*texture)->image = image;
	(*texture)->rectangle = (GpRect *) malloc (sizeof (GpRect));

	if ( (*texture)->rectangle == NULL) {
		cairo_surface_destroy (imageSurface);
		GdipFree (*texture);
		return OutOfMemory;
	}

	(*texture)->rectangle->X = 0;
	(*texture)->rectangle->Y = 0;
	(*texture)->rectangle->Width = bitmap->data.Width;
	(*texture)->rectangle->Height = bitmap->data.Height;

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
	int bmpWidth;
	int bmpHeight;
	GpBitmap *bitmap;
	cairo_t *ct;
	cairo_surface_t *original, *new;

	g_return_val_if_fail (image != NULL, InvalidParameter);

	if (image->type == imageBitmap)
		bitmap = (GpBitmap *) image;
	else
		return NotImplemented;

	bmpWidth = bitmap->data.Width;
	bmpHeight = bitmap->data.Height;

	/* MS behaves this way */
	if (x < 0 || y < 0 || width < 0 || height < 0 
		  || (bmpWidth < (x + width)) || (bmpHeight < (y + height)))
	     return OutOfMemory;

	original = cairo_surface_create_for_image (bitmap->data.Scan0, 
			bitmap->cairo_format, x + width, y + height, bitmap->data.Stride);

	g_return_val_if_fail (original != NULL, OutOfMemory);

	/* texture surface to be used by brush */
	new = cairo_surface_create_similar (original, bitmap->cairo_format, width, height);

	if (new == NULL) {
		cairo_surface_destroy (original);
		return OutOfMemory;
	}

	/* clip the rectangle from original image surface and create new surface */
	ct = cairo_create ();
	cairo_set_target_surface (ct, new);
	cairo_translate (ct, -x, -y);
	cairo_show_surface (ct, original, x + width, y + height);
	cairo_destroy (ct);
	cairo_surface_destroy (original);

	image->surface = new;

	*texture = gdip_texture_new ();

	if (*texture == NULL) {
		cairo_surface_destroy (new);
		return OutOfMemory;
	}

	(*texture)->wrapMode = wrapMode;
	(*texture)->image = image;
	(*texture)->rectangle = (GpRect *) malloc (sizeof (GpRect));

	if ( (*texture)->rectangle == NULL) {
		cairo_surface_destroy (new);
		GdipFree (*texture);
		return OutOfMemory;
	}

	(*texture)->rectangle->X = x;
	(*texture)->rectangle->Y = y;
	(*texture)->rectangle->Width = width;
	(*texture)->rectangle->Height = height;

	return Ok;
}

GpStatus
GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture)
{
	/* MonoTODO: Make use of ImageAttributes parameter when
	 * ImageAttributes is implemented
	 */
	return GdipCreateTexture2 (image, WrapModeTile, x, y, width, height, texture);
}

GpStatus
GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture)
{
	/* MonoTODO: Make use of ImageAttributes parameter when
	 * ImageAttributes is implemented
	 */
	return GdipCreateTexture2I (image, WrapModeTile, x, y, width, height, texture);
}

GpStatus
GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	*matrix = *(texture->matrix);
	return Ok;
}

GpStatus
GdipSetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	*(texture->matrix) = *matrix;
	return Ok;
}

GpStatus
GdipResetTextureTransform (GpTexture *texture)
{
	cairo_status_t status;
	g_return_val_if_fail (texture != NULL, InvalidParameter);

	status = cairo_matrix_set_identity (texture->matrix);
	return gdip_get_status (status);
}

GpStatus
GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);
	g_return_val_if_fail (matrix != NULL, InvalidParameter);

	/* FIXME: How to take care of rotation here ? */
	return GdipMultiplyMatrix (texture->matrix, matrix, order);
}

GpStatus
GdipTranslateTextureTransform (GpTexture *texture, float dx, float dy, GpMatrixOrder order)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);

	/* FIXME: Creates empty space as a side effect of translation.
	 * Might lead to total blank space depending on the amount of
	 * translation. Cairo bug? We need to see.
	 */
	return GdipTranslateMatrix (texture->matrix, dx, dy, order);
}

GpStatus
GdipScaleTextureTransform (GpTexture *texture, float sx, float sy, GpMatrixOrder order)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);

	/* FIXME: Surface dimensions do not seem to be scaling. Do we 
	 * need to scale the surface dimensions ourselves?
	 * It might be a Cairo bug. I need to come back here once I'm 
	 * sure that Cairo is doing it right.
	 */
	return GdipScaleMatrix (texture->matrix, sx, sy, order);
}

GpStatus
GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);

	/* Cairo uses origin (0,0) as the axis of rotation. However, we need 
	 * to do absolute rotation. Following approach for shifting the axis
	 * of rotation was suggested by Carl.
	 */

	GpPointF axis;

	/* FIXME: Find a decent way to handle this. If wrapmode is
	 * set after rotation, we are going to be wrong.
	 */
	switch (texture->wrapMode) {

	case WrapModeTile:
	case WrapModeClamp:
		axis.X = texture->rectangle->Width/2;
		axis.Y = texture->rectangle->Height/2;
		break;

	case WrapModeTileFlipX:
		axis.X = texture->rectangle->Width;
		axis.Y = texture->rectangle->Height/2;
		break;

	case WrapModeTileFlipY:
		axis.X = texture->rectangle->Width/2;
		axis.Y = texture->rectangle->Height;
		break;

	case WrapModeTileFlipXY:
		axis.X = texture->rectangle->Width;
		axis.Y = texture->rectangle->Height;
		break;

	default:
		break;
	}

	GdipTranslateMatrix (texture->matrix, -axis.X, -axis.Y, order);
	GdipRotateMatrix (texture->matrix, angle, order);
	return GdipTranslateMatrix (texture->matrix, axis.X, axis.Y, order);
}

GpStatus
GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);

	texture->wrapMode = wrapMode;
	return Ok;
}

GpStatus
GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);

	*wrapMode = texture->wrapMode;
	return Ok;
}

GpStatus
GdipGetTextureImage (GpTexture *texture, GpImage **image)
{
	g_return_val_if_fail (texture != NULL, InvalidParameter);
	g_return_val_if_fail (image != NULL, InvalidParameter);

	*image = texture->image;
	return Ok;
}
