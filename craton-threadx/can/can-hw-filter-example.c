/* Copyright (C) 2015 Autotalks Ltd. */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>

#include <atlk/sdk.h>
#include <atlk/can.h>

#include <craton/can_driver.h>

/*
  CRATON ThreadX CAN Driver Example

  This example demonstrates how to configure CAN HW filters via CAN
  driver API.

  In HW RX buffers, CAN ID is represented as follows:

  For standard CAN ID (11 bits ID):
  =================================
  Bit 31  30  29  28  27  26  25  24  23  22  21  20 19       18...0
    +----+---+---+---+---+---+---+---+---+---+---+--+---+---------------+
    |ID10|ID9|ID8|ID7|ID6|ID5|ID4|ID3|ID2|ID1|ID0|DC|IDE|Don't Care (DC)|
    +----+---+---+---+---+---+---+---+---+---+---+--+---+---------------+
  DC stands for "Don't care". These bits are not related to CAN ID.
  IDE stands for "ID Extenstion". This bit is 1 when we receive a frame with
  extended ID.

  For extended CAN ID (29 bits ID):
  =================================
  Bit 31   30   29   28   27   26   25   24   23   22   21  20 19   18   17   16
    +----+----+----+----+----+----+----+----+----+----+----+--+---+--------------+
    |ID28|ID27|ID26|ID25|ID24|ID23|ID22|ID21|ID20|ID19|ID18|DC|IDE|ID17|ID16|ID15|
    +----+----+----+----+----+----+----+----+----+----+----+--+---+--------------+
  Bit 15   14   13   12   11   10  9   8   7   6   5   4   3   2   1   0
    +----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+--+
    |ID14|ID13|ID12|ID11|ID10|ID9|ID8|ID7|ID6|ID5|ID4|ID3|ID2|ID1|ID0|DC|
    +----+----+----+----+----+---+---+---+---+---+---+---+---+---+---+--+
  DC stands for "Don't care". These bits are not related to CAN ID.
  IDE stands for "ID Extenstion". This bit is 1 when we receive a frame with
  extended ID.

  Bits in GMASK work as follows:
  ==============================
  1 means: Don't care about the corresponding bit in RX buffer, always accept.
  0 means: Match that corresponding bit of RX buffer with the bit specified in
           can_hw_buffer_config_t.buffer[<buffer_number>].id: If they don't
           match, drop the frame.

  GMASK works on buffers 0..13 when they are configured to RX.

  Bits in BMASK works the same but for buffer number 14 when it is configured to
  RX.

  This example will demonstrate usage of HW filter on a single RX buffer for
  simplicity. Different HW filters may apply to different RX buffers.

  The example will have the following scenarios:

  Scenario 0: Set global mask to match exact ID, which is 0xA8, of standard
              frames.
  Scenario 1: Set global mask not to care about anything. Accept everything.
              Will accept any standard/extended frame
  Scenario 2: Set global mask to match exact first three bits of extended
              CAN ID. The three bits should be 101b (0x5)
  Scenario 3: Set global mask to match extended CAN IDs only
  Scenario 4: Set global mask to match standard CAN IDs only

  You can choose which scenario to run at compile-time.

  For more information, please refer to the CAN user guide.
*/

/* Helper macros */
#define BIT(N) (1U << (N))
#define BITMASK(N) ((1U << (N)) - 1)

#define IDE_BIT BIT(19)

#ifdef __CRATON_ARM

/*
  Helper function to create HW CAN ID for GMASK, BMASK and HW RX buffer
  CAN ID.
 */
