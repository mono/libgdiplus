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
 *
 * TODO:
 *          - Cairo internal structs?
 *          - Use oriented C as suggested by Peter and Miguel
 */

#include "gdip.h"
#include "gdip_win32.h"
#include <math.h>
#include <glib.h>


/* Family and collections font functions */

GpStatus
GdipNewInstalledFontCollection(GpFontCollection** fontCollection)
{
    printf ("GdipNewInstalledFontCollection\n");
    FcObjectSet* os = FcObjectSetBuild (FC_FAMILY, FC_FOUNDRY, 0);
    FcPattern* pat = FcPatternCreate ();
    FcValue val;

    /* Only Scalable fonts for now */
    val.type = FcTypeBool;
    val.u.b = FcTrue;
    FcPatternAdd (pat, FC_SCALABLE, val, TRUE);
    FcObjectSetAdd (os, FC_SCALABLE);

    FcFontSet* col =  FcFontList(0, pat, os);    
    FcPatternDestroy(pat);
    FcObjectSetDestroy(os);
    
    GpFontCollection *result = (GpFontCollection *) GdipAlloc (sizeof (GpFontCollection));
    result->fontset = col;
    *fontCollection = result;

    return Ok;
}

GpStatus 
GdipDeleteFontFamily(GpFontCollection* fontCollection)
{
    printf ("GdipDeleteFontFamily\n");   
    
    if (fontCollection) {
          FontSetDestroy ((fontCollection)->fontset);
          GdipFree ((void *)fontCollection);
    }

    return Ok;
}

GpStatus
GdipGetFontCollectionFamilyCount(GpFontCollection* fontCollection, int* numFound)
{
    if (!fontCollection || !fontCollection->fontset || !numFound) return InvalidParameter;
    
    *numFound = fontCollection->fontset->nfont;

    printf("GdipDeletePrivateFontCollection %u\n", fontCollection->fontset->nfont);

    return Ok;
}

GpStatus
GdipGetFontCollectionFamilyList(GpFontCollection* fontCollection, int numSought, GpFontFamily** gpfamilies, int* numFound)
{
    printf("GdipGetFontCollectionFamilyList\n");

    if (!fontCollection || !gpfamilies || !numFound) return InvalidParameter;

    GpFontFamily** gpfam = gpfamilies;
    FcPattern** pattern =  fontCollection->fontset->fonts;
    int i;
    
    for (i=0; i < fontCollection->fontset->nfont; gpfam++, pattern++, i++){
      *gpfam = *pattern;      
    }

    *numFound = fontCollection->fontset->nfont;
    return Ok;  
}

GpStatus GdipCreateFontFamilyFromName(GDIPCONST WCHAR *name, GpFontCollection *fontCollection, GpFontFamily **fontFamily)
{
    printf("GdipCreateFontFamilyFromName\n");

    if (!name || !fontFamily) return InvalidParameter;

    glong items_read = 0;
    glong items_written = 0;
    
    char* string = (char*)g_utf16_to_utf8 ((const gunichar2 *)name, -1,
            &items_read, &items_written, NULL);

    if (!fontCollection) {
        
        FcPattern* pat = FcPatternCreate ();
        FcResult rlt;

        /* find the family we want */
        FcValue val;
                                   
        val.type = FcTypeString;
        val.u.s = string;
        FcPatternAdd(pat, FC_FAMILY, val, TRUE);

        FcConfigSubstitute (0, pat, FcMatchPattern);
        FcDefaultSubstitute (pat);                  
        *fontFamily =  FcFontMatch (0, pat, &rlt);

        FcChar8* str;
        FcResult r = FcPatternGetString (*fontFamily, FC_FAMILY, 0, &str);
        printf("fnt->%x, %x, %s\n", *fontFamily, rlt, str);
        g_free(string);  
        return Ok;
    }

    GpFontFamily** gpfam = fontCollection->fontset->fonts;
    FcChar8* str;
    int i; 
    
    for (i=0; i < fontCollection->fontset->nfont; gpfam++, i++){

        FcResult r = FcPatternGetString (*gpfam, FC_FAMILY, 0, &str);

        if (strcmp(string, str)==0) {
            printf("found %s\n", string);
            *fontFamily = *gpfam;
           	g_free(string);
            return Ok;
        }
    }

    g_free(string);
    return FontFamilyNotFound;
}


