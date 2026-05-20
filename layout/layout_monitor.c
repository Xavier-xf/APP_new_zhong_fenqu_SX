#include "layout_define.h"
#include "video_input.h"
#include "audio_output.h"
#include "audio_output.h"

#define LAYOUT_MONITOR_OBJ_ID_HEAD_CONT 0X01
#define LAYOUT_MONITOR_OBJ_ID_HEAD_CH 0X02
#define LAYOUT_MONITOR_OBJ_ID_HEAD_TIME 0X03
#define LAYOUT_MONITOR_OBJ_ID_HEAD_SD 0X04
#define LAYOUT_MONITOR_OBJ_ID_HEAD_TIMEOUT 0X05
#define LAYOUT_MONITOR_OBJ_ID_ADJ 0x06
#define LAYOUT_MONITOR_OBJ_ID_VOL 0X07
#define LAYOUT_MONITOR_OBJ_ID_TALK 0X08
#define LAYOUT_MONITOR_OBJ_ID_CANCEL 0X09
#define LAYOUT_MONITOR_OBJ_ID_NORMAL_LOCK 0X0A
#define LAYOUT_MONITOR_OBJ_ID_1_LOCK 0X0B
#define LAYOUT_MONITOR_OBJ_ID_2_LOCK 0X0C
#define LAYOUT_MONITOR_OBJ_ID_REC 0X0D
#define LAYOUT_MONITOR_OBJ_ID_SNAP 0X0E
#define LAYOUT_MONITOR_OBJ_ID_CCTV_SWAP 0X0F
#define LAYOUT_MONITOR_OBJ_ID_DOOR_SWAP 0X10
#define LAYOUT_MONITOR_OBJ_ID_UNLOCK_ICON 0X11
#define LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT 0X12
#define LAYOUT_MONITOR_OBJ_ID_ADJ_CONT 0X13
#define LAYOUT_MONITOR_OBJ_ID_CONTRAST_CONT_CONT 0X14
#define LAYOUT_MONITOR_OBJ_ID_COLOR_CONT_CONT 0X15
#define LAYOUT_MONITOR_OBJ_ID_BRIGHTNESS_CONT_CONT 0X17
#define LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS 0X40
#define LAYOUT_MONITOR_OBJ_ID_ADJ_CONST 0X50
#define LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR 0X60
#define LAYOUT_MONITOR_OBJ_ID_VOL_CONT 0X18
#define LAYOUT_MONITOR_OBJ_ID_VOL_CONT_CONT 0X19
#define LAYOUT_MONITOR_OBJ_ID_VOL_VOL 0X70
#define LAYOUT_MONITOR_OBJ_ID_SD 0X1A
#define LAYOUT_MONITOR_OBJ_ID_CCTV_CANCEL 0x1B
#define LAYOUT_MONITOR_OBJ_ID_MUTE_ICON 0X1C
#define LAYOUT_MONITOR_OBJ_ID_INTER 0X1D
#define LAYOUT_MONITOR_OBJ_ID_TUYA 0X1E
#define LAYOUT_MONITOR_OBJ_ID_SECURITY 0X1F
#define LAYOUT_MONITOR_OBJ_ID_OUTDOOR_UNLOCK_ICON 0XA1

// door_n:0:door1 1:door2
// ring_n:0:ring1 1:ring2 2:ring3
#define RING_TIME_CHECK(door_n, ring_n) (user_data_get()->ring_attr[door_n][ring_n].timer_start < user_data_get()->ring_attr[door_n][ring_n].timer_end &&     \
										 user_data_get()->ring_attr[door_n][ring_n].timer_start <= (tm.tm_hour * 100 + tm.tm_min) &&                           \
										 user_data_get()->ring_attr[door_n][ring_n].timer_end > (tm.tm_hour * 100 + tm.tm_min)) ||                             \
											(user_data_get()->ring_attr[door_n][ring_n].timer_start > user_data_get()->ring_attr[door_n][ring_n].timer_end && \
											 user_data_get()->ring_attr[door_n][ring_n].timer_start <= (tm.tm_hour * 100 + tm.tm_min) &&                       \
											 (user_data_get()->ring_attr[door_n][ring_n].timer_end + 24 * 60 * 60) > (tm.tm_hour * 100 + tm.tm_min))

static lv_task_t *layout_monitor_call_record_task = NULL;
static void layout_montior_timeout_reset(int timeout);
static void layout_monitor_talk_btn_up(lv_obj_t *obj);
extern void monitor_ring_task_kill(void);
extern bool sleep_mode_enter_moniter_flag;
// static lv_task_t * momitor_ring_timer_task_t = NULL;

uint8_t door_z = 0, ring_z = 0;
/***
** 日期: 2022-05-14 14:14
** 作者: leo.liu
** 函数作用：通话状态
** 返回参数说明：
***/
static bool layout_monitor_talk_enable = false;
bool layout_monitor_talk_state(void)
{
	return layout_monitor_talk_enable;
}

static bool layout_monitor_unlock_enable = false;

static void layout_monitor_talk_btn_display(void);
static void layout_monitor_cancel_btn_display(void);
// static void layout_monitor_lock_normal_btn_display(void);
static void layout_monitor_lock_1_btn_display(void);
static void layout_monitor_lock_2_btn_display(void);
static void layout_monitor_cctv_swap_btn_display(void);
static void layout_monitor_door_swap_btn_display(void);
static void layout_monitor_unlock_icon_display(void);
static void layout_monitor_outdoor_unlock_icon_display(void);
static void layout_monitor_door1_call_func(void);
static void layout_monitor_door2_call_func(void);

static void layout_monitor_call_talk_btn_func(void);

static void layout_monitor_security_display(bool display);
static void layout_monitor_volume_bar_display(bool disp);


bool unclock_enable_state_get(void)
{
	return layout_monitor_unlock_enable;
}


/***
** 日期: 2022-05-13 10:02
** 作者: leo.liu
** 函数作用：正常刷新区域
** 返回参数说明：
***/
static void layout_monitor_refresh_1(void)
{
	refresh_area_t area[] = {
		{0, 0, 1024, 70},
		{797, 101, 206, 361},
		{43, 101, 50, 361},
		{38, 465, 983, 105}};
	gui_refresh_area(area, sizeof(area) / sizeof(refresh_area_t));
}
static void layout_monitor_refresh_2(void)
{
	refresh_area_t area[] = {
		{0, 0, 1024, 70},
		{38, 465, 983, 105},
		{436, 231, 152, 152}};
	gui_refresh_area(area, sizeof(area) / sizeof(refresh_area_t));
}

static void layout_monitor_refresh_3(void)
{
	return;
	refresh_area_t area[] = {
		{0, 0, 1024, 70},
		{909, 105, 80, 80},
		{38, 465, 983, 105}};
	gui_refresh_area(area, sizeof(area) / sizeof(refresh_area_t));
}

static void layout_monitor_refresh_5(void)
{
	refresh_area_t area[] = {
		{0, 0, 1024, 70},
		{43, 101, 50, 361},
		{797, 101, 206, 361},
		{0, 465, 1024, 125}};
	gui_refresh_area(area, sizeof(area) / sizeof(refresh_area_t));
}

static void layout_monitor_back_craete_up(lv_obj_t *obj)
{

	monitor_ring_task_kill();
	ringplay_play_stop();
	user_data_get()->monitor_alarm = 1;
	goto_layout(pLAYOUT(home));
}

/***
** 日期: 2022-05-13 10:40
** 作者: leo.liu
** 函数作用：创建返回按键
** 返回参数说明：
***/
static void layout_monitor_back_btn_create(lv_obj_t *parent)
{

	lv_obj_t *obj = lv_btn_create(lv_scr_act(), NULL);
	if (obj == NULL)
	{
		printf("create cancel btn failed \n");
		return;
	}

	lv_obj_set_pos(obj, 27, 10);
	lv_obj_set_size(obj, 60, 60);

	static rom_bin_info image = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image);

	lv_obj_set_style_local_pattern_align(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_back_craete_up);
	obj_click_event_listen(obj, &click_data);
}

/***
** 日期: 2022-05-13 10:44
** 作者: leo.liu
** 函数作用：通道显示
** 返回参数说明：
***/
static void layout_monitor_channel_label_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_HEAD_CONT);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_HEAD_CH);

	MON_CH ch = monitor_channel_get();
	const char *string = language_common_ch_string_get(ch);
	lv_label_set_text(label, string);
	if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
	{
		lv_obj_set_x(label, 100);
	}
	else
	{
		lv_obj_set_x(label, 106);
	}
}
/***
** 日期: 2022-05-13 10:40
** 作者: leo.liu
** 函数作用：创建显示通道
** 返回参数说明：
***/
static void layout_monitor_channel_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_label_create(parent, NULL);
	lv_obj_set_id(obj, LAYOUT_MONITOR_OBJ_ID_HEAD_CH);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 36, 23);
	lv_obj_set_size(obj, 110, 40);
	lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);
	layout_monitor_channel_label_display();
}

/***
** 日期: 2022-05-13 11:17
** 作者: leo.liu
** 函数作用：时间文本希纳是
** 返回参数说明：
***/
static void layout_monitor_time_label_display(lv_obj_t *label)
{
	if (label == NULL)
	{
		lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_HEAD_CONT);
		label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_HEAD_TIME);
	}
	struct tm *tm = (struct tm *)label->user_data;
	lv_label_set_text_fmt(label, "%04d-%02d-%02d %02d:%02d", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
	if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
	{
		lv_obj_set_x(label, 190);
	}
	else
	{
		lv_obj_set_x(label, 212);
	}
}
/***
** 日期: 2022-05-13 10:53
** 作者: leo.liu
** 函数作用：创建时间
** 返回参数说明：
***/
static void layout_monitor_time_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_label_create(parent, NULL);
	lv_obj_set_id(obj, LAYOUT_MONITOR_OBJ_ID_HEAD_TIME);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 150, 23);
	lv_obj_set_size(obj, 240, 35);
	lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);

	static struct tm tm;
	user_time_read(&tm);
	obj->user_data = &tm;
	layout_monitor_time_label_display(obj);
}

/***
**   日期:2022-06-15 11:54:19
**   作者: leo.liu
**   函数作用：延时退出任务
**   参数说明:
***/
static void layout_monitor_delay_goto_task(lv_task_t *task)
{
	goto_layout(pLAYOUT(home));
}
/***
** 日期: 2022-05-13 11:15
** 作者: leo.liu
** 函数作用：超时文本显示
** 返回参数说明：
***/
static void layout_monitor_timeout_label_display(lv_obj_t *parent)
{
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_HEAD_TIMEOUT);
	int *ptimeout = (int *)label->user_data;
	int timeout = *ptimeout;
	lv_label_set_text_fmt(label, "%02d:%02d", timeout / 60, timeout % 60);

	if (timeout != 0)
	{
		(*ptimeout)--;
	}
	else
	{
#if 1

		lv_layout_task_create(layout_monitor_delay_goto_task, 1000, LV_TASK_PRIO_MID, NULL);

#else
		MON_ENTER_FLG flg = monitor_enter_mask_get();
		*ptimeout = flg == MON_ENTER_CALL ? 60 : 30;
#endif
	}
	if ((timeout == 5) && (user_data_get()->home_mode == 0) && (monitor_enter_mask_get() == MON_ENTER_CALL))
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_REC);
		if (video_record_status_get() == true)
		{
			record_video_close();
			lv_obj_set_state(obj, LV_STATE_DEFAULT);
		}
		ringplay_play_form_index(13, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
	}
}
/***
** 日期: 2022-05-13 10:53
** 作者: leo.liu
** 函数作用：创建超时文本
** 返回参数说明：
***/
static void layout_monitor_timeout_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_label_create(parent, NULL);
	lv_obj_set_id(obj, LAYOUT_MONITOR_OBJ_ID_HEAD_TIMEOUT);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 880, 23);
	lv_obj_set_size(obj, 100, 35);
	lv_label_set_align(obj, LV_LABEL_ALIGN_RIGHT);

	static int timeout = 0;
	// MON_ENTER_FLG flg = monitor_enter_mask_get();
	MON_CH ch = monitor_channel_get();
	if (/* (flg == MON_ENTER_CALL) || */ (ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2) || (ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		timeout = 60;
		// timeout = 120;
	}
	else
	{
		timeout = 30;
		// timeout = 120;
	}
	obj->user_data = &timeout;
	layout_monitor_timeout_label_display(parent);
}

static void layout_monitor_timer_task(lv_task_t *task_t)
{
	lv_obj_t *parent = (lv_obj_t *)task_t->user_data;

	lv_obj_t *time_label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_HEAD_TIME);
	struct tm *label_tm = (struct tm *)time_label->user_data;
	struct tm cur_tm;
	user_time_read(&cur_tm);
	if (cur_tm.tm_min != label_tm->tm_min)
	{
		*label_tm = cur_tm;
		layout_monitor_time_label_display(time_label);
	}
	layout_monitor_timeout_label_display(parent);
}
/***
**   日期:2022-07-05 09:12:21
**   作者: leo.liu
**   函数作用：静音图标显示
**   参数说明:
***/
static void layout_monitor_mute_icon_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_HEAD_CONT);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_MUTE_ICON);
	lv_obj_set_hidden(obj, ((user_data_get()->audio.ring_mute == true)) ? false : true);
}
/***
**   日期:2022-07-05 09:08:44
**   作者: leo.liu
**   函数作用：静音图标创建
**   参数说明:
***/
static void layout_monitor_mute_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, LAYOUT_MONITOR_OBJ_ID_MUTE_ICON);
	lv_obj_set_pos(obj, 699, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_MON_MUTE_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	layout_monitor_mute_icon_display();
}
/***
** 日期: 2022-05-17 14:26
** 作者: leo.liu
** 函数作用：移动侦测图标创建
** 返回参数说明：
***/
static void layout_monitor_motion_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 750, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_01_IC_IC_LIST_VISITOR_IMG_PNG);
	if (user_data_get()->motion.saving_fmt == 0)
	{
		img.offset = ROM_UI_01_IC_IC_LIST_VISITOR_IMG_PNG;
		img.size = ROM_UI_01_IC_IC_LIST_VISITOR_IMG_PNG_SIZE;
	}
	else
	{
		img.offset = ROM_UI_01_IC_IC_LIST_VISITOR_VIDEO_PNG;
		img.size = ROM_UI_01_IC_IC_LIST_VISITOR_VIDEO_PNG_SIZE;
	}
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
}

