#ifndef _USER_RECORD_H_
#define _USER_RECORD_H_
#include <stdbool.h>

typedef enum
{
	REC_MODE_NONE = 0X00,
	REC_MODE_MANUAL = 0x01,
	REC_MODE_AUTO = 0x02,
	REC_MODE_MOTION = 0x03,
	REC_MODE_ALWAYS = 0X04,
	REC_MODE_ALARM = 0x05,
	REC_MODE_TUYA_CALL = 0x10,
	REC_MODE_TUYA_ALARM = 0x20,
	REC_MODE_TUYA_MOTION = 0X40,

	REC_MODE_TOTAL = 0XFF
} REC_MODE;

/***
** 日期: 2022-05-19 10:37
** 作者: leo.liu
** 函数作用：抓拍一张照片
** 返回参数说明：
***/
bool record_jpeg_start(REC_MODE mode);
/***
** 日期: 2022-05-19 10:40
** 作者: leo.liu
** 函数作用：关闭抓拍
** 返回参数说明：
***/
bool record_jpeg_close(void);
/***
** 日期: 2022-05-19 16:45
** 作者: leo.liu
** 函数作用：开启录制视频
** 返回参数说明：
***/
bool record_video_start(REC_MODE mode);
/***
** 日期: 2022-05-19 17:18
** 作者: leo.liu
** 函数作用：停止录像
** 返回参数说明：
***/
bool record_video_close(void);
/***
** 日期: 2022-05-19 10:37
** 作者: leo.liu
** 返回参数说明：记录call的时间戳，用于挂断与call之间的时间间隔
***/
void tuya_doorbell_timestamp_set(unsigned long long value);
#endif
