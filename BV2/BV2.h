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

#if !defined(AFX_BV2_H__3A6DF5D7_C3B6_11DA_B46A_50C04BC10000__INCLUDED_)
#define AFX_BV2_H__3A6DF5D7_C3B6_11DA_B46A_50C04BC10000__INCLUDED_


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "resource.h"

#include "../bv2data/sboard.h"

#define BV2_MYVERSION_NUMBER 0x2110
#define BV2_MYTITLE "BeeVee 2.1.2.1b1"
#define BV2_MYVERSION "r2.20100612.1" /* 2.1 */
#define BV2_WINCLASSNAME   "BeeVee"

#ifdef  __cplusplus
extern "C" {
#endif

boardptr GetBVBoardPtr ();
int	EraseWindow (HWND hWnd);
int	RepaintWindow (HWND hWnd);
void WriteBV2AppSettings ();
HWND GettBV2MainWindow ();

BOOL LoadBV2WindowSettings(LPRECT r, BOOL *isMax, const char *name);
void WriteBV2WindowSettings (HWND hWnd, const char *name);


#define DoMergeTrace(a,b,c) DoMergeGenCADTrace(a,b,c)
int DoMergeGenCADTrace (boardptr curbrdptr, boardptr mergedbrdptr, char *signalname);
int DoMergeDevices (boardptr curbrdptr, boardptr mergedbrdptr, char *signalname);

UINT Unit_DialogBox (HINSTANCE hInst, HWND hWnd);

UINT BV2EditDeviceDlg (HINSTANCE hInst, HWND hWnd);

int DoMoveLowerLeftTo (board *bptr, int lower, int left);

#ifdef  __cplusplus
}
#endif

#endif // !defined(AFX_BV2_H__3A6DF5D7_C3B6_11DA_B46A_50C04BC10000__INCLUDED_)
