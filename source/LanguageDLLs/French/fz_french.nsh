;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Fabien ILLIDE (mailto:fabienillide@users.sourceforge.net)

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_FRENCH}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Fran�ais"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standard"
LangString InstTypeFullName ${CURLANG} "Complet"
LangString SecFileZillaName ${CURLANG} "FileZilla (requis)"
LangString SecDocumentationName ${CURLANG} "Documentation"
LangString SecEnglishDocumentationName ${CURLANG} "Documentation en anglais"
LangString SecFrenchDocumentationName ${CURLANG} "Documentation en fran�ais"
LangString SecLangFilesName ${CURLANG} "Fichiers de langues additionelles"
LangString SecSourceCodeName ${CURLANG} "Code source"
LangString SecStartMenuName ${CURLANG} "Raccourcis dans le menu D�marrer"
LangString SecDesktopIconName ${CURLANG} "Ic�ne sur le bureau"
LangString SecDebugName ${CURLANG} "Fichiers de debogage"

LangString SecFileZillaDesc ${CURLANG} "Installe FileZilla.exe et d'autres fichiers requis"
LangString SecDocumentationDesc ${CURLANG} "Copie les fichiers de documentation vers le r�pertoire cible"
LangString SecEnglishDocumentationDesc ${CURLANG} "Installe la documentation en anglais"
LangString SecFrenchDocumentationDesc ${CURLANG} "T�l�charge et installe la documentation en fran�ais"
LangString SecLangFilesDesc ${CURLANG} "Copie les fichiers de langues pour FileZilla vers le r�pertoire cible"
LangString SecSourceCodeDesc ${CURLANG} "T�l�charge et copie le code source vers le r�pertoire cible."
LangString SecStartMenuDesc ${CURLANG} "Cr�� les raccourcis dans le menu D�marrer"
LangString SecDesktopIconDesc ${CURLANG} "Ajoute une ic�ne sur le bureau pour un lancement rapide de FileZilla"
LangString SecDebugDesc ${CURLANG} "Copie les fichiers de debug pour aider l'analyse des crashes"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Param�tres g�n�raux"
LangString PageSettingsSubTitle ${CURLANG} "Configurer le mode s�curis� et les param�tres de stockage"
LangString PageDownloadTitle ${CURLANG} "Options de t�l�chargement"
LangString PageDownloadSubTitle ${CURLANG} "Quelques �l�ments doivent �tre t�l�charg�s."

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Voulez-vous utiliser ${MUI_PRODUCT} en mode s�curis� ?\nEn mode s�curis�, FileZilla ne se souviendra d'aucun mot de passe.\nCe mode s�curis� est recommand� si plusieurs personnes ont acc�s � cet ordinateur."
LangString UseSecureMode ${CURLANG} "Utiliser le mode s�curis�"
LangString DontUseSecureMode ${CURLANG} "Ne pas utiliser le mode s�curis�"
LangString StorageQuestion ${CURLANG} "FileZilla peut enregistrer ses param�tres � deux endroits diff�rents : Dans la base de registre ou dans un fichier XML.\r\nDans la plupart des cas, il est recommend� d'utiliser le fichier XML.\r\nSi vous �tes dans un environnement multi-utilisateur o� chaque utilisateur a son propre profile, vous devriez utiliser la base de registre comme stockage de sorte que chaque utilisateur aura ses propres param�tres pour FileZilla."
LangString UseXML ${CURLANG} "Utiliser le fichier &XML"
LangString UseRegistry ${CURLANG} "Utiliser la base de &registre"
LangString ForceUseRegistry ${CURLANG} "&Forcer l'utilisation de la base de registre m�me si un fichier XML existe"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Les �l�ments suivants ne sont pas inclus dans l'installateur pour r�duire sa taille :"
LangString DownloadDialog3 ${CURLANG} "Pour installer ces �l�ments, l'installateur peut t�l�charger les fichiers requis. Si vous ne voulez pas t�l�charger ces fichiers maintenant, vous pourrez relancer l'installateur plus tard, ou vous pouvez t�l�charger les fichiers manuellement depuis http://filezilla.sourceforge.net/"
LangString DownloadDialog4 ${CURLANG} "T�l�charger et installer depuis internet"
LangString DownloadDialog5 ${CURLANG} "Passer ces �l�ments"
LangString DownloadDialog6 ${CURLANG} "Utiliser le r�pertoire local de paquetage (entr� ci-dessous) :"
LangString DownloadDialog8 ${CURLANG} "En utilisant le r�pertoire local de paquetage, l'installateur enregistrera tout les fichiers t�l�charg�s dans ce r�pertoire. Il t�l�chargera uniquement les fichiers qui ne sont pas actuellement pr�sents dans le r�pertoire local de paquetage."

;Download strings
LangString DownloadDownloading ${CURLANG} "T�l�chargement de $1"
LangString DownloadDownloadFailed ${CURLANG} "  T�l�chargement �chou� : $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  T�l�chargement annul�" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Echec du t�l�chargement de $0. Raison : $R1$\nCet �l�ment ne sera pas install�.$\nPlus tard, lancez � nouveau l'installateur pour r�essayer de t�l�charger cet �l�ment.$\nSi cette erreur persiste, l'installateur n'arrive peut-�tre pas � acc�der � internet. Dans tout les cas vous pouvez t�l�charger manuellement les �l�ments depuis http://filezilla.sourceforge.net/"
LangString DownloadDownloadSuccessful ${CURLANG} "  T�l�chargement r�ussi" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Impossible d'extraire le contenu de $0. Raison : $R1$\nCet �l�ment ne sera pas install�."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Supprimer toutes les cl�s de la base de registre cr��es par FileZilla (incluant les entr�es du Gestionnaire de Sites si vous avez choisis la base de registre comme emplacement des enregistrements) ?"

!verbose 4