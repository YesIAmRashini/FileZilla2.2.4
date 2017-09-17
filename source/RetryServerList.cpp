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

// RetryServerList.cpp: Implementierung der Klasse CRetryServerList.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RetryServerList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CRetryServerList::CRetryServerList()
{
}

CRetryServerList::~CRetryServerList()
{
}

void CRetryServerList::AddServer(const t_server &server)
{
	for (std::list<t_retryserver>::iterator iter=m_List.begin(); iter!=m_List.end(); iter++)
	{
		if (iter->server==server)
		{
			iter->time=CTime::GetCurrentTime();
			return;
		}
		CTimeSpan span=CTime::GetCurrentTime()-iter->time;
		if (span.GetTotalSeconds()>COptions::GetOptionVal(OPTION_RETRYDELAY))
		{
			std::list<t_retryserver>::iterator iter2=iter;
			if (m_List.size()!=1)
				iter--;
			m_List.erase(iter2);
			if (!m_List.size())
				break;
		}
	}
	t_retryserver retryserver;
	retryserver.server=server;
	retryserver.time=CTime::GetCurrentTime();
	m_List.push_back(retryserver);
}

BOOL CRetryServerList::StillWait(const t_server &server)
{
	for (std::list<t_retryserver>::iterator iter=m_List.begin(); iter!=m_List.end(); iter++)
	{
		if (iter->server==server)
		{
			CTimeSpan span=CTime::GetCurrentTime()-iter->time;
			if (span.GetTotalSeconds()>COptions::GetOptionVal(OPTION_RETRYDELAY))
			{
				std::list<t_retryserver>::iterator iter2=iter;
				if (m_List.size()!=1)
					iter--;
				m_List.erase(iter2);
				if (!m_List.size())
					break;
				return FALSE;
			}
			else
				return TRUE;
		}
		else
		{
			CTimeSpan span=CTime::GetCurrentTime()-iter->time;
			if (span.GetTotalSeconds()>COptions::GetOptionVal(OPTION_RETRYDELAY))
			{
				std::list<t_retryserver>::iterator iter2=iter;
				if (m_List.size()!=1)
					iter--;
				m_List.erase(iter2);
				if (!m_List.size())
					break;
			}
		}
	}
	return FALSE;
}
