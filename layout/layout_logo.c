#include "layout_define.h"
#include "video_input.h"
#include "audio_output.h"
#include "tuya/tuya_uuid_and_key.h"
#include "user_intercom.h"

extern bool LEO_FAST_ENTER_SYSTEM_FLAG;
static void logo_version_display(void);
/***
** 日期: 2022-04-25 17:08
** 作者: leo.liu
** 函数作用：logo图标显示
** 返回参数说明：
***/
static void logo_logo_icon_display(void)
{
#ifdef MAOTOUYING_DAN_IC
	return ;
#endif
//welcome
#if 1
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	static rom_bin_info info = rom_bin_info_get(ROM_UI_SX_LOGO_PNG);
	lv_obj_set_pos(obj, 240, 254);
	lv_obj_set_size(obj, 544, 91);

	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &info);
#endif
//onetouch
#if 0
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	static rom_bin_info info = rom_bin_info_get(ROM_UI_SX_LOGO_PNG);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 1024, 600);
	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &info);
#endif
}
// static void get_count_task(lv_task_t *task)
// {
// 	// printf("====video coun is %d====\n",tuya_online_num_get_customize());

// }
/***
** 日期: 2022-05-18 09:23
** 作者: leo.liu
** 函数作用：定时器初始化
** 返回参数说明：
***/
static void layout_logo_loding_task(lv_task_t *task)
{

	/*****  tuya api初始化 *****/
	tuya_api_init(TUYA_PID,&user_data_get()->wifi_enable);
	printf("\n\n\n======TUYA PID [%s] ======\n\n\n",TUYA_PID);

	user_time_init();
	/*****  警报列表初始化 *****/
	alarm_list_init();
	/*****初始化isp *****/
	video_input_init();
	/***** 初始化声卡设备 *****/
	audio_output_init();

	/***** 初始化音频采集设备 *****/
	audio_input_init();

	/***** 初始化铃声设备 *****/
	ringplay_init();

	/***** 初始化jpg解码设备 *****/
	jpg_decode_init();
	/*****  初始化h264解码器 *****/
	h264_decode_init();
	/***** 初始化mjpeg编码 *****/
	mjpeg_encode_init();
	/***** 初始化h264编码 *****/
	h264_encode_init();
	/***** 初始化记录 *****/
	video_record_init();
	/*****  初始化播放 *****/
	video_play_init();
	/*****  户户通处理线程 *****/
	intercom_init();
	/***** gpio口初始化 *****/
	layout_gpio_init();

	media_file_list_init();

	if(user_data_get()->door1_always_enable)
	{
		door1_power_enable(true);
	}
	if(user_data_get()->door2_always_enable)
	{
		door2_power_enable(true);
	}

	/*****  注册按键音信息 *****/
	rom_bin_info info = rom_bin_info_get(ROM_UI_KEY_SOUND_PCM);
	touch_sound_rom_info_register(&info);
	/***** 注册控件按下回调函数 *****/
	lv_obj_click_down_callback_register(layout_obj_click_down_func);
	/*****  call呼叫 *****/
	layout_door1_call_callback_register(layout_door1_call_default);
	layout_door2_call_callback_register(layout_door2_call_default);
	/*****  警报触发 *****/
	layout_alarm1_trigger_callback_register(layout_alarm1_trigger_default);
	layout_alarm2_trigger_callback_register(layout_alarm2_trigger_default);
	/*****  intercon 处理 *****/
	// layout_intercom_event_callback_register(layout_intercom_trigger_default);
	/*****  tuya处理事件 *****/
	layout_tuya_event_callback_register(layout_tuya_event_default);

	/***** 设置背光使能亮度 *****/
	backlight_brightness_set(user_data_get()->display.lcd_brigtness);
	backlight_enable(true);

	/***** 设置语言 *****/
	language_id_set(user_data_get()->etc.language);

	/***** 初始化tp9950 *****/
	tp9950_init();

	

	cctv_online_check_thread_init();

	if(LEO_FAST_ENTER_SYSTEM_FLAG == false){

		usleep(1000 * 1000);
	}
	/***** 开启待机侦测 *****/
	standby_timer_init(pLAYOUT(close), 30000);
	standby_timer_restart(true);

	/*****  设置按键音 *****/
	ringplay_touchsound_mute_set(user_data_get()->audio.key_sound ? false : true);

	if (OwnID == 1)
	{
		if (alarm_trigger_check() == false)
		{
			intercom_cmd_send(CODE_ALL_ID, CMD_DATA_REBOOT);
			goto_layout(pLAYOUT(home));
		}
		else if(task != NULL)
		{
			lv_task_del(task);
		}
	}
	else
	{
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_REBOOT);
		goto_layout(pLAYOUT(home));
	}

	/***复制flash的照片信息到sd卡的backup目录*/
	// flash_backup_to_sd();
	// extern lv_task_t *lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void *user_data);
	// lv_task_create(get_count_task, 1000, LV_TASK_PRIO_MID, NULL);
	fb_refresh_enable(true);
}
/***
**   日期:2022-05-27 08:16:55
**   作者: leo.liu
**   函数作用：跳转进入系统
**   参数说明:
***/
static void layout_logo_enter_system(void)
{
	if(LEO_FAST_ENTER_SYSTEM_FLAG == false){
		logo_logo_icon_display();
		lv_layout_task_create(layout_logo_loding_task, 1000, LV_TASK_PRIO_MID, NULL);
	}
	else{
		layout_logo_loding_task(NULL);
	}
	
}

