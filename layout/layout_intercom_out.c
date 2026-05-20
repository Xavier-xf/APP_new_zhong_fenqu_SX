#include "layout_define.h"

typedef enum
{
	intercom_out_sct_act_obj_id_head_cont,
	intercom_out_sct_act_obj_id_timeout_label,
	intercom_out_sct_act_obj_id_callid_label,
	intercom_out_sct_act_obj_id_vol_btn,
	intercom_out_sct_act_obj_id_handup_btn,
	intercom_out_sct_act_obj_id_vol_cont,
	intercom_out_sct_act_obj_id_vol_bar,

	INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_1 ,
	INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_2,
	INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_3,
	INTERCOM_CALL_BTN_ID,

	INTERCOM_CALL_MIC_SLIDER_CONT_ID,
	INTERCOM_CALL_VOLOME_SLIDER_CONT_ID,

} intercom_out_sct_act_obj_id;

// typedef enum
// {
// 	INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_1 = 0x01,
// 	INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_2,
// 	INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_3,
// 	INTERCOM_CALL_BTN_ID,

// 	INTERCOM_CALL_MIC_SLIDER_CONT_ID,
// 	INTERCOM_CALL_VOLOME_SLIDER_CONT_ID,
// } intercom_scr_actl_obj_id;


unsigned int layout_intercom_call_mask_get(void);





/***
**   日期:2022-05-30 15:05:27
**   作者: leo.liu
**   函数作用：创建顶部容器
**   参数说明:
***/
void intercom_head_cont_crate(int obj_id, const char *string)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, obj_id);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, LV_HOR_RES_MAX, 90);
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00));
	lv_obj_set_style_local_value_str(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, string);
	lv_obj_set_style_local_value_align(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
	lv_obj_set_style_local_value_ofs_x(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 20);
}

/***
**   日期:2022-05-30 16:41:18
**   作者: leo.liu
**   函数作用：超时任务
**   参数说明:
***/
static void intercom_timeout_task(lv_task_t *task_t)
{
	lv_obj_t *label = (lv_obj_t *)task_t->user_data;
	int timeout = *((int *)label->user_data);
	if (timeout == 0)
	{
		LOG_GREEN("intercom timeout display to home\n");
		goto_layout(pLAYOUT(home));
	}
	else
	{
		timeout--;
		*((int *)label->user_data) = timeout;
		lv_label_set_text_fmt(label, "%02d:%02d", timeout / 60, timeout % 60);
	}
}
/***
**   日期:2022-05-30 15:06:44
**   作者: leo.liu
**   函数作用：创建超时文本
**   参数说明:
***/
void intercom_timeout_label_create(int obj_id, int t)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_id(label, obj_id);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_size(label, 80, 43);
	lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 20);
	lv_label_set_align(label, LV_LABEL_ALIGN_RIGHT);
	static int timeout = 0;
	label->user_data = &timeout;
	timeout = t;
	lv_label_set_text_fmt(label, "%02d:%02d", timeout / 60, timeout % 60);
	lv_layout_task_create(intercom_timeout_task, 1000, LV_TASK_PRIO_MID, label);
}
/***
**   日期:2022-05-30 15:08:42
**   作者: leo.liu
**   函数作用：创建intercom图标
**   参数说明:
***/
void intercom_intercom_icon_create(bool talk)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 394, 165);
	lv_obj_set_size(obj, 260, 260);
	// lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 45);

	static rom_bin_info img_call = rom_bin_info_get(ROM_UI_1_WAIT_PNG);
	static rom_bin_info img_talk = rom_bin_info_get(ROM_UI_1_WAIT_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, talk == true ? &img_talk : &img_call);
	lv_obj_set_click(obj, false);
}
void intercom_call_id_label_display(int find_id)
{
	lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), find_id);
	if (label == NULL)
	{
		return;
	}

	unsigned int flag = layout_intercom_call_mask_get();
	char buffer[128] = {0};
	if (flag & 0x01)
	{
		sprintf(&buffer[strlen(buffer)], "%s %s\n", ((flag & 0x100) == 0x100) ? "#0096ff" : "#FFFFFF", layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DEVICE_ID1));
	}
	if (flag & 0x02)
	{
		sprintf(&buffer[strlen(buffer)], "%s %s\n", ((flag & 0x200) == 0x200) ? "#0096ff" : "#FFFFFF", layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DEVICE_ID2));
	}
	if (flag & 0x04)
	{
		sprintf(&buffer[strlen(buffer)], "%s %s\n", ((flag & 0x400) == 0x400) ? "#0096ff" : "#FFFFFF", layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DEVICE_ID3));
	}
	if (flag & 0x08)
	{
		sprintf(&buffer[strlen(buffer)], "%s %s\n", ((flag & 0x800) == 0x800) ? "#0096ff" : "#FFFFFF", layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DEVICE_ID4));
	}
	lv_label_set_text(label, buffer);
	// int h = lv_obj_get_height(label);
	// int y = 170 + (396 - h) / 2;
	// lv_obj_set_y(label, y);
}
/***
**   日期:2022-05-30 15:12:57
**   作者: leo.liu
**   函数作用：显示label
**   参数说明:
***/
void intercom_call_id_label_create(int obj_id)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_id(label, obj_id);
	lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
	lv_label_set_recolor(label, true);
	// lv_obj_set_pos(label, 469, 270);
	lv_obj_set_size(label, 186, 186);
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
	lv_obj_align(label, NULL, LV_ALIGN_CENTER, 10, 20);
	lv_obj_set_auto_realign(label, true);
	// lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
	
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
} 

