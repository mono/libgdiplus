/*
 * gdip.h
 *
 * Authors:
 *      Alexandre Pigolkine (pigolkine@gmx.de)
 *      Duncan Mak (duncan@ximian.com)
 *      Miguel de Icaza (miguel@ximian.com)
 *      Ravindra (rkumar@novell.com)
 *  	Sanjay Gupta (gsanjay@novell.com)
 *	Vladimir Vukicevic (vladimir@pobox.com)
 *
 * Copyright (C) Novell, Inc. 2003-2004. http://www.novell.com
 */

#ifndef _GDIP_H
#define _GDIP_H

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include <cairo.h>
#ifndef CAIRO_HAS_XLIB_SURFACE
#include <cairo-xlib.h>
#endif

/* mono/io-layer/uglify.h also has these typedefs.
 * To avoid a dependency on mono we have copied all
 * the required stuff here. We don't include our defs
 * if uglify.h is included somehow.
 */
#ifndef _WAPI_UGLIFY_H_		/* to avoid conflict with uglify.h */

typedef const gunichar2 *LPCTSTR;
typedef gunichar2 *LPTSTR;
typedef guint8 BYTE;
typedef guint8 *LPBYTE;
typedef guint16 WORD;
typedef guint32 DWORD;
typedef gpointer PVOID;
typedef gpointer LPVOID;
typedef gboolean BOOL;
typedef guint32 *LPDWORD;
typedef gint32 LONG;
typedef guint32 ULONG;
typedef gint32 *PLONG;
typedef guint64 LONGLONG;
typedef gunichar2 TCHAR;
typedef size_t SIZE_T;

typedef gpointer HANDLE;
typedef gpointer *LPHANDLE;
typedef guint32 SOCKET;
typedef gpointer HMODULE;
#define CONST const
#define VOID void

#define IN
#define OUT
#define WINAPI

#endif 		/* to avoid conflict with uglify.h */

/* Cairo internal extructures and defines*/
#define DOUBLE_TO_26_6(d) ((FT_F26Dot6)((d) * 64.0))
#define DOUBLE_FROM_26_6(t) ((double)(t) / 64.0)
#define DOUBLE_TO_16_16(d) ((FT_Fixed)((d) * 65536.0))
#define DOUBLE_FROM_16_16(t) ((double)(t) / 65536.0)

struct cairo_matrix {
    double m[3][2];
};

struct cairo_font {
    int refcount;
    cairo_matrix_t matrix;
    //void *glyph_cache;
    const struct cairo_font_backend *backend;
};


typedef struct {
    cairo_font_t base;
    FT_Library ft_library;
    int owns_ft_library;
    FT_Face face;
    int owns_face;
    FcPattern *pattern;
} cairo_ft_font_t;

/*
 * Callbacks
 */
typedef int (*ImageAbort) (void *);
typedef ImageAbort DrawImageAbort;
typedef ImageAbort GetThumbnailImageAbort;

/*
 * Constants
 *
 */
#define PI 3.14159265358979323846
#define DEGTORAD PI / 180.0
#define LF_FACESIZE 32
#define GDIPCONST const
#define CURVE_MIN_TERMS 1
#define CURVE_MAX_TERMS 7

typedef unsigned char byte;
typedef int bool;
typedef unsigned short WCHAR; /* 16-bits unicode */
typedef unsigned int UINT;
typedef unsigned int ARGB;
typedef int PROPID;

#define MAKE_ARGB_RGB(r,g,b)  (((r&0xff) << 16) | ((g&0xff) << 8) | (b&0xff))
#define MAKE_ARGB_ARGB(a,r,g,b)  (((a&0xff) << 24) | ((r&0xff) << 16) | ((g&0xff) << 8) | (b&0xff))

#define ARGB_RED(argb) ((argb>>16)&0xff)
#define ARGB_GREEN(argb) ((argb>>8)&0xff)
#define ARGB_BLUE(argb) ((argb>>0)&0xff)
#define ARGB_ALPHA(argb) ((argb>>24)&0xff)

/* _N: normalize to 0.0 .. 1.0 */
#define ARGB_RED_N(argb) ((double)((argb>>16)&0xff)/255.0)
#define ARGB_GREEN_N(argb) ((double)((argb>>8)&0xff)/255.0)
#define ARGB_BLUE_N(argb) ((double)((argb>>0)&0xff)/255.0)
#define ARGB_ALPHA_N(argb) ((double)((argb>>24)&0xff)/255.0)

/*
 * Enums
 *
 */

