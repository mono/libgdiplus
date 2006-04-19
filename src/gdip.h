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
 *	Geoff Norton (gnorton@customerdna.com)
 *      Jonathan Gilbert (logic@deltaq.org)
 *
 * Copyright (C) 2003-2006 Novell, Inc (http://www.novell.com)
 */

#ifndef _GDIP_H
#define _GDIP_H

#include <stdlib.h>
#include <stdio.h>
#include <glib.h>
#include <unistd.h>

#include "config.h"

#include "pixman.h"

#ifdef USE_INCLUDED_CAIRO
#include <cairo.h>
#else
#include <cairo/cairo.h>
#endif
#include <X11/Xlib.h>

#ifdef CAIRO_HAS_FT_FONT
#ifdef USE_INCLUDED_CAIRO
#include <cairo-ft.h>
#else
#include <cairo/cairo-ft.h>
#endif
#endif

#define gdip_cairo_matrix_copy(m1, m2) memcpy (m1, m2, sizeof (cairo_matrix_t))

#ifdef WORDS_BIGENDIAN
#define set_pixel_bgra(pixel,index,b,g,r,a) do { \
		((unsigned char *)(pixel))[index+0] = a; \
		((unsigned char *)(pixel))[index+1] = r; \
		((unsigned char *)(pixel))[index+2] = g; \
		((unsigned char *)(pixel))[index+3] = b; \
	} while (0)
#define get_pixel_bgra(color, b, g, r, a) do { \
		a = (color & 0x000000ff); \
		r = (color & 0x0000ff00) >> 8; \
		g = (color & 0x00ff0000) >> 16; \
		b = (color & 0xff000000) >> 24; \
	} while (0)
#else
#define set_pixel_bgra(pixel,index,b,g,r,a) do { \
		((unsigned char *)(pixel))[index+0] = b; \
		((unsigned char *)(pixel))[index+1] = g; \
		((unsigned char *)(pixel))[index+2] = r; \
		((unsigned char *)(pixel))[index+3] = a; \
	} while (0)
#define get_pixel_bgra(color, b, g, r, a) do { \
		a = ((color & 0xff000000) >> 24); \
		r = ((color & 0x00ff0000) >> 16); \
		g = ((color & 0x0000ff00) >> 8); \
		b = (color & 0x000000ff); \
	} while(0)
#endif

#ifdef CAIRO_HAS_XLIB_SURFACE
#ifdef USE_INCLUDED_CAIRO
#include <cairo-xlib.h>
#else
#include <cairo/cairo-xlib.h>
#endif
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

#define CAIRO_AA_OFFSET_X	1
#define CAIRO_AA_OFFSET_Y	0.5

typedef unsigned char byte;
typedef int bool;
typedef guint16 WCHAR; /* 16-bits unicode */
typedef guint32 UINT;
typedef guint32 ARGB;
typedef gint32 PROPID;

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
    UnitMillimeter = 6,
    UnitCairoPoint = 7
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
	gtMemoryBitmap,
	gtOSXDrawable,
	gtPostScript
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
        PathPointTypeBezier3 = 0x03
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
        CoordinateSpaceDevice
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

typedef enum {
	X11DRV_GET_DISPLAY,			/* get X11 display for a DC */
	X11DRV_GET_DRAWABLE,		/* get current drawable for a DC */
	X11DRV_GET_FONT			/* get current X font for a DC */
} x11drv_escape_codes;


/*
 * Structures
 *
 */

#define GBD_OWN_SCAN0	(1<<8)
#define GBD_WRITE_OK	(1<<9)
#define GBD_LOCKED	(1<<10)

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
	int first, length;
} CharacterRange;

typedef struct _CustomLineCap GpCustomLineCap;
typedef struct _AdjustableArrowCap GpAdjustableArrowCap;
typedef struct _Brush GpBrush;
typedef cairo_matrix_t GpMatrix;

typedef struct {
	int color;
        GpBrush *brush;
	BOOL own_brush;
	float width;
        float miter_limit;
        GpLineJoin line_join;
        GpDashStyle dash_style;
	GpLineCap line_cap;  /* Cairo supports only same kind of end caps for both the ends. */
	GpLineCap end_cap;
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
	GpFillMode fill_mode;
	int count;
	GByteArray *types;
	GArray *points;
	BOOL start_new_fig; /* Flag to keep track if we need to start a new figure */
} GpPath;

typedef struct {
        int Count;
        PointF *Points;
        byte *Types;
} GpPathData;