typedef enum
{
	logo_input_scr_act_obj_id_input_txt,
} logo_input_scr_act_obj_id;
/***
**   日期:2022-06-06 16:15:22
**   作者: leo.liu
**   函数作用：创建输入文本
**   参数说明:
***/
static bool layout_logo_input_txt_create(void)
{
	lv_obj_t *textarea = lv_textarea_create(lv_scr_act(), NULL);
	if (textarea == NULL)
	{
		printf("create textarea fail \n");
		return false;
	}
	lv_obj_set_id(textarea, logo_input_scr_act_obj_id_input_txt);

	lv_obj_set_pos(textarea, 48, 70);
	lv_obj_set_size(textarea, 928, 40);
	lv_textarea_set_text(textarea, "");
	lv_textarea_set_text_align(textarea, LV_LABEL_ALIGN_LEFT);

	lv_obj_set_style_local_border_width(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, 1);
	lv_obj_set_style_local_border_color(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, lv_color_make(0x30, 0x30, 0x30));
	lv_obj_set_style_local_border_color(textarea, LV_TEXTAREA_PART_BG, LV_STATE_FOCUSED, lv_color_make(0x30, 0x30, 0x30));
	lv_obj_set_style_local_border_side(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);

	/*
	 *   修改文本颜色
	 */
	lv_obj_set_style_local_text_color(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x929292));
	lv_obj_set_style_local_value_color(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x929292));
	lv_obj_set_style_local_value_align(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
	lv_obj_set_style_local_value_ofs_x(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, 15);
	/*
	 *   禁止触摸点击光标
	 */
	lv_textarea_set_cursor_click_pos(textarea, false);
	lv_textarea_set_cursor_pos(textarea, 0);
	lv_textarea_set_one_line(textarea, true);
	lv_textarea_set_pwd_show_time(textarea, 500);
	lv_textarea_set_max_length(textarea, 5);
	return true;
}
/***
**   日期:2022-06-07 13:40:56
**   作者: leo.liu
**   函数作用：序列号提示输入
**   参数说明:
***/
static void logo_serial_numbe_txt_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 389, 40);
	lv_obj_set_size(obj, 246, 20);
	static const char *string = "Please enter the serial number";
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);
	lv_obj_set_style_local_value_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_value_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x0017FF));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
}
/***
** 日期: 2022-05-07 13:52
** 作者: leo.liu
** 函数作用：数字密码输入键盘
** 返回参数说明：
***/
lv_obj_t *logo_num_keyboard_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data)
{
	lv_obj_t *keyboard = lv_keyboard_create(parent == NULL ? lv_scr_act() : parent, NULL);
	if (keyboard == NULL)
	{
		printf("create keyboard failed \n");
		return NULL;
	}
	lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUM);
	lv_obj_set_pos(keyboard, x, y);
	lv_obj_set_size(keyboard, w, h);

	lv_obj_set_style_local_text_font(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, FONT_SIZE(40));

	lv_obj_set_style_local_border_opa(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED, LV_OPA_10);
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED, lv_color_hex(0x0096ff));

	lv_obj_set_style_local_radius(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, 60);
	lv_obj_set_style_local_pad_inner(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 30);
	//	lv_obj_set_style_local_pad_top(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 51);
	//	lv_obj_set_style_local_pad_left(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 41);
	//	lv_obj_set_style_local_pad_right(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 41);
	//	lv_obj_set_style_local_pad_bottom(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 54);

	static rom_bin_info img = rom_bin_info_get(ROM_UI_BTN_EXIT_PNG);
	lv_btnmatrix_set_pattern_image(keyboard, 10, &img);

	obj_click_event_listen(keyboard, click_data);

	lv_obj_t *textarea = lv_obj_get_child_form_id(lv_scr_act(), logo_input_scr_act_obj_id_input_txt);
	lv_keyboard_set_textarea(keyboard, textarea);
	return keyboard;
}
static void logo_num_keyboard_event(lv_obj_t *kb, lv_state_t event)
{
	if (event != LV_EVENT_VALUE_CHANGED)
	{
		return;
	}

	if (event != LV_EVENT_VALUE_CHANGED)
		return;

	lv_keyboard_ext_t *ext = lv_obj_get_ext_attr(kb);
	uint16_t btn_id = lv_btnmatrix_get_active_btn(kb);
	if (btn_id == LV_BTNMATRIX_BTN_NONE)
		return;
	if (lv_btnmatrix_get_btn_ctrl(kb, btn_id, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED))
		return;
	if (lv_btnmatrix_get_btn_ctrl(kb, btn_id, LV_BTNMATRIX_CTRL_NO_REPEAT) && event == LV_EVENT_LONG_PRESSED_REPEAT)
		return;

	const char *txt = lv_btnmatrix_get_active_btn_text(kb);
	if (txt == NULL)
		return;

	if (btn_id == 10)
	{
		int pos = lv_textarea_get_cursor_pos(ext->ta);
		if (pos == 0)
		{
			return;
		}
		lv_textarea_del_char(ext->ta);
		return;
	}
	lv_textarea_add_text(ext->ta, txt);
}
static void layout_logo_ok_btn_up(lv_obj_t *obj)
{
	char *pinput_cont = (char *)obj->user_data;
	static char input_buffer[6] = {0};
	obj = lv_obj_get_child_form_id(lv_scr_act(), logo_input_scr_act_obj_id_input_txt);
	int pos = lv_textarea_get_cursor_pos(obj);
	if (pos == 0)
	{
		return;
	}
	if ((*pinput_cont) == 0)
	{
		memset(input_buffer, 0, sizeof(input_buffer));
		strcpy(input_buffer, lv_textarea_get_text(obj));
		lv_textarea_set_text(obj, "");
		lv_textarea_set_cursor_pos(obj, 0);
		(*pinput_cont)++;
	}
	else if ((*pinput_cont) == 1)
	{
		if (strcmp(input_buffer, lv_textarea_get_text(obj)) == 0)
		{
			int index;
			sscanf(input_buffer, "%d", &index);
			if (tuya_key_and_key_xls_register(index) == true)
			{
				system("umount /mnt");
				lv_obj_clean(lv_scr_act());
				logo_version_display();
				layout_logo_enter_system();
			}
		}
		else
		{
			lv_textarea_set_text(obj, "");
			lv_textarea_set_cursor_pos(obj, 0);
			(*pinput_cont) = 0;
		}
	}
}

