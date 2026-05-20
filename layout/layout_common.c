#include "layout_define.h"
#include "ringplay.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_p2p.h"
#include "ak_common_audio.h"
#include "audio_output.h"

static lv_task_t * tuya_quit_task = NULL;
bool sleep_mode_enter_moniter_flag = false;
static bool tuya_outdoor_open_flag = false;
static bool tuya_gate_open_flag = false;
//door_n:0:door1 1:door2
#define RING_TIME_CHECK(door_n, ring_n) (user_data_get()->ring_attr[door_n][ring_n].timer_start < user_data_get()->ring_attr[door_n][ring_n].timer_end &&  \
										user_data_get()->ring_attr[door_n][ring_n].timer_start <= (tm.tm_hour*100 + tm.tm_min) && 							\
										user_data_get()->ring_attr[door_n][ring_n].timer_end > (tm.tm_hour*100 + tm.tm_min)) || 							\
										(user_data_get()->ring_attr[door_n][ring_n].timer_start > user_data_get()->ring_attr[door_n][ring_n].timer_end && 	\
										user_data_get()->ring_attr[door_n][ring_n].timer_start <= (tm.tm_hour*100 + tm.tm_min) && 											\
										(user_data_get()->ring_attr[door_n][ring_n].timer_end + 24*60*60) > (tm.tm_hour*100 + tm.tm_min))


uint8_t door_x = 0, ring_x = 0;


static bool tuya_talk_outdoor_unclock = false;
bool tuya_outdoor_open_enable_get(void)
{
	return tuya_talk_outdoor_unclock;
}

/***
**   日期:2022-06-27 14:34:04
**   作者: leo.liu
**   函数作用：播放按键音 
**   参数说明:
***/
void ringplay_keysound_start_default_func(int index)
{
	ring_output_gpio_ctrl_volume(3);
}
/***
** 日期: 2022-05-20 17:30
** 作者: leo.liu
** 函数作用：铃声结束的默认处理
** 返回参数说明：
***/
void ringplay_keysound_finish_default_func(int index)
{
	// LOG_RED("AMP 75 ctrl close\n");
	/***** 关闭功放 *****/
	if(cur_layout_get() != pLAYOUT(intercom_talk))
	{
		power_amplifier_enable(false);
	}
}

/***
** 日期: 2022-04-28 09:53
** 作者: leo.liu
** 函数作用：控件被按下执行的回调函数
** 返回参数说明：
***/
void layout_obj_click_down_func(lv_obj_t *obj)
{
	if(user_data_get()->home_mode == 1)
	{
		return;
	}
	//***** 控制声音流向 *****/
	touch_sound_play(ringplay_keysound_start_default_func, ringplay_keysound_finish_default_func);
}

/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：报警,留言铃声开始
** 返回参数说明：
***/
void ringplay_doorcall_start_default_func(int index)
{
	MON_CH ch = monitor_channel_get();
	if(OwnID == 1)
	{
		call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, true);
	}
	else
	{
		call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, false);
	}
}

/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声开始
** 返回参数说明：
***/
void ringplay_doorcall_rings_start_default_func(int index)
{
	MON_CH ch = monitor_channel_get();
	if(OwnID == 1)
	{
		call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, true);
	}
	if((user_data_get()->home_mode == 1) || (user_data_get()->ring_attr[door_x][ring_x].ring_val == 0))
	{
		sleep_mode_enter_moniter_flag = true;
	}
	ring_output_gpio_ctrl_volume(user_data_get()->ring_attr[door_x][ring_x].ring_val);
}
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：门口机铃声结束
** 返回参数说明：
***/
void ringplay_doorcall_finish_default_func(int index)
{
	/***** 开启功放 *****/
	MON_CH ch = monitor_channel_get();
	sleep_mode_enter_moniter_flag = false;

	// power_amplifier_enable(false);                               //进入监控页面可以不关功放
	call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, false);
	
}


/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：tuya使能门口机铃声结束
** 返回参数说明：
***/
void ringplay_tuya_doorcall_finish_default_func(int index)
{
	/***** 开启功放 *****/
	MON_CH ch = monitor_channel_get();
	sleep_mode_enter_moniter_flag = false;

	// power_amplifier_enable(false);                               //进入监控页面可以不关功放
	call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, false);
	tuya_api_alam_report(0);
	
}

/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：intercom铃声开始
** 返回参数说明：
***/
void ringplay_intercom_start_default_func(int index)
{
	// if(user_data_get()->audio.inter_ring_volume == 0)
	// {
	// 	ringplay_play_stop();
	// 	sleep_mode_enter_moniter_flag = true;
	// }
	tuya_spk_sw_pin_enable(false);
	// intercom_ringout_enable(true);
	ring_output_gpio_ctrl_volume(user_data_get()->audio.inter_ring_volume);
}
/***
** 日期: 2022-05-21 08:12
** 作者: leo.liu
** 函数作用：inter铃声结束
** 返回参数说明：
***/
void ringplay_intercom_finish_default_func(int index)
{
	/***** 开启功放 *****/
	// sleep_mode_enter_moniter_flag = false;
	
	power_amplifier_enable(false);
	intercom_ring_out_pin_enable(false);
}

/***
**   日期:2022-07-14 16:46:04
**   作者: leo.liu
**   函数作用：开室内机锁铃声播放前的回调处理
**   参数说明:
***/
void gate_open_ringplay_start_default_func(int index)
{
	ring_to_outdoor_mute_pin_ctrl(true);
}
/***
**   日期:2022-07-14 16:46:47
**   作者: leo.liu
**   函数作用：开室内机锁铃声结束的回调处理函数
**   参数说明:
***/
void gate_open_ringplay_finish_default_func(int index)
{
	tuya_spk_sw_pin_enable(true);
	call_ring_to_outdoor_ctrl(AUDIO_CH_DOOR1, false);
}
/***
**   日期:2022-07-14 16:46:04
**   作者: leo.liu
**   函数作用：开门口机锁铃声播放前的回调处理
**   参数说明:
***/
void door_open_ringplay_start_default_func(int index)
{

	MON_CH ch = monitor_channel_get();
	if(OwnID == 1)
	{
		call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, true);
	}
	/*美欧反馈播放开锁铃声小，故通话开锁时关闭34118EN，之后再打开*/
	if(layout_monitor_talk_state() == true)
	{
		il34118_enable_pin_ctrl(false);
	}
	sleep_mode_enter_moniter_flag = true;
	if(tuya_api_app_talk_check())
	{
		audio_input_close(0x02);
		audio_input_capture_enable(false);
	}
	
}
/***
**   日期:2022-07-14 16:46:47
**   作者: leo.liu
**   函数作用：开门口机锁铃声结束的回调处理函数
**   参数说明:
***/
void door_open_ringplay_finish_default_func(int index)
{
	MON_CH ch = monitor_channel_get();
	sleep_mode_enter_moniter_flag = false;
	tuya_talk_outdoor_unclock = false;
	call_ring_to_outdoor_ctrl(ch == MON_CH_DOOR1 ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2, false);
	
	if(layout_monitor_talk_state() == true)
	{
		il34118_enable_pin_ctrl(true);
	}
	if(tuya_api_app_talk_check())
	{
		audio_output_open(AUDIO_CHANNEL_MONO, AK_AUDIO_SAMPLE_RATE_8000);
		audio_input_capture_enable(true);
		audio_input_open(0x02);
	}
	
}


