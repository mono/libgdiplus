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

#ifndef __GDIPENUMS_H__
#define __GDIPENUMS_H__

#include "win32structs.h"

typedef UINT GraphicsContainer;
typedef UINT GraphicsState;

typedef enum {
	ColorAdjustTypeDefault,
	ColorAdjustTypeBitmap,
	ColorAdjustTypeBrush,
	ColorAdjustTypePen,
	ColorAdjustTypeText,
	ColorAdjustTypeCount,
	ColorAdjustTypeAny     
} ColorAdjustType;

typedef enum {
	ColorChannelFlagsC	= 0,
	ColorChannelFlagsM,
	ColorChannelFlagsY,
	ColorChannelFlagsK,
	ColorChannelFlagsLast
} ColorChannelFlags;

typedef enum {
	ColorMatrixFlagsDefault		= 0,
	ColorMatrixFlagsSkipGrays	= 1,
	ColorMatrixFlagsAltGray		= 2
} ColorMatrixFlags;

typedef enum {
	CombineModeReplace	= 0,
	CombineModeIntersect	= 1,
	CombineModeUnion	= 2,
	CombineModeXor		= 3,
	CombineModeExclude	= 4,
	CombineModeComplement	= 5
} CombineMode;

typedef enum {
	CompositingModeSourceOver,
	CompositingModeSourceCopy
} CompositingMode;

typedef enum {
	CoordinateSpaceWorld,
	CoordinateSpacePage,
	CoordinateSpaceDevice
} CoordinateSpace, GpCoordinateSpace;

typedef enum {
	CustomLineCapTypeDefault		= 0,
	CustomLineCapTypeAdjustableArrow	= 1
} CustomLineCapType;

typedef enum {
	EncoderParameterValueTypeByte		= 1,
	EncoderParameterValueTypeASCII		= 2,
	EncoderParameterValueTypeShort		= 3,
	EncoderParameterValueTypeLong		= 4,
	EncoderParameterValueTypeRational	= 5,
	EncoderParameterValueTypeLongRange	= 6,
	EncoderParameterValueTypeUndefined	= 7,
	EncoderParameterValueTypeRationalRange	= 8
} EncoderParameterValueType;

typedef enum {
	EncoderValueColorTypeCMYK = 0,
	EncoderValueColorTypeYCCK = 1,
	EncoderValueCompressionLZW = 2,
	EncoderValueCompressionCCITT3 = 3,
	EncoderValueCompressionCCITT4 = 4,
	EncoderValueCompressionRle = 5,
	EncoderValueCompressionNone = 6,
	EncoderValueScanMethodInterlaced = 7,
	EncoderValueScanMethodNonInterlaced = 8,
	EncoderValueVersionGif87 = 9,
	EncoderValueVersionGif89 = 10,
	EncoderValueRenderProgressive = 11,
	EncoderValueRenderNonProgressive = 12,
	EncoderValueTransformRotate90 = 13,
	EncoderValueTransformRotate180 = 14,
	EncoderValueTransformRotate270 = 15,
	EncoderValueTransformFlipHorizontal = 16,
	EncoderValueTransformFlipVertical = 17,
	EncoderValueMultiFrame = 18,
	EncoderValueLastFrame = 19,
	EncoderValueFlush = 20,
	EncoderValueFrameDimensionTime = 21,
	EncoderValueFrameDimensionResolution = 22,
	EncoderValueFrameDimensionPage = 23,
	EncoderValueColorTypeGray = 24,
	EncoderValueColorTypeRGB = 25
} EncoderValue;

typedef enum {
	FontStyleRegular	= 0,
	FontStyleBold		= 1,
	FontStyleItalic		= 2,
	FontStyleUnderline	= 4,
	FontStyleStrikeout	= 8
} FontStyle;

typedef enum {
	BrushTypeSolidColor	= 0,	/* Solid Brush */
	BrushTypeHatchFill	= 1,	/* Hatch Brush */
	BrushTypeTextureFill	= 2,	/* Texture Brush */
	BrushTypePathGradient	= 3,	/* PathGradient Brush */
	BrushTypeLinearGradient	= 4	/* LinearGradient Brush */
} GpBrushType, BrushType;

typedef enum {
	DashCapFlat		= 0,
	DashCapRound		= 2,
	DashCapTriangle		= 3
} GpDashCap, DashCap;

typedef enum {
	DashStyleSolid		= 0,	/* solid line */
	DashStyleDash		= 1,	/* dashed line */
	DashStyleDot		= 2,	/* dotted line */
	DashStyleDashDot	= 3,	/* alt. dash-dot */
	DashStyleDashDotDot	= 4,	/* alt. dash-dot-dot */
	DashStyleCustom		= 5 	/* user-defined */
} GpDashStyle, DashStyle;

typedef enum {
	FillModeAlternate,
	FillModeWinding
} GpFillMode, FillMode;

typedef enum {
	FlushIntentionFlush	= 0,
	FlushIntentionSync	= 1
} GpFlushIntention, FlushIntention;

