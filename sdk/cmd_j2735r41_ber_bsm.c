#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <linuxapi.h>
#include <tx_api.h>
#include <craton/debug.h>
#include "error_code.h"

#include "j2735r41_bsm_api.h"
#include "j2735r41_common.h"

#include <libcli.h>
#include <craton/cli.h>
#include <atlk/nav_service.h>

#include <dot3_common.h>
#include <dot3_api.h>
#include <j2735_mem.h>

#define SHOW_ENCODE 1
#define SHOW_DECODED 1

static int stop_bsm;

void stop_bsm_func(ULONG input)
{
	(void)input;
	stop_bsm = 1;
	return;
}

void _cmd_bsm_free_struct(bsm_standard_item *free_bsm_item)
{
	int i;

	if(free_bsm_item == NULL){
		debug_printf("[BSM] Error: free_bsm_struct() input is NULL\n");
	}else
	{		
		/*(1) msgID */
		/*(2) blob1 */
		/*(3) safety_ext *OP**/
		if(free_bsm_item->safety_ext != NULL)
		{
			/*(3-1)***events****/
			/*(3-2)***pathHistory****/
			if(free_bsm_item->safety_ext->path_history != NULL)
			{
				/*(3-2-1)***initialPosition****/
				if(free_bsm_item->safety_ext->path_history->initial_position != NULL)
				{
					/*(3-2-1-1)***utcTime****/
					if(free_bsm_item->safety_ext->path_history->initial_position->utc_time != NULL){
						j2735_free(free_bsm_item->safety_ext->path_history->initial_position->utc_time);
					}
					
					/*(3-2-1-6)***speed****/
					if(free_bsm_item->safety_ext->path_history->initial_position->speed != NULL){
						j2735_free(free_bsm_item->safety_ext->path_history->initial_position->speed);
					}
					
					/*(3-2-1-7)***posAccuracy****/
					if(free_bsm_item->safety_ext->path_history->initial_position->pos_accuracy != NULL){
						j2735_free(free_bsm_item->safety_ext->path_history->initial_position->pos_accuracy);
					}
					
					/*(3-2-1-8)***timeConfidence****/
					/*(3-2-1-9)***posConfidence****/
					if(free_bsm_item->safety_ext->path_history->initial_position->pos_confidence != NULL){
						j2735_free(free_bsm_item->safety_ext->path_history->initial_position->pos_confidence);
					}
					
					/*(3-2-1-10)***speedConfidence****/
					if(free_bsm_item->safety_ext->path_history->initial_position->speed_confidence != NULL){
						j2735_free(free_bsm_item->safety_ext->path_history->initial_position->speed_confidence);
					}
					
					j2735_free(free_bsm_item->safety_ext->path_history->initial_position);
				}

				/*(3-2-4)***crumbData****/
				/*(3-2-4)***crumbDataChoice 1****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_01 != NULL){
					for(i = 0; i < free_bsm_item->safety_ext->path_history->path_history_point_sets_01_num; i++){
						if(free_bsm_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy != NULL){
							j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy);
						}

						if(free_bsm_item->safety_ext->path_history->path_history_point_sets_01[i].speed != NULL){
							j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_01[i].speed);
						}
					}
					
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_01);
				}
				
				/*(3-2-4)***crumbDataChoice 2****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_02 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_02);
				}
				
				/*(3-2-4)***crumbDataChoice 3****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_03 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_03);
				}
				
				/*(3-2-4)***crumbDataChoice 4****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_04 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_04);
				}
				
				/*(3-2-4)***crumbDataChoice 5****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_05 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_05);
				}
				
				/*(3-2-4)***crumbDataChoice 6****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_06 != NULL){
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_06);
				}
				/*(3-2-4)***crumbDataChoice 7****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_07 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_07);
				}
				
				/*(3-2-4)***crumbDataChoice 8****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_08 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_08);
				}
				
				/*(3-2-4)***crumbDataChoice 9****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_09 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_09);
				}
				
				/*(3-2-4)***crumbDataChoice 10****/
				if(free_bsm_item->safety_ext->path_history->path_history_point_sets_10 != NULL){						
					j2735_free(free_bsm_item->safety_ext->path_history->path_history_point_sets_10);
				}			

				j2735_free(free_bsm_item->safety_ext->path_history);
			}
			
			
			/*(3-3)***pathPrediction****/
			if(free_bsm_item->safety_ext->path_prediction != NULL){				
				j2735_free(free_bsm_item->safety_ext->path_prediction);
			}
			
			/*(3-4)***theRTCM****/
			if(free_bsm_item->safety_ext->the_rtcm != NULL){
				/*(3-4-1)***anchorPoint****/
				if(free_bsm_item->safety_ext->the_rtcm->anchor_point != NULL){
					/*(3-4-1-1)***utcTime****/
					if(free_bsm_item->safety_ext->the_rtcm->anchor_point->utc_time != NULL){
						j2735_free(free_bsm_item->safety_ext->the_rtcm->anchor_point->utc_time);
					}

					/*(3-4-1-6)***speed****/
					if(free_bsm_item->safety_ext->the_rtcm->anchor_point->speed != NULL){
						j2735_free(free_bsm_item->safety_ext->the_rtcm->anchor_point->speed);
					}
					
					/*(3-4-1-7)***posAccuracy****/
					if(free_bsm_item->safety_ext->the_rtcm->anchor_point->pos_accuracy != NULL){
						j2735_free(free_bsm_item->safety_ext->the_rtcm->anchor_point->pos_accuracy);
					}					
					
					/*(3-4-1-9)***posConfidence****/
					if(free_bsm_item->safety_ext->the_rtcm->anchor_point->pos_confidence != NULL){
						j2735_free(free_bsm_item->safety_ext->the_rtcm->anchor_point->pos_confidence);
					}
					
					/*(3-4-1-10)***speedConfidence****/
					if(free_bsm_item->safety_ext->the_rtcm->anchor_point->speed_confidence != NULL){
						j2735_free(free_bsm_item->safety_ext->the_rtcm->anchor_point->speed_confidence);
					}
					
					j2735_free(free_bsm_item->safety_ext->the_rtcm->anchor_point);
				}
				
				j2735_free(free_bsm_item->safety_ext->the_rtcm);
			}
			
			j2735_free(free_bsm_item->safety_ext);
		}
		
		/*(4) status *OP**/
		if(free_bsm_item->status != NULL)
		{
			/*(4-3)***wipers****/
			if(free_bsm_item->status->wipers != NULL){
				j2735_free(free_bsm_item->status->wipers);
			}

			/*(4-4)***brakeStatus****/
			if(free_bsm_item->status->brake_status != NULL){
				j2735_free(free_bsm_item->status->brake_status);
			}

			/*(4-11)***steering****/
			if(free_bsm_item->status->steering != NULL){
				j2735_free(free_bsm_item->status->steering);
			}

			/*(4-12)***accelSets****/
			if(free_bsm_item->status->accel_sets != NULL){
				if(free_bsm_item->status->accel_sets->accel_4way != NULL){					
					j2735_free(free_bsm_item->status->accel_sets->accel_4way);
				}

				if(free_bsm_item->status->accel_sets->confidence_set != NULL){
					if(free_bsm_item->status->accel_sets->confidence_set->accel_confidence != NULL){
						j2735_free(free_bsm_item->status->accel_sets->confidence_set->accel_confidence);
					}

					if(free_bsm_item->status->accel_sets->confidence_set->speed_confidence != NULL){
						j2735_free(free_bsm_item->status->accel_sets->confidence_set->speed_confidence);
					}

					if(free_bsm_item->status->accel_sets->confidence_set->pos_confidence != NULL){
						j2735_free(free_bsm_item->status->accel_sets->confidence_set->pos_confidence);
					}
					
					j2735_free(free_bsm_item->status->accel_sets->confidence_set);
				}
				
				j2735_free(free_bsm_item->status->accel_sets);
			}

			/*(4-13)***object****/
			if(free_bsm_item->status->object != NULL){
				if(free_bsm_item->status->object->date_time != NULL){
					j2735_free(free_bsm_item->status->object->date_time);
				}
			
				j2735_free(free_bsm_item->status->object);
			}

			/*(4-14)***fullPos****/
			if(free_bsm_item->status->full_pos != NULL){
				/*(4-14-1)***utcTime****/
				if(free_bsm_item->status->full_pos->utc_time != NULL){
					j2735_free(free_bsm_item->status->full_pos->utc_time);
				}

				/*(4-14-6)***speed****/
				if(free_bsm_item->status->full_pos->speed != NULL){
					j2735_free(free_bsm_item->status->full_pos->speed);
				}

				/*(4-14-7)***posAccuracy****/
				if(free_bsm_item->status->full_pos->pos_accuracy != NULL){
					j2735_free(free_bsm_item->status->full_pos->pos_accuracy);
				}

				/*(4-14-9)***posConfidence****/
				if(free_bsm_item->status->full_pos->pos_confidence != NULL){
					j2735_free(free_bsm_item->status->full_pos->pos_confidence);
				}

				/*(4-14-10)***speedConfidence****/
				if(free_bsm_item->status->full_pos->speed_confidence != NULL){
					j2735_free(free_bsm_item->status->full_pos->speed_confidence);
				}
				
				j2735_free(free_bsm_item->status->full_pos);
			}

			/*(4-16)***speedHeadC****/
			if(free_bsm_item->status->speed_head_c != NULL){
				j2735_free(free_bsm_item->status->speed_head_c);
			}

			/*(4-18)***vehicleData****/
			if(free_bsm_item->status->vehicle_data != NULL){
				j2735_free(free_bsm_item->status->vehicle_data);
			}

			/*(4-19)***vehicleIdent****/
			if(free_bsm_item->status->vehicle_ident != NULL){
				j2735_free(free_bsm_item->status->vehicle_ident);
			}

			/*(4-20)***j1939data****/
			if(free_bsm_item->status->j1939_data != NULL){
				if(free_bsm_item->status->j1939_data->tires != NULL){
					j2735_free(free_bsm_item->status->j1939_data->tires);
				}

				if(free_bsm_item->status->j1939_data->axle != NULL){
					j2735_free(free_bsm_item->status->j1939_data->axle);
				}			
				
				j2735_free(free_bsm_item->status->j1939_data);
			}

			/*(4-21)***weatherReport****/
			if(free_bsm_item->status->weather_report != NULL){
				j2735_free(free_bsm_item->status->weather_report);
			}

			j2735_free(free_bsm_item->status);
		}	
		
		j2735_free(free_bsm_item);
	}
}

