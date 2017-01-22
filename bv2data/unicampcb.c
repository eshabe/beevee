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
 *  unicampcb.c
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 12/08/06.
 *

 membaca file Unicam (.PCB)

 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sboard.h"
#include "../sdata_c/sdatateks.h"


enum UNICAMPCB_AKSI2 {
UPCB_NONE,
UPCB_DATA
};
typedef int UNICAMPCB_AKSI2;

#define UNICAMPCB_STR "A!REFDES!CLASS!SUBCLASS!COMP_DEVICE_TYPE!COMP_PACKAGE!SYM_ROTATE!PIN_NUMBER!DRILL_HOLE_NAME!NET_NAME!PIN_X!PIN_Y!START_LAYER_NAME!END_LAYER_NAME!GRAPHIC_DATA_NAME!GRAPHIC_DATA_1!GRAPHIC_DATA_2!GRAPHIC_DATA_3!GRAPHIC_DATA_4!GRAPHIC_DATA_5!GRAPHIC_DATA_6!GRAPHIC_DATA_7!GRAPHIC_DATA_8!GRAPHIC_DATA_9!GRAPHIC_DATA_10!SYM_NAME!SYM_X!SYM_Y!NET_NAME!SYM_MIRROR!"
#define UNICAMPCB_DATA 31
int unicampcb_baca_pcb_header (char *teks)
{
    _sdatateks steks;
    int ret = -1;

    sdata_init (&steks);
    if (sdatateks_dariteks(&steks, teks, '!',64, 1)==UNICAMPCB_DATA)
      ret = 0;
    sdata_hapussemua (&steks);
    return ret;
}

int unicampcb_baca_pcb_etch (_sdatateksptr stptr, boardptr bptr)
{
    int node_id;
    Str63 layer,type,net_name,cw;
    int data[SBOARD_DATALEN];

    sdata_ambildatanomor (stptr, 3, layer);
    sdata_ambildatanomor (stptr, 14, type);
    sdata_ambildatanomor (stptr, 28, net_name);

    node_id = node_cari(&bptr->snode, net_name, -1);
    if (node_id<0)
        node_id = node_tambah(&bptr->snode, net_name, "");
    if (node_id<0)
        return -1;

    memset(data, 0, SBOARD_DATALEN*sizeof(int));

    data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_INNER;
    if (strcmp(layer,"TOP")==0)
        data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_TOP;
    else if (strcmp(layer,"BOTTOM")==0)
        data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_BOTTOM;

    if (strcmp(type,"LINE")==0)
    {
        sdatateks_nomorini_int (stptr, 15, &data[0]);
        sdatateks_nomorini_int (stptr, 16, &data[1]);
        sdatateks_nomorini_int (stptr, 17, &data[2]);
        sdatateks_nomorini_int (stptr, 18, &data[3]);
        return trace_tambah(&bptr->strace, node_id, SBOARD_LINE, data, SBOARD_DATALEN);
    }
    else if (strcmp(type,"ARC")==0)
    {
        sdatateks_nomorini_int (stptr, 15, &data[0]);
        sdatateks_nomorini_int (stptr, 16, &data[1]);
        sdatateks_nomorini_int (stptr, 17, &data[2]);
        sdatateks_nomorini_int (stptr, 18, &data[3]);
        sdatateks_nomorini_int (stptr, 19, &data[4]);
        sdatateks_nomorini_int (stptr, 20, &data[5]);
        sdata_ambildatanomor (stptr, 23, cw);
        if (strcmp(cw,"CLOCKWISE")==0)
            data[SBOARD_TRACE_CWF] = SBOARD_CWCLOCKWISE;
        else
            data[SBOARD_TRACE_CWF] = SBOARD_CWCOUNTERCLOCKWISE;
        return trace_tambah(&bptr->strace, node_id, SBOARD_ARC, data, SBOARD_DATALEN);
    }
    return -1;
}

