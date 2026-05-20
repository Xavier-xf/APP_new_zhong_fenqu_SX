#ifndef _LAYOUT_COMMON_H_
#define _LAYOUT_COMMON_H_

/***
** 日期: 2022-04-28 09:53
** 作者: leo.liu
** 函数作用：控件被按下执行的回调函数
** 返回参数说明：
***/
void layout_obj_click_down_func(lv_obj_t *obj);
/***
** 日期: 2022-04-26 11:06
** 作者: leo.liu
** 函数作用：创建显示时间的文本
** 返回参数说明：
***/
bool home_time_text_create(lv_obj_t *parent, int x, int y, int w, int h, int hour_id, int min_id, int dot_id);
/***
** 日期: 2022-05-10 18:00
** 作者: leo.liu
** 函数作用：时间显示
** 返回参数说明：
***/
void home_time_refresh_display(lv_obj_t *parent, struct tm *tm, int hour_id, int min_id, int dot_id, bool blink);
/***
** 日期: 2022-04-26 11:59
** 作者: leo.liu
** 函数作用：创建显示日期的文本控件
** 返回参数说明：
***/
bool home_date_text_create(lv_obj_t *parent, int x, int y, int w, int h, int obj_id);
/***
** 日期: 2022-05-11 08:07
** 作者: leo.liu
** 函数作用：日期显示
** 返回参数说明：
***/
void home_date_refresh_display(lv_obj_t *parent, struct tm *tm, int date_id);
/***
** 日期: 2022-05-12 10:27
** 作者: leo.liu
** 函数作用：door1 call 默认处理函数
** 返回参数说明：
***/
void layout_door1_call_default(void);
/***
** 日期: 2022-05-12 10:27
** 作者: leo.liu
** 函数作用：door2 call 默认处理函数
** 返回参数说明：
***/
void layout_door2_call_default(void);
/***
**   日期:2022-07-11 14:12:55
**   作者: leo.liu
**   函数作用：intercom呼叫处理
**   参数说明:
***/
bool layout_intercom_trigger_default(unsigned int arg1, unsigned int arg2);
/***
**   日期:2022-05-28 08:11:18
**   作者: leo.liu
**   函数作用：警报触发默认处理
**   参数说明:
***/
void layout_alarm1_trigger_default(void);
/***
**   日期:2022-05-28 08:11:18
**   作者: leo.liu
**   函数作用：警报触发默认处理
**   参数说明:
***/
void layout_alarm2_trigger_default(void);
/***
**   日期:2022-06-07 15:41:21
**   作者: leo.liu
**   函数作用：涂鸦事件处理
**   参数说明:
***/
bool layout_tuya_event_default(TUYA_CMD cmd, int arg);

/***
**   日期:2023-01-10 15:43:30
**   作者: leo.liu
**   函数作用：获取涂鸦设备在线数量(自定义)
**   参数说明:
***/
int tuya_online_num_get_customize();

