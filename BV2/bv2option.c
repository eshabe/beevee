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

#include "bv2option.h"
#include "resource.h"
#include "../bv2data/sboard.h"

void WriteBV2Settings ();
BOOL LoadBV2Settings ();

typedef struct option_struct {
	BOOL b_findZoom;
	BOOL b_findBlink;
	BOOL b_findLine;
	BOOL b_colorClear;
	BOOL b_showObjectInfo;
	BOOL b_enableOIBar;
	BOOL b_enableFindBar;
	BOOL b_enableTextToolBar; // 2.0.4
	BOOL b_enableFindBarEndSpace; // 2.0.4

	COLORREF crefBackColor;
	COLORREF crefDeviceTColor;
	COLORREF crefDeviceBColor;
	COLORREF crefTestpinTColor;
	COLORREF crefTestpinBColor; /* 5 */
	COLORREF crefSelectedColor;
	COLORREF crefSelectedBkColor;
	COLORREF crefTraceColor;
	COLORREF crefTraceTColor;
	COLORREF crefTraceBColor;
	COLORREF crefTraceLockColor;
	COLORREF crefLineColor; /* 10 */
	COLORREF crefViaColor;

	HBRUSH hbrBackColor;
	HBRUSH hbrDeviceTColor;
	HBRUSH hbrDeviceBColor;
	HBRUSH hbrTestpinTColor;
	HBRUSH hbrTestpinBColor;
	HBRUSH hbrSelectedColor;
	HBRUSH hbrSelectedBkColor;
	HBRUSH hbrTraceColor;
	HBRUSH hbrTraceTColor;
	HBRUSH hbrTraceBColor;
	HBRUSH hbrTraceLockColor;
	HBRUSH hbrLineColor;
	HBRUSH hbrViaColor;

} option_struct;
static option_struct myoption;

void BV2Option_DefaultColors ()
{
	myoption.crefBackColor = RGB(0,0,0);
	myoption.crefDeviceBColor = RGB(0,0,255);
	myoption.crefDeviceTColor = RGB(0,255,0);
	myoption.crefLineColor = RGB(255,255,255);
	myoption.crefSelectedBkColor = RGB(255, 255,255);
	myoption.crefSelectedColor = RGB(255, 255, 0);
	myoption.crefTestpinBColor = RGB(255, 0, 0);
	myoption.crefTestpinTColor = RGB(255, 0, 255);
	myoption.crefTraceColor = myoption.crefTraceTColor = myoption.crefTraceBColor = RGB(0,255,255);
	myoption.crefTraceLockColor = RGB(255,255,255);
	myoption.crefViaColor = RGB(255,255,0);
}

void BV2OptionDefault ()
{
	if (!LoadBV2Settings())
	{
		myoption.b_colorClear = TRUE;
		myoption.b_findBlink = TRUE;
		myoption.b_findLine = TRUE;
		myoption.b_findZoom = TRUE;
		myoption.b_showObjectInfo = TRUE;
		myoption.b_enableFindBar = TRUE;
		myoption.b_enableOIBar = TRUE;

		BV2Option_DefaultColors ();
		WriteBV2Settings (); /* 3 Okt 2006 */
	}
}

BOOL BV2Option_IsZoom ()
{
	return myoption.b_findZoom;
}

BOOL BV2Option_IsBlink ()
{
	return myoption.b_findBlink;
}

BOOL BV2Option_IsLine ()
{
	return myoption.b_findLine;
}

BOOL BV2Option_IsClear ()
{
	return myoption.b_colorClear;
}

BOOL BV2Option_IsShowObjectInfo ()
{
	return myoption.b_showObjectInfo;
}

BOOL BV2Option_IsEnableObjectInfoBar ()
{
	return myoption.b_enableOIBar;
}

BOOL BV2Option_IsEnableFindBar ()
{
	return myoption.b_enableFindBar;
}

BOOL BV2Option_IsEnableFindBarEndBySpace ()
{
	return myoption.b_enableFindBarEndSpace;
}

BOOL BV2Option_IsShowTextOfToolBar ()
{
	return myoption.b_enableTextToolBar;
}



