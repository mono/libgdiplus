/*
 * Copyright © 2000 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */


#include "pixman-xserver-compat.h"

/*
 * General purpose compositing code optimized for minimal memory
 * references
 *
 * All work is done on canonical ARGB values, functions for fetching
 * and storing these exist for each format.
 */

/*
 * Combine src and mask using IN
 */

static uint32_t
fbCombineMaskU (FbCompositeOperand   *src,
		FbCompositeOperand   *msk)
{
    CARD32  x;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    if (!msk)
	return (*src->fetch) (src);
    
    a = (*msk->fetch) (msk) >> 24;
    if (!a)
	return 0;
    
    x = (*src->fetch) (src);
    if (a == 0xff)
	return x;
    
    m = FbInU(x,0,a,t);
    n = FbInU(x,8,a,t);
    o = FbInU(x,16,a,t);
    p = FbInU(x,24,a,t);
    return m|n|o|p;
}

static FbCompSrc
fbCombineMaskC (FbCompositeOperand   *src,
		FbCompositeOperand   *msk)
{
    FbCompSrc	s;
    CARD32	x;
    CARD32	a;
    CARD16	xa;
    CARD16	t;
    CARD32	m,n,o,p;

    if (!msk)
    {
	x = (*src->fetch) (src);
	s.value = x;
	x = x >> 24;
	x |= x << 8;
	x |= x << 16;
	s.alpha = x;
	return s;
    }
    
    a = (*msk->fetcha) (msk);
    if (!a)
    {
	s.value = 0;
	s.alpha = 0;
	return s;
    }
    
    x = (*src->fetch) (src);
    if (a == 0xffffffff)
    {
	s.value = x;
	x = x >> 24;
	x |= x << 8;
	x |= x << 16;
	s.alpha = x;
	return s;
    }
    
    m = FbInC(x,0,a,t);
    n = FbInC(x,8,a,t);
    o = FbInC(x,16,a,t);
    p = FbInC(x,24,a,t);
    s.value = m|n|o|p;
    xa = x >> 24;
    m = FbInU(a,0,xa,t);
    n = FbInU(a,8,xa,t);
    o = FbInU(a,16,xa,t);
    p = FbInU(a,24,xa,t);
    s.alpha = m|n|o|p;
    return s;
}

static uint32_t
fbCombineMaskValueC (FbCompositeOperand   *src,
		     FbCompositeOperand   *msk)
{
    CARD32	x;
    CARD32	a;
    CARD16	t;
    CARD32	m,n,o,p;

    if (!msk)
    {
	return (*src->fetch) (src);
    }
    
    a = (*msk->fetcha) (msk);
    if (!a)
	return a;
    
    x = (*src->fetch) (src);
    if (a == 0xffffffff)
	return x;
    
    m = FbInC(x,0,a,t);
    n = FbInC(x,8,a,t);
    o = FbInC(x,16,a,t);
    p = FbInC(x,24,a,t);
    return m|n|o|p;
}

/*
 * Combine src and mask using IN, generating only the alpha component
 */
static uint32_t
fbCombineMaskAlphaU (FbCompositeOperand   *src,
		     FbCompositeOperand   *msk)
{
    uint32_t  x;
    uint16_t  a;
    uint16_t  t;

    if (!msk)
	return (*src->fetch) (src);
    
    a = (*msk->fetch) (msk) >> 24;
    if (!a)
	return 0;
    
    x = (*src->fetch) (src);
    if (a == 0xff)
	return x;
    
    return FbInU(x,24,a,t);
}

static uint32_t
fbCombineMaskAlphaC (FbCompositeOperand   *src,
		     FbCompositeOperand   *msk)
{
    CARD32	x;
    CARD32	a;
    CARD16	t;
    CARD32	m,n,o,p;

    if (!msk)
	return (*src->fetch) (src);
    
    a = (*msk->fetcha) (msk);
    if (!a)
	return 0;
    
    x = (*src->fetcha) (src);
    if (a == 0xffffffff)
	return x;
    
    m = FbInC(x,0,a,t);
    n = FbInC(x,8,a,t);
    o = FbInC(x,16,a,t);
    p = FbInC(x,24,a,t);
    return m|n|o|p;
}

/*
 * All of the composing functions
 */
static void
fbCombineClear (FbCompositeOperand   *src,
		FbCompositeOperand   *msk,
		FbCompositeOperand   *dst)
{
    (*dst->store) (dst, 0);
}

static void
fbCombineSrcU (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    (*dst->store) (dst, fbCombineMaskU (src, msk));
}

static void
fbCombineSrcC (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    (*dst->store) (dst, fbCombineMaskValueC (src, msk));
}

static void
fbCombineDst (FbCompositeOperand    *src,
	      FbCompositeOperand    *msk,
	      FbCompositeOperand    *dst)
{
    /* noop */
}

static void
fbCombineOverU (FbCompositeOperand   *src,
		FbCompositeOperand   *msk,
		FbCompositeOperand   *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskU (src, msk);
    a = ~s >> 24;
    if (a != 0xff)
    {
	if (a)
	{
	    d = (*dst->fetch) (dst);
	    m = FbOverU(s,d,0,a,t);
	    n = FbOverU(s,d,8,a,t);
	    o = FbOverU(s,d,16,a,t);
	    p = FbOverU(s,d,24,a,t);
	    s = m|n|o|p;
	}
	(*dst->store) (dst, s);
    }
}

static void
fbCombineOverC (FbCompositeOperand   *src,
		FbCompositeOperand   *msk,
		FbCompositeOperand   *dst)
{
    FbCompSrc	cs;
    CARD32  s, d;
    CARD32  a;
    CARD16  t;
    CARD32  m,n,o,p;

    cs = fbCombineMaskC (src, msk);
    s = cs.value;
    a = ~cs.alpha;
    if (a != 0xffffffff)
    {
	if (a)
	{
	    d = (*dst->fetch) (dst);
	    m = FbOverC(s,d,0,a,t);
	    n = FbOverC(s,d,8,a,t);
	    o = FbOverC(s,d,16,a,t);
	    p = FbOverC(s,d,24,a,t);
	    s = m|n|o|p;
	}
	(*dst->store) (dst, s);
    }
}

static void
fbCombineOverReverseU (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    d = (*dst->fetch) (dst);
    a = ~d >> 24;
    if (a)
    {
	s = fbCombineMaskU (src, msk);
	if (a != 0xff)
	{
	    m = FbOverU(d,s,0,a,t);
	    n = FbOverU(d,s,8,a,t);
	    o = FbOverU(d,s,16,a,t);
	    p = FbOverU(d,s,24,a,t);
	    s = m|n|o|p;
	}
	(*dst->store) (dst, s);
    }
}

static void
fbCombineOverReverseC (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD32  a;
    CARD16  t;
    CARD32  m,n,o,p;

    d = (*dst->fetch) (dst);
    a = ~d >> 24;
    if (a)
    {
	s = fbCombineMaskValueC (src, msk);
	if (a != 0xff)
	{
	    m = FbOverU(d,s,0,a,t);
	    n = FbOverU(d,s,8,a,t);
	    o = FbOverU(d,s,16,a,t);
	    p = FbOverU(d,s,24,a,t);
	    s = m|n|o|p;
	}
	(*dst->store) (dst, s);
    }
}

static void
fbCombineInU (FbCompositeOperand	    *src,
	      FbCompositeOperand	    *msk,
	      FbCompositeOperand	    *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    d = (*dst->fetch) (dst);
    a = d >> 24;
    s = 0;
    if (a)
    {
	s = fbCombineMaskU (src, msk);
	if (a != 0xff)
	{
	    m = FbInU(s,0,a,t);
	    n = FbInU(s,8,a,t);
	    o = FbInU(s,16,a,t);
	    p = FbInU(s,24,a,t);
	    s = m|n|o|p;
	}
    }
    (*dst->store) (dst, s);
}

static void
fbCombineInC (FbCompositeOperand	    *src,
	      FbCompositeOperand	    *msk,
	      FbCompositeOperand	    *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    d = (*dst->fetch) (dst);
    a = d >> 24;
    s = 0;
    if (a)
    {
	s = fbCombineMaskValueC (src, msk);
	if (a != 0xff)
	{
	    m = FbInU(s,0,a,t);
	    n = FbInU(s,8,a,t);
	    o = FbInU(s,16,a,t);
	    p = FbInU(s,24,a,t);
	    s = m|n|o|p;
	}
    }
    (*dst->store) (dst, s);
}

static void
fbCombineInReverseU (FbCompositeOperand  *src,
		     FbCompositeOperand  *msk,
		     FbCompositeOperand  *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskAlphaU (src, msk);
    a = s >> 24;
    if (a != 0xff)
    {
	d = 0;
	if (a)
	{
	    d = (*dst->fetch) (dst);
	    m = FbInU(d,0,a,t);
	    n = FbInU(d,8,a,t);
	    o = FbInU(d,16,a,t);
	    p = FbInU(d,24,a,t);
	    d = m|n|o|p;
	}
	(*dst->store) (dst, d);
    }
}

static void
fbCombineInReverseC (FbCompositeOperand  *src,
		     FbCompositeOperand  *msk,
		     FbCompositeOperand  *dst)
{
    CARD32  s, d;
    CARD32  a;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskAlphaC (src, msk);
    a = s;
    if (a != 0xffffffff)
    {
	d = 0;
	if (a)
	{
	    d = (*dst->fetch) (dst);
	    m = FbInC(d,0,a,t);
	    n = FbInC(d,8,a,t);
	    o = FbInC(d,16,a,t);
	    p = FbInC(d,24,a,t);
	    d = m|n|o|p;
	}
	(*dst->store) (dst, d);
    }
}

