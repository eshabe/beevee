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
    http://www.sihab.com
    eshabe@gmail.com
    Perumahan Bina Ummah B-15
    Kibing - Batu Aji
    Kota Batam - 29433
    INDONESIA
*/

#ifndef __SHB_DEVICENDLG_H__
#define __SHB_DEVICENDLG_H__

#ifdef  __cplusplus
extern "C" {
#endif

char *GetDeviceNameDlg_name (char *retname);
UINT DeviceNameDlg_DialogBox (HINSTANCE hInst, HWND hWnd);

#ifdef  __cplusplus
}
#endif

#endif

