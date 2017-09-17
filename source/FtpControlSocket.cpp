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

// CFtpControlSocket.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "FtpControlSocket.h"
#include "mainthread.h"
#include "transfersocket.h"
#include "fileexistsdlg.h"
#include "pathfunctions.h"

#include "asyncproxysocketlayer.h"
#include "AsyncSslSocketLayer.h"
#include "AsyncGssSocketLayer.h"

#include "filezillaapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CFtpControlSocket::CFileTransferData : public CFtpControlSocket::t_operation::COpData
{
public:
	CFileTransferData() 
	{
		pDirectoryListing=0;
		nGotTransferEndReply=0;
		nWaitNextOpState=0;
		bSentStor=FALSE;
		nMKDOpState=-1;
		pFileTime=0;
		pFileSize=0;
		bUseAbsolutePaths = FALSE;
		bTriedPortPasvOnce = FALSE;
	};
	~CFileTransferData() 
	{
		if (pDirectoryListing)
			delete pDirectoryListing;
		pDirectoryListing=0;
		delete pFileSize;
		delete pFileTime;
	};
	CString rawpwd;
	t_transferfile transferfile;
	t_transferdata transferdata;
	CString host;
	int port;
	BOOL bPasv;
	int nGotTransferEndReply;
	t_directory *pDirectoryListing;
	int nWaitNextOpState;
	BOOL bSentStor;
	CServerPath MKDCurrent;
	std::list<CString> MKDSegments;
	int nMKDOpState;
	CTime ListStartTime;
	CTime *pFileTime; //Used when downloading and OPTION_PRESERVEDOWNLOADFILETIME is set or when LIST fails
	_int64 *pFileSize; //Used when LIST failes
	BOOL bUseAbsolutePaths;
	BOOL bTriedPortPasvOnce;
};

class CFtpControlSocket::CListData:public CFtpControlSocket::t_operation::COpData
{
public:
	CListData() 
	{
		pDirectoryListing = 0;
		bTriedPortPasvOnce = FALSE;
	}
	virtual ~CListData() 
	{
		if (pDirectoryListing)
			delete pDirectoryListing;
	}
	CString rawpwd;
	CServerPath path;
	CString subdir;
	CServerPath realpath;
	int nListMode;
	BOOL bPasv;
	CString host;
	UINT port;
	int nFinish;
	t_directory *pDirectoryListing;
	CTime ListStartTime;
	BOOL bTriedPortPasvOnce;
};

class CFtpControlSocket::CMakeDirData : public CFtpControlSocket::t_operation::COpData
{
public:
	CMakeDirData() {}
	virtual ~CMakeDirData() {}
	CServerPath path;
	CServerPath Current;
	std::list<CString> Segments;
};

#define MKD_INIT -1
#define MKD_FINDPARENT 0
#define MKD_MAKESUBDIRS 1
#define MKD_CHANGETOSUBDIR 2

/////////////////////////////////////////////////////////////////////////////
// CFtpControlSocket

CFtpControlSocket::CFtpControlSocket(CMainThread *pMainThread) : CControlSocket(pMainThread)
{
	ASSERT(pMainThread);
	m_Operation.nOpMode=0;
	m_Operation.nOpState=-1;
	m_Operation.pData=0;
	m_pTransferSocket=0;
	m_pDataFile=0;
	m_pDirectoryListing=0;
	m_pOwner=pMainThread;
	srand( (unsigned)time( NULL ) );
	m_bKeepAliveActive=FALSE;
	m_bCheckForTimeout=TRUE;
}

CFtpControlSocket::~CFtpControlSocket()
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("~CFtpControlSocket()"));
	DoClose();
	if (m_pTransferSocket)
	{
		m_pTransferSocket->Close();
		delete m_pTransferSocket;
		m_pTransferSocket=0;
	}
	if (m_pDataFile)
	{
		delete m_pDataFile;
		m_pDataFile=0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Member-Funktion CFtpControlSocket 
#define CONNECT_INIT -1
#define CONNECT_GSS_INIT -2
#define CONNECT_GSS_AUTHDONE -3
#define CONNECT_GSS_CWD -4
#define CONNECT_GSS_FAILED -5
#define CONNECT_GSS_NEEDPASS -6
#define CONNECT_GSS_NEEDUSER -7
#define CONNECT_SSL_INIT -8
#define CONNECT_SSL_NEGOTIATE -9
#define CONNECT_SSL_WAITDONE -10
#define CONNECT_SSL_PBSZ -11
#define CONNECT_SSL_PROT -12

void CFtpControlSocket::Connect(t_server &server)
{
	USES_CONVERSION;

	ASSERT(!m_pOwner->IsConnected());
	
	ASSERT(!m_Operation.nOpMode);

	ASSERT(!m_pSslLayer);
	if (server.nServerType & FZ_SERVERTYPE_LAYER_SSL_IMPLICIT ||
		server.nServerType & FZ_SERVERTYPE_LAYER_SSL_EXPLICIT ||
		server.nServerType & FZ_SERVERTYPE_LAYER_TLS_EXPLICIT)
	{
		m_pSslLayer = new CAsyncSslSocketLayer;
		AddLayer(m_pSslLayer);
		TCHAR buffer[1000];
		GetModuleFileName(NULL, buffer, 1000);
		CString filename = buffer;
		int pos = filename.ReverseFind('\\');
		if (pos != -1)
		{
			filename = filename.Left(pos + 1);
			filename += _T("cacert.pem");
			m_pSslLayer->SetCertStorage(filename);
		}
		else
			filename = "cacert.pem";
	}

	ASSERT(!m_pProxyLayer);	
	if (!server.fwbypass)
	{
		int nProxyType = COptions::GetOptionVal(OPTION_PROXYTYPE);
		if (nProxyType != PROXYTYPE_NOPROXY)
		{
			m_pProxyLayer = new CAsyncProxySocketLayer;
			if (nProxyType == PROXYTYPE_SOCKS4)
				m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS4, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType==PROXYTYPE_SOCKS4A)
				m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS4A, T2CA(COptions::GetOption(OPTION_PROXYHOST)),COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType==PROXYTYPE_SOCKS5)
				if (COptions::GetOptionVal(OPTION_PROXYUSELOGON))
					m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS5, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT),
											T2CA(COptions::GetOption(OPTION_PROXYUSER)),
											T2CA(CCrypt::decrypt(COptions::GetOption(OPTION_PROXYPASS))));
				else
					m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS5, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType==PROXYTYPE_HTTP11)
				if (COptions::GetOptionVal(OPTION_PROXYUSELOGON))
					m_pProxyLayer->SetProxy(PROXYTYPE_HTTP11, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT),
											T2CA(COptions::GetOption(OPTION_PROXYUSER)),
											T2CA(CCrypt::decrypt(COptions::GetOption(OPTION_PROXYPASS))));
				else
					m_pProxyLayer->SetProxy(PROXYTYPE_HTTP11, T2CA(COptions::GetOption(OPTION_PROXYHOST)) ,COptions::GetOptionVal(OPTION_PROXYPORT));
			else
				ASSERT(FALSE);
			AddLayer(m_pProxyLayer);
		}
	}

	m_Operation.nOpMode=CSMODE_CONNECT;
	
	BOOL bUseGSS = FALSE;
	if (COptions::GetOptionVal(OPTION_USEGSS) && !m_pSslLayer)
	{
		CString GssServers=COptions::GetOption(OPTION_GSSSERVERS);
		LPCSTR lpszAscii=T2CA(server.host);
		hostent *fullname=gethostbyname(lpszAscii);
		CString host;
		if (fullname)
			host=fullname->h_name;
		else
			host=server.host;
		host.MakeLower();
		int i;
		while ((i=GssServers.Find(_T(";")))!=-1)
		{
			if (("."+GssServers.Left(i))==host.Right(GssServers.Left(i).GetLength()+1) || GssServers.Left(i)==host)
			{
				bUseGSS=TRUE;
				break;
			}
			GssServers=GssServers.Mid(i+1);
		}
	}
	if (bUseGSS)
	{
		m_pGssLayer = new CAsyncGssSocketLayer;
		AddLayer(m_pGssLayer);
		if (!m_pGssLayer->InitGSS())
		{
			ShowStatus("Unable to initialize GSS api", 1);
			DoClose(FZ_REPLY_CRITICALERROR);
			return;
		}
	}
	m_Operation.nOpState = m_pGssLayer?CONNECT_GSS_INIT:CONNECT_INIT;
	m_Operation.nOpMode = CSMODE_CONNECT;
	if (!Create())
	{
		DoClose();
		return;
	}
	AsyncSelect();

	if (server.nServerType&FZ_SERVERTYPE_LAYER_SSL_IMPLICIT)
	{
		ASSERT(m_pSslLayer);
		if (m_pSslLayer->InitClientSSL())
			PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SECURESERVER, 1), 0);
		else
		{
			DoClose();
			return;
		}
	}

	int logontype = COptions::GetOptionVal(OPTION_LOGONTYPE);
	int port;
	CString buf,temp;
	if (server.fwbypass)
		logontype=0;
	
	// are we connecting directly to the host (logon type 0) or via a firewall? (logon type>0)
	CString fwhost;
	int fwport;
	if(!logontype)
	{
		temp=server.host;
		port=server.port;
	}
	else
	{
		fwhost=COptions::GetOption(OPTION_FWHOST);
		fwport=COptions::GetOptionVal(OPTION_FWPORT);
		temp=fwhost;
		port=fwport;
		if(fwport!=21) 
			fwhost.Format( _T("%s:%d"), fwhost, fwport); // add port to fwhost (only if port is not 21)
	}
	m_CurrentServer = server;
	
	CString hostname = server.host;
	if(server.port!=21) 
		hostname.Format( _T("%s:%d"), hostname, server.port); // add port to hostname (only if port is not 21)
	CString str;
	str.Format(IDS_STATUSMSG_CONNECTING,hostname);
	ShowStatus(str,0);
	
	if ((server.nServerType&FZ_SERVERTYPE_LAYERMASK) & (FZ_SERVERTYPE_LAYER_SSL_EXPLICIT | FZ_SERVERTYPE_LAYER_TLS_EXPLICIT))
		m_Operation.nOpState=CONNECT_SSL_INIT;
	
	if (!CControlSocket::Connect(temp, port))
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			DoClose();
			return;
		}
	}
	m_ServerName = logontype?fwhost:hostname;
	m_LastRecvTime = m_LastSendTime = CTime::GetCurrentTime();
	
}

void CFtpControlSocket::LogOnToServer(BOOL bSkipReply /*=FALSE*/)
{
	int logontype = m_pGssLayer ? 0 : COptions::GetOptionVal(OPTION_LOGONTYPE);
	const int LO = -2, ER = -1;
	CString buf, temp;
	const int NUMLOGIN = 9; // currently supports 9 different login sequences
	int logonseq[NUMLOGIN][20] = {
		// this array stores all of the logon sequences for the various firewalls 
		// in blocks of 3 nums. 1st num is command to send, 2nd num is next point in logon sequence array
		// if 200 series response is rec'd from server as the result of the command, 3rd num is next
		// point in logon sequence if 300 series rec'd
		{0,LO,3, 1,LO,ER}, // no firewall
		{3,6,3,  4,6,ER, 5,9,9, 0,LO,12, 1,LO,ER}, // SITE hostname
		{3,6,3,  4,6,ER, 6,LO,9, 1,LO,ER}, // USER after logon
		{7,3,3,  0,LO,6, 1,LO,ER}, //proxy OPEN
		{3,6,3,  4,6,ER, 0,LO,9, 1,LO,ER}, // Transparent
		{6,LO,3, 1,LO,ER}, // USER remoteID@remotehost
		{8,6,3,  4,6,ER, 0,LO,9, 1,LO,ER}, //USER fireID@remotehost
		{9,ER,3, 1,LO,6, 2,LO,ER}, //USER remoteID@remotehost fireID
		{10,LO,3,11,LO,6,2,LO,ER} // USER remoteID@fireID@remotehost
	};
	if (m_CurrentServer.fwbypass)
		logontype = 0;
	
	if (m_Operation.nOpState == CONNECT_SSL_INIT)
	{
		if (m_CurrentServer.nServerType & FZ_SERVERTYPE_LAYER_SSL_EXPLICIT)
		{
			if (!Send("AUTH SSL"))
				return;
		}
		else
		{
			if (!Send("AUTH TLS"))
				return;
		}
		m_Operation.nOpState = CONNECT_SSL_NEGOTIATE;
		return;
	}
	else if (m_Operation.nOpState==CONNECT_SSL_NEGOTIATE)
	{
		int res=GetReplyCode();
		if (res!=2 && res!=3)
		{
			DoClose();
			return;
		}
		else
		{
			ASSERT(m_pSslLayer);
			if (!m_pSslLayer->InitClientSSL())
			{
				DoClose();
				return;
			}
		}
		m_Operation.nOpState = CONNECT_SSL_WAITDONE;
		return;
	}
	else if (m_Operation.nOpState == CONNECT_SSL_WAITDONE)
	{
		if (!Send("PBSZ 0"))
			return;
		m_Operation.nOpState = CONNECT_SSL_PBSZ;
		return;
	}
	else if (m_Operation.nOpState == CONNECT_SSL_PBSZ)
	{
		if (!Send("PROT P"))
			return;
		m_Operation.nOpState = CONNECT_SSL_PROT;
		return;
	}
	else if (m_Operation.nOpState == CONNECT_SSL_PROT)
		m_Operation.nOpState=CONNECT_INIT;
	else if (m_Operation.nOpState==CONNECT_GSS_FAILED ||
			 m_Operation.nOpState == CONNECT_GSS_NEEDPASS ||
			 m_Operation.nOpState == CONNECT_GSS_NEEDUSER)
	{
		if (!m_RecvBuffer.empty() && m_RecvBuffer.front() != _T(""))
		{
			//Incoming reply from server during async is not allowed
			DoClose();
			return;
		}
	}
	else if (!bSkipReply)
	{
		int res=GetReplyCode();
		if(res!=2 && res!=3 && m_Operation.nOpState>=0) // get initial connect msg off server
		{
			int nError=FZ_REPLY_ERROR;
			if (res==5 && logonseq[logontype][m_Operation.nOpState]==1)
				nError|=FZ_REPLY_CRITICALERROR;

			DoClose(nError);
			return; 
		}
	}
	CString hostname=m_CurrentServer.host;
	if(m_CurrentServer.port!=21) hostname.Format(hostname+":%d",m_CurrentServer.port); // add port to hostname (only if port is not 21)

	USES_CONVERSION;
	//**** GSS Authentication ****
	if (m_Operation.nOpState==CONNECT_GSS_INIT)  //authenticate
	{
		int	i = m_pGssLayer->GetClientAuth(T2CA(m_CurrentServer.host));
		if (i==-1)
			m_Operation.nOpState = CONNECT_GSS_AUTHDONE;
		else if (i != GSSAPI_AUTHENTICATION_SUCCEEDED)
		{
			m_Operation.nOpState = CONNECT_GSS_FAILED;
			CAsyncRequestData *pData=new CAsyncRequestData;
			pData->nRequestType=FZ_ASYNCREQUEST_GSS_AUTHFAILED;
			pData->nRequestID=m_pOwner->GetNextAsyncRequestID();
			if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_GSS_AUTHFAILED), (LPARAM)pData))
				delete pData;
		}
		else
		{
			// we got authentication, we need to check whether we have forwardable tickets
			//ShowStatus(IDS_STATUSMSG_GSSAUTH, 0);
			PostMessage(m_pOwner->m_hOwnerWnd,m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SECURESERVER, TRUE), 0);
			if (Send("CWD ."))
				m_Operation.nOpState = CONNECT_GSS_CWD;
		}
		return;
	}
	else if (m_Operation.nOpState == CONNECT_GSS_AUTHDONE)
	{
		if (!m_pGssLayer->AuthSuccessful())
		{
			m_Operation.nOpState = CONNECT_GSS_FAILED;
			CAsyncRequestData *pData=new CAsyncRequestData;
			pData->nRequestType = FZ_ASYNCREQUEST_GSS_AUTHFAILED;
			pData->nRequestID = m_pOwner->GetNextAsyncRequestID();
			if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_GSS_AUTHFAILED), (LPARAM)pData))
				delete pData;
			return;
		}
		else
		{
			// we got authentication, we need to check whether we have forwardable tickets
			//ShowStatus(IDS_STATUSMSG_GSSAUTH, 0);
			PostMessage(m_pOwner->m_hOwnerWnd,m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SECURESERVER, TRUE), 0);
			if (Send("CWD ."))
				m_Operation.nOpState = CONNECT_GSS_CWD;
			return;
		}
	}
	else if (m_Operation.nOpState == CONNECT_GSS_CWD)
	{ // authentication succeeded, we're now get the response to the CWD command
		if (GetReplyCode() == 2) // we're logged on
		{
			ShowStatus(IDS_STATUSMSG_CONNECTED,0);
			m_pOwner->SetConnected(TRUE);
			ResetOperation(FZ_REPLY_OK);
			return;
		}
		else
			//GSS authentication complete but we still have to go through the standard logon procedure
			m_Operation.nOpState = CONNECT_INIT;
	}
	
	if (m_Operation.nOpState==CONNECT_INIT)
	{
		if (logontype)
		{
		
			CString str;
			str.Format(IDS_STATUSMSG_FWCONNECT,hostname);
			ShowStatus(str,0);
		}
		m_Operation.nOpState++;
	}
	else if (!bSkipReply)
	{
		m_Operation.nOpState=logonseq[logontype][m_Operation.nOpState+GetReplyCode()-1]; //get next command from array
		switch(m_Operation.nOpState)
		{
		case ER: // ER means summat has gone wrong
			DoClose();
			return;
		case LO: //LO means we are logged on
			m_pOwner->SetConnected(TRUE);
			ShowStatus(IDS_STATUSMSG_CONNECTED,0);
			ResetOperation(FZ_REPLY_OK);
			return;
		}
	}
	
	// go through appropriate logon procedure
	int i = logonseq[logontype][m_Operation.nOpState];
	if (m_pGssLayer)
	{
		if ((i == 0 || i == 6 || i == 9 || i == 10) &&
			(m_CurrentServer.user == "anonymous" || m_CurrentServer.user == ""))
		{
			//Extract user from kerberos ticket
			char str[256];
			if (m_pGssLayer->GetUserFromKrbTicket(str))
				m_CurrentServer.user = str;
			if (m_CurrentServer.user == "")
			{
				CGssNeedUserRequestData *pData = new CGssNeedUserRequestData;
				pData->nRequestID = m_pOwner->GetNextAsyncRequestID();
				pData->nOldOpState = m_Operation.nOpState;
				m_Operation.nOpState = CONNECT_GSS_NEEDUSER;
				if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_GSS_NEEDUSER), (LPARAM)pData))
					delete pData;
				return;
			}
		}
		else if ((i == 1 || i == 11) && (m_CurrentServer.pass == "anon@" || m_CurrentServer.pass == ""))
		{
			CGssNeedPassRequestData *pData=new CGssNeedPassRequestData;
			pData->nRequestID=m_pOwner->GetNextAsyncRequestID();
			pData->nOldOpState = m_Operation.nOpState;
			m_Operation.nOpState = CONNECT_GSS_NEEDPASS;
			if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_GSS_NEEDPASS), (LPARAM)pData))
				delete pData;
			return;
		}
	}
	switch(logonseq[logontype][m_Operation.nOpState]) 
	{
		case 0:
			temp="USER "+m_CurrentServer.user;
			break;
		case 1:
			temp="PASS "+m_CurrentServer.pass;
			break;
		case 2:
			temp="ACCT "+CCrypt::decrypt(COptions::GetOption(OPTION_FWPASS));
			break;
		case 3:
			temp="USER "+COptions::GetOption(OPTION_FWUSER);
			break;
		case 4:
			temp="PASS "+CCrypt::decrypt(COptions::GetOption(OPTION_FWPASS));
			break;
		case 5:
			temp="SITE "+hostname;
			break;
		case 6:
			temp="USER "+m_CurrentServer.user+"@"+hostname;
			break;
		case 7:
			temp="OPEN "+hostname;
			break;
		case 8:
			temp="USER "+COptions::GetOption(OPTION_FWUSER)+"@"+hostname;
			break;
		case 9:
			temp="USER "+m_CurrentServer.user+"@"+hostname+" "+COptions::GetOption(OPTION_FWUSER);
			break;
		case 10:
			temp="USER "+m_CurrentServer.user+"@"+COptions::GetOption(OPTION_FWUSER)+"@"+hostname;
			break;
		case 11:
			temp="PASS "+m_CurrentServer.pass+"@"+CCrypt::decrypt(COptions::GetOption(OPTION_FWPASS));
			break;
	}
	// send command, get response
	if(!Send(temp))
		return;
}