void _show_bsm_standard_item(bsm_standard_item *decoded_item)
{
	int i;
	
	printf("==========BSM Message==========\n");

	/*(1) msgID */
	printf("msg_id = %d\n", decoded_item->msg_id);
	/*(2) blob1*/
	/*(1-2-1)***msgCnt****/
	printf("blob1.msg_cnt = %d\n", decoded_item->blob1.msg_cnt);
	/*(1-2-2)***id****/
	printf("blob1.id = %x %x %x %x\n", decoded_item->blob1.id[0], decoded_item->blob1.id[1], decoded_item->blob1.id[2], decoded_item->blob1.id[3]);
	/*(1-2-3)***secMark****/
	printf("blob1.sec_mark = %d\n", decoded_item->blob1.sec_mark);

	/*(1-2-4)***lat****/
	printf("blob1.lat = %d\n", decoded_item->blob1.lat);
	/*(1-2-5)***Longitude****/
	printf("blob1.longitude = %d\n", decoded_item->blob1.longitude);
	/*(1-2-6)***elev***/
	printf("blob1.elev = %d\n", decoded_item->blob1.elev);
	/*(1-2-7)***accuracy***/
	printf("blob1.accuracy.semi_major = %d\n", decoded_item->blob1.accuracy.semi_major);
	printf("blob1.accuracy.semi_minor = %d\n", decoded_item->blob1.accuracy.semi_minor);
	printf("blob1.accuracy.semi_major_orient = %hu\n", decoded_item->blob1.accuracy.semi_major_orient);
	/*(1-2-8)***speed***/
	printf("blob1.speed.speed = %d\n", decoded_item->blob1.speed.speed);
	printf("blob1.speed.transmission_state = %d\n", decoded_item->blob1.speed.transmission_state);
	/*(1-2-9)***heading***/
	printf("blob1.heading = %d\n", decoded_item->blob1.heading);
	/*(1-2-10)***angle***/
	printf("blob1.angle = %d\n", decoded_item->blob1.angle);
	/*(1-2-11)***accelSet***/
	printf("blob1.accelSet.longitude = %d\n", decoded_item->blob1.accel_set.longitude);
	printf("blob1.accelSet.lat = %d\n", decoded_item->blob1.accel_set.lat);
	printf("blob1.accelSet.vert = %d\n", decoded_item->blob1.accel_set.vert);
	printf("blob1.accelSet.yaw = %d\n", decoded_item->blob1.accel_set.yaw);
	/*(1-2-12)***brakes***/
	printf("blob1.brakes.wheel_brakes = %d\n", decoded_item->blob1.brakes.wheel_brakes);
	printf("blob1.brakes.wheel_brakes_unavailable = %d\n", decoded_item->blob1.brakes.wheel_brakes_unavailable);
	printf("blob1.brakes.spare_bit = %d\n", decoded_item->blob1.brakes.spare_bit);
	printf("blob1.brakes.traction = %d\n", decoded_item->blob1.brakes.traction);
	printf("blob1.brakes.abs = %d\n", decoded_item->blob1.brakes.abs);
	printf("blob1.brakes.scs = %d\n", decoded_item->blob1.brakes.scs);
	printf("blob1.brakes.brake_boost = %d\n", decoded_item->blob1.brakes.brake_boost);
	printf("blob1.brakes.aux_brakes = %d\n", decoded_item->blob1.brakes.aux_brakes);
	/*(1-2-13)***size***/
	printf("blob1.size.width = %d\n", decoded_item->blob1.size.width);
	printf("blob1.size.length = %d\n\n", decoded_item->blob1.size.length);
	/*(3) safety_ext *OP**/
	if(decoded_item->mask_safety_ext == 1)
	{
		/*(3-1)***events****/
		if(decoded_item->safety_ext->mask_events == 1)
		{	
			printf("safety_ext->events = %d\n", decoded_item->safety_ext->events);
		}			
		/*(3-2)***pathHistory****/
		if(decoded_item->safety_ext->mask_path_history == 1)
		{
			/*(3-2-1)***initialPosition****/
			if(decoded_item->safety_ext->path_history->mask_initial_position == 1)
			{
				/*(3-2-1-1)***utcTime****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_utc_time == 1)
				{
					/*(3-2-1-1-1)***year****/
					if(decoded_item->safety_ext->path_history->initial_position->utc_time->mask_year == 1)
					{
						printf("safety_ext->path_history->initial_position->utc_time->year = %d\n", decoded_item->safety_ext->path_history->initial_position->utc_time->year);
					}						
					/*(3-2-1-1-2)***month****/
					if(decoded_item->safety_ext->path_history->initial_position->utc_time->mask_month == 1)
					{
						printf("safety_ext->path_history->initial_position->utc_time->month = %d\n", decoded_item->safety_ext->path_history->initial_position->utc_time->month);
					}						
					/*(3-2-1-1-3)***day****/
					if(decoded_item->safety_ext->path_history->initial_position->utc_time->mask_day == 1)
					{
						printf("safety_ext->path_history->initial_position->utc_time->day = %d\n", decoded_item->safety_ext->path_history->initial_position->utc_time->day);
					}						
					/*(3-2-1-1-4)***hour****/
					if(decoded_item->safety_ext->path_history->initial_position->utc_time->mask_hour == 1)
					{
						printf("safety_ext->path_history->initial_position->utc_time->hour = %d\n", decoded_item->safety_ext->path_history->initial_position->utc_time->hour);
					}						
					/*(3-2-1-1-5)***minute****/
					if(decoded_item->safety_ext->path_history->initial_position->utc_time->mask_minute == 1)
					{
						printf("safety_ext->path_history->initial_position->utc_time->minute = %d\n", decoded_item->safety_ext->path_history->initial_position->utc_time->minute);
					}						
					/*(3-2-1-1-6)***second****/
					if(decoded_item->safety_ext->path_history->initial_position->utc_time->mask_second == 1)
					{
						printf("safety_ext->path_history->initial_position->utc_time->second = %d\n", decoded_item->safety_ext->path_history->initial_position->utc_time->second);
					}				
				}
				/*(3-2-1-2)***longitude****/
				printf("safety_ext->path_history->initial_position->longitude = %ld\n", decoded_item->safety_ext->path_history->initial_position->longitude);
				/*(3-2-1-3)***lat****/
				printf("safety_ext->path_history->initial_position->lat = %ld\n", decoded_item->safety_ext->path_history->initial_position->lat);
				/*(3-2-1-4)***elevation****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_elevation == 1)
				{
					printf("safety_ext->path_history->initial_position->elevation = %d\n", decoded_item->safety_ext->path_history->initial_position->elevation);
				}
				/*(3-2-1-5)***heading****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_heading == 1)
				{
					printf("safety_ext->path_history->initial_position->heading = %d\n", decoded_item->safety_ext->path_history->initial_position->heading);
				}				
				/*(3-2-1-6)***speed****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_speed == 1)
				{
					printf("safety_ext->path_history->initial_position->heading = %d\n", decoded_item->safety_ext->path_history->initial_position->speed->speed);
					printf("safety_ext->path_history->initial_position->transmissionState = %d\n", decoded_item->safety_ext->path_history->initial_position->speed->transmission_state);
				}				
				/*(3-2-1-7)***posAccuracy****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_pos_accuracy == 1)
				{
					printf("safety_ext->path_history->initial_position->pos_accuracy->semi_major = %d\n", decoded_item->safety_ext->path_history->initial_position->pos_accuracy->semi_major);
					printf("safety_ext->path_history->initial_position->pos_accuracy->semi_minor = %d\n", decoded_item->safety_ext->path_history->initial_position->pos_accuracy->semi_minor);
					printf("safety_ext->path_history->initial_position->pos_accuracy->semi_major_orient = %hu\n", decoded_item->safety_ext->path_history->initial_position->pos_accuracy->semi_major_orient);
				}				
				/*(3-2-1-8)***timeConfidence****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_time_confidence == 1)
				{
					printf("safety_ext->path_history->initial_position->time_confidence = %d\n", decoded_item->safety_ext->path_history->initial_position->time_confidence);
				}		
				/*(3-2-1-9)***posConfidence****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_pos_confidence == 1)
				{
					printf("safety_ext->path_history->initial_position->pos_confidence->pos = %d\n", decoded_item->safety_ext->path_history->initial_position->pos_confidence->pos);
					printf("safety_ext->path_history->initial_position->pos_confidence->elevation = %d\n", decoded_item->safety_ext->path_history->initial_position->pos_confidence->elevation);
				}
				/*(3-2-1-10)***speedConfidence****/
				if(decoded_item->safety_ext->path_history->initial_position->mask_speed_confidence == 1)
				{
					printf("safety_ext->path_history->initial_position->speed_confidence->heading = %d\n", decoded_item->safety_ext->path_history->initial_position->speed_confidence->heading);
					printf("safety_ext->path_history->initial_position->speed_confidence->speed = %d\n", decoded_item->safety_ext->path_history->initial_position->speed_confidence->speed);
					printf("safety_ext->path_history->initial_position->speed_confidence->throttle = %d\n", decoded_item->safety_ext->path_history->initial_position->speed_confidence->throttle);
				}
			}				 
			/*(3-2-2)***currGPSstatus****/
			if(decoded_item->safety_ext->path_history->mask_curr_gps_status == 1)
			{
				printf("safety_ext->path_history->currGPSstatus = %x \n", decoded_item->safety_ext->path_history->curr_gps_status);
			}				 
			/*(3-2-3)***itemCnt****/
			if(decoded_item->safety_ext->path_history->mask_item_cnt == 1)
			{
				printf("safety_ext->path_history->item_cnt = %d\n", decoded_item->safety_ext->path_history->item_cnt);
			}				 
			/*(3-2-4)***crumbData****/
			printf("safety_ext->path_history->crumb_data_choice = %d\n", decoded_item->safety_ext->path_history->crumb_data_choice);
			/*(3-2-4)***crumbDataChoice 1****/
			if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_01)
			{
				printf("safety_ext->path_history->path_history_point_sets_01_num = %d\n", decoded_item->safety_ext->path_history->path_history_point_sets_01_num);
				for(i = 0; i < decoded_item->safety_ext->path_history->path_history_point_sets_01_num; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_01[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_01[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].long_offset);
					if(decoded_item->safety_ext->path_history->path_history_point_sets_01[i].mask_elevation_offset == 1)
					{
						printf("safety_ext->path_history->path_history_point_sets_01[%d].elevation_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].elevation_offset);
					}				
					if(decoded_item->safety_ext->path_history->path_history_point_sets_01[i].mask_time_offset == 1)
					{
						printf("safety_ext->path_history->path_history_point_sets_01[%d].time_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].time_offset);
					}
					if(decoded_item->safety_ext->path_history->path_history_point_sets_01[i].mask_pos_accuracy == 1)
					{
						printf("safety_ext->path_history->path_history_point_sets_01[%d].pos_accuracy->semi_major = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_major);
						printf("safety_ext->path_history->path_history_point_sets_01[%d].pos_accuracy->semi_minor = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_minor);
						printf("safety_ext->path_history->path_history_point_sets_01[%d].pos_accuracy->semi_major_orient = %hu\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_major_orient);
						
					}						
					if(decoded_item->safety_ext->path_history->path_history_point_sets_01[i].mask_heading == 1)
					{
						printf("safety_ext->path_history->path_history_point_sets_01[%d].heading = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].heading);
					}
					if(decoded_item->safety_ext->path_history->path_history_point_sets_01[i].mask_speed == 1)
					{
						printf("safety_ext->path_history->path_history_point_sets_01[%d].speed->speed = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].speed->speed);
						printf("safety_ext->path_history->path_history_point_sets_01[%d].speed->transmission_state = %d\n\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_01[i].speed->transmission_state);
					}
				}
			}			 
			/*(3-2-4)***crumbDataChoice 2****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_02)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_02[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].elevation_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].elevation_offset);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].time_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].time_offset);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].pos_accuracy.semi_major = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].pos_accuracy.semi_major);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].pos_accuracy.semi_minor = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].pos_accuracy.semi_minor);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].pos_accuracy.semi_major_orient = %hu\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].pos_accuracy.semi_major_orient);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].heading = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].heading);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].speed.speed = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].speed.speed);
					printf("safety_ext->path_history->path_history_point_sets_02[%d].speed.transmission_state = %d\n\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_02[i].speed.transmission_state);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 3****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_03)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_03[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_03[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_03[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_03[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_03[%d].elevation_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_03[i].elevation_offset);
					printf("safety_ext->path_history->path_history_point_sets_03[%d].time_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_03[i].time_offset);
					printf("safety_ext->path_history->path_history_point_sets_03[%d].pos_accuracy.semi_major = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_03[i].pos_accuracy.semi_major);
					printf("safety_ext->path_history->path_history_point_sets_03[%d].pos_accuracy.semi_minor = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_03[i].pos_accuracy.semi_minor);
					printf("safety_ext->path_history->path_history_point_sets_03[%d].pos_accuracy.semi_major_orient = %hu\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_03[i].pos_accuracy.semi_major_orient);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 4****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_04)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_04[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_04[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_04[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_04[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_04[%d].elevation_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_04[i].elevation_offset);
					printf("safety_ext->path_history->path_history_point_sets_04[%d].time_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_04[i].time_offset);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 5****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_05)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_05[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_05[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_05[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_05[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_05[%d].elevation_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_05[i].elevation_offset);
					printf("safety_ext->path_history->path_history_point_sets_05[%d].pos_accuracy.semi_major = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_05[i].pos_accuracy.semi_major);
					printf("safety_ext->path_history->path_history_point_sets_05[%d].pos_accuracy.semi_minor = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_05[i].pos_accuracy.semi_minor);
					printf("safety_ext->path_history->path_history_point_sets_05[%d].pos_accuracy.semi_major_orient = %hu\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_05[i].pos_accuracy.semi_major_orient);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 6****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_06)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_06[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_06[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_06[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_06[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_06[%d].elevation_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_06[i].elevation_offset);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 7****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_07)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_07[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_07[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_07[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_07[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_07[%d].time_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_07[i].time_offset);
					printf("safety_ext->path_history->path_history_point_sets_07[%d].pos_accuracy.semi_major = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_07[i].pos_accuracy.semi_major);
					printf("safety_ext->path_history->path_history_point_sets_07[%d].pos_accuracy.semi_minor = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_07[i].pos_accuracy.semi_minor);
					printf("safety_ext->path_history->path_history_point_sets_07[%d].pos_accuracy.semi_major_orient = %hu\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_07[i].pos_accuracy.semi_major_orient);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 8****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_08)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_08[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_08[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_08[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_08[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_08[%d].time_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_08[i].time_offset);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 9****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_09)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_09[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_09[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_09[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_09[i].long_offset);
					printf("safety_ext->path_history->path_history_point_sets_09[%d].pos_accuracy.semi_major = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_09[i].pos_accuracy.semi_major);
					printf("safety_ext->path_history->path_history_point_sets_09[%d].pos_accuracy.semi_minor = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_09[i].pos_accuracy.semi_minor);
					printf("safety_ext->path_history->path_history_point_sets_09[%d].pos_accuracy.semi_major_orient = %hu\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_09[i].pos_accuracy.semi_major_orient);
				}
			}			 
			/*(3-2-4)***crumbDataChoice 10****/
			else if(decoded_item->safety_ext->path_history->crumb_data_choice == CRUMB_DATA_PATH_HISTORY_POINT_SETS_10)
			{
				for(i = 0; i < decoded_item->safety_ext->path_history->item_cnt; i++)
				{
					printf("safety_ext->path_history->path_history_point_sets_10[%d].lat_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_10[i].lat_offset);
					printf("safety_ext->path_history->path_history_point_sets_10[%d].long_offset = %d\n", i, decoded_item->safety_ext->path_history->path_history_point_sets_10[i].long_offset);
				}
			}else{
				printf("safety_ext->path_history->crumb_data_choice = CRUMB_DATA_PATH_HISTORY_POINT_SETS_NOTHING\n");
			}
		}
		/*(3-3)***pathPrediction****/
		if(decoded_item->safety_ext->mask_path_prediction == 1)
		{	
			/*(3-3-1)***radiusOfCurve****/
			printf("safety_ext->path_prediction->radius_of_curve = %d\n", decoded_item->safety_ext->path_prediction->radius_of_curve);
			/*(3-3-2)***confidence****/
			printf("safety_ext->path_prediction->confidence = %d\n\n", decoded_item->safety_ext->path_prediction->confidence);
		}			
		/*(3-4)***theRTCM****/			
		if(decoded_item->safety_ext->mask_the_rtcm == 1)
		{	
			/*(3-4-1)***anchorPoint****/
			if(decoded_item->safety_ext->the_rtcm->mask_anchor_point == 1)
			{
				/*(3-4-1-1)***utcTime****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_utc_time == 1)
				{
					/*(3-4-1-1-1)***year****/
					if(decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_year == 1)
					{
						printf("safety_ext->the_rtcm->anchor_point->utc_time->year = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->year);
					}
					/*(3-4-1-1-2)***month****/
					if(decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_month == 1)
					{
						printf("safety_ext->the_rtcm->anchor_point->utc_time->month = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->month);
					}
					/*(3-4-1-1-3)***day****/
					if(decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_day == 1)
					{
						printf("safety_ext->the_rtcm->anchor_point->utc_time->day = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->day);
					}
					/*(3-4-1-1-4)***hour****/
					if(decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_hour == 1)
					{
						printf("safety_ext->the_rtcm->anchor_point->utc_time->hour = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->hour);
					}
					/*(3-4-1-1-5)***minute****/
					if(decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_minute == 1)
					{
						printf("safety_ext->the_rtcm->anchor_point->utc_time->minute = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->minute);
					}
					/*(3-4-1-1-6)***second****/
					if(decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_second == 1)
					{
						printf("safety_ext->the_rtcm->anchor_point->utc_time->second = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->utc_time->second);
					}					
				}
				/*(3-4-1-2)***longitude****/
				printf("safety_ext->the_rtcm->anchor_point->longitude = %ld\n", decoded_item->safety_ext->the_rtcm->anchor_point->longitude);
				/*(3-4-1-3)***lat****/
				printf("safety_ext->the_rtcm->anchor_point->lat = %ld\n", decoded_item->safety_ext->the_rtcm->anchor_point->lat);
				/*(3-4-1-4)***elevation****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_elevation == 1)
				{
					printf("safety_ext->the_rtcm->anchor_point->elevation = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->elevation);
				}
				/*(3-4-1-5)***heading****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_heading == 1)
				{
					printf("safety_ext->the_rtcm->anchor_point->heading = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->heading);
				}
				/*(3-4-1-6)***speed****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_speed == 1)
				{
					printf("safety_ext->the_rtcm->anchor_point->speed->speed = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->speed->speed);
					printf("safety_ext->the_rtcm->anchor_point->speed->transmission_state = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->speed->transmission_state);
				}					
				/*(3-4-1-7)***posAccuracy****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_pos_accuracy == 1)
				{
					printf("safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major);
					printf("safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_minor = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_minor);
					printf("safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major_orient = %hu\n", decoded_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major_orient);
				}
				/*(3-4-1-8)***timeConfidence****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_time_confidence == 1)
				{
					printf("safety_ext->the_rtcm->anchor_point->time_confidence = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->time_confidence);
				}				
				/*(3-4-1-9)***posConfidence****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_pos_confidence == 1)
				{
					printf("safety_ext->the_rtcm->anchor_point->pos_confidence->pos = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->pos_confidence->pos);
					printf("safety_ext->the_rtcm->anchor_point->pos_confidence->elevation = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->pos_confidence->elevation);
				}
				/*(3-4-1-10)***speedConfidence****/
				if(decoded_item->safety_ext->the_rtcm->anchor_point->mask_speed_confidence == 1)
				{
					printf("safety_ext->the_rtcm->anchor_point->speed_confidence->heading = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->speed_confidence->heading);
					printf("safety_ext->the_rtcm->anchor_point->speed_confidence->speed = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->speed_confidence->speed);
					printf("safety_ext->the_rtcm->anchor_point->speed_confidence->throttle = %d\n", decoded_item->safety_ext->the_rtcm->anchor_point->speed_confidence->throttle);
				}
			}				
			/*(3-4-2)***rtcmHeader****/
			/*(3-4-2-1)***status****/
			printf("safety_ext->the_rtcm->rtcm_header->status = %d\n", decoded_item->safety_ext->the_rtcm->rtcm_header.status);
			/*(3-4-2-2)***offsetSet****/
			printf("safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_x = %d\n", decoded_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_x);
			printf("safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_y = %d\n", decoded_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_y);
			printf("safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_z = %d\n", decoded_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_z);
			/*(3-4-3)***msg1001***/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1001 == 1)
			{
				printf("safety_ext->the_rtcm->msg1001 = %s\n", decoded_item->safety_ext->the_rtcm->msg1001);
			}
			/*(3-4-4)***msg1002****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1002 == 1)
			{
				printf("safety_ext->the_rtcm->msg1002 = %s\n", decoded_item->safety_ext->the_rtcm->msg1002);
			}
			/*(3-4-5)***msg1003****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1003 == 1)
			{
				printf("safety_ext->the_rtcm->msg1003 = %s\n", decoded_item->safety_ext->the_rtcm->msg1003);
			}
			/*(3-4-6)***msg1004****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1004 == 1)
			{
				printf("safety_ext->the_rtcm->msg1004 = %s\n", decoded_item->safety_ext->the_rtcm->msg1004);
			}
			/*(3-4-7)***msg1005****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1005 == 1)
			{
				printf("safety_ext->the_rtcm->msg1005 = %s\n", decoded_item->safety_ext->the_rtcm->msg1005);
			}
			/*(3-4-8)***msg1006****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1006 == 1)
			{
				printf("safety_ext->the_rtcm->msg1006 = %s\n", decoded_item->safety_ext->the_rtcm->msg1006);
			}
			/*(3-4-9)***msg1007****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1007 == 1)
			{
				printf("safety_ext->the_rtcm->msg1007 = %s\n", decoded_item->safety_ext->the_rtcm->msg1007);
			}
			/*(3-4-10)***msg1008****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1008 == 1)
			{
				printf("safety_ext->the_rtcm->msg1008 = %s\n", decoded_item->safety_ext->the_rtcm->msg1008);
			}
			/*(3-4-11)***msg1009****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1009 == 1)
			{
				printf("safety_ext->the_rtcm->msg1009 = %s\n", decoded_item->safety_ext->the_rtcm->msg1009);
			}
			/*(3-4-12)***msg1010****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1010 == 1)
			{
				printf("safety_ext->the_rtcm->msg1010 = %s\n", decoded_item->safety_ext->the_rtcm->msg1010);
			}
			/*(3-4-13)***msg1011****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1011 == 1)
			{
				printf("safety_ext->the_rtcm->msg1011 = %s\n", decoded_item->safety_ext->the_rtcm->msg1011);
			}
			/*(3-4-14)***msg1012****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1012 == 1)
			{
				printf("safety_ext->the_rtcm->msg1012 = %s\n", decoded_item->safety_ext->the_rtcm->msg1012);
			}
			/*(3-4-15)***msg1013****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1013 == 1)
			{
				printf("safety_ext->the_rtcm->msg1013 = %s\n", decoded_item->safety_ext->the_rtcm->msg1013);
			}
			/*(3-4-16)***msg1014****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1014 == 1)
			{
				printf("safety_ext->the_rtcm->msg1014 = %s\n", decoded_item->safety_ext->the_rtcm->msg1014);
			}
			/*(3-4-17)***msg1015****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1015 == 1)
			{
				printf("safety_ext->the_rtcm->msg1015 = %s\n", decoded_item->safety_ext->the_rtcm->msg1015);
			}
			/*(3-4-18)***msg1016****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1016 == 1)
			{
				printf("safety_ext->the_rtcm->msg1016 = %s\n", decoded_item->safety_ext->the_rtcm->msg1016);
			}
			/*(3-4-19)***msg1017****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1017 == 1)
			{
				printf("safety_ext->the_rtcm->msg1017 = %s\n", decoded_item->safety_ext->the_rtcm->msg1017);
			}
			/*(3-4-20)***msg1019****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1019 == 1)
			{
				printf("safety_ext->the_rtcm->msg1019 = %s\n", decoded_item->safety_ext->the_rtcm->msg1019);
			}
			/*(3-4-21)***msg1020****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1020 == 1)
			{
				printf("safety_ext->the_rtcm->msg1020 = %s\n", decoded_item->safety_ext->the_rtcm->msg1020);
			}
			/*(3-4-22)***msg1021****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1021 == 1)
			{
				printf("safety_ext->the_rtcm->msg1021 = %s\n", decoded_item->safety_ext->the_rtcm->msg1021);
			}
			/*(3-4-23)***msg1022****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1022 == 1)
			{
				printf("safety_ext->the_rtcm->msg1022 = %s\n", decoded_item->safety_ext->the_rtcm->msg1022);
			}
			/*(3-4-24)***msg1023****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1023 == 1)
			{
				printf("safety_ext->the_rtcm->msg1023 = %s\n", decoded_item->safety_ext->the_rtcm->msg1023);
			}
			/*(3-4-25)***msg1024****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1024 == 1)
			{
				printf("safety_ext->the_rtcm->msg1024 = %s\n", decoded_item->safety_ext->the_rtcm->msg1024);
			}
			/*(3-4-25)***msg1025****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1025 == 1)
			{
				printf("safety_ext->the_rtcm->msg1025 = %s\n", decoded_item->safety_ext->the_rtcm->msg1025);
			}
			/*(3-4-27)***msg1026****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1026 == 1)
			{
				printf("safety_ext->the_rtcm->msg1026 = %s\n", decoded_item->safety_ext->the_rtcm->msg1026);
			}
			/*(3-4-28)***msg1027****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1027 == 1)
			{
				printf("safety_ext->the_rtcm->msg1027 = %s\n", decoded_item->safety_ext->the_rtcm->msg1027);
			}
			/*(3-4-29)***msg1029****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1029 == 1)
			{
				printf("safety_ext->the_rtcm->msg1029 = %s\n", decoded_item->safety_ext->the_rtcm->msg1029);
			}
			/*(3-4-30)***msg1030****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1030 == 1)
			{
				printf("safety_ext->the_rtcm->msg1030 = %s\n", decoded_item->safety_ext->the_rtcm->msg1030);
			}
			/*(3-4-31)***msg1031****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1031 == 1)
			{
				printf("safety_ext->the_rtcm->msg1031 = %s\n", decoded_item->safety_ext->the_rtcm->msg1031);
			}
			/*(3-4-32)***msg1032****/
			if(decoded_item->safety_ext->the_rtcm->mask_msg1032 == 1)
			{
				printf("safety_ext->the_rtcm->msg1032 = %s\n", decoded_item->safety_ext->the_rtcm->msg1032);
			}
		}
	}
	/*(4) status *OP**/
	if(decoded_item->mask_status == 1)
	{	
		/*(4-1)***lights****/
		if(decoded_item->status->mask_lights == 1)
		{
			printf("status->lights = %d\n", decoded_item->status->lights);
		}
		/*(4-2)***lightBar****/
		if(decoded_item->status->mask_light_bar == 1)
		{
			printf("status->light_bar = %d\n", decoded_item->status->light_bar);
		}
		/*(4-3)***wipers****/
		if(decoded_item->status->mask_wipers == 1)
		{
			printf("status->wipers->status_front = %d\n", decoded_item->status->wipers->status_front);
			printf("status->wipers->rate_front = %d\n", decoded_item->status->wipers->rate_front);
			if(decoded_item->status->wipers->mask_status_rear == 1)
			{	
				printf("status->wipers->status_rear = %d\n", decoded_item->status->wipers->status_rear);
			}
			if(decoded_item->status->wipers->mask_rate_rear == 1)
			{	
				printf("status->wipers->rate_rear = %d\n", decoded_item->status->wipers->rate_rear);
			}
		}
		/*(4-4)***brakeStatus****/
		if(decoded_item->status->mask_brake_status == 1)
		{
			printf("status->brake_status->wheel_brakes = %d\n", decoded_item->status->brake_status->wheel_brakes);			
			printf("status->brake_status->traction = %d\n", decoded_item->status->brake_status->traction);
			printf("status->brake_status->abs = %d\n", decoded_item->status->brake_status->abs);
			printf("status->brake_status->scs = %d\n", decoded_item->status->brake_status->scs);
			printf("status->brake_status->brake_boost = %d\n", decoded_item->status->brake_status->brake_boost);
			printf("status->brake_status->spare_bit = %d\n", decoded_item->status->brake_status->spare_bits);
		}
		/*(4-5)***brakePressure****/
		if(decoded_item->status->mask_brake_pressure == 1)
		{	
			printf("status->brake_pressure = %d\n", decoded_item->status->brake_pressure);
		}
		/*(4-6)***roadFriction****/
		if(decoded_item->status->mask_road_friction == 1)
		{	
			printf("status->road_friction = %d\n", decoded_item->status->road_friction);
		}
		/*(4-7)***sunData****/
		if(decoded_item->status->mask_sun_data == 1)
		{	
			printf("status->sun_data = %d\n", decoded_item->status->sun_data);
		}
		/*(4-8)***rainData****/
		if(decoded_item->status->mask_rain_data == 1)
		{	
			printf("status->rain_data = %d\n", decoded_item->status->rain_data);
		}
		/*(4-9)***airTemp****/
		if(decoded_item->status->mask_air_temp == 1)
		{	
			printf("status->air_temp = %d\n", decoded_item->status->air_temp);
		}
		/*(4-10)***airPres****/
		if(decoded_item->status->mask_air_pres == 1)
		{	
			printf("status->air_pres = %d\n", decoded_item->status->air_pres);
		}
		/*(4-11)***steering****/
		if(decoded_item->status->mask_steering == 1)
		{
			printf("status->steering->angle = %d\n", decoded_item->status->steering->angle);
			if(decoded_item->status->steering->mask_confidence == 1)
			{	
				printf("status->steering->confidence = %d\n", decoded_item->status->steering->confidence);
			}				
			if(decoded_item->status->steering->mask_rate == 1)
			{
				printf("status->steering->rate = %d\n", decoded_item->status->steering->rate);
			}				
			if(decoded_item->status->steering->mask_wheels == 1)
			{
				printf("status->steering->wheels = %d\n", decoded_item->status->steering->wheels);
			}
		}
		/*(4-12)***accelSets****/
		if(decoded_item->status->mask_accel_sets == 1)
		{	
			if(decoded_item->status->accel_sets->mask_accel_4way == 1)
			{	
				printf("status->accel_sets->accel_4way->longitude = %d\n", decoded_item->status->accel_sets->accel_4way->longitude);
				printf("status->accel_sets->accel_4way->lat = %d\n", decoded_item->status->accel_sets->accel_4way->lat);
				printf("status->accel_sets->accel_4way->vert = %d\n", decoded_item->status->accel_sets->accel_4way->vert);
				printf("status->accel_sets->accel_4way->yaw = %d\n", decoded_item->status->accel_sets->accel_4way->yaw);
			}
			if(decoded_item->status->accel_sets->mask_vert_accel_thres == 1)
			{	
				printf("status->accel_sets->vert_accel_thres = %d\n", decoded_item->status->accel_sets->vert_accel_thres);
			}				
			if(decoded_item->status->accel_sets->mask_yaw_rate_con == 1)
			{	
				printf("status->accel_sets->yaw_rate_con = %d\n", decoded_item->status->accel_sets->yaw_rate_con);
			}				
			if(decoded_item->status->accel_sets->mask_hoz_accel_con == 1)
			{	
				printf("status->accel_sets->hoz_accel_con = %d\n", decoded_item->status->accel_sets->hoz_accel_con);
			}				
			if(decoded_item->status->accel_sets->mask_confidence_set == 1)
			{	
				if(decoded_item->status->accel_sets->confidence_set->mask_accel_confidence == 1)
				{	
					printf("status->accel_sets->confidence_set->accel_confidence->yaw_rate = %d\n", decoded_item->status->accel_sets->confidence_set->accel_confidence->yaw_rate);
					printf("status->accel_sets->confidence_set->accel_confidence->acceleration = %d\n", decoded_item->status->accel_sets->confidence_set->accel_confidence->acceleration);
					printf("status->accel_sets->confidence_set->accel_confidence->steering_wheel_angle = %d\n", decoded_item->status->accel_sets->confidence_set->accel_confidence->steering_wheel_angle);
				}					
				if(decoded_item->status->accel_sets->confidence_set->mask_speed_confidence == 1)
				{
					printf("status->accel_sets->confidence_set->speed_confidence->heading = %d\n", decoded_item->status->accel_sets->confidence_set->speed_confidence->heading);
					printf("status->accel_sets->confidence_set->speed_confidence->speed = %d\n", decoded_item->status->accel_sets->confidence_set->speed_confidence->speed);
					printf("status->accel_sets->confidence_set->speed_confidence->throttle = %d\n", decoded_item->status->accel_sets->confidence_set->speed_confidence->throttle);
				}
				if(decoded_item->status->accel_sets->confidence_set->mask_time_confidence == 1)
				{
					printf("status->accel_sets->confidence_set->time_confidence = %d\n", decoded_item->status->accel_sets->confidence_set->time_confidence);
				}
				if(decoded_item->status->accel_sets->confidence_set->mask_pos_confidence == 1)
				{
					printf("status->accel_sets->confidence_set->pos_confidence->pos = %d\n", decoded_item->status->accel_sets->confidence_set->pos_confidence->pos);
					printf("status->accel_sets->confidence_set->pos_confidence->elevation = %d\n", decoded_item->status->accel_sets->confidence_set->pos_confidence->elevation);
				}					
				if(decoded_item->status->accel_sets->confidence_set->mask_steer_confidence == 1)
				{	
					printf("status->accel_sets->confidence_set->steer_confidence = %d\n", decoded_item->status->accel_sets->confidence_set->steer_confidence);
				}
				if(decoded_item->status->accel_sets->confidence_set->mask_throttle_confidence == 1)
				{	
					printf("status->accel_sets->confidence_set->throttle_confidence = %d\n", decoded_item->status->accel_sets->confidence_set->throttle_confidence);
				}				
			}
		}
		/*(4-13)***object****/
		if(decoded_item->status->mask_object == 1)
		{
			printf("status->object->ob_dist = %d\n", decoded_item->status->object->ob_dist);
			printf("status->object->ob_direct = %d\n", decoded_item->status->object->ob_direct);
			/*(4-13)***year****/
			if(decoded_item->status->object->date_time->mask_year == 1)
			{
				printf("status->object->date_time->year = %d\n", decoded_item->status->object->date_time->year);
			}
			/*(4-13)***month****/
			if(decoded_item->status->object->date_time->mask_month == 1)
			{
				printf("status->object->date_time->month = %d\n", decoded_item->status->object->date_time->month);
			}
			/*(4-13)***day****/
			if(decoded_item->status->object->date_time->mask_day == 1)
			{
				printf("status->object->date_time->day = %d\n", decoded_item->status->object->date_time->day);
			}
			/*(4-13)***hour****/
			if(decoded_item->status->object->date_time->mask_hour == 1)
			{
				printf("status->object->date_time->hour = %d\n", decoded_item->status->object->date_time->hour);
			}
			/*(4-13)***minute****/
			if(decoded_item->status->object->date_time->mask_minute == 1)
			{
				printf("status->object->date_time->minute = %d\n", decoded_item->status->object->date_time->minute);
			}
			/*(4-13)***second****/
			if(decoded_item->status->object->date_time->mask_second == 1)
			{
				printf("status->object->date_time->second = %d\n", decoded_item->status->object->date_time->second);
			}
		}			
		/*(4-14)***fullPos****/
		if(decoded_item->status->mask_full_pos == 1)
		{
			/*(4-14-1)***utcTime****/
			if(decoded_item->status->full_pos->mask_utc_time == 1)
			{
				/*(4-14-1-1)***year****/
				if(decoded_item->status->full_pos->utc_time->mask_year == 1)
				{
					printf("status->full_pos->utc_time->year = %d\n", decoded_item->status->full_pos->utc_time->year);
				}
				/*(4-14-1-2)***month****/
				if(decoded_item->status->full_pos->utc_time->mask_month == 1)
				{
					printf("status->full_pos->utc_time->month = %d\n", decoded_item->status->full_pos->utc_time->month);
				}
				/*(4-14-1-3)***day****/
				if(decoded_item->status->full_pos->utc_time->mask_day == 1)
				{
					printf("status->full_pos->utc_time->day = %d\n", decoded_item->status->full_pos->utc_time->day);
				}
				/*(4-14-1-4)***hour****/
				if(decoded_item->status->full_pos->utc_time->mask_hour == 1)
				{
					printf("status->full_pos->utc_time->hour = %d\n", decoded_item->status->full_pos->utc_time->hour);
				}
				/*(4-14-1-5)***minute****/
				if(decoded_item->status->full_pos->utc_time->mask_minute == 1)
				{
					printf("status->full_pos->utc_time->minute = %d\n", decoded_item->status->full_pos->utc_time->minute);
				}
				/*(4-14-1-6)***second****/
				if(decoded_item->status->full_pos->utc_time->mask_second == 1)
				{
					printf("status->full_pos->utc_time->second = %d\n", decoded_item->status->full_pos->utc_time->second);
				}				
			}
			/*(4-14-2)***longitude****/
			printf("status->full_pos->longitude = %ld\n", decoded_item->status->full_pos->longitude);
			/*(4-14-3)***lat****/
			printf("status->full_pos->lat = %ld\n", decoded_item->status->full_pos->lat);
			/*(4-14-4)***elevation****/
			if(decoded_item->status->full_pos->mask_elevation == 1)
			{
				printf("status->full_pos->elevation = %d\n", decoded_item->status->full_pos->elevation);
			}			
			/*(4-14-5)***heading****/
			if(decoded_item->status->full_pos->mask_heading == 1)
			{
				printf("status->full_pos->heading = %d\n", decoded_item->status->full_pos->heading);
			}
			/*(4-14-6)***speed****/
			if(decoded_item->status->full_pos->mask_speed == 1)
			{
				printf("status->full_pos->speed->speed = %d\n", decoded_item->status->full_pos->speed->speed);
				printf("status->full_pos->speed->transmission_state = %d\n", decoded_item->status->full_pos->speed->transmission_state);
			}				
			/*(4-14-7)***posAccuracy****/
			if(decoded_item->status->full_pos->mask_pos_accuracy == 1)
			{
				printf("status->full_pos->pos_accuracy->semi_major = %d\n", decoded_item->status->full_pos->pos_accuracy->semi_major);
				printf("status->full_pos->pos_accuracy->semi_minor = %d\n", decoded_item->status->full_pos->pos_accuracy->semi_minor);
				printf("status->full_pos->pos_accuracy->semi_major_orient = %hu\n", decoded_item->status->full_pos->pos_accuracy->semi_major_orient);
			}
			/*(4-14-8)***timeConfidence****/
			if(decoded_item->status->full_pos->mask_time_confidence == 1)
			{
				printf("status->full_pos->time_confidence = %d\n", decoded_item->status->full_pos->time_confidence);
			}				
			/*(4-14-9)***posConfidence****/
			if(decoded_item->status->full_pos->mask_pos_confidence == 1)
			{
				printf("status->full_pos->pos_confidence->pos = %d\n", decoded_item->status->full_pos->pos_confidence->pos);
				printf("status->full_pos->pos_confidence->elevation = %d\n", decoded_item->status->full_pos->pos_confidence->elevation);
					}
			/*(4-14-10)***speedConfidence****/
			if(decoded_item->status->full_pos->mask_speed_confidence == 1)
			{
				printf("status->full_pos->speed_confidence->heading = %d\n", decoded_item->status->full_pos->speed_confidence->heading);
				printf("status->full_pos->speed_confidence->speed = %d\n", decoded_item->status->full_pos->speed_confidence->speed);
				printf("status->full_pos->speed_confidence->throttle = %d\n", decoded_item->status->full_pos->speed_confidence->throttle);
			}
		}
		/*(4-15)***throttlePos****/
		if(decoded_item->status->mask_throttle_pos == 1)
		{	
			printf("status->throttle_pos = %d\n", decoded_item->status->throttle_pos);
		}
		/*(4-16)***speedHeadC****/
		if(decoded_item->status->mask_speed_head_c == 1)
		{
			printf("status->speed_head_c->heading = %d\n", decoded_item->status->speed_head_c->heading);
			printf("status->speed_head_c->speed = %d\n", decoded_item->status->speed_head_c->speed);
			printf("status->speed_head_c->throttle = %d\n", decoded_item->status->speed_head_c->throttle);
		}
		/*(4-17)***speedC****/
		if(decoded_item->status->mask_speed_c == 1)
		{
			printf("status->speed_c = %d\n", decoded_item->status->speed_c);
		}
		/*(4-18)***vehicleData****/
		if(decoded_item->status->mask_vehicle_data == 1)
		{
			printf("status->vehicle_data->height = %d\n", decoded_item->status->vehicle_data->height);
			printf("status->vehicle_data->bumpers.frnt = %d\n", decoded_item->status->vehicle_data->bumpers.frnt);
			printf("status->vehicle_data->bumpers.rear = %d\n", decoded_item->status->vehicle_data->bumpers.rear);
			printf("status->vehicle_data->mass = %d\n", decoded_item->status->vehicle_data->mass);
			printf("status->vehicle_data->trailer_weight = %d\n", decoded_item->status->vehicle_data->trailer_weight);
			printf("status->vehicle_data->type = %d\n", decoded_item->status->vehicle_data->type);
		}
		/*(4-19)***vehicleIdent****/
		if(decoded_item->status->mask_vehicle_ident == 1)
		{
			/*(4-19-1)***name****/
			if(decoded_item->status->vehicle_ident->mask_name == 1)
			{
				printf("status->vehicle_ident->name = %s\n", decoded_item->status->vehicle_ident->name);
			}
			/*(4-19-2)***vin****/
			if(decoded_item->status->vehicle_ident->mask_vin == 1)
			{
				printf("status->vehicle_ident->vin = %s\n", decoded_item->status->vehicle_ident->vin);
			}
			/*(4-19-3)***ownerCode****/
			if(decoded_item->status->vehicle_ident->mask_owner_code == 1)
			{
				printf("status->vehicle_ident->owner_code = %s\n", decoded_item->status->vehicle_ident->owner_code);
			}
			/*(4-19-4)***id****/
			if(decoded_item->status->vehicle_ident->mask_id == 1)
			{
				printf("status->vehicle_ident->id = %s\n", decoded_item->status->vehicle_ident->id);
			}
			/*(4-19-5)***vehicleType****/
			if(decoded_item->status->vehicle_ident->mask_vehicle_type == 1)
			{
				printf("status->vehicle_ident->vehicle_type = %d\n", decoded_item->status->vehicle_ident->vehicle_type);
			}
			/*(4-19-6)***vehicleClass****/
			if(decoded_item->status->vehicle_ident->mask_vehicle_class == 1)
			{
				printf("status->vehicle_ident->vehicle_class_choice = %d\n", decoded_item->status->vehicle_ident->vehicle_class_choice);
				if(decoded_item->status->vehicle_ident->vehicle_class_choice == VEHICLE_IDENT__VEHICLE_CLASS_VGROUP)
				{
					printf("status->vehicle_ident->vehicle_class = %d\n", decoded_item->status->vehicle_ident->vehicle_class);
				}else if(decoded_item->status->vehicle_ident->vehicle_class_choice == VEHICLE_IDENT__VEHICLE_CLASS_RGROUP)
				{
					printf("status->vehicle_ident->vehicle_class = %d\n", decoded_item->status->vehicle_ident->vehicle_class);
				}else if(decoded_item->status->vehicle_ident->vehicle_class_choice == VEHICLE_IDENT__VEHICLE_CLASS_REQUIP)
				{
					printf("status->vehicle_ident->vehicle_class = %d\n", decoded_item->status->vehicle_ident->vehicle_class);
				}else{
					printf("status->vehicle_ident->vehicle_class_choice = VEHICLE_IDENT__VEHICLE_CLASS_NOTHING\n");
				}
			}
		}
		/*(4-20)***j1939data****/
		if(decoded_item->status->mask_j1939_data == 1)
		{	
			if(decoded_item->status->j1939_data->mask_tires == 1)
			{	
				printf("status->j1939_data->tires_num = %d\n", decoded_item->status->j1939_data->tires_num);
				for(i = 0; i < decoded_item->status->j1939_data->tires_num; i++)
				{
					if(decoded_item->status->j1939_data->tires[i].mask_location == 1)
					{
						printf("status->j1939_data->tires[%d].location = %d\n", i, decoded_item->status->j1939_data->tires[i].location);
					}
					if(decoded_item->status->j1939_data->tires[i].mask_pressure == 1)
					{
						printf("status->j1939_data->tires[%d].pressure = %d\n", i, decoded_item->status->j1939_data->tires[i].pressure);
					}
					
					if(decoded_item->status->j1939_data->tires[i].mask_temp == 1)
					{
						printf("status->j1939_data->tires[%d].temp = %d\n", i, decoded_item->status->j1939_data->tires[i].temp);
					}
					
					if(decoded_item->status->j1939_data->tires[i].mask_wheel_sensor_status == 1)
					{
						printf("status->j1939_data->tires[%d].wheel_sensor_status = %d\n", i, decoded_item->status->j1939_data->tires[i].wheel_sensor_status);
					}
					
					if(decoded_item->status->j1939_data->tires[i].mask_wheel_end_elect_fault == 1)
					{
						printf("status->j1939_data->tires[%d].wheel_end_elect_fault = %d\n", i, decoded_item->status->j1939_data->tires[i].wheel_end_elect_fault);
					}
					
					if(decoded_item->status->j1939_data->tires[i].mask_leakage_rate == 1)
					{
						printf("status->j1939_data->tires[%d].leakage_rate = %d\n", i, decoded_item->status->j1939_data->tires[i].leakage_rate);
					}						
					if(decoded_item->status->j1939_data->tires[i].mask_detection == 1)
					{
						printf("status->j1939_data->tires[%d].detection = %d\n", i, decoded_item->status->j1939_data->tires[i].detection);
					}					
				}				
			}
			
			if(decoded_item->status->j1939_data->mask_axle == 1)
			{	
				printf("status->j1939_data->axle_num = %d\n", decoded_item->status->j1939_data->axle_num);
				for(i = 0; i < decoded_item->status->j1939_data->axle_num; i++)
				{
					if(decoded_item->status->j1939_data->axle[i].mask_location == 1)
					{
						printf("status->j1939_data->axle[%d].location = %d\n", i, decoded_item->status->j1939_data->axle[i].location);
					}						
					if(decoded_item->status->j1939_data->axle[i].mask_weight == 1)
					{
						printf("status->j1939_data->axle[%d].weight = %d\n", i, decoded_item->status->j1939_data->axle[i].weight);
					}
				}				
			}				
			if(decoded_item->status->j1939_data->mask_trailer_weight == 1)
			{
				printf("status->j1939_data->trailer_weight = %d\n", decoded_item->status->j1939_data->trailer_weight);
			}				
			if(decoded_item->status->j1939_data->mask_cargo_weight == 1)
			{
				printf("status->j1939_data->cargo_weight = %d\n", decoded_item->status->j1939_data->cargo_weight);
			}
			if(decoded_item->status->j1939_data->mask_steering_axle_temperature == 1)
			{
				printf("status->j1939_data->steering_axle_temperature = %d\n", decoded_item->status->j1939_data->steering_axle_temperature);
			}
			if(decoded_item->status->j1939_data->mask_drive_axle_location == 1)
			{
				printf("status->j1939_data->drive_axle_location = %d\n", decoded_item->status->j1939_data->drive_axle_location);
			}
			if(decoded_item->status->j1939_data->mask_drive_axle_lift_air_pressure == 1)
			{
				printf("status->j1939_data->drive_axle_lift_air_pressure = %d\n", decoded_item->status->j1939_data->drive_axle_lift_air_pressure);
			}
			if(decoded_item->status->j1939_data->mask_drive_axle_temperature == 1)
			{
				printf("status->j1939_data->drive_axle_temperature = %d\n", decoded_item->status->j1939_data->drive_axle_temperature);
			}
			if(decoded_item->status->j1939_data->mask_drive_axle_lube_pressure == 1)
			{
				printf("status->j1939_data->drive_axle_lube_pressure = %d\n", decoded_item->status->j1939_data->drive_axle_lube_pressure);
			}
			if(decoded_item->status->j1939_data->mask_steering_axle_lube_pressure == 1)
			{
				printf("status->j1939_data->steering_axle_lube_pressure = %d\n", decoded_item->status->j1939_data->steering_axle_lube_pressure);
			}
		}
		/*(4-21)***weatherReport****/
		if(decoded_item->status->mask_weather_report == 1)
		{	
			printf("status->weather_report->is_raining = %d\n", decoded_item->status->weather_report->is_raining);
			if(decoded_item->status->weather_report->mask_rain_rate == 1)
			{
				printf("status->weather_report->rain_rate = %d\n", decoded_item->status->weather_report->rain_rate);
			}
			if(decoded_item->status->weather_report->mask_precip_situation == 1)
			{
				printf("status->weather_report->precip_situation = %d\n", decoded_item->status->weather_report->precip_situation);
			}
			if(decoded_item->status->weather_report->mask_solar_radiation == 1)
			{	
				printf("status->weather_report->solar_radiation = %d\n", decoded_item->status->weather_report->solar_radiation);
			}
			if(decoded_item->status->weather_report->mask_friction == 1)
			{
				printf("status->weather_report->friction = %d\n", decoded_item->status->weather_report->friction);
			}
		}
		/*(4-22)***gpsStatus****/
		if(decoded_item->status->mask_gps_status == 1)
		{
			printf("status->gps_status = %x\n", decoded_item->status->gps_status);
		}
	}
	printf("===============================\n");
}

int cmd_rxBsm(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	bsm_standard_item *decoded_item = NULL;
	int err_bsm;

	int err;
	TX_TIMER stop_bsm_timer;
	wme_handle_t handle;
	unsigned char channel_number;
	unsigned char radio_number;
	unsigned int psid;
	struct event_message event;
	struct user_info user_info;
	struct wsm_info wsm_info;
	struct in_wsm in_wsm;

	if(argc != 3) { /* In threadx, cmd is not included in argc */
		printf("rxBsm <psid> <radio_number> <channel_number> \n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("-- example --\n");
		printf("rxBsm 123 0 172\n");
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
	stop_bsm = 0;
	tx_timer_create(&stop_bsm_timer, NULL, stop_bsm_func, 0, 5000, 1, TX_AUTO_ACTIVATE);
	while(stop_bsm == 0) {
		err = wme_wsm_recv(&handle, &in_wsm, 1000);
		if(err != 0) {
			printf("wsm_recv = %d\n", err);
			continue;
		}
		printf("wsm_recv success\n");
		printf("wsm data len = %d\n", in_wsm.length);

		decoded_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
		err_bsm = j2735r41_bsm_decode(decoded_item, in_wsm.data, MAX_BSM_BUFF_SIZE, 1);
		if(err_bsm < 0)
		{
			printf("decode error...\n");
			return -1;
		}
    
		if(SHOW_DECODED == 1){_show_bsm_standard_item(decoded_item);}
    
		j2735r41_bsm_free_struct(decoded_item);
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

	tx_timer_delete(&stop_bsm_timer);

	wme_exit(&handle);

	return 0;
}

int cmd_j2735r41_bsm(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */  	
	(void)command;  	
	(void)argv;
	(void)argc;
	(void)cli;

	bsm_standard_item *encode_item = NULL;
	bsm_standard_item *decoded_item = NULL;
	int err_bsm, i, j, loopCnt;  
	size_t size;
	unsigned char *encodeBuf = NULL;
  
	printf("%s", "Start BSM test!\n");	

	loopCnt = 0;
	while(loopCnt < 1)
	{
		encodeBuf = (unsigned char *)j2735_calloc(1, MAX_BSM_BUFF_SIZE);
		encode_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));		
		/*(1-2)***blob1****/
		/*(1-2-1)***msgCnt****/
		encode_item->blob1.msg_cnt = 1;
		/*(1-2-2)***id****/
		memset(encode_item->blob1.id, 0xFF, 4);
		/*(1-2-3)***secMark****/
		encode_item->blob1.sec_mark = 43532;
		/*(1-2-4)***lat****/
		encode_item->blob1.lat = 900000001;
		/*(1-2-5)***Longitude****/
		encode_item->blob1.longitude = 1800000001;
		/*(1-2-6)***elev***/
		encode_item->blob1.elev = 2848;	
		/*(1-2-7)***accuracy***/
		/*(1-2-7-1)***semi_major***/
		encode_item->blob1.accuracy.semi_major = 255;
		/*(1-2-7-2)***semi_minor***/
		encode_item->blob1.accuracy.semi_minor = 145;
		/*(1-2-7-3)***semi_major_orient***/
		encode_item->blob1.accuracy.semi_major_orient = 65535;
		/*(1-2-8)***speed***/
		/*(1-2-8-1)***speed***/
		encode_item->blob1.speed.speed = 8191;
		/*(1-2-8-1)***TransmissionState***/
		encode_item->blob1.speed.transmission_state = 5;
		/*(1-2-9)***heading***/
		encode_item->blob1.heading = 2754;
		/*(1-2-10)***angle***/	
		encode_item->blob1.angle = 127;
		/*(1-2-11)***accelSet***/	
		/*(1-2-11-1)***longitude***/
		encode_item->blob1.accel_set.longitude = -2000;
		/*(1-2-11-2)***lat***/
		encode_item->blob1.accel_set.lat = -125;
		/*(1-2-11-3)***vert***/
		encode_item->blob1.accel_set.vert = -125;
		/*(1-2-11-4)***yaw***/
		encode_item->blob1.accel_set.yaw = -32767;
		/*(1-2-12)***brakes***/
		/*(1-2-12-1)***wheel_brakes***/
		encode_item->blob1.brakes.wheel_brakes = 8;
		/*(1-2-12-2)***wheel_brakes_unavailable***/
		encode_item->blob1.brakes.wheel_brakes_unavailable = 1;
		/*(1-2-12-3)***spare_bit***/
		encode_item->blob1.brakes.spare_bit = 0;
		/*(1-2-12-4)***traction***/
		encode_item->blob1.brakes.traction = 3;
		/*(1-2-12-5)***abs***/
		encode_item->blob1.brakes.abs = 3;
		/*(1-2-12-6)***scs***/
		encode_item->blob1.brakes.scs = 2;
		/*(1-2-12-7)***brake_boost***/
		encode_item->blob1.brakes.brake_boost = 2;
		/*(1-2-12-8)***aux_brakes***/
		encode_item->blob1.brakes.aux_brakes = 3;
		/*(1-2-13)***size***/
		/*(1-2-13-1)***width***/
		encode_item->blob1.size.width = 238;
		/*(1-2-13-2)***length***/
		encode_item->blob1.size.length = 495; 		
		
		/**OPTIONAL**********************************************************/
		/*(1-3)***safety_ext****/
		//encode_item->mask_safety_ext = 0;
		//encode_item->safety_ext = NULL;
		#if 1
		encode_item->mask_safety_ext = 1;
		encode_item->safety_ext = (vehicle_safety_extension_item *)j2735_calloc(1, sizeof(vehicle_safety_extension_item));
		if(!(encode_item->safety_ext))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		encode_item->safety_ext->mask_events = 1;
		encode_item->safety_ext->events = 8192;
		encode_item->safety_ext->mask_path_history = 1;
		encode_item->safety_ext->path_history = (path_history_item *)j2735_calloc(1, sizeof(path_history_item));
		if(!(encode_item->safety_ext->path_history))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-1)***initialPosition****/
		encode_item->safety_ext->path_history->mask_initial_position = 1;
		encode_item->safety_ext->path_history->initial_position = (full_position_vector_item *)j2735_calloc(1, sizeof(full_position_vector_item));
		if(!(encode_item->safety_ext->path_history->initial_position))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-1-1)***initialPosition****/
		encode_item->safety_ext->path_history->initial_position->mask_utc_time = 1;
		encode_item->safety_ext->path_history->initial_position->utc_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));
		if(!(encode_item->safety_ext->path_history->initial_position->utc_time))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-1-1-1)***year****/
		encode_item->safety_ext->path_history->initial_position->utc_time->mask_year = 1;
		encode_item->safety_ext->path_history->initial_position->utc_time->year = 2010;
		
		/*(1-3-1-1-2)***month****/
		encode_item->safety_ext->path_history->initial_position->utc_time->mask_month = 1;
		encode_item->safety_ext->path_history->initial_position->utc_time->month = 10;
		
		/*(1-3-1-1-3)***day****/
		encode_item->safety_ext->path_history->initial_position->utc_time->mask_day = 1;
		encode_item->safety_ext->path_history->initial_position->utc_time->day = 19;
		
		/*(1-3-1-1-4)***hour****/
		encode_item->safety_ext->path_history->initial_position->utc_time->mask_hour = 1;
		encode_item->safety_ext->path_history->initial_position->utc_time->hour = 10;
		
		/*(1-3-1-1-5)***minute****/
		encode_item->safety_ext->path_history->initial_position->utc_time->mask_minute = 1;
		encode_item->safety_ext->path_history->initial_position->utc_time->minute = 18;
		
		/*(1-3-1-1-6)***second****/
		encode_item->safety_ext->path_history->initial_position->utc_time->mask_second = 1;
		encode_item->safety_ext->path_history->initial_position->utc_time->second = 55;
		
		/*(1-3-1-2)***longitude****/
		encode_item->safety_ext->path_history->initial_position->longitude = 12345;
		
		/*(1-3-1-2)***lat****/
		encode_item->safety_ext->path_history->initial_position->lat = 54321;
		
		/*(1-3-1-4)***elevation****/
		encode_item->safety_ext->path_history->initial_position->mask_elevation = 1;
		encode_item->safety_ext->path_history->initial_position->elevation = 61440;
		
		/*(1-3-1-5)***heading****/
		encode_item->safety_ext->path_history->initial_position->mask_heading = 1;
		encode_item->safety_ext->path_history->initial_position->heading = 22558;
		
		/*(1-3-1-6)***speed****/
		encode_item->safety_ext->path_history->initial_position->mask_speed = 1;
		encode_item->safety_ext->path_history->initial_position->speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
		if(!(encode_item->safety_ext->path_history->initial_position->speed))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-1-6-1)***speed****/
		encode_item->safety_ext->path_history->initial_position->speed->speed = 8191;
		
		/*(1-3-1-6)***transmission****/
		encode_item->safety_ext->path_history->initial_position->speed->transmission_state = 5;
		
		/*(1-3-1-7)***pos_accuracy****/
		encode_item->safety_ext->path_history->initial_position->mask_pos_accuracy = 1;
		encode_item->safety_ext->path_history->initial_position->pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));
		if(!(encode_item->safety_ext->path_history->initial_position->pos_accuracy))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-1-7-1)***semi_major****/
		encode_item->safety_ext->path_history->initial_position->pos_accuracy->semi_major = 250;
		
		/*(1-3-1-7-2)***semi_minor****/
		encode_item->safety_ext->path_history->initial_position->pos_accuracy->semi_minor = 249;
		
		/*(1-3-1-7-3)***semi_major_orient****/
		encode_item->safety_ext->path_history->initial_position->pos_accuracy->semi_major_orient = 65533;
		
		
		/*(1-3-1-8)***time_confidence****/
		encode_item->safety_ext->path_history->initial_position->mask_time_confidence = 1;
		encode_item->safety_ext->path_history->initial_position->time_confidence = 20;
		
		/*(1-3-1-9)***pos_confidence****/
		encode_item->safety_ext->path_history->initial_position->mask_pos_confidence = 1;
		encode_item->safety_ext->path_history->initial_position->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));
		if(!(encode_item->safety_ext->path_history->initial_position->pos_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-1-9-1)***pos****/
		encode_item->safety_ext->path_history->initial_position->pos_confidence->pos = 10;
		
		/*(1-3-1-9-2)***elevation****/
		encode_item->safety_ext->path_history->initial_position->pos_confidence->elevation = 15;
		
		/*(1-3-1-10)***speed_confidence****/
		encode_item->safety_ext->path_history->initial_position->mask_speed_confidence = 1;
		encode_item->safety_ext->path_history->initial_position->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
		if(!(encode_item->safety_ext->path_history->initial_position->speed_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-1-10-1)***heading****/
		encode_item->safety_ext->path_history->initial_position->speed_confidence->heading = 5;
		
		/*(1-3-1-10-2)***speed****/
		encode_item->safety_ext->path_history->initial_position->speed_confidence->speed = 4;
		
		/*(1-3-1-10-3)***throttle****/
		encode_item->safety_ext->path_history->initial_position->speed_confidence->throttle = 3;
		
		/*(1-3-2)***currGPSstatus****/
		encode_item->safety_ext->path_history->mask_curr_gps_status = 1;
		encode_item->safety_ext->path_history->curr_gps_status = 255;
		
		/*(1-3-3)***item_cnt****/
		encode_item->safety_ext->path_history->mask_item_cnt = 1;
		encode_item->safety_ext->path_history->item_cnt = 1;
		
		/*(1-3-4)***crumb_data****/
		/* crumb data choice 1 */
		
		encode_item->safety_ext->path_history->crumb_data_choice = CRUMB_DATA_PATH_HISTORY_POINT_SETS_01;

    	encode_item->safety_ext->path_history->path_history_point_sets_01_num = 1;
		encode_item->safety_ext->path_history->path_history_point_sets_01 = (path_history_point_type_01_item *)j2735_calloc(encode_item->safety_ext->path_history->path_history_point_sets_01_num, sizeof(path_history_point_type_01_item));
		if(!(encode_item->safety_ext->path_history->path_history_point_sets_01))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
    
    	for(i = 0; i < encode_item->safety_ext->path_history->path_history_point_sets_01_num; i++)
		{
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].lat_offset = 1000;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].long_offset = 2000;

			encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_elevation_offset = 1;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].elevation_offset = 1000;

			encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_time_offset = 1;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].time_offset = 65000;

			encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_pos_accuracy = 1;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));
			if(!(encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy))
			{
				debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
				return -MEMORY_ALLOCATE_FAIL;		 
			}
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_major = 254;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_minor = 253;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_major_orient = 65530;

			encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_heading = 1;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].heading = 123;

			encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_speed = 1;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
			if(!(encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed))
			{
				debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
				return -MEMORY_ALLOCATE_FAIL;		 
			}
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed->speed = 8191;
			encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed->transmission_state = 5;
		}

		/*(1-3-3)***path_prediction****/
		//encode_item->safety_ext->mask_path_prediction = 0;
		//encode_item->safety_ext->path_prediction = NULL;
		#if 1
		encode_item->safety_ext->mask_path_prediction = 1;
		encode_item->safety_ext->path_prediction = (path_prediction_item *)j2735_calloc(1, sizeof(path_prediction_item));
		if(!(encode_item->safety_ext->path_prediction))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->safety_ext->path_prediction->radius_of_curve = -32767;
		encode_item->safety_ext->path_prediction->confidence = 200;
		#endif
		/*(1-3-4)***the_rtcm****/
		//encode_item->safety_ext->mask_the_rtcm = 0;
		//encode_item->safety_ext->the_rtcm = NULL;
		#if 1
		encode_item->safety_ext->mask_the_rtcm = 1;
		encode_item->safety_ext->the_rtcm = (rtcm_package_item *)j2735_calloc(1, sizeof(rtcm_package_item));
		if(!(encode_item->safety_ext->the_rtcm))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->safety_ext->the_rtcm->mask_anchor_point = 1;
		encode_item->safety_ext->the_rtcm->anchor_point = (full_position_vector_item *)j2735_calloc(1, sizeof(full_position_vector_item));
		if(!(encode_item->safety_ext->the_rtcm->anchor_point))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		
		/*(1-3-4-1-1)***utc_time****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_utc_time = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));	
		if(!(encode_item->safety_ext->the_rtcm->anchor_point->utc_time))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		/*(1-3-4-1-1-1)***year****/
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_year = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->year = 2010;	
		/*(1-3-4-1-1-2)***month****/
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_month = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->month = 10;	
		/*(1-3-4-1-1-3)***day****/
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_day = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->day = 19;	
		/*(1-3-4-1-1-4)***hour****/
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_hour = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->hour = 10;	
		/*(1-3-4-1-1-5)***minute****/
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_minute = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->minute = 18;
		/*(1-3-4-1-1-6)***second****/
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_second = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->utc_time->second = 55;	
		/*(1-3-4-1-2)***longitude****/
		encode_item->safety_ext->the_rtcm->anchor_point->longitude = 12345;	
		/*(1-3-4-1-2)***lat****/
		encode_item->safety_ext->the_rtcm->anchor_point->lat = 54321;	
		/*(1-3-4-1-4)***elevation****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_elevation = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->elevation = -4095;
		/*(1-3-4-1-5)***heading****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_heading = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->heading = 22558;
		/*(1-3-4-1-6)***speed****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_speed = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
		if(!(encode_item->safety_ext->the_rtcm->anchor_point->speed))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		/*(1-3-4-1-6-1)***speed****/
		encode_item->safety_ext->the_rtcm->anchor_point->speed->speed = 888;	
		/*(1-3-4-1-6)***transmission****/
		encode_item->safety_ext->the_rtcm->anchor_point->speed->transmission_state = 5;	
		/*(1-3-4-1-7)***pos_accuracy****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_pos_accuracy = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));	
		if(!(encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		/*(1-3-4-1-7-1)***semi_major****/
		encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major = 127;	
		/*(1-3-4-1-7-2)***semi_minor****/
		encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_minor = 127;	
		/*(1-3-4-1-7-3)***semi_major_orient****/
		encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major_orient = 3005;	
		/*(1-3-4-1-8)***time_confidence****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_time_confidence = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->time_confidence = 20;	
		/*(1-3-4-1-9)***pos_confidence****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_pos_confidence = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));	
		if(!(encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		/*(1-3-4-1-9-1)***pos****/
		encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence->pos = 10;	
		/*(1-3-4-1-9-2)***elevation****/
		encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence->elevation = 15;	
		/*(1-3-4-1-10)***speed_confidence****/
		encode_item->safety_ext->the_rtcm->anchor_point->mask_speed_confidence = 1;
		encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
		if(!(encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		/*(1-3-4-1-10-1)***heading****/
		encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence->heading = 5;	
		/*(1-3-4-1-10-2)***speed****/
		encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence->speed = 4;	
		/*(1-3-4-1-10-3)***throttle****/
		encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence->throttle = 3;
		//
		encode_item->safety_ext->the_rtcm->rtcm_header.status = 7;

		encode_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_x = 8191;
		encode_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_y = 255;
		encode_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_z = 511;
    
	    encode_item->safety_ext->the_rtcm->mask_msg1001 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1001, 0x31, 16);
		encode_item->safety_ext->the_rtcm->msg1001[16] = '\0';
			
		encode_item->safety_ext->the_rtcm->mask_msg1002 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1002, 0x32, 18);
		encode_item->safety_ext->the_rtcm->msg1002[18] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1003 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1003, 0x33, 21);
		encode_item->safety_ext->the_rtcm->msg1003[21] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1004 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1004, 0x34, 24);
		encode_item->safety_ext->the_rtcm->msg1004[24] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1005 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1005, 0x35, 19);
		encode_item->safety_ext->the_rtcm->msg1005[19] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1006 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1006, 0x36, 21);
		encode_item->safety_ext->the_rtcm->msg1006[21] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1007 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1007, 0x37, 5);
		encode_item->safety_ext->the_rtcm->msg1007[5] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1008 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1008, 0x38, 6);
		encode_item->safety_ext->the_rtcm->msg1008[6] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1009 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1009, 0x39, 16);
		encode_item->safety_ext->the_rtcm->msg1009[16] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1010 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1010, 0x41, 18);
		encode_item->safety_ext->the_rtcm->msg1010[18] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1011 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1011, 0x42, 21);
		encode_item->safety_ext->the_rtcm->msg1011[21] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1012 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1012, 0x43, 24);
		encode_item->safety_ext->the_rtcm->msg1012[24] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1013 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1013, 0x44, 13);
		encode_item->safety_ext->the_rtcm->msg1013[13] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1014 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1014, 0x45, 15);
		encode_item->safety_ext->the_rtcm->msg1014[15] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1015 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1015, 0x46, 13);
		encode_item->safety_ext->the_rtcm->msg1015[13] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1016 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1016, 0x47, 14);
		encode_item->safety_ext->the_rtcm->msg1016[14] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1017 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1017, 0x48, 16);
		encode_item->safety_ext->the_rtcm->msg1017[16] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1019 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1019, 0x49, 62);
		encode_item->safety_ext->the_rtcm->msg1019[62] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1020 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1020, 0x4A, 45);
		encode_item->safety_ext->the_rtcm->msg1020[45] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1021 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1021, 0x4B, 62);
		encode_item->safety_ext->the_rtcm->msg1021[62] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1022 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1022, 0x4C, 75);
		encode_item->safety_ext->the_rtcm->msg1022[75] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1023 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1023, 0x4D, 73);
		encode_item->safety_ext->the_rtcm->msg1023[73] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1024 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1024, 0x4E, 74);
		encode_item->safety_ext->the_rtcm->msg1024[74] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1025 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1025, 0x4F, 25);
		encode_item->safety_ext->the_rtcm->msg1025[25] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1026 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1026, 0x50, 30);
		encode_item->safety_ext->the_rtcm->msg1026[30] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1027 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1027, 0x51, 33);
		encode_item->safety_ext->the_rtcm->msg1027[33] = '\0';

		encode_item->safety_ext->the_rtcm->mask_msg1029 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1029, 0x52, 10);
		encode_item->safety_ext->the_rtcm->msg1029[10] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1030 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1030, 0x53, 14);
		encode_item->safety_ext->the_rtcm->msg1030[14] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1031 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1031, 0x54, 15);
		encode_item->safety_ext->the_rtcm->msg1031[15] = '\0';
		
		encode_item->safety_ext->the_rtcm->mask_msg1032 = 1;
		memset(encode_item->safety_ext->the_rtcm->msg1032, 0x55, 20);
		encode_item->safety_ext->the_rtcm->msg1032[20] = '\0';
		#endif

		#endif
    	
		/*(1-4)***status****/
		//encode_item->mask_status = 0;
		//encode_item->status = NULL;
		#if 1
		encode_item->mask_status = 1;
		encode_item->status = (vehicle_status_item *)j2735_calloc(1, sizeof(vehicle_status_item));
		if(!(encode_item->status))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->mask_lights = 1;
		encode_item->status->lights = 256;
		encode_item->status->mask_light_bar = 1;
		encode_item->status->light_bar = 9;
		encode_item->status->mask_wipers = 1;
		encode_item->status->wipers = (wipers_item *)j2735_calloc(1, sizeof(wipers_item ));
		if(!(encode_item->status->wipers))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->wipers->status_front = 127;
		encode_item->status->wipers->rate_front = 127;
		encode_item->status->wipers->mask_status_rear = 1;
		encode_item->status->wipers->status_rear = 127;
		encode_item->status->wipers->mask_rate_rear = 1;
		encode_item->status->wipers->rate_rear = 127;
		encode_item->status->mask_brake_status = 1;
		encode_item->status->brake_status = (brake_system_status_2_item *)j2735_calloc(1, sizeof(brake_system_status_2_item));
		if(!(encode_item->status->brake_status))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->brake_status->wheel_brakes = 8;		
		encode_item->status->brake_status->traction = 3;
		encode_item->status->brake_status->abs = 3;
		encode_item->status->brake_status->scs = 2;
		encode_item->status->brake_status->brake_boost = 2;
		encode_item->status->brake_status->spare_bits = 0;
		
		encode_item->status->mask_brake_pressure = 1;
		encode_item->status->brake_pressure = 15;
		encode_item->status->mask_road_friction = 1;
		encode_item->status->road_friction = 50;
		encode_item->status->mask_sun_data = 1;
		encode_item->status->sun_data = 1000;
		encode_item->status->mask_rain_data = 1;
		encode_item->status->rain_data = 7;
		encode_item->status->mask_air_temp = 1;
		encode_item->status->air_temp = 191;
		encode_item->status->mask_air_pres = 1;
		encode_item->status->air_pres = 255;
		encode_item->status->mask_steering = 1;
		encode_item->status->steering = (steering_item *)j2735_calloc(1, sizeof(steering_item));
		if(!(encode_item->status->steering))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->steering->angle = 127;
		encode_item->status->steering->mask_confidence = 1;
		encode_item->status->steering->confidence = 3;
		encode_item->status->steering->mask_rate = 1;
		encode_item->status->steering->rate = 127;
		encode_item->status->steering->mask_wheels = 1;
		encode_item->status->steering->wheels = 42;
		encode_item->status->mask_accel_sets = 1;
		encode_item->status->accel_sets = (accel_sets_item *)j2735_calloc(1, sizeof(accel_sets_item));
		if(!(encode_item->status->accel_sets))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->accel_sets->mask_accel_4way = 1;
		encode_item->status->accel_sets->accel_4way = (acceleration_set_4way_item *)j2735_calloc(1, sizeof(acceleration_set_4way_item));
		if(!(encode_item->status->accel_sets->accel_4way))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->accel_sets->accel_4way->longitude = 2001;
		encode_item->status->accel_sets->accel_4way->lat = 2001;
		encode_item->status->accel_sets->accel_4way->vert = -3;
		encode_item->status->accel_sets->accel_4way->yaw = 32767 ;
		encode_item->status->accel_sets->mask_vert_accel_thres = 1;
		encode_item->status->accel_sets->vert_accel_thres = (char)8;
		encode_item->status->accel_sets->mask_yaw_rate_con = 1;
		encode_item->status->accel_sets->yaw_rate_con = 7; 
		encode_item->status->accel_sets->mask_hoz_accel_con = 1;
		encode_item->status->accel_sets->hoz_accel_con = 7;
		encode_item->status->accel_sets->mask_confidence_set = 1;
		encode_item->status->accel_sets->confidence_set = (confidence_set_item *)j2735_calloc(1, sizeof(confidence_set_item));
		if(!(encode_item->status->accel_sets->confidence_set))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->accel_sets->confidence_set->mask_accel_confidence = 1;
		encode_item->status->accel_sets->confidence_set->accel_confidence = (accel_steer_yaw_rate_confidence_item *)j2735_calloc(1, sizeof(accel_steer_yaw_rate_confidence_item));
		if(!(encode_item->status->accel_sets->confidence_set->accel_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->accel_sets->confidence_set->accel_confidence->yaw_rate = 7;
		encode_item->status->accel_sets->confidence_set->accel_confidence->acceleration = 7;
		encode_item->status->accel_sets->confidence_set->accel_confidence->steering_wheel_angle = 3;
		encode_item->status->accel_sets->confidence_set->mask_speed_confidence = 1;
		encode_item->status->accel_sets->confidence_set->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
		if(!(encode_item->status->accel_sets->confidence_set->speed_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->accel_sets->confidence_set->speed_confidence->heading = 7;
		encode_item->status->accel_sets->confidence_set->speed_confidence->speed = 7;
		encode_item->status->accel_sets->confidence_set->speed_confidence->throttle = 3;
		encode_item->status->accel_sets->confidence_set->mask_time_confidence = 1;
		encode_item->status->accel_sets->confidence_set->time_confidence = 39;
		encode_item->status->accel_sets->confidence_set->mask_pos_confidence = 1;
		encode_item->status->accel_sets->confidence_set->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));
		if(!(encode_item->status->accel_sets->confidence_set->pos_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->accel_sets->confidence_set->pos_confidence->pos = 15;
		encode_item->status->accel_sets->confidence_set->pos_confidence->elevation = 15;
		encode_item->status->accel_sets->confidence_set->mask_steer_confidence = 1;
		encode_item->status->accel_sets->confidence_set->steer_confidence = 3;
		encode_item->status->accel_sets->confidence_set->mask_throttle_confidence = 1;
		encode_item->status->accel_sets->confidence_set->throttle_confidence = 3;
		encode_item->status->mask_object = 1;
		encode_item->status->object = (object_item *)j2735_calloc(1, sizeof(object_item));
		if(!(encode_item->status->object))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->object->ob_dist = 3;
		encode_item->status->object->ob_direct = 3;
		encode_item->status->object->date_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));
		if(!(encode_item->status->object->date_time))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->object->date_time->mask_year = 1;
		encode_item->status->object->date_time->year = 2010;	
		encode_item->status->object->date_time->mask_month = 1;
		encode_item->status->object->date_time->month = 10;	
		encode_item->status->object->date_time->mask_day = 1;
		encode_item->status->object->date_time->day = 19;	
		encode_item->status->object->date_time->mask_hour = 1;
		encode_item->status->object->date_time->hour = 10;	
		encode_item->status->object->date_time->mask_minute = 1;
		encode_item->status->object->date_time->minute = 18;
		encode_item->status->object->date_time->mask_second = 1;
		encode_item->status->object->date_time->second = 55;	
		encode_item->status->mask_full_pos = 1;
		encode_item->status->full_pos = (full_position_vector_item *)j2735_calloc(1, sizeof(full_position_vector_item));
		if(!(encode_item->status->full_pos))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->full_pos->mask_utc_time = 1;
		encode_item->status->full_pos->utc_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));
		if(!(encode_item->status->full_pos->utc_time))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->full_pos->utc_time->mask_year = 1;
		encode_item->status->full_pos->utc_time->year = 2010;	
		encode_item->status->full_pos->utc_time->mask_month = 1;
		encode_item->status->full_pos->utc_time->month = 10;	
		encode_item->status->full_pos->utc_time->mask_day = 1;
		encode_item->status->full_pos->utc_time->day = 19;	
		encode_item->status->full_pos->utc_time->mask_hour = 1;
		encode_item->status->full_pos->utc_time->hour = 10;	
		encode_item->status->full_pos->utc_time->mask_minute = 1;
		encode_item->status->full_pos->utc_time->minute = 18;
		encode_item->status->full_pos->utc_time->mask_second = 1;
		encode_item->status->full_pos->utc_time->second = 55;	
		encode_item->status->full_pos->longitude = 12345;	
		encode_item->status->full_pos->lat = 54321;	
		encode_item->status->full_pos->mask_elevation = 1;
		encode_item->status->full_pos->elevation = 61439;
		encode_item->status->full_pos->mask_heading = 1;
		encode_item->status->full_pos->heading = 22558;
		encode_item->status->full_pos->mask_speed = 1;
		encode_item->status->full_pos->speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
		if(!(encode_item->status->full_pos->speed))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->full_pos->speed->speed = 888;	
		encode_item->status->full_pos->speed->transmission_state = 5;	
		encode_item->status->full_pos->mask_pos_accuracy = 1;
		encode_item->status->full_pos->pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));	
		if(!(encode_item->status->full_pos->pos_accuracy))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->full_pos->pos_accuracy->semi_major = 120;
		encode_item->status->full_pos->pos_accuracy->semi_minor = 120;
		encode_item->status->full_pos->pos_accuracy->semi_major_orient = 20010;
		encode_item->status->full_pos->mask_time_confidence = 1;
		encode_item->status->full_pos->time_confidence = 20;	
		encode_item->status->full_pos->mask_pos_confidence = 1;
		encode_item->status->full_pos->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));
		if(!(encode_item->status->full_pos->pos_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->full_pos->pos_confidence->pos = 10;	
		encode_item->status->full_pos->pos_confidence->elevation = 15;	
		encode_item->status->full_pos->mask_speed_confidence = 1;
		encode_item->status->full_pos->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
		if(!(encode_item->status->full_pos->speed_confidence))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->full_pos->speed_confidence->heading = 5;	
		encode_item->status->full_pos->speed_confidence->speed = 4;	
		encode_item->status->full_pos->speed_confidence->throttle = 3;

		encode_item->status->mask_throttle_pos = 1;
		encode_item->status->throttle_pos = 100;
		encode_item->status->mask_speed_head_c = 1;
		encode_item->status->speed_head_c = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
		if(!(encode_item->status->speed_head_c))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->speed_head_c->heading = 3;
		encode_item->status->speed_head_c->speed = 3;
		encode_item->status->speed_head_c->throttle = 3;
		encode_item->status->mask_speed_c = 1;
		encode_item->status->speed_c = 7;
		
		encode_item->status->mask_vehicle_data = 1;
		encode_item->status->vehicle_data = (vehicle_data_item *)j2735_calloc(1, sizeof(vehicle_data_item));
		if(!(encode_item->status->vehicle_data))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->vehicle_data->height = 6;

		encode_item->status->vehicle_data->bumpers.frnt = 15;
		encode_item->status->vehicle_data->bumpers.rear = 15;
		encode_item->status->vehicle_data->mass = 100;
		encode_item->status->vehicle_data->trailer_weight = 15;
		encode_item->status->vehicle_data->type = 15;
		
		encode_item->status->mask_vehicle_ident = 1;
		encode_item->status->vehicle_ident = (vehicle_ident_item *)j2735_calloc(1, sizeof(vehicle_ident_item));
		if(!(encode_item->status->vehicle_ident))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->vehicle_ident->mask_name = 1;
		memcpy(encode_item->status->vehicle_ident->name,"kyho",strlen("kyho"));
		encode_item->status->vehicle_ident->name[strlen("kyho")+1] = '\0';
		encode_item->status->vehicle_ident->mask_vin = 1;
		memset(encode_item->status->vehicle_ident->vin, 0xAB, 17);
		encode_item->status->vehicle_ident->mask_owner_code = 1;
		memcpy(encode_item->status->vehicle_ident->owner_code, "kyho123123", strlen("kyho123123"));
		encode_item->status->vehicle_ident->owner_code[strlen("kyho123123")+1] = '\0';
		encode_item->status->vehicle_ident->mask_id = 1;
		memset(encode_item->status->vehicle_ident->id, 0xAB, 4);
		encode_item->status->vehicle_ident->mask_vehicle_type = 1;
		encode_item->status->vehicle_ident->vehicle_type = 15;
		encode_item->status->vehicle_ident->mask_vehicle_class = 1;
		encode_item->status->vehicle_ident->vehicle_class_choice = VEHICLE_IDENT__VEHICLE_CLASS_RGROUP;
		encode_item->status->vehicle_ident->vehicle_class = 9742;
    
    	encode_item->status->mask_j1939_data = 1;
		encode_item->status->j1939_data_num = 1;
		encode_item->status->j1939_data = (j1939_data_item *)j2735_calloc(encode_item->status->j1939_data_num, sizeof(j1939_data_item));
		if(!(encode_item->status->j1939_data))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}

		for(i = 0; i < encode_item->status->j1939_data_num; i++)
		{
			encode_item->status->j1939_data[i].mask_tires = 1;
			encode_item->status->j1939_data[i].tires_num = 2;
			encode_item->status->j1939_data[i].tires = (tires_item *)j2735_calloc(encode_item->status->j1939_data[i].tires_num, sizeof(tires_item));
			if(!(encode_item->status->j1939_data[i].tires))
			{
				debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
				return -MEMORY_ALLOCATE_FAIL;		 
			}
			for(j = 0; j < encode_item->status->j1939_data[i].tires_num; j++)
			{
				encode_item->status->j1939_data[i].tires[j].mask_location = 1;
				encode_item->status->j1939_data[i].tires[j].location = 255;
				encode_item->status->j1939_data[i].tires[j].mask_pressure = 1;
				encode_item->status->j1939_data[i].tires[j].pressure = 1000;
				encode_item->status->j1939_data[i].tires[j].mask_temp = 1;
				encode_item->status->j1939_data[i].tires[j].temp = 65534;
				encode_item->status->j1939_data[i].tires[j].mask_wheel_sensor_status = 1;
				encode_item->status->j1939_data[i].tires[j].wheel_sensor_status = 3;
				encode_item->status->j1939_data[i].tires[j].mask_wheel_end_elect_fault = 1;
				encode_item->status->j1939_data[i].tires[j].wheel_end_elect_fault = 4;
				encode_item->status->j1939_data[i].tires[j].mask_leakage_rate = 1;
				encode_item->status->j1939_data[i].tires[j].leakage_rate = 65535;
				encode_item->status->j1939_data[i].tires[j].mask_detection = 1;
				encode_item->status->j1939_data[i].tires[j].detection = 7;		
			}

			encode_item->status->j1939_data[i].mask_axle = 1;
			encode_item->status->j1939_data[i].axle_num = 2;
			encode_item->status->j1939_data[i].axle = (axle_item *)j2735_calloc(encode_item->status->j1939_data[i].axle_num, sizeof(axle_item));
			if(!(encode_item->status->j1939_data[i].axle))
			{
				debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
				return -MEMORY_ALLOCATE_FAIL;		 
			}

			for(j = 0; j < encode_item->status->j1939_data[i].axle_num; j++)
			{
				encode_item->status->j1939_data[i].axle[j].mask_location = 1;
				encode_item->status->j1939_data[i].axle[j].location = 127;
				encode_item->status->j1939_data[i].axle[j].mask_weight = 1;
				encode_item->status->j1939_data[i].axle[j].weight = 65535;
			}

			encode_item->status->j1939_data[i].mask_trailer_weight = 1;
			encode_item->status->j1939_data[i].trailer_weight = 65534;
			encode_item->status->j1939_data[i].mask_cargo_weight = 1;
			encode_item->status->j1939_data[i].cargo_weight = 65533;
			encode_item->status->j1939_data[i].mask_steering_axle_temperature = 1;
			encode_item->status->j1939_data[i].steering_axle_temperature = 254;
			encode_item->status->j1939_data[i].mask_drive_axle_location = 1;
			encode_item->status->j1939_data[i].drive_axle_location = 253;
			encode_item->status->j1939_data[i].mask_drive_axle_lift_air_pressure = 1;
			encode_item->status->j1939_data[i].drive_axle_lift_air_pressure = 999;
			encode_item->status->j1939_data[i].mask_drive_axle_temperature = 1;
			encode_item->status->j1939_data[i].drive_axle_temperature = 200;
			encode_item->status->j1939_data[i].mask_drive_axle_lube_pressure = 1;
			encode_item->status->j1939_data[i].drive_axle_lube_pressure = 888;
			encode_item->status->j1939_data[i].mask_steering_axle_lube_pressure = 1;
			encode_item->status->j1939_data[i].steering_axle_lube_pressure = 252;
		}
	
		encode_item->status->mask_weather_report = 1;
		encode_item->status->weather_report = (weather_report_item *)j2735_calloc(1, sizeof(weather_report_item));
		if(!(encode_item->status->weather_report))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->status->weather_report->is_raining = 1; 
		encode_item->status->weather_report->mask_rain_rate = 1;
		encode_item->status->weather_report->rain_rate = 65530; 
		encode_item->status->weather_report->mask_precip_situation = 1;
		encode_item->status->weather_report->precip_situation = 15; 
		encode_item->status->weather_report->mask_solar_radiation = 1;
		encode_item->status->weather_report->solar_radiation = 65520; 
		encode_item->status->weather_report->mask_friction = 1;
		encode_item->status->weather_report->friction = 101; 
		encode_item->status->mask_gps_status = 1;
		encode_item->status->gps_status = 3;
		
		#endif
		
		/********************************************************************/
    
	    if((loopCnt % 100) == 0)
			printf("BSM loopCnt = %d\n", loopCnt + 1);

		//if(SHOW_ENCODE == 1){_show_bsm_standard_item(encode_item);}

		err_bsm = j2735r41_bsm_encode(encode_item, encodeBuf, MAX_BSM_BUFF_SIZE, &size, 1);
		if(err_bsm < 0)
    	{		
			printf("bsm encode fail...\n");		
			return -1;	
		}		
		_cmd_bsm_free_struct(encode_item);

		decoded_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
		err_bsm = j2735r41_bsm_decode(decoded_item, encodeBuf, MAX_BSM_BUFF_SIZE, 1);
		if(err_bsm < 0)
		{
			printf("decode error...\n");
			return -1;
		}
    
    	if(SHOW_DECODED == 1){_show_bsm_standard_item(decoded_item);}
		
		j2735r41_bsm_free_struct(decoded_item);
		j2735_free(encodeBuf);
    
		loopCnt++;
    	(void) tx_thread_sleep(10);
	}

	printf("%s", "BSM Test command executed successfully.\n");
		
	return CLI_OK;  	
}

