# Microsoft Developer Studio Project File - Name="FileZilla" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FileZilla - Win32 Debug Unicode
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "FileZilla.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "FileZilla.mak" CFG="FileZilla - Win32 Debug Unicode"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "FileZilla - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "FileZilla - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE "FileZilla - Win32 Debug Unicode" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FileZilla - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /Zi /O2 /I "misc" /I "openssl" /I "openssl\.." /D "NDEBUG" /D "KRB5" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "FILEZILLA" /FAcs /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib ws2_32.lib /nologo /subsystem:windows /map /debug /machine:I386 /OPT:REF
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "FileZilla - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /Zi /Od /I "misc" /I "openssl" /I "openssl\.." /D "_DEBUG" /D "KRB5" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "FILEZILLA" /D "FZ_NO_SDK_INSTALLED" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib ws2_32.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "FileZilla - Win32 Debug Unicode"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug Unicode"
# PROP BASE Intermediate_Dir "Debug Unicode"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug_Unicode"
# PROP Intermediate_Dir "Debug_Unicode"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MTd /W3 /Gm /Gi /GX /ZI /Od /I "misc" /I "openssl" /I "openssl\.." /D "_DEBUG" /D "_UNICODE" /D "FILEZILLA" /D "KRB5" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 Shlwapi.lib version.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 version.lib ws2_32.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "FileZilla - Win32 Release"
# Name "FileZilla - Win32 Debug"
# Name "FileZilla - Win32 Debug Unicode"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\misc\CBase64Coding.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\ComboCompletion.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\CoolBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\hookwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\HyperLink.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\Led.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\MailMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\MarkupSTL.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\MinTrayBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\SAPrefsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\SAPrefsStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\SAPrefsSubDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\SBDestination.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\SystemTray.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\VisualStylesXP.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\WheatyExceptionReport.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ApiLog.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncGssSocketLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncProxySocketLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncRequestQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncSocketEx.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncSocketExLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\AsyncSslSocketLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\BrowseForFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\ControlSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\Crypt.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectoryCache.cpp
# End Source File
# Begin Source File

SOURCE=.\DirTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\EnterSomething.cpp
# End Source File
# Begin Source File

SOURCE=.\FileAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\FileExistsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FileZilla.cpp
# End Source File
# Begin Source File

SOURCE=.\FileZilla.rc

!IF  "$(CFG)" == "FileZilla - Win32 Release"

!ELSEIF  "$(CFG)" == "FileZilla - Win32 Debug"

!ELSEIF  "$(CFG)" == "FileZilla - Win32 Debug Unicode"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FileZillaApi.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpControlSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpListResult.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\FtpView.cpp
# End Source File
# Begin Source File

SOURCE=.\FzApiStructures.cpp
# End Source File
# Begin Source File

SOURCE=.\IdentServerControl.cpp
# End Source File
# Begin Source File

SOURCE=.\IdentServerDataSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalComboCompletion.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalFileListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalView.cpp
# End Source File
# Begin Source File

SOURCE=.\LocalView2.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainThread.cpp
# End Source File
# Begin Source File

SOURCE=.\ManualTransferDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MFC64bitFix.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsConnection2.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDebugPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsDirCachePage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsFirewallPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsGssPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsIdentPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsInterfacePage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsLanguagePage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsLocalViewPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsLoggingPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsMiscPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPaneLayoutPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsProxyPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsRemoteViewPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSpeedLimitPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSshPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsTransferPage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsTypePage.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsViewEditPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PathFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\QueueView.cpp
# End Source File
# Begin Source File

SOURCE=.\RemoteComboCompletion.cpp
# End Source File
# Begin Source File

SOURCE=.\RetryServerList.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerChangePass.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerPath.cpp
# End Source File
# Begin Source File

SOURCE=.\SftpControlSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\SFtpIpc.cpp
# End Source File
# Begin Source File

