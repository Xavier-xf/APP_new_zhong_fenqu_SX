#include "lv_msg_event.h"
#include "user_common.h"
#include "user_standby.h"
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "video_decode.h"
#include "ringplay.h"
#include "video_decode.h"

// extern void fb_gui_layer_rect_fill(unsigned int color);

/***
** 日期: 2022-04-25 16:31
** 作者: leo.liu
** 函数作用：消息队列信息
** 返回参数说明：
***/
typedef struct
{
	unsigned int cmd;
	unsigned int arg1;
	unsigned int arg2;
} lv_msg_event_data;

typedef struct
{
	long type;
	lv_msg_event_data msg;
} lv_msg_queue;

/***
** 日期: 2022-04-25 16:31
** 作者: leo.liu
** 函数作用：当前页面指针
** 返回参数说明：
***/
static const layout *playout = NULL;

/***
** 日期: 2022-04-25 16:21
** 作者: leo.liu
** 函数作用：lv任务的句柄
** 返回参数说明：
***/
static int lv_msg_queue_head = -1;

/***** door1 call处理函数 *****/
static void (*layout_door1_call_pfunc)(void) = NULL;
static void (*layout_door2_call_pfunc)(void) = NULL;
static void (*layout_alarm1_trigger_pfunc)(void) = NULL;
static void (*layout_alarm2_trigger_pfunc)(void) = NULL;
static void (*lyaout_sd_state_pfunc)(void) = NULL;

static void (*lyaout_unclock_pfunc)(void) = NULL;

static void (*lyaout_call_talk_btn_pfunc)(void) = NULL;

static bool (*layout_intercom_event_pfunc)(unsigned int send_id, unsigned int cmd) = NULL;
static bool (*layout_tuya_event_func)(TUYA_CMD cmd, int) = NULL;
/***
** 日期: 2022-04-25 16:26
** 作者: leo.liu
** 函数作用：lvgl 外部事件获取任务
** 返回参数说明：
***/
static void lv_msg_event_task(lv_task_t *task)
{
	lv_msg_queue event;
	if (msgrcv(lv_msg_queue_head, (void *)&event, sizeof(lv_msg_queue), sizeof(lv_msg_event_data), IPC_NOWAIT) > 0)
	{
		//	printf("lv_msg_event : receive:%d,%d,%d \n", event.msg.cmd, event.msg.arg1, event.msg.arg2);
		switch (event.msg.cmd)
		{
		case MSG_EVENT_CMD_DOOR1_CALL:
			if (layout_door1_call_pfunc != NULL)
			{
				layout_door1_call_pfunc();
			}
			break;
		case MSG_EVENT_CMD_DOOR2_CALL:
			if (layout_door2_call_pfunc != NULL)
			{
				layout_door2_call_pfunc();
			}
			break;
		case MSG_EVENT_CMD_SENSOR1_TRIGGER:
			if (layout_alarm1_trigger_pfunc != NULL)
			{
				layout_alarm1_trigger_pfunc();
			}
			break;
		case MSG_EVENT_CMD_SENSOR2_TRIGGER:
			if (layout_alarm2_trigger_pfunc != NULL)
			{
				layout_alarm2_trigger_pfunc();
			}
			break;
		case MSG_EVENT_CMD_CALL_TALK_BTN:
			if (lyaout_call_talk_btn_pfunc != NULL)
			{
				lyaout_call_talk_btn_pfunc();
			}
			break;
		case MSG_EVENT_CMD_SD_STATE:
			if (lyaout_sd_state_pfunc != NULL)
			{
				lyaout_sd_state_pfunc();
			}
			break;
		case MSG_EVENT_CMD_UNLOCK_BTN:
			if (lyaout_unclock_pfunc != NULL)
			{
				lyaout_unclock_pfunc();
			}
			break;
		case MSG_EVENT_CMD_INTERCOM:
			if (layout_intercom_event_pfunc != NULL)
			{
				layout_intercom_event_pfunc(event.msg.arg1, event.msg.arg2);
			}
			break;
		case MSG_EVENT_CMD_TUYA:
			if (layout_tuya_event_func != NULL)
			{
				layout_tuya_event_func(event.msg.arg1, event.msg.arg2);
			}

		default:
			break;
		}
	}
	else
	{
		extern bool standby_timeout_check_and_process(void);
		standby_timeout_check_and_process();
	}
}

/***
** 日期: 2022-04-25 16:13
** 作者: leo.liu
** 函数作用：lv任务消息队列初始化
** 返回参数说明：
***/
void lv_event_task_init(void)
{
	lv_msg_queue_head = msg_queue_create();

	/***** 清空队列中的数据 *****/
	lv_msg_queue event;
	while (msgrcv(lv_msg_queue_head, (void *)&event, sizeof(lv_msg_queue), sizeof(lv_msg_event_data), IPC_NOWAIT) > 0)
	{
	}

	extern lv_task_t *lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void *user_data);
	lv_task_t *task_t = lv_task_create(lv_msg_event_task, 30, LV_TASK_PRIO_MID, NULL);
	lv_task_ready(task_t);
}

/***
** 日期: 2022-04-25 16:31
** 作者: leo.liu
** 函数作用：页面跳转
** 返回参数说明：
***/
bool goto_layout(const layout *en_layout)
{
	if ((en_layout == NULL) || (en_layout->enter == NULL))
	{
		return false;
	}
	lv_task_clean();
	lv_anim_del_all();
	const layout *old_layout = playout;
	playout = en_layout;
	if ((old_layout != NULL) && (old_layout->quit != NULL))
	{
		old_layout->quit();
	}
	lv_obj_clean(lv_scr_act());
	refresh_area_t area = {0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX};
	gui_refresh_area(&area, 1);
	playout->enter();
	return true;
}

