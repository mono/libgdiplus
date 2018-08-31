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

typedef enum {
	ImageFlagsNone			= 0,
	ImageFlagsScalable		= 1,
	ImageFlagsHasAlpha		= 2,
	ImageFlagsHasTranslucent	= 4,
	ImageFlagsPartiallyScalable	= 8,
	ImageFlagsColorSpaceRGB		= 16,
	ImageFlagsColorSpaceCMYK	= 32,
	ImageFlagsColorSpaceGRAY	= 64,
	ImageFlagsColorSpaceYCBCR	= 128,
	ImageFlagsColorSpaceYCCK	= 256,
	ImageFlagsHasRealDPI		= 4096,
	ImageFlagsHasRealPixelSize	= 8192,
	ImageFlagsReadOnly		= 65536,
	ImageFlagsCaching		= 131072,
	ImageFlagsUndocumented		= 0x00040000
} ImageFlags;

typedef enum {
	ImageLockModeRead		= 1,
	ImageLockModeWrite		= 2,
	ImageLockModeUserInputBuf	= 4
} ImageLockMode;

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
	ImageCodecFlagsEncoder		= 1,
	ImageCodecFlagsDecoder		= 2,
	ImageCodecFlagsSupportBitmap	= 4,
	ImageCodecFlagsSupportVector	= 8,
	ImageCodecFlagsSeekableEncode	= 16,
	ImageCodecFlagsBlockingDecode	= 32,
	ImageCodecFlagsBuiltin		= 65536,
	ImageCodecFlagsSystem		= 131072,
	ImageCodecFlagsUser		= 262144
} ImageCodecFlags;

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
	PaletteFlagsHasAlpha	= 0x0001,
	PaletteFlagsGrayScale	= 0x0002,
	PaletteFlagsHalftone	= 0x0004
} PaletteFlags;

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

/* The pixel format spec is:
 * [0-7 format index] [8-15 pixel size, bits] [16-23 flags] [24-31 reserved]
 * so,
 * (ID | (bpp << 8) | flags)
 */
typedef enum {
	PixelFormatAlpha		= 0x00040000,	/* flag: format has alpha */
	PixelFormatCanonical		= 0x00200000,	/* flag: unknown */
	PixelFormatExtended		= 0x00100000,	/* flag: 16 bits per channel (16bpp grayscale and 48/64bpp rgb) */
	PixelFormatGDI			= 0x00020000,	/* flag: supported by GDI */
	PixelFormatIndexed		= 0x00010000,	/* flag: is palette-indexed */
	PixelFormatPAlpha		= 0x00080000,	/* flag: alpha is pre-multiplied */

	PixelFormat16bppARGB1555	= 0x00061007,
	PixelFormat16bppGrayScale	= 0x00101004,
	PixelFormat16bppRGB555		= 0x00021005,
	PixelFormat16bppRGB565		= 0x00021006,
	PixelFormat1bppIndexed		= 0x00030101,
	PixelFormat24bppRGB		= 0x00021808,
	PixelFormat32bppARGB		= 0x0026200a,
	PixelFormat32bppPARGB		= 0x000e200b,
	PixelFormat32bppRGB		= 0x00022009,
	PixelFormat48bppRGB		= 0x0010300c,
	PixelFormat4bppIndexed		= 0x00030402,
	PixelFormat64bppARGB		= 0x0034400d,
	PixelFormat64bppPARGB		= 0x001c400e,
	PixelFormat8bppIndexed		= 0x00030803,
	PixelFormat32bppCMYK		= 0x0000200F,

	PixelFormatUndefined		= 0,
	PixelFormatDontCare		= 0,

	PixelFormatMax = 16
} PixelFormat;

typedef enum {
	PixelOffsetModeInvalid		= QualityModeInvalid,
	PixelOffsetModeDefault		= QualityModeDefault,
	PixelOffsetModeHighSpeed	= QualityModeLow,
	PixelOffsetModeHighQuality	= QualityModeHigh,
	PixelOffsetModeNone,
	PixelOffsetModeHalf
} PixelOffsetMode;