typedef enum {
	LineCapFlat		= 0,
	LineCapSquare		= 1,
	LineCapRound		= 2,
	LineCapTriangle		= 3,
	LineCapNoAnchor		= 0x10,
	LineCapSquareAnchor	= 0x11,
	LineCapRoundAnchor	= 0x12,
	LineCapDiamondAnchor	= 0x13,
	LineCapArrowAnchor	= 0x14,
	LineCapCustom		= 0xff,
	LineCapAnchorMask 	= 0xf0
} GpLineCap, LineCap;

typedef enum {
	LineJoinMiter		= 0,	/* sharp corner */
	LineJoinBevel		= 1,	/* round corner */
	LineJoinRound		= 2,	/* circular, smooth, circular arc */
	LineJoinMiterClipped	= 3	/* miter, sharp or beveled corner */
} GpLineJoin, LineJoin;

typedef enum  {
	MatrixOrderPrepend	= 0,
	MatrixOrderAppend	= 1
} GpMatrixOrder, MatrixOrder;

typedef enum {
	PenAlignmentCenter	= 0,
	PenAlignmentInset	= 1
} GpPenAlignment, PenAlignment;

typedef enum {
	HotkeyPrefixNone	= 0,
	HotkeyPrefixShow	= 1,
	HotkeyPrefixHide	= 2
} HotkeyPrefix;

typedef enum  {
	ImageTypeUnknown,
	ImageTypeBitmap,
	ImageTypeMetafile
} ImageType;

typedef enum {
	QualityModeInvalid	= -1,
	QualityModeDefault	= 0,
	QualityModeLow		= 1,
	QualityModeHigh		= 2
} QualityMode;

typedef enum {
	CompositingQualityInvalid	= QualityModeInvalid,
	CompositingQualityDefault	= QualityModeDefault,
	CompositingQualityHighSpeed	= QualityModeLow,
	CompositingQualityHighQuality	= QualityModeHigh,
	CompositingQualityGammaCorrected,
	CompositingQualityAssumeLinear
} CompositingQuality;

typedef enum {
	HatchStyleHorizontal			= 0,
	HatchStyleVertical			= 1,
	HatchStyleForwardDiagonal		= 2,
	HatchStyleBackwardDiagonal		= 3,
	HatchStyleCross				= 4,
	HatchStyleDiagonalCross			= 5,
	HatchStyle05Percent			= 6,
	HatchStyle10Percent			= 7,
	HatchStyle20Percent			= 8,
	HatchStyle25Percent			= 9,
	HatchStyle30Percent			= 10,
	HatchStyle40Percent			= 11,
	HatchStyle50Percent			= 12,
	HatchStyle60Percent			= 13,
	HatchStyle70Percent			= 14,
	HatchStyle75Percent			= 15,
	HatchStyle80Percent			= 16,
	HatchStyle90Percent			= 17,
	HatchStyleLightDownwardDiagonal		= 18,
	HatchStyleLightUpwardDiagonal		= 19,
	HatchStyleDarkDownwardDiagonal		= 20,
	HatchStyleDarkUpwardDiagonal		= 21,
	HatchStyleWideDownwardDiagonal		= 22,
	HatchStyleWideUpwardDiagonal		= 23,
	HatchStyleLightVertical			= 24,
	HatchStyleLightHorizontal		= 25,
	HatchStyleNarrowVertical		= 26,
	HatchStyleNarrowHorizontal		= 27,
	HatchStyleDarkVertical			= 28,
	HatchStyleDarkHorizontal		= 29,
	HatchStyleDashedDownwardDiagonal	= 30,
	HatchStyleDashedUpwardDiagonal		= 31,
	HatchStyleDashedHorizontal		= 32,
	HatchStyleDashedVertical		= 33,
	HatchStyleSmallConfetti			= 34,
	HatchStyleLargeConfetti			= 35,
	HatchStyleZigZag			= 36,
	HatchStyleWave				= 37,
	HatchStyleDiagonalBrick			= 38,
	HatchStyleHorizontalBrick		= 39,
	HatchStyleWeave				= 40,
	HatchStylePlaid				= 41,
	HatchStyleDivot				= 42,
	HatchStyleDottedGrid			= 43,
	HatchStyleDottedDiamond			= 44,
	HatchStyleShingle			= 45,
	HatchStyleTrellis			= 46,
	HatchStyleSphere			= 47,
	HatchStyleSmallGrid			= 48,
	HatchStyleSmallCheckerBoard		= 49,
	HatchStyleLargeCheckerBoard		= 50,
	HatchStyleOutlinedDiamond		= 51,
	HatchStyleSolidDiamond			= 52,
	HatchStyleLargeGrid			= HatchStyleCross,
	HatchStyleMin				= HatchStyleHorizontal,
	HatchStyleMax				= HatchStyleSolidDiamond
} GpHatchStyle, HatchStyle;

