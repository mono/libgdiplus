/*
* Copyright (c) 2017 Quamotion bvba
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
*	Frederik Carlier <frederik.carlier@quamotion.mobi>
*/

#include "win32_io.h"

#include <windows.h>
#include "gdipenums.h"

FILE *CreateTempFile (char *filename)
{
	TCHAR temppath[MAX_PATH];
	DWORD ret = 0;
	
	ret = GetTempPath (MAX_PATH, temppath);
	if (ret > MAX_PATH || ret == 0) {
		return NULL;
	}

	ret = GetTempFileName (temppath, "ff", 0, (LPSTR)filename);
	if (ret == 0) {
		return NULL;
	}

	return fopen (filename, "w");
}