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


#include "resource.h"
#include "bv2placement.h"
#include "../steks_c/stext.h"

importplacement_struct myplacement;

LRESULT CALLBACK HowImportPlacementDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_INITDIALOG:
        return TRUE;
        case WM_COMMAND:
        if (LOWORD(wParam)==IDCANCEL)
            EndDialog(hDlg, 0L);
        break;
    }
    return FALSE;
}

void ImportPlacement_GetResults (importplacement_struct *_result)
{
    memcpy (_result, &myplacement, sizeof (myplacement));
}

LRESULT CALLBACK ImportPlacementDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    int dlgItemID;
    HWND mCtrl;
    char tmp[64];

    switch (message)
    {
        case WM_INITDIALOG:
        CheckRadioButton (hDlg, IDC_RBCOLUMN, IDC_RBCHAR, IDC_RBCOLUMN);
        CheckRadioButton (hDlg, IDC_BTSPACE, IDC_BTOTHERCHAR, IDC_BTTAB);
        SetDlgItemText (hDlg, IDC_REFDES, "1");
        SetDlgItemText (hDlg, IDC_POSX, "2");
        SetDlgItemText (hDlg, IDC_POSY, "3");
        SetDlgItemText (hDlg, IDC_ORIENT, "4");
        SetDlgItemText (hDlg, IDC_POSZ, "5");
        SetDlgItemText (hDlg, IDC_PN, "0");
        SetDlgItemText (hDlg, IDC_SHAPE, "0");
        SetDlgItemText (hDlg, IDC_TOKENSTRING, ";");
        SetDlgItemText (hDlg, IDC_BTMMARK, "B");
        SetDlgItemText (hDlg, IDC_STARTLINE, "1");
        return TRUE;

        case WM_COMMAND:
        dlgItemID = LOWORD(wParam);
        if (dlgItemID==IDOK)
        {
            memset (&myplacement, 0, 0);
            myplacement.column = IsDlgButtonChecked (hDlg, IDC_RBCOLUMN);
            GetDlgItemText (hDlg, IDC_EDOTHERCHAR, myplacement.delimiter,2);
            GetDlgItemText (hDlg, IDC_TOKENSTRING, myplacement.token,2);
            GetDlgItemText (hDlg, IDC_BTMMARK, myplacement.bottom,2);
            GetDlgItemText (hDlg, IDC_STARTLINE, tmp,64);
            myplacement.sline = atoi (tmp);

            /*
            sprintf (tmp, "delimiter=%s\ntoken=%s\nbottom=%s\nline=%d",
                myplacement.delimiter,
                myplacement.token,
                myplacement.bottom,
                myplacement.sline);
            MessageBox (hDlg, tmp, NULL, 0);
            */

            if (IsDlgButtonChecked (hDlg, IDC_BTSPACE))
                strcpy (myplacement.delimiter," ");
            else if (IsDlgButtonChecked (hDlg, IDC_BTTAB))
                strcpy (myplacement.delimiter,"\t");
            else if (myplacement.column  && !myplacement.delimiter[0])
            {
                MessageBox (hDlg, "Base is set as Column but Delimiter is blank!", NULL, MB_ICONSTOP);
                break;
            }
            if (myplacement.column)
            {
                char temp[64];
                GetDlgItemText (hDlg, IDC_REFDES, temp,64);
                myplacement.refdes[0] = atoi (temp)-1;
                GetDlgItemText (hDlg, IDC_POSX, temp,64);
                myplacement.posx[0] = atoi (temp)-1;
                GetDlgItemText (hDlg, IDC_POSY, temp,64);
                myplacement.posy[0] = atoi (temp)-1;
                GetDlgItemText (hDlg, IDC_POSZ, temp,64);
                myplacement.posz[0] = atoi (temp)-1;
                GetDlgItemText (hDlg, IDC_ORIENT, temp,64);
                myplacement.orient[0] = atoi (temp)-1;
                GetDlgItemText (hDlg, IDC_PN, temp,64);
                myplacement.pn[0] = atoi (temp)-1;
                GetDlgItemText (hDlg, IDC_SHAPE, temp,64);
                myplacement.shape[0] = atoi (temp)-1;

                if (myplacement.refdes[0]<0 || myplacement.posx[0]<0 || myplacement.posy[0]<0 || myplacement.posz[0]<0 || myplacement.orient[0]<0)
                {
                    MessageBox (hDlg, "RefDes, PosX, PosY, TopBot and Orientation may not be blank or less then 1!", NULL, MB_ICONSTOP);
                    break;
                }
            }
            else
            {
                char temp[64];
                char *temp_2;
                int errtxt=0;
                GetDlgItemText (hDlg, IDC_REFDES, temp,64);
                if ((temp_2=strchr(temp,'-'))==NULL)
                    errtxt=1;
                else
                {
                    myplacement.refdes[0] = atoi (temp)-1;
                    myplacement.refdes[1] = atoi (temp_2+1)-1;
                }
                GetDlgItemText (hDlg, IDC_POSX, temp,64);
                if ((temp_2=strchr(temp,'-'))==NULL)
                    errtxt=1;
                else
                {
                    myplacement.posx[0] = atoi (temp)-1;
                    myplacement.posx[1] = atoi (temp_2+1)-1;
                }
                GetDlgItemText (hDlg, IDC_POSY, temp,64);
                if ((temp_2=strchr(temp,'-'))==NULL)
                    errtxt=1;
                else
                {
                    myplacement.posy[0] = atoi (temp)-1;
                    myplacement.posy[1] = atoi (temp_2+1)-1;
                }
                GetDlgItemText (hDlg, IDC_POSZ, temp,64);
                if ((temp_2=strchr(temp,'-'))==NULL)
                    errtxt=1;
                else
                {
                    myplacement.posz[0] = atoi (temp)-1;
                    myplacement.posz[1] = atoi (temp_2+1)-1;
                }
                GetDlgItemText (hDlg, IDC_ORIENT, temp,64);
                if ((temp_2=strchr(temp,'-'))==NULL)
                    errtxt=1;
                else
                {
                    myplacement.orient[0] = atoi (temp)-1;
                    myplacement.orient[1] = atoi (temp_2+1)-1;
                }
                GetDlgItemText (hDlg, IDC_PN, temp,64);
                if ((temp_2=strchr(temp,'-'))==NULL)
                    errtxt=1;
                else
                {
                    myplacement.pn[0] = atoi (temp)-1;
                    myplacement.pn[1] = atoi (temp_2+1)-1;
                }
                GetDlgItemText (hDlg, IDC_SHAPE, temp,64);
                if ((temp_2=strchr(temp,'-'))==NULL)
                    errtxt=1;
                else
                {
                    myplacement.shape[0] = atoi (temp)-1;
                    myplacement.shape[1] = atoi (temp_2+1)-1;
                }

                if (errtxt)
                {
                    MessageBox (hDlg, "Base is set as Character.\nPlease set RefDes field (and friends) as a range (1-5,6-8,..)!", NULL, MB_ICONSTOP);
                    break;
                }
                else if (myplacement.refdes[0]<0 || myplacement.posx[0]<0 || myplacement.posy[0]<0 || myplacement.posz[0]<0 || myplacement.orient[0]<0 ||
                         myplacement.refdes[1]<0 || myplacement.posx[1]<0 || myplacement.posy[1]<0 || myplacement.posz[1]<0 || myplacement.orient[1]<0 )
                {
                    MessageBox (hDlg, "RefDes, PosX, PosY, TopBot and Orientation may not be blank or less then 1!", NULL, MB_ICONSTOP);
                    break;
                }
            }
            EndDialog(hDlg, IDOK);
        }
        else if (dlgItemID==IDCANCEL)
        {
            EndDialog(hDlg, dlgItemID);
        }
        else if (dlgItemID==IDC_HOW)
            DialogBox (GetModuleHandle(0), MAKEINTRESOURCE(IDD_HOWIMPORTPLACEMENT), hDlg, (DLGPROC)HowImportPlacementDlgProc);
        break;

        case WM_DESTROY:
        break;
    }
    return FALSE;
}


UINT PlacementDialog (HINSTANCE hInst, HWND hWnd)
{
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_IMPORTPLACEMENT), hWnd, (DLGPROC)ImportPlacementDlgProc);
}
