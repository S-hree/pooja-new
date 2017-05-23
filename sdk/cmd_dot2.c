#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <libcli.h>
#include <craton/cli.h>
#include <sys/time.h>
#include <time.h>

#include <tx_api.h>
#include <porting_apis.h>

#include "dot2_api.h"

#define DOT2_DEMO_THREAD_PRIORITY 40
#define ROOT 255
#define ID_NOT_LOCALIZED 1
#define WSA 4
#define WSMP_PSID 0x20
#define WSA_PSID 0x23
#define DATA_TYPE_WSMP 0
#define DATA_TYPE_WSA 1

static TX_THREAD dot2_demo_thread;
static uint8_t dot2_demo_thread_stack[0x400000];
static void dot2_demo_wsmp_entry(ULONG input);
static void dot2_demo_wsa_entry(ULONG input);
static void dot2_simple_test_entry(ULONG input);

unsigned int demo_psid = WSMP_PSID;
unsigned int repeat = 15, simple_test_msg_len = 10;
char done_flag = 0;
struct profile_paras{
  	char sign_flag;
  	char sign_data_type;
	char attach_signer_type;
	char sig_alg;
	char sig_type;
	char enc_flag;
	char ver_flag;
	char point_type;
};
struct profile_paras demo_paras = {
  	.sign_flag = 0,
  	.sign_data_type = 0,
	.attach_signer_type = SIGNERTYPE_DEFAULT,
	.sig_alg = SIGNALG_DEFAULT,
	.sig_type = 2,
	.enc_flag = 0,
	.ver_flag = 0,
	.point_type = PKTRANSTYPE_DEFAULT,
};

int set_demo_psid(unsigned int psid){
  	demo_psid = psid;
	return 0;
}

void timersub(struct timeval *a, struct timeval *b, struct timeval *res){
	if( a->tv_usec < b->tv_usec){
	  	if( a->tv_sec <= b->tv_sec){
		  	res->tv_usec = 0;
			res->tv_sec = 0;
		}
	  	// need sec shift
		res->tv_usec = 1000000 + a->tv_usec - b->tv_usec;
		res->tv_sec = a->tv_sec - 1 - b->tv_sec;
	}
	else{
	  	if( a->tv_sec < b->tv_sec){
		  	res->tv_usec = 0;
			res->tv_sec = 0;
		}
		res->tv_usec = a->tv_usec - b->tv_usec;
		res->tv_sec = a->tv_sec - b->tv_sec;
	}
}

void dump(unsigned int len, unsigned char* str){
  	int i;

	for( i = 0; i < len; i++){
	  	if( i != 0 && i % 4 == 0){
		  	printf(" ");
		}
		if( i != 0 && i % 16 == 0){
		  	printf("\n");
		}
	  	printf("%02x", str[i]);
	}
	printf("\nlength = %d\n", len);
}

int generate_cert(int subject_type){
	unsigned char cert_str[1024];
	unsigned int cert_len;
	unsigned int ecdsa_pr, ecies_pr;
	struct cert_fields cert;
	int ret = 0;

	(void)memset(&cert, 0, sizeof(cert));
	cert.version = CERT_VERSION_2;
	cert.subject_type = subject_type;
	cert.psid_num = 1;
	cert.psid[0] = demo_psid;
	ret = wss_create_cert(&cert, cert_str, &cert_len, &ecdsa_pr, &ecies_pr);
	if( ret != 0){
	  	(void)printf("Create subject type %d cert error, code = %d\n", subject_type, ret);
		ret = -1;
	}
	else{
		(void)printf("Cert string:\n");
		dump(cert_len, cert_str);
		(void)printf("\n");
	}
  	return ret;
}

void clear_sds(void);
int set_up_sds(int demo_type){
	int ret = 0;

  	clear_sds();
  	if( demo_type == DATA_TYPE_WSMP){
	  	if( generate_cert(ROOT) != 0){
	  		ret = -1;
	  	}
		else if( generate_cert(ID_NOT_LOCALIZED) != 0){
			ret = -1;
		}
		else{
			// generate cert ok
		}
	}
	else if( demo_type == DATA_TYPE_WSA){
	  	if( generate_cert(ROOT) != 0){
		  	ret = -1;
	  	}
		else if( generate_cert(WSA) != 0){
		  	ret = -1;
		}
		else{
			// generate cert ok
		}
	}
	else{
		// should not be this case
	}
  	return ret;
}

