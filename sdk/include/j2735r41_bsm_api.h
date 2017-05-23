#ifndef _BSM_R41_CONVERTER_H_

#define _BSM_R41_CONVERTER_H_
#define MAX_BSM_BUFF_SIZE 2048

#include "j2735r41_common.h"

typedef struct bsm_standard_item_str
{
	unsigned char mask_safety_ext:1,
			mask_status:1,
			rest:6;

	/*(1) msgID */
	unsigned char msg_id; // 2

	/*(2) blob1 */
	bsm_blob_item blob1;

	/*(3) safetyExt *OP**/
	vehicle_safety_extension_item *safety_ext; //OP

	/*(4) status *OP**/
	vehicle_status_item *status; //OP
}bsm_standard_item;


int j2735r41_bsm_encode(bsm_standard_item *encode_item, unsigned char encode_buf[], size_t input_buf_size, size_t *output_buf_size, unsigned char print_level);
int j2735r41_bsm_decode(bsm_standard_item *return_decoded_item, unsigned char decode_buf[], size_t decode_buf_size, unsigned char print_level);
void j2735r41_bsm_free_struct(bsm_standard_item *free_bsm_item);

#endif
