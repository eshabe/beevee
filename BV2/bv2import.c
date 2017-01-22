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

#include "bv2import.h"
#include "resource.h"
#include "toolbox.h"

/* 7 desember 2006 */
/* add Fabmaster FAB */

static int importSource;

LRESULT CALLBACK ImportDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND importListCtrl;
	WORD dlgItemID;
	WORD notifID;

	importListCtrl = GetDlgItem (hDlg, IDC_LISTTYPE);
	switch (message)
	{
		case WM_INITDIALOG:
				AddListBoxItem (importListCtrl, "BeeVee 1.x (bev)", IMPORT_BEEVEE1);
				AddListBoxItem (importListCtrl, "Agilent 3070 (board, board_xy)", IMPORT_AGILENT3070_A);
				AddListBoxItem (importListCtrl, "Agilent 3070 with fixture", IMPORT_AGILENT3070_B);
				AddListBoxItem (importListCtrl, "Agilent SJ50 (plx/pls)", IMPORT_AGILENTSJ50);
				AddListBoxItem (importListCtrl, "GenRad 228X (tac,ckt)", IMPORT_GENRAD228X_A);
				AddListBoxItem (importListCtrl, "GenRad 228X (tac,ckt,brd)", IMPORT_GENRAD228X_B);
				AddListBoxItem (importListCtrl, "My General XY", IMPORT_GENERALXY);
				AddListBoxItem (importListCtrl, "GenCAD (cad)", IMPORT_GENCAD);
				AddListBoxItem (importListCtrl, "GenRad 228X brd only", IMPORT_GENRAD228X_BRD);
				AddListBoxItem (importListCtrl, "Placement data", IMPORT_PLACEMENT);
				AddListBoxItem (importListCtrl, "Fabmaster (fab)", IMPORT_FABMASTER_FAB);
				AddListBoxItem (importListCtrl, "Unicam (pcb)", IMPORT_UNICAM_PCB);
				AddListBoxItem (importListCtrl, "GenRad 228X tac only", IMPORT_GENRAD228X_TAC);
				AddListBoxItem (importListCtrl, "GenRad 228X (tac,brd)", IMPORT_GENRAD228X_TACBRD);
				AddListBoxItem (importListCtrl, "Pad-PowerPCB (asc)", IMPORT_PADPOWERPCB_ASC);
				AddListBoxItem (importListCtrl, "BOM/Attribute (atr)",IMPORT_BOMATTRIBUTE_ATR);

				SelectListBoxIndex(importListCtrl,0);
				return TRUE;

		case WM_COMMAND:
			dlgItemID = LOWORD(wParam);
			notifID = HIWORD(wParam);
			if (dlgItemID == IDOK ||
				(dlgItemID == IDC_LISTTYPE && notifID == LBN_DBLCLK) )
			{
				if (GetListBoxCurSelItemData(importListCtrl, &importSource)==LB_ERR)
					importSource = -1;
				EndDialog(hDlg, IDOK);
				return TRUE;
			}
			else if (dlgItemID == IDCANCEL)
			{
				EndDialog(hDlg, IDCANCEL);
				return TRUE;

			}
			break;
	}
    return FALSE;
}


int BV2ImportFile (HINSTANCE hInst, HWND hWnd)
{
	importSource = -1;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_IMPORT), hWnd, (DLGPROC)ImportDlg);
}

int GetImportSource ()
{
	return importSource;
}