typedef enum {
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

typedef enum  {
    MatrixOrderPrepend    = 0,
    MatrixOrderAppend     = 1
} MatrixOrder, GpMatrixOrder;

typedef enum {
    UnitWorld	 = 0,
    UnitDisplay	 = 1,
    UnitPixel	 = 2,
    UnitPoint	 = 3,
    UnitInch	 = 4,
    UnitDocument = 5,
    UnitMillimeter = 6
} GpUnit, Unit;

typedef enum {
        DigitSubstituteUser = 0,
        DigitSubstituteNone = 1,
        DigitSubstituteNational = 2,
        DigitSubstituteTraditional = 3
} DigitSubstitute;



/* The pixel format spec is:
 * [0-7 format index] [8-15 pixel size, bits] [16-23 flags] [24-31 reserved]
 * so,
 * (ID | (bpp << 8) | flags)
 */


typedef enum {
	PixelFormatAlpha = 0x00040000,     /* flag: format has alpha */
	PixelFormatCanonical = 0x00200000, /* flag: unknown */
	PixelFormatExtended = 0x00100000,  /* flag: 16 bits per channel (16bpp grayscale and 48/64bpp rgb) */
	PixelFormatGDI = 0x00020000,       /* flag: supported by GDI */
	PixelFormatIndexed = 0x00010000,   /* flag: is palette-indexed */
	PixelFormatPAlpha = 0x00080000,    /* flag: alpha is pre-multiplied */
        
	Format16bppArgb1555 = 0x00061007,
	Format16bppGrayScale = 0x00101004,
	Format16bppRgb555 = 0x00021005,
	Format16bppRgb565 = 0x00021006,
	Format1bppIndexed = 0x0030101,
	Format24bppRgb = 0x00021808,
	Format32bppArgb = 0x0026200a,
	Format32bppPArgb = 0x000e200b,
	Format32bppRgb = 0x00022009,
	Format48bppRgb = 0x0010300c,
	Format4bppIndexed = 0x00030402,
	Format64bppArgb = 0x0034400d,
	Format64bppPArgb = 0x001c400e,
	Format8bppIndexed = 0x00030803,

	PixelFormatUndefined = 0,
	PixelFormatDontCare = 0
} PixelFormat;

typedef enum {
	ImageLockModeRead = 1,
	ImageLockModeWrite = 2,
	ImageLockModeUserInputBuf = 4
} ImageLockMode;

typedef enum {
        FillModeAlternate,
        FillModeWinding
} GpFillMode;

typedef enum {
        BrushTypeSolidColor = 0,     /* Solid Brush */
        BrushTypeHatchFill = 1,      /* Hatch Brush */
        BrushTypeTextureFill = 2,    /* Texture Brush */
        BrushTypePathGradient = 3,   /* PathGradient Brush */
        BrushTypeLinearGradient = 4  /* LinearGradient Brush */
} GpBrushType, BrushType;

typedef enum {
	PenTypeSolidColor = BrushTypeSolidColor,
	PenTypeHatchFill = BrushTypeHatchFill,
	PenTypeTextureFill = BrushTypeTextureFill,
	PenTypePathGradient = BrushTypePathGradient,
	PenTypeLinearGradient = BrushTypeLinearGradient,
	PenTypeUnknown = -1
} GpPenType, PenType;

typedef enum {
        PenAlignmentCenter = 0,
        PenAlignmentInset = 1
} GpPenAlignment, PenAlignment;

typedef enum {
        DashStyleSolid = 0,      /* solid line */
        DashStyleDash = 1,       /* dashed line */
        DashStyleDot = 2,        /* dotted line */
        DashStyleDashDot = 3,    /* alt. dash-dot */
        DashStyleDashDotDot = 4, /* alt. dash-dot-dot */
        DashStyleCustom = 5      /* user-defined */
} GpDashStyle;

typedef enum {
	DashCapFlat = 0,
	DashCapRound = 2,
	DashCapTriangle = 3
} GpDashCap;

typedef enum {
        LineJoinMiter = 0,       /* sharp corner */
        LineJoinBevel = 1,       /* round corner */
        LineJoinRound = 2,       /* circular, smooth, circular arc */
        LineJoinMiterClipped = 3 /* miter, sharp or beveled corner */
} GpLineJoin;

typedef enum {
	LineCapFlat = 0,
	LineCapSquare = 1,
	LineCapRound = 2,
	LineCapTriangle = 3,
	LineCapNoAnchor = 0x10,
	LineCapSquareAnchor = 0x11,
	LineCapRoundAnchor = 0x12,
	LineCapDiamondAnchor = 0x13,
	LineCapArrowAnchor = 0x14,
	LineCapCustom = 0xff
} GpLineCap;

typedef enum {
	CustomLineCapTypeDefault = 0,
	CustomLineCapTypeAdjustableArrow = 1
} CustomLineCapType;

typedef enum  {
	imageUndefined,
	imageBitmap,
	imageMetafile
} ImageType;

typedef enum {
	BMP,
	TIF,
	GIF,
	PNG,
	JPEG,
	EXIF,
	WMF,
	EMF,
	ICON,
	INVALID
} ImageFormat;

typedef enum {
	gtUndefined,
	gtX11Drawable,
	gtMemoryBitmap
} GraphicsType;

typedef enum{
    FontStyleRegular    = 0,
    FontStyleBold       = 1,
    FontStyleItalic     = 2,
    FontStyleUnderline  = 4,
    FontStyleStrikeout  = 8
}  GpFontStyle;

typedef enum {
        PathPointTypeStart = 0x00,
        PathPointTypeLine = 0x01,
        PathPointTypeBezier = 0x03,
        PathPointTypePathTypeMask = 0x07,
        PathPointTypePathDashMode = 0x10,
        PathPointTypePathMarker = 0x20,
        PathPointTypeCloseSubpath = 0x80,
        PathPointTypeBezier3 = 0x30
} GpPathPointType, PathPointType;

typedef enum {
        WarpModePerspective = 0,
        WarpModeBilinear = 1
} GpWarpMode, WarpMode;

typedef enum {
	WrapModeTile = 0,
	WrapModeTileFlipX = 1,
	WrapModeTileFlipY = 2,
	WrapModeTileFlipXY = 3,
	WrapModeClamp = 4
} GpWrapMode, WrapMode;

typedef enum {
    StringFormatFlagsDirectionRightToLeft = 0x00000001,
    StringFormatFlagsDirectionVertical = 0x00000002,
    StringFormatFlagsNoFitBlackBox = 0x00000004,
    StringFormatFlagsDisplayFormatControl = 0x00000020,
    StringFormatFlagsNoFontFallback = 0x00000400,
    StringFormatFlagsMeasureTrailingSpaces = 0x00000800,
    StringFormatFlagsNoWrap = 0x00001000,
    StringFormatFlagsLineLimit = 0x00002000,
    StringFormatFlagsNoClip = 0x00004000
} StringFormatFlags;

typedef enum  {
    StringTrimmingNone = 0,
    StringTrimmingCharacter = 1,
    StringTrimmingWord = 2,
    StringTrimmingEllipsisCharacter = 3,
    StringTrimmingEllipsisWord = 4,
    StringTrimmingEllipsisPath = 5
} StringTrimming;

typedef enum {
    HotkeyPrefixNone = 0,
    HotkeyPrefixShow = 1,
    HotkeyPrefixHide = 2
} HotkeyPrefix;

typedef enum {
    StringAlignmentNear   = 0,
    StringAlignmentCenter = 1,
    StringAlignmentFar    = 2
} StringAlignment;


/* Flags and Support structure for MeasureOrDrawString */
#define STRING_DETAIL_TAB       (1<<0)
#define STRING_DETAIL_LF        (1<<1)
#define STRING_DETAIL_HOTKEY    (1<<2)
#define STRING_DETAIL_BREAK     (1<<3)
#define STRING_DETAIL_HIDDEN    (1<<4)
#define STRING_DETAIL_LINESTART (1<<5)

typedef struct {
	unsigned long	Flags;
	unsigned long	Linefeeds;
	float		TabWidth;
	float		PosX;		/* We call it X, even though it might become Y for vertical drawing */
	float		PosY;		/* We call it Y, even though it might become X for vertical drawing */
	float		Width;		/* Width of the character; height is defined in font structure */
	int		LineLen;	/* If LineStart how many chars is the line long? */
} GpStringDetailStruct;


typedef enum {
	ImageFlagsNone = 0,
	ImageFlagsScalable = 1,
	ImageFlagsHasAlpha = 2,
	ImageFlagsHasTranslucent = 4,
	ImageFlagsPartiallyScalable = 8,
	ImageFlagsColorSpaceRGB = 16,
	ImageFlagsColorSpaceCMYK = 32,
	ImageFlagsColorSpaceGRAY = 64,
	ImageFlagsColorSpaceYCBCR = 128,
	ImageFlagsColorSpaceYCCK = 256,
	ImageFlagsHasRealDPI = 4096,
	ImageFlagsHasRealPixelSize = 8192,
	ImageFlagsReadOnly = 65536,
	ImageFlagsCaching = 131072
} ImageFlags;

typedef enum 
{
    ColorAdjustTypeDefault,
    ColorAdjustTypeBitmap,
    ColorAdjustTypeBrush,
    ColorAdjustTypePen,
    ColorAdjustTypeText,
    ColorAdjustTypeCount,
    ColorAdjustTypeAny     
} ColorAdjustType;

typedef enum 
{
    CombineModeReplace          = 0,   
    CombineModeIntersect        = 1, 
    CombineModeUnion            = 2,     
    CombineModeXor              = 3,       
    CombineModeExclude          = 4,   
    CombineModeComplement       = 5  
    
} CombineMode;

typedef enum
{
    QualityModeInvalid = -1,
    QualityModeDefault = 0,
    QualityModeLow = 1,
    QualityModeHigh = 2
} QualityMode;

typedef enum
{
    InterpolationModeInvalid = QualityModeInvalid,
    InterpolationModeDefault = QualityModeDefault,
    InterpolationModeLowQuality = QualityModeLow,
    InterpolationModeHighQuality = QualityModeHigh,
    InterpolationModeBilinear,
    InterpolationModeBicubic,
    InterpolationModeNearestNeighbor,
    InterpolationModeHighQualityBilinear,
    InterpolationModeHighQualityBicubic
} InterpolationMode;

typedef enum
{
    TextRenderingHintSystemDefault = 0,
    TextRenderingHintSingleBitPerPixelGridFit,
    TextRenderingHintSingleBitPerPixel,
    TextRenderingHintAntiAliasGridFit,
    TextRenderingHintAntiAlias,
    TextRenderingHintClearTypeGridFit
} TextRenderingHint;

typedef enum
{
    PixelOffsetModeInvalid = QualityModeInvalid,
    PixelOffsetModeDefault = QualityModeDefault,
    PixelOffsetModeHighSpeed = QualityModeLow,
    PixelOffsetModeHighQuality = QualityModeHigh,
    PixelOffsetModeNone,
    PixelOffsetModeHalf
} PixelOffsetMode;

typedef enum
{
    SmoothingModeInvalid = QualityModeInvalid,
    SmoothingModeDefault = QualityModeDefault,
    SmoothingModeHighSpeed = QualityModeLow,
    SmoothingModeHighQuality = QualityModeHigh,
    SmoothingModeNone,
    SmoothingModeAntiAlias
} SmoothingMode;

typedef enum
{
    EncoderParameterValueTypeByte = 1,
    EncoderParameterValueTypeASCII = 2,
    EncoderParameterValueTypeShort = 3,
    EncoderParameterValueTypeLong = 4,
    EncoderParameterValueTypeRational = 5,
    EncoderParameterValueTypeLongRange = 6,
    EncoderParameterValueTypeUndefined = 7,
    EncoderParameterValueTypeRationalRange = 8
} EncoderParameterValueType;

typedef enum 
{
        BlockingDecode = 32,
        Builtin = 65536,
        Decoder = 2,
        Encoder = 1,
        SeekableEncode = 16,
        SupportBitmap = 4,
        SupportVector = 8,
        System = 131072,
        User = 262144
} ImageCodecFlags;

typedef enum {
        CoordinateSpaceWorld,
        CoordinateSpacePage,
        CoordinateSpaceDevice,
} GpCoordinateSpace;

typedef enum {
	FlushIntentionFlush=0,
	FlushIntentionSync=1
} GpFlushIntention;

typedef enum {
	CompositingModeSourceOver,
	CompositingModeSourceCopy
} GpCompositingMode;

typedef enum {
	CompositingQualityInvalid	= QualityModeInvalid,
	CompositingQualityDefault	= QualityModeDefault,
	CompositingQualityHighSpeed	= QualityModeLow,
	CompositingQualityHighQuality	= QualityModeHigh,
	CompositingQualityGammaCorrected,
	CompositingQualityAssumeLinear
} GpCompositingQuality;

typedef enum {
	ColorMatrixFlagsDefault		= 0,
	ColorMatrixFlagsSkipGrays	= 1,
	ColorMatrixFlagsAltGray		= 2
} GpColorMatrixFlags;

typedef struct {
	float m[5][5];
} GpColorMatrix;


/* FIXME - This should match the GdiPlus color object */
typedef struct {
	ARGB	Color;
} GpColor;

typedef struct {
	GpColor oldColor;
	GpColor newColor;
} GpColorMap;

typedef enum {
	ColorChannelFlagsC = 0,
	ColorChannelFlagsM,
	ColorChannelFlagsY,
	ColorChannelFlagsK,
	ColorChannelFlagsLast
} GpColorChannelFlags;

typedef struct {
	int	unused;
} GpGraphicsContainer;

/* private enum */

typedef enum {
        CURVE_OPEN,
        CURVE_CLOSE 
} _CurveType;

typedef enum {
        RegionTypeEmpty = 0,
        RegionTypeRect  = 1,
        RegionTypeRectF = 2,
        RegionTypePath  = 3
} RegionType;

/*
 * Structures
 *
 */

#define GBD_OWN_SCAN0	(1<<8)
#define GBD_READ_ONLY	(1<<9)
#define GBD_LOCKED	(1<<10)

typedef struct {    /* Keep in sync with BitmapData.cs */
	unsigned int Width;
	unsigned int Height;
	int          Stride;
	int          PixelFormat;
	void         *Scan0;
	unsigned int Reserved;
} GdipBitmapData, BitmapData;

typedef struct {
	int X, Y, Width, Height;
} GpRect, Rect;

typedef struct {
	float X, Y, Width, Height;
} GpRectF, RectF;

typedef struct {
        int X, Y;
} GpPoint, Point;

typedef struct {
        float X, Y;
} GpPointF, PointF;

typedef struct {
	cairo_t         *ct;
	cairo_matrix_t  *copy_of_ctm;
	void            *hdc;
	int             hdc_busy_count;
	void            *image;
	int             type; 
} GpGraphics;

typedef struct _CustomLineCap GpCustomLineCap;
typedef struct _AdjustableArrowCap GpAdjustableArrowCap;
typedef struct _Brush GpBrush;
typedef cairo_matrix_t GpMatrix;

typedef struct {
	int color;
        GpBrush *brush;
	float width;
        float miter_limit;
        GpLineJoin line_join;
        GpDashStyle dash_style;
	GpLineCap line_cap;  /* Cairo supports only same kind of end caps for both the ends. */
	int compound_count;
	float *compound_array;
        GpPenAlignment mode;
        float dash_offset;
        int dash_count;
	BOOL own_dash_array; /* flag to mark if pen maintains its own array or global array */
        float *dash_array;
        GpUnit unit; /* Always set to UnitWorld. */
        GpMatrix *matrix;
        BOOL changed; /* flag to mark if pen is changed and needs setup */
} GpPen;

typedef struct {
	cairo_matrix_t		*matrix;
} GpState;

typedef struct {
	UINT Flags;
	UINT Count;
	ARGB Entries[1];
} ColorPalette;

typedef struct {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    byte           Data4 [8];
} GUID, Guid, CLSID;

typedef struct {
    GUID    Guid;
    ULONG   NumberOfValues;
    EncoderParameterValueType   Type;
    VOID*   Value;
} EncoderParameter;

typedef struct {
    UINT Count;
    EncoderParameter Parameter[1];
} EncoderParameters;

typedef struct {
	PROPID	id;
	ULONG 	length;
	WORD 	type;
	VOID*	value;
} PropertyItem;

typedef struct {
	GUID frameDimension;
	int count;
} FrameDimensionInfo;

typedef struct {
	ImageType     	type;
	cairo_surface_t *surface;
	GpGraphics  	*graphics;		/* created by GdipGetImageGraphicsContext */
	int 		imageFlags;
	int 		height;
	int 		width;
	float 		horizontalResolution;
	ColorPalette 	*palette;
	int	 	pixFormat;
	PropertyItem 	*propItems;
	float 		verticalResolution;
	ImageFormat     format;
	int		frameDimensionCount;
	FrameDimensionInfo *frameDimensionList;
} GpImage;

typedef struct {
	GpImage	image;
        int             cairo_format;
	BitmapData	data;
	void *hBitmapDC;
	void *hInitialBitmap;
	void *hBitmap;
} GpBitmap;

typedef struct {
        GpFillMode fill_mode;
        int count;
        GByteArray *types;
        GArray *points;
} GpPath;

typedef struct {
        int Count;
        PointF *Points;
        byte *Types;
} GpPathData;

typedef struct {
        FcFontSet*  fontset;
        /* Only for private collections */
        FcConfig*   config;
} GpFontCollection;


typedef struct {
        FcPattern*	pattern;
	bool		allocated;
} GpFontFamily;


typedef struct {
        cairo_ft_font_t*    cairofnt;
        float               sizeInPixels;
        GpFontStyle         style;
        void                *wineHfont;
} GpFont;


typedef struct {
        GpRectF*        rects;
        int             cnt;
} GpRegion;


typedef struct {
        StringAlignment alignment;
        StringAlignment lineAlignment;
        HotkeyPrefix hotkeyPrefix;
        StringFormatFlags formatFlags;
        StringTrimming  trimming;
        DigitSubstitute substitute;
        float firstTabOffset;
        float *tabStops;
        int numtabStops;
}GpStringFormat;

typedef struct {
        float X, Y;
        float width;
} GpLinePointF;

typedef struct {
	GpColorMap *colormap;
	int colormap_elem;
	float gamma_correction;	
	ARGB key_colorlow;
	ARGB key_colorhigh;
	BOOL key_enabled;
	BOOL no_op;
} GpImageAttribute;


typedef struct {
	GpImageAttribute def;
	GpImageAttribute bitmap;
	GpImageAttribute brush;
	GpImageAttribute pen;
	GpImageAttribute text;
	/* Globals */
	WrapMode wrapmode;
	ARGB color;
} GpImageAttributes;

typedef struct {
        CLSID Clsid;
        GUID  FormatID;

        const WCHAR* CodecName;
        const WCHAR* DllName;
        const WCHAR* FormatDescription;
        const WCHAR* FilenameExtension;
        const WCHAR* MimeType;
        DWORD Flags;
        DWORD Version;
        DWORD SigCount;
        DWORD SigSize;
        const BYTE* SigPattern;
        const BYTE* SigMask;
} ImageCodecInfo;

                
/*
 * Functions
 * 
 */
void gdip_image_init              (GpImage *image);
void *gdip_image_create_Win32_HDC (GpImage *image);
void gdip_image_destroy_Win32_HDC (GpImage *image, void *hdc);

void gdip_bitmap_init  (GpBitmap *bitmap);
GpBitmap *gdip_bitmap_new   (void);
void gdip_bitmap_dispose (GpBitmap *bitmap);
void gdip_bitmap_clone (GpBitmap *bitmap, GpBitmap **clonedbitmap);

void *gdip_bitmap_create_Win32_HDC (GpBitmap *bitmap);
void gdip_bitmap_destroy_Win32_HDC (GpBitmap *bitmap, void *hdc);

void gdip_graphics_init (GpGraphics *graphics);
GpGraphics *gdip_graphics_new (void);
void gdip_graphics_attach_bitmap (GpGraphics *graphics, GpBitmap *image);
void gdip_graphics_detach_bitmap (GpGraphics *graphics, GpBitmap *image);

GpStatus gdip_bitmap_clone_data_rect (GdipBitmapData *srcData, Rect *srcRect, GdipBitmapData *destData, Rect *destRect);
GpStatus gdip_bitmap_change_rect_pixel_format (GdipBitmapData *srcData, Rect *srcRect, GdipBitmapData *destData, Rect *destRect);

void gdip_from_RGB_to_ARGB (BYTE *src, int width, int height, int stride, BYTE **dest, int* dest_stride);
void gdip_from_ARGB_to_RGB (BYTE *src, int width, int height, int stride, BYTE **dest, int* dest_stride);

/* Pen */
void gdip_pen_init (GpPen *pen);
GpPen *gdip_pen_new (void);
GpStatus gdip_pen_setup (GpGraphics *graphics, GpPen *pen);

extern Display *GDIP_display;

void initializeGdipWin32 (void);
void shutdownGdipWin32 (void);
void initCodecList (void);
void releaseCodecList (void);
void gdip_process_bitmap_attributes (GpBitmap *bitmap, void **dest, GpImageAttributes* attr, bool *allocated);


/* Bitmap */
GpStatus GdipCreateBitmapFromScan0 (int width, int height, int strideIn, int format, void *scan0, GpBitmap **bitmap);
GpStatus GdipCreateBitmapFromGraphics (int width, int height, GpGraphics *graphics, GpBitmap **bitmap);
GpStatus GdipBitmapLockBits (GpBitmap *bmp, Rect *rc, int flags, int format, GdipBitmapData *result);
GpStatus GdipBitmapUnlockBits (GpBitmap *bmp, GdipBitmapData *bmpData);
GpStatus GdipBitmapSetPixel ( GpBitmap *bmp, int x, int y, ARGB color);
GpStatus GdipBitmapGetPixel ( GpBitmap *bmp, int x, int y, ARGB *color);


/* Graphics */
GpStatus GdipCreateFromHDC (int hDC, GpGraphics **graphics);
GpStatus GdipDeleteGraphics (GpGraphics *graphics);
GpStatus GdipGetDC (GpGraphics *graphics, int *hDC);
GpStatus GdipReleaseDC (GpGraphics *graphics, int hDC);
GpStatus GdipRestoreGraphics (GpGraphics *graphics, unsigned int graphicsState);
GpStatus GdipSaveGraphics(GpGraphics *graphics, unsigned int * state);
GpStatus GdipRotateWorldTransform (GpGraphics *graphics, float angle, GpMatrixOrder order);
GpStatus GdipTranslateWorldTransform (GpGraphics *graphics, float dx, float dy, GpMatrixOrder order);
GpStatus GdipDrawBezier (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
GpStatus GdipDrawBezierI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4);
GpStatus GdipDrawBeziers (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawBeziersI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawCurve (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawCurveI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawCurve2 (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count, float tension);
GpStatus GdipDrawCurve2I (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count, float tension);
GpStatus GdipDrawCurve3 (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count, int offset, float numOfSegments, float tension);
GpStatus GdipDrawCurve3I (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count, int offset, float numOfSegments, float tension);
GpStatus GdipDrawClosedCurve (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawClosedCurveI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawClosedCurve2 (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count, float tension);
GpStatus GdipDrawClosedCurve2I (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count, float tension);
GpStatus GdipDrawEllipse (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height);
GpStatus GdipDrawImage (GpGraphics *graphics, GpImage *image, float x, float y);
GpStatus GdipDrawImageI (GpGraphics *graphics, GpImage *image, int x, int y);
GpStatus GdipDrawImageRect (GpGraphics *graphics, GpImage *image, float x, float y, float width, float height);
GpStatus GdipDrawImageRectI (GpGraphics *graphics, GpImage *image, int x, int y, int width, int height);
GpStatus GdipDrawImagePoints (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *dstPoints, int count);
GpStatus GdipDrawImagePointsI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *dstPoints, int count);
GpStatus GdipDrawImagePointRect (GpGraphics *graphics, GpImage *image, float x, float y, float srcx, float srcy, float srcwidth, float srcheight, GpUnit srcUnit);
GpStatus GdipDrawImagePointRectI (GpGraphics *graphics, GpImage *image, int x, int y, int srcx, int srcy, int srcwidth, int srcheight, GpUnit srcUnit);
GpStatus GdipDrawImageRectRect (GpGraphics *graphics, GpImage *image, float dstx, float dsty, float dstwidth, float dstheight, float srcx, float srcy, float srcwidth, float srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImageRectRectI (GpGraphics *graphics, GpImage *image, int dstx, int dsty, int dstwidth, int dstheight, int srcx, int srcy, int srcwidth, int srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImagePointsRect (GpGraphics *graphics, GpImage *image, GDIPCONST GpPointF *points, int count, float srcx, float srcy, float srcwidth, float srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawImagePointsRectI (GpGraphics *graphics, GpImage *image, GDIPCONST GpPoint *points, int count, int srcx, int srcy, int srcwidth, int srcheight, GpUnit srcUnit, GDIPCONST GpImageAttributes *imageAttributes, DrawImageAbort callback, void *callbackData);
GpStatus GdipDrawLine (GpGraphics *graphics, GpPen *pen, float x1, float y1, float x2, float y2);
GpStatus GdipDrawLineI (GpGraphics *graphics, GpPen *pen, int x1, int y1, int x2, int y2);
GpStatus GdipDrawLines (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawLinesI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawPie (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height, float startAngle, float sweepAngle);
GpStatus GdipDrawPieI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height, float startAngle, float sweepAngle);
GpStatus GdipDrawPolygon (GpGraphics *graphics, GpPen *pen, GpPointF *points, int count);
GpStatus GdipDrawPolygonI (GpGraphics *graphics, GpPen *pen, GpPoint *points, int count);
GpStatus GdipDrawRectangle (GpGraphics *graphics, GpPen *pen, float x, float y, float width, float height);
GpStatus GdipDrawRectangleI (GpGraphics *graphics, GpPen *pen, int x, int y, int width, int height);
GpStatus GdipDrawRectangles (GpGraphics *graphics, GpPen *pen, GpRectF *rects, int count);
GpStatus GdipDrawRectanglesI (GpGraphics *graphics, GpPen *pen, GpRect *rects, int count);
GpStatus GdipFillClosedCurve (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count);
GpStatus GdipFillClosedCurveI (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count);
GpStatus GdipFillClosedCurve2 (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count, float tension);
GpStatus GdipFillClosedCurve2I (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count, float tension);
GpStatus GdipFillEllipse (GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height);
GpStatus GdipFillEllipseI (GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height);
GpStatus GdipFillRectangle (GpGraphics *graphics, GpBrush *brush, float x1, float y1, float x2, float y2);
GpStatus GdipFillRectangleI (GpGraphics *graphics, GpBrush *brush, int x1, int y1, int x2, int y2);
GpStatus GdipFillRectangles (GpGraphics *graphics, GpBrush *brush, GpRectF *rects, int count);
GpStatus GdipFillRectanglesI (GpGraphics *graphics, GpBrush *brush, GpRect *rects, int count);
GpStatus GdipFillPolygon (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count, GpFillMode fillMode);
GpStatus GdipFillPolygonI (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count, GpFillMode fillMode);
GpStatus GdipFillPolygon2 (GpGraphics *graphics, GpBrush *brush, GpPointF *points, int count);
GpStatus GdipFillPolygon2I (GpGraphics *graphics, GpBrush *brush, GpPoint *points, int count);
GpStatus GdipFillPath (GpGraphics *graphics, GpBrush *brush, GpPath *path);
GpStatus GdipSetRenderingOrigin (GpGraphics *graphics, int x, int y);
GpStatus GdipGetRenderingOrigin (GpGraphics *graphics, int *x, int *y);
GpStatus GdipGetDpiX(GpGraphics *graphics, float *dpi);
GpStatus GdipGetDpiY(GpGraphics *graphics, float *dpi);
GpStatus GdipFillPie(GpGraphics *graphics, GpBrush *brush, float x, float y, float width, float height, float startAngle, float sweepAngle);
GpStatus GdipFillPieI(GpGraphics *graphics, GpBrush *brush, int x, int y, int width, int height, float startAngle, float sweepAngle);
GpStatus GdipGraphicsClear(GpGraphics *graphics, ARGB color);
GpStatus GdipFillRegion(GpGraphics *graphics, GpBrush *brush, GpRegion *region);
GpStatus GdipSetInteroplationMode(GpGraphics *graphics, InterpolationMode imode);
GpStatus GdipGetInteroplationMode(GpGraphics *graphics, InterpolationMode *imode);
GpStatus GdipSetTextRenderingHint(GpGraphics *graphics, TextRenderingHint mode);
GpStatus GdipGetTextRenderingHint(GpGraphics *graphics, TextRenderingHint *mode);
GpStatus GdipSetPixelOffsetMode(GpGraphics *graphics, PixelOffsetMode pixelOffsetMode);
GpStatus GdipGetPixelOffsetMode(GpGraphics *graphics, PixelOffsetMode *pixelOffsetMode);
GpStatus GdipSetTextContrast(GpGraphics *graphics, UINT contrast);
GpStatus GdipGetTextContrast(GpGraphics *graphics, UINT *contrast);
GpStatus GdipSetSmoothingMode(GpGraphics *graphics, SmoothingMode mode);
GpStatus GdipGetSmoothingMode(GpGraphics *graphics, SmoothingMode *mode);

/* Status */
GpStatus gdip_get_status (cairo_status_t status);

/* Pen */
GpStatus GdipCreatePen1 (int argb, float width, GpUnit unit, GpPen **pen);
GpStatus GdipCreatePen2 (GpBrush *brush, float width, GpUnit unit, GpPen **pen);
GpStatus GdipClonePen (GpPen *pen, GpPen **clonedpen);
GpStatus GdipSetPenWidth (GpPen *pen, float width);
GpStatus GdipGetPenWidth (GpPen *pen, float *width);
GpStatus GdipSetPenBrushFill (GpPen *pen, GpBrush *brush);
GpStatus GdipGetPenBrushFill (GpPen *pen, GpBrush **brush);
GpStatus GdipGetPenFillType (GpPen *pen, GpPenType *penType);
GpStatus GdipSetPenColor (GpPen *pen, int color);
GpStatus GdipGetPenColor (GpPen *pen, int *color);
GpStatus GdipSetPenDashStyle (GpPen *pen, GpDashStyle dashStyle);
GpStatus GdipGetPenDashStyle (GpPen *pen, GpDashStyle *dashStyle);
GpStatus GdipSetPenDashOffset (GpPen *pen, float offset);
GpStatus GdipGetPenDashOffset (GpPen *pen, float *offset);
GpStatus GdipGetPenDashCount (GpPen *pen, int *count);
GpStatus GdipSetPenDashArray (GpPen *pen, GDIPCONST float *dash, int count);
GpStatus GdipGetPenDashArray (GpPen *pen, float *dash, int count);
GpStatus GdipSetPenCompoundArray (GpPen *pen, GDIPCONST float *dash, int count);
GpStatus GdipGetPenCompoundArray (GpPen *pen, float *dash, int count);
GpStatus GdipGetPenCompoundCount (GpPen *pen, int *count);
GpStatus GdipSetPenMode (GpPen *pen, GpPenAlignment penMode);
GpStatus GdipGetPenMode (GpPen *pen, GpPenAlignment *penMode);
GpStatus GdipDeletePen (GpPen *pen);
GpStatus GdipSetPenMiterLimit (GpPen *pen, float miterLimit);
GpStatus GdipGetPenMiterLimit (GpPen *pen, float *miterLimit);
GpStatus GdipSetPenLineCap197819 (GpPen *pen, GpLineCap startCap, GpLineCap endCap, GpDashCap dashCap);
GpStatus GdipSetPenStartCap (GpPen *pen, GpLineCap startCap);
GpStatus GdipGetPenStartCap (GpPen *pen, GpLineCap *startCap);
GpStatus GdipSetPenEndCap (GpPen *pen, GpLineCap endCap);
GpStatus GdipGetPenEndCap (GpPen *pen, GpLineCap *endCap);
GpStatus GdipSetPenDashCap197819 (GpPen *pen, GpDashCap dashCap);
GpStatus GdipGetPenDashCap197819 (GpPen *pen, GpDashCap *dashCap);
GpStatus GdipSetPenLineJoin (GpPen *pen, GpLineJoin lineJoin);
GpStatus GdipGetPenLineJoin (GpPen *pen, GpLineJoin *lineJoin);
GpStatus GdipSetPenCustomStartCap (GpPen *pen, GpCustomLineCap *customCap);
GpStatus GdipGetPenCustomStartCap (GpPen *pen, GpCustomLineCap **customCap);
GpStatus GdipSetPenCustomEndCap (GpPen *pen, GpCustomLineCap *customCap);
GpStatus GdipGetPenCustomEndCap (GpPen *pen, GpCustomLineCap **customCap);
GpStatus GdipSetPenTransform (GpPen *pen, GDIPCONST GpMatrix *matrix);
GpStatus GdipGetPenTransform (GpPen *pen, GpMatrix *matrix);
GpStatus GdipResetPenTransform (GpPen *pen);
GpStatus GdipMultiplyPenTransform (GpPen *pen, GpMatrix *matrix, GpMatrixOrder order);
GpStatus GdipTranslatePenTransform (GpPen *pen, float dx, float dy, GpMatrixOrder order);
GpStatus GdipScalePenTransform (GpPen *pen, float sx, float sy, GpMatrixOrder order);
GpStatus GdipRotatePenTransform (GpPen *pen, float angle, GpMatrixOrder order);

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

/* AdjustableArrowCap functions */
GpStatus GdipCreateAdjustableArrowCap (float height, float width, bool isFilled, GpAdjustableArrowCap **arrowCap);
GpStatus GdipSetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, float height);
GpStatus GdipGetAdjustableArrowCapHeight (GpAdjustableArrowCap *arrowCap, float *height);
GpStatus GdipSetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, float width);
GpStatus GdipGetAdjustableArrowCapWidth (GpAdjustableArrowCap *arrowCap, float *width);
GpStatus GdipSetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, float middleInset);
GpStatus GdipGetAdjustableArrowCapMiddleInset (GpAdjustableArrowCap *arrowCap, float *middleInset);
GpStatus GdipSetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, bool isFilled);
GpStatus GdipGetAdjustableArrowCapFillState (GpAdjustableArrowCap *arrowCap, bool *isFilled);

/* Text */
GpStatus GdipDrawString (GpGraphics *graphics, GDIPCONST WCHAR *string, int len, GDIPCONST GpFont *font, GDIPCONST RectF *rc, GDIPCONST GpStringFormat *format, GpBrush *brush);
GpStatus GdipMeasureString(GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font, GDIPCONST RectF *layoutRect, GDIPCONST GpStringFormat *stringFormat,  RectF *boundingBox, int *codepointsFitted, int *linesFilled);


/* Matrix */
GpStatus GdipCreateMatrix (GpMatrix **matrix);
GpStatus GdipCreateMatrix2 (float m11, float m12, float m21, float m22, float dx, float dy, GpMatrix **matrix);
GpStatus GdipCreateMatrix3 (const GpRectF *rect, const GpPointF *dstplg, GpMatrix **matrix);
GpStatus GdipCreateMatrix3I (const GpRect *rect, const GpPoint *dstplg, GpMatrix **matrix);
GpStatus GdipCloneMatrix (GpMatrix *matrix, GpMatrix **cloneMatrix);
GpStatus GdipDeleteMatrix (GpMatrix *matrix);
GpStatus GdipSetMatrixElements (GpMatrix *matrix, float m11, float m12, float m21, float m22, float dx, float dy);
GpStatus GdipMultiplyMatrix (GpMatrix *matrix, GpMatrix *matrix2, GpMatrixOrder order);
GpStatus GdipTranslateMatrix (GpMatrix *matrix, float offsetX, float offsetY, GpMatrixOrder order);
GpStatus GdipScaleMatrix (GpMatrix *matrix, float scaleX, float scaleY, GpMatrixOrder order);
GpStatus GdipRotateMatrix(GpMatrix *matrix, float angle, GpMatrixOrder order);
GpStatus GdipShearMatrix (GpMatrix *matrix, float shearX, float shearY, GpMatrixOrder order);
GpStatus GdipInvertMatrix (GpMatrix *matrix);
GpStatus GdipTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count);
GpStatus GdipTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count);
GpStatus GdipVectorTransformMatrixPoints (GpMatrix *matrix, GpPointF *pts, int count);
GpStatus GdipVectorTransformMatrixPointsI (GpMatrix *matrix, GpPoint *pts, int count);
GpStatus GdipGetMatrixElements (GpMatrix *matrix, float *matrixOut);
GpStatus GdipIsMatrixInvertible (GpMatrix *matrix, int *result);
GpStatus GdipIsMatrixIdentity (GpMatrix *matrix, int *result);
GpStatus GdipIsMatrixEqual (GpMatrix *matrix, GpMatrix *matrix2, int *result);


