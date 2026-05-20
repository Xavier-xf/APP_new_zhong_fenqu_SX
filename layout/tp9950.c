#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "user_gpio.h"
#include "user_monitor.h"
#include "i2c_ctrl.h"
#include "tp9950.h"
#include "user_time.h"
#include "user_data.h"
#include "video_input.h"
#include "user_monitor.h"
#include "layout_define.h"
#include <string.h>
#include "user_common.h"
#include "TP9950_AHD_1080P_CFG"
#include "TP9950_720P_AHD_CFG"
#include "TP9950_CVBS_CFG"

#define I2C_DEVICE_PATH 0
#define SENSOR_ADDRES 0X44

#define FLAG_LOSS 0x80
#define FLAG_H_LOCKED 0x20
#define FLAG_HV_LOCKED 0x60
enum
{
	VIDEO_UNPLUG,
	VIDEO_IN,
	VIDEO_LOCKED,
	VIDEO_UNLOCK
};
enum
{
	STD_TVI,
	STD_AHD,
	STD_CVI
};
enum
{
	MIPI_PAGE,
	VIDEO_PAGE
};

typedef struct
{

	int width;
	int height;
	int fps;
} tp9950_info;

/***
** 日期: 2022-05-12 13:37
** 作者: leo.liu
** 函数作用：sensor 格式
** 返回参数说明：
***/
static pthread_mutex_t tp9950_mutex;

/***
** 日期: 2022-05-12 13:58
** 作者: leo.liu
** 函数作用：tp9950 i2c wirte
** 返回参数说明：
***/
static bool tp9950_write(I2C_DATA *reg, int len)
{
	return i2c_write(I2C_DEVICE_PATH, SENSOR_ADDRES, reg, len);
}

/***
** 日期: 2022-05-12 13:59
** 作者: leo.liu
** 函数作用：tp9950 读取数据
** 返回参数说明：
***/
static bool tp9950_read(unsigned char reg, unsigned char *data)
{
	I2C_DATA vol;
	vol.reg = reg;
	if (i2c_read(I2C_DEVICE_PATH, SENSOR_ADDRES, &vol, 1) == false)
	{
		printf("tp9950 read reg(%02x) failed \n", reg);
		return false;
	}
	*data = vol.val;
	return true;
}

/***
** 日期: 2022-05-12 14:20
** 作者: leo.liu
** 函数作用：初始屏参
** 返回参数说明：
***/
static void tp9950_comm_init(void)
{
	I2C_DATA reg[] = {
	    {0x40, 0x00}, // default Vin1
	    //{0x41, 0x03},
	    {0x4c, 0x40},
	    {0x4e, 0x00},
	    {0x35, 0x25}, //-->0x6
	    {0xf5, 0x10},
	    {0xfd, 0x80}, // ->0x00
	    {0x38, 0x40},
	    {0x3d, 0x60}};
	int size = sizeof(reg) / sizeof(I2C_DATA);
	tp9950_write(reg, size);
}

/***
** 日期: 2022-05-12 14:20
** 作者: leo.liu
** 函数作用：s设置页面
** 返回参数说明：
***/
static void tp9950_set_reg_page(unsigned char ch)
{
	I2C_DATA reg[] = {
	    {0x40, 0x08},
	    {0x40, 0x00}};
	switch (ch)
	{
	case MIPI_PAGE:
		tp9950_write(&reg[0], 1);
		break;
	default:
		tp9950_write(&reg[1], 1);
		break;
	}
}

