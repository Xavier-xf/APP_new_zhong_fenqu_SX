#include "layout_define.h"
typedef enum
{
	alarm_scr_act_obj_id_alarm_cont,
	alarm_scr_act_obj_id_alarm_password,
	alarm_scr_act_obj_id_msgdialog,
} alarm_scr_act_obj_id;
typedef enum
{
	alarm_alarm_cont_obj_id_alarm_id_label,
	alarm_alarm_cont_obj_id_time_label,
	alarm_alarm_cont_obj_id_apple_btn,
} alarm_alarm_cont_obj_id;
typedef enum
{
	alarm_alarm_password_obj_id_enter_pwd_cont,
	alarm_alarm_password_obj_id_enter_pwd_cont_to_label,
} alarm_alarm_password_obj_id;
/***
**   日期:2022-05-28 14:56:15
**   作者: leo.liu
**   函数作用：标记进入触发是否时call机后进入，客户需求触发过的不需要再次记录
**   参数说明:
***/
static bool is_alarm_trigger_call = false;
void alarm_trigger_call_set(bool en)
{
	is_alarm_trigger_call = en;
}
/***
**   日期:2022-07-18 09:05:06
**   作者: leo.liu
**   函数作用：获取触发call机的警报状态
**   参数说明:
***/
bool alarm_trigger_call_get(void)
{
	return is_alarm_trigger_call;
}
/***
**   日期:2022-05-28 11:54:57
**   作者: leo.liu
**   函数作用：交替变换显示
**   参数说明:
***/
static void alarm_alarm_cont_display(lv_task_t *task_t)
{
	lv_obj_t *cont = (lv_obj_t *)task_t->user_data;
	lv_color_t color = lv_obj_get_style_bg_color(cont, LV_CONT_PART_MAIN);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (color.full == lv_color_hex(0xdb3535).full) ? lv_color_hex(0xff4040) : lv_color_hex(0xdb3535));
}
/***
**   日期:2022-05-28 10:13:24
**   作者: leo.liu
**   函数作用：创建图标显示容器
**   参数说明:
***/
static lv_obj_t *alarm_alarm_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, alarm_scr_act_obj_id_alarm_cont);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 600);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	//	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
	static int trigger_sensor = 0;
	cont->user_data = &trigger_sensor;

	if ((user_data_get()->alarm.alarm_1_enable == false) && (user_data_get()->alarm.alarm_1_trigger == true))
	{
		trigger_sensor = 1;
		user_data_get()->alarm.alarm_1_trigger = false;
	}
	if ((user_data_get()->alarm.alarm_2_enable == false) && (user_data_get()->alarm.alarm_2_trigger == true))
	{
		trigger_sensor = 2;
		user_data_get()->alarm.alarm_2_trigger = false;
	}



	if ((user_data_get()->alarm.alarm_1_enable == true) && (user_data_get()->alarm.alarm_1_trigger == true))
	{
		trigger_sensor = 1;
	}
	else if ((user_data_get()->alarm.alarm_2_enable == true) && (user_data_get()->alarm.alarm_2_trigger == true))
	{
		trigger_sensor = 2;
	}


	lv_task_t *task_t = lv_layout_task_create(alarm_alarm_cont_display, 1000, LV_TASK_PRIO_MID, cont);
	lv_task_ready(task_t);
	return cont;
}
/***
**   日期:2022-05-28 10:13:44
**   作者: leo.liu
**   函数作用：创建警报图标显示
**   参数说明:
***/
static void alarm_security_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 392, 75);
	lv_obj_set_size(obj, 240, 150);

	static rom_bin_info img = rom_bin_info_get(ROM_UI_SECURITY_ALARM_ION_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
}
/***
**   日期:2022-05-28 10:14:06
**   作者: leo.liu
**   函数作用：创建警报文本显示
**   参数说明:
***/
static void alarm_security_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 344, 243);
	lv_obj_set_size(obj, 336, 84);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY));
}

