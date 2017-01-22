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
BV2.c
Sihabul Milah
*/

/* 16 Oktober 2007
    Melanjutkan Menu Tools...
*/
/*  11 September 2007
    Ubah -> FileCloseHandle (hWnd, bAsk)
    Tambah bomatr_baca_file
*/

/* 6 September 2006 (20070906)
/*
    Perbaiki urutan (close-open) Open Recent (lagi... :)
*/

/* 3 September 2007 (20070903)
    Perbaiki urutan (close-open) Open Recent
    (akibat dari pengalihan FileCloseHandle)
    Aktipkan berubahan ukuran open/save dialog
*/

/* 23 Agustus 2007
    Perbaiki masalah resize
    Tambah penanganan jendela
    Tambah Click-Drag->Zoom
    Tambah DblClick->MoveTo
    Tambah MouseWheel -> Zoom (WM_MOUSEWHEEL)
*/

/* 6 Agustus 2007 */
/*
    2.1
    Perbaiki bug: pembacaan probe dari file fixture, alokasi memori problem
    -> ganti format data ke 2.1
    -> info pin ada di testpoint, tidak lagi di node (signal)
    Penambahan 'punya*' dlm struktur board
    Penambahan import PAD-POWERPCB (ASC) -- in progress!!
    Penggantian email kontak ke eshabe@gmail.com
    Perbaiki pencarian probe

    Baru saja ditemukan :) Sebelum Open dialog, desktop window ter-refresh.
*/

/* 29 Jun 2007 */
/*
    Perbaiki bug: hasil perbandingan.
    Set connection dlg setelah "Find" berhasil
    Tambah FindConnection
    't' opsi FindEx dari FindConnection
    Tambah Zoom shortcuts
*/

/* 28 Jun 2007 */
/*
    Tambah ID_FILE_COMPAREDEVICES - DoFileCompareDevices
    Tambah pilihan Top/Bottom/Both dlm export placement
    Tambah "Refdes Only" dlm export placement
    Tambah import GenRad228x tac+brd
*/

/* 25 Jun 2007 */
/*
open recent file, tambah "new window"
*/

/* 21 Feb 2007 */
/*
editbox
accelerator bisa jalan saat child window aktip, set di TranslateAccelerator
end space di findbar
*/

/* 19 Feb 2007 */
/* LoadEx:
sx namfile --> set nama file
lx --> load
*/

/* 12 Des 2006 */
/* Unicam PCB */

/* 7 Des 2006
* Fabmaster.....
* Gabungkan fungsi MergeTraces
* Tambah MergeDevices
*/

/* 9 Okt 2006
* hilite obejct dari external command (BeeVee h)
*/

/* 5 Okt 2006
* diketahui problem:
- Status Bar tidak update di Win2K (sptnya semua CHILD window dech!)
  >>solusi>> ganti ke common control :)
  >> ObjectBar -> MoveWindow (..,TRUE)
* Merge/gabung data:
- gencad trace (dan turunannya), sudah ditest dg data genrad (ok).
*/

/* 3 Okt 2006
* diketahui problem:
- warna-i tidak terset dengan baik jika sudah ada reg "eshabe\BeeVee"
  >>solusi>> tambah "opt"
- Object Bar tidak terupdate setelah di minimize dlm Win2K
- tidak jalan di Win98
- mebedakan data trace inner(0),top(1),bottom(2) -> D7
*/

#include <windows.h>
#include "resource.h"
#include <commdlg.h>
#include <commctrl.h>

#include "../bv2data/beeveefile.h"
#include "../bv2data/ag3070file.h"
#include "../bv2data/agsj50file.h"
#include "../bv2data/genrad228xfile.h"
#include "../bv2data/generalxy.h"
#include "../bv2data/gencadfile.h"
#include "../bv2data/fabfile.h"
#include "../bv2data/unicampcb.h"
#include "../bv2data/ppcbfile.h"
#include "../bv2data/export.h"
#include "../bv2data/cmpdevs.h"

#include "BV2.h"
#include "toolbox.h"
#include "bv2draw.h"
#include "bv2toolbar.h"
#include "bv2object.h"
#include "bv2find.h"
#include "statusbar.h"
#include "bv2import.h"
#include "bv2option.h"
#include "bv2connect.h"
#include "bv2recents.h"
#include "bv2placement.h"
#include "bv2mergedlg.h"
#include "bv2explacement.h"
#include "bv2loadex.h"
#include "bv2devicendlg.h"

#define MAX_LOADSTRING 100

char sourceTXT[][32]={
                           "",
                           "beevee1",
                           "agilent3070brd",
                           "agilent3070fix+",
                           "agilentsj50",
                           "genrad228x",
                           "genrad228x+",
                           "generalxy",
                           "gencad",
                           "genrad228brd",
                           "placement",
                           "fabmaster_fab", /* x0B */
                           "unicampcb",
                           "genrad228xtac",
                           "genrad228x+",
                           "padpowerpcbasc", /* x0F */
                           "bomattribute"
                       };
int g_iCADOrigin;
BOOL g_bHasReadMeFile;


// Global Variables:
HINSTANCE hInst;        // current instance
HWND hWndMain;
TCHAR szTitle[MAX_LOADSTRING];   // The title bar text
TCHAR szWindowClass[MAX_LOADSTRING]; // The title bar text
char FindStrEx[64];

board g_Board;
char g_boardFile[256];
BOOL g_dataBagus;
BOOL g_bolehSimpan;
BOOL g_sudahBV2;
/* BOOL g_mergeGenCAD; * buang * */

// Foward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CopyingThisApp(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void DoImportFile(HWND hWnd, int source);
void SetBVWindowTitle (HWND hWnd);

/* MENU handlers */
void FindMenuHandler (HWND hWnd, int findType);
void FileOpenHandle (HWND hWnd);
void FileImportHandle (HWND hWnd);
void FileSaveAsHandle (HWND hWnd);
void FileSaveHandle (HWND hWnd);
UINT FileCloseHandle (HWND hWnd,BOOL bAsk);
void FileOpenRecentHandle (HWND hWnd);
void FileMergeHandle (HWND hWnd);
void DoFileMergeTraces (HWND hWnd); /* was  DoFileMergeGenCADTrace */
/* void DoFileMergeBeeVee2GenCADTrace (HWND hWnd); * buang * */
void DoFileMergeDevices (HWND hWnd);
void FileExportPlacementHandle (HWND hWnd); // 2.0.5
void FileNewWindowHandle (HINSTANCE,HWND);
void DoFileCompareDevices (HWND hWnd, int mode); // 2.0.5

/* VIEW */
void ViewRotate90CWHandle();
void ViewRotate90CCWHandle();
void ViewFlipHorizontal ();
void ViewFlipVertical ();

/* TOOLS */
void ToolsMovedByExternalReferenceHandler (HINSTANCE hInst, HWND hWnd);
void ToolsBoardOriginLowerLeftHandler ();
void ToolsOffsetHandler (HINSTANCE hInst, HWND hWnd);

void UpdateMenuItems (HWND hWnd);
void LoadBeeVee2File (HWND hWnd, LPCSTR filename);
void LoadBeeVee1XFile (HWND hWnd, char *filename);

/* properties dlg */
LRESULT BV2PropertiesDlg (HINSTANCE hInst, HWND hWnd);

int BV2FindEx (LPSTR lpszArgument);
void DoCloseFilePlease ();

#ifdef _DEBUG
#include "../slog_c/slog.h"
    slog mylog;
#endif

HWND GettBV2MainWindow ()
{
    return hWndMain;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;
    int len; /* filename len */

#ifdef _DEBUG
    slog_initbersih (&mylog,"beevee-debug.log","BeeVee2.1"); // eshabe: [2010-06-12] just 2.1 :)
    slog_tulis (&mylog, "lpCmdLine");
    slog_tulis (&mylog, lpCmdLine);
#endif
    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_BV2, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    g_dataBagus = FALSE;
    g_bolehSimpan = FALSE;
    g_sudahBV2 = FALSE;
    g_bHasReadMeFile = FALSE;

    ZeroMemory (FindStrEx, 64);

    board_init (&g_Board); // init BOARD data
    g_boardFile[0] = 0;
    if (lpCmdLine[0])
    {
        if (BV2FindEx(lpCmdLine)>=-1) /* was >=0 */
            return 0;
        else if (BV2LoadEx(lpCmdLine))
            return 0;
        strcpy (g_boardFile, lpCmdLine);
        RemoveChar (g_boardFile, '"');
    }

    FindClearStruct();
    BV2OptionDefault();
    ClearObjectInfoText();
    SetFindMode (BV2Option_IsEnableFindBar()?1:0);
    SetObjectInfoMode (BV2Option_IsEnableObjectInfoBar()?1:0);
    ExPlacementInit ();

    // Perform application initialization:
    InitCommonControls();
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    WriteBV2AppSettings ();

    char mypath[256];
    FILE *frf;
    GetModuleFileName (GetModuleHandle(0), mypath, 256);
    strcpy (&mypath[strlen(mypath)-3],"htm");
    if ((frf=fopen(mypath,"r"))!=NULL)
    {
        fclose (frf);
        g_bHasReadMeFile = TRUE;
    }

   hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_BV2);

    SetBVWindowTitle(hWndMain);
    UpdateMenuItems (hWndMain);

    if (BV2Option_IsShowObjectInfo())
    {
        ShowObjectInfo(hInstance, hWndMain);
        MoveDrawWindow ();
        UpdateWindow(hWndMain);
    }

    //MessageBox (NULL, g_boardFile, NULL, 0);
    if ( (len=strlen(g_boardFile))>3)
    {
        char ext[4];

        strcpy (ext, &g_boardFile[len-3]);
        ext[3]=0;
        UpperText(ext);
        if (strcmp(ext,"BV2")==0)
            LoadBeeVee2File (hWndMain, g_boardFile);
        else if (strcmp(ext,"BEV")==0)
            LoadBeeVee1XFile (hWndMain, g_boardFile);
        else
        {
            g_boardFile[0]=ext[0]=0;
        }

        if (ext[0])
        {
            SetBVWindowTitle(hWndMain);
            UpdateMenuItems (hWndMain);
        }
    }

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
 //       if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        if (!TranslateAccelerator(hWndMain, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    //sdata_hapussemua(&gdatImport);
    board_hapus (&g_Board);
    BV2Conn_Erase();
