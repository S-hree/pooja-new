#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <linuxapi.h>
#include <tx_api.h>
#include <craton/debug.h>
#include "error_code.h"

#include <j2735r41_spat_api.h>
#include "j2735r41_common.h"

#include <libcli.h>
#include <craton/cli.h>
#include <atlk/nav_service.h>

#include <dot3_common.h>
#include <dot3_api.h>
#include <j2735_mem.h>

#define SHOW_ENCODE 1
#define SHOW_DECODED 1

static int stop_spat;

void stop_spat_func(ULONG input)
{
	(void)input;
	stop_spat = 1;
	return;
}

void _cmd_spat_free_struct(spat_standard_item *free_spat_item)
{
	int i, j, k;
	
	if(free_spat_item == NULL){
		printf("[SPAT] Error: _cmd_spat_free_struct() input is NULL\n");
	}else{
		if(free_spat_item->intersections != NULL){
			for(i = 0; i < free_spat_item->intersections_num; i++){
				if(free_spat_item->intersections[i].enabled_lanes != NULL){
					j2735_free(free_spat_item->intersections[i].enabled_lanes);
				}

				if(free_spat_item->intersections[i].states != NULL){
					for(j = 0; j < free_spat_item->intersections[i].states_num; j++){
						if(free_spat_item->intersections[i].states[j].state_time_speed != NULL){
							for(k = 0; k < free_spat_item->intersections[i].states[j].state_time_speed_num; k++){
								if(free_spat_item->intersections[i].states[j].state_time_speed[k].speeds != NULL){
									j2735_free(free_spat_item->intersections[i].states[j].state_time_speed[k].speeds);
								}
							}
							
							j2735_free(free_spat_item->intersections[i].states[j].state_time_speed);
						}					

						if(free_spat_item->intersections[i].states[j].maneuver_assist_list != NULL){
							j2735_free(free_spat_item->intersections[i].states[j].maneuver_assist_list);
						}						
					}
					
					j2735_free(free_spat_item->intersections[i].states);
				}				

				if(free_spat_item->intersections[i].maneuver_assist_list != NULL){					
					j2735_free(free_spat_item->intersections[i].maneuver_assist_list);
				}				
			}			

			j2735_free(free_spat_item->intersections);
		}	

		j2735_free(free_spat_item);
	}
}

