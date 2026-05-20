#include "audio_input.h"
#include <pthread.h>
#include "ak_ai.h"
#include "string.h"
#include <unistd.h>
#include "user_time.h"
#include "user_common.h"
#include "audio_output.h"
// #include "lvgl/lvgl.h"
// #include "../../layout/layout_common.h"
#include "tuya/tuya_ring_buffer.h"
#include "tuya/tuya_g711_utils.h"
#include "tuya/tuya_api.h"
#include "tuya_ipc_stream_storage.h"

bool h264_encode_sample_get(void);

bool tuya_app_preview_check(void);


bool video_record_audio_write(unsigned char *data, int size);

static char audio_input_mask = 0x00;
static int audio_input_handle_id = -1;
static bool audio_input_capture = false;
static pthread_mutex_t audio_input_mutex;


static unsigned long long tuya_playback_audio_ringbuffer_post_size = 0;
void audio_playback_ringbuffer_add(int size){

	tuya_playback_audio_ringbuffer_post_size += size;
}
unsigned long long  audio_playback_ringbuffer_get(void){

	return tuya_playback_audio_ringbuffer_post_size;
}
void audio_playback_ringbuffer_clear(void){

	tuya_playback_audio_ringbuffer_post_size = 0;
}

/**********************************************************************
设置各种音频处理参数结构体为默认值，默认值是从ak_audio_config.h抄的。
参数仅作参考，实际参数要根据不同产品独立设定。
1：NEAR(ai), ak_audio_nr_attr
2：NEAR(ai), ak_audio_agc_attr
3：NEAR(ai), ak_audio_aec_attr
4：NEAR(ai), ak_audio_aslc_attr
5：FAR(ao), ak_audio_nr_attr
6：FAR(ao), ak_audio_aslc_attr
***********************************************************************/
static void setup_default_audio_argument(void *audio_args, char args_type)
{
	struct ak_audio_nr_attr default_ai_nr_attr = {-40, 0, 1};
	struct ak_audio_agc_attr default_ai_agc_attr = {24576, 4, 0, 80, 0, 1};
	struct ak_audio_aec_attr default_ai_aec_attr = {0, /*1024*/ 1024, 1024, 0, /*512*/ 640, 1};
	//struct ak_audio_aec_attr default_ai_aec_attr = {0, /*1024*/ 102, 102, 0, /*512*/ 640, 1};
	struct ak_audio_aslc_attr default_ai_aslc_attr = {9830, 0, 0};

	struct ak_audio_nr_attr default_ao_nr_attr = {-40, 0, 1};
	struct ak_audio_aslc_attr default_ao_aslc_attr = {9830, 0, 0};

	switch (args_type)
	{
	case 1:
		*(struct ak_audio_nr_attr *)audio_args = default_ai_nr_attr;
		break;
	case 2:
		*(struct ak_audio_agc_attr *)audio_args = default_ai_agc_attr;
		break;
	case 3:
		*(struct ak_audio_aec_attr *)audio_args = default_ai_aec_attr;
		break;
	case 4:
		*(struct ak_audio_aslc_attr *)audio_args = default_ai_aslc_attr;
		break;
	case 5:
		*(struct ak_audio_nr_attr *)audio_args = default_ao_nr_attr;
		break;
	case 6:
		*(struct ak_audio_aslc_attr *)audio_args = default_ao_aslc_attr;
		break;
	default:
		break;
	}

	return;
}

