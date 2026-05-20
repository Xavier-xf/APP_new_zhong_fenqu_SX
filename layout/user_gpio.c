#include "user_gpio.h"
#include <stdio.h>
#include "user_time.h"
#include <pthread.h>
#include "lv_msg_event.h"
#include "user_data.h"
#include "user_monitor.h"
#include "user_intercom.h"
#include "user_common.h"
#include "layout_common.h"
#include "user_file.h"
#include "info.h"

extern bool layout_monitor_talk_state(void);
extern bool video_record_status_get(void);

/***
** 日期: 2022-04-28 10:08
** 作者: leo.liu
** 函数作用：功放控制IO
** 返回参数说明：
***/
#define POWER_AMPLIFIER_PIN 75			
bool power_amplifier_enable(bool en)
{
	extern bool sleep_mode_enter_moniter_flag;
	if(sleep_mode_enter_moniter_flag == true)
	{
		return gpio_level_set(POWER_AMPLIFIER_PIN, GPIO_LEVEL_LOW);
	}

	// LOG_RED("AMP 75 ctrl %d\n", en);
	return gpio_level_set(POWER_AMPLIFIER_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
	
}

/***
** 日期: 2022-04-28 10:19
** 作者: leo.liu
** 函数作用：铃声输出音量控制（0-3）
** 返回参数说明：
***/
#define RING_OUTPUT_VOL_1_PIN 74
#define RING_OUTPUT_VOL_2_PIN 73
bool ring_output_gpio_ctrl_volume(int vol)
{
	switch (vol)
	{
	case 0:
		
		power_amplifier_enable(false);
	case 1:
		power_amplifier_enable(true);
		gpio_level_set(RING_OUTPUT_VOL_1_PIN, GPIO_LEVEL_HIGH);
		gpio_level_set(RING_OUTPUT_VOL_2_PIN, GPIO_LEVEL_HIGH);
		break;
	case 2:
		power_amplifier_enable(true);
		gpio_level_set(RING_OUTPUT_VOL_1_PIN, GPIO_LEVEL_LOW);
		gpio_level_set(RING_OUTPUT_VOL_2_PIN, GPIO_LEVEL_HIGH);
		break;
	case 3:
		power_amplifier_enable(true);
		gpio_level_set(RING_OUTPUT_VOL_1_PIN, GPIO_LEVEL_HIGH);
		gpio_level_set(RING_OUTPUT_VOL_2_PIN, GPIO_LEVEL_LOW);
		break;
	case 4:
		power_amplifier_enable(true);
		gpio_level_set(RING_OUTPUT_VOL_1_PIN, GPIO_LEVEL_LOW);
		gpio_level_set(RING_OUTPUT_VOL_2_PIN, GPIO_LEVEL_LOW);
		break;
	default:
		printf("ring output volume setting failed %d\n", vol);
		return false;
		break;
	}
	return true;
}

/***
** 日期: 2022-05-20 14:21
** 作者: leo.liu
** 函数作用：禁止itl34118 mic
** 返回参数说明：
***/
#define IL34118_MIC_MUTE_PIN 83
static void il34118_mic_mute_pin_ctrl(bool en)
{
	gpio_level_set(IL34118_MIC_MUTE_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}



/***
** 日期: 2022-05-20 14:20
** 作者: leo.liu
** 函数作用：34118控制
** 返回参数说明：
***/
#define IL34118_CD_PIN 84
void il34118_enable_pin_ctrl(bool en)
{
	gpio_level_set(IL34118_CD_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-20 14:25
** 作者: leo.liu
** 函数作用：通话音量控制
** 返回参数说明：
***/
#define TALK_OUTPUT_VOL_1_PIN 71
#define TALK_OUTPUT_VOL_2_PIN 72
bool talk_output_gpio_ctrl_volume(int vol)
{
	switch (vol)
	{
	case 0:
		il34118_enable_pin_ctrl(false);
		break;
	case 1:
	
		// il34118_enable_pin_ctrl(true);
		gpio_level_set(TALK_OUTPUT_VOL_1_PIN, GPIO_LEVEL_HIGH);
		gpio_level_set(TALK_OUTPUT_VOL_2_PIN, GPIO_LEVEL_HIGH);
		break;
	case 2:
	
		// il34118_enable_pin_ctrl(true);
		gpio_level_set(TALK_OUTPUT_VOL_1_PIN, GPIO_LEVEL_LOW);
		gpio_level_set(TALK_OUTPUT_VOL_2_PIN, GPIO_LEVEL_HIGH);
		break;
	case 3:
	
		// il34118_enable_pin_ctrl(true);
		gpio_level_set(TALK_OUTPUT_VOL_1_PIN, GPIO_LEVEL_HIGH);
		gpio_level_set(TALK_OUTPUT_VOL_2_PIN, GPIO_LEVEL_LOW);
		break;
	case 4:
	
		// il34118_enable_pin_ctrl(true);
		gpio_level_set(TALK_OUTPUT_VOL_1_PIN, GPIO_LEVEL_LOW);
		gpio_level_set(TALK_OUTPUT_VOL_2_PIN, GPIO_LEVEL_LOW);
		break;
	default:
		printf("talk output volume setting failed %d\n", vol);
		return false;
		break;
	}
	return true;
}

#define BL_PWM_NO 2
#define BL_PWM_CH 0
/***
** 日期: 2022-05-10 09:41
** 作者: leo.liu
** 函数作用：背光控制
** 返回参数说明：
***/
bool backlight_enable(bool en)
{
	return pwm_enable(BL_PWM_NO, BL_PWM_CH, en);
}

/***
** 日期: 2022-05-10 09:44
** 作者: leo.liu
** 函数作用：设置背光亮度
** 返回参数说明：
***/
bool backlight_brightness_set(int per)
{
	pwm_period_set(BL_PWM_NO, BL_PWM_CH, 65536);

	int duty = 5000 + per * (65536 - 5000) / 100;
	return pwm_duty_cycle_set(BL_PWM_NO, BL_PWM_CH, duty);
}


/***
**   日期:2022-07-11 11:58:28
**   作者: leo.liu
**   函数作用：
**   参数说明:
***/
#define INTER_RING_MUTE_PIN 38
bool intercom_ring_out_pin_enable(bool en)
{
	gpio_level_set(INTER_RING_MUTE_PIN, GPIO_LEVEL_LOW);
	return true;
}
/***
**   日期:2022-07-11 13:34:45
**   作者: leo.liu
**   函数作用：分机铃声输出
**   参数说明:
***/
// bool intercom_ringout_enable(bool en)
// {
// 	intercom_ring_out_pin_enable(en);
// 	return true;
// }

/***
** 日期: 2022-05-12 09:43
** 作者: leo.liu
** 函数作用：IO电平变化检测
** 返回参数说明：
***/
#define DOOR1_CALL_PIN 33
#define DOOR2_CALL_PIN 90
#define SENSOR1_DET_PIN 94
#define SENSOR2_DET_PIN 95

#define DOOR_CALL_DELAY 3000
#define DOOR_CALL_TALK_DELAY 1000

static void *gpio_det_task(void *arg)
{
	int gpio_group_pin[4] = {DOOR1_CALL_PIN, DOOR2_CALL_PIN, SENSOR1_DET_PIN, SENSOR2_DET_PIN};
	GPIO_LEVEL gpio_group_level[5] = {GPIO_LEVEL_LOW, GPIO_LEVEL_LOW, GPIO_LEVEL_LOW, GPIO_LEVEL_LOW,GPIO_LEVEL_LOW};
	for (int i = 0; i < 4; i++)
	{
		if (gpio_level_read(gpio_group_pin[i], &gpio_group_level[i]) == false)
		{
			printf("read gpio%d level failed \n", gpio_group_pin[i]);
		}
	}

	unsigned long long door1_call_ts = 0;
	unsigned long long door2_call_ts = 0;
	unsigned long long call_talk_ts = 0;


	unsigned long long door_call_ts;
	GPIO_LEVEL level;

	printf("***** gpio detection task create sccess ! *****\n");
	while (1)
	{
		/***** door1 检测 *****/
		door_call_ts = user_timestamp_get();
		if ((gpio_level_read(gpio_group_pin[0], &level) == true) && (level != gpio_group_level[0]))
		{
			usleep(20 * 1000);
			if ((gpio_level_read(gpio_group_pin[0], &level) == true) && (level != gpio_group_level[0]))
			{
				gpio_group_level[0] = level;
				if ((level == GPIO_LEVEL_LOW) && (abs(door_call_ts - door1_call_ts) > DOOR_CALL_DELAY))
				{
					door1_call_ts = user_timestamp_get();
					if (OwnID == 1)
					{
						if ((media_del_file_state() == false))
						{
							lv_msg_send_cmd(MSG_EVENT_CMD_DOOR1_CALL, 0, 0);
						}
						
					}
				}
			}
			
		}

		/***** door2 检测 *****/
		door_call_ts = user_timestamp_get();
		if ((gpio_level_read(gpio_group_pin[1], &level) == true) && (level != gpio_group_level[1]))
		{
			usleep(20 * 1000);
			if ((gpio_level_read(gpio_group_pin[1], &level) == true) && (level != gpio_group_level[1]))
			{
				gpio_group_level[1] = level;
				if ((level == GPIO_LEVEL_LOW) && (abs(door_call_ts - door2_call_ts) > DOOR_CALL_DELAY))
				{
					door2_call_ts = user_timestamp_get();
					if (OwnID == 1)
					{
						if ((media_del_file_state() == false))
						{
							lv_msg_send_cmd(MSG_EVENT_CMD_DOOR2_CALL, 0, 0);
						}
						
					}
				}
			}
			
		}

		if (user_data_get()->alarm.alarm_1_enable == true)
		{
			/***** sensor1 检测 *****/
			if ((alarm_1_gpio_level_read(gpio_group_pin[2]) == true)/* && (level != gpio_group_level[2]) */ )
			{
				usleep(20 * 1000);
				// if ((alarm_1_gpio_level_read(gpio_group_pin[2]) == true)/* && (level != gpio_group_level[2]) */ )
				{
					// gpio_group_level[2] = level;

					if (/* (flag == GPIO_LEVEL_HIGH) &&  */(user_data_get()->alarm.alarm_1_enable == true) && (user_data_get()->alarm.alarm_enable == true))
					{
						printf("goto alarm =========>>>>\n");
						if (OwnID == 1)
						{
							lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR1_TRIGGER, 0, 0);
						}
					}
				}
			}
		}
		if (user_data_get()->alarm.alarm_2_enable == true)
		{
			/***** sensor2 检测 *****/
			if ((alarm_2_gpio_level_read(gpio_group_pin[3]) == true) /* && (level != gpio_group_level[3]) */)
			{
				usleep(20 * 1000);
				// if ((alarm_2_gpio_level_read(gpio_group_pin[3]) == true) /* && (level != gpio_group_level[3]) */)
				{
					// gpio_group_level[3] = level;
					if (/* (flag == GPIO_LEVEL_HIGH) && */ (user_data_get()->alarm.alarm_2_enable == true) && (user_data_get()->alarm.alarm_enable == true))
					{
						if (OwnID == 1)
						{
							lv_msg_send_cmd(MSG_EVENT_CMD_SENSOR2_TRIGGER, 0, 0);
						}
					}
				}
			}
		}
		if(layout_monitor_talk_state() == false)
		{
			/***** 接听挂断按钮判断 *****/
			if ((adc_level_read(&level) == true))
			{
				// printf("+++++++++++++++++++++>>>>>>>>>>>>>[%d]\n",adc_value_get());
				if ((level == GPIO_LEVEL_LOW) && (abs(door_call_ts - call_talk_ts) > DOOR_CALL_TALK_DELAY) /*&& (adc_value_get() < 400)*/)
				{
					call_talk_ts = user_timestamp_get();
					lv_msg_send_cmd(MSG_EVENT_CMD_CALL_TALK_BTN, 0, 0);
				}
				// else if ((level == GPIO_LEVEL_LOW) && (abs(door_call_ts - call_talk_ts) > DOOR_CALL_TALK_DELAY) && (adc_value_get() > 400))
				// {
				// 	printf("================>>>>>>>>>>>>>unlock1\n");
				// 	call_talk_ts = user_timestamp_get();
				// 	lv_msg_send_cmd(MSG_EVENT_CMD_UNLOCK_BTN, 0, 0);
				// }
			}
		}
		if(layout_monitor_talk_state() == true)
		{
			if ((adc_level_read(&level) == true))
			{
				if ((level == GPIO_LEVEL_LOW) && (abs(door_call_ts - call_talk_ts) > DOOR_CALL_TALK_DELAY) /*&& (adc_value_get() < 400)*/)
				{
					call_talk_ts = user_timestamp_get();
					lv_msg_send_cmd(MSG_EVENT_CMD_CALL_TALK_BTN, 0, 0);
				}
				// else if ((level == GPIO_LEVEL_LOW) && (abs(door_call_ts - call_talk_ts) > DOOR_CALL_TALK_DELAY) && (adc_value_get() > 400))
				// {
				// 	printf("================>>>>>>>>>>>>>unlock2\n");
				// 	call_talk_ts = user_timestamp_get();
				// 	lv_msg_send_cmd(MSG_EVENT_CMD_UNLOCK_BTN, 0, 0);					
				// }
			}
		}
		usleep(10 * 1000);
	}
	return NULL;
}

/***
** 日期: 2022-05-12 11:04
** 作者: leo.liu
** 函数作用： door1 电源
** 返回参数说明：
***/
#define DOOR1_POWER_PIN 91
void door1_power_enable(bool en)
{
	if((user_data_get()->door1_always_enable)&&(en == false))
	{
		return;
	}
	gpio_level_set(DOOR1_POWER_PIN, en ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-12 11:04
** 作者: leo.liu
** 函数作用： door2 电源
** 返回参数说明：
***/
#define DOOR2_POWER_PIN 92
void door2_power_enable(bool en)
{
	if((user_data_get()->door2_always_enable)&&(en == false))
	{
		return;
	}
	gpio_level_set(DOOR2_POWER_PIN, en ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

#define CCTV1_POWER_PIN 34
/*************************************************************************
 * @brief  cctv1 电源控制
 * @date   2022-09-13 08:57
 * @author xiaoele
 **************************************************************************/
void cctv1_power_enable(bool en)
{
	gpio_level_set(CCTV1_POWER_PIN, en ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

#define CCTV2_POWER_PIN 40
/*************************************************************************
 * @brief  cctv2 电源控制
 * @date   2022-09-13 08:57
 * @author xiaoele
 **************************************************************************/
void cctv2_power_enable(bool en)
{
	gpio_level_set(CCTV2_POWER_PIN, en ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-12 11:07
** 作者: leo.liu
** 函数作用：tp9950复位
** 返回参数说明：
***/
#define TP_RESET_GPIO_PIN 98
void tp9950_pin_reset(void)
{
	gpio_level_set(TP_RESET_GPIO_PIN, GPIO_LEVEL_HIGH);
	usleep(1000 * 10);
	gpio_level_set(TP_RESET_GPIO_PIN, GPIO_LEVEL_LOW);
	usleep(1000 * 10);
	gpio_level_set(TP_RESET_GPIO_PIN, GPIO_LEVEL_HIGH);
	usleep(1000 * 10);
}
void tp9950_power_off(void)
{
	gpio_level_set(TP_RESET_GPIO_PIN, GPIO_LEVEL_LOW);
}
void tp9950_power_on(void)
{
	gpio_level_set(TP_RESET_GPIO_PIN, GPIO_LEVEL_HIGH);
}
bool tp9950_power_get(void)
{
	GPIO_LEVEL level;
	gpio_level_read(TP_RESET_GPIO_PIN, &level);
	return level == GPIO_LEVEL_LOW ? false : true;
}

/***
** 日期: 2022-05-20 14:05
** 作者: leo.liu
** 函数作用：禁止铃声输出到户外机
** 返回参数说明：
***/
#define RING_TO_OUTDOOR_MUTE_PIN 97
void ring_to_outdoor_mute_pin_ctrl(bool en)
{
	// printf("禁止输出到户外机\n");
	gpio_level_set(RING_TO_OUTDOOR_MUTE_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-20 14:11
** 作者: leo.liu
** 函数作用：音频传输到door1
** 返回参数说明：
***/
#define AUDIO_DOOR1_PIN 80
void audio_to_outdoor1_pin_ctrl(bool en)
{
	gpio_level_set(AUDIO_DOOR1_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}
/***
** 日期: 2022-05-20 14:13
** 作者: leo.liu
** 函数作用：音频传输到door2
** 返回参数说明：
***/
#define AUDIO_DOOR2_PIN 79
void audio_to_outdoor2_pin_ctrl(bool en)
{
	gpio_level_set(AUDIO_DOOR2_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-20 14:18
** 作者: leo.liu
** 函数作用：控制总线音频进入34118
** 返回参数说明：
***/
#define AUDIO_IN_PIN 78
static void audio_to_il34118_pin_ctrl(bool en)
{
	gpio_level_set(AUDIO_IN_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-20 17:09
** 作者: leo.liu
** 函数作用：控制总线音频输出到下一台设备
** 返回参数说明：
***/
#define AUDIO_OUT_PIN 77
void audio_to_inter_pin_ctrl(bool en)
{
	gpio_level_set(AUDIO_OUT_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}





/***
** 日期: 2022-05-20 14:27
** 作者: leo.liu
** 函数作用：通话增益控制
** 返回参数说明：通话的时候拉高，其余都拉低
***/
#define INTER_CONV_CNT_CTR_PIN 85
static void talk_agin_pin_ctrl(bool en)
{
	gpio_level_set(INTER_CONV_CNT_CTR_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-20 14:29
** 作者: leo.liu
** 函数作用：door1常规锁控制
** 返回参数说明：
***/ 
#define MEIOU_NEW_LOCK_1 87
#define MEIOU_OLD_LOCK_1 88

#define DOOR1_UNLOCK_PIN MEIOU_NEW_LOCK_1 //87
static void door1_unlock_pin_ctrl(bool en)
{
	gpio_level_set(DOOR1_UNLOCK_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-20 14:33
** 作者: leo.liu
** 函数作用：door2-1锁
** 返回参数说明:
***/
#define DOOR2_1_UNLOCK_PIN 13//88
static void door2_1_unlock_pin_ctrl(bool en)
{
	gpio_level_set(DOOR2_1_UNLOCK_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
}

/***
** 日期: 2022-05-20 14:33
** 作者: leo.liu
** 函数作用：door2锁
** 返回参数说明：
***/
// #define DOOR2_UNLOCK_PIN 88
// static void door2_unlock_pin_ctrl(bool en)
// {
// 	gpio_level_set(DOOR2_UNLOCK_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
// }

/***
** 日期: 2022-05-20 14:33
** 作者: leo.liu  
** 函数作用：door2-2锁
** 返回参数说明：
***/
#define MEIOU_NEW_LOCK_2 88
#define MEIOU_OLD_LOCK_2 87

#define DOOR2_2_UNLOCK_PIN MEIOU_NEW_LOCK_2//88///13
static void door2_2_unlock_pin_ctrl(bool en)
{
	gpio_level_set(DOOR2_2_UNLOCK_PIN, en == true ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}


// #define GATE_UNLOCK_PIN 13
// void gate_unlock_pin_ctrl(bool en)
// {
// 	gpio_level_set(GATE_UNLOCK_PIN, en == true ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW);
// }



/***
** 日期: 2022-05-21 11:03
** 作者: leo.liu
** 函数作用：选择tuya/indoor spk
** 返回参数说明：
***/
#define VOICE_SET_PIN 82
void tuya_spk_sw_pin_enable(bool en)
{
	gpio_level_set(VOICE_SET_PIN, en == false ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}
/***
** 日期: 2022-05-21 11:03
** 作者: leo.liu
** 函数作用：选择tuya/indoor mic
** 返回参数说明：
***/
#define MIC_SET_PIN 81
void tuya_mic_sw_pin_enable(bool en)
{
	gpio_level_set(MIC_SET_PIN, en == false ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

/***
**   日期:2022-05-23 09:39:26
**   作者: leo.liu
**   函数作用：记录声音
**   参数说明:
***/
#define RE_AUDIO_MUTEX_PIN 96
void voice_record_pin_enable(bool en)
{
	gpio_level_set(RE_AUDIO_MUTEX_PIN, en == true ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

/***
**   日期:2022-05-23 09:39:26
**   作者: leo.liu
**   函数作用：mic 电源
**   参数说明:
***/
#define MIC_POWER_EN_PIN 49
void mic_power_pin_enable(bool en)
{
	gpio_level_set(MIC_POWER_EN_PIN, en == true ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

/***
**   日期:2023-06-13 09:39:26
**   作者: feian.liu
**   函数作用：单IC的IPS屏幕恢复出厂设置拉低
**   参数说明:
***/
#define LCD_RESET_EN_PIN 35
void lcd_reset_pin_enable(bool en)
{
	gpio_level_set(LCD_RESET_EN_PIN, en == true ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

#define AUDIO1_PWM_GPIO 38
#define AUDIO2_PWM_GPIO 89

/***
** 日期: 2022-04-28 10:11
** 作者: leo.liu
** 函数作用：应用层对GPIO进行初始化
** 返回参数说明：
***/
void layout_gpio_init(void)
{
	
	// gpio_direction_set(GATE_UNLOCK_PIN, GPIO_DIR_OUT);
	// gpio_pull_enable(GATE_UNLOCK_PIN, true);
	// gpio_level_set(GATE_UNLOCK_PIN, GPIO_LEVEL_LOW);

	gpio_direction_set(AUDIO1_PWM_GPIO, GPIO_DIR_OUT);
	gpio_pull_enable(AUDIO1_PWM_GPIO, true);
	gpio_level_set(AUDIO1_PWM_GPIO, GPIO_LEVEL_LOW);

	gpio_direction_set(AUDIO2_PWM_GPIO, GPIO_DIR_OUT);
	gpio_pull_enable(AUDIO2_PWM_GPIO, true);
	gpio_level_set(AUDIO2_PWM_GPIO, GPIO_LEVEL_LOW);

	/***** 功放IO初始化 *****/
	gpio_direction_set(POWER_AMPLIFIER_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(POWER_AMPLIFIER_PIN, true);
	power_amplifier_enable(false);

	/***** 铃声音量控制 *****/
	gpio_direction_set(RING_OUTPUT_VOL_1_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(RING_OUTPUT_VOL_1_PIN, true);

	gpio_direction_set(RING_OUTPUT_VOL_2_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(RING_OUTPUT_VOL_2_PIN, true);
	ring_output_gpio_ctrl_volume(3);

	/***** 通话音量 *****/
	gpio_direction_set(TALK_OUTPUT_VOL_1_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(TALK_OUTPUT_VOL_1_PIN, true);

	gpio_direction_set(TALK_OUTPUT_VOL_2_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(TALK_OUTPUT_VOL_2_PIN, true);
	talk_output_gpio_ctrl_volume(0);

	/***** 初始化背光 *****/
	pwm_init(BL_PWM_NO, BL_PWM_CH);

	/***** 检测GPIO初始化 *****/
	gpio_direction_set(DOOR1_CALL_PIN, GPIO_DIR_IN);
	gpio_direction_set(DOOR2_CALL_PIN, GPIO_DIR_IN);	
	gpio_direction_set(SENSOR1_DET_PIN, GPIO_DIR_IN);
	gpio_direction_set(SENSOR2_DET_PIN, GPIO_DIR_IN);

	/***** 初始化户外机电源 *****/
	gpio_direction_set(DOOR1_POWER_PIN, GPIO_DIR_OUT);	
	gpio_pull_enable(DOOR1_POWER_PIN, true);
	gpio_direction_set(DOOR2_POWER_PIN, GPIO_DIR_OUT);	
	gpio_pull_enable(DOOR2_POWER_PIN, true);
	gpio_direction_set(CCTV1_POWER_PIN, GPIO_DIR_OUT);	
	gpio_pull_enable(CCTV1_POWER_PIN, true);
	gpio_direction_set(CCTV2_POWER_PIN, GPIO_DIR_OUT);	
	gpio_pull_enable(CCTV2_POWER_PIN, true);
	door1_power_enable(false);
	door2_power_enable(false);
	cctv1_power_enable(true);
	cctv2_power_enable(true);

	/***** 初始化tp9950复位 *****/
	gpio_direction_set(TP_RESET_GPIO_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(TP_RESET_GPIO_PIN, true);

	/***** 铃声输出到户外机 *****/
	gpio_direction_set(RING_TO_OUTDOOR_MUTE_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(RING_TO_OUTDOOR_MUTE_PIN, true);
	ring_to_outdoor_mute_pin_ctrl(true);

	/***** 声音传输到门口机1 *****/
	gpio_direction_set(AUDIO_DOOR1_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(AUDIO_DOOR1_PIN, true);
	audio_to_outdoor1_pin_ctrl(false);
	/***** 声音传输到门口机2 *****/
	gpio_direction_set(AUDIO_DOOR2_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(AUDIO_DOOR2_PIN, true);
	audio_to_outdoor2_pin_ctrl(false);

	/***** 使能34118 *****/
	gpio_direction_set(IL34118_CD_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(IL34118_CD_PIN, true);
	il34118_enable_pin_ctrl(false);

	/***** mic使能 *****/
	// gpio_direction_set(MIC_POWER_EN_PIN, GPIO_DIR_OUT);	
	// gpio_pull_enable(MIC_POWER_EN_PIN, true);
	// mic_power_pin_enable(false);

	/***** 禁止il34118 mic *****/
	gpio_direction_set(IL34118_MIC_MUTE_PIN, GPIO_DIR_OUT);	
	gpio_pull_enable(IL34118_MIC_MUTE_PIN, true);
	il34118_mic_mute_pin_ctrl(true);

	/***** 通话il34118增益控制 *****/
	gpio_direction_set(INTER_CONV_CNT_CTR_PIN, GPIO_DIR_OUT);
	gpio_pull_enable(INTER_CONV_CNT_CTR_PIN, true);
	talk_agin_pin_ctrl(false);

	/***** door1-unlock *****/
	gpio_direction_set(DOOR1_UNLOCK_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(DOOR1_UNLOCK_PIN, true);
	door1_unlock_pin_ctrl(false);


	gpio_direction_set(DOOR2_1_UNLOCK_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(DOOR2_1_UNLOCK_PIN, true);
	// door1_unlock_pin_ctrl(false);
	door2_1_unlock_pin_ctrl(false);

	/***** door2-unlock *****/
	gpio_direction_set(DOOR2_2_UNLOCK_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(DOOR2_2_UNLOCK_PIN, true);
	// door2_unlock_pin_ctrl(false);
	door2_2_unlock_pin_ctrl(true);

	/***** 音频输入pin *****/
	gpio_direction_set(AUDIO_IN_PIN, GPIO_DIR_OUT);			
	gpio_pull_enable(AUDIO_IN_PIN, true);
	audio_to_il34118_pin_ctrl(false);

	/***** 音频输出pin *****/
	gpio_direction_set(AUDIO_OUT_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(AUDIO_OUT_PIN, true);
	audio_to_inter_pin_ctrl(false);

	/***** tuya spk *****/
	gpio_direction_set(VOICE_SET_PIN, GPIO_DIR_OUT);			
	gpio_pull_enable(VOICE_SET_PIN, true);
	tuya_spk_sw_pin_enable(false);

	/***** tuya spk *****/
	gpio_direction_set(MIC_SET_PIN, GPIO_DIR_OUT);				
	gpio_pull_enable(MIC_SET_PIN, true);
	tuya_mic_sw_pin_enable(false);

	/*****  声音记录 *****/
	gpio_direction_set(RE_AUDIO_MUTEX_PIN, GPIO_DIR_OUT);		
	gpio_pull_enable(RE_AUDIO_MUTEX_PIN, true);
	voice_record_pin_enable(false);

	/* if (OWNID == 1) */
	{
		pthread_t thread_t;
		pthread_create(&thread_t, user_pthread_atter_get(), gpio_det_task, NULL);
	}
}

/***
** 日期: 2022-05-20 17:03
** 作者: leo.liu
** 函数作用：通话打开
** 返回参数说明：
***/
bool door_audio_talk(AUDIO_CH ch){
	switch (ch)
	{
	case AUDIO_CH_DOOR1:
		audio_to_outdoor1_pin_ctrl(true);
		audio_to_inter_pin_ctrl(true);
		audio_to_il34118_pin_ctrl(true);
		il34118_enable_pin_ctrl(true);
		il34118_mic_mute_pin_ctrl(false);

		// mic_power_pin_enable(true);

		power_amplifier_enable(true);
		talk_output_gpio_ctrl_volume(user_data_get()->audio.door_talk_volume);

		audio_to_outdoor2_pin_ctrl(false);
		talk_agin_pin_ctrl(false);
		break;
	case AUDIO_CH_DOOR2:
		audio_to_outdoor2_pin_ctrl(true);
		audio_to_inter_pin_ctrl(true);
		audio_to_il34118_pin_ctrl(true);
		il34118_enable_pin_ctrl(true);
		il34118_mic_mute_pin_ctrl(false);

		// mic_power_pin_enable(true);

		power_amplifier_enable(true);
		talk_output_gpio_ctrl_volume(user_data_get()->audio.door_talk_volume);

		audio_to_outdoor1_pin_ctrl(false);
		talk_agin_pin_ctrl(false);
		break;
	case AUDIO_CH_INTER:
		audio_to_inter_pin_ctrl(true);
		audio_to_il34118_pin_ctrl(true);
		il34118_enable_pin_ctrl(true);
		il34118_mic_mute_pin_ctrl(false);

		// mic_power_pin_enable(true);
		LOG_RED("AUDIO_CH_INTER AMP is open\n");
		power_amplifier_enable(true);
		talk_output_gpio_ctrl_volume(user_data_get()->audio.inter_talk_volume);

		audio_to_outdoor1_pin_ctrl(false);
		audio_to_outdoor2_pin_ctrl(false);
		talk_agin_pin_ctrl(true);
		break;
	case AUDIO_CH_CLOSE:
	default:		
		talk_output_gpio_ctrl_volume(0);
		
		power_amplifier_enable(false);

		il34118_mic_mute_pin_ctrl(true);
		il34118_enable_pin_ctrl(false);

		// mic_power_pin_enable(true);

		audio_to_il34118_pin_ctrl(false);
		audio_to_outdoor1_pin_ctrl(false);
		audio_to_outdoor2_pin_ctrl(false);
		audio_to_inter_pin_ctrl(false);
		talk_agin_pin_ctrl(true);
		break;
	}

	return true;
}

/***
** 日期: 2022-05-21 09:07
** 作者: leo.liu
** 函数作用：铃声输出到户外机控制
** 返回参数说明：
***/
bool call_ring_to_outdoor_ctrl(AUDIO_CH ch, bool en)
{
	// printf("%s============================>输出到户外机%d\n\n\n\n",__func__,ch);
	if (en == false)
	{
		ring_to_outdoor_mute_pin_ctrl(true);
		/* 分机通话开锁后，声音会变小？ */
		if(((layout_monitor_talk_state() == false) && (video_record_status_get() == false)))
		{
			audio_to_il34118_pin_ctrl(false);
		}
		if((monitor_unlock_mask_get() == MON_UNLOCK_TALK) && (layout_monitor_talk_state() == false))
		{
			audio_to_il34118_pin_ctrl(false);
		}
		
		return true;
	}
	power_amplifier_enable(true);
	ring_to_outdoor_mute_pin_ctrl(false);
	audio_to_il34118_pin_ctrl(true);
	switch (ch)
	{
	case AUDIO_CH_DOOR1:
		audio_to_outdoor1_pin_ctrl(true);
		audio_to_outdoor2_pin_ctrl(false);
		audio_to_inter_pin_ctrl(true);
		break;
	case AUDIO_CH_DOOR2:
		audio_to_outdoor1_pin_ctrl(false);
		audio_to_outdoor2_pin_ctrl(true);
		audio_to_inter_pin_ctrl(true);
		break;
	default:
		break;
	}
	return true;
}


/*************************************************************************
 * @brief  开锁处理
 * @date   2022-10-09 11:45
 * @author xiaoele
 **************************************************************************/
void monitor_unlock_open(int num, MON_CH ch)
{

	if (ch == MON_CH_DOOR1)
	{
		if (num == 1)			 /* 室内机锁 */
		{
			door2_1_unlock_pin_ctrl(true);
			if (OwnID != 1)
			{
				intercom_cmd_send(1, CMD_DOOR2_UNLOCK_OPEN);
			}
		}
		else					/* 门口机锁 */
		{
			door1_unlock_pin_ctrl(true);
			// door2_2_unlock_pin_ctrl(false);
			if (OwnID != 1)
			{
				intercom_cmd_send(1, CMD_DOOR2_UNLOCK_OPEN_2);
			}
		}
	}
	else if (ch == MON_CH_DOOR2)
	{
		if (num == 1)
		{
			door2_1_unlock_pin_ctrl(true);
			if (OwnID != 1)
			{
				intercom_cmd_send(1, CMD_DOOR2_UNLOCK_OPEN);
			}
		}
		else
		{

			door2_2_unlock_pin_ctrl(false);
			if (OwnID != 1)
			{
				intercom_cmd_send(1, CMD_DOOR2_UNLOCK_OPEN_2);
			}
		}
	}
	extern bool tuya_open_lock_flg;
	if((OwnID == 1) && (!tuya_open_lock_flg)){

		 //extern int tuya_ipc_door_bell_press_stop();
		//tuya_ipc_door_bell_press_stop();
		// extern int tuya_ipc_door_bell_press_cancel(void);
		// tuya_ipc_door_bell_press_cancel();
		tuya_api_doorbell_handup();

		extern bool h264_encode_sample_enable(bool);
		h264_encode_sample_enable(true);

		tuya_pvew_video_count_fource_set(0);

		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_STOP, 0);
	}
	tuya_open_lock_flg = false;
}

/***
** 日期: 2022-05-21 09:01
** 作者: leo.liu
** 函数作用：关锁
** 返回参数说明：
***/
void monitor_unlcok_close(void)
{
	door2_1_unlock_pin_ctrl(false);
	door1_unlock_pin_ctrl(false);
	// door2_unlock_pin_ctrl(false);
	door2_2_unlock_pin_ctrl(true);
	if (OwnID != 1)
	{
		intercom_cmd_send(1, CMD_DOOR_UNLOCK_CLOSE);
	}
}

/***
**   日期:2022-05-23 09:31:58
**   作者: leo.liu
**   函数作用：打开声音录制
**   参数说明:
***/
void monitor_record_pin_enable(bool en)
{
	voice_record_pin_enable(en);
	if (en == false)
	{
		return;
	}
	MON_CH ch = monitor_channel_get();
	if (ch == MON_CH_DOOR1)
	{
		audio_to_outdoor2_pin_ctrl(false);
		audio_to_outdoor1_pin_ctrl(true);
	}
	else if (ch == MON_CH_DOOR2)
	{
		audio_to_outdoor1_pin_ctrl(false);
		audio_to_outdoor2_pin_ctrl(true);
	}
	audio_to_inter_pin_ctrl(true);
	audio_to_il34118_pin_ctrl(true);
}

/***
**   日期:2022-05-26 16:47:55
**   作者: leo.liu
**   函数作用：判断sensor1电平是否正常
**   参数说明:
***/
bool sercurity_sensor1_normal_check(void)
{
	GPIO_LEVEL level;
	if (gpio_level_read(SENSOR1_DET_PIN, &level) == false)
	{
		return false;
	}
	return level == GPIO_LEVEL_LOW ? true : false;
}
/***
**   日期:2022-05-26 16:47:55
**   作者: leo.liu
**   函数作用：判断sensor2电平是否正常
**   参数说明:
***/
bool sercurity_sensor2_normal_check(void)
{
	GPIO_LEVEL level;
	if (gpio_level_read(SENSOR2_DET_PIN, &level) == false)
	{
		return false;
	}
	return level == GPIO_LEVEL_LOW ? true : false;
}
/***
**   日期:2022-06-01 10:34:59
**   作者: leo.liu
**   函数作用：开启intercom音频流
**   参数说明:
***/
bool intercom_audio_enable(AUDIO_CH ch)
{
	audio_to_inter_pin_ctrl(true);
	if (ch == AUDIO_CH_DOOR1)
	{
		audio_to_outdoor1_pin_ctrl(true);
		audio_to_outdoor2_pin_ctrl(false);
	}
	else if (ch == AUDIO_CH_DOOR2)
	{
		audio_to_outdoor2_pin_ctrl(true);
		audio_to_outdoor1_pin_ctrl(false);
	}
	return true;
}

void intercom_talk_to_outdoor(void)
{
	audio_to_il34118_pin_ctrl(false);
}


