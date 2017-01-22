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
 *  genrad228xfile.c
 *  BeeVee2
 *
 *  Created by Sihabul Milah on 3/23/06.
 *
 */

/*
17 Mei 2006
tambah "komp_tambah" dalam genrad_baca_brd() jika sebelumnya belum ada
*/


#include <stdio.h>
#include <string.h>
#include "genrad228xfile.h"
#include "../sdata_c/sdatateks.h"

#define GENRAD_AKSI_NONE			0
#define GENRAD_AKSI_BACA_ACCESS		1
#define GENRAD_AKSI_BACA_POWER		2
#define GENRAD_AKSI_BACA_CIRCUIT	3
#define GENRAD_AKSI_BACA_VALUE		4
#define GENRAD_AKSI_BACA_ADAPTOR	5


int genrad_baca_tac_power (const char *teks, _sdataptr snodptr, _sdataptr skonptr, _sdataptr stestpointptr)
{
	char nama[64];
	char ref[64];
	char pos[8];
	char prob[64];
	char nod[64];
	char nail[64];
	char info[256];
	int x,y;
	int n_id = -1;

	//printf ("%s\n", teks);
	sscanf (teks, "%s %s %s %d %d %s %s %s", nama, ref, pos, &x, &y, prob, nod,  nail);
	sprintf (info, "%s %s",nod, prob); /*2.1*/
	if ( node_cari(snodptr, nama, sdata_nomorini(snodptr)) >= 0 )
		n_id = node_id(snodptr);
	else
		n_id = node_tambah(snodptr, nama, "");
	if ( n_id >=0 )
	{
		/* koneksi_tambah (skonptr, n_id, ref); */
		tpt_tambah (stestpointptr, n_id, pos[1], x, y, info );
		return 1;
	}

	return 0;
}

int genrad_baca_tac_access (const char *teks, _sdataptr snodptr, _sdataptr skonptr, _sdataptr stestpointptr)
{
	char nama[64];
	char ref[64];
	char pos[8];
	char prob[64];
	char nod[64];
	char nail[64];
	char tnail[64];
	char info[256];
	int x,y;
	int n_id;

	sscanf (teks, "%s %s %s %d %d %s %s %s %s", nama, ref, pos, &x, &y, prob, nod, tnail, nail);
	/* tnail tidak perlu di ikutkan, akan problem waktu "find"
	sprintf (info, "%s %s %s %s ", nod, nail, tnail, prob);
	*/
	sprintf (info, "%s %s %s", nod, nail, prob);
	if ( (n_id = node_tambah(snodptr, nama, "") ) >=0 )
	{
		/* koneksi_tambah (skonptr, n_id, ref); */
		tpt_tambah (stestpointptr, n_id, pos[1], x, y, info);
		return 1;
	}
	return 0;
}

int genrad_baca_tac (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	int p;
	int aksi=GENRAD_AKSI_NONE;

#ifdef _BEEVEE_VERBOSE_
printf ("genrad_baca_tac %s ...\n", namafile);
#endif


	if ( (f=fopen(namafile,"r"))!=NULL )
	{
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
				ReplaceChar (teks, '!', 0);

				//printf ("2 %d %s\n", aksi,  teks);
				if (teks[0]=='%')
				{
					if (strcmp(teks, "%ACCESS")==0)
					{
						aksi = GENRAD_AKSI_BACA_ACCESS;
					}
					else if (strcmp(teks, "%PWRGND")==0)
					{
						aksi = GENRAD_AKSI_BACA_POWER;
					}
					else
						aksi=GENRAD_AKSI_NONE;
					teks[0] = 0;

				}

				if (aksi != GENRAD_AKSI_NONE && teks[0])
				{
					if (aksi==GENRAD_AKSI_BACA_ACCESS)
					{
						/* naca node access */
						genrad_baca_tac_access (teks, &bptr->snode, &bptr->skonek, &bptr->stestpoint);
					}
					else if (aksi==GENRAD_AKSI_BACA_POWER)
					{
						/* baca power/ground access */
						genrad_baca_tac_power (teks, &bptr->snode, &bptr->skonek, &bptr->stestpoint);
					}
				}
			}
			else
				aksi = GENRAD_AKSI_NONE;

			//printf ("2 %d %s\n", aksi, teks);
		}
		fclose(f);
	}
	else
	{
#ifdef _BEEVEE_VERBOSE_
		printf ("Ga bisa baca %s\n", namafile);
#endif
		return 0;
	}

	//node_tampilsemua (&bptr->snode);
	//koneksi_tampilsemua (&bptr->skonek);
	//tpt_tampilsemua (&bptr->stestpoint);
	//node_takadadinet(&bptr->snode, &bptr->stestpoint, NULL);

	bptr->punya_nodepin = 1;
	bptr->punya_nailpin = 1;

	if (sdata_jumlahdata(&bptr->stestpoint)==0 || sdata_jumlahdata(&bptr->snode)==0)
	{
		board_hapus(bptr);
		return 0;
	}


	return 1;
}

