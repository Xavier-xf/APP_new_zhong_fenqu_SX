#include "layout_define.h"
#include "tuya/tuya_uuid_and_key.h"
#include "lvgl/drive/lv_jpg_decode.h"
#include "lvgl/drive/lv_qrcode.h"
#include "lvgl/drive/qrcodegen.h"
#include "wifi_api.h"

/***** 显示时间的文本控件 *****/
#define LAYOUT_HOME_OBJ_TIME_1_TEXT_ID 0x01
#define LAYOUT_HOME_OBJ_TIME_2_TEXT_ID 0x02
#define LAYOUT_HOME_OBJ_DATE_TEXT_ID 0X03
#define LAYOUT_HOME_OBJ_NEW_OBJ_ID 0X04
#define LAYOUT_HOME_OBJ_SD_OBJ_ID 0X05
#define LAYOUT_HOME_OBJ_STANDBY_BTN_OBJ_ID 0X06
#define LAYOUT_HOME_OBJ_MONITOR_OBJ_ID 0X08
#define LAYOUT_HOME_OBJ_CCTV_OBJ_ID 0X09
#define LAYOUT_HOME_OBJ_INTERCOM_OBJ_ID 0X0A
#define LAYOUT_HOME_OBJ_TUYA_LABEL_ID 0X0B
#define LAYOUT_HOME_OBJ_NETWORK_ID 0X0C
#define LAYOUT_HOME_OBJ_AQI_ID 0X0D
#define LAYOUT_HOME_OBJ_CCTV_SIGN_ID 0X0F
#define LAYOUT_HOME_OBJ_MODE_BTN_ID 0XA0
#define LAYOUT_HOME_OBJ_TIME_MIAOHAO_OBJ_ID 0xA1
#define LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID 0xA2
#define LAYOUT_HOME_OBJ_WIFI_QR_OBJ_ID 30
#define LAYOUT_HOME_OBJ_WEATHER_TEMP_ID 31
#define LAYOUT_HOME_OBJ_WEATHER_CONDITION_LEBAL_ID 32
#define LAYOUT_HOME_OBJ_WEATHER_TRMP_RADIUS_ID 33
#define LAYOUT_HOME_OBJ_WEATHER_CONDITION_ION_ID 34


extern bool tuya_api_weather_get(tuya_api_weather *nm);

static unsigned char *mac_info = NULL;
lv_task_t * mac_qr_info = NULL;


int weather_temp_check = 0;
int weather_condition_check = 0;
/*************************************************************************
 * @brief  获取MODE这个按钮的ID,用于TUYA APP 切换状态时,更新这个按钮的显示
 * @date   2022-10-09 08:40
 * @author xiaoele
 **************************************************************************/
int home_mode_btn_id_get(void)
{
	return LAYOUT_HOME_OBJ_MODE_BTN_ID;
}


/***
** 日期: 2022-04-26 08:24
** 作者: leo.liu
** 函数作用：home页面按钮的基类创建
** 返回参数说明：
***/
static lv_obj_t *home_btn_base_create(int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *image, const char *string, int obj_id)
{
	lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
	if (btn == NULL)
	{
		printf("home monitor btn create failed \n");
		return false;
	}
	if (obj_id > 0)
	{
		lv_obj_set_id(btn, obj_id);
	}
	lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);

	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	if (image != NULL)
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, image);
		lv_obj_set_style_local_pattern_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_TOP_MID);
	}

	if (string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, string);
		lv_obj_set_style_local_value_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
	}

	if (click_data != NULL)
	{
		obj_click_event_listen(btn, click_data);
	}

	return btn;
}


static lv_obj_t *home_setting_btn_base_create(int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *image, const char *string, int obj_id)
{
	lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
	if (btn == NULL)
	{
		printf("home monitor btn create failed \n");
		return false;
	}
	if (obj_id > 0)
	{
		lv_obj_set_id(btn, obj_id);
	}
	lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);

	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	if (image != NULL)
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, image);
		lv_obj_set_style_local_pattern_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	}

	if (string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, string);
		lv_obj_set_style_local_value_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
	}

	if (click_data != NULL)
	{
		obj_click_event_listen(btn, click_data);
	}

	return btn;
}
/***
** 日期: 2022-04-26 09:41
** 作者: leo.liu
** 函数作用：监控按钮被点击
** 返回参数说明：
***/
static void home_monitor_btn_up(lv_obj_t *obj)
{
	if (intercom_data_busy_get() == true)
	{
		return;
	}

	// if ((monitor_valid_channel_check(MON_CH_DOOR1) == false) && (monitor_valid_channel_check(MON_CH_DOOR2) == false))
	// {
	// 	message_window_pops(layout_home_string_get(HOME_LANG_ID_DOOR_LEBAL));
	// 	return;
	// }
	if (OwnID == 1)
	{
		if ((monitor_valid_channel_check(MON_CH_DOOR1) == false) && (monitor_valid_channel_check(MON_CH_DOOR2) == false))
		{
			message_window_home_pops(layout_home_string_get(HOME_LANG_ID_DOOR_LEBAL));
			return;
		}

		if (monitor_valid_channel_check(MON_CH_DOOR1) == true)
		{
			monitor_channel_set(MON_CH_DOOR1);
		}
		else
		{
			monitor_channel_set(MON_CH_DOOR2);
		}
		
		intercom_mastar_id_set(OwnID);
		intercom_data_busy_enbale(true);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);

		monitor_enter_mask_set(MON_ENTER_MANUAL_DOOR);
		goto_layout(pLAYOUT(monitor));
	}
	else
	{
		intercom_cmd_send(1, CMD_DOOR_ENTER);
	}
}

/***
** 日期: 2022-04-26 08:18
** 作者: leo.liu
** 函数作用：主界面监控图标按钮
** 返回参数说明：
***/
static bool home_monitor_btn_display(void)
{
	static rom_bin_info image = rom_bin_info_get(ROM_UI_HOME_MONITOR_NEW_PNG);
	static obj_click_data click_data = obj_click_data_up_create(home_monitor_btn_up);
	home_btn_base_create(137, 399, 100,146, &click_data, &image, layout_home_string_get(HOME_LANG_ID_MONITOR), LAYOUT_HOME_OBJ_MONITOR_OBJ_ID);
	return true;
}

