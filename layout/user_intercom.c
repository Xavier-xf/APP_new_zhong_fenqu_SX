#include "user_intercom.h"
#include <pthread.h>
#include "uart_ctrl.h"
#include "lv_msg_event.h"
#include "user_standby.h"
#include "user_gpio.h"
#include "layout_define.h"
#include "ringplay.h"
#include "tuya/tuya_api.h"
#include "user_common.h"
#include "info.h"
#define DELAY_LEN 100
#define Interphone_delay() OwnID == 1 ? usleep(DELAY_LEN * 3 * 1000) : usleep(DELAY_LEN *(OwnID - 2) * 1000)
typedef bool (*_InterPhoneReceiveEventHanle)(unsigned int send_id);





typedef struct
{
	unsigned short tm_year;
	unsigned char tm_mday;	// 1 ~ 31
	unsigned char tm_mon;	// 1 ~ 12
	unsigned char tm_wday;	// 0 ~ 6
	unsigned char tm_hour;	// 0 ~ 23
	unsigned char tm_min;	// 0 ~ 59
	unsigned char tm_sec;	// 0 ~ 59
	unsigned short tm_msec; // 0 ~ 999
} USER_INTERCOM_TM;

typedef struct
{
	unsigned char lock1_time;
	unsigned char lock2_time;
} USER_INTERCOM_LOCKTIME;

typedef struct
{
	char str[30];
	unsigned char cmd;
	_InterPhoneReceiveEventHanle handler;
} intercom_param_info;
static bool intercom_event_cmd_process(unsigned int send_id, unsigned int cmd);
/*****  串口读写句柄 *****/
static int intercom_uart_fd = -1;

/***
**   日期:2022-05-31 08:22:16
**   作者: leo.liu
**   函数作用：校验intercom和
**   参数说明:
***/
static bool intercom_uart_check_sum(char *buf)
{
	unsigned char checknum = (buf[0] + buf[1] + buf[2]) & 0xFF;

	if (buf[1] != OwnID && buf[1] != CODE_ALL_ID)
	{
		// printf("checksum error :send to id : %d \n\r",buf[1]);
		return false;
	}
	else if ((buf[0] == OwnID) && (buf[1] != OwnID))
	{
		printf(" checksum error : receive owd id : %d \n",OwnID);
		return false;
	}
	else if (checknum != buf[3])
	{
		// printf("checksum error : check number error \n\r");
		return false;
	}
	/* else if ((!EnterSystemStatus) &&
		 (buf[2] != CMD_ALARM_GET_STATUS ||
		  buf[2] != CMD_ALARM_SENSOR1 ||
		  buf[2] != CMD_ALARM_SENSOR2 ||
		  buf[2] != CMD_DATA_REBOOT))
	{
		return FALSE;
	} */
	return true;
}

/***
**   日期:2022-05-31 08:16:24
**   作者: leo.liu
**   函数作用：读取串口格式命令
**   参数说明:
***/
static bool intercom_cmd_read(char *buffer, int size)
{
	if (uart_read(intercom_uart_fd, &buffer[0], 1) < 0)
	{
		return false;
	}
	if ((buffer[0] == CODE_START) && (uart_read(intercom_uart_fd, &buffer[1], 5) == 5))
	{
		if ((buffer[5] == CODE_STOP) && (intercom_uart_check_sum(&buffer[1]) == true))
		{
			return true;
		}
	}
	return false;
}

/***
**   日期:2022-05-31 08:39:46
**   作者: leo.liu
**   函数作用：发送命令处理
**   参数说明:
***/
static void intercom_uart_event_send(unsigned int id, unsigned int cmd)
{
	lv_msg_send_cmd(MSG_EVENT_CMD_INTERCOM, id, cmd);
}

/***
**   日期:2022-06-18 09:40:03
**   作者: leo.liu
**   函数作用：处理时间同步
**   参数说明:
***/
static void intercom_time_sync_receive(void)
{
	usleep(10 * 1000);
	USER_INTERCOM_TM inter_tm;
	uart_read(intercom_uart_fd, (char *)&inter_tm, sizeof(USER_INTERCOM_TM));
	printf("%d %d %d %d %d %d \n", inter_tm.tm_year, inter_tm.tm_mon, inter_tm.tm_mday, inter_tm.tm_hour, inter_tm.tm_min, inter_tm.tm_sec);
	struct tm tm;
	tm.tm_year = inter_tm.tm_year;
	tm.tm_mon = inter_tm.tm_mon;
	tm.tm_mday = inter_tm.tm_mday;
	tm.tm_hour = inter_tm.tm_hour;
	tm.tm_min = inter_tm.tm_min;
	tm.tm_sec = inter_tm.tm_sec;
	if (user_time_valid_check(&tm) == true)
	{
		standby_timer_close();
		user_time_set(&tm);
		standby_timer_restart(true);
	}
}

/***
**   日期:2022-06-18 09:40:03
**   作者: leo.liu
**   函数作用：处理开锁时间同步
**   参数说明:
***/
static void intercom_locktime_sync_receive(void)
{
	usleep(10 * 1000);
	USER_INTERCOM_LOCKTIME inter_lock_time;
	uart_read(intercom_uart_fd, (char *)&inter_lock_time, sizeof(USER_INTERCOM_LOCKTIME));

	unsigned char lock1_time;
	unsigned char lock2_time;
	lock1_time = inter_lock_time.lock1_time;
	lock2_time = inter_lock_time.lock2_time;

	// if((user_data_etc_check_range_out(lock1_time, 1, 60)) && (user_data_etc_check_range_out(lock2_time, 1, 60)))
	// {
		user_data_get()->etc.door_1_open_time = lock1_time;
		user_data_get()->etc.door_2_open_time = lock2_time;

	// }
}
/***
**   日期:2022-05-31 08:27:58
**   作者: leo.liu
**   函数作用：处理户户通命令
**   参数说明:
***/
static void intercom_uart_cmd_proc(const char *buf)
{
	char send_id = buf[1];
	char cmd = buf[3];
	if (cmd == CMD_DATA_TIMESET)
	{
		intercom_time_sync_receive();
	}
	else if(cmd == CMD_LOCK_TIME_SET)
	{
		intercom_locktime_sync_receive();
	}
	else
	{
		intercom_uart_event_send(send_id, cmd);
	}
}

/***
**   日期:2022-05-30 18:02:20
**   作者: leo.liu
**   函数作用：intercom接收任务
**   参数说明:
***/
static void *intercom_receive_task(void *arg)
{
	printf("***** intercom task create sccess ! *****\n");
	char buffer[6] = {0};
	unsigned int intercom_query_count = 0;
	while (1)
	{
		if (intercom_cmd_read(buffer, 6) == true)
		{
			intercom_uart_cmd_proc(buffer);
		}
		else if ((intercom_query_count++) > 100)
		{
			intercom_query_count = 0;
			intercom_uart_event_send(OwnID, CMD_INTERPHONE_QUERY);
		}
		usleep(1000 * 1);
	}
	return NULL;
}
/***
**   日期:2022-05-30 17:54:02
**   作者: leo.liu
**   函数作用：初始化intercom
**   参数说明:
***/
bool intercom_init(void)
{
	intercom_uart_fd = uart_open("ttySAK1", 9600, 8, 1, 'n');
	if (intercom_uart_fd < 0)
	{
		printf("open ttySAK1 faild \n");
		usleep(1000 * 1000);
		return false;
	}
	pthread_t thread_t;
	layout_intercom_event_callback_register(intercom_event_cmd_process);
	pthread_create(&thread_t, user_pthread_atter_get(), intercom_receive_task, NULL);
	return true;
}
/************************************************************************ intercom 通用接口 ***********************************************************************/
/*****  主机id *****/
static int intercom_mastar_id = 0;
/***
**   日期:2022-05-31 11:52:01
**   作者: leo.liu
**   函数作用：设置主机id
**   参数说明:
***/
void intercom_mastar_id_set(int id)
{
	intercom_mastar_id = id;
}
/***
**   日期:2022-05-31 11:52:01
**   作者: leo.liu
**   函数作用：获取主机id
**   参数说明:
***/
int intercom_mastar_id_get(void)
{
	return intercom_mastar_id;
}
/*****  busy变量 *****/
static bool intercom_data_busy = false;
/***
**   日期:2022-05-31 14:13:46
**   作者: leo.liu
**   函数作用：使能busy
**   参数说明:
***/
void intercom_data_busy_enbale(bool en)
{
	intercom_data_busy = en;
}
/***
**   日期:2022-05-31 14:13:55
**   作者: leo.liu
**   函数作用：获取状态
**   参数说明:
***/
bool intercom_data_busy_get(void)
{
	return intercom_data_busy;
}
/***
**   日期:2022-06-02 08:36:31
**   作者: leo.liu
**   函数作用：busy解除心跳包
**   参数说明:
***/
static unsigned int intercom_busy_ack_mask = 0x00;
void intercom_busy_ack_mask_set(unsigned int mask)
{
	intercom_busy_ack_mask = mask;
}
unsigned int intercom_busy_ack_mask_get(void)
{
	return intercom_busy_ack_mask;
}

