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

#include "../bv2data/sboard.h"

#define BEEVEE_MERGE_ERROR_CURNOSIGNAL  0
#define BEEVEE_MERGE_ERROR_CURNOTP      -1
#define BEEVEE_MERGE_ERROR_MERNOSIGNAL  -2
#define BEEVEE_MERGE_ERROR_MERNOTP      -3


int DoMergeGenCADTrace (boardptr curbrdptr, boardptr mergedbrdptr, char *signalname)
{
    int dx=0;
    int dy=0;
    int nodeid;
    int cid;
    int ox=0,oy=0;
    int mx=0,my=0;
    int i,j;
    Str63 nodename;
    trace tr;
    Str63 asal1,asal2;

    if ( node_cari(&curbrdptr->snode, signalname, 0)<0)
        return 0;
    nodeid = node_id(&curbrdptr->snode);
    if (tpt_cari(&curbrdptr->stestpoint, nodeid, 0)<0)
        return -1;
    tpt_xy (&curbrdptr->stestpoint, &ox, &oy);

    if ( node_cari(&mergedbrdptr->snode, signalname, 0)<0)
        return -2;
    nodeid = node_id(&mergedbrdptr->snode);
    if (tpt_cari(&mergedbrdptr->stestpoint, nodeid, 0)<0)
        return -3;
    tpt_xy (&mergedbrdptr->stestpoint, &mx, &my);

    dx = ox-mx;
    dy = oy-my;

    sdata_kekepala (&curbrdptr->snode);
    while (1)
    {
        cid = node_id (&curbrdptr->snode);
        node_nama (&curbrdptr->snode, nodename);
        if ( node_cari(&mergedbrdptr->snode, nodename, 0)>=0)
        {
            nodeid = node_id(&mergedbrdptr->snode);
            i=j=-1;

            while (1)
            {
                i = trace_cari (&mergedbrdptr->strace, nodeid, i+1);
                if (i>=0 && i!=j)
                {
                    if (j==-1)
                        j=i;
                    sdata_ambildataini(&mergedbrdptr->strace, &tr);
                    /* manipulasi XY */
                    objecttrace_geser(&tr, dx, dy);
                    tr.node_id = cid; /* ganti ke yg sekarang */

                    sdata_tambah(&curbrdptr->strace, &tr, sizeof(trace));
                }
                else
                    break;
            }

        }
        if (sdata_diekorkah(&curbrdptr->snode))
            break;
        sdata_kedepan(&curbrdptr->snode);

    }

    /*
    board_asal (curbrdptr, asal1);
    board_asal (mergedbrdptr, asal2);
    if (strstr(asal1,asal2)==NULL)
    {
        strcat(asal1,asal2);
    }
    */

    return 1;

}

int DoMergeDevices (boardptr curbrdptr, boardptr mergedbrdptr, char *signalname)
{
    int dx=0;
    int dy=0;
    int ox=0,oy=0;
    int mx=0,my=0;
    komponen komp;
    int nodeid;
    Str63 asal1,asal2;

    if ( node_cari(&curbrdptr->snode, signalname, 0)<0)
        return BEEVEE_MERGE_ERROR_CURNOSIGNAL;
    nodeid = node_id(&curbrdptr->snode);
    if (tpt_cari(&curbrdptr->stestpoint, nodeid, 0)<0)
        return BEEVEE_MERGE_ERROR_CURNOTP;
    tpt_xy (&curbrdptr->stestpoint, &ox, &oy);

    if ( node_cari(&mergedbrdptr->snode, signalname, 0)<0)
        return BEEVEE_MERGE_ERROR_MERNOSIGNAL;
    nodeid = node_id(&mergedbrdptr->snode);
    if (tpt_cari(&mergedbrdptr->stestpoint, nodeid, 0)<0)
        return BEEVEE_MERGE_ERROR_MERNOTP;
    tpt_xy (&mergedbrdptr->stestpoint, &mx, &my);

    dx = ox-mx;
    dy = oy-my;

    sdata_kekepala (&mergedbrdptr->skomp);
    while (1)
    {
        sdata_ambildataini(&mergedbrdptr->skomp, &komp);
        /* manipulasi XY */
        object_geser(&komp.pos_x1, &komp.pos_y1, dx, dy);
        object_geser(&komp.pos_x2, &komp.pos_y2, dx, dy);
        sdata_tambah(&curbrdptr->skomp, &komp, sizeof(komponen));
        if (sdata_diekorkah(&mergedbrdptr->skomp))
            break;
        sdata_kedepan(&mergedbrdptr->skomp);
    }
    return 1;

    /*
    board_asal (curbrdptr, asal1);
    board_asal (mergedbrdptr, asal2);
    if (strstr(asal1,asal2)==NULL)
    {
        strcat(asal1,asal2);
    }
    */

}

