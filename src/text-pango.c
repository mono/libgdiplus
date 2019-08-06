/*
 * Copyright (C) 2007 Novell, Inc (http://www.novell.com)
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
 *   Sebastien Pouliot  <sebastien@ximian.com>
 */

#include "gdiplus-private.h"

#ifdef USE_PANGO_RENDERING

#include "text-pango-private.h"
#include "graphics-private.h"
#include "graphics-cairo-private.h"
#include "brush-private.h"
#include "font-private.h"
#include "fontfamily-private.h"
#include "fontcollection-private.h"

int
utf8_length_for_utf16_string (GDIPCONST WCHAR *stringUnicode, int offset, int length)
{
	int utf8_length = 0;
	int end = offset + length;
	int i;
	for (i = offset; i < end; ++i) {
		unsigned short ch = (unsigned short)stringUnicode[i];
		if (ch < 0x80)
			utf8_length += 1;
		else if (ch < 0x800)
			utf8_length += 2;
		else if (ch < 0xD800 || ch >= 0xE000)
			utf8_length += 3;
		else
			utf8_length += 4;
	}
	return utf8_length;
}

/*
 * NOTE: all parameter's validations are done inside text.c
 */

static PangoAttrList*
gdip_get_layout_attributes (PangoLayout *layout)
{
	PangoAttrList *list = pango_layout_get_attributes (layout);
	if (!list)
		list = pango_attr_list_new ();
	else
		pango_attr_list_ref (list);
	return list;
}

void
gdip_set_array_values (int *array, int value, int num)
{
	int i;
	for (i = 0; i < num; i++)
		array [i] = value;
}

static GString *
gdip_process_string (gchar *text, int length, int removeAccelerators, int trimSpace, PangoAttrList *list, int **charsRemoved)
{
	int c, j, r;
	int nonws = 0;
	gchar *iter;
	gchar *iter2;
	gunichar ch;
	GString *res = g_string_sized_new (length);

	/* fast version: just check for final newline and remove */
	if (!removeAccelerators && !trimSpace) {
		j = length;
		if (j > 0 && text [j-1] == '\n') {
			j--;
			if (j > 0 && text [j-1] == '\r')
				j--;
		}
		g_string_append_len (res, text, j);
		if (j == 0 && length > 0) {
			g_string_append_c (res, ' ');
			j++;
		}
		if (charsRemoved && *charsRemoved) {
			int prevj = (g_utf8_prev_char (res->str + j) - res->str);
			gdip_set_array_values (*charsRemoved + prevj, length - j, j - prevj);
		}
		return res;
	}

	iter = text;
	// c: Characters handled in the inner loop, that might be removed.
	j = 0; // Index in output
	r = 0; // Characters removed
	while (iter - text < length) {
		ch = g_utf8_get_char (iter);
		if (ch == GDIP_WINDOWS_ACCELERATOR && removeAccelerators && (iter - text < length - 1)) {
			nonws = 1;
			iter2 = g_utf8_next_char (iter);
			iter = iter2;
			r++;
			ch = g_utf8_get_char (iter);
				/* add an attribute on the next character */
			if (list && (iter - text < length) && (ch != GDIP_WINDOWS_ACCELERATOR)) {
				PangoAttribute *attr = pango_attr_underline_new (PANGO_UNDERLINE_LOW);
				attr->start_index = j;
				attr->end_index = j + g_utf8_next_char (iter) - iter;
				pango_attr_list_insert (list, attr);
			}
		} else if (!g_unichar_isspace (ch)) {
			nonws = 1;
		} else if (trimSpace && ch != '\r' && ch != '\n') {
			/* unless specified we don't consider the trailing spaces, unless there is just one space (#80680) */
			c = 1;
			for (iter2 = g_utf8_next_char (iter); iter2 - text < length; iter2 = g_utf8_next_char (iter2)) {
				ch = g_utf8_get_char (iter2);
				if (ch == '\r' || ch == '\n')
					break;
				if (!g_unichar_isspace (ch)) {
					c = 0;
					g_string_append_len (res, iter, iter2 - iter);
					if (charsRemoved && *charsRemoved)
						gdip_set_array_values ((*charsRemoved)+j, r, iter2 - iter);
					j += iter2 - iter;
					break;
				}
				c++;
			}
			r += c;
			iter = iter2;
			continue;
		} else if ((ch == '\r' && (iter - text == length - 2) && (*g_utf8_next_char (iter) == '\n')) || (ch == '\n' && iter - text == length - 1)) {
			/* in any case, ignore a final newline as pango will add an extra line to the measurement while gdi+ does not */
			r += length - (iter - text);
			break;
		}
		iter2 = g_utf8_next_char (iter);
		g_string_append_len (res, iter, iter2 - iter);
		/* save these for string lengths later */
		if (charsRemoved && *charsRemoved)
			gdip_set_array_values ((*charsRemoved)+j, r, iter2 - iter);
		j += iter2 - iter;
		iter = iter2;
	}

	/* always ensure that at least one space is measured */
	if (!nonws && trimSpace && length > 0) {
		iter = text;
		iter2 = g_utf8_next_char (iter);
		g_string_append_len (res, iter, iter2 - iter);
		j += iter2 - iter;
		r--;
	}
	if (charsRemoved && *charsRemoved && j > 0) {
		int prevj = (g_utf8_prev_char (res->str + j) - res->str);
		gdip_set_array_values (*charsRemoved + prevj, r, j - prevj);
	}
	return res;
}

