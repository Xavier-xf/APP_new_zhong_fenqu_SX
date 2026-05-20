#include "layout_define.h"
lv_obj_t *layout_monitor_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data, rom_bin_info *img_1, rom_bin_info *img_2, int btn_id, bool mix);
static void layout_always_monitor_open(void);
static void layout_always_record_stop(void);
static void layout_always_record_delay_task(lv_task_t *task);
static void layout_always_snap_task_wait_finish_task(lv_task_t *task);
typedef enum
{
	always_record_scr_act_obj_id_head_cont,
	always_record_scr_act_obj_id_loop_obj,
	always_record_scr_act_obj_id_record_obj,
} always_record_scr_act_obj_id;
typedef enum
{
	always_record_head_cont_obj_id_cancel_obj,
	always_record_head_cont_obj_id_channel_label,
	always_record_head_cont_obj_id_time_label,
	always_record_head_cont_obj_id_motion_img,
	always_record_head_cont_obj_id_auto_img,
	always_record_head_cont_obj_id_sd_img,
	always_record_head_cont_obj_id_timeout_label
} always_record_head_cont_obj_id;
/***
**   日期:2022-06-20 14:42:46
**   作者: leo.liu
**   函数作用：记录时长
**   参数说明:
***/
static int always_record_time = 10;
void always_record_time_set(int sec)
{
	always_record_time = sec;
}
/***
**   日期:2022-06-20 14:42:46
**   作者: fei.liu
**   函数作用：判断是否在监控界面
**   参数说明:
***/
bool always_record_flg;
bool always_record_time_get(void)
{
	return always_record_flg;
}
/***
**   日期:2022-06-20 15:42:08
**   作者: leo.liu
**   函数作用：当前是循环还是单曲
**   参数说明:
***/
static bool always_record_loop = true;
/***
**   日期:2022-06-20 16:42:25
**   作者: leo.liu
**   函数作用：刷新任务
**   参数说明:
***/
static lv_task_t *always_record_task = NULL;
/***
**   日期:2022-06-20 15:33:20
**   作者: leo.liu
**   函数作用：设置刷新区域
**   参数说明:
***/
static void always_record_refresh_area(void)
{
	refresh_area_t area[] = {
	    {0, 0, 1024, 70},
	    {425, 497, 80, 80},
	    {519, 497, 80, 80}};
	gui_refresh_area(area, sizeof(area) / sizeof(refresh_area_t));
}
/***
**   日期:2022-06-20 14:54:24
**   作者: leo.liu
**   函数作用：创建顶部从其
**   参数说明:
***/
static lv_obj_t *always_record_head_cont_create(void)
{
	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_id(cont, always_record_scr_act_obj_id_head_cont);
	lv_obj_set_pos(cont, 0, 0);
	lv_obj_set_size(cont, 1024, 70);
	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00008));
	return cont;
}

