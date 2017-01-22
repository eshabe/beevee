/*
sData in (std) C

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
Digunakan untuk koleksi data dalam bentuk pointer.

Usahakan tidak mengakses langsung isi pointer

----------------------------------
27 Des 2006
tambah sdata_dataini

15 agustus 2006
ubah "sdata_kenomor"
harus di range (0-ekor)
jika tidak --> kSDATAERR_NODATA

23 maret 2006
tambah "sdata_besardataini"
ganti _SHOWERROR dg _SHOWDEBUG

22 maret 2006
tambah "sdata_jumlahdata"
perbaiki sdata_baruiini

10-Maret-2006
tambah (besar) -- lihat .h
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdata.h"

//#define _SHOWDEBUG
//#define _DEVELOPMENT

void sdata_init (_sdataptr sdata)
{
	sdata->kepala = NULL;
	sdata->ini = NULL;
	sdata->ekor = NULL;
	sdata->djumlah = 0;
	sdata->dnomor = -1;
}

void sdata_cetakstatus (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("jumlah = %d\nnomor = %d\n", sdata->djumlah, sdata->dnomor);
#endif
}

int sdata_jumlahdata (_sdataptr sdata)
{
	return sdata->djumlah;
}

int sdata_nomorini (_sdataptr sdata)
{
	return sdata->dnomor;
}

short sdata_dikepalakah (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_dikepalakah called: %d, %d\n",sdata->dnomor, (sdata->djumlah)-1);
#endif
	if (sdata->ini)
		if (sdata->ini->belakang==NULL)
		{
#ifdef _SHOWDEBUG
			printf ("> YA\n");
#endif
			return 1;
		}

	return 0;
}

short sdata_diekorkah (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_diekorkah called: %d, %d\n",sdata->dnomor, (sdata->djumlah)-1);
#endif
	if (sdata->ini)
		if (sdata->ini->depan==NULL)
		{
#ifdef _SHOWDEBUG
			printf ("> YA\n");
#endif
			return 1;
		}

	return 0;
}

int sdata_kekepala (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_kekepala called\n");
#endif
	if (sdata->kepala)
	{
		sdata->ini = sdata->kepala;
		sdata->dnomor = 0;
	}
	return (sdata->dnomor);
}

int sdata_keekor (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_keekor called\n");
#endif
	if (sdata->ekor)
	{
		sdata->ini = sdata->ekor;
		sdata->dnomor = sdata->djumlah-1;
	}
	return (sdata->dnomor);
}

int sdata_kedepan (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_kedepan called\n");
#endif
	if (sdata->ini==NULL)
		return kSDATAERR_NOMORE;

	if (sdata->ini->depan)
	{
		sdata->ini = sdata->ini->depan;
		sdata->dnomor++;
		return (sdata->dnomor);
	}
	return kSDATAERR_NOMORE;
}

int sdata_kebelakang (_sdataptr sdata)
{
	if (sdata->ini==NULL)
		return kSDATAERR_NOMORE;

	if (sdata->ini->belakang)
	{
		sdata->ini = sdata->ini->belakang;
		sdata->dnomor--;
		return (sdata->dnomor);
	}
	return kSDATAERR_NOMORE;
}

int sdata_kenomor (_sdataptr sdata, int nomor)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_kenomor called > nomor=%d\n", nomor);
#endif
	if (sdata->djumlah==0 || nomor>=sdata->djumlah || nomor<0)
		return kSDATAERR_NODATA;

	if (nomor==0)
		return sdata_kekepala(sdata);
	else if (nomor>sdata->djumlah-1)
		return sdata_keekor(sdata);
	else if (nomor<sdata->dnomor)
	{
		while (nomor!=sdata->dnomor)
			sdata_kebelakang(sdata);
	}
	else if (nomor>sdata->dnomor)
	{
		while (nomor!=sdata->dnomor)
			sdata_kedepan(sdata);
	}

	return (sdata->dnomor);
}

int sdata_baru (_sdataptr sdata, void *data, int besar)
{

#ifdef _SHOWDEBUG
	printf ("> sdata_baru called\n");
#endif

	if ( !(sdata->djumlah==0 && sdata->dnomor==-1))
	{
#ifdef _SHOWDEBUG
		fprintf (stderr, "sdata_baru error kSDATAERR_NEWEMPTY\n");
#endif
		return kSDATAERR_NEWEMPTY;
	}

	if ( (sdata->kepala = (_dataptr)malloc (sizeof(_data)))==NULL )
		return kSDATAERR_ALLOC;

	if ( (sdata->kepala->isi = (void *)malloc (besar))==NULL )
	{
		free(sdata->kepala);
		return kSDATAERR_ALLOC;
	}

	memcpy (sdata->kepala->isi, data, besar);
	sdata->kepala->besar = besar;
	sdata->kepala->belakang = NULL;
	sdata->kepala->depan = NULL;
	sdata->ini = sdata->kepala;
	sdata->ini->besar = besar;
	sdata->ekor = sdata->kepala;

#ifdef _SHOWDEBUG
	printf ("> sdata_baru\n> kepala: %s (%d)\n> ini: %s\n> ekor: %s\n",
		sdata->kepala->isi,
		sdata->kepala->besar,
		sdata->ini->isi,
		sdata->ekor->isi);
#endif

	sdata->djumlah = 1;
	sdata->dnomor = 0;

	return 0; /* (sdata->dnomor) */
}

