#include "user_monitor.h"
#include "user_gpio.h"
#include "gsm6502.h"
#include "tp9950.h"
#include "video_input.h"
#include "unistd.h"
#include "video_decode.h"
#include "user_intercom.h"
#include "lv_msg_event.h"
#include "layout_define.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya/tuya_api.h"
#include "wifi_api.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ak_thread.h>

bool layout_monitor_report_vaild_channel(void);
/***
** 日期: 2022-05-12 15:17
** 作者: leo.liu
** 函数作用：监控状态
** 返回参数说明：
***/
static bool monitor_enable = false;
bool is_monior_enable(void)
{
	return monitor_enable;
}

static bool monitor_valid_ch[MON_CH_TOTAL] = {false};

static MON_CH montor_channel = MON_CH_NONE;
/***
** 日期: 2022-05-12 11:32
** 作者: leo.liu
** 函数作用：设置监控通道
** 返回参数说明：
***/
bool monitor_channel_set(MON_CH ch)
{
	montor_channel = ch;
	return true;
}

/***
** 日期: 2022-05-12 11:33
** 作者: leo.liu
** 函数作用：获取监控通道
** 返回参数说明：
***/
MON_CH monitor_channel_get(void)
{
	return montor_channel;
}

/***
** 日期: 2022-05-12 11:17
** 作者: leo.liu
** 函数作用：打开户外机电源
** 返回参数说明：
***/
static void outdoor_power_enable(MON_CH ch, bool en)
{
	switch (ch)
	{
	case MON_CH_DOOR1:
		door1_power_enable(en);
		door2_power_enable(false);
		break;
	case MON_CH_DOOR2:
		door2_power_enable(en);
		door1_power_enable(false);
		break;
	default:
		door2_power_enable(false);
		door1_power_enable(false);
		break;
	}
}

/***
** 日期: 2022-05-12 11:23
** 作者: leo.liu
** 函数作用：开启模拟视频流向
** 返回参数说明：
** flg:0x01:only ch,0x02:obly inter, 0x03:ch and inter
***/
#define SWITCH_DATA_SET(data, flg, out1, out2, in) \
	data[out1][in] = (flg & 0x01) ? 1 : 0;         \
	data[out2][in] = (flg & 0x02) ? 1 : 0;

void video_switch_enable(MON_CH ch, char flg)
{
	unsigned char data[6][8] = {{0}, {0}};
	if (OwnID != 1)
	{
		SWITCH_DATA_SET(data, 0x03, OUT_BT601, OUT_INT_BT601, IN_IN1_DOOR);
	}
	else
	{
		if (ch == MON_CH_DOOR1)
		{
			SWITCH_DATA_SET(data, flg, OUT_BT601, OUT_INT_BT601, IN_DOOR1);
		}
		else if (ch == MON_CH_DOOR2)
		{
			SWITCH_DATA_SET(data, flg, OUT_BT601, OUT_INT_BT601, IN_DOOR2);
		}
		else if (ch == MON_CH_CCTV1)
		{
			SWITCH_DATA_SET(data, flg, OUT_BT601, OUT_INT_BT601, IN_CCTV1);
		}
		else if (ch == MON_CH_CCTV2)
		{
			SWITCH_DATA_SET(data, flg, OUT_BT601, OUT_INT_BT601, IN_CCTV2);
		}
		else if (ch == MON_CH_INTERCOM)
		{
			SWITCH_DATA_SET(data, flg, OUT_BT601, OUT_INT_BT601, IN_IN1_DOOR);
		}
	}
	gsm6502_open(data);
}
/***
**   日期:2022-06-01 17:21:28
**   作者: leo.liu
**   函数作用：关闭模拟视频通道
**   参数说明:
***/
void video_switch_disable(void)
{
	gsm6502_close();
}
/***
** 日期: 2022-05-12 11:34
** 作者: leo.liu
** 函数作用：开启监控
** 返回参数说明：mask:0x01:indoor, 0x02:intercom
***/
bool monitor_open(bool preview, unsigned char mask)
{

	printf("======%s=========\n", __func__);
	if (montor_channel == MON_CH_NONE)
	{
		return false;
	}

	monitor_close();

	if (montor_channel == MON_CH_CCTV3 || montor_channel == MON_CH_CCTV4)
	{
		rtsp_decode_open(user_data_get()->onvif_dev[montor_channel - MON_CH_CCTV3].url[2]);
		video_display_preview_enable(preview);
	}
	else
	{
		video_switch_enable(montor_channel, mask);
		if (OwnID == 1)
		{
			outdoor_power_enable(montor_channel, true);
		}

		if (mask & 0x01)
		{
			printf("====mask==is ox %x===", mask);
			// tp9950_signale_detection_mode_setting(((montor_channel == MON_CH_DOOR1)||(montor_channel == MON_CH_DOOR2))?1:0);
			video_display_preview_enable(preview);
			tp9950_vin1_enable(true);
			tp9950_restart_det();
		}
		monitor_enable = true;
	}
	layout_monitor_report_vaild_channel();

	return true;
}
/***
**   日期:2022-06-11 11:30:03
**   作者: leo.liu
**   函数作用：设置监控位置
**   参数说明:
***/
bool monitor_preivew_pos_set(int x, int y, int w, int h)
{
	return video_input_display_pos(x, y, w, h);
}

