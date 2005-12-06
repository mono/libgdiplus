/*
 * Copyright (c) 2004 Ximian
 * Copyright (c) 2004-2005 Novell, Inc.
 * 
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
 * Author:
 *          Jordi Mas i Hernandez <jordi@ximian.com>, 2004
 */
#include <stdio.h>
#include "gdip.h"
#include <math.h>
#include <glib.h>
#include <freetype/tttables.h>
#include <pthread.h>


/* Generic fonts families */
static GpFontFamily* familySerif = NULL;
static GpFontFamily* familySansSerif = NULL;
static GpFontFamily* familyMonospace = NULL;
static int ref_familySerif = 0;
static int ref_familySansSerif = 0;
static int ref_familyMonospace = 0;

extern cairo_font_face_t *
_cairo_toy_font_face_create (const char *family, 
			     cairo_font_slant_t   slant, 
			     cairo_font_weight_t  weight);


/* Family and collections font functions */

void
gdip_createFontFamily (GpFontFamily **family)
{
	GpFontFamily *result;

	result = (GpFontFamily *) GdipAlloc (sizeof (GpFontFamily));
	result->height = -1;
	result->linespacing = -1;
	result->celldescent = -1;
	result->cellascent = -1;
	*family = result;
}

GpStatus
GdipNewInstalledFontCollection (GpFontCollection **font_collection)
{	
	
	FcObjectSet *os = FcObjectSetBuild (FC_FAMILY, FC_FOUNDRY, NULL);
	FcPattern *pat = FcPatternCreate ();
	FcValue val;
	FcFontSet *col;
	GpFontCollection *result;
	    
	if (!font_collection)
		return InvalidParameter;
    
	/* Only Scalable fonts for now */
	val.type = FcTypeBool;
	val.u.b = FcTrue;
	FcPatternAdd (pat, FC_SCALABLE, val, TRUE);
	FcObjectSetAdd (os, FC_SCALABLE);

	col =  FcFontList (0, pat, os);
	FcPatternDestroy (pat);
	FcObjectSetDestroy (os);
    
	result = (GpFontCollection *) GdipAlloc (sizeof (GpFontCollection));
	result->fontset = col;
	result->config = NULL;
	*font_collection = result;

	return Ok;
}

GpStatus
GdipNewPrivateFontCollection (GpFontCollection **font_collection)
{
	GpFontCollection *result;

	if (!font_collection)
		return InvalidParameter;

	result = (GpFontCollection *) GdipAlloc (sizeof (GpFontCollection));
	result->fontset = NULL;
	result->config = FcConfigCreate ();
    
	*font_collection = result;
	return Ok;
}

GpStatus
GdipDeletePrivateFontCollection (GpFontCollection **font_collection)
{
	if (!font_collection)
		return InvalidParameter;

	if (*font_collection) {
		FcFontSetDestroy ((*font_collection)->fontset);
		FcConfigDestroy ((*font_collection)->config);
		GdipFree ((void *)font_collection);
	}

	return Ok;
}

GpStatus
GdipPrivateAddFontFile (GpFontCollection *font_collection,  GDIPCONST WCHAR *filename)
{
	unsigned char *file;
	
	if (!font_collection || !filename)
		return InvalidParameter;
    
	file = (unsigned char*) ucs2_to_utf8 ((const gunichar2 *)filename, -1);

	FcConfigAppFontAddFile (font_collection->config, file);
    
	GdipFree (file);
	return Ok;
}


GpStatus 
GdipDeleteFontFamily (GpFontFamily *fontFamily)
{
	bool delete = TRUE;
	
	if (!fontFamily)
		return Ok;
		
	if (fontFamily == familySerif) {
		ref_familySerif--;
		if (ref_familySerif)
			delete = FALSE;
	}
	
	if (fontFamily == familySansSerif) {
		ref_familySansSerif--;
		if (ref_familySansSerif)
			delete = FALSE;
	}	
	
	if (fontFamily == familyMonospace) {
		ref_familyMonospace--;
		if (ref_familyMonospace)
			delete = FALSE;
	}	
	
	if (delete) {
		FcPatternDestroy (fontFamily->pattern);
		GdipFree (fontFamily);
	}
	
	return Ok;
}

