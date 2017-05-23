#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <linuxapi.h>
#include <tx_api.h>
#include <craton/debug.h>
#include "error_code.h"

#include <j2735r41_map_api.h>
#include "j2735r41_common.h"

#include <libcli.h>
#include <craton/cli.h>
#include <atlk/nav_service.h>

#include <dot3_common.h>
#include <dot3_api.h>
#include <j2735_mem.h>

#define SHOW_ENCODE 1
#define SHOW_DECODED 1

static int stop_map;

void stop_map_func(ULONG input)
{
	(void)input;
	stop_map = 1;
	return;
}

void _cmd_map_free_struct(map_standard_item *free_map_item)
{
	int i, j, k, m;

	if(free_map_item == NULL){
		debug_printf("[MAP] Error: _cmd_map_free_struct() input is NULL\n");
	}else{
		/*(6) intersections **OP**/
		if(free_map_item->intersections != NULL){
			for(i = 0; i < free_map_item->intersections_num; i++){
				if(free_map_item->intersections[i].speed_limits != NULL){
					j2735_free(free_map_item->intersections[i].speed_limits);
				}

				if(free_map_item->intersections[i].lane_set != NULL){
					for(j = 0; j < free_map_item->intersections[i].lane_set_num; j++){
						if(free_map_item->intersections[i].lane_set[j].node_list.nodes != NULL){
							for(k = 0; k < free_map_item->intersections[i].lane_set[j].node_list.nodes_num; k++){
								if(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node != NULL){
									j2735_free(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node);
								}

								if(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled != NULL){
									j2735_free(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled);
								}

								if(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled != NULL){
									j2735_free(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled);
								}

								if(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data != NULL){
									for(m = 0; m < free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num; m++){
										if(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits != NULL){
											j2735_free(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits);
										}
									}								
									
									j2735_free(free_map_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data);
								}								
							}
							
							j2735_free(free_map_item->intersections[i].lane_set[j].node_list.nodes);
						}

						if(free_map_item->intersections[i].lane_set[j].connects_to != NULL){
							j2735_free(free_map_item->intersections[i].lane_set[j].connects_to);
						}

						if(free_map_item->intersections[i].lane_set[j].overlays != NULL){
							j2735_free(free_map_item->intersections[i].lane_set[j].overlays);
						}						
					}

					j2735_free(free_map_item->intersections[i].lane_set);
				}
			}
			
			j2735_free(free_map_item->intersections);
		}

		/*(7)***roadSegments**OP**/
		if(free_map_item->road_segments != NULL){
			for(i = 0; i < free_map_item->road_segments_num; i++){
				if(free_map_item->road_segments[i].speed_limits != NULL){
					j2735_free(free_map_item->road_segments[i].speed_limits);
				}

				if(free_map_item->road_segments[i].road_lane_set != NULL){
					for(j = 0; j < free_map_item->road_segments[i].road_lane_set_num; j++){
						
						if(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes != NULL){						
							for(k = 0; k < free_map_item->road_segments[i].road_lane_set[j].node_list.nodes_num; k++){
								if(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.local_node != NULL){
									j2735_free(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.local_node);
								}

								if(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.disabled != NULL){
									j2735_free(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.disabled);
								}

								if(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.enabled != NULL){
									j2735_free(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.enabled);
								}

								if(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data != NULL){
									for(m = 0; m < free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data_num; m++){
										if(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits != NULL){
											j2735_free(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits);
										}
									}

									j2735_free(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data);
								}
							}
							
							j2735_free(free_map_item->road_segments[i].road_lane_set[j].node_list.nodes);
						}

						if(free_map_item->road_segments[i].road_lane_set[j].connects_to != NULL){
							j2735_free(free_map_item->road_segments[i].road_lane_set[j].connects_to);
						}

						if(free_map_item->road_segments[i].road_lane_set[j].overlays != NULL){
							j2735_free(free_map_item->road_segments[i].road_lane_set[j].overlays);
						}
					}
					
					j2735_free(free_map_item->road_segments[i].road_lane_set);
				}
			}			
						
			j2735_free(free_map_item->road_segments);
		}

		/*(9)***restrictionList**OP**/
		if(free_map_item->restriction_list != NULL){
			for(i = 0; i < free_map_item->restriction_list_num; i++){
				if(free_map_item->restriction_list[i].users != NULL){					
					j2735_free(free_map_item->restriction_list[i].users);
				}
			}
			
			j2735_free(free_map_item->restriction_list);
		}
		
		j2735_free(free_map_item);
	}
}

