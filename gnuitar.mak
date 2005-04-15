# Microsoft Developer Studio Generated NMAKE File, Based on gnuitar.dsp
!IF "$(CFG)" == ""
CFG=gnuitar - Win32 Release
!MESSAGE No configuration specified. Defaulting to gnuitar - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "gnuitar - Win32 Release" && "$(CFG)" != "gnuitar - Win32 Debug" && "$(CFG)" != "gnuitar - Win32 Demo" && "$(CFG)" != "gnuitar - Win32 Release 586"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gnuitar.mak" CFG="gnuitar - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gnuitar - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gnuitar - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "gnuitar - Win32 Demo" (based on "Win32 (x86) Console Application")
!MESSAGE "gnuitar - Win32 Release 586" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gnuitar - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\gnuitar.exe"


CLEAN :
	-@erase "$(INTDIR)\autowah.obj"
	-@erase "$(INTDIR)\backbuf.obj"
	-@erase "$(INTDIR)\chorus.obj"
	-@erase "$(INTDIR)\chebyshev.obj"
	-@erase "$(INTDIR)\biquad.obj"
	-@erase "$(INTDIR)\eqbank.obj"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\distort.obj"
	-@erase "$(INTDIR)\distort2.obj"
	-@erase "$(INTDIR)\echo.obj"
	-@erase "$(INTDIR)\gnuitar.res"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\noise.obj"
	-@erase "$(INTDIR)\phasor.obj"
	-@erase "$(INTDIR)\pump.obj"
	-@erase "$(INTDIR)\rcfilter.obj"
	-@erase "$(INTDIR)\reverb.obj"
	-@erase "$(INTDIR)\sustain.obj"
	-@erase "$(INTDIR)\tracker.obj"
	-@erase "$(INTDIR)\tremolo.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vibrato.obj"
	-@erase "$(OUTDIR)\gnuitar.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\gnuitar.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gnuitar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gnuitar.pdb" /machine:I386 /out:"$(OUTDIR)\gnuitar.exe" 
LINK32_OBJS= \
	"$(INTDIR)\autowah.obj" \
	"$(INTDIR)\backbuf.obj" \
	"$(INTDIR)\chorus.obj" \
	"$(INTDIR)\chebyshev.obj" \
	"$(INTDIR)\biquad.obj" \
	"$(INTDIR)\eqbank.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\distort.obj" \
	"$(INTDIR)\echo.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\phasor.obj" \
	"$(INTDIR)\pump.obj" \
	"$(INTDIR)\rcfilter.obj" \
	"$(INTDIR)\reverb.obj" \
	"$(INTDIR)\sustain.obj" \
	"$(INTDIR)\tracker.obj" \
	"$(INTDIR)\tremolo.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\vibrato.obj" \
	"$(INTDIR)\gnuitar.res" \
	"$(INTDIR)\distort2.obj" \
	"$(INTDIR)\noise.obj"

"$(OUTDIR)\gnuitar.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\gnuitar.exe" "$(OUTDIR)\gnuitar.bsc"


