/*
 * gdipImage.h : Contains declarations for manipulating images
 *
 * Authors:
 *  	Sanjay Gupta (gsanjay@novell.com)
 *
 * Copyright (C) 2003-2006 Novell, Inc (http://www.novell.com)
 */

#ifndef _GDIPIMAGE_H
#define _GDIPIMAGE_H


#include "gdip.h"
#include <stdio.h>
#include "bmpcodec.h"
#include "pngcodec.h"
#include "jpegcodec.h"
#include "gifcodec.h"
#include "tiffcodec.h"
#include "icocodec.h"
#include "emfcodec.h"
#include "wmfcodec.h"

/*typedef bool BOOL;
typedef BOOL (CALLBACK * ImageAbort) (VOID *);
typedef ImageAbort GetThumbnailImageAbort; */

static const CLSID gdip_image_frameDimension_page_guid = {0x7462dc86U, 0x6180U, 0x4c7eU, {0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83}};
static const CLSID gdip_image_frameDimension_time_guid = {0x6aedbd6dU, 0x3fb5U, 0x418aU, {0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72}};
static const CLSID gdip_image_frameDimension_resolution_guid = {0x84236f7bU, 0x3bd3U, 0x428fU, {0x8d, 0xab, 0x4e, 0xa1, 0x43, 0x9c, 0xa3, 0x15}};


/*
 * Enumerations
 *
 */
typedef enum {
	PaletteFlagsHasAlpha = 0x0001,
	PaletteFlagsGrayScale = 0x0002,
	PaletteFlagsHalftone = 0x0004
} PaletteFlags;

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
	TypeByte		= 1,
	TypeASCII		= 2,
	TypeShort		= 3,
	TypeLong		= 4,
	TypeRational		= 5,
	TypeSByte		= 6,
	TypeUndefined		= 7,
	TypeSShort		= 8,
	TypeSLONG		= 9,
	TypeSRational		= 10,
	TypeFloat		= 11,
	TypeDouble		= 12
} PropertyTagType;

