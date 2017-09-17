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
LangString InstTypeStandardName ${CURLANG} "標準"
LangString InstTypeFullName ${CURLANG} "すべて"
LangString SecFileZillaName ${CURLANG} "FileZilla (必要です)"
LangString SecDocumentationName ${CURLANG} "ドキュメント"
LangString SecEnglishDocumentationName ${CURLANG} "英語のドキュメント"
LangString SecFrenchDocumentationName ${CURLANG} "フランス語のドキュメント"
LangString SecLangFilesName ${CURLANG} "追加の言語ファイル"
LangString SecSourceCodeName ${CURLANG} "ソースコード"
LangString SecStartMenuName ${CURLANG} "スタートメニューへショットカット追加"
LangString SecDesktopIconName ${CURLANG} "ディスクトップアイコン追加"
LangString SecDebugName ${CURLANG} "デバック用ファイル"

LangString SecFileZillaDesc ${CURLANG} "FileZilla.exeと他の必要なファイルをインストールします。"
LangString SecDocumentationDesc ${CURLANG} "インストールする場所にドキュメントファイルをコピーします。"
LangString SecEnglishDocumentationDesc ${CURLANG} "英語のドキュメントをインストールする。"
LangString SecFrenchDocumentationDesc ${CURLANG} "ダウンロードと英語のドキュメントをインストールする。"
LangString SecLangFilesDesc ${CURLANG} "インストールする場所にFileZillaの言語ファイルをコピーします。"
LangString SecSourceCodeDesc ${CURLANG} "インストールする場所にソースコードをダウンロードしてコピーします。"
LangString SecStartMenuDesc ${CURLANG} "スタートメニューへショートカットを作成します。"
LangString SecDesktopIconDesc ${CURLANG} "FileZillaを簡単に起動できるようにディスクトップにアイコンを追加します。"
LangString SecDebugDesc ${CURLANG} "クラッシュを解析する為に必要なデバックファイルをコピーします。"

;Page titles
LangString PageSettingsTitle ${CURLANG} "一般設定"
LangString PageSettingsSubTitle ${CURLANG} "安全なモードと設定内容を格納場所を設定してください"
LangString PageDownloadTitle ${CURLANG} "ダウンロードオプション"
LangString PageDownloadSubTitle ${CURLANG} "いくつかのコンポーネントをダウンロードします。"

;Settings dialog
LangString QuestionRunInSecureMode ${CURLANG} "あなたは安全なモードで${MUI_PRODUCT}を起動させますか?\n安全なモードでは、FileZillaはすべてのパスワードを記憶しません.\n複数の人が、このPCに使用する場合にはお勧めです。"
LangString UseSecureMode ${CURLANG} "安全なモードを使用する"
LangString DontUseSecureMode ${CURLANG} "安全なモードを使用しない"
LangString StorageQuestion ${CURLANG} FileZillaはレジストリかXMLファイルに設定を保存する事が出来ます:ほとんどの場合、XMLを使用する事を推奨します。\r\nもしあなたがマルチユーザー環境を使用していて各ユーザーのプロファイルを持つのであれば、レジストリを使用する事で同じ場所に存在するFileZillaでも、それぞれ個人の設定を持つ事が出来ます."
LangString UseXML ${CURLANG} "XML(&X) ファイルを使用する"
LangString UseRegistry ${CURLANG} "レジストリー(&r)使用する"
LangString ForceUseRegistry ${CURLANG} "XMLファイルが存在しても、レジストリーを使用します。"

;Download dialog
LangString DownloadDialog1 ${CURLANG} "以下のコンポーネントは、サイズを縮小させるためにインストーラに含まれていません。:"
LangString DownloadDialog3 ${CURLANG} "これらのコンポーネントをインストールするために、インストーラは必要なファイルをダウンロードすることができます。現在ファイルをダウンロードしたくない場合、後で再びインストーラを起動する事ができます。または http://filezilla.sourceforge.net からファイルを手動でダウンロードする事ができます"
LangString DownloadDialog4 ${CURLANG} "インターネットからのダウンロードとインストール"
LangString DownloadDialog5 ${CURLANG} "これらのコンポーネントをスキップする。"
LangString DownloadDialog6 ${CURLANG} "ローカルのパッケージ・ディレクトリを使用する。(以下に入力):"
LangString DownloadDialog8 ${CURLANG} "パッケージディレクトリを使用するとき、インストーラはディレクトリにダウンロードしたすべてのファイルを保存します。また、パッケージディレクトリに既に存在していない場合、ファイルをダウンロードします。"

;Download strings
LangString DownloadDownloading ${CURLANG} "ダウンロード中 $1"
LangString DownloadDownloadFailed ${CURLANG} "  ダウンロード失敗: $0" ;Don't remove the leading two whitespaces
LangString DownloadDownloadCancelled ${CURLANG} "  ダウンロードは取り消されました。" ;Don't remove the leading two whitespaces
LangString DownloadDownloadFailedBox ${CURLANG} "ダウンロード失敗 $0. 理由: $R1$\nこのコンポーネントがインストールされていません。$\n後から、このコンポーネントをダウンロードして、再試行するために再びインストーラを起動してください.$\nもしこのエラーが続く場合、インストーラがインタネットにアクセスすることができていません。手動で作業する場合は、いつでも、http://filezilla.sourceforge.netからコンポーネントをダウンロードすることができます。"
LangString DownloadDownloadSuccessful ${CURLANG} "  ダウンロード成功" ;Don't remove the leading two whitespaces
LangString DownloadExtractFailedBox ${CURLANG} "$0のコンテンツ解凍が失敗しました. 理由: $R1$\nこのコンポーネントがインストールされていません。"

;Uninstaller
LangString un.QuestionDeleteRegistry ${CURLANG} "FileZillaによって作成されたすべてのレジストリーキーを削除しますか。(選択したレジストリの保存場所は、サイトマネージャーで登録したものが含まれています。)?"

!verbose 4