int unicampcb_baca_pcb_via (_sdatateksptr stptr, boardptr bptr)
{
    int node_id;
    Str63 layer,type,net_name,cw;
    int data[SBOARD_DATALEN];

    sdata_ambildatanomor (stptr, 3, layer);
    sdata_ambildatanomor (stptr, 14, type);
    sdata_ambildatanomor (stptr, 28, net_name);

    node_id = node_cari(&bptr->snode, net_name, -1);
    if (node_id<0)
        node_id = node_tambah(&bptr->snode, net_name, "");
    if (node_id<0)
        return -1;

    memset(data, 0, SBOARD_DATALEN*sizeof(int));

    data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_INNER;
    if (strcmp(layer,"TOP")==0)
        data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_TOP;
    else if (strcmp(layer,"BOTTOM")==0)
        data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_BOTTOM;

    if (strcmp(type,"CIRCLE")==0) /* read CIRCLE only */
    {
        sdatateks_nomorini_int (stptr, 15, &data[0]);
        sdatateks_nomorini_int (stptr, 16, &data[1]);
        return trace_tambah(&bptr->strace, node_id, SBOARD_VIA, data, SBOARD_DATALEN);
    }
    /*
    if (strcmp(type,"LINE")==0)
    {
        sdatateks_nomorini_int (stptr, 15, &data[0]);
        sdatateks_nomorini_int (stptr, 16, &data[1]);
        sdatateks_nomorini_int (stptr, 17, &data[2]);
        sdatateks_nomorini_int (stptr, 18, &data[3]);
        return trace_tambah(&bptr->strace, node_id, SBOARD_LINE, data, SBOAR_DATALEN);
    }
    else if (strcmp(type,"ARC")==0)
    {
        sdatateks_nomorini_int (stptr, 15, &data[0]);
        sdatateks_nomorini_int (stptr, 16, &data[1]);
        sdatateks_nomorini_int (stptr, 17, &data[2]);
        sdatateks_nomorini_int (stptr, 18, &data[3]);
        sdatateks_nomorini_int (stptr, 19, &data[4]);
        sdatateks_nomorini_int (stptr, 20, &data[5]);
        sdata_ambildatanomor (stptr, 23, cw);
        if (strcmp(cw,"CLOCKWISE")==0)
            data[SBOARD_TRACE_CWF] = SBOARD_CWCLOCKWISE;
        else
            data[SBOARD_TRACE_CWF] = SBOARD_CWCOUNTERCLOCKWISE;
        return trace_tambah(&bptr->strace, node_id, SBOARD_ARC, data, SBOAR_DATALEN);
    }
    */
    return -1;
}


int unicampcb_baca_pcb_package (_sdatateksptr stptr, boardptr bptr)
{
    int ori;
    int i;
    int x1,y1,x2,y2;
    Str63 refdes;
    Str63 loc;
    Str63 pn;
    //Str63 shape;
    Str63 type;
    //Str255 info;

    sdata_ambildatanomor (stptr, 1, refdes);
    sdata_ambildatanomor (stptr, 3, loc);
    sdata_ambildatanomor (stptr, 4, pn);
    //sdata_ambildatanomor (stptr, 5, shape);
    sdatateks_nomorini_int (stptr, 6, &ori);

    if (strstr(loc,"BOTTOM"))
        strcpy(loc,"B");
    else if (strstr(loc,"TOP"))
        strcpy(loc,"T");
    else
        return -1;

    x1 = y1 = x2 = y2 = 0;
    if (refdes[0]=='#')
        return -1;

    i = komp_cari (&bptr->skomp, refdes, -1);
    if (i<0)
    {
        //sprintf (info,"PN=%s SHA=%s", pn, shape);
        //komp_tambah (&bptr->skomp, refdes, info, loc[0], (short) ori, x1, y1, x2, y2);
        komp_tambah (&bptr->skomp, refdes, "", loc[0], (short) ori, x1, y1, x2, y2);
    }

    /*
    if (strncmp(refdes,"TP",2)==0)
    {
        puts (refdes);
        puts (pn);
    }
    */

    if (strcmp(pn,"TESTPOINT")==0)
    {
        sdatateks_nomorini_int (stptr, 26, &x1);
        sdatateks_nomorini_int (stptr, 27, &y1);
        //printf ("* %s %d %d\n", refdes, x1, y1);
        return komp_tambahi_xy (&bptr->skomp,  x1, y1, x1, y1);
    }

    sdata_ambildatanomor (stptr, 14, type);
    if (strcmp(type,"LINE")==0 || strcmp(type,"RECTANGLE")==0)
    {
        sdatateks_nomorini_int (stptr, 15, &x1);
        sdatateks_nomorini_int (stptr, 16, &y1);
        sdatateks_nomorini_int (stptr, 17, &x2);
        sdatateks_nomorini_int (stptr, 18, &y2);
    }
    else if (strcmp(type,"ARC")==0)
    {
        sdatateks_nomorini_int (stptr, 17, &x1);
        sdatateks_nomorini_int (stptr, 18, &y1);
        sdatateks_nomorini_int (stptr, 19, &x2);
        sdatateks_nomorini_int (stptr, 20, &y2);
    }
    else
        return -1;

    return komp_tambahi_xy (&bptr->skomp,  x1, y1, x2, y2);
}