static void layout_monitior_auto_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 801, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_AUTO_PNG);
	if (user_data_get()->auto_record_mode == 0)
	{
		img.offset = ROM_UI_01_IC_IC_MONITORING_MANUAL_PNG;
		img.size = ROM_UI_01_IC_IC_MONITORING_MANUAL_PNG_SIZE;
	}
	else
	{
		img.offset = ROM_UI_01_IC_IC_MONITORING_AUTO_PNG;
		img.size = ROM_UI_01_IC_IC_MONITORING_AUTO_PNG_SIZE;
	}
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
}

static void layout_monitor_sd_icon_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_HEAD_CONT);
	lv_obj_t *icon = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_SD);
	if (media_sdcard_insert_check() == true)
	{
		// lv_obj_set_hidden(icon, false);
		lv_obj_set_state(icon, media_sdcard_full_check() == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
		// copy_flash_photo_to_sd();
	}
	else
	{
		lv_obj_set_state(icon, LV_STATE_DISABLED);
		// lv_obj_set_hidden(icon, true);
	}

	if (video_record_status_get() == true)
	{
		video_record_stop();
		lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
		if (parent != NULL)
		{
			lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_REC);
			if (obj != NULL)
			{
				lv_obj_set_state(obj, LV_STATE_DEFAULT);
			}
		}
	}
	if ((media_sdcard_insert_check() == false) && (user_data_get()->auto_record_mode == 2))
	{
		user_data_get()->auto_record_mode = 1;
	}
}

/***
** 日期: 2022-05-17 14:34
** 作者: leo.liu
** 函数作用：sd创建
** 返回参数说明：
***/
static void layout_monitor_sd_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, LAYOUT_MONITOR_OBJ_ID_SD);
	lv_obj_set_pos(obj, 852, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img_n = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_SDCARD_PNG);
	static rom_bin_info img_f = rom_bin_info_get(ROM_UI_01_IC_IC_SYSTEM_SDCARD_FULL_PNG);
	static rom_bin_info img_no = rom_bin_info_get(ROM_UI_MONITOR_NO_SD_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_n);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &img_f);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DISABLED, &img_no);
}
#if 0
/***
** 日期: 2022-05-17 15:12
** 作者: leo.liu
** 函数作用：创建cctv页面返回按钮
** 返回参数说明：
***/
static void layout_monitor_cctv_cancel_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}
static void layout_monitor_cctv_cancel_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_HEAD_CONT);
	if (parent != NULL)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_CCTV_CANCEL);
		if (obj != NULL)
		{
			if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
			{
				lv_obj_set_hidden(obj, false);
			}
			else
			{
				lv_obj_set_hidden(obj, true);
			}
		}
	}
}

static void layout_monitor_cctv_cancel_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, LAYOUT_MONITOR_OBJ_ID_CCTV_CANCEL);
	lv_obj_set_pos(obj, 32, 15);
	lv_obj_set_size(obj, 43, 43);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_02_BTN_BTN_TITLE_BACK_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_cctv_cancel_up);
	obj_click_event_listen(obj, &click_data);
}
#endif
/***
** 日期: 2022-05-13 10:29
** 作者: leo.liu
** 函数作用：创建顶部显示区域
** 返回参数说明：
***/
static void layout_monitor_head_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_HEAD_CONT);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 82);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_40);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x161616));

#if 0
	if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
	{
		layout_monitor_cctv_cancel_create(cont);
		layout_monitor_cctv_cancel_display();
	}
#endif
	layout_monitor_back_btn_create(cont);
	layout_monitor_channel_label_create(cont);
	layout_monitor_time_label_create(cont);
	layout_monitor_timeout_label_create(cont);

	if (user_data_get()->motion.enable == true)
	{
		layout_monitor_motion_icon_create(cont);
	}
	layout_monitor_mute_icon_create(cont);
	layout_monitior_auto_icon_create(cont);
	layout_monitor_sd_icon_create(cont);
	layout_monitor_sd_icon_display();

	lv_layout_task_create(layout_monitor_timer_task, 1000, LV_TASK_PRIO_MID, cont);
}

static void layout_monitor_mix_task(lv_task_t *task_t)
{
	lv_obj_t *obj = (lv_obj_t *)task_t->user_data;
	if ((lv_obj_get_hidden(obj) == true) || (lv_obj_get_hidden(lv_obj_get_parent(obj)) == true))
	{
		return;
	}
	const rom_bin_info *img = lv_obj_get_style_pattern_image(obj, LV_BTN_PART_MAIN);
	const lv_img_decoder_dsc_t *dsc = lv_img_cache_png_get_from_rom_bin((void *)img);
	if (dsc != NULL)
	{
		lv_obj_t *parent = lv_obj_get_parent(obj);
		gui_alpha_mix_pos(dsc, lv_obj_get_x(obj) + lv_obj_get_x(parent), lv_obj_get_y(obj) + lv_obj_get_y(parent));
	}
}

/***
** 日期: 2022-05-13 14:17
** 作者: leo.liu
** 函数作用：监控页面的按钮创建基础函数
** 返回参数说明：
***/
lv_obj_t *layout_monitor_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *img_1, rom_bin_info *img_2, int btn_id, bool mix)
{
	lv_obj_t *btn = lv_btn_create(parent ? parent : lv_scr_act(), NULL);

	if (btn_id > 0)
	{
		lv_obj_set_id(btn, btn_id);
	}
	lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);

	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	if (img_1 != NULL)
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, img_1);
	}

	if (img_2 != NULL)
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_CHECKED, img_2);
	}

	if (click_data != NULL)
	{
		obj_click_event_listen(btn, click_data);
	}

	if (mix == true)
	{
		lv_task_t *task_t = lv_layout_task_create(layout_monitor_mix_task, 40, LV_TASK_PRIO_MID, btn);
		lv_task_ready(task_t);
	}
	return btn;
}

/***
**   日期:2022-07-14 16:42:29
**   作者: leo.liu
**   函数作用：门口机通道切换按钮显示
**   参数说明:
***/
static void layout_monitor_door_swap_btn_display(void)
{

	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_DOOR_SWAP);
	MON_CH ch = monitor_channel_get();
	if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
	{
		lv_obj_set_hidden(obj, true);
		return;
	}

	lv_obj_set_hidden(obj, false);
	if (ch == MON_CH_DOOR1)
	{
		lv_obj_set_state(obj, LV_STATE_CHECKED);
	}
	else if (ch == MON_CH_DOOR2)
	{
		lv_obj_set_state(obj, LV_STATE_DEFAULT);
	}
	else
	{
		lv_obj_set_hidden(obj, true);
	}
}
/***
**   日期:2022-07-14 16:43:07
**   作者: leo.liu
**   函数作用：监控通道切换
**   参数说明:
***/
void layout_monitor_door_swap_btn_up(lv_obj_t *obj)
{

	ringplay_play_stop();
	user_data_get()->monitor_alarm = 1;
	MON_ENTER_FLG flg = monitor_enter_mask_get();
	if (flg == MON_ENTER_CALL)
	{
		return;
	}
	if (layout_monitor_talk_state() == true)
	{
		return;
	}
	lv_obj_t *obj_1 = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_REC);
	lv_obj_set_state(obj_1, LV_STATE_DEFAULT);
	video_record_stop();

	char channel = monitor_channel_get();

	if ((OwnID != 1) && (obj != NULL))
	{
		channel = monitor_channel_get();
		if (channel == MON_CH_DOOR1)
		{
			intercom_cmd_send(1, CMD_SWITCH_DOOR2);
			monitor_channel_set(MON_CH_DOOR2);
		}
		else if (channel == MON_CH_DOOR2)
		{
			intercom_cmd_send(1, CMD_SWITCH_DOOR1);
			monitor_channel_set(MON_CH_DOOR1);
		}
		else if (channel == MON_CH_CCTV1)
		{
			intercom_cmd_send(1, CMD_SWITCH_CCTV2);
			monitor_channel_set(MON_CH_CCTV2);
		}
		else if (channel == MON_CH_CCTV2)
		{
			intercom_cmd_send(1, CMD_SWITCH_CCTV1);
			monitor_channel_set(MON_CH_CCTV1);
		}
	}
	else if (OwnID == 1)
	{
		if ((channel == MON_CH_DOOR1) && (monitor_valid_channel_check(MON_CH_DOOR2) == true))
		{
			monitor_channel_set(MON_CH_DOOR2);
		}
		else if ((channel == MON_CH_DOOR2) && (monitor_valid_channel_check(MON_CH_DOOR1) == true))
		{
			monitor_channel_set(MON_CH_DOOR1);
		}
		else if ((channel == MON_CH_CCTV1) && (monitor_valid_channel_check(MON_CH_CCTV2) == true))
		{
			monitor_channel_set(MON_CH_CCTV2);
		}
		else if ((channel == MON_CH_CCTV2) && (monitor_valid_channel_check(MON_CH_CCTV3) == true))
		{
			monitor_channel_set(MON_CH_CCTV3);
		}
		else if ((channel == MON_CH_CCTV3) && (monitor_valid_channel_check(MON_CH_CCTV4) == true))
		{
			monitor_channel_set(MON_CH_CCTV4);
		}
		else if ((channel == MON_CH_CCTV4) && (monitor_valid_channel_check(MON_CH_CCTV1) == true))
		{
			monitor_channel_set(MON_CH_CCTV1);
		}
		else
		{
			return;
		}
	}

	if ((OwnID == 1) || (obj == NULL))
	{

		layout_monitor_door_swap_btn_display();
		layout_monitor_cctv_swap_btn_display();
		layout_monitor_channel_label_display();
		layout_monitor_talk_btn_display();
		layout_monitor_cancel_btn_display();
		// layout_monitor_lock_normal_btn_display();
		layout_monitor_lock_1_btn_display();
		layout_monitor_lock_2_btn_display();

		MON_CH ch = monitor_channel_get();
		layout_monitor_security_display(((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) ? true : false);

		// /*解决噪声问题*/
		// if (ch == MON_CH_DOOR1)
		// {
		// 	audio_to_outdoor1_pin_ctrl(true);
		// }
		// else if (ch == MON_CH_DOOR2)
		// {
		// 	audio_to_outdoor2_pin_ctrl(true);
		// }

		monitor_open(true, 0x03);
	}

	adj_value_refresh_display();
	layout_montior_timeout_reset(layout_monitor_talk_state() == true ? 60 : 30);
	// layout_montior_timeout_reset(120);
}
/***
** 日期: 2022-05-14 13:52
** 作者: leo.liu
** 函数作用：创建门口机切换按钮
** 返回参数说明：
***/
static void layout_monitor_door_swap_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_door_swap_btn_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_DOOR_HANDOFF_1_PNG);
	static rom_bin_info img2 = rom_bin_info_get(ROM_UI_DOOR_HANDOFF_2_PNG);
	layout_monitor_btn_base_create(parent, 38 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent) + 12, 80, 80, &click_data, &img1, &img2, LAYOUT_MONITOR_OBJ_ID_DOOR_SWAP, true);
	layout_monitor_door_swap_btn_display();
}
/***
**   日期:2022-07-14 16:43:22
**   作者: leo.liu
**   函数作用：CCTV 切换按钮显示
**   参数说明:
***/
static void layout_monitor_cctv_swap_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_CCTV_SWAP);
	if (obj == NULL)
	{
		return;
	}
	MON_CH ch = monitor_channel_get();
	static rom_bin_info img_1 = rom_bin_info_get(ROM_UI_DOOR_HANDOFF_1_PNG);
	static rom_bin_info img_2 = rom_bin_info_get(ROM_UI_DOOR_HANDOFF_2_PNG);
	static rom_bin_info img_3 = rom_bin_info_get(ROM_UI_CCTV_NEW_001_PNG);
	static rom_bin_info img_4 = rom_bin_info_get(ROM_UI_CCTV_NEW_002_PNG);
	static rom_bin_info img_5 = rom_bin_info_get(ROM_UI_CCTV_NEW_003_PNG);
	static rom_bin_info img_6 = rom_bin_info_get(ROM_UI_CCTV_NEW_004_PNG);
	if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
	{
		if (ch == MON_CH_CCTV1)
		{
			if(monitor_valid_channel_check(MON_CH_CCTV2) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_4);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV3) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_5);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV4) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_6);
			}
		}
		else if (ch == MON_CH_CCTV2)
		{
			if(monitor_valid_channel_check(MON_CH_CCTV3) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_5);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV4) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_6);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV1) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_3);
			}
		}
		else if (ch == MON_CH_CCTV3)
		{
			if(monitor_valid_channel_check(MON_CH_CCTV4) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_6);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV1) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_3);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV2) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_4);
			}
		}
		else if (ch == MON_CH_CCTV4)
		{
			if(monitor_valid_channel_check(MON_CH_CCTV1) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_3);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV2) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_4);
			}
			else if(monitor_valid_channel_check(MON_CH_CCTV3) == true)
			{
				lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_5);
			}
		}
		lv_obj_set_x(obj, 38 - lv_obj_get_x(lv_obj_get_parent(obj)));
		return;
	}

	if (ch == MON_CH_DOOR1)
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_3);
		lv_obj_set_x(obj, 144 - lv_obj_get_x(lv_obj_get_parent(obj)));
	}
	else if (ch == MON_CH_DOOR2)
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_4);
		lv_obj_set_x(obj, 144 - lv_obj_get_x(lv_obj_get_parent(obj)));
	}
	else if (ch == MON_CH_CCTV1)
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_1);
		lv_obj_set_x(obj, 704 - lv_obj_get_x(lv_obj_get_parent(obj)));
	}
	else if (ch == MON_CH_CCTV2)
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img_2);
		lv_obj_set_x(obj, 704 - lv_obj_get_x(lv_obj_get_parent(obj)));
	}
}
/***
**   日期:2022-07-14 16:43:40
**   作者: leo.liu
**   函数作用：CCTV 切换按钮按下函数
**   参数说明:
***/
static void test_delay_task(lv_task_t* t)
{
	monitor_open(true, 0x03);
	if(t)
	{
		lv_task_del(t);
	}
}

