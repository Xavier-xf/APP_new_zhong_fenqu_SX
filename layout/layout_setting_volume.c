#include "layout_define.h"
#include "ringplay.h"

#define LAYOUT_SETTING_VOLUME_OBJ_TOUCH_TONE_ID 0X01
#define LAYOUT_SETTING_VOLUME_OBJ_DOOR1_TONE_ID 0X02
#define LAYOUT_SETTING_VOLUME_OBJ_DOOR2_TONE_ID 0X03
#define LAYOUT_SETTING_VOLUME_OBJ_INTER_TONE_ID 0X04

extern bool set_door_ring_flag;
static void setting_volume_door1_btn_up(lv_obj_t *obj)
{
	set_door_ring_flag = 0;
	goto_layout(pLAYOUT(setting_volume_vol));
}

/***
** 日期: 2022-04-29 08:19
** 作者: leo.liu
** 函数作用：创建volme 音量设置按钮
** 返回参数说明：
***/
static bool setting_volume_door1_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_volume_door1_btn_up);
	setting_right_btn_base_create(NULL, 353, 94, 622, 90,
				      layout_setting_record_string_get(SETTING_VOLUME_LANG_ID_DOOR1),
				      NULL,
				      &click_data,
				      0);
	return true;
}

static void setting_volume_touch_tone_btn_up(lv_obj_t *obj)
{

	lv_obj_t *touch_obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_VOLUME_OBJ_TOUCH_TONE_ID);
	if (touch_obj == NULL)
	{
		printf("find touch obj failed \n");
		return;
	}

	bool en = user_data_get()->audio.key_sound ? false : true;
	lv_obj_set_style_local_value_str(touch_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, language_common_string_get(en ? LANG_COMMON_ID_ON : LANG_COMMON_ID_OFF));

	user_data_get()->audio.key_sound = en;
	user_data_save();

	ringplay_touchsound_mute_set(en ? false : true);
}
/***
** 日期: 2022-04-29 08:19
** 作者: leo.liu
** 函数作用：触摸音使能开关按钮创建
** 返回参数说明：
***/
static bool setting_volume_touch_tone_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_volume_touch_tone_btn_up);
	setting_right_btn_base_create(NULL, 353, 94+90*2, 622, 90, 
				      layout_setting_volume_string_get(SETTING_VOLUME_LANG_ID_TOUCH_TONE),
				      language_common_string_get(user_data_get()->audio.key_sound == true ? LANG_COMMON_ID_ON : LANG_COMMON_ID_OFF),
				      &click_data,
				      LAYOUT_SETTING_VOLUME_OBJ_TOUCH_TONE_ID);
	return true;
}

/***
** 日期: 2022-04-29 08:35
** 作者: leo.liu
** 函数作用：获取door铃声状态字符串
** 返回参数说明：
***/
const char *setting_volume_door_tone_status_string_get(int door)
{

	int index = door == 1 ? user_data_get()->audio.door1_tone : door == 2 ? user_data_get()->audio.door2_tone
									      : user_data_get()->audio.inter_tone;
	if ((index < 1) || (index > 6))
	{
		printf("door1 tone index failed \n");
		return NULL;
	}

	return layout_setting_volume_string_get(SETTING_VOLUME_LANG_ID_TONE1 + index - 1);
}

static void setting_volume_door2_btn_up(lv_obj_t *obj)
{
	set_door_ring_flag = 1;
	goto_layout(pLAYOUT(setting_volume_vol));
}
/***
** 日期: 2022-04-29 08:20
** 作者: leo.liu
** 函数作用：door1 铃声按钮创建
** 返回参数说明：
***/
static bool setting_volume_door2_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_volume_door2_btn_up);
	setting_right_btn_base_create(NULL, 353, 94+90, 622, 90,
				      layout_setting_record_string_get(SETTING_VOLUME_LANG_ID_DOOR2),
				      NULL,
				      &click_data,
				      0);
	return true;
}

// static void setting_volume_door2_tone_btn_up(lv_obj_t *obj)
// {
// 	lv_obj_t *door2_obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_VOLUME_OBJ_DOOR2_TONE_ID);
// 	if (door2_obj == NULL)
// 	{
// 		printf("find door2 obj failed \n");
// 		return;
// 	}

// 	user_data_get()->audio.door2_tone++;
// 	if (user_data_get()->audio.door2_tone > 6)
// 	{
// 		user_data_get()->audio.door2_tone = 1;
// 	}
// 	lv_obj_set_style_local_value_str(door2_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, setting_volume_door_tone_status_string_get(2));

// 	user_data_save();

// 	ringplay_play_form_index(user_data_get()->audio.door2_tone, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
// }
/***
** 日期: 2022-04-29 08:20
** 作者: leo.liu
** 函数作用：door2铃声按钮创建
** 返回参数说明：
***/
// static bool setting_volume_door2_tone_btn_create(void)
// {
// 	static obj_click_data click_data = obj_click_data_up_create(setting_volume_door2_tone_btn_up);
// 	setting_right_btn_base_create(NULL, 353, 364, 622, 90,
// 				      layout_setting_volume_string_get(SETTING_VOLUME_LANG_ID_DOOR2_TONE),
// 				      setting_volume_door_tone_status_string_get(2),
// 				      &click_data,
// 				      LAYOUT_SETTING_VOLUME_OBJ_DOOR2_TONE_ID);
// 	return true;
// }

/***
**   日期:2022-06-18 10:02:47
**   作者: leo.liu
**   函数作用：intercom
**   参数说明:
***/
static void setting_volume_intercom_tone_btn_up(lv_obj_t *obj)
{
	// obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_VOLUME_OBJ_INTER_TONE_ID);
	// if (obj == NULL)
	// {
	// 	printf("find intercom obj failed \n");  
	// 	return;
	// }

	// user_data_get()->audio.inter_tone++;
	// if (user_data_get()->audio.inter_tone > 6)
	// {
	// 	user_data_get()->audio.inter_tone = 1;
	// }
	// lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, setting_volume_door_tone_status_string_get(3));

	// user_data_save();

	// ringplay_play_form_index(user_data_get()->audio.inter_tone, 100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, false);
	goto_layout(pLAYOUT(intercom_ring));
}

/***
** 日期: 2022-04-29 08:20
** 作者: leo.liu
** 函数作用：intercom铃声按钮创建
** 返回参数说明：
***/

static bool setting_volume_intercom_tone_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_volume_intercom_tone_btn_up);
	setting_right_btn_base_create(NULL,  353, 364, 622, 90,
				      layout_setting_volume_string_get(SETTING_VOLUME_LANG_ID_INTERPHONE_TONE),
				      NULL,
				      &click_data,
				      LAYOUT_SETTING_VOLUME_OBJ_INTER_TONE_ID);
	return true;
}

static void LAYOUT_ENTER_FUNC(setting_volume)
{
	setting_cancel_btn_create(NULL);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	setting_main_btn_create(1);

	setting_volume_door1_btn_create();
	setting_volume_door2_btn_create();
	setting_volume_touch_tone_btn_create();
	// setting_volume_door2_tone_btn_create();

	setting_volume_intercom_tone_btn_create();

}

static void LAYOUT_QUIT_FUNC(setting_volume)
{
}

CREATE_LAYOUT(setting_volume);