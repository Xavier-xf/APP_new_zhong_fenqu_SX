#ifndef _VIDEO_RECORD_H_
#define _VIDEO_RECORD_H_
#include <stdbool.h>
/***
** 日期: 2022-05-19 13:40
** 作者: leo.liu
** 函数作用：记录视频设备初始化
** 返回参数说明：
***/
bool video_record_init(void);
/***
** 日期: 2022-05-19 13:41
** 作者: leo.liu
** 函数作用：开启记录视频设备
** 返回参数说明：
***/
bool video_record_start(bool has_audio, void (*finish_callback)(const char *path));
/***
** 日期: 2022-05-19 13:42
** 作者: leo.liu
** 函数作用：停止记录
** 返回参数说明：
***/
bool video_record_stop(void);
/***
** 日期: 2022-05-19 17:13
** 作者: leo.liu
** 函数作用：获取记录状态
** 返回参数说明：
***/
bool video_record_status_get(void);
/***
**   日期:2022-05-28 16:31:22
**   作者: leo.liu
**   函数作用：获取录制时长
**   参数说明:
***/
bool video_record_duration_get(int *duration);
#endif