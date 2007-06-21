/*
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
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *
 * Copyright (C) 2003-2007 Novell, Inc (http://www.novell.com)
 */

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "codecs.h"

/*
 * libgdiplus-specific types that needs to be exported to replace the COM-based IStream stuff GDI+ provides
 */
typedef int (*GetHeaderDelegate) (BYTE*, int);
typedef int (*GetBytesDelegate) (BYTE*, int, BOOL);
typedef long (*SeekDelegate) (int, int);
typedef int (*PutBytesDelegate) (BYTE*, int);
typedef void (*CloseDelegate) ();
typedef long (*SizeDelegate) ();

GpStatus GdipLoadImageFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, GpImage **image);

GpStatus GdipSaveImageToDelegate_linux (GpImage *image, GetBytesDelegate getBytesFunc, PutBytesDelegate putBytesFunc,
	SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, GDIPCONST CLSID *encoderCLSID,
	GDIPCONST EncoderParameters *params);


/* GDI+ exported Image functions */
GpStatus GdipLoadImageFromStream (void /*IStream*/ *stream, GpImage **image);
GpStatus GdipLoadImageFromFile (GDIPCONST WCHAR *file, GpImage **image); 
GpStatus GdipLoadImageFromFileICM (GDIPCONST WCHAR* filename, GpImage **image);

GpStatus GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *clsidEncoder, GDIPCONST EncoderParameters *encoderParams); 
GpStatus GdipSaveImageToStream (GpImage *image, void /*IStream*/ *stream, GDIPCONST CLSID *clsidEncoder, GDIPCONST EncoderParameters *encoderParams);
GpStatus GdipSaveAdd (GpImage *image, GDIPCONST EncoderParameters* encoderParams);
GpStatus GdipSaveAddImage (GpImage *image, GpImage *newImage, GDIPCONST EncoderParameters *params);

GpStatus GdipGetImageBounds (GpImage *image, GpRectF *srcRect, GpUnit *srcUnit);
GpStatus GdipGetImageDimension (GpImage *image, REAL *width, REAL *height);
GpStatus GdipGetImageType (GpImage *image, ImageType *type);
GpStatus GdipGetImageWidth (GpImage *image, UINT *width);
GpStatus GdipGetImageHeight (GpImage *image, UINT *heigth);
GpStatus GdipGetImageHorizontalResolution (GpImage *image, REAL *resolution);
GpStatus GdipGetImageVerticalResolution (GpImage *image, REAL *resolution);
GpStatus GdipGetImageFlags (GpImage *image, UINT *flags);
GpStatus GdipGetImageRawFormat (GpImage *image, GUID *format);
GpStatus GdipGetImagePixelFormat (GpImage *image, PixelFormat *format);
GpStatus GdipGetImageThumbnail (GpImage *image, UINT thumbWidth, UINT thumbHeight, GpImage **thumbImage, GetThumbnailImageAbort callback, VOID* callBackData);
GpStatus GetEncoderParameterListSize (GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT *size);
GpStatus GetEncoderParameterList (GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT size, EncoderParameters *buffer);
GpStatus GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count);
GpStatus GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count);
GpStatus GdipImageGetFrameCount (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count); 
GpStatus GdipImageSelectActiveFrame (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT frameIndex);
GpStatus GdipImageRotateFlip (GpImage *image, RotateFlipType rfType);
GpStatus GdipGetImageGraphicsContext (GpImage *image, GpGraphics **graphics);
GpStatus GdipGetImagePalette (GpImage *image, ColorPalette *palette, INT size);
GpStatus GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette);
GpStatus GdipGetImagePaletteSize (GpImage *image, INT* size);
GpStatus GdipGetPropertyCount (GpImage *image, UINT *numOfProperty);
GpStatus GdipGetPropertyIdList (GpImage *image, UINT numOfProperty, PROPID *list);
GpStatus GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size);
GpStatus GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem *buffer);
GpStatus GdipGetPropertySize (GpImage *image, UINT *totalBufferSize, UINT *numProperties);
GpStatus GdipRemoveProperyItem (GpImage *image, PROPID propID);
GpStatus GdipSetProperyItem (GpImage *image, GDIPCONST PropertyItem *item);
GpStatus GdipCloneImage(GpImage *image, GpImage **cloneImage);
GpStatus GdipDisposeImage (GpImage *image);

/* missing API
	GdipLoadImageFromStreamICM
	GdipImageForceValidation
 */
#endif
