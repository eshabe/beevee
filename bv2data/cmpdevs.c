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

#include "cmpdevs.h"
#include "../sdata_c/sdatateks.h"

#include <string.h>
#include <stdlib.h>

void DoSaveChanges (FILE *f, _sdatateks *steks_new,_sdatateks *steks_move,_sdatateks *steks_del, char *changetxt)
{
    int i;
    char *line;

    line = (char *)malloc(512);
    if (line == NULL)
        return;

    fprintf (f, "*NEW: %d\n", sdata_jumlahdata(steks_new));
    i = sdata_kekepala (steks_new);
    while (i==0)
    {
        sdata_ambildataini (steks_new, line);
        fprintf (f, "%s", line);
        if (sdata_diekorkah(steks_new))
            break;
        sdata_kedepan(steks_new);
    }
    fprintf (f, "*DEL: %d\n", sdata_jumlahdata(steks_del));
    i = sdata_kekepala (steks_del);
    while (i==0)
    {
        sdata_ambildataini (steks_del, line);
        fprintf (f, "%s", line);
         if (sdata_diekorkah(steks_del))
            break;
        sdata_kedepan(steks_del);
   }
   fprintf (f, "*%s: %d\n", changetxt, sdata_jumlahdata(steks_move)/2);
    i = sdata_kekepala (steks_move);
    while (i==0)
    {
        sdata_ambildataini (steks_move, line);
        fprintf (f, "%s", line);
        if (sdata_diekorkah(steks_move))
            break;
        sdata_kedepan(steks_move);
    }
}

int DoCompareDevices (boardptr curbrdptr, boardptr cmpbrdptr, char *repfilename)
{
    int i = 0;
    komponen komp,komp2;
    FILE *f;
    Str63 namaB, namaT;
    char *line;

    _sdatateks steks_new,steks_move,steks_del;

    board_nama (curbrdptr, namaB);
    board_nama (cmpbrdptr, namaT);

    line = (char *)malloc(512);
    if (line == NULL)
        return 0;

    if ( (f=fopen(repfilename,"w"))==NULL )
        return 0;

    sdata_init (&steks_new);
    sdata_init (&steks_move);
    sdata_init (&steks_del);

    fprintf (f, "* BASE: %s - TARGET: %s\n", namaB, namaT);

    sdata_kekepala (&curbrdptr->skomp);
    while (1)
    {
        line[0] = 0;
        sdata_ambildataini(&curbrdptr->skomp, &komp);
        i = komp_cari (&cmpbrdptr->skomp, komp.nama, i);
        if (i<0)
        {
            i = 0;
            sprintf (line, "%s BASE: %c %d %d %d %d %d TARGET: N/A\n", komp.nama, komp.pos_tb, komp.orentasi,
                    komp.pos_x1, komp.pos_y1, komp.pos_x2, komp.pos_y2);
            sdata_tambah (&steks_del, line, strlen(line)+1);
        }
        else
        {
            sdata_ambildataini(&cmpbrdptr->skomp, &komp2);
            if (komp.pos_x1 != komp2.pos_x1 || komp.pos_x2 != komp2.pos_x2 ||
                komp.pos_y1 != komp2.pos_y1 || komp.pos_y2 != komp2.pos_y2 ||
                komp.orentasi != komp2.orentasi || komp.pos_tb != komp2.pos_tb)
            {
                sprintf (line, "%s BASE: %c %d %d %d %d %d", komp.nama, komp.pos_tb, komp.orentasi,
                    komp.pos_x1, komp.pos_y1, komp.pos_x2, komp.pos_y2);
                sdata_tambah (&steks_move, line, strlen(line)+1);
                sprintf (line, " TARGET: %c %d %d %d %d %d\n", komp2.pos_tb, komp2.orentasi,
                    komp2.pos_x1, komp2.pos_y1, komp2.pos_x2, komp2.pos_y2);
                sdata_tambah (&steks_move, line, strlen(line)+1);
            }
        }

        if (sdata_diekorkah(&curbrdptr->skomp))
            break;
        sdata_kedepan(&curbrdptr->skomp);

    }
    sdata_kekepala (&cmpbrdptr->skomp);
    i = 0;
    while (1)
    {
        sdata_ambildataini(&cmpbrdptr->skomp, &komp);
        i = komp_cari (&curbrdptr->skomp, komp.nama, i);
        if (i<0)
        {
            i = 0;
            sprintf (line, "%s BASE: N/A TARGET: %c %d %d %d %d %d\n", komp.nama, komp.pos_tb, komp.orentasi,
                    komp.pos_x1, komp.pos_y1, komp.pos_x2, komp.pos_y2);
            sdata_tambah (&steks_new, line, strlen(line)+1);
        }
        if (sdata_diekorkah(&cmpbrdptr->skomp))
            break;
        sdata_kedepan(&cmpbrdptr->skomp);
    }

    DoSaveChanges (f, &steks_new, &steks_move, &steks_del, "MOVE");

    /*
    fprintf (f, "*NEW: %d\n", sdata_jumlahdata(&steks_new));
    i = sdata_kekepala (&steks_new);
    while (i==0)
    {
        sdata_ambildataini (&steks_new, line);
        fprintf (f, "%s", line);
        if (sdata_diekorkah(&steks_new))
            break;
        sdata_kedepan(&steks_new);
    }
    fprintf (f, "*DEL: %d\n", sdata_jumlahdata(&steks_del));
    i = sdata_kekepala (&steks_del);
    while (i==0)
    {
        sdata_ambildataini (&steks_del, line);
        fprintf (f, "%s", line);
         if (sdata_diekorkah(&steks_del))
            break;
        sdata_kedepan(&steks_del);
   }
   fprintf (f, "*MOVE: %d\n", sdata_jumlahdata(&steks_move)/2);
    i = sdata_kekepala (&steks_move);
    while (i==0)
    {
        sdata_ambildataini (&steks_move, line);
        fprintf (f, "%s", line);
        if (sdata_diekorkah(&steks_move))
            break;
        sdata_kedepan(&steks_move);
    }
    */

    fclose(f);

    sdata_hapussemua (&steks_new);
    sdata_hapussemua (&steks_move);
    sdata_hapussemua (&steks_del);

    return 1;
}

