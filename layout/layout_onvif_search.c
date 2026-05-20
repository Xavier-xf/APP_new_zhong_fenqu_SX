#include "layout_define.h"
#include "sat_ipcamera.h"
#include "wifi_api.h"

typedef enum
{
    ONVIF_PAGE_OBJ_ID = 0x01,
    ONVIF_PAGE_ADDED_CONT_OBJ_ID,
    ONVIF_PAGE_ADDED_DEVICE_1_ID,
    ONVIF_PAGE_ADDED_DEVICE_2_ID,

    SERACH_NET_SELECT_BTNM_ID,

    ONVIF_PAGE_DISCOVER_CONT_OBJ_ID,
    onvif_search_scr_act_obj_id_msgdialog
} CCTV_MANAGE_OBJ_ID_ENUM;

static lv_task_t *task_t = NULL;
static int serach_net_type = 0; /* [0]-br0 [1]-wlan0 */


static void onvif_search_task(lv_task_t *task)
{
    if (sat_ipcamera_status_get() == true) /* 正在运行/搜索 */
    {
        LOG_WHITE("onvif is running\n");
        return;
    }

    goto_layout(pLAYOUT(onvif_search));
    printf("========[%s:%d]====>>>> ipcamera_online_num:[%d] \n", __func__, __LINE__, sat_ipcamera_online_num_get());
}
void onvif_search_animation_task_create(lv_obj_t *parent)
{
	setting_msgdialog_msg_create(parent, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_SEARCH_DEV));
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
void onvif_search_msg_dialog_display(void)
{
	lv_obj_t *cont = lv_obj_get_child_form_id(lv_scr_act(), onvif_search_scr_act_obj_id_msgdialog);
	if (cont != NULL)
	{
		return;
	}
	cont = setting_msgdialog_msg_bg_create(onvif_search_scr_act_obj_id_msgdialog);
	onvif_search_animation_task_create(cont);
}
static void search_net_select_confirm(lv_obj_t *obj)
{
    // goto_layout(pLAYOUT(onvif_input));
    // return;
    printf("+===================>>> \n");
    if (task_t == NULL)
    {
        sat_ipcamera_device_discover_search(0);
        onvif_search_msg_dialog_display();
        task_t = lv_layout_task_create(onvif_search_task, 500, LV_TASK_PRIO_MID, NULL);
    }
}



static lv_obj_t *onvif_serach_option_create(lv_obj_t *parent, int cont, int obj_id, const char *main_str, const char *IP, obj_click_data *click_cb)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_page_glue_obj(obj, true);
	lv_obj_set_size(obj, 1024 -56*2, 72);
	lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 75*cont);

	if(obj_id > 0)	lv_obj_set_id(obj, obj_id);
	if(click_cb) obj_click_event_listen(obj, click_cb);

	/* 底部边框 */
	lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
	lv_obj_set_style_local_border_side(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x404040));
	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));

	/* main str */
	lv_obj_t *main_obj = lv_label_create(obj, NULL);
	lv_label_set_long_mode(main_obj, LV_LABEL_LONG_CROP);
	lv_label_set_text(main_obj, main_str);
	lv_obj_set_size(main_obj, 300, 40);
	lv_label_set_align(main_obj, LV_LABEL_ALIGN_LEFT);
	lv_obj_set_style_local_text_font(main_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	lv_obj_align(main_obj, obj, LV_ALIGN_IN_LEFT_MID, 4, 0);

	/* IP */
	lv_obj_t *sub_obj = lv_label_create(obj, NULL);
	lv_label_set_long_mode(sub_obj, LV_LABEL_LONG_CROP);
	lv_label_set_text(sub_obj, IP);
	lv_obj_set_size(sub_obj, 300, 40);
	lv_obj_align(sub_obj, obj, LV_ALIGN_IN_RIGHT_MID, -62, 0);
	lv_label_set_align(sub_obj, LV_LABEL_ALIGN_RIGHT);
	lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));

	return obj;
}

