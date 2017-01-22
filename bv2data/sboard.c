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
eshabe
4 maret 2006

board file u/ beevee2
menggunakan koordinat real (bukan graphics device)

  +-----------+ (x2,y2) atau (max(x), max(y)) --> board(kanan,atas)
  |           |
  |           |
  +-----------+
  (x1,y1) atau (min(x), min(y)) --> board(kiri,bawah)


board informasi standar
PN partnumber
SHA shape
VAL value
TOL tolerance
XY placement (will not be manipulated)

29 september 2007 - 20070929
- buang penyaratan board_hitung_ataskiri()

11 september 2007
+ void board_geser (boardptr bptr, int DX, int DY)

4 agutus 2007
- testpoint -> tpt
- pindah info node (nail,node,probe) ke tpt

8 desember 2006
- berdasar perubahan struct sboard

7 desember 2006
dalam xxx_cari, mulai = -1 --> sdata_nomorini

17 mei 2006
usahakan kompare x1y1
x1y1 selalu yg minimum

hindari akses data langsung dari luar sboard
24 maret 2006
23 maret 2006

*/

//#define _SBOARD_DEVELOPMENT_
//#define _SBOARD_VERBOSE_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sboard.h"

void board_default (boardptr bptr)
{
	strcpy (bptr->nama,"tak-dikenal");
	strcpy (bptr->versi, SBOARD_VERSIONSTR);
	bptr->asal[0] = 0;
	bptr->kiri = 0;
	bptr->atas = 0;
	bptr->kanan = 0;
	bptr->bawah = 0;
	bptr->punya_nodepin = 0;
	bptr->punya_nailpin = 0;
	bptr->punya_probe = 0;
	bptr->punya_tempat = 0;
}

void board_init (boardptr bptr)
{
	board_default(bptr);
	sdata_init(&bptr->skomp);
	sdata_init(&bptr->snode);
	sdata_init(&bptr->skonek);
	sdata_init(&bptr->stestpoint);
	sdata_init(&bptr->strace);
}

void board_hapus (boardptr bptr)
{
	board_default(bptr);
	sdata_hapussemua(&bptr->skomp);
	sdata_hapussemua(&bptr->snode);
	sdata_hapussemua(&bptr->skonek);
	sdata_hapussemua(&bptr->stestpoint);
	sdata_hapussemua(&bptr->strace);
}

void board_isinama (boardptr bptr, char *nama)
{
	// 25 Juni 2007
	// Antisipasi illegal characters yg mengakibatkan masalah nama file
	RemoveChar (nama, '\\');
	RemoveChar (nama, '/');
	RemoveChar (nama, ':');
	RemoveChar (nama, '*');
	RemoveChar (nama, '"');
	RemoveChar (nama, '<');
	RemoveChar (nama, '>');
	RemoveChar (nama, '|');
	RemoveChar (nama, '?');
	// -end
	strncpy(bptr->nama,nama,SBOARD_NAMALEN);
	bptr->nama[SBOARD_NAMALEN]=0;
}

void board_isiasal (boardptr bptr, char *asal)
{
	strncpy(bptr->asal,asal,63);
	bptr->asal[63]=0;
}

void board_isiversi (boardptr bptr, char *versi)
{
	strncpy(bptr->versi,versi,63);
	bptr->versi[63]=0;
}

void board_tambahversi (boardptr bptr, char *versi)
{
	char vrs[64];
	if ((strlen(versi)+strlen(bptr->versi)+1)>63)
		strncpy(vrs,versi, 63-(strlen(bptr->versi)+1));
	else
		strncpy(vrs, versi, 63);
	strcat(bptr->versi, " ");
	strcat(bptr->versi,vrs);
	bptr->versi[63]=0;
}

int board_nama (boardptr bptr, char *nama)
{
	strcpy(nama, bptr->nama);
	return ((bptr->nama[0])?1:0);
}

int board_asal (boardptr bptr, char *asal)
{
	strcpy(asal, bptr->asal);
	return ((bptr->asal[0])?1:0);
}

int board_versi (boardptr bptr, char *versi)
{
	strcpy(versi, bptr->versi);
	return ((bptr->versi[0])?1:0);
}

int board_versi_n (boardptr bptr, char *versi, int n)
{
	char vrs[64];
	int i,j;

	strcpy(vrs, bptr->versi);
	j=0;
	for (i=0;i<n;i++)
	{
		versi[0]=0;
		GetTextUntilAChar(vrs, versi, 63, ' ', &j);
	}
	ReplaceChar (versi, ' ', 0);
	return ((bptr->versi[0])?1:0);
}