static void
fbCombineOutU (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    d = (*dst->fetch) (dst);
    a = ~d >> 24;
    s = 0;
    if (a)
    {
	s = fbCombineMaskU (src, msk);
	if (a != 0xff)
	{
	    m = FbInU(s,0,a,t);
	    n = FbInU(s,8,a,t);
	    o = FbInU(s,16,a,t);
	    p = FbInU(s,24,a,t);
	    s = m|n|o|p;
	}
    }
    (*dst->store) (dst, s);
}

static void
fbCombineOutC (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    d = (*dst->fetch) (dst);
    a = ~d >> 24;
    s = 0;
    if (a)
    {
	s = fbCombineMaskValueC (src, msk);
	if (a != 0xff)
	{
	    m = FbInU(s,0,a,t);
	    n = FbInU(s,8,a,t);
	    o = FbInU(s,16,a,t);
	    p = FbInU(s,24,a,t);
	    s = m|n|o|p;
	}
    }
    (*dst->store) (dst, s);
}

static void
fbCombineOutReverseU (FbCompositeOperand *src,
		      FbCompositeOperand *msk,
		      FbCompositeOperand *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskAlphaU (src, msk);
    a = ~s >> 24;
    if (a != 0xff)
    {
	d = 0;
	if (a)
	{
	    d = (*dst->fetch) (dst);
	    m = FbInU(d,0,a,t);
	    n = FbInU(d,8,a,t);
	    o = FbInU(d,16,a,t);
	    p = FbInU(d,24,a,t);
	    d = m|n|o|p;
	}
	(*dst->store) (dst, d);
    }
}

static void
fbCombineOutReverseC (FbCompositeOperand *src,
		      FbCompositeOperand *msk,
		      FbCompositeOperand *dst)
{
    CARD32  s, d;
    CARD32  a;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskAlphaC (src, msk);
    a = ~s;
    if (a != 0xffffffff)
    {
	d = 0;
	if (a)
	{
	    d = (*dst->fetch) (dst);
	    m = FbInC(d,0,a,t);
	    n = FbInC(d,8,a,t);
	    o = FbInC(d,16,a,t);
	    p = FbInC(d,24,a,t);
	    d = m|n|o|p;
	}
	(*dst->store) (dst, d);
    }
}

static void
fbCombineAtopU (FbCompositeOperand   *src,
		FbCompositeOperand   *msk,
		FbCompositeOperand   *dst)
{
    CARD32  s, d;
    CARD16  ad, as;
    CARD16  t,u,v;
    CARD32  m,n,o,p;
    
    s = fbCombineMaskU (src, msk);
    d = (*dst->fetch) (dst);
    ad = ~s >> 24;
    as = d >> 24;
    m = FbGen(s,d,0,as,ad,t,u,v);
    n = FbGen(s,d,8,as,ad,t,u,v);
    o = FbGen(s,d,16,as,ad,t,u,v);
    p = FbGen(s,d,24,as,ad,t,u,v);
    (*dst->store) (dst, m|n|o|p);
}

static void
fbCombineAtopC (FbCompositeOperand   *src,
		FbCompositeOperand   *msk,
		FbCompositeOperand   *dst)
{
    FbCompSrc	cs;
    CARD32  s, d;
    CARD32  ad;
    CARD16  as;
    CARD16  t, u, v;
    CARD32  m,n,o,p;
    
    cs = fbCombineMaskC (src, msk);
    d = (*dst->fetch) (dst);
    s = cs.value;
    ad = cs.alpha;
    as = d >> 24;
    m = FbGen(s,d,0,as,FbGet8(ad,0),t,u,v);
    n = FbGen(s,d,8,as,FbGet8(ad,8),t,u,v);
    o = FbGen(s,d,16,as,FbGet8(ad,16),t,u,v);
    p = FbGen(s,d,24,as,FbGet8(ad,24),t,u,v);
    (*dst->store) (dst, m|n|o|p);
}

static void
fbCombineAtopReverseU (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD16  ad, as;
    CARD16  t, u, v;
    CARD32  m,n,o,p;
    
    s = fbCombineMaskU (src, msk);
    d = (*dst->fetch) (dst);
    ad = s >> 24;
    as = ~d >> 24;
    m = FbGen(s,d,0,as,ad,t,u,v);
    n = FbGen(s,d,8,as,ad,t,u,v);
    o = FbGen(s,d,16,as,ad,t,u,v);
    p = FbGen(s,d,24,as,ad,t,u,v);
    (*dst->store) (dst, m|n|o|p);
}

static void
fbCombineAtopReverseC (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    FbCompSrc	cs;
    CARD32  s, d, ad;
    CARD16  as;
    CARD16  t, u, v;
    CARD32  m,n,o,p;
    
    cs = fbCombineMaskC (src, msk);
    d = (*dst->fetch) (dst);
    s = cs.value;
    ad = cs.alpha;
    as = ~d >> 24;
    m = FbGen(s,d,0,as,FbGet8(ad,0),t,u,v);
    n = FbGen(s,d,8,as,FbGet8(ad,8),t,u,v);
    o = FbGen(s,d,16,as,FbGet8(ad,16),t,u,v);
    p = FbGen(s,d,24,as,FbGet8(ad,24),t,u,v);
    (*dst->store) (dst, m|n|o|p);
}

static void
fbCombineXorU (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD16  ad, as;
    CARD16  t, u, v;
    CARD32  m,n,o,p;
    
    s = fbCombineMaskU (src, msk);
    d = (*dst->fetch) (dst);
    ad = ~s >> 24;
    as = ~d >> 24;
    m = FbGen(s,d,0,as,ad,t,u,v);
    n = FbGen(s,d,8,as,ad,t,u,v);
    o = FbGen(s,d,16,as,ad,t,u,v);
    p = FbGen(s,d,24,as,ad,t,u,v);
    (*dst->store) (dst, m|n|o|p);
}

static void
fbCombineXorC (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    FbCompSrc	cs;
    CARD32  s, d, ad;
    CARD16  as;
    CARD16  t, u, v;
    CARD32  m,n,o,p;
    
    cs = fbCombineMaskC (src, msk);
    d = (*dst->fetch) (dst);
    s = cs.value;
    ad = ~cs.alpha;
    as = ~d >> 24;
    m = FbGen(s,d,0,as,ad,t,u,v);
    n = FbGen(s,d,8,as,ad,t,u,v);
    o = FbGen(s,d,16,as,ad,t,u,v);
    p = FbGen(s,d,24,as,ad,t,u,v);
    (*dst->store) (dst, m|n|o|p);
}

static void
fbCombineAddU (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskU (src, msk);
    if (s == ~0)
	(*dst->store) (dst, s);
    else
    {
	d = (*dst->fetch) (dst);
	if (s && d != ~0)
	{
	    m = FbAdd(s,d,0,t);
	    n = FbAdd(s,d,8,t);
	    o = FbAdd(s,d,16,t);
	    p = FbAdd(s,d,24,t);
	    (*dst->store) (dst, m|n|o|p);
	}
    }
}

static void
fbCombineAddC (FbCompositeOperand    *src,
	       FbCompositeOperand    *msk,
	       FbCompositeOperand    *dst)
{
    CARD32  s, d;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskValueC (src, msk);
    if (s == ~0)
	(*dst->store) (dst, s);
    else
    {
	d = (*dst->fetch) (dst);
	if (s && d != ~0)
	{
	    m = FbAdd(s,d,0,t);
	    n = FbAdd(s,d,8,t);
	    o = FbAdd(s,d,16,t);
	    p = FbAdd(s,d,24,t);
	    (*dst->store) (dst, m|n|o|p);
	}
    }
}

/*
 * All of the disjoint composing functions

 The four entries in the first column indicate what source contributions
 come from each of the four areas of the picture -- areas covered by neither
 A nor B, areas covered only by A, areas covered only by B and finally
 areas covered by both A and B.
 
		Disjoint			Conjoint
		Fa		Fb		Fa		Fb
(0,0,0,0)	0		0		0		0
(0,A,0,A)	1		0		1		0
(0,0,B,B)	0		1		0		1
(0,A,B,A)	1		min((1-a)/b,1)	1		max(1-a/b,0)
(0,A,B,B)	min((1-b)/a,1)	1		max(1-b/a,0)	1		
(0,0,0,A)	max(1-(1-b)/a,0) 0		min(1,b/a)	0
(0,0,0,B)	0		max(1-(1-a)/b,0) 0		min(a/b,1)
(0,A,0,0)	min(1,(1-b)/a)	0		max(1-b/a,0)	0
(0,0,B,0)	0		min(1,(1-a)/b)	0		max(1-a/b,0)
(0,0,B,A)	max(1-(1-b)/a,0) min(1,(1-a)/b)	 min(1,b/a)	max(1-a/b,0)
(0,A,0,B)	min(1,(1-b)/a)	max(1-(1-a)/b,0) max(1-b/a,0)	min(1,a/b)
(0,A,B,0)	min(1,(1-b)/a)	min(1,(1-a)/b)	max(1-b/a,0)	max(1-a/b,0)

 */

#define CombineAOut 1
#define CombineAIn  2
#define CombineBOut 4
#define CombineBIn  8