int _encode_txBsmPartI(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	bsm_standard_item *encode_item = NULL;

	encode_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
	encode_item->msg_id = 2;
	/*(1-2)***blob1****/
	/*(1-2-1)***msgCnt****/
	encode_item->blob1.msg_cnt = 1;
	/*(1-2-2)***id****/
	memset(encode_item->blob1.id, 0xFF, 4);
	/*(1-2-3)***secMark****/
	encode_item->blob1.sec_mark = 43532;
	/*(1-2-4)***lat****/
	encode_item->blob1.lat = 900000001;
	/*(1-2-5)***Longitude****/
	encode_item->blob1.longitude = 1800000001;
	/*(1-2-6)***elev***/
	encode_item->blob1.elev = 2848;	
	/*(1-2-7)***accuracy***/
	/*(1-2-7-1)***semi_major***/
	encode_item->blob1.accuracy.semi_major = 255;
	/*(1-2-7-2)***semi_minor***/
	encode_item->blob1.accuracy.semi_minor = 145;
	/*(1-2-7-3)***semi_major_orient***/
	encode_item->blob1.accuracy.semi_major_orient = 65535;
	/*(1-2-8)***speed***/
	/*(1-2-8-1)***speed***/
	encode_item->blob1.speed.speed = 8191;
	/*(1-2-8-1)***TransmissionState***/
	encode_item->blob1.speed.transmission_state = 5;
	/*(1-2-9)***heading***/
	encode_item->blob1.heading = 2754;
	/*(1-2-10)***angle***/	
	encode_item->blob1.angle = 127;
	/*(1-2-11)***accelSet***/	
	/*(1-2-11-1)***longitude***/
	encode_item->blob1.accel_set.longitude = -2000;
	/*(1-2-11-2)***lat***/
	encode_item->blob1.accel_set.lat = -125;
	/*(1-2-11-3)***vert***/
	encode_item->blob1.accel_set.vert = -125;
	/*(1-2-11-4)***yaw***/
	encode_item->blob1.accel_set.yaw = -32767;
	/*(1-2-12)***brakes***/
	/*(1-2-12-1)***wheel_brakes***/
	encode_item->blob1.brakes.wheel_brakes = 8;
	/*(1-2-12-2)***wheel_brakes_unavailable***/
	encode_item->blob1.brakes.wheel_brakes_unavailable = 1;
	/*(1-2-12-3)***spare_bit***/
	encode_item->blob1.brakes.spare_bit = 0;
	/*(1-2-12-4)***traction***/
	encode_item->blob1.brakes.traction = 3;
	/*(1-2-12-5)***abs***/
	encode_item->blob1.brakes.abs = 3;
	/*(1-2-12-6)***scs***/
	encode_item->blob1.brakes.scs = 2;
	/*(1-2-12-7)***brake_boost***/
	encode_item->blob1.brakes.brake_boost = 2;
	/*(1-2-12-8)***aux_brakes***/
	encode_item->blob1.brakes.aux_brakes = 3;
	/*(1-2-13)***size***/
	/*(1-2-13-1)***width***/
	encode_item->blob1.size.width = 238;
	/*(1-2-13-2)***length***/
	encode_item->blob1.size.length = 495;
	
	/**OPTIONAL**********************************************************/
	/*(1-3)***safety_ext****/
	encode_item->mask_safety_ext = 0;
	encode_item->safety_ext = NULL;

	/*(1-4)***status****/
	encode_item->mask_status = 0;
	encode_item->status = NULL;

	if(SHOW_ENCODE == 1)
	{
		printf("\nBSM Encode Data:\n");
		_show_bsm_standard_item(encode_item);
	}
	
	return j2735r41_bsm_encode(encode_item, encodeBuf, input_buf_size, size, 1);
}