typedef struct {
	GpPath *path;
	int markerPosition; /* The start position of next marker, index of (marker type) + 1  */
	int subpathPosition; /* The start position of next subpath, index of (start type) */
	int pathTypePosition; /* The position to get the next path type inside a subpath */
} GpPathIterator;

/* internal (private) structure */
typedef struct {
	int X;
	int Y;
	int Width;
	int Height;
	unsigned char *Mask;
	BOOL reduced;
} GpRegionBitmap;

/* internal (private) structure */
typedef struct GpPathTree {
	CombineMode	mode;
	GpPath*		path;
	struct GpPathTree*	branch1;
	struct GpPathTree*	branch2;
} GpPathTree;

typedef struct {
	guint32		type;
        int		cnt;
        GpRectF*	rects;
	GpPathTree*	tree;
	GpRegionBitmap*	bitmap;
} GpRegion;
 
typedef struct {
	cairo_matrix_t		matrix;
	GpRegion*		clip;
	cairo_matrix_t		clip_matrix;
	GpCompositingMode    	composite_mode;
	GpCompositingQuality 	composite_quality;
	InterpolationMode 	interpolation;
	GpUnit			page_unit;
	float			scale;
	SmoothingMode		draw_mode;
	TextRenderingHint 	text_mode;
	PixelOffsetMode 	pixel_mode;
	int			org_x;
	int			org_y;
} GpState;

typedef struct {
	cairo_t         *ct;
	GpMatrix	*copy_of_ctm;
	Display		*display;
	Drawable	drawable;
	void            *image;
	int             type; 
	GpRegion*	clip;
	GpMatrix*	clip_matrix;
	GpRect		bounds;
	GpUnit		page_unit;
	float		scale;
	InterpolationMode interpolation;
	GpPen*		last_pen;	/* caching pen and brush to avoid unnecessary sets */
	GpBrush*	last_brush;
	SmoothingMode	draw_mode;
	TextRenderingHint text_mode;
	GpState*	saved_status;
	int		saved_status_pos;
	GpCompositingMode    composite_mode;
	GpCompositingQuality composite_quality;
	PixelOffsetMode pixel_mode;
	float		aa_offset_x;
	float		aa_offset_y;
	int		render_origin_x;
	int		render_origin_y;
	float		dpi_x;
	float		dpi_y;
} GpGraphics;



typedef struct {
	UINT Flags;
	UINT Count;
	ARGB Entries[1];
} ColorPalette;