int sdata_tambah (_sdataptr sdata, void *data, int besar)
{
	_dataptr tdata;

#ifdef _SHOWDEBUG
	printf ("> sdata_tambah called\n");
#endif

	if ( sdata->djumlah==0 && sdata->dnomor==-1)
		return sdata_baru(sdata, data, besar);

	if ( (tdata = (_dataptr)malloc(sizeof(_data)))==NULL )
		return kSDATAERR_ALLOC;
	if ( (tdata->isi = (void *)malloc(besar))==NULL )
	{
		free (tdata);
		return kSDATAERR_ALLOC;
	}

	memcpy(tdata->isi,data, besar);
	tdata->besar = besar;
	tdata->depan = NULL;
	tdata->belakang = sdata->ekor;

	sdata->ekor->depan = tdata; /* ekor lama */
	sdata->ekor = tdata; /* ekor baru */

	sdata->ini=sdata->ekor;
	sdata->djumlah++;
	sdata->dnomor = sdata->djumlah-1;

#ifdef _SHOWDEBUG
	printf ("> sdata_tambah\n> belakang: %s\n> ekor: %s\n", sdata->ekor->belakang->isi, sdata->ekor->isi);
#endif

	return (sdata->dnomor);
}

int sdata_sisipkepala (_sdataptr sdata, void *data, int besar)
{
	_dataptr tdata;

#ifdef _SHOWDEBUG
	printf ("> sdata_sisipkepala called\n");
#endif

	if ( sdata->djumlah==0 && sdata->dnomor==-1)
		return sdata_baru(sdata, data, besar);

	if ( (tdata = (_dataptr)malloc(sizeof(_data)))==NULL )
		return kSDATAERR_ALLOC;
	if ( (tdata->isi = (void *)malloc(besar))==NULL )
	{
		free (tdata);
		return kSDATAERR_ALLOC;
	}

	memcpy(tdata->isi,data, besar);
	tdata->besar = besar;
	tdata->depan = sdata->kepala;
	tdata->belakang = NULL;

	sdata->kepala->belakang = tdata;
	sdata->kepala = tdata;

	sdata->djumlah++;

	return sdata_kekepala(sdata);
}


int sdata_sisip (_sdataptr sdata, void *data, int besar)
{
	_dataptr tdata,b;

#ifdef _SHOWDEBUG
	printf ("> sdata_sisip called\n");
#endif

	if ( sdata->djumlah==0 && sdata->dnomor==-1)
		return sdata_baru(sdata, data, besar);

	if (sdata_dikepalakah(sdata))
		return sdata_sisipkepala(sdata, data, besar);

	if ( (tdata = (_dataptr)malloc(sizeof(_data)))==NULL )
		return kSDATAERR_ALLOC;
	if ( (tdata->isi = (void *)malloc(besar))==NULL )
	{
		free (tdata);
		return kSDATAERR_ALLOC;
	}

	b = sdata->ini->belakang;

	memcpy(tdata->isi,data, besar);
	tdata->besar = besar;
	tdata->depan = sdata->ini;
	tdata->belakang = b;

	b->depan = tdata;
	sdata->ini->belakang = tdata;
	sdata->ini = tdata; /* ini baru */

	sdata->djumlah++;

	/* sdata->dnomor tidak berubah */

	return (sdata->dnomor);
}

int sdata_baruiini (_sdataptr sdata, void *data, int besar)
{
 if (sdata->ini)
    if (sdata->ini->isi)
    {
       free (sdata->ini->isi);
       if ( !(sdata->ini->isi = (void *) malloc (besar)) )
          return kSDATAERR_ALLOC;
       memcpy (sdata->ini->isi, data, besar);
	   sdata->ini->besar = besar;
	   return sdata->dnomor; /* 10 maret 2006 */
    }
  return  kSDATAERR_NODATA;
}

int sdata_besardataini (_sdataptr sdata)
{
	return sdata->ini->besar;
}

void *sdata_dataini (_sdataptr sdata)
{
    if (sdata->ini)
        return sdata->ini->isi;
    return NULL;
}

int sdata_ambildataini (_sdataptr sdata, void *data)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_ambildataini called\n");
#endif

	if (sdata->ini)
		if (sdata->ini->isi)
		{
			memcpy (data, sdata->ini->isi, sdata->ini->besar);
			return (sdata->dnomor);
		}

	return kSDATAERR_NODATA;
}

