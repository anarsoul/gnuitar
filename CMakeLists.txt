# An experimental CMake build system for gnuitar.
#
# $Id$
#
# $Log$
# Revision 1.27  2006/08/10 18:52:07  alankila
# - declare prototypes properly
# - hide some accidentally global methods
#
# Revision 1.26  2006/08/10 16:18:36  alankila
# - improve const correctness and make gnuitar compile cleanly under
#   increasingly pedantic warning models.
#
# Revision 1.25  2006/08/10 13:57:48  alankila
# - use fftw3f instead of fftw3 to avoid slower doubles
#
# Revision 1.24  2006/08/08 21:05:31  alankila
# - optimize gnuitar: this breaks dsound, I'll fix it later
#
# Revision 1.23  2006/08/07 13:20:42  alankila
# - group all effects through effect.h rather than enumerating them in
#   pump.c.
#
# Revision 1.22  2006/08/07 12:55:30  alankila
# - construct audio-driver.c to hold globals and provide some utility
#   functions to its users. This slashes interdependencies somewhat.
#
# Revision 1.21  2006/08/06 20:14:54  alankila
# - split pump.h into several domain-specific headers to reduce file
#   interdependencies (everyone included pump.h). New files are:
#   - effect.h for effect definitions
#   - audio-driver.h for work relating to audio drivers
#   - audio-midi.h for MIDI interaction.
#
# Revision 1.20  2006/08/06 01:18:34  alankila
# - pass the linker option --as-needed to reduce crazy dependencies
#
# Revision 1.19  2006/07/27 18:31:15  alankila
# - split dsound and winmm into separate drivers.
#
# Revision 1.18  2006/07/27 00:45:57  alankila
# - get rid of GTK_ENABLE_BROKEN -- no more gtk_text_new() for GTK2.
#
# Revision 1.17  2006/07/26 18:08:38  alankila
# - implement various compile fixes for mingw
#
# Revision 1.16  2006/07/24 23:18:43  alankila
# - enable use of autovectorisation, turn on sse2 along with sse.
# - in reality we should detect host system type and only use options that
#   the host system is capable of.
#
# Revision 1.15  2006/07/23 20:33:02  alankila
# - bah with this cmake BUILD TYPE. I'm forcing RELEASE now.
#
# Revision 1.14  2006/07/19 11:11:49  alankila
# - don't pass paths to FIND_LIBRARY: standard paths will do
#
# Revision 1.13  2006/07/19 11:01:43  alankila
# - install more docs
#
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

# user serviceable parts begin

SET(CMAKE_BUILD_TYPE RELEASE)
SET(OPT_FLAGS "-ffast-math -ftree-vectorize -march=athlon64 -mfpmath=sse")
ADD_DEFINITIONS(-DFLOAT_DSP)

# end of user serviceable parts

MESSAGE("Note: I am assuming that these parameters generate valid code for you.\n${OPT_FLAGS}\n")

INCLUDE(UsePkgConfig)
PKGCONFIG(gtk+-2.0    GTK2_INCLUDE_DIR    GTK2_LINK_DIR    GTK2_LINK_FLAGS    GTK2_CFLAGS)
PKGCONFIG(gthread-2.0 GTHREAD_INCLUDE_DIR GTHREAD_LINK_DIR GTHREAD_LINK_FLAGS GTHREAD_CFLAGS)

IF (GTK2_INCLUDE_DIR)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GTK2_CFLAGS} ${GTHREAD_CFLAGS}")
    SET(GTK_LIBRARIES ${GTK2_LINK_FLAGS} ${GTHREAD_LINK_FLAGS})
    ADD_DEFINITIONS(-DHAVE_GTK2)
    MESSAGE("GTK 2.x found and used as GUI\n")
    SET(GTK2_FOUND 1)
ENDIF (GTK2_INCLUDE_DIR)

IF (NOT GTK2_FOUND)
    FIND_PACKAGE(GTK)
    IF (GTK_FOUND)
        INCLUDE_DIRECTORIES(${GTK_INCLUDE_DIR})
        ADD_DEFINITIONS(-DHAVE_GTK)
        MESSAGE("GTK 1.x found and used as GUI\n")
    ENDIF (GTK_FOUND)
ENDIF (NOT GTK2_FOUND)

IF (NOT GTK_FOUND AND NOT GTK2_FOUND)
    MESSAGE(FATAL_ERROR "Please install gtk 2.x or gtk 1.x development files")
