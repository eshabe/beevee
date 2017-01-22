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
#include <commctrl.h>

#include "statusbar.h"
#include "resource.h"

#define SBAR_NAME   0
#define SBAR_ZOOM   1
#define SBAR_TEXT   2

typedef struct statusbar_struct {
	HWND hwnd_This;
	int statusbarHI;
} statusbar_struct;

static statusbar_struct mystbar;

#define SB_TIMER_EVENT 0x100

HWND GetStatusBarWindow ()
{
	return mystbar.hwnd_This;
}

int GetStatusBarHeight ()
{
	return mystbar.statusbarHI;
}

void SetStatusBarText (LPCSTR text)
{
	KillTimer (mystbar.hwnd_This, SB_TIMER_EVENT);
	//SetWindowText (mystbar.hwnd_This, text);
	SendMessage (mystbar.hwnd_This, SB_SETTEXT, SBAR_TEXT, (LPARAM)text);
}

void SetStatusBar0Text (LPCSTR text)
{
	SendMessage (mystbar.hwnd_This, SB_SETTEXT, SBAR_NAME, (LPARAM)text);
}

void SetStatusBar1Text (LPCSTR text)
{
	SendMessage (mystbar.hwnd_This, SB_SETTEXT, SBAR_ZOOM, (LPARAM)text);
}

void SetStatusBarTextFromRes (UINT stringResID)
{
	char text[100];
	LoadString (GetModuleHandle(0), stringResID, text, 100);
	SetStatusBarText (text);
}

void SetStatusBarTextDef ()
{
	SetStatusBarTextFromRes (IDS_TOOLBAR);
}

void MoveStatusBar ()
{
	if (IsWindow(mystbar.hwnd_This))
	{
		RECT r;
		GetClientRect (GetParent(mystbar.hwnd_This), &r);
		MoveWindow (mystbar.hwnd_This, 0, r.bottom-mystbar.statusbarHI, r.right-r.left, mystbar.statusbarHI, FALSE);
	}
}


HWND CreateStatusBar (HINSTANCE hInst, HWND hWndParent)
{
	InitCommonControls ();
	mystbar.hwnd_This = CreateStatusWindow (WS_CHILD | WS_VISIBLE,
                            "",
                            hWndParent,
                            IDD_STATUS);
	if (mystbar.hwnd_This)
	{
        RECT r;
        int w[3];
        w[SBAR_NAME] = 200; w[SBAR_ZOOM] = 248; w[SBAR_TEXT] = -1;

        SendMessage (mystbar.hwnd_This, SB_SETPARTS, (WPARAM)3, (LPARAM)w);
        GetWindowRect (mystbar.hwnd_This , &r);
        mystbar.statusbarHI = r.bottom - r.top;
	}

	return mystbar.hwnd_This;
}
