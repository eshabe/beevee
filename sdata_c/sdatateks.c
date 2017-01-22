/*
 *  sdatateks.c
 *
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

 *
 */

/*
22 Sept 2006 - sihab
- change sdatateks_jaditeks algortm
*/

#include <string.h>
#include <stdlib.h>
#include "sdatateks.h"
#include "../steks_c/stext.h"
#ifndef NULL
#define NULL	0
#endif

char *sdatateks_jaditeks (_sdatateksptr stptr, const char *sambungan, char *hasil)
{
	char *temp=NULL;
	char *temp1=NULL;
	char *temp2=NULL;
	int len_t;
    int besar_data=0;

	if (sdata_kekepala(stptr)<0)
		return NULL;
	while(1)
	{
	    besar_data+=sdata_besardataini(stptr);
        if (sdata_diekorkah(stptr))
            break;
        sdata_kedepan(stptr);
	}
	besar_data += (sdata_jumlahdata(stptr)*strlen(sambungan));
	++besar_data; /* add null space */

    if ( (temp1 = (char *)malloc(besar_data))==NULL )
        return NULL;
    memset (temp1, 0, besar_data);

	sdata_kekepala(stptr);
	while(1)
	{
		if (temp)
            free (temp);
		if ( (temp = (char *)malloc(sdata_besardataini(stptr)))==NULL )
			break;
		if (sdata_ambildataini(stptr, temp)>=0)
		{
			if (!temp1[0])
                strcpy(temp1, temp);
            else
                strcat(temp1, temp);

			if (sdata_diekorkah(stptr))
				break;
            if (temp1[0])
                strcat(temp1, sambungan);
            else
                strcpy (temp1, sambungan);

			sdata_kedepan(stptr);
		}
		else
			break;
	}
	temp2 = temp1;

	//strcpy (temp1,"XXXXX");
	if (hasil && temp1)
		strcpy(hasil, temp1);

	if (temp)
		free(temp);
	 if (temp1)
		free(temp1);
    return temp2;
	//return hasil;
}


int sdatateks_dariteks (_sdatateksptr stptr, const char *teks, char pemisah, int pj_maks, int bolehkosong)
{
	int p = 0;
	int i;
	char temp[256];

	if (!teks)
		return 0;
	if (!teks[0])
		return 0;

	while (1)
	{
		i=GetTextUntilAChar ((char *)teks, temp, pj_maks, pemisah, &p);

		if (i==0)
		{
			if (bolehkosong)
			{
				temp[0]=0;
				sdata_tambah(stptr, temp, 1);
			}
		}
		else
			sdata_tambah(stptr, temp, strlen(temp)+1);
		if (i<0)
			break;
	}

	//if (temp)
	//	free (temp);

	return sdata_jumlahdata(stptr);
}

int sdatateks_nomorini_int (_sdatateksptr stptr, int n, int *angka)
{
	char a[64];
	if (sdata_ambildatanomor(stptr, n, a)>=0)
	{
		*angka = atoi(a);
		return sdata_nomorini(stptr);
	}
	return -1;
}

int sdatateks_nomorini_double (_sdatateksptr stptr, int n, double *angka)
{
	char a[256];
	if (sdata_ambildatanomor(stptr, n, a)>=0)
	{
		*angka = atof(a);
		return sdata_nomorini(stptr);
	}
	return -1;
}


int sdatateks_tambahteks (_sdatateksptr stptr, char *teks)
{
    return sdata_tambah (stptr, teks, strlen(teks)+1);
}

