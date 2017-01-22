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

#ifndef _BV2OBJECT_H_
#define _BV2OBJECT_H_

#include <windows.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define kObjectBoxMode  0
#define kObjectBarMode  1

HWND InitObjectDlg (HINSTANCE hInst, HWND hWnd);
BOOL ShowObjectInfo (HINSTANCE hInst, HWND hWnd);
void SetObjectInfo (LPCSTR jenis, LPCSTR nama, LPCSTR loc, LPCSTR info);
void ClearObjectInfoText ();
void ShowHideObjectInfo (HINSTANCE hInst, HWND hWnd);

HWND GetObjectInfoWindow ();
void SetObjectInfoMode (int newmode);
BOOL GetObjectInfoMode ();
void MoveObjectInfoBar ();
int GetObjectInfoHeight();
void GetObjectInfo (LPSTR jenis, LPSTR nama, LPSTR loc, LPSTR info);


#ifdef  __cplusplus
	}
#endif


#endif