/***
** 日期: 2022-05-12 11:36
** 作者: leo.liu
** 函数作用：关闭监控
** 返回参数说明：
***/
extern void audio_playback_ringbuffer_clear(void);
extern void video_playback_ringbuffer_clear(void);
bool monitor_close(void)
{
	printf("======%s=========\n", __func__);
	video_switch_disable();
	if (cur_layout_get() != pLAYOUT(video))
	{
		video_display_preview_enable(false);
	}
	// video_display_preview_enable(false);
	video_input_skip_frame_count_set(1000);
	tp9950_vin1_enable(false);
	outdoor_power_enable(montor_channel, false);

	video_playback_ringbuffer_clear();
	audio_playback_ringbuffer_clear();
	// tp9950_signale_detection_mode_setting(0);
	rtsp_decode_close();
	monitor_enable = false;
	return true;
}

/***
** 日期: 2022-05-16 16:25
** 作者: leo.liu
** 函数作用：判断此通道时候有信号
** 返回参数说明：
***/
bool monitor_valid_channel_check(MON_CH channel)
{
	return monitor_valid_ch[channel];
}

/***
** 日期: 2022-05-16 16:38
** 作者: leo.liu
** 函数作用：设置通道有效
** 返回参数说明：
***/
void monitor_valid_channel_set(MON_CH ch, bool sate)
{
	monitor_valid_ch[ch] = sate;
}

#include "user_common.h"
#include "media_thumb.h"
#include "video_decode.h"
#include "avilib.h"
#include "ak_mem.h"
#include "user_file.h"
#include "ak_common_graphics.h"
#include "ak_tde.h"
#include "video_input.h"
extern int video_input_skip_frame_count;
extern bool mjpeg_encode_input_frame_flag;
int rtsp_input_fps = 0;
static void rtsp_decode_cb_func(struct ak_vdec_frame *frame)
{
	static unsigned long long prev_timestamp = 0;
	unsigned long long curr_timestamp = user_timestamp_get();
	if (curr_timestamp - prev_timestamp < 50)
	{
		return;
	}
	prev_timestamp = curr_timestamp;

	if (video_input_skip_frame_count)
	{
		lv_video_mode_enable(false);
		video_input_skip_frame_count--;
	}
	else if (RTSP_CLIENT_STATE == RTSP_CLIENT_SUCCESS)
	{
		lv_video_mode_enable(true);
		video_main_display_lock();
		video_input_resident_buffer_write(frame->frame_obj.data.data, frame->frame_obj.data.pitch_width, frame->frame_obj.data.pitch_height, 0, 0, frame->width, frame->height, GP_FORMAT_YUV420SP);
		video_main_display_unlock();
		screen_force_refresh();
		extern void onvif_sub_display_frame_write(struct ak_vdec_frame * frame);
		if (mjpeg_encode_input_frame_flag /* || video_record_status_get() */)
		{
			mjpeg_encode_input_frame_flag = false;
			onvif_sub_display_frame_write(frame);
			printf("mjpeg encode frame: [%dx%d]\n", frame->width, frame->height);
		}
		// printf(" decode frame: %dx%d \n", frame->width, frame->height);
	}
}

static bool rtsp_open_flag = false;
void rtsp_decode_open(char *url)
{
	int time_out = 200;
	printf("==========>> \n");
	while (video_input_state_get())
	{
		usleep(10 * 1000);
		if (time_out-- <= 0)
			break;
	}
	printf("==========>> \n");
	extern bool video_input_device_state_get(void);
	time_out = 200;
	while (video_input_device_state_get())
	{
		usleep(10 * 1000);
		if (time_out-- <= 0)
			break;
	}
	printf("==========>> \n");
	video_display_preview_enable(true);
	h264_decode_open(rtsp_decode_cb_func);
	printf("==========>> \n");
	time_out = 200;
	while (h264_decode_device_state() == false)
	{
		usleep(10 * 1000);
		if (time_out-- <= 0)
			break;
	}
	printf("==========>> \n");
	rtsp_stream_open(url);
	printf("==========>> \n");
	rtsp_open_flag = true;
}