void _show_spat_standard_item(spat_standard_item *decoded_item)
{
	int i, j, k, m;
	
	printf("==========SPAT Message==========\n");
	/*(1) msgID */
	printf("msg_id = %d\n", decoded_item->msg_id);
	/*(2) msgSubID **OP**/
	if(decoded_item->mask_msg_sub_id == 1)
	{
		printf("msg_sub_id = %d\n", decoded_item->msg_sub_id);
	}
	/*(3) name **OP**/
	if(decoded_item->mask_name == 1)
	{
		printf("name = %s\n", decoded_item->name);
	}
	/*(4) intersections */
	printf("intersections_num = %d\n", decoded_item->intersections_num);
	for(i = 0; i < decoded_item->intersections_num; i++)
	{
		/* (4-1) name **OP**/
		if(decoded_item->intersections[i].mask_name == 1)
		{
			printf("intersections[%d].name = %s\n", i, decoded_item->intersections[i].name);
		}
		/* (4-2) id */
		if(decoded_item->intersections[i].id.mask_region == 1)
		{
			printf("intersections[%d].id.region = %d\n", i, decoded_item->intersections[i].id.region);
		}
		printf("intersections[%d].id.id = %d\n", i, decoded_item->intersections[i].id.id);
		/* (4-3) revision */
		printf("intersections[%d].revision = %d\n", i, decoded_item->intersections[i].revision);
		/* (4-4) status */
		printf("intersections[%d].status = %d\n", i, decoded_item->intersections[i].status);
		/* (4-5) moy **OP**/
		if(decoded_item->intersections[i].mask_moy == 1)
		{
			printf("intersections[%d].moy = %lu\n", i, decoded_item->intersections[i].moy);
		}
		/* (4-6) timeStamp **OP**/
		if(decoded_item->intersections[i].mask_time_stamp == 1)
		{
			printf("intersections[%d].time_stamp = %d\n", i, decoded_item->intersections[i].time_stamp);
		}
		/* (4-7) enabledLanes **OP**/
		if(decoded_item->intersections[i].mask_enabled_lanes == 1)
		{
			printf("intersections[%d].enabled_lanes_num = %d\n", i, decoded_item->intersections[i].enabled_lanes_num);
			for(j = 0; j < decoded_item->intersections[i].enabled_lanes_num; j++)
			{
				printf("intersections[%d].enabled_lanes[%d] = %d\n", i, j, decoded_item->intersections[i].enabled_lanes[j]);
			}		
		}
		/* (4-8) states */
		printf("intersections[%d].states_num = %d\n", i, decoded_item->intersections[i].states_num);
		for(j = 0; j < decoded_item->intersections[i].states_num; j++)
		{
			/* (4-8-1) movementName **OP**/
			if(decoded_item->intersections[i].states[j].mask_movement_name == 1)
			{
				printf("intersections[%d].states[%d].movement_name = %s\n", i, j, decoded_item->intersections[i].states[j].movement_name);
			}
			/* (4-8-2) signalGroup */
			printf("intersections[%d].states[%d].signal_group = %d\n", i, j, decoded_item->intersections[i].states[j].signal_group);
			/* (4-8-3) state_time_speed */
			printf("intersections[%d].states[%d].state_time_speed_num = %d\n", i, j, decoded_item->intersections[i].states[j].state_time_speed_num);
			for(k = 0; k < decoded_item->intersections[i].states[j].state_time_speed_num; k++)
			{
				/* (4-8-3-1) eventState */
				printf("intersections[%d].states[%d].state_time_speed[%d].event_state = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].event_state);
				/* (4-8-3-2) timing **OP**/
				if(decoded_item->intersections[i].states[j].state_time_speed[k].mask_timing == 1)
				{
					/* (4-8-3-2-1) startTime **OP**/
					if(decoded_item->intersections[i].states[j].state_time_speed[k].timing.mask_start_time == 1)
					{
						printf("intersections[%d].states[%d].state_time_speed[%d].timing.start_time = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].timing.start_time);
					}
					/* (4-8-3-2-2) minEndTime */
					printf("intersections[%d].states[%d].state_time_speed[%d].timing.min_end_time = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].timing.min_end_time);
					/* (4-8-3-2-3) maxEndTime **OP**/
					if(decoded_item->intersections[i].states[j].state_time_speed[k].timing.mask_max_end_time == 1)
					{
						printf("intersections[%d].states[%d].state_time_speed[%d].timing.max_end_time = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].timing.max_end_time);
					}
					/* (4-8-3-2-4) likelyTime **OP**/
					if(decoded_item->intersections[i].states[j].state_time_speed[k].timing.mask_likely_time == 1)
					{
						printf("intersections[%d].states[%d].state_time_speed[%d].timing.likely_time = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].timing.likely_time);
					}
					/* (4-8-3-2-5) confidence **OP**/
					if(decoded_item->intersections[i].states[j].state_time_speed[k].timing.mask_confidence == 1)
					{
						printf("intersections[%d].states[%d].state_time_speed[%d].timing.confidence = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].timing.confidence);
					}
					/* (4-8-3-2-6) nextTime **OP**/
					if(decoded_item->intersections[i].states[j].state_time_speed[k].timing.mask_next_time == 1)
					{
						printf("intersections[%d].states[%d].state_time_speed[%d].timing.next_time = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].timing.next_time);
					}
				}
				/* (4-8-3-3) speeds **OP**/
				if(decoded_item->intersections[i].states[j].state_time_speed[k].mask_speeds == 1)
				{
					printf("intersections[%d].states[%d].state_time_speed[%d].speeds_num = %d\n", i, j, k, decoded_item->intersections[i].states[j].state_time_speed[k].speeds_num);
					for(m = 0; m < decoded_item->intersections[i].states[j].state_time_speed[k].speeds_num; m++)
					{
						/* (4-8-3-3-1) type */
						printf("intersections[%d].states[%d].state_time_speed[%d].speeds[%d].type = %d\n", i, j, k, m, decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].type);
						/* (4-8-3-3-2) speed **OP**/
						if(decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_speed == 1)
						{
							printf("intersections[%d].states[%d].state_time_speed[%d].speeds[%d].speed = %d\n", i, j, k, m, decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].speed);
						}
						/* (4-8-3-3-3) confidence **OP**/
						if(decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_confidence == 1)
						{
							printf("intersections[%d].states[%d].state_time_speed[%d].speeds[%d].confidence = %d\n", i, j, k, m, decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].confidence);
						}
						/* (4-8-3-3-4) distance **OP**/
						if(decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_distance == 1)
						{
							printf("intersections[%d].states[%d].state_time_speed[%d].speeds[%d].distance = %d\n", i, j, k, m, decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].distance);
						}
						/* (4-8-3-3-5) Class **OP**/
						if(decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_as_class == 1)
						{
							printf("intersections[%d].states[%d].state_time_speed[%d].speeds[%d].as_class = %d\n", i, j, k, m, decoded_item->intersections[i].states[j].state_time_speed[k].speeds[m].as_class);
						}
					}
				}
			}
			/* (4-8-4) maneuverAssistList **OP**/
			if(decoded_item->intersections[i].states[j].mask_maneuver_assist_list == 1)
			{
				printf("intersections[%d].states[%d].maneuver_assist_list_num = %d\n", i, j, decoded_item->intersections[i].states[j].maneuver_assist_list_num);
				for(k = 0; k < decoded_item->intersections[i].states[j].maneuver_assist_list_num; k++)
				{
					/* (4-8-4-1) connectionID */
					printf("intersections[%d].states[%d].maneuver_assist_list[%d].connection_id = %d\n", i, j, k, decoded_item->intersections[i].states[j].maneuver_assist_list[k].connection_id);
					/* (4-8-4-2) queueLength **OP**/
					if(decoded_item->intersections[i].states[j].maneuver_assist_list[k].mask_queue_length == 1)
					{
						printf("intersections[%d].states[%d].maneuver_assist_list[%d].queue_length = %d\n", i, j, k, decoded_item->intersections[i].states[j].maneuver_assist_list[k].queue_length);
					}
					/* (4-8-4-3) availableStorageLength **OP**/
					if(decoded_item->intersections[i].states[j].maneuver_assist_list[k].mask_available_storage_length == 1)
					{
						printf("intersections[%d].states[%d].maneuver_assist_list[%d].available_storage_length = %d\n", i, j, k, decoded_item->intersections[i].states[j].maneuver_assist_list[k].available_storage_length);
					}						
					/* (4-8-4-4) waitOnStop **OP**/
					if(decoded_item->intersections[i].states[j].maneuver_assist_list[k].mask_wait_on_stop == 1)
					{
						printf("intersections[%d].states[%d].maneuver_assist_list[%d].wait_on_stop = %d\n", i, j, k, decoded_item->intersections[i].states[j].maneuver_assist_list[k].wait_on_stop);
					}
					/* (4-8-4-5) pedBicycleDetect **OP**/
					if(decoded_item->intersections[i].states[j].maneuver_assist_list[k].mask_ped_bicycle_detect == 1)
					{
						printf("intersections[%d].states[%d].maneuver_assist_list[%d].ped_bicycle_detect = %d\n", i, j, k, decoded_item->intersections[i].states[j].maneuver_assist_list[k].ped_bicycle_detect);
					}
				}
			}
		}
		/* (4-9) maneuverAssistList **OP**/
		if(decoded_item->intersections[i].mask_maneuver_assist_list == 1)
		{
			printf("intersections[%d].maneuver_assist_list_num = %d\n", i, decoded_item->intersections[i].maneuver_assist_list_num);
			for(j = 0; j < decoded_item->intersections[i].maneuver_assist_list_num; j++)
			{
				/* (4-9-1) connectionID */
				printf("intersections[%d].maneuver_assist_list[%d].connection_id = %d\n", i, j, decoded_item->intersections[i].maneuver_assist_list[j].connection_id);
				/* (4-9-2) queueLength **OP**/
				if(decoded_item->intersections[i].maneuver_assist_list[j].mask_queue_length == 1)
				{
					printf("intersections[%d].maneuver_assist_list[%d].queue_length = %d\n", i, j, decoded_item->intersections[i].maneuver_assist_list[j].queue_length);
				}
				/* (4-9-3) availableStorageLength **OP**/
				if(decoded_item->intersections[i].maneuver_assist_list[j].mask_available_storage_length == 1)
				{
					printf("intersections[%d].maneuver_assist_list[%d].available_storage_length = %d\n", i, j, decoded_item->intersections[i].maneuver_assist_list[j].available_storage_length);
				}						
				/* (4-9-4) waitOnStop **OP**/
				if(decoded_item->intersections[i].maneuver_assist_list[j].mask_wait_on_stop == 1)
				{
					printf("intersections[%d].maneuver_assist_list[%d].wait_on_stop = %d\n", i, j, decoded_item->intersections[i].maneuver_assist_list[j].wait_on_stop);
				}
				/* (4-9-5) pedBicycleDetect **OP**/
				if(decoded_item->intersections[i].maneuver_assist_list[j].mask_ped_bicycle_detect == 1)
				{
					printf("intersections[%d].maneuver_assist_list[%d].ped_bicycle_detect = %d\n", i, j, decoded_item->intersections[i].maneuver_assist_list[j].ped_bicycle_detect);
				}
			}
		}
		/* (4-10) priority **OP**/
		if(decoded_item->intersections[i].mask_priority == 1)
		{
			printf("intersections[%d].priority = %d\n", i, decoded_item->intersections[i].priority);
		}
		/* (4-11) preempt **OP**/
		if(decoded_item->intersections[i].mask_preempt == 1)
		{
			printf("intersections[%d].preempt = %d\n", i, decoded_item->intersections[i].preempt);
		}
	}
}

int cmd_rxSpat(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	spat_standard_item *decoded_item = NULL;
	int err_spat;

	int err;
	TX_TIMER stop_spat_timer;
	wme_handle_t handle;
	unsigned char channel_number;
	unsigned char radio_number;
	unsigned int psid;
	struct event_message event;
	struct user_info user_info;
	struct wsm_info wsm_info;
	struct in_wsm in_wsm;

	if(argc != 3) { /* In threadx, cmd is not included in argc */
		printf("rxSpat <psid> <radio_number> <channel_number> \n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("-- example --\n");
		printf("rxSpat 123 0 172\n");
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
	stop_spat = 0;
	tx_timer_create(&stop_spat_timer, NULL, stop_spat_func, 0, 5000, 1, TX_AUTO_ACTIVATE);
	while(stop_spat == 0) {
		err = wme_wsm_recv(&handle, &in_wsm, 1000);
		if(err != 0) {
			printf("wsm_recv = %d\n", err);
			continue;
		}
		printf("wsm_recv success\n");
		printf("wsm data len = %d\n", in_wsm.length);

		decoded_item = (spat_standard_item *)j2735_calloc(1, sizeof(spat_standard_item));
		err_spat = j2735r41_spat_decode(decoded_item, in_wsm.data, MAX_SPAT_BUFF_SIZE, 1);
		if(err_spat < 0)
		{
			printf("decode error...\n");
			return -1;
		}

		if(SHOW_DECODED == 1){_show_spat_standard_item(decoded_item);}

		j2735r41_spat_free_struct(decoded_item);
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

	tx_timer_delete(&stop_spat_timer);

	wme_exit(&handle);

	return 0;
}

int cmd_j2735r41_spat(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */  	
	(void)command;  	
	(void)argv;  	
	(void)argc;
	(void)cli;

	spat_standard_item *encode_item = NULL;
	spat_standard_item *decoded_item = NULL;	
	int err_spat, loopCnt;
	int i, j, k, m;
	size_t size;
	unsigned char *encodeBuf = NULL;
	
	printf("%s", "Start J2735r41_SPAT test!\n");	

	loopCnt = 0;
	while(loopCnt < 1)
	{
		encodeBuf = (unsigned char *)j2735_calloc(1, MAX_SPAT_BUFF_SIZE);
		encode_item = (spat_standard_item *)j2735_calloc(1, sizeof(spat_standard_item));		
		/*(1) msgID */
		encode_item->msg_id = 13;
		/*(2) msgSubID **OP**/
		encode_item->mask_msg_sub_id = 1;	
		encode_item->msg_sub_id = 255;	
		/*(3) name **OP**/
		encode_item->mask_name = 1;
		memcpy(encode_item->name, "SPAT_name", strlen("SPAT_name"));
		encode_item->name[strlen("SPAT_name")+1] = '\0';
		/*(4) intersections */
		encode_item->intersections_num = 2;
		encode_item->intersections = (intersection_state_item *)j2735_calloc(encode_item->intersections_num, sizeof(intersection_state_item));
		for(i = 0; i < encode_item->intersections_num; i++)
		{
			/* (4-1) name **OP**/
			encode_item->intersections[i].mask_name = 1;
			memcpy(&(encode_item->intersections[i].name), "intersections_name", strlen("intersections_name"));
			/* (4-2) id */
			encode_item->intersections[i].id.mask_region = 1;		
			encode_item->intersections[i].id.region = 65535;		
			encode_item->intersections[i].id.id = 65534;
			/* (4-3) revision */
			encode_item->intersections[i].revision = 127;
			/* (4-4) status */
			encode_item->intersections[i].status = 1024;
			/* (4-5) moy **OP**/
			encode_item->intersections[i].mask_moy = 1;		
			encode_item->intersections[i].moy = 527040;		
			/* (4-6) timeStamp **OP**/
			encode_item->intersections[i].mask_time_stamp = 1;		
			encode_item->intersections[i].time_stamp = 65533;		
			/* (4-7) enabledLanes **OP**/
			encode_item->intersections[i].mask_enabled_lanes = 1;		
			encode_item->intersections[i].enabled_lanes_num = 2;
			encode_item->intersections[i].enabled_lanes = (unsigned char *)j2735_calloc(encode_item->intersections[i].enabled_lanes_num, sizeof(unsigned char));
			for(j = 0; j < encode_item->intersections[i].enabled_lanes_num; j++)
			{
				encode_item->intersections[i].enabled_lanes[j] = 255;
			}		
			/* (4-8) states */
			encode_item->intersections[i].states_num = 1;
			encode_item->intersections[i].states = (movement_state_item *)j2735_calloc(encode_item->intersections[i].states_num, sizeof(movement_state_item));
			for(j = 0; j < encode_item->intersections[i].states_num; j++)
			{
				/* (4-8-1) movementName **OP**/
				encode_item->intersections[i].states[j].mask_movement_name = 1;
				memcpy(&(encode_item->intersections[i].states[j].movement_name), "movement_name", strlen("movement_name"));
				/* (4-8-2) signalGroup */
				encode_item->intersections[i].states[j].signal_group = 254;
				/* (4-8-3) state_time_speed */
				encode_item->intersections[i].states[j].state_time_speed_num = 1;
				encode_item->intersections[i].states[j].state_time_speed = (movement_event_item *)j2735_calloc(encode_item->intersections[i].states[j].state_time_speed_num, sizeof(movement_event_item));
				for(k = 0; k < encode_item->intersections[i].states[j].state_time_speed_num; k++)
				{
					/* (4-8-3-1) eventState */
					encode_item->intersections[i].states[j].state_time_speed[k].event_state = 9;
					/* (4-8-3-2) timing **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].mask_timing = 1;				
					/* (4-8-3-2-1) startTime **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_start_time = 1;				
					encode_item->intersections[i].states[j].state_time_speed[k].timing.start_time = 36002;				
					/* (4-8-3-2-2) minEndTime */
					encode_item->intersections[i].states[j].state_time_speed[k].timing.min_end_time = 36001;
					/* (4-8-3-2-3) maxEndTime **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_max_end_time = 1;				
					encode_item->intersections[i].states[j].state_time_speed[k].timing.max_end_time = 36000;				
					/* (4-8-3-2-4) likelyTime **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_likely_time = 1;				
					encode_item->intersections[i].states[j].state_time_speed[k].timing.likely_time = 35999;				
					/* (4-8-3-2-5) confidence **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_confidence = 1;				
					encode_item->intersections[i].states[j].state_time_speed[k].timing.confidence = 15;				
					/* (4-8-3-2-6) nextTime **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_next_time = 1;				
					encode_item->intersections[i].states[j].state_time_speed[k].timing.next_time = 35998;				
					/* (4-8-3-3) speeds **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].mask_speeds = 1;				
					encode_item->intersections[i].states[j].state_time_speed[k].speeds_num = 1;
					encode_item->intersections[i].states[j].state_time_speed[k].speeds = (advisory_speed_item *)j2735_calloc(encode_item->intersections[i].states[j].state_time_speed[k].speeds_num, sizeof(advisory_speed_item));	
					for(m = 0; m < encode_item->intersections[i].states[j].state_time_speed[k].speeds_num; m++)
					{
						/* (4-8-3-3-1) type */
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].type = 3;
						/* (4-8-3-3-2) speed **OP**/
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_speed = 1;					
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].speed = 500;					
						/* (4-8-3-3-3) confidence **OP**/
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_confidence = 1;					
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].confidence = 7;					
						/* (4-8-3-3-4) distance **OP**/
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_distance = 1;					
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].distance = 10000;					
						/* (4-8-3-3-5) Class **OP**/
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_as_class = 1;					
						encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].as_class = 255;
					}				
				}
				/* (4-8-4) maneuverAssistList **OP**/
				encode_item->intersections[i].states[j].mask_maneuver_assist_list = 1;			
				encode_item->intersections[i].states[j].maneuver_assist_list_num = 1;
				encode_item->intersections[i].states[j].maneuver_assist_list = (connection_maneuver_assist_item *)j2735_calloc(encode_item->intersections[i].states[j].maneuver_assist_list_num, sizeof(connection_maneuver_assist_item));	
				for(k = 0; k < encode_item->intersections[i].states[j].maneuver_assist_list_num; k++)
				{
					/* (4-8-4-1) connectionID */
					encode_item->intersections[i].states[j].maneuver_assist_list[k].connection_id = 255;
					/* (4-8-4-2) queueLength **OP**/
					encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_queue_length = 1;				
					encode_item->intersections[i].states[j].maneuver_assist_list[k].queue_length = 9999;				
					/* (4-8-4-3) availableStorageLength **OP**/
					encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_available_storage_length = 1;				
					encode_item->intersections[i].states[j].maneuver_assist_list[k].available_storage_length = 9998;										
					/* (4-8-4-4) waitOnStop **OP**/
					encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_wait_on_stop = 1;				
					encode_item->intersections[i].states[j].maneuver_assist_list[k].wait_on_stop = 0;
					/* (4-8-4-5) pedBicycleDetect **OP**/
					encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_ped_bicycle_detect = 1;				
					encode_item->intersections[i].states[j].maneuver_assist_list[k].ped_bicycle_detect = 0;				
				}			
			}
			/* (4-9) maneuverAssistList **OP**/
			encode_item->intersections[i].mask_maneuver_assist_list = 1;		
			encode_item->intersections[i].maneuver_assist_list_num = 1;
			encode_item->intersections[i].maneuver_assist_list = (connection_maneuver_assist_item *)j2735_calloc(encode_item->intersections[i].maneuver_assist_list_num, sizeof(connection_maneuver_assist_item));	
			for(j = 0; j < encode_item->intersections[i].maneuver_assist_list_num; j++)
			{
				/* (4-9-1) connectionID */
				encode_item->intersections[i].maneuver_assist_list[j].connection_id = 255;
				/* (4-9-2) queueLength **OP**/
				encode_item->intersections[i].maneuver_assist_list[j].mask_queue_length = 1;			
				encode_item->intersections[i].maneuver_assist_list[j].queue_length = 10000;			
				/* (4-9-3) availableStorageLength **OP**/
				encode_item->intersections[i].maneuver_assist_list[j].mask_available_storage_length = 1;			
				encode_item->intersections[i].maneuver_assist_list[j].available_storage_length = 9999;									
				/* (4-9-4) waitOnStop **OP**/
				encode_item->intersections[i].maneuver_assist_list[j].mask_wait_on_stop = 1;			
				encode_item->intersections[i].maneuver_assist_list[j].wait_on_stop = 0;			
				/* (4-9-5) pedBicycleDetect **OP**/
				encode_item->intersections[i].maneuver_assist_list[j].mask_ped_bicycle_detect = 1;			
				encode_item->intersections[i].maneuver_assist_list[j].ped_bicycle_detect = 0;
			}		
			/* (4-10) priority **OP**/
			encode_item->intersections[i].mask_priority = 1;
			encode_item->intersections[i].priority = 0xEE;		
			/* (4-11) preempt **OP**/
			encode_item->intersections[i].mask_preempt = 1;
			encode_item->intersections[i].preempt = 0xDD;			
		}
		/***********************************************************************/

		if((loopCnt % 100) == 0)
			printf("SPAT loopCnt = %d\n", loopCnt+1);

		//if(SHOW_ENCODE == 1){_show_map_standard_item(encode_item);}

		err_spat = j2735r41_spat_encode(encode_item, encodeBuf, MAX_SPAT_BUFF_SIZE, &size, 1);
		if(err_spat < 0)
		{		
			printf("spat encode fail...\n");
			return -1;	
		}
		_cmd_spat_free_struct(encode_item);

		decoded_item = (spat_standard_item *)j2735_calloc(1, sizeof(spat_standard_item));
		err_spat = j2735r41_spat_decode(decoded_item, encodeBuf, MAX_SPAT_BUFF_SIZE, 1);
		if(err_spat < 0)
		{
			printf("decode error...\n");
			return -1;
		}

		if(SHOW_DECODED == 1){_show_spat_standard_item(decoded_item);}
		
		j2735r41_spat_free_struct(decoded_item);
		j2735_free(encodeBuf);
		
		loopCnt++;
		(void) tx_thread_sleep(10);
	}
	
	printf("%s", "J2735r41_SPAT Test command executed successfully.\n");
		
	return CLI_OK;
}

