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

/* shb-20070906 */
/*
    Fix dialog sizing -> move the "New Window" check box
*/


#include "bv2recents.h"
#include "../sdata_c/sdata.h"
#include "toolbox.h"

#include "resource.h"

#include <stdio.h>

#define MAX_RECENTSLIST 20

typedef struct recents_struct{
    char *file;
    int newwin;
    _sdata rct;
} recents_struct;

recents_struct myrecents;

BOOL ReadRecentFilesInReg ()
{
	HKEY hkey;
	DWORD type;
	DWORD size;
	char key[8];
	char value[256];
	int i=-1;

	if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee\\MRU", 0, KEY_READ, &hkey)==ERROR_SUCCESS)
	{
		while (1)
		{

            type = REG_SZ;
            size = 256; value[0]=0;
            i++;
            sprintf (key,"%d", i);
            RegQueryValueEx (hkey, key, 0, &type, (LPBYTE)value, &size);
            if (size==0||!value[0])
                break;
            sdata_tambah (&myrecents.rct, value, size);

		}
		RegCloseKey (hkey);

		return TRUE;
	}
	return FALSE;
}

BOOL WriteRecentFilesIntoReg ()
{
	HKEY hkey;
	char key[8];
	char value[256];
	int i=-1;


	if (RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee\\MRU", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL)==ERROR_SUCCESS)
	{
		for(i=0;i<sdata_jumlahdata(&myrecents.rct);i++)
		{
            sprintf (key,"%d", i);
            sdata_ambildatanomor(&myrecents.rct, i, value);
            RegSetValueEx (hkey, key, 0, REG_SZ, (LPBYTE)value, strlen(value)+1);
		}
		RegCloseKey (hkey);

		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK BV2RecentsDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int dlgItemID;
	int i;
	char filename[256];
	HWND mCtrl;
	RECT cr,r,r1;

	switch (message)
	{
		case WM_INITDIALOG:
		mCtrl = GetDlgItem(hDlg, IDC_LISTTEXT);
		sdata_init(&myrecents.rct);
		if (ReadRecentFilesInReg())
		{
		    for(i=0;i<sdata_jumlahdata(&myrecents.rct);i++)
		    {
		        sdata_ambildatanomor(&myrecents.rct, i, filename);
		        AddListBoxItem (mCtrl, filename, i);
		    }
		}

		//return TRUE;
		case WM_SIZE:
		GetClientRect (hDlg, &cr);
		mCtrl = GetDlgItem(hDlg, IDC_LISTTEXT);
		MoveWindow (mCtrl, 2, 2, cr.right-4, cr.bottom-28, TRUE);
		mCtrl = GetDlgItem(hDlg, IDOK);
		GetWindowRect(mCtrl, &r);
		MoveWindow (mCtrl, 2, cr.bottom-24, (r.right-r.left), (r.bottom-r.top), TRUE);
		mCtrl = GetDlgItem(hDlg, IDCANCEL);
		MoveWindow (mCtrl, 4+(r.right-r.left), cr.bottom-24, (r.right-r.left), (r.bottom-r.top), TRUE);
		mCtrl = GetDlgItem(hDlg, IDC_CLEAR);
		MoveWindow (mCtrl, 6+(r.right-r.left)*2, cr.bottom-24, (r.right-r.left), (r.bottom-r.top), TRUE);
		mCtrl = GetDlgItem(hDlg, IDC_NEWWINDOW);

		/* shb-20070906 */
		GetWindowRect(mCtrl, &r1);
		MoveWindow (mCtrl, 12+(r.right-r.left)*3, cr.bottom-20, (r1.right-r1.left), (r1.bottom-r1.top), TRUE);
		/* shb-20070906 */

		return TRUE;
		break;

		case WM_COMMAND:
		dlgItemID = LOWORD(wParam);
		if (dlgItemID==IDOK || (dlgItemID==IDC_LISTTEXT && HIWORD(wParam)==LBN_DBLCLK))
		{
            GetListBoxCurSelString (GetDlgItem(hDlg, IDC_LISTTEXT), myrecents.file);
            myrecents.newwin = IsDlgButtonChecked (hDlg, IDC_NEWWINDOW);
		    EndDialog(hDlg, IDOK);
		}
		else if (dlgItemID==IDCANCEL)
		{
		    EndDialog(hDlg, dlgItemID);
		}
		else if (dlgItemID==IDC_CLEAR)
		{
		    RegDeleteKey (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee\\MRU");
		    EndDialog(hDlg, dlgItemID);
		}
		break;
		case WM_DESTROY:
		sdata_hapussemua (&myrecents.rct);
		break;
	}
	return FALSE;
}


UINT GetRecentFile (HINSTANCE hInst, HWND hWnd, const char *filename)
{
    myrecents.file = (char*)filename;
    myrecents.file[0] = 0;
    myrecents.newwin = 0;
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_RECENTS), hWnd, (DLGPROC)BV2RecentsDlgProc);
}

void AddRecentFile (const char *filename)
{
    int i;
    char mf[256];
    char mf2[256];
    sdata_init(&myrecents.rct);
    ReadRecentFilesInReg();
    strcpy (mf2, filename);
    UpperText (mf2);
    for (i=0;i<sdata_jumlahdata(&myrecents.rct);i++)
    {
        sdata_ambildatanomor(&myrecents.rct, i, mf);
        UpperText (mf);
        if (strcmp(mf,mf2)==0)
        {
            sdata_hapusini(&myrecents.rct);
            break;
        }
    }
    if (sdata_jumlahdata(&myrecents.rct)>=MAX_RECENTSLIST)
        sdata_hapusekor (&myrecents.rct);
    sdata_sisipkepala (&myrecents.rct, (void *)filename, strlen(filename)+1);
    WriteRecentFilesIntoReg();
    sdata_hapussemua (&myrecents.rct);
}

int GetRecentFileIsNewWindow ()
{
    return myrecents.newwin;
}
