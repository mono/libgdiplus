/*
 * Copyright (c) 2003 Alexandre Pigolkine, Novell Inc.
 * Copyright (C) 2006-2007 Novell, Inc (http://www.novell.com)
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
 *   Alexandre Pigolkine (pigolkine@gmx.de)
 *   Duncan Mak (duncan@ximian.com)
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "gdiplus-private.h"

#ifndef USE_PANGO_RENDERING

#include <ctype.h>

#include "text-cairo-private.h"
#include "graphics-private.h"
#include "graphics-cairo-private.h"
#include "brush-private.h"
#include "font-private.h"

/*
 * NOTE: all parameter's validations are done inside text.c
 */

#undef DRAWSTRING_DEBUG

static int
CalculateStringWidths (cairo_t *ct, GDIPCONST GpFont *gdiFont, GDIPCONST gunichar2 *stringUnicode, unsigned long StringDetailElements, GpStringDetailStruct *StringDetails)
{
	size_t			i;
	cairo_text_extents_t	ext;
	GpStringDetailStruct	*CurrentDetail;
	BYTE utf8[5];

	CurrentDetail = StringDetails;

	for (i = 0; i < StringDetailElements; i++) {
		utf8[utf8_encode_ucs2char(*(stringUnicode + i), utf8)] = '\0';
		cairo_text_extents(ct, (const char *) utf8, &ext);
		CurrentDetail->Width = ext.x_advance;
		CurrentDetail++;
	}

	return StringDetailElements;
}

#ifdef NOT_USED
/* Currently not used */
static int
CalculateStringWidthsUTF8 (cairo_t *ct, GDIPCONST GpFont *gdiFont, const BYTE *utf8, unsigned long StringDetailElements, GpStringDetailStruct *StringDetails)
{
	FT_Face			face;
	size_t			i;
	gunichar		*ucs4 = NULL;
	cairo_font_face_t	*Font;
	GpStringDetailStruct	*CurrentDetail;
	glong			NumOfGlyphs;
	cairo_matrix_t		matrix;

#ifdef DRAWSTRING_DEBUG
	printf("CalculateStringWidths(font, %s, %d, details) called\n", utf8, StringDetailElements);
#endif

	Font = (cairo_font_face_t *)gdiFont->cairofnt;
	face = gdip_cairo_ft_font_lock_face(Font);
	if (!face)
		return 0;

	cairo_get_font_matrix(ct, &matrix);	
	cairo_matrix_scale(&matrix, gdiFont->sizeInPixels, gdiFont->sizeInPixels);

	ucs4 = g_utf8_to_ucs4 ((const gchar *) utf8, (glong)-1, NULL, &NumOfGlyphs, NULL);

	if ((NumOfGlyphs == 0) || (ucs4 == NULL)) {
		return 0;
	}

	CurrentDetail=StringDetails;
	for (i = 0; i < NumOfGlyphs; i++) {
		FT_Load_Glyph (face, FT_Get_Char_Index (face, ucs4[i]), FT_LOAD_DEFAULT);
		CurrentDetail->Width = DOUBLE_FROM_26_6 (face->glyph->advance.x);
		CurrentDetail++;
	}

	gdip_cairo_ft_font_unlock_face(Font);

	GdipFree(ucs4);

	
#ifdef DRAWSTRING_DEBUG
	printf("CalculateStringWidths: string >%s< translated into %d glyphs\n", utf8, NumOfGlyphs);
#endif
	return NumOfGlyphs;
}
#endif