/***
**   日期:2022-05-30 15:32:16
**   作者: leo.liu
**   函数作用：隐藏和显示按钮
**   参数说明:
***/
void intercom_obj_hidden(int vol_id, int talk_id, int hand_up, bool en, int cont_id, void (*vol_other_up)(lv_obj_t *obj))
{
	if (vol_id > 0)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), vol_id);
		lv_obj_set_hidden(obj, en);
	}
	if (talk_id > 0)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), talk_id);
		lv_obj_set_hidden(obj, en);
	}
	if (hand_up > 0)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), hand_up);
		lv_obj_set_hidden(obj, en);
	}
	if (cont_id > 0)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), cont_id);
		lv_obj_set_hidden(obj, !en);
		lv_obj_set_ext_click_area(obj, 0, 0, en == true ? 680 : 0, 0);
		static obj_click_data click_data = obj_click_data_up_create(NULL);
		click_data.up = vol_other_up;
		obj_click_event_listen(obj, &click_data);
	}
}


/***
**   日期:2022-05-30 15:19:48
**   作者: leo.liu
**   函数作用：挂断按钮创建
**   参数说明:
***/
void intercom_call_handup_and_talk_create(int x, int obj_id, const obj_click_data *click_data, bool talk)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, obj_id);
	lv_obj_set_pos(obj, x, 463);
	lv_obj_set_size(obj, 136, 136);
	// lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -32);
	static rom_bin_info img_handup = rom_bin_info_get(ROM_UI_1_CALL_ENDCALL_PNG);
	static rom_bin_info img_talk = rom_bin_info_get(ROM_UI_3_CALL_CALL_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, talk == true ? &img_talk : &img_handup);
	obj_click_event_listen(obj, click_data);
}





// static void call_room_btn_up(lv_obj_t *obj)
// {
// 	lv_obj_t *child = lv_obj_get_child(obj, NULL);
// 	lv_obj_t *call_btn = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_CALL_BTN_ID);
// 	lv_obj_t *room1 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_1);
// 	lv_obj_t *room2 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_2);
// 	lv_obj_t *room3 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_3);

// 	if (child == NULL)
// 	{
// 		return;
// 	}