/***
** 日期: 2022-05-12 14:20
** 作者: leo.liu
** 函数作用：tp9950 软复位
** 返回参数说明：
***/
static void tp9950_reset_default(void)
{
	I2C_DATA reg[] = {
	    {0x40, 0x08},
	    {0x23, 0x02},
	    {0x40, 0x00},
	    {0x07, 0xC0},
	    {0x0B, 0xC0}};
	tp9950_write(reg, sizeof(reg) / sizeof(I2C_DATA));

	unsigned char tmp = 0x00;
	tp9950_read(0x26, &tmp);
	tmp &= 0xfe;
	reg[0].reg = 0x26;
	reg[0].val = tmp;
	tp9950_write(&reg[0], 1);

	tp9950_read(0xa7, &tmp);
	tmp &= 0xfe;
	reg[0].reg = 0xa7;
	reg[0].val = tmp;
	tp9950_write(&reg[0], 1);

	tp9950_read(0x06, &tmp);
	tmp &= 0xfb;
	reg[0].reg = 0x06;
	reg[0].val = tmp;
	tp9950_write(&reg[0], 1);
}
/***
** 日期: 2022-05-12 13:41
** 作者: leo.liu
** 函数作用：判断是否为无信号
** 返回参数说明：
***/
static bool is_tp9950_signal_loss(unsigned char *status)
{
	tp9950_set_reg_page(VIDEO_PAGE);
	tp9950_read(0x01, status);
	// printf("0x01:%02x \n", *status);

	// unsigned char data = 0;
	// tp9950_read(0x41, &data);
	//  printf("0x41:%02x \n",data);
	return ((*status) & FLAG_LOSS) ? true : false;
}

/***
** 日期: 2022-05-12 19:02
** 作者: leo.liu
** 函数作用：等待无信号处理
** 返回参数说明：
***/
#if 1
static void tp9950_loss_wait(void)
{
	int delay_count = 100;
	unsigned char status = 0x00;
	while (delay_count--)
	{
		if (is_tp9950_signal_loss(&status) == true)
		{
			break;
		}
		usleep(10 * 1000);
	}
}
#endif
/***
** 日期: 2022-05-12 13:40
** 作者: leo.liu
** 函数作用：获取tp9950的格式
** 返回参数说明：
***/
TP9950_FORMAT tp9950_format_detection(void)
{
#define MAX_COUNT 5//0x20

	unsigned int det_count = 0;
	unsigned char det_state = VIDEO_UNPLUG;
	TP9950_FORMAT det_format = INVALID_FORMAT;

	unsigned char status = 0x00;
	while (det_count < MAX_COUNT)
	{
		usleep(10 * 1000);
	//	printf("%02x \n", status);
		if (is_tp9950_signal_loss(&status) == true)
		{ // no video
			if (0 == det_count)
			{
				tp9950_reset_default();
				det_count++;
			}
			else
			{
				if (det_count < MAX_COUNT)
				{
					det_count++;
				}
				continue;
			}
		}
		else
		{
			unsigned char flag_locked = FLAG_HV_LOCKED;
			if (flag_locked == (status & flag_locked))
			{

				if (VIDEO_LOCKED == det_state)
				{
					if (det_count < MAX_COUNT)
					{
						det_count++;
					}
				}
				else if (VIDEO_UNPLUG == det_state)
				{
					det_state = VIDEO_IN;
					det_count = 0;
				}
				else if (det_format != INVALID_FORMAT)
				{
					det_state = VIDEO_LOCKED;
					det_count = 0;
				}
				else
				{
					if (det_count < MAX_COUNT)
					{
						det_count++;
					}
				}
			}
			else
			{ // video in but unlocked
				if (VIDEO_UNPLUG == det_state)
				{
					det_state = VIDEO_IN;
					det_count = 0;
				}
				else if (VIDEO_LOCKED == det_state)
				{
					det_state = VIDEO_UNLOCK;
					det_count = 0;
				}
				else if (VIDEO_LOCKED == det_state)
				{
					det_state = VIDEO_UNLOCK;
					det_count = 0;
				}
				else
				{
					if (det_count < MAX_COUNT)
					{
						det_count++;
					}
					if ((VIDEO_UNLOCK == det_state) && (det_count > 2))
					{
						det_state = VIDEO_IN;
						det_count = 0;
						tp9950_reset_default();
					}
				}
			}
		}
		if (VIDEO_IN == det_state)
		{
			unsigned char cvstd = 0x00;
			unsigned char data = 0x00;
			I2C_DATA reg = {0x2f, 0x09};
			tp9950_read(0x03, &cvstd);
			cvstd &= 0x0f;

			switch (cvstd & 0x07)
			{
			case TP2802_SD:
				tp9950_write(&reg, 1);
				usleep(1000);
				tp9950_read(0x04, &data);
				if (data == 0x94)
				{
					det_format = TP2802_PAL;
				}
				else if (data == 0x93)
				{
					det_format = TP2802_NTSC;
				}
				else
				{
					tp9950_write(&reg, 1);
					usleep(1000 * 100);
					tp9950_read(0x04, &data);
					if (data == 0x94)
					{
						det_format = TP2802_PAL;
					}
					else if (data == 0x93)
					{
						det_format = TP2802_NTSC;
					}
					else
					{
						det_format = INVALID_FORMAT;
					}
				}
				goto RETURN_MODE;
				break;
			case TP2802_720P25:
				det_format = TP2802_720P25V2;
				goto RETURN_MODE;
				break;
			case TP2802_720P30:
				det_format = TP2802_720P30V2;
				goto RETURN_MODE;
				break;
			case TP2802_1080P25:
				det_format = TP2802_1080P25;
				goto RETURN_MODE;
				break;
			case TP2802_1080P30:
				det_format = TP2802_1080P30;
				goto RETURN_MODE;
				break;
			default:
				// printf("not support:%d \r\n", cvstd & 0x07);
				break;
			}
		}
	}
RETURN_MODE:
	return det_format;
}