#define BUFFERSIZE 4096
void CFtpControlSocket::OnReceive(int nErrorCode) 
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnReceive(%d)  OpMode=%d OpState=%d"), nErrorCode, m_Operation.nOpMode, m_Operation.nOpState);

	m_LastRecvTime=CTime::GetCurrentTime();
	PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SOCKETSTATUS, FZ_SOCKETSTATUS_RECV), 0);

	if (!m_pOwner->IsConnected())
	{
		if (!m_Operation.nOpMode)
		{
			LogMessage(__FILE__, __LINE__, this,FZ_LOG_INFO, _T("Socket has been closed, don't process receive") );
			return;
		}
		m_MultiLine=_T("");
		CString str;
		str.Format(IDS_STATUSMSG_CONNECTEDWITH,m_ServerName);
		ShowStatus(str,0);
		m_pOwner->SetConnected(TRUE);
	}
	char *buffer=new char[BUFFERSIZE];
	AsyncSelect(FD_CLOSE);
	int numread=Receive(buffer, BUFFERSIZE);
	while (numread!=SOCKET_ERROR && numread)
	{
		for (int i=0;i<numread;i++)
		{
			if ((buffer[i]=='\r')||(buffer[i]=='\n')||(buffer[i]==0))
			{
				if (!m_RecvBuffer.empty() && m_RecvBuffer.back()!=_T("") )
				{
					ShowStatus(m_RecvBuffer.back(), 3);
					//Check for multi-line responses
					if (m_RecvBuffer.back().GetLength()>3)
					{
						if (m_MultiLine!=_T(""))
						{
							if (m_RecvBuffer.back().Left(4)!=m_MultiLine)
	 							m_RecvBuffer.pop_back();
							else // end of multi-line found
 							{
 								m_MultiLine=_T("");
 								m_pOwner->PostThreadMessage(m_pOwner->m_nInternalMessageID,FZAPI_THREADMSG_PROCESSREPLY,0);
 							}
						}
						// start of new multi-line
						else if (m_RecvBuffer.back()[3]=='-')
						{
							// DDD<SP> is the end of a multi-line response
							m_MultiLine=m_RecvBuffer.back().Left(3)+' ';
							m_RecvBuffer.pop_back();
						}
						else
							m_pOwner->PostThreadMessage(m_pOwner->m_nInternalMessageID,FZAPI_THREADMSG_PROCESSREPLY,0);
					}
					else
						m_RecvBuffer.pop_back();
					m_RecvBuffer.push_back(_T(""));
				}
			}
			else
			{
				//The command may only be 2000 chars long. This ensures that a malicious user can't
				//send extremely large commands to fill the memory of the server
				if (m_RecvBuffer.empty())
					m_RecvBuffer.push_back(_T(""));
				if (m_RecvBuffer.back().GetLength()<2000)
					m_RecvBuffer.back()+=buffer[i];
			}
		}
		if (numread<BUFFERSIZE)
			break;
		numread=Receive(buffer,BUFFERSIZE);
		if (numread && numread!=SOCKET_ERROR)
		{
			m_LastRecvTime=CTime::GetCurrentTime();
			PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SOCKETSTATUS, FZ_SOCKETSTATUS_RECV), 0);
		}
	}
	delete buffer;
	if (numread==SOCKET_ERROR)
		if (GetLastError()!=WSAEWOULDBLOCK)
		{
			ShowStatus(IDS_STATUSMSG_DISCONNECTED,1);
			DoClose();
			return;
		}
	AsyncSelect(FD_READ | FD_WRITE | FD_CLOSE);	
}

void CFtpControlSocket::ProcessReply()
{
	if (m_RecvBuffer.empty())
		return;
	CString reply=m_RecvBuffer.front();
	if ( reply==_T("") )
		return;
	if (m_bKeepAliveActive)
	{
		m_bKeepAliveActive=FALSE;
		m_pOwner->PostThreadMessage(m_pOwner->m_nInternalMessageID,FZAPI_THREADMSG_POSTKEEPALIVE,0);
	}
	else if (m_Operation.nOpMode&CSMODE_CONNECT)
		LogOnToServer();
	else if (m_Operation.nOpMode& (CSMODE_COMMAND|CSMODE_CHMOD) )
	{
		if (GetReplyCode()==2 || GetReplyCode()==3)
			ResetOperation(FZ_REPLY_OK);
		else
			ResetOperation(FZ_REPLY_ERROR);
	}
	else if (m_Operation.nOpMode&CSMODE_TRANSFER)
	{
		FileTransfer(0);
	}
	else if (m_Operation.nOpMode&CSMODE_LIST)
		List(FALSE);
	else if (m_Operation.nOpMode&CSMODE_DELETE)
		Delete( _T(""),CServerPath());
	else if (m_Operation.nOpMode&CSMODE_RMDIR)
		RemoveDir( _T(""),CServerPath());
	else if (m_Operation.nOpMode&CSMODE_MKDIR)
		MakeDir(CServerPath());
	else if (m_Operation.nOpMode&CSMODE_RENAME)
		Rename(_T(""), _T(""), CServerPath(), CServerPath());
	if (!m_RecvBuffer.empty())
		m_RecvBuffer.pop_front();
}

void CFtpControlSocket::OnConnect(int nErrorCode) 
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnConnect(%d)  OpMode=%d OpState=%d"), nErrorCode, m_Operation.nOpMode, m_Operation.nOpState);

	if (!m_Operation.nOpMode)
	{
		if (!m_pOwner->IsConnected())
			DoClose();
		return;
	}
	if (!nErrorCode)
	{
		if (!m_pOwner->IsConnected())
		{
			m_MultiLine=_T("");
			m_pOwner->SetConnected(TRUE);
			CString str;
			str.Format(m_pSslLayer ? IDS_STATUSMSG_CONNECTEDWITHSSL : IDS_STATUSMSG_CONNECTEDWITH, m_ServerName);
			ShowStatus(str,0);
		}
	}
	else
	{
		if (nErrorCode == WSAHOST_NOT_FOUND)
			ShowStatus(IDS_ERRORMSG_CANTRESOLVEHOST, 1);
		DoClose();
	}
}

BOOL CFtpControlSocket::Send(CString str)
{
	USES_CONVERSION;
	
	ShowStatus(str,2);
	str+="\r\n";
	LPCSTR lpszAsciiSend=T2CA(str);
	int res = CAsyncSocketEx::Send(lpszAsciiSend,strlen(lpszAsciiSend));
	if (!res)
	{
		if (GetLastError()!=WSAEWOULDBLOCK)
		{
			ShowStatus(IDS_ERRORMSG_CANTSENDCOMMAND,1);
			DoClose();
			return FALSE;
		}
	}
	m_LastSendTime=CTime::GetCurrentTime();
	PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SOCKETSTATUS, FZ_SOCKETSTATUS_SEND), 0);
	return TRUE;
}

int CFtpControlSocket::GetReplyCode()
{
	if (m_RecvBuffer.empty())
		return 0;
	CString str = m_RecvBuffer.front();
	if (str == "")
		return 0;
	else
		return str[0]-'0';
}