static void home_CCTV_btn_up(lv_obj_t *obj)
{
	
	if (intercom_data_busy_get() == true)
	{
		return;
	}
	if ((monitor_valid_channel_check(MON_CH_CCTV1) == false) && 
		(monitor_valid_channel_check(MON_CH_CCTV2) == false) && 
		(monitor_valid_channel_check(MON_CH_CCTV3) == false) && 
		(monitor_valid_channel_check(MON_CH_CCTV4) == false))
	{
		message_window_home_pops(layout_home_string_get(HOME_LANG_ID_CCTV_LEBAL));
		return;
	}
	if (OwnID == 1)
	{
		if (monitor_valid_channel_check(MON_CH_CCTV1) == true)
		{
			monitor_channel_set(MON_CH_CCTV1);
		}
		else if (monitor_valid_channel_check(MON_CH_CCTV2) == true)
		{
			monitor_channel_set(MON_CH_CCTV2);
		}
		else if (monitor_valid_channel_check(MON_CH_CCTV3) == true)
		{
			monitor_channel_set(MON_CH_CCTV3);
		}
		else if (monitor_valid_channel_check(MON_CH_CCTV4) == true)
		{
			monitor_channel_set(MON_CH_CCTV4);
		}
		intercom_mastar_id_set(OwnID);
		intercom_data_busy_enbale(true);
		LOG_BLUE("分机挂断3");
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
		monitor_enter_mask_set(MON_ENTER_MANUAL_CCTV);
		goto_layout(pLAYOUT(monitor));
	}
	else
	{
		intercom_cmd_send(1, CMD_CCTV_ENTER);
	}

	//	monitor_enter_mask_set(MON_ENTER_MANUAL_CCTV);
	//	monitor_channel_set(MON_CH_CCTV1);
	//	goto_layout(pLAYOUT(monitor));
}

/***
** 日期: 2022-04-26 09:42
** 作者: leo.liu
** 函数作用：主界面CCTV图标按钮
** 返回参数说明：
***/
static bool home_CCTV_btn_display(void)
{
	static rom_bin_info image = rom_bin_info_get(ROM_UI_HOME_CCTV_NEW_PNG);
	static obj_click_data click_data = obj_click_data_up_create(home_CCTV_btn_up);
	home_btn_base_create(302, 399, 100, 146, &click_data, &image, layout_home_string_get(HOME_LANG_ID_CCTV), LAYOUT_HOME_OBJ_CCTV_OBJ_ID);
	return true;
}

/***
**   日期:2022-05-31 14:52:11
**   作者: leo.liu
**   函数作用：防止同时按下进入intercom
**   参数说明:
***/
static void home_inter_btn_up(lv_obj_t *obj)
{
	// lv_task_del(task_t);
	if (intercom_data_busy_get() == false)
	{
		intercom_mastar_id_set(OwnID);
		LOG_BLUE("分机挂断4");
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
		goto_layout(pLAYOUT(intercom));
	}
}

/***
** 日期: 2022-04-26 09:42
** 作者: leo.liu
** 函数作用：主界面户户通图标按钮
** 返回参数说明：
***/
static bool home_inter_btn_display(void)
{
	static rom_bin_info image = rom_bin_info_get(ROM_UI_HOME_INTERCOM_NEW_PNG);
	static obj_click_data click_data = obj_click_data_up_create(home_inter_btn_up);
	home_btn_base_create(467, 399, 100,146, &click_data, &image, layout_home_string_get(HOME_LNAG_ID_INTER), LAYOUT_HOME_OBJ_INTERCOM_OBJ_ID);
	return true;
}

static void home_playback_btn_up(lv_obj_t *obj)
{
	layout_playback_parm_init();
	goto_layout(pLAYOUT(playback));
}

/***
** 日期: 2022-04-26 09:42
** 作者: leo.liu
** 函数作用：主界面回放图标按钮
** 返回参数说明：
***/
static bool home_playback_btn_display(void)
{
	static rom_bin_info image = rom_bin_info_get(ROM_UI_HOME_BACKPLAY_NEW_PNG);
	static obj_click_data click_data = obj_click_data_up_create(home_playback_btn_up);
	home_btn_base_create(632, 399, 100, 146, &click_data, &image, layout_home_string_get(HOME_LANG_ID_PLAYBACK), 0);
	return true;
}




void  home_mode_btn_display(lv_obj_t *obj)
{
	static int home_mode_lan = 0;
	static rom_bin_info image = rom_bin_info_get(ROM_UI_HOME_NEW_AT_HOME_PNG);

	switch(user_data_get()->home_mode)
	{
		case 0:
			// user_data_get()->home_mode = 1;
			home_mode_lan = HOME_LANG_ID_LEAVE_HOME;
			image.offset = ROM_UI_HOME_NEW_LEAVE_PNG;
			image.size = ROM_UI_HOME_NEW_LEAVE_PNG_SIZE;
			break;
		case 1:
			// user_data_get()->home_mode = 2;
			home_mode_lan = HOME_LANG_ID_DORMANCY_HOME;
			image.offset = ROM_UI_HOME_NEW_DORMANCY_PNG;
			image.size = ROM_UI_HOME_NEW_DORMANCY_PNG_SIZE;
			break;
		case 2:
			// user_data_get()->home_mode = 0;
			home_mode_lan = HOME_LANG_ID_AT_HOME;
			image.offset = ROM_UI_HOME_NEW_AT_HOME_PNG;
			image.size = ROM_UI_HOME_NEW_AT_HOME_PNG_SIZE;
			break;
		default:
			break;
	}

	/* 更换图标 */
	// if(icon != NULL)
	// {
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image);
	// }

	/* 更换文字 */
	// if(text != NULL)
	// {
		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_home_string_get(home_mode_lan));
	// }
 
	// user_data_save();
	// goto_layout(pLAYOUT(home));
}

