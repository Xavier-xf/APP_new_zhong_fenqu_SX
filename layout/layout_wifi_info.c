#include "layout_define.h"
#include "wifi_api.h"
#include "lvgl/drive/lv_qrcode.h"
#include "tuya_ipc_api.h"
#include "tuya/tuya_uuid_and_key.h"



#define WIFI_DELETE_ION 1


// lv_task_t *tuya_wifi_info = NULL;
static void layout_wifi_info_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(wifi_display));
}

/***
**   日期:2022-07-04 15:56:41
**   作者: leo.liu
**   函数作用：显示官网二维码
**   参数说明: https://a.smart321.com/commaxcallipvideophone2
***/
static void setting_wifi_info_qrcode_display(lv_task_t *task)
{
	char serial[64] = {0};
	extern char *tuya_qr_code_info_get(void);
	if (tuya_serial_number_get(serial) == false/*|| tuya_qr_code_info_get() == NULL*/ )
	{
		return;
	}
	// if (tuya_api_network_status() != 0x02)
	// {
	// 	return ;
	// }

	// char *qr_code = tuya_qr_code_info_get();
	char qr_code[128] = {0};
	memset(qr_code,0,sizeof(qr_code));
	// printf("============[%d]=======[%s]==========\n"user_data_get()->tuya_qrcode_info,qr_code);
	memcpy(qr_code, user_data_get()->tuya_qrcode_info, 128);
	// tuya_ipc_set_region(REGION_CN);
	// int ret = tuya_ipc_get_qrcode(NULL, qr_code, 128);
	printf("wifi display 33333======= [%d]:[%s]\n",strlen(qr_code), qr_code);
	if(qr_code != 0)
	{
		static rom_bin_info img = rom_bin_raw_get();
		rom_bin_raw_init(img, video_input_resident_buffer_get(NULL), 240, 240);
		lv_obj_t *qr = lv_qrcode_create(lv_scr_act(), &img, 240, lv_color_hex(0x00), lv_color_hex(0xFFFFFF));
		lv_obj_set_pos(qr, 392, 280);
		lv_obj_set_style_local_outline_opa(qr, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
		lv_obj_set_style_local_outline_color(qr, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
		lv_obj_set_style_local_outline_width(qr, LV_CANVAS_PART_MAIN, LV_STATE_DEFAULT, 10);
		lv_qrcode_update(qr, qr_code, strlen(qr_code));
		lv_task_del(task);
	}
	


	// lv_qrcode_update(qr, "https://a.smart321.com/commaxcallipvideophone2", strlen("https://a.smart321.com/commaxcallipvideophone2"));
	
	// lv_task_del(tuya_wifi_info);
// #if 0
// 	lv_obj_t *obj = lv_img_create(qr, NULL);
// 	rom_bin_info com_img = rom_bin_info_get(ROM_UI_COMMAX_PNG);
// 	lv_img_set_src(obj, &com_img);
// 	lv_obj_align(obj, qr, LV_ALIGN_CENTER, 0, 0);
// #endif

}



/***
**   日期:2022-05-24 11:12:09
**   作者: leo.liu
**   函数作用：取消删除
**   参数说明:
***/
static void layout_wifi_delete_msg_cancel_btn_up(lv_obj_t *obj)
{
	setting_msgdialog_msg_bg_delete(WIFI_DELETE_ION);
}

/***
**   日期:2022-05-24 11:12:54
**   作者: leo.liu
**   函数作用：确认删除
**   参数说明:
***/
static void layout_wifi_delete_msg_confirm_btn_up(lv_obj_t *obj)
{
	/*恢复初始化设置之后需WiFi重连*/
	wifi_device_user_to_temp(NULL,  NULL );
	wifi_device_temp_conneting();
	wifi_device_tmp_sync();
	setting_msgdialog_msg_bg_delete(WIFI_DELETE_ION);
	goto_layout(pLAYOUT(wifi_display));
}


/***
**   日期:2022-05-24 14:12:15
**   作者: leo.liu
**   函数作用：删除按钮创建
**   参数说明:
***/
static void layout_video_delete_btn_up(lv_obj_t *obj)
{

	if (setting_msgdialog_msg_bg_delete(WIFI_DELETE_ION) == true)
	{
		return;
	}
	lv_obj_t *dialog = setting_msgdialog_msg_bg_create(WIFI_DELETE_ION);
	setting_msgdialog_msg_confirm_and_cancel_btn_create(dialog, layout_wifi_delete_msg_cancel_btn_up, layout_wifi_delete_msg_confirm_btn_up);
	setting_msgdialog_video_msg_create(dialog, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_PWD_IGNORE));
	// layout_video_full_refresh();
}






static void layout_wifi_info_delete_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 948, 10);
	lv_obj_set_size(obj, 60, 60);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_DELETE_SECURITY_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	static obj_click_data click_data = obj_click_data_up_create(layout_video_delete_btn_up);
	obj_click_event_listen(obj, &click_data);
}


// static void setting_wifi_info_task(lv_task_t *task)
// {

// 	setting_wifi_info_qrcode_display();
// 	lv_task_del(task);
// }


static void LAYOUT_ENTER_FUNC(wifi_info)
{
	setting_cancel_btn_create(layout_wifi_info_cancel_btn_up);

	static unsigned char user[64] = {0};
	memset(user, 0, sizeof(user));
	static unsigned char flags[64] = {0};
	memset(flags, 0, sizeof(flags));
	static unsigned char ip[64] = {0};
	memset(ip, 0, sizeof(ip));
	wifi_device_connection_stauts(user, NULL, ip, NULL, flags);
	setting_head_label_create((char *)user);

	setting_right_btn_base_create(NULL, 30, 70, 934, 72,
								  layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_WIFI_SECURITY),
								  (char *)flags,
								  NULL,
								  0);

	setting_right_btn_base_create(NULL, 30, 70 + 72, 934, 72,
								  layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_IP_ADDRES),
								  (char *)ip,
								  NULL,
								  0);

	// setting_wifi_info_qrcode_display();
	layout_wifi_info_delete_btn_create();
	lv_task_t *task = lv_layout_task_create(setting_wifi_info_qrcode_display,1000, LV_TASK_PRIO_MID, NULL);
	lv_task_ready(task);
}
static void LAYOUT_QUIT_FUNC(wifi_info)
{
	
}

CREATE_LAYOUT(wifi_info);