void board_hitung_ataskiri (boardptr bptr)
{
	int x1,y1,x2,y2;

	/* komponen */
	if (sdata_kekepala(&bptr->skomp)>=0)
	{
		/* inisialisasi */
		komp_xy(&bptr->skomp, &x1, &y1, &x2, &y2);
		bptr->kiri = x1;
		bptr->atas = y1;
		bptr->kanan = x2;
		bptr->bawah = y2;

		while(1)
		{
			komp_xy(&bptr->skomp, &x1, &y1, &x2, &y2);
			//if ((x1+y1+x2+y2)!=(4*x1)) // 20070929
			{
				bptr->kiri = min(bptr->kiri,x1);
				bptr->atas = max(bptr->atas,y1);
				bptr->kanan = max(bptr->kanan,x1);
				bptr->bawah = min(bptr->bawah,y1);

				bptr->kiri = min(bptr->kiri,x2);
				bptr->atas = max(bptr->atas,y2);
				bptr->kanan = max(bptr->kanan,x2);
				bptr->bawah = min(bptr->bawah,y2);
			}
			if (sdata_diekorkah(&bptr->skomp))
				break;
			sdata_kedepan(&bptr->skomp);
		}
	}

	/* testpoint (testpin) */
	if (sdata_kekepala(&bptr->stestpoint)>=0)
	{
		/* inisialisasi, jika tak ada komponen (testpin thoq!) */
		if (sdata_jumlahdata(&bptr->skomp)==0)
		{
			tpt_xy(&bptr->stestpoint, &x1, &y1);
			bptr->kiri = x1;
			bptr->atas = y1;
			bptr->kanan = x1;
			bptr->bawah = y1;
		}

		while(1)
		{
			tpt_xy(&bptr->stestpoint, &x1, &y1);
			bptr->kiri = min(bptr->kiri,x1);
			bptr->atas = max(bptr->atas,y1);
			bptr->kanan = max(bptr->kanan,x1);
			bptr->bawah = min(bptr->bawah,y1);
			if (sdata_diekorkah(&bptr->stestpoint))
				break;
			sdata_kedepan(&bptr->stestpoint);
		}
	}

	/* trace */
	if (sdata_kekepala(&bptr->strace)>=0)
	{
		while(1)
		{
			trace tr;
			sdata_ambildataini(&bptr->strace, &tr);
			switch (tr.tipe)
			{
			case SBOARD_VIA:
			case SBOARD_LINE:
			case SBOARD_RECT:
			case SBOARD_ARC:
				bptr->kiri = min(bptr->kiri,tr.data[0]);
				bptr->bawah = min(bptr->bawah,tr.data[1]);
				bptr->atas = max(bptr->atas,tr.data[1]);
				bptr->kanan = max(bptr->kanan,tr.data[0]);
				if (tr.tipe == SBOARD_VIA)
					break;
				bptr->kiri = min(bptr->kiri,tr.data[2]);
				bptr->bawah = min(bptr->bawah,tr.data[3]);
				bptr->atas = max(bptr->atas,tr.data[3]);
				bptr->kanan = max(bptr->kanan,tr.data[2]);
				if (tr.tipe == SBOARD_LINE || tr.tipe == SBOARD_RECT)
					break;
				bptr->kiri = min(bptr->kiri,tr.data[4]);
				bptr->bawah = min(bptr->bawah,tr.data[5]);
				bptr->atas = max(bptr->atas,tr.data[5]);
				bptr->kanan = max(bptr->kanan,tr.data[4]);
				break;
			}
			if (sdata_diekorkah(&bptr->strace))
				break;
			sdata_kedepan(&bptr->strace);
		}
	}

}

int board_atas (boardptr bptr)
{
	return bptr->atas;
}

int board_kiri (boardptr bptr)
{
		return bptr->kiri;
}

int board_kanan (boardptr bptr)
{
	return bptr->kanan;
}

int board_bawah (boardptr bptr)
{
	return bptr->bawah;
}

int board_punyatempat (boardptr bptr)
{
    return bptr->punya_tempat;
}

int board_cek_punyatempat (boardptr bptr)
{
    int i;
    Str255 info;

    bptr->punya_tempat = 0;
    for (i=0;i<sdata_jumlahdata(&bptr->skomp);i++)
    {
        sdata_kenomor (&bptr->skomp, i);
        komp_info (&bptr->skomp, info);
        if (strstr(info,"XY="))
        {
            bptr->punya_tempat = 1;
            return 1;
        }
    }
    return 0;
}

void object_putar (int *x1, int *y1, int *x2, int *y2, int derajat)
{
	int X1 = min(*x1,*x2);
	int Y1 = min(*y1,*y2);
	int X2 = max(*x1,*x2);
	int Y2 = max(*y1,*y2);

	if ((derajat%360)==90)
	{
		*x1 = -Y2;
		*x2 = -Y1;
		*y1 = X1;
		*y2 = X2;
	}
	else if ((derajat%360)==180)
	{
		*x1 = -X2;
		*x2 = -X1;
		*y1 = -Y2;
		*y2 = -Y1;
	}
	else if ((derajat%360)==270)
	{
		*x1 = Y1;
		*x2 = Y2;
		*y1 = -X2;
		*y2 = -X1;
	}
}

void objecttrace_putar (trace *trptr, int derajat)
{

	switch (trptr->tipe)
	{
	case SBOARD_VIA:
		object_putar (&trptr->data[0], &trptr->data[1],
			&trptr->data[0], &trptr->data[1], derajat);
		break;

	case SBOARD_LINE:
	case SBOARD_RECT:
		object_putar (&trptr->data[0], &trptr->data[1],
			&trptr->data[0], &trptr->data[1], derajat);
		object_putar (&trptr->data[2], &trptr->data[3],
			&trptr->data[2], &trptr->data[3], derajat);
		break;
	case SBOARD_ARC:
		object_putar (&trptr->data[0], &trptr->data[1],
			&trptr->data[0], &trptr->data[1], derajat);
		object_putar (&trptr->data[2], &trptr->data[3],
			&trptr->data[2], &trptr->data[3], derajat);
		object_putar (&trptr->data[4], &trptr->data[5],
			&trptr->data[4], &trptr->data[5], derajat);
		break;
	}
}

void board_putar (boardptr bptr, int derajat)
{
	int x1,y1,x2,y2;
	/*20061208*/
	komponen komp;
	Str255 info;

	if (derajat==0 || (derajat%360)==0)
		return;

	/* komponen */
	if (sdata_kekepala(&bptr->skomp)>=0)
	{
		while(1)
		{
			/*20061208
			komp_xy(&bptr->skomp, &x1, &y1, &x2, &y2);
			object_putar(&x1, &y1, &x2, &y2, derajat);
			komp_barui_xy (&bptr->skomp, x1, y1, x2, y2);
			*/
			/*20061208*/
			sdata_ambildataini(&bptr->skomp, &komp);
			object_putar(&komp.pos_x1, &komp.pos_y1, &komp.pos_x2, &komp.pos_y2, derajat);
			komp.orentasi += derajat;
			komp.orentasi = komp.orentasi%360;
			sdata_baruiini (&bptr->skomp, &komp, sizeof(komponen));

			/* 20071016 */
			komp_xyinfo (&bptr->skomp, &x1, &y1);
			object_putar (&x1, &y1, &x1, &y1, derajat);
			komp_barui_xyinfo (&bptr->skomp, x1, y1);

			if (sdata_diekorkah(&bptr->skomp))
				break;
			sdata_kedepan(&bptr->skomp);
		}
	}

	/* testpoint (testpin) */
	if (sdata_kekepala(&bptr->stestpoint)>=0)
	{
		while(1)
		{
			tpt_xy(&bptr->stestpoint, &x1, &y1);
			object_putar (&x1, &y1, &x1, &y1, derajat);
			tpt_barui_xy (&bptr->stestpoint, x1, y1);
			if (sdata_diekorkah(&bptr->stestpoint))
				break;
			sdata_kedepan(&bptr->stestpoint);
		}
	}

	/* trace */
	if (sdata_kekepala(&bptr->strace)>=0)
	{
		while(1)
		{
			trace tr;
			sdata_ambildataini (&bptr->strace, &tr);
			objecttrace_putar (&tr, derajat);
			trace_barui (&bptr->strace, tr.node_id, tr.tipe,
				tr.data, SBOARD_DATALEN);
			if (sdata_diekorkah(&bptr->strace))
				break;
			sdata_kedepan(&bptr->strace);
		}
	}
}