#define CombineClear	0
#define CombineA	(CombineAOut|CombineAIn)
#define CombineB	(CombineBOut|CombineBIn)
#define CombineAOver	(CombineAOut|CombineBOut|CombineAIn)
#define CombineBOver	(CombineAOut|CombineBOut|CombineBIn)
#define CombineAAtop	(CombineBOut|CombineAIn)
#define CombineBAtop	(CombineAOut|CombineBIn)
#define CombineXor	(CombineAOut|CombineBOut)

/* portion covered by a but not b */
static uint8_t
fbCombineDisjointOutPart (uint8_t a, uint8_t b)
{
    /* min (1, (1-b) / a) */
    
    b = ~b;		    /* 1 - b */
    if (b >= a)		    /* 1 - b >= a -> (1-b)/a >= 1 */
	return 0xff;	    /* 1 */
    return FbIntDiv(b,a);   /* (1-b) / a */
}

/* portion covered by both a and b */
static uint8_t
fbCombineDisjointInPart (uint8_t a, uint8_t b)
{
    /* max (1-(1-b)/a,0) */
    /*  = - min ((1-b)/a - 1, 0) */
    /*  = 1 - min (1, (1-b)/a) */

    b = ~b;		    /* 1 - b */
    if (b >= a)		    /* 1 - b >= a -> (1-b)/a >= 1 */
	return 0;	    /* 1 - 1 */
    return ~FbIntDiv(b,a);  /* 1 - (1-b) / a */
}

static void
fbCombineDisjointGeneralU (FbCompositeOperand   *src,
			   FbCompositeOperand   *msk,
			   FbCompositeOperand   *dst,
			   CARD8		combine)
{
    CARD32  s, d;
    CARD32  m,n,o,p;
    CARD16  Fa, Fb, t, u, v;
    CARD8   sa, da;

    s = fbCombineMaskU (src, msk);
    sa = s >> 24;
    
    d = (*dst->fetch) (dst);
    da = d >> 24;
    
    switch (combine & CombineA) {
    default:
	Fa = 0;
	break;
    case CombineAOut:
	Fa = fbCombineDisjointOutPart (sa, da);
	break;
    case CombineAIn:
	Fa = fbCombineDisjointInPart (sa, da);
	break;
    case CombineA:
	Fa = 0xff;
	break;
    }
    
    switch (combine & CombineB) {
    default:
	Fb = 0;
	break;
    case CombineBOut:
	Fb = fbCombineDisjointOutPart (da, sa);
	break;
    case CombineBIn:
	Fb = fbCombineDisjointInPart (da, sa);
	break;
    case CombineB:
	Fb = 0xff;
	break;
    }
    m = FbGen (s,d,0,Fa,Fb,t,u,v);
    n = FbGen (s,d,8,Fa,Fb,t,u,v);
    o = FbGen (s,d,16,Fa,Fb,t,u,v);
    p = FbGen (s,d,24,Fa,Fb,t,u,v);
    s = m|n|o|p;
    (*dst->store) (dst, s);
}

static void
fbCombineDisjointGeneralC (FbCompositeOperand   *src,
			   FbCompositeOperand   *msk,
			   FbCompositeOperand   *dst,
			   CARD8		combine)
{
    FbCompSrc	cs;
    CARD32  s, d;
    CARD32  m,n,o,p;
    CARD32  Fa;
    CARD16  Fb, t, u, v;
    CARD32  sa;
    CARD8   da;

    cs = fbCombineMaskC (src, msk);
    s = cs.value;
    sa = cs.alpha;
    
    d = (*dst->fetch) (dst);
    da = d >> 24;
    
    switch (combine & CombineA) {
    default:
	Fa = 0;
	break;
    case CombineAOut:
	m = fbCombineDisjointOutPart ((CARD8) (sa >> 0), da);
	n = fbCombineDisjointOutPart ((CARD8) (sa >> 8), da) << 8;
	o = fbCombineDisjointOutPart ((CARD8) (sa >> 16), da) << 16;
	p = fbCombineDisjointOutPart ((CARD8) (sa >> 24), da) << 24;
	Fa = m|n|o|p;
	break;
    case CombineAIn:
	m = fbCombineDisjointOutPart ((CARD8) (sa >> 0), da);
	n = fbCombineDisjointOutPart ((CARD8) (sa >> 8), da) << 8;
	o = fbCombineDisjointOutPart ((CARD8) (sa >> 16), da) << 16;
	p = fbCombineDisjointOutPart ((CARD8) (sa >> 24), da) << 24;
	Fa = m|n|o|p;
	break;
    case CombineA:
	Fa = 0xffffffff;
	break;
    }
    
    switch (combine & CombineB) {
    default:
	Fb = 0;
	break;
    case CombineBOut:
	Fb = fbCombineDisjointOutPart (da, sa);
	break;
    case CombineBIn:
	Fb = fbCombineDisjointInPart (da, sa);
	break;
    case CombineB:
	Fb = 0xff;
	break;
    }
    m = FbGen (s,d,0,FbGet8(Fa,0),Fb,t,u,v);
    n = FbGen (s,d,8,FbGet8(Fa,8),Fb,t,u,v);
    o = FbGen (s,d,16,FbGet8(Fa,16),Fb,t,u,v);
    p = FbGen (s,d,24,FbGet8(Fa,24),Fb,t,u,v);
    s = m|n|o|p;
    (*dst->store) (dst, s);
}

static void
fbCombineDisjointOverU (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskU (src, msk);
    a = s >> 24;
    if (a != 0x00)
    {
	if (a != 0xff)
	{
	    d = (*dst->fetch) (dst);
	    a = fbCombineDisjointOutPart (d >> 24, a);
	    m = FbOverU(s,d,0,a,t);
	    n = FbOverU(s,d,8,a,t);
	    o = FbOverU(s,d,16,a,t);
	    p = FbOverU(s,d,24,a,t);
	    s = m|n|o|p;
	}
	(*dst->store) (dst, s);
    }
}

static void
fbCombineDisjointOverC (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineAOver);
}

static void
fbCombineDisjointOverReverseU (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineBOver);
}

static void
fbCombineDisjointOverReverseC (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineBOver);
}

static void
fbCombineDisjointInU (FbCompositeOperand	    *src,
		      FbCompositeOperand	    *msk,
		      FbCompositeOperand	    *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineAIn);
}

static void
fbCombineDisjointInC (FbCompositeOperand	    *src,
		      FbCompositeOperand	    *msk,
		      FbCompositeOperand	    *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineAIn);
}

static void
fbCombineDisjointInReverseU (FbCompositeOperand  *src,
			     FbCompositeOperand  *msk,
			     FbCompositeOperand  *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineBIn);
}

static void
fbCombineDisjointInReverseC (FbCompositeOperand  *src,
			     FbCompositeOperand  *msk,
			     FbCompositeOperand  *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineBIn);
}

static void
fbCombineDisjointOutU (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineAOut);
}

static void
fbCombineDisjointOutC (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineAOut);
}

static void
fbCombineDisjointOutReverseU (FbCompositeOperand *src,
			      FbCompositeOperand *msk,
			      FbCompositeOperand *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineBOut);
}

static void
fbCombineDisjointOutReverseC (FbCompositeOperand *src,
			      FbCompositeOperand *msk,
			      FbCompositeOperand *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineBOut);
}

static void
fbCombineDisjointAtopU (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineAAtop);
}

static void
fbCombineDisjointAtopC (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineAAtop);
}

static void
fbCombineDisjointAtopReverseU (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineBAtop);
}

static void
fbCombineDisjointAtopReverseC (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineBAtop);
}

static void
fbCombineDisjointXorU (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralU (src, msk, dst, CombineXor);
}

static void
fbCombineDisjointXorC (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineDisjointGeneralC (src, msk, dst, CombineXor);
}

/* portion covered by a but not b */
static uint8_t
fbCombineConjointOutPart (uint8_t a, uint8_t b)
{
    /* max (1-b/a,0) */
    /* = 1-min(b/a,1) */
    
    /* min (1, (1-b) / a) */
    
    if (b >= a)		    /* b >= a -> b/a >= 1 */
	return 0x00;	    /* 0 */
    return ~FbIntDiv(b,a);   /* 1 - b/a */
}

/* portion covered by both a and b */
static uint8_t
fbCombineConjointInPart (uint8_t a, uint8_t b)
{
    /* min (1,b/a) */

    if (b >= a)		    /* b >= a -> b/a >= 1 */
	return 0xff;	    /* 1 */
    return FbIntDiv(b,a);   /* b/a */
}

static void
fbCombineConjointGeneralU (FbCompositeOperand   *src,
			   FbCompositeOperand   *msk,
			   FbCompositeOperand   *dst,
			   CARD8		combine)
{
    CARD32  s, d;
    CARD32  m,n,o,p;
    CARD16  Fa, Fb, t, u, v;
    CARD8   sa, da;

    s = fbCombineMaskU (src, msk);
    sa = s >> 24;
    
    d = (*dst->fetch) (dst);
    da = d >> 24;
    
    switch (combine & CombineA) {
    default:
	Fa = 0;
	break;
    case CombineAOut:
	Fa = fbCombineConjointOutPart (sa, da);
	break;
    case CombineAIn:
	Fa = fbCombineConjointInPart (sa, da);
	break;
    case CombineA:
	Fa = 0xff;
	break;
    }
    
    switch (combine & CombineB) {
    default:
	Fb = 0;
	break;
    case CombineBOut:
	Fb = fbCombineConjointOutPart (da, sa);
	break;
    case CombineBIn:
	Fb = fbCombineConjointInPart (da, sa);
	break;
    case CombineB:
	Fb = 0xff;
	break;
    }
    m = FbGen (s,d,0,Fa,Fb,t,u,v);
    n = FbGen (s,d,8,Fa,Fb,t,u,v);
    o = FbGen (s,d,16,Fa,Fb,t,u,v);
    p = FbGen (s,d,24,Fa,Fb,t,u,v);
    s = m|n|o|p;
    (*dst->store) (dst, s);
}

