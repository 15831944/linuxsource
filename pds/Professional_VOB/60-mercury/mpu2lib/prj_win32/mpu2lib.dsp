# Microsoft Developer Studio Project File - Name="mpu2lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mpu2lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mpu2lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpu2lib.mak" CFG="mpu2lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpu2lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mpu2lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "mpu2lib"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mpu2lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mpu2lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_MEDIACTRL_DM816x_" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\10-common\lib\debug\win32_816x\mpu2lib.lib"

!ENDIF 

# Begin Target

# Name "mpu2lib - Win32 Release"
# Name "mpu2lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\main.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mpu2bas.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mpu2struct.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mpu2utility.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mpu2vmp.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mpu2vmphwtest.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\evmpu2.h
# End Source File
# Begin Source File

SOURCE=..\include\mpu2baserr.h
# End Source File
# Begin Source File

SOURCE=..\include\mpu2inst.h
# End Source File
# Begin Source File

SOURCE=..\include\mpu2struct.h
# End Source File
# Begin Source File

SOURCE=..\include\mpu2utility.h
# End Source File
# Begin Source File

SOURCE=..\include\mpu2vmphwtest.h
# End Source File
# End Group
# End Target
# End Project
