#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <X11/extensions/Xrandr.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "screenmgr.h"
#include "backend.h"
#include "client.h"
#include "clutter-backend.h"

#define GPA_BACKEND_CLUTTER(a) ((GPaClutterBackend *)a)
#define GPA_BACKEND_CLUTTER_CLIENT(a) ((GPaClutterBackendClient *)a)
#define GPA_BACKEND_CLUTTER_SCREEN(a) ((GPaClutterBackendScreen *)a)

GPaBackend *cbackend;

static GPaClutterBackendClient *
gpa_backend_clutter_create_client(GrandPa *gpa, Window w)
{
	GPaClutterBackendClient *cbclient;
    XWindowAttributes attr; 

	cbclient = (GPaClutterBackendClient *)g_new0(GPaClutterBackendClient, 1);

    XGetWindowAttributes(gpa->display, w, &attr);
    if (attr.class == InputOnly)
        return cbclient;

	cbclient->window = clutter_glx_texture_pixmap_new_with_window(w);
	DEBUG("Create window texture\n");

	return cbclient;
}

static void
gpa_backend_clutter_event_configure_notify(GrandPa *gpa, Window w)
{
	GPaClient *client;

	/* get information of window */
	client = gpa_client_find_with_window(gpa, w);
	if (!client)
		return;

	if (client->priv_window)
		return;

	gpa_client_property_update(gpa, client);

	if (!client->backend)
		/* A new window */
		client->backend = (GPaClutterBackendClient *)gpa_backend_clutter_create_client(gpa, w);

	DEBUG("Configure Notify \n");
}

static void
gpa_backend_clutter_window_destroy_completed(ClutterAnimation *animation, gpointer user_data)
{
	GPaClient *client = (GPaClient *)user_data;
	GPaClutterBackendClient *cbclient = (GPaClutterBackendClient *)client->backend;

	clutter_actor_destroy(cbclient->window);

	g_free(cbclient);

	gpa_client_destroy(client->gpa, client);

	DEBUG("Window was destroyed\n");
}

static void
gpa_backend_clutter_event_destroy_notify(GrandPa *gpa, Window w)
{
	GPaClient *client;
	GPaClutterBackendClient *cbclient;

	/* get information of window */
	client = (GPaClient *)gpa_client_killer_find_with_window(gpa, w);
	if (!client)
		return;

	cbclient = (GPaClutterBackendClient *)client->backend;
	if (!cbclient)
		return;

	if (cbclient->state == GPaCBClientStateDestroying ||
		cbclient->state == GPaCBClientStateUnmapping)
		return;

	cbclient->state = GPaCBClientStateDestroying;
	if (cbclient->window) {
		if (client->never_map) {
			gpa_backend_clutter_window_destroy_completed(NULL, client);
			return;
		}

		clutter_actor_destroy(cbclient->window);

		g_free(cbclient);

		gpa_client_destroy(gpa, client);

		DEBUG("Window %ld was destroyed\n", client->window);
#if 0
		clutter_x11_texture_pixmap_set_automatic(cbclient->window, FALSE);
		clutter_actor_show(cbclient->window);

		/* menu window */
		if (client->trans != None || client->override_redirect) {
			clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 800,
				"opacity", 0x00,
				"signal-after::completed", gpa_backend_clutter_window_destroy_completed, client,
				NULL);
		} else {
			clutter_actor_set_scale_with_gravity(cbclient->window, 1.0, 1.0, CLUTTER_GRAVITY_CENTER);
			clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 800,
				"scale-x", 0.0,
				"scale-y", 0.0,
				"signal-after::completed", gpa_backend_clutter_window_destroy_completed, client,
				NULL);
		}
#endif
	}

//	DEBUG("clutter backend destroy notify\n");
}

