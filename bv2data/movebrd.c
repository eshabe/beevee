
#include "sboard.h"

#include <windows.h>

int DoMoveBoardByRef (board *curbrd, board *refbrd, const char *refname, char *error)
{
    komponen komp1,komp2;

    if (komp_cari (&curbrd->skomp, refname, 0)<0)
    {
        sprintf (error, "%s is not found in current data.", refname);
        return 0;
    }

    if (komp_cari (&refbrd->skomp, refname, 0)<0)
    {
        sprintf (error, "%s is not found in reference data.", refname);
        return 0;
    }

    sdata_ambildataini (&curbrd->skomp, &komp1);
    sdata_ambildataini (&refbrd->skomp, &komp2);

    if (komp1.pos_tb != komp2.pos_tb)
    {
        sprintf (error, "%s side location is different.", refname);
        return 0;
    }
    if (komp1.orentasi != komp2.orentasi)
    {
        sprintf (error, "%s orientation is different.", refname);
        return 0;
    }
    if ((komp1.pos_x2-komp1.pos_x1) != (komp2.pos_x2-komp2.pos_x1) ||
        (komp1.pos_y2-komp1.pos_y1) != (komp2.pos_y2-komp2.pos_y1))
    {
        sprintf (error, "%s dimension is different.", refname);
        return 0;
    }

    int dx = komp2.pos_x1 - komp1.pos_x1;
    int dy = komp2.pos_y1 - komp1.pos_y1;

    board_geser (curbrd, dx, dy);
    board_hitung_ataskiri (curbrd);
    return 1;
}

int DoMoveLowerLeftTo (board *bptr, int lower, int left)
{
    int dlower = lower - board_bawah(bptr);
    int dleft = left - board_kiri(bptr);

    //char msg[256];
    //sprintf (msg, "%d %d : %d %d", dleft,dlower,board_kiri(bptr),board_bawah(bptr));

    //MessageBox (NULL, msg, NULL, 0);

    if (!dlower && !dleft)
        return 0;

    board_geser (bptr, dleft, dlower);
    board_hitung_ataskiri (bptr);

    //sprintf (msg, "%d %d : %d %d", dleft,dlower,board_kiri(bptr),board_bawah(bptr));
    //MessageBox (NULL, msg, NULL, 0);

    return 1;
}
