#include "layout_define.h"

#define LAYOUT_SETTING_MOTION_OBJ_CONT 0X01
#define LAYOUT_SETTING_MOTION_OBJ_SELECT_CAMERA 0X02
#define LAYOUT_SETTING_MOTION_OBJ_SAVE_FMT 0X03
#define LAYOUT_SETTING_MOTION_OBJ_SENSIVITY 0X04
#define LAYOUT_SETTING_MOTION_OBJ_LCD 0X06
#define LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG 0X07
#define LAYOUT_SETTING_MOTION_OBJ_BTNMATRIX 0X09
static void motion_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_record));
}

static void motion_use_motion_detect_btn_up(lv_obj_t *obj)
{
	// printf("%d \n",lv_checkbox_get_state(obj));
	bool state = (lv_checkbox_get_state(obj) & LV_STATE_CHECKED) ? true : false;

	user_data_get()->motion.enable = state;
	user_data_save();

	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_CONT);
	if (cont == NULL)
	{
		printf("find LAYOUT_SETTING_MOTION_OBJ_CONT failed \n");
		return;
	}
	lv_obj_set_hidden(cont, !state);
	lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_AUTO_MONITOR_UPDATE,  0);
}

/***
** 日期: 2022-05-05 16:23
** 作者: leo.liu
** 函数作用：移动侦测使能按钮创建
** 返回参数说明：
***/
static bool motion_use_motion_detect_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(motion_use_motion_detect_btn_up);
	setting_sub_btn_base_create(NULL, 30, 70, 964, 88,
				    layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_USE_MOTION_DETECT),
				    &click_data,
				    user_data_get()->motion.enable, 3);
	return true;
}

/***
** 日期: 2022-05-05 17:07
** 作者: leo.liu
** 函数作用：获取camera 字符串
** 返回参数说明：
***/
static const char *motion_select_of_camera_string_get_and_save(bool save, int value)
{
	// if((value == 1) && (media_sdcard_insert_check() == false))
	// {
	// 	value = 0;
	// }
	if (save == true)
	{
		user_data_get()->motion.select_camera = value;
		user_data_save();
	}

	return language_common_string_get(user_data_get()->motion.select_camera + LANG_COMMON_ID_DOOR1);
}

/***
** 日期: 2022-05-06 08:02
** 作者: leo.liu
** 函数作用：消息框返回按钮事件
** 返回参数说明：
***/
static void setting_motion_dialog_cancel_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);
}

/***
** 日期: 2022-05-06 08:30
** 作者: leo.liu
** 函数作用：消息框确认按钮事件的基础函数
** 返回参数说明：
***/
static int setting_motion_dialog_confirm_sub_up(int btnmatrix_num, int sub_id, const char *(*string_get)(bool, int))
{
	int value = 0;
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);
	if (dialog == NULL)
	{
		printf("find obj dialog failed \n");
		return -1;
	}

	lv_obj_t *btnmatrix = lv_obj_get_child_form_id(dialog, LAYOUT_SETTING_MOTION_OBJ_BTNMATRIX);
	if (btnmatrix == NULL)
	{
		printf("find btnmatrix failed \n");
	}

	for (int i = 0; i < btnmatrix_num; i++)
	{
		if (lv_btnmatrix_get_btn_ctrl(btnmatrix, i, LV_BTNMATRIX_CTRL_CHECK_STATE) == true)
		{
			// if((btnmatrix_num == 2) && (i == 1) && (media_sdcard_insert_check() == false))
			// {
			// 	return false;
			// }
			value = i;
			break;
		}
	}

	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);

	/***** 重新更新auto record 类型 *****/
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_CONT);
	lv_obj_t *sub_string_obj = lv_obj_get_child_form_id(cont, sub_id);
	if (sub_string_obj == NULL)
	{
		printf("find auto record obj failed \n");
		return -1;
	}
	lv_obj_set_style_local_value_str(sub_string_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string_get(true, value));

	return value;
}

