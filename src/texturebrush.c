/*
 * texturebrush.c
 *
 * Copyright (C) 2003,2006-2007 Novell, Inc. http://www.novell.com
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
 *   Ravindra (rkumar@novell.com)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 *
 * TODO
 * 	reduce duplication between draw_tile*_texture functions
 */

#include "texturebrush-private.h"
#include "general-private.h"
#include "graphics-private.h"
#include "bitmap-private.h"
#include "matrix-private.h"
#include "metafile-private.h"

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

static cairo_content_t
from_cairoformat_to_content (cairo_format_t format)
{
	switch (format) {
	case CAIRO_FORMAT_RGB24:
		return CAIRO_CONTENT_COLOR;
	case CAIRO_FORMAT_A8:
		return CAIRO_CONTENT_ALPHA;
	case CAIRO_FORMAT_ARGB32:
	default:
		return CAIRO_CONTENT_COLOR_ALPHA;
    }
}

static void 
gdip_texture_init (GpTexture *texture)
{
	gdip_brush_init (&texture->base, &vtable);
	texture->wrapMode = WrapModeTile;
	texture->rectangle.X = 0;
	texture->rectangle.Y = 0;
	texture->rectangle.Width = 0;
	texture->rectangle.Height = 0;
	texture->pattern = NULL;
	cairo_matrix_init_identity (&texture->matrix);
}

static GpTexture*
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
static GpStatus
draw_tile_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpStatus	status;
	GpRect		*rect = &brush->rectangle;
	cairo_t		*ct2;

	if (!rect)
		return InvalidParameter;
	if (gdip_bitmap_ensure_surface (bitmap) == NULL)
		return OutOfMemory;

	/* Use the bitmap->surface as a pattern */
	pat = cairo_pattern_create_for_surface (bitmap->surface);
	status = gdip_get_pattern_status (pat);
	if (status != Ok) {
		return status;
	}

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (
		bitmap->surface, from_cairoformat_to_content (bitmap->cairo_format),
		rect->Width, rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		return status;
	}

	/* Draw the texture */
	ct2 = cairo_create (texture);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);
	cairo_destroy (ct2);

	brush->pattern = cairo_pattern_create_for_surface (texture);
	status = gdip_get_pattern_status (brush->pattern);
	if (status == Ok)
		cairo_pattern_set_extend (brush->pattern, CAIRO_EXTEND_REPEAT);

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	return gdip_get_status (cairo_status (ct));
}

static GpStatus
draw_tile_flipX_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;
	cairo_matrix_t	tempMatrix;

	if (!rect)
		return InvalidParameter;
	if (gdip_bitmap_ensure_surface (bitmap) == NULL)
		return OutOfMemory;

	/* Use the bitmap->surface as a pattern */
	pat = cairo_pattern_create_for_surface (bitmap->surface);
	status = gdip_get_pattern_status (pat);
	if (status != Ok)
		return status;

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (
		bitmap->surface, from_cairoformat_to_content (bitmap->cairo_format),
		2 * rect->Width, rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		return status;
	}

	/* Draw left part of the texture */
	ct2 = cairo_create (texture);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);

	/* Not sure if this is a bug, but using rect->Width - 1 avoids the seam. */
	cairo_matrix_init_identity (&tempMatrix);
	cairo_matrix_translate (&tempMatrix, rect->Width - 1, 0);

	/* scale in -X direction to flip along X */
	cairo_matrix_scale (&tempMatrix, -1.0, 1.0);
	cairo_pattern_set_matrix (pat, &tempMatrix);

	/* Draw right part of the texture */
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, rect->Width, 0, rect->Width, rect->Height);

	cairo_fill (ct2);
	cairo_destroy (ct2);

	brush->pattern = cairo_pattern_create_for_surface (texture);
	status = gdip_get_pattern_status (brush->pattern);
	if (status == Ok)
		cairo_pattern_set_extend (brush->pattern, CAIRO_EXTEND_REPEAT);

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	return gdip_get_status (cairo_status (ct));
}

