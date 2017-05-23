#ifndef	__SAE_COMMON_H_ 
#define	__SAE_COMMON_H_ 

/*Encode and Decode printf_level*/
#define PRINT_NON 0
#define PRINT_BASIC 1
#define PRINT_COMPLETE 2

/*DE_DSRC_MessageID*/
#define ACM_ID 1
#define BSM_ID 2
#define CSR_ID 4
#define EVA_ID 5
#define ICA_ID 6
#define MAP_ID 7
#define NMEA_ID 8
#define PDM_ID 9
#define PVD_ID 10
#define RSA_ID 11
#define RTCM_ID 12
#define SPAT_ID 13
#define SRM_ID 14
#define SSM_ID 15
#define TIM_ID 16

#define MIN_MSGCNT 0
#define MAX_MSGCNT 127

#define TRUE 1

enum MSG_ID
{
	NON,
	DENMID,
	CAMID,
	POIID,
	SPATID,
	MAPID,
	IVIID,
	EV_RSRID,
};

enum LANE_TYPE_ATTRIBUTES_ENUM
{
	LANE_TYPE_ATTRIBUTES_NOTHING,
	LANE_TYPE_ATTRIBUTES_VEHICLE,
	LANE_TYPE_ATTRIBUTES_CROSSWALK,
	LANE_TYPE_ATTRIBUTES_BIKELANE,
	LANE_TYPE_ATTRIBUTES_SIDEWALK,
	LANE_TYPE_ATTRIBUTES_MEDIAN,
	LANE_TYPE_ATTRIBUTES_STRIPING,
	LANE_TYPE_ATTRIBUTES_TRACKEDVEHICLE,
	LANE_TYPE_ATTRIBUTES_PARKING	
};

enum CRUMB_DATA_PATH_HISTORY_POINT_SETS_ENUM
{
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_NOTHING,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_01,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_02,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_03,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_04,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_05,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_06,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_07,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_08,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_09,
	CRUMB_DATA_PATH_HISTORY_POINT_SETS_10	
};

enum NODE_OFFSET_POINT_ENUM
{
	NODE_OFFSET_POINT_NOTHING,
	NODE_OFFSET_POINT_XY1,
	NODE_OFFSET_POINT_XY2,
	NODE_OFFSET_POINT_XY3,
	NODE_OFFSET_POINT_XY4,
	NODE_OFFSET_POINT_XY5,
	NODE_OFFSET_POINT_XY6,
	NODE_OFFSET_POINT_LATLON,
	NODE_OFFSET_POINT_REGIONAL
};

enum LANE_DATA_ATTRIBUTE_ENUM
{
	LANE_DATA_ATTRIBUTE_NOTHING,
	LANE_DATA_ATTRIBUTE_PATHENDPOINTANGLE,
	LANE_DATA_ATTRIBUTE_LANECROWNPOINTCENTER,
	LANE_DATA_ATTRIBUTE_LANECROWNPOINTLEFT,
	LANE_DATA_ATTRIBUTE_LANECROWNPOINTRIGHT,
	LANE_DATA_ATTRIBUTE_LANEANGLE,
	LANE_DATA_ATTRIBUTE_SPEEDLIMITS,
	LANE_DATA_ATTRIBUTE_REGIONAL	
};

enum OFFSET_XAXIS_ENUM
{
	OFFSET_XAXIS_NOTHING,
	OFFSET_XAXIS_SMALL,
	OFFSET_XAXIS_LARGE
};

enum OFFSET_YAXIS_ENUM
{
	OFFSET_YAXIS_NOTHING,
	OFFSET_YAXIS_SMALL,
	OFFSET_YAXIS_LARGE
};

enum NODE_LIST2_ENUM
{
	NODE_LIST2_NOTHING,
	NODE_LIST2_NODES,
	NODE_LIST2_COMPUTED
};

enum RESTRICTION_USER_TYPE_ENUM
{
	RESTRICTION_USER_TYPE_NOTHING,
	RESTRICTION_USER_TYPE_BASICTYPE,
	RESTRICTION_USER_TYPE_REGIONAL
};

enum VEHICLE_IDENT__VEHICLE_CLASS_ENUM 
{
	VEHICLE_IDENT__VEHICLE_CLASS_NOTHING,
	VEHICLE_IDENT__VEHICLE_CLASS_VGROUP,
	VEHICLE_IDENT__VEHICLE_CLASS_RGROUP,
	VEHICLE_IDENT__VEHICLE_CLASS_REQUIP
};

enum MSG_ID_ENUM 
{
	MSG_ID_NOTHING,
	MSG_ID_FURTHER_INFO_ID,
	MSG_ID_ROAD_SIGN_ID
};

enum VALID_REGION__AREA_ENUM 
{
	VALID_REGION__AREA_NOTHING,
	VALID_REGION__AREA_SHAPE_POINT_SET,
	VALID_REGION__AREA_CIRCLE,
	VALID_REGION__AREA_REGION_POINT_SET
};

enum CONTENT_ENUM 
{
	CONTENT_NOTHING,
	CONTENT_ADVISORY,
	CONTENT_WORKZONE,
	CONTENT_GENERICSIGN,
	CONTENT_SPEEDLIMIT,
	CONTENT_EXITSERVICE
};

enum PROBE_DATA_MANAGEMENT__TERM_ENUM 
{
	PROBE_DATA_MANAGEMENT__TERM_NOTHING,
	PROBE_DATA_MANAGEMENT__TERM_TERMTIME,
	PROBE_DATA_MANAGEMENT__TERM_TERMDISTANCE
};

enum PROBE_DATA_MANAGEMENT__SNAPSHOT_ENUM 
{
	PROBE_DATA_MANAGEMENT__SNAPSHOT_NOTHING,
	PROBE_DATA_MANAGEMENT__SNAPSHOT_SNAPSHOTTIME,
	PROBE_DATA_MANAGEMENT__SNAPSHOT_SNAPSHOTDISTANCE
};

enum ITIS_ITEM_ENUM 
{
	ITIS_ITEM_NOTHING,
	ITIS_ITEM_ITIS,
	ITIS_ITEM_TEXT
};

enum ITEM_ENUM 
{
	ITEM_NOTHING,
	ITEM_ITIS,
	ITEM_TEXT
};

enum CIRCLE__RADUIS_ENUM 
{
	CIRCLE__RADUIS_NOTHING,
	CIRCLE__RADUIS_RADIUSSTEPS,
	CIRCLE__RADUIS_MILES,
	CIRCLE__RADUIS_KM
};

