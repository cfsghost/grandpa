#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <X11/extensions/Xrandr.h>

#include "grandpa.h"
#include "screenmgr.h"
#include "error.h"
#include "client.h"
#include "eventdisp.h"
#include "backend.h"

extern GrandPa *gpa;

static gboolean gpa_eventdisp_create_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_configure_request(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_configure_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_map_request(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_unmap_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_map_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_destroy_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_property_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_client_message(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_circulate_request(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_focusin(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_focusout(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_buttonpress(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_keypress(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_keyrelease(GrandPa *gpa, XEvent *ev);

static EventDispatcher eventdisp[] = {
	{CreateNotify, gpa_eventdisp_create_notify},
	{ConfigureRequest, gpa_eventdisp_configure_request},
	{ConfigureNotify, gpa_eventdisp_configure_notify},
	{MapRequest, gpa_eventdisp_map_request},
	{MapNotify, gpa_eventdisp_map_notify},
	{UnmapNotify, gpa_eventdisp_unmap_notify},
	{DestroyNotify, gpa_eventdisp_destroy_notify},
	{PropertyNotify, gpa_eventdisp_property_notify},
	{ClientMessage, gpa_eventdisp_client_message},
	{CirculateRequest, gpa_eventdisp_circulate_request},
	{FocusIn, gpa_eventdisp_focusin},
	{FocusOut, gpa_eventdisp_focusout},
	{ButtonPress, gpa_eventdisp_buttonpress},
	{KeyPress, gpa_eventdisp_keypress},
	{KeyRelease, gpa_eventdisp_keyrelease},
};

gboolean
gpa_eventdisp_send(GrandPa *gpa, XEvent *ev)
{
	EventDispatcher *dispatcher;
	gint i;

	for (i = 0; i < G_N_ELEMENTS(eventdisp); ++i) {
		dispatcher = &eventdisp[i];

		if (dispatcher->type == ev->type) {
			/* call function to handle this event */
			return dispatcher->handler(gpa, ev);
		}
	}

	return FALSE;
}

void
gpa_eventdisp_new(GrandPa *gpa, XEvent *ev)
{
	gint i;
	gchar *connection[3] = { "connected", "disconnected", "unknown connection"};

	/* XRandr */
	switch(ev->type - gpa->xrandr_event_base) {
	case RRScreenChangeNotify:
		{
			XRRScreenChangeNotifyEvent *sce = (XRRScreenChangeNotifyEvent *)ev;
			XRRScreenResources *sres;
			XRROutputInfo *oinfo;

			XRRUpdateConfiguration(ev);

			DEBUG("XRandr Event RRScreenChangeNotify\n");
			DEBUG(" window = %d\n root = %d\n size_index = %d\n rotation %d\n",
				(int) sce->window, (int) sce->root,
				sce->size_index,  sce->rotation);
			DEBUG(" Rotation = %x\n", sce->rotation);
			DEBUG(" %d X %d pixels, %d X %d mm\n",
				sce->width, sce->height, sce->mwidth, sce->mheight);
			DEBUG("Display width   %d, height   %d\n",
				DisplayWidth(gpa->display, 0), DisplayHeight(gpa->display, 0));
			DEBUG("Display widthmm %d, heightmm %d\n",
				DisplayWidthMM(gpa->display, 0), DisplayHeightMM(gpa->display, 0));

			sres = XRRGetScreenResources(sce->display, sce->root);
			if (!sres)
				break;

			DEBUG("Output List:\n");
			for (i = 0; i < sres->noutput; ++i) {
				oinfo = XRRGetOutputInfo(sce->display, sres, sres->outputs[i]);
				DEBUG("%s %s\n", oinfo->name, connection[oinfo->connection]);
			}

			break;
		}
	case RRNotify:
			{
				XRRNotifyEvent * ne = (XRRNotifyEvent *)ev;
				DEBUG("XRandr Event RRNotify\n");

				switch(ne->subtype) {
				case RRNotify_OutputChange:
				{
					XRROutputChangeNotifyEvent *oce = (XRROutputChangeNotifyEvent *)ev;
					XRRScreenConfiguration *sc;
					XRRScreenResources *sres;
					XRROutputInfo *oinfo;
					XRRScreenSize *sizes;
					Rotation current_rotation;
					int nsize;
					int i;

					DEBUG("XRandr Event RROutputChangeNotify\n");

					sres = XRRGetScreenResources(oce->display, oce->window);
					sc = XRRGetScreenInfo(oce->display, oce->window);
//					rate = XRRConfigCurrentRate(sc);
					oinfo = XRRGetOutputInfo(oce->display, sres, oce->output);
					DEBUG("%s %s\n", oinfo->name, connection[oinfo->connection]);
					XRRConfigCurrentConfiguration(sc, &current_rotation);
					sizes = XRRConfigSizes(sc, &nsize);
					for (i = 0; i < nsize; i++) {
							DEBUG("%d %dx%d\n", i, sizes[i].width, sizes[i].height);
					}

					break;
				}
				case RRNotify_OutputProperty:
				{
					XRROutputPropertyNotifyEvent *ope = (XRROutputPropertyNotifyEvent *)ev;
					XRRScreenConfiguration *sc;
					XRRScreenResources *sres;
					XRROutputInfo *oinfo;
					XRRScreenSize *sizes;
					short rate;
					Rotation current_rotation;
					int nsize;

#if 1
					sc = XRRGetScreenInfo(ope->display, ope->window);
					rate = XRRConfigCurrentRate(sc);
					oinfo = XRRGetOutputInfo(ope->display, sres, ope->output);
					DEBUG("%s %s\n", oinfo->name, connection[oinfo->connection]);
					XRRConfigCurrentConfiguration(sc, &current_rotation);
					sizes = XRRConfigSizes(sc, &nsize);
					DEBUG("%d %dx%d *\n", i, sizes[0].width, sizes[0].height);

//					XRRSetScreenConfig(ope->display, sc, ope->window, (SizeID)0,
//						current_rotation, CurrentTime);
#endif
/*
					sres = XRRGetScreenResources(ope->display, ope->window);
					XRRSetCrtcConfig(gpa->display, sres, oinfo->crtc, CurrentTime, 0, 0, mode, sres->outputs, sres->noutput);
					oinfo = XRRGetOutputInfo(sres->display, sres, );
*/
					DEBUG("XRandr Event RROutputPropertyNotify\n");
					break;
				}
				case RRNotify_CrtcChange:
					DEBUG("XRandr Event RRCrtcChangeNotify\n");
					break;
				}

				break;
			}
#if 0
	case RROutputChangeNotify:
		{
			XRROutputChangeNotifyEvent *oce = (XRROutputChangeNotifyEvent *)ev;

			DEBUG("XRandr Event RROutputChangeNotify\n");
			break;
		}
#endif
	}
}

static gboolean
gpa_eventdisp_prepare(GSource *source, gint *timeout)
{
	*timeout = -1;

	return XPending(gpa->display);
}

static gboolean
gpa_eventdisp_check(GSource *source)
{
	if (gpa->event_poll_fd.revents & G_IO_IN)
		return XPending(gpa->display);

	return FALSE;
}

static gboolean
gpa_eventdisp_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
	XEvent xevent;
	GPaScreen *screen;
	GPaClient *client;

	while(XPending(gpa->display)) {
		XNextEvent(gpa->display, &xevent);

		/* Regard input window as window manager window */
		screen = (GPaScreen *)gpa_screenmgr_get_screen_with_internal_window(gpa, xevent.xany.window, GA_SCREEN_WINDOW_INPUT_WINDOW);
		if (screen)
			xevent.xany.window = gpa_backend_get_root_window(gpa, screen);

		if (!gpa_eventdisp_send(gpa, &xevent)) {
			client = gpa_client_find_with_window(gpa, xevent.xany.window);
			gpa_backend_handle_event(gpa, &xevent, client);
		}
	}

	return TRUE;
}

static GSourceFuncs gpa_eventdisp_funcs = {
	gpa_eventdisp_prepare,
	gpa_eventdisp_check,
	gpa_eventdisp_dispatch,
	NULL
};

void
gpa_eventdisp_init(GrandPa *gpa)
{
	GSource *source;
	GPollFD *poll_fd = &(gpa->event_poll_fd);

	/* create a source to hook event of X display */
	source = g_source_new(&gpa_eventdisp_funcs, sizeof(GSource));

	poll_fd->fd = ConnectionNumber(gpa->display);
	poll_fd->events = G_IO_IN;

	g_source_add_poll(source, poll_fd);
	g_source_set_can_recurse(source, TRUE);
	g_source_attach(source, NULL);

	/* Initializing backend event handler */
	gpa_backend_event_init(gpa);
}

gboolean
gpa_eventdisp_create_notify(GrandPa *gpa, XEvent *ev)
{
	GPaClient *client;
	XCreateWindowEvent *cwe = &ev->xcreatewindow;
	Window w = ev->xcreatewindow.window;

    if (gpa_screenmgr_screen_window_check(gpa, w))
        return FALSE;

	if (gpa_backend_screen_window_check(gpa, w))
        return FALSE;

	/* update client list */
	gpa_client_update(gpa);

	DEBUG("Detected new window %ld, parent %ld\n", w, cwe->parent);

	client = gpa_client_find_with_window(gpa, cwe->window);
	if (!client)
		return FALSE;

	client->container = cwe->parent;

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	return TRUE;
}

gboolean
gpa_eventdisp_configure_request(GrandPa *gpa, XEvent *ev)
{
	GPaClient *client;
	XWindowChanges xwc;
	XConfigureRequestEvent *cre = &ev->xconfigurerequest;
	int x, y;

	xwc.x = cre->x;
	xwc.y = cre->y;
	xwc.width = cre->width;
	xwc.height = cre->height;
	xwc.border_width = 0;
	xwc.sibling = cre->above;
	xwc.stack_mode = cre->detail;
	cre->value_mask |= CWBorderWidth;

	client = gpa_client_find_with_window(gpa, cre->window);
	if (!client)
		return FALSE;

	if (cre->value_mask & CWStackMode && cre->value_mask & CWSibling) {
		DEBUG("CWStackMode || CWStackMode\n");
	}

	if (client->type == WTypeNormal) { 
		xwc.x = 0;
		xwc.y = 0;
	}

	XConfigureWindow(gpa->display, cre->window, cre->value_mask, &xwc);

	DEBUG("Configuring Window Type: %ld\n", client->type);

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	return TRUE;
}

gboolean
gpa_eventdisp_configure_notify(GrandPa *gpa, XEvent *ev)
{
	XConfigureEvent *ce = &ev->xconfigure;
	GPaClient *client;

	/* Trigger Xrandr extension */
	XRRUpdateConfiguration(ev);

	/* get information of window */
	client = gpa_client_find_with_window(gpa, ce->window);
	if (!client)
		return FALSE;

	if (client->priv_window)
		return FALSE;

	DEBUG("ConfigureNotify window: %d\n", client->window);

	/* Update */
	gpa_client_property_update(gpa, client);

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	return TRUE;
}

gboolean
gpa_eventdisp_map_request(GrandPa *gpa, XEvent *ev)
{
	XMapRequestEvent *mre = &ev->xmaprequest;
	GPaClient *client;
	XWindowAttributes attr;
	long event_mask;
	unsigned int border_width, depth;
	XWMHints *hints;

	client = gpa_client_find_with_window(gpa, mre->window);
	if (!client)
		return FALSE;

	gpa_client_property_update(gpa, client);

	if (client->state == WithdrawnState) {
		client->never_map = FALSE;

		/* Get the hints (see ICCCM section 4.1.2.3) */
		hints = XGetWMHints(gpa->display, client->window);

		/* Details of input hint */
		if (hints && (hints->flags & InputHint))
			client->accepts_focus = hints->input;

		/* Setting event mask */
		XGetWindowAttributes(gpa->display, mre->window, &attr);
		event_mask = attr.your_event_mask;
		event_mask |= StructureNotifyMask;

		XSelectInput(gpa->display, mre->window, event_mask);

		/* Get size and position, we can notify some GTK+ windows will request
		 * 200x200 size of window by default, then add components to such window.
		 * GTK+ always figure real size and set size before MapRequest rather
		 * than RequestConfigure.
		 */
		XGetGeometry(gpa->display, mre->window, &client->container,
			&client->x, &client->y, &client->width, &client->height,
			&border_width, &depth);

		/* Figure size and position for difference window type and status */
		if (client->wstate.fullscreen) {
			client->x = 0;
			client->y = 0;
			client->width = client->screen->width;
			client->height = client->screen->height;
		} else if (client->type == WTypeDialog) { 
			/* figure center position */
			client->x = (int)((client->screen->width - client->width) * 0.5);
			client->y = (int)((client->screen->height - client->height) * 0.5);
		} else if (client->type == WTypeNormal || client->type == WTypeNone) { 
			/* General window to be maximum */
			client->x = client->screen->avail_x;
			client->y = client->screen->avail_y;
			client->width = client->screen->avail_width;
			client->height = client->screen->avail_height;
		} else if (client->override_redirect || client->trans != None) {
			/* Keep original position and size */
		} else {
			client->width = client->screen->width;
			client->height = client->screen->height;
		}

		/* Move and resize window */
		XMoveResizeWindow(gpa->display, client->window,
			client->x, client->y,
			client->width, client->height);

		XReparentWindow(gpa->display, client->window, client->container, client->x, client->y);
		XAddToSaveSet(gpa->display, client->window);

		/* get window name which is utf8 string */
		client->name = (gchar *)gpa_ewmh_get_window_name(gpa, client);

		/* FIXME: if there is no utf8 window name, need to get 
		 * _mozilla_url and XA_WM_NAME property
		 */
	}

	XMapWindow(gpa->display, client->container);
	XMapWindow(gpa->display, client->window);

	gpa_client_raise(gpa, client);
	gpa_client_set_state(gpa, client, NormalState);
	gpa_client_set_focus(gpa, client, TRUE);

	DEBUG("MapRequest \"%s\"\n", client->name);
	DEBUG("MapRequest window %ld, size %ldx%ld on %ldx%ld\n", client->window, client->width, client->height, client->x, client->y);

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	/* update client list */
	gpa_client_update(gpa);

	return TRUE;
}

gboolean
gpa_eventdisp_unmap_notify(GrandPa *gpa, XEvent *ev)
{
	XUnmapEvent *xue = &ev->xunmap;
	GPaClient *client;

	client = gpa_client_find_with_window(gpa, xue->window);
	if (!client)
		return FALSE;

	DEBUG("Unmapping window id: %ld\n", xue->window);

	if (client->state == NormalState) {
//		XGrabServer(gpa->display);
		XUnmapWindow(gpa->display, client->window);
		gpa_client_set_state(gpa, client, WithdrawnState);
//		XUngrabServer(gpa->display);

		/* This client is current active client */
		if (client->screen->current_client) {
			client->screen->current_client = NULL;
			if (client->screen->last_client) {
				DEBUG("Switch back to last client\n");
				gpa_client_set_focus(gpa, client->screen->last_client, TRUE);
			} else {
				client->screen->current_client = NULL;
				gpa_client_set_focus(gpa, client, FALSE);

				/* TODO: Switch to top client */
			}
		}
	}

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	return TRUE;
}

gboolean
gpa_eventdisp_map_notify(GrandPa *gpa, XEvent *ev)
{
	XMapEvent *xme = &ev->xmap;
	GPaClient *client;

	client = gpa_client_find_with_window(gpa, xme->window);
	if (!client)
		return FALSE;

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	return TRUE;
}

gboolean
gpa_eventdisp_destroy_notify(GrandPa *gpa, XEvent *ev)
{
	XDestroyWindowEvent *de = &ev->xdestroywindow;
	GPaClient *client;

	DEBUG("DestroyNotify window id: %ld\n", de->window);

	client = gpa_client_remove_with_window(gpa, de->window);
	if (!client)
		return FALSE;

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	/* TODO: release client structure */
	gpa_client_destroy(gpa, client);

	return TRUE;
}

gboolean
gpa_eventdisp_property_notify(GrandPa *gpa, XEvent *ev)
{
	GPaClient *client;
	XPropertyEvent *pe = &ev->xproperty;

	DEBUG("PropertyNotify window id: %ld\n", pe->window);

	client = gpa_client_find_with_window(gpa, pe->window);
	if (!client)
		return FALSE;

	if (client->override_redirect) {
		DEBUG("PropertyNotify Override Redirect window\n");
		return TRUE;
	}

	if (pe->atom == gpa->wm_name || pe->atom == gpa->mozilla_url) {
		DEBUG("PropertyNotify WM_NAME\n");
	} else if (pe->atom == gpa->wm_transient_for) {
		DEBUG("PropertyNotify WM_CHANGE_STATE\n");
		/* get WM_TRANSIENT_FOR */
		XGetTransientForHint(gpa->display, client->window, &client->trans);
		return TRUE;
	} else if (pe->atom == gpa->wm_normal_hints) {
		DEBUG("PropertyNotify XA_WM_NORMAL_HINTS\n");
	} else if (pe->atom == gpa->ewmh_atoms[_NET_WM_WINDOW_TYPE]) {
		DEBUG("PropertyNotify _NET_WM_WINDOW_TYPE\n");
	} else if (pe->atom == gpa->ewmh_atoms[_NET_WM_STRUT]) {
		DEBUG("PropertyNotify _NET_WM_STRUT\n");
	}

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	return TRUE;
}

gboolean
gpa_eventdisp_client_message(GrandPa *gpa, XEvent *ev)
{
	XClientMessageEvent *cme = &ev->xclient;
	GPaClient *client;

	DEBUG("ClientMessage window id: %ld\n", cme->window);

	client = gpa_client_find_with_window(gpa, cme->window);
	if (!client)
		return FALSE;

	if (cme->format == 32) {
		if (cme->message_type == gpa->wm_change_state) {
			DEBUG("ClientMessage WM_CHANGE_STATE\n");

			return TRUE;
		} else if (cme->message_type == gpa->ewmh_atoms[_NET_CLOSE_WINDOW]) {
			DEBUG("ClientMessage _NET_CLOSE_WINDOW\n");

			return TRUE;
		} else if (cme->message_type == gpa->ewmh_atoms[_NET_WM_STATE]) {
			DEBUG("ClientMessage _NET_WM_STATE\n");
			gpa_ewmh_state_trigger(gpa, client, cme->data.l[0], cme->data.l[1]);
			gpa_ewmh_state_trigger(gpa, client, cme->data.l[0], cme->data.l[2]);

			return TRUE;
		} else if (cme->message_type == gpa->ewmh_atoms[_NET_MOVERESIZE_WINDOW]) {
			XEvent nev;

			DEBUG("ClientMessage _NET_MOVERESIZE_WINDOW\n");

			nev.xconfigurerequest.window = cme->window;
			nev.xconfigurerequest.x = cme->data.l[1];
			nev.xconfigurerequest.y = cme->data.l[2];
			nev.xconfigurerequest.width = cme->data.l[3];
			nev.xconfigurerequest.height = cme->data.l[4];
			nev.xconfigurerequest.value_mask = 0;
			if (cme->data.l[0] & (1 << 8))
				nev.xconfigurerequest.value_mask |= CWX;
			if (cme->data.l[0] & (1 << 9))
				nev.xconfigurerequest.value_mask |= CWY;
			if (cme->data.l[0] & (1 << 10))
				nev.xconfigurerequest.value_mask |= CWWidth;
			if (cme->data.l[0] & (1 << 11))
				nev.xconfigurerequest.value_mask |= CWHeight;

			gpa_eventdisp_configure_request(gpa, &nev);

			return TRUE;
		} else if (cme->message_type == gpa->ewmh_atoms[_NET_RESTACK_WINDOW]) {
			XWindowChanges xwc;

			DEBUG("ClientMessage _NET_RESTACK_WINDOW\n");

			xwc.sibling = cme->data.l[1];
			xwc.stack_mode = cme->data.l[2];

			XConfigureWindow(gpa->display, cme->window, CWSibling | CWStackMode, &xwc);

			return TRUE;
		} else if (cme->message_type == gpa->ewmh_atoms[_NET_ACTIVE_WINDOW]) {
			DEBUG("ClientMessage _NET_ACTIVE_WINDOW\n");

//			XMapRaised(gpa->display, client->window);
			XMapWindow(gpa->display, client->window);
			gpa_client_set_focus(gpa, client, TRUE);
			gpa_client_raise(gpa, client);

			/* update stack */
			gpa_ewmh_client_list_update_with_screen(gpa, client->screen);

			return TRUE;
		}
	}

	/* event handler of backend */
	gpa_backend_handle_event(gpa, ev, client);

	return TRUE;
}

gboolean
gpa_eventdisp_circulate_request(GrandPa *gpa, XEvent *ev)
{
	DEBUG("Circulate Request\n");

	return TRUE;
}

gboolean
gpa_eventdisp_focusin(GrandPa *gpa, XEvent *ev)
{
	XFocusChangeEvent *fce = &ev->xfocus;
	GPaClient *client;
	GPaScreen *screen;
	Window focus_window;
	int revert;

	DEBUG("Focus In - ID: %d\n", fce->window);

	XGetInputFocus(gpa->display, &focus_window, &revert);
	if (focus_window == None)
		return TRUE;

	if (focus_window == PointerRoot || focus_window == fce->window) {
		screen = gpa_screenmgr_get_screen_with_internal_window(gpa, fce->window, GA_SCREEN_WINDOW_ROOT_WINDOW);

		/* Stay in last client */
		if (screen) {
			DEBUG("Switch back to last client %d\n", screen->last_client);
			gpa_client_set_focus(gpa, screen->last_client, TRUE);

			/* TODO: Switch to top client if last_client is NULL */
		}

		return TRUE;
	}

	client = gpa_client_find_with_window(gpa, focus_window);
	if (!client)
		return FALSE;

	gpa_client_set_focus(gpa, client, TRUE);
	gpa_client_raise(gpa, client);

	return TRUE;
}

gboolean
gpa_eventdisp_focusout(GrandPa *gpa, XEvent *ev)
{
	XFocusChangeEvent *fce = &ev->xfocus;

	DEBUG("Focus Out - ID: %d\n", fce->window);

	return TRUE;
}

gboolean
gpa_eventdisp_buttonpress(GrandPa *gpa, XEvent *ev)
{
	DEBUG("Button Press\n");

	return TRUE;
}

static void
gpa_eventdisp_key_doubleclick(GrandPa *gpa, XEvent *ev)
{
	XKeyEvent *xke = &ev->xkey;
	KeySym key;

	DEBUG("keyDoubleClick: %d\n", xke->keycode);

	key = XKeycodeToKeysym(gpa->display, xke->keycode, 0);
	switch(key) {
	case XK_Super_L:
		gpa->mode = GPA_MODE_CONTROLBAR;
		gpa_backend_call(gpa, GPA_FUNC_CALL_CONTROLBAR_ENABLE, NULL);
		gpa->key_state = 0;
		break;
	}
}

gboolean
gpa_eventdisp_keypress(GrandPa *gpa, XEvent *ev)
{
	XKeyEvent *xke = &ev->xkey;
	KeySym key;
	gboolean ret = TRUE;

	key = XKeycodeToKeysym(gpa->display, xke->keycode, 0);
	switch(key) {
	case XK_Super_L:
		DEBUG("keyPress: %d\n", xke->keycode);

		/* Check double-click behavior */
		if (gpa->keycode == xke->keycode) {
			if (xke->time - gpa->key_time <= 500)
				gpa->key_state |= GPA_KEY_STATE_DOUBLE_CLICK;
		}

		ret = TRUE;
		break;
	}

	if (ret) {
		gpa->key_time = xke->time;
		gpa->keycode = xke->keycode;
	}

	return ret;
}

gboolean
gpa_eventdisp_keyrelease(GrandPa *gpa, XEvent *ev)
{
	XKeyEvent *xke = &ev->xkey;
	KeySym key;

	key = XKeycodeToKeysym(gpa->display, xke->keycode, 0);
	switch(key) {
	case XK_F2:
		/* Super + F2 */
		if (xke->state & Mod4Mask) {
			DEBUG("Open Terminal %d %d\n");

			/* Open Terminal */
			g_spawn_command_line_async("/usr/bin/xterm", NULL);

			return TRUE;
		}

		break;
	case XK_Super_L:
		DEBUG("KeyRelease: %d\n", xke->keycode);
		/* Cancel something */
		if (gpa->mode == GPA_MODE_CONTROLBAR) {
			gpa->mode = GPA_MODE_NORMAL;
			gpa_backend_call(gpa, GPA_FUNC_CALL_CONTROLBAR_DISABLE, NULL);
		}

		/* Double click */
		if ((gpa->keycode == xke->keycode) && (gpa->key_state & GPA_KEY_STATE_DOUBLE_CLICK)) {
			gpa_eventdisp_key_doubleclick(gpa, ev);
		}

		return TRUE;
	}

	return FALSE;
}