static void
fbCombineConjointGeneralC (FbCompositeOperand   *src,
			   FbCompositeOperand   *msk,
			   FbCompositeOperand   *dst,
			   CARD8		combine)
{
    FbCompSrc	cs;
    CARD32  s, d;
    CARD32  m,n,o,p;
    CARD32  Fa;
    CARD16  Fb, t, u, v;
    CARD32  sa;
    CARD8   da;

    cs = fbCombineMaskC (src, msk);
    s = cs.value;
    sa = cs.alpha;
    
    d = (*dst->fetch) (dst);
    da = d >> 24;
    
    switch (combine & CombineA) {
    default:
	Fa = 0;
	break;
    case CombineAOut:
	m = fbCombineConjointOutPart ((CARD8) (sa >> 0), da);
	n = fbCombineConjointOutPart ((CARD8) (sa >> 8), da) << 8;
	o = fbCombineConjointOutPart ((CARD8) (sa >> 16), da) << 16;
	p = fbCombineConjointOutPart ((CARD8) (sa >> 24), da) << 24;
	Fa = m|n|o|p;
	break;
    case CombineAIn:
	m = fbCombineConjointOutPart ((CARD8) (sa >> 0), da);
	n = fbCombineConjointOutPart ((CARD8) (sa >> 8), da) << 8;
	o = fbCombineConjointOutPart ((CARD8) (sa >> 16), da) << 16;
	p = fbCombineConjointOutPart ((CARD8) (sa >> 24), da) << 24;
	Fa = m|n|o|p;
	break;
    case CombineA:
	Fa = 0xffffffff;
	break;
    }
    
    switch (combine & CombineB) {
    default:
	Fb = 0;
	break;
    case CombineBOut:
	Fb = fbCombineConjointOutPart (da, sa);
	break;
    case CombineBIn:
	Fb = fbCombineConjointInPart (da, sa);
	break;
    case CombineB:
	Fb = 0xff;
	break;
    }
    m = FbGen (s,d,0,FbGet8(Fa,0),Fb,t,u,v);
    n = FbGen (s,d,8,FbGet8(Fa,8),Fb,t,u,v);
    o = FbGen (s,d,16,FbGet8(Fa,16),Fb,t,u,v);
    p = FbGen (s,d,24,FbGet8(Fa,24),Fb,t,u,v);
    s = m|n|o|p;
    (*dst->store) (dst, s);
}

static void
fbCombineConjointOverU (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineAOver);
/*
    CARD32  s, d;
    CARD16  a;
    CARD16  t;
    CARD32  m,n,o,p;

    s = fbCombineMaskU (src, msk);
    a = s >> 24;
    if (a != 0x00)
    {
	if (a != 0xff)
	{
	    d = (*dst->fetch) (dst);
	    a = fbCombineConjointOutPart (d >> 24, a);
	    m = FbOverU(s,d,0,a,t);
	    n = FbOverU(s,d,8,a,t);
	    o = FbOverU(s,d,16,a,t);
	    p = FbOverU(s,d,24,a,t);
	    s = m|n|o|p;
	}
	(*dst->store) (dst, s);
    }
 */
}

static void
fbCombineConjointOverC (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineAOver);
}

static void
fbCombineConjointOverReverseU (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineBOver);
}

static void
fbCombineConjointOverReverseC (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineBOver);
}

static void
fbCombineConjointInU (FbCompositeOperand	    *src,
		      FbCompositeOperand	    *msk,
		      FbCompositeOperand	    *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineAIn);
}

static void
fbCombineConjointInC (FbCompositeOperand	    *src,
		      FbCompositeOperand	    *msk,
		      FbCompositeOperand	    *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineAIn);
}

static void
fbCombineConjointInReverseU (FbCompositeOperand  *src,
			     FbCompositeOperand  *msk,
			     FbCompositeOperand  *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineBIn);
}

static void
fbCombineConjointInReverseC (FbCompositeOperand  *src,
			     FbCompositeOperand  *msk,
			     FbCompositeOperand  *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineBIn);
}

static void
fbCombineConjointOutU (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineAOut);
}

static void
fbCombineConjointOutC (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineAOut);
}

static void
fbCombineConjointOutReverseU (FbCompositeOperand *src,
			      FbCompositeOperand *msk,
			      FbCompositeOperand *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineBOut);
}

static void
fbCombineConjointOutReverseC (FbCompositeOperand *src,
			      FbCompositeOperand *msk,
			      FbCompositeOperand *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineBOut);
}

static void
fbCombineConjointAtopU (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineAAtop);
}

static void
fbCombineConjointAtopC (FbCompositeOperand   *src,
			FbCompositeOperand   *msk,
			FbCompositeOperand   *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineAAtop);
}

static void
fbCombineConjointAtopReverseU (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineBAtop);
}

static void
fbCombineConjointAtopReverseC (FbCompositeOperand    *src,
			       FbCompositeOperand    *msk,
			       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineBAtop);
}

static void
fbCombineConjointXorU (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralU (src, msk, dst, CombineXor);
}

static void
fbCombineConjointXorC (FbCompositeOperand    *src,
		       FbCompositeOperand    *msk,
		       FbCompositeOperand    *dst)
{
    fbCombineConjointGeneralC (src, msk, dst, CombineXor);
}

static FbCombineFunc const fbCombineFuncU[] = {
    fbCombineClear,
    fbCombineSrcU,
    fbCombineDst,
    fbCombineOverU,
    fbCombineOverReverseU,
    fbCombineInU,
    fbCombineInReverseU,
    fbCombineOutU,
    fbCombineOutReverseU,
    fbCombineAtopU,
    fbCombineAtopReverseU,
    fbCombineXorU,
    fbCombineAddU,
    fbCombineDisjointOverU, /* Saturate */
    0,
    0,
    fbCombineClear,
    fbCombineSrcU,
    fbCombineDst,
    fbCombineDisjointOverU,
    fbCombineDisjointOverReverseU,
    fbCombineDisjointInU,
    fbCombineDisjointInReverseU,
    fbCombineDisjointOutU,
    fbCombineDisjointOutReverseU,
    fbCombineDisjointAtopU,
    fbCombineDisjointAtopReverseU,
    fbCombineDisjointXorU,
    0,
    0,
    0,
    0,
    fbCombineClear,
    fbCombineSrcU,
    fbCombineDst,
    fbCombineConjointOverU,
    fbCombineConjointOverReverseU,
    fbCombineConjointInU,
    fbCombineConjointInReverseU,
    fbCombineConjointOutU,
    fbCombineConjointOutReverseU,
    fbCombineConjointAtopU,
    fbCombineConjointAtopReverseU,
    fbCombineConjointXorU,
};

static FbCombineFunc const fbCombineFuncC[] = {
    fbCombineClear,
    fbCombineSrcC,
    fbCombineDst,
    fbCombineOverC,
    fbCombineOverReverseC,
    fbCombineInC,
    fbCombineInReverseC,
    fbCombineOutC,
    fbCombineOutReverseC,
    fbCombineAtopC,
    fbCombineAtopReverseC,
    fbCombineXorC,
    fbCombineAddC,
    fbCombineDisjointOverC, /* Saturate */
    0,
    0,
    fbCombineClear,	    /* 0x10 */
    fbCombineSrcC,
    fbCombineDst,
    fbCombineDisjointOverC,
    fbCombineDisjointOverReverseC,
    fbCombineDisjointInC,
    fbCombineDisjointInReverseC,
    fbCombineDisjointOutC,
    fbCombineDisjointOutReverseC,
    fbCombineDisjointAtopC,
    fbCombineDisjointAtopReverseC,
    fbCombineDisjointXorC,  /* 0x1b */
    0,
    0,
    0,
    0,
    fbCombineClear,
    fbCombineSrcC,
    fbCombineDst,
    fbCombineConjointOverC,
    fbCombineConjointOverReverseC,
    fbCombineConjointInC,
    fbCombineConjointInReverseC,
    fbCombineConjointOutC,
    fbCombineConjointOutReverseC,
    fbCombineConjointAtopC,
    fbCombineConjointAtopReverseC,
    fbCombineConjointXorC,
};

/*
 * All of the fetch functions
 */

static uint32_t
fbFetch_a8r8g8b8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    return ((CARD32 *)line)[offset >> 5];
}

static uint32_t
fbFetch_x8r8g8b8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    return ((CARD32 *)line)[offset >> 5] | 0xff000000;
}

static uint32_t
fbFetch_a8b8g8r8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD32 *)line)[offset >> 5];

    return ((pixel & 0xff000000) |
	    ((pixel >> 16) & 0xff) |
	    (pixel & 0x0000ff00) |
	    ((pixel & 0xff) << 16));
}

static uint32_t
fbFetch_x8b8g8r8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD32 *)line)[offset >> 5];

    return ((0xff000000) |
	    ((pixel >> 16) & 0xff) |
	    (pixel & 0x0000ff00) |
	    ((pixel & 0xff) << 16));
}

