/*
 * Copyright (c) 2004 Ximian
 * Copyright (c) 2004-2007 Novell, Inc.
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
 * Authors:
 *	Jordi Mas i Hernandez <jordi@ximian.com>, 2004-2006
 *	Peter Dennis Bartok <pbartok@novell.com>
 *	Sebastien Pouliot  <sebastien@ximian.com>
 *	Jeffrey Stedfast <fejj@novell.com>
 */

#ifdef WIN32
#include "win32-private.h"
#endif

#include <cairo-features.h>
#include "gdiplus-private.h"
#include "font-private.h"
#include "fontcollection-private.h"
#include "fontfamily-private.h"
#include "graphics-private.h"

/* Generic fonts families */
#if GLIB_CHECK_VERSION(2,32,0)
static GMutex generic;
#else
static GStaticMutex generic = G_STATIC_MUTEX_INIT;
#endif
static GpFontFamily* familySerif = NULL;
static GpFontFamily* familySansSerif = NULL;
static GpFontFamily* familyMonospace = NULL;
static int ref_familySerif = 0;
static int ref_familySansSerif = 0;
static int ref_familyMonospace = 0;


/* Family and collections font functions */
static void
gdip_fontfamily_init (GpFontFamily *fontFamily)
{
	fontFamily->height = -1;
	fontFamily->linespacing = -1;
	fontFamily->celldescent = -1;
	fontFamily->cellascent = -1;
	fontFamily->pattern = NULL;
	fontFamily->allocated = FALSE;
}

static GpFontFamily *
gdip_fontfamily_new ()
{
	GpFontFamily *result = (GpFontFamily *) GdipAlloc (sizeof (GpFontFamily));

	if (result)
		gdip_fontfamily_init (result);

	return result;
}

static GpFontCollection *system_fonts = NULL;

void
gdip_delete_system_fonts (void)
{
	GdipDeletePrivateFontCollection(&system_fonts);
}

// coverity[+alloc : arg-*0]
GpStatus WINGDIPAPI
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
		if (!system_fonts)
			return OutOfMemory;

		system_fonts->fontset = col;
		system_fonts->config = NULL;

#if USE_PANGO_RENDERING
		system_fonts->pango_font_map = pango_cairo_font_map_new_for_font_type (CAIRO_FONT_TYPE_FT);
#endif
	}

	*font_collection = system_fonts;
	return Ok;
}

// coverity[+alloc : arg-*0]
GpStatus WINGDIPAPI
GdipNewPrivateFontCollection (GpFontCollection **font_collection)
{
	GpFontCollection *result;

	if (!font_collection)
		return InvalidParameter;

	result = (GpFontCollection *) GdipAlloc (sizeof (GpFontCollection));
	if (!result)
		return OutOfMemory;

	result->fontset = NULL;
	result->config = FcConfigCreate ();

#if USE_PANGO_RENDERING
	result->pango_font_map = pango_cairo_font_map_new_for_font_type (CAIRO_FONT_TYPE_FT);
	pango_fc_font_map_set_config ((PangoFcFontMap *)result->pango_font_map, result->config);
#endif

	*font_collection = result;
	return Ok;
}