int set_up_profile(void){
  	profile_value_type pf_value;
	int profile_psid, ret = 0;

	if( demo_psid == WSA_PSID){
	  	profile_psid = WSA_PROFILE_PSID;
	}
	else{
	  	profile_psid = demo_psid;
	}
	if( wss_new_profile(profile_psid) != 0){
	  	(void)printf("[1609.2/error] Allocate profile failed.\n");
	  	ret = -1;
	}
	else{
		pf_value.u.usedot2 = 1;
		(void)wss_set_profile(profile_psid, PF_ID_USEDOT2, &pf_value);
		pf_value.u.signflag = demo_paras.sign_flag;
		(void)wss_set_profile(profile_psid, PF_ID_SIGNFLAG, &pf_value);
		pf_value.u.incgentime = 1;
		(void)wss_set_profile(profile_psid, PF_ID_INCGENTIME, &pf_value);
		pf_value.u.incexptime = 0;
		(void)wss_set_profile(profile_psid, PF_ID_INCEXPTIME, &pf_value);
		if( profile_psid == WSA_PROFILE_PSID){
		  	pf_value.u.incgenloc = 1;
		}
		else{
			pf_value.u.incgenloc = 0;
		}
		(void)wss_set_profile(profile_psid, PF_ID_INCGENLOC, &pf_value);
		pf_value.u.transcertchainint = 1000;	// for 10s
		(void)wss_set_profile(profile_psid, PF_ID_TRANSCERTCHAININT, &pf_value);
		pf_value.u.certchainlen = 1;
		(void)wss_set_profile(profile_psid, PF_ID_CERTCHAINLEN, &pf_value);
		pf_value.u.fastverflag = demo_paras.sig_type;
		(void)wss_set_profile(profile_psid, PF_ID_FASTVERFLAG, &pf_value);
		pf_value.u.signalg = demo_paras.sig_alg;
		(void)wss_set_profile(profile_psid, PF_ID_SIGNALG, &pf_value);
		pf_value.u.ecpointformat = demo_paras.point_type;
		(void)wss_set_profile(profile_psid, PF_ID_ECPOINTFORMAT, &pf_value);
		pf_value.u.verflag = demo_paras.ver_flag;
		(void)wss_set_profile(profile_psid, PF_ID_VERFLAG, &pf_value);
		pf_value.u.checkgentime = 1;
		(void)wss_set_profile(profile_psid, PF_ID_CHECKGENTIME, &pf_value);
		pf_value.u.checkexptime = 0;
		(void)wss_set_profile(profile_psid, PF_ID_CHECKEXPTIME, &pf_value);
		if( profile_psid == WSA_PROFILE_PSID){
		  	pf_value.u.checkgenloc = 1;
		}
		else{
			pf_value.u.checkgenloc = 0;
		}
		(void)wss_set_profile(profile_psid, PF_ID_CHECKGENLOC, &pf_value);
		if( profile_psid == WSA_PROFILE_PSID){
		  	pf_value.u.checkreplay = 0;
		}
		else{
			pf_value.u.checkreplay = 1;
		}
		(void)wss_set_profile(profile_psid, PF_ID_CHECKREPLAY, &pf_value);
		pf_value.u.msgvalperiod = 60.0;
		(void)wss_set_profile(profile_psid, PF_ID_MSGVALPERIOD, &pf_value);
		pf_value.u.msgvaldis = 500.0;
		(void)wss_set_profile(profile_psid, PF_ID_MSGVALDIS, &pf_value);
		if( profile_psid == WSA_PROFILE_PSID){
		  	pf_value.u.encflag = 0;
		}
		else{
			pf_value.u.encflag = demo_paras.enc_flag;
		}
		(void)wss_set_profile(profile_psid, PF_ID_ENCFLAG, &pf_value);
	}
  	return ret;
}