// 	if (lv_obj_get_state(child, LV_OBJ_PART_MAIN) != LV_STATE_CHECKED)
// 	{
// 		lv_obj_set_state(obj, LV_STATE_CHECKED);
// 		lv_obj_set_state(child, LV_STATE_CHECKED);
// 		lv_obj_set_state(call_btn, LV_STATE_CHECKED);
// 	}
// 	else
// 	{
// 		lv_obj_set_state(obj, LV_STATE_DEFAULT);
// 		lv_obj_set_state(child, LV_STATE_DEFAULT);
// 	}

// 	if (lv_obj_get_state(room1, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT &&
// 		lv_obj_get_state(room2, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT &&
// 		lv_obj_get_state(room3, LV_OBJ_PART_MAIN) == LV_STATE_DEFAULT)
// 	{
		
// 		lv_obj_set_state(call_btn, LV_STATE_DEFAULT);
// 	}
// }

static void intercom_call_room_icon(int x, const char *text, intercom_out_sct_act_obj_id id)
{
	/* 底色 */
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, id);
	lv_obj_set_size(cont, 209, 208);
	lv_obj_set_pos(cont, x, 185);
	lv_obj_set_style_local_radius(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 26);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1E1E1E));
	// lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x404040));
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, lv_color_hex(0x3AC392));
	// static obj_click_data click = obj_click_data_up_create(call_room_btn_up);
	// obj_click_event_listen(cont, &click);

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
void intercom_call_call_id_btn_create(void)
{
	intercom_call_room_icon(163, layout_setting_etc_string_get(user_data_get()->etc.deive_id == 0 ? SETTING_ETC_LANG_ID_DEVICE_ID2 : SETTING_ETC_LANG_ID_DEVICE_ID1), INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_1);
	intercom_call_room_icon(407, layout_setting_etc_string_get(user_data_get()->etc.deive_id < 2 ? SETTING_ETC_LANG_ID_DEVICE_ID3 : SETTING_ETC_LANG_ID_DEVICE_ID2), INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_2);
	intercom_call_room_icon(651, layout_setting_etc_string_get(user_data_get()->etc.deive_id < 3 ? SETTING_ETC_LANG_ID_DEVICE_ID4 : SETTING_ETC_LANG_ID_DEVICE_ID3), INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_3);
}




// static void intercom_call_id_display(void)
// {
// 	int send_id;
// 	lv_obj_t *room1 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_1);
// 	lv_obj_t *room2 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_2);
// 	lv_obj_t *room3 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_OUT_ACTL_OBJ_ID_CALL_NUM_3);

// 	int room1_num = user_data_get()->etc.deive_id == 0 ? 2 : 1;
// 	int room2_num = user_data_get()->etc.deive_id < 2 ? 3 : 2;
// 	int room3_num = user_data_get()->etc.deive_id < 3 ? 4 : 3;

// 	unsigned int mask = layout_intercom_call_mask_get();

// 	printf("room1:%d, room2:%d, room3:%d  call_id:%02x  \n", room1_num, room2_num, room3_num, mask);

// 	if(mask == 0x11)
// 	{
// 		send_id = 1;
// 	}
// 	else if(mask == 0x22)
// 	{
// 		send_id = 2;
// 	}
// 	else if(mask == 0x44)
// 	{
// 		send_id = 3;
// 	}
// 	else if(mask == 0x88)
// 	{
// 		send_id = 4;
// 	}
// 	printf("send_id %d\n",send_id);


// 	if(send_id == room1_num)
// 	{
// 		lv_obj_set_state(room1,LV_STATE_CHECKED);
// 	}

// 	else if(send_id == room2_num)
// 	{
// 		lv_obj_set_state(room2,LV_STATE_CHECKED);
// 	}