int unicampcb_baca_pcb_refdes (_sdatateksptr stptr, boardptr bptr)
{
    int ori;
    int i;
    int x,y;
    int x1,y1,x2,y2;
    Str63 refdes;
    Str63 loc;
    Str63 pn;
    Str63 shape;
    Str63 type;
    Str255 info;

    sdata_ambildatanomor (stptr, 1, refdes);
    sdata_ambildatanomor (stptr, 3, loc);
    sdata_ambildatanomor (stptr, 4, pn);
    sdata_ambildatanomor (stptr, 5, shape);
    sdatateks_nomorini_int (stptr, 6, &ori);
    sdatateks_nomorini_int (stptr, 26, &x);
    sdatateks_nomorini_int (stptr, 27, &y);

    if (strcmp(loc,"ASSEMBLY_BOTTOM")==0)
        strcpy(loc,"B");
    else if (strcmp(loc,"ASSEMBLY_TOP")==0)
        strcpy(loc,"T");
    else
        return -1;

    if (refdes[0]=='#')
        return -1;

    i = komp_cari (&bptr->skomp, refdes, -1);
    if (i>=0)
    {
        komp_barui_tb (&bptr->skomp, loc[0]);
        komp_barui_orentasi (&bptr->skomp, (short) ori);
        komp_xy (&bptr->skomp, &x1, &y1, &x2, &y2);
        if ((x1|x2|y1|y2) == 0)
            komp_tambahi_xy (&bptr->skomp, x, y, x, y);
        sprintf (info,"XY=%d:%d PN=%s SHA=%s", x,y, pn, shape);
        return komp_barui_info (&bptr->skomp, info);
    }
    return -1;
}

int unicampcb_baca_pcb_pin (_sdatateksptr stptr, boardptr bptr)
{
    Str63 refdes,loc,pn,type,net_name,pin; //,mirror;
    int x1,y1,x2,y2;
    int node_id;
    char tb;
    int data[SBOARD_DATALEN];

    sdata_ambildatanomor (stptr, 1, refdes);
    sdata_ambildatanomor (stptr, 3, loc);
    sdata_ambildatanomor (stptr, 4, pn);
    sdata_ambildatanomor (stptr, 7, pin);
    sdata_ambildatanomor (stptr, 14, type);
    sdata_ambildatanomor (stptr, 28, net_name);
    //sdata_ambildatanomor (stptr, 29, mirror);

    if (refdes[0]=='#')
        return -1;

    node_id = node_cari(&bptr->snode, net_name, -1);
    if (node_id<0)
        node_id = node_tambah(&bptr->snode, net_name, "");
    if (node_id<0)
        return -1;

    strcat(refdes,".");
    strcat(refdes,pin);
    tb = (strcmp(loc,"BOTTOM")==0)?'B':'T';

    koneksi_tambah (&bptr->skonek, node_id, refdes);

    /* testpin */
    if (strcmp(pn,"TESTPOINT")==0)
    {
        sdatateks_nomorini_int (stptr, 15, &x1);
        sdatateks_nomorini_int (stptr, 16, &y1);
        tpt_tambah (&bptr->stestpoint, node_id, tb, x1, y1, "");
    }

    if (strcmp(type,"FIG_RECTANGLE")==0)
    {
        memset(data, 0, SBOARD_DATALEN*sizeof(int));
        //printf ("%d -----\n", node_id);

        data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_INNER;
        if (strcmp(loc,"TOP")==0)
            data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_TOP;
        else if (strcmp(loc,"BOTTOM")==0)
            data[SBOARD_TRACE_LAYERF] = SBOARD_TRACE_BOTTOM;

        sdatateks_nomorini_int (stptr, 15, &x1);
        sdatateks_nomorini_int (stptr, 16, &y1);
        sdatateks_nomorini_int (stptr, 17, &x2);
        sdatateks_nomorini_int (stptr, 18, &y2);

        data[0] = x1;
        data[1] = y1;
        data[2] = x1+x2;
        data[3] = y1+y2;

        trace_tambah(&bptr->strace, node_id, SBOARD_RECT, data, SBOARD_DATALEN);
    }

    return node_id;
}

