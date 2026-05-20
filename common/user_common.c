#include "user_common.h"
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include "ak_mem.h"

/***
** 日期: 2022-04-27 10:37
** 作者: leo.liu
** 函数作用：创建消息队列句柄
** 返回参数说明：
***/
int msg_queue_create(void)
{
	static int count = 0;
	count++;
	key_t key = ftok("/tmp", count + 'L');
	int queue_head = msgget(key, IPC_CREAT | 0666);
	printf("***** msg queue head %d(%d) create sccess ! *****\n", count, queue_head);
	return queue_head;
}
/***
** 日期: 2022-04-27 10:53
** 作者: leo.liu
** 函数作用：16BIT的PCM音量增幅转换
** 返回参数说明：传入最大帧为（PCM16BIT_BUF_SIZE_MAX）
***/
#define PCM16BIT_BUF_SIZE_MAX (4 * 1024)
bool pcm16bit_volume_set(const unsigned char *src, int size, unsigned char **dst, int volume)
{
	int i = 0;
	static unsigned char *pcm_buffer = NULL;
	if (pcm_buffer == NULL)
	{
		pcm_buffer = ak_mem_alloc(MODULE_ID_AO, PCM16BIT_BUF_SIZE_MAX);
	}
	*dst = pcm_buffer;

	if (size > PCM16BIT_BUF_SIZE_MAX)
	{
		printf("It's over the maximum \n");
		return false;
	}

	if (volume == 100)
	{
		memcpy(pcm_buffer, src, size);
		return true;
	}
	memset(pcm_buffer, 0, size);
	if (volume == 0)
	{
		return true;
	}

	float bar = volume * 1.0 / 100;
	for (i = 0; i < size; i += 2)
	{
		short src_data = (src[i + 1] << 8) | (src[i] & 0xFF);
		src_data = src_data * bar;
		if (src_data > 32767)
		{
			src_data = 32767;
		}
		else if (src_data < -32768)
		{
			src_data = -32768;
		}
		pcm_buffer[i] = src_data & 0xFF;
		pcm_buffer[i + 1] = (src_data >> 8) & 0xFF;
	}
	return true;
}
/***
** 日期: 2022-04-27 11:53
** 作者: leo.liu
** 函数作用：根据参数动态分配内存，并copy数据
** 返回参数说明：
***/
unsigned char *user_mem_copy(const unsigned char *data, int size)
{
	unsigned char *addr = ak_mem_alloc(MODULE_ID_AO, size);
	memcpy(addr, data, size);
	return addr;
}
/***
**   日期:2022-06-17 09:52:44
**   作者: leo.liu
**   函数作用：设置线程堆栈属性
**   参数说明:
***/
pthread_attr_t *user_pthread_atter_get(void)
{
	static pthread_attr_t thread_attr;
	size_t stacksize = 100 * 1024;
	pthread_attr_setstacksize(&thread_attr, stacksize);
	return &thread_attr;
}