int _encode_txSpat(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	int i, j, k, m;
	spat_standard_item *encode_item = NULL;
	
	encode_item = (spat_standard_item *)j2735_calloc(1, sizeof(spat_standard_item));  
	/*(1) msgID */
	encode_item->msg_id = 13;
	/*(2) msgSubID **OP**/
	encode_item->mask_msg_sub_id = 1;	
	encode_item->msg_sub_id = 255;	
	/*(3) name **OP**/
	encode_item->mask_name = 1;
	memcpy(encode_item->name, "SPAT_name", strlen("SPAT_name"));
	encode_item->name[strlen("SPAT_name")+1] = '\0';
	/*(4) intersections */
	encode_item->intersections_num = 2;
	encode_item->intersections = (intersection_state_item *)j2735_calloc(encode_item->intersections_num, sizeof(intersection_state_item));
	for(i = 0; i < encode_item->intersections_num; i++)
	{
		/* (4-1) name **OP**/
		encode_item->intersections[i].mask_name = 1;
		memcpy(encode_item->intersections[i].name, "intersections_name", strlen("intersections_name"));
		encode_item->intersections[i].name[strlen("intersections_name")+1] = '\0';
		/* (4-2) id */
		encode_item->intersections[i].id.mask_region = 1;		
		encode_item->intersections[i].id.region = 65535;		
		encode_item->intersections[i].id.id = 65534;
		/* (4-3) revision */
		encode_item->intersections[i].revision = 127;
		/* (4-4) status */
		encode_item->intersections[i].status = 1024;
		/* (4-5) moy **OP**/
		encode_item->intersections[i].mask_moy = 1;		
		encode_item->intersections[i].moy = 527040;		
		/* (4-6) timeStamp **OP**/
		encode_item->intersections[i].mask_time_stamp = 1;		
		encode_item->intersections[i].time_stamp = 65533;		
		/* (4-7) enabledLanes **OP**/
		encode_item->intersections[i].mask_enabled_lanes = 1;		
		encode_item->intersections[i].enabled_lanes_num = 2;
		encode_item->intersections[i].enabled_lanes = (unsigned char *)j2735_calloc(encode_item->intersections[i].enabled_lanes_num, sizeof(unsigned char));
		for(j = 0; j < encode_item->intersections[i].enabled_lanes_num; j++)
		{
			encode_item->intersections[i].enabled_lanes[j] = 255;
		}		
		/* (4-8) states */
		encode_item->intersections[i].states_num = 1;
		encode_item->intersections[i].states = (movement_state_item *)j2735_calloc(encode_item->intersections[i].states_num, sizeof(movement_state_item));
		for(j = 0; j < encode_item->intersections[i].states_num; j++)
		{
			/* (4-8-1) movementName **OP**/
			encode_item->intersections[i].states[j].mask_movement_name = 1;
			memcpy(encode_item->intersections[i].states[j].movement_name, "movement_name", strlen("movement_name"));
			encode_item->intersections[i].states[j].movement_name[strlen("movement_name")+1] = '\0';
			/* (4-8-2) signalGroup */
			encode_item->intersections[i].states[j].signal_group = 254;
			/* (4-8-3) state_time_speed */
			encode_item->intersections[i].states[j].state_time_speed_num = 1;
			encode_item->intersections[i].states[j].state_time_speed = (movement_event_item *)j2735_calloc(encode_item->intersections[i].states[j].state_time_speed_num, sizeof(movement_event_item));
			for(k = 0; k < encode_item->intersections[i].states[j].state_time_speed_num; k++)
			{
				/* (4-8-3-1) eventState */
				encode_item->intersections[i].states[j].state_time_speed[k].event_state = 9;
				/* (4-8-3-2) timing **OP**/
				encode_item->intersections[i].states[j].state_time_speed[k].mask_timing = 1;				
				/* (4-8-3-2-1) startTime **OP**/
				encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_start_time = 1;				
				encode_item->intersections[i].states[j].state_time_speed[k].timing.start_time = 36002;				
				/* (4-8-3-2-2) minEndTime */
				encode_item->intersections[i].states[j].state_time_speed[k].timing.min_end_time = 36001;
				/* (4-8-3-2-3) maxEndTime **OP**/
				encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_max_end_time = 1;				
				encode_item->intersections[i].states[j].state_time_speed[k].timing.max_end_time = 36000;				
				/* (4-8-3-2-4) likelyTime **OP**/
				encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_likely_time = 1;				
				encode_item->intersections[i].states[j].state_time_speed[k].timing.likely_time = 35999;				
				/* (4-8-3-2-5) confidence **OP**/
				encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_confidence = 1;				
				encode_item->intersections[i].states[j].state_time_speed[k].timing.confidence = 15;				
				/* (4-8-3-2-6) nextTime **OP**/
				encode_item->intersections[i].states[j].state_time_speed[k].timing.mask_next_time = 1;				
				encode_item->intersections[i].states[j].state_time_speed[k].timing.next_time = 35998;				
				/* (4-8-3-3) speeds **OP**/
				encode_item->intersections[i].states[j].state_time_speed[k].mask_speeds = 1;				
				encode_item->intersections[i].states[j].state_time_speed[k].speeds_num = 1;
				encode_item->intersections[i].states[j].state_time_speed[k].speeds = (advisory_speed_item *)j2735_calloc(encode_item->intersections[i].states[j].state_time_speed[k].speeds_num, sizeof(advisory_speed_item));	
				for(m = 0; m < encode_item->intersections[i].states[j].state_time_speed[k].speeds_num; m++)
				{
					/* (4-8-3-3-1) type */
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].type = 3;
					/* (4-8-3-3-2) speed **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_speed = 1;					
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].speed = 500;					
					/* (4-8-3-3-3) confidence **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_confidence = 1;					
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].confidence = 7;					
					/* (4-8-3-3-4) distance **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_distance = 1;					
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].distance = 10000;					
					/* (4-8-3-3-5) Class **OP**/
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].mask_as_class = 1;					
					encode_item->intersections[i].states[j].state_time_speed[k].speeds[m].as_class = 255;
				}				
			}
			/* (4-8-4) maneuverAssistList **OP**/
			encode_item->intersections[i].states[j].mask_maneuver_assist_list = 1;			
			encode_item->intersections[i].states[j].maneuver_assist_list_num = 1;
			encode_item->intersections[i].states[j].maneuver_assist_list = (connection_maneuver_assist_item *)j2735_calloc(encode_item->intersections[i].states[j].maneuver_assist_list_num, sizeof(connection_maneuver_assist_item));	
			for(k = 0; k < encode_item->intersections[i].states[j].maneuver_assist_list_num; k++)
			{
				/* (4-8-4-1) connectionID */
				encode_item->intersections[i].states[j].maneuver_assist_list[k].connection_id = 255;
				/* (4-8-4-2) queueLength **OP**/
				encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_queue_length = 1;				
				encode_item->intersections[i].states[j].maneuver_assist_list[k].queue_length = 9999;				
				/* (4-8-4-3) availableStorageLength **OP**/
				encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_available_storage_length = 1;				
				encode_item->intersections[i].states[j].maneuver_assist_list[k].available_storage_length = 9998;										
				/* (4-8-4-4) waitOnStop **OP**/
				encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_wait_on_stop = 1;				
				encode_item->intersections[i].states[j].maneuver_assist_list[k].wait_on_stop = 0;
				/* (4-8-4-5) pedBicycleDetect **OP**/
				encode_item->intersections[i].states[j].maneuver_assist_list[k].mask_ped_bicycle_detect = 1;				
				encode_item->intersections[i].states[j].maneuver_assist_list[k].ped_bicycle_detect = 0;				
			}			
		}
		/* (4-9) maneuverAssistList **OP**/
		encode_item->intersections[i].mask_maneuver_assist_list = 1;		
		encode_item->intersections[i].maneuver_assist_list_num = 1;
		encode_item->intersections[i].maneuver_assist_list = (connection_maneuver_assist_item *)j2735_calloc(encode_item->intersections[i].maneuver_assist_list_num, sizeof(connection_maneuver_assist_item));	
		for(j = 0; j < encode_item->intersections[i].maneuver_assist_list_num; j++)
		{
			/* (4-9-1) connectionID */
			encode_item->intersections[i].maneuver_assist_list[j].connection_id = 255;
			/* (4-9-2) queueLength **OP**/
			encode_item->intersections[i].maneuver_assist_list[j].mask_queue_length = 1;			
			encode_item->intersections[i].maneuver_assist_list[j].queue_length = 10000;			
			/* (4-9-3) availableStorageLength **OP**/
			encode_item->intersections[i].maneuver_assist_list[j].mask_available_storage_length = 1;			
			encode_item->intersections[i].maneuver_assist_list[j].available_storage_length = 9999;									
			/* (4-9-4) waitOnStop **OP**/
			encode_item->intersections[i].maneuver_assist_list[j].mask_wait_on_stop = 1;			
			encode_item->intersections[i].maneuver_assist_list[j].wait_on_stop = 0;			
			/* (4-9-5) pedBicycleDetect **OP**/
			encode_item->intersections[i].maneuver_assist_list[j].mask_ped_bicycle_detect = 1;			
			encode_item->intersections[i].maneuver_assist_list[j].ped_bicycle_detect = 0;
		}		
		/* (4-10) priority **OP**/
		encode_item->intersections[i].mask_priority = 1;
		encode_item->intersections[i].priority = 0xEE;		
		/* (4-11) preempt **OP**/
		encode_item->intersections[i].mask_preempt = 1;
		encode_item->intersections[i].preempt = 0xDD;
		
	}

	/*************************************************************/

	if(SHOW_ENCODE == 1)
	{
		printf("\nSPAT Encode Data:\n");
		_show_spat_standard_item(encode_item);
	}
	
	return j2735r41_spat_encode(encode_item, encodeBuf, input_buf_size, size, 1);
}

int cmd_txSpat(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txSpat <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txSpat 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_SPAT_BUFF_SIZE);
	_encode_txSpat(encodeBuf, MAX_SPAT_BUFF_SIZE, &len);
	
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

	printf("%s", "cmd_txSpat command executed successfully.\n");
	return CLI_OK;
}


