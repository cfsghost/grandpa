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
gpa_screenlock_enter(GrandPa *gpa)
{
	GPaScreenLock *screenlock;
	GList *node;
	GPaScreen *screen;

	if (gpa->mode == GPA_MODE_SCREENLOCK)
		return;

	gpa->mode = GPA_MODE_SCREENLOCK;

	/* Lock all screens */
	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		gpa->backend->engine->screenlock_enter(gpa->backend->data, screen);
	}
}

void
gpa_screenlock_leave(GrandPa *gpa)
{
	GPaScreenLock *screenlock;
	GList *node;
	GPaScreen *screen;

	if (gpa->mode != GPA_MODE_SCREENLOCK)
		return;

	gpa->mode = GPA_MODE_NORMAL;

	/* Lock all screens */
	for (node = gpa->screen->screens; node; node = g_list_next(node)) {
		screen = (GPaScreen *)node->data;

		gpa->backend->engine->screenlock_leave(gpa->backend->data, screen);
	}
}
