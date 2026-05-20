#include "wifi_api.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "string.h"
#include <sys/socket.h>
#include <arpa/inet.h>

static int scanf_wifi_cont = 0;
static wifi_info scanf_wifi_info[20];

/***
**   日期:2022-06-02 15:55:31
**   作者: leo.liu
**   函数作用：关闭wifi
**   参数说明:
***/
bool wifi_device_close(void)
{
	system("ifconfig wlan0 down");
	return true;
}

/***
**   日期:2022-06-02 15:58:44
**   作者: leo.liu
**   函数作用：开启wifi
**   参数说明:
***/
bool wifi_device_open(void)
{
	system("ifconfig wlan0 up");
	system("killall udhcpc");
	system("udhcpc -b -i wlan0 &");
	return true;
}

/***
**   日期:2022-06-02 16:00:02
**   作者: leo.liu
**   函数作用：连接缓存文件
**   参数说明:
***/
bool wifi_device_temp_conneting(void)
{
	if (access("/tmp/wpa_supplicant.conf", F_OK))
	{
		printf("wpa_supplicant.conf not find \n");
		return false;
	}
	system("killall wpa_supplicant");
	system("killall udhcpc");
	system("wpa_supplicant -Dnl80211 -i wlan0 -c /tmp/wpa_supplicant.conf -B");
	system("udhcpc -b -i wlan0 &");
	return true;
}
/***
**   日期:2022-06-02 16:15:14
**   作者: leo.liu
**   函数作用：连接wifi
**   参数说明:
***/
bool wifi_device_conneting(void)
{
	if (access("/etc/config/wpa_supplicant.conf", F_OK))
	{
		printf("wpa_supplicant.conf not find \n");
		return false;
	}
	system("killall wpa_supplicant");
	system("killall udhcpc");
	system("wpa_supplicant -Dnl80211 -i wlan0 -c /etc/config/wpa_supplicant.conf -B");
	system("udhcpc -b -i wlan0 &");
	return true;
}
/***
**   日期:2022-06-02 16:03:37
**   作者: leo.liu
**   函数作用：获取wifi连接状态
**   参数说明:
***/
bool wifi_device_connection_stauts(unsigned char *user, bool *free, unsigned char *ip, bool *conneted, unsigned char *flags)
{
	FILE *fp = popen("wpa_cli -i wlan0 status", "r");
	if (fp == NULL)
	{
		printf("popen wpa_cli -i wlan0 status failed \n");
		return false;
	}
	char buffer[1024];
	if (conneted != NULL)
	{
		*conneted = false;
	}
	if (free != NULL)
	{
		*free = false;
	}
	while (fgets(buffer, sizeof(buffer), fp) > 0)
	{
		if ((user != NULL) && (strncmp(buffer, "ssid=", 5) == 0))
		{
			char *p = strchr(buffer, '=') + 1;
			strcpy((char *)user, p);
			user[strlen((char *)user) - 1] = '\0';
		}
		else if ((conneted != NULL) && (strncmp(buffer, "wpa_state=COMPLETED", 19) == 0))
		{
			*conneted = true;
		}
		else if ((ip != NULL) && (strncmp(buffer, "ip_address=", 11) == 0))
		{
			char *p = strchr(buffer, '=') + 1;
			strcpy((char *)ip, p);
			ip[strlen((char *)ip) - 1] = '\0';
		}
		else if ((strncmp(buffer, "key_mgmt=", 9) == 0))
		{
			char *p = strchr(buffer, '=') + 1;
			if (free != NULL)
			{
				if (strncmp(p, "NONE", 4) == 0)
				{
					*free = true;
				}
			}
			if (flags != NULL)
			{
				strncpy((char *)flags, p, 64);
				flags[strlen((char *)flags) - 1] = '\0';
				printf("[%s]:%d\n", user, __LINE__);
			}
		}
	}
	pclose(fp);
	return true;
}
/***
**   日期:2022-06-02 16:08:34
**   作者: leo.liu
**   函数作用：将密码和用户保存到缓存
**   参数说明:
***/
bool wifi_device_user_to_temp(unsigned char *user, unsigned char *pwd)
{
	char buffer[10 * 1024] = {0};
	bool free = (pwd == NULL) ? true : false;
	if (free == false)
	{
		printf("+++++++++++++++++========free!=NULL\n");

		sprintf(buffer,
			"ctrl_interface=/var/run/wpa_supplicant"
			"\nupdate_config=1"
			"\nnetwork={"
			"\n\t	scan_ssid=1"
			"\n\t	key_mgmt=WPA-PSK"
			"\n\t	ssid=\"%s\""
			"\n\t	psk=\"%s\""
			"\n}"
			"\n",
			user, pwd);
	}
	else
	{
		printf("+++++++++++++++++========free==NULL\n");
		sprintf(buffer,
			"ctrl_interface=/var/run/wpa_supplicant"
			"\nupdate_config=1"
			"\nnetwork={"
			"\n\t	scan_ssid=1"
			"\n\t	key_mgmt=NONE"
			"\n\t	ssid=\"%s\""
			"\n\t	scan_ssid=1"
			"\n}"
			"\n",
			user);
	}

	FILE *fp = fopen("/tmp/wpa_supplicant.conf", "wb");
	if (fp == NULL)
	{
		printf("open /tmp/wpa_supplicant.conf faild\n");
		return false;
	}
	fwrite(buffer, strlen(buffer), 1, fp);
	fclose(fp);
	return true;
}
/***
**   日期:2022-06-02 16:12:47
**   作者: leo.liu
**   函数作用：同步wifi信息
**   参数说明:
***/
bool wifi_device_tmp_sync(void)
{
	if (access("/tmp/wpa_supplicant.conf", F_OK))
	{
		printf("wpa_supplicant.conf not find \n");
		return false;
	}
	system("rm -f /etc/cofnig/wpa_supplicant.conf");
	system("cp /tmp/wpa_supplicant.conf /etc/config/");
	system("rm -f /tmp/wpa_supplicant.conf");
	system("sync");
	return true;
}

