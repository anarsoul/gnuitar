# Microsoft Developer Studio Project File - Name="gnuitar" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=gnuitar - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gnuitar.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gnuitar.mak" CFG="gnuitar - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gnuitar - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gnuitar - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "gnuitar - Win32 Demo" (based on "Win32 (x86) Console Application")
!MESSAGE "gnuitar - Win32 Release 586" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gnuitar - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "gnuitar - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "gnuitar___Win32_Debug"
# PROP BASE Intermediate_Dir "gnuitar___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib dsound.lib /nologo /version:0.20 /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /profile /pdb:none

!ELSEIF  "$(CFG)" == "gnuitar - Win32 Demo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gnuitar___Win32_Demo"
# PROP BASE Intermediate_Dir "gnuitar___Win32_Demo"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Demo"
# PROP Intermediate_Dir "Demo"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G6 /MT /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D DEMO=1 /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib dsound.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "gnuitar - Win32 Release 586"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "gnuitar___Win32_Release_586"
# PROP BASE Intermediate_Dir "gnuitar___Win32_Release_586"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release_586"
# PROP Intermediate_Dir "Release_586"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /MT /W3 /GX /O2 /Op /Ob2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib glib-2.0.lib gmodule-2.0.lib gobject-2.0.lib gthread-2.0.lib gdk.lib gtk.lib winmm.lib dsound.lib /nologo /subsystem:console /machine:I386

!ENDIF 

# Begin Target

# Name "gnuitar - Win32 Release"
# Name "gnuitar - Win32 Debug"
# Name "gnuitar - Win32 Demo"
# Name "gnuitar - Win32 Release 586"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\autowah.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\backbuf.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\chorus.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\delay.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\distort.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\distort2.c
# End Source File
# Begin Source File

SOURCE=.\src\echo.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\gui.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\main.c
# End Source File
# Begin Source File

SOURCE=.\src\noise.c
# End Source File
# Begin Source File

SOURCE=.\src\phasor.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\pump.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\rcfilter.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\reverb.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\sustain.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\tracker.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\tremolo.c
# ADD CPP /W3
# End Source File
# Begin Source File

SOURCE=.\src\utils.c
# End Source File
# Begin Source File

SOURCE=.\src\vibrato.c
# ADD CPP /W3
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\autowah.h
# End Source File
# Begin Source File

SOURCE=.\src\backbuf.h
# End Source File
# Begin Source File

SOURCE=.\src\chorus.h
# End Source File
# Begin Source File

SOURCE=.\src\delay.h
# End Source File
# Begin Source File

SOURCE=.\src\distort.h
# End Source File
# Begin Source File

SOURCE=.\src\distort2.h
# End Source File
# Begin Source File

SOURCE=.\src\echo.h
# End Source File
# Begin Source File

SOURCE=.\src\gui.h
# End Source File
# Begin Source File

SOURCE=.\src\noise.h
# End Source File
# Begin Source File

SOURCE=.\src\phasor.h
# End Source File
# Begin Source File

SOURCE=.\src\pump.h
# End Source File
# Begin Source File

SOURCE=.\src\rcfilter.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\reverb.h
# End Source File
# Begin Source File

SOURCE=.\src\sustain.h
# End Source File
# Begin Source File

SOURCE=.\src\tracker.h
# End Source File
# Begin Source File

SOURCE=.\src\tremolo.h
# End Source File
# Begin Source File

SOURCE=.\src\utils.h
# End Source File
# Begin Source File

SOURCE=.\src\vibrato.h
# End Source File
# End Group
# Begin Group "docs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Authors
# End Source File
# Begin Source File

SOURCE=.\ChangeLog
# End Source File
# Begin Source File

SOURCE=.\copying
# End Source File
# Begin Source File

SOURCE=.\Faq
# End Source File
# Begin Source File

SOURCE=.\docs\faq.html
# End Source File
# Begin Source File

SOURCE=.\docs\index.html
# End Source File
# Begin Source File

SOURCE=.\install
# End Source File
# Begin Source File

SOURCE=.\docs\install.html
# End Source File
# Begin Source File

SOURCE=.\docs\Makefile
# End Source File
# Begin Source File

SOURCE=.\News
# End Source File
# Begin Source File

SOURCE=.\readme
# End Source File
# Begin Source File

SOURCE=.\docs\readme.html
# End Source File
# Begin Source File

SOURCE=.\Testimonials
# End Source File
# Begin Source File

SOURCE=.\todo
# End Source File
# End Group
# Begin Group "UNIX configure"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aclocal.m4
# End Source File
# Begin Source File

SOURCE=.\configure
# End Source File
# Begin Source File

SOURCE=.\configure.in
# End Source File
# Begin Source File

SOURCE=.\gnuitar.spec
# End Source File
# Begin Source File

SOURCE=".\install-sh"
# End Source File
# Begin Source File

SOURCE=.\Makefile.am
# End Source File
# Begin Source File

SOURCE=.\Makefile.in
# End Source File
# Begin Source File

SOURCE=.\missing
# End Source File
# Begin Source File

SOURCE=.\mkinstalldirs
# End Source File
# End Group
# Begin Group "Effects"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\effects\HeyDay
# End Source File
# Begin Source File

SOURCE=.\effects\Phase_destruction
# End Source File
# Begin Source File

SOURCE=.\effects\Vibrato
# End Source File
# End Group
# Begin Group "Resources"

# PROP Default_Filter "*.rc"
# Begin Source File

SOURCE=.\gnuitar.ico
# End Source File
# Begin Source File

SOURCE=.\src\gnuitar.rc
# End Source File
# Begin Source File

SOURCE=.\gnuitar_small.ico
# End Source File
# End Group
# End Target
# End Project