static void always_record_cancel_btn_up(lv_obj_t *obj)
{
	always_record_flg = false;
	goto_layout(pLAYOUT(home));
}
/***
**   日期:2022-06-20 15:03:16
**   作者: leo.liu
**   函数作用：返回按钮显示
**   参数说明:
***/
static void always_record_cancel_btn_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, always_record_head_cont_obj_id_cancel_obj);
	if (obj == NULL)
	{
		obj = lv_obj_create(parent, NULL);
		lv_obj_set_id(obj, always_record_head_cont_obj_id_cancel_obj);
		lv_obj_set_pos(obj, 32, 15);
		lv_obj_set_size(obj, 43, 43);
		static rom_bin_info img = rom_bin_info_get(ROM_UI_BACK_NEW_PNG);
		lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
		static obj_click_data click_data = obj_click_data_up_create(always_record_cancel_btn_up);
		obj_click_event_listen(obj, &click_data);
	}
}
/***
**   日期:2022-06-20 14:58:42
**   作者: leo.liu
**   函数作用：创建通道显示
**   参数说明:
***/
static void always_record_channel_label_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, always_record_head_cont_obj_id_channel_label);
	if (obj == NULL)
	{
		obj = lv_label_create(parent, NULL);
		lv_obj_set_id(obj, always_record_head_cont_obj_id_channel_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_pos(obj, 100, 23);
		lv_obj_set_size(obj, 80, 35);
		lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);
	}
	lv_label_set_text(obj, language_common_ch_string_get(monitor_channel_get()));
}
/***
**   日期:2022-06-20 15:06:25
**   作者: leo.liu
**   函数作用：时间显示
**   参数说明:
***/
static void always_record_time_label_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, always_record_head_cont_obj_id_time_label);
	if (obj == NULL)
	{
		obj = lv_label_create(parent, NULL);
		lv_obj_set_id(obj, always_record_head_cont_obj_id_time_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_pos(obj, 190, 23);
		lv_obj_set_size(obj, 240, 35);
		lv_label_set_align(obj, LV_LABEL_ALIGN_LEFT);
	}
	static struct tm tm;
	user_time_read(&tm);
	lv_label_set_text_fmt(obj, "%04d-%02d-%02d %02d:%02d", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min);
	
}
/***
**   日期:2022-06-20 15:08:56
**   作者: leo.liu
**   函数作用：移动侦测
**   参数说明:
***/
static void always_record_motion_icon_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, always_record_head_cont_obj_id_motion_img);
	if (obj == NULL)
	{
		obj = lv_img_create(parent, NULL);
		lv_obj_set_id(obj, always_record_head_cont_obj_id_motion_img);
		lv_obj_set_pos(obj, 750, 15);
		lv_obj_set_size(obj, 43, 43);
		lv_obj_set_click(obj, false);
	}
	lv_obj_set_hidden(obj, user_data_get()->motion.enable == false ? true : false);
	rom_bin_info img_img = rom_bin_info_get(ROM_UI_01_IC_IC_LIST_VISITOR_IMG_PNG);
	rom_bin_info video_img = rom_bin_info_get(ROM_UI_01_IC_IC_LIST_VISITOR_VIDEO_PNG);
	lv_img_set_src(obj, user_data_get()->motion.saving_fmt == 0 ? (&img_img) : (&video_img));
}
/***
**   日期:2022-06-20 15:18:42
**   作者: leo.liu
**   函数作用：自动或手动
**   参数说明:
***/
static void always_record_auto_icon_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, always_record_head_cont_obj_id_auto_img);
	if (obj == NULL)
	{
		obj = lv_img_create(parent, NULL);
		lv_obj_set_id(obj, always_record_head_cont_obj_id_auto_img);
		lv_obj_set_pos(obj, 801, 15);
		lv_obj_set_size(obj, 43, 43);
		lv_obj_set_click(obj, false);
	}
	rom_bin_info m_img = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_MANUAL_PNG);
	rom_bin_info a_img = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_AUTO_PNG);
	lv_img_set_src(obj, user_data_get()->auto_record_mode == 0 ? (&m_img) : (&a_img));
}
/***
**   日期:2022-06-20 15:18:42
**   作者: leo.liu
**   函数作用：sd卡
**   参数说明:
***/
static void always_record_sd_icon_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, always_record_head_cont_obj_id_sd_img);
	if (obj == NULL)
	{
		obj = lv_img_create(parent, NULL);
		lv_obj_set_id(obj, always_record_head_cont_obj_id_sd_img);
		lv_obj_set_pos(obj, 852, 15);
		lv_obj_set_size(obj, 43, 43);
		lv_obj_set_click(obj, false);
	}
	lv_obj_set_hidden(obj, media_sdcard_insert_check() == true ? false : true);
	rom_bin_info n_img = rom_bin_info_get(ROM_UI_01_IC_IC_MONITORING_SDCARD_PNG);
	rom_bin_info f_img = rom_bin_info_get(ROM_UI_01_IC_IC_SYSTEM_SDCARD_FULL_PNG);
	lv_img_set_src(obj, media_sdcard_full_check() == false ? (&n_img) : (&f_img));
	
	if((media_sdcard_insert_check() == false) && (user_data_get()->auto_record_mode == 2)) {
		user_data_get()->auto_record_mode = 1;
	}
	// if(media_sdcard_insert_check() == true)
	// {
	// 	copy_flash_photo_to_sd();
	// }

	layout_always_record_stop();
}
static void layout_always_monitor_open_task(lv_task_t *task)
{
	/* if (always_record_loop == true) */
	// {
	
	layout_always_monitor_open();            //设置通道和打开监控
	always_record_channel_label_display();
	// }
	lv_task_del(task);
	always_record_task = lv_layout_task_create(layout_always_record_delay_task, 3000, LV_TASK_PRIO_MID, NULL);
}

// static void motion_device_detection_task(lv_task_t *task)
// {
// 	// LOG_WHITE("busy ack mask %d \n", intercom_busy_ack_mask_get());
// 	// if (video_input_state_get() == false)
// 	// {
// 		layout_always_monitor_open_task(task);
// 		lv_task_del(task);
// 		// return;
// 	// }