static void
gpa_backend_clutter_event_map_notify(GrandPa *gpa, Window w)
{
	GPaClient *client;
	GPaClutterBackendClient *cbclient;

	client = gpa_client_find_with_window(gpa, w);
	if (!client)
		return;

	if (client->priv_window)
		return;

	/* OpenGL application doesn't have ConfigureRequest event,
	 * so we need to create a new actor of client.
	 */
	cbclient = (GPaClutterBackendClient *)client->backend;
	if (!cbclient) {
		cbclient = gpa_backend_clutter_create_client(gpa, w);
		client->backend = cbclient;
	}

	if (!cbclient->window)
		return;

	DEBUG("Mapping Window Type: %ld\n", client->type);

	clutter_actor_set_size(cbclient->window, client->width, client->height);
	clutter_actor_set_position(cbclient->window, client->x, client->y);

	if (clutter_actor_get_parent(cbclient->window) == NULL) {
		clutter_container_add_actor(CLUTTER_CONTAINER(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport), cbclient->window);

		clutter_x11_texture_pixmap_set_automatic((ClutterX11TexturePixmap *)cbclient->window, TRUE);
	}

	clutter_actor_raise_top(cbclient->window);
	clutter_actor_show(cbclient->window);

	DEBUG("GET SIZ %lfx%lf\n", clutter_actor_get_width(cbclient->window), clutter_actor_get_height(cbclient->window));
	DEBUG("GET POS %lfx%lf\n", clutter_actor_get_x(cbclient->window), clutter_actor_get_y(cbclient->window));
	DEBUG("GET STAGE POS %lfx%lf\n", clutter_actor_get_x(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport), clutter_actor_get_y(GPA_BACKEND_CLUTTER_SCREEN(client->screen->backend)->viewport));

	/* Some GUI toolkits sets incorrect window type for menu window,
	 * so we check menu window with WM_TRANSIENT_FOR and OverrideRedirect.
	 */
	if (client->trans != None || client->override_redirect) {
		clutter_actor_set_opacity(cbclient->window, 0x00);
		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 400,
			"opacity", 0xff,
			NULL);
	} else {
		clutter_actor_set_scale_with_gravity(cbclient->window, 0.1, 0.1, CLUTTER_GRAVITY_CENTER);
//		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 600,
		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CIRC, 600,
			"scale-x", 1.0,
			"scale-y", 1.0,
			NULL);
	}

//	DEBUG("Window was mapped\n");
}

static void
gpa_backend_clutter_unmap_completed(ClutterAnimation *animation, gpointer user_data)
{
	GPaClient *client = (GPaClient *)user_data;
	GPaClutterBackendClient *cbclient = (GPaClutterBackendClient *)client->backend;

	clutter_actor_hide(cbclient->window);
	cbclient->state = GPaCBClientStateNone;

	DEBUG("Window %ld was unmapped\n", client->window);
}

static void
gpa_backend_clutter_event_unmap_notify(GrandPa *gpa, Window w)
{
	GPaClient *client;
	GPaClutterBackendClient *cbclient;

	client = gpa_client_find_with_window(gpa, w);
	if (!client)
		return;

	if (client->priv_window)
		return;

	cbclient = (GPaClutterBackendClient *)client->backend;
	if (!cbclient)
		return;

	if (!cbclient->window)
		return;

	if (cbclient->state == GPaCBClientStateUnmapping)
		return;

	clutter_x11_texture_pixmap_set_automatic((ClutterX11TexturePixmap *)cbclient->window, FALSE);
	cbclient->state = GPaCBClientStateUnmapping;

	if (client->trans != None || client->override_redirect) {
		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 400,
			"opacity", 0x00,
			"signal-after::completed", gpa_backend_clutter_unmap_completed, client,
			NULL);
	} else {
		clutter_actor_set_scale_with_gravity(cbclient->window, 1.0, 1.0, CLUTTER_GRAVITY_CENTER);
		clutter_actor_animate(cbclient->window, CLUTTER_EASE_OUT_CUBIC, 600,
			"scale-x", 0.0,
			"scale-y", 0.0,
			"signal-after::completed", gpa_backend_clutter_unmap_completed, client,
			NULL);
//	} else {
//		gpa_backend_clutter_unmap_completed(NULL, client);
	}

	DEBUG("Window %ld was unmapping\n", client->window);
}

gboolean
gpa_backend_clutter_screen_window_check(GPaBackend *this, Window w)
{
	GrandPa *gpa = this->gpa;
	GList *node;
	GPaScreen *screen;
	GPaClutterBackendScreen *cbscreen;

	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;
		cbscreen = (GPaClutterBackendScreen *)screen->backend;

		if (cbscreen->stage_window == w)
			return TRUE;
	}

	return FALSE;
}

