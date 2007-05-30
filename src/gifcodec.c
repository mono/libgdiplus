/* 
 * gifcodec.c : Contains function definitions for encoding decoding gif images
 *
 * Copyright (C) 2003-2004,2007 Novell, Inc (http://www.novell.com)
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
 *	Sebastien Pouliot  <sebastien@ximian.com>
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "gdiplus-private.h"

GUID gdip_gif_image_format_guid = {0xb96b3cb0U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

#ifdef HAVE_LIBGIF

#include <gif_lib.h>

#include "gifcodec.h"

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
static const BYTE gif_sig_pattern[] = { 0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x47, 0x49, 0x46, 0x38, 0x37, 0x61 };
static const BYTE gif_sig_mask[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };


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
	gif_codec.Flags = ImageCodecFlagsEncoder | ImageCodecFlagsDecoder | ImageCodecFlagsSupportBitmap | ImageCodecFlagsBuiltin;
	gif_codec.Version = 1;
	gif_codec.SigCount = 2;
	gif_codec.SigSize = 6;
	gif_codec.SigPattern = gif_sig_pattern;
	gif_codec.SigMask = gif_sig_mask;
	
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

/*
   This is the DGifSlurp and AddExtensionBlock code courtesy of giflib, 
   It's modified to not dump comments after the image block, since those 
   are still valid
*/

static int
AddExtensionBlockMono(SavedImage *New, int Len, BYTE ExtData[])
{
	ExtensionBlock	*ep;

	if (New->ExtensionBlocks == NULL) {
		New->ExtensionBlocks=(ExtensionBlock *)GdipAlloc(sizeof(ExtensionBlock));
	} else {
		New->ExtensionBlocks = (ExtensionBlock*) gdip_realloc (New->ExtensionBlocks, sizeof(ExtensionBlock) * (New->ExtensionBlockCount + 1));
	}

	if (New->ExtensionBlocks == NULL) {
		return (GIF_ERROR);
	}

	ep = &New->ExtensionBlocks[New->ExtensionBlockCount++];

	ep->ByteCount=Len;
	ep->Bytes = (char *)GdipAlloc(ep->ByteCount);
	if (ep->Bytes == NULL) {
		return (GIF_ERROR);
	}

	if (ExtData) {
		memcpy(ep->Bytes, ExtData, Len);
		ep->Function = New->Function;
	}

	return (GIF_OK);
}

static void
FreeExtensionMono(SavedImage *Image)
{
	ExtensionBlock *ep;

	if ((Image == NULL) || (Image->ExtensionBlocks == NULL)) {
		return;
	}
	for (ep = Image->ExtensionBlocks; ep < (Image->ExtensionBlocks + Image->ExtensionBlockCount); ep++) {
		(void)GdipFree((char *)ep->Bytes);
	}
	GdipFree((char *)Image->ExtensionBlocks);
	Image->ExtensionBlocks = NULL;
}

static int
gif_read_interlace (GifFileType* GifFile, SavedImage *sp, int start, int increment)
{
	int line;
	for (line = start; line < sp->ImageDesc.Height; line += increment) {
		int index = line * sp->ImageDesc.Width * sizeof (GifPixelType);
		if (DGifGetLine(GifFile, &sp->RasterBits[index], sp->ImageDesc.Width) == GIF_ERROR)
			return GIF_ERROR;
	}
	return GIF_OK;
}

