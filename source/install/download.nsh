!include ZipDLL.nsh

!macro Print PARAM
  SetDetailsPrint both
  DetailPrint "${PARAM}"
  SetDetailsPrint textonly
!macroend

;------------------------------------------------------------------------------
; TrimBackslashes
; input, top of stack  (i.e. whatever\)
; output, top of stack (replaces, with i.e. whatever)
; modifies no other variables.
;
Function TrimBackslashes
  Exch $0
  Push $1
  Push $2
    StrCpy $1 0
    loop:
      IntOp $1 $1 - 1
      StrCpy $2 $0 1 $1
      StrCmp $2 "\" loop
  IntOp $1 $1 + 1

  StrLen $2 $0
  IntOp $2 $2 + $1
  StrCpy $0 $0 $2
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

Function DownloadAndExtract
  SetDetailsPrint textonly

  StrCpy $R9 success

  ; Check if user has selected "Skip"
  StrCpy $R0 ""
  ReadIniStr $R0 "$PLUGINSDIR\download.ini" "Field 5" state
  StrCmp $R0 "1" function_end

  ; Check if installer should use local package directory and if the target file already exists
  
  StrCpy $R0 ""
  ReadIniStr $R0 "$PLUGINSDIR\download.ini" "Field 6" state
  StrCmp $R0 "1" try_use_local
  goto startdl
 try_use_local:
  StrCpy $R0 ""
  ReadIniStr $R0 "$PLUGINSDIR\download.ini" "Field 7" state
  StrCmp $R0 "" use_temp
  ClearErrors
  CreateDirectory $R0
  IfFileExists "$R0\*.*" "" startdl
  IfErrors startdl
  Push $R0
  Call TrimBackslashes
  Pop $R0
  StrCpy $R0 "$R0\$0"
  StrCpy $R2 1
  IfFileExists $R0 startextract

startdl:
  ;Now download the file

  ; make the call to download

  ; Check if installer should use local package directory
  
  StrCpy $R0 ""
  ReadIniStr $R0 "$PLUGINSDIR\download.ini" "Field 6" state
  StrCmp $R0 "1" use_local
 use_temp:
  StrCpy $R2 0
  GetTempFileName $R0
  goto dl_start
 use_local:
  StrCpy $R0 ""
  ReadIniStr $R0 "$PLUGINSDIR\download.ini" "Field 7" state
  StrCmp $R0 "" use_temp
  ClearErrors
  CreateDirectory $R0
  IfFileExists "$R0\*.*" use_local2 use_temp
 use_local2:
  IfErrors use_temp
  Push $R0
  Call TrimBackslashes
  Pop $R0
  StrCpy $R0 "$R0\$0"
  StrCpy $R2 1
 dl_start:

  !insertmacro Print $(DownloadDownloading)

  nsisdl::download $1 $R0 ; for a quiet install, use download_quiet

  ; check if download succeeded
  Pop $R1
  StrCmp $R1 "success" dlsuccessful
  StrCmp $R1 "cancel" dlcancelled

  ; we failed
  !insertmacro Print $(DownloadDownloadFailed)
  Delete $R0
  Pop $0
  IfSilent +2
  MessageBox MB_OK|MB_ICONEXCLAMATION $(DownloadDownloadFailedBox)
  StrCpy $R9 failure

  goto function_end

 dlcancelled:
  !insertmacro Print $(DownloadDownloadCancelled)
  Delete $R0
  Quit
 dlsuccessful:
  !insertmacro Print $(DownloadDownloadSuccessful)
 
 startextract:

  ;Now extract the file

  ; make the call to ExtractAll
  !insertmacro ZIPDLL_EXTRACT $R0 $2 <ALL>

  ; check if extract succeeded
  Pop $R1
  StrCmp $R1 "success" extract_end

  IfSilent +2
  MessageBox MB_OK|MB_ICONEXCLAMATION $(DownloadExtractFailedBox)
  StrCpy $R9 failure
 extract_end:

  StrCmp $R2 1 no_delete
  Delete $R0
 no_delete:

 function_end:
  Push $R9
  SetDetailsPrint both
FunctionEnd 