#include "layout_define.h"
#include "wifi_api.h"
typedef enum
{
	wifi_input_scr_act_obj_id_head_label,
	wifi_input_scr_act_obj_id_input_txt,
	wifi_input_scr_act_obj_id_pwd_btn,
	wifi_input_scr_act_obj_id_keyboard,
	wifi_input_scr_act_obj_id_msgdialog,
} wifi_input_scr_act_obj_id;


// #define CONNECTING 			0
// #define CONNECT_SUCCESS 	1
// #define CONNECT_FAIL 		2
// #define WIFINAME_EMPTY		3
// #define WIFIPWD_SHOT 		4
// #define NOT_FIND 			5

static unsigned char wifi_username[64] = {0};
static unsigned char wifi_password[64] = {0};

bool wifi_free_flag = false;

// static  char addwifi_name[24] = {0};
// static  char addwifi_pwd[24] = {0};
static bool layout_wif_input_textarea_display(void);
/***
**   日期:2022-06-06 15:34:38
**   作者: leo.liu
**   函数作用：外部输入用户名接口
**   参数说明:
***/
void layout_wifi_input_set_user_name(const unsigned char *name)
{
	memset((char *)wifi_username, 0, sizeof(wifi_username));
	strcpy((char *)wifi_username, (char *)name);
}
/***
**   日期:2022-06-06 15:34:54
**   作者: leo.liu
**   函数作用：设置模式
**   参数说明:0:输入用户名，1:输入密码
***/
static char wifi_input_mode = 0;
void layout_wifi_input_mode_set(char mode)
{
	wifi_input_mode = mode;
}

static void layout_wifi_input_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(wifi_display));
}





static void wifi_input_msg_dialog_error_confirm_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(wifi_input_scr_act_obj_id_msgdialog);
}

static void wifi_input_animation_connecting_task(lv_task_t *task)
{
	bool conneted = 0;
	wifi_device_connection_stauts(NULL, NULL, NULL, &conneted, NULL);
	if (conneted == true)
	{
		printf("============+++++++++tuya___wifi_seyc\n");
		wifi_device_tmp_sync();
		/*****  tuya重连 *****/
		tuya_api_reconnect_network();
		goto_layout(pLAYOUT(wifi_display));
	}
	else
	{
		int *ploop = (int *)task->user_data;
		int loop = *ploop;
		if (loop > 30)
		{
			wifi_device_conneting();

			lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_msgdialog);
			lv_obj_clean(parent);
			lv_task_del(task);

			setting_msgdialog_msg_confirm_btn_create(parent, wifi_input_msg_dialog_error_confirm_up);
			setting_msgdialog_msg_create(parent, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_NTWORK_ERROR));
			standby_timer_restart(true);
		}
		else
		{
			(*ploop)++;
		}
	}
}



/***
**   日期:2022-06-07 08:19:56
**   作者: leo.liu
**   函数作用：创建正在连接的动画
**   参数说明:
***/
void wifi_input_animation_task_create(lv_obj_t *parent)
{
	setting_msgdialog_msg_create(parent, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_WIFI_CONNECTING));

	lv_obj_t *preload = lv_spinner_create(parent, NULL);
	lv_obj_set_size(preload, 80, 80);
	lv_obj_align(preload, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

	lv_obj_set_style_local_line_color(preload, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, lv_color_make(0x00, 0x96, 0xFF));
	lv_obj_set_style_local_line_color(preload, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
	lv_obj_set_style_local_line_width(preload, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 8);
	lv_obj_set_style_local_line_width(preload, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, 8);
	static int loop_cont = 0;
	loop_cont = 0;
	standby_timer_close();
	lv_layout_task_create(wifi_input_animation_connecting_task, 1000, LV_TASK_PRIO_MID, &loop_cont);

	wifi_device_user_to_temp(wifi_username, wifi_password[0] == '\0' ? NULL : wifi_password);
	wifi_device_temp_conneting();
}
/***
**   日期:2022-06-07 08:14:52
**   作者: leo.liu
**   函数作用：创建连接的消息框
**   参数说明:
***/
void wifi_input_msg_dialog_display(void)
{
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_msgdialog);
	if (cont != NULL)
	{
		return;
	}

	if(wifi_free_flag ==true)
	{
		memset(wifi_password, 0, sizeof(wifi_password));
	}
	cont = setting_msgdialog_msg_bg_create(wifi_input_scr_act_obj_id_msgdialog);
	wifi_input_animation_task_create(cont);
}