typedef enum {
	ExifIFD             		= 0x8769,
	GpsIFD              		= 0x8825,

	NewSubfileType      		= 0x00FE,
	SubfileType         		= 0x00FF,
	ImageWidth          		= 0x0100,
	ImageHeight         		= 0x0101,
	BitsPerSample       		= 0x0102,
	Compression         		= 0x0103,
	PhotometricInterp   		= 0x0106,
	ThreshHolding       		= 0x0107,
	CellWidth           		= 0x0108,
	CellHeight          		= 0x0109,
	FillOrder           		= 0x010A,
	DocumentName        		= 0x010D,
	ImageDescription    		= 0x010E,
	EquipMake           		= 0x010F,
	EquipModel          		= 0x0110,
	StripOffsets        		= 0x0111,
	Orientation         		= 0x0112,
	SamplesPerPixel     		= 0x0115,
	RowsPerStrip        		= 0x0116,
	StripBytesCount     		= 0x0117,
	MinSampleValue      		= 0x0118,
	MaxSampleValue      		= 0x0119,
	XResolution         		= 0x011A,
	YResolution         		= 0x011B,
	PlanarConfig        		= 0x011C,
	PageName            		= 0x011D,
	XPosition           		= 0x011E,
	YPosition           		= 0x011F,
	FreeOffset          		= 0x0120,
	FreeByteCounts      		= 0x0121,
	GrayResponseUnit    		= 0x0122,
	GrayResponseCurve   		= 0x0123,
	T4Option            		= 0x0124,
	T6Option            		= 0x0125,
	ResolutionUnit      		= 0x0128,
	PageNumber          		= 0x0129,
	TransferFunction   		= 0x012D,
	SoftwareUsed        		= 0x0131,
	DateTime            		= 0x0132,
	Artist              		= 0x013B,
	HostComputer        		= 0x013C,
	Predictor           		= 0x013D,
	WhitePoint          		= 0x013E,
	PrimaryChromaticities		= 0x013F,
	ColorMap            		= 0x0140,
	HalftoneHints       		= 0x0141,
	TileWidth           		= 0x0142,
	TileLength          		= 0x0143,
	TileOffset          		= 0x0144,
	TileByteCounts      		= 0x0145,
	InkSet              		= 0x014C,
	InkNames            		= 0x014D,
	NumberOfInks        		= 0x014E,
	DotRange            		= 0x0150,
	TargetPrinter       		= 0x0151,
	ExtraSamples        		= 0x0152,
	SampleFormat        		= 0x0153,
	SMinSampleValue     		= 0x0154,
	SMaxSampleValue     		= 0x0155,
	TransferRange       		= 0x0156,

	JPEGProc            		= 0x0200,
	JPEGInterFormat     		= 0x0201,
	JPEGInterLength     		= 0x0202,
	JPEGRestartInterval 		= 0x0203,
	JPEGLosslessPredictors		= 0x0205,
	JPEGPointTransforms		= 0x0206,
	JPEGQTables         		= 0x0207,
	JPEGDCTables        		= 0x0208,
	JPEGACTables        		= 0x0209,

	YCbCrCoefficients   		= 0x0211,
	YCbCrSubsampling    		= 0x0212,
	YCbCrPositioning    		= 0x0213,
	REFBlackWhite       		= 0x0214,

	ICCProfile          		= 0x8773,
	
	Gamma               		= 0x0301,
	ICCProfileDescriptor		= 0x0302,
	SRGBRenderingIntent 		= 0x0303,

	ImageTitle          		= 0x0320,
	Copyright           		= 0x8298,

	// Other tags (Adobe Image Info, etc)
	ResolutionXUnit		= 0x5001,
	ResolutionYUnit           	= 0x5002,
	ResolutionXLengthUnit     	= 0x5003,
	ResolutionYLengthUnit     	= 0x5004,
	PrintFlags                	= 0x5005,
	PrintFlagsVersion         	= 0x5006,
	PrintFlagsCrop            	= 0x5007,
	PrintFlagsBleedWidth      	= 0x5008,
	PrintFlagsBleedWidthScale 	= 0x5009,
	HalftoneLPI               	= 0x500A,
	HalftoneLPIUnit           	= 0x500B,
	HalftoneDegree            	= 0x500C,
	HalftoneShape             	= 0x500D,
	HalftoneMisc              	= 0x500E,
	HalftoneScreen            	= 0x500F,
	JPEGQuality               	= 0x5010,
	GridSize                  	= 0x5011,
	ThumbnailFormat          	= 0x5012,  // 0 = RAW RGB, 1 = JPEG
	ThumbnailWidth            	= 0x5013,
	ThumbnailHeight           	= 0x5014,
	ThumbnailColorDepth       	= 0x5015,
	ThumbnailPlanes           	= 0x5016,
	ThumbnailRawBytes         	= 0x5017,
	ThumbnailSize             	= 0x5018,
	ThumbnailCompressedSize   	= 0x5019,
	ColorTransferFunction     	= 0x501A,
	ThumbnailData             	= 0x501B, // embeded image, format depends on ThumbnailFormat
	ThumbnailImageWidth       	= 0x5020,
	ThumbnailImageHeight      	= 0x5021,
	ThumbnailBitsPerSample    	= 0x5022,
	ThumbnailCompression      	= 0x5023,
	ThumbnailPhotometricInterp 	= 0x5024,
	ThumbnailImageDescription 	= 0x5025,
	ThumbnailEquipMake        	= 0x5026,
	ThumbnailEquipModel       	= 0x5027,
	ThumbnailStripOffsets     	= 0x5028,
	ThumbnailOrientation      	= 0x5029,
	ThumbnailSamplesPerPixel  	= 0x502A,
	ThumbnailRowsPerStrip     	= 0x502B,
	ThumbnailStripBytesCount  	= 0x502C,
	ThumbnailResolutionX      	= 0x502D,
	ThumbnailResolutionY      	= 0x502E,
	ThumbnailPlanarConfig     	= 0x502F,
	ThumbnailResolutionUnit   	= 0x5030,
	ThumbnailTransferFunction 	= 0x5031,
	ThumbnailSoftwareUsed     	= 0x5032,
	ThumbnailDateTime         	= 0x5033,
	ThumbnailArtist           	= 0x5034,
	ThumbnailWhitePoint       	= 0x5035,
	ThumbnailPrimaryChromaticities	= 0x5036,
	ThumbnailYCbCrCoefficients 	= 0x5037,
	ThumbnailYCbCrSubsampling 	= 0x5038,
	ThumbnailYCbCrPositioning 	= 0x5039,
	ThumbnailRefBlackWhite    	= 0x503A,
	ThumbnailCopyRight        	= 0x503B,
	LuminanceTable            	= 0x5090,
	ChrominanceTable          	= 0x5091,

	FrameDelay                	= 0x5100,
	LoopCount                 	= 0x5101,

	PixelUnit         		= 0x5110,
	PixelPerUnitX     		= 0x5111,
	PixelPerUnitY     		= 0x5112,
	PaletteHistogram  		= 0x5113,

	// EXIF specific tag
	ExifExposureTime  		= 0x829A,
	ExifFNumber       		= 0x829D,

	ExifExposureProg  		= 0x8822,
	ExifSpectralSense 		= 0x8824,
	ExifISOSpeed      		= 0x8827,
	ExifOECF          		= 0x8828,

	ExifVer            		= 0x9000,
	ExifDTOrig         		= 0x9003,
	ExifDTDigitized    		= 0x9004,

	ExifCompConfig     		= 0x9101,
	ExifCompBPP        		= 0x9102,

	ExifShutterSpeed   		= 0x9201,
	ExifAperture       		= 0x9202,
	ExifBrightness     		= 0x9203,
	ExifExposureBias		= 0x9204,
	ExifMaxAperture			= 0x9205,
	ExifSubjectDist			= 0x9206,
	ExifMeteringMode	 	= 0x9207,
	ExifLightSource	 		= 0x9208,
	ExifFlash		 	= 0x9209,
	ExifFocalLength	 		= 0x920A,
	ExifMakerNote	 		= 0x927C,
	ExifUserComment			= 0x9286,
	ExifDTSubsec		 	= 0x9290,
	ExifDTOrigSS		 	= 0x9291,
	ExifDTDigSS		 	= 0x9292,

	ExifFPXVer		 	= 0xA000,
	ExifColorSpace	 		= 0xA001,
	ExifPixXDim		 	= 0xA002,
	ExifPixYDim		  	= 0xA003,
	ExifRelatedWav	  		= 0xA004,
	ExifInterop        		= 0xA005,
	ExifFlashEnergy    		= 0xA20B,
	ExifSpatialFR      		= 0xA20C,
	ExifFocalXRes      		= 0xA20E,
	ExifFocalYRes      		= 0xA20F,
	ExifFocalResUnit   		= 0xA210,
	ExifSubjectLoc     		= 0xA214,
	ExifExposureIndex  		= 0xA215,
	ExifSensingMethod  		= 0xA217,
	ExifFileSource     		= 0xA300,
	ExifSceneType      		= 0xA301,
	ExifCfaPattern     		= 0xA302,

	GpsVer             		= 0x0000,
	GpsLatitudeRef     		= 0x0001,
	GpsLatitude        		= 0x0002,
	GpsLongitudeRef    		= 0x0003,
	GpsLongitude       		= 0x0004,
	GpsAltitudeRef			= 0x0005,
	GpsAltitude			= 0x0006,
	GpsGpsTime			= 0x0007,
	GpsGpsSatellites		= 0x0008,
	GpsGpsStatus			= 0x0009,
	GpsGpsMeasureMode		= 0x000A,
	GpsGpsDop			= 0x000B,
	GpsSpeedRef			= 0x000C,
	GpsSpeed			= 0x000D,
	GpsTrackRef			= 0x000E,
	GpsTrack			= 0x000F,
	GpsImgDirRef			= 0x0010,
	GpsImgDir			= 0x0011,
	GpsMapDatum			= 0x0012,
	GpsDestLatRef			= 0x0013,
	GpsDestLat			= 0x0014,
	GpsDestLongRef			= 0x0015,
	GpsDestLong			= 0x0016,
	GpsDestBearRef			= 0x0017,
	GpsDestBear			= 0x0018,
	GpsDestDistRef			= 0x0019,
	GpsDestDist			= 0x001A
} PropertyTag;