void CFtpControlSocket::DoClose(int nError /*=0*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("DoClose(%d)  OpMode=%d OpState=%d"), nError, m_Operation.nOpMode, m_Operation.nOpState);
	
	m_bCheckForTimeout=TRUE;
	m_pOwner->SetConnected(FALSE);
	m_bKeepAliveActive=FALSE;
	PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SECURESERVER, FALSE), 0);
	if (nError & FZ_REPLY_CRITICALERROR)
		nError |= FZ_REPLY_ERROR;
	ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_DISCONNECTED|nError);
	m_RecvBuffer.clear();
	m_MultiLine="";
	CControlSocket::Close();
}

void CFtpControlSocket::Disconnect()
{
	ASSERT(!m_Operation.nOpMode);
	m_Operation.nOpMode=CSMODE_DISCONNECT;
	DoClose();
	ShowStatus(IDS_STATUSMSG_DISCONNECTED,0); //Send the disconnected message to the message log		
}

void CFtpControlSocket::CheckForTimeout()
{
	if (!m_Operation.nOpMode && !m_bKeepAliveActive)
		return;
	if (!m_bCheckForTimeout)
		return;
	int delay=COptions::GetOptionVal(OPTION_TIMEOUTLENGTH);
	if (m_pTransferSocket)
	{
		int res=m_pTransferSocket->CheckForTimeout(delay);
		if (res)
			return;
	}
	CTimeSpan span=CTime::GetCurrentTime()-m_LastRecvTime;             
	if (span.GetTotalSeconds()>=delay)
	{
		ShowStatus(IDS_ERRORMSG_TIMEOUT, 1);
		DoClose();
	}
}

void CFtpControlSocket::FtpCommand(LPCTSTR pCommand)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("FtpCommand(%s)  OpMode=%d OpState=%d"), pCommand, m_Operation.nOpMode, m_Operation.nOpState);
	m_Operation.nOpMode=CSMODE_COMMAND;
	Send(pCommand);
}

void CFtpControlSocket::Cancel(BOOL bQuit/*=FALSE*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("Cancel(%s)  OpMode=%d OpState=%d"), bQuit?_T("TRUE"):_T("FALSE"), m_Operation.nOpMode, m_Operation.nOpState);
	int nOpMode=m_Operation.nOpMode;
	if (m_Operation.nOpMode&CSMODE_LIST || m_Operation.nOpMode&CSMODE_TRANSFER)
	{
		Send("ABOR");
		if (m_pTransferSocket)
		{
			m_pTransferSocket->Close();
			delete m_pTransferSocket;
			m_pTransferSocket=0;
		}
	}
	if (m_Operation.nOpMode==CSMODE_CONNECT)
		DoClose(FZ_REPLY_CANCEL);
	else if (m_Operation.nOpMode&CSMODE_TRANSFER)
		ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_CANCEL|FZ_REPLY_ABORTED);
	else if (m_Operation.nOpMode!=CSMODE_NONE)
		ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_CANCEL);
		
	if (nOpMode!=CSMODE_NONE && !bQuit)
		ShowStatus(IDS_ERRORMSG_INTERRUPTED,1);
}

void CFtpControlSocket::List(BOOL bFinish, int nError /*=FALSE*/, CServerPath path /*=CServerPath()*/, CString subdir /*=""*/,int nListMode/*=0*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("List(%s,%d,\"%s\",\"%s\",%d)  OpMode=%d OpState=%d"), bFinish?_T("TRUE"):_T("FALSE"), nError, path.GetPath(), subdir, nListMode,
				m_Operation.nOpMode, m_Operation.nOpState);

	USES_CONVERSION;

	#define LIST_INIT	-1
	#define LIST_PWD	0
	#define LIST_CWD	1
	#define LIST_PWD2	2
	#define LIST_CWD2	3
	#define LIST_PWD3	4
	#define LIST_PORT_PASV	5
	#define LIST_TYPE	6
	#define LIST_LIST	7
	#define LIST_WAITFINISH	8

	ASSERT(!m_Operation.nOpMode || m_Operation.nOpMode&CSMODE_LIST);
	
	m_Operation.nOpMode|=CSMODE_LIST;

	if (!m_pOwner->IsConnected())
	{
		ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_NOTCONNECTED);
		return;
	}

	if (bFinish || nError)
		if (m_Operation.nOpMode!=CSMODE_LIST)
			return; //Old message coming in
	
	if (nError)
	{
		delete m_pTransferSocket;
		m_pTransferSocket=0;
		if (nError&CSMODE_TRANSFERTIMEOUT)
			DoClose();
		else
			ResetOperation(FZ_REPLY_ERROR);
		return;
	}

	CListData *pData = static_cast<CListData *>(m_Operation.pData);

	if (bFinish)
	{
		if (!m_pTransferSocket || m_pTransferSocket->m_bListening)
		{
			delete m_pDirectoryListing;
			m_pDirectoryListing=0;
			delete m_pTransferSocket;
			m_pTransferSocket=0;			
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}

		int num=0;
		pData->pDirectoryListing=new t_directory;
		if (COptions::GetOptionVal(OPTION_DEBUGSHOWLISTING))
			m_pTransferSocket->m_pListResult->SendToMessageLog(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID);
		pData->pDirectoryListing->direntry=m_pTransferSocket->m_pListResult->getList(num, pData->ListStartTime);
		pData->pDirectoryListing->num=num;
		if (m_pTransferSocket->m_pListResult->m_server.nServerType&FZ_SERVERTYPE_SUB_FTP_VMS && m_CurrentServer.nServerType&FZ_SERVERTYPE_FTP)
			m_CurrentServer.nServerType |= FZ_SERVERTYPE_SUB_FTP_VMS;
		pData->pDirectoryListing->server = m_CurrentServer;
		pData->pDirectoryListing->path.SetServer(pData->pDirectoryListing->server);
		if (pData->rawpwd != "")
		{
			if (!pData->pDirectoryListing->path.SetPath(pData->rawpwd))
			{
				delete m_pDirectoryListing;
				m_pDirectoryListing=0;
				delete m_pTransferSocket;
				m_pTransferSocket=0;
				ResetOperation(FZ_REPLY_ERROR);
				return;
			}
		}
		else
			pData->pDirectoryListing->path=pData->realpath;
		
		if (m_Operation.nOpState!=LIST_WAITFINISH)
			return;
		else
		{
			delete m_pTransferSocket;
			m_pTransferSocket=0;
		}
	}
	
	if (m_Operation.nOpState==LIST_WAITFINISH)
	{
		if (!bFinish)
		{
			if (pData->nFinish==-1)
			{
				int code=GetReplyCode();
				if (code==2)
				{
					pData->nFinish=1;
				}
				else
					pData->nFinish=0;
			}
		}
		else
		{
			if (m_pTransferSocket)
				delete m_pTransferSocket;
			m_pTransferSocket=0;
		}
		if (pData->nFinish==0)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		else if (pData->pDirectoryListing && pData->nFinish==1)
		{
			ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
			CDirectoryCache cache;
			cache.Lock();
			t_directory dir;
			if (!pData->path.IsEmpty() && pData->subdir!="")
			{
				if (cache.Lookup(pData->pDirectoryListing->path, pData->pDirectoryListing->server, dir))
					pData->pDirectoryListing->Merge(dir, pData->ListStartTime);
				cache.Store(*pData->pDirectoryListing,pData->path,pData->subdir);
			}
			else
			{
				if (cache.Lookup(pData->pDirectoryListing->path, pData->pDirectoryListing->server, dir))
					pData->pDirectoryListing->Merge(dir, pData->ListStartTime);
				cache.Store(*pData->pDirectoryListing);
			}
			cache.Unlock();
			SetDirectoryListing(pData->pDirectoryListing);
			ResetOperation(FZ_REPLY_OK);			
			return;
		}
		return;
	}
	else if (m_Operation.nOpState!=LIST_INIT)
	{
		CString retmsg = m_RecvBuffer.front();
		BOOL error = FALSE;
		int code = GetReplyCode();
		switch(m_Operation.nOpState)
		{
		case LIST_PWD: //Reply to PWD command
			if (code!=2 && code!=3)
				error = TRUE;
			else
			{
				if (!ParsePwdReply(retmsg))
					return;
			}
			m_Operation.nOpState = LIST_PORT_PASV;
			break;
		case LIST_CWD:
			if (code!=2 && code!=3)
				error = TRUE;
			m_Operation.nOpState = LIST_PWD2;
			break;
		case LIST_PWD2: //Reply to PWD command
			if (code !=2 && code != 3)
				error = TRUE;
			else
			{
				if (!ParsePwdReply(retmsg))
					return;
			}
			if (pData->subdir != "")
			{
				if (pData->path != pData->realpath)
				{
					ResetOperation(FZ_REPLY_ERROR);
					return;
				}
				m_Operation.nOpState = LIST_CWD2;
			}
			else
				m_Operation.nOpState = LIST_PORT_PASV;
			break;
		case LIST_CWD2:
			if (code != 2 && code != 3)
				error = TRUE;
			m_Operation.nOpState = LIST_PWD3;
			break;
		case LIST_PWD3: //Reply to PWD command
			if (code!=2 && code!=3)
				error=TRUE;
			else
			{
				if (!ParsePwdReply(retmsg))
					return;
			}
			m_Operation.nOpState = LIST_PORT_PASV;
			break;
		case LIST_PORT_PASV:
			if (code!=2 && code!=3)
			{
				error=TRUE;
				break;
			}
			if (pData->bPasv)
			{
				CString temp;
				int i,j;
				if((i=retmsg.Find(_T("(")))==-1||(j=retmsg.Find(_T(")")))==-1)
				{
					if (!pData->bTriedPortPasvOnce)
					{
						pData->bTriedPortPasvOnce = TRUE;
						pData->bPasv = !pData->bPasv;
					}
					else
						error=TRUE;
					break;
				}

				temp=retmsg.Mid(i+1,(j-i)-1);
				i=temp.ReverseFind(',');
				pData->port=atol(  T2CA( temp.Right(temp.GetLength()-(i+1)) )  ); //get ls byte of server socket
				temp=temp.Left(i);
				i=temp.ReverseFind(',');
				pData->port+=256*atol(  T2CA( temp.Right(temp.GetLength()-(i+1)) )  ); // add ms byte to server socket
				pData->host=temp.Left(i);
				while(1) { // convert commas to dots in IP
					if((i=pData->host.Find( _T(",") ))==-1) break;
					pData->host.SetAt(i,'.');
				}
			}
			m_Operation.nOpState = LIST_TYPE;
			break;
		case LIST_TYPE:
			if (code!=2 && code!=3)
				error=TRUE;
			m_Operation.nOpState=LIST_LIST;
			break;
		case LIST_LIST:
			if (code!=1)
				error=TRUE;
			else
				m_Operation.nOpState=LIST_WAITFINISH;
			break;
		default:
			error = TRUE;
		}
		
		if (error)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}				
	}
	if (m_Operation.nOpState==LIST_INIT)
	{ //Initialize some variables
		pData=new CListData;
		pData->nListMode=nListMode;
		pData->path=path;
		pData->subdir=subdir;
		m_Operation.pData=pData;
		ShowStatus(IDS_STATUSMSG_RETRIEVINGDIRLIST,0);
		pData->nFinish=-1;
		if (m_pDirectoryListing)
		{
			delete m_pDirectoryListing;
			m_pDirectoryListing=0;
		}
		
		if (COptions::GetOptionVal(OPTION_PROXYTYPE)!=PROXYTYPE_NOPROXY && !m_CurrentServer.fwbypass)
			pData->bPasv = TRUE;
		else if (m_CurrentServer.nPasv == 1)
			pData->bPasv = TRUE;
		else if (m_CurrentServer.nPasv == 2)
			pData->bPasv = FALSE;
		else
			pData->bPasv = COptions::GetOptionVal(OPTION_PASV);

		CServerPath path = pData->path;
		m_pOwner->GetCurrentPath(pData->realpath);
		CServerPath realpath=pData->realpath;
		if (!pData->realpath.IsEmpty())
		{
			if (!pData->path.IsEmpty() && pData->path!=pData->realpath)
				m_Operation.nOpState=LIST_CWD;
			else if (!pData->path.IsEmpty() && pData->subdir!="")
				m_Operation.nOpState=LIST_CWD2;
			else
			{
				if (pData->nListMode&FZ_LIST_REALCHANGE)
				{
					if (pData->subdir=="")
						m_Operation.nOpState=LIST_CWD;
					else
						m_Operation.nOpState=LIST_CWD2;
				}
				else
				{
					if (pData->nListMode&FZ_LIST_USECACHE)
					{
						t_directory dir;
						CDirectoryCache cache;
						BOOL res=cache.Lookup(pData->realpath,m_CurrentServer,dir);
						if (res)
						{
							BOOL bExact=TRUE;
							if (pData->nListMode & FZ_LIST_EXACT)
								for (int i=0;i<dir.num;i++)
									if (dir.direntry[i].bUnsure || (dir.direntry[i].size==-1 && !dir.direntry[i].dir))
									{
										bExact=FALSE;
										break;
									}
							if (bExact)
							{
								ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL,0);
								SetDirectoryListing(&dir);
								ResetOperation(FZ_REPLY_OK);
								return;
							}
						}
					}
					m_Operation.nOpState=LIST_PORT_PASV;
				}
			}
		}
		else
		{
			if (pData->path.IsEmpty())
				m_Operation.nOpState=LIST_PWD;
			else
				m_Operation.nOpState=LIST_CWD;
		}
	}
	CString cmd;
	if (m_Operation.nOpState==LIST_PWD)
		cmd=_T("PWD");
	else if (m_Operation.nOpState==LIST_CWD)
		cmd=_T("CWD ") + pData->path.GetPath(); //Command to retrieve the current directory
	else if (m_Operation.nOpState==LIST_PWD2)
		cmd=_T("PWD");
	else if (m_Operation.nOpState==LIST_CWD2)
	{
		if (!pData->subdir)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		if (pData->subdir != _T("..") )
			cmd=_T("CWD ") + pData->subdir;
		else
			cmd=_T("CDUP");
	}
	else if (m_Operation.nOpState==LIST_PWD3)
		cmd=_T("PWD");
	else if (m_Operation.nOpState==LIST_PORT_PASV)
	{
		m_pTransferSocket=new CTransferSocket(this, m_Operation.nOpMode);
		m_pTransferSocket->m_nInternalMessageID=m_pOwner->m_nInternalMessageID;
		if (m_pGssLayer && m_pGssLayer->AuthSuccessful())
			m_pTransferSocket->UseGSS(m_pGssLayer);
		if (!m_pTransferSocket->Create(m_pSslLayer?TRUE:FALSE) || 
			!m_pTransferSocket->AsyncSelect())
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		if (pData->bPasv)
			cmd=_T("PASV");
		else
		{ 
			m_pTransferSocket->m_bListening=TRUE;
			if (m_pProxyLayer)
			{
				SOCKADDR_IN addr;
				int len=sizeof(addr);
				if (!m_pProxyLayer->GetPeerName((SOCKADDR *)&addr,&len))
				{
					ResetOperation(FZ_REPLY_ERROR);
					return;
				}
				else if (!m_pTransferSocket->Listen(addr.sin_addr.S_un.S_addr))
				{
					ResetOperation(FZ_REPLY_ERROR);
					return;
				}
			}
			else
			{
				//Set up an active file transfer
				CString temp;
				UINT nPort;
				
				if (// create listen socket (let MFC choose the port) & start the socket listening
					!m_pTransferSocket->Listen() || 
					!m_pTransferSocket->GetSockName(temp, nPort))
				{
					ResetOperation(FZ_REPLY_ERROR);
					return;
				}
		
				CString host = COptions::GetOption(OPTION_TRANSFERIP);
				if (host != "")
				{
					DWORD ip = inet_addr(T2CA(host));
					if (ip != INADDR_NONE)
						host.Format(_T("%d,%d,%d,%d"), ip%256, (ip>>8)%256, (ip>>16)%256, ip>>24);
					else
					{
						hostent *fullname = gethostbyname(T2CA(host));
						if (!fullname)
							host = "";
						else
						{
							DWORD ip = ((LPIN_ADDR)fullname->h_addr)->s_addr;
							if (ip != INADDR_NONE)
								host.Format(_T("%d,%d,%d,%d"), ip%256, (ip>>8)%256, (ip>>16)%256, ip>>24);
							else
								host = "";
						}
					}
				}
				if (host == "")
				{
					UINT temp;

					if(!GetSockName(host, temp))
					{
						ResetOperation(FZ_REPLY_ERROR);
						return;
					}
					host.Replace('.', ',');
				}
				host.Format(host+",%d,%d", nPort/256, nPort%256);
				cmd = _T("PORT ") + host; // send PORT cmd to server
			}
		}
	}
	else if (m_Operation.nOpState==LIST_TYPE)
		cmd=_T("TYPE A");
	else if (m_Operation.nOpState==LIST_LIST)
	{
		if (!m_pTransferSocket)
		{
			LogMessage(__FILE__, __LINE__, this,FZ_LOG_APIERROR, _T("Error: m_pTransferSocket==NULL") );
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}

		m_pTransferSocket->SetActive();
		
		cmd="LIST";
		if ( m_pOwner->GetOption(FZAPI_OPTION_SHOWHIDDEN) )
			cmd+=" -a";
		
		if (!Send(cmd))
			return;
	
		pData->ListStartTime=CTime::GetCurrentTime();

		if (pData->bPasv) 
		{
			// if PASV create the socket & initiate outbound data channel connection
			if (!m_pTransferSocket->Connect(pData->host,pData->port))
			{
				if (GetLastError()!=WSAEWOULDBLOCK)
				{
					ResetOperation(FZ_REPLY_ERROR);
					return;
				}
			}
		}

		return;
	}
	if ( cmd != _T("") )
		Send(cmd);
}

void CFtpControlSocket::TransferEnd(int nMode)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("TransferEnd(%d)  OpMode=%d OpState=%d"), nMode, m_Operation.nOpMode, m_Operation.nOpState);
	if (!m_Operation.nOpMode)
	{
		LogMessage(__FILE__, __LINE__, this,FZ_LOG_INFO, _T("Ignoring old TransferEnd message"));
		return;
	}
	m_LastRecvTime=CTime::GetCurrentTime();
	if (m_Operation.nOpMode&CSMODE_TRANSFER)
		FileTransfer(0,TRUE,nMode&(CSMODE_TRANSFERERROR|CSMODE_TRANSFERTIMEOUT));
	else if (m_Operation.nOpMode&CSMODE_LIST)
		List(TRUE,nMode&(CSMODE_TRANSFERERROR|CSMODE_TRANSFERTIMEOUT));
}

void CFtpControlSocket::OnClose(int nErrorCode) 
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnClose(%d)  OpMode=%d OpState=%d"), nErrorCode, m_Operation.nOpMode, m_Operation.nOpState);
	ShowStatus(IDS_STATUSMSG_DISCONNECTED, 1);
	if (m_pTransferSocket)
	{
		m_pTransferSocket->OnClose(0);
		m_pTransferSocket->Close();
		delete m_pTransferSocket;
		m_pTransferSocket=0;
		DoClose();
		ShowStatus(IDS_ERRORMSG_TIMEOUT,1);
		return;
	}
	DoClose();
}

void CFtpControlSocket::FileTransfer(t_transferfile *transferfile/*=0*/,BOOL bFinish/*=FALSE*/,int nError/*=0*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("FileTransfer(%d, %s, %d)  OpMode=%d OpState=%d"), transferfile,bFinish?_T("TRUE"):_T("FALSE"), nError, m_Operation.nOpMode, m_Operation.nOpState);

	USES_CONVERSION;
	
	#define FILETRANSFER_INIT			-1 //Opt: LIST TYPE
	#define FILETRANSFER_CWD			0 //PWD
	#define FILETRANSFER_MKD			1
	#define FILETRANSFER_CWD2			2
	#define FILETRANSFER_PWD			3
	#define FILETRANSFER_LIST_PORTPASV	4
	#define FILETRANSFER_LIST_TYPE		5
	#define FILETRANSFER_LIST_LIST		6
	#define FILETRANSFER_LIST_WAITFINISH	7
	#define FILETRANSFER_NOLIST_SIZE	8
	#define FILETRANSFER_NOLIST_MDTM	9
	#define FILETRANSFER_TYPE			10
	#define FILETRANSFER_REST			11
	#define FILETRANSFER_PORTPASV		12
	#define FILETRANSFER_RETRSTOR		13
	#define FILETRANSFER_WAITFINISH		14

	#define FILETRANSFER_WAIT			15


	//Flowchart of FileTransfer
	//
	//            +----+
	//     /------|Init|--------\
	//     |      +----+        |
	//     |         |          |
	//     |         |          |
	//     |       +---+        |
	//     |       |CWD|--\     |
	//     |       +---+  |     |
	//     |         |    |     |
	//     |         |    |     |
	//     |         |  +---+   |
	//     |         |  |MKD|   |
	//     |         |  +---+   |
	//     |         |    |     |
	//     |         |    |     |
	//     |         |  +----+  |
	//     |         |  |CWD2|  |
	//     |         |  +----+  |
	//     |         |    |     |
	//     |         +----/     |
	//     |         |          |
	//     |       +---+        |
	//     +-------|PWD|        |
	//     |       +---+        |
	//     |         |          |
	//     |         +----------/
	//     |         |
	//     |  +-------------+
	//     |  |LIST_PORTPASV|
	//     |  +-------------+
	//     |         |
	//     |         |
	//     |    +---------+
	//     |    |LIST_TYPE|
	//     |    +---------+
	//     |         |
	//     |         |
	//     |    +---------+
	//     |    |LIST_LIST|-----\ //List failes, maybe folder is list protected
	//     |    +---------+     | //Use SIZE and MDTM to get file information
	//     |         |        +----+
	//     |         |        |SIZE|
	//     |         |        +----+
	//     |         |          |
	//     |         |        +----+
	//     |         |        |MDTM|
	//     |         |        +----+
	//     |         |          |
	//     |         |          |
	//     | +---------------+  |
	//     | |LIST_WAITFINISH|  |
	//     | +---------------+  |
	//     |         |          |
	//     |         |          |
	//     |         +----------/
	//     |         |
	//     \---------+
	//               |
	//            +----+
	//            |TYPE|
	//            +----+
	//               |
	//               |
	//          +--------+
	//          |PORTPASV|--\
	//          +--------+  |
	//               |      |
	//               |      |
	//               |   +----+
	//               |   |REST|
	//               |   +----+
	//				 |      |
	//               +------/
	//               |
	//          +--------+
	//          |RETRSTOR|
	//          +--------+
	//               |
	//               |
	//         +----------+
	//         |WAITFINISH|
	//         +----------+
	
	ASSERT(!m_Operation.nOpMode || m_Operation.nOpMode&CSMODE_TRANSFER);
	if (!m_pOwner->IsConnected())
	{
		m_Operation.nOpMode=CSMODE_TRANSFER|(transferfile->get?CSMODE_DOWNLOAD:CSMODE_UPLOAD);
		ResetOperation(FZ_REPLY_ERROR|FZ_REPLY_DISCONNECTED);
		return;
	}

	CFileTransferData *pData=static_cast<CFileTransferData *>(m_Operation.pData);

	//Process finish and error messages
	if (bFinish || nError)
	{
		ASSERT(m_Operation.nOpMode&CSMODE_TRANSFER);
		
		if (!(m_Operation.nOpMode&CSMODE_TRANSFER))
			return;
		
		if (nError)
		{
			if (m_Operation.nOpState==FILETRANSFER_LIST_LIST && nError&CSMODE_TRANSFERERROR)
			{ //Don't abort operation, use fallback to SIZE and MDTM (when actual LIST reply comes in)
				if (m_pTransferSocket)
					m_pTransferSocket=0;
				delete m_pDirectoryListing;
				m_pDirectoryListing=0;
			}
			else if (nError&CSMODE_TRANSFERTIMEOUT)
				DoClose();
			else
				ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		if (m_Operation.nOpState<=FILETRANSFER_LIST_PORTPASV)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		else if (m_Operation.nOpState<=FILETRANSFER_LIST_WAITFINISH)
		{
			if (!m_pTransferSocket || m_pTransferSocket->m_bListening)
			{
				delete m_pDirectoryListing;
				m_pDirectoryListing=0;
				ResetOperation(FZ_REPLY_ERROR);
				return;
			}
		
			int num=0;
			pData->pDirectoryListing=new t_directory;
			if (COptions::GetOptionVal(OPTION_DEBUGSHOWLISTING))
				m_pTransferSocket->m_pListResult->SendToMessageLog(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID);
			pData->pDirectoryListing->direntry=m_pTransferSocket->m_pListResult->getList(num, pData->ListStartTime);
			pData->pDirectoryListing->num=num;
			if (m_pTransferSocket->m_pListResult->m_server.nServerType&FZ_SERVERTYPE_SUB_FTP_VMS && m_CurrentServer.nServerType&FZ_SERVERTYPE_FTP)
				m_CurrentServer.nServerType |= FZ_SERVERTYPE_SUB_FTP_VMS;
			pData->pDirectoryListing->server = m_CurrentServer;	
			pData->pDirectoryListing->path.SetServer(m_CurrentServer);
			pData->pDirectoryListing->path = pData->transferfile.remotepath;
			if (pData->rawpwd!="")
			{
				if (!pData->pDirectoryListing->path.SetPath(pData->rawpwd))
				{
					delete m_pDirectoryListing;
					m_pDirectoryListing=0;
					delete m_pTransferSocket;
					m_pTransferSocket=0;
					ResetOperation(FZ_REPLY_ERROR);
					return;
				}
			}
			else
				pData->pDirectoryListing->path=pData->transferfile.remotepath;
			
			if (m_Operation.nOpState!=FILETRANSFER_LIST_WAITFINISH)
				return;
		}
		else if (m_Operation.nOpState<=FILETRANSFER_PORTPASV)
		{
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		else if (m_Operation.nOpState<=FILETRANSFER_WAITFINISH)
		{
			if (m_pTransferSocket->m_bListening)
			{
				ResetOperation(FZ_REPLY_ERROR);
				return;
			}
			pData->nGotTransferEndReply|=2;
			if (m_Operation.nOpState!=FILETRANSFER_WAITFINISH)
				return;
			else
			{
				delete m_pTransferSocket;
				m_pTransferSocket=0;
			}
		}
	}

	//////////////////
	//Initialization//
	//////////////////
	int nReplyError=0;
	if (m_Operation.nOpState==FILETRANSFER_INIT)
	{
		ASSERT(transferfile);
		ASSERT(!m_Operation.nOpMode);
		ASSERT(!m_Operation.pData);

		CString str;
		str.Format(transferfile->get?IDS_STATUSMSG_DOWNLOADSTART:IDS_STATUSMSG_UPLOADSTART,transferfile->get?transferfile->remotepath.GetPath()+transferfile->remotefile:transferfile->localfile);
		ShowStatus(str,0);
		
		m_Operation.nOpMode=CSMODE_TRANSFER|(transferfile->get?CSMODE_DOWNLOAD:CSMODE_UPLOAD);

		m_Operation.pData=new CFileTransferData;
		pData=static_cast<CFileTransferData *>(m_Operation.pData);

		if (COptions::GetOptionVal(OPTION_PROXYTYPE)!=PROXYTYPE_NOPROXY && !m_CurrentServer.fwbypass)
			pData->bPasv = TRUE;
		else if (m_CurrentServer.nPasv == 1)
			pData->bPasv = TRUE;
		else if (m_CurrentServer.nPasv == 2)
			pData->bPasv = FALSE;
		else
			pData->bPasv = COptions::GetOptionVal(OPTION_PASV);

		//Replace invalid characters in the local filename
		int pos=transferfile->localfile.ReverseFind('\\');
		for (int i=(pos+1);i<transferfile->localfile.GetLength();i++)
			if (transferfile->localfile[i]==':')
				transferfile->localfile.SetAt(i, '_');

		pData->transferfile=*transferfile;
		pData->transferdata.transfersize=pData->transferfile.size;
		pData->transferdata.transferleft=pData->transferfile.size;
		pData->transferdata.bResume = FALSE;
		pData->transferdata.bType = (pData->transferfile.nType == 1) ? TRUE : FALSE;
		
		CServerPath path;
		VERIFY (m_pOwner->GetCurrentPath(path));
		if (path==pData->transferfile.remotepath)
		{
			if (m_pDirectoryListing)
			{
				m_Operation.nOpState=FILETRANSFER_TYPE;
				CString remotefile=pData->transferfile.remotefile;
				int i;
				for (i=0; i<m_pDirectoryListing->num; i++)
				{
					if (m_pDirectoryListing->direntry[i].name==remotefile &&
						( m_pDirectoryListing->direntry[i].bUnsure || m_pDirectoryListing->direntry[i].size==-1 ))
					{
						delete m_pDirectoryListing;
						m_pDirectoryListing=0;
						m_Operation.nOpState = FILETRANSFER_LIST_PORTPASV;
						break;
					}
				}				
				if (m_pDirectoryListing && i==m_pDirectoryListing->num)
				{
					nReplyError=CheckOverwriteFile();
					if (!nReplyError)
					{
						if (pData->transferfile.get)
						{
							CString path=pData->transferfile.localfile;
							if (path.ReverseFind('\\')!=-1)
							{
								path=path.Left(path.ReverseFind('\\')+1);
								CString path2;
								while (path!="")
								{
									path2+=path.Left(path.Find( _T("\\") )+1);
									path=path.Mid(path.Find( _T("\\") )+1);
									int res=CreateDirectory(path2,0);
								}
							}
						}
					}
				}
			}
			else
			{
				CDirectoryCache cache;
				t_server server;
				m_pOwner->GetCurrentServer(server);
				t_directory dir;
				BOOL res=cache.Lookup(pData->transferfile.remotepath,server,dir);
				if (res)
				{
					CString remotefile=pData->transferfile.remotefile;
					int i;
					for (i=0; i<dir.num; i++)
					{
						if (dir.direntry[i].name==remotefile &&
							( dir.direntry[i].bUnsure || dir.direntry[i].size==-1 ))
						{
							m_Operation.nOpState = FILETRANSFER_LIST_PORTPASV;
							break;
						}
					}
					if (i == dir.num)
					{
						SetDirectoryListing(&dir);
						m_Operation.nOpState=FILETRANSFER_TYPE;
						nReplyError=CheckOverwriteFile();
						if (!nReplyError)
						{
							if (pData->transferfile.get)
							{
								CString path=pData->transferfile.localfile;
								if (path.ReverseFind('\\')!=-1)
								{
									path=path.Left(path.ReverseFind('\\')+1);
									CString path2;
									while (path!="")
									{
										path2+=path.Left(path.Find( _T("\\") )+1);
										path=path.Mid(path.Find(_T( "\\") )+1);
										int res=CreateDirectory(path2,0);
									}
								}
							}
						}
					}
				}
				else
					m_Operation.nOpState = FILETRANSFER_LIST_PORTPASV;
			}
		}
		else
		{
			m_Operation.nOpState=FILETRANSFER_CWD;
		}
	}
	else
	{
		///////////
		//Replies//
		///////////
		switch(m_Operation.nOpState)
		{
		case FILETRANSFER_CWD:
			if (GetReplyCode()!=2 && GetReplyCode()!=3)
				if (GetReplyCode()==4)
					nReplyError=FZ_REPLY_ERROR;
				else if (pData->transferfile.get)
				{
					pData->bUseAbsolutePaths = TRUE;
					m_Operation.nOpState = FILETRANSFER_NOLIST_SIZE;
				}
				else
					m_Operation.nOpState=FILETRANSFER_MKD;
			else
				m_Operation.nOpState=FILETRANSFER_PWD;
			break;
		case FILETRANSFER_MKD:
			switch(pData->nMKDOpState)
			{

			case MKD_FINDPARENT:
				{
					int res=GetReplyCode();
					if (res==2 || res==3)
					{
						m_pOwner->SetCurrentPath(pData->MKDCurrent);

						if (!m_pDirectoryListing)
						{
							CDirectoryCache cache;
							t_directory dir;
							BOOL res=cache.Lookup(pData->MKDCurrent, m_CurrentServer, dir, TRUE);
							if (res)
								SetDirectoryListing(&dir);
						}
							
						pData->nMKDOpState=MKD_CHANGETOSUBDIR;
						pData->MKDCurrent.AddSubdir(pData->MKDSegments.front());
						CString Segment=pData->MKDSegments.front();
						pData->MKDSegments.pop_front();
						if (!Send( _T("MKD ") + Segment))
							return;
					}
					else
					{
						if (!pData->MKDCurrent.HasParent())
							nReplyError=FZ_REPLY_ERROR|((res==5)?FZ_REPLY_CRITICALERROR:0);
						else
						{
							pData->MKDSegments.push_front(pData->MKDCurrent.GetLastSegment());
							pData->MKDCurrent=pData->MKDCurrent.GetParent();
							if (!Send("CWD "+pData->MKDCurrent.GetPath()))
								return;		
						}
					}
				}
				break;
			case MKD_MAKESUBDIRS:
				{
					int res=GetReplyCode();
					if (res==2 || res==3)
					{ //Create dir entry in parent dir
						ASSERT(!pData->MKDSegments.empty());
						pData->MKDCurrent.AddSubdir(pData->MKDSegments.front());
						CString Segment=pData->MKDSegments.front();
						pData->MKDSegments.pop_front();
						if (Send( _T("MKD ") + Segment))
							pData->nMKDOpState=MKD_CHANGETOSUBDIR;
						else
							return;
					}
					else
						nReplyError=FZ_REPLY_ERROR;
				}
				break;
			case MKD_CHANGETOSUBDIR:
				{
					int res=GetReplyCode();
					if (res==2 || res==3 || //Creation successful
						m_RecvBuffer.front()==_T("550 Directory already exists") )//Creation was successful, although someone else did the work for us
					{
						if (m_RecvBuffer.front()==_T("550 Directory already exists") )
							printf("OK");
						CServerPath path2=pData->MKDCurrent;
						if (path2.HasParent())
						{
							CString name=path2.GetLastSegment();
							path2=path2.GetParent();
							CDirectoryCache cache;
							cache.Lock();
							t_directory dir;
							BOOL bCached=TRUE;
							BOOL res=cache.Lookup(path2,m_CurrentServer,dir);
							if (!res)
								bCached=FALSE;
							if (!res && m_pDirectoryListing)
							{
								if (m_pDirectoryListing->path==path2)
								{
									dir=*m_pDirectoryListing;
									res=TRUE;
								}
							}
							t_directory WorkingDir;
							BOOL bFound=m_pOwner->GetWorkingDir(&WorkingDir);
							if (!res && bFound)
								if (WorkingDir.path==path2)
								{
									dir=WorkingDir;
									res=TRUE;
								}
							if (!res)
							{
								dir.path=path2;
								dir.server=m_CurrentServer;
							}
					
							int i;
							for (i=0; i<dir.num; i++)
								if (dir.direntry[i].name == name)
								{
									LogMessage(__FILE__, __LINE__, this, FZ_LOG_WARNING, _T("Dir already exists in cache!"));
									break;
								}
							if (i==dir.num)
							{
								t_directory::t_direntry *entries = new t_directory::t_direntry[dir.num+1];
								for (i=0;i<dir.num;i++)
									entries[i]=dir.direntry[i];
								entries[i].name=name;
								entries[i].lName=name;
								entries[i].lName.MakeLower();
								entries[i].dir=TRUE;
								entries[i].date.hasdate=FALSE;
								entries[i].size=-1;
								entries[i].bUnsure=FALSE;
								delete [] dir.direntry;
								dir.direntry=entries;
								dir.num++;
								cache.Store(dir, bCached);
								BOOL updated=FALSE;
								if (m_pDirectoryListing && m_pDirectoryListing->path==dir.path)
								{
									updated=TRUE;
									SetDirectoryListing(&dir);
								}
								if (!updated)
									if (WorkingDir.path==dir.path)
									{
										updated=TRUE;
										m_pOwner->SetWorkingDir(&dir);
									}
							}
							cache.Unlock();
						}

					}

					//Continue operation even if MKD failed, maybe another thread did create this directory for us
					if (pData->MKDSegments.empty())
						m_Operation.nOpState=FILETRANSFER_CWD2;
					else
					{
						if (Send( _T("CWD ") + pData->MKDCurrent.GetPath()))
							pData->nMKDOpState=MKD_MAKESUBDIRS;
						else
							return;
					}		
				}
				break;
			default: 
				ASSERT(FALSE);
			}
			
			break;
		case FILETRANSFER_CWD2:
			if (GetReplyCode()!=2 && GetReplyCode()!=3)
				if (GetReplyCode()==4)
					nReplyError=FZ_REPLY_ERROR;
				else
					nReplyError=FZ_REPLY_CRITICALERROR;
			else
				m_Operation.nOpState=FILETRANSFER_PWD;
			break;
		case FILETRANSFER_PWD:
			if (GetReplyCode()!=2 && GetReplyCode()!=2)
				nReplyError=FZ_REPLY_ERROR;
			else
			{
				int pos1=m_RecvBuffer.front().Find('"');
				int pos2=m_RecvBuffer.front().ReverseFind('"');
				if (pos1==-1 || (pos1+1)>=pos2)
					nReplyError=FZ_REPLY_ERROR;
				else
				{
					CServerPath path;
					path.SetServer(m_CurrentServer);
					if (!path.SetPath(m_RecvBuffer.front().Mid(pos1+1,pos2-pos1-1)))
						nReplyError=FZ_REPLY_ERROR;
					else
					{
						pData->rawpwd=m_RecvBuffer.front().Mid(pos1+1,pos2-pos1-1);
						m_pOwner->SetCurrentPath(path);
						if (path!=pData->transferfile.remotepath)
							nReplyError=FZ_REPLY_CRITICALERROR;
						else
						{
							CDirectoryCache cache;
							t_server server;
							m_pOwner->GetCurrentServer(server);
							t_directory dir;
							BOOL res=cache.Lookup(pData->transferfile.remotepath,server,dir);
							if (res)
							{
								CString remotefile=pData->transferfile.remotefile;
								int i;
								for (i=0; i<dir.num; i++)
								{
									if (dir.direntry[i].name==remotefile &&
										( dir.direntry[i].bUnsure || dir.direntry[i].size==-1 ))
									{
										m_Operation.nOpState = FILETRANSFER_LIST_PORTPASV;
										break;
									}
								}
								if (i==dir.num)
								{
									SetDirectoryListing(&dir);
									m_Operation.nOpState=FILETRANSFER_TYPE;
									nReplyError=CheckOverwriteFile();
									if (!nReplyError)
									{
										if (pData->transferfile.get)
										{
											CString path=pData->transferfile.localfile;
											if (path.ReverseFind('\\')!=-1)
											{
												path=path.Left(path.ReverseFind('\\')+1);
												CString path2;
												while (path!="")
												{
													path2+=path.Left(path.Find( _T("\\") )+1);
													path=path.Mid(path.Find( _T("\\") )+1);
													int res=CreateDirectory(path2,0);
												}
											}
										}
									}
								}
							}
							else
								m_Operation.nOpState = FILETRANSFER_LIST_PORTPASV;
						}
					}
				}
			}
			break;
		case FILETRANSFER_LIST_PORTPASV:
			if (GetReplyCode()!=3 && GetReplyCode()!=2)
			{
				if (!pData->bTriedPortPasvOnce)
				{
					pData->bTriedPortPasvOnce = TRUE;
					pData->bPasv = !pData->bPasv;
				}
				else
					nReplyError=FZ_REPLY_ERROR;
				break;
			}
			
			if (pData->bPasv)
			{
				int i=m_RecvBuffer.front().Find( _T("(") );
				int j=j=m_RecvBuffer.front().Find( _T(")") );
				// extract connect port number and IP from string returned by server
				if(i==-1 || j==-1 || (i+11)>=j)
				{
					if (!pData->bTriedPortPasvOnce)
					{
						pData->bTriedPortPasvOnce = TRUE;
						pData->bPasv = !pData->bPasv;
					}
					else
						nReplyError = FZ_REPLY_ERROR;
					break;
				}
					
				CString temp;
				temp=m_RecvBuffer.front().Mid(i+1,(j-i)-1);
				int count=0;
				int pos=0;
				//Convert commas to dots
				temp.Replace( _T(","), _T(".") );
				while(1)
				{
					pos=temp.Find(_T("."),pos);
					if (pos!=-1)
						count++;
					else
						break;
					pos++;
				}
				if (count!=5)
				{
					if (!pData->bTriedPortPasvOnce)
					{
						pData->bTriedPortPasvOnce = TRUE;
						pData->bPasv = !pData->bPasv;
					}
					else
						nReplyError = FZ_REPLY_ERROR;
					break;
				}
					
				i=temp.ReverseFind('.');
				pData->port=atol(  T2CA( temp.Right(temp.GetLength()-(i+1)) )  ); //get ls byte of server socket
				temp=temp.Left(i);
				i=temp.ReverseFind('.');
				pData->port+=256*atol(  T2CA( temp.Right(temp.GetLength()-(i+1)) )  ); // add ms byte to server socket
				pData->host=temp.Left(i);
				m_pTransferSocket = new CTransferSocket(this, CSMODE_LIST);
				if (m_pGssLayer && m_pGssLayer->AuthSuccessful())
					m_pTransferSocket->UseGSS(m_pGssLayer);
				m_pTransferSocket->m_nInternalMessageID = m_pOwner->m_nInternalMessageID;
				if (!m_pTransferSocket->Create(m_pSslLayer?TRUE:FALSE))
				{
					nReplyError = FZ_REPLY_ERROR;
					break;
				}
				
				VERIFY(m_pTransferSocket->AsyncSelect());
			}
			m_Operation.nOpState=FILETRANSFER_LIST_TYPE;
			break;
		case FILETRANSFER_LIST_TYPE:
			if (GetReplyCode()!=2 && GetReplyCode()!=3)
				nReplyError=FZ_REPLY_ERROR;
			else
				m_Operation.nOpState=FILETRANSFER_LIST_LIST;
			break;
		case FILETRANSFER_LIST_LIST:
			if (GetReplyCode()==4 || GetReplyCode()==5) //LIST failed, try getting file information using SIZE and MDTM
			{
				if (m_pTransferSocket)
					delete m_pTransferSocket;
				m_pTransferSocket=0;
				m_Operation.nOpState=FILETRANSFER_NOLIST_SIZE;
			}
			else if (GetReplyCode()!=1)
				nReplyError=FZ_REPLY_ERROR;
			else
				m_Operation.nOpState=FILETRANSFER_LIST_WAITFINISH;
			break;
		case FILETRANSFER_LIST_WAITFINISH:
			if (!bFinish)
			{
				if (GetReplyCode()!=2 && GetReplyCode()!=3)
					nReplyError=FZ_REPLY_ERROR;
				else
					pData->nGotTransferEndReply=1;
			}
			if (pData->nGotTransferEndReply && pData->pDirectoryListing)
			{
				CDirectoryCache cache;
				t_directory dir;
				cache.Lock();
				if (cache.Lookup(pData->pDirectoryListing->path, pData->pDirectoryListing->server, dir, TRUE))
				{
					pData->pDirectoryListing->Merge(dir, pData->ListStartTime);
				}
				cache.Store(*pData->pDirectoryListing);
				cache.Unlock();
				SetDirectoryListing(pData->pDirectoryListing);
				delete m_pTransferSocket;
				m_pTransferSocket=0;
				m_Operation.nOpState=FILETRANSFER_TYPE;
				nReplyError=CheckOverwriteFile();
				if (!nReplyError)
				{
					if (pData->transferfile.get)
					{
						CString path=pData->transferfile.localfile;
						if (path.ReverseFind('\\')!=-1)
						{
							path=path.Left(path.ReverseFind('\\')+1);
							CString path2;
							while (path!="")
							{
								path2+=path.Left(path.Find( _T("\\") )+1);
								path=path.Mid(path.Find( _T("\\") )+1);
								int res=CreateDirectory(path2,0);
							}
						}
					}
				}
				pData->nGotTransferEndReply=0;
			}
			break;
		case FILETRANSFER_NOLIST_SIZE:
			if (GetReplyCode()==2)
			{
				CString line=m_RecvBuffer.front();
				if ( line.GetLength()>4  &&  line.Left(4) == _T("213 ") )
				{
					__int64 size=_ttoi64(line.Mid(4));
					ASSERT(!pData->pFileSize);
					pData->pFileSize=new _int64;
					*pData->pFileSize=size;
				}
			}
			m_Operation.nOpState=FILETRANSFER_NOLIST_MDTM;
			break;
		case FILETRANSFER_NOLIST_MDTM:
			if (GetReplyCode()==2)
			{
				CString line=m_RecvBuffer.front();
				if ( line.GetLength()>4  &&  line.Left(4) == _T("213 ") )
				{
					int y=0, M=0, d=0, h=0, m=0;
					line=line.Mid(4);
					y=_ttoi(line.Left(4));
					if (y && line.GetLength()>4)
					{
						line=line.Mid(4);
						M=_ttoi(line.Left(2));
						if (M && line.GetLength()>2)
						{
							line=line.Mid(2);
							d=_ttoi(line.Left(2));
							if (d && line.GetLength()>2)
							{
								line=line.Mid(2);
								h=_ttoi(line.Left(2));
								if (h && line.GetLength()>2)
								{
									line=line.Mid(2);
									m=_ttoi(line.Left(2));
									if (m && line.GetLength()>2)
									{
										line=line.Mid(2);
									}
								}
							}
							if (M>0 && M<=12 && d>0 && d<=31 && h>=0 && h<24 && m>=0 && m<60)
							{
								ASSERT(!pData->pFileTime);
								pData->pFileTime=new CTime(y, M, d, h, m, 0);
							}
						}
					}
				}
			}
			m_Operation.nOpState=FILETRANSFER_TYPE;
			nReplyError=CheckOverwriteFile();
			break;
		case FILETRANSFER_TYPE:
			if (GetReplyCode()!=2 && GetReplyCode()!=3)
				nReplyError = FZ_REPLY_ERROR;
			m_Operation.nOpState = FILETRANSFER_PORTPASV;
			break;
		case FILETRANSFER_WAIT:
			if (!pData->nWaitNextOpState)
				nReplyError=FZ_REPLY_ERROR;
			else
				m_Operation.nOpState=pData->nWaitNextOpState;
			break;
		case FILETRANSFER_PORTPASV:
			if (GetReplyCode()==3 || GetReplyCode()==2)
			{
				if (pData->bPasv)
				{
					int i=m_RecvBuffer.front().Find( _T("(") );
					int j=j=m_RecvBuffer.front().Find( _T(")") );
					// extract connect port number and IP from string returned by server
					if(i==-1 || j==-1 || (i+11)>=j)
					{
						if (!pData->bTriedPortPasvOnce)
						{
							pData->bTriedPortPasvOnce = TRUE;
							pData->bPasv = !pData->bPasv;
						}
						else
							nReplyError = FZ_REPLY_ERROR;
						break;
					}

					CString temp;
					temp=m_RecvBuffer.front().Mid(i+1,(j-i)-1);
					int count=0;
					int pos=0;
					//Convert commas to dots
					temp.Replace( _T(","), _T(".") );
					while(1)
					{
						pos=temp.Find( _T("."), pos);
						if (pos!=-1)
							count++;
						else
							break;
						pos++;
					}
					if (count!=5)
					{
						if (!pData->bTriedPortPasvOnce)
						{
							pData->bTriedPortPasvOnce = TRUE;
							pData->bPasv = !pData->bPasv;
						}
						else
							nReplyError = FZ_REPLY_ERROR;
						break;
					}

					i=temp.ReverseFind('.');
					pData->port=atol(  T2CA( temp.Right(temp.GetLength()-(i+1)) )  ); //get ls byte of server socket
					temp=temp.Left(i);
					i=temp.ReverseFind('.');
					pData->port+=256*atol(  T2CA( temp.Right(temp.GetLength()-(i+1)) )  ); // add ms byte to server socket
					pData->host=temp.Left(i);
					m_pTransferSocket=new CTransferSocket(this, m_Operation.nOpMode);
					if (m_pGssLayer && m_pGssLayer->AuthSuccessful())
						m_pTransferSocket->UseGSS(m_pGssLayer);
					m_pTransferSocket->m_nInternalMessageID = m_pOwner->m_nInternalMessageID;
					if (!m_pTransferSocket->Create(m_pSslLayer?TRUE:FALSE))
					{
						nReplyError = FZ_REPLY_ERROR;
						break;
					}

					VERIFY(m_pTransferSocket->AsyncSelect());					
				}

				if (pData->transferdata.bResume && pData->transferfile.get)
					m_Operation.nOpState = FILETRANSFER_REST;
				else
					m_Operation.nOpState = FILETRANSFER_RETRSTOR;
				BOOL res = FALSE;
				if (!m_pDataFile)
					m_pDataFile = new CFile;
				if (pData->transferfile.get)
				{
					if (pData->transferdata.bResume)
						res = m_pDataFile->Open(pData->transferfile.localfile,CFile::modeCreate|CFile::modeWrite|CFile::modeNoTruncate|CFile::shareDenyWrite);
					else
						res = m_pDataFile->Open(pData->transferfile.localfile,CFile::modeWrite|CFile::modeCreate|CFile::shareDenyWrite);
				}
				else
					res = m_pDataFile->Open(pData->transferfile.localfile,CFile::modeRead|CFile::shareDenyWrite);
				if (!res)
				{
					//Error opening the file
					CString str;
					str.Format(IDS_ERRORMSG_FILEOPENFAILED,pData->transferfile.localfile);
					ShowStatus(str,1);
					nReplyError = FZ_REPLY_ERROR;
					break;
				}

				if (!m_pTransferSocket)
				{
					nReplyError=FZ_REPLY_ERROR;
					break;
				}
				
				m_pTransferSocket->m_pFile=m_pDataFile;
				if (!pData->transferfile.get)
				{
					pData->transferdata.transfersize=GetLength64(*m_pDataFile);
					pData->transferdata.transferleft=pData->transferdata.transfersize;
					if (pData->transferdata.bResume)
					{
						CString remotefile=pData->transferfile.remotefile;
						if (m_pDirectoryListing)
							for (int i=0;i<m_pDirectoryListing->num;i++)
							{
								if (m_pDirectoryListing->direntry[i].name==remotefile)
								{
									pData->transferdata.transferleft-=m_pDirectoryListing->direntry[i].size;
									break;
								}
							}
						_int64 size=pData->transferdata.transfersize-pData->transferdata.transferleft;
						LONG low=static_cast<LONG>(size&0xFFFFFFFF);
						LONG high=static_cast<LONG>(size>>32);
						if (SetFilePointer((HANDLE)m_pDataFile->m_hFile, low, &high, FILE_BEGIN)==0xFFFFFFFF && GetLastError()!=NO_ERROR)
						{
							ShowStatus(IDS_ERRORMSG_SETFILEPOINTER, 1);
							nReplyError = FZ_REPLY_ERROR;
						}
					}
				}
				else
				{
					pData->transferdata.transfersize=-1;
					CString remotefile=pData->transferfile.remotefile;
					if (m_pDirectoryListing)
						for (int i=0; i<m_pDirectoryListing->num; i++)
						{
							if (m_pDirectoryListing->direntry[i].name==remotefile)
							{
								ASSERT( !pData->pFileTime);
								t_directory::t_direntry::t_date *pDate=&m_pDirectoryListing->direntry[i].date;
								if (pDate->hasdate)
								{
									if (pDate->hastime)
									{
										TIME_ZONE_INFORMATION tzInfo={0};
										BOOL res=GetTimeZoneInformation(&tzInfo);
										CTimeSpan span(0, 0, tzInfo.Bias+((res==TIME_ZONE_ID_DAYLIGHT)?tzInfo.DaylightBias:tzInfo.StandardBias), 0);
										pData->pFileTime=new CTime(pDate->year, pDate->month, pDate->day, pDate->hour, pDate->minute, 0);
										*pData->pFileTime+=span;
									}
									else
										pData->pFileTime=new CTime(pDate->year, pDate->month, pDate->day, 0, 0, 0);
								}
									
								pData->transferdata.transfersize=m_pDirectoryListing->direntry[i].size;
							}
						}
					else if (pData->pFileSize)
						pData->transferdata.transfersize=*pData->pFileSize;
					pData->transferdata.transferleft=pData->transferdata.transfersize;
				}
			}
			else
				if (!pData->bTriedPortPasvOnce)
				{
					pData->bTriedPortPasvOnce = TRUE;
					pData->bPasv = !pData->bPasv;
				}
				else
					nReplyError = FZ_REPLY_ERROR;
			break;
		case FILETRANSFER_REST:
			{ //Resume
				int code=GetReplyCode();
				if (code==3 || code==2)
				{
					LONG high = 0;
					pData->transferdata.transferleft = pData->transferdata.transfersize - GetLength64(*m_pDataFile);
					if (SetFilePointer((HANDLE)m_pDataFile->m_hFile, 0, &high, FILE_END)==0xFFFFFFFF && GetLastError()!=NO_ERROR)
					{
						ShowStatus(IDS_ERRORMSG_SETFILEPOINTER, 1);
						nReplyError = FZ_REPLY_ERROR;
					}
					else
						m_Operation.nOpState=FILETRANSFER_RETRSTOR;
				}
				else
				{
					if (code==5 && m_RecvBuffer.front()[1]=='0')
					{
						if (pData->transferdata.transfersize!=-1 && pData->transferfile.get)
						{
							ASSERT(m_pDataFile);
							if (GetLength64(*m_pDataFile) == pData->transferdata.transfersize)
							{
								ShowStatus(IDS_ERRORMSG_CANTRESUME_FINISH, 0);
								ResetOperation(FZ_REPLY_OK);
								return;
							}
						}
						
						ShowStatus(IDS_ERRORMSG_CANTRESUME, 1);
						pData->transferdata.transferleft=pData->transferdata.transfersize;
						pData->transferdata.bResume=FALSE;
						m_Operation.nOpState=FILETRANSFER_RETRSTOR;
					}
					else
						nReplyError=FZ_REPLY_ERROR;
				}
			}
			break;
		case FILETRANSFER_RETRSTOR:
			// A '1xy opening data connection' reply is expected if RETR/STOR/APPE
			// is successful.
			// On failure, it's a 4xy or 5xy reply.
			// However, some servers send a 2xy transfer complete reply without opening a data 
			// connection if there's no data to send.
			if (GetReplyCode()==2)
			{
				//Transfer successful, however server did not open data connection
				ResetOperation(FZ_REPLY_OK);
				return;
			}
			else if (GetReplyCode()!=1)
			{
				nReplyError=FZ_REPLY_ERROR;
				if (GetReplyCode()==5)
					nReplyError|=FZ_REPLY_CRITICALERROR;
			}
			else
			{
				m_Operation.nOpState=FILETRANSFER_WAITFINISH;
				
				//Look if we can find any information about the resume offset
				if (!pData->transferfile.get && pData->transferdata.bResume)
				{
					_int64 nOffset = -1;
					CString reply = m_RecvBuffer.front();
					reply.MakeLower();
					int pos = reply.Find(_T("restarting at offset "));
					if (pos != -1)
						pos += _tcslen(_T("restarting at offset "));

					reply = reply.Mid(pos);

					int i;
					for (i=0; i<reply.GetLength(); i++)
					{
						if (reply[i] < '0' || reply[i] > '9')
							break;
					}
					if (i == reply.GetLength())
						nOffset = _ttoi64(reply);
					if (nOffset != -1 && m_pDataFile)
					{
						LONG low = 0;
						LONG high = 0;
						if (nOffset >= GetLength64(*m_pDataFile))
							if (SetFilePointer((HANDLE)m_pDataFile->m_hFile, 0, &high, FILE_END)==0xFFFFFFFF && GetLastError()!=NO_ERROR)
							{
								ShowStatus(IDS_ERRORMSG_SETFILEPOINTER, 1);
								nReplyError = FZ_REPLY_ERROR;
							}
						else
						{
							low=static_cast<LONG>(nOffset&0xFFFFFFFF);
							high=static_cast<LONG>(nOffset>>32);
							if (SetFilePointer((HANDLE)m_pDataFile->m_hFile, low, &high, FILE_BEGIN)==0xFFFFFFFF && GetLastError()!=NO_ERROR)
							{
								ShowStatus(IDS_ERRORMSG_SETFILEPOINTER, 1);
								nReplyError = FZ_REPLY_ERROR;
							}
						}
					}
					if (!nReplyError)
						m_pTransferSocket->SetActive();
				}
				else if (pData->bPasv)
					m_pTransferSocket->SetActive();
				
			}
			break;
		case FILETRANSFER_WAITFINISH:
			if (!bFinish)
			{
				if (GetReplyCode() == 1)
				{
					/* Some non-rfc959 compatible servers send more than one code 1yz reply, especially if using APPE.
					 * Just ignore the additional ones.
					 */
					LogMessage(FZ_LOG_WARNING, _T("Server sent more than one code 1yz reply, ignoring additional reply"));
					break;
				}
				else if (GetReplyCode()!=2 && GetReplyCode()!=3)
					nReplyError = FZ_REPLY_ERROR;
				else
				{
					pData->nGotTransferEndReply |= 1;
				}
			}
			if (pData->nGotTransferEndReply==3)
			{
				if (COptions::GetOptionVal(OPTION_PRESERVEDOWNLOADFILETIME) && m_pDataFile)
				{
					if (pData->pFileTime)
					{
						SYSTEMTIME stime;
						FILETIME ftime;
						if (pData->pFileTime->GetAsSystemTime(stime))
							if (SystemTimeToFileTime(&stime, &ftime))
								SetFileTime((HANDLE)m_pDataFile->m_hFile, &ftime, &ftime, &ftime);
					}
				}
				//Transfer successful
				ResetOperation(FZ_REPLY_OK);
				return;
			}
			break;
		}
		if (nReplyError)
		{ //Error transferring the file	
			ResetOperation(nReplyError);
			return;
		}
	}
	/////////////////
	//Send commands//
	/////////////////
	BOOL bError=FALSE;
	switch(m_Operation.nOpState)
	{
	case FILETRANSFER_CWD:
		if (!Send("CWD "+pData->transferfile.remotepath.GetPath()))
			bError=TRUE;
		break;
	case FILETRANSFER_MKD:
		if (pData->nMKDOpState==MKD_INIT)
		{
			if (!pData->transferfile.remotepath.HasParent())
			{
				LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Can't create root dir"));
				ResetOperation(FZ_REPLY_CRITICALERROR);
				return;
			}
			if (!Send("CWD "+pData->transferfile.remotepath.GetParent().GetPath()))
				bError=TRUE;
			pData->MKDCurrent=pData->transferfile.remotepath.GetParent();
			pData->MKDSegments.push_front(pData->transferfile.remotepath.GetLastSegment());
			pData->nMKDOpState=MKD_FINDPARENT;
		}
		break;
	case FILETRANSFER_CWD2:
		if (!Send("CWD "+pData->transferfile.remotepath.GetPath()))
			bError=TRUE;
		break;
	case FILETRANSFER_PWD:
		if (!Send("PWD"))
			bError=TRUE;
		break;
	case FILETRANSFER_LIST_PORTPASV:
		delete m_pDirectoryListing;
		m_pDirectoryListing=0;
		if (pData->bPasv)
		{
			if (!Send("PASV"))
				bError=TRUE;
		}
		else
		{
			ASSERT(!m_pTransferSocket);
			m_pTransferSocket=new CTransferSocket(this, CSMODE_LIST);
			if (m_pGssLayer && m_pGssLayer->AuthSuccessful())
				m_pTransferSocket->UseGSS(m_pGssLayer);
			m_pTransferSocket->m_nInternalMessageID = m_pOwner->m_nInternalMessageID;
			m_pTransferSocket->m_bListening = TRUE;
			if(!m_pTransferSocket->Create(m_pSslLayer?TRUE:FALSE)|| !m_pTransferSocket->AsyncSelect())
				bError=TRUE;
			else if (m_pProxyLayer)
			{
				SOCKADDR_IN addr;
				int len=sizeof(addr);
				if (!m_pProxyLayer->GetPeerName((SOCKADDR *)&addr,&len))
				{
					ShowStatus(IDS_ERRORMSG_CANTGETLIST,1);
					bError=TRUE;
				}
				else if (!m_pTransferSocket->Listen(addr.sin_addr.S_un.S_addr))
				{
					ShowStatus(IDS_ERRORMSG_CANTGETLIST,1);
					bError=TRUE;
				}
				//Don't send PORT command yet, params are unknown. 
				//will be sent in TransfersocketListenFinished
			}
			else
			{
				//Set up an active file transfer
				CString temp;
				UINT nPort;
				
				if (// create listen socket (let MFC choose the port) & start the socket listening
					!m_pTransferSocket->Listen() || 
					!m_pTransferSocket->GetSockName(temp, nPort))
				{
					bError = TRUE;
				}
				else
				{
					CString host = COptions::GetOption(OPTION_TRANSFERIP);
					if (host != "")
					{
						DWORD ip = inet_addr(T2CA(host));
						if (ip != INADDR_NONE)
							host.Format(_T("%d,%d,%d,%d"), ip%256, (ip>>8)%256, (ip>>16)%256, ip>>24);
						else
						{
							hostent *fullname = gethostbyname(T2CA(host));
							if (!fullname)
								host = "";
							else
							{
								DWORD ip = ((LPIN_ADDR)fullname->h_addr)->s_addr;
								if (ip != INADDR_NONE)
									host.Format(_T("%d,%d,%d,%d"), ip%256, (ip>>8)%256, (ip>>16)%256, ip>>24);
								else
									host = "";
							}
						}
					}
					if (host == "")
					{
						UINT temp;
	
						if(!GetSockName(host, temp))
							bError = TRUE;
						else
							host.Replace('.', ',');
					}
					if (!bError)
					{
						host.Format(host+",%d,%d", nPort/256, nPort%256);
						if (!Send(_T("PORT ") + host)) // send PORT cmd to server
							bError = TRUE;
					}
				}
			}
		}
		break;
	case FILETRANSFER_LIST_TYPE:
		if (!Send("TYPE A"))
			bError=TRUE;
		break;
	case FILETRANSFER_LIST_LIST:
		{
			if (!m_pTransferSocket)
			{
				LogMessage(__FILE__, __LINE__, this,FZ_LOG_APIERROR, _T("Error: m_pTransferSocket==NULL") );
				ResetOperation(FZ_REPLY_ERROR);
				return;
			}

			m_pTransferSocket->SetActive();
			pData->ListStartTime=CTime::GetCurrentTime();
			CString cmd="LIST";
			if ( m_pOwner->GetOption(FZAPI_OPTION_SHOWHIDDEN) || pData->transferfile.remotefile.Left(1)=="." )
				cmd+=" -a";
			if(!Send(cmd))
				bError=TRUE;
			else if(pData->bPasv) 
			{
				// if PASV create the socket & initiate outbound data channel connection
				if (!m_pTransferSocket->Connect(pData->host,pData->port))
				{
					if (GetLastError()!=WSAEWOULDBLOCK)
					{
						bError=TRUE;
						ShowStatus(IDS_ERRORMSG_CANTGETLIST,1);
					}
				}
			}
		}
		break;
	case FILETRANSFER_NOLIST_SIZE:
		{
			CString command = _T("SIZE ");
			if (!pData->bUseAbsolutePaths)
				command += pData->transferfile.remotefile;
			else
				command += pData->transferfile.remotepath.GetPath() + pData->transferfile.remotefile;
		
			if (!Send(command))
				bError=TRUE;
		}
		break;
	case FILETRANSFER_NOLIST_MDTM:
		{
			CString command = _T("MDTM ");
			if (!pData->bUseAbsolutePaths)
				command += pData->transferfile.remotefile;
			else
				command += pData->transferfile.remotepath.GetPath() + pData->transferfile.remotefile;
		
			if (!Send(command))
				bError=TRUE;
		}
		break;
	case FILETRANSFER_TYPE:
		if (pData->transferfile.nType==1)
		{
			if (!Send("TYPE A"))
				bError=TRUE;
		}
		else
			if (!Send("TYPE I"))
				bError=TRUE;
		break;
	case FILETRANSFER_PORTPASV:
		if (pData->bPasv)
		{
			if (!Send("PASV"))
				bError=TRUE;
		}
		else
		{
			ASSERT(!m_pTransferSocket);
			m_pTransferSocket=new CTransferSocket(this, m_Operation.nOpMode);
			if (m_pGssLayer && m_pGssLayer->AuthSuccessful())
				m_pTransferSocket->UseGSS(m_pGssLayer);
			m_pTransferSocket->m_nInternalMessageID=m_pOwner->m_nInternalMessageID;
			m_pTransferSocket->m_bListening = TRUE;
			if(!m_pTransferSocket->Create(m_pSslLayer?TRUE:FALSE)|| !m_pTransferSocket->AsyncSelect())
				bError = TRUE;
			else if (m_pProxyLayer)
			{
				SOCKADDR_IN addr;
				int len=sizeof(addr);
				if (!m_pProxyLayer->GetPeerName((SOCKADDR *)&addr,&len))
				{
					ShowStatus(IDS_ERRORMSG_CANTGETLIST,1);
					bError=TRUE;
				}
				else if (!m_pTransferSocket->Listen(addr.sin_addr.S_un.S_addr))
				{
					ShowStatus(IDS_ERRORMSG_CANTGETLIST,1);
					bError=TRUE;
				}
				//Don't send PORT command yet, params are unknown. 
				//will be sent in TransfersocketListenFinished
			}
			else
			{
				//Set up an active file transfer
				CString temp;
				UINT nPort;
				
				if (// create listen socket (let MFC choose the port) & start the socket listening
					!m_pTransferSocket->Listen() || 
					!m_pTransferSocket->GetSockName(temp, nPort))
				{
					bError = TRUE;
				}
				else
				{
					CString host = COptions::GetOption(OPTION_TRANSFERIP);
					if (host != "")
					{
						DWORD ip = inet_addr(T2CA(host));
						if (ip != INADDR_NONE)
							host.Format(_T("%d,%d,%d,%d"), ip%256, (ip>>8)%256, (ip>>16)%256, ip>>24);
						else
						{
							hostent *fullname = gethostbyname(T2CA(host));
							if (!fullname)
								host = "";
							else
							{
								DWORD ip = ((LPIN_ADDR)fullname->h_addr)->s_addr;
								if (ip != INADDR_NONE)
									host.Format(_T("%d,%d,%d,%d"), ip%256, (ip>>8)%256, (ip>>16)%256, ip>>24);
								else
									host = "";
							}
						}
					}
					if (host == "")
					{
						UINT temp;
	
						if(!GetSockName(host, temp))
							bError = TRUE;
						else
							host.Replace('.', ',');
					}
					if (!bError)
					{
						host.Format(host+",%d,%d", nPort/256, nPort%256);
						if (!Send(_T("PORT ") + host)) // send PORT cmd to server
							bError = TRUE;
					}
				}
			}
		}
		break;
	case FILETRANSFER_REST:
		ASSERT(m_pDataFile);
		{
			CString command;
			command.Format(_T("REST %I64d"), GetLength64(*m_pDataFile));
			if (!Send(command))
				bError=TRUE;
		}
		break;
	case FILETRANSFER_RETRSTOR:
		pData->transferdata.nTransferStart=pData->transferdata.transfersize-pData->transferdata.transferleft;
		m_pTransferSocket->m_transferdata=pData->transferdata;
		// send RETR/STOR command to server
		if (!m_pTransferSocket)
		{
			LogMessage(__FILE__, __LINE__, this,FZ_LOG_APIERROR, _T("Error: m_pTransferSocket==NULL") );
			ResetOperation(FZ_REPLY_ERROR);
			return;
		}
		if ((pData->transferfile.get || !pData->transferdata.bResume) && !pData->bPasv)
			m_pTransferSocket->SetActive();
		CString filename;
		if (!pData->bUseAbsolutePaths)
			filename = pData->transferfile.remotefile;
		else
			filename = pData->transferfile.remotepath.GetPath() + pData->transferfile.remotefile;
		if(!Send((pData->transferfile.get?"RETR ":(pData->transferdata.bResume)?"APPE ":"STOR ")+ filename))
			bError=TRUE;
		else
		{
			if(!pData->transferfile.get)
				pData->bSentStor=TRUE;
			if(pData->bPasv) 
			{// if PASV create the socket & initiate outbound data channel connection
				if (!m_pTransferSocket->Connect(pData->host,pData->port))
				{
					if (GetLastError()!=WSAEWOULDBLOCK)
						bError=TRUE;
				}
			}
		}
		break;
	}
	if (bError)
	{ //Error transferring the file	
		ResetOperation(FZ_REPLY_ERROR);
		return;
	}
}		

