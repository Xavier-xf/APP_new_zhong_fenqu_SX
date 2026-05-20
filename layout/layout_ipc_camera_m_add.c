// /*******************************************************************
//  * @Descripttion   :
//  * @version        : 1.0.0
//  * @Author         : wxj
//  * @Date           : 2023-11-15 10:03
//  * @LastEditTime   : 2023-11-16 20:11
// *******************************************************************/
// #if 1

#include "layout_define.h"
// #include "user_data.h"

enum
{
	IPC_M_ADD_CCTV_SETTING_CONT,
	IPC_M_ADD_OBJ_BRAND_MAIN_OBJ,
	IPC_M_ADD_OBJ_BRAND_SUB_OBJ,
	IPC_M_ADD_CCTV_SETTING_IP_TEXTAREA,
	IPC_M_ADD_CCTV_SETTING_ACCOUNT_TEXTAREA,
	IPC_M_ADD_CCTV_SETTING_PASSWORD_TEXTAREA,
	IPC_M_ADD_CCTV_SETTING_RTSP_TEXTAREA,
	IPC_M_ADD_KEYBOARD_OBJ_ID,
	LAYOUT_SETTING_RECORD_OBJ_MSG_ID,
	LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID,
	wifi_input_scr_act_obj_id_input_txt,
	wifi_input_scr_act_obj_id_keyboard
};

enum
{
	INPUT_MODE_NULL,
	INPUT_MODE_IP,
	INPUT_MODE_ACCOUNT,
	INPUT_MODE_PASSWORD,
	INPUT_MODE_RTSP,
};

static int cctv_setting_channel = 0; /* [0]- CCTV1  [1]-CCTV2 */
static int input_mode = INPUT_MODE_NULL;
void cctv_setting_channel_set(int ch)
{
	cctv_setting_channel = ch;
}

int cctv_setting_channel_get(void)
{
	return cctv_setting_channel;
}


extern lv_obj_t *cctv_manage_rtsp_url_create_template(lv_obj_t *parent, int num, const char *main_str, const char *sub_str, obj_click_data *click_cb, int obj_id);
extern void onvif_preview_url_set(const char *url);
extern void onvif_preview_enter_flag_set(int flag);
extern void onvif_preview_ip_set(const char *ip);
static int ADD_CURR_INDEX = -1;
void onvif_add_manage_curr_index_set(int num)
{
	ADD_CURR_INDEX = num;
}

int onvif_add_manage_curr_index_get(void)
{
	return ADD_CURR_INDEX;
}




typedef enum
{
	PAN,		/* 松下 */
	DAHUA,	   /* 大华 */
	HIKVISION, /* 海康 */
	XIONGMAI,  /* 雄迈 */
	
} MY_IPC_ENUM;

static void ipc_rtsp_url_get(MY_IPC_ENUM type, char *url_main, char *url_sub, char *user, char *pswd, char *addr)
{
	LOG_WHITE("type=%d\n", type);
	switch (type)
	{
	case DAHUA:
		snprintf(url_main, 256, "rtsp://%s:%s@%s:554/cam/realmonitor?channel=1&subtype=0", user, pswd, addr);
		snprintf(url_sub, 256, "rtsp://%s:%s@%s:554/cam/realmonitor?channel=1&subtype=1", user, pswd, addr);
		break;
	case HIKVISION:
		snprintf(url_main, 256, "rtsp://%s:%s@%s:554/Streaming/Channels/1", user, pswd, addr);
		snprintf(url_sub, 256, "rtsp://%s:%s@%s:554/Streaming/Channels/2", user, pswd, addr);
		break;
	case XIONGMAI:
		snprintf(url_main, 256, "rtsp://%s:554/user=%s&password=%s&channel=1&stream=0.sdp?real_stream", addr, user, pswd);
		snprintf(url_sub, 256, "rtsp://%s:554/user=%s&password=%s&channel=1&stream=1.sdp?real_stream", addr, user, pswd);
		break;
	case PAN:

		
		snprintf(url_main, 256, "rtsp://%s:%s@%s:554/avstream/channel=0/stream=0.sdp", user, pswd,addr);
		snprintf(url_sub, 256, "rtsp://%s:%s@%s:554/avstream/channel=1/stream=1.sdp", user, pswd,addr);
		break;

	default:
		sprintf(user_data_get()->onvif_dev[cctv_setting_channel_get()].url[0], " ");
		sprintf(user_data_get()->onvif_dev[cctv_setting_channel_get()].url[1], " ");

		break;
	}
}


