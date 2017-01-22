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
find handlers
 9 oktober 2006
 - gunakan GettBV2MainWindow()

22 Sept 2006
- keep focus at text-edit
- add "@!" command to close window (FindBar)
*/

#include "bv2find.h"
#include "BV2.h"
#include "bv2draw.h"
#include "toolbox.h"
#include "statusbar.h"
#include "bv2object.h"
#include "../sdata_c/sdatateks.h"

/* globals */
typedef struct find_struct {
	HWND hwnd_This;
	int i_mode;
	int i_objectType;
	char cstr_device[64];
	char cstr_signal[64];
	char cstr_node[64];
	char cstr_nail[64];
	char cstr_probe[64];
	int hi;
} find_struct;


static find_struct myfind;
//const int kFindBarHI = 24;
const char objnama[5][7] = { "Device", "Signal", "Node", "Nail", "Probe" };

/* Find an object *********************/

int FindComponent (HWND hWnd, char *nama)
{
	boardptr bptr = GetBVBoardPtr();
	int i = komp_cari (&bptr->skomp, nama, 0);
	if (i>=0)
	{
		komponen komp;
		sdata_ambildataini(&bptr->skomp, &komp);

		if (komp.pos_x1 == 0 &&
			komp.pos_y1 == 0 &&
			komp.pos_x2 == 0 &&
			komp.pos_y2 == 0 )
		{
			MessageBox (GettBV2MainWindow(), "The device doesn't have location", "Find", MB_OK);
		}
		else
		{
			BV2Conn_SetDeviceName (nama); //2.0.5
			DrawFindComponentHandler (&bptr->skomp);
			return 1;
		}
	}
	else
	{
        char msg[256];
        sprintf (msg, "Device %s is not found!", nama);
        if (myfind.i_mode==0)
            MessageBox (hWnd, msg, "Error", MB_OK | MB_ICONERROR);
        else
            SetStatusBarText (msg);
       MessageBeep(1);
	}
	return 0;
}

int FindSignal (HWND hWnd, char *nama)
{
	boardptr bptr = GetBVBoardPtr();
	int i = node_cari (&bptr->snode, nama, -1);
	if (i>=0)
	{
		BV2Conn_SetSignalIDWithName (i, nama); //2.0.5
		DrawFindSignalHandler (&bptr->snode);
		return 1;
	}
	else
	{
		char msg[256];
		sprintf (msg, "Signal %s is not found!", nama);
		if (myfind.i_mode==0)
            MessageBox (hWnd, msg, "Error", MB_OK | MB_ICONERROR);
        else
            SetStatusBarText (msg);
        MessageBeep(1);
	}
	return 0;
}

/* node here is node known in GenRAD228x not in Agilent3070 or otherelse */
/* .... and nail */
int FindNode (HWND hWnd, char *nama)
{
	boardptr bptr = GetBVBoardPtr();
	Str63 n_node;
	int i;
	int node_id = -1;

	if (myfind.i_objectType == find_Node && nama[0]!='N')
		sprintf (n_node, "N%s ", nama);
	else if (myfind.i_objectType == find_Nail && nama[0]!='F')
		sprintf (n_node, "F%s ", nama);
	else
	{
		strcpy (n_node, nama);
		strcat (n_node, " ");
	}

	i = tpt_caridiinfo (&bptr->stestpoint, n_node, -1, &node_id);
	/* shb 20070831 - I forgot to change 'name' to 'n_node' :( */
	if (i>=0)
	{
		BV2Conn_SetSignal (node_id);
		//DrawFindSignalHandler (&bptr->snode);
		DrawFindProbeHandler (&bptr->stestpoint, n_node); /* shb 20070831 */
		return 1;
	}
	else
	{
		char msg[256];
		sprintf (msg, "Node/nail %s is not found!", n_node); /* shb 20070831 */
		if (myfind.i_mode==0)
            MessageBox (hWnd, msg, "Error", MB_OK | MB_ICONERROR);
        else
            SetStatusBarText (msg);
        MessageBeep(1);
	}
	return 0;
}

