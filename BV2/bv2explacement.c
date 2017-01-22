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
#include "resource.h"
#include "bv2explacement.h"

_explace_option exptheopt;

void ExPlacementInit ()
{
    memset (&exptheopt, 0, sizeof(exptheopt));
}

void ExPlacementGetOption (_explace_option *optptr)
{
    memcpy (optptr, &exptheopt, sizeof(exptheopt));
}

void ExPlacementSetOptionUnit (int unit)
{
    exptheopt.unit = unit;
}

int ExPlacementIsRefdesOnly ()
{
    return exptheopt.refdesonly;
}

LRESULT CALLBACK ExPlacementDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	char temp[256];

	switch (message)
	{
		case WM_INITDIALOG:
            CheckDlgButton(hDlg, IDC_POSZ, exptheopt.topbot);
            CheckDlgButton(hDlg, IDC_ORIENT, exptheopt.orient);
            CheckDlgButton(hDlg, IDC_SHAPE, exptheopt.shape);
            CheckDlgButton(hDlg, IDC_PN, exptheopt.pn);
            CheckDlgButton(hDlg, IDC_INFO, exptheopt.info);
            CheckDlgButton(hDlg, IDC_XREFDESONLY, exptheopt.refdesonly);
            CheckRadioButton (hDlg, IDC_XTOP, IDC_XBOTH, IDC_XBOTH);
            exptheopt.loc = 0;
			return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				if (LOWORD(wParam) == IDOK)
				{
				    exptheopt.topbot = IsDlgButtonChecked (hDlg, IDC_POSZ);
				    exptheopt.orient = IsDlgButtonChecked (hDlg, IDC_ORIENT);
                    exptheopt.shape = IsDlgButtonChecked (hDlg, IDC_SHAPE);
                    exptheopt.pn = IsDlgButtonChecked (hDlg, IDC_PN);
                    exptheopt.info = IsDlgButtonChecked (hDlg, IDC_INFO);
                    exptheopt.refdesonly = IsDlgButtonChecked (hDlg, IDC_XREFDESONLY);
                    if (IsDlgButtonChecked (hDlg, IDC_XBOTH))
                        exptheopt.loc = 0;
                    else if (IsDlgButtonChecked (hDlg, IDC_XTOP))
                        exptheopt.loc = 1;
                    else if (IsDlgButtonChecked (hDlg, IDC_XBOTTOM))
                        exptheopt.loc = 2;
				}
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

LRESULT BV2ExPlacementDlg (HINSTANCE hInst, HWND hWnd)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_EXPLACEMENT), hWnd, (DLGPROC)ExPlacementDlgProc);
}