static bool tp9950_module_write_config(int width, int height, int fps)
{
	tp9950_info info;
	info.width = width;
	info.height = height;
	info.fps = fps;

	FILE *fp = fopen("/proc/TP9950", "wb");
	if (fp == NULL)
	{
		printf("open /proc/tp9950 fail \n\r");
		return false;
	}

	int size = sizeof(tp9950_info);
	if (fwrite(&info, size, 1, fp) < 0)
	{
		printf("write /proc/tp9950 fail \n\r");
		fclose(fp);
		return false;
	}
	fclose(fp);
	return true;
}

/****************************************
画面调整:
 调整机制:
    根据当前通道，获取其亮度，对比度，色
度的相对值（-10 10)
    根据当前制式，获取其绝对的默认值，根
据相对值后再算出其绝对值写入到寄存器。
备注：根据其相对值值，最终有一个步宽的设置
用于设置每次减1后，实际减的数值可以使其的
系数倍。
****************************************/
static unsigned char bright_volume_base_get(TP9950_FORMAT fmt)
{
	switch (fmt)
	{
	case TP2802_PAL:
		return 0x7F;
	case TP2802_NTSC:
		return 0xf8;
	case TP2802_720P25V2:
		return 0x00;
	case TP2802_720P30V2:
		return 0x14;
	case TP2802_1080P25:
		return 0x64;            //猫头鹰0x64 公版0x00
	case TP2802_1080P30:
		return 0x00;
	default:
		break;
	}
	return 0x00;
}
static unsigned char const_voluem_base_get(TP9950_FORMAT fmt)
{
	switch (fmt)
	{
	case TP2802_PAL:
		return 0x40;
	case TP2802_NTSC:
		return 0x50;
	case TP2802_720P25V2:
		return 0x40;
	case TP2802_720P30V2:
		return 0x54;
	case TP2802_1080P25:
		return 0x35;             //猫头鹰0x35 公版0x40
	case TP2802_1080P30:
		return 0x40;
	default:
		break;
	}
	return 0x00;
}
static unsigned char color_voluem_base_get(TP9950_FORMAT fmt)
{
	switch (fmt)
	{
	case TP2802_PAL:
		return 0x60;
	case TP2802_NTSC:
		return 0x60;
	case TP2802_720P25V2:
		return 0x40;
	case TP2802_720P30V2:
		return 0x40;
	case TP2802_1080P25:
		return 0x3F;           //猫头鹰0x3F  公版0x60
	case TP2802_1080P30:
		return 0x60;
	default:
		break;
	}
	return 0x00;
}
/***
** 日期: 2022-05-12 14:54
** 作者: leo.liu
** 函数作用：根据数值写入寄存器
** 返回参数说明：
***/

