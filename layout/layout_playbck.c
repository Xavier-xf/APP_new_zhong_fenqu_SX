#include "layout_define.h"
#include "media_thumb.h"
/*****  预览的文件类型 *****/
static file_type playback_media_type = FILE_TYPE_NONE;
/*****  总文件数目 *****/
static int playback_media_total = 0;
/*****  当前页面第一个的索引 *****/
static int playback_media_top = 0;
/*****  选中的索引 *****/
static int playback_pview_item = 0;
/*****  媒体文件路径 *****/
static char *playback_media_path = NULL;

typedef enum
{
	playback_scr_act_obj_id_cont_1,
	playback_scr_act_obj_id_cont_2,
	playback_scr_act_obj_id_cont_3,
	playback_scr_act_obj_id_cont_4,
	playback_scr_act_obj_id_cont_5,
	playback_scr_act_obj_id_cont_6,
	playback_scr_act_obj_id_right,
	playback_scr_act_obj_id_left,
	playback_scr_act_obj_id_total_label,
} playback_scr_act_obj_id;

typedef enum
{
	playbck_cont_obj_id_new_obj,
	playbck_cont_obj_id_alarm_obj,
	playbck_cont_obj_id_play_obj,
	playbck_cont_obj_id_info_cont,
	playbck_cont_obj_id_motion_obj,
	playbck_cont_obj_id_muound_obj,
	playbck_cont_obj_id_auto_obj,
} playback_cont_obj_id;

typedef enum
{
	// playback_info_cont_obj_id_type_obj,
	playback_info_cont_obj_id_name_label,
	playback_info_cont_obj_id_channel_label,
} playback_info_cont_obj_id;

static void playback_total_label_display(void);
/***
**   日期:2022-05-23 18:35:40
**   作者: leo.liu
**   函数作用：获取当前选中文件的索引
**   参数说明:
***/
int playback_pview_item_get(void)
{
	return playback_pview_item;
}
void playback_pview_item_set(int item)
{
	playback_pview_item = item;
}
/***
**   日期:2022-05-24 09:56:58
**   作者: leo.liu
**   函数作用：获取预览的类型
**   参数说明:
***/
file_type playback_pview_type_get(void)
{
	return playback_media_type;
}
/***
**   日期:2022-05-24 10:23:22
**   作者: leo.liu
**   函数作用：获取媒体根路径
**   参数说明:
***/
const char *playback_pview_path_get(void)
{
	return playback_media_path;
}
/***
**   日期:2022-05-24 10:51:04
**   作者: leo.liu
**   函数作用：获取媒体总数
**   参数说明:
***/
int playback_media_total_get(void)
{
	return playback_media_total;
}
/***
**   日期:2022-05-24 10:52:06
**   作者: leo.liu
**   函数作用：设置媒体总数
**   参数说明:
***/
void playback_media_total_set(int total)
{
	playback_media_total = total;
}

/***
**   日期:2022-05-24 11:35:47
**   作者: leo.liu
**   函数作用：获取预览页面的第一个索引
**   参数说明:
***/
int playback_media_top_get(void)
{
	return playback_media_top;
}
/***
**   日期:2022-05-24 11:36:23
**   作者: leo.liu
**   函数作用：设置预览页面的第一个索引
**   参数说明:
***/
void playback_media_top_set(int val)
{
	playback_media_top = val;
}

/***
**   日期:2022-05-24 10:55:48
**   作者: leo.liu
**   函数作用：获取当前媒体信息
**   参数说明:
***/
const file_info *playback_media_info_get(void)
{
	return media_file_info_get(playback_media_type, playback_pview_item);
}
/***
**   日期:2022-05-23 11:57:10
**   作者: leo.liu
**   函数作用：缩略图参数初始化
**   参数说明:
***/
void layout_playback_parm_init(void)
{
	if (media_sdcard_insert_check() == true)
	{
		playback_media_type = FILE_TYPE_VIDEO;
		playback_media_path = SD_MEDIA_PATH;
	}
	else
	{
		playback_media_type = FILE_TYPE_FLASH_PHOTO;
		playback_media_path = FLASH_PHOTO_PATH;
	}
	media_file_total_get(playback_media_type, &playback_media_total, NULL);
	playback_media_top = playback_media_total - 1;
	playback_pview_item = 0;
}

/***
**   日期:2022-05-23 14:16:47
**   作者: leo.liu
**   函数作用：被按下
**   参数说明:
***/
static void playback_thumb_obj_up(lv_obj_t *obj)
{
	for (int i = 0; i < 6; i++)
	{
		if (obj == lv_obj_get_child_form_id(lv_scr_act(), playback_scr_act_obj_id_cont_1 + i))
		{
			int index = playback_media_top - i;
			if ((index >= 0) && (index < playback_media_total))
			{
				playback_pview_item = index;
				const file_info *info = media_file_info_get(playback_media_type, index);
				if (info->type == FILE_TYPE_VIDEO)
				{
					goto_layout(pLAYOUT(video));
				}
				else
				{
					goto_layout(pLAYOUT(photo));
				}
				return;
			}
		}
	}
}

