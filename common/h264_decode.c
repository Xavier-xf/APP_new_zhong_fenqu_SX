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
#include "user_common.h"
/***** 解码后数据输出回调函数 *****/
static decode_finish_callback h264_decode_frame_read_callback = NULL;

/*****  解码句柄 *****/
static int h264_decode_handle_id = -1;

/***** 解码使能 *****/
static bool h264_decode_enable = false;

static int h264_decode_frame_count = 0;
/*****  接收视频帧 *****/
static int h264_decode_queue_head = -1;
static pthread_mutex_t h264_decode_mutex;
/***
** 日期: 2022-05-10 11:23
** 作者: leo.liu
** 函数作用：打开解码器
** 返回参数说明：
***/
static void h264_decode_device_open(void)
{
	struct ak_vdec_param param;
	memset(&param, 0, sizeof(struct ak_vdec_param));
	param.vdec_type = H264_ENC_TYPE;
	param.output_type = AK_YUV420SP;
	param.sc_width = 1920;
	param.sc_height = 1080;
	param.stream_buf_size = 1500 * 1024;
	param.frame_buf_num = 3;
	ak_vdec_open(&param, &h264_decode_handle_id);
	ak_vdec_clear_buff(h264_decode_handle_id);
}

/***
** 日期: 2022-05-10 11:23
** 作者: leo.liu
** 函数作用：关闭解码器
** 返回参数说明：
***/
static void h264_decode_device_close(void)
{
	ak_vdec_close(h264_decode_handle_id);
	h264_decode_handle_id = -1;
}

/***
** 日期: 2022-05-10 10:59
** 作者: leo.liu
** 函数作用：接收数据队列函数
** 返回参数说明：
***/
static void *h264_decode_task(void *arg)
{
	struct ak_vdec_frame frame = {0};
	int status = 0;
	printf("***** h264 stream task create sccess ! *****\n");
	while (1)
	{
		pthread_mutex_lock(&h264_decode_mutex);
		if ((h264_decode_enable == true) && (h264_decode_handle_id == -1))
		{
			h264_decode_device_open();
		}
		else if (h264_decode_handle_id != -1)
		{
			memset(&frame, 0, sizeof(struct ak_vdec_frame));
			if (ak_vdec_get_frame(h264_decode_handle_id, &frame) == 0)
			{
				if ((h264_decode_enable == true) && (h264_decode_frame_read_callback != NULL))
				{
					h264_decode_frame_read_callback(&frame);
				}
				ak_vdec_release_frame(h264_decode_handle_id, &frame);
				h264_decode_frame_count--;
			}
			else if ((h264_decode_enable == false))
			{
				ak_vdec_get_decode_finish(h264_decode_handle_id, &status);
				if (status)
				{
					h264_decode_device_close();
				}
			}
		}
		pthread_mutex_unlock(&h264_decode_mutex);
		usleep(1 * 1000);
	}
	return NULL;
}

/***
**   日期:2022-05-25 11:25:40
**   作者: leo.liu
**   函数作用：发送到解码器
**   参数说明:
***/
static void h264_decode_frame_send(const unsigned char *data, int size)
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

/***
**   日期:2022-05-25 10:41:05
**   作者: leo.liu
**   函数作用：视频接收任务
**   参数说明:
***/
static void *h264_decode_receive_task(void *arg)
{
	common_queue node;
	while (msgrcv(h264_decode_queue_head, (void *)&node, sizeof(common_data), 0, IPC_NOWAIT) > 0)
		;

	while (1)
	{
		usleep(100000000);
		continue;
		if (msgrcv(h264_decode_queue_head, (void *)&node, sizeof(common_data), sizeof(common_data), IPC_NOWAIT) > 0)
		{
			pthread_mutex_lock(&h264_decode_mutex);
			if (h264_decode_handle_id != -1)
			{
				h264_decode_frame_send(node.msg.data, node.msg.size);
				h264_decode_frame_count++;
			}
			pthread_mutex_unlock(&h264_decode_mutex);
			ak_mem_free(node.msg.data);
		}

		usleep(10 * 1000);
	}
	return NULL;
}
/***
** 日期: 2022-05-10 10:55
** 作者: leo.liu
** 函数作用：h264解码器初始化
** 返回参数说明：
***/
bool h264_decode_init(void)
{
	static bool inited = false;
	if (inited == true)
	{
		printf("h264 decode It's already initialized \n");
		return false;
	}
	inited = true;
	pthread_t pthread;
	h264_decode_queue_head = msg_queue_create();
	pthread_mutex_init(&h264_decode_mutex, NULL);
	pthread_create(&pthread, user_pthread_atter_get(), h264_decode_task, NULL);
	pthread_create(&pthread, user_pthread_atter_get(), h264_decode_receive_task, NULL);
	return true;
}

