#include <glib.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "grandpa.h"
#include "ewmh.h"
#include "client.h"

void
gpa_ewmh_init(GrandPa *gpa)
{
	/* Internalize useful atoms. */
	gpa->wm_state =
		XInternAtom(gpa->display, "WM_STATE", False);
	gpa->wm_change_state =
		XInternAtom(gpa->display, "WM_CHANGE_STATE", False);
	gpa->wm_protocols =
		XInternAtom(gpa->display, "WM_PROTOCOLS", False);
	gpa->wm_delete =
		XInternAtom(gpa->display, "WM_DELETE_WINDOW", False);
	gpa->wm_take_focus =
		XInternAtom(gpa->display, "WM_TAKE_FOCUS", False);
	gpa->wm_colormaps =
		XInternAtom(gpa->display, "WM_COLORMAP_WINDOWS", False);
	gpa->compound_text =
		XInternAtom(gpa->display, "COMPOUND_TEXT", False);
	gpa->wm_transient_for =
		XInternAtom(gpa->display, "WM_TRANSIENT_FOR", False);
	gpa->wm_normal_hints = 
		XInternAtom(gpa->display, "WM_NORMAL_HINTS", False);
	gpa->wm_name =
		XInternAtom(gpa->display, "WM_NAME", False);
	gpa->mozilla_url =
		XInternAtom(gpa->display, "_MOZILLA_URL", False);

	/* build half a million EWMH atoms */
	gpa->ewmh_atoms[_NET_SUPPORTED] =
		XInternAtom(gpa->display, "_NET_SUPPORTED", False);
	gpa->ewmh_atoms[_NET_CLIENT_LIST] =
		XInternAtom(gpa->display, "_NET_CLIENT_LIST", False);
	gpa->ewmh_atoms[_NET_CLIENT_LIST_STACKING] =
		XInternAtom(gpa->display, "_NET_CLIENT_LIST_STACKING", False);
	gpa->ewmh_atoms[_NET_NUMBER_OF_DESKTOPS] =
		XInternAtom(gpa->display, "_NET_NUMBER_OF_DESKTOPS", False);
	gpa->ewmh_atoms[_NET_DESKTOP_GEOMETRY] =
		XInternAtom(gpa->display, "_NET_DESKTOP_GEOMETRY", False);
	gpa->ewmh_atoms[_NET_DESKTOP_VIEWPORT] =
		XInternAtom(gpa->display, "_NET_DESKTOP_VIEWPORT", False);
	gpa->ewmh_atoms[_NET_CURRENT_DESKTOP] =
		XInternAtom(gpa->display, "_NET_CURRENT_DESKTOP", False);
	gpa->ewmh_atoms[_NET_ACTIVE_WINDOW] =
		XInternAtom(gpa->display, "_NET_ACTIVE_WINDOW", False);
	gpa->ewmh_atoms[_NET_WORKAREA] =
		XInternAtom(gpa->display, "_NET_WORKAREA", False);
	gpa->ewmh_atoms[_NET_SUPPORTING_WM_CHECK] =
		XInternAtom(gpa->display, "_NET_SUPPORTING_WM_CHECK", False);
	gpa->ewmh_atoms[_NET_CLOSE_WINDOW] =
		XInternAtom(gpa->display, "_NET_CLOSE_WINDOW", False);
	gpa->ewmh_atoms[_NET_MOVERESIZE_WINDOW] =
		XInternAtom(gpa->display, "_NET_MOVERESIZE_WINDOW", False);
	gpa->ewmh_atoms[_NET_WM_MOVERESIZE] =
		XInternAtom(gpa->display, "_NET_WM_MOVERESIZE", False);
	gpa->ewmh_atoms[_NET_WM_NAME] =
		XInternAtom(gpa->display, "_NET_WM_NAME", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE", False);
	gpa->ewmh_atoms[_NET_WM_STATE] =
		XInternAtom(gpa->display, "_NET_WM_STATE", False);
	gpa->ewmh_atoms[_NET_WM_ALLOWED_ACTIONS] =
		XInternAtom(gpa->display, "_NET_WM_ALLOWED_ACTIONS", False);
	gpa->ewmh_atoms[_NET_WM_STRUT] =
		XInternAtom(gpa->display, "_NET_WM_STRUT", False);

	/* Window Type */
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_DESKTOP] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_DESKTOP", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_DOCK] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_DOCK", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_TOOLBAR] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_TOOLBAR", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_MENU] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_MENU", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_UTILITY] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_UTILITY", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_SPLASH] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_SPLASH", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_DIALOG] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
	gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_NORMAL] =
		XInternAtom(gpa->display, "_NET_WM_WINDOW_TYPE_NORMAL", False);

	gpa->ewmh_atoms[_NET_WM_STATE_SKIP_TASKBAR] =
		XInternAtom(gpa->display, "_NET_WM_STATE_SKIP_TASKBAR", False);
	gpa->ewmh_atoms[_NET_WM_STATE_SKIP_PAGER] =
		XInternAtom(gpa->display, "_NET_WM_STATE_SKIP_PAGER", False);
	gpa->ewmh_atoms[_NET_WM_STATE_HIDDEN] =
		XInternAtom(gpa->display, "_NET_WM_STATE_HIDDEN", False);
	gpa->ewmh_atoms[_NET_WM_STATE_FULLSCREEN] =
		XInternAtom(gpa->display, "_NET_WM_STATE_FULLSCREEN", False);
	gpa->ewmh_atoms[_NET_WM_ACTION_MOVE] =
		XInternAtom(gpa->display, "_NET_WM_ACTION_MOVE", False);
	gpa->ewmh_atoms[_NET_WM_ACTION_RESIZE] =
		XInternAtom(gpa->display, "_NET_WM_ACTION_RESIZE", False);
	gpa->ewmh_atoms[_NET_WM_ACTION_FULLSCREEN] =
		XInternAtom(gpa->display, "_NET_WM_ACTION_FULLSCREEN", False);
	gpa->ewmh_atoms[_NET_WM_ACTION_CLOSE] =
		XInternAtom(gpa->display, "_NET_WM_ACTION_CLOSE", False);

	/* Strings */
	gpa->utf8_string = XInternAtom(gpa->display, "UTF8_STRING", False);
}