/***
**   日期:2022-05-23 13:57:17
**   作者: leo.liu
**   函数作用：创建显示容器基础函数
**   参数说明:
***/
static void playback_thumb_obj_base(int x, int y, int w, int h, playback_scr_act_obj_id id)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, id);
	lv_obj_set_pos(cont, x, y);
	lv_obj_set_size(cont, w, h);
	lv_obj_set_hidden(cont, true);
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
	static obj_click_data click_data = obj_click_data_up_create(playback_thumb_obj_up);
	obj_click_event_listen(cont, &click_data);

	lv_obj_t *obj = lv_obj_create(cont, NULL);
	lv_obj_set_id(obj, playbck_cont_obj_id_new_obj);
	// lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 25, 25);
	lv_obj_align(obj, cont, LV_ALIGN_IN_TOP_LEFT, 0, 0);
	static rom_bin_info new_img = rom_bin_info_get(ROM_UI_33_PLAYBACK_NEW_PNG);        //新文件标识
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &new_img);  
	lv_obj_set_click(obj, false);

	obj = lv_obj_create(cont, NULL);
	lv_obj_set_id(obj, playbck_cont_obj_id_alarm_obj);
	lv_obj_set_pos(obj, w - 38 - 12, 12);
	lv_obj_set_size(obj, 38, 38);
	lv_obj_set_click(obj, false);
	static rom_bin_info alarm_img = rom_bin_info_get(ROM_UI_25_PLAYBACK_EMERGENCY_PNG);     //警报图片标识
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &alarm_img);

	obj = lv_obj_create(cont, NULL);
	lv_obj_set_id(obj, playbck_cont_obj_id_motion_obj);
	lv_obj_set_pos(obj, w - 38 - 12, 12);
	lv_obj_set_size(obj, 38, 38);
	lv_obj_set_click(obj, false);
	static rom_bin_info motion_img = rom_bin_info_get(ROM_UI_MODE_MOTTIN_PNG);     //移动侦测图片标识
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &motion_img);

	obj = lv_obj_create(cont, NULL);
	lv_obj_set_id(obj, playbck_cont_obj_id_muound_obj);
	lv_obj_set_pos(obj, w - 38 - 12, 12);
	lv_obj_set_size(obj, 38, 38);
	lv_obj_set_click(obj, false);
	static rom_bin_info muound_img = rom_bin_info_get(ROM_UI_MALUND_PNG);     //手动记录图片标识
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &muound_img);

	obj = lv_obj_create(cont, NULL);
	lv_obj_set_id(obj, playbck_cont_obj_id_auto_obj);
	lv_obj_set_pos(obj, w - 38 - 12, 12);
	lv_obj_set_size(obj, 38, 38);
	lv_obj_set_click(obj, false);
	static rom_bin_info auto_img = rom_bin_info_get(ROM_UI_AUTO_PNG);     //自动记录图片标识
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &auto_img);

	obj = lv_obj_create(cont, NULL);
	lv_obj_set_id(obj, playbck_cont_obj_id_play_obj);
	lv_obj_set_pos(obj, (w - 83) / 2, (h - 83) / 2);
	lv_obj_set_size(obj, 83, 83);
	lv_obj_set_click(obj, false);
	static rom_bin_info play_img = rom_bin_info_get(ROM_UI_02__THUMB_PLAY_PNG);      //视频文件播放标识
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &play_img);

	cont = lv_cont_create(cont, NULL);
	lv_obj_set_id(cont, playbck_cont_obj_id_info_cont);
	lv_obj_set_pos(cont, 0, h - 51);
	lv_obj_set_size(cont, w, 51);
	lv_obj_set_click(cont, false);
	lv_obj_set_style_local_bg_color(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00));
	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70);

	// obj = lv_obj_create(cont, NULL);
	// lv_obj_set_id(obj, playback_info_cont_obj_id_type_obj);
	// lv_obj_set_pos(obj, 5, 14);
	// lv_obj_set_size(obj, 32, 32);
	// lv_obj_set_click(obj, false);
	// static rom_bin_info defaul_img = rom_bin_info_get(ROM_UI_38_LIST_VISITOR_IMG_PNG);
	// static rom_bin_info defaul_video = rom_bin_info_get(ROM_UI_37_LIST_VISITOR_VIDEO_PNG);    //照片或者视频辨识
	// lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &defaul_img);
	// lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &defaul_video);

	lv_obj_t *label = lv_label_create(cont, NULL);
	lv_obj_set_id(label, playback_info_cont_obj_id_name_label);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 40, 8);
	lv_obj_set_size(label, w - 50, 35);
	lv_obj_set_click(label, false);
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);

	label = lv_label_create(cont, label);
	lv_obj_set_id(label, playback_info_cont_obj_id_channel_label);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, w - 100, 8);
	lv_obj_set_size(label, 100, 35);
	lv_obj_set_click(label, false);
	lv_label_set_align(label, LV_LABEL_ALIGN_RIGHT);
}

