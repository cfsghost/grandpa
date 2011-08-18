#ifndef GRANDPA_EVENTDISP_H
#define GRANDPA_EVENTDISP_H

#include <X11/Xlib.h>
#include "grandpa.h"

typedef struct {
	gint type;
	gboolean (*handler)(GrandPa *gpa, XEvent *ev);
} EventDispatcher;

#endif
