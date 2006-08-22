/*
 * texturebrush.c
 *
 * Copyright (C) Novell, Inc. 2003-2004.
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
 *
 */

#include "gdip.h"
#include "gdipImage.h"
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
	texture->rectangle = NULL;
	texture->pattern = NULL;
	GdipCreateMatrix (&texture->matrix);
}

GpTexture *
gdip_texture_new (void)
{
        GpTexture *result = (GpTexture *) GdipAlloc (sizeof (GpTexture));

	if (result != NULL) {
	        gdip_texture_init (result);
	}

        return result;
}

/* 
 * functions to create different wrapmodes.
 */
GpStatus
draw_tile_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpStatus	status;
	GpRect		*rect = brush->rectangle;
	cairo_t		*ct2;

	if (rect == NULL)  {
		return InvalidParameter;
	}

	gdip_bitmap_ensure_surface (bitmap);
	original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status(pat);
	if (status != Ok) {
		return status;
	}
	cairo_pattern_set_extend (pat, CAIRO_EXTEND_REPEAT);

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						2 * rect->Width, 2 * rect->Height);
	if (texture == NULL) {
		cairo_pattern_destroy (pat);
		return OutOfMemory;
	}

	/* Draw the texture */
	ct2 = cairo_create (texture);
	cairo_identity_matrix (ct2);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, 2 * rect->Width, 2 * rect->Height);
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

GpStatus
draw_tile_flipX_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpRect		*rect = brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;
	cairo_matrix_t	tempMatrix;

	if (rect == NULL) {
		return InvalidParameter;
	}

	gdip_bitmap_ensure_surface (bitmap);
	original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status(pat);
	if (status != Ok) {
		return status;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						2 * rect->Width, rect->Height);
	if (texture == NULL) {
		cairo_pattern_destroy (pat);
		return OutOfMemory;
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
	cairo_translate (ct2, rect->Width, 0);
	cairo_set_source (ct2, pat);
	cairo_rectangle (ct2, 0, 0, rect->Width, rect->Height);

	cairo_fill (ct2);
	cairo_destroy(ct2);

	brush->pattern = cairo_pattern_create_for_surface (texture);
	status = gdip_get_pattern_status(brush->pattern);
	if (status != Ok) {
		cairo_pattern_destroy(pat);
		cairo_surface_destroy(texture);
		return status;
	}
	cairo_pattern_set_extend (brush->pattern, CAIRO_EXTEND_REPEAT);

	cairo_pattern_destroy (pat);
	cairo_surface_destroy (texture);

	return gdip_get_status (cairo_status (ct));
}

