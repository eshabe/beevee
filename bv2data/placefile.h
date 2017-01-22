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

#ifndef _SHB_PLACEFILE_H__
#define _SHB_PLACEFILE_H__

#include "sboard.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct {
    int column;
    int sline;
    int unit;
    int refdes[2];
    int posx[2];
    int posy[2];
    int posz[2];
    int orient[2];
    int pn[2];
    int shape[2];
    char delimiter[2];
    char token[2];
    char bottom[2];
} importplacement_struct;


int place_baca_file (char *namafile, boardptr bptr, importplacement_struct *ips);

#ifdef  __cplusplus
}
#endif

#endif

