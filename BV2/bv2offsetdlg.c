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

typedef struct _offsetxy {
    int x;
    int y;
    int move;
} _offsetxy;
static _offsetxy myOffset;

void GetOffsetXYDlg_XY (int *x, int *y)
{
    *x = myOffset.x;
    *y = myOffset.y;
}

BOOL GetOffsetXYDlg_IsMove ()
{
    return myOffset.move;
}

int OffsetXYDlg_WM_INITDIALOG (HWND hDlg)
{
    /** EDIT */
    myOffset.x = myOffset.y = myOffset.move = 0;
    SendDlgItemMessage (hDlg, IDC_POSX, EM_SETLIMITTEXT, (WPARAM)Str63Len, 0L);
    SendDlgItemMessage (hDlg, IDC_POSY, EM_SETLIMITTEXT, (WPARAM)Str63Len, 0L);
    SetDlgItemText (hDlg, IDC_POSX, "0");
    SetDlgItemText (hDlg, IDC_POSY, "0");
    /* EDIT **/
    return TRUE;
}

void OffsetXYDlg_WM_COMMAND_IDOK_BN_CLICKED (HWND hDlg, UINT DlgItemID)
{
    /** EDIT */
    Str63 temp;
    GetDlgItemText (hDlg, IDC_POSX, temp, Str63Len);
    myOffset.x = atoi(temp);
    GetDlgItemText (hDlg, IDC_POSY, temp, Str63Len);
    myOffset.y = atoi(temp);
    myOffset.move = IsDlgButtonChecked (hDlg, IDC_MOVETO);
    EndDialog (hDlg, IDOK); /* return the selection */
    /* EDIT **/
}

void OffsetXYDlg_WM_COMMAND_IDCANCEL_BN_CLICKED (HWND hDlg, UINT DlgItemID)
{
    /** EDIT */
    EndDialog (hDlg, IDCANCEL); /* return the selection */
    /* EDIT **/
}


void OffsetXYDlg_WM_COMMAND_IDOK (HWND hDlg, UINT DlgItemID, UINT Notification)
{
    switch (Notification)
    {
        case BN_CLICKED:
        OffsetXYDlg_WM_COMMAND_IDOK_BN_CLICKED (hDlg, DlgItemID);
        break;
    }
}

void OffsetXYDlg_WM_COMMAND_IDCANCEL (HWND hDlg, UINT DlgItemID, UINT Notification)
{
    switch (Notification)
    {
        case BN_CLICKED:
        OffsetXYDlg_WM_COMMAND_IDCANCEL_BN_CLICKED (hDlg, DlgItemID);
        break;
    }
}

void OffsetXYDlg_WM_COMMAND (HWND hDlg, UINT DlgItemID, UINT Notification)
{
    switch (DlgItemID)
    {
        case IDOK:
        OffsetXYDlg_WM_COMMAND_IDOK (hDlg, DlgItemID, Notification);
        break;
        case IDCANCEL:
        OffsetXYDlg_WM_COMMAND_IDCANCEL (hDlg, DlgItemID, Notification);
        break;

    }
}

LRESULT CALLBACK OffsetXYDlg_DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG:
        return OffsetXYDlg_WM_INITDIALOG (hDlg);
        case WM_COMMAND:
        OffsetXYDlg_WM_COMMAND (hDlg, LOWORD(wParam), HIWORD(wParam));
        break;
    }
    return FALSE;
}

UINT OffsetXYDlg_DialogBox (HINSTANCE hInst, HWND hWnd)
{
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_OFFSET), hWnd, (DLGPROC)OffsetXYDlg_DlgProc);
}
