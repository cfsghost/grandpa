#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <X11/extensions/Xrandr.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "clutter-backend.h"

GPaBackend *cbackend;

static void
gpa_backend_clutter_event_configure_notify(GrandPa *gpa, GPaClient *client)
{
	GPaClutterBackendClient *cbclient;

	if (!client)
		return;

	if (client->priv_window)
		return;

	if (!client->backend) {
		DEBUG("Create Object for New Window %d\n", client->window);

		/* A new window */
		cbclient = gpa_backend_clutter_create_client(gpa, client);
		if (!cbclient)
			return;

		client->backend = (GPaClutterBackendClient *)cbclient;
		clutter_actor_hide(cbclient->window);
		clutter_container_add_actor(CLUTTER_CONTAINER(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport), cbclient->window);
	} else {
		DEBUG("Re-configure object\n");
		/* Re-configure window */
		cbclient = (GPaClutterBackendClient *)client->backend;
		if (!cbclient)
			return;

		clutter_actor_set_size(cbclient->window, client->width, client->height);
		clutter_actor_set_position(cbclient->window, client->x, client->y);

		/* Create overlay actor for dialog window */
		if ((client->trans != None || client->override_redirect) && client->type == WTypeDialog && !cbclient->overlay) {
			/* Creaye a overlay actor to be background */
			cbclient->overlay = gpa_backend_clutter_create_overlay(1, 1);
		}
	}

	if (cbclient->overlay) {
		clutter_actor_hide(cbclient->overlay);
		clutter_container_add_actor(CLUTTER_CONTAINER(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport), cbclient->overlay);
		clutter_actor_lower(cbclient->overlay, cbclient->window);
	}
}

static void
gpa_backend_clutter_event_destroy_notify(GrandPa *gpa, GPaClient *client)
{
	GPaClutterBackendClient *cbclient;

	if (!client)
		return;

	cbclient = (GPaClutterBackendClient *)client->backend;
	if (!cbclient)
		return;

	if (cbclient->state & GPaCBClientStateDestroying)
		return;

	if (cbclient->state & GPaCBClientStateUnmapping) {
		cbclient->state |= GPaCBClientStateDestroying;
		return;
	}

	clutter_x11_texture_pixmap_set_automatic((ClutterX11TexturePixmap *)cbclient->window, FALSE);
	gpa_backend_clutter_window_destroy_completed(NULL, cbclient);
}

static void
gpa_backend_clutter_event_unmap_notify(GrandPa *gpa, GPaClient *client)
{
	ClutterColor color = { 0x00, 0x00, 0x00, 0xff};
	GPaClutterBackendClient *cbclient;

	if (!client)
		return;

	if (client->priv_window)
		return;

	DEBUG("Window %ld was unmapping, type: %d\n", client->window, client->type);

	cbclient = (GPaClutterBackendClient *)client->backend;
	if (!cbclient)
		return;

	if (!cbclient->window)
		return;

	if (cbclient->state & GPaCBClientStateUnmapping)
		return;

	cbclient->state |= GPaCBClientStateUnmapping;
	clutter_x11_texture_pixmap_set_automatic((ClutterX11TexturePixmap *)cbclient->window, FALSE);

	if (client->trans != None || client->override_redirect) {
		if (client->type == WTypeDialog) {
			clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 420,
				"scale-x", 0.0,
				"scale-y", 0.0,
				"signal-after::completed", gpa_backend_clutter_unmap_completed, cbclient,
				NULL);
			/* TODO: For good looks, overlay may be hided after dialog window animation */
			clutter_actor_hide(cbclient->overlay);
		} else {
			clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 420,
				"opacity", 0x00,
				"signal-after::completed", gpa_backend_clutter_unmap_completed, cbclient,
				NULL);
		}
	} else {
		clutter_actor_set_scale_with_gravity(cbclient->window, 1.0, 1.0, CLUTTER_GRAVITY_CENTER);
		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 420,
			"scale-x", 0.0,
			"scale-y", 0.0,
			"signal-after::completed", gpa_backend_clutter_unmap_completed, cbclient,
			NULL);
	}
}

