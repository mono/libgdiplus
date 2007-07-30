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
#include "graphics-private.h"
#include "bitmap-private.h"
#include "matrix-private.h"

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
	cairo_surface_t *original = NULL;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpStatus	status;
	GpRect		*rect = &brush->rectangle;
	cairo_t		*ct2;
	BYTE *premul = NULL;

	if (!rect)
		return InvalidParameter;

	gdip_bitmap_ensure_surface (bitmap);

	if (gdip_bitmap_format_needs_premultiplication (bitmap)) {
		premul = gdip_bitmap_get_premultiplied_scan0 (bitmap);
		if (premul) {
			BitmapData *data = bitmap->active_bitmap;
			original = cairo_image_surface_create_for_data (premul, CAIRO_FORMAT_ARGB32, 
				data->width, data->height, data->stride);
		}
	}

	/* if premul isn't required (or couldn't be computed, e.g. out of memory) */
	if (!original)
		original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status (pat);
	if (status != Ok)
		goto cleanup;

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						rect->Width, rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		goto cleanup;
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

	status = gdip_get_status (cairo_status (ct));

cleanup:
	if (premul) {
		cairo_surface_destroy (original);
		GdipFree (premul);
	}
	return status;
}

static GpStatus
draw_tile_flipX_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original = NULL;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;
	cairo_matrix_t	tempMatrix;
	BYTE *premul = NULL;

	if (!rect)
		return InvalidParameter;

	gdip_bitmap_ensure_surface (bitmap);

	if (gdip_bitmap_format_needs_premultiplication (bitmap)) {
		premul = gdip_bitmap_get_premultiplied_scan0 (bitmap);
		if (premul) {
			BitmapData *data = bitmap->active_bitmap;
			original = cairo_image_surface_create_for_data (premul, CAIRO_FORMAT_ARGB32, 
				data->width, data->height, data->stride);
		}
	}

	/* if premul isn't required (or couldn't be computed, e.g. out of memory) */
	if (!original)
		original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status (pat);
	if (status != Ok)
		goto cleanup;

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						2 * rect->Width, rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		goto cleanup;
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

	status = gdip_get_status (cairo_status (ct));

cleanup:
	if (premul) {
		cairo_surface_destroy (original);
		GdipFree (premul);
	}
	return status;
}

static GpStatus
draw_tile_flipY_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original = NULL;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpMatrix	tempMatrix;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;
	BYTE *premul = NULL;

	if (!rect)
		return InvalidParameter;

	gdip_bitmap_ensure_surface (bitmap);

	if (gdip_bitmap_format_needs_premultiplication (bitmap)) {
		premul = gdip_bitmap_get_premultiplied_scan0 (bitmap);
		if (premul) {
			BitmapData *data = bitmap->active_bitmap;
			original = cairo_image_surface_create_for_data (premul, CAIRO_FORMAT_ARGB32, 
				data->width, data->height, data->stride);
		}
	}

	/* if premul isn't required (or couldn't be computed, e.g. out of memory) */
	if (!original)
		original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status (pat);
	if (status != Ok)
		goto cleanup;

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						rect->Width, 2 * rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		goto cleanup;
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
		goto cleanup;
	}
	cairo_pattern_set_extend (brush->pattern, CAIRO_EXTEND_REPEAT);	

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	status = gdip_get_status (cairo_status (ct));

cleanup:
	if (premul) {
		cairo_surface_destroy (original);
		GdipFree (premul);
	}
	return status;
}

static GpStatus
draw_tile_flipXY_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original = NULL;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpMatrix	tempMatrix;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;
	BYTE *premul = NULL;

	if (!rect)
		return InvalidParameter;

	gdip_bitmap_ensure_surface (bitmap);

	if (gdip_bitmap_format_needs_premultiplication (bitmap)) {
		premul = gdip_bitmap_get_premultiplied_scan0 (bitmap);
		if (premul) {
			BitmapData *data = bitmap->active_bitmap;
			original = cairo_image_surface_create_for_data (premul, CAIRO_FORMAT_ARGB32, 
				data->width, data->height, data->stride);
		}
	}

	/* if premul isn't required (or couldn't be computed, e.g. out of memory) */
	if (!original)
		original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status(pat);
	if (status != Ok)
		goto cleanup;

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						2 * rect->Width, 2 * rect->Height);
	status = gdip_get_status (cairo_surface_status (texture));
	if (status != Ok) {
		cairo_pattern_destroy (pat);
		goto cleanup;
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
		goto cleanup;
	}
	cairo_pattern_set_extend (brush->pattern, CAIRO_EXTEND_REPEAT);

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	status = gdip_get_status (cairo_status (ct));

