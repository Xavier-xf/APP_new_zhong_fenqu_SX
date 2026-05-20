#include "user_time.h"
#include <sys/time.h>

/***
** 日期: 2022-05-09 17:29
** 作者: leo.liu
** 函数作用：获取当前日期是星期几
** 返回参数说明：
***/
int user_cur_time_is_week(struct tm *date)
{

	int m = date->tm_mon;
	int y = date->tm_year;
	int d = date->tm_mday;
	if (m == 1 || m == 2)
	{
		m += 12;
		y--;
	}
	return (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7 + 1;
}

/***
** 日期: 2022-05-09 17:32
** 作者: leo.liu
** 函数作用：获取当前月份下最后一天日期
** 返回参数说明：
***/
int user_cur_time_last_day(struct tm *date)
{
	unsigned int year = date->tm_year;
	unsigned char mon = date->tm_mon;
	if (mon == 2)
	{
		if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400) == 0)
		{
			return 29;
		}
		else
		{
			return 28;
		}
	}
	else
	{
		switch ((mon))
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			return 31;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			return 30;
			break;
		default:
			break;
		}
	}
	return 0;
}
/***
** 日期: 2022-04-26 10:58
** 作者: leo.liu
** 函数作用：将RTC时间同步到系统时间
** 返回参数说明：
***/
void user_time_sync(void)
{
	system("hwclock -s");
}

/***
** 日期: 2022-04-26 10:49
** 作者: leo.liu
** 函数作用：获取用户时间
** 返回参数说明：
***/
bool user_time_read(struct tm *tm)
{
	time_t t = time(NULL);

	localtime_r(&t, tm);
	tm->tm_year += 1900;
	tm->tm_mon += 1;
	tm->tm_wday = user_cur_time_is_week(tm);
	return true;
}
/***
** 日期: 2022-04-26 10:57
** 作者: leo.liu
** 函数作用：设置系统时间并同步到RTC
** 返回参数说明：
***/
bool user_time_set(struct tm *tm)
{
	char string[64] = {0};
	sprintf(string, "date -s \"%04d-%02d-%02d %02d:%02d:%02d\"", tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	system(string);
	/***** 将系统时间与RTC同步 ******/
	system("hwclock -w");


	return true;
}

/***
**   日期:2022-06-18 09:42:23
**   作者: leo.liu
**   函数作用：判断时间是否合法
**   参数说明:
***/
bool user_time_valid_check(struct tm *time)
{
	if (time->tm_year < 2003 || time->tm_year > 2099)
		return false;
	if (time->tm_mon < 1 || time->tm_mon > 12)
		return false;
	if (time->tm_mday < 1 || time->tm_mday > 31)
		return false;
	if (time->tm_hour < 0 || time->tm_hour > 23)
		return false;
	if (time->tm_min < 0 || time->tm_min > 59)
		return false;
	if (time->tm_sec < 0 || time->tm_sec > 59)
		return false;
	return true;
}

/***
** 日期: 2022-04-26 10:33
** 作者: leo.liu
** 函数作用：用户时间初始化
** 返回参数说明：
***/
bool user_time_init(void)
{
	struct tm tm;
	user_time_read(&tm);
	if (user_time_valid_check(&tm) == false)
	{
		tm.tm_year = 2024;
		tm.tm_mon = 1;
		tm.tm_mday = 1;
		tm.tm_hour = 0;
		tm.tm_min = 0;
		tm.tm_sec = 0;
		user_time_set(&tm);
	}
	user_time_sync();
	return true;
}
/***
** 日期: 2022-04-26 11:02
** 作者: leo.liu
** 函数作用：获取当前时间戳
** 返回参数说明：
***/
unsigned long long user_timestamp_get(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
/***
** 日期: 2022-04-26 11:02
** 作者: fei.liu
** 函数作用：获取当前时间戳
** 返回参数说明：
***/
unsigned long long user_timestamp_us_get(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}
/***
**   日期:2022-06-09 11:45:51
**   作者: leo.liu
**   函数作用：将tm转换成sec
**   参数说明:
***/
int user_tm_conver_sec(struct tm *info)
{
	info->tm_year -= 1900;
	info->tm_mon -= 1;
	info->tm_mday -= 1;
	info->tm_isdst = -1;
	return mktime(info);
}