static int setting_motion_format_dialog_confirm_sub_up(int btnmatrix_num, int sub_id, const char *(*string_get)(bool, int))
{
	int value = 0;
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);
	if (dialog == NULL)
	{
		printf("find obj dialog failed \n");
		return -1;
	}

	lv_obj_t *btnmatrix = lv_obj_get_child_form_id(dialog, LAYOUT_SETTING_MOTION_OBJ_BTNMATRIX);
	if (btnmatrix == NULL)
	{
		printf("find btnmatrix failed \n");
	}

	for (int i = 0; i < btnmatrix_num; i++)
	{
		if (lv_btnmatrix_get_btn_ctrl(btnmatrix, i, LV_BTNMATRIX_CTRL_CHECK_STATE) == true)
		{
			if((btnmatrix_num == 2) && (i == 1) && (media_sdcard_insert_check() == false))
			{
				return false;
			}
			value = i;
			break;
		}
	}

	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);

	/***** 重新更新auto record 类型 *****/
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_CONT);
	lv_obj_t *sub_string_obj = lv_obj_get_child_form_id(cont, sub_id);
	if (sub_string_obj == NULL)
	{
		printf("find auto record obj failed \n");
		return -1;
	}
	lv_obj_set_style_local_value_str(sub_string_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string_get(true, value));

	return value;
}

/***
** 日期: 2022-05-06 08:02
** 作者: leo.liu
** 函数作用：选中通道消息框确认按钮按下
** 返回参数说明：
***/
static void setting_motion_camera_dialog_confirm_up(lv_obj_t *obj)
{
	setting_motion_dialog_confirm_sub_up(2, LAYOUT_SETTING_MOTION_OBJ_SELECT_CAMERA, motion_select_of_camera_string_get_and_save);
}

static void motion_select_of_camera_btn_up(lv_obj_t *obj)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);
	if (dialog != NULL)
	{
		return;
	}
	dialog = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(dialog, setting_motion_dialog_cancel_up, setting_motion_camera_dialog_confirm_up);

	static const char *string_map[] = {"1", "\n", "2", ""};
	string_map[0] = language_common_string_get(LANG_COMMON_ID_DOOR1);
	string_map[2] = language_common_string_get(LANG_COMMON_ID_DOOR2);
	// string_map[4] = language_common_string_get(LANG_COMMON_ID_CCTV1);
	// string_map[6] = language_common_string_get(LANG_COMMON_ID_CCTV2);

	lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(dialog, LAYOUT_SETTING_MOTION_OBJ_BTNMATRIX, string_map);
	lv_obj_set_y(btnmatrix, 50);
	lv_obj_set_height(btnmatrix, 180);

	char select_camera = user_data_get()->motion.select_camera;
	lv_btnmatrix_set_btn_ctrl(btnmatrix, select_camera, LV_BTNMATRIX_CTRL_CHECK_STATE);
}
/***
** 日期: 2022-05-05 16:29
** 作者: leo.liu
** 函数作用：移动侦测选中camera
** 返回参数说明：
***/
static bool motion_select_of_camera_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(motion_select_of_camera_btn_up);
	setting_right_btn_base_create(parent, 0, 0, 964, 88,
				      layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_SELECT_CAMERA),
				      motion_select_of_camera_string_get_and_save(false, 0),
				      &click_data,
				      LAYOUT_SETTING_MOTION_OBJ_SELECT_CAMERA);
	return true;
}

/***
** 日期: 2022-05-06 08:23
** 作者: leo.liu
** 函数作用：获取移动侦测记录格式字符串获取
** 返回参数说明：
***/
static const char *motion_select_saving_format_string_get_and_save(bool save, int value)
{

	if (save == true)
	{
		user_data_get()->motion.saving_fmt = value;
		user_data_save();
	}
	return language_common_string_get(user_data_get()->motion.saving_fmt == 0 ? LANG_COMMON_ID_PHOTO : LANG_COMMON_ID_VIDEO);
}

/***
** 日期: 2022-05-06 08:23
** 作者: leo.liu
** 函数作用：移动侦测格式消息框的确认按钮按下
** 返回参数说明：
***/

static void setting_motion_saving_fmt_dialog_confirm_up(lv_obj_t *obj)
{
	setting_motion_format_dialog_confirm_sub_up(2, LAYOUT_SETTING_MOTION_OBJ_SAVE_FMT, motion_select_saving_format_string_get_and_save);
}

