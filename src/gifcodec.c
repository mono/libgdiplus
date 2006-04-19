/* 
 * gifcodec.c : Contains function definitions for encoding decoding gif images
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
 *	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Jordi Mas (jordi@ximian.com)
 *	Jonathan Gilbert (logic@deltaq.org)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include "gdipImage.h"
#include "gifcodec.h"

#ifdef HAVE_LIBGIF

#include <gif_lib.h>
#define	APP_IDENTIFIER	"Mono libgdiplus gif codec"

/* giflib declares this incorrectly as EgifOpen */
extern GifFileType *EGifOpen(void *userData, OutputFunc writeFunc);

/* Data structure used for callback */
typedef struct
{
	GetBytesDelegate getBytesFunc;
	SeekDelegate seekFunc;
	
} gif_callback_data;

/* Codecinfo related data*/
static ImageCodecInfo gif_codec;
static const WCHAR gif_codecname[] = {'B', 'u', 'i','l', 't', '-','i', 'n', ' ', 'G', 'I', 'F', ' ', 'C', 
        'o', 'd', 'e', 'c',   0}; /* Built-in GIF Codec */
static const WCHAR gif_extension[] = {'*', '.', 'G', 'I', 'F',0}; /* *.GIF */
static const WCHAR gif_mimetype[] = {'i', 'm', 'a','g', 'e', '/', 'g', 'i', 'f', 0}; /* image/gif */
static const WCHAR gif_format[] = {'G', 'I', 'F', 0}; /* GIF */

GpStatus  gdip_load_gif_image (void *stream, GpImage **image, bool from_file);
GpStatus  gdip_save_gif_image (void *stream, GpImage *image, bool from_file);

ImageCodecInfo *
gdip_getcodecinfo_gif ()
{                        
	gif_codec.Clsid = (CLSID) { 0x557cf402, 0x1a04, 0x11d3, { 0x9a, 0x73, 0x0, 0x0, 0xf8, 0x1e, 0xf3, 0x2e } };
	gif_codec.FormatID = gdip_gif_image_format_guid;
	gif_codec.CodecName = (const WCHAR*) gif_codecname;
	gif_codec.DllName = NULL;
	gif_codec.FormatDescription = (const WCHAR*) gif_format;
	gif_codec.FilenameExtension = (const WCHAR*) gif_extension;
	gif_codec.MimeType = (const WCHAR*) gif_mimetype;
	gif_codec.Flags = Encoder | Decoder | SupportBitmap | Builtin;
	gif_codec.Version = 1;
	gif_codec.SigCount = 0;
	gif_codec.SigSize = 0;
	gif_codec.SigPattern = 0;
	gif_codec.SigMask = 0;
	
	return &gif_codec;
}

/* Read callback function for the gif libbrary*/
static int 
gdip_gif_fileinputfunc (GifFileType *gif, GifByteType *data, int len) 
{
	return fread (data, 1, len, (FILE*) gif->UserData);
}

static int 
gdip_gif_inputfunc (GifFileType *gif, GifByteType *data, int len) 
{
	int read = 0;	
	gif_callback_data *gcd = (gif_callback_data*) gif->UserData;
	
	read = gcd->getBytesFunc (data, len, 0);
	return read;
}

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_load_gif_image (fp, image, TRUE);
}

GpStatus
gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           SeekDelegate seekFunc,
                                           GpImage **image)
{
	gif_callback_data gif_data;
	
	gif_data.getBytesFunc = getBytesFunc;
	gif_data.seekFunc = seekFunc;
	
	return gdip_load_gif_image (&gif_data, image, FALSE);	
}

