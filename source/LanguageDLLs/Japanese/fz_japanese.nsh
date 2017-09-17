;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Takashi Yamaguchi (mailto:jack@omakase.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_JAPANESE}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Japanese"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "�W��"
LangString InstTypeFullName ${CURLANG} "���ׂ�"
LangString SecFileZillaName ${CURLANG} "FileZilla (�K�v�ł�)"
LangString SecDocumentationName ${CURLANG} "�h�L�������g"
LangString SecEnglishDocumentationName ${CURLANG} "�p��̃h�L�������g"
LangString SecFrenchDocumentationName ${CURLANG} "�t�����X��̃h�L�������g"
LangString SecLangFilesName ${CURLANG} "�ǉ��̌���t�@�C��"
LangString SecSourceCodeName ${CURLANG} "�\�[�X�R�[�h"
LangString SecStartMenuName ${CURLANG} "�X�^�[�g���j���[�փV���b�g�J�b�g�ǉ�"
LangString SecDesktopIconName ${CURLANG} "�f�B�X�N�g�b�v�A�C�R���ǉ�"
LangString SecDebugName ${CURLANG} "�f�o�b�N�p�t�@�C��"

LangString SecFileZillaDesc ${CURLANG} "FileZilla.exe�Ƒ��̕K�v�ȃt�@�C�����C���X�g�[�����܂��B"
LangString SecDocumentationDesc ${CURLANG} "�C���X�g�[������ꏊ�Ƀh�L�������g�t�@�C�����R�s�[���܂��B"
LangString SecEnglishDocumentationDesc ${CURLANG} "�p��̃h�L�������g���C���X�g�[������B"
LangString SecFrenchDocumentationDesc ${CURLANG} "�_�E�����[�h�Ɖp��̃h�L�������g���C���X�g�[������B"
LangString SecLangFilesDesc ${CURLANG} "�C���X�g�[������ꏊ��FileZilla�̌���t�@�C�����R�s�[���܂��B"
LangString SecSourceCodeDesc ${CURLANG} "�C���X�g�[������ꏊ�Ƀ\�[�X�R�[�h���_�E�����[�h���ăR�s�[���܂��B"
LangString SecStartMenuDesc ${CURLANG} "�X�^�[�g���j���[�փV���[�g�J�b�g���쐬���܂��B"
LangString SecDesktopIconDesc ${CURLANG} "FileZilla���ȒP�ɋN���ł���悤�Ƀf�B�X�N�g�b�v�ɃA�C�R����ǉ����܂��B"
LangString SecDebugDesc ${CURLANG} "�N���b�V������͂���ׂɕK�v�ȃf�o�b�N�t�@�C�����R�s�[���܂��B"