// coverity[+free : arg-0]
GpStatus
GdipDeletePrivateFontCollection (GpFontCollection **font_collection)
{
	if (!font_collection)
		return InvalidParameter;

	if (*font_collection) {
#if USE_PANGO_RENDERING
		if ((*font_collection)->pango_font_map != NULL) {
			g_object_unref ((*font_collection)->pango_font_map);
			(*font_collection)->pango_font_map = NULL;
		}
#endif
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

	*font_collection = NULL;
	return Ok;
}

GpStatus WINGDIPAPI
GdipPrivateAddFontFile (GpFontCollection *font_collection, GDIPCONST WCHAR *filename)
{
	BYTE *file;
	FILE *fileHandle;
	
	if (!font_collection || !filename)
		return InvalidParameter;
    
	file = (BYTE*) ucs2_to_utf8 ((const gunichar2 *)filename, -1);
	if (!file)
		return OutOfMemory;

	fileHandle = fopen ((char *)file, "r");
	if (!fileHandle) {
		GdipFree (file);
		return FileNotFound;
	}

	fclose (fileHandle);
	FcConfigAppFontAddFile (font_collection->config, file);
    
	GdipFree (file);
	return Ok;
}

GpStatus WINGDIPAPI
GdipCloneFontFamily (GpFontFamily *fontFamily, GpFontFamily **clonedFontFamily)
{
	GpFontFamily *result;

	if (!fontFamily || !clonedFontFamily)
		return InvalidParameter;

	result = gdip_fontfamily_new ();
	if (!result)
		return OutOfMemory;

	result->collection = fontFamily->collection;
	result->height = fontFamily->height;
	result->linespacing = fontFamily->linespacing;
	result->celldescent = fontFamily->celldescent;
	result->cellascent = fontFamily->cellascent;

	if (fontFamily->pattern) {
		result->pattern = FcPatternDuplicate (fontFamily->pattern);
		result->allocated = TRUE;
	}

	*clonedFontFamily = result;
	return Ok;
}

GpStatus WINGDIPAPI
GdipDeleteFontFamily (GpFontFamily *fontFamily)
{
	BOOL delete = TRUE;
	
	if (!fontFamily)
		return InvalidParameter;

#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_lock (&generic);
#else
	g_static_mutex_lock (&generic);
#endif

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

#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_unlock (&generic);
#else
	g_static_mutex_unlock (&generic);
#endif
	
	if (delete) {
		if (fontFamily->allocated) {
			FcPatternDestroy (fontFamily->pattern);
			fontFamily->pattern = NULL;
		}
		GdipFree (fontFamily);
	}
	
	return Ok;
}

static void
gdip_createPrivateFontSet (GpFontCollection *font_collection)
{
	FcObjectSet *os = FcObjectSetBuild (FC_FAMILY, FC_FOUNDRY, FC_FILE, NULL);
	FcPattern *pat = FcPatternCreate ();
	FcFontSet *col =  FcFontList (font_collection->config, pat, os);
    
	if (font_collection->fontset)
		FcFontSetDestroy (font_collection->fontset);

	FcPatternDestroy (pat);
	FcObjectSetDestroy (os);

	font_collection->fontset = col;
}

GpStatus WINGDIPAPI
GdipGetFontCollectionFamilyCount (GpFontCollection *font_collection, INT *numFound)
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

GpStatus WINGDIPAPI
GdipGetFontCollectionFamilyList (GpFontCollection *font_collection, INT num_sought, GpFontFamily *gpfamilies[], INT *num_found)
{
	int i;

	if (!font_collection || !gpfamilies || !num_found)
		return InvalidParameter;

	if (font_collection->config)
		gdip_createPrivateFontSet (font_collection);

	for (i = 0; i < num_sought && i < font_collection->fontset->nfont; i++) {
		gpfamilies[i] = gdip_fontfamily_new ();
		if (!gpfamilies[i]) {
			while (--i >= 0) {
				GdipFree (gpfamilies[i]);
				gpfamilies[i] = NULL;
			}
			return OutOfMemory;
		}

		gpfamilies[i]->pattern = font_collection->fontset->fonts[i];
		gpfamilies[i]->allocated = FALSE;
	}
	
	*num_found = i;
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
	val.u.s = (BYTE*)name;
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

#if GLIB_CHECK_VERSION(2,32,0)
static GMutex patterns_mutex;
#else
static GStaticMutex patterns_mutex = G_STATIC_MUTEX_INIT;
#endif
static GHashTable *patterns_hashtable = NULL;

static GpStatus
create_fontfamily_from_name (char* name, GpFontFamily **fontFamily)
{
	GpStatus status;
	GpFontFamily *ff = NULL;
	FcPattern *pat = NULL;
	GpFontCollection *font_collection;

	status = GdipNewInstalledFontCollection (&font_collection);
	if (status != Ok) {
		return status;
	}
	status = FontFamilyNotFound;

#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_lock (&patterns_mutex);
#else
	g_static_mutex_lock (&patterns_mutex);
#endif

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
		ff = gdip_fontfamily_new ();
		if (ff) {
			ff->pattern = pat;
			ff->allocated = FALSE;
			ff->collection = font_collection;
			status = Ok;
		} else 
			status = OutOfMemory;
	}

	*fontFamily = ff;
#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_unlock (&patterns_mutex);
#else
	g_static_mutex_unlock (&patterns_mutex);
#endif
	return status;
}

static BOOL
free_cached_pattern (gpointer key, gpointer value, gpointer user)
{
	g_free (key);
	FcPatternDestroy ((FcPattern*) value);
	return TRUE;
}

void
gdip_font_clear_pattern_cache (void)
{
#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_lock (&patterns_mutex);
#else
	g_static_mutex_lock (&patterns_mutex);
#endif
	if (patterns_hashtable) {
		g_hash_table_foreach_remove (patterns_hashtable, free_cached_pattern, NULL);
		g_hash_table_destroy (patterns_hashtable);
	}
#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_unlock (&patterns_mutex);
#else
	g_static_mutex_unlock (&patterns_mutex);
#endif
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
				GpFontFamily *result = gdip_fontfamily_new ();
				if (!result)
					return OutOfMemory;

				result->pattern = *gpfam;
				result->allocated = FALSE;
				result->collection = font_collection;

				*fontFamily = result;
				return Ok;
			}
		}
	}
	return FontFamilyNotFound;
}

