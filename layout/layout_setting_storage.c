#include "layout_define.h"
#define LAYOUT_SETTING_STORAGE_OBJ_MSG_ID 0X01

#define LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_1_ID 0x03
#define LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_2_ID 0x04
#define LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_3_ID 0x05
#define LAYOUT_SETTING_STORAGE_OBJ_SD_CAPACITY_ID 0x06


/***
**   日期:2022-05-23 09:57:36
**   作者: leo.liu
**   函数作用：sd卡格式化检测任务
**   参数说明:
***/
static void setting_format_sd_state_fmat_task(lv_task_t *task)
{
	if (media_format_sd_state() == false)
	{
		// lv_obj_t *parent = (lv_obj_t *)task->user_data;
		setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
		media_del_file_state_set(false);
		lv_task_del(task);
		standby_timer_restart(true);
		printf("##########=============sd_format\n");
	}
	
}
/***
**   日期:2022-06-15 11:09:46
**   作者: leo.liu
**   函数作用：删除文件
**   参数说明:
***/
static void setting_delete_media_file_task(lv_task_t *task)
{
	char flag = *((char *)task->user_data);
	file_type type = flag & 0x80 ? FILE_TYPE_VIDEO : FILE_TYPE_FLASH_PHOTO;
	char value = flag & 0x0F;
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		media_file_delete_all(FILE_TYPE_FLASH_PHOTO, true);
	}
	else if (value == 4)
	{
		media_file_delete_all(FILE_TYPE_VIDEO, true);
	}
	else if (value == 3)
	{
		media_file_delete_all(FILE_TYPE_VIDEO, false);
	}
	else if (value == 2)
	{
		media_file_delete_all(FILE_TYPE_PHOTO, false);
	}
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	media_del_file_state_set(false);
	lv_task_del(task);
	standby_timer_restart(true);
}


/***
**   日期:2022-05-23 09:57:36
**   作者: leo.liu
**   函数作用：复制flash中的照片到SD卡中的检测任务
**   参数说明:
***/
static void setting_format_to_sd_state_task(lv_task_t *task)
{
	if(media_sdcard_insert_check() == true)
	{
		copy_flash_photo_to_sd();
	}
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	media_del_file_state_set(false);
	lv_task_del(task);
	standby_timer_restart(true);
	
}
/***
** 日期: 2022-05-04 08:45
** 作者: leo.liu
** 函数作用：加载动画任务创建
** 返回参数说明：
** type 1：sd format
** type 2:delete photo
** type 3:delete video
** type 4:delete all
***/
static void setting_animation_task_create(lv_obj_t *parent, int type)
{
	media_del_file_state_set(true);
	standby_timer_close();
	int msg_id = 0;
	if (type == 1)
	{
		msg_id = SETTING_STORAGE_LANG_ID_MSG_FORMAT_SD;
		media_format_sd();
		lv_layout_task_create(setting_format_sd_state_fmat_task, 1000, LV_TASK_PRIO_MID, parent);
	}
	else if ((type == 2) || (type == 3) || (type == 4))
	{
		msg_id = SETTING_STORAGE_LANG_ID_MSG_DEL_FILE;
		static char flag = 0x00;
		flag = (type & 0x0F) | (media_sdcard_insert_check() ? 0x80 : 0x00);
		lv_layout_task_create(setting_delete_media_file_task, 2000, LV_TASK_PRIO_MID, &flag);
	}
	else if(type == 5)
	{
		msg_id = SETTING_STORAGE_LANG_ID_FLASH_PHOTO_TO_SD;
		lv_layout_task_create(setting_format_to_sd_state_task, 1000, LV_TASK_PRIO_MID, &parent);
	}
	else
	{
		return;
	}

	setting_msgdialog_msg_create(parent, layout_setting_storage_string_get(msg_id));
	lv_obj_set_style_local_value_ofs_y(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -30);
	lv_obj_set_style_local_value_font(parent, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));

	lv_obj_t *preload = lv_spinner_create(parent, NULL);
	lv_obj_set_size(preload, 80, 80);
	lv_obj_align(preload, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

	lv_obj_set_style_local_line_color(preload, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, lv_color_make(0x00, 0x96, 0xFF));
	lv_obj_set_style_local_line_color(preload, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
	lv_obj_set_style_local_line_width(preload, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 8);
	lv_obj_set_style_local_line_width(preload, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, 8);
}

/***
** 日期: 2022-04-29 15:51
** 作者: leo.liu
** 函数作用：格式化SD卡的返回按钮
** 返回参数说明：
***/
static void setting_storage_format_sd_dialog_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
}