typedef enum {
	InterpolationModeInvalid	= QualityModeInvalid,
	InterpolationModeDefault	= QualityModeDefault,
	InterpolationModeLowQuality	= QualityModeLow,
	InterpolationModeHighQuality	= QualityModeHigh,
	InterpolationModeBilinear,
	InterpolationModeBicubic,
	InterpolationModeNearestNeighbor,
	InterpolationModeHighQualityBilinear,
	InterpolationModeHighQualityBicubic
} InterpolationMode;

typedef enum {
	PathPointTypeStart		= 0x00,
	PathPointTypeLine		= 0x01,
	PathPointTypeBezier		= 0x03,
	PathPointTypePathTypeMask	= 0x07,
	PathPointTypeDashMode		= 0x10,
	PathPointTypePathMarker		= 0x20,
	PathPointTypeCloseSubpath	= 0x80,
	PathPointTypeBezier3		= 0x03
} PathPointType;

typedef enum {
	PenTypeSolidColor	= BrushTypeSolidColor,
	PenTypeHatchFill	= BrushTypeHatchFill,
	PenTypeTextureFill	= BrushTypeTextureFill,
	PenTypePathGradient	= BrushTypePathGradient,
	PenTypeLinearGradient	= BrushTypeLinearGradient,
	PenTypeUnknown		= -1
} GpPenType, PenType;

typedef enum {
	PixelOffsetModeInvalid		= QualityModeInvalid,
	PixelOffsetModeDefault		= QualityModeDefault,
	PixelOffsetModeHighSpeed	= QualityModeLow,
	PixelOffsetModeHighQuality	= QualityModeHigh,
	PixelOffsetModeNone,
	PixelOffsetModeHalf
} PixelOffsetMode;

typedef enum {
	SmoothingModeInvalid		= QualityModeInvalid,
	SmoothingModeDefault		= QualityModeDefault,
	SmoothingModeHighSpeed		= QualityModeLow,
	SmoothingModeHighQuality	= QualityModeHigh,
	SmoothingModeNone,
	SmoothingModeAntiAlias,
#if (GDIPVER >= 0x0110)
	SmoothingModeAntiAlias8x4	= SmoothingModeAntiAlias,
	SmoothingModeAntiAlias8x8
#endif //(GDIPVER >= 0x0110) 
} SmoothingMode;

typedef enum {
	Ok				= 0,
	GenericError			= 1,
	InvalidParameter		= 2,
	OutOfMemory			= 3,
	ObjectBusy			= 4,
	InsufficientBuffer		= 5,
	NotImplemented			= 6,
	Win32Error			= 7,
	WrongState			= 8,
	Aborted				= 9,
	FileNotFound			= 10,
	ValueOverflow			= 11,
	AccessDenied			= 12,
	UnknownImageFormat		= 13,
	FontFamilyNotFound		= 14,
	FontStyleNotFound		= 15,
	NotTrueTypeFont			= 16,
	UnsupportedGdiplusVersion	= 17,
	GdiplusNotInitialized		= 18,
	PropertyNotFound		= 19,
	PropertyNotSupported		= 20
} GpStatus;

typedef enum {
	StringAlignmentNear	= 0,
	StringAlignmentCenter	= 1,
	StringAlignmentFar	= 2
} StringAlignment;

typedef enum {
	StringDigitSubstituteUser		= 0,
	StringDigitSubstituteNone		= 1,
	StringDigitSubstituteNational		= 2,
	StringDigitSubstituteTraditional	= 3
} StringDigitSubstitute;

typedef enum {
	StringFormatFlagsDirectionRightToLeft	= 0x00000001,
	StringFormatFlagsDirectionVertical	= 0x00000002,
	StringFormatFlagsNoFitBlackBox		= 0x00000004,
	StringFormatFlagsDisplayFormatControl	= 0x00000020,
	StringFormatFlagsNoFontFallback		= 0x00000400,
	StringFormatFlagsMeasureTrailingSpaces	= 0x00000800,
	StringFormatFlagsNoWrap			= 0x00001000,
	StringFormatFlagsLineLimit		= 0x00002000,
	StringFormatFlagsNoClip			= 0x00004000
} StringFormatFlags;

typedef enum {
	StringTrimmingNone		= 0,
	StringTrimmingCharacter		= 1,
	StringTrimmingWord		= 2,
	StringTrimmingEllipsisCharacter	= 3,
	StringTrimmingEllipsisWord	= 4,
	StringTrimmingEllipsisPath	= 5
} StringTrimming;

typedef enum {
	TextRenderingHintSystemDefault = 0,
	TextRenderingHintSingleBitPerPixelGridFit,
	TextRenderingHintSingleBitPerPixel,
	TextRenderingHintAntiAliasGridFit,
	TextRenderingHintAntiAlias,
	TextRenderingHintClearTypeGridFit
} TextRenderingHint;

typedef enum {
	UnitWorld	= 0,
	UnitDisplay	= 1,
	UnitPixel	= 2,
	UnitPoint	= 3,
	UnitInch	= 4,
	UnitDocument	= 5,
	UnitMillimeter	= 6,
	UnitCairoPoint	= 7
} GpUnit, Unit;