// coverity[+alloc : arg-*2]
GpStatus WINGDIPAPI
GdipCreateFontFamilyFromName (GDIPCONST WCHAR *name, GpFontCollection *font_collection, GpFontFamily **fontFamily)
{
	GpStatus status;
	char *string;
	
	if (!name || !fontFamily)
		return InvalidParameter;

	string = (char*)ucs2_to_utf8 ((const gunichar2 *)name, -1);
	if (!string)
		return OutOfMemory;

	if (font_collection) {
		if (font_collection->config)
			gdip_createPrivateFontSet (font_collection);

		status = create_fontfamily_from_collection (string, font_collection, fontFamily);
	} else {
		status = create_fontfamily_from_name (string, fontFamily);
	}

	GdipFree (string);
	return status;
}

GpStatus WINGDIPAPI
GdipGetFamilyName (GDIPCONST GpFontFamily *family, WCHAR name[LF_FACESIZE], LANGID language)
{
	GpStatus status;
	FcChar8 *fc_str;
	FcResult r;
	
	if (!family)
		return InvalidParameter;

	if (!name)
		return Ok;

	r = FcPatternGetString (family->pattern, FC_FAMILY, 0, &fc_str);
	status = gdip_status_from_fontconfig (r);
	if (status != Ok)
		return status;

	utf8_to_ucs2((const gchar *)fc_str, (gunichar2 *)name, LF_FACESIZE);
	return Ok;
}

// coverity[+alloc : arg-*0]
GpStatus WINGDIPAPI
GdipGetGenericFontFamilySansSerif (GpFontFamily **nativeFamily)
{
	const WCHAR MSSansSerif[] = {'M','S',' ','S','a','n','s',' ', 'S','e','r','i','f', 0};
	GpStatus status = Ok;

	if (!nativeFamily)
		return InvalidParameter;
	
#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_lock (&generic);
#else
	g_static_mutex_lock (&generic);
#endif

	if (ref_familySansSerif == 0)
		status = GdipCreateFontFamilyFromName (MSSansSerif, NULL, &familySansSerif);

	if (status == Ok)
		ref_familySansSerif++;
	else
		familySansSerif = NULL;

#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_unlock (&generic);
#else
	g_static_mutex_unlock (&generic);
#endif

	*nativeFamily = familySansSerif;    
	return status;
}

// coverity[+alloc : arg-*0]
GpStatus WINGDIPAPI
GdipGetGenericFontFamilySerif (GpFontFamily **nativeFamily)
{
	const WCHAR Serif[] = {'S','e','r','i','f', 0};
	GpStatus status = Ok;
	
	if (!nativeFamily)
		return InvalidParameter;
	
#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_lock (&generic);
#else
	g_static_mutex_lock (&generic);
#endif

	if (ref_familySerif == 0)
		status = GdipCreateFontFamilyFromName (Serif, NULL, &familySerif);

	if (status == Ok)
		ref_familySerif++;
	else
		familySerif = NULL;

#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_unlock (&generic);
#else
	g_static_mutex_unlock (&generic);
#endif

	*nativeFamily = familySerif;
	return status;
}

