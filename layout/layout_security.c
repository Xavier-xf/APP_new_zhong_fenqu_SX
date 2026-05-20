#include "layout_define.h"
typedef enum
{
	
	security_scr_act_obj_id_cont_security,
	security_scr_act_obj_id_head_cont_security,
	security_scr_act_obj_id_cont_password,
	security_scr_act_obj_id_msgdialog,
} security_scr_act_obj_id;
typedef enum
{
	security_cont_security_obj_id_auto_btn,
	security_cont_security_obj_id_sensor_1_cont,
	security_cont_security_obj_id_sensor_2_cont,
	security_cont_security_obj_id_work_txt,
	security_cont_security_obj_id_apple_btn,
	security_cont_security_obj_id_cancel_btn,
	security_cont_security_obj_id_head_txt_btn,
	security_cont_security_obj_id_alarm_list_btn
} security_cont_security_obj_id;
typedef enum
{
	security_sensor_cont_obj_id_checkbox,
} security_sensor_cont_obj_id;
typedef enum
{
	security_cont_password_cont_obj_id_enter_pwd_cont,
	security_cont_password_cont_obj_id_enter_pwd_cont_to_label,
} security_cont_password_cont_obj_id;

/***
**   日期:2022-05-26 14:02:27
**   作者: leo.liu
**   函数作用：创建设置报警的容器
**   参数说明:
***/
static lv_obj_t *security_security_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, security_scr_act_obj_id_cont_security);
	lv_obj_set_pos(cont, 70, 89);
	lv_obj_set_size(cont, 880, 469);
	lv_obj_set_style_local_bg_opa(cont,LV_CONT_PART_MAIN,LV_STATE_DEFAULT,LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont,LV_CONT_PART_MAIN,LV_STATE_DEFAULT,lv_color_hex(0x090909));
	lv_obj_set_style_local_radius(cont,LV_CONT_PART_MAIN,LV_STATE_DEFAULT,16);
	return cont;
}

static lv_obj_t *security_security_head_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, security_scr_act_obj_id_head_cont_security);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 82);
	lv_obj_set_style_local_bg_opa(cont,LV_CONT_PART_MAIN,LV_STATE_DEFAULT,LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont,LV_CONT_PART_MAIN,LV_STATE_DEFAULT,lv_color_hex(0x000000));
	lv_obj_set_style_local_radius(cont,LV_CONT_PART_MAIN,LV_STATE_DEFAULT,16);
	return cont;
}

static void security_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_etc));
}
/***
**   日期:2022-05-26 14:03:23
**   作者: leo.liu
**   函数作用：创建返回按钮
**   参数说明:
***/
static void security_cancel_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj,security_cont_security_obj_id_cancel_btn);
	lv_obj_set_pos(obj, 10, 10);
	lv_obj_set_size(obj, 42, 43);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(security_cancel_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
**   日期:2022-05-26 14:03:48
**   作者: leo.liu
**   函数作用：创建头部文本显示
**   参数说明:
***/
static void security_head_txt_obj_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj,security_cont_security_obj_id_head_txt_btn);
	if (obj == NULL)
	{
		printf("create security failed \n");
		return;
	}

	lv_obj_set_pos(obj, 456, 26);
	lv_obj_set_size(obj, 111, 35);

	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_home_string_get(HOME_LANG_ID_SECURITY));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
}

static void security_alarm_list_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(alarm_list));
}
/***
**   日期:2022-05-26 14:04:12
**   作者: leo.liu
**   函数作用：创建头部alarmlist 按钮
**   参数说明:
***/
static void security_alarm_list_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj,security_cont_security_obj_id_alarm_list_btn);
	lv_obj_set_pos(obj, 962, 29);
	lv_obj_set_size(obj,40, 40);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_SECURITY_LIST_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(security_alarm_list_btn_up);
	obj_click_event_listen(obj, &click_data);
}