void display_bright_adj(int bright, TP9950_FORMAT fmt)
{
	static TP9950_FORMAT st_fmt;
	if (fmt != INVALID_FORMAT)
	{
		st_fmt = fmt;
	}
	unsigned char base = bright_volume_base_get(st_fmt);
	int vol = base + (bright - 10) * 3;
	I2C_DATA reg;
	reg.reg = 0x10;
	reg.val = vol;
	tp9950_write(&reg, 1);
}

/***
** 日期: 2022-05-17 09:06
** 作者: leo.liu
** 函数作用：对比度调整
** 返回参数说明：
***/
void display_const_adj(int cont, TP9950_FORMAT fmt)
{
	static TP9950_FORMAT st_fmt;
	if (fmt != INVALID_FORMAT)
	{
		st_fmt = fmt;
	}
	unsigned char base = const_voluem_base_get(st_fmt);
	int vol = base + (cont - 10) * 3;
	I2C_DATA reg;
	reg.reg = 0x11;
	reg.val = vol;
	tp9950_write(&reg, 1);
}

/***
** 日期: 2022-05-17 09:06
** 作者: leo.liu
** 函数作用：色度调整
** 返回参数说明：
***/
void display_color_adj(int color, TP9950_FORMAT fmt)
{
	static TP9950_FORMAT st_fmt;
	if (fmt != INVALID_FORMAT)
	{
		st_fmt = fmt;
	}

	unsigned char base = color_voluem_base_get(st_fmt);
	int vol = base + (color - 10) * 3;
	I2C_DATA reg;
	reg.reg = 0x12;
	reg.val = vol;
	tp9950_write(&reg, 1);
}

/***
** 日期: 2022-05-12 14:47
** 作者: leo.liu
** 函数作用：配置色度亮度对比度
** 返回参数说明：
***/

static void display_adjust_write_cfg(TP9950_FORMAT fmt)
{
	int bright, cont, color;
	MON_CH ch = monitor_channel_get();
	switch (ch)
	{
	case MON_CH_DOOR1:
		bright = user_data_get()->display.door1.bright;
		cont = user_data_get()->display.door1.cont;
		color = user_data_get()->display.door1.color;
		break;
	case MON_CH_DOOR2:
		bright = user_data_get()->display.door2.bright;
		cont = user_data_get()->display.door2.cont;
		color = user_data_get()->display.door2.color;
		break;
	case MON_CH_CCTV1:
		bright = user_data_get()->display.cctv1.bright;
		cont = user_data_get()->display.cctv1.cont;
		color = user_data_get()->display.cctv1.color;
		break;
	case MON_CH_CCTV2:
		bright = user_data_get()->display.cctv2.bright;
		cont = user_data_get()->display.cctv2.cont;
		color = user_data_get()->display.cctv2.color;
		break;
	default:
		return;
	}

	display_bright_adj(bright, fmt);
	display_const_adj(cont, fmt);
	display_color_adj(color, fmt);
}

