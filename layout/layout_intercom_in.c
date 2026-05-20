#include "layout_define.h"

typedef enum
{
	intercom_in_sct_act_obj_id_head_cont,
	intercom_in_sct_act_obj_id_timeout_label,
	intercom_in_sct_act_obj_id_callid_label,
	intercom_in_sct_act_obj_id_vol_btn,
	intercom_in_sct_act_obj_id_handup_btn,
	intercom_in_sct_act_obj_id_talk_btn,
	intercom_in_sct_act_obj_id_vol_cont,
	intercom_in_sct_act_obj_id_vol_bar,

	INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_1 ,
	INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_2,
	INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_3,
	INTERCOM_CALL_BTN_ID,

	INTERCOM_CALL_MIC_SLIDER_CONT_ID,
	INTERCOM_CALL_VOLOME_SLIDER_CONT_ID,

} intercom_in_sct_act_obj_id;

// typedef enum
// {
// 	INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_1 = 0x01,
// 	INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_2,
// 	INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_3,
// 	INTERCOM_CALL_BTN_ID,

// 	INTERCOM_CALL_MIC_SLIDER_CONT_ID,
// 	INTERCOM_CALL_VOLOME_SLIDER_CONT_ID,
// } intercom_scr_actl_obj_id;
void intercom_head_cont_crate(int obj_id, const char *string);
void intercom_timeout_label_create(int obj_id, int timeout);
void intercom_intercom_icon_create(bool talk);
void intercom_call_id_label_create(int obj_id);
void intercom_call_id_label_display(int find_id);
void intercom_vol_btn_create(int obj_id, bool talk, const obj_click_data *click_data);
void intercom_call_handup_and_talk_create(int x, int obj_id, const obj_click_data *click_data, bool talk);
void intercom_vol_cont_create(int cont_id, int bar_id, const obj_click_data *left, const obj_click_data *right, int vol);
void intercom_obj_hidden(int vol_id, int talk_id, int hand_up, bool en, int cont_id, void (*vol_other_up)(lv_obj_t *obj));








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

	// lv_obj_t * vol_slider = lv_obj_get_child_form_id(vol_slider_cont, INTERCOM_CALL_VOLOME_SLIDER_ID);

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

static void intercom_call_room_icon(int x, const char *text, intercom_in_sct_act_obj_id id)
{
	/* 底色 */
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, id);
	lv_obj_set_size(cont, 209, 208);
	lv_obj_set_pos(cont, x, 185);
	lv_obj_set_style_local_radius(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 26);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, LV_OPA_100);
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
static void intercom_call_call_id_btn_create(void)
{
	intercom_call_room_icon(163, layout_setting_etc_string_get(user_data_get()->etc.deive_id == 0 ? SETTING_ETC_LANG_ID_DEVICE_ID2 : SETTING_ETC_LANG_ID_DEVICE_ID1), INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_1);
	intercom_call_room_icon(407, layout_setting_etc_string_get(user_data_get()->etc.deive_id < 2 ? SETTING_ETC_LANG_ID_DEVICE_ID3 : SETTING_ETC_LANG_ID_DEVICE_ID2), INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_2);
	intercom_call_room_icon(651, layout_setting_etc_string_get(user_data_get()->etc.deive_id < 3 ? SETTING_ETC_LANG_ID_DEVICE_ID4 : SETTING_ETC_LANG_ID_DEVICE_ID3), INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_3);
}




static void intercom_call_id_display(void)
{

	lv_obj_t *room1 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_1);
	lv_obj_t *room2 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_2);
	lv_obj_t *room3 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_3);


	int room1_num = user_data_get()->etc.deive_id == 0 ? 2 : 1;
	int room2_num = user_data_get()->etc.deive_id < 2 ? 3 : 2;
	int room3_num = user_data_get()->etc.deive_id < 3 ? 4 : 3;
	int call_id = intercom_mastar_id_get();

	printf("room1:%d, room2:%d, room3:%d  call_id:%d  \n", room1_num, room2_num, room3_num, call_id);

	if(call_id == room1_num)
	{
		lv_obj_set_state(room1,LV_STATE_CHECKED);
	}
	else if(call_id == room2_num)
	{
		lv_obj_set_state(room2,LV_STATE_CHECKED);
	}
	else if(call_id == room3_num)
	{
		lv_obj_set_state(room3,LV_STATE_CHECKED);
	}
}




static void intercom_in_talk_up(lv_obj_t *obj)
{
	intercom_cmd_send(CODE_ALL_ID, CMD_INTERPHONE_ANSWER);
	goto_layout(pLAYOUT(intercom_talk));
}
static void intercom_in_handup_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}

/*** 
**   日期:2022-12-08 17:05:36
**   作者: fei.liu
**   函数作用：物理按键接听函数
**   参数说明:
***/
static void layout_intercom_talk_call_talk_btn_func(void)
{
	if(adc_value_get() < 400){
		touch_sound_play(ringplay_keysound_start_default_func, NULL);
		intercom_cmd_send(CODE_ALL_ID, CMD_INTERPHONE_ANSWER);
		goto_layout(pLAYOUT(intercom_talk));
	}

}

static void LAYOUT_ENTER_FUNC(intercom_in)
{
	intercom_head_cont_crate(intercom_in_sct_act_obj_id_head_cont, layout_intercom_string_get(LAYOUT_INTERCOM_LANGUAGE_ID_CALLING));
	intercom_timeout_label_create(intercom_in_sct_act_obj_id_timeout_label, 30);

	intercom_call_volome_btn_create();
	intercom_call_call_id_btn_create();
	intercom_call_id_display();

	static obj_click_data click_data_2 = obj_click_data_up_create(intercom_in_handup_up);
	intercom_call_handup_and_talk_create(549, intercom_in_sct_act_obj_id_handup_btn, &click_data_2, false);

	static obj_click_data click_data_3 = obj_click_data_up_create(intercom_in_talk_up);
	intercom_call_handup_and_talk_create(375, intercom_in_sct_act_obj_id_talk_btn, &click_data_3, true);

	door_audio_talk(AUDIO_CH_CLOSE);

	// layout_call_talk_btn_callback_register();

	if ((user_data_get()->audio.ring_mute == false) && (user_data_get()->audio.inter_ring_volume != 0))
	{
		ring_output_gpio_ctrl_volume(user_data_get()->audio.inter_ring_volume);
		ringplay_play_form_index(user_data_get()->audio.inter_tone, 100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, true/*user_data_get()->audio.inter_ring_volume*/);
	}
	lv_obj_t *room1 = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_IN_ACTL_OBJ_ID_CALL_NUM_1);


	layout_call_talk_btn_callback_register(layout_intercom_talk_call_talk_btn_func);
	printf("room1 state4545 :%d \n", room1->state);
}
static void LAYOUT_QUIT_FUNC(intercom_in)
{
	ringplay_play_stop();
	const layout *p = cur_layout_get();
	if ((p != pLAYOUT(intercom_talk))&&( p != pLAYOUT(monitor))) 
	{
		intercom_cmd_send(layout_intercom_call_mask_get(), CMD_INTERPHONE_QUIT);
	}
	layout_call_talk_btn_callback_register(NULL);
}
CREATE_LAYOUT(intercom_in);