void layout_monitor_cctv_swap_btn_up(lv_obj_t *obj)
{

	ringplay_play_stop();
	user_data_get()->monitor_alarm = 1;

	extern void audio_to_inter_pin_ctrl(bool en);
	LOG_RED("cctv swap :\n");

	MON_ENTER_FLG flg = monitor_enter_mask_get();
	if (flg == MON_ENTER_CALL)
	{
		return;
	}
	if (layout_monitor_talk_state() == true)
	{
		return;
	}

	video_record_stop();
	lv_obj_t *obj_1 = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_REC);
	lv_obj_set_state(obj_1, LV_STATE_DEFAULT);

	MON_CH ch = monitor_channel_get();
	if ((OwnID != 1) && (obj != NULL))
	{
		if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
		{
			ch = ch == MON_CH_CCTV1 ? MON_CH_CCTV2 : MON_CH_CCTV1;
		}
		else
		{
			ch = ch == MON_CH_DOOR1 ? MON_CH_CCTV1 : ch == MON_CH_DOOR2 ? MON_CH_CCTV2
												 : ch == MON_CH_CCTV1	? MON_CH_DOOR1
																		: MON_CH_DOOR2;
		}
		monitor_channel_set(ch);
		intercom_cmd_send(1, ch == MON_CH_DOOR1 ? CMD_SWITCH_DOOR1 : ch == MON_CH_DOOR2 ? CMD_SWITCH_DOOR2
																 : ch == MON_CH_CCTV1	? CMD_SWITCH_CCTV1
																						: CMD_SWITCH_CCTV2);
	}
	else
	{
		if (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV)
		{
			if (OwnID == 1)
			{
				// if ((monitor_valid_channel_check(MON_CH_CCTV1) == false) || (monitor_valid_channel_check(MON_CH_CCTV2) == false))
				// {
				// 	return;
				// }
				int i = 0;
				for (i = 0; i < 3; i++)
				{
					ch = ch == MON_CH_CCTV1 ? MON_CH_CCTV2 : ch == MON_CH_CCTV2 ? MON_CH_CCTV3
														 : ch == MON_CH_CCTV3	? MON_CH_CCTV4
																				: MON_CH_CCTV1;
					if (monitor_valid_channel_check(ch) == true)
					{
						break;
					}
				}
				if (i == 3)
					return;
				monitor_channel_set(ch);
			}
		//	monitor_open(true, 0x03);
			layout_monitor_cctv_swap_btn_display();
			layout_monitor_channel_label_display();
			adj_value_refresh_display();
			lv_obj_t *cont = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_CONTRAST_CONT_CONT);
			lv_obj_t *color = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_COLOR_CONT_CONT);
			lv_obj_t *bri = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_BRIGHTNESS_CONT_CONT);
			if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
			{
				lv_obj_set_hidden(cont,true);
				lv_obj_set_hidden(color,true);
				lv_obj_set_hidden(bri, true);
			}
			else
			{
				lv_obj_set_hidden(cont,false);
				lv_obj_set_hidden(color,false);
				lv_obj_set_hidden(bri, false);
			}

			video_input_skip_frame_count_set(10);
			lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
			lv_task_t task;
			if (parent != NULL)
			{
				lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_SNAP);
				if(obj)
				{
					task.user_data = obj;
					layout_monitor_mix_task(&task);
					lv_obj_invalidate(obj);
				}
				obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_REC);
				if(obj)
				{
					task.user_data = obj;
					layout_monitor_mix_task(&task);
					lv_obj_invalidate(obj);
				}
				obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_CCTV_SWAP);
				if(obj)
				{
					task.user_data = obj;
					layout_monitor_mix_task(&task);
					lv_obj_invalidate(obj);
				}
				// screen_force_refresh();
			}
			lv_layout_task_create(test_delay_task, 100, LV_TASK_PRIO_MID, NULL);
			layout_montior_timeout_reset(60);
			return;
		}
		else
		{
			if (OwnID == 1)
			{
				if (ch == MON_CH_DOOR1)
				{
					if (monitor_valid_channel_check(MON_CH_CCTV1))
					{
						ch = MON_CH_CCTV1;
					}
					else
					{
						return;
					}
				}
				else if (ch == MON_CH_DOOR2)
				{
					if (monitor_valid_channel_check(MON_CH_CCTV2))
					{
						ch = MON_CH_CCTV2;
					}
					else
					{
						return;
					}
				}
				else if (ch == MON_CH_CCTV1)
				{
					ch = MON_CH_DOOR1;
				}
				else if (ch == MON_CH_CCTV2)
				{
					ch = MON_CH_DOOR2;
				}
			}
			monitor_channel_set(ch);
			monitor_open(true, 0x03);
			layout_monitor_door_swap_btn_display();
			layout_monitor_cctv_swap_btn_display();
			layout_monitor_channel_label_display();

			layout_monitor_talk_btn_display();
			layout_monitor_cancel_btn_display();
			// layout_monitor_lock_normal_btn_display();
			layout_monitor_lock_1_btn_display();
			layout_monitor_lock_2_btn_display();
			lv_obj_t *odj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT);
			lv_obj_set_hidden(odj, true);
			// layout_monitor_brightness_display(false);
			// layout_monitor_contrast_display(false);
			// layout_monitor_color_display(false);
			layout_monitor_volume_bar_display(false);

			layout_monitor_security_display((((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) && (layout_monitor_talk_enable == false)) ? true : false);
		}
	}
	adj_value_refresh_display();
	layout_montior_timeout_reset(layout_monitor_talk_state() == true ? 60 : monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV ? 60
																															  : 30);
	// layout_montior_timeout_reset(120);
}
/***
** 日期: 2022-05-14 13:52
** 作者: leo.liu
** 函数作用：创建cctv切换按钮
** 返回参数说明：
***/
static void layout_monitor_cctv_swap_btn_create(lv_obj_t *parent)
{
	if ((monitor_valid_channel_check(MON_CH_CCTV1) == true) ||
		(monitor_valid_channel_check(MON_CH_CCTV2) == true) ||
		(monitor_valid_channel_check(MON_CH_CCTV3) == true) ||
		(monitor_valid_channel_check(MON_CH_CCTV4) == true))
	{
		static rom_bin_info img1 = rom_bin_info_get(ROM_UI_CCTV_NEW_001_PNG);
		static rom_bin_info img2 = rom_bin_info_get(ROM_UI_CCTV_NEW_002_PNG);
		static obj_click_data click_data = obj_click_data_up_create(layout_monitor_cctv_swap_btn_up);
		layout_monitor_btn_base_create(parent, 38 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent) + 12, 80, 80, &click_data, &img1, &img2, LAYOUT_MONITOR_OBJ_ID_CCTV_SWAP, true);
		layout_monitor_cctv_swap_btn_display();
	}
}
/***
** 日期: 2022-05-14 14:20
** 作者: leo.liu
** 函数作用：标准状态下通话按钮坐标
** 返回参数说明：
***/
static void layout_monitor_standard_talk_display(lv_obj_t *obj, MON_CH ch)
{
	lv_obj_set_x(obj, 462 - lv_obj_get_x(lv_obj_get_parent(obj)));
}
// /***
// **   日期:2022-07-14 16:44:01
// **   作者: leo.liu
// **   函数作用：快速模式下通话按钮显示
// **   参数说明:
// ***/
// static void layout_monitor_fast_talk_display(lv_obj_t *obj, MON_CH ch)
// {
// 	if ((ch == MON_CH_DOOR1) || (user_data_get()->etc.door2_lock_num == 1))
// 	{
// 		lv_obj_set_x(obj, 337 - lv_obj_get_x(lv_obj_get_parent(obj)));
// 	}
// 	else if ((ch == MON_CH_DOOR2) && (user_data_get()->etc.door2_lock_num == 2))
// 	{
// 		lv_obj_set_x(obj, 276 - lv_obj_get_x(lv_obj_get_parent(obj)));
// 	}
// }
/***
** 日期: 2022-05-14 14:11
** 作者: leo.liu
** 函数作用：通话按钮显示
** 返回参数说明：
***/
static void layout_monitor_talk_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_TALK);
	MON_CH ch = monitor_channel_get();
	if ((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2) || (ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4) || (layout_monitor_talk_enable == true))
	{
		lv_obj_set_hidden(obj, true);
	}
	else
	{
		layout_monitor_standard_talk_display(obj, ch);
		lv_obj_set_hidden(obj, false);

		// if (user_data_get()->etc.open_the_door == 0)
		// {
		// 	layout_monitor_standard_talk_display(obj, ch);
		// }
		// else
		// {
		// 	layout_monitor_fast_talk_display(obj, ch);
		// }
	}
}

static void monitor_AMP_ckeck_task(lv_task_t *task)
{
	// GPIO_LEVEL level;
	// gpio_level_read(75, &level);
	if (layout_monitor_talk_enable == true)
	{
		// LOG_RED("AMP pin level:%d\n",level);
		power_amplifier_enable(true);
	}
	// lv_task_del(task);
}
/***
**   日期:2022-07-14 16:44:17
**   作者: leo.liu
**   函数作用：通话按钮按下
**   参数说明:
***/
static void layout_monitor_talk_btn_up(lv_obj_t *obj)
{
	user_data_get()->monitor_alarm = 1;
	ringplay_play_stop();

	if (layout_monitor_talk_enable == false)
	{

		layout_monitor_talk_enable = true;

		sleep_mode_enter_moniter_flag = false;

		tuya_api_preview_quit();
		layout_monitor_talk_btn_display();
		layout_monitor_cancel_btn_display();
		layout_monitor_security_display(false);
		// layout_monitor_lock_normal_btn_display();
		// layout_monitor_lock_1_btn_display();
		// layout_monitor_lock_2_btn_display();
		// layout_monitor_vol_btn_display();

		door_audio_talk(AUDIO_CH_INTER);

		/***** 开锁处理 *****/
		MON_CH ch = monitor_channel_get();
		if (OwnID == 1)
		{
			if (ch == MON_CH_DOOR1)
			{
				door_audio_talk(AUDIO_CH_DOOR1);
			}
			else if (ch == MON_CH_DOOR2)
			{
				door_audio_talk(AUDIO_CH_DOOR2);
			}
			intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR_CALL_ANSWER);
			// intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR_CALL_ANSWER);
		}
		else
		{
			// video_display_preview_enable(false);
			
			if (ch == MON_CH_DOOR1)
			{
				intercom_cmd_send(CODE_ALL_ID, CMD_DOOR1_AUDIO);
			}
			else if (ch == MON_CH_DOOR2)
			{
				intercom_cmd_send(CODE_ALL_ID, CMD_DOOR2_AUDIO);
			}
			door_audio_talk(AUDIO_CH_INTER);
			talk_output_gpio_ctrl_volume(user_data_get()->audio.door_talk_volume);
			// usleep(1000 * 1000);
			// video_display_preview_enable(true);
			
		}

		// monitor_enter_mask_set(MON_ENTER_DISPLAY);
		monitor_enter_mask_set(MON_ENTER_TALK);
		/***** 设置上传到tuya app为样品流 ****/
		if (monitor_enter_mask_get() != MON_ENTER_CALL)
		{
			h264_encode_sample_enable(true);
		}

		//	extern int tuya_ipc_door_bell_press_stop();
		//	tuya_ipc_door_bell_press_stop();
		tuya_api_doorbell_handup();

		tuya_pvew_video_count_fource_set(0);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_STOP, 0);
		layout_montior_timeout_reset(60);
		// layout_montior_timeout_reset(120);

		// standby_timer_close();
	}

	lv_layout_task_create(monitor_AMP_ckeck_task, 100, LV_TASK_PRIO_MID, NULL);
}
/***
** 日期: 2022-05-14 13:52
** 作者: leo.liu
** 函数作用：创建通话按钮
** 返回参数说明：
***/
static void layout_monitor_talk_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_talk_btn_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_3_CALL_CALL_PNG);
	layout_monitor_btn_base_create(parent, 462 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent), 100, 100, &click_data, &img1, NULL, LAYOUT_MONITOR_OBJ_ID_TALK, true);
	layout_monitor_talk_btn_display();
}
/***
**   日期:2022-07-14 16:44:34
**   作者: leo.liu
**   函数作用：监控返回按钮事件函数
**   参数说明:
***/
static void layout_monitor_cancel_btn_up(lv_obj_t *obj)
{
	user_data_get()->monitor_alarm = 1;
	// if (tuya_api_app_talk_check() == true)
	// {
	// 	return;
	// }
	// monitor_ring_task_kill();
	goto_layout(pLAYOUT(home));
}
/***
**   日期:2022-07-14 16:44:54
**   作者: leo.liu
**   函数作用：标准模式下返回按钮显示
**   参数说明:
***/
static void layout_monitor_standard_cancel_display(lv_obj_t *obj, MON_CH ch)
{
	// if ((ch == MON_CH_DOOR1) || (user_data_get()->etc.door2_lock_num == 1))
	// {
	lv_obj_set_x(obj, 462 - lv_obj_get_x(lv_obj_get_parent(obj)));
	lv_obj_set_state(obj, LV_STATE_CHECKED);
	// }
	// else if ((ch == MON_CH_DOOR2) && (user_data_get()->etc.door2_lock_num == 2))
	// {
	// 	lv_obj_set_x(obj, (layout_monitor_talk_enable == true ? 641 : 520) - lv_obj_get_x(lv_obj_get_parent(obj)));
	// 	lv_obj_set_state(obj, layout_monitor_talk_enable == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
	// }
}
// /***
// **   日期:2022-07-14 16:45:10
// **   作者: leo.liu
// **   函数作用：快速模式下返回按钮显示
// **   参数说明:
// ***/
// static void layout_monitor_fast_cancel_display(lv_obj_t *obj, MON_CH ch)
// {
// 	if ((ch == MON_CH_DOOR1) || (user_data_get()->etc.door2_lock_num == 1))
// 	{
// 		lv_obj_set_x(obj, 580 - lv_obj_get_x(lv_obj_get_parent(obj)));
// 		lv_obj_set_state(obj, layout_monitor_talk_enable == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
// 	}
// 	else if ((ch == MON_CH_DOOR2) && (user_data_get()->etc.door2_lock_num == 2))
// 	{
// 		lv_obj_set_x(obj, 641 - lv_obj_get_x(lv_obj_get_parent(obj)));
// 		lv_obj_set_state(obj, layout_monitor_talk_enable == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
// 	}
// }
/***
** 日期: 2022-05-16 10:01
** 作者: leo.liu
** 函数作用：挂断返回按钮显示
** 返回参数说明：
***/
static void layout_monitor_cancel_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_CANCEL);
	MON_CH ch = monitor_channel_get();
	if (((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2) || (ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4)) && (layout_monitor_talk_enable == true))
	{
		lv_obj_set_hidden(obj, true);
		return;
	}
	if (layout_monitor_talk_enable)
	{
		lv_obj_set_hidden(obj, false);
		layout_monitor_standard_cancel_display(obj, ch);
	}
	else
	{
		lv_obj_set_hidden(obj, true);
	}
	// else
	// {
	// 	lv_obj_set_hidden(obj, false);

	// 	if (user_data_get()->etc.open_the_door == 0)
	// 	{
	// 		layout_monitor_standard_cancel_display(obj, ch);
	// 	}
	// 	else
	// 	{
	// 		layout_monitor_fast_cancel_display(obj, ch);
	// 	}
	// }
}
/***
** 日期: 2022-05-16 09:35
** 作者: leo.liu
** 函数作用：挂断按钮创建
** 返回参数说明：
***/
static void layout_monitor_cancel_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_cancel_btn_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_2_CALL_CANCEL_PNG);
	static rom_bin_info img2 = rom_bin_info_get(ROM_UI_1_CALL_ENDCALL_PNG);
	layout_monitor_btn_base_create(parent, 641 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent), 105, 105, &click_data, &img1, &img2, LAYOUT_MONITOR_OBJ_ID_CANCEL, true);
	layout_monitor_cancel_btn_display();
}

