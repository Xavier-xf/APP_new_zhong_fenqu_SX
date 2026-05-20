#include "video_decode.h"
#include "user_common.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "video_input.h"

/***** 解码后数据输出回调函数 *****/
static void (*jpg_decode_frame_read_callback)(struct ak_vdec_frame *) = NULL;

/*****  解码句柄 *****/
static int jpg_decode_handle_id = -1;

/***** 解码使能 *****/
static bool jpg_decode_enable = false;

static int jpg_decode_frame_count = 0;

static pthread_mutex_t jpg_decode_mutex;
/***
** 日期: 2022-05-10 11:23
** 作者: leo.liu
** 函数作用：打开解码器
** 返回参数说明：
***/
static void jpg_decode_device_open(void)
{
	struct ak_vdec_param param;
	memset(&param, 0, sizeof(struct ak_vdec_param));
	param.vdec_type = MJPEG_ENC_TYPE;
	param.output_type = AK_YUV420SP;
	param.sc_width = 1920;
	param.sc_height = 1080;
	param.stream_buf_size = 300 * 1024;
	param.frame_buf_num = 3;
	ak_vdec_open(&param, &jpg_decode_handle_id);
	ak_vdec_clear_buff(jpg_decode_handle_id);
	printf("jpeg decode open success ! \n");
}

/***
** 日期: 2022-05-10 11:23
** 作者: leo.liu
** 函数作用：关闭解码器
** 返回参数说明：
***/
static void jpg_decode_device_close(void)
{
	ak_vdec_close(jpg_decode_handle_id);
	jpg_decode_handle_id = -1;
	printf("jpeg decode close success ! \n");
}

/***
** 日期: 2022-05-10 10:59
** 作者: leo.liu
** 函数作用：接收数据队列函数
** 返回参数说明：
***/
static void *jpg_decode_task(void *arg)
{
	struct ak_vdec_frame frame = {0};
	int status = 0;
	printf("***** jpg stream task create sccess ! *****\n");
	while (1)
	{
		pthread_mutex_lock(&jpg_decode_mutex);
		if ((jpg_decode_enable == true) && (jpg_decode_handle_id == -1))
		{
			jpg_decode_device_open();
		}
		else if (jpg_decode_handle_id != -1)
		{
			memset(&frame, 0, sizeof(struct ak_vdec_frame));
			if (ak_vdec_get_frame(jpg_decode_handle_id, &frame) == 0)
			{
				if ((jpg_decode_enable == true) && (jpg_decode_frame_read_callback != NULL))
				{
					jpg_decode_frame_read_callback(&frame);
				}
				ak_vdec_release_frame(jpg_decode_handle_id, &frame);
				jpg_decode_frame_count--;
			}
			else if ((jpg_decode_enable == false))
			{
				ak_vdec_get_decode_finish(jpg_decode_handle_id, &status);
				if (status)
				{
					jpg_decode_device_close();
				}
			}
		}
		pthread_mutex_unlock(&jpg_decode_mutex);
		usleep(10 * 1000);
	}
	return NULL;
}

/***
** 日期: 2022-05-10 10:55
** 作者: leo.liu
** 函数作用：jpg解码器初始化
** 返回参数说明：
***/
bool jpg_decode_init(void)
{
	static bool inited = false;
	if (inited == true)
	{
		printf("jpg decode It's already initialized \n");
		return false;
	}
	inited = true;
	pthread_t pthread;
	pthread_mutex_init(&jpg_decode_mutex, NULL);
	pthread_create(&pthread, user_pthread_atter_get(), jpg_decode_task, NULL);
	return true;
}

/***
** 日期: 2022-05-10 12:02
** 作者: leo.liu
** 函数作用：打开jpg解码器
** 返回参数说明：
***/
bool jpg_decode_open(void (*read_frame)(struct ak_vdec_frame *frame))
{
	pthread_mutex_lock(&jpg_decode_mutex);
	if (jpg_decode_enable == true)
	{
		pthread_mutex_unlock(&jpg_decode_mutex);
		return false;
	}
	jpg_decode_frame_read_callback = read_frame;
	jpg_decode_enable = true;
	pthread_mutex_unlock(&jpg_decode_mutex);
	return true;
}

/***
** 日期: 2022-05-10 13:33
** 作者: leo.liu
** 函数作用：
** 返回参数说明：
***/
bool jpg_decode_close(void)
{
	pthread_mutex_lock(&jpg_decode_mutex);
	if (jpg_decode_enable == false)
	{
		pthread_mutex_unlock(&jpg_decode_mutex);
		return true;
	}
	ak_vdec_end_stream(jpg_decode_handle_id);
	jpg_decode_enable = false;
	pthread_mutex_unlock(&jpg_decode_mutex);
	return true;
}

/***
**   日期:2022-05-23 16:19:24
**   作者: leo.liu
**   函数作用：等待解码器开启
**   参数说明:
***/
static bool jpeg_decode_enable_wait(void)
{
	int count = 100;
	while (jpg_decode_handle_id == -1)
	{
		usleep(10 * 1000);
		count--;
		if (count == 0)
		{
			return false;
		}
	}
	return true;
}

/***
** 日期: 2022-05-10 13:37
** 作者: leo.liu
** 函数作用：写入解码流
** 返回参数说明：
***/
bool jpg_decode_stream_write(const unsigned char *data, int size)
{
	pthread_mutex_lock(&jpg_decode_mutex);
	if (jpg_decode_enable == false)
	{
		pthread_mutex_unlock(&jpg_decode_mutex);
		return false;
	}
	else if (jpg_decode_handle_id == -1)
	{
		pthread_mutex_unlock(&jpg_decode_mutex);
		if (jpeg_decode_enable_wait() == false)
		{
			return false;
		}
		pthread_mutex_lock(&jpg_decode_mutex);
	}
	int dec_len = 0;
	int read_len = size;
	int send_len = 0;
	while (read_len > 0)
	{
		ak_vdec_send_stream(jpg_decode_handle_id, &data[send_len], read_len, NONBLOCK, &dec_len);
		read_len -= dec_len;
		send_len += dec_len;
	}
	jpg_decode_frame_count++;
	pthread_mutex_unlock(&jpg_decode_mutex);
	return true;
}

/***
** 日期: 2022-05-10 13:41
** 作者: leo.liu
** 函数作用：判断解码线程是否彻底关闭
** 返回参数说明：
***/
bool jpg_decode_device_state(void)
{
	return jpg_decode_handle_id != -1 ? true : false;
}