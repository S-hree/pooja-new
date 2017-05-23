/***************************************************************************
 *
 *  Copyright (C) 2013 ICL, ITRI.  All Rights Reserved.
 *
 *  THIS IS AN UNPUBLISHED WORK WHICH CONTAINS CONFIDENTIAL INFORMATION
 *  FROM CCL, ITRI.  NO PART OF THIS WORK MAY BE USED IN ANY WAY WITHOUT
 *  THE PRIOR WRITTEN PERMISSION.  ANY UNAUTHORIZED USE COULD SUBJECT THE
 *  PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
 *
 *  CCL, ITRI IS NOT RESPONSIBLE OR LIABLE FOR ANY DIRECT, INDIRECT,
 *  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES THAT MAY RESULT FROM
 *  THE USE, OR INABILITY TO USE OF THIS WORK.  ANY EXPRESSED OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 *
 ***************************************************************************/

#ifndef WAVE_LIB_H
#define WAVE_LIB_H

#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/queue.h>

#include <tx_api.h>
#include <nx_api.h>

#include <atlk/v2x_service.h>
#include <atlk/nav.h>
#include <atlk/mibs/wlan-mib.h>
#include <craton/syslog.h>

#define MAX_IFS		10
#define MAX_PHYS    2
//define BIT(nr)		(1UL << (nr))
#define ETH_ALEN    6

//define V2X_BUF_LEN(x)  x+30
static inline uint16_t V2X_BUF_LEN(uint16_t x)
{
	return x + 30;
}

typedef int32_t wave_ret_t;
#define WAVE_OK 		((wave_ret_t)(0))
#define WAVE_E_EPERM   ((wave_ret_t)(-1))		/* Operation not permitted */
#define WAVE_E_INVAL 	((wave_ret_t)(-22))		/* Invalid argument */
#define WAVE_E_ATLK 	((wave_ret_t)(-35))		/* Atlk driver error */

#define IEEE80211_STYPE_TIMING_ADVERT 0x0060	/* Timing frame type */
#define IEEE80211_STYPE_ACTION 		  0x00D0

#define SKB_QUEUE_SIZE 256
#define WAVE_MAX_QUEUES 2  /* For channel switching */

extern struct skb_queue rx_queue;

#define WAVE_SCH_IFINDEX_BASE 90

//define MHZ_TO_KHZ(freq) ((freq) * 1000)
//define KHZ_TO_MHZ(freq) ((freq) / 1000)
static inline uint16_t MHZ_TO_KHZ(uint16_t freq)
{
	return ((freq) * 1000);
}

static inline uint16_t KHZ_TO_MHZ(uint16_t freq)
{
	return ((freq) / 1000);
}

extern uint8_t phy_org_mac[MAX_PHYS][ETH_ALEN];

static inline int32_t wave_ifindex_to_index(int32_t ifindex)
{
    return (ifindex > WAVE_SCH_IFINDEX_BASE) ? (ifindex - WAVE_SCH_IFINDEX_BASE) : ifindex;
}

/**
 * enum wave_rate_bit - corresponding bit in rate bitmap
 *
 * Each bit of rate bitmap map to a hardware rate.
 *
 * @WAVE_RATE_BPSK_1_2:  BPSK 1/2  ( 6Mbps @ 20MHz bandwidth,    3Mbps @ 10MHz bandwidth)
 * @WAVE_RATE_BPSK_3_4:  BPSK 3/4  ( 9Mbps @ 20MHz bandwidth,  4.5Mbps @ 10MHz bandwidth)
 * @WAVE_RATE_QPSK_1_2:  QPSK 1/2  (12Mbps @ 20MHz bandwidth,    6Mbps @ 10MHz bandwidth)
 * @WAVE_RATE_QPSK_3_4:  QPSK 3/4  (18Mbps @ 20MHz bandwidth,    9Mbps @ 10MHz bandwidth)
 * @WAVE_RATE_16QAM_1_2: 16QAM 1/2 (24Mbps @ 20MHz bandwidth,   12Mbps @ 10MHz bandwidth)
 * @WAVE_RATE_16QAM_3_4: 16QAM 3/4 (36Mbps @ 20MHz bandwidth,   18Mbps @ 10MHz bandwidth)
 * @WAVE_RATE_64QAM_2_3: 64QAM 2/3 (48Mbps @ 20MHz bandwidth,   24Mbps @ 10MHz bandwidth)
 * @WAVE_RATE_64QAM_3_4: 64QAM 3/4 (54Mbps @ 20MHz bandwidth, 27.5Mbps @ 10MHz bandwidth)
 */

