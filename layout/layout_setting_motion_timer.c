#include "layout_define.h"

#define LAYOUT_SETTING_MOTION_TIMER_OBJ_CONT 0x01
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_START_YEAR 0X02
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_START_MON 0X03
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_START_DAY 0X04
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_START_HOUR 0X05
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_START_MIN 0X06
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_START_SEC 0X07

#define LAYOUT_SETTING_MOTION_TIMER_OBJ_END_YEAR 0X08
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_END_MON 0X09
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_END_DAY 0X0A
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_END_HOUR 0X0F
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_END_MIN 0X10
#define LAYOUT_SETTING_MOTION_TIMER_OBJ_END_SEC 0X11

static void motion_tmer_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_motion));
}

static void setting_motion_time_task(lv_task_t *task)
{
	lv_obj_t *label = (lv_obj_t *)task->user_data;

	struct tm tm;
	user_time_read(&tm);
	lv_label_set_text_fmt(label, "%04d-%02d-%02d %02d:%02d:%02d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

/***
** 日期: 2022-05-06 09:54
** 作者: leo.liu
** 函数作用：显示蓝色时间
** 返回参数说明：
***/
static bool setting_motion_timer_time_display(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	if (label == NULL)
	{
		return false;
	}
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 0, 78);
	lv_obj_set_size(label, LV_HOR_RES, 33);
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);

	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));

	lv_task_t *task = lv_layout_task_create(setting_motion_time_task, 500, LV_TASK_PRIO_MID, label);
	setting_motion_time_task(task);
	lv_task_ready(task);

	/***** 画一条线 *****/
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 0, 119);
	lv_obj_set_size(obj, LV_HOR_RES_MAX, 2);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x323237));
	lv_obj_set_click(obj, false);
	return true;
}

static void setting_motion_timer_enable_btn_up(lv_obj_t *obj)
{
	bool state = (lv_checkbox_get_state(obj) & LV_STATE_CHECKED) ? true : false;

	user_data_get()->motion.timer_en = state;
	user_data_save();

	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_TIMER_OBJ_CONT);
	if (cont == NULL)
	{
		printf("find LAYOUT_SETTING_MOTION_OBJ_CONT failed \n");
		return;
	}
	lv_obj_set_hidden(cont, !state);
}
/***
** 日期: 2022-05-06 10:12
** 作者: leo.liu
** 函数作用：创建定时器开关按钮
** 返回参数说明：
***/
static void setting_motion_timer_enable_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_motion_timer_enable_btn_up);
	setting_sub_btn_base_create(NULL, 30, 121, 934, 88,
				    layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_USE_TIMER),
				    &click_data,
				    user_data_get()->motion.timer_en, 3);
}

/***
** 日期: 2022-05-06 10:20
** 作者: leo.liu
** 函数作用：创建容器
** 返回参数说明：
***/
static lv_obj_t *setting_motion_timer_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	if (cont == NULL)
	{
		printf("motion detect setting cont create failed \n");
		return NULL;
	}
	lv_obj_set_id(cont, LAYOUT_SETTING_MOTION_TIMER_OBJ_CONT);
	lv_obj_set_pos(cont, 0, 195);
	lv_obj_set_size(cont, 934, 480);

	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	/***** 判定滚筒是否修改 *****/
	static bool modidy = false;
	modidy = false;
	cont->user_data = &modidy;

	lv_obj_set_hidden(cont, user_data_get()->motion.timer_en == true ? false : true);
	return cont;
}

/***
** 日期: 2022-05-06 10:26
** 作者: leo.liu
** 函数作用：创建年的滚筒
** 返回参数说明：
***/
static bool setting_motion_timer_year_rooler_create(lv_obj_t *parent)
{
	setting_time_rooler_base(parent, 157, 216 - 195, 100, 184, 2021, 2037, user_data_get()->motion.start.tm_year, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_YEAR);
	setting_time_rooler_base(parent, 157, 399 - 195, 100, 184, 2021, 2037, user_data_get()->motion.end.tm_year, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_YEAR);
	return true;
}

static bool setting_motion_timer_mon_rooler_create(lv_obj_t *parent)
{
	setting_time_rooler_base(parent, 284, 216 - 195, 100, 184, 1, 12, user_data_get()->motion.start.tm_mon, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_MON);
	setting_time_rooler_base(parent, 284, 399 - 195, 100, 184, 1, 12, user_data_get()->motion.end.tm_mon, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_MON);
	return true;
}

