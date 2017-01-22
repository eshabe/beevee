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
 *  gencadfile.c
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 3/29/06.
 *
 */

/*
30 Agustus 2007
Diketahui masalah penamaan testpin. Trimakasih Cholis!

10 Agustus 2007
Masukkan testpin nomor dlm info

17 Mei 2006
masukan placement & shape dlm info

perbaiki orientasi

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gencadfile.h"
#include "../sdata_c/sdatateks.h"

/* gencad 1.4 */

enum GENCAD_AKSI2 {
GENCAD_NONE,
GENCAD_HEADER,
GENCAD_PADS,
GENCAD_PADSTACKS,
GENCAD_TRACKS,
GENCAD_ARTWORKS,
GENCAD_BOARD,
GENCAD_SHAPES,
GENCAD_COMPONENTS,
GENCAD_DEVICES,
GENCAD_SIGNALS,
GENCAD_LAYERS,
GENCAD_ROUTES,
GENCAD_TESTPINS,
GENCAD_MECH
};
typedef int GENCAD_AKSI2;

void gencad_baca_cad_header (char *teks, boardptr bptr)
{
	char fld[64];

#ifdef _BEEVEE_VERBOSE_
    puts ("gencad_baca_cad_header()");
#endif

	sscanf (teks, "%s", fld);
	if (strcmp(fld,"GENCAD")==0)
	{
		ReplaceChar (teks, ' ', '-');
		board_tambahversi(bptr, teks);
	}
	else if (strcmp(fld, "DRAWING")==0)
	{
		GetTextStartAtText (teks, fld, 63, " ", 0);
		TrimLeft(fld);
		RemoveChar (fld, '\"');
		ReplaceChar (fld, ' ', '-');
		board_isinama(bptr, fld);
	}
	else if (strcmp(fld,"REVISION")==0)
	{
		GetTextStartAtText (teks, fld, 63, " ", 0);
		TrimLeft(fld);
		RemoveChar (fld, '\"');
		ReplaceChar (fld, ' ', '-');
		board_tambahversi(bptr, fld);
	}
}

int gencad_baca_cad_shape (char *teks, char *nama, _sdataptr shaptr)
{
	Str63 fld;
	int x1,y1,x2,y2,d;
	_sdatateks steks;


#ifdef _BEEVEE_VERBOSE_
    puts ("gencad_baca_cad_shape()");
#endif

	sscanf (teks, "%s", fld);

	if (strcmp(fld, "SHAPE")==0)
	{
		sscanf (teks, "%s %s", fld, nama);
		return komp_tambah (shaptr, nama, "", 'T', 0, 0,0,0,0);
	}
	else if (nama[0])
	{

		if (strcmp(fld, "CIRCLE")==0)
		{
			sdata_init (&steks);
			sdatateks_dariteks(&steks, teks, ' ',64, 0);
			sdatateks_nomorini_int (&steks, 1, &x1);
			sdatateks_nomorini_int (&steks, 2, &y1);
			sdatateks_nomorini_int (&steks, 3, &d);
			x1-=(d/2);
			x2=x1+d;
			y1-=(d/2);
			y2=y1+d;
			sdata_hapussemua(&steks);
			return komp_nama_tambahi_xy (shaptr, nama, x1, y1, x2, y2);
		}
		else if (strcmp(fld, "LINE")==0)
		{
			sdata_init (&steks);
			sdatateks_dariteks(&steks, teks, ' ',64, 0);
			sdatateks_nomorini_int (&steks, 1, &x1);
			sdatateks_nomorini_int (&steks, 2, &y1);
			sdatateks_nomorini_int (&steks, 3, &x2);
			sdatateks_nomorini_int (&steks, 4, &y2);
			sdata_hapussemua(&steks);

			if (x1!=x2 || y1!=y2)
				return komp_nama_tambahi_xy (shaptr, nama, x1, y1, x2, y2);
		}
		else if (strcmp(fld, "ARC")==0)
		{
			sdata_init (&steks);
			sdatateks_dariteks(&steks, teks, ' ',64, 0);
			sdatateks_nomorini_int (&steks, 3, &x1);
			sdatateks_nomorini_int (&steks, 4, &y1);
			sdatateks_nomorini_int (&steks, 5, &x2);
			sdatateks_nomorini_int (&steks, 6, &y2);
			sdata_hapussemua(&steks);
			return komp_nama_tambahi_xy (shaptr, nama, x1, y1, x2, y2);
		}
		return sdata_nomorini(shaptr);
	}
	return -1;
}

