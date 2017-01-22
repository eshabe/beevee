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



void Unit_WM_INITDIALOG (HWND hDlg)
{
    /** EDIT */
    CheckRadioButton (hDlg, IDC_RBUNITMILS, IDC_RBUNITMM, IDC_RBUNITMILS);
    /* EDIT **/
}

void Unit_WM_COMMAND_IDOK_BN_CLICKED (HWND hDlg, UINT DlgItemID)
{
    /** EDIT */
    int i;
    for (i=IDC_RBUNITMILS;i<=IDC_RBUNITMM;i++)
    {
        if (IsDlgButtonChecked (hDlg, i))
            break;
    }
    EndDialog (hDlg, i-IDC_RBUNITMILS); /* return the selection */
    /* EDIT **/
}

void Unit_WM_COMMAND_IDOK (HWND hDlg, UINT DlgItemID, UINT Notification)
{
    switch (Notification)
    {
        case BN_CLICKED:
        Unit_WM_COMMAND_IDOK_BN_CLICKED (hDlg, DlgItemID);
        break;
    }
}

void Unit_WM_COMMAND (HWND hDlg, UINT DlgItemID, UINT Notification)
{
    switch (DlgItemID)
    {
        case IDOK:
        Unit_WM_COMMAND_IDOK (hDlg, DlgItemID, Notification);
        break;
    }
}

LRESULT CALLBACK Unit_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        Unit_WM_INITDIALOG (hDlg);
        return TRUE;
        case WM_COMMAND:
        Unit_WM_COMMAND (hDlg, LOWORD(wParam), HIWORD(wParam));
        break;
    }
    return FALSE;
}

UINT Unit_DialogBox (HINSTANCE hInst, HWND hWnd)
{
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_UNIT), hWnd, (DLGPROC)Unit_DlgProc);
}
