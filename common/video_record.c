#include "video_record.h"
#include "audio_input.h"
#include "user_common.h"
#include <pthread.h>
#include "avilib.h"
#include "user_time.h"
#include "video_input.h"
#include "user_file.h"
#include "ak_mem.h"
#include "video_encode.h"
#include "audio_output.h"
#include "user_common.h"
static bool video_record_enable = false;
static bool video_has_audio = false;

static int video_record_queue_head = -1;
static void (*video_record_finish_func)(const char *path) = NULL;
static avi_t *video_record_handle_id = NULL;
unsigned long long avi_write_frame_duration = 0;

#define VIDEO_TEMP_PATH SD_MEDIA_PATH "videotemp" VIDEO_DOT

/***
** 日期: 2022-05-19 14:46
** 作者: leo.liu
** 函数作用：更新头部信息
** 返回参数说明：
***/
static void avi_write_head(avi_t *handle, double fps, int width, int height, bool has_audio, char h264)
{
	AVI_set_video(handle, width, height, fps, h264 == 0 ? "H264" : "MJPG");
	if (has_audio == true)
	{
		AVI_set_audio(handle, 1, 8000, 16, 1, 128);
	}
	avi_update_header(handle);
}

/***
** 日期: 2022-05-19 14:09
** 作者: leo.liu
** 函数作用：打开avi录制
** 返回参数说明：
***/
static bool video_record_device_open(void)
{
	video_record_handle_id = AVI_open_output_file(VIDEO_TEMP_PATH);
	if (video_record_handle_id == 0)
	{
		return false;
	}
	avi_write_head(video_record_handle_id, 25, SUB_VIDEO_PIXEL_WIDTH, SUB_VIDEO_PIXEL_HIGHT, video_has_audio, 0);
	return true;
}
/***
** 日期: 2022-05-19 14:10
** 作者: leo.liu
** 函数作用：关闭avi录制
** 返回参数说明：
***/
static bool video_record_device_close(unsigned long long duration, unsigned int frame_total)
{
	bool reslut = true;
	duration = user_timestamp_get() - duration;
	video_record_handle_id->fps = 1000 * frame_total / (duration + 0.001);
	printf("\n\n\nEncode to AVI Finish. video frame:%0.2lffps drution:%2llus \n\n\n\n", video_record_handle_id->fps, duration / 1000);
	if ((video_record_handle_id->fps < 10) || ((duration / 1000) < 3))
	{
		reslut = false;
	}
	AVI_close(video_record_handle_id);
	video_record_handle_id = NULL;
	return reslut;
}

static bool is_h264_keyframe(unsigned char data)
{
	data &= 0x1F;
	if ((data == 0x07) || (data == 0x08) || (data == 0x05))
	{
		return true;
	}
	return false;
}
/***
** 日期: 2022-05-19 14:50
** 作者: leo.liu
** 函数作用：写入avi第一帧视频帧
** 返回参数说明：
***/
static bool avi_write_the_first_frame(common_data *node)
{
	if (node->type == 0x00)
	{
		return false;
	}

	if (is_h264_keyframe(node->data[4]) == true)
	{
		printf("\n\n\nencode avi first frame start \n\n\n");
		AVI_write_frame(video_record_handle_id, (char *)node->data, node->size, 1);
		return true;
	}

	return false;
}

