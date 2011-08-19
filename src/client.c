#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>

#include "grandpa.h"
#include "client.h"

void
gpa_client_send_configure_notify(GrandPa *gpa, GPaClient *client)
{
	XConfigureEvent ce;

	ce.type = ConfigureNotify;
	ce.event = client->window;
	ce.window = client->window;
	ce.x = client->x;
	ce.y = client->y;
	ce.width = client->width;
	ce.height = client->height;
	ce.border_width = 0;
	ce.above = None;
	ce.override_redirect = client->override_redirect;

	XSendEvent(gpa->display, client->window, False, StructureNotifyMask, (XEvent *) &ce);
}

void
gpa_client_send_xclient_message(GrandPa *gpa, GPaClient *client, Atom a, long data0, long data1)
{
	XEvent ev;

	memset(&ev, 0, sizeof(ev));
	ev.xclient.type = ClientMessage;
	ev.xclient.window = client->window;
	ev.xclient.message_type = a;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = data0;
	ev.xclient.data.l[1] = data1;

	if (!client->priv_window)
		XSendEvent(gpa->display, client->window, False, SubstructureRedirectMask, (XEvent *) &ev);
	else
		XSendEvent(gpa->display, client->window, False, 0L, (XEvent *) &ev);
}

void
gpa_client_set_active(GrandPa *gpa, GPaClient *client, gboolean active)
{
	if (active) {
		if (client->accepts_focus) {
			XSetInputFocus(gpa->display,
				client->window,
				RevertToPointerRoot,
				CurrentTime);

			gpa_client_send_xclient_message(gpa,
				client,
				gpa->wm_protocols,
				gpa->wm_take_focus,
				0L);
		}

		gpa_ewmh_set_active(gpa, client);
	}
}

void
gpa_client_property_update(GrandPa *gpa, GPaClient *client)
{
	XWindowAttributes attr;

	XGrabServer(gpa->display);

	client->type = gpa_ewmh_get_window_type(gpa, client->window);

	/* Update wstate from EWMH */
	gpa_ewmh_state_update(gpa, client);

	/* window is override redirect */
	if (!XGetWindowAttributes(gpa->display, client->window, &attr))
		return;

	if (attr.override_redirect)
		client->override_redirect = TRUE;

	/* get location and size from window */
	client->x = attr.x;
	client->y = attr.y;
	client->width = attr.width;
	client->height = attr.height;

	/* get WM_TRANSIENT_FOR */
	XGetTransientForHint(gpa->display, client->window, &client->trans);

	XUngrabServer(gpa->display);
}

void
gpa_client_set_state(GrandPa *gpa, GPaClient *client, gint state)
{
	long data[2];

	data[0] = (long) state;
	data[1] = (long) None;

	gpa_error_trap_xerrors();

	client->state = state;
	XChangeProperty(gpa->display, client->window, gpa->wm_state, gpa->wm_state, 32,
		PropModeReplace, (unsigned char *) data, 2);

	gpa_error_untrap_xerrors();

	gpa_ewmh_state_sync(gpa, client);
}

GPaClient *
gpa_client_add(GrandPa *gpa, GPaScreen *screen, Window window)
{
	GPaClient *client;

	/* Create a new client */
	client = (GPaClient *)g_new0(GPaClient, 1);
	client->gpa = gpa;
	client->screen = screen;
	client->container = screen->root;
	client->window = window;
	client->state = WithdrawnState;
	client->never_map = TRUE;
	client->accepts_focus = TRUE;

	/* Setting action of EWMH */
	gpa_ewmh_set_allowed(gpa, client);

	/* Setting client property */
	gpa_client_property_update(gpa, client);

	//XSelectInput(gpa->display, client->window, EnterWindowMask | PropertyChangeMask);

	/* Add to client list */
	screen->clients = g_list_append(screen->clients, client);
	screen->client_count++;

	//XReparentWindow(gpa->display, client->window, client->container, client->x, client->y);

	return client;
}

