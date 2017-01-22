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

/* bv2object.cpp */

/*
sihabul milah
18 mei 2006
handel object dlg (IDD_OBJECTDLG)

3 Okt 2006
buang sebagian showwindow karena sudah pake WS_VISIBLE

13 des 2006
re-size-able
*/
#include "BV2.h"
#include "bv2object.h"
#include "statusbar.h"
#include "bv2draw.h"
#include "bv2find.h"
#include "resource.h"


//const int kObjectInfoBarHI = 20;
typedef struct object_struct
{
    HWND hwnd_This;
    int i_mode; /* 0 = box; 1 = bar; def=0 */
    char cstr_kind[11];
    char cstr_name[64];
    char cstr_loc[8];
    char cstr_info[256];
    int hi;
}
object_struct;

static object_struct myobject;

HWND GetObjectInfoWindow ()
{
    return myobject.hwnd_This;
}

BOOL GetObjectInfoMode ()
{
    return myobject.i_mode;
}

void SetObjectInfoMode (int newmode)
{
    if (newmode!=myobject.i_mode)
    {
        if (IsWindow(myobject.hwnd_This))
            SendMessage(myobject.hwnd_This, WM_CLOSE, 0, 0L);
        myobject.i_mode = newmode;
    }
}

void ClearObjectInfoText ()
{
    myobject.cstr_info[0] = 0;
    myobject.cstr_loc[0] = 0;
    myobject.cstr_name[0] = 0;
    strcpy (myobject.cstr_kind, "No object");
}

int GetObjectInfoHeight()
{
    //return kObjectInfoBarHI;
    return myobject.hi;
}

void MoveObjectInfoBar ()
{
    if (IsWindow(myobject.hwnd_This) && myobject.i_mode==kObjectBarMode)
    {
        RECT r;

        GetClientRect (GetParent(myobject.hwnd_This), &r);
        r.top = r.bottom - myobject.hi;

        if (IsWindow(GetStatusBarWindow()))
            r.top -= GetStatusBarHeight();

        MoveWindow (myobject.hwnd_This, r.left, r.top, r.right-r.left, myobject.hi, TRUE);
        //MoveFindBar();
    }
}

void SetObjectItems ()
{
    if (!IsWindow(myobject.hwnd_This))
        return;

    SetDlgItemText (myobject.hwnd_This, IDC_KIND, myobject.cstr_kind);
    SetDlgItemText (myobject.hwnd_This, IDC_NAME, myobject.cstr_name);
    SetDlgItemText (myobject.hwnd_This, IDC_LOC, myobject.cstr_loc);
    SetDlgItemText (myobject.hwnd_This, IDC_INFO, myobject.cstr_info);
}

void SetObjectInfo (LPCSTR jenis, LPCSTR nama, LPCSTR loc, LPCSTR info)
{
    strcpy (myobject.cstr_kind, jenis);
    strcpy (myobject.cstr_name, nama);
    strcpy (myobject.cstr_loc, loc);
    strcpy (myobject.cstr_info, info);
    SetObjectItems();
}

void GetObjectInfo (LPSTR jenis, LPSTR nama, LPSTR loc, LPSTR info)
{
    if (jenis)
        strcpy (jenis, myobject.cstr_kind);
    if (nama)
        strcpy (nama, myobject.cstr_name);
    if (loc)
        strcpy (loc, myobject.cstr_loc);
    if (info)
        strcpy (info, myobject.cstr_info);
}


LRESULT CALLBACK BV2ObjectInfoDlg (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

    RECT wrect;
    HWND ctrl;
    switch (message)
    {
        case WM_INITDIALOG:
        return TRUE;

        case WM_SIZE:
            ctrl = GetDlgItem (hDlg, IDC_INFO);

            GetClientRect (hDlg, &wrect);
            SetWindowPos (ctrl, HWND_NOTOPMOST,
                          0,
                          0,
                          wrect.right-wrect.left-8,
                          wrect.bottom-wrect.top-4,
                          SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
            break;


        case WM_COMMAND:
            if (LOWORD(wParam)!=IDCANCEL)
                break;

        case WM_CLOSE:
            EndDialog(hDlg, 0);
            DestroyWindow(myobject.hwnd_This);
            break;

    }
    return FALSE;

}

LRESULT CALLBACK BV2ObjectInfoBar (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT wrect;
    switch (message)
    {
        case WM_INITDIALOG:
            GetWindowRect (hDlg, &wrect);
            myobject.hi = wrect.bottom - wrect.top;
            CheckDlgButton (hDlg, IDCANCEL, TRUE);
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam)!=IDCANCEL)
                break;

        case WM_CLOSE:
            EndDialog(hDlg, 0);
            MoveFindBar();
            MoveDrawWindow();
            DestroyWindow(myobject.hwnd_This);
            break;
    }
    return FALSE;

}


HWND InitObjectDlg (HINSTANCE hInst, HWND hWnd)
{
    if (IsWindow(myobject.hwnd_This))
        return myobject.hwnd_This;

    if (myobject.i_mode==kObjectBarMode)
        myobject.hwnd_This = CreateDialog (hInst, MAKEINTRESOURCE(IDD_OBJECTBAR), hWnd, (DLGPROC)BV2ObjectInfoBar);
    else
    {
        myobject.hwnd_This = CreateDialog (hInst, MAKEINTRESOURCE(IDD_OBJECTDLG), hWnd, (DLGPROC)BV2ObjectInfoDlg);

        /*
        myobject.hwnd_This = CreateWindowEx (WS_EX_TOOLWINDOW,
            "Edit",
            "Test",
            WS_POPUP | WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_VISIBLE,
            //WS_POPUP | WS_CAPTION | WS_SYSMENU  | WS_VISIBLE | WS_SIZEBOX,
            200, 200, 400, 64,
            //CW_USEDEFAULT,0,CW_USEDEFAULT, 0,
            hWnd,
            NULL,
            GetModuleHandle(0), //hInst,
            BV2ObjectInfo);
        if (myobject.hwnd_This==NULL)
            MessageBox (NULL, "sdfs",NULL,0);
        */
        ShowWindow (myobject.hwnd_This, SW_SHOW);

        myobject.i_mode = 0;
    }

    return myobject.hwnd_This;
}

BOOL ShowObjectInfo (HINSTANCE hInst, HWND hWnd)
{
     if (!IsWindow(myobject.hwnd_This))
        InitObjectDlg (hInst, hWnd);

    if (!myobject.hwnd_This)
        return FALSE;

    SetObjectItems();

    if (myobject.i_mode == kObjectBarMode)
    {
        MoveObjectInfoBar ();
        MoveFindBar(); /* move find bar, ensure it at above */
        MoveDrawWindow(); /* redraw drawing window, can-also: send WM_SIZE to parent */
    }
    ShowWindow(myobject.hwnd_This, SW_SHOWDEFAULT);

    return TRUE;
}

void ShowHideObjectInfo (HINSTANCE hInst, HWND hWnd)
{
    if(IsWindowVisible(myobject.hwnd_This))
        SendMessage (myobject.hwnd_This, WM_CLOSE, 0, 0);
    else
        ShowObjectInfo (hInst, hWnd);
}

