#ifdef WIN32
#ifndef __cplusplus
#error Please compile with a C++ compiler.
#endif
#endif

#if defined(USE_WINDOWS_GDIPLUS)
#include <Windows.h>
#include <GdiPlus.h>

#pragma comment(lib, "gdiplus.lib")
#else
#include <GdiPlusFlat.h>
#endif

#if defined(USE_WINDOWS_GDIPLUS)
using namespace Gdiplus;
using namespace DllExports;
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "testhelpers.h"

static void test_startup ()
{
	GpStatus status;
	ULONG_PTR gdiplusToken = 0;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	GdiplusStartupOutput gdiplusStartupOutput = {NULL, NULL};

	// Version 1 - no output.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	GdiplusShutdown (gdiplusToken);

	// Version 1 - output.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	assert (!gdiplusStartupOutput.NotificationHook && !gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);

	// Version 2 - no output.
	gdiplusStartupInput.GdiplusVersion = 2;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	GdiplusShutdown (gdiplusToken);

	// Version 2 - output.
	gdiplusStartupInput.GdiplusVersion = 2;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	assert (!gdiplusStartupOutput.NotificationHook && !gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);
	gdiplusStartupInput.GdiplusVersion = 1;

	// SuppressBackgroundThread is true.
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	assertEqualInt (status, Ok);
	assert (gdiplusToken);
	assert (gdiplusStartupOutput.NotificationHook && gdiplusStartupOutput.NotificationUnhook);
	GdiplusShutdown (gdiplusToken);
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;

	// Startup multiple times.
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);

	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, Ok);
	GdiplusShutdown (gdiplusToken);

	// Negative tests.
	status = GdiplusStartup (NULL, &gdiplusStartupInput, NULL);
	assertEqualInt (status, InvalidParameter);

	status = GdiplusStartup (&gdiplusToken, NULL, NULL);
	assertEqualInt (status, InvalidParameter);

	gdiplusStartupInput.GdiplusVersion = 0;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, UnsupportedGdiplusVersion);

	gdiplusStartupInput.GdiplusVersion = 3;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, UnsupportedGdiplusVersion);

	gdiplusStartupInput.GdiplusVersion = 1;
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, InvalidParameter);

	gdiplusStartupInput.GdiplusVersion = 3;
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	status = GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);
	assertEqualInt (status, InvalidParameter);
}

static void test_shutdown ()
{
	ULONG_PTR gdiplusToken = 0;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	// Valid token.
	GdiplusShutdown (gdiplusToken);

	// Valid token again.
	GdiplusShutdown (gdiplusToken);

	// Invalid token.
	GdiplusShutdown ((ULONG_PTR) 1);

	// Null token.
	GdiplusShutdown ((ULONG_PTR) 0);
}

static void test_notificationHook ()
{
	GpStatus status;
	ULONG_PTR gdiplusToken = 0;
	ULONG_PTR notificationToken;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartupOutput gdiplusStartupOutput = {NULL, NULL};

	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);

	status = GdiplusNotificationHook (&notificationToken);
	assertEqualInt (status, Ok);
	assert (notificationToken);
	GdiplusNotificationUnhook (notificationToken);

	// Negative tests.
	// This causes GDI+ to crash.
#if !defined(USE_WINDOWS_GDIPLUS)
	status = GdiplusNotificationHook (NULL);
	assertEqualInt (status, InvalidParameter);
#endif

	GdiplusShutdown (gdiplusToken);

	// SupressBackgroundThread is false.
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	status = GdiplusNotificationHook (&notificationToken);
	assertEqualInt (status, GenericError);

	status = GdiplusNotificationHook (NULL);
	assertEqualInt (status, GenericError);

	GdiplusShutdown (gdiplusToken);
}

static void test_notificationUnhook ()
{
	ULONG_PTR gdiplusToken = 0;
	ULONG_PTR notificationToken;
#if defined(USE_WINDOWS_GDIPLUS)
	GdiplusStartupInput gdiplusStartupInput;
#else
	GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
#endif
	gdiplusStartupInput.SuppressBackgroundThread = TRUE;
	GdiplusStartupOutput gdiplusStartupOutput = {NULL, NULL};

	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, &gdiplusStartupOutput);
	GdiplusNotificationHook (&notificationToken);

	// Valid token.
	GdiplusNotificationUnhook (notificationToken);

	// Valid token again.
	GdiplusNotificationUnhook (notificationToken);

	// Invalid token.
	GdiplusNotificationUnhook ((ULONG_PTR) 1);

	// Zero token.
	GdiplusNotificationUnhook ((ULONG_PTR) 0);

	GdiplusShutdown (gdiplusToken);

	// SupressBackgroundThread is false.
	gdiplusStartupInput.SuppressBackgroundThread = FALSE;
	GdiplusStartup (&gdiplusToken, &gdiplusStartupInput, NULL);

	// Valid token.
	GdiplusNotificationUnhook (notificationToken);

	// Valid token again.
	GdiplusNotificationUnhook (notificationToken);

	// Invalid token.
	GdiplusNotificationUnhook ((ULONG_PTR) 1);

	// Zero token.
	GdiplusNotificationUnhook ((ULONG_PTR) 0);

	GdiplusShutdown (gdiplusToken);
}

static void test_alloc()
{
	void *data;

	data = GdipAlloc (0);
	GdipFree (data);

	data = GdipAlloc (1);
	GdipFree (data);
}

static void test_free()
{
	void *data;

	// Valid pointer.
	data = GdipAlloc (1);
	GdipFree (data);

	// Null pointer.
	GdipFree (NULL);
}

int
main (int argc, char**argv)
{
  test_startup ();
	test_shutdown ();
	test_notificationHook ();
	test_notificationUnhook ();
	test_alloc ();
	test_free ();

  return 0;
}
