#ifndef _TP9950_H_
#define _TP9950_H_
#include "i2c_ctrl.h"

typedef enum
{
	TP2802_1080P25 = 0x03,
	TP2802_1080P30 = 0x02,
	TP2802_720P25 = 0x05,
	TP2802_720P30 = 0x04,
	TP2802_720P50 = 0x01,
	TP2802_720P60 = 0x00,
	TP2802_SD = 0x06,
	INVALID_FORMAT = 0x07,
	TP2802_720P25V2 = 0x0D,
	TP2802_720P30V2 = 0x0C,
	TP2802_PAL = 0x08,
	TP2802_NTSC = 0x09
} TP9950_FORMAT;

/***
** 日期: 2022-05-12 11:03
** 作者: leo.liu
** 函数作用：任务启动
** 返回参数说明：
***/
bool tp9950_init(void);

/***
** 日期: 2022-05-12 14:30
** 作者: leo.liu
** 函数作用：开启tp9950检测功能
** 返回参数说明：
***/
void tp9950_vin1_enable(bool en);
/***
**   日期:2022-05-23 09:17:09
**   作者: leo.liu
**   函数作用：标志重新检测
**   参数说明:
***/
void tp9950_restart_det(void);

/***
** 日期: 2022-05-12 14:54
** 作者: leo.liu
** 函数作用：根据数值写入寄存器
** 返回参数说明：
***/
void display_bright_adj(int bright, TP9950_FORMAT fmt);
/***
** 日期: 2022-05-17 09:06
** 作者: leo.liu
** 函数作用：对比度调整
** 返回参数说明：
***/
void display_const_adj(int cont, TP9950_FORMAT fmt);
/***
** 日期: 2022-05-17 09:06
** 作者: leo.liu
** 函数作用：色度调整
** 返回参数说明：
***/
void display_color_adj(int color, TP9950_FORMAT fmt);


void app_playback_record(bool en);


void tp9950_signale_detection_mode_setting(char mode);
#endif