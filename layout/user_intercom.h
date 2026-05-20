#ifndef USER_INTERCOM_H_
#define USER_INTERCOM_H_
#include <stdbool.h>
#include "user_data.h"
#include "user_monitor.h"
#define CODE_START 0x1F // start cmd
#define CODE_STOP 0XF1	// stop  cmd
#define CODE_ALL_ID 0XFF

/////////////////////////////	cmd		//////////////////////////////////////////////
#define OwnID (user_data_get()->etc.deive_id + 1)

/*****  查询intercom状态（只有本机发送到主线程） *****/
#define CMD_INTERPHONE_QUERY 0X10

// interphone call interphone
#define CMD_INTERPHONE_CALL 0XA0
#define CMD_INTERPHONE_JOIN 0XA1
#define CMD_INTERPHONE_ANSWER 0XA2
#define CMD_INTERPHONE_QUIT 0XA3

// interphone call monitor
#define CMD_DOOR_ENTER 0xA4
#define CMD_MONITOR_DOOR1 0XA5
#define CMD_MONITOR_DOOR2 0XA6

#define CMD_SWITCH_DOOR1 0XA7
#define CMD_SWITCH_DOOR2 0XA8
#define CMD_SWITCH_CCTV1 0XA9
#define CMD_SWITCH_CCTV2 0XAA

#define CMD_CCTV_ENTER 0XAB
#define CMD_MONITOR_CCTV1 0XAC
#define CMD_MONITOR_CCTV2 0XAD

#define CMD_DOORTOCCTV1 0XAE
#define CMD_DOORTOCCTV2 0XAF

#define CMD_CCTVTODOOR1 0XB0
#define CMD_CCTVTODOOR2 0XB1

#define CMD_CCTV1_SIGNAL_TRUE 0XB2
#define CMD_CCTV1_SIGNAL_FALSE 0XB3
#define CMD_CCTV2_SIGNAL_TRUE 0XB4
#define CMD_CCTV2_SIGNAL_FALSE 0XB5

#define CMD_DOOR1_CALL 0XB6
#define CMD_DOOR2_CALL 0XB7
#define CMD_DOOR_CALL_JOIN 0XB8
#define CMD_DOOR_CALL_QUIT 0XB9

#define CMD_INDOOR1_CALL 0XBA
#define CMD_INDOOR2_CALL 0XBB
#define CMD_INDOOR_CALL_ANSWER 0XBC

#define CMD_DOOR1_AUDIO 0XBD
#define CMD_DOOR2_AUDIO 0XBE

#define CMD_DOOR1_UNLOCK_OPEN 0XBF
#define CMD_DOOR2_UNLOCK_OPEN 0XE0
#define CMD_DOOR_UNLOCK_CLOSE 0XE1

#define CMD_DOOR_FASTMODE 0XE2

// alarm
#define CMD_ALARM_SENSOR1 0XE3
#define CMD_ALARM_SENSOR2 0XE4
#define CMD_ALARM_SENSOR1_AGAIN 0XE5
#define CMD_ALARM_SENSOR2_AGAIN 0XE6
#define CMD_ALARM_RING_STOP 0XE7
#define CMD_ALARM_GET_STATUS 0XE8
#define CMD_ALARM_QUIT 0XE9
#define CAM_ALARM_STOP 0XEA

/*****  新增door2开锁数目 *****/
#define CMD_DOOR2_UNLOCK_NUM_1 0xEB
#define CMD_DOOR2_UNLOCK_OPEN_2 0xED

#define CMD_DATA_BUSY_NORMAL 0XF8 // busy normal
#define CMD_DATA_BUSY_ACK 0XF9	  // busy ack
#define CMD_DATA_BUSY_UNACK 0XFA  // send bsuy unack
// PUBLIC
#define CMD_DATA_TIMESET 0XFB
#define CMD_DATA_REBOOT 0XFC
#define CMD_DATA_BUSY 0XFD
#define CMD_DATA_RELEASE 0XFE

#define CMD_LOCK_TIME_SET 0XC0
/***
**   日期:2022-05-30 17:54:02
**   作者: leo.liu
**   函数作用：初始化intercom
**   参数说明:
***/
bool intercom_init(void);
/***
**   日期:2022-05-31 09:18:54
**   作者: leo.liu
**   函数作用：发送串口命令
**   参数说明:
***/
bool intercom_cmd_send(unsigned int receive_id, unsigned char cmd);
/***
**   日期:2022-05-31 11:52:01
**   作者: leo.liu
**   函数作用：设置主机id
**   参数说明:
***/
void intercom_mastar_id_set(int id);
/***
**   日期:2022-05-31 11:52:01
**   作者: leo.liu
**   函数作用：获取主机id
**   参数说明:
***/
int intercom_mastar_id_get(void);
/***
**   日期:2022-05-31 14:13:46
**   作者: leo.liu
**   函数作用：使能busy
**   参数说明:
***/
void intercom_data_busy_enbale(bool en);
/***
**   日期:2022-05-31 14:13:55
**   作者: leo.liu
**   函数作用：获取状态
**   参数说明:
***/
bool intercom_data_busy_get(void);
/***
**   日期:2022-06-01 09:57:30
**   作者: leo.liu
**   函数作用：发送cctv1的信号状态
**   参数说明:
***/
bool intercom_cctv_signale_state_send(unsigned int receive_id, MON_CH ch);
/***
**   日期:2022-06-02 08:36:31
**   作者: leo.liu
**   函数作用：busy解除心跳包标记
**   参数说明:
***/
void intercom_busy_ack_mask_set(unsigned int mask);
/***
**   日期:2022-06-02 08:36:31
**   作者: leo.liu
**   函数作用：busy解除心跳包获取标记
**   参数说明:
***/
unsigned int intercom_busy_ack_mask_get(void);
/***
**   日期:2022-06-02 08:50:16
**   作者: leo.liu
**   函数作用：开启intercom busy定时器
**   参数说明:
***/
void intercom_busy_timer_start(unsigned int time);
/***
**   日期:2022-06-02 08:50:45
**   作者: leo.liu
**   函数作用：删除定时器
**   参数说明:
***/
void intercom_busy_timer_del(void);
/***
**   日期:2022-06-18 09:36:49
**   作者: leo.liu
**   函数作用：intercom时间同步
**   参数说明:
***/
bool intercom_user_time_sync_send(unsigned int receive_id, struct tm *tm);
/***
**   日期:2022-07-05 09:52:30
**   作者: leo.liu
**   函数作用：发送door2开锁数目
**   参数说明:
***/
bool intercom_door2_unlock_num_send(unsigned int receive_id, int num);
#endif