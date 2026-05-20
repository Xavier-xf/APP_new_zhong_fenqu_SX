#ifndef _AUDIO_INPUT_H_
#define _AUDIO_INPUT_H_
#include <stdbool.h>

/***
** 日期: 2022-05-19 11:32
** 作者: leo.liu
** 函数作用：音频采集设备
** 返回参数说明：
***/
bool audio_input_init(void);
/***
** 日期: 2022-05-19 11:33
** 作者: leo.liu
** 函数作用：音频打开采集
** 返回参数说明：
***/
bool audio_input_open(unsigned char mask);
/***
** 日期: 2022-05-19 11:34
** 作者: leo.liu
** 函数作用：音频关闭
** 返回参数说明：
***/
bool audio_input_close(unsigned char mask);
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：音频采集开关
**   参数说明:
***/
void audio_input_capture_enable(bool en);
/***
**   日期:2022-05-26 09:45:25
**   作者: leo.liu
**   函数作用：使能AEC
**   参数说明:
***/
void audio_input_aec_enable(bool en);
bool audio_input_state_get(void);
#endif
