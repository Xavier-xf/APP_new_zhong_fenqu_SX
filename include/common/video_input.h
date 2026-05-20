#ifndef _VIDEO_INPUT_H_
#define _VIDEO_INPUT_H_
#include <stdbool.h>
#include "ak_common.h"
#include "ak_common_graphics.h"
#include "ak_tde.h"
#include <pthread.h>

#define SUB_VIDEO_PIXEL_WIDTH 1024//640
#define SUB_VIDEO_PIXEL_HIGHT 600//360

typedef struct
{
	char isp_conf_file[128];
	int main_width;
	int main_hgith;

	int sub_width;
	int sub_hight;
	int fps;
} video_isp_param;

extern pthread_mutex_t video_input_mutex;
extern pthread_mutex_t video_main_display_mutex;
extern pthread_mutex_t video_sub_display_mutex;
/***
** 日期: 2022-05-12 15:44
** 作者: leo.liu
** 函数作用：关闭isp
** 返回参数说明：
***/
bool video_input_close(void);
/***
** 日期: 2022-04-22 17:26
** 作者: leo.liu
** 函数作用：使能屏幕刷新
** 返回参数说明：
***/
void fb_refresh_enable(bool en);
/***
** 日期: 2022-05-12 15:21
** 作者: leo.liu
** 函数作用：打开isp
** 返回参数说明：
***/
bool video_input_open(video_isp_param *param);

/***
** 日期: 2022-05-12 16:24
** 作者: leo.liu
** 函数作用：初始化video onput 任务
** 返回参数说明：
***/
bool video_input_init(void);

/***
** 日期: 2022-05-13 17:48
** 作者: leo.liu
** 函数作用：获取常驻视频buffer
** 返回参数说明：
***/
unsigned char *video_input_resident_buffer_get(unsigned long long *timestamp);
/***
** 日期: 2022-05-13 17:48
** 作者: leo.liu
** 函数作用：获取常驻视频子buffer
** 返回参数说明：
***/
unsigned char *video_input_sub_resident_buffer_get(unsigned long long *timestamp);
/***
** 日期: 2022-05-14 16:16
** 作者: leo.liu
** 函数作用：设置过滤帧数
** 返回参数说明：
***/
void video_input_skip_frame_count_set(int count);

/***
** 日期: 2022-05-14 16:41
** 作者: leo.liu
** 函数作用：将数据写入视频buf
** 返回参数说明：
***/
void video_input_resident_buffer_write(unsigned char *data, int w, int h, int posx, int posy, int posw, int posh, AK_GP_FORMAT fmt);

/***
** 日期: 2022-05-14 17:34
** 作者: leo.liu
** 函数作用：清空buf
** 返回参数说明：
***/
void video_input_resident_bzero(void);
/***
**   日期:2022-06-11 11:36:39
**   作者: leo.liu
**   函数作用：设置将视频显示在指定的位置
**   参数说明:
***/
bool video_input_display_pos(int x, int y, int w, int h);
/***
** 日期: 2022-05-16 10:38
** 作者: leo.liu
** 函数作用：获取视频状态
** 返回参数说明：
***/
bool video_input_state_get(void);


void video_input_sub_buffer_busy_enable(bool en);

#define video_input_lock() pthread_mutex_lock(&video_input_mutex)
#define video_input_unlock() pthread_mutex_unlock(&video_input_mutex)

#define video_main_display_lock() pthread_mutex_lock(&video_main_display_mutex)
#define video_main_display_unlock() pthread_mutex_unlock(&video_main_display_mutex)

#define video_sub_display_lock() pthread_mutex_lock(&video_sub_display_mutex)
#define video_sub_display_unlock() pthread_mutex_unlock(&video_sub_display_mutex)
#endif