/***
**   日期:2022-05-28 11:35:43
**   作者: leo.liu
**   函数作用：警报id显示
**   参数说明:
***/
static void alarm_alarm_id_label_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, alarm_alarm_cont_obj_id_alarm_id_label);
	int sensor_id = *((int *)parent->user_data);
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(sensor_id == 1 ? LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY_1 : LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY_2));
}
/***
**   日期:2022-05-28 10:17:39
**   作者: leo.liu
**   函数作用：创建警报id
**   参数说明:
***/
static void alarm_alarm_id_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, alarm_alarm_cont_obj_id_alarm_id_label);
	lv_obj_set_pos(obj, 416, 342);
	lv_obj_set_size(obj, 192, 42);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	alarm_alarm_id_label_display();
}

/***
**   日期:2022-05-28 11:42:05
**   作者: leo.liu
**   函数作用：警报时间
**   参数说明:
***/
static void alarm_alarm_time_label_display(void)
{
	struct tm tm;
	user_time_read(&tm);
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, alarm_alarm_cont_obj_id_time_label);
	lv_label_set_text_fmt(obj, "%04d.%02d.%02d  %02d:%02d:%02d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
/***
**   日期:2022-05-28 10:18:06
**   作者: leo.liu
**   函数作用：创建警报时间
**   参数说明:
***/
static void alarm_alarm_time_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_label_create(parent, NULL);
	lv_obj_set_id(obj, alarm_alarm_cont_obj_id_time_label);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 347, 391);
	lv_obj_set_size(obj, 331, 40);
	lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_style_local_text_color(obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff6b64));
	alarm_alarm_time_label_display();
}

static void alarm_apple_btn_up(lv_obj_t *obj)
{
	#ifdef MAOTOUYING_DAN_IC
	ringplay_play_stop();
	#else
	// ringplay_play_stop();
	// intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_RING_STOP);
	#endif
	
	if (OwnID != 1)
	{
		return;
	}
	// lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
	// int sensor_id = *((int *)cont->user_data);
	// if ((sensor_id == 1) && (sercurity_sensor1_normal_check()) == false)
	// {
	// 	return;
	// }
	// if ((sensor_id == 2) && (sercurity_sensor2_normal_check() == false))
	// {
	// 	return;
	// }

	// lv_obj_set_hidden(cont, true);
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_password);
	lv_obj_set_hidden(cont, false);
}
/***
**   日期:2022-05-28 10:18:28
**   作者: leo.liu
**   函数作用：创建应用按钮
**   参数说明:
***/
static void alarm_apple_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, alarm_alarm_cont_obj_id_apple_btn);
	lv_obj_set_pos(obj, 418, 462);
	lv_obj_set_size(obj, 187, 64);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,61);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_SECURITY_STOP_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	// lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	// lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_END));
	// lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff6b64));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	static obj_click_data click_data = obj_click_data_up_create(alarm_apple_btn_up);
	obj_click_event_listen(obj, &click_data);
}

/***
**   日期:2022-05-28 10:18:52
**   作者: leo.liu
**   函数作用：创建键盘容器
**   参数说明:
***/
static lv_obj_t *alarm_password_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, alarm_scr_act_obj_id_alarm_password);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 600);
	lv_obj_set_hidden(cont, true);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xd62e2e));
	return cont;
}

static void layout_alarm_msg_confirm_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(alarm_scr_act_obj_id_msgdialog);
}

/***
**   日期:2022-05-26 16:57:48
**   作者: leo.liu
**   函数作用：创建消息框
**   参数说明:
***/
static bool alarm_msg_dialog_create(const char *string)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_msgdialog);
	if (dialog != NULL)
	{
		return false;
	}
	dialog = setting_msgdialog_msg_bg_create(alarm_scr_act_obj_id_msgdialog);
	setting_msgdialog_msg_confirm_btn_create(dialog, layout_alarm_msg_confirm_btn_up);
	setting_msgdialog_msg_create(dialog, string);
	lv_obj_set_style_local_value_ofs_y(dialog, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -30);
	lv_obj_set_style_local_value_font(dialog, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	return true;
}

