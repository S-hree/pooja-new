/* Copyright (C) 2014-2015 Autotalks Ltd. */
#include <stdio.h>
#include <string.h>

#include <craton/nor_flash.h>

/*
  OTP - One Time Programmable module

  This example demonstrates basic usage of OTP API for code running
  on top of CRATON processor with ThreadX RTOS.

  The example reads OTP information and writes to OTP only once.
*/

/* Serial string- length should not exceed 127 */
#define SERIAL_STRING_EXAMPLE    "1h7J9#2a%Hk1D6j8Zz1$P2g6"

/* Max Size of serial buffer */
#define SERIAL_SIZE              128

/* check if partition is empty (all bytes == 0xff)  */
static uint8_t part_is_empty(char *part)
{
  uint16_t i;

  for (i=0 ; i < SERIAL_SIZE ; i++){
    if (part[i] != 0xff)
      return 0;
  }

  return 1;
}

/* Example how to read serial string */
static void serial_read_example(void)
{
  /* Partition table */
  norfl_part_table_t part_table = NORFL_PART_TABLE_INIT;
  /* Serial buffer */
  char serial_buff[SERIAL_SIZE];

  uint8_t part_num;

  atlk_rc_t rc = ATLK_OK;

  /* set buffer - verify that it will be zero terminated */
  memset(&serial_buff[0], 0x00, sizeof(serial_buff));

  /* Initialize NOR flash interface */
  rc = norfl_init();
  if (atlk_error(rc)) {
    printf("ERROR: Failed to initialize NOR Flash device, rc=%d\n",rc);
    goto err;
  }

  /* Read partition table */
  rc = norfl_part_table_read(&part_table);
  if (atlk_error(rc)) {
    printf("ERROR: Failed to read partition table, rc=%d\n",rc);
    goto err;
  };

  /* Search for lockable partition */
  for (part_num = 0 ; part_num < NORFL_NUM_PARTS; part_num++) {
    uint32_t part_type = part_table.part_info[part_num].part_type;
    if ((part_type == (NORFL_PART_TYPE_USER_MIN | NORFL_PART_TYPE_F_LOCKABLE)) ||
        (part_type == (NORFL_PART_TYPE_USER_MIN | NORFL_PART_TYPE_F_LOCKED))) {
      break;
    }
  }

  /* validate part_num value */
  if (part_num == NORFL_NUM_PARTS) {
    printf("ERROR: Didn't Find Lockable/Locked Partition, rc=%d\n",rc);
    goto err;
  }

  /* read serial from partition */
  rc = norfl_part_read(part_num,
                       0,
                       &serial_buff[0],
                       sizeof(serial_buff));
  if (atlk_error(rc)) {
    printf("ERROR: Failed to read, rc=%d\n",rc);
    goto err;
  };

  if (!part_is_empty(serial_buff)) {
    printf("Serial string: %s\n", serial_buff);
  }
  else {
    printf("Serial Is empty\n");
  }

err:
  return;

}

/* Example how to write serial string */
static void serial_write_example(void)
{
  /* Partition table */
  norfl_part_table_t part_table = NORFL_PART_TABLE_INIT;
  /* Serial buffer */
  char serial_buff[SERIAL_SIZE];

  uint8_t part_num;

  atlk_rc_t rc = ATLK_OK;

  /* set buffer - verify that it will be zero terminated */
  memset(&serial_buff[0], 0x00, sizeof(serial_buff));


  /* Initialize NOR flash interface */
  rc = norfl_init();
  if (atlk_error(rc)) {
    printf("ERROR: Failed to initialize NOR Flash device, rc=%d\n",rc);
    goto err;
  }

  /* Read partition table */
  rc = norfl_part_table_read(&part_table);
  if (atlk_error(rc)) {
    printf("ERROR: Failed to read partition table, rc=%d\n",rc);
    goto err;
  };

  /* Search for lockable partition */
  for (part_num = 0 ; part_num < NORFL_NUM_PARTS; part_num++) {
    uint32_t part_type = part_table.part_info[part_num].part_type;
    if (part_type == (NORFL_PART_TYPE_USER_MIN | NORFL_PART_TYPE_F_LOCKABLE)) {
      break;
    }
    else if (part_type == (NORFL_PART_TYPE_USER_MIN | NORFL_PART_TYPE_F_LOCKED)) {
      printf("ERROR: Partition is already LOCKED\n");
      goto err;
    }
  }

  /* validate part_num value */
  if (part_num == NORFL_NUM_PARTS) {
    printf("ERROR: Didn't Find Lockable Partition\n");
    goto err;
  }

  /* Set Serial string */
  memcpy(&serial_buff[0],
         SERIAL_STRING_EXAMPLE,
         strlen(SERIAL_STRING_EXAMPLE));

  /* write serial into lockable partition
   * Note : Re-write can be done as many you want until
   *        lock operation
   */
  rc = norfl_part_rewrite(part_num,
                          serial_buff,
                          sizeof(serial_buff));
  if (atlk_error(rc)) {
    printf("ERROR: Failed to re-write, rc=%d\n",rc);
    goto err;
  };
  printf("Re-Write: serial '%s' to partition %d\n",
          serial_buff,
          part_num);

  /* lock Partition */

  /*
     Note - 'Lock' code is under comment to allow execute
            example several times and not lock partition on
            first re-write.
  */

/*  rc = norfl_part_lock(part_num);
  if (atlk_error(rc)) {
    printf("ERROR: Failed to lock partition, rc=%d\n",rc);
    goto err;
  };

  printf("Partition %d is locked \n",
          part_num);
 */

err:
  return;
}


void craton_user_init(void)
{

  /*
     First Run Output: (Serial is Empty)
       read  - Serial Is empty
       write - Re-Write serial 'Hk1D6j8Zz1$P2g6' to partition 15
       read  - Serial string: 'Hk1D6j8Zz1$P2g6'

     if partition is locked:
     Second Run Output: (Serial is locked) -
       read  - Serial string: 'Hk1D6j8Zz1$P2g6'
       write - ERROR: Didn't Find Lockable Partition
       read  - Serial string: 'Hk1D6j8Zz1$P2g6'

     if partition is NOT locked (current implementation):
     Second Run Output: (Serial is Not locked) -
       read  - Serial string: 'Hk1D6j8Zz1$P2g6'
       write - Re-Write serial 'Hk1D6j8Zz1$P2g6' to partition 15
       read  - Serial string: 'Hk1D6j8Zz1$P2g6'
  */

  /* read Serial String */
  serial_read_example();

  /* Write Serial String */
  serial_write_example();

  /* read Serial String */
  serial_read_example();

  return;

}