void _show_map_standard_item(map_standard_item *decoded_item)
{
	int i, j, k, m, n;
	
	printf("==========MAP Message==========\n");

	/*(1) msgID */
	printf("msg_id = %d\n", decoded_item->msg_id);
	/*(2) msgSubID **OP**/
	if(decoded_item->mask_msg_sub_id == 1)
	{
		printf("msg_sub_id = %d\n", decoded_item->msg_sub_id);
	}
	/*(3) msgIssueRevision */
	printf("msg_issue_revision = %d\n", decoded_item->msg_issue_revision);
    /*(4) layerType **OP**/
	if(decoded_item->mask_layer_type == 1)
	{
		printf("layer_type = %d\n", decoded_item->layer_type);
	}		
	/*(5) layerID **OP**/
	if(decoded_item->mask_layer_id == 1)
	{
		printf("layer_id = %d\n", decoded_item->layer_id);
	}
	/*(6) intersections **OP**/
	if(decoded_item->mask_intersections == 1)
	{
		printf("intersections_num = %d\n", decoded_item->intersections_num);
		for(i = 0; i < decoded_item->intersections_num; i++)
		{
			/* (6-1) name **OP**/
			if(decoded_item->intersections[i].mask_name == 1)
			{
				printf("intersections[%d].name = %s\n", i, decoded_item->intersections[i].name);
			}
			/* (6-2) id */
			if(decoded_item->intersections[i].id.mask_region == 1)
			{
				printf("intersections[%d].id.region = %d\n", i, decoded_item->intersections[i].id.region);
			}
			printf("intersections[%d].id.id = %d\n", i, decoded_item->intersections[i].id.id);
			/* (6-3) revision */
			printf("intersections[%d].revision = %d\n", i, decoded_item->intersections[i].revision);
			/* (6-4) refPoint */
			/* (6-4-1) lat */
			printf("intersections[%d].ref_point.lat = %lu\n", i, decoded_item->intersections[i].ref_point.lat);
			/* (6-4-2) Long */
			printf("intersections[%d].ref_point.longitude = %lu\n", i, decoded_item->intersections[i].ref_point.longitude);
			/* (6-4-3) elevation **OP**/
			if(decoded_item->intersections[i].ref_point.mask_elevation == 1)
			{
				printf("intersections[%d].ref_point.elevation = %lu\n", i, decoded_item->intersections[i].ref_point.elevation);
			}
			/* (6-5) laneWidth **OP**/
			if(decoded_item->intersections[i].mask_lane_width == 1)
			{
				printf("intersections[%d].lane_width = %d\n", i, decoded_item->intersections[i].lane_width);
			}
			/* (6-6) speedLimits **OP**/
			if(decoded_item->intersections[i].mask_speed_limits == 1)
			{
				printf("intersections[%d].speed_limits_num = %d\n", i, decoded_item->intersections[i].speed_limits_num);
				for(j = 0; j < decoded_item->intersections[i].speed_limits_num; j++)
				{
					/* (6-6-1) type */
					printf("intersections[%d].speed_limits[%d].type = %d\n", i, j, decoded_item->intersections[i].speed_limits[j].type);
					/* (6-6-2) speed */
					printf("intersections[%d].speed_limits[%d].speed = %d\n", i, j, decoded_item->intersections[i].speed_limits[j].speed);
				}
			}
			/* (6-7) laneSet */
			printf("intersections[%d].lane_set_num = %d\n", i, decoded_item->intersections[i].lane_set_num);
			for(j = 0; j < decoded_item->intersections[i].lane_set_num; j++)
			{
				/* (6-7-1) laneID */
				printf("intersections[%d].lane_set[%d].lane_id = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_id);
				/* (6-7-2) name **OP**/
				if(decoded_item->intersections[i].lane_set[j].mask_name == 1)
				{
					printf("intersections[%d].lane_set[%d].name = %s\n", i, j, decoded_item->intersections[i].lane_set[j].name);
				}
				/* (6-7-3) ingressApproach **OP**/
				if(decoded_item->intersections[i].lane_set[j].mask_ingress_approach == 1)
				{
					printf("intersections[%d].lane_set[%d].ingress_approach = %d\n", i, j, decoded_item->intersections[i].lane_set[j].ingress_approach);
				}
				/* (6-7-4) egressApproach **OP**/
				if(decoded_item->intersections[i].lane_set[j].mask_egress_approach == 1)
				{
					printf("intersections[%d].lane_set[%d].egress_approach = %d\n", i, j, decoded_item->intersections[i].lane_set[j].egress_approach);
				}
				/* (6-7-5) laneAttributes */
				printf("intersections[%d].lane_set[%d].lane_attributes.directional_use = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.directional_use);
				printf("intersections[%d].lane_set[%d].lane_attributes.shared_with = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.shared_with);
				printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.lane_type_choice = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice);
				if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_VEHICLE)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.vehicle = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.vehicle);
				}
				else if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_CROSSWALK)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.crosswalk = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.crosswalk);
				}
				else if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_BIKELANE)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.bike_lane = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.bike_lane);
				}
				else if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_SIDEWALK)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.sidewalk = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.sidewalk);
				}
				else if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_MEDIAN)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.median = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.median);
				}
				else if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_STRIPING)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.striping = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.striping);
				}
				else if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_TRACKEDVEHICLE)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.tracked_vehicle = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.tracked_vehicle);
				}
				else if(decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_PARKING)
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.parking = %d\n", i, j, decoded_item->intersections[i].lane_set[j].lane_attributes.lane_type.parking);
				}
				else
				{
					printf("intersections[%d].lane_set[%d].lane_attributes.lane_type.parking = Nothing\n", i, j);
				}
				/* (6-7-6) maneuvers **OP**/
				if(decoded_item->intersections[i].lane_set[j].mask_maneuvers == 1)
				{
					printf("intersections[%d].lane_set[%d].maneuvers = %d\n", i, j, decoded_item->intersections[i].lane_set[j].maneuvers);
				}
				/* (6-7-7) nodeList */
				if(decoded_item->intersections[i].lane_set[j].node_list.node_list_choice == NODE_LIST2_NODES)
				{
					printf("intersections[%d].lane_set[%d].node_list.nodes_num = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.nodes_num);
					for(k = 0; k < decoded_item->intersections[i].lane_set[j].node_list.nodes_num; k++)
					{
						/* (6-7-7-1) delta */
						if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY1)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy1.x = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy1.x);
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy1.y = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy1.y);
						}
						else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY2)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy2.x = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy2.x);
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy2.y = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy2.y);
						}
						else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY3)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy3.x = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy3.x);
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy3.y = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy3.y);
						}
						else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY4)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy4.x = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy4.x);
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy4.y = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy4.y);
						}
						else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY5)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy5.x = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy5.x);
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy5.y = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy5.y);
						}
						else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY6)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy6.x = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy6.x);
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_xy6.y = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy6.y);
						}
						else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_LATLON)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_lat_lon.lat = %lu\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_lat_lon.lat);
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.node_lat_lon.lon = %lu\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_lat_lon.lon);
						}
						else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_REGIONAL)
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.choice = NODE_OFFSET_POINT_REGIONAL\n", i, j, k);
						}
						else
						{
							printf("intersections[%d].lane_set[%d].node_list.nodes[%d].delta.choice = NODE_OFFSET_POINT_NOTHING\n", i, j, k);
						}
						/* (6-7-7-2) attributes **OP**/
						if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].mask_attributes == 1)
						{
							/* (6-7-7-2-1) localNode **OP**/
							if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_local_node == 1)
							{
								printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.local_node_num = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num);
								for(m = 0; m < decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num; m++)
								{
									printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.local_node[%d] = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node[m]);
								}										
							}
							/* (6-7-7-2-2) disabled **OP**/
							if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_disabled == 1)
							{
								printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.disabled_num = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num);
								for(m = 0; m < decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num; m++)
								{
									printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.disabled[%d] = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled[m]);
								}										
							}
							/* (6-7-7-2-3) enabled **OP**/
							if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_enabled == 1)
							{
								printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.enabled_num = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num);
								for(m = 0; m < decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num; m++)
								{
									printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.enabled[%d] = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled[m]);
								}										
							}								
							/* (6-7-7-2-4) data **OP**/
							if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_data == 1)
							{
								printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data_num = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num);
								for(m = 0; m < decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num; m++)
								{
									if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_PATHENDPOINTANGLE)
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].path_end_point_angle = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].path_end_point_angle);
									}
									else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANECROWNPOINTCENTER)
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_crown_point_center = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_crown_point_center);
									}
									else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANECROWNPOINTLEFT)
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_crown_point_left = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_crown_point_left);
									}
									else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANECROWNPOINTRIGHT)
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_crown_point_right = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_crown_point_right);
									}
									else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANEANGLE)
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_angle = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_angle);
									}
									else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_SPEEDLIMITS)
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].speed_limits_num = %d\n", i, j, k, m, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits_num);
										for(n = 0; n < decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits_num; n++)
										{
											/* (6-7-7-2-4-1) type */
											printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].speed_limits[%d].type = %d\n", i, j, k, m, n, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits[n].type);
											/* (6-7-7-2-4-2) speed */
											printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].speed_limits[%d].speed = %d\n", i, j, k, m, n, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits[n].speed);
										}		
									}
									else if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_REGIONAL)
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_REGIONAL\n", i, j, k, m);
									}
									else
									{
										printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_NOTHING\n", i, j, k, m);
									}									
								}				
							}
							/* (6-7-7-2-6) dWidth **OP**/
							if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_d_width == 1)
							{
								printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.d_width = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.d_width);
							}
							/* (6-7-7-2-7) dElevation **OP**/
							if(decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_d_elevation == 1)
							{
								printf("intersections[%d].lane_set[%d].node_list.nodes[%d].attributes.d_elevation = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.d_elevation);
							}
						}
					}
				}
				else if(decoded_item->intersections[i].lane_set[j].node_list.node_list_choice == NODE_LIST2_COMPUTED)
				{						
					/* (6-7-7-1) referenceLaneId */
					printf("intersections[%d].lane_set[%d].node_list.computed.reference_lane_id = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.reference_lane_id);
					/* (6-7-7-2) offsetXaxis */
					if(decoded_item->intersections[i].lane_set[j].node_list.computed.offset_xaxis_item.offset_xaxis_choice == OFFSET_XAXIS_SMALL)
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.offset_xaxis_item.small = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.offset_xaxis_item.small);
					}
					else if(decoded_item->intersections[i].lane_set[j].node_list.computed.offset_xaxis_item.offset_xaxis_choice == OFFSET_XAXIS_LARGE)
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.offset_xaxis_item.large = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.offset_xaxis_item.large);
					}
					else
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.offset_xaxis_item.offset_xaxis_choice = OFFSET_XAXIS_NOTHING\n", i, j);
					}
					/* (6-7-7-3) offsetYaxis */
					if(decoded_item->intersections[i].lane_set[j].node_list.computed.offset_yaxis_item.offset_yaxis_choice == OFFSET_YAXIS_SMALL)
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.offset_yaxis_item.small = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.offset_yaxis_item.small);
					}
					else if(decoded_item->intersections[i].lane_set[j].node_list.computed.offset_yaxis_item.offset_yaxis_choice == OFFSET_YAXIS_LARGE)
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.offset_yaxis_item.large = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.offset_yaxis_item.large);
					}
					else
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.offset_yaxis_item.offset_yaxis_choice = OFFSET_YAXIS_NOTHING\n", i, j);
					}						
					/* (6-7-7-4) rotateXY **OP**/
					if(decoded_item->intersections[i].lane_set[j].node_list.computed.mask_rotate_xy == 1)
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.rotate_xy = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.rotate_xy);
					}
					/* (6-7-7-5) scaleXaxis **OP**/
					if(decoded_item->intersections[i].lane_set[j].node_list.computed.mask_scale_xaxis == 1)
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.scale_xaxis = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.scale_xaxis);
					}
					/* (6-7-7-6) scaleYaxis **OP**/
					if(decoded_item->intersections[i].lane_set[j].node_list.computed.mask_scale_yaxis == 1)
					{
						printf("intersections[%d].lane_set[%d].node_list.computed.scale_yaxis = %d\n", i, j, decoded_item->intersections[i].lane_set[j].node_list.computed.scale_yaxis);
					}
				}
				else
				{
					printf("intersections[%d].lane_set[%d].node_list.node_list_choice = NODE_LIST2_NOTHING\n", i, j);
				}
				/* (6-7-8) connectsTo **OP**/
				if(decoded_item->intersections[i].lane_set[j].mask_connects_to == 1)
				{
					printf("intersections[%d].lane_set[%d].connects_to_num = %d\n", i, j, decoded_item->intersections[i].lane_set[j].connects_to_num);
					for(k = 0; k < decoded_item->intersections[i].lane_set[j].connects_to_num; k++)
					{
						/* (6-7-8-1) connectingLane */
						printf("intersections[%d].lane_set[%d].connects_to[%d].connecting_lane.lane = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.lane);
						if(decoded_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.mask_maneuver == 1)
						{
							printf("intersections[%d].lane_set[%d].connects_to[%d].connecting_lane.maneuver = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.maneuver);
						}
						/* (6-7-8-2) remoteIntersection **OP**/
						if(decoded_item->intersections[i].lane_set[j].connects_to[k].mask_remote_intersection == 1)
						{
							/* (6-7-8-2-1) region**OP**/
							if(decoded_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.mask_region == 1)
							{
								printf("intersections[%d].lane_set[%d].connects_to[%d].remote_intersection.region = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.region);
							}
							/* (6-7-8-2-2) id*/
							printf("intersections[%d].lane_set[%d].connects_to[%d].remote_intersection.id = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.id);
						}							
						/* (6-7-8-3) signalGroup **OP**/
						if(decoded_item->intersections[i].lane_set[j].connects_to[k].mask_signal_group == 1)
						{
							printf("intersections[%d].lane_set[%d].connects_to[%d].signal_group = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].connects_to[k].signal_group);
						}
						/* (6-7-8-4) userClass **OP**/
						if(decoded_item->intersections[i].lane_set[j].connects_to[k].mask_user_class == 1)
						{
							printf("intersections[%d].lane_set[%d].connects_to[%d].user_class = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].connects_to[k].user_class);
						}
						/* (6-7-8-5) connectionID **OP**/
						if(decoded_item->intersections[i].lane_set[j].connects_to[k].mask_connection_id == 1)
						{
							printf("intersections[%d].lane_set[%d].connects_to[%d].connection_id = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].connects_to[k].connection_id);
						}
					}
				}
				/* (6-7-9) overlays **OP**/
				if(decoded_item->intersections[i].lane_set[j].mask_overlays == 1)
				{
					printf("intersections[%d].lane_set[%d].overlays_num = %d\n", i, j, decoded_item->intersections[i].lane_set[j].overlays_num);
					for(k = 0; k < decoded_item->intersections[i].lane_set[j].overlays_num; k++)
					{
						printf("intersections[%d].lane_set[%d].overlays[%d] = %d\n", i, j, k, decoded_item->intersections[i].lane_set[j].overlays[k]);
					}
				}
			}
		}		
	}		
	/*(7) roadSegments **OP**/
	if(decoded_item->mask_road_segments == 1)
	{
		printf("road_segments_num = %d\n", decoded_item->road_segments_num);
		for(i = 0; i < decoded_item->road_segments_num; i++)
		{
			/* (7-1) name **OP**/
			if(decoded_item->road_segments[i].mask_name == 1)
			{
				printf("road_segments[%d].name = %s\n", i, decoded_item->road_segments[i].name);
			}
			/* (7-2) id */
			if(decoded_item->road_segments[i].id.mask_region == 1)
			{
				printf("road_segments[%d].id.region = %d\n", i, decoded_item->road_segments[i].id.region);
			}
			printf("road_segments[%d].id.id = %d\n", i, decoded_item->road_segments[i].id.id);
			/* (7-3) revision */
			printf("road_segments[%d].revision = %d\n", i, decoded_item->road_segments[i].revision);
			/* (7-4) refPoint */
			/* (7-4-1) lat */
			printf("road_segments[%d].ref_point.lat = %lu\n", i, decoded_item->road_segments[i].ref_point.lat);
			/* (7-4-2) Long */
			printf("road_segments[%d].ref_point.longitude = %lu\n", i, decoded_item->road_segments[i].ref_point.longitude);
			/* (7-4-3) elevation **OP**/
			if(decoded_item->road_segments[i].ref_point.mask_elevation == 1)
			{
				printf("road_segments[%d].ref_point.elevation = %lu\n", i, decoded_item->road_segments[i].ref_point.elevation);
			}				
			/* (7-5) laneWidth **OP**/
			if(decoded_item->road_segments[i].mask_lane_width == 1)
			{
				printf("road_segments[%d].lane_width = %d\n", i, decoded_item->road_segments[i].lane_width);
			}
			/* (7-6) speedLimits **OP**/
			if(decoded_item->road_segments[i].mask_speed_limits == 1)
			{
				printf("road_segments[%d].speed_limits_num = %d\n", i, decoded_item->road_segments[i].speed_limits_num);
				for(j = 0; j < decoded_item->road_segments[i].speed_limits_num; j++)
				{
					/* (7-6-1) type */
					printf("road_segments[%d].speed_limits[%d].type = %d\n", i, j, decoded_item->road_segments[i].speed_limits[j].type);
					/* (7-6-2) speed */
					printf("road_segments[%d].speed_limits[%d].speed = %d\n", i, j, decoded_item->road_segments[i].speed_limits[j].speed);
				}	
			}
			/* (7-7) roadLaneSet */
			printf("road_segments[%d].road_lane_set_num = %d\n", i, decoded_item->road_segments[i].road_lane_set_num);
			for(j = 0; j < decoded_item->road_segments[i].road_lane_set_num; j++)
			{
				/* (7-7-1) laneID */
				printf("road_segments[%d].road_lane_set[%d].lane_id = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_id);
				/* (7-7-2) name **OP**/
				if(decoded_item->road_segments[i].road_lane_set[j].mask_name == 1)
				{
					printf("road_segments[%d].road_lane_set[%d].name = %s\n", i, j, decoded_item->road_segments[i].road_lane_set[j].name);
				}
				/* (7-7-3) ingressApproach **OP**/
				if(decoded_item->road_segments[i].road_lane_set[j].mask_ingress_approach == 1)
				{
					printf("road_segments[%d].road_lane_set[%d].ingress_approach = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].ingress_approach);
				}
				/* (7-7-4) egressApproach **OP**/
				if(decoded_item->road_segments[i].road_lane_set[j].mask_egress_approach == 1)
				{
					printf("road_segments[%d].road_lane_set[%d].egress_approach = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].egress_approach);
				}
				/* (7-7-5) laneAttributes */
				printf("road_segments[%d].road_lane_set[%d].lane_attributes.directional_use = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.directional_use);
				printf("road_segments[%d].road_lane_set[%d].lane_attributes.shared_with = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.shared_with);
				if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_VEHICLE)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.vehicle = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.vehicle);
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_CROSSWALK)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.crosswalk = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.crosswalk);
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_BIKELANE)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.bike_lane = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.bike_lane);
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_SIDEWALK)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.sidewalk = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.sidewalk);
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_MEDIAN)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.median = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.median);
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_STRIPING)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.striping = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.striping);
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_TRACKEDVEHICLE)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.tracked_vehicle = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.tracked_vehicle);
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice == LANE_TYPE_ATTRIBUTES_PARKING)
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.parking = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.parking);
				}
				else
				{
					printf("road_segments[%d].road_lane_set[%d].lane_attributes.lane_type.lane_type_choice = LANE_TYPE_ATTRIBUTES_NOTHING\n", i, j);
				}
				/* (7-7-6) maneuvers **OP**/
				if(decoded_item->road_segments[i].road_lane_set[j].mask_maneuvers == 1)
				{
					printf("road_segments[%d].road_lane_set[%d].maneuvers = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].maneuvers);
				}
				/* (7-7-7) nodeList */
				if(decoded_item->road_segments[i].road_lane_set[j].node_list.node_list_choice == NODE_LIST2_NODES)
				{
					printf("road_segments[%d].road_lane_set[%d].node_list.nodes_num = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes_num);
					for(k = 0; k < decoded_item->road_segments[i].road_lane_set[j].node_list.nodes_num; k++)
					{
						/* (7-7-7-1) delta */
						if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY1)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy1.x = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy1.x);
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy1.y = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy1.y);
						}
						else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY2)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy2.x = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy2.x);
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy2.y = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy2.y);
						}
						else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY3)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy3.x = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy3.x);
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy3.y = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy3.y);
						}
						else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY4)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy4.x = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy4.x);
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy4.y = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy4.y);
						}
						else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY5)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy5.x = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy5.x);
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy5.y = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy5.y);
						}
						else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_XY6)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy6.x = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy6.x);
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_xy6.y = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_xy6.y);
						}
						else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_LATLON)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_lat_lon.lat = %lu\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_lat_lon.lat);
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.node_lat_lon.lon = %lu\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.node_lat_lon.lon);
						}
						else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].delta.choice == NODE_OFFSET_POINT_REGIONAL)
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.choice = NODE_OFFSET_POINT_REGIONAL\n", i, j, k);
						}
						else
						{
							printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].delta.choice = NODE_OFFSET_POINT_NOTHING\n", i, j, k);
						}
						/* (7-7-7-2) attributes **OP**/
						if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].mask_attributes == 1)
						{
							/* (7-7-7-2-1) localNode **OP**/
							if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.mask_local_node == 1)
							{
								printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.local_node_num = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.local_node_num);
								for(m = 0; m < decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.local_node_num; m++)
								{
									printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.local_node[%d] = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.local_node[m]);
								}										
							}
							/* (7-7-7-2-2) disabled **OP**/
							if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.mask_disabled == 1)
							{
								printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.disabled_num = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.disabled_num);
								for(m = 0; m < decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.disabled_num; m++)
								{
									printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.disabled[%d] = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.disabled[m]);
								}										
							}
							/* (7-7-7-2-3) enabled **OP**/
							if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.mask_enabled == 1)
							{
								printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.enabled_num = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.enabled_num);
								for(m = 0; m < decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.enabled_num; m++)
								{
									printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.enabled[%d] = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.enabled[m]);
								}										
							}								
							/* (7-7-7-2-4) data **OP**/
							if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.mask_data == 1)
							{
								printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data_num = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data_num);
								for(m = 0; m < decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data_num; m++)
								{
									if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_PATHENDPOINTANGLE)
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].path_end_point_angle = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].path_end_point_angle);
									}
									else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANECROWNPOINTCENTER)
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_crown_point_center = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_crown_point_center);
									}
									else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANECROWNPOINTLEFT)
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_crown_point_left = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_crown_point_left);
									}
									else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANECROWNPOINTRIGHT)
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_crown_point_right = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_crown_point_right);
									}
									else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_LANEANGLE)
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_angle = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_angle);
									}
									else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_SPEEDLIMITS)
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].speed_limits_num = %d\n", i, j, k, m, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits_num);
										for(n = 0; n < decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits_num; n++)
										{
											/* (7-7-7-2-4-1) type */
											printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].speed_limits[%d].type = %d\n", i, j, k, m, n, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits[n].type);
											/* (7-7-7-2-4-2) speed */
											printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].speed_limits[%d].speed = %d\n", i, j, k, m, n, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].speed_limits[n].speed);
										}		
									}
									else if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_REGIONAL)
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_REGIONAL\n", i, j, k, m);
									}
									else
									{
										printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.data[%d].lane_data_attribute_choice == LANE_DATA_ATTRIBUTE_NOTHING\n", i, j, k, m);
									}									
								}				
							}
							/* (7-7-7-2-6) dWidth **OP**/
							if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.mask_d_width == 1)
							{
								printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.d_width = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.d_width);
							}
							/* (7-7-7-2-7) dElevation **OP**/
							if(decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.mask_d_elevation == 1)
							{
								printf("road_segments[%d].road_lane_set[%d].node_list.nodes[%d].attributes.d_elevation = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].node_list.nodes[k].attributes.d_elevation);
							}
						}
					}
				}
				else if(decoded_item->road_segments[i].road_lane_set[j].node_list.node_list_choice == NODE_LIST2_COMPUTED)
				{
					/* (7-7-7-1) referenceLaneId */
					printf("road_segments[%d].road_lane_set[%d].node_list.computed.reference_lane_id = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.reference_lane_id);
					/* (7-7-7-2) offsetXaxis */
					if(decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.offset_xaxis_choice == OFFSET_XAXIS_SMALL)
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.offset_xaxis_item.small = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.small);
					}
					else if(decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.offset_xaxis_choice == OFFSET_XAXIS_LARGE)
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.offset_xaxis_item.large = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.large);
					}
					else
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.offset_xaxis_item.offset_xaxis_choice = OFFSET_XAXIS_NOTHING\n", i, j);
					}
					/* (7-7-7-3) offsetYaxis */
					if(decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.offset_yaxis_choice == OFFSET_YAXIS_SMALL)
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.offset_yaxis_item.small = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.small);
					}
					else if(decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.offset_yaxis_choice == OFFSET_YAXIS_LARGE)
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.offset_yaxis_item.large = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.large);
					}
					else
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.offset_yaxis_item.offset_yaxis_choice = OFFSET_YAXIS_NOTHING\n", i, j);
					}						
					/* (7-7-7-4) rotateXY **OP**/
					if(decoded_item->road_segments[i].road_lane_set[j].node_list.computed.mask_rotate_xy == 1)
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.rotate_xy = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.rotate_xy);
					}
					/* (7-7-7-5) scaleXaxis **OP**/
					if(decoded_item->road_segments[i].road_lane_set[j].node_list.computed.mask_scale_xaxis == 1)
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.scale_xaxis = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.scale_xaxis);
					}
					/* (7-7-7-6) scaleYaxis **OP**/
					if(decoded_item->road_segments[i].road_lane_set[j].node_list.computed.mask_scale_yaxis == 1)
					{
						printf("road_segments[%d].road_lane_set[%d].node_list.computed.scale_yaxis = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].node_list.computed.scale_yaxis);
					}
				}
				else
				{
					printf("road_segments[%d].road_lane_set[%d].node_list.node_list_choice = NODE_LIST2_NOTHING\n", i, j);
				}

				/* (7-7-8) connectsTo **OP**/
				if(decoded_item->road_segments[i].road_lane_set[j].mask_connects_to == 1)
				{
					printf("road_segments[%d].road_lane_set[%d].connects_to_num = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].connects_to_num);
					for(k = 0; k < decoded_item->road_segments[i].road_lane_set[j].connects_to_num; k++)
					{
						/* (7-7-8-1) connectingLane */
						printf("road_segments[%d].road_lane_set[%d].connects_to[%d].connecting_lane.lane = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.lane);
						if(decoded_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.mask_maneuver == 1)
						{
							printf("road_segments[%d].road_lane_set[%d].connects_to[%d].connecting_lane.maneuver = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.maneuver);
						}
						/* (7-7-8-2) remoteIntersection **OP**/
						if(decoded_item->road_segments[i].road_lane_set[j].connects_to[k].mask_remote_intersection == 1)
						{
							/* (7-7-8-2-1) region**OP**/
							if(decoded_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.mask_region == 1)
							{
								printf("road_segments[%d].road_lane_set[%d].connects_to[%d].remote_intersection.region = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.region);
							}
							/* (7-7-8-2-2) id*/
							printf("road_segments[%d].road_lane_set[%d].connects_to[%d].remote_intersection.id = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.id);
						}							
						/* (7-7-8-3) signalGroup **OP**/
						if(decoded_item->road_segments[i].road_lane_set[j].connects_to[k].mask_signal_group == 1)
						{
							printf("road_segments[%d].road_lane_set[%d].connects_to[%d].signal_group = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].connects_to[k].signal_group);
						}
						/* (7-7-8-4) userClass **OP**/
						if(decoded_item->road_segments[i].road_lane_set[j].connects_to[k].mask_user_class == 1)
						{
							printf("road_segments[%d].road_lane_set[%d].connects_to[%d].user_class = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].connects_to[k].user_class);
						}
						/* (7-7-8-5) connectionID **OP**/
						if(decoded_item->road_segments[i].road_lane_set[j].connects_to[k].mask_connection_id == 1)
						{
							printf("road_segments[%d].road_lane_set[%d].connects_to[%d].connection_id = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].connects_to[k].connection_id);
						}
					}
				}

				/* (7-7-9) overlays **OP**/
				if(decoded_item->road_segments[i].road_lane_set[j].mask_overlays == 1)
				{
					printf("road_segments[%d].road_lane_set[%d].overlays_num = %d\n", i, j, decoded_item->road_segments[i].road_lane_set[j].overlays_num);
					for(k = 0; k < decoded_item->road_segments[i].road_lane_set[j].overlays_num; k++)
					{
						printf("road_segments[%d].road_lane_set[%d].overlays[%d] = %d\n", i, j, k, decoded_item->road_segments[i].road_lane_set[j].overlays[k]);
					}
				}
			}
		}		
	}
	/*(8) dataParameters **OP**/
	if(decoded_item->mask_data_parameters == 1)
	{
		/*(8-1) processMethod **OP**/
		if(decoded_item->data_parameters.mask_process_method == 1)
		{
			printf("data_parameters.process_method = %s\n", decoded_item->data_parameters.process_method);
		}
		/*(8-2) processAgency **OP**/
		if(decoded_item->data_parameters.mask_process_agency == 1)
		{
			printf("data_parameters.process_agency = %s\n", decoded_item->data_parameters.process_agency);
		}
		/*(8-3) lastCheckedDate **OP**/
		if(decoded_item->data_parameters.mask_last_checked_date == 1)
		{
			printf("data_parameters.last_checked_date = %s\n", decoded_item->data_parameters.last_checked_date);
		}
		/*(8-4) geoidUsed **OP**/
		if(decoded_item->data_parameters.mask_geoid_used == 1)
		{
			printf("data_parameters.geoid_used = %s\n", decoded_item->data_parameters.geoid_used);
		}
	}
	/*(9)***restrictionList**OP**/
	if(decoded_item->mask_restriction_list == 1)
	{
		printf("restriction_list_num = %d\n", decoded_item->restriction_list_num);
		for(i = 0; i < decoded_item->restriction_list_num; i++)
		{
			/* (1) id */
			printf("restriction_list[%d].id = %d\n", i, decoded_item->restriction_list[i].id);
			/* (2) users */
			printf("restriction_list[%d].users_num = %d\n", i, decoded_item->restriction_list[i].users_num);
			for(j = 0; j < decoded_item->restriction_list[i].users_num; j++)
			{
				if(decoded_item->restriction_list[i].users[j].users_choice == RESTRICTION_USER_TYPE_BASICTYPE)
				{

					printf("restriction_list[%d].users[%d].basic_type = %d\n", i, j, decoded_item->restriction_list[i].users[j].basic_type);
				}
				else if(decoded_item->restriction_list[i].users[j].users_choice == RESTRICTION_USER_TYPE_REGIONAL)
				{
					printf("restriction_list[%d].users[%d].users_choice = RESTRICTION_USER_TYPE_REGIONAL\n", i, j);
				}	
				else
				{
					printf("restriction_list[%d].users[%d].users_choice = RESTRICTION_USER_TYPE_NOTHING\n", i, j);
				}
			}
		}		
	}
	/*(11)***crc**OP**/
	if(decoded_item->mask_crc == 1)
	{
		printf("crc = %x %x\n", decoded_item->crc[0], decoded_item->crc[1]);
	}

	printf("===============================\n");
}

int cmd_rxMap(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	map_standard_item *decoded_item = NULL;
	int err_map;

	int err;
	TX_TIMER stop_map_timer;
	wme_handle_t handle;
	unsigned char channel_number;
	unsigned char radio_number;
	unsigned int psid;
	struct event_message event;
	struct user_info user_info;
	struct wsm_info wsm_info;
	struct in_wsm in_wsm;

	if(argc != 3) { /* In threadx, cmd is not included in argc */
		printf("rxMap <psid> <radio_number> <channel_number> \n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("-- example --\n");
		printf("rxMap 123 0 172\n");
		return 1;
	}

	psid = atoi(argv[0]);
	radio_number = atoi(argv[1]);
	channel_number = atoi(argv[2]);

	err = wme_init(&handle);
	if(err != ACCEPT) {
		printf("wme_init = %d\n", err);
		return 0;
	}
	printf("wme_init success, handle = %d\n", handle);

	/* Add user service */
	user_info.action = ADD;
	memset(&user_info, 0, sizeof(user_info));
	user_info.radio_number = radio_number;
	user_info.request_type = ACCESS_UNCONDITIONAL;
	user_info.psid = psid;
	memcpy(user_info.src_mac, "\xFF\xFF\xFF\xFF\xFF\xFF", 6);
	user_info.service_priority = 1;
	user_info.channel_number = channel_number;
	user_info.extended_access = 65535;

	err = wme_user_service(&handle, &user_info);
	if(err != 0) {
		printf("wme_user_service = %d\n", err);
		wme_exit(&handle);
		return 0;
	}
	printf("wme_user_service to add success\n");

	wsm_info.action = ADD;
	wsm_info.psid = psid;
	err  = wme_wsm_service(&handle, &wsm_info);
	if(err != 0) {
		printf("wme_wsm_service = %d\n", err);
		wme_exit(&handle);
		return 0;
	}
	printf("wme_wsm_service to add success\n");

	while(1){
		err = wme_event_recv(&handle, &event, 1000);
		if(err != 0) {
			printf("wme_event_waiting = %d\n", err);
		if(err == -SYSTEM_INTR)
			continue;
		else
			return 0;
		}
		
		if(event.event == EVENT_CHANNEL){
			if(event.reason == REASON_CHANNEL_AVAILABLE){
				printf("Channel %d assigned ...\n", event.info.channel.channel);
				break;
			}
			//else {
			//	printf("Channel %d unassigned ...\n", event.info.channel.channel);
				//c_flag = 0;
			//}
			//break;
		}
	}

	printf("waiting 5s to recv wsm packets\n");
	stop_map = 0;
	tx_timer_create(&stop_map_timer, NULL, stop_map_func, 0, 5000, 1, TX_AUTO_ACTIVATE);
	while(stop_map == 0) {
		err = wme_wsm_recv(&handle, &in_wsm, 1000);
		if(err != 0) {
			printf("wsm_recv = %d\n", err);
			continue;
		}
		printf("wsm_recv success\n");
		printf("wsm data len = %d\n", in_wsm.length);

		decoded_item = (map_standard_item *)j2735_calloc(1, sizeof(map_standard_item));
		err_map = j2735r41_map_decode(decoded_item, in_wsm.data, MAX_MAP_BUFF_SIZE, 1);
		if(err_map < 0)
		{
			printf("decode error...\n");
			return -1;
		}

		if(SHOW_DECODED == 1){_show_map_standard_item(decoded_item);}
    
		j2735r41_map_free_struct(decoded_item);
	}

	wsm_info.action = DELETE;
	err  = wme_wsm_service(&handle, &wsm_info);
	if(err != 0) {
		printf("wme_wsm_service = %d\n", err);
		wme_exit(&handle);	
		return 0;
	}
	printf("wme_wsm_service to del success\n");

	user_info.action = DELETE;
	err  = wme_user_service(&handle, &user_info);
	if(err != 0) {
		printf("wme_user_service = %d\n", err);
		wme_exit(&handle);
		return 0;
	}
	printf("wme_user_service to del success\n");

	tx_timer_delete(&stop_map_timer);

	wme_exit(&handle);

	return 0;
}

int cmd_j2735r41_map(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */  	
	(void)command;  	
	(void)argv;  	
	(void)argc;
	(void)cli;

	map_standard_item *encode_item = NULL;
	map_standard_item *decoded_item = NULL;
	int err_map, loopCnt;
	int i, j, k, m;
	size_t size;
	unsigned char *encodeBuf = NULL;

	printf("%s", "Start J2735r41_MAP test!\n");	

	loopCnt = 0;
	while(loopCnt < 1)
	{
		encodeBuf = (unsigned char *)j2735_calloc(1, MAX_MAP_BUFF_SIZE);
		encode_item = (map_standard_item *)j2735_calloc(1, sizeof(map_standard_item));		
		/*(1) msgID */
		encode_item->msg_id = 7;
		/*(2) msgSubID **OP**/
		encode_item->mask_msg_sub_id = 1;
		encode_item->msg_sub_id = 255;	
		/*(3) msgIssueRevision */
		encode_item->msg_issue_revision = 127;
	    /*(4) layerType **OP**/
		encode_item->mask_layer_type = 1;
		encode_item->layer_type = 7;
		/*(5) layerID **OP**/
		encode_item->mask_layer_id = 1;
		encode_item->layer_id = 100;
		
		/*(6) intersections **OP**/
		encode_item->intersections = NULL;
		encode_item->mask_intersections = 1;
		encode_item->intersections_num = 1;
		encode_item->intersections = (intersection_geometry_item *)j2735_calloc(encode_item->intersections_num, sizeof(intersection_geometry_item));
		for(i = 0; i < encode_item->intersections_num; i++)
		{
			/* (6-1) name **OP**/
			encode_item->intersections[i].mask_name = 1;
			memset(encode_item->intersections[i].name, 0, 64);
			memcpy(encode_item->intersections[i].name, "MAP_intersections_name", strlen("MAP_intersections_name"));
			encode_item->intersections[i].name[strlen("MAP_intersections_name")+1] = '\0';
			/* (6-2) id */
			encode_item->intersections[i].id.mask_region = 1;
			encode_item->intersections[i].id.region = 65535;		
			encode_item->intersections[i].id.id = 65535;
			/* (6-3) revision */
			encode_item->intersections[i].revision = 127;
			/* (6-4) refPoint */
				/* (6-4-1) lat */
				encode_item->intersections[i].ref_point.lat = 900000001;
				/* (6-4-2) Long */
				encode_item->intersections[i].ref_point.longitude = 1800000001;
				/* (6-4-3) elevation **OP**/
				encode_item->intersections[i].ref_point.mask_elevation = 1;
				encode_item->intersections[i].ref_point.elevation = 61439;
			/* (6-5) laneWidth **OP**/
			encode_item->intersections[i].mask_lane_width = 0;
			encode_item->intersections[i].lane_width = 32767;		
			/* (6-6) speedLimits **OP**/
			encode_item->intersections[i].speed_limits = NULL;
			encode_item->intersections[i].mask_speed_limits = 1;
			encode_item->intersections[i].speed_limits_num = 1;
			encode_item->intersections[i].speed_limits = (regulatory_speed_limit_item *)j2735_calloc(encode_item->intersections[i].speed_limits_num, sizeof(regulatory_speed_limit_item));
			for(j = 0; j < encode_item->intersections[i].speed_limits_num; j++)
			{
				/* (6-6-1) type */
				encode_item->intersections[i].speed_limits[j].type = 12;
				/* (6-6-2) speed */
				encode_item->intersections[i].speed_limits[j].speed = 8191;
			}			
			/* (6-7) laneSet */
			encode_item->intersections[i].lane_set = NULL;
			encode_item->intersections[i].lane_set_num = 1;
			encode_item->intersections[i].lane_set = (generic_lane_item *)j2735_calloc(encode_item->intersections[i].lane_set_num, sizeof(generic_lane_item));
			for(j = 0; j < encode_item->intersections[i].lane_set_num; j++)
			{
				/* (6-7-1) laneID */
				encode_item->intersections[i].lane_set[j].lane_id = 255;
				/* (6-7-2) name **OP**/
				encode_item->intersections[i].lane_set[j].mask_name = 1;
				memset(encode_item->intersections[i].lane_set[j].name, 0, 64);
				memcpy(encode_item->intersections[i].lane_set[j].name, "LaneSet_name", strlen("LaneSet_name"));
				encode_item->intersections[i].lane_set[j].name[strlen("LaneSet_name")+1] = '\0';
				/* (6-7-3) ingressApproach **OP**/
				encode_item->intersections[i].lane_set[j].mask_ingress_approach = 1;
				encode_item->intersections[i].lane_set[j].ingress_approach = 15;			
				/* (6-7-4) egressApproach **OP**/
				encode_item->intersections[i].lane_set[j].mask_egress_approach = 1;
				encode_item->intersections[i].lane_set[j].egress_approach = 15;			
				/* (6-7-5) laneAttributes */
				encode_item->intersections[i].lane_set[j].lane_attributes.directional_use = 3;
				encode_item->intersections[i].lane_set[j].lane_attributes.shared_with = 256;
				encode_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice = LANE_TYPE_ATTRIBUTES_VEHICLE;
				encode_item->intersections[i].lane_set[j].lane_attributes.lane_type.vehicle = 1024;
				/* (6-7-6) maneuvers **OP**/
				encode_item->intersections[i].lane_set[j].mask_maneuvers = 1;
				encode_item->intersections[i].lane_set[j].maneuvers = 2048;
				/* (6-7-7) nodeList */
				encode_item->intersections[i].lane_set[j].node_list.nodes = NULL;
				encode_item->intersections[i].lane_set[j].node_list.node_list_choice = NODE_LIST2_NODES;
				encode_item->intersections[i].lane_set[j].node_list.nodes_num = 2;
				encode_item->intersections[i].lane_set[j].node_list.nodes = (node_item *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes_num, sizeof(node_item));
				for(k = 0; k < encode_item->intersections[i].lane_set[j].node_list.nodes_num; k++)
				{
					/* (6-7-7-1) delta */
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice = NODE_OFFSET_POINT_XY1;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy1.x = 511;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy1.y = 511;			
					/* (6-7-7-2) attributes **OP**/
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].mask_attributes = 1;
						/* (6-7-7-2-1) localNode **OP**/
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node = NULL;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_local_node = 1;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num = 4;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num, sizeof(unsigned char));
						for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num; m++)
						{
							encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node[m] = 11;
						}						
						/* (6-7-7-2-2) disabled **OP**/
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled = NULL;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_disabled = 1;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num = 3;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num, sizeof(unsigned char));
						for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num; m++)
						{
							encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled[m] = 37;
						}
						/* (6-7-7-2-3) enabled **OP**/
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled = NULL;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_enabled = 1;					
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num = 3;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num, sizeof(unsigned char));
						for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num; m++)
						{
							encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled[m] = 36;
						}													
						/* (6-7-7-2-4) data **OP**/
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data = NULL;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_data = 1;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num = 1;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data = (lane_data_attribute_item *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num, sizeof(lane_data_attribute_item));
						for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num; m++)
						{
							encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice = LANE_DATA_ATTRIBUTE_PATHENDPOINTANGLE;
							encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].path_end_point_angle = 150;
						}					
						/* (6-7-7-2-6) dWidth **OP**/
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_d_width = 1;					
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.d_width = 511;
						/* (6-7-7-2-7) dElevation **OP**/
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_d_elevation = 1;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.d_elevation = 510;
				}
				/* (6-7-8) connectsTo **OP**/
				encode_item->intersections[i].lane_set[j].connects_to = NULL;
				encode_item->intersections[i].lane_set[j].mask_connects_to = 1;
				encode_item->intersections[i].lane_set[j].connects_to_num = 1;
				encode_item->intersections[i].lane_set[j].connects_to = (connection_item *)j2735_calloc(encode_item->intersections[i].lane_set[j].connects_to_num, sizeof(connection_item));
				for(k = 0; k < encode_item->intersections[i].lane_set[j].connects_to_num; k++)
				{
					/* (6-7-8-1) connectingLane */
					encode_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.lane = 255;
					encode_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.mask_maneuver = 1;
					encode_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.maneuver = 512;				
					/* (6-7-8-2) remoteIntersection **OP**/
					encode_item->intersections[i].lane_set[j].connects_to[k].mask_remote_intersection = 1;				
						/* (6-7-8-2-1) region**OP**/
						encode_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.mask_region = 1;				
						encode_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.region = 65535;				
						/* (6-7-8-2-2) id*/
						encode_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.id = 65535;											
					/* (6-7-8-3) signalGroup **OP**/
					encode_item->intersections[i].lane_set[j].connects_to[k].mask_signal_group = 1;
					encode_item->intersections[i].lane_set[j].connects_to[k].signal_group = 255;
					/* (6-7-8-4) userClass **OP**/
					encode_item->intersections[i].lane_set[j].connects_to[k].mask_user_class = 1;
					encode_item->intersections[i].lane_set[j].connects_to[k].user_class = 254;
					/* (6-7-8-5) connectionID **OP**/
					encode_item->intersections[i].lane_set[j].connects_to[k].mask_connection_id = 1;
					encode_item->intersections[i].lane_set[j].connects_to[k].connection_id = 253;
				}
				/* (6-7-9) overlays **OP**/
				encode_item->intersections[i].lane_set[j].overlays = NULL;
				encode_item->intersections[i].lane_set[j].mask_overlays = 1;
				encode_item->intersections[i].lane_set[j].overlays_num = 4;
				encode_item->intersections[i].lane_set[j].overlays = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].overlays_num, sizeof(unsigned char));
				for(k = 0; k < encode_item->intersections[i].lane_set[j].overlays_num; k++)
				{
					encode_item->intersections[i].lane_set[j].overlays[k] = 5;
				}	
			}
		}		

		/*(7) roadSegments **OP**/
		encode_item->road_segments = NULL;
		encode_item->mask_road_segments = 1;
		encode_item->road_segments_num = 1;
		encode_item->road_segments = (road_segment_item *)j2735_calloc(encode_item->road_segments_num, sizeof(road_segment_item));
		for(i = 0; i < encode_item->road_segments_num; i++)
		{
			/* (7-1) name **OP**/
			encode_item->road_segments[i].mask_name = 1;
			memset(encode_item->road_segments[i].name, 0, 64);
			memcpy(encode_item->road_segments[i].name, "RoadSegments_name", strlen("RoadSegments_name"));
			encode_item->road_segments[i].name[strlen("RoadSegments_name")+1] = '\0';
			/* (7-2) id */
			encode_item->road_segments[i].id.mask_region = 1;		
			encode_item->road_segments[i].id.region = 65535;		
			encode_item->road_segments[i].id.id = 65534;
			/* (7-3) revision */
			encode_item->road_segments[i].revision = 126;
			/* (7-4) refPoint */
				/* (7-4-1) lat */
				encode_item->road_segments[i].ref_point.lat = 900000000;
				/* (7-4-2) Long */
				encode_item->road_segments[i].ref_point.longitude = 1800000000;
				/* (7-4-3) elevation **OP**/
				encode_item->road_segments[i].ref_point.mask_elevation = 1;		
				encode_item->road_segments[i].ref_point.elevation = 61438;						
			/* (7-5) laneWidth **OP**/
			encode_item->road_segments[i].mask_lane_width = 1;		
			encode_item->road_segments[i].lane_width = 32766;		
			/* (7-6) speedLimits **OP**/
			encode_item->road_segments[i].mask_speed_limits = 1;		
			encode_item->road_segments[i].speed_limits_num = 1;
			encode_item->road_segments[i].speed_limits = (regulatory_speed_limit_item *)j2735_calloc(encode_item->road_segments[i].speed_limits_num, sizeof(regulatory_speed_limit_item));
			for(j = 0; j < encode_item->road_segments[i].speed_limits_num; j++)
			{
				/* (7-6-1) type */
				encode_item->road_segments[i].speed_limits[j].type = 11;
				/* (7-6-2) speed */
				encode_item->road_segments[i].speed_limits[j].speed = 8190;
			}		
			/* (7-7) roadLaneSet */
			encode_item->road_segments[i].road_lane_set_num = 1;
			encode_item->road_segments[i].road_lane_set = (generic_lane_item *)j2735_calloc(encode_item->road_segments[i].road_lane_set_num, sizeof(generic_lane_item));
			for(j = 0; j < encode_item->road_segments[i].road_lane_set_num; j++)
			{
				/* (7-7-1) laneID */
				encode_item->road_segments[i].road_lane_set[j].lane_id = 254;
				/* (7-7-2) name **OP**/
				encode_item->road_segments[i].road_lane_set[j].mask_name = 1;
				memset(encode_item->road_segments[i].road_lane_set[j].name, 0, 64);
				memcpy(encode_item->road_segments[i].road_lane_set[j].name, "RoadLaneSet_name", strlen("RoadLaneSet_name"));
				encode_item->road_segments[i].road_lane_set[j].name[strlen("RoadLaneSet_name")+1] = '\0';
				/* (7-7-3) ingressApproach **OP**/
				encode_item->road_segments[i].road_lane_set[j].mask_ingress_approach = 1;			
				encode_item->road_segments[i].road_lane_set[j].ingress_approach = 14;			
				/* (7-7-4) egressApproach **OP**/
				encode_item->road_segments[i].road_lane_set[j].mask_egress_approach = 1;			
				encode_item->road_segments[i].road_lane_set[j].egress_approach = 14;			
				/* (7-7-5) laneAttributes */
				encode_item->road_segments[i].road_lane_set[j].lane_attributes.directional_use = 1;
				encode_item->road_segments[i].road_lane_set[j].lane_attributes.shared_with = 16;
				encode_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice = LANE_TYPE_ATTRIBUTES_CROSSWALK;
				encode_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.crosswalk = 512;
				/* (7-7-6) maneuvers **OP**/
				encode_item->road_segments[i].road_lane_set[j].mask_maneuvers = 1;
				encode_item->road_segments[i].road_lane_set[j].maneuvers = 8;			
				/* (7-7-7) nodeList */
				encode_item->road_segments[i].road_lane_set[j].node_list.node_list_choice = NODE_LIST2_COMPUTED;
					/* (7-7-7-1) referenceLaneId */
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.reference_lane_id = 254;
					/* (7-7-7-2) offsetXaxis */
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.offset_xaxis_choice = OFFSET_XAXIS_LARGE;
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.large = 32767;
					/* (7-7-7-3) offsetYaxis */
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.offset_yaxis_choice = OFFSET_YAXIS_LARGE;
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.large = 32766;
					/* (7-7-7-4) rotateXY **OP**/
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.mask_rotate_xy = 1;			
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.rotate_xy = 28800;			
					/* (7-7-7-5) scaleXaxis **OP**/
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.mask_scale_xaxis = 1;			
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.scale_xaxis = 2047;			
					/* (7-7-7-6) scaleYaxis **OP**/
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.mask_scale_yaxis = 1;			
					encode_item->road_segments[i].road_lane_set[j].node_list.computed.scale_yaxis = 2046;		
				/* (7-7-8) connectsTo **OP**/
				encode_item->road_segments[i].road_lane_set[j].mask_connects_to = 1;			
				encode_item->road_segments[i].road_lane_set[j].connects_to_num = 1;
				encode_item->road_segments[i].road_lane_set[j].connects_to = (connection_item *)j2735_calloc(encode_item->road_segments[i].road_lane_set[j].connects_to_num, sizeof(connection_item));
				for(k = 0; k < encode_item->road_segments[i].road_lane_set[j].connects_to_num; k++)
				{
					/* (7-7-8-1) connectingLane */
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.lane = 254;
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.mask_maneuver = 1;				
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.maneuver = 2;				
					/* (7-7-8-2) remoteIntersection **OP**/
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_remote_intersection = 1;				
						/* (7-7-8-2-1) region**OP**/
						encode_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.mask_region = 1;
						encode_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.region = 65533;				
						/* (7-7-8-2-2) id*/
						encode_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.id = 65532;											
					/* (7-7-8-3) signalGroup **OP**/
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_signal_group = 1;				
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].signal_group = 253;				
					/* (7-7-8-4) userClass **OP**/
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_user_class = 1;				
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].user_class = 251;				
					/* (7-7-8-5) connectionID **OP**/
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_connection_id = 1;				
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].connection_id = 250;				
				}
				/* (7-7-9) overlays **OP**/
				encode_item->road_segments[i].road_lane_set[j].mask_overlays = 1;			
				encode_item->road_segments[i].road_lane_set[j].overlays_num = 3;
				encode_item->road_segments[i].road_lane_set[j].overlays = (unsigned char *)j2735_calloc(encode_item->road_segments[i].road_lane_set[j].overlays_num, sizeof(unsigned char));
				for(k = 0; k < encode_item->road_segments[i].road_lane_set[j].overlays_num; k++)
				{
					encode_item->road_segments[i].road_lane_set[j].overlays[k] = 1;
				}			
			}
		}		
		
		/*(8) dataParameters **OP**/
		encode_item->mask_data_parameters = 1;
			/*(8-1) processMethod **OP**/
			encode_item->data_parameters.mask_process_method = 1;
			memset(encode_item->data_parameters.process_method, 0, 256);
			memset(encode_item->data_parameters.process_method, 0x41, 16);
			encode_item->data_parameters.process_method[16] = '\0';
			/*(8-2) processAgency **OP**/
			encode_item->data_parameters.mask_process_agency = 1;
			memset(encode_item->data_parameters.process_agency, 0, 256);
			memset(encode_item->data_parameters.process_agency, 0x42, 16);
			encode_item->data_parameters.process_agency[16] = '\0';
			/*(8-3) lastCheckedDate **OP**/
			encode_item->data_parameters.mask_last_checked_date = 1;
			memset(encode_item->data_parameters.last_checked_date, 0, 256);
			memset(encode_item->data_parameters.last_checked_date, 0x43, 16);
			encode_item->data_parameters.last_checked_date[16] = '\0';
			/*(8-4) geoidUsed **OP**/
			encode_item->data_parameters.mask_geoid_used = 1;
			memset(encode_item->data_parameters.geoid_used, 0, 256);
			memset(encode_item->data_parameters.geoid_used, 0x44, 16);
			encode_item->data_parameters.geoid_used[16] = '\0';
		/*(9)***restrictionList**OP**/
		encode_item->mask_restriction_list = 1;
		encode_item->restriction_list_num = 1;
		encode_item->restriction_list = (restriction_class_assignment_item *)j2735_calloc(encode_item->restriction_list_num, sizeof(restriction_class_assignment_item));
		for(i = 0; i < encode_item->restriction_list_num; i++)
		{
			/* (1) id */
			encode_item->restriction_list[i].id = 255;
			/* (2) users */
			encode_item->restriction_list[i].users_num = 2;
			encode_item->restriction_list[i].users = (restriction_user_type_item *)j2735_calloc(encode_item->restriction_list[i].users_num, sizeof(restriction_user_type_item));
			for(j = 0; j < encode_item->restriction_list[i].users_num; j++)
			{
				encode_item->restriction_list[i].users[j].users_choice = RESTRICTION_USER_TYPE_BASICTYPE;		
				encode_item->restriction_list[i].users[j].basic_type = 13;
			}
		}
		/*(10)***regional**OP**/
		encode_item->mask_regional = 0;
		/*(11)***crc**OP**/
		encode_item->mask_crc = 1;
		encode_item->crc[0] = 0x41;
		encode_item->crc[1] = 0x42;
		
		/********************************************************/

		if((loopCnt % 100) == 0)
			printf("MAP loopCnt = %d\n", loopCnt + 1);

		//if(SHOW_ENCODE == 1){_show_map_standard_item(encode_item);}

	   	err_map = j2735r41_map_encode(encode_item, encodeBuf, MAX_MAP_BUFF_SIZE, &size, 1);
		if(err_map < 0)
		{		
			printf("map encode fail...\n");		
			return -1;	
		}
		_cmd_map_free_struct(encode_item);

		decoded_item = (map_standard_item *)j2735_calloc(1, sizeof(map_standard_item));
		err_map = j2735r41_map_decode(decoded_item, encodeBuf, MAX_MAP_BUFF_SIZE , 1);
		if(err_map < 0)
		{
			printf("decode error...\n");
			return -1;
		}

		if(SHOW_DECODED == 1){_show_map_standard_item(decoded_item);}
		
		j2735r41_map_free_struct(decoded_item);
		j2735_free(encodeBuf);

		loopCnt++;
		(void) tx_thread_sleep(10);
	}

	printf("%s", "J2735r41_MAP Test command executed successfully.\n");
		
	return CLI_OK;	
}

