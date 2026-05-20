#include "layout_define.h"
#include "user_intercom.h"
typedef enum
{
	intercom_talk_sct_act_obj_id_head_cont,
	intercom_talk_sct_act_obj_id_timeout_label,
	intercom_talk_sct_act_obj_id_callid_label,
	intercom_talk_sct_act_obj_id_vol_btn,
	intercom_talk_sct_act_obj_id_handup_btn,
	intercom_talk_sct_act_obj_id_vol_cont,
	intercom_talk_sct_act_obj_id_vol_bar,
} intercom_in_sct_act_obj_id;

typedef enum
{
	INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_1 = 0x01,
	INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_2,
	INTERCOM_SCR_ACTL_OBJ_ID_CALL_NUM_3,
	INTERCOM_CALL_BTN_ID,

	INTERCOM_CALL_MIC_SLIDER_CONT_ID,
	INTERCOM_CALL_VOLOME_SLIDER_CONT_ID,
} intercom_scr_actl_obj_id;


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
		}
		else if (obj == mic_slider) /* 麦克风滑块 */
		{
			lv_label_set_text_fmt(mic_text, "%d", lv_slider_get_value(mic_slider));
			user_data_get()->audio.inter_talk_volume = lv_slider_get_value(mic_slider);
		}
		break;
	case LV_EVENT_RELEASED:
		/* 释放时保存数据 */
		talk_output_gpio_ctrl_volume(user_data_get()->audio.inter_talk_volume);
		user_data_save();
		break;
	// case LV_EVENT_DEFOCUSED:
	// 	lv_obj_set_hidden(obj->parent, true);
	default:
		break;
	}
}



/*************************************************************************
 * @brief  麦克风按钮弹起时执行的函数
 * @date   2022-09-01 11:55
 * @author xiaoele
 * @param  obj
 **************************************************************************/
static void intercom_call_mic_btn_up(lv_obj_t *obj)
{
	lv_obj_t *mic_slider_cont = lv_obj_get_child_form_id(lv_scr_act(), INTERCOM_CALL_MIC_SLIDER_CONT_ID);

	if (mic_slider_cont == NULL)
	{
		return;
	}

	if (lv_obj_get_hidden(mic_slider_cont) == true)
	{
		lv_obj_set_hidden(mic_slider_cont, false);
	}
	else
	{
		lv_obj_set_hidden(mic_slider_cont, true);
	}
}



/*************************************************************************
 * @brief  麦克风 按钮 滑块 容器
 * @date   2022-09-01 11:54
 * @author xiaoele
 **************************************************************************/
static void intercom_call_mic_btn_create(void)
{
	lv_obj_t *mic_btn = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(mic_btn, 80, 80);
	lv_obj_align(mic_btn, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -4, -4);

	lv_obj_set_style_local_radius(mic_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_obj_get_width(mic_btn));
	lv_obj_set_style_local_bg_opa(mic_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_30);
	lv_obj_set_style_local_bg_color(mic_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

	static rom_bin_info png = rom_bin_info_get(ROM_UI_INTERCOM_HORN_PNG);
	lv_obj_set_style_local_pattern_image(mic_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &png);

	/* 滑块容器 */
	lv_obj_t *mic_slider_cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(mic_slider_cont, INTERCOM_CALL_MIC_SLIDER_CONT_ID);
	lv_obj_set_size(mic_slider_cont, 200, 390);
	lv_obj_align(mic_slider_cont, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, -lv_obj_get_height(mic_btn));
	lv_obj_set_hidden(mic_slider_cont, true);

	/* 滑块文字 */
	mic_text = lv_label_create(mic_slider_cont, NULL);
	lv_label_set_long_mode(mic_text, LV_LABEL_LONG_CROP);
	lv_label_set_align(mic_text, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_size(mic_text, 80, 40);
	lv_obj_align(mic_text, mic_btn, LV_ALIGN_OUT_TOP_MID, 0, -4);

	/* 滑块 */
	mic_slider = lv_slider_create(mic_slider_cont, NULL);
	lv_slider_set_range(mic_slider, 1, 4);
	lv_obj_set_size(mic_slider, 16, 256);
	lv_obj_set_ext_click_area(mic_slider, 16, 16, 8, 8);
	lv_obj_align(mic_slider, mic_text, LV_ALIGN_OUT_TOP_MID, 0, -16);
	lv_obj_set_event_cb(mic_slider, intercom_call_volome_mic_slider_event);
	lv_slider_set_value(mic_slider, user_data_get()->audio.inter_talk_volume, true);

	lv_obj_set_style_local_bg_opa(mic_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, LV_OPA_90);
	lv_obj_set_style_local_bg_color(mic_slider, LV_SLIDER_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));

	lv_label_set_text_fmt(mic_text, "%d", lv_slider_get_value(mic_slider));

	static obj_click_data click_data = obj_click_data_up_create(intercom_call_mic_btn_up);
	obj_click_event_listen(mic_btn, &click_data);
}





/***
**   日期:2022-05-31 14:06:44
**   作者: leo.liu
**   函数作用：判断mask是否发生变化
**   参数说明:
***/
static void intercom_talk_id_label_task(lv_task_t *task_t)
{
	unsigned int *mask = (unsigned int *)task_t->user_data;
	if ((*mask) != layout_intercom_call_mask_get())
	{
		LOG_BLUE("intercom to home\n");
		*mask = layout_intercom_call_mask_get();
		intercom_call_id_label_display(intercom_talk_sct_act_obj_id_callid_label);
	}
}
/***
** 日期: 2022-04-28 09:53
** 作者: leo.liu
** 函数作用：监控被按下执行的回调函数
** 返回参数说明：
***/
void layout_intercom_talk_click_down_func(lv_obj_t *obj)
{
	if(user_data_get()->home_mode == 1)
	{
		return;
	}
	//***** 控制声音流向 *****/
	touch_sound_play(NULL, NULL);
}

static void intercom_talk_handup_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}