int FindProbe (HWND hWnd, char *nama)
{
	boardptr bptr = GetBVBoardPtr();
	Str63 probename;
	int i;
	int node_id = -1;

	sprintf (probename,"%s ", nama);
	i = tpt_caridiinfo (&bptr->stestpoint, probename, -1, &node_id);
	if (i>=0)
	{
		BV2Conn_SetSignal (node_id);
		DrawFindProbeHandler (&bptr->stestpoint, nama);
		return 1;
	}
	else
	{
		char msg[256];
		sprintf (msg, "Probe %s is not found.", nama);
		if (myfind.i_mode==0)
            MessageBox (hWnd, msg, "Error", MB_OK | MB_ICONERROR);
        else
            SetStatusBarText (msg);
        MessageBeep(1);
	}
	return 0;
}


void FindClearStruct ()
{
	int oldmode = myfind.i_mode;
	if (IsWindow(myfind.hwnd_This))
	{
		SendMessage (myfind.hwnd_This, WM_CLOSE, 0, 0L);
	}

	memset (&myfind, 0, sizeof(find_struct));
	myfind.i_mode= oldmode;
}

BOOL FindDialogIsVisible ()
{
	return IsWindowVisible(myfind.hwnd_This);
}

HWND GetFindDialog ()
{
	return myfind.hwnd_This;
}

void SetFindType (int type)
{
    myfind.i_objectType = type;
}

int GetFindMode ()
{
	return myfind.i_mode;
}

int GetFindBarHeight ()
{
	return myfind.hi;
	//return kFindBarHI;
}

void SetFindMode (int newmode)
{
	if (newmode!=myfind.i_mode)
	{
		if (IsWindow(myfind.hwnd_This))
			SendMessage (myfind.hwnd_This, WM_CLOSE, 0, 0L);
		myfind.i_mode = newmode;
	}
}

/* Fill List Box *********************/

void FindFillWithComponents (HWND ctrl)
{
	boardptr brdptr =	GetBVBoardPtr ();
	if (sdata_kekepala(&brdptr->skomp)>=0)
	{
		komponen k;
		while (sdata_ambildataini(&brdptr->skomp, &k)>=0)
		{
			SendMessage (ctrl, LB_ADDSTRING, 0, (LPARAM)k.nama);
			if (sdata_diekorkah(&brdptr->skomp))
				break;
			sdata_kedepan(&brdptr->skomp);
		}
	}
}

void FindFillWithSignals (HWND ctrl)
{
	boardptr brdptr =	GetBVBoardPtr ();
	if (sdata_kekepala(&brdptr->snode)>=0)
	{
		node nd;
		while (sdata_ambildataini(&brdptr->snode, &nd)>=0)
		{
			SendMessage (ctrl, LB_ADDSTRING, 0, (LPARAM)nd.nama);
			if (sdata_diekorkah(&brdptr->snode))
				break;
			sdata_kedepan(&brdptr->snode);
		}
	}
}

/* genrad228x */
void FindFillWithNodes (HWND ctrl)
{
	boardptr brdptr =	GetBVBoardPtr ();
	_sdatateks steks;
	sdata_init (&steks);

	if (sdata_kekepala(&brdptr->stestpoint)>=0)
	{
		char info[256];
		while (tpt_info(&brdptr->stestpoint, info)>=0)
		{
			if (sdatateks_dariteks (&steks, info, ' ', 63, 0)>0)
			{
				char n_node[64];
				sdata_ambildatanomor(&steks, 0, n_node);
				if (n_node[0]=='N' && n_node[1]>='0' && n_node[1]<='9') /* Nnnnn */
					SendMessage (ctrl, LB_ADDSTRING, 0, (LPARAM)n_node);
			}
			sdata_hapussemua (&steks);
			if (sdata_diekorkah(&brdptr->stestpoint))
				break;
			sdata_kedepan(&brdptr->stestpoint);
		}
		//SelectListBoxString (ctrl, myfind.cstr_node);
	}
}

/* genrad228x */
void FindFillWithNails (HWND ctrl)
{
	boardptr brdptr =	GetBVBoardPtr ();
	_sdatateks steks;
	sdata_init (&steks);

	if (sdata_kekepala(&brdptr->stestpoint)>=0)
	{
		char info[256];
		while (tpt_info(&brdptr->stestpoint, info)>=0)
		{
			int ci;
			if ( (ci=sdatateks_dariteks (&steks, info, ' ', 63, 0)) >=2 )
			{
				int i;
				char n_nail[64];
				for (i=1;i<ci;i++)
				{
					sdata_ambildatanomor(&steks, i, n_nail);
					if (n_nail[0]=='F' && n_nail[1]>='0' && n_nail[1]<='9') /* Fnnnn */
						SendMessage (ctrl, LB_ADDSTRING, 0, (LPARAM)n_nail);
					else
						break;
				}
			}
			sdata_hapussemua (&steks);
			if (sdata_diekorkah(&brdptr->stestpoint))
				break;
			sdata_kedepan(&brdptr->stestpoint);
		}
	}
}

