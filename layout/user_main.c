
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <mqueue.h>
#include <ctype.h>

#include "ak_common_graphics.h"
#include "ak_common.h"
#include "ak_tde.h"
#include "user_common.h"
#include "layout_define.h"
#include "lv_msg_event.h"
#include "ringplay.h"
#include "signal.h"

extern void lv_init(void);
extern void lv_display_init(void);
extern void lv_port_indev_init(void);

extern void upgrade_check_firmware(void);



/***
** 日期: 2022-04-22 15:31
** 作者: leo.liu
** 函数作用：打印当前项目的相关信息
** 返回参数说明：
***/
static void project_printf(void)
{
	printf("********************************\n");
	printf("***** project: CDV1000QT   *****\n");
	printf("***** author:  leo		   *****\n");
	printf("***** date:    2022/04/22  *****\n");
	printf("********************************\n");

	// system("insmod /usr/modules/ak_fb.ko");
	system("insmod /usr/modules/ak_gui.ko");
	system("insmod /usr/modules/ts_gsl.ko");
	system("insmod /usr/modules/ak_saradc.ko");
	system("insmod /usr/modules/ak_pwm_char.ko");
}

/***
** 日期: 2022-04-23 08:12
** 作者: leo.liu
** 函数作用：平台接口初始化
** 返回参数说明：
***/
// #include "ak_its.h"
static void platform_sdk_init(void)
{
	sdk_run_config config;

	memset(&config, 0, sizeof(sdk_run_config));
	//	config.isp_tool_server_flag = 1;
		// config.mem_trace_flag = SDK_RUN_DEBUG;
		// config.dma_mem_trace_flag = SDK_RUN_DEBUG;

	ak_sdk_init(&config);
	/***** 打开tde 模块 ******/
	ak_tde_open();

	//	ak_its_start(1234);
}

static void *lv_sys_tick_task(void *arg)
{
	struct timeval tv1, tv2;
	gettimeofday(&tv2, NULL);

	while (1)
	{
		gettimeofday(&tv1, NULL);
		lv_tick_inc(tv1.tv_sec * 1000 + tv1.tv_usec / 1000 - tv2.tv_sec * 1000 - tv2.tv_usec / 1000);
		gettimeofday(&tv2, NULL);
		usleep(1000);
	}

	return NULL;
}

/***
** 日期: 2022-04-25 14:22
** 作者: leo.liu
** 函数作用：lvgl的任务调度以及心跳包线程
** 返回参数说明：
***/
static void lv_task_scheduling_start(void)
{
	pthread_t task_id;

	goto_layout(pLAYOUT(logo));
	pthread_create(&task_id, user_pthread_atter_get(), lv_sys_tick_task, NULL);

	while (1)
	{
		lv_task_handler();
		usleep(1000);
	}
}

void auto_run_sh_check(void)
{

#define AUTO_RUN_SH_PATH "/etc/config/leo_auto_run.sh"
	if (access(AUTO_RUN_SH_PATH, F_OK) != 0)
	{

		char *data = "#!/bin/bash\n"
					 "PRONAME=CDV1004QT.BIN\n"
					 "PRODIR=/etc/config/run/\n"
					 "while true\n"
					 "do\n"
					 "bRunning=`ps -ef | grep $PRONAME| grep -v \"grep\" | wc -l`\n"
					 "if [[ $bRunning != 0 ]];\n"
					 "then\n"
					 "usleep 100000\n"
					 "else\n"
					 "echo \"CDV1004QT not run\"\n"
					 "killall CDV1004QT.BIN\n"
					 "/app/app/CDV1004QT.BIN leo &\n"
					 "sleep 10\n"
					 "fi\n"
					 "done\n";

		int fd = open(AUTO_RUN_SH_PATH, O_CREAT | O_WRONLY);
		write(fd, data, strlen(data));
		close(fd);
		system("sync");
		system("chmod 777 " AUTO_RUN_SH_PATH);
	}

	system("killall leo_auto_run.sh");
	// system("killall sh leo_auto_run.sh");
	system("sh " AUTO_RUN_SH_PATH " &");
}



/*************************************************************************
 * @brief  main函数中调用此函数即可开启core dump
 * @date   2023-06-14 19:58
 * @note   摘抄至 https://blog.csdn.net/u012778549/article/details/104839187
 **************************************************************************/
// static void enableCoreDumpFunc(){
// #include <sys/resource.h>
// #include <errno.h>
// #define _MY_ULIMIT_SIZE		1024*1024*40		//RLIM_INFINITY		//

// 	// run_core_dump_script(true);


// 	struct rlimit limitParam;
// 	limitParam.rlim_cur = _MY_ULIMIT_SIZE;
// 	limitParam.rlim_max = _MY_ULIMIT_SIZE;
// 	setrlimit(RLIMIT_CORE,&limitParam);


// 	system("echo /tmp/coredump_%e_%p_%t > /proc/sys/kernel/core_pattern");	/* 设置文件存放路径 */
// 	system("cat /proc/sys/kernel/core_pattern");
// 	// LOG_B_CYAN("Set coredump file size to %lu, path = /tmp/coredump_ \n",limitParam.rlim_cur);

// }



bool LEO_FAST_ENTER_SYSTEM_FLAG = false;
//  int enter_main_num = 0;
/***
** 日期: 2022-04-22 15:30
** 作者: leo.liu
** 函数作用：主函数入口
** 返回参数说明：
***/
int main(int argc, char *argv[])
{
	printf(" main argc================= arg:%d\n", argc);	

	// enter_main_num++;
	// printf("#######enter_main_num#######\n");
	signal(SIGPIPE, SIG_IGN);

	// enableCoreDumpFunc();

	if (argc == 2)
	{

		// printf("arg:%d :%s\n",argc,argv[1]);

		if (strcmp(argv[1], "leo") == 0)
		{

			LEO_FAST_ENTER_SYSTEM_FLAG = true;
			printf("===================fast mode enter system leo\n");
		}
	}
	else if (argc == 1)
	{

		auto_run_sh_check();
		system("insmod /usr/modules/ak_fb.ko");
	}

	project_printf();

	// upgrade_check_firmware();

	platform_sdk_init();

	/***** 初始化LVGL *****/
	lv_init();
	lv_display_init();
	lv_port_indev_init();

	/***** 获取用户数据 *****/
	user_data_init();

	/***** 开启app系统调度 *****/
	lv_event_task_init();
	lv_task_scheduling_start();
	return 0;
}