void
gpa_ewmh_client_list_update_with_screen(GrandPa *gpa, GPaScreen *screen)
{
	Window *client_list = NULL;
	Window *stacked_client_list = NULL;
	GList *list = NULL;
	GPaClient *client;
	gint i, j;
	gint client_count = 0;

	if (screen->clients) {
		for (list = screen->clients; list; list = g_list_next(list)) {
			client = (GPaClient *)list->data;

			if (client->priv_window)
				continue;

			if (client->state == WithdrawnState)
				continue;

			client_count++;
		}

		client_list = (Window *)g_new0(Window, client_count);
		stacked_client_list = (Window *)g_new0(Window, client_count);

		i = 0;
		j = client_count - 1;
		for (list = screen->clients; list; list = g_list_next(list)) {
			client = (GPaClient *)list->data;

			if (client->priv_window)
				continue;

			if (client->state == WithdrawnState)
				continue;

			client_list[i] = client->window;
			stacked_client_list[j] = client->window;

			i++;
			j--;
		}
	}

	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_CLIENT_LIST],
		XA_WINDOW, 32, PropModeReplace,
		(unsigned char*)client_list, client_count);
	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_CLIENT_LIST_STACKING],
		XA_WINDOW, 32, PropModeReplace,
		(unsigned char*)stacked_client_list, client_count);

	if (client_list || stacked_client_list) {
		g_free(client_list);
		g_free(stacked_client_list);
	}
}

EWMHWindowType
gpa_ewmh_get_window_type(GrandPa *gpa, Window w)
{
	Atom rt;
	Atom *type;
	int result;
	int fmt;
	unsigned long n;
	unsigned long extra;
	EWMHWindowType ret;

	result = XGetWindowProperty(gpa->display, w,
		gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE],
		0, 100, False, XA_ATOM, &rt, &fmt, &n, &extra,
		(unsigned char **)&type);

	if (result != Success || type == NULL)
		return WTypeNone;

	ret = WTypeNone;
	for (; n; n--) {
		if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_DESKTOP]) {
			ret = WTypeDesktop;
			break;
		} else if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_DOCK]) {
			ret = WTypeDock;
			break;
		} else if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_TOOLBAR]) {
			ret = WTypeToolbar;
			break;
		} else if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_MENU]) {
			ret = WTypeMenu;
			break;
		} else if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_UTILITY]) {
			ret = WTypeUtility;
			break;
		} else if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_SPLASH]) {
			ret = WTypeSplash;
			break;
		} else if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_DIALOG]) {
			ret = WTypeDialog;
			break;
		} else if (type[n - 1] == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE_NORMAL]) {
			ret = WTypeNormal;
			break;
		}
	}

	XFree(type);

	return ret;
}