typedef enum
{
	MON_ENTER_NONE,
	MON_ENTER_MANUAL_DOOR,
	MON_ENTER_MANUAL_CCTV,
	MON_ENTER_CALL,
	MON_ENTER_TALK,
	MON_ENTER_DISPLAY,
} MON_ENTER_FLG;
typedef enum
{
	MON_UNLOCK_NONE,
	MON_UNLOCK_CALL,
	MON_UNLOCK_TALK,
} MON_UNLOCK_FLG;
/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：设置进入监控的标志位
** 返回参数说明：
***/
void monitor_enter_mask_set(MON_ENTER_FLG flg);
/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：设置进入监控开锁的标志位
** 返回参数说明：
***/
void monitor_unlock_mask_set(MON_UNLOCK_FLG flg);
/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：获取进入监控的标志位
** 返回参数说明：
***/
MON_ENTER_FLG monitor_enter_mask_get(void);
/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：获取进入监控开锁的标志位
** 返回参数说明：
***/
MON_UNLOCK_FLG monitor_unlock_mask_get(void);
/***
** 日期: 2022-05-17 08:13
** 作者: leo.liu
** 函数作用：获取当前通道的亮度值
** 返回参数说明：
***/
int monitor_display_brightness_vol_get(void);
/***
** 日期: 2022-05-17 08:15
** 作者: leo.liu
** 函数作用：设置亮度值
** 返回参数说明：
***/
void monitor_display_brightness_vol_set(int vol);
/***
** 日期: 2022-05-17 08:13
** 作者: leo.liu
** 函数作用：获取当前通道的亮度值
** 返回参数说明：
***/
int monitor_display_cont_vol_get(void);
/***
** 日期: 2022-05-17 08:15
** 作者: leo.liu
** 函数作用：设置亮度值
** 返回参数说明：
***/
void monitor_display_cont_vol_set(int vol);
/***
** 日期: 2022-05-17 08:13
** 作者: leo.liu
** 函数作用：获取当前通道的色度值
** 返回参数说明：
***/
int monitor_display_color_vol_get(void);
/***
** 日期: 2022-05-17 08:15
** 作者: leo.liu
** 函数作用：设置亮度值
** 返回参数说明：
***/
void monitor_display_color_vol_set(int vol);
/***
** 日期: 2022-05-20 17:30
** 作者: leo.liu
** 函数作用：铃声播放其实回调函数
** 返回参数说明：
***/
void ringplay_keysound_start_default_func(int index);
/***
** 日期: 2022-05-20 17:30
** 作者: leo.liu
** 函数作用：铃声结束的默认处理
** 返回参数说明：
***/
void ringplay_keysound_finish_default_func(int index);
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声开始
** 返回参数说明：
***/
void ringplay_doorcall_start_default_func(int index);
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声结束
** 返回参数说明：
***/
void ringplay_doorcall_finish_default_func(int index);
/***
**   日期:2022-05-23 11:57:10
**   作者: leo.liu
**   函数作用：缩略图参数初始化
**   参数说明:
***/
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：intercom铃声开始
** 返回参数说明：
***/
void ringplay_intercom_start_default_func(int index);
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：inter铃声结束
** 返回参数说明：
***/
void ringplay_intercom_finish_default_func(int index);
/***
**   日期:2022-07-14 16:46:04
**   作者: leo.liu
**   函数作用：开室内机锁铃声播放前的回调处理
**   参数说明:
***/
void gate_open_ringplay_start_default_func(int index);
/***
**   日期:2022-07-14 16:46:47
**   作者: leo.liu
**   函数作用：开室内机锁铃声结束的回调处理函数
**   参数说明:
***/
void gate_open_ringplay_finish_default_func(int index);
/***
**   日期:2022-07-14 16:46:04
**   作者: leo.liu
**   函数作用：开门口机锁铃声播放前的回调处理
**   参数说明:
***/
void door_open_ringplay_start_default_func(int index);
/***
**   日期:2022-07-14 16:46:47
**   作者: leo.liu
**   函数作用：开门口机锁铃声结束的回调处理函数
**   参数说明:
***/
void door_open_ringplay_finish_default_func(int index);
/***
**   日期:2022-07-04 09:57:42
**   作者: leo.liu
**   函数作用：进入预览界面参数设置
**   参数说明:
***/
void layout_playback_parm_init(void);
/***
**   日期:2022-06-07 18:35:56
**   作者: leo.liu
**   函数作用：上传有效通道
**   参数说明:
***/
bool layout_monitor_report_vaild_channel(void);
/***
**   日期:2022-06-18 10:36:39
**   作者: leo.liu
**   函数作用：tuya 预览
**   参数说明:
***/
bool tuya_api_app_preview_check(void);

int home_mode_btn_id_get(void);
void home_mode_btn_display(lv_obj_t *obj);

void message_window_pops(const char *str,int x,int y);
void message_window_home_pops(const char *str);
/***
**   日期:2022-06-09 17:52:36
**   作者: leo.liu
**   函数作用：涂鸦监控中
**   参数说明:
***/
bool tuya_app_preview_check(void);


/***
** 日期: 2023-08-18 10:27
** 作者: fa.liu
** 函数作用：按键开锁 默认处理函数
** 返回参数说明：
***/
void layout_unclock_default(void);

bool indoor_talk_state_get(void);

void tuya_app_preview_enable(bool en);
void tuya_pvew_video_count_fource_set(int cout);
int tuya_pvew_video_count_fource_get(void);

lv_obj_t *xl_obj_find_by_id_in_obj(lv_obj_t *obj, int obj_id);
lv_obj_t *xl_obj_find_by_id(int obj_id);
#endif
