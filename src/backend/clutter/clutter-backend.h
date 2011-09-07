#ifndef GRANDPA_CLUTTER_BACKEND_H
#define GRANDPA_CLUTTER_BACKEND_H

#include <clutter/clutter.h>
#include "backend.h"
#include "screenmgr.h"
#include "client.h"

#define GPA_BACKEND_CLUTTER(a) ((GPaClutterBackend *)a)
#define GPA_BACKEND_CLUTTER_CLIENT(a) ((GPaClutterBackendClient *)a)
#define GPA_BACKEND_CLUTTER_SCREEN(a) ((GPaClutterBackendScreen *)a)

#define GRANDPA_CLUTTER_BACKEND &clutter_backend_class

typedef enum {
	GPaCBClientStateNone,
	GPaCBClientStateMapping,
	GPaCBClientStateUnmapping,
	GPaCBClientStateDestroying
} GPaClientIState;

typedef struct {
	Window stage_window;
	ClutterActor *stage;
	ClutterActor *viewport;
	struct _panel {
		ClutterActor *container;
		ClutterActor *background;
		ClutterActor *shadow;
	} panel;
} GPaClutterBackendScreen;

typedef struct {
	ClutterActor *window;
	GPaClientIState state;
} GPaClutterBackendClient;

typedef struct {
	GPollFD event_poll_fd;
} GPaClutterBackend;

GPaClutterBackendClient *gpa_backend_clutter_create_client(GrandPa *gpa, Window w);
void                     gpa_backend_clutter_event_init(GPaBackend *this);

/* Animation Callback */
void gpa_backend_clutter_enable_panel_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_disable_panel_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_unmap_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_window_destroy_completed(ClutterAnimation *animation, gpointer user_data);

#endif