void CFtpControlSocket::TransfersocketListenFinished(unsigned int ip, unsigned short port)
{
	if (m_Operation.nOpMode&CSMODE_TRANSFER || m_Operation.nOpMode&CSMODE_LIST)
	{
		CString host;
		host.Format(_T("%d,%d,%d,%d,%d,%d"),ip%256,(ip>>8)%256,(ip>>16)%256,(ip>>24)%256,port%256,port>>8);
		Send("PORT "+host);
	}
}

void CFtpControlSocket::ResumeTransfer()
{
	if (m_pTransferSocket && (m_Operation.nOpMode&CSMODE_TRANSFER || m_Operation.nOpMode&CSMODE_LIST))
	{
		m_pTransferSocket->OnSend(0);
		m_pTransferSocket->OnReceive(0);
	}
}

BOOL CFtpControlSocket::Create()
{
	if (!COptions::GetOptionVal(OPTION_LIMITPORTRANGE))
		return CAsyncSocketEx::Create();
	else
	{
		int min=COptions::GetOptionVal(OPTION_PORTRANGELOW);
		int max=COptions::GetOptionVal(OPTION_PORTRANGEHIGH);
		if (min>=max)
		{
			ShowStatus(IDS_ERRORMSG_CANTCREATEDUETOPORTRANGE,1);
			return FALSE;
		}
		int startport = static_cast<int>(min+((double)rand()*(max-min))/(RAND_MAX+1));
		int port = startport;
		while (!CAsyncSocketEx::Create(port))
		{
			port++;
			if (port>max)
				port=min;
			if (port==startport)
			{
				ShowStatus(IDS_ERRORMSG_CANTCREATEDUETOPORTRANGE,1);
				return FALSE;
			}
		}
	}
	return TRUE;
}


