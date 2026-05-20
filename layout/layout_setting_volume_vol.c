#include "layout_define.h"
#include "ringplay.h"
#include "layout_setting_common.h"
#include "user_file.h"

#define LAYOUT_SETTING_VOLUME_SCHEDULE 0x01
#define LAYOUT_SETTING_VOLUME_RING_TIME 0x02
#define LAYOUT_SETTING_VOLUME_RING_MODE 0x03
#define LAYOUT_SETTING_VOLUME_RING_SELECT 0x04
#define LAYOUT_SETTING_VOLUME_RING_VOLUME 0x05
#define LAYOUT_SETTING_VOLUME_RING_START_HOUR 0x06
#define LAYOUT_SETTING_VOLUME_RING_START_MIN 0x07
#define LAYOUT_SETTING_VOLUME_RING_END_HOUR 0x08
#define LAYOUT_SETTING_VOLUME_RING_END_MIN 0x09





extern lv_obj_t *setting_main_btn_base_create(int x, int y, int w, int h, bool enable, const char *string, obj_click_data *data);

static void volume_setting_display(void);




bool set_door_ring_flag = 0;//0:door1   1:door2

static int set_ring_flag = 0;//0:ring1   1:ring2   2:ring3

static int  custom_ring_total = 0;
static file_type custom_ring_type = FILE_TYPE_SD_MUSIC;




static void volume_vol_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_volume));
}

/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声开始
** 返回参数说明：
***/
void layout_volume_vol_ringplay_doorcall_start_default_func(int index) 
{
	ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val);
}
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声结束
** 返回参数说明：
***/
void layout_volume_vol_ringplay_doorcall_finish_default_func(int index)
{
	/***** 开启功放 *****/
	
	power_amplifier_enable(false);
}






static void set_ring1_flag_up(lv_obj_t * obj)
{
	if(set_ring_flag != 0){
		
		set_ring_flag = 0;
		lv_obj_clean(lv_scr_act());
		
		volume_setting_display();
	}
}
static void set_ring2_flag_up(lv_obj_t * obj)
{
	if(set_ring_flag != 1){
		
		set_ring_flag = 1;
		lv_obj_clean(lv_scr_act());
		
		volume_setting_display();
	
	}
}
static void set_ring3_flag_up(lv_obj_t * obj)
{
	if(set_ring_flag != 2){
		
		set_ring_flag = 2;
		lv_obj_clean(lv_scr_act());
	
		volume_setting_display();
	
	}
}






/***
** 日期: 2022-04-28 10:55
** 作者: leo.liu
** 函数作用：设置侧边按钮创建
** 返回参数说明:select_id:设置当前页面：
***/
bool setting_volume_btn_create(int page_id)
{

	/***** 创建ring1按钮 *****/
	static obj_click_data record_click_data = obj_click_data_up_create(set_ring1_flag_up);
	setting_main_btn_base_create(10, 70, 280, 75, (page_id == 0) ? true : false, layout_setting_ring_string_get(RING_RING1), &record_click_data);
	/***** 创建 ring2按钮 *****/
	static obj_click_data wifi_click_data = obj_click_data_up_create(set_ring2_flag_up);
	setting_main_btn_base_create(10, 70 + 75, 280, 75, (page_id == 1) ? true : false, layout_setting_ring_string_get(RING_RING2), &wifi_click_data);
	/***** 创建ring3 *****/
	static obj_click_data volume_click_data = obj_click_data_up_create(set_ring3_flag_up);
	setting_main_btn_base_create(10, 70 + 75 * 2, 280, 75, (page_id == 2) ? true : false, layout_setting_ring_string_get(RING_RING3), &volume_click_data);

	return true;
}







