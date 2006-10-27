CONFIG_DEFS=@CONFIG_DEFS@

bin_PROGRAMS = gnuitar
gnuitar_SOURCES =  amp.c main.c pump.c chorus.c delay.c echo.c tracker.c tremolo.c \
	vibrato.c autowah.c phasor.c rcfilter.c tubeamp.c rotary.c \
	gui.c distort.c distort2.c sustain.c reverb.c backbuf.c utils.c \
	amp.h pump.h chorus.h delay.h echo.h tracker.h tremolo.h \
	vibrato.h autowah.h phasor.h rcfilter.h tubeamp.h rotary.h \
	gui.h distort.h distort2.h sustain.h reverb.h backbuf.h utils.h \
	noise.c noise.h biquad.c biquad.h eqbank.c eqbank.h \
	tuner.c tuner.h audio-alsa.c audio-alsa.h audio-oss.c audio-oss.h \
	pitch.c pitch.h glib12-compat.c glib12-compat.h audio-jack.c \
	audio-jack.h audio-driver.c audio-driver.h audio-midi.h audio-midi.c \
	effect.c effect.h

EXTRA_DIST = *.c *.h Makefile* *.rc *.xpm

INCLUDES = \
	-DG_LOG_DOMAIN=\"gnuitar\" -D_GNU_SOURCE \
	$(GTK_CFLAGS) $(GLIB_CFLAGS) $(CONFIG_DEFS) \
	-I$(includedir) -Wall -ffast-math

gnuitar_LDADD = \
	$(GTK_LIBS) $(GLIB_LIBS) $(ALSA_LIBS) $(SNDFILE_LIBS) $(JACK_LIBS) $(FFTW3_LIBS)

gnuitar_LDFLAGS= \
	-Wl,--as-needed
