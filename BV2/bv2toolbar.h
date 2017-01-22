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

#ifndef _SHB_BV2TOOLBAR_H_
#define _SHB_BV2TOOLBAR_H_

#include <windows.h>

#ifdef  __cplusplus
extern "C" {
#endif

HWND CreateToolBar			(HINSTANCE hInst, HWND hWndParent);
HWND GetToolBarWindow		();
int GetToolBarBottom			();
//int GetToolBarHeight			();

void SetBtnToolBarState		(UINT tbId, UINT state);
void EnableToolBarButton	(UINT tbId, BOOL enable);
void CheckToolBarButton		(UINT tbId, BOOL check);
void ToolBarNotifyHandler (LPARAM lParam);

BOOL MoveToolBar ();

#ifdef  __cplusplus
}
#endif

#endif
