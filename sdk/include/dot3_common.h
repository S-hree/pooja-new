#ifndef	__DOT3_COMMON_H__
#define	__DOT3_COMMON_H__

#include <stdint.h>
#include <sys/time.h>

/* EID Definition */
/*************************************************************************/
#define EID_SERVICE_INFO						1
#define EID_CHANNEL_INFO						2
#define EID_WRA									3
#define EID_TX_POWER							4
#define EID_2D_LOCATION							5
#define EID_3D_LOCATION							6
#define EID_ADVERTISER_ID						7
#define EID_PSC									8
#define EID_IPV6_ADDR							9
#define EID_IPV6_PORT							10
#define EID_PROVIDER_MAC						11
#define EID_EDCA_SET							12
#define EID_SECOND_DNS							13
#define EID_GATEWAY_MAC						14
#define EID_CHANNEL_NUMBER					15
#define EID_DATA_RATE							16
#define EID_REPEAT_RATE							17
#define EID_COUNTRY_STRING						18
#define EID_RCPI_THRESHOLD						19
#define EID_WSA_COUNT_THRESHOLD				20
#define EID_CHANNEL_ACCESS						21
#define EID_WSA_COUNT_THRESHOLD_INTERVAL	22
#define EID_WSMP								128
#define EID_WSMP_S								129
#define EID_WSMP_I								130
/*************************************************************************/


/* WSA Bit Mask Definition */
/*************************************************************************/
#define BITMASK_TX_POWER							(1 << 0)
#define BITMASK_2D_LOCATION						(1 << 1)
#define BITMASK_3D_LOCATION						(1 << 2)
#define BITMASK_ADVERTISER_ID						(1 << 3)
#define BITMASK_PSC									(1 << 4)
#define BITMASK_IPV6_ADDR							(1 << 5)
#define BITMASK_IPV6_PORT							(1 << 6)
#define BITMASK_PROVIDER_MAC						(1 << 7)
#define BITMASK_EDCA_SET							(1 << 8)
#define BITMASK_SECOND_DNS						(1 << 9)
#define BITMASK_GATEWAY_MAC						(1 << 10)
#define BITMASK_REPEAT_RATE						(1 << 11)
#define BITMASK_COUNTRY_STRING					(1 << 12)
#define BITMASK_RCPI_THRESHOLD					(1 << 13)
#define BITMASK_WSA_COUNT_THRESHOLD				(1 << 14)
#define BITMASK_CHANNEL_ACCESS					(1 << 15)
#define BITMASK_WSA_COUNT_THRESHOLD_INTERVAL	(1 << 16)
/*************************************************************************/


/* General Definition */
/*************************************************************************/
#define PSC_SIZE 31
#define SSP_SIZE 255
#define SSID_SIZE 16
#define IPV6_SIZE 16
#define MAC_SIZE 6
#define LOCATION_2D_SIZE 8
#define LOCATION_3D_SIZE 15
#define ADVERTISER_ID_SIZE 32
#define COUNTRY_STR_SIZE 3
#define DEFAULT_PRIORITY 32 
#define MAX_PRIORITY 63
#define WSA_MAX_ENTRY 2
#define ENTRY_SIZE 256
#define WSA_MAX_SIZE 2048
#define WSM_MAX_SIZE 1480
/*************************************************************************/

/* WME Handle Definiton */
typedef uint16_t wme_handle_t;

/* Return Code */
enum KERNEL_ERR{
	ACCEPT = 0,
	INVALID_VERSION,
	INVALID_ROLE,
	INVALID_PARAMETER,
	INVALID_HANDLE,
	SYSTEM_INTR, 
	UNSPEC,

