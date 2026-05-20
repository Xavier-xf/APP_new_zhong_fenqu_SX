#include "layout_define.h"
#include "layout_setting_common.h"

/***
** 日期: 2022-04-26 17:08
** 作者: leo.liu
** 函数作用：setting btn创建并返回到主界面
** 返回参数说明：
***/
static void setting_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}

/***
** 日期: 2022-04-26 17:09
** 作者: leo.liu
** 函数作用：创建设置页面的返回按钮
** 返回参数说明：
***/
bool setting_cancel_btn_create(void (*obj_click_up_callback)(lv_obj_t *))
{
	lv_obj_t *obj = lv_btn_create(lv_scr_act(), NULL);
	if (obj == NULL)
	{
		printf("create cancel btn failed \n");
		return false;
	}
	lv_obj_set_pos(obj, 20, 0);
	lv_obj_set_size(obj, 80, 80);
	lv_obj_set_ext_click_area(obj,20,180,20,0);

	static rom_bin_info image = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image);
	lv_obj_set_style_local_pattern_align(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	
	// lv_obj_set_style_local_bg_color(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,lv_color_make(0xff, 0xff, 0xff));
	lv_obj_set_style_local_bg_opa(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	static obj_click_data click_data = obj_click_data_up_create(NULL);
	if (obj_click_up_callback == NULL)
	{
		click_data.up = setting_cancel_btn_up;
	}
	else
	{
		click_data.up = obj_click_up_callback;
	}
	obj_click_event_listen(obj, &click_data);

	return true;
}

/***
** 日期: 2022-04-28 10:40
** 作者: leo.liu
** 函数作用：创建setting的顶部文本
** 返回参数说明：
***/
bool setting_head_label_create(const char *string)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	if (obj == NULL)
	{
		printf("create head label failed \n");
		return false;
	}

	lv_obj_set_pos(obj, 453, 23);
	lv_obj_set_size(obj, 118, 47);

	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	return true;
}

/***
** 日期: 2022-04-28 11:22
** 作者: leo.liu
** 函数作用：record btn 点击
** 返回参数说明：
***/
static void setting_record_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_record));
}

/***
** 日期: 2022-04-28 11:23
** 作者: leo.liu
** 函数作用：volume btn点击
** 返回参数说明：
***/
static void setting_volume_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_volume));
}

/***
** 日期: 2022-04-28 11:23
** 作者: leo.liu
** 函数作用：storage btn点击
** 返回参数说明：
***/
static void setting_storage_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_storage));
}

/***
** 日期: 2022-04-28 11:24
** 作者: leo.liu
** 函数作用：data/time btn 点击
** 返回参数说明：
***/
// static void setting_date_btn_up(lv_obj_t *obj)
// {
// 	goto_layout(pLAYOUT(setting_date_time));
// }

/***
** 日期: 2022-04-28 11:24
** 作者: leo.liu
** 函数作用：display btn 点击
** 返回参数说明：
***/
static void setting_display_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_display));
}

/***
** 日期: 2022-04-28 11:25
** 作者: leo.liu
** 函数作用：etc btn 点击
** 返回参数说明：
***/
static void setting_etc_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_etc));
}
/***
**   日期:2022-06-20 17:29:26
**   作者: leo.liu
**   函数作用：wifi
**   参数说明:
***/
static void setting_wifi_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(wifi_display));
} 
/***
** 日期: 2022-04-28 10:59
** 作者: leo.liu
** 函数作用：侧边按钮创建的基础函数
** 返回参数说明：
***/
lv_obj_t *setting_main_btn_base_create(int x, int y, int w, int h, bool enable, const char *string, obj_click_data *data)
{
	lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
	if (btn == NULL)
	{
		printf("create setting main btn failed \n");
		return NULL;
	}
	lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);

	if (string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, string);
		lv_obj_set_style_local_value_font(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_obj_set_style_local_value_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
		lv_obj_set_style_local_value_ofs_x(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 30);
		lv_obj_set_style_local_value_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, enable == true ? (lv_color_make(0xFF, 0xFF, 0xFF)) : (lv_color_make(0xFF, 0xFF, 0xFF)));
	}

	if (enable == false)
	{
		lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
		if (data != NULL)
		{
			obj_click_event_listen(btn, data);
		}
	}
	else
	{
		lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x3A, 0xC3, 0x92));

		lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 10);

		lv_obj_set_click(btn, false);
	}


	return btn;
}