static GpStatus
MeasureString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int *length, GDIPCONST GpFont *font, 
	GDIPCONST RectF *rc_org, GDIPCONST GpStringFormat *format, GpBrush *brush, RectF *boundingBox, 
	int *codepointsFitted, int *linesFilled, 
	WCHAR *CleanString, GpStringDetailStruct* StringDetails, GpDrawTextData *data)
{
	BYTE			*String;		/* Holds the UTF8 version of our sanitized string */
	unsigned long		StringLen;		/* Length of CleanString */
	GDIPCONST WCHAR		*Src;
	WCHAR	 		*Dest;
	unsigned long		i;
	unsigned long		j;
	GpStringDetailStruct	*CurrentDetail;
	GpStringDetailStruct	*CurrentLineStart;	/* For rendering engine, to bump LineLen */
	float			*TabStops;
	int			NumOfTabStops;
	int			WrapPoint;		/* Array index of wrap character */
	int			WrapX;			/* Width of text at wrap character */
	float			CursorX;		/* Current X position of drawing cursor */
	float			CursorY;		/* Current Y position of drawing cursor */
	int			MaxX;			/* Largest X of cursor */
	int			MaxXatY;		/* Y coordinate of line with largest X, needed for MaxX resetting on wrap */
	int			MaxY;			/* Largest Y of cursor */
	int			FrameWidth;		/* rc->Width (or rc->Height if vertical) */
	int			FrameHeight;		/* rc->Height (or rc->Width if vertical) */
	int			AlignHorz;		/* Horizontal Alignment mode */
	int			AlignVert;		/* Vertical Alignment mode */
	int			LineHeight;		/* Height of a line with given font */
	cairo_font_extents_t	FontExtent;		/* Info about our font */
	cairo_font_options_t	*FontOptions;
	RectF 			rc_coords, *rc = &rc_coords;
	float			FontSize;

	if (OPTIMIZE_CONVERSION (graphics)) {
		rc->X = rc_org->X;
		rc->Y = rc_org->Y;
		rc->Width = rc_org->Width;
		rc->Height = rc_org->Height;
	} else {
		rc->X = gdip_unitx_convgr (graphics, rc_org->X);
		rc->Y = gdip_unity_convgr (graphics, rc_org->Y);
		rc->Width = gdip_unitx_convgr (graphics, rc_org->Width);
		rc->Height = gdip_unity_convgr (graphics, rc_org->Height);
	}

#ifdef DRAWSTRING_DEBUG
	printf("GdipDrawString(...) called (length=%d, fontsize=%d)\n", length, (int)font->sizeInPixels);
#endif

	TabStops = format->tabStops;
	NumOfTabStops = format->numtabStops;

	/* Prepare our various buffers and variables */
	StringLen = *length;
	if (data)
		data->has_hotkeys = FALSE;

	/*
	  Set aliasing mode
	*/
	FontOptions = cairo_font_options_create();
	
	switch(graphics->text_mode) {
		default:
		case TextRenderingHintSystemDefault: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_DEFAULT);
			//cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_NONE);
			//cairo_font_options_set_subpixel_order(FontOptions, CAIRO_SUBPIXEL_ORDER_DEFAULT);
			//cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_DEFAULT);
			//cairo_font_options_set_hint_metrics(FontOptions, CAIRO_HINT_METRICS_DEFAULT);
			break;
		}

		case TextRenderingHintSingleBitPerPixelGridFit: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_NONE);
			cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_MEDIUM);
			cairo_font_options_set_hint_metrics(FontOptions, CAIRO_HINT_METRICS_ON);
			break;
		}
		case TextRenderingHintSingleBitPerPixel: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_NONE);
			cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_NONE);
			cairo_font_options_set_hint_metrics(FontOptions, CAIRO_HINT_METRICS_OFF);
			break;
		}
		case TextRenderingHintAntiAliasGridFit: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_GRAY);
			cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_MEDIUM);
			cairo_font_options_set_hint_metrics(FontOptions, CAIRO_HINT_METRICS_ON);
			break;
		}
		case TextRenderingHintAntiAlias: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_GRAY);
			cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_NONE);
			cairo_font_options_set_hint_metrics(FontOptions, CAIRO_HINT_METRICS_OFF);
			break;
		}
		case TextRenderingHintClearTypeGridFit: {
			cairo_font_options_set_antialias(FontOptions, CAIRO_ANTIALIAS_SUBPIXEL);
			cairo_font_options_set_hint_style(FontOptions, CAIRO_HINT_STYLE_MEDIUM);
			cairo_font_options_set_hint_metrics(FontOptions, CAIRO_HINT_METRICS_ON);
			break;
		}
	}

	cairo_set_font_options(graphics->ct, FontOptions);
	cairo_font_options_destroy(FontOptions);

	// Do we want this here?

/* Commented out until we properly save/restore AA settings; should fix bug #76135
	cairo_set_antialias(graphics->ct, CAIRO_ANTIALIAS_NONE);
*/

	/*
	   Get font size information; how expensive is the cairo stuff here? 
	*/	
	cairo_set_font_face (graphics->ct, (cairo_font_face_t*) font->cairofnt);	/* Set our font; this will also be used for later drawing */

	/* this will always return the same value, except when printing */
	FontSize = (graphics->type == gtPostScript) ? font->emSize : font->sizeInPixels;
	cairo_set_font_size (graphics->ct, FontSize);
	
	cairo_font_extents (graphics->ct, &FontExtent);		/* Get the size we're looking for */
/*	cairo_font_set_transform(font->cairofnt, SavedMatrix);*/	/* Restore the matrix */

	if ((LineHeight=FontExtent.ascent)<1) {
		LineHeight=1;
	}

#ifdef DRAWSTRING_DEBUG
	printf("Font extents: ascent:%d, descent: %d, height:%d, maxXadvance:%d, maxYadvance:%d\n", (int)FontExtent.ascent, (int)FontExtent.descent, (int)FontExtent.height, (int)FontExtent.max_x_advance, (int)FontExtent.max_y_advance);
#endif

	/* Sanitize string, remove formatting chars and build description array */
#ifdef DRAWSTRING_DEBUG
	printf("GdipDrawString(...) Sanitizing string, StringLen=%d\n", StringLen);
#endif

	Src=stringUnicode;

	/* unless specified we don't consider the trailing spaces, unless there is just one space (#80680) */
	if ((format->formatFlags & StringFormatFlagsMeasureTrailingSpaces) == 0) {
		while ((StringLen > 0) && (isspace ((int) ( *(Src + StringLen - 1)))))
			StringLen--;
		if (StringLen == 0)
			StringLen = 1;
	}

	Dest=CleanString;
	CurrentDetail=StringDetails;
	for (i=0; i<StringLen; i++) {
		switch(*Src) {
			case '\r': { /* CR */
				Src++;
				continue;
			}

			case '\t': { /* Tab */
				if (NumOfTabStops > 0) {
					CurrentDetail->Flags |= STRING_DETAIL_TAB;
				}
				Src++;
				continue;
			}

			case '\n': { /* LF */
				CurrentDetail->Flags |= STRING_DETAIL_LF;
				CurrentDetail->Linefeeds++;
				Src++;
				continue;
			}

			case '&': {
				/* We print *all* chars if no hotkeys */
				if (format->hotkeyPrefix==HotkeyPrefixNone) {
					break;
				}

				Src++;
				if (*Src=='&') {
					/* We skipped the first '&', the break will 
					   make us drop to copying the second */
					break;
				}
				CurrentDetail->Flags |= STRING_DETAIL_HOTKEY;
				if (data)
					data->has_hotkeys = TRUE;
				continue;
			}

			/* Boy, this must be slow, FIXME somehow */
			default: {
				if (((format->formatFlags & StringFormatFlagsNoWrap)==0) || ((format->trimming != StringTrimmingCharacter) && (format->trimming != StringTrimmingNone))) {
					break;
				}
			}
			/* fall through */
			case ' ':
			case '.':
			{
				/* Mark where we can break for a new line */
				CurrentDetail->Flags |= STRING_DETAIL_BREAK;
				break;
			}

		}
		*Dest=*Src;
		Src++;
		Dest++;
		CurrentDetail++;
	}
	*Dest='\0';	
	
	/* Recalculate StringLen; we may have shortened String */
	Dest=CleanString;
	StringLen=0;
	while (*Dest!=0) {
		StringLen++;
		Dest++;
	}

	/* Don't bother doing anything else if the length is 0 */
	if (StringLen == 0) {
		*length = 0;
		return Ok;
	}
	
	/* Convert string from Gdiplus format to UTF8, suitable for cairo */
	String = (BYTE*) utf16_to_utf8 ((const gunichar2 *)CleanString, -1);
	if (!String)
		return OutOfMemory;

