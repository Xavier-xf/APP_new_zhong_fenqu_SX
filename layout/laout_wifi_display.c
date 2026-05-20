#include "layout_define.h"
#include "wifi_api.h"
#include "tuya/tuya_uuid_and_key.h"

typedef enum
{
	wifi_display_scr_act_obj_id_conneted_cont,
	wifi_display_scr_act_obj_id_add_cont,
	wifi_display_scr_act_obj_id_serch_cont,
} wifi_display_scr_act_obj_id;


static char wifi_name[128] = {0};

/***
**   日期:2022-06-06 09:27:55
**   作者: leo.liu
**   函数作用：wifi用户创建
**   参数说明:
***/
static lv_obj_t *wifi_display_wifi_btn_base_create(lv_obj_t *parent, int y, const wifi_info *pinfo, obj_click_data *click_data, rom_bin_info *img, int obj_id)
{
	if (pinfo == NULL)
	{
		return NULL;
	}
	lv_obj_t *btn = lv_obj_create(parent, NULL);
	lv_obj_set_pos(btn, 0, y);
	lv_obj_set_size(btn, 1168, 90);
	if (obj_id >= 0)
	{
		lv_obj_set_id(btn, obj_id);
	}
	static rom_bin_info wifi_img_free_img_1 = rom_bin_info_get(ROM_UI_WIFI_004_PNG);
	static rom_bin_info wifi_img_free_img_2 = rom_bin_info_get(ROM_UI_WIFI_003_PNG);
	static rom_bin_info wifi_img_free_img_3 = rom_bin_info_get(ROM_UI_WIFI_002_PNG);
	static rom_bin_info wifi_img_free_img_4 = rom_bin_info_get(ROM_UI_WIFI_001_PNG);
	// static rom_bin_info wifi_img_free_img_5 = rom_bin_info_get(ROM_UI_WIFI_001_PNG);

	static rom_bin_info wifi_img_pwd_img_1 = rom_bin_info_get(ROM_UI_WIFI_N_004_PNG);
	static rom_bin_info wifi_img_pwd_img_2 = rom_bin_info_get(ROM_UI_WIFI_N_003_PNG);
	static rom_bin_info wifi_img_pwd_img_3 = rom_bin_info_get(ROM_UI_WIFI_N_002_PNG);
	static rom_bin_info wifi_img_pwd_img_4 = rom_bin_info_get(ROM_UI_WIFI_N_001_PNG);
	// static rom_bin_info wifi_img_pwd_img_5 = rom_bin_info_get(ROM_UI_WIFI_001_PNG);

	lv_obj_set_style_local_pattern_align(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
	/*****  显示wifi图标 *****/
	// if (abs(pinfo->db) < 55)
	// {
	// 	lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, pinfo->free ? &wifi_img_free_img_5 : &wifi_img_pwd_img_5);
	// }
	if (abs(pinfo->db) < 66)
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, pinfo->free ? &wifi_img_free_img_4 : &wifi_img_pwd_img_4);
	}
	else if (abs(pinfo->db) < 88)
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, pinfo->free ? &wifi_img_free_img_3 : &wifi_img_pwd_img_3);
	}
	else if (abs(pinfo->db) < 100)
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, pinfo->free ? &wifi_img_free_img_2 : &wifi_img_pwd_img_2);
	}
	else
	{
		lv_obj_set_style_local_pattern_image(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, pinfo->free ? &wifi_img_free_img_1 : &wifi_img_pwd_img_1);
	}

	/*****  显示wifi名字 *****/
	lv_obj_t *label = lv_label_create(btn, NULL);
	lv_label_set_long_mode(label, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(label, 64, 30);
	lv_obj_set_size(label, 506, 54);
	lv_label_set_align(label, LV_LABEL_ALIGN_LEFT);
	lv_label_set_text(label, pinfo->name);
	lv_obj_set_click(label, false);

	/***** 设置下底边框 *****/
	lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 2);
	lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_FOCUSED, lv_color_make(0xFF, 0xFF, 0xFF));
	lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x32, 0x32, 0x37));
	lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x22, 0x22, 0x27));
	lv_obj_set_style_local_border_side(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);

	if (click_data != NULL)
	{
		obj_click_event_listen(btn, click_data);
	}
	// pinfo->free = 
	if (img != NULL)
	{
		lv_obj_t *obj = lv_obj_create(btn, NULL);
		lv_obj_set_pos(obj, 530, 10);
		lv_obj_set_size(obj, 48, 48);
		lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, img);
		lv_obj_set_click(obj, false);
	}
	return btn;
}

/***
**   日期:2022-06-06 08:18:01
**   作者: leo.liu
**   函数作用：返回到layout setting record
**   参数说明:
***/
static void layout_wifi_display_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_record));
}