void print_sds(char pri_flag){
  	unsigned int cert_pool_used_num, key_size, i, j;
	int ret;
	struct cert_info cert;

	if( pri_flag == 1){
	  	(void)wss_get_pri_pool_used_num(&cert_pool_used_num);
	}
	else{
	  	(void)wss_get_pub_pool_used_num(&cert_pool_used_num);
	}
	for(i = 0; i < cert_pool_used_num; i++){
	  	if( pri_flag == 1){
		  	ret = wss_get_pri_cert_info(i, &cert);
	  	}
		else{
		  	ret = wss_get_pub_cert_info(i, &cert);
		}
	  	if( ret != 0){
		  	(void)printf("get cert info of index %d failed, ret code %d\n", i, ret);
		}
	  	else{
			(void)printf("  Index %d:\n", i);
			(void)printf("  Subject type:	%d\n", cert.sub_info.subject_type);
			(void)printf("  PSID: ");
			if( cert.sub_info.psid_num == 0){
			  	(void)printf("All");
			}
			else{
				for(j = 0; j < cert.sub_info.psid_num; j++){
				  	(void)printf("0x%x, ", cert.sub_info.psid[j]);
				}
			}
			(void)printf("\n");
			(void)printf("  Expiration: %x\n", cert.sub_info.expiration);
			(void)printf("  Start time: %x\n", cert.sub_info.start_time);
			(void)printf("  Digest: %02x%02x%02x%02x %02x%02x%02x%02x\n",
			    cert.digest[0], cert.digest[1], cert.digest[2], cert.digest[3],
			    cert.digest[4], cert.digest[5], cert.digest[6], cert.digest[7]);
			(void)printf("  Issuer ID: %02x%02x%02x%02x %02x%02x%02x%02x\n",
			    cert.issuer_id[0], cert.issuer_id[1], cert.issuer_id[2], cert.issuer_id[3],
			    cert.issuer_id[4], cert.issuer_id[5], cert.issuer_id[6], cert.issuer_id[7]);
			(void)printf("  Signing algorithm: %d\n", cert.signing_alg);
			if( cert.signing_alg == 0){
			  	key_size = 28;
			}
			else{
			  	key_size = 32;
			}
			(void)printf("  CRL series: %08x\n", cert.crl_series);
			(void)printf("  Verification key:\n");
			(void)printf("    %02x ", cert.verification_key[0]);
			for(j = 0; j < key_size; j++){
			  	(void)printf("%02x", cert.verification_key[j+1]);
			  	if( (j+1) % 4 == 0){
				  	(void)printf(" ");
			  	}
				if( ((j+1) % 16) == 0){
				  	(void)printf("\n       ");
				}
			}
			(void)printf("\n");
			(void)printf("  Encryption key:\n");
			(void)printf("    %02x ", cert.encryption_key[0]);
			for(j = 0; j < 32; j++){
			  	(void)printf("%02x", cert.encryption_key[j+1]);
			  	if( (j+1) % 4 == 0){
				  	(void)printf(" ");
			  	}
				if( ((j+1) % 16) == 0){
				  	(void)printf("\n       ");
				}
			}
			(void)printf("\n");
			(void)printf("  Certificate string: length = %d\n", cert.cert_str_len);
			(void)printf("    ");
			for(j = 0; j < cert.cert_str_len; j++){
			  	(void)printf("%02x", cert.cert_str[j]);
			  	if( (j+1) % 4 == 0){
				  	(void)printf(" ");
			  	}
				if( ((j+1) % 16) == 0){
				  	(void)printf("\n    ");
				}
			}
			(void)printf("\n\n");
	  	}
	}
	return;
}

void print_dot2_demo_para(void){
	(void)printf("usage: dot2_demo <data_type> <sign> <sign_data_type> <attach_signer_type> <enc> <verify> <sign_alg> <sig_type> <ecc_point_type>\n");
	(void)printf("<data_type>: %d: wsmp; %d: wsa\n", DATA_TYPE_WSMP, DATA_TYPE_WSA);
	(void)printf("<sign>: 0: not sign; 1: sign\n");
	(void)printf("  [ONLY APPLY FOR SIGNED WSMP]\n");
	(void)printf("  <sign_data_type>: 0: payload; 1: partial payload; 2: external payload\n");
	(void)printf("  <attach_signer_type>: 0: digest; 1: cert; 2: cert chain\n");
	(void)printf("<enc>: 0: not encrypt; 1: encrypt; [ONLY FOR WSMP]\n");
	(void)printf("<verify>: 0: not verify; 1: verify\n");
	(void)printf("<sign_alg>: 0: ECDSA-224; 1: ECDSA-256\n");
	(void)printf("<sig_type>: 0/1: fast ver with uncompressed/compressed point; 2: only x\n");
	(void)printf("<ecc_point_type>: 0: uncompressed point; 1: compressed point\n");
}

