#include "layout_define.h"
#include "tuya/tuya_uuid_and_key.h"
#include "wifi_api.h"

#define LAYOUT_SETTING_ETC_OBJ_OPEN_THE_DOOR_ID 0X01

#define LAYOUT_SETTING_ETC_OBJ_DEVICE_ID_ID 0X02

#define LAYOUT_SETTING_ETC_OBJ_OPEN_DOOR1_CLOCK_ID 0X03

#define LAYOUT_SETTING_ETC_OBJ_MSG_ID 0X04 //需要连续的两个ID

#define LAYOUT_SETTING_ETC_OBJ_MSG_BTNMATRIX_ID 0X06

#define LAYOUT_SETTING_ETC_OBJ_DOOR2_UNLOCK_ID 0X07

#define LAYOUT_SETTING_ETC_OBJ_PAGE_ID 0X08

#define DOOR_1_OPEN_TIME_TEXT_ID 0X09

#define DOOR_2_OPEN_TIME_TEXT_ID 0X10   

#define TUYA_SEND_TIME_TEXT_ID 0XA0



static void setting_etc_language_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_language));
}
/***
** 日期: 2022-05-04 14:14
** 作者: leo.liu
** 函数作用：语言按钮创建
** 返回参数说明：
***/
static void setting_etc_language_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_etc_language_btn_up);
	lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 0, 622, 90,
						      layout_setting_etc_string_get(SETTING_ETC_LANG_ID_LANGUAGE),
						      layout_setting_etc_string_get(SETTING_ETC_LANG_ID_LANGUAGE_SUB),
						      &click_data,
						      0);
	lv_page_glue_obj(btn, true);
}

/***
** 日期: 2022-05-04 17:30
** 作者: leo.liu
** 函数作用：消息框返回按钮点击事件
** 返回参数说明：
***/
static void setting_etc_msg_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_ETC_OBJ_MSG_ID);
}





static const char *setting_etc_device_id_string_get(void)
{
	char device_id = user_data_get()->etc.deive_id;
	return layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DEVICE_ID1 + device_id);
}

static void setting_etc_id_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_id));
}
/***
** 日期: 2022-05-04 15:21
** 作者: leo.liu
** 函数作用：设置设备ID按钮创建
** 返回参数说明：
***/
static void setting_etc_id_btn_create(lv_obj_t *parent)
{

	static obj_click_data click_data = obj_click_data_up_create(setting_etc_id_btn_up);
	lv_obj_t *btn = setting_right_btn_base_create(parent,0, 90*0 + 90 * 1, 622, 90,
						      layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SET_ID),
						      setting_etc_device_id_string_get(),
						      &click_data,
						      LAYOUT_SETTING_ETC_OBJ_DEVICE_ID_ID);

	lv_page_glue_obj(btn, true);
}



















