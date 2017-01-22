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
 *  generalxy.c
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 3/27/06.
 *
 */


/*
My General XY (TEXT with format: REFDES{spasi}T|B{spasi}X1{spasi}Y1{spasi}X2{spasi}Y2 -- X2 & Y2 opsional)
*/

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>

#include "generalxy.h"
#include "../sdata_c/sdatateks.h"

int gene_baca_file (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char komp[64];
	char tb[64]; /* 20071016 */
	int x1,x2,y1,y2;
	int p,i;
	_sdatateks steks;

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		sdata_init(&steks);
		while (1)
		{
			fgets (line, 255, f);
			if (feof(f))
				break;

			komp[0] = 0;
			p = 0;
			ReplaceChar (line, '\t',' ');
			ReplaceChar (line, 0xa, 0xd);
			TrimLeft(line);
			TrimRight(line);
			UpperText(line);

			if (GetLine(line,teks,&p))
			{
				if ( (i=sdatateks_dariteks (&steks, teks, ' ', 63, 0))>=4)
				{
					//printf ("%d %s\n", i, teks);
					sdata_ambildatanomor (&steks, 0, komp);
					sdata_ambildatanomor (&steks, 1, tb);
					sdata_ambildatanomor (&steks, 2, line); /* line = dummy */
					x1 = atoi (line);
					sdata_ambildatanomor (&steks, 3, line); /* line = dummy */
					y1 = atoi (line);
					if (tb[0]!='B')
                        tb[0] = 'T';
					if (i==6)
					{
						sdata_ambildatanomor (&steks, 4, line); /* line = dummy */
						x2 = atoi (line);
						sdata_ambildatanomor (&steks, 5, line); /* line = dummy */
						y2 = atoi (line);
						komp_tambah (&bptr->skomp, komp, "", tb[0], 0, x1, y1, x2, y2);
					}
					else
					{
						if (komp_tambah (&bptr->skomp, komp, "", tb[0], 0, 0, 0, 0, 0)>=0)
							komp_tambahi_xy (&bptr->skomp,  x1, y1, x1, y1);
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

