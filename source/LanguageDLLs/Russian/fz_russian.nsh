;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by <insert your name here>

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_RUSSIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Russian"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "�����������"
LangString InstTypeFullName ${CURLANG} "������"
LangString SecFileZillaName ${CURLANG} "FileZilla (�����������)"
LangString SecDocumentationName ${CURLANG} "������������"
LangString SecEnglishDocumentationName ${CURLANG} "������������ �� ����������"
LangString SecFrenchDocumentationName ${CURLANG} "������������ �� �����������"
LangString SecLangFilesName ${CURLANG} "�������������� �������� �����"
LangString SecSourceCodeName ${CURLANG} "�������� ���� ��������"
LangString SecStartMenuName ${CURLANG} "������ � ������� ����"
LangString SecDesktopIconName ${CURLANG} "����� �� ������� �����"
LangString SecDebugName ${CURLANG} "���������� �����"

LangString SecFileZillaDesc ${CURLANG} "��������� FileZilla.exe � ������ ����������� ������"
LangString SecDocumentationDesc ${CURLANG} "����������� ������ ������������ � ��������� �����"
LangString SecEnglishDocumentationDesc ${CURLANG} "���������� ������������ �� ���������� �����"
LangString SecFrenchDocumentationDesc ${CURLANG} "��������� � ���������� ������������ �� ���������� �����"
LangString SecLangFilesDesc ${CURLANG} "���������� �������� ����� FileZilla � ��������� �����"
LangString SecSourceCodeDesc ${CURLANG} "��������� � ���������� �������� ���� �������� � ��������� �����"
LangString SecStartMenuDesc ${CURLANG} "������� ������ � ������� ����"
LangString SecDesktopIconDesc ${CURLANG} "�������� ����� ��� ������� FileZilla �� ������� ����"
LangString SecDebugDesc ${CURLANG} "���������� ���������� �����, ������� ������� �������� ������"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "����� ���������"
LangString PageSettingsSubTitle ${CURLANG} "���������������� ����������� ������ � ��������� ������������ ���������"
LangString PageDownloadTitle ${CURLANG} "��������� ��������"
LangString PageDownloadSubTitle ${CURLANG} "��������� ���������� ������ ���� ������������� ���������"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "��������� ${MUI_PRODUCT} � ���������� ������?\r\n� ���������� ������, FileZilla �� ���������� ������.\r\n���� ����� ������������� ���� �� ����������� �������� ��������� �������������."
LangString UseSecureMode ${CURLANG} "&������������ ���������� �����"
LangString DontUseSecureMode ${CURLANG} "&�� ������������ ���������� �����"
LangString StorageQuestion ${CURLANG} "FileZilla ����� ������� ��������� ����� ���������: � ������� � XML �����.\r\n��� ������ ������������ ������������� XML.\r\n���� �� ���������� �������� �� ���� ������������, �� ������������� ������� ��������� � �������, �.�. � XML �� ����� �������� ����� ����� ���������."
LangString UseXML ${CURLANG} "������������ &XML ����"
LangString UseRegistry ${CURLANG} "������������ &������"
LangString ForceUseRegistry ${CURLANG} "&��������� ��������� �� XML ����� � ������."

;Download dialog
LangString DownloadDialog1 ${CURLANG} "��������� ���������� ������� ��� ��������� ����� ���������� ��������� ������������:"
LangString DownloadDialog3 ${CURLANG} "��� ��������� ���� �����������, ��������� ��������� ������ ��������� ��. ���� �� �� ������ ��������� �� ������, �� ������ ��������� ��������� ��������� �����, ��� ������ ��������� ��� ����� ������� � http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "��������� � ���������� �� ��������"
LangString DownloadDialog5 ${CURLANG} "���������� ��� ����������"
LangString DownloadDialog6 ${CURLANG} "������������ ��������� ����� (������� �����):"
LangString DownloadDialog8 ${CURLANG} "����� ������������ ������������ �����, ��������� ��������� ��������� ����� � ���. ��� ���������� ������������ �����, ����� ����������� ����� ����������� � ������������ �����."

;Download strings
LangString DownloadDownloading ${CURLANG} "��������� $1"
LangString DownloadDownloadFailed ${CURLANG} "  �������� ����������: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  �������� ��������" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "������ ��� �������� $0. �������: $R1$\n���� ��������� �� ����� ���� ����������.$\n��������� ��������� ��������� ����� ����� �������� ����������� ������.$\n���� ������ �����������, �������� ��� ����������� � ��������. �� ������ ��������� ���� ��������� ������� � ����� http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  �������� ���������" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "������ ���������� ������ $0. �������: $R1$\n���� ��������� �� ����� ���� ����������."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "������� ��� ����� ������� ��������������� FileZilla (������� ����� ��������� ������ ���� �� ������������ ������ ��� �������� ��������)?"

!verbose 4