#ifdef _DEBUG
    slog_tulis (&mylog, "EXIT");
#endif
    return msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style   = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)WndProc;
    wcex.cbClsExtra  = 0;
    wcex.cbWndExtra  = 0;
    wcex.hInstance  = hInstance;
    wcex.hIcon   = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BV2));
    wcex.hCursor  = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ARROW));
    wcex.hbrBackground = (HBRUSH)(0);
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_BV2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm  = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
    RECT r,dr,or;
    BOOL isMax = FALSE;
    HWND oldWnd;

#ifdef _DEBUG
    slog_tulis (&mylog, "InitInstance");
#endif

    hInst = hInstance; // Store instance handle in our global variable

    SetRectEmpty (&r);
    if (!LoadBV2WindowSettings(&r, &isMax, "main"))
        hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    else
    {
         GetWindowRect (GetDesktopWindow(), &dr);
         if ((oldWnd=FindWindow (BV2_WINCLASSNAME, NULL))!=NULL)
         {
             GetWindowRect (oldWnd, &or);
             CopyRect (&r, &or);
             OffsetRect (&r, 20, 20);
         }
         if (r.top>dr.bottom-100 || r.top<0 || r.left>dr.right-100 || r.left<0)
         {
            r.bottom -= r.top;
            r.top = 0;
            r.right -= r.left;
            r.left = 0;
         }
         hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                        r.left, r.top, (r.right-r.left), (r.bottom-r.top), NULL, NULL, hInstance, NULL);
    }

    if (!hWnd)
    {
        MessageBox (NULL, "Can not initialize window!", szTitle, MB_OK | MB_ICONERROR);
        return FALSE;
    }

    hWndMain = hWnd;

#ifdef _BETAVERSION
    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)About);