int cmd_dot2_demo(cli_instance_t *cli, const char *command, char *argv[], int argc){
  	(void)cli;
	(void)command;
	int data_type = DATA_TYPE_WSMP, i;
  	ULONG trv;

	if( argc < 9){
	  	print_dot2_demo_para();
	}
	else{
	  	i = i_atoi(argv[0]);
		if( i != DATA_TYPE_WSMP && i != DATA_TYPE_WSA){
		  	data_type = DATA_TYPE_WSMP;
		}
		else{
		  	data_type = i;
		}
	  	i = i_atoi(argv[1]);
		if( i != 0 && i != 1){
		  	demo_paras.sign_flag = 0;
		}
		else{
		  	demo_paras.sign_flag = i;
		}
		i = i_atoi(argv[2]);
		if( i != 0 && i != 1 && i != 2){
		  	demo_paras.sign_data_type = 0;
		}
		else{
		  	demo_paras.sign_data_type = i;
		}
		i = i_atoi(argv[3]);
		if( i != 0 && i != 1 && i != 2){
		  	demo_paras.attach_signer_type = 0;
		}
		else{
		  	demo_paras.attach_signer_type = i;
		}
	  	i = i_atoi(argv[4]);
		if( i != 0 && i != 1){
		  	demo_paras.enc_flag = 0;
		}
		else{
		  	demo_paras.enc_flag = i;
		}
		i = i_atoi(argv[5]);
		if( i != 0 && i != 1){
		  	demo_paras.ver_flag = 0;
		}
		else{
		  	demo_paras.ver_flag = i;
		}
		i = i_atoi(argv[6]);
		if( i != 0 && i != 1){
		  	demo_paras.sig_alg = 1;	// ECDSA256
		}
		else{
		  	demo_paras.sig_alg = i;
		}
		i = i_atoi(argv[7]);
		if( i != 0 && i != 1 && i != 2){
		  	demo_paras.sig_type = 1;// fast verify with compressed point
		}
		else{
		  	demo_paras.sig_type = i;
		}
		i = i_atoi(argv[8]);
		if( i != 0 && i != 1){
		  	demo_paras.point_type = 1;	// compressed point
		}
		else{
		  	demo_paras.point_type = i;
		}

		// start thread
		struct timeval sleep_s;
		(void)gettimeofday(&sleep_s, NULL);
		if( (sleep_s.tv_sec - 1072915234) < 5){
		  	(void)printf("Waiting system time be after dot2 start time...\n");
		}
		while( (sleep_s.tv_sec - 1072915234) < 5){
			(void)usleep(1000000);
			(void)gettimeofday(&sleep_s, NULL);
		}

		done_flag = 0;
		if( data_type == DATA_TYPE_WSMP){
			(void)set_demo_psid(WSMP_PSID);
			trv = tx_thread_create(&dot2_demo_thread,
			    			"dot2_demo_thread",
						dot2_demo_wsmp_entry, 0,
						dot2_demo_thread_stack,
						sizeof(dot2_demo_thread_stack),
						DOT2_DEMO_THREAD_PRIORITY,
						DOT2_DEMO_THREAD_PRIORITY,
						TX_NO_TIME_SLICE, TX_AUTO_START);
		}
		else{
			(void)set_demo_psid(WSA_PSID);
			trv = tx_thread_create(&dot2_demo_thread,
			    			"dot2_demo_thread",
						dot2_demo_wsa_entry, 0,
						dot2_demo_thread_stack,
						sizeof(dot2_demo_thread_stack),
						DOT2_DEMO_THREAD_PRIORITY,
						DOT2_DEMO_THREAD_PRIORITY,
						TX_NO_TIME_SLICE, TX_AUTO_START);
		}
		if (trv != TX_SUCCESS) {
		  	(void)fprintf(stderr, "tx_thread_create failed, trv=0x%lx\n", trv);
#if 0
			abort();
#endif
		}
		else{
			//	wait execution finished and removing testing thread
			while( done_flag == 0){
				(void)usleep(500000);
			}

			trv = tx_thread_terminate(&dot2_demo_thread);
			if (trv != TX_SUCCESS) {
			  	(void)fprintf(stderr, "tx_thread_terminate failed, trv=0x%lx\n", trv);
			}
			trv = tx_thread_delete(&dot2_demo_thread);
			if (trv != TX_SUCCESS) {
			  	(void)fprintf(stderr, "tx_thread_delete failed, trv=0x%lx\n", trv);
			}
			(void)printf("Dot2 demo thread was removed.\n");
		}
	}
	return CLI_OK;
}

