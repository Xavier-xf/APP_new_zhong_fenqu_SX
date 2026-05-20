#include "layout_define.h"

#define LAYOUT_SETTING_FRAME_OBJ_ID_LCD_OFF 0X01
#define LAYOUT_SETTING_FRAME_OBJ_ID_OPEN_FRAME 0X02
#define LAYOUT_SETTING_FRAME_OBJ_ID_CONT 0X03



static lv_obj_t *setting_btn_sub_string_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, const char *sub_string, unsigned int sub_obj_id, bool right_icon, obj_click_data *click_data)
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

	if (click_data != NULL)
	{
		obj_click_event_listen(btn, click_data);
	}
	if (main_string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, main_string);
		lv_obj_set_style_local_value_font(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_obj_set_style_local_value_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
		lv_obj_set_style_local_value_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));

		lv_obj_set_style_local_value_color(btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x66, 0x66, 0x66));
		if(user_data_get()->display.frame_background == 2)
		{
			lv_obj_set_style_local_value_color(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x38, 0x38, 0x38));
		}
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
		if(user_data_get()->display.frame_background == 2)
		{
			lv_obj_set_style_local_value_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x38, 0x38, 0x38));
		}

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


static void setting_frame_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_display));
}

static void setting_frame_lcd_off_btn_up(lv_obj_t *obj)
{
	lv_checkbox_set_checked(obj, true);
	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_OBJ_ID_OPEN_FRAME);
	lv_checkbox_set_checked(obj, false);

	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_OBJ_ID_CONT);
	lv_obj_set_hidden(obj, true);

	user_data_get()->display.standby_mode = 0;
	user_data_save();
}
/***
** 日期: 2022-05-06 15:39
** 作者: leo.liu
** 函数作用：创建lcd  off 按钮
** 返回参数说明：
***/
static void setting_frame_lcd_off_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_frame_lcd_off_btn_up);
	lv_obj_t *obj = setting_sub_btn_base_create(NULL, 30, 70, 934, 88,
						    layout_setting_display_string_get(SETTING_DISPLAY_LANG_ID_LCD_OFF),
						    &click_data,
						    user_data_get()->display.standby_mode == 0 ? true : false,
						    2);
	lv_obj_set_id(obj, LAYOUT_SETTING_FRAME_OBJ_ID_LCD_OFF);
}

static void setting_frame_open_frame_btn_up(lv_obj_t *obj)
{
	lv_checkbox_set_checked(obj, true);
	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_OBJ_ID_LCD_OFF);
	lv_checkbox_set_checked(obj, false);

	obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_FRAME_OBJ_ID_CONT);
	lv_obj_set_hidden(obj, false);

	user_data_get()->display.standby_mode = 1;
	user_data_save();
}

static void setting_open_frame_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_frame_open_frame_btn_up);
	lv_obj_t *obj = setting_sub_btn_base_create(NULL, 30, 70 + 88, 934, 88,
						    layout_setting_display_string_get(SETTING_DISPLAY_LANG_ID_OPEN_FRAME),
						    &click_data,
						    user_data_get()->display.standby_mode == 0 ? false : true,
						    2);
	lv_obj_set_id(obj, LAYOUT_SETTING_FRAME_OBJ_ID_OPEN_FRAME);
}

/***
** 日期: 2022-05-05 16:23
** 作者: leo.liu
** 函数作用：frame设置的容器
** 返回参数说明：
***/
static lv_obj_t *setting_frame_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	if (cont == NULL)
	{
		printf("motion detect setting cont create failed \n");
		return NULL;
	}
	lv_obj_set_id(cont, LAYOUT_SETTING_FRAME_OBJ_ID_CONT);
	lv_obj_set_pos(cont, 30, 70 + 88 * 2);
	lv_obj_set_size(cont, 934, 88 * 4);

	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

	lv_obj_set_hidden(cont, user_data_get()->display.standby_mode == 0 ? true : false);
	return cont;
}

static const char *setting_open_frame_time_string_get(void)
{
	if (user_data_get()->display.frame_time_en == 0)
	{
		return layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_ALWAYS);
	}
	else
	{
		static char string[12] = {0};
		int s_hour = user_data_get()->display.frame_time_start / 60;
		int s_min = user_data_get()->display.frame_time_start % 60;
		int e_hour = user_data_get()->display.frame_time_end / 60;
		int e_min = user_data_get()->display.frame_time_end % 60;
		sprintf(string, "%02d:%02d-%02d:%02d", s_hour, s_min, e_hour, e_min);
		return string;
	}
}