/* agilent3070fix */
void FindFillWithProbes (HWND ctrl)
{
	boardptr brdptr =	GetBVBoardPtr ();
	_sdatateks steks;
	sdata_init (&steks);

	if (sdata_kekepala(&brdptr->stestpoint)>=0)
	{
		char info[256];
		while (tpt_info(&brdptr->stestpoint, info)>=0)
		{
			int ci;
			if ( (ci=sdatateks_dariteks (&steks, info, ' ', 63, 0)) >=1 )
			{
				int i;
				char n_probe[64];
				for (i=0;i<ci;i++)
				{
					sdata_ambildatanomor(&steks, i, n_probe);
					if (n_probe[0]=='P' && n_probe[1]>='0' && n_probe[1]<='9') /* Pnnnn */
						SendMessage (ctrl, LB_ADDSTRING, 0, (LPARAM)n_probe);
					else
						break;
				}
			}
			sdata_hapussemua (&steks);
			if (sdata_diekorkah(&brdptr->stestpoint))
				break;
			sdata_kedepan(&brdptr->stestpoint);
		}
	}
}

void FindSetNameText ()
{
	SetFocus(GetDlgItem (myfind.hwnd_This, IDC_NAME));
	if (myfind.i_mode==1)
	{
        SetDlgItemText (myfind.hwnd_This, IDC_NAME, "");
        return;
	}

	switch (myfind.i_objectType)
	{
	case find_Device:
		SetDlgItemText (myfind.hwnd_This, IDC_NAME, myfind.cstr_device);
		break;
	case find_Signal:
		SetDlgItemText (myfind.hwnd_This, IDC_NAME, myfind.cstr_signal);
		break;
	case find_Node:
		SetDlgItemText (myfind.hwnd_This, IDC_NAME, myfind.cstr_node);
		break;
	case find_Nail:
		SetDlgItemText (myfind.hwnd_This, IDC_NAME, myfind.cstr_nail);
		break;
	case find_Probe:
		SetDlgItemText (myfind.hwnd_This, IDC_NAME, myfind.cstr_probe);
		break;
	}
}

void FindFillListView ()
{
	HWND ctrlView = GetDlgItem (myfind.hwnd_This, IDC_LISTTEXT);

	SendMessage (ctrlView, LB_RESETCONTENT, 0, 0L); // reset content
	switch (myfind.i_objectType)
	{
	case find_Device:
		FindFillWithComponents (ctrlView);
		break;
	case find_Signal:
		FindFillWithSignals (ctrlView);
		break;
	case find_Node:
		FindFillWithNodes (ctrlView);
		break;
	case find_Nail:
		FindFillWithNails (ctrlView);
		break;
	case find_Probe:
		FindFillWithProbes (ctrlView);
		break;
	}
}

void FindFindBtnClickHandle ()
{
	char text[256];
	int ret=0;
	char *temp;
	GetDlgItemText (myfind.hwnd_This, IDC_NAME, text, 255);
	if (!text[0])
		return;

    if (BV2Option_IsEnableFindBarEndBySpace() && (temp=strchr(text,' '))!=NULL)
        *temp = 0;

	switch (myfind.i_objectType)
	{
	case find_Device:
		strcpy (myfind.cstr_device, text);
		ret = FindComponent (myfind.hwnd_This,text);
		break;
	case find_Signal:
		strcpy (myfind.cstr_signal, text);
		ret = FindSignal(myfind.hwnd_This, text);
		break;
	case find_Node:
		strcpy (myfind.cstr_node, text);
		ret = FindNode (myfind.hwnd_This, text);
		break;
	case find_Nail:
		strcpy (myfind.cstr_nail, text);
		ret = FindNode (myfind.hwnd_This, text);
		break;
	case find_Probe:
		strcpy (myfind.cstr_probe, text);
		ret = FindProbe (myfind.hwnd_This, text);
		break;
	}
	if (myfind.i_mode==0 && ret)
        EndDialog (myfind.hwnd_This, 0);
}