#endif

    (void) CreateToolBar (hInst, hWnd);
    CreateDrawWindow (hInst, hWnd);
    CreateStatusBar (hInst, hWnd);
    BV2Conn_InitWindow (hInst, hWnd);

    if (isMax)
        ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    else
        ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    char szStatusTxt[MAX_LOADSTRING];

    switch (message)
    {
        case WM_MENUSELECT:
        wmId    = LOWORD(wParam);
        LoadString (hInst, wmId, szStatusTxt, MAX_LOADSTRING);
        strtok(szStatusTxt, "\n");
        SetStatusBarText(szStatusTxt);
        break;

        case WM_INITMENUPOPUP:
        UpdateMenuItems(hWnd);
        break;

        case WM_NOTIFY:
        if (((LPNMHDR)lParam)->code == TTN_GETDISPINFO)
            ToolBarNotifyHandler (lParam);
        else
            return DefWindowProc(hWnd, message, wParam, lParam);
        break;

        case WM_COMMAND:
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);


        // Parse the menu selections:
        switch (wmId)
        {
            case ID_FILE_NEW:
            FileNewWindowHandle (hInst, hWnd);
            break;

            case ID_FILE_OPEN:
            FileOpenHandle(hWnd);
            //SetBVWindowTitle(hWnd);
            break;

            case ID_FILE_OPENRECENT:
            FileOpenRecentHandle(hWnd);
            break;

            case ID_FILE_CLOSE:
            FileCloseHandle (hWnd, FALSE);
            break;

            case ID_FILE_IMPORT:
            FileImportHandle(hWnd);
            break;

            case ID_FILE_EXPLACEMENT:
            FileExportPlacementHandle (hWnd);
            break;

            case ID_FILE_MERGE:
            FileMergeHandle (hWnd);
            break;

            case ID_FILE_COMPAREDEVICES:
            DoFileCompareDevices (hWnd,0);
            break;

            case ID_FILE_COMPAREDEVICESINFO:
            DoFileCompareDevices (hWnd,1);
            break;

            case ID_FILE_COMPAREDEVICESPLACE:
            DoFileCompareDevices (hWnd,2);
            //MessageBox (hWnd, "Under development!", BV2_MYTITLE, MB_OK|MB_ICONSTOP);
            break;

            case ID_FILE_PROPERTIES:
            if (BV2PropertiesDlg(hInst, hWnd)==IDOK)
            {
                SetBVWindowTitle(hWnd);
                g_bolehSimpan = TRUE;
            }
            break;

            case ID_FILE_SAVE:
            FileSaveHandle (hWnd);
            break;

            case ID_FILE_SAVE_AS:
            FileSaveAsHandle (hWnd);
            break;

            case ID_VIEW_ROTATE90CW:
            ViewRotate90CWHandle();
            break;
            case ID_VIEW_ROTATE90CCW:
            ViewRotate90CCWHandle();
            break;
            case ID_VIEW_FLIPHORIZONTAL:
            ViewFlipHorizontal();
            break;
            case ID_VIEW_FLIPVERTICAL:
            ViewFlipVertical();
            break;

            case ID_VIEW_ACTUALSIZE:
            DrawDoScaleOne ();
            break;

            case ID_VIEW_RESET:
            DrawSetDefaultAndDraw();
            break;

            case ID_VIEW_ZOOMIN:
            DrawDoIncrScaleIndex();
            break;

            case ID_VIEW_ZOOMOUT:
            DrawDoDecrScaleIndex();
            break;

            case ID_VIEW_ZOOMTOFIT:
            DrawZoomToFit();
            break;

            case ID_VIEW_ZOOMTOSELECTION:
            DrawZoomToDeviceHilite();
            break;

            case ID_VIEW_TOPDEVICES:
            Draw_SetShowTopDevice (!Draw_IsShowTopDevice());
            break;

            case ID_VIEW_BOTTOMDEVICES:
            Draw_SetShowBotDevice (!Draw_IsShowBotDevice());
            break;

            case ID_VIEW_TOPTESTPINS:
            Draw_SetShowTopTP (!Draw_IsShowTopTP());
            break;

            case ID_VIEW_BOTTOMTESTPINS:
            Draw_SetShowBotTP (!Draw_IsShowBotTP());
            break;

            case ID_VIEW_TRACES:
            Draw_SetShowTrace (!Draw_IsShowTrace());
            break;
            case ID_VIEW_TRACESTOP:
            Draw_SetShowTopTrace (!Draw_IsShowTopTrace());
            break;
            case ID_VIEW_TRACESBOT:
            Draw_SetShowBottomTrace (!Draw_IsShowBottomTrace());
            break;
            case ID_VIEW_TRACESIN:
            Draw_SetShowInnerTrace (!Draw_IsShowInnerTrace());
            break;
            case ID_VIEW_CONNGUIDE:
            Draw_SetShowConnectionGuide (!Draw_IsShowConnectionGuide());
            break;

            case ID_VIEW_DEVICESTESTPINS:
            Draw_ShowDTAll();
            break;

            case ID_VIEW_DEVICES:
            Draw_ShowDevicesOnly();
            break;

            case ID_VIEW_TESTPINS:
            Draw_ShowTestpinsOnly();
            break;

            case ID_TOOL_OBJECTINFO:
            //ShowObjectInfo (hInst, hWnd);
            ShowHideObjectInfo (hInst, hWnd);
            break;

            case ID_TOOLS_EDIT:
            if (BV2EditDeviceDlg (hInst, hWnd))
                 g_bolehSimpan = TRUE;
            break;

            case ID_FIND_DEVICE:
            FindMenuHandler (hWnd, find_Device);
            break;
            case ID_FIND_SIGNAL:
            FindMenuHandler (hWnd, find_Signal);
            break;
            case ID_FIND_NODE:
            FindMenuHandler (hWnd, find_Node);
            break;
            case ID_FIND_NAIL:
            FindMenuHandler (hWnd, find_Nail);
            break;
            case ID_FIND_PROBE:
            FindMenuHandler (hWnd, find_Probe);
            break;
            case ID_DOFIND:
            FindMenuHandler (hWnd, -1);
            break;
            case ID_FIND_CONNECTION:
            BV2FindConnectionDlg (hInst, hWnd);
            break;

            case ID_FX_COMMAND:
            {
                int l = strlen(FindStrEx);
                if (l==63)
                    break;
                FindStrEx[l] = (char)lParam;
                FindStrEx[l+1] = 0;
                break;
            }

            case ID_FX_DEVICE:
            SetFindType(find_Device);
            FindComponent (hWnd,strupr(FindStrEx));
            ZeroMemory (FindStrEx, 64);
            break;
            case ID_FX_SIGNAL:
            SetFindType(find_Signal);
            FindSignal (hWnd, strupr(FindStrEx));
            ZeroMemory (FindStrEx, 64);
            break;
            case ID_FX_NODE:
            SetFindType(find_Node);
            FindNode (hWnd, strupr(FindStrEx));
            ZeroMemory (FindStrEx, 64);
            break;
            case ID_FX_NAIL:
            SetFindType(find_Nail);
            FindNode (hWnd, strupr(FindStrEx));
            ZeroMemory (FindStrEx, 64);
            break;
            case ID_FX_PROBE:
            SetFindType(find_Probe);
            FindProbe (hWnd, strupr(FindStrEx));
            ZeroMemory (FindStrEx, 64);
            break;
            case ID_FX_CONNECTION:
            SetFindType(find_Probe);
            FindConnection (hWnd, strupr(FindStrEx));
            ZeroMemory (FindStrEx, 64);
            break;


            case ID_TOOL_LOCK:
            DrawLockHLTrace(FALSE);
            break;

            case ID_TOOL_LOCKADD:
            DrawLockHLTrace(TRUE);
            break;

            case ID_TOOL_CLEAR:
            DrawClearLockedTrace ();
            break;

            case ID_TOOLS_CONNECTION:
            BV2Conn_ShowWindow();
            break;

            case ID_TOOLS_OPTIONS:
            if (BV2OptionDlg (hInst, hWnd)==IDOK)
            {
                SetFindMode (BV2Option_IsEnableFindBar());
                SetObjectInfoMode (BV2Option_IsEnableObjectInfoBar());
            }
            DrawResetColors ();
            EraseWindow (GetDrawWindow());
            break;

            case ID_TOOLS_MOVEPT2TP0:
            DrawSetPoint2SelectedPin1st();
            break;

            case ID_TOOLS_MOVEPT2TP9:
            DrawSetPoint2SelectedPinLast();
            break;

            case ID_TOOLS_MOVEPT2NEXT:
            DrawSetPoint2SelectedPinNext();
            break;

            case ID_TOOLS_MOVEPT2PREV:
            DrawSetPoint2SelectedPinPrevious();
            break;

            case ID_TOOLS_BOARDMOVE_EXTREF:
            if (g_dataBagus)
                ToolsMovedByExternalReferenceHandler(hInst, hWnd);
            break;

            case ID_TOOLS_BOARDORIGIN_LL:
            if (g_dataBagus)
                ToolsBoardOriginLowerLeftHandler();
            break;

            case ID_TOOLS_OFFSET:
            if (g_dataBagus)
                ToolsOffsetHandler (hInst, hWnd);
            break;

            case ID_LOADEX:
            ReadLoadExFileFromReg (g_boardFile);
            DoCloseFilePlease (); /* shb-20070906 */
            LoadBeeVee2File(hWnd, g_boardFile);
            break;

            case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)About);
            break;

            case IDM_COPYING:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_COPYING), hWnd, (DLGPROC)CopyingThisApp);
            break;

            case IDM_README:
            {
                char mypath[256];
                GetModuleFileName (GetModuleHandle(0), mypath, 256);
                strcpy (&mypath[strlen(mypath)-3],"htm");
                //MessageBox (hWnd, mypath, NULL, 0);
                ShellExecute (NULL, "open",mypath, NULL, NULL, SW_SHOW);
            }
            break;

            case IDM_CREDITS:
                MessageBox (hWnd, "- Nanang, Jumali -- BeeVee 1.x\n\
- Cholis, Didik, Sofyan, Nadhor, Andik -- BeeVee 2.x\n\
- Francisco Sanchez (2.1.1), Pantas (2.1.2) -- sample (problem) files\n\
- Burhan, Fathur, Dhani", "Credits", MB_ICONINFORMATION);
            break;

            case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

            default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        UpdateMenuItems(hWnd);
        //SetActiveWindow (hWnd);
        SetStatusBarTextDef();
        break;

        case WM_SIZE:
        MoveToolBar ();
        MoveStatusBar();
        MoveObjectInfoBar ();
        MoveFindBar();
        MoveDrawWindow ();
        SetStatusBarTextDef();
       break;

       case WM_MOUSEWHEEL:
            if (g_dataBagus)
            {
                if (HIWORD(wParam)>WHEEL_DELTA)
                {
                   DrawDoIncrScaleIndex();
                }
                else
                {
                   DrawDoDecrScaleIndex();
                }
            }
            break;

      case WM_CLOSE:
        if (g_bolehSimpan)
            if (MessageBox(hWnd,"The current data is not saved yet.\nAre you sure to exit?", "BeeVee", MB_YESNO | MB_ICONQUESTION)==IDNO)
                break;
        WriteBV2WindowSettings (hWnd, "main");
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;

        case WM_DESTROY:
        PostQuitMessage(0);
        break;

        default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND ctrl = GetDlgItem (hDlg, IDC_THANK);

    switch (message)
    {
        case WM_INITDIALOG:
#ifdef _BETAVERSION

        SetTimer (hDlg, 1001, 500, NULL);
#endif

        return TRUE;


#ifdef _BETAVERSION

        case WM_TIMER:
        EnableWindow(ctrl, !IsWindowEnabled(ctrl));
        break;
#endif

        case WM_COMMAND:
#ifdef _BETAVERSION

        KillTimer (hDlg, 1001);
#endif

        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}


void FindMenuHandler (HWND hWnd, int findType)
{
    FindShowDialog (hWnd, findType);
}

LRESULT CALLBACK OpenFileDlgHook(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int px = -1, py =-1;
    HWND prWnd = GetParent (hDlg);
    HWND pr2Wnd = GetParent (prWnd);
    RECT pr, pr2;

    switch(message)
    {
        case WM_INITDIALOG:
        GetWindowRect (prWnd, &pr);
        if (px<0 || py<0)
        {
            int w,h;

            GetWindowRect (pr2Wnd, &pr2);
            w = pr2.right-pr2.left;
            h = pr2.bottom-pr2.top;
            MoveWindow (prWnd,
                        pr2.left+w/2-(pr.right-pr.left)/2,
                        pr2.top+h/2-(pr.bottom-pr.top)/2,
                        pr.right-pr.left,
                        pr.bottom-pr.top,
                        FALSE);
        }
        else
            MoveWindow (prWnd,
                        px,
                        py,
                        pr.right-pr.left,
                        pr.bottom-pr.top,
                        FALSE);
        return TRUE;

        case WM_CLOSE:
        GetWindowRect (prWnd, &pr);
        px = pr.left;
        py = pr.top;
        break;

    }
    return FALSE;
}

BOOL OpenFileDlg (HWND hWnd, LPSTR filename, LPCSTR prompt, LPCSTR filter, int filter_index, BOOL save)
{
    OPENFILENAME ofn;
    char szFile[300];


    strcpy (szFile, filename);

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrTitle = prompt;
    //
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not
    // use the contents of szFile to initialize itself.
    //
    if (!save)
        ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = filter_index;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    /* shb-20070903
    add OFN_ENABLESIZING */
    ofn.Flags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;
    ofn.lpfnHook = (LPOFNHOOKPROC)OpenFileDlgHook;
    //ofn.Flags =  OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    if (save)
    {
        if (GetSaveFileName(&ofn))
        {
            strcpy (filename, ofn.lpstrFile);
            return TRUE;
        }
    }
    else if (GetOpenFileName(&ofn))
    {
        strcpy (filename, ofn.lpstrFile);
        return TRUE;
    }
    return FALSE;
}

void LoadBeeVee2File (HWND hWnd, LPCSTR filename)
{
    char tempmsg[300];
    int ret;

    sprintf (tempmsg, "Reading %s...", filename);
    SetStatusBarText (tempmsg);
    ret = beevee_baca_bv2(filename, &g_Board);
    if (!ret)
    {
        board_hapus(&g_Board);

        SetStatusBarText ("Reading is error!");
        MessageBox (hWnd, filename, "Reading Error", MB_OK|MB_ICONSTOP);
        SetStatusBarTextDef ();
    }
    else
    {
        if (ret == 2)
            MessageBox (hWnd, "You just read an older version of BeeVee file.\nSome informations about testpins may be lost." , "Old file format", MB_OK|MB_ICONWARNING);
        //board_hitung_ataskiri (&g_Board);
        strcpy (g_boardFile, filename);
        g_dataBagus = TRUE;
        g_sudahBV2 = TRUE;
        if (beevee_bv2_perlukoreksi()!=0)
            g_bolehSimpan = TRUE;

        DrawSetDefaultAndDraw();  /* clear draw */
        //FindClearStruct(); /* clear find */
        //ClearObjectInfoText();
        SetStatusBarText ("Reading is successful!");
        AddRecentFile (g_boardFile);
    }
}