GpStatus
GdipGetFamilyName(GDIPCONST GpFontFamily* family, WCHAR  name[LF_FACESIZE], int language)
{                
    if (!family) return InvalidParameter;

    return Ok;

    FcChar8* str;
    glong items_read = 0;
    glong items_written = 0;

    FcResult r = FcPatternGetString (family, FC_FAMILY, 0, &str);

    gunichar2* pStr =  g_utf8_to_utf16 ((const gchar *)str, -1,	&items_read, &items_written,NULL);

    printf("GdipGetFamilyName %s, %u\n", str,items_written);

    if (items_written>=sizeof(WCHAR)*(LF_FACESIZE-1))
      items_written=sizeof(WCHAR)*(LF_FACESIZE-1);

    memset (name, 0, sizeof(WCHAR)*LF_FACESIZE);
    memcpy (name, pStr, sizeof(WCHAR)*items_written);
    
    g_free(pStr);

    return Ok;
}

GpStatus
GdipGetGenericFontFamilySansSerif(GpFontFamily **nativeFamily)
{
    const WCHAR MSSansSerif[] = {'M','S',' ','S','a','n','s',' ', 'S','e','r','i','f', 0};
    
    return GdipCreateFontFamilyFromName(MSSansSerif, NULL, nativeFamily);    
}

GpStatus
GdipGetGenericFontFamilySerif(GpFontFamily **nativeFamily)
{
    const WCHAR Serif[] = {'S','e','r','i','f', 0};
    return GdipCreateFontFamilyFromName(Serif, NULL, nativeFamily);
    
}

GpStatus
GdipGetGenericFontFamilyMonospace(GpFontFamily **nativeFamily)
{
    const WCHAR Serif[] = {'S','e','r','i','f', 0};
    return GdipCreateFontFamilyFromName(Serif, NULL, nativeFamily);    
}

/* Font functions */

cairo_ft_font_t *
gdip_font_create (const char *family, int fcslant, int fcweight)
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

//    FcPatternPrint(pat); // debug

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
    return font;
}


/* Selects a font in Cairo without destroying it*/
cairo_status_t
cairo_select_font_nondestructive (cairo_t *cr,
			cairo_font_t *font)
{
    cr->gstate->font = font;
    return CAIRO_STATUS_SUCCESS;
}


GpStatus
GdipCreateFont(GDIPCONST GpFontFamily* family, float emSize, GpFontStyle style, Unit unit,  GpFont **font)
{
    printf("GdipCreateFont\n");

    if (!family || !font) return InvalidParameter;

    FcChar8* str;
    FcResult r = FcPatternGetString (family, FC_FAMILY, 0, &str);

    GpFont *result = (GpFont *) GdipAlloc (sizeof (GpFont));
    int slant = 0;
    int weight = FC_WEIGHT_LIGHT;
    
    result->sizeInPnts = emSize;
    result->sizeInPixels = (emSize * gdip_get_display_dpi()) / 72; // Points to Pixels conversion

    printf("points %f, pixels %f\n", result->sizeInPnts, result->sizeInPixels);

    switch (style) {
        
        case FontStyleRegular:
            break;        
        case FontStyleBold:
            weight = FC_WEIGHT_BOLD;
            break;
        case FontStyleItalic:
            slant = FC_SLANT_ITALIC;
            break;
        case FontStyleBoldItalic:
            weight = FC_WEIGHT_BOLD;
            slant = FC_SLANT_ITALIC;            
            break;            
        case FontStyleUnderline:
            break;
        case FontStyleStrikeout:
            break;
            
        default:
            break;
    }

    result->cairofnt  = gdip_font_create (str, slant, weight);
    *font=result;
        
    return Ok;
}

GpStatus
GdipDeleteFont(GpFont* font)
{
    if (font){        
        cairo_font_destroy(font->cairofnt);
        GdipFree ((void *)font);
    }
}


