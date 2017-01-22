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

#ifndef _RESOURCE_H__
#define _RESOURCE_H__

#define IDI_BV2                         1
#define IDI_SMALL                       2
#define IDI_BV2DOC                      3

#define IDD_ABOUTBOX                    101
#define IDD_IMPORT                      102
#define IDD_STATUS                      103
#define IDD_FIND                        104
#define IDD_FINDBAR                     105
#define IDD_OPTION                      106
#define IDD_OBJECTBAR                   107
#define IDD_OBJECTDLG                   108
#define IDD_PROPERTIES                  109
#define IDD_CONNECTION                  110
#define IDD_IMPORTPLACEMENT             111
#define IDD_RECENTS                     112
#define IDD_HOWIMPORTPLACEMENT          113
#define IDD_UNIT                        114
#define IDD_SIGNANAME                   115
#define IDD_MERGE                       116
#define IDD_EXPLACEMENT                 117
#define IDD_EDITDEV                     118
#define IDD_FINDBOX                     119
#define IDD_COPYING                     120
#define IDD_OFFSET                      121
#define IDD_REFNAME                     122

#define IDB_TOOLBAR                     100
#define IDB_BEEVEE2                     101

#define IDC_TYPE                        1000
#define IDC_NAMA                        1001
#define IDC_LISTTYPE                    1002
#define IDC_NAME                        1003
#define IDC_LOC                         1004
#define IDC_STATUS                      1004
#define IDC_INFO                        1005
#define IDC_VERSION                     1011
#define IDC_ORIGIN                      1012
#define IDC_COMP                        1013
#define IDC_NODE                        1014
#define IDC_CONN                        1015
#define IDC_NET                         1016
#define IDC_TRACE                       1017
#define IDC_XY                          1019
#define IDC_OBJECT                      1021
#define IDC_LISTTEXT                    1023
#define IDC_KIND                        1025
#define IDC_ZOOM                        1027
#define IDC_CLOSE                       1030
#define IDC_BLINK                       1033
#define IDC_LINE                        1034

#define IDC_TDEV                        1038
#define IDC_BDEV                        1039
#define IDC_TTP                         1040
#define IDC_BTP                         1041
#define IDC_SEL                         1042
#define IDC_SELB                        1043
#define IDC_BACK                        1045
#define IDC_CLINE                       1047

#define IDC_TDEV_                       1049
#define IDC_BDEV_                       1050
#define IDC_TTP_                        1051
#define IDC_BTP_                        1052
#define IDC_SEL_                        1053
#define IDC_SELB_                       1054
#define IDC_BACK_                       1055
#define IDC_TRACE_                      1056
#define IDC_CLINE_                      1057

#define IDC_TRACELK                     1068
#define IDC_TRACELK_                    1069
#define IDC_TRACET                      1070
#define IDC_TRACEB                      1071
#define IDC_TRACET_                     1072
#define IDC_TRACEB_                     1073
#define IDC_VIA                         1077 /* 2.1.1 */
#define IDC_VIA_                        1078


#define IDC_CLEAR                       1044
#define IDC_COLOR                       1058
#define IDC_DEFAULT                     1059
#define IDC_ENAOBJECT                   1060
#define IDC_ENAFIND                     1061
#define IDC_SIGNAL                      1062
#define IDC_DEVICE                      1063
#define IDC_THANK                       1065
#define IDC_UPDATE                      1066
#define IDC_DEVCONNECT                  1067
#define IDC_ENAFBSPACE                  1074
#define IDC_TOOLBARTEXT                 1075
#define IDC_NEWWINDOW                   1076 /* 2.0.5 */

#define IDC_REFDES                      2001
#define IDC_POSX                        2002
#define IDC_POSY                        2003
#define IDC_ORIENT                      2004
#define IDC_POSZ                        2005
#define IDC_PN                          2006
#define IDC_SHAPE                       2007
#define IDC_BOTID                       2008
#define IDC_BTSPACE                     2009
#define IDC_BTTAB                       2010
#define IDC_BTOTHERCHAR                 2011
#define IDC_EDOTHERCHAR                 2012
#define IDC_TOKENSTRING                 2013
#define IDC_HOW                         2014
#define IDC_RBCOLUMN                    2015
#define IDC_RBCHAR                      2016
#define IDC_BTMMARK                     2017
#define IDC_STARTLINE                   2018

#define IDC_RBUNITMILS                  2021
#define IDC_RBUNITINCH                  2022
#define IDC_RBUNITMICRON                2023
#define IDC_RBUNITMM                    2024

