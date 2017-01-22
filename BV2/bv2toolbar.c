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

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#include "bv2toolbar.h"
#include "resource.h"
#include <commctrl.h>

typedef struct toolbar_struct {
	HWND hwnd_This;
	//int height;
	int bottom;
} toolbar_struct;

static toolbar_struct mytoolbar;

#define NUMIMAGES 20
#define IMAGEWIDTH 16
#define IMAGEHEIGHT 15
#define BUTTONWIDTH 16
#define BUTTONHEIGHT 16

#define NUMBUTTON   23


void InitToolBarStruct ()
{
    memset (&mytoolbar, 0, sizeof(mytoolbar));
}

HWND CreateToolBar (HINSTANCE hInst, HWND hWndParent)
{
    DWORD dwStyle;              // Style of the toolbar
    HWND hwndTB = NULL;         // Handle to the command bar control
    HWND hwndTT;
    RECT rect;                  // Contains the coordinates of the main
                               // window client area
   //RECT rectTB;                // Contains the dimensions of the bounding
                               // rectangle of the toolbar control
   INITCOMMONCONTROLSEX iccex; // The INITCOMMONCONTROLSEX structure

    iccex.dwSize = sizeof (INITCOMMONCONTROLSEX);
    iccex.dwICC = ICC_BAR_CLASSES;

	// butuh comctl32.lib (vc98) / libcomctl32.lib (devc++)

	// Register toolbar control classes from the DLL for the common
	// control.

	InitCommonControlsEx (&iccex);

	// Create the toolbar control.
	dwStyle = WS_VISIBLE | WS_CHILD | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | //TBSTYLE_WRAPABLE |
				  CCS_TOP; // | CCS_NOPARENTALIGN;


	TBBUTTON tbButton[NUMBUTTON];
    char *szStrings = "Close\0Open\0Import\0Save\0-\0Zoom In\0Zoom Out\01:1\0Top Dev\0Btm Dev\0Top Pin\0Btm Pin\0Top Trace\0Btm Trace\0Trace\0Conn Guide\0-\0Find\0Connection\0Info\0Option\0-\0About\0";
	int tbCommands[NUMBUTTON][2] = {
		{ID_FILE_CLOSE,0},
		{ID_FILE_OPEN, 1},
		{ID_FILE_IMPORT, 2},
		{ID_FILE_SAVE, 3},
		{0, 0}, //6
		{ID_VIEW_ZOOMIN, 4},
		{ID_VIEW_ZOOMOUT, 5},
		{ID_VIEW_ACTUALSIZE,19},
		{ID_VIEW_TOPDEVICES, 6},
		{ID_VIEW_BOTTOMDEVICES, 7},
		{ID_VIEW_TOPTESTPINS, 8},
		{ID_VIEW_BOTTOMTESTPINS, 9},
		{ID_VIEW_TRACESTOP, 16},
		{ID_VIEW_TRACESBOT, 17},
		{ID_VIEW_TRACES, 10},
		{ID_VIEW_CONNGUIDE, 18},
		{0, 0},
		{ID_DOFIND, 11},
		{ID_TOOLS_CONNECTION, 12},
		{ID_TOOL_OBJECTINFO, 13},
		{ID_TOOLS_OPTIONS, 15},
		{0, 0},
		{IDM_ABOUT, 14}
	};
	int tbCount = NUMBUTTON;
	int i;

	memset (tbButton, 0, sizeof(tbButton));
	for (i=0;i<tbCount;i++)
	{
		tbButton[i].iString = i;
		if (tbCommands[i][0])
		{
			tbButton[i].iBitmap = tbCommands[i][1];
			tbButton[i].idCommand = tbCommands[i][0];
			switch (tbButton[i].idCommand)
			{
			case ID_FILE_OPEN:
			case ID_FILE_IMPORT:
			case ID_VIEW_TOPDEVICES:
			case ID_VIEW_BOTTOMDEVICES:
			case ID_VIEW_ACTUALSIZE:
			case ID_VIEW_BOTTOMTESTPINS:
			case ID_VIEW_TRACES:
			case ID_VIEW_TRACESTOP:
			case ID_VIEW_TRACESBOT:
			case ID_VIEW_CONNGUIDE:
			case ID_TOOL_OBJECTINFO:
			case ID_TOOLS_CONNECTION:
			case IDM_ABOUT:
			case ID_TOOLS_OPTIONS:
				tbButton[i].fsState = TBSTATE_ENABLED;
				break;
			default:
				tbButton[i].fsState = 0;
			}
		}
		else
		{
			tbButton[i].iBitmap = 0;
			tbButton[i].idCommand = 0;
			tbButton[i].fsStyle = TBSTYLE_SEP;
		}
	}


	if (!(hwndTB = CreateToolbarEx (
		hWndParent,          // Parent window handle
		dwStyle,            // Toolbar window styles
		(UINT) ID_TOOLBAR,  // Toolbar control identifier
		NUMIMAGES,          // Number of button images
		hInst,					// Module instance
		IDB_TOOLBAR,        // Bitmap resource identifier
		tbButton,           // Array of TBBUTTON structure
								  // contains button data
		sizeof (tbButton) / sizeof (TBBUTTON),
								  // Number of buttons in toolbar
		BUTTONWIDTH,        // Width of the button in pixels
		BUTTONHEIGHT,       // Height of the button in pixels
		IMAGEWIDTH,         // Button image width in pixels
		IMAGEHEIGHT,        // Button image height in pixels
		sizeof (TBBUTTON))))// Size of a TBBUTTON structure
	{
      return NULL;
	}

	// Add ToolTips to the toolbar.
	//SendMessage (GetParent(hwndTB), TB_SETTOOLTIPS,(WPARAM) 18, //NUMIMAGES,
	//		(LPARAM) szToolTips);

	/*
	hwndTT = (HWND)SendMessage(hwndTB, TB_GETTOOLTIPS, (WPARAM)0, 0L);
	if (hwndTT)
	{
	    SetWindowText (hwndTT, szToolTips);
	    SendMessage (hwndTB, TB_SETTOOLTIPS, 18, //NUMIMAGES,
			(LPARAM) szToolTips);
	}
	*/


	// Reposition the toolbar.
	mytoolbar.hwnd_This = hwndTB;
	//mytoolbar.bottom = 22;
	//mytoolbar.height = mytoolbar.bottom;
	//GetWindowRect (hwndTB, &rect);
	//mytoolbar.height = rect.bottom - rect.top;
	//mytoolbar.bottom = mytoolbar.height;

  	if (BV2Option_IsShowTextOfToolBar ())
    {
        SendMessage (mytoolbar.hwnd_This, TB_ADDSTRING, (WPARAM)0, (LPARAM)szStrings);
        //mytoolbar.height += 12;
        //mytoolbar.bottom += 20;
    }
	GetWindowRect (hwndTB, &rect);
	mytoolbar.bottom = rect.bottom - rect.top;

	MoveWindow (mytoolbar.hwnd_This,
		rect.left,
		rect.top,
		rect.right,
		mytoolbar.bottom,
		FALSE);

	return hwndTB;
}

