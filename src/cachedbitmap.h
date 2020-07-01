// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#ifndef __CACHEDBITMAP_H__
#define __CACHEDBITMAP_H__

GpStatus WINGDIPAPI GdipCreateCachedBitmap (GpBitmap *bitmap, GpGraphics *graphics, GpCachedBitmap **cachedBitmap);

GpStatus WINGDIPAPI GdipDeleteCachedBitmap (GpCachedBitmap *cachedBitmap);

#endif
