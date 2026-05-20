#ifndef _VIDEO_PLAY_H_
#define _VIDEO_PLAY_H_
#include <stdbool.h>
typedef enum
{
	VIDEO_PLAY_STATE_IDLE,            //视频停止播放
	VIDEO_PLAY_STATE_PLAY,            //播放视频
	VIDEO_PLAY_STATE_PAUSE            //暂停视频
} VIDEO_PLAY_STATUS;

/***
**   日期:2022-05-24 14:27:18
**   作者: leo.liu
**   函数作用：视频播放初始化
**   参数说明:
***/
bool video_play_init(void);
/***
**   日期:2022-05-24 14:27:45
**   作者: leo.liu
**   函数作用：视频开始播放
**   参数说明:
***/
bool video_play_start(const char *file);
/***
**   日期:2022-05-24 14:28:35
**   作者: leo.liu
**   函数作用：视频停止播放
**   参数说明:
***/
bool video_play_stop(void);
/***
**   日期:2022-05-24 14:28:51
**   作者: leo.liu
**   函数作用：暂停播放
**   参数说明:
***/
bool video_play_pause(void);
/***
**   日期:2022-05-24 14:30:09
**   作者: leo.liu
**   函数作用：获取视频播放状态
**   参数说明:
***/
VIDEO_PLAY_STATUS video_play_status_get(void);
/***
**   日期:2022-05-24 14:32:04
**   作者: leo.liu
**   函数作用：获取播放时长
**   参数说明:
***/
bool video_play_duration_get(int *cur, int *total);
/***
**   日期:2022-06-11 11:41:27
**   作者: leo.liu
**   函数作用：播放显示位置设置
**   参数说明:
***/
bool video_play_display_pos(int x, int y, int w, int h);
#endif