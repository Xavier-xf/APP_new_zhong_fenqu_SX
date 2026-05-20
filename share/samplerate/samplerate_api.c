#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <samplerate.h>
#include <unistd.h>

#include <fcntl.h>

#define SAMPLE_FRAME_MAX (4096)//(4096)

/***
**   日期:2022-07-06 16:00:17
**   作者: leo.liu
**   函数作用：获取句柄
**   参数说明:
***/
static bool resamplerate_open(SRC_STATE **handle, SRC_DATA **frame)
{
	static SRC_STATE *src_state = NULL;
	static SRC_DATA src_data;
	if (src_state == NULL)
	{
		int error = -1;
		src_state = src_new(SRC_LINEAR, 1, &error);

		static float in[SAMPLE_FRAME_MAX] = {0};
		static float out[SAMPLE_FRAME_MAX] = {0};
		src_data.data_in = in;
		src_data.data_out = out;
		src_data.output_frames = SAMPLE_FRAME_MAX;
		src_data.end_of_input = 1;
	}
	*handle = src_state;
	*frame = &src_data;
	return true;
}
/***
**   日期:2022-07-06 16:00:25
**   作者: leo.liu
**   函数作用：初始化参数
**   参数说明:
***/
static bool resamplerate_init(SRC_STATE *handle_id, SRC_DATA *frame, int src_sample, int dst_sample, const char *src_pcm, int src_frames)
{
	memset((float *)frame->data_in, 0, SAMPLE_FRAME_MAX * sizeof(float));
	memset(frame->data_out, 0, SAMPLE_FRAME_MAX * sizeof(float));

	frame->input_frames = src_frames / 2;
	frame->src_ratio = dst_sample * 1.0 / src_sample;
	src_short_to_float_array((const short *)src_pcm, (float *)frame->data_in, frame->input_frames);
	return true;
}
/***
**   日期:2022-07-06 16:06:02
**   作者: leo.liu
**   函数作用：开始转换
**   参数说明:
***/
static bool resamplerate_cover(SRC_STATE *handle_id, SRC_DATA *frame, unsigned char *data, int *data_len)
{
	int ret = 0;
	src_reset(handle_id);
	if ((ret = src_process(handle_id, frame)) == 0)
	{
		*data_len = frame->output_frames_gen * 2;
		src_float_to_short_array(frame->data_out, (short *)data, frame->output_frames_gen);
		return true;
	}
	return false;
}
bool pcm_resamplerate(int src_sample, int dst_sample, const char *src_pcm, int src_frames, unsigned char *dst_pcm, int *dst_frames)
{
	SRC_STATE *handle_id;
	SRC_DATA *frame;

	resamplerate_open(&handle_id, &frame);

	resamplerate_init(handle_id, frame, src_sample, dst_sample, src_pcm, src_frames);

	resamplerate_cover(handle_id, frame, dst_pcm, dst_frames);

	return true;
}