void
gdip_createPrivateFontSet (GpFontCollection *font_collection)
{
	FcObjectSet *os = FcObjectSetBuild (FC_FAMILY, FC_FOUNDRY, NULL);
	FcPattern *pat = FcPatternCreate ();
	FcFontSet *col =  FcFontList (font_collection->config, pat, os);
    
	if (font_collection->fontset)
		FcFontSetDestroy (font_collection->fontset);

	FcPatternDestroy (pat);
	FcObjectSetDestroy (os);

	font_collection->fontset = col;
}

GpStatus
GdipGetFontCollectionFamilyCount (GpFontCollection *font_collection, int *numFound)
{
	if (!font_collection  || !numFound)
		return InvalidParameter;

	if (font_collection->config)
		gdip_createPrivateFontSet (font_collection);

	if (font_collection->fontset)
		*numFound = font_collection->fontset->nfont;
	else
		*numFound = 0;

	return Ok;
}

GpStatus
GdipGetFontCollectionFamilyList (GpFontCollection *font_collection, int num_sought, GpFontFamily **gpfamilies, int *num_found)
{
	GpFontFamily **gpfam = gpfamilies;
	FcPattern **pattern =  font_collection->fontset->fonts;
	int i;

	if (!font_collection || !gpfamilies || !num_found)
		return InvalidParameter;

	if (font_collection->config)
		gdip_createPrivateFontSet (font_collection);

	for (i = 0; i < font_collection->fontset->nfont; gpfam++, pattern++, i++) {
		gdip_createFontFamily (gpfam);
		(*gpfam)->pattern = *pattern;
		(*gpfam)->allocated = FALSE;
	}

	*num_found = font_collection->fontset->nfont;
	return Ok;  
}

GpStatus
GdipCreateFontFamilyFromName (GDIPCONST WCHAR *name, GpFontCollection *font_collection, GpFontFamily **fontFamily)
{
	unsigned char *string;
	FcPattern **gpfam;
	FcChar8 *str;
	int i;
	
	if (!name || !fontFamily)
		return InvalidParameter;

	string = (unsigned char*)ucs2_to_utf8 ((const gunichar2 *)name, -1);

	if (!font_collection) {
		FcPattern *pat = FcPatternCreate ();
		FcResult rlt;
		
		/* find the family we want */
		FcValue val;
                                   
		val.type = FcTypeString;
		val.u.s = string;
		FcPatternAdd (pat, FC_FAMILY, val, TRUE);

		FcConfigSubstitute (0, pat, FcMatchPattern);
		FcDefaultSubstitute (pat);                  
		
		gdip_createFontFamily (fontFamily);
		(*fontFamily)->pattern =  FcFontMatch (0, pat, &rlt);
		(*fontFamily)->allocated = TRUE;
		GdipFree (string);
		if ((*fontFamily)->pattern == NULL) {
			(*fontFamily)->pattern = pat;
		} else {
			FcPatternDestroy (pat);			
		}
		
		return Ok;
	}

	gpfam = font_collection->fontset->fonts;
    
	for (i=0; i < font_collection->fontset->nfont; gpfam++, i++){
		FcPatternGetString (*gpfam, FC_FAMILY, 0, &str);

		if (strcmp ((char *)string, (const char *)str)==0) {
			gdip_createFontFamily (fontFamily);
			(*fontFamily)->pattern = *gpfam;
			(*fontFamily)->allocated = FALSE;
			GdipFree (string);
			return Ok;
		}
	}

	GdipFree (string);
	return FontFamilyNotFound;
}

GpStatus
GdipGetFamilyName (GDIPCONST GpFontFamily *family, WCHAR name[LF_FACESIZE], int language)
{                
	FcChar8 *fc_str;
	FcResult r;
	
	if (!family)
		return InvalidParameter;

	r = FcPatternGetString (family->pattern, FC_FAMILY, 0, &fc_str);

	utf8_to_ucs2((const gchar *)fc_str, (gunichar2 *)name, LF_FACESIZE);
	return Ok;
}


GpStatus
GdipGetGenericFontFamilySansSerif (GpFontFamily **nativeFamily)
{
	const WCHAR MSSansSerif[] = {'M','S',' ','S','a','n','s',' ', 'S','e','r','i','f', 0};
	
	if (!familySansSerif) 
		GdipCreateFontFamilyFromName (MSSansSerif, NULL, &familySansSerif);    
	
	ref_familySansSerif++;
	*nativeFamily = familySansSerif;    
	return Ok;
}

