#ifndef __DOT2_API_H__
#define __DOT2_API_H__

#define DOT2_TIME_SHIFT_VERSION_2 1072915234

/************************************************
 * Profile related				*
 ************************************************/
#define WSA_PROFILE_PSID 1

#define USEDOT2_DEFAULT 1
#define DOT2_VERSION_DEFAULT 2
#define SIGNFLAG_DEFAULT 0
#define INCGENTIME_DEFAULT 0
#define INCEXPTIME_DEFAULT 0
#define INCGENLOC_DEFAULT 0
#define TRANSCERTCHAININT_DEFAULT 1000	// in ms
#define SIGNERTYPE_DEFAULT 1		// 0: certificate digest, 1: certificate, 2: certificate chain
#define CERTCHAINLEN_DEFAULT -1
#define ENCFLAG_DEFAULT 0
#define FASTVERFLAG_DEFAULT 1		// 0: uncompressed, 1: compressed, 2: no
#define ECPOINTFORMAT_DEFAULT 1		// 0: uncompressed, 1: compressed

#define VERFLAG_DEFAULT 0
#define CHECKGENTIME_DEFAULT 0
#define GENTIMESRC_DEFAULT 0
#define CHECKEXPTIME_DEFAULT 0
#define EXPTIMESRC_DEFAULT 0
#define CHECKGENLOC_DEFAULT 0
#define GENLOCSRC_DEFAULT 0
#define CHECKREPLAY_DEFAULT 0
#define MSGVALPERIOD_DEFAULT 5.0
#define MSGVALDIS_DEFAULT 10.0
#define GENTIMECONFMUL_DEFAULT 0.0
#define GENLOCCONFMUL_DEFAULT 0.0
#define CRLTIMETOLERANCE_DEFAULT 600
#define ACCPENCMSG_DEFAULT 0
#define ALLOWEXPDECCERT_DEFAULT 1

#define SIGNALG_DEFAULT 1
#define ENCALG_DEFAULT 2
#define PKTRANSTYPE_DEFAULT 0
#define NOT_USE_CERT_CHAIN_DEFAULT 0

typedef enum{
	PF_ID_USEDOT2,			// use dot2
	PF_ID_DOT2_VERSION,		// dot2 version
	PF_ID_SIGNFLAG,			// sign flag
	PF_ID_INCGENTIME,		// include generation time in header
	PF_ID_INCEXPTIME,		// include expiration time in header
	PF_ID_INCGENLOC,		// include generation location in header
	PF_ID_TRANSCERTCHAININT,	// interval of tranmitting cert chain in ms
	PF_ID_CERTCHAINLEN,		// cert chain length
	PF_ID_ENCFLAG,			// encryption flag
	PF_ID_FASTVERFLAG,		// fast verification
	PF_ID_ECPOINTFORMAT,		// ecc point format

	PF_ID_VERFLAG,			// verification flag
	PF_ID_CHECKGENTIME,		// check generation time
	PF_ID_GENTIMESRC,		// generation time source
	PF_ID_CHECKEXPTIME,		// check expiration time
	PF_ID_EXPTIMESRC,		// expiration time source
	PF_ID_CHECKGENLOC,		// check generation location
	PF_ID_GENLOCSRC,		// generation location source
	PF_ID_CHECKREPLAY,		// check replay
	PF_ID_MSGVALPERIOD,		// message valid period
	PF_ID_MSGVALDIS,		// message valid distance
	PF_ID_GENTIMECONFMUL,		// generation time confidence multiplier
	PF_ID_CRLTIMETOLERANCE,		// CRL time of tolerance

	PF_ID_SIGNALG,			// signing algorithm
	PF_ID_ENCALG,			// encryption algorithm
	PF_ID_PKTRANSTYPE,		// public key transfer type
}profile_id;