/***
** 日期: 2022-05-19 13:45
** 作者: leo.liu
** 函数作用：视频录制线程
** 返回参数说明：
***/
static void *video_record_task(void *arg)
{
	common_queue node;
	while (msgrcv(video_record_queue_head, (void *)&node, sizeof(common_data), 0, IPC_NOWAIT) > 0)
		;

	unsigned int avi_write_frame_count = 0;

	common_data *audio_node[10] = {NULL};
	common_data *video_node[10] = {NULL};
	int audio_node_total = 0;
	int video_node_total = 0;
	printf("***** video record task create sccess ! *****\n");
	while (1)
	{
		if ((video_record_enable == true) && (video_record_handle_id == NULL))
		{
			video_record_device_open();
			avi_write_frame_count = 0;
			avi_write_frame_duration = 0;
		}
		else if ((video_record_enable == false) && (video_record_handle_id != NULL))
		{
			if ((video_record_device_close(avi_write_frame_duration, avi_write_frame_count) == true) && (video_record_finish_func != NULL))
			{
				video_record_finish_func(VIDEO_TEMP_PATH);
			}
			for (int i = 0; i < 10; i++)
			{
				if ((audio_node[i] != NULL) && (audio_node[i]->data != NULL))
				{
					ak_mem_free(audio_node[i]->data);
					audio_node[i] = NULL;
				}
				if ((video_node[i] != NULL) && (video_node[i]->data != NULL))
				{
					ak_mem_free(video_node[i]->data);
					video_node[i] = NULL;
				}
			}
		}

		if ((msgrcv(video_record_queue_head, (void *)&node, sizeof(common_data), sizeof(common_data), IPC_NOWAIT) > 0))
		{
			if ((video_record_enable == false) || (video_record_handle_id == NULL) || (media_sdcard_insert_check() == false))
			{
				ak_mem_free(node.msg.data);
				continue;
			}
			if (avi_write_frame_count == 0)
			{
				if (avi_write_the_first_frame(&node.msg) == true)
				{
					avi_write_frame_count = 1;
					avi_write_frame_duration = user_timestamp_get();
				}
				ak_mem_free(node.msg.data);
			}
			else
			{
				if (node.msg.type == 0)
				{
					if (audio_node[audio_node_total] == NULL)
					{
						if (audio_node_total < 9)
						{
							audio_node[audio_node_total] = &(node.msg);
							audio_node_total++;
						}
						else
						{
							printf("record audio data full \n");
						}
					}
				}
				else if (node.msg.type == 1)
				{
					if (video_node[video_node_total] == NULL)
					{
						if (video_node_total < 9)
						{
							video_node[video_node_total] = &(node.msg);
							video_node_total++;
						}
						else
						{
							printf("record video data full \n");
						}
					}
				}

				if ((audio_node[0] != NULL) && (((video_node[0] == NULL) || (video_node[0]->timestamp > audio_node[0]->timestamp))))
				{
					AVI_write_audio(video_record_handle_id, (char *)audio_node[0]->data, audio_node[0]->size);
					ak_mem_free(audio_node[0]->data);
					if (audio_node_total > 1)
					{
						memmove(&audio_node[0], &audio_node[1], sizeof(common_data)*(audio_node_total - 1));
					}
					audio_node_total--;
					audio_node[audio_node_total] = NULL;
				}
				else if ((video_node[0] != NULL) && ((audio_node[0] == NULL) || (video_node[0]->timestamp <= audio_node[0]->timestamp)))
				{
					AVI_write_frame(video_record_handle_id, (char *)video_node[0]->data, video_node[0]->size, is_h264_keyframe(video_node[0]->data[4]));
					ak_mem_free(video_node[0]->data);
					if (video_node_total > 1)
					{
						memmove(&video_node[0], &video_node[1], sizeof(common_data)*(video_node_total - 1));
					}
					video_node_total--;
					video_node[video_node_total] = NULL;

					avi_write_frame_count++;
				}
			}
		}
		usleep(5 * 1000);
	}

	return NULL;
}

/***
** 日期: 2022-05-19 13:40
** 作者: leo.liu
** 函数作用：记录视频设备初始化
** 返回参数说明：
***/
bool video_record_init(void)
{
	pthread_t thread_t;
	video_record_queue_head = msg_queue_create();
	pthread_create(&thread_t, user_pthread_atter_get(), video_record_task, NULL);
	return true;
}

/***
** 日期: 2022-05-19 13:41
** 作者: leo.liu
** 函数作用：开启记录视频设备
** 返回参数说明：
***/
bool video_record_start(bool has_audio, void (*finish_callback)(const char *path))
{
	if (video_record_enable == true)
	{
		return false;
	}
	h264_encode_open(0x01);
	if (has_audio == true)
	{
		// printf("audio output query wait ");
		// audio_output_buffer_try_timeout(1500);
		// printf("finish \n");
		// audio_output_open(AUDIO_CHANNEL_MONO, AK_AUDIO_SAMPLE_RATE_8000);
		//	audio_output_close();
		audio_input_open(0x01);
	}
	video_record_finish_func = finish_callback;
	video_has_audio = has_audio;
	video_record_enable = true;
	return true;
}
/***
** 日期: 2022-05-19 13:42
** 作者: leo.liu
** 函数作用：停止记录
** 返回参数说明：
***/
bool video_record_stop(void)
{
	if (video_record_enable == false)
	{
		return false;
	}
	h264_encode_close(0x01);
	audio_input_close(0x01);
	video_record_enable = false;
	return true;
}

static bool video_record_write(unsigned char *data, int size, char type)
{
	if (video_record_enable == false)
	{
		return false;
	}
	common_queue node;
	node.type = sizeof(common_data);
	node.msg.data = user_mem_copy(data, size);
	node.msg.type = type;
	node.msg.size = size;
	node.msg.timestamp = user_timestamp_get();
	msgsnd(video_record_queue_head, &node, node.type, 0);
	return true;
}

/***
** 日期: 2022-05-19 13:56
** 作者: leo.liu
** 函数作用：写入视频
** 返回参数说明：
***/
bool video_record_video_write(unsigned char *data, int size)
{
	return video_record_write(data, size, 0x01);
}
/***
** 日期: 2022-05-19 13:59
** 作者: leo.liu
** 函数作用：写入音频
** 返回参数说明：
***/
bool video_record_audio_write(unsigned char *data, int size)
{
	return video_record_write(data, size, 0x00);
}

/***
** 日期: 2022-05-19 17:13
** 作者: leo.liu
** 函数作用：获取记录状态
** 返回参数说明：
***/
bool video_record_status_get(void)
{
	return (video_record_enable); // == true);// && (video_record_handle_id != NULL);
}
/***
**   日期:2022-05-28 16:31:22
**   作者: leo.liu
**   函数作用：获取录制时长
**   参数说明:
***/
bool video_record_duration_get(int *duration)
{
	if (video_record_enable == false)
	{
		*duration = 0;
		return false;
	}
	*duration = avi_write_frame_duration == 0 ? 0 : user_timestamp_get() - avi_write_frame_duration;
	return true;
}