int DoCompareDevicesInfo (boardptr curbrdptr, boardptr cmpbrdptr, char *repfilename)
{
    int i = 0;
    komponen komp,komp2;
    FILE *f;
    Str63 namaB, namaT;

    _sdatateks steks_new,steks_move,steks_del;
    char *line;

    board_nama (curbrdptr, namaB);
    board_nama (cmpbrdptr, namaT);

    line = (char *)malloc(512);
    if (line == NULL)
        return 0;

    if ( (f=fopen(repfilename,"w"))==NULL )
        return 0;

    sdata_init (&steks_new);
    sdata_init (&steks_move);
    sdata_init (&steks_del);

    fprintf (f, "* BASE: %s - TARGET: %s\n", namaB, namaT);
    sdata_kekepala (&curbrdptr->skomp);
    while (1)
    {
        line[0] = 0;
        sdata_ambildataini(&curbrdptr->skomp, &komp);
        i = komp_cari (&cmpbrdptr->skomp, komp.nama, i);
        if (i<0)
        {
            i = 0;
            //fprintf (f, "* %s is in BASE only\n", komp.nama);
            //fprintf (f, "%s\tBASE:\t%s\tTARGET:\tN/A\n", komp.nama, komp.info);
            sprintf (line, "%s\tBASE:\t%s\tTARGET:\tN/A\n", komp.nama, komp.info);
            sdata_tambah (&steks_del, line, strlen(line)+1);
        }
        else
        {
            sdata_ambildataini(&cmpbrdptr->skomp, &komp2);
            if (strcmp(komp.info,komp2.info)!=0)
            {
                //fprintf (f, "%s\tBASE:\t%s\tTARGET:\t%s\n", komp.nama, komp.info, komp2.info);
                sprintf (line, "%s\tBASE:\t%s", komp.nama, komp.info);
                sdata_tambah (&steks_move, line, strlen(line)+1);
                sprintf (line, "\tTARGET:\t%s\n", komp2.info);
                sdata_tambah (&steks_move, line, strlen(line)+1);
            }
        }
        if (sdata_diekorkah(&curbrdptr->skomp))
            break;
        sdata_kedepan(&curbrdptr->skomp);

    }
    sdata_kekepala (&cmpbrdptr->skomp);
    i = 0;
    while (1)
    {
        sdata_ambildataini(&cmpbrdptr->skomp, &komp);
        i = komp_cari (&curbrdptr->skomp, komp.nama, i);
        if (i<0)
        {
            i = 0;
            //fprintf (f, "* %s is in TARGET only\n", komp.nama);
            //fprintf (f, "%s\tBASE:\tN/A\tTARGET:\t%s\n", komp.nama, komp.info);
            sprintf (line, "%s\tBASE:\tN/A\tTARGET:\t%s\n", komp.nama, komp.info);
            sdata_tambah (&steks_new, line, strlen(line)+1);
        }
        if (sdata_diekorkah(&cmpbrdptr->skomp))
            break;
        sdata_kedepan(&cmpbrdptr->skomp);
    }

    DoSaveChanges (f, &steks_new, &steks_move, &steks_del, "CHANGE");
    fclose(f);

    return 1;
}