static void alarm_password_keyboard_up(lv_obj_t *obj, lv_state_t ev)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_msgdialog);
	if (dialog != NULL)
	{
		return;
	}

	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_password);
	parent = lv_obj_get_child_form_id(parent, alarm_alarm_password_obj_id_enter_pwd_cont);
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
				lv_obj_t *alarm_cont = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
				int sensor_id = *((int *)alarm_cont->user_data);
				if (sensor_id == 1)
				{
					user_data_get()->alarm.alarm_1_trigger = false;
				}
				else
				{
					user_data_get()->alarm.alarm_2_trigger = false;
				}
				user_data_save();
				if (((user_data_get()->alarm.alarm_1_enable == true) && (user_data_get()->alarm.alarm_1_trigger == true)) || ((user_data_get()->alarm.alarm_2_enable == true) && (user_data_get()->alarm.alarm_2_trigger == true)))
				{
					goto_layout(pLAYOUT(alarm));
					if (user_data_get()->alarm.alarm_1_trigger == true)
					{
						intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_SENSOR1_AGAIN);
					}
					else if (user_data_get()->alarm.alarm_2_trigger == true)
					{
						intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_SENSOR2_AGAIN);
					}
				}
				else
				{
					goto_layout(pLAYOUT(home));
					intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_QUIT);
				}
			}
			else
			{
				printf("input password failed ! \n");
				alarm_msg_dialog_create(layout_setting_password_string_get(SETTING_PASSWORD_LANG_ID_PWD_NOT_MATCH));
			}
		}
	}
}
/***
**   日期:2022-05-28 10:19:18
**   作者: leo.liu
**   函数作用：创建键盘
**   参数说明:
***/
static void alarm_password_keyboard_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_anything_create(alarm_password_keyboard_up);
	setting_passowrd_num_keyboard_create(parent, 152, 100, 302, 420, &click_data);
}

static void alarm_password_cancel_btn_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
	lv_obj_set_hidden(parent, false);
	parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_password);
	lv_obj_set_hidden(parent, true);

	setting_msgdialog_msg_bg_delete(alarm_scr_act_obj_id_msgdialog);
}
/***
**   日期:2022-05-28 10:19:55
**   作者: leo.liu
**   函数作用：创建键盘返回按钮
**   参数说明:
***/
static void alarm_password_cancel_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 20, 20);
	lv_obj_set_size(obj, 78, 78);

	static rom_bin_info img = rom_bin_info_get(ROM_UI_02_BTN_BTN_BOTTOM_CANCEL_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(alarm_password_cancel_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
**   日期:2022-05-28 10:20:51
**   作者: leo.liu
**   函数作用：创建密码容器
**   参数说明:
***/
static void alarm_password_enter_pwd_cont_create(lv_obj_t *parent)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_id(cont, alarm_alarm_password_obj_id_enter_pwd_cont);

	lv_obj_set_pos(cont, 570, 195);
	lv_obj_set_size(cont, 249, 133);

	lv_obj_t *obj = lv_label_create(cont, NULL);
	lv_obj_set_id(obj, alarm_alarm_password_obj_id_enter_pwd_cont_to_label);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 249, 42);
	lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_label_set_text(obj, layout_setting_password_string_get(SETTING_PASSWORD_LANG_ID_ENTER_PWD));
	setting_password_input_label_create(cont, 0, 72, 1);
}

