#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <linuxapi.h>
#include <tx_api.h>
#include <craton/debug.h>
#include "error_code.h"

#include <j2735r41_tim_api.h>
#include "j2735r41_common.h"

#include <libcli.h>
#include <craton/cli.h>
#include <atlk/nav_service.h>

#include <dot3_common.h>
#include <dot3_api.h>
#include <j2735_mem.h>

#define SHOW_ENCODE 1
#define SHOW_DECODED 1

static int stop_tim;

 void stop_tim_func(ULONG input)
{
	(void)input;
	stop_tim = 1;
	return;
}

void _cmd_tim_free_struct(tim_standard_item *free_tim_item)
{
	int i, j;

	if(free_tim_item == NULL){
		debug_printf("[SSM] Error: free_ssm_struct() input is NULL\n");
	}else{
		if(free_tim_item->data_frames != NULL){
			for(i = 0; i < free_tim_item->data_frames_num; i++){
				if(free_tim_item->data_frames[i].common_anchor != NULL){
					j2735_free(free_tim_item->data_frames[i].common_anchor);
				}

				if(free_tim_item->data_frames[i].regions != NULL){
					for(j = 0; j < free_tim_item->data_frames[i].regions_num; j++){
						if(free_tim_item->data_frames[i].regions[j].shape_point_set != NULL){
							if(free_tim_item->data_frames[i].regions[j].shape_point_set->node_list != NULL){
								j2735_free(free_tim_item->data_frames[i].regions[j].shape_point_set->node_list);								
							}
							
							j2735_free(free_tim_item->data_frames[i].regions[j].shape_point_set);
						}

						if(free_tim_item->data_frames[i].regions[j].circle != NULL){
							j2735_free(free_tim_item->data_frames[i].regions[j].circle);
						}

						if(free_tim_item->data_frames[i].regions[j].region_point_set != NULL){
							if(free_tim_item->data_frames[i].regions[j].region_point_set->node_list != NULL){
								j2735_free(free_tim_item->data_frames[i].regions[j].region_point_set->node_list);
							}
							
							j2735_free(free_tim_item->data_frames[i].regions[j].region_point_set);
						}
					}
					
					j2735_free(free_tim_item->data_frames[i].regions);
				}				

				if(free_tim_item->data_frames[i].advisory != NULL){
					j2735_free(free_tim_item->data_frames[i].advisory);
				}				

				if(free_tim_item->data_frames[i].work_zone != NULL){
					j2735_free(free_tim_item->data_frames[i].work_zone);
				}				

				if(free_tim_item->data_frames[i].generic_sign != NULL){
					j2735_free(free_tim_item->data_frames[i].generic_sign);
				}				

				if(free_tim_item->data_frames[i].speed_limit != NULL){
					j2735_free(free_tim_item->data_frames[i].speed_limit);
				}				

				if(free_tim_item->data_frames[i].exit_service != NULL){
					j2735_free(free_tim_item->data_frames[i].exit_service);
				}
			}
			
			j2735_free(free_tim_item->data_frames);
		}

		j2735_free(free_tim_item);
	}
}