void rtsp_decode_close(void)
{
	if (rtsp_open_flag == true)
	{
		int time_out = 200;
		printf("==========>> \n");
		rtsp_open_flag = false;
		video_display_preview_enable(false);
		rtsp_stream_close();
		printf("==========>> \n");
		time_out = 200;
		while (RTSP_CLIENT_STATE == RTSP_CLIENT_SUCCESS)
		{
			usleep(10 * 1000);
			if (time_out-- <= 0)
				break;
		}
		printf("==========>> \n");
		h264_decode_close();
		printf("==========>> \n");
		time_out = 200;
		while (h264_decode_device_state())
		{
			usleep(10 * 1000);
			if (time_out-- <= 0)
				break;
		}
		printf("==========>> \n");
		lv_video_mode_enable(false);
	}
}

bool rtsp_video_input_state_get(void)
{
	if (montor_channel == MON_CH_CCTV3 || montor_channel == MON_CH_CCTV4)
	{
		if (RTSP_CLIENT_STATE == RTSP_CLIENT_SUCCESS)
			return true;
	}
	return false;
}

/**
 * @brief  tcp socket 检测ip是否在线
 * @date   2023-09-06 09:58:00
 * @author xiaole
 * @param  ip
 * @param  port
 * @param  timeout	单位[ms]
 * @return int
 * @note
 */
bool socket_connect_ip_test(const char *ip, int port, int timeout)
{
	// 创建TCP连接套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("socket\n");
		return false;
	}

	bool ret = false;

	// 设置超时时间
	struct timeval timeout_val;
	timeout_val.tv_sec = timeout / 1000;
	timeout_val.tv_usec = timeout % 1000 * 1000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout_val, sizeof(timeout_val)) == -1)
	{
		printf("setsockopt\n");
		ret = false;
		goto EXIT;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout_val, sizeof(timeout_val)) == -1)
	{
		printf("setsockopt\n");
		ret = false;
		goto EXIT;
	}

	// 设置服务器地址信息
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &server_addr.sin_addr);

	// 连接服务器
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		// printf("connect() error\n");
		ret = false;
		goto EXIT;
	}
	else
	{
		// LOG_WHITE("IP is online.\n");
		ret = true;
		goto EXIT;
	}

EXIT:
	close(sockfd);
	return ret;
}

static void *cctv_online_check_thread(void *arg)
{
	ak_thread_set_name(__FUNCTION__); // 设置线程名字
	int ret = 0;

	LOG_WHITE("************ cctv_online_check thread [ok] ***********\n");
	while (1)
	{
		if (OwnID == 1)
		{
			for (int i = 0; i < 2; i++)
			{
				ret = 0;

				/* 使用 socket  */
				if (strlen(user_data_get()->onvif_dev[i].ip) != 0)
				{

					/* 只需要一个方案正确即可 */
					if (network_ping_test(user_data_get()->onvif_dev[i].ip) ||
						socket_connect_ip_test(user_data_get()->onvif_dev[i].ip, user_data_get()->onvif_dev[i].port, 1000) ||
						socket_connect_ip_test(user_data_get()->onvif_dev[i].ip, 80, 1000) ||
						socket_connect_ip_test(user_data_get()->onvif_dev[i].ip, 554, 1000) ||
						socket_connect_ip_test(user_data_get()->onvif_dev[i].ip, 8080, 1000))
					{

						ret = 1;
					}
					else
					{
						ret = 0;
					}

					// LOG_WHITE("ip=%s, ret=%d\n", user_data_get()->onvif_dev[i].ip, ret);

					// if ((network_device_info_get(OWNFLOOR, DEVICE_CCTV_1 + i)->online != ret))
					// {
					// 	device_online_state_push(DEVICE_CCTV_1 + i, ret ? 0x01 : 0x00);
					// }

					// network_device_info_get(OWNFLOOR, DEVICE_CCTV_1 + i)->online = ret;
					monitor_valid_channel_set(MON_CH_CCTV3 + i, ret);
				}
				else
				{

					// if ((network_device_info_get(OWNFLOOR, DEVICE_CCTV_1 + i)->online == true))
					// {
					// 	device_online_state_push(DEVICE_CCTV_1 + i, 0x00);
					// }
					monitor_valid_channel_set(MON_CH_CCTV3 + i, false);
				}
			}
		}

		usleep(1000 * 1000 * 10);
	}

	ak_thread_exit();
	return NULL;
}

void cctv_online_check_thread_init(void)
{
	ak_pthread_t thread_id;
	ak_thread_create(&thread_id, cctv_online_check_thread, NULL, ANYKA_THREAD_MIN_STACK_SIZE, -1);
	ak_thread_detach(thread_id);
}