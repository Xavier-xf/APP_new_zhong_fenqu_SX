#include "video_encode.h"
#include <pthread.h>
#include "ak_ai.h"
#include "string.h"
#include <unistd.h>
#include "user_time.h"
#include "video_input.h"
#include "tuya/tuya_api.h"
#include "tuya_ring_buffer.h"
#include "user_common.h"
#include "rom.h"
#include "user_file.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya/tuya_g711_utils.h"
#include "audio_input.h"

int tuya_online_num_get_customize(void);
bool tuya_app_preview_check(void);

bool video_record_video_write(unsigned char *data, int size);

static char h264_encode_mask = 0x00;
static int h264_encode_handle_id = -1;
static pthread_mutex_t h264_encode_mutex;
static bool h264_encode_capture = false;
static int skip_frame_count = 0;
static bool h264_enable_sample_enable = false;

static unsigned long long video_playback_ringbuffer_post_size = 0;
void video_playback_ringbuffer_add(int size)
{

	video_playback_ringbuffer_post_size += size;
}
unsigned long long video_playback_ringbuffer_get(void)
{

	return video_playback_ringbuffer_post_size;
}
void video_playback_ringbuffer_clear(void)
{

	video_playback_ringbuffer_post_size = 0;
}

/***
** 日期: 2022-05-19 11:46
** 作者: leo.liu
** 函数作用：开启音频采集
** 返回参数说明：
***/
static void h264_encode_device_open(void)
{
#if 1
	struct venc_param ve_param;
	ve_param.width = SUB_VIDEO_PIXEL_WIDTH;
	ve_param.height = SUB_VIDEO_PIXEL_HIGHT;
	ve_param.fps = 25;										 // fps set
	ve_param.goplen = 50;									 // gop set
	ve_param.target_kbps = 1024;//800;								 // 800;		 //k bps
	ve_param.max_kbps = 2048;//1024;								 // 1024;		 //max kbps
	ve_param.br_mode = BR_MODE_CBR;							 // BR_MODE_VBR;//BR_MODE_CBR; //br mode
	ve_param.minqp = 25;									 // qp set
	ve_param.maxqp = 50;									 // qp max value
	ve_param.initqp = (ve_param.minqp + ve_param.maxqp) / 2; // qp value
	ve_param.jpeg_qlevel = JPEG_QLEVEL_DEFAULT;				 // jpeg qlevel
	ve_param.chroma_mode = CHROMA_4_2_0;					 // chroma mode
	ve_param.max_picture_size = 0;							 // 0 means default
	ve_param.enc_level = 50;								 // 50;				 //enc level
	ve_param.smart_mode = 0;								 // smart mode set
	ve_param.smart_goplen = 100;							 // smart mode value
	ve_param.smart_quality = 50;							 // quality
	ve_param.smart_static_value = 0;						 // value
	ve_param.enc_out_type = H264_ENC_TYPE;					 // enc type
	ve_param.profile = PROFILE_MAIN;
#else
	struct venc_param ve_param;
	ve_param.width = SUB_VIDEO_PIXEL_WIDTH;
	ve_param.height = SUB_VIDEO_PIXEL_HIGHT;
	ve_param.fps = 20;				// fps set
	ve_param.goplen = 40;			// gop set
	ve_param.target_kbps = 1024;	// k bps
	ve_param.max_kbps = 2048;		// max kbps
	ve_param.br_mode = BR_MODE_VBR; // br mode
	ve_param.minqp = 25;			// qp set
	ve_param.maxqp = 50;			// qp max value
	// ve_param.initqp       = (ve_param.minqp + ve_param.maxqp) / 2;  //qp value
	ve_param.jpeg_qlevel = JPEG_QLEVEL_DEFAULT; // jpeg qlevel
	ve_param.chroma_mode = CHROMA_4_2_0;		// chroma mode
	ve_param.max_picture_size = 30 * 1024;		// 0 means default
	ve_param.enc_level = 30;					// enc level
	ve_param.smart_mode = SMART_DISABLE;		// smart mode set
	ve_param.smart_goplen = 100;				// smart mode value
	ve_param.smart_quality = 50;				// quality
	ve_param.smart_static_value = 0;			// value
	ve_param.enc_out_type = MJPEG_ENC_TYPE;		// enc type
	/* jpeg enc profile */
	ve_param.initqp = 50;
	ve_param.profile = PROFILE_JPEG;
#endif
	ak_venc_open(&ve_param, &h264_encode_handle_id);
}

