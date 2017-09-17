;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Yuan Chia Fu (mailto:fu391@ms10.hinet.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_TRADCHINESE}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Traditional Chinese"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "�зǦw��"
LangString InstTypeFullName ${CURLANG} "����w��"
LangString SecFileZillaName ${CURLANG} "FileZilla (�򥻲ե�)"
LangString SecDocumentationName ${CURLANG} "�������"
LangString SecEnglishDocumentationName ${CURLANG} "�^�廡�����"
LangString SecFrenchDocumentationName ${CURLANG} "�k�廡�����"
LangString SecLangFilesName ${CURLANG} "�B�~�y���]"
LangString SecSourceCodeName ${CURLANG} "�{����l�X"
LangString SecStartMenuName ${CURLANG} "�}�l�{�������"
LangString SecDesktopIconName ${CURLANG} "�ୱ�ϥ�"
LangString SecDebugName ${CURLANG} "�����{��"

LangString SecFileZillaDesc ${CURLANG} "�w�� FileZilla.exe �M��L���n���ɮ�"
LangString SecDocumentationDesc ${CURLANG} "�ƻs��������{���ؿ�"
LangString SecEnglishDocumentationDesc ${CURLANG} "�w�˭^�廡�����"
LangString SecFrenchDocumentationDesc ${CURLANG} "�U���æw�˭^�廡�����"
LangString SecLangFilesDesc ${CURLANG} "�ƻs FileZilla ���B�~�y���]��{���ؿ�"
LangString SecSourceCodeDesc ${CURLANG} "�U���ýƻs�{����l�X��{���ؿ�"
LangString SecStartMenuDesc ${CURLANG} "�إ߿���}�l�{����"
LangString SecDesktopIconDesc ${CURLANG} "�b�ୱ�W�s�W�@�� FileZilla �����|"
LangString SecDebugDesc ${CURLANG} "�ƻs�����{���A�o�ǰ����{����b FileZilla �o�� crash �ɹ�{���i��ѪR�P���U"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "�򥻳]�w"
LangString PageSettingsSubTitle ${CURLANG} "�]�w�w���Ҧ��H�γ]�w���x�s��m"
LangString PageDownloadTitle ${CURLANG} "�U���ﶵ"
LangString PageDownloadSubTitle ${CURLANG} "�ݭn�U���@�Ǥ���"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "�A�Ʊ� ${MUI_PRODUCT} �B�@�b�w���Ҧ��H\n�b�w���Ҧ������AFileZilla ���|�۰ʰO�Х��󪺱K�X�C\n�p�G�o�x�q�����ܦh�H�b�ϥΡA��ĳ�A��� 'Yes'�C"
LangString UseSecureMode ${CURLANG} "�ϥΦw���Ҧ�"
LangString DontUseSecureMode ${CURLANG} "���ϥΦw���Ҧ�"
LangString StorageQuestion ${CURLANG} "FileZilla �����]�w�x�s�b registry �� XML �ɮפ��C\r\n�p�G�A�O�b�h�H���Ҥ��ϥΡA��ĳ�A�H registry ���x�s�]�w�C\r\n�_�h�A�ڭ̫�ĳ�j�������ϥΪ̿�ܨϥ� XML �ɮר��x�s�]�w�C"
LangString UseXML ${CURLANG} "�ϥ� XML �ɮר��x�s�]�w(&X)"
LangString UseRegistry ${CURLANG} "�ϥ� registry ���x�s�]�w(&r)"
LangString ForceUseRegistry ${CURLANG} "���� XML �ɮ׬O�_�s�b���j���H registry ���x�s�]�w(&F)"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "���F���w�˵{�����²�A�U�C����N�S���]�t�b�w�˵{����:"
LangString DownloadDialog3 ${CURLANG} "���F�w�˳o�Ǥ���A�w�˵{���N�|�U���o�ǥ��n���ɮסC�p�G�A���@�N�{�b�U���o���ɮסA�A�i�H�H��A����w�˵{���A�Ϊ̦ۤv�� http://filezilla.sourceforge.net �������U���C"
LangString DownloadDialog4 ${CURLANG} "�q Internet �i��U���M�w��"
LangString DownloadDialog5 ${CURLANG} "���L�o�Ǥ���"
LangString DownloadDialog6 ${CURLANG} "�ϥΥ������M��ؿ� (�п�J�b�U��):"
LangString DownloadDialog8 ${CURLANG} "��ϥγo�ӮM��ؿ��ɡA�w�˵{���|��Ҧ��U���^�Ӫ��ɮצs�J�o�ӥؿ����C�Ӧw�˵{���]�|�ˬd�M��ؿ��A�p�G�ݭn���ɮרS���b�M��ؿ������A�~�|�U���o�ǨS�����ɮסC"

;Download strings
LangString DownloadDownloading ${CURLANG} "$1 �U����..."
LangString DownloadDownloadFailed ${CURLANG} "  �U������: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  �U������" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "�U�� $0 ���ѡC��]: $R1$\n�o�Ӥ���N���|�Q�w�ˡC$\n�A�i�H�y��A����@���w�˵{���A���s�U���o�Ӥ���C$\n�p�G�o�ӿ��~�̵M�s�b�A�]�\�O�]���w�˵{���L�k�s�W Internet�C���ެO�����]�A�A���i�H�q http://filezilla.sourceforge.net �������U���o�Ǥ���C"
LangString DownloadDownloadSuccessful ${CURLANG} "  �U�����\" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "�N $0 �����e�����Y���ѡC��]: $R1$\n�o�Ӥ���N���|�Q�w�ˡC"

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "�R���Ҧ��� FileZilla �إߪ� Registry �]�w (�p�G�A�O�H Registry ���x�s�]�w�A����u���x�޲z���v���������]�|�@�_�Q�R��)?"

!verbose 4