lv_obj_t *sys_setting_etc_unclock_btn_create(lv_obj_t *parent,int id,int x, int y, int w, int h, const char *string_lable, char *string1,obj_click_data * click_data, obj_click_data * btn_pdata1, obj_click_data * btn_pdata2)
{
    lv_obj_t * btn = lv_btn_create(parent, NULL);
	
    lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);
	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	
    if (string1 != NULL)
    {
        lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,string_lable);
		lv_obj_set_style_local_value_color(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_hex(0xFFFFFF));
		lv_obj_set_style_local_value_align(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,LV_ALIGN_IN_LEFT_MID);
		// lv_obj_set_style_local_value_ofs_x(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,-210);
		lv_obj_set_style_local_value_font(btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
    }
	lv_obj_t *label = lv_label_create(parent, NULL);
   	lv_obj_set_pos(label, w-110, y+20);
	lv_obj_set_size(label, 250, 52);
	lv_obj_set_id(label,id);

	lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);
	lv_label_set_text(label, string1);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);

	obj_click_event_listen(btn, click_data);

	if(btn_pdata1 != NULL){
    	lv_obj_t * btn1 = lv_btn_create(parent, NULL);
		lv_obj_set_pos(btn1, w-200, y+20);
    	lv_obj_set_size(btn1, 48, 49);
		lv_obj_set_ext_click_area(btn1, 20, 20, 2, 2);
	    static rom_bin_info info = rom_bin_info_get(ROM_UI_VOLUME_LIGHT_PNG);
		lv_obj_set_style_local_pattern_image(btn1,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,&info);
		lv_obj_set_style_local_bg_opa(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);

		lv_obj_set_style_local_bg_color(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
		lv_obj_set_style_local_bg_color(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x3ac392));

		obj_click_event_listen(btn1, btn_pdata1);
	}
	if(btn_pdata2 != NULL){
		lv_obj_t * btn2 = lv_btn_create(parent, NULL);
		lv_obj_set_pos(btn2, w-50, y+20);
    	lv_obj_set_size(btn2, 48, 49);
		lv_obj_set_ext_click_area(btn2, 20, 20, 2, 2);
	    static rom_bin_info info1 = rom_bin_info_get(ROM_UI_VOLUME_RIGHT_PNG);
		lv_obj_set_style_local_pattern_image(btn2,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,&info1);
		lv_obj_set_style_local_bg_opa(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);

		lv_obj_set_style_local_bg_color(btn2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
		lv_obj_set_style_local_bg_color(btn2, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x3ac392));

		obj_click_event_listen(btn2, btn_pdata2);	
	}
	/***** 设置下底边框 *****/
	lv_obj_set_style_local_border_width(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
	lv_obj_set_style_local_border_color(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x32, 0x32, 0x37));
	lv_obj_set_style_local_border_color(btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x22, 0x22, 0x27));
	lv_obj_set_style_local_border_side(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
    return btn;
}
































void setting_etc_door_open_time_btn_label_create(lv_obj_t *btn, int obj_id)
{
	lv_obj_t *text = lv_label_create(btn->parent, NULL);
	lv_obj_set_id(text, obj_id);
	lv_obj_set_click(text, false);
	lv_label_set_long_mode(text, LV_LABEL_LONG_EXPAND);
	lv_label_set_align(text, LV_LABEL_ALIGN_RIGHT);

	lv_obj_align(text, btn, LV_ALIGN_IN_RIGHT_MID, -48, 0);
	lv_obj_set_auto_realign(text, true);

	lv_obj_set_style_local_text_font(text, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	lv_obj_set_style_local_text_color(text, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x97, 0x97, 0x97));
}

// static void setting_etc_door1_open_time_btn_up(lv_obj_t *obj)
// {
	
// 	lv_obj_t *parent = lv_obj_get_parent(obj);
// 	lv_obj_t *text = lv_obj_get_child_form_id(parent, DOOR_1_OPEN_TIME_TEXT_ID);

// 	if(user_data_get()->etc.door_1_open_time < 15)
// 	{
// 		user_data_get()->etc.door_1_open_time += 1;
// 	}
// 	else if(user_data_get()->etc.door_1_open_time < 60)
// 	{
// 		user_data_get()->etc.door_1_open_time += 5;
// 	}
// 	else
// 	{
// 		user_data_get()->etc.door_1_open_time = 1;
// 	}
// 	user_data_save();

// 	lv_label_set_text_fmt(text, "%ds", user_data_get()->etc.door_1_open_time);
// }



static void layout_etc_clock_set_left_btn_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_parent(obj);
	lv_obj_t *text = lv_obj_get_child_form_id(parent, DOOR_1_OPEN_TIME_TEXT_ID);
	if(--user_data_get()->etc.door_1_open_time <= 0){
		user_data_get()->etc.door_1_open_time = 60;
	}

	static char str1[32] = {0};
	sprintf(str1, "%ds",user_data_get()->etc.door_1_open_time);
	lv_label_set_text(text, str1);
	user_data_save();
}

static void layout_etc_clock_set_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_parent(obj);
	lv_obj_t *text = lv_obj_get_child_form_id(parent, DOOR_1_OPEN_TIME_TEXT_ID);
	if(++user_data_get()->etc.door_1_open_time > 60){
		user_data_get()->etc.door_1_open_time= 1;
	}

	static char str1[32] = {0};
	sprintf(str1, "%ds",user_data_get()->etc.door_1_open_time);
	lv_label_set_text(text, str1);
	user_data_save();
}

