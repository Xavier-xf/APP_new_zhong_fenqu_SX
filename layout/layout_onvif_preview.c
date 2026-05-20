#include "layout_define.h"

typedef enum
{
    MSGBOX_OBJ_ID = 0x01,
    LAYOUT_SETTING_RECORD_OBJ_MSG_ID,
    LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID
} ONVIF_PRVIEW_OBJ_ID;

static int onvif_premiew_enter_flag = 0; /* [0]-info [1]-input */

static char curr_url[128] = {0};
void onvif_preview_url_set(const char *url)
{
    memset(curr_url, 0, sizeof(curr_url));
    // strcpy(curr_url, url);
    snprintf(curr_url, sizeof(curr_url), url);

}

static char curr_ip[128] = {0};
void onvif_preview_ip_set(const char *ip)
{
    memset(curr_ip, 0, sizeof(curr_ip));
    // strcpy(curr_ip, ip);
    snprintf(curr_ip, sizeof(curr_ip), ip);
}

extern RTSP_CLIENT_STATE_T RTSP_CLIENT_STATE; /* RTSP_CLIENT_STATE_T */
extern int onvif_manage_curr_index_get();
extern int onvif_add_manage_curr_index_get();
extern int cctv_setting_channel_get(void);

/*************************************************************************
 * @brief  设置进入预览界面的标志
 * @date   2023-04-19 14:17
 * @author xiaole
 * @return
 * @param  [0]-info [1]-input
 **************************************************************************/
void onvif_preview_enter_flag_set(int flag)
{
    onvif_premiew_enter_flag = flag;
}

static void layout_onvif_preview_back_btn_up(lv_obj_t *obj)
{
    LOG_WHITE("click \n");

    if (onvif_premiew_enter_flag == 0)
        goto_layout(pLAYOUT(onvif_info));
    if (onvif_premiew_enter_flag == 1)
        goto_layout(pLAYOUT(onvif_input));
    if (onvif_premiew_enter_flag == 2)
        goto_layout(pLAYOUT(ipc_camera_m_add));
}



