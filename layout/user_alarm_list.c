#include "user_alarm_list.h"

#define USER_ALARM_LIST_PATH "/etc/config/user_alarm_list.cfg"
typedef struct
{
	int ch;
	struct tm time;
} alarm_list_info;

static int alarm_list_total = 0;
static alarm_list_info alarm_list[256];

static bool alarm_list_sync(void)
{
	int fd = open(USER_ALARM_LIST_PATH, O_CREAT | O_WRONLY);
	if (fd < 0)
	{
		return false;
	}
	write(fd, alarm_list, sizeof(alarm_list_info) * alarm_list_total);
	close(fd);
	return true;
}
/***
**   日期:2022-06-20 09:56:23
**   作者: leo.liu
**   函数作用：加载列表
**   参数说明:
***/
bool alarm_list_init(void)
{
	alarm_list_total = 0;
	memset(alarm_list, 0, sizeof(alarm_list));
	int fd = open(USER_ALARM_LIST_PATH, O_RDONLY);
	if (fd < 0)
	{
		return false;
	}
	alarm_list_info info;
	int len = sizeof(alarm_list_info);
	int read_len = 0;
	while ((read_len = read(fd, &info, len)) == len)
	{
		if ((info.ch != 1) && (info.ch != 2))
		{
			continue;
		}
		alarm_list[alarm_list_total++] = info;
		if (alarm_list_total == 256)
		{
			break;
		}
	}
	close(fd);
	alarm_list_sync();
	return true;
}
/***
**   日期:2022-06-20 10:01:56
**   作者: leo.liu
**   函数作用：添加一个警报信息
**   参数说明:
***/
bool alarm_list_add(int id, struct tm *tm)
{
	if (alarm_list_total == 256)
	{
		alarm_list_total--;
		memmove(&alarm_list[0], &alarm_list[1], alarm_list_total);
	}
	alarm_list[alarm_list_total].ch = id;
	alarm_list[alarm_list_total].time = *tm;
	alarm_list_total++;
	alarm_list_sync();
	return true;
}
/***
**   日期:2022-06-20 10:02:35
**   作者: leo.liu
**   函数作用：删除所有列表
**   参数说明:
***/
bool alarm_list_del_all(void)
{
	remove(USER_ALARM_LIST_PATH);
	system("sync");
	alarm_list_total = 0;
	memset(alarm_list, 0, sizeof(alarm_list));
	return true;
}
/***
**   日期:2022-06-20 10:14:01
**   作者: leo.liu
**   函数作用：获取列表
**   参数说明:
***/
bool alarm_list_get(int index, int *ch, struct tm *tm)
{
	if (index >= alarm_list_total)
	{
		return false;
	}

	*ch = alarm_list[index].ch;
	*tm = alarm_list[index].time;
	return true;
}
/***
**   日期:2022-06-20 10:20:05
**   作者: leo.liu
**   函数作用：获取总数
**   参数说明:
***/
bool alarm_list_total_get(int *total)
{
	*total = alarm_list_total;
	return true;
}