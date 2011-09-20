#include <glib.h>
#include <glib/gi18n.h>

#include "grandpa.h"
#include "backend.h"

inline void
gpa_backend_panel_init(GrandPa *gpa)
{
	DEBUG("Initializing Panel\n");

	gpa->backend->engine->panel_init(gpa->backend->data);
}
