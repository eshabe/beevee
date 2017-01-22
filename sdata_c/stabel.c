/*
sTabel in (std) C
Sihabul Milah

Digunakan untuk koleksi data dalam bentuk tabel.
Pengembangan dari "sdata"

Usahakan tidak mengakses langsung isi pointer

----------------------------------
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stabel.h"

void stabel_init (_stabelptr stblptr, int kolom)
{
    sdata_init (&stblptr->sdata);
    sdata_init (&stblptr->brs);
    sdata_init (&stblptr->namakolom);
    stblptr->tipekolom = (unsigned char *)malloc(kolom);
    memset (stblptr->tipekolom , _tipeBebas, kolom);
    stblptr->jumlah_kolom = kolom;
}

void stabel_hapus (_stabelptr stblptr)
{
    sdata_hapussemua (&stblptr->sdata);
    sdata_hapussemua (&stblptr->brs);
    sdata_hapussemua (&stblptr->namakolom);
    if (stblptr->tipekolom)
        free(stblptr->tipekolom);
    stblptr->jumlah_kolom = 0;
}

int stabel_jumlahkolom (_stabelptr stblptr)
{
    return stblptr->jumlah_kolom;
}

int stabel_jumlahbaris (_stabelptr stblptr)
{
    double d = sdata_jumlahdata(&stblptr->sdata);
    int i;
    d = d/stblptr->jumlah_kolom;
    i = floor(d);
    return i;
}

int stabel_jumlahsel (_stabelptr stblptr)
{
    return sdata_jumlahdata(&stblptr->sdata);
}


int stabel_adanamakolom (_stabelptr stblptr)
{
    return ((sdata_jumlahdata(&stblptr->namakolom)>0)?1:0);
}

short stabel_namakolomsesuaikah (_stabelptr stblptr)
{
    return ((sdata_jumlahdata(&stblptr->namakolom)==stblptr->jumlah_kolom)?1:0);
}

int stabel_nomorselini (_stabelptr stblptr)
{
    return sdata_nomorini (&stblptr->sdata);
}

int stabel_barisini (_stabelptr stblptr)
{
    return sdata_nomorini(&stblptr->sdata)/stblptr->jumlah_kolom;
}

int stable_kolomini (_stabelptr stblptr)
{
   return sdata_nomorini(&stblptr->sdata)%stblptr->jumlah_kolom;
}

short stabel_dibaris0kah (_stabelptr stblptr)
{
    if (sdata_nomorini(&stblptr->sdata)<stblptr->jumlah_kolom)
        return 1;
    return 0;
}

short stabel_dibaris0datakah (_stabelptr stblptr)
{
    if (sdata_nomorini(&stblptr->sdata) < stblptr->jumlah_kolom)
        return 1;
    return 0;
}

short stabel_dibarisakhirkah (_stabelptr stblptr)
{
    if (sdata_nomorini(&stblptr->sdata) >= (sdata_jumlahdata(&stblptr->sdata)-stblptr->jumlah_kolom))
        return 1;
    return 0;
}

short stabel_diselkepalakah (_stabelptr stblptr)
{
    if (stblptr->sdata.ini)
    {
        if (stblptr->sdata.ini->belakang==NULL)
        {
            return 1;
        }
        return 0;
    }
    return 1;
}

short stabel_diselekorkah (_stabelptr stblptr)
{
    if (stblptr->sdata.ini)
    {
        if (stblptr->sdata.ini->depan==NULL)
        {
            return 1;
        }
        return 0;
    }
    return 1;
}

int stabel_keselkepala (_stabelptr stblptr)
{
    return sdata_kekepala (&stblptr->sdata);
}

int stabel_keselekor (_stabelptr stblptr)
{
    return sdata_keekor (&stblptr->sdata);
}

int stabel_kedepan1baris (_stabelptr stblptr)
{
    int i=sdata_kedepan(&stblptr->sdata);
    if (i>=0)
        i -= i%stblptr->jumlah_kolom;
    return i;
}

int stabel_kebelakang1baris (_stabelptr stblptr)
{
    int i=sdata_kebelakang(&stblptr->sdata);
    if (i>=0)
        i -= (i%stblptr->jumlah_kolom);
    return i;
}

int stabel_keselnomor (_stabelptr stblptr, int nomor)
{
    return sdata_kenomor (&stblptr->sdata, nomor);
}

int stabel_kebaris (_stabelptr stblptr, int baris)
{
    int i;

    i = baris * stblptr->jumlah_kolom;
    i -= (i%stblptr->jumlah_kolom);
    return stabel_keselnomor (stblptr, i);
}

int stabel_kekolom (_stabelptr stblptr, int kolom)
{
    int i = sdata_nomorini (&stblptr->sdata);
    i -= (i%stblptr->jumlah_kolom);
    i += kolom;
    return  stabel_keselnomor (stblptr, i);
}


int stabel_seltambah (_stabelptr stblptr, void *data, int besar)
{
    return sdata_tambah (&stblptr->sdata, data, besar);
}

int stabel_selsisip (_stabelptr stblptr, void *data, int besar)
{
    return sdata_sisip (&stblptr->sdata, data, besar);
}

int stabel_baristambah (_stabelptr stblptr, _sdataptr brsptr)
{
    int l = sdata_jumlahdata(brsptr);
    int i = 0;
    unsigned char kosong = 0;

    for (i=0;i<stblptr->jumlah_kolom;i++)
    {
        if (sdata_kenomor(brsptr, i)>=0)
        {
            stabel_seltambah(stblptr, brsptr->ini->isi, brsptr->ini->besar);
        }
        else
        {
            stabel_seltambah(stblptr, &kosong, sizeof(unsigned char));
        }
    }
    return sdata_jumlahdata(&stblptr->sdata);
}

int stabel_barissisip (_stabelptr stblptr, _sdataptr brsptr)
{
    int l = sdata_jumlahdata(brsptr);
    int i = 0;
    unsigned char kosong = 0;

    for (i=stblptr->jumlah_kolom-1;i>=0;i--) /* menyisispi data sebelum "ini" */
    {
        if (sdata_kenomor(brsptr, i)>=0)
        {
            stabel_selsisip (stblptr, sdata_dataini(brsptr), sdata_besardataini(brsptr));
        }
        else
        {
            stabel_selsisip (stblptr, &kosong, sizeof(unsigned char));
        }
    }
    return sdata_jumlahdata(&stblptr->sdata);
}