#define IDC_EDX1                        2030
#define IDC_EDX2                        2031
#define IDC_EDY1                        2032
#define IDC_EDY2                        2033
#define IDC_APPLY                       2034

#define IDC_XTOP                        2035
#define IDC_XBOTTOM                     2036
#define IDC_XBOTH                       2037
#define IDC_XREFDESONLY                 2038

#define IDC_MOVETO                      2039


#define IDS_APP_TITLE                   0xFF01
#define IDC_BV2                         0xFF02
#define ID_TOOLBAR                      0xFF03
#define IDS_TOOLBAR                     0xFF04

#define IDM_ABOUT                       101
#define IDM_EXIT                        102
#define IDM_README                      103
#define IDM_COPYING                     104
#define IDM_CREDITS                     105

#define ID_FILE_OPEN                    401
#define ID_FILE_OPENRECENT              402
#define ID_FILE_CLOSE                   403
#define ID_FILE_SAVE                    405
#define ID_FILE_SAVE_AS                 406
#define ID_FILE_IMPORT                  407
#define ID_FILE_PROPERTIES              408
#define ID_FILE_MERGE                   409
#define ID_FILE_EXPLACEMENT             410
#define ID_FILE_NEW                     411
#define ID_FILE_COMPAREDEVICES          412
#define ID_FILE_COMPAREDEVICESINFO      413
#define ID_FILE_COMPAREDEVICESPLACE     414

#define ID_VIEW_ROTATE90CW              501
#define ID_VIEW_ROTATE90CCW             502
#define ID_VIEW_FLIPHORIZONTAL          503
#define ID_VIEW_FLIPVERTICAL            504
#define ID_VIEW_ZOOMIN                  505
#define ID_VIEW_ZOOMOUT                 506
#define ID_VIEW_ZOOMTOFIT               507
#define ID_VIEW_ZOOMTOSELECTION         508
#define ID_VIEW_ACTUALSIZE              509
#define ID_VIEW_SHOWTOPDEVICES          510
#define ID_VIEW_TOPDEVICES              511
#define ID_VIEW_SHOWBOTTOMDEVICES       512
#define ID_VIEW_BOTTOMDEVICES           513
#define ID_VIEW_SHOWTOPTESTPINS         514
#define ID_VIEW_TOPTESTPINS             515
#define ID_VIEW_SHOWBOTTOMTESTPINS      516
#define ID_VIEW_BOTTOMTESTPINS          517
#define ID_VIEW_SHOWTRACES              519
#define ID_VIEW_TRACES                  520
#define ID_VIEW_RESET                   521
#define ID_VIEW_DEVICES                 522
#define ID_VIEW_TESTPINS                523
#define ID_VIEW_DEVICESTESTPINS         524
#define ID_VIEW_CONNGUIDE               525
#define ID_VIEW_TRACESTOP               526
#define ID_VIEW_TRACESBOT               527
#define ID_VIEW_TRACESIN                528

#define ID_FIND_DEVICE                  601
#define ID_FIND_SIGNAL                  602
#define ID_FIND_NODE                    603
#define ID_FIND_NAIL                    604
#define ID_FIND_PROBE                   605
#define ID_DOFIND                       606
#define ID_FIND_CONNECTION              607

#define ID_TOOL_LOCK                    701
#define ID_TOOL_LOCKADD                 702
#define ID_TOOL_CLEAR                   703
#define ID_TOOLS_OPTIONS                704
#define ID_TOOL_OBJECTINFO              705
#define ID_TOOLS_CONNECTION             706
#define ID_TOOLS_EDIT                   707
#define ID_TOOLS_MOVEPT2TP0             708
#define ID_TOOLS_MOVEPT2TP9             709
#define ID_TOOLS_MOVEPT2NEXT            710
#define ID_TOOLS_MOVEPT2PREV            711
#define ID_TOOLS_MOVEPT2                712
#define ID_TOOLS_OFFSET                 713
#define ID_TOOLS_BOARDORIGIN_LL         714
#define ID_TOOLS_BOARDMOVE_EXTREF       715

#define ID_FX_COMMAND                   0x400
#define ID_FX_DEVICE                    0x401
#define ID_FX_SIGNAL                    0x402
#define ID_FX_NODE                      0x403
#define ID_FX_NAIL                      0x404
#define ID_FX_PROBE                     0x405
#define ID_FX_CONNECTION                0x406

#define ID_LOADEX                       0x410

#define IDC_STATIC                      -1

#endif
