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

#ifndef _SHB_UNITCONV_H__
#define _SHB_UNITCONV_H__

#include "sboard.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define BV2UNIT_MILS    0
#define BV2UNIT_INCHES  1
#define BV2UNIT_MICRON  2
#define BV2UNIT_MM      3

#define inch2mils(a)    (a*1000)
#define mm2mils(a)      (a*1000/25.4)
#define micron2mils(a)  (a/25.4)
#define mils2inch(a)    (a/1000)
#define mils2mm(a)      (a*25.4/1000)
#define mils2micron(a)  (a*25.4)

#ifdef  __cplusplus
}
#endif

#endif

