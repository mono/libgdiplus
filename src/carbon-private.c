/*
 * carbon-private.c
 * 
 * Copyright (C) 2008 Novell, Inc (http://www.novell.com)
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
 *   Geoff Norton  <gnorton@novell.com>
 */

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#include <IOKit/graphics/IOGraphicsLib.h>

static inline int gdip_get_int_from_dict_for_key (CFDictionaryRef dict, CFStringRef key) {
	CFNumberRef value = CFDictionaryGetValue (dict, key);
	int ret = 0;

	if (value == NULL)
		return 0;

	CFNumberGetValue (value, kCFNumberIntType, &ret);

	return ret;
}

void gdip_get_display_dpi_carbon (float *h_dpi, float *v_dpi) {
	if (getenv ("MONO_MWF_MAC_DETECT_DPI") != NULL) {
		CFDictionaryRef mode_dict = CGDisplayCurrentMode (kCGDirectMainDisplay);
		io_connect_t display_port = CGDisplayIOServicePort (kCGDirectMainDisplay);
		CFDictionaryRef display_dict = IOCreateDisplayInfoDictionary (display_port, 0);
		const float mmpi = 25.4;
		float h_size = (float) gdip_get_int_from_dict_for_key (display_dict, CFSTR (kDisplayHorizontalImageSize)) / mmpi;
		float v_size = (float) gdip_get_int_from_dict_for_key (display_dict, CFSTR (kDisplayVerticalImageSize)) / mmpi;

		*h_dpi = (float) gdip_get_int_from_dict_for_key (mode_dict, kCGDisplayWidth) / h_size; 
		*v_dpi = (float) gdip_get_int_from_dict_for_key (mode_dict, kCGDisplayHeight) / v_size; 

		CFRelease (display_dict);
	} else {
		*h_dpi = *v_dpi = 96.0f;
	}
}
#endif
