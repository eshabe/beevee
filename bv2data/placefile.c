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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "placefile.h"
#include "../sdata_c/sdatateks.h"
#include "unitconv.h"

//#include <windows.h>

int place_baca_file_kolom (char *namafile, boardptr bptr, importplacement_struct *ips)
{
    FILE *f;
    char teks[256];
    char line[256];
    char komp[64];
    char tb[8];
    char pn[64];
    char shape[64];
    char info[256];
    char info2[256];
    int x1,x2,y1,y2;
    double fx,fy;
    int ori;
    int p,i;
    int harusada=5;
    _sdatateks steks;

    if (namafile==NULL || bptr==NULL || ips==NULL)
        return 0;
    else if (namafile[0]==0 || ips->column==0)
        return 0;

    if (ips->pn[0]>=0)
        harusada++;
    if (ips->shape[0]>=0)
        harusada++;

    if ( (f=fopen(namafile,"r"))!=NULL )
    {
        sdata_init(&steks);
        i=0;
        while (1)
        {
            fgets (line, 255, f);
            if (feof(f))
                break;

            if (++i<ips->sline)
                continue;

            ReplaceChar (line, 0xa, 0xd);
            if (ips->token[0])
                ReplaceChar (line, ips->token[0], 0);
            UpperText(line);
            p = 0;

            if (GetLine(line,teks,&p))
            {
                if ( sdatateks_dariteks (&steks, teks, ips->delimiter[0], 63, 0)>=harusada)
                {
                    //printf ("%d %s\n", i, teks);
                    komp[0] = pn[0] = shape[0] = 0;
                    sdata_ambildatanomor (&steks, ips->refdes[0], komp);
                    sdata_ambildatanomor (&steks, ips->posz[0], tb);
                    //sdatateks_nomorini_int (&steks, ips->posx[0], &x1);
                    //sdatateks_nomorini_int (&steks, ips->posy[0], &y1);
                    sdatateks_nomorini_double (&steks, ips->posx[0], &fx);
                    sdatateks_nomorini_double (&steks, ips->posy[0], &fy);
                    sdatateks_nomorini_int (&steks, ips->orient[0], &ori);

                    if (ips->pn[0]>=0)
                        sdata_ambildatanomor (&steks, ips->pn[0], pn);
                    if (ips->shape[0]>=0)
                        sdata_ambildatanomor (&steks, ips->shape[0], shape);

                    switch (ips->unit)
                    {
                        case BV2UNIT_INCHES: /*inch*/
                        x1 = (int)inch2mils (fx);
                        y1 = (int)inch2mils (fy);
                        break;
                        case BV2UNIT_MICRON: /*micron*/
                        x1 = (int)micron2mils (fx);
                        y1 = (int)micron2mils (fy);
                        break;
                        case BV2UNIT_MM: /*mm*/
                        x1 = (int)mm2mils (fx);
                        y1 = (int)mm2mils (fy);
                        break;
                        default:
                        x1 = (int)fx;
                        y1 = (int)fy;
                    }

                    sprintf (info2, "XY=%d:%d ", x1,y1);
                    x2 = x1+KXY_GROW;
                    y2 = y1+KXY_GROW;
                    x1-=KXY_GROW;
                    y1-=KXY_GROW;

                    tb[0] = (tb[0]==ips->bottom[0])?'B':'T';

                    sprintf (info, "%s%s%s%s%s",
                             ((pn[0])?"PN=":""),
                             ((pn[0])?pn:""),
                             ((pn[0]&&shape[0])?" ":""),
                             ((shape[0])?"SHA=":""),
                             ((shape[0])?shape:"") );
                    strcat (info2, info);

                    komp_tambah (&bptr->skomp, komp, info2, tb[0], (short)ori, x1, y1, x2, y2);
                }
                sdata_hapussemua(&steks);
            }
        }
        fclose(f);

        if (sdata_jumlahdata(&bptr->skomp)<=0)
        {
            board_hapus(bptr);
            return 0;
        }
    }
    else
    {
        board_hapus(bptr);
        return 0;
    }

    return 1;
}