PangoLayout*
gdip_pango_setup_layout (cairo_t *cr, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font,
	GDIPCONST RectF *rc, RectF *box, PointF *box_offset, GDIPCONST GpStringFormat *format, int **charsRemoved)
{
	GpStringFormat *fmt;
	PangoLayout *layout;
	PangoContext *context;
	PangoRectangle logical;   /* logical size of text (used for alignment) */
	PangoRectangle ink;       /* ink size of text (to pixel boundaries) */
	PangoAttrList *list = NULL;
	GString *ftext;
	PangoTabArray *tabs;
	PangoLayoutIter *iter;
	int i;
	int FrameWidth;     /* rc->Width (or rc->Height if vertical) */
	int FrameHeight;    /* rc->Height (or rc->Width if vertical) */
	//int FrameX;         /* rc->X (or rc->Y if vertical) */
	//int FrameY;         /* rc->Y (or rc->X if vertical) */
	int y0;             /* y0,y1,clipNN used for checking line positions vs. clip rectangle */
	int y1;
	int trimSpace;      /* whether or not to trim the space */
	BOOL use_horizontal_layout;

	gchar *text = utf16_to_utf8 (stringUnicode, length);
	if (!text)
		return NULL;
	length = strlen(text);

	if (charsRemoved) {
		(*charsRemoved) = GdipAlloc (sizeof (int) * length);
		if (!*charsRemoved) {
			GdipFree (text);
		return NULL;
		}
		memset (*charsRemoved, 0, sizeof (int) * length);
	}

	/* TODO - Digit substitution */

// g_warning ("layout >%s< (%d) [x %g, y %g, w %g, h %g] [font %s, %g points]", text, length, rc->X, rc->Y, rc->Width, rc->Height, font->face, font->emSize);

	/* a NULL format is valid, it means get the generic default values (and free them later) */
	if (!format) {
		GpStatus status = GdipStringFormatGetGenericDefault ((GpStringFormat **)&fmt);
		if (status != Ok) {
			GdipFree (text);
			return NULL;
		}
	} else {
		fmt = (GpStringFormat *)format;
	}

	context = pango_font_map_create_context (font->family->collection->pango_font_map);
	pango_cairo_update_context (cr, context);

	layout = pango_layout_new (context);
	g_object_unref (context);

	pango_layout_set_font_description (layout, gdip_get_pango_font_description ((GpFont*) font));

	if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
		FrameWidth = MAKE_SAFE_FOR_PANGO (SAFE_FLOAT_TO_UINT32 (rc->Height));
		FrameHeight = MAKE_SAFE_FOR_PANGO (SAFE_FLOAT_TO_UINT32 (rc->Width));
		//FrameX = SAFE_FLOAT_TO_UINT32 (rc->Y);
		//FrameY = SAFE_FLOAT_TO_UINT32 (rc->X);
	} else {
		FrameWidth = MAKE_SAFE_FOR_PANGO (SAFE_FLOAT_TO_UINT32 (rc->Width));
		FrameHeight = MAKE_SAFE_FOR_PANGO (SAFE_FLOAT_TO_UINT32 (rc->Height));
		//FrameX = SAFE_FLOAT_TO_UINT32 (rc->X);
		//FrameY = SAFE_FLOAT_TO_UINT32 (rc->Y);
	}
	//g_warning("FW: %d\tFH: %d", FrameWidth, FrameHeight);

	if ((FrameWidth <= 0) || (fmt->formatFlags & StringFormatFlagsNoWrap)) {
		pango_layout_set_width (layout, -1);
		use_horizontal_layout = FALSE;
		//g_warning ("Setting width: %d", -1);
	} else {
		pango_layout_set_width (layout, FrameWidth * PANGO_SCALE);
		use_horizontal_layout = TRUE;
		//g_warning ("Setting width: %d", FrameWidth * PANGO_SCALE);
	}

	/* with GDI+ the API not the renderer makes the direction decision */
	pango_layout_set_auto_dir (layout, FALSE);	
	if (fmt->formatFlags & StringFormatFlagsDirectionRightToLeft) {
		pango_context_set_base_dir (context, PANGO_DIRECTION_WEAK_RTL);
		pango_layout_context_changed (layout);

		/* horizontal alignment */
		if (use_horizontal_layout) {
			switch (fmt->alignment) {
			case StringAlignmentNear:
				pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
				break;
			case StringAlignmentCenter:
				pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
				break;
			case StringAlignmentFar:
				pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
				break;			
			}
		}
	} else {
		/* pango default base dir is WEAK_LTR, which is what we want */

		/* horizontal alignment */
		if (use_horizontal_layout) {
			switch (fmt->alignment) {
			case StringAlignmentNear:
				pango_layout_set_alignment (layout, PANGO_ALIGN_LEFT);
				break;
			case StringAlignmentCenter:
				pango_layout_set_alignment (layout, PANGO_ALIGN_CENTER);
				break;
			case StringAlignmentFar:
				pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
				break;
			}
		}
	}

