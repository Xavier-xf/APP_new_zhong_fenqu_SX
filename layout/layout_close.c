#include "layout_define.h"
#include "motion_detection.h"
#include "audio_output.h"
#define LAYOUT_CLOSE_OBJ_ID_CANCEL 0X01

static void layout_motion_rec_icon_hidden(bool en);
static void layout_motion_timer_task(lv_task_t *task_t);
bool frame_display_timeout_check(void);
/***
**   日期:2022-06-09 11:18:51
**   作者: leo.liu
**   函数作用：开启移动侦测时间戳
**   参数说明:
***/
static unsigned long long motion_detection_timestap = 0;

static void motion_move_check_task(lv_task_t *task);
static bool motion_timer_timeout_check(void);

static void close_cancel_btn_up(lv_obj_t *obj)
{
	if(mjpeg_encode_status_get() == true)
	{
		usleep(500*1000);
	}
	goto_layout(pLAYOUT(home));
}

static void close_cancel_btn_create(void)
{
	// lv_obj_t *obj = lv_scr_act();
	lv_obj_t *obj = lv_obj_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 1024, 600);
		lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
	lv_obj_set_id(obj,LAYOUT_CLOSE_OBJ_ID_CANCEL);
	static obj_click_data click_data = obj_click_data_up_create(close_cancel_btn_up);
	obj_click_event_listen(obj, &click_data);
}

/***
**   日期:2022-06-09 10:57:01
**   作者: leo.liu
**   函数作用：开启移动侦测监控
**   参数说明:
***/
static void layout_motion_monitor_open(void)
{
	char camera = user_data_get()->motion.select_camera;
	MON_CH ch = camera == 0 ? MON_CH_DOOR1 : camera == 1 ? MON_CH_DOOR2
					     : camera == 2   ? MON_CH_CCTV1
							     : MON_CH_CCTV2;
	monitor_channel_set(ch);
	monitor_open(true, 0x01);

}

/***
**   日期:2022-06-09 10:57:09
**   作者: leo.liu
**   函数作用：关闭移动侦测监控
**   参数说明:
***/
static void layout_motion_monitor_close(void)
{
	monitor_close();
	
}

static void layout_motion_restart_motion_delay_task(lv_task_t *task)
{
	if (motion_timer_timeout_check() == true)
	{

		layout_motion_monitor_open();
		motion_detection_start();
		motion_detection_timestap = user_timestamp_get();
		lv_layout_task_create(motion_move_check_task, 200, LV_TASK_PRIO_MID, NULL);
		lv_task_del(task);
	}
}
/***
**   日期:2022-06-09 11:23:26
**   作者: leo.liu
**   函数作用：重新开启移动侦测
**   参数说明:
***/
static void layout_motion_restart_motion_detection(void)
{
	backlight_enable(false);

	layout_motion_monitor_close();
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_CLOSE_OBJ_ID_CANCEL);
	lv_obj_set_click(obj,true);
	lv_layout_task_create(layout_motion_restart_motion_delay_task, 3000, LV_TASK_PRIO_MID, NULL);
}
/***
**   日期:2022-06-09 11:13:14
**   作者: leo.liu
**   函数作用：记录处理
**   参数说明:
***/
static void layout_motion_record_process(void)
{
	if ((media_sdcard_insert_check() == false) || (user_data_get()->motion.saving_fmt == 0))
	{
		record_jpeg_start(REC_MODE_MOTION | REC_MODE_TUYA_MOTION);
	}
	else
	{
		MON_CH ch = monitor_channel_get();
		if ((ch == MON_CH_DOOR1) || (ch == MON_CH_DOOR2))
		{
			audio_input_capture_enable(true);
		}
		user_data_get()->get_record_flag = 0;
		record_video_start(REC_MODE_MOTION);
		record_jpeg_start(REC_MODE_TUYA_MOTION);
	}
	if (user_data_get()->motion.lcd_en == true)
	{
		backlight_enable(true);
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_CLOSE_OBJ_ID_CANCEL);
		lv_obj_set_click(obj,false);
	}
	
	app_playback_record(true);
	layout_motion_rec_icon_hidden(false);
	lv_layout_task_create(layout_motion_timer_task, 1000, LV_TASK_PRIO_MID, NULL);
}

