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
catatan:

	SetWindowExtEx(hdc, dx, -dy, NULL);
	SetViewportExtEx(hdc, GetDeviceCaps(hdc, LOGPIXELSX),
	                      GetDeviceCaps(hdc, LOGPIXELSY), NULL);

	vx = GetDeviceCaps(hdc, LOGPIXELSX)

- agar 1 nilai digambar sbg satu inchi, dx (dan dy) = 1
- penggambaran BeeVee adalah dalam satuan mils (1/1000 inch)
- dg SetViewportExtEx
-> 1 (inch) = pt/vx
- dg SetWindowExtEx
-> nilai/1000 = pt/vx
-> pt = nilai*vx/1000

g_VSPos MENGURANGI....
g_HSPos menambah
*/

/*
shb-20070911
+ void DrawMoveAccessories (int dx, int dy)

shb-20070903
Perbaiki penunjukkan testpin
Aktipkan connection guide pada testpin
Tambah toleransi select by click (5 mils, bukan 5 pt!)
*/

#include "bv2winmain.h"

#include "BV2.h"
#include "bv2toolbar.h"
#include "bv2draw.h"
#include "bv2object.h"
#include "statusbar.h"
#include "bv2find.h"
#include "bv2option.h"
#include "bv2connect.h"

#include <math.h>
#include <stdio.h>

typedef struct draw_struct {
	HWND hwnd_This;
	BOOL b_Draw;
	BOOL b_DrawHilitedOnly;
	BOOL b_ShowTrace; /* All, 2.1 */
	BOOL b_STopDev;
	BOOL b_SBotDev;
	BOOL b_STopPin;
	BOOL b_SBotPin;
	BOOL b_STopTrace; /*2.1*/
	BOOL b_SBotTrace; /*2.1*/
	BOOL b_SInTrace; /*2.1*/
	BOOL b_SConGuide; /*2.1*/
	BOOL b_selectfind;
	BOOL b_blinknow;
	BOOL b_drag;
	RECT drawrect;
	RECT drawrect0;
	BOOL b_goto;
} draw_struct;

static draw_struct mydraw;

extern BOOL g_dataBagus;

#define NUMSCALEINDEX 16
#define SCALENUMBER_REAL	7
#define SCALENUMBER_ZOOM   11
double g_ScaleIndex[NUMSCALEINDEX] = { 0.25, 0.35, 0.4, 0.5, 0.6, 0.75, 0.85, 1.0, 1.25, 1.50, 2.0, 2.5, 3.0, 5.0, 7.5, 10 };
int	g_ScaleNumber;

#define PTSCROLLMOVE 200
#define BLINK_TIMERMS 500
#define BLINK_TIMERID 1002

int		g_VSPos;
int		g_HSPos;
int		g_DevCapX, g_DevCapY;
komponen g_komphilite;
komponen g_kompunhili;
_sdata	g_sdHLiteDev;
_sdata	g_sdHLiteTP;
_sdata	g_sdHLiteTPOld;
_sdata	g_sdHLiteTrace;
_sdata	g_sdLockTrace;
int g_point2selectedpin;

/* warna-warni hi....! */
HBRUSH	hbNormalBkColor;
HBRUSH	hbBackColor;
HPEN		hpDeviceTColor;
HPEN		hpDeviceBColor;
HPEN		hpTestpinTColor;
HPEN		hpTestpinBColor;
HPEN		hpSelectedColor;
HBRUSH	hbSelectedBkColor;
HPEN		hpTraceColor;
HPEN		hpTraceTColor;
HPEN		hpTraceBColor;
HPEN		hpLockedTraceColor;
HPEN		hpLineColor;
HPEN        hpViaColor;

LRESULT CALLBACK	DrawWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawDoPaint(HWND hWnd, HDC hdc);
void DrawUpdateScrollBars (HWND hWnd);
void DrawUpdateScrollBarsSIZE (HWND hWnd);
BOOL DrawDoScrollH (HWND hWnd, UINT cmd);
BOOL DrawDoScrollV (HWND hWnd, UINT cmd);
void DrawWnd2ViewXy (int x, int y, int *vx, int *vy);
void DrawClickHandle (HWND hWnd, int x, int y);

char szDrawWindowClassName[] = "BV2DrawWindow";

void DrawClearSelectionData ()
{
	memset (&g_komphilite,0, sizeof(komponen));
	g_point2selectedpin = 0;

	sdata_hapussemua (&g_sdHLiteDev);
	sdata_hapussemua (&g_sdHLiteTP);
	sdata_hapussemua (&g_sdHLiteTPOld);
	sdata_hapussemua (&g_sdHLiteTrace);
	sdata_hapussemua (&g_sdLockTrace);

	SetObjectInfo ("No object", "", "", "");
	SetStatusBar0Text ("");
}

void DrawClearSlection()
{
    DrawClearSelectionData ();
    EraseWindow(mydraw.hwnd_This);
}

void DrawSetDefault ()
{
	boardptr bptr = GetBVBoardPtr();

	g_ScaleNumber = SCALENUMBER_REAL;
	g_VSPos = -board_atas(bptr);
	g_HSPos = board_kiri(bptr);


	mydraw.b_Draw = FALSE;
	mydraw.b_selectfind = FALSE;
	mydraw.b_ShowTrace = TRUE;
	mydraw.b_SBotDev = TRUE;
	mydraw.b_SBotPin = TRUE;
	mydraw.b_STopDev = TRUE;
	mydraw.b_STopPin = TRUE;
	mydraw.b_STopTrace = TRUE;
	mydraw.b_SBotTrace = TRUE;
	mydraw.b_SInTrace = TRUE;
	mydraw.b_SConGuide = TRUE;
	mydraw.b_DrawHilitedOnly = FALSE;
	mydraw.b_blinknow = FALSE;
	SetRectEmpty (&mydraw.drawrect);
	mydraw.b_drag = FALSE;
	mydraw.b_goto = FALSE;

	DrawClearSelectionData ();
}

void DrawSetDefaultAndDraw ()
{
	DrawSetDefault();
	DrawInitScrollBars();
	EraseWindow(mydraw.hwnd_This);
	//if (mydraw.hwnd_This==NULL)
    //    MessageBox (NULL, "NULL", NULL, 0);
	//InvalidateRect (GetDrawWindow(), NULL, TRUE);
}

void DrawSetColors ()
{
	LOGBRUSH lbr;
	hbBackColor = CreateSolidBrush (BV2Option_GetBackColorRef());
	lbr.lbStyle = BS_HOLLOW;
	hbNormalBkColor = CreateBrushIndirect (&lbr);

	hpDeviceTColor = CreatePen(PS_SOLID,0, BV2Option_GetDeviceTColorRef());
	hpDeviceBColor = CreatePen(PS_SOLID,0, BV2Option_GetDeviceBColorRef());
	hpTestpinTColor = CreatePen(PS_SOLID,0, BV2Option_GetTestpinTColorRef());
	hpTestpinBColor = CreatePen(PS_SOLID,0, BV2Option_GetTestpinBColorRef());
	hpSelectedColor = CreatePen(PS_SOLID,0, BV2Option_GetSelectedColorRef());
	if (BV2Option_IsClear())
		hbSelectedBkColor = CreateBrushIndirect (&lbr);
	else
		hbSelectedBkColor = CreateSolidBrush(BV2Option_GetSelectedBkColorRef());

	hpTraceColor = CreatePen(PS_SOLID,0, BV2Option_GetTraceColorRef(SBOARD_TRACE_INNER));
	hpTraceTColor = CreatePen(PS_SOLID,0, BV2Option_GetTraceColorRef(SBOARD_TRACE_TOP));
	hpTraceBColor = CreatePen(PS_SOLID,0, BV2Option_GetTraceColorRef(SBOARD_TRACE_BOTTOM));
	hpLockedTraceColor = CreatePen(PS_SOLID,0, BV2Option_GetTraceLockColorRef());
	hpLineColor = CreatePen(PS_SOLID,0, BV2Option_GetLineColorRef());
	hpViaColor = CreatePen(PS_SOLID,0, BV2Option_GetViaColorRef());
}

void DrawDeleteColors ()
{
	//DeleteObject (hbBackColor);
	DeleteObject (hbNormalBkColor);
	DeleteObject (hpDeviceTColor);
	DeleteObject (hpDeviceBColor);
	DeleteObject (hpTestpinTColor);
	DeleteObject (hpTestpinBColor);
	DeleteObject (hpSelectedColor);
	DeleteObject (hbSelectedBkColor);
	DeleteObject (hpTraceColor);
	DeleteObject (hpTraceTColor);
	DeleteObject (hpTraceBColor);
	DeleteObject (hpLockedTraceColor);
	DeleteObject (hpLineColor);
    DeleteObject (hpViaColor);
}

void DrawResetColors ()
{
	DrawDeleteColors();
	DrawSetColors();
}


