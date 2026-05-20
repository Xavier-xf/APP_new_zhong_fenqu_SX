// #include "layout_define.h"


// #define LAYOUT_DOCUMENT_MOTION_BTN_ID  		0x01
// #define LAYOUT_DOCUMENT_MONITION_BTN_ID 	0x02
// #define LAYOUT_DOCUMENT_MOTION_NEW_ID 		0x03
// #define LAYOUT_DOCUMENT_MONITION_NEW_ID 	0x04


// static lv_obj_t *document_btn_create(int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *image, const char *string, int obj_id)
// {
// 	lv_obj_t *btn = lv_btn_create(lv_scr_act(), NULL);
// 	if (btn == NULL)
// 	{
// 		printf("home monitor btn create failed \n");
// 		return false;
// 	}
// 	if (obj_id > 0)
// 	{
// 		lv_obj_set_id(btn, obj_id);
// 	}
// 	lv_obj_set_pos(btn, x, y);
// 	lv_obj_set_size(btn, w, h);

// 	lv_obj_set_style_local_bg_opa(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
// 	if (image != NULL)
// 	{
// 		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, image);
// 		lv_obj_set_style_local_pattern_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_TOP_MID);
// 	}

// 	if (string != NULL)
// 	{
// 		lv_obj_set_style_local_value_str(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, string);
// 		lv_obj_set_style_local_value_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
// 	}

// 	if (click_data != NULL)
// 	{
// 		obj_click_event_listen(btn, click_data);
// 	}

// 	return btn;
// }



// /***
// **   日期:2022-05-26 11:45:03
// **   作者: leo.liu
// **   函数作用：创建新文件图标
// **   参数说明:
// ***/
// static void document_motion_new_icon_create(void)
// {
// 	lv_obj_t * img1 = lv_obj_create(lv_scr_act(), NULL);
// 	lv_obj_set_style_local_bg_color(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFF0000));
// 	lv_obj_set_style_local_bg_opa(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
// 	lv_obj_set_style_local_radius(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 14);
// 	lv_obj_set_pos(img1, 400, 225);
// 	lv_obj_set_size(img1, 28, 28);
// 	lv_obj_set_id(img1, LAYOUT_DOCUMENT_MOTION_NEW_ID);
	
//     static char str[3] = {0};
// 	int new_total = 0;
// 	if(media_sdcard_insert_check() == true){
// 		media_file_total_get(FILE_TYPE_SD_MOTION , NULL, &new_total);
// 	}else{
// 		media_file_total_get(FILE_TYPE_FLASH_MOTOIN_PHOTO, NULL, &new_total);
// 	}

// 	if((new_total) > 99)
// 	{
// 		sprintf(str, "N");
// 	}
// 	else
// 	{
// 		sprintf(str, "%d", new_total);
// 	}
// 	lv_obj_set_style_local_value_str(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, str);
// 	lv_obj_set_style_local_value_align(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
// 	lv_obj_set_style_local_value_color(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
// 	lv_obj_set_style_local_value_font(img1, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));

// }

// static void document_monitor_new_icon_create(void)
// {
// 	lv_obj_t * img = lv_obj_create(lv_scr_act(), NULL);
// 	lv_obj_set_style_local_bg_color(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFF0000));
// 	lv_obj_set_style_local_bg_opa(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
// 	lv_obj_set_style_local_radius(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 14);
// 	lv_obj_set_pos(img, 700, 225);
// 	lv_obj_set_size(img, 28, 28);
// 	lv_obj_set_id(img, LAYOUT_DOCUMENT_MONITION_NEW_ID);
	
//     static char str[3] = {0};
// 	int new_total = 0;
// 	if(media_sdcard_insert_check() == true){
// 		media_file_total_get(FILE_TYPE_SD_MONITOR , NULL, &new_total);
// 	}else{
// 		media_file_total_get(FILE_TYPE_FLASH_MONITOR_PHOTO, NULL, &new_total);
// 	}

// 	if((new_total) > 99)
// 	{
// 		sprintf(str, "N");
// 	}
// 	else
// 	{
// 		sprintf(str, "%d", new_total);
// 	}
// 	lv_obj_set_style_local_value_str(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, str);
// 	lv_obj_set_style_local_value_align(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_BOTTOM_MID);
// 	lv_obj_set_style_local_value_color(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
// 	lv_obj_set_style_local_value_font(img, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));

// }


// /***
// ** 日期: 2022-04-26 09:41
// ** 作者: leo.liu
// ** 函数作用：监控按钮被点击
// ** 返回参数说明：
// ***/
// static void document_motion_btn_up(lv_obj_t *obj)
// {
// 	layout_motion_playback_parm_init();
// 	goto_layout(pLAYOUT(playback));
// }

// /***
// ** 日期: 2022-04-26 08:18
// ** 作者: leo.liu
// ** 函数作用：文件列表移动侦测图标按钮
// ** 返回参数说明：
// ***/
// static bool document_motion_btn_display(void)
// {
// 	static rom_bin_info image = rom_bin_info_get(ROM_UI_PLAYBACK_MONITOR_PNG);
// 	static obj_click_data click_data = obj_click_data_up_create(document_motion_btn_up);
// 	document_btn_create(326, 225, 100,146, &click_data, &image, layout_home_string_get(HOME_LANG_ID_MONITOR), LAYOUT_DOCUMENT_MOTION_BTN_ID);
// 	return true;
// }



// static void document_monitor_btn_up(lv_obj_t *obj)
// {
// 	layout_monitor_playback_parm_init();
// 	goto_layout(pLAYOUT(playback));
// }



// /***
// ** 日期: 2022-04-26 08:18
// ** 作者: leo.liu
// ** 函数作用：文件列表监控图标按钮
// ** 返回参数说明：
// ***/
// static bool document_monitor_btn_display(void)
// {
// 	static rom_bin_info image = rom_bin_info_get(ROM_UI_PLAYBACK_MOTION_PNG);
// 	static obj_click_data click_data = obj_click_data_up_create(document_monitor_btn_up);
// 	document_btn_create(628, 225, 100,146, &click_data, &image, layout_home_string_get(HOME_LANG_ID_MONITOR), LAYOUT_DOCUMENT_MONITION_BTN_ID);
// 	return true;
// }



// static void layout_document_cancel_btn_up(lv_obj_t *obj)
// {
// 	goto_layout(pLAYOUT(home));
// }

// static void LAYOUT_ENTER_FUNC(document_list)
// {
// 	setting_cancel_btn_create(layout_document_cancel_btn_up);
// 	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));
//     document_motion_btn_display();
//     document_monitor_btn_display();
// 	document_motion_new_icon_create();
// 	document_monitor_new_icon_create();

// }
// static void LAYOUT_QUIT_FUNC(document_list)
// {
	
// }

// CREATE_LAYOUT(document_list);