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
	struct _FunctionLayer {
		ClutterActor *container;
		ClutterActor *background;
		ClutterActor *shadow;
	} funclayer;

	struct _Panel {
		ClutterActor *container;
		ClutterActor *background;
		ClutterActor *clock;
		ClutterActor *battery;
	} panel;

	struct _ScreenLock {
		ClutterActor *container;
		ClutterActor *background;

		ClutterActor *panel;
		ClutterActor *panel_background;
		ClutterActor *panel_light;
		ClutterActor *panel_label;
		ClutterActor *panel_sublabel;

		ClutterActor *slider_panel;
		ClutterActor *slider_panel_background;
		ClutterActor *slider_panel_light;
	} screenlock;
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
void gpa_backend_clutter_enable_funclayer_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_disable_funclayer_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_unmap_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_window_destroy_completed(ClutterAnimation *animation, gpointer user_data);
void gpa_backend_clutter_event_handle(GPaBackend *this, XEvent *ev, GPaClient *client);

/* Panel */
void gpa_backend_clutter_panel_init(GPaBackend *this, GPaScreen *screen);
void gpa_backend_clutter_panel_resize(GPaBackend *this, GPaScreen *screen, gfloat width, gfloat height);
void gpa_backend_clutter_panel_update_clock(GPaBackend *this, GPaScreen *screen, const gchar *clock);
void gpa_backend_clutter_panel_update_battery(struct _GPaBackend *this, GPaScreen *screen, gboolean charging, int percent);

/* Screen Lock */
void gpa_backend_clutter_screenlock_init(GPaBackend *this, GPaScreen *screen);
void gpa_backend_clutter_screenlock_enter(GPaBackend *this, GPaScreen *screen);
void gpa_backend_clutter_screenlock_leave(GPaBackend *this, GPaScreen *screen);
void gpa_backend_clutter_screenlock_update_label(GPaBackend *this, GPaScreen *screen, const gchar *label, const gchar *sublabel);

#endif