lv_obj_t *sys_setting_btn_create(int x, int y, int w, int h, char *string1,const char *string_lable, obj_click_data * click_data, obj_click_data * btn_pdata1, obj_click_data * btn_pdata2)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
	
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, w, h);
	
    lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_50);

	lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x323237));   //下划线
	lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x323237));
	lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 1);
	lv_obj_set_style_local_border_side(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
	
    if (string1 != NULL)
    {
        lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,string1);
		lv_obj_set_style_local_value_color(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_hex(0xFFFFFF));
		lv_obj_set_style_local_value_align(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,LV_ALIGN_CENTER);
		lv_obj_set_style_local_value_ofs_x(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,180);
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




static int ring_start_time_temp,ring_end_time_temp;



static void setting_time_rooler_base_change(lv_obj_t *obj, lv_event_t ev)
{
	if (ev == LV_EVENT_VALUE_CHANGED)
	{
		lv_obj_t * parent = obj->parent;
		if(obj == lv_obj_get_child_form_id(parent, LAYOUT_SETTING_VOLUME_RING_START_HOUR))
		{
			ring_start_time_temp = (23 - lv_roller_get_selected(obj)) * 100 + ring_start_time_temp % 100;
		}
		else if(obj == lv_obj_get_child_form_id(parent, LAYOUT_SETTING_VOLUME_RING_START_MIN))
		{
			ring_start_time_temp = (59 - lv_roller_get_selected(obj)) + ring_start_time_temp / 100 * 100;
		}
		else if(obj == lv_obj_get_child_form_id(parent, LAYOUT_SETTING_VOLUME_RING_END_HOUR))
		{
			ring_end_time_temp = (23 - lv_roller_get_selected(obj)) * 100 + ring_end_time_temp % 100;
		}
		else if(obj == lv_obj_get_child_form_id(parent, LAYOUT_SETTING_VOLUME_RING_END_MIN))
		{
			ring_end_time_temp = (59 - lv_roller_get_selected(obj)) + ring_end_time_temp / 100 * 100;
		}
	}
}

static lv_obj_t *setting_ring_time_rooler_base(lv_obj_t *parent, int x, int y, int w, int h, int min, int max, int cur, int obj_id)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_click(obj, false);
	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);

	lv_obj_t *rooler = lv_roller_create(parent, NULL);
	lv_obj_set_id(rooler, obj_id);
	lv_obj_set_style_local_text_line_space(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 30);
	lv_obj_set_style_local_bg_opa(rooler, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	char opt[512] = {0};
	for (int i = max; i >= min; i--)
	{
		char buf[8] = {0};
		sprintf(buf, "%02d%s", i, i == min ? "" : "\n");
		strcat(opt, buf);
	}
	lv_roller_set_options(rooler, opt, LV_ROLLER_MODE_NORMAL);
	lv_roller_set_visible_row_count(rooler, 1);
	lv_obj_set_pos(rooler, x, y + 60);
	lv_obj_set_width(rooler, w);


	lv_roller_set_selected(rooler, max - cur, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_VOLUME_TIME_CHANGE_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	lv_obj_set_ext_click_area(rooler, 0, 0, 54, 54);
	static obj_click_data click_data = obj_click_data_anything_create(setting_time_rooler_base_change);
	obj_click_event_listen(rooler, &click_data);
	

	return rooler;
}



/***
** 日期: 2022-05-07 09:37
** 作者: leo.liu
** 函数作用：显示start time
** 返回参数说明：
***/
static void volume_start_time_text_display(lv_obj_t *parent, int x, int y, int w, int h,int cur)
{
	lv_obj_t *label = lv_label_create(parent, NULL);
	lv_obj_set_pos(label, x, y+65);
	lv_obj_set_size(label, w, h);


	char opt[512] = {0};
	char buf[8] = {0};
	sprintf(buf, "%02d", cur);
	strcat(opt, buf);
	lv_label_set_text(label, opt);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
    lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
}



static bool setting_time_start_hour_rooler_create(lv_obj_t *parent)
{
	// setting_ring_time_rooler_base(parent, 72,150, 80, 184, 0, 23, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start/100, LAYOUT_SETTING_VOLUME_RING_START_HOUR);
	if(set_ring_flag == 0)
	{
		volume_start_time_text_display(parent, 72,150, 80, 184, 0);
	}
	else
	{
		volume_start_time_text_display(parent, 72,150, 80, 184, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag-1].timer_end/100);
	}
	return true;
}