/***
**   日期:2022-06-09 11:05:29
**   作者: leo.liu
**   函数作用：开启移动检测
**   参数说明:
***/
static void motion_move_check_task(lv_task_t *task)
{
	if (motion_detection_check() == false)
	{
		unsigned long long time = user_timestamp_get();
		if (abs(motion_detection_timestap - time) > (1 * 60 * 60 * 1000))
		{
			motion_detection_stop();
			layout_motion_restart_motion_detection();
			lv_task_del(task);
		}
		return;
	}

	motion_detection_stop();
	layout_motion_record_process();
	lv_task_del(task);
}

static bool motion_timer_timeout_check(void)
{
	if (user_data_get()->motion.timer_en == false)
	{
		return true;
	}
	struct tm tm;
	user_time_read(&tm);
	int cur = user_tm_conver_sec(&tm);
	
	struct tm tm_start = (user_data_get()->motion.start);
	int start = user_tm_conver_sec(&tm_start);
	struct tm tm_end = (user_data_get()->motion.end);
	int end = user_tm_conver_sec(&tm_end);
	// if (end < start) 
	// {
	// 	end += 24 * 60 * 60;
	// }
	if ((cur > start) && (cur < end))
	{
		return true;
	}
	return false;
}

/***
**   日期:2022-06-09 11:40:01
**   作者: leo.liu
**   函数作用：定时超时处理
**   参数说明:
***/
static void motion_timer_check_task(lv_task_t *task)
{
	if (motion_timer_timeout_check() == true)
	{
		layout_motion_monitor_open();
		motion_detection_sensivity(user_data_get()->motion.sensivity);
		motion_detection_init();
		motion_detection_start();
		motion_detection_timestap = user_timestamp_get();
		lv_layout_task_create(motion_move_check_task, 200, LV_TASK_PRIO_MID, NULL);
		lv_task_del(task);
	}
}
/**************************************************************逻辑部分，以上**************************************************************/
typedef enum
{
	motion_scr_act_obj_id_head_cont,
} motion_scr_act_obj_id;
typedef enum
{
	motion_head_con_obj_id_rec_icon,
	motion_head_con_obj_id_channel_label,
	motion_head_con_obj_id_time_label,
	motion_head_con_obj_id_timeout_label,
	motion_head_con_obj_id_sd_icon
} motion_head_con_obj_id;
/***
** 日期: 2022-05-13 10:40
** 作者: leo.liu
** 函数作用：创建显示通道
** 返回参数说明：
***/
static void layout_motion_channel_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_label_create(parent, NULL);
	lv_obj_set_id(obj, motion_head_con_obj_id_channel_label);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 120, 23);
	lv_obj_set_size(obj, 80, 35);
	lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);

	char camera = user_data_get()->motion.select_camera;
	MON_CH ch = camera == 0 ? MON_CH_DOOR1 : camera == 1 ? MON_CH_DOOR2
					     : camera == 2   ? MON_CH_CCTV1
							     : MON_CH_CCTV2;
	const char *string = language_common_ch_string_get(ch);
	lv_label_set_text(obj, string);
}

/***
** 日期: 2022-05-13 11:17
** 作者: leo.liu
** 函数作用：时间文本希纳是
** 返回参数说明：
***/
static void layout_motion_time_label_display(lv_obj_t *label)
{
	if (label == NULL)
	{
		lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), motion_scr_act_obj_id_head_cont);
		label = lv_obj_get_child_form_id(parent, motion_head_con_obj_id_time_label);
	}
	struct tm *tm = (struct tm *)label->user_data;
	lv_label_set_text_fmt(label, "%04d-%02d-%02d %02d:%02d", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min);
}
/***
** 日期: 2022-05-13 10:53
** 作者: leo.liu
** 函数作用：创建时间
** 返回参数说明：
***/
static void layout_motion_time_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_label_create(parent, NULL);
	lv_obj_set_id(obj, motion_head_con_obj_id_time_label);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 200, 23);
	lv_obj_set_size(obj, 240, 35);
	lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);

	static struct tm tm;
	user_time_read(&tm);
	obj->user_data = &tm;
	layout_motion_time_label_display(obj);
}
/***
** 日期: 2022-05-13 11:15
** 作者: leo.liu
** 函数作用：超时文本显示
** 返回参数说明：
***/
static int layout_motion_timeout_label_display(lv_obj_t *parent)
{
	lv_obj_t *label = lv_obj_get_child_form_id(parent, motion_head_con_obj_id_timeout_label);
	int *ptimeout = (int *)label->user_data;
	int timeout = *ptimeout;
	if (timeout == 0)
	{
		*ptimeout = 60;
		// timeout = 10;
	}
	lv_label_set_text_fmt(label, "%02d:%02d", timeout / 60, timeout % 60);

	if (timeout != 0)
	{
		(*ptimeout)--;
	}
	return timeout;
}
/***
** 日期: 2022-05-13 10:53
** 作者: leo.liu
** 函数作用：创建超时文本
** 返回参数说明：
***/
static void layout_motion_timeout_label_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_label_create(parent, NULL);
	lv_obj_set_id(obj, motion_head_con_obj_id_timeout_label);
	lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
	lv_obj_set_pos(obj, 880, 23);
	lv_obj_set_size(obj, 100, 35);
	lv_label_set_align(obj, LV_LABEL_ALIGN_RIGHT);

	static int timeout = 0;
	timeout = 60;
	obj->user_data = &timeout;
	layout_motion_timeout_label_display(parent);
}