int _encode_txMap(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	int i, j, k, m;
	map_standard_item *encode_item = NULL;

	encode_item = (map_standard_item *)j2735_calloc(1, sizeof(map_standard_item));
	/*(1) msgID */
	encode_item->msg_id = 7;
	/*(2) msgSubID **OP**/
	encode_item->mask_msg_sub_id = 1;
	encode_item->msg_sub_id = 255;	
	/*(3) msgIssueRevision */
	encode_item->msg_issue_revision = 127;
    /*(4) layerType **OP**/
	encode_item->mask_layer_type = 1;
	encode_item->layer_type = 7;
	/*(5) layerID **OP**/
	encode_item->mask_layer_id = 1;
	encode_item->layer_id = 100;
	
	/*(6) intersections **OP**/
	encode_item->intersections = NULL;
	encode_item->mask_intersections = 1;
	encode_item->intersections_num = 1;
	encode_item->intersections = (intersection_geometry_item *)j2735_calloc(encode_item->intersections_num, sizeof(intersection_geometry_item));
	for(i = 0; i < encode_item->intersections_num; i++)
	{
		/* (6-1) name **OP**/
		encode_item->intersections[i].mask_name = 1;
		memset(encode_item->intersections[i].name, 0, 64);
		memcpy(encode_item->intersections[i].name, "MAP_intersections_name", strlen("MAP_intersections_name"));
		encode_item->intersections[i].name[strlen("MAP_intersections_name")+1] = '\0';
		/* (6-2) id */
		encode_item->intersections[i].id.mask_region = 1;
		encode_item->intersections[i].id.region = 65535;		
		encode_item->intersections[i].id.id = 65535;
		/* (6-3) revision */
		encode_item->intersections[i].revision = 127;
		/* (6-4) refPoint */
			/* (6-4-1) lat */
			encode_item->intersections[i].ref_point.lat = 900000001;
			/* (6-4-2) Long */
			encode_item->intersections[i].ref_point.longitude = 1800000001;
			/* (6-4-3) elevation **OP**/
			encode_item->intersections[i].ref_point.mask_elevation = 1;
			encode_item->intersections[i].ref_point.elevation = 61439;
		/* (6-5) laneWidth **OP**/
		encode_item->intersections[i].mask_lane_width = 0;
		encode_item->intersections[i].lane_width = 32767;		
		/* (6-6) speedLimits **OP**/
		encode_item->intersections[i].speed_limits = NULL;
		encode_item->intersections[i].mask_speed_limits = 1;
		encode_item->intersections[i].speed_limits_num = 1;
		encode_item->intersections[i].speed_limits = (regulatory_speed_limit_item *)j2735_calloc(encode_item->intersections[i].speed_limits_num, sizeof(regulatory_speed_limit_item));
		for(j = 0; j < encode_item->intersections[i].speed_limits_num; j++)
		{
			/* (6-6-1) type */
			encode_item->intersections[i].speed_limits[j].type = 12;
			/* (6-6-2) speed */
			encode_item->intersections[i].speed_limits[j].speed = 8191;
		}			
		/* (6-7) laneSet */
		encode_item->intersections[i].lane_set = NULL;
		encode_item->intersections[i].lane_set_num = 1;
		encode_item->intersections[i].lane_set = (generic_lane_item *)j2735_calloc(encode_item->intersections[i].lane_set_num, sizeof(generic_lane_item));
		for(j = 0; j < encode_item->intersections[i].lane_set_num; j++)
		{
			/* (6-7-1) laneID */
			encode_item->intersections[i].lane_set[j].lane_id = 255;
			/* (6-7-2) name **OP**/
			encode_item->intersections[i].lane_set[j].mask_name = 1;
			memset(encode_item->intersections[i].lane_set[j].name, 0, 64);
			memcpy(encode_item->intersections[i].lane_set[j].name, "LaneSet_name", strlen("LaneSet_name"));
			encode_item->intersections[i].lane_set[j].name[strlen("LaneSet_name")+1] = '\0';
			/* (6-7-3) ingressApproach **OP**/
			encode_item->intersections[i].lane_set[j].mask_ingress_approach = 1;
			encode_item->intersections[i].lane_set[j].ingress_approach = 15;			
			/* (6-7-4) egressApproach **OP**/
			encode_item->intersections[i].lane_set[j].mask_egress_approach = 1;
			encode_item->intersections[i].lane_set[j].egress_approach = 15;			
			/* (6-7-5) laneAttributes */
			encode_item->intersections[i].lane_set[j].lane_attributes.directional_use = 3;
			encode_item->intersections[i].lane_set[j].lane_attributes.shared_with = 256;
			encode_item->intersections[i].lane_set[j].lane_attributes.lane_type.lane_type_choice = LANE_TYPE_ATTRIBUTES_VEHICLE;
			encode_item->intersections[i].lane_set[j].lane_attributes.lane_type.vehicle = 1024;
			/* (6-7-6) maneuvers **OP**/
			encode_item->intersections[i].lane_set[j].mask_maneuvers = 1;
			encode_item->intersections[i].lane_set[j].maneuvers = 2048;
			/* (6-7-7) nodeList */
			encode_item->intersections[i].lane_set[j].node_list.nodes = NULL;
			encode_item->intersections[i].lane_set[j].node_list.node_list_choice = NODE_LIST2_NODES;
			encode_item->intersections[i].lane_set[j].node_list.nodes_num = 2;
			encode_item->intersections[i].lane_set[j].node_list.nodes = (node_item *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes_num, sizeof(node_item));
			for(k = 0; k < encode_item->intersections[i].lane_set[j].node_list.nodes_num; k++)
			{
				/* (6-7-7-1) delta */
				encode_item->intersections[i].lane_set[j].node_list.nodes[k].delta.choice = NODE_OFFSET_POINT_XY1;
				encode_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy1.x = 511;
				encode_item->intersections[i].lane_set[j].node_list.nodes[k].delta.node_xy1.y = 511;			
				/* (6-7-7-2) attributes **OP**/
				encode_item->intersections[i].lane_set[j].node_list.nodes[k].mask_attributes = 1;
					/* (6-7-7-2-1) localNode **OP**/
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node = NULL;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_local_node = 1;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num = 4;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num, sizeof(unsigned char));
					for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node_num; m++)
					{
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.local_node[m] = 11;
					}						
					/* (6-7-7-2-2) disabled **OP**/
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled = NULL;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_disabled = 1;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num = 3;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num, sizeof(unsigned char));
					for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled_num; m++)
					{
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.disabled[m] = 37;
					}
					/* (6-7-7-2-3) enabled **OP**/
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled = NULL;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_enabled = 1;					
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num = 3;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num, sizeof(unsigned char));
					for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled_num; m++)
					{
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.enabled[m] = 36;
					}													
					/* (6-7-7-2-4) data **OP**/
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data = NULL;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_data = 1;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num = 1;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data = (lane_data_attribute_item *)j2735_calloc(encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num, sizeof(lane_data_attribute_item));
					for(m = 0; m < encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data_num; m++)
					{
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].lane_data_attribute_choice = LANE_DATA_ATTRIBUTE_PATHENDPOINTANGLE;
						encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.data[m].path_end_point_angle = 150;
					}					
					/* (6-7-7-2-6) dWidth **OP**/
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_d_width = 1;					
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.d_width = 511;
					/* (6-7-7-2-7) dElevation **OP**/
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.mask_d_elevation = 1;
					encode_item->intersections[i].lane_set[j].node_list.nodes[k].attributes.d_elevation = 510;
			}
			/* (6-7-8) connectsTo **OP**/
			encode_item->intersections[i].lane_set[j].connects_to = NULL;
			encode_item->intersections[i].lane_set[j].mask_connects_to = 1;
			encode_item->intersections[i].lane_set[j].connects_to_num = 1;
			encode_item->intersections[i].lane_set[j].connects_to = (connection_item *)j2735_calloc(encode_item->intersections[i].lane_set[j].connects_to_num, sizeof(connection_item));
			for(k = 0; k < encode_item->intersections[i].lane_set[j].connects_to_num; k++)
			{
				/* (6-7-8-1) connectingLane */
				encode_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.lane = 255;
				encode_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.mask_maneuver = 1;
				encode_item->intersections[i].lane_set[j].connects_to[k].connecting_lane.maneuver = 512;				
				/* (6-7-8-2) remoteIntersection **OP**/
				encode_item->intersections[i].lane_set[j].connects_to[k].mask_remote_intersection = 1;				
					/* (6-7-8-2-1) region**OP**/
					encode_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.mask_region = 1;				
					encode_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.region = 65535;				
					/* (6-7-8-2-2) id*/
					encode_item->intersections[i].lane_set[j].connects_to[k].remote_intersection.id = 65535;											
				/* (6-7-8-3) signalGroup **OP**/
				encode_item->intersections[i].lane_set[j].connects_to[k].mask_signal_group = 1;
				encode_item->intersections[i].lane_set[j].connects_to[k].signal_group = 255;
				/* (6-7-8-4) userClass **OP**/
				encode_item->intersections[i].lane_set[j].connects_to[k].mask_user_class = 1;
				encode_item->intersections[i].lane_set[j].connects_to[k].user_class = 254;
				/* (6-7-8-5) connectionID **OP**/
				encode_item->intersections[i].lane_set[j].connects_to[k].mask_connection_id = 1;
				encode_item->intersections[i].lane_set[j].connects_to[k].connection_id = 253;
			}
			/* (6-7-9) overlays **OP**/
			encode_item->intersections[i].lane_set[j].overlays = NULL;
			encode_item->intersections[i].lane_set[j].mask_overlays = 1;
			encode_item->intersections[i].lane_set[j].overlays_num = 4;
			encode_item->intersections[i].lane_set[j].overlays = (unsigned char *)j2735_calloc(encode_item->intersections[i].lane_set[j].overlays_num, sizeof(unsigned char));
			for(k = 0; k < encode_item->intersections[i].lane_set[j].overlays_num; k++)
			{
				encode_item->intersections[i].lane_set[j].overlays[k] = 5;
			}	
		}
	}		

	/*(7) roadSegments **OP**/
	encode_item->road_segments = NULL;
	encode_item->mask_road_segments = 1;
	encode_item->road_segments_num = 1;
	encode_item->road_segments = (road_segment_item *)j2735_calloc(encode_item->road_segments_num, sizeof(road_segment_item));
	for(i = 0; i < encode_item->road_segments_num; i++)
	{
		/* (7-1) name **OP**/
		encode_item->road_segments[i].mask_name = 1;
		memset(encode_item->road_segments[i].name, 0, 64);
		memcpy(encode_item->road_segments[i].name, "RoadSegments_name", strlen("RoadSegments_name"));
		encode_item->road_segments[i].name[strlen("RoadSegments_name")+1] = '\0';
		/* (7-2) id */
		encode_item->road_segments[i].id.mask_region = 1;		
		encode_item->road_segments[i].id.region = 65535;		
		encode_item->road_segments[i].id.id = 65534;
		/* (7-3) revision */
		encode_item->road_segments[i].revision = 126;
		/* (7-4) refPoint */
			/* (7-4-1) lat */
			encode_item->road_segments[i].ref_point.lat = 900000000;
			/* (7-4-2) Long */
			encode_item->road_segments[i].ref_point.longitude = 1800000000;
			/* (7-4-3) elevation **OP**/
			encode_item->road_segments[i].ref_point.mask_elevation = 1;		
			encode_item->road_segments[i].ref_point.elevation = 61438;						
		/* (7-5) laneWidth **OP**/
		encode_item->road_segments[i].mask_lane_width = 1;		
		encode_item->road_segments[i].lane_width = 32766;		
		/* (7-6) speedLimits **OP**/
		encode_item->road_segments[i].mask_speed_limits = 1;		
		encode_item->road_segments[i].speed_limits_num = 1;
		encode_item->road_segments[i].speed_limits = (regulatory_speed_limit_item *)j2735_calloc(encode_item->road_segments[i].speed_limits_num, sizeof(regulatory_speed_limit_item));
		for(j = 0; j < encode_item->road_segments[i].speed_limits_num; j++)
		{
			/* (7-6-1) type */
			encode_item->road_segments[i].speed_limits[j].type = 11;
			/* (7-6-2) speed */
			encode_item->road_segments[i].speed_limits[j].speed = 8190;
		}		
		/* (7-7) roadLaneSet */
		encode_item->road_segments[i].road_lane_set_num = 1;
		encode_item->road_segments[i].road_lane_set = (generic_lane_item *)j2735_calloc(encode_item->road_segments[i].road_lane_set_num, sizeof(generic_lane_item));
		for(j = 0; j < encode_item->road_segments[i].road_lane_set_num; j++)
		{
			/* (7-7-1) laneID */
			encode_item->road_segments[i].road_lane_set[j].lane_id = 254;
			/* (7-7-2) name **OP**/
			encode_item->road_segments[i].road_lane_set[j].mask_name = 1;
			memset(encode_item->road_segments[i].road_lane_set[j].name, 0, 64);
			memcpy(encode_item->road_segments[i].road_lane_set[j].name, "RoadLaneSet_name", strlen("RoadLaneSet_name"));
			encode_item->road_segments[i].road_lane_set[j].name[strlen("RoadLaneSet_name")+1] = '\0';
			/* (7-7-3) ingressApproach **OP**/
			encode_item->road_segments[i].road_lane_set[j].mask_ingress_approach = 1;			
			encode_item->road_segments[i].road_lane_set[j].ingress_approach = 14;			
			/* (7-7-4) egressApproach **OP**/
			encode_item->road_segments[i].road_lane_set[j].mask_egress_approach = 1;			
			encode_item->road_segments[i].road_lane_set[j].egress_approach = 14;			
			/* (7-7-5) laneAttributes */
			encode_item->road_segments[i].road_lane_set[j].lane_attributes.directional_use = 1;
			encode_item->road_segments[i].road_lane_set[j].lane_attributes.shared_with = 16;
			encode_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.lane_type_choice = LANE_TYPE_ATTRIBUTES_CROSSWALK;
			encode_item->road_segments[i].road_lane_set[j].lane_attributes.lane_type.crosswalk = 512;
			/* (7-7-6) maneuvers **OP**/
			encode_item->road_segments[i].road_lane_set[j].mask_maneuvers = 1;
			encode_item->road_segments[i].road_lane_set[j].maneuvers = 8;			
			/* (7-7-7) nodeList */
			encode_item->road_segments[i].road_lane_set[j].node_list.node_list_choice = NODE_LIST2_COMPUTED;
				/* (7-7-7-1) referenceLaneId */
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.reference_lane_id = 254;
				/* (7-7-7-2) offsetXaxis */
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.offset_xaxis_choice = OFFSET_XAXIS_LARGE;
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_xaxis_item.large = 32767;
				/* (7-7-7-3) offsetYaxis */
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.offset_yaxis_choice = OFFSET_YAXIS_LARGE;
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.offset_yaxis_item.large = 32766;
				/* (7-7-7-4) rotateXY **OP**/
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.mask_rotate_xy = 1;			
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.rotate_xy = 28800;			
				/* (7-7-7-5) scaleXaxis **OP**/
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.mask_scale_xaxis = 1;			
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.scale_xaxis = 2047;			
				/* (7-7-7-6) scaleYaxis **OP**/
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.mask_scale_yaxis = 1;			
				encode_item->road_segments[i].road_lane_set[j].node_list.computed.scale_yaxis = 2046;		
			/* (7-7-8) connectsTo **OP**/
			encode_item->road_segments[i].road_lane_set[j].mask_connects_to = 1;			
			encode_item->road_segments[i].road_lane_set[j].connects_to_num = 1;
			encode_item->road_segments[i].road_lane_set[j].connects_to = (connection_item *)j2735_calloc(encode_item->road_segments[i].road_lane_set[j].connects_to_num, sizeof(connection_item));
			for(k = 0; k < encode_item->road_segments[i].road_lane_set[j].connects_to_num; k++)
			{
				/* (7-7-8-1) connectingLane */
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.lane = 254;
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.mask_maneuver = 1;				
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].connecting_lane.maneuver = 2;				
				/* (7-7-8-2) remoteIntersection **OP**/
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_remote_intersection = 1;				
					/* (7-7-8-2-1) region**OP**/
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.mask_region = 1;
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.region = 65533;				
					/* (7-7-8-2-2) id*/
					encode_item->road_segments[i].road_lane_set[j].connects_to[k].remote_intersection.id = 65532;											
				/* (7-7-8-3) signalGroup **OP**/
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_signal_group = 1;				
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].signal_group = 253;				
				/* (7-7-8-4) userClass **OP**/
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_user_class = 1;				
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].user_class = 251;				
				/* (7-7-8-5) connectionID **OP**/
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].mask_connection_id = 1;				
				encode_item->road_segments[i].road_lane_set[j].connects_to[k].connection_id = 250;				
			}
			/* (7-7-9) overlays **OP**/
			encode_item->road_segments[i].road_lane_set[j].mask_overlays = 1;			
			encode_item->road_segments[i].road_lane_set[j].overlays_num = 3;
			encode_item->road_segments[i].road_lane_set[j].overlays = (unsigned char *)j2735_calloc(encode_item->road_segments[i].road_lane_set[j].overlays_num, sizeof(unsigned char));
			for(k = 0; k < encode_item->road_segments[i].road_lane_set[j].overlays_num; k++)
			{
				encode_item->road_segments[i].road_lane_set[j].overlays[k] = 1;
			}			
		}
	}		
	
	/*(8) dataParameters **OP**/
	encode_item->mask_data_parameters = 1;
		/*(8-1) processMethod **OP**/
		encode_item->data_parameters.mask_process_method = 1;
		memset(encode_item->data_parameters.process_method, 0, 256);
		memset(encode_item->data_parameters.process_method, 0x41, 16);
		encode_item->data_parameters.process_method[16] = '\0';
		/*(8-2) processAgency **OP**/
		encode_item->data_parameters.mask_process_agency = 1;
		memset(encode_item->data_parameters.process_agency, 0, 256);
		memset(encode_item->data_parameters.process_agency, 0x42, 16);
		encode_item->data_parameters.process_agency[16] = '\0';
		/*(8-3) lastCheckedDate **OP**/
		encode_item->data_parameters.mask_last_checked_date = 1;
		memset(encode_item->data_parameters.last_checked_date, 0, 256);
		memset(encode_item->data_parameters.last_checked_date, 0x43, 16);
		encode_item->data_parameters.last_checked_date[16] = '\0';
		/*(8-4) geoidUsed **OP**/
		encode_item->data_parameters.mask_geoid_used = 1;
		memset(encode_item->data_parameters.geoid_used, 0, 256);
		memset(encode_item->data_parameters.geoid_used, 0x44, 16);
		encode_item->data_parameters.geoid_used[16] = '\0';
	/*(9)***restrictionList**OP**/
	encode_item->mask_restriction_list = 1;
	encode_item->restriction_list_num = 1;
	encode_item->restriction_list = (restriction_class_assignment_item *)j2735_calloc(encode_item->restriction_list_num, sizeof(restriction_class_assignment_item));
	for(i = 0; i < encode_item->restriction_list_num; i++)
	{
		/* (1) id */
		encode_item->restriction_list[i].id = 255;
		/* (2) users */
		encode_item->restriction_list[i].users_num = 2;
		encode_item->restriction_list[i].users = (restriction_user_type_item *)j2735_calloc(encode_item->restriction_list[i].users_num, sizeof(restriction_user_type_item));
		for(j = 0; j < encode_item->restriction_list[i].users_num; j++)
		{
			encode_item->restriction_list[i].users[j].users_choice = RESTRICTION_USER_TYPE_BASICTYPE;		
			encode_item->restriction_list[i].users[j].basic_type = 13;
		}
	}
	/*(10)***regional**OP**/
	encode_item->mask_regional = 0;
	/*(11)***crc**OP**/
	encode_item->mask_crc = 1;
	encode_item->crc[0] = 0x41;
	encode_item->crc[1] = 0x42;

	/*************************************************************/
	
	if(SHOW_ENCODE == 1)
	{
		printf("\nMAP Encode Data:\n");
		_show_map_standard_item(encode_item);
	}
	
	return j2735r41_map_encode(encode_item, encodeBuf, input_buf_size, size, 1);
}

