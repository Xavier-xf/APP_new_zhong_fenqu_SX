#include "layout_define.h"

typedef enum
{
	KEYBOARD_OBJ_ID = 0x01,
	TEXTAREA_ACCOUNT_ID,
	TEXTAREA_PASSWORD_ID,
	MSGBOX_OBJ_ID,
	ONVIF_INPUT_REFRESH_ID,
	wifi_input_scr_act_obj_id_keyboard,
	wifi_input_scr_act_obj_id_input_txt
} cctv_manage_input_enum;

enum
{
	INPUT_MODE_NULL,
	INPUT_MODE_ACCOUNT,
	INPUT_MODE_PASSWORD
};
static int input_mode = INPUT_MODE_NULL;

static int CURR_INDEX = -1;
// static void cctv_manage_input_screen_click_func(lv_obj_t *obj);

extern void onvif_preview_ip_set(const char *ip);
extern void onvif_preview_url_set(const char *url);
extern void onvif_preview_enter_flag_set(int flag);

extern lv_obj_t *cctv_manage_rtsp_url_create_template(lv_obj_t *parent, int num, const char *main_str, const char *sub_str, obj_click_data *click_cb, int obj_id);

void onvif_manage_curr_index_set(int num)
{
	CURR_INDEX = num;
}

int onvif_manage_curr_index_get(void)
{
	return CURR_INDEX;
}

static void cctv_manage_input_back_btn_up(lv_obj_t *obj)
{
	if (input_mode == INPUT_MODE_NULL)
	{
		goto_layout(pLAYOUT(onvif_search));
	}
	else
	{
		input_mode = INPUT_MODE_NULL;
		goto_layout(pLAYOUT(onvif_input));
	}
}


static void user_name_click_func(lv_obj_t *obj)
{
	input_mode = INPUT_MODE_ACCOUNT;
	goto_layout(pLAYOUT(onvif_input));
	// lv_textarea_set_cursor_hidden(xl_obj_find_by_id(TEXTAREA_ACCOUNT_ID), false);
	// lv_textarea_set_cursor_hidden(xl_obj_find_by_id(TEXTAREA_PASSWORD_ID), true);

	// lv_obj_t *kbd = xl_obj_find_by_id(KEYBOARD_OBJ_ID);
	// lv_keyboard_set_textarea(kbd, xl_obj_find_by_id(TEXTAREA_ACCOUNT_ID));
	// lv_obj_set_hidden(kbd, false);
}

static void user_name_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, user_name_click_func);
	setting_long_obj_template(lv_scr_act(), 1, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_ACCOUNT), -1, onvif_discover_info[CURR_INDEX].user, -1, false, SETTING_SUB_LABEL, &click_cb);
}

static void user_pswd_click_func(lv_obj_t *obj)
{
	input_mode = INPUT_MODE_PASSWORD;
	goto_layout(pLAYOUT(onvif_input));
}
static void user_pswd_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, user_pswd_click_func);
	setting_long_obj_template(lv_scr_act(), 2, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_PASSWORD), -1, onvif_discover_info[CURR_INDEX].pswd, -1, false, SETTING_SUB_LABEL, &click_cb);
}

static void rtsp_url_click_func(lv_obj_t *obj)
{
	printf("click:'%s'\n", lv_label_get_text(lv_obj_get_child(obj,NULL)));

	onvif_preview_url_set(lv_label_get_text(lv_obj_get_child(obj,NULL)));
	onvif_preview_enter_flag_set(1);

	onvif_preview_ip_set(onvif_discover_info[onvif_manage_curr_index_get()].ip);
	goto_layout(pLAYOUT(onvif_preview));
}

static void device_main_rtsp_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, rtsp_url_click_func);
	cctv_manage_rtsp_url_create_template(lv_scr_act(), 3, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_MAIN_STREAM), onvif_discover_info[CURR_INDEX].rtsp_url[0], &click_cb, 0);
}

static void device_sub_rtsp_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, rtsp_url_click_func);
	cctv_manage_rtsp_url_create_template(lv_scr_act(), 5, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SUB_STREAM), onvif_discover_info[CURR_INDEX].rtsp_url[1], &click_cb, 0);
}


