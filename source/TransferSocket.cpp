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

// TransferSocket.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "filezilla.h"
#include "TransferSocket.h"
#include "mainthread.h"
#include "AsyncProxySocketLayer.h"
#include "AsyncGssSocketLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BUFSIZE 16384

#define STATE_WAITING		0
#define STATE_STARTING		1
#define STATE_STARTED		2
#define STATE_ONCLOSECALLED	4

/////////////////////////////////////////////////////////////////////////////
// CTransferSocket

CTransferSocket::CTransferSocket(CFtpControlSocket *pOwner, int nMode)
{
	ASSERT(pOwner);
	InitLog(pOwner);
	m_pOwner = pOwner;
	m_nMode = nMode;
	m_nTransferState = STATE_WAITING;
	m_bCheckTimeout = FALSE;
	m_pBuffer = 0;
	m_bufferpos = 0;
	m_ReadPos = 0;
	m_ReadBuffer = 0;
	m_ReadSize = 0;
	m_cLastChar = 0;
	m_pFile = 0;
	m_bListening = FALSE;
	m_bSentClose = FALSE;
	m_nInternalMessageID = 0;
	m_transferdata.transfersize = 0;
	m_transferdata.transferleft = 0;
	m_transferdata.nTransferStart = 0;
	m_nNotifyWaiting = 0;
	m_bShutDown = FALSE;

	UpdateStatusBar(true);
	
	for (int i = 0; i < SPEED_SECONDS; i++)
	{
		m_Transfered[i] = 0;
		m_UsedForTransfer[i] = 0;
	}

	m_pProxyLayer = NULL;
	m_pSslLayer = NULL;
	m_pGssLayer = NULL;

	m_pListResult = new CFtpListResult(pOwner->m_CurrentServer);
	m_LastUpdateTime.QuadPart = 0;
}

CTransferSocket::~CTransferSocket()
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("~CTransferSocket()"));
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = 0;
	}
	if (m_ReadBuffer)
	{
		delete [] m_ReadBuffer;
		m_ReadBuffer = 0;
	}
	PostMessage(m_pOwner->m_pOwner->m_hOwnerWnd, m_pOwner->m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_TRANSFERSTATUS, 0), 0);
	Close();
	RemoveAllLayers();
	delete m_pProxyLayer;
	delete m_pSslLayer;
	delete m_pGssLayer;
	m_pOwner->RemoveActiveTransfer();

	delete m_pListResult;
}

