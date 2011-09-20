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
	GPaCBClientStateMapping     = (1 << 0),
	GPaCBClientStateUnmapping   = (1 << 1),
	GPaCBClientStateDestroying  = (1 << 2)
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
	ClutterActor *overlay;
	GPaClientIState state;
} GPaClutterBackendClient;

typedef struct {
	GPollFD event_poll_fd;
} GPaClutterBackend;

GPaClutterBackendClient *gpa_backend_clutter_create_client(GrandPa *gpa, GPaClient *client);
ClutterActor            *gpa_backend_clutter_create_overlay(gfloat width, gfloat height);
void                     gpa_backend_clutter_event_init(GPaBackend *this);

/* Animation Callback */
void gpa_backend_clutter_enable_panel_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_disable_panel_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_unmap_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_window_destroy_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_event_handle(GPaBackend *this, XEvent *ev, GPaClient *client);

/* Panel */
void gpa_backend_clutter_panel_init(GPaBackend *this);

#endif