GpStatus
GdipGetGenericFontFamilySerif (GpFontFamily **nativeFamily)
{
	const WCHAR Serif[] = {'S','e','r','i','f', 0};
	
	if (!familySerif)
		GdipCreateFontFamilyFromName (Serif, NULL, &familySerif);
	
	ref_familySerif++;	
	*nativeFamily = familySerif;    
	return Ok;
}

GpStatus
GdipGetGenericFontFamilyMonospace (GpFontFamily **nativeFamily)
{
	const WCHAR Serif[] = {'S','e','r','i','f', 0};
	
	if (!familyMonospace)
		GdipCreateFontFamilyFromName (Serif, NULL, &familyMonospace);    
		
	ref_familyMonospace++;
	*nativeFamily = familyMonospace;    
	return Ok;
}

FT_Face
gdip_cairo_ft_font_lock_face (cairo_font_face_t *cairofnt)
{
	cairo_scaled_font_t *scaled_ft;
	cairo_matrix_t matrix1, matrix2;
        cairo_font_options_t *options;
	
	options = cairo_font_options_create ();
		
	scaled_ft = cairo_scaled_font_create (cairofnt,
					      &matrix1,
					      &matrix2,
					      options);
	return cairo_ft_scaled_font_lock_face (scaled_ft);
}

void
gdip_cairo_ft_font_unlock_face (cairo_font_face_t *cairofnt)
{
	cairo_scaled_font_t *scaled_ft;
	cairo_matrix_t matrix1, matrix2;
	cairo_font_options_t *options = NULL;

	options = cairo_font_options_create ();	
	
	scaled_ft = cairo_scaled_font_create (cairofnt,
					      &matrix1,
					      &matrix2,
					      options);
	
	cairo_ft_scaled_font_unlock_face (scaled_ft);
}

GpStatus
GdipGetEmHeight (GDIPCONST GpFontFamily *family, GpFontStyle style, short *EmHeight)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !EmHeight)
		return InvalidParameter;

	if (family->height != -1) {
		*EmHeight = family->height;
		return Ok;
	}

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font) {
		FT_Face	face;
		TT_VertHeader *pVert;

		face = gdip_cairo_ft_font_lock_face(font->cairofnt);

		pVert = FT_Get_Sfnt_Table(face, ft_sfnt_vhea);
		if (pVert) {
			rslt = pVert->yMax_Extent;
		} else if (face) {
			rslt = face->units_per_EM;
		} else {
			rslt = 0;
		}
		gdip_cairo_ft_font_unlock_face(font->cairofnt);
		GdipDeleteFont (font);
	}

	*EmHeight = rslt;
	((GpFontFamily *)family)->height = rslt;
	return Ok;
}

GpStatus
GdipGetCellAscent (GDIPCONST GpFontFamily *family, GpFontStyle style, short *CellAscent)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !CellAscent)
		return InvalidParameter;

	if (family->cellascent != -1) {
		*CellAscent = family->cellascent;
		return Ok;
	}

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font){
                FT_Face face;
		TT_HoriHeader *pHori;

                face = gdip_cairo_ft_font_lock_face(font->cairofnt);

		pHori = FT_Get_Sfnt_Table (face, ft_sfnt_hhea);

		if (pHori)
			rslt = pHori->Ascender;

		gdip_cairo_ft_font_lock_face(font->cairofnt);
		GdipDeleteFont (font);
	}

	*CellAscent = rslt;
	((GpFontFamily *)family)->cellascent = rslt;
	return Ok;         
}

GpStatus
GdipGetCellDescent (GDIPCONST GpFontFamily *family, GpFontStyle style, short *CellDescent)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !CellDescent)
		return InvalidParameter;

	if (family->celldescent != -1) {
		*CellDescent = family->celldescent;
		return Ok;
	}

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font){
                FT_Face face;
		TT_HoriHeader* pHori;

                face = gdip_cairo_ft_font_lock_face(font->cairofnt);

		pHori = FT_Get_Sfnt_Table (face, ft_sfnt_hhea);

		if (pHori)
			rslt = -pHori->Descender;

		gdip_cairo_ft_font_unlock_face(font->cairofnt);
		GdipDeleteFont (font);
	}

	*CellDescent = rslt;
	((GpFontFamily *)family)->celldescent = rslt;
	return Ok;         
}