/////////////////////////////////////////////////////////////////////////////
// Member-Funktion CTransferSocket 
void CTransferSocket::OnReceive(int nErrorCode) 
{
	if (GetState() != connected && GetState() != attached && GetState() != closed)
		return;
	if (m_nTransferState == STATE_WAITING)
	{
		m_nNotifyWaiting |= FD_READ;
		return;
	}

	if (m_bSentClose)
		return;
	if (m_bListening)
		return;

	if (m_nMode&CSMODE_LIST)
	{
		if (m_nTransferState == STATE_STARTING)
			OnConnect(0);
		
		char *buffer=new char[BUFSIZE];
		AsyncSelect(FD_CLOSE);
		int numread = CAsyncSocketEx::Receive(buffer,BUFSIZE);
		while (numread!=SOCKET_ERROR && numread)
		{
			m_LastActiveTime = CTime::GetCurrentTime();
			UpdateRecvLed();

			m_pListResult->AddData(buffer,numread);
			m_transferdata.transfersize += numread;
			CTimeSpan timespan = CTime::GetCurrentTime()-m_StartTime;
			int elapsed = (int)timespan.GetTotalSeconds();
			//TODO
			//There are servers which report the total number of 
			//bytes in the list response message, but yet it is not supported by FZ.
			/*double leftmodifier=(transfersize-transferstart-transferleft); 
			leftmodifier*=100;
			leftmodifier/=(transfersize-transferstart);
			if (leftmodifier==0)
				leftmodifier=1;
			double leftmodifier2=100-leftmodifier;
			int left=(int)((elapsed/leftmodifier)*leftmodifier2);
			int percent=MulDiv(100,transfersize-transferleft,transfersize);*/
			int transferrate=static_cast<int>( (elapsed && m_transferdata.transfersize)?m_transferdata.transfersize/elapsed:0 );
			t_ffam_transferstatus *status = new t_ffam_transferstatus;
			status->bFileTransfer = FALSE;
			status->bytes = m_transferdata.transfersize;
			status->percent = -1;
			status->timeelapsed = elapsed;
			status->timeleft = -1;
			status->transferrate = transferrate;
			PostMessage(m_pOwner->m_pOwner->m_hOwnerWnd, m_pOwner->m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_TRANSFERSTATUS, 0), (LPARAM)status);
			
			//Check if there are other commands in the command queue.
			MSG msg;
			if (PeekMessage(&msg, 0, m_nInternalMessageID, m_nInternalMessageID, PM_NOREMOVE) && GetState() != closed)
			{
				LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("Message waiting in queue, resuming later"));
				AsyncSelect(FD_READ | FD_CLOSE);
				if (IsLayerAttached())
					TriggerEvent(FD_READ);
				return;
			}	
			
			buffer = new char[BUFSIZE];
			numread = CAsyncSocketEx::Receive(buffer, BUFSIZE);
		}
		delete [] buffer;
		if (!numread)
		{
			Close();
			if (!m_bSentClose)
			{
				m_bSentClose = TRUE;
				m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
			}
		}
		if (numread==SOCKET_ERROR)
		{
			int nError = GetLastError();
			if (nError==WSAENOTCONN)
			{
				//Not yet connected
				AsyncSelect(FD_READ | FD_CLOSE);
				return;
			}
			if (nError!=WSAEWOULDBLOCK)
			{
				Close();
				if (!m_bSentClose)
				{
					m_nMode |= CSMODE_TRANSFERERROR;
					m_bSentClose = TRUE;
					m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
				}
			}
		}
		AsyncSelect(FD_READ | FD_CLOSE);
	}
	else if (m_nMode&CSMODE_DOWNLOAD)
	{
		if (m_nTransferState == STATE_STARTING)
			OnConnect(0);
		
		AsyncSelect(FD_CLOSE);

		bool beenWaiting;
		_int64 ableToRead;
		if (GetState() != closed)
			ableToRead = m_pOwner->GetAbleToDownloadSize(beenWaiting);
		else
			ableToRead = BUFSIZE;

		if (!m_pBuffer)
			m_pBuffer = new char[BUFSIZE];

		int numread = CAsyncSocketEx::Receive(m_pBuffer, static_cast<int>(ableToRead));
		if (numread!=SOCKET_ERROR)
		{
			Transfered( numread, CTime::GetCurrentTime());
			m_pOwner->SpeedLimitAddDownloadedBytes(numread);
		}

		int loopcount=0;
		while (numread!=SOCKET_ERROR && numread)
		{
			loopcount++;
			m_LastActiveTime = CTime::GetCurrentTime();
			UpdateRecvLed();
			TRY
			{
				m_pFile->Write(m_pBuffer, numread);
			}
			CATCH(CFileException,e)
			{
				LPTSTR msg = new TCHAR[BUFSIZE];
				if (e->GetErrorMessage(msg, BUFSIZE))
					m_pOwner->ShowStatus(msg, 1);
				delete [] msg;
				Close();
				if (!m_bSentClose)
				{
					m_nMode |= CSMODE_TRANSFERERROR;
					m_bSentClose = TRUE;
					m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
				}
			}
			END_CATCH;
			m_transferdata.transferleft -= numread;

			//Check if there are other commands in the command queue.
			MSG msg;
			if (PeekMessage(&msg, 0, m_nInternalMessageID, m_nInternalMessageID, PM_NOREMOVE) && GetState() != closed)
			{
				LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("Message waiting in queue, resuming later"));
				AsyncSelect(FD_READ | FD_CLOSE);
				if (IsLayerAttached())
					TriggerEvent(FD_READ);
				
				UpdateStatusBar(false);
			
				return;
			}
			
			UpdateStatusBar(false);

			if (GetState() != closed)
				ableToRead = m_pOwner->GetAbleToDownloadSize(beenWaiting);
			else
				ableToRead = BUFSIZE;
			numread = CAsyncSocketEx::Receive(m_pBuffer, static_cast<int>(ableToRead));
			if (numread!=SOCKET_ERROR)
			{
				Transfered( numread, CTime::GetCurrentTime());
				m_pOwner->SpeedLimitAddDownloadedBytes(numread);
			}
		}
			
		if (!numread)
		{
			Close();
			if (!m_bSentClose)
			{
				m_bSentClose = TRUE;
				m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
			}
		}
		
		if (numread == SOCKET_ERROR)
		{
			int nError=GetLastError();
			if (nError==WSAENOTCONN)
			{
				//Not yet connected
				AsyncSelect(FD_READ | FD_CLOSE);
				return;
			}
			if (nError!=WSAEWOULDBLOCK)
			{
				Close();
				if (!m_bSentClose)
				{
					m_nMode |= CSMODE_TRANSFERERROR;
					m_bSentClose = TRUE;
					m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
				}
			}

			UpdateStatusBar(false);
		}
		else
			UpdateStatusBar(false);
		
		AsyncSelect(FD_READ | FD_CLOSE);
	}
}