void
gpa_ewmh_screen_init(GrandPa *gpa, GPaScreen *screen)
{
	unsigned long data[4];

	DEBUG("Initializing EWMH for screen \"%d\"\n", screen->id);

	/* Window Manager Name */
	XChangeProperty(gpa->display, screen->overlay,
		gpa->ewmh_atoms[_NET_WM_NAME],
		gpa->utf8_string, 8, PropModeReplace,
		"GrandPa", 7);

	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_SUPPORTED],
		XA_ATOM, 32, PropModeReplace,
		(unsigned char *)gpa->ewmh_atoms, EWMH_ATOM_LAST);

	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_SUPPORTING_WM_CHECK],
		XA_WINDOW, 32, PropModeReplace,
		(unsigned char *)&screen->overlay, 1);

	/* Number of desktops */
	data[0] = 1;
	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_NUMBER_OF_DESKTOPS],
		XA_CARDINAL, 32, PropModeReplace,
		(unsigned char*)data, 1);

	/* Geometry */
	data[0] = screen->width;
	data[1] = screen->height;
	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_DESKTOP_GEOMETRY],
		XA_CARDINAL, 32, PropModeReplace,
		(unsigned char*)data, 2);

	/* Workarea */
	data[0] = 0;
	data[1] = 0;
	data[2] = screen->width;
	data[3] = screen->height;
	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_WORKAREA],
		XA_CARDINAL, 32, PropModeReplace,
		(unsigned char*)data, 4);

	/* Viewport */
	data[0] = 0;
	data[1] = 0;
	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_DESKTOP_VIEWPORT],
		XA_CARDINAL, 32, PropModeReplace,
		(unsigned char*)data, 2);

	/* Current Desktop */
	data[0] = 0;
	XChangeProperty(gpa->display, screen->root,
		gpa->ewmh_atoms[_NET_CURRENT_DESKTOP],
		XA_CARDINAL, 32, PropModeReplace,
		(unsigned char*)data, 1);
}

void
gpa_ewmh_state_update(GrandPa *gpa, GPaClient *client)
{
	Atom rt;
	Atom *state;
	int fmt;
	unsigned long n;
	unsigned long extra;
	int i;

	if (!client)
		return;

	i = XGetWindowProperty(gpa->display, client->window,
		gpa->ewmh_atoms[_NET_WM_STATE],
		0, 100, False, XA_ATOM, &rt, &fmt, &n, &extra,
		(unsigned char **)&state);

	if (i != Success || state == NULL)
		return;

	client->wstate.skip_taskbar = FALSE;
	client->wstate.skip_pager = FALSE;
	client->wstate.fullscreen = FALSE;
	client->wstate.above = FALSE;
	client->wstate.below = FALSE;

	for (; n; n--) {
		if (state[n - 1] == gpa->ewmh_atoms[_NET_WM_STATE_SKIP_TASKBAR]) {
			client->wstate.skip_taskbar = TRUE;
		} else if (state[n - 1] == gpa->ewmh_atoms[_NET_WM_STATE_SKIP_PAGER]) {
			client->wstate.skip_pager = TRUE;
		} else if (state[n - 1] == gpa->ewmh_atoms[_NET_WM_STATE_FULLSCREEN]) {
			client->wstate.fullscreen = TRUE;
		} else if (state[n - 1] == gpa->ewmh_atoms[_NET_WM_STATE_ABOVE]) {
			client->wstate.above = TRUE;
		} else if (state[n - 1] == gpa->ewmh_atoms[_NET_WM_STATE_BELOW]) {
			client->wstate.below = TRUE;
		}
	}

	XFree(state);
}

