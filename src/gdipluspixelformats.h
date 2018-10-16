/*
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
 */

#ifndef __GDIPLUSPIXELFORMATS_H__
#define __GDIPLUSPIXELFORMATS_H__

#include "win32structs.h"

typedef DWORD ARGB;
typedef DWORDLONG ARGB64;

#define ALPHA_SHIFT 24
#define RED_SHIFT   16
#define GREEN_SHIFT 8
#define BLUE_SHIFT  0
#define ALPHA_MASK  ((ARGB) 0xff << ALPHA_SHIFT)

/* The pixel format spec is:
 * [0-7 format index] [8-15 pixel size, bits] [16-23 flags] [24-31 reserved]
 * so,
 * (ID | (bpp << 8) | flags)
 */
typedef INT PixelFormat;

#define    PixelFormatIndexed      0x00010000 // Indexes into a palette
#define    PixelFormatGDI          0x00020000 // Is a GDI-supported format
#define    PixelFormatAlpha        0x00040000 // Has an alpha component
#define    PixelFormatPAlpha       0x00080000 // Pre-multiplied alpha
#define    PixelFormatExtended     0x00100000 // Extended color 16 bits/channel
#define    PixelFormatCanonical    0x00200000 

#define    PixelFormatUndefined       0
#define    PixelFormatDontCare        0

#define    PixelFormat1bppIndexed     (1 | ( 1 << 8) | PixelFormatIndexed | PixelFormatGDI)
#define    PixelFormat4bppIndexed     (2 | ( 4 << 8) | PixelFormatIndexed | PixelFormatGDI)
#define    PixelFormat8bppIndexed     (3 | ( 8 << 8) | PixelFormatIndexed | PixelFormatGDI)
#define    PixelFormat16bppGrayScale  (4 | (16 << 8) | PixelFormatExtended)
#define    PixelFormat16bppRGB555     (5 | (16 << 8) | PixelFormatGDI)
#define    PixelFormat16bppRGB565     (6 | (16 << 8) | PixelFormatGDI)
#define    PixelFormat16bppARGB1555   (7 | (16 << 8) | PixelFormatAlpha | PixelFormatGDI)
#define    PixelFormat24bppRGB        (8 | (24 << 8) | PixelFormatGDI)
#define    PixelFormat32bppRGB        (9 | (32 << 8) | PixelFormatGDI)
#define    PixelFormat32bppARGB       (10 | (32 << 8) | PixelFormatAlpha | PixelFormatGDI | PixelFormatCanonical)
#define    PixelFormat32bppPARGB      (11 | (32 << 8) | PixelFormatAlpha | PixelFormatPAlpha | PixelFormatGDI)
#define    PixelFormat48bppRGB        (12 | (48 << 8) | PixelFormatExtended)
#define    PixelFormat64bppARGB       (13 | (64 << 8) | PixelFormatAlpha  | PixelFormatCanonical | PixelFormatExtended)
#define    PixelFormat64bppPARGB      (14 | (64 << 8) | PixelFormatAlpha  | PixelFormatPAlpha | PixelFormatExtended)
#define    PixelFormat32bppCMYK       (15 | (32 << 8))
#define    PixelFormatMax             16

inline UINT
GetPixelFormatSize(PixelFormat pixfmt)
{
	return (pixfmt >> 8) & 0xff;
}

inline BOOL
IsIndexedPixelFormat(PixelFormat pixfmt)
{
	return (pixfmt & PixelFormatIndexed) != 0;
}

inline BOOL
IsAlphaPixelFormat(PixelFormat pixfmt)
{
   return (pixfmt & PixelFormatAlpha) != 0;
}

inline BOOL
IsExtendedPixelFormat(PixelFormat pixfmt)
{
   return (pixfmt & PixelFormatExtended) != 0;
}

inline BOOL
IsCanonicalPixelFormat(PixelFormat pixfmt)
{
   return (pixfmt & PixelFormatCanonical) != 0;
}

#if (GDIPVER >= 0x0110)
typedef enum {
	PaletteTypeCustom = 0,
	PaletteTypeOptimal = 1,
	PaletteTypeFixedBW = 2,
	PaletteTypeFixedHalftone8 = 3,
	PaletteTypeFixedHalftone27 = 4,
	PaletteTypeFixedHalftone64 = 5,
	PaletteTypeFixedHalftone125 = 6,
	PaletteTypeFixedHalftone216 = 7,
	PaletteTypeFixedHalftone252 = 8,
	PaletteTypeFixedHalftone256 = 9,
} PaletteType;

typedef enum {
	DitherTypeNone = 0,
	DitherTypeSolid = 1,
	DitherTypeOrdered4x4 = 2,
	DitherTypeOrdered8x8 = 3,
	DitherTypeOrdered16x16 = 4,
	DitherTypeSpiral4x4 = 5,
	DitherTypeSpiral8x8 = 6,
	DitherTypeDualSpiral4x4 = 7,
	DitherTypeDualSpiral8x8 = 8,
	DitherTypeErrorDiffusion = 9,
	DitherTypeMax = 10
}
#endif //(GDIPVER >= 0x0110)

typedef enum {
	PaletteFlagsHasAlpha    = 0x0001,
	PaletteFlagsGrayScale   = 0x0002,
	PaletteFlagsHalftone    = 0x0004
} PaletteFlags;

typedef struct {
	UINT Flags;
	UINT Count;
	ARGB Entries[1];
} ColorPalette;

#endif
