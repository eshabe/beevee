/*
 *  sdatateks.h


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

 *
 */

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _SHB_SDATATEKS_H_
#define _SHB_SDATATEKS_H_

#include "sdata.h"

#ifdef  __cplusplus
extern "C" {
#endif


typedef _sdata _sdatateks;
typedef _sdataptr _sdatateksptr;

/* sdatateks_jaditeks */
/* menggabungkan data menjadi teks */
/*	in:
		_sdatateksptr (sdata)
		sambungan (penyambung)
	out:
		hasil (teks hasil sambungan)
	return:
		pointer hasil / NULL jika gagal
*/

char *sdatateks_jaditeks (_sdatateksptr stptr, const char *sambungan, char *hasil);
/* sdatateks_dariteks */
/* membuat/menambah isi _sdatatek dari teks */
/* in:
		teks (yang mau dipisah-pisah)
		pemisah (karakter pemisah antar data)
		pj_maks (panjang maksimal data _sdatateksptr)
		bolehkosong (apakah data boleh kosong?)
	out:
		_sdatateksptr
	return:
		jumlah data / -1 jika gagal
*/
int sdatateks_dariteks (_sdatateksptr stptr, const char *teks, char pemisah, int pj_maks, int bolehkosong);

/* sdatateks_nomorini_int */
/* ambil data _sdatateksptr dalam bentuk int(eger) */
/*	in:
		_sdatateksptr (kumpulan data)
		n (data index)
	out:
		angka (atoi dari data nomor n)
	return:
		n / -1 jika gagal
*/
int sdatateks_nomorini_int (_sdatateksptr stptr, int n, int *angka);

/* sdatateks_nomorini_double */
/* ambil data _sdatateksptr dalam bentuk double */
/*	in:
		_sdatateksptr (kumpulan data)
		n (data index)
	out:
		angka (atof dari data nomor n)
	return:
		n / -1 jika gagal
*/
int sdatateks_nomorini_double (_sdatateksptr stptr, int n, double *angka);

/* sdatateks_tambahteks */
/* menambah data teks */
/*  in :
		_sdatateksptr (kumpulan data)
		teks (teks data)
    out:
        jumlah data (n)
    return:
        n / -1 jika gagal
*/
int sdatateks_tambahteks (_sdatateksptr stptr, char *teks);

#ifdef  __cplusplus
}
#endif

#endif /* _SHB_SDATATEKS_H_ */



