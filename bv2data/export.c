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

/* 16 Oktober 2007
implementasi komp_xyinfo
*/

#include "../BV2/bv2explacement.h"
#include "export.h"
#include "unitconv.h"

int exportplacement (boardptr bptr, _sdataptr sptr, char *versi)
{
    _explace_option opt;
    komponen komp;
    Str255 line;
    Str63 temp;
    Str63 temp2;
    int p;
    int i=0;
    int x,y;
    double dx,dy;

    ExPlacementGetOption (&opt);
    if (sdata_kekepala (&bptr->skomp)>=0)
    {
        sprintf (line, "#BeeVee %s - Tab Delimited Placement",versi);
        sdatateks_tambahteks (sptr, line);
        if (opt.refdesonly)
        {
            opt.orient = opt.topbot = opt.pn = opt.shape = 0;
        }
        else if (!opt.refdesonly)
        {
            switch (opt.unit)
            {
                case BV2UNIT_INCHES:
                    sdatateks_tambahteks (sptr, "#Unit: inches"); break;
                case BV2UNIT_MICRON:
                    sdatateks_tambahteks (sptr, "#Unit: micron"); break;
                case BV2UNIT_MM:
                    sdatateks_tambahteks (sptr, "#Unit: mm"); break;
                 case BV2UNIT_MILS:
                 default:
                    sdatateks_tambahteks (sptr, "#Unit: mils");
           }
        }
        strcpy (line, "#RefDes");
        if (!opt.refdesonly)
            strcat (line, "\tX\tY");
        if (opt.orient)
            strcat (line, "\tOrient");
        if (opt.topbot)
            strcat (line, "\tTopBot");
        if (opt.pn)
            strcat (line, "\tPN");
        if (opt.shape)
            strcat (line, "\tShape");

        sdatateks_tambahteks (sptr, line);

        while(1)
        {
            sdata_ambildataini (&bptr->skomp, &komp);
            x = y = 0;
            p = 0;
            if (opt.loc == 1 /*TOP*/ && komp.pos_tb == 'T' ||
                opt.loc == 2 /*BOTTOM*/ && komp.pos_tb == 'B' ||
                opt.loc == 0)
            {
                if (opt.info)
                {
                    if (komp_xyinfo(&bptr->skomp, &x, &y)>=0)
                        p = 1;
                    /*
                    if (GetTextStartAtText (komp.info, temp, 63, "XY=", 0)>=0)
                    {
                        GetTextUntilAChar (temp, temp2, 63, ' ', &p);
                        ReplaceChar (temp2, '=',' ');
                        ReplaceChar (temp2, ':',' ');
                        sscanf (temp2,"%s %d %d", temp, &x, &y);
                        p = 1;
                    }
                    */
                }
                else if (komp.pos_x2 != komp.pos_x1 && komp.pos_y2 != komp.pos_y1)
                {
                    x = komp.pos_x1 + (komp.pos_x2 - komp.pos_x1)/2;
                    y = komp.pos_y1 + (komp.pos_y2 - komp.pos_y1)/2;
                    p = 1;
                }

                if (p)
                {
                    sprintf (line, "%s", komp.nama);

                    if (!opt.refdesonly)
                    {
                        dx = x; dy = y;
                        switch (opt.unit)
                        {
                            case BV2UNIT_INCHES:
                                dx = mils2inch ((double)x);
                                dy = mils2inch ((double)y);
                                break;
                            case BV2UNIT_MICRON:
                                dx = mils2micron ((double)x);
                                dy = mils2micron ((double)y);
                                break;
                            case BV2UNIT_MM:
                                dx = mils2mm ((double)x);
                                dy = mils2mm ((double)y);
                                break;
                            default:
                                dx = x;
                                dy = y;
                        }
                        sprintf (temp, "\t%.3f\t%.3f", dx, dy);
                        strcat (line, temp);
                    }

                    if (opt.orient)
                    {
                        sprintf (temp, "\t%d", komp.orentasi);
                        strcat (line, temp);
                    }
                    if (opt.topbot)
                    {
                        sprintf (temp, "\t%c", komp.pos_tb);
                        strcat (line, temp);
                    }
                    if (opt.pn)
                    {
                        if (GetTextStartAtText (komp.info, temp, 63, "PN=", 0)>=0)
                        {
                            p = 0;
                            GetTextUntilAChar (temp, temp2, 63, ' ', &p);
                            sprintf (temp, "\t%s", &temp2[3]);
                        }
                        else
                            strcpy (temp, "\tNO-PN");
                        strcat (line, temp);
                    }
                    if (opt.shape)
                    {
                        if (GetTextStartAtText (komp.info, temp, 63, "SHA=", 0)>=0)
                        {
                            p = 0;
                            GetTextUntilAChar (temp, temp2, 63, ' ', &p);
                            sprintf (temp, "\t%s", &temp2[4]);
                        }
                        else
                            strcpy (temp, "\tNO-SHAPE");
                        strcat (line, temp);
                    }

                    sdatateks_tambahteks (sptr, line);
                    ++i;
                } //p
            } // top-bottom

            if (sdata_diekorkah(&bptr->skomp))
                break;
            sdata_kedepan (&bptr->skomp);
        }
    }

    return i;
}

int exportsimpanfile (_sdataptr sptr, char *namafile)
{
    FILE *f;
    Str255 teks;

    if ( (f=fopen(namafile,"w"))!=NULL )
    {
        if (sdata_kekepala (sptr)>=0)
        {
            while (1)
            {
                sdata_ambildataini (sptr, teks);
                fprintf (f, "%s\n",teks);
                if (sdata_diekorkah (sptr))
                    break;
                sdata_kedepan (sptr);
            }
        }
        fclose (f);
        return 1;
    }

    return 0;
}
