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

#include "config.h"
#include "codecs-private.h"
#include "gifcodec.h"

GUID gdip_gif_image_format_guid = {0xb96b3cb0U, 0x0728U, 0x11d3U, {0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e}};

#ifdef HAVE_LIBGIF

#include <gif_lib.h>
#include <stdint.h>

#include "gifcodec.h"


/* START GifQuantizeBuffer copy from giflib

The giflib 5.2.0 release notes mention:

> The undocumented and deprecated GifQuantizeBuffer() entry point
> has been moved to the util library to reduce libgif size and attack
> surface. Applications needing this function are couraged to link the
> util library or make their own copy.

Since the util library doesn't get installed in most distros we can't
link against it and need to make our own copy called LibgdiplusGifQuantizeBuffer.
This is taken from giflib 52b62de83d5facbbbde042b85bf3f61182e3bebd.

> The GIFLIB distribution is Copyright (c) 1997  Eric S. Raymond
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in
> all copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
> THE SOFTWARE.

*/

/*****************************************************************************

 quantize.c - quantize a high resolution image into lower one

 Based on: "Color Image Quantization for frame buffer Display", by
 Paul Heckbert SIGGRAPH 1982 page 297-307.

 This doesn't really belong in the core library, was undocumented,
 and was removed in 4.2.  Then it turned out some client apps were
 actually using it, so it was restored in 5.0.

SPDX-License-Identifier: MIT

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "gif_lib.h"
//#include "gif_lib_private.h"

//#define ABS(x)    ((x) > 0 ? (x) : (-(x)))

#define COLOR_ARRAY_SIZE 32768
#define BITS_PER_PRIM_COLOR 5
#define MAX_PRIM_COLOR      0x1f

static int SortRGBAxis;

typedef struct QuantizedColorType {
    GifByteType RGB[3];
    GifByteType NewColorIndex;
    long Count;
    struct QuantizedColorType *Pnext;
} QuantizedColorType;

typedef struct NewColorMapType {
    GifByteType RGBMin[3], RGBWidth[3];
    unsigned int NumEntries; /* # of QuantizedColorType in linked list below */
    unsigned long Count; /* Total number of pixels in all the entries */
    QuantizedColorType *QuantizedColors;
} NewColorMapType;

static int SubdivColorMap(NewColorMapType * NewColorSubdiv,
                          unsigned int ColorMapSize,
                          unsigned int *NewColorMapSize);
static int SortCmpRtn(const void *Entry1, const void *Entry2);