static void cctv_set_back_btn_func(lv_obj_t *obj)
{
	LOG_WHITE("click \n");
	// cctv_setting_screen_click(NULL);
	if (input_mode == INPUT_MODE_NULL)
	{
		goto_layout(pLAYOUT(onvif_info));
	}
	else
	{
		input_mode = INPUT_MODE_NULL;
		goto_layout(pLAYOUT(ipc_camera_m_add));
	}
}


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

	for (int i = 0; i < 5; i++)
	{
		if (lv_btnmatrix_get_btn_ctrl(btnmatrix, i, LV_BTNMATRIX_CTRL_CHECK_STATE) == true)
		{
			printf("ch=%d\n", cctv_setting_channel_get());
			user_data_get()->onvif_dev[cctv_setting_channel_get()].brand = i;
			user_data_save();
			break;
		}
	}

	goto_layout(pLAYOUT(ipc_camera_m_add));
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

static void cctv_setting_brand_click(lv_obj_t *obj)
{
	LOG_WHITE("click \n");

	static const char *string_map[] = {
		"",
		"\n",
		"",
		"\n",
		"",
		"\n",
		"",
		"\n",
		"",
		"",
	};

	string_map[0] = layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND_PAN);
	string_map[2] = layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND_DAHUA);
	string_map[4] = layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND_HIKVISION);
	string_map[6] = layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND_XM);
	string_map[8] = layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND_RTSP);


	lv_obj_t *cont = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(cont, setting_msgdialog_cancel_btn_up, setting_msgdialog_confirm_btn_up);

	lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(cont, LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID, string_map);

	lv_btnmatrix_set_btn_ctrl(btnmatrix, user_data_get()->onvif_dev[cctv_setting_channel_get()].brand, LV_BTNMATRIX_CTRL_CHECK_STATE);
}




static void cctv_setting_brand_create(int numb)
{
	LOG_WHITE("select=%d\n", user_data_get()->onvif_dev[cctv_setting_channel_get()].brand);

	static obj_click_data click_cb = obj_click_data_create(NULL, cctv_setting_brand_click);
	const char *str = (user_data_get()->onvif_dev[cctv_setting_channel_get()].brand == -1) ? "IPC" : layout_setting_etc_string_get(user_data_get()->onvif_dev[cctv_setting_channel_get()].brand + SETTING_ETC_LANG_ID_BRAND_PAN);
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_M_ADD_CCTV_SETTING_CONT), numb, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND), IPC_M_ADD_OBJ_BRAND_MAIN_OBJ, str, IPC_M_ADD_OBJ_BRAND_SUB_OBJ, true, SETTING_SUB_LABEL, &click_cb);
}

static void cctv_setting_ip_click(lv_obj_t *obj)
{
	input_mode = INPUT_MODE_IP;
	goto_layout(pLAYOUT(ipc_camera_m_add));
}

static void cctv_setting_ip_create(int numb)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, cctv_setting_ip_click);
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_M_ADD_CCTV_SETTING_CONT), numb, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_IP_ADDRESS), -1, onvif_add_discover_info[ADD_CURR_INDEX].ip, -1, false, SETTING_SUB_LABEL, &click_cb);

}

static void cctv_setting_account_click(lv_obj_t *obj)
{
	input_mode = INPUT_MODE_ACCOUNT;
	goto_layout(pLAYOUT(ipc_camera_m_add));
}

static void cctv_setting_account_create(int numb)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, cctv_setting_account_click);
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_M_ADD_CCTV_SETTING_CONT), numb, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_ACCOUNT), -1, onvif_add_discover_info[ADD_CURR_INDEX].user, -1, false, SETTING_SUB_LABEL, &click_cb);
}

static void cctv_setting_password_click(lv_obj_t *obj)
{
	input_mode = INPUT_MODE_PASSWORD;
	goto_layout(pLAYOUT(ipc_camera_m_add));
}

