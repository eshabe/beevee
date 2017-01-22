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
 *  fabfile.c
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 10/18/06.
 *
 */

 /* 26 Juli 2007 */
 /* Perbaiki pembacaan file : fabmaster_baca_fab
    -> Header tidak harus diedit.
	-> Fix 4 Mac reading due testing
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sboard.h"
#include "../sdata_c/sdatateks.h"


enum FABM_AKSI2 {
FABM_NONE,
FABM_HEADER,
FABM_PLACES,
FABM_CONNECTIONS,
FABM_LAYERS,
FABM_PADS,
FABM_ASSEMBLY,
FABM_PINS,
FABM_VIAS,
FABM_TRACES,
FABM_FIGSHAPES
};
typedef int FABM_AKSI2;

#define FABM_PLACES_STR "A!REFDES!COMP_CLASS!COMP_PART_NUMBER!COMP_HEIGHT!COMP_DEVICE_LABEL!COMP_INSERTION_CODE!SYM_TYPE!SYM_NAME!SYM_MIRROR!SYM_ROTATE!SYM_X!SYM_Y!COMP_VALUE!COMP_TOL!COMP_VOLTAGE!"
#define FABM_CONNECTIONS_STR "A!NET_NAME!REFDES!PIN_NUMBER!PIN_NAME!PIN_GROUND!PIN_POWER!"
#define FABM_ASSEMBLY_STR "A!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!RECORD_TAG!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!SUBCLASS!SYM_NAME!REFDES!"
#define FABM_PINS_STR "A!SYM_NAME!SYM_MIRROR!PIN_NAME!PIN_NUMBER!PIN_X!PIN_Y!PAD_STACK_NAME!REFDES!PIN_ROTATION!TEST_POINT!"
#define FABM_VIAS_STR "A!VIA_X!VIA_Y!PAD_STACK_NAME!NET_NAME!TEST_POINT!VIA_MIRROR!"
#define FABM_TRACES_STR "A!CLASS!SUBCLASS!GRAPHIC_DATA_NAME!GRAPHIC_DATA_NUMBER!RECORD_TAG!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!NET_NAME!"

void fabmaster_baca_fab_header (char *teks, boardptr bptr)
{
	char fld[64];

	ReplaceChar (&teks[2],'!',0);
	ReplaceChar (teks, ' ', '-');
	board_tambahversi(bptr, &teks[2]);
}

int fabmaster_baca_fab_place (char *teks, _sdataptr komptr)
{
    _sdatateks steks;
    komponen komp;
    int x,y;
    Str63 temp;

    sdata_init (&steks);
    memset (&komp, 0, sizeof(komp));

    //printf (teks);

    if (sdatateks_dariteks(&steks, teks, '!',64, 1)!=17)
    {
        sdata_hapussemua (&steks);
        return -1;
    }


    sdata_ambildatanomor(&steks, 1, komp.nama);
    sdata_ambildatanomor(&steks, 9, temp);
    if (strcmp(temp,"YES")==0)
        komp.pos_tb = 'B';
    else
        komp.pos_tb = 'T';
    sdatateks_nomorini_int (&steks, 10, &x);
    komp.orentasi = (short)x;
    sdatateks_nomorini_int (&steks, 11, &x);
    sdatateks_nomorini_int (&steks, 12, &y);
    sdata_ambildatanomor(&steks, 8, temp);

    sprintf (komp.info,"XY=%d:%d SHA=%s", x,y, temp);
    sdata_ambildatanomor(&steks, 13, temp);
    if (temp[0])
    {
        strcat (komp.info, " VAL=");
        strcat (komp.info, temp);
    }
    sdata_ambildatanomor(&steks, 14, temp);
    if (temp[0])
    {
        strcat (komp.info, " TOL=");
        strcat (komp.info, temp);
    }
    sdata_hapussemua (&steks);
    return (sdata_tambah (komptr, &komp, sizeof(komp)));
}

int fabmaster_baca_fab_connection (char *teks, _sdataptr snodeptr, _sdataptr skonekptr)
{
    _sdatateks steks;
    int i;
    Str63 node_name;
    Str63 comp;
    Str63 pin;

    sdata_init (&steks);

    if (sdatateks_dariteks(&steks, teks, '!',64, 1)!=8)
    {
        sdata_hapussemua (&steks);
        return -1;
    }

    node_name[0] = comp[0] = pin[0] = 0;
    sdata_ambildatanomor(&steks, 1, node_name);
    sdata_ambildatanomor(&steks, 2, comp);
    sdata_ambildatanomor(&steks, 3, pin);
    strcat (comp,".");
    strcat (comp,pin); /* comp -> comp.pin */

    sdata_hapussemua (&steks);

    if (!node_name[0])
        return -1;
    if ((i=node_cari (snodeptr, node_name, 0))<0)
    {
        i = node_tambah (snodeptr, node_name, "");
    }

    if (i>=0)
    {
        return koneksi_tambah (skonekptr, i, comp);
    }

    return -1;
}