static void h264_encode_device_close(void)
{
	printf("h264_encode_device_close\n");
	ak_venc_close(h264_encode_handle_id);
	h264_encode_handle_id = -1;

	h264_enable_sample_enable = false;
}
/***
** 日期: 2022-05-19 11:34
** 作者: leo.liu
** 函数作用：音频采集线程
** 返回参数说明：
***/
static void *h264_encode_task(void *arg)
{
	struct video_stream vstream;
	unsigned long long pre_timestamp = 0;
	printf("***** h264 encode task create sccess ! *****\n");

	h264_encode_capture = true;
	h264_encode_mask = 0x02;

	// int app_preview_delay = 0;

	bool is_smaple_frame_pre = false;
	int smaple_frame_count_delay = 0;

	while (1)
	{
		pthread_mutex_lock(&h264_encode_mutex);
		if ((h264_encode_capture == false) && (h264_encode_handle_id != -1))
		{
			h264_encode_device_close();

			is_smaple_frame_pre = false;
			smaple_frame_count_delay = 0;
			printf("### h264 encode close success ! ###\n");
		}
		else if ((h264_encode_capture == true) && (h264_encode_handle_id == -1))
		{
			h264_encode_device_open();
			skip_frame_count = skip_frame_count == 0 ? 5 : skip_frame_count;

			is_smaple_frame_pre = false;
			smaple_frame_count_delay = 0;
		}

		else if ((h264_encode_capture == true) && (h264_encode_mask) && (h264_encode_handle_id != -1))
		{
			unsigned long long timestamp;
			video_input_sub_buffer_busy_enable(true);
			unsigned char *data = video_input_sub_resident_buffer_get(&timestamp);
			if (timestamp != pre_timestamp)
			{
				pre_timestamp = timestamp;
				if (ak_venc_encode_frame(h264_encode_handle_id, data, SUB_VIDEO_PIXEL_WIDTH * SUB_VIDEO_PIXEL_HIGHT * 3 / 2, NULL, &vstream) == 0)
				{
					video_input_sub_buffer_busy_enable(false);
					if (skip_frame_count > 0)
					{
						skip_frame_count--;
						if (skip_frame_count == 0)
						{
							ak_venc_request_idr(h264_encode_handle_id);
						}
						is_smaple_frame_pre = false;
						smaple_frame_count_delay = 0;
					}
					else
					{
						if (h264_encode_mask & 0x01)
						{
							video_record_video_write(vstream.data, vstream.len);
						}
						if (1) // (h264_encode_mask & 0x02)
						{
							if (tuya_api_online_check() == true)
							{
								if ((h264_enable_sample_enable == false) || (tuya_api_client_num() == 0))
								{

									if (1) //(((video_playback_ringbuffer_get()) || ((video_playback_ringbuffer_get() == 0)&&(vstream.frame_type == FRAME_TYPE_I))))
									{
										// printf("##################device_send_video_to_tuya################\n");
										tuya_ipc_ring_buffer_append_data(E_CHANNEL_VIDEO_MAIN, vstream.data, vstream.len, vstream.frame_type == FRAME_TYPE_I ? E_VIDEO_I_FRAME : E_VIDEO_PB_FRAME, user_timestamp_get());

										video_playback_ringbuffer_add(vstream.len);

										is_smaple_frame_pre = false;
										smaple_frame_count_delay = 0;
										if (audio_input_state_get() == false)
										{

											unsigned char data1[640] = {0};
											unsigned char dst_data[320] = {0};
											unsigned int dst_data_len = 0;
											tuya_g711_encode(1, (unsigned short *)data1, sizeof(data1), dst_data, &dst_data_len);
											tuya_ipc_ring_buffer_append_data(E_CHANNEL_AUDIO, dst_data, dst_data_len, E_AUDIO_FRAME, user_timestamp_get());
										}
									}
								}
								else if ((is_smaple_frame_pre == false) /*&&(tuya_online_num_get_customize() > 0)*/)
								{
									smaple_frame_count_delay++;
									if (smaple_frame_count_delay < 10)
									{

										extern const unsigned char *get_rom_bin_base(void);
										unsigned char *data = (unsigned char *)(get_rom_bin_base() + ROM_UI_YUV420SP_TO_H264_H264);
										int size = ROM_UI_YUV420SP_TO_H264_H264_SIZE;
										tuya_ipc_ring_buffer_append_data(E_CHANNEL_VIDEO_MAIN, data, size, E_VIDEO_I_FRAME, user_timestamp_get());
										// unsigned char data1[1024] = {0};
										// unsigned char dst_data[512] = {0};
										// unsigned int dst_data_len = 0;
										// tuya_g711_encode(1, (unsigned short *)data1, sizeof(data1), dst_data, &dst_data_len);
										// tuya_ipc_ring_buffer_append_data(E_CHANNEL_AUDIO, dst_data, dst_data_len, E_AUDIO_FRAME, user_timestamp_get());
										video_playback_ringbuffer_clear();
									}
									else
									{

										smaple_frame_count_delay = 0;
										is_smaple_frame_pre = true;
									}
								}
								static int tuya_online_num = 0;
								if (tuya_online_num_get_customize() != tuya_online_num)
								{

									tuya_online_num = tuya_online_num_get_customize();
									smaple_frame_count_delay = 0;
									is_smaple_frame_pre = false;
								}
							}
						}
					}
					ak_venc_release_stream(h264_encode_handle_id, &vstream);
				}
			} /*
			 else if((tuya_app_preview_check()  == false)&&(app_preview_delay != 0)){

				 app_preview_delay = 0;
			 }
			 */
			video_input_sub_buffer_busy_enable(false);
		}
		pthread_mutex_unlock(&h264_encode_mutex);
		usleep(10 * 1000);
	}
	return NULL;
}

