#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "tuya_ring_buffer.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_stream_storage.h"
#include "tuya_ipc_p2p.h"
#include "tuya_g711_utils.h"
#include "lv_msg_event.h"
#include "audio_output.h"
#include "tuya_api.h"
#include "../../layout/layout_common.h"

/***************************************录像case*****************************************************/
typedef struct
{
    BOOL_T enabled;
    TRANSFER_VIDEO_CLARITY_TYPE_E live_clarity;
    UINT_T max_users;
    TUYA_CODEC_ID p2p_audio_codec;
} TUYA_APP_P2P_MGR;

// STATIC TUYA_APP_P2P_MGR s_p2p_mgr = {0};

STATIC VOID __TUYA_APP_media_frame_TO_trans_video(IN CONST MEDIA_FRAME_S *p_in, INOUT TRANSFER_VIDEO_FRAME_S *p_out)
{
    UINT_T codec_type = 0;
    codec_type = (p_in->type & 0xff00) >> 8;
    p_out->video_codec = (codec_type == 0 ? TUYA_CODEC_VIDEO_H264 : TUYA_CODEC_VIDEO_H265);
    p_out->video_frame_type = (p_in->type && 0xff) == E_VIDEO_PB_FRAME ? TY_VIDEO_FRAME_PBFRAME : TY_VIDEO_FRAME_IFRAME;
    p_out->p_video_buf = p_in->p_buf;
    p_out->buf_len = p_in->size;
    p_out->pts = p_in->pts;
    p_out->timestamp = p_in->timestamp;
}

STATIC VOID __TUYA_APP_media_frame_TO_trans_audio(IN CONST MEDIA_FRAME_S *p_in, INOUT TRANSFER_AUDIO_FRAME_S *p_out)
{
    p_out->audio_codec = TUYA_CODEC_AUDIO_G711U;
    p_out->audio_sample = TUYA_AUDIO_SAMPLE_8K;
    p_out->audio_databits = TUYA_AUDIO_DATABITS_16;
    p_out->audio_channel = TUYA_AUDIO_CHANNEL_MONO;
    p_out->p_audio_buf = p_in->p_buf;
    p_out->buf_len = p_in->size;
    p_out->pts = p_in->pts;
    p_out->timestamp = p_in->timestamp;
}

STATIC VOID __TUYA_APP_ss_pb_event_cb(IN UINT_T pb_idx, IN SS_PB_EVENT_E pb_event, IN PVOID_T args)
{
    printf("ss pb rev event: %u %d", pb_idx, pb_event);
    if (pb_event == SS_PB_FINISH)
    {
        tuya_ipc_playback_send_finish(pb_idx);
    }
}

STATIC VOID __TUYA_APP_ss_pb_get_video_cb(IN UINT_T pb_idx, IN CONST MEDIA_FRAME_S *p_frame)
{
    TRANSFER_VIDEO_FRAME_S video_frame = {0};
    __TUYA_APP_media_frame_TO_trans_video(p_frame, &video_frame);
    tuya_ipc_playback_send_video_frame(pb_idx, &video_frame);
}

STATIC VOID __TUYA_APP_ss_pb_get_audio_cb(IN UINT_T pb_idx, IN CONST MEDIA_FRAME_S *p_frame)
{
    TRANSFER_AUDIO_FRAME_S audio_frame = {0};
    __TUYA_APP_media_frame_TO_trans_audio(p_frame, &audio_frame);
    tuya_ipc_playback_send_audio_frame(pb_idx, &audio_frame);
}
/*********************************************结束**********************************************************************/

/***
**   日期:2022-06-07 16:46:00
**   作者: leo.liu
**   函数作用：用于标记第一次接收到音频数据
**   参数说明:
***/
static bool tuya_receive_audio_first = false;

/***
**   日期:2022-06-07 14:46:12
**   作者: leo.liu
**   函数作用：p2p状态发生改变
**   参数说明:暂时为用到
***/
STATIC VOID __depereated_online_cb(IN TRANSFER_ONLINE_E status)
{
    printf("__depereated_online_cb====================>>%d \n", status);
}

