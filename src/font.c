/*
 * Copyright (c) 2004 Ximian
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

#include "gdip.h"
#include "gdip_win32.h"
#include <math.h>
#include <glib.h>
#include <freetype/tttables.h>


/* Family and collections font functions */

GpStatus
GdipNewInstalledFontCollection (GpFontCollection **font_collection)
{
	FcObjectSet *os = FcObjectSetBuild (FC_FAMILY, FC_FOUNDRY, 0);
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
    
	file = (unsigned char*) g_utf16_to_utf8 ((const gunichar2 *)filename, -1,
						 NULL, NULL, NULL);
	
	FcConfigAppFontAddFile (font_collection->config, file);
    
	g_free (file);
	return Ok;
}


GpStatus 
GdipDeleteFontFamily (GpFontFamily *FontFamily)
{
	return Ok;
}

void
gdip_createPrivateFontSet (GpFontCollection *font_collection)
{
	FcObjectSet *os = FcObjectSetBuild (FC_FAMILY, FC_FOUNDRY, 0);
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
		*gpfam = *pattern;
	}

	*num_found = font_collection->fontset->nfont;
	return Ok;  
}

GpStatus
GdipCreateFontFamilyFromName (GDIPCONST WCHAR *name, GpFontCollection *font_collection, GpFontFamily **fontFamily)
{
	glong items_read = 0;
	glong items_written = 0;
	unsigned char *string;
	GpFontFamily **gpfam;
	FcChar8 *str;
	int i;
	
	if (!name || !fontFamily)
		return InvalidParameter;

	string = (unsigned char*)g_utf16_to_utf8 ((const gunichar2 *)name, -1,
						  &items_read, &items_written, NULL);

	if (!font_collection) {
		FcChar8 *str;
		FcPattern *pat = FcPatternCreate ();
		FcResult rlt;
		FcResult r;
		
		/* find the family we want */
		FcValue val;
                                   
		val.type = FcTypeString;
		val.u.s = string;
		FcPatternAdd (pat, FC_FAMILY, val, TRUE);

		FcConfigSubstitute (0, pat, FcMatchPattern);
		FcDefaultSubstitute (pat);                  
		*fontFamily =  FcFontMatch (0, pat, &rlt);

		r = FcPatternGetString (*fontFamily, FC_FAMILY, 0, &str);
		g_free (string);
		return Ok;
	}

	gpfam = font_collection->fontset->fonts;
    
	for (i=0; i < font_collection->fontset->nfont; gpfam++, i++){
		FcResult r = FcPatternGetString (*gpfam, FC_FAMILY, 0, &str);

		if (strcmp (string, str)==0) {
			*fontFamily = *gpfam;
			g_free (string);
			return Ok;
		}
	}

	g_free (string);
	return FontFamilyNotFound;
}


GpStatus
GdipGetFamilyName (GDIPCONST GpFontFamily *family, WCHAR name[LF_FACESIZE], int language)
{                
	FcChar8 *fc_str;
	glong items_read = 0;
	glong items_written = 0;
	FcResult r;
	gunichar2 *string;
	
	if (!family)
		return InvalidParameter;

	r = FcPatternGetString (family, FC_FAMILY, 0, &fc_str);

	string =  g_utf8_to_utf16 ((const gchar *)fc_str, -1, &items_read, &items_written,NULL);

	if (items_written>= (LF_FACESIZE-1))
		items_written= (LF_FACESIZE-1);

	memcpy (name, string, items_written * sizeof (WCHAR));
	name [1+items_written*sizeof (WCHAR)]=0;

	g_free (string);

	return Ok;
}

GpStatus
GdipGetGenericFontFamilySansSerif (GpFontFamily **nativeFamily)
{
	const WCHAR MSSansSerif[] = {'M','S',' ','S','a','n','s',' ', 'S','e','r','i','f', 0};
    
	return GdipCreateFontFamilyFromName (MSSansSerif, NULL, nativeFamily);    
}

GpStatus
GdipGetGenericFontFamilySerif (GpFontFamily **nativeFamily)
{
	const WCHAR Serif[] = {'S','e','r','i','f', 0};
	return GdipCreateFontFamilyFromName (Serif, NULL, nativeFamily);
    
}

GpStatus
GdipGetGenericFontFamilyMonospace (GpFontFamily **nativeFamily)
{
	const WCHAR Serif[] = {'S','e','r','i','f', 0};
	return GdipCreateFontFamilyFromName (Serif, NULL, nativeFamily);    
}

GpStatus
GdipGetEmHeight (GDIPCONST GpFontFamily *family, GpFontStyle style, short *EmHeight)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !EmHeight)
		return InvalidParameter;

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font){
		TT_VertHeader *pVert = FT_Get_Sfnt_Table (font->cairofnt->face, ft_sfnt_vhea);
		rslt = pVert->yMax_Extent;

		GdipDeleteFont (font);
	}

	*EmHeight = rslt;
	return Ok;
}