int unicampcb_baca_pcb (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[512];
	char line[512];
	int p=0;
	UNICAMPCB_AKSI2 aksi = UPCB_NONE;
	_sdatateks steks;
	Str63 refdes, mclass, msubclass, net_name;

#ifdef _BEEVEE_VERBOSE_
printf ("unicampcb_baca_pcb %s ...\n", namafile);
#endif

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		line[0] = 0;
		sdata_init (&steks);
		while (1)
		{
			if (p==0 || line[p]==0)
			{
                p = 0;
                fgets (line, 511, f);
                //line[512] = 0;

                if (feof(f))
                    break;
			}

			ReplaceChar (line, '\t',' ');
			ReplaceChar (line, 0xa, 0xd);
			TrimLeft(line);
			TrimRight(line);
			UpperText(line);


			if (GetLine(line,teks,&p))
			{
				if (teks[0]=='A')
				{
                    if (strcmp(teks,UNICAMPCB_STR)==0)
                    {

                       if (unicampcb_baca_pcb_header (teks)<0)
                            break;
                        else
                            aksi = UPCB_DATA;
                    }
                    else
                        aksi = UPCB_NONE;
				}

				if (aksi == UPCB_DATA && teks[0]=='S')
				{
					if (sdatateks_dariteks(&steks, teks, '!',64, 1)!=UNICAMPCB_DATA)
					{
                        sdata_hapussemua (&steks);
                        continue;
					}

					sdata_ambildatanomor (&steks, 1, refdes);
					sdata_ambildatanomor (&steks, 2, mclass);
					sdata_ambildatanomor (&steks, 3, msubclass);
					sdata_ambildatanomor (&steks, 28, net_name);

					if (strcmp(mclass,"ETCH")==0 && net_name[0]) // traces
					{
                        unicampcb_baca_pcb_etch (&steks, bptr);
					}
					else if (strcmp(mclass,"VIA CLASS")==0 && net_name[0]) //traces - via
					{
					    unicampcb_baca_pcb_via (&steks, bptr);
					}
					else if (strcmp(mclass,"PACKAGE GEOMETRY")==0 && refdes[0])
					{
					    //puts(line);
					    unicampcb_baca_pcb_package (&steks, bptr);
					}
					else if (strcmp(mclass,"REF DES")==0 && refdes[0])
					{
					    unicampcb_baca_pcb_refdes (&steks, bptr);
					}
					else if (strcmp(mclass,"PIN")==0 && refdes[0] && net_name[0])
					{
					    unicampcb_baca_pcb_pin (&steks, bptr);
					}



				}
			}
			else
			{
				//aksi = UPCB_NONE;
				p = 0;
			}

			sdata_hapussemua (&steks);
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

    board_hitung_ataskiri (bptr);
    bptr->punya_tempat = 1;

    return ((sdata_jumlahdata(&bptr->snode)==0 ||
        sdata_jumlahdata(&bptr->skomp)==0 ||
        sdata_jumlahdata(&bptr->strace)==0 ||
        sdata_jumlahdata(&bptr->skonek)==0)?0:1);
}