void FileOpenHandle (HWND hWnd)
{
    char szFile[256];

    if ( OpenFileDlg ( hWnd,
                       szFile,
                       NULL,
                       "All files (*.*)\0*.*\0BeeVee 2.x file (*.bv2)\0*.bv2\0",
                       2,
                       FALSE) )
    {
        if (!FileCloseHandle(hWnd, TRUE))
            return;
        LoadBeeVee2File (hWnd, szFile);
        SetBVWindowTitle(hWnd);
    }
}

void FileOpenRecentHandle (HWND hWnd)
{
   // if (!FileCloseHandle(hWnd)) /* shb-20070903 */
  //      return;

  //  InvalidateRect (hWnd, NULL, FALSE);

    char szFile[256];
    if (GetRecentFile (hInst, hWnd, szFile)==IDOK)
    {
        if (szFile[0])
        {
            //g_dataBagus = FALSE;
            //board_hapus(&g_Board);
            // 2.0.5
            if (GetRecentFileIsNewWindow())
            {
                char myapp[256];
                GetModuleFileName (GetModuleHandle(0), myapp, 255);
                ShellExecute (NULL, NULL, myapp, szFile, NULL, SW_SHOWNORMAL );
            }
            // -end
            else
            {
                if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
                    return;

                LoadBeeVee2File (hWnd, szFile);
                SetBVWindowTitle(hWnd);
            }
        }
    }
}

void FileSaveAsHandle (HWND hWnd)
{
    char szFile[256];
    char tempmsg[300];
    char ext[4];
    int len;

    board_nama(&g_Board, szFile);
    strcat (szFile, ".bv2");
    if ( OpenFileDlg ( hWnd,
                       szFile,
                       NULL,
                       "All files (*.*)\0*.*\0BeeVee 2.x file (*.bv2)\0*.bv2\0",
                       2,
                       TRUE ) )
    {
        if ( (len=strlen(szFile))>4)
        {
            strcpy (ext,&szFile[len-4]);
            if (ext[0]!='.')
                strcat(szFile,".bv2");
        }
        sprintf (tempmsg, "Saving %s...", szFile);
        SetStatusBarText (tempmsg);
        if ( !beevee_tulis_bv2(szFile, &g_Board) )
        {
            SetStatusBarText ("Saving is error!");
            MessageBox (hWnd, szFile, "Saving Error", MB_OK|MB_ICONSTOP);
            SetStatusBarTextDef ();
        }
        else
        {
            strcpy (g_boardFile, szFile);
            g_bolehSimpan = FALSE;
            g_sudahBV2 = TRUE;
            SetStatusBarText ("Saving is successful!");
            SetBVWindowTitle(hWnd);
        }

        EraseWindow(hWnd);
    }
}

void FileSaveHandle (HWND hWnd)
{
    if (g_sudahBV2)
    {
        SetStatusBarText ("Saving....");
        if (!beevee_tulis_bv2(g_boardFile, &g_Board))
        {
            MessageBox (hWnd, g_boardFile, "Saving Error", MB_OK|MB_ICONSTOP);
        }
        else
        {
            g_bolehSimpan = FALSE;
            SetStatusBarText ("Saving is successful!");
        }
    }
    else
        FileSaveAsHandle (hWnd);
}

int EraseWindow (HWND hWnd)
{
    RECT r;
    GetClientRect(hWnd, &r);
    return InvalidateRect(hWnd, &r, TRUE);
}

int RepaintWindow (HWND hWnd)
{
    RECT r;
    GetClientRect(hWnd, &r);
    return InvalidateRect(hWnd, &r, FALSE);
}


void LoadBeeVee1XFile (HWND hWnd, char *filename)
{
    char tempmsg[300];

    g_dataBagus = FALSE;
    board_hapus(&g_Board);
    board_isiasal(&g_Board, sourceTXT[IMPORT_BEEVEE1]);

    sprintf (tempmsg, "Reading %s...", filename);
    SetStatusBarText (tempmsg);
    if (!beevee_baca_bev(filename, &g_Board))
    {
        board_hapus(&g_Board);

        SetStatusBarText ("Reading is error!");
        MessageBox (hWnd, filename, "Reading Error", MB_OK|MB_ICONSTOP);
        SetStatusBarTextDef ();
    }
    else
    {
        //board_hitung_ataskiri (&g_Board);
        strcpy (g_boardFile, filename);
        g_dataBagus = TRUE;
        g_sudahBV2 = FALSE;

        if (g_Board.punya_nodepin)
        {
            sprintf(tempmsg, "%s-%s",sourceTXT[IMPORT_GENRAD228X_A],sourceTXT[IMPORT_BEEVEE1]);
            board_isiasal(&g_Board, tempmsg);
        }
    }
}