int genrad_baca_ckt_circuit (const char *teks, char *komp, _sdataptr stp_ptr, _sdataptr skonptr)
{
	char nama[64];
	char temp[64];
	char temp2[256];
	char temp3[64];
	char pin[8];
	char nod[64];
	int p=0;
	int n_id = -1;
	int i;

	strcpy (temp2,teks);

	if (!komp[0])
	{
		if (strstr(teks, " EXT,")!=NULL)
			return 0;
		sscanf (teks, "%s %s", nama, temp);
		strcpy (komp, nama);
		MidText (temp2, temp2, strlen(nama), 255);
		TrimLeft (temp2);
		MidText (temp2, temp2, strlen(temp), 255);

		if (temp2[0]==0)
			return 0;
	}

	TrimLeft (temp2);
	//printf ("%s *%s\n", komp, temp2);
	while (1)
	{
		i = GetTextUntilAChar (temp2, temp, 63, ' ', &p);
		if (temp[0])
		{
			RemoveChar (temp, ',');
			ReplaceChar (temp, '=', ' ');
			sscanf (temp, "%s %s", pin, nod);
			sprintf (temp3, "%s ", nod);
			//printf ("%s\n", temp3);
			n_id = -1;
			if (tpt_caridiinfo (stp_ptr, temp3, -1, &n_id)>=0)
			{
				sprintf (temp3, "%s.%s", komp, pin);
				koneksi_tambah(skonptr, n_id, temp3);
			}
		}
		if (i<0)
			break;
	}
	return 1;
}


int genrad_baca_ckt_komponen (const char *teks, _sdataptr skomptr)
{
	char komp[64];
	char info[256];

	sscanf (teks, "%s", komp);
	MidText ((char *)teks, info, strlen(komp)+3, 255);
	TrimLeft (info);
	//return (komp_nama_barui_info(skomptr, komp, info)>=0)?1:0;
	return (komp_tambah(skomptr, komp, info, 'T', 0, 0,0,0,0)>=0)?1:0;
}