void MoveFindBar ()
{
	if (IsWindow(myfind.hwnd_This) && myfind.i_mode==kFindBarMode)
	{
		RECT r;
		GetClientRect (GetParent(myfind.hwnd_This), &r);
		r.top = r.bottom - myfind.hi;
		if (IsWindowVisible(GetStatusBarWindow()))
		{
			r.top -= GetStatusBarHeight();
		}
		if (IsWindowVisible(GetObjectInfoWindow()) && GetObjectInfoMode()==kFindBarMode)
		{
			r.top -= GetObjectInfoHeight();
		}
		MoveWindow (myfind.hwnd_This, r.left, r.top, r.right-r.left, myfind.hi, TRUE);
	}
}

void FindDlgInitDialog ()
{
	HWND objCB = GetDlgItem (myfind.hwnd_This, IDC_OBJECT);
	HWND nameEB = GetDlgItem (myfind.hwnd_This, IDC_NAME);

	boardptr brdptr;
	char asal[64];

	brdptr = GetBVBoardPtr ();

	board_asal (brdptr, asal);

	SendMessage (nameEB, EM_SETLIMITTEXT, (WPARAM)Str63Len, 0L);
	if (myfind.i_mode==1)
	{
        RECT wrect;
        GetWindowRect (myfind.hwnd_This, &wrect);
        myfind.hi = wrect.bottom - wrect.top;
		CheckDlgButton (myfind.hwnd_This, IDCANCEL, TRUE);
	}
	if (sdata_jumlahdata(&brdptr->skomp)>0)
		AddComboBoxItem (objCB, objnama[find_Device], find_Device);

	if (sdata_jumlahdata(&brdptr->snode)>0)
	{
		AddComboBoxItem (objCB, objnama[find_Signal], find_Signal);
		if (strstr(asal, "genrad228x")!=NULL)
		{
			AddComboBoxItem (objCB, objnama[find_Nail], find_Nail);
			AddComboBoxItem (objCB, objnama[find_Node], find_Node);
		}
		else
		{
			if (strstr(asal, "agilent3070fix")!=NULL)
				AddComboBoxItem (objCB, objnama[find_Probe], find_Probe);
		}
	}


	SelectComboBoxString (objCB, (LPSTR)(objnama[myfind.i_objectType]));
	if (myfind.i_mode==0)
		FindFillListView ();
    FindSetNameText();
}

LRESULT CALLBACK FindDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND objCB = GetDlgItem (hDlg, IDC_OBJECT);
	HWND nameLB = GetDlgItem (hDlg, IDC_LISTTEXT);
	UINT objID;
	UINT ntCode;
	static BOOL edfromlist = FALSE;
	switch (message)
	{
		case WM_INITDIALOG:
			myfind.hwnd_This = hDlg;
			FindDlgInitDialog();
			edfromlist = FALSE;
			return TRUE;

		case WM_CLOSE:
			EndDialog(hDlg,0);
			DestroyWindow(hDlg);
			if (myfind.i_mode==kFindBarMode)
				MoveDrawWindow();
			break;

		case WM_COMMAND:
			objID = LOWORD(wParam); /* object id */
			ntCode = HIWORD(wParam); /* nofication code */
			if (objID == IDCANCEL)
				SendMessage (hDlg, WM_CLOSE, 0, 0L);
			else if (objID == IDOK)
				FindFindBtnClickHandle (); /* find button is clicked */
			else if (objID==IDC_OBJECT && ntCode==CBN_SELCHANGE)
			{
				/* object type is changed, update part list */
				int data;
				GetComboBoxCurSelItemData (objCB, &data);
				if (data!=myfind.i_objectType)
				{
					myfind.i_objectType = data;
					FindSetNameText();
					if (myfind.i_mode==kFindBoxMode)
						FindFillListView ();
				}
				else // maintain to focus at text input
				{
				    SetFocus (GetDlgItem (hDlg, IDC_NAME));
				}
			}
			else if (objID==IDC_LISTTEXT && (ntCode==LBN_DBLCLK || ntCode==LBN_SELCHANGE))
			{
				/* part list is clicked */
				char text[256];
				text[0] = 0;
				GetListBoxCurSelString (nameLB, text);
				if (text[0])
				{
					edfromlist = TRUE; /* prevent to reselect this list box */
					SetDlgItemText (hDlg, IDC_NAME, text);
					/* part list is double-clicked */
					if (ntCode==LBN_DBLCLK)
						SendMessage (hDlg, WM_COMMAND, IDOK, 0);
				}
			}
			else if (objID == IDC_NAME)
			{
                char text[256];
				text[0] = 0;
				if (ntCode == EN_CHANGE && myfind.i_mode==kFindBoxMode)
				{
					GetDlgItemText (hDlg, IDC_NAME, text, 255);
					if (!edfromlist)
                        SelectListBoxString (nameLB, text);
                    edfromlist = FALSE;
				}
				else if (ntCode == EN_CHANGE && myfind.i_mode==kFindBarMode)
				{
					GetDlgItemText (hDlg, IDC_NAME, text, 255);
					if (strcmp(text,"@!")==0)
                        SendMessage (hDlg, WM_CLOSE, 0, 0L);
                    else
                    {
                        if (BV2Option_IsEnableFindBarEndBySpace() && strchr(text,' ')==NULL)
                            break;
                        SendMessage (hDlg, WM_COMMAND, (WPARAM)IDOK, 0L);
                    }
				}
			}
			//else
            //    SendMessage (GetParent(hDlg), message, wParam, lParam);

			break;
	}
    return FALSE;
}

