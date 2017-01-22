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


#ifndef _SHB_BV2_OPTION_H__
#define _SHB_BV2_OPTION_H__

#include <windows.h>

#ifdef  __cplusplus
extern "C" {
#endif

void BV2OptionDefault ();
int BV2OptionDlg (HINSTANCE hInst, HWND hWnd);

BOOL BV2Option_IsZoom ();
BOOL BV2Option_IsBlink ();
BOOL BV2Option_IsLine ();
BOOL BV2Option_IsClear ();
BOOL BV2Option_IsShowObjectInfo ();
BOOL BV2Option_IsEnableObjectInfoBar ();
BOOL BV2Option_IsEnableFindBar ();
BOOL BV2Option_IsEnableFindBarEndBySpace ();
BOOL BV2Option_IsShowTextOfToolBar ();

COLORREF BV2Option_GetBackColorRef ();
COLORREF BV2Option_GetDeviceBColorRef ();
COLORREF BV2Option_GetDeviceTColorRef ();
COLORREF BV2Option_GetLineColorRef ();
COLORREF BV2Option_GetSelectedBkColorRef ();
COLORREF BV2Option_GetSelectedColorRef ();
COLORREF BV2Option_GetTestpinBColorRef ();
COLORREF BV2Option_GetTestpinTColorRef ();
COLORREF BV2Option_GetTraceColorRef (int layer);
COLORREF BV2Option_GetTraceLockColorRef ();
COLORREF BV2Option_GetViaColorRef ();

#ifdef  __cplusplus
}
#endif

#endif /* _SHB_BV2_OPTION_H__ */