static lv_obj_t *onvif_serach_added_option_create(lv_obj_t *parent, int cont, int obj_id, const char *main_str, const char *IP, obj_click_data *click_cb)
{
    lv_obj_t *obj = lv_obj_create(parent, NULL);
    lv_page_glue_obj(obj, true);
    lv_obj_set_size(obj, 1024 - 56 * 2, 72);
    lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 75 * cont);

    if (obj_id > 0)
        lv_obj_set_id(obj, obj_id);
    if (click_cb)
        obj_click_event_listen(obj, click_cb);

    static rom_bin_info online = rom_bin_info_get(ROM_UI_CCTV_ONLINE_PNG);
    static rom_bin_info offline = rom_bin_info_get(ROM_UI_CCTV_OFFLINE_PNG);
    lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &offline); /* 默认离线 */
    lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &online);
    lv_obj_set_style_local_pattern_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);

    /* 底部边框 */
    lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_border_side(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
    lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
    lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x404040));
    lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));

    /* main str */
    lv_obj_t *main_obj = lv_label_create(obj, NULL);
    lv_label_set_long_mode(main_obj, LV_LABEL_LONG_CROP);
    lv_label_set_text(main_obj, main_str);
    lv_obj_set_size(main_obj, 200, 30);
    lv_obj_align(main_obj, obj, LV_ALIGN_IN_LEFT_MID, 60, -16);
    lv_label_set_align(main_obj, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style_local_text_font(main_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));

    /* IP */
    lv_obj_t *sub_obj = lv_label_create(obj, NULL);
    lv_label_set_long_mode(sub_obj, LV_LABEL_LONG_CROP);
    lv_label_set_text(sub_obj, IP);
    lv_obj_set_size(sub_obj, 200, 30);
    lv_obj_align(sub_obj, main_obj, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_label_set_align(sub_obj, LV_LABEL_ALIGN_LEFT);
    lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
    lv_obj_set_style_local_text_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_SUB_FONT_COLOR));

    return obj;
}