void FindShowDialog (HWND hWndParent, int findType)
{
	if (myfind.hwnd_This && IsWindowVisible(myfind.hwnd_This))
	{
		if (myfind.i_mode==kFindBarMode) // && myfind.i_objectType != findType)
		{
            SelectComboBoxString (GetDlgItem(myfind.hwnd_This, IDC_OBJECT), (LPSTR)(objnama[findType]));
            SendMessage (myfind.hwnd_This, WM_COMMAND, (WPARAM)MAKELONG(IDC_OBJECT,CBN_SELCHANGE), 0L);
		}
		return;
	}

	if (IsWindow(myfind.hwnd_This))
	{
		myfind.i_objectType = findType;
		if (myfind.i_mode==kFindBarMode)
			MoveFindBar(); /* place me */
		ShowWindow (myfind.hwnd_This, SW_SHOW);
		if (myfind.i_mode==kFindBarMode)
			MoveDrawWindow(); /* resize draw wnd */
	}
	else
	{
		if (findType>=find_Device)
			myfind.i_objectType = findType;

		if (myfind.i_mode==kFindBoxMode)
		{
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_FIND), hWndParent, (DLGPROC)FindDlg);
		}
		else if (myfind.i_mode==kFindBarMode)
		{
			myfind.hwnd_This = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_FINDBAR), hWndParent, (DLGPROC)FindDlg);
			if (myfind.hwnd_This)
			{
				MoveFindBar();
				//ShowWindow (myfind.hwnd_This, SW_SHOW); /* 3 Okt 2006 - WS_VISIBLE di rc */
				MoveDrawWindow();
			}
		}
	}
}

// --------------------------------------------------- CONNECTION
// 2.0.5
int FindConnection (HWND hWnd, char *nama)
{
	boardptr bptr = GetBVBoardPtr();
	int i;
	int nodeid = -1;

	i = koneksi_cari_dkonek (&bptr->skonek, nama, &nodeid, -1);
	if (i>=0)
	{
		BV2Conn_SetSignal (nodeid);
		DrawFindSignalHandler (&bptr->snode);
		return 1;
	}
	else
	{
		char msg[256];
		sprintf (msg, "%s is not found.", nama);
		MessageBox (hWnd, msg, "Error", MB_OK | MB_ICONERROR);
	}
	return 0;
}

LRESULT CALLBACK BV2FindConnectionDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD dlgItemID;
	WORD notifID;

	switch (message)
	{
		case WM_INITDIALOG:
            return TRUE;

		case WM_COMMAND:
			dlgItemID = LOWORD(wParam);
			notifID = HIWORD(wParam);
			if (dlgItemID == IDOK)
			{
				char text[256];
				memset(text,0,256);
				GetDlgItemText (hDlg, IDC_NAME, text, 255);
				if (FindConnection(hDlg, text))
				{
                    EndDialog(hDlg, IDOK);
				}
				return FALSE;
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

UINT BV2FindConnectionDlg (HINSTANCE hInst, HWND hWnd)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_FINDBOX), hWnd, (DLGPROC)BV2FindConnectionDlgProc);
}


