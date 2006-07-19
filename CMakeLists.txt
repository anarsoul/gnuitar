# An experimental CMake build system for gnuitar.
#
# $Id$
#
# $Log$
# Revision 1.12  2006/07/19 10:48:50  alankila
# - make cmake prefer gtk2 gui
#
# Revision 1.11  2006/07/19 10:34:04  alankila
# - update install to cmake 2.4 style
#
# Revision 1.10  2006/07/17 22:01:59  alankila
# - various compile fixes
#
# Revision 1.9  2006/07/17 21:39:38  alankila
# - use dynamically allocated sample buffers instead of static ones.
#   (Win32 still uses static buffers moved directly into audio-windows.c)
#
# Revision 1.8  2006/07/17 20:23:06  alankila
# - set a default build type
#
# Revision 1.7  2006/07/17 20:02:00  alankila
# - win32 should work too now
#
# Revision 1.6  2006/07/17 19:59:51  alankila
# - first stab at install targets
#
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

# force optimizings on... We put yet more optimizing options later.
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)

IF (UNIX)
    INCLUDE(UsePkgConfig)
    # This doesn't work on Windows, so I need to come up with something for that OS.
    # (As soon as I understand what...)
    PKGCONFIG(gtk+-2.0    GTK2_INCLUDE_DIR    GTK2_LINK_DIR    GTK2_LINK_FLAGS    GTK2_CFLAGS)
    PKGCONFIG(gthread-2.0 GTHREAD_INCLUDE_DIR GTHREAD_LINK_DIR GTHREAD_LINK_FLAGS GTHREAD_CFLAGS)

    IF (GTK2_INCLUDE_DIR)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GTK2_CFLAGS} ${GTHREAD_CFLAGS}")
        SET(GTK_LIBRARIES ${GTK2_LINK_FLAGS} ${GTHREAD_LINK_FLAGS})
        ADD_DEFINITIONS(-DHAVE_GTK2)
        MESSAGE("GTK 2.x found and used as GUI\n")
        SET(GTK2_FOUND 1)
    ENDIF (GTK2_INCLUDE_DIR)
ENDIF(UNIX)

IF (NOT GTK2_FOUND)
    FIND_PACKAGE(GTK)
    IF (GTK_FOUND)
        INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
        ADD_DEFINITIONS(-DHAVE_GTK)
        MESSAGE("GTK 1.x found and used as GUI\n")
    ENDIF (GTK_FOUND)
ENDIF (NOT GTK2_FOUND)

IF (GTK_FOUND OR GTK2_FOUND)
ELSE (GTK_FOUND OR GTK2_FOUND)
    MESSAGE(FATAL_ERROR "Please install gtk 2.x or gtk 1.x development files")
ENDIF (GTK_FOUND OR GTK2_FOUND)

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
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -ffast-math -Wall")

# for vasprintf
IF (UNIX)
    ADD_DEFINITIONS(-D_GNU_SOURCE)
ENDIF (UNIX)

ADD_EXECUTABLE(gnuitar
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
    src/glib12-compat.c src/glib12-compat.h src/audio-jack.c src/audio-jack.h
    src/audio-windows.c)
TARGET_LINK_LIBRARIES(gnuitar ${GTK_LIBRARIES} ${OTHER_LIBRARIES})

INSTALL(TARGETS gnuitar RUNTIME DESTINATION bin)
MAKE_DIRECTORY("${CMAKE_INSTALL_PREFIX}/share/doc/gnuitar/docs")
INSTALL(FILES docs/devdocs.txt docs/faq.html docs/index.html docs/install.html
        docs/readme.html
        DESTINATION share/doc/gnuitar/docs)