static void layout_motion_rec_icon_hidden(bool en)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), motion_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, motion_head_con_obj_id_rec_icon);
	if (lv_obj_get_hidden(obj) != en)
	{
		lv_obj_set_hidden(obj, en);
	}
}
/***
**   日期:2022-06-09 17:24:52
**   作者: leo.liu
**   函数作用：记录创建
**   参数说明:
***/
static void layout_motion_rec_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, motion_head_con_obj_id_rec_icon);
	lv_obj_set_pos(obj, 699, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_34_MONITORING_RECORDING_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
	layout_motion_rec_icon_hidden(true);
}
/***
** 日期: 2022-05-17 14:26
** 作者: leo.liu
** 函数作用：移动侦测图标创建
** 返回参数说明：
***/
static void layout_motion_motion_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 750, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_01_IC_IC_LIST_VISITOR_IMG_PNG);
	if (user_data_get()->motion.saving_fmt == 0)
	{
		img.offset = ROM_UI_01_IC_IC_LIST_VISITOR_IMG_PNG;
		img.size = ROM_UI_01_IC_IC_LIST_VISITOR_IMG_PNG_SIZE;
	}
	else
	{
		img.offset = ROM_UI_01_IC_IC_LIST_VISITOR_VIDEO_PNG;
		img.size = ROM_UI_01_IC_IC_LIST_VISITOR_VIDEO_PNG_SIZE;
	}
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
}

static void layout_motion_auto_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_pos(obj, 801, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_AUTO_PNG);
	if (user_data_get()->auto_record_mode == 0)
	{
		img.offset = ROM_UI_01_IC_IC_MONITORING_MANUAL_PNG;
		img.size = ROM_UI_01_IC_IC_MONITORING_MANUAL_PNG_SIZE;
	}
	else
	{
		img.offset = ROM_UI_01_IC_IC_MONITORING_AUTO_PNG;
		img.size = ROM_UI_01_IC_IC_MONITORING_AUTO_PNG_SIZE;
	}
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
}

static void layout_motion_sd_icon_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), motion_scr_act_obj_id_head_cont);
	lv_obj_t *icon = lv_obj_get_child_form_id(parent, motion_head_con_obj_id_sd_icon);
	if (media_sdcard_insert_check() == true)
	{
		// lv_obj_set_hidden(icon, false);
		lv_obj_set_state(icon, media_sdcard_full_check() == true ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
		// copy_flash_photo_to_sd();
	}
	else
	{
		// lv_obj_set_hidden(icon, true);
		lv_obj_set_state(icon, LV_STATE_FOCUSED);
	}
	if (video_record_status_get() == true)
	{
		video_record_stop();
	}

	if((media_sdcard_insert_check() == false) && (user_data_get()->auto_record_mode == 2)) {
		user_data_get()->auto_record_mode = 1;
	}
}