static void
gpa_backend_clutter_disable_panel_completed(ClutterAnimation *animation, gpointer user_data)
{
	GPaClient *client = (GPaClient *)user_data;
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)user_data;

	/* Hide Panel */
	clutter_actor_hide(cbscreen->panel.container);

	DEBUG("Hide Panel\n");
}

static void
gpa_backend_clutter_enable_panel_completed(ClutterAnimation *animation, gpointer user_data)
{
	GPaClient *client = (GPaClient *)user_data;
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)user_data;

	/* Hide Panel */
	clutter_actor_show(cbscreen->panel.container);

	DEBUG("Show Panel\n");
}

gboolean
gpa_backend_clutter_call(GPaBackend *this, GPaFuncCall func, gpointer userdata)
{
	GrandPa *gpa = this->gpa;
	GList *node;
	GPaScreen *screen;
	GPaClutterBackendScreen *cbscreen;

	DEBUG("Backend got a Call\n");

	switch(func) {
	case GPA_FUNC_CALL_CONTROLBAR_ENABLE:
		DEBUG("Enable Control Bar\n");

		/* Move all viewports for showing control bar */
		for (node = gpa->screen->screens; node; node = g_list_next(node)) {
			screen = (GPaScreen *)node->data;
			cbscreen = (GPaClutterBackendScreen *)screen->backend;

			/* Show Panel */
			clutter_actor_show_all(cbscreen->panel.container);
			clutter_actor_animate(cbscreen->viewport, CLUTTER_EASE_OUT_CIRC, 600,
				"y", -120.0,
				"signal-after::completed", gpa_backend_clutter_enable_panel_completed, cbscreen,
				NULL);
		}

		break;
	case GPA_FUNC_CALL_CONTROLBAR_DISABLE:
		DEBUG("Disable Control Bar\n");

		/* Move all viewports for showing control bar */
		for (node = gpa->screen->screens; node; node = g_list_next(node)) {
			screen = (GPaScreen *)node->data;
			cbscreen = (GPaClutterBackendScreen *)screen->backend;

			clutter_actor_animate(cbscreen->viewport, CLUTTER_EASE_OUT_CIRC, 600,
				"y", .0,
				"signal-after::completed", gpa_backend_clutter_disable_panel_completed, cbscreen,
				NULL);
		}

		break;
	default:
		return FALSE;
	}

	return TRUE;
}

static ClutterX11FilterReturn
gpa_backend_clutter_event_filter(XEvent *ev, ClutterEvent *cev, gpointer data)
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
		gpa_eventdisp_send((GrandPa *)data, ev);
		gpa_backend_clutter_event_configure_notify((GrandPa *)data, ev->xconfigure.window);
		return CLUTTER_X11_FILTER_CONTINUE;

	case MapNotify:
		gpa_backend_clutter_event_map_notify((GrandPa *)data, ev->xmap.window);
		return CLUTTER_X11_FILTER_CONTINUE;

	case UnmapNotify:
		gpa_eventdisp_send((GrandPa *)data, ev);
		gpa_backend_clutter_event_unmap_notify((GrandPa *)data, ev->xmap.window);
		return CLUTTER_X11_FILTER_CONTINUE;

	case ClientMessage:
		DEBUG("Backend got ClientMessage\n");
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_CONTINUE;

	case PropertyNotify:
		gpa_eventdisp_send((GrandPa *)data, ev);
		return CLUTTER_X11_FILTER_CONTINUE;

	case ButtonPress:
	case EnterNotify:
	case LeaveNotify:
	case FocusIn:
	case VisibilityNotify:
	case ColormapNotify:
	case MappingNotify:
	case MotionNotify:
	case SelectionNotify:
		g_print("XXXX %d\n", ev->type);

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

void
gpa_backend_clutter_event_init(GPaBackend *this)
{
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
}

void
gpa_backend_clutter_init(GPaBackend *this, int *argc, char ***argv)
{
	GrandPa *gpa = this->gpa;

	DEBUG("Initializing Clutter Backend\n");

	/* Initializing private data space for itself */
	this->priv = (gpointer)g_new0(GPaClutterBackend, 1);

	clutter_x11_set_use_argb_visual(TRUE);

	clutter_x11_disable_event_retrieval();

	clutter_init(argc, argv);

	clutter_x11_add_filter(gpa_backend_clutter_event_filter, gpa);

	/* get display */
	gpa->display = clutter_x11_get_default_display();
}