	CONFLIT_INDEX,
	CONFLIT_PSID,
	OUT_OF_MEM,
	INDEX_NOT_FOUND,
	ENTRY_NOT_FOUND,
	ENTRY_NOT_MATCH,
	INDEX_FULL,
	MIB_NOT_FOUND,
	NO_DEVICE,
	NO_CHANNEL,
	BUSY_CHANNEL,
	TA_EXISTED,  /* every channel can only have a TA service */
	WME_KRN_ERR_MAX,
};


enum SERVICE_STATUS{
	STATUS_PENDING = 0,
	STATUS_SATISFIED = 1,
	STATUS_PARTIALLY_SATISFIED = 2,
	STATUS_AVAILABLE = 0,
	STATUS_ACTIVE = 1,
};

enum WME_ACTION {
	ADD,
	DELETE,
	CHANGE,
};

enum WME_EVENT{
	EVENT_SERVICE,
	EVENT_CHANNEL,
};


enum MIB_TYPE{
	PROVIDER_ENTRY,
	USER_ENTRY,
	//MDATA_ENTRY,
	//TA_ENTRY,
	//CCH_ENTRY,
	WSM_ENTRY,
	AVAILABLE_ENTRY,
};

enum WSA_CFG_ID {
	CFG_REPEAT_RATE,
	CFG_TRANSMIT_POWER,
	CFG_ADVERTISEMENT_ID,
	CFG_COUNTRY_STR,
	//CFG_LOCATION,
	//CFG_EDCA,
	CFG_ROUTER_LIFETIME,
	CFG_IP_PREFIX,
	CFG_IP_PREFIX_LEN,
	CFG_DEFAULT_GATEWAY,
	CFG_PRIMARY_DNS,
	CFG_SECONDARY_DNS,
	CFG_GATEWAY_MAC
};

enum WME_REASON{
	REASON_SERVICE_AVAILABLE,
	REASON_SERVICE_UNAVAILABLE,
	REASON_CHANNEL_AVAILABLE,
	REASON_CHANNEL_UNAVAILABLE,
};


/* WSMP Protocol */
struct sockaddr_wsmp
{
	unsigned short		wsmp_family;
	unsigned short		app_id;
	unsigned char		channel_number;  /* Only selecting the channel number, Class and Country are chosen by the system */
	unsigned char		data_rate;
	unsigned char		txpwr_level;
	signed char			user_priority;
	unsigned int		psid;
	unsigned long long	expiry_time;  /* Not supporting now */
	unsigned char		wsmp_addr[6];
	struct{
		unsigned char	channel:1,
						rate:1,
						power:1,
						reserved:5;
	}extensions;
	unsigned char		wsm_type;

	/* for recv only */
	signed char			rssi;
	unsigned short	wsm_len;
	struct timeval recv_time;
#if 1
	unsigned int	ex_header;
#endif
};


struct in_wsm
{
	unsigned char		version;
	unsigned char		channel_number;
	unsigned char		data_rate;
	unsigned char		txpwr_level;
	signed char		user_priority;  /* Not supproting now */
	unsigned int		psid;
	unsigned char		src_mac[MAC_SIZE];
	struct{
		unsigned char	channel:1,
					rate:1,
					power:1,
					reserved:5;
	} extensions;
	unsigned char		wsm_type;
	unsigned int		ex_header;
	signed char		rssi;
	unsigned char		real_channel;
	unsigned char 	data[WSM_MAX_SIZE];
	unsigned short	length;
	
};


struct out_wsm
{
	unsigned char		channel_number;  /* Only selecting the channel number, Class and Country are chosen by the system */
	unsigned char		data_rate;
	unsigned char		txpwr_level;
	signed char		user_priority;
	unsigned int		psid;
	unsigned long long	expiry_time;  /* Not supporting now */
	unsigned char		dest_mac[MAC_SIZE];
	struct{
		unsigned char	channel:1,
						rate:1,
						power:1,
						reserved:5;
	} extensions;
	unsigned char		wsm_type;
	unsigned int		ex_header;
	unsigned char 	data[WSM_MAX_SIZE];
	unsigned short	length;
	
};


