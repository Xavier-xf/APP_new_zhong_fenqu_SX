#include "layout_define.h"

#define SETTING_LANGUAGE_OBJ_ID_PARENT 0X10

#define SETTING_LANGUAGE_OBJ_ID_1 LANGUAGE_ID_ENGLISH
// #define SETTING_LANGUAGE_OBJ_ID_2 LANGUAGE_ID_HANYU
// #define SETTING_LANGUAGE_OBJ_ID_3 LANGUAGE_ID_ELUOSI
// #define SETTING_LANGUAGE_OBJ_ID_4 LANGUAGE_ID_XIBANYA
// #define SETTING_LANGUAGE_OBJ_ID_5 LANGUAGE_ID_CHINESE
// #define SETTING_LANGUAGE_OBJ_ID_6 LANGUAGE_ID_TUERQI
// #define SETTING_LANGUAGE_OBJ_ID_7 LANGUAGE_ID_BOLAN
// #define SETTING_LANGUAGE_OBJ_ID_8 LANGUAGE_ID_JIEKE
#define SETTING_LANGUAGE_OBJ_ID_2 LANGUAGE_ID_ALABOYU
// #define SETTING_LANGUAGE_OBJ_ID_10 LANGUAGE_ID_PUTAOYA
// #define SETTING_LANGUAGE_OBJ_ID_11 LANGUAGE_ID_FAYU
// #define SETTING_LANGUAGE_OBJ_ID_12 LANGUAGE_ID_DEYU


static void setting_language_cancel_btn_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(setting_etc));
}

static lv_obj_t *setting_language_list_create(void)
{
	lv_obj_t *obj = lv_page_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 30, 70);
	lv_obj_set_size(obj, 934, 88 * 6 - 20);
	lv_page_set_scrollable_fit4(obj, LV_FIT_NONE, LV_FIT_NONE, LV_FIT_MAX, LV_FIT_MAX);
	lv_obj_set_id(obj, SETTING_LANGUAGE_OBJ_ID_PARENT);
	return obj;
}

static void setting_language_btn_up(lv_obj_t *obj)
{
	user_data_get()->etc.language = obj->obj_id;
	user_data_save();
	language_id_set(obj->obj_id);

	goto_layout(pLAYOUT(setting_language));
}

static void setting_language_btn_create(lv_obj_t *parent, int language)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_language_btn_up);

	lv_obj_t *obj = setting_sub_btn_base_create(parent, 0, 88 * language, 934, 88,
						    layout_setting_etc_string_get_form_language(SETTING_ETC_LANG_ID_LANGUAGE_SUB, language),
						    &click_data,
						    user_data_get()->etc.language == language ? true : false,
						    2);

	lv_obj_set_id(obj, language);

	lv_page_glue_obj(obj, true);
}

static void LAYOUT_ENTER_FUNC(setting_language)
{
	setting_cancel_btn_create(setting_language_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	lv_obj_t *list = setting_language_list_create();

	setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_1);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_2);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_3);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_4);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_5);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_6);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_7);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_8);
	setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_2);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_10);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_11);
	// setting_language_btn_create(list, SETTING_LANGUAGE_OBJ_ID_12);

}

static void LAYOUT_QUIT_FUNC(setting_language)
{
}

CREATE_LAYOUT(setting_language);