int cmd_txMap(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */  	
	(void)command;  	
	(void)argv;
	(void)argc;
	(void)cli;

	int err, i;
	wme_handle_t handle;
	unsigned short num;
	unsigned char request_type, channel_number, is_switch, data_rate, tx_power, priority, interval;
	unsigned int psid;
	unsigned char macAddr_c[6] = {0};
	unsigned int macAddr_i[6];
	struct event_message event;
	struct user_info user_info;
	struct out_wsm out_wsm;
	int c_flag, s_flag;
	size_t len;
	unsigned char *encodeBuf = NULL;

	if(argc != 10) { /* In threadx, cmd is not included in argc */
		printf("txMap <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<request_type> access of user service, 0:MATCH/1:UNCONDITIONAL/2:NO ACCESS\n");
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("<is_switch> channel mode of user service, 0:continuous/1:channel switch\n");
		printf("<data_rate> data rate of sending WSMV2 packets, 6 9 12 18 24 36 48 54\n");
		printf("<tx_power> tx power level of sending WSMV2 packets, 12- 25, unit:dBm\n");
		printf("<priority> priority of user service, 0 - 63\n");
		printf("<dest_mac> peer mac address of sending WSMV2 packets, format: XX:XX:XX:XX:XX:XX\n");
		printf("<interval> the interval of sending packets, 0 - 1000, unit:ms\n");
		printf("<num> the number of sending packets, 1 - 10000\n");		
		printf("-- example --\n");
		printf("txMap 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
		return 1;
	}

	//psid = (unsigned int) atoll(argv[1]);
	psid = atoi(argv[0]);
	request_type = atoi(argv[1]);
	channel_number = atoi(argv[2]);
	is_switch = atoi(argv[3]);
	data_rate = atoi(argv[4]);
	tx_power = atoi(argv[5]);
	priority = atoi(argv[6]);

	/* Convert the mac addr */
	if(sscanf(argv[7], "%2x:%2x:%2x:%2x:%2x:%2x\n", &macAddr_i[0], &macAddr_i[1], &macAddr_i[2], &macAddr_i[3], &macAddr_i[4], &macAddr_i[5]) != 6) {
		printf("peerMacAddress invalid...\n");
		return 0;
	}
	for(i = 0; i < 6; i++) {
		macAddr_c[i] = (unsigned char) macAddr_i[i];
	}

	interval = atoi(argv[8]);
	num = atoi(argv[9]);

	err = wme_init(&handle);
	if(err != 0) {
		printf("wme_init = %d\n", err);
		return 0;
	}
	printf("wme_init success, handle = %d\n", handle);

	/* Add user service */
	user_info.action = ADD;
	memset(&user_info, 0, sizeof(user_info));
	user_info.radio_number = 0;
	user_info.request_type = request_type;
	user_info.psid = psid;
	memcpy(user_info.src_mac, "\xFF\xFF\xFF\xFF\xFF\xFF", 6);
	user_info.service_priority = priority;
	user_info.channel_number = channel_number;
	user_info.extended_access = (is_switch == 1) ? 0 : 65535;

	err = wme_user_service(&handle, &user_info);
	if(err != 0) {
		printf("wme_user_service = %d\n", err);
		return 0;
	}
	printf("wme_user_service to add success\n");

	c_flag = 0;
	s_flag = request_type == 1 ? 1 : 0;
	while(1){
		err = wme_event_recv(&handle, &event, 0xFFFFFFFF);
		if(err != 0) {
			printf("wme_event_waiting = %d\n", err);
			return 0;
		}
		
		if(event.event == EVENT_CHANNEL){
			if(event.reason == REASON_CHANNEL_AVAILABLE){
				printf("Channel %d assigned ...\n", event.info.channel.channel);
				c_flag = 1;
			}
			else {
				printf("Channel %d unassigned ...\n", event.info.channel.channel);
				c_flag = 0;
			}
		}

		if(event.event == EVENT_SERVICE){
			if(event.reason == REASON_SERVICE_AVAILABLE){
				printf("Service available ...\n");
				s_flag = 1;
			}
			else {
				printf("Service unavailable ...\n");
				s_flag = 0;
			}
		}

		if(c_flag != 0 && s_flag != 0)
			break;
	}

	out_wsm.psid = psid;	
	out_wsm.channel_number = channel_number;
	out_wsm.data_rate = data_rate;
	out_wsm.txpwr_level = tx_power;
	out_wsm.extensions.channel = 1;
	out_wsm.extensions.rate = 1;
	out_wsm.extensions.power = 1;
	out_wsm.wsm_type = EID_WSMP;
	memcpy(out_wsm.dest_mac, macAddr_c, 6);

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_MAP_BUFF_SIZE);
	_encode_txMap(encodeBuf, MAX_MAP_BUFF_SIZE, &len);
	
	memcpy(out_wsm.data, encodeBuf, len);
	out_wsm.length = len;

	j2735_free(encodeBuf);

	while(num != 0){
		err = wme_wsm_send(&handle, &out_wsm);
		if(err != 0) {
			printf("wsm_send = %d\n", err);
			return 0;
		}
		printf("wsm_send success\n");
		num--;
		(void) tx_thread_sleep(interval * 10);
	}

	user_info.action = DELETE;
	err = wme_user_service(&handle, &user_info);
	if(err != 0) {
		printf("wme_user_service = %d\n", err);
		return 0;
	}
	printf("wme_user_service to delete success\n");

	wme_exit(&handle);

	printf("%s", "cmd_txMap command executed successfully.\n");
	return CLI_OK;
}