/***
** 日期: 2022-05-06 08:23
** 作者: leo.liu
** 函数作用：移动侦测记录的事件
** 返回参数说明：
***/
static void motion_select_saving_format_btn_up(lv_obj_t *obj)
{
	// if((media_sdcard_insert_check() == false) && (user_data_get()->motion.saving_fmt == 1))
	// {
	// 	user_data_get()->motion.saving_fmt = 0;
	// }
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);
	if (dialog != NULL)
	{
		return;
	}
	dialog = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(dialog, setting_motion_dialog_cancel_up, setting_motion_saving_fmt_dialog_confirm_up);

	static const char *string_map[] = {"1", "\n", "2", ""};
	string_map[0] = language_common_string_get(LANG_COMMON_ID_PHOTO);
	string_map[2] = language_common_string_get(LANG_COMMON_ID_VIDEO);

	lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(dialog, LAYOUT_SETTING_MOTION_OBJ_BTNMATRIX, string_map);
	lv_obj_set_y(btnmatrix, 50);
	lv_obj_set_height(btnmatrix, 180);

	char saving_fmt = user_data_get()->motion.saving_fmt;
	// if((saving_fmt == 1) && (media_sdcard_insert_check() == false))
	// {
	// 	saving_fmt = 0;
	// }
	lv_btnmatrix_set_btn_ctrl(btnmatrix, saving_fmt, LV_BTNMATRIX_CTRL_CHECK_STATE);
}
/***
** 日期: 2022-05-05 17:08
** 作者: leo.liu
** 函数作用：移动侦测选择通道设置按钮创建
** 返回参数说明：
***/
static bool motion_select_saving_format_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(motion_select_saving_format_btn_up);
	setting_right_btn_base_create(parent, 0, 88, 964, 88,
				      layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_SAVING_FORMAT),
				      motion_select_saving_format_string_get_and_save(false, 0),
				      &click_data,
				      LAYOUT_SETTING_MOTION_OBJ_SAVE_FMT);
	return true;
}

static const char *motion_setting_sensitivity_string_get_and_save(bool save, int value)
{
	if (save == true)
	{
		user_data_get()->motion.sensivity = value;
		user_data_save();
	}
	char sensivity = user_data_get()->motion.sensivity;
	return layout_setting_motion_string_get(sensivity == 0 ? SETTING_MOTION_LANG_ID_MOTION_LOW : sensivity == 1 ? SETTING_MOTION_LANG_ID_MOTION_MIDDLE
														    : SETTING_MOTION_LANG_ID_MOTION_HIGHT);
}

/***
** 日期: 2022-05-06 08:32
** 作者: leo.liu
** 函数作用：灵敏度消息框的确认按钮事件
** 返回参数说明：
***/
static void setting_motion_sensitivity_dialog_confirm_up(lv_obj_t *obj)
{
	int value = setting_motion_dialog_confirm_sub_up(3, LAYOUT_SETTING_MOTION_OBJ_SENSIVITY, motion_setting_sensitivity_string_get_and_save);
	user_data_get()->motion.sensivity = value;
	user_data_save();
}

static void motion_setting_sensitivity_btn_up(lv_obj_t *obj)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);
	if (dialog != NULL)
	{
		return;
	}
	dialog = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(dialog, setting_motion_dialog_cancel_up, setting_motion_sensitivity_dialog_confirm_up);

	static const char *string_map[] = {"1", "\n", "2", "\n", "", ""};
	string_map[0] = layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_MOTION_LOW);
	string_map[2] = layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_MOTION_MIDDLE);
	string_map[4] = layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_MOTION_HIGHT);

	lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(dialog, LAYOUT_SETTING_MOTION_OBJ_BTNMATRIX, string_map);

	char sensivity = user_data_get()->motion.sensivity;
	lv_btnmatrix_set_btn_ctrl(btnmatrix, sensivity, LV_BTNMATRIX_CTRL_CHECK_STATE);
}

/***
** 日期: 2022-05-05 17:13
** 作者: leo.liu
** 函数作用：设置移动侦测灵敏度
** 返回参数说明：
***/
static bool motion_setting_sensitivity_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(motion_setting_sensitivity_btn_up);
	setting_right_btn_base_create(parent, 0, 88 + 88, 964, 88,
				      layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_SENSITIVITY),
				      motion_setting_sensitivity_string_get_and_save(false, 0),
				      &click_data,
				      LAYOUT_SETTING_MOTION_OBJ_SENSIVITY);
	return true;
}

