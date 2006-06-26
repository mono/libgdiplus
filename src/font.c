/*
 * Copyright (c) 2004 Ximian
 * Copyright (c) 2004-2006 Novell, Inc.
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
 *          Jordi Mas i Hernandez <jordi@ximian.com>, 2004-2006
 *          Peter Dennis Bartok <pbartok@novell.com>
 */
#include <stdio.h>
#include "gdip.h"
#include <math.h>
#include <glib.h>
#include <freetype/tttables.h>
#include <pthread.h>


/* Generic fonts families */
static GStaticMutex generic = G_STATIC_MUTEX_INIT;
static GpFontFamily* familySerif = NULL;
static GpFontFamily* familySansSerif = NULL;
static GpFontFamily* familyMonospace = NULL;
static int ref_familySerif = 0;
static int ref_familySansSerif = 0;
static int ref_familyMonospace = 0;


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
	result->pattern = NULL;
	result->allocated = FALSE;
	*family = result;
}

static GpFontCollection *system_fonts = NULL;

GpStatus
GdipNewInstalledFontCollection (GpFontCollection **font_collection)
{	
	if (!font_collection)
		return InvalidParameter;

	/*
	 * Ensure we leak this data only a single time, because:
	 * (a) there is no API to free it;
	 * (b) other libgdiplus structures depends on that allocated data;
	 */
	if (!system_fonts) {
		FcObjectSet *os = FcObjectSetBuild (FC_FAMILY, FC_FOUNDRY, NULL);
		FcPattern *pat = FcPatternCreate ();
		FcValue val;
		FcFontSet *col;

		/* Only Scalable fonts for now */
		val.type = FcTypeBool;
		val.u.b = FcTrue;
		FcPatternAdd (pat, FC_SCALABLE, val, TRUE);
		FcObjectSetAdd (os, FC_SCALABLE);

		col = FcFontList (0, pat, os);
		FcPatternDestroy (pat);
		FcObjectSetDestroy (os);
    
		system_fonts = (GpFontCollection *) GdipAlloc (sizeof (GpFontCollection));
		system_fonts->fontset = col;
		system_fonts->config = NULL;
	}

	*font_collection = system_fonts;
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
		if ((*font_collection)->fontset != NULL) {
			FcFontSetDestroy ((*font_collection)->fontset);
			(*font_collection)->fontset = NULL;
		}
		if ((*font_collection)->config != NULL) {
			FcConfigDestroy ((*font_collection)->config);
			(*font_collection)->config = NULL;
		}
		GdipFree ((void *)*font_collection);
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

	g_static_mutex_lock (&generic);
	
	if (fontFamily == familySerif) {
		ref_familySerif--;
		if (ref_familySerif)
			delete = FALSE;
		else
			familySerif = NULL;
	}
	
	if (fontFamily == familySansSerif) {
		ref_familySansSerif--;
		if (ref_familySansSerif)
			delete = FALSE;
		else
			familySansSerif = NULL;
	}	
	
	if (fontFamily == familyMonospace) {
		ref_familyMonospace--;
		if (ref_familyMonospace)
			delete = FALSE;
		else
			familyMonospace = NULL;
	}	

	g_static_mutex_unlock (&generic);
	
	if (delete) {
		if (fontFamily->allocated) {
			FcPatternDestroy (fontFamily->pattern);
			fontFamily->pattern = NULL;
		}
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
GdipGetFontCollectionFamilyList (GpFontCollection *font_collection, int num_sought, GpFontFamily *gpfamilies[], int *num_found)
{
	int i;

	if (!font_collection || !gpfamilies || !num_found)
		return InvalidParameter;

	if (font_collection->config)
		gdip_createPrivateFontSet (font_collection);

	for (i = 0; i < font_collection->fontset->nfont; i++) {
		gdip_createFontFamily(&gpfamilies[i]);
		gpfamilies[i]->pattern = font_collection->fontset->fonts[i];
		gpfamilies[i]->allocated = FALSE;
	}
	
	*num_found = font_collection->fontset->nfont;
	return Ok;  
}

static GpStatus
gdip_status_from_fontconfig (FcResult result)
{
	switch (result) {
	case FcResultMatch:
		return Ok;
	case FcResultNoMatch:
	case FcResultTypeMismatch:
	case FcResultNoId:
		return FontFamilyNotFound;
	default:
		return GenericError;
	}
}

/* note: MUST be executed inside a lock because FcConfig isn't thread-safe */
static FcPattern*
create_pattern_from_name (char* name)
{
	FcValue val;
	/* FcResult must be initialized because it's changed only in error conditions */
	FcResult rlt = FcResultMatch;
	FcPattern *full_pattern = NULL;
	FcPattern *name_pattern = FcPatternCreate ();

	if (!name_pattern)
		return NULL;
		
	/* find the family we want */
	val.type = FcTypeString;
	val.u.s = (unsigned char*)name;
	if (!FcPatternAdd (name_pattern, FC_FAMILY, val, TRUE)) {
		FcPatternDestroy (name_pattern);
		return NULL;
	}

	if (!FcConfigSubstitute (0, name_pattern, FcMatchPattern)) {
		FcPatternDestroy (name_pattern);
		return NULL;
	}

	FcDefaultSubstitute (name_pattern);                  
		
	full_pattern = FcFontMatch (0, name_pattern, &rlt);
	if (gdip_status_from_fontconfig (rlt) == Ok) {
		if (full_pattern == NULL) {
			full_pattern = name_pattern;
		} else {
			FcPatternDestroy (name_pattern);
		}
	} else {
		FcPatternDestroy (name_pattern);
		if (full_pattern) {
			FcPatternDestroy (full_pattern);
			full_pattern = NULL;
		}
	}

	return full_pattern;
}

static GStaticMutex patterns_mutex = G_STATIC_MUTEX_INIT;
static GHashTable *patterns_hashtable;

static GpStatus
create_fontfamily_from_name (char* name, GpFontFamily **fontFamily)
{
	GpStatus status = OutOfMemory;
	GpFontFamily *ff = NULL;
	FcPattern *pat = NULL;

	g_static_mutex_lock (&patterns_mutex);

	if (patterns_hashtable) {
		pat = (FcPattern*) g_hash_table_lookup (patterns_hashtable, name);
	} else {
		patterns_hashtable = g_hash_table_new (g_str_hash, g_str_equal);
	}

	if (!pat) {
		pat = create_pattern_from_name (name);
		if (pat) {
			/* create the pattern and store it for further usage */
			g_hash_table_insert (patterns_hashtable, g_strdup (name), pat);
		}
	}

	if (pat) {
		gdip_createFontFamily (&ff);
		if (ff) {
			ff->pattern = pat;
			ff->allocated = FALSE;
			status = Ok;
		}
	}

	*fontFamily = ff;
	g_static_mutex_unlock (&patterns_mutex);
	return status;
}

static
gboolean free_cached_pattern (gpointer key, gpointer value, gpointer user)
{
	g_free (value);
	FcPatternDestroy ((FcPattern*) value);
	return TRUE;
}

void
gdip_font_clear_pattern_cache (void)
{
	g_static_mutex_lock (&patterns_mutex);
	g_hash_table_foreach_remove (patterns_hashtable, free_cached_pattern, NULL);
	g_hash_table_destroy (patterns_hashtable);
	g_static_mutex_unlock (&patterns_mutex);
}

static GpStatus
create_fontfamily_from_collection (char* name, GpFontCollection *font_collection, GpFontFamily **fontFamily)
{
	/* note: fontset can be NULL when we supply an empty private collection */
	if (font_collection->fontset) {
		int i;
		FcChar8 *str;
		FcPattern **gpfam = font_collection->fontset->fonts;
    
		for (i=0; i < font_collection->fontset->nfont; gpfam++, i++) {
			FcResult rlt = FcPatternGetString (*gpfam, FC_FAMILY, 0, &str);
			GpStatus status = gdip_status_from_fontconfig (rlt);
			if (status != Ok)
				return status;

			if (strcmp ((char *)name, (const char *)str) == 0) {
				gdip_createFontFamily (fontFamily);
				(*fontFamily)->pattern = *gpfam;
				(*fontFamily)->allocated = FALSE;
				return Ok;
			}
		}
	}
	return FontFamilyNotFound;
}


GpStatus
GdipCreateFontFamilyFromName (GDIPCONST WCHAR *name, GpFontCollection *font_collection, GpFontFamily **fontFamily)
{
	GpStatus status;
	char *string;
	
	if (!name || !fontFamily)
		return InvalidParameter;

	string = (char*)ucs2_to_utf8 ((const gunichar2 *)name, -1);

	if (font_collection) {
		status = create_fontfamily_from_collection (string, font_collection, fontFamily);
	} else {
		status = create_fontfamily_from_name (string, fontFamily);
	}

	GdipFree (string);
	return status;
}

GpStatus
GdipGetFamilyName (GDIPCONST GpFontFamily *family, WCHAR name[LF_FACESIZE], int language)
{
	GpStatus status;
	FcChar8 *fc_str;
	FcResult r;
	
	if (!family)
		return InvalidParameter;

	r = FcPatternGetString (family->pattern, FC_FAMILY, 0, &fc_str);
	status = gdip_status_from_fontconfig (r);
	if (status != Ok)
		return status;

	utf8_to_ucs2((const gchar *)fc_str, (gunichar2 *)name, LF_FACESIZE);
	return Ok;
}


GpStatus
GdipGetGenericFontFamilySansSerif (GpFontFamily **nativeFamily)
{
	const WCHAR MSSansSerif[] = {'M','S',' ','S','a','n','s',' ', 'S','e','r','i','f', 0};
	GpStatus status = Ok;
	
	g_static_mutex_lock (&generic);

	if (ref_familySansSerif == 0)
		status = GdipCreateFontFamilyFromName (MSSansSerif, NULL, &familySansSerif);

	if (status == Ok)
		ref_familySansSerif++;
	else
		familySansSerif = NULL;

	g_static_mutex_unlock (&generic);

	*nativeFamily = familySansSerif;    
	return status;
}

GpStatus
GdipGetGenericFontFamilySerif (GpFontFamily **nativeFamily)
{
	const WCHAR Serif[] = {'S','e','r','i','f', 0};
	GpStatus status = Ok;
	
	g_static_mutex_lock (&generic);

	if (ref_familySerif == 0)
		status = GdipCreateFontFamilyFromName (Serif, NULL, &familySerif);

	if (status == Ok)
		ref_familySerif++;
	else
		familySerif = NULL;

	g_static_mutex_unlock (&generic);

	*nativeFamily = familySerif;
	return status;
}

GpStatus
GdipGetGenericFontFamilyMonospace (GpFontFamily **nativeFamily)
{
	const WCHAR Serif[] = {'S','e','r','i','f', 0};
	GpStatus status = Ok;
	
	g_static_mutex_lock (&generic);

	if (ref_familyMonospace == 0)
		status = GdipCreateFontFamilyFromName (Serif, NULL, &familyMonospace);

	if (status == Ok)
		ref_familyMonospace++;
	else
		familyMonospace = NULL;

	g_static_mutex_unlock (&generic);

	*nativeFamily = familyMonospace;    
	return status;
}

FT_Face
gdip_cairo_ft_font_lock_face (cairo_font_face_t *cairofnt, cairo_scaled_font_t **scaled_ft)
{
	FT_Face face;
	cairo_matrix_t matrix1, matrix2;
        cairo_font_options_t *options = NULL;
	
	options = cairo_font_options_create ();
		
	cairo_matrix_init (&matrix1, 1, 0, 0, 1, 0, 0);
	cairo_matrix_init (&matrix2, 1, 0, 0, 1, 0, 0);
	*scaled_ft = cairo_scaled_font_create (cairofnt,
					      &matrix1,
					      &matrix2,
					      options);
	/* a missing fonts.conf will resuls in a NULL *scaled_ft (#78237) */
	if (!*scaled_ft) {
		static int flag = 0;
		if (flag == 0) {
			g_warning ("couldn't lock the font face. this may be due to a missing fonts.conf on the system.");
			flag = 1;
		}
		return NULL;
	}

	face = cairo_ft_scaled_font_lock_face (*scaled_ft);

	cairo_font_options_destroy (options);
	return face;
}

void
gdip_cairo_ft_font_unlock_face (cairo_scaled_font_t* scaled_ft)
{
	cairo_ft_scaled_font_unlock_face (scaled_ft);
	cairo_scaled_font_destroy (scaled_ft);
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
		cairo_scaled_font_t* scaled_ft;
		FT_Face	face;
		TT_VertHeader *pVert;

		face = gdip_cairo_ft_font_lock_face (font->cairofnt, &scaled_ft);
		if (face) {
			pVert = FT_Get_Sfnt_Table(face, ft_sfnt_vhea);
			if (pVert) {
				rslt = pVert->yMax_Extent;
			} else if (face) {
				rslt = face->units_per_EM;
			}
			gdip_cairo_ft_font_unlock_face (scaled_ft);
		}
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
		cairo_scaled_font_t* scaled_ft;
                FT_Face face;
		TT_HoriHeader *pHori;

		face = gdip_cairo_ft_font_lock_face (font->cairofnt, &scaled_ft);
		if (face) {
			pHori = FT_Get_Sfnt_Table (face, ft_sfnt_hhea);
			if (pHori)
				rslt = pHori->Ascender;

			gdip_cairo_ft_font_unlock_face (scaled_ft);
		}
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
		cairo_scaled_font_t* scaled_ft;
                FT_Face face;
		TT_HoriHeader* pHori;

		face = gdip_cairo_ft_font_lock_face (font->cairofnt, &scaled_ft);
		if (face) {
			pHori = FT_Get_Sfnt_Table (face, ft_sfnt_hhea);
			if (pHori)
				rslt = -pHori->Descender;

			gdip_cairo_ft_font_unlock_face (scaled_ft);
		}
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
		cairo_scaled_font_t* scaled_ft;
                FT_Face face;
		TT_HoriHeader *pHori;

		face = gdip_cairo_ft_font_lock_face (font->cairofnt, &scaled_ft);
		if (face) {
			pHori = FT_Get_Sfnt_Table (face, ft_sfnt_hhea);
			if (pHori) {
				rslt = pHori->Ascender + (-pHori->Descender) + pHori->Line_Gap;
			} else if (face) {
				rslt = face->height;
			}

			gdip_cairo_ft_font_unlock_face(scaled_ft);
		}
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
cairo_font_face_t *
gdip_face_create (const char *family, 
			cairo_font_slant_t   slant, 
			cairo_font_weight_t  weight,
			cairo_t** ct)
{
	cairo_surface_t *surface;
	cairo_font_face_t *face;

	surface = cairo_image_surface_create_for_data ((unsigned char *)NULL, CAIRO_FORMAT_ARGB32, 0, 0, 0);
	*ct = cairo_create (surface);
	cairo_select_font_face (*ct, (const char *) family, slant, weight);
	face = cairo_get_font_face (*ct);
	cairo_surface_destroy (surface);
	return face;

}

GpStatus
GdipCreateFont (GDIPCONST GpFontFamily* family, float emSize, GpFontStyle style, Unit unit,  GpFont **font)
{
	GpStatus status;
	FcChar8* str;
	FcResult r;
	GpFont *result;
	cairo_font_slant_t slant;
	cairo_font_weight_t weight;
	float sizeInPixels;
	cairo_font_face_t* cairofnt;
	
	if (!family || !font || (unit == UnitDisplay))
		return InvalidParameter;

	r = FcPatternGetString (family->pattern, FC_FAMILY, 0, &str);
	status = gdip_status_from_fontconfig (r);
	if (status != Ok)
		return status;
	
	gdip_unit_conversion (unit, UnitPixel, gdip_get_display_dpi(), gtMemoryBitmap, emSize, &sizeInPixels);
		
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

	cairofnt = gdip_face_create ((const char*) str, slant, weight, &result->ct);

	if (cairofnt == NULL) {
		GdipFree(result);
		return GenericError;
	}

	result->face = GdipAlloc(strlen((char *)str) + 1);
	if (!result->face) {
		GdipFree(result);
		return OutOfMemory;
	}

	memcpy(result->face, str, strlen((char *)str) + 1);

	result->cairofnt = cairofnt;
        result->style = style;
	result->emSize = emSize;
	result->unit = unit;
	result->family = (GpFontFamily*) family;
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

	cairo_destroy (font->ct);

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
		utf8_to_ucs2((const gchar *)font->face, (gunichar2 *)logFont->lfFaceName, LF_FACESIZE);
	} else {
		int len = strlen ((char*)font->face);
		memset (logFont->lfFaceName, 0, LF_FACESIZE);
		memcpy (logFont->lfFaceName, font->face, len < LF_FACESIZE ? len : LF_FACESIZE - 1);
	}
}

GpStatus
GdipCreateFontFromHfontA(void *hfont, GpFont **font, void *lf)
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
	result->family = src_font->family;
	result->style = src_font->style;
	result->emSize = src_font->emSize;
	result->unit = src_font->unit;

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
	result->face = GdipAlloc(strlen((char *)src_font->face) + 1);
	if (!result->face) {
		GdipFree(result);
		return OutOfMemory;
	}

	memcpy(result->face, src_font->face, strlen((char *)src_font->face) + 1);

	result->cairofnt = gdip_face_create ((const char*) src_font->face, slant, weight, &result->ct);

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
gdip_create_font_from_logfont(void *hdc, void *lf, GpFont **font, bool ucs2)
{
	GpFont			*result;
	cairo_font_face_t	*face;
	cairo_font_slant_t	slant;
	cairo_font_weight_t	weight;
	LOGFONTA		*logfont;

	logfont = (LOGFONTA *)lf;

	result = (GpFont *) GdipAlloc(sizeof(GpFont));
	if (logfont->lfHeight < 0) {
		result->sizeInPixels = -(logfont->lfHeight);
	} else {
		result->sizeInPixels = logfont->lfHeight;	// Fixme - convert units
	}
	result->style = 0;
	result->family = 0;
	/* Fixme - this is wrong, but I don't know of a quick way to get the emSize */
	result->emSize = result->sizeInPixels;
	result->unit = UnitPixel;

	if (logfont->lfItalic) {
		result->style |= FontStyleItalic;
		slant = CAIRO_FONT_SLANT_ITALIC;
	} else {
		slant = CAIRO_FONT_SLANT_NORMAL;
	}

	if (logfont->lfWeight > 400) {
		result->style |= FontStyleBold;
		weight = CAIRO_FONT_WEIGHT_BOLD;
	} else {
		weight = CAIRO_FONT_WEIGHT_NORMAL;
	}

	if (logfont->lfUnderline) {
		result->style |= FontStyleUnderline;
	}
	if (logfont->lfStrikeOut) {
		result->style |= FontStyleStrikeout;
	}

	if (ucs2) {
		result->face = (unsigned char*) ucs2_to_utf8 ((const gunichar2 *)logfont->lfFaceName, -1);
	} else {
		result->face = GdipAlloc(LF_FACESIZE);
		memcpy(result->face, logfont->lfFaceName, LF_FACESIZE);
		result->face[LF_FACESIZE - 1] = '\0';
	}

	result->cairofnt = gdip_face_create ((const char *)result->face, slant, weight, &result->ct);
	if (result->cairofnt == NULL) {
		GdipFree(result);
		return GenericError;
	}

	*font = result;

	return Ok;
}

GpStatus
GdipCreateFontFromLogfontA(void *hdc, GDIPCONST LOGFONTA *logfont, GpFont **font)
{
	return gdip_create_font_from_logfont(hdc, (void *)logfont, font, FALSE);
}

GpStatus
GdipCreateFontFromLogfontW(void *hdc, GDIPCONST LOGFONTW *logfont, GpFont **font)
{
	return gdip_create_font_from_logfont(hdc, (void *)logfont, font, TRUE);
}

GpStatus
GdipPrivateAddMemoryFont(GpFontCollection *fontCollection, GDIPCONST void *memory, int length)
{
	FcBool result;
	FcChar8 fontfile[256];
	int	f;

	if (!memory)
		return InvalidParameter;

	strcpy((char *) fontfile, "/tmp/ffXXXXXX");

	f = mkstemp((char*)fontfile);
	if (f == -1)
		return FileNotFound;

	if (write(f, memory, length)!=length) {
		close(f);
		return FileNotFound;
	}
	close(f);

	/* FIXME - this doesn't seems to catch "bad" (e.g. invalid) font files */
	result = FcConfigAppFontAddFile (fontCollection->config, fontfile);

	/* FIXME - May we delete our temporary font file or does 
	   FcConfigAppFontAddFile just reference our file?  */
	/* unlink(fontfile); */

	return result ? Ok : FileNotFound;
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


GpStatus
GdipGetFontHeight (GDIPCONST GpFont *font, GDIPCONST GpGraphics *graphics, float *height)
{
	short emHeight, lineSpacing;
	float emSize;

	if (!font || !height || !graphics)
		return InvalidParameter;

	/* Operations in display dpi's */	
	gdip_unit_conversion (font->unit, UnitPixel, gdip_get_display_dpi (), gtMemoryBitmap, font->emSize, &emSize);
	GdipGetEmHeight (font->family, font->style, &emHeight);
	GdipGetLineSpacing (font->family, font->style, &lineSpacing);

	*height = lineSpacing * (emSize / emHeight);
	gdip_unit_conversion (UnitPixel, graphics->page_unit, gdip_get_display_dpi (), graphics->type, *height, height);
	return Ok;
}

GpStatus
GdipGetFontHeightGivenDPI (GDIPCONST GpFont *font, float dpi, float *height)
{
	short emHeight, lineSpacing;

	if (!font || !height)
		return InvalidParameter;

	GdipGetEmHeight (font->family, font->style, &emHeight);
	GdipGetLineSpacing (font->family, font->style, &lineSpacing);
	*height = lineSpacing * (font->emSize / emHeight);
	gdip_unit_conversion (font->unit, UnitInch, dpi, gtMemoryBitmap, *height, height);
	*height = *height * dpi;
	return Ok;
}

GpStatus
GdipGetFontSize (GpFont *font, float *size)
{
	if (!font ||!size)
		return InvalidParameter;

	*size = font->emSize;
	return Ok;
}

