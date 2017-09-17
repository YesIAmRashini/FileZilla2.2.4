;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Kim Jinwook

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_KOREAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Korean"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "�⺻"
LangString InstTypeFullName ${CURLANG} "Full"
LangString SecFileZillaName ${CURLANG} "FileZilla (�ʼ�)"
LangString SecDocumentationName ${CURLANG} "����"
LangString SecEnglishDocumentationName ${CURLANG} "���� ����"
LangString SecFrenchDocumentationName ${CURLANG} "�������� ����"
LangString SecLangFilesName ${CURLANG} "�߰����� ��� ���ϵ�"
LangString SecSourceCodeName ${CURLANG} "�ҽ� �ڵ�"
LangString SecStartMenuName ${CURLANG} "���� �޴� �ٷΰ���"
LangString SecDesktopIconName ${CURLANG} "����ȭ�� ������"
LangString SecDebugName ${CURLANG} "����� ���ϵ�"

LangString SecFileZillaDesc ${CURLANG} "FileZilla.exe �� �ٸ� �ʼ� ���ϵ��� ��ġ�մϴ�."
LangString SecDocumentationDesc ${CURLANG} "��ġ�� ������ �������� �����մϴ�."
LangString SecEnglishDocumentationDesc ${CURLANG} "���� ������ ��ġ�մϴ�"
LangString SecFrenchDocumentationDesc ${CURLANG} "���� ������ �ٿ�ε� �� ��ġ�մϴ�."
LangString SecLangFilesDesc ${CURLANG} "��ġ ������ fileZilla �� ��� ���ϵ��� �����մϴ�."
LangString SecSourceCodeDesc ${CURLANG} "��ġ ������ �ҽ��ڵ带 �ٿ�ε� �� �����մϴ�."
LangString SecStartMenuDesc ${CURLANG} "���� �޴��� �ٷΰ��⸦ �����մϴ�."
LangString SecDesktopIconDesc ${CURLANG} "FileZilla �� ����ȭ�� �ٷΰ��⸦ �����մϴ�."
LangString SecDebugDesc ${CURLANG} "������� ���� ���� ����� ���ϵ��� �����մϴ�."
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "�Ϲ� ����"
LangString PageSettingsSubTitle ${CURLANG} "���� ���� ���� ������ġ ����"
LangString PageDownloadTitle ${CURLANG} "�ٿ�ε� �ɼǵ�"
LangString PageDownloadSubTitle ${CURLANG} "Ư�� ������Ʈ���� �ٿ�ε� �Ǿ�� ��"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "${MUI_PRODUCT} �� ���� ��忡�� �����Ͻðڽ��ϱ�?\r\n���� ��忡���� FileZilla �� � ��ȣ�� ������� �ʽ��ϴ�.\r\n���� �������� �� PC �� ���ٸ� ���� ��带 ����ϱ⸦ �����մϴ�."
LangString UseSecureMode ${CURLANG} "���� ��� ���(&U)"
LangString DontUseSecureMode ${CURLANG} "���� ��� ��� ����(&N)"
LangString StorageQuestion ${CURLANG} "FileZilla �� ������ XML ���ϰ� ������Ʈ�� �� ���� ������ �� �ֽ��ϴ�.\r\n��κ��� ��� XML �� ���� ���� �����մϴ�.\r\n���� ������ ���������� �̿��ϴ� ���� ����� ȯ���̶��, ������ ������ ���� ������ҷ� ������Ʈ���� ����մϴ�."
LangString UseXML ${CURLANG} "XML ���� ���(&X)"
LangString UseRegistry ${CURLANG} "������Ʈ�� ���(&R)"
LangString ForceUseRegistry ${CURLANG} "XML ������ �̹� �ִ��� ������Ʈ���� ���(&F)"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "�Ʒ��� ������Ʈ���� �ν��緯�� ũ�⸦ ���̱� ���ؼ� �ν��緯�� ���Ե��� �ʾҽ��ϴ�:"
LangString DownloadDialog3 ${CURLANG} "�� ������Ʈ���� ��ġ�ϱ� ���ؼ��� �ν��緯�� �ʿ��� ���ϵ��� �ٿ�ε��� ���Դϴ�. ���� ���� ���ϵ��� �ٿ�ε��ϱ⸦ ������ �ʴ´ٸ�, ���߿� �ν��緯�� �����ų �� �ֽ��ϴ�. �׷��� ������ http://filezilla.sourceforge.net ���� �������� �ٿ���� �� �ֽ��ϴ�."
LangString DownloadDialog4 ${CURLANG} "���ͳ����� �ٿ�޾� ��ġ"
LangString DownloadDialog5 ${CURLANG} "�� ������Ʈ���� �ǳʶ�"
LangString DownloadDialog6 ${CURLANG} "���� ��Ű�� ���丮 ���(�Ʒ��� �Է�):"
LangString DownloadDialog8 ${CURLANG} "��Ű�� ���丮�� ����� ��, �ν��緯�� �Ʒ��� ���丮�� ��� �ٿ�ε�� ������ ������ ���Դϴ�. �׸��� ���� ��Ű���� �������� �ʴ� ���ϵ��� �ٿ�ε��� �� �Դϴ�."

;Download strings
LangString DownloadDownloading ${CURLANG} "$1 �ٿ�ε� ��"
LangString DownloadDownloadFailed ${CURLANG} "  �ٿ�ε� ����: $0" ;Do not remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  �ٿ�ε� ��ҵ�" ;Do not remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "$0 �� �ٿ�ε� �ϴ� �� ������. ����: $R1$\n�� ������Ʈ�� ��ġ���� ���� ���Դϴ�.$\n
�� ������Ʈ�� �ٽ� �ٿ�ε��ϱ� ���ؼ��� ���߿� �ٽ� �ν��緯�� �����Ű����.$\n
���� �� ������ ��� �ִٸ�, �ν��緯�� ���ͳݿ� ������ �� ���� ��� �Դϴ�. � ���� http://filezilla.sourceforge.net ���� 
���� �� ������Ʈ�� �ٿ���� �� �ֽ��ϴ�."
LangString DownloadDownloadSuccessful ${CURLANG} "  �ٿ�ε� ����" ;Do not remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "$0 �� �����ϴµ� �����߽��ϴ�. ����: $R1$\n�� ������Ʈ�� ��ġ���� ���� �� �Դϴ�."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "FileZilla �� ������ ��� ������Ʈ�� Ű�� �����մϱ�(����Ʈ ������ �׸��� ������ġ�� ������Ʈ���� �ߴٸ� ����Ʈ �������� ��� �׸� ���Ե˴ϴ�)?"

!verbose 4