GpStatus
draw_tile_flipY_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpMatrix	tempMatrix;
	GpRect		*rect = brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;

	if (rect == NULL) {
		return InvalidParameter;
	}

	gdip_bitmap_ensure_surface (bitmap);
	original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status(pat);
	if (status != Ok) {
		return status;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						rect->Width, 2 * rect->Height);
	if (texture == NULL) {
		cairo_pattern_destroy (pat);
		return OutOfMemory;
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

GpStatus
draw_tile_flipXY_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpMatrix	tempMatrix;
	GpRect		*rect = brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;

	if (rect == NULL) {
		return InvalidParameter;
	}

	gdip_bitmap_ensure_surface (bitmap);
	original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status(pat);
	if (status != Ok) {
		return status;
	}

	/* texture surface to be created */
	texture = cairo_surface_create_similar (original, from_cairoformat_to_content (bitmap->cairo_format),
						2 * rect->Width, 2 * rect->Height);
	if (texture == NULL) {
		cairo_pattern_destroy (pat);
		return OutOfMemory;
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

GpStatus
draw_clamp_texture (cairo_t *ct, GpBitmap *bitmap, GpTexture *brush)
{
	cairo_surface_t *original;
	cairo_surface_t *texture;
	cairo_pattern_t *pat;
	GpRect		*rect = brush->rectangle;
	GpStatus	status;
	cairo_t		*ct2;

	if (rect == NULL) {
		return InvalidParameter;
	}

	/* Original image surface */
	gdip_bitmap_ensure_surface (bitmap);
	original = bitmap->surface;

	/* Use the original as a pattern */
	pat = cairo_pattern_create_for_surface (original);
	status = gdip_get_pattern_status(pat);
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
	status = gdip_get_pattern_status(brush->pattern);
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
	GpImage		*gr_img;
	GpBitmap	*gr_bmp;
	cairo_format_t	format;
	unsigned int	width;
	unsigned int	height;
	GpStatus	status = Ok;
	BOOL		dispose_bitmap;
	GpMatrix	*product;

	if ((graphics == NULL) || (brush == NULL) || (graphics->ct == NULL)) {
		return InvalidParameter;
	}

	texture = (GpTexture *) brush;
	img = texture->image;
	if (img == NULL) {
		return InvalidParameter;
	}

	if (img->type != imageBitmap) {
		return NotImplemented;
	}

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
	width = img->active_bitmap->width;
	height = img->active_bitmap->height;
	format = img->active_bitmap->pixel_format;

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

	GdipCreateMatrix (&product);

	/* FIXME Cairo Bug: REPEAT and transformation do not go together.  */
	/* To work around this we can create an intermediate surface first */
	/* with REPEAT and use that with transformation as a pattern. But, */
	/* that gets another problem of handling rotation. Since, we need  */
	/* absolute rotation instead of rotation around 0,0.               */
	/* There are two ways to get around this problem:                  */
	/* 1. Uncommenting the following commented code once we know a     */
	/* better way to handle the rotation axis problem.      OR         */
	/* 2. Cairo bug gets fixed. Following commented code won't be      */
	/* needed in that case.                                            */
	/*
		cairo_save (ct);
		{
			temp = cairo_surface_create_similar (cairo_get_target (ct),
								format, width, height);
			
			if (temp == NULL) {
				cairo_matrix_destroy (product);
				return OutOfMemory;
			}
			
			cairo_set_target_surface (ct, temp);
			cairo_set_source (ct, texture->pattern);
			cairo_rectangle (ct, 0, 0, width, height);
			cairo_fill (ct);
		}
		cairo_restore (ct);

		pattern = cairo_pattern_create_for_surface (temp);
	*/
	pattern = texture->pattern;

	if (pattern != NULL) {
		/* Use both the matrices */
		cairo_matrix_multiply (product, texture->matrix, graphics->copy_of_ctm);
		cairo_matrix_invert (product);
		cairo_pattern_set_matrix (pattern, product);
		cairo_set_source (ct, pattern);
		/*	cairo_pattern_destroy (pattern); */
	}
	status = gdip_get_status (cairo_status (ct));
	/*	cairo_surface_destroy (temp); */
	GdipDeleteMatrix (product);

	return status;
}

GpStatus
gdip_texture_clone (GpBrush *brush, GpBrush **clonedBrush)
{
	GpTexture *result;
	GpTexture *texture;

	if (brush == NULL) {
		return InvalidParameter;
	}

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
	result->rectangle = (GpRect *) GdipAlloc (sizeof (GpRect));

	if (result->rectangle == NULL) {
		GdipFree (result);
		return OutOfMemory;
	}

	memcpy (result->rectangle, texture->rectangle, sizeof (GpRect));

	result->image = texture->image;
	cairo_surface_reference (result->image->surface);

	*clonedBrush = (GpBrush *) result;

	return Ok;
}

GpStatus
gdip_texture_destroy (GpBrush *brush)
{
	GpTexture *texture;

	if (brush == NULL) {
		return InvalidParameter;
	}

	texture = (GpTexture *) brush;

	if (texture->rectangle) {
		GdipFree (texture->rectangle);
		texture->rectangle = NULL;
	}

	if (texture->pattern) {
		cairo_pattern_destroy (texture->pattern);
		texture->pattern = NULL;
	}

	if (texture->image) {
		GdipDisposeImage (texture->image);
		texture->image = NULL;
	}

	if (texture->matrix) {
		GdipDeleteMatrix (texture->matrix);
		texture->matrix = NULL;
	}

	GdipFree (texture);

	return Ok;
}

/* coverity[+alloc : arg-*2] */
GpStatus
GdipCreateTexture (GpImage *image, GpWrapMode wrapMode, GpTexture **texture)
{
	cairo_surface_t *imageSurface;
	GpTexture	*result;

	if (image == NULL) {
		return InvalidParameter;
	}

	if (image->type != imageBitmap) {
		return NotImplemented;
	}

	imageSurface = cairo_image_surface_create_for_data ((unsigned char *)image->active_bitmap->scan0,
					image->cairo_format, image->active_bitmap->width,
					image->active_bitmap->height, image->active_bitmap->stride);
	if (imageSurface == NULL) {
		return OutOfMemory;
	}

	result = gdip_texture_new ();
	if (result == NULL) {
		cairo_surface_destroy (imageSurface);
		return OutOfMemory;
	}

	result->wrapMode = wrapMode;
	GdipCloneImage (image, &result->image);
	if (result->image->surface != NULL) {
		cairo_surface_destroy(result->image->surface);
	}
	result->image->surface = imageSurface;

	result->rectangle = (GpRect *) GdipAlloc (sizeof (GpRect));

	if (result->rectangle == NULL) {
		cairo_surface_destroy (imageSurface);
		GdipFree (result);
		return OutOfMemory;
	}

	result->rectangle->X = 0;
	result->rectangle->Y = 0;
	result->rectangle->Width = image->active_bitmap->width;
	result->rectangle->Height = image->active_bitmap->height;

	*texture = result;
	return Ok;
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
	int		bmpWidth;
	int		bmpHeight;
	cairo_t		*ct;
	cairo_surface_t *original;
	cairo_surface_t	*new;

	if (image == NULL) {
		return InvalidParameter;
	}

	if (image->type != imageBitmap) {
		return NotImplemented;
	}

	bmpWidth = image->active_bitmap->width;
	bmpHeight = image->active_bitmap->height;

	/* MS behaves this way */
	if (x < 0 || y < 0 || width < 0 || height < 0 || (bmpWidth < (x + width)) || (bmpHeight < (y + height))) {
	     return OutOfMemory;
	}

	original = cairo_image_surface_create_for_data ((unsigned char *)image->active_bitmap->scan0, 
			image->cairo_format, x + width, y + height, image->active_bitmap->stride);

	if (original == NULL) {
		return OutOfMemory;
	}

	/* texture surface to be used by brush */
	new = cairo_surface_create_similar (original, from_cairoformat_to_content (image->cairo_format), width, height);

	if (new == NULL) {
		cairo_surface_destroy (original);
		return OutOfMemory;
	}

	/* clip the rectangle from original image surface and create new surface */
	ct = cairo_create (new);
	cairo_translate (ct, -x, -y);
	cairo_set_source_surface (ct, original, x + width, y + height);
	cairo_paint (ct);
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
	(*texture)->rectangle = (GpRect *) GdipAlloc (sizeof (GpRect));

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

/* coverity[+alloc : arg-*6] */
GpStatus
GdipCreateTextureIA (GpImage *image, GpImageAttributes *imageAttributes, float x, float y, float width, float height, GpTexture **texture)
{
	/* FIXME MonoTODO: Make use of ImageAttributes parameter when
	 * ImageAttributes is implemented */
	return GdipCreateTexture2 (image, WrapModeTile, x, y, width, height, texture);
}

/* coverity[+alloc : arg-*6] */
GpStatus
GdipCreateTextureIAI (GpImage *image, GpImageAttributes *imageAttributes, int x, int y, int width, int height, GpTexture **texture)
{
	/* FIXME MonoTODO: Make use of ImageAttributes parameter when
	 * ImageAttributes is implemented */
	return GdipCreateTexture2I (image, WrapModeTile, x, y, width, height, texture);
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

	 cairo_matrix_init_identity (texture->matrix);
	texture->base.changed = TRUE;
	return Ok;
}

GpStatus
GdipMultiplyTextureTransform (GpTexture *texture, GpMatrix *matrix, GpMatrixOrder order)
{
	GpStatus status;

	if ((texture == NULL) || (matrix == NULL)) {
		return InvalidParameter;
	}

	/* FIXME: How to take care of rotation here ? */
	status = GdipMultiplyMatrix (texture->matrix, matrix, order);
	if (status == Ok)
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

	status = GdipTranslateMatrix (texture->matrix, dx, dy, order);
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

	status = GdipScaleMatrix (texture->matrix, sx, sy, order);
	if (status == Ok)
		texture->base.changed = TRUE;
	return status;
}

GpStatus
GdipRotateTextureTransform (GpTexture *texture, float angle, GpMatrixOrder order)
{
	GpStatus status;
	GpPointF axis;

	if (texture == NULL) {
		return InvalidParameter;
	}

	/* Cairo uses origin (0,0) as the axis of rotation. However, we need 
	 * to do absolute rotation. Following approach for shifting the axis
	 * of rotation was suggested by Carl.
	 */

	/* Our pattern size is 2*(texture->rect) hence its centre is following */
	axis.X = texture->rectangle->Width;
	axis.Y = texture->rectangle->Height;

	if ((status = GdipTranslateMatrix (texture->matrix, -axis.X, -axis.Y, order)) == Ok) {
		if ((status = GdipRotateMatrix (texture->matrix, angle, order)) == Ok) {
			if ((status = GdipTranslateMatrix (texture->matrix, axis.X, axis.Y, order)) == Ok) {
				texture->base.changed = TRUE;
			}
		}
	}
	return status;
}

GpStatus
GdipSetTextureWrapMode (GpTexture *texture, GpWrapMode wrapMode)
{
	if (texture == NULL) {
		return InvalidParameter;
	}

	texture->wrapMode = wrapMode;
	texture->base.changed = TRUE;

	return Ok;
}

GpStatus
GdipGetTextureWrapMode (GpTexture *texture, GpWrapMode *wrapMode)
{
	if (texture == NULL) {
		return InvalidParameter;
	}

	*wrapMode = texture->wrapMode;
	return Ok;
}

GpStatus
GdipGetTextureImage (GpTexture *texture, GpImage **image)
{
	if ((texture == NULL) || (image == NULL)) {
		return InvalidParameter;
	}

	*image = texture->image;
	return Ok;
}