/***
**   日期:2022-07-14 16:46:34
**   作者: leo.liu
**   函数作用：开锁图标任务显示
**   参数说明:
***/
static void layout_monitor_unlock_icon_task(lv_task_t *task_t)
{
	LOG_BLUE("close door \n");
	layout_monitor_unlock_enable = false;
	layout_monitor_unlock_icon_display();
	lv_task_del(task_t);

	monitor_unlcok_close();
}
// static void layout_monitor_outunlock_icon_task(lv_task_t *task_t)
// {
// 	layout_monitor_unlock_enable = false;
// 	layout_monitor_outdoor_unlock_icon_display();
// 	lv_task_del(task_t);

// 	monitor_unlcok_close();
// }

/***
**   日期:2022-07-14 16:46:04
**   作者: leo.liu
**   函数作用：开门口机锁铃声播放前的回调处理
**   参数说明:
***/
// static void unlock_ringa_start_func(int index)
// {
// 	// /***** 开启功放 *****/
// 	// tuya_spk_sw_pin_enable(false);
// 	ring_to_outdoor_mute_pin_ctrl(true);
// }
/***
**   日期:2022-07-14 16:46:47
**   作者: leo.liu
**   函数作用：开门口机锁铃声结束的回调处理函数
**   参数说明:
***/
// static void unlock_ring_finish_func(int index)
// {
// 	/***** 开启功放 *****/
// 	tuya_spk_sw_pin_enable(true);
// 	call_ring_to_outdoor_ctrl(AUDIO_CH_DOOR1, false);
// }
// /***
// **   日期:2022-07-14 16:47:15
// **   作者: leo.liu
// **   函数作用：正常开锁图标按钮事件处理函数
// **   参数说明:
// ***/
// static void layout_monitor_lock_normal_btn_up(lv_obj_t *obj)
// {
// 	if (layout_monitor_unlock_enable == true)
// 	{
// 		return;
// 	}
// 	layout_monitor_unlock_enable = true;
// 	layout_monitor_unlock_icon_display();
// 	lv_layout_task_create(layout_monitor_unlock_icon_task, 1500, LV_TASK_PRIO_MID, NULL);

// 	int num = 0;
// 	MON_CH ch = monitor_channel_get();
// 	if (ch == MON_CH_DOOR1)
// 	{
// 		call_ring_to_outdoor_ctrl(AUDIO_CH_DOOR1, true);
// 		num = (user_data_get()->etc.door1_open_door_mode == 0) ? 1 : 2;
// 	}
// 	else if (ch == MON_CH_DOOR2)
// 	{
// 		call_ring_to_outdoor_ctrl(AUDIO_CH_DOOR2, true);
// 		num = 1;
// 	}
// 	monitor_unlock_open(num, ch);
// 	ringplay_play_form_index(7, 100, unlock_ringa_start_func, unlock_ring_finish_func, false);
// }
// /***
// **   日期:2022-07-14 16:47:46
// **   作者: leo.liu
// **   函数作用：标准开锁图标显示
// **   参数说明:
// ***/
// static void layout_monitor_standard_lock_normal_display(lv_obj_t *obj, MON_CH ch)
// {
// 	if ((ch == MON_CH_DOOR1) || (user_data_get()->etc.door2_lock_num == 1))
// 	{
// 		lv_obj_set_x(obj, 398 - lv_obj_get_x(lv_obj_get_parent(obj)));
// 		lv_obj_set_hidden(obj, layout_monitor_talk_enable ? false : true);
// 	}
// 	else if ((ch == MON_CH_DOOR2) && (user_data_get()->etc.door2_lock_num == 2))
// 	{
// 		lv_obj_set_hidden(obj, true);
// 	}
// }
// /***
// **   日期:2022-07-14 16:47:57
// **   作者: leo.liu
// **   函数作用：开锁开锁图标显示
// **   参数说明:
// ***/
// static void layout_monitor_fast_lock_normal_display(lv_obj_t *obj, MON_CH ch)
// {
// 	if ((ch == MON_CH_DOOR1) || (user_data_get()->etc.door2_lock_num == 1))
// 	{
// 		lv_obj_set_x(obj, 459 - lv_obj_get_x(lv_obj_get_parent(obj)));
// 		lv_obj_set_hidden(obj, false);
// 	}
// 	else if ((ch == MON_CH_DOOR2) && (user_data_get()->etc.door2_lock_num == 2))
// 	{
// 		lv_obj_set_hidden(obj, true);
// 	}
// }
// /***
// **   日期:2022-07-14 16:48:06
// **   作者: leo.liu
// **   函数作用：正常开锁图标显示
// **   参数说明:
// ***/
// static void layout_monitor_lock_normal_btn_display(void)
// {
// 	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_NORMAL_LOCK);
// 	MON_CH ch = monitor_channel_get();
// 	if ((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
// 	{
// 		lv_obj_set_hidden(obj, true);
// 		return;
// 	}

// 	if (user_data_get()->etc.open_the_door == 0)
// 	{
// 		layout_monitor_standard_lock_normal_display(obj, ch);
// 	}
// 	else
// 	{
// 		layout_monitor_fast_lock_normal_display(obj, ch);
// 	}
// }
// /***
// ** 日期: 2022-05-16 10:06
// ** 作者: leo.liu
// ** 函数作用：正常锁
// ** 返回参数说明：
// ***/
// static void layout_monitor_lock_normal_btn_create(lv_obj_t *parent)
// {
// 	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_lock_normal_btn_up);
// 	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_5_CALL_DOOR_OPEN_PNG);
// 	layout_monitor_btn_base_create(parent, 398 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent), 105, 105, &click_data, &img1, NULL, LAYOUT_MONITOR_OBJ_ID_NORMAL_LOCK, true);
// 	layout_monitor_lock_normal_btn_display();
// }
// /***
// ** 日期: 2022-05-16 16:15
// ** 作者: leo.liu
// ** 函数作用：锁1
// ** 返回参数说明：
// ***/
// static void layout_monitor_standard_lock_1_display(lv_obj_t *obj)
// {
// 	lv_obj_set_hidden(obj, layout_monitor_talk_enable ? false : true);
// }
/***
**   日期:2022-07-14 16:48:29
**   作者: leo.liu
**   函数作用：快速开锁1图标显示
**   参数说明:
***/
static void layout_monitor_fast_lock_1_display(lv_obj_t *obj)
{
	lv_obj_set_hidden(obj, false);
}
/***
**   日期:2022-07-14 16:53:00
**   作者: leo.liu
**   函数作用：开锁图标1显示
**   参数说明:
***/
static void layout_monitor_lock_1_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_1_LOCK);
	if (obj == NULL)
	{
		return;
	}
	MON_CH ch = monitor_channel_get();
	if ((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2) || (ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(obj, true);
		return;
	}

	// if ((ch != MON_CH_DOOR2) || (user_data_get()->etc.door2_lock_num != 2))
	// {
	// 	lv_obj_set_hidden(obj, true);
	// }
	else
	{
		layout_monitor_fast_lock_1_display(obj);
		// if (user_data_get()->etc.open_the_door == 0)
		// {
		// 	layout_monitor_standard_lock_1_display(obj);
		// }
		// else
		// {
		// 	layout_monitor_fast_lock_1_display(obj);
		// }
	}
}
/***
**   日期:2022-07-14 16:53:24
**   作者: leo.liu
**   函数作用：开锁图标1按钮按下
**   参数说明:
***/
static void layout_monitor_lock_1_btn_up(lv_obj_t *obj)
{
	user_data_get()->monitor_alarm = 1;
	if (layout_monitor_unlock_enable == true)
	{
		return;
	}
	layout_monitor_unlock_enable = true;
	monitor_unlock_mask_set(MON_UNLOCK_TALK);

	layout_monitor_outdoor_unlock_icon_display();
	lv_layout_task_create(layout_monitor_unlock_icon_task, (user_data_get()->etc.door_1_open_time+0.5) * 1000, LV_TASK_PRIO_MID, NULL);
	MON_CH ch = monitor_channel_get();
	monitor_unlock_open(1, ch == MON_CH_DOOR1 ? MON_CH_DOOR1 : MON_CH_DOOR2);
	if (1)//(/*(layout_monitor_talk_state() == false) &&*/ (tuya_api_app_talk_check() == false))
	{
		
		#ifdef MAOTOUYING_DAN_IC
		ringplay_play_form_index(7, 100,   door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
		#else
		ringplay_play_form_index(12, 100,   door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
		#endif
	}
}
/***
**   日期:2022-07-14 16:54:46
**   作者: leo.liu
**   函数作用：室内机开锁按钮创建
**   参数说明:
***/
static void layout_monitor_lock_1_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_lock_1_btn_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_DOOR_002_PNG);
	layout_monitor_btn_base_create(parent, 324 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent), 100, 100, &click_data, &img1, NULL, LAYOUT_MONITOR_OBJ_ID_1_LOCK, true);
	layout_monitor_lock_1_btn_display();
}
// /***
// **   日期:2022-07-14 16:55:40
// **   作者: leo.liu
// **   函数作用：标准模式下锁2图标显示
// **   参数说明:
// ***/
// static void layout_monitor_standard_lock_2_display(lv_obj_t *obj)
// {
// 	lv_obj_set_hidden(obj, layout_monitor_talk_enable ? false : true);
// }
/***
**   日期:2022-07-14 16:55:57
**   作者: leo.liu
**   函数作用：快速模式下锁2显示
**   参数说明:
***/
static void layout_monitor_fast_lock_2_display(lv_obj_t *obj)
{
	lv_obj_set_hidden(obj, false);
}
/***
**   日期:2022-07-14 16:56:14
**   作者: leo.liu
**   函数作用：锁2按钮显示
**   参数说明:
***/
static void layout_monitor_lock_2_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_2_LOCK);
	if (obj == NULL)
	{
		return;
	}

	MON_CH ch = monitor_channel_get();
	if ((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2) || (ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(obj, true);
		return;
	}

	// if ((ch != MON_CH_DOOR2) || (user_data_get()->etc.door2_lock_num != 2))
	// {
	// 	lv_obj_set_hidden(obj, true);
	// }
	else
	{
		layout_monitor_fast_lock_2_display(obj);
		// if (user_data_get()->etc.open_the_door == 0)
		// {
		// 	layout_monitor_standard_lock_2_display(obj);
		// }
		// else
		// {
		// 	layout_monitor_fast_lock_2_display(obj);
		// }
	}
}
/***
**   日期:2022-07-14 16:56:30
**   作者: leo.liu
**   函数作用：锁2按钮事件函数
**   参数说明:
***/
static void layout_monitor_lock_2_btn_up(lv_obj_t *obj)
{
	user_data_get()->monitor_alarm = 1;
	MON_CH ch = monitor_channel_get();
	layout_monitor_unlock_enable = true;
	monitor_unlock_mask_set(MON_UNLOCK_TALK);

	layout_monitor_unlock_icon_display();
	lv_layout_task_create(layout_monitor_unlock_icon_task, (user_data_get()->etc.door_2_open_time+0.5) * 1000, LV_TASK_PRIO_MID, NULL);
	monitor_unlock_open(2, ch == MON_CH_DOOR1 ? MON_CH_DOOR1 : MON_CH_DOOR2);
#ifdef MAOTOUYING_DAN_IC
	if (/*(layout_monitor_talk_state() == false) &&*/ (tuya_api_app_talk_check() == false))
	{
		ringplay_play_form_index(7, 100, door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
	}
#endif
}
/***
**   日期:2022-07-14 16:56:53
**   作者: leo.liu
**   函数作用：锁2按钮创建
**   参数说明:
***/
static void layout_monitor_lock_2_btn_create(lv_obj_t *parent)
{
	// if (user_data_get()->etc.door2_lock_num != 2)
	// {
	// 	return;
	// }
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_lock_2_btn_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_5_DOOR2_LOCK2_OPEN_PNG);
	layout_monitor_btn_base_create(parent, 600 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent), 100, 100, &click_data, &img1, NULL, LAYOUT_MONITOR_OBJ_ID_2_LOCK, true);
	// layout_monitor_lock_1_btn_display();
	layout_monitor_lock_2_btn_display();
}
/***
**   日期:2022-07-14 16:57:06
**   作者: leo.liu
**   函数作用：照片抓拍任务
**   参数说明:
***/
static void layout_monitor_mjpeg_snap_task(lv_task_t *task_t)
{
	lv_obj_t *obj = (lv_obj_t *)task_t->user_data;
	int *count = (int *)obj->user_data;
	if (mjpeg_encode_status_get() == false)
	{
		lv_obj_set_state(obj, LV_STATE_DEFAULT);
		lv_task_del(task_t);
		return;
	}
	else if ((*count) > 20)
	{
		record_jpeg_close();
		lv_obj_set_state(obj, LV_STATE_DEFAULT);
		lv_task_del(task_t);
		return;
	}
	(*count)++;
}
/***
**   日期:2022-06-08 11:28:02
**   作者: leo.liu
**   函数作用：监控图纸视频处理
**   参数说明:
***/
static void layout_monitor_snap_process(int mode)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_SNAP);
	if (record_jpeg_start(mode) == true)
	{
		if (mode & (REC_MODE_AUTO | REC_MODE_MANUAL))
		{
			lv_obj_set_state(obj, LV_STATE_CHECKED);
			static int count = 0;
			count = 0;
			obj->user_data = &count;
			lv_layout_task_create(layout_monitor_mjpeg_snap_task, 100, LV_TASK_PRIO_MID, obj);
		}
	}
}
/***
**   日期:2022-07-14 16:57:20
**   作者: leo.liu
**   函数作用：抓拍按钮事件函数
**   参数说明:
***/
static void layout_monitor_snap_btn_up(lv_obj_t *obj)
{
	user_data_get()->monitor_alarm = 1;

	if (lv_obj_get_state(obj, LV_BTN_PART_MAIN) & LV_STATE_CHECKED)
	{
		return;
	}
	if (video_input_state_get() == false)
	{
		return;
	}
	layout_monitor_snap_process(REC_MODE_MANUAL);
	ringplay_play_stop();
}
/***
** 日期: 2022-05-16 16:56
** 作者: leo.liu
** 函数作用：抓拍按钮
** 返回参数说明：
***/
static void layout_monitor_snap_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_snap_btn_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_MONITOR_PHOTO_001_PNG);
	static rom_bin_info img2 = rom_bin_info_get(ROM_UI_MONITOR_PHOTO_002_PNG);
	layout_monitor_btn_base_create(parent, 896 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent) + 12, 80, 80, &click_data, &img1, &img2, LAYOUT_MONITOR_OBJ_ID_SNAP, true);
}
/***
**   日期:2022-07-14 16:57:38
**   作者: leo.liu
**   函数作用：监控按钮录制处理
**   参数说明:
***/
static void layout_monitor_record_process(int mode)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_REC);
	if (media_sdcard_insert_check() == false)
	{
		return;
	}
	if (ringplay_ing_check() == true)
	{
		ringplay_play_stop();
	}

	if ((video_record_status_get() == false) && (video_input_state_get() == true))
	{
		// user_data_get()->get_record_flag = 1;
		record_video_start(mode);
	}
	else
	{
		record_video_close();
	}

	if (video_record_status_get() == false)
	{
		lv_obj_set_state(obj, LV_STATE_DEFAULT);
	}
	else
	{
		lv_obj_set_state(obj, LV_STATE_CHECKED);
	}
}
void layout_monitor_record_leave_home(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
	if (parent != NULL)
	{

		lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_REC);
		lv_obj_set_state(obj, LV_STATE_CHECKED);
	}
}
/***
**   日期:2022-07-14 16:57:53
**   作者: leo.liu
**   函数作用：监控录制事件函数
**   参数说明:
***/
static void layout_montor_rec_btn_up(lv_obj_t *obj)
{
	user_data_get()->monitor_alarm = 1;
	// ringplay_play_stop();
	layout_monitor_record_process(REC_MODE_MANUAL);
}
/***
** 日期: 2022-05-16 16:56
** 作者: leo.liu
** 函数作用：录制按钮
** 返回参数说明：
***/
static void layout_montor_rec_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(layout_montor_rec_btn_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_MONITOR_VIDEO_001_PNG);
	static rom_bin_info img2 = rom_bin_info_get(ROM_UI_MONITOR_VIDEO_002_PNG);
	layout_monitor_btn_base_create(parent, 800 - lv_obj_get_x(parent), 465 - lv_obj_get_y(parent) + 12, 80, 80, &click_data, &img1, &img2, LAYOUT_MONITOR_OBJ_ID_REC, true);
}
/***
** 日期: 2022-05-16 17:36
** 作者: leo.liu
** 函数作用：开锁1图标显示
** 返回参数说明：
***/
static void layout_monitor_unlock_icon_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_UNLOCK_ICON);
	lv_obj_t *obj_2 = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_OUTDOOR_UNLOCK_ICON);
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT);
	if (layout_monitor_unlock_enable == true)
	{
		lv_obj_set_hidden(obj, false);
		lv_obj_set_hidden(obj_2, true);
		MON_CH ch = monitor_channel_get();
		layout_monitor_security_display((((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) && (layout_monitor_talk_enable == false)) ? true : false);
		lv_obj_set_hidden(cont, true);
		layout_monitor_refresh_2();
	}
	else
	{
		lv_obj_set_hidden(obj, true);
		layout_monitor_refresh_1();
		lv_obj_set_hidden(obj_2, true);
	}
}

