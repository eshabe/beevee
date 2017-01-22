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

#ifndef _SHB_BV2_IMPORT_H__
#define _SHB_BV2_IMPORT_H__

#include <windows.h>

#ifdef  __cplusplus
extern "C" {
#endif


#define IMPORT_BEEVEE1			0x01
#define IMPORT_AGILENT3070_A	0x02
#define IMPORT_AGILENT3070_B	0x03
#define IMPORT_AGILENTSJ50		0x04
#define IMPORT_GENRAD228X_A	    0x05
#define IMPORT_GENRAD228X_B	    0x06
#define IMPORT_GENERALXY		0x07
#define IMPORT_GENCAD			0x08
#define IMPORT_GENRAD228X_BRD	0x09
#define IMPORT_PLACEMENT	    0x0A
#define IMPORT_FABMASTER_FAB    0x0B
#define IMPORT_UNICAM_PCB       0x0C
#define IMPORT_GENRAD228X_TAC   0x0D
#define IMPORT_GENRAD228X_TACBRD   0x0E
#define IMPORT_PADPOWERPCB_ASC   0x0F
#define IMPORT_BOMATTRIBUTE_ATR 0x10

int BV2ImportFile (HINSTANCE hInst, HWND hWnd);
int GetImportSource ();

#ifdef  __cplusplus
}
#endif

#endif /* _SHB_BV2_IMPORT_H__ */