static bool setting_time_start_min_rooler_create(lv_obj_t *parent)
{
	// setting_ring_time_rooler_base(parent, 168,150, 80, 184, 0, 59, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start%100, LAYOUT_SETTING_VOLUME_RING_START_MIN);
	if(set_ring_flag == 0)
	{
		volume_start_time_text_display(parent, 168,150, 80, 184, 0);
	}
	else
	{
		volume_start_time_text_display(parent, 168,150, 80, 184, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag-1].timer_end%100);
	}
	return true;
}


static bool setting_time_end_hour_rooler_create(lv_obj_t *parent)
{
	if(set_ring_flag == 2)
	{
		volume_start_time_text_display(parent,  417,150, 80, 184, 23);
	}
	else
	{
		setting_ring_time_rooler_base(parent, 417,150, 80, 184, 0, 23, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end/100, LAYOUT_SETTING_VOLUME_RING_END_HOUR);
	}
	
	return true;
}

static bool setting_time_end_min_rooler_create(lv_obj_t *parent)
{
	if(set_ring_flag == 2)
	{
		volume_start_time_text_display(parent,  513,150, 80, 184, 59);
	}
	else
	{
		setting_ring_time_rooler_base(parent, 513,150, 80, 184, 0, 59,user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end%100, LAYOUT_SETTING_VOLUME_RING_END_MIN);
	}
	return true;
}







static void ring_time_set_ok_btn_up(lv_obj_t *obj)
{

	if((set_ring_flag == 0) && (ring_end_time_temp > user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag +1].timer_end))
	{
		message_window_pops(layout_setting_ring_string_get(COMMON_ERROR),0,0);
		return ;
	}
	if((set_ring_flag == 1) && (ring_end_time_temp < user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag -1].timer_end))
	{
		message_window_pops(layout_setting_ring_string_get(COMMON_ERROR),0,0);
		return ;
	}



	lv_obj_t* window_cont = lv_obj_get_child_form_id(lv_scr_act(),888);
	if(window_cont != NULL){
		lv_obj_del(window_cont);
	}

	if(set_ring_flag == 0)
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start = 0;
	}
	else
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start = user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag - 1].timer_end;
	}
	if(set_ring_flag == 2)
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end = 2359;
	}
	else
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end = ring_end_time_temp;
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag+1].timer_start = ring_end_time_temp;
	}
	printf("_________%s:%d__________[%d]:[%d]:[%d]\n",__func__,__LINE__,user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start,user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end,user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag+1].timer_start);
	// user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start = ring_start_time_temp;
	// user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end = ring_end_time_temp;
	
	static char str1[24] = {0};
	sprintf(str1,"%02d:%02d - %02d:%02d",	user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start/100,
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start%100,
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end/100,
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end%100);
	
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_SETTING_VOLUME_SCHEDULE);
	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
}

static void ring_time_set_cancel_btn_up(lv_obj_t *obj)
{
	lv_obj_t* window_cont = lv_obj_get_child_form_id(lv_scr_act(),888);
	if(window_cont != NULL){
		lv_obj_del(window_cont);
	}
}