int stabel_baruiselini (_stabelptr stblptr, void *data, int besar)
{
    return sdata_baruiini(&stblptr->sdata, data, besar);
}

int stabel_baruibarisini (_stabelptr stblptr, _sdataptr brsptr)
{
    int n = sdata_nomorini (&stblptr->sdata);
    int i = n%stblptr->jumlah_kolom;
    if (i!=0)
        sdata_kenomor (&stblptr->sdata, n-i);

    for (i=0;i<stblptr->jumlah_kolom;i++)
    {
        if (sdata_kenomor(brsptr, i)>=0)
        {
            stabel_baruiselini(stblptr, sdata_dataini(brsptr), sdata_besardataini(brsptr));
        }
        sdata_kedepan (&stblptr->sdata);
    }
    return (i==stblptr->jumlah_kolom);
}

int stabel_ambildataselini (_stabelptr stblptr, void *data)
{
    return sdata_ambildataini (&stblptr->sdata, data);
}

int stabel_ambildatasel (_stabelptr stblptr, int baris, int kolom, void *data)
{
    int i = stblptr->jumlah_kolom * (stabel_adanamakolom(stblptr) + baris);
    i += kolom;
    if (sdata_kenomor(&stblptr->sdata,i)>=0)
    {
        return stabel_ambildataselini (stblptr, data);
    }
    return kSDATAERR_NODATA;
}