#ifdef DRAWSTRING_DEBUG
	printf("Sanitized string: >%s<, length %d (utf8-length:%d)\n", String, StringLen, strlen((char *)String));
#endif

	/* Generate size array */
	if (CalculateStringWidths (graphics->ct, font, CleanString, StringLen, StringDetails)==0) {
		/* FIXME; pick right return code */
		GdipFree(String);
		return Ok;
	}
	GdipFree (String);

	CursorX=0;
	CursorY=0;
	MaxX=0;
	MaxXatY=0;
	MaxY=0;
	CurrentLineStart=StringDetails;
	CurrentDetail=StringDetails;
	CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
	WrapPoint=-1;
	WrapX=0;

	if (format->formatFlags & StringFormatFlagsDirectionVertical) {
		FrameWidth = SAFE_FLOAT_TO_UINT32 (rc->Height);
		FrameHeight = SAFE_FLOAT_TO_UINT32 (rc->Width);
	} else {
		FrameWidth = SAFE_FLOAT_TO_UINT32 (rc->Width);
		FrameHeight = SAFE_FLOAT_TO_UINT32 (rc->Height);
	}

#ifdef DRAWSTRING_DEBUG
	printf("Frame %d x %d\n", FrameWidth, FrameHeight);
#endif
	for (i=0; i<StringLen; i++) {
		/* Handle tabs and new lines */
		if (CurrentDetail->Flags & STRING_DETAIL_TAB) {
			float	tab_pos;
			int	tab_index;

			tab_pos = format->firstTabOffset;
			tab_index = 0;
			while (CursorX > tab_pos) {
				tab_pos += TabStops[tab_index % NumOfTabStops];
				tab_index++;
			}
			CursorX = tab_pos;
			CurrentLineStart = CurrentDetail;
			CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
		}
		if (CurrentDetail->Flags & STRING_DETAIL_LF) {
			CursorX = 0;
			CursorY += LineHeight * CurrentDetail->Linefeeds;
			CurrentDetail->Flags |= STRING_DETAIL_LINESTART;
			CurrentLineStart = CurrentDetail;
#ifdef DRAWSTRING_DEBUG
			{
				int j;

				for (j=0; j<CurrentDetail->Linefeeds; j++) {
					printf("<LF>\n");
				}
			}
#endif
		}

#ifdef DRAWSTRING_DEBUG
		printf("[%3d] X: %3d, Y:%3d, '%c'  | ", i, (int)CursorX, (int)CursorY, CleanString[i]>=32 ? CleanString[i] : '?');
#endif
		/* Remember where to wrap next, but only if wrapping allowed */
		if (((format->formatFlags & StringFormatFlagsNoWrap)==0) && (CurrentDetail->Flags & STRING_DETAIL_BREAK)) {
			if (CleanString[i] == ' ') {
				WrapPoint=i+1;	/* We skip the break char itself, keeping it at the end of the old line */
			} else {
				WrapPoint=i;
			}

			if (CursorX>MaxX) {
				WrapX=CursorX;
			} else {
				WrapX=MaxX;
			}
#ifdef DRAWSTRING_DEBUG
			printf("<W>");
#endif
		}

		/* New line voids any previous wrap point */
		if (CurrentDetail->Flags & STRING_DETAIL_LINESTART) {
			WrapPoint=-1;
		}

		CurrentDetail->PosX=CursorX;
		CurrentDetail->PosY=CursorY;

		/* Advance cursor */
		CursorX+=CurrentDetail->Width;
		if (MaxX<CursorX) {
			MaxX=CursorX;
			MaxXatY=CursorY;
		}

		/* Time for a new line? Go back to last WrapPoint and wrap */
		if (FrameWidth && CursorX>FrameWidth) {
			if (WrapPoint!=-1) {
				/** Re-Calculate line lengths **/
				/* Old line */
				CurrentLineStart->LineLen-=i-WrapPoint;
				if (MaxXatY==CursorY) {
					MaxX=WrapX;
				}

				/* Remove the trailing space from being counted if we're not supposed to */
				if (((format->formatFlags & StringFormatFlagsMeasureTrailingSpaces)==0) && (WrapPoint>0)) {
					if (CleanString[WrapPoint-1]==' ') {
						if (MaxXatY==CursorY) {
							MaxX-=StringDetails[WrapPoint-1].Width;
						}
						StringDetails[WrapPoint-1].Width=0;
						CurrentLineStart->LineLen--;
					}
				}

				/* New line */
				CurrentLineStart=&(StringDetails[WrapPoint]);
				CurrentLineStart->Flags|=STRING_DETAIL_LINESTART;
				CurrentLineStart->LineLen=0;

				/* Generate CursorX/Y for new line */
				CursorY+=LineHeight;
				CursorX=CurrentLineStart->Width;

				i=WrapPoint;
#ifdef DRAWSTRING_DEBUG
				printf("\n<Forcing break at index %d, CursorX:%f, LineLen:%d>\n", WrapPoint, CursorX, CurrentLineStart->LineLen);
#endif
				CurrentDetail=&(StringDetails[WrapPoint]);
				CurrentDetail->PosX=0;
				CurrentDetail->PosY=CursorY;
				WrapPoint=-1;
			} else {
				/*
				   This line is too long and has no wrap points, check if we need to insert ellipsis.
				   To keep at least a bit of performance, we cheat - we don't actually calculate the
				   size of the elipsis chars but assume that they're always smaller than any other
				   character. And we don't try to hard to fit as many chars as possible.
				*/

				int	EndOfLine;

#ifdef DRAWSTRING_DEBUG
				printf("No wrappoint (yet) set\n");
#endif
				/* Find end of line, index i is the first char no longer visible on the line */
				EndOfLine=i;
				if ((format->formatFlags & StringFormatFlagsNoWrap)==0) {
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & STRING_DETAIL_LF)==0)) {
						EndOfLine++;
					}
				} else {
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & (STRING_DETAIL_LF | STRING_DETAIL_BREAK))==0)) {
						EndOfLine++;
					}
					if (EndOfLine<StringLen) {
						if (StringDetails[EndOfLine].Flags & STRING_DETAIL_BREAK) {
							EndOfLine++;
						}
					}
				}

				if ((format->trimming==StringTrimmingEllipsisWord) || (format->trimming==StringTrimmingEllipsisCharacter)) {
					if (CurrentLineStart->LineLen>3) {
						if (format->trimming==StringTrimmingEllipsisCharacter) {
							CleanString[i-1]='.';
							CleanString[i-2]='.';
							CleanString[i-3]='.';
						} else {
							int	found=0;

							j=i;
							while(j>(i-CurrentLineStart->LineLen)) {
								if (CleanString[j]==' ') {
									CleanString[j]='.';
									CurrentLineStart->LineLen-=i-j-1;

									if ((j+1)<EndOfLine) {
										CleanString[j+1]='.';
										CurrentLineStart->LineLen++;
									}

									if ((j+2)<EndOfLine) {
										CleanString[j+2]='.';
										CurrentLineStart->LineLen++;
									}
									found=1;
									break;
								}
								j--;
							}

							if (!found) {
								CleanString[i-1]='.';
								CleanString[i-2]='.';
								CleanString[i-3]='.';
							}
						}
					}
				} else if (format->trimming==StringTrimmingEllipsisPath) {
					int	k;
					float	LineWidth;

					/* Find end of line, index i is the first char no longer visible on the line */
					EndOfLine=i;
					while (EndOfLine<StringLen && ((StringDetails[EndOfLine].Flags & (STRING_DETAIL_LF | STRING_DETAIL_BREAK))==0)) {
						EndOfLine++;
					}

					/* Whack the center, make sure we've got space in the string */
					if (CurrentLineStart->LineLen>3) {
						j=i-(CurrentLineStart->LineLen/2);
						CleanString[j-1]='.';
						CleanString[j]='.';
						CleanString[j+1]='.';

						/* Have just enough to include our ellipsis */
						LineWidth=0;
						for (k=i-CurrentLineStart->LineLen; k<(j+1); k++) {
							LineWidth+=StringDetails[k].Width;
						}
						CurrentLineStart->LineLen=i-j+3;	/* 3=ellipsis */

						/* Now figure out how many chars from the end of the string we have to copy */
						j+=2;	/* Points to the char behind the last ellipsis */
						k=EndOfLine-1;
						while ((LineWidth+StringDetails[k].Width)<FrameWidth) {
							LineWidth+=StringDetails[k].Width;
							k--;
						}
						memcpy (&CleanString[j], &CleanString[k+1], sizeof(WCHAR)*(EndOfLine-k-1));

						CurrentLineStart->LineLen+=EndOfLine-k-1;
					} 
					
				} else {
#ifdef DRAWSTRING_DEBUG
					/* Just cut off the text */
					printf("End of line at index:%d\n", EndOfLine);
#endif
					CurrentLineStart->LineLen=EndOfLine;
				}

				if ((format->formatFlags & StringFormatFlagsNoWrap)!=0) {
					// Avoid endless loops, always print at least one char
					if (CurrentLineStart->LineLen == 0) {
						CurrentLineStart->LineLen = 1;
					}
					break;
				}

				/* avoid endless loop when wrapping is allowed */
				if (CurrentLineStart->LineLen == 0) {
					CurrentLineStart->LineLen = 1;
				}

				/* New line */
				CurrentLineStart=&(StringDetails[EndOfLine]);
				CurrentLineStart->Flags|=STRING_DETAIL_LINESTART;
				CurrentLineStart->LineLen=0;

				/* Generate CursorX/Y for new line */
				CursorY+=LineHeight;
				CursorX=CurrentLineStart->Width;

				i=EndOfLine;

				CurrentDetail=&(StringDetails[EndOfLine]);
				CurrentDetail->PosX=0;
				CurrentDetail->PosY=CursorY;
			}
		}


		/* Still visible? */
		if ((FrameWidth && CursorX>FrameWidth) || (FrameHeight && ((CursorY>FrameHeight) || ((format->formatFlags & StringFormatFlagsLineLimit) && (CursorY+LineHeight)>FrameHeight)))) {
			CurrentDetail->Flags|=STRING_DETAIL_HIDDEN;
#ifdef DRAWSTRING_DEBUG
			if (CurrentDetail->Flags & STRING_DETAIL_LINESTART) {
				printf("<LSTART-H>");
			} else {
				printf("<H>");
			}
#endif
		} else {
			if (MaxY<CursorY) {
				MaxY=CursorY;
			}
		}