typedef struct vehicle_ident_str
{
	unsigned char mask_name:1,
			 mask_vin:1,
			 mask_owner_code:1,
			 mask_id:1,
			 mask_vehicle_type:1,
			 mask_vehicle_class:1,
			 rest:2;

	char name[64]; //OP; IA5String (SIZE(1..63))
	char vin[18]; //OP; OCTET STRING (SIZE(1..17))
	char owner_code[33]; //OP; IA5String(SIZE(1..32))
	unsigned char id[4]; //OP; OCTET STRING (SIZE(4))
	unsigned char vehicle_type; //OP; ENUMERATED {0..15}
	
	unsigned char vehicle_class_choice;
	unsigned int vehicle_class;	/*OP, choice
								  {
									  VEHICLE_IDENT__VEHICLE_CLASS_VGROUP,
									  VEHICLE_IDENT__VEHICLE_CLASS_RGROUP,
									  VEHICLE_IDENT__VEHICLE_CLASS_REQUIP
								  } */
}vehicle_ident_item;

typedef struct ddate_time_str 
{
	unsigned char mask_year:1,
			mask_month:1,
			mask_day:1,
			mask_hour:1,
			mask_minute:1,
			mask_second:1,
			reserved:2;

	unsigned short year; //OP; INTEGER (0..9999); units of years; unknown: 0
	unsigned char month; //OP; INTEGER (0..15); units of months; unknown: 15
	unsigned char day; //OP; INTEGER (0..31); units of days; unknown: 0
	unsigned char hour; //OP; INTEGER (0..31); units of hours; unknown: 31
	unsigned char minute; //OP; INTEGER (0..63); units of minutes; unknown: 63
	unsigned short second; //OP; INTEGER (0..65535); units of miliseconds; unavailable: 65535
}ddate_time_item; 

typedef struct speed_heading_throttle_confidence_str
{
	unsigned char heading; //ENUMERATED {0..7}
	unsigned char speed; //ENUMERATED {0..7}
	unsigned char throttle; //ENUMERATED {0..3}
}speed_heading_throttle_confidence_item;

typedef struct position_confidence_set_str
{
	unsigned char pos; //ENUMERATED {0..15}
	unsigned char elevation; //ENUMERATED {0..15}
}position_confidence_set_item;

typedef struct transmission_speed_str 
{
	unsigned char transmission_state; //ENUMERATED {0..7}
	unsigned short speed; //INTEGER (0..8191); LSB = 0.02 m/s; unavailable: 8191
}transmission_speed_item;

typedef struct positional_accuracy_str { 
	unsigned char semi_major; //RANGE(0..254); LSB = 0.05m; unavailable: 255
	unsigned char semi_minor; //RANGE(0..254); LSB = 0.05m; unavailable: 255
	unsigned short semi_major_orient; //RANGE(0..65535); LSB = 0.0054932479 degrees; unavailable: 65535
}positional_accuracy_item;

typedef struct vehicle_size_str{
	unsigned short width; //INTEGER (0..1023); LSB = 1 cm; unavailable: 0
	unsigned short length; //INTEGER (0..16383); LSB = 1 cm; unavailable: 0
}vehicle_size_item;

typedef struct wipers_str
{
	unsigned char mask_status_rear:1,
			mask_rate_rear:1,
			rest:6;

	unsigned char status_front; //ENUMERATED {0..127}
	unsigned char rate_front; //INTEGER (0..127); units of sweeps per minute
	unsigned char status_rear; //OP; ENUMERATED {0..127}
	unsigned char rate_rear; //OP; INTEGER (0..127); units of sweeps per minute
}wipers_item;

typedef struct brake_system_status_str
{
	unsigned char wheel_brakes; //BIT STRING 4 bits; RANGE(0..15)
	unsigned char wheel_brakes_unavailable; //BOOL 1 bit; RANGE(0..1)
	unsigned char spare_bit; // 1 bit, set to zero
	unsigned char traction; //ENUMERATED {0..3}
	unsigned char abs; //ENUMERATED {0..3}
	unsigned char scs; //ENUMERATED {0..2}
	unsigned char brake_boost; //ENUMERATED {0..2}
	unsigned char aux_brakes; //ENUMERATED {0..3}
}brake_system_status_item;

typedef struct brake_system_status_2_str
{
	unsigned char wheel_brakes; //BIT STRING 4 bits; RANGE(0..15)
	unsigned char traction; //ENUMERATED {0..3}
	unsigned char abs; //ENUMERATED {0..3}
	unsigned char scs; //ENUMERATED {0..2}
	unsigned char brake_boost; //ENUMERATED {0..2}
	unsigned char spare_bits; // 4 bit, set to zero
}brake_system_status_2_item;


typedef struct steering_str
{
	unsigned char mask_confidence:1,
			mask_rate:1,
			mask_wheels:1,
			rest:5;

	signed char angle; //RANGE(-126..126); LSB = 1.5 degrees; unavailable: 127
	unsigned char confidence; //OP; ENUMERATED {0..3}
	signed char rate; //OP; INTEGER (-127..127)				
	signed char wheels; //OP; INTEGER (-127..127); LSB = 0.3333 degrees; unavailable: 0
}steering_item;

typedef struct acceleration_set_4way_str
{
	short longitude; //INTEGER (-2000..2001); LSB = 0.01 m/s^2; unavailable: 2001
	short lat; //INTEGER (-2000..2001); LSB = 0.01 m/s^2; unavailable: 2001
	signed char vert; // INTEGER (-127..127); LSB = 0.02 G; unavailable: -127
	short yaw; //INTEGER (-32767..32767); LSB = 0.01 degrees
}acceleration_set_4way_item;

typedef struct accel_steer_yaw_rate_confidence_str
{
	unsigned char yaw_rate; //ENUMERATED {0..7}
	unsigned char acceleration; //ENUMERATED {0..7}
	unsigned char steering_wheel_angle; //ENUMERATED {0..3}
}accel_steer_yaw_rate_confidence_item;

typedef struct confidence_set_str
{
	unsigned char mask_accel_confidence :1,
			mask_speed_confidence:1,
			mask_time_confidence:1,
			mask_pos_confidence:1,
			mask_steer_confidence:1,
			mask_throttle_confidence:1,
			rest:2;

	accel_steer_yaw_rate_confidence_item *accel_confidence; //OP
	speed_heading_throttle_confidence_item *speed_confidence; //OP
	unsigned char time_confidence; //OP; ENUMERATED {0..39}
	position_confidence_set_item *pos_confidence; //OP
	unsigned char steer_confidence; //OP; ENUMERATED {0..3}
	unsigned char throttle_confidence; //OP; ENUMERATED {0..3}
}confidence_set_item;