/***
**   日期:2022-06-02 08:49:58
**   作者: leo.liu
**   函数作用：忙定时器处理
**   参数说明:
***/
static lv_task_t *intercom_busy_timer_ptask = NULL;
extern lv_task_t *lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void *user_data);
static void intercom_busy_timer_task(lv_task_t *task_t)
{
	intercom_cmd_send(OwnID, CMD_DATA_BUSY_UNACK);
	intercom_busy_timer_del();
}

/***
**   日期:2022-06-02 08:51:46
**   作者: leo.liu
**   函数作用：定时器任务
**   参数说明:
***/
static void intercom_busy_timer_check_task(lv_task_t *task_t)
{
	intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY_ACK);
	intercom_busy_timer_del();
	printf("=====================%s+++++++++++++++++++++++\n", __func__);
	intercom_busy_timer_ptask = lv_task_create(intercom_busy_timer_task, 3000, LV_TASK_PRIO_MID, NULL);

}
/***
**   日期:2022-06-02 08:50:45
**   作者: leo.liu
**   函数作用：删除定时器
**   参数说明:
***/
void intercom_busy_timer_del(void)
{
	if (intercom_busy_timer_ptask != NULL)
	{
		lv_task_del(intercom_busy_timer_ptask);
		intercom_busy_timer_ptask = NULL;
	}
}
/***
**   日期:2022-06-02 08:50:16
**   作者: leo.liu
**   函数作用：开启intercom busy定时器
**   参数说明:
***/
void intercom_busy_timer_start(unsigned int time)
{
	intercom_busy_timer_del();
	intercom_busy_timer_ptask = lv_task_create(intercom_busy_timer_check_task, time, LV_TASK_PRIO_MID, NULL);

}
/************************************************************************** 接收处理范围 **************************************************************************/

/***
**   日期:2022-05-31 09:01:17
**   作者: leo.liu
**   函数作用：intercom 发送呼叫命令
**   参数说明:
***/
static bool intercom_intercom_call_receive_event(unsigned int send_id)
{

	intercom_mastar_id = send_id;
	standby_timer_restart(true);

	layout_intercom_call_mask_set(0x01 << (send_id - 1));
	goto_layout(pLAYOUT(intercom_in));

	Interphone_delay();
	intercom_cmd_send(send_id, CMD_INTERPHONE_JOIN);
	return true;
}
/***
**   日期:2022-05-31 11:25:29
**   作者: leo.liu
**   函数作用：加入call的行列
**   参数说明:
***/
static bool intercom_intercom_join_receive_event(unsigned int send_id)
{
	unsigned int mask = layout_intercom_call_mask_get();
	if (mask & (0x01 << (send_id - 1)))
	{
		mask |= 0x10 << (send_id - 1);
		layout_intercom_call_mask_set(mask); 
	}

	if (cur_layout_get() == pLAYOUT(intercom))
	{
		goto_layout(pLAYOUT(intercom_out));
	}

	if(cur_layout_get() == pLAYOUT(intercom_out))
	{
		lv_obj_t *room1 = lv_obj_get_child_form_id(lv_scr_act(), 7);
		lv_obj_t *room2 = lv_obj_get_child_form_id(lv_scr_act(), 8);
		lv_obj_t *room3 = lv_obj_get_child_form_id(lv_scr_act(), 9);

		int room1_num = user_data_get()->etc.deive_id == 0 ? 2 : 1;
		int room2_num = user_data_get()->etc.deive_id < 2 ? 3 : 2;
		int room3_num = user_data_get()->etc.deive_id < 3 ? 4 : 3;

		if(send_id == room1_num)
		{
			lv_obj_set_state(room1,LV_STATE_CHECKED);
		}

		else if(send_id == room2_num)
		{
			lv_obj_set_state(room2,LV_STATE_CHECKED);
		}

		else if(send_id == room3_num)
		{
			lv_obj_set_state(room3,LV_STATE_CHECKED);
		}

	}


	return true;
}
/***
**   日期:2022-05-31 11:44:14
**   作者: leo.liu
**   函数作用:同意通话
**   参数说明:
***/
static bool intercom_intercom_answer_receive_event(unsigned int send_id)
{
	const layout *playout = cur_layout_get();
	if ((playout != pLAYOUT(intercom_out)) && (playout != pLAYOUT(intercom_talk)) && (playout != pLAYOUT(intercom)))
	{
		return false;
	}

	if (OwnID == intercom_mastar_id)
	{
		unsigned int mask = layout_intercom_call_mask_get();
		if ((mask & 0xF00) == 0x00)
		{
			goto_layout(pLAYOUT(intercom_talk));
		}
		mask |= (0x0111 << (send_id - 1));
		layout_intercom_call_mask_set(mask);
		printf("intercom call mask:%03x \n", mask);
	}
	return true;
}

/***
**   日期:2022-05-31 14:11:31
**   作者: leo.liu
**   函数作用：接收到busy
**   参数说明:
***/
static bool intercom_data_busy_receive_event(unsigned int send_id)
{
	intercom_data_busy_enbale(true);
	layout_intercom_call_mask_set(0x00);
	if (OwnID != 1)
	{
		video_switch_enable(MON_CH_INTERCOM, 0x02);
	}
	else
	{
		const layout *cur_p = cur_layout_get();
		if ((cur_p == pLAYOUT(close)) && (user_data_get()->motion.enable == true) )
		{
			goto_layout(pLAYOUT(close));
		}
		else if(cur_p == pLAYOUT(frame_show))
		{
			goto_layout(pLAYOUT(close));
		}
	}
	return true;
}
/***
**   日期:2022-05-31 15:13:29
**   作者: leo.liu
**   函数作用：释放数据总线
**   参数说明:
***/
static bool intercom_data_release_receive_event(unsigned int send_id)
{
	const layout *playout = cur_layout_get();

	intercom_data_busy_enbale(false);
	intercom_mastar_id_set(0x00);

	layout_intercom_call_mask_set(0x00);
	if(!tuya_api_app_talk_check())
	{
		door_audio_talk(AUDIO_CH_CLOSE);
	}
	monitor_enter_mask_set(MON_ENTER_NONE);
	/*****  关闭锁 *****/
	monitor_unlcok_close();

	if((playout != pLAYOUT(tuya_monitor)) && (playout != pLAYOUT(always_record))){

		monitor_close();
	}

	if ((playout != pLAYOUT(alarm)) && (alarm_trigger_check() == true))
	{
		return true;
	}

	if ((playout == pLAYOUT(intercom)) || (playout == pLAYOUT(intercom_in)) || (playout == pLAYOUT(intercom_out)) || (playout == pLAYOUT(intercom_talk)) || (playout == pLAYOUT(monitor)) || (playout == pLAYOUT(tuya_monitor)))
	{
		goto_layout(pLAYOUT(home));
		return true;
	}

	if (playout == pLAYOUT(alarm))
	{
		power_amplifier_enable(true);
	}
	else if ((playout == pLAYOUT(video)) && (video_play_status_get() == VIDEO_PLAY_STATE_PLAY))
	{
		power_amplifier_enable(true);

	}
	// else
	// {
	// 	if(playout != pLAYOUT(tuya_monitor))
	// 	{
	// 		monitor_close();
	// 	}
	// 	// else
	// 	// {
	// 	// 	/*****退出样品流 *****/
	// if(tuya_online_num_get_customize() <= 0)
	// {
	// 	h264_encode_sample_enable(false);
	// }
	return true;
}

