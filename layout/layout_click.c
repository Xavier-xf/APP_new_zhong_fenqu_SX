/*******************************************************************
 * @Descripttion   : 
 * @version        : 1.0.0
 * @Author         : wxj
 * @Date           : 2022-11-22 18:12
 * @LastEditTime   : 2022-11-24 14:29
*******************************************************************/
#include "layout_define.h"

static int obj_num = 0;






extern void obj_screen_click_event_register(void (*callback)(lv_obj_t*));



static void layout_dev_back_btn_func(lv_obj_t *obj)
{
	LOG_WHITE("click \n");
	goto_layout(pLAYOUT(home));
}

static void click_cancel_btn_create(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 20, 0);
	lv_obj_set_size(obj, 80, 80);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
	static obj_click_data click_data = obj_click_data_up_create(layout_dev_back_btn_func);
	obj_click_event_listen(obj, &click_data);
    static rom_bin_info image = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image);
}



void dev_touch_func(lv_obj_t *obj)
{
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t p;
    obj_num++;
    if(obj_num == 250)
    {
        lv_obj_clean(lv_scr_act());
        click_cancel_btn_create();
        obj_num = 0;
    }
    if(lv_indev_get_type(indev) == LV_INDEV_TYPE_POINTER)
    {
        lv_indev_get_point(indev, &p);
        // LOG_B_WHITE("(%d, %d)\n", p.x, p.y);

        lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
        lv_obj_set_size(cont, 1, 1);
        lv_obj_set_pos(cont, p.x, p.y);
        lv_obj_set_click(cont, false);
        lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
        lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

        lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
        lv_obj_set_size(obj, 20, 1);
        lv_obj_align(obj, cont, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_click(obj, false);
        lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
        lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFF0000));

        obj = lv_obj_create(lv_scr_act(), obj);
        lv_obj_set_size(obj, 1, 20);
        lv_obj_set_click(obj, false);
        lv_obj_align(obj, cont, LV_ALIGN_CENTER, 0, 0);

    }
    
}


static void LAYOUT_ENTER_FUNC(click)
{    
	click_cancel_btn_create();
	obj_screen_click_event_register(dev_touch_func) ;


}

static void LAYOUT_QUIT_FUNC(click)
{
    obj_screen_click_event_register(NULL);
}

CREATE_LAYOUT(click);