static void home_at_home_mode_btn_up(lv_obj_t *obj)
{
	if(OwnID == 1)
	{
		LOG_WHITE("click\n");
		user_data_get()->home_mode += 1;
		if (user_data_get()->home_mode >= 3)
		{
			user_data_get()->home_mode = 0;
		}
		user_data_save();
		home_mode_btn_display(obj);
		// lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_UPDATE_MODE,  0);
		tuya_api_home_mode_report(user_data_get()->home_mode);
	}
	else
	{
		if (user_data_get()->home_mode == 2)
		{
			user_data_get()->home_mode = 1;
		}
		else 
		{
			user_data_get()->home_mode = 2;
		}
		user_data_save();
		home_mode_btn_display(obj);
	}
}
/***
** 日期: 2022-04-26 09:42
** 作者: leo.liu
** 函数作用：主界面情景模式图标按钮
** 返回参数说明：
***/
static bool home_home_mode_btn_display(void)
{
	static int home_mode_lan = 0;
	static rom_bin_info image = rom_bin_info_get(ROM_UI_HOME_NEW_AT_HOME_PNG);


	switch(user_data_get()->home_mode)
	{
		case 2:
			home_mode_lan = HOME_LANG_ID_AT_HOME;
			image.offset = ROM_UI_HOME_NEW_AT_HOME_PNG;
			image.size = ROM_UI_HOME_AT_HOME_PNG_SIZE;
			break;
		case 0:
			home_mode_lan = HOME_LANG_ID_LEAVE_HOME;
			image.offset = ROM_UI_HOME_NEW_LEAVE_PNG;
			image.size = ROM_UI_HOME_NEW_LEAVE_PNG_SIZE;
			break;
		case 1:
			home_mode_lan = HOME_LANG_ID_DORMANCY_HOME;
			image.offset = ROM_UI_HOME_NEW_DORMANCY_PNG;
			image.size = ROM_UI_HOME_NEW_DORMANCY_PNG_SIZE;
			break;
		default:
			break;
	}
	// if(OwnID != 1)
	// {
	// 	home_mode_lan = HOME_LANG_ID_AT_HOME;
	// 	image.offset = ROM_UI_HOME_NEW_AT_HOME_PNG;
	// 	image.size = ROM_UI_HOME_AT_HOME_PNG_SIZE;
	// }
	static obj_click_data click_data = obj_click_data_up_create(home_at_home_mode_btn_up);
	home_btn_base_create(806, 399, 100, 146, &click_data, &image, layout_home_string_get(home_mode_lan), LAYOUT_HOME_OBJ_MODE_BTN_ID);

	return true;
}


/***
** 日期: 2022-04-26 09:53
** 作者: leo.liu
** 函数作用：设置按钮点击回调函数
** 返回参数说明：
***/
static void home_setting_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_record));
}

/***
** 日期: 2022-04-26 09:50
** 作者: leo.liu
** 函数作用：主界面设置按钮图标
** 返回参数说明：
***/
static bool home_setting_btn_display(void)
{
	static rom_bin_info image = rom_bin_info_get(ROM_UI_02_BTN_BTN_TITLE_SETTING_PNG);
	static obj_click_data click_data = obj_click_data_up_create(home_setting_btn_up);
	home_setting_btn_base_create(0, 0, 120, 120, &click_data, &image, NULL, 0);
	return true;
}


static void layout_home_sd_icon_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_SD_OBJ_ID);
	
	lv_obj_set_state(obj,media_sdcard_insert_check() == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);

	// if(media_sdcard_insert_check() == true)
	// {
		// copy_flash_photo_to_sd();
	// }
	// if((media_sdcard_insert_check() == false) && (user_data_get()->auto_record_mode == 2)) {
	// 	user_data_get()->auto_record_mode = 1;
	// }
}

/***
** 日期: 2022-05-17 14:34
** 作者: leo.liu
** 函数作用：sd创建
** 返回参数说明：
***/
static void layout_home_sd_icon_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, LAYOUT_HOME_OBJ_SD_OBJ_ID);
	lv_obj_set_pos(obj, 863, 37);
	lv_obj_set_size(obj, 33, 39.11);
	lv_obj_set_click(obj, false);
	static rom_bin_info img_n = rom_bin_info_get(ROM_UI_MONITOR_NO_SD_PNG);
	static rom_bin_info img_f = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_SDCARD_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_n);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &img_f);
	layout_home_sd_icon_display();
}

/***
** 日期: 2022-04-26 11:06
** 作者: leo.liu
** 函数作用：创建显示时间的文本
** 返回参数说明：
***/
bool home_time_text_create(lv_obj_t *parent, int x, int y, int w, int h, int hour_id, int min_id, int dot_id)
{
	/***** 创建小时label控件 *****/
	lv_obj_t *label = lv_label_create(parent, NULL);
	if (label == NULL)
	{
		printf("home create time(hour) label failed \n");
		return false;
	}
	/***** 设置ID，定时器需要定时刷新 *****/
	if (hour_id > 0)
	{
		lv_obj_set_id(label, hour_id);
	}

	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	/***** 设置控件大小文本信息 *****/
	lv_obj_set_pos(label, x, y);
	lv_obj_set_size(label, (w - 40) / 2, h);
	lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(150));
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);

	/***** 创建分钟label控件 *****/
	lv_obj_t *label1 = lv_label_create(parent, label);
	if (label1 == NULL)
	{
		printf("home create time(min) label failed \n");
		return false;
	}
	if (min_id > 0)
	{
		lv_obj_set_id(label1, min_id);
	}
	lv_obj_set_x(label1, x + w / 2 + 20);

	/***** 创建":"文本控件 *****/
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	if (obj == NULL)
	{
		printf("home create time(:) obj failed \n");
		return false;
	}
	if (dot_id > 0)
	{
		lv_obj_set_id(obj, dot_id);
	}
	lv_obj_set_pos(obj, x + (w - 40) / 2 + 15, y + 50);
	lv_obj_set_size(obj, 10, 70);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(150));
	static char *dot = ":";
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, dot);
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	return true;
}

/***
** 日期: 2022-04-26 11:59
** 作者: leo.liu
** 函数作用：创建显示日期的文本控件
** 返回参数说明：
***/
bool home_date_text_create(lv_obj_t *parent, int x, int y, int w, int h, int obj_id)
{
	lv_obj_t *label = lv_label_create(parent, NULL);
	if (label == NULL)
	{
		printf("home create date label failed \n");
		return false;
	}
	/***** 设置ID，定时器需要定时刷新 *****/
	if (obj_id > 0)
	{
		lv_obj_set_id(label, obj_id);
	}
	lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);

	lv_obj_set_pos(label, x, y);
	lv_obj_set_size(label, w, h);
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
	return true;
}

/***
** 日期: 2022-05-10 18:00
** 作者: leo.liu
** 函数作用：时间显示
** 返回参数说明：
***/
void home_time_refresh_display(lv_obj_t *parent, struct tm *tm, int hour_id, int min_id, int dot_id, bool blink)
{
	lv_obj_t *label_hour = lv_obj_get_child_form_id(parent, hour_id);
	if (label_hour != NULL)
	{
		lv_label_set_text_fmt(label_hour, "%02d", tm->tm_hour);
	}

	lv_obj_t *label_min = lv_obj_get_child_form_id(parent, min_id);
	if (label_min != NULL)
	{
		lv_label_set_text_fmt(label_min, "%02d", tm->tm_min);
	}

	if (dot_id != 0)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(parent, dot_id);
		bool state = lv_obj_get_hidden(obj);
		if (blink == true)
		{
			lv_obj_set_hidden(obj, state ? false : true);
		}
		else if (state == true)
		{
			lv_obj_set_hidden(obj, false);
		}
	}
}

