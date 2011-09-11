#include <glib.h>
#include <glib/gi18n.h>

#include "grandpa.h"
#include "client.h"
#include "backend.h"

Window
gpa_backend_get_root_window(GrandPa *gpa, GPaScreen *screen)
{
	return gpa->backend->engine->get_root_window(gpa->backend->data, screen);
}

gboolean
gpa_backend_call(GrandPa *gpa, GPaFuncCall func, gpointer userdata)
{
	return gpa->backend->engine->call(gpa->backend->data, func, userdata);
}

gboolean
gpa_backend_screen_window_check(GrandPa *gpa, Window w)
{
	return gpa->backend->engine->screen_window_check(gpa->backend->data, w);
}

void
gpa_backend_handle_event(GrandPa *gpa, XEvent *ev, GPaClient *client)
{
	gpa->backend->engine->handle_event(gpa->backend->data, ev, client);
}

inline void
gpa_backend_main(GrandPa *gpa)
{
	DEBUG("Main Loop\n");

	gpa->backend->engine->main(gpa->backend->data);
}

inline void
gpa_backend_screen_init(GrandPa *gpa, GPaScreen *screen)
{
	gpa->backend->engine->screen_init(gpa->backend->data, screen);
}

inline void
gpa_backend_event_init(GrandPa *gpa)
{
	DEBUG("Initializing event dispatcher\n");
	gpa->backend->engine->event_init(gpa->backend->data);
}

void
gpa_backend_init(GrandPa *gpa, GPaBackendClass *class, int *argc, char ***argv)
{
	GPaBackend *backend;

	gpa->backend = (GPaBackendManager *)g_new0(GPaBackendManager, 1);
	gpa->backend->data = (GPaBackend *)g_new0(GPaBackend, 1);
	gpa->backend->data->gpa = gpa;
	gpa->backend->engine = class;
	gpa->backend->engine->init(gpa->backend->data, argc, argv);
}
