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
 *  ppcbfile.c
 *
 *  Ditulis oleh Sihabul Milah sejak 27/07/07.
 *
 */

/*
	Kegunaan:
	Membaca file PAD-POWERPCB (.ASC) untuk diproses dlm aplikasi BeeVee 2

*/

#include <stdio.h>
#include <string.h>

#include "sboard.h"
#include "../sdata_c/sdatateks.h"
#include "unitconv.h"
#include "ppcbfile.h"

enum POWERPCB_AKSI2 {
	POWERPCB_NONE,
	POWERPCB_HEADER,
	POWERPCB_PCB,
	POWERPCB_PARTDECAL,
	POWERPCB_PARTTYPE,
	POWERPCB_PART,
	POWERPCB_ROUTE,
	POWERPCB_TESTPOINT
};
typedef int POWERPCB_AKSI2;


enum POWERPCB_UNIT2 {
	POWERPCB_MILS = 0,
	POWERPCB_MM,
	POWERPCB_INCHES,
	POWERPCB_BASIC /* MICRON/1000 */
};

#define PADS_POWERPCB_HEADER_STR "!PADS-POWERPCB-V5.0"

static short powerpcb_unit;
static short powerpcb_maxlayer;

/* Baca header untuk mendapatkan jenis 'unit' */
/* Saya baru menemui BASIC dan MILS dlm contoh yg ada. 'METRIC' adalah jenis unit yg saya TIDAK TAHU,
saya samakan saja dg mm */
void pads_powerpcb_baca_asc_header (const char *teks, boardptr bptr)
{
    _sdatateks steks;
    char tag[256];
    char item[64];

    sdata_init (&steks);
    if (sdatateks_dariteks (&steks, teks, '!', 255, 0)>=2)
    {
        sdata_ambildatanomor (&steks, 0, tag);
        sdata_hapussemua(&steks);
        if (sdatateks_dariteks (&steks, tag, '-', 63, 0)>=2)
        {
            sdata_ambildatanomor (&steks, 3, item);

            if (strcmp(item,"BASIC")==0)
                powerpcb_unit = POWERPCB_BASIC;
            else if (strcmp(item,"MILS")==0)
                powerpcb_unit = POWERPCB_MILS;
            else if (strcmp(item,"METRIC")==0)
                powerpcb_unit = POWERPCB_MM;
            else if (strcmp(item,"INCHES")==0)
                powerpcb_unit = POWERPCB_INCHES;
            else
                powerpcb_unit = POWERPCB_MILS;

             //printf ("%s %d!!\n",item, powerpcb_unit);
        }
    }
    sdata_hapussemua(&steks);
}

/* Baca blok PCB */
/* Ambil data MAXIMUMLAYER */
void pads_powerpcb_baca_asc_pcb (const char *teks, boardptr bptr)
{
	_sdatateks steks;
	char tag[14];
	int ival;

	sdata_init(&steks);
	if (sdatateks_dariteks (&steks, teks, ' ', 63, 0)>=2)
	{
		sdata_ambildatanomor (&steks, 0, tag);
		/* Gunakan HEADER */
		/*if (strcmp(tag,"UNIT")==0)
		{
			sdatateks_nomorini_int (&steks, 1, &ival);
			if (ival<=POWERPCB_INCHES)
				powerpcb_unit = ival;
			else
				powerpcb_unit = POWERPCB_MILS;
		}
		else*/ if (strcmp(tag,"MAXIMUMLAYER")==0)
		{
			sdatateks_nomorini_int (&steks, 1, &ival);
			powerpcb_maxlayer = (short)ival;
		}
	}
	sdata_hapussemua(&steks);
}