SOURCE=.\SiteManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SiteManagerAdvancedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeedLimit.cpp
# End Source File
# Begin Source File

SOURCE=.\SpeedLimitRuleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\splitex.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StatusView.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\structures.cpp
# End Source File
# Begin Source File

SOURCE=.\TextProgressCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\TransferAsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TransferSocket.cpp
# End Source File
# Begin Source File

SOURCE=.\misc\TransparentDialogBar.cpp
# ADD CPP /I ".."
# End Source File
# Begin Source File

SOURCE=.\VerifyCertDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\version.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ApiLog.h
# End Source File
# Begin Source File

SOURCE=.\AsyncGssSocketLayer.h
# End Source File
# Begin Source File

SOURCE=.\AsyncProxySocketLayer.h
# End Source File
# Begin Source File

SOURCE=.\AsyncRequestQueue.h
# End Source File
# Begin Source File

SOURCE=.\AsyncSocketEx.h
# End Source File
# Begin Source File

SOURCE=.\AsyncSocketExLayer.h
# End Source File
# Begin Source File

SOURCE=.\AsyncSslSocketLayer.h
# End Source File
# Begin Source File

SOURCE=.\misc\ComboCompletion.h
# End Source File
# Begin Source File

SOURCE=.\CommandQueue.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\ControlSocket.h
# End Source File
# Begin Source File

SOURCE=.\Crypt.h
# End Source File
# Begin Source File

SOURCE=.\DirectoryCache.h
# End Source File
# Begin Source File

SOURCE=.\DirTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\EnterSomething.h
# End Source File
# Begin Source File

SOURCE=.\FileAttributes.h
# End Source File
# Begin Source File

SOURCE=.\FileExistsDlg.h
# End Source File
# Begin Source File

SOURCE=.\FileZilla.h
# End Source File
# Begin Source File

SOURCE=.\FileZillaApi.h
# End Source File
# Begin Source File

SOURCE=.\FtpControlSocket.h
# End Source File
# Begin Source File

SOURCE=.\FtpListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FtpListResult.h
# End Source File
# Begin Source File

SOURCE=.\FtpTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\FtpTreeView.h
# End Source File
# Begin Source File

SOURCE=.\FtpView.h
# End Source File
# Begin Source File

SOURCE=.\FzApiStructures.h
# End Source File
# Begin Source File

SOURCE=.\GSSAsyncSocksifiedSocket.h
# End Source File
# Begin Source File

SOURCE=.\IdentServerControl.h
# End Source File
# Begin Source File

SOURCE=.\IdentServerDataSocket.h
# End Source File
# Begin Source File

SOURCE=.\LocalComboCompletion.h
# End Source File
# Begin Source File

SOURCE=.\LocalFileListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\LocalView.h
# End Source File
# Begin Source File

SOURCE=.\LocalView2.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MainThread.h
# End Source File
# Begin Source File

SOURCE=.\ManualTransferDlg.h
# End Source File
# Begin Source File

SOURCE=.\MFC64bitFix.h
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\OptionsConnection.h
# End Source File
# Begin Source File

SOURCE=.\OptionsConnection2.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDebugPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsDirCachePage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsFirewallPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsGssPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsIdentPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsInterfacePage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsLanguagePage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsLocalViewPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsLoggingPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsMiscPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPaneLayoutPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsProxyPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsRemoteViewPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSpeedLimitPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSshPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsTransferPage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsTypePage.h
# End Source File
# Begin Source File

SOURCE=.\OptionsViewEditPage.h
# End Source File
# Begin Source File

SOURCE=.\PathFunctions.h
# End Source File
# Begin Source File

SOURCE=.\QueueCtrl.h
# End Source File
# Begin Source File

SOURCE=.\QueueView.h
# End Source File
# Begin Source File

SOURCE=.\RemoteComboCompletion.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RetryServerList.h
# End Source File
# Begin Source File

SOURCE=.\ServerChangePass.h
# End Source File
# Begin Source File

