// FileZilla - a Windows ftp client

// Copyright (C) 2002 - Tim Kosse <tim.kosse@gmx.de>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

// stdafx.h : Include-Datei f�r Standard-System-Include-Dateien,
//  oder projektspezifische Include-Dateien, die h�ufig benutzt, aber
//      in unregelm��igen Abst�nden ge�ndert werden.
//

#if !defined(AFX_STDAFX_H__A2BD532B_C6A5_4A21_9595_7B99F7A69745__INCLUDED_)
#define AFX_STDAFX_H__A2BD532B_C6A5_4A21_9595_7B99F7A69745__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Selten verwendete Teile der Windows-Header nicht einbinden

#pragma warning (disable : 4786)

#include <afxwin.h>         // MFC-Kern- und -Standardkomponenten
#include <afxext.h>         // MFC-Erweiterungen
#include <afxcview.h>
#include <afxdtctl.h>		// MFC-Unterst�tzung f�r allgemeine Steuerelemente von Internet Explorer 4
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC-Unterst�tzung f�r g�ngige Windows-Steuerelemente
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>
#include <misc\htmlhelp.h>
#include <atlconv.h>

#include "config.h"

#include "MFC64bitFix.h"

//STL includes
#include <list>
#include <map>
#include <vector>
#include <deque>
#include <set>
#include <algorithm>


#include "AsyncSocketEx.h"

#include "misc\saprefsdialog.h"
#include "misc\saprefssubdlg.h"

#include "options.h"
#include "crypt.h"

#include "structures.h"

#define FILEZILLA_PROCESSCMDLINE_MESSAGE _T("FileZilla Process CmdLine Message")
#define FILEZILLA_THREAD_MESSAGE _T("FileZilla Thread Message")

const UINT WM_FILEZILLA_PROCESSCMDLINE = ::RegisterWindowMessage(FILEZILLA_PROCESSCMDLINE_MESSAGE);
const UINT WM_THREADMSG = ::RegisterWindowMessage(FILEZILLA_THREAD_MESSAGE);

#define TRANSFER_STARTED 1
#define TRANSFER_ERROR 2
#define TRANSFER_COMPLETE 3
#define TRANSFER_CRITICALERROR 4
#define TRANSFER_ABORTED 5

typedef struct
{
	unsigned int ip;
	unsigned short port;
} t_host;

class t_ffam_statusmessage
{
public:
	CString status;
	int type;
	BOOL post;
};

typedef struct
{
	__int64 bytes;
	int percent;
	int timeelapsed;
	int timeleft;
	int transferrate;
	BOOL bFileTransfer;
} t_ffam_transferstatus;

#define MINDLLVERSION      0x0002000200040000
#define MINVALIDDLLVERSION 0x0002000200040000

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // !defined(AFX_STDAFX_H__A2BD532B_C6A5_4A21_9595_7B99F7A69745__INCLUDED_)