cleanup:
	if (premul) {
		cairo_surface_destroy (original);
		GdipFree (premul);
	}
	return status;
}

static GpStatus
draw_clamp_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpRect		*rect = &brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;

	/* Original image surface */
	gdip_bitmap_ensure_surface (bitmap);
	original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status (pat);
	if (status != Ok) {
		return status;
	}

	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						rect->Width, rect->Height);
	if (texture == NULL) {
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
		if (img == NULL) {
			return OutOfMemory;
		}
		gdip_bitmap_ensure_surface (img);
		dispose_bitmap = TRUE;
	} else {
		dispose_bitmap = FALSE;
	}

	ct = graphics->ct;

	/* We create the new pattern for brush, if the brush is changed
	 * or if pattern has not been created yet. */
	if (texture->base.changed || (texture->pattern == NULL)) {
		if (texture->pattern != NULL) {
			cairo_pattern_destroy (texture->pattern);
		}

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

	result = (GpTexture *) GdipAlloc (sizeof (GpTexture));
	if (result == NULL) {
		return OutOfMemory;
	}

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
		if (result->image)
			GdipDisposeImage (result->image);
		GdipFree (result);
		result = NULL;
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

/* coverity[+alloc : arg-*2] */
GpStatus
GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture)
{
	cairo_surface_t *imageSurface = NULL;
	GpTexture	*result;
	GpStatus status;

	if (!image || !texture)
		return InvalidParameter;

	if ((wrapMode < WrapModeTile) || (wrapMode > WrapModeClamp))
		return OutOfMemory;

	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	result = gdip_texture_new ();
	if (!result)
		return OutOfMemory;

	result->image = NULL;
	status = GdipCloneImage (image, &result->image);
	if (status != Ok)
		goto failure;

	/* note: we must keep the scan0 alive, so we must use the cloned image (and not the original) see bug #80971 */
	imageSurface = cairo_image_surface_create_for_data ((BYTE*)result->image->active_bitmap->scan0,
		image->cairo_format, image->active_bitmap->width, image->active_bitmap->height, image->active_bitmap->stride);
	if (!imageSurface)
		goto failure;

	result->wrapMode = wrapMode;
	if (result->image->surface)
		cairo_surface_destroy (result->image->surface);
	result->image->surface = imageSurface;
	result->rectangle.X = 0;
	result->rectangle.Y = 0;
	result->rectangle.Width = image->active_bitmap->width;
	result->rectangle.Height = image->active_bitmap->height;

	*texture = result;
	return Ok;

failure:
	if (result->image)
		GdipDisposeImage (result->image);
	if (imageSurface)
		cairo_surface_destroy (imageSurface);
	GdipFree (result);
	*texture = NULL;
	return status;
}

/* coverity[+alloc : arg-*6] */
GpStatus
GdipCreateTexture2 (GpImage *image, GpWrapMode wrapMode, float x, float y, float width, float height, GpTexture **texture)
{
	return GdipCreateTexture2I (image, wrapMode, (int) x, (int) y, (int) width, (int) height, texture);
}

/* coverity[+alloc : arg-*6] */
GpStatus
GdipCreateTexture2I (GpImage *image, GpWrapMode wrapMode, int x, int y, int width, int height, GpTexture **texture)
{
	int bmpWidth;
	int bmpHeight;
	GpStatus status;
	GpImage *resized_image = NULL;

	if (!image || !texture)
		return InvalidParameter;

	if (image->type != ImageTypeBitmap)
		return NotImplemented;

	bmpWidth = image->active_bitmap->width;
	bmpHeight = image->active_bitmap->height;

	/* MS behaves this way */
	if ((x < 0) || (y < 0) || (width <= 0) || (height <= 0) || (bmpWidth < (x + width)) || (bmpHeight < (y + height)))
		return OutOfMemory;

	status = GdipCloneBitmapAreaI (x, y, width, height, image->active_bitmap->pixel_format, image, &resized_image);
	if (status != Ok)
		return status;

	status = GdipCreateTexture (resized_image, wrapMode, texture);
	GdipDisposeImage (resized_image);
	return status;
}