// coverity[+alloc : arg-*0]
GpStatus WINGDIPAPI
GdipGetGenericFontFamilyMonospace (GpFontFamily **nativeFamily)
{
	const WCHAR Monospace[] = {'C','o','u','r','i', 'e', 'r', ' ', 'N', 'e', 'w', 0};
	GpStatus status = Ok;

	if (!nativeFamily)
		return InvalidParameter;
	
#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_lock (&generic);
#else
	g_static_mutex_lock (&generic);
#endif

	if (ref_familyMonospace == 0)
		status = GdipCreateFontFamilyFromName (Monospace, NULL, &familyMonospace);

	if (status == Ok)
		ref_familyMonospace++;
	else
		familyMonospace = NULL;

#if GLIB_CHECK_VERSION(2,32,0)
	g_mutex_unlock (&generic);
#else
	g_static_mutex_unlock (&generic);
#endif

	*nativeFamily = familyMonospace;    
	return status;
}

/* OpenType's OS/2 fsSelection Table:
 *
 * http://www.microsoft.com/typography/otspec/os2.htm#fss
 */
enum fsSelection {
	fsSelectionItalic         = (1 << 0),
	fsSelectionUnderscore     = (1 << 1),
	fsSelectionNegative       = (1 << 2),
	fsSelectionOutlined       = (1 << 3),
	fsSelectionStrikeout      = (1 << 4),
	fsSelectionBold           = (1 << 5),
	fsSelectionRegular        = (1 << 6),
	fsSelectionUseTypoMetrics = (1 << 7),
	fsSelectionWWS            = (1 << 8),
	fsSelectionOblique        = (1 << 9),
};

static void
gdip_get_fontfamily_details_from_freetype (GpFontFamily *family, FT_Face face)
{
	if (FT_IS_SFNT (face)) {
		TT_HoriHeader *hhea = FT_Get_Sfnt_Table (face, ft_sfnt_hhea);
		TT_OS2 *os2 = FT_Get_Sfnt_Table (face, ft_sfnt_os2);
		
		if (os2 && (os2->fsSelection & fsSelectionUseTypoMetrics)) {
			/* Use the typographic Ascender, Descender, and LineGap values for everything. */
			family->linespacing = os2->sTypoAscender - os2->sTypoDescender + os2->sTypoLineGap;
			family->celldescent = -os2->sTypoDescender;
			family->cellascent = os2->sTypoAscender;
		} else {
			/* Calculate the LineSpacing for both the hhea table and the OS/2 table. */
			int hhea_linespacing = hhea->Ascender + abs (hhea->Descender) + hhea->Line_Gap;
			int os2_linespacing = os2 ? (os2->usWinAscent + os2->usWinDescent) : 0;
			
			/* The LineSpacing is the maximum of the two sumations. */
			family->linespacing = MAX (hhea_linespacing, os2_linespacing);
			
			/* If the OS/2 table exists, use usWinDescent as the
			 * CellDescent. Otherwise use hhea's Descender value. */
			family->celldescent = os2 ? os2->usWinDescent : hhea->Descender;
			
			/* If the OS/2 table exists, use usWinAscent as the
			 * CellAscent. Otherwise use hhea's Ascender value. */
			family->cellascent = os2 ? os2->usWinAscent : hhea->Ascender;
		}
	} else {
		/* Fall back to using whatever FreeType2 provides. */
		family->celldescent = -face->descender;
		family->cellascent = face->ascender;
		family->linespacing = face->height;
	}
	
	family->height = face->units_per_EM;
}

#ifdef USE_PANGO_RENDERING

PangoFontDescription*
gdip_get_pango_font_description (GpFont *font)
{
	if (!font->pango) {
		font->pango = pango_font_description_new ();
		pango_font_description_set_family (font->pango, (char *)font->face);
		pango_font_description_set_size (font->pango, font->emSize * PANGO_SCALE);

		if (font->style & FontStyleBold)
			pango_font_description_set_weight (font->pango, PANGO_WEIGHT_BOLD);

		if (font->style & FontStyleItalic)
			pango_font_description_set_style (font->pango, PANGO_STYLE_ITALIC);
	}
	return font->pango;
}