typedef struct accel_sets_str
{
	unsigned char mask_accel_4way:1,
			mask_vert_accel_thres:1,
			mask_yaw_rate_con:1,
			mask_hoz_accel_con:1,
			mask_confidence_set:1,
			rest:3;

	acceleration_set_4way_item *accel_4way; //OP
	unsigned char vert_accel_thres; //OP; BIT STRING 4 bits
	unsigned char yaw_rate_con; //OP; ENUMERATED {0..7}
	unsigned char hoz_accel_con; //OP; ENUMERATED {0..7}
	confidence_set_item *confidence_set; //OP
}accel_sets_item;

typedef struct weather_report_str
{
	unsigned char mask_rain_rate :1,
			mask_precip_situation :1,
			mask_solar_radiation :1,
			mask_friction :1,
			rest:4;

	unsigned char is_raining; //ENUMERATED {1..3)
	unsigned short rain_rate; //OP; INTEGER (0..65535)*/
	unsigned char precip_situation; //OP; ENUMERATED {0..15}
	unsigned short solar_radiation; //OP; INTEGER (0..65535)
	unsigned char friction; //OP; INTEGER (0..101)
}weather_report_item;

typedef struct tires_str
{
	unsigned char mask_location:1,
			mask_pressure:1,
			mask_temp:1,
			mask_wheel_sensor_status:1,
			mask_wheel_end_elect_fault:1,
			mask_leakage_rate:1,
			mask_detection:1,
			rest:1;

	unsigned char location; //OP; INTEGER (0..255)
	unsigned short pressure; //OP; INTEGER (0..1000)
	unsigned short temp; //OP; INTEGER (0..65535)
	unsigned char wheel_sensor_status; //OP; ENUMERATED {0..3}
	unsigned char wheel_end_elect_fault; //OP; BIT STRING 4 bits
	unsigned short leakage_rate; //OP;	INTEGER (0..65535)
	unsigned char detection; //OP; ENUMERATED {0..7}
}tires_item;

typedef struct axle_str
{
	unsigned char mask_location:1,
			mask_weight:1,
			rest:6;

	unsigned char location; //OP; INTEGER (0..127)
	unsigned short weight; //OP; INTEGER (0..65535)
}axle_item;

typedef struct j1939_data_str
{
	unsigned char 	mask_trailer_weight:1,
			mask_cargo_weight:1,
			mask_steering_axle_temperature:1,
			mask_drive_axle_location:1,
			mask_drive_axle_lift_air_pressure:1,
			mask_drive_axle_temperature:1,			
			mask_drive_axle_lube_pressure:1,
			mask_steering_axle_lube_pressure:1;

	unsigned char mask_tires:1,
			mask_axle:1,
			rest:6;

	unsigned char tires_num; //SEQUENCE (SIZE(0..16))
	tires_item *tires;
	
	unsigned char axle_num;	//SEQUENCE (SIZE(0..16))
	axle_item *axle;
	
	unsigned short trailer_weight; //OP;	INTEGER (0..65535)
	unsigned short cargo_weight; //OP; INTEGER (0..65535)
	unsigned char steering_axle_temperature; //OP; INTEGER (0..255)
	unsigned char drive_axle_location; //OP; INTEGER (0..255)
	unsigned short drive_axle_lift_air_pressure; //OP; INTEGER (0..1000)
	short drive_axle_temperature; //OP; INTEGER (-40..210)
	unsigned short drive_axle_lube_pressure; //OP; INTEGER (0..1000)
	unsigned char steering_axle_lube_pressure; //OP;	INTEGER (0..255)
}j1939_data_item;

typedef struct bumper_heights_str
{
	unsigned char frnt; //INTEGER (0..127); LSB = 0.01 meters
	unsigned char rear; //INTEGER (0..127); LSB = 0.01 meters
}bumper_heights_item;

typedef struct vehicle_data_str
{
	unsigned char height; //INTEGER (0..127); LSB = 5 cm; 
	bumper_heights_item bumpers;
	unsigned char mass; //INTEGER (1..127); LSB = 50 Kg
	unsigned short trailer_weight; //INTEGER (0..65535)
	unsigned char type; //ENUMERATED {0..15}
}vehicle_data_item;

typedef struct object_str
{
	unsigned int ob_dist; //INTEGER (0..32767); LSB units of meters
	unsigned int ob_direct; // INTEGER (0..28800); LSB = 0.0125 degrees; unavailable: 28800
	ddate_time_item *date_time;
}object_item;

typedef struct full_position_vector_str
{
	unsigned char mask_utc_time:1,
			  mask_elevation:1,
			  mask_heading:1,
			  mask_speed:1,
			  mask_pos_accuracy:1,
			  mask_time_confidence:1,
			  mask_pos_confidence:1,
			  mask_speed_confidence:1;

	ddate_time_item *utc_time; //OP
	long longitude; //INTEGER (-1799999999..1800000001); LSB = 1/10 micro degree; unavailable: 1800000001
	long lat; //INTEGER (-900000000..900000001); LSB = 1/10 micro degree; unavailable: 900000001
	int elevation; //OP; RANG(-4095..61439); LSB = 0.1 meters; unknown: 61440;
	unsigned short heading; //OP; INTEGER (0..28800); LSB = 0.0125 degrees; unavailable: 28800
	transmission_speed_item *speed;	//OP
	positional_accuracy_item *pos_accuracy; //OP
	unsigned char time_confidence; //OP; ENUMERATED {0..39}
	position_confidence_set_item *pos_confidence; //OP
	speed_heading_throttle_confidence_item *speed_confidence; //OP
}full_position_vector_item;