void CTransferSocket::OnAccept(int nErrorCode) 
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnAccept(%d)"), nErrorCode);
	m_bListening=FALSE;
	CAsyncSocketEx tmp;
	Accept(tmp);
	SOCKET socket=tmp.Detach();
	CAsyncSocketEx::Close();
	
	Attach(socket);

	/* Set internal socket send buffer to twice the programs buffer size
	 * this should fix the speed problems some users have reported
	 */
	DWORD value;
	int len = sizeof(value);
	GetSockOpt(SO_SNDBUF, &value, &len);
	if (value < (BUFSIZE*2))
	{
		value = BUFSIZE * 2;
		SetSockOpt(SO_SNDBUF, &value, sizeof(value));
	}

	if (m_nTransferState == STATE_STARTING)
	{
		m_nTransferState = STATE_STARTED;	
		
		if (m_pSslLayer)
		{
			AddLayer(m_pSslLayer);
			m_pSslLayer->InitClientSSL();
		}
		
		if (m_pGssLayer)
		{
			AddLayer(m_pGssLayer);
		}
		
		m_TransferedFirst = m_StartTime = CTime::GetCurrentTime();
		m_LastActiveTime=CTime::GetCurrentTime();
	}
}

void CTransferSocket::OnConnect(int nErrorCode) 
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("OnConnect(%d)"), nErrorCode);
	if (nErrorCode)
	{
		TCHAR buffer[1000];
		memset(buffer,0,1000);
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, nErrorCode, 0, buffer, 999, 0);
		CString str;
		str.Format(IDS_ERRORMSG_CANTOPENTRANSFERCHANNEL,buffer);
		str.Replace( _T("\n"), _T("\0") );
		str.Replace( _T("\r"), _T("\0") );
		m_pOwner->ShowStatus(str,1);
		Close();
		if (!m_bSentClose)
		{
			m_nMode|=CSMODE_TRANSFERERROR;
			m_bSentClose=TRUE;
			m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
		}	
	}
	else
	{
		/* Set internal socket send buffer to twice the programs buffer size
		 * this should fix the speed problems some users have reported
		 */
		DWORD value;
		int len = sizeof(value);
		GetSockOpt(SO_SNDBUF, &value, &len);
		if (value < (BUFSIZE*2))
		{
			value = BUFSIZE * 2;
			SetSockOpt(SO_SNDBUF, &value, sizeof(value));
		}
	}
	if (m_nTransferState == STATE_STARTING)
	{
		m_nTransferState = STATE_STARTED;
		
		m_TransferedFirst = m_StartTime = CTime::GetCurrentTime();
		m_LastActiveTime=CTime::GetCurrentTime();
		
		if (m_pSslLayer)
		{
			AddLayer(m_pSslLayer);
			m_pSslLayer->InitClientSSL();
		}
		
		if (m_pGssLayer)
		{
			AddLayer(m_pGssLayer);
		}
	}
}	


void CTransferSocket::OnClose(int nErrorCode) 
{
	LogMessage(__FILE__, __LINE__, this, FZ_LOG_DEBUG, _T("OnClose(%d)"), nErrorCode);

	if (m_nTransferState == STATE_WAITING)
	{
		m_nNotifyWaiting |= FD_CLOSE;
		return;
	}

	OnReceive(0);
	Close();
	if (!m_bSentClose)
	{
		m_bSentClose=TRUE;
		m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
	}
}

int CTransferSocket::CheckForTimeout(int delay)
{
	UpdateStatusBar(false);
	if (!m_bCheckTimeout)
		return 1;
	CTimeSpan span = CTime::GetCurrentTime()-m_LastActiveTime;
	if (span.GetTotalSeconds()>=delay)
	{
		m_pOwner->ShowStatus(IDS_ERRORMSG_TIMEOUT, 1);
		Close();
		if (!m_bSentClose)
		{
			m_nMode |= CSMODE_TRANSFERTIMEOUT;
			m_bSentClose = TRUE;
			VERIFY(m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode));
		}
		return 2;
	}
	return 1;
}