static void
gpa_backend_clutter_event_map_notify(GrandPa *gpa, GPaClient *client)
{
	GPaClutterBackendClient *cbclient;

	if (!client)
		return;

	if (client->priv_window)
		return;

	/* OpenGL application doesn't have ConfigureRequest event,
	 * so we need to create a new actor of client.
	 */
	cbclient = (GPaClutterBackendClient *)client->backend;
	if (!cbclient) {
		cbclient = gpa_backend_clutter_create_client(gpa, client);
		if (!cbclient)
			return;

		DEBUG("Create object Now\n");
		client->backend = cbclient;

		clutter_actor_hide(cbclient->window);
		clutter_container_add_actor(CLUTTER_CONTAINER(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport), cbclient->window);

		if (cbclient->overlay) {
			clutter_actor_hide(cbclient->overlay);
			clutter_container_add_actor(CLUTTER_CONTAINER(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport), cbclient->overlay);
			clutter_actor_lower(cbclient->overlay, cbclient->window);
		}
	}

	if (!cbclient->window)
		return;

	DEBUG("Mapping Window ID: %ld, Type: %ld\n", client->window, client->type);

	clutter_actor_set_size(cbclient->window, client->width, client->height);
	clutter_actor_set_position(cbclient->window, client->x, client->y);

//	if (clutter_actor_get_parent(cbclient->window) == NULL) {
	DEBUG("Start to update window content automatically\n");

	clutter_x11_texture_pixmap_set_window((ClutterX11TexturePixmap *)cbclient->window, client->window, TRUE);
	if (!client->wstate.fullscreen)
		clutter_x11_texture_pixmap_set_automatic((ClutterX11TexturePixmap *)cbclient->window, TRUE);
//	}

//	clutter_actor_raise_top(cbclient->window);
	clutter_actor_show(cbclient->window);

	DEBUG("GET SIZ %lfx%lf\n", clutter_actor_get_width(cbclient->window), clutter_actor_get_height(cbclient->window));
	DEBUG("GET POS %lfx%lf\n", clutter_actor_get_x(cbclient->window), clutter_actor_get_y(cbclient->window));
	DEBUG("GET STAGE POS %lfx%lf\n", clutter_actor_get_x(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport), clutter_actor_get_y(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport));

	/* Some GUI toolkits sets incorrect window type for menu window,
	 * so we check menu window with WM_TRANSIENT_FOR and OverrideRedirect.
	 */
	if (client->trans != None || client->override_redirect) {
		if (client->type == WTypeDialog) {
			/* Dialog window has overlay actor to cover current viewport */
			clutter_actor_set_position(cbclient->overlay, 0.0, 0.0);
			clutter_actor_set_size(cbclient->overlay, client->screen->width, client->screen->height);
			clutter_actor_show(cbclient->overlay);

			clutter_actor_set_scale_with_gravity(cbclient->window, 0.1, 0.1, CLUTTER_GRAVITY_CENTER);
			clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_ELASTIC, 840,
				"scale-x", 1.0,
				"scale-y", 1.0,
				NULL);
		} else {
			clutter_actor_set_opacity(cbclient->window, 0x00);
			clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 420,
				"opacity", 0xff,
				NULL);
		}
	} else {
		if (client->type == WTypeDesktop) {
//			clutter_actor_lower_bottom(cbclient->window);
		}

		clutter_actor_set_scale_with_gravity(cbclient->window, 0.1, 0.1, CLUTTER_GRAVITY_CENTER);
//		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 600,
		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CIRC, 600,
			"scale-x", 1.0,
			"scale-y", 1.0,
			NULL);
	}