;Page titles
LangString PageSettingsTitle ${CURLANG} "��ʐݒ�"
LangString PageSettingsSubTitle ${CURLANG} "���S�ȃ��[�h�Ɛݒ���e���i�[�ꏊ��ݒ肵�Ă�������"
LangString PageDownloadTitle ${CURLANG} "�_�E�����[�h�I�v�V����"
LangString PageDownloadSubTitle ${CURLANG} "�������̃R���|�[�l���g���_�E�����[�h���܂��B"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "���Ȃ��͈��S�ȃ��[�h��${MUI_PRODUCT}���N�������܂���?\n���S�ȃ��[�h�ł́AFileZilla�͂��ׂẴp�X���[�h���L�����܂���.\n�����̐l���A����PC�Ɏg�p����ꍇ�ɂ͂����߂ł��B"
LangString UseSecureMode ${CURLANG} "���S�ȃ��[�h���g�p����"
LangString DontUseSecureMode ${CURLANG} "���S�ȃ��[�h���g�p���Ȃ�"
LangString StorageQuestion ${CURLANG} FileZilla�̓��W�X�g����XML�t�@�C���ɐݒ��ۑ����鎖���o���܂�:�قƂ�ǂ̏ꍇ�AXML���g�p���鎖�𐄏����܂��B\r\n�������Ȃ����}���`���[�U�[�����g�p���Ă��Ċe���[�U�[�̃v���t�@�C�������̂ł���΁A���W�X�g�����g�p���鎖�œ����ꏊ�ɑ��݂���FileZilla�ł��A���ꂼ��l�̐ݒ���������o���܂�."
LangString UseXML ${CURLANG} "XML(&X) �t�@�C�����g�p����"
LangString UseRegistry ${CURLANG} "���W�X�g���[(&r)�g�p����"
LangString ForceUseRegistry ${CURLANG} "XML�t�@�C�������݂��Ă��A���W�X�g���[���g�p���܂��B"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "�ȉ��̃R���|�[�l���g�́A�T�C�Y���k�������邽�߂ɃC���X�g�[���Ɋ܂܂�Ă��܂���B:"
LangString DownloadDialog3 ${CURLANG} "�����̃R���|�[�l���g���C���X�g�[�����邽�߂ɁA�C���X�g�[���͕K�v�ȃt�@�C�����_�E�����[�h���邱�Ƃ��ł��܂��B���݃t�@�C�����_�E�����[�h�������Ȃ��ꍇ�A��ōĂуC���X�g�[�����N�����鎖���ł��܂��B�܂��� http://filezilla.sourceforge.net ����t�@�C�����蓮�Ń_�E�����[�h���鎖���ł��܂�"
LangString DownloadDialog4 ${CURLANG} "�C���^�[�l�b�g����̃_�E�����[�h�ƃC���X�g�[��"
LangString DownloadDialog5 ${CURLANG} "�����̃R���|�[�l���g���X�L�b�v����B"
LangString DownloadDialog6 ${CURLANG} "���[�J���̃p�b�P�[�W�E�f�B���N�g�����g�p����B(�ȉ��ɓ���):"
LangString DownloadDialog8 ${CURLANG} "�p�b�P�[�W�f�B���N�g�����g�p����Ƃ��A�C���X�g�[���̓f�B���N�g���Ƀ_�E�����[�h�������ׂẴt�@�C����ۑ����܂��B�܂��A�p�b�P�[�W�f�B���N�g���Ɋ��ɑ��݂��Ă��Ȃ��ꍇ�A�t�@�C�����_�E�����[�h���܂��B"

;Download strings
LangString DownloadDownloading ${CURLANG} "�_�E�����[�h�� $1"
LangString DownloadDownloadFailed ${CURLANG} "  �_�E�����[�h���s: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  �_�E�����[�h�͎�������܂����B" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "�_�E�����[�h���s $0. ���R: $R1$\n���̃R���|�[�l���g���C���X�g�[������Ă��܂���B$\n�ォ��A���̃R���|�[�l���g���_�E�����[�h���āA�Ď��s���邽�߂ɍĂуC���X�g�[�����N�����Ă�������.$\n�������̃G���[�������ꍇ�A�C���X�g�[�����C���^�l�b�g�ɃA�N�Z�X���邱�Ƃ��ł��Ă��܂���B�蓮�ō�Ƃ���ꍇ�́A���ł��Ahttp://filezilla.sourceforge.net����R���|�[�l���g���_�E�����[�h���邱�Ƃ��ł��܂��B"
LangString DownloadDownloadSuccessful ${CURLANG} "  �_�E�����[�h����" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "$0�̃R���e���c�𓀂����s���܂���. ���R: $R1$\n���̃R���|�[�l���g���C���X�g�[������Ă��܂���B"

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "FileZilla�ɂ���č쐬���ꂽ���ׂẴ��W�X�g���[�L�[���폜���܂����B(�I���������W�X�g���̕ۑ��ꏊ�́A�T�C�g�}�l�[�W���[�œo�^�������̂��܂܂�Ă��܂��B)?"

!verbose 4