#ifndef GRANDPA_CLUTTER_BACKEND_H
#define GRANDPA_CLUTTER_BACKEND_H

#include <clutter/clutter.h>

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
} GPaClutterBackendScreen;

typedef struct {
	ClutterActor *window;
	GPaClientIState state;
} GPaClutterBackendClient;

typedef struct {
	GPollFD event_poll_fd;
} GPaClutterBackend;

#endif