static void layout_wifi_enable_btn_up(lv_obj_t *obj)
{
	bool state = lv_checkbox_is_checked(obj);
	user_data_get()->wifi_enable = state;
	user_data_save();
	if (state)
	{
		wifi_device_open();
	}
	else
	{
		wifi_device_close();
	}
	goto_layout(pLAYOUT(wifi_display));
}

/*************************************************************************
 * @brief  wifi使能按钮和开关
 * @date   2022-08-26 13:54
 * @author xiaoele
 **************************************************************************/
static bool layout_wifi_enable_btn_create(void)
{
	// if(OwnID != 1)
	// {
	// 	user_data_get()->wifi_enable = 0;
	// }
	static obj_click_data click_data = obj_click_data_up_create(layout_wifi_enable_btn_up);
	setting_sub_btn_base_create(NULL, 346, 77, 622, 68,
				    layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_WIFI),
				    &click_data,
				    user_data_get()->wifi_enable, 3);

	unsigned char ip_addr[64] = {0};
	if (wifi_device_hwadders_get(ip_addr) == false)
	{
		printf("get wifi hw addres failed \n");
		return false;
	}

	return true;
}


static void layout_wifi_conneted_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(wifi_info));
}

/***
**   日期:2022-06-06 09:18:59
**   作者: leo.liu
**   函数作用：创建已连接的wifi
**   参数说明:
***/
static void layout_wifi_conneted_btn_create(void)
{
	/*****  创建一个容器 *****/
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, wifi_display_scr_act_obj_id_conneted_cont);
	lv_obj_set_pos(cont, 346, 146);
	lv_obj_set_size(cont, 622, 120);

	/* connected network 文字*/
	lv_obj_t *obj = lv_obj_create(cont, NULL);
	lv_obj_set_pos(obj, 0, 4);
	lv_obj_set_size(obj, 154, 34);
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
	// lv_obj_align(obj,cont,LV_ALIGN_IN_TOP_LEFT,-30,0);
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_NETWORK_CONNECTED));
	
	/* 已连接的wifi的名称 */
	wifi_info info;
	info.db = 0;
	unsigned char ip[32] = {0};
	static bool connected = false;
	memset(info.name, 0, sizeof(info.name));
	wifi_device_connection_stauts((unsigned char *)(info.name), &info.free, ip, &connected, NULL);
	static obj_click_data click_data = obj_click_data_up_create(layout_wifi_conneted_btn_up);
	char serial[64] = {0};
	if (tuya_serial_number_get(serial) == true)
	{
		static rom_bin_info img = rom_bin_info_get(ROM_UI_TUYA_WIFI_PNG);
		wifi_display_wifi_btn_base_create(cont, 20, &info, &click_data, &img, -1);
	}
	else
	{
		wifi_display_wifi_btn_base_create(cont, 20, &info, &click_data, NULL, -1);
	}

	if ((user_data_get()->wifi_enable == true) && (connected == true))
	{
		lv_obj_set_hidden(cont, false);
	}
	else
	{
		lv_obj_set_hidden(cont, true);
	}
	memcpy(wifi_name, info.name, 128);
	cont->user_data = &connected;
}
/***
**   日期:2022-06-06 09:18:25
**   作者: leo.liu
**   函数作用：搜索到的wifi创建
**   参数说明:
***/
static lv_obj_t *wifi_display_page_obj_create(void)
{
	lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);
	if (page == NULL)
	{
		printf("create page failed \n");
		return NULL;
	}
	lv_obj_set_id(page, wifi_display_scr_act_obj_id_serch_cont);

	lv_obj_set_pos(page, 346-24, 252 + 60);
	lv_obj_set_size(page, 630, 280);
	lv_page_set_scrollable_fit4(page, LV_FIT_NONE, LV_FIT_NONE, LV_FIT_MAX, LV_FIT_MAX);
	return page;
}

static void wifi_display_add_wifi_btn_up(lv_obj_t *obj)
{
	layout_wifi_input_mode_set(0);
	goto_layout(pLAYOUT(wifi_input));
}
/***
**   日期:2022-06-06 09:18:18
**   作者: leo.liu
**   函数作用：添加wifi
**   参数说明:
***/
static bool wifi_display_add_wifi_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	// lv_obj_t *conneted_cont = lv_obj_get_child_form_id(lv_scr_act(), wifi_display_scr_act_obj_id_conneted_cont);
	// int y = lv_obj_get_hidden(conneted_cont) ? 100 : 230;
	lv_obj_set_pos(obj, 346, 252);
	lv_obj_set_size(obj, 600, 68);
	lv_obj_set_id(obj, wifi_display_scr_act_obj_id_add_cont);
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_wifi_string_get(LAYOUT_WIFI_LANGUAGE_ID_ADD_WIFI));	/* Add Wi-Fi network */

	static obj_click_data click_data = obj_click_data_up_create(wifi_display_add_wifi_btn_up);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_6_ADDWIFI_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	lv_obj_set_style_local_pattern_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_RIGHT_MID);

	// lv_obj_set_style_local_pattern_ofs_x(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,20);
	obj_click_event_listen(obj, &click_data);
	return true;
}