void _show_tim_standard_item(tim_standard_item *decoded_item)
{
	int i, j, k;
	
	printf("==========TIM Message==========\n");
	/*(1) msgID */
	printf("msg_id = %d\n", decoded_item->msg_id);
	/*(2) packetID *OP**/
	if(decoded_item->mask_packet_id == 1)
	{
		printf("packet_id = %s\n", decoded_item->packet_id);
	}
	/*(3) urlB *OP**/
	if(decoded_item->mask_url_b == 1)
	{
		printf("url_b = %s\n", decoded_item->url_b);
	}
	/*(4) dataFrameCount *OP*/
	if(decoded_item->mask_data_frame_count == 1)
	{
		printf("data_frame_count = %d\n", decoded_item->data_frame_count);
	}
	/*(5) dataFrames*/
	printf("data_frames_num = %d\n", decoded_item->data_frames_num);
	for(i = 0; i < decoded_item->data_frames_num; i++)
	{
		/*(5-1)***dataFrames->frameType****/
		printf("data_frames[%d].frame_type = %d\n", i, decoded_item->data_frames[i].frame_type);
		/*(5-2)***dataFrames->msgId(CHOICE_FURTHERINFOID)****/
		if(decoded_item->data_frames[i].msg_id_choice == MSG_ID_FURTHER_INFO_ID)
		{
			printf("data_frames[%d].further_info_id = %s\n", i, decoded_item->data_frames[i].further_info_id);
		}
		/*(5-2)***dataFrames->msgId(CHOICE_ROADSIGNID****/
		else if(decoded_item->data_frames[i].msg_id_choice == MSG_ID_ROAD_SIGN_ID)
		{
			/*(5-2-1)***dataFrames->msgId(roadSignID)->position******/
			printf("data_frames[%d].road_sign_id.position.lat = %li\n", i, decoded_item->data_frames[i].road_sign_id.position.lat);
			printf("data_frames[%d].road_sign_id.position.longitude = %li\n", i, decoded_item->data_frames[i].road_sign_id.position.longitude);
			if(decoded_item->data_frames[i].road_sign_id.position.mask_elevation == 1)
			{
				printf("data_frames[%d].road_sign_id.position.elevation = %d\n", i, decoded_item->data_frames[i].road_sign_id.position.elevation);
			}
			/*(5-2-2)***dataFrames->msgId(roadSignID)->viewAngle******/
			printf("data_frames[%d].road_sign_id.view_angle = %x %x\n", i, decoded_item->data_frames[i].road_sign_id.view_angle[0], decoded_item->data_frames[i].road_sign_id.view_angle[1]);
			/*(5-2-3)***dataFrames->msgId(roadSignID)->mutcdCode******/
			if(decoded_item->data_frames[i].road_sign_id.mask_mutcd_code == 1)
			{
				printf("data_frames[%d].road_sign_id.mutcd_code = %d\n", i, decoded_item->data_frames[i].road_sign_id.mutcd_code);
			}				
			/*(5-2-4)***dataFrames->msgId(roadSignID)->crc***OP***/
			if(decoded_item->data_frames[i].road_sign_id.mask_crc == 1)
			{
				printf("data_frames[%d].road_sign_id.crc = %s\n", i, decoded_item->data_frames[i].road_sign_id.crc);
			}
		}else{
			printf("data_frames[i].msg_id_choice = MSG_ID_NOTHING\n");
		}
		/*(5-3)***dataFrames->startYear******OP***/
		if(decoded_item->data_frames[i].mask_start_year == 1)
		{
			printf("data_frames[%d].start_year = %d\n", i, decoded_item->data_frames[i].start_year);
		}
		/*(5-4)***dataFrames->startTime****/
		printf("data_frames[%d].start_time = %d\n", i, decoded_item->data_frames[i].start_time);
		/*(5-5)***dataFrames->duratonTime****/
		printf("data_frames[%d].duraton_time = %d\n", i, decoded_item->data_frames[i].duraton_time);
		/*(5-6)***dataFrames->priority****/
		printf("data_frames[%d].priority = %d\n", i, decoded_item->data_frames[i].priority);
		/*(5-7)***dataFrames->commonAnchor****OP***/
		if(decoded_item->data_frames[i].mask_common_anchor == 1)
		{
			printf("data_frames[%d].common_anchor->lat = %li\n", i, decoded_item->data_frames[i].common_anchor->lat);
			printf("data_frames[%d].common_anchor->longitude = %li\n", i, decoded_item->data_frames[i].common_anchor->longitude);
			if(decoded_item->data_frames[i].common_anchor->mask_elevation == 1)
			{
				printf("data_frames[%d].common_anchor->elevation = %d\n", i, decoded_item->data_frames[i].common_anchor->elevation);
			}
		}
		/*(5-8)***dataFrames->commonLaneWidth****OP***/
		if(decoded_item->data_frames[i].mask_common_lane_width == 1)
		{
			printf("data_frames[%d].common_lane_width = %d\n", i, decoded_item->data_frames[i].common_lane_width);
		}
		/*(5-9)***dataFrames->commonDirectionality**OP***/
		if(decoded_item->data_frames[i].mask_common_directionality == 1)
		{
			printf("data_frames[%d].common_directionality = %d\n", i, decoded_item->data_frames[i].common_directionality);
		}		
		/*(5-10)***dataFrames->regions(SEQUENCE OF)****/			
		printf("data_frames[%d].regions_num = %d\n", i, decoded_item->data_frames[i].regions_num);
		for(j = 0; j < decoded_item->data_frames[i].regions_num; j++)
		{
			/*(5-10-1)***dataFrames->regions->direction****/
			printf("data_frames[%d].regions[%d].direction = %x %x\n", i, j, decoded_item->data_frames[i].regions[j].direction[0], decoded_item->data_frames[i].regions[j].direction[1]);
			/*(5-10-2)***dataFrames->regions->extent**OP***/
			if(decoded_item->data_frames[i].regions[j].mask_extent == 1)
			{
				printf("data_frames[%d].regions[%d].extent = %d\n", i, j, decoded_item->data_frames[i].regions[j].extent);
			}
			/*(5-10-3)***dataFrames->regions->area->shapePointSet(CHOICE1)***/
			if(decoded_item->data_frames[i].regions[j].area_choice == VALID_REGION__AREA_SHAPE_POINT_SET)
			{
				/*(5-10-3-1)****dataFrames->regions->area->shapePointSet(CHOICE)->anchor**OP*****/
				if(decoded_item->data_frames[i].regions[j].shape_point_set->mask_anchor == 1)
				{
					printf("data_frames[%d].regions[%d].shape_point_set->anchor.lat = %li\n", i, j, decoded_item->data_frames[i].regions[j].shape_point_set->anchor.lat);
					printf("data_frames[%d].regions[%d].shape_point_set->anchor.longitude = %li\n", i, j, decoded_item->data_frames[i].regions[j].shape_point_set->anchor.longitude);
					if(decoded_item->data_frames[i].regions[j].shape_point_set->anchor.mask_elevation == 1)
					{
						printf("data_frames[%d].regions[%d].shape_point_set->anchor.elevation = %d\n", i, j, decoded_item->data_frames[i].regions[j].shape_point_set->anchor.elevation);
					}
				}
				/*(5-10-3-2)****dataFrames->regions->area->shapePointSet(CHOICE)->laneWidth**OP*****/
				if(decoded_item->data_frames[i].regions[j].shape_point_set->mask_lane_width == 1)
				{
					printf("data_frames[%d].regions[%d].shape_point_set->lane_width = %d\n", i, j, decoded_item->data_frames[i].regions[j].shape_point_set->lane_width);
				}
				/*(5-10-3-3)****dataFrames->regions->area->shapePointSet(CHOICE)->directionality**OP*****/
				if(decoded_item->data_frames[i].regions[j].shape_point_set->mask_directionality == 1)
				{
					printf("data_frames[%d].regions[%d].shape_point_set->directionality = %d\n", i, j, decoded_item->data_frames[i].regions[j].shape_point_set->directionality);
				}
				/*(5-10-3-4)****dataFrames->regions->area->shapePointSet(CHOICE)->nodeList***SEQUENCE OF**/
				printf("data_frames[%d].regions[%d].shape_point_set->node_list_num = %d\n", i, j, decoded_item->data_frames[i].regions[j].shape_point_set->node_list_num);
				for(k = 0; k < decoded_item->data_frames[i].regions[j].shape_point_set->node_list_num; k++)
				{
					printf("data_frames[%d].regions[%d].shape_point_set->node_list[%d].x_offset = %d\n", i, j, k, decoded_item->data_frames[i].regions[j].shape_point_set->node_list[k].x_offset);
					printf("data_frames[%d].regions[%d].shape_point_set->node_list[%d].y_offset = %d\n", i, j, k, decoded_item->data_frames[i].regions[j].shape_point_set->node_list[k].y_offset);
					if(decoded_item->data_frames[i].regions[j].shape_point_set->node_list[k].mask_z_offset == 1)
						printf("data_frames[%d].regions[%d].shape_point_set->node_list[%d].z_offset = %d\n", i, j, k, decoded_item->data_frames[i].regions[j].shape_point_set->node_list[k].z_offset);
					if(decoded_item->data_frames[i].regions[j].shape_point_set->node_list[k].mask_width == 1)
						printf("data_frames[%d].regions[%d].shape_point_set->node_list[%d].width = %d\n", i, j, k, decoded_item->data_frames[i].regions[j].shape_point_set->node_list[k].width);
				}/*end for loop:SEQUENCE OF shapePointSet->nodeList*/
			}
			/*(5-10-3)***dataFrames->regions->area->circle(CHOICE2)***/
			else if(decoded_item->data_frames[i].regions[j].area_choice == VALID_REGION__AREA_CIRCLE)
			{
				/*(5-10-3-1)***dataFrames->regions->area->circle(CHOICE)->center****/
				printf("data_frames[%d].regions[%d].circle->center.lat = %li\n", i, j, decoded_item->data_frames[i].regions[j].circle->center.lat);
				printf("data_frames[%d].regions[%d].circle->center.longitude = %li\n", i, j, decoded_item->data_frames[i].regions[j].circle->center.longitude);
				if(decoded_item->data_frames[i].regions[j].circle->center.mask_elevation == 1)
				{
					printf("data_frames[%d].regions[%d].circle->center.elevation = %d\n", i, j, decoded_item->data_frames[i].regions[j].circle->center.elevation);
				}
				/*(5-10-3-2)***dataFrames->regions->area->circle(CHOICE)->raduis(CHOICE)****/
				if(decoded_item->data_frames[i].regions[j].circle->raduis_choice == CIRCLE__RADUIS_RADIUSSTEPS)
				{
					printf("data_frames[%d].regions[%d].circle->radius_steps = %d\n", i, j, decoded_item->data_frames[i].regions[j].circle->radius_steps);
				}
				else if(decoded_item->data_frames[i].regions[j].circle->raduis_choice == CIRCLE__RADUIS_MILES)
				{
					printf("data_frames[%d].regions[%d].circle->miles = %d\n", i, j, decoded_item->data_frames[i].regions[j].circle->miles);
				}
				else if(decoded_item->data_frames[i].regions[j].circle->raduis_choice == CIRCLE__RADUIS_KM)
				{
					printf("data_frames[%d].regions[%d].circle->km = %d\n", i, j, decoded_item->data_frames[i].regions[j].circle->km);
				}else{
					printf("data_frames[%d].regions[%d].circle->raduis_choice = CIRCLE__RADUIS_NOTHING\n", i, j);
				}
			}
			/*(5-10-3)***dataFrames->regions->area->regionPointSet(CHOICE3)***/
			else if(decoded_item->data_frames[i].regions[j].area_choice == VALID_REGION__AREA_REGION_POINT_SET)
			{
				/*(5-10-3-1)*****dataFrames->regions->area->regionPointSet->anchor***OP*****/
				printf("data_frames[%d].regions[%d].region_point_set->anchor.lat = %li\n", i, j, decoded_item->data_frames[i].regions[j].region_point_set->anchor.lat);
				printf("data_frames[%d].regions[%d].region_point_set->anchor.longitude = %li\n", i, j, decoded_item->data_frames[i].regions[j].region_point_set->anchor.longitude);
				if(decoded_item->data_frames[i].regions[j].region_point_set->anchor.mask_elevation == 1)
				{
					printf("data_frames[%d].regions[%d].region_point_set->anchor.elevation = %d\n", i, j, decoded_item->data_frames[i].regions[j].region_point_set->anchor.elevation);
				}
				/*(5-10-3-2)*****dataFrames->regions->area->regionPointSet->nodeList*****/
				printf("data_frames[%d].regions[%d].region_point_set->node_list_num = %d\n", i, j, decoded_item->data_frames[i].regions[j].region_point_set->node_list_num);
				for(k = 0; k < decoded_item->data_frames[i].regions[j].region_point_set->node_list_num; k++)
				{
					printf("data_frames[%d].regions[%d].region_point_set->node_list[%d].x_offset = %d\n", i, j, k, decoded_item->data_frames[i].regions[j].region_point_set->node_list[k].x_offset);
					printf("data_frames[%d].regions[%d].region_point_set->node_list[%d].y_offset = %d\n", i, j, k, decoded_item->data_frames[i].regions[j].region_point_set->node_list[k].y_offset);
					if(decoded_item->data_frames[i].regions[j].region_point_set->node_list[k].mask_z_offset == 1)
					{
						printf("data_frames[%d].regions[%d].region_point_set->node_list[%d].z_offset = %d\n", i, j, k, decoded_item->data_frames[i].regions[j].region_point_set->node_list[k].z_offset);
					}
				}
			}else{
				printf("data_frames[%d].regions[%d].area_choice = ValidRegion__area_PR_NOTHING\n", i, j);
			}
		}
		/*(5-11)***dataFrames->content***CHOICE****/			
		if(decoded_item->data_frames[i].content_choice == CONTENT_ADVISORY)
		{
			printf("data_frames[%d].advisory_num = %d\n", i, decoded_item->data_frames[i].advisory_num);
			for(j = 0; j < decoded_item->data_frames[i].advisory_num; j++)
			{
				if(decoded_item->data_frames[i].advisory[j].itis_item_choice == ITEM_ITIS)
				{
					printf("data_frames[%d].advisory[%d].itis = %d\n", i, j, decoded_item->data_frames[i].advisory[j].itis);
				}
				else if(decoded_item->data_frames[i].advisory[j].itis_item_choice == ITEM_TEXT)
				{
					printf("data_frames[%d].advisory[%d].text = %s\n", i, j, decoded_item->data_frames[i].advisory[j].text);
				}else{
					printf("data_frames[%d].advisory[%d].itis_item_choice = ITEM_NOTHING\n", i, j);
				}
			}			
		}
		else if(decoded_item->data_frames[i].content_choice == CONTENT_WORKZONE)
		{
			printf("data_frames[%d].work_zone_num = %d\n", i, decoded_item->data_frames[i].work_zone_num);
			for(j = 0; j < decoded_item->data_frames[i].work_zone_num; j++)
			{
				if(decoded_item->data_frames[i].work_zone[j].item_choice == ITEM_ITIS)
				{
					printf("data_frames[%d].work_zone[%d].itis = %d\n", i, j, decoded_item->data_frames[i].work_zone[j].itis);
				}
				else if(decoded_item->data_frames[i].work_zone[j].item_choice == ITEM_TEXT)
				{
					printf("data_frames[%d].work_zone[%d].text = %s\n", i, j, decoded_item->data_frames[i].work_zone[j].text);
				}else{
					printf("data_frames[%d].work_zone[%d].item_choice = ITEM_NOTHING\n", i, j);
				}
			}			
		}
		else if(decoded_item->data_frames[i].content_choice == CONTENT_GENERICSIGN)
		{
			printf("data_frames[%d].generic_sign_num = %d\n", i, decoded_item->data_frames[i].generic_sign_num);
			for(j = 0; j < decoded_item->data_frames[i].generic_sign_num; j++)
			{
				if(decoded_item->data_frames[i].generic_sign[j].item_choice == ITEM_ITIS)
				{
					printf("data_frames[%d].generic_sign[%d].itis = %d\n", i, j, decoded_item->data_frames[i].generic_sign[j].itis);
				}
				else if(decoded_item->data_frames[i].generic_sign[j].item_choice == ITEM_TEXT)
				{
					printf("data_frames[%d].generic_sign[%d].text = %s\n", i, j, decoded_item->data_frames[i].generic_sign[j].text);
				}else{
					printf("data_frames[%d].generic_sign[%d].item_choice = ITEM_NOTHING\n", i, j);
				}
			}
		}
		else if(decoded_item->data_frames[i].content_choice == CONTENT_SPEEDLIMIT)
		{
			printf("data_frames[%d].speed_limit_num = %d\n", i, decoded_item->data_frames[i].speed_limit_num);
			for(j = 0; j < decoded_item->data_frames[i].speed_limit_num; j++)
			{
				if(decoded_item->data_frames[i].speed_limit[j].item_choice == ITEM_ITIS)
				{
					printf("data_frames[%d].speed_limit[%d].itis = %d\n", i, j, decoded_item->data_frames[i].speed_limit[j].itis);
				}
				else if(decoded_item->data_frames[i].speed_limit[j].item_choice == ITEM_TEXT)
				{
					printf("data_frames[%d].speed_limit[%d].text = %s\n", i, j, decoded_item->data_frames[i].speed_limit[j].text);
				}else{
					printf("data_frames[%d].speed_limit[%d].item_choice = ITEM_NOTHING\n", i, j);
				}
			}				
		}
		else if(decoded_item->data_frames[i].content_choice == CONTENT_EXITSERVICE)
		{
			printf("data_frames[%d].exit_service_num = %d\n", i, decoded_item->data_frames[i].exit_service_num);
			for(j = 0; j < decoded_item->data_frames[i].exit_service_num; j++)
			{
				if(decoded_item->data_frames[i].exit_service[j].item_choice == ITEM_ITIS)
				{
					printf("data_frames[%d].exit_service[%d].itis = %d\n", i, j, decoded_item->data_frames[i].exit_service[j].itis);
				}
				else if(decoded_item->data_frames[i].exit_service[j].item_choice == ITEM_TEXT)
				{
					printf("data_frames[%d].exit_service[%d].text = %s\n", i, j, decoded_item->data_frames[i].exit_service[j].text);
				}else{
					printf("data_frames[%d].exit_service[%d].item_choice = ITEM_NOTHING\n", i, j);
				}
			}
		}else{
			printf("data_frames[%d].content_choice = CONTENT_NOTHING\n", i);
		}
		/*(5-12)***dataFrames->url**OP***/
		if(decoded_item->data_frames[i].mask_url == 1)
		{
			printf("data_frames[%d].url = %s\n", i, decoded_item->data_frames[i].url);
		}		
	}/*end for loop:SEQUENCE OF dataFrames*/

	/*(6) crc*/
	printf("crs = %x %x\n", decoded_item->crc[0], decoded_item->crc[1]);

}

