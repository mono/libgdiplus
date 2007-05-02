/* vim:set ts=8 sw=4 noet cin: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Takashi Toyoshima <toyoshim@be-in.org>
 *   Fredrik Holmqvist <thesuckiestemail@yahoo.se>
 *   Christian Biesinger <cbiesinger@web.de>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

// BeOS's C++ compiler does not support varargs in macros
// So, define CAIRO_BOILERPLATE_LOG here
#define CAIRO_BOILERPLATE_LOG cairo_beos_boilerplate_log

extern "C" {
#include "cairo-boilerplate.h"
}
#include "cairo-boilerplate-beos-private.h"

#include <cairo-beos.h>

// Part of this code was originally part of
// xpfe/bootstrap/nsNativeAppSupportBeOS.cpp in the Mozilla source code.

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>

static int cairo_beos_boilerplate_log(const char* format, ...) {
    va_list args;
    int rv;
    va_start(args, format);
    rv = vfprintf(stderr, format, args);
    va_end(args);
    return rv;
}

class CairoTestWindow : public BWindow
{
public:
    CairoTestWindow(BRect frame, const char* title);
    virtual ~CairoTestWindow();
    BView* View() const { return mView; }
private:
    BView* mView;
};

CairoTestWindow::CairoTestWindow(BRect frame, const char* title)
    : BWindow(frame, title, B_TITLED_WINDOW,
	      B_NOT_RESIZABLE|B_NOT_ZOOMABLE)
{
    mView = new BView(frame, "CairoWindowTestView", B_FOLLOW_ALL_SIDES, 0);
    AddChild(mView);
    Show();

    // Make sure the window is actually on screen
    Lock();
    Sync();
    mView->SetViewColor(B_TRANSPARENT_COLOR);
    mView->Sync();
    Unlock();
}

CairoTestWindow::~CairoTestWindow()
{
    RemoveChild(mView);
    delete mView;
}


class nsBeOSApp : public BApplication
{
public:
    nsBeOSApp(sem_id sem) : BApplication(GetAppSig()), init(sem)
    {}

    void ReadyToRun()
    {
        release_sem(init);
    }

    static int32 Main(void *args)
    {
        nsBeOSApp *app = new nsBeOSApp( (sem_id)args );
        if(app == NULL)
            return B_ERROR;
        return app->Run();
    }

private:

    const char *GetAppSig()
    {
        return "application/x-vnd.cairo-test-app";
    }

    sem_id init;
}; //class nsBeOSApp

class AppRunner
{
    public:
	AppRunner();
	~AppRunner();
};

AppRunner::AppRunner()
{
    if (be_app)
	return;

    sem_id initsem = create_sem(0, "Cairo BApplication init");
    if (initsem < B_OK) {
	CAIRO_BOILERPLATE_LOG("Error creating BeOS initialization semaphore\n");
        return;
    }

    thread_id tid = spawn_thread(nsBeOSApp::Main, "Cairo/BeOS test", B_NORMAL_PRIORITY, (void *)initsem);
    if (tid < B_OK || B_OK != resume_thread(tid)) {
	CAIRO_BOILERPLATE_LOG("Error spawning thread\n");
	return;
    }

    if (B_OK != acquire_sem(initsem)) {
	CAIRO_BOILERPLATE_LOG("Error acquiring semaphore\n");
	return;
    }

    delete_sem(initsem);
    return;
}

AppRunner::~AppRunner()
{
    if (be_app) {
	if (be_app->Lock())
	    be_app->Quit();
	delete be_app;
	be_app = NULL;
    }
}

// Make sure that the BApplication is initialized
static AppRunner sAppRunner;

struct beos_boilerplate_closure
{
    BView* view;
    BBitmap* bitmap;
    BWindow* window;
};

// Test a real window
cairo_surface_t *
_cairo_boilerplate_beos_create_surface (const char			 *name,
					cairo_content_t			  content,
					int				  width,
					int				  height,
					cairo_boilerplate_mode_t	  mode,
					void				**closure)
{
    float right = width ? width - 1 : 0;
    float bottom = height ? height - 1 : 0;
    BRect rect(0.0, 0.0, right, bottom);
    CairoTestWindow* wnd = new CairoTestWindow(rect, name);

    beos_boilerplate_closure* bclosure = new beos_boilerplate_closure;
    bclosure->view = wnd->View();
    bclosure->bitmap = NULL;
    bclosure->window = wnd;

    *closure = bclosure;

    return cairo_beos_surface_create(wnd->View());
}

void
_cairo_boilerplate_beos_cleanup (void* closure)
{
    beos_boilerplate_closure* bclosure = reinterpret_cast<beos_boilerplate_closure*>(closure);

    bclosure->window->Lock();
    bclosure->window->Quit();

    delete bclosure;
}

// Test a bitmap
cairo_surface_t *
_cairo_boilerplate_beos_create_surface_for_bitmap (const char			 *name,
						   cairo_content_t		  content,
						   int				  width,
						   int				  height,
						   cairo_boilerplate_mode_t	  mode,
						   void				**closure)
{
    BRect rect(0.0, 0.0, width - 1, height - 1);
    color_space beosformat = (content == CAIRO_CONTENT_COLOR_ALPHA) ? B_RGBA32
								    : B_RGB32;
    BBitmap* bmp = new BBitmap(rect, beosformat, true);
    BView* view = new BView(rect, "Cairo test view", B_FOLLOW_ALL_SIDES, 0);
    bmp->AddChild(view);

    beos_boilerplate_closure* bclosure = new beos_boilerplate_closure;
    bclosure->view = view;
    bclosure->bitmap = bmp;
    bclosure->window = NULL;
    *closure = bclosure;

    return cairo_beos_surface_create_for_bitmap(view, bmp);
}

void
_cairo_boilerplate_beos_cleanup_bitmap (void* closure)
{
    beos_boilerplate_closure* bclosure = reinterpret_cast<beos_boilerplate_closure*>(closure);

    bclosure->bitmap->RemoveChild(bclosure->view);


    delete bclosure->view;
    delete bclosure->bitmap;

    delete bclosure;
}