static void security_auto_record_btn_up(lv_obj_t *obj)
{
	bool at = user_data_get()->alarm.auto_record;
	at = at == false ? true : false;
	lv_state_t state = at == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT;
	lv_obj_set_state(obj, state);

	user_data_get()->alarm.auto_record = at;
	user_data_save();
}
/***
**   日期:2022-05-26 14:04:47
**   作者: leo.liu
**   函数作用：创建自动记录按钮
**   参数说明:
***/
static void security_auto_record_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, security_cont_security_obj_id_auto_btn);
	lv_obj_set_pos(obj, 741, 20);
	lv_obj_set_size(obj, 131, 47);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3a3f47));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, lv_color_hex(0xff6b64));
	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 54);
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_AUTO_REC));
	lv_state_t state = user_data_get()->alarm.auto_record == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT;
	lv_obj_set_state(obj, state);

	static obj_click_data click_data = obj_click_data_up_create(security_auto_record_btn_up);
	obj_click_event_listen(obj, &click_data);

	if (user_data_get()->alarm.alarm_enable == true)
	{
		lv_obj_set_click(obj, false);
	}
}

/***
**   日期:2022-05-26 15:57:46
**   作者: leo.liu
**   函数作用：sensor1容器显示
**   参数说明:
***/
static void security_sensor_1_cont_display(lv_obj_t *cont)
{
	bool en = user_data_get()->alarm.alarm_1_enable;
	bool em = user_data_get()->alarm.alarm_enable;
	if( (en == true) && (em == true))
	{
		lv_obj_set_state(cont, LV_STATE_DISABLED);
		lv_obj_set_click(cont, false);
	}
	if (en == true)
	{
		lv_obj_set_state(cont, LV_STATE_CHECKED);
		
	}
	
	else
	{
		en = user_data_get()->alarm.alarm_2_enable;
		if ((en == true)&&(user_data_get()->alarm.alarm_enable == true))
		{
			lv_obj_set_click(cont, false);
		}
		lv_obj_set_state(cont, LV_STATE_DEFAULT);
	}
}

static void layout_security_msg_confirm_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(security_scr_act_obj_id_msgdialog);
}

/***
**   日期:2022-05-26 16:57:48
**   作者: leo.liu
**   函数作用：创建消息框
**   参数说明:
***/

static bool security_msg_dialog_create(const char *string)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_msgdialog);
	if (dialog != NULL)
	{
		return false;
	}
	
	dialog = setting_mecurity_msgdialog_msg_bg_create(security_scr_act_obj_id_msgdialog);
	setting_security_msgdialog_msg_confirm_btn_create(dialog, layout_security_msg_confirm_btn_up);
	setting_msgdialog_msg_create(dialog, string);
	return true;
}



static void security_sensor_cont_up(lv_obj_t *obj)
{
	
	
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_cont_security);
	lv_obj_t *sensor1_cont = lv_obj_get_child_form_id(parent, security_cont_security_obj_id_sensor_1_cont);
	lv_obj_t *sensor2_cont = lv_obj_get_child_form_id(parent, security_cont_security_obj_id_sensor_2_cont);
	if (obj == sensor1_cont)
	{
		if(sercurity_sensor1_normal_check() == true) 
		{
		
			if((user_data_get()->alarm.alarm_1_enable = !user_data_get()->alarm.alarm_1_enable))
			{
				lv_obj_set_state(obj,LV_STATE_CHECKED);
			}
			else
			{
				lv_obj_set_state(obj,LV_STATE_DEFAULT);
			}
		}
		else
		{
			security_msg_dialog_create(layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1_ERROR));
		}
	}
	else if (obj == sensor2_cont)
	{
		if ((sercurity_sensor2_normal_check() == true))
		{
			
			if((user_data_get()->alarm.alarm_2_enable = !user_data_get()->alarm.alarm_2_enable))
			{
				lv_obj_set_state(obj,LV_STATE_CHECKED);
			}
			else
			{
				lv_obj_set_state(obj,LV_STATE_DEFAULT);
			}
		}
		else
		{
			security_msg_dialog_create(layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_2_ERROR));
		}
	}
}
/***
**   日期:2022-05-26 14:05:12
**   作者: leo.liu
**   函数作用：创建sensor 1容器
**   参数说明:
***/
static lv_obj_t *security_sensor_1_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, security_cont_security_obj_id_sensor_1_cont);
	lv_obj_set_pos(cont, 139, 91);
	lv_obj_set_size(cont, 281, 264);
	lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3a3f47));

	static rom_bin_info img_offline = rom_bin_info_get(ROM_UI_SECURITY_001_PNG);
	static rom_bin_info img_online = rom_bin_info_get(ROM_UI_SECURITY_002_PNG);
	static rom_bin_info img_select = rom_bin_info_get(ROM_UI_SECURITY_003_PNG);

	lv_obj_set_style_local_pattern_image(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, sercurity_sensor1_normal_check() ? &img_online : &img_offline);
	lv_obj_set_style_local_pattern_image(cont, LV_CONT_PART_MAIN, LV_STATE_CHECKED, &img_select);

	lv_obj_set_style_local_value_str(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1));
	lv_obj_set_style_local_value_align(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
	lv_obj_set_style_local_value_ofs_y(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -10);
	static obj_click_data click_data = obj_click_data_up_create(security_sensor_cont_up);
	obj_click_event_listen(cont, &click_data); 
	return cont;
}