#ifdef DRAWSTRING_DEBUG
		if (i % 3 == 2) {
			printf("\n");
		}
#endif

		CurrentDetail++;

		CurrentLineStart->LineLen++;
	}

	/* We ignored it above, for shorter of calculations, also, add a bit of padding */
	MaxY+=LineHeight+FontExtent.descent;

#ifdef DRAWSTRING_DEBUG
	printf("\n");

	printf("Bounding box: %d x %d\n", MaxX, MaxY);

	printf("Line layout [Total len %d]:\n", StringLen);
	for (i=0; i<StringLen; i++) {
		if (StringDetails[i].Flags & STRING_DETAIL_LF) {
			for (j=0; j<StringDetails[i].Linefeeds; j++) {
				printf("\n");
			}
		}
		if (StringDetails[i].Flags & STRING_DETAIL_LINESTART) {
			printf("[Len %2d %dx%d] ", StringDetails[i].LineLen, (int)StringDetails[i].PosX, (int)StringDetails[i].PosY);
			for (j=0; j<StringDetails[i].LineLen; j++) {
				printf("%c", CleanString[i+j]);
			}
			if (j == 0) {
				break;
			}
			i+=j-1;
			printf("\n");
		}
	}
#endif

	/* Prepare alignment handling */
	AlignHorz = format->alignment;
	if (format->formatFlags & StringFormatFlagsDirectionRightToLeft) {
		if (format->alignment==StringAlignmentNear) {
			AlignHorz=StringAlignmentFar;
		} else if (format->alignment==StringAlignmentFar) {
			AlignHorz=StringAlignmentNear;
		}
	}
	AlignVert = format->lineAlignment;

	/*
	   At this point we know our bounding box, what characters
	   are to be displayed and where every character goes
	*/
	if (boundingBox) {
		boundingBox->X = rc->X;
		boundingBox->Y = rc->Y;
		if (format->formatFlags & StringFormatFlagsDirectionVertical) {
			boundingBox->Width = MaxY;
			boundingBox->Height = MaxX;
		} else {
			boundingBox->Width = MaxX;
			boundingBox->Height = MaxY;
		}
		if ((rc->Width > 0) && (boundingBox->Width > rc->Width)) {
			boundingBox->Width = rc->Width;
		}
		if ((rc->Height > 0) && (boundingBox->Height > rc->Height)) {
			boundingBox->Height = rc->Height;
		}

		/* avoid conversion computations if possible */
		if (!OPTIMIZE_CONVERSION (graphics)) {
			boundingBox->X = gdip_convgr_unitx (graphics, boundingBox->X);
			boundingBox->Y = gdip_convgr_unity (graphics, boundingBox->Y);
			boundingBox->Width = gdip_convgr_unitx (graphics, boundingBox->Width);
			boundingBox->Height = gdip_convgr_unity (graphics, boundingBox->Height);
		}

		switch (AlignVert) {
		case StringAlignmentCenter:
			if (format->formatFlags & StringFormatFlagsDirectionVertical) {
				boundingBox->X += (FrameHeight - MaxY) * 0.5;
			} else {
				boundingBox->Y += (FrameHeight - MaxY) * 0.5;
			}
			break;
		case StringAlignmentFar:
			if (format->formatFlags & StringFormatFlagsDirectionVertical) {
				boundingBox->X += (FrameHeight - MaxY);
			} else {
				boundingBox->Y += (FrameHeight - MaxY);
			}
			break;
		}

		switch (AlignHorz) {
		case StringAlignmentCenter:
			if (format->formatFlags & StringFormatFlagsDirectionVertical) {
				boundingBox->Y += (FrameWidth - MaxX) * 0.5;
			} else {
				boundingBox->X += (FrameWidth - MaxX) * 0.5;
			}
			break;
		case StringAlignmentFar:
			if (format->formatFlags & StringFormatFlagsDirectionVertical) {
				boundingBox->Y += (FrameWidth - MaxX);
			} else {
				boundingBox->X += (FrameWidth - MaxX);
			}
			break;
		}
	}

	if (codepointsFitted) {
		/* how many characters from the string can be drawn in the boundingBox (#76664) */
		double max_width = boundingBox ? (boundingBox->X + boundingBox->Width) : rc->X + min (MaxX, rc->Width);
		double max_height = boundingBox ? (boundingBox->Y + boundingBox->Height) : rc->Y + min (MaxY, rc->Height);
		int charactersFitted;
		for (charactersFitted = 0; charactersFitted < StringLen; charactersFitted++) {
			if ((StringDetails[charactersFitted].PosX + StringDetails[charactersFitted].Width) > max_width)
				break;
			if (StringDetails[charactersFitted].PosY + LineHeight > max_height)
				break;
		}
		*codepointsFitted = charactersFitted;
	}

	if (linesFilled) {
		/* how many *complete* lines fits in our calculated boundingBox */
		double height = (boundingBox) ? boundingBox->Height : min (MaxY, rc->Height);
		*linesFilled = floor (height / LineHeight);
	}

	if (AlignHorz != StringAlignmentNear || AlignVert != StringAlignmentNear) {
		// Update alignment
		int length = 0;
		int current_line_length = 0;
		for (i = 0; i < StringLen; i++) {
			if (i == current_line_length) {
				length = StringDetails[i].LineLen;
				current_line_length = min(length + i, StringLen);
			}

			switch (AlignHorz) {
			case StringAlignmentNear:
				break;
			case StringAlignmentCenter:
				if ((current_line_length == 1) || (StringDetails [current_line_length - 1].PosX > 0)) {
					StringDetails[i].PosX += (FrameWidth - StringDetails [current_line_length - 1].PosX -
						StringDetails [current_line_length - 1].Width) / 2;
				}
				break;
			case StringAlignmentFar:
				StringDetails[i].PosX += FrameWidth - StringDetails [current_line_length - 1].PosX -
					StringDetails [current_line_length - 1].Width;
				break;
			}

			switch (AlignVert) {
			case StringAlignmentNear:
				break;
			case StringAlignmentCenter:
				StringDetails[i].PosY += (FrameHeight - MaxY) / 2;
				break;
			case StringAlignmentFar:
				StringDetails[i].PosY += FrameHeight - MaxY;
				break;
			}
		}
	}

	/* if asked, supply extra data to be reused when drawing the same string */
	if (data) {
		data->align_horz = AlignHorz;
		data->align_vert = AlignVert;
		data->line_height = LineHeight;
		data->max_y = MaxY;
		data->descent = FontExtent.descent;
	}

	return Ok;
}