typedef struct vehicle_status_str
{
	unsigned char mask_lights :1,
			mask_light_bar:1,
			mask_wipers:1,
			mask_brake_status:1,
			mask_brake_pressure:1,
			mask_road_friction:1,
			mask_sun_data:1,
			mask_rain_data:1;

	unsigned char mask_air_temp:1,
			mask_air_pres:1,
			mask_steering:1,
			mask_accel_sets:1,
			mask_object:1,
			mask_full_pos:1,
			mask_throttle_pos:1,
			mask_speed_head_c:1;

	unsigned char	mask_speed_c:1,
			mask_vehicle_data:1,
			mask_vehicle_ident:1,
			mask_j1939_data:1,
			mask_weather_report:1,
			mask_gps_status:1,
			rest:2;

	unsigned short lights; //OP;	INTEGER (0..256)
	unsigned char light_bar; //OP; ENUMERATED {0..9}
	wipers_item *wipers; //OP
	brake_system_status_2_item *brake_status; //OP
	unsigned char brake_pressure; //OP; ENUMERATED {0..15}
	unsigned char road_friction; //OP; INTEGER (0..50); 
	unsigned short sun_data; //OP; INTEGER (0..1000)
	unsigned char rain_data; //OP; ENUMERATED {0..7}
	unsigned char air_temp; //OP; INTEGER (0..191)
	unsigned char air_pres; //OP; INTEGER (0..255)
	steering_item *steering; //OP
	accel_sets_item *accel_sets; //OP
	object_item *object; //OP
	full_position_vector_item *full_pos; //OP
	unsigned char throttle_pos; //OP;  /*optional, integer (0..200)*/
	
	speed_heading_throttle_confidence_item *speed_head_c; //OP
	unsigned char speed_c; //OP; 	/*optional, enum (0...7)*/
	vehicle_data_item *vehicle_data; //OP
	vehicle_ident_item *vehicle_ident; //OP
	
	unsigned char j1939_data_num;
	j1939_data_item *j1939_data; //OP

	weather_report_item *weather_report; //OP
	unsigned char gps_status; //OP; BIT STRING 8 bits
}vehicle_status_item;

typedef struct antenna_offset_set_str
{
	short ant_offset_x; //INTEGER (-8191..8191); LSB = 1cm; unavailable: 8191
	short ant_offset_y; //INTEGER (-255..255); LSB = 1cm; unavailable: 255
	short ant_offset_z; //INTEGER (0..511); LSB = 1cm; unavailable: 511
}antenna_offset_set_item;

typedef struct rtcm_header_str
{
	unsigned char status; //BIT STRING 8 bits
	antenna_offset_set_item offset_set;
}rtcm_header_item;

typedef struct rtcm_package_str
{
	unsigned char mask_anchor_point:1,
			mask_msg1001:1,
			mask_msg1002:1,
			mask_msg1003:1,
			mask_msg1004:1,
			mask_msg1005:1,
			mask_msg1006:1,
			mask_msg1007:1;

	unsigned char	mask_msg1008:1,
			mask_msg1009:1,
			mask_msg1010:1,
			mask_msg1011:1,
			mask_msg1012:1,
			mask_msg1013:1,
			mask_msg1014:1,
			mask_msg1015:1;
			
	unsigned char	mask_msg1016:1,
			mask_msg1017:1,
			mask_msg1019:1,
			mask_msg1020:1,
			mask_msg1021:1,
			mask_msg1022:1,
			mask_msg1023:1,
			mask_msg1024:1;

	unsigned char	mask_msg1025:1,
			mask_msg1026:1,
			mask_msg1027:1,
			mask_msg1029:1,
			mask_msg1030:1,
			mask_msg1031:1,
			mask_msg1032:1,
			rest:1;

	full_position_vector_item *anchor_point; //OP
	rtcm_header_item rtcm_header;
	unsigned char msg1001[125];	//OP; octet string (size(16..124))
	unsigned char msg1002[157]; //OP; octet string (size(18..156))
	unsigned char msg1003[211]; //OP; octet string (size(21..210))
	unsigned char msg1004[259]; //OP; octet string (size(24..258))
	unsigned char msg1005[20]; //OP; octet string (size(19))
	unsigned char msg1006[22]; //OP; octet string (size(21))
	unsigned char msg1007[37]; //OP; octet string (size(5..36))
	unsigned char msg1008[69]; //OP; octet string (size(6..68))
	unsigned char msg1009[137]; //OP; octet string (size(16..136))
	unsigned char msg1010[167]; //OP; octet string (size(18..166))
	unsigned char msg1011[223]; //OP; octet string (size(21..222))
	unsigned char msg1012[269]; //OP; octet string (size(24..268))
	unsigned char msg1013[28]; //OP; octet string (size(13..27))
	unsigned char msg1014[16]; //OP; octet string (size(15))
	unsigned char msg1015[70]; //OP; octet string (size(13..69))
	unsigned char msg1016[82]; //OP; octet string (size(14..81))
	unsigned char msg1017[116]; //OP; octet string (size(16..115))
	unsigned char msg1019[63]; //OP; octet string (size(62))
	unsigned char msg1020[46]; //OP; octet string (size(45))
	unsigned char msg1021[63]; //OP; octet string (size(62))
	unsigned char msg1022[76]; //OP; octet string (size(75))
	unsigned char msg1023[74]; //OP; octet string (size(73))
	unsigned char msg1024[75]; //OP; octet string (size(74))
	unsigned char msg1025[26]; //OP; octet string (size(25))
	unsigned char msg1026[31]; //OP; octet string (size(30))
	unsigned char msg1027[34]; //OP; octet string (size(33))
	unsigned char msg1029[70]; //OP; octet string (size(10..69))
	unsigned char msg1030[106]; //OP; octet string (size(14..105))
	unsigned char msg1031[108]; //OP; octet string (size(15..107))
	unsigned char msg1032[21]; //OP; octet string (size(20))
}rtcm_package_item;

typedef struct path_prediction_str
{
	int radius_of_curve; //INTEGER (-32767..32767); LSB = 10cm
	unsigned char confidence; //INTEGER (0..200); LSB = 0.5 percent
}path_prediction_item;

typedef struct path_history_point_type_01_str
{
	unsigned char mask_elevation_offset:1,
                  mask_time_offset:1,
                  mask_pos_accuracy:1,
                  mask_heading:1,
                  mask_speed:1,
                  reserved:3;

 	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072
 	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072
 	short elevation_offset; //OP; INTEGER (-2048..2047); LSB = 10 cm; unavailable: -2048
	unsigned short time_offset; //OP; INTEGER (1..65535); LSB = 10 mSec; unavailable: 65535
	positional_accuracy_item *pos_accuracy; //OP
	signed char heading; //OP; INTEGER (-128..127); LSB = 1.5 degrees; unavailable: -128
	transmission_speed_item *speed; //OP
}path_history_point_type_01_item;

typedef struct path_history_point_type_02_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	short elevation_offset; //INTEGER (-2048..2047); LSB = 10 cm; unavailable: -2048; 12 signed bits
	unsigned short time_offset; //INTEGER (1..65535); LSB = 10 mSec; unavailable: 65535; 16 unsigned bits
	positional_accuracy_item pos_accuracy;
	signed char heading; //INTEGER (-128..127); LSB = 1.5 degrees; unavailable: -128; 8 signed bits	
	transmission_speed_item speed; //16 encoded bits
}path_history_point_type_02_item;