HWND CreateDrawWindow (HINSTANCE hInst, HWND hWndParent)
{
	HWND hWnd;
	WNDCLASSEX wcex;

	 /* init */
	DrawSetColors ();
	DrawSetDefault();

	g_DevCapX = g_DevCapY = -1;


	sdata_init (&g_sdHLiteDev);
	sdata_init (&g_sdHLiteTP);
	sdata_init (&g_sdHLiteTPOld);
	sdata_init (&g_sdHLiteTrace);
	sdata_init (&g_sdLockTrace);

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_DBLCLKS; //CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)DrawWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= GetModuleHandle(0);
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= hbBackColor;
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szDrawWindowClassName;
	wcex.hIconSm		= NULL;

	if (!RegisterClassEx(&wcex))
		return NULL;

   hWnd = CreateWindow(szDrawWindowClassName, NULL,
		WS_CHILD | WS_HSCROLL | WS_VSCROLL,
      CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, hWndParent, NULL, hInst, NULL);

	mydraw.hwnd_This = hWnd;

	if (hWnd)
		ShowWindow(hWnd, SW_SHOW);
	return hWnd;

}

HWND GetDrawWindow ()
{
	return mydraw.hwnd_This;
}

BOOL Draw_IsShowTrace ()
{
	return mydraw.b_ShowTrace;
}

BOOL Draw_IsShowTopTrace ()
{
	return mydraw.b_STopTrace;
}
BOOL Draw_IsShowBottomTrace ()
{
	return mydraw.b_SBotTrace;
}
BOOL Draw_IsShowInnerTrace ()
{
	return mydraw.b_SInTrace;
}

