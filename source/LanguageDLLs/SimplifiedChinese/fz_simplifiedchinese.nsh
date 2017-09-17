;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Thunder White (mailto:thunder@cumt.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_SIMPCHINESE}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Simplified Chinese"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "��׼��װ"
LangString InstTypeFullName ${CURLANG} "��ȫ��װ"
LangString SecFileZillaName ${CURLANG} "FileZilla (����)"
LangString SecDocumentationName ${CURLANG} "�����ļ�"
LangString SecEnglishDocumentationName ${CURLANG} "Ӣ������ļ�"
LangString SecFrenchDocumentationName ${CURLANG} "��������ļ�"
LangString SecLangFilesName ${CURLANG} "�������԰�"
LangString SecSourceCodeName ${CURLANG} "Դ����"
LangString SecStartMenuName ${CURLANG} "��ʼ�˵���ݷ�ʽ"
LangString SecDesktopIconName ${CURLANG} "����ͼ��"
LangString SecDebugName ${CURLANG} "�����ļ�"

LangString SecFileZillaDesc ${CURLANG} "��װ FileZilla.exe ������������ļ�"
LangString SecDocumentationDesc ${CURLANG} "�����ĵ���ָ����λ��"
LangString SecEnglishDocumentationDesc ${CURLANG} "��װӢ�İ����ļ�"
LangString SecFrenchDocumentationDesc ${CURLANG} "���ز���װӢ�İ����ļ�"
LangString SecLangFilesDesc ${CURLANG} "���� FileZilla �����԰��ļ���ָ����λ��"
LangString SecSourceCodeDesc ${CURLANG} "����Դ���򲢸��Ƶ�ָ��λ��."
LangString SecStartMenuDesc ${CURLANG} "�ڿ�ʼ�˵��д�����ݷ�ʽ"
LangString SecDesktopIconDesc ${CURLANG} "������Ϊ FileZilla ������ݷ�ʽ"
LangString SecDebugDesc ${CURLANG} "�����������������������ĵ����ļ�"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "һ������"
LangString PageSettingsSubTitle ${CURLANG} "���ð�ȫģʽ�Լ����õı���λ��"
LangString PageDownloadTitle ${CURLANG} "����ѡ��"
LangString PageDownloadSubTitle ${CURLANG} "��Ҫ����һЩ���"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "��ϣ���԰�ȫģʽ���� ${MUI_PRODUCT} ô��\n�ڰ�ȫģʽ�£�FileZilla �����ס�û��������롣\n����ж���ʹ����̨������������ѡ��ʹ�ð�ȫģʽ��"
LangString UseSecureMode ${CURLANG} "ʹ�ð�ȫģʽ"
LangString DontUseSecureMode ${CURLANG} "��ʹ�ð�ȫģʽ"
LangString StorageQuestion ${CURLANG} "FileZilla �ܹ������ñ�����������ͬ�ĵط���ע���� XML �ļ���\r\nһ�㽨��ʹ�� XML �ļ���\r\n����㹤����ÿ���û������Լ��������ļ��Ķ��û���������Ӧ��ѡ��ʹ��ע����Ա���ÿ���û������Լ��� FileZilla ���á�"
LangString UseXML ${CURLANG} "ʹ�� XML �ļ�(&X)"
LangString UseRegistry ${CURLANG} "ʹ��ע���(&R)"
LangString ForceUseRegistry ${CURLANG} "��ʹ XML �ļ�����Ҳǿ��ʹ��ע���(&F)"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Ϊ�˾���װ���򣬱���װ������û�а������������"
LangString DownloadDialog3 ${CURLANG} "Ҫ��װ��Щ�������װ�������������Ҫ�ĵ��ļ�����������ڲ���������Щ�ļ������Ժ�����ٴ����д˰�װ���򣬻�����Ҳ���Դ� http://filezilla.sourceforge.net �ֹ�������Щ�ļ���"
LangString DownloadDialog4 ${CURLANG} "�ӻ��������ز���װ"
LangString DownloadDialog5 ${CURLANG} "������Щ���"
LangString DownloadDialog6 ${CURLANG} "ʹ�ñ��ص����Ŀ¼ (������������)��"
LangString DownloadDialog8 ${CURLANG} "���ʹ�����Ŀ¼����װ���򽫰����ص������ļ����������Ŀ¼��������������Ŀ¼��û�е���Щ�ļ���"

;Download strings
LangString DownloadDownloading ${CURLANG} "�������� $1"
LangString DownloadDownloadFailed ${CURLANG} "  ����ʧ�ܣ�$0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  ����ȡ��" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "���� $0 ʧ�ܡ�ԭ��$R1$\n����������ᱻ��װ��$\nҪ���������������ٴ����а�װ����$\n���������Ȼ���ڣ������ܰ�װ�����޷����ʻ�������������ʲôԭ���㶼���Դ� http://filezilla.sourceforge.net �ֶ�������Щ�ļ���"
LangString DownloadDownloadSuccessful ${CURLANG} "  ���سɹ�" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "��ѹ�� $0 ʧ�ܡ�ԭ��$R1$\n����������ᱻ��װ��"

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "ȷʵҪɾ�� FileZilla ����������ע�����ô(�����ѡ���˱�����ע�����վ��������еļ�¼Ҳ����һ��ɾ��)��"

!verbose 4