// }

static int no_signal_cont = 0;

static void always_record_tiemout_label_display(void)
{
	lv_obj_t *parent = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_head_cont);
	lv_obj_t *obj = lv_obj_get_child_form_id(parent, always_record_head_cont_obj_id_timeout_label);
	if (obj == NULL)
	{
		obj = lv_label_create(parent, NULL);
		lv_obj_set_id(obj, always_record_head_cont_obj_id_timeout_label);
		lv_label_set_long_mode(obj, LV_LABEL_LONG_CROP);
		lv_obj_set_pos(obj, 880, 23);
		lv_obj_set_size(obj, 100, 35);
		lv_label_set_align(obj, LV_LABEL_ALIGN_RIGHT);
		static int timeout = 0;
		timeout = always_record_time;
		obj->user_data = &timeout;
	}
	int *ptimeout = (int *)obj->user_data;
	int timeout = *ptimeout;
	lv_label_set_text_fmt(obj, "%02d:%02d", timeout / 60, timeout % 60);

	if (video_input_state_get() == false && no_signal_cont < 3)
	{
		no_signal_cont += 1;
		return;
	}

	if (timeout != 0)
	{
		(*ptimeout)--;
	}
	else
	{
		no_signal_cont = 0;
		(*ptimeout) = always_record_time;
		layout_always_record_stop();
		usleep(500 * 1000);
		/* if (always_record_loop == true) */
		// { 
		monitor_close();
		// }

		 if (always_record_task == NULL)
		 {
			always_record_task = lv_layout_task_create(layout_always_monitor_open_task, 1000, LV_TASK_PRIO_MID, NULL);
		 }
	}
}
/***
**   日期:2022-06-20 15:28:20
**   作者: leo.liu
**   函数作用：刷新时间
**   参数说明:
***/
static void always_record_timeout_refresh_task(lv_task_t *task)
{
	always_record_time_label_display();
	always_record_tiemout_label_display();
}

static void always_record_loop_btn_display(void);
static void always_record_loop_btn_up(lv_obj_t *obj)
{
	always_record_loop = always_record_loop ? false : true;
	always_record_loop_btn_display();
}
/***
**   日期:2022-06-20 15:37:11
**   作者: leo.liu
**   函数作用：循环或者单个
**   参数说明:
***/
static void always_record_loop_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_loop_obj);
	if (obj == NULL)
	{
		always_record_loop = true;
		static obj_click_data click_data = obj_click_data_up_create(always_record_loop_btn_up);
		static rom_bin_info img1 = rom_bin_info_get(ROM_UI_11_MONITORING_REPEAT_PNG);
		static rom_bin_info img2 = rom_bin_info_get(ROM_UI_10_MONITORING_FIXED_PNG);
		obj = layout_monitor_btn_base_create(NULL, 425, 497, 80, 80, &click_data, &img1, &img2, always_record_scr_act_obj_id_loop_obj, true);
	}
	lv_obj_set_state(obj, always_record_loop == false ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
}