int gencad_baca_cad_komponen (char *teks, char *nama, _sdataptr skomptr, _sdataptr shaptr)
{
	Str63 fld;
	Str255 info;
	int x1,y1,x2,y2;
	int x,y;
	short or;
	char tb;
	_sdatateks steks;

#ifdef _BEEVEE_VERBOSE_
    puts ("gencad_baca_cad_komponen()");
#endif

	sscanf (teks, "%s", fld);

	if (strcmp(fld, "COMPONENT")==0)
	{
		sscanf (teks, "%s %s", fld, nama);
		return komp_tambah (skomptr, nama, "", 'T', 0, 0,0,0,0);
	}
	else if (nama[0])
	{
		if (strcmp(fld, "DEVICE")==0)
		{
			sscanf (teks, "%s %s", fld, info);
			sprintf (fld,"PN=%s", info);
			return komp_tambahi_info (skomptr, fld); /*20061208*/
		}
		else if (strcmp(fld, "PLACE")==0)
		{
			sdata_init (&steks);
			sdatateks_dariteks(&steks, teks, ' ',64, 0);
			sdatateks_nomorini_int(&steks, 1, &x);
			sdatateks_nomorini_int(&steks, 2, &y);
			sdata_hapussemua(&steks);

			sprintf (info, "XY=%d:%d", x, y); /*20061208*//*20070111*/
			komp_tambahi_info (skomptr, info); /*20061208*/
			return komp_nama_barui_xy (skomptr, nama, x, y, x, y);
		}
		else if (strcmp(fld, "LAYER")==0)
		{
			sscanf (teks, "%s %s", fld, info);
			return komp_barui_tb (skomptr, info[0]);
		}
		else if (strcmp(fld, "ROTATION")==0)
		{
			sscanf (teks, "%s %s", fld, info);
			return komp_barui_orentasi (skomptr, (short)atoi(info));
		}
		else if (strcmp(fld, "SHAPE")==0)
		{
			sscanf (teks, "%s %s", fld, info);
			if (komp_cari (shaptr, info, sdata_nomorini(shaptr))>=0)
			{
				/* ambil center */
				komp_xy (skomptr, &x1, &y1, &x2, &y2);
				x = x1;
				y = y1;
				komp_orentasi(skomptr, &or);
				komp_tb(skomptr, &tb);
				sprintf(fld,"SHA=%s", info); /*20061208*/
				komp_tambahi_info (skomptr, fld);


				/* shape */
				komp_xy (shaptr, &x1, &y1, &x2, &y2);

				/* pastikan center = 0,0 */
				/*
				dx = (x2-x1)/2;
				dy = (y2-y1)/2;
				x1 = -dx;
				y1 = -dy;
				x2 = dx;
				y2 = dy;
				*/

				if (tb=='B')
				{
				    x1*=-1;
				    x2*=-1;
				}

				/* update komp xy berdasar shape... */
				if (or!=0)
					object_putar(&x1, &y1, &x2, &y2, or);
				/* object putar hanya bisa pada derajat 90,180, 270
				   karena rectangle hanya mengenal dua pojok
				*/

				x1 += x;
				y1 += y;
				x2 += x;
				y2 += y;
				return komp_nama_barui_xy (skomptr, nama, x1, y1, x2, y2);
			}
		}
	}
	return -1;
}