static void setting_msgdialog_confirm_btn_up(lv_obj_t *obj)
{
    lv_obj_t *dialog = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_RECORD_OBJ_MSG_ID);
    if (dialog == NULL)
    {
        printf("find obj dialog failed \n");
        return;
    }

    lv_obj_t *btnmatrix = lv_obj_get_child_form_id(dialog, LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID);
    if (btnmatrix == NULL)
    {
        printf("find btnmatrix failed \n");
    }

    if(onvif_premiew_enter_flag == 1)
    {
        for (int i = 0; i < 2; i++)
        {
            if (lv_btnmatrix_get_btn_ctrl(btnmatrix, i, LV_BTNMATRIX_CTRL_CHECK_STATE) == true)
            {
                memset(user_data_get()->onvif_dev[i].ip, 0, sizeof(user_data_get()->onvif_dev[i].ip));
                memset(user_data_get()->onvif_dev[i].user, 0, sizeof(user_data_get()->onvif_dev[i].user));
                memset(user_data_get()->onvif_dev[i].pswd, 0, sizeof(user_data_get()->onvif_dev[i].pswd));
                memset(user_data_get()->onvif_dev[i].url, 0, sizeof(user_data_get()->onvif_dev[i].url));

                user_data_get()->onvif_dev[i].brand = -1;
                user_data_get()->onvif_dev[i].port = onvif_discover_info[onvif_manage_curr_index_get()].port;
                strcpy(user_data_get()->onvif_dev[i].ip, onvif_discover_info[onvif_manage_curr_index_get()].ip);
                strcpy(user_data_get()->onvif_dev[i].user, onvif_discover_info[onvif_manage_curr_index_get()].user);
                strcpy(user_data_get()->onvif_dev[i].pswd, onvif_discover_info[onvif_manage_curr_index_get()].pswd);
                strcpy(user_data_get()->onvif_dev[i].url[0], onvif_discover_info[onvif_manage_curr_index_get()].rtsp_url[0]);
                strcpy(user_data_get()->onvif_dev[i].url[1], onvif_discover_info[onvif_manage_curr_index_get()].rtsp_url[1]);
                strcpy(user_data_get()->onvif_dev[i].url[2], curr_url);
                monitor_valid_channel_set(MON_CH_CCTV3 + i, true);
                user_data_save();
                break;
            }
        }

        goto_layout(pLAYOUT(onvif_input));
    }
    // else if(onvif_premiew_enter_flag == 2)
    // {
    //     for (int i = 0; i < 2; i++)
    //     {
    //         if (lv_btnmatrix_get_btn_ctrl(btnmatrix, i, LV_BTNMATRIX_CTRL_CHECK_STATE) == true)
    //         {
    //             user_onvif_info *pdata = &user_data_get()->onvif_dev[i];
    //             ONVIF_ADD_INFO_STRUCT *pinfo = &onvif_add_discover_info[onvif_add_manage_curr_index_get()];

    //             memset(pdata->ip, 0, sizeof(pdata->ip));
    //             memset(pdata->user, 0, sizeof(pdata->user));
    //             memset(pdata->pswd, 0, sizeof(pdata->pswd));
    //             memset(pdata->url, 0, sizeof(pdata->url));

    //             // LOG_WHITE("===========[%d]========\n",pdata->brand);
    //             // LOG_WHITE("===========[%d]========\n",pdata->port);
    //             // LOG_WHITE("===========[%s]========\n",pdata->ip);
    //             // LOG_WHITE("===========[%s]========\n",pdata->user);
    //             // LOG_WHITE("===========[%s]========\n",pdata->pswd);
    //             // LOG_WHITE("===========[%s]========\n",pdata->url[0]);
    //             // LOG_WHITE("===========[%s]========\n",pdata->url[1]);
    //             // LOG_WHITE("%d\n", onvif_add_manage_curr_index_get());


    //             // user_data_get()->onvif_dev[i].brand = -1;
    //             pdata->port = pinfo->port;
    //             snprintf(pdata->ip, sizeof(pdata->ip), pinfo->ip);
    //             snprintf(pdata->user, sizeof(pdata->user), pinfo->user);
    //             snprintf(pdata->pswd, sizeof(pdata->pswd), pinfo->pswd);
    //             snprintf(pdata->url[0], sizeof(pdata->url[0]), pinfo->rtsp_url[0]);
    //             snprintf(pdata->url[1], sizeof(pdata->url[1]), pinfo->rtsp_url[1]);
    //             snprintf(pdata->url[2], sizeof(pdata->url[2]), curr_url);

    //             monitor_valid_channel_set(MON_CH_CCTV3 + i, true);
    //             user_data_save();




    //             // LOG_WHITE("===========[%d]========\n\n",user_data_get()->onvif_dev[i].brand);
    //             // LOG_WHITE("===========[%d]========\n\n",user_data_get()->onvif_dev[i].port);
    //             // LOG_WHITE("===========[%s]========\n\n",user_data_get()->onvif_dev[i].ip);
    //             // LOG_WHITE("===========[%s]========\n\n",user_data_get()->onvif_dev[i].user);
    //             // LOG_WHITE("===========[%s]========\n\n",user_data_get()->onvif_dev[i].pswd);
    //             // LOG_WHITE("===========[%s]========\n\n",user_data_get()->onvif_dev[i].url[0]);
    //             // LOG_WHITE("===========[%s]========\n\n",user_data_get()->onvif_dev[i].url[1]);

    //             break;
    //         }
    //     }

    //     goto_layout(pLAYOUT(ipc_camera_m_add));
    // }
    
}

