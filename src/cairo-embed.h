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

#define cairo_append_path				mono_cairo_append_path
#define cairo_arc					mono_cairo_arc
#define cairo_arc_negative				mono_cairo_arc_negative
#define cairo_clip					mono_cairo_clip
#define cairo_clip_extents				mono_cairo_clip_extents
#define cairo_clip_preserve				mono_cairo_clip_preserve
#define cairo_close_path				mono_cairo_close_path
#define cairo_copy_clip_rectangle_list			mono_cairo_copy_clip_rectangle_list
#define cairo_copy_page					mono_cairo_copy_page
#define cairo_copy_path					mono_cairo_copy_path
#define cairo_copy_path_flat				mono_cairo_copy_path_flat
#define cairo_create					mono_cairo_create
#define cairo_curve_to					mono_cairo_curve_to
#define cairo_debug_reset_static_data			mono_cairo_debug_reset_static_data
#define cairo_destroy					mono_cairo_destroy
#define cairo_device_to_user				mono_cairo_device_to_user
#define cairo_device_to_user_distance			mono_cairo_device_to_user_distance
#define cairo_fill					mono_cairo_fill
#define cairo_fill_extents				mono_cairo_fill_extents
#define cairo_fill_preserve				mono_cairo_fill_preserve
#define cairo_font_extents				mono_cairo_font_extents
#define cairo_font_face_destroy				mono_cairo_font_face_destroy
#define cairo_font_face_get_reference_count		mono_cairo_font_face_get_reference_count
#define cairo_font_face_get_type			mono_cairo_font_face_get_type
#define cairo_font_face_get_user_data			mono_cairo_font_face_get_user_data
#define cairo_font_face_reference			mono_cairo_font_face_reference
#define cairo_font_face_set_user_data			mono_cairo_font_face_set_user_data
#define cairo_font_face_status				mono_cairo_font_face_status
#define cairo_font_options_copy				mono_cairo_font_options_copy
#define cairo_font_options_create			mono_cairo_font_options_create
#define cairo_font_options_destroy			mono_cairo_font_options_destroy
#define cairo_font_options_equal			mono_cairo_font_options_equal
#define cairo_font_options_get_antialias		mono_cairo_font_options_get_antialias
#define cairo_font_options_get_hint_metrics		mono_cairo_font_options_get_hint_metrics
#define cairo_font_options_get_hint_style		mono_cairo_font_options_get_hint_style
#define cairo_font_options_get_subpixel_order		mono_cairo_font_options_get_subpixel_order
#define cairo_font_options_hash				mono_cairo_font_options_hash
#define cairo_font_options_merge			mono_cairo_font_options_merge
#define cairo_font_options_set_antialias		mono_cairo_font_options_set_antialias
#define cairo_font_options_set_hint_metrics		mono_cairo_font_options_set_hint_metrics
#define cairo_font_options_set_hint_style		mono_cairo_font_options_set_hint_style
#define cairo_font_options_set_subpixel_order		mono_cairo_font_options_set_subpixel_order
#define cairo_font_options_status			mono_cairo_font_options_status
#define cairo_ft_font_face_create_for_ft_face		mono_cairo_ft_font_face_create_for_ft_face
#define cairo_ft_font_face_create_for_pattern		mono_cairo_ft_font_face_create_for_pattern
#define cairo_ft_font_options_substitute		mono_cairo_ft_font_options_substitute
#define cairo_ft_scaled_font_lock_face			mono_cairo_ft_scaled_font_lock_face
#define cairo_ft_scaled_font_unlock_face		mono_cairo_ft_scaled_font_unlock_face
#define cairo_get_antialias				mono_cairo_get_antialias
#define cairo_get_current_point				mono_cairo_get_current_point
#define cairo_get_dash					mono_cairo_get_dash
#define cairo_get_dash_count				mono_cairo_get_dash_count
#define cairo_get_fill_rule				mono_cairo_get_fill_rule
#define cairo_get_font_face				mono_cairo_get_font_face
#define cairo_get_font_matrix				mono_cairo_get_font_matrix
#define cairo_get_font_options				mono_cairo_get_font_options
#define cairo_get_group_target				mono_cairo_get_group_target
#define cairo_get_line_cap				mono_cairo_get_line_cap
#define cairo_get_line_join				mono_cairo_get_line_join
#define cairo_get_line_width				mono_cairo_get_line_width
#define cairo_get_matrix				mono_cairo_get_matrix
#define cairo_get_miter_limit				mono_cairo_get_miter_limit
#define cairo_get_operator				mono_cairo_get_operator
#define cairo_get_reference_count			mono_cairo_get_reference_count
#define cairo_get_scaled_font				mono_cairo_get_scaled_font
#define cairo_get_source				mono_cairo_get_source
#define cairo_get_target				mono_cairo_get_target
#define cairo_get_tolerance				mono_cairo_get_tolerance
#define cairo_get_user_data				mono_cairo_get_user_data
#define cairo_glyph_extents				mono_cairo_glyph_extents
#define cairo_glyph_path				mono_cairo_glyph_path
#define cairo_identity_matrix				mono_cairo_identity_matrix
#define cairo_image_surface_create			mono_cairo_image_surface_create
#define cairo_image_surface_create_for_data		mono_cairo_image_surface_create_for_data
#define cairo_image_surface_create_from_png		mono_cairo_image_surface_create_from_png
#define cairo_image_surface_create_from_png_stream	mono_cairo_image_surface_create_from_png_stream
#define cairo_image_surface_get_data			mono_cairo_image_surface_get_data
#define cairo_image_surface_get_format			mono_cairo_image_surface_get_format
#define cairo_image_surface_get_height			mono_cairo_image_surface_get_height
#define cairo_image_surface_get_stride			mono_cairo_image_surface_get_stride
#define cairo_image_surface_get_width			mono_cairo_image_surface_get_width
#define cairo_in_fill					mono_cairo_in_fill
#define cairo_in_stroke					mono_cairo_in_stroke
#define cairo_line_to					mono_cairo_line_to
#define cairo_mask					mono_cairo_mask
#define cairo_mask_surface				mono_cairo_mask_surface
#define cairo_matrix_init				mono_cairo_matrix_init
#define cairo_matrix_init_identity			mono_cairo_matrix_init_identity
#define cairo_matrix_init_rotate			mono_cairo_matrix_init_rotate
#define cairo_matrix_init_scale				mono_cairo_matrix_init_scale
#define cairo_matrix_init_translate			mono_cairo_matrix_init_translate
#define cairo_matrix_invert				mono_cairo_matrix_invert
#define cairo_matrix_multiply				mono_cairo_matrix_multiply
#define cairo_matrix_rotate				mono_cairo_matrix_rotate
#define cairo_matrix_scale				mono_cairo_matrix_scale
#define cairo_matrix_transform_distance			mono_cairo_matrix_transform_distance
#define cairo_matrix_transform_point			mono_cairo_matrix_transform_point
#define cairo_matrix_translate				mono_cairo_matrix_translate
#define cairo_move_to					mono_cairo_move_to
#define cairo_new_path					mono_cairo_new_path
#define cairo_new_sub_path				mono_cairo_new_sub_path
#define cairo_paint					mono_cairo_paint
#define cairo_paint_with_alpha				mono_cairo_paint_with_alpha
#define cairo_path_destroy				mono_cairo_path_destroy
#define cairo_pattern_add_color_stop_rgb		mono_cairo_pattern_add_color_stop_rgb
#define cairo_pattern_add_color_stop_rgba		mono_cairo_pattern_add_color_stop_rgba
#define cairo_pattern_create_for_surface		mono_cairo_pattern_create_for_surface
#define cairo_pattern_create_linear			mono_cairo_pattern_create_linear
#define cairo_pattern_create_radial			mono_cairo_pattern_create_radial
#define cairo_pattern_create_rgb			mono_cairo_pattern_create_rgb
#define cairo_pattern_create_rgba			mono_cairo_pattern_create_rgba
#define cairo_pattern_destroy				mono_cairo_pattern_destroy
#define cairo_pattern_get_color_stop_count		mono_cairo_pattern_get_color_stop_count
#define cairo_pattern_get_color_stop_rgba		mono_cairo_pattern_get_color_stop_rgba
#define cairo_pattern_get_extend			mono_cairo_pattern_get_extend
#define cairo_pattern_get_filter			mono_cairo_pattern_get_filter
#define cairo_pattern_get_linear_points			mono_cairo_pattern_get_linear_points
#define cairo_pattern_get_matrix			mono_cairo_pattern_get_matrix
#define cairo_pattern_get_radial_circles		mono_cairo_pattern_get_radial_circles
#define cairo_pattern_get_reference_count		mono_cairo_pattern_get_reference_count
#define cairo_pattern_get_rgba				mono_cairo_pattern_get_rgba
#define cairo_pattern_get_surface			mono_cairo_pattern_get_surface
#define cairo_pattern_get_type				mono_cairo_pattern_get_type
#define cairo_pattern_get_user_data			mono_cairo_pattern_get_user_data
#define cairo_pattern_reference				mono_cairo_pattern_reference
#define cairo_pattern_set_extend			mono_cairo_pattern_set_extend
#define cairo_pattern_set_filter			mono_cairo_pattern_set_filter
#define cairo_pattern_set_matrix			mono_cairo_pattern_set_matrix
#define cairo_pattern_set_user_data			mono_cairo_pattern_set_user_data
#define cairo_pattern_status				mono_cairo_pattern_status
#define cairo_pop_group					mono_cairo_pop_group
#define cairo_pop_group_to_source			mono_cairo_pop_group_to_source
#define cairo_ps_surface_create				mono_cairo_ps_surface_create
#define cairo_ps_surface_create_for_stream		mono_cairo_ps_surface_create_for_stream
#define cairo_ps_surface_dsc_begin_page_setup		mono_cairo_ps_surface_dsc_begin_page_setup
#define cairo_ps_surface_dsc_begin_setup		mono_cairo_ps_surface_dsc_begin_setup
#define cairo_ps_surface_dsc_comment			mono_cairo_ps_surface_dsc_comment
#define cairo_ps_surface_set_size			mono_cairo_ps_surface_set_size
#define cairo_push_group				mono_cairo_push_group
#define cairo_push_group_with_content			mono_cairo_push_group_with_content
#define cairo_rectangle					mono_cairo_rectangle
#define cairo_rectangle_list_destroy			mono_cairo_rectangle_list_destroy
#define cairo_reference					mono_cairo_reference
#define cairo_rel_curve_to				mono_cairo_rel_curve_to
#define cairo_rel_line_to				mono_cairo_rel_line_to
#define cairo_rel_move_to				mono_cairo_rel_move_to
#define cairo_reset_clip				mono_cairo_reset_clip
#define cairo_restore					mono_cairo_restore
#define cairo_rotate					mono_cairo_rotate
#define cairo_save					mono_cairo_save
#define cairo_scale					mono_cairo_scale
#define cairo_scaled_font_create			mono_cairo_scaled_font_create
#define cairo_scaled_font_destroy			mono_cairo_scaled_font_destroy
#define cairo_scaled_font_extents			mono_cairo_scaled_font_extents
#define cairo_scaled_font_get_ctm			mono_cairo_scaled_font_get_ctm
#define cairo_scaled_font_get_font_face			mono_cairo_scaled_font_get_font_face
#define cairo_scaled_font_get_font_matrix		mono_cairo_scaled_font_get_font_matrix
#define cairo_scaled_font_get_font_options		mono_cairo_scaled_font_get_font_options
#define cairo_scaled_font_get_reference_count		mono_cairo_scaled_font_get_reference_count
#define cairo_scaled_font_get_type			mono_cairo_scaled_font_get_type
#define cairo_scaled_font_get_user_data			mono_cairo_scaled_font_get_user_data
#define cairo_scaled_font_glyph_extents			mono_cairo_scaled_font_glyph_extents
#define cairo_scaled_font_reference			mono_cairo_scaled_font_reference
#define cairo_scaled_font_set_user_data			mono_cairo_scaled_font_set_user_data
#define cairo_scaled_font_status			mono_cairo_scaled_font_status
#define cairo_scaled_font_text_extents			mono_cairo_scaled_font_text_extents
#define cairo_select_font_face				mono_cairo_select_font_face
#define cairo_set_antialias				mono_cairo_set_antialias
#define cairo_set_dash					mono_cairo_set_dash
#define cairo_set_fill_rule				mono_cairo_set_fill_rule
#define cairo_set_font_face				mono_cairo_set_font_face
#define cairo_set_font_matrix				mono_cairo_set_font_matrix
#define cairo_set_font_options				mono_cairo_set_font_options
#define cairo_set_font_size				mono_cairo_set_font_size
#define cairo_set_line_cap				mono_cairo_set_line_cap
#define cairo_set_line_join				mono_cairo_set_line_join
#define cairo_set_line_width				mono_cairo_set_line_width
#define cairo_set_matrix				mono_cairo_set_matrix
#define cairo_set_miter_limit				mono_cairo_set_miter_limit
#define cairo_set_operator				mono_cairo_set_operator
#define cairo_set_scaled_font				mono_cairo_set_scaled_font
#define cairo_set_source				mono_cairo_set_source
#define cairo_set_source_rgb				mono_cairo_set_source_rgb
#define cairo_set_source_rgba				mono_cairo_set_source_rgba
#define cairo_set_source_surface			mono_cairo_set_source_surface
#define cairo_set_tolerance				mono_cairo_set_tolerance
#define cairo_set_user_data				mono_cairo_set_user_data
#define cairo_show_glyphs				mono_cairo_show_glyphs
#define cairo_show_page					mono_cairo_show_page
#define cairo_show_text					mono_cairo_show_text
#define cairo_status					mono_cairo_status
#define cairo_status_to_string				mono_cairo_status_to_string
#define cairo_stroke					mono_cairo_stroke
#define cairo_stroke_extents				mono_cairo_stroke_extents
#define cairo_stroke_preserve				mono_cairo_stroke_preserve
#define cairo_surface_create_similar			mono_cairo_surface_create_similar
#define cairo_surface_destroy				mono_cairo_surface_destroy
#define cairo_surface_finish				mono_cairo_surface_finish
#define cairo_surface_flush				mono_cairo_surface_flush
#define cairo_surface_get_content			mono_cairo_surface_get_content
#define cairo_surface_get_device_offset			mono_cairo_surface_get_device_offset
#define cairo_surface_get_font_options			mono_cairo_surface_get_font_options
#define cairo_surface_get_reference_count		mono_cairo_surface_get_reference_count
#define cairo_surface_get_type				mono_cairo_surface_get_type
#define cairo_surface_get_user_data			mono_cairo_surface_get_user_data
#define cairo_surface_mark_dirty			mono_cairo_surface_mark_dirty
#define cairo_surface_mark_dirty_rectangle		mono_cairo_surface_mark_dirty_rectangle
#define cairo_surface_reference				mono_cairo_surface_reference
#define cairo_surface_set_device_offset			mono_cairo_surface_set_device_offset
#define cairo_surface_set_fallback_resolution		mono_cairo_surface_set_fallback_resolution
#define cairo_surface_set_user_data			mono_cairo_surface_set_user_data
#define cairo_surface_status				mono_cairo_surface_status
#define cairo_surface_write_to_png			mono_cairo_surface_write_to_png
#define cairo_surface_write_to_png_stream		mono_cairo_surface_write_to_png_stream
#define cairo_text_extents				mono_cairo_text_extents
#define cairo_text_path					mono_cairo_text_path
#define cairo_transform					mono_cairo_transform
#define cairo_translate					mono_cairo_translate
#define cairo_user_to_device				mono_cairo_user_to_device
#define cairo_user_to_device_distance			mono_cairo_user_to_device_distance
#define cairo_version					mono_cairo_version
#define cairo_version_string				mono_cairo_version_string
#define cairo_xlib_surface_create			mono_cairo_xlib_surface_create
#define cairo_xlib_surface_create_for_bitmap		mono_cairo_xlib_surface_create_for_bitmap
#define cairo_xlib_surface_create_with_xrender_format	mono_cairo_xlib_surface_create_with_xrender_format
#define cairo_xlib_surface_get_depth			mono_cairo_xlib_surface_get_depth
#define cairo_xlib_surface_get_display			mono_cairo_xlib_surface_get_display
#define cairo_xlib_surface_get_drawable			mono_cairo_xlib_surface_get_drawable
#define cairo_xlib_surface_get_height			mono_cairo_xlib_surface_get_height
#define cairo_xlib_surface_get_screen			mono_cairo_xlib_surface_get_screen
#define cairo_xlib_surface_get_visual			mono_cairo_xlib_surface_get_visual
#define cairo_xlib_surface_get_width			mono_cairo_xlib_surface_get_width
#define cairo_xlib_surface_set_drawable			mono_cairo_xlib_surface_set_drawable
#define cairo_xlib_surface_set_size			mono_cairo_xlib_surface_set_size

#endif
