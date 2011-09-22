#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "clutter-backend.h"

typedef enum {
	BATTERY_ICON_AC,
	BATTERY_ICON_CHARGING,
	BATTERY_ICON_100_PERCENT,
	BATTERY_ICON_70_PERCENT,
	BATTERY_ICON_50_PERCENT,
	BATTERY_ICON_30_PERCENT,
	BATTERY_ICON_15_PERCENT,
	BATTERY_ICON_10_PERCENT,
	BATTERY_ICON_5_PERCENT
} BatteryIcon;

gchar *battery_icons[] = {
	"battery_ac.png",
	"battery_charging.png",
	"battery_100.png",
	"battery_70.png",
	"battery_50.png",
	"battery_30.png",
	"battery_15.png",
	"battery_10.png",
	"battery_5.png"
};

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

	/* Clock */
	cbscreen->panel.clock = clutter_text_new();
	clutter_text_set_color(CLUTTER_TEXT(cbscreen->panel.clock), &text_color);
	clutter_text_set_font_name(CLUTTER_TEXT(cbscreen->panel.clock), "Sans 12px");
	clutter_actor_set_anchor_point_from_gravity(cbscreen->panel.clock, CLUTTER_GRAVITY_CENTER);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->panel.container), cbscreen->panel.clock);

	/* Battery */
	cbscreen->panel.battery = clutter_texture_new();
	clutter_actor_set_anchor_point_from_gravity(cbscreen->panel.battery, CLUTTER_GRAVITY_CENTER);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->panel.container), cbscreen->panel.battery);
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

	/* Relocate clock */
	clutter_actor_set_position(cbscreen->panel.battery,
		width - (clutter_actor_get_width(cbscreen->panel.battery) + 2) * 0.5,
		height * 0.5);
}

void
gpa_backend_clutter_panel_update_clock(GPaBackend *this, GPaScreen *screen, const gchar *clock)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	clutter_text_set_text(CLUTTER_TEXT(cbscreen->panel.clock), clock);
}

void
gpa_backend_clutter_panel_update_battery(GPaBackend *this, GPaScreen *screen, gboolean charging, int percent)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	if (charging) {
		if (percent == 100)
			clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_AC], NULL);
		else
			clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_CHARGING], NULL);

		return;
	}

	if (percent > 90) {
		clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_100_PERCENT], NULL);
	} else if (percent > 70) {
		clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_70_PERCENT], NULL);
	} else if (percent > 50) {
		clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_50_PERCENT], NULL);
	} else if (percent > 30) {
		clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_30_PERCENT], NULL);
	} else if (percent > 15) {
		clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_15_PERCENT], NULL);
	} else if (percent > 5) {
		clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_10_PERCENT], NULL);
	} else {
		clutter_texture_set_from_file(CLUTTER_TEXTURE(cbscreen->panel.battery), battery_icons[BATTERY_ICON_5_PERCENT], NULL);
	}
}