GpStatus
GdipGetCellAscent (GDIPCONST GpFontFamily *family, GpFontStyle style, short *CellAscent)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !CellAscent)
		return InvalidParameter;

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font){
		TT_HoriHeader *pHori = FT_Get_Sfnt_Table (font->cairofnt->face, ft_sfnt_hhea);

		if (pHori)
			rslt = pHori->Ascender;

		GdipDeleteFont (font);
	}

	*CellAscent = rslt;
	return Ok;         
}

GpStatus
GdipGetCellDescent (GDIPCONST GpFontFamily *family, GpFontStyle style, short *CellDescent)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !CellDescent)
		return InvalidParameter;

	*CellDescent = 0;

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font){
		TT_HoriHeader *pHori = FT_Get_Sfnt_Table (font->cairofnt->face, ft_sfnt_hhea);

		if (pHori)
			rslt = -pHori->Descender;

		GdipDeleteFont (font);
	}

	*CellDescent = rslt;
	return Ok;         
}

GpStatus
GdipGetLineSpacing (GDIPCONST GpFontFamily *family, GpFontStyle style, short *LineSpacing)
{
	short rslt = 0;
	GpFont *font = NULL;

	if (!family || !LineSpacing)
		return InvalidParameter;

	GdipCreateFont (family, 0.0f, style, UnitPoint, &font);

	if (font){
		TT_HoriHeader *pHori = FT_Get_Sfnt_Table (font->cairofnt->face, ft_sfnt_hhea);
		rslt = pHori->Ascender + (-pHori->Descender) + pHori->Line_Gap;
		GdipDeleteFont (font);
	}

	*LineSpacing = rslt;
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

cairo_ft_font_t *
gdip_font_create (const unsigned char *family, int fcslant, int fcweight)
{
	cairo_ft_font_t *ft_font = NULL;
	cairo_font_t *font = NULL;
	FcPattern * pat = NULL;
	FT_Library ft_library;
	FT_Error error;

	pat = FcPatternCreate ();
	if (pat == NULL)
		return NULL;

	FcPatternAddString (pat, FC_FAMILY, family);
	FcPatternAddInteger (pat, FC_SLANT, fcslant);
	FcPatternAddInteger (pat, FC_WEIGHT, fcweight);

	error = FT_Init_FreeType (&ft_library);
	if (error) {
		FcPatternDestroy (pat);
		return NULL;
	}

	font = cairo_ft_font_create (ft_library, pat);
	if (font == NULL)
		return NULL;

	ft_font = (cairo_ft_font_t *) font;

	ft_font->owns_ft_library = 1;

	FT_Set_Char_Size (ft_font->face,
			  DOUBLE_TO_26_6 (1.0),
			  DOUBLE_TO_26_6 (1.0),
			  0, 0);

	FcPatternDestroy (pat);
	return ft_font;
}

void
gdip_font_drawunderline (GpGraphics *graphics, GpBrush *brush, float x, float y, float width)
{
        float pos, size;
        cairo_font_extents_t extents;

        cairo_current_font_extents (graphics->ct, &extents);
        pos = 0.5 + ((extents.ascent + extents.descent) *0.1);
        size = 0.5 + ((extents.ascent + extents.descent) *0.05);

        GdipFillRectangle (graphics, brush, x, y +pos, width, size);     
}

void
gdip_font_drawstrikeout (GpGraphics *graphics, GpBrush *brush, float x, float y, float width)
{
        float pos, size;
        cairo_font_extents_t extents;

        cairo_current_font_extents (graphics->ct, &extents);
        pos = 0.5 + ((extents.ascent + extents.descent) *0.5);
        size = 0.5 + ((extents.ascent + extents.descent) *0.05);

        GdipFillRectangle (graphics, brush, x, y -pos, width, size);
}

GpStatus
GdipCreateFont (GDIPCONST GpFontFamily* family, float emSize, GpFontStyle style, Unit unit,  GpFont **font)
{
	FcChar8* str;
	FcResult r;
	GpFont *result;
	int slant = 0;
	int weight = FC_WEIGHT_LIGHT;
	
	if (!family || !font)
		return InvalidParameter;

	r = FcPatternGetString (family, FC_FAMILY, 0, &str);

	result = (GpFont *) GdipAlloc (sizeof (GpFont));

	gdip_unitConversion (unit, UnitPixel, emSize, &result->sizeInPixels);

        if ((style & FontStyleBold) == FontStyleBold)
                weight = FC_WEIGHT_BOLD;

        if ((style & FontStyleItalic) == FontStyleItalic)        
                slant = FC_SLANT_ITALIC;
        
	result->cairofnt  = gdip_font_create (str, slant, weight);
        result->style = style;
	cairo_font_reference ((cairo_font_t *)result->cairofnt);
	result->wineHfont=CreateWineFont(str, style, emSize, unit);
	*font=result;
        
	return Ok;
}

GpStatus
GdipGetHfont(GpFont* font, void **Hfont)
{
	if (font) {
		*Hfont=font->wineHfont;
		return(Ok);
	}
	return InvalidParameter;
}

GpStatus
GdipDeleteFont (GpFont* font)
{
	if (font) {
		cairo_font_destroy ((cairo_font_t *)font->cairofnt);
		GdipFree ((void *)font);
		if (font->wineHfont) {
			DeleteWineFont(font->wineHfont);
		}
                return Ok;
	}
        return InvalidParameter;
}