/***
**   日期:2022-05-23 13:41:22
**   作者: leo.liu
**   函数作用：获取当前页面
**   参数说明:
***/
const layout *cur_layout_get(void)
{
	return playout;
}

/***
** 日期: 2022-05-12 10:34
** 作者: leo.liu
** 函数作用：发送doo1 call event
** 返回参数说明：
***/
bool lv_msg_send_cmd(unsigned int cmd, unsigned int arg1, unsigned int arg2)
{
	lv_msg_queue msg_queue;
	msg_queue.type = sizeof(lv_msg_event_data);
	msg_queue.msg.cmd = cmd;
	msg_queue.msg.arg1 = arg1;
	msg_queue.msg.arg2 = arg2;
	msgsnd(lv_msg_queue_head, &msg_queue, msg_queue.type, 0);
	return true;
}

/***
** 日期: 2022-04-28 09:49
** 作者: leo.liu
** 函数作用：所有控件被按下的统一回调函数
** 返回参数说明：
***/
static void (*lv_obj_click_down_callback)(lv_obj_t *obj) = NULL;

/***
** 日期: 2022-04-28 09:49
** 作者: leo.liu
** 函数作用：注册控件被按下执行的函数
** 返回参数说明：
***/
bool lv_obj_click_down_callback_register(void (*callback)(lv_obj_t *))
{
	lv_obj_click_down_callback = callback;
	return true;
}

static void obj_click_event_handler(lv_obj_t *obj, lv_event_t event)
{
	obj_click_data *clict_data = (obj_click_data *)obj->clict_data;

	if (clict_data == NULL)
	{
		return;
	}
	if (event == LV_EVENT_PRESSED)
	{
		if (lv_obj_click_down_callback != NULL)
		{
			lv_obj_click_down_callback(obj);
		}
		standby_timer_restart(false);
	}
	if (clict_data->anything_func != NULL)
	{
		clict_data->anything_func(obj, event);
	}
	else if ((event == LV_EVENT_PRESSED) && (clict_data->down != NULL))
	{
		clict_data->down(obj);
	}
	else if ((event == LV_EVENT_CLICKED) && (clict_data->up != NULL))
	{
		clict_data->up(obj);
	}
}

void obj_click_event_listen(lv_obj_t *obj, const obj_click_data *lick_data)
{
	lv_obj_set_click(obj, true);
	obj->clict_data = lick_data;
	lv_obj_set_event_cb(obj, obj_click_event_handler);
}

/***
** 日期: 2022-05-12 10:28
** 作者: leo.liu
** 函数作用：注册door1呼叫执行函数
** 返回参数说明：
***/
bool layout_door1_call_callback_register(void (*callback)(void))
{
	layout_door1_call_pfunc = callback;
	return true;
}

/***
** 日期: 2022-05-12 10:28
** 作者: leo.liu
** 函数作用：注册door2呼叫执行函数
** 返回参数说明： 
***/
bool layout_door2_call_callback_register(void (*callback)(void))
{
	layout_door2_call_pfunc = callback;
	return true;
}

/***
** 日期: 2022-05-17 14:47
** 作者: leo.liu
** 函数作用：sdcard状态注册
** 返回参数说明：
***/
bool layout_sd_state_callback_register(void (*callback)(void))
{
	lyaout_sd_state_pfunc = callback;
	return true;
}
/***
**   日期:2022-05-28 08:07:55
**   作者: leo.liu
**   函数作用：警报1执行注册
**   参数说明:
***/
bool layout_alarm1_trigger_callback_register(void (*callback)(void))
{
	layout_alarm1_trigger_pfunc = callback;
	return true;
}
/***
**   日期:2022-05-28 08:07:55
**   作者: leo.liu
**   函数作用：警报1执行注册
**   参数说明:
***/
bool layout_alarm2_trigger_callback_register(void (*callback)(void))
{
	layout_alarm2_trigger_pfunc = callback;
	return true;
}
/***
**   日期:2022-05-31 08:46:12
**   作者: leo.liu
**   函数作用：注册处理intercom事件
**   参数说明:
***/
bool layout_intercom_event_callback_register(bool (*callback)(unsigned int send_id, unsigned int cmd))
{
	layout_intercom_event_pfunc = callback;
	return true;
}
/***
**   日期:2022-06-07 15:37:04
**   作者: leo.liu
**   函数作用：tuya事件执行注册
**   参数说明:
***/
bool layout_tuya_event_callback_register(bool (*callback)(TUYA_CMD cmd, int arg1))
{
	layout_tuya_event_func = callback;
	return true;
}


/***
** 日期: 2022-05-12 10:28
** 作者: leo.liu
** 函数作用：注册接听执行函数
** 返回参数说明： 
***/
bool layout_call_talk_btn_callback_register(void (*callback)(void))
{
	lyaout_call_talk_btn_pfunc = callback;
	return true;
}






/***
** 日期: 2023-08-18 14:47
** 作者: fa.liu
** 函数作用：按键开锁状态函数
** 返回参数说明：
***/
bool layout_unclock_btn_callback_register(void (*callback)(void))
{
	lyaout_unclock_pfunc = callback;
	return true;
}









/***
** 日期: 2022-05-12 10:28
** 作者: feian.liu
** 函数作用：屏幕点击事件
** 返回参数说明：
***/

void obj_screen_click_event_register(void (*callback)(lv_obj_t*))
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_click(screen,callback ? true : false);


    static obj_click_data click_cb = obj_click_data_create(NULL,NULL);
    click_cb.up = callback;

    screen->clict_data = &click_cb;
    lv_obj_set_event_cb(screen, obj_click_event_handler);

}