static int
DGifSlurpMono(GifFileType * GifFile, SavedImage *TrailingExtensions)
{
	int		ImageSize;
	GifRecordType	RecordType;
	SavedImage	*sp;
	GifByteType	*ExtData;
	SavedImage	temp_save;

	temp_save.ExtensionBlocks = NULL;
	temp_save.ExtensionBlockCount = 0;

	if (TrailingExtensions != NULL) {
		TrailingExtensions->ExtensionBlocks = NULL;
		TrailingExtensions->ExtensionBlockCount = 0;
	}

	sp = NULL;

	do {
		if (DGifGetRecordType(GifFile, &RecordType) == GIF_ERROR) {
			return (GIF_ERROR);
		}

		switch (RecordType) {
			case IMAGE_DESC_RECORD_TYPE: {
				/* This call will leak GifFile->Image.ColorMap; there's a fixme in the DGifGetImageDesc code */
				if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
					return (GIF_ERROR);
				}

				sp = &GifFile->SavedImages[GifFile->ImageCount - 1];
				ImageSize = sp->ImageDesc.Width * sp->ImageDesc.Height;

				sp->RasterBits = (BYTE*) GdipAlloc (ImageSize * sizeof (GifPixelType));
				if (sp->RasterBits == NULL) {
					return GIF_ERROR;
				}

				if (GifFile->Image.Interlace) {
					/* first start at line 0 and read every 8th lines */
					if (gif_read_interlace (GifFile, sp, 0, 8) == GIF_ERROR)
						return GIF_ERROR;
					/* then start at line 4 and read every 8th lines */
					if (gif_read_interlace (GifFile, sp, 4, 8) == GIF_ERROR)
						return GIF_ERROR;
					/* then start at line 2 and read every 4th lines */
					if (gif_read_interlace (GifFile, sp, 2, 4) == GIF_ERROR)
						return GIF_ERROR;
					/* then start at line 1 and read every 2th lines */
					if (gif_read_interlace (GifFile, sp, 1, 2) == GIF_ERROR)
						return GIF_ERROR;
					/* all lines are read */
				} else {
					if (DGifGetLine(GifFile, sp->RasterBits, ImageSize) == GIF_ERROR) {
						return (GIF_ERROR);
					}
				}

				if (temp_save.ExtensionBlocks) {
					sp->ExtensionBlocks = temp_save.ExtensionBlocks;
					sp->ExtensionBlockCount = temp_save.ExtensionBlockCount;

					temp_save.ExtensionBlocks = NULL;
					temp_save.ExtensionBlockCount = 0;
				}
				break;
			}

			case EXTENSION_RECORD_TYPE: {
				if (DGifGetExtension(GifFile, &temp_save.Function, &ExtData) == GIF_ERROR) {
					return (GIF_ERROR);
				}

				while (ExtData != NULL) {
					/* Create an extension block with our data */
					if (AddExtensionBlockMono(&temp_save, ExtData[0], &ExtData[1]) == GIF_ERROR) {
						return (GIF_ERROR);
					}

					if (DGifGetExtensionNext(GifFile, &ExtData) == GIF_ERROR) {
						return (GIF_ERROR);
					}
					temp_save.Function = 0;
				}
				break;
			}

			case TERMINATE_RECORD_TYPE: {
				break;
			}

			default: {   /* Should be trapped by DGifGetRecordType */
				break;
			}
		}
	} while (RecordType != TERMINATE_RECORD_TYPE);

	/* In case the Gif has an extension block without an associated
	* image we return it in TrailingExtensions, if provided */
	if ((TrailingExtensions != NULL) && (temp_save.ExtensionBlocks != NULL)) {
		*TrailingExtensions = temp_save;
	}


	return (GIF_OK);
}