int _encode_txBsmPartIISafetyExtEvents(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	bsm_standard_item *encode_item = NULL;

	encode_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
	encode_item->msg_id = 2;
	/*(1-2)***blob1****/
	/*(1-2-1)***msgCnt****/
	encode_item->blob1.msg_cnt = 1;
	/*(1-2-2)***id****/
	memset(encode_item->blob1.id, 0xFF, 4);
	/*(1-2-3)***secMark****/
	encode_item->blob1.sec_mark = 43532;
	/*(1-2-4)***lat****/
	encode_item->blob1.lat = 900000001;
	/*(1-2-5)***Longitude****/
	encode_item->blob1.longitude = 1800000001;
	/*(1-2-6)***elev***/
	encode_item->blob1.elev = 2848;	
	/*(1-2-7)***accuracy***/
	/*(1-2-7-1)***semi_major***/
	encode_item->blob1.accuracy.semi_major = 255;
	/*(1-2-7-2)***semi_minor***/
	encode_item->blob1.accuracy.semi_minor = 145;
	/*(1-2-7-3)***semi_major_orient***/
	encode_item->blob1.accuracy.semi_major_orient = 65535;
	/*(1-2-8)***speed***/
	/*(1-2-8-1)***speed***/
	encode_item->blob1.speed.speed = 8191;
	/*(1-2-8-1)***TransmissionState***/
	encode_item->blob1.speed.transmission_state = 5;
	/*(1-2-9)***heading***/
	encode_item->blob1.heading = 2754;
	/*(1-2-10)***angle***/	
	encode_item->blob1.angle = 127;
	/*(1-2-11)***accelSet***/	
	/*(1-2-11-1)***longitude***/
	encode_item->blob1.accel_set.longitude = -2000;
	/*(1-2-11-2)***lat***/
	encode_item->blob1.accel_set.lat = -125;
	/*(1-2-11-3)***vert***/
	encode_item->blob1.accel_set.vert = -125;
	/*(1-2-11-4)***yaw***/
	encode_item->blob1.accel_set.yaw = -32767;
	/*(1-2-12)***brakes***/
	/*(1-2-12-1)***wheel_brakes***/
	encode_item->blob1.brakes.wheel_brakes = 8;
	/*(1-2-12-2)***wheel_brakes_unavailable***/
	encode_item->blob1.brakes.wheel_brakes_unavailable = 1;
	/*(1-2-12-3)***spare_bit***/
	encode_item->blob1.brakes.spare_bit = 0;
	/*(1-2-12-4)***traction***/
	encode_item->blob1.brakes.traction = 3;
	/*(1-2-12-5)***abs***/
	encode_item->blob1.brakes.abs = 3;
	/*(1-2-12-6)***scs***/
	encode_item->blob1.brakes.scs = 2;
	/*(1-2-12-7)***brake_boost***/
	encode_item->blob1.brakes.brake_boost = 2;
	/*(1-2-12-8)***aux_brakes***/
	encode_item->blob1.brakes.aux_brakes = 3;
	/*(1-2-13)***size***/
	/*(1-2-13-1)***width***/
	encode_item->blob1.size.width = 238;
	/*(1-2-13-2)***length***/
	encode_item->blob1.size.length = 495;

	/**OPTIONAL**********************************************************/
	/*(1-3)***safety_ext****/
	encode_item->mask_safety_ext = 1;
	encode_item->safety_ext = (vehicle_safety_extension_item *)j2735_calloc(1, sizeof(vehicle_safety_extension_item));
	encode_item->safety_ext->mask_events = 1;
	encode_item->safety_ext->events = 8192;
	
	encode_item->safety_ext->mask_path_history = 0;
	encode_item->safety_ext->path_history = NULL;
	
	encode_item->safety_ext->mask_path_prediction = 0;
	encode_item->safety_ext->path_prediction = NULL;
	
	encode_item->safety_ext->mask_the_rtcm = 0;
	encode_item->safety_ext->the_rtcm = NULL;
	
	/*(1-4)***status****/	
	encode_item->mask_status = 0;
	encode_item->status = NULL;
  
  if(SHOW_ENCODE == 1)
	{
		printf("\nBSM Encode Data:\n");
		_show_bsm_standard_item(encode_item);
	}

	return j2735r41_bsm_encode(encode_item, encodeBuf, input_buf_size, size, 1); 	
}

