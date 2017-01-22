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
	Sihabul Milah

	Manipulasi file ag3070

	+ membaca file "board" & "board_xy"
	+ board_xy dalam satuan 1/10 MILS --> x = X/10
	+ ukuran (access) node = 50x50
*/

#include "ag3070file.h"

#define AG3070_KOMPONENSTR	"#CAPACITOR#CONNECTOR#JUMPER#NODE LIBRARY#PIN LIBRARY#RESISTOR#"
#define AG3070_BOARDXY_TAGS	"#UNITS#SCALE#PLACEMENT#OUTLINE#TOOLING#OTHER#ALTERNATES#DEVICES#END#"

#define AG3070_BOARD_AKSI_NONE	0
#define AG3070_BOARD_AKSI_BACAKOMPONEN	1
#define AG3070_BOARD_AKSI_BACANODE	2
#define AG3070_BOARD_AKSI_BACAALTER	3
#define AG3070_BOARD_AKSI_BACAPROBE	4
#define AG3070_BOARD_AKSI_BACATOOLING 5
#define AG3070_BOARD_AKSI_BACAHEAD 6


#include <stdio.h>
#include <string.h>

#include "../steks_c/stext.h"
#include "../sdata_c/sdatateks.h"

short gag3070_fixture = 0;
int gag3070_dx = 0;
int gag3070_dy = 0;

void ag3070_pakefixture (short pake)
{
	gag3070_fixture = pake;
}

short ag3070_pakefixturekah ()
{
	return gag3070_fixture;
}

int ag3070_baca_board_komponen (char *teks, _sdataptr skomp_ptr)
{
	Str63 nama;
	Str255 info;
	int i;

	if (ReplaceChar(teks, ';', 0) != 0 && strstr(teks, "VERSION") == NULL )
	{
		i = 0;
		GetTextUntilAChar (teks, nama, Str63Len, ' ', &i);
		MidText(teks, info, i, Str255Len);
		TrimLeft(info);

		if (komp_tambah (skomp_ptr, nama, info, 'T', 0, 0,0,0,0) <0 )
			return 0;
	}
	/*
	else if (strstr (teks, "CONNECTIONS") != NULL || strstr (teks, "PIN_MAP") != NULL)
		return 0;
	*/
	return 1;
}

int ag3070_baca_board_node (char *teks, _sdataptr snode_ptr, _sdataptr skone_ptr)
{
	static int id = -1;
	int fend;

	RemoveChar (teks, '\"');

	if (id<0)
	{
		/*
		if (strcmp(teks, "END")==0 || strcmp(teks,"DEVICE")==0)
			return 0;
		*/
		id = node_tambah(snode_ptr, teks, "");
		if (id<0)
			return 0;
	}
	else
	{
		fend = RemoveChar (teks, ';');
		if (koneksi_tambah(skone_ptr, id, teks)<0)
		{
			id = -1;
			return 0;
		}
		if (fend>0)
			id = -1;
		return 2; /* koneksi */
	}

	return 1; /* node */
}