static void wifi_input_keypad_event(lv_obj_t *kb, lv_state_t event)
{
	if (event != LV_EVENT_CLICKED)
	{
		return;
	}
	printf("event %d\n", event);

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

	else if (strcmp(txt, "1#") == 0)
	{
		lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_SPECIAL);
		static rom_bin_info info11 = rom_bin_info_get(ROM_UI_KD_DELETE_PNG);
		lv_keyboard_img_set(kb, 10, &info11);

		lv_keyboard_img_set(kb, 11, NULL);

		lv_keyboard_img_set(kb, 22, NULL);
		return;
	}
	else if (strcmp(txt, "abc") == 0) // 和键盘的id对比，选择是否隐藏32～36的text
	{
		lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_LOWER); // 选择小写

		lv_keyboard_img_set(kb, 10, NULL);
		static rom_bin_info info11 = rom_bin_info_get(ROM_UI_KD_DELETE_PNG);
		lv_keyboard_img_set(kb, 11, &info11);
		static rom_bin_info info22 = rom_bin_info_get(ROM_UI_KD_ADD_CONFIRM_PNG);
		lv_keyboard_img_set(kb, 22, &info22);
		return;
	}
	else if (strcmp(txt, "ABC") == 0)
	{
		lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_UPPER); // 选择小写

		lv_keyboard_img_set(kb, 10, NULL);
		static rom_bin_info info11 = rom_bin_info_get(ROM_UI_KD_DELETE_PNG);
		lv_keyboard_img_set(kb, 11, &info11);
		static rom_bin_info info22 = rom_bin_info_get(ROM_UI_KD_ADD_CONFIRM_PNG);
		lv_keyboard_img_set(kb, 22, &info22);
		return;
	}

	if (ext->ta == NULL)
		return;

	if (ext->btnm.pattern_p[btn_id])
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_input_txt);

		if (ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_003_PNG)
		{ /*LV_SYMBOL_CLOSE*/

			lv_textarea_set_text(ext->ta, ""); /*De-assign the text area to hide it cursor if needed*/
			return;
		}
		else if (ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_004_PNG)
		{ /*LV_SYMBOL_OK*/

			// int pos = lv_textarea_get_cursor_pos(obj);
			if (input_mode == INPUT_MODE_ACCOUNT)
			{
				/* 更新账户 */
				memset(onvif_discover_info[CURR_INDEX].user, 0, sizeof(onvif_discover_info[CURR_INDEX].user));
				strncpy(onvif_discover_info[CURR_INDEX].user, lv_textarea_get_text(obj), sizeof(onvif_discover_info[CURR_INDEX].user));
			}
			else if (input_mode == INPUT_MODE_PASSWORD)
			{
				/* 更新密码 */
				memset(onvif_discover_info[CURR_INDEX].pswd, 0, sizeof(onvif_discover_info[CURR_INDEX].pswd));
				strncpy(onvif_discover_info[CURR_INDEX].pswd, lv_textarea_get_text(obj), sizeof(onvif_discover_info[CURR_INDEX].pswd));
			}
			input_mode = INPUT_MODE_NULL;
			goto_layout(pLAYOUT(onvif_input));
			return;
		}

		else if (ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KD_ADD_CONFIRM_PNG) /* LV_SYMBOL_NEW_LINE*/
		{
			return;
		}
		else if (ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_001_PNG) /*LV_SYMBOL_LEFT*/
		{
			int pos = lv_textarea_get_cursor_pos(obj);

			if (pos > 0)
				lv_textarea_set_cursor_pos(obj, pos - 1);

			return;
		}
		else if (ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_002_PNG) /*LV_SYMBOL_RIGHT*/
		{
			int pos = lv_textarea_get_cursor_pos(obj);

			if (pos < strlen(lv_textarea_get_text(obj)))
				lv_textarea_set_cursor_pos(obj, pos + 1);

			return;
		}
		else if (ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KD_DELETE_PNG || ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KD_DELETE_PNG) /*LV_SYMBOL_BACKSPACE*/
		{
			int pos = lv_textarea_get_cursor_pos(obj);
			if (pos == 0)
			{
				return;
			}
			lv_textarea_del_char(obj);
			if (pos == 1)
			{
				// layout_wif_input_textarea_display();
			}
			return;
		}
		else if (ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_002_PNG) /*LV_SYMBOL_SPACE*/
		{
			lv_textarea_add_char(ext->ta, ' ');
		}
	}

	lv_textarea_add_text(ext->ta, txt);
	// layout_wif_input_textarea_display();
}
static bool onvif_input_textarea_create(void)
{
	lv_obj_t *textarea = lv_textarea_create(lv_scr_act(), NULL);
	if (textarea == NULL)
	{
		printf("create textarea fail \n");
		return false;
	}
	lv_obj_set_id(textarea, wifi_input_scr_act_obj_id_input_txt);

	lv_obj_set_pos(textarea, 48, 70);
	lv_obj_set_size(textarea, 937, 40);
	lv_textarea_set_text(textarea, "");
	lv_textarea_set_text_align(textarea, LV_LABEL_ALIGN_LEFT);

	lv_obj_set_style_local_bg_opa(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);

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
	lv_textarea_set_max_length(textarea, 63);
	if (input_mode == INPUT_MODE_ACCOUNT)
	{
		lv_textarea_set_placeholder_text(textarea, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_INPUT_ACCOUNT));
	}
	else if (input_mode == INPUT_MODE_PASSWORD)
	{
		lv_textarea_set_placeholder_text(textarea, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_INPUT_PASSWORD));
	}

	// layout_wif_input_textarea_display();
	return true;
}
static bool onvif_input_keypad_create(void)
{
	lv_obj_t *keyboard = lv_keyboard_create(lv_scr_act(), NULL);
	if (keyboard == NULL)
	{
		printf("create keyboard fail \n");
		return false;
	}
	lv_obj_set_id(keyboard, wifi_input_scr_act_obj_id_keyboard);
	lv_obj_set_pos(keyboard, 0, 200);
	lv_obj_set_size(keyboard, 1024, 400);

	/*
	 *   设置键盘的颜色，属性等
	 */
	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x181818));
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, lv_color_hex(0x292929));
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED, lv_color_hex(0x194861));
	lv_obj_set_style_local_value_font(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED, FONT_SIZE(150));
	/*
	 *   设置键盘的按键矩阵边距，
	 */
	lv_obj_set_style_local_pad_inner(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 7);
	lv_obj_set_style_local_pad_top(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 7);
	lv_obj_set_style_local_pad_left(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 7);
	lv_obj_set_style_local_pad_right(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 7);
	lv_obj_set_style_local_pad_bottom(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 5);
	lv_obj_set_style_local_text_font(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, FONT_SIZE(31));

	/*
	 *   设置键盘功能性键盘图标
	 */
	static rom_bin_info keyboard_del_img1 = rom_bin_info_get(ROM_UI_KEYPAD_003_PNG);
	lv_keyboard_img_set(keyboard, 35, &keyboard_del_img1);
	static rom_bin_info keyboard_del_img2 = rom_bin_info_get(ROM_UI_KEYPAD_001_PNG);
	lv_keyboard_img_set(keyboard, 36, &keyboard_del_img2);
	static rom_bin_info keyboard_space_img3 = rom_bin_info_get(ROM_UI_28_KEYPAD_SPACE_PNG);
	lv_keyboard_img_set(keyboard, 37, &keyboard_space_img3);
	static rom_bin_info keyboard_del_img4 = rom_bin_info_get(ROM_UI_KEYPAD_002_PNG);
	lv_keyboard_img_set(keyboard, 38, &keyboard_del_img4);
	static rom_bin_info keyboard_del_img5 = rom_bin_info_get(ROM_UI_KEYPAD_004_PNG);
	lv_keyboard_img_set(keyboard, 39, &keyboard_del_img5);
	static rom_bin_info keyboard_del_img6 = rom_bin_info_get(ROM_UI_KD_DELETE_PNG);
	lv_keyboard_img_set(keyboard, 11, &keyboard_del_img6);
	static rom_bin_info keyboard_del_img7 = rom_bin_info_get(ROM_UI_KD_ADD_CONFIRM_PNG);
	lv_keyboard_img_set(keyboard, 22, &keyboard_del_img7);
	/*
	 *   将文本框与键盘绑定
	 */
	lv_obj_t *textarea = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_input_txt);
	lv_keyboard_set_textarea(keyboard, textarea);

	/*
	 *   输入键盘事件
	 */
	static obj_click_data key_btn = obj_click_data_anything_create(wifi_input_keypad_event);
	obj_click_event_listen(keyboard, &key_btn);
	return true;
}