int _encode_txBsmPartIISafetyExtPathHistory(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	int i;
	bsm_standard_item *encode_item = NULL;

	encode_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
	encode_item->msg_id = 2;
	/*(1-2)***blob1****/
	/*(1-2-1)***msgCnt****/
	encode_item->blob1.msg_cnt = 1;
	/*(1-2-2)***id****/
	memset(encode_item->blob1.id, 0xFF, 4);
	/*(1-2-3)***secMark****/
	encode_item->blob1.sec_mark = 43532;
	/*(1-2-4)***lat****/
	encode_item->blob1.lat = 900000001;
	/*(1-2-5)***Longitude****/
	encode_item->blob1.longitude = 1800000001;
	/*(1-2-6)***elev***/
	encode_item->blob1.elev = 2848;	
	/*(1-2-7)***accuracy***/
	/*(1-2-7-1)***semi_major***/
	encode_item->blob1.accuracy.semi_major = 255;
	/*(1-2-7-2)***semi_minor***/
	encode_item->blob1.accuracy.semi_minor = 145;
	/*(1-2-7-3)***semi_major_orient***/
	encode_item->blob1.accuracy.semi_major_orient = 65535;
	/*(1-2-8)***speed***/
	/*(1-2-8-1)***speed***/
	encode_item->blob1.speed.speed = 8191;
	/*(1-2-8-1)***TransmissionState***/
	encode_item->blob1.speed.transmission_state = 5;
	/*(1-2-9)***heading***/
	encode_item->blob1.heading = 2754;
	/*(1-2-10)***angle***/	
	encode_item->blob1.angle = 127;
	/*(1-2-11)***accelSet***/	
	/*(1-2-11-1)***longitude***/
	encode_item->blob1.accel_set.longitude = -2000;
	/*(1-2-11-2)***lat***/
	encode_item->blob1.accel_set.lat = -125;
	/*(1-2-11-3)***vert***/
	encode_item->blob1.accel_set.vert = -125;
	/*(1-2-11-4)***yaw***/
	encode_item->blob1.accel_set.yaw = -32767;
	/*(1-2-12)***brakes***/
	/*(1-2-12-1)***wheel_brakes***/
	encode_item->blob1.brakes.wheel_brakes = 8;
	/*(1-2-12-2)***wheel_brakes_unavailable***/
	encode_item->blob1.brakes.wheel_brakes_unavailable = 1;
	/*(1-2-12-3)***spare_bit***/
	encode_item->blob1.brakes.spare_bit = 0;
	/*(1-2-12-4)***traction***/
	encode_item->blob1.brakes.traction = 3;
	/*(1-2-12-5)***abs***/
	encode_item->blob1.brakes.abs = 3;
	/*(1-2-12-6)***scs***/
	encode_item->blob1.brakes.scs = 2;
	/*(1-2-12-7)***brake_boost***/
	encode_item->blob1.brakes.brake_boost = 2;
	/*(1-2-12-8)***aux_brakes***/
	encode_item->blob1.brakes.aux_brakes = 3;
	/*(1-2-13)***size***/
	/*(1-2-13-1)***width***/
	encode_item->blob1.size.width = 238;
	/*(1-2-13-2)***length***/
	encode_item->blob1.size.length = 495;	
	
	/**OPTIONAL**********************************************************/
	/*(1-3)***safety_ext****/
	encode_item->mask_safety_ext = 1;
	encode_item->safety_ext = (vehicle_safety_extension_item *)j2735_calloc(1, sizeof(vehicle_safety_extension_item));

	/*(1-3-1)***events****/
	encode_item->safety_ext->mask_events = 0;

	/*(1-3-2)***pathHistory****/
	encode_item->safety_ext->mask_path_history = 1;
	encode_item->safety_ext->path_history = (path_history_item *)j2735_calloc(1, sizeof(path_history_item));
	if(!(encode_item->safety_ext->path_history))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-1)***initialPosition****/
	encode_item->safety_ext->path_history->mask_initial_position = 1;
	encode_item->safety_ext->path_history->initial_position = (full_position_vector_item *)j2735_calloc(1, sizeof(full_position_vector_item));
	if(!(encode_item->safety_ext->path_history->initial_position))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-1-1)***initialPosition****/
	encode_item->safety_ext->path_history->initial_position->mask_utc_time = 1;
	encode_item->safety_ext->path_history->initial_position->utc_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));
	if(!(encode_item->safety_ext->path_history->initial_position->utc_time))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-1-1-1)***year****/
	encode_item->safety_ext->path_history->initial_position->utc_time->mask_year = 1;
	encode_item->safety_ext->path_history->initial_position->utc_time->year = 2010;
	
	/*(1-3-1-1-2)***month****/
	encode_item->safety_ext->path_history->initial_position->utc_time->mask_month = 1;
	encode_item->safety_ext->path_history->initial_position->utc_time->month = 10;
	
	/*(1-3-1-1-3)***day****/
	encode_item->safety_ext->path_history->initial_position->utc_time->mask_day = 1;
	encode_item->safety_ext->path_history->initial_position->utc_time->day = 19;
	
	/*(1-3-1-1-4)***hour****/
	encode_item->safety_ext->path_history->initial_position->utc_time->mask_hour = 1;
	encode_item->safety_ext->path_history->initial_position->utc_time->hour = 10;
	
	/*(1-3-1-1-5)***minute****/
	encode_item->safety_ext->path_history->initial_position->utc_time->mask_minute = 1;
	encode_item->safety_ext->path_history->initial_position->utc_time->minute = 18;
	
	/*(1-3-1-1-6)***second****/
	encode_item->safety_ext->path_history->initial_position->utc_time->mask_second = 1;
	encode_item->safety_ext->path_history->initial_position->utc_time->second = 55;
	
	/*(1-3-1-2)***longitude****/
	encode_item->safety_ext->path_history->initial_position->longitude = 12345;
	
	/*(1-3-1-2)***lat****/
	encode_item->safety_ext->path_history->initial_position->lat = 54321;
	
	/*(1-3-1-4)***elevation****/
	encode_item->safety_ext->path_history->initial_position->mask_elevation = 1;
	encode_item->safety_ext->path_history->initial_position->elevation = 61440;
	
	/*(1-3-1-5)***heading****/
	encode_item->safety_ext->path_history->initial_position->mask_heading = 1;
	encode_item->safety_ext->path_history->initial_position->heading = 22558;
	
	/*(1-3-1-6)***speed****/
	encode_item->safety_ext->path_history->initial_position->mask_speed = 1;
	encode_item->safety_ext->path_history->initial_position->speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
	if(!(encode_item->safety_ext->path_history->initial_position->speed))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-1-6-1)***speed****/
	encode_item->safety_ext->path_history->initial_position->speed->speed = 8191;
	
	/*(1-3-1-6)***transmission****/
	encode_item->safety_ext->path_history->initial_position->speed->transmission_state = 5;
	
	/*(1-3-1-7)***pos_accuracy****/
	encode_item->safety_ext->path_history->initial_position->mask_pos_accuracy = 1;
	encode_item->safety_ext->path_history->initial_position->pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));
	if(!(encode_item->safety_ext->path_history->initial_position->pos_accuracy))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-1-7-1)***semi_major****/
	encode_item->safety_ext->path_history->initial_position->pos_accuracy->semi_major = 250;
	
	/*(1-3-1-7-2)***semi_minor****/
	encode_item->safety_ext->path_history->initial_position->pos_accuracy->semi_minor = 249;
	
	/*(1-3-1-7-3)***semi_major_orient****/
	encode_item->safety_ext->path_history->initial_position->pos_accuracy->semi_major_orient = 65533;
	
	
	/*(1-3-1-8)***time_confidence****/
	encode_item->safety_ext->path_history->initial_position->mask_time_confidence = 1;
	encode_item->safety_ext->path_history->initial_position->time_confidence = 20;
	
	/*(1-3-1-9)***pos_confidence****/
	encode_item->safety_ext->path_history->initial_position->mask_pos_confidence = 1;
	encode_item->safety_ext->path_history->initial_position->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));
	if(!(encode_item->safety_ext->path_history->initial_position->pos_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-1-9-1)***pos****/
	encode_item->safety_ext->path_history->initial_position->pos_confidence->pos = 10;
	
	/*(1-3-1-9-2)***elevation****/
	encode_item->safety_ext->path_history->initial_position->pos_confidence->elevation = 15;
	
	/*(1-3-1-10)***speed_confidence****/
	encode_item->safety_ext->path_history->initial_position->mask_speed_confidence = 1;
	encode_item->safety_ext->path_history->initial_position->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
	if(!(encode_item->safety_ext->path_history->initial_position->speed_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-1-10-1)***heading****/
	encode_item->safety_ext->path_history->initial_position->speed_confidence->heading = 5;
	
	/*(1-3-1-10-2)***speed****/
	encode_item->safety_ext->path_history->initial_position->speed_confidence->speed = 4;
	
	/*(1-3-1-10-3)***throttle****/
	encode_item->safety_ext->path_history->initial_position->speed_confidence->throttle = 3;
	
	/*(1-3-2)***currGPSstatus****/
	encode_item->safety_ext->path_history->mask_curr_gps_status = 1;
	encode_item->safety_ext->path_history->curr_gps_status = 255;
	
	/*(1-3-3)***item_cnt****/
	encode_item->safety_ext->path_history->mask_item_cnt = 1;
	encode_item->safety_ext->path_history->item_cnt = 1;
	
	/*(1-3-4)***crumb_data****/
	/* crumb data choice 1 */
	
	encode_item->safety_ext->path_history->crumb_data_choice = CRUMB_DATA_PATH_HISTORY_POINT_SETS_01;

	encode_item->safety_ext->path_history->path_history_point_sets_01_num = 1;
	encode_item->safety_ext->path_history->path_history_point_sets_01 = (path_history_point_type_01_item *)j2735_calloc(encode_item->safety_ext->path_history->path_history_point_sets_01_num, sizeof(path_history_point_type_01_item));
	if(!(encode_item->safety_ext->path_history->path_history_point_sets_01))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}

	for(i = 0; i < encode_item->safety_ext->path_history->path_history_point_sets_01_num; i++)
	{
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].lat_offset = 1000;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].long_offset = 2000;

		encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_elevation_offset = 1;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].elevation_offset = 1000;

		encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_time_offset = 1;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].time_offset = 65000;

		encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_pos_accuracy = 1;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));
		if(!(encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_major = 254;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_minor = 253;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].pos_accuracy->semi_major_orient = 65530;

		encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_heading = 1;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].heading = 123;

		encode_item->safety_ext->path_history->path_history_point_sets_01[i].mask_speed = 1;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
		if(!(encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed->speed = 8191;
		encode_item->safety_ext->path_history->path_history_point_sets_01[i].speed->transmission_state = 5;
	}
	
	/*(1-3-3)***pathPrediction****/
	encode_item->safety_ext->mask_path_prediction = 0;
	encode_item->safety_ext->path_prediction = NULL;

	/*(1-3-4)***theRTCM****/
	encode_item->safety_ext->mask_the_rtcm = 0;
	encode_item->safety_ext->the_rtcm = NULL;

	/*(1-4)***status****/	
	encode_item->mask_status = 0;
	encode_item->status = NULL;
  
  if(SHOW_ENCODE == 1)
	{
		printf("\nBSM Encode Data:\n");
		_show_bsm_standard_item(encode_item);
	}

	return j2735r41_bsm_encode(encode_item, encodeBuf, input_buf_size, size, 1);	
}