static void layout_etc_unclock_set_left_btn_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_parent(obj);
	lv_obj_t *text = lv_obj_get_child_form_id(parent, DOOR_2_OPEN_TIME_TEXT_ID);
	if(--user_data_get()->etc.door_2_open_time <= 0){
		user_data_get()->etc.door_2_open_time = 60;
	}

	static char str1[32] = {0};
	sprintf(str1, "%ds",user_data_get()->etc.door_2_open_time);
	lv_label_set_text(text, str1);
	user_data_save();
}

static void layout_etc_unclock_set_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_parent(obj);
	lv_obj_t *text = lv_obj_get_child_form_id(parent, DOOR_2_OPEN_TIME_TEXT_ID);
	if(++user_data_get()->etc.door_2_open_time > 60){
		user_data_get()->etc.door_2_open_time= 1;
	}

	static char str1[32] = {0};
	sprintf(str1, "%ds",user_data_get()->etc.door_2_open_time);
	lv_label_set_text(text, str1);
	user_data_save();
}



/***
** 日期: 2022-05-04 14:19
** 作者: leo.liu
** 函数作用：开锁1模式按钮创建
** 返回参数说明：
***/
static void setting_etc_door1_open_time_btn_create(lv_obj_t *parent)
{
	// static obj_click_data click_data = obj_click_data_up_create(setting_etc_door1_open_time_btn_up);
	static obj_click_data btn_data1 = obj_click_data_up_create(layout_etc_clock_set_left_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create( layout_etc_clock_set_right_btn_up);
	static char str[32] = {0};
	sprintf(str, "%ds",user_data_get()->etc.door_1_open_time);
	lv_obj_t *btn = sys_setting_etc_unclock_btn_create(parent,DOOR_1_OPEN_TIME_TEXT_ID,0, 90 + 90 * 1, 622, 90,
							layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DOOR_1_OPEN_TIME),
							str,
						    NULL,
							&btn_data1,
							&btn_data2);
	lv_page_glue_obj(btn, true);				

}


static void setting_etc_security_password_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_password));
}
/***
** 日期: 2022-05-04 14:21
** 作者: leo.liu
** 函数作用：修改密码按钮创建
** 返回参数说明：
***/
static void setting_etc_security_password_btn_create(lv_obj_t *parent)
{
	if(OwnID == 1)
	{
		static obj_click_data click_data = obj_click_data_up_create(setting_etc_security_password_btn_up);
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 3, 622, 90,
								layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SECURITY_PASSWORD),
								NULL,
								&click_data,
								0);

		lv_page_glue_obj(btn, true);
	}
	else
	{
		static obj_click_data click_data = obj_click_data_up_create(setting_etc_security_password_btn_up);
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 1, 622, 90,
								layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SECURITY_PASSWORD),
								NULL,
								&click_data,
								0);

		lv_page_glue_obj(btn, true);
	}

}
#ifdef MAOTOUYING_DAN_IC
// static void get_count_task(lv_task_t *task)
// {
// 	// printf("====video coun is %d====\n",tuya_online_num_get_customize());
// 	goto_layout(pLAYOUT(home));
// 	lv_task_del(task);
// }
#endif
#include "tuya_ipc_api.h"
static void setting_etc_msg_reset_system_confirm_btn_up(lv_obj_t *obj)
{
	user_data_reset();
	// tuay_api_data_reset();

	if(tuya_ipc_get_mqtt_status() == true)
	{
		http_gw_reset(2);
		tuay_api_data_reset();
	}

			/*恢复初始化设置之后需WiFi重连*/
	wifi_device_user_to_temp(NULL,  NULL );
	wifi_device_temp_conneting();
	wifi_device_tmp_sync();


	system("rm -rf /app/data/photo");

	// user_data_get()->door1_always_enable = true;
	// user_data_get()->door2_always_enable = true;
#ifdef MAOTOUYING_DAN_IC
	// backlight_enable(false);
	// /* 单IC的IPS屏设置：不重启，控制背光 */
	// user_data_init();
	// extern lv_task_t *lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void *user_data);
	// lv_task_create(get_count_task, 3*1000, LV_TASK_PRIO_MID, NULL);
	system("killall CDV1004QT.BIN");
	
#else
	usleep(1000 * 1000);
	system("reboot");
#endif
}