static uint32_t
fbFetch_r8g8b8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD8   *pixel = ((CARD8 *) line) + (offset >> 3);
#if IMAGE_BYTE_ORDER == MSBFirst
    return (0xff000000 |
	    (pixel[0] << 16) |
	    (pixel[1] << 8) |
	    (pixel[2]));
#else
    return (0xff000000 |
	    (pixel[2] << 16) |
	    (pixel[1] << 8) |
	    (pixel[0]));
#endif
}

static uint32_t
fbFetch_b8g8r8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD8   *pixel = ((CARD8 *) line) + (offset >> 3);
#if IMAGE_BYTE_ORDER == MSBFirst
    return (0xff000000 |
	    (pixel[2] << 16) |
	    (pixel[1] << 8) |
	    (pixel[0]));
#else
    return (0xff000000 |
	    (pixel[0] << 16) |
	    (pixel[1] << 8) |
	    (pixel[2]));
#endif
}

static uint32_t
fbFetch_r5g6b5 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  r,g,b;

    r = ((pixel & 0xf800) | ((pixel & 0xe000) >> 5)) << 8;
    g = ((pixel & 0x07e0) | ((pixel & 0x0600) >> 6)) << 5;
    b = ((pixel & 0x001c) | ((pixel & 0x001f) << 5)) >> 2;
    return (0xff000000 | r | g | b);
}

static uint32_t
fbFetch_b5g6r5 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  r,g,b;

    b = ((pixel & 0xf800) | ((pixel & 0xe000) >> 5)) >> 8;
    g = ((pixel & 0x07e0) | ((pixel & 0x0600) >> 6)) << 5;
    r = ((pixel & 0x001c) | ((pixel & 0x001f) << 5)) << 14;
    return (0xff000000 | r | g | b);
}

static uint32_t
fbFetch_a1r5g5b5 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  a,r,g,b;

    a = (CARD32) ((CARD8) (0 - ((pixel & 0x8000) >> 15))) << 24;
    r = ((pixel & 0x7c00) | ((pixel & 0x7000) >> 5)) << 9;
    g = ((pixel & 0x03e0) | ((pixel & 0x0380) >> 5)) << 6;
    b = ((pixel & 0x001c) | ((pixel & 0x001f) << 5)) >> 2;
    return (a | r | g | b);
}

static uint32_t
fbFetch_x1r5g5b5 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  r,g,b;

    r = ((pixel & 0x7c00) | ((pixel & 0x7000) >> 5)) << 9;
    g = ((pixel & 0x03e0) | ((pixel & 0x0380) >> 5)) << 6;
    b = ((pixel & 0x001c) | ((pixel & 0x001f) << 5)) >> 2;
    return (0xff000000 | r | g | b);
}

static uint32_t
fbFetch_a1b5g5r5 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  a,r,g,b;

    a = (CARD32) ((CARD8) (0 - ((pixel & 0x8000) >> 15))) << 24;
    b = ((pixel & 0x7c00) | ((pixel & 0x7000) >> 5)) >> 7;
    g = ((pixel & 0x03e0) | ((pixel & 0x0380) >> 5)) << 6;
    r = ((pixel & 0x001c) | ((pixel & 0x001f) << 5)) << 14;
    return (a | r | g | b);
}

static uint32_t
fbFetch_x1b5g5r5 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  r,g,b;

    b = ((pixel & 0x7c00) | ((pixel & 0x7000) >> 5)) >> 7;
    g = ((pixel & 0x03e0) | ((pixel & 0x0380) >> 5)) << 6;
    r = ((pixel & 0x001c) | ((pixel & 0x001f) << 5)) << 14;
    return (0xff000000 | r | g | b);
}

static uint32_t
fbFetch_a4r4g4b4 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  a,r,g,b;

    a = ((pixel & 0xf000) | ((pixel & 0xf000) >> 4)) << 16;
    r = ((pixel & 0x0f00) | ((pixel & 0x0f00) >> 4)) << 12;
    g = ((pixel & 0x00f0) | ((pixel & 0x00f0) >> 4)) << 8;
    b = ((pixel & 0x000f) | ((pixel & 0x000f) << 4));
    return (a | r | g | b);
}
    
static uint32_t
fbFetch_x4r4g4b4 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  r,g,b;

    r = ((pixel & 0x0f00) | ((pixel & 0x0f00) >> 4)) << 12;
    g = ((pixel & 0x00f0) | ((pixel & 0x00f0) >> 4)) << 8;
    b = ((pixel & 0x000f) | ((pixel & 0x000f) << 4));
    return (0xff000000 | r | g | b);
}
    
static uint32_t
fbFetch_a4b4g4r4 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  a,r,g,b;

    a = ((pixel & 0xf000) | ((pixel & 0xf000) >> 4)) << 16;
    b = ((pixel & 0x0f00) | ((pixel & 0x0f00) >> 4)) << 12;
    g = ((pixel & 0x00f0) | ((pixel & 0x00f0) >> 4)) << 8;
    r = ((pixel & 0x000f) | ((pixel & 0x000f) << 4));
    return (a | r | g | b);
}
    
static uint32_t
fbFetch_x4b4g4r4 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD16 *) line)[offset >> 4];
    CARD32  r,g,b;

    b = ((pixel & 0x0f00) | ((pixel & 0x0f00) >> 4)) << 12;
    g = ((pixel & 0x00f0) | ((pixel & 0x00f0) >> 4)) << 8;
    r = ((pixel & 0x000f) | ((pixel & 0x000f) << 4));
    return (0xff000000 | r | g | b);
}
    
static uint32_t
fbFetch_a8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32   pixel = ((CARD8 *) line)[offset>>3];
    
    return pixel << 24;
}

static uint32_t
fbFetcha_a8 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32   pixel = ((CARD8 *) line)[offset>>3];
    
    pixel |= pixel << 8;
    pixel |= pixel << 16;
    return pixel;
}

static uint32_t
fbFetch_r3g3b2 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32   pixel = ((CARD8 *) line)[offset>>3];
    CARD32  r,g,b;
    
    r = ((pixel & 0xe0) | ((pixel & 0xe0) >> 3) | ((pixel & 0xc0) >> 6)) << 16;
    g = ((pixel & 0x1c) | ((pixel & 0x18) >> 3) | ((pixel & 0x1c) << 3)) << 8;
    b = (((pixel & 0x03)     ) | 
	 ((pixel & 0x03) << 2) | 
	 ((pixel & 0x03) << 4) |
	 ((pixel & 0x03) << 6));
    return (0xff000000 | r | g | b);
}

static uint32_t
fbFetch_b2g3r3 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32   pixel = ((CARD8 *) line)[offset>>3];
    CARD32  r,g,b;
    
    b = (((pixel & 0xc0)     ) | 
	 ((pixel & 0xc0) >> 2) |
	 ((pixel & 0xc0) >> 4) |
	 ((pixel & 0xc0) >> 6));
    g = ((pixel & 0x38) | ((pixel & 0x38) >> 3) | ((pixel & 0x30) << 2)) << 8;
    r = (((pixel & 0x07)     ) | 
	 ((pixel & 0x07) << 3) | 
	 ((pixel & 0x06) << 6)) << 16;
    return (0xff000000 | r | g | b);
}

static uint32_t
fbFetch_a2r2g2b2 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32   pixel = ((CARD8 *) line)[offset>>3];
    CARD32   a,r,g,b;

    a = ((pixel & 0xc0) * 0x55) << 18;
    r = ((pixel & 0x30) * 0x55) << 12;
    g = ((pixel & 0x0c) * 0x55) << 6;
    b = ((pixel & 0x03) * 0x55);
    return a|r|g|b;
}

#define Fetch8(l,o)    (((CARD8 *) (l))[(o) >> 3])
#if IMAGE_BYTE_ORDER == MSBFirst
#define Fetch4(l,o)    ((o) & 2 ? Fetch8(l,o) & 0xf : Fetch8(l,o) >> 4)
#else
#define Fetch4(l,o)    ((o) & 2 ? Fetch8(l,o) >> 4 : Fetch8(l,o) & 0xf)
#endif

static uint32_t
fbFetch_a4 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = Fetch4(line, offset);
    
    pixel |= pixel << 4;
    return pixel << 24;
}

static uint32_t
fbFetcha_a4 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = Fetch4(line, offset);
    
    pixel |= pixel << 4;
    pixel |= pixel << 8;
    pixel |= pixel << 16;
    return pixel;
}

static uint32_t
fbFetch_r1g2b1 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = Fetch4(line, offset);
    CARD32  r,g,b;

    r = ((pixel & 0x8) * 0xff) << 13;
    g = ((pixel & 0x6) * 0x55) << 7;
    b = ((pixel & 0x1) * 0xff);
    return 0xff000000|r|g|b;
}

static uint32_t
fbFetch_b1g2r1 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = Fetch4(line, offset);
    CARD32  r,g,b;

    b = ((pixel & 0x8) * 0xff) >> 3;
    g = ((pixel & 0x6) * 0x55) << 7;
    r = ((pixel & 0x1) * 0xff) << 16;
    return 0xff000000|r|g|b;
}

static uint32_t
fbFetch_a1r1g1b1 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = Fetch4(line, offset);
    CARD32  a,r,g,b;

    a = ((pixel & 0x8) * 0xff) << 21;
    r = ((pixel & 0x4) * 0xff) << 14;
    g = ((pixel & 0x2) * 0xff) << 7;
    b = ((pixel & 0x1) * 0xff);
    return a|r|g|b;
}