#ifdef PANGO_VERSION_CHECK
#if PANGO_VERSION_CHECK(1,16,0)
	if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
		if (fmt->formatFlags & StringFormatFlagsDirectionRightToLeft) {
			cairo_rotate (cr, M_PI/2.0);
			cairo_translate (cr, 0, -FrameHeight);
			pango_cairo_update_context (cr, context);
		} else {
			cairo_rotate (cr, 3.0*M_PI/2.0);
			cairo_translate (cr, -FrameWidth, 0);
			pango_cairo_update_context (cr, context);
		}
		/* only since Pango 1.16 */
		pango_context_set_base_gravity (context, PANGO_GRAVITY_AUTO);
		pango_context_set_gravity_hint (context, PANGO_GRAVITY_HINT_LINE);
		pango_layout_context_changed (layout);
	}
#endif
#endif

	/* TODO - StringFormatFlagsDisplayFormatControl
		scan and replace them ??? */

	/* Trimming options seem to apply only to the end of the string - gdi+ will still wrap
	 * with preference to word first, then character.  Unfortunately, pango doesn't have
	 * any way to differentiate wrapping behavior from trimming behavior that I could find */
	pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
	switch (fmt->trimming) {
	case StringTrimmingNone:
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_NONE);
		break;
	case StringTrimmingCharacter:
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_NONE);
		break;
	case StringTrimmingWord:
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_NONE);
		break;
	case StringTrimmingEllipsisCharacter:
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
		if (!(fmt->formatFlags & StringFormatFlagsNoWrap))
			pango_layout_set_height (layout, FrameHeight == 0 ? G_MAXINT32 : FrameHeight * PANGO_SCALE);
		break;
	case StringTrimmingEllipsisWord:
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
		if (!(fmt->formatFlags & StringFormatFlagsNoWrap))
			pango_layout_set_height (layout, FrameHeight == 0 ? G_MAXINT32 : FrameHeight * PANGO_SCALE);
		break;
	case StringTrimmingEllipsisPath:
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_MIDDLE);
		if (!(fmt->formatFlags & StringFormatFlagsNoWrap))
			pango_layout_set_height (layout, FrameHeight == 0 ? G_MAXINT32 : FrameHeight * PANGO_SCALE);
		break;
	}

	/* some stuff can only be done by manipulating the attributes (but we can avoid this most of the time) */
	if ((fmt->formatFlags & StringFormatFlagsNoFontFallback) || (font->style & (FontStyleUnderline | FontStyleStrikeout))) {

		list = gdip_get_layout_attributes (layout);

		/* StringFormatFlagsNoFontFallback */
		if (fmt->formatFlags & StringFormatFlagsNoFontFallback) {
			PangoAttribute *attr = pango_attr_fallback_new (FALSE);
			attr->start_index = 0;
			attr->end_index = length;
			pango_attr_list_insert (list, attr);
		}

		if (font->style & FontStyleUnderline) {
			PangoAttribute *attr = pango_attr_underline_new (PANGO_UNDERLINE_SINGLE);
			attr->start_index = 0;
			attr->end_index = length;
			pango_attr_list_insert (list, attr);
		}

		if (font->style & FontStyleStrikeout) {
			PangoAttribute *attr = pango_attr_strikethrough_new (TRUE);
			attr->start_index = 0;
			attr->end_index = length;
			pango_attr_list_insert (list, attr);
		}
	}

	if (fmt->numtabStops > 0) {
		float tabPosition;
		tabs = pango_tab_array_new (fmt->numtabStops, FALSE);
		tabPosition = fmt->firstTabOffset;
		for (i = 0; i < fmt->numtabStops; i++) {
			tabPosition += fmt->tabStops[i];
			pango_tab_array_set_tab (tabs, i, PANGO_TAB_LEFT, (gint)min (tabPosition, PANGO_MAX) * PANGO_SCALE);
		}
		pango_layout_set_tabs (layout, tabs);
		pango_tab_array_free (tabs);
	}

	//g_warning ("length before ws removal: %d", length);
	trimSpace = (fmt->formatFlags & StringFormatFlagsMeasureTrailingSpaces) == 0;
	switch (fmt->hotkeyPrefix) {
	case HotkeyPrefixHide:
		/* we need to remove any accelerator from the string */
		ftext = gdip_process_string (text, length, 1, trimSpace, NULL, charsRemoved);
		break;
	case HotkeyPrefixShow:
		/* optimization: is seems that we never see the hotkey when using an underline font */
		if (font->style & FontStyleUnderline) {
			/* so don't bother drawing it (and simply add the '&' character) */
			ftext = gdip_process_string (text, length, 1, trimSpace, NULL, charsRemoved);
		} else {
			/* find accelerator and add attribute to the next character (unless it's the prefix too) */
			if (!list)
				list = gdip_get_layout_attributes (layout);
			ftext = gdip_process_string (text, length, 1, trimSpace, list, charsRemoved);
		}
		break;
	default:
		ftext = gdip_process_string (text, length, 0, trimSpace, NULL, charsRemoved);
		break;
	}
	length = ftext->len;
	//g_warning ("length after ws removal: %d", length);

	if (list) {
		pango_layout_set_attributes (layout, list);
		pango_attr_list_unref (list);
	}

