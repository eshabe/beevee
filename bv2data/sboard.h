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

/*
 *  sboard.h
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 3/21/06.
 *
 */

/*
16 Okt 2007
+ board.punya_tempat
+ board_cek_punyatempat
+ komp_xyinfo
+ komp_barui_xyinfo

11 sep 2007
+ board_geser (boardptr bptr, int DX, int DY)

8 des 2006
+ SBOARD_RECT

23 maret 2006
*/

#ifndef _SHB_SBOARD_H_
#define _SHB_SBOARD_H_

#include <stdio.h>
#include "../sdata_c/sdata.h"
#include "../sdata_c/sdatateks.h"
#include "../steks_c/stext.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define	XY_GROW	25
#define	KXY_GROW	25
#define  VIA_GROW 20

#define SBOARD_VERSIONSTR	"2.1.0" /* this is board version not application version */
/* 2.0 - initial release for BeeVee 2 */
/* 2.0.1 - remove Temp Nail from GenRAD data */
/* 2.0.2 - add trace layer (D7) */
/* 2.0.3 - add arc CW (D6) */
/* 2.1.0 - add info -str63- in testpin / net */

#define SBOARD_NAMALEN		63
#define SBOARD_VERSILEN		63
#define SBOARD_ASALLEN		63
#define SBOARD_INFOLEN		255
#define SBOARD_KONEKSILEN	63
#define SBOARD_DATALEN		8
#define SBOARD_TPINFOLEN	63
#define SBOARD_NDINFOLEN	63

#define SBOARD_TRACE_INNER  0
#define SBOARD_TRACE_TOP    1
#define SBOARD_TRACE_BOTTOM 2

#define SBOARD_TRACE_CWF    6
#define SBOARD_TRACE_LAYERF 7

#define SBOARD_CWCLOCKWISE  1
#define SBOARD_CWCOUNTERCLOCKWISE   0


enum _SBOARD_ROUTETYPES {
	SBOARD_VIA = 0,
	SBOARD_LINE,
	SBOARD_ARC,
	SBOARD_RECT /*20061208*/
};
typedef short SBOARD_ROUTETYPE;

enum _SBOARD_ELEMENTS {
	SBOARD_KOMPONEN,
	SBOARD_TESTPIN,
	SBOARD_TRACE
};
typedef int SBOARD_ELEMENT;

typedef struct komponen {
	Str63 nama; /* R1,R2... */
	Str255 info;
	char pos_tb; /* T/B */
	short orentasi;
	int pos_x1;
	int pos_y1;
	int pos_x2;
	int pos_y2;
} komponen;

typedef struct node {
	int id;
	Str63 nama; /* SIGNAL_S,NODE_N,...  */
	Str63 info;
} node;

typedef struct koneksi {
	int node_id;
	Str63 komp_pin; /* R1.1,R1.2,... */
} koneksi;

typedef struct testpoint {
	int node_id;
	char pos_tb;
	int pos_x;
	int pos_y;
	Str63 info; /* NODE_PIN,NAIL_PIN,PROBE */
} testpoint;

typedef struct trace {
	int node_id;
	SBOARD_ROUTETYPE tipe;
	int data[SBOARD_DATALEN];
} trace;

typedef struct board {
	Str63 nama;
	Str63 versi; /*beevee versi_asal*/
	Str63 asal; /*ag3070|genrad28xx|...*/
	int kiri;
	int atas;
	int kanan;
	int bawah;
	_sdata skomp;
	_sdata snode;
	_sdata skonek;
	_sdata stestpoint;
	_sdata strace;
	int punya_nodepin;
	int punya_nailpin;
	int punya_probe;
	int punya_tempat;
} board;
typedef board *boardptr;

void object_putar (int *x1, int *y1, int *x2, int *y2, int derajat);
void object_cermin (int *x1, int *y1, int *x2, int *y2, int X, int Y);
void object_geser (int *x, int *y, int dx, int dy);

void objecttrace_putar (trace *trptr, int derajat);
void objecttrace_cermin (trace *trptr, int X, int Y);
void objecttrace_geser (trace *trptr, int dx, int dy);

void board_init		(boardptr btpr);
void board_hapus		(boardptr bptr);
void board_isinama	(boardptr bptr, char *nama);
void board_isiasal	(boardptr bptr, char *asal);
void board_isiversi	(boardptr bptr, char *versi);
void board_tambahversi	(boardptr bptr, char *versi);
int board_nama				(boardptr bptr, char *nama);
int board_asal				(boardptr bptr, char *asal);
int board_versi			(boardptr bptr, char *versi);
int board_versi_n			(boardptr bptr, char *versi, int n);
void board_hitung_ataskiri (boardptr bptr);
int board_atas (boardptr bptr);
int board_kiri (boardptr bptr);
int board_kanan (boardptr bptr);
int board_bawah (boardptr bptr);
void board_putar (boardptr bptr, int derajat);
void board_cermin (boardptr bptr, int X, int Y);
void board_geser (boardptr bptr, int DX, int DY);
int board_punyatempat (boardptr bptr);
int board_cek_punyatempat (boardptr bptr);