/***
**   日期:2022-05-28 14:21:18
**   作者: leo.liu
**   函数作用：警报1在alarm页面触发
**   参数说明:
***/
static void layout_alarm_trigger_1_func(void)
{
	if (user_data_get()->alarm.alarm_1_trigger == true)
	{
		return;
	}
	alarm_trigger_call_set(false);
	user_data_get()->alarm.alarm_1_trigger = true;
	user_data_save();
	if(OwnID != 1)
	{
		lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
		*((int *)parent->user_data) = 1;
		alarm_alarm_id_label_display();
		alarm_alarm_time_label_display();
	}
}
static void layout_alarm_trigger_2_func(void)
{
	if (user_data_get()->alarm.alarm_2_trigger == true)
	{
		return;
	}
	alarm_trigger_call_set(false);
	user_data_get()->alarm.alarm_2_trigger = true;
	user_data_save();
	if(OwnID != 1)
	{
		lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
		*((int *)parent->user_data) = 2;
		alarm_alarm_id_label_display();
		alarm_alarm_time_label_display();
	}
}

static void layout_alarm_alarm_ring_start(int index)
{
	/***** 开启功放 *****/
	tuya_spk_sw_pin_enable(false);
}
static void layout_alarm_alarm_ring_end(int index)
{
	/***** 关闭功放 *****/
	power_amplifier_enable(false);
}

static void alram_ring_timeout_task(lv_task_t *task_t)
{
	if (ringplay_ing_check() == true)
	{
		ringplay_play_stop();
	}
	lv_task_del(task_t);
	printf("ring play finish \n");
}

/***
**   日期:2022-05-28 16:05:17
**   作者: leo.liu
**   函数作用：开启警报监控
**   参数说明:
***/
static void layout_alarm_monitor_open(void)
{
	lv_obj_t *alarm_cont = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
	int sensor_id = *((int *)alarm_cont->user_data);
	if (sensor_id == 1)
	{
		monitor_channel_set(MON_CH_CCTV1);
	}
	else
	{
		monitor_channel_set(MON_CH_CCTV2);
	}
	monitor_open(false, 0x03);
}

/***
**   日期:2022-05-28 16:26:11
**   作者: leo.liu
**   函数作用：创建后台录制任务
**   参数说明:
***/
static void alarm_auto_record_task(lv_task_t *task_t)
{
	if (video_input_state_get() == false)
	{
		if (video_record_status_get() == true)
		{
			video_record_stop();
		}
		return;
	}

	int *prec_mode = (int *)task_t->user_data;
	int rec_mode = (*prec_mode);
	if (rec_mode == 0)
	{
		if (media_sdcard_insert_check() == false)
		{
			(*prec_mode) = REC_MODE_ALARM | REC_MODE_TUYA_ALARM;
		}
		else
		{
			(*prec_mode) = REC_MODE_TUYA_ALARM;
			record_video_start(REC_MODE_ALARM);
			printf("##### alarm start record ##### \n");
		}
		record_jpeg_start((*prec_mode));
	}
	else if (rec_mode == (REC_MODE_ALARM | REC_MODE_TUYA_ALARM))
	{
		if (mjpeg_encode_status_get() == false)
		{
			monitor_close();
			lv_task_del(task_t);
		}
	}
	else if (rec_mode == REC_MODE_TUYA_ALARM)
	{
		int duration = 0;
		if (((video_record_duration_get(&duration) == true) && (duration >= 10000)) || (media_sdcard_insert_check() == false))
		{
			video_record_stop();
			monitor_close();
			lv_task_del(task_t);
			printf("##### alarm finish record ##### \n");
		}
	}
}

/***
**   日期:2022-06-20 10:24:14
**   作者: leo.liu
**   函数作用：增加到列表
**   参数说明:
***/
static void layout_alarm_list_add(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
	int sensor_id = *((int *)parent->user_data);

	struct tm tm;
	user_time_read(&tm);
	alarm_list_add(sensor_id, &tm);
}

/***
** 日期: 2022-04-28 09:53
** 作者: leo.liu
** 函数作用：监控被按下执行的回调函数
** 返回参数说明：
***/
void layout_alarm_click_down_func(lv_obj_t *obj)
{
	if ((user_data_get()->home_mode == 1) || (layout_monitor_talk_state() == true))
	{
		return;
	}
	printf("#############monitor_layout_keysound\n");
	//***** 控制声音流向 *****/
	touch_sound_play(ringplay_keysound_start_default_func, NULL);
}

