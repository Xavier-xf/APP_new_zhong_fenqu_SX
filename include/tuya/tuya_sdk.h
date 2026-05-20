#ifndef TUYA_SDK_H
#define TUYA_SDK_H

#include <stdbool.h>
#include  "tuya_cloud_types.h"
#include  "tuya_ipc_api.h"
#define IPC_APP_PID   "8hbui777kxqebtpk"

#define BCOMTECH_DP_LIGHT              (138)
#define BCOMTECH_DP_DOORBELL             (136)
#define BCOMTECH_DP_OUTDOOR_LOCK                 (148)
#define BCOMTECH_DP_INDORR_GATE2              (233)
#define BCOMTECH_DP_PICTURE              (154)
#define BCOMTECH_DP_ALARM_MSG            (185)
#define BCOMTECH_DP_WORK_MODE            (189)
#define BCOMTECH_DP_SWITCH_CHANNEL       (231)
#define TUYA_DP_SD_STORAGE_ONLY_GET	(109)
#define TUYA_DP_SD_STATUS_ONLY_GET	(110)
#define BCOMTECH_DP_ABNORMAL_LOCK      (232)
#define BCOMTECH_DP_OUTDOOR_GATE1      (232)
#define BCOMTECH_DP_ABSENT_MODE          (234)
#define BCOMTECH_DP_DEVICE_ACTIVE        (235)
#define BCOMTECH_DP_DEVICE_SCREENSHOT        (236)

typedef enum
{
	/*切换监控*/
	TUYA_EVENT_MONITOR_SWAP,

	/*开灯*/
	TUYA_EVENT_LIGHT_ON,

	/*开锁*/
	TUYA_EVENT_OPEN_LOCK,

	/*开锁2*/
	TUYA_EVENT_OPEN_GATE1,

	/*开门*/
	TUYA_EVENT_OPEN_GATE2,

	/*模式切换*/
	TUYA_EVENT_WORK_MODE,

    /*Absent mode*/
    TUYA_EVENT_ABSENT_MODE,
	
	/*通话*/
	TUYA_EVENT_TALK,

	/*进入监控*/
	TUYA_EVENT_MONITOR_ENTER,

	/*退出监控*/
	TUYA_EVENT_MONITOR_QUIT,

	/* 截屏 */
	TUYA_EVENT_SCREENSHOT,

	/* 门钟检测 */
	TUYA_EVENT_DOOR_CHRIME,
	
	/* 涂鸦监控 */
	TUYA_EVENT_MONITOR_ING,
}tuya_event;

typedef struct
{
    int temp;
    int condition;
    int humidity;
    int pressure;
    int pm10;
    int pm25;
	char city[32];
}tuya_api_weather;


void tuya_ipc_ring_buffer_video_release_data(void);//释放涂鸦队列音视频数据

bool wifi_work_restart(void);//重新连接涂鸦服务器

bool tuya_wifi_sdk_init(const char *pid,char *uuid,char *key);//初始化涂鸦库
bool is_online_tuya_cloud(void);


void tuya_current_channel_set(int channel);//设置APP预览通道

void tuya_set_current_language(int language);//设置上传APP的语言

int tuya_get_current_language(void);//

int is_tuya_cloud_connected_num(void);//当前APP连接室内机的数目

int tuya_switch_channel_upload_results(int channel);//设置通道返回的结果

int tuya_channel_valid_report(void);//上传有效通道到APP

int tuya_dp_138_response_light_switch(BOOL_T state);

int tuya_dp_148_response_accessory_lock(BOOL_T state);//设置开锁状态

int tuya_dp_233_response_gate2(BOOL_T state);//设置室内机开关门

int tuya_dp_189_response_work_mode(UINT_T mode);

int tuya_dp_232_response_abnormal_lock(BOOL_T state);

int tuya_dp_232_response_outdoor_gate1(BOOL_T state);

int tuya_dp_234_response_absent_mode(BOOL_T state);//离家模式

int tuya_dp_235_response_device_active(void);//

int tuya_dp_uploads_security_msg(char id , char *data,int size);


void update_tuya_lock_state(BOOL_T state);

void set_tuya_channel_state(int channel, bool state);

bool tuya_net_time_sync(struct tm *local_time);

IPC_REGISTER_STATUS tuya_ipc_register_status_get(void);

void tuya_network_dev_set(bool dev);

#endif

