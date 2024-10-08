/*
 *  LDLITE, a program for viewing *.dat files.
 *  Copyright (C) 1998  Paul J. Gyugyi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/// ldliteCommandLineInfo.h: interface for the CldliteCommandLineInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LDLITECOMMANDLINEINFO_H__366E2B82_5306_11D2_A9A1_0080ADB46730__INCLUDED_)
#define AFX_LDLITECOMMANDLINEINFO_H__366E2B82_5306_11D2_A9A1_0080ADB46730__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CldliteCommandLineInfo : public CCommandLineInfo
{
public:
    void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);
    CldliteCommandLineInfo();
    virtual ~CldliteCommandLineInfo();

};

#endif // !defined(AFX_LDLITECOMMANDLINEINFO_H__366E2B82_5306_11D2_A9A1_0080ADB46730__INCLUDED_)
