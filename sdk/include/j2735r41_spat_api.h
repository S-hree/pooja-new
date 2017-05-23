#ifndef _SPAT_R41_CONVERTER_H_
#define _SPAT_R41_CONVERTER_H_

#define MAX_SPAT_BUFF_SIZE 2048

#include "j2735r41_common.h"

typedef struct spat_standard_item_str
{
	unsigned char mask_msg_sub_id:1,
			mask_name:1,
			mask_regional:1,
			rest:5;

	unsigned char msg_id;
	unsigned char msg_sub_id; //op, integer (0..255)
	unsigned char name[64]; //op, ia5string (size(1..63))

	unsigned char intersections_num;
	intersection_state_item *intersections; //sequence (size(1..32)) of  intersectionstate
}spat_standard_item;


int j2735r41_spat_encode(spat_standard_item *encode_item, unsigned char encode_buf[], size_t input_buf_size, size_t *output_buf_size, unsigned char print_level);
int j2735r41_spat_decode(spat_standard_item *return_decoded_item, unsigned char decode_buf[], size_t decode_buf_size, unsigned char print_level);
void j2735r41_spat_free_struct(spat_standard_item *free_spat_item);

#endif