static void layout_monitor_outdoor_unlock_icon_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_UNLOCK_ICON);
	lv_obj_t *obj_2 = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_OUTDOOR_UNLOCK_ICON);
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT);

	if (layout_monitor_unlock_enable == true)
	{
		lv_obj_set_hidden(obj, true);
		lv_obj_set_hidden(obj_2, false);
		lv_obj_set_hidden(cont, true);
		MON_CH ch = monitor_channel_get();
		layout_monitor_security_display((((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) && (layout_monitor_talk_enable == false)) ? true : false);

		layout_monitor_refresh_2();
	}
	else
	{
		lv_obj_set_hidden(obj, true);
		layout_monitor_refresh_1();
	}
}
/***
**   日期:2022-07-14 16:58:10
**   作者: leo.liu
**   函数作用：开锁图标显示
**   参数说明:
***/
static void layout_monitor_indoor_unlock_icon_create(void)
{
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_6_CALL_NOTICE_DOOR_PNG);
	lv_obj_t *obj = layout_monitor_btn_base_create(NULL, 436, 231, 120, 120, NULL, &img1, NULL, LAYOUT_MONITOR_OBJ_ID_UNLOCK_ICON, false);
	lv_obj_set_hidden(obj, true);
}

static void layout_monitor_outdoor_unlock_icon_create(void)
{
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_DOOR_001_PNG);
	lv_obj_t *obj = layout_monitor_btn_base_create(NULL, 436, 231, 120, 120, NULL, &img1, NULL, LAYOUT_MONITOR_OBJ_ID_OUTDOOR_UNLOCK_ICON, false);
	lv_obj_set_hidden(obj, true);
}
/***
**   日期:2022-07-14 16:58:19
**   作者: leo.liu
**   函数作用：创建正常的按钮显示容器
**   参数说明:
***/
static lv_obj_t *layout_monitor_normal_btn_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
	lv_obj_set_pos(cont, 38, 465);
	lv_obj_set_size(cont, 983, 105);
	return cont;
}
/***
** 日期: 2022-05-17 09:22
** 作者: leo.liu
** 函数作用：将隐藏其他容器，显示btn需要的容器
** 返回参数说明：
***/
static void layout_montior_normal_cont_visiable(void)
{
	// lv_obj_t *normal = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
	lv_obj_t *adj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT);
	//	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_SECURITY);
	// if ((adj)) /* && (lv_obj_get_hidden(normal) == true))*/
	// {

	// fb_gui_layer_rect_fill(0x00, lv_obj_get_x(normal), lv_obj_get_y(normal), lv_obj_get_width(normal), lv_obj_get_height(normal));
	// lv_obj_set_hidden(normal, false);
	lv_obj_set_hidden(adj, true);
	MON_CH ch = monitor_channel_get();
	layout_monitor_security_display((((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) && (layout_monitor_talk_enable == false)) ? true : false);
	layout_monitor_refresh_3();
}
// }

static void layout_monitor_screen_full_obj_up(lv_obj_t *obj)
{
	monitor_ring_task_kill();
	ringplay_play_stop();
	user_data_get()->monitor_alarm = 1;
	lv_obj_t *normal = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT);
	lv_obj_t *odj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT);
	// lv_obj_t *adj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_SECURITY);
	lv_obj_set_hidden(normal, false);
	lv_obj_set_hidden(odj, false);
	layout_monitor_security_display(false);

	MON_CH ch = monitor_channel_get();
	layout_monitor_volume_bar_display(((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) ? true : false);
	// layout_monitor_brightness_display(true);
	// layout_monitor_contrast_display(true);
	// layout_monitor_color_display(true);
	layout_monitor_refresh_5();
}

static void layout_monitor_screen_full_obj_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 0, 82);
	lv_obj_set_size(obj, 1024, 600);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_screen_full_obj_up);
	obj_click_event_listen(obj, &click_data);
}

static void layout_alarm_alarm_ring_start(int index)
{
	/***** 开启功放 *****/
	tuya_spk_sw_pin_enable(false);
	MON_CH ch = monitor_channel_get();
	call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, true);
}
static void layout_alarm_alarm_ring_end(int index)
{
	/***** 关闭功放 *****/
	// LOG_RED("AMP 75 ctrl close\n");
	power_amplifier_enable(false);
	MON_CH ch = monitor_channel_get();
	call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, false);
}

/***
** 日期: 2022-05-06 13:58
** 作者: leo.liu
** 函数作用：进度条调整
** 返回参数说明：flg:0x01 声音，flg = 0x02 亮度，flg 0x03对比度  flg 0x04色度
***/
void layout_monitor_adj_bar_base_create(lv_obj_t *parent, int x, int h, int y, int max, obj_click_data *left_click, obj_click_data *right_click, int bar_id, int vol, char flg)
{

	MON_CH ch = monitor_channel_get();
	/***** bar *****/
	lv_obj_t *bar = lv_bar_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_id(bar, bar_id);
	lv_obj_set_pos(bar, x, y);
	lv_obj_set_size(bar, 8, h);
	lv_bar_set_range(bar, 0, max);
	lv_bar_set_value(bar, vol, false);

	lv_obj_set_style_local_bg_opa(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x666666));
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x0096ff));
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(bar,true);
	}

	/***** btn *****/
	lv_obj_t *add_obj = lv_obj_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_pos(add_obj, x - 25, y - 55);
	lv_obj_set_size(add_obj, 54, 54);
	static rom_bin_info right_img = rom_bin_info_get(ROM_UI_VOL_ADD_PNG);
	lv_obj_set_style_local_pattern_image(add_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &right_img);
	obj_click_event_listen(add_obj, left_click);
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(add_obj,true);
	}

	lv_obj_t *sub_obj = lv_obj_create(parent ? parent : lv_scr_act(), add_obj);
	lv_obj_set_y(sub_obj, y + h);
	static rom_bin_info left_img = rom_bin_info_get(ROM_UI_VOL_SUB_PNG);
	lv_obj_set_style_local_pattern_image(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &left_img);
	obj_click_event_listen(sub_obj, right_click);
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(sub_obj,true);
	}

	/***** label *****/
	lv_obj_t *label = lv_label_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_id(label, bar_id + 1);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	lv_label_set_text_fmt(label, "%02d", vol);
	lv_obj_align(label, add_obj, LV_ALIGN_IN_LEFT_MID, 15, y + h + 30);
	lv_obj_set_auto_realign(label, true);
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(label,true);
	}

	/*****  icon *****/
	lv_obj_t *icon = lv_obj_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_click(icon, false);
	lv_obj_set_size(icon, 54, 54);
	lv_obj_align(icon, add_obj, LV_ALIGN_IN_LEFT_MID, 0, 320);
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(icon,true);
	}

	if (flg)
	{
		static rom_bin_info vol_img = rom_bin_info_get(ROM_UI_MONITOR_VOL_PNG);
		static rom_bin_info bri_img = rom_bin_info_get(ROM_UI_MONITOR_BRIGHTNESS_PNG);
		static rom_bin_info cont_img = rom_bin_info_get(ROM_UI_MONITOR_CONTRAST_PNG);
		static rom_bin_info color_img = rom_bin_info_get(ROM_UI_MONITOR_COLOR_PNG);
		lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, flg == 0x01 ? &vol_img : flg == 0x02 ? &bri_img
																											: flg == 0x03	? &cont_img
																															: &color_img);
	}
	
}

void layout_monitor_adj_volume_bar_base_create(lv_obj_t *parent, int x, int h, int y, int max, obj_click_data *left_click, obj_click_data *right_click, int bar_id, int vol, char flg)
{
	/***** bar *****/
	lv_obj_t *bar = lv_bar_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_id(bar, bar_id);
	lv_obj_set_pos(bar, x, y);
	lv_obj_set_size(bar, 8, h);
	lv_bar_set_range(bar, 0, max);
	lv_bar_set_value(bar, vol, false);

	lv_obj_set_style_local_bg_opa(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x666666));
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x0096ff));

	/***** btn *****/
	lv_obj_t *add_obj = lv_obj_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_pos(add_obj, x - 25, y - 55);
	lv_obj_set_size(add_obj, 54, 54);
	static rom_bin_info right_img = rom_bin_info_get(ROM_UI_VOL_ADD_PNG);
	lv_obj_set_style_local_pattern_image(add_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &right_img);
	obj_click_event_listen(add_obj, left_click);

	lv_obj_t *sub_obj = lv_obj_create(parent ? parent : lv_scr_act(), add_obj);
	lv_obj_set_y(sub_obj, y + h);
	static rom_bin_info left_img = rom_bin_info_get(ROM_UI_VOL_SUB_PNG);
	lv_obj_set_style_local_pattern_image(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &left_img);
	obj_click_event_listen(sub_obj, right_click);

	/***** label *****/
	lv_obj_t *label = lv_label_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_id(label, bar_id + 1);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	lv_label_set_text_fmt(label, "%d", vol);
	lv_obj_align(label, add_obj, LV_ALIGN_CENTER, 0, y + h + 30);
	lv_obj_set_auto_realign(label, true);

	/*****  icon *****/
	lv_obj_t *icon = lv_obj_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_click(icon, false);
	lv_obj_set_size(icon, 54, 54);
	lv_obj_align(icon, add_obj, LV_ALIGN_IN_LEFT_MID, 0, 320);

	if (flg)
	{
		static rom_bin_info vol_img = rom_bin_info_get(ROM_UI_MONITOR_VOL_PNG);
		static rom_bin_info bri_img = rom_bin_info_get(ROM_UI_MONITOR_BRIGHTNESS_PNG);
		static rom_bin_info cont_img = rom_bin_info_get(ROM_UI_MONITOR_CONTRAST_PNG);
		static rom_bin_info color_img = rom_bin_info_get(ROM_UI_MONITOR_COLOR_PNG);
		lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, flg == 0x01 ? &vol_img : flg == 0x02 ? &bri_img
																											: flg == 0x03	? &cont_img
																															: &color_img);
	}
}
/***
** 日期: 2022-05-17 08:06
** 作者: leo.liu
** 函数作用：创建亮度进图条
** 返回参数说明：
***/
static void layout_monitor_brightness_bar_left_up(lv_obj_t *obj)
{

	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_BRIGHTNESS_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS + 1);

	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol > 0)
		{
			vol--;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_brightness_vol_set(vol);
			display_bright_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:00:26
**   作者: leo.liu
**   函数作用：亮度进度条，右边按钮事件函数
**   参数说明:
***/
static void layout_monitor_brightness_bar_right_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_BRIGHTNESS_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol < 20)
		{
			vol++;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_brightness_vol_set(vol);
			display_bright_adj(vol, INVALID_FORMAT);
		}
	}
}

