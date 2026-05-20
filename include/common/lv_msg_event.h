#ifndef LV_MSG_EVENT_H_
#define LV_MSG_EVENT_H_
#include <stdbool.h>
#include <stdio.h>
#include "../share/include/lvgl/lvgl.h"
#include "../share/include/tuya/tuya_api.h"
typedef enum
{
	MSG_EVENT_CMD_DOOR1_CALL = 0X0000,
	MSG_EVENT_CMD_DOOR2_CALL,
	MSG_EVENT_CMD_SENSOR1_TRIGGER,
	MSG_EVENT_CMD_SENSOR2_TRIGGER,
	MSG_EVENT_CMD_SD_STATE,
	MSG_EVENT_CMD_INTERCOM,

	MSG_EVENT_CMD_CALL_TALK_BTN,

	MSG_EVENT_CMD_TUYA,
	MSG_EVENT_CMD_UNLOCK_BTN,
} MSG_CMD;

typedef enum
{
	TUYA_EVENT_CMD_VIDEO_START,
	TUYA_EVENT_CMD_VIDEO_STOP,
	TUYA_EVENT_CMD_AUDIO_START,
	TUYA_EVENT_CMD_ONLINE,
	TUYA_EVENT_CMD_CH_CHANGE,
	TUYA_EVENT_CMD_MOTION_ENBALE,
	TUYA_EVENT_CMD_DOOR_OPEN,
	TUYA_EVENT_CMD_DOOR_OPEN_2,
	
	TUYA_EVENT_CMD_UPDATE_MODE,

	TUYA_EVENT_CMD_LEAVE_MODE,
	TUYA_EVENT_CMD_AT_MODE,
	TUYA_EVENT_CMD_SLEEP_MODE,
	TUYA_EVENT_CMD_ALAM_UPDATE_MONITOR,
	TUYA_EVENT_CMD_ALAM_SWITCH,
	TUYA_EVENT_CMD_AUTO_MONITOR_UPDATE,

	TUYA_EVENT_CMD_REBOOT,
	TUYA_EVENT_CMD_SD_FORMAT,

	TUYA_EVENT_CMD_WEATHER_CHANGE,
	TUYA_EVENT_CMD_MILIEU_CHANGE,
	TUYA_EVENT_CMD_GAS_CHANGE
} TUYA_CMD;
/***
** 日期: 2022-04-25 16:13
** 作者: leo.liu
** 函数作用：layout页面属性
** 返回参数说明：
***/
typedef struct
{
	void (*enter)(void);
	void (*quit)(void);
} layout;
/***
** 日期: 2022-04-25 16:14
** 作者: leo.liu
** 函数作用：创建一个页面
** 返回参数说明：
***/
#define CREATE_LAYOUT(x) layout layout_##x = {            \
			     .enter = layout_##x##_enter, \
			     .quit = layout_##x##_quit};
/***
** 日期: 2022-04-25 16:14
** 作者: leo.liu
** 函数作用：进入此页面的第一个执行的函数
** 返回参数说明：
***/
#define LAYOUT_ENTER_FUNC(x) layout_##x##_enter(void)
/***
** 日期: 2022-04-25 16:14
** 作者: leo.liu
** 函数作用：退出此页面的最后一个函数
** 返回参数说明：
***/
#define LAYOUT_QUIT_FUNC(x) layout_##x##_quit(void)
/***
** 日期: 2022-04-25 16:15
** 作者: leo.liu
** 函数作用：页面地址
** 返回参数说明：
***/
#define pLAYOUT(x) &layout_##x
/***
** 日期: 2022-04-25 16:15
** 作者: leo.liu
** 函数作用：声明页面
** 返回参数说明：
***/
#define DEFINE_LAYOUT(x) extern layout layout_##x
/***
** 日期: 2022-04-25 16:31
** 作者: leo.liu
** 函数作用：页面跳转
** 返回参数说明：
***/
bool goto_layout(const layout *layout);
/***
**   日期:2022-05-23 13:41:22
**   作者: leo.liu
**   函数作用：获取当前页面
**   参数说明:
***/
const layout *cur_layout_get(void);
/***
** 日期: 2022-04-25 16:16
** 作者: leo.liu
** 函数作用：按钮相关属性
** 返回参数说明：
***/
typedef struct
{
	void (*down)(lv_obj_t *obj);
	void (*up)(lv_obj_t *obj);
	void (*anything_func)(lv_obj_t *obj, lv_event_t event);
} obj_click_data;
/***
** 日期: 2022-04-25 16:17
** 作者: leo.liu
** 函数作用：获取按钮相关属性
** 返回参数说明：
***/
#define obj_click_data_create(down_ex, up_ex) {.down = down_ex, \
					       .up = up_ex,     \
					       .anything_func = NULL};

