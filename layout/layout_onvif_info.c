/*******************************************************************
 * @Descripttion   :
 * @version        : 1.0.0
 * @Author         : wxj
 * @Date           : 2023-11-15 13:58
 * @LastEditTime   : 2023-11-20 10:53
 *******************************************************************/
#include "layout_define.h"

enum
{
	IPC_INFO_PAGE_ID,
	LAYOUT_SETTING_ETC_OBJ_MSG_ID
};

static int info_index = -1;
void onvif_info_index_set(int num)
{
	info_index = num;
}

static void onvif_info_back_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(onvif_search));
}

/*************************************************************************
 * @brief  设置界面 长条（占屏幕宽度大小）
 * @date   2022-11-05 15:48
 * @author xiaoele
 * @param  num 第几个
 * @param  main_string 主要控件显示的文本
 * @param  main_id	主要空间的ID
 * @param  sub_string
 * @param  show_arr	是否显示右边的箭头
 * @param  type	类型	SETTING_SUB_NONE
 * @param  click_cb	点击的回调
 **************************************************************************/
lv_obj_t *setting_long_obj_template(lv_obj_t *parent, int num, const char *main_string, int main_id, const char *sub_string, int sub_id, bool show_arr, int type, obj_click_data *click_cb)
{
	lv_obj_t *obj = lv_obj_create(parent == NULL ? lv_scr_act() : parent, NULL);
	lv_obj_set_size(obj, LV_HOR_RES_MAX - 56 * 2, 70);
	lv_obj_align_x(obj, NULL, LV_ALIGN_IN_TOP_MID, 0);
	lv_obj_set_y(obj, 75 * num);
	lv_page_glue_obj(obj, true);
	lv_obj_set_ext_click_area(obj, 16, 16, 0, 0);

	if (main_id > 0)
		lv_obj_set_id(obj, main_id);

	/* 点击回调 */
	if (click_cb != NULL)
	{
		obj_click_event_listen(obj, click_cb);
	}

	/* 右边的灰色箭头 */
	if (show_arr)
	{
		lv_obj_t *icon = lv_obj_create(obj, NULL);
		lv_obj_set_click(icon, false);
		lv_obj_set_size(icon, 20, 40);
		lv_obj_align(icon, icon->parent, LV_ALIGN_IN_RIGHT_MID, -22, 0);
		lv_obj_set_auto_realign(icon, true);
		static rom_bin_info img = rom_bin_info_get(ROM_UI_02_BTN_BTN_LIST_RIGHT_PNG);
		lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	}

	/* 底部边框 */
	lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
	lv_obj_set_style_local_border_side(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x404040));
	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));

	/* 按钮的文字 */
	if (main_string != NULL)
	{
		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, main_string);
		lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
		lv_obj_set_style_local_value_ofs_x(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 4);
		lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	}

	if (type == SETTING_SUB_NONE)
	{
		return obj;
	}

	/* 以下控件只能选其一 */
	if ((sub_string != NULL) && type == SETTING_SUB_SWITCH)
	{
		// LOG_B_RED("SW控件不能显示附加文本\n");
	}
	lv_obj_t *sub_obj = NULL;

	/* 子控件 文本  */
	if (type == SETTING_SUB_LABEL)
	{
		sub_obj = lv_label_create(obj, NULL);
		lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -55, 0);
		lv_obj_set_auto_realign(sub_obj, true);
		lv_obj_set_click(sub_obj, false);

		lv_label_set_align(sub_obj, LV_LABEL_ALIGN_RIGHT);
		lv_label_set_long_mode(sub_obj, LV_LABEL_LONG_EXPAND);
		lv_label_set_text(sub_obj, sub_string);
		lv_obj_set_style_local_text_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x66, 0x66, 0x66));
		lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	}
	else if (type == SETTING_SUB_SWITCH)
	{
		// sub_obj = lv_switch_create(obj, NULL);
		// lv_obj_set_click(sub_obj, false);
		// lv_obj_set_size(sub_obj, 51, 26);
		// lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -22, 0);
		// lv_obj_set_style_local_bg_color(sub_obj, LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, lv_color_hex(0x3AC392));
	}
	else if (type == SETTING_SUB_TEXTAREA)
	{
		// sub_obj = lv_textarea_create(obj, NULL);
		// lv_obj_set_size(sub_obj, 260, 60);
		// lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -55, 0);
		// lv_obj_set_auto_realign(sub_obj, true);

		// if(click_cb != NULL)
		// {
		// 	obj_click_event_listen(sub_obj, click_cb);
		// }

		// lv_textarea_set_text(sub_obj, sub_string);
		// lv_textarea_set_text_align(sub_obj, LV_LABEL_ALIGN_RIGHT);
		// lv_textarea_set_cursor_click_pos(sub_obj, false);
		// lv_textarea_set_cursor_hidden(sub_obj, true);
		// lv_textarea_set_one_line(sub_obj, true);
		// lv_textarea_set_max_length(sub_obj, 16);
		// lv_textarea_set_cursor_pos(sub_obj, strlen(sub_string));

		// lv_obj_set_style_local_text_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x66, 0x66, 0x66));
		// lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
		// lv_obj_set_style_local_bg_opa(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_80);
		// lv_obj_set_style_local_bg_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	}

	/* 子控件 ID */
	// if(sub_id > 0)	lv_obj_set_id(sub_obj, sub_id);

	return obj;
}