int fabmaster_baca_fab_assembly (char *teks, _sdataptr komptr)
{
    _sdatateks steks;
    int ret_status=0;
    int x1,y1,x2,y2;
    int i=-1;
    Str63 layer;
    Str63 comp;
    Str63 grap;
    char pos;

    sdata_init (&steks);

    if ( sdatateks_dariteks(&steks, teks, '!',64, 1)==17)
    {
        sdata_ambildatanomor(&steks, 15, comp);
        sdata_ambildatanomor(&steks, 13, layer);
        if (!comp[0])
            ret_status = -1;
        else
            ret_status = 0;
    }
    else
        ret_status = -1;

    if (ret_status>=0 && (strcmp(layer,"ASSEMBLY_TOP")==0 || strcmp(layer,"ASSEMBLY_BOTTOM")==0))
    {
        sdata_ambildatanomor(&steks, 1, grap);
        x1=y1=x2=y2=0;
        if (strcmp(grap,"LINE")==0)
        {
            sdatateks_nomorini_int (&steks, 4, &x1);
            sdatateks_nomorini_int (&steks, 5, &y1);
            sdatateks_nomorini_int (&steks, 6, &x2);
            sdatateks_nomorini_int (&steks, 7, &y2);

            ret_status = 1;
            //printf ("*1 %s %d %d %d %d\n", comp, x1,y1,x2,y2);
        }
        else if (strcmp(grap,"ARC")==0)
        {
            sdatateks_nomorini_int (&steks, 6, &x1);
            sdatateks_nomorini_int (&steks, 7, &y1);
            sdatateks_nomorini_int (&steks, 8, &x2);
            sdatateks_nomorini_int (&steks, 9, &y2);
            ret_status = 1;
        }
        else
            ret_status = -1;
    }

    if (ret_status>0)
    {
        //printf ("*2 %d %d %d %d\n", x1,y1,x2,y2);
        ret_status = komp_nama_tambahi_xy (komptr, comp, x1,y1,x2,y2);
    }

    sdata_hapussemua (&steks);
    return ret_status;
}

int fabmaster_baca_fab_pins (char *teks, _sdataptr konptr, _sdataptr netptr)
{
    _sdatateks steks;
    int x,y;
    Str63 comp,pin,loc; //,mirr;
    int i;
    int ret_status = -1;
    //char tb='B';

    sdata_init (&steks);

    if ( sdatateks_dariteks(&steks, teks, '!',64, 1)==12)
    {
        sdata_ambildatanomor(&steks, 10, loc);
        if (loc[0])
        {
            sdata_ambildatanomor(&steks, 8, comp);
            sdata_ambildatanomor(&steks, 4, pin);
            //sdata_ambildatanomor(&steks, 2, mirr);
            sdatateks_nomorini_int (&steks, 5, &x);
            sdatateks_nomorini_int (&steks, 6, &y);
            //if (strcmp(mirr,"NO")==0)
                //tb = 'T';
            if (pin[0])
            {
                strcat(comp,".");
                strcat(comp,pin);
                if (koneksi_cari_dkonek(konptr, comp, &i, sdata_nomorini(konptr))>=0)
                {
                    ret_status = tpt_tambah(netptr, i, loc[0], x, y, "");
                    //ret_status = tpt_tambah(netptr, i, tb, x, y, "");
                }
            }
        }
    }
    sdata_hapussemua (&steks);
    return ret_status;
}

