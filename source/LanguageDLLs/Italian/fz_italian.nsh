;Language specific include file for FileZilla installer
;Created by Tim Kosse (mailto:tim.kosse@gmx.de)
;This file has been translated by Andrea Sanavia

!verbose 3

!ifdef CURLANG
  !undef CURLANG
!endif
!define CURLANG ${LANG_ITALIAN}

;Language name as used by the FZ language dlls
LangString FZLanguageName ${CURLANG} "Italiano"

;License dialog
LangString LicenseFile ${CURLANG} "license.txt"

;Component-select dialog
LangString InstTypeStandardName ${CURLANG} "Standard"
LangString InstTypeFullName ${CURLANG} "Completa"
LangString SecFileZillaName ${CURLANG} "FileZilla (richiesto)"
LangString SecDocumentationName ${CURLANG} "Documentazioni"
LangString SecEnglishDocumentationName ${CURLANG} "Documentazione inglese"
LangString SecFrenchDocumentationName ${CURLANG} "Documentazione francese"
LangString SecLangFilesName ${CURLANG} "File linguistici aggiuntivi"
LangString SecSourceCodeName ${CURLANG} "Codice Sorgente"
LangString SecStartMenuName ${CURLANG} "Alias nel Menu Start"
LangString SecDesktopIconName ${CURLANG} "Icona su Desktop"
LangString SecDebugName ${CURLANG} "File di Debug"

LangString SecFileZillaDesc ${CURLANG} "Installa FileZilla.exe ed altri file richiesti"
LangString SecDocumentationDesc ${CURLANG} "Copia i file di documentazione nel percorso di destinazione"
LangString SecEnglishDocumentationDesc ${CURLANG} "Installa la documentazione in inglese"
LangString SecFrenchDocumentationDesc ${CURLANG} "Preleva ed installa la documentazione in inglese"
LangString SecLangFilesDesc ${CURLANG} "Copia i file linguistici aggiuntivi di FileZilla nel percorso di destinazione"
LangString SecSourceCodeDesc ${CURLANG} "Preleva e copia il codice sorgente nel percorso di destinazione."
LangString SecStartMenuDesc ${CURLANG} "Crea alias nel menu Start"
LangString SecDesktopIconDesc ${CURLANG} "Aggiungi una icona sul desktop per un accesso rapido a FileZilla"
LangString SecDebugDesc ${CURLANG} "Copia i file debug di supporto per analizzare i crash"
  
;Page titles
LangString PageSettingsTitle ${CURLANG} "Impostazioni generali"
LangString PageSettingsSubTitle ${CURLANG} "Configura modalit� sicura e salvataggio delle impostazioni"
LangString PageDownloadTitle ${CURLANG} "Opzioni di download"
LangString PageDownloadSubTitle ${CURLANG} "Alcuni componenti devono essere scaricarti"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "Desiderate avviare ${MUI_PRODUCT} in modalit� sicura?\nIn modalit� sicura, FileZilla non memorizza le password.\nSi raccomanda di farlo se diverse persone hanno accesso a questo PC."
LangString UseSecureMode ${CURLANG} "Utlizza modalit� sicura"
LangString DontUseSecureMode ${CURLANG} "Non utlizza modalit� sicura"
LangString StorageQuestion ${CURLANG} "FileZilla pu� salvare le sue impostazioni in due luoghi diversi: nel registro ed in un file XML.\r\nIn molti casi � preferibile utilizzare il file XML.\r\nSe operate in un ambiente multi-utente dove ogni utente ha il suo profilo, dovreste utilizzare il registro come luogo di salvataggio in modo che ogni utente abbia le sue impostazioni personali anche per FileZilla."
LangString UseXML ${CURLANG} "Utilizza file &XML"
LangString UseRegistry ${CURLANG} "Utlizza &registro"
LangString ForceUseRegistry ${CURLANG} "&Forza l'utilizzo del registro anche se il file XML esiste"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "Le seguenti componenti non sono comprese nell'installatore per ridurrne le dimensioni:"
LangString DownloadDialog3 ${CURLANG} "Per installare queste componenti, l'installatore pu� scaricare i file richiesti. Se non desiderate scaricare i file adesso, potrete avviare l'installatore di nuovo pi� avanti, o potrete scaricare i file manualmente da http://filezilla.sourceforge.net"
LangString DownloadDialog4 ${CURLANG} "Preleva ed installa da internet"
LangString DownloadDialog5 ${CURLANG} "Salta questi componenti"
LangString DownloadDialog6 ${CURLANG} "Utilizza la directory locale  (inserire qui sotto):"
LangString DownloadDialog8 ${CURLANG} "Quando si utilizza la directory locale, l'installatore salver� tutti i file scaricati in questa directory. Inoltre, prelever� solo quei file che non sono gi� presenti nella directory locale."

;Download strings
LangString DownloadDownloading ${CURLANG} "Sto prelevando $1"
LangString DownloadDownloadFailed ${CURLANG} "  Prelievo fallito: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  Prelievo annullato" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "Prelievo di  $0  fallito. Motivo: $R1$\nQuesto componente non sar� installato.$\nAvviate l'installatore di nuovo pi� tardi  per ritentare il prelievo di questo componente.$\nSe questo errore persiste, l'installatore potrebbe non essere in grado di accedere ad internet. In ogni caso potete prelevare i componenti manualmente da  http://filezilla.sourceforge.net"
LangString DownloadDownloadSuccessful ${CURLANG} "  Prelievo terminato con successo" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "Estrazione dei contenuti $0  fallita. Motivo: $R1$\nQuesto componente non sar� installato."

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "Elimina tutti i dati nel Registro creati da FileZilla (compresi le voci del Site Manager se avete scelto il registro come luogo di memorizzazione)?"

!verbose 4