enum wave_rate_bit {
    // Rate Index
    WAVE_RATE_IDX_BPSK_1_2	= 0,
    WAVE_RATE_IDX_BPSK_3_4	= 1,
    WAVE_RATE_IDX_QPSK_1_2	= 2,
    WAVE_RATE_IDX_QPSK_3_4	= 3,
    WAVE_RATE_IDX_16QAM_1_2	= 4,
    WAVE_RATE_IDX_16QAM_3_4	= 5,
    WAVE_RATE_IDX_64QAM_2_3	= 6,
    WAVE_RATE_IDX_64QAM_3_4	= 7,
    // Rate Mask
    WAVE_RATE_BPSK_1_2	= (1UL << (WAVE_RATE_IDX_BPSK_1_2)),
    WAVE_RATE_BPSK_3_4	= (1UL << (WAVE_RATE_IDX_BPSK_3_4)),
    WAVE_RATE_QPSK_1_2	= (1UL << (WAVE_RATE_IDX_QPSK_1_2)),
    WAVE_RATE_QPSK_3_4	= (1UL << (WAVE_RATE_IDX_QPSK_3_4)),
    WAVE_RATE_16QAM_1_2	= (1UL << (WAVE_RATE_IDX_16QAM_1_2)),
    WAVE_RATE_16QAM_3_4	= (1UL << (WAVE_RATE_IDX_16QAM_3_4)),
    WAVE_RATE_64QAM_2_3	= (1UL << (WAVE_RATE_IDX_64QAM_2_3)),
    WAVE_RATE_64QAM_3_4	= (1UL << (WAVE_RATE_IDX_64QAM_3_4)),

    WAVE_RATE_ALL_OFDM	= 0xFF,
};

void wave_init(void);
void wave_exit(void);

uint32_t rate_value_to_bitmap(int32_t value);
int32_t rate_bitmap_to_index(uint32_t bitmap);
int32_t rate_bitmap_to_value(uint32_t bitmap);

extern int sched_handler_run;

/**
 * struct wave_channel - channel set definition
 *
 * This structure describes a channel set, supported rates and power limit.
 * The center freq of second channel is (@freq_khz_first + @bandwidth_khz).
 *
 * @bandwidth_khz: 10000 or 20000 (10/20MHz)
 * @freq_mhz: center freq
 * @rate_bitmap: supported rates. (default is WAVE_RATE_ALL_OFDM)
 * @max_power: maximum transmission power in dBm.
 */
struct wave_channel {
    uint32_t	bandwidth_khz;
    uint32_t 	freq_mhz;
    uint32_t	rate_bitmap;
    uint32_t	max_power;
};

#define WAVE_CHAN_INIT {     \
  .bandwidth_khz = 10 * 1000,      \
  .freq_mhz = 5890,     \
  .rate_bitmap = WAVE_RATE_64QAM_3_4,     \
  .max_power = 18,     \
}

#define WAVE_CHAN_SCH_INIT {     \
  .bandwidth_khz = 10 * 1000,      \
  .freq_mhz = 5860,     \
  .rate_bitmap = WAVE_RATE_64QAM_3_4,     \
  .max_power = 18,     \
}

/* TAI */
#if 1 /* DENSO_API */
struct channel_interval {
    uint8_t 	cch_interval;       // TRUE=CCH, FALSE=SCH
    uint8_t     channel_number;     // Channel Number
    uint16_t	timeOnChannel;      // Time on this channel before switching
//win: TODO
//    struct timeval     timestamp;
    uint32_t	cch_busy_ratio;  // WAVE control channel busy ratio (0-100%), 999=unknown
    uint32_t	sch_busy_ratio;  // WAVE service channel busy ratio (0-100%), 999=unknown
};
#endif
/* End TAI */