void CTransferSocket::SetActive()
{
	LogMessage(__FILE__, __LINE__, this, FZ_LOG_DEBUG, _T("SetActive()"));

	if (m_nTransferState == STATE_WAITING)
		m_nTransferState = STATE_STARTING;
	m_bCheckTimeout = TRUE;
	m_LastActiveTime = CTime::GetCurrentTime();

	if (m_nNotifyWaiting & FD_READ)
		OnReceive(0);
	if (m_nNotifyWaiting & FD_WRITE)
		OnSend(0);
	if (m_nNotifyWaiting & FD_CLOSE)
		OnClose(0);
}

void CTransferSocket::OnSend(int nErrorCode) 
{
	if (m_nTransferState == STATE_WAITING)
	{
		m_nNotifyWaiting |= FD_WRITE;
		return;
	}

	if (m_bSentClose)
		return;
	if (m_bListening)
		return;
	
	if (m_nMode&CSMODE_UPLOAD && !(m_nMode&CSMODE_LIST))
	{
		if (!m_pFile)
			return;
		if (m_nTransferState == STATE_STARTING)
			OnConnect(0);

		if (m_pSslLayer && m_bShutDown)
			return;
		
		if (!m_pBuffer)
			m_pBuffer = new char[BUFSIZE];
		int numread;
		TRY
		{
			bool beenWaiting = false;
			_int64 currentBufferSize;
			if (GetState() != closed)
				currentBufferSize = m_pOwner->GetAbleToUploadSize(beenWaiting);
			else
				currentBufferSize = BUFSIZE;

			if (m_bufferpos<currentBufferSize)
			{
				if (!m_transferdata.bType)
					numread=m_pFile->Read(m_pBuffer+m_bufferpos, static_cast<int>(currentBufferSize-m_bufferpos));
				else
				{   //Read the file as ASCII file with CRLF line end
					if (!m_ReadBuffer)
						m_ReadBuffer=new char[BUFSIZE];
					if (!m_ReadSize)
						m_ReadSize = m_pFile->Read(m_ReadBuffer, static_cast<int>(currentBufferSize));
					numread = 0;
					while (numread<(currentBufferSize-m_bufferpos))
					{
						if (m_ReadPos>=m_ReadSize)
						{
							if (m_ReadPos!=currentBufferSize)
								break;
							else
							{
								m_ReadSize = m_pFile->Read(m_ReadBuffer, static_cast<int>(currentBufferSize));
								m_ReadPos=0;
								continue;
							}
						}
						if (m_ReadBuffer[m_ReadPos]=='\n' && m_cLastChar!='\r')
						{
							m_pBuffer[m_bufferpos+numread++]='\r';
						}
						if (numread!=currentBufferSize)
						{
							m_pBuffer[m_bufferpos+numread]=m_ReadBuffer[m_ReadPos];
							m_cLastChar=m_pBuffer[m_bufferpos+numread];
							numread++;
							m_ReadPos++;
						}
						else
							m_cLastChar='\r';
					}
					
				}
			}
			else
				numread=0;
			
			ASSERT((numread+m_bufferpos)<=BUFSIZE);
			ASSERT(numread>=0);
			ASSERT(m_bufferpos>=0);

			if (numread+m_bufferpos <= 0)
			{
				if (ShutDown() || GetLastError()!=WSAEWOULDBLOCK)
				{
					Close();
					if (!m_bSentClose)
					{
						m_bSentClose=TRUE;
						m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
					}
				}
				return;
			}
			
			int numsent = Send(m_pBuffer, numread+m_bufferpos);
			if (numsent!=SOCKET_ERROR)
			{
				Transfered( numsent, CTime::GetCurrentTime());
				m_pOwner->SpeedLimitAddUploadedBytes(numsent);
			}
			
			int nLoopCount = 0;
			while (TRUE)
			{
				if (numsent != SOCKET_ERROR)
				{
					m_LastActiveTime = CTime::GetCurrentTime();
					UpdateSendLed();
					m_transferdata.transferleft -= numsent;
				}					
				
			/*	if (numsent!=SOCKET_ERROR && numsent!=(numread+m_bufferpos))
				{
					int pos=numread+m_bufferpos-numsent;
					char *tmp=new char[pos];
					memcpy(tmp,m_pBuffer+numsent,pos);
					memcpy(m_pBuffer,tmp,pos);
					m_bufferpos=pos;
					delete [] tmp;
					if (!m_pOwner->SpeedLimitGetUploadBytesAvailable())
						beenWaiting = true;
					else
						/* Give control back to the system. Else FileZilla would 
						 * consume most CPU power until the data was sent.
						 * This behaviour is very strange, and I can't explain it,
						 * maybe it's just another bug in Windows
						 */
						/*Sleep(0);
					if ((nLoopCount%100) || beenWaiting)
						UpdateStatusBar(false);
					return;
				}*/
				if (numsent==SOCKET_ERROR || !numsent)
				{
					/* Give control back to the system. Else FileZilla would 
					 * consume most CPU power until the data was sent.
					 * This behaviour is very strange, and I can't explain it,
					 * maybe it's just another bug in Windows
					 */
					Sleep(0);
								
					int nError = GetLastError();
					if (nError==WSAENOTCONN)
					{
						//Not yet connected
						m_bufferpos += numread;
						return;
					}
					if (nError==WSAEWOULDBLOCK)
						m_bufferpos += numread;
					else
					{
						if (ShutDown() || GetLastError()!=WSAEWOULDBLOCK)
						{
							Close();
							if (!m_bSentClose)
							{
								m_nMode |= CSMODE_TRANSFERERROR;
								m_bSentClose = TRUE;
								m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
							}
						}
					}
					UpdateStatusBar(false);
					return;
				}
				else
				{
					int pos = numread + m_bufferpos - numsent;

					if (!pos && numread<(currentBufferSize-m_bufferpos) && m_bufferpos!=currentBufferSize)
					{
						if (ShutDown() || GetLastError()!=WSAEWOULDBLOCK)
						{
							Close();
							if (!m_bSentClose)
							{
								m_bSentClose=TRUE;
								m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
							}
						}
						return;
					}
									
					if (!pos)
						m_bufferpos = 0;
					else
					{
						memmove(m_pBuffer, m_pBuffer+numsent, pos);
						m_bufferpos=pos;
					}
				}
				//Check if there are other commands in the command queue.
				MSG msg;
				if (PeekMessage(&msg, 0, m_nInternalMessageID, m_nInternalMessageID, PM_NOREMOVE))
				{
					//Send resume message
					LogMessage(__FILE__, __LINE__, this, FZ_LOG_DEBUG, _T("Message waiting in queue, resuming later"));
					TriggerEvent(FD_WRITE);
					UpdateStatusBar(false);
					return;
				}
				nLoopCount++;
				UpdateStatusBar(false);
				
				if (GetState() != closed)
					currentBufferSize = m_pOwner->GetAbleToUploadSize( beenWaiting);
				else
					currentBufferSize = BUFSIZE;

				if (m_bufferpos<currentBufferSize)
				{
					if (!m_transferdata.bType)
					{
						numread=m_pFile->Read(m_pBuffer+m_bufferpos, static_cast<int>(currentBufferSize-m_bufferpos));
						ASSERT((numread+m_bufferpos)<=currentBufferSize);
					}
					else
					{   //Read the file as ASCII file with CRLF line end
						if (!m_ReadBuffer)
							m_ReadBuffer=new char[BUFSIZE];
						if (!m_ReadSize)
							m_ReadSize=m_pFile->Read(m_ReadBuffer, static_cast<int>(currentBufferSize));
						numread=0;
						while (numread<(currentBufferSize-m_bufferpos))
						{
							if (m_ReadPos>=m_ReadSize)
							{
								if (m_ReadPos!=currentBufferSize)
									break;
								else
								{
									m_ReadSize = m_pFile->Read(m_ReadBuffer, static_cast<int>(currentBufferSize));
									m_ReadPos=0;
									continue;
								}
							}
							if (m_ReadBuffer[m_ReadPos]=='\n' && m_cLastChar!='\r')
							{
								m_pBuffer[m_bufferpos+numread++]='\r';
							}
							if (numread!=currentBufferSize)
							{
								m_pBuffer[m_bufferpos+numread]=m_ReadBuffer[m_ReadPos];
								m_cLastChar=m_pBuffer[m_bufferpos+numread];
								numread++;
								m_ReadPos++;
							}
							else
								m_cLastChar='\r';
						}
					}
				}
				else 
					numread = 0;
				ASSERT(numread>=0);
				ASSERT(m_bufferpos>=0);
				numsent = Send(m_pBuffer, numread+m_bufferpos);	
				if (numsent!=SOCKET_ERROR)
				{
					Transfered(numsent, CTime::GetCurrentTime());
					m_pOwner->SpeedLimitAddUploadedBytes(numsent);
				}
			}
		}
		CATCH_ALL(e)
		{
			TCHAR error[BUFSIZE];
			if (e->GetErrorMessage(error, BUFSIZE))
				m_pOwner->ShowStatus(error,1);
			if (ShutDown() || GetLastError() != WSAEWOULDBLOCK)
			{
				Close();
				if (!m_bSentClose)
				{
					m_nMode |= CSMODE_TRANSFERERROR;
					m_bSentClose = TRUE;
					m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
				}
			}
		}
		END_CATCH_ALL;
	}
}