/***
**   日期:2022-05-23 14:27:25
**   作者: leo.liu
**   函数作用：创建缩略图
**   参数说明:
***/
static void playback_thumb_obj_create(void)
{
	playback_thumb_obj_base(0, 74, 340, 260, playback_scr_act_obj_id_cont_1);
	playback_thumb_obj_base(342, 74, 340, 260, playback_scr_act_obj_id_cont_2);
	playback_thumb_obj_base(684, 74, 340, 260, playback_scr_act_obj_id_cont_3);
	playback_thumb_obj_base(0, 338, 340, 260, playback_scr_act_obj_id_cont_4);
	playback_thumb_obj_base(342, 338, 340, 260, playback_scr_act_obj_id_cont_5);
	playback_thumb_obj_base(684, 338, 340, 260, playback_scr_act_obj_id_cont_6);
}

/***
**   日期:2022-05-23 14:42:55
**   作者: leo.liu
**   函数作用：文件信息显示
**   参数说明:
***/
static void layout_playback_info_display(lv_obj_t *parent, const file_info *pinfo)
{
	lv_obj_t *new = lv_obj_get_child_form_id(parent, playbck_cont_obj_id_new_obj);
	lv_obj_set_hidden(new, pinfo->is_new == true ? false : true);

	lv_obj_t *alarm = lv_obj_get_child_form_id(parent, playbck_cont_obj_id_alarm_obj);
	lv_obj_set_hidden(alarm, pinfo->mode == REC_MODE_ALARM ? false : true);

	lv_obj_t *motion = lv_obj_get_child_form_id(parent, playbck_cont_obj_id_motion_obj);
	lv_obj_set_hidden(motion, pinfo->mode == REC_MODE_MOTION ? false : true);

	lv_obj_t *muound = lv_obj_get_child_form_id(parent, playbck_cont_obj_id_muound_obj);
	lv_obj_set_hidden(muound, pinfo->mode == REC_MODE_MANUAL ? false : true);

	lv_obj_t *auto_flag = lv_obj_get_child_form_id(parent, playbck_cont_obj_id_auto_obj);
	lv_obj_set_hidden(auto_flag, pinfo->mode == REC_MODE_AUTO ? false : true);

	lv_obj_t *play = lv_obj_get_child_form_id(parent, playbck_cont_obj_id_play_obj);
	lv_obj_set_hidden(play, pinfo->type == FILE_TYPE_VIDEO ? false : true);

	lv_obj_t *cont = lv_obj_get_child_form_id(parent, playbck_cont_obj_id_info_cont);
	// lv_obj_t *type = lv_obj_get_child_form_id(cont, playback_info_cont_obj_id_type_obj);
	// lv_obj_set_state(type, pinfo->type == FILE_TYPE_VIDEO ? LV_STATE_CHECKED : LV_STATE_DEFAULT);

	lv_obj_t *name = lv_obj_get_child_form_id(cont, playback_info_cont_obj_id_name_label);
	lv_label_set_text(name, pinfo->file_name);

	lv_obj_t *ch = lv_obj_get_child_form_id(cont, playback_info_cont_obj_id_channel_label);
	const char *pch = language_common_ch_string_get(pinfo->ch);

	lv_label_set_text(ch, pch);
}
/***
**   日期:2022-05-23 13:48:09
**   作者: leo.liu
**   函数作用：缩略图显示
**   参数说明:
***/
static void layout_playback_thumb_load(void)
{
	thumb_media_buffer_clear();
	playback_scr_act_obj_id obj_id = playback_scr_act_obj_id_cont_1;
	for (int i = 0; i < 6; i++)
	{
		int index = playback_media_top - i;
		lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), obj_id + i);
		if (index < 0)
		{
			lv_obj_set_hidden(cont, true);
		}
		else
		{
			lv_obj_set_hidden(cont, false);

			const file_info *pinfo = media_file_info_get(playback_media_type, index);

			char file[128] = {0};
			strcpy(file, playback_media_path);

			strcat(file, pinfo->file_name);
			// if (access(file, F_OK) != 0)
			// {

			// }  

			thumb_media_load(lv_obj_get_x(cont), lv_obj_get_y(cont), lv_obj_get_width(cont), lv_obj_get_height(cont), file);

			layout_playback_info_display(cont, pinfo);
		}
	}
}