static void sys_time_setting_window_create(void){

	if(set_ring_flag == 0)
	{
		ring_start_time_temp = 0;
	}
	else
	{
		ring_start_time_temp = user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag -1].timer_end;
	}
	// ring_start_time_temp = user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_start;
	
	if(set_ring_flag == 2)
	{
		ring_end_time_temp = 2359;
	}
	else
	{
		ring_end_time_temp = user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end;
	}
	printf("_________%s:%d__________[%d]:[%d]\n",__func__,__LINE__,ring_start_time_temp,ring_end_time_temp);
	
	lv_obj_t* window_cont = lv_cont_create(lv_scr_act(),NULL);
	lv_obj_set_style_local_bg_opa(window_cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT,  LV_OPA_TRANSP);
	lv_obj_set_pos(window_cont, 0, 0);
	lv_obj_set_size(window_cont,1024,600);
	lv_obj_set_id(window_cont,888);

	lv_obj_t *msgbox_cont = lv_cont_create(window_cont, NULL);
	lv_obj_set_id(msgbox_cont, 666);
	lv_obj_set_pos(msgbox_cont, 203, 84);
	lv_obj_set_size(msgbox_cont, 648, 441);
	lv_obj_set_style_local_bg_color(msgbox_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x090909));
	lv_obj_set_style_local_bg_opa(msgbox_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_radius(msgbox_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 12);

	lv_obj_t * img = lv_obj_create(msgbox_cont, NULL);
	lv_obj_set_pos(img, 2,82);
    lv_obj_set_size(img, 644, 2);
    static rom_bin_info info = rom_bin_info_get(ROM_UI_VOLUME_LINE_PNG);
	lv_obj_set_style_local_pattern_image(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &info);
	
	setting_time_start_hour_rooler_create(msgbox_cont);
	setting_time_start_min_rooler_create(msgbox_cont);
	setting_time_end_hour_rooler_create(msgbox_cont);
	setting_time_end_min_rooler_create(msgbox_cont);

	lv_obj_t *window_ok_btn = lv_btn_create(msgbox_cont, NULL);
	lv_obj_set_pos(window_ok_btn, 326, 364);
	lv_obj_set_size(window_ok_btn, 322, 77);
	// lv_obj_set_pos(window_ok_btn, 0, 364);
	// lv_obj_set_size(window_ok_btn, 322, 77);

	static rom_bin_info image_confirm = rom_bin_info_get(ROM_UI_02_BTN_BTN_BOTTOM_CONFIRM_PNG);
	lv_obj_set_style_local_pattern_image(window_ok_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image_confirm);

	lv_obj_set_style_local_bg_color(window_ok_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3BD741));
	lv_obj_set_style_local_bg_color(window_ok_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));
	lv_obj_set_style_local_bg_opa(window_ok_btn, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_radius(window_ok_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 12);
	static obj_click_data btn_data1 = obj_click_data_up_create(ring_time_set_ok_btn_up);
	obj_click_event_listen(window_ok_btn,&btn_data1);

	// lv_obj_set_style_local_value_str(window_ok_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_ring_string_get(COMMON_OK));
	lv_obj_set_style_local_value_color(window_ok_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
	lv_obj_set_style_local_value_align(window_ok_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_value_font(window_ok_btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));

	lv_obj_t *window_cancel_btn = lv_btn_create(msgbox_cont, NULL);
	lv_obj_set_pos(window_cancel_btn, 0, 364);
	lv_obj_set_size(window_cancel_btn, 322, 77);
	// lv_obj_set_pos(window_cancel_btn, 326, 364);
	// lv_obj_set_size(window_cancel_btn, 322, 77);
	// lv_obj_set_x(window_cancel_btn, 326);
	static rom_bin_info image_cancel = rom_bin_info_get(ROM_UI_02_BTN_BTN_BOTTOM_CANCEL_PNG);
	lv_obj_set_style_local_radius(window_cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 12);
	lv_obj_set_style_local_pattern_image(window_cancel_btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image_cancel);
	lv_obj_set_style_local_bg_opa(window_cancel_btn, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(window_cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x202020));
	lv_obj_set_style_local_bg_color(window_cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x3BD741));
	// lv_obj_set_style_local_value_str(window_cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_ring_string_get(COMMON_CANCEL));
	// lv_obj_add_state(window_cancel_btn, LV_STATE_PRESSED);
	static obj_click_data btn_data2 = obj_click_data_up_create(ring_time_set_cancel_btn_up);
	obj_click_event_listen(window_cancel_btn,&btn_data2);
}




static void ring_schedule_btn_up(lv_obj_t *obj)
{
	sys_time_setting_window_create();
}


static void setting_volume_schedule_btn_create(void)
{
	static char str1[24] = {0};
	if(set_ring_flag == 0)
	{
		sprintf(str1,"%02d:%02d - %02d:%02d",	0, 0,
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end/100,
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end%100);
	}
	else if(set_ring_flag == 2)
	{
		sprintf(str1,"%02d:%02d - %02d:%02d",	user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag-1].timer_end/100, 
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag-1].timer_end%100,
											23,59);
	}
	else
	{
		sprintf(str1,"%02d:%02d - %02d:%02d",	user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag-1].timer_end/100, 
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag-1].timer_end%100,
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end/100,
											user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].timer_end%100);
	}
	
	static obj_click_data click_data = obj_click_data_up_create(ring_schedule_btn_up);
	setting_right_btn_base_create(NULL,353, 90 + 90 * 0, 622, 90,
						      layout_setting_ring_string_get(RING_SCHEDULE),
						      str1,
						      &click_data,
						      LAYOUT_SETTING_VOLUME_SCHEDULE);
}