void object_cermin (int *x1, int *y1, int *x2, int *y2, int X, int Y)
{
	X = (X<0)?-1:1;
	Y = (Y<0)?-1:1;

	*x1 *= X;
	*x2 *= X;
	*y1 *= Y;
	*y2 *= Y;
}

void objecttrace_cermin (trace *trptr, int X, int Y)
{
	X = (X<0)?-1:1;
	Y = (Y<0)?-1:1;

	switch (trptr->tipe)
	{
	case SBOARD_VIA:
		trptr->data[0] *= X;
		trptr->data[1] *= Y;
		break;
	case SBOARD_LINE:
	case SBOARD_RECT:
		trptr->data[0] *= X;
		trptr->data[1] *= Y;
		trptr->data[2] *= X;
		trptr->data[3] *= Y;
		break;
	case SBOARD_ARC:
		trptr->data[0] *= X;
		trptr->data[1] *= Y;
		trptr->data[2] *= X;
		trptr->data[3] *= Y;
		trptr->data[4] *= X;
		trptr->data[5] *= Y;
		trptr->data[SBOARD_TRACE_CWF] = !trptr->data[SBOARD_TRACE_CWF];
		break;
	}
}

void board_cermin (boardptr bptr, int X, int Y)
{
	int x1,y1,x2,y2;

	/* komponen */
	if (sdata_kekepala(&bptr->skomp)>=0)
	{
		while(1)
		{
			komp_xy(&bptr->skomp, &x1, &y1, &x2, &y2);
			object_cermin(&x1, &y1, &x2, &y2, X, Y);
			komp_barui_xy (&bptr->skomp, x1, y1, x2, y2);

			/* 20071016 */
			komp_xyinfo (&bptr->skomp, &x1, &y1);
			x2 = y2 = 0;
			object_cermin(&x1, &y1, &x2, &y2, X, Y);
			komp_barui_xyinfo (&bptr->skomp, x1, y1);

			if (sdata_diekorkah(&bptr->skomp))
				break;
			sdata_kedepan(&bptr->skomp);
		}
	}

	/* testpoint (testpin) */
	if (sdata_kekepala(&bptr->stestpoint)>=0)
	{
		while(1)
		{
			tpt_xy(&bptr->stestpoint, &x1, &y1);
			x2 = y2 = 0;
			object_cermin (&x1, &y1, &x2, &y2, X, Y);
			tpt_barui_xy (&bptr->stestpoint, x1, y1);
			if (sdata_diekorkah(&bptr->stestpoint))
				break;
			sdata_kedepan(&bptr->stestpoint);
		}
	}

	/* trace */
	if (sdata_kekepala(&bptr->strace)>=0)
	{
		while(1)
		{
			trace tr;
			sdata_ambildataini (&bptr->strace, &tr);
			objecttrace_cermin (&tr, X, Y);
			trace_barui (&bptr->strace, tr.node_id, tr.tipe,
				tr.data, SBOARD_DATALEN);
			if (sdata_diekorkah(&bptr->strace))
				break;
			sdata_kedepan(&bptr->strace);
		}
	}
}

void object_geser (int *x, int *y, int dx, int dy)
{
    *x+=dx;
    *y+=dy;
}

void objecttrace_geser (trace *trptr, int dx, int dy)
{
	switch (trptr->tipe)
	{
	case SBOARD_VIA:
		trptr->data[0] += dx;
		trptr->data[1] += dy;
		break;
	case SBOARD_LINE:
	case SBOARD_RECT:
		trptr->data[0] += dx;
		trptr->data[1] += dy;
		trptr->data[2] += dx;
		trptr->data[3] += dy;
		break;
	case SBOARD_ARC:
		trptr->data[0] += dx;
		trptr->data[1] += dy;
		trptr->data[2] += dx;
		trptr->data[3] += dy;
		trptr->data[4] += dx;
		trptr->data[5] += dy;
		break;
	}
}

void board_geser (boardptr bptr, int DX, int DY)
{
	int x1,y1,x2,y2;

	/* komponen */
	if (sdata_kekepala(&bptr->skomp)>=0)
	{
		while(1)
		{
			komp_xy(&bptr->skomp, &x1, &y1, &x2, &y2);
			object_geser(&x1, &y1, DX, DY);
			object_geser(&x2, &y2, DX, DY);
			komp_barui_xy (&bptr->skomp, x1, y1, x2, y2);

			/* 20071016 */
			komp_xyinfo (&bptr->skomp, &x1, &y1);
			object_geser(&x1, &y1, DX, DY);
			komp_barui_xyinfo (&bptr->skomp, x1, y1);

			if (sdata_diekorkah(&bptr->skomp))
				break;
			sdata_kedepan(&bptr->skomp);
		}
	}

	/* testpoint (testpin) */
	if (sdata_kekepala(&bptr->stestpoint)>=0)
	{
		while(1)
		{
			tpt_xy(&bptr->stestpoint, &x1, &y1);
			object_geser (&x1, &y1, DX, DY);
			tpt_barui_xy (&bptr->stestpoint, x1, y1);
			if (sdata_diekorkah(&bptr->stestpoint))
				break;
			sdata_kedepan(&bptr->stestpoint);
		}
	}

	/* trace */
	if (sdata_kekepala(&bptr->strace)>=0)
	{
		while(1)
		{
			trace tr;
			sdata_ambildataini (&bptr->strace, &tr);
			objecttrace_geser (&tr, DX, DY);
			trace_barui (&bptr->strace, tr.node_id, tr.tipe,
				tr.data, SBOARD_DATALEN);
			if (sdata_diekorkah(&bptr->strace))
				break;
			sdata_kedepan(&bptr->strace);
		}
	}
}

