#ifndef _MJPEG_ENCODE_H_
#define _MJPEG_ENCODE_H_
#include <stdbool.h>
#include "ak_common.h"
#include "ak_venc.h"

/***
** 日期: 2022-05-18 08:36
** 作者: leo.liu
** 函数作用：设备初始化
** 返回参数说明：
***/
bool mjpeg_encode_init(void);
/***
** 日期: 2022-05-18 08:36
** 作者: leo.liu
** 函数作用：开启设备
** 返回参数说明：
***/
bool mjpeg_encode_open(void (*callback)(struct video_stream *farme));
/***
** 日期: 2022-05-18 08:38
** 作者: leo.liu
** 函数作用：关闭编码
** 返回参数说明：
***/
bool mjpeg_encode_close(void);
/***
** 日期: 2022-05-19 09:50
** 作者: leo.liu
** 函数作用：编码状态
** 返回参数说明：
***/
bool mjpeg_encode_status_get(void);
/***
** 日期: 2022-05-19 11:32
** 作者: leo.liu
** 函数作用：音频采集设备
** 返回参数说明：
***/
bool h264_encode_init(void);
/***
** 日期: 2022-05-19 11:33
** 作者: leo.liu
** 函数作用：音频打开采集
** 返回参数说明：
***/
bool h264_encode_open(unsigned char mask);
/***
** 日期: 2022-05-19 11:34
** 作者: leo.liu
** 函数作用：音频关闭
** 返回参数说明：
***/
bool h264_encode_close(unsigned char mask);
/***
**   日期:2022-06-08 08:17:53
**   作者: leo.liu
**   函数作用：设置过滤前面的n帧
**   参数说明:
***/
bool h264_encode_skip_frame(int n);
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：编码采集开关
**   参数说明:
***/
void h264_encode_capture_enable(bool en);
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：采集樣品数据流
**   参数说明:
***/
void h264_encode_sample_enable(bool en);
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：强制关闭解码器
**   参数说明:
***/
void h264_encode_fouce_close(void);
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：强制打开解码器
**   参数说明:
***/
void h264_encode_fouce_open(void);
bool h264_encode_sample_get();
#endif