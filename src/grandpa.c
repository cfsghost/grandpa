#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>

#include "grandpa.h"
#include "error.h"
#include "ewmh.h"
#include "backend.h"
#include "client.h"
#include "backend/clutter/clutter-backend.h"

extern GPaBackendClass clutter_backend_class;
GrandPa *gpa;

int main(int argc, char *argv[])
{

#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif

	DEBUG("Starting GrandPa Window Manager\n");
	gpa = (GrandPa *)g_new0(GrandPa, 1);

	/* Trap all X error event */
	gpa_error_trap_xerrors();

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

	gpa->mode = GPA_MODE_NORMAL;

	/* Main loop */
	gpa_backend_main(gpa);

	/* Untrap X error event */
	gpa_error_trap_xerrors();

	return 0;
}