static void setting_etc_system_reset_btn_up(lv_obj_t *obj)
{
	/***** 消息框存在，则不再做任何处理 *****/
	lv_obj_t *msg_obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_ETC_OBJ_MSG_ID);
	if (msg_obj != NULL)
	{
		return;
	}

	msg_obj = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_ETC_OBJ_MSG_ID);
	setting_msgdialog_msg_confirm_and_cancel_btn_create(msg_obj, setting_etc_msg_cancel_btn_up, setting_etc_msg_reset_system_confirm_btn_up);

	obj = lv_obj_create(msg_obj, NULL);
	lv_obj_align(obj, msg_obj, LV_ALIGN_CENTER, 0, -30);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SYSTIME_RESET_SUB));
}

static void setting_system_time_set_btn_up(lv_obj_t *obj)
{
	setting_time_enter_flag_set(0x02);
	goto_layout(pLAYOUT(setting_time));
}

/***
** 日期: 2022-05-04 14:21
** 作者: leo.liu
** 函数作用：系统时间的按钮创建
** 返回参数说明：
***/
static void setting_etc_system_time_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_system_time_set_btn_up);
	lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 4, 622, 90,
								layout_setting_systime_string_get(SETTING_DATE_LANG_ID_SYS_TIME),
								NULL,
								&click_data,
								0);

	lv_page_glue_obj(btn, true);
}
/***
** 日期: 2022-05-04 14:21
** 作者: leo.liu
** 函数作用：系统复位的按钮创建
** 返回参数说明：
***/
static void setting_etc_system_reset_btn_create(lv_obj_t *parent)
{
	if(OwnID == 1)
	{
		static obj_click_data click_data = obj_click_data_up_create(setting_etc_system_reset_btn_up);
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 5, 622, 90,
								layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SYSTEM_RESET),
								NULL,
								&click_data,
								0);

		lv_page_glue_obj(btn, true);
	}
	else
	{
		static obj_click_data click_data = obj_click_data_up_create(setting_etc_system_reset_btn_up);
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 2, 622, 90,
								layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SYSTEM_RESET),
								NULL,
								&click_data,
								0);

		lv_page_glue_obj(btn, true);
	}

}


static void layout_weather_enable_btn_up(lv_obj_t *obj)
{
	bool status = lv_checkbox_is_checked(obj);
	user_data_get()->weather_enable = status;
	user_data_save();

	

}

static void setting_etc_weather_btn_create(lv_obj_t *parent)
{
	return;
	static obj_click_data click_data = obj_click_data_up_create(layout_weather_enable_btn_up);
	lv_obj_t *btn = setting_sub_btn_base_create(parent, 0, 90 + 90 * 7, 622, 90,
							layout_setting_etc_string_get(SETTING_ETC_LANG_ID_WEATHER),
		
							&click_data,
							user_data_get()->weather_enable,
							3);

	lv_page_glue_obj(btn, true);
}



static void setting_etc_security_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(security));
	
}

static void setting_etc_security_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_etc_security_btn_up);
	if(OwnID == 1)
	{
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 6, 622, 90,
							layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SECURITY),
							NULL,
							&click_data,
							0);
		lv_page_glue_obj(btn, true);
	}
	else
	{
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 3, 622, 90,
					layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SECURITY),
					NULL,
					&click_data,
					0);
		lv_page_glue_obj(btn, true);
	}
	
}



static void setting_etc_onvif_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(onvif_search));
	
}

static void setting_etc_onvif_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_etc_onvif_btn_up);
	if(OwnID == 1)
	{
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 7, 622, 90,
							layout_setting_etc_string_get(SETTING_ETC_LANG_ID_CCTV_MANAGE),
							NULL,
							&click_data,
							0);
		lv_page_glue_obj(btn, true);
	}
}


