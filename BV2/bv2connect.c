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
	bv2connect.c
	connection dialog implementation routines

	sihabul milah
	31-july-2006
*/

#include "bv2winmain.h"
#include "BV2.h"
#include "bv2draw.h"
#include "toolbox.h"
#include "bv2connect.h"

/* inernal use */
LRESULT CALLBACK BV2Conn_Proc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

typedef struct connection_struct {
	HWND hwnd_This;
	BOOL updated;
	char devname[64];
} connection_struct;

static connection_struct myconnection;

void BV2Conn_Init ()
{
	myconnection.hwnd_This = NULL;
	myconnection.updated = FALSE;
	myconnection.devname[0] = 0;
}

void BV2Conn_Erase ()
{
	DestroyWindow (myconnection.hwnd_This);
	myconnection.updated = FALSE;
}

HWND BV2Conn_InitWindow (HINSTANCE hInst, HWND hWndParent)
{
	BV2Conn_Init();
	myconnection.hwnd_This = CreateDialog (hInst, MAKEINTRESOURCE(IDD_CONNECTION), hWndParent, (DLGPROC) BV2Conn_Proc);
	return myconnection.hwnd_This;
}

BOOL BV2Conn_ShowWindow ()
{
	if (IsWindowVisible(myconnection.hwnd_This))
		return TRUE;
	return ShowWindow (myconnection.hwnd_This, SW_SHOWNORMAL);
}

BOOL BV2Conn_CloseWindow ()
{
	return ShowWindow (myconnection.hwnd_This, SW_HIDE);
}

void BV2Conn_SetLate ()
{
	SetDlgItemText (myconnection.hwnd_This, IDC_UPDATE, myconnection.devname);
	EnableWindow (GetDlgItem (myconnection.hwnd_This, IDOK),
                    (BOOL)(myconnection.devname[0]!=0));
	myconnection.updated = FALSE;
}

void BV2Conn_Update ()
{
	char tmp[256];
	boardptr bptr = GetBVBoardPtr ();
	int node_id;
	int i,j,k,count;
	HWND ctrl1 = GetDlgItem (myconnection.hwnd_This, IDC_SIGNAL);
	HWND ctrl3 = GetDlgItem (myconnection.hwnd_This, IDC_DEVCONNECT);

	SendMessage (ctrl3, LB_RESETCONTENT, 0, 0L);
	if (!myconnection.devname[0])
        return;

    DrawSetCHLiteDev (myconnection.devname, TRUE);
    count = SendMessage (ctrl1, LB_GETCOUNT, 0, 0L);
	for (i=0;i<count;i++)
	{
        node_id=SendMessage (ctrl1, LB_GETITEMDATA, i, 0L);
	    SendMessage (ctrl1, LB_GETTEXT, i, (LPARAM)(LPCSTR)tmp);
	    if (strcmp(tmp,"GND")!=0 && strcmp(tmp,"NC")!=0)
	    {
	        j = -1;
	        k = -2;
	        tmp[0]='*';
	        while ( (j = koneksi_cari (&bptr->skonek, node_id, tmp, j+1))>=0 && j!=k)
	        {
	            if (k==-2)
                    k = j;
                strtok(tmp,".");
                if (FindListBoxStringExact(ctrl3, tmp, -1)==LB_ERR)
                {
                    AddListBoxItem (ctrl3, tmp, 0L);
                    DrawSetCHLiteDev (tmp, FALSE);
                }
                tmp[0]='*';
	        }
	    }
	}
	sprintf (tmp,"%s's connection updated!", myconnection.devname);
	SetDlgItemText (myconnection.hwnd_This, IDC_UPDATE, tmp);
	//EnableWindow (GetDlgItem (myconnection.hwnd_This, IDOK), FALSE);
	myconnection.updated = TRUE;

	InvalidateRect (GetDrawWindow(), NULL, TRUE);
}

void BV2Conn_SetDeviceName (char *namakomp)
{
	boardptr bptr = GetBVBoardPtr ();
	int i,s;
	int nodeid,lid;
	char nama[64];
	char namadev[64];
	HWND ctrl1 = GetDlgItem (myconnection.hwnd_This, IDC_SIGNAL);
	//HWND ctrl2 = GetDlgItem (myconnection.hwnd_This, IDC_DEVICE);

	//SendMessage (ctrl1, LB_RESETCONTENT, 0, 0L);
	//SendMessage (ctrl2, LB_RESETCONTENT, 0, 0L);

	BV2Conn_ClearList();

	strcpy (myconnection.devname, namakomp);
	sprintf (namadev,".%s.", namakomp);

	s=-1;
	lid = nodeid = -1;
	if (sdata_kekepala(&bptr->skonek)>=0)
	{
		i = -1;
		while (1)
		{
			i = koneksi_cari_dkonek (&bptr->skonek, namadev, &nodeid, i+1);
            if (i>=0 && i>s)
            {
                if (lid!=nodeid)
                {
                    if (sdata_kenomor (&bptr->snode, nodeid)>=0)
                    {
                        node_nama (&bptr->snode, nama);
                        if (ListBox_FindStringExact(ctrl1, 0, nama)<0)
                            AddListBoxItem (ctrl1, nama, nodeid);
                    }
                    lid = nodeid;
                }
                s=i;
            }
            else
                break;
		}
	}
    BV2Conn_SetLate ();
}