/***
**   日期:2022-05-26 15:57:46
**   作者: leo.liu
**   函数作用：sensor2容器显示
**   参数说明:
***/
static void security_sensor_2_cont_display(lv_obj_t *cont)
{
	bool en = user_data_get()->alarm.alarm_2_enable;
	bool em = user_data_get()->alarm.alarm_enable;
	if ((en == true) && (em == true) )
	{
		lv_obj_set_state(cont, LV_STATE_CHECKED);
		lv_obj_set_click(cont, false);
	}
	if( (en == true))
	{
		lv_obj_set_state(cont, LV_STATE_CHECKED);
		
	}
	else
	{
		en = user_data_get()->alarm.alarm_1_enable;
		if ((en == true)&&(user_data_get()->alarm.alarm_enable == true))
		{
			lv_obj_set_click(cont, false);
		}
		lv_obj_set_state(cont, LV_STATE_DEFAULT);
	}
}
/***
**   日期:2022-05-26 14:06:27
**   作者: leo.liu
**   函数作用：创建sensor2 容器
**   参数说明:
***/
static lv_obj_t *security_sensor_2_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, security_cont_security_obj_id_sensor_2_cont);
	lv_obj_set_pos(cont, 465, 91);
	lv_obj_set_size(cont, 281, 264);
	lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3a3f47));

	static rom_bin_info img_offline = rom_bin_info_get(ROM_UI_SECURITY_001_PNG);
	static rom_bin_info img_online = rom_bin_info_get(ROM_UI_SECURITY_002_PNG);
	static rom_bin_info img_select = rom_bin_info_get(ROM_UI_SECURITY_003_PNG);

	lv_obj_set_style_local_pattern_image(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, sercurity_sensor2_normal_check() ? &img_online : &img_offline);
	lv_obj_set_style_local_pattern_image(cont, LV_CONT_PART_MAIN, LV_STATE_CHECKED, &img_select);

	lv_obj_set_style_local_value_str(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_2));
	lv_obj_set_style_local_value_align(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
	lv_obj_set_style_local_value_ofs_y(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -10);
	static obj_click_data click_data = obj_click_data_up_create(security_sensor_cont_up);
	obj_click_event_listen(cont, &click_data);
	return cont;
}

/***
**   日期:2022-05-26 14:07:11
**   作者: leo.liu
**   函数作用：创建提示工作中
**   参数说明:
***/
// static void security_work_txt_create(lv_obj_t *parent)
// {
// 	lv_obj_t *obj = lv_obj_create(parent, NULL);
// 	lv_obj_set_id(obj, security_cont_security_obj_id_work_txt);
// 	lv_obj_set_pos(obj, 370, 600);
// 	lv_obj_set_size(obj, 284, 35);
// 	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
// 	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_WORKING));
// 	lv_obj_set_style_local_value_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff6666));
// 	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
// 	if ((user_data_get()->alarm.alarm_enable == true))
// 	{
// 		lv_obj_set_hidden(obj, true);
// 	}
// 	else
// 	{
// 		lv_obj_set_hidden(obj, true);
// 	}
// }

