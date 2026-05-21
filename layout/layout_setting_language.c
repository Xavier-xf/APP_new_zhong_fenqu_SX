#include "layout_define.h"

#define SETTING_LANGUAGE_OBJ_ID_PARENT 0X10
#define SETTING_LANGUAGE_BTN_OBJ_ID_BASE 0X100

static const char *setting_language_name_get(LANGUAGE_ID language)
{
	if (is_language_xls_inited())
	{
		const char *name = lang_xls_language_name_get(language_to_xls_col(language));
		if (name != NULL && name[0] != '\0')
		{
			return name;
		}
	}

	return layout_setting_etc_string_get_form_language(SETTING_ETC_LANG_ID_LANGUAGE_SUB, language);
}

static int setting_language_obj_id_get(int language)
{
	return SETTING_LANGUAGE_BTN_OBJ_ID_BASE + language;
}

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
	int language = obj->obj_id - SETTING_LANGUAGE_BTN_OBJ_ID_BASE;
	int old_language = user_data_get()->etc.language;
	lv_obj_t *old_obj;

	if (language < 0 || language >= LANGUAGE_ID_TOTAL)
	{
		return;
	}

	user_data_get()->etc.language = language;
	user_data_save();
	language_id_set(language);

	old_obj = lv_obj_get_child_form_id(obj->parent, setting_language_obj_id_get(old_language));
	if (old_obj != NULL)
	{
		lv_checkbox_set_checked(old_obj, false);
	}
	lv_checkbox_set_checked(obj, true);
}

static void setting_language_btn_create(lv_obj_t *parent, int language, int index)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_language_btn_up);

	lv_obj_t *obj = setting_sub_btn_base_create(parent, 0, 88 * index, 934, 88,
						    setting_language_name_get(language),
						    &click_data,
						    user_data_get()->etc.language == language ? true : false,
						    2);
	if (obj == NULL)
	{
		printf("create setting language row failed \n");
		return;
	}

	lv_obj_set_id(obj, setting_language_obj_id_get(language));
	lv_page_glue_obj(obj, true);
}

static void LAYOUT_ENTER_FUNC(setting_language)
{
	setting_cancel_btn_create(setting_language_cancel_btn_up);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	lv_obj_t *list = setting_language_list_create();
	int language;

	for (language = 0; language < LANGUAGE_ID_TOTAL; language++)
	{
		setting_language_btn_create(list, language, language);
	}

}

static void LAYOUT_QUIT_FUNC(setting_language)
{
}

CREATE_LAYOUT(setting_language);
