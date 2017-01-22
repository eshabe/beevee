/*
    BeeVee: Board viewer application - ECAD Layout reader like.
    Copyright (C) 2004-2007  Sihabul Milah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sihabul Milah
    http://eshabe.wordpress.com http://eshabe.blogspot.com
    eshabe@gmail.com
    Perumahan Bina Ummah B-15
    Kibing - Batu Aji
    Kota Batam - 29432
    INDONESIA
*/

#include <windows.h>

#include "resource.h"

BOOL WriteLoadExFileIntoReg (char *filename)
{
	HKEY hkey;

    if (strlen(filename)>255)
        return FALSE;

	if (RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL)==ERROR_SUCCESS)
	{
        RegSetValueEx (hkey, "LoadEx", 0, REG_SZ, (LPBYTE)filename, strlen(filename)+1);
		RegCloseKey (hkey);

		return TRUE;
	}
	return FALSE;
}

BOOL ReadLoadExFileFromReg (char *filename)
{
	HKEY hkey;
	DWORD type;
	DWORD size;

	if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee", 0, KEY_READ, &hkey)==ERROR_SUCCESS)
	{
        type = REG_SZ;
        size = 256;
        memset(filename,0,256);
        RegQueryValueEx (hkey, "LoadEx", 0, &type, (LPBYTE)filename, &size);
		RegCloseKey (hkey);

		return TRUE;
	}
	return FALSE;
}

BOOL BV2LoadEx (LPSTR lpszArgument)
{
    if (strncmp(lpszArgument,"sx ",3)==0 && strlen(lpszArgument)>3)
    {
        WriteLoadExFileIntoReg (&lpszArgument[3]);
        return TRUE;
    }

    if (strcmp(lpszArgument,"lx")!=0)
        return FALSE; /* not LoadEx */

    HWND bvwnd = FindWindow ("BeeVee", NULL);
    if (bvwnd==NULL)
    {
        //MessageBox (NULL, "Can not find BeeVee 2 window!", "LoadEx", MB_ICONSTOP);
        ReadLoadExFileFromReg (lpszArgument);
        return FALSE;
    }
    else
        SendMessage (bvwnd, WM_COMMAND, ID_LOADEX, 0L);
    return TRUE;
}

