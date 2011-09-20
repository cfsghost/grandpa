#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "clutter-backend.h"

void
gpa_backend_clutter_panel_init(GPaBackend *this)
{
	ClutterActor *panel;
	ClutterActor *background;
	ClutterColor color = { 0x00, 0x00, 0x00, 0xaa};

	panel = clutter_group_new(); 
	background = clutter_rectangle_new_with_color(&color);
	clutter_container_add_actor(CLUTTER_CONTAINER(panel), background);
}