GpStatus 
gdip_load_gif_image (void *stream, GpImage **image, bool from_file)
{
	GifFileType	*gif;
	guchar		*readptr;
	guchar		*writeptr;
	int		i;
	int		l;
	int		num_of_images;
	bool		animated;
	const GUID	*dimension;
	FrameData	*frame;
	GpBitmap	*result;
	BitmapData	*bitmap_data;
	SavedImage	si;
	ColorPalette	*global_palette;

	global_palette = NULL;
	result = NULL;

	if (from_file) {
		gif = DGifOpen(stream, &gdip_gif_fileinputfunc);
	} else {
		gif = DGifOpen (stream, &gdip_gif_inputfunc);
	}
	
	if (gif == NULL) {
		goto error;
	}

	/* Read the image */
	if (DGifSlurp (gif) != GIF_OK) {
		goto error;
	}

	/* 
	   We can have either multiple pages (page dimension) or an animation (time dimension)
	   If we find an Application Extension, it's an animated gif, otherwise we've got multiple
	   pages
	*/

	animated = FALSE;
	num_of_images = gif->ImageCount;

	for (i = 0; i < num_of_images; i++) {
		for (l = 0; l < gif->SavedImages[i].ExtensionBlockCount; l++) {
			switch(gif->SavedImages[i].ExtensionBlocks[l].Function) {
				case APPLICATION_EXT_FUNC_CODE: {
					if (num_of_images > 1) {
						animated = TRUE;
					}
					break;
				}

				case COMMENT_EXT_FUNC_CODE: {
printf("FIXME - read the comment and store it as property\n");
					/* FIXME - we should pull these into properties */
					break;
				}
			}
		}
	}

	if (animated) {
		dimension = &gdip_image_frameDimension_time_guid;
	} else {
		dimension = &gdip_image_frameDimension_page_guid;
	}

	result = gdip_bitmap_new();
	result->type = imageBitmap;
	frame = gdip_frame_add(result, dimension);

	/* Copy the palette over, if there is one */
	if (gif->SColorMap != NULL) {
		global_palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * gif->SColorMap->ColorCount);
		global_palette->Flags = 0;
		global_palette->Count = gif->SColorMap->ColorCount;
		for (i = 0; i < gif->SColorMap->ColorCount; i++) {
			global_palette->Entries[i] = MAKE_ARGB_RGB(gif->SColorMap->Colors[i].Red, gif->SColorMap->Colors[i].Green, gif->SColorMap->Colors[i].Blue);
		}
	} else {
		/* Assume a grayscale image for the global palette. Individual images might still have a different one. */
		global_palette = GdipAlloc (sizeof(ColorPalette) + 256 * sizeof(ARGB));
		global_palette->Flags = PaletteFlagsGrayScale;
		global_palette->Count = 256;
		for (i=0; i < 256; i++) {
			global_palette->Entries[i] = MAKE_ARGB_RGB(i, i, i);
		}
	}

	result->cairo_format = CAIRO_FORMAT_A8;

	/* create our bitmaps */
	for (i = 0; i < num_of_images; i++) {

		/* Add BitmapData to our frame */
		bitmap_data = gdip_frame_add_bitmapdata(frame);
		if (bitmap_data == NULL) {
			goto error;
		}

		si = gif->SavedImages[i];

		for (l = 0; l < si.ExtensionBlockCount; l++) {
			ExtensionBlock eb = si.ExtensionBlocks[l];

			switch(eb.Function) {
				case GRAPHICS_EXT_FUNC_CODE: {
					/* Pull animation time and/or transparent color */

					printf("Doing extension. Bytecount = %d, Got transparency=%d, Delay=%d\n", eb.ByteCount, ((eb.Bytes[0] & 0x80) != 0), (eb.Bytes[1] << 8) + eb.Bytes[2]);
					if (eb.ByteCount > 3) {	/* Sanity */
						if ((eb.Bytes[0] & 0x80) != 0) {
							/* We've got transparency; we store it negative, offset by one. 
							   This gives -257 to -1 as valid numbers and allows us to distinguish 0 and not transparent */
							// FIXME - store 'eb.Bytes[3]' in properties as transparent color;
						}
						// FIXME - store '(eb.Bytes[1] << 8) + eb.Bytes[2]' in properties as delay time
					}
					break;
				}

				case COMMENT_EXT_FUNC_CODE: {
					printf("FIXME - Have COMMENT_EXT_FUNC_CODE\n");
					break;
				}

				case APPLICATION_EXT_FUNC_CODE: {
					printf("FIXME - Have APPLICATION_EXT_FUNC_CODE\n");
					break;
				}

				case PLAINTEXT_EXT_FUNC_CODE: {
					printf("FIXME - Have PLAINTEXT_EXT_FUNC_CODE\n");
					break;
				}
			}
		}

		/* copy the local color map if there is one, otherwise we duplicate the global one */
		if (si.ImageDesc.ColorMap != NULL) {
			ColorMapObject	*local_palette_obj;

			local_palette_obj = si.ImageDesc.ColorMap;
	
			bitmap_data->palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * local_palette_obj->ColorCount);
			bitmap_data->palette->Flags = 0;
			bitmap_data->palette->Count = local_palette_obj->ColorCount;
			for (l = 0; l < local_palette_obj->ColorCount; l++) {
				bitmap_data->palette->Entries[l] = MAKE_ARGB_RGB(local_palette_obj->Colors[l].Red,
										local_palette_obj->Colors[l].Green,
										local_palette_obj->Colors[l].Blue);
			}
		} else {
			bitmap_data->palette = gdip_palette_clone(global_palette);
		}

		bitmap_data->pixel_format = Format8bppIndexed;
		bitmap_data->width = si.ImageDesc.Width;
		bitmap_data->height = si.ImageDesc.Height;
		bitmap_data->stride = (bitmap_data->width + sizeof(pixman_bits_t) - 1) & ~(sizeof(pixman_bits_t) - 1);
		bitmap_data->left = si.ImageDesc.Left;
		bitmap_data->top = si.ImageDesc.Top;

		bitmap_data->scan0 = GdipAlloc (bitmap_data->stride * bitmap_data->height);
		bitmap_data->reserved = GBD_OWN_SCAN0;
		bitmap_data->image_flags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | ImageFlagsColorSpaceRGB;
		bitmap_data->dpi_horz = 0;
		bitmap_data->dpi_vert = 0;
	
		readptr = (guchar *) si.RasterBits;
		writeptr = bitmap_data->scan0;

		for (l = 0; l < bitmap_data->height; l++) {
			memcpy(writeptr, readptr, bitmap_data->width);
			readptr += bitmap_data->width;
			writeptr += bitmap_data->stride;
		}
	}

	gdip_bitmap_setactive(result, dimension, 0);

	if (global_palette != NULL) {
		GdipFree(global_palette);
	}

	DGifCloseFile (gif);

	*image = result;
	return Ok;

