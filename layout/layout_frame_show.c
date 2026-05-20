#include "layout_define.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "video_input.h"
#include "ak_common_graphics.h"
#include "ak_tde.h"
#include "media_thumb.h"

#define FRAME_1_FILE_PATH "/app/app/wallpaper/frame_1.jpg" //"/mnt/res/wallpaper/frame_1.jpg"
#define FRAME_2_FILE_PATH "/app/app/wallpaper/frame_2.jpg" //"/mnt/res/wallpaper/frame_2.jpg"
typedef enum
{
	frame_show_scr_act_obj_id_head_obj,
	frame_show_scr_act_obj_id_cancel_obj,
	frame_show_scr_act_obj_id_channel_time_label,
	frame_show_scr_act_obj_id_aoto_icon,
	frame_show_scr_act_obj_id_sd_icon,
	frame_show_scr_act_obj_id_setting_obj,
	frame_show_scr_act_obj_id_time_hour_label,
	frame_show_scr_act_obj_id_time_min_label,
	frame_show_scr_act_obj_id_time_dot_label,
	frame_show_scr_act_obj_id_time_date_label,
	frame_show_scr_act_obj_id_calendar_year_label,
	frame_show_scr_act_obj_id_calendar_day_label,
	frame_show_scr_act_obj_id_calendar_week_label,
	frame_show_scr_act_obj_id_calendar_mon_label,
	frame_show_scr_act_obj_id_playback_name_label,
	frame_show_scr_act_obj_id_total
} frame_show_scr_act_obj_id;
static void frame_show_restart(void);
/***
**   日期:2022-06-10 09:37:43
**   作者: leo.liu
**   函数作用：动态切换的buffer
**   参数说明:
***/
static unsigned char *frame_buffer_cur_a = NULL;
static unsigned char *frame_buffer_cur_b = NULL;
/***
**   日期:2022-06-10 09:40:37
**   作者: leo.liu
**   函数作用：当前显示位置
**   参数说明:
***/
static int frame_show_frame_index = 0x00;
/***
**   日期:2022-06-10 10:24:09
**   作者: leo.liu
**   函数作用：当前显示的buffer是那个
**   参数说明: 0x00:没有显示
			低4位 0x1 -a，0x2-b
			高4位 0x1 -frame1/0x2-frame2
***/
static char *frame_show_frame_path = NULL;
/***
**   日期:2022-06-10 11:17:29
**   作者: leo.liu
**   函数作用：等待渲染次数
**   参数说明:
***/
static int frame_show_refresh_cont = 0;
/***
**   日期:2022-06-10 11:27:00
**   作者: leo.liu
**   函数作用：当前显示的是a/b
**   参数说明: 2-b 1-a
***/
static unsigned char *frame_show_pbuffer = NULL;
/***
**   日期:2022-06-10 11:33:05
**   作者: leo.liu
**   函数作用：显示偏移
**   参数说明:
***/
static int frame_show_effeset_x = 0;
/***
**   日期:2022-06-11 09:14:59
**   作者: leo.liu
**   函数作用：创建背景
**   参数说明: 必须第一个创建的控件
***/
static void frame_show_monitor_head_display(void);
/***
**   日期:2022-06-22 09:55:40
**   作者: leo.liu
**   函数作用：上次亮度值
**   参数说明:
***/
static int pre_frame_diplay_light_value = -1;
/***
**   日期:2022-06-10 09:51:32
**   作者: leo.liu
**   函数作用：判断是否可以进入frame show
**   参数说明:
***/
bool frame_display_timeout_check(void)
{
	if (user_data_get()->display.frame_time_en == false)
	{
		return false;
	}
	struct tm tm;
	user_time_read(&tm);
	unsigned int cur = tm.tm_hour * 60 + tm.tm_min;
	unsigned int start = user_data_get()->display.frame_time_start;
	unsigned int end = user_data_get()->display.frame_time_end;
	if (end < start)
	{
		end += 24 * 60;
	}
	if ((cur > end) || (cur < start))
	{
		return true;
	}
	return false;
}
/***
**   日期:2022-06-11 11:00:52
**   作者: leo.liu
**   函数作用：判断夜间模式
**   参数说明:
***/
static bool frame_display_lightmode_check(void)
{
	if (user_data_get()->display.night_mode == false)
	{
		return false;
	}
	struct tm tm;
	user_time_read(&tm);
	unsigned int cur = tm.tm_hour * 60 + tm.tm_min;
	unsigned int start = user_data_get()->display.night_time_start;
	unsigned int end = user_data_get()->display.night_time_end;
	if (start < end)
	{
		end += 24 * 60;
	}
	if ((cur < end) && (cur > start))
	{
		return true;
	}
	return false;
}
/***
**   日期:2022-06-11 10:45:59
**   作者: leo.liu
**   函数作用：设置lcr_act buffer
**   参数说明:
***/
static bool frame_show_scr_act_buffer_init(void)
{
	static rom_bin_info img = rom_bin_raw_get();
	rom_bin_raw_init(img, video_input_resident_buffer_get(NULL), LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_obj_set_style_local_pattern_image(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	return true;
}
/***
**   日期:2022-06-11 10:46:56
**   作者: leo.liu
**   函数作用：scr act buffer 销毁
**   参数说明:
***/
static bool frame_show_scr_act_buffer_destroy(void)
{
	const rom_bin_info *img = lv_obj_get_style_pattern_image(lv_scr_act(), LV_OBJ_PART_MAIN);
	lv_img_cache_invalidate_src(img);
	lv_obj_set_style_local_pattern_image(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, NULL);
	return true;
}

/***
** 日期: 2022-05-10 16:04
** 作者: leo.liu
** 函数作用：初始化帧页面
** 返回参数说明：
***/
static void frame_show_layer_init(void)
{
	frame_buffer_cur_a = ak_mem_dma_alloc(MODULE_ID_VDEC, LV_HOR_RES_MAX * LV_VER_RES_MAX * 2);
	frame_buffer_cur_b = ak_mem_dma_alloc(MODULE_ID_VDEC, LV_HOR_RES_MAX * LV_VER_RES_MAX * 2);
	frame_show_scr_act_buffer_init();
	frame_show_monitor_head_display();
	frame_show_frame_index = 0x00;
	frame_show_frame_path = NULL;
	frame_show_refresh_cont = 0;
	frame_show_pbuffer = NULL;
}
/***
**   日期:2022-06-10 09:31:27
**   作者: leo.liu
**   函数作用：销毁页面
**   参数说明:
***/
static void frame_show_layer_destory(void)
{
	if (frame_buffer_cur_a != NULL)
	{
		ak_mem_dma_free(frame_buffer_cur_a);
		frame_buffer_cur_a = NULL;
	}
	if (frame_buffer_cur_b != NULL)
	{
		ak_mem_dma_free(frame_buffer_cur_b);
		frame_buffer_cur_b = NULL;
	}
	frame_show_scr_act_buffer_destroy();
}
/***
** 日期: 2022-05-10 18:35
** 作者: leo.liu
** 函数作用：读取文件送入解码
** 返回参数说明：
***/
static bool frame_show_read_jpg_and_decode(const char *path)
{
	return thumb_media_load(0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX, path);
}
/***
**   日期:2022-06-10 10:21:11
**   作者: leo.liu
**   函数作用：隐藏所有子空间
**   参数说明:
***/
static void frame_show_hidden_obj_all(void)
{
	for (int i = 0; i < frame_show_scr_act_obj_id_total; i++)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), i);
		if (obj != NULL)
		{
			lv_obj_set_hidden(obj, true);
		}
	}
}
/***
**   日期:2022-06-10 10:41:56
**   作者: leo.liu
**   函数作用：返回按钮
**   参数说明:
***/
static void frame_show_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}
/***
**   日期:2022-06-11 09:05:44
**   作者: leo.liu
**   函数作用：上面容器显示
**   参数说明:
***/
static void frame_show_monitor_head_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_head_obj);
	if (obj == NULL)
	{
		obj = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_head_obj);
		lv_obj_set_pos(obj, 0, 0);
		lv_obj_set_size(obj, 1024, 70);
		lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000008));
		lv_obj_set_click(obj, false);
	}
	lv_obj_set_hidden(obj, false);
}
/***
**   日期:2022-06-11 09:21:01
**   作者: leo.liu
**   函数作用：通道时间显示
**   参数说明:
***/
static void frame_show_monitor_channle_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_channel_time_label);
	if (obj == NULL)
	{
		obj = lv_label_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_channel_time_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_pos(obj, 90, 25);
		lv_obj_set_size(obj, 300, 35);
		lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);
	}
	lv_obj_set_hidden(obj, false);
	static struct tm tm;
	user_time_read(&tm);
	lv_label_set_text_fmt(obj, "%s     %04d-%02d-%02d %02d:%02d", language_common_ch_string_get(monitor_channel_get()), tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min);
}
/***
**   日期:2022-06-11 09:09:56
**   作者: leo.liu
**   函数作用：显示自动图标
**   参数说明:
***/
static void frame_show_monitor_auto_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_aoto_icon);
	if (obj == NULL)
	{
		obj = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_aoto_icon);
		lv_obj_set_pos(obj, 801, 15);
		lv_obj_set_size(obj, 43, 43);
		lv_obj_set_click(obj, false);
	}
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
	lv_obj_set_hidden(obj, false);
}
/***
**   日期:2022-06-11 09:12:19
**   作者: leo.liu
**   函数作用：sd状态
**   参数说明:
***/
static void frame_show_monitor_sd_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_sd_icon);
	if (obj == NULL)
	{
		obj = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_sd_icon);
		lv_obj_set_pos(obj, 852, 15);
		lv_obj_set_size(obj, 43, 43);
		lv_obj_set_click(obj, false);
	}
	static rom_bin_info img_n = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_SDCARD_PNG);
	static rom_bin_info img_f = rom_bin_info_get(ROM_UI_01_IC_IC_SYSTEM_SDCARD_FULL_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_n);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &img_f);
	if (media_sdcard_insert_check() == true)
	{
		lv_obj_set_hidden(obj, false);
		lv_obj_set_state(obj, media_sdcard_full_check() == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
	}
	else
	{
		lv_obj_set_hidden(obj, true);
	}
}
/***
**   日期:2022-06-10 10:36:16
**   作者: leo.liu
**   函数作用：返回按钮显示
**   参数说明:
***/
static void frame_show_cancel_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_cancel_obj);
	if (obj == NULL)
	{
		obj = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_cancel_obj);
		lv_obj_set_pos(obj, 10, 5);
		lv_obj_set_size(obj, 64, 64);
		static rom_bin_info img = rom_bin_info_get(ROM_UI_WALLPAPER_HOME_PNG);
		lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);

		static obj_click_data click_data = obj_click_data_up_create(frame_show_cancel_btn_up);
		obj_click_event_listen(obj, &click_data);
	}
	lv_obj_set_hidden(obj, false);
}
/***
**   日期:2022-06-10 10:44:51
**   作者: leo.liu
**   函数作用：设置按钮
**   参数说明:
***/
static void frame_show_setting_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_frame));
}
/***
**   日期:2022-06-10 10:36:30
**   作者: leo.liu
**   函数作用：设置按钮创建
**   参数说明:
***/
static void frame_show_setting_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_setting_obj);
	if (obj == NULL)
	{
		obj = lv_obj_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_setting_obj);
		lv_obj_set_pos(obj, 928, 5);
		lv_obj_set_size(obj, 64, 64);
		static rom_bin_info img = rom_bin_info_get(ROM_UI_02_BTN_BTN_TITLE_SETTING_PNG);
		lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);

		static obj_click_data click_data = obj_click_data_up_create(frame_show_setting_btn_up);
		obj_click_event_listen(obj, &click_data);
	}
	lv_obj_set_hidden(obj, false);
}
/***
**   日期:2022-06-11 08:37:36
**   作者: leo.liu
**   函数作用：创建文件名控件
**   参数说明:
***/
static void frame_show_playback_name_display(const char *name)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_playback_name_label);
	if (obj == NULL)
	{
		obj = lv_label_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_playback_name_label);
		lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_pos(obj, 232, 30);
		lv_obj_set_size(obj, 560, 41);
		lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	}
	lv_obj_set_hidden(obj, false);
	lv_label_set_text(obj, name);
}
/***
**   日期:2022-06-10 16:30:26
**   作者: leo.liu
**   函数作用：时间控件
**   参数说明:
***/
static void frame_show_time_label_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_time_hour_label);
	if (obj == NULL)
	{
		home_time_text_create(lv_scr_act(), 360, 200, 278, 198, frame_show_scr_act_obj_id_time_hour_label, frame_show_scr_act_obj_id_time_min_label, frame_show_scr_act_obj_id_time_dot_label);
	}
	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_time_date_label);
	if (obj == NULL)
	{
		home_date_text_create(lv_scr_act(), 400, 344, 278, 41, frame_show_scr_act_obj_id_time_date_label);
	}
	struct tm tm;
	user_time_read(&tm);
	home_time_refresh_display(lv_scr_act(), &tm, frame_show_scr_act_obj_id_time_hour_label, frame_show_scr_act_obj_id_time_min_label, frame_show_scr_act_obj_id_time_dot_label, false);
	home_date_refresh_display(lv_scr_act(), &tm, frame_show_scr_act_obj_id_time_date_label);

	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_time_hour_label);
	lv_obj_set_hidden(obj, false);
	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_time_min_label);
	lv_obj_set_hidden(obj, false);
	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_time_dot_label);
	lv_obj_set_hidden(obj, false);
	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_time_date_label);
	lv_obj_set_hidden(obj, false);
}
/***
**   日期:2022-06-10 10:07:04
**   作者: leo.liu
**   函数作用：时间加载
**   参数说明:
***/
static void frame_show_time_display(void)
{
	frame_show_cancel_btn_display();
	frame_show_setting_btn_display();
	frame_show_time_label_display();
}
/***
**   日期:2022-06-10 16:46:08
**   作者: leo.liu
**   函数作用：日历控件显示
**   参数说明:
***/
static void frame_show_calendar_label_display(void)
{
	struct tm tm;
	user_time_read(&tm);
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_calendar_year_label);
	if (obj == NULL)
	{
		obj = lv_label_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_calendar_year_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));

		lv_obj_set_pos(obj, 250, 180);
		lv_obj_set_size(obj, 540, 60);
		lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	}
	lv_obj_set_hidden(obj, false);
	lv_label_set_text_fmt(obj, "%04d", tm.tm_year);

	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_calendar_week_label);
	if (obj == NULL)
	{
		obj = lv_label_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_calendar_week_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
		lv_obj_set_pos(obj, 250, 365);
		lv_obj_set_size(obj, 540, 60);

		lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	}
	lv_obj_set_hidden(obj, false);
	lv_label_set_text(obj, layout_home_string_get(HOME_LANG_ID_WEEK_1 + tm.tm_wday - 1));

	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_calendar_mon_label);
	if (obj == NULL)
	{
		obj = lv_label_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_calendar_mon_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));

		lv_obj_set_pos(obj, 250, 405);
		lv_obj_set_size(obj, 540, 60);
		lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	}
	lv_obj_set_hidden(obj, false);
	lv_label_set_text(obj, layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm.tm_mon - 1));

	obj = lv_obj_get_child_form_id(lv_scr_act(), frame_show_scr_act_obj_id_calendar_day_label);
	if (obj == NULL)
	{
		obj = lv_label_create(lv_scr_act(), NULL);
		lv_obj_set_id(obj, frame_show_scr_act_obj_id_calendar_day_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(150));

		lv_obj_set_pos(obj, 250, 200);
		lv_obj_set_size(obj, 540, 145);
		lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	}
	lv_obj_set_hidden(obj, false);
	lv_label_set_text_fmt(obj, "%02d", tm.tm_mday);
}
/***
**   日期:2022-06-11 08:26:27
**   作者: leo.liu
**   函数作用：显示预览照片
**   参数说明:
***/
static void frame_show_playback_display(void)
{
	frame_show_cancel_btn_display();
	frame_show_setting_btn_display();
	const file_info *info;
	int total;
	char file_path[128] = {0};
	memset(file_path, 0, sizeof(file_path));
	if ((media_file_total_get(FILE_TYPE_VIDEO, &total, NULL) == true) && (total > 0))
	{
		info = media_file_info_get(FILE_TYPE_VIDEO, total - 1);
		sprintf(file_path, "%s%s", SD_MEDIA_PATH, info->file_name);
	}
	else if ((media_file_total_get(FILE_TYPE_FLASH_PHOTO, &total, NULL) == true) && (total > 0))
	{
		info = media_file_info_get(FILE_TYPE_FLASH_PHOTO, total - 1);
		sprintf(file_path, "%s%s", FLASH_PHOTO_PATH, info->file_name);
	}
	else
	{
		return;
	}
	if (frame_show_read_jpg_and_decode(file_path) == true)
	{
		sprintf(file_path, "%s/%s", info->file_name, language_common_ch_string_get(info->ch));
		frame_show_playback_name_display(file_path);
	}
}
/***
**   日期:2022-06-11 09:04:58
**   作者: leo.liu
**   函数作用：door1显示
**   参数说明:
***/
static void frame_show_door_display(void)
{
	frame_show_monitor_head_display();
	frame_show_monitor_channle_display();
	frame_show_monitor_auto_display();
	frame_show_monitor_sd_display();
	frame_show_cancel_btn_display();
	frame_show_setting_btn_display();
}
/***
**   日期:2022-06-10 16:44:37
**   作者: leo.liu
**   函数作用：日历显示
**   参数说明:
***/
static void frame_show_clendar_display(void)
{
	frame_show_cancel_btn_display();
	frame_show_setting_btn_display();
	frame_show_calendar_label_display();
}
/***
**   日期:2022-06-10 16:27:09
**   作者: leo.liu
**   函数作用：延时后重新开始
**   参数说明:
***/
static void frame_show_delay_restart_task(lv_task_t *task)
{
	/* int fd = open("/mnt/a2.rgb", O_CREAT | O_WRONLY);
	write(fd, frame_buffer_cur_a, 1024 * 600 * 2);
	close(fd);
	printf("=====>>2 >frame_show_buffer=%s\n", frame_show_pbuffer == frame_buffer_cur_a ? "a" : "b");
 */
	if(intercom_data_busy_get() == false)
	{
		monitor_close();
	}
	frame_show_restart();
	frame_show_scr_act_buffer_init();
	lv_task_del(task);
}
/***
**   日期:2022-06-10 11:22:22
**   作者: leo.liu
**   函数作用：显示加载的frame
**   参数说明:
***/
static void frame_show_display(void)
{
	struct ak_tde_layer src, dst;
	tde_layer_layer_init(src, GP_FORMAT_RGB565, LV_HOR_RES_MAX, LV_VER_RES_MAX, 0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	ak_mem_dma_vaddr2paddr(frame_show_pbuffer, (unsigned long *)&src.phyaddr);
	tde_layer_layer_init(dst, GP_FORMAT_RGB565, LV_HOR_RES_MAX, LV_VER_RES_MAX, 0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	ak_mem_dma_vaddr2paddr(gui_full_varaddr_get(), (unsigned long *)&dst.phyaddr);
	ak_tde_opt_blit(&src, &dst);
	gui_refresh_enable(true);
	screen_force_refresh();
}
/***
**   日期:2022-06-10 11:34:27
**   作者: leo.liu
**   函数作用：动画左移效果
**   参数说明:
***/
static void frame_show_animation_effect_task(lv_task_t *task_t)
{
#define FRAME_STEP 32
	frame_show_effeset_x += FRAME_STEP;
	struct ak_tde_layer src, dst;
	int x = frame_show_effeset_x;

	tde_layer_layer_init(src, GP_FORMAT_RGB565, LV_HOR_RES_MAX, LV_VER_RES_MAX, LV_HOR_RES_MAX - x, 0, x, LV_VER_RES_MAX);
	ak_mem_dma_vaddr2paddr(frame_show_pbuffer == frame_buffer_cur_a ? frame_buffer_cur_b : frame_buffer_cur_a, (unsigned long *)&src.phyaddr);
	tde_layer_layer_init(dst, GP_FORMAT_RGB565, LV_HOR_RES_MAX, LV_VER_RES_MAX, 0, 0, x, LV_VER_RES_MAX);
	ak_mem_dma_vaddr2paddr(gui_full_varaddr_get(), (unsigned long *)&dst.phyaddr);
	ak_tde_opt_blit(&src, &dst);

	if (frame_show_effeset_x < 1024)
	{
		tde_layer_layer_init(src, GP_FORMAT_RGB565, LV_HOR_RES_MAX, LV_VER_RES_MAX, 0, 0, LV_HOR_RES_MAX - x, LV_VER_RES_MAX);
		ak_mem_dma_vaddr2paddr(frame_show_pbuffer, (unsigned long *)&src.phyaddr);
		tde_layer_layer_init(dst, GP_FORMAT_RGB565, LV_HOR_RES_MAX, LV_VER_RES_MAX, x, 0, LV_HOR_RES_MAX - x, LV_VER_RES_MAX);
		ak_mem_dma_vaddr2paddr(gui_full_varaddr_get(), (unsigned long *)&dst.phyaddr);
		ak_tde_opt_blit(&src, &dst);
		lv_task_set_period(task_t, 30); // 30
		lv_task_reset(task_t);
	}
	else
	{
		frame_show_pbuffer = frame_show_pbuffer == frame_buffer_cur_a ? frame_buffer_cur_b : frame_buffer_cur_a;
		if ((frame_show_frame_index == 0x08) || (frame_show_frame_index == 0x09) || (frame_show_frame_index == 0x10) || (frame_show_frame_index == 0x11))
		{
			frame_show_scr_act_buffer_destroy();
			if (intercom_data_busy_get() == false)
			{
				thumb_media_close();
				monitor_open(true, 0x01);
			}
		}
		lv_layout_task_create(frame_show_delay_restart_task, 10000, LV_TASK_PRIO_MID, NULL);
		lv_task_del(task_t);
	}
	screen_force_refresh();
}
/***
**   日期:2022-06-10 11:00:37
**   作者: leo.liu
**   函数作用：等待渲染完成
**   参数说明:
***/
static void frame_show_refresh_wait_task(lv_task_t *task)
{
	struct ak_tde_layer src, dst;
	tde_layer_layer_init(src, GP_FORMAT_RGB565, 1024, 600, 0, 0, 1024, 600);
	ak_mem_dma_vaddr2paddr(gui_full_varaddr_get(), (unsigned long *)&src.phyaddr);
	tde_layer_layer_init(dst, GP_FORMAT_RGB565, 1024, 600, 0, 0, 1024, 600);
	unsigned char *data = frame_show_pbuffer == frame_buffer_cur_a ? frame_buffer_cur_b : frame_buffer_cur_a;
	if (frame_show_pbuffer == NULL)
	{
		frame_show_pbuffer = frame_buffer_cur_a;
	}
	ak_mem_dma_vaddr2paddr(data, (unsigned long *)&dst.phyaddr);
	ak_tde_opt_blit(&src, &dst);
	frame_show_refresh_cont++;
	if (frame_show_refresh_cont == 2)
	{
		frame_show_refresh_cont = 1;

		frame_show_effeset_x = 0;
		lv_layout_task_create(frame_show_animation_effect_task, 1000, LV_TASK_PRIO_HIGHEST, NULL);
	}
	else
	{
		frame_show_display();
		lv_layout_task_create(frame_show_delay_restart_task, 10000, LV_TASK_PRIO_MID, NULL);
	}
	lv_task_del(task);
}
/***
**   日期:2022-06-10 10:34:38
**   作者: leo.liu
**   函数作用：等待解码完成
**   参数说明:
***/
static void frame_show_decode_wait_task(lv_task_t *task)
{
	lv_obj_invalidate(lv_scr_act());
	lv_layout_task_create(frame_show_refresh_wait_task, 1000, LV_TASK_PRIO_MID, NULL);
	lv_task_del(task);
}
/***
**   日期:2022-06-10 10:03:58
**   作者: leo.liu
**   函数作用：开始时间显示
**   参数说明:
***/
static void frame_show_time_start(void)
{
	if (user_data_get()->display.frame_background == true)
	{

		if ((frame_show_frame_path == NULL) || (strcmp(frame_show_frame_path, FRAME_2_FILE_PATH) == 0))
		{
			frame_show_frame_path = FRAME_1_FILE_PATH;
		}
		else
		{
			frame_show_frame_path = FRAME_2_FILE_PATH;
		}
		frame_show_read_jpg_and_decode(frame_show_frame_path);
	}
	frame_show_time_display();
	lv_layout_task_create(frame_show_decode_wait_task, 100, LV_TASK_PRIO_MID, NULL);
}
/***
**   日期:2022-06-10 10:04:29
**   作者: leo.liu
**   函数作用：开启日历显示
**   参数说明:
***/
static void frame_show_calendar_start(void)
{
	if (user_data_get()->display.frame_background == true)
	{
		if ((frame_show_frame_path == NULL) || (strcmp(frame_show_frame_path, FRAME_2_FILE_PATH) == 0))
		{
			frame_show_frame_path = FRAME_1_FILE_PATH;
		}
		else
		{
			frame_show_frame_path = FRAME_2_FILE_PATH;
		}
		frame_show_read_jpg_and_decode(frame_show_frame_path);
	}
	frame_show_clendar_display();
	lv_layout_task_create(frame_show_decode_wait_task, 100, LV_TASK_PRIO_MID, NULL);
}
/***
**   日期:2022-06-10 10:04:45
**   作者: leo.liu
**   函数作用：开启回放显示
**   参数说明:
***/
static void frame_show_playback_start(void)
{
	frame_show_playback_display();
	lv_layout_task_create(frame_show_decode_wait_task, 100, LV_TASK_PRIO_MID, NULL);

}
/***
**   日期:2022-06-10 10:05:05
**   作者: leo.liu
**   函数作用：开启door1
**   参数说明:
***/
static void frame_show_door1_start(void)
{

		// video_input_resident_bzero();
	monitor_channel_set(MON_CH_DOOR1);
	if((frame_show_refresh_cont == 0) && (frame_show_frame_index == 0x08))
	{
		monitor_open(true, 0x01);
	}
	frame_show_door_display();
	lv_layout_task_create(frame_show_decode_wait_task, 1000, LV_TASK_PRIO_MID, NULL);
}
/***
**   日期:2022-06-10 10:05:18
**   作者: leo.liu
**   函数作用：开启cctv1
**   参数说明:
***/
static void frame_show_cctv1_start(void)
{
	// video_input_resident_bzero();
	monitor_channel_set(MON_CH_CCTV1);
	frame_show_door_display();
	lv_layout_task_create(frame_show_decode_wait_task, 1000, LV_TASK_PRIO_MID, NULL);
}
/***
**   日期:2022-06-10 10:05:31
**   作者: leo.liu
**   函数作用：开启door2
**   参数说明:
***/
static void frame_show_door2_start(void)
{

	// video_input_resident_bzero();
	monitor_channel_set(MON_CH_DOOR2);
	if((frame_show_refresh_cont == 0) && (frame_show_frame_index == 0x10))
	{
		monitor_open(true, 0x01);
	}
	frame_show_door_display();
	lv_layout_task_create(frame_show_decode_wait_task, 1000, LV_TASK_PRIO_MID, NULL);
}
/***
**   日期:2022-06-10 10:05:49
**   作者: leo.liu
**   函数作用：开启cctv2
**   参数说明:
***/
static void frame_show_cctv2_start(void)
{
	//	video_input_resident_bzero();
	monitor_channel_set(MON_CH_CCTV2);
	frame_show_door_display();
	lv_layout_task_create(frame_show_decode_wait_task, 1000, LV_TASK_PRIO_MID, NULL);
}
/***
**   日期:2022-06-10 09:41:46
**   作者: leo.liu
**   函数作用：开始显示
**   参数说明:
***/
static void frame_show_restart(void)
{
	if (frame_display_timeout_check() == true)
	{
		goto_layout(pLAYOUT(close));
		return;
	}

	if ((frame_display_lightmode_check() == true) && (pre_frame_diplay_light_value != 0))
	{
		pre_frame_diplay_light_value = 0;
		backlight_brightness_set(0);
	}

	gui_refresh_enable(false);
	frame_show_hidden_obj_all();
	if ((user_data_get()->display.frame_list & 0x01) && (frame_show_frame_index < 0x01))
	{
		printf("display frame show time \n");
		/*****  开始时间的显示 *****/
		thumb_media_open();
		frame_show_frame_index = 0x01;
		frame_show_time_start();
	}
	else if ((user_data_get()->display.frame_list & 0x02) && (frame_show_frame_index < 0x02))
	{
		/*****  开始日历的显示 *****/
		thumb_media_open();
		frame_show_frame_index = 0x02;
		frame_show_calendar_start();
	}
	else if ((user_data_get()->display.frame_list & 0x04) && (frame_show_frame_index < 0x04))
	{
		/*****  开始回放的显示 *****/
		thumb_media_open();
		frame_show_frame_index = 0x04;
		frame_show_playback_start();
	}
	else if ((user_data_get()->display.frame_list & 0x08) && (frame_show_frame_index < 0x08))
	{
		/*****  开始回放的显示 *****/
		frame_show_frame_index = 0x08;
		frame_show_door1_start();
	}
	else if ((user_data_get()->display.frame_list & 0x08) && (frame_show_frame_index == 0x08))
	{
		/*****  开始回放的显示 *****/
		frame_show_frame_index = 0x09;
		frame_show_cctv1_start();
	}
	else if ((user_data_get()->display.frame_list & 0x10) && (frame_show_frame_index < 0x10))
	{
		/*****  开始回放的显示 *****/
		frame_show_frame_index = 0x10;
		frame_show_door2_start();
	}
	else if ((user_data_get()->display.frame_list & 0x10) && (frame_show_frame_index == 0x10))
	{
		/*****  开始回放的显示 *****/
		frame_show_frame_index = 0x11;
		frame_show_cctv2_start();
	}
	else
	{

		if ((user_data_get()->display.frame_list & 0x1F) == 0x00)
		{
			goto_layout(pLAYOUT(close));
			return;
		}
		else
		{
			frame_show_frame_index = 0x00;
			frame_show_restart();
		}
	}
}

static void LAYOUT_ENTER_FUNC(frame_show)
{
	h264_encode_fouce_close();
	pre_frame_diplay_light_value = -1;
	standby_timer_close();
	/***** frame 页面初始化 *****/
	frame_show_layer_init();

	/***** 打开解码器 *****/
	// thumb_media_open();
	frame_show_restart();
}

static void LAYOUT_QUIT_FUNC(frame_show)
{
	tuya_api_time_sync();
	thumb_media_close();
	frame_show_layer_destory();
	gui_refresh_enable(true);
	video_display_preview_enable(false);
	video_input_skip_frame_count_set(1000);
	fb_gui_layer_rect_fill(0x0, 0, 0, LV_HOR_RES, LV_VER_RES_MAX);
	screen_force_refresh();

	standby_timer_restart(true);

	backlight_brightness_set(user_data_get()->display.lcd_brigtness);
	h264_encode_fouce_open();
}

CREATE_LAYOUT(frame_show);