static void ring_time_set_left_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_SETTING_VOLUME_RING_TIME);          //定位到设置时间的id
	static char str1[20] = {0};
	if(--user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_time < 3)
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_time = 20;
	}
	sprintf(str1, "%d S",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_time);

	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
}
static void ring_time_set_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_SETTING_VOLUME_RING_TIME);
	static char str1[20] = {0};
	if(++user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_time > 20)
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_time = 3;
	}
	sprintf(str1, "%d S",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_time);

	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
}


static void setting_volume_ring_time_btn_create(void)
{

	static obj_click_data btn_data1 = obj_click_data_up_create(ring_time_set_left_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create(ring_time_set_right_btn_up);
	static char str1[20] = {0};
	sprintf(str1, "%d S",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_time);
	lv_obj_t * btn = sys_setting_btn_create(353, 90 + 90 * 1, 622, 90, str1, layout_setting_ring_string_get(RING_TIME), NULL, &btn_data1, &btn_data2);
	lv_obj_set_id(btn, LAYOUT_SETTING_VOLUME_RING_TIME);
}




void ring_ring_mode_set_btn_switch(void)
{
	lv_obj_t * btn1 = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_VOLUME_RING_MODE);
	lv_obj_t * btn2 = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_VOLUME_RING_SELECT);
	media_file_total_get(custom_ring_type, &custom_ring_total, NULL);
	if(((user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode) == 0) && (custom_ring_total == 0))
	{
		return;
	}
	static char str1[128] = {0};
	if((user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode = !user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode))
	{
		
		// printf("custom_ring_total ====>>%d\n\r",custom_ring_total);
		if(custom_ring_total)
		{
			const file_info *info = media_file_info_get(custom_ring_type, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring);
			sprintf(str1, "%s",info->file_name);
			printf("==============================>>>custom_ring : [%d] <><><> file_name : [%s] \n", user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring, str1);
		}
		else
		{
			memset(str1, 0, strlen(str1));      //将字符串str清零
		} 
	}
	else
	{
		memset(str1, 0, strlen(str1));
		sprintf(str1, "%d",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring);
	}

	static char str2[128] = {0};
	sprintf(str2,"%s",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode == 0 ?layout_setting_ring_string_get(RING_MODE_STD):layout_setting_ring_string_get(RING_MODE_CUSTOM));


	lv_obj_set_style_local_value_str(btn1,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str2);
	lv_obj_set_style_local_value_str(btn2,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT, str1);
}


static void ring_ring_mode_set_btn_up(lv_obj_t *obj)
{
	if(media_sdcard_insert_check())
	{
		ring_ring_mode_set_btn_switch();
	}
}

static void setting_volume_ring_mode_btn_create(void)
{
	static obj_click_data btn_data1 = obj_click_data_up_create( ring_ring_mode_set_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create( ring_ring_mode_set_btn_up);
	media_file_total_get(custom_ring_type, &custom_ring_total, NULL);
	// user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode = media_sdcard_insert_check() ? 1 : 0;
	if(media_sdcard_insert_check() == false)
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode = 0;
	}
	else if (custom_ring_total == 0)
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode = 0;
	}
	static char str1[128] = {0};
	sprintf(str1,"%s",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode == 0 ?layout_setting_ring_string_get(RING_MODE_STD):layout_setting_ring_string_get(RING_MODE_CUSTOM));


	lv_obj_t *btn =  sys_setting_btn_create(353, 90 + 90 * 2, 622, 90, str1, layout_setting_ring_string_get(RING_MODE), NULL,&btn_data1,&btn_data2);
	lv_obj_set_id(btn, LAYOUT_SETTING_VOLUME_RING_MODE);
}



