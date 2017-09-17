;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Piotr Murawski & Rafa� Lampe; www.lomsel.com.pl

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_POLISH}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Polski"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standardowa"
LangString InstTypeFullName ${CURLANG} "Pe�na"
LangString SecFileZillaName ${CURLANG} "FileZilla (wymagane)"
LangString SecDocumentationName ${CURLANG} "Dokumentacja"
LangString SecEnglishDocumentationName ${CURLANG} "Angielska dokumentacja"
LangString SecFrenchDocumentationName ${CURLANG} "Francuska dokumentacja"
LangString SecLangFilesName ${CURLANG} "Dodatkowe pliki j�zyka"
LangString SecSourceCodeName ${CURLANG} "�r�d�o kodu"
LangString SecStartMenuName ${CURLANG} "Skr�ty klawiszowe Menu Start"
LangString SecDesktopIconName ${CURLANG} "Ikona pulpitu"
LangString SecDebugName ${CURLANG} "Pliki Debug"

LangString SecFileZillaDesc ${CURLANG} "Instaluje FileZilla.exe oraz inne wymagane pliki"
LangString SecDocumentationDesc ${CURLANG} "Kopiuje pliki dokumentacji do miejsca docelowego"
LangString SecEnglishDocumentationDesc ${CURLANG} "Instaluj angielsk� dokumentacj�"
LangString SecFrenchDocumentationDesc ${CURLANG} "Pobierz i instaluj angielsk� dokumentacj�"
LangString SecLangFilesDesc ${CURLANG} "Kopiuje pliki j�zyka do miejsca docelowego"
LangString SecSourceCodeDesc ${CURLANG} "Pobiera i kopiuje �r�d�o kodu do miejsca docelowego."
LangString SecStartMenuDesc ${CURLANG} "Tworzy klawisze skr�t�w w menu start"
LangString SecDesktopIconDesc ${CURLANG} "Dodaje ikon� pulpitu, aby uzyska� szybszy dost�p do programu FileZilla"
LangString SecDebugDesc ${CURLANG} "Kopiuj pliki debug, kt�re pomog� zanalizowa� uszkodzenia"

;Page titles
LangString PageSettingsTitle ${CURLANG} "Og�lne ustawienia"
LangString PageSettingsSubTitle ${CURLANG} "Potwierd� tryb ochrony i zapis ustawie�"
LangString PageDownloadTitle ${CURLANG} "Opcje pobierania"
LangString PageDownloadSubTitle ${CURLANG} "Niekt�re komponenty musz� by� pobrane"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Czy chcesz uruchomi� ${MUI_PRODUCT} w bezpiecznym trybie?\nW bezpiecznym trybie FileZilla nie pami�ta �adnych hase�.\nJest to zalecane je�eli kilka os�b ma dost�p do komputera."
LangString UseSecureMode ${CURLANG} "U�yj trybu ochrony"
LangString DontUseSecureMode ${CURLANG} "Nie u�ywaj trybu ochrony"
LangString StorageQuestion ${CURLANG} "FileZilla mo�e zapisa� ustawienia w dw�ch r�nych miejscach: W rejestrze i w pliku XML.\r\nW wi�kszo�ci przypadk�w zaleca si� zapis do XML.\r\nJe�eli pracujesz w trybie wielu u�ytkownik�w, gdzie ka�dy u�ytkownik ma sw�j w�asny profil, to nale�y u�y� zapisu do rejestru �eby ka�dy u�ytkownik mia� swoje ustawienia dla FileZilla."
LangString UseXML ${CURLANG} "U�yj pliku &XML"
LangString UseRegistry ${CURLANG} "U�yj &rejestru"
LangString ForceUseRegistry ${CURLANG} "Wymu� zapis do rejestru nawet, gdy istnieje plik XML"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Nast�puj�ce komponenty nie s� w��czone do instalatora po to by zredukowa� rozmiar:"
LangString DownloadDialog3 ${CURLANG} "Aby zainstalowa� te komponenty instalator mo�e pobra� wymagane pliki. Je�eli nie chcesz teraz pobra� plik�w, to mo�esz uruchomi� instalator p�niej lub pobra� pliki r�cznie z http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Pobierz i instaluj z Internetu"
LangString DownloadDialog5 ${CURLANG} "Pomi� te komponenty"
LangString DownloadDialog6 ${CURLANG} "U�yj lokalnego katalogu (wpisz poni�ej):"
LangString DownloadDialog8 ${CURLANG} "Kiedy u�ywasz katalogu, to instalator zapisze wszystkie pobrane pliki do tego katalogu. Zostan� pobrane tylko te pliki, kt�re nie s� obecne w tym katalogu."

;Download strings
LangString DownloadDownloading ${CURLANG} "Pobiera $1"
LangString DownloadDownloadFailed ${CURLANG} "  Pobieranie nie powiod�o si�: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Pobieranie anulowano" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "B��d pobierania $0. Przyczyna: $R1$\nTen komponent nie zostanie zainstalowany.$\nUruchom p�niej jeszcze raz instalator, aby ponownie spr�bowa� pobra� ten komponent.$\nJe�eli b��d wyst�puje nadal, to mo�e instalator nie ma dost�pu do internetu. W ka�dym przypadku mo�na pobra� komponenty r�cznie z http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Pobieranie powiod�o si�" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "B��d rozpakowania zawarto�ci $0. Przyczyna: $R1$\nTen komponent nie zostanie zainstalowany."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Czy usun�� wszystkie wpisy w rejestrze utworzone przez FileZilla (w��czaj�c wpisy mened�era stron, gdy wybrano rejestr do zapisu ustawie�)?"

!verbose 4
