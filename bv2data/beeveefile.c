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
 *  beeveefile.c
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 3/27/06.
 *
 */

/* 4 agustus 2007
perubahan dari 2.0 ke 2.1
tambah beevee_bv2_koreksi_tp
*/

 /*
22 Sept 2006 - sihab
- koreksi pembacaan bv2 versi 2.0, hilangkan Temp nail!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "beeveefile.h"
#include "../sdata_c/sdatateks.h"

#define BEEVEE_AKSI_NONE   0
#define BEEVEE1_AKSI_BACA_KOMPONEN 1
#define BEEVEE1_AKSI_BACA_NODE  2
#define BEEVEE1_AKSI_BACA_GROUND  2

#define BEEVEE2_AKSI_HEADER   1
#define BEEVEE2_AKSI_KOMPONEN  2
#define BEEVEE2_AKSI_NODE   3
#define BEEVEE2_AKSI_KONEKSI  4
#define BEEVEE2_AKSI_TESTPIN  5
#define BEEVEE2_AKSI_TRACE   6



static int perlukoreksi_bv2_0_genrad228x;
static int perlukoreksi_bv2_versi;
static int perlukoreksi_bv2_tp;

int beevee_bv2_perlukoreksi ()
{
    return (perlukoreksi_bv2_0_genrad228x | perlukoreksi_bv2_versi | perlukoreksi_bv2_tp);
}

int beevee_bv2_koreksi_tp ( _sdataptr snode_ptr, _sdataptr stp_ptr)
{
	node nd;
	testpoint tp;
	int i,j,nid;
	int genrad = 0;
	int agilent = 0;
	_sdatateks steks;
	Str63 info;
	Str63 node,nail,probe;
	int k,l, m;
	int not_finish = 0;

#ifdef _BEEVEE_VERBOSE_
	printf ("beevee_bv2_koreksi_tp..\n");
#endif

	sdata_init (&steks);
	for (j=0;j<sdata_jumlahdata(snode_ptr);j++)
	{
		sdata_ambildatanomor (snode_ptr,j, &nd);
		if (nd.info[0])
		{
			if (strstr(nd.info,"P"))
                agilent = 1;
            else if (strstr(nd.info,"N") || strstr(nd.info,"F"))
                genrad = 1;
			if (agilent)
			{
				nid = sdata_nomorini (snode_ptr);

				k = sdatateks_dariteks (&steks, nd.info, ' ', 63, 0);
				m = -1; i = -1;
				for (l=0;l<k;l++)
				{
				    sdata_ambildatanomor (&steks, l, info);
				    i = tpt_cari (stp_ptr, nid, i+1);
                    if (i>=0 && i!=m)
                    {
                        strcat (info," ");
                        tpt_barui_info (stp_ptr, info);
                    }
                    else
                    {
                        not_finish = 1;
                        break;
                    }
                    if (m=-1)
                        m = i;
				}
                node_barui_info (snode_ptr, "");
			}

			else if (genrad)
			{
				nid = sdata_nomorini (snode_ptr);

				k = sdatateks_dariteks (&steks, nd.info, ' ', 63, 0);
				m = -1; i = -1;
				for (l=0;l<k;l++)
				{
				    sdata_ambildatanomor (&steks, l, node);
				    if (node[0] == 'N')
				    {
                        strcpy (info, node); strcat(info, " ");
                        ++l;
				    }
				    else
                        continue;
				    if (sdata_ambildatanomor (&steks, 1, nail)>=0)
                    {
                        if (nail[0]=='F')
                        {
                            strcat (info, nail);
                            strcat (info, " ");
                            ++l;
                        }
                    }
				    if (sdata_ambildatanomor (&steks, 1, probe))
				    {
                        if (probe[0]!='N' && probe[0]!='F')
                        {
                            strcat (info, probe);
                            ++l;
                        }
				    }

				    i = tpt_cari (stp_ptr, nid, i+1);
                    if (i>=0 && i!=m)
                    {
                        tpt_barui_info (stp_ptr, info);
                    }
                    else
                    {
                        not_finish = 1;
                        break;
                    }
                    if (m=-1)
                        m = i;
				}
                node_barui_info (snode_ptr, "");
			}
		}
		sdata_hapussemua (&steks);
	}
	//sdata_hapussemua (&steks);
	return not_finish;
}


int beevee_baca_bev_komponen ( char *teks, char *nama, _sdataptr skomptr)
{
    int x1,x2,y1,y2;
    char tb[8];

    sscanf (teks, "%s %d %d %d %d %s", nama, &x1, &y1, &x2, &y2, &tb);
    y1*=-1;
    y2*=-1;
	if (nama[0] == '%' || !tb[0])
	{
		nama[0] = 0;
		return 0;
	}
	else if (strstr (nama, "."))
	{
		ReplaceChar (nama, '.', 0);
		if (komp_cari (skomptr, nama, -1)>=0)
		{
			nama[0] = 0;
			return 0;
		}
    }

	if (komp_tambah(skomptr, nama, "", tb[0], 0, x1, y1, x2, y2)>=0) /* ok */
        return 1;
    /* gagal */
    nama[0] = 0;
    return 0;
}