/* Font Family*/         
GpStatus GdipNewInstalledFontCollection(GpFontCollection** fontCollection);
GpStatus GdipGetFontCollectionFamilyCount(GpFontCollection* fontCollection, int* numFound);
GpStatus GdipGetFontCollectionFamilyList(GpFontCollection* fontCollection, int numSought, GpFontFamily** gpfamilies, int* numFound);
GpStatus GdipGetFamilyName(GDIPCONST GpFontFamily* family, WCHAR name[LF_FACESIZE], int language);
GpStatus GdipGetGenericFontFamilySansSerif(GpFontFamily **nativeFamily);
GpStatus GdipGetGenericFontFamilySerif(GpFontFamily **nativeFamily);
GpStatus GdipGetGenericFontFamilyMonospace(GpFontFamily **nativeFamily);
GpStatus GdipDeleteFontFamily(GpFontFamily *FontFamily);    
GpStatus GdipGetEmHeight(GDIPCONST GpFontFamily* family, GpFontStyle style, short* EmHeight);
GpStatus GdipGetCellAscent(GDIPCONST GpFontFamily* family, GpFontStyle style, short* CellAscent);
GpStatus GdipGetCellDescent(GDIPCONST GpFontFamily* family, GpFontStyle style, short* CellDescent);
GpStatus GdipGetLineSpacing(GDIPCONST GpFontFamily* family, GpFontStyle style, short* LineSpacing);
GpStatus GdipIsStyleAvailable(GDIPCONST GpFontFamily* family, int style, BOOL* IsStyleAvailable);
GpStatus GdipNewPrivateFontCollection(GpFontCollection** fontCollection);
GpStatus GdipDeletePrivateFontCollection(GpFontCollection** fontCollection);
GpStatus GdipPrivateAddFontFile(GpFontCollection* fontCollection,  GDIPCONST WCHAR* filename);


