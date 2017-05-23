#ifndef _TIM_R41_CONVERTER_H_
#define _TIM_R41_CONVERTER_H_

#define MAX_TIM_BUFF_SIZE 2048

#include "j2735r41_common.h"

typedef struct position3d_str
{
	unsigned char mask_elevation:1,
		      rest:7;

	long lat; /* Latitude ::= INTEGER (-900000000..900000001) */
	long longitude; /* Longitude ::= INTEGER (-1799999999..1800000001) */
	int elevation; //OP; RANG(-4095..61439); LSB = 0.1 meters; unknown: 61440;
}position3d_item;

typedef struct road_sign_id_str /*msgid_choice:1*/
{
	unsigned char mask_mutcd_code:1,
		      mask_crc:1,
		      rest:6;

	position3d_item position;
	unsigned char view_angle[2];/*octet string (size(2))*/
	unsigned char mutcd_code;/*optional, enumerated:0-6*/		
	unsigned char crc[2];/*optional*/
}road_sign_id_item;  

typedef struct offsets_str
{
	unsigned char mask_z_offset:1,
		      mask_width:1,
		      resr:6;	

	signed short x_offset;/*integer (-32767..32767),cm*/
	signed short y_offset;/*integer (-32767..32767),cm*/
	signed short z_offset;/*optional, integer (-32767..32767),cm*/
	unsigned short width;/*optional, integer (0..32767),cm*/
}offsets_item;

typedef struct shape_point_set_str/*area_choice:1*/
{
	unsigned char mask_anchor:1,
		      mask_lane_width:1,
		      mask_directionality:1,
		      rest:5;	

	position3d_item anchor;/*optional*/
	unsigned short lane_width;/*optional, integer (0..32767),cm*/
	unsigned char directionality;/*optional, enumerated:0-2 */
		
	unsigned char node_list_num;
	offsets_item *node_list;/*1..64*/	
}shape_point_set_item;

typedef struct circle_str /*area_choice:2*/
{
	position3d_item center;
	unsigned char raduis_choice;/*1-3*/
	/*raduischoice:1*/
	unsigned short radius_steps;/*integer (0..32767)*/
	/*raduischoice:2*/
	unsigned short miles;/*integer (1..2000)*/
	/*raduischoice:3*/
	unsigned short km;/*integer (1..5000)*/
}circle_item;

typedef struct region_offsets_str
{
	unsigned char mask_z_offset:1,
		      rest:7;

	signed short x_offset;/*integer (-32767..32767),meter*/ 
	signed short y_offset;/* integer (-32767..32767),meter*/
	signed short z_offset;/* integer (-32767..32767) optional*/
}region_offsets_item;

typedef struct region_point_set_str /*area_choice:3*/
{
	unsigned char mask_anchor:1,
		      rest:1;	

	position3d_item anchor;/*optional*/
	
	unsigned char node_list_num;
	region_offsets_item *node_list; /*sequence of(size(1..64))*/	
}region_point_set_item;

typedef struct valid_region_str
{
	unsigned char mask_extent:1,
		      rest:7;

	unsigned char direction[2];/*octet string (size(2))*/
	unsigned char extent;/*optional, enumerated:0-10,127(forever)*/
	
	unsigned char area_choice; /*1-3*/
	/*areachoice:1*/
	shape_point_set_item *shape_point_set;
	
	/*areachoice:2*/
	circle_item *circle;	
	/*areachoice:3*/
	region_point_set_item *region_point_set;
}valid_region_item;

typedef struct iti_scodes_text__member_str
{
	unsigned char itis_item_choice;/*1-2*/
	/*choice: 1*/
	unsigned int itis;/*integer (0..65565)*/
	/*choice: 2*/
	unsigned char text[500];/*ia5string (size(1..500))*/
}iti_scodes_text__member_item;

typedef struct work_zone__member_str
{
	unsigned char item_choice;/*1-2*/
	/*choice: 1*/
	unsigned int itis;/*integer (0..65565)*/
	/*choice: 2*/
	unsigned char text[16];/*ia5string  (size(1..16))*/
}work_zone__member_item;

typedef struct generic_signage__member_str
{
	unsigned char item_choice;/*1-2*/
	/*choice: 1*/
	unsigned int itis;/*integer (0..65565)*/
	/*choice: 2*/
	unsigned char text[16];/*ia5string  (size(1..16))*/	
}generic_signage__member_item;	