/***
**   日期:2022-07-14 17:00:49
**   作者: leo.liu
**   函数作用：亮度进度条创建
**   参数说明:
***/
static void layout_monitor_brightness_bar_create(lv_obj_t *parent)
{
	static obj_click_data left = obj_click_data_up_create(layout_monitor_brightness_bar_left_up);
	static obj_click_data right = obj_click_data_up_create(layout_monitor_brightness_bar_right_up);
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_BRIGHTNESS_CONT_CONT);
	layout_monitor_adj_bar_base_create(obj, 21, 200, 49, 20, &right, &left, LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS, monitor_display_brightness_vol_get(), 0x02);
}
/***
** 日期: 2022-05-17 08:06
** 作者: leo.liu
** 函数作用：创建对比度进图条
** 返回参数说明：
***/
static void layout_monitor_cont_bar_left_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_CONTRAST_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_CONST);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_CONST + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol > 0)
		{
			vol--;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_cont_vol_set(vol);
			display_const_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:01:21
**   作者: leo.liu
**   函数作用：对比度容器，右边按钮创建
**   参数说明:
***/
static void layout_monitor_cont_bar_right_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_CONTRAST_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_CONST);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_CONST + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol < 20)
		{
			vol++;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_cont_vol_set(vol);
			display_const_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:01:52
**   作者: leo.liu
**   函数作用：对比度进度条创建
**   参数说明:
***/
static void layout_monitor_cont_bar_create(lv_obj_t *parent)
{
	static obj_click_data left = obj_click_data_up_create(layout_monitor_cont_bar_left_up);
	static obj_click_data right = obj_click_data_up_create(layout_monitor_cont_bar_right_up);
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_CONTRAST_CONT_CONT);
	layout_monitor_adj_bar_base_create(obj, 21, 200, 49, 20, &right, &left, LAYOUT_MONITOR_OBJ_ID_ADJ_CONST, monitor_display_cont_vol_get(), 0x03);
}
/***
** 日期: 2022-05-17 08:06
** 作者: leo.liu
** 函数作用：创建色度进图条
** 返回参数说明：
***/
static void layout_monitor_color_bar_left_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_COLOR_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR + 1);
	if (bar && label)
	{
		printf("jfhsifhseif\n");
		int vol = lv_bar_get_value(bar);
		if (vol > 0)
		{
			vol--;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_color_vol_set(vol);
			display_color_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:02:06
**   作者: leo.liu
**   函数作用：色彩进度条，右边点击函数
**   参数说明:
***/
static void layout_monitor_color_bar_right_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_COLOR_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol < 20)
		{
			vol++;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_color_vol_set(vol);
			display_color_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:03:03
**   作者: leo.liu
**   函数作用：色彩进度条创建
**   参数说明:
***/
static void layout_monitor_color_bar_create(lv_obj_t *parent)
{
	static obj_click_data left = obj_click_data_up_create(layout_monitor_color_bar_left_up);
	static obj_click_data right = obj_click_data_up_create(layout_monitor_color_bar_right_up);
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_COLOR_CONT_CONT);
	layout_monitor_adj_bar_base_create(obj, 21, 200, 49, 20, &right, &left, LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR, monitor_display_color_vol_get(), 0x04);
}

/***
** 日期: 2022-05-17 09:30
** 作者: leo.liu
** 函数作用：音量调整容器创建
** 返回参数说明：
***/
// static void layout_monitor_vol_cancel_btn_up(lv_obj_t *obj)
// {
// 	layout_montior_normal_cont_visiable();
// }
// /***
// **   日期:2022-07-14 17:03:40
// **   作者: leo.liu
// **   函数作用：音量进度条容器创建
// **   参数说明:
// ***/
// static lv_obj_t *layout_monitor_vol_btn_cont_create(lv_obj_t *parent)
// {
// 	lv_obj_t *cont = lv_cont_create(parent, NULL);
// 	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_VOL_CONT);
// 	lv_obj_set_pos(cont, 0, 0);
// 	lv_obj_set_size(cont, 1024, 600);
// 	lv_obj_set_hidden(cont, true);

// 	lv_obj_t *obj = lv_obj_create(cont, NULL);
// 	lv_obj_set_pos(obj, 0, 0);
// 	lv_obj_set_size(obj, 1024, 600 - 90);
// 	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
// 	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_vol_cancel_btn_up);
// 	obj_click_event_listen(obj, &click_data);

// 	cont = lv_cont_create(cont, NULL);
// 	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_VOL_CONT_CONT);
// 	lv_obj_set_pos(cont, 43, 101);
// 	lv_obj_set_size(cont, 36, 361);
// 	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
// 	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));

// 	return cont;
// }
/***
**   日期:2022-07-14 17:03:57
**   作者: leo.liu
**   函数作用：音量进度条左边按钮点击函数
**   参数说明:
***/
static void layout_monitor_volume_bar_left_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_VOL_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_VOL_VOL);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_VOL_VOL + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		// if ((layout_monitor_talk_enable == true))
		// {
		if (vol > 1)
		{
			vol--;
			user_data_get()->audio.door_talk_volume = vol;

			talk_output_gpio_ctrl_volume(vol);
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);
			user_data_save();
		}
		// }
		// else if (vol > 0)
		// {
		// 	vol--;
		// 	user_data_get()->ring_attr[0][0].ring_val = vol;
		// 	user_data_save();
		// 	lv_bar_set_value(bar, vol, true);
		// 	lv_label_set_text_fmt(label, "%d", vol);
		// 	if (vol == 0)
		// 	{
		// 		layout_monitor_mute_icon_display();
		// 	}
		// }
	}
}
/***
**   日期:2022-07-14 17:04:17
**   作者: leo.liu
**   函数作用：音量进度条右边按钮点击函数
**   参数说明:
***/
static void layout_monitor_volume_bar_right_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_VOL_CONT_CONT);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_VOL_VOL);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_VOL_VOL + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol < 4)
		{
			vol++;
			// if (layout_monitor_talk_enable == true)
			// {
			user_data_get()->audio.door_talk_volume = vol;
			talk_output_gpio_ctrl_volume(vol);
			// }
			// else
			// {
			// 	user_data_get()->audio.door_ring_volume = vol;
			// }

			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);
			user_data_save();
			if (vol == 0)
			{
				layout_monitor_mute_icon_display();
			}
		}
	}
}
/***
**   日期:2022-07-14 17:04:33
**   作者: leo.liu
**   函数作用：音量进度条创建
**   参数说明:
***/
static void layout_monitor_volume_bar_create(lv_obj_t *parent)
{
	static obj_click_data left = obj_click_data_up_create(layout_monitor_volume_bar_left_up);
	static obj_click_data right = obj_click_data_up_create(layout_monitor_volume_bar_right_up);

	int vol = user_data_get()->audio.door_talk_volume;
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_VOL_CONT_CONT);
	layout_monitor_adj_volume_bar_base_create(obj, 21, 200, 49, 4, &right, &left, LAYOUT_MONITOR_OBJ_ID_VOL_VOL, vol, 0x01);
}
static void layout_monitor_volume_bar_display(bool disp)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT);
	if (obj == NULL)
	{
		printf("not find adj cont \n");
		return;
	}
	obj = lv_obj_get_child_form_id(obj, LAYOUT_MONITOR_OBJ_ID_VOL_CONT_CONT);
	if (obj == NULL)
	{
		printf("not find monitor volume bar \n");
		return;
	}
	lv_obj_set_hidden(obj, disp == true ? false : true);
}
static void layout_monitor_adj_cancel_btn_up(lv_obj_t *obj)
{
	layout_montior_normal_cont_visiable();
}
/***
**   日期:2022-07-14 16:59:04
**   作者: leo.liu
**   函数作用：调整按钮容器创建
**   参数说明:
***/
static lv_obj_t *layout_monitor_adj_btn_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_ADJ_CONT);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 600 - 125);
	lv_obj_set_hidden(cont, true);

	lv_obj_t *obj = lv_obj_create(cont, NULL);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 1024, 600 - 125);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_adj_cancel_btn_up);
	obj_click_event_listen(obj, &click_data);
	return cont;
}

static lv_obj_t *layout_monitor_brightness_btn_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_BRIGHTNESS_CONT_CONT);
	lv_obj_set_pos(cont, 953, 101);
	lv_obj_set_size(cont, 50, 361);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));
	lv_obj_set_style_local_radius(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 22);

	MON_CH ch = monitor_channel_get();
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(cont,true);
	}

	// layout_monitor_brightness_bar_create(cont);
	return cont;
}

static lv_obj_t *layout_monitor_contrast_btn_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_CONTRAST_CONT_CONT);
	lv_obj_set_pos(cont, 797, 101);
	lv_obj_set_size(cont, 50, 361);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));
	lv_obj_set_style_local_radius(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 22);

	// layout_monitor_cont_bar_create(cont);
	MON_CH ch = monitor_channel_get();
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(cont,true);
	}
	return cont;
}

static lv_obj_t *layout_monitor_color_btn_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_COLOR_CONT_CONT);
	lv_obj_set_pos(cont, 875, 101);
	lv_obj_set_size(cont, 50, 361);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));
	lv_obj_set_style_local_radius(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 22);

	// layout_monitor_color_bar_create(cont);
	MON_CH ch = monitor_channel_get();
	if((ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4))
	{
		lv_obj_set_hidden(cont,true);
	}
	return cont;
}

static lv_obj_t *layout_monitor_vol_btn_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, LAYOUT_MONITOR_OBJ_ID_VOL_CONT_CONT);
	lv_obj_set_pos(cont, 43, 101);
	lv_obj_set_size(cont, 50, 361);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));
	lv_obj_set_style_local_radius(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 22);

	// layout_monitor_volume_bar_create(cont);
	return cont;
}

/***
** 日期: 2022-04-28 09:53
** 作者: leo.liu
** 函数作用：监控被按下执行的回调函数
** 返回参数说明：
***/
void layout_monitor_click_down_func(lv_obj_t *obj)
{
	if ((user_data_get()->home_mode == 1) || (layout_monitor_talk_state() == true))
	{
		return;
	}
	printf("#############monitor_layout_keysound\n");
	//***** 控制声音流向 *****/
	touch_sound_play(ringplay_keysound_start_default_func, NULL);
}
/***
**   日期:2022-06-08 11:09:15
**   作者: leo.liu
**   函数作用：call机参考任务
**   参数说明:
***/
static void monitor_call_record_delay_task(lv_task_t *task)
{

	if (ringplay_ing_check() == true)
	{
		return;
	}

	if (video_input_state_get() == false)
	{
		return;
	}

	int mode = REC_MODE_NONE;
	if ((layout_monitor_talk_state() == false) && (monitor_unlock_mask_get() == MON_UNLOCK_CALL) && (user_data_get()->etc.tuya_send_switch == false))
	{
		// if((user_data_get()->etc.tuya_send_switch == false))
		// {
		// printf("##########AAAAAAAAAAAAAAAAAAAAAA\n");
		mode = REC_MODE_TUYA_CALL;
		// }
	}

	layout_monitor_call_record_task = NULL;
	if ((user_data_get()->auto_record_mode != 0) || (user_data_get()->home_mode == 0))
	{
		if ((media_sdcard_insert_check() == true) && ((user_data_get()->auto_record_mode == 2) || (user_data_get()->home_mode == 0)))
		{
			if (video_record_status_get() == false)
			{
				layout_monitor_record_process(REC_MODE_AUTO);
			}
			//	lv_task_del(task);
			// return;
		}
		else
		{
			// printf("media_sdcard_insert_is_false");
			mode |= REC_MODE_AUTO;
		}
	}
	if ((mjpeg_encode_status_get() == true) && (mode == REC_MODE_AUTO))
	{
		return;
	}
	layout_monitor_snap_process(mode);

	lv_task_del(task);
}
/***
**   日期:2022-06-08 11:09:15
**   作者: fei.liu
**   函数作用：tuya推送参考任务
**   参数说明:
***/
#if 1
static int monitor_tuya_timer_count = 0;
static void monitor_tuya_send_delay_task(lv_task_t *task)
{
	if ((user_data_get()->etc.tuya_send_switch) == false)
	{
		// printf("========>>>%d=======\n",__LINE__);
		return;
	}
	monitor_tuya_timer_count++;
	if (monitor_tuya_timer_count > 10)
	{
		goto finish;
	}
	if (video_input_state_get() == false)
	{
		lv_task_set_period(task, 500);
		lv_task_reset(task);
		return;
	}

	int mode = REC_MODE_NONE;
	if ((layout_monitor_talk_state() == false) && (monitor_unlock_mask_get() == MON_UNLOCK_CALL) /*&& ((user_data_get()->etc.tuya_send_switch) == true)*/)
	{
		printf("========>>>%d=======\n", __LINE__);
		mode = REC_MODE_TUYA_CALL;
		layout_monitor_snap_process(mode);
	}

finish:
	lv_task_del(task);
	monitor_tuya_timer_count = 0;
}
#endif
/***
**   日期:2022-07-12 11:37:45
**   作者: leo.liu
**   函数作用：intercom事件处理
**   参数说明:
***/
// static bool layout_monitor_intercom_func(unsigned int arg1, unsigned int arg2)
// {
// 	GPIO_LEVEL leve = (GPIO_LEVEL)arg1;
// 	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_INTER);
// 	lv_task_t *task = (lv_task_t *)obj->user_data;
// 	if (leve == GPIO_LEVEL_LOW)
// 	{
// 		ringplay_play_form_index(9, 100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, true);
// 		lv_task_set_cb(task, layout_monitor_intercom_icon_blink_task);
// 	}
// 	else
// 	{
// 		ringplay_play_stop();
// 		lv_task_set_cb(task, NULL);
// 		lv_obj_set_hidden(obj, false);
// 	}
// 	return true;
// }
/***
**   日期:2022-06-08 09:14:25
**   作者: leo.liu
**   函数作用：tuya文本提示
**   参数说明:
***/
static void monitor_tuya_lable_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_id(label, LAYOUT_MONITOR_OBJ_ID_TUYA);
	lv_obj_set_style_local_bg_opa(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
	lv_obj_set_style_local_radius(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
	lv_obj_set_style_local_transform_width(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 20);
	lv_obj_set_style_local_transform_height(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 8);
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 80);
	lv_obj_set_auto_realign(label, true);
	lv_obj_set_hidden(label, true);
}
/***
**   日期:2022-07-13 16:58:03
**   作者: leo.liu
**   函数作用：监控判断tuya在线
**   参数说明:
***/
static void monitor_tuya_app_check_task(lv_task_t *task_t)
{
	return;
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_TUYA);
	bool talk = tuya_api_app_talk_check();
	int num = tuya_online_num_get_customize();
	if ((num == 0) && (lv_obj_get_hidden(obj) == false))
	{
		lv_obj_set_hidden(obj, true);
		lv_label_set_text(obj, "");
	}
	else if (num > 0)
	{
		if ((talk == true) && (strcmp(lv_label_get_text(obj), layout_home_string_get(HOME_LANG_ID_TUYA_TALK))))
		{
			lv_label_set_text(obj, layout_home_string_get(HOME_LANG_ID_TUYA_TALK));
			lv_obj_set_hidden(obj, false);
		}
		else if ((talk == false) && (strcmp(lv_label_get_text(obj), layout_home_string_get(HOME_LANG_ID_TUYA_PREVIEW))))
		{
			lv_label_set_text(obj, layout_home_string_get(HOME_LANG_ID_TUYA_PREVIEW));
			lv_obj_set_hidden(obj, false);
		}
	}
}