static void security_apple_btn_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_cont_security);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, security_cont_security_obj_id_apple_btn);
	if ((user_data_get()->alarm.alarm_enable == true))
	{
		lv_obj_set_state(obj, LV_STATE_CHECKED);
	}
	else
	{
		lv_obj_set_state(obj, LV_STATE_DEFAULT);
	}
}
static void security_apple_btn_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_cont_security);
	lv_obj_t *cont_1 = lv_obj_get_child_form_id(parent, security_cont_security_obj_id_sensor_1_cont);
	lv_obj_t *cont_2 = lv_obj_get_child_form_id(parent, security_cont_security_obj_id_sensor_2_cont);
	
	if ((user_data_get()->alarm.alarm_enable))
	{
		/*****  进入输入密码界面 *****/
		lv_obj_set_hidden(parent, true);
		parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_cont_password);
		lv_obj_set_hidden(parent, false);
		parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_head_cont_security);
		lv_obj_set_hidden(parent, true);
		// user_data_get()->alarm.alarm_enable = false ;
		return;
	}

	// if(user_data_get()->alarm.alarm_enable ==  false)
	// {
	// 	user_data_get()->alarm.alarm_enable = true;
	// }

	int sensor_open_status = user_data_get()->alarm.alarm_1_enable | user_data_get()->alarm.alarm_2_enable << 1;
	if(sensor_open_status == 0x03 &&(sercurity_sensor1_normal_check() == false) && (sercurity_sensor2_normal_check() == false))
	{
		security_msg_dialog_create(layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1_AND_2_ERROR));
	}
	else if (sensor_open_status == 0x01 && (sercurity_sensor1_normal_check() == false))
	{
		security_msg_dialog_create(layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1_ERROR));
	}
	else if (sensor_open_status == 0x02 && (sercurity_sensor2_normal_check() == false))
	{
		security_msg_dialog_create(layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_2_ERROR));
	}
	else if (sensor_open_status == 0x00)
	{
		security_msg_dialog_create(layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SENSOR_1_AND_2_ERROR));//
	}
	else
	{
		if(user_data_get()->alarm.alarm_enable ==  false)
		{
			user_data_get()->alarm.alarm_enable = true;
		}
		// user_data_save();
		security_sensor_1_cont_display(cont_1);
		security_sensor_2_cont_display(cont_2);
		security_apple_btn_display();
		// lv_obj_t *obj = lv_obj_get_child_form_id(parent, security_cont_security_obj_id_work_txt);
		// lv_obj_set_hidden(obj, false);

		lv_obj_t *obj = lv_obj_get_child_form_id(parent, security_cont_security_obj_id_auto_btn);
		lv_obj_set_click(obj, false);
	}
	// lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_ALAM_UPDATE_MONITOR,  0);

}
/***
**   日期:2022-05-26 15:50:13
**   作者: leo.liu
**   函数作用：创建应用按钮
**   参数说明:
***/
static void security_apple_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, security_cont_security_obj_id_apple_btn);
	lv_obj_set_pos(obj, 137, 390);
	lv_obj_set_size(obj, 606, 54);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_EXE));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_END));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, lv_color_hex(0x47494a));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,10);

	static obj_click_data click_data = obj_click_data_up_create(security_apple_btn_up);
	obj_click_event_listen(obj, &click_data);
	security_apple_btn_display();
}
/***
**   日期:2022-05-26 14:07:45
**   作者: leo.liu
**   函数作用：创建密码输入容器
**   参数说明:
***/
static lv_obj_t *security_password_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, security_scr_act_obj_id_cont_password);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 600);
	lv_obj_set_hidden(cont, true);
	return cont;
}