/***
** 日期: 2022-05-12 14:40
** 作者: leo.liu
** 函数作用：打印当前信号
** 返回参数说明：
***/
static void tp9950_detection_printf(TP9950_FORMAT format)
{
	char *format_str[7] = {"FORMAT UNKNOW", "CVBS-PAL", "CVBS-NTSC", "AHD-720P-25FPS", "AHD-720P-30FPS", "AHD-1080P-25FPS", "AHD-1080P-30FPS"};
	TP9950_FORMAT format_type[7] = {INVALID_FORMAT, TP2802_PAL, TP2802_NTSC, TP2802_720P25V2, TP2802_720P30V2, TP2802_1080P25, TP2802_1080P30};

	for (int i = 0; i < 7; i++)
	{
		if (format_type[i] == format)
		{
			printf("\n\n\n=====================================:%s\n\n\n", format_str[i]);
			break;
		}
	}
}
/***
** 日期: 2022-05-12 14:36
** 作者: leo.liu
** 函数作用：根据制式刷入屏参
** 返回参数说明：
***/
static void tp9950_write_cfg(TP9950_FORMAT fmt)
{
	I2C_DATA *pdata = NULL;
	int data_len = 0;
	switch (fmt)
	{
	case TP2802_PAL:
		pdata = TP9950_CVBS_PAL_CFG;
		data_len = sizeof(TP9950_CVBS_PAL_CFG) / sizeof(I2C_DATA);
		tp9950_module_write_config(960, 288, 50);
		break;
	case TP2802_NTSC:
		pdata = TP9950_CVBS_NTSC_CFG;
		data_len = sizeof(TP9950_CVBS_NTSC_CFG) / sizeof(I2C_DATA);
		tp9950_module_write_config(960, 240, 60);
		break;
	case TP2802_720P25V2:
		pdata = TP9950_720P_AHD_25FPS_CFG;
		data_len = sizeof(TP9950_720P_AHD_25FPS_CFG) / sizeof(I2C_DATA);
		tp9950_module_write_config(1280, 720, 25);
		break;
	case TP2802_720P30V2:
		pdata = TP9950_720P_AHD_30FPS_CFG;
		data_len = sizeof(TP9950_720P_AHD_30FPS_CFG) / sizeof(I2C_DATA);
		tp9950_module_write_config(1280, 720, 30);
		break;
	case TP2802_1080P25:
		pdata = TP9950_1080P_AHD_25FPS_CFG;
		data_len = sizeof(TP9950_1080P_AHD_25FPS_CFG) / sizeof(I2C_DATA);
		tp9950_module_write_config(960, 1080, 25);
		break;
	case TP2802_1080P30:
		pdata = TP9950_1080P_AHD_30FPS_CFG;
		data_len = sizeof(TP9950_1080P_AHD_30FPS_CFG) / sizeof(I2C_DATA);
		tp9950_module_write_config(960, 1080, 30);
		break;
	default:
		return;
	}
	tp9950_write(pdata, data_len);

	display_adjust_write_cfg(fmt);
}

/***
** 日期: 2022-05-12 15:05
** 作者: leo.liu
** 函数作用：判断hv信号稳定
** 返回参数说明：
***/
static bool tp9950_hvsync_wait(void)
{
	return true;
	int delay = 50;
	while (delay--)
	{
		tp9950_set_reg_page(VIDEO_PAGE);
		unsigned char status = 0x00;
		tp9950_read(0x01, &status);
		if ((status & 0x60) == 0x60)
		{
			return true;
		}
		usleep(1000 * 10);
	}
	return false;
}

