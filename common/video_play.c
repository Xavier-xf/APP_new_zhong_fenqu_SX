#include "video_play.h"
#include "avilib.h"
#include <pthread.h>
#include "unistd.h"
#include "ak_mem.h"
#include "video_decode.h"
#include "video_input.h"
#include "audio_output.h"
#include "user_time.h"
#include "lvgl/lv_obj.h"
#include "lvgl/lv_disp.h"
#include "user_common.h"

decode_finish_callback h264_decode_read_frame_func_register(decode_finish_callback read_frame);

static decode_finish_callback old_decode_finish_func = NULL;

static const rom_bin_info *p_scr_act_img = NULL;

#define AUDIO_FRAME_SIZE 512
/*****  播放状态 *****/
static VIDEO_PLAY_STATUS video_play_status = VIDEO_PLAY_STATE_IDLE;
static pthread_mutex_t video_play_mutex;
/*****  播放设备句柄 *****/
static avi_t *avi_handle_id = NULL;
/*****  播放文件 *****/
static char video_play_file[128] = {0};
/*****  一帧视频时长 *****/
static double video_one_frame_duration = 0;
/*****  一帧音频时长 *****/
static double audio_one_frame_duration = 0;
/*****  播放总帧数 *****/
static int video_frame_total = 0;
/*****  视频当前播放帧 *****/
static int video_frame_video_index = 0;
/*****  音频当前帧 *****/
static int video_frame_audio_index = 0;
/*****  标志已经播放完成 *****/
static bool video_play_eof_flg = false;
/*****  标记第一帧解码显示完成 *****/
static char video_first_play = 0x00;
/*****  是否有音频数据 *****/
static bool video_play_has_audio = false;

// static bool video_play_thread_run = false;
/***** 开启功放 *****/
extern bool power_amplifier_enable(bool);
/***
**   日期:2022-05-24 15:17:02
**   作者: leo.liu
**   函数作用：关闭播放设备
**   参数说明:
***/
static void video_play_device_close(void)
{
	AVI_close(avi_handle_id);
	avi_handle_id = NULL;
}
/***
**   日期:2022-05-24 15:17:10
**   作者: leo.liu
**   函数作用：打开播放设备
**   参数说明:
***/
static void video_play_device_open(void)
{
	avi_handle_id = AVI_open_input_file(video_play_file, 1);
	video_frame_total = AVI_video_frames(avi_handle_id);
	video_frame_video_index = 0;
	video_frame_audio_index = 0;
	video_play_eof_flg = false;
	video_one_frame_duration = (double)(1000.0 / (AVI_frame_rate(avi_handle_id) + 0.0001));
	audio_one_frame_duration = (double)(AUDIO_FRAME_SIZE * 8 * 1000.0 / (8000 * 16 * 1));
	h264_decode_buffer_clear();
	while (audio_output_buffer_query() > 0)
	{
		usleep(1000); 
	}

	int audio_channels = AVI_audio_channels(avi_handle_id);
	int audio_rate = AVI_audio_rate(avi_handle_id);
	int audio_byte = AVI_audio_bits(avi_handle_id);
	video_play_has_audio = false;
	if ((audio_channels == 1) && (audio_rate == 8000) && (audio_byte == 16))
	{
		video_play_has_audio = true;
		audio_output_open(AUDIO_CHANNEL_MONO, AK_AUDIO_SAMPLE_RATE_8000);
		audido_output_volume_set(100);
		/***** 执行用户回调函数 *****/
		power_amplifier_enable(true);
	}
}


/***
**   日期:2022-05-25 10:54:13
**   作者: leo.liu
**   函数作用：发送第一帧视频帧
**   参数说明:
***/
static void video_play_send_video_first_frame(void)
{
	int size = 0;
	for (int i = 0; size < 100 && i < 10; i++)
	{
		AVI_set_video_position(avi_handle_id, i);
		size = AVI_frame_size(avi_handle_id, i);
		unsigned char *data = ak_mem_alloc(MODULE_ID_VDEC, size);
		int keyframe;
		size = AVI_read_frame(avi_handle_id, (char *)data, &keyframe);
		h264_decode_stream_write(data, size);
		ak_mem_free(data);
	}
}

/***
**   日期:2022-05-25 10:54:13
**   作者: leo.liu
**   函数作用：发送一帧视频帧
**   参数说明:
***/
static void video_play_send_video_frame(void)
{
	AVI_set_video_position(avi_handle_id, video_frame_video_index);
	int size = AVI_frame_size(avi_handle_id, video_frame_video_index);
	unsigned char *data = ak_mem_alloc(MODULE_ID_VDEC, size);
	int keyframe;
	size = AVI_read_frame(avi_handle_id, (char *)data, &keyframe);
	h264_decode_stream_write(data, size);
	ak_mem_free(data);
	// printf("==================>>> size:[%d]\n", size);
}