/***
** 日期: 2022-05-11 08:07
** 作者: leo.liu
** 函数作用：日期显示
** 返回参数说明：
***/
void home_date_refresh_display(lv_obj_t *parent, struct tm *tm, int date_id)
{
	lv_obj_t *label_date = lv_obj_get_child_form_id(parent, date_id);
	if (label_date == NULL)
	{
		printf("not find child obj date \n");
		return;
	}

	const char *week_str = layout_home_string_get(HOME_LANG_ID_WEEK_1 + tm->tm_wday - 1);
	const char *mon_str = layout_home_string_get(HOME_LANG_ID_MONTH_1 + tm->tm_mon - 1);

	LANGUAGE_ID lang = language_id_get();
	if (lang == LANGUAGE_ID_ENGLISH)
	{
		// printf("%d %d %d %d\n",tm->tm_wday,tm->tm_mday,tm->tm_mon,tm->tm_year);
		lv_label_set_text_fmt(label_date, "%s, %d  %s  %04d", week_str,tm->tm_mday, mon_str, tm->tm_year);
	}
	// else if (lang == LANGUAGE_ID_HANYU)
	// {
	// 	lv_label_set_text_fmt(label_date, "%04d년%d월%d일,%s", tm->tm_year, tm->tm_mon, tm->tm_mday, week_str);
	// }
	// else if (lang == LANGUAGE_ID_ELUOSI)
	// {
	// 	lv_label_set_text_fmt(label_date, "%s,%s,%d,%04d", week_str, mon_str, tm->tm_mday, tm->tm_year);
	// }
	// else if (lang == LANGUAGE_ID_XIBANYA)
	// {
	// 	lv_label_set_text_fmt(label_date, "%s,%d-%s-%04d", week_str, tm->tm_mday, mon_str, tm->tm_year);
	// }
	// else if (lang == LANGUAGE_ID_CHINESE)
	// {
	// 	lv_label_set_text_fmt(label_date, "%04d年%d月%d日,%s", tm->tm_year, tm->tm_mon, tm->tm_mday, week_str);
	// }
	// else if (lang == LANGUAGE_ID_TUERQI)
	// {
	// 	lv_label_set_text_fmt(label_date, "%d %s %04d,%s", tm->tm_mday, week_str, tm->tm_year, mon_str);
	// }
	// else if (lang == LANGUAGE_ID_BOLAN)
	// {
	// 	lv_label_set_text_fmt(label_date, "%s,%d %s %04d", week_str, tm->tm_mday, mon_str, tm->tm_year);
	// }
	// else if (lang == LANGUAGE_ID_JIEKE)
	// {
	// 	lv_label_set_text_fmt(label_date, "%s,%d %s %04d", week_str, tm->tm_mday, mon_str, tm->tm_year);
	// }
	else if (lang == LANGUAGE_ID_ALABOYU)
	{
		lv_label_set_text_fmt(label_date, "%s, %s %d, %04d", week_str, mon_str, tm->tm_mday, tm->tm_year);
	}
	// else if (lang == LANGUAGE_ID_PUTAOYA)
	// {
	// 	lv_label_set_text_fmt(label_date, "%s, %s %d, %04d", week_str, mon_str, tm->tm_mday, tm->tm_year);
	// }
	// else if (lang == LANGUAGE_ID_FAYU)
	// {
	// 	lv_label_set_text_fmt(label_date, "%s, %s %d, %04d", week_str, mon_str, tm->tm_mday, tm->tm_year);
	// }
	// else if (lang == LANGUAGE_ID_DEYU)
	// {
	// 	lv_label_set_text_fmt(label_date, "%s, %s %d, %04d", week_str, mon_str, tm->tm_mday, tm->tm_year);
	// }
}

/***
** 日期: 2022-04-26 14:26
** 作者: leo.liu
** 函数作用：时间刷新函数
** 返回参数说明：
***/
static void home_date_and_time_refresh_task(lv_task_t *task)
{
	
	// printf("================%d================%d========%d\n",1,1,tuya_online_num_get_customize());
	struct tm tm;
	user_time_read(&tm);
	home_time_refresh_display(lv_scr_act(), &tm, LAYOUT_HOME_OBJ_TIME_1_TEXT_ID, LAYOUT_HOME_OBJ_TIME_2_TEXT_ID, 0, false);
	home_date_refresh_display(lv_scr_act(), &tm, LAYOUT_HOME_OBJ_DATE_TEXT_ID);
}

/***
** 日期: 2022-04-26 10:24
** 作者: leo.liu
** 函数作用：主界面显示时间
** 返回参数说明：
***/
// static lv_task_t *task_t = NULL;
static void home_date_and_time_display(void)
{
	struct tm tm;
	user_time_read(&tm);
	tuya_api_weather weather;
	tuya_api_weather_get(&weather);
	// char serial[64] = {0};
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_TIME_1_TEXT_ID);
	if(obj != NULL)
	{
		lv_obj_del(obj);
	}
	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_TIME_2_TEXT_ID);
	if(obj != NULL)
	{
		lv_obj_del(obj);
	}
	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_TIME_MIAOHAO_OBJ_ID);
	if(obj != NULL)
	{
		lv_obj_del(obj);
	}
	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_DATE_TEXT_ID);
	if(obj != NULL)
	{
		lv_obj_del(obj);
	}

	// if((tuya_serial_number_get(serial) == true) && (user_data_get()->weather_enable) && (tuya_api_network_status() != 0x00) && (weather.condition != 0) && (tuya_api_online_check() == true))
	// {
	// 	home_time_text_create(lv_scr_act(), 197, 102, 270, 150, LAYOUT_HOME_OBJ_TIME_1_TEXT_ID, LAYOUT_HOME_OBJ_TIME_2_TEXT_ID, LAYOUT_HOME_OBJ_TIME_MIAOHAO_OBJ_ID);
	// 	home_date_text_create(lv_scr_act(), 230, 273, 200, 40, LAYOUT_HOME_OBJ_DATE_TEXT_ID);
	// }
	// else
	{
		home_time_text_create(lv_scr_act(), 370, 119, 270, 150, LAYOUT_HOME_OBJ_TIME_1_TEXT_ID, LAYOUT_HOME_OBJ_TIME_2_TEXT_ID, LAYOUT_HOME_OBJ_TIME_MIAOHAO_OBJ_ID);
		home_date_text_create(lv_scr_act(), 410, 273, 200, 40, LAYOUT_HOME_OBJ_DATE_TEXT_ID);
	}

	/***** 一秒钟跑一次 *****/
	// if(task_t == NULL)
	// {
		// task_t = 
		lv_layout_task_create(home_date_and_time_refresh_task, 1000, LV_TASK_PRIO_MID, NULL);
		home_date_and_time_refresh_task(NULL);
	// }
	
}