static void wifi_input_keypad_event(lv_obj_t *kb, lv_state_t event)
{
	
	if (event != LV_EVENT_CLICKED )
	{
		return;
	}
	printf("event %d\n",event);

	// if (event != LV_EVENT_VALUE_CHANGED)
	// 	return;

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


	// if (btn_id == 12)
	// {
	// 	lv_keyboard_mode_t mode = lv_keyboard_get_mode(kb);
	// 	lv_keyboard_set_mode(kb, mode == LV_KEYBOARD_MODE_TEXT_UPPER ? LV_KEYBOARD_MODE_TEXT_LOWER : LV_KEYBOARD_MODE_TEXT_UPPER);   //判断大小写选择
	// 	return;
	// }
	else if (strcmp(txt, "1#") == 0)
	{
		lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_SPECIAL);  
		static rom_bin_info info11 = rom_bin_info_get(ROM_UI_KD_DELETE_PNG);
		lv_keyboard_img_set(kb,10,&info11);
		
		lv_keyboard_img_set(kb,11,NULL);
		
		lv_keyboard_img_set(kb,22,NULL);
		return;
	}
	else if (strcmp(txt, "abc") == 0)                                  //和键盘的id对比，选择是否隐藏32～36的text               
	{
		lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_LOWER);                         //选择小写

		lv_keyboard_img_set(kb,10,NULL);
		static rom_bin_info info11 = rom_bin_info_get(ROM_UI_KD_DELETE_PNG);
		lv_keyboard_img_set(kb,11,&info11);
		static rom_bin_info info22 = rom_bin_info_get(ROM_UI_KD_ADD_CONFIRM_PNG);
		lv_keyboard_img_set(kb,22,&info22);
		return;
	}
	else if (strcmp(txt, "ABC") == 0)                                             
	{
		lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_UPPER);                         //选择小写
		
		lv_keyboard_img_set(kb,10,NULL);
		static rom_bin_info info11 = rom_bin_info_get(ROM_UI_KD_DELETE_PNG);
		lv_keyboard_img_set(kb,11,&info11);
		static rom_bin_info info22 = rom_bin_info_get(ROM_UI_KD_ADD_CONFIRM_PNG);
		lv_keyboard_img_set(kb,22,&info22);
		return;
	}

	if(ext->ta == NULL) return;

	if(ext->btnm.pattern_p[btn_id])
	{ 
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_input_txt);

		if(ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_003_PNG)
		{ /*LV_SYMBOL_CLOSE*/
				
			lv_textarea_set_text(ext->ta, ""); /*De-assign the text area to hide it cursor if needed*/
			return;
		} 
		else if(ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_004_PNG) 
		{/*LV_SYMBOL_OK*/

			int pos = lv_textarea_get_cursor_pos(obj);
			
			if (wifi_input_mode == 0)
			{
				printf("wifi name add\n");
				if (pos == 0)
				{
					message_window_home_pops(layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_NAME_SHORT));
					return;
				}
				printf("==========================>>> lv_textarea_get_text : [%s]\n", lv_textarea_get_text(obj));
				layout_wifi_input_set_user_name((const unsigned char *)lv_textarea_get_text(obj));
				layout_wifi_input_mode_set(1);
				printf("wifi name add=====>>>>><<<<<<\n");
				goto_layout(pLAYOUT(wifi_input));
				printf("wifi name add====++++++++------\n");
				return ;
			}
			else if (wifi_input_mode == 1)
			{
				if ((pos < 8) && (pos != 0))
				{
					message_window_home_pops(layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_PWD_SHORT));
					return;
				}
				memset(wifi_password, 0, sizeof(wifi_password));
				if (pos != 0)
				{
					strcpy((char *)wifi_password, lv_textarea_get_text(obj));
				}
				printf("==========================>>> wifi_password : [%s]\n", wifi_password);
				wifi_input_msg_dialog_display();
			}
		}
		// printf("=========--------56465484485 \n");
		// child = lv_obj_get_child_form_id (kb->parent ,1);
		// if (child == kb) {
				
		// 	memset(addwifi_name,0,strlen(addwifi_name));
		// 	sprintf(addwifi_name,"%s",lv_textarea_get_text(ext->ta));
		
		// 	if(strlen(addwifi_name) < 1){
		// 		memset(addwifi_name,0,strlen(addwifi_name));
		// 		lv_label_set_text((lv_obj_t *)kb->parent->user_data,addwifi_name);
		// 		wifi_input_msg_dialog_display();
		// 		// lv_obj_t *msg = connect_wifi_cb();
		// 		// set_msg_text(msg,WIFINAME_EMPTY);
		// 		return ;
		// 	}

		// 	lv_label_set_text((lv_obj_t *)kb->parent->user_data,addwifi_name);

		// 	lv_obj_del(lv_obj_get_child(lv_scr_act(),NULL ));
		// 	lv_obj_del(kb->parent);
		// 	return;
		// }
			
		
	
			else if(ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KD_ADD_CONFIRM_PNG)/* LV_SYMBOL_NEW_LINE*/
			{
				return;
			}
			else if(ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_001_PNG)/*LV_SYMBOL_LEFT*/
			{
				int pos = lv_textarea_get_cursor_pos(obj);
				// printf("========================================>>> pos:[%d], [%d] \n", pos, strlen(lv_textarea_get_text(obj)));

				if(pos > 0) lv_textarea_set_cursor_pos(obj, pos - 1);

				return;
			}
			else if(ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_002_PNG)/*LV_SYMBOL_RIGHT*/
			{
				int pos = lv_textarea_get_cursor_pos(obj);

				if(pos < strlen(lv_textarea_get_text(obj))) lv_textarea_set_cursor_pos(obj, pos + 1);

				return;
			}
			else if(ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KD_DELETE_PNG || ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KD_DELETE_PNG)/*LV_SYMBOL_BACKSPACE*/
			{
				
				int pos = lv_textarea_get_cursor_pos(obj);
				if (pos == 0)
				{
					return;
				}
				lv_textarea_del_char(obj);
				if (pos == 1)
				{
					layout_wif_input_textarea_display();
				}
				return;
			}
			else if(ext->btnm.pattern_p[btn_id]->offset == ROM_UI_KEYPAD_002_PNG)/*LV_SYMBOL_SPACE*/
			{
				lv_textarea_add_char(ext->ta, ' ');
			}
		}
	// }

	lv_textarea_add_text(ext->ta, txt);
	layout_wif_input_textarea_display(); 
}
/***
**   日期:2022-06-06 17:00:29
**   作者: leo.liu
**   函数作用：创建键盘
**   参数说明:
***/
static bool wifi_input_keypad_create(void)
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
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x101010));
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, lv_color_hex(0x202020));
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
	lv_obj_set_style_local_text_font(keyboard,LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, FONT_SIZE(31));

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

