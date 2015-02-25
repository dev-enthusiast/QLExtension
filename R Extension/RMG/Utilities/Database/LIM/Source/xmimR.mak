# Microsoft Developer Studio Generated NMAKE File, Based on xmimR.dsp
!IF "$(CFG)" == ""
CFG=xmimR - Win32 Release
!MESSAGE No configuration specified. Defaulting to xmimR - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "xmimR - Win32 Release" && "$(CFG)" != "xmimR - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xmimR.mak" CFG="xmimR - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xmimR - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xmimR - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe /IH:\user\R\RMG\InterfaceR\Lim\xmims\LimLibs
# Header file, xmim_api.h
# Library file, xmim4.lib
# should be located in the directory defined above
# Those files are available from LIM Server
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "xmimR - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\xmimR.dll"

!ELSE 

ALL : "$(OUTDIR)\xmimR.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\xmimR.obj"
	-@erase "$(OUTDIR)\xmimR.dll"
	-@erase "$(OUTDIR)\xmimR.exp"
	-@erase "$(OUTDIR)\xmimR.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\xmimR.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xmimR.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=H:\user\R\RMG\InterfaceR\Lim\xmims\LimLibs\xmim4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:no\
 /pdb:"$(OUTDIR)\xmimR.pdb" /machine:I386 /def:".\xmimR.def"\
 /out:"$(OUTDIR)\xmimR.dll" /implib:"$(OUTDIR)\xmimR.lib" 
DEF_FILE= \
	".\xmimR.def"
LINK32_OBJS= \
	"$(INTDIR)\xmimR.obj"

"$(OUTDIR)\xmimR.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
#	copy $(OUTDIR)\xmimR.dll d:\splus45\cmd

!ELSEIF  "$(CFG)" == "xmimR - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\xmimR.dll"

!ELSE 

ALL : "$(OUTDIR)\xmimR.dll"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(INTDIR)\xmimR.obj"
	-@erase "$(OUTDIR)\xmimR.dll"
	-@erase "$(OUTDIR)\xmimR.exp"
	-@erase "$(OUTDIR)\xmimR.ilk"
	-@erase "$(OUTDIR)\xmimR.lib"
	-@erase "$(OUTDIR)\xmimR.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)\xmimR.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xmimR.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=H:\user\R\RMG\InterfaceR\Lim\xmims\LimLibs\xmim4.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes\
 /pdb:"$(OUTDIR)\xmimR.pdb" /debug /machine:I386 /def:".\xmimR.def"\
 /out:"$(OUTDIR)\xmimR.dll" /implib:"$(OUTDIR)\xmimR.lib" /pdbtype:sept 
DEF_FILE= \
	".\xmimR.def"
LINK32_OBJS= \
	"$(INTDIR)\xmimR.obj"

"$(OUTDIR)\xmimR.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
#	copy $(OUTDIR)\xmimR.dll d:\splus45\cmd

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "xmimR - Win32 Release" || "$(CFG)" == "xmimR - Win32 Debug"
SOURCE=.\xmimR.c

"$(INTDIR)\xmimR.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