static void wifi_display_scan_wifi_btn_up(lv_obj_t *obj)
{
	// extern bool wifi_free_flag;
	wifi_info *pinfo = NULL;
	int total = 0;
	if (wifi_device_scanf_info_get(&pinfo, &total) == false)
	{
		return;
	}
	for (int i = 0; i < total; i++)
	{
		if (obj->obj_id == i)
		{
			layout_wifi_input_mode_set(1);
			layout_wifi_input_set_user_name((unsigned char *)pinfo[obj->obj_id].name);
			printf("====================><><><><><pinfo[obj->obj_id].name=:%s\n",pinfo[obj->obj_id].name);
			// if(pinfo[obj->obj_id].free == true)
			// {
			// 	printf("========--------wuwifi\n");
			// 	wifi_free_flag = true;
			// 	extern void wifi_input_msg_dialog_display(void);
			// 	wifi_input_msg_dialog_display();
			// }
			// else
			// {
				// wifi_free_flag = false;
				// printf("========--------wifi\n");
				goto_layout(pLAYOUT(wifi_input));
			// }
			return;
		}
	}
}
/***
**   日期:2022-06-06 11:27:49
**   作者: leo.liu
**   函数作用：创建扫描得wifi
**   参数说明:
***/
static bool wifi_display_scan_wifi_btn_create(lv_obj_t *parent)
{
	wifi_info *pinfo = NULL;
	int total = 0;
	if (wifi_device_scanf_info_get(&pinfo, &total) == false)
	{
		return false;
	}
	static obj_click_data click_data = obj_click_data_up_create(wifi_display_scan_wifi_btn_up);
	int m = 0;
	for (int i = 0; i < total; i++)
	{
		printf("======================>>>>>wifi_name[%s]>>>>>>pinfo->name[%s]\n",wifi_name,pinfo[i].name);
		if(strcmp(wifi_name,pinfo[i].name) == 0)
		{
			continue;
		}
		
		lv_obj_t *obj = wifi_display_wifi_btn_base_create(parent, 80 * m, &pinfo[i], &click_data, NULL, i);
		lv_page_glue_obj(obj, true);
		m += 1;
	}
	return true;
}

/***
**   日期:2022-06-06 14:15:42
**   作者: leo.liu
**   函数作用：依据状态显示和隐藏容器
**   参数说明:
***/
static void layout_wifi_display_cont_enable(void)
{
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), wifi_display_scr_act_obj_id_add_cont);
	lv_obj_set_hidden(cont, user_data_get()->wifi_enable ? false : true);
	cont = lv_obj_get_child_form_id(lv_scr_act(), wifi_display_scr_act_obj_id_serch_cont);
	lv_obj_set_hidden(cont, user_data_get()->wifi_enable ? false : true);
}

/***
**   日期:2022-06-06 14:24:52
**   作者: leo.liu
**   函数作用：实时检测wifi状态
**   参数说明:
***/
static void wifi_display_state_task(lv_task_t *task_t)
{
	lv_obj_t *conneted_cont = lv_obj_get_child_form_id(lv_scr_act(), wifi_display_scr_act_obj_id_conneted_cont);
	bool state = *((bool *)conneted_cont->user_data);
	bool connected = false;
	wifi_device_connection_stauts(NULL, NULL, NULL, &connected, NULL);
	if (connected != state)
	{
		if(connected == true)
		{
			tuya_api_reconnect_network();
		}
		goto_layout(pLAYOUT(wifi_display));
	}
}
static void LAYOUT_ENTER_FUNC(wifi_display)
{
	if (user_data_get()->wifi_enable == true)
	{
		wifi_device_scanf();
	}

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));
	setting_cancel_btn_create(layout_wifi_display_cancel_btn_up);
	setting_main_btn_create(6);
	layout_wifi_enable_btn_create();
	layout_wifi_conneted_btn_create();
	wifi_display_add_wifi_btn_create();
	
	lv_obj_t *page = wifi_display_page_obj_create();
	wifi_display_scan_wifi_btn_create(page);

	layout_wifi_display_cont_enable();


	lv_layout_task_create(wifi_display_state_task, 500, LV_TASK_PRIO_MID, NULL);
}
static void LAYOUT_QUIT_FUNC(wifi_display)
{
}
CREATE_LAYOUT(wifi_display)