/***
** 日期: 2022-05-05 14:12
** 作者: leo.liu
** 函数作用：设置右边字符串的按钮
** 返回参数说明：
***/
static lv_obj_t *setting_btn_sub_string_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, const char *sub_string, unsigned int sub_obj_id, bool right_icon)
{
	lv_obj_t *btn = lv_btn_create(parent == NULL ? lv_scr_act() : parent, NULL);
	if (btn == NULL)
	{
		printf("create setting right btn failed \n");
		return NULL;
	}
	lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);
	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	if (main_string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, main_string);
		lv_obj_set_style_local_value_font(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_obj_set_style_local_value_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
		lv_obj_set_style_local_value_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));

		lv_obj_set_style_local_value_color(btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x66, 0x66, 0x66));
	}

	if (sub_string != NULL)
	{
		lv_obj_t *sub_obj = lv_obj_create(parent == NULL ? lv_scr_act() : parent, NULL);
		if (sub_obj == NULL)
		{
			printf("create setting sub string fail \n");
			return btn;
		}
		lv_obj_set_pos(sub_obj, x, y);
		lv_obj_set_size(sub_obj, w, h);
		lv_obj_set_click(sub_obj, false);

		lv_obj_set_style_local_value_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
		lv_obj_set_style_local_value_align(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_RIGHT_MID);
		lv_obj_set_style_local_value_ofs_x(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -48);
		lv_obj_set_style_local_value_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x97, 0x97, 0x97));
		lv_obj_set_style_local_value_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x33, 0x33, 0x33));

		lv_obj_set_style_local_value_str(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, sub_string);

		if (sub_obj_id != 0)
		{
			lv_obj_set_id(sub_obj, sub_obj_id);
		}
	}

	if (right_icon)
	{
		static rom_bin_info img = rom_bin_info_get(ROM_UI_02_BTN_BTN_LIST_RIGHT_PNG); 
		lv_obj_set_style_local_pattern_image(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
		lv_obj_set_style_local_pattern_align(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_RIGHT_MID);
	}

	/***** 设置下底边框 *****/
	lv_obj_set_style_local_border_width(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
	lv_obj_set_style_local_border_color(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x32, 0x32, 0x37));
	lv_obj_set_style_local_border_color(btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x22, 0x22, 0x27));
	lv_obj_set_style_local_border_side(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
	return btn;
}

/***
** 日期: 2022-05-05 14:12
** 作者: leo.liu
** 函数作用：设置左边为checkbox图标
** 返回参数说明：
***/
static lv_obj_t *setting_btn_sub_checkbox_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, bool focus, char sub_type_show)
{
	lv_obj_t *btn = lv_checkbox_create(parent == NULL ? lv_scr_act() : parent, NULL);
	if (btn == NULL)
	{
		printf("create setting right btn failed \n");
		return NULL;
	}
	lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);

	lv_obj_set_ext_click_area(btn, 0, w - 38, (h - 38) / 2, (h - 38) / 2);
	lv_obj_set_style_local_bg_opa(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	if (sub_type_show == 2)
	{
		static rom_bin_info image_off_d = rom_bin_info_get(ROM_UI_CHECK_BTN_001_PNG);
		static rom_bin_info image_on_d = rom_bin_info_get(ROM_UI_CHECK_BTN_002_PNG);
		lv_obj_set_style_local_pattern_image(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, &image_off_d);
		lv_obj_set_style_local_pattern_image(btn, LV_CHECKBOX_PART_BG, LV_STATE_CHECKED, &image_on_d);
	}
	else if (sub_type_show == 4)
	{
		static rom_bin_info image_off_d = rom_bin_info_get(ROM_UI_CHECK_BTN_003_PNG);
		static rom_bin_info image_on_d = rom_bin_info_get(ROM_UI_CHECK_BTN_004_PNG);
		lv_obj_set_style_local_pattern_image(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, &image_off_d);
		lv_obj_set_style_local_pattern_image(btn, LV_CHECKBOX_PART_BG, LV_STATE_CHECKED, &image_on_d);
	}

	lv_obj_set_style_local_pattern_align(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);

	lv_checkbox_set_text(btn, "");
	if (main_string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, main_string);
		lv_obj_set_style_local_value_font(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_obj_set_style_local_value_align(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
		lv_obj_set_style_local_value_color(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
		lv_obj_set_style_local_value_color(btn, LV_CHECKBOX_PART_BG, LV_STATE_PRESSED, lv_color_make(0x66, 0x66, 0x66));
		lv_obj_set_style_local_value_ofs_x(btn, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, 70);
	}

	lv_checkbox_set_checked(btn, focus);

	lv_obj_t *obj = lv_obj_create(parent == NULL ? lv_scr_act() : parent, NULL);
	lv_obj_set_pos(obj, x, y + h - 2);
	lv_obj_set_size(obj, w, 2);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x32, 0x32, 0x37));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x22, 0x22, 0x27));
	lv_obj_set_click(obj, false);
	return btn;
}

/***
** 日期: 2022-05-05 14:31
** 作者: leo.liu
** 函数作用：创建开关的按钮
** 返回参数说明：
***/
static lv_obj_t *setting_btn_sub_switch_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, bool focus)
{
	lv_obj_t *btn = lv_switch_create(parent == NULL ? lv_scr_act() : parent, NULL);
	if (btn == NULL)
	{
		printf("create setting right btn failed \n");
		return NULL;
	}
	lv_obj_set_pos(btn, x + w - 95, y + (h - 34) / 2);
	lv_obj_set_size(btn, 75, 36);

	lv_obj_set_ext_click_area(btn, w - 95, 0, (h - 54) / 2, (h - 54) / 2);
	lv_obj_set_style_local_bg_opa(btn, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);

	lv_obj_set_style_local_bg_color(btn, LV_SWITCH_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0x808080));
	lv_obj_set_style_local_bg_color(btn, LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, lv_color_hex(0x3AC392));

	if (main_string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, main_string);
		lv_obj_set_style_local_value_font(btn, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_obj_set_style_local_value_align(btn, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
		lv_obj_set_style_local_value_ofs_x(btn, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, -w + 95);
		lv_obj_set_style_local_value_color(btn, LV_SWITCH_PART_BG, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
		lv_obj_set_style_local_value_color(btn, LV_SWITCH_PART_BG, LV_STATE_PRESSED, lv_color_make(0x66, 0x66, 0x66));
	}
	if (focus)
	{
		lv_switch_on(btn, false);
	}
	else
	{
		lv_switch_off(btn, false);
	}

	lv_obj_t *obj = lv_obj_create(parent == NULL ? lv_scr_act() : parent, NULL);
	lv_obj_set_pos(obj, x, y + h - 2);
	lv_obj_set_size(obj, w, 2);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x32, 0x32, 0x37));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x3A, 0xC3, 0x92));
	lv_obj_set_click(obj, false);
	return btn;
}