/***
**   日期:2022-06-02 16:25:45
**   作者: leo.liu
**   函数作用：执行扫描操作
**   参数说明:
***/
static bool wifi_device_scanf_signal(void)
{
	/*****  扫描太快，容易出现wifi busy，所以循环三次 *****/
	bool scanf_reslut = false;
	for (int i = 0; i < 3; i++)
	{
		FILE *fp = popen("wpa_cli scan", "r");
		if (fp == NULL)
		{
			printf("popen fail wpa_cli scan failed \n");
			return false;
		}
		char buffer[1024] = {0};
		while (fgets(buffer, sizeof(buffer), fp) > 0)
		{
			if (strncmp(buffer, "OK", 2) == 0)
			{
				scanf_reslut = true;
				break;
			}
		}

		pclose(fp);
		if (scanf_reslut == true)
		{
			break;
		}
		usleep(1000 * 200);
	}
	return scanf_reslut;
}
/***
**   日期:2022-06-02 16:36:50
**   作者: leo.liu
**   函数作用：解析扫描的数据
**   参数说明:
***/
static void wifi_device_info_parsing(char *buffer)
{
	unsigned char bssid[128] = {0};
	unsigned int frequency;
	printf("%d.%s\n", scanf_wifi_cont, buffer);
	int ret =  sscanf(buffer, "%s	%d	%d	%s	%s", bssid, &frequency, &scanf_wifi_info[scanf_wifi_cont].db, scanf_wifi_info[scanf_wifi_cont].psk_flags, scanf_wifi_info[scanf_wifi_cont].name);
	if(ret < 5){
		return;
	}
	char *ps_name = strrchr(buffer, '	') + 1;
	memset(scanf_wifi_info[scanf_wifi_cont].name, 0, sizeof(scanf_wifi_info[scanf_wifi_cont].name));
	strncpy(scanf_wifi_info[scanf_wifi_cont].name, ps_name, strlen(ps_name) - 1);

	if (strcmp(scanf_wifi_info[scanf_wifi_cont].psk_flags, "[ESS]") == 0)
	{
		scanf_wifi_info[scanf_wifi_cont].free = true;
	}
	else
	{
		scanf_wifi_info[scanf_wifi_cont].free = false;
	}
	scanf_wifi_cont++;
}
/***
**   日期:2022-06-02 16:31:28
**   作者: leo.liu
**   函数作用：获取扫描后的结果
**   参数说明:
***/
static bool wifi_device_scanf_reslut(void)
{
	FILE *fp = popen("wpa_cli scan_result", "r");
	if (fp == NULL)
	{
		printf("popen wpa_cli scan_result \n\r");
		return false;
	}
	memset(scanf_wifi_info, 0, sizeof(scanf_wifi_info));
	scanf_wifi_cont = 0;

	char buffer[1024] = {0};
	int line_index = 0;
	while (fgets(buffer, sizeof(buffer), fp) > 0)
	{
		if (scanf_wifi_cont >= 20)
		{
			break;
		}
		if (line_index > 1)
		{
			wifi_device_info_parsing(buffer);
		}
		line_index++;
	}
	pclose(fp);
	return true;
}
/***
**   日期:2022-06-02 16:18:52
**   作者: leo.liu
**   函数作用：扫描wifi
**   参数说明:
***/
bool wifi_device_scanf(void)
{
	if (wifi_device_scanf_signal() == false)
	{
		printf("scanf wifi failed \n");
		return false;
	}
	wifi_device_scanf_reslut();
	return true;
}
/***
**   日期:2022-06-02 16:50:15
**   作者: leo.liu
**   函数作用：获取扫描的结果
**   参数说明:
***/
bool wifi_device_scanf_info_get(wifi_info **info, int *total)
{
	if (scanf_wifi_cont == 0)
	{
		return false;
	}
	*info = scanf_wifi_info;
	*total = scanf_wifi_cont;
	return true;
}
/***
**   日期:2022-06-02 16:59:55
**   作者: leo.liu
**   函数作用：获取当前wifi的物理地址
**   参数说明:hwadders:必须大于64个字节
***/
bool wifi_device_hwadders_get(unsigned char *hwadders)
{
	if (hwadders == NULL)
	{
		return false;
	}
	FILE *fp = popen("cat /sys/class/net/wlan0/address", "r");
	if (fp == NULL)
	{
		printf("popen /sys/class/net/wlan0/address failed \n\r");
		return false;
	}
	fgets((char *)hwadders, 64, fp);
	pclose(fp);
	return true;
}
/***
**   日期:2022-06-08 17:42:41
**   作者: leo.liu
**   函数作用：判断wpa_supplicant线程存在
**   参数说明:
***/
bool wpa_supplicant_pid_exist_check(void)
{
	bool reslut = false;
	FILE *fp = popen("ps -ef | grep wpa_supplicant", "r");
	if (fp == NULL)
	{
		return false;
	}
	char buffer[1024] = {0};
	int line_index = 0;
	while (fgets(buffer, sizeof(buffer), fp) > 0)
	{

		if (strstr(buffer, "wpa_supplicant"))
		{
			line_index++;
			if (line_index > 2)
			{
				reslut = true;
				break;
			}
		}
	}
	pclose(fp);
	return reslut;
}