/***
** 日期: 2022-04-29 15:52
** 作者: leo.liu
** 函数作用：格式化SD卡的确认按钮
** 返回参数说明：
***/
static void setting_storage_format_sd_dialog_confirm_btn_up(lv_obj_t *obj)
{
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	if (cont == NULL)
	{
		return;
	}
	lv_obj_clean(cont);

	setting_animation_task_create(cont, 1);
}

/***
** 日期: 2022-05-04 08:39
** 作者: leo.liu
** 函数作用：格式化sd卡选项按钮点击事件
** 返回参数说明：
***/
static void setting_storage_format_sd_btn_up(lv_obj_t *obj)
{
	if (media_sdcard_insert_check() == false)
	{
		return;
	}

	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	if (cont != NULL)
	{
		return;
	}

	cont = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(cont, setting_storage_format_sd_dialog_cancel_btn_up, setting_storage_format_sd_dialog_confirm_btn_up);

	setting_msgdialog_msg_create(cont, layout_setting_storage_string_get(SETTING_STORAGE_LANG_ID_FORMAT_SD));
	lv_obj_set_style_local_value_ofs_y(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -30);
	lv_obj_set_style_local_value_font(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
}

/***
** 日期: 2022-04-29 15:41
** 作者: leo.liu
** 函数作用：创建格式话SD卡按钮
** 返回参数说明：
***/
static void setting_storage_format_sd_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_storage_format_sd_btn_up);
	setting_right_btn_base_create(NULL, 353, 94, 622, 90,
				      layout_setting_storage_string_get(SETTING_STORAGE_LANG_ID_FORMAT_SD),
				      NULL, &click_data, 0);
}

/***
** 日期: 2022-05-04 08:40
** 作者: leo.liu
** 函数作用：删除选项选中框点击事件
** 返回参数说明：
***/
static void setting_storage_delete_chcekbox_up(lv_obj_t *obj)
{
	if ((obj->obj_id == LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_1_ID) && (media_sdcard_insert_check() == false))
	{
		lv_checkbox_set_checked(obj, false);
	}
}

/***
** 日期: 2022-04-29 15:51
** 作者: leo.liu
** 函数作用：文件删除消息框的返回按钮
** 返回参数说明：
***/
static void setting_storage_del_file_dialog_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
}

/***
** 日期: 2022-05-04 09:39
** 作者: leo.liu
** 函数作用：删除消息框选中框类型获取
** 返回参数说明：
***/
static int setting_storage_delete_file_checkbox_select_type_get(void)
{
	lv_obj_t *msg_obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	if (msg_obj == NULL)
	{
		printf("find LAYOUT_SETTING_STORAGE_OBJ_MSG_ID failed \n");
		return -1;
	}

	lv_obj_t *checkbox_1 = lv_obj_get_child_form_id(msg_obj, LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_1_ID);
	if (checkbox_1 == NULL)
	{
		printf("find LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_1_ID failed \n");
		return -1;
	}

	lv_obj_t *checkbox_2 = lv_obj_get_child_form_id(msg_obj, LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_2_ID);
	if (checkbox_2 == NULL)
	{
		printf("find LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_2_ID failed \n");
		return -1;
	}

	int value = 0;
	if (lv_checkbox_get_state(checkbox_1) == LV_BTN_STATE_CHECKED_RELEASED)
	{
		value = 3;
	}

	if (lv_checkbox_get_state(checkbox_2) == LV_BTN_STATE_CHECKED_RELEASED)
	{
		value = value == 3 ? 4 : 2;
	}

	return value;
}

/***
** 日期: 2022-04-29 17:28
** 作者: leo.liu
** 函数作用：文件删除消息框的确认按钮
** 返回参数说明：
***/
static void setting_storage_del_file_dialog_confirm_btn_up(lv_obj_t *obj)
{
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	if (cont == NULL)
	{
		return;
	}

	int value = setting_storage_delete_file_checkbox_select_type_get();
	if (value <= 0)
	{
		return;
	}

	lv_obj_clean(cont);

	setting_animation_task_create(cont, value);
}