/***
** 日期: 2022-05-05 13:53
** 作者: leo.liu
** 函数作用：setting btn 基础函数
** 返回参数说明：
    sub_type_show:
    0,不显示，
    1:显示字符串，并且根据click_data显示right icon
    2:显示checkbox(单选)
    3:显示switch
    4:多选
***/
static lv_obj_t *setting_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, obj_click_data *click_data,
					 char sub_type_show, unsigned int sub_obj_id, const char *sub_string, bool fouces)
{
	lv_obj_t *btn = NULL;
	if (sub_type_show == 1)
	{
		btn = setting_btn_sub_string_create(parent, x, y, w, h, main_string, sub_string, sub_obj_id, click_data ? true : false);
	}
	else if ((sub_type_show == 2) || (sub_type_show == 4))
	{
		btn = setting_btn_sub_checkbox_create(parent, x, y, w, h, main_string, fouces, sub_type_show);
	}
	else if (sub_type_show == 3)
	{
		btn = setting_btn_sub_switch_create(parent, x, y, w, h, main_string, fouces);
	}

	if (btn == NULL)
	{
		printf("create setting_btn_base_create failed \n");
		return NULL;
	}

	if (click_data != NULL)
	{
		obj_click_event_listen(btn, click_data);
	}
	else
	{
		lv_obj_set_click(btn, false);
	}

	return btn;
}

/***
** 日期: 2022-04-28 11:59
** 作者: leo.liu
** 函数作用：创建右边按钮基础函数
** 返回参数说明：
***/
lv_obj_t *setting_right_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, const char *sub_string, obj_click_data *click_data, unsigned int sub_obj_id)
{
	return setting_btn_base_create(parent, x, y, w, h, main_string, click_data, 1, sub_obj_id, sub_string, 0);
}

/***
** 日期: 2022-04-28 10:55
** 作者: leo.liu
** 函数作用：设置侧边按钮创建
** 返回参数说明:select_id:设置当前页面：
***/
bool setting_main_btn_create(int page_id)
{
	if(OwnID == 1)
	{
		/***** 创建 record按钮 *****/
		static obj_click_data record_click_data = obj_click_data_up_create(setting_record_btn_up);
		setting_main_btn_base_create(10, 70, 280, 75, (page_id == 0) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_RECORD_WIFI), &record_click_data);
		/***** 创建 wifi按钮 *****/
		static obj_click_data wifi_click_data = obj_click_data_up_create(setting_wifi_btn_up);
		setting_main_btn_base_create(10, 70 + 75, 280, 75, (page_id == 6) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_WIFI), &wifi_click_data);
		/***** 创建volume *****/
		static obj_click_data volume_click_data = obj_click_data_up_create(setting_volume_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 2, 280, 75, (page_id == 1) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_VOLUME), &volume_click_data);
		/***** 创建storage *****/
		static obj_click_data storage_click_data = obj_click_data_up_create(setting_storage_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 3, 280, 75, (page_id == 2) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_STORAGE), &storage_click_data);
		/***** 创建date *****/
		// static obj_click_data date_click_data = obj_click_data_up_create(setting_date_btn_up);
		// setting_main_btn_base_create(10, 70 + 75 * 4, 280, 75, (page_id == 3) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DATE), &date_click_data);
		/***** 创建display *****/
		static obj_click_data display_click_data = obj_click_data_up_create(setting_display_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 4, 280, 75, (page_id == 4) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DISPLAY), &display_click_data);
		/***** 创建etc *****/
		static obj_click_data etc_click_data = obj_click_data_up_create(setting_etc_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 5, 280, 75, (page_id == 5) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_ETC), &etc_click_data);
	}
	else
	{
		/***** 创建 record按钮 *****/
		static obj_click_data record_click_data = obj_click_data_up_create(setting_record_btn_up);
		setting_main_btn_base_create(10, 70, 280, 75, (page_id == 0) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_RECORD_WIFI), &record_click_data);
		// /***** 创建 wifi按钮 *****/
		// static obj_click_data wifi_click_data = obj_click_data_up_create(setting_wifi_btn_up);
		// setting_main_btn_base_create(10, 70 + 75, 280, 75, (page_id == 6) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_WIFI), &wifi_click_data);
		/***** 创建volume *****/
		static obj_click_data volume_click_data = obj_click_data_up_create(setting_volume_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 1, 280, 75, (page_id == 1) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_VOLUME), &volume_click_data);
		/***** 创建storage *****/
		static obj_click_data storage_click_data = obj_click_data_up_create(setting_storage_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 2, 280, 75, (page_id == 2) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_STORAGE), &storage_click_data);
		/***** 创建date *****/
		// static obj_click_data date_click_data = obj_click_data_up_create(setting_date_btn_up);
		// setting_main_btn_base_create(10, 70 + 75 * 3, 280, 75, (page_id == 3) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DATE), &date_click_data);
		/***** 创建display *****/
		static obj_click_data display_click_data = obj_click_data_up_create(setting_display_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 3, 280, 75, (page_id == 4) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_DISPLAY), &display_click_data);
		/***** 创建etc *****/
		static obj_click_data etc_click_data = obj_click_data_up_create(setting_etc_btn_up);
		setting_main_btn_base_create(10, 70 + 75 * 4, 280, 75, (page_id == 5) ? true : false, layout_setting_record_string_get(SETTING_RECORD_LANG_ID_ETC), &etc_click_data);
	}
	return true;
}
/***
** 日期: 2022-04-29 15:56
** 作者: leo.liu
** 函数作用：消息框的背景创建
** 返回参数说明：
***/
lv_obj_t *setting_msgdialog_msg_bg_create(int id)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	if (cont == NULL)
	{
		printf("create auto record cont failed \n");
		return NULL;
	}
	if (id > 0)
	{
		lv_obj_set_id(cont, id + 1);
	}
	lv_obj_set_click(cont, true);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	// lv_obj_set_pos(cont, 352, 224);
	// lv_obj_set_size(cont, 576, 352);
	cont = lv_cont_create(lv_scr_act(), NULL);
	if (id > 0)
	{
		lv_obj_set_id(cont, id);
	}
	lv_obj_set_pos(cont, 225, 124);
	lv_obj_set_size(cont, 576, 352);
	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x090909));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	return cont;
}