static void *video_play_task(void *arg)
{
	printf("***** video play create sccess ! *****\n");
	unsigned char audio_data[AUDIO_FRAME_SIZE] = {0};
	// video_play_thread_run = true;
	while (1)
	{
		
		pthread_mutex_lock(&video_play_mutex);

		if ((video_play_status == VIDEO_PLAY_STATE_IDLE) && (avi_handle_id != NULL))
		{

			video_play_device_close();
			printf("#### video play close \n");
		}
		else if ((video_play_status != VIDEO_PLAY_STATE_IDLE) && (avi_handle_id == NULL))
		{

			video_first_play = 0x00;
			video_play_device_open();

			printf("### video play open \n");
		}

		if ((video_play_status == VIDEO_PLAY_STATE_PLAY) && (avi_handle_id != NULL))
		{

			if (video_frame_video_index < video_frame_total)           
			{

				if (video_first_play == 0x00)
				{
					video_play_send_video_first_frame();
					usleep(10 * 1000);
				}
				else if (video_first_play == 0x01)
				{
					video_first_play = 0x02;
					video_play_status = VIDEO_PLAY_STATE_PAUSE;
					video_frame_video_index = 0;
					video_frame_audio_index = 0;
					AVI_set_video_position(avi_handle_id, video_frame_video_index);
					AVI_set_audio_position(avi_handle_id, video_frame_audio_index);
				}
				else
				{

					unsigned long long cur_pts = user_timestamp_get();
					if (video_play_has_audio == true)
					{
						unsigned long long video_play_clock_base = cur_pts - (unsigned long long)(video_frame_audio_index * audio_one_frame_duration);
						unsigned long long v_pts = video_play_clock_base + (unsigned long long)(video_frame_video_index * video_one_frame_duration);
						if (((cur_pts >= v_pts) && ((cur_pts - v_pts) < 25)) || ((cur_pts < v_pts) && ((v_pts - cur_pts) < 100)))
						{
							video_play_send_video_frame();
							video_frame_video_index++;
							// printf("decode video frame:%d \n", video_frame_video_index);
						}
						else if (cur_pts > v_pts)
						{
							video_frame_video_index++;
							AVI_set_video_position(avi_handle_id, video_frame_video_index);
							// printf("video frame skip index:%d \n", video_frame_video_index);
						}

						if (audio_output_buffer_query() < 3000)
						{
							int size = (int)AVI_read_audio(avi_handle_id, (char *)audio_data, AUDIO_FRAME_SIZE);
							if (size > 0)
							{
								audio_output_write(audio_data, size);
							}
							video_frame_audio_index++;
							// printf("decode audio frame:%d \n", video_frame_audio_index);
						}
					}
					else
					{

						static unsigned long long video_pre_pts = 0;
						if (video_pre_pts == 0)
						{
							video_pre_pts = user_timestamp_get();
						}
						unsigned long long v_next_pts = video_pre_pts + (unsigned long long)(video_one_frame_duration);
						if (cur_pts >= v_next_pts)
						{
							video_play_send_video_frame();
							video_frame_video_index++;
							video_pre_pts = user_timestamp_get();
							// printf("decode video frame:%d \n", video_frame_video_index);
						}
					}
				}
			}
			else if (video_play_eof_flg == false)
			{

				video_play_eof_flg = true;
				video_play_status = VIDEO_PLAY_STATE_PAUSE;
				video_frame_video_index = 0;
				video_frame_audio_index = 0;
				AVI_set_video_position(avi_handle_id, video_frame_video_index);
				AVI_set_audio_position(avi_handle_id, video_frame_audio_index);
				h264_decode_buffer_clear();
			}
		}
		else if ((video_play_status == VIDEO_PLAY_STATE_PAUSE) && (avi_handle_id != NULL))
		{
			if(video_play_eof_flg || video_frame_video_index == 0)
				video_play_send_video_first_frame();
			else
				video_play_send_video_frame();
			screen_force_refresh();
			usleep(100 * 1000);
		}
		
		pthread_mutex_unlock(&video_play_mutex);
		usleep(1 * 1000);
	}
	// video_play_thread_run = false;
	return NULL;
}

/***
**   日期:2022-05-24 14:27:18
**   作者: leo.liu
**   函数作用：视频播放初始化
**   参数说明:
***/
bool video_play_init(void)
{

	pthread_mutex_init(&video_play_mutex, NULL);
	pthread_t thread_t;
	pthread_create(&thread_t, user_pthread_atter_get(), video_play_task, NULL);
	return true;
}

static void video_play_decode_read_frame_func(struct ak_vdec_frame *frame)
{
	video_main_display_lock();
	video_input_resident_buffer_write(frame->frame_obj.data.data, frame->frame_obj.data.pitch_width, frame->frame_obj.data.pitch_height, 0, 0, frame->width, frame->height, GP_FORMAT_YUV420SP);
	video_main_display_unlock();
	if (video_first_play == 0x02)
	{
		screen_force_refresh();
	}
	else if (video_first_play == 0x00)
	{
		video_first_play = 0x01;
	}
}