static GpStatus
DrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, 
	GDIPCONST RectF *rc_org, GDIPCONST GpStringFormat *fmt, GpBrush *brush, 
	WCHAR *CleanString, GpStringDetailStruct* StringDetails, GpDrawTextData *data)
{
	float CursorX = 0.0;		/* Current X position of drawing cursor */
	float CursorY = 0.0;		/* Current Y position of drawing cursor */
	int StringLen = length;
	BOOL SetClipping = FALSE;
	unsigned long	i, j;
	int LineHeight = data->line_height;
	cairo_font_extents_t FontExtent;
	RectF rect, *rc = &rect;

	cairo_font_extents (graphics->ct, &FontExtent);

	if (OPTIMIZE_CONVERSION (graphics)) {
		rc->X = rc_org->X;
		rc->Y = rc_org->Y;
		rc->Width = rc_org->Width;
		rc->Height = rc_org->Height;
	} else {
		rc->X = gdip_unitx_convgr (graphics, rc_org->X);
		rc->Y = gdip_unity_convgr (graphics, rc_org->Y);
		rc->Width = gdip_unitx_convgr (graphics, rc_org->Width);
		rc->Height = gdip_unity_convgr (graphics, rc_org->Height);
	}

	/* Set our clipping rectangle */
	if ((rc->Width!=0) && (rc->Height!=0) && ((fmt->formatFlags & StringFormatFlagsNoClip)==0)) {
#ifdef DRAWSTRING_DEBUG
		printf("Setting clipping rectangle (%f, %f %fx%f)\n", rc->X, rc->Y, rc->Width, rc->Height);
#endif		
		/* We do not call cairo_reset_clip because we want to take previous clipping into account */
		gdip_cairo_rectangle (graphics, rc->X, rc->Y, rc->Width, rc->Height, TRUE);
		cairo_clip (graphics->ct);
		SetClipping = TRUE;
	}

	/* Setup cairo */

	if (brush) {
		gdip_brush_setup (graphics, (GpBrush *)brush);
	} else {
		cairo_set_source_rgb (graphics->ct, 0., 0., 0.);
	}

	for (i=0; i<StringLen; i++) {
		if (StringDetails[i].Flags & STRING_DETAIL_LINESTART) {
			BYTE *String;
			int length = StringDetails[i].LineLen;

			/* To support the LineLimit flag */
			if ((StringDetails[i].Flags & STRING_DETAIL_HIDDEN)!=0){
#ifdef DRAWSTRING_DEBUG
				printf("Hidding partially visible line\n");
#endif
				i=StringLen;
				continue;
			}

			if (length > StringLen - i)
				length = StringLen - i;
			String = (BYTE*) utf16_to_utf8 ((const gunichar2 *)(CleanString+i), length);
#ifdef DRAWSTRING_DEBUG
			printf("Displaying line >%s< (%d chars)\n", String, length);
#endif

			if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
				CursorX = rc->X + StringDetails[i].PosX;
				CursorY = rc->Y + StringDetails[i].PosY + LineHeight;

				gdip_cairo_move_to (graphics, CursorX, CursorY, FALSE, TRUE);
				cairo_show_text (graphics->ct, (const char *) String);
			} else {
				CursorY = rc->Y + StringDetails[i].PosX;
				CursorX = rc->X + StringDetails[i].PosY;

				/* Rotate text for vertical drawing */
				cairo_save (graphics->ct);
				gdip_cairo_move_to (graphics, CursorX, CursorY, FALSE, TRUE);
				cairo_rotate (graphics->ct, PI/2);
				cairo_show_text (graphics->ct, (const char *) String);
				cairo_restore (graphics->ct);
			}

#ifdef DRAWSTRING_DEBUG
			printf("Drawing %d chars at %d x %d (width=%f pixels)\n", StringDetails[i].LineLen, (int)CursorX, (int)CursorY, StringDetails[i+StringDetails[i].LineLen-1].PosX);
#endif
			GdipFree (String);

			if (font->style & (FontStyleUnderline | FontStyleStrikeout)) {
				double line_width = cairo_get_line_width (graphics->ct);

				/* Calculate the width of the line */
				cairo_set_line_width (graphics->ct, 1.0);
				j=StringDetails[i+StringDetails[i].LineLen-1].PosX+StringDetails[i+StringDetails[i].LineLen-1].Width;

				if (font->style & FontStyleStrikeout) {
					if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
						gdip_cairo_move_to (graphics, (int)(CursorX), (int)(CursorY-FontExtent.descent), FALSE, TRUE);
						gdip_cairo_line_to (graphics, (int)(CursorX+j), (int)(CursorY-FontExtent.descent), FALSE, TRUE);
					} else {
						gdip_cairo_move_to (graphics, (int)(CursorX+FontExtent.descent), (int)(CursorY), FALSE, TRUE);
						gdip_cairo_line_to (graphics, (int)(CursorX+FontExtent.descent), (int)(CursorY+j), FALSE, TRUE);
					}
				}

				if (font->style & FontStyleUnderline) {
					if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
						gdip_cairo_move_to (graphics, (int)(CursorX), (int)(CursorY+FontExtent.descent-2), FALSE, TRUE);
						gdip_cairo_line_to (graphics, (int)(CursorX+j), (int)(CursorY+FontExtent.descent-2), FALSE, TRUE);
					} else {
						gdip_cairo_move_to (graphics, (int)(CursorX+FontExtent.descent-2), (int)(CursorY), FALSE, TRUE);
						gdip_cairo_line_to (graphics, (int)(CursorX+FontExtent.descent-2), (int)(CursorY+j), FALSE, TRUE);
					}
				}

				cairo_stroke (graphics->ct);
				cairo_set_line_width (graphics->ct, line_width);
			}

			i+=StringDetails[i].LineLen-1;
		}
	}

	/* Handle Hotkey prefix */
	if (fmt->hotkeyPrefix==HotkeyPrefixShow && data->has_hotkeys) {
		GpStringDetailStruct *CurrentDetail = StringDetails;
		for (i=0; i<StringLen; i++) {
			if (CurrentDetail->Flags & STRING_DETAIL_HOTKEY) {
				if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
					CursorX = rc->X + StringDetails[i].PosX;
					CursorY = rc->Y + StringDetails[i].PosY + LineHeight;
				} else {
					CursorY = rc->Y + StringDetails[i].PosX;
					CursorX = rc->X + StringDetails[i].PosY;
				}

				if ((fmt->formatFlags & StringFormatFlagsDirectionVertical)==0) {
					cairo_set_line_width(graphics->ct, 1);
					gdip_cairo_move_to (graphics, (int)(CursorX), (int)(CursorY+FontExtent.descent), FALSE, TRUE);
					gdip_cairo_line_to (graphics, (int)(CursorX+CurrentDetail->Width), (int)(CursorY+FontExtent.descent), FALSE, TRUE);
					cairo_stroke (graphics->ct);
				} else {
					gdip_cairo_move_to (graphics, (int)(CursorX-FontExtent.descent), (int)(CursorY), FALSE, TRUE);
					gdip_cairo_line_to (graphics, (int)(CursorX-FontExtent.descent), (int)(CursorY+CurrentDetail->Width), FALSE, TRUE);
				}
			}
			CurrentDetail++;
		}
	}

	/* Restore the graphics clipping region */
	if (SetClipping)
		cairo_SetGraphicsClip (graphics);

	return Ok;
}