/* Font */
GpStatus GdipCreateFont(GDIPCONST GpFontFamily* family, float emSize, GpFontStyle style, Unit unit,  GpFont **font);
GpStatus GdipCreateFontFamilyFromName(GDIPCONST WCHAR *name, GpFontCollection *fontCollection, GpFontFamily **FontFamily);
GpStatus GdipDeleteFont(GpFont* font);


/* String format*/
GpStatus GdipCreateStringFormat(int formatAttributes, int language, GpStringFormat  **format);
GpStatus GdipStringFormatGetGenericDefault(GpStringFormat **format);               
GpStatus GdipStringFormatGetGenericTypographic(GpStringFormat **format);                                                                                              
GpStatus GdipCloneStringFormat(GDIPCONST GpStringFormat* format,  GpStringFormat** newFormat);
GpStatus GdipDeleteStringFormat(GpStringFormat *format);
GpStatus GdipSetStringFormatAlign(GpStringFormat *format, StringAlignment align);
GpStatus GdipGetStringFormatAlign(GDIPCONST GpStringFormat *format, StringAlignment *align);
GpStatus GdipSetStringFormatLineAlign(GpStringFormat *format, StringAlignment align);
GpStatus GdipGetStringFormatLineAlign(GDIPCONST GpStringFormat *format, StringAlignment *align);
GpStatus GdipSetStringFormatTrimming(GpStringFormat  *format, StringTrimming trimming);
GpStatus GdipGetStringFormatTrimming(GDIPCONST GpStringFormat *format, StringTrimming *trimming);
GpStatus GdipSetStringFormatHotkeyPrefix(GpStringFormat *format, HotkeyPrefix hotkeyPrefix);
GpStatus GdipGetStringFormatHotkeyPrefix(GpStringFormat *format, HotkeyPrefix *hotkeyPrefix);
GpStatus GdipSetStringFormatFlags(GpStringFormat *format, StringFormatFlags flags);
GpStatus GdipGetStringFormatFlags(GDIPCONST GpStringFormat *format, StringFormatFlags *flags);
GpStatus GdipSetStringFormatTrimming(GpStringFormat* format,  StringTrimming trimming);
GpStatus GdipGetStringFormatTrimming(GDIPCONST GpStringFormat *format, StringTrimming* trimming);
GpStatus GdipSetStringFormatTabStops(GpStringFormat *format, float firstTabOffset, int count, float *tabStops);
GpStatus GdipGetStringFormatDigitSubstitution(GDIPCONST GpStringFormat *format, int *language, DigitSubstitute *substitute);
GpStatus GdipSetStringFormatDigitSubstitution(GpStringFormat *format, int language, DigitSubstitute substitute);      
GpStatus GdipGetStringFormatTabStopCount(GDIPCONST GpStringFormat *format, int *count);
GpStatus GdipGetStringFormatTabStops(GDIPCONST GpStringFormat *format, int count, float *firstTabOffset, float *tabStops);

