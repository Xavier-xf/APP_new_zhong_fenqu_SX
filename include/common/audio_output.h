#ifndef _AUDIO_OUTPUT_H_
#define _AUDIO_OUTPUT_H_
#include <stdbool.h>
#include "ak_common_audio.h"

/***
** 日期: 2022-04-27 10:22
** 作者: leo.liu
** 函数作用：声卡设备初始化
** 返回参数说明：
***/
bool audio_output_init(void);
/***
** 日期: 2022-04-27 10:23
** 作者: leo.liu
** 函数作用：设置声卡播放参数,
** 返回参数说明：设备之前打开了，但是声卡参数一致返回true,否则返回false
***/
bool audio_output_open(enum ak_audio_channel_type ch, enum ak_audio_sample_rate rate);
/***
** 日期: 2022-04-27 10:24
** 作者: leo.liu
** 函数作用：关闭声卡设备,关闭声卡，必须同步执行（阻塞到声卡设备关闭为止，500ms超时处理）
** 返回参数说明：
***/
bool audio_output_close(void);
/***
** 日期: 2022-04-27 10:24
** 作者: leo.liu
** 函数作用：将音频数据送入声卡中
** 返回参数说明：
***/
bool audio_output_write(const unsigned char *data, int size);
/***
** 日期: 2022-04-27 10:25
** 作者: leo.liu
** 函数作用：设置声卡播放音量
** 返回参数说明：
***/
bool audido_output_volume_set(int volume);
/***
** 日期: 2022-04-27 10:26
** 作者: leo.liu
** 函数作用：获取声卡播放音量
** 返回参数说明：
***/
int audio_output_volume_get(void);
/***
** 日期: 2022-04-27 15:36
** 作者: leo.liu
** 函数作用：查询声卡中剩余的音频数据
** 返回参数说明：
***/
int audio_output_buffer_query(void);
/***
**   日期:2022-05-26 11:37:15
**   作者: leo.liu
**   函数作用：超时等待
**   参数说明:
***/
bool audio_output_buffer_try_timeout(int ms);

bool audio_output_device_restart(void);
#endif
