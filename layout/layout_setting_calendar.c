#include "layout_define.h"

#define LAYOUT_SETTING_CALENDAR_OBJ_ID_YEAR_MONTH_DAY_TXT 0X01
#define LAYOUT_SETTING_CALENDAR_OBJ_ID_DAY_SELECT 0X02
#define LAYOUT_SETTING_CALENDAR_OBJ_ID_DAY_BASE 0X1000

static void setting_calendar_year_month_day_txt_display(void);
static void setting_calendar_day_txt_display(void);

/***
** 日期: 2022-05-09 15:56
** 作者: leo.liu
** 函数作用：calendar 返回
** 返回参数说明：
***/
static void layout_calendar_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}

static void calendar_setting_btn_up(lv_obj_t *obj)
{
	setting_time_enter_flag_set(0x01);
	goto_layout(pLAYOUT(setting_time));
}

/***
** 日期: 2022-05-09 16:28
** 作者: leo.liu
** 函数作用：创建设置事件按钮
** 返回参数说明：
***/
static void setting_clendar_seting_btn_create(void)
{
	static rom_bin_info image = rom_bin_info_get(ROM_UI_02_BTN_BTN_TITLE_SETTING_PNG);
	static obj_click_data click_data = obj_click_data_up_create(calendar_setting_btn_up);

	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 948, 18);
	lv_obj_set_size(obj, 58, 58);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &image);
	obj_click_event_listen(obj, &click_data);
}

static void setting_calendar_month_left_btn_up(lv_obj_t *obj)
{
	lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_CALENDAR_OBJ_ID_YEAR_MONTH_DAY_TXT);
	struct tm *tm = (struct tm *)label->user_data;
	tm->tm_mon--;
	if (tm->tm_mon < 1)
	{
		tm->tm_mon = 12;
		tm->tm_year--;
	}

	setting_calendar_year_month_day_txt_display();
	setting_calendar_day_txt_display();
}

static void setting_calendar_month_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_CALENDAR_OBJ_ID_YEAR_MONTH_DAY_TXT);
	struct tm *tm = (struct tm *)label->user_data;
	tm->tm_mon++;
	if (tm->tm_mon > 12)
	{
		tm->tm_mon = 1;
		tm->tm_year++;
	}

	setting_calendar_year_month_day_txt_display();
	setting_calendar_day_txt_display();
}
/***
** 日期: 2022-05-09 16:26
** 作者: leo.liu
** 函数作用：创建月份调整按钮
** 返回参数说明：
***/
static void setting_calendar_month_btn_create(void)
{

	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 232, 128);
	lv_obj_set_size(obj, 100, 48);
	static rom_bin_info image_left = rom_bin_info_get(ROM_UI_02_BTN_BTN_LIST_LEFT_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &image_left);

	static obj_click_data click_data_left = obj_click_data_up_create(setting_calendar_month_left_btn_up);
	obj_click_event_listen(obj, &click_data_left);

	obj = lv_obj_create(lv_scr_act(), obj);
	lv_obj_set_x(obj, 760);
	static rom_bin_info image_right = rom_bin_info_get(ROM_UI_02_BTN_BTN_LIST_RIGHT_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &image_right);
	static obj_click_data click_data_right = obj_click_data_up_create(setting_calendar_month_right_btn_up);
	obj_click_event_listen(obj, &click_data_right);
}

