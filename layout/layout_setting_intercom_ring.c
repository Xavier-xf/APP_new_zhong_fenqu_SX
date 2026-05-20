#include "layout_define.h"
#include "ringplay.h"
#include "layout_setting_common.h"
#include "user_file.h"

#define LAYOUT_SETTING_INTERCOM_RING_SELECT 0x01
#define LAYOUT_SETTING_INTERCOM_RING_VOLUME 0x02






lv_obj_t *sys_setting_intercom_btn_create(int x, int y, int w, int h, char *string1,const char *string_lable, obj_click_data * click_data, obj_click_data * btn_pdata1, obj_click_data * btn_pdata2)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
	
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
    lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_50);

	lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x323237));
	lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x323237));
	lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 1);
	lv_obj_set_style_local_border_side(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
	
    if (string1 != NULL) 
    {
        lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,string1);
		lv_obj_set_style_local_value_color(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_hex(0xFFFFFF));
		lv_obj_set_style_local_value_align(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,LV_ALIGN_CENTER);
		lv_obj_set_style_local_value_ofs_x(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,325);
		lv_obj_set_style_local_value_font(btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
    }
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
   	lv_obj_set_pos(label, x, y+25);
	lv_obj_set_size(label, 250, 52);
	lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);
	lv_label_set_text(label, string_lable);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);

	obj_click_event_listen(btn, click_data);

	if(btn_pdata1 != NULL){
    	lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
		lv_obj_set_pos(btn1, 674, y+20);
    	lv_obj_set_size(btn1, 48, 49);
		lv_obj_set_ext_click_area(btn1, 20, 20, 2, 2);
	    static rom_bin_info info = rom_bin_info_get(ROM_UI_VOLUME_LIGHT_PNG);
		lv_obj_set_style_local_pattern_image(btn1,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,&info);
		lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_bg_color(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
		lv_obj_set_style_local_bg_color(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x3ac392));

		obj_click_event_listen(btn1, btn_pdata1);
	}
	if(btn_pdata2 != NULL){
		lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
		lv_obj_set_pos(btn1, 937, y+20);
    	lv_obj_set_size(btn1, 48, 49);
		lv_obj_set_ext_click_area(btn1, 20, 20, 2, 2);
	    static rom_bin_info info = rom_bin_info_get(ROM_UI_VOLUME_RIGHT_PNG);
		lv_obj_set_style_local_pattern_image(btn1,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,&info);
		lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_bg_color(btn1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
		lv_obj_set_style_local_bg_color(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x3ac392));

		obj_click_event_listen(btn1, btn_pdata2);
	
	}
    return btn;
}






static void ring_ring_select_set_left_btn_up(lv_obj_t *obj)
{
	
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_INTERCOM_RING_SELECT);

	static char str1[20] = {0};
	
    if(--user_data_get()->audio.inter_tone < 1)
    {
        user_data_get()->audio.inter_tone = 6;
    }
    sprintf(str1, "%d",user_data_get()->audio.inter_tone);
	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT, str1);
	if(user_data_get()->audio.inter_ring_volume == 0)
	{
		return;
	}
    ringplay_play_form_index(user_data_get()->audio.inter_tone, 100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, false);
}

static void ring_ring_select_set_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_INTERCOM_RING_SELECT);

	static char str1[20] = {0};
	
    if(++user_data_get()->audio.inter_tone > 6)
    {
        user_data_get()->audio.inter_tone = 1;
    }
    sprintf(str1, "%d",user_data_get()->audio.inter_tone);
	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
	if(user_data_get()->audio.inter_ring_volume == 0)
	{
		return;
	}
    ringplay_play_form_index(user_data_get()->audio.inter_tone, 100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, false);
	
}
static void setting_intercom_ring_select_btn_create(void)
{
	static obj_click_data btn_data1 = obj_click_data_up_create( ring_ring_select_set_left_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create( ring_ring_select_set_right_btn_up);

	static char str1[32] = {0};
    sprintf(str1, "%d",user_data_get()->audio.inter_tone);
	
	lv_obj_t *btn =  sys_setting_intercom_btn_create(49, 92, 919, 90, str1, layout_setting_ring_string_get(RING_SELECT),NULL,&btn_data1,&btn_data2);
	lv_obj_set_id(btn, LAYOUT_SETTING_INTERCOM_RING_SELECT);	
}








static void ring_ring_volume_set_left_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_SETTING_INTERCOM_RING_VOLUME);
	static char str1[4] = {0};
	if(--user_data_get()->audio.inter_ring_volume < 0)
    {
		user_data_get()->audio.inter_ring_volume = 4;
	}
	sprintf(str1, "%d",user_data_get()->audio.inter_ring_volume);
	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
	if(user_data_get()->audio.inter_ring_volume ==0)
	{
		ringplay_play_stop();
		// power_amplifier_enable(false);
		return;
	}
	else
	{
		ringplay_play_form_index(user_data_get()->audio.inter_tone,100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, false);
	}
}
static void ring_ring_volume_set_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_SETTING_INTERCOM_RING_VOLUME);
	static char str1[4] = {0};
	if(++user_data_get()->audio.inter_ring_volume > 4)
    {
		user_data_get()->audio.inter_ring_volume = 0;
	}
	sprintf(str1, "%d",user_data_get()->audio.inter_ring_volume);
	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
	if(user_data_get()->audio.inter_ring_volume ==0)
	{
		ringplay_play_stop();
		// power_amplifier_enable(false);
		return;
	}
	else
	{
		ringplay_play_form_index(user_data_get()->audio.inter_tone,100, ringplay_intercom_start_default_func, ringplay_intercom_finish_default_func, false);
	}
	
}


static void setting_intercom_ring_volume_btn_create(void)
{


	static obj_click_data btn_data1 = obj_click_data_up_create(ring_ring_volume_set_left_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create( ring_ring_volume_set_right_btn_up);

	static char str1[4] = {0};
	 sprintf(str1, "%d",user_data_get()->audio.inter_ring_volume);
	lv_obj_t * btn = sys_setting_intercom_btn_create(49, 200, 919, 90, str1, layout_setting_ring_string_get(RING_VOLUME), NULL,&btn_data1,&btn_data2);
	lv_obj_set_id(btn, LAYOUT_SETTING_INTERCOM_RING_VOLUME);
}



static void volume_vol_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_volume));
}


static void LAYOUT_ENTER_FUNC(intercom_ring)
{
	setting_cancel_btn_create(volume_vol_cancel_btn_up);
	setting_head_label_create(layout_setting_record_string_get(SETTING_VOLUME_LANG_ID_INTERCOM));

	setting_intercom_ring_select_btn_create();
    setting_intercom_ring_volume_btn_create();
}

static void LAYOUT_QUIT_FUNC(intercom_ring)
{
	ringplay_play_stop();
	user_data_save();
}

CREATE_LAYOUT(intercom_ring);