static GpStatus
gdip_get_fontfamily_details (GpFontFamily *family, FontStyle style)
{
	GpFont *font = NULL;
	GpStatus status = GdipCreateFont (family, 8.0f, style, UnitPoint, &font);

	if ((status == Ok) && font) {
		PangoFontMap *map = family->collection->pango_font_map;
#if PANGO_VERSION_CHECK(1,22,0)
		PangoContext *context = pango_font_map_create_context (PANGO_FONT_MAP (map));
#else
		PangoContext *context = pango_cairo_font_map_create_context ((PangoCairoFontMap*)map);
#endif
		PangoFont *pf = pango_font_map_load_font (map, context, gdip_get_pango_font_description (font));

		FT_Face face = pango_fc_font_lock_face ((PangoFcFont*)pf);
		if (face) {
			gdip_get_fontfamily_details_from_freetype (family, face);
			pango_fc_font_unlock_face ((PangoFcFont*)pf);
		} else {
			status = FontFamilyNotFound;
		}

		g_object_unref (pf);
		g_object_unref (context);
	}

	if (font)
		GdipDeleteFont (font);
	return status;
}

#else

cairo_font_face_t*
gdip_get_cairo_font_face (GpFont *font)
{
	if (!font->cairofnt) {
		FcPattern *pattern = FcPatternBuild (
			FcPatternDuplicate (font->family->pattern),
			FC_SLANT,  FcTypeInteger, ((font->style & FontStyleItalic) ? FC_SLANT_ITALIC : FC_SLANT_ROMAN), 
			FC_WEIGHT, FcTypeInteger, ((font->style & FontStyleBold)   ? FC_WEIGHT_BOLD  : FC_WEIGHT_MEDIUM),
			NULL);

		font->cairofnt = cairo_ft_font_face_create_for_pattern (pattern);
		cairo_font_face_reference (font->cairofnt);
		FcPatternDestroy (pattern);
	}
	return font->cairofnt;
}

static GpStatus
gdip_get_fontfamily_details (GpFontFamily *family, FontStyle style)
{
	GpFont *font = NULL;
	GpStatus status = GdipCreateFont (family, 0.1f, style, UnitPoint, &font);

	if ((status == Ok) && font) {
		cairo_scaled_font_t* scaled_ft;
		FT_Face face = NULL;
		cairo_matrix_t matrix1, matrix2;
			cairo_font_options_t *options = cairo_font_options_create ();
		cairo_font_face_t* cairofnt = gdip_get_cairo_font_face (font);

		cairo_matrix_init (&matrix1, 1, 0, 0, 1, 0, 0);
		cairo_matrix_init (&matrix2, 1, 0, 0, 1, 0, 0);
		scaled_ft = cairo_scaled_font_create (cairofnt, &matrix1, &matrix2, options);
		/* a missing fonts.conf will resuls in a NULL *scaled_ft (#78237) */
		if (!scaled_ft) {
			static int flag = 0;
			if (flag == 0) {
				g_warning ("couldn't lock the font face. this may be due to a missing fonts.conf on the system.");
				flag = 1;
			}
			status = FontFamilyNotFound;
		}

		if (status == Ok)
			face = cairo_ft_scaled_font_lock_face (scaled_ft);

		cairo_font_options_destroy (options);

		if (face) {
			gdip_get_fontfamily_details_from_freetype (family, face);

			cairo_ft_scaled_font_unlock_face (scaled_ft);
			cairo_scaled_font_destroy (scaled_ft);
		} else {
			status = FontFamilyNotFound;
		}
	}

	if (font)
		GdipDeleteFont (font);
	return status;
}
#endif

GpStatus WINGDIPAPI
GdipGetEmHeight (GDIPCONST GpFontFamily *family, INT style, UINT16 *EmHeight)
{
	GpStatus status = Ok;

	if (!family || !EmHeight)
		return InvalidParameter;

	if (family->height == -1)
		status = gdip_get_fontfamily_details ((GpFontFamily*)family, style);

	*EmHeight = family->height;
	return status;
}

GpStatus WINGDIPAPI
GdipGetCellAscent (GDIPCONST GpFontFamily *family, INT style, UINT16 *CellAscent)
{
	GpStatus status = Ok;

	if (!family || !CellAscent)
		return InvalidParameter;

	if (family->cellascent == -1)
		status = gdip_get_fontfamily_details ((GpFontFamily*)family, style);

	*CellAscent = family->cellascent;
	return status;
}

GpStatus WINGDIPAPI
GdipGetCellDescent (GDIPCONST GpFontFamily *family, INT style, UINT16 *CellDescent)
{
	GpStatus status = Ok;

	if (!family || !CellDescent)
		return InvalidParameter;

	if (family->celldescent == -1)
		status = gdip_get_fontfamily_details ((GpFontFamily*)family, style);

	*CellDescent = family->celldescent;
	return status;
}