void CFtpControlSocket::ResetOperation(int nSuccessful /*=FALSE*/)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("ResetOperation(%d)  OpMode=%d OpState=%d"), nSuccessful, m_Operation.nOpMode, m_Operation.nOpState);

	if (nSuccessful & FZ_REPLY_CRITICALERROR)
		nSuccessful |= FZ_REPLY_ERROR;

	if (m_pTransferSocket)
		delete m_pTransferSocket;
	m_pTransferSocket=0;

	//There may be an active ident socket, close it
	if (m_pIdentControl)
	{
		delete m_pIdentControl;
		m_pIdentControl=0;
	}

	if (m_pDataFile)
		delete m_pDataFile;
	m_pDataFile=0;
	
	if (m_Operation.nOpMode)
	{	
		//Unset busy attribute so that new commands can be executed
		m_pOwner->SetBusy(FALSE);
		
		if (m_Operation.nOpMode&CSMODE_CONNECT && nSuccessful&FZ_REPLY_ERROR)
		{
			nSuccessful|=FZ_REPLY_DISCONNECTED;
			ShowStatus(IDS_ERRORMSG_CANTCONNECT, 1);
		}
		
		if (m_Operation.nOpMode & (CSMODE_LIST|CSMODE_TRANSFER) && nSuccessful==FZ_REPLY_OK)
			m_LastSendTime=CTime::GetCurrentTime();
	
		//Update remote file entry
		if (m_Operation.pData && 
			m_Operation.nOpMode&CSMODE_TRANSFER && 
			(!((CFileTransferData*)m_Operation.pData)->transferfile.get) && 
			m_pDirectoryListing &&
			m_Operation.nOpState>=FILETRANSFER_RETRSTOR)
		{
			CString filename=((CFileTransferData*)m_Operation.pData)->transferfile.remotefile;
			CServerPath path=((CFileTransferData*)m_Operation.pData)->transferfile.remotepath;
			CDirectoryCache cache;
			cache.Lock();
			t_directory dir;
			BOOL bCached=TRUE;
			BOOL res=cache.Lookup(path, m_CurrentServer, dir);
			if (!res)
				bCached=FALSE;
			if (!res && m_pDirectoryListing)
			{
				if (m_pDirectoryListing->path==path)
				{
					dir=*m_pDirectoryListing;
					res=TRUE;
				}
			}
			t_directory WorkingDir;
			BOOL bFound=m_pOwner->GetWorkingDir(&WorkingDir);
			if (!res && bFound)
				if (WorkingDir.path==path)
				{
					dir=WorkingDir;
					res=TRUE;
				}
			if (res)
			{
				int i;
				for (i=0; i<dir.num; i++)
					if (dir.direntry[i].name==filename)
					{
						if (nSuccessful&FZ_REPLY_ERROR)
						{
							dir.direntry[i].bUnsure=TRUE;
							if (!((CFileTransferData *)m_Operation.pData)->transferfile.get)
								dir.direntry[i].size = -1;
						}
						break;
					}
				if (i==dir.num)
				{
					t_directory::t_direntry *entries=new t_directory::t_direntry[dir.num+1];
					int i;
					for (i=0; i<dir.num; i++)
						entries[i]=dir.direntry[i];
					entries[i].name=filename;
					entries[i].lName=filename;
					entries[i].lName.MakeLower();
					entries[i].dir=FALSE;
					entries[i].date.hasdate=FALSE;
					entries[i].size=-1;
					if (nSuccessful&FZ_REPLY_OK)
						entries[i].bUnsure=FALSE;
					else
						entries[i].bUnsure=TRUE;
					
					delete [] dir.direntry;
					dir.direntry=entries;
					dir.num++;
				}
				cache.Store(dir, bCached);
				BOOL updated=FALSE;
				if (m_pDirectoryListing && m_pDirectoryListing->path==dir.path)
				{
					updated=TRUE;
					SetDirectoryListing(&dir, bFound && WorkingDir.path == dir.path);
				}
				if (!updated)
					if (bFound && WorkingDir.path==dir.path)
					{
						updated = TRUE;
						m_pOwner->SetWorkingDir(&dir);
					}
			}
			cache.Unlock();
		}
		
		if (m_Operation.pData && nSuccessful&FZ_REPLY_ERROR)
		{
			if (m_Operation.nOpMode&CSMODE_TRANSFER)
				if (nSuccessful&FZ_REPLY_ABORTED)
					//Transfer aborted by user
					ShowStatus((m_Operation.nOpMode&CSMODE_DOWNLOAD)?IDS_ERRORMSG_DOWNLOADABORTED:IDS_ERRORMSG_UPLOADABORTED,1);
				else
					ShowStatus(((CFileTransferData*)m_Operation.pData)->transferfile.get?IDS_ERRORMSG_DOWNLOADFAILED:IDS_ERRORMSG_UPLOADFAILED,1);
			else if (m_Operation.nOpMode&CSMODE_LIST)
				ShowStatus(IDS_ERRORMSG_CANTGETLIST,1);
		}
		else if (m_Operation.pData && m_Operation.nOpMode&CSMODE_TRANSFER && nSuccessful==FZ_REPLY_OK)
			ShowStatus(((CFileTransferData*)m_Operation.pData)->transferfile.get?IDS_STATUSMSG_DOWNLOADSUCCESSFUL:IDS_STATUSMSG_UPLOADSUCCESSFUL,0);
	}
	else
	{
		//No operation in progress
		nSuccessful&=FZ_REPLY_DISCONNECTED|FZ_REPLY_CANCEL;
		if (!nSuccessful)
			ASSERT(FALSE);
	}
	
	if (nSuccessful&FZ_REPLY_DISCONNECTED)
		m_pOwner->SetWorkingDir(0); //Disconnected, reset working dir
	
	if (m_Operation.nOpMode)
		PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_REPLY, m_pOwner->m_LastCommand.id), nSuccessful);
	else
		PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_REPLY, 0), nSuccessful);

	m_Operation.nOpMode=0;
	m_Operation.nOpState=-1;

	if (m_Operation.pData)
		delete m_Operation.pData;
	m_Operation.pData=0;
}

