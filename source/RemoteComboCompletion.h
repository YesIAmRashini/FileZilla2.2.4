// FileZilla - a Windows ftp client

// Copyright (C) 2003 - Tim Kosse <tim.kosse@gmx.de>

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

#if !defined(AFX_REMOTECOMBOCOMPLETION_H__25C0E1F7_4353_4CAF_9546_972059907921__INCLUDED_)
#define AFX_REMOTECOMBOCOMPLETION_H__25C0E1F7_4353_4CAF_9546_972059907921__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MISC\ComboCompletion.h"

class CRemoteComboCompletion : public CComboCompletion  
{
public:
	void SetPath(const CServerPath &path);
	void SetServer(const t_server &server);
	CRemoteComboCompletion();
	virtual ~CRemoteComboCompletion();

protected:
	virtual void OnChangeDir(CString dir);

	t_server* m_pServer;
	CServerPath* m_pPath;
};

#endif // !defined(AFX_REMOTECOMBOCOMPLETION_H__25C0E1F7_4353_4CAF_9546_972059907921__INCLUDED_)