static void layout_monitor_secueity_craete_up(lv_obj_t *obj)
{
	if (user_data_get()->monitor_alarm == 1)
	{
		if ((layout_monitor_talk_state() == false) && (tuya_api_app_talk_check() == false))
		{
			ringplay_play_form_index(8, 100, layout_alarm_alarm_ring_start, layout_alarm_alarm_ring_end, true);
		}
		user_data_get()->monitor_alarm = 0;
	}
	else
	{
		ringplay_play_stop();
		user_data_get()->monitor_alarm = 1;
	}
}

/***
** 日期: 2022-05-06 13:58
** 作者: leo.liu
** 函数作用：警报按钮的创建
** 返回参数说明：
***/
static void layout_monitor_security_create()
{
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_MONITOR_SECURITY_PNG);
	lv_obj_t *obj = layout_monitor_btn_base_create(NULL, 909, 105, 80, 80, NULL, &img1, NULL, LAYOUT_MONITOR_OBJ_ID_SECURITY, true);
	static obj_click_data click_data = obj_click_data_up_create(layout_monitor_secueity_craete_up);
	obj_click_event_listen(obj, &click_data);
}

static void layout_monitor_security_display(bool display)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_SECURITY);
	if (obj == NULL)
	{
		printf("not find security obj id \n");
		return;
	}

	lv_obj_set_hidden(obj, display ? false : true);
}
/***
**   日期:2022-07-14 17:04:56
**   作者: leo.liu
**   函数作用：进入监控页面处理的第一个函数
**   参数说明:
***/