GpStatus
GdipGetLineSpacing (GDIPCONST GpFontFamily *family, GpFontStyle style, short *LineSpacing)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !LineSpacing)
		return InvalidParameter;

	if (family->linespacing != -1) {
		*LineSpacing = family->linespacing;
		return Ok;
	}

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font){
                FT_Face face;
		TT_HoriHeader *pHori;

                face = gdip_cairo_ft_font_lock_face(font->cairofnt);

		pHori = FT_Get_Sfnt_Table (face, ft_sfnt_hhea);
		if (pHori) {
			rslt = pHori->Ascender + (-pHori->Descender) + pHori->Line_Gap;
		} else if (face) {
			rslt = face->height;
		} else {
			rslt = 0;
		}

		gdip_cairo_ft_font_unlock_face(font->cairofnt);
		GdipDeleteFont (font);
	}

	*LineSpacing = rslt;
	((GpFontFamily *)family)->linespacing = rslt;
	return Ok;
}

GpStatus
GdipIsStyleAvailable (GDIPCONST GpFontFamily *family, int style, BOOL *IsStyleAvailable)
{
	if (!family || !IsStyleAvailable)
		return InvalidParameter;

	*IsStyleAvailable = TRUE;
	return Ok;    
}

/* Font functions */

GpStatus
GdipCreateFont (GDIPCONST GpFontFamily* family, float emSize, GpFontStyle style, Unit unit,  GpFont **font)
{
	FcChar8* str;
	FcResult r;
	GpFont *result;
	cairo_font_slant_t slant;
	cairo_font_weight_t weight;
	float sizeInPixels;
	cairo_font_face_t* cairofnt;
	
	if (!family || !font)
		return InvalidParameter;

	r = FcPatternGetString (family->pattern, FC_FAMILY, 0, &str);
	
	gdip_unitConversion (unit, UnitPixel, emSize, &sizeInPixels);
		
	result = (GpFont *) GdipAlloc (sizeof (GpFont));
	result->sizeInPixels = sizeInPixels;

        if ((style & FontStyleBold) == FontStyleBold)
                weight = CAIRO_FONT_WEIGHT_BOLD;
	else
		weight = CAIRO_FONT_WEIGHT_NORMAL;

        if ((style & FontStyleItalic) == FontStyleItalic)        
                slant = CAIRO_FONT_SLANT_ITALIC;
	else
		slant = CAIRO_FONT_SLANT_NORMAL;

	cairofnt = _cairo_toy_font_face_create ((const char*) str, slant, weight);

	if (cairofnt == NULL) {
		GdipFree(result);
		return GenericError;
	}

	result->face = GdipAlloc(strlen((unsigned char *)str) + 1);
	if (!result->face) {
		GdipFree(result);
		return OutOfMemory;
	}

	memcpy(result->face, str, strlen((unsigned char *)str) + 1);

	result->cairofnt = cairofnt;
        result->style = style;
	*font = result;	        		
	return Ok;
}

GpStatus
GdipGetHfont(GpFont* font, void **Hfont)
{
	if (font) {
		*Hfont=font;
		return(Ok);
	}
	return InvalidParameter;
}


GpStatus
GdipDeleteFont (GpFont* font)
{
	if (!font)
		return InvalidParameter;

	cairo_font_face_destroy (font->cairofnt);
	GdipFree ((void *)font->face);
	GdipFree ((void *)font);
	return Ok;	       
}

GpStatus
GdipCreateFontFromDC(void *hdc, GpFont **font)
{
	return(NotImplemented);
}

static void
gdip_logfont_from_font(GpFont *font, GpGraphics *graphics, void *lf, bool ucs2)
{
	LOGFONTA		*logFont;

	logFont = (LOGFONTA *)lf;

	logFont->lfHeight = -(font->sizeInPixels);
	logFont->lfWidth = 0;
	logFont->lfEscapement = 0;	// FIXME
	logFont->lfOrientation = logFont->lfEscapement;
	if (font->style & FontStyleBold) {
		logFont->lfWeight = 700;
	} else {
		logFont->lfWeight = 400;
	}

	if (font->style & FontStyleItalic) {
		logFont->lfItalic = 1;
	} else {
		logFont->lfItalic = 0;
	}

	if (font->style & FontStyleUnderline) {
		logFont->lfUnderline = 1;
	} else {
		logFont->lfUnderline = 0;
	}

	if (font->style & FontStyleStrikeout) {
		logFont->lfStrikeOut = 1;
	} else {
		logFont->lfStrikeOut = 0;
	}

	logFont->lfCharSet = 1;	// DEFAULT_CHARSET
	logFont->lfOutPrecision = 0;
	logFont->lfClipPrecision = 0;
	if (graphics != NULL) {
		switch(graphics->text_mode) {
			case TextRenderingHintSystemDefault: {
				logFont->lfQuality = 0;
				break;
			}
	
			case TextRenderingHintSingleBitPerPixelGridFit:
			case TextRenderingHintSingleBitPerPixel:
			case TextRenderingHintAntiAliasGridFit:
			case TextRenderingHintAntiAlias: {
				logFont->lfQuality = 3;	// ANTIALIASED_QUALITY;
				break;
			}
	
			case TextRenderingHintClearTypeGridFit: {
				logFont->lfQuality = 5;	// CLEARTYPE_QUALITY
				break;
			}
		}
	} else {
		logFont->lfQuality = 0;
	}

	logFont->lfPitchAndFamily = 0;
	if (ucs2) {
		utf8_to_ucs2(font->face, (gunichar2 *)logFont->lfFaceName, LF_FACESIZE);
	} else {
		memcpy(logFont->lfFaceName, font->face, LF_FACESIZE);
		logFont->lfFaceName[LF_FACESIZE - 1] = '\0';
	}
}