static uint32_t
fbFetch_a1b1g1r1 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = Fetch4(line, offset);
    CARD32  a,r,g,b;

    a = ((pixel & 0x8) * 0xff) << 21;
    r = ((pixel & 0x4) * 0xff) >> 3;
    g = ((pixel & 0x2) * 0xff) << 7;
    b = ((pixel & 0x1) * 0xff) << 16;
    return a|r|g|b;
}

static uint32_t
fbFetcha_a1 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD32 *)line)[offset >> 5];
    CARD32  a;
#if BITMAP_BIT_ORDER == MSBFirst
    a = pixel >> (0x1f - (offset & 0x1f));
#else
    a = pixel >> (offset & 0x1f);
#endif
    a = a & 1;
    a |= a << 1;
    a |= a << 2;
    a |= a << 4;
    a |= a << 8;
    a |= a << 16;
    return a;
}

static uint32_t
fbFetch_a1 (FbCompositeOperand *op)
{
    FbBits  *line = op->u.drawable.line; CARD32 offset = op->u.drawable.offset;
    CARD32  pixel = ((CARD32 *)line)[offset >> 5];
    CARD32  a;
#if BITMAP_BIT_ORDER == MSBFirst
    a = pixel >> (0x1f - (offset & 0x1f));
#else
    a = pixel >> (offset & 0x1f);
#endif
    a = a & 1;
    a |= a << 1;
    a |= a << 2;
    a |= a << 4;
    return a << 24;
}

/*
 * All the store functions
 */

#define Splita(v)	uint32_t	a = ((v) >> 24), r = ((v) >> 16) & 0xff, g = ((v) >> 8) & 0xff, b = (v) & 0xff
#define Split(v)	uint32_t	r = ((v) >> 16) & 0xff, g = ((v) >> 8) & 0xff, b = (v) & 0xff

static void
fbStore_a8r8g8b8 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    ((uint32_t *)line)[offset >> 5] = value;
}

static void
fbStore_x8r8g8b8 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    ((uint32_t *)line)[offset >> 5] = value & 0xffffff;
}

static void
fbStore_a8b8g8r8 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    Splita(value);
    ((uint32_t *)line)[offset >> 5] = a << 24 | b << 16 | g << 8 | r;
}

static void
fbStore_x8b8g8r8 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    Split(value);
    ((uint32_t *)line)[offset >> 5] = b << 16 | g << 8 | r;
}

static void
fbStore_r8g8b8 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint8_t   *pixel = ((uint8_t *) line) + (offset >> 3);
    Split(value);
#if IMAGE_BYTE_ORDER == MSBFirst
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
#else
    pixel[0] = b;
    pixel[1] = g;
    pixel[2] = r;
#endif
}

static void
fbStore_b8g8r8 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint8_t   *pixel = ((uint8_t *) line) + (offset >> 3);
    Split(value);
#if IMAGE_BYTE_ORDER == MSBFirst
    pixel[0] = b;
    pixel[1] = g;
    pixel[2] = r;
#else
    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;
#endif
}

static void
fbStore_r5g6b5 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Split(value);
    *pixel = (((r << 8) & 0xf800) |
	      ((g << 3) & 0x07e0) |
	      ((b >> 3)         ));
}

static void
fbStore_b5g6r5 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Split(value);
    *pixel = (((b << 8) & 0xf800) |
	      ((g << 3) & 0x07e0) |
	      ((r >> 3)         ));
}

static void
fbStore_a1r5g5b5 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Splita(value);
    *pixel = (((a << 8) & 0x8000) |
	      ((r << 7) & 0x7c00) |
	      ((g << 2) & 0x03e0) |
	      ((b >> 3)         ));
}

static void
fbStore_x1r5g5b5 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Split(value);
    *pixel = (((r << 7) & 0x7c00) |
	      ((g << 2) & 0x03e0) |
	      ((b >> 3)         ));
}

static void
fbStore_a1b5g5r5 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Splita(value);
    *pixel = (((a << 8) & 0x8000) |
	      ((b << 7) & 0x7c00) |
	      ((g << 2) & 0x03e0) |
	      ((r >> 3)         ));
}

static void
fbStore_x1b5g5r5 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Split(value);
    *pixel = (((b << 7) & 0x7c00) |
	      ((g << 2) & 0x03e0) |
	      ((r >> 3)         ));
}

static void
fbStore_a4r4g4b4 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Splita(value);
    *pixel = (((a << 8) & 0xf000) |
	      ((r << 4) & 0x0f00) |
	      ((g     ) & 0x00f0) |
	      ((b >> 4)         ));
}

static void
fbStore_x4r4g4b4 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Split(value);
    *pixel = (((r << 4) & 0x0f00) |
	      ((g     ) & 0x00f0) |
	      ((b >> 4)         ));
}

static void
fbStore_a4b4g4r4 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Splita(value);
    *pixel = (((a << 8) & 0xf000) |
	      ((b << 4) & 0x0f00) |
	      ((g     ) & 0x00f0) |
	      ((r >> 4)         ));
}

static void
fbStore_x4b4g4r4 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint16_t  *pixel = ((uint16_t *) line) + (offset >> 4);
    Split(value);
    *pixel = (((b << 4) & 0x0f00) |
	      ((g     ) & 0x00f0) |
	      ((r >> 4)         ));
}

static void
fbStore_a8 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint8_t   *pixel = ((uint8_t *) line) + (offset >> 3);
    *pixel = value >> 24;
}

static void
fbStore_r3g3b2 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint8_t   *pixel = ((uint8_t *) line) + (offset >> 3);
    Split(value);
    *pixel = (((r     ) & 0xe0) |
	      ((g >> 3) & 0x1c) |
	      ((b >> 6)       ));
}

static void
fbStore_b2g3r3 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint8_t   *pixel = ((uint8_t *) line) + (offset >> 3);
    Split(value);
    *pixel = (((b     ) & 0xe0) |
	      ((g >> 3) & 0x1c) |
	      ((r >> 6)       ));
}

static void
fbStore_a2r2g2b2 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint8_t   *pixel = ((uint8_t *) line) + (offset >> 3);
    Splita(value);
    *pixel = (((a     ) & 0xc0) |
	      ((r >> 2) & 0x30) |
	      ((g >> 4) & 0x0c) |
	      ((b >> 6)       ));
}

#define Store8(l,o,v)  (((uint8_t *) l)[(o) >> 3] = (v))
#if IMAGE_BYTE_ORDER == MSBFirst
#define Store4(l,o,v)  Store8(l,o,((o) & 4 ? \
				   (Fetch8(l,o) & 0xf0) | (v) : \
				   (Fetch8(l,o) & 0x0f) | ((v) << 4)))
#else
#define Store4(l,o,v)  Store8(l,o,((o) & 4 ? \
				   (Fetch8(l,o) & 0x0f) | ((v) << 4) : \
				   (Fetch8(l,o) & 0xf0) | (v)))
#endif

static void
fbStore_a4 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    Store4(line,offset,value>>28);
}

static void
fbStore_r1g2b1 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint32_t  pixel;
    
    Split(value);
    pixel = (((r >> 4) & 0x8) |
	     ((g >> 5) & 0x6) |
	     ((b >> 7)      ));
    Store4(line,offset,pixel);
}

static void
fbStore_b1g2r1 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint32_t  pixel;
    
    Split(value);
    pixel = (((b >> 4) & 0x8) |
	     ((g >> 5) & 0x6) |
	     ((r >> 7)      ));
    Store4(line,offset,pixel);
}

static void
fbStore_a1r1g1b1 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint32_t  pixel;
    Splita(value);
    pixel = (((a >> 4) & 0x8) |
	     ((r >> 5) & 0x4) |
	     ((g >> 6) & 0x2) |
	     ((b >> 7)      ));
    Store4(line,offset,pixel);
}

static void
fbStore_a1b1g1r1 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint32_t  pixel;
    Splita(value);
    pixel = (((a >> 4) & 0x8) |
	     ((b >> 5) & 0x4) |
	     ((g >> 6) & 0x2) |
	     ((r >> 7)      ));
    Store4(line,offset,pixel);
}

static void
fbStore_a1 (FbCompositeOperand *op, uint32_t value)
{
    FbBits  *line = op->u.drawable.line; uint32_t offset = op->u.drawable.offset;
    uint32_t  *pixel = ((uint32_t *) line) + (offset >> 5);
    uint32_t  mask = FbStipMask(offset & 0x1f, 1);

    value = value & 0x80000000 ? mask : 0;
    *pixel = (*pixel & ~mask) | value;
}

static uint32_t
fbFetch_external (FbCompositeOperand *op)
{
    uint32_t  rgb = (*op[1].fetch) (&op[1]);
    uint32_t  a = (*op[2].fetch) (&op[2]);

    return (rgb & 0xffffff) | (a & 0xff000000);
}


static uint32_t
fbFetcha_external (FbCompositeOperand *op)
{
    return (*op[2].fetch) (&op[2]);
}

static void
fbStore_external (FbCompositeOperand *op, uint32_t value)
{
    (*op[1].store) (&op[1], value | 0xff000000);
    (*op[2].store) (&op[2], value & 0xff000000);
}