/* komponen aka component / device */
int komp_tambah				(_sdataptr sptr, const char *n, const char *i, char pt, short ori, int px1, int py1, int px2, int py2);
int komp_cari				(_sdataptr sptr, const char *n, int mulai);
int komp_cari_xy			(_sdataptr sptr, int x, int y, int mulai);
int komp_barui_info			(_sdataptr sptr, const char *info);
int komp_nama_barui_info	(_sdataptr sptr, const char *nama, const char *info);
int komp_tambahi_info		(_sdataptr sptr, const char *info);
int komp_tambahi_xy			(_sdataptr sptr, int x1, int y1, int x2, int y2);
int komp_nama_tambahi_xy	(_sdataptr sptr, const char *nama, int x1, int y1, int x2, int y2);
int komp_barui_xy			(_sdataptr sptr, int x1, int y1, int x2, int y2);
int komp_barui_xyinfo       (_sdataptr sptr, int x, int y); /* shb: 16 Oktober 2007 */
int komp_nama_barui_xy		(_sdataptr sptr, const char *nama, int x1, int y1, int x2, int y2);
int komp_barui_tb			(_sdataptr sptr, char tb);
int komp_nama_barui_tb		(_sdataptr sptr, const char *nama, char tb);
int komp_barui_orentasi		(_sdataptr sptr, short ori);
int komp_nama_barui_orentasi (_sdataptr sptr, const char *nama, short ori);
int komp_nama				(_sdataptr sptr, char *nama);
int komp_info				(_sdataptr sptr, char *i);
int komp_tb					(_sdataptr sptr, char *tb);
int komp_orentasi			(_sdataptr sptr, short *ori);
int komp_xy					(_sdataptr sptr, int *x1, int *y1, int *x2, int *y2);
int komp_xyinfo             (_sdataptr sptr, int *x, int *y); /* shb: 16 Oktober 2007 */
void komp_tampildata		(_sdataptr sptr, FILE *f);
void komp_tampilsemua		(_sdataptr sptr, FILE *f);

/* node aka signal name */
int node_tambah			(_sdataptr sptr, const char *n, const char *i);
int node_tambahi_info	(_sdataptr sptr, const char *info);
int node_id				(_sdataptr sptr);
int node_nama			(_sdataptr sptr, char *nama);
int node_info			(_sdataptr sptr, char *info);
int node_cari			(_sdataptr sptr, const char *n, int mulai);
int node_caridiinfo		(_sdataptr sptr, const char *inf, int mulai);
int node_takadadinet	(_sdataptr snodptr, _sdataptr _stestpointptr, char *hasil);
void node_tampildata	(_sdataptr sptr, FILE *f);
void node_tampilsemua	(_sdataptr sptr, FILE *f);
int node_barui_info		(_sdataptr sptr, char *info);

/* koneksi aka connection / route */
int koneksi_tambah			(_sdataptr sptr, int id, const char *konek);
int koneksi_nodeid		(_sdataptr sptr);
int koneksi_komppin		(_sdataptr sptr, char *komppin);
int koneksi_carijangkau (_sdataptr sptr, int id, const char *n, int mulai, int akhir); /* 21 Feb 2007 */
int koneksi_cari			(_sdataptr sptr, int id, const char *n, int mulai); /* id=-1|n="*" => abaikan n=".zz" => cari yg awalannya zz */
int koneksi_cari_dkonek		(_sdataptr sptr, const char *n, int *id, int mulai);
void koneksi_tampildata		(_sdataptr sptr, FILE *f);
void koneksi_tampilsemua	(_sdataptr sptr, FILE *f);

/* testpoint aka testpin*/
/* int tpt_tambah (_sdataptr sptr, int nid, char tb, int x1, int y1, int x2, int y2); */
//int tpt_tambah (_sdataptr sptr, int nid, char tb, int x, int y);
int tpt_tambah (_sdataptr sptr, int nid, char tb, int x, int y, char *info);
int tpt_cari (_sdataptr sptr, int id, int mulai);
int tpt_cari_xy (_sdataptr sptr, int x, int y, int mulai);
int tpt_caridiinfo (_sdataptr sptr, const char *inf, int mulai, int *node_id);
void tpt_tampildata (_sdataptr sptr, FILE *f);
void tpt_tampilsemua (_sdataptr sptr, FILE *f);
int tpt_id (_sdataptr sptr, int *nid);
int tpt_tb (_sdataptr sptr, char *tb);
int tpt_xy (_sdataptr sptr, int *x, int *y);
int tpt_barui_xy (_sdataptr sptr, int x, int y);
int tpt_barui_info (_sdataptr sptr, char *info);
int tpt_tambahi_info (_sdataptr sptr, const char *info);
void tpt_urut (_sdataptr sptr);
void tpt_cekpin (_sdataptr sptr, int *ndp, int *nlp, int *pb);

/* trace aka route (line) */
int trace_tambah (_sdataptr sptr, int nid, SBOARD_ROUTETYPE tipe, int *data, int datalen);
int trace_barui (_sdataptr sptr, int nid, SBOARD_ROUTETYPE tipe, int *data, int datalen);
int trace_cari (_sdataptr sptr, int id, int mulai);
void trace_tampildata (_sdataptr sptr, FILE *f);
void trace_tampilsemua (_sdataptr sptr, FILE *f);

#ifndef min
#define min(a,b) (((a)<(b))?a:b)
#endif
#ifndef max
#define max(a,b) (((a)>(b))?a:b)
#endif

#ifdef  __cplusplus
}
#endif

#endif
