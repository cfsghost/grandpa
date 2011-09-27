#ifndef GRANDPA_BACKEND_H
#define GRANDPA_BACKEND_H

#include "grandpa.h"
#include "client.h"

typedef enum {
	GPA_FUNC_CALL_CONTROLBAR_ENABLE,
	GPA_FUNC_CALL_CONTROLBAR_DISABLE
} GPaFuncCall;

typedef struct _GPaBackend {
	GrandPa *gpa;
	gpointer priv;
} GPaBackend;

typedef struct _GPaBackendClass {
	Window (*get_root_window)(struct _GPaBackend *this, GPaScreen *screen);

	gboolean (*call)(struct _GPaBackend *this, GPaFuncCall func, gpointer userdata);
	gboolean (*screen_window_check)(struct _GPaBackend *this, Window w);
	void (*handle_event)(struct _GPaBackend *this, XEvent *ev, GPaClient *client);

	void (*init)(struct _GPaBackend *this, int *argc, char ***argv);
	void (*screen_init)(struct _GPaBackend *this, GPaScreen *screen);
	void (*event_init)(struct _GPaBackend *this);
	void (*event_create_notify)(struct _GPaBackend *this, XEvent *ev);
	void (*main)(struct _GPaBackend *this);

	/* Panel */
	void (*panel_init)(struct _GPaBackend *this, GPaScreen *screen);
	void (*panel_resize)(struct _GPaBackend *this, GPaScreen *screen, gfloat width, gfloat height);
	void (*panel_update_clock)(struct _GPaBackend *this, GPaScreen *screen, const gchar *clock);
	void (*panel_update_battery)(struct _GPaBackend *this, GPaScreen *screen, gboolean charging, int percent);

	/* Screen Lock */
	void (*screenlock_init)(struct _GPaBackend *this, GPaScreen *screen);
	void (*screenlock_enter)(struct _GPaBackend *this, GPaScreen *screen);
	void (*screenlock_leave)(struct _GPaBackend *this, GPaScreen *screen);
	void (*screenlock_update_label)(struct _GPaBackend *this, GPaScreen *screen, const gchar *label, const gchar *sublabel);
} GPaBackendClass;

typedef struct _GPaBackendManager {
	GPaBackendClass *engine;
	GPaBackend *data;
} GPaBackendManager;

typedef void (*BackendXEventFunc) (GrandPa *gpa, XEvent *ev, GPaClient *client);

void gpa_backend_handle_event(GrandPa *gpa, XEvent *ev, GPaClient *client);

#endif