int gencad_baca_cad_device (char *teks, char *nama, _sdataptr sptr)
{
	char fld[64];
	char val[64];
	char info[SBOARD_INFOLEN+1];

#ifdef _BEEVEE_VERBOSE_
    puts ("gencad_baca_cad_device()");
#endif

	sscanf (teks, "%s %s", fld, val);
	RemoveChar (val, '\"');
	if (strcmp(fld, "DEVICE")==0)
	{
		strcpy(nama, val);
		return komp_tambah (sptr, nama, "", 'T', 0, 0,0,0,0);
	}
	else if (nama[0])
	{
		if (strcmp(fld,"VALUE")==0)
            strcpy(fld,"VAL");
        else if (strcmp(fld, "PINDESCR")==0)
            return 0;
		sprintf (info, "%s=%s", fld, val);
		return komp_tambahi_info (sptr, info);
	}

	return -1;
}

void gedcad_barui_komponen_info (_sdataptr skomptr, _sdataptr sdevptr)
{
	Str255 infok;
	Str255 infod;
	Str63 pn;

#ifdef _BEEVEE_VERBOSE_
    puts ("gedcad_barui_komponen_info()");
#endif

	if (sdata_kekepala(skomptr)>=0)
	{
		while(1)
		{
			komp_info(skomptr, infok); /* komponen */
			pn[0] = 0;
            GetTextStartAtText(infok,pn, 63, "PN=", 0);
            if(strncmp(pn,"PN=",3)==0)
            {
                ReplaceChar(pn, ' ', 0);
                if (komp_cari(sdevptr, &pn[3], sdata_nomorini(sdevptr))>=0)
                {
                    komp_info(sdevptr, infod); /* device info */
                    TrimRight(infod);
                    komp_tambahi_info (skomptr, infod);
                }
            }
			if (sdata_diekorkah(skomptr))
				break;
			sdata_kedepan(skomptr);
		}
	}
}

int gencad_baca_cad_signal (char *teks, int nid, _sdataptr snodptr, _sdataptr skonptr)
{
	Str63 komp;

#ifdef _BEEVEE_VERBOSE_
    puts ("gencad_baca_cad_signal()");
#endif

	if (strncmp(teks, "SIGNAL", 6) == 0)
	{
		GetTextStartAtText (teks, komp, Str63Len, " ", 0);
		TrimLeft(komp);
		return node_tambah(snodptr, komp, "");
	}
	else if (strncmp (teks, "NODE", 4)==0 && nid>=0 )
	{
		GetTextStartAtText (teks, komp, Str63Len, " ", 0);
		TrimLeft(komp);
		ReplaceChar(komp,' ','.');
		if (koneksi_tambah(skonptr, nid, komp)>=0)
			return nid;
	}
	return -1;
}

int gencad_baca_cad_testpin (char *teks, _sdataptr snodptr, _sdataptr stestpointptr)
{
	_sdatateks steks;
	int nid;
	Str63 sig;
	char tb[8];
	Str63 testpin;
	Str63 nomor; /* shb 20070830 - change from 8 to 64 */
	int x,y;

#ifdef _BEEVEE_VERBOSE_
    puts ("gencad_baca_cad_testpin()");
#endif

	sdata_init(&steks);
	if (strncmp(teks, "TESTPIN", 7)==0)
	{
		if (sdatateks_dariteks(&steks, teks, ' ', Str63Len, 1)==9)
		{
			sdata_ambildatanomor(&steks, 4, sig);
			if ( (nid=node_cari(snodptr, sig, sdata_nomorini(snodptr)))>=0) /* cari signal dlm data node */
			{
				x = y = 0;
				sdata_ambildatanomor(&steks, 8, tb);
				sdatateks_nomorini_int(&steks, 2, &x);
				sdatateks_nomorini_int(&steks, 3, &y);
				//printf ("%d %d %d %s\n", nid, x, y, tb);
				sdata_ambildatanomor(&steks, 0, testpin);
				sdata_ambildatanomor(&steks, 1, nomor);
				strcat (testpin,nomor);
				sdata_hapussemua(&steks);
				return tpt_tambah (stestpointptr, nid, tb[0], x, y, testpin);
			}
		}
		sdata_hapussemua(&steks);
	}

	return -1;
}


