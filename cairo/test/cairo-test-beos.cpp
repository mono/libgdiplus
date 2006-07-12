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

// Part of this code was originally part of
// xpfe/bootstrap/nsNativeAppSupportBeOS.cpp in the Mozilla source code.

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Bitmap.h>

extern "C" {
#include "cairo-test.h"
}

#include "cairo-test-beos.h"
#include "cairo-beos.h"

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
	cairo_test_log("Error creating BeOS initialization semaphore\n");
        return;
    }

    thread_id tid = spawn_thread(nsBeOSApp::Main, "Cairo/BeOS test", B_NORMAL_PRIORITY, (void *)initsem);
    if (tid < B_OK || B_OK != resume_thread(tid)) {
	cairo_test_log("Error spawning thread\n");
	return;
    }

    if (B_OK != acquire_sem(initsem)) {
	cairo_test_log("Error acquiring semaphore\n");
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

struct beos_test_closure
{
    BView* view;
    BBitmap* bitmap;
    BWindow* window;
};

// Test a real window
cairo_surface_t *
create_beos_surface (cairo_test_t* test, cairo_content_t content, void **closure)
{
    float right = test->width ? test->width - 1 : 0;
    float bottom = test->height ? test->height - 1 : 0;
    BRect rect(0.0, 0.0, right, bottom);
    CairoTestWindow* wnd = new CairoTestWindow(rect, test->name);

    beos_test_closure* bclosure = new beos_test_closure;
    bclosure->view = wnd->View();
    bclosure->bitmap = NULL;
    bclosure->window = wnd;

    *closure = bclosure;

    return cairo_beos_surface_create(wnd->View());
}

void
cleanup_beos (void* closure)
{
    beos_test_closure* bclosure = reinterpret_cast<beos_test_closure*>(closure);

    bclosure->window->Lock();
    bclosure->window->Quit();

    delete bclosure;
}

// Test a bitmap
cairo_surface_t *
create_beos_bitmap_surface (cairo_test_t* test, cairo_content_t content,
	                    void **closure)
{
    BRect rect(0.0, 0.0, test->width - 1, test->height - 1);
    color_space beosformat = (content == CAIRO_CONTENT_COLOR_ALPHA) ? B_RGBA32
								    : B_RGB32;
    BBitmap* bmp = new BBitmap(rect, beosformat, true);
    BView* view = new BView(rect, "Cairo test view", B_FOLLOW_ALL_SIDES, 0);
    bmp->AddChild(view);

    beos_test_closure* bclosure = new beos_test_closure;
    bclosure->view = view;
    bclosure->bitmap = bmp;
    bclosure->window = NULL;
    *closure = bclosure;

    return cairo_beos_surface_create_for_bitmap(view, bmp);
}

void
cleanup_beos_bitmap (void* closure)
{
    beos_test_closure* bclosure = reinterpret_cast<beos_test_closure*>(closure);

    bclosure->bitmap->RemoveChild(bclosure->view);


    delete bclosure->view;
    delete bclosure->bitmap;

    delete bclosure;
}