static void dot2_demo_wsmp_entry(ULONG input){
  	(void)input;

  	char id_tmp[8], input_str[100];
	int input_str_len, i;
	unsigned int current_used_cert_index, cert_pool_used_num;
	struct encode_in encode_in_str;
	struct encode_out encode_out_str;
	struct decode_in decode_in_str;
	struct decode_out decode_out_str;
	struct timeval tv1;
	int ret = 0;

  	if( set_up_profile() == -1){
	  	(void)printf("[1609.2/error] Set up profile failed.\n");
	  	ret = -1;
	}
  	else if( demo_paras.sign_flag == 1 || demo_paras.enc_flag == 1){
		if( set_up_sds(DATA_TYPE_WSMP) == -1){
		  	(void)printf("[1609.2/error] Set up SDS failed.\n");
		  	ret = -1;
		}
	}
  	else{
  		// setup profile & sds ok
  	}
#if 0
	load_usdot_roots();
#endif

	if( ret == 0){
		(void)gettimeofday(&tv1, NULL);
		srand(tv1.tv_sec);

		(void)wss_get_cur_pri_pool_ind(&current_used_cert_index);
		(void)wss_get_pri_pool_used_num(&cert_pool_used_num);
		current_used_cert_index = (current_used_cert_index % (cert_pool_used_num - 1)) + 1;
		(void)wss_set_cur_pri_pool_ind(current_used_cert_index);

		(void)memset(input_str, 0, sizeof(input_str));
		input_str_len = rand() % 100;
		for( i = 0; i < input_str_len; i++){
		  	input_str[i] = rand() % 256;
		}
		(void)printf("\n");
		(void)printf("Testing msg [length = %d bytes]:\n", input_str_len);
		dump(input_str_len, (unsigned char*)input_str);
		(void)printf("\n");

		(void)memset(&encode_in_str, 0, sizeof(encode_in_str));
		(void)memset(&encode_out_str, 0, sizeof(encode_out_str));
		(void)memset(&decode_in_str, 0, sizeof(decode_in_str));
		(void)memset(&decode_out_str, 0, sizeof(decode_out_str));
		encode_in_str.msg_len = input_str_len;
		(void)memcpy(encode_in_str.msg, input_str, input_str_len);

		if( demo_paras.sign_data_type == 1 || demo_paras.sign_data_type == 2){
		  	encode_in_str.ext_msg_len = input_str_len;
			(void)memcpy(encode_in_str.ext_msg, input_str, input_str_len);
		}
		if( demo_paras.sign_data_type == 2){
		  	encode_in_str.msg_len = 0;
		}

		encode_in_str.psid = demo_psid;
		encode_in_str.ssp_len = 0;
		(void)gettimeofday(&tv1, NULL);
		encode_in_str.expiry = (unsigned long long)(tv1.tv_sec - 1072915234 + MSGVALPERIOD_DEFAULT * 2)*1000000 + tv1.tv_usec;
		encode_in_str.attach_cert_type = demo_paras.attach_signer_type;

		(void)wss_get_pri_cert_digest(current_used_cert_index, (char*)id_tmp);
		encode_in_str.recp_num = 1;
		(void)memcpy(encode_in_str.recp_cert, id_tmp, 8);
		// set location to (24.7776950, 121.0435533) and altitude 106.3m (1063 = 0x427)
		encode_in_str.latitude = 247776950;
		encode_in_str.longitude = 1210435533;
		encode_in_str.elevation[0] = 0x04;
		encode_in_str.elevation[1] = 0x27;

		if( wss_encode_sec_msg(&encode_in_str, &encode_out_str) !=0){
		  	(void)printf("Encode msg failed, return code = %d:\n\tsign return code = %d\n\tencrypt return code = %d\n",
			    encode_out_str.ret_code, encode_out_str.encode_log.sign_ret_code, encode_out_str.encode_log.enc_ret_code);
		  	ret = -1;
		}
		else{
			(void)printf("1609.2 Message:\n");
			dump(encode_out_str.msg_len, (unsigned char*)encode_out_str.msg);
			(void)printf("\n");
		}

		if( ret == 0){
			// continue to test decoding
			(void)memcpy(decode_in_str.msg, encode_out_str.msg, encode_out_str.msg_len);
			decode_in_str.msg_len = encode_out_str.msg_len;
			decode_in_str.external_msg_len = input_str_len;
			(void)memcpy(decode_in_str.external_msg, input_str, input_str_len);
			decode_in_str.psid = encode_in_str.psid;
			// set location to (24.7776950, 121.0435533) and altitude 106.3m (1063 = 0x427)
			decode_in_str.latitude = 247776950;
			decode_in_str.longitude = 1210435533;
			decode_in_str.elevation[0] = 0x04;
			decode_in_str.elevation[1] = 0x27;

			if( wss_decode_sec_msg(&decode_in_str, &decode_out_str) != 0){
			  	(void)printf("Extract msg failed, return code = %d:\n\tdata extract return code = %d\n\tverify return code = %d\n",
				    decode_out_str.ret_code, decode_out_str.decode_log.de_ret_code, decode_out_str.decode_log.ver_ret_code);
			  	ret = -1;
			}
			else{
				if( decode_out_str.decode_log.ver_ret_code != 0){
				  	(void)printf("Verify failed, return code = %d\n", decode_out_str.decode_log.ver_ret_code);
				}
				(void)printf("Extracted msg:\n");
				dump(decode_out_str.msg_len, (unsigned char*)decode_out_str.msg);
				(void)printf("\n");
			}
		}
	}

	(void)printf("Bye\n");
	done_flag = 1;
}