static GpStatus 
gdip_load_gif_image (void *stream, GpImage **image, BOOL from_file)
{
	GifFileType	*gif;
	BYTE		*readptr;
	BYTE		*writeptr;
	int		i;
	int		l;
	int		num_of_images;
	BOOL		animated;
	const GUID	*dimension;
	FrameData	*frame;
	GpBitmap	*result;
	BitmapData	*bitmap_data;
	SavedImage	si;
	SavedImage	global_extensions;
	ColorPalette	*global_palette;
	BOOL		loop_counter;
	unsigned short	loop_value;
	int		disposal;
	int		last_disposal;
	int 		transparent_index;
	int		screen_width;
	int		screen_height;
	GifImageDesc	*img_desc;

	disposal = 0;
	last_disposal = 0;
	loop_value = 0;
	global_palette = NULL;
	result = NULL;
	loop_counter = FALSE;

	if (from_file) {
		gif = DGifOpen(stream, &gdip_gif_fileinputfunc);
	} else {
		gif = DGifOpen (stream, &gdip_gif_inputfunc);
	}
	
	if (gif == NULL) {
		goto error;
	}

	/* Read the image */
	if (DGifSlurpMono(gif, &global_extensions) != GIF_OK) {
		goto error;
	}

	/* 
	   We can have either multiple pages (page dimension) or an animation (time dimension)
	   If we find an Application Extension, it's an animated gif, otherwise we've got multiple
	   pages
	*/

	screen_width = gif->SWidth;
	screen_height = gif->SHeight;
	animated = FALSE;
	num_of_images = gif->ImageCount;

	for (i = 0; i < num_of_images; i++) {
		for (l = 0; l < gif->SavedImages[i].ExtensionBlockCount; l++) {
			switch(gif->SavedImages[i].ExtensionBlocks[l].Function) {
				case APPLICATION_EXT_FUNC_CODE: {
					if (num_of_images > 1) {
						animated = TRUE;
					}

					/* Mention of the Netscape  format is here: http://list-archive.xemacs.org/xemacs-beta/199910/msg00070.html */
					/* We seem to be dealing with a bug in the gif lib here, it breaks the sub-bart part of the
					 * extension data up into a separate ExtensionBlock (of type 0x00) */
					if (gif->SavedImages[i].ExtensionBlocks[l].ByteCount > 10) {
						if (memcmp(gif->SavedImages[i].ExtensionBlocks[l].Bytes, "NETSCAPE2.0", 11) == 0) {
							loop_counter = TRUE;

							/* The next Block is a subblock of the app extension. The 3 bytes are the loop counter */
							if ((l + 1) < gif->SavedImages[i].ExtensionBlockCount) {
								if ((gif->SavedImages[i].ExtensionBlocks[l + 1].Function == 0) &&
								    (gif->SavedImages[i].ExtensionBlocks[l + 1].ByteCount == 3) &&
								    (gif->SavedImages[i].ExtensionBlocks[l + 1].Bytes[0] == 1)) {
									loop_value = (BYTE)(gif->SavedImages[i].ExtensionBlocks[l + 1].Bytes[2] << 8) + (BYTE)gif->SavedImages[i].ExtensionBlocks[l + 1].Bytes[1];
								}
							}
						}
					}
					break;
				}

				case 0: {
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
	result->type = ImageTypeBitmap;
	frame = gdip_frame_add(result, dimension);

	/* Copy the palette over, if there is one */
	if (gif->SColorMap != NULL) {
		global_palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * gif->SColorMap->ColorCount);
		global_palette->Flags = 0;
		global_palette->Count = gif->SColorMap->ColorCount;
		for (i = 0; i < gif->SColorMap->ColorCount; i++) {
			global_palette->Entries[i] = MAKE_ARGB_ARGB(0xff, gif->SColorMap->Colors[i].Red, gif->SColorMap->Colors[i].Green, gif->SColorMap->Colors[i].Blue);
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
		img_desc = &si.ImageDesc;
		if (img_desc->Top < 0 || img_desc->Height < 0 ||
		    img_desc->Left < 0 || img_desc->Width < 0 ||
		    (img_desc->Width + img_desc->Left) > screen_width ||
		    (img_desc->Height + img_desc->Top) > screen_height) {
			goto error;
		}

		for (l = 0; l < global_extensions.ExtensionBlockCount; l++) {
			ExtensionBlock eb = global_extensions.ExtensionBlocks[l];
			if (eb.Function == COMMENT_EXT_FUNC_CODE) {
				int	index;

				if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagExifUserComment, &index) != Ok) {
					BYTE *bytes = (BYTE*) GdipAlloc (eb.ByteCount + 1);
					if (bytes == NULL)
						goto error;

					memcpy (bytes, eb.Bytes, eb.ByteCount);
					bytes [eb.ByteCount] = '\0';
					gdip_bitmapdata_property_add_ASCII (bitmap_data, PropertyTagExifUserComment, bytes);
					GdipFree (bytes);
				}
			}
		}

		for (l = 0; l < si.ExtensionBlockCount; l++) {
			ExtensionBlock eb = si.ExtensionBlocks[l];

			switch(eb.Function) {
				case GRAPHICS_EXT_FUNC_CODE: {
					/* Pull animation time and/or transparent color */

					if (eb.ByteCount > 3) {	/* Sanity */
						guint32	delay;

						disposal = ((eb.Bytes [0] >> 2) & 7);
						if ((eb.Bytes[0] & 0x01) != 0) {
							/* 0 = no transparency, so we need to shift range */
							bitmap_data->transparent = -eb.Bytes[3] - 1;
						}

						delay = (eb.Bytes[2] << 8) + (eb.Bytes[1]);
						gdip_bitmapdata_property_add_long(bitmap_data, PropertyTagFrameDelay, delay);
						if (loop_counter) {
							gdip_bitmapdata_property_add_short(bitmap_data, PropertyTagLoopCount, loop_value);
						}
					}
					break;
				}

				case COMMENT_EXT_FUNC_CODE: {
					int		index;
					BYTE *text;

					/* Per-image comments override global */
					if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagExifUserComment, &index) == Ok) {
						gdip_bitmapdata_property_remove_index(bitmap_data, index);
					}
					/* String is not null terminated */
					text = (BYTE*)GdipAlloc(eb.ByteCount + 1);
					if (text != NULL) {
						memcpy(text, eb.Bytes, eb.ByteCount);
						text[eb.ByteCount] = '\0';
						gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagExifUserComment, text);
						GdipFree(text);
					}
					break;
				}

#if 0
				case PLAINTEXT_EXT_FUNC_CODE: {
					printf("Do something with PLAINTEXT_EXT_FUNC_CODE?\n");
					break;
				}
#endif
			}
		}

		/* copy the local color map if there is one, otherwise we duplicate the global one */
		if (img_desc->ColorMap != NULL) {
			ColorMapObject	*local_palette_obj;

			/* TODO: what do we do with combined gif + local palettes? */
			local_palette_obj = img_desc->ColorMap;
	
			bitmap_data->palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * local_palette_obj->ColorCount);
			bitmap_data->palette->Flags = 0;
			bitmap_data->palette->Count = local_palette_obj->ColorCount;
			for (l = 0; l < local_palette_obj->ColorCount; l++) {
				bitmap_data->palette->Entries[l] = MAKE_ARGB_ARGB(0xff, local_palette_obj->Colors[l].Red,
										local_palette_obj->Colors[l].Green,
										local_palette_obj->Colors[l].Blue);
			}
		} else {
			bitmap_data->palette = gdip_palette_clone(global_palette);

		}

		if (bitmap_data->transparent < 0) {
			BYTE *v;

			bitmap_data->palette->Flags |= PaletteFlagsHasAlpha;
			transparent_index = (bitmap_data->transparent + 1) * -1;
			v = (BYTE*)&bitmap_data->palette->Entries [transparent_index];
#ifdef WORDS_BIGENDIAN
			v[0] = 0x00;
#else
			v[3] = 0x00;
#endif /* WORDS_BIGENDIAN */
		} else {
			transparent_index = -1;
		}

		bitmap_data->pixel_format = PixelFormat8bppIndexed;
		bitmap_data->width = screen_width;
		bitmap_data->height = screen_height;
		bitmap_data->stride = bitmap_data->width;
		gdip_align_stride (bitmap_data->stride);
		bitmap_data->left = img_desc->Left;
		bitmap_data->top = img_desc->Top;

		bitmap_data->scan0 = GdipAlloc (bitmap_data->stride * bitmap_data->height);
		bitmap_data->reserved = GBD_OWN_SCAN0;
		bitmap_data->image_flags = ImageFlagsHasAlpha | ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | ImageFlagsColorSpaceRGB;
		bitmap_data->dpi_horz = gdip_get_display_dpi ();
		bitmap_data->dpi_vert = bitmap_data->dpi_horz;
	
		readptr = (BYTE*) si.RasterBits;
		writeptr = bitmap_data->scan0;

		for (l = 0; l < bitmap_data->height; l++) {
			if (l >= img_desc->Top && l - img_desc->Top < img_desc->Height) {
				/* Ignore 'disposal' 0 (don't care) and 4, 5, 6, 7 (undocumented) */
				if (i == 0 || transparent_index == -1 || (last_disposal != 1 && last_disposal != 3)) {
					writeptr += img_desc->Left;
					memcpy (writeptr, readptr, img_desc->Width);
					writeptr -= img_desc->Left;
					writeptr += bitmap_data->stride;
					readptr += img_desc->Width;
				} else {
					int ridx, widx;
					BitmapData *last_bitmap;

					last_bitmap = &frame->bitmap [i - 1];
					if (l == img_desc->Top) {
						/* Copy the previous bitmap as the base for this one */
						/* TODO: This will be wrong if each image has a different palette */
						/* There's a comment up there too */
						memcpy (bitmap_data->scan0, last_bitmap->scan0,
							bitmap_data->height * bitmap_data->stride);
					}

					for (ridx = 0, widx = bitmap_data->left; ridx < img_desc->Width; widx++, ridx++) {
						BYTE bt = readptr [ridx];
						if (bt == transparent_index)
							continue;
						writeptr [widx] = bt;
					}
					readptr += img_desc->Width;
					writeptr += bitmap_data->stride;
				}
			} else {
				writeptr += bitmap_data->stride;
			}
		}
		last_disposal = disposal;
		disposal = 0;
	}

	gdip_bitmap_setactive(result, dimension, 0);

	if (global_palette != NULL) {
		GdipFree(global_palette);
	}

	FreeExtensionMono(&global_extensions);
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

	FreeExtensionMono(&global_extensions);

	if (gif != NULL) {
		DGifCloseFile (gif);
	}

	*image = NULL;
	return InvalidParameter;
}

