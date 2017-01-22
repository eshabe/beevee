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

#include "BV2.h"

/* 061009 initial */

/* 070222
- perbaikan minor tampilan -g tertulis -d
*/

#define _FINDBV_DEVICE  1
#define _FINDBV_SIGNAL  2
#define _FINDBV_NODE    3
#define _FINDBV_NAIL    4
#define _FINDBV_PROBE   5
#define _FINDBV_CONNECTION   6 // 2.0.5

#define BEEVEE2_FIND_COMMANDEX  0x400 /*1025*/

void ShowMyHelp ()
{
    MessageBox (NULL, "Beevee FindEx Command> BeeVee {-option:objectname|h}\n\nh = this help\
    \noptions:\n d = find device, -d:R123\
    \n g = find signal, -g:TX_OUT\
    \n n = find node, -n:N1234\
    \n l = find nail, -l:F3456\
    \n p = find probe, -p:P321", "FindEx (061009)", MB_ICONINFORMATION);
}

static  char findStr[64];

int BV2FindEx (LPSTR lpszArgument)
{
    int findType=-1;
    UINT cmd=0;

    if ((lpszArgument[0]=='h' || lpszArgument[0]=='H') &&  strlen(lpszArgument)==1)
    {
        ShowMyHelp ();
        return -1;
    }
    if (strlen(lpszArgument)<4)
        return -2;

    if (lpszArgument[0]!='-' || lpszArgument[2]!=':')
        return -3;

    switch (lpszArgument[1])
    {
        case 'd':
        case 'D':
            cmd = BEEVEE2_FIND_COMMANDEX + _FINDBV_DEVICE;
            break;
        case 'g':
        case 'G':
            cmd = BEEVEE2_FIND_COMMANDEX + _FINDBV_SIGNAL;
            break;
        case 'n':
        case 'N':
            cmd = BEEVEE2_FIND_COMMANDEX + _FINDBV_NODE;
            break;
        case 'l':
        case 'L':
            cmd = BEEVEE2_FIND_COMMANDEX + _FINDBV_NAIL;
            break;
        case 'p':
        case 'P':
             cmd = BEEVEE2_FIND_COMMANDEX + _FINDBV_PROBE;
            break;
        case 't':
        case 'T':
             cmd = BEEVEE2_FIND_COMMANDEX + _FINDBV_CONNECTION;
            break;
    }

    if (cmd==0)
    {
        MessageBox (NULL, "Unkown find command!", "FindEx", MB_ICONSTOP);
        return 1;
    }
    strcpy (findStr, &lpszArgument[3]);

    HWND bvwnd = FindWindow (BV2_WINCLASSNAME, NULL);
    if (bvwnd==NULL)
    {
        MessageBox (NULL, "Can not find BeeVee 2 window!", "FindEx", MB_ICONSTOP);
        return 2;
    }
    unsigned int i=0;
    for (i=0;i<strlen(findStr);i++)
        SendMessage (bvwnd, WM_COMMAND, BEEVEE2_FIND_COMMANDEX, (LPARAM)findStr[i]);
    SendMessage (bvwnd, WM_COMMAND, cmd, 0L);
    return 0;
}


