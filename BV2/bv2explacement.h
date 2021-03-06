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

#ifndef _SHB_BV2EXPLACEMENT_H_
#define _SHB_BV2EXPLACEMENT_H_

#include <windows.h>
#include "../bv2data/export.h"

#ifdef  __cplusplus
extern "C" {
#endif

void ExPlacementInit ();
void ExPlacementGetOption (_explace_option *optptr);
void ExPlacementSetOptionUnit (int unit);
int ExPlacementIsRefdesOnly ();
LRESULT BV2ExPlacementDlg (HINSTANCE hInst, HWND hWnd);

#ifdef  __cplusplus
}
#endif

#endif