typedef struct path_history_point_type_03_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	short elevation_offset; //INTEGER (-2048..2047); LSB = 10 cm; unavailable: -2048; 12 signed bits
	unsigned short time_offset; //INTEGER (1..65535); LSB = 10 mSec; unavailable: 65535; 16 unsigned bits
	positional_accuracy_item pos_accuracy;
}path_history_point_type_03_item;

typedef struct path_history_point_type_04_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	short elevation_offset; //INTEGER (-2048..2047); LSB = 10 cm; unavailable: -2048; 12 signed bits
	unsigned short time_offset; //INTEGER (1..65535); LSB = 10 mSec; unavailable: 65535; 16 unsigned bits
}path_history_point_type_04_item;

typedef struct path_history_point_type_05_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	short elevation_offset; //INTEGER (-2048..2047); LSB = 10 cm; unavailable: -2048; 12 signed bits
	positional_accuracy_item pos_accuracy;
}path_history_point_type_05_item;

typedef struct path_history_point_type_06_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	short elevation_offset; //INTEGER (-2048..2047); LSB = 10 cm; unavailable: -2048; 12 signed bits
}path_history_point_type_06_item;

typedef struct path_history_point_type_07_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	unsigned short time_offset; //INTEGER (1..65535); LSB = 10 mSec; unavailable: 65535; 16 unsigned bits
	positional_accuracy_item pos_accuracy;
}path_history_point_type_07_item;

typedef struct path_history_point_type_08_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	unsigned short time_offset; //INTEGER (1..65535); LSB = 10 mSec; unavailable: 65535; 16 unsigned bits
}path_history_point_type_08_item;

typedef struct path_history_point_type_09_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	positional_accuracy_item pos_accuracy;
}path_history_point_type_09_item;

typedef struct path_history_point_type_10_str
{
	int lat_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
	int long_offset; //INTEGER (-131072..131071); LSB = 1/10 micro degrees; unavailable: -131072; 18 signed bits
}path_history_point_type_10_item;

typedef struct path_history_str
{
	unsigned char mask_initial_position:1,
			mask_curr_gps_status:1,
			mask_item_cnt:1,
			reserved:5;

	full_position_vector_item *initial_position; //OP
	unsigned char curr_gps_status; //OP; BIT STRING 8 bits
	unsigned char item_cnt; //OP; INTEGER (0..32)

	unsigned char crumb_data_choice;
	/*choice 1: CRUMB_DATA_PATH_HISTORY_POINT_SETS_01*/
	unsigned char path_history_point_sets_01_num; //SEQUENCE (SIZE(1..23))
	path_history_point_type_01_item *path_history_point_sets_01;

	/*choice 2: CRUMB_DATA_PATH_HISTORY_POINT_SETS_02*/
	path_history_point_type_02_item *path_history_point_sets_02;

	/*choice 3: CRUMB_DATA_PATH_HISTORY_POINT_SETS_03*/
	path_history_point_type_03_item *path_history_point_sets_03;

	/*choice 4: CRUMB_DATA_PATH_HISTORY_POINT_SETS_04*/
	path_history_point_type_04_item *path_history_point_sets_04;

	/*choice 5: CRUMB_DATA_PATH_HISTORY_POINT_SETS_05*/
	path_history_point_type_05_item *path_history_point_sets_05;

	/*choice 6: CRUMB_DATA_PATH_HISTORY_POINT_SETS_06*/
	path_history_point_type_06_item *path_history_point_sets_06;

	/*choice 7: CRUMB_DATA_PATH_HISTORY_POINT_SETS_07*/
	path_history_point_type_07_item *path_history_point_sets_07;

	/*choice 8: CRUMB_DATA_PATH_HISTORY_POINT_SETS_08*/
	path_history_point_type_08_item *path_history_point_sets_08;

	/*choice 9: CRUMB_DATA_PATH_HISTORY_POINT_SETS_09*/
	path_history_point_type_09_item *path_history_point_sets_09;

	/*choice 10: CRUMB_DATA_PATH_HISTORY_POINT_SETS_10*/
	path_history_point_type_10_item *path_history_point_sets_10;
}path_history_item;


typedef struct vehicle_safety_extension_str
{
	unsigned char mask_events:1,
			mask_path_history:1,
			mask_path_prediction:1,
			mask_the_rtcm:1,
			rest:4;

	unsigned short events; //OP; INTEGER (0..8192)
	path_history_item *path_history; //OP
	path_prediction_item *path_prediction; //OP
	rtcm_package_item *the_rtcm; //OP
}vehicle_safety_extension_item;

typedef struct node_xy_20b_item_str {
	int x; //integer (-512..511)
	int y; //integer (-512..511)
}node_xy_20b_item;

typedef struct node_xy_22b_item_str {
	int x; //integer (-1024..1023)
	int y; //integer (-1024..1023)
}node_xy_22b_item;

typedef struct node_xy_24b_item_str {
	int x; //integer (-2048..2047)
	int y; //integer (-2048..2047)
}node_xy_24b_item;

typedef struct node_xy_26b_item_str {
	int x; //integer (-4096..4095)
	int y; //integer (-4096..4095)
}node_xy_26b_item;

typedef struct node_xy_28b_item_str {
	int x; //integer (-8192..8191)
	int y; //integer (-8192..8191)
}node_xy_28b_item;

typedef struct node_xy_32b_item_str {
	int x; //integer (-32768..32767)
	int y; //integer (-32768..32767)
}node_xy_32b_item;

typedef struct node_llmd_64b_item_str {
	long lon; //integer {onemicrodegreeeast (10), onemicrodegreewest (-10), unavailable(1800000001)} (-1800000000..1800000001)
	long lat; //integer {onemicrodegreenorth (10), onemicrodegreesouth (-10), unavailable(900000001)} (-900000000..900000001)
}node_llmd_64b_item;

typedef struct node_offset_point_item_str {
	unsigned char choice;

	/*choice 1: node_xy1*/
	node_xy_20b_item node_xy1;
	
	/*choice 2: node_xy2*/
	node_xy_22b_item node_xy2;

	/*choice 3: node_xy3*/
	node_xy_24b_item node_xy3;

	/*choice 4: node_xy4*/
	node_xy_26b_item node_xy4;

	/*choice 5: node_xy5*/
	node_xy_28b_item node_xy5;

	/*choice 6: node_xy6*/
	node_xy_32b_item node_xy6;

	/*choice 7: node_latlon*/
	node_llmd_64b_item node_lat_lon;

	/*choice 8: node_regional*/	
	/* nothing */
}node_offset_point_item;