/***
** 日期: 2022-05-19 11:32
** 作者: leo.liu
** 函数作用：音频采集设备
** 返回参数说明：
***/
bool h264_encode_init(void)
{
	pthread_t thread_t;
	pthread_mutex_init(&h264_encode_mutex, NULL);
	pthread_create(&thread_t, user_pthread_atter_get(), h264_encode_task, NULL);
	return true;
}

/***
** 日期: 2022-05-19 11:33
** 作者: leo.liu
** 函数作用：音频打开采集
** 返回参数说明：
***/
bool h264_encode_open(unsigned char mask)
{
	pthread_mutex_lock(&h264_encode_mutex);
	if (h264_encode_mask & mask)
	{
		pthread_mutex_unlock(&h264_encode_mutex);
		return false;
	}
	h264_encode_mask |= mask;
	pthread_mutex_unlock(&h264_encode_mutex);
	return true;
}
/***
** 日期: 2022-05-19 11:34
** 作者: leo.liu
** 函数作用：音频关闭
** 返回参数说明：
***/
bool h264_encode_close(unsigned char mask)
{
	pthread_mutex_lock(&h264_encode_mutex);
	if ((h264_encode_mask & mask) == 0x00)
	{
		pthread_mutex_unlock(&h264_encode_mutex);
		return true;
	}
	if (mask != 0x02)
	{
		h264_encode_mask &= (~mask);
	}
	pthread_mutex_unlock(&h264_encode_mutex);
	return true;
}
/***
**   日期:2022-06-08 08:17:53
**   作者: leo.liu
**   函数作用：设置过滤前面的n帧
**   参数说明:
***/
bool h264_encode_skip_frame(int n)
{
	pthread_mutex_lock(&h264_encode_mutex);
	if (h264_encode_mask == 0x00)
	{
		pthread_mutex_unlock(&h264_encode_mutex);
		return true;
	}
	skip_frame_count = n;
	pthread_mutex_unlock(&h264_encode_mutex);
	return true;
}
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：编码采集开关
**   参数说明:
***/
void h264_encode_capture_enable(bool en)
{
	return;
	pthread_mutex_lock(&h264_encode_mutex);
	h264_encode_capture = en;
	pthread_mutex_unlock(&h264_encode_mutex);
}
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：采集樣品数据流
**   参数说明:
***/
void h264_encode_sample_enable(bool en)
{
	pthread_mutex_lock(&h264_encode_mutex);
	h264_enable_sample_enable = en;
	pthread_mutex_unlock(&h264_encode_mutex);
}
/***
**   日期:2022-05-26 08:39:18
**   作者: fei.liu
**   函数作用：获取样品流参数
**   参数说明:
***/
bool h264_encode_sample_get(void)
{
	pthread_mutex_lock(&h264_encode_mutex);
	bool en = h264_enable_sample_enable;
	pthread_mutex_unlock(&h264_encode_mutex);
	return en;
}

void h264_encode_fouce_close(void)
{
	pthread_mutex_lock(&h264_encode_mutex);
	if (h264_encode_handle_id != -1)
	{
		h264_encode_device_close();
	}
	pthread_mutex_unlock(&h264_encode_mutex);
}

void h264_encode_fouce_open(void)
{
	pthread_mutex_lock(&h264_encode_mutex);
	if (h264_encode_handle_id == -1)
	{
		h264_encode_device_open();
		skip_frame_count = skip_frame_count == 0 ? 5 : skip_frame_count;
	}
	pthread_mutex_unlock(&h264_encode_mutex);
}
void h264_encode_requeset_idr(void)
{
	pthread_mutex_lock(&h264_encode_mutex);
	if (h264_encode_handle_id != -1)
	{

		ak_venc_request_idr(h264_encode_handle_id);
	}
	pthread_mutex_unlock(&h264_encode_mutex);
}