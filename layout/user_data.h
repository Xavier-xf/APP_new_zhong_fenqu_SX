#ifndef _USER_DATA_H_
#define _USER_DATA_H_
#include "stdbool.h"
#include <time.h>

typedef struct
{
	bool enable;
	/***** 0:door1 ,1:door2 --- 3:cctv2 *****/
	char select_camera;
	/***** 0:photo 1:video *****/
	char saving_fmt;
	/***** 灵敏度 0：low,1:middle,2:height *****/
	char sensivity;
	/***** 定时器 *****/
	bool timer_en;
	/***** lcd *****/
	bool lcd_en;

	struct tm start;
	struct tm end;
} user_motion_info;

typedef struct
{
	bool key_sound;
	bool ring_mute;
	/***** 1-6 rings *****/
	int door1_tone;
	int door2_tone;
	int inter_tone;

	int door_ring_volume;
	int door_talk_volume;
	int inter_ring_volume;
	int inter_talk_volume;
} user_audio_info;

typedef struct
{
	int bright;
	int cont;
	int color;
} isp_info;

typedef struct
{
	/***** 0:lcd off 1:open frame *****/
	char standby_mode;
	/***** lcd的亮度 *****/
	int lcd_brigtness;

	/***** open frame time:0:always 1:时间段*****/
	char frame_time_en;
	/***** 转换为分钟 （hh：mm）*****/
	int frame_time_start;
	int frame_time_end;

	/***** 夜间模式 *****/
	bool night_mode;
	int night_time_start;
	int night_time_end;
	/***** 显示列表 0b 11111 低五位表示显示位*****/
	int frame_list;
	/***** 帧背景显示 *****/
	char frame_background;

	isp_info door1;
	isp_info door2;
	isp_info cctv1;
	isp_info cctv2;
} user_display_info;

typedef struct
{
	/***** 0:english *****/
	char language;
	/***** 0：id1  1：id2 *****/
	char deive_id;
	/***** 0:标准模式，1快速模式 *****/
	char open_the_door;

	/***** door1开锁模式 0:camera locak ,1：digital door lock*****/
	char door1_open_door_mode;
	/***** 1:1lock 2:2lock *****/
	char door2_lock_num;
	/*****  password *****/
	char password[4];
	unsigned char door_1_open_time;
	unsigned char door_2_open_time;
	unsigned char tuya_send_time;
	bool tuya_send_switch;


} user_etc_info;

typedef struct
{
	bool auto_record;

	bool alarm_1_enable;
	bool alarm_1_trigger;

	bool alarm_2_enable;
	bool alarm_2_trigger;
	bool alarm_enable;

} user_alarm_info;


typedef struct
{
	bool wifi_open_flag;//wifi打开标志位 0:关闭 1:打开
	bool wifi_connect_flag;

	char *wifi_name;//已经连接的wifi名字 
	char *wifi_pwd;//已经连接的wifi密码
}user_wifi_info;




// typedef struct
// {
// 	int door1_timer_start;
// 	int door1_timer_end;
// 	int door2_timer_start;
// 	int door2_timer_end;

// 	int door1_ring_time;
// 	int door2_ring_time;

// 	int door1_ring_mode;
// 	int door2_ring_mode;
	
// 	int door1_ring;
// 	int door2_ring;
// 	int door1_custom_ring;
// 	int door2_custom_ring;

// 	int door1_ring_val;
// 	int door2_ring_val;
// }user_ring_info;
// user_ring_info ring_attr;


typedef struct
{
	int timer_start;
	int timer_end;

	int ring_time;
	
	int ring_mode;

	int ring;

	int custom_ring;

	int ring_val;
}ring_info;

typedef struct {

	char ip[20];
	char url[3][256];	
	char user[20];
	char pswd[20];
	int  port;		/* 端口号 */
	int  brand;		/* [-1]-IPC  [0]-DAHUA  [1]-HIKVISION  [2]-XM [3]-RTSP*/

}user_onvif_info;

typedef struct
{
	char auto_record_mode; // 0:off,1:video,2:photo;
	bool wifi_enable;
	bool weather_enable;

	bool door1_always_enable;
	bool door2_always_enable;
	bool home_wifi_display;
	// user_ring_info ring1;
	// user_ring_info ring2;
	// user_ring_info ring3;

	ring_info ring_attr[2][3];


	char home_mode;
	char monitor_alarm;
	user_motion_info motion;

	user_audio_info audio;

	user_wifi_info wifi;

	user_display_info display;

	user_etc_info etc;

	user_alarm_info alarm;
	char tuya_qrcode_info[128];
	char get_record_flag; // 0:motion,1:monitor,2:always;
	user_onvif_info     onvif_dev[2];
	char mac_qr_info[128];
	char always_recode_mode;
} user_data_info;




bool user_data_save(void);
bool user_data_init(void);
user_data_info *user_data_get(void);
void user_data_reset(void);

#endif
