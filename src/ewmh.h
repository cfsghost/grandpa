#ifndef GRANDPA_EWMH_H
#define GRANDPA_EWMH_H

typedef enum {
/* root window properties */
	_NET_SUPPORTED,
	_NET_CLIENT_LIST,
	_NET_CLIENT_LIST_STACKING,
	_NET_NUMBER_OF_DESKTOPS,
	_NET_DESKTOP_GEOMETRY,
	_NET_DESKTOP_VIEWPORT,
	_NET_CURRENT_DESKTOP,
	/*_NET_DESKTOP_NAMES,*/
	_NET_ACTIVE_WINDOW,
	_NET_WORKAREA,
	_NET_SUPPORTING_WM_CHECK,
	/*_NET_VIRTUAL_ROOTS,*/
	/*_NET_DESKTOP_LAYOUT,*/
	/*_NET_SHOWING_DESKTOP,*/
/* other root window messages */
	_NET_CLOSE_WINDOW,
	_NET_MOVERESIZE_WINDOW,
	_NET_WM_MOVERESIZE,
	_NET_RESTACK_WINDOW,
/* application window properties */
	_NET_WM_NAME,
	/*_NET_WM_VISIBLE_NAME,*/
	/*_NET_WM_ICON_NAME,*/
	/*_NET_WM_VISIBLE_ICON_NAME,*/
	/*_NET_WM_DESKTOP,*/
	_NET_WM_WINDOW_TYPE,
	_NET_WM_STATE,
	_NET_WM_ALLOWED_ACTIONS,
	_NET_WM_STRUT,
	/*_NET_WM_ICON_GEOMETRY,*/
	/*_NET_WM_ICON,*/
	/*_NET_WM_PID,*/
	/*_NET_WM_HANDLED_ICONS,*/
/* window types for _NET_WM_WINDOW_TYPE */
	_NET_WM_WINDOW_TYPE_DESKTOP,
	_NET_WM_WINDOW_TYPE_DOCK,
	_NET_WM_WINDOW_TYPE_TOOLBAR,
	_NET_WM_WINDOW_TYPE_MENU,
	_NET_WM_WINDOW_TYPE_UTILITY,
	_NET_WM_WINDOW_TYPE_SPLASH,
	_NET_WM_WINDOW_TYPE_DIALOG,
	_NET_WM_WINDOW_TYPE_NORMAL,
/* window states for _NET_WM_STATE */
	/*_NET_WM_STATE_MODAL,*/
	/*_NET_WM_STATE_STICKY,*/
	_NET_WM_STATE_MAXIMIZED_VERT,
	_NET_WM_STATE_MAXIMIZED_HORZ,
	/*_NET_WM_STATE_SHADED,*/
	_NET_WM_STATE_SKIP_TASKBAR,
	_NET_WM_STATE_SKIP_PAGER,
	_NET_WM_STATE_HIDDEN,
	_NET_WM_STATE_FULLSCREEN,
	_NET_WM_STATE_ABOVE,
	_NET_WM_STATE_BELOW,
/* allowed actions for _NET_WM_ALLOWED_ACTIONS */
	_NET_WM_ACTION_MOVE,
	_NET_WM_ACTION_RESIZE,
	/*_NET_WM_ACTION_MINIMIZE,*/
	/*_NET_WM_ACTION_SHADE,*/
	/*_NET_WM_ACTION_STICK,*/
	_NET_WM_ACTION_MAXIMIZE_HORZ,
	_NET_WM_ACTION_MAXIMIZE_VERT,
	_NET_WM_ACTION_FULLSCREEN,
	/*_NET_WM_ACTION_CHANGE_DESKTOP,*/
	_NET_WM_ACTION_CLOSE,
	EWMH_ATOM_LAST
} EWMHAtom;

typedef enum {
	WTypeDesktop,
	WTypeDock,
	WTypeToolbar,
	WTypeMenu,
	WTypeUtility,
	WTypeSplash,
	WTypeDialog,
	WTypeNormal,
	WTypeNone
} EWMHWindowType;

typedef struct {
	gboolean skip_taskbar;
	gboolean skip_pager;
	gboolean fullscreen;
	gboolean above;
	gboolean below;
} EWMHWindowState;

#endif