typedef struct intersection_reference_id_item_str {
	unsigned char mask_region:1,
				  rest:7;

	unsigned int region; //op, integer (0..65535)
	unsigned int id; //integer (0..65535)
}intersection_reference_id_item;

typedef struct its_header_str {
	char protocol_version; /*integer(0..255)*/
	char message_id; /*integer(0..255)*/
	unsigned long station_id; /*integer(0..4294967295),unsigned long long = 8 bytes,unsigned long = 4 bytes(stationid)*/
}its_header_item;

typedef struct regulatory_speed_limit_str{
	unsigned char type; //enumerated {0..12}
	unsigned int speed; //integer (0..8191) -- units of 0.02 m/s
}regulatory_speed_limit_item;

typedef struct lane_type_attributes_str {
	unsigned char lane_type_choice;

	/*choice 1: vehicle*/
	unsigned short vehicle; // 16 bit  string

	/*choice 2: crosswalk*/
	unsigned short crosswalk; // 16 bit  string

	/*choice 3: bikelane*/
	unsigned short bike_lane; // 16 bit  string

	/*choice 4: sidewalk*/
	unsigned short sidewalk; // 16 bit  string

	/*choice 5: median*/
	unsigned short median; // 16 bit  string

	/*choice 6: striping*/
	unsigned short striping; // 16 bit  string

	/*choice 7: trackedvehicle*/
	unsigned short tracked_vehicle; // 16 bit  string

	/*choice 8: parking*/
	unsigned short parking; // 16 bit  string
}lane_type_attributes_item;


typedef struct lane_attributes_str{
	unsigned char mask_regional:1,				  
				  rest:7;

	unsigned char directional_use; // 2 bit string
	unsigned short shared_with; // 10 bit string
	lane_type_attributes_item lane_type;
}lane_attributes_item;

typedef struct altitude_str {
	long altitude_value; //integer {referenceellipsoidsurface(0), onecentimeter(1), unavailable(800001)} (-100000..800001)
	unsigned char altitude_confidence; //enumerated {0..15}
}altitude_item;

typedef struct reg_position3d_item_str {
	unsigned char mask_altitude:1,
				  rest:7;
	
	altitude_item altitude;
}reg_position3d_item;

typedef struct position3d_2_str {
	unsigned char mask_elevation:1,		
			  mask_regional:1,
			  rest:6;

	long lat; //integer {onemicrodegreenorth (10), onemicrodegreesouth (-10), unavailable(900000001)} (-900000000..900000001)
	long longitude; //integer {onemicrodegreeeast (10), onemicrodegreewest (-10), unavailable(1800000001)} (-1800000000..1800000001)
	int elevation; //RANG(-4095..61439); LSB = 0.1 meters; unknown: 61440; 
}position3d_2_item;

typedef struct lane_data_attribute_str {
	unsigned char lane_data_attribute_choice;

	/*choice 1: pathendpointangle*/
	int path_end_point_angle; //integer (-150..150) 

	/*choice 2: lanecrownpointcenter*/
	int lane_crown_point_center; //integer (-128..127)

	/*choice 3: lanecrownpointleft*/
	int lane_crown_point_left; ////integer (-128..127)

	/*choice 4: lanecrownpointright*/
	int lane_crown_point_right; ////integer (-128..127)

	/*choice 5: laneangle*/
	int lane_angle; //integer (-180..180)

	/*choice 6: speedlimits*/
	unsigned char speed_limits_num; 
	regulatory_speed_limit_item *speed_limits; //sequence (size(1..9)) of regulatoryspeedlimit

	/*choice 7: regional*/
	/* nothing */
}lane_data_attribute_item;

typedef struct node_attribute_set_str {
	unsigned char mask_local_node:1,
				  mask_disabled:1,
				  mask_enabled:1,
				  mask_data:1,
				  mask_regional:1,
				  mask_d_width:1,
				  mask_d_elevation:1,
				  rest:1;
	
	unsigned char local_node_num; 
	unsigned char *local_node; //op, sequence (size(1..8)) of nodeattribute, enumerated {0..11}

	unsigned char disabled_num; 
	unsigned char *disabled; //op, sequence (size(1..8)) of nodeattribute, enumerated {0..37}

	unsigned char enabled_num; 
	unsigned char *enabled; //op, sequence (size(1..8)) of nodeattribute, enumerated {0..37}

	unsigned char data_num;
	lane_data_attribute_item *data; //op, sequence (size(1..8)) of lanedataattribute

	int	d_width;	//op, integer (-512..511)
	int	d_elevation;	//op, integer (-512..511) 
}node_attribute_set_item;

typedef struct node_str {
	unsigned char mask_attributes:1,
				  rest:7;

	node_offset_point_item delta;
	node_attribute_set_item attributes; /*op*/
}node_item;

typedef struct computed_lane_str {
	unsigned char mask_rotate_xy:1,
				  mask_scale_xaxis:1,
				  mask_scale_yaxis:1,
				  mask_regional:1,
				  rest:4;
		
	unsigned char reference_lane_id; //integer (0..255)

	struct offset_xaxis_str {
		unsigned char offset_xaxis_choice;

		/*choice 1: small*/
		int small; //integer (-2047..2047)

		/*choice 2: large*/
		int large; //integer (-32767..32767)
	} offset_xaxis_item;

	struct offset_yaxis_str {
		unsigned char offset_yaxis_choice;		

		/*choice 1: small*/
		int small; //integer (-2047..2047)

		/*choice 2: large*/
		int large; //integer (-32767..32767)	
	} offset_yaxis_item;
	
	unsigned int rotate_xy; //op, integer (0..28800) 
	int scale_xaxis; //op, integer (-2048..2047)  -- in steps of 0.05 percent
	int scale_yaxis;	//op, integer (-2048..2047)  -- in steps of 0.05 percent
}computed_lane_item;

typedef struct node_list2_str{
	unsigned char node_list_choice;

	/*choice 1: nodes*/
	unsigned char nodes_num; 
	node_item *nodes; //sequence (size(2..63)) of node

	/*choice 2: computed*/
	computed_lane_item computed;
}node_list2_item;

typedef struct connecting_lane_str {
	unsigned char mask_maneuver:1,
				  rest:7;
	
	unsigned char lane; //integer (0..255)
	unsigned short maneuver; //op, bit string {size(12)}
}connecting_lane_item;

