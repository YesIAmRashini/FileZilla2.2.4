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

// Crypt.cpp: Implementierung der Klasse CCrypt.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "crypt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

char* CCrypt::m_key = "FILEZILLA1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";

CString CCrypt::encrypt(CString str)
{
	USES_CONVERSION;
	int pos=str.GetLength()%strlen(m_key);
	CString ret;
	LPCSTR lpszAscii=T2CA(str);
	for (unsigned int i=0;i<strlen(lpszAscii);i++)
	{
		CString tmp=ret;
		ret.Format(_T("%s%03d"),tmp,(unsigned char)lpszAscii[i]^m_key[(i+pos)%strlen(m_key)]);
	}
	return ret;
}

CString CCrypt::decrypt(CString str)
{
	USES_CONVERSION;

	LPCSTR lpszAscii=T2CA(str);
	int pos=(strlen(lpszAscii)/3)%strlen(m_key);
	CString ret;
	char buffer[4];
	buffer[3]=0;
	for (unsigned int i=0;i<strlen(lpszAscii)/3;i++)
	{
		memcpy(buffer,lpszAscii+i*3,3);
		TCHAR tmp[2];
		tmp[1]=0;
		tmp[0]=atoi(buffer)^m_key[(i+pos)%strlen(m_key)];
		ret+=tmp;
	}
	return ret;
}