/*
 * Strutures
 *
 */


/*
 * Functions
 * 
 */

/* Image */
/* GpStatus GdipLoadImageFromStream (IStream *stream, GpImage **image); */
/* GpStatus GdipSaveImageToStream (GpImage *image, IStream *stream, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params); */
GpStatus GdipLoadImageFromFile (GDIPCONST WCHAR *file, GpImage **image); 
GpStatus GdipLoadImageFromFileICM (GDIPCONST WCHAR* filename, GpImage **image);
GpStatus GdipSaveImageToFile (GpImage *image, GDIPCONST WCHAR *file, GDIPCONST CLSID *encoderCLSID, GDIPCONST EncoderParameters *params); 
GpStatus GdipSaveAdd (GpImage *image, EncoderParameters* encoderParams);
GpStatus GdipSaveAddImage (GpImage *image, GpImage *imageNew, EncoderParameters *params);

GpStatus GdipGetImageBounds (GpImage *image, GpRectF *rect, GpUnit *unit);
GpStatus GdipGetImageDimension (GpImage *image, float *width, float *height);
GpStatus GdipGetImageType (GpImage *image, ImageType *type);
GpStatus GdipGetImageWidth (GpImage *image, UINT *width);
GpStatus GdipGetImageHeight (GpImage *image, UINT *heigth);
GpStatus GdipGetImageHorizontalResolution (GpImage *image, float *resolution);
GpStatus GdipGetImageVerticalResolution (GpImage *image, float *resolution);
GpStatus GdipGetImageFlags (GpImage *image, UINT *flags);
/* GpStatus GdipGetImageRawFormat (GpImage *image, GUID *format); */
GpStatus GdipGetImagePixelFormat (GpImage *image, PixelFormat *format);
/*GpStatus GdipGetImageThumbnail (GpImage *image, UINT width, UINT height, GpImage **thumbImage, GetThumbnailImageAbort callback, VOID* callBackData);*/
/* GpStatus GetEncoderParameterListSize (GpImage *image, GDIPCONST CLSID *encoderCLSID, UINT *size); */
/* GpStatus GetEncoderParameterList (GpImage *image, GDIPCONST CLSID *encoderCLSID, UINT size, EncoderParameters *buffer); */
GpStatus GdipImageGetFrameDimensionsCount (GpImage *image, UINT *count);
/* GpStatus GdipImageGetFrameDimensionsList (GpImage *image, GUID *dimensionGUID, UINT count); */
GpStatus GdipImageGetFrameCount (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT* count); 
GpStatus GdipImageSelectActiveFrame (GpImage *image, GDIPCONST GUID *dimensionGUID, UINT index);
GpStatus GdipImageRotateFlip (GpImage *image, RotateFlipType type);
GpStatus GdipGetImageGraphicsContext (GpImage *image, GpGraphics **graphics);
GpStatus GdipGetImagePalette (GpImage *image, ColorPalette *palette, int size);
GpStatus GdipSetImagePalette (GpImage *image, GDIPCONST ColorPalette *palette);
GpStatus GdipGetImagePaletteSize (GpImage *image, int* size);
GpStatus GdipGetPropertyCount (GpImage *image, UINT *propertyNumber);
GpStatus GdipGetPropertyIdList (GpImage *image, UINT propertyNumber, PROPID *list);
GpStatus GdipGetPropertyItemSize (GpImage *image, PROPID propID, UINT *size);
GpStatus GdipGetPropertyItem (GpImage *image, PROPID propID, UINT size, PropertyItem *buffer);
GpStatus GdipGetPropertySize (GpImage *image, UINT *bufferSize, UINT *propertyNumbers);
GpStatus GdipRemoveProperyItem (GpImage *image, PROPID propID);
GpStatus GdipSetProperyItem (GpImage *image, GDIPCONST PropertyItem *item);
GpStatus GdipCloneImage(GpImage *image, GpImage **cloneImage);

