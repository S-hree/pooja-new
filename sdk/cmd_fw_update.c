#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#include <tx_api.h>

#include <atlk/sdk.h>

#include <craton/fs.h>
#include <craton/nor_flash.h>
#include <craton/fw_uimage.h>

#include <libcli.h>
#include <craton/cli.h>

/* 
  CRATON ThreadX Firmware Update Example 
  
  This program basic usage of NOR Flash API for code running on
  top of CRATON processor with ThreadX RTOS.

  Expected firmware image format is U-Boot uImage. For simplicty, the program
  assumes a valid firmware image is at 'B:/uImage' (i.e. on flash).

  Warning! When 'B:/uImage' exists and is valid, this program will re-write
  main firmware image partition with this file.
*/

/* Firmware update thread priority */
#define FW_UPDATE_THREAD_PRIORITY 20

/* Firmware update thread */
static TX_THREAD fw_update_thread;
static uint8_t fw_update_thread_stack[0x8000];
static void fw_update_thread_entry(ULONG input);

/* Firmware image path */
static char fw_image_path[] = "D:/wave_itri.img";  /* D: is RAM disk */

/* Device used in this program */
static char fs_device[] = "D:/";

#define FS_IMAGE_NAME "wave_itri.img"

/* Firmware image max size */
#define FW_IMAGE_SIZE_MAX 0x200000

/* Firmware image buffer */
static uint8_t fw_image_buffer[FW_IMAGE_SIZE_MAX];

static cli_instance_t *fw_cli;

static int fw_thread_del_flag;


void fw_update_init(void)
{
	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	/* ThreadX return value */
	ULONG trv = TX_SUCCESS;
	
	fw_thread_del_flag = 0;
	
	/* Initialize NOR flash interface */
	rc = norfl_init();
	if (atlk_error(rc)) 
	{
		cli_print(fw_cli, "norfl_init: %s\n", atlk_rc_to_str(rc));
		return;
	}
	
	/* Create firmware update thread */
	trv = tx_thread_create(&fw_update_thread, "fw_update_thread",
                         fw_update_thread_entry, 0,
                         fw_update_thread_stack,
                         sizeof(fw_update_thread_stack),
                         FW_UPDATE_THREAD_PRIORITY,
                         FW_UPDATE_THREAD_PRIORITY,
                         TX_NO_TIME_SLICE, TX_AUTO_START);
	
	while(fw_thread_del_flag == 0)
	{
		usleep(2 * 1000 * 1000);
	};
	
    cli_print(fw_cli, "Delete fw_update_thread\n");                     
	tx_thread_terminate(&fw_update_thread);
    tx_thread_delete(&fw_update_thread);  
    cli_print(fw_cli, "Firmware update finish and success\n");                         
	

	return;
}

