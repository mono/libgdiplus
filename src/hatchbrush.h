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

GpStatus GdipCreateHatchBrush (GpHatchStyle hatchstyle, ARGB forecolor, ARGB backcolor, GpHatch **brush);
GpStatus GdipGetHatchStyle (GpHatch *brush, GpHatchStyle *hatchstyle);
GpStatus GdipGetHatchForegroundColor (GpHatch *brush, ARGB *forecolor);
GpStatus GdipGetHatchBackgroundColor (GpHatch *brush, ARGB *backcolor);

#endif