int beevee_baca_bev_node ( char *teks, int *jml_tp, int *jml_komp, _sdataptr snodptr)
{
    char nama[64];
    sscanf (teks, "%s %d %d", nama, jml_tp, jml_komp);
    return  node_tambah(snodptr, nama, "");
}

int beevee_baca_bev_node_tp ( char *teks, int n_id, _sdataptr skomptr, _sdataptr stestpointptr)
{
    int mid;
    int x1,x2,y1,y2;
    char tb;

    mid = atoi (teks);
    if (sdata_kenomor(skomptr, mid)>=0)
    {
        komp_xy (skomptr, &x1, &y1, &x2, &y2);
        komp_tb (skomptr, &tb);
        return tpt_tambah (stestpointptr, n_id, tb, x1+(x2-x1)/2, y1+(y2-y1)/2, "");
    }
    return -1;
}

int beevee_baca_bev (const char *namafile, boardptr bptr)
{
    FILE *f;
    char teks[256];
    char line[256];
    char nama[64];
    int p,i;
    int t,k, jtp, jkomp;
    int node_id;
    int aksi = BEEVEE_AKSI_NONE;

#ifdef _BEEVEE_VERBOSE_
    printf ("beevee_baca_bev %s ...\n", namafile);
#endif

    if ( (f=fopen(namafile,"r"))!=NULL )
    {
        nama[0] = 0;
        t = k = 0;
        jtp = jkomp = 0;
        node_id = -1;
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
                if (aksi == BEEVEE_AKSI_NONE && teks[0]=='*')
                {
                    if (strncmp (teks, "*NAME", 5)==0)
                    {
                        MidText (teks, line, 6, 63); /* line = dummy */
                        board_isinama (bptr, line);
                    }
                    else if (strncmp (teks, "*DEVICES", 8)==0)
                    {
                        aksi = BEEVEE1_AKSI_BACA_KOMPONEN;
                    }
                    else if (strncmp (teks, "*NODES", 6)==0)
                    {
                        aksi = BEEVEE1_AKSI_BACA_NODE;
                    }
                    else if (strncmp (teks, "*GND", 6)==0)
                    {
                        aksi = BEEVEE1_AKSI_BACA_GROUND;
                    }

                    teks[0] = 0;
                }

                if (aksi != BEEVEE_AKSI_NONE && teks[0])
                {
                    i = ReplaceChar (teks, ';', 0);
                    if (aksi==BEEVEE1_AKSI_BACA_KOMPONEN)
                    {
                        if (!nama[0])
                            beevee_baca_bev_komponen(teks, nama, &bptr->skomp);
                        else
                            //komp_nama_barui_info (&bptr->skomp, nama, teks);
                            komp_barui_info (&bptr->skomp, teks);
                    }
                    else if (aksi==BEEVEE1_AKSI_BACA_NODE || aksi==BEEVEE1_AKSI_BACA_GROUND)
                    {
                        if (node_id<0)
                        {
                            jtp = jkomp = 0;
                            node_id = beevee_baca_bev_node(teks, &jtp, &jkomp, &bptr->snode);
                            t = k = 0;
                            //printf ("%s *%d %d %d\n", teks, node_id, jtp, jkomp);
                        }
                        else if (node_id>=0)
                        {
                            if (t<jtp)
                            {
                                ++t;
                                beevee_baca_bev_node_tp (teks, node_id, &bptr->skomp, &bptr->stestpoint);
                            }
                            else if (k<jkomp)
                            {
                                k++;
                                /* abaikan */
                            }
                            else
                            {
                                if (strncmp(teks,"N=",2)==0)
                                {
                                    strtok(teks,".");
                                    strcat(teks," ");
                                    sdata_kenomor(&bptr->snode, node_id);
                                    node_tambahi_info(&bptr->snode, &teks[2]);
                                }
                                else if (strncmp(teks,"S=",2)==0)
                                {
                                    node nd;
                                    char node_n[64];

                                    strtok(teks,".");
                                    sdata_kenomor(&bptr->snode, node_id);
                                    sdata_ambildatanomor (&bptr->snode, node_id, &nd);
                                    strcpy (node_n, nd.nama);
                                    strcat (node_n, " ");
                                    strcat (nd.info, node_n);
                                    strcpy (nd.nama, &teks[2]);
                                    sdata_baruiini (&bptr->snode, &nd, sizeof(node));
                                }
                                else
                                    koneksi_tambah (&bptr->skonek, node_id, teks);
                            }
                            //printf ("*%d %d %d %s\n", node_id, t, k, teks);

                        }
                    }
                    if (i>0)
                    {
                        nama[0]=0;
                        node_id = -1;
                    }
                }
            }
            else
                aksi = BEEVEE_AKSI_NONE;
        }
        fclose (f);
    }
    else
    {
#ifdef _BEEVEE_VERBOSE_
        printf ("Ga bisa baca %s\n", namafile);
#endif

        return 0;
    }

    //komp_tampilsemua (&bptr->skomp);
    if (sdata_jumlahdata(&bptr->skomp)==0 &&
            //sdata_jumlahdata(&bptr->snode)==0 &&
            //sdata_jumlahdata(&bptr->skonek)==0 &&
            sdata_jumlahdata(&bptr->stestpoint)==0)
    {
        board_hapus(bptr);
        return 0;
    }

	beevee_bv2_koreksi_tp (&bptr->snode, &bptr->stestpoint);
	tpt_cekpin (&bptr->stestpoint, &bptr->punya_nodepin, &bptr->punya_nailpin, &bptr->punya_probe);
	board_hitung_ataskiri (bptr);

    if (!(bptr->punya_nodepin==0 && bptr->punya_nailpin==0 && bptr->punya_probe==0))
        return 2;

    return 1;
}

