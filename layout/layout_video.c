#include "layout_define.h"
#include "layout_setting_common.h"
#include "media_thumb.h"
int playback_pview_item_get(void);
void playback_pview_item_set(int item);
file_type playback_pview_type_get(void);
const char *playback_pview_path_get(void);
void playbacK_left_btn_create(int obj_id, void (*callback)(lv_obj_t *), int x, int y, int w, int h);
void playback_right_btn_create(int obj_id, void (*callback)(lv_obj_t *), int x, int y, int w, int h);
int playback_media_total_get(void);
void playback_media_total_set(int total);
const file_info *playback_media_info_get(void);
int playback_media_top_get(void);
void playback_media_top_set(int val);
lv_obj_t *layout_media_head_count_create(int obj_id, lv_opa_t);
void layout_media_cancel_btn_create(lv_obj_t *parent, void (*callback)(lv_obj_t *obj));
void layout_media_info_label_create(lv_obj_t *parent, int obj_id);

static void layout_video_full_load(void);
typedef enum
{
	video_scr_act_obj_id_head_cont,
	video_scr_act_obj_id_head_info_label,
	video_scr_act_obj_id_left_btn,
	video_scr_act_obj_id_right_btn,
	video_scr_act_obj_id_buttom_name,
	video_scr_act_obj_id_timeout_label,
	video_scr_act_obj_id_play,
	video_scr_act_obj_id_bar,
	video_scr_act_obj_id_msg_dialog,
} video_scr_act_obj_id;
/***
** 日期: 2022-05-13 10:02
** 作者: leo.liu
** 函数作用：正常刷新区域
** 返回参数说明：
***/
static void layout_video_full_refresh(void)
{
	refresh_area_t area[] = {
	    {0, 0, 1024, 70},
	    {15, 264, 83, 83},
	    {935, 264, 83, 83},
	    {281, 168, 576, 352},
	    {48, 513, 525, 41},
	    {892, 513, 104, 41},
	    {41, 556, 940, 18}};
	gui_refresh_area(area, sizeof(area) / sizeof(refresh_area_t));
	fb_gui_layer_rect_fill(0x00, 0, 0, 1024, 600);
}
/***
**   日期:2022-06-20 09:19:37
**   作者: leo.liu
**   函数作用：部分刷新
**   参数说明:
***/
static void layout_video_part_refresh(void)
{
	refresh_area_t area[] = {
	    {892, 513, 104, 41},
	    {41, 556, 940, 18}};
	gui_refresh_area(area, sizeof(area) / sizeof(refresh_area_t));
}
/***
**   日期:2022-05-24 14:08:52
**   作者: leo.liu
**   函数作用：返回
**   参数说明:
***/
static void layout_video_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(playback));
}

/***
**   日期:2022-05-24 11:12:09
**   作者: leo.liu
**   函数作用：取消删除
**   参数说明:
***/
static void layout_video_delete_msg_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(video_scr_act_obj_id_msg_dialog);
}
/***
**   日期:2022-05-24 11:12:54
**   作者: leo.liu
**   函数作用：确认删除
**   参数说明:
***/
static void layout_video_delete_msg_confirm_btn_up(lv_obj_t *obj)
{
	video_play_stop();

	file_type type = playback_pview_type_get();
	int item = playback_pview_item_get();
	media_file_delete(type, item);

	int total;
	media_file_total_get(type, &total, NULL);
	if (total < 1)
	{
		goto_layout(pLAYOUT(home));
		return;
	}

	playback_media_total_set(total);
	if (item >= total)
	{
		playback_pview_item_set(total - 1);
	}

	int top = playback_media_top_get();
	if (top == 0)
	{
		top = (total > 5) ? 5 : (total - 1);
	}
	else
	{
		top -= 1;
	}
	playback_media_top_set(top);

	setting_msgdialog_msg_bg_delete(video_scr_act_obj_id_msg_dialog);

	const file_info *info = playback_media_info_get();
	if (info->type != FILE_TYPE_VIDEO)
	{
		goto_layout(pLAYOUT(photo));
	}
	else
	{
		layout_video_full_load();
	}
}

