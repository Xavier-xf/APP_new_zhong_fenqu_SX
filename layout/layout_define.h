#ifndef _LAYOUT_DEFINE_H_
#define _LAYOUT_DEFINE_H_
//德国 cofuuda4jlgzyfat
//安娜 sag3lsgv5iezqexg
//后面中性pid  26mo7s6rl5mob1du 
// ME-AHD7P4C-ID   gcuxnqqs1dfnwa8v
// ME-AHD7T4J-S "9vbnhhruihxkw3wg" 
// 7.0 "wrw4vog3vosbq4yd" 
// 10.1  "iet0idk0yw3fgren"
#define TUYA_PID_PE ("cofuuda4jlgzyfat")
#define TUYA_PID_ANNA ("sag3lsgv5iezqexg")
#define TUYA_PID_ZHONG ("26mo7s6rl5mob1du")
#define TUYA_PID_ZHONG_AL ("j4ofpikawr05gnpi")
#define TUYA_PID_ZHONG_OR ("e4lp2xxjfrhdplpx")
#define TUYA_PID_ZHONG_LYD ("sqcjkss6hskjzs7f")
#define TUYA_PID_ZHONG_DST ("ionnuvpeccqvn9pg")
#define TUYA_PID_ZHONG_SX ("5d5kpetj4d5broip")


//5d5kpetj4d5broip

#define TUYA_PID TUYA_PID_ZHONG_SX

// #define MAOTOUYING_DAN_IC

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "lvgl/lv_img_decoder.h"
#include "rom.h"
#include "lv_msg_event.h"
#include "language.h"
#include "user_data.h"
#include "user_common.h"
#include "video_decode.h"
#include "user_standby.h"
#include "user_monitor.h"
#include "user_time.h"
#include "user_file.h"
#include "audio_input.h"
#include "layout_common.h"
#include "user_gpio.h"
#include "layout_setting_common.h"
#include "video_decode.h"
#include "user_monitor.h"
#include "tp9950.h"
#include "ringplay.h"
#include "tuya/tuya_api.h"
#include "video_encode.h"
#include "user_record.h"
#include "video_record.h"
#include "video_input.h"
#include "user_gpio.h"
#include "video_play.h"
#include "user_intercom.h"
#include "user_alarm_list.h"
#include "info.h"
#include "layout_setting_obj_template.h"
#include "sat_ipcamera.h"
#include "user_onvif.h"

#define SYSTEM_VERSION "V5.0.27new"//"V0.0.3ME"//"V5.0.18new"


DEFINE_LAYOUT(logo);
DEFINE_LAYOUT(home);

DEFINE_LAYOUT(setting_record);
DEFINE_LAYOUT(setting_volume);
DEFINE_LAYOUT(setting_storage);
DEFINE_LAYOUT(setting_date_time);
DEFINE_LAYOUT(setting_display);
DEFINE_LAYOUT(setting_etc);
DEFINE_LAYOUT(setting_motion);
DEFINE_LAYOUT(setting_motion_timer);
DEFINE_LAYOUT(setting_volume_vol);
DEFINE_LAYOUT(setting_frame);
DEFINE_LAYOUT(setting_frame_time);
DEFINE_LAYOUT(setting_night_mode);
DEFINE_LAYOUT(setting_frame_list);
DEFINE_LAYOUT(setting_background);
DEFINE_LAYOUT(clear_screen);
DEFINE_LAYOUT(setting_language);
// DEFINE_LAYOUT(setting_id);
DEFINE_LAYOUT(setting_password);
DEFINE_LAYOUT(calendar);

void setting_time_enter_flag_set(char);
DEFINE_LAYOUT(setting_time);

DEFINE_LAYOUT(close);
DEFINE_LAYOUT(frame_show);

bool layout_monitor_talk_state(void);
DEFINE_LAYOUT(monitor);
DEFINE_LAYOUT(playback);
DEFINE_LAYOUT(video);
DEFINE_LAYOUT(photo);
DEFINE_LAYOUT(security);
DEFINE_LAYOUT(setting_id);
DEFINE_LAYOUT(intercom_in);
DEFINE_LAYOUT(intercom_out);
DEFINE_LAYOUT(intercom_talk);
DEFINE_LAYOUT(intercom_ring);
DEFINE_LAYOUT(tuya_monitor);
DEFINE_LAYOUT(tuya_info);
DEFINE_LAYOUT(click);
DEFINE_LAYOUT(document_list);
DEFINE_LAYOUT(onvif_search);
DEFINE_LAYOUT(onvif_info);
DEFINE_LAYOUT(onvif_preview);
DEFINE_LAYOUT(onvif_input);
DEFINE_LAYOUT(ipc_camera_m_add);
DEFINE_LAYOUT(standby_picture);

typedef struct
{
	void *user_data;
	// void *main_obj;
	void (*down)(lv_obj_t *obj);
	void (*up)(lv_obj_t *obj);
	void (*anything_func)(lv_obj_t *obj, lv_event_t event);
} btn_data;




#define btn_data_create(down_ex, up_ex, user_data_ex) {.down = down_ex,           \
						       .up = up_ex,               \
						       .user_data = user_data_ex, \
						       .anything_func = NULL};

#define btn_data_up_create(x) {.user_data = NULL, \
			       .down = NULL,      \
			       .up = x,           \
			       .anything_func = NULL};

#define btn_data_anything_create(x) {.user_data = NULL, \
				     .down = NULL,      \
				     .up = NULL,        \
				     .anything_func = x};

/***
**   日期:2022-05-28 14:56:15
**   作者: leo.liu
**   函数作用：标记进入触发是否时call机后进入，客户需求触发过的不需要再次记录
**   参数说明:
***/
void alarm_trigger_call_set(bool en);
/***
**   日期:2022-07-18 09:05:06
**   作者: leo.liu
**   函数作用：获取触发call机的警报状态
**   参数说明:
***/
bool alarm_trigger_call_get(void);
/***
**   日期:2022-05-31 15:24:51
**   作者: leo.liu
**   函数作用：检测是否需要警报触发
**   参数说明:
***/
bool alarm_trigger_check(void);
DEFINE_LAYOUT(alarm);
// void intercom_talk_status_is_in(void);
// void intercom_talk_status_is_out(void);
DEFINE_LAYOUT(intercom);
DEFINE_LAYOUT(wifi_display);
DEFINE_LAYOUT(wifi_info);
void layout_wifi_input_set_user_name(const unsigned char *name);
void layout_wifi_input_mode_set(char mode);
unsigned int layout_intercom_call_mask_get(void);
void layout_intercom_call_mask_set(unsigned int mask);
DEFINE_LAYOUT(wifi_input);
DEFINE_LAYOUT(adj_display);
DEFINE_LAYOUT(alarm_list);

DEFINE_LAYOUT(always_record);

#endif