typedef enum {
	WarpModePerspective	= 0,
	WarpModeBilinear	= 1
} WarpMode;

typedef enum {
	WrapModeTile		= 0,
	WrapModeTileFlipX	= 1,
	WrapModeTileFlipY	= 2,
	WrapModeTileFlipXY	= 3,
	WrapModeClamp		= 4
} WrapMode, GpWrapMode;

typedef enum {
	EmfTypeEmfOnly        = MetafileTypeEmf,
	EmfTypeEmfPlusOnly    = MetafileTypeEmfPlusOnly,
	EmfTypeEmfPlusDual    = MetafileTypeEmfPlusDual
} EmfType;

typedef enum {
	MetafileFrameUnitPixel        = UnitPixel,
	MetafileFrameUnitPoint        = UnitPoint,
	MetafileFrameUnitInch        = UnitInch,
	MetafileFrameUnitDocument    = UnitDocument,
	MetafileFrameUnitMillimeter    = UnitMillimeter,
	MetafileFrameUnitGdi
} MetafileFrameUnit;

#define GDIP_EMFPLUS_RECORD_BASE        0x00004000
#define GDIP_WMF_RECORD_BASE            0x00010000
#define GDIP_WMF_RECORD_TO_EMFPLUS(n)   ((n) | GDIP_WMF_RECORD_BASE)
#define GDIP_EMFPLUS_RECORD_TO_WMF(n)   ((n) & (~GDIP_WMF_RECORD_BASE))
#define GDIP_IS_WMF_RECORDTYPE(n)       (((n) & GDIP_WMF_RECORD_BASE) != 0)

