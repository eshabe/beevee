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

#ifndef _BV2FIND_WIN32_

#include <windows.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* find */
enum BV2_FIND_MODE {
	find_Device,
	find_Signal,
	find_Node,
	find_Nail,
	find_Probe
};

#define kFindBoxMode 0
#define kFindBarMode 1

void	FindShowDialog (HWND hWndParent, int findType);
HWND	GetFindDialog ();
BOOL	FindDialogIsVisible ();
void	FindClearStruct ();
int	GetFindMode ();
void	SetFindMode (int newmode);
int	GetFindBarHeight ();
void	MoveFindBar();

int FindComponent (HWND hWnd, char *nama);
int FindSignal (HWND hWnd, char *nama);
int FindNode (HWND hWnd, char *nama);
int FindProbe (HWND hWnd, char *nama);
void SetFindType (int type);

UINT BV2FindConnectionDlg (HINSTANCE hInst, HWND hWnd);
int FindConnection (HWND hWnd, char *nama);


#ifdef  __cplusplus
}
#endif


#endif
