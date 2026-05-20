#include "layout_define.h"

static void setting_system_time_set_btn_up(lv_obj_t *obj)
{
	setting_time_enter_flag_set(0x02);
	goto_layout(pLAYOUT(setting_time));
}

static void setting_time_system_time_btn_create(void)
{
	static obj_click_data click_data = obj_click_data_up_create(setting_system_time_set_btn_up);
	setting_right_btn_base_create(NULL, 353, 94, 622, 90,
				      layout_setting_systime_string_get(SETTING_DATE_LANG_ID_SYS_TIME),
				      NULL,
				      &click_data,
				      0);
}

static void LAYOUT_ENTER_FUNC(setting_date_time)
{
	setting_cancel_btn_create(NULL);

	setting_head_label_create(layout_setting_record_string_get(SETTING_RECORD_LANG_ID_SETTING));

	setting_main_btn_create(3);

	setting_time_system_time_btn_create();
}

static void LAYOUT_QUIT_FUNC(setting_date_time)
{
}

CREATE_LAYOUT(setting_date_time);