int fabmaster_baca_fab_vias (char *teks, _sdataptr ndptr, _sdataptr netptr, _sdataptr trcptr)
{
    _sdatateks steks;
    int x,y;
    Str63 s_name,loc;
    int i;
    int data[SBOARD_DATALEN];
    int ret_status = -1;

    sdata_init (&steks);

    if ( sdatateks_dariteks(&steks, teks, '!',64, 1)==8)
    {
        sdata_ambildatanomor(&steks, 5, loc);
        sdata_ambildatanomor(&steks, 4, s_name);
        if (s_name[0])
        {
            sdatateks_nomorini_int (&steks, 1, &x);
            sdatateks_nomorini_int (&steks, 2, &y);
            if ((i=node_cari(ndptr, s_name, sdata_nomorini(ndptr)))>=0)
            {
                memset(data, 0, SBOARD_DATALEN*sizeof(int));
                data[0] = x;
                data[1] = y;
                switch(loc[0])
                {
                    case 'B':
                    data[7] = SBOARD_TRACE_BOTTOM;
                    break;
                    case 'T':
                    data[7] = SBOARD_TRACE_TOP;
                    break;
                    default:
                    data[7] = SBOARD_TRACE_INNER;
                }
                ret_status = trace_tambah (trcptr, i, SBOARD_VIA, data, SBOARD_DATALEN);
                if (loc[0])
                    ret_status = tpt_tambah(netptr, i, loc[0], x, y, "");
            }
        }
    }
    sdata_hapussemua (&steks);
    return ret_status;
}

int fabmaster_baca_fab_traces (char *teks, _sdataptr ndptr, _sdataptr trcptr)
{
    _sdatateks steks;
    int x,y;
    Str63 s_name,loc,type;
    int i=-1;
    int data[SBOARD_DATALEN];
    int ret_status = -1;

    sdata_init (&steks);
    s_name[0] = loc[0] = type[0] = 0;

    if ( sdatateks_dariteks(&steks, teks, '!',64, 1)==17)
    {
        sdata_ambildatanomor(&steks, 2, loc);
        sdata_ambildatanomor(&steks, 15, s_name);
        sdata_ambildatanomor(&steks, 3, type);
    }

    if (loc[0] && s_name[0] && type[0])
        i = node_cari (ndptr, s_name, -1);

    if (i>=0)
    {
        memset(data, 0, SBOARD_DATALEN*sizeof(int));
        if (strcmp(loc, "TOP")==0)
            data[7] = SBOARD_TRACE_TOP;
        else if (strcmp(loc, "BOTTOM")==0)
            data[7] = SBOARD_TRACE_BOTTOM;
        else
            data[7] = SBOARD_TRACE_INNER;

        if (strcmp(type,"LINE")==0)
        {
            sdatateks_nomorini_int (&steks, 6, &data[0]);
            sdatateks_nomorini_int (&steks, 7, &data[1]);
            sdatateks_nomorini_int (&steks, 8, &data[2]);
            sdatateks_nomorini_int (&steks, 9, &data[3]);
            ret_status = trace_tambah (trcptr, i, SBOARD_LINE, data, SBOARD_DATALEN);
        }
        else if (strcmp(type,"ARC")==0)
        {
            sdatateks_nomorini_int (&steks, 6, &data[0]);
            sdatateks_nomorini_int (&steks, 7, &data[1]);
            sdatateks_nomorini_int (&steks, 8, &data[2]);
            sdatateks_nomorini_int (&steks, 9, &data[3]);
            sdatateks_nomorini_int (&steks, 10, &data[4]);
            sdatateks_nomorini_int (&steks, 11, &data[5]);
            sdata_ambildatanomor(&steks, 14, type); /* clockwise */
            if (strcmp(type,"CLOCKWISE")==0)
                data[SBOARD_TRACE_CWF] = SBOARD_CWCLOCKWISE; /* 28 juli 07: 7 -> 6 */
            ret_status = trace_tambah (trcptr, i, SBOARD_ARC, data, SBOARD_DATALEN);
        }
    }

    sdata_hapussemua (&steks);
    return ret_status;
}