error:	
	if (global_palette != NULL) {
		GdipFree(global_palette);
	}

	if (result != NULL) {
		gdip_bitmap_dispose (result);
	}

	if (gif != NULL) {
		DGifCloseFile (gif);
	}

	*image = NULL;
	return InvalidParameter;
}

/* Write callback function for the gif libbrary*/
static int 
gdip_gif_outputfunc (GifFileType *gif,  const GifByteType *data, int len) 
{
	int written = 0;	
	PutBytesDelegate putBytesFunc = (PutBytesDelegate) gif->UserData;
	
	written = putBytesFunc ((void *)data, len);
	return written;
}


GpStatus 
gdip_save_gif_image_to_file (unsigned char *filename, GpImage *image)
{
	return gdip_save_gif_image ( (void *)filename, image, TRUE);
}

GpStatus
gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)
{
	return gdip_save_gif_image ( (void *)putBytesFunc, image, FALSE);
}

GpStatus 
gdip_save_gif_image (void *stream, GpImage *image, bool from_file)
{
	GifFileType	*fp;
	int i, x, y, size;
	GifByteType	*red;
	GifByteType	*green;
	GifByteType	*blue;
	GifByteType	*red_ptr;
	GifByteType	*green_ptr;
	GifByteType	*blue_ptr;
	GifByteType	*pixbuf;
	GifByteType	*pixbuf_org;
	int		cmap_size;
	ColorMapObject *cmap = NULL;	
	int		k;
	unsigned char	*v;
	int		c;
	BOOL		animated;
	int		frame;
	BitmapData	*bitmap_data;
	int		pixbuf_size;

	if (!stream) {
		return InvalidParameter;
	}

	if (from_file) {
		fp = EGifOpenFileName (stream, 0);
	} else {
		fp = EGifOpen (stream, gdip_gif_outputfunc);
	}
		
	if (!fp) {
		return FileNotFound;
	}

	red = NULL;
	green = NULL;
	blue = NULL;

	for (frame = 0; frame < image->num_of_frames; frame++) {
		animated = FALSE;
		if (memcmp(&image->frames[frame].frame_dimension, &gdip_image_frameDimension_time_guid, sizeof(GUID)) == 0) {
			animated = TRUE;
		}
		for (k = 0; k < image->frames[frame].count; k++) {
			bitmap_data = &image->frames[frame].bitmap[k];

			pixbuf_size = bitmap_data->width * bitmap_data->height * sizeof(GifByteType);

			if (gdip_is_an_indexed_pixelformat(bitmap_data->pixel_format)) {
				unsigned char w;

				switch(bitmap_data->pixel_format) {
					case Format1bppIndexed: cmap_size =   2; break;
					case Format4bppIndexed: cmap_size =  16; break;
					case Format8bppIndexed: cmap_size = 256; break;
				}

				cmap = MakeMapObject(cmap_size, 0);

				pixbuf = GdipAlloc(pixbuf_size);
				if (pixbuf == NULL) {
					goto error;
				}

				pixbuf_org = pixbuf;

				for (c = 0; (c < cmap_size) && (c < bitmap_data->palette->Count); c++) {
					v = (unsigned char *)&bitmap_data->palette->Entries[c];

#ifdef WORDS_BIGENDIAN
					cmap->Colors[c].Red =   v[1];
					cmap->Colors[c].Green = v[2];
					cmap->Colors[c].Blue =  v[3];
#else
					cmap->Colors[c].Red =   v[2];
					cmap->Colors[c].Green = v[1];
					cmap->Colors[c].Blue =  v[0];
#endif /* WORDS_BIGENDIAN */
				}

				switch(bitmap_data->pixel_format) {
					case Format1bppIndexed: {
						for (y = 0; y < bitmap_data->height; y++) {
							v = bitmap_data->scan0 + y * bitmap_data->stride;
							for (x = 0; x + 7 < bitmap_data->width; x += 8) {
								w = *v;

								*(pixbuf++) = ((w & 0x80) != 0);
								*(pixbuf++) = ((w & 0x40) != 0);
								*(pixbuf++) = ((w & 0x20) != 0);
								*(pixbuf++) = ((w & 0x10) != 0);
								*(pixbuf++) = ((w & 0x08) != 0);
								*(pixbuf++) = ((w & 0x04) != 0);
								*(pixbuf++) = ((w & 0x02) != 0);
								*(pixbuf++) = ((w & 0x01) != 0);
								
								v++;
							}

							w = *v;

							switch (bitmap_data->width & 7) {/* every 'case' here flows into the next */
								case 7: pixbuf[6] = ((w & 0x02) != 0);
								case 6: pixbuf[5] = ((w & 0x04) != 0);
								case 5: pixbuf[4] = ((w & 0x08) != 0);
								case 4: pixbuf[3] = ((w & 0x10) != 0);
								case 3: pixbuf[2] = ((w & 0x20) != 0);
								case 2: pixbuf[1] = ((w & 0x40) != 0);
								case 1: pixbuf[0] = ((w & 0x80) != 0);
							}

							pixbuf += (bitmap_data->width & 7);
						}
						break;
					}

					case Format4bppIndexed: {
						for (y = 0; y < bitmap_data->height; y++) {
							v = bitmap_data->scan0 + y * bitmap_data->stride;
							for (x = 0; x + 1 < bitmap_data->width; x += 2) {
								w = *v;

								*(pixbuf++) = ((w >> 4) & 0xF);
								*(pixbuf++) = ( w       & 0xF);
								
								v++;
							}

							if ((bitmap_data->width & 1) != 0) {
								*(pixbuf++) = ((*v >> 4) & 0xF);
							}
						}
						break;
					}

					case Format8bppIndexed: {
						for (y = 0; y < bitmap_data->height; y++) {
							memcpy(pixbuf + y * bitmap_data->width,
							       bitmap_data->scan0 + y * bitmap_data->stride,
							       bitmap_data->width);
						}
						break;
					}
				}
				/* Restore pointer, 1bpp and 4bpp above alter it */
				pixbuf = pixbuf_org;
			} else {
				cmap_size = 256;
				cmap  = MakeMapObject (cmap_size, 0);

				red = GdipAlloc(pixbuf_size);
				green = GdipAlloc(pixbuf_size);
				blue = GdipAlloc(pixbuf_size);
				pixbuf = GdipAlloc(pixbuf_size);
				if ((red == NULL) || (green == NULL) || (blue == NULL) || (pixbuf == NULL)) {
					goto error;
				}

				pixbuf_org = pixbuf;
				red_ptr = red;
				green_ptr = green;
				blue_ptr = blue;

				for (y = 0; y < bitmap_data->height; y++) {
					v = bitmap_data->scan0 + y * bitmap_data->stride;
					for (x = 0; x < bitmap_data->width; x++) {
#ifdef WORDS_BIGENDIAN
						*red_ptr++ =   v[1];
						*green_ptr++ = v[2];
						*blue_ptr++ =  v[3];
#else
						*red_ptr++ =   v[2];
						*green_ptr++ = v[1];
						*blue_ptr++ =  v[0];
#endif

						v += 4;
					}
				}
				if (QuantizeBuffer(bitmap_data->width, bitmap_data->height, &cmap_size, 
						red,  green, blue, pixbuf, cmap->Colors) == GIF_ERROR) {
					goto error;
				}
			}

			cmap->BitsPerPixel = BitSize (cmap_size);
			cmap->ColorCount = 1 << cmap->BitsPerPixel;

			if ((frame == 0) && (k == 0)) {
				/* First Image defines the global colormap */
				if (EGifPutScreenDesc (fp, bitmap_data->width, bitmap_data->height, cmap->BitsPerPixel, 0, cmap) == GIF_ERROR) {
					goto error;
				}

				/* An animated image must have the application extension */
				if (animated) {
					EGifPutExtension (fp, APPLICATION_EXT_FUNC_CODE, strlen(APP_IDENTIFIER) + 1, APP_IDENTIFIER);
				}
				#if 0
					/* This will make it loop */
					EGifPutExtensions(fp, APPLICATION_EXT_FUNC_CODE, 11, "NETSCAPE2.0");
					EGifPutExtensions(fp, 0, 2, "\x03\x01\0");
				#endif
			}

			/* Every image has a control extension specifying the time delay */
			if (animated) {
				unsigned char	buffer[4];

				buffer[0] = 0x03;		/* 0000 0100 = do not dispose */
				#if 0	// FIXME - read properties and store transparent and delay numbers
				if (bitmap_data->transparent < 0) {
					buffer[0] |= 0x01;	/* 0000 0001 = transparent */
				}
				buffer[1] = (bitmap_data->delay >> 8) & 0xff;
				buffer[2] = bitmap_data->delay & 0xff;
				if (bitmap_data->transparent < 0) {
					buffer[3] = (bitmap_data->transparent + 1) * -1;
				} else {
					buffer[3] = 0;
				}
				#else
					buffer[1] = 0;
					buffer[2] = 0;
					buffer[3] = 0;
				#endif

				EGifPutExtension(fp, GRAPHICS_EXT_FUNC_CODE, 4, buffer);
			}

			/* Store the image description */
			if (EGifPutImageDesc (fp, bitmap_data->left, bitmap_data->top, bitmap_data->width, bitmap_data->height, FALSE, cmap) == GIF_ERROR) {
				goto error;
			}

			for (i = 0;  i < bitmap_data->height;  ++i) {
				if (EGifPutLine (fp, pixbuf, bitmap_data->width) == GIF_ERROR) {
					goto error;
				}
				pixbuf += bitmap_data->width;
			}

			FreeMapObject (cmap);
			if (red != NULL) {
				GdipFree (red);
			}
			if (green != NULL) {
				GdipFree (green);
			}
			if (blue != NULL) {
				GdipFree (blue);
			}

			if (pixbuf_org != NULL) {
				GdipFree (pixbuf_org);
			}

			red = NULL;
			green = NULL;
			blue = NULL;
			pixbuf_org = NULL;
		}
	}

	EGifCloseFile (fp);	
	
	return Ok;

error:
	if (cmap != NULL) {
		FreeMapObject (cmap);
	}

	if (red != NULL) {
		GdipFree (red);
	}

	if (green != NULL) {
		GdipFree (green);
	}

	if (blue != NULL) {
		GdipFree (blue);
	}

	if (pixbuf_org != NULL) {
		GdipFree (pixbuf_org);
	}

	return GenericError;
}

#else

/* No libgif */

ImageCodecInfo *
gdip_getcodecinfo_gif ()
{
	return NULL;
}

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

GpStatus 
gdip_save_gif_image_to_file (unsigned char *filename, GpImage *image)
{
	return UnknownImageFormat;
}

GpStatus
gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc,
                                         GpImage *image,
                                         GDIPCONST EncoderParameters *params)
{
	return UnknownImageFormat;
}

GpStatus
gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc,
                                           SeekDelegate seekFunc,
                                           GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

#endif