CLEAN :
	-@erase "$(INTDIR)\autowah.obj"
	-@erase "$(INTDIR)\autowah.sbr"
	-@erase "$(INTDIR)\backbuf.obj"
	-@erase "$(INTDIR)\backbuf.sbr"
	-@erase "$(INTDIR)\chorus.obj"
	-@erase "$(INTDIR)\chebyshev.obj"
	-@erase "$(INTDIR)\biquad.obj"
	-@erase "$(INTDIR)\eqbank.obj"
	-@erase "$(INTDIR)\chebyshev.sbr"
	-@erase "$(INTDIR)\biquad.sbr"
	-@erase "$(INTDIR)\eqbank.sbr"
	-@erase "$(INTDIR)\chorus.sbr"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\delay.sbr"
	-@erase "$(INTDIR)\distort.obj"
	-@erase "$(INTDIR)\distort.sbr"
	-@erase "$(INTDIR)\distort2.obj"
	-@erase "$(INTDIR)\distort2.sbr"
	-@erase "$(INTDIR)\echo.obj"
	-@erase "$(INTDIR)\echo.sbr"
	-@erase "$(INTDIR)\gnuitar.res"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\gui.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\noise.obj"
	-@erase "$(INTDIR)\noise.sbr"
	-@erase "$(INTDIR)\phasor.obj"
	-@erase "$(INTDIR)\phasor.sbr"
	-@erase "$(INTDIR)\pump.obj"
	-@erase "$(INTDIR)\pump.sbr"
	-@erase "$(INTDIR)\rcfilter.obj"
	-@erase "$(INTDIR)\rcfilter.sbr"
	-@erase "$(INTDIR)\reverb.obj"
	-@erase "$(INTDIR)\reverb.sbr"
	-@erase "$(INTDIR)\sustain.obj"
	-@erase "$(INTDIR)\sustain.sbr"
	-@erase "$(INTDIR)\tracker.obj"
	-@erase "$(INTDIR)\tracker.sbr"
	-@erase "$(INTDIR)\tremolo.obj"
	-@erase "$(INTDIR)\tremolo.sbr"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\utils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vibrato.obj"
	-@erase "$(INTDIR)\vibrato.sbr"
	-@erase "$(OUTDIR)\gnuitar.bsc"
	-@erase "$(OUTDIR)\gnuitar.exe"
	-@erase "$(OUTDIR)\gnuitar.ilk"
	-@erase "$(OUTDIR)\gnuitar.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\gnuitar.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gnuitar.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\autowah.sbr" \
	"$(INTDIR)\backbuf.sbr" \
	"$(INTDIR)\chebyshev.sbr" \
	"$(INTDIR)\biquad.sbr" \
	"$(INTDIR)\eqbank.sbr" \
	"$(INTDIR)\chorus.sbr" \
	"$(INTDIR)\delay.sbr" \
	"$(INTDIR)\distort.sbr" \
	"$(INTDIR)\echo.sbr" \
	"$(INTDIR)\gui.sbr" \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\phasor.sbr" \
	"$(INTDIR)\pump.sbr" \
	"$(INTDIR)\rcfilter.sbr" \
	"$(INTDIR)\reverb.sbr" \
	"$(INTDIR)\sustain.sbr" \
	"$(INTDIR)\tracker.sbr" \
	"$(INTDIR)\tremolo.sbr" \
	"$(INTDIR)\utils.sbr" \
	"$(INTDIR)\vibrato.sbr" \
	"$(INTDIR)\distort2.sbr" \
	"$(INTDIR)\noise.sbr"

"$(OUTDIR)\gnuitar.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib dsound.lib /nologo /version:0.20 /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\gnuitar.pdb" /debug /machine:I386 /out:"$(OUTDIR)\gnuitar.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\autowah.obj" \
	"$(INTDIR)\backbuf.obj" \
	"$(INTDIR)\chebyshev.obj" \
	"$(INTDIR)\biquad.obj" \
	"$(INTDIR)\eqbank.obj" \
	"$(INTDIR)\chorus.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\distort.obj" \
	"$(INTDIR)\echo.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\phasor.obj" \
	"$(INTDIR)\pump.obj" \
	"$(INTDIR)\rcfilter.obj" \
	"$(INTDIR)\reverb.obj" \
	"$(INTDIR)\sustain.obj" \
	"$(INTDIR)\tracker.obj" \
	"$(INTDIR)\tremolo.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\vibrato.obj" \
	"$(INTDIR)\gnuitar.res" \
	"$(INTDIR)\distort2.obj" \
	"$(INTDIR)\noise.obj"

"$(OUTDIR)\gnuitar.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

OUTDIR=.\Demo
INTDIR=.\Demo
# Begin Custom Macros
OutDir=.\Demo
# End Custom Macros

ALL : "$(OUTDIR)\gnuitar.exe"