/* #NAME!VERSION!ORIGINAL FORMAT */
int beevee_baca_bv2_header (char *teks, boardptr bptr)
{
    _sdatateks stx;
    Str63 temp;
    Str63 vers;
    int result = 0;
    //size_t l;

    sdata_init(&stx);
    if (sdatateks_dariteks(&stx, teks, '!', Str63Len, 1)==3)
    {
        sdata_ambildatanomor(&stx, 0, temp);
        board_isinama(bptr, temp);
        sdata_ambildatanomor(&stx, 1, vers);
        board_isiversi(bptr, vers);
        sdata_ambildatanomor(&stx, 2, temp);
        board_isiasal(bptr, temp);
        result = 1;


        /* tidak perlu koreksi versi
        l = strlen(SBOARD_VERSIONSTR);
        if (strncmp (vers,SBOARD_VERSIONSTR,l)==0)
        {
            if (strlen(vers)>l)
            {
                if (vers[l]!=' ')
                    perlukoreksi_bv2_versi = 1;
            }
        }
        else
            perlukoreksi_bv2_versi = 1;
        */

        if (strncmp (vers,"2.0 ",4)==0 && strstr(temp, "genrad228x") && strstr(temp, "beevee1")==NULL)
        {
            perlukoreksi_bv2_0_genrad228x = 1;
            perlukoreksi_bv2_tp = 1;
            strcpy (temp, SBOARD_VERSIONSTR);
            strcat (temp, &vers[3]);
            board_isiversi (bptr, temp);
        }
        else if (strncmp (vers,"2.0.",4)==0)
        {
            perlukoreksi_bv2_tp = 1;
            strcpy (temp, SBOARD_VERSIONSTR);
            strcat (temp, &vers[5]);
            board_isiversi (bptr, temp);
        }
        else if (strncmp (vers,"2.0 ",4)==0 || strncmp (vers,"2.0",3)==0 && strlen(vers)==3)
        {
            perlukoreksi_bv2_tp = 1;
            strcpy (temp, SBOARD_VERSIONSTR);
            strcat (temp, &vers[3]);
            board_isiversi (bptr, temp);
        }
    }
    sdata_hapussemua(&stx);
    return result;
}