typedef struct connection_str {
	unsigned char mask_remote_intersection:1,
				  mask_signal_group:1,
				  mask_user_class:1,
				  mask_connection_id:1,
				  rest:4;
	
	connecting_lane_item connecting_lane;
	intersection_reference_id_item remote_intersection; /*op*/
	unsigned char signal_group; //op, integer (0..255)
	unsigned char user_class; //op, integer (0..255)
	unsigned char connection_id; //op, integer (0..255)	
}connection_item;

typedef struct generic_lane_str{
	unsigned char mask_name:1,
				  mask_ingress_approach:1,
				  mask_egress_approach:1,
				  mask_maneuvers:1,
				  mask_connects_to:1,
				  mask_overlays:1,
				  mask_regional:1,
				  rest:1;

	unsigned char lane_id; //integer (0..255)
	unsigned char name[64]; // op, ia5string (size(1..63))
	unsigned char ingress_approach; // op, integer (0..15) -- zero to be used when valid value is unknown
	unsigned char egress_approach; // op, integer (0..15) -- zero to be used when valid value is unknown
	lane_attributes_item lane_attributes;	
	unsigned short maneuvers; // op, 12 bit string
	node_list2_item node_list;	

	unsigned char connects_to_num; 
	connection_item *connects_to; //op, sequence (size(1..16)) of connection

	unsigned char overlays_num;
	unsigned char *overlays; //op, sequence (size(1..5)) of laneid
}generic_lane_item;

typedef struct intersection_geometry_str{
	unsigned char mask_name:1,		
				  mask_lane_width:1,	
				  mask_speed_limits:1,
				  mask_preempt_priority_data:1,
				  mask_regional:1,
				  rest:3;

	unsigned char name[64]; /*op, ia5string (size(1..63))*/
	intersection_reference_id_item id;	
	unsigned char revision; //integer (0..127)
	position3d_2_item ref_point;
	unsigned int lane_width; /*op, integer (0..32767) -- units of 1 cm*/

	unsigned char speed_limits_num;
	regulatory_speed_limit_item *speed_limits; //op, sequence (size(1..9)) of regulatoryspeedlimit
	
	unsigned char lane_set_num;
	generic_lane_item *lane_set; //sequence (size(1..255)) of genericlane
}intersection_geometry_item;

typedef struct data_parameters_str {
	unsigned char mask_process_method:1,
				  mask_process_agency:1,
				  mask_last_checked_date:1,
				  mask_geoid_used:1,
				  rest:4;

	unsigned char process_method[256]; /* op, ia5string (size(1..255))*/
	unsigned char process_agency[256]; /* op, ia5string (size(1..255))*/
	unsigned char last_checked_date[256]; /* op, ia5string (size(1..255))*/
	unsigned char geoid_used[256]; /* op, ia5string (size(1..255))*/
}data_parameters_item;

typedef struct road_segment_reference_id_str {
	unsigned char mask_region:1,
				  rest:7;
	
	unsigned int region; //op, integer (0..65535)
	unsigned int id; //integer (0..65535)
}road_segment_reference_id_item;

typedef struct road_segment_str {
	unsigned char mask_name:1,
				  mask_lane_width:1,
				  mask_speed_limits:1,
				  mask_regional:1,
				  resr:4;
	
	unsigned char name[64]; /* op, ia5string (size(1..63))*/
	road_segment_reference_id_item id;
	unsigned char revision; //integer (0..127)
	position3d_2_item ref_point;	
	unsigned int lane_width; //op, integer (0..32767) -- units of 1 cm

	unsigned char speed_limits_num;
	regulatory_speed_limit_item *speed_limits; //op, sequence (size(1..9)) of regulatoryspeedlimit
	
	unsigned char road_lane_set_num;
	generic_lane_item *road_lane_set; //sequence (size(1..255)) of genericlane
}road_segment_item;

typedef struct reg_restriction_user_type_str {
	unsigned char mask_emission:1,
				  rest:7;
	
	unsigned char emission; //op, //enumerated {0..4}
}reg_restriction_user_type_item;

typedef struct restriction_user_type_str {
	unsigned char users_choice;

	/*choice 1: basictype*/
	unsigned char basic_type; //enumerated {0..13}
	
	/*choice 2: regional*/
	/*nothing*/
}restriction_user_type_item;

typedef struct restriction_class_assignment_str {
	unsigned char id; //integer (0..255)

	unsigned char users_num;
	restriction_user_type_item *users; //sequence (size(1..16)) of  restrictionusertype
}restriction_class_assignment_item;

typedef struct signal_head_location_str {
	node_offset_point_item node;	
	int	 elevation; //integer (-1024..1023)
	unsigned char signal_group_id; //integer (0..255)	
}signal_head_location_item;

typedef struct reg_map_data_str {
	unsigned char mask_signal_head_locations:1,
				  rest:7;

	unsigned char signal_head_locations_num;	
	signal_head_location_item	*signal_head_locations; //op, sequence (size(1..20)) of  signalheadlocation	
}reg_map_data_item;

typedef struct time_change_details_str {
	unsigned char mask_start_time:1,
				  mask_max_end_time:1,
				  mask_likely_time:1,
				  mask_confidence:1,
				  mask_next_time:1,
				  rest:3;

	unsigned int start_time;	//op, integer (0..36002)
	unsigned int min_end_time; //integer (0..36002)
	unsigned int max_end_time; //op, integer (0..36002)
	unsigned int likely_time; //op, integer (0..36002)
	unsigned char confidence; //op, integer (0..15)
	unsigned int next_time; //op, integer (0..36002)
}time_change_details_item;

typedef struct advisory_speed_str {
	unsigned char mask_speed:1,
				  mask_confidence:1,
				  mask_distance:1,
				  mask_as_class:1,
				  mask_regional:1,
				  rest:3;
	
	unsigned char type; //enumerated {0..3}
	unsigned int speed; //op, integer (0..500) -- units of 0.1 m/s
	unsigned char confidence; //op, ENUMERATED (0..7)
	unsigned int distance; //op, integer (0..10000)
	unsigned char as_class; //op, integer (0..255)
}advisory_speed_item;

typedef struct movement_event_str {
	unsigned char mask_timing:1,
				  mask_speeds:1,
				  mask_regional:1,
				  rest:5;
		
	unsigned char event_state; //enumerated {0..9}
	time_change_details_item timing; /*op*/

	unsigned char speeds_num;
	advisory_speed_item *speeds; //op, sequence (size(1..16)) of advisoryspeed
}movement_event_item;

