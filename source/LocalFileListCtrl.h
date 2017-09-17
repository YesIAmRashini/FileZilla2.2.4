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

#if !defined(AFX_LOCALFILELISTCTRL_H__453BE9E4_AFDA_4745_A48F_E8BDC0311E31__INCLUDED_)
#define AFX_LOCALFILELISTCTRL_H__453BE9E4_AFDA_4745_A48F_E8BDC0311E31__INCLUDED_

#include "LocalView2.h"	// Hinzugef�gt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LocalFileListCtrl.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Fenster CLocalFileListCtrl 
class CLocalView2;

class CLocalFileListCtrl : public CListCtrl
{

// Konstruktion
public:
	CLocalFileListCtrl(CLocalView2 *pOwner);

// Attribute
public:

// Operationen
public:

// �berschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktions�berschreibungen
	//{{AFX_VIRTUAL(CLocalFileListCtrl)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	protected:
	//}}AFX_VIRTUAL

// Implementierung
public:
	void RefreshFile(CString file);
	void UpdateColumns(int nHideColumns);
	void SetListStyle(int nStyle);
	void SetFolder(CString folder);
	void ReloadHeader();
	CString GetFolder() const;
	virtual ~CLocalFileListCtrl();
	void SaveColumnSizes();
	void SortList(int item, int direction = 0);
	void GetSortInfo(int &column, int &direction) const;

	// Drag&Drop
	BOOL DragPosition(CImageList *pImageList, CWnd* pDragWnd, CPoint point);
	void DragLeave(CImageList *pImageList);
	void OnDragEnd(int target, CPoint point);
	CString GetDropTarget() const;

	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	CLocalView2 *m_pOwner;
	BOOL UpdateStatusBar();
	CString GetType(CString lName, BOOL bIsDir);
	class t_FileData
	{
	public:
		t_FileData()
		{
			iIcon = -1;
		};
		CString Name;
		CString lName;
		BOOL bIsDir;
		_int64 nSize;
		CTime Time;
		int iIcon;
	};
	CString m_NewDir;
	BOOL m_bUpdating;
	int m_nHideColumns;
	int m_nStyle;
	CString m_transferuser;
	CString m_transferpass;

	void DisplayDrives();
	std::vector<unsigned int> m_IndexMapping;
	std::map<CString, CString> m_TypeCache;
	void ReloadHeaderItem(int nIndex,UINT nID);
	void UploadDir(CString dir,CString subdir,BOOL upload);
	int m_sortcolumn;
	int m_sortdir;
	std::map<CString, CString> m_permanentTypeCache;
	CImageList m_SortImg;
	BOOL GetSysImgList();
	CString m_Fullpath;
	std::vector<t_FileData> m_FileData;
	UINT GetSelectedItem();
	BOOL m_Columns[4];

	void quicksortbyname(const BOOL &direction, int anf, int ende);
	void quicksortbytype(const std::vector<CString> &array, const BOOL &direction, int anf, int ende);
	void quicksortbysize(const BOOL &direction, int anf, int ende);
	void quicksortbytime(const BOOL &direction, int anf, int ende);

	// Drag&Drop
	int m_nDragHilited;
	
	//{{AFX_MSG(CLocalFileListCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnUpdateContinue(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLocalcontextAddtoqueue();
	afx_msg void OnLocalcontextCreatedirectory();
	afx_msg void OnLocalcontextDelete();
	afx_msg void OnLocalcontextOpen();
	afx_msg void OnLocalcontextProperties();
	afx_msg void OnLocalcontextRename();
	afx_msg void OnLocalcontextUpload();
	afx_msg void OnLocalcontextUploadas();
	afx_msg void OnLocalcontextViewEdit();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ f�gt unmittelbar vor der vorhergehenden Zeile zus�tzliche Deklarationen ein.

#endif // AFX_LOCALFILELISTCTRL_H__453BE9E4_AFDA_4745_A48F_E8BDC0311E31__INCLUDED_
