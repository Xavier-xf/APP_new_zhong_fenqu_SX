#include "layout_define.h"

#define LAYOUT_SETTING_RECORD_OBJ_AUTO_RECORD_ID 0X01

#define LAYOUT_SETTING_RECORD_OBJ_MSG_ID 0X02

#define LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID 0X04

/***
** 日期: 2022-04-28 17:20
** 作者: leo.liu
** 函数作用：获取auto_reocrd mode 的字符串
** 返回参数说明：
***/
static const char *setting_auto_record_mode_string_get(void)
{
	char auto_record_mode = user_data_get()->auto_record_mode;
	// if((auto_record_mode == 2) && (media_sdcard_insert_check() == false))
	// {
	// 	return language_common_string_get(LANG_COMMON_ID_PHOTO);
	// }

	return language_common_string_get(auto_record_mode == 0 ? LANG_COMMON_ID_OFF : auto_record_mode == 1 ? LANG_COMMON_ID_PHOTO
													     : LANG_COMMON_ID_VIDEO);
}

/***
** 日期: 2022-04-28 16:59
** 作者: leo.liu
** 函数作用：消息框按下确认键
** 返回参数说明：
***/
static void setting_msgdialog_confirm_btn_up(lv_obj_t *obj)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_RECORD_OBJ_MSG_ID);
	if (dialog == NULL)
	{
		printf("find obj dialog failed \n");
		return;
	}

	lv_obj_t *btnmatrix = lv_obj_get_child_form_id(dialog, LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID);
	if (btnmatrix == NULL)
	{
		printf("find btnmatrix failed \n");
	}

	for (int i = 0; i < 3; i++)
	{
		if (lv_btnmatrix_get_btn_ctrl(btnmatrix, i, LV_BTNMATRIX_CTRL_CHECK_STATE) == true)
		{
			if((i == 2) && (media_sdcard_insert_check() == false))
			{
				message_window_pops(layout_setting_ring_string_get(COMMON_NO_SD),0,0);
				// i = 0;
				return;
			}
			user_data_get()->auto_record_mode = i;
			user_data_save();
			break;
		}
	}

	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);

	/***** 重新更新auto record 类型 *****/
	lv_obj_t *sub_string_obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_RECORD_OBJ_AUTO_RECORD_ID);
	if (sub_string_obj == NULL)
	{
		printf("find auto record obj failed \n");
		return;
	}
	lv_obj_set_style_local_value_str(sub_string_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, setting_auto_record_mode_string_get());
}

/***
** 日期: 2022-04-28 16:59
** 作者: leo.liu
** 函数作用：消息框按下返回键
** 返回参数说明：
***/
static void setting_msgdialog_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);
}

/***
** 日期: 2022-04-28 14:53
** 作者: leo.liu
** 函数作用：auto record 消息框创建
** 返回参数说明：
***/
static bool setting_record_auto_msgdialog_create(void)
{
	lv_obj_t *cont = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(cont, setting_msgdialog_cancel_btn_up, setting_msgdialog_confirm_btn_up);

	static const char *string_map[] = {"", "\n", "", "\n", "", ""};
	string_map[0] = language_common_string_get(LANG_COMMON_ID_OFF);
	string_map[2] = language_common_string_get(LANG_COMMON_ID_PHOTO);
	string_map[4] = language_common_string_get(LANG_COMMON_ID_VIDEO);
	
	
	lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(cont, LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID, string_map);

	char auto_mode = user_data_get()->auto_record_mode;
	// if((auto_mode == 2) && (media_sdcard_insert_check() == false))
	// {
	// 	auto_mode = 1;
	// }

	lv_btnmatrix_set_btn_ctrl(btnmatrix, auto_mode, LV_BTNMATRIX_CTRL_CHECK_STATE);
	return true;
}