static const char *motion_setting_motion_timer_string_get(void)
{
	return language_common_string_get(user_data_get()->motion.timer_en ? LANG_COMMON_ID_ON : LANG_COMMON_ID_OFF);
}

static void motion_setting_motion_timer_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_motion_timer));
}

/***
** 日期: 2022-05-05 17:13
** 作者: leo.liu
** 函数作用：设置移动侦测定时
** 返回参数说明：
***/
static bool motion_setting_motion_timer_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(motion_setting_motion_timer_btn_up);
	setting_right_btn_base_create(parent, 0, 88 + 88 * 2, 964, 88,
				      layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_MOTION_TIMER),
				      motion_setting_motion_timer_string_get(),
				      &click_data,
				      0);
	return true;
}

static const char *motion_setting_motion_lcd_string_get_and_save(bool save, int en)
{
	if (save == true)
	{
		user_data_get()->motion.lcd_en = (bool)en;
		user_data_save();
	}
	return language_common_string_get(user_data_get()->motion.lcd_en ? LANG_COMMON_ID_ON : LANG_COMMON_ID_OFF);
}

static void setting_motion_lcd_dialog_confirm_up(lv_obj_t *obj)
{
	setting_motion_dialog_confirm_sub_up(2, LAYOUT_SETTING_MOTION_OBJ_LCD, motion_setting_motion_lcd_string_get_and_save);
}
static void motion_setting_motion_lcd_btn_up(lv_obj_t *obj)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);
	if (dialog != NULL)
	{
		return;
	}
	dialog = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_MOTION_OBJ_DIALOGMSG);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(dialog, setting_motion_dialog_cancel_up, setting_motion_lcd_dialog_confirm_up);

	static const char *string_map[] = {"1", "\n", "2", ""};
	string_map[0] = language_common_string_get(LANG_COMMON_ID_OFF);
	string_map[2] = language_common_string_get(LANG_COMMON_ID_ON);

	lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(dialog, LAYOUT_SETTING_MOTION_OBJ_BTNMATRIX, string_map);
	lv_obj_set_y(btnmatrix, 50);
	lv_obj_set_height(btnmatrix, 180);

	char lcd_en = user_data_get()->motion.lcd_en;
	lv_btnmatrix_set_btn_ctrl(btnmatrix, lcd_en, LV_BTNMATRIX_CTRL_CHECK_STATE);
}

/***
** 日期: 2022-05-05 17:13
** 作者: leo.liu
** 函数作用：设置移动侦测开启背光
** 返回参数说明：
***/
static bool motion_setting_motion_lcd_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(motion_setting_motion_lcd_btn_up);
	setting_right_btn_base_create(parent, 0, 88 + 88 * 3, 964, 88,
				      layout_setting_motion_string_get(SETTING_MOTION_LANG_ID_LCD_IS_ON),
				      motion_setting_motion_lcd_string_get_and_save(false, false),
				      &click_data,
				      LAYOUT_SETTING_MOTION_OBJ_LCD);
	return true;
}
/***
** 日期: 2022-05-05 16:23
** 作者: leo.liu
** 函数作用：移动侦测参数按钮设置的容器
** 返回参数说明：
***/
static lv_obj_t *setting_motion_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	if (cont == NULL)
	{
		printf("motion detect setting cont create failed \n");
		return NULL;
	}
	lv_obj_set_id(cont, LAYOUT_SETTING_MOTION_OBJ_CONT);
	lv_obj_set_pos(cont, 30, 158);
	lv_obj_set_size(cont, 964, 442);

	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	lv_obj_set_hidden(cont, user_data_get()->motion.enable == true ? false : true);
	return cont;
}

static void LAYOUT_ENTER_FUNC(setting_motion)
{
	setting_cancel_btn_create(motion_cancel_btn_up);
	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));
	motion_use_motion_detect_btn_create();

	lv_obj_t *cont = setting_motion_cont_create();
	motion_select_of_camera_btn_create(cont);
	motion_select_saving_format_btn_create(cont);
	motion_setting_sensitivity_btn_create(cont); 
	motion_setting_motion_timer_btn_create(cont);
	motion_setting_motion_lcd_btn_create(cont);
}

static void LAYOUT_QUIT_FUNC(setting_motion)
{
}

CREATE_LAYOUT(setting_motion);