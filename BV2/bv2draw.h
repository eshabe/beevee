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

#ifndef _SHB_BV2DRAW_H_
#define _SHB_BV2DRAW_H_

#include <windows.h>
#include "../sdata_c/sdata.h"
#include "../bv2data/sboard.h"

#ifdef  __cplusplus
extern "C" {
#endif

HWND CreateDrawWindow (HINSTANCE hInst, HWND hWndParent);
HWND GetDrawWindow ();
void MoveDrawWindow ();
void DrawResetColors ();

BOOL Draw_IsShowTrace ();
BOOL Draw_IsShowTopTrace ();
BOOL Draw_IsShowBottomTrace ();
BOOL Draw_IsShowInnerTrace ();
BOOL Draw_SetShowTrace (BOOL bShow);
BOOL Draw_IsShowTopDevice ();
BOOL Draw_SetShowTopDevice(BOOL bShow);
BOOL Draw_IsShowBotDevice ();
BOOL Draw_SetShowBotDevice(BOOL bShow);
BOOL Draw_IsShowTopTP ();
BOOL Draw_SetShowTopTP (BOOL bShow);
BOOL Draw_IsShowBotTP ();
BOOL Draw_SetShowBotTP (BOOL bShow);
void Draw_ShowDevicesOnly ();
void Draw_ShowTestpinsOnly ();
void Draw_ShowDTAll ();
BOOL Draw_IsShowConnectionGuide ();
BOOL Draw_SetShowConnectionGuide (BOOL bShow);


BOOL DrawSetScaleIndex (int index);
void DrawInitScrollBars ();
void DrawDoIncrScaleIndex ();
void DrawDoDecrScaleIndex ();
void DrawSetDefaultAndDraw ();
void DrawDoScaleOne ();
void DrawFindComponentHandler (_sdataptr skomptr);
void DrawFindSignalHandler (_sdataptr snode);
void DrawFindProbeHandler (_sdataptr tp_ptr, char *probe);
void DrawZoomToFit ();
void DrawZoomToThisRect (int x1, int y1, int x2, int y2);
void DrawZoomToDeviceHilite ();
BOOL IsDrawHasHilitedDevice ();
BOOL DrawIsHilitedTPMoreThanOne (); /* shb: 27Agu07 */

BOOL DrawIsHasHLTrace ();
void DrawLockHLTrace (BOOL add);
BOOL DrawIsHasLockedTrace ();
void DrawClearLockedTrace ();

void DrawFlipAccessories (int x, int y);
void DrawRotateAccessories (int angle);
void DrawMoveAccessories (int dx, int dy);

BOOL DrawSetCHLiteDev (char *name, BOOL IsCenter);
BOOL DrawGetHLiteDev (komponen *kptr); //2.0.4

int DrawGetPoint2SelectedPin (); // 2.1
int DrawGetCountOfSelectedPins ();
void DrawSetPoint2SelectedPin1st ();
void DrawSetPoint2SelectedPinLast ();
void DrawSetPoint2SelectedPinNext ();
void DrawSetPoint2SelectedPinPrevious ();

void DrawMoveThisPtToCenterOfScreen (int ocx, int ocy);
void DrawClearSlection();

#ifdef  __cplusplus
}
#endif

#endif
