#include "layout_define.h"
/***
**   日期:2022-07-14 17:09:28
**   作者: leo.liu
**   函数作用：画面调整根obj id
**   参数说明:
***/
typedef enum
{
	adj_disp_scr_act_obj_id_adj_head,
	adj_disp_scr_act_obj_id_adj_cont,
} adj_disp_scr_act_obj_id;
/***
**   日期:2022-07-14 17:09:51
**   作者: leo.liu
**   函数作用：画面调整的子obj id
**   参数说明:
***/
typedef enum
{
	adj_cont_obj_id_brighness = 0,
	adj_cont_obj_id_const = 2,
	adj_cont_obj_id_color = 4,
} adj_cont_obj_id;

// void layout_display_adj_bar_base_create(lv_obj_t *parent, int x, int w, int y, int max, obj_click_data *left_click, obj_click_data *right_click, int bar_id, int vol, char flg);
/***
**   日期:2022-07-14 17:10:36
**   作者: leo.liu
**   函数作用：创建画面调整按钮的基础函数
**   参数说明:
***/
lv_obj_t *layout_monitor_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *img_1, rom_bin_info *img_2, int btn_id, bool mix);

/***
**   日期:2022-07-14 17:10:15
**   作者: leo.liu
**   函数作用：创建画面调整进度条的基础函数
**   参数说明:
***/

void layout_display_adj_bar_base_create(lv_obj_t *parent, int x, int w, int y, int max, obj_click_data *left_click, obj_click_data *right_click, int bar_id, int vol, char flg)
{
	/***** bar *****/
	lv_obj_t *bar = lv_bar_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_id(bar, bar_id);
	lv_obj_set_pos(bar, x, y + 22);
	lv_obj_set_size(bar, w, 16);
	lv_bar_set_range(bar, 0, max);
	lv_bar_set_value(bar, vol, false);

	lv_obj_set_style_local_bg_opa(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x666666));
	lv_obj_set_style_local_bg_color(bar, LV_BAR_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));

	/***** btn *****/
	lv_obj_t *left_obj = lv_obj_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_pos(left_obj, x - 60, y);
	lv_obj_set_size(left_obj, 54, 54);
	static rom_bin_info left_img = rom_bin_info_get(ROM_UI_VOL_SUB_PNG);
	lv_obj_set_style_local_pattern_image(left_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &left_img);
	obj_click_event_listen(left_obj, left_click);

	lv_obj_t *right_obj = lv_obj_create(parent ? parent : lv_scr_act(), left_obj);
	lv_obj_set_x(right_obj, x + w);
	static rom_bin_info right_img = rom_bin_info_get(ROM_UI_VOL_ADD_PNG);
	lv_obj_set_style_local_pattern_image(right_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &right_img);
	obj_click_event_listen(right_obj, right_click);

	/***** label *****/
	lv_obj_t *label = lv_label_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_id(label, bar_id + 1);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_label_set_text_fmt(label, "%02d", vol);
	lv_obj_align(label, left_obj, LV_ALIGN_IN_LEFT_MID, -40, 0);
	lv_obj_set_auto_realign(label, true);

	/*****  icon *****/
	lv_obj_t *icon = lv_obj_create(parent ? parent : lv_scr_act(), NULL);
	lv_obj_set_click(icon, false);
	lv_obj_set_size(icon, 54, 54);
	lv_obj_align(icon, left_obj, LV_ALIGN_IN_LEFT_MID, -120, 0);

	if (flg)
	{
		static rom_bin_info vol_img = rom_bin_info_get(ROM_UI_MONITOR_VOL_PNG);
		static rom_bin_info bri_img = rom_bin_info_get(ROM_UI_MONITOR_BRIGHTNESS_PNG);
		static rom_bin_info cont_img = rom_bin_info_get(ROM_UI_MONITOR_CONTRAST_PNG);
		static rom_bin_info color_img = rom_bin_info_get(ROM_UI_MONITOR_COLOR_PNG);
		lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,flg == 0x01 ? &vol_img :  flg == 0x02 ? &bri_img : flg == 0x03 ? &cont_img
																    : &color_img);
	}
}











/***
**   日期:2022-07-14 17:11:01
**   作者: leo.liu
**   函数作用：画面调整的按钮容器创建
**   参数说明:
***/
static lv_obj_t *layout_adj_display_btn_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, adj_disp_scr_act_obj_id_adj_cont);
	lv_obj_set_pos(cont, 105, 382);
	lv_obj_set_size(cont, 814, 183);

	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));

	return cont;
}

