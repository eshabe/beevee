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

#include <windows.h>
#include <windowsx.h>

#include "resource.h"
#include "BV2.h"
#include "bv2draw.h"
#include "../bv2data/sboard.h"

komponen edkomp;
BOOL do_edit;

BOOL EditDeviceUpdateKoneksi (komponen *komptr)
{
	boardptr bptr = GetBVBoardPtr();
	int i=0;
	int k = sdata_jumlahdata(&bptr->skonek);
	Str63 tkon;
	Str63 nekp;
	char *tmp;
	koneksi mkon;
	if (k<=0)
        return FALSE;
	sprintf (tkon,".%s.",edkomp.nama);
	while ( (i = koneksi_carijangkau (&bptr->skonek, -1, tkon, i, k)) >=0 )
	{
        sdata_ambildataini (&bptr->skonek, &mkon);
        if ( (tmp=strchr(mkon.komp_pin,'.'))!=NULL)
        {
            tmp++;
            sprintf (nekp,"%s.%s",komptr->nama,tmp);
            strcpy (mkon.komp_pin,nekp);
            sdata_baruiini (&bptr->skonek, &mkon, sizeof(koneksi));
        }
        ++i;
	}
	//if (komp_cari (&bptr->skomp, edkomp.nama, 0)>=0)
    //    DrawFindComponentHandler (&bptr->skomp);
	return TRUE;
}

BOOL EditDeviceUpdateKomponen (komponen *komptr)
{
	boardptr bptr = GetBVBoardPtr();
	int i = komp_cari (&bptr->skomp, edkomp.nama, 0);
	if (i>=0)
	{
		do_edit = TRUE;
		sdata_baruiini(&bptr->skomp, komptr, sizeof(komponen));
		if (komptr->pos_x1<board_kiri(bptr) || komptr->pos_y1<board_bawah(bptr) ||
            komptr->pos_x2>board_kanan(bptr) || komptr->pos_y2>board_atas(bptr))
        {
            board_hitung_ataskiri(bptr);
            sdata_kenomor (&bptr->skomp, i);
        }

		DrawFindComponentHandler (&bptr->skomp);
		memcpy (&edkomp, komptr, sizeof(komponen));
		//InvalidateRect (GetDrawWindow(), NULL, TRUE);
		return TRUE;
	}
	return FALSE;
}

BOOL EditDeviceCheckRefDes (char *refdes)
{
    unsigned int i;
    for (i=0;i<strlen(refdes);i++)
    {
        if (!(refdes[i]=='_'  || refdes[i]=='-'||  refdes[i]>='A' && refdes[i]<='Z' ||
            refdes[i]>='0' && refdes[i]<='9'))
            return FALSE;
    }
    return TRUE;
}

BOOL EditDeviceCheckRefDesExist (char *refdes)
{
	boardptr bptr = GetBVBoardPtr();
	return (komp_cari (&bptr->skomp, refdes, 0)>=0);
}