int genrad_baca_ckt (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char komp[64];
	int p,i;
	int fC;
	int aksi=GENRAD_AKSI_NONE;

#ifdef _BEEVEE_VERBOSE_
printf ("genrad_baca_ckt %s ...\n", namafile);
#endif

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		komp[0] = 0;
		fC = 0; /* found comment=0 */
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
				ReplaceChar (teks, '!', 0);
				i = ReplaceChar (teks, ';', 0);

				//printf ("1 %d %s\n", aksi,  teks);
				/* perubahan aksi ditentukan oleh "%" */
				if (teks[0]=='%')
				{
					if (strcmp(teks, "%CIRCUIT")==0)
						aksi = GENRAD_AKSI_BACA_CIRCUIT;
					else if (strcmp(teks, "%VALUE")==0)
						aksi = GENRAD_AKSI_BACA_VALUE;
					else if (strncmp(teks, "%ADAPTOR",8)==0)
						aksi = GENRAD_AKSI_BACA_ADAPTOR;
					else
						aksi=GENRAD_AKSI_NONE;
					teks[0] = 0;

				}
				else if (aksi == GENRAD_AKSI_NONE)
				{
					if (strncmp(teks,"PROJECT NAME:",13)==0) /* -> board name */
					{
						MidText(teks, line, 13, SBOARD_NAMALEN); /* line = dummy */
						TrimLeft(line);
						ReplaceChar (line, ' ', '-');
						board_isinama(bptr, line);
					}
					else if (strncmp(teks,"ASSEMBLY NUMBER:", 16)==0) /* -> version */
					{
						MidText(teks, line, 16, SBOARD_VERSILEN); /* line = dummy */
						TrimLeft(line);
						board_tambahversi(bptr, line);
					}
					else if (strncmp(teks,"FAB REV:", 8)==0) /* -> version */
					{
						MidText(teks, line, 8, SBOARD_VERSILEN); /* line = dummy */
						TrimLeft(line);
						board_tambahversi(bptr, line);
					}
					teks[0] = 0;
				}
				else
				{
					if (!fC && teks[0] == '/' && teks[1] == '*')
					{
						fC = 1;
						teks[0] = 0;
					}
					else if (fC)
					{
						if (strstr(teks, "*/") != NULL)
						{
							fC = 0;
						}
						teks[0] = 0;
					}
				}

				if (aksi != GENRAD_AKSI_NONE && teks[0])
				{
					if (aksi==GENRAD_AKSI_BACA_CIRCUIT)
					{
						genrad_baca_ckt_circuit (teks, komp, &bptr->stestpoint, &bptr->skonek);
						if (i>0)
							komp[0] = 0;
					}
					else if (aksi==GENRAD_AKSI_BACA_VALUE)
					{
						genrad_baca_ckt_komponen (teks, &bptr->skomp);
					}
					/* diabaikan, baca lewat .tac */
					/*
					else if (aksi==GENRAD_AKSI_BACA_ADAPTOR)
					{
					}
					*/
				}
			}

			//printf ("2 %d %s\n", aksi, teks);
		}
		fclose(f);
	}
	else
	{
#ifdef _BEEVEE_VERBOSE_
		printf ("Ga bisa baca %s\n", namafile);
#endif
		return 0;
	}

	//komp_tampilsemua (&bptr->skomp);
	//node_tampilsemua (&bptr->snode);
	//koneksi_tampilsemua (&bptr->skonek);
	//tpt_tampilsemua (&bptr->stestpoint);
	//node_takadadinet(&bptr->snode, &bptr->stestpoint, NULL);

	if (sdata_jumlahdata(&bptr->skomp)==0 || sdata_jumlahdata(&bptr->skonek)==0)
	{
		board_hapus(bptr);
		return 0;
	}

	return 1;
}

/*
brd
berisi: komp x1 y1 x2 y2
*/
int genrad_baca_brd (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char komp[64];
	char tb[8];
	int x1,x2,y1,y2;
	int p;

#ifdef _BEEVEE_VERBOSE_
printf ("genrad_baca_brd %s ...\n", namafile);
#endif

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
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
				sscanf (teks, "%s %s %d %d %d %d", komp, tb, &x1, &y1, &x2, &y2);
				if (x1==x2)
				{
					x1 -= KXY_GROW;
					x2 += KXY_GROW;
				}
				if (y1==y2)
				{
					y1 -= KXY_GROW;
					y2 += KXY_GROW;
				}
				if (komp_nama_barui_xy (&bptr->skomp, komp, x1, y1, x2, y2)>=0)
					komp_nama_barui_tb (&bptr->skomp, komp, tb[0]);
				else
					komp_tambah (&bptr->skomp, komp, "", tb[0], 0, x1, y1, x2, y2);
			}
		}
		fclose(f);
	}
	else
	{
#ifdef _BEEVEE_VERBOSE_
		printf ("Ga bisa baca %s\n", namafile);
#endif
		return 0;
	}

	if (sdata_jumlahdata(&bptr->skomp)==0)
	{
	    board_hapus(bptr);
		return 0;
	}
	//komp_tampilsemua (&bptr->skomp, stdout);

	return 1;
}