void CFtpControlSocket::Delete(CString filename, const CServerPath &path)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("Delete(\"%s\", \"%s\")  OpMode=%d OpState=%d"), filename, path.GetPath(), m_Operation.nOpMode, m_Operation.nOpState);
	
	class CDeleteData : public CFtpControlSocket::t_operation::COpData
	{
public:
		CDeleteData() {}
		virtual ~CDeleteData() {}
		CString m_FileName;
		CServerPath path;
	};
	if (filename!="")
	{
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_NONE);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(!m_Operation.pData);
		m_Operation.nOpMode=CSMODE_DELETE;
		if (!Send("DELE "+path.GetPath()+filename))
			return;
		CDeleteData *data=new CDeleteData;
		data->m_FileName=filename;
		data->path=path;
		m_Operation.pData=data;
	}
	else
	{
		ASSERT(path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_DELETE);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(m_Operation.pData);
		int res=GetReplyCode();
		if (res==2 || res==3)
		{ //Remove file from cached dirs
			CDeleteData *pData=(CDeleteData *)m_Operation.pData;
			CDirectoryCache cache;
			cache.Lock();
			BOOL bCached=TRUE;
			t_directory dir;
			BOOL res=cache.Lookup(pData->path,m_CurrentServer,dir);
			if (!res)
				bCached=FALSE;
			if (!res && m_pDirectoryListing)
			{
				if (m_pDirectoryListing->path==pData->path)
				{
					dir=*m_pDirectoryListing;
					res=TRUE;
				}
			}
			t_directory WorkingDir;
			BOOL bFound=m_pOwner->GetWorkingDir(&WorkingDir);
			if (!res && bFound)
				if (WorkingDir.path==pData->path)
				{
					dir=WorkingDir;
					res=TRUE;
				}
			if (res)
			{
				BOOL found=FALSE;
				for (int i=0;i<dir.num;i++)
				{
					if (dir.direntry[i].name==pData->m_FileName)
					{
						ASSERT(!dir.direntry[i].dir || dir.direntry[i].bLink);
						found=TRUE;
						break;
					}
				}
				if (found)
				{
					t_directory::t_direntry *direntry=new t_directory::t_direntry[dir.num-1];
					int j=0;
					int i;
					for (i=0; i<dir.num; i++)
					{
						if (dir.direntry[i].name==pData->m_FileName)
							continue;
						direntry[j]=dir.direntry[i];
						j++;
					}
					delete [] dir.direntry;
					dir.direntry=direntry;
					dir.num--;
					cache.Store(dir, bCached);
					BOOL updated=FALSE;
					if (m_pDirectoryListing)
						if (m_pDirectoryListing->path==dir.path)
						{
							updated=TRUE;
							SetDirectoryListing(&dir);
						}
					if (!updated)
						if (WorkingDir.path==dir.path)
						{
							updated=TRUE;
							m_pOwner->SetWorkingDir(&dir);
						}
				}
			}
			cache.Unlock();
		}
		ResetOperation(FZ_REPLY_OK);
	}
}