static void always_record_record_btn_display(void);
static void always_record_record_btn_up(lv_obj_t *obj)
{
	if (mjpeg_encode_status_get() == true)
	{
		return;
	}


	if (video_record_status_get() == true)
	{
		video_record_stop();
	}
	// else if ((media_sdcard_insert_check() == true) && (video_input_state_get() == true))
	// {
	// 	record_video_start(REC_MODE_ALWAYS);
	// }

	else if ((media_sdcard_insert_check() == false) || ((media_sdcard_insert_check() == true) && (user_data_get()->auto_record_mode == 1)))
	{
		if (record_jpeg_start(REC_MODE_ALWAYS) == true)
		{
			lv_layout_task_create(layout_always_snap_task_wait_finish_task, 100, LV_TASK_PRIO_MID, NULL);
		}
	}
	else
	{
		record_video_start(REC_MODE_ALWAYS);
	}


	always_record_record_btn_display();
}
/***
**   日期:2022-06-20 15:43:45
**   作者: leo.liu
**   函数作用：记录按钮
**   参数说明:
***/
static void always_record_record_btn_display(void)
{
	lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), always_record_scr_act_obj_id_record_obj);
	if (obj == NULL)
	{
		always_record_loop = true;
		static obj_click_data click_data = obj_click_data_up_create(always_record_record_btn_up);
		obj = layout_monitor_btn_base_create(NULL, 519, 497, 80, 80, &click_data, NULL, NULL, always_record_scr_act_obj_id_record_obj, true);
	}

	static rom_bin_info img1 = rom_bin_info_get(ROM_UI_MONITOR_VIDEO_001_PNG);
	static rom_bin_info img2 = rom_bin_info_get(ROM_UI_MONITOR_VIDEO_002_PNG);
	static rom_bin_info img3 = rom_bin_info_get(ROM_UI_MONITOR_PHOTO_001_PNG);
	static rom_bin_info img4 = rom_bin_info_get(ROM_UI_MONITOR_PHOTO_002_PNG);
	if ((media_sdcard_insert_check() == true) && (user_data_get()->auto_record_mode == 2))
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img1);
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_CHECKED, &img2);
	}
	else
	{
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, &img3);
		lv_obj_set_style_local_pattern_image(obj, LV_BTN_PART_MAIN, LV_STATE_CHECKED, &img4);
	}

	if ((video_record_status_get() == true) || (mjpeg_encode_status_get() == true))
	{
		lv_obj_set_state(obj, LV_STATE_CHECKED);
	}
	else
	{
		lv_obj_set_state(obj, LV_STATE_DEFAULT);
	}
}
/***
**   日期:2022-06-20 16:12:19
**   作者: leo.liu
**   函数作用：获取通道
**   参数说明:
***/
extern TP9950_FORMAT video_door_format_check;
//bit4:表示四个设备已全部检测（1：成功）
// static uint8_t valid_ch = 0x0F;//用低四位表示通道的有效(0:无效 1:有效)，bit0：door1;    bit1：door2;    bit2：cctv1;   bit3：cctv2
static MON_CH always_record_channel_get(void)
{
	if ((monitor_valid_channel_check(MON_CH_DOOR1) == false) && 
	(monitor_valid_channel_check(MON_CH_DOOR2) == false) && 
	(monitor_valid_channel_check(MON_CH_CCTV1) == false) && 
	(monitor_valid_channel_check(MON_CH_CCTV2) == false) && 
	(monitor_valid_channel_check(MON_CH_CCTV3) == false) && 
	(monitor_valid_channel_check(MON_CH_CCTV4) == false))
	{
		return MON_CH_NONE;
	}
	MON_CH ch = monitor_channel_get();

	if (always_record_loop == false)
	{
		return ch;
	}
	if (ch == MON_CH_NONE)
	{
		ch = MON_CH_DOOR1;
		if ((monitor_valid_channel_check(ch) == true) /*&& (valid_ch & 0x01)*/)
		{
			return ch;
		}
	}
	int find = 0;
find_start:
	if( (ch == MON_CH_DOOR1))
	{		
		ch = MON_CH_DOOR2;
		if ((monitor_valid_channel_check(ch) == true) /*&& (valid_ch & 0x02)*/)
		{
			LOG_BLUE("MON_CH====== : 1\n");
			return ch;
		}
	}
	if ((ch == MON_CH_DOOR2))
	{
		ch = MON_CH_CCTV1;
		if ((monitor_valid_channel_check(ch) == true) /*&& (valid_ch & 0x04)*/)
		{
			LOG_BLUE("MON_CH====== : 2\n");
			return ch;
		}
	}
	if ((ch == MON_CH_CCTV1) )
	{
		ch = MON_CH_CCTV2;
		if ((monitor_valid_channel_check(ch) == true) /*&& (valid_ch & 0x08)*/)
		{
			LOG_BLUE("MON_CH====== : 3\n");
			return ch;
		}
	}
	if ((ch == MON_CH_CCTV2))
	{
		ch = MON_CH_CCTV3;
		if ((monitor_valid_channel_check(ch) == true)/*&& ((valid_ch & 0x01) || (valid_ch == 0x00))*/ )
		{
			LOG_BLUE("MON_CH====== : 4\n");
			return ch;
		}
	}
	if ((ch == MON_CH_CCTV3))
	{
		ch = MON_CH_CCTV4;
		if ((monitor_valid_channel_check(ch) == true)/*&& ((valid_ch & 0x01) || (valid_ch == 0x00))*/ )
		{
			LOG_BLUE("MON_CH====== : 5\n");
			return ch;
		}
	}
	if ((ch == MON_CH_CCTV4))
	{
		ch = MON_CH_DOOR1;
		if ((monitor_valid_channel_check(ch) == true)/*&& ((valid_ch & 0x01) || (valid_ch == 0x00))*/ )
		{
			LOG_BLUE("MON_CH====== : 6\n");
			return ch;
		}
	}
	find++;
	if (find == 2)
	{
		return MON_CH_NONE;
	}
	goto find_start;
}
static void layout_always_monitor_open(void)
{	
	MON_CH ch = always_record_channel_get();
	// printf("====================>>> ch : [%d] valid_ch:[0x%x]\n", ch, valid_ch);
	if( (monitor_valid_channel_check(ch) == true) )
	{
		// printf("====================>>> ,,,,,,,,,,,,,,,,,\n");
		monitor_channel_set(ch);
		monitor_open(true, 0x01);
	}
}
static void layout_always_snap_task_wait_finish_task(lv_task_t *task)
{
	if (mjpeg_encode_status_get() == true)
	{
		return;
	}
	always_record_record_btn_display();
	lv_task_del(task);
}
static void layout_always_record_start(void)
{
	if (mjpeg_encode_status_get() == true)
	{
		return;
	}

	if(user_data_get()->auto_record_mode == 0)
	{
		return;
	}

	if (video_record_status_get() == true)
	{
		video_record_stop();
	}
	else if ((media_sdcard_insert_check() == false) || ((media_sdcard_insert_check() == true) && (user_data_get()->auto_record_mode == 1)))
	{
		if (record_jpeg_start(REC_MODE_ALWAYS) == true)
		{
			lv_layout_task_create(layout_always_snap_task_wait_finish_task, 100, LV_TASK_PRIO_MID, NULL);
		}
	}
	else
	{
		user_data_get()->get_record_flag = 2;
		record_video_start(REC_MODE_ALWAYS);
	}
	always_record_record_btn_display();
}
static void layout_always_record_stop(void)
{
	if (video_record_status_get() == true)
	{
		video_record_stop();
	}
	always_record_record_btn_display();
}
/***
**   日期:2022-06-20 16:38:19
**   作者: leo.liu
**   函数作用：创建任务等待录像
**   参数说明:
***/
static void layout_always_record_delay_task(lv_task_t *task)
{
	// if(always_record_task != NULL) 
	// {
	// 	return;
	// }
	if (video_input_state_get() == true)
	{
		layout_always_record_start();
	}

	// if(!(valid_ch & 0x10))
	// {
	// 	MON_CH ch = monitor_channel_get();
	// 	if(video_door_format_check != INVALID_FORMAT)
	// 	{
	// 		valid_ch |= (0x01 << (ch - 1));
	// 	}
	// 	else
	// 	{
	// 		valid_ch &= ~(0x01 << (ch - 1));
	// 	}
	// 	if(ch == MON_CH_CCTV2)
	// 	{
	// 		valid_ch |= 0x10;
	// 	}
	// }
	

	lv_task_del(task);
	always_record_task = NULL;
}