static GpStatus
draw_tile_flipY_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpMatrix	tempMatrix;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;

	if (!rect)
		return InvalidParameter;
	if (gdip_bitmap_ensure_surface (bitmap) == NULL)
		return OutOfMemory;

	/* Use the bitmap->surface as a pattern */
	pat = cairo_pattern_create_for_surface (bitmap->surface);
	status = gdip_get_pattern_status (pat);
	if (status != Ok)
		return status;

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (
		bitmap->surface, from_cairoformat_to_content (bitmap->cairo_format),
		rect->Width, 2 * rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		return status;
	}

	/* Draw upper part of the texture */
	ct2 = cairo_create (texture);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);
	
	/* Not sure if this is a bug, but using rect->Height - 1 avoids the seam. */
	cairo_matrix_init_identity (&tempMatrix);
	cairo_matrix_translate (&tempMatrix, 0, rect->Height - 1);

	/* scale in -Y direction to flip along Y */
	cairo_matrix_scale (&tempMatrix, 1.0, -1.0);
	cairo_pattern_set_matrix (pat, &tempMatrix);

	/* Draw lower part of the texture */
	cairo_translate (ct2, 0, rect->Height);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);
	cairo_destroy(ct2);

	brush->pattern = cairo_pattern_create_for_surface (texture);
	status = gdip_get_pattern_status (brush->pattern);
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		cairo_surface_destroy (texture);
		return status;
	}
	cairo_pattern_set_extend (brush->pattern, CAIRO_EXTEND_REPEAT);	

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	return gdip_get_status (cairo_status (ct));
}

static GpStatus
draw_tile_flipXY_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpMatrix	tempMatrix;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;

	if (!rect)
		return InvalidParameter;
	if (gdip_bitmap_ensure_surface (bitmap) == NULL)
		return OutOfMemory;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (bitmap->surface);
	status = gdip_get_pattern_status (pat);
	if (status != Ok)
		return status;

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (
		bitmap->surface, from_cairoformat_to_content (bitmap->cairo_format),
		2 * rect->Width, 2 * rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		return status;
	}

	/* Draw upper left part of the texture */
	ct2 = cairo_create (texture);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);

	/* Not sure if this is a bug, but using rect->Height - 1 avoids the seam. */
	cairo_matrix_init_identity (&tempMatrix);
	cairo_matrix_translate (&tempMatrix, 0, rect->Height - 1);
	/* scale in -Y direction to flip along Y */
	cairo_matrix_scale (&tempMatrix, 1.0, -1.0);
	cairo_pattern_set_matrix (pat, &tempMatrix);

	/* Draw lower left part of the texture */
	cairo_translate (ct2, 0, rect->Height);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);

	/* Reset the pattern matrix and do fresh transformation */
	cairo_matrix_init_identity (&tempMatrix);

	/* Not sure if this is a bug, but using rect->Width - 1 avoids the seam. */
	cairo_matrix_translate (&tempMatrix, rect->Width - 1, 0);

	/* scale in -X direction to flip along X */
	cairo_matrix_scale (&tempMatrix, -1.0, 1.0);
	cairo_pattern_set_matrix (pat, &tempMatrix);

	/* Draw upper right part of the texture */
	cairo_translate (ct2, rect->Width, -rect->Height);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);

	/* Not sure if this is a bug, but using rect->Height - 1 avoids the seam. */
	cairo_matrix_translate (&tempMatrix, 0, rect->Height - 1);

	/* scale in -Y direction to flip along Y */
	cairo_matrix_scale (&tempMatrix, 1.0, -1.0);
	cairo_pattern_set_matrix (pat, &tempMatrix);

	/* Draw lower right part of the texture */
	cairo_translate (ct2, 0, rect->Height);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);
	cairo_destroy(ct2);

	brush->pattern = cairo_pattern_create_for_surface (texture);
	status = gdip_get_pattern_status(brush->pattern);
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		cairo_surface_destroy (texture);
		return status;
	}
	cairo_pattern_set_extend (brush->pattern, CAIRO_EXTEND_REPEAT);

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	return gdip_get_status (cairo_status (ct));
}