// g_warning("\tftext>%s< (%d)", ftext->str, -1);
	pango_layout_set_text (layout, ftext->str, ftext->len);
	GdipFree (text);
	g_string_free(ftext, TRUE);

	/* Trim the text after the last line for ease of counting lines/characters */
	/* Also prevents drawing whole lines outside the boundaries if NoClip was specified */
	/* In case of pre-existing clipping, use smaller of clip rectangle or our specified height */
	if (FrameHeight > 0) {
		iter = pango_layout_get_iter (layout);
		do {
			if (iter == NULL)
				break;
			pango_layout_iter_get_line_yrange (iter, &y0, &y1);
			//g_warning("yrange: %d  %d  FrameHeight: %f", y0 / PANGO_SCALE, y1 / PANGO_SCALE, FrameHeight);
			/* StringFormatFlagsLineLimit */
			if (((fmt->formatFlags & StringFormatFlagsLineLimit) && y1 / PANGO_SCALE > FrameHeight) || (y0 / PANGO_SCALE >= FrameHeight)) {
				PangoLayoutLine *line = pango_layout_iter_get_line_readonly (iter);
				pango_layout_set_text (layout, pango_layout_get_text (layout), line->start_index);
				
				break;
			}
		} while (pango_layout_iter_next_line (iter));
		pango_layout_iter_free (iter);
	}

	pango_layout_get_pixel_extents (layout, &ink, &logical);
	//g_warning ("\tlogical\t[x %d, y %d, w %d, h %d][x %d, y %d, w %d, h %d]", logical.x, logical.y, logical.width, logical.height, ink.x, ink.y, ink.width, ink.height);

	if ((fmt->formatFlags & StringFormatFlagsNoFitBlackBox) == 0) {
		/* By default don't allow overhang - ink space may be larger than logical space */
		box->X = min (ink.x, logical.x);
		box->Y = min (ink.y, logical.y);
		box->Height = max (ink.height, logical.height);
		box->Width = max (ink.width, logical.width);
	} else {
		/* Allow overhang */
		box->X = logical.x;
		box->Y = logical.y;
		box->Height = logical.height;
		box->Width = logical.width;
	}
	//g_warning ("\tbox\t[x %g, y %g, w %g, h %g]", box->X, box->Y, box->Width, box->Height);

	box_offset->X = 0;
	box_offset->Y = 0;

	switch (fmt->lineAlignment) {
	case StringAlignmentNear:
		break;
	case StringAlignmentCenter:
		box_offset->Y += (FrameHeight - box->Height) * 0.5;
		break;
	case StringAlignmentFar:
		box_offset->Y += (FrameHeight - box->Height);
		break;
	}

	if (!use_horizontal_layout) {
		switch (fmt->alignment) {
		case StringAlignmentNear:
			if (fmt->formatFlags & StringFormatFlagsDirectionRightToLeft)
				box_offset->X += (FrameWidth - box->Width);
			break;
		case StringAlignmentCenter:
			box_offset->X += (FrameWidth - box->Width) * 0.5;
			break;
		case StringAlignmentFar:
			if (!(fmt->formatFlags & StringFormatFlagsDirectionRightToLeft))
				box_offset->X += (FrameWidth - box->Width);
			break;
		}
	}

	box->X += box_offset->X;
	box->Y += box_offset->Y;

	if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
		float tmp = box->X;
		box->X = box->Y;
		box->Y = tmp;
		tmp = box->Width;
		box->Width = box->Height;
		box->Height = tmp;
		tmp = box_offset->X;
		box_offset->X = box_offset->Y;
		box_offset->Y = tmp;
	}

	box->X += rc->X;
	box->Y += rc->Y;

	//g_warning ("va-box\t[x %g, y %g, w %g, h %g]", box->X, box->Y, box->Width, box->Height);

	pango_cairo_update_layout (cr, layout);

	return layout;
}