/***
** 日期: 2022-05-12 16:27
** 作者: leo.liu
** 函数作用：打开isp设备
** 返回参数说明：
***/
static bool video_isp_core_open(TP9950_FORMAT fmt)
{
#define SENSOR_LOAD_FILE_CVBS_PAL "/etc/isp_tp9950_dvp_960x288_nosignal.conf"
#define SENSOR_LOAD_FILE_720P25FPS "/etc/isp_tp9950_dvp_960x1080_30fps.conf"
	video_isp_param param;
	switch (fmt)
	{
	case TP2802_1080P30:
		strcpy(param.isp_conf_file, SENSOR_LOAD_FILE_720P25FPS);
		param.main_width = 960;
		param.main_hgith = 1080;

		param.sub_width = 960;//640;
		param.sub_hight = 1080;//360;

		param.fps = 18;//30;

		break;
	case TP2802_1080P25:
		strcpy(param.isp_conf_file, SENSOR_LOAD_FILE_720P25FPS);
		param.main_width = 960;
		param.main_hgith = 1080;

		param.sub_width = 960;//640;
		param.sub_hight = 1080;//360;

		param.fps = 18;//25;
		break;
	case TP2802_720P30V2:
		strcpy(param.isp_conf_file, SENSOR_LOAD_FILE_720P25FPS);
		param.main_width = 1280;
		param.main_hgith = 720;

		param.sub_width = 1280;//640;
		param.sub_hight = 720;//360;

		param.fps = 18;//30;
		break;
	case TP2802_720P25V2:
		strcpy(param.isp_conf_file, SENSOR_LOAD_FILE_720P25FPS);
		param.main_width = 1280;
		param.main_hgith = 720;

		param.sub_width = 1280;//640;
		param.sub_hight = 720;//360;

		param.fps = 18;//25;
		break;
	case TP2802_PAL:
		strcpy(param.isp_conf_file, SENSOR_LOAD_FILE_CVBS_PAL);
		param.main_width = 960;
		param.main_hgith = 288;

		param.sub_width = 960;//512; // 640;
		param.sub_hight = 288; // 288;

		param.fps = 18;//25;
		break;
	case TP2802_NTSC:
		strcpy(param.isp_conf_file, SENSOR_LOAD_FILE_CVBS_PAL);
		param.main_width = 960;
		param.main_hgith = 240;

		param.sub_width = 960;//512;
		param.sub_hight = 240;

		param.fps = 18;//30;
		break;
	default:
		return false;
	}
	return video_input_open(&param);
}

/***
** 日期: 2022-05-12 19:02
** 作者: leo.liu
** 函数作用：上电检测
** 返回参数说明：
***/

static void tp9950_format_auto_detection(void)
{
	unsigned long long  timeout_state = user_timestamp_get();
	
	#if 1
	tp9950_loss_wait();
	monitor_channel_set(MON_CH_DOOR1);
	monitor_open(false, 0x01);
	tp9950_vin1_enable(true);

	TP9950_FORMAT mode = INVALID_FORMAT;
	int timeout = 8;
	printf("=============================>> DOOR1:\n\r");
	while (timeout--)
	{
		if ((mode = tp9950_format_detection()) != INVALID_FORMAT)
		{
			tp9950_detection_printf(mode);
			monitor_valid_channel_set(MON_CH_DOOR1, true);
			break;
		}
		usleep(1 * 1000);
	}
	printf("#####################timeout_end=%llu\n ", user_timestamp_get() - timeout_state);
	tp9950_vin1_enable(false);
	tp9950_loss_wait();
	monitor_channel_set(MON_CH_DOOR2);
	monitor_open(false, 0x01);
	tp9950_vin1_enable(true);
	timeout = 8;
	printf("=============================>> DOOR2:\n\r");
	while (timeout--)
	{
		if ((mode = tp9950_format_detection()) != INVALID_FORMAT)
		{
			tp9950_detection_printf(mode);
			monitor_valid_channel_set(MON_CH_DOOR2, true);
			break;
		}
		usleep(1 * 1000);
	}
	printf("#####################timeout_end=%llu\n ", user_timestamp_get() - timeout_state);
	tp9950_vin1_enable(false);
	tp9950_loss_wait();
	monitor_channel_set(MON_CH_CCTV1);
	monitor_open(false, 0x01);
	tp9950_vin1_enable(true);
	timeout = 2;
	printf("=============================>> CCTV1:\n\r");
	while (timeout--)
	{
		if ((mode = tp9950_format_detection()) != INVALID_FORMAT)
		{
			tp9950_detection_printf(mode);
			monitor_valid_channel_set(MON_CH_CCTV1, true);
			break;
		}
		usleep(1 * 1000);
	}
	printf("#####################timeout_end=%llu\n ", user_timestamp_get() - timeout_state);
	tp9950_vin1_enable(false);
	tp9950_loss_wait();
	monitor_channel_set(MON_CH_CCTV2);
	monitor_open(false, 0x01);
	tp9950_vin1_enable(true);
	timeout = 2;
	printf("=============================>> CCTV2:\n\r");
	while (timeout--)
	{
		if ((mode = tp9950_format_detection()) != INVALID_FORMAT)
		{
			tp9950_detection_printf(mode);
			monitor_valid_channel_set(MON_CH_CCTV2, true);
			break;
		}
		usleep(1 * 1000);
	}
	tp9950_vin1_enable(false);

	monitor_close();

	printf("#####################timeout_end=%llu\n ", user_timestamp_get() - timeout_state);
#endif
	// monitor_valid_channel_set(MON_CH_DOOR1, true);
	// monitor_valid_channel_set(MON_CH_DOOR2, true);
	// monitor_valid_channel_set(MON_CH_CCTV1, true);
	// monitor_valid_channel_set(MON_CH_CCTV2, true);
}