/***
** 日期: 2022-04-29 15:56
** 作者: leo.liu
** 函数作用：消息框的背景创建
** 返回参数说明：
***/
// lv_obj_t *setting_msgdialog_msg_bg_create(int id)
// {
// 	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
// 	if (cont == NULL)
// 	{
// 		printf("create auto record cont failed \n");
// 		return NULL;
// 	}
// 	if (id > 0)
// 	{
// 		lv_obj_set_id(cont, id + 1);
// 	}
// 	// lv_obj_set_click(cont, false);
// 	lv_obj_set_pos(cont, 0, 0);
// 	lv_obj_set_size(cont, LV_HOR_RES_MAX, LV_VER_RES_MAX);
// 	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
// 	// lv_obj_set_pos(cont, 352, 224);
// 	// lv_obj_set_size(cont, 576, 352);
// 	cont = lv_cont_create(lv_scr_act(), NULL);
// 	if (id > 0)
// 	{
// 		lv_obj_set_id(cont, id);
// 	}
// 	lv_obj_set_pos(cont, 225, 124);
// 	lv_obj_set_size(cont, 564, 353);
// 	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(24, 25, 26));
// 	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	
// 	return cont;
// }


lv_obj_t *setting_mecurity_msgdialog_msg_bg_create(int id)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	if (cont == NULL)
	{
		printf("create auto record cont failed \n");
		return NULL;
	}
	if (id > 0)
	{
		lv_obj_set_id(cont, id + 1);
	}
	// lv_obj_set_click(cont, false);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	// lv_obj_set_pos(cont, 352, 224);
	// lv_obj_set_size(cont, 576, 352);
	cont = lv_cont_create(lv_scr_act(), NULL);
	if (id > 0)
	{
		lv_obj_set_id(cont, id);
	}
	lv_obj_set_pos(cont, 190, 100);
	lv_obj_set_size(cont, 610, 65);
	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(38, 38, 38));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_radius(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 52);
	return cont;
}
/***
**   日期:2022-07-04 17:00:03
**   作者: leo.liu
**   函数作用：销毁消息框
**   参数说明:
***/
bool setting_msgdialog_msg_bg_delete(int id)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), id);
	if (dialog == NULL)
	{
		printf("find obj id (%d )failed \n", id);
		return false;
	}
	lv_obj_del(dialog);

	dialog = lv_obj_get_child_form_id(lv_scr_act(), id + 1);
	if (dialog == NULL)
	{
		printf("find obj id (%d )failed \n", id + 1);
		return false;
	}
	lv_obj_del(dialog);
	return true;
}


/***
** 日期: 2022-04-28 16:26
** 作者: leo.liu
** 函数作用：创建消息框的关闭和确认函数创建基础函数
** 返回参数说明：
***/
static bool setting_msgdialog_confirm_cancel_btn_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *image, lv_color_t bg_color, lv_color_t press_color)
{
	lv_obj_t *obj = lv_btn_create(parent, NULL);
	if (obj == NULL)
	{
		printf("create msgdialog btn failed \n");
		return false;
	}

	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);

	lv_obj_set_style_local_bg_color(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, bg_color);
	lv_obj_set_style_local_bg_color(obj, LV_BTN_PART_MAIN, LV_STATE_PRESSED, press_color);

	if (image != NULL)
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, image);
		lv_obj_set_style_local_pattern_align(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	}

	if (click_data != NULL)
	{
		obj_click_event_listen(obj, click_data);
	}
	
	return true;
}