GpStatus
GdipCreateFontFromHfont(void *hfont, GpFont **font, void *lf)
{
	GpFont			*src_font;
	GpFont			*result;
	cairo_font_face_t	*face;
	cairo_font_slant_t	slant;
	cairo_font_weight_t	weight;

	src_font = (GpFont *)hfont;

	result = (GpFont *) GdipAlloc (sizeof (GpFont));
	result->sizeInPixels = src_font->sizeInPixels;
	result->style = src_font->style;

        if ((result->style & FontStyleBold) == FontStyleBold) {
                weight = CAIRO_FONT_WEIGHT_BOLD;
	} else {
		weight = CAIRO_FONT_WEIGHT_NORMAL;
	}

        if ((result->style & FontStyleItalic) == FontStyleItalic) {
                slant = CAIRO_FONT_SLANT_ITALIC;
	} else {
		slant = CAIRO_FONT_SLANT_NORMAL;
	}
	result->face = GdipAlloc(strlen((unsigned char *)src_font->face) + 1);
	if (!result->face) {
		GdipFree(result);
		return OutOfMemory;
	}

	memcpy(result->face, src_font->face, strlen((unsigned char *)src_font->face) + 1);

	result->cairofnt = _cairo_toy_font_face_create ((const char*) src_font->face, slant, weight);

	if (result->cairofnt == NULL) {
		GdipFree(result);
		return GenericError;
	}

	*font = result;

	gdip_logfont_from_font(result, NULL, lf, FALSE);

	return Ok;
}

GpStatus
GdipGetLogFontW(GpFont *font, GpGraphics *graphics, void *lf)
{
	if (!font || !lf) {
		return(InvalidParameter);
	}
	gdip_logfont_from_font(font, graphics, lf, TRUE);
	return Ok;
}

GpStatus
GdipGetLogFontA(GpFont *font, GpGraphics *graphics, void *lf)
{
		
	if (!font || !lf) {
		return(InvalidParameter);
	}
	
	gdip_logfont_from_font(font, graphics, lf, FALSE);
	return Ok;
}

GpStatus
GdipPrivateAddMemoryFont(GpFontCollection *fontCollection, GDIPCONST void *memory, int length)
{
	FcChar8 fontfile[256];
	int	f;

	strcpy((char *) fontfile, "/tmp/ffXXXXXX");

	f = mkstemp((char*)fontfile);
	if (f == -1) {
		return(GenericError);
	}

	if (write(f, memory, length)!=length) {
		close(f);
		return(GenericError);
	}
	close(f);

	FcConfigAppFontAddFile(fontCollection->config, fontfile);

	/* FIXME - May we delete our temporary font file or does 
	   FcConfigAppFontAddFile just reference our file?  */
	/* unlink(fontfile); */

	return(Ok);
}


bool
GetFontMetrics(GpGraphics *graphics, GpFont *font, int *ascent, int *descent)
{
	cairo_font_extents_t	font_extent;

	cairo_set_font_face (graphics->ct, (cairo_font_face_t*) font->cairofnt);
	cairo_set_font_size (graphics->ct, font->sizeInPixels);
	cairo_font_extents (graphics->ct, &font_extent);

	if (ascent) {
		*ascent = (int)font_extent.ascent;
	}

	if (descent) {
		*descent = (int)font_extent.descent;
	}

	return TRUE;
}
