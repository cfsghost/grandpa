#ifndef GRANDPA_SCREENMGR_H
#define GRANDPA_SCREENMGR_H

typedef enum {
	GA_SCREEN_WINDOW_ROOT_WINDOW,
	GA_SCREEN_WINDOW_OVERLAY_WINDOW,
	GA_SCREEN_WINDOW_INPUT_WINDOW
} GaScreenWindowType;

GPaScreen *gpa_screenmgr_get_screen_with_internal_window(GrandPa *gpa, Window w, GaScreenWindowType type);

#endif
