#include "layout_define.h"
#include "media_thumb.h"
#include "layout_setting_common.h"
int playback_pview_item_get(void);
void playback_pview_item_set(int item);
file_type playback_pview_type_get(void);
const char *playback_pview_path_get(void);
void playbacK_left_btn_create(int obj_id, void (*callback)(lv_obj_t *), int x, int y, int w, int h);
void playback_right_btn_create(int obj_id, void (*callback)(lv_obj_t *), int x, int y, int w, int h);
int playback_media_total_get(void);
void playback_media_total_set(int total);
const file_info *playback_media_info_get(void);
int playback_media_top_get(void);
void playback_media_top_set(int val);
static void layout_photo_full_load(void);
typedef enum
{
	photo_scr_act_obj_id_head_cont,
	photo_scr_act_obj_id_head_info_label,
	photo_scr_act_obj_id_left_btn,
	photo_scr_act_obj_id_right_btn,
	photo_scr_act_obj_id_buttom_name,
	photo_scr_act_obj_id_msg_dialog
} photo_scr_act_obj_id;

/***
**   日期:2022-05-24 09:38:11
**   作者: leo.liu
**   函数作用：常见顶部容器
**   参数说明:
***/
lv_obj_t *layout_media_head_count_create(int obj_id, lv_opa_t opa)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, obj_id);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 70);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008)); //
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, opa);
	return cont;
}
/***
**   日期:2022-05-24 09:38:33
**   作者: leo.liu
**   函数作用：创建返回按钮
**   参数说明:
***/
static void layout_photo_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(playback));
}
void layout_media_cancel_btn_create(lv_obj_t *parent, void (*callback)(lv_obj_t *obj))
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 10, 10);
	lv_obj_set_size(obj, 60, 60);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(layout_photo_cancel_btn_up);
	click_data.up = callback;
	obj_click_event_listen(obj, &click_data);
}

/***
**   日期:2022-05-24 10:00:17
**   作者: leo.liu
**   函数作用：显示文本信息
**   参数说明:
***/
void layout_photo_info_label_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), photo_scr_act_obj_id_head_cont);
	lv_obj_t *label = lv_obj_get_child_form_id(parent, photo_scr_act_obj_id_head_info_label);
	const file_info *pinfo = playback_media_info_get();
	const char *ch = language_common_ch_string_get(pinfo->ch);
	lv_label_set_text_fmt(label, "%s[%05d/%05d]", ch, playback_pview_item_get() + 1, playback_media_total_get());
}

/***
**   日期:2022-05-24 09:55:22
**   作者: leo.liu
**   函数作用：创建信息显示
**   参数说明:
***/
void layout_media_info_label_create(lv_obj_t *parent, int obj_id)
{
	lv_obj_t *label = lv_label_create(parent, NULL);
	lv_obj_set_id(label, obj_id);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 353, 23);
	lv_obj_set_size(label, 318, 47);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
}
/***
**   日期:2022-05-24 11:12:09
**   作者: leo.liu
**   函数作用：取消删除
**   参数说明:
***/
static void layout_photo_delete_msg_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(photo_scr_act_obj_id_msg_dialog);
}
/***
**   日期:2022-05-24 11:12:54
**   作者: leo.liu
**   函数作用：确认删除
**   参数说明:
***/
static void layout_photo_delete_msg_confirm_btn_up(lv_obj_t *obj)
{
	file_type type = playback_pview_type_get();
	int item = playback_pview_item_get();
	media_file_delete(type, item);

	int total;
	media_file_total_get(type, &total, NULL);
	if (total < 1)
	{
		goto_layout(pLAYOUT(home));
		return;
	}

	playback_media_total_set(total);
	if (item >= total)
	{
		playback_pview_item_set(total - 1);
	}

	int top = playback_media_top_get();
	if (top == 0)
	{
		top = (total > 5) ? 5 : (total - 1);
	}
	else
	{
		top -= 1;
	}
	playback_media_top_set(top);

	setting_msgdialog_msg_bg_delete(photo_scr_act_obj_id_msg_dialog);

	const file_info *info = playback_media_info_get();
	if (info->type == FILE_TYPE_VIDEO)
	{
		goto_layout(pLAYOUT(video));
	}
	else
	{
		layout_photo_full_load();
	}
}

