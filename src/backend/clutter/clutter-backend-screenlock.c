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
	cairo_t *cr;
	cairo_pattern_t *pat;
	ClutterColor color = { 0x00, 0x00, 0x00, 0x22};
	ClutterColor panel_color = { 0x00, 0x00, 0x00, 0xbb};
	ClutterColor text_color = { 0xff, 0xff, 0xff, 0xff};

	cbscreen->screenlock.container = clutter_group_new(); 
	cbscreen->screenlock.background = clutter_rectangle_new_with_color(&color);
	clutter_actor_set_size(cbscreen->screenlock.background, screen->width, screen->height);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.container), cbscreen->screenlock.background);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->screenlock.container);

	/* Like iOS, screen lock is lower than panel */
	clutter_actor_lower(cbscreen->screenlock.container, cbscreen->panel.container);

	/* Panel */
	cbscreen->screenlock.panel = clutter_group_new(); 
	cbscreen->screenlock.panel_background = clutter_rectangle_new_with_color(&panel_color);	
	clutter_actor_set_size(cbscreen->screenlock.panel_background, screen->width, screen->height * 0.125);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.container), cbscreen->screenlock.panel);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.panel), cbscreen->screenlock.panel_background);

	/* Light on panel */
	cbscreen->screenlock.panel_light = clutter_cairo_texture_new(screen->width, screen->height * 0.0625);
	cr = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(cbscreen->screenlock.panel_light));
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.panel), cbscreen->screenlock.panel_light);

	/* Clear Cairo operator */
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, screen->height * 0.0625);
	cairo_pattern_add_color_stop_rgba(pat, 1, 1, 1, 1, 0.25);
	cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 0.5);
	cairo_rectangle(cr, 0, 0, screen->width, screen->height * 0.0625);

	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);
	cairo_destroy(cr);

	/* Label */
	cbscreen->screenlock.panel_label = clutter_text_new();
	cbscreen->screenlock.panel_sublabel = clutter_text_new();
	clutter_text_set_color(CLUTTER_TEXT(cbscreen->screenlock.panel_label), &text_color);
	clutter_text_set_color(CLUTTER_TEXT(cbscreen->screenlock.panel_sublabel), &text_color);
	clutter_text_set_font_name(CLUTTER_TEXT(cbscreen->screenlock.panel_label), "Sans 48");
	clutter_text_set_font_name(CLUTTER_TEXT(cbscreen->screenlock.panel_sublabel), "Sans 14");
	clutter_actor_set_anchor_point_from_gravity(cbscreen->screenlock.panel_label, CLUTTER_GRAVITY_NORTH);
	clutter_actor_set_anchor_point_from_gravity(cbscreen->screenlock.panel_sublabel, CLUTTER_GRAVITY_SOUTH);
	clutter_actor_set_position(cbscreen->screenlock.panel_label, screen->width * 0.5, .0);
	clutter_actor_set_position(cbscreen->screenlock.panel_sublabel, screen->width * 0.5, clutter_actor_get_height(cbscreen->screenlock.panel) - 10);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.panel), cbscreen->screenlock.panel_label);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.panel), cbscreen->screenlock.panel_sublabel);

	/* Slider Panel */
	cbscreen->screenlock.slider_panel = clutter_group_new(); 
	cbscreen->screenlock.slider_panel_background = clutter_rectangle_new_with_color(&panel_color);	
	clutter_actor_set_size(cbscreen->screenlock.slider_panel_background, screen->width, screen->height * 0.1);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.container), cbscreen->screenlock.slider_panel);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider_panel), cbscreen->screenlock.slider_panel_background);

	/* Light on panel */
	cbscreen->screenlock.slider_panel_light = clutter_cairo_texture_new(screen->width, screen->height * 0.05);
	cr = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(cbscreen->screenlock.slider_panel_light));
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider_panel), cbscreen->screenlock.slider_panel_light);

	/* Clear Cairo operator */
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, screen->height * 0.05);
	cairo_pattern_add_color_stop_rgba(pat, 1, 1, 1, 1, 0.25);
	cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 0.5);
	cairo_rectangle(cr, 0, 0, screen->width, screen->height * 0.05);

	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);
	cairo_destroy(cr);

	clutter_actor_hide(cbscreen->screenlock.container);
}

void
gpa_backend_clutter_screenlock_update_label(GPaBackend *this, GPaScreen *screen, const gchar *label, const gchar *sublabel)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	clutter_text_set_text(CLUTTER_TEXT(cbscreen->screenlock.panel_label), label);
	clutter_text_set_text(CLUTTER_TEXT(cbscreen->screenlock.panel_sublabel), sublabel);
}

void
gpa_backend_clutter_screenlock_enter(GPaBackend *this, GPaScreen *screen)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	clutter_actor_set_opacity(cbscreen->screenlock.background, 0x00);
	clutter_actor_set_y(cbscreen->screenlock.panel, screen->avail_y);
	clutter_actor_set_y(cbscreen->screenlock.slider_panel, screen->height - clutter_actor_get_height(cbscreen->screenlock.slider_panel));
	clutter_actor_show(cbscreen->screenlock.container);
	clutter_actor_animate(cbscreen->screenlock.background, CLUTTER_EASE_OUT_CUBIC, 420,
		"opacity", 0xff,
		NULL);
}

void
gpa_backend_clutter_screenlock_leave(GPaBackend *this, GPaScreen *screen)
{
	GPaClutterBackendScreen *cbscreen = (GPaClutterBackendScreen *)screen->backend;

	clutter_actor_animate(cbscreen->screenlock.background, CLUTTER_EASE_OUT_CUBIC, 420,
		"opacity", 0x00,
		NULL);

	clutter_actor_animate(cbscreen->screenlock.panel, CLUTTER_EASE_OUT_CUBIC, 360,
		"y", -clutter_actor_get_height(cbscreen->screenlock.panel),
		NULL);

	clutter_actor_animate(cbscreen->screenlock.slider_panel, CLUTTER_EASE_OUT_CUBIC, 360,
		"y", (gfloat)screen->height,
		NULL);
}