typedef enum {
	PropertyTagExifIFD             		= 0x8769,
	PropertyTagGpsIFD              		= 0x8825,

	PropertyTagNewSubfileType      		= 0x00FE,
	PropertyTagSubfileType         		= 0x00FF,
	PropertyTagImageWidth          		= 0x0100,
	PropertyTagImageHeight         		= 0x0101,
	PropertyTagBitsPerSample       		= 0x0102,
	PropertyTagCompression         		= 0x0103,
	PropertyTagPhotometricInterp   		= 0x0106,
	PropertyTagThreshHolding       		= 0x0107,
	PropertyTagCellWidth           		= 0x0108,
	PropertyTagCellHeight          		= 0x0109,
	PropertyTagFillOrder           		= 0x010A,
	PropertyTagDocumentName        		= 0x010D,
	PropertyTagImageDescription    		= 0x010E,
	PropertyTagEquipMake           		= 0x010F,
	PropertyTagEquipModel          		= 0x0110,
	PropertyTagStripOffsets        		= 0x0111,
	PropertyTagOrientation         		= 0x0112,
	PropertyTagSamplesPerPixel     		= 0x0115,
	PropertyTagRowsPerStrip        		= 0x0116,
	PropertyTagStripBytesCount     		= 0x0117,
	PropertyTagMinSampleValue      		= 0x0118,
	PropertyTagMaxSampleValue      		= 0x0119,
	PropertyTagXResolution         		= 0x011A,
	PropertyTagYResolution         		= 0x011B,
	PropertyTagPlanarConfig        		= 0x011C,
	PropertyTagPageName            		= 0x011D,
	PropertyTagXPosition           		= 0x011E,
	PropertyTagYPosition           		= 0x011F,
	PropertyTagFreeOffset          		= 0x0120,
	PropertyTagFreeByteCounts      		= 0x0121,
	PropertyTagGrayResponseUnit    		= 0x0122,
	PropertyTagGrayResponseCurve   		= 0x0123,
	PropertyTagT4Option            		= 0x0124,
	PropertyTagT6Option            		= 0x0125,
	PropertyTagResolutionUnit      		= 0x0128,
	PropertyTagPageNumber          		= 0x0129,
	PropertyTagTransferFunction   		= 0x012D,
	PropertyTagSoftwareUsed        		= 0x0131,
	PropertyTagDateTime            		= 0x0132,
	PropertyTagArtist              		= 0x013B,
	PropertyTagHostComputer        		= 0x013C,
	PropertyTagPredictor           		= 0x013D,
	PropertyTagWhitePoint          		= 0x013E,
	PropertyTagPrimaryChromaticities	= 0x013F,
	PropertyTagColorMap            		= 0x0140,
	PropertyTagHalftoneHints       		= 0x0141,
	PropertyTagTileWidth           		= 0x0142,
	PropertyTagTileLength          		= 0x0143,
	PropertyTagTileOffset          		= 0x0144,
	PropertyTagTileByteCounts      		= 0x0145,
	PropertyTagInkSet              		= 0x014C,
	PropertyTagInkNames            		= 0x014D,
	PropertyTagNumberOfInks        		= 0x014E,
	PropertyTagDotRange            		= 0x0150,
	PropertyTagTargetPrinter       		= 0x0151,
	PropertyTagExtraSamples        		= 0x0152,
	PropertyTagSampleFormat        		= 0x0153,
	PropertyTagSMinSampleValue     		= 0x0154,
	PropertyTagSMaxSampleValue     		= 0x0155,
	PropertyTagTransferRange       		= 0x0156,

	PropertyTagJPEGProc            		= 0x0200,
	PropertyTagJPEGInterFormat     		= 0x0201,
	PropertyTagJPEGInterLength     		= 0x0202,
	PropertyTagJPEGRestartInterval 		= 0x0203,
	PropertyTagJPEGLosslessPredictors	= 0x0205,
	PropertyTagJPEGPointTransforms		= 0x0206,
	PropertyTagJPEGQTables         		= 0x0207,
	PropertyTagJPEGDCTables        		= 0x0208,
	PropertyTagJPEGACTables        		= 0x0209,

	PropertyTagYCbCrCoefficients   		= 0x0211,
	PropertyTagYCbCrSubsampling    		= 0x0212,
	PropertyTagYCbCrPositioning    		= 0x0213,
	PropertyTagREFBlackWhite       		= 0x0214,

	PropertyTagICCProfile          		= 0x8773,
	
	PropertyTagGamma               		= 0x0301,
	PropertyTagICCProfileDescriptor		= 0x0302,
	PropertyTagSRGBRenderingIntent 		= 0x0303,

	PropertyTagImageTitle          		= 0x0320,
	PropertyTagCopyright           		= 0x8298,

	// Other tags (Adobe Image Info, etc)
	PropertyTagResolutionXUnit		= 0x5001,
	PropertyTagResolutionYUnit           	= 0x5002,
	PropertyTagResolutionXLengthUnit     	= 0x5003,
	PropertyTagResolutionYLengthUnit     	= 0x5004,
	PropertyTagPrintFlags                	= 0x5005,
	PropertyTagPrintFlagsVersion         	= 0x5006,
	PropertyTagPrintFlagsCrop            	= 0x5007,
	PropertyTagPrintFlagsBleedWidth      	= 0x5008,
	PropertyTagPrintFlagsBleedWidthScale 	= 0x5009,
	PropertyTagHalftoneLPI               	= 0x500A,
	PropertyTagHalftoneLPIUnit           	= 0x500B,
	PropertyTagHalftoneDegree            	= 0x500C,
	PropertyTagHalftoneShape             	= 0x500D,
	PropertyTagHalftoneMisc              	= 0x500E,
	PropertyTagHalftoneScreen            	= 0x500F,
	PropertyTagJPEGQuality               	= 0x5010,
	PropertyTagGridSize                  	= 0x5011,
	PropertyTagThumbnailFormat          	= 0x5012,  // 0 = RAW RGB, 1 = JPEG
	PropertyTagThumbnailWidth            	= 0x5013,
	PropertyTagThumbnailHeight           	= 0x5014,
	PropertyTagThumbnailColorDepth       	= 0x5015,
	PropertyTagThumbnailPlanes           	= 0x5016,
	PropertyTagThumbnailRawBytes         	= 0x5017,
	PropertyTagThumbnailSize             	= 0x5018,
	PropertyTagThumbnailCompressedSize   	= 0x5019,
	PropertyTagColorTransferFunction     	= 0x501A,
	PropertyTagThumbnailData             	= 0x501B, // embeded image, format depends on ThumbnailFormat
	PropertyTagThumbnailImageWidth       	= 0x5020,
	PropertyTagThumbnailImageHeight      	= 0x5021,
	PropertyTagThumbnailBitsPerSample    	= 0x5022,
	PropertyTagThumbnailCompression      	= 0x5023,
	PropertyTagThumbnailPhotometricInterp 	= 0x5024,
	PropertyTagThumbnailImageDescription 	= 0x5025,
	PropertyTagThumbnailEquipMake        	= 0x5026,
	PropertyTagThumbnailEquipModel       	= 0x5027,
	PropertyTagThumbnailStripOffsets     	= 0x5028,
	PropertyTagThumbnailOrientation      	= 0x5029,
	PropertyTagThumbnailSamplesPerPixel  	= 0x502A,
	PropertyTagThumbnailRowsPerStrip     	= 0x502B,
	PropertyTagThumbnailStripBytesCount  	= 0x502C,
	PropertyTagThumbnailResolutionX      	= 0x502D,
	PropertyTagThumbnailResolutionY      	= 0x502E,
	PropertyTagThumbnailPlanarConfig     	= 0x502F,
	PropertyTagThumbnailResolutionUnit   	= 0x5030,
	PropertyTagThumbnailTransferFunction 	= 0x5031,
	PropertyTagThumbnailSoftwareUsed     	= 0x5032,
	PropertyTagThumbnailDateTime         	= 0x5033,
	PropertyTagThumbnailArtist           	= 0x5034,
	PropertyTagThumbnailWhitePoint       	= 0x5035,
	PropertyTagThumbnailPrimaryChromaticities = 0x5036,
	PropertyTagThumbnailYCbCrCoefficients 	= 0x5037,
	PropertyTagThumbnailYCbCrSubsampling 	= 0x5038,
	PropertyTagThumbnailYCbCrPositioning 	= 0x5039,
	PropertyTagThumbnailRefBlackWhite    	= 0x503A,
	PropertyTagThumbnailCopyRight        	= 0x503B,
	PropertyTagLuminanceTable            	= 0x5090,
	PropertyTagChrominanceTable          	= 0x5091,

	PropertyTagFrameDelay                	= 0x5100,
	PropertyTagLoopCount                 	= 0x5101,

	PropertyTagPixelUnit         		= 0x5110,
	PropertyTagPixelPerUnitX     		= 0x5111,
	PropertyTagPixelPerUnitY     		= 0x5112,
	PropertyTagPaletteHistogram  		= 0x5113,

	// EXIF specific tag
	PropertyTagExifExposureTime  		= 0x829A,
	PropertyTagExifFNumber       		= 0x829D,

	PropertyTagExifExposureProg  		= 0x8822,
	PropertyTagExifSpectralSense 		= 0x8824,
	PropertyTagExifISOSpeed      		= 0x8827,
	PropertyTagExifOECF          		= 0x8828,

	PropertyTagExifVer            		= 0x9000,
	PropertyTagExifDTOrig         		= 0x9003,
	PropertyTagExifDTDigitized    		= 0x9004,

	PropertyTagExifCompConfig     		= 0x9101,
	PropertyTagExifCompBPP        		= 0x9102,

	PropertyTagExifShutterSpeed   		= 0x9201,
	PropertyTagExifAperture       		= 0x9202,
	PropertyTagExifBrightness     		= 0x9203,
	PropertyTagExifExposureBias		= 0x9204,
	PropertyTagExifMaxAperture		= 0x9205,
	PropertyTagExifSubjectDist		= 0x9206,
	PropertyTagExifMeteringMode	 	= 0x9207,
	PropertyTagExifLightSource	 	= 0x9208,
	PropertyTagExifFlash		 	= 0x9209,
	PropertyTagExifFocalLength	 	= 0x920A,
	PropertyTagExifMakerNote	 	= 0x927C,
	PropertyTagExifUserComment		= 0x9286,
	PropertyTagExifDTSubsec		 	= 0x9290,
	PropertyTagExifDTOrigSS		 	= 0x9291,
	PropertyTagExifDTDigSS		 	= 0x9292,

	PropertyTagExifFPXVer		 	= 0xA000,
	PropertyTagExifColorSpace	 	= 0xA001,
	PropertyTagExifPixXDim		 	= 0xA002,
	PropertyTagExifPixYDim		  	= 0xA003,
	PropertyTagExifRelatedWav	  	= 0xA004,
	PropertyTagExifInterop        		= 0xA005,
	PropertyTagExifFlashEnergy    		= 0xA20B,
	PropertyTagExifSpatialFR      		= 0xA20C,
	PropertyTagExifFocalXRes      		= 0xA20E,
	PropertyTagExifFocalYRes      		= 0xA20F,
	PropertyTagExifFocalResUnit   		= 0xA210,
	PropertyTagExifSubjectLoc     		= 0xA214,
	PropertyTagExifExposureIndex  		= 0xA215,
	PropertyTagExifSensingMethod  		= 0xA217,
	PropertyTagExifFileSource     		= 0xA300,
	PropertyTagExifSceneType      		= 0xA301,
	PropertyTagExifCfaPattern     		= 0xA302,

	PropertyTagGpsVer             		= 0x0000,
	PropertyTagGpsLatitudeRef     		= 0x0001,
	PropertyTagGpsLatitude        		= 0x0002,
	PropertyTagGpsLongitudeRef    		= 0x0003,
	PropertyTagGpsLongitude       		= 0x0004,
	PropertyTagGpsAltitudeRef		= 0x0005,
	PropertyTagGpsAltitude			= 0x0006,
	PropertyTagGpsGpsTime			= 0x0007,
	PropertyTagGpsGpsSatellites		= 0x0008,
	PropertyTagGpsGpsStatus			= 0x0009,
	PropertyTagGpsGpsMeasureMode		= 0x000A,
	PropertyTagGpsGpsDop			= 0x000B,
	PropertyTagGpsSpeedRef			= 0x000C,
	PropertyTagGpsSpeed			= 0x000D,
	PropertyTagGpsTrackRef			= 0x000E,
	PropertyTagGpsTrack			= 0x000F,
	PropertyTagGpsImgDirRef			= 0x0010,
	PropertyTagGpsImgDir			= 0x0011,
	PropertyTagGpsMapDatum			= 0x0012,
	PropertyTagGpsDestLatRef		= 0x0013,
	PropertyTagGpsDestLat			= 0x0014,
	PropertyTagGpsDestLongRef		= 0x0015,
	PropertyTagGpsDestLong			= 0x0016,
	PropertyTagGpsDestBearRef		= 0x0017,
	PropertyTagGpsDestBear			= 0x0018,
	PropertyTagGpsDestDistRef		= 0x0019,
	PropertyTagGpsDestDist			= 0x001A
} PropertyTag;

