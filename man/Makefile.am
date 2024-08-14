EXTRA_DIST =						\
	xings-date-time.sgml

if HAVE_DOCBOOK2MAN
man_MANS =						\
	xings-date-time.1
endif

if HAVE_DOCBOOK2MAN
xings-date-time.1: xings-date-time.sgml
	$(AM_V_GEN) docbook2man $? > /dev/null
endif

clean-local :
	rm -f *~
	rm -f *.1
	rm -f manpage.*
