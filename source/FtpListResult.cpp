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
// along with this program; if not, write	 to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

// FtpListResult.cpp: Implementierung der Klasse CFtpListResult.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FtpListResult.h"
#include "FileZillaApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////
//#define LISTDEBUG
#ifdef LISTDEBUG
	//It's the normal UNIX format (or even another nonstandard format)
	//Some samples are from http://cr.yp.to/ftpparse/ftpparse.c
	/* UNIX-style listing, without inum and without blocks */
		
	static char data[][100]={
		"-rw-r--r--   1 root     other        531 Jan 29 03:26 README",
		"dr-xr-xr-x   2 root     other        512 Apr  8  1994 etc",
		"dr-xr-xr-x   2 root                  512 Apr  8  1994 etc2",
		"lrwxrwxrwx   1 root     other          7 Jan 25 00:17 bin -> usr/bin",
		
		/* Some listings with uncommon date/time format: */
		"-rw-r--r--   1 root     other        531 09-26 2000 README2",
		"-rw-r--r--   1 root     other        531 09-26 13:45 README3",
		"-rw-r--r--   1 root     other        531 2005-06-07 21:22 README4",
		
		/* Also produced by Microsoft's FTP servers for Windows: */
		"----------   1 owner    group         1803128 Jul 10 10:18 ls-lR.Z",
		"d---------   1 owner    group               0 May  9 19:45 Softlib",
		
		/* Also WFTPD for MSDOS: */
		"-rwxrwxrwx   1 noone    nogroup      322 Aug 19  1996 message.ftp",
		
		/* Also NetWare: */
		"d [R----F--] supervisor            512       Jan 16 18:53    login",
		"- [R----F--] rhesus             214059       Oct 20 15:27    cx.exe",
		
		/* Also NetPresenz for the Mac: */
		"-------r--         326  1391972  1392298 Nov 22  1995 MegaPhone.sit",
		"drwxrwxr-x               folder        2 May 10  1996 network",

		/* Some other formats some windows servers send */
		"-rw-r--r--   1 root 531 Jan 29 03:26 README5",
		"-rw-r--r--   1 group domain user 531 Jan 29 03:26 README6",

		/* EPLF directory listings */
		"+i8388621.48594,m825718503,r,s280,\teplf test 1.file",
		"+i8388621.50690,m824255907,/,\teplf test 2.dir",
		"+i8388621.48598,m824253270,r,s612,\teplf test 3.file",

		/* MSDOS type listing used by IIS */
		"04-27-00  09:09PM       <DIR>          DOS dir 1",
		"04-14-00  03:47PM                  589 DOS file 1",

		/* Another type of MSDOS style listings */
		"2002-09-02  18:48       <DIR>          DOS dir 2",
		"2002-09-02  19:06                9,730 DOS file 2",

		/* Numerical Unix style format */
		"0100644   500  101   12345    123456789       filename",

		/* This one is used by SSH-2.0-VShell_2_1_2_143, this is the old VShell format */
		"206876  Apr 04, 2000 21:06 VShell (old)",
		"0  Dec 12, 2002 02:13 VShell (old) Dir/",

		/* This type of directory listings is sent by some newer versions of VShell
		 * both year and time in one line is uncommon.
		 */
		"-rwxr-xr-x    1 user group        9 Oct 08, 2002 09:47 VShell (new)",

		/* Next ones come from an OS/2 server. The server obviously isn't Y2K aware */
		"36611      A    04-23-103   10:57  OS2 test1.file",
		" 1123      A    07-14-99   12:37  OS2 test2.file",
		"    0 DIR       02-11-103   16:15  OS2 test1.dir",
		" 1123 DIR  A    10-05-100   23:38  OS2 test2.dir",

		/* Some servers send localized date formats, here the German one: */
		"dr-xr-xr-x   2 root     other      2235 26. Juli, 20:10 datetest1 (ger)",
		"-r-xr-xr-x   2 root     other      2235 2.   Okt.  2003 datetest2 (ger)",
		"-r-xr-xr-x   2 root     other      2235 1999/10/12 17:12 datetest3",
		"-r-xr-xr-x   2 root     other      2235 24-04-2003 17:12 datetest4",

		/* Here a Japanese one: */
		"-rw-r--r--   1 root       sys           8473  4\x8c\x8e 18\x93\xfa 2003\x94\x4e datatest1 (jap)",

		/* VMS style listings */
		"vms_dir_1.DIR;1  1 19-NOV-2001 21:41 [root,root] (RWE,RWE,RE,RE)",

		/* VMS multiline */
		"VMS_file_1;1\r\n170774/170775     24-APR-2003 08:16:15  [FTP_CLIENT,SCOT]      (RWED,RWED,RE,)",
		"VMS_file_2;1\r\n10			     2-JUL-2003 10:30:08.59  [FTP_CLIENT,SCOT]      (RWED,RWED,RE,)",

		/* IBM AS/400 style listing */
		"QSYS            77824 02/23/00 15:09:55 *DIR IBM Dir1/",

		/* aligned directory listing with too long size */
		"-r-xr-xr-x longowner longgroup123456 Feb 12 17:20 long size test1",

		/* short directory listing with month name */
		"-r-xr-xr-x 2 owner group 4512 01-jun-99 shortdate with monthname",

		/* the following format is sent by the Connect:Enterprise server by Sterling Commerce */
		"-C--E-----FTP B BCC3I1       7670  1294495 Jan 13 07:42 ConEnt file",

		""};

#endif