/***
** 日期: 2022-05-09 17:10
** 作者: leo.liu
** 函数作用：年月日文本显示
** 返回参数说明：
***/
static void setting_calendar_year_month_day_txt_display(void)
{
	lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_CALENDAR_OBJ_ID_YEAR_MONTH_DAY_TXT);
	struct tm *tm = (struct tm *)label->user_data;

	LANGUAGE_ID language_id = language_id_get();
	if (language_id == LANGUAGE_ID_ENGLISH)
	{
		lv_label_set_text_fmt(label, "%s %d,%04d", layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_mday, tm->tm_year);
	}
	// else if (language_id == LANGUAGE_ID_HANYU)
	// {
	// 	lv_label_set_text_fmt(label, "%04d년%d월%d일", tm->tm_year, tm->tm_mon, tm->tm_mday);
	// }
	// else if (language_id == LANGUAGE_ID_ELUOSI)
	// {
	// 	lv_label_set_text_fmt(label, "%s,%d,%04d", layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_mday, tm->tm_year);
	// }
	// else if (language_id == LANGUAGE_ID_XIBANYA)
	// {
	// 	lv_label_set_text_fmt(label, "%d-%s,%04d", tm->tm_mday, layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_year);
	// }
	// else if (language_id == LANGUAGE_ID_CHINESE)
	// {
	// 	lv_label_set_text_fmt(label, "%04d年%d月%d日", tm->tm_year, tm->tm_mon, tm->tm_mday);
	// }
	// else if (language_id == LANGUAGE_ID_TUERQI)
	// {
	// 	lv_label_set_text_fmt(label, "%d %s %04d", tm->tm_mday, layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_year);
	// }
	// else if (language_id == LANGUAGE_ID_BOLAN)
	// {
	// 	lv_label_set_text_fmt(label, "%d %s %04d", tm->tm_mday, layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_year);
	// }
	// else if (language_id == LANGUAGE_ID_JIEKE)
	// {
	// 	lv_label_set_text_fmt(label, "%d %s %04d", tm->tm_mday, layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_year);
	// }
	else if (language_id == LANGUAGE_ID_ALABOYU)
	{
		lv_label_set_text_fmt(label, "%s, %d, %04d", layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_mday, tm->tm_year);
	}
	// else if (language_id == LANGUAGE_ID_PUTAOYA)
	// {
	// 	lv_label_set_text_fmt(label, "%s, %d, %04d", layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_mday, tm->tm_year);
	// }
	// else if (language_id == LANGUAGE_ID_FAYU)
	// {
	// 	lv_label_set_text_fmt(label, "%s, %d, %04d", layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_mday, tm->tm_year);
	// }
	// else if (language_id == LANGUAGE_ID_DEYU)
	// {
	// 	lv_label_set_text_fmt(label, "%s, %d, %04d", layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1), tm->tm_mday, tm->tm_year);
	// }
}
/***
** 日期: 2022-05-09 16:26
** 作者: leo.liu
** 函数作用：创建年月日文本显示框
** 返回参数说明：
***/
static void setting_calendar_year_month_day_txt_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 302, 120);
	lv_obj_set_size(label, 419, 33);
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_id(label, LAYOUT_SETTING_CALENDAR_OBJ_ID_YEAR_MONTH_DAY_TXT);

	static struct tm calendar_date;
	user_time_read(&calendar_date);
	label->user_data = &calendar_date;
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));

	setting_calendar_year_month_day_txt_display();
}

/***
** 日期: 2022-05-09 16:26
** 作者: leo.liu
** 函数作用：创建星期文本显示框
** 返回参数说明：
***/
static void setting_calendar_week_txt_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 232, 186);
	lv_obj_set_size(label, 100, 45);
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
	lv_label_set_text(label, layout_home_string_get(HOME_LANG_ID_WEEK_7));
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xb6b6b6));

	label = lv_label_create(lv_scr_act(), label);
	lv_obj_set_x(label, 232 + 90);
	lv_label_set_text(label, layout_home_string_get(HOME_LANG_ID_WEEK_1));

	label = lv_label_create(lv_scr_act(), label);
	lv_obj_set_x(label, 232 + 90 * 2);
	lv_label_set_text(label, layout_home_string_get(HOME_LANG_ID_WEEK_2));

	label = lv_label_create(lv_scr_act(), label);
	lv_obj_set_x(label, 232 + 90 * 3);
	lv_label_set_text(label, layout_home_string_get(HOME_LANG_ID_WEEK_3));

	label = lv_label_create(lv_scr_act(), label);
	lv_obj_set_x(label, 232 + 90 * 4);
	lv_label_set_text(label, layout_home_string_get(HOME_LANG_ID_WEEK_4));

	label = lv_label_create(lv_scr_act(), label);
	lv_obj_set_x(label, 232 + 90 * 5);
	lv_label_set_text(label, layout_home_string_get(HOME_LANG_ID_WEEK_5));

	label = lv_label_create(lv_scr_act(), label);
	lv_obj_set_x(label, 232 + 90 * 6);
	lv_label_set_text(label, layout_home_string_get(HOME_LANG_ID_WEEK_6));
}

