/*
 * brush.h
 *
 * Authors:
 *      Ravindra (rkumar@novell.com)
 *
 * Copyright (C) Novell, Inc. 2004.
 */

#ifndef _BRUSH_H
#define _BRUSH_H

/* Enums */

typedef enum _Status {
    Ok = 0,
    GenericError = 1,
    InvalidParameter = 2,
    OutOfMemory = 3,
    ObjectBusy = 4,
    InsufficientBuffer = 5,
    NotImplemented = 6,
    Win32Error = 7,
    WrongState = 8,
    Aborted = 9,
    FileNotFound = 10,
    ValueOverflow = 11,
    AccessDenied = 12,
    UnknownImageFormat = 13,
    FontFamilyNotFound = 14,
    FontStyleNotFound = 15,
    NotTrueTypeFont = 16,
    UnsupportedGdiplusVersion = 17,
    GdiplusNotInitialized = 18,
    PropertyNotFound = 19,
    PropertyNotSupported = 20
} GpStatus;

typedef enum {
        BrushTypeSolidColor = 0,     /* Solid Brush */
        BrushTypeHatchFill = 1,      /* Hatch Brush */
        BrushTypeTextureFill = 2,    /* Texture Brush */
        BrushTypePathGradient = 3,   /* PathGradient Brush */
        BrushTypeLinearGradient = 4  /* LinearGradient Brush */
} GpBrushType, BrushType;

/* Structures */

typedef struct _Graphics GpGraphics;

typedef struct _Brush GpBrush;

typedef struct _BrushClass {

	/* Brush virtual functions */
        GpBrushType type;
	void (*setup) (GpGraphics *graphics, GpBrush *brush);
	void (*clone_brush) (GpBrush *brush, GpBrush **clonedBrush);
	void (*destroy) (GpBrush *brush);
} BrushClass;

typedef struct _Brush {
	BrushClass *vtable;
} Brush;

void gdip_brush_init (GpBrush *brush, BrushClass* vtable);
void gdip_brush_setup (GpGraphics *graphics, GpBrush *brush);
void clone_brush (GpBrush *brush, GpBrush **clonedBrush);
void gdip_brush_destroy (GpBrush *brush);

GpStatus GdipCloneBrush (GpBrush *brush, GpBrush **clonedBrush); 
GpStatus GdipDeleteBrush (GpBrush *brush);
GpStatus GdipGetBrushType (GpBrush *brush, GpBrushType *type);

#endif /* _BRUSH_H */