/***
** 日期: 2022-04-28 16:26
** 作者: leo.liu
** 函数作用：安防界面创建消息框的关闭和确认函数创建基础函数
** 返回参数说明：
***/
static bool setting_security_msgdialog_confirm_cancel_btn_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *image, lv_color_t press_color)
{
	lv_obj_t *obj = lv_btn_create(parent, NULL);
	if (obj == NULL)
	{
		printf("create msgdialog btn failed \n");
		return false;
	}

	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);

	if (image != NULL)
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, image);
		lv_obj_set_style_local_pattern_align(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	}

	if (click_data != NULL)
	{
		obj_click_event_listen(obj, click_data);
	}
	lv_obj_set_style_local_bg_opa(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT,LV_OPA_0);
	
	return true;
}
/***
** 日期: 2022-04-28 17:20
** 作者: leo.liu
** 函数作用：消息框按键矩阵按下回调函数
** 返回参数说明：
***/
static void setting_msgdialog_btnmatrix_up(lv_obj_t *obj)
{
	uint16_t btn_id = lv_btnmatrix_get_active_btn(obj);
	if (btn_id == LV_BTNMATRIX_BTN_NONE)
		return;

	if (lv_btnmatrix_get_btn_ctrl(obj, btn_id, LV_BTNMATRIX_CTRL_DISABLED))
		return;

	lv_btnmatrix_clear_btn_ctrl_all(obj, LV_BTNMATRIX_CTRL_CHECK_STATE);
	lv_btnmatrix_set_btn_ctrl(obj, btn_id, LV_BTNMATRIX_CTRL_CHECK_STATE);
}
/***
 ** 日期: 2022-04-28 15:32
 ** 作者: leo.liu
 ** 函数作用：创建按键矩阵
 ** 返回参数说明：
 ***/
lv_obj_t *setting_msgdialog_btnmatrix_create(lv_obj_t *prarent, int obj_id, const char *string_map[])
{
	lv_obj_t *btnmatrix = lv_btnmatrix_create(prarent, NULL);
	if (btnmatrix == NULL)
	{
		printf("auto recrod create btnmatrix failed \n");
		return NULL;
	}
	lv_obj_set_id(btnmatrix, obj_id);

	lv_obj_set_pos(btnmatrix, 66, 0);
	lv_obj_set_size(btnmatrix, 576, 274);
	static rom_bin_info image_off = rom_bin_info_get(ROM_UI_CHECK_BTN_001_PNG);
	lv_obj_set_style_local_pattern_image(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, &image_off);
	lv_obj_set_style_local_pattern_align(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);

	static rom_bin_info image_on = rom_bin_info_get(ROM_UI_CHECK_BTN_002_PNG);
	lv_obj_set_style_local_pattern_image(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_CHECKED, &image_on);
	lv_obj_set_style_local_pattern_align(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_CHECKED, LV_ALIGN_IN_LEFT_MID);

	lv_btnmatrix_set_map(btnmatrix, string_map);
	lv_btnmatrix_set_align(btnmatrix, LV_LABEL_ALIGN_LEFT, 58, 0);

	static obj_click_data click_data = obj_click_data_up_create(setting_msgdialog_btnmatrix_up);
	obj_click_event_listen(btnmatrix, &click_data);

	lv_btnmatrix_clear_btn_ctrl_all(btnmatrix, LV_BTNMATRIX_CTRL_CHECK_STATE);


	return btnmatrix;
}

/***
** 日期: 2022-05-04 09:10
** 作者: leo.liu
** 函数作用：消息框的确认和取消按钮创建
** 返回参数说明：
***/
void setting_msgdialog_msg_confirm_and_cancel_btn_create(lv_obj_t *parent, void (*cancel_func)(lv_obj_t *obj), void (*confirm_func)(lv_obj_t *obj))
{
	static rom_bin_info image_cancel = rom_bin_info_get(ROM_UI_02_BTN_BTN_BOTTOM_CANCEL_PNG);
	static obj_click_data click_data_cancel = {NULL, NULL, NULL};
	click_data_cancel.up = cancel_func;
	setting_msgdialog_confirm_cancel_btn_create(parent, 0, 274, 288, 78, &click_data_cancel, &image_cancel, lv_color_make(0x47, 0x49, 0x4a), lv_color_make(0x40, 0x40, 0x40));

	static rom_bin_info image_confirm = rom_bin_info_get(ROM_UI_02_BTN_BTN_BOTTOM_CONFIRM_PNG);
	static obj_click_data click_data_confirm = {NULL, NULL, NULL};
	click_data_confirm.up = confirm_func;
	setting_msgdialog_confirm_cancel_btn_create(parent, 288, 274, 288, 78, &click_data_confirm, &image_confirm, lv_color_make(0x3A, 0xC3, 0x92), lv_color_make(0x00, 0x76, 0xCF));
}
/***
** 日期: 2022-05-04 09:10
** 作者: leo.liu
** 函数作用：消息框的确认按钮创建
** 返回参数说明：
***/
void setting_msgdialog_msg_confirm_btn_create(lv_obj_t *parent, void (*confirm_func)(lv_obj_t *obj))
{
	static rom_bin_info image_confirm = rom_bin_info_get(ROM_UI_02_BTN_BTN_BOTTOM_CONFIRM_PNG);
	static obj_click_data click_data_confirm = {NULL, NULL, NULL};
	click_data_confirm.up = confirm_func;
	setting_msgdialog_confirm_cancel_btn_create(parent, 0, 274, 576, 78, &click_data_confirm, &image_confirm, lv_color_make(0x3A, 0xC3, 0x92), lv_color_make(0x1C, 0x1C, 0x1C));
}