static void fw_update_thread_entry(ULONG input)
{
	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	/* POSIX return value */
	int rv = 0;
	/* File statistics */
	struct stat st;
	/* Partition table */
	norfl_part_table_t part_table = NORFL_PART_TABLE_INIT;
	/* Size of main firmware image partition */
	uint32_t main_fw_part_size;
	/* Size of firmware image file */
	uint32_t fw_image_size;
	/* File descriptor */
	int fd = -1;
	/* Not using input */
	(void)input;
	int rc2 = 0;
	int ret_flag = 0;

	

	/* Enable the usage of file system in this thread */
	rv = fs_thread_enable();
	if (rv == -1) 
	{
		cli_print(fw_cli, "fs_thread_enable failed, errno: %d\n", errno);
		ret_flag = 1;
	}

	if (ret_flag == 0)
	{
		/* Get firmware image file statistics */
		rv = stat(fw_image_path, &st);
		if (rv == -1) 
		{
			if (errno == 2)
			{
				cli_print(fw_cli, "No new firmware %s need to update\n", fw_image_path);
			}
			else
			{  	
    			cli_print(fw_cli, "New firmware %s stat failed, errno: %d\n", fw_image_path, errno);
			}
   			ret_flag = 1;
		}
	}

	if (ret_flag == 0)
	{
		cli_print(fw_cli, "Start uImage firmware update ....\n");
		
		/* Size of firmware image file */
		fw_image_size = (uint32_t)st.st_size;

		/* Read partition table */
		rc = norfl_part_table_read(&part_table);
		if (atlk_error(rc)) 
		{
    		cli_print(fw_cli, "norfl_part_table_read: %s\n", atlk_rc_to_str(rc));
    		ret_flag = 1;
  		}
	}

	if (ret_flag == 0)
	{
		/* Size of main firmware image partition */
		main_fw_part_size =
		part_table.part_info[NORFL_PART_FIRMWARE_MAIN].part_size;

		cli_print(fw_cli, "Make sure firmware image size makes sense\n");
		/* Make sure firmware image size makes sense */
		
		if ((fw_image_size > main_fw_part_size) || (fw_image_size > sizeof(fw_image_buffer)))
		{
			cli_print(fw_cli, "New firmware %s too big\n", fw_image_path);
			ret_flag = 1;
		}
	}
	
	if (ret_flag == 0)
	{

		cli_print(fw_cli, "Open firmware image in read-only mode\n");
		/* Open firmware image in read-only mode */
		fd = open(fw_image_path, O_RDONLY);
		if (fd == -1) 
		{
    		cli_print(fw_cli, "open failed, errno %d\n", errno);
    		ret_flag = 1;
		}
		else
		{
			cli_print(fw_cli, "Open firmware image success\n");
    		
		}
    }
    
	if (ret_flag == 0)
	{
		cli_print(fw_cli, "Read image into buffer\n");
		/* Read image into buffer */
		rv = read(fd, fw_image_buffer, fw_image_size);
		if (rv == -1) 
		{
			cli_print(fw_cli, "read failed, errno %d\n", errno);
    		ret_flag = 2;
		}
		else
		{
			
			if ((uint32_t)rv != fw_image_size)
			{
				cli_print(fw_cli, "Read %s size no correct\n", fw_image_path);
    			ret_flag = 2;
			}
		}
		
	}
	
	

	if (ret_flag == 0)
	{
		cli_print(fw_cli, "Make sure firmware image is a valid file\n");
		/* Make sure firmware image is a valid uImage file */
  		if (!fw_uimage_valid(fw_image_buffer, fw_image_size)) 
  		{
    		cli_print(fw_cli, "fw_uimage_valid returned false\n");
    		ret_flag = 2;
  		}
	}

	if (ret_flag == 0)
	{
		cli_print(fw_cli, "Re-write main firmware image ...\n");
		
		/* Re-write main firmware image */  
		rc = norfl_part_rewrite(NORFL_PART_FIRMWARE_MAIN,
                          fw_image_buffer, fw_image_size);
  		if (atlk_error(rc)) 
  		{
			cli_print(fw_cli, "norfl_part_rewrite: %s\n", atlk_rc_to_str(rc));
			ret_flag = 2;
		}
		else
		{
	  		cli_print(fw_cli, "Write new %s file to flash successfully !\n", fw_image_path);
		}
	}
  
	if (ret_flag == 0)
	{
  		/* Close the file */
  		rc2 = close(fd);
  		if (rc2 == -1) 
  		{
    		rc2 = -errno;
    		cli_print(fw_cli, "close failed, errno %d\n", rc2);
    		ret_flag = 2;
  		}
	}
  
	if (ret_flag == 0)
	{
		cli_print(fw_cli, "Firmware %s updated successfully!\n", fw_image_path);
  
  		/* Change directory to root of device */
  		rc2 = chdir(fs_device);
  		if (rc2 == -1) 
  		{
    		rc2 = -errno;
    		cli_print(fw_cli, "chdir failed, errno %d\n", rc2);
    		ret_flag = 1;
  		}
  		else
  		{
	  		cli_print(fw_cli, "Changed to directory '%s' successfully !\n", fs_device);
  		}
	}
	
	if (ret_flag == 0)
	{		
  
  		/* Delete the file */
  		rc2 = remove(FS_IMAGE_NAME);
  		if (rc2 == -1) 
  		{
     		rc2 = -errno;
     		cli_print(fw_cli, "remove %s failed, errno %d\n", FS_IMAGE_NAME,rc2);
     		ret_flag = 1;
  		}
  		else
  		{
	  		cli_print(fw_cli,"Remove %s firmware file successfully!\n", fw_image_path);
  		}
	}

  

	if (ret_flag == 2)
	{
  		/* Clean-up resources */
  		close(fd);
	}
	
	fw_thread_del_flag = 1;

  return;
}

static void print_usage(void)
{
	cli_print(fw_cli, "Usage : fw_update < a | d>\n");
	cli_print(fw_cli, "a : firmware put on A:/wave_itri.img  (A: mean SD)\n");
	cli_print(fw_cli, "d : firmware put on D:/wave_itri.img  (D: mean RAM disk)\n");
	return;
}

int cmd_fw_update2(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	(void)cli;
	(void)command;
	(void)argc;
	(void)argv;
	
	fw_cli = cli;
	
	if (argc != 1)
	{
		print_usage();
		return -1;
	}
	else
	{
		if (argv[0][0] == 'a')
		{
			fw_image_path[0] = 'A';
			fs_device[0] = 'A';			
		}
		else if (argv[0][0] == 'd')
		{
			fw_image_path[0] = 'D';
			fs_device[0] = 'D';
		}
		else
		{
			print_usage();
			return -1;
		}		
		
	}
	
	fw_update_init();
	
	return CLI_OK;
    
	
}