int _encode_txBsmPartIISafetyExtPathPrediction(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	bsm_standard_item *encode_item = NULL;

	encode_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
	encode_item->msg_id = 2;
	/*(1-2)***blob1****/
	/*(1-2-1)***msgCnt****/
	encode_item->blob1.msg_cnt = 1;
	/*(1-2-2)***id****/
	memset(encode_item->blob1.id, 0xFF, 4);
	/*(1-2-3)***secMark****/
	encode_item->blob1.sec_mark = 43532;
	/*(1-2-4)***lat****/
	encode_item->blob1.lat = 900000001;
	/*(1-2-5)***Longitude****/
	encode_item->blob1.longitude = 1800000001;
	/*(1-2-6)***elev***/
	encode_item->blob1.elev = 2848;	
	/*(1-2-7)***accuracy***/
	/*(1-2-7-1)***semi_major***/
	encode_item->blob1.accuracy.semi_major = 255;
	/*(1-2-7-2)***semi_minor***/
	encode_item->blob1.accuracy.semi_minor = 145;
	/*(1-2-7-3)***semi_major_orient***/
	encode_item->blob1.accuracy.semi_major_orient = 65535;
	/*(1-2-8)***speed***/
	/*(1-2-8-1)***speed***/
	encode_item->blob1.speed.speed = 8191;
	/*(1-2-8-1)***TransmissionState***/
	encode_item->blob1.speed.transmission_state = 5;
	/*(1-2-9)***heading***/
	encode_item->blob1.heading = 2754;
	/*(1-2-10)***angle***/	
	encode_item->blob1.angle = 127;
	/*(1-2-11)***accelSet***/	
	/*(1-2-11-1)***longitude***/
	encode_item->blob1.accel_set.longitude = -2000;
	/*(1-2-11-2)***lat***/
	encode_item->blob1.accel_set.lat = -125;
	/*(1-2-11-3)***vert***/
	encode_item->blob1.accel_set.vert = -125;
	/*(1-2-11-4)***yaw***/
	encode_item->blob1.accel_set.yaw = -32767;
	/*(1-2-12)***brakes***/
	/*(1-2-12-1)***wheel_brakes***/
	encode_item->blob1.brakes.wheel_brakes = 8;
	/*(1-2-12-2)***wheel_brakes_unavailable***/
	encode_item->blob1.brakes.wheel_brakes_unavailable = 1;
	/*(1-2-12-3)***spare_bit***/
	encode_item->blob1.brakes.spare_bit = 0;
	/*(1-2-12-4)***traction***/
	encode_item->blob1.brakes.traction = 3;
	/*(1-2-12-5)***abs***/
	encode_item->blob1.brakes.abs = 3;
	/*(1-2-12-6)***scs***/
	encode_item->blob1.brakes.scs = 2;
	/*(1-2-12-7)***brake_boost***/
	encode_item->blob1.brakes.brake_boost = 2;
	/*(1-2-12-8)***aux_brakes***/
	encode_item->blob1.brakes.aux_brakes = 3;
	/*(1-2-13)***size***/
	/*(1-2-13-1)***width***/
	encode_item->blob1.size.width = 238;
	/*(1-2-13-2)***length***/
	encode_item->blob1.size.length = 495;	
	
	/**OPTIONAL**********************************************************/
	/*(1-3)***safety_ext****/
	encode_item->mask_safety_ext = 1;
	encode_item->safety_ext = (vehicle_safety_extension_item *)j2735_calloc(1, sizeof(vehicle_safety_extension_item));
	/*(1-3-1)***events****/
	encode_item->safety_ext->mask_events = 0;

	/*(1-3-2)***pathHistory****/
	encode_item->safety_ext->mask_path_history = 0;
	encode_item->safety_ext->path_history = NULL;
	
	/*(1-3-3)***pathPrediction****/
	encode_item->safety_ext->mask_path_prediction = 1;
	encode_item->safety_ext->path_prediction = (path_prediction_item *)j2735_calloc(1, sizeof(path_prediction_item));
	encode_item->safety_ext->path_prediction->radius_of_curve = -32767;
	encode_item->safety_ext->path_prediction->confidence = 200;

	/*(1-3-4)***theRTCM****/
	encode_item->safety_ext->mask_the_rtcm = 0;
	encode_item->safety_ext->the_rtcm = NULL;

	/*(1-4)***status****/	
	encode_item->mask_status = 0;
	encode_item->status = NULL;
  
  if(SHOW_ENCODE == 1)
	{
		printf("\nBSM Encode Data:\n");
		_show_bsm_standard_item(encode_item);
	}

	return j2735r41_bsm_encode(encode_item, encodeBuf, input_buf_size, size, 1);	
}

