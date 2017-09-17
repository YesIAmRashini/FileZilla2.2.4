# Microsoft Developer Studio Project File - Name="FzSFtp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FzSFtp - Win32 Release
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "FzSFtp.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "FzSFtp.mak" CFG="FzSFtp - Win32 Release"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "FzSFtp - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /map /debug /machine:I386
# Begin Target

# Name "FzSFtp - Win32 Release"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BE_NONE.C
# End Source File
# Begin Source File

SOURCE=.\cmdline.c
# End Source File
# Begin Source File

SOURCE=.\CONSOLE.C
# End Source File
# Begin Source File

SOURCE=.\FzSFtp.rc
# End Source File
# Begin Source File

SOURCE=.\FzSFtpIpc.cpp
# End Source File
# Begin Source File

SOURCE=.\FzSFtpIpcClient.cpp
# End Source File
# Begin Source File

SOURCE=.\INT64.C
# End Source File
# Begin Source File

SOURCE=.\MISC.C
# End Source File
# Begin Source File

SOURCE=.\NOISE.C
# End Source File
# Begin Source File

SOURCE=.\PAGEANTC.C
# End Source File
# Begin Source File

SOURCE=.\PSFTP.C
# End Source File
# Begin Source File

SOURCE=.\SETTINGS.C
# End Source File
# Begin Source File

SOURCE=.\SFTP.C
# End Source File
# Begin Source File

SOURCE=.\SSH.C
# End Source File
# Begin Source File

SOURCE=.\SSHAES.C
# End Source File
# Begin Source File

SOURCE=.\SSHBLOWF.C
# End Source File
# Begin Source File

SOURCE=.\SSHBN.C
# End Source File
# Begin Source File

SOURCE=.\SSHCRC.C
# End Source File
# Begin Source File

SOURCE=.\SSHCRCDA.C
# End Source File
# Begin Source File

SOURCE=.\SSHDES.C
# End Source File
# Begin Source File

SOURCE=.\SSHDH.C
# End Source File
# Begin Source File

SOURCE=.\SSHDSS.C
# End Source File
# Begin Source File

SOURCE=.\SSHMD5.C
# End Source File
# Begin Source File

SOURCE=.\SSHPUBK.C
# End Source File
# Begin Source File

SOURCE=.\SSHRAND.C
# End Source File
# Begin Source File

SOURCE=.\SSHRSA.C
# End Source File
# Begin Source File

SOURCE=.\SSHSH512.C
# End Source File
# Begin Source File

SOURCE=.\SSHSHA.C
# End Source File
# Begin Source File

SOURCE=.\SSHZLIB.C
# End Source File
# Begin Source File

SOURCE=.\TREE234.C
# End Source File
# Begin Source File

SOURCE=.\VERSION.C
# End Source File
# Begin Source File

SOURCE=.\wildcard.c
# End Source File
# Begin Source File

SOURCE=.\windefs.c
# End Source File
# Begin Source File

SOURCE=.\winmisc.c
# End Source File
# Begin Source File

SOURCE=.\WINNET.C
# End Source File
# Begin Source File

SOURCE=.\winsftp.c
# End Source File
# Begin Source File

SOURCE=.\WINSTORE.C
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FzSFtpIpc.h
# End Source File
# Begin Source File

SOURCE=.\FzSFtpIpcClient.h
# End Source File
# Begin Source File

SOURCE=.\INT64.H
# End Source File
# Begin Source File

SOURCE=.\MISC.H
# End Source File
# Begin Source File

SOURCE=.\NETWORK.H
# End Source File
# Begin Source File

SOURCE=.\PUTTY.H
# End Source File
# Begin Source File

SOURCE=.\PUTTYMEM.H
# End Source File
# Begin Source File

SOURCE=.\SFTP.H
# End Source File
# Begin Source File

SOURCE=.\SSH.H
# End Source File
# Begin Source File

SOURCE=.\STORAGE.H
# End Source File
# Begin Source File

SOURCE=.\TREE234.H
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\SCP.ICO
# End Source File
# Begin Source File

SOURCE=.\SCP.ICO
# End Source File
# End Group
# End Target
# End Project