int fabmaster_baca_fab (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	//char nama[64];
	int p=0;
	//int i=0;
	FABM_AKSI2 aksi = FABM_NONE;
	//int node_id = -1;

#ifdef _BEEVEE_VERBOSE_
printf ("fabmaster_baca_fab %s ...\n", namafile);
#endif

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		//nama[0] = 0;
		line[0] = 0;
		while (1)
		{
			if (p==0 || line[p]==0)
			{
                p = 0;
                memset (line,0,256);
                fgets (line, 255, f);
                if (line[0]==0 && line[1]!=0)
                    line[0] = ' ';
                //if (i++<4) printf ("%d: %s",i,line);
                if (feof(f))
                    break;
			}
			//else
            //    printf (line);

			ReplaceChar (line, '\t',' ');
			ReplaceChar (line, 0xa, 0xd);
			TrimLeft(line);
			TrimRight(line);
			UpperText(line);


			if (GetLine(line,teks,&p))
			{
				if (teks[0]=='R')
                    aksi = FABM_HEADER;
				else if (teks[0]=='A')
				{
					if (strcmp (teks, FABM_PLACES_STR)==0)
						aksi = FABM_PLACES;
					else if (strcmp (teks, FABM_CONNECTIONS_STR)==0)
						aksi = FABM_CONNECTIONS;
                    else if (strcmp (teks, FABM_ASSEMBLY_STR)==0)
                        aksi = FABM_ASSEMBLY;
                    else if (strcmp (teks, FABM_PINS_STR)==0)
                        aksi = FABM_PINS;
                    else if (strcmp (teks, FABM_VIAS_STR)==0)
                        aksi = FABM_VIAS;
                    else if (strcmp (teks, FABM_TRACES_STR)==0)
                        aksi = FABM_TRACES;
					else
						aksi = FABM_NONE;

					//printf ("%d %s\n",aksi,teks);


					teks[0] = 0;
				}

				//if (aksi == FABM_NONE)
				//	printf ("%s\n", teks);

				if (aksi != FABM_NONE && teks[0]=='S')
				{
					//printf (teks);
					switch (aksi)
					{
					case FABM_PLACES:
						fabmaster_baca_fab_place (teks, &bptr->skomp);
						break;
                    case FABM_CONNECTIONS:
                        fabmaster_baca_fab_connection (teks, &bptr->snode, &bptr->skonek);
                        break;
                    case FABM_ASSEMBLY:
                        fabmaster_baca_fab_assembly (teks, &bptr->skomp);
                        break;
                    case FABM_PINS:
                        fabmaster_baca_fab_pins (teks, &bptr->skonek, &bptr->stestpoint);
                        break;
                    case FABM_VIAS:
                        fabmaster_baca_fab_vias (teks, &bptr->snode, &bptr->stestpoint, &bptr->strace);
                        break;
                    case FABM_TRACES:
                        fabmaster_baca_fab_traces (teks, &bptr->snode, &bptr->strace);
                        break;
					}
				}
				else if (aksi==FABM_HEADER)
				{
				    fabmaster_baca_fab_header (teks, bptr);
				}

			}
			else
			{
				//aksi = FABM_NONE;
				p = 0;
			}
		}
		fclose (f);

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