enum wave_diagnose {
    WAVE_DIAG_DIS_TX	= (1UL << (0)),		// Disable transmit

    WAVE_DIAG_DIS_ACK	= (1UL << (1)),		// Disable ACKs
    WAVE_DIAG_DIS_CTS	= (1UL << (2)),		// Disable CTSs
    WAVE_DIAG_DIS_RX	= (1UL << (5)),		// Disable recieve

    WAVE_DIAG_NORMAL	= 0,			// None diagnose
    WAVE_DIAG_MASK_RX	= 0x00000026,	// all available RX bits
    WAVE_DIAG_MASK		= (WAVE_DIAG_MASK_RX|WAVE_DIAG_DIS_TX),
};

#define	WAVE_DIAG_DEFAULT				WAVE_DIAG_NORMAL

enum wave_frames {
	WAVE_FRAME_DATA = 0x1,
	WAVE_FRAME_MGMT = 0x2,
};

#define IFNAMSIZ 16

struct net_device_stats {
	uint64_t	rx_packets;
	uint64_t	tx_packets;
	uint64_t	rx_bytes;
	uint64_t	tx_bytes;
};

struct net_device {
    int32_t if_index;
	char name[IFNAMSIZ];

	const struct net_device_ops *netdev_ops;

	char priv[0x5000];
	struct net_device_stats stats;
};

static inline void *netdev_priv(const struct net_device *dev)
{
	return (char *) &(dev->priv);
}

struct net_device * wave_iface_add(const char * if_name, uint64_t proto_id);
void wave_iface_del(struct net_device *dev);

struct net_device * get_dev_by_index(int32_t if_index);
typedef void (*dev_cb)(struct net_device *dev);
void wave_foreach_ndev(dev_cb cb);

int32_t if_name_to_index(const char * if_name);
struct net_device * get_dev_by_name(const char *if_name);
uint8_t wave_dev_is_initialed(struct net_device *dev);

typedef struct {

#define	WAVE_CB_MAGIC	0x57415645	// "WAVE"
    uint32_t	magic;				// if magic wrong, tx will apply default

    struct {
        struct {
#define	WAVE_PREFERRED_NONE		0
#define	WAVE_PREFERRED_RATE		(1UL << (0))
#define	WAVE_PREFERRED_RETRY	(1UL << (1))
#define	WAVE_PREFERRED_TX_POWER	(1UL << (2))
            /**
             * if not set WAVE_PREFERRED_QOS, the frame send without qos field (assume priority = 0)
             * if set, look up TID of QoS from skb->priority or IP TOS.
             *		if skb->priority values from 256 to 263, directly indicate a priority (0 ~ 7)
             *		if it's IP packet, priority is TOS bit5-7 of IP header;
             *		otherwise, assume priority = 0.
             *
             * 802.1d priority (0 ~ 7) to wave aci (queue) mapping table:
             *		{	WAVE_AC_VI, WAVE_AC_VO, WAVE_AC_VO, WAVE_AC_VI,
             *			WAVE_AC_BE, WAVE_AC_BE, WAVE_AC_BK, WAVE_AC_BK, }
             */
#define	WAVE_PREFERRED_QOS		(1UL << (3))

#define	IEEE802_1D_PRIORITY_INDICATE	0x0100
#define	IEEE802_1D_PRIORITY_MASK		0x0007

            int16_t preferred;		// default: WAVE_PREFERRED_NONE
            // if set the corresponding bit, then assigned tx_rate_bitmap, tx_power ..
            // otherwise, use default.

            uint16_t tx_rate_idx;	// default: auto
            // range: WAVE_RATE_IDX_BPSK_1_2 .. WAVE_RATE_IDX_64QAM_3_4

#define	WAVE_DEFAULT_RETRY_LIMIT	5
            int16_t	tx_retry_limit;	// default: WAVE_DEFAULT_RETRY_LIMIT
            // [0..15]

            int16_t tx_power;		// default: by H/W
            // in dBm.
        } tx;

        struct {
            uint16_t bandwidth;		// in MHz	( 10 / 20 )
            uint16_t freq;			// in MHz
            int16_t	 signal;			// in dBm
            int16_t  noise;			// in dBm
            uint16_t rx_rate_idx;	// WAVE_RATE_IDX_BPSK_1_2 .. WAVE_RATE_IDX_64QAM_3_4
            uint16_t mpdu_bytes;		// original bytes on radio

            // some 802.11 fields of data frame
            uint8_t	 bssid[ETH_ALEN];// bssid
            uint8_t	 src_mac[ETH_ALEN];
			uint8_t	 dst_mac[ETH_ALEN];
#define	WAVE_CB_RX_TID_VALID	(1UL << (15))
            uint16_t tid;			// tid of qos if set WAVE_CB_RX_TID_VALID
        } rx;
    } u;
} wave_skb_cb_t;