/***
** 日期: 2022-05-17 14:34
** 作者: leo.liu
** 函数作用：sd创建
** 返回参数说明：
***/
static void layout_motion_sd_icon_create(lv_obj_t *parent)
{
	lv_obj_t *obj = lv_obj_create(parent, NULL);
	lv_obj_set_id(obj, motion_head_con_obj_id_sd_icon);
	lv_obj_set_pos(obj, 852, 15);
	lv_obj_set_size(obj, 43, 43);
	lv_obj_set_click(obj, false);
	static rom_bin_info img_n = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_SDCARD_PNG);
	static rom_bin_info img_f = rom_bin_info_get(ROM_UI_01_IC_IC_SYSTEM_SDCARD_FULL_PNG);
	static rom_bin_info img_no_card = rom_bin_info_get(ROM_UI_MONITOR_NO_SD_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_n);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &img_f);
	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, &img_no_card);
}
static void layout_motion_timer_task(lv_task_t *task_t)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), motion_scr_act_obj_id_head_cont);
	lv_obj_t *time_label = lv_obj_get_child_form_id(parent, motion_head_con_obj_id_time_label);
	struct tm *label_tm = (struct tm *)time_label->user_data;
	struct tm cur_tm;
	user_time_read(&cur_tm);
	if (cur_tm.tm_min != label_tm->tm_min)
	{
		*label_tm = cur_tm;
		layout_motion_time_label_display(time_label);
	}
	int timeout = layout_motion_timeout_label_display(parent);
	if (timeout == 0)
	{
		if (video_record_status_get() == true)
		{
			video_record_stop();
		}
		audio_input_capture_enable(false);
		app_playback_record(false);

		layout_motion_restart_motion_detection();
		lv_task_del(task_t);
	}
	else if ((video_record_status_get() == false) && (mjpeg_encode_status_get() == false))
	{
		layout_motion_rec_icon_hidden(true);
	}
}



static void layout_close_back_craete_up(lv_obj_t *obj)
{
	goto_layout(pLAYOUT(home));
}




static void layout_close_back_btn_create(lv_obj_t *parent)
{
	
	lv_obj_t *obj = lv_btn_create(lv_scr_act(), NULL);
	if (obj == NULL)
	{
		printf("create cancel btn failed \n");
		return;
	}
	
	lv_obj_set_pos(obj, 27, 10);
	lv_obj_set_size(obj, 60, 60);

	static rom_bin_info image = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
	lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &image);

	lv_obj_set_style_local_pattern_align(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
	static obj_click_data click_data = obj_click_data_up_create(layout_close_back_craete_up);
	obj_click_event_listen(obj, &click_data);
}





/***
** 日期: 2022-05-13 10:29
** 作者: leo.liu
** 函数作用：创建顶部显示区域
** 返回参数说明：
***/
static void layout_motion_head_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, motion_scr_act_obj_id_head_cont);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 70);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));



	layout_motion_channel_label_create(cont);
	layout_motion_time_label_create(cont);
	layout_motion_timeout_label_create(cont);
	layout_close_back_btn_create(cont);

	layout_motion_rec_icon_create(cont);                   //记录标识
	layout_motion_motion_icon_create(cont);                //移动侦测标识
	layout_motion_auto_icon_create(cont);					//自动记录视频/照片
	layout_motion_sd_icon_create(cont);						//SD卡标识
	layout_motion_sd_icon_display();
}
/*************************************************************************
 * @brief  移动侦测处理函数
 * @date   2022-09-30 13:48
 * @author xiaoele
 **************************************************************************/
static void enable_motion_detection(void)
{
	if ((user_data_get()->motion.timer_en == false))
	{
		refresh_area_t area = {0, 0, LV_HOR_RES_MAX, 70};
		gui_refresh_area(&area, 1);
		layout_motion_monitor_open();		/* 开启移动侦测 */
		motion_detection_sensivity(user_data_get()->motion.sensivity);
		motion_detection_init();
		motion_detection_start();
		motion_detection_timestap = user_timestamp_get();
		lv_layout_task_create(motion_move_check_task, 200, LV_TASK_PRIO_MID, NULL);
	}
	else
	{
		lv_layout_task_create(motion_timer_check_task, 1000, LV_TASK_PRIO_MID, NULL);
	}

	layout_motion_head_cont_create();
	layout_sd_state_callback_register(layout_motion_sd_icon_display);
	h264_encode_capture_enable(true);
}
/*************************************************************************
 * @brief  移动侦测已开启,检查当前是否为忙碌状态
 * @date   2022-09-30 13:53
 * @author xiaoele
 **************************************************************************/
