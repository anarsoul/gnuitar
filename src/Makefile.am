CONFIG_DEFS=@CONFIG_DEFS@

bin_PROGRAMS = gnuitar
gnuitar_SOURCES =  main.c pump.c chorus.c delay.c echo.c tracker.c tremolo.c \
	vibrato.c autowah.c phasor.c rcfilter.c \
	gui.c distort.c distort2.c sustain.c reverb.c backbuf.c utils.c \
	pump.h chorus.h delay.h echo.h tracker.h tremolo.h \
	vibrato.h autowah.h phasor.h rcfilter.h \
	gui.h distort.h distort2.h sustain.h reverb.h backbuf.h utils.h \
	noise.c noise.h biquad.c biquad.h eqbank.c eqbank.h \
	tuner.c tuner.h

EXTRA_DIST = *.c *.h Makefile* *.rc *.xpm

INCLUDES = \
	-DG_LOG_DOMAIN=\"gnuitar\" \
	$(GTK_CFLAGS) $(CONFIG_DEFS) \
	-I$(includedir)

gnuitar_LDADD = \
	$(GTK_LIBS) -lsndfile \
