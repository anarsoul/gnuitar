# Microsoft Developer Studio Generated NMAKE File, Based on gnuitar.dsp
!IF "$(CFG)" == ""
CFG=gnuitar - Win32 Debug
!MESSAGE No configuration specified. Defaulting to gnuitar - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "gnuitar - Win32 Release" && "$(CFG)" != "gnuitar - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gnuitar.mak" CFG="gnuitar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gnuitar - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gnuitar - Win32 Debug" (based on "Win32 (x86) Console Application")
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
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\distort.obj"
	-@erase "$(INTDIR)\echo.obj"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\phasor.obj"
	-@erase "$(INTDIR)\pump.obj"
	-@erase "$(INTDIR)\rcfilter.obj"
	-@erase "$(INTDIR)\reverb.obj"
	-@erase "$(INTDIR)\sustain.obj"
	-@erase "$(INTDIR)\tracker.obj"
	-@erase "$(INTDIR)\tremolo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vibrato.obj"
	-@erase "$(OUTDIR)\gnuitar.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gnuitar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\gnuitar.pdb" /machine:I386 /out:"$(OUTDIR)\gnuitar.exe" 
LINK32_OBJS= \
	"$(INTDIR)\vibrato.obj" \
	"$(INTDIR)\backbuf.obj" \
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
	"$(INTDIR)\autowah.obj"

"$(OUTDIR)\gnuitar.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

OUTDIR=.\gnuitar___Win32_Debug
INTDIR=.\gnuitar___Win32_Debug
# Begin Custom Macros
OutDir=.\gnuitar___Win32_Debug
# End Custom Macros

ALL : "$(OUTDIR)\gnuitar.exe"


CLEAN :
	-@erase "$(INTDIR)\autowah.obj"
	-@erase "$(INTDIR)\backbuf.obj"
	-@erase "$(INTDIR)\chorus.obj"
	-@erase "$(INTDIR)\delay.obj"
	-@erase "$(INTDIR)\distort.obj"
	-@erase "$(INTDIR)\echo.obj"
	-@erase "$(INTDIR)\gui.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\phasor.obj"
	-@erase "$(INTDIR)\pump.obj"
	-@erase "$(INTDIR)\rcfilter.obj"
	-@erase "$(INTDIR)\reverb.obj"
	-@erase "$(INTDIR)\sustain.obj"
	-@erase "$(INTDIR)\tracker.obj"
	-@erase "$(INTDIR)\tremolo.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vibrato.obj"
	-@erase "$(OUTDIR)\gnuitar.exe"
	-@erase "$(OUTDIR)\gnuitar.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\gnuitar.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\gnuitar.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib libglib-2.0-0.dll libgmodule-2.0-0.dll libgobject-2.0-0.dll libgthread-2.0-0.dll libgdk-0.dll libgtk-0.dll libintl-1.dll libglib-2.0.dll.a libgmodule-2.0.dll.a libgobject-2.0.dll.a libgthread-2.0.dll.a libgdk.dll.a libgtk.dll.a /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\gnuitar.pdb" /machine:I386 /out:"$(OUTDIR)\gnuitar.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\vibrato.obj" \
	"$(INTDIR)\backbuf.obj" \
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
	"$(INTDIR)\autowah.obj"

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


!IF "$(CFG)" == "gnuitar - Win32 Release" || "$(CFG)" == "gnuitar - Win32 Debug"
SOURCE=.\src\autowah.c

"$(INTDIR)\autowah.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\backbuf.c

"$(INTDIR)\backbuf.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\chorus.c

"$(INTDIR)\chorus.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\delay.c

"$(INTDIR)\delay.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\distort.c

"$(INTDIR)\distort.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\echo.c

"$(INTDIR)\echo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\gui.c

"$(INTDIR)\gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\main.c

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\phasor.c

"$(INTDIR)\phasor.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\pump.c

"$(INTDIR)\pump.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\rcfilter.c

"$(INTDIR)\rcfilter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\reverb.c

"$(INTDIR)\reverb.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\sustain.c

"$(INTDIR)\sustain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\tracker.c

"$(INTDIR)\tracker.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\tremolo.c

"$(INTDIR)\tremolo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\src\vibrato.c

"$(INTDIR)\vibrato.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