static void dot2_demo_wsa_entry(ULONG input){
  	(void)input;

  	char input_str[100];
	int input_str_len, i;
	struct encode_sec_wsa_pack encode_in_str;
	struct encode_sec_wsa_result encode_out_str;
	struct decode_sec_wsa_pack decode_in_str;
	struct decode_sec_wsa_result decode_out_str;
	struct timeval tv;
	int ret = 0;

  	if( set_up_profile() == -1){
	  	(void)printf("[1609.2/error] Set up profile failed.\n");
	  	ret = -1;
	}
  	else if( demo_paras.sign_flag == 1){
		if( set_up_sds(DATA_TYPE_WSA) == -1){
		  	(void)printf("[1609.2/error] Set up SDS failed.\n");
		  	ret = -1;
		}
	}
  	else{
  		// setup profile & sds ok
  	}
#if 0
	load_usdot_roots();
#endif

	if( ret == 0){
		(void)gettimeofday(&tv, NULL);
		srand(tv.tv_sec);

		(void)memset(input_str, 0, sizeof(input_str));
		input_str_len = rand() % 100;
		for( i = 0; i < input_str_len; i++){
		  	input_str[i] = rand() % 256;
		}
		(void)printf("\n");
		(void)printf("Testing msg [length = %d bytes]:\n", input_str_len);
		dump(input_str_len, (unsigned char*)input_str);
		(void)printf("\n");

		(void)memset(&encode_in_str, 0, sizeof(encode_in_str));
		(void)memset(&encode_out_str, 0, sizeof(encode_out_str));
		(void)memset(&decode_in_str, 0, sizeof(decode_in_str));
		(void)memset(&decode_out_str, 0, sizeof(decode_out_str));

		encode_in_str.wsa_data_len = input_str_len;
		(void)memcpy(encode_in_str.wsa_data, input_str, input_str_len);

		encode_in_str.permission_num = 1;
		encode_in_str.psid[0] = demo_psid;
		encode_in_str.lifetime = 60000000;
		// set location to (24.7776950, 121.0435533) and altitude 106.3m (1063 = 0x427)
		encode_in_str.latitude = 247776950;
		encode_in_str.longitude = 1210435533;
		encode_in_str.elevation[0] = 0x04;
		encode_in_str.elevation[1] = 0x27;

		if( wss_encode_sec_wsa(&encode_in_str, &encode_out_str) !=0){
		  	(void)printf("Encode wsa failed, return code = %d:\n\tsign return code = %d\n",
			    encode_out_str.ret_code, encode_out_str.sign_ret_code);
		  	ret = -1;
		}
		else{
			(void)printf("1609.2 WSA Message:\n");
			dump(encode_out_str.signed_wsa_len, (unsigned char*)encode_out_str.signed_wsa);
			(void)printf("\n");
		}

		if( ret == 0){
			// continue to test decoding
			(void)memcpy(decode_in_str.signed_wsa, encode_out_str.signed_wsa, encode_out_str.signed_wsa_len);
			decode_in_str.signed_wsa_len = encode_out_str.signed_wsa_len;
			// set location to (24.7776950, 121.0435533) and altitude 106.3m (1063 = 0x427)
			decode_in_str.latitude = 247776950;
			decode_in_str.longitude = 1210435533;
			decode_in_str.elevation[0] = 0x04;
			decode_in_str.elevation[1] = 0x27;

			if( wss_decode_sec_wsa(&decode_in_str, &decode_out_str) != 0){
			  	(void)printf("Extract wsa failed, return code = %d:\n\tdata extract return code = %d\n\tverify return code = %d\n",
				    decode_out_str.ret_code, decode_out_str.de_ret_code, decode_out_str.ver_ret_code);
			  	ret = -1;
			}
			else{
				if( decode_out_str.ver_ret_code != 0){
				  	(void)printf("Verify failed, return code = %d\n", decode_out_str.ver_ret_code);
				}
				(void)printf("Extracted msg:\n");
				dump(decode_out_str.wsa_data_len, (unsigned char*)decode_out_str.wsa_data);
				(void)printf("\n");
			}
		}
	}

	(void)printf("Bye\n");
	done_flag = 1;
}