/* color */
COLORREF BV2Option_GetBackColorRef ()
{
	return myoption.crefBackColor;
}

COLORREF BV2Option_GetDeviceBColorRef ()
{
	return myoption.crefDeviceBColor;
}

COLORREF BV2Option_GetDeviceTColorRef ()
{
	return myoption.crefDeviceTColor;
}

COLORREF BV2Option_GetLineColorRef ()
{
	return myoption.crefLineColor;
}

COLORREF BV2Option_GetSelectedBkColorRef ()
{
	return myoption.crefSelectedBkColor;
}

COLORREF BV2Option_GetSelectedColorRef ()
{
	return myoption.crefSelectedColor;
}

COLORREF BV2Option_GetTestpinBColorRef ()
{
	return myoption.crefTestpinBColor;
}

COLORREF BV2Option_GetTestpinTColorRef ()
{
	return myoption.crefTestpinTColor;
}

COLORREF BV2Option_GetTraceColorRef (int layer)
{
	switch (layer)
	{
	    case SBOARD_TRACE_TOP:
	    return myoption.crefTraceTColor;
	    case SBOARD_TRACE_BOTTOM:
	    return myoption.crefTraceBColor;
	    case SBOARD_TRACE_INNER:
	    default:
        return myoption.crefTraceColor;
	}
}

COLORREF BV2Option_GetTraceLockColorRef ()
{
	return myoption.crefTraceLockColor;
}

COLORREF BV2Option_GetViaColorRef ()
{
	return myoption.crefViaColor;
}


/* end color */