//typedef char* wave_skb_buf_t;
typedef v2x_socket_t wave_sock_t;

struct sk_buff {
	struct sk_buff 	*next;
	struct sk_buff 	*prev;
	struct sk_buff_header	*list;

	//struct sock		*sk;
	struct net_device	*dev;

	uint32_t		len;
	uint32_t		truesize;
	uint32_t		priority;
	uint16_t		frame_type;

	//TX_SEMAPHORE	user_lock;
	//u8				users;
	//uint8_t			pkt_type;
	//uint8_t			protocol;

	unsigned char	*transport_header;
	unsigned char	*network_header;
	unsigned char 	*mac_header;

	unsigned char	*head;
	unsigned char	*buf;	//*data;
	unsigned char	*tail;
	unsigned char	*end;

    uint8_t         dest_mac[ETH_ALEN];

	//unsigned char	its_aid/ added by Tien-Yuan
	//unsigned int 	ssp_len/ added by Tien-Yuan
	//char 			ssp[32]/ added by Tien-Yuan

	wave_skb_cb_t	cb;
	wave_sock_t		*sk;
//	void	(*destructor)(struct sk_buff *skb);
};

extern TX_BYTE_POOL skb_pool;
void wave_skb_init(void);
void wave_skb_exit(void);
struct sk_buff * wave_skb_alloc(uint32_t len);
void wave_skb_free(struct sk_buff *skb);

static inline wave_skb_cb_t *WAVE_SKB_CB(struct sk_buff *skb)
{
    return &(skb->cb);
}

extern uint32_t wave_debug;
//define dprintf(_fmt, ...) {if (wave_debug) printf(_fmt, ##__VA_ARGS__)}
//define dfunction(_fmt, ...)
//        if (wave_debug)
//            printf("****** %s(" _fmt ") ******\n", __FUNCTION__, ##__VA_ARGS__)
//    }

// return number of wave_channel.
int wave_channels_get(struct net_device *dev, struct wave_channel *channels, int num);

// channel->freq_khz_last will be ignored.
wave_ret_t wave_channel_set(struct net_device *dev, struct wave_channel *channel);
wave_ret_t wave_channel_get(struct net_device *dev, struct wave_channel *channel);

wave_ret_t wave_start_xmit(struct sk_buff *skb, struct net_device *ndev);
void wave_txq_wait_complete(struct net_device *ndev);

void wave_rx_thread_entry(ULONG input);
void wave_rx_mgmt_thread_entry(ULONG input);
#ifndef CHANNEL_SW_SOFT
void wave_rx_sch_thread_entry(ULONG input);
#endif

typedef void (*wave_rx_indication)(struct sk_buff *skb);
wave_ret_t wave_reg_rx(wave_rx_indication indication);
void wave_unreg_rx(void);
wave_ret_t wave_reg_mgmt_rx(wave_rx_indication indication);
void wave_unreg_mgmt_rx(void);