static uint32_t
fbFetch_transform (FbCompositeOperand *op)
{
    pixman_vector_t	v;
    int		x, y;
    int		minx, maxx, miny, maxy;
    uint32_t	rtot, gtot, btot, atot;
    uint32_t	xerr, yerr;
    uint32_t	bits;
    pixman_box16_t	box;

    v.vector[0] = IntToxFixed(op->u.transform.x);
    v.vector[1] = IntToxFixed(op->u.transform.y);
    v.vector[2] = xFixed1;
    if (!pixman_transform_point (op->u.transform.transform, &v))
	return 0;
    switch (op->u.transform.filter) {
    case PIXMAN_FILTER_FAST:
    case PIXMAN_FILTER_NEAREST:
	y = xFixedToInt (v.vector[1]) + op->u.transform.top_y;
	x = xFixedToInt (v.vector[0]) + op->u.transform.left_x;
	if (op->u.transform.repeat)
	{
	    y = MOD (y, op->u.transform.height);
	    x = MOD (x, op->u.transform.width);
	}
	if (pixman_region_contains_point (op->src_clip, x, y, &box))
	{
	    (*op[1].set) (&op[1], x, y);
	    bits = (*op[1].fetch) (&op[1]);
	}
	else
	    bits = 0;
	break;
    case PIXMAN_FILTER_GOOD:
    case PIXMAN_FILTER_BEST:
    case PIXMAN_FILTER_BILINEAR:
	rtot = gtot = btot = atot = 0;
	miny = xFixedToInt (v.vector[1]) + op->u.transform.top_y;
	maxy = xFixedToInt (xFixedCeil (v.vector[1])) + op->u.transform.top_y;
	
	minx = xFixedToInt (v.vector[0]) + op->u.transform.left_x;
	maxx = xFixedToInt (xFixedCeil (v.vector[0])) + op->u.transform.left_x;
	
	yerr = xFixed1 - xFixedFrac (v.vector[1]);
	for (y = miny; y <= maxy; y++)
	{
	    uint32_t	lrtot = 0, lgtot = 0, lbtot = 0, latot = 0;
	    int         tx, ty;

	    if (op->u.transform.repeat)
		ty = MOD (y, op->u.transform.height);
	    else
		ty = y;
	    
	    xerr = xFixed1 - xFixedFrac (v.vector[0]);
	    for (x = minx; x <= maxx; x++)
	    {
		if (op->u.transform.repeat)
		    tx = MOD (x, op->u.transform.width);
		else
		    tx = x;

		if (pixman_region_contains_point (op->src_clip, tx, ty, &box))
		{
		    (*op[1].set) (&op[1], tx, ty);
		    bits = (*op[1].fetch) (&op[1]);
		    {
			Splita(bits);
			lrtot += r * xerr;
			lgtot += g * xerr;
			lbtot += b * xerr;
			latot += a * xerr;
		    }
		}
		xerr = xFixed1 - xerr;
	    }
	    rtot += (lrtot >> 10) * yerr;
	    gtot += (lgtot >> 10) * yerr;
	    btot += (lbtot >> 10) * yerr;
	    atot += (latot >> 10) * yerr;
	    yerr = xFixed1 - yerr;
	}
	if ((atot >>= 22) > 0xff) atot = 0xff;
	if ((rtot >>= 22) > 0xff) rtot = 0xff;
	if ((gtot >>= 22) > 0xff) gtot = 0xff;
	if ((btot >>= 22) > 0xff) btot = 0xff;
	bits = ((atot << 24) |
		(rtot << 16) |
		(gtot <<  8) |
		(btot       ));
	break;
    default:
	bits = 0;
	break;
    }
    return bits;
}

static uint32_t
fbFetcha_transform (FbCompositeOperand *op)
{
    pixman_vector_t	v;
    int		x, y;
    int		minx, maxx, miny, maxy;
    uint32_t	rtot, gtot, btot, atot;
    uint32_t	xerr, yerr;
    uint32_t	bits;
    pixman_box16_t	box;

    v.vector[0] = IntToxFixed(op->u.transform.x);
    v.vector[1] = IntToxFixed(op->u.transform.y);
    v.vector[2] = xFixed1;
    if (!pixman_transform_point (op->u.transform.transform, &v))
	return 0;
    switch (op->u.transform.filter) {
    case PIXMAN_FILTER_FAST:
    case PIXMAN_FILTER_NEAREST:
	y = xFixedToInt (v.vector[1]) + op->u.transform.left_x;
	x = xFixedToInt (v.vector[0]) + op->u.transform.top_y;
	if (op->u.transform.repeat)
	{
	    y = MOD (y, op->u.transform.height);
	    x = MOD (x, op->u.transform.width);
	}
	if (pixman_region_contains_point (op->src_clip, x, y, &box))
	{
	    (*op[1].set) (&op[1], x, y);
	    bits = (*op[1].fetcha) (&op[1]);
	}
	else
	    bits = 0;
	break;
    case PIXMAN_FILTER_GOOD:
    case PIXMAN_FILTER_BEST:
    case PIXMAN_FILTER_BILINEAR:
	rtot = gtot = btot = atot = 0;
	
	miny = xFixedToInt (v.vector[1]) + op->u.transform.top_y;
	maxy = xFixedToInt (xFixedCeil (v.vector[1])) + op->u.transform.top_y;
	
	minx = xFixedToInt (v.vector[0]) + op->u.transform.left_x;
	maxx = xFixedToInt (xFixedCeil (v.vector[0])) + op->u.transform.left_x;
	
	yerr = xFixed1 - xFixedFrac (v.vector[1]);
	for (y = miny; y <= maxy; y++)
	{
	    uint32_t	lrtot = 0, lgtot = 0, lbtot = 0, latot = 0;
	    int         tx, ty;

	    if (op->u.transform.repeat)
		ty = MOD (y, op->u.transform.height);
	    else
		ty = y;
	    
	    xerr = xFixed1 - xFixedFrac (v.vector[0]);
	    for (x = minx; x <= maxx; x++)
	    {
		if (op->u.transform.repeat)
		    tx = MOD (x, op->u.transform.width);
		else
		    tx = x;
		
		if (pixman_region_contains_point (op->src_clip, tx, ty, &box))
		{
		    (*op[1].set) (&op[1], tx, ty);
		    bits = (*op[1].fetcha) (&op[1]);
		    {
			Splita(bits);
			lrtot += r * xerr;
			lgtot += g * xerr;
			lbtot += b * xerr;
			latot += a * xerr;
		    }
		}
		x++;
		xerr = xFixed1 - xerr;
	    }
	    rtot += (lrtot >> 10) * yerr;
	    gtot += (lgtot >> 10) * yerr;
	    btot += (lbtot >> 10) * yerr;
	    atot += (latot >> 10) * yerr;
	    y++;
	    yerr = xFixed1 - yerr;
	}
	if ((atot >>= 22) > 0xff) atot = 0xff;
	if ((rtot >>= 22) > 0xff) rtot = 0xff;
	if ((gtot >>= 22) > 0xff) gtot = 0xff;
	if ((btot >>= 22) > 0xff) btot = 0xff;
	bits = ((atot << 24) |
		(rtot << 16) |
		(gtot <<  8) |
		(btot       ));
	break;
    default:
	bits = 0;
	break;
    }
    return bits;
}

static FbAccessMap const fbAccessMap[] = {
    /* 32bpp formats */
    { PICT_a8r8g8b8,	fbFetch_a8r8g8b8,	fbFetch_a8r8g8b8,	fbStore_a8r8g8b8 },
    { PICT_x8r8g8b8,	fbFetch_x8r8g8b8,	fbFetch_x8r8g8b8,	fbStore_x8r8g8b8 },
    { PICT_a8b8g8r8,	fbFetch_a8b8g8r8,	fbFetch_a8b8g8r8,	fbStore_a8b8g8r8 },
    { PICT_x8b8g8r8,	fbFetch_x8b8g8r8,	fbFetch_x8b8g8r8,	fbStore_x8b8g8r8 },

    /* 24bpp formats */
    { PICT_r8g8b8,	fbFetch_r8g8b8,		fbFetch_r8g8b8,		fbStore_r8g8b8 },
    { PICT_b8g8r8,	fbFetch_b8g8r8,		fbFetch_b8g8r8,		fbStore_b8g8r8 },

    /* 16bpp formats */
    { PICT_r5g6b5,	fbFetch_r5g6b5,		fbFetch_r5g6b5,		fbStore_r5g6b5 },
    { PICT_b5g6r5,	fbFetch_b5g6r5,		fbFetch_b5g6r5,		fbStore_b5g6r5 },

    { PICT_a1r5g5b5,	fbFetch_a1r5g5b5,	fbFetch_a1r5g5b5,	fbStore_a1r5g5b5 },
    { PICT_x1r5g5b5,	fbFetch_x1r5g5b5,	fbFetch_x1r5g5b5,	fbStore_x1r5g5b5 },
    { PICT_a1b5g5r5,	fbFetch_a1b5g5r5,	fbFetch_a1b5g5r5,	fbStore_a1b5g5r5 },
    { PICT_x1b5g5r5,	fbFetch_x1b5g5r5,	fbFetch_x1b5g5r5,	fbStore_x1b5g5r5 },
    { PICT_a4r4g4b4,	fbFetch_a4r4g4b4,	fbFetch_a4r4g4b4,	fbStore_a4r4g4b4 },
    { PICT_x4r4g4b4,	fbFetch_x4r4g4b4,	fbFetch_x4r4g4b4,	fbStore_x4r4g4b4 },
    { PICT_a4b4g4r4,	fbFetch_a4b4g4r4,	fbFetch_a4b4g4r4,	fbStore_a4b4g4r4 },
    { PICT_x4b4g4r4,	fbFetch_x4b4g4r4,	fbFetch_x4b4g4r4,	fbStore_x4b4g4r4 },

    /* 8bpp formats */
    { PICT_a8,		fbFetch_a8,		fbFetcha_a8,		fbStore_a8 },
    { PICT_r3g3b2,	fbFetch_r3g3b2,		fbFetch_r3g3b2,		fbStore_r3g3b2 },
    { PICT_b2g3r3,	fbFetch_b2g3r3,		fbFetch_b2g3r3,		fbStore_b2g3r3 },
    { PICT_a2r2g2b2,	fbFetch_a2r2g2b2,	fbFetch_a2r2g2b2,	fbStore_a2r2g2b2 },

    /* 4bpp formats */
    { PICT_a4,		fbFetch_a4,		fbFetcha_a4,		fbStore_a4 },
    { PICT_r1g2b1,	fbFetch_r1g2b1,		fbFetch_r1g2b1,		fbStore_r1g2b1 },
    { PICT_b1g2r1,	fbFetch_b1g2r1,		fbFetch_b1g2r1,		fbStore_b1g2r1 },
    { PICT_a1r1g1b1,	fbFetch_a1r1g1b1,	fbFetch_a1r1g1b1,	fbStore_a1r1g1b1 },
    { PICT_a1b1g1r1,	fbFetch_a1b1g1r1,	fbFetch_a1b1g1r1,	fbStore_a1b1g1r1 },

    /* 1bpp formats */
    { PICT_a1,		fbFetch_a1,		fbFetcha_a1,		fbStore_a1 },
};
#define NumAccessMap (sizeof fbAccessMap / sizeof fbAccessMap[0])