int stabel_ambildatabaris (_stabelptr stblptr, int baris, _sdataptr sbrsptr)
{
    int i;
    int n;
    sdata_hapussemua (sbrsptr);
    if ((n=stabel_kebaris (stblptr, baris))<0)
        return kSDATAERR_NODATA;
    for (i=0;i<stblptr->jumlah_kolom;i++)
    {
        if (stabel_keselnomor (stblptr, n+i)>=0)
        {
            sdata_tambah (sbrsptr,sdata_dataini(&stblptr->sdata), sdata_besardataini(&stblptr->sdata));
        }
        else
            break;
    }
    return (i==stblptr->jumlah_kolom);
}


int stabel_hapusselini (_stabelptr stblptr)
{
    return sdata_hapusini (&stblptr->sdata);
}

void stabel_hapussemuasel (_stabelptr stblptr)
{
    sdata_hapussemua (&stblptr->sdata);
}

int stable_hapus1barisini (_stabelptr stblptr)
{
    int i = stabel_barisini (stblptr);
    int n;

    if ((n=stabel_kebaris (stblptr, i))>=0)
    {
        for (i=0;i<stblptr->jumlah_kolom;i++)
        {
            if (stabel_keselnomor (stblptr, n+i)>=0)
                stabel_hapusselini (stblptr);
        }
        return sdata_jumlahdata(&stblptr->sdata);
    }
    return kSDATAERR_NODATA;
}

int stable_hapussejumlahbarisini (_stabelptr stblptr, int jumlah)
{
    int i = stabel_barisini (stblptr);
    int n;

    if ((n=stabel_kebaris (stblptr, i))>=0)
    {
        for (i=0;i<stblptr->jumlah_kolom*jumlah;i++)
        {
            if (stabel_keselnomor (stblptr, n+i)>=0)
                stabel_hapusselini (stblptr);
        }
        return sdata_jumlahdata(&stblptr->sdata);
    }
    return kSDATAERR_NODATA;
}

int stabel_tambahnamakolom (_stabelptr stblptr, char *nama)
{
    return sdata_tambah (&stblptr->namakolom, nama, strlen(nama)+1);
}

int stabel_settipekolom (_stabelptr stblptr, int kolom, unsigned char tipe)
{
    memset((stblptr->tipekolom+kolom),tipe,1);
    return *(stblptr->tipekolom+kolom);
}

unsigned char stabel_kolomtipeapa (_stabelptr stblptr, int kolom)
{
    return *(stblptr->tipekolom+kolom);
}

void stabel_barisbaru0 (_stabelptr stblptr)
{
    sdata_hapussemua (&stblptr->brs);
}

int stabel_barisbaru0tambah (_stabelptr stblptr, void *data, int besar)
{
    return sdata_tambah (&stblptr->brs, data, besar);
}

int stabel_barisbaru0tambahangka (_stabelptr stblptr, void *angka, int besar)
{
    return sdata_tambah (&stblptr->brs, angka, sizeof(besar));
}

int stabel_barisbaru0tambahangkaInt16 (_stabelptr stblptr, short angka)
{
    return stabel_barisbaru0tambahangka (stblptr, &angka, sizeof(short));
}

int stabel_barisbaru0tambahangkaInt32 (_stabelptr stblptr, int angka)
{
    return stabel_barisbaru0tambahangka (stblptr, &angka, sizeof(int));
}

int stabel_barisbaru0tambaChr (_stabelptr stblptr, char c)
{
    return sdata_tambah (&stblptr->brs, &c, sizeof(char));
}


int stabel_barisbaru0tambahStr (_stabelptr stblptr, char *kata)
{
    return sdata_tambah (&stblptr->brs, kata, strlen(kata)+1);
}

int stabel_barisbaru0tambahBebas (_stabelptr stblptr, void *data, int besar)
{
    return sdata_tambah (&stblptr->brs, data, besar);
}

int stabel_barisbaru0simpan (_stabelptr stblptr)
{
    return stabel_baristambah (stblptr, &stblptr->brs);
}