static void home_date_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(calendar));
}
/***
** 日期: 2022-05-09 16:02
** 作者: leo.liu
** 函数作用：创建点击日期的按钮
** 返回参数说明：
***/
static void home_date_btn_create(void)
{
	tuya_api_weather weather;
	tuya_api_weather_get(&weather);
	char serial[64] = {0};
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	if((tuya_serial_number_get(serial) == true) && (user_data_get()->weather_enable) && (tuya_api_network_status() != 0x00) && (weather.condition != 0) && (tuya_api_online_check() == true))
	{
		lv_obj_set_pos(obj, 197, 152);
		lv_obj_set_size(obj, 270, 140);
	}
	else
	{
		lv_obj_set_pos(obj, 412, 169);
		lv_obj_set_size(obj, 270, 140);
	}
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	static obj_click_data click_data = obj_click_data_up_create(home_date_btn_up);
	obj_click_event_listen(obj, &click_data);
}

/***
** 日期: 2022-05-09 16:02
** 作者: leo.liu
** 函数作用：主界面横线创建
** 返回参数说明：
***/
static void home_line_create(void)
{
	return;
	tuya_api_weather weather;
	tuya_api_weather_get(&weather);
	char serial[64] = {0};
	if((tuya_serial_number_get(serial) == false) || (user_data_get()->wifi_enable == 0) ||(tuya_api_online_check() == false))
	{
		return;
	}
	if(user_data_get()->weather_enable == 0)
	{
		return;
	}

	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 546, 150);
	lv_obj_set_size(obj, 2, 140);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));
	lv_obj_set_click(obj, false);


	if(tuya_api_network_status() != 0x02)
	{
		lv_obj_set_hidden(obj,true);
	}
	else if(weather.condition == 0)
	{
		lv_obj_set_hidden(obj,true);
	}
}









/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：温度文案显示
**   参数说明:
***/
static void home_weather_temp_display(void)
{
	tuya_api_weather weather;
	tuya_api_weather_get(&weather);

	lv_obj_t* label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_WEATHER_TEMP_ID);
	// if (tuya_api_online_check() == false)
	// {
	// 	return ;
	// }
	if(weather.condition == 0)
	{
		return ;
	}

	weather_temp_check = weather.temp;
	
	char str1[16] = {0};
	sprintf(str1,"%d℃",weather.temp);
	
	lv_label_set_text_fmt(label,"%s",str1);
	if((user_data_get()->weather_enable))
	{
		lv_obj_set_hidden(label,false);
	}	
	else
	{
		lv_obj_set_hidden(label,true);
	}	
	if((tuya_api_network_status() != 0x02) || (tuya_api_online_check() == false))
	{
		lv_obj_set_hidden(label,true);
	}

}

/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：天气情况文案显示
**   参数说明:
***/
static void home_weather_condition_display(void)
{
	tuya_api_weather weather;
	memset(&weather, 0, sizeof(weather));

	tuya_api_weather_get(&weather);
	
	if(weather.condition == 0)
	{
		return;
	}
	// if (tuya_api_online_check() == false)
	// {
	// 	return ;
	// }
	lv_obj_t* label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_WEATHER_CONDITION_LEBAL_ID);
	weather_condition_check = weather.condition;
	if((user_data_get()->weather_enable))
	{
		lv_obj_set_hidden(label,false);
	}
	else
	{
		lv_obj_set_hidden(label,true);
	}	
	if((tuya_api_network_status() != 0x02) || (tuya_api_online_check() == false))
	{
		lv_obj_set_hidden(label,true);
	}
	lv_label_set_text_fmt(label,"%s",layout_home_weather_string_get((weather.condition)-101));
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
}


/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：温度范围文案显示
**   参数说明:
***/
static void home_weather_temp_radius_display(void)
{
	tuya_api_weather weather;
	tuya_api_weather_get(&weather);

	if(weather.condition == 0)
	{
		return ;
	}
	// if (tuya_api_online_check() == false)
	// {
	// 	return ;
	// }
	lv_obj_t* label = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_WEATHER_TRMP_RADIUS_ID);
	char str1[16] = {0};
	sprintf(str1,"%d℃",weather.tlow);
	char str2[16] = {0};
	sprintf(str2,"%d℃",weather.thigh);
	char str3[16] = {0};
	if (tuya_api_online_check() == false)
	{
		sprintf(str3,"%s","Weather acquisition failure");
	}
	else
	{
		sprintf(str3,"%s~%s",str1,str2);
	}
	
	lv_label_set_text_fmt(label,"%s",str3);
	if((user_data_get()->weather_enable))
	{
		lv_obj_set_hidden(label,false);
	}	
	else
	{
		lv_obj_set_hidden(label,true);
	}	
	if((tuya_api_network_status() != 0x02) || (tuya_api_online_check() == false))
	{
		lv_obj_set_hidden(label,true);
	}
}