static GpStatus
AllocStringData (WCHAR **clean_string, GpStringDetailStruct **details, int length)
{
	*details = gdip_calloc (length + 1, sizeof (GpStringDetailStruct));
	if (!*details)
		return OutOfMemory;

	*clean_string = GdipAlloc (sizeof (WCHAR) * (length + 1));
	if (!*clean_string) {
		GdipFree (*details);
		return OutOfMemory;
	}

	return Ok;
}

GpStatus
cairo_DrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, INT length, GDIPCONST GpFont *font, GDIPCONST RectF *rc, 
	GDIPCONST GpStringFormat *format, GpBrush *brush)
{
	cairo_matrix_t SavedMatrix;
	GpStringFormat *fmt;
	GpStringDetailStruct *StringDetails;
	WCHAR *CleanString;
	GpDrawTextData data; /* avoid recomputation of stuff done while measuring */
	int StringLen = length;

	GpStatus status = AllocStringData (&CleanString, &StringDetails, length);
	if (status != Ok)
		return status;

	/* a NULL format is valid, it means get the generic default values (and free them later) */
	if (!format) {
		GdipStringFormatGetGenericDefault ((GpStringFormat **)&fmt);
	} else {
		fmt = (GpStringFormat *)format;
	}

	/* is the following ok ? */
	cairo_get_font_matrix (graphics->ct, &SavedMatrix);

	status = MeasureString (graphics, stringUnicode, &StringLen, font, rc, fmt, brush, NULL, NULL, NULL, CleanString, StringDetails, &data);
	if ((status == Ok) && (StringLen > 0)) {
		status = DrawString (graphics, stringUnicode, StringLen, font, rc, fmt, brush, CleanString, StringDetails, &data);
	}

	/* Restore matrix to original values */
	cairo_set_font_matrix (graphics->ct, &SavedMatrix);

	/* Cleanup */
	GdipFree (CleanString);
	GdipFree (StringDetails);

	/* we must delete the default stringformat (when one wasn't provided by the caller) */
	if (format != fmt)
		GdipDeleteStringFormat (fmt);

	return status;
}