ENDIF (NOT GTK_FOUND AND NOT GTK2_FOUND)

SET(OTHER_LIBRARIES m)
IF (UNIX)
    # for vasprintf
    ADD_DEFINITIONS(-D_GNU_SOURCE)

    # OSS is always available on Unix
    ADD_DEFINITIONS(-DHAVE_OSS)

    # these libraries are dynamically detected.
    # probably pkg-config should be used for these as well.
    FIND_LIBRARY(JACK_LIBRARY NAMES jack)
    IF (JACK_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_JACK)
        SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} jack)
    ELSE (JACK_LIBRARY)
        MESSAGE("libjack not found -- not compiling jack support.")
    ENDIF (JACK_LIBRARY)

    FIND_LIBRARY(ALSA_LIBRARY NAMES asound)
    IF (ALSA_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_ALSA)
        SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} asound)
    ELSE (ALSA_LIBRARY)
        MESSAGE("libasound not found -- not compiling ALSA support.")
    ENDIF (ALSA_LIBRARY)

    # sndfile is only required for unix
    FIND_LIBRARY(SNDFILE_LIBRARY NAMES sndfile)
    IF (SNDFILE_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_SNDFILE)
        SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} sndfile)
    ELSE (SNDFILE_LIBRARY)
        MESSAGE("libsndfile not found -- .wav export not possible.")
    ENDIF (SNDFILE_LIBRARY)
ENDIF (UNIX)

IF (WIN32)
    FIND_LIBRARY(DSOUND_LIBRARY NAMES dsound)
    IF (DSOUND_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_DSOUND)
        SET (OTHER_LIBRARIES ${OTHER_LIBRARIES} dsound dxguid)
    ELSE (DSOUND_LIBRARY)
        MESSAGE("dsound not found -- disabling DirectSound output.")
    ENDIF (DSOUND_LIBRARY)

    FIND_LIBRARY(WINMM_LIBRARY NAMES winmm)
    IF (WINMM_LIBRARY)
        ADD_DEFINITIONS(-DHAVE_WINMM)
        SET (OTHER_LIBRARIES ${OTHER_LIBRARIES} winmm)
    ELSE (WINMM_LIBRARY)
        MESSAGE("winmm not found -- disabling MMSystem output.")
    ENDIF (WINMM_LIBRARY)

    IF (NOT DSOUND_LIBRARY AND NOT WINMM_LIBRARY)
        MESSAGE(FATAL_ERROR "Neither DirectSound or WinMM libraries could be found for audio playback.")
    ENDIF (NOT DSOUND_LIBRARY AND NOT WINMM_LIBRARY)
ENDIF (WIN32)

FIND_LIBRARY(FFTW3_LIBRARY NAMES fftw3f)
IF (FFTW3_LIBRARY)
    ADD_DEFINITIONS(-DHAVE_FFTW3)
    SET(OTHER_LIBRARIES ${OTHER_LIBRARIES} fftw3f)
ELSE (FFTW3_LIBRARY)
    MESSAGE("libfftw3f not found -- using time-domain implementations where possible.")
ENDIF (FFTW3_LIBRARY)

# set final C flags
SET(WARNING_FLAGS "-Wall -Wunsafe-loop-optimizations -Wbad-function-cast -Wcast-align -Wmissing-field-initializers -Wmissing-noreturn -Wredundant-decls -Winline -Wpointer-sign -Wpointer-arith -Wmissing-prototypes")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARNING_FLAGS} ${OPT_FLAGS}")

# setup complete: define build targets and build files
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
    src/audio-winmm.h src/audio-winmm.c src/audio-dsound.h src/audio-dsound.c
    src/audio-driver.h src/audio-driver.c src/audio-midi.h src/effect.c src/effect.h)
SET_TARGET_PROPERTIES(gnuitar PROPERTIES LINK_FLAGS "-Wl,--as-needed")
TARGET_LINK_LIBRARIES(gnuitar ${GTK_LIBRARIES} ${OTHER_LIBRARIES})

INSTALL(TARGETS gnuitar RUNTIME DESTINATION bin)
MAKE_DIRECTORY("${CMAKE_INSTALL_PREFIX}/share/doc/gnuitar/docs")
INSTALL(FILES AUTHORS NEWS README TODO FAQ COPYING ChangeLog
        DESTINATION share/doc/gnuitar)
INSTALL(FILES docs/devdocs.txt docs/faq.html docs/index.html docs/install.html
        docs/readme.html
        DESTINATION share/doc/gnuitar/docs)
