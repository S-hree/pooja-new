#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <libcli.h>
#include <craton/cli.h>
#include <craton/debug.h>
#include <tx_api.h>
#include <dot3_common.h>
#include <dot3_api.h>


static int stop;

void stop_func(ULONG input)
{
	(void)input;
	
	stop = 1;
	
	return;
}

/* Dot3 Rx User Program */
/*************************************************************************/
int cmd_dot3_rx_user_normal(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	int err;
	TX_TIMER stop_timer;
	wme_handle_t handle;
	unsigned char channel_number;
	unsigned char radio_number;
	unsigned int psid;
	struct event_message event;
	struct user_info user_info;
	struct wsm_info wsm_info;
	struct in_wsm in_wsm;	

	if(argc != 3) { /* In threadx, cmd is not included in argc */
		printf("rx_user <psid> <radio_number> <channel_number> \n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("-- example --\n");
		printf("dot3_rx_user 123 0 172\n");
		return 1;
	}

	//psid = (unsigned int) atoll(argv[1]);
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
	stop = 0;
	tx_timer_create(&stop_timer, NULL, stop_func, 0, 5000, 1, TX_AUTO_ACTIVATE);
	while(stop == 0) {
		err = wme_wsm_recv(&handle, &in_wsm, 1000);
		if(err != 0) {
			printf("wsm_recv = %d\n", err);
			continue;
		}
		printf("wsm_recv success\n");
		printf("wsm data len     = %d\n", in_wsm.length);
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

	tx_timer_delete(&stop_timer);

	wme_exit(&handle);

	return 0;
}
/*************************************************************************/


TX_THREAD rx_user_thread;
unsigned char rx_user_stack[5*1024];
static unsigned char dot3_channel_number;
static unsigned char dot3_radio_number;
static unsigned int dot3_psid;


/* Dot3 Rx User Stop Program */
/*************************************************************************/
int cmd_dot3_rx_user_stop(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	stop = 1;

	usleep(1000000);

	tx_thread_terminate(&rx_user_thread);
	tx_thread_delete(&rx_user_thread);

	return 0;
}

/*************************************************************************/

/* Dot3 Rx User Start Program */
/*************************************************************************/
void rx_user_start(ULONG input)
{
	(void)input;
	int err;

	wme_handle_t handle;
	unsigned char channel_number;
	unsigned char radio_number;
	unsigned int psid;
	struct event_message event;
	struct user_info user_info;
	struct wsm_info wsm_info;
	struct in_wsm in_wsm;
	int count;
	//struct timeval time, cur_time;

	psid = dot3_psid;
	radio_number = dot3_radio_number;
	channel_number = dot3_channel_number;
	
	err = wme_init(&handle);
	if(err != ACCEPT) {
		printf("wme_init = %d\n", err);
		return ;
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
		return ;
	}
	printf("wme_user_service to add success\n");

	wsm_info.action = ADD;
	wsm_info.psid = psid;
	err  = wme_wsm_service(&handle, &wsm_info);
	if(err != 0) {
		printf("wme_wsm_service = %d\n", err);
		wme_exit(&handle);
		return ;
	}
	printf("wme_wsm_service to add success\n");

	while(1){
		err = wme_event_recv(&handle, &event, 1000);
		if(err != 0) {
			printf("wme_event_waiting = %d\n", err);
		if(err == -SYSTEM_INTR)
			continue;
		else
			return ;
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

	printf("start to recv wsm packets\n");
	stop = 0;
	count = 0;
	//tx_timer_create(&stop_timer, NULL, stop_func, 0, 5000, 1, TX_AUTO_ACTIVATE);
	while(stop == 0) {
		err = wme_wsm_recv(&handle, &in_wsm, 100);
		if(err != 0) {
			//printf("wsm_recv = %d\n", err);
			continue;
		}
		printf("wsm_recv success\n");
		printf("count = %d\n",  ++count);
		printf("psid = %d\n",  in_wsm.psid);
		printf("channel = %d\n",  in_wsm.channel_number);
		printf("tx_power = %d\n",  in_wsm.txpwr_level);
		printf("data_rate = %d\n",  in_wsm.data_rate);
		printf("data len = %d\n",  in_wsm.length);		
	}

	wsm_info.action = DELETE;
	err  = wme_wsm_service(&handle, &wsm_info);
	if(err != 0) {
		printf("wme_wsm_service = %d\n", err);
		wme_exit(&handle);	
		return ;
	}
	printf("wme_wsm_service to del success\n");

	user_info.action = DELETE;
	err  = wme_user_service(&handle, &user_info);
	if(err != 0) {
		printf("wme_user_service = %d\n", err);
		wme_exit(&handle);
		return ;
	}
	printf("wme_user_service to del success\n");

	wme_exit(&handle);
}

int cmd_dot3_rx_user_start(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	int err;

	if(argc != 3) { /* In threadx, cmd is not included in argc */
		printf("rx_user <psid> <radio_number> <channel_number> \n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("-- example --\n");
		printf("dot3_rx_user_start 123 0 172\n");
		return 1;
	}

	dot3_psid = atoi(argv[0]);
	dot3_radio_number = atoi(argv[1]);
	dot3_channel_number = atoi(argv[2]);

	err = tx_thread_create(&rx_user_thread, "rx_user_thread", rx_user_start, (ULONG)NULL, rx_user_stack, 5*1024, 15, 15, TX_NO_TIME_SLICE, TX_AUTO_START);
	if(err != 0) {
		printf("rx_provider_recv_thread create failure\n");
		return -1;
	}

	return 0;
}
/*************************************************************************/



/* Dot3 Rx User Notify Program */
/*************************************************************************/

static TX_SEMAPHORE notify;

void rx_wsm_notify(wme_handle_t *handle)
{
	(void)handle;

	tx_semaphore_put(&notify);

	return;
}

void stop_notify_func(ULONG input)
{
	(void)input;
	stop = 1;
	tx_semaphore_put(&notify);
	return;
}

int cmd_dot3_rx_user_notify(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	int err;
	TX_TIMER stop_timer;
	wme_handle_t handle;
	unsigned char channel_number;
	unsigned char radio_number;
	unsigned int psid;
	struct event_message event;
	struct user_info user_info;
	struct wsm_info wsm_info;
	struct in_wsm in_wsm;	

	if(argc != 3) { /* In threadx, cmd is not included in argc */
		printf("rx_user <psid> <radio_number> <channel_number> \n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("-- example --\n");
		printf("dot3_rx_user notify 123 0 172\n");
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

	tx_semaphore_create(&notify, NULL, 0);

	err = wme_wsm_recv_notify(&handle, rx_wsm_notify);
	if(err != ACCEPT) {
		printf("wme_wsm_recv_notify = %d\n", err);
		wme_exit(&handle);	
		return 0;
	}
	printf("wme_wsm_recv_notify add success\n");

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
	stop = 0;
	tx_timer_create(&stop_timer, NULL, stop_notify_func, 0, 5000, 1, TX_AUTO_ACTIVATE);
	while(stop == 0) {
		tx_semaphore_get(&notify, TX_WAIT_FOREVER);  /* Waiting for notification or timer stoping */
		err = wme_wsm_recv(&handle, &in_wsm, 0);
		if(err != 0) {
			printf("wsm_recv = %d\n", err);
			break;
		}
		printf("wsm_recv success\n");
		printf("wsm data len     = %d\n", in_wsm.length);
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

	wme_wsm_recv_notify(&handle, NULL);
	tx_semaphore_delete(&notify);	
	tx_timer_delete(&stop_timer);

	wme_exit(&handle);

	return 0;
}
/*************************************************************************/


/* Dot3 Tx User Program */
/*************************************************************************/
int cmd_dot3_tx_user(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	int err, i;
	wme_handle_t handle;
	unsigned short size, num;
	unsigned char request_type, radio_number, channel_number, is_switch, data_rate, tx_power, priority, interval;
	unsigned int psid;
	unsigned char macAddr_c[6] = {0};
	unsigned int macAddr_i[6];
	struct event_message event;
	struct user_info user_info;
	struct out_wsm out_wsm;
	int c_flag, s_flag;
	unsigned char buf[WSM_MAX_SIZE] = {0};
	//struct timeval time;

	
	if(argc != 12) { /* In threadx, cmd is not included in argc */
		printf("tx_user <psid> <request_type> <radio_number> <channel_number> <is_switch> <data_rate> <tx_power> <priority> <dest_mac> <size> <interval> <num>\n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<request_type> access of user service, 0:MATCH/1:UNCONDITIONAL/2:NO SCH ACCESS\n");
		printf("<radio_number> the radio number, 0 - 1\n");		
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("<is_switch> channel mode of user service, 0:continuous/1:channel switch\n");
		printf("<data_rate> data rate of sending WSMV2 packets, 6 9 12 18 24 36 48 54\n");
		printf("<tx_power> tx power level of sending WSMV2 packets, 12- 25, unit:dBm\n");
		printf("<priority> priority of user service, 0 - 63\n");
		printf("<dest_mac> peer mac address of sending WSMV2 packets, format: XX:XX:XX:XX:XX:XX\n");
		printf("<size> the size of sending packets, 0 - 1399, unit:byte\n");
		printf("<interval> the interval of sending packets, 0 - 1000, unit:ms\n");
		printf("<num> the number of sending packets, 1 - 10000\n");		
		printf("-- example --\n");
		printf("dot3_tx_user 123 1 0 172 0 6 18 1 FF:FF:FF:FF:FF:FF 1000 100 10\n");
		return 1;
	}

	//psid = (unsigned int) atoll(argv[1]);
	psid = atoi(argv[0]);
	request_type = atoi(argv[1]);
	radio_number = atoi(argv[2]);
	channel_number = atoi(argv[3]);
	is_switch = atoi(argv[4]);
	data_rate = atoi(argv[5]);
	tx_power = atoi(argv[6]);
	priority = atoi(argv[7]);

	/* Convert the mac addr */
	if(sscanf(argv[8], "%2x:%2x:%2x:%2x:%2x:%2x\n", &macAddr_i[0], &macAddr_i[1], &macAddr_i[2], &macAddr_i[3], &macAddr_i[4], &macAddr_i[5]) != 6) {
		printf("peerMacAddress invalid...\n");
		return 0;
	}
	for(i = 0; i < 6; i++) {
		macAddr_c[i] = (unsigned char) macAddr_i[i];
	}

	size = atoi(argv[9]);
	interval = atoi(argv[10]);
	num = atoi(argv[11]);

	err = wme_init(&handle);
	if(err != 0) {
		printf("wme_init = %d\n", err);
		return 0;
	}
	printf("wme_init success, handle = %d\n", handle);

	/* Add user service */
	user_info.action = ADD;
	memset(&user_info, 0, sizeof(user_info));
	user_info.radio_number = radio_number;
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

	c_flag = request_type == 2 ? 1 : 0;
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
	memcpy(out_wsm.data, buf, size); 
	out_wsm.length = size;

	if(request_type == 2){
		printf("The request type is NO_ACCESS, just return\n");
	}
	else {
		while(num != 0){
			err = wme_wsm_send(&handle, &out_wsm);
			if(err != 0) {
				printf("wsm_send = %d\n", err);
				return 0;
			}
			//printf("wsm_send success\n");
			num--;
			usleep(interval * 1000);
		}
	}
	user_info.action = DELETE;
	err = wme_user_service(&handle, &user_info);
	if(err != 0) {
		printf("wme_user_service = %d\n", err);
		return 0;
	}
	printf("wme_user_service to delete success\n");

	wme_exit(&handle);

	return 0;
}
/*************************************************************************/


/* Dot3 Rx Provider Program */
/*************************************************************************/
int cmd_dot3_rx_provider(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	int ret;
	wme_handle_t handle;
	TX_TIMER stop_timer;	
	unsigned char radio_number, channel_number, is_switch, priority, is_sec;
	unsigned int psid, timeout, count;
	struct provider_info provider_info;
	struct wsm_info wsm_info;	
	struct in_wsm in_wsm;	

	if(argc != 7) { /* In threadx, cmd is not included in argc */
		printf("rx_provider <psid> <radio_number> <channel_number> <is_switch> <priority> <is_secure> <timeout>\n");
		printf("-- usage --\n");
		printf("<psid> psid of sending WSMV2 packets, 1 - 127\n");
		printf("<radio_number> the radio number, 0 - 1\n");
		printf("<channel_number> channel number of sending WSMV2 packets, 172 174 176 180 182 184\n");
		printf("<is_switch> channel mode of user service, 0:continuous/1:channel switch\n");		
		printf("<priority> priority of user service, 0 - 63\n");
		printf("<is_secure> enalbe the security wsa or not, 0:no sec wsa/1:sec wsa,\n");
		printf("<timeout> the timeout value, unit: sec\n");
		printf("-- example --\n");
		printf("dot3_rx_provider 123 1 172 0 1 0 10\n");
		return 1;
	}

	psid = atoi(argv[0]);
	radio_number = atoi(argv[1]);
	channel_number = atoi(argv[2]);
	is_switch = atoi(argv[3]);
	priority = atoi(argv[4]);
	is_sec = atoi(argv[5]);
	timeout = atoi(argv[6]);

	ret = wme_init(&handle);
	if(ret != 0) {
		printf("wme_init = %d\n", ret);
		return 0;
	}
	printf("wme_init success, handle = %d\n", handle);

	/* Add provider service */
	provider_info.action = ADD;
	memset(&provider_info, 0, sizeof(provider_info));
	provider_info.radio_number = radio_number;
	memcpy(provider_info.dest_mac, "\xFF\xFF\xFF\xFF\xFF\xFF", MAC_SIZE);
	provider_info.wsa_type= (is_sec == 0 ? WSA_UNSECURE : WSA_SECURE);
	provider_info.psid = psid;
	provider_info.service_priority = priority;
	provider_info.channel_number = channel_number;
	provider_info.channel_access = (is_switch == 1) ? MODE_SCH : MODE_BOTH;
	provider_info.repeat_rate = 50;
	provider_info.ip_service = 1;
	memcpy(provider_info.psc, "ITRI_WSA", 9);
	provider_info.psc_len= 9;
	provider_info.wsa_count_threshold = 1;
	//provider_info.wsa_hdr_extensions;

	ret = wme_provider_service(&handle, &provider_info);
	if(ret < 0) {
		printf("wme_provider_service = %d\n", ret);
		wme_exit(&handle);
		return 0;
	}
	printf("wme_provider_service to add success\n");

	
	wsm_info.action = ADD;
	wsm_info.psid = psid;
	ret  = wme_wsm_service(&handle, &wsm_info);
	if(ret != 0) {
		printf("wme_wsm_service = %d\n", ret);
		return 0;
	}
	printf("wme_wsm_service to add success\n");

	printf("waiting 5s to recv wsm packets\n");
	stop = 0;
	count = 0;
	tx_timer_create(&stop_timer, NULL, stop_func, 0, timeout * 1000, 1, TX_AUTO_ACTIVATE);
	while(stop == 0) {
		ret = wme_wsm_recv(&handle, &in_wsm, 1000);
		if(ret != 0) {
			printf("wsm_recv = %d\n", ret);
			continue;
		}
		printf("wsm_recv success\n");
		count++;
		printf("count = %d\n", count);
		printf("psid = %d\n",  in_wsm.psid);
		printf("channel = %d\n",  in_wsm.channel_number);
		printf("tx_power = %d\n",  in_wsm.txpwr_level);
		printf("data_rate = %d\n",  in_wsm.data_rate);
		printf("data len = %d\n",  in_wsm.length);			
	}
	wsm_info.action = DELETE;
	ret  = wme_wsm_service(&handle, &wsm_info);
	if(ret != 0) {
		printf("wme_wsm_service = %d\n", ret);
		return 0;
	}
	printf("wme_wsm_service to delete success\n");

	provider_info.action = DELETE;
	ret = wme_provider_service(&handle, &provider_info);
	if(ret < 0) {
		printf("wme_provider_service = %d\n", ret);
		wme_exit(&handle);
		return 0;
	}
	printf("wme_provider_service to delete success\n");

	tx_timer_delete(&stop_timer);

	printf("wme exit\n");

	wme_exit(&handle);

	return 0;
}
/*************************************************************************/



/* Dot3 Show Mib Program */
/*************************************************************************/
int cmd_dot3_show_mib(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	struct mib_info info;
	wme_handle_t handle;
	int err;

	err = wme_init(&handle);
	if(err != 0) {
		printf("wme_init = %d\n", err);
		return 0;
	}
	printf("wme_init success, handle = %d\n", handle);	

	printf("*** Provider entry ***\n");
	info.entry_type = PROVIDER_ENTRY;
	info.entry_index = -1;
	info.next = 1;
	while(wme_mib_get(&info) == ACCEPT){
		printf("index = %d\n", info.entry_index);
		printf("psid  = %d\n", info.entry_value.provider_entry.psid);
	}

	
	printf("*** User entry ***\n");
	info.entry_type = USER_ENTRY;
	info.entry_index = -1;
	info.next = 1;
	while(wme_mib_get(&info) == ACCEPT){
		printf("index = %d\n", info.entry_index);
		printf("psid  = %d\n", info.entry_value.user_entry.psid);
		printf("psc   = %s\n", info.entry_value.user_entry.psc);
		printf("priority   = %d\n", info.entry_value.user_entry.service_priority);
		printf("wsa_type  = %d\n", info.entry_value.user_entry.wsa_type);
		printf("radio num  = %d\n", info.entry_value.user_entry.radio_number);
		printf("channel  = %d\n", info.entry_value.user_entry.channel_number);
		printf("\n");
	}

	printf("*** Wsm entry ***\n");
	info.entry_type = WSM_ENTRY;
	info.entry_index = -1;
	info.next = 1;
	while(wme_mib_get(&info) == ACCEPT){
		printf("index = %d\n", info.entry_index);
		printf("psid = %d\n", info.entry_value.wsm_entry.psid);
		printf("\n");
	}

	printf("*** Available entry ***\n");
	info.entry_type = AVAILABLE_ENTRY;
	info.entry_index = -1;
	info.next = 1;
	while(wme_mib_get(&info) == ACCEPT){
		printf("index = %d\n", info.entry_index);
		printf("psid = %d\n", info.entry_value.available_entry.psid);
		printf("count_threshold = %d\n", info.entry_value.available_entry.wsa_count_threshold);
		printf("count_threshold_interval = %d\n", info.entry_value.available_entry.wsa_count_threshold_interval);		
		printf("channel_access = %d\n", info.entry_value.available_entry.channel_info.channel_access);	
		printf("\n");
	}

	wme_exit(&handle);

	return 0;
}
/*************************************************************************/


/* Dot3 Config WSA Program */
/*************************************************************************/
int cmd_dot3_cfg_wsa(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	int err;
	struct wsa_cfg_info info;
	wme_handle_t handle;

	err = wme_init(&handle);
	if(err != 0) {
		printf("wme_init = %d\n", err);
		return 0;
	}
	printf("wme_init success, handle = %d\n", handle);

	err = wme_wsa_cfg_get(&info);
	if(err != 0) {
		printf("wme_wsa_cfg_get = %d\n", err);
		return 0;
	}
	printf("current wsa config\n");
	printf("  repeat rate = %d\n", info.repeat_rate);
	printf("  transmit power = %d\n", info.transmit_power);
	printf("  advertiser id = %s\n", info.advertiser_id);
	printf("  country_str = %s\n", info.country_str);

	wme_exit(&handle);

	return 0;
}
/*************************************************************************/


/* Dot3 Channel Status Program */
/*************************************************************************/
int cmd_dot3_show_channel(cli_instance_t *cli, const char *command, char *argv[], int argc)
{
	/* Unused parameters */
	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	int err;
	struct dev_info dev_c, dev_s;
	wme_handle_t handle;

	err = wme_init(&handle);
	if(err != 0) {
		printf("wme_init = %d\n", err);
		return 0;
	}

	err = wme_device_get(DEV_CCH0, &dev_c);
	if(err != 0) {
		printf("wme_device_get = %d\n", err);
		printf("query CCH0 failed...\n");
		return 0;
	}

	if(dev_c.mode == OP_SWITCH){
		err = wme_device_get(DEV_SCH0, &dev_s);
		if(err != 0){
			printf("wme_device_get = %d\n", err);
			printf("query SCH0 failed...\n");
			return 1;
		}
	}

	if(dev_c.mode == OP_EXTEND)
	        printf("wave0 channel = %d\n", dev_c.channel_number);
	else
	        printf("wave0 channel = %d / %d\n", dev_c.channel_number, dev_s.channel_number);


	err = wme_device_get(DEV_SCH1, &dev_s);
	if(err != 0){
		printf("wme_device_get = %d\n", err);
		printf("query SCH1 failed...\n");
		return 0;
	}

	if(dev_s.mode == OP_EXTEND)
		printf("wave1 channel = %d\n", dev_s.channel_number);
	else
		printf("wave1 channel = 0 / %d\n", dev_s.channel_number);

	wme_exit(&handle);

	return 0;
}
/*************************************************************************/