int cmd_rxTim(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	tim_standard_item *decoded_item = NULL;
	int err_tim;

	int err;
	TX_TIMER stop_tim_timer;
	wme_handle_t handle;
	unsigned char channel_number;
	unsigned char radio_number;
	unsigned int psid;
	struct event_message event;
	struct user_info user_info;
	struct wsm_info wsm_info;
	struct in_wsm in_wsm;

	if(argc != 3) { /* In threadx, cmd is not included in argc */
		printf("rxTim <psid> <radio_number> <channel_number> \n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("-- example --\n");
		printf("rxTim 123 0 172\n");
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
	stop_tim = 0;
	tx_timer_create(&stop_tim_timer, NULL, stop_tim_func, 0, 5000, 1, TX_AUTO_ACTIVATE);
	while(stop_tim == 0) {
		err = wme_wsm_recv(&handle, &in_wsm, 1000);
		if(err != 0) {
			printf("wsm_recv = %d\n", err);
			continue;
		}
		printf("wsm_recv success\n");
		printf("wsm data len = %d\n", in_wsm.length);

		decoded_item = (tim_standard_item *)j2735_calloc(1, sizeof(tim_standard_item));
		err_tim = j2735r41_tim_decode(decoded_item, in_wsm.data, MAX_TIM_BUFF_SIZE, 1);
		if(err_tim < 0)
		{
			printf("decode error...\n");
			return -1;
		}

		if(SHOW_DECODED == 1){_show_tim_standard_item(decoded_item);}
    
		j2735r41_tim_free_struct(decoded_item);
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

	tx_timer_delete(&stop_tim_timer);

	wme_exit(&handle);

	return 0;
}

int cmd_j2735r41_tim(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */  	
	(void)command;  	
	(void)argv;  	
	(void)argc;
	(void)cli;

	tim_standard_item *encode_item = NULL;
	tim_standard_item *decoded_item = NULL;
	int err_tim, loopCnt, i, j, k;
	size_t size;
	unsigned char *encodeBuf = NULL;

	printf("%s", "Start J2735r41_TIM test!\n");

	loopCnt = 0;
	while(loopCnt < 1)
	{
		encodeBuf = (unsigned char *)j2735_calloc(1, MAX_TIM_BUFF_SIZE);
		encode_item = (tim_standard_item *)j2735_calloc(1, sizeof(tim_standard_item));
		
		/*(1) msgID */
		encode_item->msg_id = 16;
		/*(2) packetID *OP**/
		encode_item->mask_packet_id = 1;
		for(i = 0; i < 8; i++)
			encode_item->packet_id[i] = 0x31 + i;
		encode_item->packet_id[8] = '\0';

		/*(3) urlB *OP**/
		encode_item->mask_url_b = 1;
		//memset(encode_item->url_b, 0x66, 21);
		//encode_item->url_b[21] = '\0';		
		memcpy(encode_item->url_b, "ABCD", strlen("ABCD"));
		encode_item->url_b[strlen("ABCD")+1] = '\0';
		/*(4) dataFrameCount *OP*/
		encode_item->mask_data_frame_count = 1;	
		encode_item->data_frame_count = 32;	
		/*(5) dataFrames*/
		encode_item->data_frames_num = 1;
		encode_item->data_frames = (data_frames_item *)j2735_calloc(encode_item->data_frames_num, sizeof(data_frames_item));
		for(i = 0; i < encode_item->data_frames_num; i++)
		{
			/*(5-1)***dataFrames->frameType****/
			encode_item->data_frames[i].frame_type = 3;
			/*(5-2)***dataFrames->msgId(CHOICE_ROADSIGNID****/
			encode_item->data_frames[i].msg_id_choice = MSG_ID_ROAD_SIGN_ID;		
			/*(5-2-1)***dataFrames->msgId(roadSignID)->position******/
			encode_item->data_frames[i].road_sign_id.position.lat = 900000001;
			encode_item->data_frames[i].road_sign_id.position.longitude = 1800000001;
			encode_item->data_frames[i].road_sign_id.position.mask_elevation = 1;		
			encode_item->data_frames[i].road_sign_id.position.elevation = 61439;		
			/*(5-2-2)***dataFrames->msgId(roadSignID)->viewAngle******/
			encode_item->data_frames[i].road_sign_id.view_angle[0] = 0xAA;
			encode_item->data_frames[i].road_sign_id.view_angle[1] = 0xBB;
			/*(5-2-3)***dataFrames->msgId(roadSignID)->mutcdCode******/
			encode_item->data_frames[i].road_sign_id.mask_mutcd_code = 1;		
			encode_item->data_frames[i].road_sign_id.mutcd_code = 6;						
			/*(5-2-4)***dataFrames->msgId(roadSignID)->crc***OP***/
			encode_item->data_frames[i].road_sign_id.mask_crc = 1;		
			encode_item->data_frames[i].road_sign_id.crc[0] = 0x41;
			encode_item->data_frames[i].road_sign_id.crc[1] = 0x42;		
			/*(5-3)***dataFrames->startYear******OP***/
			encode_item->data_frames[i].mask_start_year = 1;		
			encode_item->data_frames[i].start_year = 9999;		
			/*(5-4)***dataFrames->startTime****/
			encode_item->data_frames[i].start_time = 525960;
			/*(5-5)***dataFrames->duratonTime****/
			encode_item->data_frames[i].duraton_time = 32000;
			/*(5-6)***dataFrames->priority****/
			encode_item->data_frames[i].priority = 7;
			/*(5-7)***dataFrames->commonAnchor****OP***/
			encode_item->data_frames[i].mask_common_anchor = 1;
			encode_item->data_frames[i].common_anchor = (position3d_item *)j2735_calloc(1, sizeof(position3d_item));
			encode_item->data_frames[i].common_anchor->lat = 900000000;
			encode_item->data_frames[i].common_anchor->longitude = 1800000000;
			encode_item->data_frames[i].common_anchor->mask_elevation = 1;
			encode_item->data_frames[i].common_anchor->elevation = 2864;	
			/*(5-8)***dataFrames->commonLaneWidth****OP***/
			encode_item->data_frames[i].mask_common_lane_width = 1;		
			encode_item->data_frames[i].common_lane_width = 32767;		
			/*(5-9)***dataFrames->commonDirectionality**OP***/
			encode_item->data_frames[i].mask_common_directionality = 1;		
			encode_item->data_frames[i].common_directionality = 2;
			/*(5-10)***dataFrames->regions(SEQUENCE OF)****/
			encode_item->data_frames[i].regions_num = 1;
			encode_item->data_frames[i].regions = (valid_region_item *)j2735_calloc(encode_item->data_frames[i].regions_num, sizeof(valid_region_item));
			for(j = 0; j < encode_item->data_frames[i].regions_num; j++)
			{
				/*(5-10-1)***dataFrames->regions->direction****/
				encode_item->data_frames[i].regions[j].direction[0] = 0x11;
				encode_item->data_frames[i].regions[j].direction[1] = 0x12;
				/*(5-10-2)***dataFrames->regions->extent**OP***/
				encode_item->data_frames[i].regions[j].mask_extent = 1;			
				encode_item->data_frames[i].regions[j].extent = 10;			
				/*(5-10-3)***dataFrames->regions->area->shapePointSet(CHOICE1)***/
				encode_item->data_frames[i].regions[j].area_choice = VALID_REGION__AREA_SHAPE_POINT_SET;			
				/*(5-10-3-1)****dataFrames->regions->area->shapePointSet(CHOICE)->anchor**OP*****/
				encode_item->data_frames[i].regions[j].shape_point_set = (shape_point_set_item *)j2735_calloc(1, sizeof(shape_point_set_item));
				encode_item->data_frames[i].regions[j].shape_point_set->mask_anchor = 1;			
				encode_item->data_frames[i].regions[j].shape_point_set->anchor.lat = 900000001;
				encode_item->data_frames[i].regions[j].shape_point_set->anchor.longitude = 1800000001;
				encode_item->data_frames[i].regions[j].shape_point_set->anchor.mask_elevation = 1;				
				encode_item->data_frames[i].regions[j].shape_point_set->anchor.elevation = 2848;			
				/*(5-10-3-2)****dataFrames->regions->area->shapePointSet(CHOICE)->laneWidth**OP*****/
				encode_item->data_frames[i].regions[j].shape_point_set->mask_lane_width = 1;
				encode_item->data_frames[i].regions[j].shape_point_set->lane_width = 32767;			
				/*(5-10-3-3)****dataFrames->regions->area->shapePointSet(CHOICE)->directionality**OP*****/
				encode_item->data_frames[i].regions[j].shape_point_set->mask_directionality = 1;			
				encode_item->data_frames[i].regions[j].shape_point_set->directionality = 2;			
				/*(5-10-3-4)****dataFrames->regions->area->shapePointSet(CHOICE)->nodeList***SEQUENCE OF**/
				encode_item->data_frames[i].regions[j].shape_point_set->node_list_num = 2;
				encode_item->data_frames[i].regions[j].shape_point_set->node_list = (offsets_item *)j2735_calloc(encode_item->data_frames[i].regions[j].shape_point_set->node_list_num, sizeof(offsets_item));
				for(k = 0; k < encode_item->data_frames[i].regions[j].shape_point_set->node_list_num; k++)
				{
					encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].x_offset = 32767;
					encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].y_offset = 32766;
					encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].mask_z_offset = 1;
					encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].z_offset = 32765;
					encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].mask_width = 1;
					encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].width = 32764;
				}			
			}
			/*(5-11)***dataFrames->content***CHOICE****/
			encode_item->data_frames[i].content_choice = CONTENT_ADVISORY;		
			encode_item->data_frames[i].advisory_num = 3;
			encode_item->data_frames[i].advisory = (iti_scodes_text__member_item *)j2735_calloc(encode_item->data_frames[i].advisory_num, sizeof(iti_scodes_text__member_item));
			for(j = 0; j < encode_item->data_frames[i].advisory_num; j++)
			{
				encode_item->data_frames[i].advisory[j].itis_item_choice = ITEM_ITIS;			
				encode_item->data_frames[i].advisory[j].itis = 65565;			
			}		
			/*(5-12)***dataFrames->url**OP***/
			encode_item->data_frames[i].mask_url = 1;
			memcpy(encode_item->data_frames[i].url, "kyho", strlen("kyho"));
			encode_item->data_frames[i].url[strlen("kyho")+1] = '\0';
		}
		/*(6) crc*/
		encode_item->crc[0] = 0x55;
		encode_item->crc[1] = 0x66;	
		
		/**************************************************************/

		if((loopCnt % 100) == 0)
			printf("TIM loopCnt = %d\n", loopCnt+1);

		//if(SHOW_ENCODE == 1){_show_tim_standard_item(encode_item);}

		err_tim = j2735r41_tim_encode(encode_item, encodeBuf, MAX_TIM_BUFF_SIZE, &size, 1);
		if(err_tim < 0)
		{		
			printf("tim encode fail...\n");		
			return -1;	
		}
		_cmd_tim_free_struct(encode_item);

		decoded_item = (tim_standard_item *)j2735_calloc(1, sizeof(tim_standard_item));
		err_tim = j2735r41_tim_decode(decoded_item, encodeBuf, MAX_TIM_BUFF_SIZE, 1);
		if(err_tim < 0)
		{
			printf("decode error...\n");
			return -1;
		}

		if(SHOW_DECODED == 1){_show_tim_standard_item(decoded_item);}
		
		j2735r41_tim_free_struct(decoded_item);
		j2735_free(encodeBuf);

		loopCnt++;	
		(void) tx_thread_sleep(10);
	}

	printf("%s", "J2735r41_TIM Test command executed successfully.\n");
		
	return CLI_OK;
}