void ToolBarNotifyHandler (LPARAM lParam)
{
    LPNMTTDISPINFO lpntt = (LPNMTTDISPINFO)lParam;
    UINT cmdid = lpntt->hdr.idFrom;
    char szText[100];
    char *ttext;

    LoadString (GetModuleHandle(0), cmdid, szText, 100);
    if ( (ttext=strchr(szText,'\n'))!=NULL)
    {
        ++ttext;
        lpntt->lpszText =ttext;
    }
    //strtok(szText,"\n");
    //SetStatusBarText (szText);

}

void SetBtnToolBarState (UINT tbId, UINT state)
{
	SendMessage (mytoolbar.hwnd_This, TB_SETSTATE, (WPARAM)tbId, (LPARAM)state);
}

void EnableToolBarButton (UINT tbId, BOOL enable)
{
	UINT state = SendMessage (mytoolbar.hwnd_This, TB_GETSTATE, (WPARAM)tbId, 0L);
	if (enable)
		state |= TBSTATE_ENABLED;
	else
		state &= ~TBSTATE_ENABLED;
	SetBtnToolBarState (tbId, state);
}


void CheckToolBarButton (UINT tbId, BOOL check)
{
	UINT state = SendMessage (mytoolbar.hwnd_This, TB_GETSTATE, (WPARAM)tbId, 0L);
	if (check)
		state |= TBSTATE_CHECKED;
	else
		state &= ~TBSTATE_CHECKED;

	SetBtnToolBarState (tbId, state);
}

HWND GetToolBarWindow ()
{
	return mytoolbar.hwnd_This;
}

BOOL IsToolBarVisible ()
{
	return IsWindowVisible (mytoolbar.hwnd_This);
}

/*
int GetToolBarHeight ()
{
	if (IsToolBarVisible())
		return mytoolbar.height;
	return 0;
}
*/

int GetToolBarBottom ()
{
	if (IsToolBarVisible())
		return mytoolbar.bottom;
	return 0;
}

BOOL MoveToolBar ()
{
	RECT r;
	RECT rect;
	if (!IsToolBarVisible())
		return FALSE;
	GetClientRect (GetParent(mytoolbar.hwnd_This), &r);
	//mytoolbar.bottom += r.top;
	GetWindowRect (mytoolbar.hwnd_This, &rect);
	mytoolbar.bottom = rect.bottom - rect.top;

	return MoveWindow (mytoolbar.hwnd_This,
		r.left,
		r.top,
		r.right,
		mytoolbar.bottom,
		TRUE);
}
