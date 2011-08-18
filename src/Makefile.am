## Process this file with automake to produce Makefile.in

bin_PROGRAMS = grandpa

INCLUDES = \
	-I$(top_srcdir) \
	-DPACKAGE_BIN_DIR=\""$(bindir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	$(PACKAGE_CFLAGS) \
	$(G_CAST_CHECKS)

grandpa_SOURCES = \
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
	clutter-backend.c \
	clutter-backend.h \
	screenmgr.c \
	screenmgr.h \
	grandpa.h \
	grandpa.c


grandpa_LDADD = \
		$(PACKAGE_LIBS) \
		$(INTLLIBS)