static void setting_record_auto_record_btn_up(lv_obj_t *obj)
{
	/***** 如果已经创建了，就不再执行 *****/
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_RECORD_OBJ_MSG_ID);
	if (parent != NULL)
	{
		return;
	}

	setting_record_auto_msgdialog_create();
}
/***
** 日期: 2022-04-28 14:39
** 作者: leo.liu
** 函数作用：auto record btn创建
** 返回参数说明：
***/
static void setting_record_auto_record_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_record_auto_record_btn_up);

	setting_right_btn_base_create(NULL, 353, 94+90*0, 622, 90,
				      layout_setting_record_string_get(SETTING_RECORD_LANG_ID_AUTO_RECORD),
				      setting_auto_record_mode_string_get(),
				      &click_data,
				      LAYOUT_SETTING_RECORD_OBJ_AUTO_RECORD_ID);
}

static void setting_record_motion_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_motion));
}

static void setting_always_msgdialog_confirm_btn_up(lv_obj_t *obj)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_RECORD_OBJ_MSG_ID);
	if (dialog == NULL)
	{
		printf("find obj dialog failed \n");
		return;
	}

	lv_obj_t *btnmatrix = lv_obj_get_child_form_id(dialog, LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID);
	if (btnmatrix == NULL)
	{
		printf("find btnmatrix failed \n");
	}

	int i = 0;
	for (i = 0; i < 3; i++)
	{
		if (lv_btnmatrix_get_btn_ctrl(btnmatrix, i, LV_BTNMATRIX_CTRL_CHECK_STATE) == true)
		{
			user_data_get()->always_recode_mode = i;
			user_data_save();
			break;
		}
	}
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);

	if ((i == 0) || (i == 1) || (i == 2))
	{
		extern void always_record_time_set(int sec);
		always_record_time_set(i == 0 ? 10 : i == 1 ? 30
							    : 60);
		goto_layout(pLAYOUT(always_record));
	}
}
/***
** 日期: 2022-04-28 14:53
** 作者: leo.liu
** 函数作用：auto record 消息框创建
** 返回参数说明：
***/
static bool setting_aways_msgdialog_create(void)
{
	lv_obj_t *cont = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(cont, setting_msgdialog_cancel_btn_up, setting_always_msgdialog_confirm_btn_up);

	static const char *string_map[] = {/*"", "\n",*/ "", "\n", "", "\n", "", ""};
	// string_map[0] = language_common_string_get(LANG_COMMON_ID_OFF);
	string_map[0] = layout_setting_record_string_get(SETTING_RECORD_LANG_ID_ALWAYS_10SEC);
	string_map[2] = layout_setting_record_string_get(SETTING_RECORD_LANG_ID_ALWAYS_30SEC);
	string_map[4] = layout_setting_record_string_get(SETTING_RECORD_LANG_ID_ALWAYS_60SEC);

	lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(cont, LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID, string_map);

	char auto_mode = user_data_get()->always_recode_mode;
	lv_btnmatrix_set_btn_ctrl(btnmatrix, auto_mode, LV_BTNMATRIX_CTRL_CHECK_STATE);
	// user_data_save();
	return true;
}
/***
** 日期: 2022-04-28 14:46
** 作者: leo.liu
** 函数作用：移动侦测设置按钮创建
** 返回参数说明：
***/
static void setting_record_motion_btn_create(void)
{
	char motion_enable = user_data_get()->motion.enable;
	static obj_click_data click_data = obj_click_data_up_create(setting_record_motion_btn_up);
	setting_right_btn_base_create(NULL, 353, 94+90*1, 622, 90, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_MOTION_DETECTION),
				      language_common_string_get(motion_enable ? LANG_COMMON_ID_ON : LANG_COMMON_ID_OFF),
				      &click_data, 0);
}
static void setting_record_aways_btn_up(lv_obj_t *obj)
{
	if (intercom_data_busy_get() == true)
	{
		return;
	}
	/***** 如果已经创建了，就不再执行 *****/
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_RECORD_OBJ_MSG_ID);
	if (parent != NULL)
	{
		return;
	}

	setting_aways_msgdialog_create();
}

