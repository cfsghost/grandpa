#include <glib.h>
#include <glib/gi18n.h>

#include "grandpa.h"
#include "backend.h"

gboolean
gpa_clock_update_loop(gpointer data)
{
	GrandPa *gpa = (GrandPa *)data;

	g_date_time_unref(gpa->datetime);
	gpa->datetime = g_date_time_new_now_local();

	return TRUE;
}

inline void
gpa_clock_init(GrandPa *gpa)
{
	gpa->datetime = g_date_time_new_now_local();
}

inline void
gpa_clock_start(GrandPa *gpa)
{
	if (gpa->clock_source)
		return;

	/* Loop to update status */
	gpa->clock_source = g_timeout_add_full(G_PRIORITY_LOW, 1000, gpa_clock_update_loop, gpa, NULL);
}

inline void
gpa_clock_stop(GrandPa *gpa)
{
	g_source_remove(gpa->clock_source);
	gpa->clock_source = 0;
}
