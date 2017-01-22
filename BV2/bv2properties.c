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


/* bv2properties.cpp */

/*
handle IDD_PROPERTIES dialog
writen by sihabul milah
12 April 2006

  */

#include "resource.h"
#include "../bv2data/sboard.h"
#include "BV2.h"

LRESULT CALLBACK PropertiesDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char temp[256];
	boardptr bptr = GetBVBoardPtr();

	switch (message)
	{
		case WM_INITDIALOG:
			board_nama(bptr, temp);
			SetDlgItemText (hDlg, IDC_NAME, temp);
			SendDlgItemMessage (hDlg, IDC_NAME, EM_SETLIMITTEXT, (WPARAM)Str63Len, 0L);
			board_versi(bptr, temp);
			SetDlgItemText (hDlg, IDC_VERSION, temp);
			board_asal (bptr, temp);
			SetDlgItemText (hDlg, IDC_ORIGIN, temp);
			SetDlgItemInt (hDlg, IDC_COMP, sdata_jumlahdata(&bptr->skomp), FALSE);
			SetDlgItemInt (hDlg, IDC_NODE, sdata_jumlahdata(&bptr->snode), FALSE);
			SetDlgItemInt (hDlg, IDC_CONN, sdata_jumlahdata(&bptr->skonek), FALSE);
			SetDlgItemInt (hDlg, IDC_NET, sdata_jumlahdata(&bptr->stestpoint), FALSE);
			SetDlgItemInt (hDlg, IDC_TRACE, sdata_jumlahdata(&bptr->strace), FALSE);
			sprintf (temp,"%d,%d - %d,%d", board_bawah(bptr),board_kiri(bptr),board_atas(bptr),board_kanan(bptr));
			SetDlgItemText (hDlg, IDC_XY, temp);
			return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				if (LOWORD(wParam) == IDOK)
				{
					GetDlgItemText (hDlg, IDC_NAME, temp, 255);
					board_isinama (bptr, temp);
				}
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

LRESULT BV2PropertiesDlg (HINSTANCE hInst, HWND hWnd)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_PROPERTIES), hWnd, (DLGPROC)PropertiesDlg);
}