GpStatus WINGDIPAPI
GdipGetLineSpacing (GDIPCONST GpFontFamily *family, INT style, UINT16 *LineSpacing)
{
	GpStatus status = Ok;

	if (!family || !LineSpacing)
		return InvalidParameter;

	if (family->linespacing == -1)
		status = gdip_get_fontfamily_details ((GpFontFamily*)family, style);

	*LineSpacing = family->linespacing;
	return status;
}

GpStatus WINGDIPAPI
GdipIsStyleAvailable (GDIPCONST GpFontFamily *family, INT style, BOOL *IsStyleAvailable)
{
	if (!family || !IsStyleAvailable)
		return InvalidParameter;

	*IsStyleAvailable = TRUE;
	return Ok;    
}

/* Font functions */

// coverity[+alloc : arg-*4]
GpStatus
GdipCreateFont (GDIPCONST GpFontFamily* family, REAL emSize, INT style, Unit unit, GpFont **font)
{
	GpStatus status;
	FcChar8* str;
	FcResult r;
	GpFont *result;
	REAL sizeInPixels;
	
	if (!family || !font || unit == UnitDisplay || unit < UnitWorld || unit > UnitCairoPoint)
		return InvalidParameter;

	r = FcPatternGetString (family->pattern, FC_FAMILY, 0, &str);
	status = gdip_status_from_fontconfig (r);
	if (status != Ok)
		return status;
	
	sizeInPixels = gdip_unit_conversion (unit, UnitPixel, gdip_get_display_dpi(), gtMemoryBitmap, emSize);
		
	result = (GpFont *) GdipAlloc (sizeof (GpFont));
	if (!result)
		return OutOfMemory;

	result->sizeInPixels = sizeInPixels;

	result->face = GdipAlloc(strlen((char *)str) + 1);
	if (!result->face) {
		GdipFree(result);
		return OutOfMemory;
	}

	memcpy(result->face, str, strlen((char *)str) + 1);

	result->style = style;
	result->emSize = emSize;
	result->unit = unit;
	status = GdipCloneFontFamily ((GpFontFamily*) family, &result->family);
	if (status != Ok) {
		GdipFree (result);
		return OutOfMemory;
	}

	result->style = style;
#ifdef USE_PANGO_RENDERING
	result->pango = NULL;
#else
	result->cairofnt = NULL;
	gdip_get_cairo_font_face (result);
#endif

	*font = result;	        		
	return Ok;
}

GpStatus WINGDIPAPI
GdipCloneFont (GpFont* font, GpFont** cloneFont)
{
	GpFont *result;
	GpStatus status;

	if (!font || !cloneFont)
		return InvalidParameter;
		
	result = (GpFont *) GdipAlloc (sizeof (GpFont));
	if (!result)
		return OutOfMemory;

	result->sizeInPixels = font->sizeInPixels;
	result->style = font->style;
	result->emSize = font->emSize;
	result->unit = font->unit;

	result->face = (unsigned char*)g_strdup ((char *)font->face);

	status = GdipCloneFontFamily (font->family, &result->family);
	if (status != Ok) {
		GdipFree (result->face);
		GdipFree (result);
		return OutOfMemory;
	}

#ifdef USE_PANGO_RENDERING
	result->pango = NULL;
#else
	result->cairofnt = NULL;
	gdip_get_cairo_font_face (result);
#endif

	*cloneFont = result;
	return Ok;
}

GpStatus WINGDIPAPI
GdipDeleteFont (GpFont* font)
{
	if (!font)
		return InvalidParameter;

	if (font->family)
		GdipDeleteFontFamily (font->family);

#ifdef USE_PANGO_RENDERING
	if (font->pango)
		pango_font_description_free (font->pango);
#else
	if (font->cairofnt)
		cairo_font_face_destroy (font->cairofnt);
#endif

	GdipFree (font->face);
	GdipFree (font);
	return Ok;	       
}

GpStatus WINGDIPAPI
GdipCreateFontFromDC (HDC hdc, GpFont **font)
{
	if (!hdc || !font)
		return InvalidParameter;

	return NotImplemented;
}

