/*
 * gdipImage.h : Contains declarations for manipulating images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004.
 */

#ifndef _GDIPIMAGE_H
#define _GDIPIMAGE_H


#include "gdip.h"
#include "gdip_win32.h"
#include <stdio.h>
#include "bmpcodec.h"
#include "pngcodec.h"
#include "jpegcodec.h"
#include "gifcodec.h"
#include "tiffcodec.h"

/*typedef bool BOOL;
typedef BOOL (CALLBACK * ImageAbort) (VOID *);
typedef ImageAbort GetThumbnailImageAbort; */

static const CLSID gdip_image_frameDimension_page_guid = {0x7462dc86U, 0x6180U, 0x4c7eU, {0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83}};								              
static const CLSID gdip_image_frameDimension_time_guid = {0x6aedbd6dU, 0x3fb5U, 0x418aU, {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};										
static const CLSID gdip_image_frameDimension_resolution_guid = {0x84236f7bU, 0x3bd3U, 0x428fU, {0x8d, 0xab, 0x4e, 0xa1, 0x43, 0x9c, 0xa3, 0x15}};


/*
 * Enumerations
 *
 */
typedef enum {
	PaletteFlagsHasAlpha = 0x0001,
	PaletteFlagsGrayScale = 0x0002,
	PaletteFlagsHalftone = 0x0004
} PaletteFlags;

typedef enum {
	RotateNoneFlipNone	= 0,
	Rotate90FlipNone	= 1,
	Rotate180FlipNone	= 2,
	Rotate270FlipNone	= 3,
	
	RotateNoneFlipX		= 4,
	Rotate90FlipX		= 5,
	Rotate180FlipX		= 6,
	Rotate270FlipX		= 7,
	
	RotateNoneFlipY		= Rotate180FlipX,
	Rotate90FlipY		= Rotate270FlipX,
	Rotate180FlipY		= RotateNoneFlipX,
	Rotate270FlipY		= Rotate90FlipX,

	RotateNoneFlipXY	= Rotate180FlipNone,
	Rotate90FlipXY		= Rotate270FlipNone,
	Rotate180FlipXY		= RotateNoneFlipNone,
	Rotate270FlipXY		= Rotate90FlipNone
} RotateFlipType; 

/*
 * Strutures
 *
 */


/*
 * Functions
 * 
 */

/* Image */
/* GpStatus GdipLoadImageFromStream (IStream *stream, GpImage **image); */
/* GpStatus GdipSaveImageToStream (GpImage *image, IStream *stream, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params); */
GpStatus GdipLoadImageFromFile (GDIPCONST WCHAR *file, GpImage **image); 
GpStatus GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params); 
/* GpStatus GdipSaveAddImage (GpImage *image, GpImage *imageNew, GDIPCONST EncoderParameters *params); */
GpStatus GdipGetImageBounds (GpImage *image, GpRectF *rect, GpUnit *unit);
GpStatus GdipGetImageDimension (GpImage *image, float *width, float *height);
GpStatus GdipGetImageType (GpImage *image, ImageType *type);
GpStatus GdipGetImageWidth (GpImage *image, UINT *width);
GpStatus GdipGetImageHeight (GpImage *image, UINT *heigth);
GpStatus GdipGetImageHorizontalResolution (GpImage *image, float *resolution);
GpStatus GdipGetImageVerticalResolution (GpImage *image, float *resolution);
GpStatus GdipGetImageFlags (GpImage *image, UINT *flags);
/* GpStatus GdipGetImageRawFormat (GpImage *image, GUID *format); */
GpStatus GdipGetImagePixelFormat (GpImage *image, PixelFormat *format);
/*GpStatus GdipGetImageThumbnail (GpImage *image, UINT width, UINT height, GpImage **thumbImage, GetThumbnailImageAbort callback, VOID* callBackData);*/
/* GpStatus GetEncoderParameterListSize (GpImage *image, GDIPCONST CLSID *encoderCLSID, UINT *size); */
/* GpStatus GetEncoderParameterList (GpImage *image, GDIPCONST CLSID *encoderCLSID, UINT size, EncoderParameters *buffer); */
GpStatus GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count);
/* GpStatus GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count); */
GpStatus GdipImageGetFrameCount (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count); 
GpStatus GdipImageSelectActiveFrame (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT index);
GpStatus GdipImageRotateFlip (GpImage *image, RotateFlipType type);
GpStatus GdipGetImagePalette (GpImage *image, ColorPalette *palette, int size);
GpStatus GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette);
GpStatus GdipGetImagePaletteSize (GpImage *image, int* size);
GpStatus GdipGetPropertyCount (GpImage *image, UINT *propertyNumber);
GpStatus GdipGetPropertyIdList (GpImage *image, UINT propertyNumber, PROPID *list);
GpStatus GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size);
GpStatus GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem buffer);
GpStatus GdipGetPropertySize (GpImage *image, UINT *bufferSize, UINT *propertyNumbers);
GpStatus GdipRemoveProperyItem (GpImage *image, PROPID propID);
GpStatus GdipSetProperyItem (GpImage *image, GDIPCONST PropertyItem *item);
GpStatus GdipCloneImage(GpImage *image, GpImage **cloneImage);

GpStatus GdipLoadImageFromDelegate_linux (GetBytesDelegate getBytesFunc,
                                          SeekDelegate seekFunc,
                                          GpImage **image);
GpStatus GdipSaveImageToDelegate_linux (GpImage *image,
                                        PutBytesDelegate putBytesFunc,
                                        GDIPCONST CLSID *encoderCLSID,
                                        GDIPCONST EncoderParameters *params);
					
GpStatus GdipDisposeImage (GpImage *image);


ImageFormat get_image_format (char *sig_read, size_t size_read);

int gdip_getpixel_formatsize(PixelFormat pixfmt);
int gdip_get_pixel_format_depth(PixelFormat pixfmt);
int gdip_get_pixel_format_components(PixelFormat pixfmt);
ImageFormat gdip_image_format_for_clsid (GDIPCONST CLSID *encoderCLSID);



#endif /* _GDIPIMAGE_H */
