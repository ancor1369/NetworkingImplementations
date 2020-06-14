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


/* Display functions when user presses ?*/
void
display_graph_nodes(param_t *param, ser_buff_t *tlv_buf){

    node_t *node;
    gddl_t *curr;

    ITERATE_GDDL_BEGIN(&topo->node_list, curr){

        node =  graphGlueToNode(curr);
        printf("%s\n", node->node_name);
    } ITERATE_GDDL_END(&topo->node_list, curr);
}

/*General Validations*/
int
validate_node_extistence(char *node_name){

    node_t *node = getNodeByNodeName(topo, node_name);

    if(node)
        return VALIDATION_SUCCESS;
    printf("Error : Node %s do not exist\n", node_name);
    return VALIDATION_FAILED;
}

static int show_arp_handler(param_t *param, ser_buff_t *tlv_buf,
                    op_mode enable_or_disable){

    node_t *node;
    char *node_name;
    tlv_struct_t *tlv = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv){

        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;

    }TLV_LOOP_END;

    node = getNodeByNodeName(topo, node_name);
    dumpArpTable(NODE_ARP_TABLE(node));
    return 0;
}





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

extern void sendArpBroadcastRequest(node_t *node, interface_t *oif, char *ipAddr);

static int arp_handler(param_t *param, ser_buff_t *tlv_buf, op_mode enabable_or_disable)
{
	node_t *node;
	char *nodeName;
	char *ipAddr;
	tlv_struct_t *tlv = NULL;

	TLV_LOOP_BEGIN(tlv_buf, tlv)
	{
		if(strncmp(tlv->leaf_id, "node-name", strlen("node-name"))==0)
			nodeName = tlv->value;
		else if(strncmp(tlv->leaf_id, "ip-address", strlen("ip-address"))==0)
			ipAddr = tlv->value;
	}TLV_LOOP_END;
	node = getNodeByNodeName(topo, nodeName);
	sendArpBroadcastRequest(node, NULL, ipAddr);
}



//Suppoort for the command show node <node-name> mac
typedef struct mac_table_ mac_table_t;
extern void dump_mac_table(mac_table_t *mac_table);
static int
show_mac_handler(param_t *param, ser_buff_t *tlv_buf,
                    op_mode enable_or_disable){

    node_t *node;
    char *node_name;
    tlv_struct_t *tlv = NULL;

    TLV_LOOP_BEGIN(tlv_buf, tlv){

        if(strncmp(tlv->leaf_id, "node-name", strlen("node-name")) ==0)
            node_name = tlv->value;

    }TLV_LOOP_END;


    node = getNodeByNodeName(topo, node_name);
    dump_mac_table(NODE_MAC_TABLE(node));
    return 0;

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
    	{
    		/*show node*/
			 static param_t node;
			 init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
			 libcli_register_param(show, &node);
			 libcli_register_display_callback(&node, display_graph_nodes);
			 {
				/*show node <node-name>*/
				 static param_t node_name;
				 init_param(&node_name, LEAF, 0, 0, validate_node_extistence, STRING, "node-name", "Node Name");
				 libcli_register_param(&node, &node_name);
				 {
					/*show node <node-name> arp*/
					static param_t arp;
					init_param(&arp, CMD, "arp", show_arp_handler, 0, INVALID, 0, "\"arp\" keyword");
					libcli_register_param(&node_name, &arp);
					set_param_cmd_code(&arp, CMDCODE_SHOW_NODE_ARP_TABLE);
				 }
				 {
					 /*show node <node-name> mac*/
					 static param_t mac;
					 init_param(&mac, CMD, "mac", show_mac_handler, 0, INVALID, 0, "Dump Mac table");
					 libcli_register_param(&node_name, &mac);
					 set_param_cmd_code(&mac, CMDCODE_SHOW_NODE_MAC_TABLE);
				  }
			 }
    	}
    }

    {
            /*run node*/
            static param_t node;
            init_param(&node, CMD, "node", 0, 0, INVALID, 0, "\"node\" keyword");
            libcli_register_param(run, &node);
            libcli_register_display_callback(&node, display_graph_nodes);
            {
                /*run node <node-name>*/
                static param_t node_name;
                init_param(&node_name, LEAF, 0, 0, validate_node_extistence, STRING, "node-name", "Node Name");
                libcli_register_param(&node, &node_name);
//                {
//                    /*run node <node-name> ping */
//                    static param_t ping;
//                    init_param(&ping, CMD, "ping" , 0, 0, INVALID, 0, "Ping utility");
//                    libcli_register_param(&node_name, &ping);
//                    {
//                        /*run node <node-name> ping <ip-address>*/
//                        static param_t ip_addr;
//                        init_param(&ip_addr, LEAF, 0, ping_handler, 0, IPV4, "ip-address", "Ipv4 Address");
//                        libcli_register_param(&ping, &ip_addr);
//                        set_param_cmd_code(&ip_addr, CMDCODE_PING);
//                    }
//                }
                {
                    /*run node <node-name> resolve-arp*/
                    static param_t resolve_arp;
                    init_param(&resolve_arp, CMD, "resolve-arp", 0, 0, INVALID, 0, "Resolve ARP");
                    libcli_register_param(&node_name, &resolve_arp);
                    {
                        /*run node <node-name> resolve-arp <ip-address>*/
                        static param_t ip_addr;
                        init_param(&ip_addr, LEAF, 0, arp_handler, 0, IPV4, "ip-address", "Nbr IPv4 Address");
                        libcli_register_param(&resolve_arp, &ip_addr);
                        set_param_cmd_code(&ip_addr, CMDCODE_RUN_ARP);
                    }
                }
            }
        }

    support_cmd_negation(config);
}