//	DEBUG("Window was mapped\n");
}
#if 0
static ClutterX11FilterReturn
_gpa_backend_clutter_event_filter(XEvent *ev, ClutterEvent *cev, gpointer data)
{
	GrandPa *gpa = (GrandPa *)data;
	GPaClient *client;

	/* Initializing event dispatcher */
	gpa_eventdisp_new(gpa, ev);

	switch (ev->type) {
	case CreateNotify:
		DEBUG("Backend got CreateNotify\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_CONTINUE;

	case KeyPress:
		if (gpa_eventdisp_send((GrandPa *)data, ev))
			return CLUTTER_X11_FILTER_REMOVE;
		else
			return CLUTTER_X11_FILTER_CONTINUE;

	case KeyRelease:
		if (gpa_eventdisp_send((GrandPa *)data, ev))
			return CLUTTER_X11_FILTER_REMOVE;
		else
			return CLUTTER_X11_FILTER_CONTINUE;

	case ConfigureRequest:
		DEBUG("Backend got ConfigureRequest\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_REMOVE;

	case MapRequest:
		DEBUG("Backend got MapRequest\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_REMOVE;

	case DestroyNotify:
		gpa_backend_clutter_event_destroy_notify((GrandPa *)data, ev->xdestroywindow.window);
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_CONTINUE;

	case ConfigureNotify:
		DEBUG("Backend got ConfigureNotify\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		gpa_backend_clutter_event_configure_notify((GrandPa *)data, ev->xconfigure.window);
		return CLUTTER_X11_FILTER_CONTINUE;

	case MapNotify:
		DEBUG("Backend got MapNotify\n");
		gpa_backend_clutter_event_map_notify((GrandPa *)data, ev->xmap.window);
		return CLUTTER_X11_FILTER_CONTINUE;

	case UnmapNotify:
		DEBUG("Backend got UnmapNotify\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		gpa_backend_clutter_event_unmap_notify((GrandPa *)data, ev->xmap.window);
		return CLUTTER_X11_FILTER_CONTINUE;

	case ClientMessage:
		DEBUG("Backend got ClientMessage\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_CONTINUE;

	case PropertyNotify:
		DEBUG("Backend got PropertyNotify\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_CONTINUE;

#if 0
	/* Ignore these events for optimization */
	case ButtonPress:
	case EnterNotify:
	case LeaveNotify:
	case FocusIn:
	case VisibilityNotify:
	case ColormapNotify:
	case MappingNotify:
	case MotionNotify:
	case SelectionNotify:
#endif
	default:
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_CONTINUE;
	}
}

static gboolean
gpa_backend_clutter_event_prepare(GSource *source, gint *timeout)
{
	gboolean ret = FALSE;

	clutter_threads_enter();

	*timeout = -1;

	if (clutter_events_pending() || XPending(cbackend->gpa->display))
		ret = TRUE;

	clutter_threads_leave();

	return ret;
}

static gboolean
gpa_backend_clutter_event_check(GSource *source)
{
	gboolean ret = FALSE;

	clutter_threads_enter();

	if (GPA_BACKEND_CLUTTER(cbackend->priv)->event_poll_fd.revents & G_IO_IN)
		if (clutter_events_pending() || XPending(cbackend->gpa->display))
			ret = TRUE;

	clutter_threads_leave();

	return ret;
}

static gboolean
gpa_backend_clutter_event_dispatch(GSource *source, GSourceFunc callback, gpointer user_data)
{
	GrandPa *gpa = (GrandPa *)cbackend->gpa;
	ClutterEvent *event;
	XEvent xevent;
	GPaScreen *screen;

	clutter_threads_enter();

	while(!clutter_events_pending() && XPending(gpa->display)) {
		XNextEvent(gpa->display, &xevent);

		screen = (GPaScreen *)gpa_screenmgr_get_screen_with_internal_window(gpa, xevent.xany.window, GA_SCREEN_WINDOW_INPUT_WINDOW);
		if (screen)
			xevent.xany.window = ((GPaClutterBackendScreen *)(screen->backend))->stage_window;
		
		clutter_x11_handle_event(&xevent);
	}
	
	event = clutter_event_get();
	if (event) {
		clutter_do_event(event);
		clutter_event_free(event);
	}

	clutter_threads_leave();

	return TRUE;
}

static GSourceFuncs gpa_backend_clutter_event_funcs = {
	gpa_backend_clutter_event_prepare,
	gpa_backend_clutter_event_check,
	gpa_backend_clutter_event_dispatch,
	NULL
};
#endif

static ClutterX11FilterReturn
gpa_backend_clutter_event_filter(XEvent *ev, ClutterEvent *cev, gpointer data)
{
	GrandPa *gpa = (GrandPa *)data;

	switch (ev->type) {
	case CreateNotify:
//		return CLUTTER_X11_FILTER_CONTINUE;
		return CLUTTER_X11_FILTER_REMOVE;

	case KeyPress:
		return CLUTTER_X11_FILTER_REMOVE;
//		return CLUTTER_X11_FILTER_CONTINUE;

	case KeyRelease:
		return CLUTTER_X11_FILTER_CONTINUE;

	case ConfigureRequest:
		return CLUTTER_X11_FILTER_REMOVE;

	case MapRequest:
		return CLUTTER_X11_FILTER_REMOVE;

	case DestroyNotify:
		return CLUTTER_X11_FILTER_REMOVE;
//		return CLUTTER_X11_FILTER_CONTINUE;

	case ConfigureNotify:
		return CLUTTER_X11_FILTER_CONTINUE;

	case MapNotify:
		return CLUTTER_X11_FILTER_CONTINUE;

	case UnmapNotify:
		return CLUTTER_X11_FILTER_CONTINUE;

	case ClientMessage:
		return CLUTTER_X11_FILTER_CONTINUE;

	case PropertyNotify:
		return CLUTTER_X11_FILTER_REMOVE;
		//return CLUTTER_X11_FILTER_CONTINUE;

	case ButtonPress:
		DEBUG("XCXXXXXXXXXXXXXXXXXXXXXXXx\n");
		return CLUTTER_X11_FILTER_REMOVE;
//		return CLUTTER_X11_FILTER_CONTINUE;
		
#if 0
	/* Ignore these events for optimization */
	case EnterNotify:
	case LeaveNotify:
	case FocusIn:
	case VisibilityNotify:
	case ColormapNotify:
	case MappingNotify:
	case MotionNotify:
	case SelectionNotify:
#endif
	}

	return CLUTTER_X11_FILTER_CONTINUE;
}

void
gpa_backend_clutter_event_init(GPaBackend *this)
{
	GrandPa *gpa = this->gpa;

	DEBUG("Initializing backend event handler\n");
#if 0
	GSource *source;
	GPollFD *event_poll_fd = &(GPA_BACKEND_CLUTTER(this->priv)->event_poll_fd);

	/* setting global variable */
	cbackend = this;

	/* create a source to hook event of X display */
	source = g_source_new(&gpa_backend_clutter_event_funcs, sizeof(GSource));

	event_poll_fd->fd = ConnectionNumber(this->gpa->display);
	event_poll_fd->events = G_IO_IN;

	g_source_set_priority(source, CLUTTER_PRIORITY_EVENTS);
	g_source_add_poll(source, event_poll_fd);
	g_source_set_can_recurse(source, TRUE);
	g_source_attach(source, NULL);
#endif
	/* Initializing event filter */
	clutter_x11_add_filter(gpa_backend_clutter_event_filter, gpa);
}

void
gpa_backend_clutter_event_handle(GPaBackend *this, XEvent *ev, GPaClient *client)
{
	GrandPa *gpa = this->gpa;
	ClutterEvent *event;

	/* We need to push X event to clutter internal handler. */
	clutter_x11_handle_event(ev);
	event = clutter_event_get();
	if (event) {
		DEBUG("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCOOOOOOOOOOOOOOOOOOOLLLL\n");
		clutter_do_event(event);
		clutter_event_free(event);
	}

	/* Handle X event by ourself */
	switch (ev->type) {
	case CreateNotify:
		DEBUG("Backend got CreateNotify\n");
		break;

	case KeyPress:
		break;

	case KeyRelease:
		break;

	case ConfigureRequest:
		DEBUG("Backend got ConfigureRequest\n");
		break;

	case MapRequest:
		DEBUG("Backend got MapRequest\n");
		break;

	case DestroyNotify:
		DEBUG("Backend got DestroyNotify\n");
		gpa_backend_clutter_event_destroy_notify(gpa, client);
		break;

	case ConfigureNotify:
		DEBUG("Backend got ConfigureNotify\n");
		gpa_backend_clutter_event_configure_notify(gpa, client);
		break;

	case MapNotify:
		DEBUG("Backend got MapNotify\n");
		gpa_backend_clutter_event_map_notify(gpa, client);
		break;

	case UnmapNotify:
		DEBUG("Backend got UnmapNotify\n");
		gpa_backend_clutter_event_unmap_notify(gpa, client);
		break;

	case ClientMessage:
		DEBUG("Backend got ClientMessage\n");
		break;

	case PropertyNotify:
		DEBUG("Backend got PropertyNotify\n");
		break;

#if 0
	/* Ignore these events for optimization */
	case ButtonPress:
	case EnterNotify:
	case LeaveNotify:
	case FocusIn:
	case VisibilityNotify:
	case ColormapNotify:
	case MappingNotify:
	case MotionNotify:
	case SelectionNotify:
#endif
	}
}