void CTransferSocket::UpdateStatusBar(bool forceUpdate)
{
	if (m_nTransferState != STATE_STARTED)
		return;

	if (!forceUpdate)
	{
		//Don't flood the main window with messages
		//Else performance would be really low
		LARGE_INTEGER curtime;
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&curtime);
		if (((curtime.QuadPart-m_LastUpdateTime.QuadPart) < (freq.QuadPart/15) ) )
			return;
		m_LastUpdateTime = curtime;
	}
	
	//Update the statusbar
	CTimeSpan timespan=CTime::GetCurrentTime()-m_StartTime;
	int elapsed=(int)timespan.GetTotalSeconds();

	t_ffam_transferstatus *status=new t_ffam_transferstatus;
	status->bFileTransfer=TRUE;
	status->timeelapsed=elapsed;
	status->bytes=m_transferdata.transfersize-m_transferdata.transferleft;
	if (m_transferdata.transfersize>0 && !(m_nMode&CSMODE_LIST))
	{
		double leftmodifier=static_cast<double>(m_transferdata.transfersize-m_transferdata.nTransferStart-m_transferdata.transferleft);
		leftmodifier*=100;
		if (m_transferdata.transfersize-m_transferdata.nTransferStart)
			leftmodifier /= (m_transferdata.transfersize-m_transferdata.nTransferStart);
		else
			leftmodifier = 1;
		if (leftmodifier == 0)
			leftmodifier = 1;
		double leftmodifier2 = 100 - leftmodifier;
		int left=static_cast<int>((elapsed/leftmodifier)*leftmodifier2);
		double percent=100*static_cast<double>(m_transferdata.transfersize-m_transferdata.transferleft);
		percent/=m_transferdata.transfersize;
		status->percent=static_cast<int>(percent);
		if (status->percent>100)
			status->percent=100;
		
		if (left < 0)
			left = -1;
		status->timeleft=left;		
	}
	else
	{
		status->percent=-1;
		status->timeleft=-1;
	}

	int count = 0;
	status->transferrate = 0;

	for ( int i = 0; i < SPEED_SECONDS; i++)
	{
		if ( m_UsedForTransfer[ i])
		{
			status->transferrate += m_Transfered[ i];

			count++;
		}
	}

	if ( count > 0)
		status->transferrate = status->transferrate / count;
	else if (m_Transfered[0])
		status->transferrate = m_Transfered[0];
	else
		status->timeleft=-1;
		
	PostMessage(m_pOwner->m_pOwner->m_hOwnerWnd, m_pOwner->m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_TRANSFERSTATUS, 0), (LPARAM)status);
}

