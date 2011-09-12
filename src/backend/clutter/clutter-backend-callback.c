#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <X11/extensions/Xrandr.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "clutter-backend.h"

void
gpa_backend_clutter_window_destroy_completed(ClutterAnimation *animation, gpointer user_data)
{
//	GPaClient *client = (GPaClient *)user_data;
//	GPaClutterBackendClient *cbclient = (GPaClutterBackendClient *)client->backend;
	GPaClutterBackendClient *cbclient = (GPaClutterBackendClient *)user_data;

	if (!cbclient)
		return;

	clutter_actor_destroy(cbclient->window);

	g_free(cbclient);

	DEBUG("Window was destroyed\n");
}

void
gpa_backend_clutter_unmap_completed(ClutterAnimation *animation, gpointer user_data)
{
//	GPaClient *client = (GPaClient *)user_data;
//	GPaClutterBackendClient *cbclient = (GPaClutterBackendClient *)client->backend;
	GPaClutterBackendClient *cbclient = (GPaClutterBackendClient *)user_data;

	if (!cbclient)
		return;

	if (cbclient->state & GPaCBClientStateDestroying) {
		gpa_backend_clutter_window_destroy_completed(NULL, cbclient);
	} else {
		clutter_actor_hide(cbclient->window);
		cbclient->state = GPaCBClientStateNone;
	}
}

void
gpa_backend_clutter_disable_panel_completed(ClutterAnimation *animation, gpointer user_data)
{
	GPaClient *client = (GPaClient *)user_data;
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)user_data;

	/* Hide Panel */
	clutter_actor_hide(cbscreen->panel.container);

	DEBUG("Hide Panel\n");
}

void
gpa_backend_clutter_enable_panel_completed(ClutterAnimation *animation, gpointer user_data)
{
	GPaClient *client = (GPaClient *)user_data;
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)user_data;

	/* Hide Panel */
	clutter_actor_show(cbscreen->panel.container);

	DEBUG("Show Panel\n");
}