static void cctv_setting_password_create(int numb)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, cctv_setting_password_click);
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_M_ADD_CCTV_SETTING_CONT), numb, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_PASSWORD), -1, onvif_add_discover_info[ADD_CURR_INDEX].pswd, -1, false, SETTING_SUB_LABEL, &click_cb);
}






static void rtsp_url_click_func(lv_obj_t *obj)
{
	printf("click:'%s'\n", lv_label_get_text(lv_obj_get_child(obj,NULL)));

	onvif_preview_url_set(lv_label_get_text(lv_obj_get_child(obj,NULL)));
	onvif_preview_enter_flag_set(2);

	onvif_preview_ip_set(onvif_add_discover_info[onvif_add_manage_curr_index_get()].ip);
	goto_layout(pLAYOUT(onvif_preview));
}


static void device_main_rtsp_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, rtsp_url_click_func);
	cctv_manage_rtsp_url_create_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_M_ADD_CCTV_SETTING_CONT), 4, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_MAIN_STREAM), onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[0], &click_cb, 0);
}

static void device_sub_rtsp_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, rtsp_url_click_func);
	cctv_manage_rtsp_url_create_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_M_ADD_CCTV_SETTING_CONT), 6, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SUB_STREAM), onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[1], &click_cb, 0);
}






static void onvif_get_rtsp_url_delay_task(lv_task_t *task)
{
	ADD_CURR_INDEX = 0;
	memset(onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[0], 0, sizeof(onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[0]));
	memset(onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[1], 0, sizeof(onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[1]));

	ipc_rtsp_url_get(user_data_get()->onvif_dev[cctv_setting_channel_get()].brand,
							 onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[0],
							 onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[1],
							 onvif_add_discover_info[ADD_CURR_INDEX].user,
							 onvif_add_discover_info[ADD_CURR_INDEX].pswd,
							 onvif_add_discover_info[ADD_CURR_INDEX].ip);








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
	// lv_obj_set_id(obj, ONVIF_INPUT_REFRESH_ID);
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


static void IPC_M_CCTV_SETTING_CONT_create(void)
{
	// lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	// lv_obj_set_size(cont, 1024, 600);
	// lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
	// lv_obj_set_id(cont, IPC_M_ADD_CCTV_SETTING_CONT);
	// lv_obj_set_hidden(cont, false);

	lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);

	lv_obj_set_id(page, IPC_M_ADD_CCTV_SETTING_CONT);
	lv_obj_set_size(page, LV_HOR_RES_MAX - 50 * 2, 75 * 6);
	lv_obj_align(page, NULL, LV_ALIGN_IN_TOP_MID, 0, 75);

	lv_page_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_DRAG);
	lv_obj_set_style_local_pad_top(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_pad_left(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_pad_right(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_pad_bottom(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
}

static void rtsp_input_click_func(lv_obj_t *obj)
{
	LOG_WHITE("click\n");
	input_mode = INPUT_MODE_RTSP;
	goto_layout(pLAYOUT(ipc_camera_m_add));
}

static void rtsp_input_create(int numb)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, rtsp_input_click_func);
	extern lv_obj_t *cctv_manage_rtsp_url_create_template(lv_obj_t * parent, int num, const char *main_str, const char *sub_str, obj_click_data *click_cb, int obj_id);
	cctv_manage_rtsp_url_create_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_M_ADD_CCTV_SETTING_CONT), numb, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND_RTSP), onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[0], &click_cb, 0);

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

			ONVIF_ADD_INFO_STRUCT *info = &onvif_add_discover_info[ADD_CURR_INDEX];

			if (input_mode == INPUT_MODE_IP)
			{
				memset(info->ip, 0, sizeof(info->ip));
				strncpy(info->ip, lv_textarea_get_text(obj), sizeof(info->ip));
			}
			else if (input_mode == INPUT_MODE_ACCOUNT)
			{

				memset(info->user, 0, sizeof(info->user));
				strncpy(info->user, lv_textarea_get_text(obj), sizeof(info->user));
			}
			else if (input_mode == INPUT_MODE_PASSWORD)
			{
				memset(info->pswd, 0, sizeof(info->pswd));
				strncpy(info->pswd, lv_textarea_get_text(obj), sizeof(info->pswd));
			}
			else if (input_mode == INPUT_MODE_RTSP)
			{
				memset(info->rtsp_url, 0, sizeof(info->rtsp_url));
				snprintf(info->rtsp_url[0], 256, "%s", lv_textarea_get_text(obj));
				snprintf(info->rtsp_url[1], 256, "%s", lv_textarea_get_text(obj));
			}
			info->port = 80;

			/* 获取注码流和子码流 */
			// ipc_rtsp_url_get(user_data_get()->onvif_dev[cctv_setting_channel_get()].brand,
			// 				 user_data_get()->onvif_dev[cctv_setting_channel_get()].url[0],
			// 				 user_data_get()->onvif_dev[cctv_setting_channel_get()].url[1],
			// 				 user_data_get()->onvif_dev[cctv_setting_channel_get()].user,
			// 				 user_data_get()->onvif_dev[cctv_setting_channel_get()].pswd,
			// 				 user_data_get()->onvif_dev[cctv_setting_channel_get()].ip);
			// user_data_save();
			input_mode = INPUT_MODE_NULL;
			goto_layout(pLAYOUT(ipc_camera_m_add));
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
	lv_textarea_set_max_length(textarea, 128);
	if (input_mode == INPUT_MODE_IP)
	{

		lv_textarea_set_placeholder_text(textarea, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_INPUT_IP_ADDR));
	}
	else if (input_mode == INPUT_MODE_ACCOUNT)
	{
		lv_textarea_set_placeholder_text(textarea, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_INPUT_ACCOUNT));
	}
	else if (input_mode == INPUT_MODE_PASSWORD)
	{
		lv_textarea_set_placeholder_text(textarea, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_INPUT_PASSWORD));
	}
	else if (input_mode == INPUT_MODE_RTSP)
	{
		lv_textarea_set_placeholder_text(textarea, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_INPUT_RTSP));
	}

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



