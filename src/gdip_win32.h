/*
 * gdip_win32.h
 * 
 * Authors:
 *   Alexandre Pigolkine(pigolkine@gmx.de)
 */

#ifndef _GDIP_WIN32_H
#define _GDIP_WIN32_H

#include <cairo.h>
#ifndef CAIRO_HAS_XLIB_SURFACE
#include <cairo-xlib.h>
#endif
#include <mono/io-layer/uglify.h>

/* sizeof (GDIOBJHDR) = 12 (2 + 2 + 4 + 4) */
/* offsetof (DC, physDev) = 20 (12 + 4 + 4) */
typedef struct tagGDIOBJHDR {
	short 	next;
	short 	wMagic;
	long 	dwCount;
	void*	funcs;
	void* 	hdcs;
} GDIOBJHDR;

/* x11drv GDI escapes */
#define X11DRV_ESCAPE 6789
enum x11drv_escape_codes
{
    X11DRV_GET_DISPLAY,			/* get X11 display for a DC */
    X11DRV_GET_DRAWABLE,		/* get current drawable for a DC */
    X11DRV_GET_FONT,			/* get current X font for a DC */
};

typedef struct tagDC {
    GDIOBJHDR    	header;
    void*        	hSelf;          /* Handle to this DC */
    void 		*funcs; 	/* DC function table */
    void 		*physDev;       /* Physical device (driver-specific) */
} DC;

typedef struct {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD, *LPRGBQUAD;

typedef struct
{
    DWORD 	biSize;
    LONG  	biWidth;
    LONG  	biHeight;
    WORD 	biPlanes;
    WORD 	biBitCount;
    DWORD 	biCompression;
    DWORD 	biSizeImage;
    LONG  	biXPelsPerMeter;
    LONG  	biYPelsPerMeter;
    DWORD 	biClrUsed;
    DWORD 	biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;

  /* biCompression */
#define BI_RGB           0
#define BI_RLE8          1
#define BI_RLE4          2
#define BI_BITFIELDS     3

#define BITMAPINFOHEADER_SIZE   40
#define BITMAPCOREHEADER_SIZE   12

typedef struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD	bmiColors[1];
} BITMAPINFO, *PBITMAPINFO, *LPBITMAPINFO;

#ifdef __GNUC__
#    pragma pack(2)
#endif
typedef struct
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER, *LPBITMAPFILEHEADER;
#ifdef __GNUC__
#    pragma pack()
#endif

#define BFT_BITMAP 0x4d42

#ifdef __GNUC__
#  define __stdcall __attribute__((__stdcall__))
#else
#  define __stdcall 
#endif

extern void* (__stdcall *CreateCompatibleDC_pfn) (void * hdc);
extern void* (__stdcall *CreateCompatibleBitmap_pfn) (void * hdc, int width, int height);
extern void* (__stdcall *GetDC_pfn) (void * hwnd);

extern void* (__stdcall *SelectObject_pfn) (void * hdc, void *object);

extern void (__stdcall *DeleteDC_pfn) (void * hdc);
extern int (__stdcall *DeleteObject_pfn) (void * obj);
extern void (__stdcall *ReleaseDC_pfn) (void *hwnd, void * hdc);

extern int (__stdcall *GetDIBits_pfn) (void *hdc, void *hbitmap, unsigned startScan, unsigned scanLines, void *bitmapBits, PBITMAPINFO pbmi, unsigned int colorUse);
extern int (__stdcall *SetDIBits_pfn) (void *hdc, void *hbitmap, unsigned startScan, unsigned scanLines, void *bitmapBits, PBITMAPINFO pbmi, unsigned int colorUse);

extern int (*X11DRV_ExtEscape_pfn)(void *physDev, int escape, int in_count, void *in_data, int out_count, void *out_data);

extern void* (__stdcall *CreateFont_pfn)(int Height, int Width, int Orientation, int Escapement,
				int Weight, unsigned long Italic, unsigned long Underline,
				unsigned long Strikeout, unsigned long Charset,  
				unsigned long OutputPrecision, unsigned long ClipPrecision, 
				unsigned long Quality, unsigned long PitchAndFamily, 
				unsigned char *Face);



DC *_get_DC_by_HDC (int hDC);
void _release_hdc (int hdc);
void * CreateWineFont(FcChar8 *str, GpFontStyle style, float emSize, Unit unit);
void DeleteWineFont(void *hFont);

void gdip_bitmap_fill_info_header (GpBitmap *bitmap, PBITMAPINFOHEADER bmi);

#endif /* _GDIP_WIN32_H */