/***
** 日期: 2022-05-04 16:15
** 作者: leo.liu
** 函数作用：版本号显示
** 返回参数说明：
***/
static void setting_etc_version_btn_create(lv_obj_t *parent)
{
	static char sub_string[64] = {0};
	char serial[64] = {0};
	unsigned char uuid[128] = {0};
	unsigned char key[128] = {0};

	sprintf(sub_string, "%s", SYSTEM_VERSION);
	if (tuya_serial_number_get(serial) == true)
	{
		strcat(sub_string, " serial number:");
		strcat(sub_string, serial);
	}
	if(OwnID == 1)
	{
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 8, 622, 90,
						      layout_setting_etc_string_get(SETTING_ETC_LANG_ID_VERSION),
						      sub_string,
						      NULL,
						      0);
		lv_page_glue_obj(btn, true);
	}
	else
	{
		if(tuya_uuid_and_key_read(uuid, key) == false)
		{
			lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 5, 622, 90,
						layout_setting_etc_string_get(SETTING_ETC_LANG_ID_VERSION),
						sub_string,
						NULL,
						0);
			lv_page_glue_obj(btn, true);
		}
		else
		{
			lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 6, 622, 90,
						layout_setting_etc_string_get(SETTING_ETC_LANG_ID_VERSION),
						sub_string,
						NULL,
						0);
			lv_page_glue_obj(btn, true);
		}
		
	}
}
/***
** 日期: 2022-05-04 16:15
** 作者: leo.liu
** 函数作用：进入涂鸦info界面
** 返回参数说明：
***/

static void setting_etc_tuya_info_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(tuya_info));
}
/***
** 日期: 2022-05-04 16:15
** 作者: leo.liu
** 函数作用：uuid显示
** 返回参数说明：
***/

static void setting_etc_uuid_btn_create(lv_obj_t *parent)
{
	unsigned char uuid[128] = {0};
	unsigned char key[128] = {0};
	static char uid[128];
	if(tuya_uuid_and_key_read(uuid, key) == false){

		return ;
	}
	sprintf(uid, "%s", uuid);
	static obj_click_data click_data = obj_click_data_up_create(setting_etc_tuya_info_btn_up);

	if(OwnID == 1)
	{

		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 9, 622, 90,
						      layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TUYA_UUID),
						      uid,
						    &click_data,
						      0);
		lv_page_glue_obj(btn, true);
	}
	else
	{
		lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 5, 622, 90,
						layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TUYA_UUID),
						uid,
						&click_data,
						0);
		lv_page_glue_obj(btn, true);
	}
}



static void setting_etc_tp_check_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(click));
}

static void setting_etc_tp_check_btn_create(lv_obj_t *parent)
{
	unsigned char uuid[128] = {0};
	unsigned char key[128] = {0};
	static obj_click_data click_data = obj_click_data_up_create(setting_etc_tp_check_btn_up);

	if(OwnID == 1)
	{
		if(tuya_uuid_and_key_read(uuid, key) == false)
		{
			lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 9, 622, 90,
						      layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TP_CHECK),
						      NULL,
						    &click_data,
						      0);
			lv_page_glue_obj(btn, true);
		}
		else
		{
			lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 10, 622, 90,
						layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TP_CHECK),
						NULL,
					&click_data,
						0);
			lv_page_glue_obj(btn, true);
		}

	}
	else
	{
		if(tuya_uuid_and_key_read(uuid, key) == false)
		{
			lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 6, 622, 90,
						layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TP_CHECK),
						NULL,
						&click_data,
						0);
			lv_page_glue_obj(btn, true);
		}
		else
		{
			lv_obj_t *btn = setting_right_btn_base_create(parent, 0, 90 + 90 * 7, 622, 90,
						layout_setting_etc_string_get(SETTING_ETC_LANG_ID_TP_CHECK),
						NULL,
						&click_data,
						0);
			lv_page_glue_obj(btn, true);
		}
		
	}
}