void CTransferSocket::UpdateSendLed()
{
	//Don't flood the main window with messages
	//Else performance would be really low
	LARGE_INTEGER curtime;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&curtime);
	static LARGE_INTEGER oldtime={0};
	if ( ( (curtime.QuadPart-oldtime.QuadPart) < (freq.QuadPart/15) ) )
		return;
	oldtime=curtime;
	
	PostMessage(m_pOwner->m_pOwner->m_hOwnerWnd, m_pOwner->m_pOwner->m_nReplyMessageID,	FZ_MSG_MAKEMSG(FZ_MSG_SOCKETSTATUS, FZ_SOCKETSTATUS_SEND), 0);
}

void CTransferSocket::UpdateRecvLed()
{
	//Don't flood the main window with messages
	//Else performance would be really low
	LARGE_INTEGER curtime;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&curtime);
	static LARGE_INTEGER oldtime={0};
	if ( ( (curtime.QuadPart-oldtime.QuadPart) < (freq.QuadPart/15) ) )
		return;
	oldtime=curtime;
	
	PostMessage(m_pOwner->m_pOwner->m_hOwnerWnd, m_pOwner->m_pOwner->m_nReplyMessageID, FZ_MSG_MAKEMSG(FZ_MSG_SOCKETSTATUS, FZ_SOCKETSTATUS_RECV), 0);
}