static void motion_device_is_busy_check_task(lv_task_t *task)
{
	LOG_WHITE("busy ack mask %d \n", intercom_busy_ack_mask_get());
	if(intercom_data_busy_get() == false)
	{
		enable_motion_detection();
		lv_task_del(task);
	}
	else
	{
		LOG_WHITE(" continue check busy \n");
	}
}
/*************************************************************************
 * @brief  壁纸已开启,检查当前是否为忙碌状态
 * @date   2022-09-30 13:53
 * @author an
 **************************************************************************/
static void frame_show_device_is_busy_check_task(lv_task_t *task)
{
	// LOG_WHITE("busy ack mask %d \n", intercom_busy_ack_mask_get());
	if((intercom_data_busy_get() == false) && (frame_display_timeout_check() == false) && (user_data_get()->display.frame_background != 2)  && (user_data_get()->display.frame_list & 0x1F))
	{
		goto_layout(pLAYOUT(frame_show));
		// lv_task_del(task);
	}
	else if(media_sdcard_insert_check() == true)
	{
		if (access(SD_PICTURE_PATH, F_OK) == 0)
		{
			int total = 0;
			media_file_total_get(FILE_TYPE_SD_PICTURE, &total, NULL);
			if(total > 0)
			{
				goto_layout(pLAYOUT(standby_picture));
			}
		}
	}
}

lv_task_t *audio_buffer_check_task = NULL;

static void standby_audio_buffer_check_task(lv_task_t *task)
{
	if(audio_buffer_check_task == NULL)
	{
		lv_task_del(task);
	}
	/*修改在长时间待机时call机没有铃声的问题*/
	if(audio_output_buffer_query() > 50*1024)
	{
		audio_output_device_restart();
	}
	printf("\n\n\n\naudio_output_buffer_query==[%d]\n\n\n\n",audio_output_buffer_query());
}
static void LAYOUT_ENTER_FUNC(close)
{
	printf("goto close motion\n");
	standby_timer_close();
	backlight_enable(false);
	close_cancel_btn_create();
	ringplay_play_stop();


	if ((OwnID == 1) && (user_data_get()->motion.enable == true)/* && (intercom_data_busy_get() == false)*/)
	{
		lv_task_t *task =  lv_layout_task_create(motion_device_is_busy_check_task, 3*1000, LV_TASK_PRIO_MID, NULL);
		lv_task_ready(task);
	}
	else if ((user_data_get()->display.standby_mode == 1) )
	{
		lv_task_t *task =  lv_layout_task_create(frame_show_device_is_busy_check_task, 2*1000, LV_TASK_PRIO_MID, NULL);
		lv_task_ready(task);
	}
	// else if(media_sdcard_insert_check() == true)
	// {
	// 	if (access(SD_PICTURE_PATH, F_OK) == 0)
	// 	{
	// 		int total = 0;
	// 		media_file_total_get(FILE_TYPE_SD_PICTURE, &total, NULL);
	// 		if(total > 0)
	// 		{
	// 			goto_layout(pLAYOUT(standby_picture));
	// 		}
	// 	}
	// }
	audio_buffer_check_task = lv_layout_task_create(standby_audio_buffer_check_task, 60*1000, LV_TASK_PRIO_MID, NULL);
}

static void LAYOUT_QUIT_FUNC(close)
{
	audio_buffer_check_task = NULL;
	tuya_api_time_sync();
	
	lv_obj_t *obj = lv_scr_act();
	obj->clict_data = NULL;
	lv_obj_set_event_cb(obj, NULL);
	lv_obj_t *obj1 = lv_obj_get_child_form_id(lv_scr_act(),LAYOUT_CLOSE_OBJ_ID_CANCEL);
	lv_obj_set_click(obj1,true);
	layout_sd_state_callback_register(NULL);
	audio_input_capture_enable(false);
	motion_detection_destory();
	h264_encode_capture_enable(false);
	record_video_close();
	app_playback_record(false);
	
	if (intercom_data_busy_get() == false)
	{
		monitor_close();
	}
	video_display_preview_enable(false);
	video_input_skip_frame_count_set(1000);
	fb_gui_layer_rect_fill(0x0, 0, 0, LV_HOR_RES_MAX, LV_VER_RES_MAX);
	screen_force_refresh();
	usleep(100 * 1000);
	standby_timer_restart(true);
	backlight_enable(true);

	lv_obj_set_style_local_pattern_image(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, NULL);
}

CREATE_LAYOUT(close);