static GpStatus
draw_clamp_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;

	/* Original image surface */
	if (gdip_bitmap_ensure_surface (bitmap) == NULL)
		return OutOfMemory;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (bitmap->surface);
	status = gdip_get_pattern_status (pat);
	if (status != Ok) {
		return status;
	}

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (
		bitmap->surface, from_cairoformat_to_content (bitmap->cairo_format),
		rect->Width, rect->Height);
	if (!texture) {
		cairo_pattern_destroy (pat);
		return OutOfMemory;
	}

	/* Draw the texture */
	ct2 = cairo_create (texture);
	cairo_identity_matrix (ct2);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);
	cairo_fill (ct2);
	cairo_destroy(ct2);

	brush->pattern = cairo_pattern_create_for_surface (texture);
	status = gdip_get_pattern_status (brush->pattern);
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		cairo_surface_destroy (texture);
		return status;
	}

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	return gdip_get_status (cairo_status (ct));
}

GpStatus
gdip_texture_setup (GpGraphics *graphics, GpBrush *brush)
{
	cairo_t		*ct;
	cairo_pattern_t	*pattern;
	GpTexture	*texture;
	GpImage		*img;
	GpStatus	status = Ok;
	BOOL		dispose_bitmap;

	if (!graphics || !brush || !graphics->ct)
		return InvalidParameter;

	texture = (GpTexture *) brush;
	img = texture->image;
	if (!img)
		return InvalidParameter;

	if (img->type != ImageTypeBitmap)
		return NotImplemented;

	if (gdip_is_an_indexed_pixelformat (img->active_bitmap->pixel_format)) {
		/* Unable to create a surface for the bitmap; it is an indexed image.
		 * Instead, it will first be converted to 32-bit RGB. */
		img = gdip_convert_indexed_to_rgb (img);
		if (!img)
			return OutOfMemory;
		if (gdip_bitmap_ensure_surface (img) == NULL)
			return OutOfMemory;
		dispose_bitmap = TRUE;
	} else {
		dispose_bitmap = FALSE;
	}

	ct = graphics->ct;

	/* We create the new pattern for brush, if the brush is changed
	 * or if pattern has not been created yet. */
	if (texture->base.changed || !texture->pattern) {
		if (texture->pattern)
			cairo_pattern_destroy (texture->pattern);

		switch (texture->wrapMode) {
			case WrapModeTile:
				status = draw_tile_texture (ct, img, texture);
				break;

			case WrapModeTileFlipX:
				status = draw_tile_flipX_texture (ct, img, texture);
				break;

			case WrapModeTileFlipY:
				status = draw_tile_flipY_texture (ct, img, texture);
				break;

			case WrapModeTileFlipXY:
				status = draw_tile_flipXY_texture (ct, img, texture);
				break;

			case WrapModeClamp:
				status = draw_clamp_texture (ct, img, texture);
				break;

			default:
				status = InvalidParameter;
		}
	}

	if (dispose_bitmap) {
		GdipDisposeImage((GpImage *)img);
	}

	if ((status != Ok) || (gdip_get_pattern_status(texture->pattern) != Ok)) {
		return GenericError;
	}

	pattern = texture->pattern;
	if (pattern != NULL) {
		/* got something to apply ? */
		if (!gdip_is_matrix_empty (&texture->matrix)) {
			cairo_matrix_t product;

			gdip_cairo_matrix_copy (&product, &texture->matrix);
			cairo_matrix_invert (&product);
			cairo_pattern_set_matrix (pattern, &product);
		}
		cairo_set_source (ct, pattern);
	}

	return gdip_get_status (cairo_status (ct));
}

