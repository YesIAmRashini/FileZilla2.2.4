;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by <Asparouh Kalyandjiev (acnapyx@computers.bg)>

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_BULGARIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Bulgarian"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "����������"
LangString InstTypeFullName ${CURLANG} "�����"
LangString SecFileZillaName ${CURLANG} "FileZilla (������������)"
LangString SecDocumentationName ${CURLANG} "������������"
LangString SecEnglishDocumentationName ${CURLANG} "English Documentation"
LangString SecFrenchDocumentationName ${CURLANG} "French Documentation"
LangString SecLangFilesName ${CURLANG} "������������ ������� �������"
LangString SecSourceCodeName ${CURLANG} "������� ���"
LangString SecStartMenuName ${CURLANG} "��������� � Start ������"
LangString SecDesktopIconName ${CURLANG} "����� �� ��������"
LangString SecDebugName ${CURLANG} "Debug files"

LangString SecFileZillaDesc ${CURLANG} "����������� �� FileZilla.exe � ������� ������������ �������"
LangString SecDocumentationDesc ${CURLANG} "�������� �� ��������� � �������������� � �������� �����"
LangString SecEnglishDocumentationDesc ${CURLANG} "Install the English documentation"
LangString SecFrenchDocumentationDesc ${CURLANG} "Download and install the English documentation"
LangString SecLangFilesDesc ${CURLANG} "�������� �� ��������� ������� �� FileZilla � �������� �����"
LangString SecSourceCodeDesc ${CURLANG} "��������� � �������� �� �������� ��� � �������� �����"
LangString SecStartMenuDesc ${CURLANG} "��������� �� ��������� � Start ������"
LangString SecDesktopIconDesc ${CURLANG} "�������� �� ����� �� �������� �� ���� ������ �� FileZilla"
LangString SecDebugDesc ${CURLANG} "Copy debug files which help to analyze crashes"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "General settings"
LangString PageSettingsSubTitle ${CURLANG} "Configure secure mode and settings storage"
LangString PageDownloadTitle ${CURLANG} "����� �� ���������"
LangString PageDownloadSubTitle ${CURLANG} "����� ���������� ������ �� ����� ��������� �� ��������"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "������� �� �� ���������� ${MUI_PRODUCT} � ������� �����?\n� ������� �����, FileZilla �� ������� ������� ������.\n�������������� � �� ���������� ���� �����, ��� ������� ���� ���� ������ �� ���� PC."
LangString UseSecureMode ${CURLANG} "Use secure mode"
LangString DontUseSecureMode ${CURLANG} "Do not use secure mode"
LangString StorageQuestion ${CURLANG} "FileZilla can store its settings in two different places: In the registry and in an XML file.\r\nIn most cases it is recommended to use the XML.\r\nIf you are in a multi-user environment where each user has its own profile, you should use the registry as storage location so that each user has it's own settings for FileZilla as well."
LangString UseXML ${CURLANG} "Use &XML file"
LangString UseRegistry ${CURLANG} "use &registry"
LangString ForceUseRegistry ${CURLANG} "&Force registry even if XML file exists"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "�������� ���������� �� �� �������� � ������������, �� �� �� ������ �������� �:"
LangString DownloadDialog3 ${CURLANG} "�� �� ����������� ���� ����������, ��������������� �������� ���� �� �� ������� �� ��������. ��� �� ������� �� �� ��������� ����, ������ �� ���������� ������������ ��-����� ��� �� �� ��������� ����� �� http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "��������� � ���������� �� ��������"
LangString DownloadDialog5 ${CURLANG} "�������� ���� ����������"
LangString DownloadDialog6 ${CURLANG} "��������� ������� ����� (�������� � ��-����):"
LangString DownloadDialog8 ${CURLANG} "��� ���������� ������� �����, ������������ �� ������ ������ ��������� ������� � ���� �����. ���� ���� �� �� ������� ���� ���������, ����� ���� �� ���������� � ���� �����."

;Download strings
LangString DownloadDownloading ${CURLANG} "��������� �� $1"
LangString DownloadDownloadFailed ${CURLANG} "  ����������� ��������: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  ����������� ����������" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "�������� ����������� �� $0. �������: $R1$\n���� ��������� ���� �� ���� ����������.$\n����������� ����������� ��-�����, �� �� ������� ������ �� ��������� ���� ���������.$\n��� �������� �� ������� ���������, �������� ������������ ���� ������ �� ��������. ������ �� ��������� ������������ � ����� �� http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  ����������� ������� �������" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "�������� ����������� �� ������������ �� $0. �������: $R1$\n���� ��������� ���� �� ���� ����������."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "�� ����� �� ������� ������ ������� �� ���������, ��������� �� FileZilla (���������� ������������ �� ��������� �� �������), ��� ��� ������� ��������� ���� ����� �� ���������� �� �����������?"

!verbose 4