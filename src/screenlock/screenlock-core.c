#include <glib.h>
#include <glib/gi18n.h>

#include "grandpa.h"
#include "backend.h"

typedef struct _GPaScreenLock {
	GrandPa *gpa;
	GPaScreen *screen;
} GPaScreenLock;

void
gpa_screenlock_init(GrandPa *gpa, GPaScreen *screen)
{
	GPaPanel *screenlock;

	DEBUG("Initializing ScreenLock\n");

	panel = (GPaPanel *)g_new0(GPaPanel, 1);
	panel->gpa = gpa;
	panel->screen = screen;

	gpa->backend->engine->screenlock_init(gpa->backend->data, screen);
}