/***
** 日期: 2022-04-29 16:49
** 作者: leo.liu
** 函数作用:删除按钮消息框的按键矩阵创建
** 返回参数说明：
***/
static bool setting_storage_delete_chcekbox_create(lv_obj_t *prarent)
{
	lv_obj_t *checkbox = lv_checkbox_create(prarent, NULL);
	if (checkbox == NULL)
	{
		printf("storage recrod create checkbox failed \n");
		return false;
	}
	lv_obj_set_id(checkbox, LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_1_ID);
	lv_obj_set_pos(checkbox, 66, 74);
	lv_obj_set_size(checkbox, 376, 50);
	static rom_bin_info image_off = rom_bin_info_get(ROM_UI_CHECK_BTN_003_PNG);
	lv_obj_set_style_local_pattern_image(checkbox, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, &image_off);
	lv_obj_set_style_local_pattern_align(checkbox, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
	static rom_bin_info image_on = rom_bin_info_get(ROM_UI_CHECK_BTN_004_PNG);
	lv_obj_set_style_local_pattern_image(checkbox, LV_CHECKBOX_PART_BG, LV_STATE_CHECKED, &image_on);
	lv_obj_set_style_local_pattern_align(checkbox, LV_CHECKBOX_PART_BG, LV_STATE_CHECKED, LV_ALIGN_IN_LEFT_MID);
	
	lv_checkbox_set_text(checkbox, language_common_string_get(LANG_COMMON_ID_VIDEO));
	static obj_click_data click_data = obj_click_data_up_create(setting_storage_delete_chcekbox_up);
	obj_click_event_listen(checkbox, &click_data);

	lv_obj_t *checkbox_2 = lv_checkbox_create(prarent, checkbox);
	lv_obj_set_id(checkbox_2, LAYOUT_SETTING_STORAGE_OBJ_CHCKBOX_2_ID);
	lv_obj_set_pos(checkbox_2, 66, 142);
	lv_checkbox_set_text(checkbox_2, language_common_string_get(LANG_COMMON_ID_PHOTO));
	obj_click_event_listen(checkbox_2, &click_data);
	return true;
}

/***
** 日期: 2022-05-04 08:41
** 作者: leo.liu
** 函数作用：删除选项按钮事件
** 返回参数说明：
***/
static void setting_storage_delete_file_btn_up(lv_obj_t *obj)
{
	/***** 消息框存在，则不再做任何处理 *****/
	lv_obj_t *msg_obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	if (msg_obj != NULL)
	{
		return;
	}

	msg_obj = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	setting_msgdialog_msg_confirm_and_cancel_btn_create(msg_obj, setting_storage_del_file_dialog_cancel_btn_up, setting_storage_del_file_dialog_confirm_btn_up);

	setting_storage_delete_chcekbox_create(msg_obj);
}
/***
** 日期: 2022-04-29 15:45
** 作者: leo.liu
** 函数作用：文件删除按钮创建
** 返回参数说明：
***/
static void setting_storage_delete_file_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_storage_delete_file_btn_up);
	setting_right_btn_base_create(NULL, 353, 184, 622, 90,
				      layout_setting_storage_string_get(SETTING_STORAGE_LANG_ID_DELETION_FILE),
				      NULL, &click_data, 0);
}

/***
** 日期: 2022-04-29 15:45
** 作者: leo.liu
** 函数作用：sd卡储存容量显示
** 返回参数说明：
***/
static void setting_storage_sd_capacity_btn_create(void)
{
	static char str1[64] = {0};
	unsigned long  bavail,disk_all_space;
	get_SD_space(&bavail,&disk_all_space);
	float remain = 0,all = 0;
	remain = bavail/(float)1024;
	all = disk_all_space/(float)1024;
	if(!media_sdcard_insert_check())
	{
		remain = 0;
		all = 0;
	}
	memset(str1,0,sizeof(str1));
	sprintf(str1, "%.2fGB/%0.2fGB",remain,all);

	setting_right_btn_base_create(NULL, 353, 274, 622, 90,
						layout_setting_storage_string_get(SETTING_STORAGE_LANG_ID_SD),
						str1, NULL, LAYOUT_SETTING_STORAGE_OBJ_SD_CAPACITY_ID);
}

