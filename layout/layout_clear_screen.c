#include "layout_define.h"

/***
** 日期: 2022-05-07 09:37
** 作者: leo.liu
** 函数作用：显示finish time
** 返回参数说明：
***/
static void clear_screen_text_display(void)
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 0, 168);
	lv_obj_set_size(obj, 1024, 42);

	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(40));

	lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, layout_setting_display_string_get(SETTING_DISPLAY_LANG_ID_FINISH_TIME));
	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
}

static void clear_screen_timeout_task(lv_task_t *task_t)
{
	lv_obj_t *obj = (lv_obj_t *)task_t->user_data;
	int value = *((int *)obj->user_data);

	if (value == 0)
	{
		goto_layout(pLAYOUT(home));
		/***** 在goto后不能再有执行语句 *****/
		return;
	}
	else
	{
		lv_label_set_text_fmt(obj, "%02d", value);
	}
	(*((int *)obj->user_data))--;
}

/***
** 日期: 2022-05-07 09:46
** 作者: leo.liu
** 函数作用：创建超时
** 返回参数说明：
***/
static void clear_screen_timeout_task_create(void)
{
	lv_obj_t *obj = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 0, 225);
	lv_obj_set_size(obj, 1024, 225);

	lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_style_local_text_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(150));

	static int timeout = 30;
	timeout = 30;
	obj->user_data = &timeout;
	lv_task_t *task_t = lv_layout_task_create(clear_screen_timeout_task, 1000, LV_TASK_PRIO_MID, obj);
	clear_screen_timeout_task(task_t);
	lv_task_ready(task_t);
}

static void LAYOUT_ENTER_FUNC(clear_screen)
{
	standby_timer_close();
	clear_screen_text_display();

	clear_screen_timeout_task_create();
}

static void LAYOUT_QUIT_FUNC(clear_screen)
{
	standby_timer_restart(true);
}

CREATE_LAYOUT(clear_screen);