GpStatus
gdip_texture_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpTexture *result;
	GpTexture *texture;
	GpStatus status;

	if (!brush || !clonedBrush)
		return InvalidParameter;

	result = gdip_texture_new ();
	if (!result)
		return OutOfMemory;

	texture = (GpTexture *) brush;
	result->base = texture->base;
	result->wrapMode = texture->wrapMode;

	/* Let the clone create its own pattern. */
	result->pattern = NULL;
	result->base.changed = TRUE;

	gdip_cairo_matrix_copy (&result->matrix, &texture->matrix);
	result->rectangle.X = texture->rectangle.X;
	result->rectangle.Y = texture->rectangle.Y;
	result->rectangle.Width = texture->rectangle.Width;
	result->rectangle.Height = texture->rectangle.Height;

	result->image = NULL;
	status = GdipCloneImage (texture->image, &result->image);
	if (status != Ok) {
		GdipDeleteBrush ((GpBrush *) result);
		*clonedBrush = NULL;
		return status;
	} else {
		cairo_surface_reference (result->image->surface);
	}

	*clonedBrush = (GpBrush *) result;
	return status;
}

GpStatus
gdip_texture_destroy (GpBrush *brush)
{
	/* a. the NULL check for brush is done by the caller, GdipDeleteBrush */
	/* b. brush itself is freed by the caller */

	GpTexture *texture = (GpTexture *) brush;

	if (texture->pattern) {
		cairo_pattern_destroy (texture->pattern);
		texture->pattern = NULL;
	}

	if (texture->image) {
		GdipDisposeImage (texture->image);
		texture->image = NULL;
	}

	return Ok;
}

static GpStatus
gdip_texture_create_from_cloned_image (GpImage *image, GpWrapMode wrapMode, GpTexture **texture)
{
	GpTexture *result;

	result = gdip_texture_new ();
	if (!result)
		return OutOfMemory;

	result->image = image;
	gdip_bitmap_ensure_surface (image);
	result->wrapMode = wrapMode;
	result->rectangle.X = 0;
	result->rectangle.Y = 0;
	result->rectangle.Width = result->image->active_bitmap->width;
	result->rectangle.Height = result->image->active_bitmap->height;

	*texture = result;
	return Ok;
}

/* coverity[+alloc : arg-*2] */
GpStatus WINGDIPAPI
GdipCreateTexture (GpImage *image, GpWrapMode wrapmode, GpTexture **texture)
{
	GpImage *textureImage;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!image || !texture)
		return InvalidParameter;

	if (wrapmode > WrapModeClamp) {
		*texture = NULL;
		return OutOfMemory;
	}

	switch (image->type) {
	case ImageTypeBitmap:
		status = GdipCloneImage (image, &textureImage);
		if (status != Ok)
			return status;
		break;
	case ImageTypeMetafile:
		status = gdip_get_bitmap_from_metafile ((GpMetafile *) image, 0, 0, &textureImage);
		if (status != Ok)
			return status;
		break;
	default:
		return GenericError;
	}

	return gdip_texture_create_from_cloned_image (textureImage, wrapmode, texture);
}

/* coverity[+alloc : arg-*6] */
GpStatus WINGDIPAPI
GdipCreateTexture2 (GpImage *image, GpWrapMode wrapmode, REAL x, REAL y, REAL width, REAL height, GpTexture **texture)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	return GdipCreateTexture2I (image, wrapmode, (INT) x, (INT) y, (INT) width, (INT) height, texture);
}

/* coverity[+alloc : arg-*6] */
GpStatus WINGDIPAPI
GdipCreateTexture2I (GpImage *image, GpWrapMode wrapmode, INT x, INT y, INT width, INT height, GpTexture **texture)
{
	GpImage *textureImage;
	GpStatus status;

	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!image || !texture)
		return InvalidParameter;

	if (wrapmode > WrapModeClamp) {
		*texture = NULL;
		return OutOfMemory;
	}

	switch (image->type) {
	case ImageTypeBitmap: {
		INT bmpWidth = image->active_bitmap->width;
		INT bmpHeight = image->active_bitmap->height;
		if ((x < 0) || (y < 0) || (width <= 0) || (height <= 0) || (bmpWidth < (x + width)) || (bmpHeight < (y + height))) {
			*texture = NULL;
			return OutOfMemory;
		}

		status = GdipCloneBitmapAreaI (x, y, width, height, image->active_bitmap->pixel_format, image, &textureImage);
		if (status != Ok)
			return status;
		break;
	}
	case ImageTypeMetafile:
		status = gdip_get_bitmap_from_metafile ((GpMetafile *) image, width, height, &textureImage);
		if (status != Ok)
			return status;
		break;
	default:
		return GenericError;
	}
	
	return gdip_texture_create_from_cloned_image (textureImage, wrapmode, texture);
}

