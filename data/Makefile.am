SUBDIRS = appdata

@INTLTOOL_XML_NOMERGE_RULE@

@INTLTOOL_DESKTOP_RULE@
desktopdir = $(datadir)/applications
desktop_in_files = xings-date-time.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

pkgdata_DATA = \
	xdt-datetime-dialog.ui \
	xdt-timezone-dialog.ui \
	xdt-panel.ui

EXTRA_DIST= \
	$(desktop_in_files) \
	$(pkgdata_DATA)

CLEANFILES = \
	$(desktop_DATA)