static void security_password_cancel_btn_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_cont_security);
	lv_obj_set_hidden(parent, false);
	parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_cont_password);
	lv_obj_set_hidden(parent, true);
	parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_head_cont_security);
	lv_obj_set_hidden(parent, false);
	setting_msgdialog_msg_bg_delete(security_scr_act_obj_id_msgdialog);
}
/***
** 日期: 2022-05-07 13:43
** 作者: leo.liu
** 函数作用：创建返回按钮
** 返回参数说明：
***/
static void security_password_cancel_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 20, 20);
	lv_obj_set_size(obj, 78, 78);

	static rom_bin_info img = rom_bin_info_get(ROM_UI_02_BTN_BTN_BOTTOM_CANCEL_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(security_password_cancel_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
**   日期:2022-05-26 17:32:11
**   作者: leo.liu
**   函数作用：数字键盘
**   参数说明:
***/
static void security_password_keyboard_password_btn_up(lv_obj_t *obj, lv_state_t ev)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_msgdialog);
	if (dialog != NULL)
	{
		return;
	}

	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), security_scr_act_obj_id_cont_password);
	parent = lv_obj_get_child_form_id(parent, security_cont_password_cont_obj_id_enter_pwd_cont);
	if (parent == NULL)
	{
		return;
	}

	if (ev != LV_EVENT_VALUE_CHANGED)
	{
		return;
	}

	const char *txt = lv_btnmatrix_get_active_btn_text(obj);
	if (txt[0] == ' ')
	{
		setting_password_del_string(parent, 4);
	}
	else if (setting_password_input_string(parent, txt, 4) == true)
	{
		if (setting_password_edit_index_get(parent) == 4)
		{
			char buffer[16] = {0};
			setting_password_get_string(parent, buffer);
			printf("password:%c%c%c%c \n", user_data_get()->etc.password[0], user_data_get()->etc.password[1], user_data_get()->etc.password[2], user_data_get()->etc.password[3]);
			if (strncmp(buffer, user_data_get()->etc.password, 4) == 0)
			{
				printf("input password success ! \n");
				user_data_get()->alarm.alarm_1_enable = false;
				user_data_get()->alarm.alarm_2_enable = false;
				user_data_get()->alarm.alarm_enable = false;
				user_data_save();
				// lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_ALAM_UPDATE_MONITOR,  0);
				goto_layout(pLAYOUT(security));
			}
			else
			{
				printf("input password failed ! \n");
				security_msg_dialog_create(layout_setting_password_string_get(SETTING_PASSWORD_LANG_ID_PWD_NOT_MATCH));
			}
		}
	}
}

/***
**   日期:2022-05-26 14:08:07
**   作者: leo.liu
**   函数作用：创建数字键盘
**   参数说明:
***/
static void security_password_keyboard_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_anything_create(security_password_keyboard_password_btn_up);
	setting_passowrd_num_keyboard_create(parent, 152, 100, 302, 420, &click_data);
}
/***
** 日期: 2022-05-07 16:04
** 作者: leo.liu
** 函数作用：创建输入密码容器
** 返回参数说明：
***/
static void security_password_enter_pwd_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, security_cont_password_cont_obj_id_enter_pwd_cont);

	lv_obj_set_pos(cont, 570, 195);
	lv_obj_set_size(cont, 249, 125);

	lv_obj_t *obj = lv_label_create(cont, NULL);
	lv_obj_set_id(obj, security_cont_password_cont_obj_id_enter_pwd_cont_to_label);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 249, 42);
	lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_label_set_text(obj, layout_setting_password_string_get(SETTING_PASSWORD_LANG_ID_ENTER_PWD));
	setting_password_input_label_create(cont, 0, 68, 1);
}
static void LAYOUT_ENTER_FUNC(security)
{
	lv_obj_t *cont_head = security_security_head_cont_create();

	security_cancel_btn_create(cont_head);
	security_head_txt_obj_create(cont_head);
	security_alarm_list_btn_create(cont_head);

	lv_obj_t *cont = security_security_cont_create();
	security_auto_record_btn_create(cont);
	lv_obj_t *sensor_cont = security_sensor_1_cont_create(cont);
	security_sensor_1_cont_display(sensor_cont);
	sensor_cont = security_sensor_2_cont_create(cont);
	security_sensor_2_cont_display(sensor_cont);
	// security_work_txt_create(cont);
	security_apple_btn_create(cont);
	cont = security_password_cont_create();
	security_password_keyboard_create(cont);
	security_password_cancel_btn_create(cont);
	security_password_enter_pwd_cont_create(cont);
}
static void LAYOUT_QUIT_FUNC(security)
{

}

CREATE_LAYOUT(security);