/* Baca blok PARTDECAL */
/* PARTDECAL ekuivalen dg PACKAGE di GenCAD */
void pads_powerpcb_baca_asc_partdecal (const char *teks, char *nama, _sdataptr sdecal_ptr)
{
	_sdatateks steks;
	int i;
	double fx,fy;
	int x,y;

	sdata_init(&steks);
	i=sdatateks_dariteks (&steks, teks, ' ', 63, 0);
	if (!nama[0] && i == 9)
	{
		sdata_ambildatanomor (&steks, 0, nama);
		komp_tambah(sdecal_ptr, nama, "", 'T', 0, 0,0,0,0);
	}
	else if (nama[0] && i==2)
	{
		sdatateks_nomorini_double (&steks, 0, &fx);
		sdatateks_nomorini_double (&steks, 1, &fy);
		switch (powerpcb_unit)
		{
		    case POWERPCB_BASIC:
                //x = (int)micron2mils(fx); y = (int)micron2mils(fy); break;
                x = fx/1000/25.4; y = fy/1000/25.4; break;
		    case POWERPCB_MM:
                x = (int)mm2mils(fx); y = (int)mm2mils(fy); break;
		    case POWERPCB_INCHES:
                x = (int)inch2mils(fx); y = (int)inch2mils(fy); break;
                break;
            default:
                x = (int)fx; y = (int)fy;
		}
		komp_nama_tambahi_xy (sdecal_ptr, nama, x,y,x,y);
	}

	sdata_hapussemua(&steks);
}

/* Baca blok PARTTYPE */
/* Pada contoh yg saya gunakan, PARTTYPE ekuivalen dg PARTNUMBER, memiliki informasi yg merujuk ke
PARTDECAL */
void pads_powerpcb_baca_asc_parttype (const char *teks, _sdataptr sdecal_ptr, _sdataptr stype_ptr)
{
	_sdatateks steks;
	Str63 type;
	Str63 decal;
	int decal_id;

	sdata_init(&steks);
	if (sdatateks_dariteks (&steks, teks, ' ', 63, 0)==9)
	{
		sdata_ambildatanomor (&steks, 0, type);
		sdata_ambildatanomor (&steks, 1, decal);
		if ((decal_id=komp_cari (sdecal_ptr, decal, -1))>=0)
			koneksi_tambah(stype_ptr, decal_id, type);
	}

	sdata_hapussemua(&steks);
}

/* Baca blok PART */
/* Mengambil informasi placement dan testpoint */
short pads_powerpcb_baca_asc_part (const char *teks, _sdataptr skomp_ptr, _sdataptr sdecal_ptr, _sdataptr stype_ptr)
{
	_sdatateks steks;
	Str63 tb;
	Str63 type;
	Str63 decal;
	komponen komp;
	int ori;
	int ok=-1;
	int i;
	int decal_id;
	int x1=0,y1=0,x2=0,y2=0;
	Str63 temp;
	double fx,fy;

	sdata_init(&steks);
	if ((i=sdatateks_dariteks (&steks, teks, ' ', 63, 0))==12)
	{
		sdata_ambildatanomor (&steks, 0, komp.nama);
		sdata_ambildatanomor (&steks, 1, type); memset (temp,0, 64);
		sdatateks_nomorini_double (&steks, 2, &fx);
		sdatateks_nomorini_double (&steks, 3, &fy);
		sdatateks_nomorini_int (&steks, 4, &ori);
		sdata_ambildatanomor (&steks, 6, tb);
		tb[0] = (tb[0]=='N')?'T':'B';

		switch (powerpcb_unit)
		{
		    case POWERPCB_BASIC:
                //komp.pos_x1 = (int)micron2mils(fx); komp.pos_y1 = (int)micron2mils(fy); break;
                komp.pos_x1 = fx/1000/25.4; komp.pos_y1 = fy/1000/25.4; break;
		    case POWERPCB_MM:
                komp.pos_x1 = (int)mm2mils(fx); komp.pos_y1 = (int)mm2mils(fy); break;
		    case POWERPCB_INCHES:
                komp.pos_x1 = (int)inch2mils(fx); komp.pos_y1 = (int)inch2mils(fy); break;
            default:
                komp.pos_x1 = (int)fx; komp.pos_y1 = (int)fy;
		}

		sprintf (komp.info,"XY=%d:%d", komp.pos_x1, komp.pos_y1);

		if (strstr(type,"TESTPOINT")!=NULL)
		{
			strcat (komp.info, " ");
			strcat (komp.info, type);
		}

		if (koneksi_cari_dkonek (stype_ptr, type, &decal_id, -1)>=0)
		{
			sdata_kenomor (sdecal_ptr, decal_id);

			komp_xy (sdecal_ptr, &x1, &y1, &x2, &y2);
			if (tb[0]=='B')
			{
				x1*=-1;
				x2*=-1;
			}
			if (ori!=0)
				object_putar(&x1, &y1, &x2, &y2, ori);
		}
		ok = komp_tambah (skomp_ptr, komp.nama, komp.info, tb[0], (short)ori,
			komp.pos_x1+x1, komp.pos_y1+y1,  komp.pos_x1+x2, komp.pos_y1+y2);

	}
	sdata_hapussemua(&steks);

	if (ok<0)
		return -1;

	return 0;
}

