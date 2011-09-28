#include <glib.h>
#include <glib/gi18n.h>
#include <X11/X.h>
#include <clutter/clutter.h>
#include <clutter/x11/clutter-x11.h>
#include <clutter/glx/clutter-glx.h>

#include "grandpa.h"
#include "clutter-backend.h"

static ClutterActor *
gpa_backend_round_rectangle_new(gint width, gint height, gdouble radius, ClutterColor *color)
{
	ClutterActor *actor;
	cairo_t *cr;
	cairo_pattern_t *pat;
	gdouble degrees = G_PI / 180.0;
	gdouble border_radius = radius + 1;
	gdouble border_width = width;
	gdouble border_height = height;
	gdouble content_width = width - 2;
	gdouble content_height = height - 2;

	/* Initializing a new actor to use Cairo */
	actor = clutter_cairo_texture_new(width, height);
	cr = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(actor));

	/* Clear */
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	/* Border */
	cairo_new_sub_path(cr);
	cairo_arc(cr, border_width - border_radius, border_radius, border_radius, -90 * degrees, 0);
	cairo_arc(cr, border_width - border_radius, border_height - border_radius, border_radius, 0, 90 * degrees);
	cairo_arc(cr, border_radius, border_height - border_radius, border_radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr, border_radius, border_radius, border_radius, 180 * degrees, 270 * degrees);
	cairo_close_path(cr);

	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.3);
	cairo_fill(cr);

	/* path for rounded rectangle */
	cairo_new_sub_path(cr);
	cairo_arc(cr, 1 + content_width - radius, 1 + radius, radius, -90 * degrees, 0);
	cairo_arc(cr, 1 + content_width - radius, 1 + content_height - radius, radius, 0, 90 * degrees);
	cairo_arc(cr, 1 + radius, 1 + content_height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr, 1 + radius, 1+ radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path(cr);

	pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, height);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0.1, 0.1, 0.1, 1.0);
	cairo_pattern_add_color_stop_rgba(pat, 0.2, 0.0, 0.0, 0.0, 1.0);
	cairo_pattern_add_color_stop_rgba(pat, 0, 0, 0, 0, 1.0);
	cairo_set_source(cr, pat);
//	cairo_set_source_rgb(cr, (gdouble)color->red / 255, (gdouble)color->green / 255, (gdouble)color->blue / 255);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);

	cairo_destroy(cr);

	return actor;
}

static ClutterActor *
gpa_backend_clutter_create_panel(gfloat width, gfloat height)
{
	ClutterActor *panel;
	cairo_t *cr;

	panel = clutter_cairo_texture_new(width, height);
	cr = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(panel));
	
	/* Clear Cairo operator */
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	cairo_rectangle(cr, 0, 0, width, height);
	cairo_set_source_rgba(cr, .0, .0, .0, 0.8);
	cairo_fill(cr);

	cairo_move_to(cr, 0, height - 1.5);
	cairo_line_to(cr, width, height - 1.5);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.2);

	cairo_set_line_width(cr, 1.0);
	cairo_stroke(cr);

	cairo_destroy(cr);

	return panel;
}

static ClutterActor *
gpa_backend_clutter_create_light(gfloat width, gfloat height)
{
	ClutterActor *light;
	cairo_t *cr;
	cairo_pattern_t *pat;

	light = clutter_cairo_texture_new(width, height);
	cr = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(light));

	/* Clear Cairo operator */
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, height);
	cairo_pattern_add_color_stop_rgba(pat, 1, 1, 1, 1, 0.25);
	cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 0.5);
	cairo_rectangle(cr, 0, 0, width, height);

	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);

	cairo_move_to(cr, 0.0, 0.0);
	cairo_line_to(cr, width, 0.0);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.6);
	cairo_set_line_width(cr, 1.0);
	cairo_stroke(cr);

	cairo_destroy(cr);

	return light;
}