void
gpa_backend_clutter_input_init(GPaBackend *this, Window w)
{
	GrandPa *gpa = this->gpa;

	/* Using XFixes extension to allow input pass through */
	XserverRegion region = XFixesCreateRegion(gpa->display, NULL, 0);

	XFixesSetWindowShapeRegion(gpa->display, w, ShapeBounding, 0, 0, 0);
	XFixesSetWindowShapeRegion(gpa->display, w, ShapeInput, 0, 0, region);

	XFixesDestroyRegion(gpa->display, region);
}

void
gpa_backend_clutter_screen_init(GPaBackend *this, GPaScreen *screen)
{
	GrandPa *gpa = this->gpa;
	GPaClutterBackend *priv = (GPaClutterBackend *)this->priv;
	GPaClutterBackendScreen *cbscreen;

	ClutterColor color = { 0x00, 0x00, 0x00, 0xff};
	ClutterActor *actor;

	/* Allocate */
	cbscreen = (GPaClutterBackendScreen *)g_new0(GPaClutterBackendScreen, 1);
	screen->backend = cbscreen;
#if 1
	/* Initializing stage */
	cbscreen->stage = clutter_stage_get_default();
	cbscreen->stage_window = clutter_x11_get_stage_window(CLUTTER_STAGE(cbscreen->stage));
	cbscreen->viewport = clutter_group_new();
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->viewport);
	DEBUG("Stage window id: %ld\n", cbscreen->stage_window);

	/* Create a Panel */
	cbscreen->panel.container = clutter_group_new();
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->panel.container);
	cbscreen->panel.background = clutter_texture_new_from_file("panel_background.png", NULL);

	/* Setting X Window */
	XReparentWindow(gpa->display, cbscreen->stage_window, screen->overlay, 0, 0);
	XSelectInput(gpa->display, cbscreen->stage_window,
		ExposureMask | StructureNotifyMask);
	XSync(gpa->display, FALSE);

#if 0
	{
		XWindowChanges xwc;
		xwc.x = 0;
		xwc.y = 0;
		xwc.width = screen->width;
		xwc.height = screen->height;

		XConfigureWindow(gpa->display, cbscreen->stage_window, CWX | CWY | CWWidth | CWHeight, &xwc);
		XMapWindow(gpa->display, cbscreen->stage_window);
	}

	{
		XSetWindowAttributes attr;
		attr.win_gravity = StaticGravity;
		XChangeWindowAttributes(gpa->display, cbscreen->stage_window, CWWinGravity, &attr);
	}
#endif

	clutter_stage_set_color(CLUTTER_STAGE(cbscreen->stage), &color);
	clutter_actor_set_size(cbscreen->stage, (gfloat)screen->width, (gfloat)screen->height);
	clutter_actor_set_size(cbscreen->viewport, (gfloat)screen->width, (gfloat)screen->height);
	clutter_stage_ensure_viewport(CLUTTER_STAGE(cbscreen->stage));

	/* Configure panel */
	clutter_texture_set_repeat(CLUTTER_TEXTURE(cbscreen->panel.background), TRUE, TRUE);
	clutter_actor_set_position(cbscreen->panel.container, 0, clutter_actor_get_height(cbscreen->stage) - 120);
	clutter_actor_set_size(cbscreen->panel.background, clutter_actor_get_width(cbscreen->stage), 120);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->panel.container), cbscreen->panel.background);
	clutter_actor_lower(cbscreen->panel.container, cbscreen->viewport);
	clutter_actor_hide(cbscreen->panel.container);

	/* Allow input pass through */
	gpa_backend_clutter_input_init(this, screen->overlay);
	gpa_backend_clutter_input_init(this, cbscreen->stage_window);

	clutter_actor_realize(cbscreen->stage);
	clutter_actor_show(cbscreen->stage);
#endif
}

void
gpa_backend_clutter_main(GPaBackend *this)
{
	clutter_main();
//	g_main_loop_run(g_main_loop_new(NULL, FALSE));
}

GPaBackendClass clutter_backend_class = {
	call: gpa_backend_clutter_call,
	screen_window_check: gpa_backend_clutter_screen_window_check,

	init: gpa_backend_clutter_init,
	screen_init: gpa_backend_clutter_screen_init,
	event_init: gpa_backend_clutter_event_init,
	main: gpa_backend_clutter_main
};