/* #REFDES!TOP-BOTTOM!ORIENT!X1!Y1!X2!Y2!INFO */
int beevee_baca_bv2_komponen (char *teks, _sdataptr skomptr)
{
    _sdatateks stx;
    komponen komp;
    Str63 temp;
    int temp_i;
    int result = 0;

    sdata_init(&stx);
    if (sdatateks_dariteks(&stx, teks, '!', Str255Len, 1)==8)
    {
        memset(&komp, 0, sizeof(komponen));
        sdata_ambildatanomor(&stx, 0, komp.nama);
        sdata_ambildatanomor(&stx, 1, temp);
        komp.pos_tb = temp[0];
        sdatateks_nomorini_int(&stx, 2, &temp_i);
        komp.orentasi = (short)temp_i;
        sdatateks_nomorini_int(&stx, 3, &komp.pos_x1);
        sdatateks_nomorini_int(&stx, 4, &komp.pos_y1);
        sdatateks_nomorini_int(&stx, 5, &komp.pos_x2);
        sdatateks_nomorini_int(&stx, 6, &komp.pos_y2);
        sdata_ambildatanomor(&stx, 7, komp.info);

        if (komp_tambah(skomptr, komp.nama, komp.info, komp.pos_tb, komp.orentasi,
                        komp.pos_x1, komp.pos_y1, komp.pos_x2, komp.pos_y2)>=0)
            //if (sdata_tambah(skomptr, &komp, sizeof(komponen))>=0)
            result = 1;
    }
    sdata_hapussemua(&stx);

    return result;
}

/* #ID!NAME!INFO */
int beevee_baca_bv2_signal (char *teks, _sdataptr snodptr)
{
    _sdatateks stx;
    node nd;
    int result = 0;

    sdata_init(&stx);
    if (sdatateks_dariteks(&stx, teks, '!', Str255Len, 1)==3)
    {
        memset(&nd, 0, sizeof(node));
        sdatateks_nomorini_int(&stx, 0, &nd.id);
        sdata_ambildatanomor(&stx, 1, nd.nama);
        sdata_ambildatanomor(&stx, 2, nd.info);

        /* koreksi kesalahan: buang Temp nail !*/
        if (perlukoreksi_bv2_0_genrad228x)
        {
            sdata_hapussemua(&stx);
            if (sdatateks_dariteks(&stx, nd.info, ' ', Str63Len, 1)>=4)
            {
                Str63 n1;
                Str63 n2;
                sdata_ambildatanomor(&stx, 1, n1);
                sdata_ambildatanomor(&stx, 2, n2);
                if (n1[0]=='F' && n2[0]=='F')
                {
                    sdata_kenomor (&stx, 2);
                    sdata_hapusini (&stx);
                    sdatateks_jaditeks (&stx, " ", nd.info);

                }
            }
        }

        /*if (node_tambah(snodptr, nd.nama, nd.info)==nd.id)*/
        if (sdata_tambah(snodptr, &nd, sizeof(node))>=0)
            result = 1;
    }
    sdata_hapussemua(&stx);

    return result;
}

/* #SID!REFDES.PIN */
int beevee_baca_bv2_koneksi (char *teks, _sdataptr skonptr)
{
    _sdatateks stx;
    koneksi konek;
    int result = 0;

    sdata_init(&stx);
    if (sdatateks_dariteks(&stx, teks, '!', Str63Len, 1)==2)
    {
        memset(&konek, 0, sizeof(koneksi));
        sdatateks_nomorini_int(&stx, 0, &konek.node_id);
        sdata_ambildatanomor(&stx, 1, konek.komp_pin);

        /*if (koneksi_tambah(skonptr, konek.node_id, konek.komp_pin)>=0)*/
        if (sdata_tambah(skonptr, &konek, sizeof(koneksi))>=0)
            result = 1;
    }
    sdata_hapussemua(&stx);

    return result;
}