/***
**   日期:2022-06-20 14:05:36
**   作者: leo.liu
**   函数作用：一直记录
**   参数说明:
***/
static void setting_record_aways_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_record_aways_btn_up);
	setting_right_btn_base_create(NULL, 353, 94+90*2, 622, 90, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_ALWAYS),
				      NULL, &click_data, 0);
	
}
#if 0
// static void setting_record_wifi_btn_up(lv_obj_t *obj)
// {
// 	goto_layout(pLAYOUT(wifi_display));
// }

/***
** 日期: 2022-04-28 14:49
** 作者: leo.liu
** 函数作用：wifi设置按钮
** 返回参数说明：
***/
// static void setting_record_wifi_btn_create(void)
// {
// 	static obj_click_data click_data = obj_click_data_up_create(setting_record_wifi_btn_up);
// 	setting_right_btn_base_create(NULL, 353, 364, 622, 90, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_WIFI),
// 				      NULL, &click_data, 0);
// }
#endif







// static void layout_door1_always_enable_btn_up(lv_obj_t *obj)
// {
// 	bool state = lv_checkbox_is_checked(obj);
// 	user_data_get()->door1_always_enable = state;
// 	user_data_save();
// 	if (state)
// 	{
// 		door1_power_enable(true);
// 	}
// 	else
// 	{
// 		door1_power_enable(false);
// 	} 
// 	// audio_to_outdoor1_pin_ctrl(true);
// }

// /*************************************************************************
//  * @brief  door1使能按钮和开关
//  * @date   2022-08-26 13:54
//  * @author xiaoele
//  **************************************************************************/
// static bool layout_door1_always_enable_btn_create(void)
// {
// 	static obj_click_data click_data = obj_click_data_up_create(layout_door1_always_enable_btn_up);
// 	setting_sub_btn_base_create(NULL, 353, 94, 622, 90,
// 				    layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DOOR1_ALWAYS),
// 				    &click_data,
// 				    user_data_get()->door1_always_enable, 3);
// 	return true;
// }




// static void layout_door2_always_enable_btn_up(lv_obj_t *obj)
// {
// 	bool state = lv_checkbox_is_checked(obj);
// 	user_data_get()->door2_always_enable = state;
// 	user_data_save();
// 	if (state)
// 	{
// 		door2_power_enable(true);
// 	}
// 	else
// 	{
// 		door2_power_enable(false);
// 	}
// 	// audio_to_outdoor2_pin_ctrl(true);
// }

// /*************************************************************************
//  * @brief  door2使能按钮和开关
//  * @date   2022-08-26 13:54
//  * @author xiaoele
//  **************************************************************************/
// static bool layout_door2_always_enable_btn_create(void)
// {
// 	static obj_click_data click_data = obj_click_data_up_create(layout_door2_always_enable_btn_up);
// 	setting_sub_btn_base_create(NULL, 353, 94+90, 622, 90,
// 				    layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DOOR2_ALWAYS),
// 				    &click_data,
// 				    user_data_get()->door2_always_enable, 3);
// 	return true;
// }


static void setting_record_sd_card_state_change_cb(void)
{
	// if(media_sdcard_insert_check() == true)
	// {
	// 	copy_flash_photo_to_sd();
	// }
	if((media_sdcard_insert_check() == false) && (user_data_get()->auto_record_mode == 2)) 
	{
		goto_layout(pLAYOUT(setting_record));
	}

}

static void LAYOUT_ENTER_FUNC(setting_record)
{
	if((media_sdcard_insert_check() == false) && (user_data_get()->auto_record_mode == 2)) {
		user_data_get()->auto_record_mode = 1;
		user_data_save();
	}
	setting_cancel_btn_create(NULL);
	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));
	setting_main_btn_create(0);

	// layout_door1_always_enable_btn_create();
	// layout_door2_always_enable_btn_create();
	setting_record_auto_record_btn_create();
	
	if(OwnID == 1)
	{
		setting_record_motion_btn_create();
		setting_record_aways_btn_create();
	}
	layout_sd_state_callback_register(setting_record_sd_card_state_change_cb);
	// setting_record_wifi_btn_create();
}

static void LAYOUT_QUIT_FUNC(setting_record)
{
	layout_sd_state_callback_register(NULL);
}

CREATE_LAYOUT(setting_record);