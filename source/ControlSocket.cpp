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

// ControlSocket.cpp: Implementierung der Klasse CControlSocket.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ControlSocket.h"
#include "mainthread.h"
#include "AsyncProxySocketLayer.h"
#include "AsyncSslSocketLayer.h"
#include "AsyncGssSocketLayer.h"
#include "SpeedLimit.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

std::list<CControlSocket::t_ActiveList> CControlSocket::m_DownloadInstanceList;
std::list<CControlSocket::t_ActiveList> CControlSocket::m_UploadInstanceList;

CTime CControlSocket::m_CurrentDownloadTime = CTime::GetCurrentTime();;
_int64 CControlSocket::m_CurrentDownloadLimit = 0;

CTime CControlSocket::m_CurrentUploadTime = CTime::GetCurrentTime();;
_int64 CControlSocket::m_CurrentUploadLimit = 0;

CCriticalSection CControlSocket::m_SpeedLimitSync;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CControlSocket::CControlSocket(CMainThread *pMainThread)
{
	ASSERT(pMainThread);
	m_pOwner=pMainThread;

	m_Operation.nOpMode=0;
	m_Operation.nOpState=-1;
	m_Operation.pData=0;

	m_pProxyLayer = NULL;
	m_pSslLayer = NULL;
	m_pGssLayer = NULL;

	m_pDirectoryListing=0;
	m_pIdentControl=0;
}

CControlSocket::~CControlSocket()
{
	LogMessage(__FILE__, __LINE__, this, FZ_LOG_DEBUG, _T("~CControlSocket()"));
	Close();
}

/////////////////////////////////////////////////////////////////////////////
// Member-Funktion CControlSocket 
#define CONNECT_INIT -1
#define CONNECT_GSS -3
#define CONNECT_SSL_INIT -6
#define CONNECT_SSL_NEGOTIATE -5
#define CONNECT_SSL_WAITDONE -4 

void CControlSocket::ShowStatus(UINT nID, int type) const
{
	CString str;
	str.LoadString(nID);
	ShowStatus(str, type);
}

void CControlSocket::ShowStatus(CString status, int type) const
{
	if ( status.Left(5)==_T("PASS ") )
	{
		int len=status.GetLength()-5;
		status=_T("PASS ");
		for (int i=0;i<len;i++)
			status+="*";
	}
	else if ( status.Left(5)==_T("ACCT ") )
	{
		int len=status.GetLength()-5;
		status=_T("PASS ");
		for (int i=0;i<len;i++)
			status+="*";
	}
	status.Replace(_T("%"), _T("%%"));
	LogMessage(type, (LPCTSTR)status);
}


t_server CControlSocket::GetCurrentServer()
{
	return m_CurrentServer;
}

void CControlSocket::Close()
{
	if(m_pIdentControl)
		delete m_pIdentControl;
	m_pIdentControl=0;

	if (m_pDirectoryListing)
	{
		delete m_pDirectoryListing;
	}
	m_pDirectoryListing=0;
	CAsyncSocketEx::Close();

	delete m_pProxyLayer;
	m_pProxyLayer = NULL;

	delete m_pSslLayer;
	m_pSslLayer = NULL;

	delete m_pGssLayer;
	m_pGssLayer = NULL;

}

BOOL CControlSocket::Connect(LPCTSTR lpszHostAddress, UINT nHostPort)
{
	//Don't resolve host asynchronously when using proxies
	if (m_pProxyLayer)
	{
		//If using proxies, we can't use ident -> won't be reachable from outside
		
		return CAsyncSocketEx::Connect(lpszHostAddress, nHostPort);
	}
	BOOL res = CAsyncSocketEx::Connect(lpszHostAddress, nHostPort);
	int nLastError = WSAGetLastError();
	if (res || nLastError==WSAEWOULDBLOCK)
	{
		if (COptions::GetOptionVal(OPTION_IDENT))
			m_pIdentControl = new CIdentServerControl(this);
		WSASetLastError(nLastError);
	}
	
	return res;
}

void CControlSocket::SetDirectoryListing(t_directory *pDirectory, bool bSetWorkingDir /*=true*/)
{
	if (m_pDirectoryListing)
		delete m_pDirectoryListing;
	m_CurrentServer=pDirectory->server;
	m_pDirectoryListing=new t_directory;
	*m_pDirectoryListing=*pDirectory;

	if (bSetWorkingDir)
		m_pOwner->SetWorkingDir(pDirectory);
}