BOOL BV2Option_ChooseColor (HWND hWnd, COLORREF *mycolor)
{
	CHOOSECOLOR cc;
	static COLORREF custClrs[16];

	ZeroMemory (&cc, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.rgbResult = *mycolor;
	cc.lpCustColors = (LPDWORD) custClrs;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;
	if (ChooseColor(&cc)==TRUE)
	{
		*mycolor = cc.rgbResult;
		return TRUE;
	}
	return FALSE;
}

void BV2Option_CreateBrushes ()
{
	myoption.hbrBackColor = CreateSolidBrush(myoption.crefBackColor);
	myoption.hbrDeviceTColor = CreateSolidBrush(myoption.crefDeviceTColor);
	myoption.hbrDeviceBColor = CreateSolidBrush(myoption.crefDeviceBColor);
	myoption.hbrTestpinTColor = CreateSolidBrush(myoption.crefTestpinTColor);
	myoption.hbrTestpinBColor = CreateSolidBrush(myoption.crefTestpinBColor);
	myoption.hbrSelectedColor = CreateSolidBrush(myoption.crefSelectedColor);
	myoption.hbrSelectedBkColor = CreateSolidBrush(myoption.crefSelectedBkColor);
	myoption.hbrTraceColor = CreateSolidBrush(myoption.crefTraceColor);
	myoption.hbrTraceTColor = CreateSolidBrush(myoption.crefTraceTColor);
	myoption.hbrTraceBColor = CreateSolidBrush(myoption.crefTraceBColor);
	myoption.hbrTraceLockColor = CreateSolidBrush(myoption.crefTraceLockColor);
	myoption.hbrViaColor = CreateSolidBrush(myoption.crefViaColor);
	myoption.hbrLineColor = CreateSolidBrush(myoption.crefLineColor);
}

void BV2Option_DeleteBrushes ()
{
	DeleteObject(myoption.hbrBackColor);
	DeleteObject(myoption.hbrDeviceTColor);
	DeleteObject(myoption.hbrDeviceBColor);
	DeleteObject(myoption.hbrTestpinTColor);
	DeleteObject(myoption.hbrTestpinBColor);
	DeleteObject(myoption.hbrSelectedColor);
	DeleteObject(myoption.hbrSelectedBkColor);
	DeleteObject(myoption.hbrTraceColor);
	DeleteObject(myoption.hbrTraceTColor);
	DeleteObject(myoption.hbrTraceBColor);
	DeleteObject(myoption.hbrTraceLockColor);
	DeleteObject(myoption.hbrLineColor);
	DeleteObject(myoption.hbrViaColor);
}

LRESULT CALLBACK BV2OptionDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int dlgItemID;
	BOOL clrChg=FALSE;

	switch (message)
	{
		case WM_INITDIALOG:
			CheckDlgButton(hDlg, IDC_ZOOM, myoption.b_findZoom);
			CheckDlgButton(hDlg, IDC_BLINK, myoption.b_findBlink);
			CheckDlgButton(hDlg, IDC_LINE, myoption.b_findLine);
			CheckDlgButton(hDlg, IDC_CLEAR, myoption.b_colorClear);
			CheckDlgButton(hDlg, IDC_OBJECT, myoption.b_showObjectInfo);
			CheckDlgButton(hDlg, IDC_ENAOBJECT, myoption.b_enableOIBar);
			CheckDlgButton(hDlg, IDC_ENAFIND, myoption.b_enableFindBar);
			CheckDlgButton(hDlg, IDC_ENAFBSPACE, myoption.b_enableFindBarEndSpace);
            CheckDlgButton(hDlg, IDC_TOOLBARTEXT, myoption.b_enableTextToolBar);
			if (myoption.hbrBackColor)
				BV2Option_DeleteBrushes();
			BV2Option_CreateBrushes();
			return TRUE;

		/*
		case WM_CTLCOLORBTN:
			return (LRESULT) myoption.hbrDeviceTColor;
		*/
		case WM_CTLCOLORSTATIC:
			dlgItemID = GetDlgCtrlID((HWND)lParam);
			switch (dlgItemID)
			{
			case IDC_TDEV_:
				return (LRESULT) myoption.hbrDeviceTColor;
			case IDC_BDEV_:
				return (LRESULT) myoption.hbrDeviceBColor;
			case IDC_TTP_:
				return (LRESULT) myoption.hbrTestpinTColor;
			case IDC_BTP_:
				return (LRESULT) myoption.hbrTestpinBColor;
			case IDC_SEL_:
				return (LRESULT) myoption.hbrSelectedColor;
			case IDC_SELB_:
				return (LRESULT) myoption.hbrSelectedBkColor;
			case IDC_BACK_:
				return (LRESULT) myoption.hbrBackColor;
			case IDC_TRACE_:
				return (LRESULT) myoption.hbrTraceColor; /* INNER, default */
			case IDC_TRACET_:
				return (LRESULT) myoption.hbrTraceTColor; /* TOP */
			case IDC_TRACEB_:
				return (LRESULT) myoption.hbrTraceBColor; /* BOTTOM */
			case IDC_TRACELK_:
				return (LRESULT) myoption.hbrTraceLockColor;
			case IDC_VIA_:
				return (LRESULT) myoption.hbrViaColor;
			case IDC_CLINE_:
				return (LRESULT) myoption.hbrLineColor;
			}
			break;

		case WM_COMMAND:
			dlgItemID = LOWORD(wParam);
			switch (dlgItemID)
			{
			case IDOK:
				myoption.b_colorClear = IsDlgButtonChecked (hDlg, IDC_CLEAR);
				myoption.b_findZoom = IsDlgButtonChecked (hDlg, IDC_ZOOM);
				myoption.b_findBlink = IsDlgButtonChecked (hDlg, IDC_BLINK);
				myoption.b_findLine = IsDlgButtonChecked (hDlg, IDC_LINE);
				myoption.b_showObjectInfo = IsDlgButtonChecked (hDlg, IDC_OBJECT);
				myoption.b_enableOIBar = IsDlgButtonChecked (hDlg, IDC_ENAOBJECT);
				myoption.b_enableFindBar = IsDlgButtonChecked (hDlg, IDC_ENAFIND);
				myoption.b_enableFindBarEndSpace = IsDlgButtonChecked (hDlg, IDC_ENAFBSPACE);
				myoption.b_enableTextToolBar = IsDlgButtonChecked (hDlg, IDC_TOOLBARTEXT);

				WriteBV2Settings ();

			case IDCANCEL:
				EndDialog (hDlg, dlgItemID);
				return TRUE;
			case IDC_TDEV:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefDeviceTColor);
				break;
			case IDC_BDEV:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefDeviceBColor);
				break;
			case IDC_TTP:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefTestpinTColor);
				break;
			case IDC_BTP:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefTestpinBColor);
				break;
			case IDC_SEL:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefSelectedColor);
				break;
			case IDC_SELB:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefSelectedBkColor);
				break;
			case IDC_BACK:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefBackColor);
				break;
			case IDC_TRACE:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefTraceColor);
				break;
			case IDC_TRACET:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefTraceTColor);
				break;
			case IDC_TRACEB:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefTraceBColor);
				break;
			case IDC_TRACELK:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefTraceLockColor);
				break;
			case IDC_VIA:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefViaColor);
				break;
			case IDC_CLINE:
				clrChg=BV2Option_ChooseColor (hDlg, &myoption.crefLineColor);
				break;
			case IDC_DEFAULT:
				BV2Option_DefaultColors ();
				clrChg = TRUE;
				break;

			}
			if (clrChg)
			{
				//HWND ctrl = GetDlgItem (hDlg, IDC_COLOR);
				//RECT r;
				//GetWindowRect(ctrl, &r);
				BV2Option_DeleteBrushes	();
				BV2Option_CreateBrushes();
				InvalidateRect (hDlg, NULL, TRUE);
			}
			break;

		case WM_DESTROY:
			BV2Option_DeleteBrushes	();
			break;
	}
    return FALSE;
}