GpStatus
cairo_MeasureString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, INT length, GDIPCONST GpFont *font, GDIPCONST RectF *rc,
	GDIPCONST GpStringFormat *format,  RectF *boundingBox, INT *codepointsFitted, INT *linesFilled)
{
	cairo_matrix_t SavedMatrix;
	GpStringFormat *fmt;
	GpStringDetailStruct *StringDetails;
	WCHAR *CleanString;
	int StringLen = length;
	GpStatus status;

	status = AllocStringData (&CleanString, &StringDetails, length);
	if (status != Ok)
		return status;

	/* a NULL format is valid, it means get the generic default values (and free them later) */
	if (!format) {
		GdipStringFormatGetGenericDefault ((GpStringFormat **)&fmt);
	} else {
		fmt = (GpStringFormat *)format;
	}

	/* is the following ok ? */
	cairo_get_font_matrix (graphics->ct, &SavedMatrix);

	status = MeasureString (graphics, stringUnicode, &StringLen, font, rc, fmt, NULL, boundingBox, codepointsFitted, 
		linesFilled, CleanString, StringDetails, NULL);

	/* Restore matrix to original values */
	cairo_set_font_matrix (graphics->ct, &SavedMatrix);

	/* Cleanup */
	GdipFree (CleanString);
	GdipFree (StringDetails);

	/* we must delete the default stringformat (when one wasn't provided by the caller) */
	if (format != fmt)
		GdipDeleteStringFormat (fmt);

	return status;
}

