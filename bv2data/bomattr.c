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
 *  bomattr.c
 *  BeeVee2
 *
 *  Sihabul Milah - 11 September 2007
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <math.h>

#include "bomattr.h"
#include "../sdata_c/sdatateks.h"

#define COMPPLACESTR ".PCBCOMPPLACE"

int bomatr_baca_file (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	static char komp[64];
	char tb[8];
	Str255 info;
	int x1,y1;
	int p,i;
	_sdatateks steks;

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		sdata_init(&steks);
		memset (komp,0,64);
		while (1)
		{
			fgets (line, 255, f);
			if (feof(f))
				break;

			p = 0;
			ReplaceChar (line, '\t',' ');
			ReplaceChar (line, 0xa, 0xd);
			TrimLeft(line);
			TrimRight(line);
			UpperText(line);

			if (GetLine(line,teks,&p))
			{
				if ( (i=sdatateks_dariteks (&steks, teks, ' ', 63, 0))>=2)
				{
                    if (strncmp(line,COMPPLACESTR,strlen(COMPPLACESTR))==0)
                    {
                        sdata_ambildatanomor (&steks, 1, komp);
                    }
                    else if (komp[0] && i==4)
                    {
                        double x,y;
                        int i_or;
                        sdatateks_nomorini_double (&steks, 0, &x);
                        sdatateks_nomorini_double (&steks, 1, &y);
                        sdatateks_nomorini_int (&steks, 2, &i_or);
                        sdata_ambildatanomor (&steks, 3, tb);
                        x1 = floor(x*1000);
                        y1 = floor(y*1000);

                        sprintf (info,"XY=%d:%d ", x1, y1);

						if (komp_tambah (&bptr->skomp, komp, info, tb[0], (short)i_or, 0, 0, 0, 0)>=0)
							komp_tambahi_xy (&bptr->skomp,  x1, y1, x1, y1);
                        komp[0] = 0;
					}
				}
				sdata_hapussemua(&steks);
			}
		}
		fclose(f);

       if (sdata_jumlahdata(&bptr->skomp)<=0)
            return 0;

	}
	else
	{
		return 0;
	}

	//komp_tampilsemua (&bptr->skomp);

	board_hitung_ataskiri (bptr);

	return 1;
}