static void ring_ring_select_set_left_btn_up(lv_obj_t *obj)
{
	
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_VOLUME_RING_SELECT);

	static char str1[128] = {0};
	if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode)
	{
		media_file_total_get(custom_ring_type, &custom_ring_total, NULL);
		// printf("custom_ring_total ====>>%d\n\r",custom_ring_total);
		if(custom_ring_total)
		{
			if(--user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring < 0)
			{
				user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring = custom_ring_total - 1;
			}
			const file_info *info = media_file_info_get(custom_ring_type, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring);
			sprintf(str1, "%s",info->file_name);
			lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT, str1);
			printf("==============================>>>custom_ring : [%d] <><><> file_name : [%s] \n", user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring, str1);
			if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val == 0)
			{
				return;
			}
			custom_music_play(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring, 100, layout_volume_vol_ringplay_doorcall_start_default_func, layout_volume_vol_ringplay_doorcall_finish_default_func, false);
			
		}
		else
			memset(str1, 0, strlen(str1));
	}
	else
	{
		if(--user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring < 1){
			user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring = 6;
		}
		sprintf(str1, "%d",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring);
		lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT, str1);
		if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val == 0)
		{
			return;
		}
		ringplay_play_form_index(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring,
								100, 
								layout_volume_vol_ringplay_doorcall_start_default_func, 
								layout_volume_vol_ringplay_doorcall_finish_default_func, false);
	
	}
}

static void ring_ring_select_set_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_VOLUME_RING_SELECT);

	static char str1[128] = {0};
	if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode)
	{
		media_file_total_get(custom_ring_type, &custom_ring_total, NULL);
		printf("custom_ring_total ====>>%d\n\r",custom_ring_total);
		if(custom_ring_total)
		{
			if(++user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring >= custom_ring_total)
			{
				user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring = 0;
			}
			const file_info *info = media_file_info_get(custom_ring_type, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring);
			sprintf(str1, "%s",info->file_name);
			lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
			if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val == 0)
			{
				return;
			}
			printf("==============================>>>custom_ring : [%d] <><><> file_name : [%s] \n", user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring, str1);
			custom_music_play(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring, 100, layout_volume_vol_ringplay_doorcall_start_default_func, layout_volume_vol_ringplay_doorcall_finish_default_func, false);
			
		}
		else
			memset(str1, 0, strlen(str1));
	}
	else
	{
		if(++user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring > 6){
			user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring = 1;
		}
		sprintf(str1, "%d",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring);
		lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
		if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val == 0)
		{
			return;
		}
		ringplay_play_form_index(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring, 100, layout_volume_vol_ringplay_doorcall_start_default_func, layout_volume_vol_ringplay_doorcall_finish_default_func, false);
		
	}
}
static void setting_volume_ring_select_btn_create(void)
{
	static obj_click_data btn_data1 = obj_click_data_up_create( ring_ring_select_set_left_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create( ring_ring_select_set_right_btn_up);

	if(media_sdcard_insert_check() == false)
	{
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode = 0;
	}
	
	static char str1[128] = {0};
	if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode)
	{
		media_file_total_get(custom_ring_type, &custom_ring_total, NULL);
		printf("custom_ring_total ====>>%d\n\r",custom_ring_total);
		if(custom_ring_total)
		{
			const file_info *info = media_file_info_get(custom_ring_type, user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring);
			sprintf(str1, "%s",info->file_name);
		}
		else
		{

			memset(str1, 0, strlen(str1));      //将字符串str清零
		} 
	}
	else
	{
		sprintf(str1, "%d",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring);
	}
		

	user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode = media_sdcard_insert_check() ? user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode : false;
	lv_obj_t *btn =  sys_setting_btn_create(353, 90 + 90 * 3, 622, 90, str1, layout_setting_ring_string_get(RING_SELECT),NULL,&btn_data1,&btn_data2);
	lv_obj_set_id(btn, LAYOUT_SETTING_VOLUME_RING_SELECT);	
}