#if 0
int wave_bssid_set(struct net_device *dev, uint8_t const *bssid);
int wave_bssid_get(struct net_device *dev, uint8_t *bssid);

extern int (*wave_mgmt_handle_hook)(struct net_device *dev, v2x_buf_t *skb);

int wave_sta_update(struct net_device *dev, uint8_t const *mac, uint32_t rate_bitmap);
int wave_sta_remove(struct net_device *dev, uint8_t const *mac);

//
// new features for switch channel
//
#endif

// dev_ctl: device of control instance, created by wave_iface_add()
// name:	preferred device name. for example: "wave_srv1".
//			or "wave_srv%d" (%d means smallest number not used)
// return:	NON-NULL, create device for the service successfully.
//			NULL, if system has a device with duplicated name.
//
// NOTE: please configure bandwidth and freq with wave_channel_set().
//		Do not call rtnl_lock, before call this routine.
struct net_device* wave_dev_srv_add(struct net_device *dev_ctl, const char *name);
void wave_dev_srv_del(struct net_device* dev);

#if 0
// release service device and its schedule times.
static inline int wave_dev_srv_del(struct net_device *dev_srv)
{
    return wave_iface_del(dev_srv->name);
}

// dev_ctl:	control device
// entries:	list size in (struct net_device*)
// list:	space for store service device list.
//
// return:	>=0	actual count of service device, (>entries, list space is not enough)
//			-1, dev_srv is not control device
//
// NOTE: entries=0 or list=NULL, just return count.
//
int wave_dev_srv_list(struct net_device *dev_ctl, int entries, struct net_device **list);

#endif

/**
 * One second is the period of channel schedule.
 * Therefore, "enter" and "leave" are the millisecond part of GMT(sync by GPS).
 * Only assign schedule time for service channels,
 * remainder (default) times are ownned by control channel.
 *
 * NOTE: min interval between two channel switching is 10 milliseconds.
 */
#define	WAVE_SCHED_MIN_INTERVAL	10
struct wave_chan_schedule {
    uint16_t	enter;	// millisecond of enter time
    uint16_t	leave;	// millisecond of leave time
};

//
// Add the schedule time for service device.
// overwrite previous schedules if overlapped, even owned by other service device
// example:
//	{
//		{ 50, 100},
//		{150, 200},
//		{250, 300},
//		{350, 400},
//		{850,1000},
//		{  0,   0},
//	};
//
// return	>=0	OK
//			-ENODEV: dev_srv not a service device
//int wave_chan_schedule_add(struct net_device *dev_srv, struct wave_chan_schedule const* sched)

// Remove all the schedule time of any service device.
//
wave_ret_t wave_chan_schedule_set(struct net_device *dev_srv, int cch, int sch);
wave_ret_t wave_chan_schedule_del(struct net_device *dev_ctl);

// Get schedule time of control/service device
int wave_chan_schedule_get(struct net_device *dev, int entries, struct wave_chan_schedule *sched);
void wave_chan_schedule_reset(struct net_device *dev);

#if 0

/////////////////////////////////////////////////////////////////////////////
// EDCA API

enum wave_aci {
    WAVE_AC_BK = 0,	/*Background traffic*/
    WAVE_AC_BE, 	/*Best-effort (normal) traffic)*/
    WAVE_AC_VI, 	/*Video traffic*/
    WAVE_AC_VO, 	/*Voice traffic*/

    WAVE_MAX_QUEUES,	// = IEEE80211_MAX_QUEUES = 4
};

/*
 * wave_txq_conf: Configure TX queue parameters
 *	(EDCF (aifs, cw_min, cw_max), bursting) for a hardware TX queue.
 *	Returns a negative error code on failure.
 */
int wave_txq_conf(struct net_device *dev, uint16_t queue,
                  const struct ieee80211_tx_queue_params *params);
#endif

wave_ret_t wave_mgmt_tx(struct net_device *dev, struct sk_buff *skb,
	int stype, const uint8_t *da, const uint8_t *sa, const uint8_t *bssid);