static void LAYOUT_ENTER_FUNC(monitor)
{
	layout_monitor_refresh_1();

	/***** 通话状态置位 *****/
	layout_monitor_talk_enable = false;
	layout_monitor_unlock_enable = false;

	standby_timer_close();

	layout_monitor_head_cont_create();

	layout_monitor_screen_full_obj_create();
	layout_monitor_security_create();
	MON_CH ch = monitor_channel_get();

	layout_monitor_security_display(((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) ? true : false);
	layout_monitor_volume_bar_display(((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2)) ? true : false);
	lv_obj_t *cont = layout_monitor_normal_btn_cont_create();

	layout_monitor_door_swap_btn_create(cont);
	layout_monitor_cctv_swap_btn_create(cont);

	layout_monitor_talk_btn_create(cont);
	layout_monitor_cancel_btn_create(cont);
	// layout_monitor_lock_normal_btn_create(cont);
	layout_monitor_lock_1_btn_create(cont);
	layout_monitor_lock_2_btn_create(cont);

	layout_monitor_snap_btn_create(cont);
	layout_montor_rec_btn_create(cont);

	layout_monitor_indoor_unlock_icon_create();
	layout_monitor_outdoor_unlock_icon_create();

	cont = layout_monitor_adj_btn_cont_create();
	layout_monitor_brightness_btn_cont_create(cont);
	layout_monitor_contrast_btn_cont_create(cont);
	layout_monitor_color_btn_cont_create(cont);
	layout_monitor_vol_btn_cont_create(cont);

	layout_monitor_brightness_bar_create(cont);
	layout_monitor_cont_bar_create(cont);
	layout_monitor_color_bar_create(cont);
	layout_monitor_volume_bar_create(cont);
	/***** 开启监控 *****/
	monitor_open(true, 0x03);

	layout_door1_call_callback_register(layout_monitor_door1_call_func);
	layout_door2_call_callback_register(layout_monitor_door2_call_func);
	layout_sd_state_callback_register(layout_monitor_sd_icon_display);
	lv_obj_click_down_callback_register(layout_monitor_click_down_func);

	layout_call_talk_btn_callback_register(layout_monitor_call_talk_btn_func);
	// layout_unclock_btn_callback_register(layout_unclock_default);

	// layout_intercom_event_callback_register(layout_monitor_intercom_func);
	monitor_record_pin_enable(true);

	audio_input_capture_enable(true);
	h264_encode_capture_enable(true);
	printf("====monitor_unlock_mask_get=%d\n", monitor_unlock_mask_get());
	if (monitor_enter_mask_get() == MON_ENTER_CALL)
	{
		video_display_preview_enable(false);
		usleep(1200 * 1000);
		layout_monitor_call_record_task = lv_layout_task_create(monitor_call_record_delay_task, 100, LV_TASK_PRIO_MID, NULL);
		lv_task_reset(layout_monitor_call_record_task);
		lv_layout_task_create(monitor_tuya_send_delay_task, user_data_get()->etc.tuya_send_time * 1000, LV_TASK_PRIO_MID, NULL);
		video_display_preview_enable(true);
		app_playback_record(true);
	}

	//	h264_encode_sample_enable(false);

	monitor_tuya_lable_create();
	lv_layout_task_create(monitor_tuya_app_check_task, 1500, LV_TASK_PRIO_MID, NULL);

	if (tuya_online_num_get_customize() > 0)
	{

		tuya_app_preview_enable(true);
	}
}
/***
**   日期:2022-07-14 17:05:13
**   作者: leo.liu
**   函数作用：离开监控页面函数处理
**   参数说明:
***/
static void LAYOUT_QUIT_FUNC(monitor)
{
	tuya_api_preview_quit();

	app_playback_record(false);

	// /*****退出样品流 *****/
	// h264_encode_sample_enable(false);

	layout_door1_call_callback_register(layout_door1_call_default);
	layout_door2_call_callback_register(layout_door2_call_default);
	lv_obj_click_down_callback_register(layout_obj_click_down_func);
	// layout_intercom_event_callback_register(layout_intercom_trigger_default);
	layout_sd_state_callback_register(NULL);

	record_jpeg_close();
	record_video_close();
	standby_timer_restart(true);
	monitor_unlcok_close();
	video_display_preview_enable(false);
	video_input_skip_frame_count_set(1000);
	fb_gui_layer_rect_fill(0x0, 0, 0, 1024, 600);
	screen_force_refresh();

	monitor_ring_task_kill();

	monitor_unlock_mask_set(MON_UNLOCK_NONE);
	sleep_mode_enter_moniter_flag = false;

	layout_monitor_call_record_task = NULL;
	// layout_monitor_talk_enable = false;

	if (ringplay_ing_check() == true)
	{
		ringplay_play_stop();
		// monitor_ring_task_kill();
	}
	if (tuya_online_num_get_customize() <= 0)
	{
		audio_input_capture_enable(false);
		h264_encode_capture_enable(false);
		MON_ENTER_FLG flg = monitor_enter_mask_get();
		if ((flg == MON_ENTER_MANUAL_DOOR) || (flg == MON_ENTER_MANUAL_CCTV))
		{
			door_audio_talk(AUDIO_CH_CLOSE);
			monitor_close();

			intercom_data_busy_enbale(false);
			monitor_enter_mask_set(MON_ENTER_NONE);
			LOG_GREEN("set release 5\n");
			intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
		}
		else if (flg == MON_ENTER_CALL)
		{
			MON_CH ch = monitor_channel_get();
			door_audio_talk(AUDIO_CH_CLOSE);
			intercom_audio_enable((ch == MON_CH_DOOR1) ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2);
			if (OwnID != 1)
			{
				intercom_audio_enable((ch == MON_CH_DOOR1) ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2);
			}
			intercom_data_busy_enbale(true);

			if (OwnID == 1)
			{
				unsigned int mask = layout_intercom_call_mask_get();
				printf("enter call quit mask:%x \n", mask);
				mask &= 0x0E;
				layout_intercom_call_mask_set(mask);
				if ((mask & 0x0F) == 0x00)
				{
					monitor_close();
					monitor_enter_mask_set(MON_ENTER_NONE);
					LOG_GREEN("set release 6\n");
					intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
				}
				else
				{
					// printf("CMD_DOOR1_CALL_QUIT \n");
					intercom_busy_ack_mask_set(0x00);
					intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY_ACK);
					video_input_close();
					video_switch_enable(monitor_channel_get(), 0x02);
				}
			}
			else
			{
				// printf("CMD_DOOR2_CALL_QUIT \n");
				intercom_busy_ack_mask_set(0x00);
				intercom_cmd_send(1, CMD_DOOR_CALL_QUIT);

				intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY_ACK);
				// monitor_close();
				video_input_close();
				video_switch_enable(monitor_channel_get(), 0x02);
				// video_switch_enable(monitor_channel_get(), 0x03);
				
			}
		}
		else if (flg == MON_ENTER_TALK)
		{
			intercom_data_busy_enbale(true);
			if (layout_monitor_talk_state() == true)
			{
				monitor_close();
				door_audio_talk(AUDIO_CH_CLOSE);
				monitor_enter_mask_set(MON_ENTER_NONE);
				LOG_GREEN("set release 7\n");
				intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
			}
			else
			{
				// monitor_open(false, 0x02);
				intercom_talk_to_outdoor();
				video_input_close();
				video_switch_enable(monitor_channel_get(), 0x02);
			}
		}
		else
		{
			LOG_WHITE("exit uknown flg[%d]\n", flg);
		}
	}
	else
	{
		if (OwnID == 1)
		{
			unsigned int mask = layout_intercom_call_mask_get();
			printf("enter call quit mask:%x \n", mask);
			mask &= 0x0E;

			layout_intercom_call_mask_set(mask);

			MON_ENTER_FLG flg = monitor_enter_mask_get();
			if (flg == MON_ENTER_TALK)
			{
				intercom_data_busy_enbale(true);
				if (layout_monitor_talk_state() == true)
				{
					monitor_close();
					door_audio_talk(AUDIO_CH_CLOSE);
					monitor_enter_mask_set(MON_ENTER_NONE);
					// LOG_GREEN("set release 7\n");
					intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
				}
			}
			else if (mask == 0)
			{
				monitor_enter_mask_set(MON_ENTER_NONE);
			}
			/*手机app在忙线状态点通话按键，占用通话资源的室内机退出来，打开涂鸦通话通道
			if(tuya_api_app_talk_check())
			{
				tuya_spk_sw_pin_enable(true);
				tuya_mic_sw_pin_enable(true);
				ring_to_outdoor_mute_pin_ctrl(true);
			}*/
			extern void il34118_enable_pin_ctrl(bool en);
			il34118_enable_pin_ctrl(false);
		}
	}
	layout_monitor_talk_enable = false;
	// monitor_close();
	// door_audio_talk(AUDIO_CH_CLOSE);
	// monitor_enter_mask_set(MON_ENTER_NONE);
	layout_call_talk_btn_callback_register(NULL);
	layout_unclock_btn_callback_register(NULL);

	tuya_doorbell_timestamp_set(0);
}
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声开始
** 返回参数说明：
***/
static void layout_monitor_ringplay_doorcall_start_func(int index)
{
	if (user_data_get()->home_mode == 1)
	{
		sleep_mode_enter_moniter_flag = true;
	}
	if (user_data_get()->ring_attr[door_z][ring_z].ring_val == 0)
	{
		sleep_mode_enter_moniter_flag = true;
	}

	MON_CH ch = monitor_channel_get();
	if (OwnID == 1)
	{
		call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, true);
	}
	else
	{
		LOG_GREEN("intercom call rings mixs");
		call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, false);
	}
}
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声结束
** 返回参数说明：
***/
static void layout_monitor_ringplay_doorcall_finish_func(int index)
{
	/***** 开启功放 *****/
	// power_amplifier_enable(false);
	sleep_mode_enter_moniter_flag = false;
	call_ring_to_outdoor_ctrl(AUDIO_CH_DOOR1, false);
}
/***
**   日期:2022-06-15 17:11:41
**   作者: leo.liu
**   函数作用：重置监控时间
**   参数说明:
***/
static void layout_montior_timeout_reset(int timeout)
{
	LOG_WHITE("timeout reset display \n");
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_HEAD_CONT);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_HEAD_TIMEOUT);
	int *ptimeout = (int *)obj->user_data;
	*ptimeout = timeout;
}
/***
** 日期: 2022-05-16 17:30
** 作者: leo.liu
** 函数作用：监控页内door1_call
** 返回参数说明：
***/
static void layout_monitor_door1_call_func(void)
{
	app_playback_record(true);
	monitor_unlock_mask_set(MON_UNLOCK_CALL);
	ringplay_play_stop();
	monitor_valid_channel_set(MON_CH_DOOR1, true);

	if (OwnID == 1)
	{
		if ((monitor_enter_mask_get() == MON_ENTER_MANUAL_DOOR) || (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV))
		{
			h264_encode_sample_enable(false);
			intercom_cmd_send(CODE_ALL_ID, CMD_DOOR1_CALL);
		}
		else if (monitor_enter_mask_get() == MON_ENTER_CALL)
		{
			h264_encode_sample_enable(false);
			intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR1_CALL);
		}
	}

	MON_CH ch = monitor_channel_get();
	monitor_channel_set(MON_CH_DOOR1);
	adj_value_refresh_display();
	monitor_enter_mask_set(layout_monitor_talk_enable == false ? MON_ENTER_CALL : MON_ENTER_TALK);

	if (ch != MON_CH_DOOR1)
	{
		video_record_stop();
		lv_obj_t *obj_1 = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_REC);
		lv_obj_set_state(obj_1, LV_STATE_DEFAULT);

		layout_montior_normal_cont_visiable();

		layout_monitor_door_swap_btn_display();
		layout_monitor_cctv_swap_btn_display();
		layout_monitor_channel_label_display();
		layout_monitor_talk_btn_display();
		// layout_monitor_cancel_btn_display();
		//  layout_monitor_lock_normal_btn_display();
		layout_monitor_lock_1_btn_display();
		layout_monitor_lock_2_btn_display();

		//		layout_monitor_cctv_cancel_display();
		layout_monitor_time_label_display(NULL);

		layout_monitor_security_display((layout_monitor_talk_enable == true) ? false : true);

		monitor_open(true, 0x03);
		// intercom_cmd_send(CODE_ALL_ID, CMD_DOOR1_CALL);
		if (layout_monitor_talk_enable == true)
		{
			door_audio_talk(AUDIO_CH_DOOR1);
		}
		layout_montior_timeout_reset(layout_monitor_talk_enable ? 60 : 30);
		// layout_montior_timeout_reset(120);
	}
	if (layout_monitor_talk_enable == false)
	{
		if (alarm_trigger_call_get() == true)
		{
			ringplay_play_form_index(8, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
		}
		else
		{
			struct tm tm;
			user_time_read(&tm);
			if (user_data_get()->home_mode == 0)
			{
				if ((media_sdcard_insert_check() == true) && (video_input_state_get() == true))
				{
					record_video_start(REC_MODE_MANUAL);
					layout_monitor_record_leave_home();
				}
				ringplay_play_form_index(10, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
				// return;
			}
			else
			{
				if (RING_TIME_CHECK(0, 0))
				{
					door_z = 0, ring_z = 0;
					// if( user_data_get()->ring_attr[0][0].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[0][0].ring_val);
					if (user_data_get()->ring_attr[0][0].ring_mode == 1)
					{
						custom_music_play(user_data_get()->ring_attr[0][0].custom_ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
					else if (user_data_get()->ring_attr[0][0].ring_mode == 0)
					{
						ringplay_play_form_index(user_data_get()->ring_attr[0][0].ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
				}
				else if (RING_TIME_CHECK(0, 1))
				{
					door_z = 0, ring_z = 1;
					// if (user_data_get()->ring_attr[0][1].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[0][1].ring_val);
					if (user_data_get()->ring_attr[0][1].ring_mode == 1)
					{
						custom_music_play(user_data_get()->ring_attr[0][1].custom_ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
					else if (user_data_get()->ring_attr[0][1].ring_mode == 0)
					{
						ringplay_play_form_index(user_data_get()->ring_attr[0][1].ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
				}
				else if (RING_TIME_CHECK(0, 2))
				{
					door_z = 0, ring_z = 2;
					// if (user_data_get()->ring_attr[0][2].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[0][2].ring_val);
					if (user_data_get()->ring_attr[0][2].ring_mode == 1)
					{
						custom_music_play(user_data_get()->ring_attr[0][2].custom_ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
					else if (user_data_get()->ring_attr[0][2].ring_mode == 0)
					{
						printf("============>>>>qunimade\n");
						ringplay_play_form_index(user_data_get()->ring_attr[0][2].ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
				}
			}
		}
	}

	if (layout_monitor_call_record_task == NULL)
	{
		layout_monitor_call_record_task = lv_layout_task_create(monitor_call_record_delay_task, 100, LV_TASK_PRIO_MID, NULL);
		lv_layout_task_create(monitor_tuya_send_delay_task, user_data_get()->etc.tuya_send_time * 1000, LV_TASK_PRIO_MID, NULL);
	}
}
/***
**   日期:2022-07-14 17:05:36
**   作者: leo.liu
**   函数作用：门口机2 call呼叫函数
**   参数说明:
***/
static void layout_monitor_door2_call_func(void)
{
	app_playback_record(true);
	monitor_unlock_mask_set(MON_UNLOCK_CALL);
	ringplay_play_stop();
	printf("==============layout_monitor_door2_call_func\n");
	monitor_valid_channel_set(MON_CH_DOOR2, true);
	// lv_obj_t *obj_1 = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_REC);
	// lv_obj_set_state(obj_1, LV_STATE_DEFAULT);

	if (OwnID == 1)
	{
		if ((monitor_enter_mask_get() == MON_ENTER_MANUAL_DOOR) || (monitor_enter_mask_get() == MON_ENTER_MANUAL_CCTV))
		{
			h264_encode_sample_enable(false);
			intercom_cmd_send(CODE_ALL_ID, CMD_DOOR2_CALL);
		}
		else if (monitor_enter_mask_get() == MON_ENTER_CALL)
		{
			h264_encode_sample_enable(false);
			intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR2_CALL);
		}
	}

	MON_CH ch = monitor_channel_get();
	monitor_channel_set(MON_CH_DOOR2);
	adj_value_refresh_display();
	monitor_enter_mask_set(layout_monitor_talk_enable == false ? MON_ENTER_CALL : MON_ENTER_TALK);

	if (ch != MON_CH_DOOR2)
	{
		video_record_stop();
		lv_obj_t *obj_1 = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_NORMAL_CONT), LAYOUT_MONITOR_OBJ_ID_REC);
		lv_obj_set_state(obj_1, LV_STATE_DEFAULT);

		LOG_WHITE("enter door2 call\n");
		layout_montior_normal_cont_visiable();

		layout_monitor_door_swap_btn_display();
		layout_monitor_cctv_swap_btn_display();
		layout_monitor_channel_label_display();
		layout_monitor_talk_btn_display();
		// layout_monitor_cancel_btn_display();
		//  layout_monitor_lock_normal_btn_display();
		layout_monitor_lock_1_btn_display();
		layout_monitor_lock_2_btn_display();

		// layout_monitor_cctv_cancel_display();
		layout_monitor_time_label_display(NULL);
		layout_monitor_security_display((layout_monitor_talk_enable == true) ? false : true);
		monitor_open(true, 0x03);

		if (layout_monitor_talk_enable == true)
		{
			printf("===========%d==========%s=========\n", __LINE__, __func__);
			door_audio_talk(AUDIO_CH_DOOR2);
		}
		layout_montior_timeout_reset(layout_monitor_talk_enable ? 60 : 30);
		// layout_montior_timeout_reset(120);
	}
	if (layout_monitor_talk_enable == false)
	{
		LOG_WHITE("timeeout door2 call\n");
		if (alarm_trigger_call_get() == true)
		{
			ringplay_play_form_index(8, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
		}
		else
		{
			struct tm tm;
			user_time_read(&tm);
			if (user_data_get()->home_mode == 0)
			{
				if ((media_sdcard_insert_check() == true) && (video_input_state_get() == true))
				{
					record_video_start(REC_MODE_MANUAL);
					layout_monitor_record_leave_home();
				}
				ringplay_play_form_index(10, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
				// return;
			}
			else
			{
				if (RING_TIME_CHECK(1, 0))
				{
					door_z = 1, ring_z = 0;
					// if (user_data_get()->ring_attr[1][0].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[1][0].ring_val);
					if (user_data_get()->ring_attr[1][0].ring_mode == 1)
					{
						custom_music_play(user_data_get()->ring_attr[1][0].custom_ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
					else if (user_data_get()->ring_attr[1][0].ring_mode == 0)
					{
						ringplay_play_form_index(user_data_get()->ring_attr[1][0].ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
				}
				else if (RING_TIME_CHECK(1, 1))
				{
					door_z = 1, ring_z = 1;
					// if (user_data_get()->ring_attr[1][1].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[1][1].ring_val);
					if (user_data_get()->ring_attr[1][1].ring_mode == 1)
					{
						custom_music_play(user_data_get()->ring_attr[1][1].custom_ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
					else if (user_data_get()->ring_attr[1][1].ring_mode == 0)
					{
						ringplay_play_form_index(user_data_get()->ring_attr[1][1].ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
				}
				else if (RING_TIME_CHECK(1, 2))
				{
					door_z = 1, ring_z = 2;
					// if (user_data_get()->ring_attr[1][2].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[1][2].ring_val);
					if (user_data_get()->ring_attr[1][2].ring_mode == 1)
					{
						custom_music_play(user_data_get()->ring_attr[1][2].custom_ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
					else if (user_data_get()->ring_attr[1][2].ring_mode == 0)
					{
						ringplay_play_form_index(user_data_get()->ring_attr[1][2].ring, 100, layout_monitor_ringplay_doorcall_start_func, layout_monitor_ringplay_doorcall_finish_func, false);
					}
				}
			}

			monitor_ring_task_kill();
		}
	}

	if (layout_monitor_call_record_task == NULL)
	{
		layout_monitor_call_record_task = lv_layout_task_create(monitor_call_record_delay_task, 100, LV_TASK_PRIO_MID, NULL);
		lv_layout_task_create(monitor_tuya_send_delay_task, user_data_get()->etc.tuya_send_time * 1000, LV_TASK_PRIO_MID, NULL);
	}
}

/***
**   日期:2022-12-08 17:05:36
**   作者: fei.liu
**   函数作用：按键接听/挂断函数
**   参数说明:
***/
static void layout_monitor_call_talk_btn_func(void)
{
	if (adc_value_get() < 400)
	{
		touch_sound_play(ringplay_keysound_start_default_func, NULL);
		user_data_get()->monitor_alarm = 1;
		ringplay_play_stop();

		if (layout_monitor_talk_enable == false)
		{

			layout_monitor_talk_enable = true;

			sleep_mode_enter_moniter_flag = false;

			tuya_api_preview_quit();
			layout_monitor_talk_btn_display();
			layout_monitor_cancel_btn_display();
			layout_monitor_security_display(false);
			// layout_monitor_lock_normal_btn_display();
			// layout_monitor_lock_1_btn_display();
			// layout_monitor_lock_2_btn_display();
			// layout_monitor_vol_btn_display();

			door_audio_talk(AUDIO_CH_INTER);

			/***** 开锁处理 *****/
			MON_CH ch = monitor_channel_get();
			if (OwnID == 1)
			{
				if (ch == MON_CH_DOOR1)
				{
					door_audio_talk(AUDIO_CH_DOOR1);
				}
				else if (ch == MON_CH_DOOR2)
				{
					door_audio_talk(AUDIO_CH_DOOR2);
				}
				intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR_CALL_ANSWER);
				// intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR_CALL_ANSWER);
			}
			else
			{
				if (ch == MON_CH_DOOR1)
				{
					intercom_cmd_send(CODE_ALL_ID, CMD_DOOR1_AUDIO);
				}
				else if (ch == MON_CH_DOOR2)
				{
					intercom_cmd_send(CODE_ALL_ID, CMD_DOOR2_AUDIO);
				}
				door_audio_talk(AUDIO_CH_INTER);
				talk_output_gpio_ctrl_volume(user_data_get()->audio.door_talk_volume);
			}

			// monitor_enter_mask_set(MON_ENTER_DISPLAY);
			monitor_enter_mask_set(MON_ENTER_TALK);
			/***** 设置上传到tuya app为样品流 ****/
			if (monitor_enter_mask_get() != MON_ENTER_CALL)
			{
				h264_encode_sample_enable(true);
			}

			//	extern int tuya_ipc_door_bell_press_stop();
			//	tuya_ipc_door_bell_press_stop();
			tuya_api_doorbell_handup();

			tuya_pvew_video_count_fource_set(0);
			lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_STOP, 0);
			layout_montior_timeout_reset(60);
			// layout_montior_timeout_reset(120);

			// standby_timer_close();
			lv_layout_task_create(monitor_AMP_ckeck_task, 100, LV_TASK_PRIO_MID, NULL);
		}
		else
		{
			user_data_get()->monitor_alarm = 1;
			goto_layout(pLAYOUT(home));
		}
	}
	else
	{
		user_data_get()->monitor_alarm = 1;
		MON_CH ch = monitor_channel_get();
		layout_monitor_unlock_enable = true;
		monitor_unlock_mask_set(MON_UNLOCK_TALK);

		layout_monitor_unlock_icon_display();
		lv_layout_task_create(layout_monitor_unlock_icon_task, user_data_get()->etc.door_2_open_time * 1000, LV_TASK_PRIO_MID, NULL);
		monitor_unlock_open(2, ch == MON_CH_DOOR1 ? MON_CH_DOOR1 : MON_CH_DOOR2);
#ifdef MAOTOUYING_DAN_IC
		if ((tuya_api_app_talk_check() == false))
		{
			ringplay_play_form_index(7, 100, door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
		}
#endif
	}
}

/***屏幕图像效果调整数值刷新显示***/
void adj_value_refresh_display(void)
{
	extern TP9950_FORMAT tp9950_format_detection(void);
	int brightness = monitor_display_brightness_vol_get();
	int cost = monitor_display_cont_vol_get();
	int color = monitor_display_color_vol_get();

	lv_obj_t *parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_BRIGHTNESS_CONT_CONT);
	lv_obj_t *bar1 = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS);
	lv_obj_t *label1 = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_BRIGHNESS + 1);
	if (bar1 && label1)
	{
		lv_bar_set_value(bar1, brightness, true);
		lv_label_set_text_fmt(label1, "%02d", brightness);
		TP9950_FORMAT fmt = tp9950_format_detection();
		display_bright_adj(brightness, fmt);
	}

	parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_CONTRAST_CONT_CONT);
	lv_obj_t *bar2 = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_CONST);
	lv_obj_t *label2 = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_CONST + 1);
	if (bar2 && label2)
	{
		lv_bar_set_value(bar2, cost, true);
		lv_label_set_text_fmt(label2, "%02d", cost);
		TP9950_FORMAT fmt = tp9950_format_detection();
		display_const_adj(cost, fmt);
	}

	parent = lv_obj_get_child_form_id(lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_MONITOR_OBJ_ID_ADJ_CONT), LAYOUT_MONITOR_OBJ_ID_COLOR_CONT_CONT);
	lv_obj_t *bar3 = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR);
	lv_obj_t *label3 = lv_obj_get_child_form_id(parent, LAYOUT_MONITOR_OBJ_ID_ADJ_COLOR + 1);
	if (bar3 && label3)
	{
		lv_bar_set_value(bar3, color, true);
		lv_label_set_text_fmt(label3, "%02d", color);
		TP9950_FORMAT fmt = tp9950_format_detection();
		display_color_adj(color, fmt);
	}
}

/***
** 日期: 2023-08-18 10:27
** 作者: fa.liu
** 函数作用：物理按键开锁 默认处理函数
** 返回参数说明：
***/
// void layout_unclock_default(void)
// {
// 	user_data_get()->monitor_alarm = 1;
// 	MON_CH ch = monitor_channel_get();
// 	layout_monitor_unlock_enable = true;
// 	monitor_unlock_mask_set(MON_UNLOCK_TALK);

// 	layout_monitor_unlock_icon_display();
// 	lv_layout_task_create(layout_monitor_unlock_icon_task,user_data_get()->etc.door_2_open_time*1000, LV_TASK_PRIO_MID, NULL);
// 	monitor_unlock_open(2, ch == MON_CH_DOOR1? MON_CH_DOOR1 : MON_CH_DOOR2);
// 	if((layout_monitor_talk_state() == false) && (tuya_api_app_talk_check() == false))
// 	{
// 		ringplay_play_form_index(7, 100, unlock_ringa_start_func, unlock_ring_finish_func, false);
// 	}
// }

/***
**   日期:2022-07-14 17:05:56
**   作者: leo.liu
**   函数作用:创建一个监控页面
**   参数说明:
***/
CREATE_LAYOUT(monitor);