enum ACCESS_MODE{
	MODE_CCH,
	MODE_BOTH = 1,  /* Defined by MIB */
	MODE_SCH = 2,  
	MODE_MAX,
};


struct mib_provider_service_info{
	unsigned char		dest_mac[MAC_SIZE];	
	unsigned char		wsa_type;  /* enum WSA_TYPE */
	unsigned int		psid;
	unsigned char		psc[PSC_SIZE];
	unsigned char		psc_len;
	unsigned char		service_priority;  /* 0 ~ 63 */
	unsigned char		service_specific_permissions[SSP_SIZE];
	unsigned char		service_specific_permissions_len;	
	unsigned char		security_service_id[SSID_SIZE];	
	unsigned char		channel_access;  /* enum ACCESS_MODE */  
	//__u8	best_available;  /* true/false */
	unsigned char		op_class;
	unsigned char		radio_number;  /* 0 = Wave0, 1 = Wave1 */
	unsigned char		channel_number;
	unsigned char		repeat_rate;  /* 0 ~ 255 per 5s */
	unsigned char		ip_service;  /* true/false */
	unsigned char		ipv6_addr[IPV6_SIZE];
	unsigned short	service_port;
	unsigned char		provider_mac[MAC_SIZE];
	unsigned char		rcpi_threshold;
	unsigned char		wsa_count_threshold;
	unsigned char		wsa_count_threshold_interval;
	unsigned short	signature_lifetime;  /* No support */
	unsigned char		service_status;

	/* internal use */
	unsigned int		wsa_hdr_extensions;	
	unsigned char		channel_interval; 
	
};


struct provider_info{
	unsigned char		action;
	unsigned char		dest_mac[MAC_SIZE];
	unsigned char		wsa_type;  /* enum WSA_TYPE */
	unsigned int		psid;
	unsigned char		psc[PSC_SIZE];
	unsigned char		psc_len;
	unsigned char		service_priority;
	unsigned char		service_specific_permissions[SSP_SIZE];    /* No support */
	unsigned char		service_specific_permissions_len;	  /* No support */
	unsigned char		security_service_id[SSID_SIZE];    /* No support */
	unsigned char		radio_number;  /* 0 = Wave0, 1 = Wave1 */
	unsigned char		channel_number;
	unsigned char		channel_access;  /* enum ACCESS_MODE */
	unsigned char		repeat_rate;
	unsigned char		ip_service;
	unsigned char		ipv6_addr[IPV6_SIZE];
	unsigned short	service_port;
	unsigned char		provider_mac[MAC_SIZE];
	unsigned char		rcpi_threshold;  /* no use now */
	unsigned char		wsa_count_threshold;
	unsigned char		wsa_count_threshold_interval;
	unsigned int		wsa_hdr_extensions;
	unsigned short	signature_lifetime;  /* No support */
};


/* User Servicel Definition */
/*************************************************************************/
struct mib_user_service_info{
	//unsigned short	local_service_index;
	unsigned char		request_type;
	unsigned int		psid;
	unsigned char		psc[PSC_SIZE];
	unsigned char		psc_len;
	unsigned char		service_priority;
	unsigned char		wsa_type;
	unsigned char		src_mac[MAC_SIZE];
	unsigned char		advertiser_id[ADVERTISER_ID_SIZE];
	unsigned char		advertiser_id_len;
	unsigned char		op_class;
	unsigned char		radio_number;  /* 0 = Wave0, 1 = Wave1 */	
	unsigned char		channel_number;
	unsigned char		link_quality;
	unsigned char		immediate_access;  /* true/false */
	unsigned short	extended_access;  /* 0 = Channel switch, 65535 = Continuous */	
	unsigned char		service_status;

	/* internal use */
	//unsigned char	base_available;  /* if the req assign channel = 0, set to True */
	unsigned char		assigned_channel;  /* the real assigned channel */
	//unsigned char	channel_access;  /* record the channel mode */	
	unsigned char		channel_interval;  /* the real assigned channel interval */
};

