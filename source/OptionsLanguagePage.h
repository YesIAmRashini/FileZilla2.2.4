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

#if !defined(AFX_OPTIONSLANGUAGEPAGE_H__FE50B0C7_E24D_426A_902F_8C6394104E5E__INCLUDED_)
#define AFX_OPTIONSLANGUAGEPAGE_H__FE50B0C7_E24D_426A_902F_8C6394104E5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionsLanguagePage.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld COptionsLanguagePage 

class COptionsLanguagePage : public CSAPrefsSubDlg
{
	DECLARE_DYNCREATE(COptionsLanguagePage)

// Konstruktion
public:
	CString m_selLang;
	std::set<CString> m_LanguageStringList;
	COptionsLanguagePage();
	~COptionsLanguagePage();

// Dialogfelddaten
	//{{AFX_DATA(COptionsLanguagePage)
	enum { IDD = IDD_OPTIONS_LANGUAGE };
	CListBox	m_List;
	//}}AFX_DATA


// �berschreibungen
	// Der Klassen-Assistent generiert virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(COptionsLanguagePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterst�tzung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COptionsLanguagePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_OPTIONSLANGUAGEPAGE_H__FE50B0C7_E24D_426A_902F_8C6394104E5E__INCLUDED_