int BV2OptionDlg (HINSTANCE hInst, HWND hWnd)
{
	return DialogBox(hInst, MAKEINTRESOURCE(IDD_OPTION), hWnd, (DLGPROC)BV2OptionDlgProc);
}


/* 3 Okt 2006 - tambah dOptSet ("opt") u/ memastikan data sudah tersimpan */

BOOL LoadBV2Settings()
{
	HKEY hkey;
	DWORD type;
	DWORD size;
	DWORD dOptSet=0;

	if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee", 0, KEY_READ, &hkey)==ERROR_SUCCESS)
	{
		type = REG_DWORD;
		size = sizeof(DWORD);

		RegQueryValueEx (hkey, "opt", 0, &type, (LPBYTE)&dOptSet, &size);
		if (dOptSet)
		{
            RegQueryValueEx (hkey, "findZoom", 0, &type, (LPBYTE)&myoption.b_findZoom, &size);
            RegQueryValueEx (hkey, "findBlink", 0, &type, (LPBYTE)&myoption.b_findBlink, &size);
            RegQueryValueEx (hkey, "findLine", 0, &type, (LPBYTE)&myoption.b_findLine, &size);
            RegQueryValueEx (hkey, "backClear", 0, &type, (LPBYTE)&myoption.b_colorClear, &size);
            RegQueryValueEx (hkey, "showOI", 0, &type, (LPBYTE)&myoption.b_showObjectInfo, &size);
            RegQueryValueEx (hkey, "enableOIBar", 0, &type, (LPBYTE)&myoption.b_enableOIBar, &size);
            RegQueryValueEx (hkey, "enableFindBar", 0, &type, (LPBYTE)&myoption.b_enableFindBar, &size);
            RegQueryValueEx (hkey, "enableFindBarSpace", 0, &type, (LPBYTE)&myoption.b_enableFindBarEndSpace, &size);
            RegQueryValueEx (hkey, "showTBText", 0, &type, (LPBYTE)&myoption.b_enableTextToolBar, &size);

            RegQueryValueEx (hkey, "colorBackground", 0, &type, (LPBYTE)&myoption.crefBackColor, &size);
            RegQueryValueEx (hkey, "colorDeviceTop", 0, &type, (LPBYTE)&myoption.crefDeviceTColor, &size);
            RegQueryValueEx (hkey, "colorDeviceBot", 0, &type, (LPBYTE)&myoption.crefDeviceBColor, &size);
            RegQueryValueEx (hkey, "colorTPTop", 0, &type, (LPBYTE)&myoption.crefTestpinTColor, &size);
            RegQueryValueEx (hkey, "colorTPBot", 0, &type, (LPBYTE)&myoption.crefTestpinBColor, &size);
            RegQueryValueEx (hkey, "colorSelected", 0, &type, (LPBYTE)&myoption.crefSelectedColor, &size);
            RegQueryValueEx (hkey, "colorSelectedBk", 0, &type, (LPBYTE)&myoption.crefSelectedBkColor, &size);
            RegQueryValueEx (hkey, "colorTrace", 0, &type, (LPBYTE)&myoption.crefTraceColor, &size);
            RegQueryValueEx (hkey, "colorTraceTop", 0, &type, (LPBYTE)&myoption.crefTraceTColor, &size);
            RegQueryValueEx (hkey, "colorTraceBottom", 0, &type, (LPBYTE)&myoption.crefTraceBColor, &size);
            RegQueryValueEx (hkey, "colorTraceLock", 0, &type, (LPBYTE)&myoption.crefTraceLockColor, &size);
            RegQueryValueEx (hkey, "colorVia", 0, &type, (LPBYTE)&myoption.crefViaColor, &size);
            RegQueryValueEx (hkey, "colorLine", 0, &type, (LPBYTE)&myoption.crefLineColor, &size);
		}
		RegCloseKey (hkey);

		return dOptSet;
	}

	return FALSE;
}