static bool onvif_get_rtsp_url(void)
{

	/* 设置账户&密码 */
	sat_ipcamera_user_password_set(CURR_INDEX, onvif_discover_info[CURR_INDEX].user, onvif_discover_info[CURR_INDEX].pswd);

	memset(onvif_discover_info[CURR_INDEX].rtsp_url[0], 0, sizeof(onvif_discover_info[CURR_INDEX].rtsp_url[0]));
	memset(onvif_discover_info[CURR_INDEX].rtsp_url[1], 0, sizeof(onvif_discover_info[CURR_INDEX].rtsp_url[1]));

	unsigned long long time_x = user_timestamp_get();
	int oneMore = 1;
ONVIF_GET_ONE_MORE:

	time_x = user_timestamp_get();
	while (sat_ipcamera_status_get() == true && (user_timestamp_get() - time_x < 2000))
	{
		LOG_WHITE("onvif is busy...\n");
		usleep(1000 * 100);
	}

	/* 获取rtsp */
	if ((sat_ipcamera_rtsp_url_get(CURR_INDEX) == true) && oneMore)
	{
		oneMore--;
		LOG_WHITE("one more=%d\n", oneMore);

		time_x = user_timestamp_get();
		while (sat_ipcamera_status_get() == true && (user_timestamp_get() - time_x < 2000))
		{
			LOG_WHITE("onvif is busy...\n");
			usleep(1000 * 100);
		}

		int token_num = sat_ipcamera_profile_token_num_get(CURR_INDEX);
		printf("token num=%d\n", token_num);
		if (token_num == 0)
		{
			goto ONVIF_GET_ONE_MORE;
		}

		for (int i = 0; i < 2; i++)
		{
			const char *rtsp_data = sat_ipcamera_rtsp_addr_get(CURR_INDEX, i);
			printf("rtsp data[%d]='%s'\n", i, rtsp_data);

			if (rtsp_data[0] != 0)
			{
				char *pstr = strstr(rtsp_data, "//");
				if (*pstr)
				{
					pstr += 2;
					memset(onvif_discover_info[CURR_INDEX].rtsp_url[i], 0, sizeof(onvif_discover_info[CURR_INDEX].rtsp_url[i]));
					snprintf(onvif_discover_info[CURR_INDEX].rtsp_url[i], 128, "rtsp://%s:%s@%s", onvif_discover_info[CURR_INDEX].user, onvif_discover_info[CURR_INDEX].pswd, pstr);
				}
			}
			else
			{
				goto ONVIF_GET_ONE_MORE;
			}
		}
	}
	else
	{
		printf("rtsp url get fail...\n");

		return false;
	}

	return true;
}