static GpStatus
gdip_logfont_from_font (GpFont *font, GpGraphics *graphics, void *lf, BOOL ucs2)
{
	LOGFONTA		*logFont;

	if (!lf)
		return InvalidParameter;

	logFont = (LOGFONTA *)lf;

	/* will be changed back to 1 inside System.Drawing */
	logFont->lfCharSet = 0;

	if (!font || !graphics) {
		int size = (ucs2) ? 2 * LF_FACESIZE : LF_FACESIZE;
		memset (logFont->lfFaceName, 0, size);
		return InvalidParameter;
	}

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

	logFont->lfOutPrecision = 0;
	logFont->lfClipPrecision = 0;

	switch (graphics->text_mode) {
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

	logFont->lfPitchAndFamily = 0;
	if (ucs2) {
		utf8_to_ucs2((const gchar *)font->face, (gunichar2 *)logFont->lfFaceName, LF_FACESIZE);
	} else {
		int len = strlen ((char*)font->face);
		memset (logFont->lfFaceName, 0, LF_FACESIZE);
		memcpy (logFont->lfFaceName, font->face, len < LF_FACESIZE ? len : LF_FACESIZE - 1);
	}
	return Ok;
}

// coverity[+alloc : arg-*1]
GpStatus WINGDIPAPI
GdipCreateFontFromHfontA (HFONT hfont, GpFont **font, void *lf)
{
	GpStatus		status;
	GpFont			*src_font;
	GpFont			*result;

	src_font = (GpFont *)hfont;

	result = (GpFont *) GdipAlloc (sizeof (GpFont));
	if (!result)
		return OutOfMemory;

	result->sizeInPixels = src_font->sizeInPixels;
	result->style = src_font->style;
	status = GdipCloneFontFamily (src_font->family, &result->family);
	if (!status) {
		GdipFree (result);
		return OutOfMemory;
	}

	result->style = src_font->style;
	result->emSize = src_font->emSize;
	result->unit = src_font->unit;

	result->face = GdipAlloc(strlen((char *)src_font->face) + 1);
	if (!result->face) {
		GdipFree(result);
		return OutOfMemory;
	}

	memcpy(result->face, src_font->face, strlen((char *)src_font->face) + 1);

	*font = result;

	return gdip_logfont_from_font (result, NULL, lf, FALSE);
}

GpStatus WINGDIPAPI
GdipGetLogFontW (GpFont *font, GpGraphics *graphics, LOGFONTW *logfontW)
{
	return gdip_logfont_from_font (font, graphics, logfontW, TRUE);
}

GpStatus WINGDIPAPI
GdipGetLogFontA (GpFont *font, GpGraphics *graphics, LOGFONTA *logfontA)
{
	return gdip_logfont_from_font (font, graphics, logfontA, FALSE);
}

static GpStatus
gdip_create_font_from_logfont (HDC hdc, void *lf, GpFont **font, BOOL ucs2)
{
	if (!hdc || !lf || !font)
		return InvalidParameter;

	GpFont *result = (GpFont*) GdipAlloc (sizeof (GpFont));
	if (!result)
		return OutOfMemory;

	LOGFONTA *logfont = (LOGFONTA *)lf;

	if (logfont->lfHeight < 0) {
		result->sizeInPixels = abs (logfont->lfHeight);
	} else {
		result->sizeInPixels = logfont->lfHeight;	// Fixme - convert units
	}
	result->style = 0;
	result->family = NULL;
	/* Fixme - this is wrong, but I don't know of a quick way to get the emSize */
	result->emSize = result->sizeInPixels;
	result->unit = UnitWorld;

	if (logfont->lfItalic) {
		result->style |= FontStyleItalic;
	}
	if (logfont->lfWeight > 400) {
		result->style |= FontStyleBold;
	}
	if (logfont->lfUnderline) {
		result->style |= FontStyleUnderline;
	}
	if (logfont->lfStrikeOut) {
		result->style |= FontStyleStrikeout;
	}

	if (ucs2) {
		result->face = (BYTE*) ucs2_to_utf8 ((const gunichar2 *)logfont->lfFaceName, -1);
		if (!result->face){
			GdipFree (result);
			return OutOfMemory;
		}
	} else {
		result->face = GdipAlloc(LF_FACESIZE);
		if (!result->face){
			GdipFree (result);
			return OutOfMemory;
		}
		memcpy(result->face, logfont->lfFaceName, LF_FACESIZE);
		result->face[LF_FACESIZE - 1] = '\0';
	}

#ifdef USE_PANGO_RENDERING
	result->pango = NULL;
#else
	result->cairofnt = NULL;
#endif

	*font = result;

	return Ok;
}

// coverity[+alloc : arg-*2]
GpStatus
GdipCreateFontFromLogfontA(HDC hdc, GDIPCONST LOGFONTA *logfont, GpFont **font)
{
	return gdip_create_font_from_logfont (hdc, (void *)logfont, font, FALSE);
}

// coverity[+alloc : arg-*2]
GpStatus
GdipCreateFontFromLogfontW(HDC hdc, GDIPCONST LOGFONTW *logfont, GpFont **font)
{
	return gdip_create_font_from_logfont (hdc, (void *)logfont, font, TRUE);
}

GpStatus WINGDIPAPI
GdipPrivateAddMemoryFont(GpFontCollection *fontCollection, GDIPCONST void *memory, INT length)
{
	FcChar8	fontfile[256];
#ifdef WIN32
	FILE	*f;
#else
	int	f;
#endif

	if (!fontCollection || !memory)
		return InvalidParameter;
	if (length <= 0)
		return InvalidParameter;

#ifdef WIN32
	f = CreateTempFile (fontfile);
	if (!f)
		return FileNotFound;

	if (fwrite(memory, sizeof(BYTE), length, f) != length) {
		fclose (f);
		return FileNotFound;
	}

	fclose (f);
#else
	strcpy ((char *) fontfile, "/tmp/ffXXXXXX");
	f = mkstemp ((char *) fontfile);
	
	if (f == -1)
		return FileNotFound;

	if (write (f, memory, length) != length) {
		close (f);
		return FileNotFound;
	}
	close (f);
#endif

	FcConfigAppFontAddFile (fontCollection->config, fontfile);
	/* FIXME - May we delete our temporary font file or does 
	   FcConfigAppFontAddFile just reference our file?  */
	/* unlink(fontfile); */

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetFontHeight (GDIPCONST GpFont *font, GDIPCONST GpGraphics *graphics, REAL *height)
{
	GpStatus status;
	UINT16 emHeight, lineSpacing;
	REAL emSize, h;

	if (!font || !height)
		return InvalidParameter;

	status = GdipGetEmHeight (font->family, font->style, &emHeight);
	if (status != Ok)
		return status;

	status = GdipGetLineSpacing (font->family, font->style, &lineSpacing);
	if (status != Ok)
		return status;

	/* Operations in display dpi's */	
	emSize = gdip_unit_conversion (font->unit, UnitPixel, gdip_get_display_dpi (), gtMemoryBitmap, font->emSize);

	h = lineSpacing * (emSize / emHeight);
	if (!graphics)
		*height = h;
	else
		*height = gdip_unit_conversion (UnitPixel, graphics->page_unit, gdip_get_display_dpi (), graphics->type, h);

	return Ok;
}

GpStatus WINGDIPAPI
GdipGetFontHeightGivenDPI (GDIPCONST GpFont *font, REAL dpi, REAL *height)
{
	GpStatus status;
	UINT16 emHeight, lineSpacing;
	REAL h;

	if (!font || !height)
		return InvalidParameter;

	status = GdipGetEmHeight (font->family, font->style, &emHeight);
	if (status != Ok)
		return status;

	status = GdipGetLineSpacing (font->family, font->style, &lineSpacing);
	if (status != Ok)
		return status;

	h = lineSpacing * (font->emSize / emHeight);
	*height = gdip_unit_conversion (font->unit, UnitInch, dpi, gtMemoryBitmap, h) * dpi;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetFontSize (GpFont *font, REAL *size)
{
	if (!font ||!size)
		return InvalidParameter;

	*size = font->emSize;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetFontStyle (GpFont *font, INT *style)
{
	if (!font || !style)
		return InvalidParameter;

	*style = font->style;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetFontUnit (GpFont *font, Unit *unit)
{
	if (!font || !unit)
		return InvalidParameter;

	*unit = font->unit;
	return Ok;
}

GpStatus WINGDIPAPI
GdipGetFamily (GpFont *font, GpFontFamily **family)
{
	if (!font || !family)
		return InvalidParameter;

	return GdipCloneFontFamily (font->family, family);
}