GpStatus GdipLoadImageFromDelegate_linux (GetHeaderDelegate getHeaderFunc,
					  GetBytesDelegate getBytesFunc,
                                          PutBytesDelegate putBytesFunc,
					  SeekDelegate seekFunc,
					  CloseDelegate closeFunc,
					  SizeDelegate sizeFunc,
                                          GpImage **image);
GpStatus GdipSaveImageToDelegate_linux (GpImage *image,
                                        GetBytesDelegate getBytesFunc,
                                        PutBytesDelegate putBytesFunc,
					SeekDelegate seekFunc,
					CloseDelegate closeFunc,
					SizeDelegate sizeFunc,
                                        GDIPCONST CLSID *encoderCLSID,
                                        GDIPCONST EncoderParameters *params);
					
GpStatus GdipDisposeImage (GpImage *image);


ImageFormat get_image_format (char *sig_read, size_t size_read);

int gdip_getpixel_formatsize(PixelFormat pixfmt);
int gdip_get_pixel_format_depth(PixelFormat pixfmt);
int gdip_get_pixel_format_components(PixelFormat pixfmt);
int gdip_get_pixel_format_bpp (PixelFormat pixfmt);
ImageFormat gdip_image_format_for_format_guid (GDIPCONST GUID *formatGUID);



#endif /* _GDIPIMAGE_H */