static void layout_logo_ok_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 355, 500);
	lv_obj_set_size(obj, 96, 96);
	static const char *string = "OK";
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);

	lv_obj_set_style_local_border_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, LV_OPA_10);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FF00));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x0096ff));

	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 60);
	lv_obj_set_style_local_pad_inner(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 30);

	static obj_click_data click_data = obj_click_data_up_create(layout_logo_ok_btn_up);
	obj_click_event_listen(obj, &click_data);

	static char input_cont = 0;
	input_cont = 0;
	obj->user_data = &input_cont;
}
/***
**   日期:2022-07-13 13:41:14
**   作者: leo.liu
**   函数作用：显示版本号
**   参数说明:
***/
static void logo_version_display(void)
{
	#ifdef MAOTOUYING_DAN_IC
	return;
	#endif
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 1024 - 200 - 50, 600 - 40 - 0);
	lv_obj_set_size(label, 200, 40);
	lv_label_set_align(label, LV_LABEL_ALIGN_RIGHT);
	lv_label_set_text_fmt(label, "%s", SYSTEM_VERSION);
	lv_obj_set_top(label, true);
}
static void LAYOUT_ENTER_FUNC(logo)
{
	if(LEO_FAST_ENTER_SYSTEM_FLAG == false){

		logo_version_display();
	}
	
	if (tuya_key_and_uuid_init() == false)
	{
		fb_refresh_enable(true);
		logo_serial_numbe_txt_create();
		layout_logo_input_txt_create();
		static obj_click_data click_data = obj_click_data_anything_create(logo_num_keyboard_event);
		logo_num_keyboard_create(NULL, 360, 170, 302, 420, &click_data);
		layout_logo_ok_btn_create();
	}
	else
	{
		layout_logo_enter_system();
	}
}

static void LAYOUT_QUIT_FUNC(logo)
{
}

CREATE_LAYOUT(logo);