int sdata_ambildatanomor (_sdataptr sdata, int nomor, void *data)
{
	if (sdata_kenomor(sdata,nomor)>=0)
	{
		return sdata_ambildataini (sdata, data);
	}
    return kSDATAERR_NODATA;
}

int  sdata_hapuskepala (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_hapuskepala called > jumlah=%d\n", sdata->djumlah);
#endif

	if (sdata->djumlah==0)
		return 0;

	sdata->ini = sdata->kepala->depan;

	if (sdata->kepala)
	{
		if (sdata->kepala->isi)
			free (sdata->kepala->isi);
			free (sdata->kepala);
	}

	if (sdata->ini)
		sdata->ini->belakang = NULL;

	sdata->kepala = sdata->ini;
	sdata->djumlah--;
	sdata->dnomor = 0;

	if (sdata->djumlah==0)
		sdata->dnomor = -1;

	return (sdata->djumlah);
}

int  sdata_hapusekor (_sdataptr sdata)
{
	if (sdata->djumlah==0)
		return 0;
	sdata->ini = sdata->ekor->belakang;

	if (sdata->ekor)
	{
		if (sdata->ekor->isi)
			free (sdata->ekor->isi);
			free (sdata->ekor);
	}

	if (sdata->ini)
		sdata->ini->depan = NULL;
	sdata->ekor = sdata->ini;

	sdata->djumlah--;
	sdata->dnomor = sdata->djumlah-1;

	return (sdata->djumlah);
}

int sdata_hapusini (_sdataptr sdata)
{
	_dataptr b,d;

	if (sdata->djumlah==0)
		return 0;

	if (sdata_dikepalakah (sdata))
		sdata_hapuskepala (sdata);
	else if (sdata_diekorkah (sdata))
		sdata_hapusekor (sdata);
	else
	{
		b = sdata->ini->belakang;
		d = sdata->ini->depan;

		if (sdata->ini->isi)
			free(sdata->ini->isi);
		if (sdata->ini)
			free (sdata->ini);

		b->depan = d;
		d->belakang = b;
		sdata->ini = b;
		sdata->dnomor--;
		sdata->djumlah--;
	}
    return sdata->djumlah;
}

void sdata_hapussemua (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_hapussemua called\n");
#endif
	while (sdata_hapuskepala(sdata));
}


/* aplikasi */
#ifdef _DEVELOPMENT
void sdata_tampilisi (_sdataptr sdata)
{
	if (sdata->ini)
		if (sdata->ini->isi!=NULL)
			printf ("** %d:%s\n", sdata->dnomor, (char *)sdata->ini->isi);
}


void sdata_tampilsemua (_sdataptr sdata)
{
#ifdef _SHOWDEBUG
	printf ("> sdata_tampilsemua called\n");
#endif
	sdata_kekepala(sdata);
	while (1)
	{
		sdata_tampilisi(sdata);
		if (sdata_diekorkah(sdata)==1)
			break;
		sdata_kedepan(sdata);
	}
}

int main (int argc, char **argv)
{
	_sdata myData;
	char test[10];
    char *test2;

	sdata_init (&myData);

	strcpy(test, "saya");
	sdata_baru (&myData, test, sizeof(test));
	sdata_cetakstatus(&myData);

	strcpy(test, "dia");
	sdata_tambah (&myData, test, sizeof(test));
	sdata_cetakstatus(&myData);

	strcpy(test, "diass");
	sdata_tambah (&myData, test, sizeof(test));

    test2 = (char *)malloc(100);
    strcpy(test2,"asdsadsfsd sdfsdlkfsdf sdfsdfsdfkl sdflsdkfksd");
    sdata_tambah (&myData, test2, strlen(test2)+1);
    free(test2);

    test2 = (char *) malloc (7);
    strcpy (test2,"123456");
    sdata_tambah (&myData, test2, strlen(test2)+1);
    free(test2);


	sdata_tampilsemua (&myData);

	sdata_hapuskepala (&myData);
	sdata_tampilsemua (&myData);

    sdata_kenomor (&myData, 2);
	sdata_hapusini(&myData);

	strcpy(test, "saya2");
	sdata_tambah (&myData, test, sizeof(test));
	sdata_tampilsemua (&myData);

	sdata_kekepala(&myData);
	sdata_kedepan(&myData);
	sdata_hapusini(&myData); /* hapus nomor dua (diass) */
	sdata_tampilsemua (&myData);

	sdata_kenomor(&myData,0);
	strcpy(test, "sisip");
	sdata_sisip (&myData, test, sizeof(test));

	strcpy(test, "sisip2");
	sdata_sisip (&myData, test, sizeof(test));

	strcpy(test, "sisip3");
	sdata_sisip (&myData, test, sizeof(test));
	sdata_tampilsemua (&myData);

	sdata_ambildatanomor(&myData, 3, test);
	printf ("3! %s\n", test);

	sdata_hapussemua(&myData);
	sdata_cetakstatus(&myData);

	return kSDATAERR_NOERROR;
}

#endif