static void setting_msgdialog_cancel_btn_up(lv_obj_t *obj)
{
    setting_msgdialog_msg_bg_delete(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);
}
static void register_button_func(lv_obj_t *obj)
{
    LOG_WHITE("click\n");

    if (RTSP_CLIENT_STATE != RTSP_CLIENT_SUCCESS)
    {
        printf("rtsp open fail\n");
        return;
    }

    if(onvif_premiew_enter_flag == 2)
    {
        user_onvif_info *pdata = &user_data_get()->onvif_dev[cctv_setting_channel_get()];
        ONVIF_ADD_INFO_STRUCT *pinfo = &onvif_add_discover_info[onvif_add_manage_curr_index_get()];

        memset(pdata->ip, 0, sizeof(pdata->ip));
        memset(pdata->user, 0, sizeof(pdata->user));
        memset(pdata->pswd, 0, sizeof(pdata->pswd));
        memset(pdata->url, 0, sizeof(pdata->url));

        // LOG_WHITE("===========[%d]========\n",pdata->brand);
        // LOG_WHITE("===========[%d]========\n",pdata->port);
        // LOG_WHITE("===========[%s]========\n",pdata->ip);
        // LOG_WHITE("===========[%s]========\n",pdata->user);
        // LOG_WHITE("===========[%s]========\n",pdata->pswd);
        // LOG_WHITE("===========[%s]========\n",pdata->url[0]);
        // LOG_WHITE("===========[%s]========\n",pdata->url[1]);
        // LOG_WHITE("%d\n", onvif_add_manage_curr_index_get());


        // user_data_get()->onvif_dev[i].brand = -1;
        pdata->port = pinfo->port;
        snprintf(pdata->ip, sizeof(pdata->ip), pinfo->ip);
        snprintf(pdata->user, sizeof(pdata->user), pinfo->user);
        snprintf(pdata->pswd, sizeof(pdata->pswd), pinfo->pswd);
        snprintf(pdata->url[0], sizeof(pdata->url[0]), pinfo->rtsp_url[0]);
        snprintf(pdata->url[1], sizeof(pdata->url[1]), pinfo->rtsp_url[1]);
        snprintf(pdata->url[2], sizeof(pdata->url[2]), curr_url);
        printf("========>>>>[%d] \n",cctv_setting_channel_get());
        monitor_valid_channel_set(MON_CH_CCTV3 + cctv_setting_channel_get(), true);
        user_data_save();

        goto_layout(pLAYOUT(ipc_camera_m_add));
        return;
    }

    static const char *string_map[] = {
        "",
        "\n",
        "",
        "",
    };
    string_map[0] = language_common_ch_string_get(MON_CH_CCTV3);
    string_map[2] = language_common_ch_string_get(MON_CH_CCTV4);

    // lv_obj_t *msgbox = msg_window_with_button(600, 360, NULL,NULL,msg_window_confirm_func);
    // msg_window_btnmatrix_create(msgbox, string_map, MSGBOX_OBJ_ID, true);

    lv_obj_t *cont = setting_msgdialog_msg_bg_create(LAYOUT_SETTING_RECORD_OBJ_MSG_ID);

    setting_msgdialog_msg_confirm_and_cancel_btn_create(cont, setting_msgdialog_cancel_btn_up, setting_msgdialog_confirm_btn_up);

    lv_obj_t *btnmatrix = setting_msgdialog_btnmatrix_create(cont, LAYOUT_SETTING_RECORD_OBJ_BTNMATRIX_ID, string_map);

    lv_btnmatrix_set_btn_ctrl(btnmatrix, monitor_valid_channel_check(MON_CH_CCTV3) == true ? 1 : 0, LV_BTNMATRIX_CTRL_CHECK_STATE);
}

static void onvif_device_register_task(lv_task_t *task)
{
    lv_obj_set_hidden((lv_obj_t *)task->user_data, (RTSP_CLIENT_STATE == RTSP_CLIENT_SUCCESS) ? false : true);
    // lv_task_del(task);
}

static void onvif_device_register_btn_create(void)
{
    if (onvif_premiew_enter_flag == 0)
        return; /* 查看信息时不创建注册按钮 */

    static obj_click_data click_cb = obj_click_data_create(NULL, register_button_func);
    lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj, 1024, 72);
    lv_obj_align(obj, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    obj_click_event_listen(obj, &click_cb);

    lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_etc_string_get(SETTING_ETC_LANG_ID_REGISTER_DEV));
    lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(31));
    lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x3AC392));

    if (RTSP_CLIENT_STATE != RTSP_CLIENT_SUCCESS)
    {
        // LOG_CYAN("main strp ont success\n");
        lv_obj_set_hidden(obj, true);
    }

    lv_layout_task_create(onvif_device_register_task, 1000, LV_TASK_PRIO_MID, obj);
}

static void onvif_preview_title_bar_create(void)
{
    lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
    lv_obj_set_size(obj, LV_HOR_RES_MAX, 75);
    lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

    lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_40);
    lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x161616));
}

static void LAYOUT_ENTER_FUNC(onvif_preview)
{
    rtsp_decode_open(curr_url);
    // local_need_video_set(true);
    onvif_preview_title_bar_create();

    setting_cancel_btn_create(layout_onvif_preview_back_btn_up);
    setting_head_label_create(curr_ip);

    // layout_title_label_create(curr_ip);
    // layout_back_btn_create(layout_onvif_preview_back_btn_up);

    // system_bg_fill_color(0x00, 0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    // fb_video_mode_enable(true);
    // rtsp_open(curr_url);

    onvif_device_register_btn_create();
}

static void LAYOUT_QUIT_FUNC(onvif_preview)
{
    rtsp_decode_close();
    // local_need_video_set(false);
    standby_timer_restart(false);
    // monitor_close();
    // video_raw_clear();

    // system_bg_fill_color(0x00, 0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    // fb_video_mode_enable(false);
}

CREATE_LAYOUT(onvif_preview);