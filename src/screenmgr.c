#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/Xrandr.h>

#include "grandpa.h"
#include "screenmgr.h"

GPaScreen *
gpa_screenmgr_get_screen_with_window(GrandPa *gpa, Window w)
{
	GList *node;
	GPaScreen *screen;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		if (gpa_client_find_with_window_from_screen(gpa, screen, w))
			return screen;
	}

	return NULL;
}

GPaScreen *
gpa_screenmgr_get_screen_with_internal_window(GrandPa *gpa, Window w, GaScreenWindowType type)
{
	GList *node;
	GPaScreen *screen;

	switch(type) {
	case GA_SCREEN_WINDOW_ROOT_WINDOW:
		for (node = gpa->screen->screens; node; node = g_list_next(node)) {
			screen = (GPaScreen *)node->data;

			if (screen->root == w)
				return screen;
		}
		break;
	case GA_SCREEN_WINDOW_OVERLAY_WINDOW:
		for (node = gpa->screen->screens; node; node = g_list_next(node)) {
			screen = (GPaScreen *)node->data;

			if (screen->overlay == w)
				return screen;
		}
		break;
	case GA_SCREEN_WINDOW_INPUT_WINDOW:
		for (node = gpa->screen->screens; node; node = g_list_next(node)) {
			screen = (GPaScreen *)node->data;

			if (screen->input_win == w)
				return screen;
		}
		break;
	}

	return NULL;
}

gboolean
gpa_screenmgr_screen_window_check(GrandPa *gpa, Window w)
{
	GList *node;
	GPaScreen *screen;

	if (gpa_backend_screen_window_check(gpa, w))
		return TRUE;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		if (screen->root == w || screen->overlay == w || screen->input_win == w)
			return TRUE;
	}

	return FALSE;
}

void
gpa_screenmgr_screen_grabkey_init(GrandPa *gpa, GPaScreen *screen)
{
	KeyCode keycode;

	/* Release any previous grabs */
	XUngrabKey(gpa->display, AnyKey, AnyModifier, screen->root);

	/* Grab Keyboard */
	keycode = XKeysymToKeycode(gpa->display, XStringToKeysym("Super_L"));
	XGrabKey(gpa->display, keycode, AnyModifier, screen->root,
		False, GrabModeAsync, GrabModeAsync);
}

inline void
gpa_screenmgr_screen_ungrabkey(GrandPa *gpa, GPaScreen *screen, KeyCode keycode)
{
	XUngrabKey(gpa->display, keycode, AnyModifier, screen->root);
}

inline void
gpa_screenmgr_screen_grabkey(GrandPa *gpa, GPaScreen *screen, KeySym keysym)
{
	KeyCode keycode = XKeysymToKeycode(gpa->display, keysym);

	XGrabKey(gpa->display, keycode, AnyModifier, screen->root,
		False, GrabModeAsync, GrabModeAsync);
}

void
gpa_screenmgr_screen_ungrabkey_all(GrandPa *gpa, KeySym *code_table)
{
	GList *node;
	GPaScreen *screen;
	KeySym *key;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		for (key = (KeySym *)code_table; *key != 0; ++key) {
			gpa_screenmgr_screen_ungrabkey(gpa, screen, *key);
		}
	}
}

void
gpa_screenmgr_screen_grabkey_all(GrandPa *gpa, KeyCode *code_table)
{
	GList *node;
	GPaScreen *screen;
	KeyCode *key;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		for (key = (KeyCode *)code_table; *key != 0; ++key) {
			gpa_screenmgr_screen_grabkey(gpa, screen, *key);
		}
	}
}

void
gpa_screenmgr_screen_input_configure(GrandPa *gpa, GPaScreen *screen)
{
	XWindowAttributes attr;

	XGetWindowAttributes(gpa->display, screen->root, &attr);

	/* Create window for input */
	screen->input_win = XCreateWindow(gpa->display, screen->root,
		0, 0,
		attr.width, attr.height,
		0, 0,
		InputOnly, DefaultVisual(gpa->display, 0), 0, NULL);

	XSelectInput(gpa->display, screen->input_win,
		StructureNotifyMask | FocusChangeMask | PointerMotionMask |
		KeyPressMask | KeyReleaseMask | ButtonPressMask |
		ButtonReleaseMask | PropertyChangeMask);

	XMapWindow(gpa->display, screen->input_win);
	XSetInputFocus(gpa->display, screen->input_win, RevertToPointerRoot, CurrentTime);

	/* INitializing Grab Input */
	gpa_screenmgr_screen_grabkey_init(gpa, screen);
}

void
gpa_screenmgr_screen_configure(GrandPa *gpa, GPaScreen *screen)
{
	XSetWindowAttributes attr;

	DEBUG("Configuring screen\n");

	/* Setting cursor */
	gpa_cursor_init(gpa, screen);
	attr.cursor = screen->root_cursor;
	XChangeWindowAttributes(gpa->display, screen->root, CWCursor, &attr);

	/* Initiaiizing XComposite */
	XCompositeRedirectSubwindows(gpa->display, screen->root, CompositeRedirectAutomatic);

	/* Tell root window we wanna be window manager */
	attr.event_mask = SubstructureRedirectMask |
		SubstructureNotifyMask |
		StructureNotifyMask |
		PropertyChangeMask;

	XChangeWindowAttributes(gpa->display, screen->root, CWEventMask, &attr);

	XSelectInput(gpa->display, screen->root, attr.event_mask);

	/* Listen to XRandr event */
	XRRSelectInput(gpa->display, screen->root,
		RRCrtcChangeNotifyMask | RRScreenChangeNotifyMask |
		RROutputChangeNotifyMask | RROutputPropertyNotifyMask);

	XSync(gpa->display, FALSE);

	screen->overlay = XCompositeGetOverlayWindow(gpa->display, screen->root);

	XMapWindow(gpa->display, screen->overlay);
	DEBUG("Screen root window id: %ld\n", screen->root);
	DEBUG("Screen overlay window id: %ld\n", screen->overlay);

	/* Initializing Input */
	gpa_screenmgr_screen_input_configure(gpa, screen);

	/* Initializing window */
	gpa_backend_screen_init(gpa, screen);
	gpa_ewmh_screen_init(gpa, screen);
}

GPaScreen *
gpa_screenmgr_screen_new(GPaScreenManager *mgr, Display *display, gint num)
{
	GPaScreen *screen;

	DEBUG("Getting %d screen informations\n", num);
	screen = (GPaScreen *)g_new0(GPaScreen, 1);
	screen->id = num;
	screen->root = RootWindow(display, num);
	screen->width = DisplayWidth(display, num);
	screen->height = DisplayHeight(display, num);

	mgr->screens = g_list_append(mgr->screens, screen);

	return screen;
}

void
gpa_screenmgr_init(GrandPa *gpa)
{
	GPaScreenManager *mgr;
	GPaScreen *screen;
	gint i;

	DEBUG("Initializing Screens\n");
	mgr = (GPaScreenManager *)g_new0(GPaScreenManager, 1);
	gpa->screen = mgr;

	/* Find out how many screens we've got. */
	mgr->screen_count = ScreenCount(gpa->display);
	
	/* Initializing each screen. */
	for (i = 0; i < mgr->screen_count; ++i) {
		screen = gpa_screenmgr_screen_new(mgr, gpa->display, i);
		gpa_screenmgr_screen_configure(gpa, screen);
		gpa_client_update_from_screen(gpa, screen);
	}

	DEBUG("Screens was initialized\n\n");
}
