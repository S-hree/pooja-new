#ifndef _MAP_R41_CONVERTER_H_
#define _MAP_R41_CONVERTER_H_

#define MAX_MAP_BUFF_SIZE 2048

/*data_choice*/
#define LANESET_CHOICE 0 
#define ZONES_CHOICE 1

#include "j2735r41_common.h"

typedef struct map_standard_item_str
{	
	unsigned char mask_msg_sub_id:1,	
			mask_layer_type:1,				  
			mask_layer_id:1,	
			mask_intersections:1,				  
			mask_road_segments:1,				  
			mask_data_parameters:1,				  
			mask_restriction_list:1,				  
			mask_regional:1;
	
	unsigned char mask_crc:1,				  
			rest:7;

	/*(1)***msgid****/
	unsigned char msg_id;
	
	/*(2)***msgsubid**op**/
	unsigned char msg_sub_id; //integer (0..255)

	/*(3)***msgissuerevision****/
	unsigned char msg_issue_revision; //integer (0..127)

	/*(4)***layertype**op**/
	unsigned char layer_type; //enumerated {0..7}
                	            
	/*(5)***layerid**op**/
	unsigned char layer_id; //integer (0..100)
							   
	/*(6)***intersections**op**/
	unsigned char intersections_num;
	intersection_geometry_item *intersections; //op, sequence (size(1..32)) of intersectiongeometry

	/*(7)***roadsegments**op**/
	unsigned char road_segments_num;
	road_segment_item *road_segments; //op, sequence (size(1..32)) of roadsegment

	/*(8)***dataparameters**op**/
	data_parameters_item data_parameters; /*op*/	

	/*(9)***restrictionlist**op**/
	unsigned char restriction_list_num;
	restriction_class_assignment_item	*restriction_list; //op, sequence (size(1..254)) of  restrictionclassassignment

	/*(10)***regional**op**/

	/*(11)***crc**op**/
	unsigned char crc[2]; /*op*/
}map_standard_item;


int j2735r41_map_encode(map_standard_item *encode_item, unsigned char encode_buf[], size_t input_buf_size, size_t *output_buf_size, unsigned char print_level);
int j2735r41_map_decode(map_standard_item *return_decoded_item, unsigned char decode_buf[], size_t decode_buf_size, unsigned char print_level);
void j2735r41_map_free_struct(map_standard_item *free_map_item);

#endif