static void onvif_info_dev_bottom_btn_func(lv_obj_t *obj)
{
	onvif_preview_url_set(onvif_add_discover_info[ADD_CURR_INDEX].rtsp_url[0]);

	onvif_preview_enter_flag_set(2);

	goto_layout(pLAYOUT(onvif_preview));
}

static void onvif_info_dev_bottom_btn_create()
{
	static obj_click_data click_cb = obj_click_data_create(NULL, onvif_info_dev_bottom_btn_func);
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);

	lv_obj_set_size(obj, 1024, 70);
	lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	obj_click_event_listen(obj, &click_cb);

	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_REGISTER_DEV));
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x4ED7A6));
}



static void LAYOUT_ENTER_FUNC(ipc_camera_m_add)
{
	printf("enter \n");
	ADD_CURR_INDEX = 0;


	

	setting_cancel_btn_create(cctv_set_back_btn_func);
	setting_head_label_create(cctv_setting_channel_get() == 0 ? language_common_ch_string_get(MON_CH_CCTV3) : language_common_ch_string_get(MON_CH_CCTV4));


	if (input_mode == INPUT_MODE_NULL)
	{
		IPC_M_CCTV_SETTING_CONT_create();
		cctv_setting_brand_create(0);
		cctv_setting_ip_create(1);
		if (user_data_get()->onvif_dev[cctv_setting_channel_get()].brand != 4)
		{
			cctv_setting_account_create(2);
			cctv_setting_password_create(3);
			device_main_rtsp_create();
			device_sub_rtsp_create();
			onvif_device_refresh_btn_create();
		}
		else
		{
			rtsp_input_create(3);
			onvif_info_dev_bottom_btn_create();
		}
	}
	else
	{
		onvif_input_textarea_create();
		onvif_input_keypad_create();
	}
LOG_WHITE("enter \n");
	// cctv_setting_kbd_create();

	// obj_screen_click_event_register(cctv_setting_screen_click);
}

static void LAYOUT_QUIT_FUNC(ipc_camera_m_add)
{
	if (cur_layout_get() != pLAYOUT(ipc_camera_m_add))
	{
		// cctv_setting_channel_set(0);
		// input_mode = INPUT_MODE_NULL;
	}
}

CREATE_LAYOUT(ipc_camera_m_add);