typedef struct{
  	union{
		char	usedot2;		// use dot2; 0: disable, 1: enable;
		char	dot2_version;		// dot2 version; 2: 1609.2 v2;
		char	signflag;		// sign flag; 
						// 0: not sign, 
						// 1: always sign, 
						// 2: adaptive;
		char	incgentime;		// include generation time in header; 
						// 0: exclude, 1: include;
		char	incexptime;		// include expiration time in header; 
						// 0: exclude, 1: include;
		char	incgenloc;		// include generation location in header; 
						// 0: exclude, 1: include;
		unsigned int	transcertchainint;	
						// interval of tranmitting cert chain in ms;
		int	certchainlen;		// cert chain length; # of cert in chain
		char	encflag;		// encryption flag; 
						// 0: not encrypt, 
						// 1: always encrypt, 
						// 2: adaptive;
		char	fastverflag;		// fast verification;
						// 0: uncompressed,
						// 1: compressed,
						// 2: only x-coordinate; 
		char	ecpointformat;		// ecc point format;
						// 0: uncompressed,
						// 1: compressed;

		char	verflag;		// verification flag;
						// 0: not verify,
						// 1: always verify,
						// 2: adaptive;
		char	checkgentime;		// check generation time;
						// 0: not check,
						// 1: check;
		char	gentimesrc;		// generation time source;
						// 0: header,
						// 1: payload; (not support)
		char	checkexptime;		// check expiration time;
						// 0: not check,
						// 1: check;
		char	exptimesrc;		// expiration time source;
						// 0: header,
						// 1: payload; (not support)
		char	checkgenloc;		// check generation location;
						// 0: not check,
						// 1: check;
		char	genlocsrc;		// generation location source;
						// 0: header,
						// 1: payload; (not support)
		char	checkreplay;		// check replay;
						// 0: not check,
						// 1: check;
		double	msgvalperiod;		// message valid period; in seconds;
		double	msgvaldis;		// message valid distance; in meters;
		double	gentimeconfmul;		// generation time confidence multiplier;
		int	crltimetolerance;	// crl time of tolerance; in seconds;

		char	signalg;		// signing algorithm;
						// 0: ECDSA-224,
						// 1: ECDSA-256;
		char	encalg;			// encryption algorithm;
						// 2: ECIES-256;
		char	pktranstype;		// public key transfer type;
						// 0: explicit,
						// 1: implict,
						// 2: both;
	}u;
}profile_value_type;

int wss_new_profile(unsigned int psid);
int wss_del_profile(unsigned int psid);
int wss_set_profile(unsigned int psid, profile_id pf_id, profile_value_type *value);
int wss_get_profile(unsigned int psid, profile_id pf_id, profile_value_type *value);
int wss_query_profile(unsigned int *avail_psids_num, unsigned int *avail_psids);

/************************************************
 * Certificate related				*
 ************************************************/
#define CERT_VERSION_1 1
#define CERT_VERSION_2 2
#define MAX_NUM_OF_PERMISSIONS 8
#define CERT_DIGEST_BYTES 8

typedef enum{
  	CERT_SUCCESS = 0,
	CERT_PROFILE_ERR = 1,
	CERT_STR_EXCEED_MAX = 2,
	CERT_UNSUPPORT_TYPE = 3,
	CERT_NO_ROOT_CA = 4,
	CERT_GEN_ERR = 5,
	CERT_ADD_SDS_ERR = 6,
	CERT_UNSUPPORT_VERSION = 7,
	CERT_TOO_MANY_PSIDS = 8,
	CERT_EXPIRATION_IN_PAST = 9,
	CERT_START_AFTER_EXPIRATION = 10,
	RESERVED_CERT_RET_CODE
}enum_cert_ret_code;

struct cert_fields{
  	char version;
	unsigned char subject_type;
	unsigned int psid_num;
	unsigned int psid[MAX_NUM_OF_PERMISSIONS];
	unsigned int expiration;	// express time in 1609.2
	unsigned int start_time;
};

int wss_create_cert(struct cert_fields *cert, unsigned char *cert_str, unsigned int *cert_len, unsigned int *ecdsa_pri_index, unsigned int *ecies_pri_index);

/************************************************
 * SDS related					*
 ************************************************/
#define MAX_CERT_STR_LENGTH 256
#define MAX_CERT_LENGTH (MAX_CERT_STR_LENGTH*4)
#define MAX_CERT_CHAIN_NUM 4
#define MAX_COMPRESSED_PUBLIC_KEY_BYTES 33