GpStatus
cairo_MeasureCharacterRanges (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, INT length, GDIPCONST GpFont *font, 
	GDIPCONST GpRectF *layout, GDIPCONST GpStringFormat *format, INT regionCount, GpRegion **regions)
{
	int			i, j, start, end;
	int			lineHeight;
	CharacterRange		range;
	GpStringDetailStruct*	StringDetails;
	RectF			charRect;
	RectF 			rc_coords, *layoutRect = &rc_coords;
	BOOL			optimize_convert;
	WCHAR *CleanString;
	GpDrawTextData data; /* avoid recomputation of stuff done while measuring */
	int StringLen = length;

	GpStatus status = AllocStringData (&CleanString, &StringDetails, length);
	if (status != Ok)
		return status;

	/* avoid conversion if possible */
	optimize_convert = OPTIMIZE_CONVERSION (graphics);
	if (optimize_convert) {
		layoutRect->X = layout->X;
		layoutRect->Y = layout->Y;
		layoutRect->Width = layout->Width;
		layoutRect->Height = layout->Height;
	} else {
		layoutRect->X = gdip_unitx_convgr (graphics, layout->X);
		layoutRect->Y = gdip_unity_convgr (graphics, layout->Y);
		layoutRect->Width = gdip_unitx_convgr (graphics, layout->Width);
		layoutRect->Height = gdip_unity_convgr (graphics, layout->Height);
	}

	if (layoutRect->Width <= 0.0) {
		if (layoutRect->Height < 0.0) {
			/* special case only if BOTH values are negative */
			for (i = 0; i < format->charRangeCount; i++)
				GdipSetInfinite (regions [i]);
			status = Ok;
			goto cleanup;
		} else {
			layoutRect->Width = REGION_INFINITE_LENGTH;
		}
	}
	if (layoutRect->Height <= 0.0) {
		layoutRect->Height = REGION_INFINITE_LENGTH;
	}

	/* string measurements */
	status = MeasureString (graphics, stringUnicode, &StringLen, font, layoutRect, format, NULL, NULL, NULL, NULL,
		CleanString, StringDetails, &data);
	if (status != Ok)
		goto cleanup;

	lineHeight = data.line_height + data.descent;

	/* Create a region for every char range */
	for (i = 0; i < format->charRangeCount; i++) {
		range = format->charRanges [i];
		GdipSetEmpty (regions [i]); 

		if (range.Length > 0)
			start = range.First;
		else
			start = range.First + range.Length;

		end = start + range.Length;

		/* sanity check on charRange. negative lengths are allowed */
		if (range.First < 0) {
			status = InvalidParameter;
			goto cleanup;
		}

		if ((start < 0) || (end > length)) {
			status = InvalidParameter;
			goto cleanup;
		}

		/* calculate the regions */
		for (j = start; j < end; j++) {

			/* the prefix char (&) always count - even if we are not actually print it as a char */
			if ((StringDetails[j].Flags & STRING_DETAIL_HOTKEY) && (format->hotkeyPrefix != HotkeyPrefixNone)) {
				end--; /* '&' count as invisible char */
				continue;
			}

			/* workaround the fact that current implementation thinks LF is on the next line */
			if ((j == end - 1) && (StringDetails[j].Flags & STRING_DETAIL_LF))
				continue;

			if (format->formatFlags & StringFormatFlagsDirectionVertical) {
				charRect.X = layoutRect->X + StringDetails [j].PosY;
				charRect.Y = layoutRect->Y + StringDetails [j].PosX;
				charRect.Width = lineHeight;
				charRect.Height = StringDetails [j].Width;
			} else {
				charRect.X = layoutRect->X + StringDetails [j].PosX;
				charRect.Y = layoutRect->Y + StringDetails [j].PosY;
				charRect.Width = StringDetails [j].Width;
				charRect.Height = lineHeight;
			}

			if (optimize_convert) {
				charRect.X = gdip_convgr_unitx (graphics, charRect.X);
				charRect.Y = gdip_convgr_unity (graphics, charRect.Y);
				charRect.Width = gdip_convgr_unitx (graphics, charRect.Width);
				charRect.Height = gdip_convgr_unity (graphics, charRect.Height);
			}

			status = GdipCombineRegionRect (regions [i], &charRect, CombineModeUnion);
			if (status != Ok)
				break;
		}
		if (status != Ok)
			break;
	}

cleanup:
	/* Cleanup */
	GdipFree (CleanString);
	GdipFree (StringDetails);

	return status;
}

#endif