static void audio_input_device_init(void)
{
	struct ak_audio_in_param ai_param;
	ai_param.pcm_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;
	ai_param.pcm_data_attr.channel_num = AUDIO_CHANNEL_MONO;
	ai_param.pcm_data_attr.sample_rate = AK_AUDIO_SAMPLE_RATE_8000; // AK_AUDIO_SAMPLE_RATE_8000;
	ai_param.dev_id = DEV_ADC;
	if (ak_ai_open(&ai_param, &audio_input_handle_id))
	{
		printf("ai open fail \n\r");
		return;
	}
	ak_ai_set_source(audio_input_handle_id, AI_SOURCE_MIC);

	ak_ai_set_gain(audio_input_handle_id, 5);	// enable_agc
	ak_ai_set_volume(audio_input_handle_id, 0); // enable_agc

	struct ak_audio_nr_attr nr_attr;
	setup_default_audio_argument(&nr_attr, 1);
	ak_ai_set_nr_attr(audio_input_handle_id, &nr_attr);
	ak_ai_enable_nr(audio_input_handle_id, AUDIO_FUNC_ENABLE);

	struct ak_audio_agc_attr agc_attr;
	setup_default_audio_argument(&agc_attr, 2);
	ak_ai_set_agc_attr(audio_input_handle_id, &agc_attr);
	ak_ai_enable_agc(audio_input_handle_id, AUDIO_FUNC_ENABLE);

	struct ak_audio_aec_attr aec_attr;
	setup_default_audio_argument(&aec_attr, 3);
	ak_ai_set_aec_attr(audio_input_handle_id, &aec_attr);
	ak_ai_enable_aec(audio_input_handle_id, AUDIO_FUNC_DISABLE);

	ak_ai_enable_eq(audio_input_handle_id, AUDIO_FUNC_ENABLE);
}

/***
** 日期: 2022-05-19 11:46
** 作者: leo.liu
** 函数作用：开启音频采集
** 返回参数说明：
***/
static void audio_input_device_open(void)
{
	ak_ai_get_handle_id(DEV_ADC, &audio_input_handle_id);
	ak_ai_clear_frame_buffer(audio_input_handle_id);
	ak_ai_start_capture(audio_input_handle_id);
	ak_ai_enable_aec(audio_input_handle_id,0);

}

