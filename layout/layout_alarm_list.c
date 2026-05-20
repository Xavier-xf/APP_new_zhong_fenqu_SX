#include "layout_define.h"
/***
**   日期:2022-07-14 17:17:22
**   作者: leo.liu
**   函数作用：警报记录obj id
**   参数说明:
***/
typedef enum
{
	alarm_list_scr_act_obj_id_list,
	alarm_list_scr_act_obj_id_msg
} alarm_list_scr_act_obj_id;
/***
**   日期:2022-07-14 17:17:32
**   作者: leo.liu
**   函数作用：警报记录返回按钮函数
**   参数说明:
***/
static void alarm_list_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(security));
}
/***
** 日期: 2022-05-04 17:30
** 作者: leo.liu
** 函数作用：消息框返回按钮点击事件
** 返回参数说明：
***/
static void alarm_list_msg_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(alarm_list_scr_act_obj_id_msg);
}
/***
** 日期: 2022-05-04 17:31
** 作者: leo.liu
** 函数作用：消息框确认按钮点击事件
** 返回参数说明：
***/
static void alarm_list_msg_confirm_btn_up(lv_obj_t *obj)
{
	lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), alarm_list_scr_act_obj_id_msg);
	if (dialog == NULL)
	{
		printf("find obj dialog failed \n");
		return;
	}
	alarm_list_del_all();
	goto_layout(pLAYOUT(alarm_list));
}
/***
**   日期:2022-07-14 17:18:09
**   作者: leo.liu
**   函数作用：警报记录删除函数
**   参数说明:
***/
static void layout_alarm_list_delete_btn_up(lv_obj_t *obj)
{
	lv_obj_t *msg = lv_obj_get_child_form_id(lv_scr_act(), alarm_list_scr_act_obj_id_msg);
	if (msg != NULL)
	{
		return;
	}
	msg = setting_msgdialog_msg_bg_create(alarm_list_scr_act_obj_id_msg);
	setting_msgdialog_msg_confirm_and_cancel_btn_create(msg, alarm_list_msg_cancel_btn_up, alarm_list_msg_confirm_btn_up);
	setting_msgdialog_msg_create(msg, layout_setting_storage_string_get(SETTING_STORAGE_LANG_ID_MSG_DEL_FILE));
	lv_obj_set_style_local_value_ofs_y(msg, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -30);
	lv_obj_set_style_local_value_font(msg, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
}
/***
**   日期:2022-07-14 17:20:42
**   作者: leo.liu
**   函数作用：警报记录列表创建
**   参数说明:
***/
static lv_obj_t *layout_alarm_list_list_create(void)
{
	lv_obj_t *obj = lv_page_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 30, 88);
	lv_obj_set_size(obj, 934, 88 * 7);
	lv_page_set_scrollable_fit4(obj, LV_FIT_NONE, LV_FIT_NONE, LV_FIT_MAX, LV_FIT_MAX);
	lv_obj_set_id(obj, alarm_list_scr_act_obj_id_list);
	return obj;
}
/***
**   日期:2022-07-14 17:21:04
**   作者: leo.liu
**   函数作用：警报记录按钮创建
**   参数说明:
***/
static void layout_alarm_list_delete_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 948, 10);
	lv_obj_set_size(obj, 60, 60);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_DELETE_SECURITY_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(layout_alarm_list_delete_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
** 日期: 2022-05-05 14:12
** 作者: leo.liu
** 函数作用：设置右边字符串的按钮
** 返回参数说明：
***/
lv_obj_t *alarm_list_btn_sub_string_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, struct tm *tm)
{
	lv_obj_t *btn = lv_btn_create(parent == NULL ? lv_scr_act() : parent, NULL);
	if (btn == NULL)
	{
		printf("create setting right btn failed \n");
		return NULL;
	}
	lv_obj_set_pos(btn, x, y);
	lv_obj_set_size(btn, w, h);
	lv_obj_set_click(btn, false);
	if (main_string != NULL)
	{
		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, main_string);
		lv_obj_set_style_local_value_font(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_obj_set_style_local_value_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
		lv_obj_set_style_local_value_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
	}

	if (tm != NULL)
	{
		lv_obj_t *sub_obj = lv_label_create(parent == NULL ? lv_scr_act() : parent, NULL);
		if (sub_obj == NULL)
		{
			printf("create setting sub string fail \n");
			return btn;
		}
		lv_obj_set_style_local_text_font(sub_obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
		lv_label_set_long_mode(sub_obj, LV_LABEL_LONG_CROP);
		lv_obj_set_pos(sub_obj, x + 640, y + 32);
		lv_obj_set_size(sub_obj, 240, 35);
		lv_obj_set_click(sub_obj, false);

		lv_label_set_text_fmt(sub_obj, "%04d.%02d.%02d %s%02d:%02d:%02d", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour < 12 ? "AM" : "PM", tm->tm_hour, tm->tm_min, tm->tm_sec);
		lv_label_set_align(sub_obj, LV_LABEL_ALIGN_RIGHT);
	}

	/***** 设置下底边框 *****/
	lv_obj_set_style_local_border_width(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
	lv_obj_set_style_local_border_color(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x32, 0x32, 0x37));
	lv_obj_set_style_local_border_color(btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x32, 0x32, 0x37));
	lv_obj_set_style_local_border_side(btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
	lv_page_glue_obj(btn, true);
	return btn;
}
/***
**   日期:2022-07-14 17:23:37
**   作者: leo.liu
**   函数作用：警报记录信息控件创建
**   参数说明:
***/
static void alarm_list_info_create(lv_obj_t *parent)
{
	int total = 0;
	if ((alarm_list_total_get(&total) == false) || (total < 1))
	{
		return;
	}
	for (int i = 0; i < total; i++)
	{
		int ch = 0;
		struct tm tm;
		if (alarm_list_get(i, &ch, &tm) == false)
		{
			break;
		}
		alarm_list_btn_sub_string_create(parent, 0, 96 * i, 934, 88,
						 layout_security_string_get(ch == 1 ? LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY_1 : LAYOUT_SECURITY_LANGUAGE_ID_SECURITY_EMERGENCY_2),
						 &tm);
	}
}
/***
**   日期:2022-07-14 17:24:27
**   作者: leo.liu
**   函数作用：警报记录页面进入的第一个函数
**   参数说明:
***/
static void LAYOUT_ENTER_FUNC(alarm_list)
{
	setting_cancel_btn_create(alarm_list_cancel_btn_up);
	setting_head_label_create(layout_security_string_get(LAYOUT_SECURITY_LANGUAGE_ID_EMERITY_RECORD));
	layout_alarm_list_delete_btn_create();

	lv_obj_t *list = layout_alarm_list_list_create();
	alarm_list_info_create(list);
}
/***
**   日期:2022-07-14 17:24:43
**   作者: leo.liu
**   函数作用：警报记录页面退出的最后一个函数
**   参数说明:
***/
static void LAYOUT_QUIT_FUNC(alarm_list)
{
}
/***
**   日期:2022-07-14 17:25:07
**   作者: leo.liu
**   函数作用：创建警报记录页面
**   参数说明:
***/
CREATE_LAYOUT(alarm_list);