/***
**   日期:2022-06-06 17:00:15
**   作者: leo.liu
**   函数作用：显示头部信息
**   参数说明:
***/
static void layout_wifi_input_head_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_head_label);
	if (wifi_input_mode == 0)
	{
		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_WIFI));
	}
	else
	{
		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, (char *)wifi_username);
	}
}

/***
**   日期:2022-06-06 16:05:43
**   作者: leo.liu
**   函数作用：头部文本显示
**   参数说明:
***/
static void layout_wifi_input_head_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	if (obj == NULL)
	{
		printf("create head label failed \n");
		return;
	}
	lv_obj_set_id(obj, wifi_input_scr_act_obj_id_head_label);
	lv_obj_set_pos(obj, 453, 23);
	lv_obj_set_size(obj, 118, 47);

	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	layout_wifi_input_head_display();
}

/***
**   日期:2022-06-06 16:19:22
**   作者: leo.liu
**   函数作用：输入框显示
**   参数说明:
***/
static bool layout_wif_input_textarea_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_input_txt);
	lv_obj_t *textarea = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_input_txt);
	if (textarea == NULL)
	{
		printf("get child form id fail \n");
		return false;
	}
	int pos = lv_textarea_get_cursor_pos(textarea);
	if ((pos == 0) && (strlen(lv_textarea_get_text(obj)) == 0))
	{
		if (wifi_input_mode == 0)
		{
			lv_obj_set_style_local_value_str(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_ENTER_ACCOUNT));
			//	lv_textarea_set_text(textarea, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_ENTER_ACCOUNT));
		}
		else
		{
			lv_obj_set_style_local_value_str(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, layout_setting_password_string_get(SETTING_PASSWORD_LANG_ID_ENTER_PWD));
			// lv_textarea_set_text(textarea, layout_setting_password_string_get(SETTING_PASSWORD_LANG_ID_ENTER_PWD));
		}
	}
	else if (pos == 1)
	{
		lv_obj_set_style_local_value_str(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, NULL);
	}

	return true;
}
/***
**   日期:2022-06-06 16:15:22
**   作者: leo.liu
**   函数作用：创建输入文本
**   参数说明:
***/
static bool layout_wifi_input_txt_create(void)
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
	layout_wif_input_textarea_display();
	return true;
}
static void layout_wifi_input_pwd_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_pwd_btn);
	lv_obj_set_hidden(obj, wifi_input_mode == 0 ? true : false);

	lv_obj_t *text_obj = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_input_txt);
	if (wifi_input_mode)
	{
		if (lv_textarea_get_pwd_mode(text_obj) == true)
		{
			static rom_bin_info img = rom_bin_info_get(ROM_UI_3_LIST_HIDE_PNG);
			lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
		}
		else
		{
			static rom_bin_info img = rom_bin_info_get(ROM_UI_5_LIST_SHOW_PNG);
			lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
		}
	}
}

