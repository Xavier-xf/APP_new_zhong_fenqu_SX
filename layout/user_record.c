#include "user_record.h"
#include "user_monitor.h"
#include "layout_define.h"

static REC_MODE record_jpeg_mode = REC_MODE_MANUAL;
static MON_CH record_jpeg_channel = MON_CH_NONE;
static REC_MODE record_video_mode = REC_MODE_MANUAL;
static MON_CH record_video_channel = MON_CH_NONE;

/***
** 日期: 2022-05-19 10:37
** 作者: leo.liu
** 函数作用：接收编码后的流
** 返回参数说明：
***/
static void jpeg_encode_callback(struct video_stream *stream)
{
	// if(tuya_api_preview_quit() == true)
	// {
	// 	printf("==============++++++tuya_api_preview_quit \n");
	// 	record_jpeg_mode = REC_MODE_MANUAL;
	// }

	if(video_input_state_get() == false)
	{
		mjpeg_encode_close();
		return ;
	}

	if ((record_jpeg_mode & 0x0F))
	{
		file_type type = FILE_TYPE_FLASH_PHOTO;
		if (media_sdcard_insert_check() == true)
		{
			type = FILE_TYPE_PHOTO;
		}
		char file_path[128] = {0};
		media_file_create(type, record_jpeg_channel, record_jpeg_mode & 0x0F, file_path);
		int fd = open(file_path, O_CREAT | O_WRONLY);
		if (fd >= 0)
		{
			printf("\n\n snap jpg:%s %dkb \n\n", file_path, stream->len / 1024);
			write(fd, stream->data, stream->len);

			close(fd);
		}
		else
		{
			printf("\n\n snap jpg: %s open failed \n\n", file_path);
		}
		media_file_bad_check(file_path);
	}
	if (record_jpeg_mode & REC_MODE_TUYA_CALL)
	{
		tuya_api_call_event(record_jpeg_channel, (char *)stream->data, stream->len);
		
	}
	if (record_jpeg_mode & REC_MODE_TUYA_ALARM)
	{
		tuya_api_alarm_event(record_jpeg_channel, (char *)stream->data, stream->len);
	}
	if (record_jpeg_mode & REC_MODE_TUYA_MOTION)
	{
		tuya_api_motion_event(record_jpeg_channel, (char *)stream->data, stream->len);
	}
	mjpeg_encode_close();
	system("sync");
}

/***
** 日期: 2022-05-19 10:37
** 作者: leo.liu
** 函数作用：抓拍一张照片
** 返回参数说明：
***/
extern bool tuya_api_doorbell_event(void);
bool record_jpeg_start(REC_MODE mode)
{
	// printf("%d#############%s======#######%d#######\n",mode, __func__, __LINE__);
	if ((mjpeg_encode_status_get() == true))
	{
		// printf("%d#############%s======#######%d#######\n",mode, __func__, __LINE__);
		if(1)// (mode & (REC_MODE_TUYA_CALL | REC_MODE_TUYA_ALARM | REC_MODE_TUYA_MOTION))
		{
			int timer_count = 100;
			while (mjpeg_encode_status_get() == true)
			{
				timer_count--;
				if (timer_count == 0)
				{
					break;
				}
				usleep(10 * 1000);
			}
		}
		else
		{
			printf("#############%s======#######%d#######\n", __func__, __LINE__);
			return false;
		}
	}
	
	if (mjpeg_encode_open(jpeg_encode_callback) == true)
	{
		record_jpeg_mode = mode;
		record_jpeg_channel = monitor_channel_get();
	}
	
	if(mode & REC_MODE_TUYA_CALL){
		printf("#############%s======#######%d#######\n", __func__, __LINE__);
		tuya_api_doorbell_event();
		tuya_doorbell_timestamp_set(user_timestamp_get());
	}

	return true;
}

/***
** 日期: 2022-05-19 10:40
** 作者: leo.liu
** 函数作用：关闭抓拍
** 返回参数说明：
***/
bool record_jpeg_close(void)
{
	if ((mjpeg_encode_status_get() == false))
	{
		return false;
	}
	mjpeg_encode_close();
	return true;
}

static void video_encode_callback(const char *path)
{
	if (media_sdcard_insert_check() == false)
	{
		return;
	}

	char file_path[128] = {0};
	media_file_create(FILE_TYPE_VIDEO, record_video_channel, record_video_mode, file_path);

	char cmd[128] = {0};
	sprintf(cmd, "mv %s %s", path, file_path);
	system(cmd);
	media_file_bad_check(file_path);
	system("sync");
	printf("record video:%s \n", file_path);
}
/***
** 日期: 2022-05-19 16:45
** 作者: leo.liu
** 函数作用：开启录制视频
** 返回参数说明：
***/
extern void h264_encode_requeset_idr(void);
bool record_video_start(REC_MODE mode)
{
	if ((video_record_status_get() == true))
	{
		return false;
	}
	h264_encode_requeset_idr();
	monitor_record_pin_enable(true);
	MON_CH ch = monitor_channel_get();
	bool had_audo = ((ch == MON_CH_CCTV1) || (ch == MON_CH_CCTV2) || (ch == MON_CH_CCTV3) || (ch == MON_CH_CCTV4)) ? false : true;
	if (video_record_start(had_audo, video_encode_callback) == true)
	{
		record_video_mode = mode;
		record_video_channel = ch;
	}
	return true;
}
/***
** 日期: 2022-05-19 17:18
** 作者: leo.liu
** 函数作用：停止录像
** 返回参数说明：
***/
bool record_video_close(void)
{
	if ((video_record_status_get() == false))
	{
		return false;
	}
	video_record_stop();
	//monitor_record_pin_enable(false);
	return true;
}