/***
**   日期:2022-05-23 16:09:38
**   作者: leo.liu
**   函数作用：创建左右按钮
**   参数说明:
***/
static void playbacK_right_btn_up(lv_obj_t *obj)
{
	if (playback_media_total <= 6)
	{
		return;
	}
	playback_media_top -= 6; 
	if (playback_media_top < 0)
	{
		playback_media_top = playback_media_total - 1;
	}
	layout_playback_thumb_load();
	playback_total_label_display();
}
/***
**   日期:2022-05-24 10:37:20
**   作者: leo.liu
**   函数作用：创建下一个按钮
**   参数说明:
***/
void playback_right_btn_create(int obj_id, void (*callback)(lv_obj_t *), int x, int y, int w, int h)
{ // 1161,391,104,104
	static obj_click_data click_data = obj_click_data_up_create(playbacK_right_btn_up);
	click_data.up = callback;
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, obj_id);
	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_2_THUMB_RIGHT_N_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	obj_click_event_listen(obj, &click_data);
	lv_obj_set_hidden(obj, playback_media_total > 6 ? false : true);
}
static void playbacK_left_btn_up(lv_obj_t *obj)
{
	if (playback_media_total <= 6)
	{
		return;
	}
	playback_media_top += 6;
	if (playback_media_top >= playback_media_total)
	{
		playback_media_top = playback_media_total % 6 - 1;
		if (playback_media_top == -1)
		{
			playback_media_top = 5;
		}
	}
	layout_playback_thumb_load();
	playback_total_label_display();
}
/***
**   日期:2022-05-24 10:36:17
**   作者: leo.liu
**   函数作用：创建上个按钮
**   参数说明:
***/
void playbacK_left_btn_create(int obj_id, void (*callback)(lv_obj_t *), int x, int y, int w, int h)
{ // 45,391,104,104
	static obj_click_data click_data = obj_click_data_up_create(playbacK_left_btn_up);
	click_data.up = callback;
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, obj_id);
	lv_obj_set_pos(obj, x, y);
	lv_obj_set_size(obj, w, h);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_1_THUMB_LEFT_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	obj_click_event_listen(obj, &click_data);
	lv_obj_set_hidden(obj, playback_media_total > 6 ? false : true);
}

/***
**   日期:2022-05-23 16:28:24
**   作者: leo.liu
**   函数作用：页面显示
**   参数说明:
***/
static void playback_total_label_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), playback_scr_act_obj_id_total_label);
	lv_label_set_text_fmt(obj, "%04d/%04d", (playback_media_total - playback_media_top) / 6 + 1, (playback_media_total - 1) / 6 + 1);
	lv_obj_set_style_local_text_color(obj,LV_OBJ_PART_MAIN,LV_STATE_DEFAULT,lv_color_hex(0x0096ff));
}
/***
**   日期:2022-05-23 16:25:07
**   作者: leo.liu
**   函数作用：显示页面创建
**   参数说明:
***/
static void playback_total_label_create(void)
{
	lv_obj_t *obj = lv_label_create(lv_scr_act(), NULL);
	lv_obj_set_id(obj, playback_scr_act_obj_id_total_label);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 1024 - 120, 30);
	lv_obj_set_size(obj, 120, 47);
	lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);

	lv_obj_set_style_local_text_font(obj, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	playback_total_label_display();
}


static void playback_sd_detection_task(void)
{
	if(media_sdcard_insert_check() == false)
	{
		goto_layout(pLAYOUT(home));
	}
}


static void LAYOUT_ENTER_FUNC(playback)
{
	setting_cancel_btn_create(NULL);
	setting_head_label_create(layout_home_string_get(HOME_LANG_ID_PLAYBACK));
	thumb_media_buffer_clear();
	fb_gui_layer_rect_fill(0x00,0,0,LV_HOR_RES_MAX,LV_VER_RES_MAX);

	if (playback_media_total < 1)
	{
		return;
	}
	thumb_media_open();
	
	playback_thumb_obj_create();
	

	playback_right_btn_create(playback_scr_act_obj_id_right, playbacK_right_btn_up, 935, 293, 83, 83);
	playbacK_left_btn_create(playback_scr_act_obj_id_left, playbacK_left_btn_up, 15, 293, 83, 83);
	playback_total_label_create();
	layout_playback_thumb_load();
	layout_sd_state_callback_register(playback_sd_detection_task);
}
static void LAYOUT_QUIT_FUNC(playback)
{
	const layout *cur_layout = cur_layout_get();
	if ((cur_layout != pLAYOUT(photo)) && (cur_layout != pLAYOUT(video)))
	{
		thumb_media_close();
		layout_sd_state_callback_register(NULL);
	}
}

CREATE_LAYOUT(playback);