lv_obj_t *cctv_manage_rtsp_url_create_template(lv_obj_t *parent, int num, const char *main_str, const char *sub_str, obj_click_data *click_cb, int obj_id)
{
	lv_obj_t *cont = lv_cont_create(parent, NULL);
	lv_obj_set_size(cont, 1024 - 56 * 2, 72 * 2);
	lv_obj_align(cont, parent, LV_ALIGN_IN_TOP_MID, 0, 75 * num);

	/* 底部边框 */
	lv_obj_set_style_local_border_width(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
	lv_obj_set_style_local_border_side(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
	lv_obj_set_style_local_border_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
	lv_obj_set_style_local_border_color(cont, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x404040));
	lv_obj_set_style_local_border_color(cont, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));

	lv_obj_t *main_label = lv_label_create(cont, NULL);
	lv_label_set_text(main_label, main_str);
	lv_label_set_align(main_label, LV_LABEL_ALIGN_LEFT);
	lv_label_set_long_mode(main_label, LV_LABEL_LONG_CROP);
	lv_obj_set_style_local_text_font(main_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));

	lv_obj_set_size(main_label, 320, 40);
	lv_obj_align(main_label, cont, LV_ALIGN_IN_TOP_LEFT, 0, 20);

	lv_obj_t *sub_label = lv_label_create(cont, NULL);
	lv_label_set_text(sub_label, sub_str);
	lv_label_set_align(sub_label, LV_LABEL_ALIGN_LEFT);
	lv_label_set_long_mode(sub_label, LV_LABEL_LONG_BREAK);
	lv_obj_set_style_local_text_font(sub_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	lv_obj_set_style_local_text_color(sub_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x66, 0x66, 0x66));

	lv_obj_set_size(sub_label, 1024 - 56 * 2, 60);
	lv_obj_align(sub_label, cont, LV_ALIGN_IN_BOTTOM_LEFT, 0, -20);
	lv_obj_set_auto_realign(sub_label, true);

	obj_click_event_listen(cont, click_cb);
	lv_obj_set_id(sub_label, obj_id);

	// lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
	// lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

	// lv_obj_set_style_local_bg_opa(main_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
	// lv_obj_set_style_local_bg_color(main_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

	// lv_obj_set_style_local_bg_opa(sub_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
	// lv_obj_set_style_local_bg_color(sub_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

	return cont;
}
// /*************************************************************************
//  * @brief  清空结构体所有成员
//  * @date   2023-04-20 09:03
//  * @author xiaole
//  * @return
//  * @param  pta 需要强制转换
//  * @note   struct_clear(( unsigned char *)&user_data_get()->onvif_dev[info_index], sizeof(user_data_get()->onvif_dev));
//  **************************************************************************/
// void struct_clear(unsigned char *pta, int size )
// {
//     while(size>0)
//     {
//         *pta++ = 0;
//         size --;
//     }
// }

// static void delete_confirm_fun(lv_obj_t *obj)
// {

