#include "layout_define.h"

#define LAYOUT_SETTING_BACKGROUND_OBJ_ID_ON_BACKGROUND 0x01
#define LAYOUT_SETTING_BACKGROUND_OBJ_ID_BACKGROUND 0x02
#define LAYOUT_SETTING_BACKGROUND_OBJ_ID_SD_BACKGROUND 0x03





static void setting_background_cancel_btn_up(lv_obj_t* obj)
{
    goto_layout(pLAYOUT(setting_frame));
}



static void setting_background_off_btn_up(lv_obj_t* obj)
{
    lv_checkbox_set_checked(obj, true);
    obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_BACKGROUND_OBJ_ID_BACKGROUND);
    lv_obj_t* obj1 = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_BACKGROUND_OBJ_ID_SD_BACKGROUND);
    lv_checkbox_set_checked(obj, false);
    lv_checkbox_set_checked(obj1, false);

    user_data_get()->display.frame_background = 0;
    user_data_save();
}
/***
** 日期: 2022-05-06 15:39
** 作者: leo.liu
** 函数作用：创建backgounrd off
** 返回参数说明：
***/

static void setting_background_off_btn_create(void)
{
    static obj_click_data click_data = obj_click_data_up_create(setting_background_off_btn_up);
    lv_obj_t *obj = setting_sub_btn_base_create(NULL, 48, 94, 1168, 96,
                                                layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_NO_BACKGROUND_IMAGE),
                                                &click_data,
                                                user_data_get()->display.frame_background == 0 ? true : false,
                                                2);
    lv_obj_set_id(obj, LAYOUT_SETTING_BACKGROUND_OBJ_ID_ON_BACKGROUND);
}


static void setting_background_on_btn_up(lv_obj_t* obj)
{
    lv_checkbox_set_checked(obj, true);
    obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_BACKGROUND_OBJ_ID_ON_BACKGROUND);
    lv_obj_t* obj1 = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_BACKGROUND_OBJ_ID_SD_BACKGROUND);
    lv_checkbox_set_checked(obj, false);
    lv_checkbox_set_checked(obj1, false);

    user_data_get()->display.frame_background = 1;
    user_data_save();
}
/***
** 日期: 2022-05-06 15:39
** 作者: leo.liu
** 函数作用：创建backgounrd on
** 返回参数说明：
***/

static void setting_background_on_btn_create(void)
{
    static obj_click_data click_data = obj_click_data_up_create(setting_background_on_btn_up);
    lv_obj_t *obj = setting_sub_btn_base_create(NULL, 48, 94+96, 1168, 96,
                                                layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_BASIC_IMAGE),
                                                &click_data,
                                                user_data_get()->display.frame_background == 1 ? true : false,
                                                2);
    lv_obj_set_id(obj, LAYOUT_SETTING_BACKGROUND_OBJ_ID_BACKGROUND);
}



static void setting_sd_background_on_btn_up(lv_obj_t* obj)
{
    lv_checkbox_set_checked(obj, true);
    obj = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_BACKGROUND_OBJ_ID_ON_BACKGROUND);
    lv_obj_t* obj1 = lv_obj_get_child_form_id(lv_scr_act(), LAYOUT_SETTING_BACKGROUND_OBJ_ID_BACKGROUND);
    lv_checkbox_set_checked(obj, false);
    lv_checkbox_set_checked(obj1, false);

    user_data_get()->display.frame_background = 2;
    user_data_save();
}


static void setting_sd_background_on_btn_create(void)
{
    static obj_click_data click_data = obj_click_data_up_create(setting_sd_background_on_btn_up);
    lv_obj_t *obj = setting_sub_btn_base_create(NULL, 48, 94+96*2, 1168, 96,
                                                layout_setting_frame_string_get(SETTING_FRAME_LANG_ID_SD_IMAGE),
                                                &click_data,
                                                user_data_get()->display.frame_background == 2 ? true : false,
                                                2);
    lv_obj_set_id(obj, LAYOUT_SETTING_BACKGROUND_OBJ_ID_SD_BACKGROUND);
}



static void LAYOUT_ENTER_FUNC(setting_background)
{
    setting_cancel_btn_create(setting_background_cancel_btn_up);

    setting_head_label_create(layout_setting_record_string_get(SETTING_VOLUME_LANG_ID_API_OPEN_FRAME_TIME));
    setting_background_off_btn_create();
    setting_background_on_btn_create();
    setting_sd_background_on_btn_create();

}

static void LAYOUT_QUIT_FUNC(setting_background)
{

}

CREATE_LAYOUT(setting_background);