/***
**   日期:2022-05-24 14:12:15
**   作者: leo.liu
**   函数作用：删除按钮创建
**   参数说明:
***/
static void layout_video_delete_btn_up(lv_obj_t *obj)
{
	VIDEO_PLAY_STATUS state = video_play_status_get();
	if (state == VIDEO_PLAY_STATE_PLAY)
	{
		video_play_pause();
	}

	if (setting_msgdialog_msg_bg_delete(video_scr_act_obj_id_msg_dialog) == true)
	{
		return;
	}
	lv_obj_t *dialog = setting_msgdialog_msg_bg_create(video_scr_act_obj_id_msg_dialog);
	setting_msgdialog_msg_confirm_and_cancel_btn_create(dialog, layout_video_delete_msg_cancel_btn_up, layout_video_delete_msg_confirm_btn_up);
	setting_msgdialog_video_msg_create(dialog, layout_playback_string_get(PLAYBACK_LANG_ID_DELETE_VIDEO));
	layout_video_full_refresh();
}
static void layout_video_delete_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 948, 10);
	lv_obj_set_size(obj, 60, 60);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_DELETE_SECURITY_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(layout_video_delete_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
**   日期:2022-05-24 14:14:30
**   作者: leo.liu
**   函数作用：信息显示
**   参数说明:
***/
static void layout_video_info_label_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_head_cont);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, video_scr_act_obj_id_head_info_label);
	const file_info *pinfo = playback_media_info_get();
	const char *ch = language_common_ch_string_get(pinfo->ch);
	lv_label_set_text_fmt(label, "%s[%05d/%05d]", ch, playback_pview_item_get() + 1, playback_media_total_get());
}

/***
**   日期:2022-05-24 14:09:10
**   作者: leo.liu
**   函数作用：上一个视频
**   参数说明:
***/
static void layout_video_left_btn_up(lv_obj_t *obj)
{
	video_play_stop();
	int item = playback_pview_item_get();
	item++;
	if (item >= playback_media_total_get())
	{
		item = 0;
	}
	playback_pview_item_set(item);
	const file_info *info = playback_media_info_get();
	if (info->type != FILE_TYPE_VIDEO)
	{
		goto_layout(pLAYOUT(photo));
	}
	else
	{
		setting_msgdialog_msg_bg_delete(video_scr_act_obj_id_msg_dialog);
		layout_video_full_load();
	}
}
/***
**   日期:2022-05-24 14:09:32
**   作者: leo.liu
**   函数作用：下一个视频
**   参数说明:
***/
static void layout_video_right_btn_up(lv_obj_t *obj)
{
	video_play_stop();
	int item = playback_pview_item_get();
	item--;
	if (item < 0)
	{
		item = playback_media_total_get() - 1;
	}
	playback_pview_item_set(item);
	const file_info *info = playback_media_info_get();
	if (info->type != FILE_TYPE_VIDEO)
	{
		goto_layout(pLAYOUT(photo));
	}
	else
	{
		setting_msgdialog_msg_bg_delete(video_scr_act_obj_id_msg_dialog);
		layout_video_full_load();
	}
}

/***
**   日期:2022-05-24 14:16:56
**   作者: leo.liu
**   函数作用：时间显示
**   参数说明:
***/
static void layout_video_buttom_name_display(void)
{
	lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_buttom_name);
	const file_info *info = playback_media_info_get();

	char str[128] = {"20"};
	strncpy(&str[2], &info->file_name[0], 2);
	str[4] = '-';
	strncat(&str[5], &info->file_name[2], 2);
	str[7] = '-';
	strncat(&str[8], &info->file_name[4], 2);
	str[10] = ' ';
	str[11] = ' ';

	strncat(&str[12], &info->file_name[7], 2);
	str[14] = ':';
	strncat(&str[15], &info->file_name[9], 2);
	str[17] = ':';
	strncat(&str[18], &info->file_name[11], 2);

	lv_label_set_text(label, str);
}
/***
**   日期:2022-05-24 14:09:55
**   作者: leo.liu
**   函数作用：文件时间创建
**   参数说明:
***/
static void layout_video_buttom_name_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_id(label, video_scr_act_obj_id_buttom_name);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 48, 513);
	lv_obj_set_size(label, 525, 60);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
}