static void
fbStepOver (FbCompositeOperand *op)
{
    op->u.drawable.offset += op->u.drawable.bpp;
}

static void
fbStepDown (FbCompositeOperand *op)
{
    op->u.drawable.line += op->u.drawable.stride;
    op->u.drawable.offset = op->u.drawable.start_offset;
}

static void
fbSet (FbCompositeOperand *op, int x, int y)
{
    op->u.drawable.line = op->u.drawable.top_line + y * op->u.drawable.stride;
    op->u.drawable.offset = op->u.drawable.left_offset + x * op->u.drawable.bpp;
}

static void
fbStepOver_external (FbCompositeOperand *op)
{
    (*op[1].over) (&op[1]);
    (*op[2].over) (&op[2]);
}

static void
fbStepDown_external (FbCompositeOperand *op)
{
    (*op[1].down) (&op[1]);
    (*op[2].down) (&op[2]);
}

static void
fbSet_external (FbCompositeOperand *op, int x, int y)
{
    (*op[1].set) (&op[1], x, y);
    (*op[2].set) (&op[2], 
		  x - op->u.external.alpha_dx,
		  y - op->u.external.alpha_dy);
}

static void
fbStepOver_transform (FbCompositeOperand *op)
{
    op->u.transform.x++;   
}

static void
fbStepDown_transform (FbCompositeOperand *op)
{
    op->u.transform.y++;
    op->u.transform.x = op->u.transform.start_x;
}

static void
fbSet_transform (FbCompositeOperand *op, int x, int y)
{
    op->u.transform.x = x - op->u.transform.left_x;
    op->u.transform.y = y - op->u.transform.top_y;
}


int
fbBuildCompositeOperand (pixman_image_t	    *image,
			 FbCompositeOperand op[4],
			 INT16		    x,
			 INT16		    y,
			 Bool		    transform,
			 Bool		    alpha)
{
    /* Check for transform */
    if (transform && image->transform)
    {
	if (!fbBuildCompositeOperand (image, &op[1], 0, 0, 0, alpha))
	    return 0;
	
	op->u.transform.top_y = image->pixels->y;
	op->u.transform.left_x = image->pixels->x;
	
	op->u.transform.start_x = x - op->u.transform.left_x;
	op->u.transform.x = op->u.transform.start_x;
	op->u.transform.y = y - op->u.transform.top_y;
	op->u.transform.transform = image->transform;
	op->u.transform.filter = image->filter;
	op->u.transform.repeat = image->repeat;
	op->u.transform.width = image->pixels->width;
	op->u.transform.height = image->pixels->height;
	
	op->fetch = fbFetch_transform;
	op->fetcha = fbFetcha_transform;
	op->store = 0;
	op->over = fbStepOver_transform;
	op->down = fbStepDown_transform;
	op->set = fbSet_transform;

	op->src_clip = op[1].src_clip;
	op->dst_clip = op[1].dst_clip;
	
	return 1;
    }
    /* Check for external alpha */
    else if (alpha && image->alphaMap)
    {
	if (!fbBuildCompositeOperand (image, &op[1], x, y, 0, 0))
	    return 0;
	if (!fbBuildCompositeOperand (image->alphaMap, &op[2],
				      x - image->alphaOrigin.x,
				      y - image->alphaOrigin.y,
				      0, 0))
	    return 0;
	op->u.external.alpha_dx = image->alphaOrigin.x;
	op->u.external.alpha_dy = image->alphaOrigin.y;

	op->fetch = fbFetch_external;
	op->fetcha = fbFetcha_external;
	op->store = fbStore_external;
	op->over = fbStepOver_external;
	op->down = fbStepDown_external;
	op->set = fbSet_external;

	op->src_clip = op[1].dst_clip;
	op->dst_clip = op[1].dst_clip;
	
	return 1;
    }
    /* Build simple operand */
    else
    {
	int	    i;
	int	    xoff, yoff;

	for (i = 0; i < NumAccessMap; i++)
	    if (fbAccessMap[i].format_code == image->format_code)
	    {
		FbBits	*bits;
		FbStride	stride;
		int		bpp;

		op->fetch = fbAccessMap[i].fetch;
		op->fetcha = fbAccessMap[i].fetcha;
		op->store = fbAccessMap[i].store;
		op->over = fbStepOver;
		op->down = fbStepDown;
		op->set = fbSet;

		op->dst_clip = image->pCompositeClip;
		if (image->compositeClipSource)
		    op->src_clip = image->pCompositeClip;
		else
		    op->src_clip = image->pSourceClip;

		FbGetPixels (image->pixels, bits, stride, bpp,
			     xoff, yoff);
		if (image->repeat && image->pixels->width == 1 && 
		    image->pixels->height == 1)
		{
		    bpp = 0;
		    stride = 0;
		}
		/*
		 * Coordinates of upper left corner of drawable
		 */
		op->u.drawable.top_line = bits + yoff * stride;
		op->u.drawable.left_offset = xoff * bpp;

		/*
		 * Starting position within drawable
		 */
		op->u.drawable.start_offset = op->u.drawable.left_offset + x * bpp;
		op->u.drawable.line = op->u.drawable.top_line + y * stride;
		op->u.drawable.offset = op->u.drawable.start_offset;

		op->u.drawable.stride = stride;
		op->u.drawable.bpp = bpp;
		return TRUE;
	    }
	return FALSE;
    }
}

void
pixman_compositeGeneral (pixman_operator_t	op,
		    PicturePtr	pSrc,
		    PicturePtr	pMask,
		    PicturePtr	pDst,
		    INT16	xSrc,
		    INT16	ySrc,
		    INT16	xMask,
		    INT16	yMask,
		    INT16	xDst,
		    INT16	yDst,
		    CARD16	width,
		    CARD16	height)
{
    FbCompositeOperand	src[4],msk[4],dst[4],*pmsk;
    FbCompositeOperand	*srcPict, *srcAlpha;
    FbCompositeOperand	*dstPict, *dstAlpha;
    FbCompositeOperand	*mskPict = 0, *mskAlpha = 0;
    FbCombineFunc	f;
    int			w;

    if (!fbBuildCompositeOperand (pSrc, src, xSrc, ySrc, TRUE, TRUE))
	return;
    if (!fbBuildCompositeOperand (pDst, dst, xDst, yDst, FALSE, TRUE))
	return;
    if (pSrc->alphaMap)
    {
	srcPict = &src[1];
	srcAlpha = &src[2];
    }
    else
    {
	srcPict = &src[0];
	srcAlpha = 0;
    }
    if (pDst->alphaMap)
    {
	dstPict = &dst[1];
	dstAlpha = &dst[2];
    }
    else
    {
	dstPict = &dst[0];
	dstAlpha = 0;
    }
    f = fbCombineFuncU[op];
    if (pMask)
    {
	if (!fbBuildCompositeOperand (pMask, msk, xMask, yMask, TRUE, TRUE))
	    return;
	pmsk = msk;
	if (pMask->componentAlpha)
	    f = fbCombineFuncC[op];
	if (pMask->alphaMap)
	{
	    mskPict = &msk[1];
	    mskAlpha = &msk[2];
	}
	else
	{
	    mskPict = &msk[0];
	    mskAlpha = 0;
	}
    }
    else
	pmsk = 0;
    while (height--)
    {
	w = width;
	
	while (w--)
	{
	    (*f) (src, pmsk, dst);
	    (*src->over) (src);
	    (*dst->over) (dst);
	    if (pmsk)
		(*pmsk->over) (pmsk);
	}
	(*src->down) (src);
	(*dst->down) (dst);
	if (pmsk)
	    (*pmsk->down) (pmsk);
    }
}