/***
**   日期:2022-06-07 14:47:04
**   作者: leo.liu
**   函数作用：设备端接收app端的音频数据
**   参数说明:
***/
extern bool h264_encode_sample_get();
STATIC VOID tuya_device_audio_receive_cb(IN CONST TRANSFER_AUDIO_FRAME_S *p_audio_frame, IN CONST UINT_T frame_no)
{
    extern bool layout_monitor_talk_state(void);
    extern bool tuya_outdoor_open_enable_get(void);
    extern MON_ENTER_FLG monitor_enter_mask_get(void);
    if((tuya_outdoor_open_enable_get() == true) || h264_encode_sample_get() == true)
    {
        return;
    }
    if ((layout_monitor_talk_state() == false) && (monitor_enter_mask_get() != MON_ENTER_TALK)) 
    {
        if (tuya_receive_audio_first == true)
        {
            /*****  用于挂断其他设备 *****/
            tuya_api_preview_quit();
            audio_output_open(AUDIO_CHANNEL_MONO, AK_AUDIO_SAMPLE_RATE_8000);
            tuya_receive_audio_first = false;
        }
        unsigned char pcm_data[640] = {0};
        unsigned int pcm_len = 0;
        tuya_g711_decode(TUYA_G711_MU_LAW, (unsigned short *)p_audio_frame->p_audio_buf, p_audio_frame->buf_len, pcm_data, &pcm_len);
        audio_output_write(pcm_data, pcm_len);
    }
}

