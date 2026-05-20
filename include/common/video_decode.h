#ifndef MPJEG_DECODE_H_
#define MPJEG_DECODE_H_
#include "ak_common.h"
#include "ak_vdec.h"
#include "ak_mem.h"
#include <stdbool.h>

typedef enum
{
    RTSP_CLIENT_NONE = 0,
    RTSP_CLIENT_FAIL,
    RTSP_CLIENT_SUCCESS,

}RTSP_CLIENT_STATE_T;

RTSP_CLIENT_STATE_T RTSP_CLIENT_STATE;      /* cctv通道打开的状态 */

typedef void (*decode_finish_callback)(struct ak_vdec_frame *);
typedef struct
{
	unsigned short pos_left;
	unsigned short pos_top;
	unsigned short pos_width;
	unsigned short pos_height;
} refresh_area_t;

#define tde_layer_pos_init(layer, px, py, pw, ph) \
	layer.pos_left = px;                      \
	layer.pos_top = py;                       \
	layer.pos_width = pw;                     \
	layer.pos_height = ph;

#define tde_layer_layer_init(layer, fmt, w, h, px, py, pw, ph) \
	layer.format_param = fmt;                              \
	layer.width = w;                                       \
	layer.height = h;                                      \
	tde_layer_pos_init(layer, px, py, pw, ph);

/***
** 日期: 2022-05-10 10:55
** 作者: leo.liu
** 函数作用：jpg解码器初始化
** 返回参数说明：
***/
bool jpg_decode_init(void);

/***
** 日期: 2022-05-10 12:02
** 作者: leo.liu
** 函数作用：打开jpg解码器
** 返回参数说明：
***/
bool jpg_decode_open(void (*read_frame)(struct ak_vdec_frame *frame));
/***
** 日期: 2022-05-10 13:33
** 作者: leo.liu
** 函数作用：
** 返回参数说明：
***/
bool jpg_decode_close(void);
/***
** 日期: 2022-05-10 13:37
** 作者: leo.liu
** 函数作用：写入解码流
** 返回参数说明：
***/
bool jpg_decode_stream_write(const unsigned char *data, int size);
/***
** 日期: 2022-05-10 13:41
** 作者: leo.liu
** 函数作用：判断解码线程是否彻底关闭
** 返回参数说明：
***/
bool jpg_decode_device_state(void);
/***
** 日期: 2022-05-10 10:55
** 作者: leo.liu
** 函数作用：h264解码器初始化
** 返回参数说明：
***/
bool h264_decode_init(void);
/***
** 日期: 2022-05-10 12:02
** 作者: leo.liu
** 函数作用：打开h264解码器
** 返回参数说明：
***/
bool h264_decode_open(void (*read_frame)(struct ak_vdec_frame *frame));
/***
** 日期: 2022-05-10 13:33
** 作者: leo.liu
** 函数作用：
** 返回参数说明：
***/
bool h264_decode_close(void);
/***
** 日期: 2022-05-10 13:37
** 作者: leo.liu
** 函数作用：写入解码流
** 返回参数说明：
***/
bool h264_decode_stream_write(const unsigned char *data, int size);
/***
** 日期: 2022-05-10 13:41
** 作者: leo.liu
** 函数作用：判断解码线程是否彻底关闭
** 返回参数说明：
***/
bool h264_decode_device_state(void);
/***
**   日期:2022-05-25 18:21:21
**   作者: leo.liu
**   函数作用：获取带解码帧
**   参数说明:
***/
void h264_decode_buffer_clear(void);
/***
** 日期: 2022-04-26 15:03
** 作者: leo.liu
** 函数作用：设置UI刷新进入视频模式
** 返回参数说明：
***/
void lv_video_mode_enable(bool en);

/***
** 日期: 2022-04-26 15:06
** 作者: leo.liu
** 函数作用：获取是否进入视频状态模式
** 返回参数说明：
***/
bool lv_video_mode_enable_get(void);
/***
**   日期:2022-05-28 16:15:00
**   作者: leo.liu
**   函数作用：使能视频显示到lcd
**   参数说明:
***/
void video_display_preview_enable(bool en);
/***
**   日期:2022-06-11 10:10:00
**   作者: leo.liu
**   函数作用：获取视频显示到lcd上面的状态
**   参数说明:
***/
bool video_display_preview_enable_check(void);
/***
** 日期: 2022-05-12 08:13
** 作者: leo.liu
** 函数作用：设置gui显示区域
** 返回参数说明：
***/
void gui_refresh_area(refresh_area_t *area_t, int count);

/***
** 日期: 2022-05-13 10:14
** 作者: leo.liu
** 函数作用：强制刷新
** 返回参数说明：
***/
void screen_force_refresh(void);

/***
** 日期: 2022-05-14 17:42
** 作者: leo.liu
** 函数作用：使能刷新gui
** 返回参数说明：
***/
void gui_refresh_enable(bool en);

/***
** 日期: 2022-05-14 17:55
** 作者: leo.liu
** 函数作用：获取GUI的地址
** 返回参数说明：
***/
unsigned char *gui_full_varaddr_get(void);

/***
** 日期: 2022-05-14 11:04
** 作者: leo.liu
** 函数作用：alpha混合gui局部位置
** 返回参数说明：
***/
#include "lvgl/lv_img_decoder.h"
void gui_alpha_mix_pos(const lv_img_decoder_dsc_t *dsc, int pos_x, int pos_y);

/***
** 日期: 2022-04-25 17:22
** 作者: leo.liu
** 函数作用：将gui layer 填充黑色
** 返回参数说明：
***/
void fb_gui_layer_rect_fill(unsigned int color, int x, int y, int w, int h);
#endif
