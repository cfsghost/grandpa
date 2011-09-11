#ifndef GRANDPA_CLIENT_H
#define GRANDPA_CLIENT_H

#include "ewmh.h"

struct _GPaScreen;
typedef struct _GPaClient {
	GrandPa *gpa;

	struct _GPaScreen *screen;
	Window container;
	Window window;

	gint x, y;
	gint width, height;
	gchar *name;

	gboolean override_redirect;
	Window trans;

	gpointer backend;

	EWMHWindowType type;
	EWMHWindowState wstate;
	gint state;

	gboolean accepts_focus;
	gboolean never_map;
	gboolean priv_window;
} GPaClient;


GPaClient *gpa_client_find_with_window(GrandPa *gpa, Window w);
GPaClient *gpa_client_remove_with_window(GrandPa *gpa, Window w);

#endif