/***
**   日期:2022-05-24 14:10:17
**   作者: leo.liu
**   函数作用：全屏加载
**   参数说明:
***/
static void layout_video_full_load(void)
{
	thumb_media_buffer_clear();
	layout_video_info_label_display();
	layout_video_buttom_name_display();

	const file_info *pinfo = playback_media_info_get();
	char file[128] = {0};
	strcpy(file, playback_pview_path_get());
	strcat(file, pinfo->file_name);
	video_play_start(file);

	if (pinfo->is_new == true)
	{
		media_file_new_clear(pinfo->type, playback_pview_item_get());
	}
}

static void layout_video_play_btn_up(lv_obj_t *obj)
{
	VIDEO_PLAY_STATUS status = video_play_status_get();
	if (status == VIDEO_PLAY_STATE_IDLE)
	{
		layout_video_full_load();
	}
	else
	{
		video_play_pause();
	}
}
/***
**   日期:2022-05-24 17:07:29
**   作者: leo.liu
**   函数作用：创建播放按钮
**   参数说明:
***/
static void layout_video_play_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, video_scr_act_obj_id_play);
	lv_obj_set_pos(obj, 436, 228);
	lv_obj_set_size(obj, 152, 152);
	static rom_bin_info img_pause = rom_bin_info_get(ROM_UI_29_FULL_PAUSE_PNG);
	static rom_bin_info img_play = rom_bin_info_get(ROM_UI_30_FULLL_PLAY_L_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_play);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &img_pause);
	static obj_click_data click_data = obj_click_data_up_create(layout_video_play_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
**   日期:2022-05-25 18:54:29
**   作者: leo.liu
**   函数作用：任务状态显示
**   参数说明:
***/
static void layout_play_state_task(lv_task_t *task_t)
{
	int cur = 0, total = 0;
	bool reslut = video_play_duration_get(&cur, &total);
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_play);
	if (obj != NULL)
	{
		VIDEO_PLAY_STATUS status = video_play_status_get();
		lv_state_t state = lv_obj_get_state(obj, LV_OBJ_PART_MAIN);
		if ((reslut == false) || (cur < 100) || ((status != VIDEO_PLAY_STATE_PLAY) && (state != LV_STATE_DEFAULT)))
		{
			lv_obj_set_state(obj, LV_STATE_DEFAULT);
			standby_timer_restart(true);
		}
		else if ((status == VIDEO_PLAY_STATE_PLAY) && ((state & LV_STATE_CHECKED) == 0))
		{
			lv_obj_set_state(obj, LV_STATE_CHECKED);
			standby_timer_close();
		}

		lv_obj_t *bar = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_bar);
		if (bar != NULL)
		{
			lv_bar_set_value(bar, cur * 100 / total, true);

			lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_timeout_label);
			lv_label_set_text_fmt(label, "%02d:%02d", cur / 1000, total / 1000);
			// if(obj->state == LV_STATE_DEFAULT)
			// {
			// 	lv_label_set_text_fmt(label, "%02d:%02d", 0, total / 1000);
			// }
			// else
			// {
			// 	lv_label_set_text_fmt(label, "%02d:%02d", 0,  cur / 1000);
			// }
		}
	}
}

