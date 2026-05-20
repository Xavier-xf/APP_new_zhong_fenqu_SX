
#include "lv_drv_conf.h"
#include "drive/lv_port_disp.h"
#include "lv_color.h"

#include <stdbool.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include "unistd.h"

#include <semaphore.h>
#include <pthread.h>

#include "ak_mem.h"
#include "ak_common_graphics.h"
#include "ak_common.h"
#include "ak_tde.h"
#include "video_decode.h"
#include "user_common.h"
#include "video_input.h"
#include "user_common.h"

#include "rom.h"
/***
**	日期：2022-04-22 17:22
**	作者：leo.liu
**	说明：由于anyka37d不支持alpha通道透明，故将fb设置为rgb565，并且在视频叠加UI
**	的时候，将0x00的颜色扣除
***/
#define FB_PATH "/dev/fb0"

extern void *osal_fb_mmap_viraddr(int fb_len, int fb_fd);

static int fb_handle_id = -1;
static struct fb_var_screeninfo var_info;
static struct fb_fix_screeninfo fix_info;

/***
** 日期: 2022-04-25 14:19
** 作者: leo.liu
** 函数作用：fb layer信息
** 返回参数说明：上电第一次被初始化后，不能修改此值
***/
static struct ak_tde_layer fb_full_layer;

/***
** 日期: 2022-04-25 14:57
** 作者: leo.liu
** 函数作用：gui layer信息
** 返回参数说明：上电第一次被初始化后，不能修改此值
***/
static struct ak_tde_layer gui_full_layer;
static unsigned char *gui_full_varaddr = NULL;
static bool is_gui_refresh_enable = false;
/***
** 日期: 2022-04-26 14:59
** 作者: leo.liu
** 函数作用：进入视频模式使能
** 返回参数说明：
***/
static bool is_lv_fb_video_mode_enable = false;
/***
**   日期:2022-05-28 16:13:10
**   作者: leo.liu
**   函数作用：禁止视频显示
**   参数说明:
***/
static bool lv_fb_video_display_preview = false;
/***
** 日期: 2022-05-11 08:57
** 作者: leo.liu
** 函数作用：刷新信号量
** 返回参数说明：
***/
static sem_t lv_fb_sem_t;

/***
** 日期: 2022-04-22 17:26
** 作者: leo.liu
** 函数作用：lvgl 内部运算buffer
** 返回参数说明：
***/
static unsigned char *lv_gui_varaddr = NULL;
/***
** 日期: 2022-04-22 17:26
** 作者: leo.liu
** 函数作用：使能屏幕刷新
** 返回参数说明：
***/
static bool is_fb_refresh_enable = false;
void fb_refresh_enable(bool en)
{
	is_fb_refresh_enable = en;
}

static int gui_area_count = 0;

static refresh_area_t gui_area_group[32] = {{0}};

/***
** 日期: 2022-05-12 08:13
** 作者: leo.liu
** 函数作用：设置gui显示区域
** 返回参数说明：
***/
void gui_refresh_area(refresh_area_t *area, int count)
{
	refresh_area_t *area_t = (refresh_area_t *)area;
	if (count > 0)
	{
		memcpy(gui_area_group, area_t, count * sizeof(refresh_area_t));
	}
	gui_area_count = count;
}

/***
** 日期: 2022-04-26 15:03
** 作者: leo.liu
** 函数作用：设置UI刷新进入视频模式
** 返回参数说明：
***/
void lv_video_mode_enable(bool en)
{
	if (en == is_lv_fb_video_mode_enable)
	{
		return;
	}
	video_input_resident_bzero();
	is_lv_fb_video_mode_enable = en;
}

/***
** 日期: 2022-04-26 15:06
** 作者: leo.liu
** 函数作用：获取是否进入视频状态模式
** 返回参数说明：
***/
bool lv_video_mode_enable_get(void)
{
	return is_lv_fb_video_mode_enable;
}

