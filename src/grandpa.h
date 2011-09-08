#ifndef GRANDPA_H
#define GRANDPA_H

#ifdef _DEBUG
#define DEBUG(format, args...) g_print("[%s:%d] "format, __FILE__, __LINE__, ##args)
#else
#define DEBUG(args...)
#endif

#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/shape.h>
#include "ewmh.h"

#define GRANDPA_CONTROLBAR_HEIGHT 100

typedef enum {
	ETopLeft,    ETop,     ETopRight,
	ERight,      ENone,    ELeft,
	EBottomLeft, EBottom,  EBottomRight,
	E_LAST
} WindowEdge ;

typedef enum {
	GPA_KEY_STATE_DOUBLE_CLICK = (1 << 0)
} GPaKeyState;

typedef enum {
	GPA_MODE_NORMAL,
	GPA_MODE_CONTROLBAR
} GPaMode;

typedef struct _GPaScreen {
	gint id;
	Window root;
	Window overlay;
	Window input_win;

	/* Display size */
	gint width;
	gint height;

	/* Available size */
	gint avail_width;
	gint avail_height;

	/* Cursor */
	Cursor root_cursor;
	Cursor cursor_map[E_LAST];

	/* Backend private data */
	gpointer backend;

	/* clients */
	GList *clients;
	gint client_count;

	GList *destroyed_clients;
} GPaScreen;

typedef struct {
	gint screen_count;
	GList *screens;
} GPaScreenManager;

struct _GPaBackendManager;
struct _GPaClientManager;
typedef struct {
	Display *display;
	struct _GPaBackendManager *backend;
	GPaScreenManager *screen;
	struct _GPaClientManager *client;

	Atom wm_state;
	Atom wm_change_state;
	Atom wm_protocols;
	Atom wm_delete;
	Atom wm_take_focus;
	Atom wm_colormaps;
	Atom wm_transient_for;
	Atom compound_text;
	Atom ewmh_atoms[EWMH_ATOM_LAST];
	Atom utf8_string;

	/* Mode */
	GPaMode mode;

	/* Key state */
	Time key_time;
	KeyCode keycode;
	GPaKeyState key_state;

	/* XRandr extension */
	gint xrandr_event_base;
	gint xrandr_error_base;
} GrandPa;

#endif