static void LAYOUT_ENTER_FUNC(alarm)
{
	lv_obj_t *cont = alarm_alarm_cont_create();
	alarm_security_icon_create(cont);
	alarm_security_label_create(cont);
	alarm_alarm_id_label_create(cont);
	alarm_alarm_time_label_create(cont);
	alarm_apple_btn_create(cont);

	cont = alarm_password_cont_create();
	alarm_password_keyboard_create(cont);
	alarm_password_cancel_btn_create(cont);
	alarm_password_enter_pwd_cont_create(cont);

	layout_alarm1_trigger_callback_register(layout_alarm_trigger_1_func);
	layout_alarm2_trigger_callback_register(layout_alarm_trigger_2_func);
	lv_obj_click_down_callback_register(NULL);

	standby_timer_close();

	ringplay_play_form_index(8, 100, layout_alarm_alarm_ring_start, layout_alarm_alarm_ring_end, true);
	/*****  创建一个1分钟的定时器 *****/
	lv_layout_task_create(alram_ring_timeout_task, 1 * 60 * 1000, LV_TASK_PRIO_MID, NULL);

	if (is_alarm_trigger_call == false)
	{
		layout_alarm_list_add();
		if (user_data_get()->alarm.auto_record == true)
		{
			h264_encode_capture_enable(true);
			layout_alarm_monitor_open();
			static int rec_mode = 0;
			rec_mode = 0;
			lv_layout_task_create(alarm_auto_record_task, 500, LV_TASK_PRIO_MID, &rec_mode);
		}
		else
		{
			lv_obj_t *alarm_cont = lv_obj_get_child_form_id(lv_scr_act(), alarm_scr_act_obj_id_alarm_cont);
			int sensor_id = *((int *)alarm_cont->user_data);
			tuya_api_alarm_event(sensor_id + 2, (char *)&sensor_id, 1);
		}
	}

	is_alarm_trigger_call = false;
}
static void LAYOUT_QUIT_FUNC(alarm)
{
	#ifdef MAOTOUYING_DAN_IC
	// ringplay_play_stop();
	#else
	ringplay_play_stop();
	if (((user_data_get()->alarm.alarm_1_enable == false) && (user_data_get()->alarm.alarm_1_trigger == false)) || ((user_data_get()->alarm.alarm_2_enable == false) && (user_data_get()->alarm.alarm_2_trigger == false)))
	{
		intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_RING_STOP);
	}
	#endif
	record_video_close();
	h264_encode_capture_enable(false);
	if((tuya_app_preview_check() == false) && (tuya_online_num_get_customize() <= 0)) 
	{
		monitor_close();
	}
	layout_alarm1_trigger_callback_register(layout_alarm1_trigger_default);
	layout_alarm2_trigger_callback_register(layout_alarm2_trigger_default);
	lv_obj_click_down_callback_register(layout_alarm_click_down_func);
	standby_timer_restart(true);
}

CREATE_LAYOUT(alarm);

/***
**   日期:2022-05-31 15:24:51
**   作者: leo.liu
**   函数作用：检测是否需要警报触发
**   参数说明:
***/
bool alarm_trigger_check(void)
{
	bool reslut = false;

	if (user_data_get()->alarm.alarm_1_trigger == true)
	{
		if (user_data_get()->alarm.alarm_1_enable == false)
		{
			user_data_get()->alarm.alarm_1_trigger = false;
			user_data_save();
		}
		else
		{
			lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR1_TRIGGER, 0, 0);
			reslut = true;
		}
	}
	if (user_data_get()->alarm.alarm_2_trigger == true)
	{
		if (user_data_get()->alarm.alarm_2_enable == false)
		{
			user_data_get()->alarm.alarm_2_trigger = false;
			user_data_save();
		}
		else
		{
			lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR2_TRIGGER, 0, 0);
			reslut = true;
		}
	}
	return reslut;
}