void CFtpControlSocket::RemoveDir(CString dirname, const CServerPath &path)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("RemoveDir(\"%s\", \"%s\")  OpMode=%d OpState=%d"), dirname, path.GetPath(), m_Operation.nOpMode, m_Operation.nOpState);
	
	class CRemoveDirData : public CFtpControlSocket::t_operation::COpData
	{
public:
		CRemoveDirData() {}
		virtual ~CRemoveDirData() {}
		CString m_DirName;
		CServerPath path;
	};
	if (dirname!="")
	{
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_NONE);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(!m_Operation.pData);
		m_Operation.nOpMode=CSMODE_RMDIR;
		if (!Send("RMD "+path.GetPath()+dirname))
			return;
		CRemoveDirData *data=new CRemoveDirData;
		data->m_DirName=dirname;
		data->path=path;
		m_Operation.pData=data;
	}
	else
	{
		ASSERT(path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_RMDIR);
		ASSERT(m_Operation.nOpState==-1);
		ASSERT(m_Operation.pData);
		int res=GetReplyCode();
		if (res==2 || res==3)
		{ //Remove dir from cached dirs
			CRemoveDirData *pData=(CRemoveDirData *)m_Operation.pData;
			CDirectoryCache cache;
			cache.Lock();
			BOOL bCached=TRUE;
			t_directory dir;
			BOOL res=cache.Lookup(pData->path,m_CurrentServer,dir);
			if (!res)
				bCached=FALSE;
			if (!res && m_pDirectoryListing)
			{
				if (m_pDirectoryListing->path==pData->path)
				{
					dir=*m_pDirectoryListing;
					res=TRUE;
				}
			}
			t_directory WorkingDir;
			BOOL bFound=m_pOwner->GetWorkingDir(&WorkingDir);
			if (!res && bFound)
				if (WorkingDir.path==pData->path)
				{
					dir=WorkingDir;
					res=TRUE;
				}
			if (res)
			{
				BOOL found=FALSE;
				for (int i=0;i<dir.num;i++)
				{
					if (dir.direntry[i].name == pData->m_DirName)
					{
						ASSERT(dir.direntry[i].dir);
						found=TRUE;
						break;
					}
				}
				if (found)
				{
					t_directory::t_direntry *direntry=new t_directory::t_direntry[dir.num-1];
					int j=0;
					int i;
					for (i=0; i<dir.num; i++)
					{
						if (dir.direntry[i].name==pData->m_DirName)
							continue;
						direntry[j]=dir.direntry[i];
						j++;
					}
					delete [] dir.direntry;
					dir.direntry=direntry;
					dir.num--;
					cache.Store(dir, bCached);
					BOOL updated=FALSE;
					if (m_pDirectoryListing)
						if (m_pDirectoryListing->path==dir.path)
						{
							updated=TRUE;
							SetDirectoryListing(&dir);
						}
					if (!updated)
						if (WorkingDir.path==dir.path)
						{
							updated=TRUE;
							m_pOwner->SetWorkingDir(&dir);
						}
				}
			}
			if (cache.Lookup(pData->path,pData->m_DirName,m_CurrentServer,dir))
				cache.Purge(dir.path,dir.server);
			cache.Unlock();
			ResetOperation(FZ_REPLY_OK);
		}
		else
			ResetOperation(FZ_REPLY_ERROR);
	}
}

int CFtpControlSocket::CheckOverwriteFile()
{
	int nReplyError=0;
	CFileStatus64 status;
	BOOL res=GetStatus64(((CFileTransferData*)m_Operation.pData)->transferfile.localfile, status);
	if (!res)
		if (!((CFileTransferData *)m_Operation.pData)->transferfile.get)
			nReplyError=FZ_REPLY_CRITICALERROR; //File has to exist when uploading
		else
			m_Operation.nOpState=FILETRANSFER_TYPE;
	else
	{
		if (status.m_attribute&0x10)
			nReplyError=FZ_REPLY_CRITICALERROR; //Can't transfer to/from dirs
		else
		{			
			_int64 localsize;
			if (!GetLength64(((CFileTransferData*)m_Operation.pData)->transferfile.localfile, localsize))
				if (!((CFileTransferData *)m_Operation.pData)->transferfile.get)
					nReplyError=FZ_REPLY_CRITICALERROR;
				else
					m_Operation.nOpState=FILETRANSFER_TYPE;

			CTime *localtime=new CTime(status.m_mtime);
			BOOL bRemoteFileExists=FALSE;
			__int64 remotesize=-1;
			CTime *remotetime=0;
			if (m_pDirectoryListing)
				for (int i=0;i<m_pDirectoryListing->num;i++)
				{
					CString remotefile=((CFileTransferData *)m_Operation.pData)->transferfile.remotefile;
					if (m_pDirectoryListing->direntry[i].name==remotefile)
					{
						remotesize=m_pDirectoryListing->direntry[i].size;
						if (m_pDirectoryListing->direntry[i].date.hasdate)
							remotetime=new CTime(m_pDirectoryListing->direntry[i].date.year,
								m_pDirectoryListing->direntry[i].date.month,
								m_pDirectoryListing->direntry[i].date.day,
								m_pDirectoryListing->direntry[i].date.hastime?m_pDirectoryListing->direntry[i].date.hour:0,
								m_pDirectoryListing->direntry[i].date.hastime?m_pDirectoryListing->direntry[i].date.minute:0,
								0,-1);
						bRemoteFileExists=TRUE;
						break;
					}
				}
			else
			{
				if ( ((CFileTransferData *)m_Operation.pData)->pFileSize )
				{
					remotesize = *((CFileTransferData *)m_Operation.pData)->pFileSize;
					bRemoteFileExists=TRUE;
				}
				if ( ((CFileTransferData *)m_Operation.pData)->pFileTime )
				{
					remotetime = new CTime;
					*remotetime = *((CFileTransferData *)m_Operation.pData)->pFileTime;
					bRemoteFileExists=TRUE;
				}
			}
			
			if (bRemoteFileExists || ((CFileTransferData *)m_Operation.pData)->transferfile.get )
			{
				COverwriteRequestData *pData=new COverwriteRequestData;
				t_transferfile *pTransferFile=new t_transferfile;
				*pTransferFile=((CFileTransferData *)m_Operation.pData)->transferfile;
				pData->pTransferFile=pTransferFile;
				if (((CFileTransferData *)m_Operation.pData)->transferfile.get)
				{
					int pos=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.ReverseFind('\\');
					ASSERT(pos!=-1);
					pData->FileName1=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.Mid(pos+1);
					pData->FileName2=((CFileTransferData *)m_Operation.pData)->transferfile.remotefile;
					pData->path1=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.Left(pos+1);
					pData->path2=((CFileTransferData *)m_Operation.pData)->transferfile.remotepath.GetPath();
					pData->size1=localsize;
					pData->size2=remotesize;
					pData->time1=localtime;
					pData->time2=remotetime;
				}
				else
				{
					int pos=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.ReverseFind('\\');
					ASSERT(pos!=-1);
					pData->FileName1=((CFileTransferData *)m_Operation.pData)->transferfile.remotefile;
					pData->FileName2=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.Mid(pos+1);
					pData->path1=((CFileTransferData *)m_Operation.pData)->transferfile.remotepath.GetPath();
					pData->path2=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.Left(pos+1);
					pData->size1=remotesize;
					pData->size2=localsize;
					pData->time1=remotetime;
					pData->time2=localtime;								
				}
				pData->nRequestID=m_pOwner->GetNextAsyncRequestID();
				if (!PostMessage(m_pOwner->m_hOwnerWnd, m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_OVERWRITE), (LPARAM)pData))
				{
					delete pData;
					nReplyError=FZ_REPLY_ERROR;
				}
				else
				{
					m_bCheckForTimeout=FALSE;
					m_Operation.nOpState=FILETRANSFER_WAIT;
				}
			}
			else
			{
				m_Operation.nOpState=FILETRANSFER_TYPE;
				delete localtime;
				delete remotetime;
			}
		}
	}
	return nReplyError;
}