int gencad_baca_cad_trace (char *teks, int nid, _sdataptr snodptr, _sdataptr strcptr)
{
	_sdatateks steks;
	Str63 sig;
	static int layer=SBOARD_TRACE_INNER;
	/* int x1,x2,y1,y2; */
	int data[SBOARD_DATALEN];

#ifdef _BEEVEE_VERBOSE_
    puts ("gencad_baca_cad_trace()");
#endif

	if (strncmp(teks, "ROUTE", 5)==0)
	{
		MidText (teks, sig, 6, Str63Len);
		layer = SBOARD_TRACE_INNER;
		return node_cari(snodptr, sig, sdata_nomorini(snodptr));
	}
	else if (strncmp(teks, "LAYER", 5)==0)
	{
        if (strstr(teks,"TOP"))
            layer = SBOARD_TRACE_TOP;
        else if (strstr(teks,"BOTTOM"))
            layer = SBOARD_TRACE_BOTTOM;
        return nid;
	}
	else if (nid>=0)
	{
		sdata_init (&steks);
		sdatateks_dariteks(&steks, teks, ' ', Str63Len, 1);
		memset(data, 0, SBOARD_DATALEN*sizeof(int));
		data[7] = layer;
		if (strncmp(teks, "VIA", 3)==0)
		{
			sdatateks_nomorini_int(&steks, 2, &data[0]);
			sdatateks_nomorini_int(&steks, 3, &data[1]);
			trace_tambah(strcptr, nid, SBOARD_VIA, data, SBOARD_DATALEN);
		}
		else if (strncmp(teks, "LINE", 4)==0)
		{
			sdatateks_nomorini_int(&steks, 1, &data[0]);
			sdatateks_nomorini_int(&steks, 2, &data[1]);
			sdatateks_nomorini_int(&steks, 3, &data[2]);
			sdatateks_nomorini_int(&steks, 4, &data[3]);
			//printf ("%d %d %d %d\n", data[0], data[1], data[2], data[3]);
			trace_tambah(strcptr, nid, SBOARD_LINE, data, SBOARD_DATALEN);
		}
		else if (strncmp(teks, "ARC", 3)==0)
		{
			sdatateks_nomorini_int(&steks, 1, &data[0]);
			sdatateks_nomorini_int(&steks, 2, &data[1]);
			sdatateks_nomorini_int(&steks, 3, &data[2]);
			sdatateks_nomorini_int(&steks, 4, &data[3]);
			sdatateks_nomorini_int(&steks, 5, &data[4]);
			sdatateks_nomorini_int(&steks, 6, &data[5]);
			/* TODO: NO NEED TO SET SBOARD_CWCLOCKWISE?? */
			trace_tambah(strcptr, nid, SBOARD_ARC, data, SBOARD_DATALEN);
		}
		sdata_hapussemua(&steks);
		return nid;
	}

	return -1;
}