static void layout_wifi_input_pwd_btn_up(lv_obj_t *obj)
{
	obj = lv_obj_get_child_form_id(lv_scr_act(), wifi_input_scr_act_obj_id_input_txt);
	bool pwd_mode = lv_textarea_get_pwd_mode(obj);
	lv_textarea_set_pwd_mode(obj, pwd_mode == true ? false : true);
	layout_wifi_input_pwd_btn_display();
}
/***
**   日期:2022-06-06 16:38:22
**   作者: leo.liu
**   函数作用：创建密码显示/隐藏按钮
**   参数说明:
***/
static void layout_wifi_input_pwd_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, wifi_input_scr_act_obj_id_pwd_btn);
	lv_obj_set_pos(obj, 937, 71);
	lv_obj_set_size(obj, 38, 38);
	static obj_click_data click_data = obj_click_data_up_create(layout_wifi_input_pwd_btn_up);
	obj_click_event_listen(obj, &click_data);
	layout_wifi_input_pwd_btn_display();
}


static void LAYOUT_ENTER_FUNC(wifi_input)
{
	setting_cancel_btn_create(layout_wifi_input_cancel_btn_up);
	
	layout_wifi_input_head_create();
	
	layout_wifi_input_txt_create();

	layout_wifi_input_pwd_btn_create();
	
	wifi_input_keypad_create();
	
	
}
static void LAYOUT_QUIT_FUNC(wifi_input)
{
	standby_timer_restart(true);
}

CREATE_LAYOUT(wifi_input);