static inline wave_ret_t wave_timing_advert_tx(struct net_device *dev, struct sk_buff *skb)
{
    return wave_mgmt_tx(dev, skb, IEEE80211_STYPE_TIMING_ADVERT, NULL, NULL, NULL);
}

static inline wave_ret_t wave_vendor_action_tx(struct net_device *dev, struct sk_buff *skb)
{
    return wave_mgmt_tx(dev, skb, IEEE80211_STYPE_ACTION, NULL, NULL, NULL);;
}

/////////////////////////////////////////////////////////////////////////////
// Statistic & Diagnose

typedef struct wave_stats_type {
	unsigned long	rx_ucast;
	unsigned long	rx_bcast;
	unsigned long	rx_mcast;
	unsigned long	rx_data;
	unsigned long	rx_ctrl;
	unsigned long	rx_mgt;

	unsigned long	rx_total_count;
	unsigned long	rx_total_bytes;

	unsigned long	tx_ucast;
	unsigned long	tx_bcast;
	unsigned long	tx_mcast;
	unsigned long	tx_data;
	unsigned long	tx_ctrl;
	unsigned long	tx_mgt;

	unsigned long	tx_total_count;
	unsigned long	tx_total_bytes;

	long			last_rssi;
	unsigned long	last_rx_rate;
	unsigned long	last_tx_rate;
	unsigned long	last_rx_ant;
	unsigned long	rxant_sw_cnt;
} wave_stats;

wave_ret_t wave_get_stats(struct net_device *dev, wave_stats *stats);

int wave_diagnose_set(struct net_device *dev, enum wave_diagnose diag);
int wave_diagnose_get(struct net_device *dev, enum wave_diagnose *diag);

#define	WAVE_TX_BACKOFF_BASE_DEFAULT	1000	// in microseconds
#define	WAVE_TX_BACKOFF_RANDOM_DEFAULT	0		// in microseconds

#define	WAVE_TX_BACKOFF_MAX				5000	// in microseconds

// tx random backoff after channel switch
int wave_tx_backoff_set(struct net_device *dev, uint16_t base, uint16_t random);
int wave_tx_backoff_get(struct net_device *dev, uint16_t *base, uint16_t *random);

#if 1 //DENSO_API
int wave_get_rawmode(const char *eth_name);
int wave_set_rawmode(const char *eth_name, int mode);
int wave_get_subtype(const char *eth_name);
int wave_set_subtype(const char *eth_name, int qos_type);
int wave_dev_get_diversity(struct net_device *dev);
void wave_dev_set_diversity(struct net_device *dev, int mode);

/*	TAI	*/

int wave_get_default_txrate(const char *eth_name);
int wave_set_default_txrate(const char *eth_name, int tx_rate);
int wave_get_default_power(const char *eth_name);
int wave_set_default_power(const char *eth_name, int tx_power);
int wave_get_default_qos(const char *eth_name);
int wave_set_default_qos(const char *eth_name, int qos);

/* END TAI	*/

#define WAVE_ANTENNA_NULL 99
struct antenna_mode {
    uint8_t tx_ant;
    uint8_t rx_ant;
};

void wave_dev_set_antenna(struct net_device *dev, uint8_t tx_ant, uint8_t rx_ant);
struct antenna_mode wave_dev_get_antenna(struct net_device *dev);
void wave_random_mac(struct net_device *dev, uint8_t NewMAC[ETH_ALEN]);
wave_ret_t wave_get_org_mac(struct net_device *dev, uint8_t *mac);
int wave_get_usage(struct net_device *dev);
#endif

/* TAI */
#if 1 //DENSO_API
extern void (*wave_chan_notify_rcv)(struct channel_interval *);
int wave_reg_chan_notify(void (*rcv_cb)(struct channel_interval *));
int wave_unreg_chan_notify(void);
#endif
/* End TAI */

wave_ret_t wave_get_mac(struct net_device *dev, uint8_t *mac);
wave_ret_t wave_set_mac(struct net_device *dev, uint8_t *mac);

#endif /* WAVE_LIB_H */