static void audio_input_device_close(void)
{
	ak_ai_stop_capture(audio_input_handle_id);
	ak_ai_enable_aec(audio_input_handle_id,0);
	audio_input_handle_id = -1;
}
/***
** 日期: 2022-05-19 11:34
** 作者: leo.liu
** 函数作用：音频采集线程user
** 返回参数说明：
***/
static void *audio_input_task(void *arg)
{
	struct frame ai_frame;
	printf("***** audio input task create sccess ! *****\n");
	audio_input_device_init();

//	int app_preview_delay = 0;
	static int aec_enable_delay_count = 0;
	static unsigned char aec_enable_flag = 0x00;
	while (1)
	{
		pthread_mutex_lock(&audio_input_mutex);
		if ((audio_input_capture == false) && (audio_input_handle_id != -1))
		{
			audio_input_device_close();
			aec_enable_flag = 0x00;
			aec_enable_delay_count = 0;
			//app_preview_delay = 0;
			printf("##### audio input close success ! \n");
		}
		else if ((audio_input_capture == true) && (audio_input_handle_id == -1))
		{
			audio_input_device_open();
			aec_enable_flag = 0x00;
			aec_enable_delay_count = 0;
			//app_preview_delay = 0;
			printf("##### audio input open success ! \n");
		}

		if ((audio_input_capture == true) && (audio_input_handle_id != -1))
		{
			memset(&ai_frame, 0, sizeof(struct frame));
			if (ak_ai_get_frame(audio_input_handle_id, &ai_frame, 0) == 0)
			{

				if ((audio_input_mask & 0x01) && (audio_input_handle_id != -1))
				{
					video_record_audio_write(ai_frame.data, ai_frame.len);

					if((aec_enable_flag == 0x01)&&((audio_input_mask & 0x02) == 0))
					{
						aec_enable_flag = 0x00;
						aec_enable_delay_count = 0;
						ak_ai_enable_aec(audio_input_handle_id,0);
						printf("########## audio input aec dsiable ###############\n");
					}
				} 
				if (/*(audio_input_mask & 0x02) && */(audio_input_handle_id != -1))
				{
					
					extern bool tuya_api_app_talk_check(void);
					extern bool layout_monitor_talk_state(void);
					extern bool indoor_talk_state_get(void);
					
					// if((tuya_api_app_talk_check() == true) && ((layout_monitor_talk_state() == true) || indoor_talk_state_get()))
					// {
					// 	memset(ai_frame.data,0,ai_frame.len);
					// }
					#if 1
					extern bool h264_encode_sample_get();
					extern int tuya_online_num_get_customize();

					if(h264_encode_sample_get() && (tuya_online_num_get_customize() > 0))
					{
						memset(ai_frame.data,0,ai_frame.len);
					}
					#endif
					if(tuya_online_num_get_customize() > 0)
					{	
						
						if((aec_enable_flag != 0x01)&&((aec_enable_delay_count ++) > 100))
						{
							ak_ai_enable_aec(audio_input_handle_id,1);
							aec_enable_flag = 0x01;
							printf("########## audio input aec enable ###############\n");
						}
					}
					unsigned char dst_data[1024] = {0};
					unsigned int dst_data_len = 0;
					tuya_g711_encode(1, (unsigned short *)ai_frame.data, ai_frame.len, dst_data, &dst_data_len);
					
					if(1){//(/*tuya_app_preview_check() == true&&*/(app_preview_delay++ > 30)){
 
						//app_preview_delay = 101;
						// printf("device_send_audio_to_tuya################\n");
						tuya_ipc_ring_buffer_append_data(E_CHANNEL_AUDIO, dst_data, dst_data_len, E_AUDIO_FRAME, user_timestamp_get());
						audio_playback_ringbuffer_add(dst_data_len);
					}
				}
				ak_ai_release_frame(audio_input_handle_id, &ai_frame);
			}
		}
		pthread_mutex_unlock(&audio_input_mutex);
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
bool audio_input_init(void)
{
	pthread_mutex_init(&audio_input_mutex, NULL);
	pthread_t thread_t;
	pthread_create(&thread_t, user_pthread_atter_get(), audio_input_task, NULL);
	return true;
}

/***
** 日期: 2022-05-19 11:33
** 作者: leo.liu
** 函数作用：音频打开采集
** 返回参数说明：
***/
bool audio_input_open(unsigned char mask)
{
	pthread_mutex_lock(&audio_input_mutex);
	if (audio_input_mask & mask)
	{
		pthread_mutex_unlock(&audio_input_mutex);
		return false;
	}
	audio_input_mask |= mask;
	pthread_mutex_unlock(&audio_input_mutex);
	return true;
}
/***
** 日期: 2022-05-19 11:34
** 作者: leo.liu
** 函数作用：音频关闭
** 返回参数说明：
***/
bool audio_input_close(unsigned char mask)
{
	pthread_mutex_lock(&audio_input_mutex);
	if ((audio_input_mask & mask) == 0x00)
	{
		pthread_mutex_unlock(&audio_input_mutex);
		return true;
	}
	audio_input_mask &= (~mask);
	pthread_mutex_unlock(&audio_input_mutex);
	return true;
}
/***
**   日期:2022-05-26 08:39:18
**   作者: leo.liu
**   函数作用：音频采集开关
**   参数说明:
***/
void audio_input_capture_enable(bool en)
{
	pthread_mutex_lock(&audio_input_mutex);
	audio_input_capture = en;
	pthread_mutex_unlock(&audio_input_mutex);
}
/***
**   日期:2022-05-26 09:45:25
**   作者: leo.liu
**   函数作用：使能AEC/消回声
**   参数说明:
***/
void audio_input_aec_enable(bool en)
{
	pthread_mutex_lock(&audio_input_mutex);
	if (audio_input_handle_id != -1)
	{
		ak_ai_enable_aec(audio_input_handle_id, en);
	}
	pthread_mutex_unlock(&audio_input_mutex);
}

bool audio_input_state_get(void)
{
	return (audio_input_handle_id != -1)&&(audio_input_capture == true);
}