void WriteBV2Settings ()
{

	HKEY hkey;
	DWORD dOptSet = 1;

	RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);

	RegSetValueEx (hkey, "opt", 0, REG_DWORD, (LPBYTE)&dOptSet, sizeof(DWORD));
	RegSetValueEx (hkey, "findZoom", 0, REG_DWORD, (LPBYTE)&myoption.b_findZoom, sizeof(DWORD));
	RegSetValueEx (hkey, "findBlink", 0, REG_DWORD, (LPBYTE)&myoption.b_findBlink, sizeof(DWORD));
	RegSetValueEx (hkey, "findLine", 0, REG_DWORD, (LPBYTE)&myoption.b_findLine, sizeof(DWORD));
	RegSetValueEx (hkey, "backClear", 0, REG_DWORD, (LPBYTE)&myoption.b_colorClear, sizeof(DWORD));
	RegSetValueEx (hkey, "showOI", 0, REG_DWORD, (LPBYTE)&myoption.b_showObjectInfo, sizeof(DWORD));
	RegSetValueEx (hkey, "enableOIBar", 0, REG_DWORD, (LPBYTE)&myoption.b_enableOIBar, sizeof(DWORD));
	RegSetValueEx (hkey, "enableFindBar", 0, REG_DWORD, (LPBYTE)&myoption.b_enableFindBar, sizeof(DWORD));
	RegSetValueEx (hkey, "enableFindBarSpace", 0, REG_DWORD, (LPBYTE)&myoption.b_enableFindBarEndSpace, sizeof(DWORD));
	RegSetValueEx (hkey, "showTBText", 0, REG_DWORD, (LPBYTE)&myoption.b_enableTextToolBar, sizeof(DWORD));

	RegSetValueEx (hkey, "colorBackground", 0, REG_DWORD, (LPBYTE)&myoption.crefBackColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorDeviceTop", 0, REG_DWORD, (LPBYTE)&myoption.crefDeviceTColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorDeviceBot", 0, REG_DWORD, (LPBYTE)&myoption.crefDeviceBColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorTPTop", 0, REG_DWORD, (LPBYTE)&myoption.crefTestpinTColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorTPBot", 0, REG_DWORD, (LPBYTE)&myoption.crefTestpinBColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorSelected", 0, REG_DWORD, (LPBYTE)&myoption.crefSelectedColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorSelectedBk", 0, REG_DWORD, (LPBYTE)&myoption.crefSelectedBkColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorTrace", 0, REG_DWORD, (LPBYTE)&myoption.crefTraceColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorTraceTop", 0, REG_DWORD, (LPBYTE)&myoption.crefTraceTColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorTraceBottom", 0, REG_DWORD, (LPBYTE)&myoption.crefTraceBColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorTraceLock", 0, REG_DWORD, (LPBYTE)&myoption.crefTraceLockColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorVia", 0, REG_DWORD, (LPBYTE)&myoption.crefViaColor, sizeof(DWORD));
	RegSetValueEx (hkey, "colorLine", 0, REG_DWORD, (LPBYTE)&myoption.crefLineColor, sizeof(DWORD));

	RegCloseKey (hkey);

}