static void LAYOUT_ENTER_FUNC(always_record)
{
	// intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
	// intercom_data_busy_enbale(true);
	// intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
	always_record_flg = true;
	always_record_loop = true;
	monitor_channel_set(MON_CH_NONE);
	layout_always_monitor_open();
	always_record_refresh_area();
	always_record_head_cont_create();
	always_record_cancel_btn_display();
	always_record_channel_label_display();
	always_record_time_label_display();
	always_record_motion_icon_display();
	always_record_auto_icon_display();
	always_record_sd_icon_display();
	always_record_tiemout_label_display();
	always_record_loop_btn_display();
	always_record_record_btn_display();
	layout_sd_state_callback_register(always_record_sd_icon_display);
	lv_layout_task_create(always_record_timeout_refresh_task, 1000, LV_TASK_PRIO_MID, NULL);
	standby_timer_close();
	h264_encode_capture_enable(true);
	audio_input_capture_enable(true);
	always_record_task = lv_layout_task_create(layout_always_record_delay_task, 3000, LV_TASK_PRIO_MID, NULL);

}
static void LAYOUT_QUIT_FUNC(always_record)
{
	record_video_close();
	// intercom_data_busy_enbale(false);
	// intercom_cmd_send(CODE_ALL_ID, CMD_DATA_REBOOT);
	h264_encode_capture_enable(false);
	audio_input_capture_enable(false);
	video_display_preview_enable(false);
	video_input_skip_frame_count_set(1000);
	fb_gui_layer_rect_fill(0x0, 0, 0, 1024, 600);
	screen_force_refresh();
	monitor_close();
	layout_sd_state_callback_register(NULL);

	standby_timer_restart(true);
}
CREATE_LAYOUT(always_record);