BOOL Draw_SetShowTrace (BOOL bShow)
{
	BOOL old = mydraw.b_ShowTrace;
	mydraw.b_ShowTrace = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_SetShowTopTrace (BOOL bShow)
{
	BOOL old = mydraw.b_STopTrace;
	mydraw.b_STopTrace = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_SetShowInnerTrace (BOOL bShow)
{
	BOOL old = mydraw.b_SInTrace;
	mydraw.b_SInTrace = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_SetShowBottomTrace (BOOL bShow)
{
	BOOL old = mydraw.b_SBotTrace;
	mydraw.b_SBotTrace = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_IsShowConnectionGuide ()
{
	return mydraw.b_SConGuide;
}

BOOL Draw_SetShowConnectionGuide (BOOL bShow)
{
	BOOL old = mydraw.b_SConGuide;
	mydraw.b_SConGuide = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_IsShowTopDevice ()
{
	return mydraw.b_STopDev;
}

BOOL Draw_SetShowTopDevice(BOOL bShow)
{
	BOOL old = mydraw.b_STopDev;
	mydraw.b_STopDev = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_IsShowBotDevice ()
{
	return mydraw.b_SBotDev;
}

BOOL Draw_SetShowBotDevice(BOOL bShow)
{
	BOOL old = mydraw.b_SBotDev;
	mydraw.b_SBotDev = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_IsShowTopTP ()
{
	return mydraw.b_STopPin;
}

BOOL Draw_SetShowTopTP (BOOL bShow)
{
	BOOL old = mydraw.b_STopPin;
	mydraw.b_STopPin = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

BOOL Draw_IsShowBotTP ()
{
	return mydraw.b_SBotPin;
}

BOOL Draw_SetShowBotTP (BOOL bShow)
{
	BOOL old = mydraw.b_SBotPin;
	mydraw.b_SBotPin = bShow;
	InvalidateRect (mydraw.hwnd_This, NULL, !bShow);
	return old;
}

void Draw_ShowDevicesOnly ()
{
    mydraw.b_SBotDev = TRUE;
    mydraw.b_STopDev = TRUE;
    mydraw.b_STopPin = FALSE;
    mydraw.b_SBotPin = FALSE;
    InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

void Draw_ShowTestpinsOnly ()
{
    mydraw.b_SBotDev = FALSE;
    mydraw.b_STopDev = FALSE;
    mydraw.b_STopPin = TRUE;
    mydraw.b_SBotPin = TRUE;
    InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

void Draw_ShowDTAll ()
{
    mydraw.b_SBotDev = TRUE;
    mydraw.b_STopDev = TRUE;
    mydraw.b_STopPin = TRUE;
    mydraw.b_SBotPin = TRUE;
    InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

void MoveDrawWindow ()
{
	RECT rect;
	GetClientRect (GetParent(mydraw.hwnd_This), &rect);
	rect.top += GetToolBarBottom();

	if (GetStatusBarWindow())
		rect.bottom -= GetStatusBarHeight();
	if (IsWindowVisible(GetObjectInfoWindow()) && GetObjectInfoMode()==kObjectBarMode)
		rect.bottom -= GetObjectInfoHeight();
	if (IsWindowVisible(GetFindDialog()) && GetFindMode()==1)
		rect.bottom -= GetFindBarHeight();
	MoveWindow (mydraw.hwnd_This,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		TRUE);
    InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

LRESULT CALLBACK DrawWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	static int iLBDx=0, iLBDy=0;
	static BOOL mymouse_down = FALSE;

	switch (message)
	{
		case WM_ERASEBKGND:
			{
				RECT cr;
				GetClientRect (hWnd, &cr);
				FillRect ( (HDC)wParam, &cr, hbBackColor); // "erase" client area
				mydraw.b_DrawHilitedOnly = FALSE; // ensure not to draw hlite only
			}
			mymouse_down = FALSE;
			return TRUE;
			//SetBkColor ((HDC)wParam, BV2Option_GetBackColorRef());
			//return DefWindowProc(hWnd, message, wParam, lParam);

		case WM_PAINT:
            DrawUpdateScrollBars (hWnd);
			hdc = BeginPaint(hWnd, &ps);
			DrawDoPaint (hWnd, hdc);
			EndPaint(hWnd, &ps);
			//SetActiveWindow (hWnd);

			{
                char zoomNow[10];
                sprintf (zoomNow,"%d%%", (int)(g_ScaleIndex[g_ScaleNumber]*100));
                SetStatusBar1Text (zoomNow);
			}
			break;

		case WM_TIMER:
            if (!g_dataBagus)
                break;
			KillTimer (hWnd, wParam);
			if (wParam==BLINK_TIMERID && mydraw.b_selectfind && BV2Option_IsBlink())
			{
				mydraw.b_DrawHilitedOnly = TRUE;
				mydraw.b_blinknow = !mydraw.b_blinknow;
				InvalidateRect (hWnd, NULL, FALSE);
			}
			break;

		case WM_HSCROLL:
            if (!g_dataBagus || !mydraw.b_Draw)
                break;
			if (DrawDoScrollH (hWnd, LOWORD(wParam)))
				EraseWindow (hWnd);
			break;

		case WM_VSCROLL:
			if (DrawDoScrollV (hWnd, LOWORD(wParam)))
				EraseWindow (hWnd);
			break;

		case WM_MOUSEMOVE:
			if (!g_dataBagus || !mydraw.b_Draw)
                break;
            else
			{
				char pointer[256];
				int x,y;
				int fKey = wParam;

				DrawWnd2ViewXy (LOWORD(lParam), HIWORD(lParam), &x, &y);
				if ( (fKey & MK_LBUTTON) == MK_LBUTTON && mymouse_down)
				{
                    mydraw.b_drag = TRUE;
                    CopyRect (&mydraw.drawrect0,&mydraw.drawrect);
                    SetRect (&mydraw.drawrect, min(iLBDx,x),min(iLBDy,y),max(iLBDx,x),max(iLBDy,y));
                    InvalidateRect (hWnd, NULL, FALSE);
				}
				else if (mydraw.b_drag)
				{
                    mydraw.b_drag = FALSE;
                    InvalidateRect (hWnd, NULL, TRUE);
				}
				sprintf (pointer, "Pointer: %d, %d", x, y);
				//sprintf (pointer, "Pointer: %d, %d - %d, %d", iLBDx, iLBDy, x, y);
				//sprintf (pointer, "Pointer: %d, %d - %d, %d", mydraw.drawrect.left, mydraw.drawrect.top, mydraw.drawrect.right, mydraw.drawrect.bottom);
				SetStatusBarText (pointer);
			}
            break;


		case WM_LBUTTONDOWN:
			if (!g_dataBagus || !mydraw.b_Draw)
                break;
            iLBDy = HIWORD(lParam);
            iLBDx = LOWORD(lParam);
            DrawWnd2ViewXy (LOWORD(lParam), HIWORD(lParam), &iLBDx, &iLBDy);
            SetRect (&mydraw.drawrect, iLBDx,iLBDy,iLBDx,iLBDy);
            mymouse_down = TRUE;
			break;

/*
		case WM_MOUSELEAVE:
		case WM_MOUSEHOVER:
			mydraw.b_drag = FALSE;
            mydraw.b_goto = FALSE;
            EraseWindow (hWnd);
            break;
*/

		case WM_RBUTTONUP:
            DrawClearSlection();
            break;

		case WM_LBUTTONUP:
			if (!g_dataBagus || !mydraw.b_Draw)
                break;
            else
			{
				int dy = 0;
				int dx = 0;
				int x,y;

                CopyRect (&mydraw.drawrect0, &mydraw.drawrect);
                SetRectEmpty (&mydraw.drawrect);
				DrawWnd2ViewXy (LOWORD(lParam), HIWORD(lParam), &dx, &dy);
				x = dx; y = dy;
				dx -= iLBDx; dy -= iLBDy;
				if (mydraw.b_goto)
                    DrawZoomToThisRect (x-2,y-2,x+2,y+2);
                else if (mydraw.b_drag)
                {
                    if (abs(dx)>=100 || abs(dy)>=100)
                    {
                        DrawZoomToThisRect (min(mydraw.drawrect0.left,mydraw.drawrect0.right), min(mydraw.drawrect0.top,mydraw.drawrect0.bottom),
                            max(mydraw.drawrect0.left,mydraw.drawrect0.right), max(mydraw.drawrect0.top,mydraw.drawrect0.bottom));
                        //EraseWindow (hWnd);
                    }
                    else
                    {
                        EraseWindow (hWnd);
                    }

                }
                // else /*shb-20070903*/
				if (abs(dx)<=5 && abs(dy)<=5)
				//if (iLBDx == dx && iLBDy == dy)
				{
					int fKey = wParam;
					if ( (fKey & MK_CONTROL) == MK_CONTROL)
					{
					    DrawMoveThisPtToCenterOfScreen(x,y);
					    if ((fKey & MK_SHIFT) == MK_SHIFT)
					    {
					        DrawDoDecrScaleIndex();
					    }
					    else
                            DrawDoIncrScaleIndex();
					}
					else
					{
                        DrawWnd2ViewXy (LOWORD(lParam), HIWORD(lParam), &x, &y);
                        DrawClickHandle (hWnd, x, y);
					}
				}

				mydraw.b_drag = FALSE;
				mydraw.b_goto = FALSE;
				mymouse_down = FALSE;
			}
			break;

        case WM_LBUTTONDBLCLK:
			if (!g_dataBagus || !mydraw.b_Draw)
                break;
            else
			{
				mydraw.b_goto = TRUE;
				mydraw.b_drag = FALSE;
			}
            break;

		case WM_DESTROY:
			/* prepare to close */
			sdata_hapussemua (&g_sdHLiteDev);
			sdata_hapussemua (&g_sdHLiteTP);
			sdata_hapussemua (&g_sdHLiteTPOld);
			sdata_hapussemua (&g_sdHLiteTrace);
			sdata_hapussemua (&g_sdLockTrace);

            mydraw.b_drag = FALSE;
            mydraw.b_goto = FALSE;
			mymouse_down = FALSE;

			DrawDeleteColors ();

			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

void DrawLine (HDC hdc, int x1, int y1, int x2, int y2)
{
	if (mydraw.b_selectfind && BV2Option_IsLine())
	{
		SelectObject(hdc, hpLineColor);
		MoveToEx (hdc, x1, y1, NULL);
		LineTo (hdc, x2, y2);
	}
}

void DrawNormalComponent (HDC hdc, komponen *komp)
{
	if (komp->pos_tb=='T' && mydraw.b_STopDev)
	{
		SelectObject(hdc, hpDeviceTColor);
		Rectangle (hdc, komp->pos_x1, komp->pos_y1, komp->pos_x2, komp->pos_y2); /* gambar kotak */
	}
	else if (komp->pos_tb=='B' && mydraw.b_SBotDev)
	{
		SelectObject(hdc, hpDeviceBColor);
		Rectangle (hdc, komp->pos_x1, komp->pos_y1, komp->pos_x2, komp->pos_y2); /* gambar kotak */
	}
}

void DrawNormalComponents (HDC hdc, _sdataptr skomp)
{
	if (sdata_kekepala(skomp)>=0)
	{
		komponen komp;
		while(1)
		{
			sdata_ambildataini(skomp, &komp);
			DrawNormalComponent(hdc, &komp);
			if (sdata_diekorkah(skomp))
				break;
			sdata_kedepan(skomp);
		}
	}
}

void DrawLineToRect (HDC hdc, int x1, int y1, int x2, int y2)
{
	boardptr bptr = GetBVBoardPtr();
	DrawLine (hdc, bptr->kiri, y1+(y2-y1)/2, bptr->kanan, y1+(y2-y1)/2);
	DrawLine (hdc, x1+(x2-x1)/2, bptr->bawah, x1+(x2-x1)/2, bptr->atas);
}

void DrawSelectedComponent (HDC hdc, komponen *komp)
{
	if (komp->pos_tb=='T' && mydraw.b_STopDev ||
		komp->pos_tb=='B' && mydraw.b_SBotDev)
	{
		if (mydraw.b_selectfind && mydraw.b_blinknow)
			SelectObject(hdc, hpLineColor);
		else
			SelectObject(hdc, hpSelectedColor);
		Rectangle (hdc, komp->pos_x1, komp->pos_y1, komp->pos_x2, komp->pos_y2); /* gambar kotak */
	}
}

void DrawCHLComponents (HDC hdc)
{
    int i;
    int x,y;
    komponen k;

    if (sdata_kekepala(&g_sdHLiteDev)!=0)
        return;
    i = 0;
    x=y=0;
    while (sdata_ambildatanomor(&g_sdHLiteDev, i, &k)>=0)
    {
        DrawSelectedComponent(hdc, &k);
        if (i==0)
        {
            x=k.pos_x1+(k.pos_x2-k.pos_x1)/2;
            y=k.pos_y1+(k.pos_y2-k.pos_y1)/2;
         }
        else if (mydraw.b_SConGuide)
        {
            DrawLine (hdc, x, y, k.pos_x1+(k.pos_x2-k.pos_x1)/2, k.pos_y1+(k.pos_y2-k.pos_y1)/2);
        }
       ++i;
    }
}

void DrawNormalTestpin (HDC hdc, testpoint *n)
{
	if (n->pos_tb =='T' && mydraw.b_STopPin)
	{
		SelectObject(hdc, hpTestpinTColor);
		Rectangle (hdc, n->pos_x-XY_GROW, n->pos_y-XY_GROW, n->pos_x+XY_GROW, n->pos_y+XY_GROW); /* gambar kotak */
	}
	else if (n->pos_tb=='B' && mydraw.b_SBotPin)
	{
		SelectObject(hdc, hpTestpinBColor);
		Rectangle (hdc, n->pos_x-XY_GROW, n->pos_y-XY_GROW, n->pos_x+XY_GROW, n->pos_y+XY_GROW); /* gambar kotak */
	}
}

void DrawSelectedTestpin (HDC hdc, testpoint *n)
{
	if (n->pos_tb =='T' && mydraw.b_STopPin ||
		n->pos_tb=='B' && mydraw.b_SBotPin)
	{
		if (mydraw.b_selectfind && mydraw.b_blinknow)
			SelectObject(hdc, hpLineColor);
		else
			SelectObject(hdc, hpSelectedColor);
		Rectangle (hdc, n->pos_x-XY_GROW, n->pos_y-XY_GROW, n->pos_x+XY_GROW, n->pos_y+XY_GROW); /* gambar kotak */
	}
}

void DrawNormalTestpins (HDC hdc, _sdataptr stp_ptr)
{
	if (sdata_kekepala(stp_ptr)>=0)
	{
		testpoint tp;
		while(1)
		{
			sdata_ambildataini(stp_ptr, &tp);
			DrawNormalTestpin (hdc, &tp);
			if (sdata_diekorkah(stp_ptr))
				break;
			sdata_kedepan(stp_ptr);
		}
	}
}

void DrawSelectedTestpins (HDC hdc)
{
	int i=0;
	if (sdata_jumlahdata(&g_sdHLiteTP)>0)
	{
		if (sdata_kekepala(&g_sdHLiteTP)>=0)
		{
			testpoint tp;
			int x,y;
			//x = g_HSPos; y  = -g_VSPos-PTSCROLLMOVE;
			x=y=0;
			while(1)
			{
				sdata_ambildataini(&g_sdHLiteTP, &tp);

                if (i!=0 && mydraw.b_SConGuide)  /*shb-20070903*/
                    DrawLine (hdc, x, y, tp.pos_x, tp.pos_y);
				x = tp.pos_x; y =  tp.pos_y;

				DrawSelectedTestpin (hdc, &tp);
				if (i==g_point2selectedpin)
                    DrawLineToRect (hdc, tp.pos_x, tp.pos_y, tp.pos_x, tp.pos_y);
				if (sdata_diekorkah(&g_sdHLiteTP))
					break;
				sdata_kedepan(&g_sdHLiteTP);
                i++;

			}
		}
	}
}

void DrawTrace (HDC hDC, trace *trc)
{
	HPEN oldpen;
	switch (trc->tipe)
	{
	case SBOARD_VIA:
        oldpen = SelectObject(hDC, hpViaColor);
		Ellipse (hDC, trc->data[0]-VIA_GROW, trc->data[1]-VIA_GROW,
			trc->data[0]+VIA_GROW, trc->data[1]+VIA_GROW);
        SelectObject(hDC, oldpen); /* shb 20071025 */
		break;
	case SBOARD_LINE:
		MoveToEx (hDC, trc->data[0], trc->data[1], NULL);
		LineTo (hDC, trc->data[2], trc->data[3]);
		break;
	case SBOARD_ARC:
		{
			double dx,dy;
			double dr;
			int r;
            int oldad = GetArcDirection (hDC);

			dx = trc->data[0]-trc->data[4];
			dy = trc->data[1]-trc->data[5];
			dr = sqrt(dx*dx+dy*dy);
			r = (int)dr;
			if (!trc->data[SBOARD_TRACE_CWF])
                SetArcDirection (hDC, AD_COUNTERCLOCKWISE);
            else
                SetArcDirection (hDC, AD_CLOCKWISE);
			Arc (hDC, trc->data[4]-r, trc->data[5]-r, trc->data[4]+r, trc->data[5]+r,
				trc->data[0], trc->data[1], trc->data[2], trc->data[3]);
            SetArcDirection (hDC, oldad);
		}

		break;
    case SBOARD_RECT:
        Rectangle (hDC, trc->data[0], trc->data[1], trc->data[2], trc->data[3]);
        break;

	}

}

void DrawTracesAll (HDC hDC, _sdataptr traceptr)
{
	if (sdata_kekepala(traceptr)>=0)
	{
		trace trc;
		while (1)
		{
			sdata_ambildataini (traceptr, &trc);
			switch (trc.data[SBOARD_TRACE_LAYERF])
			{
			    case SBOARD_TRACE_TOP:
				if (mydraw.b_STopTrace)
				{
					SelectObject(hDC, hpTraceTColor);
					DrawTrace (hDC, &trc);
				}
			    break;
			    case SBOARD_TRACE_BOTTOM:
				if (mydraw.b_SBotTrace)
				{
					SelectObject(hDC, hpTraceBColor);
					DrawTrace (hDC, &trc);
				}
			    break;
			    default:
				if (mydraw.b_SInTrace)
				{
					SelectObject(hDC, hpTraceColor);
					DrawTrace (hDC, &trc);
				}
			}

			if (sdata_diekorkah(traceptr))
				break;
			sdata_kedepan (traceptr);
		}
	}
}

void DrawLockedTraces (HDC hDC, _sdataptr traceptr)
{
	if (sdata_kekepala(traceptr)>=0)
	{
		trace trc;
		SelectObject(hDC, hpLockedTraceColor);
		while (1)
		{
			sdata_ambildataini (traceptr, &trc);
			DrawTrace (hDC, &trc);
			if (sdata_diekorkah(traceptr))
				break;
			sdata_kedepan (traceptr);
		}
	}
}


/********************************* MAIN PAINT *********************************************************8********/
void DrawDoPaint(HWND hWnd, HDC hdc)
{
	mydraw.b_Draw = FALSE;

	int dx,dy;
	double ts;

	ts = 1000/g_ScaleIndex[g_ScaleNumber]; //(1000 -> 1/1000 inch = 1 mils)

	dx=dy=(int)ts;

	SetMapMode(hdc, MM_ISOTROPIC);
	SetWindowExtEx(hdc, dx, -dy, NULL);
	SetViewportExtEx(hdc, GetDeviceCaps(hdc, LOGPIXELSX),
	                      GetDeviceCaps(hdc, LOGPIXELSY), NULL);

	if (g_DevCapX<0)
	{
		g_DevCapX = GetDeviceCaps(hdc, LOGPIXELSX);
		g_DevCapY = GetDeviceCaps(hdc, LOGPIXELSY);
	}


	if (!g_dataBagus)
		return;

	SetWindowOrgEx (hdc, g_HSPos, -g_VSPos, NULL); /* view port */

	HBRUSH oldBrush = (HBRUSH) SelectObject(hdc, hbNormalBkColor); /* object background/fill color */
	HPEN oldPen = (HPEN) SelectObject(hdc, hpDeviceTColor); /* object color */

	if (mydraw.b_DrawHilitedOnly)
	{
		if (g_kompunhili.nama[0])
			DrawNormalComponent(hdc, &g_kompunhili);

		if (sdata_jumlahdata(&g_sdHLiteTPOld)>0)
			DrawNormalTestpins (hdc, &g_sdHLiteTPOld);

		mydraw.b_DrawHilitedOnly = FALSE;
		g_kompunhili.nama[0] = 0;
		sdata_hapussemua (&g_sdHLiteTPOld);
	}
	else if (!mydraw.b_drag)
	{
		boardptr bptr = GetBVBoardPtr();

		SetStatusBarText ("Drawing...");

		/* gambar komponen */
		DrawNormalComponents (hdc, &bptr->skomp);

		/* testpin */
		DrawNormalTestpins (hdc, &bptr->stestpoint);

		SetStatusBarText ("Drawing was completed!");
	}

	if (mydraw.b_drag)
	{
        int oldDMode = SetROP2 (hdc, R2_XORPEN);
        SelectObject(hdc, hpLineColor);
        //DrawFocusRect (hdc, &mydraw.drawrect);
        Rectangle (hdc, mydraw.drawrect0.left,mydraw.drawrect0.top, mydraw.drawrect0.right,mydraw.drawrect0.bottom);
        //if (!IsRectEmpty(&mydraw.drawrect))
            Rectangle (hdc, mydraw.drawrect.left,mydraw.drawrect.top, mydraw.drawrect.right,mydraw.drawrect.bottom);
        //else
        //    Rectangle (hdc, mydraw.drawrect0.left,mydraw.drawrect0.top, mydraw.drawrect0.right,mydraw.drawrect0.bottom);

        SetROP2 (hdc, oldDMode);

		//mydraw.b_drag = FALSE;
	}

	/* selected/hilited */
	if (mydraw.b_ShowTrace)
	{
		DrawTracesAll (hdc, &g_sdHLiteTrace);
		DrawLockedTraces (hdc, &g_sdLockTrace);
	}

	/* change bk color when blinking, at not clear mode only */
	if (mydraw.b_selectfind && !BV2Option_IsClear())
	{
		if (mydraw.b_blinknow)
			SelectObject(hdc, hbBackColor);
		else
			SelectObject(hdc, hbSelectedBkColor);
	}

	if (g_komphilite.nama[0])
	{
        DrawLineToRect (hdc,
            g_komphilite.pos_x1, g_komphilite.pos_y1,
            g_komphilite.pos_x2, g_komphilite.pos_y2);
		DrawSelectedComponent (hdc, &g_komphilite);
	}
  	DrawSelectedTestpins (hdc);

	DrawCHLComponents(hdc);

	if (mydraw.b_selectfind && BV2Option_IsBlink())
		SetTimer (hWnd, BLINK_TIMERID, BLINK_TIMERMS, NULL);

	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);

	mydraw.b_Draw = TRUE;
}

double GetPtFactorX ()
{
	double ts = 1000/g_ScaleIndex[g_ScaleNumber];
	return g_DevCapX/ts;

}

double GetPtFactorY ()
{
	double ts = 1000/g_ScaleIndex[g_ScaleNumber];
	return g_DevCapY/ts;

}

/* dipanggil HANYA jika ada WM_SIZE */
void DrawUpdateScrollBarsSIZE (HWND hWnd)
{
	SCROLLINFO sinfo;
	RECT r;

	if (!g_dataBagus)
		return;

	GetClientRect (hWnd, &r);

	/* SBS_HORZ */
	sinfo.fMask = SIF_ALL;
	sinfo.cbSize = sizeof (SCROLLINFO);
	GetScrollInfo (hWnd,SBS_HORZ, &sinfo);
	sinfo.nPage = (UINT)((r.right-r.left)/GetPtFactorX());
	if (sinfo.nPage>(UINT)(sinfo.nMax-sinfo.nMin))
		sinfo.nPos = g_HSPos = sinfo.nMin;
	SetScrollInfo (hWnd, SBS_HORZ, &sinfo, FALSE);


	/* SBS_VERT */
	sinfo.fMask = SIF_ALL;
	sinfo.cbSize = sizeof (SCROLLINFO);
	GetScrollInfo (hWnd,SBS_VERT, &sinfo);
	sinfo.nPage = (UINT)((r.bottom-r.top)/GetPtFactorY());
	if (sinfo.nPage>(UINT)(sinfo.nMax-sinfo.nMin))
		sinfo.nPos = g_VSPos = sinfo.nMin;
	SetScrollInfo (hWnd, SBS_VERT, &sinfo, FALSE);
}

/* dipanggil untuk menggambar scrollbar */
void DrawUpdateScrollBars (HWND hWnd)
{
	SCROLLINFO sinfo;
	RECT r;
	int lenpt;
	double flenpt;
	static BOOL bInitiated=FALSE;
	//BOOL Need2Redraw =  FALSE;

	GetClientRect (hWnd, &r);

	if (!g_dataBagus)
	{
		ShowScrollBar (hWnd, SBS_VERT, FALSE);
		ShowScrollBar (hWnd, SBS_HORZ, FALSE);
		bInitiated=FALSE;
		return;
	}
	else if (!bInitiated)
	{
		DrawInitScrollBars();
		bInitiated=TRUE;
	}

    ShowScrollBar (hWnd, SBS_VERT, TRUE);
    ShowScrollBar (hWnd, SBS_HORZ, TRUE);

	/* SBS_HORZ */
	sinfo.fMask = SIF_ALL;
	sinfo.cbSize = sizeof (SCROLLINFO);
	GetScrollInfo (hWnd, SBS_HORZ, &sinfo);
	sinfo.nPos = g_HSPos;
	flenpt = GetPtFactorX()*(sinfo.nMax-sinfo.nMin);
	lenpt = (int)flenpt;
	sinfo.nPage = (UINT)((r.right-r.left)/GetPtFactorX());
	if (sinfo.nPage>(UINT)(sinfo.nMax-sinfo.nMin))
	{
		//if (sinfo.nPos != sinfo.nMin)
        //    Need2Redraw = TRUE;
		sinfo.nPos = g_HSPos = sinfo.nMin;
	}
	else if (sinfo.nPage>(UINT)(sinfo.nMax-sinfo.nPos))
	{
		//if (sinfo.nPos != sinfo.nMax-sinfo.nPage)
        //    Need2Redraw = TRUE;
		sinfo.nPos = g_HSPos = sinfo.nMax-sinfo.nPage;
	}

	if (lenpt>(r.right-r.left))
	{
		SetScrollInfo (hWnd, SBS_HORZ, &sinfo, TRUE); //TRUE
		//ShowScrollBar (hWnd, SBS_HORZ, TRUE); //TRUE
		EnableScrollBar (hWnd, SBS_HORZ, ESB_ENABLE_BOTH);
	}
	else
		//ShowScrollBar (hWnd, SBS_HORZ, FALSE);
		EnableScrollBar (hWnd, SBS_HORZ, ESB_DISABLE_BOTH);

	/* SBS_VERT */
	//Need2Redraw =  FALSE;

	sinfo.fMask = SIF_ALL;
	sinfo.cbSize = sizeof (SCROLLINFO);
	GetScrollInfo (hWnd, SBS_VERT, &sinfo);
	sinfo.nPos = g_VSPos;
	flenpt = GetPtFactorY()*(sinfo.nMax-sinfo.nMin);
	lenpt = (int)flenpt;
	sinfo.nPage = (UINT)((r.bottom-r.top)/GetPtFactorY());
	if (sinfo.nPage>(UINT)(sinfo.nMax-sinfo.nMin))
	{
		//if (sinfo.nPos != sinfo.nMin)
        //    Need2Redraw = TRUE;
		sinfo.nPos = g_VSPos = sinfo.nMin;
	}
	else if (sinfo.nPage>(UINT)(sinfo.nMax-sinfo.nPos))
	{
		//if (sinfo.nPos != sinfo.nMax-sinfo.nPage)
        //    Need2Redraw = TRUE;
		sinfo.nPos = g_VSPos = sinfo.nMax-sinfo.nPage;
	}

	if (lenpt>(r.bottom-r.top))
	{
		SetScrollInfo (hWnd, SBS_VERT, &sinfo, TRUE);  //TRUE
		//ShowScrollBar (hWnd, SBS_VERT, TRUE);  //TRUE
		EnableScrollBar (hWnd, SBS_VERT, ESB_ENABLE_BOTH);
	}
	else
		//ShowScrollBar (hWnd, SBS_VERT, FALSE);
		EnableScrollBar (hWnd, SBS_VERT, ESB_DISABLE_BOTH);
}

void DrawEnsureHVLimits ()
{
    boardptr bptr = GetBVBoardPtr();

    if (g_HSPos < board_kiri(bptr)-PTSCROLLMOVE)
        g_HSPos = board_kiri(bptr)-PTSCROLLMOVE;
    if (g_VSPos < -(board_atas(bptr)+PTSCROLLMOVE))
        g_VSPos = -(board_atas(bptr)+PTSCROLLMOVE);
}

BOOL DrawSetScaleIndex (int index)
{
	int old = g_ScaleNumber;
	if (index<0)
		index=0;
	else if (index>=NUMSCALEINDEX)
		index=NUMSCALEINDEX-1;

    RECT r;
    GetClientRect (mydraw.hwnd_This, &r);

    int x,y;
    DrawWnd2ViewXy ((r.right-r.left)/2,(r.bottom-r.top)/2, &x,&y);

    int wwid=(int)((r.right-r.left)/GetPtFactorX());
    int whi=(int)((r.bottom-r.top)/GetPtFactorY());

	g_ScaleNumber = index;

    int wwid2=(int)((r.right-r.left)/GetPtFactorX());
    int whi2=(int)((r.bottom-r.top)/GetPtFactorY());

    if (old != g_ScaleNumber)
    {
        g_HSPos -= (wwid2-wwid)/2;
        g_VSPos -= (whi2-whi)/2;
        DrawEnsureHVLimits();
    }

	return (old != g_ScaleNumber);
}

BOOL DrawIncrScaleIndex ()
{
	int index = g_ScaleNumber;
	return DrawSetScaleIndex(++index);
}

BOOL DrawDecrScaleIndex ()
{
	int index = g_ScaleNumber;
	return DrawSetScaleIndex(--index);
}

void DrawDoIncrScaleIndex ()
{
	if (DrawIncrScaleIndex())
	{
		//DrawUpdateScrollBars (mydraw.hwnd_This);
		EraseWindow(mydraw.hwnd_This);
	}
	else
        SetStatusBarText ("This is maximum size!");
}

void DrawDoDecrScaleIndex ()
{
	if (DrawDecrScaleIndex())
	{
		//DrawUpdateScrollBars (mydraw.hwnd_This);
		EraseWindow(mydraw.hwnd_This);
	}
	else
        SetStatusBarText ("This is minimum size!");
}

void DrawDoScaleOne ()
{
	if (g_ScaleNumber!=SCALENUMBER_REAL)
	{
		g_ScaleNumber = SCALENUMBER_REAL;
		InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
	}
}

void DrawInitScrollBars ()
{
	if (g_dataBagus)
	{
		boardptr bptr = GetBVBoardPtr();

		g_VSPos = -(board_atas(bptr)+PTSCROLLMOVE);
		g_HSPos = board_kiri(bptr)-PTSCROLLMOVE;

		SetScrollRange (mydraw.hwnd_This, SBS_VERT, -(board_atas(bptr)+PTSCROLLMOVE), -(board_bawah(bptr)-PTSCROLLMOVE), FALSE);
		SetScrollRange (mydraw.hwnd_This, SBS_HORZ, board_kiri(bptr)-PTSCROLLMOVE, board_kanan(bptr)+PTSCROLLMOVE, FALSE);
		SetScrollPos (mydraw.hwnd_This, SBS_VERT, g_VSPos, FALSE);
		SetScrollPos (mydraw.hwnd_This, SBS_HORZ, g_HSPos, FALSE);

	}
	else
	{
		g_HSPos = g_VSPos = 0;
	}
}

BOOL DrawDoScrollH (HWND hWnd, UINT cmd)
{
	BOOL result = FALSE;
	int sisa = 0;
	SCROLLINFO sinfo;

	//int scrollpt;
	//RECT r;
	//GetClientRect (hWnd, &r);

	sinfo.fMask = SIF_ALL;
	sinfo.cbSize = sizeof (SCROLLINFO);
	GetScrollInfo (hWnd,SBS_HORZ, &sinfo);


	switch (cmd)
	{
	case SB_LINERIGHT:
	case SB_PAGERIGHT:

		sisa = sinfo.nMax - sinfo.nPos - sinfo.nPage; /* dalam satuan mils */
		if (sisa<=0)
			return result;

		if (cmd == SB_LINERIGHT)
		{
			if (sisa>PTSCROLLMOVE)
				sisa = PTSCROLLMOVE;
		}
		else if (cmd == SB_PAGERIGHT)
		{
			if (sisa > (int)sinfo.nPage)
				sisa = sinfo.nPage;
		}
		g_HSPos += sisa;
		sisa = -sisa;

		break;

	case SB_LINELEFT:
	case SB_PAGELEFT:
		sisa = sinfo.nPos - sinfo.nMin;
		if (sisa<=0)
			return result;

		if (cmd == SB_LINELEFT)
		{
			if (sisa>PTSCROLLMOVE)
				sisa = PTSCROLLMOVE;
		}
		else if (cmd == SB_PAGELEFT)
		{
			if (sisa > (int)sinfo.nPage)
				sisa = sinfo.nPage;
		}
		g_HSPos -= sisa;
		break;

	case SB_THUMBPOSITION:
	//case SB_THUMBTRACK:
		g_HSPos = sinfo.nTrackPos;
		sisa = sinfo.nPos-sinfo.nTrackPos;
		break;

	case SB_ENDSCROLL:
		//DrawUpdateScrollBars (hWnd);
		//result = TRUE;
		break;
	}

	if (sisa!=0)
	{
		//scrollpt = (int)(sisa*GetPtFactorX()); /* dalam satuan pt */
		//ScrollWindow (hWnd, scrollpt, 0, &r, NULL);
		SetScrollPos (hWnd, SBS_HORZ, g_HSPos, TRUE);
		result = TRUE;
	}

	return result;
}

BOOL DrawDoScrollV (HWND hWnd, UINT cmd)
{
	BOOL result = FALSE;
	int sisa = 0;
	SCROLLINFO sinfo;

	//int scrollpt;
	//RECT r;

	//GetClientRect (hWnd, &r);

	sinfo.fMask = SIF_ALL;
	sinfo.cbSize = sizeof (SCROLLINFO);
	GetScrollInfo (hWnd,SBS_VERT, &sinfo);



	//char temp[256];
	//sprintf (temp, "max=%d min=%d pos=%d", sinfo.nMax, sinfo.nMin, sinfo.nPos);
	//SetStatusBarText (temp);

	switch (cmd)
	{
	case SB_LINEUP:
	case SB_PAGEUP:
		sisa = sinfo.nPos - sinfo.nMin;
		if (sisa<=0)
			return result;

		if (cmd == SB_LINEUP)
		{
			if (sisa>PTSCROLLMOVE)
				sisa = PTSCROLLMOVE;
		}
		else if (cmd == SB_PAGEUP)
		{
			if (sisa > (int)sinfo.nPage)
				sisa = sinfo.nPage;
		}
		g_VSPos -= sisa;
		break;

	case SB_LINEDOWN:
	case SB_PAGEDOWN:
		sisa = sinfo.nMax - sinfo.nPos - sinfo.nPage; /* dalam satuan mils */
		if (sisa<=0)
			return result;

		if (cmd == SB_LINEDOWN)
		{
			if (sisa>PTSCROLLMOVE)
				sisa = PTSCROLLMOVE;
		}
		else if (cmd == SB_PAGEDOWN)
		{
			if (sisa > (int)sinfo.nPage)
				sisa = sinfo.nPage;
		}
		g_VSPos += sisa;
		sisa = -sisa;
		break;

	case SB_THUMBPOSITION:
	//case SB_THUMBTRACK:
		g_VSPos = sinfo.nTrackPos;
		sisa = sinfo.nPos-sinfo.nTrackPos;
		break;

	case SB_ENDSCROLL:
		//DrawUpdateScrollBars (hWnd);
		//result = TRUE;
		break;
	}

	if (sisa!=0)
	{
		//scrollpt = (int)(sisa*GetPtFactorY()); /* dalam satuan pt */
		//ScrollWindow (hWnd, 0, scrollpt, &r, NULL);
		SetScrollPos (hWnd, SBS_VERT, g_VSPos, TRUE);
		result = TRUE;
	}

	return result;
}

void DrawWnd2ViewXy (int x, int y, int *vx, int *vy)
{
	double fx,fy;
	fx = x/GetPtFactorX();
	fy = -y/GetPtFactorY();
	*vx = (int)floor(fx+g_HSPos);
	*vy = (int)floor(fy-g_VSPos);
}

/*
void DrawView2WndXy (int x, int y, int *wx, int *wy)
{
	double fx,fy;
	fx = x-g_HSPos;
	fy = y+g_VSPos;
	*wx = (int)fx*GetPtFactorX();
	*wy = (int)fy*GetPtFactorY();
}
*/

BOOL DrawFindElement (int x, int y, SBOARD_ELEMENT *eResult, int *rec_no)
{
	BOOL findResult = FALSE;
	int i,k,start;
	char tb;
	char nama[64];
	boardptr bptr = GetBVBoardPtr();

	*eResult = -1;
	*rec_no = -1;

	/* find tp */

	i = tpt_cari_xy (&bptr->stestpoint, x, y, -1 ); //sdata_nomorini(&bptr->stestpoint));
	if (i>=0)
	{
		tpt_tb (&bptr->stestpoint, &tb);
		if (tb=='T' && mydraw.b_STopPin || tb=='B' && mydraw.b_SBotPin)
		{
			*eResult = SBOARD_TESTPIN;
			*rec_no = i;
			findResult = TRUE;
		}
	}

	if (findResult)
		return findResult;

	/* komponen */
	i = -1;
	k = -1;
	start = i;
	while( (i= komp_cari_xy (&bptr->skomp, x, y, i+1))>=0)
	{
        if (k==i || i==start)
            break;

        komp_nama (&bptr->skomp, nama);
        if (strcmp (nama, g_komphilite.nama)==0 )
        {
            k = i;
            continue;
        }

        komp_tb(&bptr->skomp,&tb);
        if ( ( tb=='T' && mydraw.b_STopDev || tb=='B' && mydraw.b_SBotDev ))
        {
                *eResult = SBOARD_KOMPONEN;
                *rec_no = i;
                findResult = TRUE;
                break;
        }
	}

	return findResult;
}

void DrawDevHLitePrepare ()
{
	memcpy (&g_kompunhili, &g_komphilite, sizeof(komponen));
	memset(&g_komphilite,0, sizeof(komponen));
	sdata_hapussemua (&g_sdHLiteDev);
}

void DrawTPHLitePrepare ()
{
	testpoint n;

	sdata_hapussemua (&g_sdHLiteTPOld);
	if (sdata_jumlahdata(&g_sdHLiteTP)<=0)
		return;
	sdata_kekepala(&g_sdHLiteTP);
	while(sdata_ambildataini(&g_sdHLiteTP, &n)>=0)
	{
		sdata_tambah(&g_sdHLiteTPOld, &n, sizeof(testpoint));
		if (sdata_diekorkah(&g_sdHLiteTP))
			break;
		sdata_kedepan(&g_sdHLiteTP);
	}
	sdata_hapussemua (&g_sdHLiteTP);
}

void DrawTraceHLitePrepare ()
{
	sdata_hapussemua (&g_sdHLiteTrace);
}

int DrawSetTraces (int node_id)
{
	boardptr bptr = GetBVBoardPtr();
	int i,j;
	trace tr;

	sdata_hapussemua (&g_sdHLiteTrace);

	i=j=-1;

	while (1)
	{
		i = trace_cari (&bptr->strace, node_id, i+1);
		if (i>=0 && i!=j)
		{
			if (j==-1)
				j=i;
			sdata_ambildataini(&bptr->strace, &tr);
			sdata_tambah(&g_sdHLiteTrace, &tr, sizeof(trace));
		}
		else
			break;
	}

	return sdata_jumlahdata (&g_sdHLiteTrace);
}

void DrawClickHandle (HWND hWnd, int x, int y)
{
	SBOARD_ELEMENT elm;
	int rec;
	boardptr bptr = GetBVBoardPtr();

	SetFocus (GetParent(hWnd));

	if (DrawFindElement(x,y, &elm, &rec))
	{
		mydraw.b_DrawHilitedOnly = TRUE;

		DrawTPHLitePrepare ();
		DrawDevHLitePrepare ();


		if (sdata_jumlahdata(&g_sdHLiteTrace)!=0)
		{
			mydraw.b_DrawHilitedOnly = FALSE;
			sdata_hapussemua (&g_sdHLiteTrace);
		}

		if (mydraw.b_selectfind)
			mydraw.b_DrawHilitedOnly = FALSE;
		mydraw.b_selectfind = FALSE;

		if (elm==SBOARD_KOMPONEN)
		{
			//sdata_kenomor(&bptr->skomp, rec);
			if (sdata_ambildatanomor(&bptr->skomp, rec, &g_komphilite)>=0)
			{
                SetObjectInfo ("Device",g_komphilite.nama, (g_komphilite.pos_tb=='T')?"TOP":"BOTTOM", g_komphilite.info);
                SetStatusBar0Text (g_komphilite.nama);
                BV2Conn_SetDeviceName (g_komphilite.nama);
			}
		}
		else if (elm==SBOARD_TESTPIN)
		{
			testpoint n;
			Str63 nama;
			Str63 info;

			//sdata_kenomor(&bptr->snet, rec);
			if (sdata_ambildatanomor(&bptr->stestpoint, rec, &n)>=0)
			{
                sdata_tambah(&g_sdHLiteTP, &n, sizeof (testpoint));

                if (sdata_kenomor(&bptr->snode, n.node_id)>=0)
                {
                    node_nama (&bptr->snode, nama);
                    tpt_info (&bptr->stestpoint, info);
                    SetObjectInfo ((sdata_jumlahdata(&bptr->strace)==0)?"Testpin":"Signal",nama, (n.pos_tb=='T')?"TOP":"BOTTOM",info); /* Actualy, it is a testpin not a signal */
                    SetStatusBar0Text (nama);

                    BV2Conn_SetSignalIDWithName (n.node_id, nama);
                    DrawSetTraces (n.node_id);
                }
			}
		}

		InvalidateRect (mydraw.hwnd_This, NULL, !mydraw.b_DrawHilitedOnly);
	}
}

void DrawFindComponentHandler (_sdataptr skomptr)
{
	DrawTPHLitePrepare ();
	DrawDevHLitePrepare ();
	sdata_hapussemua (&g_sdHLiteTrace);

	sdata_ambildataini(skomptr, &g_komphilite);
	SetObjectInfo ("Device",g_komphilite.nama, (g_komphilite.pos_tb=='T')?"TOP":"BOTTOM", g_komphilite.info);
    SetStatusBar0Text(g_komphilite.nama);

	mydraw.b_selectfind = TRUE;
	mydraw.b_blinknow = FALSE;

	if (BV2Option_IsZoom())
	{
		DrawZoomToThisRect (g_komphilite.pos_x1,
			g_komphilite.pos_y1,
			g_komphilite.pos_x2,
			g_komphilite.pos_y2 );
	}
	else
	{
		//g_ScaleNumber = SCALENUMBER_REAL;
		g_HSPos = min (g_komphilite.pos_x1, g_komphilite.pos_x2)-PTSCROLLMOVE;
		g_VSPos = -(PTSCROLLMOVE+max (g_komphilite.pos_y1, g_komphilite.pos_y2));

		InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
	}
}

void DrawFindSignalHandler (_sdataptr nodeptr)
{
	Str63 nama;
	Str255 info;
	int id;
	testpoint n;
	int i,j;
	int trc;

	boardptr bptr = GetBVBoardPtr ();

	DrawTPHLitePrepare ();
	DrawDevHLitePrepare ();
	/* sdata_hapussemua (&g_sdHLiteTrace); */
	/* akan dikerjakan oleh DrawSetTraces() */

	node_nama (nodeptr, nama);
	node_info (nodeptr, info);
	id = node_id (nodeptr);

	i = -1; j = -1;
	while (1)
	{
		i = tpt_cari (&bptr->stestpoint, id, i+1);
		if (i>=0 && j!=i)
		{
			if (j==-1)
				j = i;
			sdata_ambildataini(&bptr->stestpoint, &n);
			sdata_tambah(&g_sdHLiteTP, &n, sizeof (testpoint));
			if (strlen(n.info)+1+strlen(info)<=255)
			{
                if (info[0])
                    strcat (info, " ");
                strcat (info,n.info);
			}
			else if (strlen(info)+3<=255)
			{
                strcat (info, "...");
			}
		}
		else
			break;
	}
	g_point2selectedpin = 0;

	tpt_urut (&g_sdHLiteTP);
	if (sdata_kekepala(&g_sdHLiteTP)>=0)
	{
	    sdata_ambildataini(&g_sdHLiteTP, &n);
	}

	trc = DrawSetTraces (id);

	/* testpoint (testpin) not found but found trace(s) */
	if (j==-1 && trc>0)
	{
		trace tr;
		sdata_ambildatanomor(&g_sdHLiteTrace, 0, &tr);
		n.pos_tb = 'T';
		n.pos_x = tr.data[0];
		n.pos_y = tr.data[1];
	}

	SetObjectInfo ("Signal",nama, (n.pos_tb=='T')?"TOP":"BOTTOM",info);
	SetStatusBar0Text (nama);

	if (j==-1 && trc<=0)
		MessageBox (GetParent(mydraw.hwnd_This), "Can not draw the signal!", "Warning", MB_OK|MB_ICONWARNING);
	else
	{
		mydraw.b_selectfind = TRUE;
		mydraw.b_blinknow = FALSE;

		if (BV2Option_IsZoom())
		{
			DrawZoomToThisRect (n.pos_x-XY_GROW,
				n.pos_y-XY_GROW,
				n.pos_x+XY_GROW,
				n.pos_y+XY_GROW );
		}
		else
		{
			//g_ScaleNumber = SCALENUMBER_REAL;
			g_HSPos = n.pos_x-PTSCROLLMOVE;
			g_VSPos = -(PTSCROLLMOVE+n.pos_y);

			InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
		}
	}
}

void DrawFindProbeHandler (_sdataptr tp_ptr, char *probe)
{
	Str63 nama;
	Str255 info;
	//Str63 dummy;
	int id;
	//int probe_index, len;
	testpoint n;
	//int i;

	boardptr bptr = GetBVBoardPtr ();
	//_sdatateks stek;

	DrawTPHLitePrepare ();
	DrawDevHLitePrepare ();
	sdata_hapussemua (&g_sdHLiteTrace);

	tpt_info (tp_ptr, info);
	tpt_id (tp_ptr, &id);
	sdata_kenomor (&bptr->snode, id);
	node_nama (&bptr->snode, nama);
	if (info[0])
        strcat (info," ");
    strcat (info, nama);

	/*
	sdata_init (&stek);
	len = sdatateks_dariteks (&stek, info, ' ', Str63Len, 0);
	probe_index = -1;
	if (len<=0)
		return;
	else if (len==1)
		probe_index = 0;
	else
	{
		for (i=0;i<len;i++)
		{
			sdata_ambildatanomor  (&stek, i, dummy);
			if (strcmp(dummy, probe)==0)
			{
				probe_index = i;
				break;
			}
		}
	}
	sdata_hapussemua (&stek);
	if (probe_index == -1)
		return;

	i = tpt_cari (&bptr->stestpoint, id, 0);
	if (i<0)
		return;
	else if (probe_index!=0)
	{
		i+= probe_index;
		if (sdata_kenomor(&bptr->stestpoint, i)<0)
            return;
	}
	*/

	sdata_ambildataini(&bptr->stestpoint, &n);
	sdata_tambah(&g_sdHLiteTP, &n, sizeof (testpoint));

	SetObjectInfo ("Testpin",probe, (n.pos_tb=='T')?"TOP":"BOTTOM",info);
	SetStatusBar0Text(nama);


	mydraw.b_selectfind = TRUE;
	mydraw.b_blinknow = FALSE;

	if (BV2Option_IsZoom())
	{
		DrawZoomToThisRect (n.pos_x-XY_GROW,
			n.pos_y-XY_GROW,
			n.pos_x+XY_GROW,
			n.pos_y+XY_GROW );
	}
	else
	{
		//g_ScaleNumber = SCALENUMBER_REAL;
		g_HSPos = n.pos_x-PTSCROLLMOVE;
		g_VSPos = -(PTSCROLLMOVE+n.pos_y);

		InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
	}
}

void DrawMoveThisPtToCenterOfScreen (int ocx, int ocy)
{
	RECT r;
	GetClientRect (mydraw.hwnd_This, &r);

	int wwid=(int)((r.right-r.left)/GetPtFactorX());
	int whi=(int)((r.bottom-r.top)/GetPtFactorY());

    g_HSPos = ocx - wwid/2;
    g_VSPos = -(ocy+whi/2);
    //DrawEnsureHVLimits();

}

void DrawZoomToThisRect (int x1, int y1, int x2, int y2)
{
	//if (g_ScaleNumber==0)
	//	return;

	//int oldscale = g_ScaleNumber;
	RECT r;
	GetClientRect (mydraw.hwnd_This, &r);

	boardptr bptr = GetBVBoardPtr ();

	int i;
	int wwid=(int)((r.right-r.left)/GetPtFactorX());
	int whi=(int)((r.bottom-r.top)/GetPtFactorY());
	int ocx = x1+(x2-x1)/2;
	int ocy = y1+(y2-y1)/2;

	if (mydraw.b_drag)
	{
        for (i=0;i<NUMSCALEINDEX;i++)
        {
            g_ScaleNumber = i;
            wwid = (int)((r.right-r.left)/GetPtFactorX());
            whi = (int)((r.bottom-r.top)/GetPtFactorY());
            if (wwid < (x2-x1) &&  whi < (y2-y1) )
                break;
        }
	}
    else if (!mydraw.b_goto)
    {
        for (i=SCALENUMBER_ZOOM;i>=0;i--)
        {
            g_ScaleNumber = i;
            wwid = (int)((r.right-r.left)/GetPtFactorX());
            whi = (int)((r.bottom-r.top)/GetPtFactorY());
            if (wwid > (x2-x1) &&  whi > (y2-y1) )
                break;
        }
    }

    g_HSPos = ocx - wwid/2;
    g_VSPos = -(ocy+whi/2);
    //g_VSPos = -(board_atas(bptr)+PTSCROLLMOVE);
    //g_HSPos = board_kiri(bptr)-PTSCROLLMOVE;
    /*

    if (g_HSPos < board_kiri(bptr)-PTSCROLLMOVE)
        g_HSPos = board_kiri(bptr)-PTSCROLLMOVE;
    if (g_VSPos < -(board_atas(bptr)+PTSCROLLMOVE))
        g_VSPos = -(board_atas(bptr)+PTSCROLLMOVE);
    */
    DrawEnsureHVLimits();

	EraseWindow (mydraw.hwnd_This);
}

BOOL IsDrawHasHilitedDevice ()
{
	return (g_komphilite.nama[0])?TRUE:FALSE;
}

void DrawZoomToDeviceHilite ()
{
	if (g_komphilite.nama[0])
		DrawZoomToThisRect (g_komphilite.pos_x1, g_komphilite.pos_y1,
			g_komphilite.pos_x2, g_komphilite.pos_y2);
}

void DrawZoomToFit ()
{
	if (g_ScaleNumber==0)
		return;

	int oldscale = g_ScaleNumber;
	RECT r;
	GetClientRect (mydraw.hwnd_This, &r);

	boardptr bptr = GetBVBoardPtr ();

	int i;
	for (i=NUMSCALEINDEX-1;i>=0;i--)
	{
		g_ScaleNumber = i;
		if (g_ScaleNumber==0)
			break;
		else if ( ((r.right-r.left)/GetPtFactorX()) > (board_kanan(bptr)-board_kiri(bptr)) &&
			((r.bottom-r.top)/GetPtFactorY()) > (board_atas(bptr)-board_bawah(bptr)) )
			break;
	}

	if (g_ScaleNumber!=oldscale)
		EraseWindow (mydraw.hwnd_This);
}

BOOL DrawIsHasHLTrace ()
{
	return ((sdata_jumlahdata(&g_sdHLiteTrace)==0)?FALSE:TRUE);
}

void DrawLockHLTrace (BOOL add)
{
	trace tr;

	if (!add)
        sdata_hapussemua (&g_sdLockTrace);
	if (sdata_kekepala(&g_sdHLiteTrace)<0)
		return;
	while (1)
	{
		sdata_ambildataini (&g_sdHLiteTrace, &tr);
		sdata_tambah (&g_sdLockTrace, &tr, sizeof (trace));
		if (sdata_kedepan (&g_sdHLiteTrace)==kSDATAERR_NOMORE)
			break;
	}
	sdata_hapussemua (&g_sdHLiteTrace);
    EraseWindow (mydraw.hwnd_This);
}

BOOL DrawIsHasLockedTrace ()
{
	return ((sdata_jumlahdata(&g_sdLockTrace)==0)?FALSE:TRUE);
}

void DrawClearLockedTrace ()
{
	sdata_hapussemua (&g_sdLockTrace);
	InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

void DrawFlipAccessories (int x, int y)
{
	int x1,y1,x2,y2;
	trace tr;
	if (g_komphilite.nama[0])
	{
		object_cermin (&g_komphilite.pos_x1,
			&g_komphilite.pos_y1,
			&g_komphilite.pos_x2,
			&g_komphilite.pos_y2,
			x, y);
	}
	if (sdata_kekepala(&g_sdHLiteDev)==0)
	{
		do
		{
			komp_xy (&g_sdHLiteDev, &x1, &y1, &x2, &y2);
			object_cermin (&x1, &y1, &x2, &y2, x, y);
			komp_barui_xy (&g_sdHLiteDev, x1, y1, x2, y2);
		} while (sdata_kedepan (&g_sdHLiteDev)>=0);
	}

	if (sdata_kekepala(&g_sdHLiteTP)==0)
	{
		do
		{
			tpt_xy (&g_sdHLiteTP, &x1, &y1); x2=x1; y2=y1;
			object_cermin (&x1, &y1, &x2, &y2, x, y);
			tpt_barui_xy (&g_sdHLiteTP, x1, y1);
		} while (sdata_kedepan (&g_sdHLiteTP)>=0);
	}

	if (sdata_kekepala(&g_sdHLiteTrace)==0)
	{
		do
		{
			sdata_ambildataini (&g_sdHLiteTrace, &tr);
			objecttrace_cermin(&tr, x, y);
			sdata_baruiini (&g_sdHLiteTrace, &tr, sizeof(trace));
		} while((sdata_kedepan(&g_sdHLiteTrace)>=0));
	}
	if (sdata_kekepala(&g_sdLockTrace)==0)
	{
		do
		{
			sdata_ambildataini (&g_sdLockTrace, &tr);
			objecttrace_cermin(&tr, x, y);
			sdata_baruiini (&g_sdLockTrace, &tr, sizeof(trace));
		} while((sdata_kedepan(&g_sdLockTrace)>=0));
	}

}

void DrawRotateAccessories (int angle)
{
	int x1,y1,x2,y2;
	trace tr;
	if (g_komphilite.nama[0])
	{
		object_putar (&g_komphilite.pos_x1,
			&g_komphilite.pos_y1,
			&g_komphilite.pos_x2,
			&g_komphilite.pos_y2,
			angle);
	}
	if (sdata_kekepala(&g_sdHLiteDev)==0)
	{
		do
		{
			komp_xy (&g_sdHLiteDev, &x1, &y1, &x2, &y2);
			object_putar (&x1, &y1, &x2, &y2, angle);
			komp_barui_xy (&g_sdHLiteDev, x1, y1, x2, y2);
		} while (sdata_kedepan (&g_sdHLiteDev)>=0);
	}

	if (sdata_kekepala(&g_sdHLiteTP)==0)
	{
		do
		{
			tpt_xy (&g_sdHLiteTP, &x1, &y1); x2=x1; y2=y1;
			object_putar (&x1, &y1, &x2, &y2, angle);
			tpt_barui_xy (&g_sdHLiteTP, x1, y1);
		} while (sdata_kedepan (&g_sdHLiteTP)>=0);
	}

	if (sdata_kekepala(&g_sdHLiteTrace)==0)
	{
		do
		{
			sdata_ambildataini (&g_sdHLiteTrace, &tr);
			objecttrace_putar(&tr, angle);
			sdata_baruiini (&g_sdHLiteTrace, &tr, sizeof(trace));
		} while((sdata_kedepan(&g_sdHLiteTrace)>=0));
	}
	if (sdata_kekepala(&g_sdLockTrace)==0)
	{
		do
		{
			sdata_ambildataini (&g_sdLockTrace, &tr);
			objecttrace_putar(&tr, angle);
			sdata_baruiini (&g_sdLockTrace, &tr, sizeof(trace));
		} while((sdata_kedepan(&g_sdLockTrace)>=0));
	}

}

void DrawMoveAccessories (int dx, int dy)
{
	int x1,y1,x2,y2;
	trace tr;
	if (g_komphilite.nama[0])
	{
		object_geser (&g_komphilite.pos_x1, &g_komphilite.pos_y1, dx, dy);
		object_geser (&g_komphilite.pos_x2, &g_komphilite.pos_y2, dx, dy);
	}
	if (sdata_kekepala(&g_sdHLiteDev)==0)
	{
		do
		{
			komp_xy (&g_sdHLiteDev, &x1, &y1, &x2, &y2);
			object_geser (&x1, &y1, dx, dy);
			object_geser (&x2, &y2, dx, dy);
			komp_barui_xy (&g_sdHLiteDev, x1, y1, x2, y2);
		} while (sdata_kedepan (&g_sdHLiteDev)>=0);
	}

	if (sdata_kekepala(&g_sdHLiteTP)==0)
	{
		do
		{
			tpt_xy (&g_sdHLiteTP, &x1, &y1); x2=x1; y2=y1;
			object_geser (&x1, &y1, dx, dy);
			tpt_barui_xy (&g_sdHLiteTP, x1, y1);
		} while (sdata_kedepan (&g_sdHLiteTP)>=0);
	}

	if (sdata_kekepala(&g_sdHLiteTrace)==0)
	{
		do
		{
			sdata_ambildataini (&g_sdHLiteTrace, &tr);
			objecttrace_geser(&tr, dx, dy);
			sdata_baruiini (&g_sdHLiteTrace, &tr, sizeof(trace));
		} while((sdata_kedepan(&g_sdHLiteTrace)>=0));
	}
	if (sdata_kekepala(&g_sdLockTrace)==0)
	{
		do
		{
			sdata_ambildataini (&g_sdLockTrace, &tr);
			objecttrace_geser(&tr, dx, dy);
			sdata_baruiini (&g_sdLockTrace, &tr, sizeof(trace));
		} while((sdata_kedepan(&g_sdLockTrace)>=0));
	}

}


BOOL DrawSetCHLiteDev (char *name, BOOL IsCenter)
{
    boardptr bptr = GetBVBoardPtr();
    komponen k;

    if (IsCenter)
    {
        sdata_hapussemua (&g_sdHLiteDev);
        sdata_hapussemua (&g_sdHLiteTP);
        sdata_hapussemua (&g_sdHLiteTrace);
        //sdata_hapussemua (&g_sdLockTrace);
        if (sdata_kekepala(&bptr->skomp)<0)
            return FALSE;
        mydraw.b_selectfind = TRUE;

    }

    if (komp_cari (&bptr->skomp, name, sdata_nomorini(&bptr->skomp))>=0)
    {
        sdata_ambildataini (&bptr->skomp, &k);
        sdata_tambah(&g_sdHLiteDev, &k, sizeof(komponen));
    }
    return TRUE;
}

BOOL DrawGetHLiteDev (komponen *kptr)
{
    if (!IsDrawHasHilitedDevice())
        return FALSE;
    memcpy (kptr, &g_komphilite, sizeof(g_komphilite));
    return TRUE;
}

int DrawGetPoint2SelectedPin ()
{
    return g_point2selectedpin;
}

int DrawGetCountOfSelectedPins ()
{
    return sdata_jumlahdata(&g_sdHLiteTP);
}

BOOL DrawSetOpbjectInfoOfSelectedTestpin ()
{
    testpoint tp;
    Str63 signal;

    GetObjectInfo (NULL, signal, NULL, NULL);
    sdata_ambildatanomor( &g_sdHLiteTP, g_point2selectedpin,&tp);
    SetObjectInfo ("Testpin", signal, (tp.pos_tb=='T')?"TOP":"BOTTOM", tp.info);
	if (BV2Option_IsZoom())
	{
        DrawZoomToThisRect (tp.pos_x-XY_GROW,
				tp.pos_y-XY_GROW,
				tp.pos_x+XY_GROW,
				tp.pos_y+XY_GROW );
        return TRUE;
    }
    return FALSE;
}

void DrawSetPoint2SelectedPin1st ()
{
    //if (g_point2selectedpin==0) /*shb-20070903*/
    //    return;
    g_point2selectedpin = 0;
    if (!DrawSetOpbjectInfoOfSelectedTestpin())
        InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

void DrawSetPoint2SelectedPinLast ()
{
    int c = DrawGetCountOfSelectedPins()-1;
    //if (g_point2selectedpin>=c) /*shb-20070903*/
    //    return;
    g_point2selectedpin = c;
    if (!DrawSetOpbjectInfoOfSelectedTestpin())
        InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

void DrawSetPoint2SelectedPinNext ()
{
    int c = DrawGetCountOfSelectedPins()-1;
    if (g_point2selectedpin>=c)
        return;
    ++g_point2selectedpin;
    if (!DrawSetOpbjectInfoOfSelectedTestpin())
        InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}

void DrawSetPoint2SelectedPinPrevious ()
{
    if (g_point2selectedpin==0)
        return;
    --g_point2selectedpin;
    if (!DrawSetOpbjectInfoOfSelectedTestpin())
        InvalidateRect (mydraw.hwnd_This, NULL, TRUE);
}


BOOL DrawIsHilitedTPMoreThanOne ()
{
    if (sdata_jumlahdata(&g_sdHLiteTP)>1)
        return TRUE;
    return FALSE;
}