/***
**   日期:2022-05-23 09:17:09
**   作者: leo.liu
**   函数作用：标志重新检测
**   参数说明:
***/
static bool is_tp9950_restart_det = false;
void tp9950_restart_det(void)
{
	is_tp9950_restart_det = true;
}

static int tp9950_detection_valid_cont = 0;
static TP9950_FORMAT tp9950_format = INVALID_FORMAT;
static void tp9950_and_isp_device_enable(bool en)
{
	video_input_close();
	tp9950_power_off();
	tp9950_format = INVALID_FORMAT;
	tp9950_detection_valid_cont = 0;
	if (en == true)
	{
		if (is_tp9950_restart_det == true)
		{
			is_tp9950_restart_det = false;
		}
		tp9950_pin_reset();
		tp9950_comm_init();
	}
}

//0:自动侦测
//1:设置为1080p25fps
static char tp9950_signal_detection_mode = 0; 
void tp9950_signale_detection_mode_setting(char mode){

	tp9950_signal_detection_mode = mode;
}


static bool app_playback_record_falg = false;
void app_playback_record(bool en){

	app_playback_record_falg = en;
}

/***
** 日期: 2022-05-12 16:27
** 作者: leo.liu
** 函数作用：
** 返回参数说明：
***/
#include "tuya_ipc_stream_storage.h"
#include "tuya/tuya_api.h"
extern unsigned long long  video_playback_ringbuffer_get(void);
extern void audio_playback_ringbuffer_clear(void);
extern void video_playback_ringbuffer_clear(void);
extern void h264_encode_requeset_idr(void);
static TP9950_FORMAT tuya_playback_record_check(bool tp9950_state,bool monitor_state){

	STORAGE_STATUS_E record_state = tuya_ipc_ss_get_status();
	TP9950_FORMAT fmt = tp9950_format_detection();
	unsigned long long ringbuffer = video_playback_ringbuffer_get();
	static unsigned long long record_timestamp = 0;
	unsigned long long cur = user_timestamp_get();

	if(app_playback_record_falg)
	{
		if((media_sdcard_insert_check() == true) &&(tp9950_state == true) && (monitor_state == true) /*&& (tuya_api_online_check() == true)*/)
		{
			if((record_state != E_STORAGE_START) && (record_state != E_STORAGE_ONGOING))
			{
				if((fmt != INVALID_FORMAT)&&(ringbuffer > (10*1024)) &&((cur- record_timestamp)>(3*1000))){
					
					h264_encode_requeset_idr();
					tuya_ipc_ss_start_event();
					record_timestamp = user_timestamp_get();
					// printf("######################## app playback record start #########################\n");
					return fmt;
				}
			}
		}
	}
	//else
	// {
		if((record_state == E_STORAGE_START) || (record_state == E_STORAGE_ONGOING)){
 
			if((media_sdcard_insert_check() == false)||
				(fmt == INVALID_FORMAT) ||
				(ringbuffer == 0) ||
				(tuya_api_online_check() == false)||
				(tp9950_state == false) || 
				(monitor_state == false) ||
				((cur- record_timestamp)>(90*1000) ||
				(app_playback_record_falg == false))
			){

				tuya_ipc_ss_stop_event();
				video_playback_ringbuffer_clear();
				audio_playback_ringbuffer_clear();
				record_timestamp = user_timestamp_get();
				// printf("######################## app playback record stop #########################\n");
			}

		}
	// }	
	return fmt;
}
TP9950_FORMAT video_door_format_check = INVALID_FORMAT;