void CFtpControlSocket::SetFileExistsAction(int nAction, COverwriteRequestData *pData)
{
	if (!pData)
		return;
	if (!(m_Operation.nOpMode&CSMODE_TRANSFER))
		return;
	if (m_Operation.nOpState!=FILETRANSFER_WAIT)
		return;
	m_bCheckForTimeout=TRUE;
	int nReplyError=0;
	switch (nAction)
	{
	case FILEEXISTS_SKIP:
		nReplyError=FZ_REPLY_OK;
		break;
	case FILEEXISTS_OVERWRITE:
		((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_TYPE;
		break;
	case FILEEXISTS_OVERWRITEIFNEWER:
		if ( !pData->time1 || !pData->time2 )
			((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_TYPE;
		else if (*pData->time2<=*pData->time1)
			nReplyError=FZ_REPLY_OK;
		else
			((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_TYPE;
		break;
	case FILEEXISTS_RENAME:
		if (((CFileTransferData *)m_Operation.pData)->transferfile.get)
		{
			CFileStatus64 status;
			if (GetStatus64(pData->FileName1, status))
			{
				ShowStatus(IDS_ERRORMSG_NAMEINUSE,1);
				nReplyError=FZ_REPLY_CRITICALERROR;
			}
			else
			{
				((CFileTransferData *)m_Operation.pData)->transferfile.localfile=pData->path1+pData->FileName1;
				//Replace invalid characters in the local filename
				int pos=((CFileTransferData *)m_Operation.pData)->transferfile.localfile.ReverseFind('\\');
				for (int i=(pos+1);i<((CFileTransferData *)m_Operation.pData)->transferfile.localfile.GetLength();i++)
					if (((CFileTransferData *)m_Operation.pData)->transferfile.localfile[i]==':')
						((CFileTransferData *)m_Operation.pData)->transferfile.localfile.SetAt(i, '_');
					
				((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_TYPE;
			}						
		}
		else
		{
			ASSERT(m_pDirectoryListing);
			int i;
			for (i=0; i<m_pDirectoryListing->num; i++)
			{
				if (m_pDirectoryListing->direntry[i].name==pData->FileName1)
				{
					ShowStatus(IDS_ERRORMSG_NAMEINUSE,1);
					nReplyError=FZ_REPLY_CRITICALERROR;
					break;
				}
			}
			if (i==m_pDirectoryListing->num)
			{
				((CFileTransferData *)m_Operation.pData)->transferfile.remotefile=pData->FileName1;
				((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_TYPE;
			}
		}
		break;
	case FILEEXISTS_RESUME:
		if (pData->size1 >= 0)
		{
			((CFileTransferData *)m_Operation.pData)->transferdata.bResume = TRUE;
		}
		((CFileTransferData *)m_Operation.pData)->nWaitNextOpState=FILETRANSFER_TYPE;
		break;
	}
	if (nReplyError==FZ_REPLY_OK)
		ResetOperation(FZ_REPLY_OK);
	else if (nReplyError)
		ResetOperation(FZ_REPLY_ERROR|nReplyError); //Error transferring the file
	else
		FileTransfer();
}

void CFtpControlSocket::SendKeepAliveCommand()
{
	m_bKeepAliveActive=TRUE;
	//Choose a random command from the list
	char commands[4][7]={"PWD","REST 0","TYPE A","TYPE I"};
	int choice=(rand()*4)/(RAND_MAX+1);
	Send(commands[choice]);
}

void CFtpControlSocket::MakeDir(const CServerPath &path)
{
	//Directory creation works like this:
	//Find existing parent and create subdirs one by one
	LogMessage(__FILE__, __LINE__, this, FZ_LOG_DEBUG, _T("MakeDir(\"%s\")  OpMode=%d OpState=%d"), path.GetPath(), m_Operation.nOpMode, m_Operation.nOpState);
	
	if (m_Operation.nOpState == MKD_INIT)
	{
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode==CSMODE_NONE);
		ASSERT(!m_Operation.pData);
		m_Operation.nOpMode = CSMODE_MKDIR;
		if (!Send("CWD "+path.GetParent().GetPath()))
			return;
		CMakeDirData *data = new CMakeDirData;
		data->path = path;
		data->Current = path.GetParent();
		data->Segments.push_front(path.GetLastSegment());
		m_Operation.pData = data;
		m_Operation.nOpState = MKD_FINDPARENT;
	}
	else if (m_Operation.nOpState==MKD_FINDPARENT)
	{
		ASSERT(m_Operation.nOpMode==CSMODE_MKDIR);
		ASSERT(path.IsEmpty());
		ASSERT(m_Operation.pData);
		CMakeDirData *pData=(CMakeDirData *)m_Operation.pData;
		int res=GetReplyCode();
		if (res==2 || res==3)
		{
			m_pOwner->SetCurrentPath(pData->Current);

			m_Operation.nOpState=MKD_MAKESUBDIRS;
			pData->Current.AddSubdir(pData->Segments.front());
			CString Segment=pData->Segments.front();
			pData->Segments.pop_front();
			if (Send( _T("MKD ") + Segment))
				m_Operation.nOpState = MKD_CHANGETOSUBDIR;
			else
				return;
		}
		else
		{
			if (!pData->Current.HasParent())
				ResetOperation(FZ_REPLY_ERROR);
			else
			{
				pData->Segments.push_front(pData->Current.GetLastSegment());
				pData->Current=pData->Current.GetParent();
				if (!Send("CWD "+pData->Current.GetPath()))
					return;		
			}
		}
	}
	else if (m_Operation.nOpState==MKD_MAKESUBDIRS)
	{
		int res=GetReplyCode();
		if (res==2 || res==3)
		{ //Create dir entry in parent dir
			CMakeDirData *pData=(CMakeDirData *)m_Operation.pData;

			ASSERT(!pData->Segments.empty());

			m_pOwner->SetCurrentPath(pData->Current);
			
			pData->Current.AddSubdir(pData->Segments.front());
			CString Segment=pData->Segments.front();
			pData->Segments.pop_front();
			if (Send( _T("MKD ") + Segment))
				m_Operation.nOpState=MKD_CHANGETOSUBDIR;
			else
				return;
		}
		else
			ResetOperation(FZ_REPLY_ERROR);
	}
	else if (m_Operation.nOpState==MKD_CHANGETOSUBDIR)
	{
		CMakeDirData *pData=(CMakeDirData *)m_Operation.pData;
		int res=GetReplyCode();
		if (res==2 || res==3 || //Creation successful
		m_RecvBuffer.front()==_T("550 Directory already exists") )//Creation was successful, although someone else did the work for us
		{ //Create dir entry in parent dir
			CServerPath path2=pData->Current;
			if (path2.HasParent())
			{
				CString name=path2.GetLastSegment();
				path2=path2.GetParent();

				CDirectoryCache cache;
				t_directory dir;
				cache.Lock();
				BOOL bCached = TRUE;
				BOOL res = cache.Lookup(path2, m_CurrentServer,dir);
				if (!res)
					bCached = FALSE;
				if (!res && m_pDirectoryListing)
				{
					if (m_pDirectoryListing->path == path2)
					{
						dir = *m_pDirectoryListing;
						res = TRUE;
					}
				}
				t_directory WorkingDir;
				BOOL bFound = m_pOwner->GetWorkingDir(&WorkingDir);
				if (!res && bFound)
					if (WorkingDir.path == path2)
					{
						dir = WorkingDir;
						res = TRUE;
					}
				if (!res)
				{
					dir.path = path2;
					dir.server = m_CurrentServer;
				}

				int i;
				for (i=0; i<dir.num; i++)
					if (dir.direntry[i].name==name)
					{
						LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Dir already exists in cache!"));
						break;
					}
				if (i==dir.num)
				{
					t_directory::t_direntry *entries=new t_directory::t_direntry[dir.num+1];
					for (i=0;i<dir.num;i++)
						entries[i]=dir.direntry[i];
					entries[i].name=name;
					entries[i].lName=name;
					entries[i].lName.MakeLower();
					entries[i].dir=TRUE;
					entries[i].date.hasdate=FALSE;
					entries[i].size=-1;
					entries[i].bUnsure=FALSE;
					delete [] dir.direntry;
					dir.direntry=entries;
					dir.num++;

					cache.Store(dir, bCached);
					BOOL updated = FALSE;
					if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
					{
						updated = TRUE;
						SetDirectoryListing(&dir, bFound && WorkingDir.path == dir.path);
					}
					if (!updated)
						if (bFound && WorkingDir.path == dir.path)
						{
							updated = TRUE;
							m_pOwner->SetWorkingDir(&dir);
						}
				}
				
				cache.Unlock();
			}
		}

		//Continue operation even if MKD failed, maybe another thread did create this directory for us
		if (pData->Segments.empty())
			ResetOperation(FZ_REPLY_OK);
		else
		{
			if (Send( _T("CWD ") + pData->Current.GetPath()))
				m_Operation.nOpState=MKD_MAKESUBDIRS;
			else
				return;
		}				
	}
	else 
		ASSERT(FALSE);
}

void CFtpControlSocket::Rename(CString oldName, CString newName, const CServerPath &path, const CServerPath &newPath)
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("Rename(\"%s\", \"%s\", \"%s\")  OpMode=%d OpState=%d"), oldName, newName, path.GetPath(),
			   m_Operation.nOpMode, m_Operation.nOpState);
	class CRenameData : public CFtpControlSocket::t_operation::COpData
	{
	public:
		CRenameData() {}
		virtual ~CRenameData() {}
		CString oldName, newName;
		CServerPath path;
		CServerPath newPath;
	};
	if (oldName != "")
	{
		ASSERT(newName != "");
		ASSERT(!path.IsEmpty());
		ASSERT(m_Operation.nOpMode == CSMODE_NONE);
		ASSERT(m_Operation.nOpState == -1);
		ASSERT(!m_Operation.pData);
		m_Operation.nOpMode = CSMODE_RENAME;
		if (!Send("RNFR " + path.GetPath() + oldName))
			return;
		CRenameData *data = new CRenameData;
		data->oldName = oldName;
		data->newName = newName;
		data->path = path;
		data->newPath = newPath;
		m_Operation.pData = data;
	}
	else
	{
		ASSERT(oldName == "");
		ASSERT(path.IsEmpty());
		ASSERT(m_Operation.nOpMode == CSMODE_RENAME);
		ASSERT(m_Operation.pData);
		CRenameData *pData = reinterpret_cast<CRenameData *>(m_Operation.pData);
		
		if (m_Operation.nOpState == -1)
		{
			int res = GetReplyCode();
			if (res == 2 || res == 3)
			{
				m_Operation.nOpState++;
				if (pData->newPath.IsEmpty())
				{
					if (!Send("RNTO " + pData->path.GetPath() + ((CRenameData *)m_Operation.pData)->newName))
						return;
				}
				else
					if (!Send("RNTO " + pData->newPath.GetPath() + ((CRenameData *)m_Operation.pData)->newName))
						return;
			}
			else
				ResetOperation(FZ_REPLY_ERROR);
		}
		else
		{
			int res = GetReplyCode();
			if (res == 2 || res == 3)
			{ //Rename entry in cached directory
				CRenameData *pData = reinterpret_cast<CRenameData *>(m_Operation.pData);
				
				CDirectoryCache cache;

				cache.Lock();
				
				//Rename all references to the directory in the cache
				if (pData->newPath.IsEmpty())
					cache.Rename(pData->path, pData->oldName, pData->newName, m_CurrentServer);
				
				BOOL bCached = TRUE;
				t_directory dir;
				BOOL res = cache.Lookup(pData->path, m_CurrentServer, dir);
				if (!res)
					bCached = FALSE;
				if (!res && m_pDirectoryListing)
				{
					if (m_pDirectoryListing->path == pData->path)
					{
						dir = *m_pDirectoryListing;
						res = TRUE;
					}
				}
				t_directory WorkingDir;
				BOOL bFound = m_pOwner->GetWorkingDir(&WorkingDir);
				if (!res && bFound)
					if (WorkingDir.path == pData->path)
					{
						dir = WorkingDir;
						res = TRUE;
					}
				if (res)
				{
					for (int i=0; i<dir.num; i++)
						if (dir.direntry[i].name == pData->oldName)
						{
							if (pData->newPath.IsEmpty())
							{
								dir.direntry[i].name = pData->newName;
								dir.direntry[i].lName = pData->newName;
								dir.direntry[i].lName.MakeLower();

								CDirectoryCache cache;
								cache.Store(dir, bCached);
								BOOL updated = FALSE;
								if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
								{
									updated = TRUE;
									SetDirectoryListing(&dir, WorkingDir.path == dir.path);
								}
								if (!updated)
									if (WorkingDir.path == dir.path)
									{
										updated = TRUE;
										m_pOwner->SetWorkingDir(&dir);
									}
							}
							else
							{
								t_directory::t_direntry oldentry = dir.direntry[i];

								for (int j = i+1; j < dir.num; j++)
								{
									dir.direntry[j-1] = dir.direntry[j];
								}
								dir.num--;

								cache.Store(dir, bCached);
								
								// If directory, delete old directory from cache
								t_directory olddir;
								res = cache.Lookup(pData->path, pData->oldName, m_CurrentServer, olddir);
								if (res)
								{
									cache.Purge(olddir.path, m_CurrentServer);
									t_directory newdir;
									if (cache.Lookup(dir.path, m_CurrentServer, newdir))
										dir = newdir;
								}

								BOOL updated = FALSE;
								if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
								{
									updated = TRUE;
									SetDirectoryListing(&dir, WorkingDir.path == dir.path);
								}
								if (!updated)
									if (WorkingDir.path == dir.path)
									{
										updated = TRUE;
										m_pOwner->SetWorkingDir(&dir);
									}
																	
								BOOL bCached = TRUE;
								BOOL res = cache.Lookup(pData->newPath, m_CurrentServer, dir);
								if (!res)
									bCached = FALSE;
								if (!res && m_pDirectoryListing)
								{
									if (m_pDirectoryListing->path == pData->newPath)
									{
										dir = *m_pDirectoryListing;
										res = TRUE;
									}
								}
								t_directory WorkingDir;
								BOOL bFound = m_pOwner->GetWorkingDir(&WorkingDir);
								if (!res && bFound)
									if (WorkingDir.path == pData->newPath)
									{
										dir = WorkingDir;
										res = TRUE;
									}
								if (res)
								{
									t_directory::t_direntry *direntry = new t_directory::t_direntry[dir.num + 1];
									for (int i = 0; i < dir.num; i++)
										direntry[i] = dir.direntry[i];
									direntry[dir.num] = oldentry;
									direntry[dir.num].name = pData->newName;
									direntry[dir.num].lName = pData->newName;
									direntry[dir.num].lName.MakeLower();
									dir.num++;
									delete [] dir.direntry;
									dir.direntry = direntry;
									
									cache.Store(dir, bCached);
									BOOL updated = FALSE;
									if (m_pDirectoryListing && m_pDirectoryListing->path == dir.path)
									{
										updated = TRUE;
										SetDirectoryListing(&dir, bFound && WorkingDir.path == dir.path);
									}
									if (!updated)
										if (bFound && WorkingDir.path == dir.path)
										{
											updated = TRUE;
											m_pOwner->SetWorkingDir(&dir);
										}
								}
							}
							break;
						}
				}
				cache.Unlock();
				ResetOperation(FZ_REPLY_OK);
			}
			else
				ResetOperation(FZ_REPLY_ERROR);
		}
	}
}

int CFtpControlSocket::OnSslNotify(int nType, int nCode)
{
	if (nType==SSL_INFO)
	{
		if (nCode==SSL_INFO_ESTABLISHED)
		{
			ShowStatus(IDS_STATUSMSG_SSLESTABLISHED, 0);
			if (m_Operation.nOpState==CONNECT_SSL_WAITDONE)
			{
				LogOnToServer();
			}
		}
	}
	else if (nType==SSL_FAILURE)
	{
		switch (nCode)
		{
		case SSL_FAILURE_ESTABLISH:
			ShowStatus(IDS_ERRORMSG_CANTESTABLISHSSLCONNECTION, 1);
			break;
		case SSL_FAILURE_LOADDLLS:
			ShowStatus(IDS_ERRORMSG_CANTLOADSSLDLLS, 1);
			break;
		case SSL_FAILURE_INITSSL:
			ShowStatus(IDS_ERRORMSG_CANTINITSSL, 1);
			break;
		case SSL_FAILURE_VERIFYCERT:
			ShowStatus(IDS_ERRORMSG_CERTREJECTED, 1);
			break;
		}
		if (m_Operation.nOpMode==CSMODE_CONNECT && m_Operation.nOpState==-1 && nCode==SSL_FAILURE_UNKNOWN)
			return 1;
		TriggerEvent(FD_CLOSE);
	}
	else if (nType==SSL_VERIFY_CERT)
	{
		CVerifyCertRequestData *pData=new CVerifyCertRequestData;
		pData->nRequestID=m_pOwner->GetNextAsyncRequestID();

		pData->pCertData = new t_SslCertData;
		int res = m_pSslLayer->GetPeerCertificateData(*pData->pCertData);
		if (!res)
		{
			delete pData->pCertData;
			delete pData;
			return 0;
		}
		m_bCheckForTimeout=FALSE;
		if (!PostMessage(m_pOwner->m_hOwnerWnd,m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_VERIFYCERT), (LPARAM)pData))
		{
			delete pData;
			ResetOperation(FZ_REPLY_ERROR);
		}
		return -1;
	}
	return 1;
}

void CFtpControlSocket::SetVerifyCertResult(int nResult, t_SslCertData *pData)
{
	ASSERT(pData);
	if (!m_pSslLayer)
		return;
	if (!m_Operation.nOpMode==CSMODE_CONNECT)
		return;
	m_bCheckForTimeout=TRUE;
	m_pSslLayer->SetNotifyReply( pData->priv_data, SSL_VERIFY_CERT, nResult );
	m_LastRecvTime = CTime::GetCurrentTime();
}

void CFtpControlSocket::OnTimer()
{
	CheckForTimeout();
	ResumeTransfer();
	if (COptions::GetOptionVal(OPTION_KEEPALIVE))
	{
		if (!m_pOwner->IsBusy() && m_pOwner->IsConnected() && !m_bKeepAliveActive)
		{
			//Getting intervals for the Keep Alive feature
			int low=COptions::GetOptionVal(OPTION_INTERVALLOW);
			int diff=COptions::GetOptionVal(OPTION_INTERVALHIGH)-low;
			
			//Choose a new delay
			static int delay=low+(rand()*diff)/RAND_MAX;
			
			CTimeSpan span=CTime::GetCurrentTime()-m_LastSendTime;
			if (span.GetTotalSeconds()>=delay)
				SendKeepAliveCommand();
		}
	}
}

BOOL CFtpControlSocket::IsReady()
{
	return !m_bKeepAliveActive;
}

void CFtpControlSocket::Chmod(CString filename, const CServerPath &path, int nValue)
{
	m_Operation.nOpMode=CSMODE_CHMOD;
	CString str;
	str.Format( _T("SITE CHMOD %03d %s%s"), nValue, path.GetPath(), filename);
	Send(str);
}

void CFtpControlSocket::SetAsyncRequestResult(int nAction, CAsyncRequestData *pData)
{
	switch (pData->nRequestType)
	{
	case FZ_ASYNCREQUEST_OVERWRITE:
		SetFileExistsAction(nAction, (COverwriteRequestData *)pData);
		break;
	case FZ_ASYNCREQUEST_VERIFYCERT:
		SetVerifyCertResult(nAction, ((CVerifyCertRequestData *)pData)->pCertData );
		break;
	case FZ_ASYNCREQUEST_GSS_AUTHFAILED:
		if (m_Operation.nOpMode!=CSMODE_CONNECT || m_Operation.nOpState!=CONNECT_GSS_FAILED)
			break;
		if (!m_RecvBuffer.empty() && m_RecvBuffer.front()!= _T(""))
		{
			DoClose();
			break; 
		}
		if (!nAction)
		{
			DoClose(FZ_REPLY_CRITICALERROR|FZ_REPLY_CANCEL);
			ShowStatus(IDS_ERRORMSG_INTERRUPTED,1);
			break;
		}
		m_Operation.nOpState=-1;
		LogOnToServer(TRUE);
		break;
	case FZ_ASYNCREQUEST_GSS_NEEDPASS:
		if (m_Operation.nOpMode!=CSMODE_CONNECT ||
			m_Operation.nOpState != CONNECT_GSS_NEEDPASS)
			break;
		if (!m_RecvBuffer.empty() && m_RecvBuffer.front()!="")
		{
			DoClose();
			break; 
		}
		if (!nAction)
		{
			DoClose(FZ_REPLY_CRITICALERROR|FZ_REPLY_CANCEL);
			ShowStatus(IDS_ERRORMSG_INTERRUPTED,1);
			break;
		}
		else
		{
			m_CurrentServer.pass=((CGssNeedPassRequestData *)pData)->pass;
			m_Operation.nOpState=((CGssNeedPassRequestData *)pData)->nOldOpState;
			LogOnToServer(TRUE);
		}
		break;
	case FZ_ASYNCREQUEST_GSS_NEEDUSER:
		if (m_Operation.nOpMode != CSMODE_CONNECT ||
			m_Operation.nOpState != CONNECT_GSS_NEEDUSER)
			break;
		if (!m_RecvBuffer.empty() && m_RecvBuffer.front() != "")
		{
			DoClose();
			break; 
		}
		if (!nAction)
		{
			DoClose(FZ_REPLY_CRITICALERROR | FZ_REPLY_CANCEL);
			ShowStatus(IDS_ERRORMSG_INTERRUPTED, 1);
			break;
		}
		else
		{
			m_CurrentServer.user = ((CGssNeedUserRequestData *)pData)->user;
			m_Operation.nOpState=((CGssNeedUserRequestData *)pData)->nOldOpState;
			LogOnToServer(TRUE);
		}
		break;
	default:
		LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Unknown request reply %d"), pData->nRequestType);
		break;
	}
}

int CFtpControlSocket::OnLayerCallback(const CAsyncSocketExLayer *pLayer, int nType, int nParam1, int nParam2)
{
	ASSERT(pLayer);
	if (nType==LAYERCALLBACK_STATECHANGE)
	{
		if (pLayer==m_pSslLayer)
		{
			LogMessage(__FILE__, __LINE__, this,FZ_LOG_INFO, _T("m_pSslLayer changed state from %d to %d"), nParam2, nParam1);
			return 1;
		}
	}
	else if (nType==LAYERCALLBACK_LAYERSPECIFIC)
	{
		if (pLayer == m_pSslLayer)
		{
			USES_CONVERSION;

			switch (nParam1)
			{
			case SSL_INFO:
				switch (nParam2)
				{
				case SSL_INFO_ESTABLISHED:
					ShowStatus(IDS_STATUSMSG_SSLESTABLISHED, 0);
					if (m_Operation.nOpState == CONNECT_SSL_WAITDONE)
					{
						LogOnToServer();
					}
					break;
				}
				break;
			case SSL_FAILURE:
				switch (nParam2)
				{
				case SSL_FAILURE_UNKNOWN:
					ShowStatus(IDS_ERRORMSG_UNKNOWNSSLERROR, 1);
					break;
				case SSL_FAILURE_ESTABLISH:
					ShowStatus(IDS_ERRORMSG_CANTESTABLISHSSLCONNECTION, 1);
					break;
				case SSL_FAILURE_LOADDLLS:
					ShowStatus(IDS_ERRORMSG_CANTLOADSSLDLLS, 1);
					break;
				case SSL_FAILURE_INITSSL:
					ShowStatus(IDS_ERRORMSG_CANTINITSSL, 1);
					break;
				case SSL_FAILURE_VERIFYCERT:
					ShowStatus(IDS_ERRORMSG_SSLCERTIFICATEERROR, 1);
					break;
				}
				TriggerEvent(FD_CLOSE);
				break;
			case SSL_VERBOSE_INFO:
				LogMessage(FZ_LOG_INFO, A2T((char *)nParam2));
				break;
			case SSL_VERBOSE_WARNING:
				LogMessage(FZ_LOG_WARNING, A2T((char *)nParam2));
				break;
			case SSL_VERIFY_CERT:
				t_SslCertData *pData = new t_SslCertData;
				if (m_pSslLayer->GetPeerCertificateData(*pData))
				{
					CVerifyCertRequestData *pRequestData=new CVerifyCertRequestData;
					pRequestData->nRequestID=m_pOwner->GetNextAsyncRequestID();
					
					pRequestData->pCertData = pData;

					m_bCheckForTimeout = FALSE;
					if (!PostMessage(m_pOwner->m_hOwnerWnd,m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_ASYNCREQUEST, FZ_ASYNCREQUEST_VERIFYCERT), (LPARAM)pRequestData))
					{
						delete pRequestData->pCertData;
						delete pRequestData;
						ResetOperation(FZ_REPLY_ERROR);
					}
					return 2;
				}
				else
				{
					delete pData;
					return 0;
				}
				break;
			}
			return 1;
		}
		else if (pLayer == m_pGssLayer)
		{
			if (nParam1 == GSS_AUTHCOMPLETE ||
				nParam1 == GSS_AUTHFAILED)
			{
				LogOnToServer(TRUE);
				return 0;
			}
		}
	}
	
	return CControlSocket::OnLayerCallback(pLayer, nType, nParam1, nParam2);
}

BOOL CFtpControlSocket::ParsePwdReply(CString reply)
{
	CListData *pData = static_cast<CListData *>(m_Operation.pData);
	ASSERT(pData);

	int pos1 = reply.Find('"');
	int pos2 = reply.ReverseFind('"');
	if (pos1==-1 || pos2==-1 || pos1>=pos2)
	{
		LogMessage(__FILE__, __LINE__, this, FZ_LOG_WARNING, _T("No quoted path found, try using first token as path") );
		pos1 = reply.Find(' ');
		if (pos1 != -1)
		{
			pos2 = reply.Find(' ', pos1 + 1);
			if (pos2 == -1)
				pos2 = reply.GetLength();
		}

		if (pos1 == -1)
		{
			LogMessage(__FILE__, __LINE__, this, FZ_LOG_WARNING, _T("Can't parse path!") );
			ResetOperation(FZ_REPLY_ERROR);
			return FALSE;
		}
	}
	pData->realpath.SetServer(m_CurrentServer);
	if (!pData->realpath.SetPath(reply.Mid(pos1+1, pos2-pos1-1)))
	{
		LogMessage(__FILE__, __LINE__, this, FZ_LOG_WARNING, _T("Can't parse path!") );
		ResetOperation(FZ_REPLY_ERROR);
		return FALSE;
	}
	pData->rawpwd = reply.Mid(pos1+1, pos2-pos1-1);
	m_pOwner->SetCurrentPath(pData->realpath);
	if (m_Operation.nOpState == LIST_PWD2 && pData->subdir != "")
		return TRUE;
	if (pData->nListMode & FZ_LIST_USECACHE)
	{
		t_directory dir;
		CDirectoryCache cache;
		BOOL res = cache.Lookup(pData->realpath, m_CurrentServer, dir);
		if (res)
		{
			BOOL bExact = TRUE;
			if (pData->nListMode & FZ_LIST_EXACT)
				for (int i=0; i<dir.num; i++)
					if (dir.direntry[i].bUnsure || (dir.direntry[i].size==-1 && !dir.direntry[i].dir))
					{
						bExact = FALSE;
						break;
					}
					if (bExact)
					{
						ShowStatus(IDS_STATUSMSG_DIRLISTSUCCESSFUL, 0);
						SetDirectoryListing(&dir);
						ResetOperation(FZ_REPLY_OK);
						return FALSE;
					}
		}
	}

	return TRUE;
}