#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdbool.h>
#include <time.h>

#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <mqueue.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define DBUG (printf("\033[31m %s[%u]: \033[0m ", __PRETTY_FUNCTION__, __LINE__), printf)
#define DBUG_LINE DBUG("=====================================\n");
typedef struct
{
	unsigned char *data;
	int size;
	unsigned long long timestamp;
	char type;
	int w;
	int h;
	int pos_x;
	int pos_y;
	int pos_w;
	int pos_h;
	int phyaddr;
} common_data;

typedef struct
{
	long type;
	common_data msg;
} common_queue;

/***
** 日期: 2022-04-27 10:37
** 作者: leo.liu
** 函数作用：创建消息队列句柄
** 返回参数说明：
***/
int msg_queue_create(void);
/***
** 日期: 2022-04-27 10:53
** 作者: leo.liu
** 函数作用：16BIT的PCM音量增幅转换
** 返回参数说明：传入最大帧为（PCM16BIT_BUF_SIZE_MAX）
***/
bool pcm16bit_volume_set(const unsigned char *src, int size, unsigned char **dst, int volume);
/***
** 日期: 2022-04-27 11:53
** 作者: leo.liu
** 函数作用：根据参数动态分配内存，并copy数据
** 返回参数说明：
***/
unsigned char *user_mem_copy(const unsigned char *data, int size);
/***
**   日期:2022-06-17 09:52:44
**   作者: leo.liu
**   函数作用：设置线程堆栈属性
**   参数说明:
***/
pthread_attr_t *user_pthread_atter_get(void);
#endif