int _encode_txTim(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	int i, j, k;
	tim_standard_item *encode_item = NULL;

	encode_item = (tim_standard_item *)j2735_calloc(1, sizeof(tim_standard_item));
	/*(1) msgID */
	encode_item->msg_id = 16;
	/*(2) packetID *OP**/
	encode_item->mask_packet_id = 1;
	for(i = 0; i < 8; i++)
		encode_item->packet_id[i] = 0x31 + i;
	encode_item->packet_id[8] = '\0';

	/*(3) urlB *OP**/
	encode_item->mask_url_b = 1;	
	memcpy(encode_item->url_b, "ABCD", strlen("ABCD"));
	encode_item->url_b[strlen("ABCD")+1] = '\0';
	/*(4) dataFrameCount *OP*/
	encode_item->mask_data_frame_count = 1;	
	encode_item->data_frame_count = 32;	
	/*(5) dataFrames*/
	encode_item->data_frames_num = 1;
	encode_item->data_frames = (data_frames_item *)j2735_calloc(encode_item->data_frames_num, sizeof(data_frames_item));
	for(i = 0; i < encode_item->data_frames_num; i++)
	{
		/*(5-1)***dataFrames->frameType****/
		encode_item->data_frames[i].frame_type = 3;
		/*(5-2)***dataFrames->msgId(CHOICE_ROADSIGNID****/
		encode_item->data_frames[i].msg_id_choice = MSG_ID_ROAD_SIGN_ID;		
		/*(5-2-1)***dataFrames->msgId(roadSignID)->position******/
		encode_item->data_frames[i].road_sign_id.position.lat = 900000001;
		encode_item->data_frames[i].road_sign_id.position.longitude = 1800000001;
		encode_item->data_frames[i].road_sign_id.position.mask_elevation = 1;		
		encode_item->data_frames[i].road_sign_id.position.elevation = 61439;		
		/*(5-2-2)***dataFrames->msgId(roadSignID)->viewAngle******/
		encode_item->data_frames[i].road_sign_id.view_angle[0] = 0xAA;
		encode_item->data_frames[i].road_sign_id.view_angle[1] = 0xBB;
		/*(5-2-3)***dataFrames->msgId(roadSignID)->mutcdCode******/
		encode_item->data_frames[i].road_sign_id.mask_mutcd_code = 1;		
		encode_item->data_frames[i].road_sign_id.mutcd_code = 6;						
		/*(5-2-4)***dataFrames->msgId(roadSignID)->crc***OP***/
		encode_item->data_frames[i].road_sign_id.mask_crc = 1;		
		encode_item->data_frames[i].road_sign_id.crc[0] = 0x41;
		encode_item->data_frames[i].road_sign_id.crc[1] = 0x42;		
		/*(5-3)***dataFrames->startYear******OP***/
		encode_item->data_frames[i].mask_start_year = 1;		
		encode_item->data_frames[i].start_year = 9999;		
		/*(5-4)***dataFrames->startTime****/
		encode_item->data_frames[i].start_time = 525960;
		/*(5-5)***dataFrames->duratonTime****/
		encode_item->data_frames[i].duraton_time = 32000;
		/*(5-6)***dataFrames->priority****/
		encode_item->data_frames[i].priority = 7;
		/*(5-7)***dataFrames->commonAnchor****OP***/
		encode_item->data_frames[i].mask_common_anchor = 1;
		encode_item->data_frames[i].common_anchor = (position3d_item *)j2735_calloc(1, sizeof(position3d_item));
		encode_item->data_frames[i].common_anchor->lat = 900000000;
		encode_item->data_frames[i].common_anchor->longitude = 1800000000;
		encode_item->data_frames[i].common_anchor->mask_elevation = 1;
		encode_item->data_frames[i].common_anchor->elevation = 2864;	
		/*(5-8)***dataFrames->commonLaneWidth****OP***/
		encode_item->data_frames[i].mask_common_lane_width = 1;		
		encode_item->data_frames[i].common_lane_width = 32767;		
		/*(5-9)***dataFrames->commonDirectionality**OP***/
		encode_item->data_frames[i].mask_common_directionality = 1;		
		encode_item->data_frames[i].common_directionality = 2;
		/*(5-10)***dataFrames->regions(SEQUENCE OF)****/
		encode_item->data_frames[i].regions_num = 1;
		encode_item->data_frames[i].regions = (valid_region_item *)j2735_calloc(encode_item->data_frames[i].regions_num, sizeof(valid_region_item));
		for(j = 0; j < encode_item->data_frames[i].regions_num; j++)
		{
			/*(5-10-1)***dataFrames->regions->direction****/
			encode_item->data_frames[i].regions[j].direction[0] = 0x11;
			encode_item->data_frames[i].regions[j].direction[1] = 0x12;
			/*(5-10-2)***dataFrames->regions->extent**OP***/
			encode_item->data_frames[i].regions[j].mask_extent = 1;			
			encode_item->data_frames[i].regions[j].extent = 10;			
			/*(5-10-3)***dataFrames->regions->area->shapePointSet(CHOICE1)***/
			encode_item->data_frames[i].regions[j].area_choice = VALID_REGION__AREA_SHAPE_POINT_SET;			
			/*(5-10-3-1)****dataFrames->regions->area->shapePointSet(CHOICE)->anchor**OP*****/
			encode_item->data_frames[i].regions[j].shape_point_set = (shape_point_set_item *)j2735_calloc(1, sizeof(shape_point_set_item));
			encode_item->data_frames[i].regions[j].shape_point_set->mask_anchor = 1;			
			encode_item->data_frames[i].regions[j].shape_point_set->anchor.lat = 900000001;
			encode_item->data_frames[i].regions[j].shape_point_set->anchor.longitude = 1800000001;
			encode_item->data_frames[i].regions[j].shape_point_set->anchor.mask_elevation = 1;				
			encode_item->data_frames[i].regions[j].shape_point_set->anchor.elevation = 2848;			
			/*(5-10-3-2)****dataFrames->regions->area->shapePointSet(CHOICE)->laneWidth**OP*****/
			encode_item->data_frames[i].regions[j].shape_point_set->mask_lane_width = 1;
			encode_item->data_frames[i].regions[j].shape_point_set->lane_width = 32767;			
			/*(5-10-3-3)****dataFrames->regions->area->shapePointSet(CHOICE)->directionality**OP*****/
			encode_item->data_frames[i].regions[j].shape_point_set->mask_directionality = 1;			
			encode_item->data_frames[i].regions[j].shape_point_set->directionality = 2;			
			/*(5-10-3-4)****dataFrames->regions->area->shapePointSet(CHOICE)->nodeList***SEQUENCE OF**/
			encode_item->data_frames[i].regions[j].shape_point_set->node_list_num = 2;
			encode_item->data_frames[i].regions[j].shape_point_set->node_list = (offsets_item *)j2735_calloc(encode_item->data_frames[i].regions[j].shape_point_set->node_list_num, sizeof(offsets_item));
			for(k = 0; k < encode_item->data_frames[i].regions[j].shape_point_set->node_list_num; k++)
			{
				encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].x_offset = 32767;
				encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].y_offset = 32766;
				encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].mask_z_offset = 1;
				encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].z_offset = 32765;
				encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].mask_width = 1;
				encode_item->data_frames[i].regions[j].shape_point_set->node_list[k].width = 32764;
			}			
		}
		/*(5-11)***dataFrames->content***CHOICE****/
		encode_item->data_frames[i].content_choice = CONTENT_ADVISORY;		
		encode_item->data_frames[i].advisory_num = 3;
		encode_item->data_frames[i].advisory = (iti_scodes_text__member_item *)j2735_calloc(encode_item->data_frames[i].advisory_num, sizeof(iti_scodes_text__member_item));
		for(j = 0; j < encode_item->data_frames[i].advisory_num; j++)
		{
			encode_item->data_frames[i].advisory[j].itis_item_choice = ITEM_ITIS;			
			encode_item->data_frames[i].advisory[j].itis = 65565;			
		}		
		/*(5-12)***dataFrames->url**OP***/
		encode_item->data_frames[i].mask_url = 1;
		memcpy(encode_item->data_frames[i].url, "kyho", strlen("kyho"));
		encode_item->data_frames[i].url[strlen("kyho")+1] = '\0';
	}
	/*(6) crc*/
	encode_item->crc[0] = 0x55;
	encode_item->crc[1] = 0x66;	

	if(SHOW_ENCODE == 1)
	{
		printf("\nTIM Encode Data:\n");
		_show_tim_standard_item(encode_item);
	}
	
	return j2735r41_tim_encode(encode_item, encodeBuf, input_buf_size, size, 1);
}

int cmd_txTim(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txTim <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txTim 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_TIM_BUFF_SIZE);
	_encode_txTim(encodeBuf, MAX_TIM_BUFF_SIZE, &len);
	
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

	printf("%s", "cmd_txTim command executed successfully.\n");
	return CLI_OK;
}