static bool intercom_intercom_quit_receive_event(unsigned int send_id)
{
	if (send_id == intercom_mastar_id)
	{
		intercom_data_busy_enbale(false);
		door_audio_talk(AUDIO_CH_CLOSE);
		goto_layout(pLAYOUT(home));
	}
	else if (OwnID == intercom_mastar_id)
	{
		unsigned int mask = layout_intercom_call_mask_get();
		unsigned int mk = (0x100 << (send_id - 1)) & mask;
		/*****  mk不等于，说明处于通话状态 *****/
		if (mk)
		{
			intercom_data_busy_enbale(false);
			door_audio_talk(AUDIO_CH_CLOSE);
			goto_layout(pLAYOUT(home));
		}
		else
		{
			mask &= ~(0x111 << (send_id - 1));
			layout_intercom_call_mask_set(mask);
			if (mask == 0)
			{
				intercom_data_busy_enbale(false);
				door_audio_talk(AUDIO_CH_CLOSE);
				goto_layout(pLAYOUT(home));
			}
		}
	}
	return true;
}
static bool intercom_monitor_door_enter_receive_event(unsigned int send_id)
{
	/*****  id1发送cctv1/2的信号状态 *****/
	intercom_cctv_signale_state_send(CODE_ALL_ID, MON_CH_CCTV1);
	intercom_cctv_signale_state_send(CODE_ALL_ID, MON_CH_CCTV2);

	if ((monitor_valid_channel_check(MON_CH_DOOR1) == false) && (monitor_valid_channel_check(MON_CH_DOOR2) == false))
	{
		return false;
	}

	monitor_enter_mask_set(MON_ENTER_MANUAL_DOOR);
	if (monitor_valid_channel_check(MON_CH_DOOR1) == true)
	{
		monitor_channel_set(MON_CH_DOOR1);
		// door_audio_talk(AUDIO_CH_DOOR1);
		intercom_cmd_send(send_id, CMD_MONITOR_DOOR1);
	}
	else if (monitor_valid_channel_check(MON_CH_DOOR2) == true)
	{
		monitor_channel_set(MON_CH_DOOR2);
		// door_audio_talk(AUDIO_CH_DOOR2);
		intercom_cmd_send(send_id, CMD_MONITOR_DOOR2);
	}
	if(intercom_data_busy_get() == false)
	{
		goto_layout(pLAYOUT(home));
	}
	monitor_open(false, 0x02);
	MON_CH ch = monitor_channel_get();
	intercom_audio_enable((ch == MON_CH_DOOR1) ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2);

	return true;
}
static bool intercom_monitor_enter_door1_receive_event(unsigned int send_id)
{
	const layout *p = cur_layout_get();
	if (p == pLAYOUT(home))
	{
		intercom_mastar_id_set(OwnID);
		monitor_channel_set(MON_CH_DOOR1);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
		monitor_enter_mask_set(MON_ENTER_MANUAL_DOOR);
		goto_layout(pLAYOUT(monitor));
	}
	else if (p == pLAYOUT(setting_display))
	{
		monitor_channel_set(MON_CH_DOOR1);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
		monitor_enter_mask_set(MON_ENTER_DISPLAY);
		goto_layout(pLAYOUT(adj_display));
	}
	return true;
}
static bool intercom_monitor_enter_door2_receive_event(unsigned int send_id)
{
	const layout *p = cur_layout_get();
	if (p == pLAYOUT(home))
	{
		intercom_mastar_id_set(OwnID);
		monitor_channel_set(MON_CH_DOOR2);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
		monitor_enter_mask_set(MON_ENTER_MANUAL_DOOR);
		goto_layout(pLAYOUT(monitor));
	}
	else if (p == pLAYOUT(setting_display))
	{
		monitor_channel_set(MON_CH_DOOR2);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
		monitor_enter_mask_set(MON_ENTER_DISPLAY);
		goto_layout(pLAYOUT(adj_display));
	}
	return true;
}
static bool intercom_monitor_switch_door1_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		if (monitor_valid_channel_check(MON_CH_DOOR1) == false)
		{
			return false;
		}
		intercom_cmd_send(send_id, CMD_SWITCH_DOOR1);
		usleep(50 * 1000);
		monitor_channel_set(MON_CH_DOOR1);
		intercom_audio_enable(AUDIO_CH_DOOR1);
		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_door_swap_btn_up(lv_obj_t * obj);
		layout_monitor_door_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_switch_door2_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		if (monitor_valid_channel_check(MON_CH_DOOR2) == false)
		{
			return false;
		}
		intercom_cmd_send(send_id, CMD_SWITCH_DOOR2);
		usleep(50 * 1000);
		monitor_channel_set(MON_CH_DOOR2);
		intercom_audio_enable(AUDIO_CH_DOOR2);
		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_door_swap_btn_up(lv_obj_t * obj);
		layout_monitor_door_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_switch_cctv1_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		if (monitor_valid_channel_check(MON_CH_CCTV1) == false)
		{
			return false;
		}
		intercom_cmd_send(send_id, CMD_SWITCH_CCTV1);
		usleep(50 * 1000);
		monitor_channel_set(MON_CH_CCTV1);
		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_cctv_swap_btn_up(lv_obj_t * obj);
		layout_monitor_cctv_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_switch_cctv2_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		if (monitor_valid_channel_check(MON_CH_CCTV2) == false)
		{
			return false;
		}
		intercom_cmd_send(send_id, CMD_SWITCH_CCTV2);
		usleep(50 * 1000);
		monitor_channel_set(MON_CH_CCTV2);
		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_cctv_swap_btn_up(lv_obj_t * obj);
		layout_monitor_cctv_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_cctv_enter_receive_event(unsigned int send_id)
{
	if ((monitor_valid_channel_check(MON_CH_CCTV1) == false) && (monitor_valid_channel_check(MON_CH_CCTV2) == false))
	{
		return false;
	}
	monitor_enter_mask_set(MON_ENTER_MANUAL_CCTV);
	if (monitor_valid_channel_check(MON_CH_CCTV1) == true)
	{
		monitor_channel_set(MON_CH_CCTV1);
		intercom_cmd_send(send_id, CMD_MONITOR_CCTV1);
	}
	else
	{
		monitor_channel_set(MON_CH_CCTV2);
		intercom_cmd_send(send_id, CMD_MONITOR_CCTV2);
	}
	if(intercom_data_busy_get() == false)
	{
		goto_layout(pLAYOUT(home));
	}
	monitor_open(false, 0x02);
	return true;
}
static bool intercom_monitor_enter_cctv1_receive_event(unsigned int send_id)
{
	const layout *p = cur_layout_get();
	if (p != pLAYOUT(home))
	{
		return false;
	}
	monitor_valid_channel_set(MON_CH_CCTV1, true);
	intercom_mastar_id_set(OwnID);
	monitor_channel_set(MON_CH_CCTV1);
	intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
	monitor_enter_mask_set(MON_ENTER_MANUAL_CCTV);
	goto_layout(pLAYOUT(monitor));
	return true;
}
static bool intercom_monitor_enter_cctv2_receive_event(unsigned int send_id)
{
	const layout *p = cur_layout_get();
	if (p != pLAYOUT(home))
	{
		return false;
	}
	monitor_valid_channel_set(MON_CH_CCTV2, true);
	intercom_mastar_id_set(OwnID);
	monitor_channel_set(MON_CH_CCTV2);
	intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
	monitor_enter_mask_set(MON_ENTER_MANUAL_CCTV);
	goto_layout(pLAYOUT(monitor));
	return true;
}