void setting_security_msgdialog_msg_confirm_btn_create(lv_obj_t *parent, void (*confirm_func)(lv_obj_t *obj))
{
	static rom_bin_info image_confirm = rom_bin_info_get(ROM_UI_SECURITY_CONFIRM_PNG);
	static obj_click_data click_data_confirm = {NULL, NULL, NULL};
	click_data_confirm.up = confirm_func;
	setting_security_msgdialog_confirm_cancel_btn_create(parent, 550, 5, 56, 56, &click_data_confirm, &image_confirm, lv_color_make(0x3A, 0xC3, 0x92));
}
/***
** 日期: 2022-05-04 09:18
** 作者: leo.liu
** 函数作用：消息框的消息内容显示控件创建
** 返回参数说明：
***/
void setting_msgdialog_msg_create(lv_obj_t *parent, const char *msg_string)
{
	lv_obj_set_style_local_value_str(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, msg_string);
	lv_obj_set_style_local_value_align(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_value_ofs_y(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_value_font(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	lv_obj_set_style_local_value_color(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT,lv_color_hex(0xFF0000));
}

void setting_msgdialog_video_msg_create(lv_obj_t *parent, const char *msg_string)
{
	lv_obj_set_style_local_value_str(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, msg_string);
	lv_obj_set_style_local_value_align(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_value_ofs_y(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_value_font(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	lv_obj_set_style_local_value_color(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT,lv_color_hex(0xffffff));
}

/***FF0000
** 日期: 2022-04-28 11:59
** 作者: leo.liu
** 函数作用：创建设置图标
** 返回参数说明：focus
    0,不显示，
    1:显示字符串，并且根据click_data显示right icon
    2:显示checkbox
    3:显示switch
    4:多选框
***/
lv_obj_t *setting_sub_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, obj_click_data *click_data, bool focus, int type)
{
	return setting_btn_base_create(parent, x, y, w, h, main_string, click_data, type, 0, NULL, focus);
}

static void setting_time_rooler_base_change(lv_obj_t *obj, lv_event_t ev)
{
	if (ev == LV_EVENT_VALUE_CHANGED)
	{
		if (obj->user_data != NULL)
		{
			bool *pmodiy = (bool *)(obj->user_data);
			if ((*pmodiy) == false)
			{
				*pmodiy = true;
			}
		}
	}
}
/***
** 日期: 2022-05-06 10:22
** 作者: leo.liu
** 函数作用：滚动基础函数
** 返回参数说明：
***/
lv_obj_t *setting_time_rooler_base(lv_obj_t *parent, int x, int y, int w, int h, int min, int max, int cur, int obj_id)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_click(obj, false);
	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);

	static rom_bin_info img = rom_bin_info_get(ROM_UI_TIME_SIGN_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);

	lv_obj_t *rooler = lv_roller_create(parent, NULL);
	lv_obj_set_id(rooler, obj_id);
	lv_obj_set_style_local_text_line_space(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 30);
	//   lv_obj_set_style_local_text_color(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_make(0x80, 0x80, 0x80));

	// lv_obj_set_style_local_bg_opa(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_opa(rooler, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	// lv_obj_set_style_local_bg_color(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_make(0xCC, 0x33, 0x33));
	//  lv_obj_set_style_local_bg_color(rooler, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_make(0x80, 0x80, 0x80));
	//  lv_obj_set_style_local_text_color(rooler, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_make(0x00, 0x96, 0xFF));

	char opt[512] = {0};
	for (int i = max; i >= min; i--)
	{
		char buf[8] = {0};
		sprintf(buf, "%02d%s", i, i == min ? "" : "\n");
		strcat(opt, buf);
	}

	lv_roller_set_options(rooler, opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(rooler, 1);
	lv_obj_set_pos(rooler, x, y + 60);
	lv_obj_set_width(rooler, w);

	lv_obj_set_ext_click_area(rooler, 0, 0, 54, 54);

	lv_roller_set_selected(rooler, max - cur, false);
	static obj_click_data click_data = obj_click_data_anything_create(setting_time_rooler_base_change);
	rooler->user_data = parent->user_data;
	obj_click_event_listen(rooler, &click_data);

	return rooler;
}

lv_obj_t *setting_time_rooler_big_base(lv_obj_t *parent, int x, int y, int w, int h, int min, int max, int cur, int obj_id)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_click(obj, false);
	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);

	static rom_bin_info img = rom_bin_info_get(ROM_UI_TIME_SIGN_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	lv_obj_set_style_local_pattern_align(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);

	lv_obj_t *rooler = lv_roller_create(parent, NULL);
	lv_obj_set_id(rooler, obj_id);
	lv_obj_set_style_local_text_line_space(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 30);
	//   lv_obj_set_style_local_text_color(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_make(0x80, 0x80, 0x80));

	// lv_obj_set_style_local_bg_opa(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_opa(rooler, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	// lv_obj_set_style_local_bg_color(rooler, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, lv_color_make(0xCC, 0x33, 0x33));
	//  lv_obj_set_style_local_bg_color(rooler, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_make(0x80, 0x80, 0x80));
	//  lv_obj_set_style_local_text_color(rooler, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_make(0x00, 0x96, 0xFF));

	char opt[512] = {0};
	for (int i = max; i >= min; i--)
	{
		char buf[8] = {0};
		sprintf(buf, "%02d%s", i, i == min ? "" : "\n");
		strcat(opt, buf);
	}

	lv_roller_set_options(rooler, opt, LV_ROLLER_MODE_INFINITE);
	lv_roller_set_visible_row_count(rooler, 1);
	lv_obj_set_pos(rooler, x, y + 60);
	lv_obj_set_width(rooler, w);

	lv_obj_set_ext_click_area(rooler, 0, 0, 80, 80);

	lv_roller_set_selected(rooler, max - cur, false);
	static obj_click_data click_data = obj_click_data_anything_create(setting_time_rooler_base_change);
	rooler->user_data = parent->user_data;
	obj_click_event_listen(rooler, &click_data);

	return rooler;
}
/***
** 日期: 2022-05-07 13:52
** 作者: leo.liu
** 函数作用：数字密码输入键盘
** 返回参数说明：
***/
lv_obj_t *setting_passowrd_num_keyboard_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data)
{
	lv_obj_t *keyboard = lv_keyboard_create(parent == NULL ? lv_scr_act() : parent, NULL);
	if (keyboard == NULL)
	{
		printf("create keyboard failed \n");
		return NULL;
	}
	lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUM);
	lv_obj_set_pos(keyboard, x, y);
	lv_obj_set_size(keyboard, w, h);

	lv_obj_set_style_local_text_font(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, FONT_SIZE(40));
	
	lv_obj_set_style_local_border_opa(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, LV_OPA_10);
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED, lv_color_hex(0x00CC00));
	

	lv_obj_set_style_local_radius(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, 60);

	lv_obj_set_style_local_pad_inner(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 24);

	static rom_bin_info img = rom_bin_info_get(ROM_UI_BTN_EXIT_PNG);
	lv_btnmatrix_set_pattern_image(keyboard, 10, &img);

	obj_click_event_listen(keyboard, click_data);

	return keyboard;
}