static void onvif_page_create(void)
{
    /* 已添加设备列表 */
    lv_obj_t *addded_label = lv_label_create(lv_scr_act(), NULL);
    // lv_obj_set_id(addded_label, ONVIF_PAGE_ADDED_CONT_OBJ_ID);
    lv_obj_align(addded_label, NULL, LV_ALIGN_IN_TOP_LEFT, 22, 106);
    lv_obj_set_style_local_text_font(addded_label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
    lv_obj_set_style_local_text_color(addded_label, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
    lv_label_set_text(addded_label, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_ADDED_DEV));

    // /* 搜索按钮 */
    lv_obj_t *discovered_cont = lv_cont_create(lv_scr_act(), NULL);
    // lv_obj_set_id(discovered_cont, ONVIF_PAGE_ADDED_CONT_OBJ_ID);
    lv_obj_set_size(discovered_cont, 912, 80);
    lv_obj_align(discovered_cont, NULL, LV_ALIGN_IN_TOP_LEFT, 22, 300);
    lv_obj_set_style_local_value_str(discovered_cont, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_DISCOVERED_DEV));
    lv_obj_set_style_local_value_font(discovered_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
    lv_obj_set_style_local_value_align(discovered_cont, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
    lv_obj_set_style_local_value_color(discovered_cont, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
    lv_obj_set_style_local_value_color(discovered_cont, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x66, 0x66, 0x66));
    static rom_bin_info img_serach = rom_bin_info_get(ROM_UI_ONVIF_DEV_SEARCH_PNG);
    lv_obj_set_style_local_pattern_image(discovered_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_serach);
    lv_obj_set_style_local_pattern_align(discovered_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_RIGHT_MID);

    static obj_click_data click_cb = obj_click_data_create(NULL, search_net_select_confirm);
    obj_click_event_listen(discovered_cont, &click_cb);
}

extern void onvif_manage_curr_index_set(int num);
static void onvif_serach_index_click_fun(lv_obj_t *obj)
{
    int index = (int)obj->user_data;
    onvif_manage_curr_index_set(index);

    goto_layout(pLAYOUT(onvif_input));
}

extern void onvif_info_index_set(int num);
static void onvif_added_opt_click_func(lv_obj_t *obj)
{
    onvif_info_index_set((obj->obj_id == ONVIF_PAGE_ADDED_DEVICE_1_ID) ? 0 : 1);

    goto_layout(pLAYOUT(onvif_info));
}


static void onvif_added_device_1_create()
{
    static obj_click_data click_cb = obj_click_data_create(NULL, onvif_added_opt_click_func);
    onvif_serach_added_option_create(lv_scr_act(), 2, ONVIF_PAGE_ADDED_DEVICE_1_ID, language_common_ch_string_get(MON_CH_CCTV3), user_data_get()->onvif_dev[0].ip, &click_cb);
    lv_obj_set_state(xl_obj_find_by_id(ONVIF_PAGE_ADDED_DEVICE_1_ID), monitor_valid_channel_check(MON_CH_CCTV3) == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
}
static void onvif_added_device_2_create()
{
    static obj_click_data click_cb = obj_click_data_create(NULL, onvif_added_opt_click_func);
    onvif_serach_added_option_create(lv_scr_act(), 3, ONVIF_PAGE_ADDED_DEVICE_2_ID, language_common_ch_string_get(MON_CH_CCTV4), user_data_get()->onvif_dev[1].ip, &click_cb);
    lv_obj_set_state(xl_obj_find_by_id(ONVIF_PAGE_ADDED_DEVICE_2_ID), monitor_valid_channel_check(MON_CH_CCTV4) == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
}

static void onvif_discover_list_create(void)
{
    lv_obj_t *page = lv_page_create(lv_scr_act(), NULL);
    lv_page_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_DRAG);
    lv_obj_set_pos(page, 0, 375);
    lv_obj_set_size(page, 1024, 75 * 3);
    lv_obj_set_style_local_pad_top(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_left(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_right(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_bottom(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

    static obj_click_data click_cb = obj_click_data_create(NULL, onvif_serach_index_click_fun);
    lv_obj_t *obj = NULL;
    // lv_obj_t *label = NULL;

    for (int i = 0; i < sat_ipcamera_online_num_get(); i++)
    {
#if 0
        const char *ip_addr = sat_ipcamera_ipaddr_get(i);
        if (ip_addr == NULL)
            ip_addr = "000.000.000.000";

        // char text[32] = {0};
        // snprintf(text, 32, "%d    %s", i + 1, ip_addr);

        obj = lv_obj_create(page, NULL);
        lv_page_glue_obj(obj, true);
        lv_obj_set_size(obj, 912, 72);
        lv_obj_set_pos(obj, 22, i * 75);
        lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
        lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0x32, 0x32, 0x37));
        lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_make(0x32, 0x32, 0x37));
        lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x22, 0x22, 0x27));
        lv_obj_set_style_local_border_side(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);

        label = lv_label_create(obj, NULL);
        lv_obj_set_style_local_text_font(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
        lv_obj_set_style_local_text_color(label, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
        lv_obj_set_style_local_text_color(label, LV_BTN_PART_MAIN, LV_STATE_PRESSED, lv_color_make(0x66, 0x66, 0x66));
        lv_label_set_text_fmt(label, "%d    %s", i + 1, ip_addr);
        lv_obj_align(label, NULL, LV_ALIGN_IN_LEFT_MID, 44, 0);
        obj_click_event_listen(obj, &click_cb);

        // obj = onvif_serach_option_create(page, i, 0, text, "", &click_cb);
        obj->user_data = (void *)i;

        onvif_discover_info[i].type = serach_net_type;
        onvif_discover_info[i].port = sat_ipcamera_port_get(i);
        strcpy(onvif_discover_info[i].ip, ip_addr);
#else
        const char *ip_addr = sat_ipcamera_ipaddr_get(i);
        if (ip_addr == NULL)
            ip_addr = "000.000.000.000";

        char text[32] = {0};
        snprintf(text, 32, "%d    %s", i + 1, ip_addr);

        obj = onvif_serach_option_create(page, i, 0, text, "", &click_cb);
        obj->user_data = (void *)i;

        onvif_discover_info[i].type = serach_net_type;
        onvif_discover_info[i].port = sat_ipcamera_port_get(i);
        strcpy(onvif_discover_info[i].ip, ip_addr);
#endif
    }
}


static void setting_onvif_search_cancel_btn_up(lv_obj_t *obj)
{
    goto_layout(pLAYOUT(setting_etc));
}

static void LAYOUT_ENTER_FUNC(onvif_search)
{
    route_224_0_0_0_select(0, WLAN_DEV_NAME);
    route_224_0_0_0_select(1, WLAN_DEV_NAME);
    task_t = NULL;
    setting_cancel_btn_create(setting_onvif_search_cancel_btn_up);

    setting_head_label_create(layout_setting_etc_string_get(SETTING_ETC_LANG_ID_CCTV_MANAGE));

    onvif_page_create();
    onvif_added_device_1_create();
    onvif_added_device_2_create();

    onvif_discover_list_create();
}

static void LAYOUT_QUIT_FUNC(onvif_search)
{
    // cctv_online_check_ctrl(false);
    // device_online_state_register(NULL);
}

CREATE_LAYOUT(onvif_search);