/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：天气情况图标显示
**   参数说明:
***/
static void home_weather_condition_ios_display(void)
{
	tuya_api_weather weather;
	tuya_api_weather_get(&weather);
	
	
	// if (tuya_api_network_status() != 0X02)
	// {
	// 	return false;
	// }
	// if (tuya_api_online_check() == false)
	// {
	// 	return false;
	// }
	if(weather.condition == 0)
	{
		return ;
	}
	lv_obj_t* obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_WEATHER_CONDITION_ION_ID);
	/*****  显示天气图标 *****/
	static rom_bin_info img = rom_bin_info_get(ROM_UI_CLOUD_PNG);
	switch (weather.condition)
	{
		case 101:	/* 大雨 */
			img.offset = ROM_UI_HEAVY_RAIN_PNG;
			img.size = ROM_UI_HEAVY_RAIN_PNG_SIZE;
			break;
		case 103:	/* 沙尘暴 */
			img.offset = ROM_UI_SAND_STORM_PNG;
			img.size = ROM_UI_SAND_STORM_PNG_SIZE;
			break;
		case 104:	/* 小雪 */
			img.offset = ROM_UI_MINOR_SNOW_PNG;
			img.size = ROM_UI_MINOR_SNOW_PNG_SIZE;
			break;
		case 113:	/* 雨夹雪 */
			img.offset = ROM_UI_SLEET_PNG;
			img.size = ROM_UI_SLEET_PNG_SIZE;
			break;
		case 119:	/* 大部晴朗 */
			img.offset = ROM_UI_SUNNY_PNG;
			img.size = ROM_UI_SUNNY_PNG_SIZE;
			break;
		case 120:	/* 晴 */
			img.offset = ROM_UI_SUNNY_PNG;
			img.size = ROM_UI_SUNNY_PNG_SIZE;
			break;
		case 124:	/* 大雪 */
			img.offset = ROM_UI_MAJOR_SNOW_PNG;
			img.size = ROM_UI_MAJOR_SNOW_PNG_SIZE;
			break;
		case 125:	/* 特大暴雨 */
			img.offset = ROM_UI_RAINSTORM_PNG;
			img.size = ROM_UI_RAINSTORM_PNG_SIZE;
			break;
		case 126:	/* 暴雪 */
			img.offset = ROM_UI_BLIZZARD_PNG;
			img.size = ROM_UI_BLIZZARD_PNG_SIZE;
			break;
		case 129:	/* 少云 */
			img.offset = ROM_UI_CLOUD_PNG;
			img.size = ROM_UI_CLOUD_PNG_SIZE;
			break;
		case 131:	/* 中雪 */
			img.offset = ROM_UI_MODERATE_SNOW_PNG;
			img.size = ROM_UI_MODERATE_SNOW_PNG_SIZE;
			break;
		case 132:	/* 阴 */
			img.offset = ROM_UI_CLOUDY_PNG;
			img.size = ROM_UI_CLOUDY_PNG_SIZE;
			break;
		case 139:	/* 小雨 */
			img.offset = ROM_UI_LIGHT_RAIN_PNG;
			img.size = ROM_UI_LIGHT_RAIN_PNG_SIZE;
			break;
		case 140:	/* 霾 */
			img.offset = ROM_UI_SMOG_PNG;
			img.size = ROM_UI_SMOG_PNG_SIZE;
			break;
		case 141:	/* 中雨 */
			img.offset = ROM_UI_MODERATE_RAIN_PNG;
			img.size = ROM_UI_MODERATE_RAIN_PNG_SIZE;
			break;
		case 142:	/* 多云 */
			img.offset = ROM_UI_OVERCAST_PNG;
			img.size = ROM_UI_OVERCAST_PNG_SIZE;
			break;
		default:
			img.offset = ROM_UI_CLOUDY_PNG;
			img.size = ROM_UI_CLOUDY_PNG_SIZE;
			break;
	}

	lv_obj_set_style_local_pattern_image(obj,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,&img);

	if((user_data_get()->weather_enable))
	{
		lv_obj_set_hidden(obj,false);
	}	
	else
	{
		lv_obj_set_hidden(obj,true);
	}

	if((tuya_api_network_status() != 0X02) || (tuya_api_online_check() == false))
	{
		lv_obj_set_hidden(obj,true);
	}		
}














/***
**   日期:2022-06-06 09:27:55
**   作者: leo.liu
**   函数作用：空气质量图标创建
**   参数说明:
***/
static void home_temp_ion_create(void)
{
	return;
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 570, 130);
	lv_obj_set_size(label, 150, 60);
	lv_obj_set_id(label,LAYOUT_HOME_OBJ_WEATHER_TEMP_ID);
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
	lv_obj_set_hidden(label,true);
	home_weather_temp_display();
}


/***
**   日期:2022-06-06 09:27:55
**   作者: leo.liu
**   函数作用：天气文本创建
**   参数说明:
***/
static void home_weather_label_cteate(void)
{
	return;
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
	lv_obj_set_pos(label, 570, 190);
	lv_obj_set_size(label, 150, 60);
	lv_obj_set_id(label,LAYOUT_HOME_OBJ_WEATHER_CONDITION_LEBAL_ID);
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
	lv_obj_set_hidden(label,true);
	home_weather_condition_display();
}

/***
**   日期:2022-06-06 09:27:55
**   作者: leo.liu
**   函数作用：温度文本创建
**   参数说明:
***/
static void home_temp_label_cteate(void)
{
	return;
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 570, 250);
	lv_obj_set_size(label, 150, 60);
	lv_obj_set_id(label,LAYOUT_HOME_OBJ_WEATHER_TRMP_RADIUS_ID);
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
	lv_obj_set_hidden(label,true);
	home_weather_temp_radius_display();
}

/***
**   日期:2022-06-06 09:27:55
**   作者: leo.liu
**   函数作用：天气图标创建
**   参数说明:
***/
static void home_weather_condition_icon_create(void)
{
	return;
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 700, 115);
	lv_obj_set_size(obj, 120, 120);
	lv_obj_set_click(obj, false);
	lv_obj_set_id(obj,LAYOUT_HOME_OBJ_WEATHER_CONDITION_ION_ID);
	lv_obj_set_hidden(obj,true);
	home_weather_condition_ios_display();
}

/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：创建new图标
**   参数说明:
***/
static void home_new_icon_create(void)
{
	lv_obj_t * img1 = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_style_local_bg_color(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFF0000));
	lv_obj_set_style_local_bg_opa(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_radius(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 15);
	lv_obj_set_pos(img1, 707, 393);
	lv_obj_set_size(img1, 35, 35);
	lv_obj_set_id(img1, ROM_UI_HOME_NEW_PNG);
	
    static char str[6] = {0};
	int new_total = 0;

	media_file_total_get(media_sdcard_insert_check() ? FILE_TYPE_SD_MIXED : FILE_TYPE_FLASH_PHOTO, NULL, &new_total);

	if((new_total) > 999)
	{
		sprintf(str, "%d", 999);
	}
	else
	{
		sprintf(str, "%d", new_total);
	}
	lv_obj_set_style_local_value_str(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, str);
	lv_obj_set_style_local_value_align(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
	lv_obj_set_style_local_value_ofs_y(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,-3);
	lv_obj_set_style_local_value_color(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
	lv_obj_set_style_local_value_font(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));

}


static void cctv_display_task(lv_task_t *task)
{
	if	((monitor_valid_channel_check(MON_CH_CCTV1) == true) || 
		(monitor_valid_channel_check(MON_CH_CCTV2) == true) || 
		(monitor_valid_channel_check(MON_CH_CCTV3) == true) || 
		(monitor_valid_channel_check(MON_CH_CCTV4) == true))
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_CCTV_SIGN_ID);
		lv_obj_set_hidden(obj,true);
		lv_task_del(task);
	}
}