/* ImageAttributes */
GpStatus GdipCreateImageAttributes(GpImageAttributes **imageattr);
GpStatus GdipCloneImageAttributes(GDIPCONST GpImageAttributes *imageattr, GpImageAttributes **cloneImageattr);
GpStatus GdipDisposeImageAttributes(GpImageAttributes *imageattr);
GpStatus GdipSetImageAttributesThreshold(GpImageAttributes *imageattr,  ColorAdjustType type, BOOL enableFlag, float threshold);
GpStatus GdipSetImageAttributesGamma(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, float gamma);
GpStatus GdipSetImageAttributesNoOp(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag);
GpStatus GdipSetImageAttributesColorKeys(GpImageAttributes *imageattr, ColorAdjustType type,  BOOL enableFlag,
        ARGB colorLow, ARGB colorHigh);
GpStatus GdipSetImageAttributesOutputChannelColorProfile(GpImageAttributes *imageattr, ColorAdjustType type,  BOOL enableFlag,
        GDIPCONST WCHAR *colorProfileFilename);
GpStatus GdipSetImageAttributesRemapTable(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, UINT mapSize, GDIPCONST GpColorMap *map);
GpStatus GdipSetImageAttributesWrapMode(GpImageAttributes *imageattr, WrapMode wrap, ARGB argb, BOOL clamp); 
GpStatus GdipGetImageAttributesAdjustedPalette(GpImageAttributes *imageattr, ColorPalette *colorPalette, ColorAdjustType type); 
GpStatus GdipSetImageAttributesColorMatrix(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag,  GpColorMatrix* colorMatrix,
	GpColorMatrix* grayMatrix, GpColorMatrixFlags flags);