/* coverity[+alloc : arg-*6] */
GpStatus WINGDIPAPI
GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, REAL x, REAL y, REAL width, REAL height, GpTexture **texture)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	return GdipCreateTextureIAI (image, imageAttributes, (INT) x, (INT) y, (INT) width, (INT) height, texture);
}

/* coverity[+alloc : arg-*6] */
GpStatus WINGDIPAPI
GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, INT x, INT y, INT width, INT height, GpTexture **texture)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	/* FIXME MonoTODO: Make use of ImageAttributes parameter when
	 * ImageAttributes is implemented */
	GpWrapMode mode = imageAttributes ? imageAttributes->wrapmode : WrapModeTile;
	return GdipCreateTexture2I (image, mode, x, y, width, height, texture);
}

GpStatus WINGDIPAPI
GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	if (!texture || !matrix)
		return InvalidParameter;

	gdip_cairo_matrix_copy (matrix, &texture->matrix);
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetTextureTransform (GpTexture *texture, GDIPCONST GpMatrix *matrix)
{
	BOOL invertible;

	if (!texture || !matrix)
		return InvalidParameter;
		
	GdipIsMatrixInvertible ((GpMatrix *) matrix, &invertible);
	if (!invertible)
		return InvalidParameter;

	gdip_cairo_matrix_copy (&texture->matrix, matrix);
	texture->base.changed = TRUE;

	return Ok;
}

GpStatus WINGDIPAPI
GdipResetTextureTransform (GpTexture *texture)
{
	if (!texture)
		return InvalidParameter;

	cairo_matrix_init_identity (&texture->matrix);
	texture->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order)
{
	BOOL invertible;

	if (!texture)
		return InvalidParameter;
	
	if (!matrix)
		return Ok;

	/* the matrix MUST be invertible to be used */
	GdipIsMatrixInvertible (matrix, &invertible);
	if (!invertible)
		return InvalidParameter;

	if (order == MatrixOrderPrepend)
		cairo_matrix_multiply (&texture->matrix, matrix, &texture->matrix);
	else
		cairo_matrix_multiply (&texture->matrix, &texture->matrix, matrix);

	texture->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipTranslateTextureTransform (GpTexture *texture, REAL dx, REAL dy, GpMatrixOrder order)
{
	GpStatus status;

	if (!texture)
		return InvalidParameter;

	status = GdipTranslateMatrix (&texture->matrix, dx, dy, order);
	if (status != Ok)
		return status;
	
	texture->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipScaleTextureTransform (GpTexture *texture, REAL sx, REAL sy, GpMatrixOrder order)
{
	GpStatus status;

	if (!texture)
		return InvalidParameter;

	status = GdipScaleMatrix (&texture->matrix, sx, sy, order);
	if (status != Ok)
		return status;

	texture->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipRotateTextureTransform (GpTexture *texture, REAL angle, GpMatrixOrder order)
{
	GpStatus status;

	if (!texture)
		return InvalidParameter;

	status = GdipRotateMatrix (&texture->matrix, angle, order);
	if (status != Ok)
		return status;

	texture->base.changed = TRUE;
	return Ok;
}

GpStatus WINGDIPAPI
GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode)
{
	if (!texture)
		return InvalidParameter;

	/* ignore invalid GpWrapMode value */
	if (wrapMode > WrapModeClamp)
		return Ok;

	texture->wrapMode = wrapMode;
	texture->base.changed = TRUE;

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode)
{
	if (!texture || !wrapMode)
		return InvalidParameter;

	*wrapMode = texture->wrapMode;
	return Ok;
}

/* coverity[+alloc : arg-*1] */
GpStatus WINGDIPAPI
GdipGetTextureImage (GpTexture *texture, GpImage **image)
{
	if (!gdiplusInitialized)
		return GdiplusNotInitialized;

	if (!texture || !image)
		return InvalidParameter;

	return GdipCloneImage (texture->image, image);
}