/***
** 日期: 2022-05-11 14:25
** 作者: leo.liu
** 函数作用：将gui送到队列数据
** 返回参数说明：
***/
static inline void platform_gui_to_queue_layer(void)
{
	struct ak_tde_cmd opt;
	memcpy(&(opt.tde_layer_src), &gui_full_layer, sizeof(struct ak_tde_layer));
	memcpy(&(opt.tde_layer_dst), &gui_full_layer, sizeof(struct ak_tde_layer));
	opt.tde_layer_dst.format_param = GP_FORMAT_RGB888;
	ak_mem_dma_vaddr2paddr(video_input_resident_buffer_get(NULL), (unsigned long *)&opt.tde_layer_dst.phyaddr);

	opt.opt = GP_OPT_BLIT_TRANSPARENT_COLORKEY; // GP_OPT_BLIT_TRANSPARENT_COLORKEY;
	opt.alpha = 13;
	opt.colorkey_param.coloract = COLOR_KEEP; // COLOR_KEEP;
	opt.colorkey_param.color_min = 0x000008;  // 0x000008
	opt.colorkey_param.color_max = 0xFFFFFF;  // 0xFFFFFF

	for (int i = 0; i < gui_area_count; i++)
	{
		tde_layer_pos_init(opt.tde_layer_src, gui_area_group[i].pos_left, gui_area_group[i].pos_top, gui_area_group[i].pos_width, gui_area_group[i].pos_height);
		tde_layer_pos_init(opt.tde_layer_dst, gui_area_group[i].pos_left, gui_area_group[i].pos_top, gui_area_group[i].pos_width, gui_area_group[i].pos_height);
		ak_tde_opt(&opt);
	}
}

/***
** 日期: 2022-05-11 15:36
** 作者: leo.liu
** 函数作用：将队列数据送到fb
** 返回参数说明：
***/
static inline void platform_queue_to_fb_layer(void)
{
	struct ak_tde_cmd opt;
	memcpy(&(opt.tde_layer_src), &gui_full_layer, sizeof(struct ak_tde_layer));
	opt.tde_layer_src.format_param = GP_FORMAT_RGB888;
	ak_mem_dma_vaddr2paddr(video_input_resident_buffer_get(NULL), (unsigned long *)&opt.tde_layer_src.phyaddr);

	memcpy(&(opt.tde_layer_dst), &fb_full_layer, sizeof(struct ak_tde_layer));
	/***** 如果正显示的是第一个framebuffer ,将第二个地址赋值给opt *****/
	if (var_info.reserved[0] == 0)
	{
		opt.tde_layer_dst.phyaddr += var_info.xres * var_info.yres * 3; // fix_info.smem_len;
	}
	opt.opt = GP_OPT_BLIT;
	// opt.rotate_param = AK_GP_ROTATE_90;
	ak_tde_opt(&opt);
}

/***
** 日期: 2022-05-11 08:50
** 作者: leo.liu
** 函数作用：将gui to fb
** 返回参数说明：
***/
static inline void platform_gui_to_fb_layer(void)
{
	struct ak_tde_cmd opt;
	memcpy(&(opt.tde_layer_src), &gui_full_layer, sizeof(struct ak_tde_layer));
	memcpy(&(opt.tde_layer_dst), &fb_full_layer, sizeof(struct ak_tde_layer));
	/***** 如果正显示的是第一个framebuffer ,将第二个地址赋值给opt *****/
	if (var_info.reserved[0] == 0)
	{
		opt.tde_layer_dst.phyaddr += var_info.xres * var_info.yres * 3;
	}
	opt.opt = GP_OPT_BLIT;
	// opt.rotate_param = AK_GP_ROTATE_90;
	ak_tde_opt(&opt);
}

/***
** 日期: 2022-04-25 15:44
** 作者: leo.liu
** 函数作用：双buffer进行切换
** 返回参数说明：
***/
static inline void platform_frambuffer_swap(void)
{
	var_info.reserved[0] = var_info.reserved[0] ? 0 : 1;
	ioctl(fb_handle_id, FBIOPUT_VSCREENINFO, &var_info);
}