int _encode_txBsmPartIISafetyExtRtcm(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	bsm_standard_item *encode_item = NULL;

	encode_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
	encode_item->msg_id = 2;
	/*(1-2)***blob1****/
	/*(1-2-1)***msgCnt****/
	encode_item->blob1.msg_cnt = 1;
	/*(1-2-2)***id****/
	memset(encode_item->blob1.id, 0xFF, 4);
	/*(1-2-3)***secMark****/
	encode_item->blob1.sec_mark = 43532;
	/*(1-2-4)***lat****/
	encode_item->blob1.lat = 900000001;
	/*(1-2-5)***Longitude****/
	encode_item->blob1.longitude = 1800000001;
	/*(1-2-6)***elev***/
	encode_item->blob1.elev = 2848;	
	/*(1-2-7)***accuracy***/
	/*(1-2-7-1)***semi_major***/
	encode_item->blob1.accuracy.semi_major = 255;
	/*(1-2-7-2)***semi_minor***/
	encode_item->blob1.accuracy.semi_minor = 145;
	/*(1-2-7-3)***semi_major_orient***/
	encode_item->blob1.accuracy.semi_major_orient = 65535;
	/*(1-2-8)***speed***/
	/*(1-2-8-1)***speed***/
	encode_item->blob1.speed.speed = 8191;
	/*(1-2-8-1)***TransmissionState***/
	encode_item->blob1.speed.transmission_state = 5;
	/*(1-2-9)***heading***/
	encode_item->blob1.heading = 2754;
	/*(1-2-10)***angle***/	
	encode_item->blob1.angle = 127;
	/*(1-2-11)***accelSet***/	
	/*(1-2-11-1)***longitude***/
	encode_item->blob1.accel_set.longitude = -2000;
	/*(1-2-11-2)***lat***/
	encode_item->blob1.accel_set.lat = -125;
	/*(1-2-11-3)***vert***/
	encode_item->blob1.accel_set.vert = -125;
	/*(1-2-11-4)***yaw***/
	encode_item->blob1.accel_set.yaw = -32767;
	/*(1-2-12)***brakes***/
	/*(1-2-12-1)***wheel_brakes***/
	encode_item->blob1.brakes.wheel_brakes = 8;
	/*(1-2-12-2)***wheel_brakes_unavailable***/
	encode_item->blob1.brakes.wheel_brakes_unavailable = 1;
	/*(1-2-12-3)***spare_bit***/
	encode_item->blob1.brakes.spare_bit = 0;
	/*(1-2-12-4)***traction***/
	encode_item->blob1.brakes.traction = 3;
	/*(1-2-12-5)***abs***/
	encode_item->blob1.brakes.abs = 3;
	/*(1-2-12-6)***scs***/
	encode_item->blob1.brakes.scs = 2;
	/*(1-2-12-7)***brake_boost***/
	encode_item->blob1.brakes.brake_boost = 2;
	/*(1-2-12-8)***aux_brakes***/
	encode_item->blob1.brakes.aux_brakes = 3;
	/*(1-2-13)***size***/
	/*(1-2-13-1)***width***/
	encode_item->blob1.size.width = 238;
	/*(1-2-13-2)***length***/
	encode_item->blob1.size.length = 495;
	
	/**OPTIONAL**********************************************************/
	/*(1-3)***safety_ext****/
	encode_item->mask_safety_ext = 1;
	encode_item->safety_ext = (vehicle_safety_extension_item *)j2735_calloc(1, sizeof(vehicle_safety_extension_item));

	/*(1-3-1)***events****/
	encode_item->safety_ext->mask_events = 0;

	/*(1-3-2)***pathHistory****/
	encode_item->safety_ext->mask_path_history = 0;
	encode_item->safety_ext->path_history = NULL;
	
	/*(1-3-3)***pathPrediction****/
	encode_item->safety_ext->mask_path_prediction = 0;
	encode_item->safety_ext->path_prediction = NULL;

	/*(1-3-4)***theRTCM****/
	encode_item->safety_ext->mask_the_rtcm = 1;
	encode_item->safety_ext->the_rtcm = (rtcm_package_item *)j2735_calloc(1, sizeof(rtcm_package_item));
	if(!(encode_item->safety_ext->the_rtcm))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->safety_ext->the_rtcm->mask_anchor_point = 1;
	encode_item->safety_ext->the_rtcm->anchor_point = (full_position_vector_item *)j2735_calloc(1, sizeof(full_position_vector_item));
	if(!(encode_item->safety_ext->the_rtcm->anchor_point))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	
	/*(1-3-4-1-1)***utc_time****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_utc_time = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));	
	if(!(encode_item->safety_ext->the_rtcm->anchor_point->utc_time))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	/*(1-3-4-1-1-1)***year****/
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_year = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->year = 2010;	
	/*(1-3-4-1-1-2)***month****/
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_month = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->month = 10;	
	/*(1-3-4-1-1-3)***day****/
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_day = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->day = 19;	
	/*(1-3-4-1-1-4)***hour****/
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_hour = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->hour = 10;	
	/*(1-3-4-1-1-5)***minute****/
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_minute = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->minute = 18;
	/*(1-3-4-1-1-6)***second****/
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->mask_second = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->utc_time->second = 55;	
	/*(1-3-4-1-2)***longitude****/
	encode_item->safety_ext->the_rtcm->anchor_point->longitude = 12345;	
	/*(1-3-4-1-2)***lat****/
	encode_item->safety_ext->the_rtcm->anchor_point->lat = 54321;	
	/*(1-3-4-1-4)***elevation****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_elevation = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->elevation = -4095;
	/*(1-3-4-1-5)***heading****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_heading = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->heading = 22558;
	/*(1-3-4-1-6)***speed****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_speed = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
	if(!(encode_item->safety_ext->the_rtcm->anchor_point->speed))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	/*(1-3-4-1-6-1)***speed****/
	encode_item->safety_ext->the_rtcm->anchor_point->speed->speed = 888;	
	/*(1-3-4-1-6)***transmission****/
	encode_item->safety_ext->the_rtcm->anchor_point->speed->transmission_state = 5;	
	/*(1-3-4-1-7)***pos_accuracy****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_pos_accuracy = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));	
	if(!(encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	/*(1-3-4-1-7-1)***semi_major****/
	encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major = 127;	
	/*(1-3-4-1-7-2)***semi_minor****/
	encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_minor = 127;	
	/*(1-3-4-1-7-3)***semi_major_orient****/
	encode_item->safety_ext->the_rtcm->anchor_point->pos_accuracy->semi_major_orient = 3005;	
	/*(1-3-4-1-8)***time_confidence****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_time_confidence = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->time_confidence = 20;	
	/*(1-3-4-1-9)***pos_confidence****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_pos_confidence = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));	
	if(!(encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	/*(1-3-4-1-9-1)***pos****/
	encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence->pos = 10;	
	/*(1-3-4-1-9-2)***elevation****/
	encode_item->safety_ext->the_rtcm->anchor_point->pos_confidence->elevation = 15;	
	/*(1-3-4-1-10)***speed_confidence****/
	encode_item->safety_ext->the_rtcm->anchor_point->mask_speed_confidence = 1;
	encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
	if(!(encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	/*(1-3-4-1-10-1)***heading****/
	encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence->heading = 5;	
	/*(1-3-4-1-10-2)***speed****/
	encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence->speed = 4;	
	/*(1-3-4-1-10-3)***throttle****/
	encode_item->safety_ext->the_rtcm->anchor_point->speed_confidence->throttle = 3;
	//
	encode_item->safety_ext->the_rtcm->rtcm_header.status = 7;

	encode_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_x = 8191;
	encode_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_y = 255;
	encode_item->safety_ext->the_rtcm->rtcm_header.offset_set.ant_offset_z = 511;

    encode_item->safety_ext->the_rtcm->mask_msg1001 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1001, 0x31, 16);
	encode_item->safety_ext->the_rtcm->msg1001[16] = '\0';
		
	encode_item->safety_ext->the_rtcm->mask_msg1002 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1002, 0x32, 18);
	encode_item->safety_ext->the_rtcm->msg1002[18] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1003 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1003, 0x33, 21);
	encode_item->safety_ext->the_rtcm->msg1003[21] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1004 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1004, 0x34, 24);
	encode_item->safety_ext->the_rtcm->msg1004[24] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1005 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1005, 0x35, 19);
	encode_item->safety_ext->the_rtcm->msg1005[19] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1006 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1006, 0x36, 21);
	encode_item->safety_ext->the_rtcm->msg1006[21] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1007 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1007, 0x37, 5);
	encode_item->safety_ext->the_rtcm->msg1007[5] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1008 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1008, 0x38, 6);
	encode_item->safety_ext->the_rtcm->msg1008[6] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1009 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1009, 0x39, 16);
	encode_item->safety_ext->the_rtcm->msg1009[16] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1010 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1010, 0x41, 18);
	encode_item->safety_ext->the_rtcm->msg1010[18] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1011 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1011, 0x42, 21);
	encode_item->safety_ext->the_rtcm->msg1011[21] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1012 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1012, 0x43, 24);
	encode_item->safety_ext->the_rtcm->msg1012[24] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1013 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1013, 0x44, 13);
	encode_item->safety_ext->the_rtcm->msg1013[13] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1014 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1014, 0x45, 15);
	encode_item->safety_ext->the_rtcm->msg1014[15] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1015 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1015, 0x46, 13);
	encode_item->safety_ext->the_rtcm->msg1015[13] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1016 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1016, 0x47, 14);
	encode_item->safety_ext->the_rtcm->msg1016[14] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1017 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1017, 0x48, 16);
	encode_item->safety_ext->the_rtcm->msg1017[16] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1019 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1019, 0x49, 62);
	encode_item->safety_ext->the_rtcm->msg1019[62] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1020 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1020, 0x4A, 45);
	encode_item->safety_ext->the_rtcm->msg1020[45] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1021 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1021, 0x4B, 62);
	encode_item->safety_ext->the_rtcm->msg1021[62] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1022 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1022, 0x4C, 75);
	encode_item->safety_ext->the_rtcm->msg1022[75] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1023 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1023, 0x4D, 73);
	encode_item->safety_ext->the_rtcm->msg1023[73] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1024 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1024, 0x4E, 74);
	encode_item->safety_ext->the_rtcm->msg1024[74] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1025 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1025, 0x4F, 25);
	encode_item->safety_ext->the_rtcm->msg1025[25] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1026 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1026, 0x50, 30);
	encode_item->safety_ext->the_rtcm->msg1026[30] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1027 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1027, 0x51, 33);
	encode_item->safety_ext->the_rtcm->msg1027[33] = '\0';

	encode_item->safety_ext->the_rtcm->mask_msg1029 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1029, 0x52, 10);
	encode_item->safety_ext->the_rtcm->msg1029[10] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1030 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1030, 0x53, 14);
	encode_item->safety_ext->the_rtcm->msg1030[14] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1031 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1031, 0x54, 15);
	encode_item->safety_ext->the_rtcm->msg1031[15] = '\0';
	
	encode_item->safety_ext->the_rtcm->mask_msg1032 = 1;
	memset(encode_item->safety_ext->the_rtcm->msg1032, 0x55, 20);
	encode_item->safety_ext->the_rtcm->msg1032[20] = '\0';

	/*(1-4)***status****/	
	encode_item->mask_status = 0;
	encode_item->status = NULL;
  
	if(SHOW_ENCODE == 1)
	{
		printf("\nBSM Encode Data:\n");
		_show_bsm_standard_item(encode_item);
	}

	return j2735r41_bsm_encode(encode_item, encodeBuf, input_buf_size, size, 1);	
}