/***
** 日期: 2022-05-09 17:27
** 作者: leo.liu
** 函数作用：显示万年历
** 返回参数说明：
***/
static void setting_calendar_day_txt_display(void)
{
	lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_CALENDAR_OBJ_ID_YEAR_MONTH_DAY_TXT);
	struct tm tm = *((struct tm *)label->user_data);
	struct tm cur_time;
	user_time_read(&cur_time);
	/***** 当前显示的是当前月份的 *****/
	bool info_is_cur_month = ((cur_time.tm_year == tm.tm_year) && (cur_time.tm_mon == tm.tm_mon)) ? true : false;

	/***** 显示的的日期的1号的星期*****/
	tm.tm_mday = 1;
	int cur_day_week = user_cur_time_is_week(&tm) % 7;
	/***** 获取当前月份的最后一天 *****/
	int cur_month_last_day = user_cur_time_last_day(&tm);
	struct tm prev = tm;
	prev.tm_mon--;
	if (prev.tm_mon < 1)
	{
		prev.tm_year--;
		prev.tm_mon = 12;
	}

	int prev_month_last_day = user_cur_time_last_day(&prev);
	int prev_month_start_day = prev_month_last_day - cur_day_week + 1;

	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_CALENDAR_OBJ_ID_DAY_SELECT);
	lv_obj_set_hidden(obj, true);

	int cur_month_start_day = 1;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_CALENDAR_OBJ_ID_DAY_BASE + i * 7 + j);
			if ((i == 0) && (j < cur_day_week))
			{
				lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4a4a4a));
				lv_label_set_text_fmt(label, "%d", prev_month_start_day++);
			}
			else if (cur_month_start_day <= cur_month_last_day)
			{
				lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
				lv_label_set_text_fmt(label, "%d", cur_month_start_day);
				if ((info_is_cur_month == true) && (cur_month_start_day == cur_time.tm_mday))
				{
					lv_obj_set_pos(obj, lv_obj_get_x(label) + 8, lv_obj_get_y(label) + 2);
					lv_obj_set_hidden(obj, false);
				}
				cur_month_start_day++;
			}
			else
			{
				lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x4a4a4a));
				lv_label_set_text_fmt(label, "%d", cur_month_start_day - cur_month_last_day);
				cur_month_start_day++;
			}
		}
	}
}

/***
** 日期: 2022-05-09 16:27
** 作者: leo.liu
** 函数作用：创建日历显示
** 返回参数说明：
***/
static void setting_calendar_day_txt_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, LAYOUT_SETTING_CALENDAR_OBJ_ID_DAY_SELECT);
	lv_obj_set_size(obj, 48, 48);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));
	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 60);
	lv_obj_set_hidden(obj, true);

	lv_obj_t *label = NULL;
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			label = lv_label_create(lv_scr_act(), label);
			lv_obj_set_id(label, LAYOUT_SETTING_CALENDAR_OBJ_ID_DAY_BASE + i * 7 + j);
			if ((i == 0) && (j == 0))
			{
				lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
				lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
				lv_obj_set_size(label, 64, 36);
				lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
			}
			lv_obj_set_pos(label, 250 + 90 * j, 246 + 48 * i);
		}
	}
	setting_calendar_day_txt_display();
}

/***
** 日期: 2022-05-09 16:10
** 作者: leo.liu
** 函数作用：创建万年历
** 返回参数说明：
***/
static void setting_calendar_calendar_create(void)
{
	/***** 创建月份调整按钮 *****/
	setting_calendar_month_btn_create();
	/***** 创建年月日文本显示 *****/
	setting_calendar_year_month_day_txt_create();
	/***** 星期文本显示 *****/
	setting_calendar_week_txt_create();
	/***** 日历显示 *****/
	setting_calendar_day_txt_create();
}

static void LAYOUT_ENTER_FUNC(calendar)
{
	setting_cancel_btn_create(layout_calendar_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DATE));

	setting_clendar_seting_btn_create();

	setting_calendar_calendar_create();
}

static void LAYOUT_QUIT_FUNC(calendar)
{
}

CREATE_LAYOUT(calendar);