/***
** 日期: 2022-04-22 17:32
** 作者: leo.liu
** 函数作用：lvgl运算后填充到framebuffer的回调函数
** 返回参数说明：
***/
extern unsigned long long user_timestamp_get(void);

static void *platform_refresh_screen_task(void *arg)
{
	printf("***** refresh screen task create sccess ! *****\n");
	//	int average_frame_time = 0;
	//	unsigned long long timestamp = user_timestamp_get();
	//	int frame_count = 0;

	while (1)
	{
		sem_wait(&lv_fb_sem_t);
		if(is_fb_refresh_enable == false)
		{
			usleep(1 * 1000);
			continue;
		}
		// unsigned long long start = user_timestamp_get();
		if ((is_lv_fb_video_mode_enable == true) && (lv_fb_video_display_preview == true))
		{
			static unsigned long long pre_timestamp = 0;
			unsigned long long timestamp = 0;
			video_input_resident_buffer_get(&timestamp);
			if (timestamp == pre_timestamp)
			{
				usleep(1 * 1000);
				continue;
			}
			pre_timestamp = timestamp;
			video_main_display_lock();
			platform_gui_to_queue_layer();
			platform_queue_to_fb_layer();
			video_main_display_unlock();
		}
		else
		{
			platform_gui_to_fb_layer();
		}
		platform_frambuffer_swap();
		/*
				frame_count += 1;
				unsigned long long end3 = user_timestamp_get();
				if (abs(end3 - timestamp) > 3000)
				{
					average_frame_time = (end3 - timestamp) / frame_count;
					printf("refresh : %02dfps  %02llums \n", 1000 / average_frame_time, end3 - start);
					fflush(stdout);
					frame_count = 0;
					timestamp = end3;
				} */
		usleep(1 * 1000);
	}

	return NULL;
}

static void platform_display_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
	if (area->x2 < 0 || area->y2 < 0 || area->x1 > (int32_t)LV_HOR_RES_MAX || area->y1 > (int32_t)LV_VER_RES_MAX)
	{
		lv_disp_flush_ready(disp_drv);
		return;
	}
	video_main_display_lock();
	/***** 将图片tde biblt到gui layer上面 *****/
	struct ak_tde_layer src, dst;
	src.format_param = GP_FORMAT_RGB565;
	src.width = lv_area_get_width(area);
	src.height = lv_area_get_height(area);
	// printf("w%d h%d \n", src.width, src.height);
	if ((src.width < 18) || (src.height < 18))
	{
		unsigned char *src_addr = lv_gui_varaddr;
		unsigned char *dst = gui_full_varaddr + area->y1 * LV_HOR_RES_MAX * 2 + area->x1 * 2;
		for (int h = 0; h < src.height; h++)
		{
			memcpy(&dst[h * LV_HOR_RES_MAX * 2], &src_addr[h * src.width * 2], src.width * 2);
		}
	}
	else
	{
		src.pos_left = 0;
		src.pos_top = 0;
		src.pos_width = src.width;
		src.pos_height = src.height;
		ak_mem_dma_vaddr2paddr((void *)lv_gui_varaddr, (unsigned long *)&src.phyaddr);

		memcpy(&dst, &gui_full_layer, sizeof(struct ak_tde_layer));
		dst.pos_left = area->x1;
		dst.pos_top = area->y1;
		dst.pos_width = src.width;
		dst.pos_height = src.height;
		ak_tde_opt_blit(&src, &dst);
	}
	video_main_display_unlock();

	if ((lv_disp_flush_is_last(disp_drv) == true) && (is_gui_refresh_enable == true) && (is_lv_fb_video_mode_enable == false))
	{
		sem_post(&lv_fb_sem_t);
	}
	lv_disp_flush_ready(disp_drv);
}