// }
static void setting_etc_msg_reset_system_confirm_btn_up(lv_obj_t *obj)
{
	LOG_WHITE("confirm\n");
	memset(&user_data_get()->onvif_dev[info_index], 0, sizeof(user_onvif_info));

	memset(&onvif_add_discover_info, 0, sizeof(onvif_add_discover_info));

	// network_device_info_get(OWNFLOOR, info_index==0?DEVICE_CCTV_1:DEVICE_CCTV_2)->online = false;
	monitor_valid_channel_set(info_index == 0 ? MON_CH_CCTV3 : MON_CH_CCTV4, false);
	user_data_save();
	goto_layout(pLAYOUT(onvif_search));
}

static void setting_etc_msg_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(onvif_info));
}

static void onvif_info_delete_fun(lv_obj_t *obj)
{
	LOG_WHITE("click\n");
	// lv_obj_t *msgbox = msg_window_with_button(600, 360, NULL, NULL, delete_confirm_fun);
	// lv_obj_set_style_local_value_str(msgbox, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lan_str_get(LAN_CONFIRM_Q));
	/***** 消息框存在，则不再做任何处理 *****/
	lv_obj_t *msg_obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_ETC_OBJ_MSG_ID);
	if (msg_obj != NULL)
	{
		return;
	}

	msg_obj = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_ETC_OBJ_MSG_ID);
	setting_msgdialog_msg_confirm_and_cancel_btn_create(msg_obj, setting_etc_msg_cancel_btn_up, setting_etc_msg_reset_system_confirm_btn_up);

	obj = lv_obj_create(msg_obj, NULL);
	lv_obj_align(obj, msg_obj, LV_ALIGN_CENTER, 0, -30);
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DEL_DEV));
}
static void onvif_info_delete_button_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, onvif_info_delete_fun);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_DELETE_SECURITY_PNG);

	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_size(obj, 60, 60);
	lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_RIGHT, -28, 14);
	lv_obj_set_ext_click_area(obj, 20, 20, 20, 20);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	obj_click_event_listen(obj, &click_cb);
}

static void ipc_camera_dev_brand_create(void)
{
	const char *str = (user_data_get()->onvif_dev[info_index].brand > -1) ? layout_setting_etc_string_get(user_data_get()->onvif_dev[info_index].brand + SETTING_ETC_LANG_ID_BRAND_PAN) : "IPC";
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 0, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_BRAND), -1, str, -1, false, SETTING_SUB_LABEL, NULL);
}

static void onvif_info_dev_ip_create(void)
{
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 1, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_IP_ADDRESS), -1, user_data_get()->onvif_dev[info_index].ip, -1, false, SETTING_SUB_LABEL, NULL);
}

static void onvif_info_dev_user_create(void)
{
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 2, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_ACCOUNT), -1, user_data_get()->onvif_dev[info_index].user, -1, false, SETTING_SUB_LABEL, NULL);
}

static void onvif_info_dev_pswd_create(void)
{
	setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 3, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_PASSWORD), -1, user_data_get()->onvif_dev[info_index].pswd, -1, false, SETTING_SUB_LABEL, NULL);
}

extern void onvif_preview_url_set(const char *url);
extern void onvif_preview_ip_set(const char *ip);
extern void onvif_preview_enter_flag_set(int flag);

static void onvif_info_url_click_func(lv_obj_t *obj)
{
	printf("=================>>>> \n");
	onvif_preview_url_set(lv_label_get_text(lv_obj_get_child(obj,NULL)));
	onvif_preview_ip_set(user_data_get()->onvif_dev[info_index].ip);

	onvif_preview_enter_flag_set(0);
	goto_layout(pLAYOUT(onvif_preview));
}

static void onvif_info_dev_maincode_url_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, onvif_info_url_click_func);
	// lv_obj_t *obj = setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 4, lan_str_get(LAN_MAIN_STREAM), -1, user_data_get()->onvif_dev[info_index].url[0], -1, true, SETTING_SUB_LABEL, &click_cb);

	// lv_obj_t *label = lv_obj_get_child(obj, NULL);
	// lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
	// lv_obj_set_size(label, 500, 40);

	cctv_manage_rtsp_url_create_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 4, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_MAIN_STREAM), user_data_get()->onvif_dev[info_index].url[0], &click_cb, 0);
}

