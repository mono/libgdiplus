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

GpStatus WINGDIPAPI GdipLoadImageFromDelegate_linux (GetHeaderDelegate getHeaderFunc, GetBytesDelegate getBytesFunc,
	PutBytesDelegate putBytesFunc, SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, GpImage **image);

GpStatus WINGDIPAPI GdipSaveImageToDelegate_linux (GpImage *image, GetBytesDelegate getBytesFunc, PutBytesDelegate putBytesFunc,
	SeekDelegate seekFunc, CloseDelegate closeFunc, SizeDelegate sizeFunc, GDIPCONST CLSID *encoderCLSID,
	GDIPCONST EncoderParameters *params);


/* GDI+ exported Image functions */
GpStatus WINGDIPAPI GdipLoadImageFromStream (void /*IStream*/ *stream, GpImage **image);
GpStatus WINGDIPAPI GdipLoadImageFromFile (GDIPCONST WCHAR *file, GpImage **image); 
GpStatus WINGDIPAPI GdipLoadImageFromStreamICM (void /*IStream*/ *stream, GpImage **image);
GpStatus WINGDIPAPI GdipLoadImageFromFileICM (GDIPCONST WCHAR* filename, GpImage **image);

GpStatus WINGDIPAPI GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *clsidEncoder, GDIPCONST EncoderParameters *encoderParams); 
GpStatus WINGDIPAPI GdipSaveImageToStream (GpImage *image, void /*IStream*/ *stream, GDIPCONST CLSID *clsidEncoder, GDIPCONST EncoderParameters *encoderParams);
GpStatus WINGDIPAPI GdipSaveAdd (GpImage *image, GDIPCONST EncoderParameters* encoderParams);
GpStatus WINGDIPAPI GdipSaveAddImage (GpImage *image, GpImage *newImage, GDIPCONST EncoderParameters *params);

GpStatus WINGDIPAPI GdipGetImageBounds (GpImage *image, GpRectF *srcRect, GpUnit *srcUnit);
GpStatus WINGDIPAPI GdipGetImageDimension (GpImage *image, REAL *width, REAL *height);
GpStatus WINGDIPAPI GdipGetImageType (GpImage *image, ImageType *type);
GpStatus WINGDIPAPI GdipGetImageWidth (GpImage *image, UINT *width);
GpStatus WINGDIPAPI GdipGetImageHeight (GpImage *image, UINT *height);
GpStatus WINGDIPAPI GdipGetImageHorizontalResolution (GpImage *image, REAL *resolution);
GpStatus WINGDIPAPI GdipGetImageVerticalResolution (GpImage *image, REAL *resolution);
GpStatus WINGDIPAPI GdipGetImageFlags (GpImage *image, UINT *flags);
GpStatus WINGDIPAPI GdipGetImageRawFormat (GpImage *image, GUID *format);
GpStatus WINGDIPAPI GdipGetImagePixelFormat (GpImage *image, PixelFormat *format);
GpStatus WINGDIPAPI GdipGetImageThumbnail (GpImage *image, UINT thumbWidth, UINT thumbHeight, GpImage **thumbImage, GetThumbnailImageAbort callback, VOID* callBackData);
GpStatus WINGDIPAPI GdipGetEncoderParameterListSize (GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT *size);
GpStatus WINGDIPAPI GdipGetEncoderParameterList (GpImage *image, GDIPCONST CLSID *clsidEncoder, UINT size, EncoderParameters *buffer);
GpStatus WINGDIPAPI GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count);
GpStatus WINGDIPAPI GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count);
GpStatus WINGDIPAPI GdipImageGetFrameCount (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count); 
GpStatus WINGDIPAPI GdipImageSelectActiveFrame (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT frameIndex);
GpStatus WINGDIPAPI GdipImageRotateFlip (GpImage *image, RotateFlipType rfType);
GpStatus WINGDIPAPI GdipGetImageGraphicsContext (GpImage *image, GpGraphics **graphics);
GpStatus WINGDIPAPI GdipGetImagePalette (GpImage *image, ColorPalette *palette, INT size);
GpStatus WINGDIPAPI GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette);
GpStatus WINGDIPAPI GdipGetImagePaletteSize (GpImage *image, INT* size);
GpStatus WINGDIPAPI GdipGetPropertyCount (GpImage *image, UINT *numOfProperty);
GpStatus WINGDIPAPI GdipGetPropertyIdList (GpImage *image, UINT numOfProperty, PROPID *list);
GpStatus WINGDIPAPI GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size);
GpStatus WINGDIPAPI GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem *buffer);
GpStatus WINGDIPAPI GdipGetPropertySize (GpImage *image, UINT *totalBufferSize, UINT *numProperties);
GpStatus WINGDIPAPI GdipRemovePropertyItem (GpImage *image, PROPID propID);
GpStatus WINGDIPAPI GdipSetPropertyItem (GpImage *image, GDIPCONST PropertyItem *item);
GpStatus WINGDIPAPI GdipGetAllPropertyItems (GpImage *image, UINT totalBufferSize, UINT numProperties, PropertyItem *allItems);
GpStatus WINGDIPAPI GdipCloneImage(GpImage *image, GpImage **cloneImage);
GpStatus WINGDIPAPI GdipDisposeImage (GpImage *image);
GpStatus WINGDIPAPI GdipImageForceValidation (GpImage *image);

#endif