static void ring_ring_volume_set_left_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_SETTING_VOLUME_RING_VOLUME);
	static char str1[32] = {0};
	if(--user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val < 0){
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val= 4;
	}
	sprintf(str1, "%d",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val);
	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);
	if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val == 0)
	{
		ringplay_play_stop();
		power_amplifier_enable(false);
		// return;
	}
	else
	{
		if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode == 0)
		{
			ringplay_play_form_index(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring,100, layout_volume_vol_ringplay_doorcall_start_default_func, layout_volume_vol_ringplay_doorcall_finish_default_func, false);
		}
		else
		{
			custom_music_play(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring, 100, layout_volume_vol_ringplay_doorcall_start_default_func, layout_volume_vol_ringplay_doorcall_finish_default_func, false);
		}
	}
	
	
}
static void ring_ring_volume_set_right_btn_up(lv_obj_t *obj)
{
	lv_obj_t * btn = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_SETTING_VOLUME_RING_VOLUME);
	static char str1[32] = {0};
	if(++user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val > 4){
		user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val = 0;
	}

	sprintf(str1, "%d",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val);

	lv_obj_set_style_local_value_str(btn,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,str1);

	if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val == 0)
	{
		ringplay_play_stop();
		power_amplifier_enable(false);
		return;
	}
	else
	{
		if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode == 0)
		{
			ringplay_play_form_index(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring, 100, layout_volume_vol_ringplay_doorcall_start_default_func, layout_volume_vol_ringplay_doorcall_finish_default_func, false);
		}
		else
		{
			custom_music_play(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].custom_ring,100, layout_volume_vol_ringplay_doorcall_start_default_func, layout_volume_vol_ringplay_doorcall_finish_default_func, false);
		}
		}
}


static void setting_volume_ring_volume_btn_create(void)
{


	static obj_click_data btn_data1 = obj_click_data_up_create(ring_ring_volume_set_left_btn_up);
	static obj_click_data btn_data2 = obj_click_data_up_create( ring_ring_volume_set_right_btn_up);
	// static obj_click_data btn_data3 = obj_click_data_up_create(NULL);

	static char str1[32] = {0};
	sprintf(str1, "%d",user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_val);
	lv_obj_t * btn = sys_setting_btn_create(353, 90 + 90 * 4, 622, 90, str1, layout_setting_ring_string_get(RING_VOLUME), NULL,&btn_data1,&btn_data2);
	lv_obj_set_id(btn, LAYOUT_SETTING_VOLUME_RING_VOLUME);
}



static void setting_sdcard_callback()
{
	printf("--------=====121212122\n");
	if(user_data_get()->ring_attr[set_door_ring_flag][set_ring_flag].ring_mode)
	{
		ring_ring_mode_set_btn_switch();
	}
	if((media_sdcard_insert_check() == false) && (user_data_get()->auto_record_mode == 2)) {
		user_data_get()->auto_record_mode = 1;
	}

	// if(media_sdcard_insert_check() == true)
	// {
	// 	copy_flash_photo_to_sd();
	// }
}


static void volume_setting_display(void)
{
	setting_cancel_btn_create(volume_vol_cancel_btn_up);
	setting_head_label_create(layout_setting_record_string_get(set_door_ring_flag == 0 ? SETTING_VOLUME_LANG_ID_DOOR1 : SETTING_VOLUME_LANG_ID_DOOR2));

	setting_volume_btn_create(set_ring_flag);
	setting_volume_schedule_btn_create();
	setting_volume_ring_time_btn_create();
	setting_volume_ring_mode_btn_create();
	setting_volume_ring_select_btn_create();
	setting_volume_ring_volume_btn_create();
}

static void LAYOUT_ENTER_FUNC(setting_volume_vol)
{
	volume_setting_display();
	int total = 0;
	int new_total = 0;
	media_file_total_get(FILE_TYPE_SD_MUSIC, &total, &new_total);
	
	layout_sd_state_callback_register(setting_sdcard_callback);

}

static void LAYOUT_QUIT_FUNC(setting_volume_vol)
{
	ringplay_play_stop();
	user_data_save();
	set_ring_flag = 0;
	layout_sd_state_callback_register(NULL);
}

CREATE_LAYOUT(setting_volume_vol);