#define obj_click_data_up_create(x) { \
    .down = NULL,                     \
    .up = x,                          \
    .anything_func = NULL};

#define obj_click_data_anything_create(x) { \
    .down = NULL,                           \
    .up = NULL,                             \
    .anything_func = x};

/***
** 日期: 2022-04-25 16:18
** 作者: leo.liu
** 函数作用：监听此控件的点击事件
** 返回参数说明：
***/
void obj_click_event_listen(lv_obj_t *obj, const obj_click_data *click_data);
/***
** 日期: 2022-04-28 09:49
** 作者: leo.liu
** 函数作用：注册控件被按下执行的函数
** 返回参数说明：
***/
bool lv_obj_click_down_callback_register(void (*callback)(lv_obj_t *));
/***
** 日期: 2022-04-25 16:13
** 作者: leo.liu
** 函数作用：lv任务消息任务初始化
** 返回参数说明：
***/
void lv_event_task_init(void);
/***
** 日期: 2022-05-12 10:28
** 作者: leo.liu
** 函数作用：注册door1呼叫执行函数
** 返回参数说明：
***/
bool layout_door1_call_callback_register(void (*callback)(void));
/***
** 日期: 2022-05-12 10:28
** 作者: leo.liu
** 函数作用：注册door2呼叫执行函数
** 返回参数说明：
***/
bool layout_door2_call_callback_register(void (*callback)(void));
/***
** 日期: 2022-05-17 14:47
** 作者: leo.liu
** 函数作用：sdcard状态注册
** 返回参数说明：
***/
bool layout_sd_state_callback_register(void (*callback)(void));
/***
** 日期: 2022-05-12 10:34
** 作者: leo.liu
** 函数作用：发送doo1 call event
** 返回参数说明：
***/
bool lv_msg_send_cmd(unsigned int cmd, unsigned int arg1, unsigned int arg2);
/***
**   日期:2022-05-28 08:07:55
**   作者: leo.liu
**   函数作用：警报1执行注册
**   参数说明:
***/
bool layout_alarm1_trigger_callback_register(void (*callback)(void));
/***
**   日期:2022-05-28 08:07:55
**   作者: leo.liu
**   函数作用：警报1执行注册
**   参数说明:
***/
bool layout_alarm2_trigger_callback_register(void (*callback)(void));
/***
**   日期:2022-06-07 15:37:04
**   作者: leo.liu
**   函数作用：tuya事件执行注册
**   参数说明:
***/
bool layout_tuya_event_callback_register(bool (*callback)(TUYA_CMD cmd, int arg));
/***
**   日期:2022-05-31 08:46:12
**   作者: leo.liu
**   函数作用：注册天气变化处理函数
**   参数说明:
***/
bool layout_tuya_sub_event_weather_callback_register(bool (*callback)(tuya_api_weather* weather));
/***
**   日期:2022-05-31 08:46:12
**   作者: leo.liu
**   函数作用：注册处理intercom事件
**   参数说明:
***/
bool layout_call_talk_btn_callback_register(void (*callback)(void));

bool layout_unclock_btn_callback_register(void (*callback)(void));
/***
**   日期:2022-05-31 08:46:12
**   作者: leo.liu
**   函数作用：注册处理intercom事件
**   参数说明:
***/
bool layout_intercom_event_callback_register(bool (*callback)(unsigned int send_id, unsigned int cmd));
#endif