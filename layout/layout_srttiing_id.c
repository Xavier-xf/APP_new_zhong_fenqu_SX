#include "layout_define.h"
#include "wifi_api.h"

#define SETTING_ID_OBJ_ID_1 SETTING_ETC_LANG_ID_DEVICE_ID1
#define SETTING_ID_OBJ_ID_2 SETTING_ETC_LANG_ID_DEVICE_ID2
#define SETTING_ID_OBJ_ID_3 SETTING_ETC_LANG_ID_DEVICE_ID3
#define SETTING_ID_OBJ_ID_4 SETTING_ETC_LANG_ID_DEVICE_ID4

static void setting_id_cancel_btn_up(lv_obj_t *obj)
{
    goto_layout(pLAYOUT(setting_etc));
}

#define OBJ_ID_GET(id) SETTING_ID_OBJ_ID_##id
#define OBJ_IS_ID(obj, id) (OBJ_ID_GET(id) == obj->obj_id)

static void setting_id_btn_up(lv_obj_t *obj)
{
    char id = obj->obj_id - SETTING_ID_OBJ_ID_1;
    if(user_data_get()->etc.deive_id != id)
    {
        user_data_get()->etc.deive_id = id;
        user_data_save();
    }
    

    lv_checkbox_set_checked(obj, true);
    if (OBJ_IS_ID(obj, 1) == false)
    {
        lv_obj_t*obj = lv_obj_get_child_form_id(lv_scr_act(), OBJ_ID_GET(1));
        lv_checkbox_set_checked(obj, false);
    }

    if (OBJ_IS_ID(obj, 2) == false)
    {
        lv_obj_t*obj = lv_obj_get_child_form_id(lv_scr_act(), OBJ_ID_GET(2));
        lv_checkbox_set_checked(obj, false);
    }

    if (OBJ_IS_ID(obj, 3) == false)
    {
        lv_obj_t*obj = lv_obj_get_child_form_id(lv_scr_act(), OBJ_ID_GET(3));
        lv_checkbox_set_checked(obj, false);
    }
    if (OBJ_IS_ID(obj, 4) == false)
    {
        lv_obj_t*obj = lv_obj_get_child_form_id(lv_scr_act(), OBJ_ID_GET(4));
        lv_checkbox_set_checked(obj, false);
    }

   
}
/***
** 日期: 2022-05-07 08:31
** 作者: leo.liu
** 函数作用：时间
** 返回参数说明：
***/
static void setting_id_btn_create(int index)
{
    static obj_click_data click_data = obj_click_data_up_create(setting_id_btn_up);
    lv_obj_t *obj = setting_sub_btn_base_create(NULL, 56, 94 + 96 * index, 912, 96,
                                                layout_setting_etc_string_get(index + SETTING_ID_OBJ_ID_1),
                                                &click_data,
                                                user_data_get()->etc.deive_id == index ? true : false,
                                                2);

    lv_obj_set_id(obj, index + SETTING_ID_OBJ_ID_1);
}

static void LAYOUT_ENTER_FUNC(setting_id)
{
    setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));
    setting_cancel_btn_create(setting_id_cancel_btn_up);

    setting_id_btn_create(0);
    setting_id_btn_create(1);
    setting_id_btn_create(2);
    setting_id_btn_create(3);
}

static void LAYOUT_QUIT_FUNC(setting_id)
{ 
    if(OwnID != 1)
    {
        user_data_get()->display.frame_list = 0x00;
        user_data_get()->wifi_enable = false;
        user_data_get()->home_mode = 2;
        user_data_save();
        
        wifi_device_close();
    }
}

CREATE_LAYOUT(setting_id);