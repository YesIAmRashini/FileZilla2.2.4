;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Bata Gy�rgy

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_HUNGARIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Hungarian"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Szok�sos"
LangString InstTypeFullName ${CURLANG} "Teljes"
LangString SecFileZillaName ${CURLANG} "FileZilla (sz�ks�ges)"
LangString SecDocumentationName ${CURLANG} "Dokument�ci�"
LangString SecEnglishDocumentationName ${CURLANG} "Angol dokument�ci�"
LangString SecFrenchDocumentationName ${CURLANG} "Francia dokument�ci�"
LangString SecLangFilesName ${CURLANG} "Nyelvi f�jlok"
LangString SecSourceCodeName ${CURLANG} "Forr�sk�d"
LangString SecStartMenuName ${CURLANG} "Start men� parancsikonok"
LangString SecDesktopIconName ${CURLANG} "Asztali ikon"
LangString SecDebugName ${CURLANG} "Hibakeres�si f�jlok"

LangString SecFileZillaDesc ${CURLANG} "A FileZilla.exe �s m�s sz�ks�ges f�jlok telep�t�se"
LangString SecDocumentationDesc ${CURLANG} "A dokument�ci�s f�jlok telep�t�se"
LangString SecEnglishDocumentationDesc ${CURLANG} "Az angol dokument�ci� telep�t�se"
LangString SecFrenchDocumentationDesc ${CURLANG} "A francia dokument�ci� let�lt�se �s telep�t�se"
LangString SecLangFilesDesc ${CURLANG} "A FileZilla nyelvi f�jljainak telep�t�se"
LangString SecSourceCodeDesc ${CURLANG} "A forr�sk�d let�lt�se �s telep�t�se"
LangString SecStartMenuDesc ${CURLANG} "Parancsikonok l�trehoz�sa a Start men�ben"
LangString SecDesktopIconDesc ${CURLANG} "Parancsikon l�trehoz�sa az asztalon"
LangString SecDebugDesc ${CURLANG} "A hibakeres�si f�jlok telep�t�se, melyek az �sszeoml�s elemz�s�ben seg�thetnek"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "�ltal�nos be�ll�t�sok"
LangString PageSettingsSubTitle ${CURLANG} "A biztons�gi m�d �s a be�ll�t�sok t�rol�s�nak konfigur�l�sa"
LangString PageDownloadTitle ${CURLANG} "Let�lt�s be�ll�t�sai"
LangString PageDownloadSubTitle ${CURLANG} "N�h�ny �sszetev� let�lt�s�re van sz�ks�g."

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "A ${MUI_PRODUCT}-t biztons�gos m�dban szeretn� futtatni?\nA biztons�gos m�dban a FileZilla nem jegyzi meg a jelszavakat.\nA biztons�gos m�d v�laszt�sa aj�nlott, ha t�bben haszn�lj�k ezt a sz�m�t�g�pet."
LangString UseSecureMode ${CURLANG} "Biztons�gos m�d haszn�lata"
LangString DontUseSecureMode ${CURLANG} "Ne haszn�lja a biztons�gos m�dot"
LangString StorageQuestion ${CURLANG} "A FileZilla a be�ll�t�sait k�tf�le helyen t�rolhatja: a regisztr�ci�s adatb�zisban vagy egy XML f�jlban. A legt�bb esetben az XML f�jl haszn�lata az aj�nlott. Azonban ha egy t�bbfelhaszn�l�s sz�m�t�g�pen dolgozik, amelyen a felhaszn�l�k saj�t fel�lettel rendelkeznek, akkor a regisztr�ci�s adatb�zist c�lszer� haszn�lni a be�ll�t�sok t�rol�s�ra, �gy a FileZilla be�ll�t�sai is egy�nileg testre szabhat�k lesznek."
LangString UseXML ${CURLANG} "&XML f�jl haszn�lata"
LangString UseRegistry ${CURLANG} "A &regisztr�ci�s adatb�zis haszn�lata"
LangString ForceUseRegistry ${CURLANG} "&Abban az esetben is, ha az XML f�jl m�r l�tezik"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "A telep�t� a k�vetkez� �sszetev�ket nem tartalmazza a kis f�jlm�ret megtart�sa �rdek�ben:"
LangString DownloadDialog3 ${CURLANG} "Ezen �sszetev�k telep�t�s�hez a sz�ks�ges f�jlokat a telep�t� le tudja t�lteni. Ha nem szeretn�, hogy a telep�t� most let�ltse a f�jlokat, b�rmikor �jb�l futtathatja a telep�t�t, vagy saj�t maga k�zzel let�ltheti a f�jlokat a http://filezilla.sourceforge.net c�mr�l."
LangString DownloadDialog4 ${CURLANG} "Let�lt�s �s telep�t�s az Internetr�l"
LangString DownloadDialog5 ${CURLANG} "Ezeknek az �sszetev�knek a kihagy�sa"
LangString DownloadDialog6 ${CURLANG} "Helyi let�lt�si k�nyvt�r haszn�lata (adja meg al�bb):"
LangString DownloadDialog8 ${CURLANG} "Ha a helyi let�lt�si k�nyvt�r haszn�lat�t enged�lyezi, a telep�t� az �sszes let�lt�tt f�jlt ebbe a mapp�ba menti el. M�sr�szt csak azokat a f�jlokat t�lti le, melyek m�g nincsenek jelen a let�lt�si k�nyvt�rban."

;Download strings
LangString DownloadDownloading ${CURLANG} "$1 let�lt�se"
LangString DownloadDownloadFailed ${CURLANG} "  $0 let�lt�se nem siker�lt." ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  A let�lt�s megszakadt." ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "$0 let�lt�se nem siker�lt. Oka: $R1$\nEz az �sszetev� nem fog telep�t�dni.$\nK�s�bb futtassa �jra a telep�t�t az �sszetev� let�lt�s�nek megism�tl�s�hez.$\nHa ez a hiba�zenet megmarad, akkor lehet, hogy a telep�t� nem tud az internethez csatlakozni. Az �sszetev�ket b�rmikor k�zzel is let�ltheti a http://filezilla.sourceforge.net c�mr�l."
LangString DownloadDownloadSuccessful ${CURLANG} "  A let�lt�s siker�lt." ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "$0 kicsomagol�sa nem siker�lt. Oka: $R1$\nEz az �sszetev� nem fog telep�t�dni."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "T�r�ljem a FileZilla �ltal l�trehozott �sszes regisztr�ci�s bejegyz�st (bele�rtve a Kedvenc Helyek �llom�sait is, amennyiben a regisztr�ci�s adatb�zist v�lasztotta a be�ll�t�sok t�rol�s�hoz)?"

!verbose 4