// 	else if(send_id == room3_num)
// 	{
// 		lv_obj_set_state(room3,LV_STATE_CHECKED);
// 	}
// 	printf("room1 state :%d \n", room1->state);
// 	printf("room2 state :%d \n", room2->state);
// 	printf("room3 state :%d \n", room3->state);
// }

/* 为避免触发滑块的事件而重复查找对象，故在此声明全局变量 */
static lv_obj_t *vol_slider = NULL;
static lv_obj_t *mic_slider = NULL;
static lv_obj_t *vol_text = NULL;
static lv_obj_t *mic_text = NULL;

/*************************************************************************
 * @brief  滑块事件处理函数
 * @date   2022-09-01 11:36
 * @author xiaoele
 * @param  obj event
 **************************************************************************/
static void intercom_call_volome_mic_slider_event(lv_obj_t *obj, lv_event_t event)
{
	switch (event)
	{
		case LV_EVENT_VALUE_CHANGED:
			if (obj == vol_slider) /* 声音滑块 */
			{
				lv_label_set_text_fmt(vol_text, "%d", lv_slider_get_value(vol_slider));
				user_data_get()->audio.inter_ring_volume = lv_slider_get_value(vol_slider);
				if ((user_data_get()->audio.ring_mute == false) && (user_data_get()->audio.inter_ring_volume != 0))
				{
					ring_output_gpio_ctrl_volume(user_data_get()->audio.inter_ring_volume);
					ringplay_play_form_index(user_data_get()->audio.inter_tone, 100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, false /*user_data_get()->audio.inter_ring_volume*/);
				}
				else
				{
					ringplay_play_stop();
				}
			}
			else if (obj == mic_slider) /* 麦克风滑块 */
			{
				lv_label_set_text_fmt(mic_text, "%d", lv_slider_get_value(mic_slider));
				user_data_get()->audio.inter_talk_volume = lv_slider_get_value(mic_slider);
			}
			break;
		case LV_EVENT_RELEASED:
			/* 释放时保存数据 */
			
			user_data_save();
			break;
		// case LV_EVENT_DEFOCUSED:
		// 	lv_obj_set_hidden(obj->parent, true);
		default:
			break;
	}
}

/*************************************************************************
 * @brief  扬声器按钮弹起执行的函数
 * @date   2022-09-01 11:56
 * @author xiaoele
 **************************************************************************/
static void intercom_call_volome_btn_up(lv_obj_t *obj)
{
	lv_obj_t *vol_slider_cont = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_CALL_VOLOME_SLIDER_CONT_ID);

	if (vol_slider_cont == NULL)
	{
		return;
	}

	if (lv_obj_get_hidden(vol_slider_cont) == true)
	{
		lv_obj_set_hidden(vol_slider_cont, false);
	}
	else
	{
		lv_obj_set_hidden(vol_slider_cont, true);
	}
}

/*************************************************************************
 * @brief  铃声/声音 按钮 滑块 容器 文字
 * @date   2022-09-01 11:56
 * @author xiaoele
 **************************************************************************/