void board_tampilinfo (boardptr bptr)
{
	printf ("*BOARD INFO*\nnama: %s\nversi: %s\nasal: %s\n", bptr->nama, bptr->versi, bptr->asal);
	printf ("komponen: %d\nnode: %d\nkoneksi: %d\nnet(tp): %d\ntrace: %d\n",
		sdata_jumlahdata(&bptr->skomp), sdata_jumlahdata(&bptr->snode), sdata_jumlahdata(&bptr->skonek),
		sdata_jumlahdata(&bptr->stestpoint), sdata_jumlahdata(&bptr->strace));
	printf ("kiri,bawah: %d,%d\nkanan,atas: %d,%d\n", bptr->kiri, bptr->bawah, bptr->kanan, bptr->atas);
	printf ("nodepin : %d\nnailpin: %d\nprobe : %d\n", bptr->punya_nodepin, bptr->punya_nailpin, bptr->punya_probe);
	printf ("*END*\n");
}

/* KOMPONEN */
int komp_tambah (_sdataptr sptr, const char *n, const char *i, char pt, short or, int px1, int py1, int px2, int py2)
{
	komponen komp;

	memset (&komp, 0, sizeof(komponen));

	if (strlen(n)>=1)
	{
		strncpy (komp.nama, n, SBOARD_NAMALEN);
		strncpy (komp.info, i, SBOARD_INFOLEN);

		komp.pos_tb = pt;
		komp.orentasi = or;
		komp.pos_x1 = min(px1,px2);
		komp.pos_y1 = min(py1,py2);
		komp.pos_x2 = max(px1,px2);
		komp.pos_y2 = max(py1,py2);

		//printf ("* %d\n", sizeof(komponen));

		return sdata_tambah(sptr, &komp, sizeof(komponen));
	}
	return -1;
}

int komp_nama (_sdataptr sptr, char *n)
{
	komponen komp;
	if (sdata_ambildataini(sptr, &komp)>=0)
	{
		strcpy (n, komp.nama);
		return sdata_nomorini(sptr);
	}
	return -1;
}


int komp_info (_sdataptr sptr, char *i)
{
	komponen komp;
	if (sdata_ambildataini(sptr, &komp)>=0)
	{
		strcpy (i, komp.info);
		return sdata_nomorini(sptr);
	}
	return -1;
}

int komp_tb (_sdataptr sptr, char *tb)
{
	komponen komp;
	if (sdata_ambildataini(sptr, &komp)>=0)
	{
		*tb =  komp.pos_tb;
		return sdata_nomorini(sptr);
	}
	return -1;
}

int komp_orentasi (_sdataptr sptr, short *or)
{
	komponen komp;
	if (sdata_ambildataini(sptr, &komp)>=0)
	{
		*or = komp.orentasi;
		return sdata_nomorini(sptr);
	}
	return -1;
}

int komp_xy (_sdataptr sptr, int *x1, int *y1, int *x2, int *y2)
{
	komponen komp;
	if (sdata_ambildataini(sptr, &komp)>=0)
	{
		*x1 = komp.pos_x1;
		*y1 = komp.pos_y1;
		*x2 = komp.pos_x2;
		*y2 = komp.pos_y2;
		return sdata_nomorini(sptr);
	}
	return -1;
}

int komp_xyinfo (_sdataptr sptr, int *x, int *y)
{
	komponen komp;
	char *xy;
	Str63 temp,temp2;
	int p = 0;
	if (sdata_ambildataini(sptr, &komp)>=0)
	{
		if (GetTextStartAtText (komp.info, temp, Str63Len, "XY=", 0)<0)
            return -1;
        GetTextUntilAChar (temp, temp2, Str63Len, ' ', &p);
        ReplaceChar (temp2, '=',' ');
        ReplaceChar (temp2, ':',' ');
        sscanf (temp2,"%s %d %d", temp, x, y);
		return sdata_nomorini(sptr);
	}
	return -1;
}

int komp_cari (_sdataptr sptr, const char *n, int mulai)
{
	komponen komp;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &komp);
		if (strcmp(komp.nama,n)==0)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &komp);
		if (strcmp(komp.nama,n)==0)
			return sdata_nomorini(sptr);
		sdata_kedepan(sptr);
	}

	return -1;
}

int komp_cari_xy (_sdataptr sptr, int x, int y, int mulai)
{
	komponen komp;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &komp);
		if (x>=komp.pos_x1 && x<=komp.pos_x2 && y>=komp.pos_y1 && y<=komp.pos_y2)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &komp);
		if (x>=komp.pos_x1 && x<=komp.pos_x2 && y>=komp.pos_y1 && y<=komp.pos_y2)
			return sdata_nomorini(sptr);
		sdata_kedepan(sptr);
	}

	return -1;
}


int komp_barui_info (_sdataptr sptr, const char *info)
{
	komponen komp;
	int len = strlen(info);

	if (len>SBOARD_INFOLEN)
        len = SBOARD_INFOLEN;
	if (sdata_ambildataini (sptr, &komp)>=0)
	{
		strncpy (komp.info, info,len);
		return (sdata_baruiini(sptr, &komp, sizeof (komponen)));
	}
	return -1;
}


int komp_nama_barui_info (_sdataptr sptr, const char *nama, const char *info)
{
	if (komp_cari(sptr, nama, -1)>=0)
	{
		return komp_barui_info (sptr, info);
	}
	return -1;
}