typedef enum {
	WmfRecordTypeSetBkColor              = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETBKCOLOR),
	WmfRecordTypeSetBkMode               = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETBKMODE),
	WmfRecordTypeSetMapMode              = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETMAPMODE),
	WmfRecordTypeSetROP2                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETROP2),
	WmfRecordTypeSetRelAbs               = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETRELABS),
	WmfRecordTypeSetPolyFillMode         = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETPOLYFILLMODE),
	WmfRecordTypeSetStretchBltMode       = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETSTRETCHBLTMODE),
	WmfRecordTypeSetTextCharExtra        = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTCHAREXTRA),
	WmfRecordTypeSetTextColor            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTCOLOR),
	WmfRecordTypeSetTextJustification    = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTJUSTIFICATION),
	WmfRecordTypeSetWindowOrg            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETWINDOWORG),
	WmfRecordTypeSetWindowExt            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETWINDOWEXT),
	WmfRecordTypeSetViewportOrg          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETVIEWPORTORG),
	WmfRecordTypeSetViewportExt          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETVIEWPORTEXT),
	WmfRecordTypeOffsetWindowOrg         = GDIP_WMF_RECORD_TO_EMFPLUS(META_OFFSETWINDOWORG),
	WmfRecordTypeScaleWindowExt          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SCALEWINDOWEXT),
	WmfRecordTypeOffsetViewportOrg       = GDIP_WMF_RECORD_TO_EMFPLUS(META_OFFSETVIEWPORTORG),
	WmfRecordTypeScaleViewportExt        = GDIP_WMF_RECORD_TO_EMFPLUS(META_SCALEVIEWPORTEXT),
	WmfRecordTypeLineTo                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_LINETO),
	WmfRecordTypeMoveTo                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_MOVETO),
	WmfRecordTypeExcludeClipRect         = GDIP_WMF_RECORD_TO_EMFPLUS(META_EXCLUDECLIPRECT),
	WmfRecordTypeIntersectClipRect       = GDIP_WMF_RECORD_TO_EMFPLUS(META_INTERSECTCLIPRECT),
	WmfRecordTypeArc                     = GDIP_WMF_RECORD_TO_EMFPLUS(META_ARC),
	WmfRecordTypeEllipse                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_ELLIPSE),
	WmfRecordTypeFloodFill               = GDIP_WMF_RECORD_TO_EMFPLUS(META_FLOODFILL),
	WmfRecordTypePie                     = GDIP_WMF_RECORD_TO_EMFPLUS(META_PIE),
	WmfRecordTypeRectangle               = GDIP_WMF_RECORD_TO_EMFPLUS(META_RECTANGLE),
	WmfRecordTypeRoundRect               = GDIP_WMF_RECORD_TO_EMFPLUS(META_ROUNDRECT),
	WmfRecordTypePatBlt                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_PATBLT),
	WmfRecordTypeSaveDC                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_SAVEDC),
	WmfRecordTypeSetPixel                = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETPIXEL),
	WmfRecordTypeOffsetClipRgn           = GDIP_WMF_RECORD_TO_EMFPLUS(META_OFFSETCLIPRGN),
	WmfRecordTypeTextOut                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_TEXTOUT),
	WmfRecordTypeBitBlt                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_BITBLT),
	WmfRecordTypeStretchBlt              = GDIP_WMF_RECORD_TO_EMFPLUS(META_STRETCHBLT),
	WmfRecordTypePolygon                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_POLYGON),
	WmfRecordTypePolyline                = GDIP_WMF_RECORD_TO_EMFPLUS(META_POLYLINE),
	WmfRecordTypeEscape                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_ESCAPE),
	WmfRecordTypeRestoreDC               = GDIP_WMF_RECORD_TO_EMFPLUS(META_RESTOREDC),
	WmfRecordTypeFillRegion              = GDIP_WMF_RECORD_TO_EMFPLUS(META_FILLREGION),
	WmfRecordTypeFrameRegion             = GDIP_WMF_RECORD_TO_EMFPLUS(META_FRAMEREGION),
	WmfRecordTypeInvertRegion            = GDIP_WMF_RECORD_TO_EMFPLUS(META_INVERTREGION),
	WmfRecordTypePaintRegion             = GDIP_WMF_RECORD_TO_EMFPLUS(META_PAINTREGION),
	WmfRecordTypeSelectClipRegion        = GDIP_WMF_RECORD_TO_EMFPLUS(META_SELECTCLIPREGION),
	WmfRecordTypeSelectObject            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SELECTOBJECT),
	WmfRecordTypeSetTextAlign            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTALIGN),
	WmfRecordTypeDrawText                = GDIP_WMF_RECORD_TO_EMFPLUS(0x062F),  // META_DRAWTEXT
	WmfRecordTypeChord                   = GDIP_WMF_RECORD_TO_EMFPLUS(META_CHORD),
	WmfRecordTypeSetMapperFlags          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETMAPPERFLAGS),
	WmfRecordTypeExtTextOut              = GDIP_WMF_RECORD_TO_EMFPLUS(META_EXTTEXTOUT),
	WmfRecordTypeSetDIBToDev             = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETDIBTODEV),
	WmfRecordTypeSelectPalette           = GDIP_WMF_RECORD_TO_EMFPLUS(META_SELECTPALETTE),
	WmfRecordTypeRealizePalette          = GDIP_WMF_RECORD_TO_EMFPLUS(META_REALIZEPALETTE),
	WmfRecordTypeAnimatePalette          = GDIP_WMF_RECORD_TO_EMFPLUS(META_ANIMATEPALETTE),
	WmfRecordTypeSetPalEntries           = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETPALENTRIES),
	WmfRecordTypePolyPolygon             = GDIP_WMF_RECORD_TO_EMFPLUS(META_POLYPOLYGON),
	WmfRecordTypeResizePalette           = GDIP_WMF_RECORD_TO_EMFPLUS(META_RESIZEPALETTE),
	WmfRecordTypeDIBBitBlt               = GDIP_WMF_RECORD_TO_EMFPLUS(META_DIBBITBLT),
	WmfRecordTypeDIBStretchBlt           = GDIP_WMF_RECORD_TO_EMFPLUS(META_DIBSTRETCHBLT),
	WmfRecordTypeDIBCreatePatternBrush   = GDIP_WMF_RECORD_TO_EMFPLUS(META_DIBCREATEPATTERNBRUSH),
	WmfRecordTypeStretchDIB              = GDIP_WMF_RECORD_TO_EMFPLUS(META_STRETCHDIB),
	WmfRecordTypeExtFloodFill            = GDIP_WMF_RECORD_TO_EMFPLUS(META_EXTFLOODFILL),
	WmfRecordTypeSetLayout               = GDIP_WMF_RECORD_TO_EMFPLUS(0x0149),  // META_SETLAYOUT
	WmfRecordTypeResetDC                 = GDIP_WMF_RECORD_TO_EMFPLUS(0x014C),  // META_RESETDC
	WmfRecordTypeStartDoc                = GDIP_WMF_RECORD_TO_EMFPLUS(0x014D),  // META_STARTDOC
	WmfRecordTypeStartPage               = GDIP_WMF_RECORD_TO_EMFPLUS(0x004F),  // META_STARTPAGE
	WmfRecordTypeEndPage                 = GDIP_WMF_RECORD_TO_EMFPLUS(0x0050),  // META_ENDPAGE
	WmfRecordTypeAbortDoc                = GDIP_WMF_RECORD_TO_EMFPLUS(0x0052),  // META_ABORTDOC
	WmfRecordTypeEndDoc                  = GDIP_WMF_RECORD_TO_EMFPLUS(0x005E),  // META_ENDDOC
	WmfRecordTypeDeleteObject            = GDIP_WMF_RECORD_TO_EMFPLUS(META_DELETEOBJECT),
	WmfRecordTypeCreatePalette           = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEPALETTE),
	WmfRecordTypeCreateBrush             = GDIP_WMF_RECORD_TO_EMFPLUS(0x00F8),  // META_CREATEBRUSH
	WmfRecordTypeCreatePatternBrush      = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEPATTERNBRUSH),
	WmfRecordTypeCreatePenIndirect       = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEPENINDIRECT),
	WmfRecordTypeCreateFontIndirect      = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEFONTINDIRECT),
	WmfRecordTypeCreateBrushIndirect     = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEBRUSHINDIRECT),
	WmfRecordTypeCreateBitmapIndirect    = GDIP_WMF_RECORD_TO_EMFPLUS(0x02FD),  // META_CREATEBITMAPINDIRECT
	WmfRecordTypeCreateBitmap            = GDIP_WMF_RECORD_TO_EMFPLUS(0x06FE),  // META_CREATEBITMAP
	WmfRecordTypeCreateRegion            = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEREGION),

	EmfRecordTypeHeader                  = EMR_HEADER,
	EmfRecordTypePolyBezier              = EMR_POLYBEZIER,
	EmfRecordTypePolygon                 = EMR_POLYGON,
	EmfRecordTypePolyline                = EMR_POLYLINE,
	EmfRecordTypePolyBezierTo            = EMR_POLYBEZIERTO,
	EmfRecordTypePolyLineTo              = EMR_POLYLINETO,
	EmfRecordTypePolyPolyline            = EMR_POLYPOLYLINE,
	EmfRecordTypePolyPolygon             = EMR_POLYPOLYGON,
	EmfRecordTypeSetWindowExtEx          = EMR_SETWINDOWEXTEX,
	EmfRecordTypeSetWindowOrgEx          = EMR_SETWINDOWORGEX,
	EmfRecordTypeSetViewportExtEx        = EMR_SETVIEWPORTEXTEX,
	EmfRecordTypeSetViewportOrgEx        = EMR_SETVIEWPORTORGEX,
	EmfRecordTypeSetBrushOrgEx           = EMR_SETBRUSHORGEX,
	EmfRecordTypeEOF                     = EMR_EOF,
	EmfRecordTypeSetPixelV               = EMR_SETPIXELV,
	EmfRecordTypeSetMapperFlags          = EMR_SETMAPPERFLAGS,
	EmfRecordTypeSetMapMode              = EMR_SETMAPMODE,
	EmfRecordTypeSetBkMode               = EMR_SETBKMODE,
	EmfRecordTypeSetPolyFillMode         = EMR_SETPOLYFILLMODE,
	EmfRecordTypeSetROP2                 = EMR_SETROP2,
	EmfRecordTypeSetStretchBltMode       = EMR_SETSTRETCHBLTMODE,
	EmfRecordTypeSetTextAlign            = EMR_SETTEXTALIGN,
	EmfRecordTypeSetColorAdjustment      = EMR_SETCOLORADJUSTMENT,
	EmfRecordTypeSetTextColor            = EMR_SETTEXTCOLOR,
	EmfRecordTypeSetBkColor              = EMR_SETBKCOLOR,
	EmfRecordTypeOffsetClipRgn           = EMR_OFFSETCLIPRGN,
	EmfRecordTypeMoveToEx                = EMR_MOVETOEX,
	EmfRecordTypeSetMetaRgn              = EMR_SETMETARGN,
	EmfRecordTypeExcludeClipRect         = EMR_EXCLUDECLIPRECT,
	EmfRecordTypeIntersectClipRect       = EMR_INTERSECTCLIPRECT,
	EmfRecordTypeScaleViewportExtEx      = EMR_SCALEVIEWPORTEXTEX,
	EmfRecordTypeScaleWindowExtEx        = EMR_SCALEWINDOWEXTEX,
	EmfRecordTypeSaveDC                  = EMR_SAVEDC,
	EmfRecordTypeRestoreDC               = EMR_RESTOREDC,
	EmfRecordTypeSetWorldTransform       = EMR_SETWORLDTRANSFORM,
	EmfRecordTypeModifyWorldTransform    = EMR_MODIFYWORLDTRANSFORM,
	EmfRecordTypeSelectObject            = EMR_SELECTOBJECT,
	EmfRecordTypeCreatePen               = EMR_CREATEPEN,
	EmfRecordTypeCreateBrushIndirect     = EMR_CREATEBRUSHINDIRECT,
	EmfRecordTypeDeleteObject            = EMR_DELETEOBJECT,
	EmfRecordTypeAngleArc                = EMR_ANGLEARC,
	EmfRecordTypeEllipse                 = EMR_ELLIPSE,
	EmfRecordTypeRectangle               = EMR_RECTANGLE,
	EmfRecordTypeRoundRect               = EMR_ROUNDRECT,
	EmfRecordTypeArc                     = EMR_ARC,
	EmfRecordTypeChord                   = EMR_CHORD,
	EmfRecordTypePie                     = EMR_PIE,
	EmfRecordTypeSelectPalette           = EMR_SELECTPALETTE,
	EmfRecordTypeCreatePalette           = EMR_CREATEPALETTE,
	EmfRecordTypeSetPaletteEntries       = EMR_SETPALETTEENTRIES,
	EmfRecordTypeResizePalette           = EMR_RESIZEPALETTE,
	EmfRecordTypeRealizePalette          = EMR_REALIZEPALETTE,
	EmfRecordTypeExtFloodFill            = EMR_EXTFLOODFILL,
	EmfRecordTypeLineTo                  = EMR_LINETO,
	EmfRecordTypeArcTo                   = EMR_ARCTO,
	EmfRecordTypePolyDraw                = EMR_POLYDRAW,
	EmfRecordTypeSetArcDirection         = EMR_SETARCDIRECTION,
	EmfRecordTypeSetMiterLimit           = EMR_SETMITERLIMIT,
	EmfRecordTypeBeginPath               = EMR_BEGINPATH,
	EmfRecordTypeEndPath                 = EMR_ENDPATH,
	EmfRecordTypeCloseFigure             = EMR_CLOSEFIGURE,
	EmfRecordTypeFillPath                = EMR_FILLPATH,
	EmfRecordTypeStrokeAndFillPath       = EMR_STROKEANDFILLPATH,
	EmfRecordTypeStrokePath              = EMR_STROKEPATH,
	EmfRecordTypeFlattenPath             = EMR_FLATTENPATH,
	EmfRecordTypeWidenPath               = EMR_WIDENPATH,
	EmfRecordTypeSelectClipPath          = EMR_SELECTCLIPPATH,
	EmfRecordTypeAbortPath               = EMR_ABORTPATH,
	EmfRecordTypeReserved_069            = 69,  // Not Used
	EmfRecordTypeGdiComment              = EMR_GDICOMMENT,
	EmfRecordTypeFillRgn                 = EMR_FILLRGN,
	EmfRecordTypeFrameRgn                = EMR_FRAMERGN,
	EmfRecordTypeInvertRgn               = EMR_INVERTRGN,
	EmfRecordTypePaintRgn                = EMR_PAINTRGN,
	EmfRecordTypeExtSelectClipRgn        = EMR_EXTSELECTCLIPRGN,
	EmfRecordTypeBitBlt                  = EMR_BITBLT,
	EmfRecordTypeStretchBlt              = EMR_STRETCHBLT,
	EmfRecordTypeMaskBlt                 = EMR_MASKBLT,
	EmfRecordTypePlgBlt                  = EMR_PLGBLT,
	EmfRecordTypeSetDIBitsToDevice       = EMR_SETDIBITSTODEVICE,
	EmfRecordTypeStretchDIBits           = EMR_STRETCHDIBITS,
	EmfRecordTypeExtCreateFontIndirect   = EMR_EXTCREATEFONTINDIRECTW,
	EmfRecordTypeExtTextOutA             = EMR_EXTTEXTOUTA,
	EmfRecordTypeExtTextOutW             = EMR_EXTTEXTOUTW,
	EmfRecordTypePolyBezier16            = EMR_POLYBEZIER16,
	EmfRecordTypePolygon16               = EMR_POLYGON16,
	EmfRecordTypePolyline16              = EMR_POLYLINE16,
	EmfRecordTypePolyBezierTo16          = EMR_POLYBEZIERTO16,
	EmfRecordTypePolylineTo16            = EMR_POLYLINETO16,
	EmfRecordTypePolyPolyline16          = EMR_POLYPOLYLINE16,
	EmfRecordTypePolyPolygon16           = EMR_POLYPOLYGON16,
	EmfRecordTypePolyDraw16              = EMR_POLYDRAW16,
	EmfRecordTypeCreateMonoBrush         = EMR_CREATEMONOBRUSH,
	EmfRecordTypeCreateDIBPatternBrushPt = EMR_CREATEDIBPATTERNBRUSHPT,
	EmfRecordTypeExtCreatePen            = EMR_EXTCREATEPEN,
	EmfRecordTypePolyTextOutA            = EMR_POLYTEXTOUTA,
	EmfRecordTypePolyTextOutW            = EMR_POLYTEXTOUTW,
	EmfRecordTypeSetICMMode              = 98,  // EMR_SETICMMODE,
	EmfRecordTypeCreateColorSpace        = 99,  // EMR_CREATECOLORSPACE,
	EmfRecordTypeSetColorSpace           = 100, // EMR_SETCOLORSPACE,
	EmfRecordTypeDeleteColorSpace        = 101, // EMR_DELETECOLORSPACE,
	EmfRecordTypeGLSRecord               = 102, // EMR_GLSRECORD,
	EmfRecordTypeGLSBoundedRecord        = 103, // EMR_GLSBOUNDEDRECORD,
	EmfRecordTypePixelFormat             = 104, // EMR_PIXELFORMAT,
	EmfRecordTypeDrawEscape              = 105, // EMR_RESERVED_105,
	EmfRecordTypeExtEscape               = 106, // EMR_RESERVED_106,
	EmfRecordTypeStartDoc                = 107, // EMR_RESERVED_107,
	EmfRecordTypeSmallTextOut            = 108, // EMR_RESERVED_108,
	EmfRecordTypeForceUFIMapping         = 109, // EMR_RESERVED_109,
	EmfRecordTypeNamedEscape             = 110, // EMR_RESERVED_110,
	EmfRecordTypeColorCorrectPalette     = 111, // EMR_COLORCORRECTPALETTE,
	EmfRecordTypeSetICMProfileA          = 112, // EMR_SETICMPROFILEA,
	EmfRecordTypeSetICMProfileW          = 113, // EMR_SETICMPROFILEW,
	EmfRecordTypeAlphaBlend              = 114, // EMR_ALPHABLEND,
	EmfRecordTypeSetLayout               = 115, // EMR_SETLAYOUT,
	EmfRecordTypeTransparentBlt          = 116, // EMR_TRANSPARENTBLT,
	EmfRecordTypeReserved_117            = 117, // Not Used
	EmfRecordTypeGradientFill            = 118, // EMR_GRADIENTFILL,
	EmfRecordTypeSetLinkedUFIs           = 119, // EMR_RESERVED_119,
	EmfRecordTypeSetTextJustification    = 120, // EMR_RESERVED_120,
	EmfRecordTypeColorMatchToTargetW     = 121, // EMR_COLORMATCHTOTARGETW,
	EmfRecordTypeCreateColorSpaceW       = 122, // EMR_CREATECOLORSPACEW,
	EmfRecordTypeMax                     = 122,
	EmfRecordTypeMin                     = 1,

	// That is the END of the GDI EMF records.

	// Now we start the list of EMF+ records.  We leave quite
	// a bit of room here for the addition of any new GDI
	// records that may be added later.

	EmfPlusRecordTypeInvalid = GDIP_EMFPLUS_RECORD_BASE,
	EmfPlusRecordTypeHeader,
	EmfPlusRecordTypeEndOfFile,

	EmfPlusRecordTypeComment,

	EmfPlusRecordTypeGetDC,

	EmfPlusRecordTypeMultiFormatStart,
	EmfPlusRecordTypeMultiFormatSection,
	EmfPlusRecordTypeMultiFormatEnd,

	// For all persistent objects
	
	EmfPlusRecordTypeObject,

	// Drawing Records
	
	EmfPlusRecordTypeClear,
	EmfPlusRecordTypeFillRects,
	EmfPlusRecordTypeDrawRects,
	EmfPlusRecordTypeFillPolygon,
	EmfPlusRecordTypeDrawLines,
	EmfPlusRecordTypeFillEllipse,
	EmfPlusRecordTypeDrawEllipse,
	EmfPlusRecordTypeFillPie,
	EmfPlusRecordTypeDrawPie,
	EmfPlusRecordTypeDrawArc,
	EmfPlusRecordTypeFillRegion,
	EmfPlusRecordTypeFillPath,
	EmfPlusRecordTypeDrawPath,
	EmfPlusRecordTypeFillClosedCurve,
	EmfPlusRecordTypeDrawClosedCurve,
	EmfPlusRecordTypeDrawCurve,
	EmfPlusRecordTypeDrawBeziers,
	EmfPlusRecordTypeDrawImage,
	EmfPlusRecordTypeDrawImagePoints,
	EmfPlusRecordTypeDrawString,

	// Graphics State Records
	
	EmfPlusRecordTypeSetRenderingOrigin,
	EmfPlusRecordTypeSetAntiAliasMode,
	EmfPlusRecordTypeSetTextRenderingHint,
	EmfPlusRecordTypeSetTextContrast,
	EmfPlusRecordTypeSetInterpolationMode,
	EmfPlusRecordTypeSetPixelOffsetMode,
	EmfPlusRecordTypeSetCompositingMode,
	EmfPlusRecordTypeSetCompositingQuality,
	EmfPlusRecordTypeSave,
	EmfPlusRecordTypeRestore,
	EmfPlusRecordTypeBeginContainer,
	EmfPlusRecordTypeBeginContainerNoParams,
	EmfPlusRecordTypeEndContainer,
	EmfPlusRecordTypeSetWorldTransform,
	EmfPlusRecordTypeResetWorldTransform,
	EmfPlusRecordTypeMultiplyWorldTransform,
	EmfPlusRecordTypeTranslateWorldTransform,
	EmfPlusRecordTypeScaleWorldTransform,
	EmfPlusRecordTypeRotateWorldTransform,
	EmfPlusRecordTypeSetPageTransform,
	EmfPlusRecordTypeResetClip,
	EmfPlusRecordTypeSetClipRect,
	EmfPlusRecordTypeSetClipPath,
	EmfPlusRecordTypeSetClipRegion,
	EmfPlusRecordTypeOffsetClip,

	EmfPlusRecordTypeDrawDriverString,
#if (GDIPVER >= 0x0110)
	EmfPlusRecordTypeStrokeFillPath,
	EmfPlusRecordTypeSerializableObject,

	EmfPlusRecordTypeSetTSGraphics,
	EmfPlusRecordTypeSetTSClip,
#endif

	EmfPlusRecordTotal,

	EmfPlusRecordTypeMax = EmfPlusRecordTotal-1,
	EmfPlusRecordTypeMin = EmfPlusRecordTypeHeader
} EmfPlusRecordType;

#endif