int gencad_baca_cad (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char nama[64];
	int p;
	GENCAD_AKSI2 aksi = GENCAD_NONE;
	_sdata shapes;
	_sdata sdevs;
	int node_id = -1;

#ifdef _BEEVEE_VERBOSE_
printf ("gencad_baca_cad %s ...\n", namafile);
#endif

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		sdata_init(&shapes);
		sdata_init(&sdevs);
		nama[0] = 0;
		while (1)
		{
			fgets (line, 255, f);
			if (feof(f))
				break;

#ifdef _BEEVEE_VERBOSE_
puts (line);
#endif

			p = 0;
			ReplaceChar (line, '\t',' ');
			ReplaceChar (line, 0xa, 0xd);
			TrimLeft(line);
			TrimRight(line);
			UpperText(line);

			if (GetLine(line,teks,&p))
			{
				if (teks[0]=='$')
				{
					if (strcmp (teks, "$HEADER")==0)
						aksi = GENCAD_HEADER;
					else if (strcmp (teks, "$PADS")==0)
						aksi = GENCAD_PADS;
					else if (strcmp (teks, "$PADSTACKS")==0)
						aksi = GENCAD_PADSTACKS;
					else if (strcmp (teks, "TRACKS")==0)
						aksi = GENCAD_TRACKS;
					else if (strcmp (teks, "$ARTWORKS")==0)
						aksi = GENCAD_ARTWORKS;
					else if (strcmp (teks, "$BOARD")==0)
						aksi = GENCAD_BOARD;
					else if (strcmp (teks, "$SHAPES")==0)
						aksi = GENCAD_SHAPES;
					else if (strcmp (teks, "$COMPONENTS")==0)
						aksi = GENCAD_COMPONENTS;
					else if (strcmp (teks, "$DEVICES")==0)
						aksi = GENCAD_DEVICES;
					else if (strcmp (teks, "$SIGNALS")==0)
						aksi = GENCAD_SIGNALS;
					else if (strcmp (teks, "$LAYERS")==0)
						aksi = GENCAD_LAYERS;
					else if (strcmp (teks, "$ROUTES")==0)
						aksi = GENCAD_ROUTES;
					else if (strcmp (teks, "$TESTPINS")==0)
						aksi = GENCAD_TESTPINS;
					else if (strcmp (teks, "$MECH")==0)
						aksi = GENCAD_MECH;
					//else if (aksi == GENCAD_DEVICES)
					//{
						/* sebelumnya baca devices */
						/* perbaharui data komponen */
						//gedcad_barui_komponen_info (&bptr->skomp, &sdevs);
						//aksi = GENCAD_NONE;
					//}
					else
						aksi = GENCAD_NONE;

					teks[0] = 0;
				}

				if (aksi != GENCAD_NONE && teks[0])
				{
					if (aksi == GENCAD_HEADER)
						gencad_baca_cad_header (teks, bptr);
					else if (aksi == GENCAD_SHAPES)
					{
						if (gencad_baca_cad_shape (teks, nama, &shapes)<0)
							nama[0] = 0;
					}
					else if (aksi == GENCAD_COMPONENTS)
					{
						if (gencad_baca_cad_komponen (teks, nama, &bptr->skomp, &shapes)<0)
							nama[0] = 0;
					}
					else if (aksi == GENCAD_DEVICES)
					{
						if (gencad_baca_cad_device (teks, nama, &sdevs)<0)
							nama[0] = 0;
					}
					else if (aksi==GENCAD_SIGNALS)
					{
						node_id = gencad_baca_cad_signal (teks, node_id, &bptr->snode, &bptr->skonek);
					}
					else if (aksi==GENCAD_TESTPINS)
					{
						gencad_baca_cad_testpin (teks, &bptr->snode, &bptr->stestpoint);
					}
					else if (aksi==GENCAD_ROUTES)
					{
						node_id = gencad_baca_cad_trace(teks, node_id, &bptr->snode, &bptr->strace);
					}
				}
			}
			//else
			//	aksi = GENCAD_NONE;

            printf ("aksi=%d\n", aksi);

		}
		fclose (f);
		//komp_tampilsemua (&sdevs, stdout);

        if (sdata_jumlahdata(&sdevs)>0 && sdata_jumlahdata(&bptr->skomp)>0)
        {
            gedcad_barui_komponen_info (&bptr->skomp, &sdevs);
        }

		sdata_hapussemua(&shapes);
		sdata_hapussemua(&sdevs);
	}
	else
	{
#ifdef _BEEVEE_VERBOSE_
		printf ("Ga bisa baca %s\n", namafile);
#endif
		return 0;
	}

#ifdef _BEEVEE_VERBOSE_
    printf ("Jumlah Node %d\n", sdata_jumlahdata(&bptr->snode));
    printf ("Jumlah Komponen %d\n", sdata_jumlahdata(&bptr->skomp));
    printf ("Jumlah Trace %d\n", sdata_jumlahdata(&bptr->strace));
    printf ("Jumlah Koneksi %d\n", sdata_jumlahdata(&bptr->skonek));
#endif

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