GpStatus
pango_DrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, INT length, GDIPCONST GpFont *font, GDIPCONST RectF *rc,
	GDIPCONST GpStringFormat *format, GpBrush *brush)
{
	PangoLayout *layout;
	RectF box;
	PointF box_offset;

	/* Setup cairo */
	if (brush) {
		gdip_brush_setup (graphics, brush);
	} else {
		cairo_set_source_rgb (graphics->ct, 0., 0., 0.);
	}

	cairo_save (graphics->ct);

	layout = gdip_pango_setup_layout (graphics->ct, stringUnicode, length, font, rc, &box, &box_offset, format, NULL);
	if (!layout) {
		cairo_restore (graphics->ct);
		return OutOfMemory;
	}

	if ((rc->Width != 0) && (rc->Height != 0) && (format == NULL || (format->formatFlags & StringFormatFlagsNoClip) == 0)) {
// g_warning ("\tclip [%g %g %g %g]", rc->X, rc->Y, rc->Width, rc->Height);
		/* We do not call cairo_reset_clip because we want to take previous clipping into account */
		/* Use rc instead of frame variables because this is pre-transform */
		gdip_cairo_rectangle (graphics, rc->X, rc->Y, rc->Width, rc->Height, TRUE);
		cairo_clip (graphics->ct);
	}

	gdip_cairo_move_to (graphics, rc->X + box_offset.X, rc->Y + box_offset.Y, FALSE, TRUE);
	pango_cairo_show_layout (graphics->ct, layout);

	g_object_unref (layout);
	cairo_restore (graphics->ct);
	return Ok;
}