void
gpa_ewmh_state_sync(GrandPa *gpa, GPaClient *client)
{
	int atoms = 0;
	Atom *a = NULL;
	int i = 0;

	if (!client)
		return;

	if (client->state != WithdrawnState) {
		if (client->wstate.skip_taskbar)
			atoms++;

		if (client->wstate.skip_pager)
			atoms++;

		if (client->wstate.fullscreen)
			atoms++;

		if (client->wstate.above)
			atoms++;

		if (client->wstate.below)
			atoms++;

		if (atoms > 0)
			a = (Atom *)g_new0(Atom, atoms);
 
		if (client->wstate.skip_taskbar) {

			a[i] = gpa->ewmh_atoms[_NET_WM_STATE_SKIP_TASKBAR];
			i++;
		}

		if (client->wstate.skip_pager) {
			a[i] = gpa->ewmh_atoms[_NET_WM_STATE_SKIP_PAGER];
			i++;
		}

		if (client->wstate.fullscreen) {
			a[i] = gpa->ewmh_atoms[_NET_WM_STATE_FULLSCREEN];
			i++;
		}

		if (client->wstate.above) {
			a[i] = gpa->ewmh_atoms[_NET_WM_STATE_ABOVE];
			i++;
		}

		if (client->wstate.below) {
			a[i] = gpa->ewmh_atoms[_NET_WM_STATE_BELOW];
			i++;
		}
	}

	XChangeProperty(gpa->display, client->window, gpa->ewmh_atoms[_NET_WM_STATE],
		XA_ATOM, 32, PropModeReplace,
		(unsigned char *)a, atoms);

	if (a)
		XFree(a);
}

void
gpa_ewmh_set_allowed(GrandPa *gpa, GPaClient *client)
{
	Atom action[4];

	action[0] = gpa->ewmh_atoms[_NET_WM_ACTION_MOVE];
	action[1] = gpa->ewmh_atoms[_NET_WM_ACTION_RESIZE];
	action[2] = gpa->ewmh_atoms[_NET_WM_ACTION_FULLSCREEN];
	action[3] = gpa->ewmh_atoms[_NET_WM_ACTION_CLOSE];
	XChangeProperty(gpa->display, client->window, gpa->ewmh_atoms[_NET_WM_ALLOWED_ACTIONS],
		XA_ATOM, 32, PropModeReplace,
		(unsigned char *)action, 4);
}

#define GPA_NEW_STATE(action,current) (action == 0) ? FALSE : ((action == 1) ? TRUE : (current ? FALSE : TRUE))

void
gpa_ewmh_state_trigger(GrandPa *gpa, GPaClient *client, unsigned long action, unsigned long atom)
{
	Atom *a = (Atom *)&atom;

	if (atom == 0)
		return;

	if (*a == gpa->ewmh_atoms[_NET_WM_STATE_SKIP_TASKBAR]) {
		client->wstate.skip_taskbar == GPA_NEW_STATE(action, client->wstate.skip_taskbar);
	} else if (*a == gpa->ewmh_atoms[_NET_WM_STATE_SKIP_PAGER]) {
		client->wstate.skip_pager == GPA_NEW_STATE(action, client->wstate.skip_pager);
	} else if (*a == gpa->ewmh_atoms[_NET_WM_STATE_FULLSCREEN]) {
		client->wstate.fullscreen == GPA_NEW_STATE(action, client->wstate.fullscreen);
	} else if (*a == gpa->ewmh_atoms[_NET_WM_STATE_ABOVE]) {
		client->wstate.above == GPA_NEW_STATE(action, client->wstate.above);
	} else if (*a == gpa->ewmh_atoms[_NET_WM_STATE_BELOW]) {
		client->wstate.below == GPA_NEW_STATE(action, client->wstate.below);
	}

	gpa_ewmh_state_sync(gpa, client);

	gpa_ewmh_client_list_update_with_screen(gpa, client->screen);
}

void
gpa_ewmh_set_active(GrandPa *gpa, GPaClient *client)
{
	if (!client)
		return;

	/* Setting current active window of screen */
	XChangeProperty(gpa->display, client->screen->root,
		gpa->ewmh_atoms[_NET_ACTIVE_WINDOW],
		XA_WINDOW, 32, PropModeReplace,
		(unsigned char *)&client->window, 1);
}

gchar *
gpa_ewmh_get_window_name(GrandPa *gpa, GPaClient *client)
{
	Atom rt;
	gchar *name;
	int fmt;
	unsigned long n;
	unsigned long extra;
	int i;

	i = XGetWindowProperty(gpa->display, client->window,
		gpa->ewmh_atoms[_NET_WM_NAME],
		0, 100, False, gpa->utf8_string, &rt, &fmt, &n, &extra,
		(unsigned char **)&name);

	if (i != Success || name == NULL)
		return NULL;

	return name;
}
