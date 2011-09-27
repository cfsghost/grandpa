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
	gchar *date_strings;
} GPaPanel;

void
gpa_panel_update_clock(GPaPanel *panel)
{
	GrandPa *gpa = panel->gpa;

	/* Get current date */
	panel->date_strings = g_date_time_format(gpa->datetime, "%p %I:%M");

	/* Update clock */
	gpa->backend->engine->panel_update_clock(gpa->backend->data, panel->screen, panel->date_strings);
	g_free(panel->date_strings);
}

void
gpa_panel_update_battery(GPaPanel *panel)
{
	GrandPa *gpa = panel->gpa;

	/* TODO: Get battery status */

	/* Update clock */
	gpa->backend->engine->panel_update_battery(gpa->backend->data, panel->screen, TRUE, 100);
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
	panel->height = 20;

	gpa->backend->engine->panel_init(gpa->backend->data, screen);

	/* Update available space of screen */
	screen->avail_y += panel->height;
	screen->avail_height -= panel->height;

	/* We need to run clock updater */
	gpa_clock_start(gpa);

	/* Update components */
	gpa_panel_update_clock(panel);
	gpa_panel_update_battery(panel);

	/* Using components size to resize panel */
	gpa->backend->engine->panel_resize(gpa->backend->data, screen, panel->width, panel->height);

	/* Loop to update status */
	g_timeout_add_full(G_PRIORITY_LOW, 1000, gpa_panel_update_loop, panel, NULL);
}
