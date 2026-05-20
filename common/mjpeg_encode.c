#include "video_encode.h"
#include <pthread.h>
#include <string.h>
#include "video_input.h"
#include <unistd.h>
#include "user_common.h"
#include "user_time.h"
#include <sys/time.h>
static int mjpeg_encode_handle_id = -1;
static bool mjpeg_encode_enable = false;
static int mjpeg_encode_count = 0;
static void (*mjpeg_encode_output_func)(struct video_stream *farme) = NULL;
static pthread_mutex_t mjpeg_encode_mutex_t;
/***
** 日期: 2022-05-18 09:19
** 作者: leo.liu
** 函数作用：关闭编码设备
** 返回参数说明：
***/
static void mjpeg_encode_device_close(void)
{
	ak_venc_close(mjpeg_encode_handle_id);
	mjpeg_encode_handle_id = -1;
}
/***
** 日期: 2022-05-18 09:19
** 作者: leo.liu
** 函数作用：开启编码设备
** 返回参数说明：
***/
static bool mjpeg_encode_device_open(void)
{
	struct venc_param ve_param;
	ve_param.width = SUB_VIDEO_PIXEL_WIDTH;
	ve_param.height = SUB_VIDEO_PIXEL_HIGHT;
	ve_param.fps = 20;		// fps set
	ve_param.goplen = 40;		// gop set
	ve_param.target_kbps = 1024;	// k bps
	ve_param.max_kbps = 2048;	// max kbps
	ve_param.br_mode = BR_MODE_VBR; // br mode
	ve_param.minqp = 25;		// qp set
	ve_param.maxqp = 45;		// qp max value
	// ve_param.initqp       = (ve_param.minqp + ve_param.maxqp) / 2;  //qp value
	ve_param.jpeg_qlevel = JPEG_QLEVEL_LOW; // jpeg qlevel
	ve_param.chroma_mode = CHROMA_4_2_0;	    // chroma mode
	ve_param.max_picture_size = 30 * 1024;	    // 0 means default
	ve_param.enc_level = 30;		    // enc level
	ve_param.smart_mode = SMART_DISABLE;	    // smart mode set
	ve_param.smart_goplen = 100;		    // smart mode value
	ve_param.smart_quality = 50;		    // quality
	ve_param.smart_static_value = 0;	    // value
	ve_param.enc_out_type = MJPEG_ENC_TYPE;	    // enc type
	/* jpeg enc profile */
	ve_param.initqp = 25;
	ve_param.profile = PROFILE_JPEG;
	if (ak_venc_open(&ve_param, &mjpeg_encode_handle_id))
	{
		printf("open venc fail \n\r");
		return false;
	}
	return true;
}
bool mjpeg_encode_input_frame_flag = false;
static void *mjpeg_encode_task(void *arg)
{
	unsigned long long pre_timestamp = 0;
	printf("***** jpg encode task create sccess ! *****\n");
	while (1)
	{
		// printf("@@@@@@@@@[%d]===[%d]@@@@@@[%d]@@\n",mjpeg_encode_enable,mjpeg_encode_handle_id,mjpeg_encode_count);
		if ((mjpeg_encode_enable == false) && (mjpeg_encode_handle_id != -1))
		{
			mjpeg_encode_device_close();
		}
		else if ((mjpeg_encode_enable == true) && (mjpeg_encode_handle_id == -1))
		{
			mjpeg_encode_device_open();
		}
		else if ((mjpeg_encode_enable == true) && (mjpeg_encode_handle_id != -1) && (mjpeg_encode_output_func != NULL))
		{
			// printf("=====================%d================================\n",__LINE__);
			unsigned long long timestamp;
			unsigned char *data = video_input_sub_resident_buffer_get(&timestamp);
			if (timestamp != pre_timestamp)
			{
				mjpeg_encode_input_frame_flag = false;
				// printf("=====================%d================================\n",__LINE__);
				pre_timestamp = timestamp;
				struct video_stream stream;
				memset(&stream, 0, sizeof(struct video_stream));
				ak_venc_encode_frame(mjpeg_encode_handle_id, data, SUB_VIDEO_PIXEL_HIGHT * SUB_VIDEO_PIXEL_WIDTH * 3 / 2, NULL, &stream);
				if (stream.len > 0)
				{
					// printf("=====================%d================================\n",__LINE__);
					pthread_mutex_lock(&mjpeg_encode_mutex_t);
					mjpeg_encode_count--;
					pthread_mutex_unlock(&mjpeg_encode_mutex_t);
					mjpeg_encode_output_func(&stream);

				}
				ak_venc_release_stream(mjpeg_encode_handle_id, &stream);
			}
			else
			{
				mjpeg_encode_input_frame_flag = true;
			}
		}
		usleep(10 * 1000);
	}
	return NULL;
}

/***
** 日期: 2022-05-18 08:36
** 作者: leo.liu
** 函数作用：设备初始化
** 返回参数说明：
***/
bool mjpeg_encode_init(void)
{
	pthread_t thread_t;
	pthread_mutex_init(&mjpeg_encode_mutex_t,NULL);
	pthread_create(&thread_t, user_pthread_atter_get(), mjpeg_encode_task, NULL);
	return true;
}

/***
** 日期: 2022-05-18 08:36
** 作者: leo.liu
** 函数作用：开启设备
** 返回参数说明：
***/
bool mjpeg_encode_open(void (*callback)(struct video_stream *farme))
{
	// if (mjpeg_encode_enable == true)
	// {
	// 	return false;
	// }
	mjpeg_encode_input_frame_flag = true;
	pthread_mutex_lock(&mjpeg_encode_mutex_t);
	mjpeg_encode_count++;
	pthread_mutex_unlock(&mjpeg_encode_mutex_t);
	mjpeg_encode_output_func = callback;
	mjpeg_encode_enable = true;
	// printf("###########[%llu]===[%d]====\n",user_timestamp_get(),__LINE__);
	return true;
}

/***
** 日期: 2022-05-18 08:38
** 作者: leo.liu
** 函数作用：关闭编码
** 返回参数说明：
***/
bool mjpeg_encode_close(void)
{
	if (mjpeg_encode_enable == false)
	{
		return true;
	}
	printf("#####################mjpeg_encode_count==[%d]#\n",mjpeg_encode_count);
	pthread_mutex_lock(&mjpeg_encode_mutex_t);
	if(mjpeg_encode_count !=0)
	{
		pthread_mutex_unlock(&mjpeg_encode_mutex_t);
		return true;
	}
	pthread_mutex_unlock(&mjpeg_encode_mutex_t);
	mjpeg_encode_enable = false;
	// printf("###########[%llu]===[%d]====\n",user_timestamp_get(),__LINE__);
	return true;
}

/***
** 日期: 2022-05-19 09:50
** 作者: leo.liu
** 函数作用：编码状态
** 返回参数说明：
***/
bool mjpeg_encode_status_get(void)
{
	return ((mjpeg_encode_handle_id != -1) || (mjpeg_encode_enable == true));
}