/******************************************************************************
 Quantize high resolution image into lower one. Input image consists of a
 2D array for each of the RGB colors with size Width by Height. There is no
 Color map for the input. Output is a quantized image with 2D array of
 indexes into the output color map.
   Note input image can be 24 bits at the most (8 for red/green/blue) and
 the output has 256 colors at the most (256 entries in the color map.).
 ColorMapSize specifies size of color map up to 256 and will be updated to
 real size before returning.
   Also non of the parameter are allocated by this routine.
   This function returns GIF_OK if successful, GIF_ERROR otherwise.
******************************************************************************/
int
LibgdiplusGifQuantizeBuffer(unsigned int Width,
               unsigned int Height,
               int *ColorMapSize,
               GifByteType * RedInput,
               GifByteType * GreenInput,
               GifByteType * BlueInput,
               GifByteType * OutputBuffer,
               GifColorType * OutputColorMap) {

    unsigned int Index, NumOfEntries;
    int i, j, MaxRGBError[3];
    unsigned int NewColorMapSize;
    long Red, Green, Blue;
    NewColorMapType NewColorSubdiv[256];
    QuantizedColorType *ColorArrayEntries, *QuantizedColor;

    ColorArrayEntries = (QuantizedColorType *)malloc(
                           sizeof(QuantizedColorType) * COLOR_ARRAY_SIZE);
    if (ColorArrayEntries == NULL) {
        return GIF_ERROR;
    }

    for (i = 0; i < COLOR_ARRAY_SIZE; i++) {
        ColorArrayEntries[i].RGB[0] = i >> (2 * BITS_PER_PRIM_COLOR);
        ColorArrayEntries[i].RGB[1] = (i >> BITS_PER_PRIM_COLOR) &
           MAX_PRIM_COLOR;
        ColorArrayEntries[i].RGB[2] = i & MAX_PRIM_COLOR;
        ColorArrayEntries[i].Count = 0;
    }

    /* Sample the colors and their distribution: */
    for (i = 0; i < (int)(Width * Height); i++) {
        Index = ((RedInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                  (2 * BITS_PER_PRIM_COLOR)) +
                ((GreenInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                  BITS_PER_PRIM_COLOR) +
                (BlueInput[i] >> (8 - BITS_PER_PRIM_COLOR));
        ColorArrayEntries[Index].Count++;
    }

    /* Put all the colors in the first entry of the color map, and call the
     * recursive subdivision process.  */
    for (i = 0; i < 256; i++) {
        NewColorSubdiv[i].QuantizedColors = NULL;
        NewColorSubdiv[i].Count = NewColorSubdiv[i].NumEntries = 0;
        for (j = 0; j < 3; j++) {
            NewColorSubdiv[i].RGBMin[j] = 0;
            NewColorSubdiv[i].RGBWidth[j] = 255;
        }
    }

    /* Find the non empty entries in the color table and chain them: */
    for (i = 0; i < COLOR_ARRAY_SIZE; i++)
        if (ColorArrayEntries[i].Count > 0)
            break;
    QuantizedColor = NewColorSubdiv[0].QuantizedColors = &ColorArrayEntries[i];
    NumOfEntries = 1;
    while (++i < COLOR_ARRAY_SIZE)
        if (ColorArrayEntries[i].Count > 0) {
            QuantizedColor->Pnext = &ColorArrayEntries[i];
            QuantizedColor = &ColorArrayEntries[i];
            NumOfEntries++;
        }
    QuantizedColor->Pnext = NULL;

    NewColorSubdiv[0].NumEntries = NumOfEntries; /* Different sampled colors */
    NewColorSubdiv[0].Count = ((long)Width) * Height; /* Pixels */
    NewColorMapSize = 1;
    if (SubdivColorMap(NewColorSubdiv, *ColorMapSize, &NewColorMapSize) !=
       GIF_OK) {
        free((char *)ColorArrayEntries);
        return GIF_ERROR;
    }
    if (NewColorMapSize < *ColorMapSize) {
        /* And clear rest of color map: */
        for (i = NewColorMapSize; i < *ColorMapSize; i++)
            OutputColorMap[i].Red = OutputColorMap[i].Green =
                OutputColorMap[i].Blue = 0;
    }

    /* Average the colors in each entry to be the color to be used in the
     * output color map, and plug it into the output color map itself. */
    for (i = 0; i < NewColorMapSize; i++) {
        if ((j = NewColorSubdiv[i].NumEntries) > 0) {
            QuantizedColor = NewColorSubdiv[i].QuantizedColors;
            Red = Green = Blue = 0;
            while (QuantizedColor) {
                QuantizedColor->NewColorIndex = i;
                Red += QuantizedColor->RGB[0];
                Green += QuantizedColor->RGB[1];
                Blue += QuantizedColor->RGB[2];
                QuantizedColor = QuantizedColor->Pnext;
            }
            OutputColorMap[i].Red = (Red << (8 - BITS_PER_PRIM_COLOR)) / j;
            OutputColorMap[i].Green = (Green << (8 - BITS_PER_PRIM_COLOR)) / j;
            OutputColorMap[i].Blue = (Blue << (8 - BITS_PER_PRIM_COLOR)) / j;
        }
    }

    /* Finally scan the input buffer again and put the mapped index in the
     * output buffer.  */
    MaxRGBError[0] = MaxRGBError[1] = MaxRGBError[2] = 0;
    for (i = 0; i < (int)(Width * Height); i++) {
        Index = ((RedInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                 (2 * BITS_PER_PRIM_COLOR)) +
                ((GreenInput[i] >> (8 - BITS_PER_PRIM_COLOR)) <<
                 BITS_PER_PRIM_COLOR) +
                (BlueInput[i] >> (8 - BITS_PER_PRIM_COLOR));
        Index = ColorArrayEntries[Index].NewColorIndex;
        OutputBuffer[i] = Index;
        if (MaxRGBError[0] < ABS(OutputColorMap[Index].Red - RedInput[i]))
            MaxRGBError[0] = ABS(OutputColorMap[Index].Red - RedInput[i]);
        if (MaxRGBError[1] < ABS(OutputColorMap[Index].Green - GreenInput[i]))
            MaxRGBError[1] = ABS(OutputColorMap[Index].Green - GreenInput[i]);
        if (MaxRGBError[2] < ABS(OutputColorMap[Index].Blue - BlueInput[i]))
            MaxRGBError[2] = ABS(OutputColorMap[Index].Blue - BlueInput[i]);
    }

#ifdef DEBUG
    fprintf(stderr,
            "Quantization L(0) errors: Red = %d, Green = %d, Blue = %d.\n",
            MaxRGBError[0], MaxRGBError[1], MaxRGBError[2]);
#endif /* DEBUG */

    free((char *)ColorArrayEntries);

    *ColorMapSize = NewColorMapSize;

    return GIF_OK;
}

/******************************************************************************
 Routine to subdivide the RGB space recursively using median cut in each
 axes alternatingly until ColorMapSize different cubes exists.
 The biggest cube in one dimension is subdivide unless it has only one entry.
 Returns GIF_ERROR if failed, otherwise GIF_OK.
*******************************************************************************/
static int
SubdivColorMap(NewColorMapType * NewColorSubdiv,
               unsigned int ColorMapSize,
               unsigned int *NewColorMapSize) {

    unsigned int i, j, Index = 0;
    QuantizedColorType *QuantizedColor, **SortArray;

    while (ColorMapSize > *NewColorMapSize) {
        /* Find candidate for subdivision: */
	long Sum, Count;
        int MaxSize = -1;
	unsigned int NumEntries, MinColor, MaxColor;
        for (i = 0; i < *NewColorMapSize; i++) {
            for (j = 0; j < 3; j++) {
                if ((((int)NewColorSubdiv[i].RGBWidth[j]) > MaxSize) &&
                      (NewColorSubdiv[i].NumEntries > 1)) {
                    MaxSize = NewColorSubdiv[i].RGBWidth[j];
                    Index = i;
                    SortRGBAxis = j;
                }
            }
        }

        if (MaxSize == -1)
            return GIF_OK;

        /* Split the entry Index into two along the axis SortRGBAxis: */

        /* Sort all elements in that entry along the given axis and split at
         * the median.  */
        SortArray = (QuantizedColorType **)malloc(
                      sizeof(QuantizedColorType *) * 
                      NewColorSubdiv[Index].NumEntries);
        if (SortArray == NULL)
            return GIF_ERROR;
        for (j = 0, QuantizedColor = NewColorSubdiv[Index].QuantizedColors;
             j < NewColorSubdiv[Index].NumEntries && QuantizedColor != NULL;
             j++, QuantizedColor = QuantizedColor->Pnext)
            SortArray[j] = QuantizedColor;

	/*
	 * Because qsort isn't stable, this can produce differing 
	 * results for the order of tuples depending on platform
	 * details of how qsort() is implemented.
	 *
	 * We mitigate this problem by sorting on all three axes rather
	 * than only the one specied by SortRGBAxis; that way the instability
	 * can only become an issue if there are multiple color indices
	 * referring to identical RGB tuples.  Older versions of this 
	 * sorted on only the one axis.
	 */
        qsort(SortArray, NewColorSubdiv[Index].NumEntries,
              sizeof(QuantizedColorType *), SortCmpRtn);

        /* Relink the sorted list into one: */
        for (j = 0; j < NewColorSubdiv[Index].NumEntries - 1; j++)
            SortArray[j]->Pnext = SortArray[j + 1];
        SortArray[NewColorSubdiv[Index].NumEntries - 1]->Pnext = NULL;
        NewColorSubdiv[Index].QuantizedColors = QuantizedColor = SortArray[0];
        free((char *)SortArray);

        /* Now simply add the Counts until we have half of the Count: */
        Sum = NewColorSubdiv[Index].Count / 2 - QuantizedColor->Count;
        NumEntries = 1;
        Count = QuantizedColor->Count;
        while (QuantizedColor->Pnext != NULL &&
	       (Sum -= QuantizedColor->Pnext->Count) >= 0 &&
               QuantizedColor->Pnext->Pnext != NULL) {
            QuantizedColor = QuantizedColor->Pnext;
            NumEntries++;
            Count += QuantizedColor->Count;
        }
        /* Save the values of the last color of the first half, and first
         * of the second half so we can update the Bounding Boxes later.
         * Also as the colors are quantized and the BBoxes are full 0..255,
         * they need to be rescaled.
         */
        MaxColor = QuantizedColor->RGB[SortRGBAxis]; /* Max. of first half */
	/* coverity[var_deref_op] */
        MinColor = QuantizedColor->Pnext->RGB[SortRGBAxis]; /* of second */
        MaxColor <<= (8 - BITS_PER_PRIM_COLOR);
        MinColor <<= (8 - BITS_PER_PRIM_COLOR);

        /* Partition right here: */
        NewColorSubdiv[*NewColorMapSize].QuantizedColors =
           QuantizedColor->Pnext;
        QuantizedColor->Pnext = NULL;
        NewColorSubdiv[*NewColorMapSize].Count = Count;
        NewColorSubdiv[Index].Count -= Count;
        NewColorSubdiv[*NewColorMapSize].NumEntries =
           NewColorSubdiv[Index].NumEntries - NumEntries;
        NewColorSubdiv[Index].NumEntries = NumEntries;
        for (j = 0; j < 3; j++) {
            NewColorSubdiv[*NewColorMapSize].RGBMin[j] =
               NewColorSubdiv[Index].RGBMin[j];
            NewColorSubdiv[*NewColorMapSize].RGBWidth[j] =
               NewColorSubdiv[Index].RGBWidth[j];
        }
        NewColorSubdiv[*NewColorMapSize].RGBWidth[SortRGBAxis] =
           NewColorSubdiv[*NewColorMapSize].RGBMin[SortRGBAxis] +
           NewColorSubdiv[*NewColorMapSize].RGBWidth[SortRGBAxis] - MinColor;
        NewColorSubdiv[*NewColorMapSize].RGBMin[SortRGBAxis] = MinColor;

        NewColorSubdiv[Index].RGBWidth[SortRGBAxis] =
           MaxColor - NewColorSubdiv[Index].RGBMin[SortRGBAxis];

        (*NewColorMapSize)++;
    }

    return GIF_OK;
}

/****************************************************************************
 Routine called by qsort to compare two entries.
*****************************************************************************/

static int
SortCmpRtn(const void *Entry1,
           const void *Entry2) {
	   QuantizedColorType *entry1 = (*((QuantizedColorType **) Entry1));
	   QuantizedColorType *entry2 = (*((QuantizedColorType **) Entry2));

	   /* sort on all axes of the color space! */
	   int hash1 = entry1->RGB[SortRGBAxis] * 256 * 256
	   			+ entry1->RGB[(SortRGBAxis+1) % 3] * 256
				+ entry1->RGB[(SortRGBAxis+2) % 3];
	   int hash2 = entry2->RGB[SortRGBAxis] * 256 * 256
	   			+ entry2->RGB[(SortRGBAxis+1) % 3] * 256
				+ entry2->RGB[(SortRGBAxis+2) % 3];

    return hash1 - hash2;
}

/* END GifQuantizeBuffer copy from giflib */

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
AddExtensionBlockMono(SavedImage *New, int Function, int Len, BYTE ExtData[])
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

	ep->Function = Function;
	ep->ByteCount=Len;
#if GIFLIB_MAJOR >= 5
	ep->Bytes = (GifByteType *)GdipAlloc(ep->ByteCount);
#else
	ep->Bytes = (char *)GdipAlloc(ep->ByteCount);
#endif
	if (ep->Bytes == NULL) {
		GdipFree (New->ExtensionBlocks);
		New->ExtensionBlocks = NULL;
		return (GIF_ERROR);
	}

	if (ExtData) {
		memcpy(ep->Bytes, ExtData, Len);
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

#ifndef SIZE_MAX
    #define SIZE_MAX     UINTPTR_MAX
#endif

static int
DGifSlurpMono(GifFileType * GifFile, SavedImage *TrailingExtensions)
{
	int		ImageSize;
	int		Function;
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
			goto error;
		}

		switch (RecordType) {
			case IMAGE_DESC_RECORD_TYPE: {
				if (DGifGetImageDesc(GifFile) == GIF_ERROR) {
					goto error;
				}

				sp = &GifFile->SavedImages[GifFile->ImageCount - 1];
				/* Allocate memory for the image */
				if (sp->ImageDesc.Width < 0 && sp->ImageDesc.Height < 0 && sp->ImageDesc.Width > (INT_MAX / sp->ImageDesc.Height)) {
					goto error;
				}

				ImageSize = sp->ImageDesc.Width * sp->ImageDesc.Height;
				if (ImageSize == 0 || ImageSize > (SIZE_MAX / sizeof(GifPixelType))) {
					goto error;
				}

				sp->RasterBits = (BYTE *) GdipAlloc (ImageSize * sizeof(GifPixelType));
				if (sp->RasterBits == NULL) {
					goto error;
				}

				if (sp->ImageDesc.Interlace) {
					int i, j;
					/* 
					* The way an interlaced image should be read - 
					* offsets and jumps...
					*/
					int InterlacedOffset[] = { 0, 4, 2, 1 };
					int InterlacedJumps[] = { 8, 8, 4, 2 };
					/* Need to perform 4 passes on the image */
					for (i = 0; i < 4; i++) {
						for (j = InterlacedOffset[i]; j < sp->ImageDesc.Height; j += InterlacedJumps[i]) {
							if (DGifGetLine(GifFile, sp->RasterBits + j * sp->ImageDesc.Width, sp->ImageDesc.Width) == GIF_ERROR)
								goto error;
						}
					}
				}
				else if (DGifGetLine(GifFile, sp->RasterBits, ImageSize) == GIF_ERROR) {
					goto error;
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
				if (DGifGetExtension(GifFile, &Function, &ExtData) == GIF_ERROR) {
					return (GIF_ERROR);
				}

				while (ExtData != NULL) {
					/* Create an extension block with our data */
					if (AddExtensionBlockMono(&temp_save, Function, ExtData[0], &ExtData[1]) == GIF_ERROR) {
						goto error;
					}

					if (DGifGetExtensionNext(GifFile, &ExtData) == GIF_ERROR) {
						goto error;
					}

					/* Graphics control blocks cannot contain any sub blocks. */
					if (Function == 0xF9 && ExtData) {
						goto error;
					}
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

	/* The gif file must contain at least one image block. */
	if (GifFile->ImageCount == 0)
		goto error;

	/* In case the Gif has an extension block without an associated
	* image we return it in TrailingExtensions, if provided */
	if ((TrailingExtensions != NULL) && (temp_save.ExtensionBlocks != NULL)) {
		*TrailingExtensions = temp_save;
	} else {
		FreeExtensionMono (&temp_save);
	}

	return (GIF_OK);

error:
	FreeExtensionMono (&temp_save);
	return GIF_ERROR;
}

static GpStatus 
gdip_load_gif_image (void *stream, GpImage **image, BOOL from_file)
{
	GpStatus status;
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
	ActiveBitmapData	*bitmap_data;
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

	status = Ok;
	disposal = 0;
	last_disposal = 0;
	loop_value = 0;
	global_palette = NULL;
	result = NULL;
	loop_counter = FALSE;

	if (from_file) {
#if GIFLIB_MAJOR >= 5
		gif = DGifOpen(stream, &gdip_gif_fileinputfunc, NULL);
#else
		gif = DGifOpen(stream, &gdip_gif_fileinputfunc);
#endif
	} else {
#if GIFLIB_MAJOR >= 5
		gif = DGifOpen (stream, &gdip_gif_inputfunc, NULL);
#else
		gif = DGifOpen (stream, &gdip_gif_inputfunc);
#endif
	}
	
	if (gif == NULL) {
		status = OutOfMemory;
		goto error;
	}

	/* Read the image */
	if (DGifSlurpMono (gif, &global_extensions) != GIF_OK) {
		status = OutOfMemory;
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
	if (!result) {
		status = OutOfMemory;
		goto error;
	}

	result->type = ImageTypeBitmap;
	frame = gdip_frame_add(result, dimension);
	if (!frame) {
		status = OutOfMemory;
		goto error;
	}

	/* Copy the palette over, if there is one */
	if (gif->SColorMap != NULL) {
		global_palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * gif->SColorMap->ColorCount);
		if (!global_palette) {
			status = OutOfMemory;
			goto error;
		}
		
		global_palette->Flags = 0;
		global_palette->Count = gif->SColorMap->ColorCount;
		for (i = 0; i < gif->SColorMap->ColorCount; i++) {
			global_palette->Entries[i] = MAKE_ARGB_ARGB(0xff, gif->SColorMap->Colors[i].Red, gif->SColorMap->Colors[i].Green, gif->SColorMap->Colors[i].Blue);
		}
	} else {
		/* Assume a grayscale image for the global palette. Individual images might still have a different one. */
		global_palette = GdipAlloc (sizeof(ColorPalette) + 256 * sizeof(ARGB));
		if (!global_palette) {
			status = OutOfMemory;
			goto error;
		}

		global_palette->Flags = PaletteFlagsGrayScale;
		global_palette->Count = 256;
		for (i=0; i < 256; i++) {
			global_palette->Entries[i] = MAKE_ARGB_RGB(i, i, i);
		}
	}

	result->cairo_format = CAIRO_FORMAT_A8;

	/* create our bitmaps */
	for (i = 0; i < num_of_images; i++) {

		/* Add ActiveBitmapData to our frame */
		bitmap_data = gdip_frame_add_bitmapdata(frame);
		if (bitmap_data == NULL) {
			status = OutOfMemory;
			goto error;
		}

		si = gif->SavedImages[i];
		img_desc = &si.ImageDesc;
		if (img_desc->Top < 0 || img_desc->Height < 0 ||
		    img_desc->Left < 0 || img_desc->Width < 0 ||
		    (img_desc->Width + img_desc->Left) > screen_width ||
		    (img_desc->Height + img_desc->Top) > screen_height) {
			status = OutOfMemory;
			goto error;
		}

		for (l = 0; l < global_extensions.ExtensionBlockCount; l++) {
			ExtensionBlock eb = global_extensions.ExtensionBlocks[l];
			if (eb.Function == COMMENT_EXT_FUNC_CODE) {
				int	index;

				if (gdip_bitmapdata_property_find_id(bitmap_data, PropertyTagExifUserComment, &index) != Ok) {
					BYTE *bytes = (BYTE*) GdipAlloc (eb.ByteCount + 1);
					if (bytes == NULL) {
						status = OutOfMemory;
						goto error;
					}

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
					if (!text) {
						status = OutOfMemory;
						goto error;
					}

					memcpy(text, eb.Bytes, eb.ByteCount);
					text[eb.ByteCount] = '\0';
					gdip_bitmapdata_property_add_ASCII(bitmap_data, PropertyTagExifUserComment, text);
					GdipFree(text);

					break;
				}
			}
		}

		/* copy the local color map if there is one, otherwise we duplicate the global one */
		if (img_desc->ColorMap != NULL) {
			ColorMapObject	*local_palette_obj;

			/* TODO: what do we do with combined gif + local palettes? */
			local_palette_obj = img_desc->ColorMap;
	
			bitmap_data->palette = GdipAlloc (sizeof(ColorPalette) + sizeof(ARGB) * local_palette_obj->ColorCount);
			if (!bitmap_data->palette) {
				status = OutOfMemory;
				goto error;
			}

			bitmap_data->palette->Flags = 0;
			bitmap_data->palette->Count = local_palette_obj->ColorCount;
			for (l = 0; l < local_palette_obj->ColorCount; l++) {
				bitmap_data->palette->Entries[l] = MAKE_ARGB_ARGB(0xff, local_palette_obj->Colors[l].Red,
										local_palette_obj->Colors[l].Green,
										local_palette_obj->Colors[l].Blue);
			}
		} else {
			bitmap_data->palette = gdip_palette_clone(global_palette);
			if (!bitmap_data->palette) {
				status = OutOfMemory;
				goto error;
			}
		}

		if (bitmap_data->transparent < 0) {
			BYTE *v;

			bitmap_data->palette->Flags |= PaletteFlagsHasAlpha;
			transparent_index = (bitmap_data->transparent + 1) * -1;

			if (transparent_index >= bitmap_data->palette->Count) {
				status = OutOfMemory;
				goto error;
			}

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

		unsigned long long int size = (unsigned long long int)bitmap_data->stride * bitmap_data->height;
		if (size > G_MAXINT32) {
			status = OutOfMemory;
			goto error;
		}

		bitmap_data->scan0 = GdipAlloc (size);
		if (!bitmap_data->scan0) {
			status = OutOfMemory;
			goto error;
		}

		bitmap_data->reserved = GBD_OWN_SCAN0;
		bitmap_data->image_flags = ImageFlagsReadOnly | ImageFlagsHasRealPixelSize | ImageFlagsHasRealDPI | ImageFlagsColorSpaceRGB;
		if (bitmap_data->transparent < 0)
			bitmap_data->image_flags |= ImageFlagsHasAlpha;

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
					ActiveBitmapData *last_bitmap;

					last_bitmap = &frame->bitmap [i - 1];
					if (l == img_desc->Top) {
						/* Copy the previous bitmap as the base for this one */
						/* TODO: This will be wrong if each image has a different palette */
						/* There's a comment up there too */
						memcpy (bitmap_data->scan0, last_bitmap->scan0,
							size);
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
#if (GIFLIB_MAJOR > 5) || ((GIFLIB_MAJOR == 5) && (GIFLIB_MINOR >= 1))
	DGifCloseFile (gif, NULL);
#else
	DGifCloseFile (gif);
#endif
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
		FreeExtensionMono (&global_extensions);
#if (GIFLIB_MAJOR > 5) || ((GIFLIB_MAJOR == 5) && (GIFLIB_MINOR >= 1))
		DGifCloseFile (gif, NULL);
#else
		DGifCloseFile (gif);
#endif
	}

	*image = NULL;
	return status;
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
	GpStatus status;
	GifFileType	*fp;
	int		i, x, y;
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
	ActiveBitmapData	*bitmap_data;
	unsigned long long int		pixbuf_size;

	if (!stream) {
		return InvalidParameter;
	}

	if (from_file) {
#if GIFLIB_MAJOR >= 5
		fp = EGifOpenFileName (stream, 0, NULL);
#else
		fp = EGifOpenFileName (stream, 0);
#endif
	} else {
#if GIFLIB_MAJOR >= 5
		fp = EGifOpen (stream, gdip_gif_outputfunc, NULL);
#else
		fp = EGifOpen (stream, gdip_gif_outputfunc);
#endif
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

			pixbuf_size = (unsigned long long int)bitmap_data->width * bitmap_data->height * sizeof(GifByteType);

			if (pixbuf_size > G_MAXINT32) {
				status = OutOfMemory;
				goto error;
			}

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
					status = GenericError;
					goto error; 
				}

#if GIFLIB_MAJOR >= 5
				cmap = GifMakeMapObject(cmap_size, 0);
#else
				cmap = MakeMapObject(cmap_size, 0);
#endif
				pixbuf = GdipAlloc(pixbuf_size);
				if (pixbuf == NULL) {
					status = OutOfMemory;
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

							switch (bitmap_data->width & 7) {
								case 7: pixbuf[6] = ((w & 0x02) != 0); /* fall through */
								case 6: pixbuf[5] = ((w & 0x04) != 0); /* fall through */
								case 5: pixbuf[4] = ((w & 0x08) != 0); /* fall through */
								case 4: pixbuf[3] = ((w & 0x10) != 0); /* fall through */
								case 3: pixbuf[2] = ((w & 0x20) != 0); /* fall through */
								case 2: pixbuf[1] = ((w & 0x40) != 0); /* fall through */
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
#if GIFLIB_MAJOR >= 5
				cmap  = GifMakeMapObject (cmap_size, 0);
#else
				cmap  = MakeMapObject (cmap_size, 0);
#endif
				red = GdipAlloc(pixbuf_size);
				green = GdipAlloc(pixbuf_size);
				blue = GdipAlloc(pixbuf_size);
				pixbuf = GdipAlloc(pixbuf_size);
				if ((red == NULL) || (green == NULL) || (blue == NULL) || (pixbuf == NULL)) {
					status = OutOfMemory;
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
				if (LibgdiplusGifQuantizeBuffer(bitmap_data->width, bitmap_data->height, &cmap_size, red,  green, blue, pixbuf, cmap->Colors) == GIF_ERROR) {
					status = GenericError;
					goto error;
				}
			}

#if GIFLIB_MAJOR >= 5
			cmap->BitsPerPixel = GifBitSize (cmap_size);
#else
			cmap->BitsPerPixel = BitSize (cmap_size);
#endif
			cmap->ColorCount = 1 << cmap->BitsPerPixel;

			if ((frame == 0) && (k == 0)) {
				/* First Image defines the global colormap */
				if (EGifPutScreenDesc (fp, bitmap_data->width, bitmap_data->height, cmap->BitsPerPixel, 0, cmap) == GIF_ERROR) {
					status = GenericError;
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
#if GIFLIB_MAJOR >= 5
						EGifPutExtensionLeader(fp, APPLICATION_EXT_FUNC_CODE);
						EGifPutExtensionBlock(fp, 11, "NETSCAPE2.0");
						EGifPutExtensionBlock(fp, 3, Buffer);
						EGifPutExtensionTrailer(fp);
#else
						EGifPutExtensionFirst(fp, APPLICATION_EXT_FUNC_CODE, 11, "NETSCAPE2.0"); 
						EGifPutExtensionLast(fp, APPLICATION_EXT_FUNC_CODE, 3, Buffer); 
#endif
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
				status = GenericError;
				goto error;
			}

			for (i = 0;  i < bitmap_data->height;  ++i) {
				if (EGifPutLine (fp, pixbuf, bitmap_data->width) == GIF_ERROR) {
					status = GenericError;
					goto error;
				}
				pixbuf += bitmap_data->width;
			}

#if GIFLIB_MAJOR >= 5
			GifFreeMapObject (cmap);
#else
			FreeMapObject (cmap);
#endif
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

#if (GIFLIB_MAJOR > 5) || ((GIFLIB_MAJOR == 5) && (GIFLIB_MINOR >= 1))
	EGifCloseFile (fp, NULL);
#else
	EGifCloseFile (fp);
#endif
	
	return Ok;

error:
	if (cmap != NULL) {
#if GIFLIB_MAJOR >= 5
		GifFreeMapObject (cmap);
#else
		FreeMapObject (cmap);
#endif
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

	return status;
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
gdip_save_gif_image_to_stream_delegate (PutBytesDelegate putBytesFunc, GpImage *image, GDIPCONST EncoderParameters *params)
{
	return UnknownImageFormat;
}

GpStatus
gdip_load_gif_image_from_stream_delegate (GetBytesDelegate getBytesFunc, SeekDelegate seekFunc, GpImage **image)
{
	*image = NULL;
	return UnknownImageFormat;
}

#endif

GpStatus
gdip_fill_encoder_parameter_list_gif (EncoderParameters *buffer, UINT size)
{
	GifEncoderParameters *gifBuffer = (GifEncoderParameters *) buffer;

	if (!buffer || size != sizeof (GifEncoderParameters))
		return InvalidParameter;
	
	gifBuffer->count = 2;

	gifBuffer->imageItems.Guid = GdipEncoderImageItems;
	gifBuffer->imageItems.NumberOfValues = 0;
	gifBuffer->imageItems.Type = 9; // Undocumented type.
	gifBuffer->imageItems.Value = NULL;

	gifBuffer->saveFlag.Guid = GdipEncoderSaveFlag;
	gifBuffer->saveFlag.NumberOfValues = 1;
	gifBuffer->saveFlag.Type = EncoderParameterValueTypeLong;
	gifBuffer->saveFlagValue = EncoderValueMultiFrame;
	gifBuffer->saveFlag.Value = &gifBuffer->saveFlagValue;

	return Ok;
}