GpStatus GdipSetImageAttributesOutputChannel(GpImageAttributes *imageattr, ColorAdjustType type, BOOL enableFlag, GpColorChannelFlags channelFlags);

/* Region */
GpStatus GdipCreateRegion(GpRegion **region);
GpStatus GdipCreateRegionRect(GDIPCONST GpRectF *rect, GpRegion **region);
GpStatus GdipCreateRegionRectI(GDIPCONST GpRect *rect, GpRegion **region);
GpStatus GdipCreateRegionPath(GpPath *path, GpRegion **region);
GpStatus GdipCloneRegion(GpRegion *region, GpRegion **cloneRegion);
GpStatus GdipDeleteRegion(GpRegion *region);
GpStatus GdipSetInfinite(GpRegion *region);
GpStatus GdipSetEmpty(GpRegion *region);
GpStatus GdipCombineRegionRect(GpRegion *region, GDIPCONST GpRectF *rect, CombineMode combineMode);
GpStatus GdipCombineRegionRectI(GpRegion *region, GDIPCONST GpRect *rect, CombineMode combineMode);
GpStatus GdipCombineRegionPath(GpRegion *region, GpPath *path, CombineMode combineMode);
GpStatus GdipCombineRegionRegion(GpRegion *region,  GpRegion *region2, CombineMode combineMode);
GpStatus GdipTransformRegion(GpRegion *region, GpMatrix *matrix);
GpStatus GdipGetRegionBounds(GpRegion *region, GpGraphics *graphics, GpRectF *rect);
GpStatus GdipIsEmptyRegion(GpRegion *region, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsInfiniteRegion(GpRegion *region, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleRegionPoint(GpRegion *region, float x, float y, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleRegionPointI(GpRegion *region, int x, int y, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleRegionRect(GpRegion *region, float x, float y, float width, float height, GpGraphics *graphics, BOOL *result);
GpStatus GdipIsVisibleRegionRectI(GpRegion *region, int x, int y, int width, int height, GpGraphics *graphics, BOOL *result);
GpStatus GdipGetRegionScansCount(GpRegion *region, int* count, GpMatrix* matrix);
GpStatus GdipGetRegionScans(GpRegion *region, GpRectF* rects, int* count, GpMatrix* matrix);
GpStatus GdipIsEqualRegion(GpRegion *region, GpRegion *region2, GpGraphics *graphics, BOOL *result);
GpStatus GdipGetRegionDataSize(GpRegion *region, UINT * bufferSize);
GpStatus GdipGetRegionData(GpRegion *region, BYTE * buffer, UINT bufferSize, UINT *sizeFilled);
GpStatus GdipTranslateRegion(GpRegion *region, float dx, float dy);
GpStatus GdipTranslateRegionI(GpRegion *region, int dx, int dy);
GpStatus GdipTransformRegion(GpRegion *region, GpMatrix *matrix);


/* Encoders / Decoders */
GpStatus GdipGetImageDecodersSize (int *numDecoders, int *size);
GpStatus GdipGetImageDecoders (int numDecoders, int size, ImageCodecInfo *decoders);
GpStatus GdipGetImageEncodersSize (int *numEncoders, int *size);
GpStatus GdipGetImageEncoders (UINT numEncoders, UINT size, ImageCodecInfo *encoders);


/* Path*/
#include "graphics-path.h"

/* for drawing curves */
GpPointF *convert_points (const GpPoint *points, int count);
GpPointF *gdip_closed_curve_tangents (int terms, const GpPointF *points, int count, float tension);
GpPointF *gdip_open_curve_tangents (int terms,  const GpPointF *points, int count, float tension);
void gdip_calculate_coefficients (int count, int terms, float **coefficients, int *coefficients_count);

/* Memory */
void *GdipAlloc (int size);
void GdipFree (void *ptr);

/* Utility*/
int fcmp (double x1, double x2, double epsilon);
float gdip_get_display_dpi();
void gdip_unitConversion(Unit fromUnit, Unit toUnit, float nSrc, float* nTrg);
int gdpi_utf8_to_glyphs (cairo_ft_font_t* font,	 const unsigned char* utf8, double	x0,
   double	y0, cairo_glyph_t** glyphs, size_t* nglyphs);

void gdip_font_drawunderline (GpGraphics *graphics, GpBrush *brush, float x, float y, float width);
void gdip_font_drawstrikeout (GpGraphics *graphics, GpBrush *brush, float x, float y, float width);

cairo_status_t gdip_cairo_set_surface_pattern (cairo_t *t, cairo_surface_t *s);

void gdip_rect_expand_by (GpRectF *rect, GpPointF *point);

cairo_surface_t * gdip_bitmap_ensure_surface (GpBitmap *bitmap);

const EncoderParameter *gdip_find_encoder_parameter (GDIPCONST EncoderParameters *eps, const GUID *guid);

/* Stream handling bits */
typedef int (*GetBytesDelegate) (unsigned char *, int, BOOL);
typedef int (*SeekDelegate) (int, int);
typedef int (*PutBytesDelegate) (unsigned char *, int);
typedef void (*CloseDelegate) ();
typedef long (*SizeDelegate) ();

/* CLSIDs/GUIDs */
extern GUID gdip_bmp_image_format_guid;
extern GUID gdip_jpg_image_format_guid;
extern GUID gdip_png_image_format_guid;
extern GUID gdip_gif_image_format_guid;
extern GUID gdip_tif_image_format_guid;

extern GUID GdipEncoderQuality;
extern GUID GdipEncoderCompression;

#endif /* _GDIP_H */