SOURCE=.\ServerPath.h
# End Source File
# Begin Source File

SOURCE=.\SFtpCommandIDs.h
# End Source File
# Begin Source File

SOURCE=.\SftpControlSocket.h
# End Source File
# Begin Source File

SOURCE=.\SFtpIpc.h
# End Source File
# Begin Source File

SOURCE=.\SiteManager.h
# End Source File
# Begin Source File

SOURCE=.\SiteManagerAdvancedDlg.h
# End Source File
# Begin Source File

SOURCE=.\SpeedLimit.h
# End Source File
# Begin Source File

SOURCE=.\SpeedLimitRuleDlg.h
# End Source File
# Begin Source File

SOURCE=.\splitex.h
# End Source File
# Begin Source File

SOURCE=.\StatusCtrl.h
# End Source File
# Begin Source File

SOURCE=.\StatusView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\structures.h
# End Source File
# Begin Source File

SOURCE=.\TextProgressCtrl.h
# End Source File
# Begin Source File

SOURCE=.\TransferAsDlg.h
# End Source File
# Begin Source File

SOURCE=.\TransferSocket.h
# End Source File
# Begin Source File

SOURCE=.\misc\TransparentDialogBar.h
# End Source File
# Begin Source File

SOURCE=.\VerifyCertDlg.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\certificate.ico
# End Source File
# Begin Source File

SOURCE=.\res\default1.bin
# End Source File
# Begin Source File

SOURCE=.\res\donate.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down.ico
# End Source File
# Begin Source File

SOURCE=.\res\empty.ico
# End Source File
# Begin Source File

SOURCE=.\res\FileZilla.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\leds.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Graphics\LUNA_BLUE.BMP
# End Source File
# Begin Source File

SOURCE=.\res\Luna_Blue.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Graphics\LUNA_HOMESTEAD.BMP
# End Source File
# Begin Source File

SOURCE=.\res\Luna_Homestead.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Graphics\LUNA_METALLIC.BMP
# End Source File
# Begin Source File

SOURCE=.\res\Luna_Metallic.bmp
# End Source File
# Begin Source File

SOURCE=.\res\remotetree.bmp
# End Source File
# Begin Source File

SOURCE=.\res\secure.ico
# End Source File
# Begin Source File

SOURCE=.\res\sitemanagertree.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\trayicon.ico
# End Source File
# Begin Source File

SOURCE=.\res\up.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\FzSFtp\Release\FzSFtp.exe

!IF  "$(CFG)" == "FileZilla - Win32 Release"

# Begin Custom Build - Copy FzSFtp.exe to output folder
OutDir=.\Release
InputPath=.\FzSFtp\Release\FzSFtp.exe

"$(OutDir)\FzSFtp.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist FzSFtp\Release\FzSFtp.exe copy FzSFtp\Release\FzSFtp.exe $(OutDir)\FzSFtp.exe

# End Custom Build

!ELSEIF  "$(CFG)" == "FileZilla - Win32 Debug"

# Begin Custom Build - Copy FzSFtp.exe to output folder
OutDir=.\Debug
InputPath=.\FzSFtp\Release\FzSFtp.exe

"$(OutDir)\FzSFtp.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist FzSFtp\Release\FzSFtp.exe copy FzSFtp\Release\FzSFtp.exe $(OutDir)\FzSFtp.exe

# End Custom Build

!ELSEIF  "$(CFG)" == "FileZilla - Win32 Debug Unicode"

# Begin Custom Build - Copy FzSFtp.exe to output folder
OutDir=.\Debug_Unicode
InputPath=.\FzSFtp\Release\FzSFtp.exe

"$(OutDir)\FzSFtp.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist FzSFtp\Release\FzSFtp.exe copy FzSFtp\Release\FzSFtp.exe "$(OutDir)\FzSFtp.exe"

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\res\manifest.xml
# End Source File
# End Target
# End Project