static bool intercom_monitor_door_to_cctv1_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		if (monitor_valid_channel_check(MON_CH_CCTV1) == false)
		{
			return false;
		}
		intercom_cmd_send(send_id, CMD_DOORTOCCTV1);
		monitor_channel_set(MON_CH_CCTV1);

		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_cctv_swap_btn_up(lv_obj_t * obj);
		layout_monitor_cctv_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_door_to_cctv2_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		if (monitor_valid_channel_check(MON_CH_CCTV2) == false)
		{
			return false;
		}
		intercom_cmd_send(send_id, CMD_DOORTOCCTV2);
		monitor_channel_set(MON_CH_CCTV1);
		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_cctv_swap_btn_up(lv_obj_t * obj);
		layout_monitor_cctv_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_cctv_to_door1_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		monitor_channel_set(MON_CH_DOOR1);
		intercom_cmd_send(send_id, CMD_CCTVTODOOR1);
		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_cctv_swap_btn_up(lv_obj_t * obj);
		layout_monitor_cctv_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_cctv_to_door2_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		monitor_channel_set(MON_CH_DOOR2);
		intercom_cmd_send(send_id, CMD_CCTVTODOOR2);
		monitor_open(false, 0x02);
	}
	else if (cur_layout_get() == pLAYOUT(monitor))
	{
		extern void layout_monitor_cctv_swap_btn_up(lv_obj_t * obj);
		layout_monitor_cctv_swap_btn_up(NULL);
	}
	return true;
}
static bool intercom_monitor_cctv1_signal_true_receive_event(unsigned int send_id)
{
	monitor_valid_channel_set(MON_CH_CCTV1, true);
	return true;
}
static bool intercom_monitor_cctv1_signal_false_receive_event(unsigned int send_id)
{
	monitor_valid_channel_set(MON_CH_CCTV1, false);
	return true;
}
static bool intercom_monitor_cctv2_signal_true_receive_event(unsigned int send_id)
{
	monitor_valid_channel_set(MON_CH_CCTV2, true);
	return true;
}
static bool intercom_monitor_cctv2_signal_false_receive_event(unsigned int send_id)
{
	monitor_valid_channel_set(MON_CH_CCTV2, false);
	return true;
}
static bool intercom_monitor_door1_call_recevice_event(unsigned int send_id)
{
	// video_input_skip_frame_count_set(1000);                 //设置过滤帧数，分机闪频
	MON_ENTER_FLG flag = monitor_enter_mask_get();
	if (flag == MON_ENTER_TALK)
	{
		monitor_channel_set(MON_CH_DOOR1);
		monitor_open(false, 0x02);
	}
	else
	{
		MON_CH ch =monitor_channel_get();
		if(ch != MON_CH_DOOR1)
		{
			video_input_skip_frame_count_set(1000);
		}
		lv_msg_send_cmd(MSG_EVENT_CMD_DOOR1_CALL, 0, 0);
		if ((flag != MON_ENTER_CALL) || (cur_layout_get() == pLAYOUT(monitor)))
		{
			Interphone_delay();
			intercom_cmd_send(send_id, CMD_DOOR_CALL_JOIN);
		}
	}
	return true;
}
static bool intercom_monitor_door2_call_recevice_event(unsigned int send_id)
{

	MON_ENTER_FLG flag = monitor_enter_mask_get();
	if (flag == MON_ENTER_TALK)
	{
		monitor_channel_set(MON_CH_DOOR2);
		monitor_open(false, 0x02);
	}
	else
	{
		MON_CH ch =monitor_channel_get();
		if(ch != MON_CH_DOOR2)
		{
			video_input_skip_frame_count_set(1000);
		}
		lv_msg_send_cmd(MSG_EVENT_CMD_DOOR2_CALL, 0, 0);
		if ((flag != MON_ENTER_CALL) || (cur_layout_get() == pLAYOUT(monitor)))
		{
			Interphone_delay();
			intercom_cmd_send(send_id, CMD_DOOR_CALL_JOIN);
		}
	}
	return true;
}
static bool intercom_monitor_call_join_receive_event(unsigned int send_id)
{
	if (OwnID != 1)
	{
		return false;
	}
	unsigned int mask = layout_intercom_call_mask_get();
	mask |= 0x01 << (send_id - 1);
	mask |= 0x01;
	layout_intercom_call_mask_set(mask);
	printf("monitor call join:%03x \n", mask);
	return true;
}
static bool intercom_monitor_call_quit_receive_event(unsigned int send_id)
{
	unsigned int mask = layout_intercom_call_mask_get();
	printf("##############=======mask====[%d]\n",mask);
	mask &= ~(0x01 << (send_id - 1));
	mask &= 0x0F;
	LOG_WHITE("monitor call quit:%03x \n", mask);
	layout_intercom_call_mask_set(mask);
	if ((mask == 0) )
	{
		monitor_enter_mask_set(MON_ENTER_NONE);
		if((tuya_api_app_preview_check() == false))
		{
			door_audio_talk(AUDIO_CH_CLOSE);
			monitor_unlcok_close();
			monitor_close();

			intercom_mastar_id_set(0);
			intercom_data_busy_enbale(false);
			LOG_GREEN("set release 8\n");
			intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);

			alarm_trigger_check();
		}
	}
	// monitor_open(true,0x03);
	// video_switch_enable(MON_CH_INTERCOM, 0x03);
	return true;
}
static bool intercom_monitor_door1_in_call_recevice_event(unsigned int send_id)
{
	MON_CH ch =monitor_channel_get();
	if(ch != MON_CH_DOOR1)
	{
		video_input_skip_frame_count_set(1000);
	}
	lv_msg_send_cmd(MSG_EVENT_CMD_DOOR1_CALL, 0, 0);
	return true;
}
static bool intercom_monitor_door2_in_call_recevice_event(unsigned int send_id)
{
	MON_CH ch =monitor_channel_get();
	if(ch != MON_CH_DOOR2)
	{
		video_input_skip_frame_count_set(1000);
	}
	lv_msg_send_cmd(MSG_EVENT_CMD_DOOR2_CALL, 0, 0);
	return true;
}
static bool intercom_monitor_call_answer_recevice_event(unsigned int send_id)
{
	// MON_ENTER_FLG flg = monitor_enter_mask_get();
	if (cur_layout_get() == pLAYOUT(monitor) /*&& (flg != MON_ENTER_CALL)*/)
	{
		goto_layout(pLAYOUT(home));
	}
	intercom_data_busy_enbale(true);
	layout_intercom_call_mask_set(MON_ENTER_TALK);
	door_audio_talk(AUDIO_CH_CLOSE);
	return true;
}
static bool intercom_monitor_call_door1_audio_recevice_event(unsigned int send_id)
{
	//unsigned int mask = layout_intercom_call_mask_get();
	printf("layout_intercom_call_mask_get== %d\n",layout_intercom_call_mask_get());
	if (OwnID == 1)
	{
		// mask &= ~(0x01 << (send_id - 1));
		// layout_intercom_call_mask_set(mask);
		video_switch_enable(MON_CH_DOOR1, 0x02);
		h264_encode_sample_enable(true);

		//  extern int tuya_ipc_door_bell_press_stop();
		//  tuya_ipc_door_bell_press_stop();
		tuya_api_doorbell_handup();
		 
		tuya_pvew_video_count_fource_set(0);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_STOP, 0);

	}
	else
	{
		video_switch_enable(MON_CH_INTERCOM, 0x02);
	}
	MON_ENTER_FLG flg = monitor_enter_mask_get();
	if (flg != MON_ENTER_TALK)
	{
		monitor_enter_mask_set(MON_ENTER_TALK);
		ringplay_play_stop();
	}
	intercom_data_busy_enbale(true); 
	if (cur_layout_get() == pLAYOUT(monitor))
	{
		goto_layout(pLAYOUT(home));
	}
	intercom_audio_enable(AUDIO_CH_DOOR1);

	
	return true;
}
static bool intercom_monitor_call_door2_audio_recevice_event(unsigned int send_id)
{
	//unsigned int mask = layout_intercom_call_mask_get();
	if (OwnID == 1)
	{
		// mask &= ~(0x01 << (send_id - 1));
		// layout_intercom_call_mask_set(mask);
		video_switch_enable(MON_CH_DOOR2, 0x02);
		h264_encode_sample_enable(true);

		//  extern int tuya_ipc_door_bell_press_stop();
		//  tuya_ipc_door_bell_press_stop();
		tuya_api_doorbell_handup();

		tuya_pvew_video_count_fource_set(0);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_STOP, 0);
	}
	else
	{
		video_switch_enable(MON_CH_INTERCOM, 0x02);
	}
	MON_ENTER_FLG flg = monitor_enter_mask_get();
	if (flg != MON_ENTER_TALK)
	{
		monitor_enter_mask_set(MON_ENTER_TALK);
		ringplay_play_stop();
	}
	intercom_data_busy_enbale(true);
	if (cur_layout_get() == pLAYOUT(monitor))
	{
		goto_layout(pLAYOUT(home));
	}
	intercom_audio_enable(AUDIO_CH_DOOR2);

	
	return true;
}
static bool intercom_monitor_call_door2_unlock_recevice_event(unsigned int send_id){

	monitor_unlock_open(1,monitor_channel_get());
	if(cur_layout_get() == pLAYOUT(monitor)){

		monitor_unlock_mask_set(MON_UNLOCK_TALK);
	}
	if(cur_layout_get() != pLAYOUT(video))
	{
		#ifdef MAOTOUYING_DAN_IC
		ringplay_play_form_index(7, 100,   door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
		#else
		ringplay_play_form_index(12, 100,   door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
		#endif
	}
	
	
	return true;
}

static bool intercom_monitor_door_unlock_close_recevice_event(unsigned int send_id)
{
	monitor_unlcok_close();
	return true;
}
static bool intercom_data_reboot_receive_event(unsigned int send_id)
{
	intercom_mastar_id_set(0x00);
	intercom_data_busy_enbale(false);
	monitor_enter_mask_set(MON_ENTER_NONE);
	monitor_enter_mask_set(0x00);
	door_audio_talk(AUDIO_CH_CLOSE);
	monitor_unlcok_close();
	const layout *playout = cur_layout_get();
	if ((playout == pLAYOUT(intercom)) || (playout == pLAYOUT(intercom_in)) || (playout == pLAYOUT(intercom_out)) || (playout == pLAYOUT(intercom_talk))|| (playout == pLAYOUT(monitor)))
	{
		goto_layout(pLAYOUT(home));
	}
	
	return true;
}
static bool intercom_alarm_sensor_1_receive_event(unsigned int send_id)
{
	lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR1_TRIGGER, 0, 0);
	return true;
}
static bool intercom_alarm_sensor_2_receive_event(unsigned int send_id)
{
	lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR2_TRIGGER, 0, 0);
	return true;
}
static bool intercom_alarm_sensor_quit_receive_event(unsigned int send_id)
{
	const layout *p = cur_layout_get();
	user_data_get()->alarm.alarm_1_trigger = false;
	user_data_get()->alarm.alarm_2_trigger = false;
	if (p == pLAYOUT(alarm))
	{
		goto_layout(pLAYOUT(home));
	}
	return true;
}
static bool intercom_alarm_sensor_stop_receive_event(unsigned int send_id)
{
	printf("You don't do anything \n");
	return true;
}
static bool intercom_alarm_sensor_1_again_receive_event(unsigned int send_id)
{
	lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR1_TRIGGER, 0, 0);
	return true;
}
static bool intercom_alarm_sensor_2_again_receive_event(unsigned int send_id)
{
	lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR2_TRIGGER, 0, 0);
	return true;
}
static bool intercom_alarm_ring_stop_receive_event(unsigned int send_id)
{
	ringplay_play_stop();
	return true;
}
static bool intercom_monitor_fast_mode_receive_event(unsigned int send_id)
{
	if (OwnID == 1)
	{
		unsigned int mask = (0x01 << (send_id - 1));
		layout_intercom_call_mask_set(mask);
	}
	monitor_enter_mask_set(MON_ENTER_TALK);
	intercom_data_busy_enbale(true);
	if (cur_layout_get() == pLAYOUT(monitor))
	{
		goto_layout(pLAYOUT(monitor));
	}
	return true;
}
static bool intercom_alarm_status_get_receive_event(unsigned int send_id)
{
	if (OwnID != 1)
	{
		return false;
	}
	if (user_data_get()->alarm.alarm_1_trigger == true)
	{
		intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_SENSOR1);
	}
	if (user_data_get()->alarm.alarm_2_trigger == true)
	{
		intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_SENSOR2);
	}
	return true;
}


