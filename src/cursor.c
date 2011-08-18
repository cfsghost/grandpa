#include <glib.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>

#include "grandpa.h"
#include "cursor.h"

static GPaCursorMapping cursor_map[] = {
	{ETopLeft,      XC_top_left_corner},
	{ETop,          XC_top_side},
	{ETopRight,     XC_top_right_corner},
	{ERight,        XC_right_side},
	{ENone,         XC_fleur},
	{ELeft,         XC_left_side},
	{EBottomLeft,   XC_bottom_left_corner},
	{EBottom,       XC_bottom_side},
	{EBottomRight,  XC_bottom_right_corner},
	{ENone,         0}
};

void
gpa_cursor_init(GrandPa *gpa, GPaScreen *screen)
{
	WindowEdge e;
	XColor black, white, exact;
	Colormap cmap;
	gint i;
	
	/* Initializing color map */
	cmap = DefaultColormap(gpa->display, screen->id);
	XAllocNamedColor(gpa->display, cmap, "white", &white, &exact);
	XAllocNamedColor(gpa->display, cmap, "black", &black, &exact);

	/* Seting cursor */
	screen->root_cursor = XCreateFontCursor(gpa->display, XC_left_ptr);
	XRecolorCursor(gpa->display, screen->root_cursor, &white, &black);

#if 0
	mwm->screens[screen_num].box_cursor = XCreateFontCursor(mwm->display, XC_draped_box);
	XRecolorCursor(mwm->display, mwm->screens[screen_num].box_cursor, &white, &black);
#endif
	DEBUG("Setting cursor map for window\n");
	for (i = 0; cursor_map[i].font_char; ++i) {
		e = cursor_map[i].edge;

		screen->cursor_map[e] = XCreateFontCursor(gpa->display, cursor_map[i].font_char);
		XRecolorCursor(gpa->display, screen->cursor_map[e], &white, &black);
	}
}
