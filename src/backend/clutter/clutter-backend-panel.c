#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "clutter-backend.h"

void
gpa_backend_clutter_panel_init(GPaBackend *this, GPaScreen *screen)
{
	GrandPa *gpa = this->gpa;
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;
	ClutterColor color = { 0x00, 0x00, 0x00, 0xaa};
	ClutterColor text_color = { 0xff, 0xff, 0xff, 0xff};

	cbscreen->panel.container = clutter_group_new(); 
	cbscreen->panel.background = clutter_rectangle_new_with_color(&color);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->panel.container), cbscreen->panel.background);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->panel.container);

	cbscreen->panel.clock = clutter_text_new();
	clutter_text_set_color(CLUTTER_TEXT(cbscreen->panel.clock), &text_color);
	clutter_text_set_font_name(CLUTTER_TEXT(cbscreen->panel.clock), "Sans 12px");
	clutter_actor_set_anchor_point_from_gravity(cbscreen->panel.clock, CLUTTER_GRAVITY_CENTER);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->panel.container), cbscreen->panel.clock);
}

void
gpa_backend_clutter_panel_resize(GPaBackend *this, GPaScreen *screen, gfloat width, gfloat height)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	DEBUG("Resize Panel to %fx%f\n", width, height);

	/* Resize panel size */
	clutter_actor_set_size(cbscreen->panel.background, width, height);
	clutter_actor_set_size(cbscreen->panel.container, width, height);

	/* Relocate clock */
	clutter_actor_set_position(cbscreen->panel.clock, width * 0.5, height * 0.5);
}

void
gpa_backend_clutter_panel_update_clock(GPaBackend *this, GPaScreen *screen, const gchar *clock)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	clutter_text_set_text(CLUTTER_TEXT(cbscreen->panel.clock), clock);
}