static void onvif_info_dev_subcode_url_create(void)
{
	static obj_click_data click_cb = obj_click_data_create(NULL, onvif_info_url_click_func);
	// lv_obj_t *obj = setting_long_obj_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 6, lan_str_get(LAN_SUB_STREAM), -1, user_data_get()->onvif_dev[info_index].url[1], -1, true, SETTING_SUB_LABEL, &click_cb);
	cctv_manage_rtsp_url_create_template(lv_obj_get_child_form_id(lv_scr_act(), IPC_INFO_PAGE_ID), 6, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SUB_STREAM), user_data_get()->onvif_dev[info_index].url[1], &click_cb, 0);

	// lv_obj_t *label = lv_obj_get_child(obj, NULL);
	// lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
	// lv_obj_set_size(label, 500, 40);
}

static void onvif_info_dev_bottom_btn_func()
{
	// LOG_WHITE("click\n");

	extern void cctv_setting_channel_set(int ch);

	cctv_setting_channel_set(info_index);
	user_onvif_info *pdata = &user_data_get()->onvif_dev[info_index];
    ONVIF_ADD_INFO_STRUCT *pinfo = &onvif_add_discover_info[0];
	memset(pinfo->ip, 0, sizeof(pinfo->ip));
	memset(pinfo->user, 0, sizeof(pinfo->user));
	memset(pinfo->pswd, 0, sizeof(pinfo->pswd));
	memset(pinfo->rtsp_url, 0, sizeof(pinfo->rtsp_url));

	snprintf(pinfo->ip, sizeof(pinfo->ip), pdata->ip);
	snprintf(pinfo->user, sizeof(pinfo->user), pdata->user);
	snprintf(pinfo->pswd, sizeof(pinfo->pswd), pdata->pswd);
	snprintf(pinfo->rtsp_url[0], sizeof(pinfo->rtsp_url[0]), pdata->url[0]);
	snprintf(pinfo->rtsp_url[1], sizeof(pinfo->rtsp_url[1]), pdata->url[1]);
	goto_layout(pLAYOUT(ipc_camera_m_add));
}

static void onvif_info_dev_bottom_btn_create()
{
	static obj_click_data click_cb = obj_click_data_create(NULL, onvif_info_dev_bottom_btn_func);
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);

	lv_obj_set_size(obj, 1024, 70);
	lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
	obj_click_event_listen(obj, &click_cb);

	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_REGISTER_DEV));
	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));
	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x4ED7A6));
}

static void setting_onvif_info_page_create()
{
	lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);

	lv_obj_set_id(page, IPC_INFO_PAGE_ID);
	lv_obj_set_size(page, LV_HOR_RES_MAX - 50 * 2, 75 * 6);
	lv_obj_align(page, NULL, LV_ALIGN_IN_TOP_MID, 0, 75);

	lv_page_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_DRAG);
	lv_obj_set_style_local_pad_top(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_pad_left(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_pad_right(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
	lv_obj_set_style_local_pad_bottom(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
}

static void LAYOUT_ENTER_FUNC(onvif_info)
{
	setting_cancel_btn_create(onvif_info_back_btn_up);

	setting_head_label_create(info_index == 0 ? language_common_ch_string_get(MON_CH_CCTV3) : language_common_ch_string_get(MON_CH_CCTV4));

	// layout_title_label_create(lan_str_get(info_index == 0 ? LAN_CCTV1 : LAN_CCTV2));
	// layout_back_btn_create(onvif_info_back_btn_up);
	onvif_info_delete_button_create();

	// LOG_CYAN("cctv='%d'\n",  info_index);
	// LOG_CYAN("brand='%d'\n",   user_data_get()->onvif_dev[info_index].brand);
	// LOG_CYAN("ip  ='%s'\n",    user_data_get()->onvif_dev[info_index].ip);
	// LOG_CYAN("user='%s'\n",    user_data_get()->onvif_dev[info_index].user);
	// LOG_CYAN("pswd='%s'\n",    user_data_get()->onvif_dev[info_index].pswd);
	// LOG_CYAN("url[0]= '%s'\n", user_data_get()->onvif_dev[info_index].url[0]);
	// LOG_CYAN("url[1]= '%s'\n", user_data_get()->onvif_dev[info_index].url[1]);

	setting_onvif_info_page_create();

	ipc_camera_dev_brand_create();
	onvif_info_dev_ip_create();
	onvif_info_dev_user_create();
	onvif_info_dev_pswd_create();
	onvif_info_dev_maincode_url_create();
	onvif_info_dev_subcode_url_create();
	onvif_info_dev_bottom_btn_create();
}

static void LAYOUT_QUIT_FUNC(onvif_info)
{
}

CREATE_LAYOUT(onvif_info);