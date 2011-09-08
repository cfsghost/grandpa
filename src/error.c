#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>

#include "grandpa.h"
#include "error.h"

static int trapped_error_code = 0;
extern GrandPa *gpa;

static int
gpa_error_trapped_xerror_handler(Display *display, XErrorEvent *error)
{
	trapped_error_code = error->error_code;

	if (gpa->mode == GPA_MODE_INITIALIZING &&
		error->request_code == X_ChangeWindowAttributes &&
		error->error_code == BadAccess) {
		g_print("Another window manager is already running!\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}

void
gpa_error_trap_xerrors()
{
	trapped_error_code = 0;

	XSetErrorHandler(gpa_error_trapped_xerror_handler);
}

void
gpa_error_untrap_xerrors()
{
	trapped_error_code = 0;

	XSetErrorHandler(NULL);
}