static void intercom_call_volome_btn_create(void)
{
	lv_obj_t *vol_btn = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(vol_btn, 80, 80);
	lv_obj_align(vol_btn, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 4, -4);
	lv_obj_set_style_local_radius(vol_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_obj_get_width(vol_btn));
	lv_obj_set_style_local_bg_opa(vol_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_30);
	lv_obj_set_style_local_bg_color(vol_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
	static rom_bin_info png = rom_bin_info_get(ROM_UI_INTERCOM_HORN_PNG);
	lv_obj_set_style_local_pattern_image(vol_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &png);

	lv_obj_t *vol_slider_cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(vol_slider_cont, INTERCOM_CALL_VOLOME_SLIDER_CONT_ID);
	lv_obj_set_size(vol_slider_cont, 200, 390);
	lv_obj_align(vol_slider_cont, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -lv_obj_get_height(vol_btn));
	lv_obj_set_hidden(vol_slider_cont, true);

	vol_text = lv_label_create(vol_slider_cont, NULL);
	lv_label_set_long_mode(vol_text, LV_LABEL_LONG_CROP);
	lv_label_set_align(vol_text, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(vol_text, 80, 40);
	lv_obj_align(vol_text, vol_btn, LV_ALIGN_OUT_TOP_MID, 0, -4);

	vol_slider = lv_slider_create(vol_slider_cont, NULL);
	lv_slider_set_range(vol_slider, 0, 4);
	lv_obj_set_size(vol_slider, 16, 256);
	lv_obj_set_ext_click_area(vol_slider, 16, 16, 8, 8);
	lv_obj_align(vol_slider, vol_text, LV_ALIGN_OUT_TOP_MID, 0, -16);
	lv_obj_set_event_cb(vol_slider, intercom_call_volome_mic_slider_event);
	lv_slider_set_value(vol_slider, user_data_get()->audio.inter_ring_volume, true);

	lv_obj_set_style_local_bg_opa(vol_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_OPA_90);
	lv_obj_set_style_local_bg_color(vol_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));

	lv_label_set_text_fmt(vol_text, "%d", lv_slider_get_value(vol_slider));

	static obj_click_data click_data = obj_click_data_up_create(intercom_call_volome_btn_up);
	obj_click_event_listen(vol_btn, &click_data);
}













/***
**   日期:2022-05-30 15:21:09
**   作者: leo.liu
**   函数作用：创建
**   参数说明:
***/
static void intercom_out_handup_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}
/***
**   日期:2022-12-08 17:05:36
**   作者: fei.liu
**   函数作用：按键接听/挂断函数
**   参数说明:
***/
static void layout_intercom_out_call_talk_btn_func(void)
{
	if(adc_value_get() < 400)
	{
		touch_sound_play(ringplay_keysound_start_default_func, NULL);
		goto_layout(pLAYOUT(home));
	}

}

static void LAYOUT_ENTER_FUNC(intercom_out)
{
	unsigned int mask = layout_intercom_call_mask_get();
	printf("layout_intercom_call_mask_set %d \n",mask);
	intercom_head_cont_crate(intercom_out_sct_act_obj_id_head_cont, layout_intercom_string_get(LAYOUT_INTERCOM_LANGUAGE_ID_CALLING));
	intercom_timeout_label_create(intercom_out_sct_act_obj_id_timeout_label, 30);
	// intercom_intercom_icon_create(false);
	intercom_call_id_label_create(intercom_out_sct_act_obj_id_callid_label);
	intercom_call_id_label_display(intercom_out_sct_act_obj_id_callid_label);


	intercom_call_call_id_btn_create();
	// intercom_call_id_display();
	intercom_call_volome_btn_create();

	static obj_click_data click_data_2 = obj_click_data_up_create(intercom_out_handup_up);    //挂断按钮
	intercom_call_handup_and_talk_create(462, intercom_out_sct_act_obj_id_handup_btn, &click_data_2, false);

	if ((user_data_get()->audio.ring_mute == false) && (user_data_get()->audio.inter_ring_volume != 0))
	{
		ring_output_gpio_ctrl_volume(user_data_get()->audio.inter_ring_volume);
		ringplay_play_form_index(user_data_get()->audio.inter_tone, 100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, true /*user_data_get()->audio.inter_ring_volume*/);
	}
	layout_call_talk_btn_callback_register(layout_intercom_out_call_talk_btn_func);
}

static void LAYOUT_QUIT_FUNC(intercom_out)
{
	ringplay_play_stop();
	const layout *p = cur_layout_get(); 
	if ((intercom_mastar_id_get() == OwnID) && (p != pLAYOUT(intercom_talk)) && (p != pLAYOUT(monitor)))
	{ 
		usleep(200 * 1000);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_REBOOT);
	}
	layout_call_talk_btn_callback_register(NULL);
}
CREATE_LAYOUT(intercom_out);