/***
**   日期:2022-05-24 09:38:56
**   作者: leo.liu
**   函数作用：创建删除按钮
**   参数说明:
***/
static void layout_photo_delete_btn_up(lv_obj_t *obj)
{
	if (setting_msgdialog_msg_bg_delete(photo_scr_act_obj_id_msg_dialog) == true)
	{
		return;
	}
	lv_obj_t *dialog = setting_msgdialog_msg_bg_create(photo_scr_act_obj_id_msg_dialog);
	setting_msgdialog_msg_confirm_and_cancel_btn_create(dialog, layout_photo_delete_msg_cancel_btn_up, layout_photo_delete_msg_confirm_btn_up);
	setting_msgdialog_msg_create(dialog, layout_playback_string_get(PLAYBACK_LANG_ID_DELETE_PHOTO));
	lv_obj_set_style_local_value_ofs_y(dialog, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -30);
	lv_obj_set_style_local_value_font(dialog, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
}
static void layout_photo_delete_btn_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 948, 10);
	lv_obj_set_size(obj, 60, 60);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_DELETE_SECURITY_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(layout_photo_delete_btn_up);
	obj_click_event_listen(obj, &click_data);
}
/***
**   日期:2022-05-24 09:39:22
**   作者: leo.liu
**   函数作用：创建上一个按钮
**   参数说明:
***/
static void layout_photo_left_btn_up(lv_obj_t *obj)
{
	int item = playback_pview_item_get();
	item++;
	if (item >= playback_media_total_get())
	{
		item = 0;
	}
	playback_pview_item_set(item);
	const file_info *info = playback_media_info_get();
	if (info->type == FILE_TYPE_VIDEO)
	{
		goto_layout(pLAYOUT(video));
	}
	else
	{
		setting_msgdialog_msg_bg_delete(photo_scr_act_obj_id_msg_dialog);
		layout_photo_full_load();
	}
}
/***
**   日期:2022-05-24 09:39:47
**   作者: leo.liu
**   函数作用：创建下一个按钮
**   参数说明:
***/
static void layout_photo_right_btn_up(lv_obj_t *obj)
{
	int item = playback_pview_item_get();
	item--;
	if (item < 0)
	{
		item = playback_media_total_get() - 1;
	}
	playback_pview_item_set(item);
	const file_info *info = playback_media_info_get();
	if (info->type == FILE_TYPE_VIDEO)
	{
		goto_layout(pLAYOUT(video));
	}
	else
	{
		setting_msgdialog_msg_bg_delete(photo_scr_act_obj_id_msg_dialog);

		layout_photo_full_load();
	}
}

/***
**   日期:2022-05-24 10:45:53
**   作者: leo.liu
**   函数作用：记录时间显示
**   参数说明:
***/
static void layout_photo_buttom_name_display(void)
{
	lv_obj_t *label = lv_obj_get_child_form_id(lv_scr_act(), photo_scr_act_obj_id_buttom_name);
	const file_info *info = playback_media_info_get();

	char str[128] = {"20"};
	strncpy(&str[2], &info->file_name[0], 2);
	str[4] = '-';
	strncat(&str[5], &info->file_name[2], 2);
	str[7] = '-';
	strncat(&str[8], &info->file_name[4], 2);
	str[10] = ' ';
	str[11] = ' ';

	strncat(&str[12], &info->file_name[7], 2);
	str[14] = ':';
	strncat(&str[15], &info->file_name[9], 2);
	str[17] = ':';
	strncat(&str[18], &info->file_name[11], 2);

	lv_label_set_text(label, str);
}
/***
**   日期:2022-05-24 09:40:06
**   作者: leo.liu
**   函数作用：创建底部时间显示
**   参数说明:
***/
static void layout_photo_buttom_name_create(void)
{
	lv_obj_t *label = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_id(label, photo_scr_act_obj_id_buttom_name);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 48, 540);
	lv_obj_set_size(label, 252, 60);
	lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
}

/***
**   日期:2022-05-24 10:21:20
**   作者: leo.liu
**   函数作用：加载照片
**   参数说明:
***/
static void layout_photo_full_load(void)
{
	const file_info *pinfo = playback_media_info_get();
	char file[128] = {0};
	strcpy(file, playback_pview_path_get());
	strcat(file, pinfo->file_name);
	thumb_media_load(0, 0, 1024, 600, file);
	if (pinfo->is_new == true)
	{
		media_file_new_clear(pinfo->type, playback_pview_item_get());
	}
	layout_photo_info_label_display();
	layout_photo_buttom_name_display();

	lv_obj_invalidate(lv_scr_act());
}

/***
**   日期:2022-05-26 08:08:49
**   作者: leo.liu
**   函数作用：左按钮显示
**   参数说明:
***/
static void layout_photo_left_enable(bool en)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), photo_scr_act_obj_id_left_btn);
	lv_obj_set_hidden(obj, en ? false : true);
}
/***
**   日期:2022-05-26 08:09:39
**   作者: leo.liu
**   函数作用：右显示
**   参数说明:
***/
static void layout_photo_right_enable(bool en)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), photo_scr_act_obj_id_right_btn);
	lv_obj_set_hidden(obj, en ? false : true);
}
static void LAYOUT_ENTER_FUNC(photo)
{
	lv_obj_t *cont = layout_media_head_count_create(photo_scr_act_obj_id_head_cont, LV_OPA_80);
	layout_media_cancel_btn_create(cont, layout_photo_cancel_btn_up);
	layout_media_info_label_create(cont, photo_scr_act_obj_id_head_info_label);
	layout_photo_delete_btn_create(cont);
	playbacK_left_btn_create(photo_scr_act_obj_id_left_btn, layout_photo_left_btn_up, 15, 293, 83, 83);
	layout_photo_left_enable(true);
	playback_right_btn_create(photo_scr_act_obj_id_right_btn, layout_photo_right_btn_up, 935, 293, 83, 83);
	layout_photo_right_enable(true);
	layout_photo_buttom_name_create();
	layout_photo_full_load();
}
static void LAYOUT_QUIT_FUNC(photo)
{
	const layout *cur_layout = cur_layout_get();
	if ((cur_layout != pLAYOUT(playback)) && (cur_layout != pLAYOUT(video)))
	{
		thumb_media_close();
		layout_sd_state_callback_register(NULL);
	}
	else if (cur_layout == pLAYOUT(playback))
	{
		lv_obj_invalidate(lv_scr_act());
	}
}

CREATE_LAYOUT(photo);