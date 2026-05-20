#include "layout_define.h"

typedef enum
{
	INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_1 = 0x01,
	INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_2,
	INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_3,
	INTERCOM_CALL_BTN_ID,

	INTERCOM_CALL_MIC_SLIDER_CONT_ID,
	INTERCOM_CALL_VOLOME_SLIDER_CONT_ID,
} intercom_scr_actl_obj_id;
/*****  0xcba a:111 呼叫的id，b:1111 应答的ID, c:1111通话的id*****/
static unsigned int layout_intercom_call_flg = 0x00;
unsigned int layout_intercom_call_mask_get(void)
{
	return layout_intercom_call_flg; 
}
void layout_intercom_call_mask_set(unsigned int mask)
{
	layout_intercom_call_flg = mask;
}

/***                               
**   日期:2022-05-30 09:10:56
**   作者: leo.liu
**   函数作用：返回按钮创建
**   参数说明:
***/
static void intercom_call_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}
/***
**   日期:2022-05-30 08:59:30
**   作者: leo.liu
**   函数作用：返回按钮创建
**   参数说明:
***/
static void intercom_call_cancel_btn_create(void)
{
	lv_obj_t *obj = lv_btn_create(lv_scr_act(), NULL);
	if (obj == NULL)
	{
		printf("create cancel btn failed \n");
		return;
	}
	lv_obj_set_pos(obj, 36, 18);
	lv_obj_set_size(obj, 58, 58);

	static rom_bin_info image = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image);
	lv_obj_set_style_local_pattern_align(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);

	static obj_click_data click_data = obj_click_data_up_create(intercom_call_cancel_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
**   日期:2022-05-30 08:59:44
**   作者: leo.liu
**   函数作用：头文本创建
**   参数说明:
***/
static void intercom_call_head_label_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	if (obj == NULL)
	{
		printf("create head label failed \n");
		return;
	}

	// lv_obj_set_pos(obj, 582, 23);
	lv_obj_set_size(obj, 118, LV_VER_RES * 0.12);
	lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_home_string_get(HOME_LNAG_ID_INTER));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
}

static void call_room_btn_up(lv_obj_t *obj)
{
	lv_obj_t *child = lv_obj_get_child(obj, NULL);
	lv_obj_t *call_btn = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_CALL_BTN_ID);
	lv_obj_t *room1 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_1);
	lv_obj_t *room2 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_2);
	lv_obj_t *room3 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_3);

	if (child == NULL)
	{
		return;
	}

	if (lv_obj_get_state(child, LV_OBJ_PART_MAIN) != LV_STATE_CHECKED)
	{
		lv_obj_set_state(obj, LV_STATE_CHECKED);
		lv_obj_set_state(child, LV_STATE_CHECKED);
		lv_obj_set_state(call_btn, LV_STATE_CHECKED);
	}
	else
	{
		lv_obj_set_state(obj, LV_STATE_DEFAULT);
		lv_obj_set_state(child, LV_STATE_DEFAULT);
	}

	if (lv_obj_get_state(room1, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT &&
		lv_obj_get_state(room2, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT &&
		lv_obj_get_state(room3, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT)
	{
		
		lv_obj_set_state(call_btn, LV_STATE_DEFAULT);
	}
}

static void intercom_call_room_icon(int x, const char *text, intercom_scr_actl_obj_id id)
{
	/* 底色 */
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, id);
	lv_obj_set_size(cont, 209, 208);
	lv_obj_set_pos(cont, x, 185);
	lv_obj_set_style_local_radius(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 26);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1E1E1E));
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x404040));
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, lv_color_hex(0x3AC392));
	static obj_click_data click = obj_click_data_up_create(call_room_btn_up);
	obj_click_event_listen(cont, &click);

	/* 中间 */
	static rom_bin_info image = rom_bin_info_get(ROM_UI_INTERCOM_NEW_PNG);
	lv_obj_t *obj = lv_obj_create(cont, NULL);
	lv_obj_set_click(obj, false);
	lv_obj_set_size(obj, 94, 84);
	lv_obj_align(obj, cont, LV_ALIGN_CENTER, 0, -16);
	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 6);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xC3A46A));
	lv_obj_set_style_local_pattern_image(obj, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, &image);

	lv_obj_set_style_local_value_str(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, text);
	lv_obj_set_style_local_value_font(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_align(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_value_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
	// lv_obj_set_style_local_value_color(cont, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0X8D8D8D));
	lv_obj_set_style_local_value_ofs_y(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 76);
}

/***
**   日期:2022-05-30 09:00:01
**   作者: leo.liu
**   函数作用：呼叫id创建
**   参数说明:
***/
static void intercom_call_call_id_btn_create(void)
{
	intercom_call_room_icon(163, layout_setting_etc_string_get(user_data_get()->etc.deive_id == 0 ? SETTING_ETC_LANG_ID_DEVICE_ID2 : SETTING_ETC_LANG_ID_DEVICE_ID1), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_1);
	intercom_call_room_icon(407, layout_setting_etc_string_get(user_data_get()->etc.deive_id < 2 ? SETTING_ETC_LANG_ID_DEVICE_ID3 : SETTING_ETC_LANG_ID_DEVICE_ID2), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_2);
	intercom_call_room_icon(651, layout_setting_etc_string_get(user_data_get()->etc.deive_id < 3 ? SETTING_ETC_LANG_ID_DEVICE_ID4 : SETTING_ETC_LANG_ID_DEVICE_ID3), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_3);
}

/***
**   日期:2022-06-18 11:25:25
**   作者: leo.liu
**   函数作用：呼叫等待延时
**   参数说明:
***/
static void  layout_intercom_call_timeout_task(lv_task_t *task)
{
	lv_obj_t *obj = (lv_obj_t *)task->user_data;
	lv_obj_set_click(obj, true);
	if (cur_layout_get() == pLAYOUT(intercom))
	{
		message_window_pops(layout_intercom_string_get(LAYOUT_INTERCOM_LANGUAGE_ID_TISHI),0,120);
	}
	
	lv_task_del(task);
}

static void intercom_call_call_btn_up(lv_obj_t *obj)
{
	
	unsigned int flag = 0;
	lv_obj_t *room1 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_1);
	lv_obj_t *room2 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_2);
	lv_obj_t *room3 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_3);

	if (lv_obj_get_state(room1, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT &&
		lv_obj_get_state(room2, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT &&
		lv_obj_get_state(room3, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT)
	{
		message_window_pops(layout_intercom_string_get(LAYOUT_INTERCOM_LANGUAGE_ID_PROMPT),0,120);
	}
	lv_obj_t *room_icon = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_1);
	if (lv_obj_get_state(room_icon, LV_OBJ_PART_MAIN) == LV_STATE_CHECKED)
	{
		flag |= user_data_get()->etc.deive_id == 0 ? 0x02 : 0x01;
	}
 
	room_icon = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_2);

	if (lv_obj_get_state(room_icon, LV_OBJ_PART_MAIN) == LV_STATE_CHECKED)
	{
		flag |= user_data_get()->etc.deive_id < 2 ? 0x04 : 0x02;
	}

	room_icon = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_3);

	if (lv_obj_get_state(room_icon, LV_OBJ_PART_MAIN) == LV_STATE_CHECKED)
	{
		flag |= user_data_get()->etc.deive_id < 3 ? 0x08 : 0x04;
	}

	if (flag)
	{
		/*****  发送呼叫 *****/
		layout_intercom_call_flg = flag;
		intercom_cmd_send(layout_intercom_call_flg, CMD_INTERPHONE_CALL);
		lv_obj_set_click(obj, false);
		lv_layout_task_create(layout_intercom_call_timeout_task, 1000, LV_TASK_PRIO_MID, obj);
	}
}