typedef struct {
    DWORD Data1;
    WORD  Data2;
    WORD  Data3;
    byte  Data4 [8];
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

/* This structure is mirrored in System.Drawing.Imaging.BitmapData.
   Any changes here must also be made to BitmapData.cs */
typedef struct {
	unsigned int	width;
	unsigned int	height;
	int		stride;
	int		pixel_format;
	byte 		*scan0;
	unsigned int	reserved;
	ColorPalette	*palette;

	int		property_count;		/* Number of properties */
	PropertyItem 	*property;		/* Properties associated with image */

	float 		dpi_horz;		/* */
	float 		dpi_vert;		/* */
	ImageFlags	image_flags;		/* Alpha, ColorSpace, etc. */

	unsigned int	left;			/* left display coordinate of frame */
	unsigned int	top;			/* top display coordinate of frame */
	unsigned int	x;			/* LockBits: left coordinate of locked rectangle */
	unsigned int	y;			/* LockBits: top coordinate of locked rectangle */
} GdipBitmapData, BitmapData;

typedef struct {
	int		count;			/* Number of bitmaps contained in this frame */
	BitmapData	*bitmap;		/* Bitmaps for this frame */
	GUID		frame_dimension;	/* GUID describing the frame type */
} FrameData;

typedef struct {
	/* Image Description */
	ImageType     	type;			/* Undefined, Bitmap, MetaFile */
	ImageFormat     image_format;		/* BMP, TIF, GIF, PNG, etc.	*/

	/* Image Data */
	int		num_of_frames;		/* Number of frames */
	FrameData	*frames;		/* Array of frames (Page, Time, Resolution) for the image */

	/* Tracking of active image */
	int		active_frame;		/* Index of frame currently used */
	int		active_bitmap_no;	/* Index of active bitmap in current frame */
	BitmapData	*active_bitmap;		/* Pointer to active frame/bitmap; DO NOT free() */

	/* Internal fields */
        int             cairo_format;
	cairo_surface_t *surface;
} GpImage, GpBitmap;

typedef struct {
        FcFontSet*  fontset;
        /* Only for private collections */
        FcConfig*   config;
} GpFontCollection;

typedef struct {
        FcPattern*	pattern;
	bool		allocated;
	short 		height;
	short 		linespacing;
	short		celldescent;
	short		cellascent;
} GpFontFamily;


typedef struct {
        cairo_font_face_t*  cairofnt;
        float               sizeInPixels;
        GpFontStyle         style;
        unsigned char       *face;
	GpFontFamily	    *family;
	float               emSize;
	GpUnit              unit;
} GpFont;


typedef struct {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    WCHAR lfFaceName[LF_FACESIZE];
} LOGFONTW;

typedef struct {
    LONG lfHeight;
    LONG lfWidth;
    LONG lfEscapement;
    LONG lfOrientation;
    LONG lfWeight;
    BYTE lfItalic;
    BYTE lfUnderline;
    BYTE lfStrikeOut;
    BYTE lfCharSet;
    BYTE lfOutPrecision;
    BYTE lfClipPrecision;
    BYTE lfQuality;
    BYTE lfPitchAndFamily;
    char lfFaceName[LF_FACESIZE];
} LOGFONTA;
 
typedef struct
{
	char		szFamily [128];
	float		sizeInPixels;
	GpFontStyle	style;
	GpFont *	font;
	int		refcount;
} GpCachedFont;

typedef struct {
        StringAlignment alignment;
        StringAlignment lineAlignment;
        HotkeyPrefix hotkeyPrefix;
        StringFormatFlags formatFlags;
        StringTrimming  trimming;
        DigitSubstitute substitute;
	CharacterRange *charRanges;
        float firstTabOffset;
        float *tabStops;
        int numtabStops;
	int charRangeCount;
} GpStringFormat;

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
	GpColorMatrix *colormatrix;
	BOOL colormatrix_enabled;
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
 * Pixel Streams
 *
 */

typedef struct
{
	Rect		region;
	int		x, y;			/* the offset of the next byte that will be loaded, once the buffer is depleted */
	unsigned short	buffer;
	int		p;			/* index of pixel within 'buffer' that was returned by the last call to gdip_pixel_stream_get_next () */
	int		one_pixel_mask;
	int		one_pixel_shift;
	int		pixels_per_byte;	/* a negative value is used to indicate a count of bytes per pixel for depths of more than 8 bits */

	BitmapData	*data;
	unsigned char	*scan;
} StreamingState;

GpStatus gdip_init_pixel_stream (StreamingState *state, BitmapData *data, int x, int y, int w, int h);
BOOL gdip_pixel_stream_has_next (StreamingState *state);
unsigned int gdip_pixel_stream_get_next (StreamingState *state);
void gdip_pixel_stream_set_next (StreamingState *state, unsigned int pixel_value);

/*
 * Functions
 * 
 */
void gdip_image_init              (GpImage *image);

void		gdip_bitmap_init(GpBitmap *bitmap);
GpBitmap	*gdip_bitmap_new(void);
GpBitmap	*gdip_bitmap_new_with_frame(const GUID *dimension, bool add_bitmapdata);
FrameData	*gdip_frame_add(GpBitmap *bitmap, const GUID *dimension);
BitmapData	*gdip_frame_add_bitmapdata(FrameData *frame);
void		gdip_bitmap_dispose (GpBitmap *bitmap);
GpStatus	gdip_bitmap_clone (GpBitmap *bitmap, GpBitmap **clonedbitmap);
GpStatus	gdip_bitmap_setactive(GpBitmap *bitmap, const GUID *dimension, int index);
GpStatus	gdip_bitmapdata_clone(BitmapData *src, BitmapData **dest, int count);
ColorPalette	*gdip_palette_clone(ColorPalette *original);


void gdip_graphics_init (GpGraphics *graphics, cairo_surface_t *surface);
GpGraphics *gdip_graphics_new (cairo_surface_t *surface);

GpStatus gdip_bitmap_clone_data_rect (GdipBitmapData *srcData, Rect *srcRect, GdipBitmapData *destData, Rect *destRect);
GpStatus gdip_bitmap_change_rect_pixel_format (GdipBitmapData *srcData, Rect *srcRect, GdipBitmapData *destData, Rect *destRect);

int gdip_from_RGB_to_ARGB (BYTE *src, int width, int height, int stride, BYTE **dest, int* dest_stride);
int gdip_from_ARGB_to_RGB (BYTE *src, int width, int height, int stride, BYTE **dest, int* dest_stride);

/* Pen */
void gdip_pen_init (GpPen *pen);
GpPen *gdip_pen_new (void);
GpStatus gdip_pen_setup (GpGraphics *graphics, GpPen *pen);

void initCodecList (void);
void releaseCodecList (void);
void gdip_process_bitmap_attributes (GpBitmap *bitmap, void **dest, GpImageAttributes* attr, bool *allocated);


/* Bitmap */
GpStatus GdipCreateBitmapFromFile(GDIPCONST WCHAR* filename, GpBitmap **bitmap);
GpStatus GdipCreateBitmapFromFileICM(GDIPCONST WCHAR* filename, GpBitmap **bitmap);
GpStatus GdipCreateBitmapFromScan0 (int width, int height, int strideIn, int format, void *scan0, GpBitmap **bitmap);
GpStatus GdipCreateBitmapFromGraphics (int width, int height, GpGraphics *graphics, GpBitmap **bitmap);
GpStatus GdipBitmapLockBits (GpBitmap *bmp, Rect *rc, int flags, int format, GdipBitmapData *result);
GpStatus GdipBitmapUnlockBits (GpBitmap *bmp, GdipBitmapData *bmpData);
GpStatus GdipBitmapSetPixel ( GpBitmap *bmp, int x, int y, ARGB color);
GpStatus GdipBitmapGetPixel ( GpBitmap *bmp, int x, int y, ARGB *color);


/* Graphics */
GpStatus GdipCreateFromHDC (void *hDC, GpGraphics **graphics);
GpStatus GdipDeleteGraphics (GpGraphics *graphics);
GpStatus GdipGetDC (GpGraphics *graphics, void **hDC);
GpStatus GdipReleaseDC (GpGraphics *graphics, void *hDC);
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
GpStatus GdipSetVisibleClip_linux (GpGraphics *graphics, GpRect *rect);
GpStatus GdipTranslateClip (GpGraphics *graphics, float dx, float dy);
GpStatus GdipTranslateClipI (GpGraphics *graphics, int dx, int dy);

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
GpStatus GdipMeasureCharacterRanges(GpGraphics *graphics, GDIPCONST WCHAR *string, int length, GDIPCONST GpFont *font, GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *stringFormat, int regionCount, GpRegion **regions);

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
GpStatus GdipIsMatrixInvertible (GpMatrix *matrix, BOOL *result);
GpStatus GdipIsMatrixIdentity (GpMatrix *matrix, BOOL *result);
GpStatus GdipIsMatrixEqual (GpMatrix *matrix, GpMatrix *matrix2, BOOL *result);


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
GpStatus GdipGetStringFormatMeasurableCharacterRangeCount(GDIPCONST GpStringFormat *format, int *count);
GpStatus GdipSetStringFormatMeasurableCharacterRanges(GpStringFormat *format, int rangeCount, GDIPCONST CharacterRange *ranges);

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
GpStatus GdipCreateRegionRgnData (GDIPCONST BYTE *regionData, int size, GpRegion **region);
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
void *GdipRealloc (void *org, int size);
void *GdipCalloc (size_t nelem, size_t elsize); 
void GdipFree (void *ptr);

/* Utility*/
int fcmp (double x1, double x2, double epsilon);
float gdip_get_display_dpi();
float gdip_unitx_convgr (GpGraphics *graphics, float nSrc);
float gdip_unity_convgr (GpGraphics *graphics, float nSrc);
void gdip_unit_conversion (Unit from, Unit to, float dpi, GraphicsType type, float nSrc, float* nTrg);

cairo_content_t from_cairoformat_to_content (cairo_format_t format);

void gdip_rect_expand_by (GpRectF *rect, GpPointF *point);

cairo_surface_t * gdip_bitmap_ensure_surface (GpBitmap *bitmap);
BOOL gdip_is_an_alpha_pixelformat (PixelFormat pixfmt);
BOOL gdip_is_an_indexed_pixelformat (PixelFormat pixfmt);
GpBitmap * gdip_convert_indexed_to_rgb (GpBitmap *bitmap);

const EncoderParameter *gdip_find_encoder_parameter (GDIPCONST EncoderParameters *eps, const GUID *guid);
gchar *ucs2_to_utf8(const gunichar2 *ucs2, int length);
bool utf8_to_ucs2(const gchar *utf8, gunichar2 *ucs2, int ucs2_len);
int utf8_encode_ucs2char(gunichar2 unichar, unsigned char *dest);
int utf8_decode_ucs2char(const unsigned char *src, gunichar2 *uchar);

GpStatus gdip_get_pattern_status (cairo_pattern_t *pat);


/* Stream handling bits */
typedef int (*GetHeaderDelegate) (unsigned char *, int);
typedef int (*GetBytesDelegate) (unsigned char *, int, BOOL);
typedef long (*SeekDelegate) (int, int);
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