CLEAN :
	-@erase "$(INTDIR)\autowah.obj"
	-@erase "$(INTDIR)\backbuf.obj"
	-@erase "$(INTDIR)\chorus.obj"
	-@erase "$(INTDIR)\chebyshev.obj"
	-@erase "$(INTDIR)\biquad.obj"
	-@erase "$(INTDIR)\eqbank.obj"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\distort.obj"
	-@erase "$(INTDIR)\distort2.obj"
	-@erase "$(INTDIR)\echo.obj"
	-@erase "$(INTDIR)\gnuitar.res"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\noise.obj"
	-@erase "$(INTDIR)\phasor.obj"
	-@erase "$(INTDIR)\pump.obj"
	-@erase "$(INTDIR)\rcfilter.obj"
	-@erase "$(INTDIR)\reverb.obj"
	-@erase "$(INTDIR)\sustain.obj"
	-@erase "$(INTDIR)\tracker.obj"
	-@erase "$(INTDIR)\tremolo.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vibrato.obj"
	-@erase "$(OUTDIR)\gnuitar.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\gnuitar.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gnuitar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib dsound.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gnuitar.pdb" /machine:I386 /out:"$(OUTDIR)\gnuitar.exe" 
LINK32_OBJS= \
	"$(INTDIR)\autowah.obj" \
	"$(INTDIR)\backbuf.obj" \
	"$(INTDIR)\chebyshev.obj" \
	"$(INTDIR)\biquad.obj" \
	"$(INTDIR)\eqbank.obj" \
	"$(INTDIR)\chorus.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\distort.obj" \
	"$(INTDIR)\echo.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\phasor.obj" \
	"$(INTDIR)\pump.obj" \
	"$(INTDIR)\rcfilter.obj" \
	"$(INTDIR)\reverb.obj" \
	"$(INTDIR)\sustain.obj" \
	"$(INTDIR)\tracker.obj" \
	"$(INTDIR)\tremolo.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\vibrato.obj" \
	"$(INTDIR)\gnuitar.res" \
	"$(INTDIR)\distort2.obj" \
	"$(INTDIR)\noise.obj"

"$(OUTDIR)\gnuitar.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

OUTDIR=.\Release_586
INTDIR=.\Release_586
# Begin Custom Macros
OutDir=.\Release_586
# End Custom Macros

ALL : "$(OUTDIR)\gnuitar.exe"


CLEAN :
	-@erase "$(INTDIR)\autowah.obj"
	-@erase "$(INTDIR)\backbuf.obj"
	-@erase "$(INTDIR)\chorus.obj"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\distort.obj"
	-@erase "$(INTDIR)\distort2.obj"
	-@erase "$(INTDIR)\chebyshev.obj"
	-@erase "$(INTDIR)\biquad.obj"
	-@erase "$(INTDIR)\eqbank.obj"
	-@erase "$(INTDIR)\echo.obj"
	-@erase "$(INTDIR)\gnuitar.res"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\noise.obj"
	-@erase "$(INTDIR)\phasor.obj"
	-@erase "$(INTDIR)\pump.obj"
	-@erase "$(INTDIR)\rcfilter.obj"
	-@erase "$(INTDIR)\reverb.obj"
	-@erase "$(INTDIR)\sustain.obj"
	-@erase "$(INTDIR)\tracker.obj"
	-@erase "$(INTDIR)\tremolo.obj"
	-@erase "$(INTDIR)\utils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vibrato.obj"
	-@erase "$(OUTDIR)\gnuitar.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\gnuitar.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gnuitar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib dsound.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gnuitar.pdb" /machine:I386 /out:"$(OUTDIR)\gnuitar.exe" 
LINK32_OBJS= \
	"$(INTDIR)\autowah.obj" \
	"$(INTDIR)\backbuf.obj" \
	"$(INTDIR)\chebyshev.obj" \
	"$(INTDIR)\biquad.obj" \
	"$(INTDIR)\eqbank.obj" \
	"$(INTDIR)\chorus.obj" \
	"$(INTDIR)\delay.obj" \
	"$(INTDIR)\distort.obj" \
	"$(INTDIR)\echo.obj" \
	"$(INTDIR)\gui.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\phasor.obj" \
	"$(INTDIR)\pump.obj" \
	"$(INTDIR)\rcfilter.obj" \
	"$(INTDIR)\reverb.obj" \
	"$(INTDIR)\sustain.obj" \
	"$(INTDIR)\tracker.obj" \
	"$(INTDIR)\tremolo.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\vibrato.obj" \
	"$(INTDIR)\gnuitar.res" \
	"$(INTDIR)\distort2.obj" \
	"$(INTDIR)\noise.obj"