static void onvif_get_rtsp_url_delay_task(lv_task_t *task)
{
	bool ret = onvif_get_rtsp_url();

	printf("===========================>>> onvif_get_rtsp_url:[%d] \n", ret);

	goto_layout(cur_layout_get());

}

static void cctv_manage_input_screen_click_func(lv_obj_t *obj)
{

	lv_layout_task_create(onvif_get_rtsp_url_delay_task, 200, LV_TASK_PRIO_MID, NULL);
}

static void refresh_button_func(lv_obj_t *obj)
{
	LOG_WHITE("click\n");

	cctv_manage_input_screen_click_func(NULL);
}

static void onvif_device_refresh_btn_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, refresh_button_func);
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, ONVIF_INPUT_REFRESH_ID);
	lv_obj_set_size(obj, 1024, 70);
	lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	obj_click_event_listen(obj, &click_cb);

	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_REGISTER_DEV));
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);

	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x4ED7A6));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DISABLED, lv_color_hex(THEAME_COLOR_DISABLE));
}

static void LAYOUT_ENTER_FUNC(onvif_input)
{
	setting_cancel_btn_create(cctv_manage_input_back_btn_up);
	setting_head_label_create(onvif_discover_info[CURR_INDEX].ip);

	//     layout_title_label_create(onvif_discover_info[CURR_INDEX].ip);
	// 	layout_back_btn_create(cctv_manage_input_back_btn_up);
	if (input_mode == INPUT_MODE_NULL)
	{
		user_name_create();
		user_pswd_create();
		device_main_rtsp_create();
		device_sub_rtsp_create();
		onvif_device_refresh_btn_create();
	}
	else
	{

		onvif_input_textarea_create();
		onvif_input_keypad_create();
	}

	// cctv_manage_kbd_create();
	//     obj_screen_click_event_register(cctv_manage_input_screen_click_func);
}

static void LAYOUT_QUIT_FUNC(onvif_input)
{
	if (cur_layout_get() != pLAYOUT(onvif_input))
	{
		input_mode = INPUT_MODE_NULL;
	}
	//     obj_screen_click_event_register(NULL);
}

CREATE_LAYOUT(onvif_input);