/*************************************************************************
 * @brief  网卡路由快速设置
 * @date   2023-06-20 19:51
 * @author xiaole
 * @param  add  [1]-add	[0]-del
 * @param  name "wlan0"	"br0"
 * @note 

 **************************************************************************/
void route_224_0_0_0_select(int add, char *name){
	char cmd[128] = {0};
	memset(cmd, 0, sizeof(cmd));
	snprintf(cmd, 128,"route %s -net 224.0.0.0 netmask 240.0.0.0 dev %s", (add == 1 ? "add" : "del"), name);

	// LOG_WHITE("%s\n", cmd);
	system(cmd);
}

bool network_ping_test(const char *address)
{

#define BUF_SIZE	(64)
	if (strlen(address) == 0 || strlen(address) > BUF_SIZE){
		return false;
	}

	char buf[BUF_SIZE] ={0};
	int ret = snprintf(buf, BUF_SIZE,"ping -W 1 -c 2 -i 0.2 %s", address);
	// LOG_WHITE("buf:%s\n", buf);

	if (ret > BUF_SIZE)	printf("Data leakage, memory offside !!!\n");

	FILE *fp = popen(buf,"r");
	if (fp == NULL){
		printf("err: open [%s] fail...\n",buf);
		return false;
	}


	char buffer[32] ={0};
	while (fgets(buffer,sizeof(buffer),fp)){
		if (strstr(buffer, "ttl=") != NULL){
			pclose(fp);
			return true;
		}
		memset(buffer,0,sizeof(buffer));
	}

	pclose(fp);
	return false;

}