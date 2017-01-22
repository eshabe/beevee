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
 *  agsj50file.c
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 3/27/06.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "agsj50file.h"
#include "../sdata_c/sdatateks.h"

int agsj50_baca_plx (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char komp[64];
	char info[256];
	short or;
	int x1,x2,y1,y2;
	int p;
	int hasil = 0;
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

			if (line[0]=='D' && GetLine(line,teks,&p))
			{
				if ( sdatateks_dariteks (&steks, teks, ' ', 63, 0)==10 )
				{
					//printf ("%d %s\n", i, teks);
					sdata_ambildatanomor (&steks, 3, komp);
					sdata_ambildatanomor (&steks, 5, line); /* line = dummy */
					or = (short) atoi (line);
					sdata_ambildatanomor (&steks, 1, line); /* line = dummy */
					x1 = x2 = (int)(atof (line) / 25.4);
					//printf ("%d %s\n", x1, line);
					sdata_ambildatanomor (&steks, 2, line); /* line = dummy */
					y1 = y2 = (int)(atof (line) / 25.4);
					sdata_ambildatanomor (&steks, 4, line); /* line = dummy */
					sprintf (info,"XY=%d:%d ", x1, y1);
					strcat (info,"FAM=");
					strcat (info, line);
					sdata_ambildatanomor (&steks, 6, line); /* line = dummy */
					strcat (info, " PN="); strcat (info, line);
					sdata_ambildatanomor (&steks, 7, line); /* line = dummy */
					strcat (info, " FED="); strcat (info, line);
					sdata_ambildatanomor (&steks, 9, line); /* line = dummy */
					strcat (info, " SHA="); strcat (info, line);
					if (komp_tambah (&bptr->skomp, komp, info, 'T', or, 0, 0, 0, 0)>=0)
						komp_tambahi_xy (&bptr->skomp, x1, y1, x2, y2);
					/* komp_tambah (&bptr->skomp, komp, info, 'T', or, x1, y1, x2, y2); */
					sdata_hapussemua(&steks);
				}
			}
		}
		fclose(f);
		if (sdata_jumlahdata(&bptr->skomp)>0)
			hasil = 1;
	}

	if(!hasil)
        board_hapus(bptr);
    else
        board_hitung_ataskiri (bptr);

	return hasil;
}
