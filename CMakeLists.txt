# An experimental CMake build system for gnuitar.
#
# $Id$
#
# $Log$
# Revision 1.5  2006/07/17 19:06:41  alankila
# - add some stock optimising settings
# - fix copypaste snafu that caused jack=asound
#
# Revision 1.4  2006/07/17 14:21:56  alankila
# - SEPARATE_ARGUMENTS() already does the " " => ";" translation, so I rather
#   use it. PKGCONFIG macros still suck, though.
# - make GTK2 missing error message fatal.
#
# Revision 1.3  2006/07/17 12:46:38  alankila
# - use pkg-config and huge pile of kludges to compile with GTK+ 2.0.
#   I guess CMake isn't that great after all, as pkg-config is not platform
#   independent.
#
# Revision 1.2  2006/07/17 11:31:06  alankila
# - move fftw3 for platform-independent inclusion, add some comments
#
# Revision 1.1  2006/07/17 11:26:17  alankila
# - commit an experimental exit from autotools hell
#

PROJECT(GNUitar)
INCLUDE(${CMAKE_ROOT}/Modules/UsePkgConfig.cmake)

# GTK+ is mandatory
FIND_PACKAGE(GTK)
IF (GTK_FOUND)
    INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
    ADD_DEFINITIONS(-DHAVE_GTK)
    MESSAGE("GTK 1.x found and used as GUI\n")
ELSE (GTK_FOUND)
    # This is a huge, quivering pile of poo. I guess CMake sucks.
    # There is obviously no way this will work on Windows. ;-(

    # PKGCONFIG doesn't exist on cmake 2.2 but on cmake 2.4 earliest.
    # I need to fix this, at the moment I'm compiling with GTK1 only.
    PKGCONFIG(gtk+-2.0 GTK2_INCLUDE_DIR GTK2_LINK_DIR GTK2_LINK_FLAGS GTK2_CFLAGS)
    PKGCONFIG(gthread-2.0 GTHREAD_INCLUDE_DIR GTHREAD_LINK_DIR GTHREAD_LINK_FLAGS GTHREAD_CFLAGS)
    
    IF (GTK2_INCLUDE_DIR)
        # ok, I can get compile work through CFLAGS
        SET(CMAKE_C_FLAGS "${GTK2_CFLAGS} ${GTHREAD_CFLAGS}")
        # but to include libraries I have to do these awful hacks
        # remove -l from in front of library
        STRING(REGEX REPLACE "-l" "" GTK_LIBRARIES "${GTK2_LINK_FLAGS} ${GTHREAD_LINK_FLAGS}")
        SEPARATE_ARGUMENTS(GTK_LIBRARIES)
        ADD_DEFINITIONS(-DHAVE_GTK2)
        MESSAGE("GTK 2.x found and used as GUI\n")
    ELSE (GTK2_INCLUDE_DIR)
        MESSAGE(FATAL_ERROR "GTK or GTK2 is mandatory -- make will fail. :-(")
    ENDIF (GTK2_INCLUDE_DIR)
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
        NAMES asound
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
ENDIF (UNIX)

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

# default to DSP & SSE
ADD_DEFINITIONS(-DFLOAT_DSP)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -O2 -fomit-frame-pointer -ffast-math")

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