static bool intercom_monitor_door2_lock_open_2_receive_event(unsigned int send_id)
{
	MON_CH ch = monitor_channel_get();
	if(ch == MON_CH_DOOR1)
	{
		monitor_unlock_open(send_id,MON_CH_DOOR1);
#ifdef MAOTOUYING_DAN_IC
		ringplay_play_form_index(7, 100, door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
#endif
	}
	if(ch == MON_CH_DOOR2)
	{
		monitor_unlock_open(send_id,MON_CH_DOOR2);
#ifdef MAOTOUYING_DAN_IC
		ringplay_play_form_index(7, 100, door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
#endif
	}

	if(cur_layout_get() == pLAYOUT(monitor)){

		monitor_unlock_mask_set(MON_UNLOCK_TALK);
	}
	return true;
}
static bool intercom_montior_busy_ack_receive_event(unsigned int send_id)
{
	const layout *p = cur_layout_get();
	if ((p == pLAYOUT(intercom)) || (p == pLAYOUT(intercom_out)) || (p == pLAYOUT(intercom_in)) || (p == pLAYOUT(intercom_talk)) || (p == pLAYOUT(monitor)) || (p == pLAYOUT(tuya_monitor))|| (p == pLAYOUT(adj_display)))
	{
		Interphone_delay();
		intercom_cmd_send(send_id, CMD_DATA_BUSY_NORMAL);
	}
	else
	{
		Interphone_delay();
		intercom_cmd_send(send_id, CMD_DATA_BUSY_UNACK);
	}
	// video_switch_enable(MON_CH_INTERCOM, 0x01);
	return true;
}
static bool intercom_montior_busy_unack_receive_event(unsigned int send_id)
{
	if (send_id == OwnID)
	{
		//tuya_api_ringbuffer_clear();
		h264_encode_sample_enable(true);
		//usleep(1000*1000);
		tuya_pvew_video_count_fource_set(0);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_STOP, 0);

		intercom_busy_ack_mask_set(0x00);
		LOG_GREEN("set release 9\n");
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
		intercom_data_release_receive_event(send_id);
		return true;
	}
	unsigned int mask = intercom_busy_ack_mask_get();
	mask |= 0x01 << (send_id);
	intercom_busy_ack_mask_set(mask);
	unsigned int loc_mask = (0x0F & (~(0x01 << (OwnID - 1))));
	if (loc_mask == mask)
	{
		intercom_busy_ack_mask_set(0x00);
		LOG_GREEN("set release 10\n");
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
		intercom_data_release_receive_event(send_id);
	}
	return true;
}
static bool intercom_montior_busy_normal_receive_event(unsigned int send_id)
{
	intercom_busy_ack_mask_set(0x00);
	intercom_busy_timer_del();
	return true;
}

static bool intercom_busy_status_check(void)
{
	if (intercom_data_busy_get() == false)
	{
		return false;
	}
	// if (monitor_enter_mask_get() == MON_ENTER_CALL)
	// {
	// 	return false;
	// }
	const layout *p = cur_layout_get();
	if ((p == pLAYOUT(intercom)) || (p == pLAYOUT(intercom_out)) || (p == pLAYOUT(intercom_in)) || (p == pLAYOUT(intercom_talk)) || (p == pLAYOUT(monitor)))
	{
		return false;
	}

	if ((tuya_online_num_get_customize() > 0) && (tuya_api_app_talk_check()))
	{
		return false;
	}
	return true;
}
static bool intercom_query_status_receive_event(unsigned int send_id)
{
	if(OwnID == 1){
		
		if (intercom_busy_status_check() == true)
		{
			if (intercom_busy_timer_ptask == NULL)
			{
				intercom_busy_timer_start(120000);
			}
		}
		else if (monitor_enter_mask_get() != MON_ENTER_CALL)
		{
			intercom_busy_ack_mask_set(0x00);
			intercom_busy_timer_del();
		}
	}

	return true;
}
static const intercom_param_info intercom_receive_handle[] =
    {
	{"CMD_INTERPHONE_CALL", CMD_INTERPHONE_CALL, intercom_intercom_call_receive_event},
	{"CMD_INTERPHONE_JOIN", CMD_INTERPHONE_JOIN, intercom_intercom_join_receive_event},
	{"CMD_INTERPHONE_ANSWER", CMD_INTERPHONE_ANSWER, intercom_intercom_answer_receive_event},
	{"CMD_DATA_BUSY", CMD_DATA_BUSY, intercom_data_busy_receive_event},
	{"CMD_DATA_RELEASE", CMD_DATA_RELEASE, intercom_data_release_receive_event},
	{"CMD_INTERPHONE_QUIT", CMD_INTERPHONE_QUIT, intercom_intercom_quit_receive_event},
	{"CMD_DOOR_ENTER", CMD_DOOR_ENTER, intercom_monitor_door_enter_receive_event},
	{"CMD_MONITOR_DOOR1", CMD_MONITOR_DOOR1, intercom_monitor_enter_door1_receive_event},
	{"CMD_MONITOR_DOOR2", CMD_MONITOR_DOOR2, intercom_monitor_enter_door2_receive_event},
	{"CMD_SWITCH_DOOR1", CMD_SWITCH_DOOR1, intercom_monitor_switch_door1_receive_event},
	{"CMD_SWITCH_DOOR2", CMD_SWITCH_DOOR2, intercom_monitor_switch_door2_receive_event},
	{"CMD_SWITCH_CCTV1", CMD_SWITCH_CCTV1, intercom_monitor_switch_cctv1_receive_event},
	{"CMD_SWITCH_CCTV2", CMD_SWITCH_CCTV2, intercom_monitor_switch_cctv2_receive_event},
	{"CMD_CCTV_ENTER", CMD_CCTV_ENTER, intercom_monitor_cctv_enter_receive_event},
	{"CMD_MONITOR_CCTV1", CMD_MONITOR_CCTV1, intercom_monitor_enter_cctv1_receive_event},
	{"CMD_MONITOR_CCTV2", CMD_MONITOR_CCTV2, intercom_monitor_enter_cctv2_receive_event},
	{"CMD_DOORTOCCTV1", CMD_DOORTOCCTV1, intercom_monitor_door_to_cctv1_receive_event},
	{"CMD_DOORTOCCTV2", CMD_DOORTOCCTV2, intercom_monitor_door_to_cctv2_receive_event},
	{"CMD_CCTVTODOOR1", CMD_CCTVTODOOR1, intercom_monitor_cctv_to_door1_receive_event},
	{"CMD_CCTVTODOOR2", CMD_CCTVTODOOR2, intercom_monitor_cctv_to_door2_receive_event},
	{"CMD_CCTV1_SIGNAL_TRUE", CMD_CCTV1_SIGNAL_TRUE, intercom_monitor_cctv1_signal_true_receive_event},
	{"CMD_CCTV1_SIGNAL_FALSE", CMD_CCTV1_SIGNAL_FALSE, intercom_monitor_cctv1_signal_false_receive_event},
	{"CMD_CCTV2_SIGNAL_TRUE", CMD_CCTV2_SIGNAL_TRUE, intercom_monitor_cctv2_signal_true_receive_event},
	{"CMD_CCTV2_SIGNAL_FALSE", CMD_CCTV2_SIGNAL_FALSE, intercom_monitor_cctv2_signal_false_receive_event},
	{"CMD_DOOR1_CALL", CMD_DOOR1_CALL, intercom_monitor_door1_call_recevice_event},
	{"CMD_DOOR2_CALL", CMD_DOOR2_CALL, intercom_monitor_door2_call_recevice_event},
	{"CMD_DOOR_CALL_JOIN", CMD_DOOR_CALL_JOIN, intercom_monitor_call_join_receive_event},
	{"CMD_DOOR_CALL_QUIT", CMD_DOOR_CALL_QUIT, intercom_monitor_call_quit_receive_event},
	{"CMD_INDOOR1_CALL", CMD_INDOOR1_CALL, intercom_monitor_door1_in_call_recevice_event},
	{"CMD_INDOOR2_CALL", CMD_INDOOR2_CALL, intercom_monitor_door2_in_call_recevice_event},
	{"CMD_INDOOR_CALL_ANSWER", CMD_INDOOR_CALL_ANSWER, intercom_monitor_call_answer_recevice_event},
	{"CMD_DOOR1_AUDIO", CMD_DOOR1_AUDIO, intercom_monitor_call_door1_audio_recevice_event},
	{"CMD_DOOR2_AUDIO", CMD_DOOR2_AUDIO, intercom_monitor_call_door2_audio_recevice_event},
	{"CMD_DOOR2_UNLOCK_OPEN", CMD_DOOR2_UNLOCK_OPEN, intercom_monitor_call_door2_unlock_recevice_event},
	{"CMD_DOOR_UNLOCK_CLOSE", CMD_DOOR_UNLOCK_CLOSE, intercom_monitor_door_unlock_close_recevice_event},

	{"CMD_DATA_REBOOT", CMD_DATA_REBOOT, intercom_data_reboot_receive_event},
	{"CMD_ALARM_SENSOR1", CMD_ALARM_SENSOR1, intercom_alarm_sensor_1_receive_event},
	{"CMD_ALARM_SENSOR2", CMD_ALARM_SENSOR2, intercom_alarm_sensor_2_receive_event},
	{"CMD_ALARM_QUIT", CMD_ALARM_QUIT, intercom_alarm_sensor_quit_receive_event},
	{"CAM_ALARM_STOP", CAM_ALARM_STOP, intercom_alarm_sensor_stop_receive_event},
	{"CMD_ALARM_SENSOR1_AGAIN", CMD_ALARM_SENSOR1_AGAIN, intercom_alarm_sensor_1_again_receive_event},
	{"CMD_ALARM_SENSOR1_AGAIN", CMD_ALARM_SENSOR2_AGAIN, intercom_alarm_sensor_2_again_receive_event},
	{"CMD_ALARM_RING_STOP", CMD_ALARM_RING_STOP, intercom_alarm_ring_stop_receive_event},
	{"CMD_DOOR_FASTMODE", CMD_DOOR_FASTMODE, intercom_monitor_fast_mode_receive_event},
	{"CMD_ALARM_GET_STATUS", CMD_ALARM_GET_STATUS, intercom_alarm_status_get_receive_event},
	{"CMD_DOOR2_UNLOCK_OPEN_2", CMD_DOOR2_UNLOCK_OPEN_2, intercom_monitor_door2_lock_open_2_receive_event},

	{"CMD_DATA_BUSY_ACK", CMD_DATA_BUSY_ACK, intercom_montior_busy_ack_receive_event},
	{"CMD_DATA_BUSY_UNACK", CMD_DATA_BUSY_UNACK, intercom_montior_busy_unack_receive_event},
	{"CMD_DATA_BUSY_NORMAL", CMD_DATA_BUSY_NORMAL, intercom_montior_busy_normal_receive_event},

	{"CMD_INTERPHONE_QUERY", CMD_INTERPHONE_QUERY, intercom_query_status_receive_event},
};
/***
**   日期:2022-05-31 08:47:35
**   作者: leo.liu
**   函数作用：接收intercom，主线程处理任务
**   参数说明:
***/
static bool intercom_event_cmd_process(unsigned int send_id, unsigned int cmd)
{
	int total = sizeof(intercom_receive_handle) / sizeof(intercom_param_info);
	for (int i = 0; i < total; i++)
	{
		if ((cmd == intercom_receive_handle[i].cmd) && (intercom_receive_handle[i].handler != NULL))
		{
			if (cmd != CMD_INTERPHONE_QUERY)
			{
				LOG_BLUE("\r Receive Event : ( id%d)  send  CMD -%s  to  ( id%d)\n\r", send_id, intercom_receive_handle[i].str, OwnID);
			}
			return intercom_receive_handle[i].handler(send_id);
		}
	}
	printf("not find cmd event (%02x ->%d)\n\r", cmd, send_id);
	return false;
}
/************************************************************************** 发送处理范围 **************************************************************************/
static unsigned char intercom_send_default_cmd = 0x00;
static void intercom_standard_format_process(char buf[6], unsigned int receive_id, unsigned char cmd)
{
	memset(buf, 0, 6);
	buf[0] = CODE_START;
	buf[1] = OwnID;
	buf[2] = receive_id;
	buf[3] = cmd;
	buf[4] = (OwnID + receive_id + cmd) & 0xff;
	buf[5] = CODE_STOP;
}
/***
**   日期:2022-05-31 11:34:04
**   作者: leo.liu
**   函数作用：默认发送intercom事件函数
**   参数说明:
***/
static bool intercom_normal_send_default_event(unsigned int receive_id)
{
	char buf[6];
	intercom_standard_format_process(buf, receive_id, intercom_send_default_cmd);
	uart_write(intercom_uart_fd, buf, 6);
	return true;
}
static bool intercom_intercom_call_send_event(unsigned int receive_id)
{
	char buf[6];
	for (int i = 0; i < 4; i++)
	{
		unsigned char id = (0x01 << i) & receive_id;
		if (id)
		{
			intercom_standard_format_process(buf, i + 1, CMD_INTERPHONE_CALL);
			uart_write(intercom_uart_fd, buf, 6);
		}
	}
	return true;
}
static bool intercom_data_release_send_event(unsigned int receive_id)
{
	char buf[6];
	intercom_data_busy_enbale(false);
	Interphone_delay();
	intercom_standard_format_process(buf, receive_id, CMD_DATA_RELEASE);
	uart_write(intercom_uart_fd, buf, 6);
	intercom_data_release_receive_event(receive_id);
	return true;
}
static bool intercom_monitor_door_call_send_event(unsigned int receive_id)
{
	char buf[6];
	intercom_standard_format_process(buf, CODE_ALL_ID, monitor_valid_channel_check(MON_CH_CCTV1) == true ? CMD_CCTV1_SIGNAL_TRUE : CMD_CCTV1_SIGNAL_FALSE);
	uart_write(intercom_uart_fd, buf, 6);
	intercom_standard_format_process(buf, CODE_ALL_ID, monitor_valid_channel_check(MON_CH_CCTV2) == true ? CMD_CCTV2_SIGNAL_TRUE : CMD_CCTV2_SIGNAL_FALSE);
	uart_write(intercom_uart_fd, buf, 6);
	uart_write(intercom_uart_fd, buf, 6);

	intercom_standard_format_process(buf, receive_id, intercom_send_default_cmd);
	uart_write(intercom_uart_fd, buf, 6);
	return true;
}
static bool intercom_door_call_quit_event(unsigned int receive_id)
{
	char buf[6];
	Interphone_delay();
	intercom_standard_format_process(buf, receive_id, CMD_DOOR_CALL_QUIT);
	uart_write(intercom_uart_fd, buf, 6);
	return true;
}
static bool intercom_data_busy_send_event(unsigned int receive_id)
{
	char buf[6];
	intercom_busy_timer_start(500);//防止设备退出时，没有收到应答
	intercom_standard_format_process(buf, receive_id, CMD_DATA_BUSY_ACK);
	uart_write(intercom_uart_fd, buf, 6);
	return true;
}

static USER_INTERCOM_TM intercom_user_time;
static bool intercom_normal_send_time_set_event(unsigned int receive_id)
{
	char buf[6];
	intercom_standard_format_process(buf, receive_id, CMD_DATA_TIMESET);
	uart_write(intercom_uart_fd, buf, 6);
	uart_write(intercom_uart_fd, (char *)&intercom_user_time, sizeof(intercom_user_time));
	return true;
}


static bool intercom_data_busy_unack_send_event(unsigned int receive_id)
{

	char buf[6];
	printf("=====%s=====\n",__func__);
	// intercom_busy_timer_start(1500);
	intercom_standard_format_process(buf, receive_id, CMD_DATA_BUSY_UNACK);
	uart_write(intercom_uart_fd, buf, 6);


	// intercom_montior_busy_unack_receive_event(receive_id);


	return true;
}

static USER_INTERCOM_LOCKTIME intercom_user_locktime;
static bool intercom_normal_send_lock_time_event(unsigned int receive_id)
{
	char buf[6];
	intercom_standard_format_process(buf, receive_id, CMD_LOCK_TIME_SET);
	uart_write(intercom_uart_fd, buf, 6);
	uart_write(intercom_uart_fd, (char *)&intercom_user_locktime, sizeof(intercom_user_locktime));
	return true;
}

static const intercom_param_info intercom_send_handle[] = {
    {"CMD_INTERPHONE_CALL", CMD_INTERPHONE_CALL, intercom_intercom_call_send_event},
    {"CMD_INTERPHONE_JOIN", CMD_INTERPHONE_JOIN, intercom_normal_send_default_event},
    {"CMD_INTERPHONE_ANSWER", CMD_INTERPHONE_ANSWER, intercom_normal_send_default_event},
    {"CMD_DATA_BUSY", CMD_DATA_BUSY, intercom_normal_send_default_event},
    {"CMD_DATA_RELEASE", CMD_DATA_RELEASE, intercom_data_release_send_event},
    {"CMD_INTERPHONE_QUIT", CMD_INTERPHONE_QUIT, intercom_normal_send_default_event},
    {"CMD_DOOR_ENTER", CMD_DOOR_ENTER, intercom_normal_send_default_event},
    {"CMD_MONITOR_DOOR1", CMD_MONITOR_DOOR1, intercom_normal_send_default_event},
    {"CMD_MONITOR_DOOR2", CMD_MONITOR_DOOR2, intercom_normal_send_default_event},
    {"CMD_SWITCH_DOOR1", CMD_SWITCH_DOOR1, intercom_normal_send_default_event},
    {"CMD_SWITCH_DOOR2", CMD_SWITCH_DOOR2, intercom_normal_send_default_event},
    {"CMD_SWITCH_CCTV1", CMD_SWITCH_CCTV1, intercom_normal_send_default_event},
    {"CMD_SWITCH_CCTV2", CMD_SWITCH_CCTV2, intercom_normal_send_default_event},
    {"CMD_CCTV_ENTER", CMD_CCTV_ENTER, intercom_normal_send_default_event},
    {"CMD_MONITOR_CCTV1", CMD_MONITOR_CCTV1, intercom_normal_send_default_event},
    {"CMD_MONITOR_CCTV2", CMD_MONITOR_CCTV2, intercom_normal_send_default_event},
    {"CMD_DOORTOCCTV1", CMD_DOORTOCCTV1, intercom_normal_send_default_event},
    {"CMD_DOORTOCCTV2", CMD_DOORTOCCTV2, intercom_normal_send_default_event},
    {"CMD_CCTVTODOOR1", CMD_CCTVTODOOR1, intercom_normal_send_default_event},
    {"CMD_CCTVTODOOR2", CMD_CCTVTODOOR2, intercom_normal_send_default_event},
    {"CMD_CCTV1_SIGNAL_TRUE", CMD_CCTV1_SIGNAL_TRUE, intercom_normal_send_default_event},
    {"CMD_CCTV1_SIGNAL_FALSE", CMD_CCTV1_SIGNAL_FALSE, intercom_normal_send_default_event},
    {"CMD_CCTV2_SIGNAL_TRUE", CMD_CCTV2_SIGNAL_TRUE, intercom_normal_send_default_event},
    {"CMD_CCTV2_SIGNAL_FALSE", CMD_CCTV2_SIGNAL_FALSE, intercom_normal_send_default_event},
    {"CMD_DOOR1_CALL", CMD_DOOR1_CALL, intercom_monitor_door_call_send_event},
    {"CMD_DOOR2_CALL", CMD_DOOR2_CALL, intercom_monitor_door_call_send_event},
    {"CMD_DOOR_CALL_JOIN", CMD_DOOR_CALL_JOIN, intercom_normal_send_default_event},
    {"CMD_DOOR_CALL_QUIT", CMD_DOOR_CALL_QUIT, intercom_door_call_quit_event},
    {"CMD_INDOOR1_CALL", CMD_INDOOR1_CALL, intercom_normal_send_default_event},
    {"CMD_INDOOR2_CALL", CMD_INDOOR2_CALL, intercom_normal_send_default_event},
    {"CMD_INDOOR_CALL_ANSWER", CMD_INDOOR_CALL_ANSWER, intercom_normal_send_default_event},
    {"CMD_DOOR1_AUDIO", CMD_DOOR1_AUDIO, intercom_normal_send_default_event},
    {"CMD_DOOR2_AUDIO", CMD_DOOR2_AUDIO, intercom_normal_send_default_event},
    {"CMD_DOOR1_UNLOCK_OPEN", CMD_DOOR1_UNLOCK_OPEN, intercom_normal_send_default_event},
    {"CMD_DOOR2_UNLOCK_OPEN", CMD_DOOR2_UNLOCK_OPEN, intercom_normal_send_default_event},
    {"CMD_DOOR_UNLOCK_CLOSE", CMD_DOOR_UNLOCK_CLOSE, intercom_normal_send_default_event},

    {"CMD_DATA_REBOOT", CMD_DATA_REBOOT, intercom_normal_send_default_event},
    {"CMD_ALARM_SENSOR1", CMD_ALARM_SENSOR1, intercom_normal_send_default_event},
    {"CMD_ALARM_SENSOR2", CMD_ALARM_SENSOR2, intercom_normal_send_default_event},
    {"CMD_ALARM_QUIT", CMD_ALARM_QUIT, intercom_normal_send_default_event},
    {"CAM_ALARM_STOP", CAM_ALARM_STOP, intercom_normal_send_default_event},
    {"CMD_ALARM_SENSOR1_AGAIN", CMD_ALARM_SENSOR1_AGAIN, intercom_normal_send_default_event},
    {"CMD_ALARM_SENSOR1_AGAIN", CMD_ALARM_SENSOR2_AGAIN, intercom_normal_send_default_event},
    {"CMD_ALARM_RING_STOP", CMD_ALARM_RING_STOP, intercom_normal_send_default_event},
    {"CMD_DOOR_FASTMODE", CMD_DOOR_FASTMODE, intercom_normal_send_default_event},
    {"CMD_ALARM_GET_STATUS", CMD_ALARM_GET_STATUS, intercom_normal_send_default_event},
    {"CMD_DOOR2_UNLOCK_NUM_1", CMD_DOOR2_UNLOCK_NUM_1, intercom_normal_send_default_event},
    {"CMD_DOOR2_UNLOCK_OPEN_2", CMD_DOOR2_UNLOCK_OPEN_2, intercom_normal_send_default_event},

    {"CMD_DATA_BUSY_ACK", CMD_DATA_BUSY_ACK, intercom_data_busy_send_event},
	{"CMD_DATA_BUSY_UNACK", CMD_DATA_BUSY_UNACK, intercom_data_busy_unack_send_event},
    // {"CMD_DATA_BUSY_UNACK", CMD_DATA_BUSY_UNACK, intercom_normal_send_default_event},
    {"CMD_DATA_BUSY_NORMAL", CMD_DATA_BUSY_NORMAL, intercom_normal_send_default_event},

    {"CMD_DATA_TIMESET", CMD_DATA_TIMESET, intercom_normal_send_time_set_event},
	{"CMD_LOCK_TIME_SET", CMD_LOCK_TIME_SET, intercom_normal_send_lock_time_event},
};

/***
**   日期:2022-05-31 09:18:54
**   作者: leo.liu
**   函数作用：发送串口命令
**   参数说明:
***/
bool intercom_cmd_send(unsigned int receive_id, unsigned char cmd)
{
	int total = sizeof(intercom_send_handle) / sizeof(intercom_param_info);
	for (int i = 0; i < total; i++)
	{
		if ((cmd == intercom_send_handle[i].cmd) && (intercom_send_handle[i].handler != NULL))
		{
			LOG_BLUE("\r send Event : ( id%d)  send  CMD -%s  to  ( id%d)\n\r", OwnID, intercom_send_handle[i].str, receive_id);
			intercom_send_default_cmd = cmd;
			return intercom_send_handle[i].handler(receive_id);
		}
	}
	printf("intercom send:not find cmd event (%02x ->%d)\n\r", cmd, receive_id);
	return false;
}
/***
**   日期:2022-06-01 09:57:30
**   作者: leo.liu
**   函数作用：发送cctv1的信号状态
**   参数说明:
***/
bool intercom_cctv_signale_state_send(unsigned int receive_id, MON_CH ch)
{
	if (OwnID != 1)
	{
		return false;
	}
	if (ch == MON_CH_CCTV1)
	{
		intercom_cmd_send(receive_id, monitor_valid_channel_check(MON_CH_CCTV1) == true ? CMD_CCTV1_SIGNAL_TRUE : CMD_CCTV1_SIGNAL_FALSE);
	}
	else if (ch == MON_CH_CCTV2)
	{
		intercom_cmd_send(receive_id, monitor_valid_channel_check(MON_CH_CCTV2) == true ? CMD_CCTV2_SIGNAL_TRUE : CMD_CCTV2_SIGNAL_FALSE);
	}
	return true;
}
/***
**   日期:2022-06-18 09:36:49
**   作者: leo.liu
**   函数作用：intercom时间同步
**   参数说明:
***/
bool intercom_user_time_sync_send(unsigned int receive_id, struct tm *tm)
{
	intercom_user_time.tm_year = tm->tm_year;
	intercom_user_time.tm_mon = tm->tm_mon;
	intercom_user_time.tm_mday = tm->tm_mday;
	intercom_user_time.tm_hour = tm->tm_hour;
	intercom_user_time.tm_min = tm->tm_min;
	intercom_user_time.tm_sec = tm->tm_sec;
	intercom_cmd_send(receive_id, CMD_DATA_TIMESET);
	return true;
}
/***
**   日期:2022-07-05 09:52:30
**   作者: leo.liu
**   函数作用：发送door2开锁数目
**   参数说明:
***/
bool intercom_door2_unlock_num_send(unsigned int receive_id, int num)
{
	if (OwnID != 1)
	{
		return false;
	}

	intercom_cmd_send(receive_id, CMD_DOOR2_UNLOCK_NUM_1);
	
	return true;
}


bool intercom_user_locktime_sync_send(unsigned int receive_id, unsigned char lock1_time,unsigned char lock2_time)
{
	intercom_user_locktime.lock1_time = lock1_time;
	intercom_user_locktime.lock2_time = lock2_time;
	intercom_cmd_send(receive_id, CMD_LOCK_TIME_SET);
	return true;
}