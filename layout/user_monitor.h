#ifndef _USER_MONTIRO_H_
#define _USER_MONTIRO_H_
#include <stdbool.h>

typedef enum
{
	MON_CH_NONE = 0x00,
	MON_CH_DOOR1,
	MON_CH_DOOR2,
	MON_CH_CCTV1,
	MON_CH_CCTV2,
	MON_CH_CCTV3,
	MON_CH_CCTV4,
	MON_CH_INTERCOM,
	MON_CH_TOTAL
} MON_CH;

/***
** 日期: 2022-05-12 11:32
** 作者: leo.liu
** 函数作用：设置监控通道
** 返回参数说明：
***/
bool monitor_channel_set(MON_CH ch);
/***
** 日期: 2022-05-12 11:23
** 作者: leo.liu
** 函数作用：开启模拟视频流向
** 返回参数说明：
** flg:0x01:only ch,0x02:obly inter, 0x03:ch and inter
***/
void video_switch_enable(MON_CH ch, char flg);
/***
**   日期:2022-06-01 17:21:28
**   作者: leo.liu
**   函数作用：关闭模拟视频通道
**   参数说明:
***/
void video_switch_disable(void);
/***
** 日期: 2022-05-12 11:33
** 作者: leo.liu
** 函数作用：获取监控通道
** 返回参数说明：
***/
MON_CH monitor_channel_get(void);

/***
** 日期: 2022-05-12 11:34
** 作者: leo.liu
** 函数作用：开启监控
** 返回参数说明：
***/
bool monitor_open(bool preview, unsigned char mask);
/***
** 日期: 2022-05-12 11:36
** 作者: leo.liu
** 函数作用：关闭监控
** 返回参数说明：
***/
bool monitor_close(void);

/***
** 日期: 2022-05-16 16:25
** 作者: leo.liu
** 函数作用：判断此通道时候有信号
** 返回参数说明：
***/
bool monitor_valid_channel_check(MON_CH channel);

/***
** 日期: 2022-05-16 16:38
** 作者: leo.liu
** 函数作用：设置通道有效
** 返回参数说明：
***/
void monitor_valid_channel_set(MON_CH ch, bool sate);
/***
** 日期: 2022-05-12 15:17
** 作者: leo.liu
** 函数作用：监控状态
** 返回参数说明：
***/
bool is_monior_enable(void);
/***
**   日期:2022-06-11 11:30:03
**   作者: leo.liu
**   函数作用：设置监控位置
**   参数说明:
***/
bool monitor_preivew_pos_set(int x, int y, int w, int h);

extern bool rtsp_stream_open(char *url);
extern bool rtsp_stream_close(void);

void rtsp_decode_open(char *url);
void rtsp_decode_close(void);
void cctv_online_check_thread_init(void);
#endif
