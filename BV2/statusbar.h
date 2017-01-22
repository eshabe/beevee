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

#ifndef _BV2_STATUSBAR_
#define _BV2_STATUSBAR_

#include <windows.h>

#ifdef  __cplusplus
extern "C" {
#endif

HWND CreateStatusBar (HINSTANCE hInst, HWND hWndParent);
HWND GetStatusBarWindow ();
void SetStatusBarText (LPCSTR text);
void SetStatusBarTextFromRes (UINT stringResID);
void SetStatusBarTextDef ();
void MoveStatusBar ();
int GetStatusBarHeight ();
void SetStatusBar0Text (LPCSTR text);
void SetStatusBar1Text (LPCSTR text);


#ifdef  __cplusplus
			}
#endif


#endif /* _BV2_STATUSBAR_ */