int place_baca_file_huruf (char *namafile, boardptr bptr, importplacement_struct *ips)
{
    FILE *f;
    char teks[256];
    char line[256];
    char temp[64];
    char komp[64];
    char tb[8];
    char pn[64];
    char shape[64];
    char info[256];
    int x1,x2,y1,y2;
    double fx,fy;
    int ori;
    int i,p;
    int harusada=5;

    if (namafile==NULL || bptr==NULL || ips==NULL)
        return 0;
    else if (namafile[0]==0 || ips->column!=0)
        return 0;

    if (ips->pn[0]>=0)
        harusada++;
    if (ips->shape[0]>=0)
        harusada++;

    ips->refdes[1]-=(ips->refdes[0]-1);
    ips->posx[1]-=(ips->posx[0]-1);
    ips->posy[1]-=(ips->posy[0]-1);
    ips->posz[1]-=(ips->posz[0]-1);
    ips->orient[1]-=(ips->orient[0]-1);
    ips->pn[1]-=(ips->pn[0]-1);
    ips->shape[1]-=(ips->shape[0]-1);

    if (ips->refdes[1]<=0)
        ips->refdes[1] = 1;
    if (ips->posx[1]<=0)
        ips->posx[1] = 1;
    if (ips->posy[1]<=0)
        ips->posy[1] = 1;
    if (ips->posz[1]<=0)
        ips->posz[1] = 1;
    if (ips->orient[1]<=0)
        ips->orient[1] = 1;
    if (ips->pn[1]<=0)
        ips->pn[1] = 1;
    if (ips->shape[1]<=0)
        ips->shape[1] = 1;

    if ( (f=fopen(namafile,"r"))!=NULL )
    {
        i = 0;
        while (1)
        {
            fgets (line, 255, f);
            if (feof(f))
                break;

            if (++i<ips->sline)
                continue;

            ReplaceChar (line, 0xa, 0xd);
            if (ips->token[0])
                ReplaceChar (line, ips->token[0], 0);
            UpperText(line);
            p = 0;

            if (GetLine(line,teks,&p))
            {

                pn[0] = shape[0] = 0;
                MidText (teks, komp, ips->refdes[0], ips->refdes[1]);
                RemoveChar (komp, ' ');
                MidText (teks, temp, ips->posz[0], ips->posz[1]);
                RemoveChar (temp, ' ');
                tb[0] = temp[0];
                MidText (teks, temp, ips->posx[0], ips->posx[1]);
                RemoveChar (temp, ' ');
                fx = atof(temp);
                MidText (teks, temp, ips->posy[0], ips->posy[1]);
                RemoveChar (temp, ' ');
                fy = atof(temp);
                MidText (teks, temp, ips->orient[0], ips->orient[1]);
                RemoveChar (temp, ' ');
                ori = atoi(temp);

                if (ips->pn[0]>=0)
                {
                    MidText (teks, pn, ips->pn[0], ips->pn[1]);
                    RemoveChar (pn, ' ');
                }
                if (ips->shape[0]>=0)
                {
                    MidText (teks, shape, ips->shape[0], ips->shape[1]);
                    RemoveChar (shape, ' ');
                }

                switch (ips->unit)
                {
                    case BV2UNIT_INCHES: /*inch*/
                    x1 = (int)inch2mils (fx);
                    y1 = (int)inch2mils (fy);
                    break;
                    case BV2UNIT_MICRON: /*micron*/
                    x1 = (int)micron2mils (fx);
                    y1 = (int)micron2mils (fy);
                    break;
                    case BV2UNIT_MM: /*mm*/
                    x1 = (int)mm2mils (fx);
                    y1 = (int)mm2mils (fy);
                    break;
                    default:
                    x1 = (int)fx;
                    y1 = (int)fy;
                }

                x2 = x1+KXY_GROW;
                y2 = y1+KXY_GROW;
                x1-=KXY_GROW;
                y1-=KXY_GROW;

                tb[0] = (tb[0]==ips->bottom[0])?'B':'T';

                sprintf (info, "%s%s%s%s%s",
                         ((pn[0])?"PN=":""),
                         ((pn[0])?pn:""),
                         ((pn[0]&&shape[0])?" ":""),
                         ((shape[0])?"SHAPE=":""),
                         ((shape[0])?shape:"") );

                komp_tambah (&bptr->skomp, komp, info, tb[0], (short)ori, x1, y1, x2, y2);
            }
        }
        fclose(f);

        if (sdata_jumlahdata(&bptr->skomp)<=0)
        {
            board_hapus(bptr);
            return 0;
        }
    }
    else
    {
        board_hapus(bptr);
        return 0;
    }

    board_hitung_ataskiri (bptr);
    bptr->punya_tempat = 1;
    return 1;
}

int place_baca_file (char *namafile, boardptr bptr, importplacement_struct *ips)
{
    if (ips->column)
       return place_baca_file_kolom (namafile, bptr, ips);
    else
       return place_baca_file_huruf (namafile, bptr, ips);
}