int CControlSocket::OnLayerCallback(const CAsyncSocketExLayer *pLayer, int nType, int nParam1, int nParam2)
{
	USES_CONVERSION;

	ASSERT(pLayer);
	if (nType==LAYERCALLBACK_STATECHANGE)
	{
		if (pLayer==m_pProxyLayer)
			LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("m_pProxyLayer changed state from %d to %d"), nParam2, nParam1);
		else if (pLayer == m_pGssLayer)
			LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("m_pGssLayer changed state from %d to %d"), pLayer, nParam2, nParam1);
		else
			LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("Layer @ %d changed state from %d to %d"), pLayer, nParam2, nParam1);
		return 1;
	}
	else if (nType==LAYERCALLBACK_LAYERSPECIFIC)
	{
		if (pLayer==m_pProxyLayer)
		{
			switch (nParam1)
			{
			case PROXYERROR_NOCONN:
				ShowStatus(IDS_ERRORMSG_PROXY_NOCONN, 1);
				break;
			case PROXYERROR_REQUESTFAILED:
				ShowStatus(IDS_ERRORMSG_PROXY_REQUESTFAILED, 1);
				if (nParam2)
					ShowStatus((LPCTSTR)nParam2, 1);
				break;
			case PROXYERROR_AUTHTYPEUNKNOWN:
				ShowStatus(IDS_ERRORMSG_PROXY_AUTHTYPEUNKNOWN, 1);
				break;
			case PROXYERROR_AUTHFAILED:
				ShowStatus(IDS_ERRORMSG_PROXY_AUTHFAILED, 1);
				break;
			case PROXYERROR_AUTHNOLOGON:
				ShowStatus(IDS_ERRORMSG_PROXY_AUTHNOLOGON, 1);
				break;
			case PROXYERROR_CANTRESOLVEHOST:
				ShowStatus(IDS_ERRORMSG_PROXY_CANTRESOLVEHOST, 1);
				break;
			default:
				LogMessage(__FILE__, __LINE__, this, FZ_LOG_WARNING, _T("Unknown proxy error") );
			}
		}
		else if (pLayer == m_pGssLayer)
		{
			switch(nParam1)
			{
			case GSS_INFO:
				LogMessage(FZ_LOG_INFO, A2CT((const char *)nParam2));
				break;
			case GSS_ERROR:
				LogMessage(FZ_LOG_APIERROR, A2CT((const char *)nParam2));
				break;
			case GSS_COMMAND:
				ShowStatus((char*)nParam2, 2);
				break;
			case GSS_REPLY:
				ShowStatus((char*)nParam2, 3);
				break;
			}
		}
	}
	return 1;
}

_int64 CControlSocket::GetSpeedLimit(CTime &time, int valType, int valValue, SPEEDLIMITSLIST &list)
{
	int type = COptions::GetOptionVal(valType);

	if ( type == 1)
		return ( _int64)COptions::GetOptionVal(valValue) * 1024;

	if ( type == 2)
	{
		CSingleLock lock(&COptions::m_Sync, TRUE);
		for ( unsigned int i = 0; i < list.size(); i++)
		{
			if ( list[ i]->IsItActive(time) && list[i]->m_Speed)
				return list[ i]->m_Speed * 1024;
		}
	}

	return ( _int64)1000000000000;	//I hope that when there will be something with 1000GB/s then I'll change it :)
}

_int64 CControlSocket::GetDownloadSpeedLimit( CTime &time)
{
	return GetSpeedLimit( time, OPTION_SPEEDLIMIT_DOWNLOAD_TYPE, OPTION_SPEEDLIMIT_DOWNLOAD_VALUE, COptions::m_DownloadSpeedLimits);
}

_int64 CControlSocket::GetUploadSpeedLimit( CTime &time)
{
	return GetSpeedLimit( time, OPTION_SPEEDLIMIT_UPLOAD_TYPE, OPTION_SPEEDLIMIT_UPLOAD_VALUE, COptions::m_UploadSpeedLimits);
}

