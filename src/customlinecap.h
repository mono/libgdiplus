/*
 * customlinecap.h
 *
 * Authors:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004, 2007 Novell, Inc (http://www.novell.com)
 */

#ifndef __CUSTOMLINECAP_H__
#define __CUSTOMLINECAP_H__

/* CustomLineCap functions */
GpStatus GdipCreateCustomLineCap (GpPath *fillPath, GpPath *strokePath, GpLineCap baseCap, float baseInset, GpCustomLineCap **customCap);
GpStatus GdipDeleteCustomLineCap (GpCustomLineCap *customCap);
GpStatus GdipCloneCustomLineCap (GpCustomLineCap *customCap, GpCustomLineCap **clonedCap);
GpStatus GdipSetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap startCap, GpLineCap endCap);
GpStatus GdipGetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap *startCap, GpLineCap *endCap);
GpStatus GdipSetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin lineJoin);
GpStatus GdipGetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin *lineJoin);
GpStatus GdipSetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap baseCap);
GpStatus GdipGetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap *baseCap);
GpStatus GdipSetCustomLineCapBaseInset (GpCustomLineCap *customCap, float inset);
GpStatus GdipGetCustomLineCapBaseInset (GpCustomLineCap *customCap, float *inset);
GpStatus GdipSetCustomLineCapWidthScale (GpCustomLineCap *customCap, float widthScale);
GpStatus GdipGetCustomLineCapWidthScale (GpCustomLineCap *customCap, float *widthScale);

#endif