typedef struct vehicle_to_lane_position_str {
	unsigned long station_id; //integer(0..4294967295)
	unsigned char lane_id; //integer (0..255)
}vehicle_to_lane_position_item;

typedef struct reg_connection_maneuver_assist_str {
	unsigned char mask_vehicle_to_lane_positions:1,
				  mask_rsu_distance_from_anchor:1,
				  rest:6;

	unsigned char vehicle_to_lane_positions_num;
	vehicle_to_lane_position_item *vehicle_to_lane_positions; //op, sequence size(1..5) of vehicletolaneposition
	
	node_offset_point_item	rsu_distance_from_anchor;	/*op*/
}reg_connection_maneuver_assist_item;

typedef struct connection_maneuver_assist_str {
	unsigned char mask_queue_length:1,
				  mask_available_storage_length:1,
				  mask_wait_on_stop:1,
				  mask_ped_bicycle_detect:1,
				  mask_regional:1,
				  rest:3;

	unsigned char connection_id; //integer (0..255)
	unsigned int queue_length; //op, integer (0..10000)
	unsigned int available_storage_length; //op, integer (0..10000)
	unsigned char wait_on_stop; //op, boolean
	unsigned char ped_bicycle_detect; //op, boolean
}connection_maneuver_assist_item;

typedef struct movement_state_str {
	unsigned char mask_movement_name:1,
				  mask_maneuver_assist_list:1,
				  mask_regional:1,
				  rest:5;
	
	unsigned char movement_name[64]; //op, ia5string (size(1..63))
	unsigned char signal_group; //integer (0..255)

	unsigned char state_time_speed_num;
	movement_event_item *state_time_speed; //sequence (size(1..16)) of movementevent

	unsigned char maneuver_assist_list_num;
	connection_maneuver_assist_item *maneuver_assist_list; //op, sequence (size(1..16)) of connectionmaneuverassist
}movement_state_item;

typedef struct prioritization_response_str {
	unsigned long station_id; //integer(0..4294967295)
	unsigned char prior_state; //enumerated {0..6}
	unsigned char signal_group; //integer (0..255)
}prioritization_response_item;

typedef struct reg_intersection_state_str {
	unsigned char mask_active_prioritizations:1,
				  rest:7;

	unsigned char active_prioritizations_num;
	prioritization_response_item	*active_prioritizations; //op, sequence size(1..10) of prioritizationresponse
}reg_intersection_state_item;

typedef struct intersection_state_str {
	unsigned char mask_name:1,
				  mask_moy:1,
				  mask_time_stamp:1,
				  mask_enabled_lanes:1,
				  mask_maneuver_assist_list:1,
				  mask_priority:1,
				  mask_preempt:1,
				  mask_regional:1;

	unsigned char name[64]; //op, ia5string (size(1..63))
	intersection_reference_id_item id;
	unsigned char revision; //integer (0..127)
	unsigned short status; //bit string {size(16)}
	unsigned long moy; //op, integer (0..527040)
	unsigned int time_stamp; //op, integer (0..65535) -- units of miliseconds

	unsigned char enabled_lanes_num;
	unsigned char *enabled_lanes; //op, sequence (size(1..16)) of laneid, LaneID ::= INTEGER (0..255)

	unsigned char states_num;
	movement_state_item *states; //sequence (size(1..255)) of movementstate

	unsigned char maneuver_assist_list_num;
	connection_maneuver_assist_item *maneuver_assist_list; 	//op, sequence (size(1..16)) of connectionmaneuverassist 

	unsigned char priority; //op, octet string (size(1))
	unsigned char preempt; //op, octet string (size(1))	
}intersection_state_item;

typedef struct bsm_blob_str
{
	unsigned char msg_cnt; //INTEGER (0..127)
	unsigned char id[4]; //OCTET STRING (SIZE(4))	
	unsigned short sec_mark; //INTEGER (0..65535); units of miliseconds; unavailable: 65535

	int lat; //INTEGER (-900000000..900000001); LSB = 1/10 micro degree; unavailable: 900000001
	int longitude; //INTEGER (-1799999999..1800000001); LSB = 1/10 micro degree; unavailable: 1800000001
	int elev; //RANG(-4095..61439); LSB = 0.1 meters; unknown: 61440; 

	positional_accuracy_item accuracy;

	transmission_speed_item speed;
	
	unsigned short heading; //INTEGER (0..28800); LSB = 0.0125 degrees; unavailable: 28800
	signed char angle; //RANGE(-126..126); LSB = 1.5 degrees; unavailable: 127

	acceleration_set_4way_item accel_set;
	
	brake_system_status_item brakes;
	
	vehicle_size_item size;
}bsm_blob_item;

typedef struct heading_slice_str
{
	unsigned char from000_0to022_5degrees:1,
	from022_5to045_0degrees:1,
	from045_0to067_5degrees:1,
	from067_5to090_0degrees:1,
	from090_0to112_5degrees:1,
	from112_5to135_0degrees:1,
	from135_0to157_5degrees:1,
	from157_5to180_0degrees:1;
   
   unsigned char from180_0to202_5degrees:1,
   from202_5to225_0degrees:1,
   from225_0to247_5degrees:1,
   from247_5to270_0degrees:1,
   from270_0to292_5degrees:1,
   from292_5to315_0degrees:1,
   from315_0to337_5degrees:1,
   from337_5to360_0degrees:1;
}heading_slice_item;

typedef struct vehicle_request_status_str
{
	char msb;/* (msb)  brakes-on*/
	char emergency_use;/*emergency use or operation*/
	char lights_in_use;/*lights in use (see also the light bar element)*/
	char bit5;
	char lightbar_in_use;
	char transist_status;/* -- when a priority, map the values of 
						-- lightbarinuse to the lower 4 bits 
						-- and set the 5th bit to zero
						-- when a preemption, map the values of 
						-- transiststatus to the lower 5 bits
						*/				
}vehicle_request_status_item;

typedef struct transit_status_str
{
	char none;	
	char an_ad_ause;
	char a_bike_load;
	char door_open;
	char occ_m;
	char occ_l;
}transit_status_item;


int rand_J2735_temporaryID( unsigned int *return_temp_id , int if_random );
int rand_J2735_msgCnt( unsigned char *return_msgCnt , int if_random );
int parser_DSRC_msgID(  char input_buf[] , size_t input_buf_size , unsigned char *return_msgID );
#endif /* __SAE_COMMON_H_ */
