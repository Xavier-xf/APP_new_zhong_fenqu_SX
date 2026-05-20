#include "layout_define.h"

static void layout_tuya_monitor_task(lv_task_t* task){

	if( tuya_online_num_get_customize() > 0)
	{
		return ;
	}

	goto_layout(pLAYOUT(home));
}


static void layout_tuya_monitor_photo()
{
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 204, 230);
	lv_obj_set_size(obj, 618, 96);
	
	static rom_bin_info image = rom_bin_info_get(ROM_UI_TUYA_MONITOR_NEW_ION_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &image);

	lv_layout_task_create(layout_tuya_monitor_task, 100, LV_TASK_PRIO_MID, NULL);
	layout_tuya_monitor_task(NULL);

}

extern TP9950_FORMAT video_door_format_check;
static void layout_tuya_monitor_vaild_channel_switch_task(lv_task_t *task_t)
{
	MON_CH ch = monitor_channel_get();
	if ((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
	{
		if(video_door_format_check != INVALID_FORMAT)
		{
			return;
		}
		else
		{
			if ((ch == MON_CH_CCTV1) )
			{
				ch = MON_CH_CCTV2;
				if ((monitor_valid_channel_check(ch) == true))
				{
					monitor_channel_set(ch);
					// monitor_open(false, 0x03);
					return;
				}
			}
			if ((ch == MON_CH_CCTV2))
			{	
				ch = MON_CH_DOOR1;
				monitor_channel_set(ch);
				monitor_open(false, 0x03);
				// lv_task_del(task_t);
				return;
			}			
		}
	}
}

static void LAYOUT_ENTER_FUNC(tuya_monitor)
{
	lv_video_mode_enable(false);
	video_display_preview_enable(false);
	standby_timer_close();
	MON_CH ch = monitor_channel_get();
	intercom_data_busy_enbale(true);
	if (ch == MON_CH_DOOR1)
	{
		audio_to_outdoor2_pin_ctrl(false);
		audio_to_outdoor1_pin_ctrl(true);
	}
	else if (ch == MON_CH_DOOR2)
	{
		audio_to_outdoor2_pin_ctrl(true);
		audio_to_outdoor1_pin_ctrl(false);
	}
    layout_tuya_monitor_photo();
	screen_force_refresh();
	lv_layout_task_create(layout_tuya_monitor_vaild_channel_switch_task, 3000, LV_TASK_PRIO_HIGH, NULL);
}

static void LAYOUT_QUIT_FUNC(tuya_monitor)
{
	standby_timer_restart(true);
}

CREATE_LAYOUT(tuya_monitor);