void home_cctv_sign_icon_display(void)
{
	if((monitor_valid_channel_check(MON_CH_CCTV1) == true) && (monitor_valid_channel_check(MON_CH_CCTV2) == false))
	{
		if(OwnID == 1)
		{
			intercom_cmd_send(CODE_ALL_ID, CMD_CCTV1_SIGNAL_TRUE);
		}
		
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_CCTV_SIGN_ID);
		lv_obj_set_hidden(obj,true);
	}
	else if((monitor_valid_channel_check(MON_CH_CCTV2) == true) && (monitor_valid_channel_check(MON_CH_CCTV1) == false))
	{
		if(OwnID == 1)
		{
			intercom_cmd_send(CODE_ALL_ID, CMD_CCTV2_SIGNAL_TRUE);
		}
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_CCTV_SIGN_ID);
		lv_obj_set_hidden(obj,true);
	}
	else if	((monitor_valid_channel_check(MON_CH_CCTV1) == true) && (monitor_valid_channel_check(MON_CH_CCTV2) == true))
	{
		if(OwnID == 1)
		{
			intercom_cmd_send(CODE_ALL_ID, CMD_CCTV1_SIGNAL_TRUE);
			intercom_cmd_send(CODE_ALL_ID, CMD_CCTV2_SIGNAL_TRUE);
		}
		
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_CCTV_SIGN_ID);
		lv_obj_set_hidden(obj,true);
	}
	else if	((monitor_valid_channel_check(MON_CH_CCTV3) == true) || (monitor_valid_channel_check(MON_CH_CCTV4) == true))
	{		
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_HOME_OBJ_CCTV_SIGN_ID);
		lv_obj_set_hidden(obj,true);
	}
	lv_task_t *task =  lv_layout_task_create(cctv_display_task, 1000, LV_TASK_PRIO_MID, NULL);
	lv_task_ready(task);
}
/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：创建cctv连接状态图标
**   参数说明:
***/
static void home_cctv_sign_icon_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, LAYOUT_HOME_OBJ_CCTV_SIGN_ID);
	lv_obj_set_pos(obj, 377, 393);
	lv_obj_set_size(obj, 35, 35);
	lv_obj_set_click(obj, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_HOME_CCTV_SIGN_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	home_cctv_sign_icon_display();
}

static void home_busy_icon_display_task(lv_task_t *task)
{
	if(intercom_data_busy_get() == true)
	{
		lv_obj_t * obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID);
		if(obj != NULL)
		{
			lv_obj_set_hidden(obj,false);
		}
		obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID+1);
		if(obj != NULL)
		{
			lv_obj_set_hidden(obj,false);
		}
		obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID+2);
		if(obj != NULL)
		{
			lv_obj_set_hidden(obj,false);
		}
	}
	else
	{
		lv_obj_t * obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID);
		if(obj != NULL)
		{
			lv_obj_set_hidden(obj,true);
		}
		obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID+1);
		if(obj != NULL)
		{
			lv_obj_set_hidden(obj,true);
		}
		obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID+2);
		if(obj != NULL)
		{
			lv_obj_set_hidden(obj,true);
		}
	}

}



/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：繁忙状态图标显示
***/
static void home_busy_icon_create(void)
{
	static rom_bin_info img = rom_bin_info_get(ROM_UI_HOME_CCTV_SIGN_PNG);
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID);
	lv_obj_set_pos(obj, 212, 393);
	lv_obj_set_size(obj, 35, 35);
	lv_obj_set_click(obj, false);
	lv_obj_set_hidden(obj,true);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID+1);
	lv_obj_set_pos(obj, 377, 393);
	lv_obj_set_size(obj, 35, 35);
	lv_obj_set_click(obj, false);
	lv_obj_set_hidden(obj,true);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, LAYOUT_HOME_OBJ_BUSY_ICON_OBJ_ID+2);
	lv_obj_set_pos(obj, 542, 393);
	lv_obj_set_size(obj, 35, 35);
	lv_obj_set_click(obj, false);
	lv_obj_set_hidden(obj,true);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);


	lv_task_t * busy_icon_task = lv_layout_task_create(home_busy_icon_display_task,500, LV_TASK_PRIO_MID, NULL);
	lv_task_ready(busy_icon_task);
}


static void home_network_icon_display(lv_obj_t *obj)
{

	char state = tuya_api_network_status();
	if(tuya_api_online_check() == false)
	{
		if ((state == 0x00) || ( user_data_get()->wifi_enable == false))
		{
			lv_obj_set_state(obj, LV_STATE_DEFAULT);
		}
		else if (state == 0x01)
		{
			lv_obj_set_state(obj, LV_STATE_DISABLED);
		}
		else
		{
			lv_obj_set_state(obj, LV_STATE_CHECKED);
		}
	}
	else
	{
		lv_obj_set_state(obj, LV_STATE_CHECKED);
	}
	
}


static void  home_wifi_qr_display_task(lv_task_t *task)
{
	lv_obj_t *qr = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_WIFI_QR_OBJ_ID);
	if(qr)
	{
		lv_obj_set_hidden(qr,true);
	}	
	if(mac_qr_info != NULL){
		lv_task_del(mac_qr_info);
		mac_qr_info = NULL;
	}
}
static void wifi_btn_up(lv_obj_t *obj)
{
	lv_obj_t *qr = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_HOME_OBJ_WIFI_QR_OBJ_ID);
	if(qr == NULL)
	{
		return;
	}
	
	if(lv_obj_get_hidden(qr) == true)
	{
		lv_obj_set_hidden(qr,false);
	}
	else
	{
		lv_obj_set_hidden(qr,true);

		if(mac_qr_info != NULL){
			lv_task_del(mac_qr_info);
			mac_qr_info = NULL;
		}
		
	}


	if(mac_qr_info == NULL)
	{
		mac_qr_info = lv_layout_task_create(home_wifi_qr_display_task,10000, LV_TASK_PRIO_MID, NULL);
	}
	
}


