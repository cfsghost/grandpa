#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>

#include "grandpa.h"

void
gpa_panel_init(GrandPa *gpa)
{
	gpa_backend_panel_init(gpa);
}