/***
** 日期: 2022-05-09 10:53
** 作者: leo.liu
** 函数作用：设置下直线的选中线条
** 返回参数说明：
***/
static void setting_password_line_set_cheked(lv_obj_t *parent, int cur_id, int max_edit)
{
	for (int i = 0; i < max_edit; i++)
	{
		int id = 30 + i;
		lv_obj_t *obj = lv_obj_get_child_form_id(parent, id);
		lv_obj_set_state(obj, id == cur_id ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
	}
}
/***
** 日期: 2022-05-09 10:34
** 作者: leo.liu
** 函数作用：延时隐藏密码
** 返回参数说明：
***/
static void setting_password_input_string_task(lv_task_t *task_t)
{
	lv_obj_t *edit_label = (lv_obj_t *)(task_t->user_data);
	if (edit_label == NULL)
	{
		return;
	}

	int edit_index = *((int *)edit_label->user_data);
	int cur_index = (edit_label->obj_id) % 10;
	if (cur_index >= edit_index)
	{
		return;
	}

	lv_obj_set_hidden(edit_label, true);

	int id = 20 + cur_index;
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_obj_get_parent(edit_label), id);
	if (obj == NULL)
	{
		return;
	}
	lv_obj_set_hidden(obj, false);

	lv_task_del(task_t);
}

/***
** 日期: 2022-05-09 10:26
** 作者: leo.liu
** 函数作用：输入编辑的数值
** 返回参数说明：
***/
bool setting_password_input_string(lv_obj_t *parent, const char *string, int max_edit)
{
	lv_obj_t *label = lv_obj_get_child_form_id(parent, 11);
	if (label == NULL)
	{
		return false;
	}

	int edit_index = *((int *)label->user_data);
	if (edit_index >= max_edit)
	{
		return false;
	}
	int id = 10 + edit_index;
	lv_obj_t *edit_label = lv_obj_get_child_form_id(parent, id);
	if (edit_label == NULL)
	{
		printf("dind id:%d failed \n", id);
		return false;
	}

	setting_password_line_set_cheked(parent, 30 + edit_index, max_edit);

	lv_obj_set_hidden(edit_label, false);
	lv_label_set_text(edit_label, string);
	(*((int *)label->user_data))++;

	lv_layout_task_create(setting_password_input_string_task, 500, LV_TASK_PRIO_MID, edit_label);
	return true;
}

/***
** 日期: 2022-05-09 10:57
** 作者: leo.liu
** 函数作用：删除一个字符
** 返回参数说明：
***/
bool setting_password_del_string(lv_obj_t *parent, int max_edit)
{
	lv_obj_t *label = lv_obj_get_child_form_id(parent, 11);
	if (label == NULL)
	{
		return false;
	}

	int edit_index = *((int *)label->user_data);
	if (edit_index == 0)
	{
		return false;
	}
	edit_index -= 1;

	int id = 10 + edit_index;
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, id);
	if (obj == NULL)
	{
		printf("dind id:%d failed \n", id);
		return false;
	}
	lv_obj_set_hidden(obj, true);

	id = 20 + edit_index;
	obj = lv_obj_get_child_form_id(parent, id);
	if (obj == NULL)
	{
		return false;
	}
	lv_obj_set_hidden(obj, true);

	setting_password_line_set_cheked(parent, 30 + edit_index - 1, max_edit);

	(*((int *)label->user_data))--;
	return true;
}

/***
** 日期: 2022-05-09 11:22
** 作者: leo.liu
** 函数作用：获取输入字符串
** 返回参数说明：
***/
bool setting_password_get_string(lv_obj_t *parent, char *buffer)
{
	lv_obj_t *label = lv_obj_get_child_form_id(parent, 11);
	if (label == NULL)
	{
		return false;
	}
	int edit_index = *((int *)label->user_data);
	if (edit_index == 0)
	{
		return false;
	}

	for (int i = 0; i < edit_index; i++)
	{
		label = lv_obj_get_child_form_id(parent, 10 + i);
		if (label == NULL)
		{
			printf("find obj failed %d\n", i);
		}
		strcat(buffer, lv_label_get_text(label));    //连接bufer和label（密码）
	}

	return true;
}