/***
** 日期: 2022-04-25 15:13
** 作者: leo.liu
** 函数作用：fb layer 全屏页面信息
** 返回参数说明：
***/
static void fb_layer_full_layer_init(struct ak_tde_layer *layer)
{
	layer->format_param = GP_FORMAT_BGR888; // GP_FORMAT_RGB565;
	layer->width = var_info.xres;
	layer->height = var_info.yres;
	layer->pos_left = 0;
	layer->pos_top = 0;
	layer->pos_width = var_info.xres;
	layer->pos_height = var_info.yres;
	layer->phyaddr = fix_info.smem_start;
}
/***
** 日期: 2022-04-23 08:19
** 作者: leo.liu
** 函数作用：加载动画
** 获取其相关的参数
** 返回参数说明：
***/ 
// #define POWER_ON_ANIMATION_DEF
#ifdef POWER_ON_ANIMATION_DEF
static void power_on_animation_stream_send(int h264_decode_handle_id,unsigned char *data, int size)
{
	int dec_len = 0;
	int read_len = size;
	int send_len = 0;
	while (read_len > 0)
	{
		ak_vdec_send_stream(h264_decode_handle_id, &data[send_len], read_len, NONBLOCK, &dec_len);
		read_len -= dec_len;
		send_len += dec_len;
	}
}
static bool power_on_animation_loading(float fps)
{
	struct ak_tde_cmd opt;

	extern const unsigned char *get_rom_bin_base(void);
	unsigned char* p_stream_data = (unsigned char* )(get_rom_bin_base() + ROM_UI_POWER_ON_ANIMATION_H264);
	const int stream_size = ROM_UI_POWER_ON_ANIMATION_H264_SIZE;

	int h264_decode_handle_id = -1;
	struct ak_vdec_param param;
	memset(&param, 0, sizeof(struct ak_vdec_param));
	param.vdec_type = H264_ENC_TYPE;
	param.output_type = AK_YUV420SP;
	param.sc_width = 1024;
	param.sc_height = 600;
	param.stream_buf_size = 1024 * 1024;
	param.frame_buf_num = 3;
	ak_vdec_open(&param, &h264_decode_handle_id);
	ak_vdec_clear_buff(h264_decode_handle_id);

	int state = 0;

	power_on_animation_stream_send(h264_decode_handle_id,p_stream_data,stream_size);
	
	struct ak_vdec_frame frame;
	while(1)
	{
		
		memset(&frame,0,sizeof(struct ak_vdec_frame));
		ak_vdec_frame_decode(h264_decode_handle_id);
		if (ak_vdec_get_frame(h264_decode_handle_id, &frame) == 0)
		{
			
			tde_layer_layer_init(opt.tde_layer_src, GP_FORMAT_YUV420SP, frame.frame_obj.data.pitch_width, frame.frame_obj.data.pitch_height, 
			0, 0, frame.width, frame.height);
			ak_mem_dma_vaddr2paddr(frame.frame_obj.data.data, (unsigned long *)&opt.tde_layer_src.phyaddr);
		
			memcpy(&(opt.tde_layer_dst), &fb_full_layer, sizeof(struct ak_tde_layer));
			/***** 如果正显示的是第一个framebuffer ,将第二个地址赋值给opt *****/
			if (var_info.reserved[0] == 0)
			{
				opt.tde_layer_dst.phyaddr += var_info.xres * var_info.yres * 3;
			}
			opt.opt = GP_OPT_SCALE;
			// opt.rotate_param = AK_GP_ROTATE_90;
			ak_tde_opt(&opt);
			platform_frambuffer_swap();
			ak_vdec_release_frame(h264_decode_handle_id, &frame);
			usleep(1000*1000/fps);
			//printf("=============>>\n");
		}
		else if((ak_vdec_get_decode_finish(h264_decode_handle_id,&state)== 0)&&(state))
		{
			break;
		}
		else
		{
			ak_vdec_end_stream(h264_decode_handle_id);
		}
		usleep(1000);
	}
	ak_vdec_close(h264_decode_handle_id);

	return true;
}
#endif
/***
** 日期: 2022-04-23 08:19
** 作者: leo.liu
** 函数作用：将fb的格式设置为rgb565
** 获取其相关的参数
** 返回参数说明：
***/
extern bool LEO_FAST_ENTER_SYSTEM_FLAG;
static bool platform_fb_init(void)
{

	/***** 设置格式为RGB565 *****/
	if (fb_handle_id > 0)
	{
		printf("It's already been opened. \n");
		return true;
	}
	fb_handle_id = open(FB_PATH, O_RDWR);
	if (fb_handle_id < 0)
	{
		printf("open %s failed \n", FB_PATH);
		return false;
	}
	/***** 获取fb的相关的信息 *****/

	ioctl(fb_handle_id, FBIOGET_VSCREENINFO, &var_info);

	var_info.activate |= FB_ACTIVATE_FORCE;
	var_info.activate |= FB_ACTIVATE_NOW;
	var_info.xres = var_info.xres_virtual;
	var_info.yres = var_info.yres_virtual;
	/***** 设置RGB565格式 *****/
	var_info.bits_per_pixel = 24; // 16;
	var_info.red.offset = 16;     // 11;
	var_info.red.length = 8;      // 5;
	var_info.green.offset = 8;    // 5;
	var_info.green.length = 8;    // 6;
	var_info.blue.offset = 0;     // 0;
	var_info.blue.length = 8;     // 5;
	ioctl(fb_handle_id, FBIOPUT_VSCREENINFO, &var_info);

	/***** 获取不可变参数 *****/
	ioctl(fb_handle_id, FBIOGET_FSCREENINFO, &fix_info);

	/***** 初始化tde 需要的fb layer ******/
	unsigned char *fb_viraddr = (unsigned char *)osal_fb_mmap_viraddr(fix_info.smem_len, fb_handle_id);
	memset((void *)fb_viraddr, 0, fix_info.smem_len);

	
	fb_layer_full_layer_init(&fb_full_layer);
	var_info.reserved[0] = 1;
	platform_frambuffer_swap();
#ifdef POWER_ON_ANIMATION_DEF
#ifdef ROM_UI_POWER_ON_ANIMATION_H264
	if(LEO_FAST_ENTER_SYSTEM_FLAG == false){

		power_on_animation_loading(25.0);
	}
#endif
#else
	fb_refresh_enable(true);
#endif

	sem_init(&lv_fb_sem_t, 0, 0);
	pthread_t thread_t;
	pthread_create(&thread_t, user_pthread_atter_get(), platform_refresh_screen_task, NULL);
	return true;
}

