#include "layout_define.h"

#define LAYOUT_SETTING_FRAME_LIST_OBJ_ID_TIME 0X01
#define LAYOUT_SETTING_FRAME_LIST_OBJ_ID_CALENDAR 0X02
#define LAYOUT_SETTING_FRAME_LIST_OBJ_ID_PVIEW 0X03
#define LAYOUT_SETTING_FRAME_LIST_OBJ_ID_DOOR1 0x04
#define LAYOUT_SETTING_FRAME_LIST_OBJ_ID_DOOR2 0X05

static void setting_frame_list_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_frame));
}

static void setting_frame_list_time_btn_up(lv_obj_t *obj)
{
}
static obj_click_data setting_frame_list_click_data = obj_click_data_up_create(setting_frame_list_time_btn_up);

/***
** 日期: 2022-05-07 08:31
** 作者: leo.liu
** 函数作用：时间
** 返回参数说明：
***/
static void setting_frame_list_time_btn_create(void)
{

	lv_obj_t *obj = setting_sub_btn_base_create(NULL, 30, 70, 934, 88,
						    layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_TIME),
						    &setting_frame_list_click_data,
						    user_data_get()->display.frame_list & 0x01 ? true : false,
						    4);
	lv_obj_set_id(obj, LAYOUT_SETTING_FRAME_LIST_OBJ_ID_TIME);
}

/***
** 日期: 2022-05-07 08:31
** 作者: leo.liu
** 函数作用：日历
** 返回参数说明：
***/
static void setting_frame_list_calendar_btn_create(void)
{
	lv_obj_t *obj = setting_sub_btn_base_create(NULL, 30, 70 + 88, 934, 88,
						    layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_CALENDAR),
						    &setting_frame_list_click_data,
						    user_data_get()->display.frame_list & 0x02 ? true : false,
						    4);
	lv_obj_set_id(obj, LAYOUT_SETTING_FRAME_LIST_OBJ_ID_CALENDAR);
}

/***
** 日期: 2022-05-07 08:32
** 作者: leo.liu
** 函数作用：预览
** 返回参数说明：
***/
static void setting_frame_list_playback_btn_create(void)
{
	lv_obj_t *obj = setting_sub_btn_base_create(NULL, 30, 70 + 88 * 2, 934, 88,
						    layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_PLAYBACK),
						    &setting_frame_list_click_data,
						    user_data_get()->display.frame_list & 0x04 ? true : false,
						    4);
	lv_obj_set_id(obj, LAYOUT_SETTING_FRAME_LIST_OBJ_ID_PVIEW);
}

/***
** 日期: 2022-05-07 08:33
** 作者: leo.liu
** 函数作用：door1&cctv1
** 返回参数说明：
***/
// static void setting_frame_list_door1_btn_create(void)
// {
// 	lv_obj_t *obj = setting_sub_btn_base_create(NULL, 30, 70 + 88 * 3, 934, 88,
// 						    layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_DOOR1_CCTV1),
// 						    &setting_frame_list_click_data,
// 						    user_data_get()->display.frame_list & 0x08 ? true : false,
// 						    4);
// 	lv_obj_set_id(obj, LAYOUT_SETTING_FRAME_LIST_OBJ_ID_DOOR1);
// }

/***
** 日期: 2022-05-07 08:33
** 作者: leo.liu
** 函数作用：door2&cctv2
** 返回参数说明：
***/
// static void setting_frame_list_door2_btn_create(void)
// {
// 	lv_obj_t *obj = setting_sub_btn_base_create(NULL, 30, 70 + 88 * 4, 934, 88,
// 						    layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_DOOR2_CCTV2),
// 						    &setting_frame_list_click_data,
// 						    user_data_get()->display.frame_list & 0x10 ? true : false,
// 						    4);
// 	lv_obj_set_id(obj, LAYOUT_SETTING_FRAME_LIST_OBJ_ID_DOOR2);
// }

static void LAYOUT_ENTER_FUNC(setting_frame_list)
{
	setting_cancel_btn_create(setting_frame_list_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_VOLUME_LANG_ID_API_LISE_OF_DISPLAY));

	setting_frame_list_time_btn_create();
	setting_frame_list_calendar_btn_create();
	setting_frame_list_playback_btn_create();
	// if(OwnID == 1)
	// {
		// setting_frame_list_door1_btn_create();
		// setting_frame_list_door2_btn_create();
	// }
	
}

static void LAYOUT_QUIT_FUNC(setting_frame_list)
{
	/***** 退出后统一保存 *****/
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_LIST_OBJ_ID_TIME);
	int data = user_data_get()->display.frame_list;
	if (lv_checkbox_get_state(obj) == LV_BTN_STATE_CHECKED_RELEASED)
	{
		data |= 0x01;
	}
	else
	{
		data &= 0x1E;
	}

	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_LIST_OBJ_ID_CALENDAR);
	if (lv_checkbox_get_state(obj) == LV_BTN_STATE_CHECKED_RELEASED)
	{
		data |= 0x02;
	}
	else
	{
		data &= 0x1D;
	}

	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_LIST_OBJ_ID_PVIEW);
	if (lv_checkbox_get_state(obj) == LV_BTN_STATE_CHECKED_RELEASED)
	{
		data |= 0x04;
	}
	else
	{
		data &= 0x1B;
	}
	// if(OwnID == 1)
	// {
	// 	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_LIST_OBJ_ID_DOOR1);
	// 	if (lv_checkbox_get_state(obj) == LV_BTN_STATE_CHECKED_RELEASED)
	// 	{
	// 		data |= 0x08;
	// 	}
	// 	else
	// 	{
	// 		data &= 0x17;
	// 	}

	// 	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_LIST_OBJ_ID_DOOR2);
	// 	if (lv_checkbox_get_state(obj) == LV_BTN_STATE_CHECKED_RELEASED)
	// 	{
	// 		data |= 0x10;
	// 	}
	// 	else
	// 	{
	// 		data &= 0x0F;
	// 	}
	// }
	

	user_data_get()->display.frame_list = data;
	user_data_save();
}

CREATE_LAYOUT(setting_frame_list);