static void *tp9950_task(void *arg)
{

	printf("***** tp9950 task create sccess ! *****\n");
	while (1)
	{
		bool tp9950_state = tp9950_power_get();
		bool monitor_state = is_monior_enable();
		TP9950_FORMAT fmt = tuya_playback_record_check(tp9950_state,monitor_state);

		if ((monitor_state == false) && (tp9950_state == true))
		{
			tp9950_and_isp_device_enable(false);
			usleep(1000 * 10);
			continue;
		}
		else if ((monitor_state == true) && (tp9950_state == false))
		{

			tp9950_and_isp_device_enable(true);
			usleep(1000 * 10);
			continue;
		}

		if ((monitor_state == false) || (tp9950_state == false))
		{
			usleep(1000 * 10);
			continue;
		}
		video_door_format_check = fmt;
		
		//TP9950_FORMAT fmt = INVALID_FORMAT;
		
		// if(tp9950_signal_detection_mode == 1){
			
		// 	fmt = TP2802_1080P25;
		// }
		//else{

			//fmt = tp9950_format_detection();
		//}
		if ((is_tp9950_restart_det == true) || (fmt == INVALID_FORMAT))
		{
			tp9950_and_isp_device_enable(true);
		}
		else if (tp9950_format != fmt)
		{
			/***** 信号变化，重新初始化设备后再检测 *****/
			if ((fmt == INVALID_FORMAT) || (tp9950_format != INVALID_FORMAT))
			{
				tp9950_and_isp_device_enable(true);
			}
			/***** 有信号 *****/
			else
			{
				tp9950_detection_valid_cont++;
				if (tp9950_detection_valid_cont > 5)
				{
					tp9950_detection_valid_cont = 0;
					tp9950_detection_printf(fmt);

					tp9950_write_cfg(fmt);
					if (tp9950_hvsync_wait() == true)
					{
						video_isp_core_open(fmt);
					}
					tp9950_format = fmt;
				}
			}
		}
		usleep(1000 * 10);
	}

	return NULL;
}

/***
** 日期: 2022-05-12 11:03
** 作者: leo.liu
** 函数作用：任务启动
** 返回参数说明：
***/
bool tp9950_init(void)
{
	monitor_close();
	tp9950_pin_reset();
	tp9950_comm_init();

	tp9950_format_auto_detection();

	pthread_t thread_t;
	pthread_mutex_init(&tp9950_mutex, NULL);
	pthread_create(&thread_t, user_pthread_atter_get(), tp9950_task, NULL);
	return true;
}

/***
** 日期: 2022-05-12 14:30
** 作者: leo.liu
** 函数作用：开启tp9950检测功能
** 返回参数说明：
***/
void tp9950_vin1_enable(bool en)
{
	I2C_DATA data[2];
	data[0].reg = 0x40;
	data[0].val = 0x00;

	data[1].reg = 0x41;
	/***** 不需要的时候将vin接到vin4 *****/
	data[1].val = en ? 0x0 : 0x04;
	tp9950_set_reg_page(VIDEO_PAGE);
	tp9950_write(&data[0], 2);
}
