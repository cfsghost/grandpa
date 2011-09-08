#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <X11/extensions/Xrandr.h>

#include "grandpa.h"
#include "error.h"
#include "client.h"
#include "eventdisp.h"
#include "backend.h"

static gboolean gpa_eventdisp_create_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_configure_request(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_configure_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_map_request(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_unmap_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_destroy_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_property_notify(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_client_message(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_keypress(GrandPa *gpa, XEvent *ev);
static gboolean gpa_eventdisp_keyrelease(GrandPa *gpa, XEvent *ev);

static EventDispatcher eventdisp[] = {
	{CreateNotify, gpa_eventdisp_create_notify},
	{ConfigureRequest, gpa_eventdisp_configure_request},
	{ConfigureNotify, gpa_eventdisp_configure_notify},
	{MapRequest, gpa_eventdisp_map_request},
	{UnmapNotify, gpa_eventdisp_unmap_notify},
	{DestroyNotify, gpa_eventdisp_destroy_notify},
	{PropertyNotify, gpa_eventdisp_property_notify},
	{ClientMessage, gpa_eventdisp_client_message},
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

	if (client->type == WTypeNormal) { 
		xwc.x = 0;
		xwc.y = 0;
	}

	XConfigureWindow(gpa->display, cre->window, cre->value_mask, &xwc);

	DEBUG("Configuring Window Type: %ld\n", client->type);

	return TRUE;
}

gboolean
gpa_eventdisp_configure_notify(GrandPa *gpa, XEvent *ev)
{
	XRRUpdateConfiguration(ev);

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
			client->x = 0;
			client->y = 0;
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

	gpa_client_set_state(gpa, client, NormalState);
	gpa_client_set_active(gpa, client, TRUE);

	DEBUG("MapRequest \"%s\"\n", client->name);
	DEBUG("MapRequest window %ld, size %ldx%ld on %ldx%ld\n", client->window, client->width, client->height, client->x, client->y);

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


	if (client->state == NormalState) {
		XGrabServer(gpa->display);
		gpa_error_trap_xerrors();

		gpa_client_set_state(gpa, client, WithdrawnState);

		gpa_error_untrap_xerrors();
		XUngrabServer(gpa->display);
	}

	return TRUE;
}

gboolean
gpa_eventdisp_destroy_notify(GrandPa *gpa, XEvent *ev)
{
	XDestroyWindowEvent *de = &ev->xdestroywindow;

	gpa_client_remove_with_window(gpa, de->window);

	DEBUG("DestroyNotify window id: %ld\n", de->window);

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

	if (client->override_redirect)
		return TRUE;

	if (pe->atom == gpa->wm_transient_for) {
		DEBUG("PropertyNotify WM_CHANGE_STATE\n");
		/* get WM_TRANSIENT_FOR */
		XGetTransientForHint(gpa->display, client->window, &client->trans);
		return TRUE;
	}

	client->type = gpa_ewmh_get_window_type(gpa, pe->window);

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
		}

		if (cme->message_type == gpa->ewmh_atoms[_NET_CLOSE_WINDOW]) {
			DEBUG("ClientMessage _NET_CLOSE_WINDOW\n");

			return TRUE;
		}
		
		if (cme->message_type == gpa->ewmh_atoms[_NET_WM_STATE]) {
			DEBUG("ClientMessage _NET_WM_STATE\n");
			gpa_ewmh_state_trigger(gpa, client, cme->data.l[0], cme->data.l[1]);
			gpa_ewmh_state_trigger(gpa, client, cme->data.l[0], cme->data.l[2]);

			return TRUE;
		}

		if (cme->message_type == gpa->ewmh_atoms[_NET_MOVERESIZE_WINDOW]) {
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
		}

		if (cme->message_type == gpa->ewmh_atoms[_NET_ACTIVE_WINDOW]) {
			DEBUG("ClientMessage _NET_ACTIVE_WINDOW\n");

			XMapRaised(gpa->display, client->window);

			/* update stack */
			gpa_ewmh_client_list_update_with_screen(gpa, client->screen);

			return TRUE;
		}
	}

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
