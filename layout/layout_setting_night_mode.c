#include "layout_define.h"
#define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_CONT 0X01
#define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_START_HOUR 0X02
#define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_START_MIN 0X03
#define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_HOUR 0X04
#define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_MIN 0X07

static void setting_night_mode_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_frame));
}

static void setting_night_mode_btn_up(lv_obj_t *obj)
{
	bool state = (lv_checkbox_get_state(obj) & LV_STATE_CHECKED) ? true : false;

	user_data_get()->display.night_mode = state;
	user_data_save();

	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_CONT);
	if (cont == NULL)
	{
		printf("find LAYOUT_SETTING_MOTION_OBJ_CONT failed \n");
		return;
	}
	lv_obj_set_hidden(cont, !state);
}
/***
** 日期: 2022-05-06 17:16
** 作者: leo.liu
** 函数作用：夜间模式
** 返回参数说明：
***/

static void setting_night_mode_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_night_mode_btn_up);
	setting_sub_btn_base_create(NULL, 48, 70, 934, 88,
				    layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_NIGHT_MODE),
				    &click_data,
				    user_data_get()->display.night_mode == 0 ? false : true,
				    3);
}

/***
** 日期: 2022-05-05 16:23
** 作者: leo.liu
** 函数作用：frame time设置的容器
** 返回参数说明：
***/
static lv_obj_t *setting_night_mode_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	if (cont == NULL)
	{
		printf("motion detect setting cont create failed \n");
		return NULL;
	}
	lv_obj_set_id(cont, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_CONT);
	lv_obj_set_pos(cont, 30, 70 + 88 * 1);
	lv_obj_set_size(cont, 934, 288);

	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	static bool modidy = false;
	modidy = false;
	cont->user_data = &modidy;
	lv_obj_set_hidden(cont, user_data_get()->display.night_mode == 0 ? true : false);
	return cont;
}

/***
** 日期: 2022-05-06 17:24
** 作者: leo.liu
** 函数作用：创建时间显示滚筒
** 返回参数说明：
***/
static void setting_night_mode_roller_create(lv_obj_t *parent)
{
	setting_time_rooler_base(parent, 224, 100, 100, 184, 0, 23, user_data_get()->display.night_time_start / 60, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_START_HOUR);
	setting_time_rooler_base(parent, 350, 100, 100, 184, 0, 59, user_data_get()->display.night_time_start % 60, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_START_MIN);

	setting_time_rooler_base(parent, 597, 100, 100, 184, 0, 23, user_data_get()->display.night_time_end / 60, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_HOUR);
	setting_time_rooler_base(parent, 724, 100, 100, 184, 0, 59, user_data_get()->display.night_time_end % 60, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_MIN);
}

/***
** 日期: 2022-05-06 13:29
** 作者: leo.liu
** 函数作用：所需图标显示
** 返回参数说明：
***/
static void setting_night_mode_text_display(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 224 - 80, 140);
	lv_obj_set_size(obj, 48, 100);
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_START));

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 597 - 80);
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_END));

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 4320);
	lv_obj_set_width(obj, 35);
	static char string[2] = {":"};
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);

	obj = lv_obj_create(parent, obj);
	lv_obj_set_x(obj, 690);
}
static void LAYOUT_ENTER_FUNC(setting_night_mode)
{
	setting_cancel_btn_create(setting_night_mode_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_VOLUME_LANG_ID_API_NIGHT));

	setting_night_mode_btn_create();

	lv_obj_t *cont = setting_night_mode_cont_create();

	setting_night_mode_roller_create(cont);

	setting_night_mode_text_display(cont);
}

static void LAYOUT_QUIT_FUNC(setting_night_mode)
{
	/***** 返回判定是否有修改滚筒的值 *****/
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_CONT);
	bool modify = *((bool *)parent->user_data);
	if (modify == true)
	{
		int hour = 0, min = 0;
		/***** year *****/
		char buffer[8] = {0};
		lv_obj_t *obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_START_HOUR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &hour);

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_START_MIN);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &min);

		user_data_get()->display.night_time_start = hour * 60 + min;

		/***** month *****/
		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_HOUR);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &hour);

		obj = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_MIN);
		lv_roller_get_selected_str(obj, buffer, 8);
		sscanf(buffer, "%d", &min);
		user_data_get()->display.night_time_end = hour * 60 + min;

		user_data_save();
	}
}

CREATE_LAYOUT(setting_night_mode);