int komp_tambahi_info (_sdataptr sptr, const char *info)
{
	komponen komp;
	Str255 info2;

    if (sdata_ambildataini (sptr, &komp)>=0)
	{
        if (strlen(komp.info)>=SBOARD_INFOLEN) // eshabe : [2010-06-12] : fix adding device information / note
            return -1;
		if ((strlen(info)+strlen(komp.info)+1)>SBOARD_INFOLEN)
			strncpy(info2,info, SBOARD_INFOLEN-(strlen(komp.info)+1));
		else
			strncpy(info2, info, SBOARD_INFOLEN);

		if (komp.info[0])
			strcat(komp.info, " ");
		strcat(komp.info,info2);
		komp.info[SBOARD_INFOLEN] = 0;
		return (sdata_baruiini(sptr, &komp, sizeof (komponen)));
	}
	return -1;
}

int komp_barui_xy (_sdataptr sptr, int x1, int y1, int x2, int y2)
{
	komponen komp;
	if (sdata_ambildataini (sptr, &komp)>=0)
	{
		komp.pos_x1 = min(x1,x2);
		komp.pos_y1 = min(y1,y2);
		komp.pos_x2 = max(x1,x2);
		komp.pos_y2 = max(y1,y2);

		return (sdata_baruiini(sptr, &komp, sizeof (komponen)));
	}

	return -1;
}

int komp_barui_xyinfo (_sdataptr sptr, int x, int y)
{
	komponen komp;
	_sdatateks steks;
	Str63 txt;
	int i,ex;
	if (sdata_ambildataini (sptr, &komp)>=0)
	{
        if (strstr(komp.info,"XY=")==NULL)
            return -1;
        sdata_init (&steks);
        sdatateks_dariteks(&steks, komp.info, ' ',Str63Len, 1);
        ex = 0;
        for (i=0;i<sdata_jumlahdata(&steks);i++)
        {
            sdata_ambildatanomor (&steks, i, txt);
            if (strstr(txt,"XY="))
            {
                sprintf(txt,"XY=%d:%d",x,y);
                sdata_baruiini (&steks, txt, strlen(txt)+1);
                ex = 1;
                break;
            }
        }
        if (ex)
            sdatateks_jaditeks (&steks, " ", komp.info);
        sdata_hapussemua(&steks);
        if (ex)
            return (sdata_baruiini(sptr, &komp, sizeof (komponen)));
	}

	return -1;
}


int komp_nama_barui_xy (_sdataptr sptr, const char *nama, int x1, int y1, int x2, int y2)
{
	if (komp_cari(sptr, nama, -1)>=0)
	{
		return komp_barui_xy (sptr, x1, y1, x2, y2);
	}

	return -1;
}

int komp_barui_tb (_sdataptr sptr, char tb)
{
	komponen komp;
	if (sdata_ambildataini (sptr, &komp)>=0)
	{
		komp.pos_tb = tb;
		return (sdata_baruiini(sptr, &komp, sizeof (komponen)));
	}
	return -1;
}

int komp_nama_barui_tb (_sdataptr sptr, const char *nama, char tb)
{
	if (komp_cari(sptr, nama, -1)>=0)
		return komp_barui_tb (sptr, tb);

	return -1;
}

int komp_barui_orentasi (_sdataptr sptr, short or)
{
	komponen komp;
	if (sdata_ambildataini (sptr, &komp)>=0)
	{
		komp.orentasi = or;
		return (sdata_baruiini(sptr, &komp, sizeof (komponen)));
	}
	return -1;
}

int komp_nama_barui_orentasi (_sdataptr sptr, const char *nama, short or)
{
	if (komp_cari(sptr, nama, -1)>=0)
		return komp_barui_orentasi (sptr, or);

	return -1;
}

int komp_tambahi_xy (_sdataptr sptr, int x1, int y1, int x2, int y2)
{
	komponen komp;
	if (sdata_ambildataini (sptr, &komp)>=0)
	{
		if (komp.pos_x1==0 &&
			komp.pos_x2==0 &&
			komp.pos_y1==0 &&
			komp.pos_y2==0)
		{
			komp.pos_x1 = min(x1,x2);
			komp.pos_y1 = min(y1,y2);
			komp.pos_x2 = max(x1,x2);
			komp.pos_y2 = max(y1,y2);
		}
		else
		{
			komp.pos_x1 = min(min(x1,x2), komp.pos_x1);
			komp.pos_y1 = min(min(y1,y2), komp.pos_y1);
			komp.pos_x2 = max(max(x1,x2), komp.pos_x2);
			komp.pos_y2 = max(max(y1,y2), komp.pos_y2);
		}

		// it is a point
		/*
		if (komp.pos_x1==komp.pos_x2 && komp.pos_y1==komp.pos_y2)
		{
			komp.pos_x1-=KXY_GROW;
			komp.pos_x2+=KXY_GROW;
			komp.pos_y1-=KXY_GROW;
			komp.pos_y2+=KXY_GROW;
		}
		*/
		if (komp.pos_x1==komp.pos_x2)
		{
			komp.pos_x1-=KXY_GROW;
			komp.pos_x2+=KXY_GROW;
		}
		if (komp.pos_y1==komp.pos_y2)
		{
			komp.pos_y1-=KXY_GROW;
			komp.pos_y2+=KXY_GROW;
		}
		return (sdata_baruiini(sptr, &komp, sizeof (komponen)));
	}

	return -1;
}

int komp_nama_tambahi_xy (_sdataptr sptr, const char *nama, int x1, int y1, int x2, int y2)
{
	if (komp_cari(sptr, nama, -1)>=0)
	{
		return komp_tambahi_xy (sptr, x1, y1, x2, y2);
	}

	return -1;
}

void komp_tampildata (_sdataptr sptr, FILE *f)
{
	komponen komp;
	if (sdata_ambildataini(sptr, &komp)>=0)
		fprintf (f, "%s!%c!%d!%d!%d!%d!%d!%s\n",
			komp.nama, komp.pos_tb, komp.orentasi, komp.pos_x1, komp.pos_y1, komp.pos_x2, komp.pos_y2, komp.info);
}


