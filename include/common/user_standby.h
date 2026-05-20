#ifndef _USER_STANDBY_H_
#define _USER_STANDBY_H_
#include "lv_msg_event.h"
#include <stdbool.h>
/***
** 日期: 2022-05-10 08:33
** 作者: leo.liu
** 函数作用：初始化待机检测
** 返回参数说明：
***/
bool standby_timer_init(const layout *playout,int timeout);


/***
** 日期: 2022-05-10 08:47 
** 作者: leo.liu
** 函数作用：待机检测重新及时
** 返回参数说明：
***/
bool standby_timer_restart(bool fouce_enable);


/***
** 日期: 2022-05-10 08:42
** 作者: leo.liu
** 函数作用：关闭待机检测
** 返回参数说明：
***/
bool standby_timer_close(void);



#endif