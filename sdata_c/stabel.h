/*
sTabel.h

Koleksi data dlm tabel pointer
Oleh Sihabul Milah

*/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _SHB_STABEL_H__
#define _SHB_STABEL_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include "sdata.h"

#define STABEL_VERSION 0x0100

typedef struct _stabel {
       _sdata sdata;
       _sdata brs; /* data baris sementara */
       _sdata namakolom;
       unsigned char *tipekolom;
       unsigned int jumlah_kolom;
} _stabel;
typedef _stabel* _stabelptr;

enum _tipedata {
    _tipeBebas, /* 1 */
    _tipeInt16, /* 4 */
    _tipeUInt16,
    _tipeInt32, /* 8 */
    _tipeUInt32,
    _tipeDbl, /* 8 */
    _tipeChr, /* 1 */
    _tipeStr
};
typedef unsigned char  _tipedata;

/* initial, harus dipanggil pertama */
void stabel_init (_stabelptr stblptr, int kolom);
int stabel_tambahnamakolom (_stabelptr stblptr, char *nama);
int stabel_tambahtipekolom (_stabelptr stblptr, _tipedata tipe);

/* navigasi */
int stabel_jumlahkolom (_stabelptr stblptr);
int stabel_jumlahbaris (_stabelptr stblptr);
int stabel_jumlahsel (_stabelptr stblptr);
int stabel_adanamakolom (_stabelptr stblptr);
short stabel_namakolomsesuaikah (_stabelptr stblptr);
int stabel_nomorselini (_stabelptr stblptr);
int stabel_barisini (_stabelptr stblptr);
int stable_kolomini (_stabelptr stblptr);
short stabel_dibaris0kah (_stabelptr stblptr);
short stabel_dibaris0datakah (_stabelptr stblptr);
short stabel_dibarisakhirkah (_stabelptr stblptr);
short stabel_diselkepalakah (_stabelptr stblptr);
short stabel_diselekorkah (_stabelptr stblptr);
int stabel_keselkepala (_stabelptr stblptr);
int stabel_keselekor (_stabelptr stblptr);
int stabel_kedepan1baris (_stabelptr stblptr);
int stabel_kebelakang1baris (_stabelptr stblptr);
int stabel_keselnomor (_stabelptr stblptr, int nomor);
int stabel_kebaris (_stabelptr stblptr, int baris);
int stabel_kekolom (_stabelptr stblptr, int kolom);
_tipedata stabel_kolomtipeapa (_stabelptr stblptr, int kolom);

/* manipulasi */
int stabel_seltambah (_stabelptr stblptr, void *data, int besar);
int stabel_selsisip (_stabelptr stblptr, void *data, int besar);
int stabel_baristambah (_stabelptr stblptr, _sdataptr brsptr);
int stabel_barissisip (_stabelptr stblptr, _sdataptr brsptr);
int stabel_baruiselini (_stabelptr stblptr, void *data, int besar);
int stabel_baruibarisini (_stabelptr stblptr, _sdataptr brsptr);
void stabel_barisbaru0 (_stabelptr stblptr);
int stabel_barisbaru0tambah (_stabelptr stblptr, void *data, int besar);
int stabel_barisbaru0tambahkata (_stabelptr stblptr, char *kata);
//int stabel_barisbaru0tambahangka (_stabelptr stblptr, int angka);
int stabel_barisbaru0tambahangka (_stabelptr stblptr, void *angka, int besar);
int stabel_barisbaru0simpan (_stabelptr stblptr);
int stabel_ambildataselini (_stabelptr stblptr, void *data);
int stabel_ambildatasel (_stabelptr stblptr, int baris, int kolom, void *data);
int stabel_ambildatabaris (_stabelptr stblptr, int baris, _sdataptr sbrsptr);
int stabel_hapusselini (_stabelptr stblptr);
void stabel_hapussemuasel (_stabelptr stblptr);
int stable_hapus1barisini (_stabelptr stblptr);
int stable_hapussejumlahbarisini (_stabelptr stblptr, int jumlah);

#ifdef  __cplusplus
}
#endif

#endif
