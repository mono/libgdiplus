/*
Test were run with the following script
target can be directfb_bitmap or directfb

export CAIRO_TEST_TARGET=directfb_bitmap
export DFBARGS=quiet,no-banner,no-debug,log-file=dfblog,system=x11
cd cairo/test
make check

*/

#include <stdio.h>
#include <stdlib.h>
#include "cairo-test.h"
#include "cairo-test-directfb.h"
#include <directfb.h>
#include "cairo-directfb.h"

/* macro for a safe call to DirectFB functions */
#define DFBCHECK(x...) \
{                                                                \
	err = x;                                                    \
	if (err != DFB_OK) {                                        \
		fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
		DirectFBErrorFatal( #x, err );                         \
	}                                                           \
}

typedef struct _DFBInfo {
	IDirectFB              *dfb;
	IDirectFBDisplayLayer  *layer;
	IDirectFBWindow        *window;
	IDirectFBSurface       *surface;
} DFBInfo ;

static DFBInfo *init(void) {
	DFBDisplayLayerConfig        layer_config;
	DFBGraphicsDeviceDescription desc;
	int err;
	DFBInfo *info = malloc(sizeof(DFBInfo));
	if( !info )
		return NULL;

	DFBCHECK(DirectFBInit( NULL,NULL));
	DFBCHECK(DirectFBCreate( &info->dfb ));
	info->dfb->GetDeviceDescription(info->dfb, &desc );

	DFBCHECK(info->dfb->GetDisplayLayer( info->dfb, DLID_PRIMARY, &info->layer ));
	info->layer->SetCooperativeLevel( info->layer, DLSCL_ADMINISTRATIVE );

	if (!((desc.blitting_flags & DSBLIT_BLEND_ALPHACHANNEL) &&
				(desc.blitting_flags & DSBLIT_BLEND_COLORALPHA  )))
	{
		layer_config.flags = DLCONF_BUFFERMODE;
		layer_config.buffermode = DLBM_BACKSYSTEM;
		info->layer->SetConfiguration( info->layer, &layer_config );
	}
	return info;
}

cairo_surface_t *
create_directfb_surface (cairo_test_t* test, cairo_format_t format,
                     void **closure) {
	DFBWindowDescription desc;
	int err;
	DFBInfo *info = init();
	if( !info )
		return NULL;
	*closure = info;

	desc.flags  = ( DWDESC_POSX | DWDESC_POSY |
			DWDESC_WIDTH | DWDESC_HEIGHT | DWDESC_CAPS|DSDESC_PIXELFORMAT );
	desc.posx   = 0;
	desc.posy   = 0;
	desc.width  = test->width;
	desc.height = test->height;
	desc.caps =  DWCAPS_DOUBLEBUFFER;
	desc.caps |= DWCAPS_ALPHACHANNEL;
	desc.pixelformat = DSPF_ARGB;

	DFBCHECK(info->layer->CreateWindow( info->layer, &desc, &info->window ) );
	info->window->SetOpacity( info->window, 0xFF );
	info->window->GetSurface( info->window, &info->surface );
	info->surface->SetColor( info->surface, 0xFF, 0xFF, 0xFF, 0xFF );
	info->surface->FillRectangle( info->surface,0, 0, desc.width, desc.height );
	info->surface->Flip( info->surface, NULL, 0 );
	cairo_surface_t *csurface=cairo_directfb_surface_create(info->dfb,info->surface);
	return csurface;
}

cairo_surface_t *
create_directfb_bitmap_surface (cairo_test_t* test, cairo_format_t format,
                            void **closure) {
	int  err;
	DFBInfo *info = init();
	if( !info ) return NULL;
	*closure = info;
	info->window= NULL; /* make sure window is null*/
	DFBSurfaceDescription  desc;

	desc.flags = DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT;
	desc.caps = DSCAPS_NONE;
	desc.width  = test->width;
	desc.height = test->height;
	desc.pixelformat = DSPF_ARGB;
	DFBCHECK(info->dfb->CreateSurface (info->dfb, &desc,&info->surface));
	cairo_surface_t *csurface=cairo_directfb_surface_create(info->dfb,
		info->surface);
	return csurface;
}

void
cleanup_directfb (void* closure) {
	DFBInfo *info = (DFBInfo *)closure;
	if( info->surface )
		info->surface->Release( info->surface );
	if( info->window )
		info->window->Release( info->window );
	if( info->layer )
		info->layer->Release( info->layer );
	if( info->dfb )
		info->dfb->Release( info->dfb );
}
