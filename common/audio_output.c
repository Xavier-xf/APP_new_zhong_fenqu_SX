#include "audio_output.h"
#include "audio_config.h"
#include "user_common.h"
#include <pthread.h>
#include "user_time.h"

#include "ak_ao.h"
#include "ak_mem.h"

/***** 声卡播放音量 *****/
static int audio_output_volume = 100;

/***** 声卡播放线程相关 *****/
static pthread_mutex_t audio_output_mutex;

/***** 声卡句柄 ,只有这个才能作为声卡工作唯一的证据*****/
static int audio_output_handle_id = -1;

/***** 声卡播放参数 *****/
static enum ak_audio_channel_type audio_output_channel = AUDIO_CHANNEL_RESERVED;
static enum ak_audio_sample_rate audio_output_sample = AK_AUDIO_SAMPLE_RATE_96000;
/***
** 日期: 2022-04-27 11:18
** 作者: leo.liu
** 函数作用：打开声卡设备:这个函数只能在audio_output_task中调用
** 返回参数说明：
***/
static void audio_output_device_open(void)
{
	struct ak_audio_out_param par;
	par.dev_id = DEV_DAC;
	par.pcm_data_attr.channel_num = audio_output_channel;
	par.pcm_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;
	par.pcm_data_attr.sample_rate = 8000;//audio_output_sample;
	ak_ao_open(&par, &audio_output_handle_id);

	ak_ao_set_volume(audio_output_handle_id, 0);
	ak_ao_set_gain(audio_output_handle_id, 6);

	ak_ao_set_speaker(audio_output_handle_id, 1);
	ak_ao_enable_eq(audio_output_handle_id, 0);
	ak_ao_enable_nr(audio_output_handle_id, 0);
}

/***
** 日期: 2022-04-27 11:18
** 作者: leo.liu
** 函数作用：关闭声卡设备:这个函数只能在audio_output_task中调用
** 返回参数说明：
***/
static void audio_output_device_close(void)
{
	ak_ao_close(audio_output_handle_id);
	//   audio_output_channel = AUDIO_CHANNEL_RESERVED;
	//   audio_output_sample = AK_AUDIO_SAMPLE_RATE_96000;
	audio_output_handle_id = -1;
}

/***
** 日期: 2022-05-20 11:08
** 作者: leo.liu
** 函数作用：复位参数
** 返回参数说明：
***/
static void audio_output_device_reset(void)
{
	struct ak_audio_out_param par;
	par.dev_id = DEV_DAC;
	par.pcm_data_attr.channel_num = audio_output_channel;
	par.pcm_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;
	par.pcm_data_attr.sample_rate = 8000;//audio_output_sample;
	ak_ao_reset_params(audio_output_handle_id, &par);
}

/***
** 日期: 2022-04-27 10:22
** 作者: leo.liu
** 函数作用：声卡设备初始化
** 返回参数说明：
***/
bool audio_output_init(void)
{
	static bool inited = false;
	if (inited == true)
	{
		printf("audio ouput It's already initialized \n");
		return false;
	}
	inited = true;
	pthread_mutex_init(&audio_output_mutex, NULL);
	return true;
}

/***
** 日期: 2022-04-27 10:23
** 作者: leo.liu
** 函数作用：设置声卡播放参数,
** 返回参数说明：设备之前打开了，但是声卡参数一致返回true,否则返回false
***/
bool audio_output_open(enum ak_audio_channel_type ch, enum ak_audio_sample_rate rate)
{
	bool reslut = true;
	//  common_queue audio_data;
	pthread_mutex_lock(&audio_output_mutex);
	/***** 设备已经打开 *****/
	if ((audio_output_handle_id != -1) /*  && (audio_output_enable == true) */)
	{
		/***** 声卡参数不一致*****/
		if ((ch != audio_output_channel || rate != audio_output_sample))
		{
			printf("The device has been turned on, but the parameters are inconsistent \n");
			audio_output_channel = ch;
			audio_output_sample = rate;
			audio_output_device_reset();
		}
	}
	else 
	{
		audio_output_channel = ch;
		audio_output_sample = rate;
		audio_output_device_open();
		// audio_output_enable = true;
	}
	pthread_mutex_unlock(&audio_output_mutex);

	int timeout = 100;
	while (audio_output_buffer_query() > 2048)
	{
		usleep(1000 * 10);
		timeout--;
		if (timeout == 0)
		{
			if(audio_output_buffer_query() > 50*1024)
			{
				audio_output_device_restart();
			}
			printf("query audio buffer timeout!!!\n");
			break;
		}
	}
	return reslut;
}