void DoImportFileBeeVee1X (HWND hWnd, int source)
{
#pragma unused source
    char szFile[256];

    if (OpenFileDlg(hWnd, szFile, "Import BeeVee 1.x", "All files (*.*)\0*.*\0BeeVee 1.x file (*.bev)\0*.bev\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        LoadBeeVee1XFile (hWnd, szFile);
    }
}

void DoImportFileAgilent3070 (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int read_err = 0;

    if (OpenFileDlg(hWnd, szFile, "Import Agilent3070 \"board\"", "All files (*.*)\0*.*\0",1, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);
        ag3070_pakefixture(0);

        if (source==IMPORT_AGILENT3070_B)
            ag3070_pakefixture(1);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (!ag3070_baca_board(szFile, &g_Board))
            read_err = 1;
        else if (OpenFileDlg(hWnd, szFile, "Import Agilent3070 \"board_xy\"", "All files (*.*)\0*.*\0",1, FALSE))
        {
            sprintf (tempmsg, "Reading %s...", szFile);
            SetStatusBarText (tempmsg);
            if (!ag3070_baca_boardxy(szFile, &g_Board))
                read_err = 1;
            else if (source == IMPORT_AGILENT3070_B)
            {
                SetStatusBarText ("Reading is successful!");
                if (OpenFileDlg(hWnd, szFile, "Import Agilent3070 \"fixture\"", "All files (*.*)\0*.*\0",1, FALSE))
                {
                    sprintf (tempmsg, "Reading %s...", szFile);
                    SetStatusBarText (tempmsg);
                    if (!ag3070_baca_fixture(szFile, &g_Board))
                        read_err = 1;
                    else
                    {
                        board_hitung_ataskiri (&g_Board);
                        strcpy (g_boardFile, szFile);
                        g_sudahBV2 = FALSE;
                    }
                }
            }
            else
            {
                board_hitung_ataskiri (&g_Board);
                strcpy (g_boardFile, szFile);
                g_sudahBV2 = FALSE;
            }
        }

        if (read_err)
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
        else
        {
            g_dataBagus = TRUE;
        }

    }
}

void DoImportFileAgilentSJ50 (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];

    if (OpenFileDlg(hWnd, szFile, "Import Agilent-SJ50", "All files (*.*)\0*.*\0Agilent-SJ50 file (.plx;.pls)\0*.plx;*.pls\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (!agsj50_baca_plx(szFile, &g_Board))
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
        else
        {
            //board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        EraseWindow(hWnd);
    }
}

void DoImportFileGenRad228X (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int read_err = 0;
    int read_ok = 0;

    if (OpenFileDlg(hWnd, szFile, "Import GenRad .tac", "All files (*.*)\0*.*\0TAC file (*.tac)\0*.tac\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);
        if (!genrad_baca_tac(szFile, &g_Board))
            read_err = 1;
        if (!read_err && source != IMPORT_GENRAD228X_TACBRD)
        {
            if (OpenFileDlg(hWnd, szFile, "Import GenRad .ckt", "All files (*.*)\0*.*\0CKT file (*.ckt)\0*.ckt\0",2, FALSE))
            {
                sprintf (tempmsg, "Reading %s...", szFile);
                SetStatusBarText (tempmsg);

                if (!genrad_baca_ckt(szFile, &g_Board))
                    read_err = 1;
                else
                    read_ok = 1;
            }
        }

        if (!read_err && (source == IMPORT_GENRAD228X_B || source == IMPORT_GENRAD228X_TACBRD))
        {
            read_ok = 0;
            if (OpenFileDlg(hWnd, szFile, "Import GenRad .brd", "All files (*.*)\0*.*\0BRD file (*.brd)\0*.brd\0",2, FALSE))
            {
                sprintf (tempmsg, "Reading %s...", szFile);
                SetStatusBarText (tempmsg);

                if (!genrad_baca_brd(szFile, &g_Board))
                    read_err = 1;
                else
                    read_ok = 1;
            }
        }
    }

    if (read_err)
    {
        board_hapus(&g_Board);
        MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        EraseWindow(hWnd);
    }

    if (read_ok)
    {
        board_hitung_ataskiri (&g_Board);
        strcpy (g_boardFile, szFile);
        g_dataBagus = TRUE;
        g_sudahBV2 = FALSE;
    }
}

void DoImportFileGenRad228XBRD (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];

    if (OpenFileDlg(hWnd, szFile, "Import GenRad228X BRD", "All files (*.*)\0*.*\0BRD file (*.brd)\0*.brd\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (!genrad_baca_brd(szFile, &g_Board))
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
        else
        {

            board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
    }
}

void DoImportFileGenRad228XTAC (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];

    if (OpenFileDlg(hWnd, szFile, "Import GenRad228X TAC", "All files (*.*)\0*.*\0TAC file (*.tac)\0*.tac\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (!genrad_baca_tac(szFile, &g_Board))
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
        else
        {

            board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
    }
}

void DoImportFileGeneralXY (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int baca_ok = 1;

    if (OpenFileDlg(hWnd, szFile, "Import General-XY", "All files (*.*)\0*.*\0Text file (*.txt)\0*.txt\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        baca_ok = gene_baca_file (szFile, &g_Board);

        if (baca_ok)
        {
            //board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        else
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
}


void DoImportFileGenCAD (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int baca_ok = 1;

    if (OpenFileDlg(hWnd, szFile, "Import GenCAD", "All files (*.*)\0*.*\0GenCAD file (*.cad)\0*.cad\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        baca_ok = gencad_baca_cad (szFile, &g_Board);

        if (baca_ok)
        {
            //board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        else
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
}

void DoImportPlacement (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int baca_ok = 1;
    importplacement_struct hasil;


    if (OpenFileDlg(hWnd, szFile, "Import Placement Data", "All files (*.*)\0*.*\0Text file (*.txt)\0*.txt\0",2, FALSE))
    {
        if (PlacementDialog (hInst, hWnd)==IDOK)
        {
            ImportPlacement_GetResults (&hasil);
            hasil.unit = Unit_DialogBox (hInst, hWnd);
        }
        else
            return;

        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        baca_ok = place_baca_file (szFile, &g_Board, &hasil);

        if (baca_ok)
        {
            //board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        else
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
}

void DoImportFileFabmasterFAB (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int baca_ok = 1;

    if (OpenFileDlg(hWnd, szFile, "Import Fabmaster", "All files (*.*)\0*.*\0Fabmaster file (*.fab)\0*.fab\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        baca_ok = fabmaster_baca_fab (szFile, &g_Board);

        if (baca_ok)
        {
            //board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        else
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
}

void DoImportFileUnicamPCB (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int baca_ok = 1;

    if (OpenFileDlg(hWnd, szFile, "Import Unicam", "All files (*.*)\0*.*\0Unicam file (*.pcb)\0*.pcb\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        baca_ok = unicampcb_baca_pcb (szFile, &g_Board);

        if (baca_ok)
        {
            //board_hitung_ataskiri (&g_Board);
            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        else
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
}

void DoImportFilePadPowerPCBASC (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int baca_ok = 1;

    if (OpenFileDlg(hWnd, szFile, "Import Pad-PowerPCB", "All files (*.*)\0*.*\0PowerPCB file (*.asc)\0*.asc\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        baca_ok = pads_powerpcb_baca_asc (szFile, &g_Board);

        if (baca_ok)
        {

            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        else
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
}

void DoImportFileBOMAttribute (HWND hWnd, int source)
{
    char szFile[256];
    char tempmsg[300];
    int baca_ok = 1;

    if (OpenFileDlg(hWnd, szFile, "Import BOM/Attribute", "All files (*.*)\0*.*\0BOM/Attribute file (*.atr)\0*.atr\0",2, FALSE))
    {
        if (!FileCloseHandle(hWnd, TRUE)) /* shb-20070906 */
            return;

        board_isiasal(&g_Board, sourceTXT[source]);

        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        baca_ok = bomatr_baca_file (szFile, &g_Board);

        if (baca_ok)
        {

            strcpy (g_boardFile, szFile);
            g_dataBagus = TRUE;
            g_sudahBV2 = FALSE;
        }
        else
        {
            board_hapus(&g_Board);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
}


void DoImportFile(HWND hWnd, int source)
{
    switch (source)
    {
        case IMPORT_BEEVEE1:
        DoImportFileBeeVee1X(hWnd, source);
        break;

        case IMPORT_AGILENT3070_A:
        case IMPORT_AGILENT3070_B:
        DoImportFileAgilent3070 (hWnd, source);
        break;

        case IMPORT_AGILENTSJ50:
        DoImportFileAgilentSJ50 (hWnd, source);
        break;

        case IMPORT_GENRAD228X_A:
        case IMPORT_GENRAD228X_B:
        case IMPORT_GENRAD228X_TACBRD:
        DoImportFileGenRad228X (hWnd, source);
        break;

        case IMPORT_GENRAD228X_BRD:
        DoImportFileGenRad228XBRD (hWnd, source);
        break;

        case IMPORT_GENRAD228X_TAC:
        DoImportFileGenRad228XTAC (hWnd, source);
        break;

        case IMPORT_GENERALXY:
        DoImportFileGeneralXY (hWnd, source);
        break;

        case IMPORT_GENCAD:
        DoImportFileGenCAD (hWnd, source);
        break;

        case IMPORT_PLACEMENT:
        DoImportPlacement (hWnd, source);
        break;

        case IMPORT_FABMASTER_FAB:
        DoImportFileFabmasterFAB (hWnd, source);
        break;

        case IMPORT_UNICAM_PCB:
        DoImportFileUnicamPCB (hWnd, source);
        break;

        case IMPORT_PADPOWERPCB_ASC:
        DoImportFilePadPowerPCBASC (hWnd, source);
        break;

        case IMPORT_BOMATTRIBUTE_ATR:
        DoImportFileBOMAttribute (hWnd, source);
        break;

    }
    SetStatusBarTextDef ();
}

void FileImportHandle (HWND hWnd)
{
    int ret = BV2ImportFile (hInst, hWnd);
    if (ret==IDOK)
    {
        DoImportFile (hWnd, GetImportSource());
        if (g_dataBagus)
        {
            SetBVWindowTitle (hWnd);
            DrawSetDefaultAndDraw();
        }
        g_bolehSimpan = g_dataBagus;
    }
}


void SetBVWindowTitle (HWND hWnd)
{
    char title[300];
    char filename[36];

    if (g_dataBagus)
    {
        board_nama(&g_Board, title);
        strcat (title, " - ");
        if (strlen(g_boardFile)>40)
        {
            strncat(title,g_boardFile,3);
            strcat (title,"...");
            GetTextStartAtText (g_boardFile, filename, 35, "\\", strlen(g_boardFile)-35);
            strcat (title, filename);

        }
        else
            strcat (title, g_boardFile);
        strcat (title, " - ");
        strcat (title, szTitle);
        SetWindowText (hWnd, title);
    }
    else
        SetWindowText (hWnd, szTitle);
}

void ViewRotateBoard (BOOL IsCW)
{
    int a;
    komponen k;

    a = (IsCW)?270:90;
    board_putar (&g_Board, a);
    DrawRotateAccessories (a);
    board_hitung_ataskiri(&g_Board);

    g_bolehSimpan = TRUE;
    //DrawSetDefaultAndDraw();  /* clear draw */
    //FindClearStruct(); /* clear find */
    //ClearObjectInfoText();
    if (g_Board.punya_tempat && DrawGetHLiteDev(&k))
    {
        if (komp_cari (&g_Board.skomp, k.nama, 0)>=0)
        {
            komp_info(&g_Board.skomp, k.info);
            SetObjectInfo ("Device",k.nama, (k.pos_tb=='T')?"TOP":"BOTTOM", k.info);
        }
        else
            ClearObjectInfoText();
    }

    DrawInitScrollBars();
    InvalidateRect (GetDrawWindow(), NULL, TRUE);
}

void ViewRotate90CWHandle()
{
    ViewRotateBoard (TRUE);
}

void ViewRotate90CCWHandle()
{
    ViewRotateBoard (FALSE);
}

void ViewFlipBoard (BOOL IsVertical)
{
    int x,y;
    komponen k;

    x = (IsVertical)?1:-1;
    y = (IsVertical)?-1:1;
    board_cermin (&g_Board, x, y);
    DrawFlipAccessories (x, y);
    board_hitung_ataskiri(&g_Board);

    g_bolehSimpan = TRUE;
    //DrawSetDefaultAndDraw();  /* clear draw */
    //FindClearStruct(); /* clear find */
    //ClearObjectInfoText();
    if (g_Board.punya_tempat && DrawGetHLiteDev(&k))
    {
        if (komp_cari (&g_Board.skomp, k.nama, 0)>=0)
        {
            komp_info(&g_Board.skomp, k.info);
            SetObjectInfo ("Device",k.nama, (k.pos_tb=='T')?"TOP":"BOTTOM", k.info);
        }
        else
            ClearObjectInfoText();
    }

    DrawInitScrollBars();
    InvalidateRect (GetDrawWindow(), NULL, TRUE);
}

void ViewFlipHorizontal ()
{
    ViewFlipBoard (FALSE);
}

void ViewFlipVertical ()
{
    ViewFlipBoard (TRUE);
}

BOOL IsOKToFind (int objectType)
{
    boardptr brdptr = GetBVBoardPtr();
    char asal[64];
    BOOL f_find_Device = FALSE;
    BOOL f_find_Signal = FALSE;
    BOOL f_find_Nail = FALSE;
    BOOL f_find_Node = FALSE;
    BOOL f_find_Probe = FALSE;

    board_asal (brdptr, asal);

    if (sdata_jumlahdata(&brdptr->skomp)>0)
        f_find_Device = TRUE;

    if (sdata_jumlahdata(&brdptr->snode)>0)
    {
        f_find_Signal = TRUE;

        /*
        if (strstr(asal, "genrad228x")!=NULL)
        {
            f_find_Nail = TRUE;
            f_find_Node = TRUE;
        }
        else if (strstr(asal, "agilent3070fix")!=NULL)
            f_find_Probe = TRUE;
        */
        if (brdptr->punya_nailpin)
            f_find_Nail = TRUE;
        if (brdptr->punya_nodepin)
            f_find_Node = TRUE;
        if (brdptr->punya_probe)
            f_find_Probe = TRUE;


    }

    switch (objectType)
    {
        case find_Device:
        return f_find_Device;
        case find_Signal:
        return f_find_Signal;
        case find_Node:
        return f_find_Node;
        case find_Nail:
        return f_find_Nail;
        case find_Probe:
        return f_find_Probe;
        default:
        return (f_find_Device || f_find_Signal || f_find_Node || f_find_Nail || f_find_Probe);
    }
}

void UpdateMenuItems (HWND hWnd)
{
    HMENU hMenu = GetMenu (hWnd);

    if (hMenu==NULL)
        return;

    /* FILE ************/
    EnableMenuItem (hMenu, ID_FILE_CLOSE, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableToolBarButton (ID_FILE_CLOSE, g_dataBagus);

    EnableMenuItem (hMenu, ID_FILE_MERGE, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FILE_SAVE, (g_dataBagus && g_bolehSimpan)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableToolBarButton (ID_FILE_SAVE, (g_dataBagus && g_bolehSimpan));

    EnableMenuItem (hMenu, ID_FILE_SAVE_AS, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FILE_EXPLACEMENT, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FILE_PROPERTIES, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FILE_COMPAREDEVICES, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FILE_COMPAREDEVICESINFO, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FILE_COMPAREDEVICESPLACE, (g_dataBagus && board_punyatempat(&g_Board))?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    /* VIEW *************/
    EnableMenuItem (hMenu, ID_VIEW_ROTATE90CW, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_VIEW_ROTATE90CCW, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_VIEW_FLIPHORIZONTAL, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_VIEW_FLIPVERTICAL, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    EnableMenuItem (hMenu, ID_VIEW_ZOOMIN, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableToolBarButton (ID_VIEW_ZOOMIN, g_dataBagus);

    EnableMenuItem (hMenu, ID_VIEW_ZOOMOUT, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableToolBarButton (ID_VIEW_ZOOMOUT, g_dataBagus);

    EnableMenuItem (hMenu, ID_VIEW_ACTUALSIZE, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableToolBarButton (ID_VIEW_ACTUALSIZE, g_dataBagus);

    EnableMenuItem (hMenu, ID_VIEW_ZOOMTOFIT, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_VIEW_ZOOMTOSELECTION, (IsDrawHasHilitedDevice())?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    CheckMenuItem (hMenu, ID_VIEW_TOPDEVICES, Draw_IsShowTopDevice()?MF_CHECKED:MF_UNCHECKED);
    CheckToolBarButton (ID_VIEW_TOPDEVICES, Draw_IsShowTopDevice());

    CheckMenuItem (hMenu, ID_VIEW_BOTTOMDEVICES, Draw_IsShowBotDevice()?MF_CHECKED:MF_UNCHECKED);
    CheckToolBarButton (ID_VIEW_BOTTOMDEVICES, Draw_IsShowBotDevice());

    CheckMenuItem (hMenu, ID_VIEW_TOPTESTPINS, Draw_IsShowTopTP()?MF_CHECKED:MF_UNCHECKED);
    CheckToolBarButton (ID_VIEW_TOPTESTPINS, Draw_IsShowTopTP());

    CheckMenuItem (hMenu, ID_VIEW_BOTTOMTESTPINS, Draw_IsShowBotTP()?MF_CHECKED:MF_UNCHECKED);
    CheckToolBarButton (ID_VIEW_BOTTOMTESTPINS, Draw_IsShowBotTP());

    CheckMenuItem (hMenu, ID_VIEW_TRACES, Draw_IsShowTrace()?MF_CHECKED:MF_UNCHECKED);
    CheckMenuItem (hMenu, ID_VIEW_TRACESTOP, Draw_IsShowTopTrace()?MF_CHECKED:MF_UNCHECKED);
    CheckMenuItem (hMenu, ID_VIEW_TRACESBOT, Draw_IsShowBottomTrace()?MF_CHECKED:MF_UNCHECKED);
    CheckMenuItem (hMenu, ID_VIEW_TRACESIN, Draw_IsShowInnerTrace()?MF_CHECKED:MF_UNCHECKED);
    EnableMenuItem (hMenu, ID_VIEW_TRACESTOP, (Draw_IsShowTrace())?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_VIEW_TRACESBOT, (Draw_IsShowTrace())?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_VIEW_TRACESIN, (Draw_IsShowTrace())?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    CheckToolBarButton (ID_VIEW_TRACES, Draw_IsShowTrace());
    CheckToolBarButton (ID_VIEW_TRACESTOP, Draw_IsShowTopTrace());
    CheckToolBarButton (ID_VIEW_TRACESBOT, Draw_IsShowBottomTrace());
    EnableToolBarButton (ID_VIEW_TRACESTOP, Draw_IsShowTrace());
    EnableToolBarButton (ID_VIEW_TRACESBOT, Draw_IsShowTrace());

    CheckMenuItem (hMenu, ID_VIEW_CONNGUIDE, Draw_IsShowConnectionGuide()?MF_CHECKED:MF_UNCHECKED);
    CheckToolBarButton (ID_VIEW_CONNGUIDE, Draw_IsShowConnectionGuide());

    /* FIND **************/
    EnableMenuItem (hMenu, ID_FIND_DEVICE, (IsOKToFind(find_Device))?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FIND_SIGNAL, (IsOKToFind(find_Signal))?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FIND_NODE, (IsOKToFind(find_Node))?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FIND_NAIL, (IsOKToFind(find_Nail))?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_FIND_PROBE, (IsOKToFind(find_Probe))?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    EnableMenuItem (hMenu, ID_DOFIND, (IsOKToFind(-1))?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableToolBarButton (ID_DOFIND, IsOKToFind(-1));

    EnableMenuItem (hMenu, ID_FIND_CONNECTION, (sdata_jumlahdata(&g_Board.skonek)>0)?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    /* TOOL */
    EnableMenuItem (hMenu, ID_TOOL_LOCK, (DrawIsHasHLTrace())?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOL_LOCKADD, (DrawIsHasHLTrace())?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOL_CLEAR, (DrawIsHasLockedTrace())?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOLS_CONNECTION, (sdata_jumlahdata(&g_Board.skonek)>0)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableToolBarButton (ID_TOOLS_CONNECTION,  (sdata_jumlahdata(&g_Board.skonek)>0));

    BOOL menuToolMoveEnable = DrawIsHilitedTPMoreThanOne();
    EnableMenuItem (hMenu, ID_TOOLS_MOVEPT2TP0, (menuToolMoveEnable)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOLS_MOVEPT2NEXT, (menuToolMoveEnable)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOLS_MOVEPT2PREV, (menuToolMoveEnable)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOLS_MOVEPT2TP9, (menuToolMoveEnable)?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    EnableMenuItem (hMenu, ID_TOOLS_OFFSET, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOLS_BOARDMOVE_EXTREF, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    EnableMenuItem (hMenu, ID_TOOLS_BOARDORIGIN_LL, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    //EnableMenuItem (hMenu, ID_TOOL_OBJECTINFO, (g_dataBagus)?MF_ENABLED:MF_DISABLED|MF_GRAYED);
    //EnableToolBarButton (ID_TOOL_OBJECTINFO,  g_dataBagus);
    EnableMenuItem (hMenu, ID_TOOLS_EDIT, (IsDrawHasHilitedDevice())?MF_ENABLED:MF_DISABLED|MF_GRAYED);

    /* HELP */
    EnableMenuItem (hMenu, IDM_README, (g_bHasReadMeFile)?MF_ENABLED:MF_DISABLED|MF_GRAYED);

}

boardptr GetBVBoardPtr ()
{
    return (&g_Board);
}

void DoCloseFilePlease () /* shb-20070906 */
{
    g_dataBagus = FALSE;
    g_sudahBV2 = FALSE;
    g_bolehSimpan = FALSE;
    g_boardFile[0] = 0;

    board_hapus(&g_Board);

    DrawSetDefaultAndDraw();
    FindClearStruct();
    ClearObjectInfoText();
    SetStatusBarTextDef();
    BV2Conn_ClearList();
    BV2Conn_CloseWindow();
}

UINT FileCloseHandle (HWND hWnd, BOOL bAsk)
{
    if (g_bolehSimpan)
    {
        if (MessageBox(hWnd,"The current data is NOT saved yet.\nAre you sure to continue?", "BeeVee", MB_YESNO | MB_ICONQUESTION)==IDNO)
            return FALSE;
    }
    else if (g_dataBagus && bAsk)
    {
        if (MessageBox(hWnd,"Do you want to close the current data?", "BeeVee", MB_YESNO | MB_ICONQUESTION)==IDNO)
            return FALSE;
    }

    DoCloseFilePlease (); /* shb-20070906 */
    SetBVWindowTitle (hWnd);

    return TRUE;
}

void WriteBV2AppSettings ()
{

    HKEY hkey;
    char myapp[] = "eshabe.BeeVee.2.1";
    char mypath[256];
    char mykey[256];
    char myvalue[256];
    char myversion[256];

    myversion[0] = 0;
    if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee\\2.1", 0, KEY_READ, &hkey)==ERROR_SUCCESS)
    {
        DWORD type = REG_SZ;
        DWORD size = 256;

        RegQueryValueEx (hkey, "", 0, &type, (LPBYTE)myversion, &size);
        RegCloseKey (hkey);
    }

    GetModuleFileName (GetModuleHandle(0), mypath, 255);
    sprintf (mykey, "%s\\shell\\open\\command", myapp);
    sprintf (myvalue, "%s \"%%1\"", mypath);
    RegCreateKeyEx (HKEY_CLASSES_ROOT, mykey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
    RegSetValueEx (hkey, "", 0, REG_SZ, (LPBYTE)myvalue, strlen(myvalue)+1);
    RegCloseKey (hkey);

    sprintf (mykey, "%s\\DefaultIcon", myapp);
    sprintf (myvalue, "%s,%d", mypath,2);
    RegCreateKeyEx (HKEY_CLASSES_ROOT, mykey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
    RegSetValueEx (hkey, "", 0, REG_SZ, (LPBYTE)myvalue, strlen(myvalue)+1);
    RegCloseKey (hkey);

    RegCreateKeyEx (HKEY_CLASSES_ROOT, ".bv2", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
    RegSetValueEx (hkey, "", 0, REG_SZ, (LPBYTE)myapp, strlen(myapp)+1);
    RegCloseKey (hkey);

    if (strcmp( myversion, BV2_MYVERSION)!=0)
    {
        strcpy (myvalue, BV2_MYVERSION);
        RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee\\2.1", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
        RegSetValueEx (hkey, "", 0, REG_SZ, (LPBYTE)myvalue, strlen(myvalue)+1);
        RegCloseKey (hkey);

        COLORREF crefViaColor = RGB(255,255,0);
       	RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\eshabe\\BeeVee", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);
        RegSetValueEx (hkey, "colorVia", 0, REG_DWORD, (LPBYTE)&crefViaColor, sizeof(DWORD));
        RegCloseKey (hkey);

#ifndef _BETAVERSION
        DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndMain, (DLGPROC)About);
#endif
    }

}

void FileMergeHandle (HWND hWnd)
{
    int mergetype=0;

    if (BV2MergeDlg(hInst, hWnd)!=IDOK)
        return;
    mergetype = GetMergeType();
    switch (mergetype)
    {
        case MERGE_TRACE:
        DoFileMergeTraces (hWnd);
        break;

        case MERGE_DEVICES:
        DoFileMergeDevices (hWnd);
        break;
    }
    SetStatusBarTextDef ();
}

void DoFileMergeTraces (HWND hWnd)
{
    Str63 signal_name;
    board gcbrd;
    char szFile[256];
    char tempmsg[300];
    char title[256];
    int erase;


    strcpy (title, "Merge Traces");

    if (sdata_jumlahdata(&g_Board.strace)>0)
    {
        erase = MessageBox(hWnd, "WARNING! Current data already has traces.\nDo you want to remove the traces?", title, MB_YESNOCANCEL | MB_ICONWARNING);
        if (erase==IDCANCEL)
            return;
        else if (erase==IDYES)
        {
            sdata_hapussemua (&g_Board.strace);
            EraseWindow(hWnd);
            MessageBox (hWnd, "Current traces have been removed!", "Warning:", MB_ICONWARNING);
        }
    }

    SinalNameDlg_DialogBox (hInst, hWnd);
    signal_name[0] = 0;
    GetSinalNameDlg_name (signal_name);
    if (!signal_name[0])
        return;

    sprintf (tempmsg, "Do you want to merge TRACES to current data with \"%s\" as reference?", signal_name);
    if (MessageBox (hWnd, tempmsg, title, MB_YESNO | MB_ICONQUESTION)==IDNO)
        return;

    if (node_cari(&g_Board.snode, signal_name, 0)<0)
    {
        MessageBox (hWnd, "The reference is not in current data!", title, MB_OK | MB_ICONSTOP);
        return;
    }

    board_init (&gcbrd);
    if (OpenFileDlg(hWnd, szFile, "Open BeeVee2", "All files (*.*)\0*.*\0BeeVee 2 file (*.bv2)\0*.bv2\0",2, FALSE))
    {
        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (beevee_baca_bv2 (szFile, &gcbrd))
        {
            SetStatusBarText ("Merge traces...");
            if (DoMergeTrace (&g_Board, &gcbrd, signal_name)<=0)
            {
                MessageBox (hWnd, "Can not merge to the current data!", title, MB_OK | MB_ICONSTOP);
            }
            else
            {
                board_hitung_ataskiri (&g_Board);
                g_bolehSimpan = TRUE;
                DrawSetDefaultAndDraw();  /* clear draw */
                FindClearStruct(); /* clear find */
                ClearObjectInfoText();
            }
        }
        else
        {
            board_hapus(&gcbrd);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
    board_hapus (&gcbrd);
}

void DoFileMergeDevices (HWND hWnd)
{
    Str63 signal_name;
    board gcbrd;
    char szFile[256];
    char tempmsg[300];
    char title[256];
    int baca_ok = 1;
    int erase;


    strcpy (title, "Merge Devices");

    if (sdata_jumlahdata(&g_Board.skomp)>0)
    {
        erase = MessageBox(hWnd, "WARNING! Current data already has devices.\nDo you want to remove the devices?", title, MB_YESNOCANCEL | MB_ICONWARNING);
        if (erase==IDCANCEL)
            return;
        else if (erase==IDYES)
        {
            sdata_hapussemua (&g_Board.skomp);
            EraseWindow(hWnd);
            MessageBox (hWnd, "Current devices have been removed!", "Warning:", MB_ICONWARNING);
        }
    }

    SinalNameDlg_DialogBox (hInst, hWnd);
    signal_name[0] = 0;
    GetSinalNameDlg_name (signal_name);
    if (!signal_name[0])
        return;

    sprintf (tempmsg, "Do you want to merge DEVICES to current data with \"%s\" as reference?", signal_name);
    if (MessageBox (hWnd, tempmsg, title, MB_YESNO | MB_ICONQUESTION)==IDNO)
        return;

    if (node_cari(&g_Board.snode, signal_name, 0)<0)
    {
        MessageBox (hWnd, "The reference is not in current data!", title, MB_OK | MB_ICONSTOP);
        return;
    }

    board_init (&gcbrd);
    if (OpenFileDlg(hWnd, szFile, "Open BeeVee2", "All files (*.*)\0*.*\0BeeVee 2 file (*.bv2)\0*.bv2\0",2, FALSE))
    {
        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (beevee_baca_bv2 (szFile, &gcbrd))
        {
            SetStatusBarText ("Merge devices...");
            if (DoMergeDevices (&g_Board, &gcbrd, signal_name)<=0)
            {
                MessageBox (hWnd, "Can not merge to the current data!", title, MB_OK | MB_ICONSTOP);
            }
            else
            {
                board_hitung_ataskiri (&g_Board);
                g_bolehSimpan = TRUE;
                DrawSetDefaultAndDraw();  /* clear draw */
                FindClearStruct(); /* clear find */
                ClearObjectInfoText();
            }
        }
        else
        {
            board_hapus(&gcbrd);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
    board_hapus (&gcbrd);
}

void FileExportPlacementHandle (HWND hWnd)
{
    int unit;
    int xkomp;
    _sdata lines;
    Str255 msg;
    char szFile[256];

    if (BV2ExPlacementDlg(hInst, hWnd) == IDOK)
    {
        if (!ExPlacementIsRefdesOnly())
        {
            unit = Unit_DialogBox (hInst, hWnd);
            ExPlacementSetOptionUnit (unit);
        }
        sdata_init (&lines);
        xkomp = exportplacement (&g_Board, &lines, BV2_MYVERSION);
        if (!xkomp)
        {
            MessageBox (hWnd, "There are no devices will be saved!", "Export", MB_OK|MB_ICONSTOP);
            sdata_hapussemua (&lines);
            return;
        }
        else
        {
            sprintf (msg, "There are %d of %d devices will be saved!", xkomp, sdata_jumlahdata(&g_Board.skomp));
            MessageBox (hWnd, msg, "Export", MB_OK|MB_ICONINFORMATION);
        }
       board_nama(&g_Board, szFile);
       strcat (szFile, ".txt");
        if (OpenFileDlg(hWnd, szFile, NULL, "All files (*.*)\0*.*\0TEXT file (*.txt)\0*.txt\0",2, TRUE))
        {
            if (exportsimpanfile(&lines, szFile))
            {
                MessageBox (hWnd, "Saving is successful!", "Export", MB_OK|MB_ICONINFORMATION);
            }
            else
            {
                MessageBox (hWnd, "Saving error!", "Export", MB_OK|MB_ICONSTOP);
            }
        }
        sdata_hapussemua (&lines);
    }
}

void FileNewWindowHandle (HINSTANCE hInst, HWND hWnd)
{
    char myapp[256];
    WriteBV2WindowSettings (hWnd, "main");
    GetModuleFileName (hInst, myapp, 255);
    ShellExecute (NULL, NULL, myapp, "", NULL, SW_SHOWNORMAL );
}

void DoFileCompareDevices (HWND hWnd, int mode)
{
    board gcbrd;
    char szFile[256];
    char tempmsg[300];
    int ok = 0;


    board_init (&gcbrd);
    if (OpenFileDlg(hWnd, szFile, "Open a file will be compared with the current (TARGET)", "All files (*.*)\0*.*\0BeeVee 2 file (*.bv2)\0*.bv2\0",2, FALSE))
    {
        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (beevee_baca_bv2 (szFile, &gcbrd))
        {
            SetStatusBarText ("Comparing devices...");
            switch (mode)
            {
                case 2:
                strcat (szFile,"-cmpixy.txt");
                ok = DoCompareDevicesPlacement (&g_Board, &gcbrd, szFile);
                break;
                case 1:
                strcat (szFile,"-cmpi.txt");
                ok = DoCompareDevicesInfo (&g_Board, &gcbrd, szFile);
                break;
                default:
                strcat (szFile,"-cmp.txt");
                ok = DoCompareDevices (&g_Board, &gcbrd, szFile);
            }

            if (!ok)
                MessageBox (hWnd, szFile, "Can not save to", MB_OK | MB_ICONSTOP);
            else
            {
               sprintf (tempmsg, "Open %s?", szFile);
               if (MessageBox (hWnd, tempmsg, "Save to OK",  MB_YESNO | MB_ICONQUESTION)==IDYES)
               {
                   ShellExecute (NULL, NULL, szFile, "", NULL, SW_SHOWNORMAL );
               }
            }
        }
        else
        {
            board_hapus(&gcbrd);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
    board_hapus (&gcbrd);
}


BOOL LoadBV2WindowSettings(LPRECT r, BOOL *isMax, const char *name)
{
	HKEY hkey;
	DWORD type;
	DWORD size;
	DWORD dOptSet=0;
	Str255 keyStr;

	sprintf (keyStr,"Software\\eshabe\\BeeVee\\win\\%s", name);

	if (RegOpenKeyEx (HKEY_CURRENT_USER, keyStr, 0, KEY_READ, &hkey)==ERROR_SUCCESS)
	{
		type = REG_DWORD;
		size = sizeof(DWORD);

		RegQueryValueEx (hkey, "opt", 0, &type, (LPBYTE)&dOptSet, &size);
		if (dOptSet)
		{

            RegQueryValueEx (hkey, "top", 0, &type, (LPBYTE)&r->top, &size);
            RegQueryValueEx (hkey, "bottom", 0, &type, (LPBYTE)&r->bottom, &size);
            RegQueryValueEx (hkey, "left", 0, &type, (LPBYTE)&r->left, &size);
            RegQueryValueEx (hkey, "right", 0, &type, (LPBYTE)&r->right, &size);
            RegQueryValueEx (hkey, "max", 0, &type, (LPBYTE)isMax, &size);
		}
		RegCloseKey (hkey);

		return dOptSet;
	}

	return FALSE;
}

void WriteBV2WindowSettings (HWND hWnd, const char *name)
{

	HKEY hkey;
	DWORD dOptSet = 1;
	Str255 keyStr;
	RECT r;
	BOOL isMax = IsZoomed (hWnd);
	GetWindowRect (hWnd, &r);

	sprintf (keyStr,"Software\\eshabe\\BeeVee\\win\\%s", name);

	RegCreateKeyEx (HKEY_CURRENT_USER, keyStr, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hkey, NULL);

	RegSetValueEx (hkey, "opt", 0, REG_DWORD, (LPBYTE)&dOptSet, sizeof(DWORD));
	if (!isMax)
	{
        RegSetValueEx (hkey, "top", 0, REG_DWORD, (LPBYTE)&r.top, sizeof(DWORD));
        RegSetValueEx (hkey, "bottom", 0, REG_DWORD, (LPBYTE)&r.bottom, sizeof(DWORD));
        RegSetValueEx (hkey, "left", 0, REG_DWORD, (LPBYTE)&r.left, sizeof(DWORD));
        RegSetValueEx (hkey, "right", 0, REG_DWORD, (LPBYTE)&r.right, sizeof(DWORD));
	}
    RegSetValueEx (hkey, "max", 0, REG_DWORD, (LPBYTE)&isMax, sizeof(DWORD));

	RegCloseKey (hkey);

}


void ToolsMovedByExternalReferenceHandler (HINSTANCE hInst, HWND hWnd)
{
    DeviceNameDlg_DialogBox (hInst, hWnd);

    char *refName=GetDeviceNameDlg_name(NULL);
    if (!refName[0])
        return;
    if (komp_cari(&g_Board.skomp, refName, 0)<0)
    {
        MessageBox (hWnd, "The reference is not in current data!", refName, MB_OK | MB_ICONSTOP);
        return;
    }

    board gcbrd;
    Str255 szFile,tempmsg;
    board_init (&gcbrd);
    if (OpenFileDlg(hWnd, szFile, "Open BeeVee2", "All files (*.*)\0*.*\0BeeVee 2 file (*.bv2)\0*.bv2\0",2, FALSE))
    {
        sprintf (tempmsg, "Reading %s...", szFile);
        SetStatusBarText (tempmsg);

        if (beevee_baca_bv2 (szFile, &gcbrd))
        {
            SetStatusBarText ("Moving board...");

            Str255 myerror;
            if (!DoMoveBoardByRef (&g_Board, &gcbrd, refName,myerror))
            {
                MessageBox (hWnd, myerror, BV2_MYTITLE, MB_OK | MB_ICONSTOP);
            }
            else
            {
                //board_hitung_ataskiri (&g_Board);
                g_bolehSimpan = TRUE;
                DrawSetDefaultAndDraw();  /* clear draw */
                FindClearStruct(); /* clear find */
                ClearObjectInfoText();
            }
        }
        else
        {
            board_hapus(&gcbrd);
            MessageBox (hWnd, szFile, "Reading Error", MB_OK|MB_ICONSTOP);
        }
    }
    board_hapus (&gcbrd);
}

void ToolsBoardOriginLowerLeftHandler ()
{
    SetStatusBarText ("Moving board...");
    if (DoMoveLowerLeftTo (&g_Board,0,0))
    {
        g_bolehSimpan = TRUE;
        DrawSetDefaultAndDraw();  /* clear draw */
        FindClearStruct(); /* clear find */
        ClearObjectInfoText();
        SetStatusBarText ("Moved!");
    }
    else
        SetStatusBarText ("NOT MOVED YET!");
}

void ToolsOffsetHandler (HINSTANCE hInst, HWND hWnd)
{
    int x,y;


    if (OffsetXYDlg_DialogBox(hInst, hWnd)!=IDOK)
        return;

    GetOffsetXYDlg_XY (&x, &y);
    if (!GetOffsetXYDlg_IsMove())
    {
        int x1 = board_kiri (&g_Board);
        int y1 = board_bawah (&g_Board);
        x += x1;
        y += y1;
    }
    SetStatusBarText ("Moving board...");
    if (DoMoveLowerLeftTo (&g_Board,y,x))
    {
        g_bolehSimpan = TRUE;
        DrawSetDefaultAndDraw();  /* clear draw */
        FindClearStruct(); /* clear find */
        ClearObjectInfoText();
        SetStatusBarText ("Moved!");
    }
    else
        SetStatusBarText ("NOT MOVED YET!");
}

