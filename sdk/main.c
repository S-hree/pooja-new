
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>

#include <tx_api.h>
#include <atlk/v2x_service.h>

#include <atlk/nav.h>
#include <atlk/mibs/wlan-mib.h>
#include <libcli.h>
#include <craton/cli.h>
#include <craton/syslog.h>

#include <linuxapi.h>
#include <wave_lib.h>
#include <dot3_common.h>
#include <dot3_api.h>
#include <gps.h>
//#include <channel.h>

#include <j2735_mem.h>

//#include <fhwainit.h>
#include <craton/bootparam.h>


int dot2_init();
int dot3_init();
//void init_ITRIdot4(void);

int cmd_dot3_rx_user_normal(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_rx_user_notify(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_rx_user_start(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_rx_user_stop(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_tx_user(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_rx_provider(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_show_mib(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_cfg_wsa(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot3_show_channel(cli_instance_t *cli, const char *command, char *argv[], int argc);

int cmd_oslink(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_oslink_debug(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_oslink_raw(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_cvt_rule(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_cvt_rule_raw(cli_instance_t *cli, const char *command, char *argv[], int argc);

int cmd_audio_test(cli_instance_t *cli, const char *command, char *argv[], int argc);

int cmd_wave(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_ip(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_gps(cli_instance_t *cli, const char *command, char *argv[], int argc);
void _ip_init(void);
int cmd_dot2_demo(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_dot2_show_sds(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_test_us_sec(cli_instance_t *cli, const char *command, char *argv[], int argc);

int cmd_j2735r41_bsm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_csr(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_eva(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_ica(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_j2735r41_map(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_nmea(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_pdm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_pvd(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_rsa(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_rtcm(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_j2735r41_spat(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_srm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_j2735r41_ssm(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_j2735r41_tim(cli_instance_t *cli, const char *command, char *argv[], int argc);

int cmd_txBsmPartI(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_txBsmPartIISafetyExtEvents(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_txBsmPartIISafetyExtPathHistory(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_txBsmPartIISafetyExtPathPrediction(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_txBsmPartIISafetyExtRtcm(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_txBsmPartIIStatus(cli_instance_t *cli, const char *command, char *argv[], int argc);
int cmd_rxBsm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txCsr(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxCsr(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txEva(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxEva(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txIca(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxIca(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txMap(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxMap(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txNmea(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxNmea(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txPdm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxPdm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txPvd(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxPvd(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txRsa(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxRsa(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txRtcm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxRtcm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txSpat(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxSpat(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txSrm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxSrm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txSsm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxSsm(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_txTim(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_rxTim(cli_instance_t *cli, const char *command, char *argv[], int argc);

int cmd_tftp_get(cli_instance_t *cli, const char *command, char *argv[], int argc);

//int cmd_dot11_msgtest(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_dot11_payee(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_dot11_payer(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_dot11_request(cli_instance_t *cli, const char *command, char *argv[], int argc);
//int cmd_dot11_response(cli_instance_t *cli, const char *command, char *argv[], int argc);

int cmd_gpio_pca9554a(cli_instance_t *cli, const char *command, char *argv[], int argc);



void craton_user_init(void)
{
	/* CLI instance */
	cli_instance_t *cli = NULL;
	cli_instance_t *cli_telnet = NULL;
	/* CLI command handle */
	struct cli_command *command = NULL;
	/* Autotalks return code */
	atlk_rc_t rc = ATLK_OK;
	mib_service_t *service = NULL;

	rc = mib_default_service_get(&service);
	if (atlk_error(rc))
		fprintf(stderr, "mib_default_service_get: %s\n", atlk_rc_to_str(rc));
	rc = mib_set_wlanDcocEnabled(service, 1, 0);
	rc = mib_set_wlanDcocEnabled(service, 2, 0);
	if (atlk_error(rc))
		fprintf(stderr, "mib_set_wlanDcocEnabled: %s\n", atlk_rc_to_str(rc));

	linux_api_init();
	_ip_init();
	wave_init();
	//init_ITRIdot4();
	dot2_init();
	dot3_init();
	gps_init();
  j2735_init();

	/* Get CRATON UART CLI instance */
	rc = cli_instance_get(&cli, CLI_INSTANCE_TYPE_UART);
	if (atlk_error(rc)) {
		fprintf(stderr, "cli_instance_get: %s\n", atlk_rc_to_str(rc));
		abort();
	}
	assert(cli != NULL);

	rc = cli_instance_get(&cli_telnet, CLI_INSTANCE_TYPE_TELNET);
	if (atlk_error(rc)) {
		fprintf(stderr, "cli_instance_get: %s\n", atlk_rc_to_str(rc));
		abort();
	}
	assert(cli_telnet != NULL);


	printf("This is US, version 747, date 20160923\n");

	/* Register example command */
//	command = cli_register_command(cli, NULL, "wave", cmd_wave,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI wave command");
//	command = cli_register_command(cli, NULL, "ip", cmd_ip,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI ip command");
	command = cli_register_command(cli, NULL, "gps", cmd_gps,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show GPS information");
	command = cli_register_command(cli_telnet, NULL, "gps", cmd_gps,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show GPS information");			 
//	command = cli_register_command(cli, NULL, "dot3_rx_user_normal", cmd_dot3_rx_user_normal,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot3 rx user normal command");
//	command = cli_register_command(cli_telnet, NULL, "dot3_rx_user_normal", cmd_dot3_rx_user_normal,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot3 rx user normal command");
//	command = cli_register_command(cli, NULL, "dot3_rx_user_notify", cmd_dot3_rx_user_notify,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot3 rx user notify command");
	command = cli_register_command(cli, NULL, "dot3_rx_user_start", cmd_dot3_rx_user_start,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Start receiving WSM packets");
	command = cli_register_command(cli_telnet, NULL, "dot3_rx_user_start", cmd_dot3_rx_user_start,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Start receiving WSM packets");
	command = cli_register_command(cli, NULL, "dot3_rx_user_stop", cmd_dot3_rx_user_stop,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Stop receiving WSM packets");
	command = cli_register_command(cli_telnet, NULL, "dot3_rx_user_stop", cmd_dot3_rx_user_stop,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Stop receiving WSM packets");
	command = cli_register_command(cli, NULL, "dot3_tx_user", cmd_dot3_tx_user,
                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Transmit WSM packets");
	command = cli_register_command(cli_telnet, NULL, "dot3_tx_user", cmd_dot3_tx_user,
                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Transmit WSM packets");
	command = cli_register_command(cli, NULL, "dot3_rx_provider", cmd_dot3_rx_provider,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Transmit WSA & Receive WSM");
	command = cli_register_command(cli, NULL, "dot3_show_mib", cmd_dot3_show_mib,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show MIB information");
	command = cli_register_command(cli, NULL, "dot3_show_wsa_cfg", cmd_dot3_cfg_wsa,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show WSA parameters");
	command = cli_register_command(cli, NULL, "dot3_show_channel", cmd_dot3_show_channel,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show channel information");

	command = cli_register_command(cli, NULL, "dot2_demo", cmd_dot2_demo,
                                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Demo 1609 dot2 feature");
	command = cli_register_command(cli, NULL, "dot2_show_sds", cmd_dot2_show_sds,
                                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Show SDS information");

                              
	cmd_oslink(cli, NULL, NULL, 0);	
	
//	command = cli_register_command(cli, NULL, "oslink_debug", cmd_oslink_debug,
//                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI os link rule command debug for US protocol"); 	
                                 
//    command = cli_register_command(cli_telnet, NULL, "oslink_debug", cmd_oslink_debug,
//                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI os link rule command debug for US protocol");
	
	command = cli_register_command(cli, NULL, "cvt_rule", cmd_cvt_rule,
                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Convert packet between WSM & UDP"); 	
                                 
    command = cli_register_command(cli_telnet, NULL, "cvt_rule", cmd_cvt_rule,
                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Convert packet between WSM & UDP"); 
                
	/* cmd_oslink_raw(cli, NULL, NULL, 0);	
	
	command = cli_register_command(cli, NULL, "cvt_rule_raw", cmd_cvt_rule_raw,
                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI os link rule command for raw protocol"); 	
                                 
    command = cli_register_command(cli_telnet, NULL, "cvt_rule_raw", cmd_cvt_rule_raw,
                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI os link rule command for raw protocol"); 
    */            
//	command = cli_register_command(cli, NULL, "audio_test", cmd_audio_test,
//                                 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI audio test command");

	/*j2735r63*/	
//	command = cli_register_command(cli, NULL, "j2735r63_bsm", cmd_j2735r63_bsm,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r63_bsm command");

	/*j2735r41*/
	command = cli_register_command(cli, NULL, "j2735r41_bsm", cmd_j2735r41_bsm,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Example j2735r41_bsm command");
//	command = cli_register_command(cli, NULL, "j2735r41_csr", cmd_j2735r41_csr,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_csr command");
//	command = cli_register_command(cli, NULL, "j2735r41_eva", cmd_j2735r41_eva,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_eva command");
//	command = cli_register_command(cli, NULL, "j2735r41_ica", cmd_j2735r41_ica,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_ica command");
	command = cli_register_command(cli, NULL, "j2735r41_map", cmd_j2735r41_map,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Example j2735r41_map command");
//	command = cli_register_command(cli, NULL, "j2735r41_nmea", cmd_j2735r41_nmea,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_nmea command");
//	command = cli_register_command(cli, NULL, "j2735r41_pdm", cmd_j2735r41_pdm,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_pdm command");
//	command = cli_register_command(cli, NULL, "j2735r41_pvd", cmd_j2735r41_pvd,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_pvd command");
//	command = cli_register_command(cli, NULL, "j2735r41_rsa", cmd_j2735r41_rsa,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_rsa command");
//	command = cli_register_command(cli, NULL, "j2735r41_rtcm", cmd_j2735r41_rtcm,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_rtcm command");
	command = cli_register_command(cli, NULL, "j2735r41_spat", cmd_j2735r41_spat,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Example j2735r41_spat command");
//	command = cli_register_command(cli, NULL, "j2735r41_srm", cmd_j2735r41_srm,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_srm command");
//	command = cli_register_command(cli, NULL, "j2735r41_ssm", cmd_j2735r41_ssm,
//				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI j2735r41_ssm command");
	command = cli_register_command(cli, NULL, "j2735r41_tim", cmd_j2735r41_tim,
				 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Example j2735r41_tim command");

	command = cli_register_command(cli, NULL, "txBsm", cmd_txBsmPartI,
                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Transmit BSM");
//	command = cli_register_command(cli, NULL, "txBsmPartIISafetyExtEvents", cmd_txBsmPartIISafetyExtEvents,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txBsmPartIISafetyExtEvents command");
//	command = cli_register_command(cli, NULL, "txBsmPartIISafetyExtPathHistory", cmd_txBsmPartIISafetyExtPathHistory,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txBsmPartIISafetyExtPathHistory command");
//	command = cli_register_command(cli, NULL, "txBsmPartIISafetyExtPathPrediction", cmd_txBsmPartIISafetyExtPathPrediction,
//			PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txBsmPartIISafetyExtPathPrediction command");
//	command = cli_register_command(cli, NULL, "txBsmPartIISafetyExtRtcm", cmd_txBsmPartIISafetyExtRtcm,
//			PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txBsmPartIISafetyExtRtcm command");
//	command = cli_register_command(cli, NULL, "txBsmPartIIStatus", cmd_txBsmPartIIStatus,
//			PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txBsmPartIIStatus command");
	command = cli_register_command(cli, NULL, "rxBsm", cmd_rxBsm,
			PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Receive BSM");
//	command = cli_register_command(cli, NULL, "txCsr", cmd_txCsr,
//                       PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txCsr command");
//	command = cli_register_command(cli, NULL, "rxCsr", cmd_rxCsr,
//                         PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxCsr command");
//	command = cli_register_command(cli, NULL, "txEva", cmd_txEva,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txEva command");
//	command = cli_register_command(cli, NULL, "rxEva", cmd_rxEva,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxEva command");
//	command = cli_register_command(cli, NULL, "txIca", cmd_txIca,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txIca command");
//	command = cli_register_command(cli, NULL, "rxIca", cmd_rxIca,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxIca command");
//	command = cli_register_command(cli, NULL, "txMap", cmd_txMap,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Transmit MAP");
//	command = cli_register_command(cli, NULL, "rxMap", cmd_rxMap,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Receive MAP");
//	command = cli_register_command(cli, NULL, "txNmea", cmd_txNmea,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txNmea command");
//	command = cli_register_command(cli, NULL, "rxNmea", cmd_rxNmea,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxNmea command");
//	command = cli_register_command(cli, NULL, "txPdm", cmd_txPdm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txPdm command");
//	command = cli_register_command(cli, NULL, "rxPdm", cmd_rxPdm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxPdm command");
//	command = cli_register_command(cli, NULL, "txPvd", cmd_txPvd,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txPvd command");
//	command = cli_register_command(cli, NULL, "rxPvd", cmd_rxPvd,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxPvd command");
//	command = cli_register_command(cli, NULL, "txRsa", cmd_txRsa,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txRsa command");
//	command = cli_register_command(cli, NULL, "rxRsa", cmd_rxRsa,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxRsa command");
//	command = cli_register_command(cli, NULL, "txRtcm", cmd_txRtcm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txRtcm command");
//	command = cli_register_command(cli, NULL, "rxRtcm", cmd_rxRtcm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxRtcm command");
//	command = cli_register_command(cli, NULL, "txSpat", cmd_txSpat,
//				PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Transmit SPAT");
//	command = cli_register_command(cli, NULL, "rxSpat", cmd_rxSpat,
//				PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Receive SPAT");
//	command = cli_register_command(cli, NULL, "txSrm", cmd_txSrm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txSrm command");
//	command = cli_register_command(cli, NULL, "rxSrm", cmd_rxSrm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxSrm command");
//	command = cli_register_command(cli, NULL, "txSsm", cmd_txSsm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI txSsm command");
//	command = cli_register_command(cli, NULL, "rxSsm", cmd_rxSsm,
//                           PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI rxSsm command");
//	command = cli_register_command(cli, NULL, "txTim", cmd_txTim,
//				PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Transmit TIM");
//	command = cli_register_command(cli, NULL, "rxTim", cmd_rxTim,
//				PRIVILEGE_UNPRIVILEGED, MODE_ANY, "Receive TIM");

	/*********************************************************************************/

//	command = cli_register_command(cli, NULL, "dot11_message_test", cmd_dot11_msgtest,
//                                 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot11 message encode/decode testing");
//	command = cli_register_command(cli, NULL, "dot11_payee_service", cmd_dot11_payee,
//                                 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot11 payee service");
//	command = cli_register_command(cli, NULL, "dot11_payer_service", cmd_dot11_payer,
//                                 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot11 payer service");
//	command = cli_register_command(cli, NULL, "dot11_payee_request", cmd_dot11_request,
//                                 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot11 payee action request");
//	command = cli_register_command(cli, NULL, "dot11_payer_response", cmd_dot11_response,
//                                 PRIVILEGE_UNPRIVILEGED, MODE_ANY, "ITRI dot11 payee action response");
                                
	/* PCA9554A GPIO */
//	command = cli_register_command(cli, NULL, "gpio_pca9554a", cmd_gpio_pca9554a,
//                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "GPIO for PCA9554A"); 	
                                 
//    command = cli_register_command(cli_telnet, NULL, "gpio_pca9554a", cmd_gpio_pca9554a,
//                PRIVILEGE_UNPRIVILEGED, MODE_ANY, "GPIO for PCA9554A");     

                
	/* Add your own cmd by using the following cli_register_command */

	if (command == NULL) {
		fprintf(stderr, "cli_register_command returned NULL\n");
		abort();
	}

	return;
}