/***
**   日期:2022-05-25 19:18:44
**   作者: leo.liu
**   函数作用：显示进度条
**   参数说明:
***/
static void layout_video_progress_bar_create(void)
{
	lv_obj_t *bar = lv_bar_create(lv_scr_act(), NULL);
	lv_obj_set_id(bar, video_scr_act_obj_id_bar);
	lv_obj_set_pos(bar, 41, 556);
	lv_obj_set_size(bar, 940, 10);
	lv_bar_set_range(bar, 0, 100);
	lv_bar_set_value(bar, 0, false);

	lv_obj_set_style_local_bg_opa(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x666666));
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x0096ff));
}
/***
**   日期:2022-05-26 08:12:47
**   作者: leo.liu
**   函数作用：显示剩余时间
**   参数说明:
***/
static void layout_video_timeout_label_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_id(label, video_scr_act_obj_id_timeout_label);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 892, 513);
	lv_obj_set_size(label, 104, 41);
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text_fmt(label, "%02d:%0d", 0, 0);
}
/***
**   日期:2022-05-26 08:08:49
**   作者: leo.liu
**   函数作用：左按钮显示
**   参数说明:
***/
static void layout_video_left_enable(bool en)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_left_btn);
	lv_obj_set_hidden(obj, en ? false : true);
}
/***
**   日期:2022-05-26 08:09:39
**   作者: leo.liu
**   函数作用：右显示
**   参数说明:
***/
static void layout_video_right_enable(bool en)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_right_btn);
	lv_obj_set_hidden(obj, en ? false : true);
}
/***
** 日期: 2022-04-28 09:53
** 作者: leo.liu
** 函数作用：监控被按下执行的回调函数
** 返回参数说明：
***/
void layout_video_click_down_func(lv_obj_t *obj)
{
	if(user_data_get()->home_mode == 1)
	{
		return;
	}
	lv_obj_t *obj1 = lv_obj_get_child_form_id(lv_scr_act(), video_scr_act_obj_id_play);
	if(obj1->state == LV_STATE_CHECKED)
	{
		return;
	}
	//***** 控制声音流向 *****/
	touch_sound_play(NULL, NULL);
}

/***
**   日期:2022-06-20 09:21:38
**   作者: leo.liu
**   函数作用：创建一个容器，用于全屏显示
**   参数说明:
***/
static void layout_video_screen_full_obj_up(lv_obj_t *obj)
{
	bool *p_is_full = (bool *)obj->user_data;
	bool is_full = *p_is_full;
	if (is_full == false)
	{
		*p_is_full = true;
		layout_video_part_refresh();
	}
	else
	{
		*p_is_full = false;
		layout_video_full_refresh();
	}
	lv_obj_invalidate(lv_scr_act());
}

static void layout_video_screen_full_obj_create(void) 
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 1024, 600);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	static obj_click_data click_data = obj_click_data_up_create(layout_video_screen_full_obj_up);
	obj_click_event_listen(obj, &click_data);
	static bool is_full = false;
	is_full = false;
	obj->user_data = &is_full;
}

static void LAYOUT_ENTER_FUNC(video)
{
	layout_video_full_refresh();
	/***** 必须放在其他的控件的最下面层 *****/
	layout_video_screen_full_obj_create();

	lv_obj_t *cont = layout_media_head_count_create(video_scr_act_obj_id_head_cont, LV_OPA_COVER);
	layout_media_cancel_btn_create(cont, layout_video_cancel_btn_up);
	layout_media_info_label_create(cont, video_scr_act_obj_id_head_info_label);
	layout_video_delete_btn_create(cont);
	playbacK_left_btn_create(video_scr_act_obj_id_left_btn, layout_video_left_btn_up, 15, 264, 83, 83);
	playback_right_btn_create(video_scr_act_obj_id_right_btn, layout_video_right_btn_up, 935, 264, 83, 83);
	layout_video_buttom_name_create();
	layout_video_play_btn_create();
	layout_video_progress_bar_create();
	layout_video_timeout_label_create();
	layout_video_left_enable(true);
	layout_video_right_enable(true);
	layout_video_full_load();
	lv_layout_task_create(layout_play_state_task, 45, LV_TASK_PRIO_MID, NULL);
	lv_obj_click_down_callback_register(layout_video_click_down_func);
}
static void LAYOUT_QUIT_FUNC(video)
{
	video_display_preview_enable(false);
	lv_obj_click_down_callback_register(layout_obj_click_down_func);
	video_play_stop();
	const layout *cur_layout = cur_layout_get();
	if ((cur_layout != pLAYOUT(playback)) && (cur_layout != pLAYOUT(photo)))
	{
		thumb_media_close();
		layout_sd_state_callback_register(NULL);
	}
	else if (cur_layout == pLAYOUT(playback))
	{
		lv_obj_invalidate(lv_scr_act());
	}
	standby_timer_restart(true);
}

CREATE_LAYOUT(video);