typedef enum{
  	SDS_SUCCESS = 0,
	SDS_API_ERR = 1,
	SDS_INDEX_OUT_OF_RANGE = 2,
	SDS_GET_VERTIFICATION_KEY_FAIL = 3,
	SDS_GET_ENCRYPTION_KEY_FAIL = 4,
	SDS_GET_CERT_STR_FAIL = 5,
	RESERVED_SDS_RET_CODE
}enum_sds_ret_code;

struct cert_info{
  	struct cert_fields sub_info;
	char digest[CERT_DIGEST_BYTES];
	char issuer_id[CERT_DIGEST_BYTES];
	unsigned char signing_alg;
	unsigned int crl_series;
	char verification_key[MAX_COMPRESSED_PUBLIC_KEY_BYTES];
	char encryption_key[MAX_COMPRESSED_PUBLIC_KEY_BYTES];
	char cert_str[MAX_CERT_LENGTH];
	unsigned int cert_str_len;
};

int wss_set_cur_pri_pool_ind(unsigned int value);
int wss_get_cur_pri_pool_ind(unsigned int *value);
int wss_get_cur_pub_pool_ind(unsigned int *value);
int wss_get_pri_pool_used_num(unsigned int *value);
int wss_get_pub_pool_used_num(unsigned int *value);
// ind start from 0
int wss_get_pri_cert_info(unsigned int ind, struct cert_info *cert);
int wss_get_pub_cert_info(unsigned int ind, struct cert_info *cert);
int wss_get_pri_cert_digest(unsigned int ind, char *digest);
int wss_get_pub_cert_digest(unsigned int ind, char *digest);

int wss_append_pri_cert_str(char *cert_str, unsigned int cert_len, unsigned int ecdsa_pri_index, unsigned int ecies_pri_index);
int wss_append_pri_cert_key_str(unsigned char *cert_str, unsigned int cert_len, unsigned char *ecdsa_pri_key, unsigned char *ecies_pri_key);
int wss_append_pub_cert_str(char *cert_str, unsigned int cert_len);

/************************************************
 * Message processing log related		*
 ************************************************/
struct encode_log_items{
	unsigned char attach_cert_type;
	char sign_cert_digest[CERT_DIGEST_BYTES];
	unsigned int sign_cert_str_len;
	char sign_cert_str[MAX_CERT_LENGTH];
	unsigned long long sign_cert_digest_val;
	unsigned long long msg_gen_time_us;
	unsigned char msg_gen_time_conf;
	unsigned int signer_cert_exp;
	unsigned char sign_ret_code;
	unsigned char enc_ret_code;
};

struct decode_log_items{
	unsigned char attach_cert_type;
	char sign_cert_digest[CERT_DIGEST_BYTES];
	unsigned int sign_cert_str_len;
	char sign_cert_str[MAX_CERT_LENGTH];
	unsigned long long sign_cert_digest_val;
	unsigned long long msg_gen_time_us;
	unsigned char msg_gen_time_conf;
	unsigned char de_ret_code;
	unsigned char ver_ret_code;
};

/************************************************
 * Message processing related			*
 ************************************************/
#define MAX_IN_MSG_LENGTH (MAX_CERT_LENGTH*MAX_CERT_CHAIN_NUM)
#define MAX_MSG_LENGTH (MAX_IN_MSG_LENGTH*2)
#define MAX_NUM_OF_ENC_RECP 4

typedef enum{
  	ENCODE_SUCCESS = 0,
	ENCODE_PROFILE_ERR = 1,
	ENCODE_STR_EXCEED_MAX = 2,
	ENCODE_SIGN_ERR = 3,
	ENCODE_ENC_ERR = 4,
	ENCODE_INVALID_INPUT = 5,
	RESERVED_ENCODE_RET_CODE
}enum_encode_ret_code;