typedef enum {
	PropertyTagTypeByte		= 1,
	PropertyTagTypeASCII		= 2,
	PropertyTagTypeShort		= 3,
	PropertyTagTypeLong		= 4,
	PropertyTagTypeRational		= 5,
	PropertyTagTypeSByte		= 6,
	PropertyTagTypeUndefined	= 7,
	PropertyTagTypeSShort		= 8,
	PropertyTagTypeSLONG		= 9,
	PropertyTagTypeSRational	= 10,
	PropertyTagTypeFloat		= 11,
	PropertyTagTypeDouble		= 12
} PropertyTagType;

typedef enum {
	RotateNoneFlipNone	= 0,
	Rotate90FlipNone	= 1,
	Rotate180FlipNone	= 2,
	Rotate270FlipNone	= 3,

	RotateNoneFlipX		= 4,
	Rotate90FlipX		= 5,
	Rotate180FlipX		= 6,
	Rotate270FlipX		= 7,

	RotateNoneFlipY		= Rotate180FlipX,
	Rotate90FlipY		= Rotate270FlipX,
	Rotate180FlipY		= RotateNoneFlipX,
	Rotate270FlipY		= Rotate90FlipX,

	RotateNoneFlipXY	= Rotate180FlipNone,
	Rotate90FlipXY		= Rotate270FlipNone,
	Rotate180FlipXY		= RotateNoneFlipNone,
	Rotate270FlipXY		= Rotate90FlipNone
} RotateFlipType; 

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