static void layout_home_sd_capacity_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_SD_CAPACITY_ID);
	static char str1[64] = {0};
	unsigned long  bavail,disk_all_space;
	get_SD_space(&bavail,&disk_all_space);
	float remain = 0,all = 0;
	remain = bavail/(float)1024;
	all = disk_all_space/(float)1024;
	if(!media_sdcard_insert_check())
	{
		remain = 0;
		all = 0;
	}
	memset(str1,0,sizeof(str1));
	printf("============%.2f========%.2f=======\n",remain,all);
	sprintf(str1, "%.2fGB/%0.2fGB",remain,all);
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, str1);
	// if(media_sdcard_insert_check() == true)
	// {
	// 	copy_flash_photo_to_sd();
	// }
}
/***
** 日期: 2022-04-29 15:45
** 作者: leo.liu
** 函数作用：falsh储存容量显示
** 返回参数说明：
***/
static void setting_storage_falsh_capacity_btn_create(void)
{
	static char str1[64] = {0};
	unsigned long  bavail,disk_all_space;
	get_falsh_space(&bavail,&disk_all_space);
	float remain = 0,all = 0;
	remain = bavail;
	all = disk_all_space;
	// if(!media_sdcard_insert_check())
	// {
	// 	remain = 0;
	// 	all = 0;
	// }
	memset(str1,0,sizeof(str1));
	sprintf(str1, "%0.1fMB/%0.1fMB",remain,all);

	setting_right_btn_base_create(NULL, 353, 364, 622, 90,
						layout_setting_storage_string_get(SETTING_STORAGE_LANG_ID_FLASH),
						str1, NULL, 0);
}





/***
** 日期: 2022-04-29 15:52
** 作者: leo.liu
** 函数作用：将falsh照片储存到SD卡中的确认按钮
** 返回参数说明：
***/
static void setting_storage_format_to_sd_dialog_confirm_btn_up(lv_obj_t *obj)
{

	if (media_sdcard_insert_check() == false)
	{
		message_window_pops(layout_setting_ring_string_get(COMMON_NO_SD),0,0);
		return;
	}
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	if (cont == NULL)
	{
		return;
	}
	lv_obj_clean(cont);

	setting_animation_task_create(cont, 5);
}


/***
** 日期: 2022-05-04 08:39
** 作者: leo.liu
** 函数作用：格式化sd卡选项按钮点击事件
** 返回参数说明：
***/
static void setting_storage_cp_flash_to_sd_btn_up(lv_obj_t *obj)
{

	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);
	if (cont != NULL)
	{
		return;
	}

	cont = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_STORAGE_OBJ_MSG_ID);

	setting_msgdialog_msg_confirm_and_cancel_btn_create(cont, setting_storage_format_sd_dialog_cancel_btn_up, setting_storage_format_to_sd_dialog_confirm_btn_up);

	setting_msgdialog_msg_create(cont, language_common_string_get(LANG_COMMON_ID_CONFIRM));
	lv_obj_set_style_local_value_ofs_y(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, -30);
	lv_obj_set_style_local_value_font(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
}


/***
** 日期: 2022-04-29 15:45
** 作者: leo.liu
** 函数作用：将falsh照片储存到SD卡中并且进行显示
** 返回参数说明：
***/
static void setting_storage_falsh_photo_to_sd_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_storage_cp_flash_to_sd_btn_up);
	setting_right_btn_base_create(NULL, 353, 364+90, 622, 90,
				      layout_setting_storage_string_get(SETTING_STORAGE_LANG_ID_FLASH_PHOTO_TO_SD),
				      NULL, &click_data, 0);
}



static void LAYOUT_ENTER_FUNC(setting_storage)
{
	setting_cancel_btn_create(NULL);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	setting_main_btn_create(2);

	setting_storage_format_sd_btn_create();

	setting_storage_delete_file_btn_create();

	setting_storage_sd_capacity_btn_create();

	setting_storage_falsh_capacity_btn_create();

	setting_storage_falsh_photo_to_sd_btn_create();

	layout_sd_state_callback_register(layout_home_sd_capacity_display);

}

static void LAYOUT_QUIT_FUNC(setting_storage)
{
	layout_sd_state_callback_register(NULL);
}

CREATE_LAYOUT(setting_storage);