static void tuya_p2p_ringbuffer_clean(void)
{
    tuya_ipc_ring_buffer_clean_user_state_and_buffer(E_CHANNEL_VIDEO_MAIN, E_USER_P2P_USER);
    tuya_ipc_ring_buffer_clean_user_state_and_buffer(E_CHANNEL_AUDIO, E_USER_P2P_USER);

    // tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_VIDEO_MAIN, E_USER_P2P_USER);
    // tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_AUDIO, E_USER_P2P_USER);

#if 0
    Ring_Buffer_Node_S *node = tuya_ipc_ring_buffer_get_video_frame(E_CHANNEL_VIDEO_MAIN, E_USER_P2P_USER, 0);
    if (node)
    {

        tuya_ipc_ring_buffer_anchor_user(E_CHANNEL_VIDEO_MAIN, E_USER_P2P_USER, node->seqNo, 0);
    }

    node = tuya_ipc_ring_buffer_get_audio_frame(E_CHANNEL_AUDIO, E_USER_P2P_USER, 0);
    if (node)
    {

        tuya_ipc_ring_buffer_anchor_user(E_CHANNEL_AUDIO, E_USER_P2P_USER, node->seqNo, 0);
    }
#endif
}
/***
**   日期:2022-06-07 14:47:43
**   作者: leo.liu
**   函数作用：p2ps事件接收
**   参数说明:
***/
STATIC INT_T tuya_p2p_event_cb(IN CONST TRANSFER_EVENT_E event, IN CONST PVOID_T args)
{
    if ((h264_encode_sample_get()) && (event != TRANS_LIVE_VIDEO_STOP) && (event != TRANS_LIVE_VIDEO_START))
    {
        // printf("==AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA==ksfkajfkaf\n");
        return 0;
    }

    printf("========tuya_p2p_event %d==========\n", event);
    switch (event)
    {
    case TRANS_LIVE_VIDEO_START:
    {
        // printf("========tuya_ipc_get_client_online_num### %d\n",tuya_ipc_get_client_online_num());
        // printf("===================######tuya_app_preview_check#####======%d\n",tuya_app_preview_check());

        if (tuya_app_preview_check() == false)
        {
            tuya_api_ringbuffer_clear();
            tuya_p2p_ringbuffer_clean();
        }
        lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_START, 0);

        C2C_TRANS_CTRL_VIDEO_START *parm = (C2C_TRANS_CTRL_VIDEO_START *)args;
        printf("chn[%u] video start type:%d \n", parm->channel, parm->type);
    }
    break;
    case TRANS_LIVE_VIDEO_STOP:
    {
        // printf("========tuya_ipc_get_client_online_num %d\n",tuya_ipc_get_client_online_num());
        // printf("===================######tuya_app_preview_check======%d\n",tuya_app_preview_check());
        // if ((tuya_ipc_get_client_online_num() > 1)&&(tuya_app_preview_check() == true))
        //{
        //	break;
        //}
        // unsigned char data[1024] =  {0,0,0,1,0X67,0,0,0,1,0X68,0x00,0x00,0x00,0x01,0x65};
        // tuya_ipc_ring_buffer_append_data(E_CHANNEL_VIDEO_MAIN, data, 1024, E_VIDEO_I_FRAME, user_timestamp_get());
        // tuya_ipc_ring_buffer_append_data(E_CHANNEL_AUDIO, data, 1024, E_CHANNEL_AUDIO, user_timestamp_get());
#if 0
		unsigned char data[1024] = {0x00,0x00,0x00,0x01,0x65};
		tuya_ipc_ring_buffer_append_data(E_CHANNEL_VIDEO_MAIN, data, 1024, E_VIDEO_I_FRAME, user_timestamp_get());	
		tuya_ipc_ring_buffer_append_data(E_CHANNEL_AUDIO, data, 1024, E_AUDIO_FRAME, user_timestamp_get());
#endif
        //  printf("===================######intercom_data_busy_enbale\n");
        lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_VIDEO_STOP, 0);
        // tuya_p2p_ringbuffer_clean();
    }
    break;
    case TRANS_SPEAKER_START:
    {
        tuya_receive_audio_first = true;
        lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_AUDIO_START, 0);
    }
    break;
    case TRANS_PLAYBACK_QUERY_MONTH_SIMPLIFY:

    {

        C2C_TRANS_QUERY_PB_MONTH_REQ *p = (C2C_TRANS_QUERY_PB_MONTH_REQ *)args;

        printf("pb query by month: %d-%d", p->year, p->month);

        OPERATE_RET ret = tuya_ipc_pb_query_by_month(p->year, p->month, &(p->day));

        if (OPRT_OK != ret)

        {

            printf("pb query by month: %d-%d ret:%d", p->year, p->month, ret);
        }

        break;
    }

    case TRANS_PLAYBACK_QUERY_DAY_TS:

    {

        C2C_TRANS_QUERY_PB_DAY_RESP *pquery = (C2C_TRANS_QUERY_PB_DAY_RESP *)args;

        printf("pb_ts query by day: idx[%d]%d-%d-%d", pquery->channel, pquery->year, pquery->month, pquery->day);

        SS_QUERY_DAY_TS_ARR_S *p_day_ts = NULL;

        OPERATE_RET ret = tuya_ipc_pb_query_by_day(pquery->channel, pquery->year, pquery->month, pquery->day, &p_day_ts);

        if (OPRT_OK != ret)

        {

            printf("pb_ts query by day: %d-%d-%d-%d Fail", pquery->channel, pquery->year, pquery->month, pquery->day);

            break;
        }

        if (p_day_ts)
        {

            printf("%s %d count = %d\n", __FUNCTION__, __LINE__, p_day_ts->file_count);

            PLAY_BACK_ALARM_INFO_ARR *pResult = (PLAY_BACK_ALARM_INFO_ARR *)malloc(sizeof(PLAY_BACK_ALARM_INFO_ARR) + p_day_ts->file_count * sizeof(PLAY_BACK_ALARM_FRAGMENT));

            if (NULL == pResult)
            {

                printf("%s %d malloc failed \n", __FUNCTION__, __LINE__);

                free(p_day_ts);

                pquery->alarm_arr = NULL;

                break;
            }

            INT_T i;

            pResult->file_count = p_day_ts->file_count;

            for (i = 0; i < p_day_ts->file_count; i++)
            {

                pResult->file_arr[i].type = p_day_ts->file_arr[i].type;

                pResult->file_arr[i].time_sect.start_timestamp = p_day_ts->file_arr[i].start_timestamp;

                pResult->file_arr[i].time_sect.end_timestamp = p_day_ts->file_arr[i].end_timestamp;
            }

            pquery->alarm_arr = pResult;

            free(p_day_ts);
        }
        else
        {

            pquery->alarm_arr = NULL;
        }

        break;
    }

    case TRANS_PLAYBACK_START_TS:

    {

        /* Client will bring the start time when playback.

        For the sake of simplicity, only log printing is done. */

        C2C_TRANS_CTRL_PB_START *pParam = (C2C_TRANS_CTRL_PB_START *)args;

        printf("PB StartTS idx:%d %u [%u %u]", pParam->channel, pParam->playTime, pParam->time_sect.start_timestamp, pParam->time_sect.end_timestamp);

        SS_FILE_TIME_TS_S pb_file_info;

        int ret;

        memset(&pb_file_info, 0x00, sizeof(SS_FILE_TIME_TS_S));

        // memcpy(&pb_file_info, &pParam->time_sect, sizeof(SS_FILE_TIME_TS_S));

        pb_file_info.start_timestamp = pParam->time_sect.start_timestamp;

        pb_file_info.end_timestamp = pParam->time_sect.end_timestamp;

        ret = tuya_ipc_ss_pb_start(pParam->channel, __TUYA_APP_ss_pb_event_cb, __TUYA_APP_ss_pb_get_video_cb, __TUYA_APP_ss_pb_get_audio_cb);

        if (0 != ret)
        {

            printf("%s %d pb_start failed\n", __FUNCTION__, __LINE__);

            tuya_ipc_playback_send_finish(pParam->channel);
        }
        else
        {

            if (0 != tuya_ipc_ss_pb_seek(pParam->channel, &pb_file_info, pParam->playTime))
            {

                printf("%s %d pb_seek failed\n", __FUNCTION__, __LINE__);

                tuya_ipc_playback_send_finish(pParam->channel);
            }
        }

        break;
    }

    case TRANS_PLAYBACK_PAUSE:

    {

        C2C_TRANS_CTRL_PB_PAUSE *pParam = (C2C_TRANS_CTRL_PB_PAUSE *)args;

        printf("PB Pause idx:%d", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_PAUSE);

        break;
    }

    case TRANS_PLAYBACK_RESUME:

    {

        C2C_TRANS_CTRL_PB_RESUME *pParam = (C2C_TRANS_CTRL_PB_RESUME *)args;

        printf("PB Resume idx:%d", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_RESUME);

        break;
    }

    case TRANS_PLAYBACK_MUTE:

    {

        C2C_TRANS_CTRL_PB_MUTE *pParam = (C2C_TRANS_CTRL_PB_MUTE *)args;

        printf("PB idx:%d mute", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_MUTE);

        break;
    }

    case TRANS_PLAYBACK_UNMUTE:

    {

        C2C_TRANS_CTRL_PB_UNMUTE *pParam = (C2C_TRANS_CTRL_PB_UNMUTE *)args;

        printf("PB idx:%d unmute", pParam->channel);

        tuya_ipc_ss_pb_set_status(pParam->channel, SS_PB_UN_MUTE);

        break;
    }

    case TRANS_PLAYBACK_STOP:

    {

        C2C_TRANS_CTRL_PB_STOP *pParam = (C2C_TRANS_CTRL_PB_STOP *)args;

        printf("PB Stop idx:%d", pParam->channel);

        tuya_ipc_ss_pb_stop(pParam->channel);

        break;
    }
    case TRANS_STREAMING_VIDEO_START:
    {
        TRANSFER_SOURCE_TYPE_E *pSrcType = (TRANSFER_SOURCE_TYPE_E *)args;
        printf("streaming start type %d", *pSrcType);
        break;
    }
    case TRANS_STREAMING_VIDEO_STOP:
    {
        TRANSFER_SOURCE_TYPE_E *pSrcType = (TRANSFER_SOURCE_TYPE_E *)args;
        printf("streaming stop type %d", *pSrcType);
        break;
    }
    default:

        break;
    }
    return 0;
}
/***
**   日期:2022-06-07 14:34:10
**   作者: leo.liu
**   函数作用：p2p接口
**   参数说明:
***/
void tuya_p2p_transfer_enable(void)
{
    TUYA_IPC_TRANSFER_VAR_S p2p_var = {0};
    memset(&p2p_var, 0, sizeof(TUYA_IPC_TRANSFER_VAR_S));
    p2p_var.online_cb = __depereated_online_cb;
    p2p_var.on_rev_audio_cb = tuya_device_audio_receive_cb;

    p2p_var.rev_audio_codec = TUYA_CODEC_AUDIO_G711U; // TUYA_CODEC_AUDIO_PCM;
    p2p_var.audio_sample = TUYA_AUDIO_SAMPLE_8K;
    p2p_var.audio_databits = TUYA_AUDIO_DATABITS_16;
    p2p_var.audio_channel = TUYA_AUDIO_CHANNEL_MONO;
    p2p_var.on_event_cb = tuya_p2p_event_cb;
    p2p_var.live_quality = TRANS_LIVE_QUALITY_MAX; // TRANS_LIVE_QUALITY_MAX;
    p2p_var.defLiveMode = TRANS_DEFAULT_HIGH;
    p2p_var.max_client_num = 4;

    IPC_MEDIA_INFO_S ringbuffer;
    extern void tuya_ringbuffer_init(IPC_MEDIA_INFO_S * ringbuffer);
    tuya_ringbuffer_init(&ringbuffer);
    memcpy(&p2p_var.AVInfo, &ringbuffer, sizeof(IPC_MEDIA_INFO_S));
    tuya_ipc_tranfser_init(&p2p_var);
    printf("===================tuya p2p transfer enable \n");
}