int cmd_dot2_show_sds(cli_instance_t *cli, const char *command, char *argv[], int argc){
  	(void)cli;
	(void)command;
	(void)argv;
	(void)argc;

	// print current SDS status
	(void)printf("Dump private cert cache info:\n");
	print_sds(1);
	(void)printf("\n");
	(void)printf("Dump public cert cache info:\n");
	print_sds(0);
	(void)printf("\n");

	// print profile status
	unsigned int profile_num, psids[8], i;
	if( wss_query_profile(&profile_num, psids) == 0){
	  	(void)printf("Dump profile cache info:\n");
		(void)printf("PSID:\n");
	  	for(i = 0; i < profile_num; i++){
		  	(void)printf("0x%8x\n", psids[i]);
		}
		(void)printf("\n");
	}
	else{
	  	(void)printf("Profile query failed\n");
	}

	return CLI_OK;
}

int cmd_test_us_sec(cli_instance_t *cli, const char *command, char *argv[], int argc){
  	(void)cli;
	(void)command;
  	ULONG trv;

	if( argc < 5){
	  	printf("usage: test_us_sec <psid> <sign_flag> <verify_flag> <msg_len> <repeat>\n");
		printf("<psid>: \tpsid to be encoded if <sign_flag> is set\n");
		printf("<sign_flag>:\t0: not sign; 1: sign\n");
		printf("<verify_flag>:\t0: only decode, not verify; 1: decode and verify\n");
		printf("<msg_len>:\tlength of testing message, 1~256\n");
		printf("<repeat>:\t# of testing rounds\n");
	}
	else{
	  	demo_psid = atoi(argv[0]);
		demo_paras.sign_flag = atoi(argv[1]);
		demo_paras.ver_flag = atoi(argv[2]);
		simple_test_msg_len = atoi(argv[3]);
		repeat = atoi(argv[4]);
		if( simple_test_msg_len < 1 || simple_test_msg_len > 256){
		  	simple_test_msg_len = 100;
		}
		if( repeat < 0){
		  	repeat = 15;
		}
		demo_paras.sign_data_type = 0;
		demo_paras.attach_signer_type = 0;
		demo_paras.sig_alg = 1;
		demo_paras.sig_type = 1;
		demo_paras.enc_flag = 0;
		demo_paras.point_type = 0;

		done_flag = 0;
		trv = tx_thread_create(&dot2_demo_thread,
		    			"dot2_demo_thread",
					dot2_simple_test_entry, 0,
					dot2_demo_thread_stack,
					sizeof(dot2_demo_thread_stack),
					DOT2_DEMO_THREAD_PRIORITY,
					DOT2_DEMO_THREAD_PRIORITY,
					TX_NO_TIME_SLICE, TX_AUTO_START);
		if (trv != TX_SUCCESS) {
		  	(void)fprintf(stderr, "tx_thread_create failed, trv=0x%lx\n", trv);
		}
		else{
			// wait execution finished and removing testing thread
			while( done_flag == 0){
				(void)usleep(500000);
			}
			trv = tx_thread_terminate(&dot2_demo_thread);
			if (trv != TX_SUCCESS) {
			  	(void)fprintf(stderr, "tx_thread_terminate failed, trv=0x%lx\n", trv);
			}
			trv = tx_thread_delete(&dot2_demo_thread);
			if (trv != TX_SUCCESS) {
			  	(void)fprintf(stderr, "tx_thread_delete failed, trv=0x%lx\n", trv);
			}
		}
	}
	return CLI_OK;
}