/* aplikasi */
#ifdef _DEVELOPMENT_STABEL
void stabel_tampilnamakolom (_stabelptr stblptr)
{
    int i;
    for (i=0;i<stabel_jumlahkolom(stblptr);i++)
    {
        if (sdata_kenomor(&stblptr->namakolom, i)<0)
            break;
        printf ("|");
        if (stblptr->namakolom.ini)
            if (stblptr->namakolom.ini->isi!=NULL)
            printf ("%d:%s", stblptr->namakolom.dnomor, (char *)stblptr->namakolom.ini->isi);
    }
     printf ("|\n");
}

void stabel_tampilisi (_stabelptr stblptr)
{
    int i;
    int d;
    short s;
    double dbl;
    char c;
    char *str;
    for (i=0;i<stabel_jumlahkolom(stblptr);i++)
    {
        if (stabel_kekolom(stblptr, i)<0)
            break;
        printf ("|");
        if (stblptr->sdata.ini)
            if (stblptr->sdata.ini->isi!=NULL)
            {
                switch (stabel_kolomtipeapa(stblptr, i))
                {
                    //case _tipeBebas:
                    case _tipeStr:
                    str = malloc (sdata_besardataini(stblptr));
                    sdata_ambildataini (stblptr, str);
                    printf ("%s", str);
                    break;

                    case _tipeInt16:
                    sdata_ambildataini (stblptr, &s);
                    printf ("%d", s);
                    break;
                    case _tipeUInt16:
                    sdata_ambildataini (stblptr, (void *)&s);
                    printf ("%u", s);
                    break;
                    case _tipeInt32:
                    sdata_ambildataini (stblptr, &d);
                    printf ("%d", d);
                    break;
                    case _tipeUInt32:
                    sdata_ambildataini (stblptr, &d);
                    printf ("%u", d);
                    break;
                    case _tipeDbl:
                    sdata_ambildataini (stblptr, &dbl);
                    printf ("%f", dbl);
                    break;
                    case _tipeChr:
                    sdata_ambildataini (stblptr, &c);
                    printf ("%c", c);
                    break;
                    default:
                    printf ("_tipeBebas");
                }

            }
    }
     printf ("|\n");
}


void stabel_tampilsemua (_stabelptr stblptr)
{
    int i;
#ifdef _SHOWDEBUG
    printf ("> stabel_tampilsemua called\n");
#endif

    i=0;
    while (i<stabel_jumlahbaris(stblptr))
    {
        stabel_kebaris(stblptr, i);
        stabel_tampilisi(stblptr);
        if (stabel_diselekorkah(stblptr))
            break;
        ++i;
    }
}

int main (int argc, char **argv)
{
    _stabel myTabel;
    char test[10];
    char *test2;
    int i;
    short x = 10;

    stabel_init (&myTabel,3);
    stabel_tambahnamakolom (&myTabel, "Nomor");
    stabel_tambahnamakolom (&myTabel, "Nama");
    stabel_tambahnamakolom (&myTabel, "Kode");
    stabel_settipekolom (&myTabel, 0, _tipeInt16);
    stabel_settipekolom (&myTabel, 1, _tipeStr);
    stabel_settipekolom (&myTabel, 2, _tipeStr);

    stabel_barisbaru0 (&myTabel);
    stabel_barisbaru0tambahangkaInt16 (&myTabel,x);
    stabel_barisbaru0tambah (&myTabel,"Saya",5);
    stabel_barisbaru0tambah (&myTabel,"S",2);
    stabel_barisbaru0simpan (&myTabel);

    for (i=2;i<13;i++)
    {
        stabel_barisbaru0 (&myTabel);
        stabel_barisbaru0tambahangkaInt16 (&myTabel,i);
        sprintf (test, "Saya%d", i);
        stabel_barisbaru0tambah (&myTabel,test,strlen(test)+1);
        stabel_barisbaru0tambah (&myTabel,"X",2);
        stabel_barisbaru0simpan (&myTabel);
    }

    stabel_tampilnamakolom (&myTabel);
    stabel_tampilsemua (&myTabel);
    stabel_hapus (&myTabel);


    return kSDATAERR_NOERROR;
}

#endif