/* #SID!TOP-BOTTOM!X!Y */
int beevee_baca_bv2_testpin (char *teks, _sdataptr stestpointptr)
{
    _sdatateks stx;
    Str63 temp;
    testpoint nt;
    int result = 0;

    sdata_init(&stx);
    if (sdatateks_dariteks(&stx, teks, '!', Str63Len, 1)>=4) /* 2.0 ==4, 2.1===5 */
    {
        memset(&nt, 0, sizeof(testpoint));
        sdatateks_nomorini_int(&stx, 0, &nt.node_id);
        sdata_ambildatanomor(&stx, 1, temp);
        nt.pos_tb = temp[0];
        sdatateks_nomorini_int(&stx, 2, &nt.pos_x);
        sdatateks_nomorini_int(&stx, 3, &nt.pos_y);
        sdata_ambildatanomor(&stx, 4, nt.info); /* 2.1 */


        /*if (tpt_tambah(skonptr, nt.node_id, nt.pos_tb, nt.pos_x, nt.pos_y)>=0)*/
        if (sdata_tambah(stestpointptr, &nt, sizeof(testpoint))>=0)
            result = 1;
    }
    sdata_hapussemua(&stx);

    return result;
}

/* #SID!TYPE!D0!D1!D2!D3!D4!D5!D6!D7 */
int beevee_baca_bv2_trace (char *teks, _sdataptr straptr)
{
    _sdatateks stx;
    trace tr;
    int temp_i;
    int result = 0;

    sdata_init(&stx);
    if (sdatateks_dariteks(&stx, teks, '!', Str255Len, 1)==10)
    {
        memset(&tr, 0, sizeof(trace));
        sdatateks_nomorini_int(&stx, 0, &tr.node_id);
        sdatateks_nomorini_int(&stx, 1, &temp_i);
        tr.tipe = (SBOARD_ROUTETYPE) temp_i;
        sdatateks_nomorini_int(&stx, 2, &tr.data[0]);
        sdatateks_nomorini_int(&stx, 3, &tr.data[1]);
        sdatateks_nomorini_int(&stx, 4, &tr.data[2]);
        sdatateks_nomorini_int(&stx, 5, &tr.data[3]);
        sdatateks_nomorini_int(&stx, 6, &tr.data[4]);
        sdatateks_nomorini_int(&stx, 7, &tr.data[5]);
        sdatateks_nomorini_int(&stx, 8, &tr.data[6]);
        sdatateks_nomorini_int(&stx, 9, &tr.data[7]);

        if (sdata_tambah(straptr, &tr, sizeof(trace))>=0)
            result = 1;
    }
    sdata_hapussemua(&stx);

    return result;
}


