#include "layout_define.h"

#define LAYOUT_SETTING_TIME_OBJ_CONT 0X01

#define LAYOUT_SETTING_TIME_OBJ_YEAR 0X02
#define LAYOUT_SETTING_TIME_OBJ_MON 0X03
#define LAYOUT_SETTING_TIME_OBJ_DAY 0X04
#define LAYOUT_SETTING_TIME_OBJ_HOUR 0X05
#define LAYOUT_SETTING_TIME_OBJ_MIN 0X06
#define LAYOUT_SETTING_TIME_OBJ_SEC 0X07
/***
** 日期: 2022-05-09 15:54
** 作者: leo.liu
** 函数作用：进入日历有两个地方进入，功能需求是要求从哪个界面进入，则返回到某界面，
** 返回参数说明：0x01:home 进入 ，0x02：set页面进入
***/
static char setting_time_enter_flag = 0x00;
void setting_time_enter_flag_set(char flag)
{
	setting_time_enter_flag = flag;
}

/***
** 日期: 2022-05-10 08:01
** 作者: leo.liu
** 函数作用：返回按钮事件
** 返回参数说明：
***/
static void setting_time_cancel_btn_up(lv_obj_t *obj)
{
	if (setting_time_enter_flag == 0x02)
	{
		goto_layout(pLAYOUT(setting_etc));
	}
	else if (setting_time_enter_flag == 0x01)
	{
		goto_layout(pLAYOUT(calendar));
	}
}

/***
** 日期: 2022-05-06 10:20
** 作者: leo.liu
** 函数作用：创建容器
** 返回参数说明：
***/
static lv_obj_t *setting_time_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	if (cont == NULL)
	{
		printf("time setting cont create failed \n");
		return NULL;
	}
	lv_obj_set_id(cont, LAYOUT_SETTING_TIME_OBJ_CONT);
	lv_obj_set_pos(cont, 0, 216);
	lv_obj_set_size(cont, 1024, 200);

	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	/***** 判定滚筒是否修改 *****/
	static bool modidy = false;
	modidy = false;
	cont->user_data = &modidy;
	return cont;
}

/***
** 日期: 2022-05-06 10:26
** 作者: leo.liu
** 函数作用：创建年的滚筒
** 返回参数说明：
***/
static bool setting_time_year_rooler_create(lv_obj_t *parent, struct tm *tm)
{
	setting_time_rooler_big_base(parent, 110, 135 - 135, 90, 200, 2021, 2037, tm->tm_year, LAYOUT_SETTING_TIME_OBJ_YEAR);
	return true;
}

static bool setting_time_mon_rooler_create(lv_obj_t *parent, struct tm *tm)
{
	setting_time_rooler_big_base(parent, 248, 135 - 135, 90, 200, 1, 12, tm->tm_mon, LAYOUT_SETTING_TIME_OBJ_MON);
	return true;
}

static bool setting_time_day_rooler_create(lv_obj_t *parent, struct tm *tm)
{
	setting_time_rooler_big_base(parent, 385, 135 - 135, 90, 200, 1, 31, tm->tm_mday, LAYOUT_SETTING_TIME_OBJ_DAY);
	return true;
}

static bool setting_time_hour_rooler_create(lv_obj_t *parent, struct tm *tm)
{
	setting_time_rooler_big_base(parent, 536, 135 - 135, 90, 200, 0, 23, tm->tm_hour, LAYOUT_SETTING_TIME_OBJ_HOUR);
	return true;
}

static bool setting_time_min_rooler_create(lv_obj_t *parent, struct tm *tm)
{
	setting_time_rooler_big_base(parent, 673, 135 - 135, 90, 200, 0, 59, tm->tm_min, LAYOUT_SETTING_TIME_OBJ_MIN);
	return true;
}

static bool setting_time_sec_rooler_create(lv_obj_t *parent, struct tm *tm)
{
	setting_time_rooler_big_base(parent, 811, 135 - 135, 90, 200, 0, 59, tm->tm_sec, LAYOUT_SETTING_TIME_OBJ_SEC);
	return true;
}

/***
** 日期: 2022-05-06 13:29
** 作者: leo.liu
** 函数作用：所需图标显示
** 返回参数说明：
***/
static void setting_time_icon_display(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 110 + 5 + 80, 135 - 135 + 50);
	lv_obj_set_size(obj, 44, 90);
	static char string[2] = {"-"};
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 248 + 5 + 80);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_pos(obj, 536 + 5 + 80, 135 - 135 + 50);
	static char string_2[2] = {":"};
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string_2);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 673 + 5 + 80);
}
static void LAYOUT_ENTER_FUNC(setting_time)
{
	setting_cancel_btn_create(setting_time_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DATE));

	lv_obj_t *cont = setting_time_cont_create();

	struct tm tm;
	user_time_read(&tm);

	setting_time_year_rooler_create(cont, &tm);
	setting_time_mon_rooler_create(cont, &tm);
	setting_time_day_rooler_create(cont, &tm);
	setting_time_hour_rooler_create(cont, &tm);
	setting_time_min_rooler_create(cont, &tm);
	setting_time_sec_rooler_create(cont, &tm);

	setting_time_icon_display(cont);
}
static void LAYOUT_QUIT_FUNC(setting_time)
{
	/***** 返回判定是否有修改滚筒的值 *****/
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_TIME_OBJ_CONT);
	bool modify = *((bool *)parent->user_data);
	if (modify == true)
	{
		struct tm tm;
		/***** year *****/
		char buffer[8] = {0};
		lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_TIME_OBJ_YEAR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(tm.tm_year));
		/***** month *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_TIME_OBJ_MON);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(tm.tm_mon));

		/***** day *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_TIME_OBJ_DAY);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(tm.tm_mday));

		/***** hour *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_TIME_OBJ_HOUR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(tm.tm_hour));

		/***** min *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_TIME_OBJ_MIN);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(tm.tm_min));

		/***** sec *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_TIME_OBJ_SEC);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &(tm.tm_sec));

		
		standby_timer_close();
		user_time_set(&tm);
		intercom_user_time_sync_send(CODE_ALL_ID,&tm);
		standby_timer_restart(true);
	}
}

CREATE_LAYOUT(setting_time);