void komp_tampilsemua (_sdataptr sptr, FILE *f)
{
	if (sdata_kekepala(sptr)<0)
		return;
	fprintf (f, "#REFDES!TOP-BOTTOM!ORIENT!X1!Y1!X2!Y2!INFO\n");
	while (1)
	{
		komp_tampildata(sptr, f);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
}

/* NODE */
int node_tambah (_sdataptr sptr, const char *n, const char *i)
{
	node nd;

	memset (&nd, 0, sizeof(node));
	nd.id = sdata_jumlahdata(sptr);
	strncpy (nd.nama, n, SBOARD_NAMALEN);
	strncpy (nd.info, i, SBOARD_NDINFOLEN);
	return sdata_tambah(sptr, &nd, sizeof(node));
}

int node_id (_sdataptr sptr)
{
	node nd;

	if (sdata_ambildataini(sptr, &nd)>=0)
		return nd.id;
	return -1;
}

int node_nama (_sdataptr sptr, char *nama)
{
	node nd;

	if (sdata_ambildataini(sptr, &nd)>=0)
	{
		strcpy (nama, nd.nama);
		return 1;
	}
	return 0;
}

int node_info (_sdataptr sptr, char *info)
{
	node nd;

	if (sdata_ambildataini(sptr, &nd)>=0)
	{
		strcpy (info, nd.info);
		return 1;
	}
	return 0;
}

int node_barui_info (_sdataptr sptr, char *info)
{
	node nd;

	sdata_ambildataini (sptr, &nd);
	strncpy (nd.info, info, SBOARD_NDINFOLEN);
	nd.info[SBOARD_NDINFOLEN]=0;
	return sdata_baruiini(sptr, &nd, sizeof(node));
}


int node_tambahi_info (_sdataptr sptr, const char *info)
{
	node nd;
	char info2[SBOARD_INFOLEN+1];
	if (sdata_ambildataini (sptr, &nd)>=0)
	{
	    if (strlen(nd.info)>=SBOARD_INFOLEN) // eshabe : [2010-06-12] fix problem long info
            return -1;

		if ((strlen(info)+strlen(nd.info)+1)>SBOARD_INFOLEN)
			strncpy(info2,info, SBOARD_INFOLEN-(strlen(nd.info)+1));
		else
			strncpy(info2, info, SBOARD_INFOLEN);

		if (nd.info[0])
			strcat(nd.info, " ");
		strcat(nd.info,info2);
		nd.info[SBOARD_INFOLEN] = 0;
		return (sdata_baruiini(sptr, &nd, sizeof (node)));
	}
	return -1;
}

int node_cari (_sdataptr sptr, const char *n, int mulai)
{
	node nd;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
    {
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
    }
	while (1)
	{
		sdata_ambildataini(sptr, &nd);
		if (strcmp(nd.nama,n)==0)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &nd);
		if (strcmp(nd.nama,n)==0)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}

	return -1;
}

int node_caridiinfo (_sdataptr sptr, const char *inf, int mulai)
{
	node nd;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &nd);
		if (strstr(nd.info,inf)!=NULL)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &nd);
		if (strstr(nd.info,inf)!=NULL)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}

	return -1;
}

int node_takadadinet (_sdataptr snodptr, _sdataptr stestpointptr, char *hasil)
{
	node nd;
	int ada = 0;

#ifdef _SBOARD_VERBOSE_
		printf ("node_takadadinet\n");
#endif

	if (hasil)
		hasil[0] = 0;
	if (sdata_kekepala(snodptr)<0 || sdata_kekepala(stestpointptr)<0)
		return 0;
	while(1)
	{
		sdata_ambildataini(snodptr, &nd);
		if (tpt_cari(stestpointptr, nd.id, -1)<0)
		{
			ada++;
			if (hasil)
			{
				strcat(hasil,nd.nama);
				strcat(hasil," ");
			}
#ifdef _SBOARD_DEVELOPMENT_
		printf ("%5d %5d %s\n", ada, nd.id,nd.nama);
#endif
		}
		if (sdata_diekorkah(snodptr))
			break;
		sdata_kedepan(snodptr);
	}
	return ada;
}

void node_tampildata (_sdataptr sptr, FILE *f)
{
	node nd;
	if (sdata_ambildataini(sptr, &nd)>=0)
	{
		fprintf (f, "%d!%s!%s\n", nd.id, nd.nama, nd.info);
	}
}

void node_tampilsemua (_sdataptr sptr, FILE *f)
{
	if (sdata_kekepala(sptr)<0)
		return;
	fprintf (f, "#ID!NAME!INFO\n");
	while (1)
	{
		node_tampildata(sptr, f);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
}

/* KONEKSI */

int koneksi_tambah (_sdataptr sptr, int id, const char *konek)
{
	koneksi kon;
	kon.node_id = id;
	strcpy (kon.komp_pin, konek);
	return sdata_tambah(sptr, &kon, sizeof(koneksi));
}

int koneksi_nodeid (_sdataptr sptr)
{
	koneksi kon;

	kon.node_id = -1;
	sdata_ambildataini (sptr, &kon);

	return kon.node_id;
}

int koneksi_komppin (_sdataptr sptr, char *komppin)
{
	koneksi kon;

	kon.node_id = -1;
	komppin[0] = 0;
	sdata_ambildataini (sptr, &kon);
	strcpy (komppin, kon.komp_pin);

	return kon.node_id;
}

int koneksi_carijangkau (_sdataptr sptr, int id, const char *n, int mulai, int akhir)
{
	koneksi konek;

	if (sdata_jumlahdata(sptr)<0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &konek);
		if (id>=0 && konek.node_id==id ||
			n[0]!='*' && strcmp(konek.komp_pin,n)==0 ||
			n[0]=='.' && strncmp(konek.komp_pin,&n[1],strlen(n)-1)==0)
		{
			if (n[0]=='*')
				strcpy ((char *)n, konek.komp_pin);
			return sdata_nomorini(sptr);
		}
		if (sdata_nomorini(sptr)==akhir)
            return -1;
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai || sdata_nomorini(sptr)==akhir)
			break;
		sdata_ambildataini(sptr, &konek);
		if (id>=0 && konek.node_id==id ||
			n[0]!='*' && strcmp(konek.komp_pin,n)==0)
		{
			if (n[0]=='*')
				strcpy ((char *)n, konek.komp_pin);
			return sdata_nomorini(sptr);
		}
		sdata_kedepan(sptr);
	}

	return -1;
}

