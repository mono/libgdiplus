#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#include <windows.h>
#include <GdiPlus.h>
#else
#include <GdiPlusFlat.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
using namespace Gdiplus;
using namespace DllExports;
#endif

int
main(int argc, char**argv)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	
	GpFontCollection *collection;
	int status = GdipNewPrivateFontCollection(&collection);
	assert(status == 0);
	
	status = GdipNewPrivateFontCollection(NULL);
	assert(status == 2);

	int memory = 1;
	status = GdipPrivateAddMemoryFont(NULL, &memory, 1);
	assert(status == 2);
	
	status = GdipDeletePrivateFontCollection(NULL);
	assert(status == 2);
	
	status = GdipPrivateAddMemoryFont(collection, NULL, 1);
	assert(status == 2);

	status = GdipPrivateAddMemoryFont(collection, &memory, -1);
	assert(status == 2);

	status = GdipPrivateAddMemoryFont(collection, &memory, 0);
	assert(status == 2);

	status = GdipDeletePrivateFontCollection(&collection);
	assert(status == 0);

	GdiplusShutdown(gdiplusToken);
	return 0;
}