static bool setting_motion_timer_day_rooler_create(lv_obj_t *parent)
{
	setting_time_rooler_base(parent, 410, 216 - 195, 100, 184, 1, 31, user_data_get()->motion.start.tm_mday, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_DAY);
	setting_time_rooler_base(parent, 410, 399 - 195, 100, 184, 1, 31, user_data_get()->motion.end.tm_mday, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_DAY);
	return true;
}

static bool setting_motion_timer_hour_rooler_create(lv_obj_t *parent)
{
	setting_time_rooler_base(parent, 536, 216 - 195, 100, 184, 0, 23, user_data_get()->motion.start.tm_hour, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_HOUR);
	setting_time_rooler_base(parent, 536, 399 - 195, 100, 184, 0, 23, user_data_get()->motion.end.tm_hour, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_HOUR);
	return true;
}

static bool setting_motion_timer_min_rooler_create(lv_obj_t *parent)
{
	setting_time_rooler_base(parent, 663, 216 - 195, 100, 184, 0, 59, user_data_get()->motion.start.tm_min, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_MIN);
	setting_time_rooler_base(parent, 663, 399 - 195, 100, 184, 0, 59, user_data_get()->motion.end.tm_min, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_MIN);
	return true;
}

static bool setting_motion_timer_sec_rooler_create(lv_obj_t *parent)
{
	setting_time_rooler_base(parent, 789, 216 - 195, 100, 184, 0, 59, user_data_get()->motion.start.tm_sec, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_SEC);

	setting_time_rooler_base(parent, 789, 399 - 195, 100, 184, 0, 59, user_data_get()->motion.end.tm_sec, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_SEC);
	return true;
}

/***
** 日期: 2022-05-06 13:29
** 作者: leo.liu
** 函数作用：所需图标显示
** 返回参数说明：
***/
static void setting_motion_timer_icon_display(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 0, 240 - 195);
	lv_obj_set_size(obj, 157, 138);
	static rom_bin_info img_start = rom_bin_info_get(ROM_UI_IC_MOTION_TIMER_START_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_start);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_y(obj, 420 - 195);
	static rom_bin_info img_end = rom_bin_info_get(ROM_UI_IC_MOTION_TIMER_END_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_end);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_pos(obj, 0, 240 - 195 + 90);
	lv_obj_set_height(obj, 240 + 138 - 240);
	static rom_bin_info img_arrow = rom_bin_info_get(ROM_UI_IC_MOTION_TIMER_ARROW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_arrow);

	obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 202 + 40, 289 - 260 + 35 + 5);
	lv_obj_set_size(obj, 44, 90);
	static char string[2] = {"-"};
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 300 + 75);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_y(obj, 240 - 195 + 138 + 60 + 5);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 202 + 40);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_pos(obj, 300 + 75 + 250, 289 - 260 + 35 + 5);
	static char string_2[2] = {":"};
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string_2);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 300 + 75 + 370 + 5);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_y(obj, 240 - 195 + 138 + 60 + 5);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 300 + 75 + 250);
}

static void LAYOUT_ENTER_FUNC(setting_motion_timer)
{
	setting_cancel_btn_create(motion_tmer_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	setting_motion_timer_time_display();

	setting_motion_timer_enable_btn_create();

	lv_obj_t *cont = setting_motion_timer_cont_create();

	setting_motion_timer_year_rooler_create(cont);
	setting_motion_timer_mon_rooler_create(cont);
	setting_motion_timer_day_rooler_create(cont);
	setting_motion_timer_hour_rooler_create(cont);
	setting_motion_timer_min_rooler_create(cont);
	setting_motion_timer_sec_rooler_create(cont);

	setting_motion_timer_icon_display(cont);
}

static void LAYOUT_QUIT_FUNC(setting_motion_timer)
{
	/***** 返回判定是否有修改滚筒的值 *****/
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_TIMER_OBJ_CONT);
	bool modify = *((bool *)parent->user_data);
	if (modify == true)
	{
		/***** year *****/
		char buffer[8] = {0};
		lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_YEAR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.start.tm_year));

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_YEAR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.end.tm_year));

		/***** month *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_MON);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.start.tm_mon));

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_MON);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.end.tm_mon));

		/***** day *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_DAY);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.start.tm_mday));

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_DAY);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.end.tm_mday));

		/***** hour *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_HOUR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.start.tm_hour));

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_HOUR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.end.tm_hour));

		/***** min *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_MIN);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.start.tm_min));

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_MIN);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.end.tm_min));

		/***** sec *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_START_SEC);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.start.tm_sec));

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_MOTION_TIMER_OBJ_END_SEC);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(user_data_get()->motion.end.tm_sec));

		user_data_save();
	
	}
}

CREATE_LAYOUT(setting_motion_timer);