/***
**   日期:2022-06-17 17:31:51
**   作者: leo.liu
**   函数作用：网络图标创建
**   参数说明:
***/
static lv_obj_t *home_network_icon_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, LAYOUT_HOME_OBJ_NETWORK_ID);
	lv_obj_set_pos(obj, 792, 30);
	lv_obj_set_size(obj, 46, 46);
	// lv_obj_set_click(obj, false);
	static rom_bin_info img_off = rom_bin_info_get(ROM_UI_HOME_WIFI_OFF_PNG);
	static rom_bin_info img_on = rom_bin_info_get(ROM_UI_WIFI_001_PNG);
	static rom_bin_info img_problem = rom_bin_info_get(ROM_UI_HOME_WIFI_PROBLEM_PNG);
	static rom_bin_info tuya_img = rom_bin_info_get(ROM_UI_TUYA_ONLINE_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_off);
	
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DISABLED, &img_problem);
	if(tuya_api_online_check() == false)
	{
		lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &img_on);
	}
	else
	{
		lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &tuya_img);
	}
	if(OwnID != 1)
	{
		lv_obj_set_hidden(obj,true);
	}
	static char online_state;
	online_state = tuya_api_network_status();
	obj->user_data = &online_state;
	home_network_icon_display(obj);

	static unsigned char ip_addr[128] = {0};
	mac_info = (unsigned char *)ak_mem_alloc(MODULE_ID_APP, 128 * 128 * 4);//MODULE_ID_APP

	// char *qr_code = NULL;
	memset(ip_addr, 0, sizeof(ip_addr));

	if(wifi_device_hwadders_get(ip_addr) == true)
	{
		static rom_bin_info img = rom_bin_raw_get();
		rom_bin_raw_init(img, mac_info, 90, 90);
		lv_obj_t *qr = lv_qrcode_create(lv_scr_act(), &img, 90, lv_color_hex(0x00), lv_color_hex(0xFFFFFF));
		lv_obj_set_pos(qr, 642, 30);
		lv_obj_set_id(qr,LAYOUT_HOME_OBJ_WIFI_QR_OBJ_ID);
		lv_obj_set_style_local_outline_opa(qr, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		// lv_obj_set_style_local_shadow_opa(qr, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
		lv_obj_set_style_local_outline_color(qr, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
		lv_obj_set_style_local_outline_width(qr, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, 10);
		char str[128] = {0};
		sprintf(str, "MAC:%s", ip_addr);
		memset(user_data_get()->mac_qr_info, 0, sizeof(user_data_get()->mac_qr_info));
		memcpy(user_data_get()->mac_qr_info,str, 128);

		lv_qrcode_update(qr, user_data_get()->mac_qr_info, strlen(user_data_get()->mac_qr_info));
		lv_obj_set_hidden(qr,true);
	}

	static obj_click_data click_data = obj_click_data_up_create(wifi_btn_up);
	obj_click_event_listen(obj, &click_data);

	return obj;
}
static void home_network_check_task(lv_task_t *task)
{
	lv_obj_t *obj = (lv_obj_t *)task->user_data;
	char *p_sate = (char *)obj->user_data;
	if ((*p_sate) != tuya_api_network_status())
	{
		*p_sate = tuya_api_network_status();
		home_network_icon_display(obj);
		// if(tuya_api_network_status() == 0x02)
		// {
		// 	home_date_and_time_display();
		// 	home_date_btn_create();
		// 	home_line_create();
		// 	home_temp_ion_create();
		// 	home_weather_label_cteate();
		// 	home_temp_label_cteate();
		// 	home_weather_condition_icon_create();
		// }
	}
}


static void  home_always_check_task(lv_task_t *task)
{
	extern bool always_record_time_get(void);
	if((intercom_data_busy_get() == false) && (always_record_time_get() == true))
	{
		goto_layout(pLAYOUT(always_record));
	}
}



static void layout_home_standby_btn_up(lv_obj_t *btn)
{
	goto_layout(pLAYOUT(close));
}


/***
**   日期:2022-05-26 11:45:03
**   作者: leo.liu
**   函数作用：创建一键待机图标
**   参数说明:
***/
static void layout_home_standby_btn_crate(void)
{
	static rom_bin_info image = rom_bin_info_get(ROM_UI_HOME_STANDBY_PNG);
	static obj_click_data click_data = obj_click_data_up_create(layout_home_standby_btn_up);
	home_btn_base_create(921, 28, 60, 60, &click_data, &image,NULL, LAYOUT_HOME_OBJ_STANDBY_BTN_OBJ_ID);
	return;
}



static void home_weather_check_task(lv_task_t *task)
{
	return;
	tuya_api_weather weather;
	tuya_api_weather_get(&weather);
	if((user_data_get()->weather_enable) || (tuya_api_network_status() == 0x02) || (weather.temp != weather_temp_check) || (weather.condition != weather_condition_check))
	{
		home_date_and_time_display();
		home_date_btn_create();
		home_line_create();
		home_weather_temp_display();
		home_weather_condition_display();
		home_weather_temp_radius_display();
		home_weather_condition_ios_display();
	}
}


static void home_tuya_online_task(lv_task_t *task)
{
	if(h264_encode_sample_get() == false){
		
		if(/*(tuya_app_preview_check() == true) &&*/ (tuya_online_num_get_customize() > 0)) 
		{
			goto_layout(pLAYOUT(tuya_monitor));
			return ;
		}
	}
}



static void LAYOUT_ENTER_FUNC(home)
{
	// MON_ENTER_FLG flg = monitor_enter_mask_get();
	// MON_CH ch = monitor_channel_get(); 
	if (alarm_trigger_check() == true)
	{
		return;
	}

	printf("h264_encode_sample_get:%d tuya_app_preview_check() = %d tuya_online_num_get_customize():%d\n",h264_encode_sample_get() ,tuya_app_preview_check(),tuya_online_num_get_customize());
	
	
	home_monitor_btn_display();
	home_CCTV_btn_display();
	home_inter_btn_display();
	home_playback_btn_display();
	home_home_mode_btn_display();
	home_setting_btn_display();
	layout_home_sd_icon_create();
	layout_home_standby_btn_crate();
	home_date_and_time_display();
	home_date_btn_create();
	home_line_create();
	home_temp_ion_create();
	home_weather_label_cteate();
	home_temp_label_cteate();
	home_weather_condition_icon_create();
	home_new_icon_create();
	home_cctv_sign_icon_create();
	home_busy_icon_create();
	// layout_sd_state_callback_register(home_new_icon_display);
	layout_sd_state_callback_register(layout_home_sd_icon_display);
	tuya_api_network_detect_enable(true);

	lv_layout_task_create(home_always_check_task,1000, LV_TASK_PRIO_MID, NULL);

	lv_layout_task_create(home_network_check_task, 1000, LV_TASK_PRIO_MID, home_network_icon_create());

	lv_layout_task_create(home_weather_check_task, 3000, LV_TASK_PRIO_MID, NULL);
	lv_layout_task_create(home_tuya_online_task, 100, LV_TASK_PRIO_MID, NULL);
#if 0
	static rom_bin_info img = rom_bin_raw_get();
	unsigned char *data = ak_mem_dma_alloc(MODULE_ID_ADEC, LV_HOR_RES_MAX * LV_VER_RES_MAX * LV_COLOR_SIZE / 8);
	rom_bin_raw_init(img, data, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_jpg_decode_data("/mnt/CDV1004QT/project/444.jpg", &img, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_disp_set_bg_image(lv_disp_get_default(), &img);

	lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
#endif
	// lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	// lv_obj_set_style_local_bg_color(lv_scr_act(),LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));

	// monitor_enter_mask_set(MON_ENTER_NONE);
}

static void LAYOUT_QUIT_FUNC(home)
{
	tuya_api_network_detect_enable(false);
	layout_sd_state_callback_register(NULL);
	if (mac_info != NULL)
	{
		ak_mem_free(mac_info);
		mac_info = NULL;
	}
	mac_qr_info = NULL;
}

CREATE_LAYOUT(home);