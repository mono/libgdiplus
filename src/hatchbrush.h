/*
 * hatchbrush.h
 *
 * Author:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) 2004, 2007 Novell, Inc. http://www.novell.com
 */

#ifndef __HATCHBRUSH_H__
#define __HATCHBRUSH_H__

#include "brush.h"

GpStatus WINGDIPAPI GdipCreateHatchBrush (GpHatchStyle hatchstyle, ARGB forecol, ARGB backcolor, GpHatch **brush);
GpStatus WINGDIPAPI GdipGetHatchStyle (GpHatch *brush, GpHatchStyle *hatchstyle);
GpStatus WINGDIPAPI GdipGetHatchForegroundColor (GpHatch *brush, ARGB *forecol);
GpStatus WINGDIPAPI GdipGetHatchBackgroundColor (GpHatch *brush, ARGB *backcol);

#endif