int ag3070_baca_board (const char *namafile, boardptr mbptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char temp[256];
	int p;
	int aksi=AG3070_BOARD_AKSI_NONE;

#ifdef _BEEVEE_VERBOSE_
printf ("ag3070_baca_board %s\n", namafile);
#endif

	teks[0] = 0;
	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		while (1)
		{
			fgets (line, 255, f);
			if (feof(f))
				break;
			else
			{
				p = 0;
				ReplaceChar (line, '\t',' ');
				ReplaceChar (line, 0xa, 0xd);
				TrimLeft(line);
				TrimRight(line);
				UpperText(line);

				if (GetLine(line,teks,&p))
				{
					ReplaceChar (line, '!', 0);
					if (aksi==AG3070_BOARD_AKSI_NONE)
					{
						sprintf (temp, "#%s#",teks);
						if (strstr(AG3070_KOMPONENSTR, temp)!=NULL)
							aksi = AG3070_BOARD_AKSI_BACAKOMPONEN;
						else if (strcmp("HEADING", teks)==0)
							aksi = AG3070_BOARD_AKSI_BACAHEAD;
						else if (strcmp("CONNECTIONS", teks)==0)
							aksi = AG3070_BOARD_AKSI_BACANODE;
						teks[0]=0;
					}


					if (aksi != AG3070_BOARD_AKSI_NONE && teks[0])
					{
						if (aksi == AG3070_BOARD_AKSI_BACAKOMPONEN)
						{
							if (ag3070_baca_board_komponen (teks, &mbptr->skomp) <= 0)
								aksi = AG3070_BOARD_AKSI_NONE;

						}
						else if (aksi == AG3070_BOARD_AKSI_BACAHEAD)
						{
							RemoveChar (teks, '\"');
							ReplaceChar (teks, ';', 0);
							ReplaceChar (teks, ' ', '-');
							board_isinama (mbptr, teks);
							aksi = AG3070_BOARD_AKSI_NONE;
						}
						else if (aksi == AG3070_BOARD_AKSI_BACANODE)
						{
							if (ag3070_baca_board_node (teks, &mbptr->snode, &mbptr->skonek) <= 0)
								aksi = AG3070_BOARD_AKSI_NONE;
						}
					}
				}
				else
					aksi = AG3070_BOARD_AKSI_NONE;
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

	//komp_tampilsemua (&mbptr->skomp);
	//node_tampilsemua (&mbptr->snode);
	//koneksi_tampilsemua (&mbptr->skonek);

	if (sdata_jumlahdata(&mbptr->skomp)==0 || sdata_jumlahdata(&mbptr->snode)==0)
		return 0;
	return 1;
}

int ag3070_baca_boardxy_alternate (char *teks, _sdataptr skomp_ptr, _sdataptr skonek_ptr, _sdataptr stp_ptr)
{
	int x,y;
	char kon[256];
	char info[256];
	char komp[64];
	char *temp;
	int i;
	char tb = 0;

	/* RemoveChar(teks, ','); */
	sscanf (teks, "%d, %d %s", &x, &y, kon);

	x = x/10;
	y = y/10;
	temp = strstr (teks, kon);
	temp += (strlen(kon));
	strcpy (info, temp);
	TrimLeft(info);

	strcpy(komp, kon);
	if (ReplaceChar (komp, '.', 0)<=0) /* eshabe / 2010-05-18: hanya membaca alternate yg berhubungan dg pin/kaki komponen */
        return 0;

	//komp_nama_barui_xy (skomp_ptr, komp, x, y, x, y);
	komp_nama_tambahi_xy (skomp_ptr, komp, x, y, x, y);
	if (strstr(info, "TOP")!=NULL)
	{
		komp_nama_barui_tb (skomp_ptr, komp,'T');
		tb = 'T';
	}

	if (!gag3070_fixture && strstr(info, "NO_ACCESS")==NULL)
	{
		i = -1;
		if (koneksi_cari_dkonek(skonek_ptr, kon, &i, sdata_nomorini(skonek_ptr)) >= 0)
		{
			if (!tb)
				tb = 'B';

			tpt_tambah(stp_ptr, i, tb, x, y, "");
		}
	}

	return 1;
}

int ag3070_baca_boardxy_komponen (char *teks, _sdataptr skomp_ptr)
{
	char komp[64];
	char lok[64];

	sscanf (teks, "%s %s", komp, lok);

	if (strcmp(lok,"BOTTOM")==0)
	{
		if ( komp_nama_barui_tb (skomp_ptr, komp, 'B') < 0 )
			return 0;
	}
	return 1;
}

int ag3070_baca_boardxy (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	char temp[256];
	char long_teks[1024];
	int p,i;
	int aksi=AG3070_BOARD_AKSI_NONE;

#ifdef _BEEVEE_VERBOSE_
printf ("ag3070_baca_boardxy %s ...\n", namafile);
#endif

	gag3070_dx = 0;
	gag3070_dy = 0;

	if ( (f=fopen(namafile,"r"))!=NULL )
	{
		strcpy (long_teks, "");
		while (1)
		{
			fgets (line, 255, f);
			if (feof(f))
				break;
			else
			{
				p = 0;
				ReplaceChar (line, '\t',' ');
				ReplaceChar (line, 0xa, 0xd);
				ReplaceChar (line, '!', 0);
				TrimLeft(line);
				TrimRight(line);
				UpperText(line);

				if (GetLine(line,teks,&p))
				{
					i = ReplaceChar (teks, ';', 0);
#ifdef _BEEVEE_VERBOSE_
                    puts(teks);
#endif

					strcat (long_teks, teks);
					if (i<=0)
					{
						strcat (long_teks," ");

						sprintf (temp, "#%s#", teks);
						if (aksi != AG3070_BOARD_AKSI_NONE)
						{
							if (strstr(AG3070_BOARDXY_TAGS,temp) != NULL)
								aksi = AG3070_BOARD_AKSI_NONE;
						}

						if (aksi == AG3070_BOARD_AKSI_NONE)
						{
							if (strstr ("#ALTERNATES#", temp) != NULL)
							{
								aksi = AG3070_BOARD_AKSI_BACAALTER;
								strcpy(long_teks,"");
							}
							else if (strstr ("#DEVICES#", temp)!=NULL)
							{
								aksi = AG3070_BOARD_AKSI_BACAKOMPONEN;
								strcpy(long_teks,"");
							}
							else if (gag3070_fixture && strcmp ("#TOOLING#", temp)==0)
							{
								aksi = AG3070_BOARD_AKSI_BACATOOLING;
								strcpy(long_teks,"");
							}
						}
					}

					if (i>0 && long_teks[0])
					{
						if (aksi == AG3070_BOARD_AKSI_BACAALTER)
						{
							if (ag3070_baca_boardxy_alternate(long_teks, &bptr->skomp, &bptr->skonek, &bptr->stestpoint)!=1)
								aksi=AG3070_BOARD_AKSI_NONE;
						}
						else if (aksi == AG3070_BOARD_AKSI_BACAKOMPONEN)
						{
							if (ag3070_baca_boardxy_komponen(long_teks, &bptr->skomp)!=1)
								aksi=AG3070_BOARD_AKSI_NONE;
						}
						else if (aksi == AG3070_BOARD_AKSI_BACATOOLING)
						{
							sscanf (long_teks, "%s %d, %d", line, &gag3070_dx, &gag3070_dy); /* line = dummy */
							aksi=AG3070_BOARD_AKSI_NONE;
						}
						strcpy(long_teks,"");
					}
				}
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

	//komp_tampilsemua (&bptr->skomp);
	//tpt_tampilsemua (&bptr->stestpoint);
	//komp_cari(&bptr->skomp, "C195",0);
	//komp_tampildata(&bptr->skomp);
	//node_takadadinet(&bptr->snode, &bptr->stestpoint, NULL);

	if (gag3070_fixture==0 && sdata_jumlahdata(&bptr->stestpoint)==0)
		return 0;

	return 1;
}

int ag3070_baca_fixture_node (const char *teks, _sdataptr snodptr, char *nama)
{
	char dummy[64];

	sscanf (teks, "%s %s", dummy, nama);
	RemoveChar(nama,'\"');
	return node_cari(snodptr, nama, sdata_nomorini(snodptr));
}

int ag3070_baca_fixture_probe (const char *teks, _sdataptr snodeptr, _sdataptr stestpointptr)
{
	char dummy[64];
	char probe[64];
	int x,y;
	char tb='B';
	int i;
	_sdatateks steks;

	RemoveChar ((char *)teks, ',');
	sdata_init(&steks);
	i = sdatateks_dariteks (&steks, teks, ' ', 63, 0);
	sdata_ambildatanomor (&steks, 0, probe);
	sdatateks_nomorini_int (&steks, 1, &x);
	sdatateks_nomorini_int (&steks, 2, &y);

	/* printf ("%d %d,%d\n", id, x,y); */
	if (strstr(teks," TOP")!=NULL)
	{
		tb = 'T';
	}
	x-=gag3070_dx;
	y-=gag3070_dy;
	/* convert to mils */
	x = x/10;
	y = y/10;
	sprintf (dummy, "%s ", probe);

	sdata_ambildatanomor (&steks, i-1, probe);
	if (strstr(probe,"MIL")!=NULL)
        strcat (dummy, probe);

	sdata_hapussemua (&steks);
	//node_tambahi_info (snodeptr, probe);
	return tpt_tambah (stestpointptr, node_id(snodeptr), tb, x, y, dummy);
}

int ag3070_baca_fixture (const char *namafile, boardptr bptr)
{
	FILE *f;
	char teks[256];
	char line[256];
	int i,p;
	int aksi=AG3070_BOARD_AKSI_NONE;
	int jumpa_BOARD=0;
	int node_id=-1;
	int tx,ty;

#ifdef _BEEVEE_VERBOSE_
printf ("ag3070_baca_fixture %s ...\n", namafile);
#endif

	if (!gag3070_fixture)
		return 0;

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
			ReplaceChar (line, '!', 0);
			TrimLeft(line);
			TrimRight(line);
			UpperText(line);

			if (GetLine(line,teks,&p))
			{
				i = ReplaceChar (teks, ';', 0);
				if (i<=0)
				{
					if (!jumpa_BOARD)
					{
						if (strspn(teks, "BOARD ")==6)
							jumpa_BOARD = 1;
					}
					else
					{
						if (aksi == AG3070_BOARD_AKSI_NONE)
						{
							if (strncmp(teks,"NODE ",5)==0)
							{
								aksi = AG3070_BOARD_AKSI_BACANODE;
								node_id = -1;
							}
							else if (strcmp(teks,"PROBES")==0 && node_id>=0)
							{
								aksi = AG3070_BOARD_AKSI_BACAPROBE;
								teks[0] = 0;
							}
							else if (strcmp(teks,"TOOLING")==0)
							{
								aksi = AG3070_BOARD_AKSI_BACATOOLING;
								teks[0] = 0;
							}
							else if (strcmp(teks,"TESTJET")==0)
							{
								node_id =-1;
								teks[0] = 0;
							}
						}
					}
				}

				if (aksi != AG3070_BOARD_AKSI_NONE && teks[0])
				{
					//printf ("%d : %s\n", aksi, teks);
					if (aksi==AG3070_BOARD_AKSI_BACANODE)
					{
						/* naca node */
						node_id = ag3070_baca_fixture_node (teks, &bptr->snode, line); /* line = dummy */
						if (node_id<0)
							node_id = node_tambah(&bptr->snode, line, "");
						aksi = AG3070_BOARD_AKSI_NONE;
					}
					else if (aksi==AG3070_BOARD_AKSI_BACAPROBE)
					{
						/* baca probes */
						if (strstr(teks, " ")!=NULL && node_id>=0)
							ag3070_baca_fixture_probe (teks, &bptr->snode, &bptr->stestpoint);
							//ag3070_baca_fixture_probe (teks, node_id, &bptr->stestpoint);
						else
							aksi = AG3070_BOARD_AKSI_NONE;
					}
					else if (aksi==AG3070_BOARD_AKSI_BACATOOLING)
					{
						/* baca tooling */
						sscanf (teks, "%s %d, %d", line, &tx, &ty); /* line = dummy */
						gag3070_dx=tx-gag3070_dx;
						gag3070_dy=ty-gag3070_dy;
						aksi = AG3070_BOARD_AKSI_NONE;
					}

				}
			}
			else
				aksi = AG3070_BOARD_AKSI_NONE;

			//printf ("%d %d %s\n", aksi, node_id, teks);
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

	//tpt_tampilsemua (&bptr->stestpoint);
	//node_takadadinet(&bptr->snode, &bptr->stestpoint, NULL);

	bptr->punya_probe = 1;

	if (sdata_jumlahdata(&bptr->stestpoint)==0)
		return 0;

    return 1;
}