/***
**   日期:2022-05-30 09:39:12
**   作者: leo.liu
**   函数作用：呼叫按钮/图标
**   参数说明:
***/
static void intercom_call_call_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, INTERCOM_CALL_BTN_ID);
	lv_obj_set_size(obj, 132, 132);
	lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -16);

	lv_obj_set_style_local_radius(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_obj_get_width(obj) / 2);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x414141));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, lv_color_hex(0x414141));

	static rom_bin_info img = rom_bin_info_get(ROM_UI_3_CALL_CALL_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(intercom_call_call_btn_up);
	obj_click_event_listen(obj, &click_data);
}


/***
**   日期:2022-05-30 16:59:15
**   作者: leo.liu
**   函数作用：显示本地id
**   参数说明:
***/
static void intercom_local_id_label_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	// lv_obj_set_pos(label, 489, 104);
	lv_obj_set_size(label, 301, 43);
	lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 80);

	int loc_dev = user_data_get()->etc.deive_id;
	int id = loc_dev == 0 ? SETTING_ETC_LANG_ID_DEVICE_ID1 : loc_dev == 1 ? SETTING_ETC_LANG_ID_DEVICE_ID2
														 : loc_dev == 2	  ? SETTING_ETC_LANG_ID_DEVICE_ID3
																		  : SETTING_ETC_LANG_ID_DEVICE_ID4;
	lv_label_set_text(label, layout_setting_etc_string_get(id));
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));

	/***** 画一条线 *****/
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 0, 80);
	lv_obj_set_size(obj, LV_HOR_RES_MAX, 1);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
	lv_obj_set_click(obj, false);
}
static void LAYOUT_ENTER_FUNC(intercom)
{
	layout_intercom_call_mask_set(0x00);
	intercom_call_cancel_btn_create(); /* 返回 */
	intercom_call_head_label_create(); /* 标题 */

	intercom_local_id_label_create();	/* 本地ID */
	intercom_call_call_id_btn_create(); /* 可拨打的ID */

	intercom_call_call_btn_create();   /* 呼叫按钮 */
	// intercom_call_volome_btn_create(); /* 声音/铃声调节按钮 */
	// intercom_call_mic_btn_create();	   /* 麦克风调节按钮 */
}

static void LAYOUT_QUIT_FUNC(intercom)
{
	const layout *p = cur_layout_get();
	if ((intercom_mastar_id_get() == OwnID) && (p != pLAYOUT(intercom_out)) && (p != pLAYOUT(monitor)))
	{
		usleep(200 * 1000);
		LOG_GREEN("set release 4\n");
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
	}
	if(p == pLAYOUT(monitor))
	{
		layout_intercom_call_mask_set(0x00);
	}
	
	// const layout *p = cur_layout_get();
	// if ((intercom_mastar_id_get() == OwnID) && (p != pLAYOUT(intercom_out)) && (p != pLAYOUT(monitor)))
	// {
	// 	usleep(200 * 1000);
	// 	intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
	// }
}
CREATE_LAYOUT(intercom);