int _encode_txBsmPartIIStatus(unsigned char *encodeBuf, size_t input_buf_size, size_t *size)
{
	int i, j;
	bsm_standard_item *encode_item = NULL;

	encode_item = (bsm_standard_item *)j2735_calloc(1, sizeof(bsm_standard_item));
	encode_item->msg_id = 2;
	/*(1-2)***blob1****/
	/*(1-2-1)***msgCnt****/
	encode_item->blob1.msg_cnt = 1;
	/*(1-2-2)***id****/
	memset(encode_item->blob1.id, 0xFF, 4);
	/*(1-2-3)***secMark****/
	encode_item->blob1.sec_mark = 43532;
	/*(1-2-4)***lat****/
	encode_item->blob1.lat = 900000001;
	/*(1-2-5)***Longitude****/
	encode_item->blob1.longitude = 1800000001;
	/*(1-2-6)***elev***/
	encode_item->blob1.elev = 2848;	
	/*(1-2-7)***accuracy***/
	/*(1-2-7-1)***semi_major***/
	encode_item->blob1.accuracy.semi_major = 255;
	/*(1-2-7-2)***semi_minor***/
	encode_item->blob1.accuracy.semi_minor = 145;
	/*(1-2-7-3)***semi_major_orient***/
	encode_item->blob1.accuracy.semi_major_orient = 65535;
	/*(1-2-8)***speed***/
	/*(1-2-8-1)***speed***/
	encode_item->blob1.speed.speed = 8191;
	/*(1-2-8-1)***TransmissionState***/
	encode_item->blob1.speed.transmission_state = 5;
	/*(1-2-9)***heading***/
	encode_item->blob1.heading = 2754;
	/*(1-2-10)***angle***/	
	encode_item->blob1.angle = 127;
	/*(1-2-11)***accelSet***/	
	/*(1-2-11-1)***longitude***/
	encode_item->blob1.accel_set.longitude = -2000;
	/*(1-2-11-2)***lat***/
	encode_item->blob1.accel_set.lat = -125;
	/*(1-2-11-3)***vert***/
	encode_item->blob1.accel_set.vert = -125;
	/*(1-2-11-4)***yaw***/
	encode_item->blob1.accel_set.yaw = -32767;
	/*(1-2-12)***brakes***/
	/*(1-2-12-1)***wheel_brakes***/
	encode_item->blob1.brakes.wheel_brakes = 8;
	/*(1-2-12-2)***wheel_brakes_unavailable***/
	encode_item->blob1.brakes.wheel_brakes_unavailable = 1;
	/*(1-2-12-3)***spare_bit***/
	encode_item->blob1.brakes.spare_bit = 0;
	/*(1-2-12-4)***traction***/
	encode_item->blob1.brakes.traction = 3;
	/*(1-2-12-5)***abs***/
	encode_item->blob1.brakes.abs = 3;
	/*(1-2-12-6)***scs***/
	encode_item->blob1.brakes.scs = 2;
	/*(1-2-12-7)***brake_boost***/
	encode_item->blob1.brakes.brake_boost = 2;
	/*(1-2-12-8)***aux_brakes***/
	encode_item->blob1.brakes.aux_brakes = 3;
	/*(1-2-13)***size***/
	/*(1-2-13-1)***width***/
	encode_item->blob1.size.width = 238;
	/*(1-2-13-2)***length***/
	encode_item->blob1.size.length = 495;	
	
	/**OPTIONAL**********************************************************/
	/*(1-3)***safety_ext****/
	encode_item->mask_safety_ext = 0;
	encode_item->safety_ext = NULL;

	/*(1-4)***status****/	
	encode_item->mask_status = 1;
	encode_item->status = (vehicle_status_item *)j2735_calloc(1, sizeof(vehicle_status_item));
	if(!(encode_item->status))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->mask_lights = 1;
	encode_item->status->lights = 256;
	encode_item->status->mask_light_bar = 1;
	encode_item->status->light_bar = 9;
	encode_item->status->mask_wipers = 1;
	encode_item->status->wipers = (wipers_item *)j2735_calloc(1, sizeof(wipers_item ));
	if(!(encode_item->status->wipers))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->wipers->status_front = 127;
	encode_item->status->wipers->rate_front = 127;
	encode_item->status->wipers->mask_status_rear = 1;
	encode_item->status->wipers->status_rear = 127;
	encode_item->status->wipers->mask_rate_rear = 1;
	encode_item->status->wipers->rate_rear = 127;
	encode_item->status->mask_brake_status = 1;
	encode_item->status->brake_status = (brake_system_status_2_item *)j2735_calloc(1, sizeof(brake_system_status_2_item));
	if(!(encode_item->status->brake_status))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->brake_status->wheel_brakes = 8;		
	encode_item->status->brake_status->traction = 3;
	encode_item->status->brake_status->abs = 3;
	encode_item->status->brake_status->scs = 2;
	encode_item->status->brake_status->brake_boost = 2;
	encode_item->status->brake_status->spare_bits = 0;
	
	encode_item->status->mask_brake_pressure = 1;
	encode_item->status->brake_pressure = 15;
	encode_item->status->mask_road_friction = 1;
	encode_item->status->road_friction = 50;
	encode_item->status->mask_sun_data = 1;
	encode_item->status->sun_data = 1000;
	encode_item->status->mask_rain_data = 1;
	encode_item->status->rain_data = 7;
	encode_item->status->mask_air_temp = 1;
	encode_item->status->air_temp = 191;
	encode_item->status->mask_air_pres = 1;
	encode_item->status->air_pres = 255;
	encode_item->status->mask_steering = 1;
	encode_item->status->steering = (steering_item *)j2735_calloc(1, sizeof(steering_item));
	if(!(encode_item->status->steering))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->steering->angle = 127;
	encode_item->status->steering->mask_confidence = 1;
	encode_item->status->steering->confidence = 3;
	encode_item->status->steering->mask_rate = 1;
	encode_item->status->steering->rate = 127;
	encode_item->status->steering->mask_wheels = 1;
	encode_item->status->steering->wheels = 42;
	encode_item->status->mask_accel_sets = 1;
	encode_item->status->accel_sets = (accel_sets_item *)j2735_calloc(1, sizeof(accel_sets_item));
	if(!(encode_item->status->accel_sets))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->accel_sets->mask_accel_4way = 1;
	encode_item->status->accel_sets->accel_4way = (acceleration_set_4way_item *)j2735_calloc(1, sizeof(acceleration_set_4way_item));
	if(!(encode_item->status->accel_sets->accel_4way))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->accel_sets->accel_4way->longitude = 2001;
	encode_item->status->accel_sets->accel_4way->lat = 2001;
	encode_item->status->accel_sets->accel_4way->vert = -3;
	encode_item->status->accel_sets->accel_4way->yaw = 32767 ;
	encode_item->status->accel_sets->mask_vert_accel_thres = 1;
	encode_item->status->accel_sets->vert_accel_thres = (char)8;
	encode_item->status->accel_sets->mask_yaw_rate_con = 1;
	encode_item->status->accel_sets->yaw_rate_con = 7; 
	encode_item->status->accel_sets->mask_hoz_accel_con = 1;
	encode_item->status->accel_sets->hoz_accel_con = 7;
	encode_item->status->accel_sets->mask_confidence_set = 1;
	encode_item->status->accel_sets->confidence_set = (confidence_set_item *)j2735_calloc(1, sizeof(confidence_set_item));
	if(!(encode_item->status->accel_sets->confidence_set))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->accel_sets->confidence_set->mask_accel_confidence = 1;
	encode_item->status->accel_sets->confidence_set->accel_confidence = (accel_steer_yaw_rate_confidence_item *)j2735_calloc(1, sizeof(accel_steer_yaw_rate_confidence_item));
	if(!(encode_item->status->accel_sets->confidence_set->accel_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->accel_sets->confidence_set->accel_confidence->yaw_rate = 7;
	encode_item->status->accel_sets->confidence_set->accel_confidence->acceleration = 7;
	encode_item->status->accel_sets->confidence_set->accel_confidence->steering_wheel_angle = 3;
	encode_item->status->accel_sets->confidence_set->mask_speed_confidence = 1;
	encode_item->status->accel_sets->confidence_set->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
	if(!(encode_item->status->accel_sets->confidence_set->speed_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->accel_sets->confidence_set->speed_confidence->heading = 7;
	encode_item->status->accel_sets->confidence_set->speed_confidence->speed = 7;
	encode_item->status->accel_sets->confidence_set->speed_confidence->throttle = 3;
	encode_item->status->accel_sets->confidence_set->mask_time_confidence = 1;
	encode_item->status->accel_sets->confidence_set->time_confidence = 39;
	encode_item->status->accel_sets->confidence_set->mask_pos_confidence = 1;
	encode_item->status->accel_sets->confidence_set->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));
	if(!(encode_item->status->accel_sets->confidence_set->pos_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->accel_sets->confidence_set->pos_confidence->pos = 15;
	encode_item->status->accel_sets->confidence_set->pos_confidence->elevation = 15;
	encode_item->status->accel_sets->confidence_set->mask_steer_confidence = 1;
	encode_item->status->accel_sets->confidence_set->steer_confidence = 3;
	encode_item->status->accel_sets->confidence_set->mask_throttle_confidence = 1;
	encode_item->status->accel_sets->confidence_set->throttle_confidence = 3;
	encode_item->status->mask_object = 1;
	encode_item->status->object = (object_item *)j2735_calloc(1, sizeof(object_item));
	if(!(encode_item->status->object))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->object->ob_dist = 3;
	encode_item->status->object->ob_direct = 3;
	encode_item->status->object->date_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));
	if(!(encode_item->status->object->date_time))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->object->date_time->mask_year = 1;
	encode_item->status->object->date_time->year = 2010;	
	encode_item->status->object->date_time->mask_month = 1;
	encode_item->status->object->date_time->month = 10;	
	encode_item->status->object->date_time->mask_day = 1;
	encode_item->status->object->date_time->day = 19;	
	encode_item->status->object->date_time->mask_hour = 1;
	encode_item->status->object->date_time->hour = 10;	
	encode_item->status->object->date_time->mask_minute = 1;
	encode_item->status->object->date_time->minute = 18;
	encode_item->status->object->date_time->mask_second = 1;
	encode_item->status->object->date_time->second = 55;	
	encode_item->status->mask_full_pos = 1;
	encode_item->status->full_pos = (full_position_vector_item *)j2735_calloc(1, sizeof(full_position_vector_item));
	if(!(encode_item->status->full_pos))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->full_pos->mask_utc_time = 1;
	encode_item->status->full_pos->utc_time = (ddate_time_item *)j2735_calloc(1, sizeof(ddate_time_item));
	if(!(encode_item->status->full_pos->utc_time))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->full_pos->utc_time->mask_year = 1;
	encode_item->status->full_pos->utc_time->year = 2010;	
	encode_item->status->full_pos->utc_time->mask_month = 1;
	encode_item->status->full_pos->utc_time->month = 10;	
	encode_item->status->full_pos->utc_time->mask_day = 1;
	encode_item->status->full_pos->utc_time->day = 19;	
	encode_item->status->full_pos->utc_time->mask_hour = 1;
	encode_item->status->full_pos->utc_time->hour = 10;	
	encode_item->status->full_pos->utc_time->mask_minute = 1;
	encode_item->status->full_pos->utc_time->minute = 18;
	encode_item->status->full_pos->utc_time->mask_second = 1;
	encode_item->status->full_pos->utc_time->second = 55;	
	encode_item->status->full_pos->longitude = 12345;	
	encode_item->status->full_pos->lat = 54321;	
	encode_item->status->full_pos->mask_elevation = 1;
	encode_item->status->full_pos->elevation = 61439;
	encode_item->status->full_pos->mask_heading = 1;
	encode_item->status->full_pos->heading = 22558;
	encode_item->status->full_pos->mask_speed = 1;
	encode_item->status->full_pos->speed = (transmission_speed_item *)j2735_calloc(1, sizeof(transmission_speed_item));
	if(!(encode_item->status->full_pos->speed))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->full_pos->speed->speed = 888;	
	encode_item->status->full_pos->speed->transmission_state = 5;	
	encode_item->status->full_pos->mask_pos_accuracy = 1;
	encode_item->status->full_pos->pos_accuracy = (positional_accuracy_item *)j2735_calloc(1, sizeof(positional_accuracy_item));	
	if(!(encode_item->status->full_pos->pos_accuracy))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->full_pos->pos_accuracy->semi_major = 120;
	encode_item->status->full_pos->pos_accuracy->semi_minor = 120;
	encode_item->status->full_pos->pos_accuracy->semi_major_orient = 20010;
	encode_item->status->full_pos->mask_time_confidence = 1;
	encode_item->status->full_pos->time_confidence = 20;	
	encode_item->status->full_pos->mask_pos_confidence = 1;
	encode_item->status->full_pos->pos_confidence = (position_confidence_set_item *)j2735_calloc(1, sizeof(position_confidence_set_item));
	if(!(encode_item->status->full_pos->pos_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->full_pos->pos_confidence->pos = 10;	
	encode_item->status->full_pos->pos_confidence->elevation = 15;	
	encode_item->status->full_pos->mask_speed_confidence = 1;
	encode_item->status->full_pos->speed_confidence = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
	if(!(encode_item->status->full_pos->speed_confidence))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->full_pos->speed_confidence->heading = 5;	
	encode_item->status->full_pos->speed_confidence->speed = 4;	
	encode_item->status->full_pos->speed_confidence->throttle = 3;

	encode_item->status->mask_throttle_pos = 1;
	encode_item->status->throttle_pos = 100;
	encode_item->status->mask_speed_head_c = 1;
	encode_item->status->speed_head_c = (speed_heading_throttle_confidence_item *)j2735_calloc(1, sizeof(speed_heading_throttle_confidence_item));
	if(!(encode_item->status->speed_head_c))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->speed_head_c->heading = 3;
	encode_item->status->speed_head_c->speed = 3;
	encode_item->status->speed_head_c->throttle = 3;
	encode_item->status->mask_speed_c = 1;
	encode_item->status->speed_c = 7;
	
	encode_item->status->mask_vehicle_data = 1;
	encode_item->status->vehicle_data = (vehicle_data_item *)j2735_calloc(1, sizeof(vehicle_data_item));
	if(!(encode_item->status->vehicle_data))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->vehicle_data->height = 6;

	encode_item->status->vehicle_data->bumpers.frnt = 15;
	encode_item->status->vehicle_data->bumpers.rear = 15;
	encode_item->status->vehicle_data->mass = 100;
	encode_item->status->vehicle_data->trailer_weight = 15;
	encode_item->status->vehicle_data->type = 15;
	
	encode_item->status->mask_vehicle_ident = 1;
	encode_item->status->vehicle_ident = (vehicle_ident_item *)j2735_calloc(1, sizeof(vehicle_ident_item));
	if(!(encode_item->status->vehicle_ident))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->vehicle_ident->mask_name = 1;
	memcpy(encode_item->status->vehicle_ident->name,"kyho",strlen("kyho"));
	encode_item->status->vehicle_ident->name[strlen("kyho")+1] = '\0';
	encode_item->status->vehicle_ident->mask_vin = 1;
	memset(encode_item->status->vehicle_ident->vin, 0xAB, 17);
	encode_item->status->vehicle_ident->mask_owner_code = 1;
	memcpy(encode_item->status->vehicle_ident->owner_code, "kyho123123", strlen("kyho123123"));
	encode_item->status->vehicle_ident->owner_code[strlen("kyho123123")+1] = '\0';
	encode_item->status->vehicle_ident->mask_id = 1;
	memset(encode_item->status->vehicle_ident->id, 0xAB, 4);
	encode_item->status->vehicle_ident->mask_vehicle_type = 1;
	encode_item->status->vehicle_ident->vehicle_type = 15;
	encode_item->status->vehicle_ident->mask_vehicle_class = 1;
	encode_item->status->vehicle_ident->vehicle_class_choice = VEHICLE_IDENT__VEHICLE_CLASS_RGROUP;
	encode_item->status->vehicle_ident->vehicle_class = 9742;

	encode_item->status->mask_j1939_data = 1;
	encode_item->status->j1939_data_num = 1;
	encode_item->status->j1939_data = (j1939_data_item *)j2735_calloc(encode_item->status->j1939_data_num, sizeof(j1939_data_item));
	if(!(encode_item->status->j1939_data))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}

	for(i = 0; i < encode_item->status->j1939_data_num; i++)
	{
		encode_item->status->j1939_data[i].mask_tires = 1;
		encode_item->status->j1939_data[i].tires_num = 2;
		encode_item->status->j1939_data[i].tires = (tires_item *)j2735_calloc(encode_item->status->j1939_data[i].tires_num, sizeof(tires_item));
		if(!(encode_item->status->j1939_data[i].tires))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}
		for(j = 0; j < encode_item->status->j1939_data[i].tires_num; j++)
		{
			encode_item->status->j1939_data[i].tires[j].mask_location = 1;
			encode_item->status->j1939_data[i].tires[j].location = 255;
			encode_item->status->j1939_data[i].tires[j].mask_pressure = 1;
			encode_item->status->j1939_data[i].tires[j].pressure = 1000;
			encode_item->status->j1939_data[i].tires[j].mask_temp = 1;
			encode_item->status->j1939_data[i].tires[j].temp = 65534;
			encode_item->status->j1939_data[i].tires[j].mask_wheel_sensor_status = 1;
			encode_item->status->j1939_data[i].tires[j].wheel_sensor_status = 3;
			encode_item->status->j1939_data[i].tires[j].mask_wheel_end_elect_fault = 1;
			encode_item->status->j1939_data[i].tires[j].wheel_end_elect_fault = 4;
			encode_item->status->j1939_data[i].tires[j].mask_leakage_rate = 1;
			encode_item->status->j1939_data[i].tires[j].leakage_rate = 65535;
			encode_item->status->j1939_data[i].tires[j].mask_detection = 1;
			encode_item->status->j1939_data[i].tires[j].detection = 7;		
		}

		encode_item->status->j1939_data[i].mask_axle = 1;
		encode_item->status->j1939_data[i].axle_num = 2;
		encode_item->status->j1939_data[i].axle = (axle_item *)j2735_calloc(encode_item->status->j1939_data[i].axle_num, sizeof(axle_item));
		if(!(encode_item->status->j1939_data[i].axle))
		{
			debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
			return -MEMORY_ALLOCATE_FAIL;		 
		}

		for(j = 0; j < encode_item->status->j1939_data[i].axle_num; j++)
		{
			encode_item->status->j1939_data[i].axle[j].mask_location = 1;
			encode_item->status->j1939_data[i].axle[j].location = 127;
			encode_item->status->j1939_data[i].axle[j].mask_weight = 1;
			encode_item->status->j1939_data[i].axle[j].weight = 65535;
		}

		encode_item->status->j1939_data[i].mask_trailer_weight = 1;
		encode_item->status->j1939_data[i].trailer_weight = 65534;
		encode_item->status->j1939_data[i].mask_cargo_weight = 1;
		encode_item->status->j1939_data[i].cargo_weight = 65533;
		encode_item->status->j1939_data[i].mask_steering_axle_temperature = 1;
		encode_item->status->j1939_data[i].steering_axle_temperature = 254;
		encode_item->status->j1939_data[i].mask_drive_axle_location = 1;
		encode_item->status->j1939_data[i].drive_axle_location = 253;
		encode_item->status->j1939_data[i].mask_drive_axle_lift_air_pressure = 1;
		encode_item->status->j1939_data[i].drive_axle_lift_air_pressure = 999;
		encode_item->status->j1939_data[i].mask_drive_axle_temperature = 1;
		encode_item->status->j1939_data[i].drive_axle_temperature = 200;
		encode_item->status->j1939_data[i].mask_drive_axle_lube_pressure = 1;
		encode_item->status->j1939_data[i].drive_axle_lube_pressure = 888;
		encode_item->status->j1939_data[i].mask_steering_axle_lube_pressure = 1;
		encode_item->status->j1939_data[i].steering_axle_lube_pressure = 252;
	}

	encode_item->status->mask_weather_report = 1;
	encode_item->status->weather_report = (weather_report_item *)j2735_calloc(1, sizeof(weather_report_item));
	if(!(encode_item->status->weather_report))
	{
		debug_printf("[BSM] Error: MEMORY_ALLOCATE_FAIL\n");
		return -MEMORY_ALLOCATE_FAIL;		 
	}
	encode_item->status->weather_report->is_raining = 1; 
	encode_item->status->weather_report->mask_rain_rate = 1;
	encode_item->status->weather_report->rain_rate = 65530; 
	encode_item->status->weather_report->mask_precip_situation = 1;
	encode_item->status->weather_report->precip_situation = 15; 
	encode_item->status->weather_report->mask_solar_radiation = 1;
	encode_item->status->weather_report->solar_radiation = 65520; 
	encode_item->status->weather_report->mask_friction = 1;
	encode_item->status->weather_report->friction = 101; 
	encode_item->status->mask_gps_status = 1;
	encode_item->status->gps_status = 3;
  
  if(SHOW_ENCODE == 1)
	{
		printf("\nBSM Encode Data:\n");
		_show_bsm_standard_item(encode_item);
	}

	return j2735r41_bsm_encode(encode_item, encodeBuf, input_buf_size, size, 1); 	
}

int cmd_txBsmPartI(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txBsmPartI <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txBsmPartI 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_BSM_BUFF_SIZE);
	_encode_txBsmPartI(encodeBuf, MAX_BSM_BUFF_SIZE, &len);
	
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

	printf("%s", "cmd_txBsmPartI command executed successfully.\n");
	return CLI_OK;
}

int cmd_txBsmPartIISafetyExtEvents(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txBsmPartIISafetyExtEvents <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txBsmPartIISafetyExtEvents 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_BSM_BUFF_SIZE);
	_encode_txBsmPartIISafetyExtEvents(encodeBuf, MAX_BSM_BUFF_SIZE, &len);

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


	printf("%s", "cmd_txBsmPartIISafetyExtEvents command executed successfully.\n");
	return CLI_OK;
}

int cmd_txBsmPartIISafetyExtPathHistory(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txBsmPartIISafetyExtPathHistory <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txBsmPartIISafetyExtPathHistory 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_BSM_BUFF_SIZE);
	_encode_txBsmPartIISafetyExtPathHistory(encodeBuf, MAX_BSM_BUFF_SIZE, &len);

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

	printf("%s", "cmd_txBsmPartIISafetyExtPathHistory command executed successfully.\n");
	return CLI_OK;
}

int cmd_txBsmPartIISafetyExtPathPrediction(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txBsmPartIISafetyExtPathPrediction <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txBsmPartIISafetyExtPathPrediction 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_BSM_BUFF_SIZE);
	_encode_txBsmPartIISafetyExtPathPrediction(encodeBuf, MAX_BSM_BUFF_SIZE, &len);
	
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

	printf("%s", "cmd_txBsmPartIISafetyExtPathPrediction command executed successfully.\n");
	return CLI_OK;
}

int cmd_txBsmPartIISafetyExtRtcm(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txBsmPartIISafetyExtRtcm <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txBsmPartIISafetyExtRtcm 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_BSM_BUFF_SIZE);
	_encode_txBsmPartIISafetyExtRtcm(encodeBuf, MAX_BSM_BUFF_SIZE, &len);

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

	printf("%s", "cmd_txBsmPartIISafetyExtRtcm command executed successfully.\n");
	return CLI_OK;
}

int cmd_txBsmPartIIStatus(cli_instance_t *cli, const char *command, char *argv[], int argc)
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
		printf("txBsmPartIIStatus <psid> <request_type> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <interval> <num>\n");
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
		printf("txBsmPartIIStatus 123 1 172 0 6 18 1 FF:FF:FF:FF:FF:FF 100 10\n");
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

	encodeBuf = (unsigned char *)j2735_calloc(1, MAX_BSM_BUFF_SIZE);
	_encode_txBsmPartIIStatus(encodeBuf, MAX_BSM_BUFF_SIZE, &len);

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

	printf("%s", "cmd_txBsmPartIIStatus command executed successfully.\n");
	return CLI_OK;
}     