GPaClient *
gpa_client_find_with_window_from_screen(GrandPa *gpa, GPaScreen *screen, Window window)
{
	GList *list = NULL;
	GPaClient *client;

	if (!window)
		return NULL;

	/* Finding client with window from the list of screen */
	for (list = screen->clients; list; list = g_list_next(list)) {
		client = (GPaClient *)list->data;

		if (client->window == window)
			return client;
	}

	return NULL;
}

GPaClient *
gpa_client_find_with_window(GrandPa *gpa, Window w)
{
	GList *node;
	GPaScreen *screen;
	GPaClient *client;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		client = gpa_client_find_with_window_from_screen(gpa, screen, w);
		if (client)
			return client;
	}

	return NULL;
}

void
gpa_client_remove(GrandPa *gpa, GPaClient *client)
{
	GPaScreen *screen = client->screen;

	screen->clients = g_list_remove(screen->clients, (gconstpointer)client);
	screen->client_count--;

	/* update client list of EWMH */
	gpa_ewmh_client_list_update_with_screen(gpa, screen);

	/* add to kill list */
	screen->destroyed_clients = g_list_append(screen->destroyed_clients, client);
}

GPaClient *
gpa_client_remove_with_window(GrandPa *gpa, Window w)
{
	GPaClient *client;

	client = gpa_client_find_with_window(gpa, w);
	if (!client)
		return NULL;

	gpa_client_remove(gpa, client);

	return client;
}

void
gpa_client_update_from_screen(GrandPa *gpa, GPaScreen *screen)
{
	gint i;
	gint win_count;
	Window root_win;
	Window parent_win;
	Window *wins;
	GPaClient *client;

	DEBUG("Scanning window from screen \"%d\"\n", screen->id);

	/* Scanning window on the screen */
	XQueryTree(gpa->display, screen->root, &root_win, &parent_win, &wins, &win_count);
	DEBUG("We got %d window:\n", win_count);
	for (i = 0; i < win_count; ++i) {
		/* make sure it exists on our client list */
		client = gpa_client_find_with_window_from_screen(gpa, screen, wins[i]);
		if (!client) {
			client = gpa_client_add(gpa, screen, wins[i]);

			if (client->window == screen->root || client->window == screen->overlay)
				client->priv_window = TRUE;

			if (gpa_backend_screen_window_check(gpa, client->window))
				client->priv_window = TRUE;

			DEBUG("* new window %ld, priv_window = %d\n", client->window, client->priv_window);
		}
	}

	if (win_count)
		XFree(wins);

	/* update client list of EWMH */
	gpa_ewmh_client_list_update_with_screen(gpa, screen);
}

void
gpa_client_update(GrandPa *gpa)
{
	GList *node;
	GPaScreen *screen;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		gpa_client_update_from_screen(gpa, screen);
	}
}

void
gpa_client_destroy(GrandPa *gpa, GPaClient *client)
{
	GPaScreen *screen = client->screen;

	screen->destroyed_clients = g_list_remove(screen->destroyed_clients, (gconstpointer)client);

	g_free(client);
}

GPaClient *
gpa_client_killer_find_with_window_from_screen(GrandPa *gpa, GPaScreen *screen, Window window)
{
	GList *list = NULL;
	GPaClient *client;

	if (!window)
		return NULL;

	/* Finding client with window from the list of screen */
	for (list = screen->destroyed_clients; list; list = g_list_next(list)) {
		client = (GPaClient *)list->data;

		if (client->window == window)
			return client;
	}

	return NULL;
}

GPaClient *
gpa_client_killer_find_with_window(GrandPa *gpa, Window w)
{
	GList *node;
	GPaScreen *screen;
	GPaClient *client;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		client = gpa_client_killer_find_with_window_from_screen(gpa, screen, w);
		if (client)
			return client;
	}

	return NULL;
}