enum REQUEST_ACCESS {
	ACCESS_ON_MATCH,
	ACCESS_UNCONDITIONAL,
	ACCESS_NO_SCH,
	ACCESS_MAX,
};

enum WSA_TYPE{
	WSA_SECURE = 1,
	WSA_UNSECURE = 2,
	WSA_BOTH = 3,
	WSA_ANY = 4,
	WSA_MAX,
};

struct user_info {
	unsigned char		action;
	unsigned char		request_type;  /* enum REQUEST_ACCESS */
	unsigned int		psid;
	unsigned char		psc[PSC_SIZE];
	unsigned char		psc_len;
	unsigned char		service_priority;
	unsigned char		wsa_type;
	unsigned char		radio_number;  /* 0 = Wave0, 1 = Wave1 */		
	unsigned char		channel_number;
	unsigned char		src_mac[MAC_SIZE];
	unsigned char		advertiser_id[ADVERTISER_ID_SIZE];
	unsigned char		advertiser_id_len;	
	unsigned char		link_quality;  /* No support */
	unsigned char		immediate_access;  /* No support */
	unsigned short	extended_access;  /* 0 or 65535 */
};
/*************************************************************************/


/* Wsm Servicel Definition */
/*************************************************************************/
struct mib_wsm_service_info{
	unsigned int 		psid;
};

struct wsm_info
{
	unsigned char	action;
	unsigned int	psid;
};
/*************************************************************************/

enum EDCA_LEVELS {
	EDCA_LEVEL_BK = 0,
	EDCA_LEVEL_BE,
	EDCA_LEVEL_VO,
	EDCA_LEVEL_VI,
	EDCA_LEVEL_MAX,
};

struct mib_edca
{
	//__u8	aci;
	unsigned char	acm;	
	unsigned char	aifsn;
	unsigned char	cw_min;
	unsigned short	cw_max;
	unsigned short	txop_limit;
};

struct mib_channel_info{
	unsigned char		op_class;
	unsigned char		channel_number;
	unsigned char		adaptable;  /* true/false */	
	unsigned char		data_rate;
	signed char		tx_power;
	unsigned char		channel_access;
	struct mib_edca edca[EDCA_LEVEL_MAX];
};


/* Available Service Definition */
/*************************************************************************/
struct mib_available_service_info{
	unsigned char		wsa_type;
	unsigned char		src_mac[MAC_SIZE];
	unsigned int		psid;
	unsigned char		service_specific_permissions;  /* relation to 1609.2 */
	unsigned char		service_priority;
	unsigned char		psc_len;
	unsigned char		psc[PSC_SIZE];
	unsigned char		ipv6_addr[IPV6_SIZE];
	unsigned short	service_port;	
	unsigned char		provider_mac[MAC_SIZE];
	unsigned char		rcpi_threshold;
	unsigned char		rcpi;  /* the last received wsa */
	unsigned char		wsa_count_threshold;
	unsigned char		wsa_count_threshold_interval;
	unsigned char		advertiser_id_len;
	unsigned char		advertiser_id[ADVERTISER_ID_SIZE];
	unsigned char		change_count;
	unsigned char		repeat_rate;	
	unsigned char		transmit_power;
	unsigned char		country_string[COUNTRY_STR_SIZE];	

	unsigned int		latitude_2d;
	unsigned int		longitude_2d;
	unsigned int		latitude_3d;
	unsigned int		longitude_3d;
	unsigned short	elevation_3d;
	unsigned char		confidence_3d;
	unsigned char		semi_major_accuracy_3d;
	unsigned char		semi_minor_accuracy_3d;
	unsigned short	semi_major_orientation_3d;

	unsigned char		link_quality;
	unsigned char		service_status;

	struct mib_channel_info channel_info;