/***
** 日期: 2022-04-27 10:24
** 作者: leo.liu
** 函数作用：关闭声卡设备,关闭声卡，必须同步执行（阻塞到声卡设备关闭为止，500ms超时处理）
** 返回参数说明：
***/
bool audio_output_close(void)
{
	pthread_mutex_lock(&audio_output_mutex);
	audio_output_device_close();
	pthread_mutex_unlock(&audio_output_mutex);

	return true;
}

/***
** 日期: 2022-04-27 10:24
** 作者: leo.liu
** 函数作用：将音频数据送入声卡中
** 返回参数说明：
***/
extern 	bool pcm_resamplerate(int src_sample, int dst_dample, const char *src_pcm, int src_frames, unsigned char *dst_pcm, int *dst_frames);
bool audio_output_write(const unsigned char *data, int size)
{
	pthread_mutex_lock(&audio_output_mutex);
	if (audio_output_handle_id == -1)
	{
		audio_output_device_open();
	}
	int len = 0;
	if (audio_output_volume != 100)
	{
		unsigned char *dst;
		pcm16bit_volume_set(data, size, &dst, audio_output_volume);
		data = dst;
	}

 	
	
	if(audio_output_sample == AK_AUDIO_SAMPLE_RATE_8000)
	{
		ak_ao_send_frame(audio_output_handle_id, (unsigned char *)data, size, &len);
	}
	else
	{
		int cover_len =0;
		unsigned char temp[4096];
		pcm_resamplerate(audio_output_sample,8000,(const char *)data,size,temp,&cover_len);       //重采样：回铃声8000，所以将所有的采样率设置为8000
		ak_ao_send_frame(audio_output_handle_id, (unsigned char *)temp, cover_len, &len);
	}
	

	pthread_mutex_unlock(&audio_output_mutex);

	// printf("=====================>>>> %d\n", audio_output_buffer_query());
	return true;
}

/***
** 日期: 2022-04-27 10:25
** 作者: leo.liu
** 函数作用：设置声卡播放音量
** 返回参数说明：
***/
bool audido_output_volume_set(int volume)
{
	if ((volume < 0) || (volume > 100))
	{
		return false;
	}
	audio_output_volume = volume;
	return true;
}

/***
** 日期: 2022-04-27 10:26
** 作者: leo.liu
** 函数作用：获取声卡播放音量
** 返回参数说明：
***/
int audio_output_volume_get(void)
{
	return audio_output_volume;
}

/***
** 日期: 2022-04-27 15:36
** 作者: leo.liu
** 函数作用：查询声卡中剩余的音频数据
** 返回参数说明：
***/
int audio_output_buffer_query(void)
{
	struct ak_dev_buf_status buf_status;
	pthread_mutex_lock(&audio_output_mutex);
	/***** 设备已经打开 *****/
	if (audio_output_handle_id == -1)
	{
		pthread_mutex_unlock(&audio_output_mutex);
		return -1;
	}
	ak_ao_get_buf_status(audio_output_handle_id, &buf_status);
	pthread_mutex_unlock(&audio_output_mutex);
	return buf_status.buf_remain_len;
} 
/***
**   日期:2022-05-26 11:37:15
**   作者: leo.liu
**   函数作用：超时等待
**   参数说明:
***/
bool audio_output_buffer_try_timeout(int ms)
{
	unsigned long long start = user_timestamp_get();
	while (1)
	{
		if (audio_output_buffer_query() < 1)
		{
			return true;
		}
		unsigned long long end = user_timestamp_get();
		if((end - start) > ms){

			break;
		}
		
		usleep(1 * 1000);
	}
	printf("audio output buffer query timeout :%d \n", ms);
	return false;
}

bool audio_output_device_restart(void)
{
	pthread_mutex_lock(&audio_output_mutex);
	if (audio_output_handle_id == -1)
	{
		pthread_mutex_unlock(&audio_output_mutex);
		return false;
	}
	ak_ao_cancel(audio_output_handle_id);
	ak_ao_restart(audio_output_handle_id);
	pthread_mutex_unlock(&audio_output_mutex);
	return true;
}
