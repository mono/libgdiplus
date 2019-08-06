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
GpStatus WINGDIPAPI GdipCreateCustomLineCap (GpPath *fillPath, GpPath *strokePath, GpLineCap baseCap, REAL baseInset, GpCustomLineCap **customCap);
GpStatus WINGDIPAPI GdipDeleteCustomLineCap (GpCustomLineCap *customCap);
GpStatus WINGDIPAPI GdipCloneCustomLineCap (GpCustomLineCap *customCap, GpCustomLineCap **clonedCap);
GpStatus WINGDIPAPI GdipGetCustomLineCapType (GpCustomLineCap *customCap, CustomLineCapType *capType);
GpStatus WINGDIPAPI GdipSetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap startCap, GpLineCap endCap);
GpStatus WINGDIPAPI GdipGetCustomLineCapStrokeCaps (GpCustomLineCap *customCap, GpLineCap *startCap, GpLineCap *endCap);
GpStatus WINGDIPAPI GdipSetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin lineJoin);
GpStatus WINGDIPAPI GdipGetCustomLineCapStrokeJoin (GpCustomLineCap *customCap, GpLineJoin *lineJoin);
GpStatus WINGDIPAPI GdipSetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap baseCap);
GpStatus WINGDIPAPI GdipGetCustomLineCapBaseCap (GpCustomLineCap *customCap, GpLineCap *baseCap);
GpStatus WINGDIPAPI GdipSetCustomLineCapBaseInset (GpCustomLineCap *customCap, REAL inset);
GpStatus WINGDIPAPI GdipGetCustomLineCapBaseInset (GpCustomLineCap *customCap, REAL *inset);
GpStatus WINGDIPAPI GdipSetCustomLineCapWidthScale (GpCustomLineCap *customCap, REAL widthScale);
GpStatus WINGDIPAPI GdipGetCustomLineCapWidthScale (GpCustomLineCap *customCap, REAL *widthScale);

#endif