int koneksi_cari (_sdataptr sptr, int id, const char *n, int mulai)
{
    return koneksi_carijangkau (sptr, id, n, mulai, -1);
}


int koneksi_cari_dkonek (_sdataptr sptr, const char *n, int *nodeid, int mulai)
{
	koneksi konek;

	if (koneksi_cari(sptr, -1, n, mulai)>=0)
	{
		sdata_ambildataini(sptr, &konek);
		*nodeid = konek.node_id;
		return sdata_nomorini(sptr);
	}
	return -1;
}


void koneksi_tampildata (_sdataptr sptr, FILE *f)
{
	koneksi kon;
	if (sdata_ambildataini(sptr, &kon)>=0)
	{
		fprintf (f, "%d!%s\n", kon.node_id, kon.komp_pin);
	}
}

void koneksi_tampilsemua (_sdataptr sptr, FILE *f)
{
	if (sdata_kekepala(sptr)<0)
		return;
	fprintf (f, "#SID!REFDES.PIN\n");
	while (1)
	{
		koneksi_tampildata(sptr, f);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
}

/*int tpt_tambah (_sdataptr sptr, int nid, char tb, int x1, int y1, int x2, int y2)*/
/*int tpt_tambah (_sdataptr sptr, int nid, char tb, int x, int y)*/
int tpt_tambah (_sdataptr sptr, int nid, char tb, int x, int y, char *info)
{
	testpoint tp;

	memset (&tp, 0, sizeof (testpoint));

	if (nid<0)
		return -1;

	tp.node_id = nid;
	tp.pos_tb = tb;
	tp.pos_x = x;
	tp.pos_y = y;
	strncpy (tp.info, info, SBOARD_TPINFOLEN); /* 2.1 */

	return sdata_tambah(sptr, &tp, sizeof(testpoint));
}

int tpt_barui_xy (_sdataptr sptr, int x, int y)
{
	testpoint tp;

	sdata_ambildataini (sptr, &tp);
	tp.pos_x = x;
	tp.pos_y = y;
	return sdata_baruiini(sptr, &tp, sizeof(testpoint));
}

int tpt_barui_info (_sdataptr sptr, char *info)
{
	testpoint tp;
	int len = strlen(info);

	if (len>SBOARD_TPINFOLEN)
        len = SBOARD_TPINFOLEN;

	sdata_ambildataini (sptr, &tp);
	strncpy (tp.info, info, len);
	tp.info[SBOARD_TPINFOLEN]=0;
	return sdata_baruiini(sptr, &tp, sizeof(testpoint));
}

int tpt_id (_sdataptr sptr, int *nid)
{
	testpoint net;
	if (sdata_ambildataini(sptr, &net)>=0)
	{
		*nid = net.node_id;
		return 1;
	}
	return 0;
}

int tpt_tb (_sdataptr sptr, char *tb)
{
	testpoint tp;
	if (sdata_ambildataini(sptr, &tp)>=0)
	{
		*tb = tp.pos_tb;
		return 1;
	}
	return 0;
}

int tpt_xy (_sdataptr sptr, int *x, int *y)
{
	testpoint tp;
	if (sdata_ambildataini(sptr, &tp)>=0)
	{
		*x = tp.pos_x;
		*y = tp.pos_y;
		return 1;
	}
	return 0;
}

int tpt_info (_sdataptr sptr, char *info)
{
	testpoint tp;
	if (sdata_ambildataini(sptr, &tp)>=0)
	{
		strcpy (info, tp.info);
		return 1;
	}
	return 0;
}

int tpt_tambahi_info (_sdataptr sptr, const char *info)
{
	testpoint tp;
	char info2[SBOARD_TPINFOLEN+1];
	if (sdata_ambildataini (sptr, &tp)>=0)
	{
	    if (strlen(tp.info)>=SBOARD_TPINFOLEN)
            return -1;

		if ((strlen(info)+strlen(tp.info)+1)>SBOARD_TPINFOLEN)
			strncpy(info2,info, SBOARD_TPINFOLEN-(strlen(tp.info)+1));
		else
			strncpy(info2, info, SBOARD_TPINFOLEN);

		if (tp.info[0])
			strcat(tp.info, " ");
		strcat(tp.info,info2);
		tp.info[SBOARD_INFOLEN] = 0;
		return (sdata_baruiini(sptr, &tp, sizeof (testpoint)));
	}
	return -1;
}

int tpt_cari (_sdataptr sptr, int id, int mulai)
{
	testpoint net;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &net);
		if (net.node_id==id)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &net);
		if (net.node_id==id)
			return sdata_nomorini(sptr);
		sdata_kedepan(sptr);
	}

	return -1;
}

//Rectangle (hdc, x1-XY_GROW, y1-XY_GROW, x1+XY_GROW, y1+XY_GROW); /* gambar kotak */

int tpt_cari_xy (_sdataptr sptr, int x, int y, int mulai)
{
	testpoint net;
	int x1,y1,x2,y2;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &net);
		x1 = net.pos_x-XY_GROW;
		x2 = net.pos_x+XY_GROW;
		y1 = net.pos_y-XY_GROW;
		y2 = net.pos_y+XY_GROW;

		if (x>=x1 && x<=x2 && y>=y1 && y<=y2)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &net);
		x1 = net.pos_x-XY_GROW;
		x2 = net.pos_x+XY_GROW;
		y1 = net.pos_y-XY_GROW;
		y2 = net.pos_y+XY_GROW;

		if (x>=x1 && x<=x2 && y>=y1 && y<=y2)
			return sdata_nomorini(sptr);
		sdata_kedepan(sptr);
	}

	return -1;
}

int tpt_caridiinfo (_sdataptr sptr, const char *inf, int mulai, int *node_id)
{
	testpoint tp;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &tp);
		if (strstr(tp.info,inf)!=NULL)
		{
			*node_id = tp.node_id;
			return sdata_nomorini(sptr);
		}
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &tp);
		if (strstr(tp.info,inf)!=NULL)
		{
			*node_id = tp.node_id;
			return sdata_nomorini(sptr);
		}
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}

	return -1;
}