int DoCompareDevicesPlacement (boardptr curbrdptr, boardptr cmpbrdptr, char *repfilename)
{
    int i = 0;
    komponen komp,komp2;
    FILE *f;
    Str63 namaB, namaT;
    int x1,y1,x2,y2;

    _sdatateks steks_new,steks_move,steks_del;
    char *line;

    board_nama (curbrdptr, namaB);
    board_nama (cmpbrdptr, namaT);

    line = (char *)malloc(512);
    if (line == NULL)
        return 0;

    if ( (f=fopen(repfilename,"w"))==NULL )
        return 0;

    sdata_init (&steks_new);
    sdata_init (&steks_move);
    sdata_init (&steks_del);

    fprintf (f, "* BASE: %s - TARGET: %s\n", namaB, namaT);
    sdata_kekepala (&curbrdptr->skomp);
    while (1)
    {
        line[0] = 0;
        sdata_ambildataini(&curbrdptr->skomp, &komp);
        komp_xyinfo (&curbrdptr->skomp, &x1, &y1);
        i = komp_cari (&cmpbrdptr->skomp, komp.nama, i);
        if (i<0)
        {
            i = 0;
            sprintf (line, "%s\tBASE:\t%c %d %d\tTARGET:\tN/A\n", komp.nama, komp.pos_tb, x1, y1);
            sdata_tambah (&steks_del, line, strlen(line)+1);
        }
        else
        {
            sdata_ambildataini(&cmpbrdptr->skomp, &komp2);
            komp_xyinfo (&curbrdptr->skomp, &x2, &y2);
            if (strcmp(komp.info,komp2.info)!=0)
            {
                sprintf (line, "%s\tBASE:\t%c %d %d", komp.nama, komp.pos_tb, x1, y1);
                sdata_tambah (&steks_move, line, strlen(line)+1);
                sprintf (line, "\tTARGET:\t%c %d %d\n", komp2.pos_tb, x2, y2);
                sdata_tambah (&steks_move, line, strlen(line)+1);
            }
        }
        if (sdata_diekorkah(&curbrdptr->skomp))
            break;
        sdata_kedepan(&curbrdptr->skomp);

    }
    sdata_kekepala (&cmpbrdptr->skomp);
    i = 0;
    while (1)
    {
        sdata_ambildataini(&cmpbrdptr->skomp, &komp);
        komp_xyinfo (&cmpbrdptr->skomp, &x1, &y1);
        i = komp_cari (&curbrdptr->skomp, komp.nama, i);
        if (i<0)
        {
            i = 0;
            sprintf (line, "%s\tBASE:\tN/A\tTARGET:\t%c %d %d\n", komp.nama, komp.pos_tb, x1, y1);
            sdata_tambah (&steks_new, line, strlen(line)+1);
        }
        if (sdata_diekorkah(&cmpbrdptr->skomp))
            break;
        sdata_kedepan(&cmpbrdptr->skomp);
    }

    DoSaveChanges (f, &steks_new, &steks_move, &steks_del, "CHANGE");
    fclose(f);

    return 1;
}