/***
** 日期: 2022-04-25 15:04
** 作者: leo.liu
** 函数作用：gui layer 全屏页面信息
** 返回参数说明：
***/
static void gui_layer_full_layer_init(struct ak_tde_layer *layer, unsigned char **varaddr, enum ak_gp_format fmt)
{
	layer->format_param = fmt;
	layer->width = LV_HOR_RES_MAX;
	layer->height = LV_VER_RES_MAX;
	layer->pos_left = 0;
	layer->pos_top = 0;
	layer->pos_width = LV_HOR_RES_MAX;
	layer->pos_height = LV_VER_RES_MAX;
	unsigned char *data = (unsigned char *)ak_mem_dma_alloc(MODULE_ID_VO, LV_HOR_RES_MAX * LV_VER_RES_MAX * 2);
	ak_mem_dma_vaddr2paddr((void *)data, (unsigned long *)&layer->phyaddr);
	if (varaddr != NULL)
	{
		*varaddr = data;
	}
}

/***
** 日期: 2022-04-22 17:18
** 作者: leo.liu
** 函数作用：lvgl显示驱动初始化
** 返回参数说明：
***/
void lv_display_init(void)
{
	static lv_disp_buf_t draw_buf_dsc;

	/***** 分配内存给lvgl format rgb565*****/
	lv_gui_varaddr = (unsigned char *)ak_mem_dma_alloc(MODULE_ID_VO, LV_HOR_RES_MAX * LV_VER_RES_MAX * 2 /*  / 10 */);
	lv_disp_buf_init(&draw_buf_dsc, lv_gui_varaddr, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX /*  / 10 */);
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.hor_res = LV_HOR_RES_MAX;
	disp_drv.ver_res = LV_VER_RES_MAX;
	disp_drv.flush_cb = platform_display_flush;
	disp_drv.buffer = &draw_buf_dsc;
	lv_disp_drv_register(&disp_drv);

	/***** 软件gui layer 初始化 ******/
	gui_layer_full_layer_init(&gui_full_layer, &gui_full_varaddr, GP_FORMAT_RGB565);
	is_gui_refresh_enable = true;
	lv_fb_video_display_preview = false;
	/***** linux framebuffer 初始化 *****/
	platform_fb_init();

}

