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
    Kota Batam - 29424
    INDONESIA
*/

#include <windows.h>
#include "resource.h"
#include "../steks_c/stext.h"

Str63 DeviceNameDlg_name;

char *GetDeviceNameDlg_name (char *retname)
{
    if (retname)
        strcpy (retname, DeviceNameDlg_name);
    return DeviceNameDlg_name;
}

int DeviceNameDlg_WM_INITDIALOG (HWND hDlg)
{
    /** EDIT */
    memset(DeviceNameDlg_name,0,Str63Len);
    /* EDIT **/
    return TRUE;
}

void DeviceNameDlg_WM_COMMAND_IDOK_BN_CLICKED (HWND hDlg, UINT DlgItemID)
{
    /** EDIT */
    GetDlgItemText (hDlg, IDC_NAME, DeviceNameDlg_name, Str63Len);
    EndDialog (hDlg, 0); /* return the selection */
    /* EDIT **/
}

void DeviceNameDlg_WM_COMMAND_IDOK (HWND hDlg, UINT DlgItemID, UINT Notification)
{
    switch (Notification)
    {
        case BN_CLICKED:
        DeviceNameDlg_WM_COMMAND_IDOK_BN_CLICKED (hDlg, DlgItemID);
        break;
    }
}

void DeviceNameDlg_WM_COMMAND (HWND hDlg, UINT DlgItemID, UINT Notification)
{
    switch (DlgItemID)
    {
        case IDOK:
        DeviceNameDlg_WM_COMMAND_IDOK (hDlg, DlgItemID, Notification);
        break;
    }
}

LRESULT CALLBACK DeviceNameDlg_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        return DeviceNameDlg_WM_INITDIALOG (hDlg);
        case WM_COMMAND:
        DeviceNameDlg_WM_COMMAND (hDlg, LOWORD(wParam), HIWORD(wParam));
        break;
    }
    return FALSE;
}

UINT DeviceNameDlg_DialogBox (HINSTANCE hInst, HWND hWnd)
{
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_REFNAME), hWnd, (DLGPROC)DeviceNameDlg_DlgProc);
}