GpStatus
pango_MeasureString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, INT length, GDIPCONST GpFont *font, GDIPCONST RectF *rc,
	GDIPCONST GpStringFormat *format, RectF *boundingBox, INT *codepointsFitted, INT *linesFilled)
{
	PangoLayout *layout;
	PangoLayoutLine *line;
	PangoRectangle logical;
	PangoLayoutIter *iter;
	PointF box_offset;
	int *charsRemoved = NULL;

	cairo_save (graphics->ct);

	layout = gdip_pango_setup_layout (graphics->ct, stringUnicode, length, font, rc, boundingBox, &box_offset, format, &charsRemoved);
	if (!layout) {
		cairo_restore (graphics->ct);
		return OutOfMemory;
	}

	if (codepointsFitted || linesFilled) {
		int charsFitted;
		int lines = 0;
		int lastIndex;
		int y0;
		int y1;
		int len;
		double min_x;
		double max_x;
		double max_y;
		const char *layoutText;
		if (format && (format->formatFlags & StringFormatFlagsDirectionVertical)) {
			min_x = 0;
			max_x = rc->Height;
			max_y = rc->Width;
		} else {
			min_x = 0;
			max_x = rc->Width;
			max_y = rc->Height;
		}
		lastIndex = 0;
		iter = pango_layout_get_iter (layout);
		do {
			if (iter == NULL)
				break;
			pango_layout_iter_get_line_yrange (iter, &y0, &y1);
			if ((format && (format->formatFlags & StringFormatFlagsLineLimit) && y1 / PANGO_SCALE > max_y) || y0 / PANGO_SCALE >= max_y)
				break;
			lines++;
			if (pango_layout_iter_at_last_line (iter)) {
				do {
					pango_layout_iter_get_char_extents (iter, &logical);
					/* check both max and min to catch right-to-left text, also width may be negative */
					if ((logical.x / PANGO_SCALE > max_x || (logical.x + logical.width) / PANGO_SCALE > max_x) || (logical.x / PANGO_SCALE < min_x || (logical.x + logical.width) / PANGO_SCALE < min_x))
						break;
					lastIndex = pango_layout_iter_get_index (iter);
				} while (pango_layout_iter_next_char (iter));
				break;
			} else {
				line = pango_layout_iter_get_line_readonly (iter);
				lastIndex = line->start_index + line->length - 1;
			}
		} while (pango_layout_iter_next_line (iter));
		pango_layout_iter_free (iter);

		if (codepointsFitted) {
			layoutText = pango_layout_get_text (layout);

			if (lines > 0)
				len = strlen (layoutText);

			if (lines > 0 && len > 0) {
				/* this can happen when the string ends in a newline */
				if (lastIndex >= len) {
					lastIndex = g_utf8_prev_char(layoutText + len) - layoutText;
				}
				/* Add back in any & characters removed and the final newline characters (if any) */
				charsFitted = g_utf8_strlen (layoutText, g_utf8_next_char (layoutText + lastIndex) - layoutText) + charsRemoved [lastIndex];
				//g_warning("lastIndex: %d\t\tcharsRemoved: %d", lastIndex, charsRemoved[lastIndex]);
				/* safe because of null termination */
				switch (layoutText [lastIndex + 1]) {
					case '\r':
						charsFitted++;
						if (layoutText [lastIndex + 2] == '\n')
							charsFitted++;
						break;
					case '\n':
						charsFitted++;
						break;
				}
			} else {
				// Nothing was fitted. Most likely either the input length was zero or LineLimit prevented fitting any lines (the height of the first line is greater than the height of the bounding box).
				charsFitted = 0;
			}
			*codepointsFitted = charsFitted;
		}

		if (linesFilled) {
			*linesFilled = lines;
		}
	}

	GdipFree (charsRemoved);

	g_object_unref (layout);
	cairo_restore (graphics->ct);
	return Ok;
}

