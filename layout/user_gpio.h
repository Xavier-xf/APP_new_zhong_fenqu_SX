#ifndef _LAYOUT_GPIO_H_
#define _LAYOUT_GPIO_H_
#include "gpio_ctrl.h"
#include "pwm_ctrl.h"
#include "user_monitor.h"
typedef enum
{
	AUDIO_CH_DOOR1,
	AUDIO_CH_DOOR2,        
	AUDIO_CH_INTER,
	AUDIO_CH_CLOSE
} AUDIO_CH;

/***
** 日期: 2022-04-28 10:11
** 作者: leo.liu
** 函数作用：应用层对GPIO进行初始化
** 返回参数说明：
***/
void layout_gpio_init(void);
/***
** 日期: 2022-05-10 09:41
** 作者: leo.liu
** 函数作用：背光控制
** 返回参数说明：
***/
bool backlight_enable(bool en);
/***
** 日期: 2022-05-10 09:44
** 作者: leo.liu
** 函数作用：设置背光亮度
** 返回参数说明：per:亮度值的百分比
***/
bool backlight_brightness_set(int per);
/***
** 日期: 2022-04-28 10:08
** 作者: leo.liu
** 函数作用：功放控制IO
** 返回参数说明：
***/
bool power_amplifier_enable(bool en);
/***
** 日期: 2022-05-21 11:03
** 作者: leo.liu
** 函数作用：选择tuya/indoor spk
** 返回参数说明：
***/
void tuya_spk_sw_pin_enable(bool en);
/***
** 日期: 2022-05-21 11:03
** 作者: leo.liu
** 函数作用：选择tuya/indoor mic
** 返回参数说明：
***/
void tuya_mic_sw_pin_enable(bool en);
/***
** 日期: 2022-05-12 11:04
** 作者: leo.liu
** 函数作用： door1 电源
** 返回参数说明：
***/
void door1_power_enable(bool en);
/***
** 日期: 2022-05-12 11:04
** 作者: leo.liu
** 函数作用： door2 电源
** 返回参数说明：
***/
void door2_power_enable(bool en);
/***
** 日期: 2022-05-12 11:07
** 作者: leo.liu
** 函数作用：tp9950复位
** 返回参数说明：
***/
void tp9950_pin_reset(void);

void tp9950_power_off(void);

void tp9950_power_on(void);

bool tp9950_power_get(void);

/***
** 日期: 2022-05-21 08:52
** 作者: leo.liu
** 函数作用：door 1开锁处理
** 返回参数说明：
***/
// void monitor_unlock_open(MON_CH ch);
/***
** 日期: 2022-05-21 09:01
** 作者: leo.liu
** 函数作用：关锁
** 返回参数说明：
***/
void monitor_unlcok_close(void);
/***
** 日期: 2022-05-20 17:03
** 作者: leo.liu
** 函数作用：通话打开
** 返回参数说明：
***/
bool door_audio_talk(AUDIO_CH ch);
/***
**   日期:2022-06-01 10:34:59
**   作者: leo.liu
**   函数作用：开启intercom音频流
**   参数说明:
***/
bool intercom_audio_enable(AUDIO_CH ch);
/***
** 日期: 2022-05-21 09:07
** 作者: leo.liu
** 函数作用：铃声输出到户外机控制
** 返回参数说明：
***/
bool call_ring_to_outdoor_ctrl(AUDIO_CH ch, bool en);
/***
**   日期:2022-05-23 09:31:58
**   作者: leo.liu
**   函数作用：打开声音录制
**   参数说明:
***/
void monitor_record_pin_enable(bool en);
/***  
**   日期:2022-05-26 16:47:55
**   作者: leo.liu
**   函数作用：判断sensor1电平是否正常
**   参数说明:
***/
bool sercurity_sensor1_normal_check(void);
/***
**   日期:2022-05-26 16:47:55
**   作者: leo.liu
**   函数作用：判断sensor2电平是否正常
**   参数说明:
***/
bool sercurity_sensor2_normal_check(void);
/***
** 日期: 2022-05-20 14:11
** 作者: leo.liu
** 函数作用：音频传输到door1
** 返回参数说明：
***/
void audio_to_outdoor1_pin_ctrl(bool en);
/***
** 日期: 2022-05-20 14:13
** 作者: leo.liu
** 函数作用：音频传输到door2
** 返回参数说明：
***/
void audio_to_outdoor2_pin_ctrl(bool en);
/***
** 日期: 2022-05-20 14:25
** 作者: leo.liu
** 函数作用：通话音量控制
** 返回参数说明：
***/
bool talk_output_gpio_ctrl_volume(int vol);

/***
** 日期: 2022-04-28 10:19
** 作者: leo.liu
** 函数作用：铃声输出音量控制（0-3）
** 返回参数说明：
***/
bool ring_output_gpio_ctrl_volume(int vol);

void gate_unlock_pin_ctrl(bool en);

bool intercom_ringout_enable(bool en);
/***
** 日期: 2022-05-21 08:52
** 作者: leo.liu
** 函数作用：door 1开锁处理
** 返回参数说明：
***/
void monitor_unlock_open(int num, MON_CH ch);
bool intercom_ring_out_pin_enable(bool en);
void voice_record_pin_enable(bool en);
void il34118_enable_pin_ctrl(bool en);
/***
** 日期: 2022-05-20 14:05
** 作者: leo.liu
** 函数作用：禁止铃声输出到户外机
** 返回参数说明：
***/
void ring_to_outdoor_mute_pin_ctrl(bool en);


/***
**   日期:2023-06-13 09:39:26
**   作者: feian.liu
**   函数作用：单IC的IPS屏幕恢复出厂设置拉低
**   参数说明:
***/
void lcd_reset_pin_enable(bool en);

void intercom_talk_to_outdoor(void);
#endif