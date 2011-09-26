#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "clutter-backend.h"

void
gpa_backend_clutter_screenlock_init(GPaBackend *this, GPaScreen *screen)
{
	GrandPa *gpa = this->gpa;
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;
	ClutterColor color = { 0x00, 0x00, 0x00, 0xaa};
	ClutterColor text_color = { 0xff, 0xff, 0xff, 0xff};

	cbscreen->panel.container = clutter_group_new(); 
	cbscreen->panel.background = clutter_rectangle_new_with_color(&color);
	clutter_actor_set_size(cbscreen->panel.background, screen->width, screen->height);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->panel.container), cbscreen->panel.background);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->panel.container);

	clutter_actor_hide(cbscreen->panel.container);
}

void
gpa_backend_clutter_screenlock_enter(GPaBackend *this, GPaScreen *screen)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	clutter_actor_set_opacity(cbscreen->panel.container, 0x00);
	clutter_actor_show(cbscreen->panel.container);
	clutter_actor_animate(cbscreen->panel.container, CLUTTER_EASE_OUT_CUBIC, 420,
		"opacity", 0xff,
		NULL);
}

void
gpa_backend_clutter_screenlock_leave(GPaBackend *this, GPaScreen *screen)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	clutter_actor_animate(cbscreen->panel.container, CLUTTER_EASE_OUT_CUBIC, 420,
		"opacity", 0x00,
		NULL);
}