GpStatus
pango_MeasureCharacterRanges (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, INT length, GDIPCONST GpFont *font,
	GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *format, INT regionCount, GpRegion **regions)
{
	PangoLayout *layout;
	GpStatus status = Ok;
	int i, j;
	GpRectF boundingBox;
	GpPointF box_offset;

	if (layoutRect->Width <= 0.0 && layoutRect->Height < 0.0) {
		/* special case only if BOTH values are negative */
		for (i = 0; i < format->charRangeCount; i++)
			GdipSetInfinite (regions [i]);
		return Ok;
	}

	cairo_save (graphics->ct);

	layout = gdip_pango_setup_layout (graphics->ct, stringUnicode, length, font, layoutRect, &boundingBox, &box_offset, format, NULL);
	if (!layout) {
		cairo_restore (graphics->ct);
		return OutOfMemory;
	}

	/* Create a region for every char range */
	for (i = 0; i < format->charRangeCount; i++) {
		int start, end;
		CharacterRange range = format->charRanges [i];

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

		/* calculate the initial UTF-8 index */
		int idxUtf8 = utf8_length_for_utf16_string (stringUnicode, 0, start);

		/* calculate the regions */
		for (j = start; j < end; j++) {
			PangoRectangle box;
			GpRectF charRect;

			pango_layout_index_to_pos (layout, idxUtf8, &box);
			if (format->formatFlags & StringFormatFlagsDirectionVertical) {
				charRect.X = (float)box.y / PANGO_SCALE;
				charRect.Y = (float)box.x / PANGO_SCALE;
				charRect.Width = (float)box.height / PANGO_SCALE;
				charRect.Height = (float)box.width / PANGO_SCALE;
			} else {
				charRect.X = (float)box.x / PANGO_SCALE;
				charRect.Y = (float)box.y / PANGO_SCALE;
				charRect.Width = (float)box.width / PANGO_SCALE;
				charRect.Height = (float)box.height / PANGO_SCALE;
			}
			/* Normalize values (width/height can be negative) */
			if (charRect.Width < 0) {
				charRect.Width = -charRect.Width;
				charRect.X -= charRect.Width;
			}
			if (charRect.Height < 0) {
				charRect.Height = -charRect.Height;
				charRect.Y -= charRect.Height;
			}
			charRect.X += box_offset.X + layoutRect->X;
			charRect.Y += box_offset.Y + layoutRect->Y;
// g_warning ("[%d] [%d : %d-%d] %c [x %g y %g w %g h %g]", i, j, start, end, (char)stringUnicode[j], charRect.X, charRect.Y, charRect.Width, charRect.Height);
			status = GdipCombineRegionRect (regions [i], &charRect, CombineModeUnion);
			if (status != Ok)
				break;

			// update the UTF-8 index
			idxUtf8 += utf8_length_for_utf16_string (stringUnicode, j, 1);
		}
		if (status != Ok)
			break;
	}

cleanup:
	g_object_unref (layout);
	cairo_restore (graphics->ct);
	return status;
}

#endif