CFtpListResult::CFtpListResult(t_server server)
{
	listhead=curpos=0;
	
	m_server = server;

	pos=0;

	m_prevline=0;
	m_curline=0;
	m_curlistaddpos=0;

	//Fill the month names map

	//English month names
	m_MonthNamesMap[_T("jan")] = 1;
	m_MonthNamesMap[_T("feb")] = 2;
	m_MonthNamesMap[_T("mar")] = 3;
	m_MonthNamesMap[_T("apr")] = 4;
	m_MonthNamesMap[_T("may")] = 5;
	m_MonthNamesMap[_T("jun")] = 6;
	m_MonthNamesMap[_T("june")] = 6;
	m_MonthNamesMap[_T("jul")] = 7;
	m_MonthNamesMap[_T("july")] = 7;
	m_MonthNamesMap[_T("aug")] = 8;
	m_MonthNamesMap[_T("sep")] = 9;
	m_MonthNamesMap[_T("sept")] = 9;
	m_MonthNamesMap[_T("oct")] = 10;
	m_MonthNamesMap[_T("nov")] = 11;
	m_MonthNamesMap[_T("dec")] = 12;

	//Numerical values for the month
	m_MonthNamesMap[_T("1")] = 1;
	m_MonthNamesMap[_T("01")] = 1;
	m_MonthNamesMap[_T("2")] = 2;
	m_MonthNamesMap[_T("02")] = 2;
	m_MonthNamesMap[_T("3")] = 3;
	m_MonthNamesMap[_T("03")] = 3;
	m_MonthNamesMap[_T("4")] = 4;
	m_MonthNamesMap[_T("04")] = 4;
	m_MonthNamesMap[_T("5")] = 5;
	m_MonthNamesMap[_T("05")] = 5;
	m_MonthNamesMap[_T("6")] = 6;
	m_MonthNamesMap[_T("06")] = 6;
	m_MonthNamesMap[_T("7")] = 7;
	m_MonthNamesMap[_T("07")] = 7;
	m_MonthNamesMap[_T("8")] = 8;
	m_MonthNamesMap[_T("08")] = 8;
	m_MonthNamesMap[_T("9")] = 9;
	m_MonthNamesMap[_T("09")] = 9;
	m_MonthNamesMap[_T("10")] = 10;
	m_MonthNamesMap[_T("11")] = 11;
	m_MonthNamesMap[_T("12")] = 12;
	
	//German month names
	m_MonthNamesMap[_T("mrz")] = 3;
	m_MonthNamesMap[_T("mär")] = 3;
	m_MonthNamesMap[_T("märz")] = 3;
	m_MonthNamesMap[_T("mai")] = 5;
	m_MonthNamesMap[_T("juni")] = 5;
	m_MonthNamesMap[_T("juli")] = 6;
	m_MonthNamesMap[_T("okt")] = 10;
	m_MonthNamesMap[_T("dez")] = 12;
	
	//French month names
	m_MonthNamesMap[_T("janv")] = 1;
	m_MonthNamesMap[_T("féb")] = 1;
	m_MonthNamesMap[_T("fév")] = 2;
	m_MonthNamesMap[_T("fev")] = 2;
	m_MonthNamesMap[_T("févr")] = 2;
	m_MonthNamesMap[_T("fevr")] = 2;
	m_MonthNamesMap[_T("mars")] = 3;
	m_MonthNamesMap[_T("avr")] = 4;
	m_MonthNamesMap[_T("juin")] = 7;
	m_MonthNamesMap[_T("juil")] = 7;
	m_MonthNamesMap[_T("aoû"")] = 8;
	m_MonthNamesMap[_T("août")] = 8;
	m_MonthNamesMap[_T("aout")] = 8;
	m_MonthNamesMap[_T("déc")] = 12;
	m_MonthNamesMap[_T("dec")] = 12;
	
	//Italian month names
	m_MonthNamesMap[_T("gen")] = 1;
	m_MonthNamesMap[_T("mag")] = 5;
	m_MonthNamesMap[_T("giu")] = 6;
	m_MonthNamesMap[_T("lug")] = 7;
	m_MonthNamesMap[_T("ago")] = 8;
	m_MonthNamesMap[_T("set")] = 9;
	m_MonthNamesMap[_T("ott")] = 9;
	m_MonthNamesMap[_T("dic")] = 12;

	//Spanish month names
	m_MonthNamesMap[_T("ene")] = 1;
	m_MonthNamesMap[_T("fbro")] = 2;
	m_MonthNamesMap[_T("mzo")] = 3;
	m_MonthNamesMap[_T("ab")] = 4;
	m_MonthNamesMap[_T("abr")] = 4;
	m_MonthNamesMap[_T("agto")] = 8;
	m_MonthNamesMap[_T("sbre")] = 9;
	m_MonthNamesMap[_T("obre")] = 9;
	m_MonthNamesMap[_T("nbre")] = 9;
	m_MonthNamesMap[_T("dbre")] = 9;

	//Polish month names
	m_MonthNamesMap[_T("sty")] = 1;
	m_MonthNamesMap[_T("lut")] = 2;
	m_MonthNamesMap[_T("kwi")] = 4;
	m_MonthNamesMap[_T("maj")] = 5;
	m_MonthNamesMap[_T("cze")] = 6;
	m_MonthNamesMap[_T("lip")] = 7;
	m_MonthNamesMap[_T("sie")] = 8;
	m_MonthNamesMap[_T("wrz")] = 9;
	m_MonthNamesMap[_T("paŸ"")] = 10;
	m_MonthNamesMap[_T("lis")] = 11;
	m_MonthNamesMap[_T("gru")] = 12;

	//Russian month names
	m_MonthNamesMap[_T("íâ?")] = 1;
	m_MonthNamesMap[_T("ôåâ"")] = 2;
	m_MonthNamesMap[_T("ìàð"")] = 3;
	m_MonthNamesMap[_T("àïð"")] = 4;
	m_MonthNamesMap[_T("ìàé"")] = 5;
	m_MonthNamesMap[_T("èþí"")] = 6;
	m_MonthNamesMap[_T("èþë"")] = 7;
	m_MonthNamesMap[_T("àâã"")] = 8;
	m_MonthNamesMap[_T("ñåí"")] = 9;
	m_MonthNamesMap[_T("îêò"")] = 10;
	m_MonthNamesMap[_T("íî?")] = 11;
	m_MonthNamesMap[_T("äåê"")] = 12;

	//Dutch month names
	m_MonthNamesMap[_T("mrt")] = 3;
	m_MonthNamesMap[_T("mei")] = 5;

	//Portuguese month names
	m_MonthNamesMap[_T("out")] = 10;

	//Japanese month names
	/*
	m_MonthNamesMap[_T("1\x8c\x8e")] = 1;
	m_MonthNamesMap[_T("2\x8c\x8e")] = 2;
	m_MonthNamesMap[_T("3\x8c\x8e")] = 3;
	m_MonthNamesMap[_T("4\x8c\x8e")] = 4;
	m_MonthNamesMap[_T("5\x8c\x8e")] = 5;
	m_MonthNamesMap[_T("6\x8c\x8e")] = 6;
	m_MonthNamesMap[_T("7\x8c\x8e")] = 7;
	m_MonthNamesMap[_T("8\x8c\x8e")] = 8;
	m_MonthNamesMap[_T("9\x8c\x8e")] = 9;
	m_MonthNamesMap[_T("10\x8c\x8e")] = 10;
	m_MonthNamesMap[_T("11\x8c\x8e")] = 11;
	m_MonthNamesMap[_T("12\x8c\x8e")] = 12;
*/
	//There are more languages and thus month 
	//names, but as long knowbody reports a 
	//problem, I won't add them, there are way 
	//too much languages

	//Some very strange combinations of names and numbers I've seen. 
	//The developers of those ftp servers must have been dumb.
	m_MonthNamesMap[_T("jan1")] = 1;
	m_MonthNamesMap[_T("feb2")] = 2;
	m_MonthNamesMap[_T("mar3")] = 3;
	m_MonthNamesMap[_T("apr4")] = 4;
	m_MonthNamesMap[_T("may5")] = 5;
	m_MonthNamesMap[_T("jun6")] = 6;
	m_MonthNamesMap[_T("jul7")] = 7;
	m_MonthNamesMap[_T("aug8")] = 8;
	m_MonthNamesMap[_T("sep9")] = 9;
	m_MonthNamesMap[_T("sept9")] = 9;
	m_MonthNamesMap[_T("oct0")] = 10;
	m_MonthNamesMap[_T("nov1")] = 11;
	m_MonthNamesMap[_T("dec2")] = 12;

	
#ifdef LISTDEBUG
	int i=-1;
	while (*data[++i])
	{
		char *pData=new char[strlen(data[i])+3];
		sprintf(pData, "%s\r\n", data[i]);
		AddData(pData, strlen(pData));
	}
	TRACE1("%d lines added\n", i);
#endif
}


CFtpListResult::~CFtpListResult()
{
	t_list *ptr=listhead;
	t_list *ptr2;
	while (ptr)
	{
		delete [] ptr->buffer;
		ptr2=ptr;
		ptr=ptr->next;
		delete ptr2;
	}
	if (m_prevline)
		delete [] m_prevline;
	if (m_curline)
		delete [] m_curline;
}

t_directory::t_direntry *CFtpListResult::getList(int &num, CTime EntryTime)
{
	t_directory::t_direntry direntry;
	char *line=GetLine();
	m_curline=line;
	while (line)
	{
		int tmp;
		char *tmpline=new char[strlen(line)+1];
		strcpy(tmpline, line);
		if (parseLine(tmpline, strlen(tmpline), direntry, tmp))
		{
			delete [] tmpline;
			if (tmp)
				m_server.nServerType |= tmp;
			if (direntry.name!="." && direntry.name!="..")
				AddLine(direntry);
			if (m_prevline)
			{
				delete [] m_prevline;
				m_prevline=0;
			}
			if (m_curline!=line)
				delete [] m_curline;
			delete [] line;
			line=GetLine();
			m_curline=line;
		}
		else
		{
			delete [] tmpline;
			if (m_prevline)
			{
				if (m_curline!=line)
				{
					delete [] m_prevline;
					m_prevline=m_curline;
					delete [] line;
					line=GetLine();
					m_curline=line;
				}
				else
				{
					line=new char[strlen(m_prevline)+strlen(m_curline)+2];
					sprintf(line, "%s %s", m_prevline, m_curline);
				}
			}
			else
			{
				m_prevline=line;
				line=GetLine();
				m_curline=line;
			}
		}
	}
	if (m_prevline)
	{
		delete [] m_prevline;
		m_prevline=0;
	}
	if (m_curline!=line)
		delete [] m_curline;
	delete [] line;
	m_curline=0;
	
	num=m_EntryList.size();
	if (!num)
		return 0;
	t_directory::t_direntry *res=new t_directory::t_direntry[num];
	int i=0;
	for (tEntryList::iterator iter=m_EntryList.begin();iter!=m_EntryList.end();iter++, i++)
	{
		res[i]=*iter;
		res[i].EntryTime=EntryTime;
	}
	m_EntryList.clear();	
	return res;
}

BOOL CFtpListResult::parseLine(const char *lineToParse, const int linelen, t_directory::t_direntry &direntry, int &nFTPServerType)
{
	USES_CONVERSION;

	nFTPServerType = 0;
	direntry.ownergroup = _T("");
	
	if (parseAsUnix(lineToParse, linelen, direntry))
		return TRUE;
	
	if (parseAsDos(lineToParse, linelen, direntry))
		return TRUE;

	if (parseAsEPLF(lineToParse, linelen, direntry))
		return TRUE;

	if (parseAsVMS(lineToParse, linelen, direntry))
	{
#ifndef LISTDEBUG
		m_server.nServerType |= FZ_SERVERTYPE_SUB_FTP_VMS;
#endif // LISTDEBUG
		return TRUE;
	}
	
	if (parseAsOther(lineToParse, linelen, direntry))
		return TRUE;

	if (parseAsIBM(lineToParse, linelen, direntry))
		return TRUE;

	return FALSE;
}

void CFtpListResult::AddData(char *data, int size)
{
	if (!m_curlistaddpos)
		m_curlistaddpos = new t_list;
	else
	{
		m_curlistaddpos->next = new t_list;
		m_curlistaddpos = m_curlistaddpos->next;
	}
	if (!listhead)
	{
		curpos = m_curlistaddpos;
		listhead = m_curlistaddpos;
	}
	m_curlistaddpos->buffer = data;
	m_curlistaddpos->len = size;
	m_curlistaddpos->next = 0;

	t_list *pOldListPos = curpos;
	int nOldListBufferPos = pos;

	//Try if there are already some complete lines
	t_directory::t_direntry direntry;
	char *line = GetLine();
	m_curline = line;
	while (line)
	{
		if (curpos)
		{
			pOldListPos = curpos;
			nOldListBufferPos = pos;
		}
		else
		{
			delete [] line;
			if (m_curline != line)
				delete [] m_curline;				
			m_curline = 0;
			break;
		}			
		int tmp;
		char *tmpline = new char[strlen(line)+1];
		strcpy(tmpline, line);
		if (parseLine(tmpline, strlen(tmpline), direntry, tmp))
		{
			delete [] tmpline;
			if (tmp)
				m_server.nServerType |= tmp;
			if (direntry.name!="." && direntry.name!="..")
				AddLine(direntry);
			if (m_prevline)
			{
				delete [] m_prevline;
				m_prevline=0;
			}
			if (m_curline!=line)
				delete [] m_curline;
			delete [] line;
			line = GetLine();
			m_curline = line;
		}
		else
		{
			delete [] tmpline;
			if (m_prevline)
			{
				if (m_curline != line)
				{
					delete [] m_prevline;
					m_prevline = m_curline;
					delete [] line;
					line = GetLine();
					m_curline = line;
				}
				else
				{
					line=new char[strlen(m_prevline)+strlen(m_curline)+2];
					sprintf(line, "%s %s", m_prevline, m_curline);
				}
			}
			else
			{
				m_prevline=line;
				line=GetLine();
				m_curline=line;
			}
		}
	}
	curpos=pOldListPos;
	pos=nOldListBufferPos;
		
}

void CFtpListResult::SendToMessageLog(HWND hWnd, UINT nMsg)
{
	t_list *oldlistpos = curpos;
	int oldbufferpos = pos;
	curpos = listhead;
	pos=0;
	char *line = GetLine();
	if (!line)
	{
		//Displays a message in the message log
		t_ffam_statusmessage *pStatus = new t_ffam_statusmessage;
		pStatus->post = TRUE;
		pStatus->status = _T("<Empty directory listing>");
		pStatus->type = 5;
		PostMessage(hWnd, nMsg, FZ_MSG_MAKEMSG(FZ_MSG_STATUS, 0), (LPARAM)pStatus);
	}
	while (line)
	{
		CString status = line;
		delete [] line;
		
		//Displays a message in the message log
		t_ffam_statusmessage *pStatus = new t_ffam_statusmessage;
		pStatus->post = TRUE;
		pStatus->status = status;
		pStatus->type = 5;
		if (!PostMessage(hWnd, nMsg, FZ_MSG_MAKEMSG(FZ_MSG_STATUS, 0), (LPARAM)pStatus))
			delete pStatus;
	
		line = GetLine();
	}
	curpos = oldlistpos;
	pos = oldbufferpos;
}

char * CFtpListResult::GetLine()
{
	if (!curpos)
		return 0;
	int len=curpos->len;
	while (curpos->buffer[pos]=='\r' || curpos->buffer[pos]=='\n' || curpos->buffer[pos]==' ' || curpos->buffer[pos]=='\t')
	{
		pos++;
		if (pos>=len)
		{
			curpos=curpos->next;
			if (!curpos)
				return 0;
			len=curpos->len;
			pos=0;
		}
	}

	t_list *startptr=curpos;
	int startpos=pos;
	int reslen=0;

	int emptylen=0;

	while ((curpos->buffer[pos]!='\n')&&(curpos->buffer[pos]!='\r'))
	{
		if (curpos->buffer[pos]!=' ' && curpos->buffer[pos]!='\t')
		{
			reslen+=emptylen+1;
			emptylen=0;
		}
		else
			emptylen++;
		pos++;
		if (pos>=len)
		{
			curpos=curpos->next;
			if (!curpos)
				break;
			len=curpos->len;
			pos=0;
		}
	}
	
	char *res = new char[reslen+1];
	res[reslen]=0;
	int respos=0;
	while (startptr!=curpos && reslen)
	{
		int copylen=startptr->len-startpos;
		if (copylen>reslen)
			copylen=reslen;
		memcpy(&res[respos],&startptr->buffer[startpos], copylen);
		reslen-=copylen;
		respos+=startptr->len-startpos;
		startpos=0;
		startptr=startptr->next;
	}
	if (curpos && reslen)
	{
		int copylen=pos-startpos;
		if (copylen>reslen)
			copylen=reslen;
		memcpy(&res[respos], &curpos->buffer[startpos], copylen);
	}

	return res;
}

void CFtpListResult::AddLine(t_directory::t_direntry &direntry)
{
	if (m_server.nTimeZoneOffset && direntry.date.hasdate && direntry.date.hastime)
	{
		SYSTEMTIME st = {0};
		st.wYear = direntry.date.year;
		st.wMonth = direntry.date.month;
		st.wDay = direntry.date.day;
		st.wHour = direntry.date.hour;
		st.wMinute = direntry.date.minute;
		
		FILETIME ft;
		SystemTimeToFileTime(&st, &ft);
		_int64 nFt = ((_int64)ft.dwHighDateTime << 32) + ft.dwLowDateTime;
		_int64 nFt2 = nFt;
		nFt += ((_int64)m_server.nTimeZoneOffset) * 10000000 * 60;
		ft.dwHighDateTime = static_cast<unsigned long>(nFt >> 32);
		ft.dwLowDateTime = static_cast<unsigned long>(nFt % 0xFFFFFFFF);
		FileTimeToSystemTime(&ft, &st);
		direntry.date.year = st.wYear;
		direntry.date.month = st.wMonth;
		direntry.date.day = st.wDay;
		direntry.date.hour = st.wHour;
		direntry.date.minute = st.wMinute;
	}
	direntry.lName = direntry.name;
	direntry.lName.MakeLower();

	if (m_server.nServerType&FZ_SERVERTYPE_SUB_FTP_VMS)
	{ //Remove version information, only keep the latest file
		int pos=direntry.name.ReverseFind(';');
		if (pos<=0 || pos>=(direntry.name.GetLength()-1))
			return;;
		int version=_ttoi(direntry.name.Mid(pos+1));
		direntry.name=direntry.name.Left(pos);
		
		tEntryList::iterator entryiter=m_EntryList.begin();
		tTempData::iterator dataiter=m_TempData.begin();
		BOOL bContinue=FALSE;
		while (entryiter!=m_EntryList.end())
		{
			ASSERT(dataiter!=m_TempData.end());
			t_directory::t_direntry dir=*entryiter;
			int oldversion=*dataiter;
			if (direntry.name==dir.name)
			{
				bContinue=TRUE;
				if (version>oldversion)
				{
					*entryiter=direntry;
					*dataiter=version;
				}
				break;
			}
			entryiter++;
			dataiter++;
		}
		if (bContinue)
			return;
		m_EntryList.push_back(direntry);
		m_TempData.push_back(version);				
	}
	else
	{
		m_EntryList.push_back(direntry);
		m_TempData.push_back(0);
	}
}

bool CFtpListResult::IsNumeric(const char *str, int len) const
{
	if (!str)
		return false;
	if (!*str)
		return false;
	const char *p=str;
	while(*p)
	{
		if (len != -1)
			if ((p - str) >= len)
				return true;

		if (*p<'0' || *p>'9')
		{
			return false;
		}
		p++;
	}
	return true;
}

bool CFtpListResult::ParseShortDate(const char *str, int len, t_directory::t_direntry::t_date &date) const
{
	if (!str)
		return false;

	if (len <= 0)
		return false;
	
	int i=0;
	
	//Extract the date
	BOOL bGotYear = FALSE;
	BOOL bGotMonth = FALSE;
	BOOL bGotDay = FALSE;
	int value = 0;
	while (str[i]!='-' && str[i]!='.' && str[i]!='/')
	{
		if (!str[i])
			return false;
		value *= 10;
		value+=str[i]-'0';
		i++;
		if (i == len)
			return false;
	}
	if (i == 4)
	{ //Seems to be yyyy-mm-dd
		if (value < 1900)
			return false;
		date.year = value;
		bGotYear = TRUE;
	}
	else if (i <= 2)
	{
		if (str[i] == '.')
		{
			// Maybe dd.mm.yyyy
			if (!value || value > 31)
				return false;
			date.day = value;
			bGotDay = TRUE;
		}
		else
		{
			// Seems to be mm-dd-yyyy or mm/dd/yyyy (stupid format, though)
			if (!value || value > 12)
				return false;
			date.month = value;
			bGotMonth = TRUE;
		}
	}
	else
		return false;
	
	
	//Extract the second date field
	const char *p = str + i + 1;
	len -= i + 1;
	i=0;
	value=0;

	if (i >= len)
		return false;

	while (p[i]!='-' && p[i]!='.' && p[i]!='/')
	{
		value *= 10;
		value += p[i]-'0';
		i++;

		if (i >= len)
			return false;
	}
	if (bGotYear || bGotDay)
	{
		// Month field in yyyy-mm-dd or dd-mm-yyyy
		if (!value || value > 12)
			return false;
		date.month = value;
		bGotMonth = TRUE;
		
	}
	else
	{
		// Day field in mm-dd-yyyy
		if (!value || value > 31)
			return false;
		date.day = value;
		bGotDay = TRUE;
	}
	
	//Extract the last date field
	p += i+1;
	len -= i + 1;
	i=0;
	value=0;

	if (i >= len)
		return false;
	while (p[i]!='-' && p[i]!='.' && p[i]!='/')
	{
		value *= 10;
		value += p[i]-'0';
		i++;
		if (i >= len)
			break;
	}
	
	if (bGotYear)
	{
		// Day field in yyyy-mm-dd
		if (!value || value > 31)
			return false;
		date.day = value;
	}
	else
	{
		//Year in dd.mm.yyyy or mm-dd-yyyy
		date.year = value;
		if (date.year<50)
			date.year+=2000;
		else if (date.year<1000)
			date.year += 1900;
	}

	date.hasdate = TRUE;
	return true;
}

BOOL CFtpListResult::parseAsVMS(const char *line, const int linelen, t_directory::t_direntry &direntry)
{
	int tokenlen = 0;
	int pos = 0;
	USES_CONVERSION;
	
	std::map<CString, int>::const_iterator iter;
	t_directory::t_direntry dir;
	
	const char *str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;

	if (!strnchr(str, tokenlen, ';'))
		return FALSE;

	dir.size = -2;
	const char *separator = strnchr(str, tokenlen, ';');
	if (!separator)
		return FALSE;
		
	dir.dir = FALSE;
	
	if ((separator - str) > 4)
		if (*(separator - 4) == '.')
			if (*(separator - 3) == 'D')
				if (*(separator - 2) == 'I')
					if (*(separator - 1) == 'R')
					{
						dir.dir = TRUE;
					}
	if (dir.dir)
	{
		int i;
		LPTSTR pBuffer = dir.name.GetBuffer(tokenlen - 4);
		for (i = 0; i < (separator - str - 4); i++)
			pBuffer[i] = str[i];
		for (i = 0; i < (tokenlen - (separator - str)); i++)
			pBuffer[i + (separator - str) - 4] = separator[i];
		dir.name.ReleaseBuffer(tokenlen - 4);
	}
	else
		copyStr(dir.name, 0, str, tokenlen);

	//Size
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	
	const char *p = strnchr(str, tokenlen, '/');
	int len;
	if (p)
		len = p - str;
	else
		len = tokenlen;
	
	if (!IsNumeric(str, len))
		return FALSE;
	
	dir.size = strntoi64(str, len) * 512;
		
	//Get date
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;

	dir.date.hasdate = TRUE;

	//Day
	p = str;

	while (*p != '-') 
		if ((++p - str) == tokenlen)
			return 0;

	dir.date.day = static_cast<int>(strntoi64(str, p-str));
	p++;
	const char *pMonth = p;
	//Month
	while (*p != '-') 
		if ((++p - str) == tokenlen)
			return 0;
	if ((p - pMonth) >= 15)
		return 0;
	char buffer[15] = {0};
	memcpy(buffer, pMonth, p-pMonth);
	strlwr(buffer);
	iter = m_MonthNamesMap.find(A2T(buffer));
	if (iter==m_MonthNamesMap.end())
		return FALSE;
	dir.date.month = iter->second;
	p++;

	dir.date.year = static_cast<int>(strntoi64(p, tokenlen - (p - str)));

	//Get time
	dir.date.hastime = TRUE;
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	
	p = str;
	//Hours
	while (*p != ':') 
		if ((++p - str) == tokenlen)
			return 0;
	
	dir.date.hour = static_cast<int>(strntoi64(str, p - str));
	p++;
	
	const char *pMinute = p;
	//Minutes
	while (*p && *p != ':' )
		p++;

	dir.date.minute = static_cast<int>(strntoi64(pMinute, p - pMinute));

	//Owner/Group
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	
	copyStr(dir.ownergroup, 0, str, tokenlen);
	if (tokenlen > 2 && *str == '[' && str[tokenlen-1] == ']')
		dir.ownergroup = dir.ownergroup.Mid(1, dir.ownergroup.GetLength() - 2);
	
	//Permissions
	str = GetNextToken(line, linelen, tokenlen, pos, 1);
	if (!str)
		return FALSE;
	copyStr(dir.permissionstr, 0, str, tokenlen);
	if (tokenlen > 2 && *str == '(' && str[tokenlen-1] == ')')
		dir.permissionstr = dir.permissionstr.Mid(1, dir.permissionstr.GetLength() - 2);
	
	direntry = dir;

	return TRUE;
}

BOOL CFtpListResult::parseAsEPLF(const char *line, const int linelen, t_directory::t_direntry &direntry)
{
	t_directory::t_direntry dir;
	const char *str = strstr(line, "\t");
	
	//Check if directory listing is an EPLF one
	if (*line=='+' && str)
	{
		str++;
		if (!*str)
			return FALSE;
		dir.bLink = FALSE;
		dir.bUnsure = FALSE;
		dir.date.hasdate = dir.date.hastime = FALSE;
		dir.dir = FALSE;
		dir.size = -2;
		dir.name = str;
		const char *fact = line + 1;
		const char *nextfact = fact;
		nextfact = strstr(nextfact, ",");
		//if (nextfact && nextfact < str)
		//	*nextfact=0;
		while (fact<=(str-2))
		{
			int len;
			if (!nextfact)
				len = str - fact - 1;
			else
				len = nextfact - fact;
			if (len == 1 && fact[0] == '/')
				dir.dir = TRUE;
			else if (*fact=='s')
				dir.size = strntoi64(fact+1, len-1);
			else if (*fact=='m')
			{
				__int64 rawtime = strntoi64(fact+1, len-1);
				COleDateTime time((time_t)rawtime);
				dir.date.hasdate = TRUE;
				dir.date.hastime = TRUE;
				dir.date.year = time.GetYear();
				dir.date.month = time.GetMonth();
				dir.date.day = time.GetDay();
				dir.date.hour = time.GetHour();
				dir.date.minute = time.GetMinute();
			}
			else if (len = 5 && *fact=='u' && *(fact+1)=='p')
			{
				char buffer[4] = {0};
				memcpy(buffer, fact+2, len-2);
				direntry.permissionstr = buffer;
			}
			if (!nextfact || nextfact>=(str-2))
				break;
			fact = nextfact+1;
			nextfact = strstr(nextfact+1, ",");
//			if (nextfact && nextfact<str)
//				*nextfact=0;
		}
		
		direntry = dir;
		
		return TRUE;
	}

	return FALSE;
}

BOOL CFtpListResult::parseAsUnix(const char *line, const int linelen, t_directory::t_direntry &direntry)
{
	int pos = 0;
	int tokenlen = 0;

	const char *str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	//Check the first token
	if (str[0] != 'b' &&
		str[0] != 'c' &&
		str[0] != 'd' &&
		str[0] != 'l' &&
		str[0] != 'p' &&
		str[0] != 's' &&
		str[0] != '-')
		return FALSE;

	//First check if it is a netware server
	bool bNetWare = false;
	copyStr(direntry.permissionstr, 0, str, tokenlen);
	if (tokenlen == 1)
	{
		//Yes, it's most likely a netware server
		//Now get the full permission string
		bNetWare = true;
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		if (!str)
			return FALSE;
		direntry.permissionstr += " ";
		copyStr(direntry.permissionstr, direntry.permissionstr.GetLength(), str, tokenlen);
	}

	//Set directory and link flags
	//Always assume links point directories
	//GUI frontend should try to figure out
	//to where the link really points
	if (direntry.permissionstr[0]=='d' || direntry.permissionstr[0]=='l')
		direntry.dir = true;
	else
		direntry.dir = false;
	
	if (direntry.permissionstr[0]=='l')
		direntry.bLink = true;
	else
		direntry.bLink = false;

	bool bNetPresenz = false;
	if (!bNetWare) //On non-netware servers, expect at least two unused tokens
	{
		bool groupid = false;
		// Unused param
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		if (!str)
			return FALSE;
		if (!strncmp(str, "folder", tokenlen) && direntry.dir)
			bNetPresenz = true;	//Assume NetPresenz server
								//However, it's possible that we mark a non-NetPresenz 
								//server if the fileowner is "folder"
		else if (!IsNumeric(str, tokenlen))
		{
			// Check for Connect:Enterprise server
			if (direntry.permissionstr.GetLength() > 3 && direntry.permissionstr.Right(3) == "FTP")
				groupid = TRUE;

			copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), str, tokenlen);
		}
		else
			groupid = TRUE;
	
		if (!bNetPresenz && groupid)
		{
			//Unused param
			str = GetNextToken(line, linelen, tokenlen, pos, 0);
			if (!str)
				return FALSE;

			if (direntry.ownergroup != _T(""))
				direntry.ownergroup += _T(" ");
			copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), str, tokenlen);
		}
	}

	//Skip param, may be used for size
	int skippedlen = 0;
	const char *skipped = GetNextToken(line, linelen,skippedlen, pos, 0);
	if (!skipped)
		return FALSE;
	
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;

	// Keep parsing the information until we get a numerical string,
	// because some broken servers may send 3 tags: domain/network, group and user
	const char *prevstr = 0;
	int prevstrlen = 0;

	std::map<CString, int>::const_iterator iter;
	while (str && !IsNumeric(str, tokenlen) && !IsNumeric(skipped, skippedlen))
	{
		//Maybe the server has left no space between the group and the size
		//because of stupid alignment
		char *tmpstr = new char[tokenlen + 1];
		strncpy(tmpstr, str, tokenlen);
		tmpstr[tokenlen] = 0;
		strlwr(tmpstr);

		USES_CONVERSION;
		iter = m_MonthNamesMap.find(A2T(tmpstr));
		delete [] tmpstr;
		if (iter != m_MonthNamesMap.end())
		{
			BOOL bRightNumeric = true;
			if (skipped[skippedlen-1]<'0' || skipped[skippedlen-1]>'9')
				bRightNumeric = false;

			if (bRightNumeric)
				break;
			
			bRightNumeric = true;
			if (prevstr && prevstrlen)
			{
				if (prevstr[prevstrlen-1]<'0' || prevstr[prevstrlen-1]>'9')
					bRightNumeric = false;
	
				if (bRightNumeric)
				{
					if (direntry.ownergroup != _T(""))
						direntry.ownergroup += _T(" ");
					copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), str, tokenlen);
					skipped = prevstr;
					skippedlen = prevstrlen;
					break;
				}
			}
		}
		if (prevstr)
		{
			if (direntry.ownergroup != _T(""))
				direntry.ownergroup += _T(" ");

			copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), prevstr, prevstrlen);
		}
		prevstr = str;
		prevstrlen = tokenlen;
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
	}
	
	if (!str)
		return FALSE;

	const char *size = str;
	int sizelen = tokenlen;
	if (!IsNumeric(str, tokenlen))
	{
		//Maybe we've skipped too much tokens
		if (!IsNumeric(skipped, skippedlen))
		{
			//Maybe the server has left no space between the group and the size
			//because of stupid alignment
			bool bRightNumeric = true;
			const char *pos;
			for (pos=(str+tokenlen-1); pos > str; pos--)
			{
				if (*pos<'0' || *pos>'9')
				{
					if (pos==(str+tokenlen-1))
						bRightNumeric=false;
					break;
				}
			}
			if (bRightNumeric && pos>str)
			{
				size = pos + 1;
				sizelen = pos - str;
				direntry.ownergroup += _T(" ");

				if (direntry.ownergroup != _T(""))
					direntry.ownergroup += _T(" ");

				copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), str, pos-str);
				
			}
			else
			{
				for (pos=(skipped+skippedlen-1); pos > skipped; pos--)
				{
					if (*pos<'0' || *pos>'9')
					{
						if (pos==(skipped+skippedlen-1))
							return false;
						break;
					}
				}
				size = pos + 1;
				sizelen = skippedlen + skipped - size;

				if (direntry.ownergroup != _T(""))
					direntry.ownergroup += _T(" ");
				
				copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), skipped, skippedlen - sizelen);

			}
		}
		else
		{
			//We should've not skipped the last token
			//This also fixes the problem with the NetPresenz detection
			size = skipped;
			sizelen = skippedlen;
			if (bNetPresenz && direntry.dir && direntry.ownergroup != "")
			{
				direntry.ownergroup = "folder "+direntry.ownergroup;
			}
		}
	}
	else
	{
		if (direntry.ownergroup != _T(""))
			direntry.ownergroup += _T(" ");
		copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), skipped, skippedlen);
		if (prevstr)
		{
			direntry.ownergroup += _T(" ");
			copyStr(direntry.ownergroup, direntry.ownergroup.GetLength(), prevstr, prevstrlen);
		}
		str = 0;
	}

	direntry.size = strntoi64(size, sizelen);
		
	//Month
	if (!str)
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		
	if (!str)
		return FALSE;
		
	const char *smonth = str;
	int smonthlen = tokenlen;
	direntry.date.year = 0;

	//Day
	const char *sday = 0;
	int sdaylen = 0;
	
	// Some VShell server send both the year and the time, try to detect them
	BOOL bCouldBeVShell = FALSE;

	//Some servers use the following date formats: 
	// 26-09 2002, 2002-10-14, 01-jun-99
	const char *p = strnchr(smonth, smonthlen, '-');
	if (p)
	{
		int plen = smonthlen - (p - smonth);
		const char *pos2 = strnchr(p+1, plen - 1, '-');
		if (!pos2) //26-09 2002
		{
			sday = p + 1;
			sdaylen = plen - 1;
			smonthlen = p-smonth;
		}
		else if (p-smonth == 4) //2002-10-14
		{
			direntry.date.year = static_cast<int>(strntoi64(smonth, p-smonth));
			sday = pos2 + 1;
			sdaylen = smonthlen - (pos2 - smonth) - 1;
			smonthlen = pos2-smonth - (p-smonth) - 1;
			smonth = p + 1;
			/* Try to detect difference between yyyy/dd/mm and yyyy/mm/dd
			 * Unfortunately we have to guess which one is the right if 
			 * the month is < 12
			 */
			if (strntoi64(smonth, smonthlen) > 12)
			{
				const char *tmp = smonth;
				smonth = sday;
				sday = tmp;
				int tmplen = smonthlen;
				smonthlen = sdaylen;
				sdaylen = tmplen;
			}
		}
		else if (p-smonth) //14-10-2002 or 01-jun-99
		{
			direntry.date.year = static_cast<int>(strntoi64(pos2+1, tokenlen - (pos2-smonth) - 1));
			sday = smonth;
			sdaylen = p - smonth;
			smonthlen = pos2-smonth - (p-smonth) - 1;
			smonth = p + 1;
			/* Try to detect difference between yyyy/dd/mm and yyyy/mm/dd
			 * Unfortunately we have to guess which one is the right if 
			 * the month is < 12
			 */
			if (strntoi64(smonth, smonthlen) > 12)
			{
				const char *tmp = smonth;
				smonth = sday;
				sday = tmp;
				int tmplen = smonthlen;
				smonthlen = sdaylen;
				sdaylen = tmplen;
			}
		}
		else
			return FALSE;
	}
	/* Some servers use the following date formats: 
	 * yyyy/dd/mm, yyyy/mm/dd, dd/mm/yyyy, mm/dd/yyyy
	 * try to detect them.
	 */
	else if (strnchr(smonth, smonthlen, '/'))
	{
		const char *p = strnchr(smonth, smonthlen, '/');
		int plen = smonthlen - (p - smonth);
		const char *pos2 = strnchr(p+1, plen - 1, '/');
		if (!pos2) //Assume 26/09 2002
		{
			sday = p + 1;
			sdaylen = plen - 1;
			smonthlen = p-smonth;
		}
		else if (p-smonth==4)
		{
			direntry.date.year = static_cast<int>(strntoi64(smonth, p-smonth));
			sday = pos2 + 1;
			sdaylen = smonthlen - (pos2 - smonth) - 1;
			smonthlen = pos2-smonth - (p-smonth) - 1;
			smonth = p + 1;
			/* Try to detect difference between yyyy/dd/mm and yyyy/mm/dd
			 * Unfortunately we have to guess which one is the right if 
			 * the month is < 12
			 */
			if (strntoi64(smonth, smonthlen) > 12)
			{
				const char *tmp = smonth;
				smonth = sday;
				sday = tmp;
				int tmplen = smonthlen;
				smonthlen = sdaylen;
				sdaylen = tmplen;
			}
		}
		else if (p-smonth==2)
		{
			direntry.date.year = static_cast<int>(strntoi64(pos2+1, tokenlen - (pos2-smonth) - 1));
			sday = smonth;
			sdaylen = p - smonth;
			smonthlen = pos2-smonth - (p-smonth) - 1;
			smonth = p + 1;
			/* Try to detect difference between yyyy/dd/mm and yyyy/mm/dd
			 * Unfortunately we have to guess which one is the right if 
			 * the month is < 12
			 */
			if (strntoi64(smonth, smonthlen) > 12)
			{
				const char *tmp = smonth;
				smonth = sday;
				sday = tmp;
				int tmplen = smonthlen;
				smonthlen = sdaylen;
				sdaylen = tmplen;
			}
		}
		else
			return FALSE;
	}
	else
	{
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		if (!str)
			return FALSE;
	
		sday = str;
		sdaylen = tokenlen;
		
		if (sdaylen && sday[sdaylen-1] == ',')
		{
			sdaylen--;
			bCouldBeVShell = TRUE;
		}

		//Trim trailing characters
		while (sdaylen && (sday[sdaylen-1]=='.' || sday[sdaylen-1]==','))
		{
			bCouldBeVShell = FALSE;
			sdaylen--;
		}

		if (!sdaylen)
			return FALSE;
	}

	if (!strntoi64(sday, sdaylen)) //Day field invalid
	{ //Maybe the server is sending a directory listing with localized date format. 
	  //Try to fix this really bad behaviour
		bCouldBeVShell = FALSE;
		const char *tmp = smonth;
		smonth = sday;
		sday = tmp;
		int tmplen = smonthlen;
		smonthlen = sdaylen;
		sdaylen = tmplen;
	}
	
	//Time/Year
	str = GetNextToken(line, linelen, tokenlen, pos, 0);(&line, 0);
	if (!str)
		return FALSE;
					
	//Trim trailing characters
	while (smonthlen && (smonth[smonthlen-1]=='.' || smonth[smonthlen-1]==','))
		smonthlen--;

	if (!smonthlen)
		return FALSE;

	char *lwr = new char[smonthlen + 1];
	memcpy(lwr, smonth, smonthlen);
	lwr[smonthlen] = 0;
	_strlwr(lwr);

	//Try if we can recognize the month name
	USES_CONVERSION;
	iter = m_MonthNamesMap.find(A2T(lwr));
	delete [] lwr;
	if (iter == m_MonthNamesMap.end())
		return FALSE;
	direntry.date.month = iter->second;

	direntry.date.day = static_cast<int>(strntoi64(sday, sdaylen));

	const char *stimeyear = str;
	int stimeyearlen = tokenlen;

	//Parse the time/year token
	const char *strpos = strnchr(stimeyear, stimeyearlen, ':');
	if (!strpos)
		strpos = strnchr(stimeyear, stimeyearlen, '.');
	if (!strpos)
		strpos = strnchr(stimeyear, stimeyearlen, '-');
	if (strpos)
	{
		//stimeyear has delimiter, so it's a time
		direntry.date.hour = static_cast<int>(strntoi64(stimeyear, strpos - stimeyear));
		direntry.date.minute = static_cast<int>(strntoi64(strpos + 1, stimeyearlen - (strpos - stimeyear) - 1));
		direntry.date.hastime = TRUE;

		//Problem: Some servers use times only for files newer than 6 months,
		//others use one year as limit. So there is no support for files with time 
		//dated in the near future. Under normal conditions there should not be such files
		if (!direntry.date.year)
		{
			CTime curtime = CTime::GetCurrentTime();
			int curday = curtime.GetDay();
			int curmonth = curtime.GetMonth();
			int curyear = curtime.GetYear();
			int now = curmonth*31+curday;
			int file = direntry.date.month*31+direntry.date.day;
			if ((now+1)>=file)
				direntry.date.year = curyear;
			else
				direntry.date.year = curyear-1;
		}
		bCouldBeVShell = FALSE;
	}
	else
	{
		if (!direntry.date.year)
		{
			//No delimiters -> year

			direntry.date.hastime = FALSE;
			direntry.date.year = static_cast<int>(strntoi64(stimeyear, stimeyearlen));
		}
		else
		{
			// File has no time token and short date format
			pos -= stimeyearlen;
		}
	}

	if (!direntry.date.year) //Year 0? Really ancient file, this is invalid!
		return FALSE;			

	// Check if server could still be one of the newer VShell servers
	if (bCouldBeVShell)
	{
		int oldpos = pos;

		//Get time
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		if (!str)
			return FALSE;

		const char *p = strnchr(str, tokenlen, ':');
		if (pos && (p - str) == 2 && IsNumeric(str, 2) && IsNumeric(str + 3, 2))
		{
			//stimeyear has delimiter, so it's a time
			int hour = static_cast<int>(strntoi64(str, 2));
			int minute = static_cast<int>(strntoi64(str + 3, 2));

			if (hour >= 0 && hour < 24 && minute >= 0 && minute < 60)
			{
				direntry.date.hour = hour;
				direntry.date.minute = minute;			
				direntry.date.hastime = TRUE;
			}
			else
				pos = oldpos;
		}
		else
			pos = oldpos;
	}

	//Get filename
	str = GetNextToken(line, linelen, tokenlen, pos, 1);
	if (!str)
		return FALSE;

	//Trim link data from filename
	if (direntry.bLink)
	{
		const char *pos = strnstr(str, tokenlen, " ->");
		if (pos)
			tokenlen = pos - str;

		if (!tokenlen)
			return FALSE;
	}

	//Trim indicators, some server add those to mark special files
	if (str[tokenlen - 1] == '*' ||
		str[tokenlen - 1] == '/' ||
//		str[tokenlen - 1] == '=' || //Don't trim this char, it would cause problems on certain servers
									//This char just marks sockets, so it will never appear as indicator
									//However it is valid as character for filenames on some systems
		str[tokenlen - 1] == '|')
		tokenlen--;

	copyStr(direntry.name, 0, str, tokenlen);

	direntry.bUnsure = FALSE;
	direntry.date.hasdate = TRUE;
	
	return TRUE;
}

BOOL CFtpListResult::parseAsDos(const char *line, const int linelen, t_directory::t_direntry &direntry)
{
	int pos = 0;
	int tokenlen = 0;

	const char *str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	//Check the first token
	if (str[0]=='b' ||
		str[0]=='c' ||
		str[0]=='d' ||
		str[0]=='l' ||
		str[0]=='p' ||
		str[0]=='s' ||
		str[0]=='-')
		return FALSE;
	
	if (IsNumeric(str, tokenlen))
		return FALSE;
	
	//It's a NT server with MSDOS directory format

	if (!ParseShortDate(str, tokenlen, direntry.date))
		return FALSE;
			
	//Extract time
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;

	if (!parseTime(str, tokenlen, direntry.date))
		return FALSE;
			
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	if (tokenlen == 5 && !memcmp(str, "<DIR>", 5))
	{
		direntry.dir = TRUE;
		direntry.size = -1;
	}
	else
	{
		direntry.dir = FALSE;
		direntry.size = strntoi64(str, tokenlen);
	}
		
	str = GetNextToken(line, linelen, tokenlen, pos, 1);
	if (!str)
		return FALSE;
	copyStr(direntry.name, 0, str, tokenlen);

	direntry.bUnsure = FALSE;
		
	return TRUE;
}

BOOL CFtpListResult::parseAsOther(const char *line, const int linelen, t_directory::t_direntry &direntry)
{
	int pos = 0;
	int tokenlen = 0;
	
	const char *str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	
	//Check the first token
	if (str[0]=='b' ||
		str[0]=='c' ||
		str[0]=='d' ||
		str[0]=='l' ||
		str[0]=='p' ||
		str[0]=='s' ||
		str[0]=='-')
		return FALSE;

	if (!IsNumeric(str, tokenlen))
		return FALSE;
	
	//Could be numerical Unix style format or VShell format
	//or even an OS2 server

	const char *skipped = str;
	int skippedtokenlen = tokenlen;
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;

	//If next token is numerical, than it's the numerical Unix style format, 
	//else it's the VShell or OS/2 format
	if (IsNumeric(str, tokenlen))
	{
		copyStr(direntry.permissionstr, 0, skipped, skippedtokenlen);

		if (skippedtokenlen >= 2 && skipped[1] == '4')
			direntry.dir = TRUE;
		else
			direntry.dir = FALSE;

		//Unused token
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		if (!str)
			return FALSE;
			
		//Size
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		if (!str)
			return FALSE;

		direntry.size = strntoi64(str, tokenlen);

		//Date/Time
		str = GetNextToken(line, linelen, tokenlen, pos, 0);
		if (!str)
			return FALSE;

		time_t secsSince1970 = static_cast<long>(strntoi64(str, tokenlen));
		tm *sTime = gmtime(&secsSince1970);
		direntry.date.year = sTime->tm_year + 1900;
		direntry.date.month = sTime->tm_mon+1;
		direntry.date.day = sTime->tm_mday;
		direntry.date.hour = sTime->tm_hour;
		direntry.date.minute = sTime->tm_min;
		direntry.date.hasdate = direntry.date.hastime = TRUE;


		str = GetNextToken(line, linelen, tokenlen, pos, 1);
		copyStr(direntry.name, 0, str, tokenlen);
	}
	else
	{
		std::map<CString, int>::const_iterator iter;	

		//Get size
		direntry.size = strntoi64(skipped, skippedtokenlen);
		
		//Get date, month first
		if (tokenlen >= 15)
			return FALSE;

		char buffer[15] = {0};
		memcpy(buffer, str, tokenlen);
		strlwr(buffer);

		USES_CONVERSION;
		iter = m_MonthNamesMap.find(A2T(buffer));
		if (iter == m_MonthNamesMap.end())
		{
			direntry.dir = FALSE;
			while (str)
			{
				//Could be an OS/2 server
				if (tokenlen == 3 && !memcmp(str, "dir", 3))
					direntry.dir = TRUE;
				else if (tokenlen == 3 && !memcmp(str, "DIR", 3))
					direntry.dir = TRUE;
				else if (strnchr(str, tokenlen, '-'))
					break;
				str = GetNextToken(line, linelen, tokenlen, pos, 0);
			}
			if (!str)
				return FALSE;

			if (!ParseShortDate(str, tokenlen, direntry.date))
				return FALSE;
		
			str = GetNextToken(line, linelen, tokenlen, pos, 0);
			if (!str)
				return FALSE;

			//Parse the time token
			const char *strpos = strnchr(str, tokenlen, ':');
			if (!strpos)
				return FALSE;
			if (strpos)
			{
				//stimeyear has delimiter, so it's a time
				direntry.date.hour = static_cast<int>(strntoi64(str, strpos - str));
				direntry.date.minute = static_cast<int>(strntoi64(strpos+1, tokenlen - (strpos - str) - 1));
				direntry.date.hastime = TRUE;

				//Problem: Some servers use times only for files newer than 6 months,
				//others use one year as limit. So there is no support for files with time 
				//dated in the near future. Under normal conditions there should not be such files
				if (!direntry.date.year)
				{
					CTime curtime = CTime::GetCurrentTime();
					int curday = curtime.GetDay();
					int curmonth = curtime.GetMonth();
					int curyear = curtime.GetYear();
					int now = curmonth*31+curday;
					int file = direntry.date.month*31+direntry.date.day;
					if ((now+1)>=file)
						direntry.date.year = curyear;
					else
						direntry.date.year = curyear-1;
				}
			}

			str = GetNextToken(line, linelen, tokenlen, pos, 1);
			if (!str)
				return FALSE;

			copyStr(direntry.name, 0, str, tokenlen);
		}
		else
		{				
			direntry.date.month = iter->second;
			
			//Day
			str = GetNextToken(line, linelen, tokenlen, pos, 0);
			if (!str)
				return FALSE;
			
			if (str[tokenlen-1]==',')
				tokenlen--;
			if (!IsNumeric(str, tokenlen))
				return FALSE;

			direntry.date.day = static_cast<int>(strntoi64(str, tokenlen));
			if (direntry.date.day < 1 || direntry.date.day > 31)
				return FALSE;
	
			//Year
			str = GetNextToken(line, linelen, tokenlen, pos, 0);
			if (!str)
				return FALSE;
	
			if (!IsNumeric(str, tokenlen))
				return FALSE;
			
			direntry.date.year = static_cast<int>(strntoi64(str, tokenlen));
			if (direntry.date.year < 50)
				direntry.date.year += 2000;
			else if (direntry.date.year < 1000)
				direntry.date.year += 1900;
						
			direntry.date.hasdate = TRUE;
	
			//Now get the time
			str	= GetNextToken(line, linelen, tokenlen, pos, 0);
			const char *p = strnchr(str, tokenlen, ':');
			if (!p)
				return FALSE;
			
			if (p==str || !IsNumeric(str, p-str) || (p-str + 1) >= tokenlen || !IsNumeric(p+1, tokenlen - (p-str) - 1))
				return FALSE;
			direntry.date.hour = static_cast<int>(strntoi64(str, p-str));
			direntry.date.minute = static_cast<int>(strntoi64(p+1, tokenlen - (p-str) - 1));
	
			if (direntry.date.hour<1 || direntry.date.hour > 24)
				return FALSE;
			if (direntry.date.minute<0 || direntry.date.minute>59)
				return FALSE;
	
			direntry.date.hastime = TRUE;
	
			str = GetNextToken(line, linelen, tokenlen, pos, 1);
			if (!str)
				return FALSE;
	
			if (tokenlen > 1 && (str[tokenlen-1] == '\\' || str[tokenlen-1] == '/'))
			{
				direntry.dir = TRUE;
				tokenlen--;
			}
			else
				direntry.dir = FALSE;
	
			copyStr(direntry.name, 0, str, tokenlen);
		}
	}

	direntry.bUnsure = FALSE;
	direntry.date.hasdate = TRUE;
	
	return TRUE;
}

_int64 CFtpListResult::strntoi64(const char *str, int len) const
{
	_int64 res = 0;
	const char *p = str;
	while ((p-str) < len)
	{
		if (*p < '0' || *p > '9')
			break;
		res *= 10;
		res += *p++ - '0';
	}
	return res;
}

const char *CFtpListResult::GetNextToken(const char *line, const int linelen, int &len, int &pos, int type) const
{
	const char *p = line + pos;
	if ((p - line) >= linelen)
		return NULL;
	while ((p - line) < linelen && (!p || *p==' ' || *p=='\t'))
		p++;

	if ((p - line) >= linelen)
		return NULL;
	
	const char *res = p;
	
	if (type)
	{
		pos = linelen;
		len = linelen - (p - line);
	}
	else
	{
		while ((p - line) < linelen && *p && *p != ' ' && *p!='\t')
			p++;
		
		len = p - res;
		pos = p - line;
	}

	return res;
}

const char * CFtpListResult::strnchr(const char *str, int len, char c) const
{
	if (!str)
		return NULL;

	const char *p = str;
	while (len > 0)
	{
		if (!*p)
			return NULL;
		if (*p == c)
			return p;
		p++;
		len--;
	}
	return NULL;
}

const char * CFtpListResult::strnstr(const char *str, int len, const char *c) const
{
	if (!str)
		return NULL;
	if (!c)
		return NULL;
	int clen = strlen(c);

	const char *p = str;
	while (len > 0)
	{
		if (!*p)
			return NULL;
		if (*p == *c)
		{
			if (clen == 1)
				return p;
			else if (len >= clen)
			{
				if (!memcmp(p + 1, c+1, clen-1))
					return p;
			}
			else
				return NULL;
		}
		p++;
		len--;
	}
	return NULL;
}

void CFtpListResult::copyStr(CString &target, int pos, const char *source, int len) const
{
	int i;
	LPTSTR pBuffer = target.GetBuffer(pos + len);
	ASSERT(pBuffer);
	for (i = 0; i < len; i++)
		pBuffer[pos + i] = source[i];
	target.ReleaseBuffer(pos + len);
}

BOOL CFtpListResult::parseAsIBM(const char *line, const int linelen, t_directory::t_direntry &direntry)
{
	int pos = 0;
	int tokenlen = 0;
	
	const char *str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;

	copyStr(direntry.ownergroup , 0, str, tokenlen);

	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;

	if (!IsNumeric(str, tokenlen))
		return FALSE;

	direntry.size = strntoi64(str, tokenlen);

	//Date
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	if (!ParseShortDate(str, tokenlen, direntry.date))
		return FALSE;

	//Time
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	if (!parseTime(str, tokenlen, direntry.date))
		return FALSE;

	//Unused Token
	str = GetNextToken(line, linelen, tokenlen, pos, 0);
	if (!str)
		return FALSE;
	
	//Name
	str = GetNextToken(line, linelen, tokenlen, pos, 1);
	if (!str)
		return FALSE;

	if (str[tokenlen-1] == '/')
	{
		direntry.dir = TRUE;
		if (!--tokenlen)
			return FALSE;
	}
	else
		direntry.dir = FALSE;

	copyStr(direntry.name, 0, str, tokenlen);

	return true;
}

bool CFtpListResult::parseTime(const char *str, int len, t_directory::t_direntry::t_date &date) const
{
	int i = 0;
	//Extract the hour
	date.hastime = TRUE;
	date.hour=0;
	while (str[i]!=':')
	{
		if (!str[i])
			return FALSE;
		date.hour *= 10;
		date.hour += str[i]-'0';
		if (date.hour > 24)
			return false;
	
		i++;
		if (i == len)
			return false;
	}
	i++;
	if (!date.hour)
		return false;

	if (i == len)
		return false;
	
	//Extract the minute
	date.minute=0;
	while (str[i]>='0' && str[i]<='9')
	{
		date.minute *= 10;
		date.minute += str[i]-'0';
		if (date.minute > 59)
			return false;
	
		i++;
		if (i == len)
			break;
	}
	
	//Convert to 24h format
	//I really wish we would have the following system:
	//one year->ten months->ten days->ten hours->ten minutes->ten seconds and so on...
	//I should modifiy the earth rotation to force everyone to use this system *g*
	if (i != len && str[i]=='P')
	{
		if (date.hour <= 12)
			date.hour += 12;
	}
	else 
		if (date.hour == 12)
			date.hour = 0;

	date.hastime = TRUE;

	return true;
}