/*
 * nwcli.c
 *
 *  Created on: May 25, 2020
 *      Author: andres
 */


#include "CommandParser/libcli.h"
#include "CommandParser/cmdtlv.h"
#include "cmdcodes.h"
#include "graph.h"
#include <stdio.h>





extern graph_t *topo;

static int showNwTopologyHandler(param_t * param, ser_buff_t *tlv_buf, op_mode enable_or_disable)
{
	int CMDCODE = -1;
	CMDCODE = EXTRACT_CMD_CODE(tlv_buf);

	switch(CMDCODE)
	{
		case CMDCODE_SHOW_NW_TOPOLOGY:
			dumpNwkGraph(topo);
			break;
		default:
		;
	}
}

void nwInitCLI()
{
	init_libcli();

    param_t *show   = libcli_get_show_hook();
    param_t *debug  = libcli_get_debug_hook();
    param_t *config = libcli_get_config_hook();
    param_t *clear  = libcli_get_clear_hook();
    param_t *run    = libcli_get_run_hook();

    {
    	/*Show topology*/
    	static param_t topology;

    	init_param(&topology, CMD, "topology", showNwTopologyHandler, 0, INVALID, 0, "Dump Complete Network Topology");
    	libcli_register_param(show, &topology);
    	set_param_cmd_code(&topology, CMDCODE_SHOW_NW_TOPOLOGY);
    }
    support_cmd_negation(config);
}