static ClutterActor *
gpa_backend_clutter_create_button(gfloat width, gfloat height)
{
	ClutterActor *button;
	cairo_t *cr;
	cairo_pattern_t *pat;
	gdouble radius = 8.0;
	gdouble degrees = G_PI / 180.0;

	button = clutter_cairo_texture_new(width, height);
	cr = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(button));

	/* Clear Cairo operator */
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, height);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0.55, 0.55, 0.55, 1.0);
	cairo_pattern_add_color_stop_rgba(pat, 0.9, 0.5, 0.5, 0.5, 1.0);
	cairo_pattern_add_color_stop_rgba(pat, 0, 1, 1, 1, 1.0);

	/* path for rounded rectangle */
	cairo_new_sub_path(cr);
	cairo_arc(cr, width - radius, radius, radius, -90 * degrees, 0);
	cairo_arc(cr, width - radius, height - radius, radius, 0, 90 * degrees);
	cairo_arc(cr, radius, height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr, radius, radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path(cr);

	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);

	/* highlight */
	cairo_move_to(cr, radius, height - 0.5);
	cairo_line_to(cr, width - radius, height - 0.5);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.2);
	cairo_set_line_width(cr, 1.0);
	cairo_stroke(cr);

	cairo_destroy(cr);

	return button;
}

