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

#if !defined(AFX_FTPCONTROLSOCKET_H__AE6AA44E_B09D_487A_8EF2_A23697434945__INCLUDED_)
#define AFX_FTPCONTROLSOCKET_H__AE6AA44E_B09D_487A_8EF2_A23697434945__INCLUDED_

#include "structures.h"	// Hinzugefügt von der Klassenansicht
#include "StdAfx.h"	// Hinzugefügt von der Klassenansicht
#include "FileZillaApi.h"
#include "ControlSocket.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FtpControlSocket.h : Header-Datei
//

class CTransferSocket;
class CMainThread;
/////////////////////////////////////////////////////////////////////////////
// Befehlsziel CFtpControlSocket 

class CAsyncProxySocketLayer;
class CMainThread;
class CFtpControlSocket : public CControlSocket
{
	friend CTransferSocket;
// Attribute
public:

// Operationen
public:
	CFtpControlSocket(CMainThread *pMainThread);
	virtual ~CFtpControlSocket();

// Überschreibungen
public:
	virtual void Connect(t_server &server);
	virtual void OnTimer();
	virtual BOOL IsReady();
	virtual void List(BOOL bFinish, int nError=0, CServerPath path=CServerPath(), CString subdir="", int nListMode = 0);
	virtual void FtpCommand(LPCTSTR pCommand);
	virtual void Disconnect();
	virtual void FileTransfer(t_transferfile *transferfile = 0, BOOL bFinish = FALSE, int nError = 0);
	virtual void Delete(CString filename, const CServerPath &path);
	virtual void Rename(CString oldName, CString newName, const CServerPath &path, const CServerPath &newPath);
	virtual void MakeDir(const CServerPath &path);
	virtual void RemoveDir(CString dirname, const CServerPath &path);
	virtual void Chmod(CString filename, const CServerPath &path, int nValue);
		
	virtual void ProcessReply();
	virtual void TransferEnd(int nMode);
	virtual void Cancel(BOOL bQuit=FALSE);

	virtual void SetAsyncRequestResult(int nAction, CAsyncRequestData *pData);
	
	
	int CheckOverwriteFile();
	virtual BOOL Create();
	void TransfersocketListenFinished(unsigned int ip,unsigned short port);
	
	BOOL m_bKeepAliveActive;
	

	
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CFtpControlSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CFtpControlSocket)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG

// Implementierung
protected:
	//Called by OnTimer()
	void ResumeTransfer();
	void CheckForTimeout();
	void SendKeepAliveCommand();

	virtual int OnLayerCallback(const CAsyncSocketExLayer *pLayer, int nType, int nParam1, int nParam2);	
	void SetFileExistsAction(int nAction, COverwriteRequestData *pData);
	void SetVerifyCertResult( int nResult, t_SslCertData *pData );
	virtual int OnSslNotify(int nType, int nCode);
	void ResetOperation(int nSuccessful = -1);

	virtual void DoClose(int nError = 0);
	int GetReplyCode();
	void LogOnToServer(BOOL bSkipReply = FALSE);
	BOOL Send(CString str);
	
	BOOL ParsePwdReply(CString reply);

	CFile *m_pDataFile;
	CTransferSocket *m_pTransferSocket;
	CString m_MultiLine;
	CTime m_LastSendTime;
	
	CString m_ServerName;
	std::list<CString> m_RecvBuffer;
	CTime m_LastRecvTime;
	class CListData;
	class CFileTransferData;
	class CMakeDirData;
private:
	BOOL m_bCheckForTimeout;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_FTPCONTROLSOCKET_H__AE6AA44E_B09D_487A_8EF2_A23697434945__INCLUDED_