BOOL CTransferSocket::Create(BOOL bUseSsl)
{
	if (bUseSsl)
		m_pSslLayer = new CAsyncSslSocketLayer;

	if (!m_pOwner->m_CurrentServer.fwbypass)
	{
		int nProxyType = COptions::GetOptionVal(OPTION_PROXYTYPE);
		if (nProxyType != PROXYTYPE_NOPROXY)
		{
			USES_CONVERSION;
			
			m_pProxyLayer = new CAsyncProxySocketLayer;
			if (nProxyType == PROXYTYPE_SOCKS4)
				m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS4, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType == PROXYTYPE_SOCKS4A)
				m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS4A, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType == PROXYTYPE_SOCKS5)
				if (COptions::GetOptionVal(OPTION_PROXYUSELOGON))
					m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS5, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT),
											T2CA(COptions::GetOption(OPTION_PROXYUSER)),
											T2CA(CCrypt::decrypt(COptions::GetOption(OPTION_PROXYPASS))));
				else
					m_pProxyLayer->SetProxy(PROXYTYPE_SOCKS5, T2CA(COptions::GetOption(OPTION_PROXYHOST)),
											COptions::GetOptionVal(OPTION_PROXYPORT));
			else if (nProxyType == PROXYTYPE_HTTP11)
				if (COptions::GetOptionVal(OPTION_PROXYUSELOGON))
					m_pProxyLayer->SetProxy(PROXYTYPE_HTTP11, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT),
											T2CA(COptions::GetOption(OPTION_PROXYUSER)),
											T2CA(CCrypt::decrypt(COptions::GetOption(OPTION_PROXYPASS))));
				else
					m_pProxyLayer->SetProxy(PROXYTYPE_HTTP11, T2CA(COptions::GetOption(OPTION_PROXYHOST)), COptions::GetOptionVal(OPTION_PROXYPORT));
			else
				ASSERT(FALSE);
			AddLayer(m_pProxyLayer);
		}
	}

	if (!COptions::GetOptionVal(OPTION_LIMITPORTRANGE))
	{
		if (!CAsyncSocketEx::Create())
			return FALSE;

		return TRUE;
	}
	else
	{
		int min=COptions::GetOptionVal(OPTION_PORTRANGELOW);
		int max=COptions::GetOptionVal(OPTION_PORTRANGEHIGH);
		if (min>=max)
		{
			m_pOwner->ShowStatus(IDS_ERRORMSG_CANTCREATEDUETOPORTRANGE,1);
			return FALSE;
		}
		int startport=static_cast<int>(min+((double)rand()*(max-min))/(RAND_MAX+1));
		int port=startport;
		while (!CAsyncSocketEx::Create(port))
		{
			port++;
			if (port>max)
				port=min;
			if (port==startport)
			{
				m_pOwner->ShowStatus(IDS_ERRORMSG_CANTCREATEDUETOPORTRANGE,1);
				return FALSE;
			}
		}
	}
	
	return TRUE;
}

void CTransferSocket::Close()
{
	LogMessage(__FILE__, __LINE__, this,FZ_LOG_DEBUG, _T("Close()"));
	m_bCheckTimeout = FALSE;
	CAsyncSocketEx::Close();
}