_int64 CControlSocket::GetAbleToUDSize( bool &beenWaiting, CTime &curTime, _int64 &curLimit, std::list<CControlSocket::t_ActiveList>::iterator &iter, bool download, int nBufSize)
{
	beenWaiting = false;

	CTime nowTime = CTime::GetCurrentTime();
	_int64 ableToRead = BUFSIZE;

	if ( nowTime == curTime)
	{
		ableToRead = iter->nBytesAvailable;

		if ( ableToRead <= 0)
		{
			//	we should wait till next second
			nowTime = CTime::GetCurrentTime();

			while (nowTime == curTime && !iter->nBytesAvailable)
			{
				if (beenWaiting)
				{
					//Check if there are other commands in the command queue.
					MSG msg;
					if (PeekMessage(&msg, 0, m_pOwner->m_nInternalMessageID, m_pOwner->m_nInternalMessageID, PM_NOREMOVE))
					{
						LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("Message waiting in queue, resuming later"));
						return 0;
					}
				}
				m_SpeedLimitSync.Unlock();
				Sleep(100);
				m_SpeedLimitSync.Lock();
				nowTime = CTime::GetCurrentTime();
				beenWaiting = true;
			}
			
		}
		ableToRead = iter->nBytesAvailable;
	}

	if (nowTime != curTime)
	{
		if ( ableToRead > 0)
			ableToRead = 0;

		if (download)
		{
			curLimit = GetDownloadSpeedLimit( curTime);
			__int64 nMax = curLimit / CControlSocket::m_DownloadInstanceList.size();
			_int64 nLeft=0;
			int nCount=0;
			std::list<CControlSocket::t_ActiveList>::iterator iter2;
			for (iter2=CControlSocket::m_DownloadInstanceList.begin(); iter2!=CControlSocket::m_DownloadInstanceList.end(); iter2++)
			{
				if (iter2->nBytesAvailable>0)
				{
					nLeft+=iter2->nBytesAvailable;
					iter2->nBytesTransferred=1;
					nCount++;
				}
				else
				{
					iter2->nBytesTransferred=0;
				}
				iter2->nBytesAvailable=nMax;
			}
			if (nCount)
			{
				nMax=nLeft/nCount;
				for (iter2=CControlSocket::m_DownloadInstanceList.begin(); iter2!=CControlSocket::m_DownloadInstanceList.end(); iter2++)
				{
					if (!iter2->nBytesTransferred)
					{
						iter2->nBytesAvailable+=nMax;
						iter2->nBytesTransferred=0;
					}
					iter2->nBytesTransferred=0;
				}
			}
			for (iter2=CControlSocket::m_DownloadInstanceList.begin(); iter2!=CControlSocket::m_DownloadInstanceList.end(); iter2++)
				iter2->nBytesTransferred=0;
		}
		else
		{
			curLimit = GetUploadSpeedLimit(curTime);
			__int64 nMax = curLimit / CControlSocket::m_UploadInstanceList.size();
			_int64 nLeft=0;
			int nCount=0;
			std::list<CControlSocket::t_ActiveList>::iterator iter2;
			for (iter2=CControlSocket::m_UploadInstanceList.begin(); iter2!=CControlSocket::m_UploadInstanceList.end(); iter2++)
			{
				if (iter2->nBytesAvailable>0)
				{
					nLeft+=iter2->nBytesAvailable;
					iter2->nBytesTransferred=1;
					nCount++;
				}
				else
				{
					iter2->nBytesTransferred=0;
				}
				iter2->nBytesAvailable=nMax;
			}
			if (nCount)
			{
				nMax=nLeft/nCount;
				for (iter2=CControlSocket::m_UploadInstanceList.begin(); iter2!=CControlSocket::m_UploadInstanceList.end(); iter2++)
				{
					if (!iter2->nBytesTransferred)
					{
						iter2->nBytesAvailable+=nMax;
						iter2->nBytesTransferred=0;
					}
					iter2->nBytesTransferred=0;
				}
			}
			for (iter2=CControlSocket::m_UploadInstanceList.begin(); iter2!=CControlSocket::m_UploadInstanceList.end(); iter2++)
				iter2->nBytesTransferred=0;
		}
		ableToRead=iter->nBytesAvailable;
	}

	curTime = nowTime;

	if (!nBufSize)
		nBufSize = BUFSIZE;
	if (ableToRead > nBufSize)
		ableToRead = nBufSize;
	
	return ableToRead;
}