void BV2Conn_SetConnectionList (int signalID, char *namasignal)
{
	boardptr bptr = GetBVBoardPtr ();
	int i,s;
	char nama[64];
	HWND ctrl2 = GetDlgItem (myconnection.hwnd_This, IDC_DEVICE);

	SendMessage (ctrl2, LB_RESETCONTENT, 0, 0L);

	s=-1;
	if (sdata_kekepala(&bptr->skonek)>=0)
	{
		i = -1;
		while (1)
		{
			strcpy (nama, "*");
			i = koneksi_cari (&bptr->skonek, signalID, nama, i+1);
            if (i>=0 && i>s)
            {
                AddListBoxItem (ctrl2, nama, i);
                s=i;
            }
            else
                break;
		}
	}
}

void BV2Conn_SetSignalIDWithName (int signalID, char *namasignal)
{
	boardptr bptr = GetBVBoardPtr ();
	int i,s;
	char nama[64];
	HWND ctrl1 = GetDlgItem (myconnection.hwnd_This, IDC_SIGNAL);
	//HWND ctrl2 = GetDlgItem (myconnection.hwnd_This, IDC_DEVICE);

	//SendMessage (ctrl1, LB_RESETCONTENT, 0, 0L);
	//SendMessage (ctrl2, LB_RESETCONTENT, 0, 0L);

	BV2Conn_ClearList();

	AddListBoxItem (ctrl1, namasignal, signalID);
	SelectListBoxIndex (ctrl1, 0);
	BV2Conn_SetConnectionList (signalID, namasignal);

	myconnection.devname[0] = 0;
	BV2Conn_SetLate ();
}

void BV2Conn_ClearList ()
{
	SendMessage (GetDlgItem (myconnection.hwnd_This, IDC_SIGNAL), LB_RESETCONTENT, 0, 0L);
	SendMessage (GetDlgItem (myconnection.hwnd_This, IDC_DEVICE), LB_RESETCONTENT, 0, 0L);
	SendMessage (GetDlgItem (myconnection.hwnd_This, IDC_DEVCONNECT), LB_RESETCONTENT, 0, 0L);
}

void BV2Conn_SetSignal (int signalID)
{
	boardptr bptr = GetBVBoardPtr ();
	char nama[64];

	if (sdata_kenomor(&bptr->snode, signalID)>=0)
	{
        node_nama(&bptr->snode, nama);
        BV2Conn_SetSignalIDWithName (signalID, nama);
	}
}

void BV2Conn_SelectSignal (char *name)
{
    boardptr bptr = GetBVBoardPtr();
    if (node_cari(&bptr->snode, name, 0)>=0)
    {
        DrawFindSignalHandler (&bptr->snode);
    }
}
void BV2Conn_SelectDevice (char *name)
{
    char namedev[64];
    boardptr bptr = GetBVBoardPtr();

    strcpy (namedev, name);
    strtok (namedev, ".");

    if (komp_cari (&bptr->skomp, namedev, 0)>=0)
    {
        DrawFindComponentHandler (&bptr->skomp);
    }
}

LRESULT CALLBACK BV2Conn_Proc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UINT objID;
	UINT ntCode;
	int nodeid;
	char signal[64];
	char device[64];

	switch (message)
	{
	case WM_INITDIALOG:
		BV2Conn_SetLate ();
		return TRUE;

	case WM_CLOSE:
		BV2Conn_CloseWindow ();
		break;

    case WM_COMMAND:
        objID = LOWORD(wParam); /* object id */
        ntCode = HIWORD(wParam); /* nofication code */
        if (objID==IDC_SIGNAL && ntCode == LBN_SELCHANGE)
        {
            if ( GetListBoxCurSelItemDataAndString (GetDlgItem (hDlg, IDC_SIGNAL),
            &nodeid,
            signal) != LB_ERR )
            {
                BV2Conn_SetConnectionList (nodeid, signal);
                BV2Conn_SelectSignal (signal);
            }
        }
        else if (objID==IDC_DEVICE && ntCode == LBN_SELCHANGE)
        {
            if ( GetListBoxCurSelString (GetDlgItem (hDlg, objID), device) != LB_ERR )
            {
                BV2Conn_SelectDevice (device);
            }
        }
        else if (objID==IDC_DEVICE && ntCode == LBN_DBLCLK)
        {
            if ( GetListBoxCurSelString (GetDlgItem (hDlg, objID), device) != LB_ERR )
            {
                strtok(device,".");
                BV2Conn_SetDeviceName (device);
            }
        }
        else if (objID==IDC_DEVCONNECT && ntCode == LBN_SELCHANGE)
        {
            if ( GetListBoxCurSelString (GetDlgItem (hDlg, objID), device) != LB_ERR )
            {
                BV2Conn_SelectDevice (device);
            }
        }
        else if (objID==IDC_DEVCONNECT && ntCode == LBN_DBLCLK)
        {
            if ( GetListBoxCurSelString (GetDlgItem (hDlg, objID), device) != LB_ERR )
            {
                BV2Conn_SetDeviceName (device);
            }
        }
        else if (objID==IDOK)
        {
            BV2Conn_Update ();
        }
       break;


	}

	return FALSE;
}