	unsigned short	router_life_time;
	unsigned char		ip_prefix[IPV6_SIZE];
	unsigned char		prefix_len;
	unsigned char		default_gateway[IPV6_SIZE];
	unsigned char		primary_dns[IPV6_SIZE];
	unsigned char		secondary_dns[IPV6_SIZE];
	unsigned char		gateway_mac[MAC_SIZE];
	
	unsigned int		wsa_hdr_extensions;

	/* internal */
	unsigned char 	channel_mapping;  /* record the mapping between service info and channel info */
	unsigned char		wsa_count;
	unsigned char		interval_counter;  /* count the passing interval */

	//struct timeval		timestamp;
};
/*************************************************************************/


struct mib_info
{
	unsigned char		entry_type;
	unsigned char		next;  /* true: fetch the next available index, false: fetch the index */
	short		entry_index;
	union {
		struct mib_provider_service_info provider_entry;
		struct mib_user_service_info user_entry;
		//struct mib_mdata_service_info mdata_entry;
		//struct mib_ta_service_info ta_entry;		
		struct mib_wsm_service_info wsm_entry;
		struct mib_available_service_info available_entry;
		//struct mib_channel_info channel_entry;
		//struct mib_capabilities_info capabilities_entry;
		//struct mib_switching_info switching_entry;
		//struct mib_channelset_info channelset_entry;
		//struct mib_edcacch_info edcacch_entry;
		//struct mib_edcasch_info edcasch_entry;
		//struct mib_txprofile_info txprofile_entry;
		//struct mib_timinginfo_info timinginfo_entry;
	} entry_value;
};



//#define VISIBLE_REPEAT_RATE (1 << 0)
//#define VISIBLE_TRANSMIT_POWER (1 << 1)
//#define VISIBLE_ADVERTISEMENT_ID (1 << 2)
//#define VISIBLE_COUNTRY_STR (1 << 3)
	//CFG_LOCATION,
	//CFG_EDCA,
//#define VISIBLE_SECONDARY_DNS (1 << 4)
//#define VISIBLE_GATEWAY_MAC (1 << 5)


struct wsa_cfg_info
{
	unsigned char repeat_rate;
	signed char transmit_power;
	unsigned char	advertiser_id[ADVERTISER_ID_SIZE];
	unsigned char	advertiser_id_len;
	unsigned char	country_str[COUNTRY_STR_SIZE];
	unsigned short router_lifetime;
	unsigned char ip_prefix[IPV6_SIZE];
	unsigned char prefix_len;
	unsigned char default_gateway[IPV6_SIZE];
	unsigned char primary_dns[IPV6_SIZE];
	unsigned char secondary_dns[IPV6_SIZE];
	unsigned char gateway_mac[MAC_SIZE];
	unsigned int visible_mask;  /* for optional field, defined by BITMASK_XXX  */
};


enum WAVE_DEV {
	DEV_CCH0 = 0,
	DEV_SCH0,
	DEV_SCH1,
	DEV_MAX,
};

enum WAVE_OP{
	OP_EXTEND = 1,
	OP_SWITCH,
	OP_MAX,
};

struct edca {
	//	__u8	aci;
	unsigned char	aifsn;
	unsigned char	ecw_min;
	unsigned char	ecw_max;
	unsigned short	txop_limit;
};

struct dev_info {
	unsigned char	channel_number;
	unsigned char	mode;  /* OP_EXTEND¡BOP_SWITCH */
	unsigned char	disable;
	unsigned char	data_rate;
	unsigned char	tx_power;
	unsigned char	adapter;  /* true/false */
	struct edca	edca[EDCA_LEVEL_MAX];
};

struct event_channel
{
	unsigned char channel;
};

struct event_message
{
	unsigned char	event;
	unsigned char	reason;
	//struct timeval	timestamp;
	union {
		struct 	event_channel channel;
		struct mib_available_service_info service;
	} info;
};

#endif