GpStatus 
gdip_load_gif_image_from_file (FILE *fp, GpImage **image)
{
	return gdip_load_gif_image (fp, image, TRUE);
}

GpStatus
gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc, SeekDelegate seekFunc, GpImage **image)
{
	gif_callback_data gif_data;
	
	gif_data.getBytesFunc = getBytesFunc;
	gif_data.seekFunc = seekFunc;
	
	return gdip_load_gif_image (&gif_data, image, FALSE);	
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

static GpStatus 
gdip_save_gif_image (void *stream, GpImage *image, BOOL from_file)
{
	GifFileType	*fp;
	int		i, x, y, size;
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
	BYTE		*v;
	int		c;
	int		index;
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
	pixbuf_org = NULL;

	for (frame = 0; frame < image->num_of_frames; frame++) {
		animated = FALSE;
		if (memcmp(&image->frames[frame].frame_dimension, &gdip_image_frameDimension_time_guid, sizeof(GUID)) == 0) {
			animated = TRUE;
		}
		for (k = 0; k < image->frames[frame].count; k++) {
			bitmap_data = &image->frames[frame].bitmap[k];

			pixbuf_size = bitmap_data->width * bitmap_data->height * sizeof(GifByteType);

			if (gdip_is_an_indexed_pixelformat(bitmap_data->pixel_format)) {
				BYTE w;

				switch (bitmap_data->pixel_format) {
				case PixelFormat1bppIndexed:
					cmap_size = 2;
					break;
				case PixelFormat4bppIndexed:
					cmap_size = 16;
					break;
				case PixelFormat8bppIndexed:
					cmap_size = 256;
					break;
				default:
					goto error; 
				}

				cmap = MakeMapObject(cmap_size, 0);

				pixbuf = GdipAlloc(pixbuf_size);
				if (pixbuf == NULL) {
					goto error;
				}

				pixbuf_org = pixbuf;

				for (c = 0; (c < cmap_size) && (c < bitmap_data->palette->Count); c++) {
					v = (BYTE*)&bitmap_data->palette->Entries[c];

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
					case PixelFormat1bppIndexed: {
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

					case PixelFormat4bppIndexed: {
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

					case PixelFormat8bppIndexed: {
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
					/* Store the LoopCount extension */
					if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagLoopCount, &index) == Ok) {
						BYTE Buffer[3];
						BYTE *ptr = bitmap_data->property[index].value;
						Buffer[0] = 1;
						Buffer[1] = ptr[0];
						Buffer[2] = ptr[1];
						EGifPutExtensionFirst(fp, APPLICATION_EXT_FUNC_CODE, 11, "NETSCAPE2.0");
						EGifPutExtensionLast(fp, APPLICATION_EXT_FUNC_CODE, 3, Buffer);
					}
				}

				if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagExifUserComment, &index) == Ok) {
					EGifPutComment(fp, (const char *)bitmap_data->property[index].value);
				}
			}

			/* Every image has a control extension specifying the time delay */
			if (animated || bitmap_data->transparent < 0) {
				BYTE buffer[4];

				buffer[0] = 0x03;		/* 0000 0100 = do not dispose */

				if (bitmap_data->transparent < 0) {
					buffer[0] |= 0x01;	/* 0000 0001 = transparent */
				}

				if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagFrameDelay, &index) == Ok) {
					BYTE *ptr;

					ptr = bitmap_data->property[index].value;
					buffer[1] = ptr[0];
					buffer[2] = ptr[1];
				} else {
					buffer[1] = 0;
					buffer[2] = 0;
				}

				if (bitmap_data->transparent < 0) {
					buffer[3] = (bitmap_data->transparent + 1) * -1;
				} else {
					buffer[3] = 0;
				}

				EGifPutExtension(fp, GRAPHICS_EXT_FUNC_CODE, 4, buffer);
			}

			/* Store the image description */
			/* This call will leak GifFile->Image.ColorMap */
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

GpStatus 
gdip_save_gif_image_to_file (BYTE *filename, GpImage *image)
{
	return gdip_save_gif_image ((void *)filename, image, TRUE);
}

GpStatus
gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return gdip_save_gif_image ( (void *)putBytesFunc, image, FALSE);
}

#else

/* No libgif */

#include "image.h"

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
gdip_save_gif_image_to_file (BYTE *filename, GpImage *image)
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