/* Baca blok ROUTE */
/* Mengambil informasi trace */
short pads_powerpcb_baca_asc_route (const char *teks, _sdataptr snode_ptr, _sdataptr skon_ptr, _sdataptr strace_ptr)
{
	_sdatateks steks;
	Str63 tb;
	Str63 node_name;
	Str63 komp_pin;
	Str63 via;
	static int node_id = -1;
	int ok=-1;
	int i,j;
	int t_id;
	int tx,ty;
	static int l_tx=-1, l_ty=0;
	static int l_ly=0;
	int datatrace[SBOARD_DATALEN];
	int layer;
	double fx,fy;

	sdata_init(&steks);
	i=sdatateks_dariteks (&steks, teks, ' ', 14, 0);
	if (strncmp(teks, "*SIGNAL*", 8)==0)
	{
		sdata_ambildatanomor (&steks, 1, node_name);
		node_id = node_cari(snode_ptr, node_name, -1);
		if (node_id<0)
		{
			node_id = node_tambah (snode_ptr, node_name, "");
		}
		l_tx=-1; l_ty=-1; l_ly=0;
	}
	else if (i==2)
	{
		for (j=0;j<2;j++)
		{
			sdata_ambildatanomor (&steks, j, komp_pin);
			t_id = -1;
			if (koneksi_cari_dkonek(skon_ptr, komp_pin, &t_id, -1)<0)
			{
				koneksi_tambah (skon_ptr, node_id, komp_pin);
			}
		}
		l_tx=-1; l_ty=-1; l_ly=0;
	}
	else if (l_tx == -1 && l_ty== -1)
	{
		sdatateks_nomorini_double (&steks, 0, &fx);
		sdatateks_nomorini_double (&steks, 1, &fy);
		sdatateks_nomorini_int (&steks, 2, &l_ly);

		switch (powerpcb_unit)
		{
		    case POWERPCB_BASIC:
                //l_tx = (int)micron2mils(fx); l_ty = (int)micron2mils(fy); break;
                l_tx = fx/1000/25.4; l_ty = fy/1000/25.4; break;
		    case POWERPCB_MM:
                l_tx = (int)mm2mils(fx); l_ty = (int)mm2mils(fy); break;
		    case POWERPCB_INCHES:
                l_tx = (int)inch2mils(fx); l_ty = (int)inch2mils(fy); break;
            default:
                l_tx = (int)(fx); l_ty = (int)(fy);
		}

	}
	else if (l_tx != -1 || l_ty != -1)
	{
		sdatateks_nomorini_double (&steks, 0, &fx);
		sdatateks_nomorini_double (&steks, 1, &fy);
		sdatateks_nomorini_int (&steks, 2, &layer);
		switch (powerpcb_unit)
		{
		    case POWERPCB_BASIC:
                //tx = (int)micron2mils(fx); ty = (int)micron2mils(fy); break;
                tx = fx/1000/25.4; ty = fy/1000/25.4; break;
		    case POWERPCB_MM:
                tx = (int)mm2mils(fx); ty = (int)mm2mils(fy); break;
		    case POWERPCB_INCHES:
                tx = (int)inch2mils(fx); ty = (int)inch2mils(fy); break;
            default:
                tx = (int)(fx); ty = (int)(fy);
		}
		if (node_id>=0)
		{
			memset (datatrace, 0, SBOARD_DATALEN*sizeof(int));
			datatrace[0]=tx; datatrace[1] = ty;
			datatrace[2]=l_tx; datatrace[3] = l_ty;
			/* layer 1 = TOP, layer MAX = BOTTOM.
			di dalam range = INNER
			di luar range -> abaikan */
			if (l_ly==1)
				datatrace[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_TOP;
			else if (l_ly==powerpcb_maxlayer)
				datatrace[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_BOTTOM;
            else
                datatrace[SBOARD_TRACE_LAYERF] =  SBOARD_TRACE_INNER;

            if (l_ly>=1 && l_ly<=powerpcb_maxlayer)
                trace_tambah (strace_ptr, node_id, SBOARD_LINE, datatrace, SBOARD_DATALEN);
			if (i>=6) /* VIA ? */
			{
				sdata_ambildatanomor (&steks, 5, via);
				if (strstr(via,"VIA")!=NULL)
				{
					datatrace[2]=0; datatrace[3]=0;
					trace_tambah (strace_ptr, node_id, SBOARD_VIA, datatrace, SBOARD_DATALEN);
				}
			}
		}
		l_tx=tx; l_ty=ty; l_ly=layer;
	}


	sdata_hapussemua(&steks);

	if (ok<0)
		return -1;

	return 0;
}

/* Baca blok TESTPOINT */
/* Pada sebagian contoh, ada yg menggunakan blok TESTPOIN */
/* Tentu saja, ia tidak berisi informasi probe/node/nail spt pada data tester */
void pads_powerpcb_baca_asc_testpoint (const char *teks, _sdataptr snode_ptr, _sdataptr stp_ptr)
{
	_sdatateks steks;
	Str63 signal;
	int node_id;
	int x,y;
	int itb;
	double fx,fy;

	sdata_init(&steks);
	if (sdatateks_dariteks (&steks, teks, ' ', 63, 0)==6)
	{
		sdatateks_nomorini_double (&steks, 1, &fx);
		sdatateks_nomorini_double (&steks, 2, &fy);
		sdatateks_nomorini_int (&steks, 3, &itb);
		sdata_ambildatanomor (&steks, 4, signal);
		node_id = node_cari (snode_ptr, signal, -1);
		if (node_id>=0)
		{
            switch (powerpcb_unit)
            {
                case POWERPCB_BASIC:
                    //x = micron2mils(fx); y = micron2mils(fy); break;
                    x = fx/1000/25.4; y = fy/1000/25.4; break;
                case POWERPCB_MM:
                    x = mm2mils(fx); y = mm2mils(fy); break;
                case POWERPCB_INCHES:
                    x = inch2mils(fx); y = inch2mils(fy); break;
                default:
                    x = (int)(fx); y = (int)(fy);
           }
			tpt_tambah (stp_ptr, node_id, (itb==0)?'B':'T', x, y, "");
		}
	}

	sdata_hapussemua(&steks);
}

/* Manipulasi: sebagian PART adalah TESTPOINT
Pd proses ini, part-2 tsb didupikat ke testpoint */
void pads_powerpcb_baca_asc_part2testpoint (_sdataptr skomp_ptr, _sdataptr skonek_ptr, _sdataptr stp_ptr)
{
	komponen komp;
	Str63 komp_pin;
	Str63 xy_str;
	int node_id;
	int x,y;
	int p = 0;

	if (sdata_kekepala (skomp_ptr)<0)
		return;
	while (sdata_ambildataini(skomp_ptr, &komp)>=0)
	{
		if (strstr(komp.info,"TESTPOINT")!=NULL)
		{
			sprintf (komp_pin,"%s.1",komp.nama);
			if (koneksi_cari_dkonek (skonek_ptr, komp_pin, &node_id,-1)>=0)
			{
				p = 0;
				GetTextUntilAChar (komp.info, xy_str, 63, ' ', &p);
				ReplaceChar(xy_str,':',' ');
				sscanf (&xy_str[3],"%d %d", &x, &y);
				tpt_tambah (stp_ptr, node_id, komp.pos_tb, x,y, "");
			}
		}

		if (sdata_diekorkah (skomp_ptr))
			break;
		sdata_kedepan (skomp_ptr);
	}

}

/* Baca file ASC */
int pads_powerpcb_baca_asc (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char nama[64];
	int p;
	short f1rst = 1;
	POWERPCB_AKSI2 aksi = POWERPCB_NONE;
	_sdata sdecal;
	_sdata stype;
	int node_id = -1;

#ifdef _BEEVEE_VERBOSE_
printf ("pads_powerpcb_baca_asc %s ...\n", namafile);
#endif

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		sdata_init(&sdecal);
		sdata_init(&stype);
		nama[0] = 0;

		powerpcb_unit = POWERPCB_MILS;

		while (1)
		{
			fgets (line, 255, f);
			if (feof(f))
				break;

			p = 0;
			ReplaceChar (line, '\t',' ');
			ReplaceChar (line, 0xa, 0xd);
			UpperText(line);

			if (GetLine(line,teks,&p))
			{
				TrimLeft(teks);
				TrimRight(teks);
				if (f1rst)
				{
					if (strncmp(teks, PADS_POWERPCB_HEADER_STR, strlen(PADS_POWERPCB_HEADER_STR))!=0)
					{
#ifdef _BEEVEE_VERBOSE_
						printf ("Bukan PADS POWERPCB yg dikenal!\n");
#endif
						break;
					}
					else
						aksi = POWERPCB_HEADER;
					f1rst = 0;
				}
				else if (teks[0]=='*')
				{
					//printf ("%s\n",teks);
					if (strncmp (teks, "*PCB*", 5)==0)
						aksi = POWERPCB_PCB;
					else if (strncmp (teks, "*PARTDECAL*", 10)==0)
						aksi = POWERPCB_PARTDECAL;
					else if (strncmp (teks, "*PARTTYPE*", 10)==0)
						aksi = POWERPCB_PARTTYPE;
					else if (strncmp (teks, "*PART*", 6)==0)
						aksi = POWERPCB_PART;
					else if (strcmp (teks, "*ROUTE*")==0 || strncmp (teks, "*SIGNAL*",8)==0)
						aksi = POWERPCB_ROUTE;
					else if (strcmp (teks, "*TESTPOINT*")==0)
						aksi = POWERPCB_TESTPOINT;
					else if (strncmp (teks, "*REMARK*", 8)!=0)
						aksi = POWERPCB_NONE;

					if (strncmp (teks, "*SIGNAL*",8)!=0)
						teks[0] = 0;
				}

				if (aksi != POWERPCB_NONE && teks[0])
				{
					if (aksi == POWERPCB_HEADER)
						pads_powerpcb_baca_asc_header (teks, bptr);
					else if (aksi == POWERPCB_PCB)
						pads_powerpcb_baca_asc_pcb (teks, bptr);
					else if (aksi == POWERPCB_PARTDECAL)
						pads_powerpcb_baca_asc_partdecal (teks, nama, &sdecal);
					else if (aksi == POWERPCB_PARTTYPE)
						pads_powerpcb_baca_asc_parttype (teks, &sdecal, &stype);
					else if (aksi == POWERPCB_PART)
						pads_powerpcb_baca_asc_part (teks, &bptr->skomp, &sdecal, &stype);
					else if (aksi == POWERPCB_ROUTE)
						pads_powerpcb_baca_asc_route (teks, &bptr->snode, &bptr->skonek, &bptr->strace);
					else if (aksi == POWERPCB_TESTPOINT)
						pads_powerpcb_baca_asc_testpoint (teks, &bptr->snode, &bptr->stestpoint);
				}
			}
			else if (aksi == POWERPCB_PARTDECAL)
				nama[0]=0;
		}

		fclose (f);

		//komp_tampilsemua (&sdecal, stdout);
		//koneksi_tampilsemua (&stype, stdout);

		sdata_hapussemua(&sdecal);
		sdata_hapussemua(&stype);

		pads_powerpcb_baca_asc_part2testpoint (&bptr->skomp, &bptr->skonek, &bptr->stestpoint);
	}
	else
	{
#ifdef _BEEVEE_VERBOSE_
		printf ("Ga bisa baca %s\n", namafile);
#endif
		return 0;
	}

    if (sdata_jumlahdata(&bptr->snode)==0 ||
        sdata_jumlahdata(&bptr->skomp)==0 ||
        sdata_jumlahdata(&bptr->strace)==0 ||
        sdata_jumlahdata(&bptr->skonek)==0)
        {
            board_hapus(bptr);
            return 0;
        }

    board_hitung_ataskiri (bptr);
    bptr->punya_tempat = 1;

    return 1;
}