/*** 
**   日期:2022-12-08 17:05:36
**   作者: fei.liu
**   函数作用：按键接听函数
**   参数说明:
***/
static void layout_intercom_talk_call_talk_btn_func(void)
{
	if(adc_value_get() < 400)
	{
		touch_sound_play(ringplay_keysound_start_default_func, NULL);
		goto_layout(pLAYOUT(home));
	}

}

static void LAYOUT_ENTER_FUNC(intercom_talk)
{
	standby_timer_close();
	intercom_head_cont_crate(intercom_talk_sct_act_obj_id_head_cont, layout_intercom_string_get(LAYOUT_INTERCOM_LANGUAGE_ID_TALKING));
	intercom_timeout_label_create(intercom_talk_sct_act_obj_id_timeout_label, 60);
	intercom_intercom_icon_create(false);
	intercom_call_id_label_create(intercom_talk_sct_act_obj_id_callid_label);
	intercom_call_id_label_display(intercom_talk_sct_act_obj_id_callid_label);


	// static obj_click_data click_data_1 = obj_click_data_up_create(intercom_talk_vol_btn_up);
	// intercom_vol_btn_create(intercom_talk_sct_act_obj_id_vol_btn, true, &click_data_1);
	intercom_call_mic_btn_create();

	static obj_click_data click_data_2 = obj_click_data_up_create(intercom_talk_handup_up);
	intercom_call_handup_and_talk_create(462, intercom_talk_sct_act_obj_id_handup_btn, &click_data_2, false);

	// static obj_click_data left = obj_click_data_up_create(intercom_talk_left_up);
	// static obj_click_data right = obj_click_data_up_create(intercom_talk_right_up);
	// intercom_vol_cont_create(intercom_talk_sct_act_obj_id_vol_cont, intercom_talk_sct_act_obj_id_vol_bar, &left, &right, user_data_get()->audio.inter_talk_volume);

	static unsigned int mask = 0;
	mask = layout_intercom_call_mask_get();
	lv_layout_task_create(intercom_talk_id_label_task, 1000, LV_TASK_PRIO_MID, &mask);

	ringplay_play_stop();
	lv_obj_click_down_callback_register(layout_intercom_talk_click_down_func);
	door_audio_talk(AUDIO_CH_INTER);

	layout_call_talk_btn_callback_register(layout_intercom_talk_call_talk_btn_func);
}
static void LAYOUT_QUIT_FUNC(intercom_talk)
{
	standby_timer_restart(true);
	lv_obj_click_down_callback_register(layout_obj_click_down_func);
	door_audio_talk(AUDIO_CH_CLOSE);
	LOG_GREEN("set release 3\n");
	// intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
	const layout *p = cur_layout_get();
	if ((p != pLAYOUT(monitor))&&(p != pLAYOUT(tuya_monitor)))
	{
		usleep(200 * 1000);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
	}
	layout_call_talk_btn_callback_register(NULL);
}
CREATE_LAYOUT(intercom_talk);