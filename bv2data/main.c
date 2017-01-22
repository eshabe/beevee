#include "sboard.h"
#include "ag3070file.h"
#include "genrad228xfile.h"
#include "generalxy.h"

int main (int argc, char * const argv[])
{
	board mb;

    printf ("testboard!\n");

	board_init(&mb);

	/*
	board_isiasal(&mb,"agilent3070");
	ag3070_pakefixture(1);
	ag3070_baca_board("../test-bv2/eric/board", &mb);
	ag3070_baca_boardxy("../test-bv2/eric/board_xy", &mb);
	ag3070_baca_fixture("../test-bv2/eric/fixture",&mb);

	board_hitung_ataskiri (&mb);
	board_tampilinfo(&mb);
	*/

	/*
	board_isiasal(&mb,"genrad228x");
	genrad_baca_tac("../test-bv2/11815a.tac", &mb);
	genrad_baca_ckt("../test-bv2/11815a.ckt", &mb);
	genrad_baca_brd("../test-bv2/11815a.brd", &mb);

	board_hitung_ataskiri (&mb);
	board_tampilinfo(&mb);
	*/

	/*
	board_isiasal(&mb,"generalxy");
	gene_baca_file("../test-bv2/apollo.xy", &mb);
	board_tampilinfo(&mb);
	*/

	/*
	board_isiasal(&mb,"agilentsj50");
	agsj50_baca_plx ("../test-bv2/merlione_s.plx", &mb);
	board_tampilinfo(&mb);
	*/

	/*
	board_isiasal(&mb,"beevee1");
	//beevee_baca_bev ("../test-bv2/fangio2.bev", &mb);
	beevee_baca_bev ("../test-bv2/Copernicus.BEV", &mb);
	board_tampilinfo(&mb);
    */

	/*
	board_isiasal(&mb,"gencad");
	gencad_baca_cad("../test-bv2/012100a.cad", &mb);
	board_tampilinfo(&mb);
	*/

	/*
	board_isiasal(&mb,"unicampcb");
	unicampcb_baca_pcb("../test-bv2/012700a.pcb", &mb);
	board_tampilinfo(&mb);
	*/

	/*
	board_isiasal(&mb,"fabmaster");
	//fabmaster_baca_fab("test-bv2\\012610a.fab", &mb);
	fabmaster_baca_fab("../test-bv2/ag637-80103e1.fab", &mb);
	board_tampilinfo(&mb);
	*/

	board_isiasal(&mb,"padspowerpcb");
	pads_powerpcb_baca_asc("../test-bv2/PB000115D_2004.asc", &mb);
	//pads_powerpcb_baca_asc("../test-bv2/seneca.asc", &mb);
	board_tampilinfo(&mb);

	//node_tampilsemua (&bptr->snode);
	//koneksi_tampilsemua (&bptr->skonek);
	//tpt_tampilsemua (&mb.stestpoint);

	beevee_tulis_bv2("../test-bv2/test.bv2", &mb);
	board_hapus(&mb);

    return 0;
}