/***
** 日期: 2022-04-25 17:22
** 作者: leo.liu
** 函数作用：将gui layer 填充黑色
** 返回参数说明：
***/
void fb_gui_layer_rect_fill(unsigned int color, int x, int y, int w, int h)
{
	struct ak_tde_layer src;
	memcpy(&src, &gui_full_layer, sizeof(struct ak_tde_layer));
	src.pos_left = x;
	src.pos_top = y;
	src.pos_width = w;
	src.pos_height = h;
	video_main_display_lock();
	ak_tde_opt_fillrect(&src, color);
	video_main_display_unlock();
}

/***
** 日期: 2022-05-13 10:14
** 作者: leo.liu
** 函数作用：强制刷新
** 返回参数说明：
***/
void screen_force_refresh(void)
{
	sem_post(&lv_fb_sem_t);
}

/***
** 日期: 2022-05-14 17:42
** 作者: leo.liu
** 函数作用：使能刷新gui
** 返回参数说明：
***/
void gui_refresh_enable(bool en)
{
	is_gui_refresh_enable = en;
}

/***
** 日期: 2022-05-14 17:55
** 作者: leo.liu
** 函数作用：获取GUI的地址
** 返回参数说明：
***/
unsigned char *gui_full_varaddr_get(void)
{
	return gui_full_varaddr;
}

/***
**   日期:2022-05-28 16:15:00
**   作者: leo.liu
**   函数作用：使能视频显示到lcd
**   参数说明:
***/
void video_display_preview_enable(bool en)
{
	lv_fb_video_display_preview = en;
}
/***
** 日期: 2022-05-14 11:04
** 作者: leo.liu
** 函数作用：alpha混合gui局部位置
** 返回参数说明：
***/
void gui_alpha_mix_pos(const lv_img_decoder_dsc_t *dsc, int pos_x, int pos_y)
{
	/***
	** 日期: 2022-05-14 11:17
	** 作者: leo.liu
	** 函数作用：rgba565 [2]alpha [0:1]rgb565
	** 返回参数说明：
	***/
	const unsigned char *src = dsc->img_data;
	int img_w = dsc->header.w;
	int img_h = dsc->header.h;

	unsigned char *bg_data = (unsigned char *)(video_input_resident_buffer_get(NULL) + pos_y * LV_HOR_RES_MAX * 3 + pos_x * 3);

	lv_color_t *dst = (lv_color_t *)(gui_full_varaddr + pos_y * LV_HOR_RES_MAX * 2 + pos_x * 2);

	// video_main_display_lock();
	lv_color_t src_data;
	for (int y = 0; y < img_h; y++)
	{
		for (int x = 0; x < img_w; x++)
		{
			uint8_t alpha = src[x * 3 + 2];
			src_data.full = ((src[x * 3 + 1] << 8) | src[x * 3]);
			if (alpha == 0)
			{
			} // 0xc0
			else if ((alpha > 0xc0))
			{
				dst[x] = src_data;
			}
			else
			{
				lv_color_t bg = lv_color_make(bg_data[x * 3 + 0], bg_data[x * 3 + 1], bg_data[x * 3 + 2]);
				dst[x] = lv_color_mix(src_data, bg, alpha);
			}
		}
		bg_data += LV_HOR_RES_MAX * 3;
		dst += LV_HOR_RES_MAX;
		src += img_w * 3;
	}
	// video_main_display_unlock();
}