int CTransferSocket::OnLayerCallback(const CAsyncSocketExLayer *pLayer, int nType, int nParam1, int nParam2)
{
	ASSERT(pLayer);
	if (nType==LAYERCALLBACK_STATECHANGE)
	{
		if (pLayer==m_pProxyLayer)
			LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("m_pProxyLayer changed state from %d to %d"), nParam2, nParam1);
		else if (pLayer==m_pSslLayer)
			LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("m_pSslLayer changed state from %d to %d"), nParam2, nParam1);
		else if (pLayer==m_pGssLayer)
			LogMessage(__FILE__, __LINE__, this, FZ_LOG_INFO, _T("m_pGssLayer changed state from %d to %d"), nParam2, nParam1);
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
				m_pOwner->ShowStatus(IDS_ERRORMSG_PROXY_NOCONN, 1);
				break;
			case PROXYERROR_REQUESTFAILED:
				m_pOwner->ShowStatus(IDS_ERRORMSG_PROXY_REQUESTFAILED, 1);
				break;
			case PROXYERROR_AUTHTYPEUNKNOWN:
				m_pOwner->ShowStatus(IDS_ERRORMSG_PROXY_AUTHTYPEUNKNOWN, 1);
				break;
			case PROXYERROR_AUTHFAILED:
				m_pOwner->ShowStatus(IDS_ERRORMSG_PROXY_AUTHFAILED, 1);
				break;
			case PROXYERROR_AUTHNOLOGON:
				m_pOwner->ShowStatus(IDS_ERRORMSG_PROXY_AUTHNOLOGON, 1);
				break;
			case PROXYERROR_CANTRESOLVEHOST:
				m_pOwner->ShowStatus(IDS_ERRORMSG_PROXY_CANTRESOLVEHOST, 1);
				break;
			default:
				LogMessage(__FILE__, __LINE__, this,FZ_LOG_WARNING, _T("Unknown proxy error") );
			}
		}
		else if (pLayer == m_pSslLayer)
		{
			switch (nParam1)
			{
				case SSL_INFO:
					switch(nParam2)
					{
						case SSL_INFO_SHUTDOWNCOMPLETE:
							Close();
							if (!m_bSentClose)
							{
								m_bSentClose=TRUE;
								m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
							}
							break;
						case SSL_INFO_ESTABLISHED:
							m_pOwner->ShowStatus(IDS_STATUSMSG_SSLESTABLISHEDTRANSFER, 0);
							TriggerEvent(FD_FORCEREAD);
							break;
					}
					break;
				case SSL_FAILURE:
					switch (nParam2)
					{
						case SSL_FAILURE_ESTABLISH:
							m_pOwner->ShowStatus(IDS_ERRORMSG_CANTESTABLISHSSLCONNECTION, 1);
							break;
						case SSL_FAILURE_LOADDLLS:
							m_pOwner->ShowStatus(IDS_ERRORMSG_CANTLOADSSLDLLS, 1);
							break;
						case SSL_FAILURE_INITSSL:
							m_pOwner->ShowStatus(IDS_ERRORMSG_CANTINITSSL, 1);
							break;
					}
					if (!m_bSentClose)
					{
						m_nMode|=CSMODE_TRANSFERERROR;
						m_bSentClose = TRUE;
						m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
					}
					break;
			}
		}
		else if (pLayer == m_pGssLayer)
		{
			switch(nParam1)
			{
			case GSS_INFO:
				LogMessage(FZ_LOG_INFO, (LPCTSTR)nParam2);
				break;
			case GSS_ERROR:
				LogMessage(FZ_LOG_APIERROR, (LPCTSTR)nParam2);
				break;
			case GSS_SHUTDOWN_COMPLETE:
				Close();
				if (!m_bSentClose)
				{
					m_bSentClose=TRUE;
					m_pOwner->m_pOwner->PostThreadMessage(m_nInternalMessageID, FZAPI_THREADMSG_TRANSFEREND, m_nMode);
				}
				break;
			}
		}
	}

	return 1;
}

void CTransferSocket::Transfered(int count, CTime time)
{
	CTimeSpan ts = time - m_TransferedFirst;
	int diff = ts.GetTotalSeconds();
	if (diff < 0)
		diff = 0;
	
	if ( diff >= SPEED_SECONDS)
	{
		int move = diff - SPEED_SECONDS + 1;
		int start = SPEED_SECONDS - move;

		if ( start <= 0)
			start = 0;
		else
		{
			for ( int i = 0; i < SPEED_SECONDS - move; i++)
			{
				m_Transfered[ i] = m_Transfered[ i + move];
				m_UsedForTransfer[ i] = m_UsedForTransfer[ i + move];
			}
		}

		for ( int i = start; i < SPEED_SECONDS; i++)
		{
			m_Transfered[ i] = 0;
			m_UsedForTransfer[ i] = false;
		}

		if (move >= SPEED_SECONDS)
		{
			m_TransferedFirst = time;
			diff = 0;
		}
		else
		{
			m_TransferedFirst += CTimeSpan( move);
			ts = time - m_TransferedFirst;
			diff = ts.GetTotalSeconds() % 60;
		}
	}

	m_Transfered[ diff] += count;

	for ( int i = 0; i < diff - 1; i++)
		m_UsedForTransfer[ i] = true;
}

void CTransferSocket::UseGSS(CAsyncGssSocketLayer *pGssLayer)
{
	m_pGssLayer = new CAsyncGssSocketLayer;
	m_pGssLayer->InitTransferChannel(pGssLayer);
}