/***
** 日期: 2022-05-17 08:06
** 作者: leo.liu
** 函数作用：创建亮度进图条
** 返回参数说明：
***/
static void layout_adj_brightness_bar_left_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), adj_disp_scr_act_obj_id_adj_cont);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_brighness);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_brighness + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol > 0)
		{
			vol--;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_brightness_vol_set(vol);
			display_bright_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:11:18
**   作者: leo.liu
**   函数作用：亮度进度条右边按钮函数处理
**   参数说明:
***/
static void layout_adj_brightness_bar_right_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), adj_disp_scr_act_obj_id_adj_cont);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_brighness);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_brighness + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol < 20)
		{
			vol++;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_brightness_vol_set(vol);
			display_bright_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:11:37
**   作者: leo.liu
**   函数作用: 亮度进度条创建
**   参数说明:
***/
static void layout_adj_brightness_bar_create(lv_obj_t *parent)
{
	static obj_click_data left = obj_click_data_up_create(layout_adj_brightness_bar_left_up);
	static obj_click_data right = obj_click_data_up_create(layout_adj_brightness_bar_right_up);

	layout_display_adj_bar_base_create(parent, 207, 470, 15, 20, &left, &right, adj_cont_obj_id_brighness, monitor_display_brightness_vol_get(), 0x02);
}
/***
** 日期: 2022-05-17 08:06
** 作者: leo.liu
** 函数作用：对比度进度条左侧按钮函数
** 返回参数说明：
***/
static void layout_adj_cont_bar_left_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), adj_disp_scr_act_obj_id_adj_cont);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_const);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_const + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol > 0)
		{
			vol--;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_cont_vol_set(vol);
			display_const_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:12:06
**   作者: leo.liu
**   函数作用：对比度进度条右侧按钮函数
**   参数说明:
***/
static void layout_adj_cont_bar_right_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), adj_disp_scr_act_obj_id_adj_cont);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_const);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_const + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol < 20)
		{
			vol++;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_cont_vol_set(vol);
			display_const_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:13:23
**   作者: leo.liu
**   函数作用：对比度进度条创建
**   参数说明:
***/
static void layout_adj_cont_bar_create(lv_obj_t *parent)
{
	static obj_click_data left = obj_click_data_up_create(layout_adj_cont_bar_left_up);
	static obj_click_data right = obj_click_data_up_create(layout_adj_cont_bar_right_up);
	layout_display_adj_bar_base_create(parent, 207, 470, 71, 20, &left, &right, adj_cont_obj_id_const, monitor_display_cont_vol_get(), 0x03);
}
/***
** 日期: 2022-05-17 08:06
** 作者: leo.liu
** 函数作用：创建色度进图条
** 返回参数说明：
***/
static void layout_adj_color_bar_left_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), adj_disp_scr_act_obj_id_adj_cont);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_color);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_color + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol > 0)
		{
			vol--;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_color_vol_set(vol);
			display_color_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:13:44
**   作者: leo.liu
**   函数作用：色彩进度条右侧按钮函数
**   参数说明:
***/
static void layout_adj_color_bar_right_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), adj_disp_scr_act_obj_id_adj_cont);
	lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_color);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_color + 1);
	if (bar && label)
	{
		int vol = lv_bar_get_value(bar);
		if (vol < 20)
		{
			vol++;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);

			monitor_display_color_vol_set(vol);
			display_color_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:14:01
**   作者: leo.liu
**   函数作用：色彩进度条创建
**   参数说明:
***/
static void layout_adj_color_bar_create(lv_obj_t *parent)
{
	static obj_click_data left = obj_click_data_up_create(layout_adj_color_bar_left_up);
	static obj_click_data right = obj_click_data_up_create(layout_adj_color_bar_right_up);

	layout_display_adj_bar_base_create(parent, 207, 470, 127, 20, &left, &right, adj_cont_obj_id_color, monitor_display_color_vol_get(), 0x04);
}
/***
** 日期: 2022-05-17 09:12
** 作者: leo.liu
** 函数作用：恢复调整
** 返回参数说明：
***/
static void layout_adj_adj_reset_up(lv_obj_t *obj)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), adj_disp_scr_act_obj_id_adj_cont);
	int brightness = monitor_display_brightness_vol_get();
	int cost = monitor_display_cont_vol_get();
	int color = monitor_display_color_vol_get();
	if (brightness != 10)
	{
		lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_brighness);
		lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_brighness + 1);
		if (bar && label)
		{
			int vol = 10;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);
			monitor_display_brightness_vol_set(vol);
			display_bright_adj(vol, INVALID_FORMAT);
		}
	}

	if (cost != 10)
	{
		lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_const);
		lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_const + 1);
		if (bar && label)
		{
			int vol = 10;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);
			monitor_display_cont_vol_set(vol);
			display_const_adj(vol, INVALID_FORMAT);
		}
	}
	if (color != 10)
	{
		lv_obj_t *bar = lv_obj_get_child_form_id(parent, adj_cont_obj_id_color);
		lv_obj_t *label = lv_obj_get_child_form_id(parent, adj_cont_obj_id_color + 1);
		if (bar && label)
		{
			int vol = 10;
			lv_bar_set_value(bar, vol, true);
			lv_label_set_text_fmt(label, "%d", vol);
			monitor_display_color_vol_set(vol);
			display_color_adj(vol, INVALID_FORMAT);
		}
	}
}
/***
**   日期:2022-07-14 17:14:16
**   作者: leo.liu
**   函数作用：画面调整复位按钮创建
**   参数说明:
***/
static void layout_adj_adj_reset_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(layout_adj_adj_reset_up);
	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_ADJ_RESET_PNG);
	layout_monitor_btn_base_create(parent, 926, 11, 48, 48, &click_data, &img1, NULL, 0, false);
}
/***
**   日期:2022-07-14 17:14:31
**   作者: leo.liu
**   函数作用：画面调整返回按钮函数
**   参数说明:
***/
static void layout_adj_cancel_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_display));
}
/***
**   日期:2022-07-14 17:14:44
**   作者: leo.liu
**   函数作用：画面调整返回按钮创建
**   参数说明:
***/
static void layout_adj_cancel_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 10, 5);
	lv_obj_set_size(obj, 60, 60);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(layout_adj_cancel_up);
	obj_click_event_listen(obj, &click_data);
}
/***
** 日期: 2022-04-28 10:40
** 作者: leo.liu
** 函数作用：创建setting的顶部文本
** 返回参数说明：
***/
static bool layout_adj_head_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	if (obj == NULL)
	{
		printf("create head label failed \n");
		return false;
	}

	lv_obj_set_pos(obj, 453, 10);
	lv_obj_set_size(obj, 118, 47);

	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	return true;
}
/***
** 日期: 2022-05-13 10:29
** 作者: leo.liu
** 函数作用：创建顶部显示区域
** 返回参数说明：
***/
static void layout_adj_head_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, adj_disp_scr_act_obj_id_adj_head);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 70);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));

	layout_adj_cancel_create(cont);
	layout_adj_head_label_create(cont);
	layout_adj_adj_reset_create(cont);
}
/***
**   日期:2022-07-14 17:15:06
**   作者: leo.liu
**   函数作用：画面调整页面的第一个函数
**   参数说明:
***/
static void LAYOUT_ENTER_FUNC(adj_display)
{
	layout_adj_head_cont_create();
	lv_obj_t *cont = layout_adj_display_btn_cont_create();
	layout_adj_brightness_bar_create(cont);
	layout_adj_cont_bar_create(cont);
	layout_adj_color_bar_create(cont);
	
	monitor_open(true, 0x03);
}
/***
**   日期:2022-07-14 17:15:24
**   作者: leo.liu
**   函数作用：退出画面调整的最后一个函数
**   参数说明:
***/
static void LAYOUT_QUIT_FUNC(adj_display)
{
	MON_ENTER_FLG flg = monitor_enter_mask_get();
	if (flg != MON_ENTER_CALL)
	{

		LOG_GREEN("set release \n");
		monitor_enter_mask_set(MON_ENTER_NONE);
		intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
	}
	video_display_preview_enable(false);
	video_input_skip_frame_count_set(1000);
	fb_gui_layer_rect_fill(0x0, 0, 0, 1024, 600);
	screen_force_refresh();
	monitor_close();
}
/***
**   日期:2022-07-14 17:15:41
**   作者: leo.liu
**   函数作用：创建画面调整页面
**   参数说明:
***/
CREATE_LAYOUT(adj_display);