void EditDeviceDoManip (HWND hDlg)
{
    komponen tkomp;
    int or;
    char tbtemp[7];
    BOOL neg;
    BOOL changeminor = FALSE;
    HWND TBCtrl = GetDlgItem (hDlg, IDC_LOC);

    GetDlgItemText (hDlg, IDC_REFDES, tkomp.nama, Str63Len);
    GetDlgItemText (hDlg, IDC_INFO, tkomp.info, Str255Len);
    ComboBox_GetText (TBCtrl, tbtemp, 6);
    or = GetDlgItemInt (hDlg, IDC_ORIENT, &neg, FALSE);
    tkomp.pos_x1 = GetDlgItemInt (hDlg, IDC_EDX1, &neg, TRUE);
    tkomp.pos_x2 = GetDlgItemInt (hDlg, IDC_EDX2, &neg, TRUE);
    tkomp.pos_y1 = GetDlgItemInt (hDlg, IDC_EDY1, &neg, TRUE);
    tkomp.pos_y2 = GetDlgItemInt (hDlg, IDC_EDY2, &neg, TRUE);
    strupr (tkomp.nama);
    strupr (tkomp.info);
    tkomp.orentasi = (short)or;
    tkomp.pos_tb = tbtemp[0];

    if (!EditDeviceCheckRefDes(tkomp.nama))
    {
        MessageBox (hDlg, "The RefDes must contain 'A'-'Z','0'-'9','_' and '-' only!", "Edit Error", MB_OK | MB_ICONSTOP);
        return;
    }

    // ganti nama efeknya banyak
    else if (strcmp(tkomp.nama,edkomp.nama)!=0)
    {
        if (EditDeviceCheckRefDesExist(tkomp.nama))
        {
            MessageBox (hDlg, "The RefDes is already exist!", "Edit Error", MB_OK | MB_ICONSTOP);
            return;
        }
       EditDeviceUpdateKoneksi (&tkomp);
        EditDeviceUpdateKomponen (&tkomp);
    }
    // ganti selain nama update data komponen saja
    else if (tkomp.orentasi != edkomp.orentasi ||
    tkomp.pos_tb != edkomp.pos_tb ||
    tkomp.pos_x1 != edkomp.pos_x1 ||
    tkomp.pos_x2 != edkomp.pos_x2 ||
    tkomp.pos_y1 != edkomp.pos_y1 ||
    tkomp.pos_y2 != edkomp.pos_y2 ||
    strcmp(tkomp.info,edkomp.info)!=0)
    {
        // manipulasi di sini
        EditDeviceUpdateKomponen (&tkomp);
    }
}

LRESULT CALLBACK EditDeviceDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND TBCtrl = NULL;
	WORD dlgItemID;
	WORD notifID;

	TBCtrl = GetDlgItem (hDlg, IDC_LOC);
	switch (message)
	{
		case WM_INITDIALOG:
            do_edit = FALSE;
            ComboBox_AddString (TBCtrl, "TOP");
            ComboBox_AddString (TBCtrl, "BOTTOM");
            Edit_LimitText (GetDlgItem (hDlg, IDC_REFDES), Str63Len-5);
            Edit_LimitText (GetDlgItem (hDlg, IDC_INFO), Str255Len);
            if (DrawGetHLiteDev (&edkomp))
            {
                SetDlgItemText (hDlg, IDC_REFDES, edkomp.nama);
                SetDlgItemText (hDlg, IDC_INFO, edkomp.info);
                SetDlgItemInt (hDlg, IDC_ORIENT, edkomp.orentasi, FALSE);
                SetDlgItemInt (hDlg, IDC_EDX1, edkomp.pos_x1, TRUE);
                SetDlgItemInt (hDlg, IDC_EDX2, edkomp.pos_x2, TRUE);
                SetDlgItemInt (hDlg, IDC_EDY1, edkomp.pos_y1, TRUE);
                SetDlgItemInt (hDlg, IDC_EDY2, edkomp.pos_y2, TRUE);
                ComboBox_SelectString (TBCtrl, 0, (edkomp.pos_tb=='T')?"TOP":"BOTTOM");
            }
            else
                ComboBox_SetCurSel (TBCtrl, 0);

            return TRUE;

		case WM_COMMAND:
			dlgItemID = LOWORD(wParam);
			notifID = HIWORD(wParam);
			if (dlgItemID == IDOK || dlgItemID == IDC_APPLY)
			{
				// do manipulation
				EditDeviceDoManip (hDlg);

				if (dlgItemID == IDC_APPLY)
                    break;
				EndDialog(hDlg, do_edit);
				return TRUE;
			}
			else if (dlgItemID == IDCANCEL)
			{
				EndDialog(hDlg, do_edit);
				return TRUE;

			}
			break;
	}
    return FALSE;
}


UINT BV2EditDeviceDlg (HINSTANCE hInst, HWND hWnd)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_EDITDEV), hWnd, (DLGPROC)EditDeviceDlgProc);
}

