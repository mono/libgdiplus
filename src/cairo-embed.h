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
 *          Sebastien Pouliot  <sebastien@ximian.com>
 */

#ifndef _CAIRO_EMBED_H_
#define _CAIRO_EMBED_H_

/* avoid symbol mixup between Gtk+2.8 using libcairo and libgdiplus embedded cairo library */

#define cairo_arc				mono_cairo_arc
#define cairo_clip				mono_cairo_clip
#define cairo_close_path			mono_cairo_close_path
#define cairo_copy_path				mono_cairo_copy_path
#define cairo_create				mono_cairo_create
#define cairo_curve_to				mono_cairo_curve_to
#define cairo_destroy				mono_cairo_destroy
#define cairo_device_to_user_distance		mono_cairo_device_to_user_distance
#define cairo_fill				mono_cairo_fill
#define cairo_font_extents			mono_cairo_font_extents
#define cairo_font_options_create		mono_cairo_font_options_create
#define cairo_font_options_destroy		mono_cairo_font_options_destroy
#define cairo_font_options_set_antialias	mono_cairo_font_options_set_antialias
#define cairo_font_options_set_hint_metrics	mono_cairo_font_options_set_hint_metrics
#define cairo_font_options_set_hint_style	mono_cairo_font_options_set_hint_style
#define cairo_font_options_set_subpixel_order	mono_cairo_font_options_set_subpixel_order
#define cairo_font_set_transform		mono_cairo_font_set_transform
#define cairo_ft_scaled_font_lock_face		mono_cairo_ft_scaled_font_lock_face
#define cairo_ft_scaled_font_unlock_face	mono_cairo_ft_scaled_font_unlock_face
#define cairo_get_antialias			mono_cairo_get_antialias
#define cairo_get_font_face			mono_cairo_get_font_face
#define cairo_get_font_matrix			mono_cairo_get_font_matrix
#define cairo_get_line_width			mono_cairo_get_line_width
#define cairo_get_matrix			mono_cairo_get_matrix
#define cairo_get_source			mono_cairo_get_source
#define cairo_get_target			mono_cairo_get_target
#define cairo_identity_matrix			mono_cairo_identity_matrix
#define cairo_image_surface_create		mono_cairo_image_surface_create
#define cairo_image_surface_create_for_data	mono_cairo_image_surface_create_for_data
#define cairo_line_to				mono_cairo_line_to
#define cairo_matrix_init			mono_cairo_matrix_init
#define cairo_matrix_init_identity		mono_cairo_matrix_init_identity 
#define cairo_matrix_invert			mono_cairo_matrix_invert
#define cairo_matrix_multiply			mono_cairo_matrix_multiply
#define cairo_matrix_rotate			mono_cairo_matrix_rotate
#define cairo_matrix_scale			mono_cairo_matrix_scale
#define cairo_matrix_transform_distance		mono_cairo_matrix_transform_distance
#define cairo_matrix_transform_point		mono_cairo_matrix_transform_point
#define cairo_matrix_translate			mono_cairo_matrix_translate
#define cairo_move_to				mono_cairo_move_to
#define cairo_new_path				mono_cairo_new_path
#define cairo_paint				mono_cairo_paint
#define cairo_path_destroy			mono_cairo_path_destroy
#define cairo_pattern_add_color_stop_rgba	mono_cairo_pattern_add_color_stop_rgba
#define cairo_pattern_create_for_surface	mono_cairo_pattern_create_for_surface
#define cairo_pattern_create_linear		mono_cairo_pattern_create_linear
#define cairo_pattern_create_radial		mono_cairo_pattern_create_radial
#define cairo_pattern_destroy			mono_cairo_pattern_destroy
#define cairo_pattern_reference			mono_cairo_pattern_reference
#define cairo_pattern_set_extend		mono_cairo_pattern_set_extend
#define cairo_pattern_set_filter		mono_cairo_pattern_set_filter
#define cairo_pattern_set_matrix 		mono_cairo_pattern_set_matrix
#define cairo_pattern_status			mono_cairo_pattern_status
#define cairo_ps_surface_create			mono_cairo_ps_surface_create
#define cairo_surface_reference			mono_cairo_surface_reference
#define cairo_rectangle				mono_cairo_rectangle
#define cairo_reset_clip			mono_cairo_reset_clip
#define cairo_restore				mono_cairo_restore
#define cairo_rotate				mono_cairo_rotate
#define cairo_save				mono_cairo_save
#define cairo_scale				mono_cairo_scale
#define cairo_scaled_font_create		mono_cairo_scaled_font_create
#define cairo_scaled_font_destroy		mono_cairo_scaled_font_destroy
#define cairo_select_font_face			mono_cairo_select_font_face
#define cairo_set_antialias			mono_cairo_set_antialias
#define cairo_set_dash				mono_cairo_set_dash
#define cairo_set_fill_rule			mono_cairo_set_fill_rule
#define cairo_set_font_face			mono_cairo_set_font_face  
#define cairo_set_font_size			mono_cairo_set_font_size
#define cairo_set_font_options			mono_cairo_set_font_options
#define cairo_set_font_matrix			mono_cairo_set_font_matrix
#define cairo_set_line_cap			mono_cairo_set_line_cap
#define cairo_set_line_join			mono_cairo_set_line_join
#define cairo_set_line_width			mono_cairo_set_line_width
#define cairo_set_matrix			mono_cairo_set_matrix
#define cairo_set_miter_limit			mono_cairo_set_miter_limit
#define cairo_set_operator			mono_cairo_set_operator
#define cairo_set_source			mono_cairo_set_source
#define cairo_set_source_rgb			mono_cairo_set_source_rgb
#define cairo_set_source_rgba			mono_cairo_set_source_rgba
#define cairo_set_source_surface		mono_cairo_set_source_surface
#define cairo_show_page				mono_cairo_show_page
#define cairo_show_text				mono_cairo_show_text
#define cairo_status				mono_cairo_status
#define cairo_stroke				mono_cairo_stroke
#define cairo_surface_create_similar		mono_cairo_surface_create_similar
#define cairo_surface_destroy			mono_cairo_surface_destroy
#define	cairo_surface_flush			mono_cairo_surface_flush
#define cairo_surface_reference			mono_cairo_surface_reference
#define cairo_surface_set_fallback_resolution	mono_cairo_surface_set_fallback_resolution 
#define cairo_surface_status			mono_cairo_surface_status
#define cairo_text_extents			mono_cairo_text_extents
#define cairo_text_path				mono_cairo_text_path
#define cairo_translate				mono_cairo_translate
#define cairo_xlib_surface_create		mono_cairo_xlib_surface_create

#endif
