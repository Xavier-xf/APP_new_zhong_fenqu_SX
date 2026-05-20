#include "layout_define.h"

#define LAYOUT_SETTING_DISPLAY_OBJ_OPEN_FRAME_ID 0X01
#define LAYOUT_SETTING_DISPLAY_OBJ_PROGRESS_ID 0X02
#define LAYOUT_SETTING_DISPLAY_OBJ_PROGRESS_LABEL_ID 0X03

static void setting_display_screen_adj_btn_up(lv_obj_t *obj)
{
	if (intercom_data_busy_get() == true)
	{
		return;
	}
	if (OwnID == 1)
	{

		if (monitor_valid_channel_check(MON_CH_DOOR1) == true)
		{
			monitor_channel_set(MON_CH_DOOR1);
		}
		else if (monitor_valid_channel_check(MON_CH_DOOR2) == true)
		{
			monitor_channel_set(MON_CH_DOOR2);
		}
		else
		{
			return;
		}
		LOG_BLUE("分机挂断5");
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
		monitor_enter_mask_set(MON_ENTER_DISPLAY);
		goto_layout(pLAYOUT(adj_display));
	}
	else
	{
		intercom_cmd_send(1, CMD_DOOR_ENTER);
	}
}
/***
** 日期: 2022-05-04 10:10
** 作者: leo.liu
** 函数作用：画面调整按钮创建
** 返回参数说明：
***/
static bool setting_display_screen_adj_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_display_screen_adj_btn_up);
	setting_right_btn_base_create(NULL, 353, 94, 622, 90,
				      layout_setting_display_string_get(SETTING_DISPLAY_LANG_ID_SCREEN_ADJ),
				      NULL,
				      &click_data,
				      0);
	return true;
}

/***
** 日期: 2022-05-04 10:18
** 作者: leo.liu
** 函数作用：获取待机类型字符串
** 返回参数说明：
***/
static const char *setting_display_standby_mode_string_get(void)
{
	int id = user_data_get()->display.standby_mode ? SETTING_DISPLAY_LANG_ID_OPEN_FRAME : SETTING_DISPLAY_LANG_ID_LCD_OFF;

	return layout_setting_display_string_get(id);
}

static void setting_display_enter_standby_mode_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_frame));
}
/***
** 日期: 2022-05-04 10:10
** 作者: leo.liu
** 函数作用：进入待机模式设置按钮创建
** 返回参数说明：
***/
static bool setting_display_enter_standby_mode_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_display_enter_standby_mode_btn_up);
	setting_right_btn_base_create(NULL, 353, 184, 622, 90,
				      layout_setting_display_string_get(SETTING_DISPLAY_LANG_ID_STANDBY_MODE),
				      setting_display_standby_mode_string_get(),
				      &click_data,
				      LAYOUT_SETTING_DISPLAY_OBJ_OPEN_FRAME_ID);

	return true;
}

static void setting_display_display_clean_mode_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(clear_screen));
}
/***
** 日期: 2022-05-04 10:11
** 作者: leo.liu
** 函数作用：屏幕清除按钮创建
** 返回参数说明：
***/
static bool setting_display_display_clean_mode_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_display_display_clean_mode_btn_up);
	setting_right_btn_base_create(NULL, 353, 274, 622, 90,
				      layout_setting_display_string_get(SETTING_DISPLAY_LANG_ID_DISPLAY_CLEAN),
				      NULL,
				      &click_data,
				      0);

	return true;
}

/***
** 日期: 2022-05-04 11:40
** 作者: leo.liu
** 函数作用：设置进度条显示的数值
** 返回参数说明：
***/
static bool setting_bright_progress_value_label_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	if (label == NULL)
	{
		return false;
	}
	lv_obj_set_id(label, LAYOUT_SETTING_DISPLAY_OBJ_PROGRESS_LABEL_ID);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));

	lv_obj_t *slider = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_DISPLAY_OBJ_PROGRESS_ID);

	lv_label_set_text_fmt(label, "%02d", user_data_get()->display.lcd_brigtness);
	lv_obj_align(label, slider, LV_ALIGN_IN_RIGHT_MID, 60, 0);
	lv_obj_set_auto_realign(label, true);
	return true;
}

/***
** 日期: 2022-05-04 11:55
** 作者: leo.liu
** 函数作用：进度条事件
** 返回参数说明：
***/
static void setting_bright_progress_click(lv_obj_t *obj, lv_event_t ev)
{
	if (ev == LV_EVENT_VALUE_CHANGED)
	{
		lv_obj_t *lebel = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_DISPLAY_OBJ_PROGRESS_LABEL_ID);
		int value = lv_slider_get_value(obj);
		lv_label_set_text_fmt(lebel, "%02d", value);

		user_data_get()->display.lcd_brigtness = value;
		user_data_save();
		backlight_brightness_set(value);
	}
}
/***
** 日期: 2022-05-04 11:24
** 作者: leo.liu
** 函数作用：创建亮度设置的进度条
** 返回参数说明：
***/
static bool setting_bright_progress_create(void)
{
	lv_obj_t *slider = lv_slider_create(lv_scr_act(), NULL);
	if (slider == NULL)
	{
		return false;
	}
	lv_obj_set_id(slider, LAYOUT_SETTING_DISPLAY_OBJ_PROGRESS_ID);
	lv_obj_set_pos(slider, 520, 403);
	lv_obj_set_size(slider, 400, 9);

	lv_obj_set_style_local_bg_color(slider, LV_SLIDER_PART_BG, LV_STATE_DEFAULT, lv_color_make(0xb6, 0xb6, 0xb6));
	lv_obj_set_style_local_bg_color(slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, lv_color_make(0x3A, 0xC3, 0x92));

	lv_slider_set_range(slider, 0, 100);
	lv_slider_set_value(slider, user_data_get()->display.lcd_brigtness, false);

	static obj_click_data click_data = obj_click_data_anything_create(setting_bright_progress_click);
	obj_click_event_listen(slider, &click_data);
	return true;
}

/***
** 日期: 2022-05-04 10:11
** 作者: leo.liu
** 函数作用：画面亮度调整按钮创建
** 返回参数说明：
***/
static bool setting_dipslay_display_bright_btn_create(void)
{

	setting_right_btn_base_create(NULL, 353, 364, 622, 90,
					layout_setting_display_string_get(SETTING_DISPLAY_LANG_ID_DISPLAY_BRIGHTNESS),
					NULL,
					NULL,
					0);
		
	setting_bright_progress_create();
	setting_bright_progress_value_label_create();

	return true;
}

static void LAYOUT_ENTER_FUNC(setting_display)
{
	setting_cancel_btn_create(NULL);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	setting_main_btn_create(4);

	setting_display_screen_adj_btn_create();

	setting_display_enter_standby_mode_btn_create();

	setting_display_display_clean_mode_btn_create();

	setting_dipslay_display_bright_btn_create();
}

static void LAYOUT_QUIT_FUNC(setting_display)
{
}

CREATE_LAYOUT(setting_display);