int beevee_baca_bv2 (const char *namafile, boardptr bptr)
{
    FILE *f;
    Str255 line;
    Str255 teks;
    int aksi = BEEVEE_AKSI_NONE;
    int p;

    perlukoreksi_bv2_0_genrad228x = 0;
    perlukoreksi_bv2_versi = 0;
	perlukoreksi_bv2_tp = 0;

    if ( (f=fopen(namafile,"r"))!=NULL )
    {
        while (1)
        {
            fgets (line, Str255Len, f);
            if (feof(f))
                break;

            p = 0;
            ReplaceChar (line, '#',0);
            ReplaceChar (line, '\t',' ');
            ReplaceChar (line, 0xa, 0xd);
            TrimLeft(line);
            TrimRight(line);
            //UpperText(line);

            //SwitchToThread();

            if (GetLine(line,teks,&p))
            {
                if (teks[0]=='*')
                {
                    if (strncmp(teks, "*HEADER*",8)==0)
                    {
                        aksi = BEEVEE2_AKSI_HEADER;
                    }
                    else if (strncmp(teks, "*COMPONENTS*",12)==0)
                    {
                        aksi = BEEVEE2_AKSI_KOMPONEN;
                    }
                    else if (strncmp(teks, "*SIGNALS*",9)==0)
                    {
                        aksi = BEEVEE2_AKSI_NODE;
                    }
                    else if (strncmp(teks, "*CONNECTIONS*",13)==0)
                    {
                        aksi = BEEVEE2_AKSI_KONEKSI;
                    }
                    else if (strncmp(teks, "*TESTPINS*",10)==0)
                    {
                        aksi = BEEVEE2_AKSI_TESTPIN;
                    }
                    else if (strncmp(teks, "*TRACES*",8)==0)
                    {
                        aksi = BEEVEE2_AKSI_TRACE;
                    }
                    else
                        aksi = BEEVEE_AKSI_NONE;
                    teks[0] =0;
                }

                if (teks[0] && aksi != BEEVEE_AKSI_NONE)
                {
                    if (aksi==BEEVEE2_AKSI_HEADER)
                    {
                        beevee_baca_bv2_header(teks, bptr);
                    }
                    else if (aksi==BEEVEE2_AKSI_KOMPONEN)
                    {
                        beevee_baca_bv2_komponen(teks, &bptr->skomp);
                    }
                    else if (aksi==BEEVEE2_AKSI_NODE)
                    {
                        beevee_baca_bv2_signal(teks, &bptr->snode);
                    }
                    else if (aksi==BEEVEE2_AKSI_KONEKSI)
                    {
                        beevee_baca_bv2_koneksi(teks, &bptr->skonek);
                    }
                    else if (aksi==BEEVEE2_AKSI_TESTPIN)
                    {
                        beevee_baca_bv2_testpin(teks, &bptr->stestpoint);
                    }
                    else if (aksi==BEEVEE2_AKSI_TRACE)
                    {
                        beevee_baca_bv2_trace(teks, &bptr->strace);
                    }
                }
            }
        }
        fclose(f);
    }

    if (sdata_jumlahdata(&bptr->skomp)==0 &&
            //sdata_jumlahdata(&bptr->snode)==0 &&
            //sdata_jumlahdata(&bptr->skonek)==0 &&
            sdata_jumlahdata(&bptr->stestpoint)==0 &&
            sdata_jumlahdata(&bptr->strace)==0 )
    {
        board_hapus(bptr);
        return 0;
    }

	if (perlukoreksi_bv2_tp)
		perlukoreksi_bv2_tp = beevee_bv2_koreksi_tp (&bptr->snode, &bptr->stestpoint);
    tpt_cekpin (&bptr->stestpoint, &bptr->punya_nodepin, &bptr->punya_nailpin, &bptr->punya_probe);
    board_hitung_ataskiri (bptr);
    board_cek_punyatempat (bptr);

    if (bptr->punya_nodepin==0 && bptr->punya_nailpin==0 && bptr->punya_probe==0)
        perlukoreksi_bv2_tp =0;
    else if (perlukoreksi_bv2_tp)
        return 2;

    return 1;
}

int beevee_tulis_bv2 (const char *namafile, boardptr bptr)
{
    FILE *f;
    char nama[64];
    char versi[64];
    char asal[64];

    if ( (f=fopen(namafile,"w"))!=NULL )
    {
        board_nama(bptr, nama);
        board_versi(bptr, versi);
        board_asal(bptr, asal);

        /* HEADER */
        fprintf (f, "# BeeVee - BoardViewer (c) Sihabul Milah 2004,2005,2006\n\n");
        fprintf (f, "*HEADER*\n#NAME!VERSION!ORIGINAL FORMAT\n%s!%s!%s\n*END*\n\n", nama, versi, asal);
        /* COMPONENTS */
        fprintf (f, "*COMPONENTS*\n");
        komp_tampilsemua (&bptr->skomp, f);
        fprintf (f, "*END*\n\n");
        /* SIGNALS */
        fprintf (f, "*SIGNALS*\n");
        node_tampilsemua (&bptr->snode, f);
        fprintf (f, "*END*\n\n");
        /* CONNECTIONS */
        fprintf (f, "*CONNECTIONS*\n");
        koneksi_tampilsemua (&bptr->skonek, f);
        fprintf (f, "*END*\n\n");
        /* TESTPINS */
        fprintf (f, "*TESTPINS*\n");
        tpt_tampilsemua (&bptr->stestpoint, f);
        fprintf (f, "*END*\n\n");
        /* TRACES */
        fprintf (f, "*TRACES*\n");
        trace_tampilsemua (&bptr->strace, f);
        fprintf (f, "*END*\n\n");

        fclose(f);
        return 1;
    }

    return 0;
}
