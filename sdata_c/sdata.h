/*
sData.h

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

/*
Koleksi data dlm pointer
Oleh Sihabul Milah
10-Maret-2006

*** Sejarah ******************
23 maret 2006
tambah "sdata_besardataini"

22 maret 2006
tambah "sdata_jumlahdata"
*/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _SHB_SDATA_H__
#define _SHB_SDATA_H__

#ifdef  __cplusplus
extern "C" {
#endif

#define SDATA_VERSION 0x0100

#define kSDATAERR_NOERROR	0
#define kSDATAERR_NEWEMPTY	-1
#define kSDATAERR_ALLOC	-2
#define kSDATAERR_NOMORE	-3
#define kSDATAERR_NODATA	-4

/*	_data structure
	digunakan oleh _sdata
*/

struct _data {
	struct _data *belakang;
	int besar; /* 10-3-2006 */
	void *isi;
	struct _data *depan;
};
typedef struct _data _data;
typedef _data* _dataptr;

/*	_sdata structure
	digunakan oleh user untuk koleksi data
	deklarasi: _sdata mYdata;
*/
struct _sdata {
	_dataptr kepala;
	_dataptr ini;
	_dataptr ekor;
	int djumlah;
	int dnomor;
};
typedef struct _sdata _sdata; /* structure */
typedef _sdata* _sdataptr;	/* pointer */

/* initial, harus dipanggil pertama */
void	sdata_init 				(_sdataptr sdata);
int		sdata_jumlahdata			(_sdataptr sdata);
/* cetak status ke stdout, hanya jika _SHOWDEBUG */
void	sdata_cetakstatus 		(_sdataptr sdata);

/* navigasi */
int		sdata_nomorini 			(_sdataptr sdata);  /* nomor record (n) */
short	sdata_dikepalakah 		(_sdataptr sdata);	/* test dikepalakah */
short	sdata_diekorkah 		(_sdataptr sdata);	/* test diekorkah */
int		sdata_kekepala 			(_sdataptr sdata);	/* ke record kepala */
int		sdata_keekor 			(_sdataptr sdata);	/* ke record ekor */
/*
sdata_kedepan ()
return ok -> nomor : err -> kSDATAERR_NOMORE
*/
int		sdata_kedepan 			(_sdataptr sdata);	/* ke record depan (n-1) */
int		sdata_kebelakang 		(_sdataptr sdata);	/* ke record belakang (n+1) */
int		sdata_kenomor 			(_sdataptr sdata, int nomor);	/* ke record nomor */

/* manipulasi */
int		sdata_baru 				(_sdataptr sdata, void *data, int besar);
int		sdata_tambah 			(_sdataptr sdata, void *data, int besar);
int		sdata_sisipkepala 		(_sdataptr sdata, void *data, int besar);
int		sdata_sisip 			(_sdataptr sdata, void *data, int besar);
int		sdata_baruiini			(_sdataptr sdata, void *data, int besar);
int		sdata_besardataini		(_sdataptr sdata);
int 	sdata_ambildataini 		(_sdataptr sdata, void *data);
void    *sdata_dataini          (_sdataptr sdata); /* 27 Des 2006 */
int 	sdata_ambildatanomor	(_sdataptr sdata, int nomor, void *data);
int  	sdata_hapuskepala 		(_sdataptr sdata);
int  	sdata_hapusekor 		(_sdataptr sdata);
int 	sdata_hapusini 			(_sdataptr sdata);
void 	sdata_hapussemua 		(_sdataptr sdata);

#ifdef  __cplusplus
}
#endif

#endif