static void setting_open_frame_time_btn_up(lv_obj_t *obj)
{
	if(user_data_get()->display.frame_background == 2)
	{
		return;
	}
	goto_layout(pLAYOUT(setting_frame_time));
}
/***
** 日期: 2022-05-06 16:24
** 作者: leo.liu
** 函数作用：设置帧时间按钮创建
** 返回参数说明：
***/
static void setting_open_frame_time_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_open_frame_time_btn_up);
	setting_btn_sub_string_create(parent, 0, 0, 934, 88,
				      layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_OPEN_FRAME_TIME),
				      setting_open_frame_time_string_get(),
					  0,
					  1,
				      &click_data);
}

static const char *setting_night_mode_string_get(void)
{
	return language_common_string_get(user_data_get()->display.night_mode == true ? LANG_COMMON_ID_ON : LANG_COMMON_ID_OFF);
}

static void setting_night_mode_btn_up(lv_obj_t *obj)
{
	if(user_data_get()->display.frame_background == 2)
	{
		return;
	}
	goto_layout(pLAYOUT(setting_night_mode));
}
/***
** 日期: 2022-05-06 16:24
** 作者: leo.liu
** 函数作用：设置夜间模式
** 返回参数说明：
***/
static void setting_night_mode_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_night_mode_btn_up);
	setting_btn_sub_string_create(parent, 0, 88, 934, 88,
				      layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_NIGHT_MODE),
				      setting_night_mode_string_get(),
				      0,
					  1,
				      &click_data);
}

static const char *setting_frame_list_string_get(void)
{
	static char buffer[4] = {0};

	int data = user_data_get()->display.frame_list;
	int count = 0;
	if(OwnID == 1)
	{
		for (int i = 0; i < 3; i++)
		{
			if (data & (0x01 << i))
			{
				count++;
			}
		}
		sprintf(buffer, "%d/3", count);
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			if (data & (0x01 << i))
			{
				count++;
			}
		}
		sprintf(buffer, "%d/3", count);
	}

	return buffer;
}
static void setting_frame_list_btn_up(lv_obj_t *obj)
{
	if(user_data_get()->display.frame_background == 2)
	{
		return;
	}
	goto_layout(pLAYOUT(setting_frame_list));
}
/***
** 日期: 2022-05-06 16:24
** 作者: leo.liu
** 函数作用：帧列表按钮创建
** 返回参数说明：
***/
static void setting_frame_list_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_frame_list_btn_up);
	setting_btn_sub_string_create(parent, 0, 88*2, 934, 88,
				      layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_LIST_OF_DISPLAY),
				      setting_frame_list_string_get(),
				      0,
					  1,
				      &click_data);
}

static const char *setting_frame_background_string_get(void)
{
	return layout_setting_frame_string_get(user_data_get()->display.frame_background == 1 ? SETTING_FRAME_LANG_ID_BASIC_IMAGE : user_data_get()->display.frame_background == 0 ? SETTING_FRAME_LANG_ID_NO_BACKGROUND_IMAGE : SETTING_FRAME_LANG_ID_SD_IMAGE);
}

static void setting_frame_background_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_background));
}
/***
** 日期: 2022-05-06 16:24
** 作者: leo.liu
** 函数作用：帧背景显示
** 返回参数说明：
***/
static void setting_frame_background_btn_create(lv_obj_t *parent)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_frame_background_btn_up);
	setting_right_btn_base_create(parent, 0, 88 * 3, 934, 88,
				      layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_IMAGE_OF_BACKGROUND),
				      setting_frame_background_string_get(),
				      &click_data,
				      0);
}

static void LAYOUT_ENTER_FUNC(setting_frame)
{
	setting_cancel_btn_create(setting_frame_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	setting_frame_lcd_off_btn_create();
	

	setting_open_frame_btn_create();
	lv_obj_t *cont = setting_frame_cont_create();
	setting_open_frame_time_btn_create(cont);
	setting_night_mode_btn_create(cont);
	setting_frame_list_btn_create(cont);
	setting_frame_background_btn_create(cont);

}

static void LAYOUT_QUIT_FUNC(setting_frame)
{
}

CREATE_LAYOUT(setting_frame);