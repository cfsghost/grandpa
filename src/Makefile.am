## Process this file with automake to produce Makefile.in

bin_PROGRAMS = grandpa

INCLUDES = \
	-I$(top_srcdir) \
	-DPACKAGE_BIN_DIR=\""$(bindir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	$(PACKAGE_CFLAGS) \
	$(G_CAST_CHECKS)

AUTOMAKE_OPTIONS = subdir-objects

grandpa_SOURCES = \
	keydef.h \
	error.c \
	error.h \
	cursor.c \
	cursor.h \
	ewmh.c \
	ewmh.h \
	client.c \
	client.h \
	backend.c \
	backend.h \
	eventdisp.c \
	eventdisp.h \
	backend/clutter/clutter-backend.c \
	backend/clutter/clutter-backend-event.c \
	backend/clutter/clutter-backend-callback.c \
	backend/clutter/clutter-backend-panel.c \
	backend/clutter/clutter-backend-screenlock.c \
	backend/clutter/clutter-backend.h \
	clock/clock.c \
	panel/panel-core.c \
	screenlock/screenlock-core.c \
	screenmgr.c \
	screenmgr.h \
	grandpa.h \
	grandpa.c

grandpa_LDADD = \
		$(PACKAGE_LIBS) \
		$(INTLLIBS)