"$(OUTDIR)\gnuitar.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("gnuitar.dep")
!INCLUDE "gnuitar.dep"
!ELSE 
!MESSAGE Warning: cannot find "gnuitar.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "gnuitar - Win32 Release" || "$(CFG)" == "gnuitar - Win32 Debug" || "$(CFG)" == "gnuitar - Win32 Demo" || "$(CFG)" == "gnuitar - Win32 Release 586"
SOURCE=.\src\autowah.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\autowah.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\autowah.obj"	"$(INTDIR)\autowah.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\autowah.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\autowah.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\backbuf.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\backbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\backbuf.obj"	"$(INTDIR)\backbuf.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\backbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\backbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\chorus.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chorus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\chorus.obj"	"$(INTDIR)\chorus.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chorus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chorus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\delay.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\delay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\delay.obj"	"$(INTDIR)\delay.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\delay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\delay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\distort.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\distort.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\distort.obj"	"$(INTDIR)\distort.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\distort.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\distort.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\distort2.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"


"$(INTDIR)\distort2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"


"$(INTDIR)\distort2.obj"	"$(INTDIR)\distort2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"


"$(INTDIR)\distort2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"


"$(INTDIR)\distort2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\echo.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\echo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\echo.obj"	"$(INTDIR)\echo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\echo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\echo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\gui.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\gui.obj"	"$(INTDIR)\gui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\main.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\noise.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"


"$(INTDIR)\noise.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"


"$(INTDIR)\noise.obj"	"$(INTDIR)\noise.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"


"$(INTDIR)\noise.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"


"$(INTDIR)\noise.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\phasor.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\phasor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\phasor.obj"	"$(INTDIR)\phasor.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\phasor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\phasor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\pump.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pump.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\pump.obj"	"$(INTDIR)\pump.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pump.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\pump.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\rcfilter.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\rcfilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\rcfilter.obj"	"$(INTDIR)\rcfilter.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\rcfilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\rcfilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\reverb.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\reverb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\reverb.obj"	"$(INTDIR)\reverb.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\reverb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\reverb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\sustain.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sustain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\sustain.obj"	"$(INTDIR)\sustain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sustain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\sustain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\tracker.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tracker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\tracker.obj"	"$(INTDIR)\tracker.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tracker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tracker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\tremolo.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tremolo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\tremolo.obj"	"$(INTDIR)\tremolo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tremolo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\tremolo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\utils.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"


"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"


"$(INTDIR)\utils.obj"	"$(INTDIR)\utils.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"


"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"


"$(INTDIR)\utils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\src\vibrato.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\vibrato.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\vibrato.obj"	"$(INTDIR)\vibrato.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\vibrato.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\vibrato.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\biquad.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biquad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\biquad.obj"	"$(INTDIR)\biquad.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biquad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biquad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\eqbank.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eqbank.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\eqbank.obj"	"$(INTDIR)\eqbank.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eqbank.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\eqbank.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\biquad.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biquad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\biquad.obj"	"$(INTDIR)\biquad.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biquad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biquad.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\src\chebyshev.c

!IF  "$(CFG)" == "gnuitar - Win32 Release"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chebyshev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "HAVE_GTK" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\chebyshev.obj"	"$(INTDIR)\chebyshev.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

CPP_SWITCHES=/nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chebyshev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "HAVE_GTK" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\chebyshev.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


SOURCE=.\src\gnuitar.rc

!IF  "$(CFG)" == "gnuitar - Win32 Release"


"$(INTDIR)\gnuitar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\gnuitar.res" /i "src" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"


"$(INTDIR)\gnuitar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\gnuitar.res" /i "src" /d "_DEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"


"$(INTDIR)\gnuitar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\gnuitar.res" /i "src" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"


"$(INTDIR)\gnuitar.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\gnuitar.res" /i "src" /d "NDEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

