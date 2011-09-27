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

Window
gpa_backend_clutter_get_root_window(GPaBackend *this, GPaScreen *screen)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	return cbscreen->stage_window;
}

ClutterActor *
gpa_backend_clutter_create_overlay(gfloat width, gfloat height)
{
	ClutterActor *overlay;
	ClutterColor color = { 0x00, 0x00, 0x00, 0xff};

	DEBUG("Create a overlay actor\n");
	overlay = clutter_rectangle_new_with_color(&color);
	clutter_actor_set_size(overlay, width, height);
	clutter_actor_set_opacity(overlay, 0xcc);

	return overlay;
}

GPaClutterBackendClient *
gpa_backend_clutter_create_client(GrandPa *gpa, GPaClient *client)
{
	GPaClutterBackendClient *cbclient;
    XWindowAttributes attr; 

	/* It doesn't need any real window */
    XGetWindowAttributes(gpa->display, client->window, &attr);
    if (attr.class == InputOnly) {
		DEBUG("This is InputOnly window\n");
        return NULL;
	}

	cbclient = (GPaClutterBackendClient *)g_new0(GPaClutterBackendClient, 1);
	cbclient->window = clutter_glx_texture_pixmap_new_with_window(client->window);
	DEBUG("Create window texture\n");

	/* Dialog Window */
	if ((client->trans != None || client->override_redirect) && client->type == WTypeDialog) {
		/* Creaye a overlay actor to be background */
		cbclient->overlay = gpa_backend_clutter_create_overlay(1, 1);
	}

	return cbclient;
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
			clutter_actor_show_all(cbscreen->funclayer.container);
			clutter_actor_animate(cbscreen->viewport, CLUTTER_EASE_OUT_CIRC, 600,
				"y", (gfloat)-GRANDPA_CONTROLBAR_HEIGHT,
				"signal-after::completed", gpa_backend_clutter_enable_funclayer_completed, cbscreen,
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
				"signal-after::completed", gpa_backend_clutter_disable_funclayer_completed, cbscreen,
				NULL);
		}

		break;
	default:
		return FALSE;
	}

	return TRUE;
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

//	clutter_x11_add_filter(gpa_backend_clutter_event_filter, gpa);

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

	/* Initializing stage */
	cbscreen->stage = clutter_stage_get_default();
	cbscreen->stage_window = clutter_x11_get_stage_window(CLUTTER_STAGE(cbscreen->stage));
	cbscreen->viewport = clutter_group_new();
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->viewport);
	DEBUG("Stage window id: %ld\n", cbscreen->stage_window);

	/* Create a Panel */
	cbscreen->funclayer.container = clutter_group_new();
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->funclayer.container);
	cbscreen->funclayer.background = clutter_texture_new_from_file("panel_background.png", NULL);
	cbscreen->funclayer.shadow = clutter_texture_new_from_file("shadow.png", NULL);

	/* Setting X Window */
	XReparentWindow(gpa->display, cbscreen->stage_window, screen->overlay, 0, 0);
	XSelectInput(gpa->display, cbscreen->stage_window,
		ButtonPressMask | ButtonReleaseMask |
		ExposureMask | StructureNotifyMask);
	XSync(gpa->display, FALSE);

	clutter_stage_set_color(CLUTTER_STAGE(cbscreen->stage), &color);
	clutter_actor_set_size(cbscreen->stage, (gfloat)screen->width, (gfloat)screen->height);
	clutter_actor_set_size(cbscreen->viewport, (gfloat)screen->width, (gfloat)screen->height);

	/* Configure Function Layer */
	clutter_texture_set_repeat(CLUTTER_TEXTURE(cbscreen->funclayer.background), TRUE, TRUE);
	clutter_actor_set_position(cbscreen->funclayer.container, 0, clutter_actor_get_height(cbscreen->stage) - GRANDPA_CONTROLBAR_HEIGHT);
	clutter_actor_set_size(cbscreen->funclayer.background, clutter_actor_get_width(cbscreen->stage), GRANDPA_CONTROLBAR_HEIGHT);
	clutter_actor_set_width(cbscreen->funclayer.shadow, clutter_actor_get_width(cbscreen->stage));
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->funclayer.container), cbscreen->funclayer.background);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->funclayer.container), cbscreen->funclayer.shadow);
	clutter_actor_lower(cbscreen->funclayer.container, cbscreen->viewport);
	clutter_actor_hide(cbscreen->funclayer.container);

	/* Allow input pass through */
	gpa_backend_clutter_input_init(this, screen->overlay);
	gpa_backend_clutter_input_init(this, cbscreen->stage_window);

	clutter_actor_realize(cbscreen->stage);
	clutter_actor_show(cbscreen->stage);
}

void
gpa_backend_clutter_main(GPaBackend *this)
{
	clutter_main();
//	g_main_loop_run(g_main_loop_new(NULL, FALSE));
}

GPaBackendClass clutter_backend_class = {
	get_root_window: gpa_backend_clutter_get_root_window,

	call: gpa_backend_clutter_call,
	screen_window_check: gpa_backend_clutter_screen_window_check,
	handle_event: gpa_backend_clutter_event_handle,

	init: gpa_backend_clutter_init,
	screen_init: gpa_backend_clutter_screen_init,
	event_init: gpa_backend_clutter_event_init,
	main: gpa_backend_clutter_main,

	/* Panel */
	panel_init: gpa_backend_clutter_panel_init,
	panel_resize: gpa_backend_clutter_panel_resize,
	panel_update_clock: gpa_backend_clutter_panel_update_clock,
	panel_update_battery: gpa_backend_clutter_panel_update_battery,

	/* Screen Lock */
	screenlock_init: gpa_backend_clutter_screenlock_init,
	screenlock_enter: gpa_backend_clutter_screenlock_enter,
	screenlock_leave: gpa_backend_clutter_screenlock_leave,
	screenlock_update_label: gpa_backend_clutter_screenlock_update_label
};