/* coverity[+alloc : arg-*6] */
GpStatus
GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture)
{
	/* FIXME MonoTODO: Make use of ImageAttributes parameter when
	 * ImageAttributes is implemented */
	GpWrapMode mode = imageAttributes ? WrapModeClamp : WrapModeTile;
	return GdipCreateTexture2 (image, mode, x, y, width, height, texture);
}

/* coverity[+alloc : arg-*6] */
GpStatus
GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture)
{
	/* FIXME MonoTODO: Make use of ImageAttributes parameter when
	 * ImageAttributes is implemented */
	GpWrapMode mode = imageAttributes ? WrapModeClamp : WrapModeTile;
	return GdipCreateTexture2I (image, mode, x, y, width, height, texture);
}

GpStatus
GdipGetTextureTransform (GpTexture *texture, GpMatrix *matrix)
{
	if ((texture == NULL) || (matrix == NULL)) {
		return InvalidParameter;
	}

	gdip_cairo_matrix_copy(matrix, &texture->matrix);
	return Ok;
}

GpStatus
GdipSetTextureTransform (GpTexture *texture, GDIPCONST GpMatrix *matrix)
{
	if ((texture == NULL) || (matrix == NULL)) {
		return InvalidParameter;
	}

	gdip_cairo_matrix_copy(&texture->matrix, matrix);
	texture->base.changed = TRUE;

	return Ok;
}

GpStatus
GdipResetTextureTransform (GpTexture *texture)
{
	if (texture == NULL) {
		return InvalidParameter;
	}

	cairo_matrix_init_identity (&texture->matrix);
	texture->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order)
{
	GpStatus status;
	BOOL invertible = FALSE;
	cairo_matrix_t mat;

	if ((texture == NULL) || (matrix == NULL)) {
		return InvalidParameter;
	}

	/* the matrix MUST be invertible to be used */
	status = GdipIsMatrixInvertible ((GpMatrix*) matrix, &invertible);
	if (!invertible || (status != Ok))
		return InvalidParameter;

	if (order == MatrixOrderPrepend)
		cairo_matrix_multiply (&mat, matrix, &texture->matrix);
	else if (order == MatrixOrderAppend)
		cairo_matrix_multiply (&mat, &texture->matrix, matrix);

	gdip_cairo_matrix_copy (&texture->matrix, &mat);
	texture->base.changed = TRUE;
	return status;
}

GpStatus
GdipTranslateTextureTransform (GpTexture *texture, float dx, float dy, GpMatrixOrder order)
{
	GpStatus status;

	if (texture == NULL) {
		return InvalidParameter;
	}

	status = GdipTranslateMatrix (&texture->matrix, dx, dy, order);
	if (status == Ok)
		texture->base.changed = TRUE;
	return status;
}

GpStatus
GdipScaleTextureTransform (GpTexture *texture, float sx, float sy, GpMatrixOrder order)
{
	GpStatus status;

	if (texture == NULL) {
		return InvalidParameter;
	}

	status = GdipScaleMatrix (&texture->matrix, sx, sy, order);
	if (status == Ok)
		texture->base.changed = TRUE;
	return status;
}

/* MonoTODO - FIXME - this hack affect the public transform and doesn't work with MultiplyTransform */
GpStatus
GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order)
{
	GpStatus status;

	if (texture == NULL)
		return InvalidParameter;

	status = GdipRotateMatrix (&texture->matrix, angle, order);
	if (status == Ok)
		texture->base.changed = TRUE;

	return status;
}

GpStatus
GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode)
{
	if (texture == NULL)
		return InvalidParameter;

	/* ignore invalid GpWrapMode value */
	if ((wrapMode < WrapModeTile) || (wrapMode > WrapModeClamp))
		return Ok;

	texture->wrapMode = wrapMode;
	texture->base.changed = TRUE;

	return Ok;
}

GpStatus
GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode)
{
	if (!texture || !wrapMode)
		return InvalidParameter;

	*wrapMode = texture->wrapMode;
	return Ok;
}

/* coverity[+alloc : arg-*1] */
GpStatus
GdipGetTextureImage (GpTexture *texture, GpImage **image)
{
	if (!texture || !image)
		return InvalidParameter;

	return GdipCloneImage (texture->image, image);
}