/***
**   日期:2022-05-24 14:27:45
**   作者: leo.liu
**   函数作用：视频开始播放
**   参数说明:
***/
bool video_play_start(const char *file)
{
	pthread_mutex_lock(&video_play_mutex);
	if (video_play_status != VIDEO_PLAY_STATE_IDLE)
	{
		pthread_mutex_unlock(&video_play_mutex);
		return false;
	}
	memset(video_play_file, 0, sizeof(video_play_file));
	strcpy(video_play_file, file);
	video_play_status = VIDEO_PLAY_STATE_PLAY;
	if (old_decode_finish_func == NULL)
	{
		old_decode_finish_func = h264_decode_read_frame_func_register(video_play_decode_read_frame_func);
	}
	if (p_scr_act_img == NULL)
	{
		p_scr_act_img = lv_obj_get_style_pattern_image(lv_scr_act(), LV_OBJ_PART_MAIN);
	}
	lv_obj_set_style_local_pattern_image(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, NULL);
	lv_video_mode_enable(true);
	video_display_preview_enable(true);
	pthread_mutex_unlock(&video_play_mutex);
	return true;
}
/***
**   日期:2022-05-24 14:28:35
**   作者: leo.liu
**   函数作用：视频停止播放
**   参数说明:
***/
bool video_play_stop(void)
{
	pthread_mutex_lock(&video_play_mutex);
	if (video_play_status == VIDEO_PLAY_STATE_IDLE)
	{
		pthread_mutex_unlock(&video_play_mutex);
		return false;
	}
	video_play_status = VIDEO_PLAY_STATE_IDLE;

	// video_play_thread_run = false;
	// int timeout = 300;
	// while (video_play_thread_run == true)
	// {
	// 	timeout--;
	// 	if(timeout == 0)
	// 	{
	// 		break;
	// 	}
	// 	usleep(10*1000);
	// }

	if (old_decode_finish_func != NULL)
	{
		h264_decode_read_frame_func_register(old_decode_finish_func);
		old_decode_finish_func = NULL;
	}
	if (p_scr_act_img != NULL)
	{
		lv_obj_set_style_local_pattern_image(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, p_scr_act_img);
		p_scr_act_img = NULL;
	}
	lv_video_mode_enable(false);
	video_display_preview_enable(false);
	pthread_mutex_unlock(&video_play_mutex);
	return true;
}
/***
**   日期:2022-05-24 14:28:51
**   作者: leo.liu
**   函数作用：暂停播放
**   参数说明:
***/
bool video_play_pause(void)
{
	pthread_mutex_lock(&video_play_mutex);
	if (video_play_status == VIDEO_PLAY_STATE_IDLE)
	{
		pthread_mutex_unlock(&video_play_mutex);
		return false;
	}
	if (video_play_status == VIDEO_PLAY_STATE_PLAY)
	{
		video_play_status = VIDEO_PLAY_STATE_PAUSE;
	}
	else if (video_play_status == VIDEO_PLAY_STATE_PAUSE)
	{
		video_play_status = VIDEO_PLAY_STATE_PLAY;
		if (video_play_eof_flg == true)
		{
			video_play_eof_flg = false;
		}
	}
	pthread_mutex_unlock(&video_play_mutex);
	return true;
}
/***
**   日期:2022-05-24 14:30:09
**   作者: leo.liu
**   函数作用：获取视频播放状态
**   参数说明:
***/
VIDEO_PLAY_STATUS video_play_status_get(void)
{
	pthread_mutex_lock(&video_play_mutex);
	VIDEO_PLAY_STATUS state = video_play_status;
	pthread_mutex_unlock(&video_play_mutex);
	return state;
}
/***
**   日期:2022-05-24 14:32:04
**   作者: leo.liu
**   函数作用：获取播放时长
**   参数说明:
***/
bool video_play_duration_get(int *cur, int *total)
{
	pthread_mutex_lock(&video_play_mutex);
	if (video_play_status == VIDEO_PLAY_STATE_IDLE)
	{
		pthread_mutex_unlock(&video_play_mutex);
		return false;
	}
	*total = video_frame_total * video_one_frame_duration;
	if (video_play_eof_flg == true)
	{
		*cur = *total;
	}
	else
	{
		*cur = video_frame_video_index * video_one_frame_duration;
	}
	pthread_mutex_unlock(&video_play_mutex);
	return true;
}
/***
**   日期:2022-06-11 11:41:27
**   作者: leo.liu
**   函数作用：播放显示位置设置
**   参数说明:
***/
bool video_play_display_pos(int x, int y, int w, int h)
{
	return video_input_display_pos(x, y, w, h);
}