void tpt_cekpin (_sdataptr sptr, int *ndp, int *nlp, int *pb)
{
	int i;
	testpoint tp;

	*ndp = 0;
	*nlp = 0;
	*pb = 0;

	for (i=0;i<sdata_jumlahdata(sptr);i++)
	{
		sdata_ambildatanomor (sptr, i, &tp);
		if (strstr(tp.info, "N"))
			*ndp = 1;
		if (strstr(tp.info, "F"))
			*nlp = 1;
		if (strstr(tp.info, "P"))
			*pb = 1;

		if (*ndp && *nlp || *pb)
			break;
	}
}


/* x..<< , y..>> */
void tpt_urut (_sdataptr sptr)
{
	int i,j,l;
	testpoint n1,n2;
	l = sdata_jumlahdata(sptr);
	if (l<=1)
		return;
	for (i=0;i<l-1;i++)
	{
		for (j=i+1;j<l;j++)
		{
			sdata_ambildatanomor (sptr, i, &n1);
			sdata_ambildatanomor (sptr, j, &n2);
			if (n2.pos_x<n1.pos_x)
			{
				/* swap */
				sdata_kenomor (sptr, i);
				sdata_baruiini (sptr, &n2, sizeof(testpoint));
				sdata_kenomor(sptr, j);
				sdata_baruiini (sptr, &n1, sizeof(testpoint));
			}
		}
	}

	for (i=0;i<l-1;i++)
	{
		for (j=i+1;j<l;j++)
		{
			sdata_ambildatanomor (sptr, i, &n1);
			sdata_ambildatanomor (sptr, j, &n2);
			if (n2.pos_x<=n1.pos_x && n2.pos_y>n1.pos_y)
			{
				/* swap */
				sdata_kenomor (sptr, i);
				sdata_baruiini (sptr, &n2, sizeof(testpoint));
				sdata_kenomor(sptr, j);
				sdata_baruiini (sptr, &n1, sizeof(testpoint));
			}
		}
	}
}

void tpt_tampildata (_sdataptr sptr, FILE *f)
{
	testpoint net;
	if (sdata_ambildataini(sptr, &net)>=0)
	{
		/* fprintf (f, "%d!%c!%d!%d!%d!%d\n", net.node_id, net.pos_tb,net.pos_x1,net.pos_y1,net.pos_x2,net.pos_y2);  */
		fprintf (f, "%d!%c!%d!%d!%s\n", net.node_id, net.pos_tb,net.pos_x,net.pos_y,net.info);
	}
}

void tpt_tampilsemua (_sdataptr sptr, FILE *f)
{
	if (sdata_kekepala(sptr)<0)
		return;
	fprintf (f, "#SID!TOP-BOTTOM!X!Y!INFO\n");
	while (1)
	{
		tpt_tampildata(sptr, f);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
}

/* TRACE */
int trace_tambah (_sdataptr sptr, int nid, SBOARD_ROUTETYPE tipe, int *data, int datalen)
{
	int l=(datalen>SBOARD_DATALEN)?SBOARD_DATALEN:datalen;
	trace trc;

	memset(&trc, 0, sizeof (trace));

	trc.node_id = nid;
	trc.tipe = tipe;
	//memset(trc.data, 0, SBOARD_DATALEN*sizeof(int));
	memcpy(trc.data, data, l*sizeof(int));
	return sdata_tambah(sptr, &trc, sizeof(trace));
}

int trace_barui (_sdataptr sptr, int nid, SBOARD_ROUTETYPE tipe, int *data, int datalen)
{
	int l=(datalen>SBOARD_DATALEN)?SBOARD_DATALEN:datalen;
	trace trc;

	trc.node_id = nid;
	trc.tipe = tipe;
	memset(trc.data, 0, SBOARD_DATALEN*sizeof(int));
	memcpy(trc.data, data, l*sizeof(int));
	return sdata_baruiini(sptr, &trc, sizeof(trace));
}

int trace_cari (_sdataptr sptr, int id, int mulai)
{
	trace trc;

	if (sdata_jumlahdata(sptr)==0)
		return -1;
	if (mulai<0)
        mulai = sdata_nomorini(sptr);
	else if (mulai != sdata_nomorini(sptr))
	{
		if (sdata_kenomor(sptr, mulai)<0)
            return -1;
	}
	while (1)
	{
		sdata_ambildataini(sptr, &trc);
		if (trc.node_id==id)
			return sdata_nomorini(sptr);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
	if (mulai<=0)
        return -1;
	sdata_kekepala(sptr);
	while(1)
	{
		if (sdata_nomorini(sptr) == mulai)
			break;
		sdata_ambildataini(sptr, &trc);
		if (trc.node_id==id)
			return sdata_nomorini(sptr);
		sdata_kedepan(sptr);
	}

	return -1;
}

void trace_tampildata (_sdataptr sptr, FILE *f)
{
	trace trc;
	int i;
	if (sdata_ambildataini(sptr, &trc)>=0)
	{
		fprintf (f, "%d!%d", trc.node_id, trc.tipe);
		for (i=0;i<SBOARD_DATALEN;i++)
			fprintf (f, "!%d", trc.data[i]);
		fprintf (f, "\n");
	}
}

void trace_tampilsemua (_sdataptr sptr, FILE *f)
{
	if (sdata_kekepala(sptr)<0)
		return;
	fprintf (f, "#SID!TYPE!D0!D1!D2!D3!D4!D5!D6!D7\n");
	fprintf (f, "#TYPE=0:VIA,1:LINE,2:ARC,3:RECT\n");
	fprintf (f, "#D7=0:INNER,1:TOP,2:BOTTOM\n");
	fprintf (f, "#D6=0:COUNTERCLOCKWISE,1:CLOCKWISE;TYPE=ARC\n");
	while (1)
	{
		trace_tampildata(sptr, f);
		if (sdata_diekorkah(sptr))
			break;
		sdata_kedepan(sptr);
	}
}


