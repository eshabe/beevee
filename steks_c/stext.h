/*
   Name: stext.h
   Author: Sihabul Milah
   Description:
   Date: 08/12/05 17:19

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


   22 maret 2006
	tambah m_min, m_max

	29 maret 2006
	tambah GetTextStartAtText
*/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _STEXT_H__INCLUDED_
#define _STEXT_H__INCLUDED_

#ifdef  __cplusplus
extern "C" {
#endif

#define Str63Len 63
#define Str255Len 255
#define StrDOSFileLen 12

typedef unsigned char Str63[64];
typedef unsigned char Str255[256];
typedef unsigned char StrDOSFile[13];

int GetLine(char *pszFileText, char *pszLineText, int *pos);
int ReplaceChar (char* wholeText, char oldC, char newC);
int RemoveChar (char* wholeText, char rmC);
int RemoveText (char* wholeText, char *rmText);
int LeftText (char *longText, char *left, int len);
int MidText (char *longText, char *mid, int start, int len);
int RightText (char *longText, char *right, int len);
void TrimLeft (char *longText);
void TrimRight (char *longText);
void UpperText (char *text);
void LowerText (char *text);

/*	GetTextUntilAChar
	INPUT
	longText = teks asli (panjang)
	len = maksimum panjang text
	aChar = karakter yg dicari sbg pemisah/pembatas
	pos = posisi awal
	OUTPUT
	text = hasil
	pos = posisi akhir, setelah aChar (mis. jika posisi aChar = 10, maka pos = 11)
	RETURN
	jarak p input dan p output; -1 jika tidak menemukan aChar
*/
int GetTextUntilAChar (char *longText, char *text, int len, char aChar, int *pos);

/*  GetTextStartAtText */
/*  Mengambil teks panjang mulai dari suatu teks */
/*  INPUT
	longText = teks asli (panjang)
	len = maksimum panjang text
	atText = teks yg dicari sbg awal
	start_pos = posisi awal
	OUTPUT
	text = hasil
	RETURN
	jarak p input dan p output; -1 jika tidak menemukan atText
*/
int GetTextStartAtText (const char *longText, char *text, int len, const char *atText, int start_pos);

void CopyText (char *dest, const char *src);

#define m_min(a,b)	((a<b)?a:b)
#define m_max(a,b)	((a>b)?a:b)

#ifdef  __cplusplus
}
#endif

#endif

