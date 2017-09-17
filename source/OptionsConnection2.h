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

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsConnection2 

#pragma once

class COptionsConnection2 : public CSAPrefsSubDlg
{
	DECLARE_DYNCREATE(COptionsConnection2)

// Konstruktion
public:
	COptionsConnection2();
	~COptionsConnection2();

// Dialogfelddaten
	//{{AFX_DATA(COptionsConnection2)
	enum { IDD = IDD_CONNECTION2 };
	CEdit	m_IntervalLowCtrl;
	CEdit	m_IntervalHighCtrl;
	UINT	m_Timeout;
	BOOL	m_UseKeepAlive;
	CString	m_IntervalHigh;
	CString	m_IntervalLow;
	CString	m_Delay;
	CString	m_NumRetries;
	//}}AFX_DATA


// �berschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(COptionsConnection2)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsConnection2)
	afx_msg void OnCheck1();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