static uint32_t
can_hw_id_from_can_id(uint32_t can_id,
                      int is_extended_id,
                      int is_mask)
{
  uint32_t can_hw_id;
  uint32_t chunk;

  can_hw_id = 0;

  if (is_extended_id) {
    /* These bits are don't care bit in extended frames */
    can_hw_id |= BIT(0) | BIT(20);

    /* Take first chunk of 18 bits of CAN ID. Chunk is at offset of 1 bit */
    chunk = (can_id & BITMASK(18)) << 1;
    if (is_mask) {
      /* If this is GMASK or BMASK, care about these bits (0 means match) */
      chunk = ~chunk;
    }
    else {
      /* Turn extended bit ON in case of buffer CAN ID (is_mask == 0) */
      can_hw_id |= IDE_BIT;
    }

    /* Take relevant bits [18:1] */
    can_hw_id |= (0x7FFFE & chunk);

    /* Take second chunk of 11 bits of CAN ID. Chunk is at offset of 21 bits */
    can_id >>= 18;
    chunk = (can_id & BITMASK(11)) << 21;
    if (is_mask) {
      /* If this is GMASK or BMASK, care about these bits (0 means match) */
      chunk = ~chunk;
    }

    /* Take relevant bits [31:21] */
    can_hw_id |= (0xFFE00000 & chunk);

    return can_hw_id;
  }

  /* Standard frames */

  /* These bits are don't care bit in standard frames */
  can_hw_id |= BITMASK(19) | BIT(20);

  /* Take 11 bits of CAN ID. CAN ID is at offset of 21 bits */
  chunk = (can_id & BITMASK(11)) << 21;
  if (is_mask) {
    /* If this is GMASK or BMASK, care about these bits (0 means match) */
    chunk = ~chunk;
  }

  /* Take relevant bits [31:21] */
  can_hw_id |= (0xFFE00000 & chunk);

  return can_hw_id;
}

/*
   Create a hardware mask where can_id_bits specifies
   which bits are compared, as follows:
   If bit N is 0: Don't care about bit in position N
   If bit N is 1: Match incoming bit in position N
 */
static uint32_t
gmask_can_id_check_mask(uint32_t can_id_bits,
                        int is_extended_id)
{
  return can_hw_id_from_can_id(can_id_bits,
                               is_extended_id,
                               1);
}

/*
   Create a hardware CAN ID where can_id specifies
   the desired CAN ID to be matched.
 */
static uint32_t
can_hw_id_create(uint32_t can_id,
                 int is_extended_id)
{
  return can_hw_id_from_can_id(can_id,
                               is_extended_id,
                               0);
}

void craton_user_init(void)
{
  /* Autotalks return code */
  atlk_rc_t rc = ATLK_OK;
  /* CAN HW buffer configuration */
  can_hw_buffer_config_t config = CAN_HW_BUFFER_CONFIG_INIT;
  /* Scenario to test. Change to any one of the below scenarios */
  int scenario = 0;

  /* Set buffer 0 as RX buffer */
  config.buffers[0].direction = CAN_HW_BUFFER_DIRECTION_RX;

  /* Set buffers 1-14 as TX buffers; there's no need to set ID */
  for (int i = 1; i < 15; ++i) {
    config.buffers[i].direction = CAN_HW_BUFFER_DIRECTION_TX;
  }

  switch(scenario) {
  case 0:
    /* Set global mask to match exact ID of standard frames which is 0xA8 */
    config.gmask = gmask_can_id_check_mask(0x7FF, 0);
    config.buffers[0].id = can_hw_id_create(0xA8, 0);
    break;
  case 1:
    /* Set global mask not to care about anything. Accept everything.
       Will accept any standard/extended frame */
    config.gmask = 0xFFFFFFFF;
    /* buffer ID doesn't play a role here */
    break;
  case 2:
    /* Set global mask to match exact first three bits of extended CAN ID.
       The three bits should be 101b (0x5) */
    config.gmask = gmask_can_id_check_mask(0x7, 1);
    config.buffers[0].id = can_hw_id_create(0x5, 1);
    break;
  case 3:
    /* Set global mask to match extended CAN IDs only */
    config.gmask = gmask_can_id_check_mask(0, 1);
    config.buffers[0].id = can_hw_id_create(0, 1);
    break;
  case 4:
    /* Set global mask to match standard CAN IDs only */
    config.gmask = gmask_can_id_check_mask(0, 0);
    config.buffers[0].id = can_hw_id_create(0, 0);
    break;
  default:
    fprintf(stderr, "Invalid scenario number: %d\n", scenario);
    return;
  }

  /* Set CAN HW buffer configuration for device 0 */
  rc = can_hw_buffer_config_set(0, &config);
  if (atlk_error(rc)) {
    fprintf(stderr, "can_hw_config_set: %s\n", atlk_rc_to_str(rc));
    return;
  }

  printf("CAN HW configuration done.\n");
  printf("GMASK = %"PRIx32" , ID = %"PRIx32"\n",
         config.gmask, config.buffers[0].id);

  return;
}

#else /* __CRATON_ARM */

void craton_user_init(void)
{
}

#endif /* __CRATON_ARM */