// static void setting_etc_door2_open_time_btn_up(lv_obj_t *obj)
// {
	
// 	lv_obj_t *parent = lv_obj_get_parent(obj);
// 	lv_obj_t *text = lv_obj_get_child_form_id(parent, DOOR_2_OPEN_TIME_TEXT_ID);
// 	if(text == NULL)
// 	{
// 		LOG_RED("text not found \n");
// 		return ;
// 	}
// 	if(user_data_get()->etc.door_2_open_time < 15)
// 	{
// 		user_data_get()->etc.door_2_open_time += 1;
// 	}
// 	else if(user_data_get()->etc.door_2_open_time < 60)
// 	{
// 		user_data_get()->etc.door_2_open_time += 5;
// 	}
// 	else
// 	{
// 		user_data_get()->etc.door_2_open_time = 1;
// 	}
// 	user_data_save();
// 	lv_label_set_text_fmt(text, "%ds", user_data_get()->etc.door_2_open_time);
// }

/***
** 日期: 2022-05-04 14:20
** 作者: leo.liu
** 函数作用：door1 开锁模式创建274
** 返回参数说明：
***/
static void setting_etc_door2_open_time_btn_create(lv_obj_t *parent)
{
	static obj_click_data btn_data1 = obj_click_data_up_create(layout_etc_unclock_set_left_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create( layout_etc_unclock_set_right_btn_up);
	static char str[32] = {0};
	sprintf(str, "%ds",user_data_get()->etc.door_2_open_time);
	lv_obj_t *btn = sys_setting_etc_unclock_btn_create(parent,DOOR_2_OPEN_TIME_TEXT_ID,0, 90 + 90 * 2, 622, 90,
							layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DOOR_2_OPEN_TIME),
							str,
						    NULL,
							&btn_data1,
							&btn_data2);

	lv_page_glue_obj(btn, true);
}



/***
** 日期: 2022-05-04 15:40
** 作者: leo.liu
** 函数作用：创建页面控件
** 返回参数说明：
***/

static lv_obj_t *setting_page_obj_create(void)
{
	lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);
	if (page == NULL)
	{
		printf("create page failed \n");
		return NULL;
	}

	lv_obj_set_pos(page, 353, 75);
	lv_obj_set_size(page, 670, 600 - 75);
	lv_obj_set_id(page, LAYOUT_SETTING_ETC_OBJ_PAGE_ID);
	lv_page_get_scrollbar_mode(page);               //显示滚动条 ，注意设置背景透明度
	lv_obj_set_style_local_bg_opa(page, LV_PAGE_PART_SCROLLBAR, LV_STATE_DEFAULT, LV_OPA_COVER);
	return page;
}

static void LAYOUT_ENTER_FUNC(setting_etc)
{
	setting_cancel_btn_create(NULL);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	setting_main_btn_create(5);

	lv_obj_t *page = setting_page_obj_create();

	setting_etc_language_btn_create(page);
	
	setting_etc_id_btn_create(page);
	
	setting_etc_security_password_btn_create(page);

	setting_etc_system_time_btn_create(page);

	setting_etc_system_reset_btn_create(page);
	
	if(OwnID == 1)
	{
		setting_etc_door1_open_time_btn_create(page);
		
		setting_etc_door2_open_time_btn_create(page);

		setting_etc_weather_btn_create(page);
		
		setting_etc_onvif_btn_create(page);
	}

	setting_etc_security_btn_create(page);

	setting_etc_version_btn_create(page);

	setting_etc_uuid_btn_create(page);
	setting_etc_tp_check_btn_create(page);
}

static void LAYOUT_QUIT_FUNC(setting_etc)
{
#ifdef MAOTOUYING_DAN_IC
	backlight_enable(true);
#endif
    extern bool intercom_user_locktime_sync_send(unsigned int receive_id, unsigned char lock1_time,unsigned char lock2_time);

	intercom_user_locktime_sync_send(CODE_ALL_ID,user_data_get()->etc.door_1_open_time,user_data_get()->etc.door_2_open_time);
}

CREATE_LAYOUT(setting_etc);