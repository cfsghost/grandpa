#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>

#include "error.h"

static int trapped_error_code = 0;

static int
gpa_error_trapped_xerror_handler(Display *display, XErrorEvent *error)
{
	trapped_error_code = error->error_code;

	return 0;
}

void
gpa_error_trap_xerrors()
{
	trapped_error_code = 0;

	XSetErrorHandler(gpa_error_trapped_xerror_handler);
}

int
gpa_error_untrap_xerrors()
{
	trapped_error_code = 0;

	XSetErrorHandler(NULL);

	return 0;
}