/***
** 日期: 2022-05-09 11:27
** 作者: leo.liu
** 函数作用：获取当前输入的索引
** 返回参数说明：
***/
int setting_password_edit_index_get(lv_obj_t *parent)
{
	lv_obj_t *label = lv_obj_get_child_form_id(parent, 11);
	if (label == NULL)
	{
		return false;
	}
	int edit_index = *((int *)label->user_data);
	return edit_index;
}

/***
** 日期: 2022-05-09 11:31
** 作者: leo.liu
** 函数作用：复位输入框
** 返回参数说明：
***/
bool setting_password_input_reset(lv_obj_t *parent, int edit_max)
{
	lv_obj_t *label = lv_obj_get_child_form_id(parent, 11);
	if (label == NULL)
	{
		return false;
	}
	(*((int *)label->user_data)) = 0;

	for (int i = 0; i < edit_max; i++)
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(parent, 10 + i);
		if (obj != NULL)
		{
			lv_obj_set_hidden(obj, true);
		}
		obj = lv_obj_get_child_form_id(parent, 20 + i);
		if (obj != NULL)
		{
			lv_obj_set_hidden(obj, true);
		}

		obj = lv_obj_get_child_form_id(parent, 30 + i);
		if (obj != NULL)
		{
			lv_obj_set_state(obj, LV_STATE_DEFAULT);
		}
	}
	return true;
}

/***
** 日期: 2022-05-07 16:58
** 作者: leo.liu
** 函数作用：设置密码输入框
** 返回参数说明
** 3x4个控件
** LABEL ：显示文本
** OBJ ：显示圆点
** OBJ :显示下划线
** 要获取控件:可以通过矩阵id获取，如第一个lable:11
***/
bool setting_password_input_label_create(lv_obj_t *parent, int x, int y, int row)
{
	static int edit_index = 0;
	edit_index = 0;
	/***** 行号减1 *****/
	if (row < 1)
	{
		return false;
	}

	for (int i = 0; i < row; i++)
	{
		lv_obj_t *label = lv_label_create(parent == NULL ? lv_scr_act() : parent, NULL);
		lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
		lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
		lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_FONT_SIZE_31);
		lv_label_set_text(label, "");
		lv_obj_set_size(label, 48, 48);
		label->user_data = &edit_index;

		lv_obj_set_pos(label, x, y + 10 + i * 127);
		lv_obj_set_id(label, 10 + i * 4);

		label = lv_label_create(parent == NULL ? lv_scr_act() : parent, label);
		lv_obj_set_x(label, x + (48 + 19) * 1);
		lv_obj_set_id(label, 11 + i * 4);

		label = lv_label_create(parent == NULL ? lv_scr_act() : parent, label);
		lv_obj_set_x(label, x + (48 + 19) * 2);
		lv_obj_set_id(label, 12 + i * 4);

		label = lv_label_create(parent == NULL ? lv_scr_act() : parent, label);
		lv_obj_set_x(label, x + (48 + 19) * 3);
		lv_obj_set_id(label, 13 + i * 4);

		lv_obj_t *rot = lv_obj_create(parent == NULL ? lv_scr_act() : parent, NULL);
		rot->user_data = &edit_index;
		lv_obj_set_size(rot, 16, 16);
		lv_obj_set_hidden(rot, true);

		lv_obj_set_style_local_bg_opa(rot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_bg_color(rot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
		lv_obj_set_style_local_radius(rot, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 60);

		lv_obj_set_pos(rot, x + 17, y + 21 + i * 127);
		lv_obj_set_id(rot, 20 + i * 4);

		rot = lv_obj_create(parent == NULL ? lv_scr_act() : parent, rot);
		lv_obj_set_x(rot, x + 17 + (48 + 19) * 1);
		lv_obj_set_id(rot, 21 + i * 4);

		rot = lv_obj_create(parent == NULL ? lv_scr_act() : parent, rot);
		lv_obj_set_x(rot, x + 17 + (48 + 19) * 2);
		lv_obj_set_id(rot, 22 + i * 4);

		rot = lv_obj_create(parent == NULL ? lv_scr_act() : parent, rot);
		lv_obj_set_x(rot, x + 17 + (48 + 19) * 3);
		lv_obj_set_id(rot, 23 + i * 4);

		lv_obj_t *line = lv_obj_create(parent == NULL ? lv_scr_act() : parent, NULL);
		line->user_data = &edit_index;
		lv_obj_set_size(line, 48, 2);
		lv_obj_set_style_local_bg_opa(line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_bg_color(line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
		lv_obj_set_style_local_bg_color(line, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, lv_color_hex(0x0096ff));
		lv_obj_set_style_local_radius(line, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 60);
		lv_obj_set_id(line, 30 + i * 4);
		lv_obj_set_pos(line, x, y + 55 + i * 127);

		line = lv_obj_create(parent == NULL ? lv_scr_act() : parent, line);
		lv_obj_set_x(line, x + (48 + 19) * 1);
		lv_obj_set_id(line, 31 + i * 4);

		line = lv_obj_create(parent == NULL ? lv_scr_act() : parent, line);
		lv_obj_set_x(line, x + (48 + 19) * 2);
		lv_obj_set_id(line, 32 + i * 4);

		line = lv_obj_create(parent == NULL ? lv_scr_act() : parent, line);
		lv_obj_set_x(line, x + (48 + 19) * 3);
		lv_obj_set_id(line, 33 + i * 4);
	}
	return true;
}
