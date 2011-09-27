#include <glib.h>
#include <glib/gi18n.h>

#include "grandpa.h"
#include "backend.h"

void
gpa_screenlock_init(GrandPa *gpa, GPaScreen *screen)
{
	GPaScreenLock *screenlock;

	DEBUG("Initializing Screen Lock\n");

	/* Initializing Screen Lock */
	screenlock = (GPaScreenLock *)g_new0(GPaScreenLock, 1);
	screenlock->gpa = gpa;
	screenlock->screen = screen;

	screen->screenlock = screenlock;
	gpa->backend->engine->screenlock_init(gpa->backend->data, screen);
}

void
gpa_screenlock_update(GrandPa *gpa, GPaScreen *screen)
{
	gchar *date_strings = NULL;

	/* Get current date */
	date_strings = g_date_time_format(gpa->datetime, "%I:%M");

	/* Update clock */
	gpa->backend->engine->screenlock_update_label(gpa->backend->data, screen, date_strings, NULL);

	g_free(date_strings);
}

void
gpa_screenlock_enter(GrandPa *gpa)
{
	GList *node;
	GPaScreen *screen;

	if (gpa->mode == GPA_MODE_SCREENLOCK)
		return;

	gpa->mode = GPA_MODE_SCREENLOCK;

	/* Start clock updater */
	gpa_clock_start(gpa);

	/* Lock all screens */
	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		/* Update Labels */
		gpa_screenlock_update(gpa, screen);

		/* Show screen lock */
		gpa->backend->engine->screenlock_enter(gpa->backend->data, screen);
	}
}

void
gpa_screenlock_leave(GrandPa *gpa)
{
	GList *node;
	GPaScreen *screen;

	if (gpa->mode != GPA_MODE_SCREENLOCK)
		return;

	gpa->mode = GPA_MODE_NORMAL;

	/* Unlock all screens */
	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		gpa->backend->engine->screenlock_leave(gpa->backend->data, screen);
	}
}
