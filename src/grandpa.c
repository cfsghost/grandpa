#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include "grandpa.h"
#include "ewmh.h"
#include "backend.h"
#include "client.h"
#include "clutter-backend.h"

extern GPaBackendClass clutter_backend_class;

int main(int argc, char *argv[])
{
	GrandPa *gpa;

#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif

	DEBUG("Starting GrandPa Window Manager\n");
	gpa = (GrandPa *)g_new0(GrandPa, 1);
	/* Initial Backend */
	gpa_backend_init(gpa, GRANDPA_CLUTTER_BACKEND, &argc, &argv);

	/* XRandr extension */
	XRRQueryExtension(gpa->display,
		&gpa->xrandr_event_base,
		&gpa->xrandr_error_base);

	/* Initializing EWMH */
	gpa_ewmh_init(gpa);

	/* Initializing all screens */
	gpa_screenmgr_init(gpa);

	gpa_backend_event_init(gpa);

	/* Main loop */
	gpa_backend_main(gpa);

	return 0;
}
