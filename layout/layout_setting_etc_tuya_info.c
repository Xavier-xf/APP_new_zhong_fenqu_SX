#include "layout_define.h"
#define LAYOUT_SETTING_TUYA_MODE_OBJ_ID_CONT 0X01
#define LAYOUT_SETTING_TUYA_SEND_TIME 0X02
// #define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_START_MIN 0X03
// #define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_HOUR 0X04
// #define LAYOUT_SETTING_NIGHT_MODE_OBJ_ID_END_MIN 0X07

static void setting_night_mode_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_etc));
}

static void setting_tuya_mode_btn_up(lv_obj_t *obj)
{
	bool state = (lv_checkbox_get_state(obj) & LV_STATE_CHECKED) ? true : false;

	user_data_get()->etc.tuya_send_switch = state;
	user_data_save();

	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_TUYA_MODE_OBJ_ID_CONT);
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
** 函数作用：涂鸦推送模式
** 返回参数说明：
***/

static void setting_tuya_mode_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_tuya_mode_btn_up);
	setting_sub_btn_base_create(NULL, 48, 70, 934, 88,
				     layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TUYA_SWITCH),
				    &click_data,
				    user_data_get()->etc.tuya_send_switch == 0 ? false : true,
				    3);
}

/***
** 日期: 2022-05-04 14:19
** 作者: fei.liu
** 函数作用：涂鸦推送时间回调
** 返回参数说明：
***/
static void setting_etc_tuya_send_time_btn_up(lv_obj_t *obj)
{
	
	lv_obj_t *parent = lv_obj_get_parent(obj);
	lv_obj_t *text = lv_obj_get_child_form_id(parent, LAYOUT_SETTING_TUYA_SEND_TIME);

	if(user_data_get()->etc.tuya_send_time < 25)
	{
		user_data_get()->etc.tuya_send_time += 5;
	}
	else
	{
		user_data_get()->etc.tuya_send_time = 0;
	}
	user_data_save();

	lv_label_set_text_fmt(text, "%ds", user_data_get()->etc.tuya_send_time);
}
/***
** 日期: 2022-05-05 16:29
** 作者: leo.liu
** 函数作用：设置涂鸦推送时间
** 返回参数说明：
***/
static bool tuya_select_of_send_time_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_etc_tuya_send_time_btn_up);
	lv_obj_t *btn = setting_right_btn_base_create(parent, 15, 0, 964, 88,
				      layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TUYA_SEND_TIME),
				      NULL,
				      &click_data,
				      LAYOUT_SETTING_TUYA_SEND_TIME);
    extern void setting_etc_door_open_time_btn_label_create(lv_obj_t *btn, int obj_id);
    setting_etc_door_open_time_btn_label_create(btn, LAYOUT_SETTING_TUYA_SEND_TIME);
    lv_obj_t *text = lv_obj_get_child_form_id(btn->parent, LAYOUT_SETTING_TUYA_SEND_TIME);
    if(text == NULL)
    {
        LOG_RED("obj not found \n");
    }
    lv_label_set_text_fmt(text, "%ds", user_data_get()->etc.tuya_send_time);
	return true;
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
	lv_obj_set_id(cont, LAYOUT_SETTING_TUYA_MODE_OBJ_ID_CONT);
	lv_obj_set_pos(cont, 30, 70 + 88 * 1);
	lv_obj_set_size(cont, 934, 288);

	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	static bool modidy = false;
	modidy = false;
	cont->user_data = &modidy;
	lv_obj_set_hidden(cont, user_data_get()->etc.tuya_send_switch == 0 ? true : false);
	return cont;
}



static void LAYOUT_ENTER_FUNC(tuya_info)
{
	setting_cancel_btn_create(setting_night_mode_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_VOLUME_LANG_ID_API_NIGHT));

	setting_tuya_mode_btn_create();

	lv_obj_t *cont = setting_night_mode_cont_create();

    tuya_select_of_send_time_btn_create(cont);
}

static void LAYOUT_QUIT_FUNC(tuya_info)
{

}

CREATE_LAYOUT(tuya_info);