#include <glib.h>
#include <glib/gi18n.h>

#include "grandpa.h"
#include "backend.h"

typedef struct _GPaPanel {
	GrandPa *gpa;
	GPaScreen *screen;
	gint width;
	gint height;

	/* Clock */
	GDateTime *datetime;
	gchar *date_strings;
} GPaPanel;

void
gpa_panel_update_clock(GPaPanel *panel)
{
	GrandPa *gpa = panel->gpa;
	/* Get current date */
	panel->datetime = g_date_time_to_local(panel->datetime);
	panel->date_strings = g_date_time_format(panel->datetime, "%p %I:%M");

	/* Update clock */
	gpa->backend->engine->panel_update_clock(gpa->backend->data, panel->screen, panel->date_strings);
	g_free(panel->date_strings);
}

gboolean
gpa_panel_update_loop(gpointer data)
{
	GPaPanel *panel = (GPaPanel *)data;

	gpa_panel_update_clock(panel);

	/* TODO: Update battery information */

	return TRUE;
}

void
gpa_panel_init(GrandPa *gpa, GPaScreen *screen)
{
	GPaPanel *panel;

	DEBUG("Initializing Panel\n");

	panel = (GPaPanel *)g_new0(GPaPanel, 1);
	panel->gpa = gpa;
	panel->screen = screen;
	panel->width = screen->width;
	panel->height = 18;
	panel->datetime = g_date_time_new_now_local();

	gpa->backend->engine->panel_init(gpa->backend->data, screen);
	gpa->backend->engine->panel_resize(gpa->backend->data, screen, panel->width, panel->height);

	/* Update available space of screen */
	screen->avail_y += panel->height;
	screen->avail_height -= panel->height;

	/* Update clock on panel */
	gpa_panel_update_clock(panel);
	g_timeout_add_full(G_PRIORITY_LOW, 1000, gpa_panel_update_loop, panel, NULL);
}
