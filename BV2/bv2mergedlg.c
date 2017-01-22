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
#include "bv2mergedlg.h"

static int mergeType;

LRESULT CALLBACK MergeDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND mergeListCtrl;
	WORD dlgItemID;
	WORD notifID;

	mergeListCtrl = GetDlgItem (hDlg, IDC_LISTTYPE);
	switch (message)
	{
		case WM_INITDIALOG:
				AddListBoxItem (mergeListCtrl, "Traces", MERGE_TRACE);
				AddListBoxItem (mergeListCtrl, "Devices", MERGE_DEVICES);

				SelectListBoxIndex(mergeListCtrl,0);
				return TRUE;

		case WM_COMMAND:
			dlgItemID = LOWORD(wParam);
			notifID = HIWORD(wParam);
			if (dlgItemID == IDOK ||
				(dlgItemID == IDC_LISTTYPE && notifID == LBN_DBLCLK) )
			{
				if (GetListBoxCurSelItemData(mergeListCtrl, &mergeType)==LB_ERR)
					mergeType = -1;
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


int BV2MergeDlg (HINSTANCE hInst, HWND hWnd)
{
	mergeType = -1;
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_MERGE), hWnd, (DLGPROC)MergeDlgProc);
}

int GetMergeType ()
{
	return mergeType;
}