static lv_task_t * momitor_ring_timer_task_t = NULL;
void momitor_ring_timer_task(lv_task_t *task)
{
	ringplay_play_stop();
	if(momitor_ring_timer_task_t != NULL)
	{
		lv_task_del(momitor_ring_timer_task_t);
		momitor_ring_timer_task_t = NULL;
	}
}

void monitor_ring_task_kill(void)
{
	LOG_WHITE("task kill \n");
	// ringplay_play_stop();
	if(momitor_ring_timer_task_t != NULL)
	{
		lv_task_del(momitor_ring_timer_task_t);
		momitor_ring_timer_task_t = NULL;
	}
}
/***
** 日期: 2022-05-12 10:27
** 作者: leo.liu
** 函数作用：door1 call 默认处理函数
** 返回参数说明：
***/
void layout_door1_call_default(void)
{
	
	MON_ENTER_FLG flg = monitor_enter_mask_get();
	MON_CH ch = monitor_channel_get();
	monitor_valid_channel_set(MON_CH_DOOR1, true);
	if ((flg == MON_ENTER_TALK) || ((ch == MON_CH_DOOR1) && (flg == MON_ENTER_CALL)))
	{
		if (ch != MON_CH_DOOR1)
		{
			monitor_channel_set(MON_CH_DOOR1);
			monitor_open(false, 0x02);
		}
		if (OwnID == 1)
		{
			intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR1_CALL);
		}
		intercom_audio_enable(AUDIO_CH_DOOR1);
		
	}
	else
	{
		
		MON_CH old_ch = monitor_channel_get();
		monitor_channel_set(MON_CH_DOOR1);
		if (tuya_api_app_preview_check() == false)
		{
			monitor_enter_mask_set(MON_ENTER_CALL);
			monitor_unlock_mask_set(MON_UNLOCK_CALL);            //开锁的标志位

			if (cur_layout_get() == pLAYOUT(alarm))
			{
				alarm_trigger_call_set(true);
			} 
			else if((cur_layout_get() == pLAYOUT(close))&&(mjpeg_encode_status_get() == true))     // 增加延时，防止再close界面的时候call，导致出现的记录“黑图”
			{
				usleep(500*1000);
			}

			goto_layout(pLAYOUT(monitor));
			if (OwnID == 1)
			{
				h264_encode_sample_enable(false);
				layout_intercom_call_mask_set(0x00);
				intercom_cmd_send(CODE_ALL_ID, CMD_DOOR1_CALL);
			}
			
			
		}
		else if ((is_monior_enable() == false) || (old_ch != MON_CH_DOOR1))
		{
			monitor_open(false, 0x03);
		}
		const layout *playout = cur_layout_get();
		
		if(playout == pLAYOUT(tuya_monitor))
		{

			// monitor_channel_set(MON_CH_DOOR1);
			
			if(monitor_channel_get() == MON_CH_DOOR2)
			{
				monitor_open(false, 0x03);
			}
			
			if(tuya_api_app_talk_check())
			{
				tuya_spk_sw_pin_enable(true);
				tuya_mic_sw_pin_enable(true);
				voice_record_pin_enable(false);
				ring_to_outdoor_mute_pin_ctrl(true);

				MON_CH ch = monitor_channel_get();
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
				if((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
				{
					door_audio_talk(AUDIO_CH_CLOSE);
				}
				
			}
			layout_monitor_report_vaild_channel();
			
		}
		if(tuya_api_app_talk_check() == false)
		{
			if (alarm_trigger_call_get() == true)
			{
				ringplay_play_form_index(8, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
			}
			else
			{
				if(user_data_get()->home_mode == 0)
				{
					// if (media_sdcard_insert_check() == true)
					// {
					// 	extern void layout_monitor_record_leave_home(void);
					// 	record_video_start(REC_MODE_MANUAL);
					// 	layout_monitor_record_leave_home();
					// }
					ringplay_play_form_index(10,100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
					return;
				}
				
				struct tm tm;
				user_time_read(&tm);

				if(RING_TIME_CHECK(0, 0))
				{
					door_x = 0, ring_x = 0;
					// if( user_data_get()->ring_attr[0][0].ring_val == 0) 
					// {
					// 	power_amplifier_enable(false);
					// }
					if((user_data_get()->ring_attr[0][0].ring_mode == 1) && (media_sdcard_insert_check() == true))
					{ 					
						custom_music_play(user_data_get()->ring_attr[0][0].custom_ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					else /*if(user_data_get()->ring_attr[0][0].ring_mode == 0)*/
					{
						ringplay_play_form_index(user_data_get()->ring_attr[0][0].ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					
				}
				else if(RING_TIME_CHECK(0, 1)) 
				{
					door_x = 0, ring_x = 1;
					// if( user_data_get()->ring_attr[0][1].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					if((user_data_get()->ring_attr[0][1].ring_mode == 1) && (media_sdcard_insert_check() == true))
					{
						custom_music_play(user_data_get()->ring_attr[0][1].custom_ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					else /*if(user_data_get()->ring_attr[0][1].ring_mode == 0)*/
					{
						ringplay_play_form_index(user_data_get()->ring_attr[0][1].ring,100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					
				}
				else if(RING_TIME_CHECK(0, 2))
				{
					door_x = 0, ring_x = 2;
					// if( user_data_get()->ring_attr[0][2].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					if((user_data_get()->ring_attr[0][2].ring_mode == 1) && (media_sdcard_insert_check() == true))
					{
						custom_music_play(user_data_get()->ring_attr[0][2].custom_ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					else /*if(user_data_get()->ring_attr[0][2].ring_mode == 0)*/
					{
						ringplay_play_form_index(user_data_get()->ring_attr[0][2].ring,100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					
				}
				else
				{
					ringplay_play_form_index(user_data_get()->ring_attr[0][0].ring,100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
				}
				
				monitor_ring_task_kill();
				momitor_ring_timer_task_t = lv_layout_task_create(momitor_ring_timer_task, user_data_get()->ring_attr[door_x][ring_x].ring_time * 1000, LV_TASK_PRIO_MID, NULL);
				momitor_ring_timer_task_t->clean_lock = 0;
			}
		}
	}
	
}
/***
** 日期: 2022-05-12 10:27
** 作者: leo.liu
** 函数作用：door2 call 默认处理函数
** 返回参数说明：通道设置必须放在goto后面，
***/
void layout_door2_call_default(void)
{
	MON_ENTER_FLG flg = monitor_enter_mask_get();
	MON_CH ch = monitor_channel_get(); 
	monitor_valid_channel_set(MON_CH_DOOR2, true);
	
	if ((flg == MON_ENTER_TALK) || ((ch == MON_CH_DOOR2) && (flg == MON_ENTER_CALL)))
	{
		if (ch != MON_CH_DOOR2)
		{
			monitor_channel_set(MON_CH_DOOR2);
			
			monitor_open(false, 0x02);
		}
		if (OwnID == 1)
		{
			intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR2_CALL);
		}
		intercom_audio_enable(AUDIO_CH_DOOR2);
		
	}
	else 
	{
		MON_CH old_ch = monitor_channel_get();
		monitor_channel_set(MON_CH_DOOR2);
		if (tuya_api_app_preview_check() == false)
		{
			monitor_enter_mask_set(MON_ENTER_CALL);
			monitor_unlock_mask_set(MON_UNLOCK_CALL);

			if (cur_layout_get() == pLAYOUT(alarm))
			{
				alarm_trigger_call_set(true);
			}
			else if((cur_layout_get() == pLAYOUT(close))&&(mjpeg_encode_status_get() == true))
			{
				usleep(500*1000);
			}

			goto_layout(pLAYOUT(monitor));
			if (OwnID == 1)
			{
				h264_encode_sample_enable(false);
				layout_intercom_call_mask_set(0x00);
				intercom_cmd_send(CODE_ALL_ID, CMD_DOOR2_CALL);
			}
			
		}
		else if ((is_monior_enable() == false) || (old_ch != MON_CH_DOOR2))
		{
			
			monitor_open(false, 0x03);
		}
		
		const layout *playout = cur_layout_get();
		if(playout == pLAYOUT(tuya_monitor))
		{
			// monitor_channel_set(MON_CH_DOOR2);
			if(monitor_channel_get() == MON_CH_DOOR1)
			{
				monitor_open(false, 0x03);
			}
			
			if(tuya_api_app_talk_check())
			{
				
				tuya_spk_sw_pin_enable(true);
				tuya_mic_sw_pin_enable(true);
				voice_record_pin_enable(false);
				ring_to_outdoor_mute_pin_ctrl(true);


				MON_CH ch = monitor_channel_get();
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
				if((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
				{
					door_audio_talk(AUDIO_CH_CLOSE);
				}
			}
			layout_monitor_report_vaild_channel();
			
			
		}
		if(tuya_api_app_talk_check() == false)
		{
			if (alarm_trigger_call_get() == true)
			{
				ringplay_play_form_index(8, 100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
			}
			else
			{
				if(user_data_get()->home_mode == 0)
				{
					// if (media_sdcard_insert_check() == true)
					// {
					// 	extern void layout_monitor_record_leave_home(void);
					// 	record_video_start(REC_MODE_MANUAL);
					// 	layout_monitor_record_leave_home();
					// }
					ringplay_play_form_index(10,100, ringplay_doorcall_start_default_func, ringplay_doorcall_finish_default_func, false);
					return;
				}
				
				struct tm tm;
				user_time_read(&tm);

				
				if(RING_TIME_CHECK(1, 0))
				{
					door_x = 1, ring_x = 0;
					// if( user_data_get()->ring_attr[1][0].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					
					if((user_data_get()->ring_attr[1][0].ring_mode == 1) && (media_sdcard_insert_check() == true))
					{
						
						custom_music_play(user_data_get()->ring_attr[1][0].custom_ring, 100 ,ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					else /*if(user_data_get()->ring_attr[1][0].ring_mode == 0)*/
					{
						ringplay_play_form_index(user_data_get()->ring_attr[1][0].ring,100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					
					
				}
				else if(RING_TIME_CHECK(1, 1))
				{
					
					door_x = 1, ring_x = 1;
					// if( user_data_get()->ring_attr[1][1].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					if((user_data_get()->ring_attr[1][1].ring_mode == 1) && (media_sdcard_insert_check() == true))
					{
						
						custom_music_play(user_data_get()->ring_attr[1][1].custom_ring,100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					else /*if(user_data_get()->ring_attr[1][1].ring_mode == 0)*/
					{
						
						ringplay_play_form_index(user_data_get()->ring_attr[1][1].ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					
				}
				else if(RING_TIME_CHECK(1, 2))
				{
					printf("display music ring3\n");
					door_x = 1, ring_x = 2;
					// if( user_data_get()->ring_attr[1][2].ring_val == 0)
					// {
					// 	power_amplifier_enable(false);
					// }
					if((user_data_get()->ring_attr[1][2].ring_mode == 1) && (media_sdcard_insert_check() == true))
					{
						
						custom_music_play(user_data_get()->ring_attr[1][2].custom_ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					else /*if(user_data_get()->ring_attr[1][2].ring_mode == 0)*/
					{
						
						ringplay_play_form_index(user_data_get()->ring_attr[1][2].ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
					}
					
				}
				else
				{
					
					ringplay_play_form_index(user_data_get()->ring_attr[1][0].ring, 100, ringplay_doorcall_rings_start_default_func, ringplay_doorcall_finish_default_func, true);
				}
				monitor_ring_task_kill();
				
				// printf("rings playing time ");
				momitor_ring_timer_task_t = lv_layout_task_create(momitor_ring_timer_task, user_data_get()->ring_attr[door_x][ring_x].ring_time * 1000, LV_TASK_PRIO_MID, NULL);
				momitor_ring_timer_task_t->clean_lock = 0;
			}
		}
	}
	
}
/***
**   日期:2022-05-28 08:11:18
**   作者: leo.liu
**   函数作用：警报触发默认处理
**   参数说明:
***/
void layout_alarm1_trigger_default(void)
{
	alarm_trigger_call_set(false);
	user_data_get()->alarm.alarm_1_trigger = true;
	user_data_save();
	goto_layout(pLAYOUT(alarm));
	if (OwnID == 1)
	{
		intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_SENSOR1);
	}
}
/***
**   日期:2022-05-28 08:12:01
**   作者: leo.liu
**   函数作用：警报触发默认处理
**   参数说明:
***/
void layout_alarm2_trigger_default(void)
{
	printf("========++++++>>>>>>>>158484\n");
	alarm_trigger_call_set(false);
	user_data_get()->alarm.alarm_2_trigger = true;
	user_data_save();
	goto_layout(pLAYOUT(alarm));
	if (OwnID == 1)
	{
		intercom_cmd_send(CODE_ALL_ID, CMD_ALARM_SENSOR2);
	}
}
/***
**   日期:2022-07-11 14:12:55
**   作者: leo.liu
**   函数作用：intercom呼叫处理
**   参数说明:
***/
bool layout_intercom_trigger_default(unsigned int arg1, unsigned int arg2)
{
	GPIO_LEVEL leve = (GPIO_LEVEL)arg1;
	if (leve == GPIO_LEVEL_LOW)
	{
		// intercom_talk_status_is_in();
		goto_layout(pLAYOUT(intercom));
	}
	else
	{
		ringplay_play_stop();
	}
	return true;
}
static MON_ENTER_FLG layout_monitor_enter_flag = MON_ENTER_NONE;
static MON_UNLOCK_FLG layout_monitor_unlock_flag = MON_UNLOCK_NONE;

/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：设置进入监控的标志位
** 返回参数说明：
***/
void monitor_enter_mask_set(MON_ENTER_FLG flg)
{
	layout_monitor_enter_flag = flg;
}

/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：获取进入监控的标志位
** 返回参数说明：
***/
MON_ENTER_FLG monitor_enter_mask_get(void)
{
	return layout_monitor_enter_flag;
}
/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：设置进入监控开锁的标志位
** 返回参数说明：
***/
void monitor_unlock_mask_set(MON_UNLOCK_FLG flg)
{
	layout_monitor_unlock_flag = flg;
}
/***
** 日期: 2022-05-13 11:05
** 作者: leo.liu
** 函数作用：获取进入监控开锁的标志位
** 返回参数说明：
***/
MON_UNLOCK_FLG monitor_unlock_mask_get(void)
{
	return layout_monitor_unlock_flag;
}
/***
** 日期: 2022-05-17 08:13
** 作者: leo.liu
** 函数作用：获取当前通道的亮度值
** 返回参数说明：
***/
int monitor_display_brightness_vol_get(void)
{
	MON_CH channel = monitor_channel_get();
	if (channel == MON_CH_DOOR1)
	{
		return user_data_get()->display.door1.bright;
	}
	else if (channel == MON_CH_DOOR2)
	{
		return user_data_get()->display.door2.bright;
	}
	else if (channel == MON_CH_CCTV1)
	{
		return user_data_get()->display.cctv1.bright;
	}
	else
	{
		return user_data_get()->display.cctv2.bright;
	}
}

/***
** 日期: 2022-05-17 08:15
** 作者: leo.liu
** 函数作用：设置亮度值
** 返回参数说明：
***/
void monitor_display_brightness_vol_set(int vol)
{
	MON_CH channel = monitor_channel_get();
	if (channel == MON_CH_DOOR1)
	{
		user_data_get()->display.door1.bright = vol;
	}
	else if (channel == MON_CH_DOOR2)
	{
		user_data_get()->display.door2.bright = vol;
	}
	else if (channel == MON_CH_CCTV1)
	{
		user_data_get()->display.cctv1.bright = vol;
	}
	else
	{
		user_data_get()->display.cctv2.bright = vol;
	}
	user_data_save();
}

/***
** 日期: 2022-05-17 08:13
** 作者: leo.liu
** 函数作用：获取当前通道的对比度值
** 返回参数说明：
***/
int monitor_display_cont_vol_get(void)
{
	MON_CH channel = monitor_channel_get();
	if (channel == MON_CH_DOOR1)
	{
		return user_data_get()->display.door1.cont;
	}
	else if (channel == MON_CH_DOOR2)
	{
		return user_data_get()->display.door2.cont;
	}
	else if (channel == MON_CH_CCTV1)
	{
		return user_data_get()->display.cctv1.cont;
	}
	else
	{
		return user_data_get()->display.cctv2.cont;
	}
}

/***
** 日期: 2022-05-17 08:15
** 作者: leo.liu
** 函数作用：设置亮度值
** 返回参数说明：
***/
void monitor_display_cont_vol_set(int vol)
{
	MON_CH channel = monitor_channel_get();
	if (channel == MON_CH_DOOR1)
	{
		user_data_get()->display.door1.cont = vol;
	}
	else if (channel == MON_CH_DOOR2)
	{
		user_data_get()->display.door2.cont = vol;
	}
	else if (channel == MON_CH_CCTV1)
	{
		user_data_get()->display.cctv1.cont = vol;
	}
	else
	{
		user_data_get()->display.cctv2.cont = vol;
	}
	user_data_save();
}

/***
** 日期: 2022-05-17 08:13
** 作者: leo.liu
** 函数作用：获取当前通道的色度值
** 返回参数说明：
***/
int monitor_display_color_vol_get(void)
{
	MON_CH channel = monitor_channel_get();
	if (channel == MON_CH_DOOR1)
	{
		return user_data_get()->display.door1.color;
	}
	else if (channel == MON_CH_DOOR2)
	{
		return user_data_get()->display.door2.color;
	}
	else if (channel == MON_CH_CCTV1)
	{
		return user_data_get()->display.cctv1.color;
	}
	else
	{
		return user_data_get()->display.cctv2.color;
	}
}

/***
** 日期: 2022-05-17 08:15
** 作者: leo.liu
** 函数作用：设置色度值
** 返回参数说明：
***/
void monitor_display_color_vol_set(int vol)
{
	MON_CH channel = monitor_channel_get();
	if (channel == MON_CH_DOOR1)
	{
		user_data_get()->display.door1.color = vol;
	}
	else if (channel == MON_CH_DOOR2)
	{
		user_data_get()->display.door2.color = vol;
	}
	else if (channel == MON_CH_CCTV1)
	{
		user_data_get()->display.cctv1.color = vol;
	}
	else
	{
		user_data_get()->display.cctv2.color = vol;
	}
	user_data_save();
}

/************************************************************** tuya处理  **************************************************************/
/***
**   日期:2022-06-07 16:36:51
**   作者: leo.liu
**   函数作用：tuya通话状态
**   参数说明:
***/
static bool is_tuya_app_talk = false;

/***
**   日期:2022-06-09 17:52:36
**   作者: leo.liu
**   函数作用：涂鸦监控中
**   参数说明:
***/
static bool is_tuya_app_preview = false;
bool tuya_app_preview_check(void)
{
	return is_tuya_app_preview;
}

void tuya_app_preview_enable(bool en){

	is_tuya_app_preview = en;
}
#if 0
/***
**   日期:2022-06-07 16:30:46
**   作者: leo.liu
**   函数作用：判断设备正忙
**   参数说明:
***/
static bool tuya_preview_busy_check(void)
{
	return layout_monitor_talk_state();
}
#endif



static MON_CH tuya_monitor_channel = MON_CH_DOOR1;
/***
**   日期:2022-06-07 15:43:30
**   作者: leo.liu
**   函数作用：tuya查看视频
**   参数说明:
***/
static int tuya_pview_video_count = 0;
void tuya_pvew_video_count_fource_set(int cout){

	tuya_pview_video_count = cout;
}
int tuya_pvew_video_count_fource_get(void){

	return tuya_pview_video_count;
}
/***
**   日期:2023-01-10 15:43:30
**   作者: leo.liu
**   函数作用：获取涂鸦设备在线数量(自定义)
**   参数说明:
***/
int tuya_online_num_get_customize()
{
	// if (tuya_api_online_check() == false)
	// {
	// 	return 0;
	// }
	return tuya_pview_video_count;
}
/***
**   日期:2022-06-08 08:07:57
**   作者: leo.liu
**   函数作用：tuya默认进入的监控
**   参数说明:
***/
static bool tuya_event_cmd_video_start(void)
{
	LOG_RED("############  tuya video pview start @1############### \n");
#if 0
	if (tuya_preview_busy_check() == true)
	{
		tuya_api_preview_quit();
		return true;
	} 
#endif
	if(tuya_quit_task != NULL)
	{
		return false;
	}
	tuya_pview_video_count++;
	if(tuya_pview_video_count > 1){

		return true;
	}
//	if (cur_layout_get() != pLAYOUT(tuya_monitor))
/****** 室内机在查看监控的时候，不跳转到“tuya_monitor页面” ******/
	intercom_data_busy_enbale(true);
	const layout* p_cur_layout = cur_layout_get();
	if (layout_monitor_talk_state() == false)//((p_cur_layout!= pLAYOUT(tuya_monitor))&&(p_cur_layout != pLAYOUT(monitor)))
	{
		if((monitor_enter_mask_get() == MON_ENTER_TALK) || (monitor_unlock_mask_get() == MON_UNLOCK_TALK))
		{
			h264_encode_sample_enable(true);
			return false;
		}
		else
		{
			h264_encode_sample_enable(false);
		}
		tuya_monitor_channel = monitor_channel_get();

		if (monitor_valid_channel_check(tuya_monitor_channel) == true)
		{
			monitor_channel_set(tuya_monitor_channel);
		}
		else if (monitor_valid_channel_check(MON_CH_DOOR1) == true)
		{
			monitor_channel_set(MON_CH_DOOR1);
		}
		else if (monitor_valid_channel_check(MON_CH_DOOR2) == true)
		{
			monitor_channel_set(MON_CH_DOOR2);
		}
		else if (monitor_valid_channel_check(MON_CH_CCTV1) == true)
		{
			monitor_channel_set(MON_CH_CCTV1);
		}
		else if (monitor_valid_channel_check(MON_CH_CCTV2) == true)
		{
			monitor_channel_set(MON_CH_CCTV2);
		}
		else
		{
			monitor_channel_set(MON_CH_DOOR1);
		}
		if((monitor_enter_mask_get() != MON_ENTER_CALL) && (h264_encode_sample_get() == false))
		{
			goto_layout(pLAYOUT(tuya_monitor));
		}
	}
	else
	{
		h264_encode_sample_enable(true);
		return false;
	}
	
	standby_timer_close();
	/*****  同步时间 *****/
	/*****  在layout_monitor页面，不需要再次打开监控 ******/
	if(p_cur_layout != pLAYOUT(monitor))
	{
		if(p_cur_layout == pLAYOUT(intercom_talk)){

			intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
		}
		else{

			intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
			intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR_CALL_ANSWER);
		}
		
		
		
		if(monitor_enter_mask_get() == MON_ENTER_NONE)
		{
			monitor_open(false, 0x03);
		}
		else
		{
			video_switch_enable(monitor_channel_get(), 0x03);
		}
	}
	else
	{
		if(monitor_enter_mask_get() != MON_ENTER_CALL){

			intercom_cmd_send(CODE_ALL_ID, CMD_DATA_BUSY);
			intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR_CALL_ANSWER);
		}
		tuya_monitor_channel = monitor_channel_get();
	}

	tuya_api_app_sync_utc_time();
	/*****  开启视频并编码 *****/
	h264_encode_capture_enable(true);
	h264_encode_open(0x02);
	h264_encode_skip_frame(5);  

	tuya_spk_sw_pin_enable(true);                                           
	tuya_mic_sw_pin_enable(false);
	voice_record_pin_enable(false);

	// ring_to_outdoor_mute_pin_ctrl(true);

	
	
	audio_input_open(0x02);
	audio_input_capture_enable(true);

	MON_CH ch = monitor_channel_get();
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
	if((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
	{
		door_audio_talk(AUDIO_CH_CLOSE);
	}
	is_tuya_app_preview = true;
	printf("############  tuya video pview start @2############### \n");
	return true; 
}

/***
**   日期:2022-06-02 08:51:46
**   作者: leo.liu
**   函数作用：定时器任务
**   参数说明:
***/
static void tuya_timer_quit_task_func(lv_task_t *task_t)
{
	// printf("abd--------------[%s:%d]\n",__func__,__LINE__);
	if(tuya_quit_task != NULL){
		
		lv_task_del(tuya_quit_task);
		tuya_quit_task = NULL;
	}
	// printf("abd--------------[%s:%d]\n",__func__,__LINE__);
}
extern lv_task_t *lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void *user_data);
/***
**   日期:2022-06-07 16:21:27
**   作者: leo.liu
**   函数作用：app退出查看监控
**   参数说明:
***/
bool tuya_event_cmd_video_stop(void)
{
	if((tuya_pview_video_count == 0) && (tuya_quit_task == NULL))
	{
		tuya_quit_task = lv_task_create(tuya_timer_quit_task_func, 1000, LV_TASK_PRIO_MID, NULL);
	}
	else if((tuya_pview_video_count !=0) && (tuya_quit_task != NULL))
	{
		lv_task_del(tuya_quit_task);
		tuya_quit_task = NULL;
	}
	printf("tuya_event_cmd_video_stop\n");
	// intercom_data_busy_enbale(true);
	//if (is_tuya_app_preview == false)
	//{
		//return false; 
	//}
	tuya_pview_video_count--;
	if(tuya_pview_video_count > 0){

		return true;
	}

	if (tuya_outdoor_open_flag)
	{
		tuya_outdoor_open_flag = false;
		tuya_api_open_outdoor_success_report(false);
	}
	if (tuya_gate_open_flag)
	{
		tuya_gate_open_flag = false;
		tuya_api_open_door_success_report(false);
	}

	tuya_pview_video_count = tuya_pview_video_count<0?0:tuya_pview_video_count;

	is_tuya_app_preview = false;
	if(cur_layout_get() == pLAYOUT(tuya_monitor))
	{
		MON_ENTER_FLG flg = monitor_enter_mask_get();
		if(flg == MON_ENTER_NONE)
		{
			monitor_close();
			monitor_enter_mask_set(MON_ENTER_NONE);
			intercom_cmd_send(CODE_ALL_ID, CMD_DATA_RELEASE);
		}
		else if(flg != MON_ENTER_TALK)
		{
			door_audio_talk(AUDIO_CH_CLOSE);
			/* 问题：call机，主机返回，然后涂鸦推送再退出，分机回放没有声音。时间：20240928 */
			MON_CH ch = monitor_channel_get();
			intercom_audio_enable((ch == MON_CH_DOOR1) ? AUDIO_CH_DOOR1 : AUDIO_CH_DOOR2);
		}
		goto_layout(pLAYOUT(home));
	
		standby_timer_restart(true);
	
		audio_input_capture_enable(false);
		h264_encode_capture_enable(false);
		LOG_GREEN("set release 1\n");
		
	}
	// printf("abd--------------[%s:%d]\n",__func__,__LINE__);
	if(cur_layout_get() != pLAYOUT(monitor)){
		
		h264_encode_close(0x02);
		audio_input_close(0x02);
	}
	

	tuya_mic_sw_pin_enable(false);
#if 0
	tuya_spk_sw_pin_enable(false);

	


	if(!indoor_talk_state_get())
	{
		audio_to_outdoor2_pin_ctrl(false);
		audio_to_outdoor1_pin_ctrl(false);
	}
#endif

	//h264_encode_sample_enable(false);
	is_tuya_app_talk = false;
	// printf("abd--------------[%s:%d]\n",__func__,__LINE__);
	return true;
}

/***
**   日期:2022-06-07 16:35:49
**   作者: leo.liu
**   函数作用：app与设备通话处理
**   参数说明:
***/
static bool truye_event_cmd_audio_start(void)
{
	
	if (is_tuya_app_talk == true)
	{
		return false;
	}
	else if(h264_encode_sample_get() == true){

		return false;
	}
	is_tuya_app_talk = true;
	/*如果室内机通话，涂鸦接听则退出*/
	if(h264_encode_sample_get() == true)
	{
		return false;
	}
	/**** 室内机已经在通话状态，app按下通话键后没有任何作用 ****/
	if(indoor_talk_state_get() == true)
	{
		return false;
	}

	if(cur_layout_get() == pLAYOUT(monitor))
	{
		goto_layout(pLAYOUT(tuya_monitor));
		video_display_preview_enable(false);
		video_switch_enable(tuya_monitor_channel, 0x01);
		/****** 退出监控页面，一并将编码也关闭了，需要重新打开 ******/
		tuya_api_app_sync_utc_time();
		/*****  开启视频并编码 *****/
		h264_encode_capture_enable(true);
		h264_encode_open(0x02);
		h264_encode_skip_frame(5);
	}


	
	tuya_spk_sw_pin_enable(true);
	tuya_mic_sw_pin_enable(true);
	voice_record_pin_enable(false);
	ring_to_outdoor_mute_pin_ctrl(true);
	audio_input_open(0x02);
	audio_input_capture_enable(true);


	MON_CH ch = monitor_channel_get();
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
	if((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
	{
		door_audio_talk(AUDIO_CH_CLOSE);
	}
	monitor_enter_mask_set(MON_ENTER_NONE);
	intercom_cmd_send(CODE_ALL_ID, CMD_INDOOR_CALL_ANSWER);

	return true;
}
/***
**   日期:2022-06-18 10:36:39
**   作者: leo.liu
**   函数作用：tuya 预览
**   参数说明:
***/
bool tuya_api_app_preview_check(void)
{
	return is_tuya_app_preview;
}
/***
**   日期:2022-06-08 09:34:33
**   作者: leo.liu
**   函数作用：获取tuya app是否在通话
**   参数说明:
***/
bool tuya_api_app_talk_check(void)
{
	return is_tuya_app_talk;
}

/***
**   日期:2022-06-07 19:37:59
**   作者: leo.liu
**   函数作用：通道切换
**   参数说明:
***/
static bool tuya_event_cmd_ch_channge(int channel)
{
	if (monitor_valid_channel_check(channel) == false)
	{
		return false;
	}
	/*****  记录上次的通道 *****/
	tuya_monitor_channel = channel;
	if (monitor_channel_get() == channel)
	{	
		// printf("==============++++++++++++++======通道切换%d\n",monitor_channel_get());
		return layout_monitor_report_vaild_channel();
	}
	if (channel == MON_CH_DOOR1)
	{
		audio_to_outdoor2_pin_ctrl(false);
		audio_to_outdoor1_pin_ctrl(true);
	}
	else if (channel == MON_CH_DOOR2)
	{
		audio_to_outdoor2_pin_ctrl(true);
		audio_to_outdoor1_pin_ctrl(false);
	}
	if((channel == MON_CH_CCTV1) || (channel == MON_CH_CCTV2) || (channel == MON_CH_CCTV3) || (channel == MON_CH_CCTV4))
	{
		door_audio_talk(AUDIO_CH_CLOSE);
	}
	monitor_channel_set(channel);
	monitor_open(false, 0x01);
	h264_encode_skip_frame(5);
	return true;
}


/***
**   日期:2022-06-08 08:30:44
**   作者: leo.liu
**   函数作用：开启或关闭移动侦测
**   参数说明:
***/
static bool tuya_event_cmd_motion_enable(int arg)
{
	user_data_get()->motion.enable = (bool)arg;
	tuya_api_auto_monitor_report(user_data_get()->motion.enable);
	user_data_save();
	if((cur_layout_get() == pLAYOUT(close)) || (cur_layout_get() == pLAYOUT(frame_show)))
	{
		goto_layout(pLAYOUT(close));
	}
	return true;
}




/***
**   日期:2022-06-08 08:30:44
**   作者: leo.liu
**   函数作用：开启或关闭报警功能
**   参数说明:
***/
static bool tuya_event_cmd_alam_enable(int arg)
{
	// if((sercurity_sensor1_normal_check() == true)&&(sercurity_sensor2_normal_check() == true )){

	// 	user_data_get()->alarm.alarm_enable = (bool)arg;
	// 	tuya_api_alam_report(user_data_get()->alarm.alarm_enable);
	// 	if(arg == 0)
	// 	{
	// 		user_data_get()->alarm.alarm_1_enable = false;
	// 		user_data_get()->alarm.alarm_2_enable = false;
	// 	}
	// 	else
	// 	{
	// 		user_data_get()->alarm.alarm_1_enable = true;
	// 		user_data_get()->alarm.alarm_2_enable = true;
	// 	}
	// 	user_data_save();
	// }
	// else
	// {
	// 	tuya_api_alam_report(arg?0:1);
	// }
	if(arg)
	{
		tuya_api_alam_report(1);
		ringplay_play_form_index(8, 100, ringplay_doorcall_start_default_func, ringplay_tuya_doorcall_finish_default_func, false);
	}
	else
	{
		tuya_api_alam_report(0);
		if (ringplay_ing_check() == true)
		{
			ringplay_play_stop();
		}
	}
	
	return true;
}
/***
**   日期:2022-06-08 09:03:59
**   作者: leo.liu
**   函数作用：延时关闭锁
**   参数说明:
***/
static void tuya_door_outclose_delay_task(lv_task_t *task_t)
{
	//TODO 自动关锁
	monitor_unlcok_close();
	tuya_api_open_outdoor_success_report(false);
	tuya_outdoor_open_flag = false;
	lv_task_del(task_t);
}

static void tuya_door_close_delay_task(lv_task_t *task_t)
{
	//TODO 自动关锁
	monitor_unlcok_close();
	tuya_gate_open_flag = false;
	tuya_api_open_door_success_report(false);
	lv_task_del(task_t);
}

bool tuya_open_lock_flg = false;
/***
**   日期:2022-06-08 08:53:21
**   作者: leo.liu
**   函数作用：开锁处理
**   参数说明:
***/
static bool tuya_event_cmd_door_open(int arg)
{
	LOG_BLUE("arg:%d \n", arg);
	
	MON_CH ch = monitor_channel_get();
	if((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
	{
		tuya_api_open_outdoor_success_report(false);
		return false;
	}
	tuya_open_lock_flg = true;
	monitor_unlock_open(1, ch);
	tuya_outdoor_open_flag = true;
	tuya_talk_outdoor_unclock = true;
	tuya_api_open_outdoor_success_report(true);
	lv_layout_task_create(tuya_door_outclose_delay_task, user_data_get()->etc.door_1_open_time * 1000, LV_TASK_PRIO_MID, NULL);
	// if(tuya_api_app_talk_check() == false)
	{
		// ringplay_play_form_index(14, 100, gate_open_ringplay_start_default_func, gate_open_ringplay_finish_default_func, false);
		#ifdef MAOTOUYING_DAN_IC
		ringplay_play_form_index(7, 100,   door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
		#else
		ringplay_play_form_index(12, 100,   door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
		#endif
	}
	return true;
}

static bool tuya_event_cmd_door_open_2(int arg)
{
	LOG_BLUE("arg:%d \n", arg);
	MON_CH ch = monitor_channel_get();
	if((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2))
	{
		tuya_api_open_outdoor_success_report(false);
		return false;
	}
	tuya_open_lock_flg = true;
	monitor_unlock_open(2, ch);
	LOG_BLUE("open door %d \n", ch);
	tuya_gate_open_flag = true;
	tuya_api_open_door_success_report(true);
	lv_layout_task_create(tuya_door_close_delay_task, user_data_get()->etc.door_2_open_time * 1000, LV_TASK_PRIO_MID, NULL);
#ifdef MAOTOUYING_DAN_IC
	if(tuya_api_app_talk_check() == false)
	{
		ringplay_play_form_index(7, 100, door_open_ringplay_start_default_func, door_open_ringplay_finish_default_func, false);
	}
#endif	
	return true;
}




/*************************************************************************
 * @brief  app切换到居家模式
 * @date   2022-10-08 17:22
 * @author xiaoele
 **************************************************************************/
bool tuya_event_cmd_at_mode()
{
	LOG_BLUE("TUYA SWITCH TO AT MODE \n");
	user_data_get()->home_mode = 2;
	user_data_save();
	tuya_api_home_mode_report(user_data_get()->home_mode);

	if(cur_layout_get() == pLAYOUT(home))
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), home_mode_btn_id_get());
		if(obj == NULL)
		{
			LOG_RED("obj is NULL \n");
			return false;
		}
		home_mode_btn_display(obj);
	}

	return true;
}

/*************************************************************************
 * @brief  app切换到离家模式
 * @date   2022-10-08 17:23
 * @author xiaoele
 **************************************************************************/
bool tuya_event_cmd_leave_mode()
{
	LOG_BLUE("TUYA SWITCH TO LEAVE MODE \n");
	user_data_get()->home_mode = 0;
	user_data_save();
	tuya_api_home_mode_report(user_data_get()->home_mode);
	
	if(cur_layout_get() == pLAYOUT(home))
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), home_mode_btn_id_get());
		if(obj == NULL)
		{
			LOG_RED("obj is NULL \n");
			return false;
		}
		home_mode_btn_display(obj);
	}
	
	return true;
}


/*************************************************************************
 * @brief  app 切换到睡眠模式
 * @date   2022-10-08 17:22
 * @author xiaoele
 **************************************************************************/
bool tuya_event_cmd_sleep_mode()
{
	LOG_BLUE("TUYA SWITCH TO SLEEP MODE \n");
	user_data_get()->home_mode = 1;
	user_data_save();
	tuya_api_home_mode_report(user_data_get()->home_mode);

	if(cur_layout_get() == pLAYOUT(home))
	{
		lv_obj_t *obj = lv_obj_get_child_form_id(lv_scr_act(), home_mode_btn_id_get());
		if(obj == NULL)
		{
			LOG_RED("obj is NULL \n");
			return false;
		}
		home_mode_btn_display(obj);
	}

	return true;
}



/*************************************************************************
 * @brief  更新情景模式
 * @date   2022-10-08 17:22
 * @author xiaoele
 **************************************************************************/
static bool tuya_event_updata_mode(int arg)
{
	LOG_BLUE("TUYA UPDATA MODE \n");
	
	switch (user_data_get()->home_mode)
	{
	case 2:
		tuya_event_cmd_at_mode(true);
		break;
	case 0:
		tuya_event_cmd_leave_mode(true);
		break;
	case 1:
		tuya_event_cmd_sleep_mode(true);
		break;
	
	default:
		break;
	}

	return true;
}








/*************************************************************************
 * @brief  更新报警开关
 * @date   2022-10-08 17:22
 * @author xiaoele
 **************************************************************************/
static bool tuya_event_alam_updata_mode(int arg)
{
	tuya_event_cmd_alam_enable(user_data_get()->alarm.alarm_enable);
	return true;
}

/*************************************************************************
 * @brief  更新移动侦测开关
 * @date   2022-10-08 17:22
 * @author xiaoele
 **************************************************************************/
static bool tuya_event_auto_monitor_updata_mode(int arg)
{
	tuya_event_cmd_motion_enable(user_data_get()->motion.enable);
	return true;
}
/*************************************************************************
 * @brief  设备重启
 * @date   2022-10-08 17:22
 * @author xiaoele
 **************************************************************************/
static bool tuya_event_reboot(int arg)
{
	system("reboot");
	return true;
}
/*************************************************************************
 * @brief  格式化SD卡
 * @date   2022-10-08 17:22
 * @author xiaoele
 **************************************************************************/
// static bool tuya_event_sd_format(int arg)
// {
// 	printf("##### start fomrat sdcard ##### \n");
// 	system("rm -rf " SD_MEDIA_PATH);
// 	system("umount " SD_BASE_PATH);

// 	system("mkdosfs -F 32 -I /dev/mmcblk0 -n Panasonic ");
// 	system("sync");
// 	printf("##### finish fomrat sdcard ##### \n");
// 	return true;
// }

/***
**   日期:2022-06-07 15:41:21
**   作者: leo.liu
**   函数作用：涂鸦事件处理
**   参数说明:
***/
bool layout_tuya_event_default(TUYA_CMD cmd, int arg)
{
	switch ((cmd))
	{
		case TUYA_EVENT_CMD_VIDEO_START:
			tuya_event_cmd_video_start();
			break;
		case TUYA_EVENT_CMD_AUDIO_START:
			tuya_api_door2_unlock_mode_report(user_data_get()->etc.door2_lock_num);
			return truye_event_cmd_audio_start();
			break;
		case TUYA_EVENT_CMD_VIDEO_STOP:
			return tuya_event_cmd_video_stop();
			break;
		case TUYA_EVENT_CMD_ONLINE:
			return layout_monitor_report_vaild_channel();
			break;
		case TUYA_EVENT_CMD_CH_CHANGE:
			return tuya_event_cmd_ch_channge(arg);
			break;
		case TUYA_EVENT_CMD_MOTION_ENBALE:
			return tuya_event_cmd_motion_enable(arg);
			break;
		case TUYA_EVENT_CMD_DOOR_OPEN:
			printf("arg ======++++ %d",arg);
			return tuya_event_cmd_door_open(arg);
			break;
		case TUYA_EVENT_CMD_DOOR_OPEN_2:
			printf("LOCK2 ======++++ %d",arg);
			return tuya_event_cmd_door_open_2(arg);
			break;
		case TUYA_EVENT_CMD_UPDATE_MODE:
			return tuya_event_updata_mode(arg);
			break;
		// case TUYA_EVENT_CMD_AT_MODE:
		// 	return tuya_event_cmd_at_mode(arg);
		// 	break;
		// case TUYA_EVENT_CMD_LEAVE_MODE:
		// 	return tuya_event_cmd_leave_mode(arg);
		// 	break;
		// case TUYA_EVENT_CMD_SLEEP_MODE:
		// 	return tuya_event_cmd_sleep_mode(arg);
		// 	break;
		case TUYA_EVENT_CMD_ALAM_UPDATE_MONITOR:
			return tuya_event_alam_updata_mode(arg);
			break;
		case TUYA_EVENT_CMD_ALAM_SWITCH:
			return tuya_event_cmd_alam_enable(arg);
			break;
		case TUYA_EVENT_CMD_AUTO_MONITOR_UPDATE:
			return tuya_event_auto_monitor_updata_mode(arg);
			break;
		case TUYA_EVENT_CMD_REBOOT:
			return tuya_event_reboot(arg);
			break;
		// case TUYA_EVENT_CMD_SD_FORMAT:
		// 	return tuya_event_sd_format(arg);
		// 	break;
		default:
			break;
	}
	return true;
}
/***
**   日期:2022-06-07 18:35:56
**   作者: leo.liu
**   函数作用：上传有效通道
**   参数说明:
***/
bool layout_monitor_report_vaild_channel(void)
{
	return tuya_api_channel_report(monitor_channel_get(),
				       monitor_valid_channel_check(MON_CH_DOOR1), language_common_ch_string_get(MON_CH_DOOR1),
				       monitor_valid_channel_check(MON_CH_DOOR2), language_common_ch_string_get(MON_CH_DOOR2),
				       monitor_valid_channel_check(MON_CH_CCTV1), language_common_ch_string_get(MON_CH_CCTV1),
				       monitor_valid_channel_check(MON_CH_CCTV2), language_common_ch_string_get(MON_CH_CCTV2),
				       monitor_valid_channel_check(MON_CH_CCTV3), language_common_ch_string_get(MON_CH_CCTV3),
				       monitor_valid_channel_check(MON_CH_CCTV4), language_common_ch_string_get(MON_CH_CCTV4));

}


/***
**   日期:2022-06-07 18:35:56
**   作者: leo.liu
**   函数作用：消息弹窗
**   参数说明:
***/

static void lv_toast_obj_del(lv_anim_t * a)
{
	lv_obj_del(a->var);                 //删除控件
}


void message_window_pops(const char *str,int x,int y)
{
	// static rom_bin_info imgarr[] = {rom_bin_info_get(ROM_UI_SMALL_SUCCESS_PNG), rom_bin_info_get(ROM_UI_SMALL_WARNING_PNG), rom_bin_info_get(ROM_UI_SMALL_ERROR_PNG), rom_bin_info_get(ROM_UI_SMALL_NOTICE_PNG)};
	lv_obj_t *toast_cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_t *label = lv_label_create(toast_cont, NULL);
	// lv_obj_set_pos(toast_cont,307,88);
	// lv_obj_t *icon = lv_obj_create(toast_cont, NULL);

	// lv_obj_set_size(icon, 24, 24);

	lv_label_set_text_fmt(label, "%s", str);
	lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);

	// lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &imgarr[type]);

	lv_obj_set_style_local_pad_top(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 12);
	lv_obj_set_style_local_pad_left(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 24);
	lv_obj_set_style_local_pad_right(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 24);
	lv_obj_set_style_local_pad_bottom(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 12);

	lv_obj_set_style_local_radius(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 8);
	lv_obj_set_style_local_bg_opa(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_90);
	lv_obj_set_style_local_bg_color(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));

	lv_obj_set_style_local_text_color(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff0000));
	lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	
	lv_obj_set_auto_realign(toast_cont, true);
	// lv_obj_set_auto_realign(icon, true);
	lv_obj_set_auto_realign(label, true);

	lv_obj_align(toast_cont, NULL, LV_ALIGN_CENTER, x, y);
	// lv_obj_set_size(toast_cont, 322, 88);
	// lv_obj_align(icon, toast_cont, LV_ALIGN_IN_LEFT_MID, 24, 0);
	// lv_obj_align(label, icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
	
	lv_cont_set_fit(toast_cont, LV_FIT_TIGHT);

	/* 动画 自动删除 */
	lv_anim_t toast_anim;
	lv_anim_init(&toast_anim);
	lv_anim_set_time(&toast_anim, 300);
	lv_anim_set_delay(&toast_anim, 2000);
	lv_anim_set_var(&toast_anim, toast_cont);
	lv_anim_set_exec_cb(&toast_anim, (lv_anim_exec_xcb_t)lv_obj_set_height);
	lv_anim_set_values(&toast_anim, lv_obj_get_height(toast_cont), 0);
	lv_anim_set_ready_cb(&toast_anim, lv_toast_obj_del);
	lv_anim_start(&toast_anim);
}

void message_window_home_pops(const char *str)
{
	// static rom_bin_info imgarr[] = {rom_bin_info_get(ROM_UI_SMALL_SUCCESS_PNG), rom_bin_info_get(ROM_UI_SMALL_WARNING_PNG), rom_bin_info_get(ROM_UI_SMALL_ERROR_PNG), rom_bin_info_get(ROM_UI_SMALL_NOTICE_PNG)};
	lv_obj_t *toast_cont = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_t *label = lv_label_create(toast_cont, NULL);
	// lv_obj_set_pos(toast_cont,307,88);
	// lv_obj_t *icon = lv_obj_create(toast_cont, NULL);

	// lv_obj_set_size(icon, 24, 24);

	lv_label_set_text_fmt(label, "%s", str);
	lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);

	// lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &imgarr[type]);

	lv_obj_set_style_local_pad_top(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 12);
	lv_obj_set_style_local_pad_left(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 24);
	lv_obj_set_style_local_pad_right(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 24);
	lv_obj_set_style_local_pad_bottom(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 12);

	lv_obj_set_style_local_radius(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 8);
	lv_obj_set_style_local_bg_opa(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_90);
	lv_obj_set_style_local_bg_color(toast_cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xffffff));

	lv_obj_set_style_local_text_color(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xff0000));
	lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(20));
	
	lv_obj_set_auto_realign(toast_cont, true);
	// lv_obj_set_auto_realign(icon, true);
	lv_obj_set_auto_realign(label, true);

	lv_obj_align(toast_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 100);
	// lv_obj_set_size(toast_cont, 322, 88);
	// lv_obj_align(icon, toast_cont, LV_ALIGN_IN_LEFT_MID, 24, 0);
	// lv_obj_align(label, icon, LV_ALIGN_OUT_RIGHT_MID, 8, 0);
	
	lv_cont_set_fit(toast_cont, LV_FIT_TIGHT);

	/* 动画 自动删除 */
	lv_anim_t toast_anim;
	lv_anim_init(&toast_anim);
	lv_anim_set_time(&toast_anim, 300);
	lv_anim_set_delay(&toast_anim, 2000);
	lv_anim_set_var(&toast_anim, toast_cont);
	lv_anim_set_exec_cb(&toast_anim, (lv_anim_exec_xcb_t)lv_obj_set_height);
	lv_anim_set_values(&toast_anim, lv_obj_get_height(toast_cont), 0);
	lv_anim_set_ready_cb(&toast_anim, lv_toast_obj_del);
	lv_anim_start(&toast_anim);
}

//判断是否有室内机在与门口机通话(包括主机与分机)
bool indoor_talk_state_get(void)
{
	MON_ENTER_FLG flg = monitor_enter_mask_get();
	if(flg == MON_ENTER_TALK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*************************************************************************
 * @brief  在指定控件上上递归查找ID
 * @date   2022-11-16 10:16
 * @author xiaoele
 * @param  obj_id 查找的控件的ID
 **************************************************************************/
lv_obj_t *xl_obj_find_by_id_in_obj(lv_obj_t *obj, int obj_id)
{
	lv_obj_t *child = lv_obj_get_child(obj ? obj:lv_scr_act(), NULL); /* 在屏幕上查找子控件 */
	while (child)
	{
		if (child && child->obj_id == obj_id)
		{
			return child;
		}

		if (lv_obj_get_child(child, NULL))
		{

			lv_obj_t *child_child = xl_obj_find_by_id_in_obj(child, obj_id);

			/* 找到就返回, 没有找到就继续在下一个控件上查找 */
			if (child_child)
			{
				return child_child;
			}
		}

		/* 查找下一个 */
		child = lv_obj_get_child(obj == NULL ? lv_scr_act() : obj, child);
		usleep(1 * 1000);
	}

	return NULL;
}

/*************************************************************************
 * @brief  在屏幕上递归查找ID
 * @date   2022-11-16 10:16
 * @author xiaoele
 * @param  obj_id 查找的控件的ID
 **************************************************************************/
lv_obj_t *xl_obj_find_by_id(int obj_id)
{
	lv_obj_t *child = lv_obj_get_child(lv_scr_act(), NULL); /* 在屏幕上查找子控件 */
	while (child)
	{
		if (child->obj_id == obj_id)
		{
			return child;
		}

		/* 有子控件 在子控件上查找 */
		if (lv_obj_get_child(child, NULL))
		{
			lv_obj_t *child_child = xl_obj_find_by_id_in_obj(child, obj_id);

			/* 找到就返回, 没有找到就继续 */
			if (child_child)
			{
				return child_child;
			}
		}

		/* 查找下一个 */
		child = lv_obj_get_child(lv_scr_act(), child);
		usleep(1 * 1000);
	}

	// printf("layout[%d], obj id[%d] not found!\n", layout_index_get((layout *)cur_layout_get()), obj_id);
	
	return NULL;
}