static ClutterActor *
gpa_backend_clutter_create_arrow(gfloat width, gfloat height)
{
	ClutterActor *icon;
	cairo_t *cr;
	cairo_pattern_t *pat;
	gint x, y;

	icon = clutter_cairo_texture_new(width, height);
	cr = clutter_cairo_texture_create(CLUTTER_CAIRO_TEXTURE(icon));

	/* Clear Cairo operator */
	cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
	cairo_paint(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

	pat = cairo_pattern_create_linear(0.0, 0.0, 0.0, height);
	cairo_pattern_add_color_stop_rgba(pat, 1, 0.5, 0.5, 0.5, 1.0);
	cairo_pattern_add_color_stop_rgba(pat, 0.9, 0.0, 0.0, 0.0, 1.0);
	cairo_pattern_add_color_stop_rgba(pat, 0, 0.0, 0.0, 0.0, 1.0);

	/* path for arrow */
	cairo_new_sub_path(cr);
	y = (gint)(height * 0.25);
	cairo_move_to(cr, 0.0, y);
	x = (gint)(width * 0.55);
	cairo_line_to(cr, x, y);
	cairo_line_to(cr, x, 0);
	y = (gint)(height * 0.5);
	cairo_line_to(cr, width, y);
	cairo_line_to(cr, x, height);
	y = (gint)(height * 0.75);
	cairo_line_to(cr, x, y);
	cairo_line_to(cr, 0.0, y);
	y = (gint)(height * 0.25);
	cairo_line_to(cr, 0.0, y);
	cairo_close_path(cr);

	cairo_set_source(cr, pat);
	cairo_fill(cr);
	cairo_pattern_destroy(pat);
#if 0
	/* highlight */
	cairo_move_to(cr, radius, height - 0.5);
	cairo_line_to(cr, width - radius, height - 0.5);
	cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.2);
	cairo_set_line_width(cr, 1.0);
	cairo_stroke(cr);
#endif
	cairo_destroy(cr);

	return icon;
}

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
	ClutterColor slider_text_color = { 0x77, 0x77, 0x77, 0xff};

	cbscreen->screenlock.container = clutter_group_new(); 
	cbscreen->screenlock.background = clutter_rectangle_new_with_color(&color);
	clutter_actor_set_size(cbscreen->screenlock.background, screen->width, screen->height);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.container), cbscreen->screenlock.background);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->stage), cbscreen->screenlock.container);

	/* Like iOS, screen lock is lower than panel */
	clutter_actor_lower(cbscreen->screenlock.container, cbscreen->panel.container);

	/* Panel */
	cbscreen->screenlock.panel = clutter_group_new(); 
	cbscreen->screenlock.panel_background = gpa_backend_clutter_create_panel(screen->width, screen->height * 0.125);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.container), cbscreen->screenlock.panel);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.panel), cbscreen->screenlock.panel_background);

	/* Light on panel */
	cbscreen->screenlock.panel_light = gpa_backend_clutter_create_light(screen->width, screen->height * 0.0625);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.panel), cbscreen->screenlock.panel_light);

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
	cbscreen->screenlock.slider_panel_background = gpa_backend_clutter_create_panel(screen->width, screen->height * 0.1);
	clutter_actor_set_position(cbscreen->screenlock.slider_panel, screen->width * 0.5, screen->height);
	clutter_actor_set_anchor_point_from_gravity(cbscreen->screenlock.slider_panel, CLUTTER_GRAVITY_SOUTH);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.container), cbscreen->screenlock.slider_panel);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider_panel), cbscreen->screenlock.slider_panel_background);

	/* Light on panel */
	cbscreen->screenlock.slider_panel_light = gpa_backend_clutter_create_light(screen->width, screen->height * 0.05);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider_panel), cbscreen->screenlock.slider_panel_light);

	/* Slider */
	cbscreen->screenlock.slider = clutter_group_new();
	cbscreen->screenlock.slider_background = gpa_backend_round_rectangle_new(200, 40, 10.0, &panel_color);
	clutter_actor_set_position(cbscreen->screenlock.slider,
		clutter_actor_get_width(cbscreen->screenlock.slider_panel) * 0.5,
		clutter_actor_get_height(cbscreen->screenlock.slider_panel) * 0.5);
	clutter_actor_set_anchor_point_from_gravity(cbscreen->screenlock.slider, CLUTTER_GRAVITY_CENTER);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider), cbscreen->screenlock.slider_background);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider_panel), cbscreen->screenlock.slider);

	/* Slider Label */
	cbscreen->screenlock.slider_label = clutter_text_new();
	clutter_text_set_text(CLUTTER_TEXT(cbscreen->screenlock.slider_label), _("Slide to unlock"));
	clutter_text_set_color(CLUTTER_TEXT(cbscreen->screenlock.slider_label), &slider_text_color);
	clutter_text_set_font_name(CLUTTER_TEXT(cbscreen->screenlock.slider_label), "Sans 12");
	clutter_actor_set_anchor_point_from_gravity(cbscreen->screenlock.slider_label, CLUTTER_GRAVITY_WEST);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider), cbscreen->screenlock.slider_label);
	clutter_actor_set_position(cbscreen->screenlock.slider_label,
		clutter_actor_get_width(cbscreen->screenlock.slider_background) * 0.25 + 15,
		clutter_actor_get_height(cbscreen->screenlock.slider) * 0.5);

	/* Slider Button */
	cbscreen->screenlock.slider_button = clutter_group_new();
	cbscreen->screenlock.slider_button_background = gpa_backend_clutter_create_button(
		clutter_actor_get_width(cbscreen->screenlock.slider_background) * 0.25,
		clutter_actor_get_height(cbscreen->screenlock.slider_background) - 6);
	clutter_actor_set_position(cbscreen->screenlock.slider_button, 3, 3);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider_button), cbscreen->screenlock.slider_button_background);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider), cbscreen->screenlock.slider_button);

	/* Slider button icon */
	cbscreen->screenlock.slider_button_icon = gpa_backend_clutter_create_arrow(
		clutter_actor_get_width(cbscreen->screenlock.slider_button) * 0.5,
		clutter_actor_get_height(cbscreen->screenlock.slider_button) * 0.5);
	clutter_actor_set_position(cbscreen->screenlock.slider_button_icon,
		clutter_actor_get_width(cbscreen->screenlock.slider_button) * 0.5,
		clutter_actor_get_height(cbscreen->screenlock.slider_button) * 0.5);
	clutter_actor_set_anchor_point_from_gravity(cbscreen->screenlock.slider_button_icon, CLUTTER_GRAVITY_CENTER);
	clutter_container_add_actor(CLUTTER_CONTAINER(cbscreen->screenlock.slider_button), cbscreen->screenlock.slider_button_icon);

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
	clutter_actor_set_y(cbscreen->screenlock.slider_panel, (gfloat)screen->height);
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
		"y", (gfloat)screen->height + clutter_actor_get_height(cbscreen->screenlock.slider_panel),
		NULL);
}
