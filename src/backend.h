#ifndef GRANDPA_BACKEND_H
#define GRANDPA_BACKEND_H

#include "grandpa.h"

typedef enum {
	GPA_FUNC_CALL_CONTROLBAR_ENABLE,
	GPA_FUNC_CALL_CONTROLBAR_DISABLE
} GPaFuncCall;

typedef struct _GPaBackend {
	GrandPa *gpa;
	gpointer priv;
} GPaBackend;

typedef struct _GPaBackendClass {
	gboolean (*call)(struct _GPaBackend *this, GPaFuncCall func, gpointer userdata);
	gboolean (*screen_window_check)(struct _GPaBackend *this, Window w);

	void (*init)(struct _GPaBackend *this, int *argc, char ***argv);
	void (*screen_init)(struct _GPaBackend *this, GPaScreen *screen);
	void (*event_init)(struct _GPaBackend *this);
	void (*event_create_notify)(struct _GPaBackend *this, XEvent *ev);
	void (*main)(struct _GPaBackend *this);
} GPaBackendClass;

typedef struct _GPaBackendManager {
	GPaBackendClass *engine;
	GPaBackend *data;
} GPaBackendManager;

#endif