_int64 CControlSocket::GetAbleToDownloadSize( bool &beenWaiting, int nBufSize /*=0*/)
{
	CSingleLock lock(&m_SpeedLimitSync, TRUE);
	std::list<CControlSocket::t_ActiveList>::iterator iter;
	for (iter=CControlSocket::m_DownloadInstanceList.begin(); iter!=CControlSocket::m_DownloadInstanceList.end(); iter++)
		if (iter->pOwner == this)
			break;
	if (iter==CControlSocket::m_DownloadInstanceList.end())
	{
		CControlSocket::t_ActiveList item;
		item.nBytesAvailable = GetDownloadSpeedLimit(CTime::GetCurrentTime())/(CControlSocket::m_DownloadInstanceList.size()+1);
		item.nBytesTransferred = 0;
		item.pOwner = this;
		CControlSocket::m_DownloadInstanceList.push_back(item);
		iter=CControlSocket::m_DownloadInstanceList.end();
		iter--;
	}
	return GetAbleToUDSize( beenWaiting, m_CurrentDownloadTime, m_CurrentDownloadLimit, iter, true, nBufSize);
}

_int64 CControlSocket::GetAbleToUploadSize( bool &beenWaiting, int nBufSize)
{
	CSingleLock lock(&m_SpeedLimitSync, TRUE);
	std::list<CControlSocket::t_ActiveList>::iterator iter;
	for (iter=CControlSocket::m_UploadInstanceList.begin(); iter!=CControlSocket::m_UploadInstanceList.end(); iter++)
		if (iter->pOwner == this)
			break;
	if (iter==CControlSocket::m_UploadInstanceList.end())
	{
		CControlSocket::t_ActiveList item;
		item.nBytesAvailable = GetUploadSpeedLimit(CTime::GetCurrentTime())/(CControlSocket::m_UploadInstanceList.size()+1);
		item.nBytesTransferred = 0;
		item.pOwner = this;
		CControlSocket::m_UploadInstanceList.push_back(item);
		iter=CControlSocket::m_UploadInstanceList.end();
		iter--;
	}
	return GetAbleToUDSize( beenWaiting, m_CurrentUploadTime, m_CurrentUploadLimit, iter, false, nBufSize);
}

BOOL CControlSocket::RemoveActiveTransfer()
{
	BOOL bFound = FALSE;
	CSingleLock(&m_SpeedLimitSync, TRUE);
	std::list<CControlSocket::t_ActiveList>::iterator iter;
	for (iter=m_UploadInstanceList.begin(); iter!=m_UploadInstanceList.end(); iter++)
		if (iter->pOwner == this)
		{
			m_UploadInstanceList.erase(iter);
			bFound = TRUE;
			break;
		}
	for (iter=m_DownloadInstanceList.begin(); iter!=m_DownloadInstanceList.end(); iter++)
		if (iter->pOwner == this)
		{
			m_DownloadInstanceList.erase(iter);
			bFound = TRUE;
			break;
		}
	return bFound;
}

BOOL CControlSocket::SpeedLimitAddDownloadedBytes(_int64 nBytesDownloaded)
{
	CSingleLock(&m_SpeedLimitSync, TRUE);
	std::list<t_ActiveList>::iterator iter;
	for (iter=m_DownloadInstanceList.begin(); iter!=m_DownloadInstanceList.end(); iter++)
		if (iter->pOwner == this)
		{
			iter->nBytesAvailable -= nBytesDownloaded;
			iter->nBytesTransferred += nBytesDownloaded;
			return TRUE;
		}
	return FALSE;
}

BOOL CControlSocket::SpeedLimitAddUploadedBytes(_int64 nBytesUploaded)
{
	CSingleLock(&m_SpeedLimitSync, TRUE);
	std::list<t_ActiveList>::iterator iter;
	for (iter=m_UploadInstanceList.begin(); iter!=m_UploadInstanceList.end(); iter++)
		if (iter->pOwner == this)
		{
			iter->nBytesAvailable -= nBytesUploaded;
			iter->nBytesTransferred += nBytesUploaded;
			return TRUE;
		}
	return FALSE;
}

_int64 CControlSocket::SpeedLimitGetDownloadBytesAvailable()
{
	CSingleLock(&m_SpeedLimitSync, TRUE);
	std::list<t_ActiveList>::const_iterator iter;
	for (iter=m_DownloadInstanceList.begin(); iter!=m_DownloadInstanceList.end(); iter++)
		if (iter->pOwner == this)
			return iter->nBytesAvailable;
	return 0;
}

_int64 CControlSocket::SpeedLimitGetUploadBytesAvailable()
{
	CSingleLock(&m_SpeedLimitSync, TRUE);
	std::list<t_ActiveList>::const_iterator iter;
	for (iter=m_UploadInstanceList.begin(); iter!=m_UploadInstanceList.end(); iter++)
		if (iter->pOwner == this)
			return iter->nBytesAvailable;
	return 0;
}