struct encode_in{
  	unsigned int msg_len;
	char msg[MAX_IN_MSG_LENGTH];
	unsigned int ext_msg_len;
	char ext_msg[MAX_IN_MSG_LENGTH];
	char adaptive_sign;
	char adaptive_enc;
	unsigned int ssp_len;
	char ssp[MAX_CERT_STR_LENGTH];
	unsigned int psid;
	unsigned long long expiry;
	int latitude;			// used if needs to encode location information
	int longitude;			// used if needs to encode location information
	char elevation[2];		// used if needs to encode location information
	char attach_cert_type;		// 0: digest, 1: full cert, 2: cert chain
	unsigned int recp_num;
	char recp_cert[MAX_NUM_OF_ENC_RECP][CERT_DIGEST_BYTES];
};

struct encode_out{
  	unsigned char ret_code;
  	unsigned int msg_len;
	char msg[MAX_MSG_LENGTH];
	unsigned int failed_cert_num;
	char failed_recp[MAX_NUM_OF_ENC_RECP][CERT_DIGEST_BYTES];

	struct encode_log_items encode_log;
};

typedef enum{
  	DECODE_SUCCESS_NO_NEED_VER = 0,	// secure but no need verify (e.g. not signed data)
	DECODE_SUCCESS_NOT_VER = 1,	// signed but not verify yet
	DECODE_SUCCESS_VER = 2,		// signed and verified success
	DECODE_PROFILE_ERR = 3,
	DECODE_STR_EXCEED_MAX = 4,
	DECODE_DATA_EXTRACT_ERR = 5,
	DECODE_VER_ERR = 6,
	DECODE_INVALID_INPUT = 7,
	RESERVED_DECODE_RET_CODE
}enum_decode_ret_code;

struct decode_in{
  	unsigned int psid;
  	unsigned int msg_len;
	char msg[MAX_MSG_LENGTH];
	unsigned int external_msg_len;
	char external_msg[MAX_MSG_LENGTH];
	int latitude;		// used if needs to do location check
	int longitude;		// used if needs to do location check
	char elevation[2];	// used if needs to do location check
	char adaptive_ver;
};

struct decode_out{
  	unsigned char ret_code;
  	unsigned int msg_len;
	char msg[MAX_MSG_LENGTH];

	struct decode_log_items decode_log;
};

int wss_encode_sec_msg(struct encode_in *in_pack, struct encode_out *out_pack);
int wss_decode_sec_msg(struct decode_in *in_pack, struct decode_out *out_pack);

/************************************************
 * Secure WSA processing related		*
 ************************************************/
struct encode_sec_wsa_pack{
	unsigned int wsa_data_len;
	unsigned char wsa_data[MAX_IN_MSG_LENGTH];
	unsigned int permission_num;
	unsigned int psid[MAX_NUM_OF_PERMISSIONS];
	unsigned char priority[MAX_NUM_OF_PERMISSIONS];
//	unsigned char ssp_len[MAX_NUM_OF_PERMISSIONS];	// currently not used
//	unsigned char ssp[MAX_NUM_OF_PERMISSIONS][MAX_CERT_STR_LENGTH];	// currently not used
	unsigned long long lifetime;	// microseconds
	int latitude;			// used for encoding location information
	int longitude;			// used for encoding location information
	char elevation[2];		// used for encoding location information
};

struct encode_sec_wsa_result{
  	unsigned char ret_code;
  	unsigned int sign_ret_code;
  	unsigned int signed_wsa_len;
	unsigned char signed_wsa[MAX_MSG_LENGTH];
};

struct decode_sec_wsa_pack{
  	unsigned int signed_wsa_len;
	unsigned char signed_wsa[MAX_MSG_LENGTH];
	unsigned char secure_handle;
	int latitude;			// used for checking location information
	int longitude;			// used for checking location information
	char elevation[2];		// used for checking location information
};

struct decode_sec_wsa_result{
  	unsigned char ret_code;
	unsigned char de_ret_code;
	unsigned char ver_ret_code;
  	unsigned int wsa_data_len;
	unsigned char wsa_data[MAX_MSG_LENGTH];
};

int wss_encode_sec_wsa(struct encode_sec_wsa_pack *in_pack, struct encode_sec_wsa_result *out_pack);
int wss_decode_sec_wsa(struct decode_sec_wsa_pack *in_pack, struct decode_sec_wsa_result *out_pack);

#endif	/* __DOT2_API_H__ */