/************************************************************************
 *
 * 录像相关函数
 *
 * ***********************************************************************/
#include "tuya_ipc_stream_storage.h"
#include "user_file.h"
/***********************************************
** 作者: leo.liu
** 日期: 2022-11-5 17:6:55
** 说明: 获取sd 卡状态
***********************************************/
E_SD_STATUS tuya_ipc_sd_get_status(VOID)
{
    if (media_sdcard_insert_check() == true)
    {
        return SD_STATUS_NORMAL;
    }
    if (media_format_sd_state() == true)
    {

        return SD_STATUS_FORMATING;
    }

    return SD_STATUS_NOT_EXIST;

    //   return ((media_sdcard_insert_check())||(media_format_sd_state() == true)) ? SD_STATUS_NORMAL: SD_STATUS_NOT_EXIST;
}

/***********************************************
** 作者: leo.liu
** 日期: 2022-11-5 17:9:30
** 说明: 获取sd挂载路径
***********************************************/
CHAR_T *tuya_ipc_get_sd_mount_path(VOID)
{
    return TUYA_IPC_STREAM_BASE_PATH;
}

/***********************************************
** 作者: leo.liu
** 日期: 2022-11-5 17:10:35
** 说明: 获取sd卡使用信息
***********************************************/
#include "tuya_cloud_com_defs.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
VOID tuya_ipc_sd_get_capacity(UINT_T *p_total, UINT_T *p_used, UINT_T *p_free)
{
   // UINT_T total, used, free, reserved_size;
   // bool reslut = false;

    struct statfs diskinfo_p2;
    statfs("/mnt/tuya", &diskinfo_p2);
    unsigned long long total_blocks_p2 = diskinfo_p2.f_bsize;

    *p_total = (diskinfo_p2.f_blocks * total_blocks_p2);
    *p_free = (diskinfo_p2.f_bavail * total_blocks_p2); 
    *p_used = (*p_total) - (*p_free); 
  
#if 0
    extern void get_SD_space(unsigned long  *bavail,unsigned long  *disk_all_space);
	get_SD_space(&bavail,&disk_all_space);

	empty = bavail * 1024;
	total = disk_all_space* 1024;
	used = total - empty;
/*
    *p_total = *p_used = *p_free = 0;
    reslut = media_sd_memory_query(&total, &used, &free);
    if (reslut == false)
    {
        return;
    }
*/
    reserved_size = total * 1.0 / 3;
    int temp = free - reserved_size;
    *p_total = total - reserved_size;
    *p_free = temp < 0 ? 0 : temp;
    *p_used = (*p_total) - (*p_free);
#endif
    // printf("============= tuya sd disk:total:%0.2fM/ free:%0.2fM  Reserved size:%0.fM =============\n", (*p_total) / (1024.0), (*p_free) / (1024.0), (*p_total)  / 1024.0);
}

