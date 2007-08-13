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

static void
gdip_process_accelerators (gchar *text, int length, PangoAttrList *list)
{
	int i;
	for (i = 0; i < length; i++) {
		if (*(text + i) == GDIP_WINDOWS_ACCELERATOR) {
			/* don't show the prefix character */
			*(text + i) = GDIP_PANGOHACK_ACCELERATOR;
			/* if the next character is an accelerator then skip over it (&& == &) */
			if ((i < length - 1) && (*(text + i + 1) == GDIP_WINDOWS_ACCELERATOR)) {
				i++;
			} else if (list) {
				/* add an attribute on the next character */
				PangoAttribute *attr = pango_attr_underline_new (PANGO_UNDERLINE_LOW);
				attr->start_index = i + 1;
				attr->end_index = i + 2;
				pango_attr_list_insert (list, attr);
			}
		}
	}
}

PangoLayout*
gdip_pango_setup_layout (cairo_t *ct, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, 
	GDIPCONST RectF *rc, RectF *box, GDIPCONST GpStringFormat *format)
{
	GpStringFormat *fmt;
	PangoLayout *layout;
	PangoContext *context;
	PangoMatrix matrix = PANGO_MATRIX_INIT;
	PangoRectangle logical;
	PangoAttrList *list = NULL;

	gchar *text = ucs2_to_utf8 (stringUnicode, length);
	if (!text)
		return NULL;

//g_warning ("layout >%s< (%d) [x %g, y %g, w %g, h %g] [font %s, %g points]", text, length, rc->X, rc->Y, rc->Width, rc->Height, font->face, font->emSize);

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

	/* unless specified we don't consider the trailing spaces, unless there is just one space (#80680) */
	if ((fmt->formatFlags & StringFormatFlagsMeasureTrailingSpaces) == 0) {
		while ((length > 0) && (isspace (*(text + length - 1))))
			length--;
		if (length == 0)
			length = 1;
	}

	layout = pango_cairo_create_layout (ct);

	/* context is owned by Pango (i.e. not referenced counted) do not free */
	context = pango_layout_get_context (layout);

	pango_layout_set_font_description (layout, gdip_get_pango_font_description ((GpFont*) font));

	if ((rc->Width <= 0.0) || (fmt->formatFlags & StringFormatFlagsNoWrap)) {
		pango_layout_set_width (layout, -1);
	} else {
		/* minus one to deal with our AA offset */
		int width = rc->Width - 1;
		/* TODO incomplete (missing height adjustment) */
		if ((fmt->formatFlags & StringFormatFlagsNoFitBlackBox) == 0)
			width -= 2;
		pango_layout_set_width (layout, width * PANGO_SCALE);
	}
	
	if (fmt->formatFlags & StringFormatFlagsDirectionRightToLeft) {
		/* with GDI+ the API not the renderer makes the direction decision */
		pango_layout_set_auto_dir (layout, FALSE);
		pango_context_set_base_dir (context, PANGO_DIRECTION_RTL);
		pango_layout_context_changed (layout);

		/* horizontal alignment */
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
	} else {
		/* horizontal alignment */
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

#ifdef PANGO_VERSION_CHECK
#if PANGO_VERSION_CHECK(1,16,0)
	if (fmt->formatFlags & StringFormatFlagsDirectionVertical) {
		/* only since Pango 1.16 */
		pango_context_set_base_gravity (context, PANGO_GRAVITY_EAST);
		pango_context_set_gravity_hint (context, PANGO_GRAVITY_HINT_STRONG);
		pango_layout_context_changed (layout);
	}
#endif
#endif
	
	/* TODO - StringFormatFlagsDisplayFormatControl
		scan and replace them ??? */

	/* TODO - StringFormatFlagsLineLimit */

	if ((rc->Width != 0) && (rc->Height != 0) && ((fmt->formatFlags & StringFormatFlagsNoClip) == 0)) {
//g_warning ("\tclip [%g %g %g %g]", rc->X, rc->Y, rc->Width, rc->Height);
		/* We do not call cairo_reset_clip because we want to take previous clipping into account */
		cairo_rectangle (ct, rc->X, rc->Y, rc->Width + 0.5, rc->Height + 0.5);
		cairo_clip (ct);
	}

	switch (fmt->trimming) {
	case StringTrimmingNone:
		pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_NONE);
		break;
	case StringTrimmingCharacter:
		pango_layout_set_wrap (layout, PANGO_WRAP_CHAR);
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_NONE);
		break;
	case StringTrimmingWord:
		pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_NONE);
		break;
	case StringTrimmingEllipsisCharacter:
		pango_layout_set_wrap (layout, PANGO_WRAP_CHAR);
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
		break;
	case StringTrimmingEllipsisWord:
		pango_layout_set_wrap (layout, PANGO_WRAP_WORD);
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
		break;
	case StringTrimmingEllipsisPath:
		pango_layout_set_wrap (layout, PANGO_WRAP_WORD_CHAR);
		pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_MIDDLE);
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

	switch (fmt->hotkeyPrefix) {
	case HotkeyPrefixHide:
		/* we need to remove any accelerator from the string */
		gdip_process_accelerators (text, length, NULL);
		break;
	case HotkeyPrefixShow:
		/* optimization: is seems that we never see the hotkey when using an underline font */
		if (font->style & FontStyleUnderline) {
			/* so don't bother drawing it (and simply add the '&' character) */
			gdip_process_accelerators (text, length, NULL);
		} else {
			/* find accelerator and add attribute to the next character (unless it's the prefix too) */
			if (!list)
				list = gdip_get_layout_attributes (layout);
			gdip_process_accelerators (text, length, list);
		}
		break;
	default:
		break;
	}

	if (list) {
		pango_layout_set_attributes (layout, list);
		pango_attr_list_unref (list);
	}

	pango_layout_set_text (layout, text, length);
	GdipFree (text);

	pango_layout_get_pixel_extents (layout, NULL, &logical);