static void dot2_simple_test_entry(ULONG input){
  	(void)input;
  	int ret = 0, i, input_str_len = simple_test_msg_len, round;
  	unsigned char input_str[256];
	struct timeval a, b, diff;
	struct encode_in encode_in_str;
	struct encode_out encode_out_str;
	struct decode_in decode_in_str;
	struct decode_out decode_out_str;

	(void)gettimeofday(&a, NULL);
	srand(a.tv_sec);
  	if( set_up_profile() == -1){
	  	(void)printf("[1609.2/error] Set up profile failed.\n");
	  	ret = -1;
	}
	else if( demo_paras.sign_flag == 1 ){
		if( set_up_sds(DATA_TYPE_WSMP) == -1){
		  	(void)printf("[1609.2/error] Set up SDS failed.\n");
		  	ret = -1;
		}
	}
	else{
 		// setup profile & sds ok
	}

	round = 0;
	while( ret == 0 && round < repeat){
		usleep(100000);
		(void)memset(input_str, 0, sizeof(input_str));
		for( i = 0; i < input_str_len; i++){
		  	input_str[i] = rand() % 256;
		}
	  	printf("Test msg (round %d):\n", round+1);
		(void)dump(input_str_len, input_str);
		printf("\n");
		// encode msg
		(void)memset(&encode_in_str, 0, sizeof(encode_in_str));
		(void)memset(&encode_out_str, 0, sizeof(encode_out_str));
		(void)memset(&decode_in_str, 0, sizeof(decode_in_str));
		(void)memset(&decode_out_str, 0, sizeof(decode_out_str));
		encode_in_str.msg_len = input_str_len;
		(void)memcpy(encode_in_str.msg, input_str, input_str_len);
		encode_in_str.psid = demo_psid;
		encode_in_str.attach_cert_type = demo_paras.attach_signer_type;

		gettimeofday(&a, NULL);
		ret = wss_encode_sec_msg(&encode_in_str, &encode_out_str);
		gettimeofday(&b, NULL);
		timersub(&b, &a, &diff);
		printf("dot2_encode_msg: %ld.%03ld ms\n", 
		    	diff.tv_sec*1000 + diff.tv_usec/1000, 
			diff.tv_usec%1000);
		if( ret == 0){
		  	printf("Encoded 1609.2 data:\n");
			(void)dump(encode_out_str.msg_len, encode_out_str.msg);
			printf("\n");
		}
		else{
		  	(void)printf("Encode msg failed, ret code = %d:\n", encode_out_str.ret_code);
			(void)printf("\tsign ret code = %d\n", encode_out_str.encode_log.sign_ret_code);
			(void)printf("\tencrypt ret code = %d\n", encode_out_str.encode_log.enc_ret_code);
			ret = -1;
		}
		if( ret == 0){
		  	// decode msg
			(void)memcpy(decode_in_str.msg, encode_out_str.msg, encode_out_str.msg_len);
			decode_in_str.msg_len = encode_out_str.msg_len;
			decode_in_str.psid = encode_in_str.psid;

			gettimeofday(&a, NULL);
			ret = wss_decode_sec_msg(&decode_in_str, &decode_out_str);
			gettimeofday(&b, NULL);
			timersub(&b, &a, &diff);
			printf("dot2_decode_msg: %ld.%03ld ms\n", 
			    	diff.tv_sec*1000 + diff.tv_usec/1000, 
				diff.tv_usec%1000);
			if( ret == 0){
			  	// decode ok
				if( decode_out_str.decode_log.ver_ret_code != 0){
				  	(void)printf("Verify failed, ret code = %d\n", decode_out_str.decode_log.ver_ret_code);
				}
				printf("Decoded 1609.2 data:\n");
				(void)dump(decode_out_str.msg_len, decode_out_str.msg);
				printf("\n");		  	
			}
			else{
			  	(void)printf("Extract msg failed, ret code = %d:\n", decode_out_str.ret_code);
				(void)printf("\tdata extract ret code = %d\n", decode_out_str.decode_log.de_ret_code);
				(void)printf("\tverify ret code = %d\n", decode_out_str.decode_log.ver_ret_code);
			  	ret = -1;
			}
		}
		round++;
	}
	done_flag = 1;
}

