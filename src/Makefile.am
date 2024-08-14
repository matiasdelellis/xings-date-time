bin_PROGRAMS = \
	xings-date-time

AM_CPPFLAGS = \
	-I$(top_srcdir)					\
	-I$(top_srcdir)/src				\
	-DPKGDATADIR=\"$(pkgdatadir)\"			\
	-DG_LOG_DOMAIN=\"xings-date-time\"		\
	-DXINGS_LOCALE_DIR=\""$(datadir)/locale"\"

xings_date_time_SOURCES =				\
	xings-date-time.c				\
	xdt-common.c					\
	xdt-common.h					\
	xdt-panel.c					\
	xdt-panel.h					\
	xdt-datetime-dialog.c				\
	xdt-datetime-dialog.h				\
	xdt-dbus.c					\
	xdt-dbus.h					\
	xdt-timezone-dialog.c				\
	xdt-timezone-dialog.h				\
	xdt-debug.c					\
	xdt-debug.h

xings_date_time_CFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GIO_CFLAGS)					\
	$(GTK_CFLAGS)

xings_date_time_LDADD =					\
	$(GLIB_LIBS)					\
	$(GIO_LIBS)					\
	$(GTK_LIBS)

CLEANFILES = *~