//g_warning ("\tlogical\t[x %d, y %d, w %d, h %d]", logical.x, logical.y, logical.width, logical.height);

	box->X = rc->X;
	box->Y = rc->Y;
	box->Height = logical.height;
	/* add an extra pixel for our AA hack + 2 more if we don't draw on the box itself */
	box->Width = logical.width + (fmt->formatFlags & StringFormatFlagsNoFitBlackBox) ? 1 : 3;
//g_warning ("\tbox\t[x %g, y %g, w %g, h %g]", box->X, box->Y, box->Width, box->Height);

	/* vertical alignment*/
	switch (fmt->lineAlignment) {
	case StringAlignmentNear:
		break;
	case StringAlignmentCenter:
		box->Y += (rc->Height - logical.height) / 2;
		break;
	case StringAlignmentFar:
		box->Y += (rc->Height - logical.height);
		break;
	}
//g_warning ("va-box\t[x %g, y %g, w %g, h %g]", box->X, box->Y, box->Width, box->Height);

	pango_cairo_update_layout (ct, layout);

	return layout;
}

GpStatus
pango_DrawString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, GDIPCONST RectF *rc, 
	GDIPCONST GpStringFormat *format, GpBrush *brush)
{
	PangoLayout *layout;
	RectF box;

	cairo_save (graphics->ct);

	layout = gdip_pango_setup_layout (graphics->ct, stringUnicode, length, font, rc, &box, format);
	if (!layout) {
		cairo_restore (graphics->ct);
		return OutOfMemory;
	}

	/* Setup cairo */
	if (brush) {
		gdip_brush_setup (graphics, brush);
	} else {
		cairo_set_source_rgb (graphics->ct, 0., 0., 0.);
	}

	gdip_cairo_move_to (graphics, box.X, box.Y, FALSE, TRUE);
	pango_cairo_show_layout (graphics->ct, layout);

	g_object_unref (layout);
	cairo_restore (graphics->ct);
	return Ok;
}

GpStatus
pango_MeasureString (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, GDIPCONST RectF *rc,
	GDIPCONST GpStringFormat *format, RectF *boundingBox, int *codepointsFitted, int *linesFilled)
{
	PangoLayout *layout;

	cairo_save (graphics->ct);

	layout = gdip_pango_setup_layout (graphics->ct, stringUnicode, length, font, rc, boundingBox, format);
	if (!layout) {
		cairo_restore (graphics->ct);
		return OutOfMemory;
	}
		
	if (codepointsFitted) {
		// TODO - dummy (total) value returned
		*codepointsFitted = length;
	}

	if (linesFilled) {
		*linesFilled = pango_layout_get_line_count (layout);
//g_warning ("linesFilled %d", *linesFilled);
	}
//else g_warning ("linesFilled %d", pango_layout_get_line_count (layout));

	g_object_unref (layout);
	cairo_restore (graphics->ct);
	return Ok;
}

GpStatus
pango_MeasureCharacterRanges (GpGraphics *graphics, GDIPCONST WCHAR *stringUnicode, int length, GDIPCONST GpFont *font, 
	GDIPCONST GpRectF *layoutRect, GDIPCONST GpStringFormat *format, int regionCount, GpRegion **regions)
{
	PangoLayout *layout;
	GpStatus status = Ok;
	int i, j;
	GpRectF boundingBox;

	cairo_save (graphics->ct);

	layout = gdip_pango_setup_layout (graphics->ct, stringUnicode, length, font, layoutRect, &boundingBox, format);
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

		/* calculate the regions */
		for (j = start; j < end; j++) {
			PangoRectangle box;
			GpRectF charRect;

			pango_layout_index_to_pos (layout, j, &box);
			charRect.X = (float)box.x / PANGO_SCALE;
			charRect.Y = (float)box.y / PANGO_SCALE;
			charRect.Width = (float)box.width / PANGO_SCALE;
			charRect.Height = (float)box.height / PANGO_SCALE;
//g_warning ("[%d] [%d : %d-%d] %c [x %g y %g w %g h %g]", i, j, start, end, (char)stringUnicode[j], charRect.X, charRect.Y, charRect.Width, charRect.Height);
			status = GdipCombineRegionRect (regions [i], &charRect, CombineModeUnion);
			if (status != Ok)
				break;
		}
		if (status != Ok)
			break;
	}

cleanup:
	cairo_restore (graphics->ct);
	return status;
}

#endif