/***
** 日期: 2022-05-10 12:02
** 作者: leo.liu
** 函数作用：打开h264解码器
** 返回参数说明：
***/
bool h264_decode_open(decode_finish_callback read_frame)
{
	pthread_mutex_lock(&h264_decode_mutex);
	if (h264_decode_enable == true)
	{
		pthread_mutex_unlock(&h264_decode_mutex);
		return false;
	}
	h264_decode_frame_read_callback = read_frame;
	h264_decode_enable = true;
	pthread_mutex_unlock(&h264_decode_mutex);
	return true;
}

/***
** 日期: 2022-05-10 13:33
** 作者: leo.liu
** 函数作用：
** 返回参数说明：
***/
bool h264_decode_close(void)
{
	printf("==========>> \n");
	pthread_mutex_lock(&h264_decode_mutex);
	printf("==========>> \n");
	if (h264_decode_enable == false)
	{
		printf("==========>> \n");
		pthread_mutex_unlock(&h264_decode_mutex);
		return true;
	}
	printf("==========>> \n");
	ak_vdec_end_stream(h264_decode_handle_id);
	printf("==========>> \n");
	h264_decode_enable = false;
	pthread_mutex_unlock(&h264_decode_mutex);
	printf("==========>> \n");
	return true;
}
/***
** 日期: 2022-05-10 13:37
** 作者: leo.liu
** 函数作用：写入解码流
** 返回参数说明：
***/
bool h264_decode_stream_write(const unsigned char *data, int size)
{
	if (h264_decode_handle_id != -1)
	{
		h264_decode_frame_send(data, size);
		ak_vdec_frame_decode(h264_decode_handle_id);
	}
	return true;

	common_queue node;
	node.type = sizeof(common_data);
	node.msg.data = user_mem_copy(data, size);
	node.msg.size = size;
	msgsnd(h264_decode_queue_head, &node, node.type, 0);
	return true;
}

/***
** 日期: 2022-05-10 13:41
** 作者: leo.liu
** 函数作用：判断解码线程是否彻底关闭
** 返回参数说明：
***/
bool h264_decode_device_state(void)
{
	return h264_decode_handle_id != -1 ? true : false;
}
/***
**   日期:2022-05-24 16:54:22
**   作者: leo.liu
**   函数作用：修改视频输出的解码函数
**   参数说明:
***/
decode_finish_callback h264_decode_read_frame_func_register(decode_finish_callback read_frame)
{
	pthread_mutex_lock(&h264_decode_mutex);
	if (h264_decode_enable == false)
	{
		pthread_mutex_unlock(&h264_decode_mutex);
		return NULL;
	}
	decode_finish_callback old_callback = h264_decode_frame_read_callback;
	h264_decode_frame_read_callback = read_frame;
	pthread_mutex_unlock(&h264_decode_mutex);
	return old_callback;
}
/***
**   日期:2022-05-25 18:21:21
**   作者: leo.liu
**   函数作用：获取带解码帧
**   参数说明:
***/
void h264_decode_buffer_clear(void)
{
	pthread_mutex_lock(&h264_decode_mutex);
	if (h264_decode_handle_id != -1)
	{
		ak_vdec_clear_buff(h264_decode_handle_id);
	}
	pthread_mutex_unlock(&h264_decode_mutex);
}

int h264_decode_queue_num_get(void)
{
	struct msqid_ds buf;
	// 获取消息队列的状态信息
	if (msgctl(h264_decode_queue_head, IPC_STAT, &buf) == -1)
	{
		perror("msgctl");
		return 1;
	}

	// 打印消息队列中的消息数量
	printf("Messages in queue: %ld\n", buf.msg_qnum);
	return buf.msg_qnum;
}