typedef struct speed_limit__member_str
{
	unsigned char item_choice;/*1-2*/
	/*choice: 1*/
	unsigned int itis;/*integer (0..65565)*/
	/*choice: 2*/
	unsigned char text[16];/*ia5string  (size(1..16))*/
		
}speed_limit__member_item;

typedef struct exit_service__member_str
{
	unsigned char item_choice;/*1-2*/
	/*choice: 1*/
	unsigned int itis;/*integer (0..65565)*/
	/*choice: 2*/
	unsigned char text[16];/*ia5string  (size(1..16))*/
}exit_service__member_item;

typedef struct data_frames_str
{
	unsigned char mask_start_year:1,
		      mask_common_anchor:1,
		      mask_common_lane_width:1,
		      mask_common_directionality:1,
		      mask_url:1,
		      rest:3;		

	/*[part i], frame header*/
	unsigned char frame_type;/*enum, 0-3*/
	
	unsigned char msg_id_choice;
	/*msgid_choice:0, octet string (size(2))*/
	unsigned char further_info_id[2];
	/*msgid_choice:1*/	
	road_sign_id_item road_sign_id;
     
	unsigned short start_year; /*optional, integer (0..9999),current year used if missing*/ 
	unsigned int start_time; /*integer (0..525960)*/
	unsigned short duraton_time;/*integer (0..32000)*/
	unsigned char priority; /*integer (0..7)*/

	/*[part ii], applicable regions of use*/	
	position3d_item *common_anchor;/*optional*/
	unsigned short common_lane_width;/*optional, integer (0..32767),cm*/
	unsigned char common_directionality;/*optional, enumerate:0-2*/
	
	unsigned char regions_num;/*regions sequence of(1..16) number*/
	valid_region_item *regions;
	
	/*[part ii], content*/
	unsigned char content_choice;/*1-5*/
	/*contentchoice:1*/
	unsigned char advisory_num;
	iti_scodes_text__member_item *advisory; /*sequence (size(1..100)) of*/
	/*contentchoice:2*/
	unsigned char work_zone_num;
	work_zone__member_item *work_zone; /*sequence (size(1..10)) of*/
	/*contentchoice:3*/
	unsigned char generic_sign_num;
	generic_signage__member_item *generic_sign; /*sequence (size(1..10)) of*/
	/*contentchoice:4*/
	unsigned char speed_limit_num;
	speed_limit__member_item *speed_limit; /*sequence (size(1..10)) of*/
	/*contentchoice:5*/
	unsigned char exit_service_num;
	exit_service__member_item *exit_service; /*sequence (size(1..10)) of*/
			
	unsigned char url[15];/*optional, ia5string (size(1..15))*/
}data_frames_item;

typedef struct offsets_xy_str
{
	signed short x_offset;
	signed short y_offset;
}offsets_xy_item;

typedef struct offsets_xyz_str
{
	signed short x_offset;
	signed short y_offset;
	signed short z_offset;
}offsets_xyz_item;

typedef struct offsets_xyw_str
{
	signed short x_offset;
	signed short y_offset;
	unsigned short width;
}offsets_xyw_item;

typedef struct offsets_xyzw_str
{
	signed short x_offset;
	signed short y_offset;
	signed short z_offset;
	unsigned short width;
}offsets_xyzw_item;


typedef struct tim_standard_item_str
{
	unsigned char mask_packet_id:1,
		      mask_url_b:1,
		      mask_data_frame_count:1,
		      rest:5;

	unsigned char msg_id;		
	unsigned char packet_id[9];/*optional,octet string (size(9))*/    
	unsigned char url_b[46];/* optional,ia5string (size(1..45)) */ 
	unsigned char data_frame_count;/*optional, integer (0..32) */

	unsigned char data_frames_num;
	data_frames_item *data_frames;/*sequence of(size(1..8))*/
	unsigned char crc[2];/*octet string (size(2))*/

}tim_standard_item;


int j2735r41_tim_encode(tim_standard_item *encode_item, unsigned char encode_buf[], size_t input_buf_size, size_t *output_buf_size, unsigned char print_level);
int j2735r41_tim_decode(tim_standard_item *return_decoded_item, unsigned char decode_buf[], size_t decode_buf_size, unsigned char print_level);
void j2735r41_tim_free_struct(tim_standard_item *free_tim_item);

#endif