/***********************************************
** 作者: leo.liu
** 日期: 2022-11-7 13:47:19
** 说明: 涂鸦记录写入模式
***********************************************/
STREAM_STORAGE_WRITE_MODE_E tuya_ipc_sd_get_mode_config(void)
{
    // printf("=====%s:%d =======\n",__func__,__LINE__);
    return SS_WRITE_MODE_EVENT;
}

/***********************************************
** 作者: leo.liu
** 日期: 2022-11-7 13:48:17
** 说明: 移除sdcard 挂载
***********************************************/
void tuya_ipc_sd_remount(void)
{
    printf("=====%s:%d =======\n", __func__, __LINE__);
}
/***********************************************
** 作者: WQS
** 日期: 2022-12-8 8:48:17
** 说明: 涂鸦sdcard 格式化
***********************************************/

#include "ak_thread.h"
extern VOID IPC_APP_report_sd_format_status(INT_T status);
extern VOID handle_DP_SD_STORAGE_ONLY_GET(IN TY_OBJ_DP_S *p_obj_dp);
STATIC INT_T s_sd_format_progress = 0;
INT_T IPC_APP_get_sd_format_status(VOID)
{
    return s_sd_format_progress;
}
void *thread_sd_format(void *arg)
{
    /* First notify to app, progress 0% */
    if (media_sdcard_insert_check() == false)
    {
        ak_thread_exit();
        return NULL;
    }
    s_sd_format_progress = 0;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    usleep(100);

    /* Stop local SD card recording and playback, progress 10%*/
    s_sd_format_progress = 10;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_NONE);
    tuya_ipc_ss_pb_stop_all();
    usleep(100);

    /* Delete the media files in the SD card, the progress is 30% */
    s_sd_format_progress = 30;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    tuya_ipc_ss_delete_all_files();
    usleep(100);

    /* Perform SD card formatting operation */
    // printf("%s ===============>>%d:%d\r\n",__func__,media_format_sd_state(),media_sdcard_insert_check());
    media_format_sd();
    while (media_format_sd_state())
    {
        usleep(100);
    }
    printf("%s #############===============>>%d:%d\r\n", __func__, media_format_sd_state(), media_sdcard_insert_check());
    s_sd_format_progress = 80;
    IPC_APP_report_sd_format_status(s_sd_format_progress);
    // TODO
    //     IPC_APP_set_sd_record_onoff( IPC_APP_get_sd_record_onoff());

    /* progress 100% */
    usleep(100);
    s_sd_format_progress = 100;
    IPC_APP_report_sd_format_status(s_sd_format_progress);

    tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_EVENT);

    // handle_DP_SD_STORAGE_ONLY_GET(NULL);
    printf("format sd_card finish!!!!\r\n");

    ak_thread_exit();
    return NULL;
}
VOID IPC_APP_format_sd_card(VOID)
{
    printf("start to format sd_card \r\n");
    /* SD card formatting.
     * The SDK has already completed the writing of some of the code,
     and the developer only needs to implement the formatting operation. */

    ak_pthread_t sd_format_thread;
    ak_thread_create(&sd_format_thread, thread_sd_format, NULL, ANYKA_THREAD_NORMAL_STACK_SIZE, -1);
    ak_thread_detach(sd_format_thread);
}