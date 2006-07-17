# An experimental CMake build system for gnuitar.
#
# $Id$
#
# $Log$
# Revision 1.1  2006/07/17 11:26:17  alankila
# - commit an experimental exit from autotools hell
#

PROJECT(GNUitar)

# GTK+ is mandatory
FIND_PACKAGE(GTK)
IF (GTK_FOUND)
    MESSAGE("GTK found -- using GTK GUI.")
    INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
    ADD_DEFINITIONS(-DHAVE_GTK)
ELSE (GTK_FOUND)
    FIND_PACKAGE(GTK2)
    IF (GTK2_FOUND)
        MESSAGE("GTK2 found -- using GTK2 GUI.")
        INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIR})
        ADD_DEFINITIONS(-DHAVE_GTK2)
    ELSE (GTK2_FOUND)
        MESSAGE("GTK or GTK2 is mandatory -- make will fail. :-(")
    ENDIF (GTK2_FOUND)
ENDIF (GTK_FOUND)

SET(OTHER_LIBRARIES m)
IF (UNIX)
    # OSS is always available on Unix
    ADD_DEFINITIONS(-DHAVE_OSS)

    # these libraries are dynamically detected
    FIND_LIBRARY(JACK_LIBRARY
        NAMES jack
        PATHS
        /usr/local/lib
        /usr/lib)
    IF (JACK_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_JACK)
        SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} jack)
    ELSE (JACK_LIBRARY)
        MESSAGE("libjack not found -- not compiling jack support.")
    ENDIF (JACK_LIBRARY)

    FIND_LIBRARY(ALSA_LIBRARY
        NAMES jack
        PATHS
        /usr/local/lib
        /usr/lib)
    IF (ALSA_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_ALSA)
        SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} asound)
    ELSE (ALSA_LIBRARY)
        MESSAGE("libasound not found -- not compiling ALSA support.")
    ENDIF (ALSA_LIBRARY)

    FIND_LIBRARY(SNDFILE_LIBRARY
        NAMES sndfile
        PATHS
        /usr/local/lib
        /usr/lib)
    IF (SNDFILE_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_SNDFILE)
        SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} sndfile)
    ELSE (SNDFILE_LIBRARY)
        MESSAGE("libsndfile not found -- .wav export not possible.")
    ENDIF (SNDFILE_LIBRARY)

    FIND_LIBRARY(FFTW3_LIBRARY
        NAMES fftw3
        PATHS
        /usr/local/lib
        /usr/lib)
    IF (FFTW3_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_FFTW3)
        SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} fftw3)
    ELSE (FFTW3_LIBRARY)
        MESSAGE("libfftw3 not found -- using time-domain implementations where possible.")
    ENDIF (FFTW3_LIBRARY)
ENDIF (UNIX)

ADD_DEFINITIONS(-DFLOAT_DSP)

SET(Libraries ${GTK_LIBRARIES} ${OTHER_LIBRARIES})
SET(CurrentExe "gnuitar")

ADD_EXECUTABLE(${CurrentExe}
    src/amp.c src/main.c src/pump.c src/chorus.c
    src/delay.c src/echo.c src/tracker.c src/tremolo.c src/vibrato.c src/autowah.c
    src/phasor.c src/rcfilter.c src/tubeamp.c src/rotary.c src/gui.c src/distort.c
    src/distort2.c src/sustain.c src/reverb.c src/backbuf.c src/utils.c src/amp.h
    src/pump.h src/chorus.h src/delay.h src/echo.h src/tracker.h src/tremolo.h
    src/vibrato.h src/autowah.h src/phasor.h src/rcfilter.h src/tubeamp.h
    src/rotary.h src/gui.h src/distort.h src/distort2.h src/sustain.h src/reverb.h
    src/backbuf.h src/utils.h src/noise.c src/noise.h src/biquad.c src/biquad.h
    src/eqbank.c src/eqbank.h src/tuner.c src/tuner.h src/audio-alsa.c
    src/audio-alsa.h src/audio-oss.c src/audio-oss.h src/pitch.c src/pitch.h
    src/fft.c src/fft.h src/glib12-compat.c src/glib12-compat.h src/audio-jack.c
    src/audio-jack.h src/audio-windows.c)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

