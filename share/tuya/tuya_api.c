#include "tuya_api.h"
#include <pthread.h>
#include <string.h>
#include "tuya_ipc_api.h"
#include "wifi_api.h"
#include "tuya_uuid_and_key.h"
#include "tuya_cloud_wifi_defs.h"
#include "tuya_cloud_base_defs.h"
#include "tuya_ipc_media.h"
#include "tuya_ring_buffer.h"
#include "lv_msg_event.h"
#include "unistd.h"
#include "tuya_ipc_p2p.h"
#include "user_common.h"
#include "info.h"
#include "user_data.h"
#include "tuya_ipc_stream_storage.h"
#include "user_intercom.h"
#include "layout_define.h"
static char tuya_pid_string[64] = {0};
static bool is_tuya_online = false;
static tuya_api_weather tuya_api_werather_const = {0};

#define TUYA_DP_SD_STORAGE_ONLY_GET (109)
#define TUYA_DP_SD_STATUS_ONLY_GET (110)
#define TUYA_DP_SD_FORMAT (111)
#define TUYA_DP_SD_FORMAT_STATUS_ONLY_GET (117)

STATIC VOID respone_dp_str(BYTE_T dp_id, CHAR_T *p_val_str)
{
	tuya_ipc_dp_report(NULL, dp_id, PROP_STR, p_val_str, 1);
}
STATIC VOID respone_dp_value(BYTE_T dp_id, INT_T val)
{
	tuya_ipc_dp_report(NULL, dp_id, PROP_VALUE, &val, 1);
}

VOID handle_DP_SD_STORAGE_ONLY_GET(IN TY_OBJ_DP_S *p_obj_dp)
{
	CHAR_T tmp_str[100] = {0};

	UINT_T total;

	UINT_T used;

	UINT_T empty;

	unsigned long bavail, disk_all_space;
	// unsigned int empty = 0,total = 0,used = 0;
	extern void get_SD_space(unsigned long *bavail, unsigned long *disk_all_space);
	get_SD_space(&bavail, &disk_all_space);

	empty = bavail * 1024;
	total = disk_all_space * 1024;
	used = total - empty;
	// extern  VOID tuya_ipc_sd_get_capacity(UINT_T * p_total, UINT_T * p_used, UINT_T * p_free);

	// tuya_ipc_sd_get_capacity(&total, &used, &empty);

	//"total capacity|Current usage|remaining capacity"
	printf("====total%u=====empty%u====used%u\n", total, empty, used);
	snprintf(tmp_str, 100, "%u|%u|%u", total, used, empty);

	respone_dp_str(TUYA_DP_SD_STORAGE_ONLY_GET, tmp_str);
}

/*************************************************************************
 * @brief  涂鸦获取SD卡格式化进度
 * @date   2022-12-7 17:31
 * @author WQS
 **************************************************************************/
VOID IPC_APP_report_sd_format_status(INT_T status)
{
	printf("IPC_APP_report_sd_format_status :%d%%\n", status);
	tuya_ipc_dp_report(NULL, TUYA_DP_SD_FORMAT_STATUS_ONLY_GET, PROP_VALUE, &status, 1);
}
/*************************************************************************
 * @brief  涂鸦获取SD卡格式化进度
 * @date   2022-12-7 17:31
 * @author WQS
 **************************************************************************/
STATIC VOID handle_DP_SD_FORMAT_STATUS_ONLY_GET(IN TY_OBJ_DP_S *p_obj_dp)
{
	extern INT_T IPC_APP_get_sd_format_status(VOID);
	INT_T progress = IPC_APP_get_sd_format_status();
	tuya_ipc_dp_report(NULL, TUYA_DP_SD_FORMAT_STATUS_ONLY_GET, PROP_VALUE, &progress, 1);
}
/*************************************************************************
 * @brief  涂鸦下发格式化SD卡
 * @date   2022-12-7 17:31
 * @author WQS
 **************************************************************************/
STATIC VOID handle_DP_SD_FORMAT(IN TY_OBJ_DP_S *p_obj_dp)
{
	extern VOID IPC_APP_format_sd_card(VOID);
	IPC_APP_format_sd_card();
	BOOL_T param_1 = 1;
	tuya_ipc_dp_report(NULL, TUYA_DP_SD_FORMAT, PROP_BOOL, &param_1, 1);
}

void tuya_dp_home_mode_change(TY_OBJ_DP_S *dp, int mode)
{
	if (dp == NULL || (dp->type != PROP_ENUM))
	{
		LOG_RED("Error! type invalid %d \n\r", dp->type);
		return;
	}

	LOG_WHITE("enum=%d\n", dp->value.dp_enum);
	extern bool tuya_event_cmd_at_mode();
	extern bool tuya_event_cmd_leave_mode();
	extern bool tuya_event_cmd_sleep_mode();
	switch (mode)
	{
	case 0:

		tuya_event_cmd_at_mode();
		break;
	case 1:

		tuya_event_cmd_leave_mode();
		break;
	case 2:

		tuya_event_cmd_sleep_mode();
		break;

	default:
		tuya_event_cmd_at_mode();
		break;
	}
}

OPERATE_RET IPC_APP_get_file_data_cb(IN CONST FW_UG_S *fw,

									 IN CONST UINT_T total_len,

									 IN CONST UINT_T offset,

									 IN CONST BYTE_T *data,

									 IN CONST UINT_T len,

									 OUT UINT_T *remain_len,

									 IN PVOID_T pri_data)

{

	printf("Rev File Data:\n\r");

	printf("total_len:%d  fw_url:%s\n\r", total_len, fw->fw_url);

	printf("Offset:%d Len:%d\n\r", offset, len);

	// report UPGRADE process, NOT only download percent, consider flash-write time

	// APP will report overtime fail, if uprgade process is not updated within 60 seconds

	int download_percent = (offset * 100) / (total_len + 1);

	int report_percent = download_percent; // as an example, download 100% = 50%  upgrade work finished

	tuya_ipc_upgrade_progress_report(report_percent);

	FILE *p_upgrade_fd = (FILE *)pri_data;

	if (p_upgrade_fd)

	{

		fwrite(data, 1, len, p_upgrade_fd);
	}

	// APP will report "uprage success" after reboot and new FW version is reported inside SDK automaticlly

	return OPRT_OK;
}

VOID IPC_APP_upgrade_notify_cb(IN CONST FW_UG_S *fw,

							   IN CONST INT_T download_result,

							   IN PVOID_T pri_data)

{

	FILE *p_upgrade_fd = (FILE *)pri_data;

	if (p_upgrade_fd)

	{

		fclose(p_upgrade_fd);
	}

	printf("Upgrade Finish\n\r");

	printf("download_result:%d fw_url:%s\n\r", download_result, fw->fw_url);

	if (download_result == 0)

	{

		/* The developer needs to implement the operation of OTA upgrade,

		when the OTA file has been downloaded successfully to the specified path. [ p_mgr_info->upgrade_file_path ]*/
	}

	// system("cp /etc/config/wpa_supplicant.conf /app/data");

	// system("killall sh leo_auto_run.sh");

	// system("killall CDV1004QT.BIN");

	system("reboot");

	// TODO

	// reboot system
}

/***
**   日期:2022-06-07 10:44:05
**   作者: leo.liu
**   函数作用：满足tuya接口需求，暂未用到
**   参数说明:
***/
static void tuya_dp_query_func(IN CONST TY_DP_QUERY_S *dp_query)
{
}
/***
**   日期:2022-06-07 10:45:18
**   作者: leo.liu
**   函数作用：升级接口，暂未用到
**   参数说明:
***/
static void tuya_upgrade_func(CONST FW_UG_S *fw)
{

	printf("Rev Upgrade Info \n\r");

	printf("fw->fw_url:%s \n\r", fw->fw_url);

	printf("fw->fw_md5:%s \n\r", fw->fw_md5);

	printf("fw->sw_ver:%s \n\r", fw->sw_ver);

	printf("fw->file_size:%u \n\r", fw->file_size);

	printf("fw->fw_hmac:%s \n\r", fw->fw_hmac);

	FILE *p_upgrade_fd = fopen("/mnt/tf/CDV1004QTOS", "w+b");

	tuya_ipc_upgrade_sdk(fw, IPC_APP_get_file_data_cb, IPC_APP_upgrade_notify_cb, p_upgrade_fd);
}
/***
**   日期:2022-06-07 10:47:29
**   作者: leo.liu
**   函数作用：重启接口，暂未用到
**   参数说明:
***/
static void tuya_sdk_restart_func(void)
{
}

/***
**   日期:2022-06-07 19:30:28
**   作者: leo.liu
**   函数作用：通道切换处理
**   参数说明:
***/
/************************************************************************************************************
下发指令的格式为：
{"cmd":1,"cc":1,"chs":[{"id":1,"n":"door1"},{"id":2,"n":"door2"},{"id":3,"n":"CCTV1"},{"id":4,"n":"CCTV2"}]}

反馈指令的格式为:
"{\\\"res\\\":1,\\\"err\\\":0,\\\"cc\\\":1,\\\"chs\\\":[{\\\"id\\\":1,\\\"n\\\":\\\"door1\\\"},{\\\"id\\\":2,\\\"n\\\":\\\"door2\\\"},{\\\"id\\\":3,\\\"n\\\":\\\"CCTV1\\\"},{\\\"id\\\":4,\\\"n\\\":\\\"CCTV2\\\"}]}\"}"
************************************************************************************************************/

/***
**   日期:2022-06-07 10:43:19
**   作者: leo.liu
**   函数作用：dp处理接口
**   参数说明:
***/
static void tuya_dp_handle_func(IN CONST TY_RECV_OBJ_DP_S *dp_rev)
{
	TY_OBJ_DP_S *dp_data = (TY_OBJ_DP_S *)(dp_rev->dps);
	extern bool h264_encode_sample_get();
	if ((h264_encode_sample_get()) && (dp_data->dpid != 232) && (dp_data->dpid != 237) && (dp_data->dpid != 236) && (dp_data->dpid != 240))
	{
		return;
	}

#if 0 /* 是否开启dp点调试模式 */
	LOG_BLUE("dpid: %d type: %d time: %d \n", dp_data->dpid, dp_data->type, dp_data->time_stamp);
	if (dp_data->type == PROP_BOOL)
    {
        LOG_BLUE("type bool: %d \033[37m \r\n", dp_data->value.dp_bool);
    }
    else if (dp_data->type == PROP_VALUE)
    {
        LOG_BLUE("type value: %d \033[37m \r\n", dp_data->value.dp_value);
    }
    else if (dp_data->type == PROP_STR)
    {
        LOG_BLUE("type str: %s \033[37m \r\n", dp_data->value.dp_str);
    }
    else if (dp_data->type == PROP_ENUM)
    {
        LOG_BLUE("type enmu: %d \033[37m \r\n", dp_data->value.dp_enum);
    }
#endif

	switch (dp_data->dpid)
	{
	case TUYA_DP_SD_STORAGE_ONLY_GET:
	{
		LOG_BLUE("############### TUYA_DP_SD_STORAGE_ONLY_GET ###############\n");
		handle_DP_SD_STORAGE_ONLY_GET(NULL);
		break;
	}
	case TUYA_DP_SD_STATUS_ONLY_GET:
	{
		printf("TUYA_DP_SD_STATUS_ONLY_GET =================+++>\n");
		respone_dp_value(TUYA_DP_SD_STATUS_ONLY_GET, (INT_T)tuya_ipc_sd_get_status());
		break;
	}
	case TUYA_DP_SD_FORMAT:
	{
		handle_DP_SD_FORMAT(NULL);
		break;
	}
	case TUYA_DP_SD_FORMAT_STATUS_ONLY_GET:
	{
		handle_DP_SD_FORMAT_STATUS_ONLY_GET(NULL);
		break;
	}
	case 231:
	{
		/***** 切换通道 *****/
		LOG_BLUE("############### dp:%d 切换通道 ###############\n", dp_data->dpid);
		char *down_head_str = {"{\"cmd\":1,\"cc\":"};
		if (strncmp(dp_data->value.dp_str, down_head_str, strlen(down_head_str)) != 0)
		{
			LOG_RED("String error \n\r");
			return;
		}
		int channel_id = dp_data->value.dp_str[strlen(down_head_str)] - 48;
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_CH_CHANGE, channel_id);
		break;
	}
	case 240:
	{
		/*****  移动侦测开关 *****/
		LOG_BLUE("############### dp:%d 移动侦测开关 ###############\n", dp_data->dpid);
		tuya_ipc_dp_report(NULL, dp_data->dpid, PROP_BOOL, &(dp_data->value.dp_bool), 1);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_MOTION_ENBALE, dp_data->value.dp_bool);
		break;
	}
	case 148:
	{
		/*****  开锁 *****/
		LOG_BLUE("############### dp:%d 开锁 ###############\n", dp_data->dpid);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_DOOR_OPEN, dp_data->value.dp_bool);
		break;
	}
	// case 232:
	// {
	// 	LOG_BLUE("############### dp:%d 离家模式 ###############\n", dp_data->dpid);
	// 	lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_LEAVE_MODE, dp_data->value.dp_bool);
	// 	break;
	// }
	// case 236:
	// {
	// 	LOG_BLUE("############### dp:%d 居家模式 ###############\n", dp_data->dpid);
	// 	lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_AT_MODE, dp_data->value.dp_bool);
	// 	break;
	// }
	// case 237:
	// {
	// 	LOG_BLUE("############### dp:%d 睡眠模式 ###############\n", dp_data->dpid);
	// 	lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_SLEEP_MODE, dp_data->value.dp_bool);
	// 	break;
	// }
	// case 240:
	// {
	// 	LOG_BLUE("############### dp:%d 移动侦测开关 ###############\n", dp_data->dpid);
	// 	lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_AUTO_MONITOR, dp_data->value.dp_bool);
	// 	break;
	// }
	case 159:
	{
		LOG_BLUE("############### dp:%d 警报开关 ###############\n", dp_data->dpid);
		// tuya_ipc_dp_report(NULL, dp_data->dpid, PROP_BOOL, &(dp_data->value.dp_bool), 1);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_ALAM_SWITCH, dp_data->value.dp_bool);
		break;
	}
	case 162:
	{
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_REBOOT, dp_data->value.dp_bool);
		break;
	}

	case 239:
	{
		LOG_BLUE("############### dp:%d 车库门锁 ###############\n", dp_data->dpid);
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_DOOR_OPEN_2, dp_data->value.dp_bool);
		break;
	}
	case 241:
	{
		LOG_WHITE(" situational_model enum:%d\n", dp_data->value.dp_enum);
		extern void tuya_dp_home_mode_change(TY_OBJ_DP_S * dp, int mode);
		tuya_dp_home_mode_change(dp_data, dp_data->value.dp_enum);
		break;
	}
	default:
	{
		LOG_RED("############### dp unkonw:%d ###############\n", dp_data->dpid);
		break;
	}
	}
}
/***
**   日期:2022-06-07 10:44:30
**   作者: leo.liu
**   函数作用：tuya状态改变接口
**   参数说明:
***/
static void tuya_status_channge_func(IN CONST BYTE_T stat)
{
	switch ((stat))
	{
#if defined(WIFI_GW) && (WIFI_GW == 1)
	case STAT_CLOUD_CONN:  // for wifi ipc
	case STAT_MQTT_ONLINE: // for low-power wifi ipc
	case GB_STAT_CLOUD_CONN:
#endif
#if defined(WIFI_GW) && (WIFI_GW == 0)
	case GB_STAT_CLOUD_CONN: // for wired ipc
#endif
		// case GB_STAT_LAN_CONN:
	{
		/*****  tuya 上线 *****/
		static bool tuya_p2p_enable = false;
		if (tuya_p2p_enable == false)
		{
			tuya_p2p_enable = true;
			extern void tuya_p2p_transfer_enable(void);
			tuya_p2p_transfer_enable();
			tuya_ipc_upload_skills();

			printf("=-======================\n");
		}
		lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_ONLINE, 0);
		is_tuya_online = true;
		printf("############### tuya  in-line###############\n");
		tuya_api_time_sync();
		tuya_api_home_mode_report(user_data_get()->home_mode);
		tuya_api_auto_monitor_report(user_data_get()->motion.enable);
		tuya_api_alam_report(user_data_get()->alarm.alarm_enable);
		tuya_dp_238_response_access_lock_support();
		extern bool tuya_api_sdcard_status_report(void);
		tuya_api_sdcard_status_report();
		handle_DP_SD_STORAGE_ONLY_GET(NULL);
	}
	break;
	case WF_START_AP_ONLY:
	case STAT_MQTT_OFFLINE:
	{
		if ((is_tuya_online == true) && (user_data_get()->wifi_enable))
		{
			// system("killall udhcpc");
			// system("rmmod hi3881");
			// system("rmmod ak_mci");
			// system("insmod /usr/modules/ak_mci.ko");
			// usleep(10*1000);
			// system("insmod /usr/modules/hi3881.ko");
			// usleep(10*1000);
			// system("udhcpc -i wlan0 -B &");
		}
		// if((is_tuya_online == true) && (user_data_get()->wifi_enable))
		// {
		// 	system("killall wpa_supplicant");
		// 	system("killall udhcpc");
		// 	system("killall udhcpd");
		// 	system("killall hostapd");
		// 	system("rmmod wifi_soc");
		// 	system("rmmod plat_soc");
		// 	system("rmmod ak_mci");
		// 	system("insmod /usr/modules/ak_mci.ko");
		// 	usleep(5*1000);
		// 	system("insmod /usr/modules/plat_soc.ko");
		// 	usleep(5*1000);
		// 	system("insmod /usr/modules/wifi_soc.ko");
		// 	system("wpa_supplicant -Dnl80211 -i wlan0 -c /etc/config/wpa_supplicant.conf -B");
		// 	system("udhcpc -i wlan0 &");
		// }
		/*****  tuya 离线 *****/
		is_tuya_online = false;
		printf("############### tuya  off-line###############\n");
	}
	break;
	default:
	{
		printf("############### status change unkonw:%d ###############\n", stat);
		break;
	}
	}
}
/***
**   日期:2022-06-15 17:26:18
**   作者: leo.liu
**   函数作用：重置tuya data
**   参数说明:
***/
void tuay_api_data_reset(void)
{
	remove(TUYA_UUID_AND_KEY_CONF_PATH "tuya_user.db_bak");
	remove(TUYA_UUID_AND_KEY_CONF_PATH "tuya_user.db");
	remove(TUYA_UUID_AND_KEY_CONF_PATH "tuya_enckey.db");

	remove(TUYA_UUID_AND_KEY_CONF_PATH "logseq0");
	remove(TUYA_UUID_AND_KEY_CONF_PATH "log_seq_stat");
	system("sync");
}
/***
**   日期:2022-06-07 10:46:35
**   作者: leo.liu
**   函数作用：删除tuya配置文件
**   参数说明:
***/
static void tuya_reset_system_func(GW_RESET_TYPE_E type)
{
	printf("tuya reset==============>>>>>\n");
	tuay_api_data_reset();
	printf("##### tuya unbundle \n");
	memset(user_data_get()->tuya_qrcode_info, 0, sizeof(user_data_get()->tuya_qrcode_info));
	// user_data_get()->tuya_qrcode_info = NULL;
#ifdef MAOTOUYING_DAN_IC
	system("killall CDV1004QT.BIN");

#else
	system("reboot");
#endif
	printf("tuya reset==============>>>>>11111111111\n");
}

/***
**   日期:2022-06-07 14:28:37
**   作者: leo.liu
**   函数作用：音视频循环队列初始化
**   参数说明:
***/
void tuya_ringbuffer_init(IPC_MEDIA_INFO_S *ringbuffer)
{
	memset(ringbuffer, 0, sizeof(IPC_MEDIA_INFO_S));
	ringbuffer->channel_enable[E_CHANNEL_VIDEO_MAIN] = TRUE;				 /* Whether to enable local HD video streaming */
	ringbuffer->video_fps[E_CHANNEL_VIDEO_MAIN] = 25;						 /* FPS */
	ringbuffer->video_gop[E_CHANNEL_VIDEO_MAIN] = 30;						 /* GOP */
	ringbuffer->video_bitrate[E_CHANNEL_VIDEO_MAIN] = TUYA_VIDEO_BITRATE_1M; /* Rate limit */
	ringbuffer->video_width[E_CHANNEL_VIDEO_MAIN] = 640;					 /* Single frame resolution of width*/
	ringbuffer->video_height[E_CHANNEL_VIDEO_MAIN] = 360;					 /* Single frame resolution of height */
	ringbuffer->video_freq[E_CHANNEL_VIDEO_MAIN] = 90000;					 /* Clock frequency */
	ringbuffer->video_codec[E_CHANNEL_VIDEO_MAIN] = TUYA_CODEC_VIDEO_H264;
	// TUYA_CODEC_VIDEO_H264; // TUYA_CODEC_VIDEO_MJPEG;
	//  TUYA_CODEC_VIDEO_H264; /* Encoding format */

	ringbuffer->channel_enable[E_CHANNEL_AUDIO] = TRUE;					  /* Whether to enable local sound collection */
	ringbuffer->audio_codec[E_CHANNEL_AUDIO] = TUYA_CODEC_AUDIO_G711U;	  // TUYA_CODEC_AUDIO_PCM; //TUYA_CODEC_AUDIO_PCM /* Encoding format */
	ringbuffer->audio_sample[E_CHANNEL_AUDIO] = TUYA_AUDIO_SAMPLE_8K;	  // TUYA_AUDIO_SAMPLE_8K /* Sampling Rate */
	ringbuffer->audio_databits[E_CHANNEL_AUDIO] = TUYA_AUDIO_DATABITS_16; /* Bit width */
	ringbuffer->audio_channel[E_CHANNEL_AUDIO] = TUYA_AUDIO_CHANNEL_MONO; // TUYA_AUDIO_CHANNEL_MONO;/* channel */
	ringbuffer->audio_fps[E_CHANNEL_AUDIO] = 25;						  /* Fragments per second */
}

/***
**   日期:2022-06-07 10:23:20
**   作者: leo.liu
**   函数作用：涂鸦回放功能实现
**   参数说明:
***/
#include "tuya_ipc_stream_storage.h"

void TUYA_APP_Init_Stream_Storage(void)
{
	IPC_MEDIA_INFO_S ringbuffer;
	tuya_ringbuffer_init(&ringbuffer);
	tuya_ipc_ss_init(TUYA_IPC_STREAM_BASE_PATH, &ringbuffer, 1440, NULL); // 事件多的话加载不出来
	tuya_ipc_ss_set_write_mode(SS_WRITE_MODE_EVENT);
	// tuya_ipc_ss_set_pre_record_time(10);
}

/***
**   日期:2022-06-07 10:23:20
**   作者: leo.liu
**   函数作用：tuya sdk初始化
**   参数说明:
***/
static void tuya_sdk_init(void)
{
	unsigned char uuid[128] = {0};
	unsigned char key[128] = {0};
	if (tuya_uuid_and_key_read(uuid, key) == false)
	{

		return;
	}
	printf("\n\n#####[uuid:%s] [key:%s]#####\n\n\n", uuid, key);

	IPC_MEDIA_INFO_S ringbuffer;
	tuya_ringbuffer_init(&ringbuffer);
	tuya_ipc_ring_buffer_init(E_CHANNEL_VIDEO_MAIN, ringbuffer.video_bitrate[E_CHANNEL_VIDEO_MAIN], ringbuffer.video_fps[E_CHANNEL_VIDEO_MAIN], 0, NULL);
	tuya_ipc_ring_buffer_init(E_CHANNEL_AUDIO, ringbuffer.audio_sample[E_CHANNEL_AUDIO] * ringbuffer.audio_databits[E_CHANNEL_AUDIO] / 1024, ringbuffer.audio_fps[E_CHANNEL_AUDIO], 0, NULL);

	/*****  tuya信息初始化 *****/
	TUYA_IPC_ENV_VAR_S env;
	memset(&env, 0, sizeof(TUYA_IPC_ENV_VAR_S));
	strcpy(env.storage_path, TUYA_UUID_AND_KEY_CONF_PATH);
	strcpy(env.product_key, tuya_pid_string);
	strcpy(env.uuid, (char *)uuid);
	strcpy(env.auth_key, (char *)key);
	strcpy(env.dev_sw_version, "1.2.3");
	strcpy(env.dev_serial_num, "tuya_ipc");

	/*****  dp回调处理接口 *****/
	env.dev_obj_dp_cb = tuya_dp_handle_func;
	env.dev_dp_query_cb = tuya_dp_query_func;
	/*****  tuya状态改变接口 *****/
	env.status_changed_cb = tuya_status_channge_func;
	env.gw_ug_cb = tuya_upgrade_func;
	env.gw_rst_cb = tuya_reset_system_func,
	env.gw_restart_cb = tuya_sdk_restart_func;
	env.mem_save_mode = false;
	/*****  初始化sdk *****/
	tuya_ipc_init_sdk(&env);
	tuya_ipc_set_region(REGION_CN);
	tuya_ipc_start_sdk(WIFI_INIT_NULL, NULL);
	TUYA_APP_Init_Stream_Storage();
}
/***
**   日期:2022-06-17 17:46:52
**   作者: leo.liu
**   函数作用：使能tuya 网络检测功能
**   参数说明:
***/
static bool tuya_network_detect_enabel = false;
bool tuya_api_network_detect_enable(bool en)
{
	tuya_network_detect_enabel = en;
	return true;
}
/*
 *	这个只是用于wifi的状态获取,ping www.tuya.com
 *
 *  0x00:网络没连接
 *   0x01:网络连接热点，但是不能访问外网。
 *   0x02:网络通畅
 */
static char tuya_netwrok_status = 0x00;
char tuya_api_network_status(void)
{
	return tuya_netwrok_status;
}
/***
**   日期:2022-06-17 17:51:32
**   作者: leo.liu
**   函数作用：ping 网址
**   参数说明:
***/
static char tuya_network_online_check(void)
{
	char on_line = 0x00;
#define DETECT_WAN0_PTAH "/sys/class/net/wlan0/operstate"
	int fd = open(DETECT_WAN0_PTAH, O_RDONLY);
	if (fd < 0)
	{
		return on_line;
	}
	char buffer[128] = {0};
	read(fd, buffer, 2);
	close(fd);

	if (strncmp(buffer, "up", 2))
	{
		return on_line;
	}
	on_line = 0x01;

#define PING_WWW "www.microsoft.com"
	FILE *pfd = popen("ping -W 2 -c 2 -i 0.9 " PING_WWW, "r");
	if (pfd == NULL)
	{
		printf("open ping " PING_WWW " fail \n");
		return on_line;
	}

	while (fgets(buffer, sizeof(buffer), pfd))
	{
		//	printf("%s \n",buffer);
		char *pstr = strstr(buffer, "ttl=");
		if (pstr != NULL)
		{
			on_line = 0x02;
			break;
		}
		memset(buffer, 0, sizeof(buffer));
	}
	pclose(pfd);
	// printf("======>>> wifi :%s \n",on_line?"online":"offline");
	return on_line;
}
/***
**   日期:2022-06-18 08:05:12
**   作者: leo.liu
**   函数作用：获取天气信息
**   参数说明:
***/
OPERATE_RET http_gw_ipc_custom_msg(IN CONST CHAR_T *api_name, IN CONST CHAR_T *api_version, IN CONST CHAR_T *message, OUT cJSON **result);
static bool _tuya_api_weather_get(tuya_api_weather *nm)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	/* 预报 */
	const char *weather_choose[] = {
		"{                              \
            \"codes\":[                 \
                \"w.date.1\",           \
                \"w.currdate\",         \
                \"w.conditionNum\",     \
                \"w.temp\",             \
                \"w.humidity\",         \
                \"w.pressure\",         \
                \"w.pm10\",             \
                \"w.pm25\",             \
                \"w.aqi\",              \
                \"w.thigh\",            \
                \"w.tlow\",             \
                \"c.city\",             \
                ]                       \
        }"

	};
	// "{\"codes\":[\"w.conditionNum\", \"w.temp\", \"w.humidity\", \"w.pressure\", \"w.pm10\", \"w.pm25\", \"w.aqi\"]}"
	cJSON *result = NULL;
	if (http_gw_ipc_custom_msg("tuya.device.public.data.get", "1.0", *weather_choose, &result))
	{
		return false;
	}

	cJSON *data_json = cJSON_GetArrayItem(result, 0);
	cJSON *condition_json = cJSON_GetObjectItem(data_json, "w.conditionNum");
	cJSON *temp_json = cJSON_GetObjectItem(data_json, "w.temp");
	cJSON *humidity_json = cJSON_GetObjectItem(data_json, "w.humidity");
	cJSON *pressure_json = cJSON_GetObjectItem(data_json, "w.pressure");
	cJSON *pm10_json = cJSON_GetObjectItem(data_json, "w.pm10");
	cJSON *pm25_json = cJSON_GetObjectItem(data_json, "w.pm25");
	cJSON *aqi_json = cJSON_GetObjectItem(data_json, "w.aqi");
	cJSON *thigh_json = cJSON_GetObjectItem(data_json, "w.thigh.0");
	cJSON *tlow_json = cJSON_GetObjectItem(data_json, "w.tlow.0");

	if (temp_json != NULL)
	{
		sscanf(cJSON_Print(temp_json), "%d", &(nm->temp));
	}
	if (condition_json != NULL)
	{
		sscanf(cJSON_Print(condition_json), "\"%d\"", &(nm->condition));
	}
	if (humidity_json != NULL)
	{
		sscanf(cJSON_Print(humidity_json), "%d", &(nm->humidity));
	}
	if (pressure_json != NULL)
	{
		sscanf(cJSON_Print(pressure_json), "%d", &(nm->pressure));
	}
	if (pm10_json != NULL)
	{
		sscanf(cJSON_Print(pm10_json), "%d", &(nm->pm10));
	}
	if (pm25_json != NULL)
	{
		sscanf(cJSON_Print(pm25_json), "%d", &(nm->pm25));
	}
	if (aqi_json != NULL)
	{
		sscanf(cJSON_Print(aqi_json), "%d", &(nm->aqi));
	}
	if (thigh_json != NULL)
	{
		sscanf(cJSON_Print(thigh_json), "%d", &(nm->thigh));
	}
	if (tlow_json != NULL)
	{
		sscanf(cJSON_Print(tlow_json), "%d", &(nm->tlow));
	}
	// printf("=============>>>>data_json====>%s\n", cJSON_Print(data_json));

	cJSON_Delete(result);
	return true;
}

char qr_code_info[128] = {0};
char *tuya_qr_code_info_get(void)
{
	if (qr_code_info[0] == 0)
		return NULL;
	// return qr_code_info;
	return user_data_get()->tuya_qrcode_info;
}

/***
**   日期:2022-06-07 10:19:09
**   作者: leo.liu
**   函数作用：tuya网络检测
**   参数说明:
***/
static void *tuya_api_task(void *arg)
{
	bool conneted = false;

	bool wifi_enable = *(bool *)arg;

	// int get_weather_count = 0;
	memset(qr_code_info, 0, 128);
	if ((wifi_device_connection_stauts(NULL, NULL, NULL, &conneted, NULL) == false) || (conneted == false))
	{
		wifi_device_conneting();
		wifi_device_open();
	}

	while ((wifi_device_connection_stauts(NULL, NULL, NULL, &conneted, NULL) == false) || (conneted == false) || tuya_network_online_check() != 0x02)
	{
		usleep(1000 * 1000 * 2);
	}

	system("echo wlan0 set tx pwr_offset 30 > /sys/hisys/hipriv");

	tuya_sdk_init();

	if (wifi_enable == false)
	{
		wifi_device_close();
	}

	while (1)
	{
		if (tuya_network_detect_enabel == true)
		{
			tuya_netwrok_status = tuya_network_online_check();
			if (tuya_netwrok_status == 0X02)
			{
				tuya_api_weather tmp_weather = {0};
				if ((_tuya_api_weather_get(&tmp_weather) == true) && ((tuya_api_werather_const.temp != tmp_weather.temp) || (tuya_api_werather_const.condition != tmp_weather.condition) || (tuya_api_werather_const.humidity != tmp_weather.humidity) || (tuya_api_werather_const.pressure != tmp_weather.pressure) || (tuya_api_werather_const.pm10 != tmp_weather.pm10) || (tuya_api_werather_const.pm25 != tmp_weather.pm25)))
				{
					tuya_api_werather_const = tmp_weather;
					// lv_msg_send_cmd(MSG_EVENT_CMD_TUYA, TUYA_EVENT_CMD_WEATHER_CHANGE, (tmp_weather.temp << 8) | tmp_weather.condition);
				}
				char serial[64] = {0};
				if (qr_code_info[0] == 0 && tuya_serial_number_get(serial) == true)
				{
					printf("=================>>> 二维码获取===========<<<<<<<\n");
					tuya_ipc_get_qrcode(NULL, qr_code_info, 128);
					if (qr_code_info[0] != 0)
					{
						// if(user_data_get()->tuya_qrcode_info == NULL)
						{
							memcpy(user_data_get()->tuya_qrcode_info, qr_code_info, 128);
							user_data_save();
						}

						printf("=================>>> 二维码获取成功:[%s]:[%s]\n", qr_code_info, user_data_get()->tuya_qrcode_info);
					}
				}
			}
			usleep(2000 * 1000);
		}
		else
		{
			usleep(2000 * 1000);
		}
	}
	return NULL;
}

tuya_api_weather tuya_weather_get(void)
{
	return tuya_api_werather_const;
}

/***
**   日期:2022-06-07 10:12:44
**   作者: leo.liu
**   函数作用：tuya api初始化
**   参数说明:
***/
bool tuya_api_init(const char *pid, bool *wifi_en)
{
	pthread_t thread_t;
	memset(tuya_pid_string, 0, sizeof(tuya_pid_string));
	strcpy(tuya_pid_string, pid);
	pthread_create(&thread_t, user_pthread_atter_get(), tuya_api_task, wifi_en);
	return true;
}

/***
**   日期:2022-06-07 15:52:37
**   作者: leo.liu
**   函数作用：获取tuya在线状态
**   参数说明:
***/
bool tuya_api_online_check(void)
{
	if (is_tuya_online == false)
	{
		return false;
	}

	return tuya_ipc_get_mqtt_status() ? true : false;
}

/*
 *
 *	dp233,用于设备端上报接收和挂断通话的dp点，因为app端下发通话和挂断已经有其他dp点使用，故不能作为其app下发的dp点。
 *
 *	1(字符串类型):接收通话
 *	   设备端上报到app:设备端已接受通话。
 *
 *	2(字符串类型):挂断通话
 *		设备端上报到app:设备端已挂断通话。
 *
 * 	整体逻辑:
 *		在门铃按下后，设备和app都能够同时预览门铃画面。
 *
 *		场景1:	在设备端按下挂断后->将上传dp233(字符串内容:2)，app端同步挂断。
 *
 *		场景2:	在设备端按下接收通话 ->将上传dp233(字符串内容:1)，app端应该是挂断操作（这个动作需客户端确认）
 *
 *	备注:在每次离开预览页面都会上报dp233+2。每次接受通话后都会上报dp233+1
 *
 */
/***
**   日期:2022-06-07 15:51:54
**   作者: leo.liu
**   函数作用：app退出监控
**   参数说明: 1(字符串类型):接收通话
**	   设备端上报到app:设备端已接受通话。
***/
bool tuya_api_preview_quit(void)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	tuya_ipc_dp_report(NULL, 233, PROP_STR, "1", 1);
	return true;
}
/***
**   日期:2022-06-07 15:58:20
**   作者: leo.liu
**   函数作用：app与
**   参数说明:2(字符串类型):挂断通话
**		设备端上报到app:设备端已挂断通话。
***/
bool tuya_api_monitor_handup(void)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	tuya_ipc_dp_report(NULL, 233, PROP_STR, "2", 1);
	return true;
}
/***
**   日期:2022-06-07 16:10:44
**   作者: leo.liu
**   函数作用：强制时间同步
**   参数说明:
***/
bool tuya_api_app_sync_utc_time(void)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	TIME_T time_utc;
	INT_T time_zone;
	tuya_ipc_get_service_time_force(&time_utc, &time_zone);
	return true;
}

#define SWITCH_CHANNEL_CMD_HEAD "{\\\"cmd\\\":1,\\\"cc\\\":%d,\\\"chs\\\":["
#define TUYA_CHANNEL_CMD_TAIL "]}\"}"

/***
**   日期:2022-06-07 18:15:39
**   作者: leo.liu
**   函数作用：上传有效通道
**   参数说明:channe:当前监控的通道
***/
bool tuya_api_channel_report(int cur,
							 bool door1_valild, const char *door1_str,
							 bool door2_valild, const char *door2_str,
							 bool cctv1_valid, const char *cctv1_str,
							 bool cctv2_valid, const char *cctv2_str,
							 bool cctv3_valid, const char *cctv3_str,
							 bool cctv4_valid, const char *cctv4_str)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	if (((door1_valild == false) && (door2_valild == false) && (cctv1_valid == false) && (cctv2_valid == false) && (cctv3_valid == false) && (cctv4_valid == false)))
	{
		return false;
	}
	char dp_str[512] = {0};
	cur = cur == 0 ? 1 : cur;
	sprintf(dp_str, SWITCH_CHANNEL_CMD_HEAD, cur);
	if (door1_valild == true)
	{
		char temp[128] = {0};
		sprintf(temp, "{\\\"id\\\":%d,\\\"n\\\":\\\"%s", 1, door1_str);
		if (cur == 1)
		{
			strcat(temp, " current");
		}
		strcat(temp, "\\\"},");
		strcat(dp_str, temp);
	}
	if (door2_valild == true)
	{
		char temp[128] = {0};
		sprintf(temp, "{\\\"id\\\":%d,\\\"n\\\":\\\"%s", 2, door2_str);
		if (cur == 2)
		{
			strcat(temp, " current");
		}
		strcat(temp, "\\\"},");
		strcat(dp_str, temp);
	}
	if (cctv1_valid == true)
	{
		char temp[128] = {0};
		sprintf(temp, "{\\\"id\\\":%d,\\\"n\\\":\\\"%s", 3, cctv1_str);
		if (cur == 3)
		{
			strcat(temp, " current");
		}
		strcat(temp, "\\\"},");
		strcat(dp_str, temp);
	}
	if (cctv2_valid == true)
	{
		char temp[128] = {0};
		sprintf(temp, "{\\\"id\\\":%d,\\\"n\\\":\\\"%s", 4, cctv2_str);
		if (cur == 4)
		{
			strcat(temp, " current");
		}
		strcat(temp, "\\\"},");
		strcat(dp_str, temp);
	}
	if (cctv3_valid == true)
	{
		char temp[128] = {0};
		sprintf(temp, "{\\\"id\\\":%d,\\\"n\\\":\\\"%s", 5, cctv3_str);
		if (cur == 5)
		{
			strcat(temp, " current");
		}
		strcat(temp, "\\\"},");
		strcat(dp_str, temp);
	}
	if (cctv4_valid == true)
	{
		char temp[128] = {0};
		sprintf(temp, "{\\\"id\\\":%d,\\\"n\\\":\\\"%s", 6, cctv4_str);
		if (cur == 6)
		{
			strcat(temp, " current");
		}
		strcat(temp, "\\\"},");
		strcat(dp_str, temp);
	}
	/*****************************
	以为结束的最后一个","是不需要。
	******************************/
	dp_str[strlen(dp_str) - 1] = '\0';
	strcat(dp_str, TUYA_CHANNEL_CMD_TAIL);
	tuya_ipc_dp_report(NULL, 231, PROP_STR, dp_str, 1);
	// printf("report :dp231( %s ) \n", dp_str);

	return true;
}

/***
**   日期:2022-06-08 08:26:18
**   作者: leo.liu
**   函数作用：使tuya重新连接
**   参数说明:
***/
bool tuya_api_reconnect_network(void)
{
	// if (tuya_api_online_check() == false)
	// {
	// 	return false;
	// }
	printf("=========================================>>>>>> WIFI reconnect\n");
	tuya_ipc_reconnect_wifi();
	return true;
}
/***
**   日期:2022-06-08 08:36:35
**   作者: leo.liu
**   函数作用：上传门2的开锁模式
**   参数说明:  "1"/"2"
***/
bool tuya_api_door2_unlock_mode_report(int mode)
{
	if (tuya_api_online_check() == false)
	{
		printf("###### device off line ###### \n");
		return false;
	}
	char dp_str[6] = {0};
	sprintf(dp_str, "%d", mode);
	tuya_ipc_dp_report(NULL, 234, PROP_STR, dp_str, 1);
	return true;
}
/***
**   日期:2022-06-08 08:50:31
**   作者: leo.liu
**   函数作用：开锁成功/失败反馈
**   参数说明:
***/
bool tuya_api_open_door_success_report(bool ok)
{
	if (tuya_api_online_check() == false)
	{
		printf("###### device off line ###### \n");
		return false;
	}
	BOOL_T ret = ok ? 1 : 0;
	tuya_ipc_dp_report(NULL, 239, PROP_BOOL, &ret, 1);
	return true;
}

bool tuya_api_open_outdoor_success_report(bool ok)
{
	if (tuya_api_online_check() == false)
	{
		printf("###### device off line ###### \n");
		return false;
	}
	BOOL_T ret = ok ? 1 : 0;
	tuya_ipc_dp_report(NULL, 148, PROP_BOOL, &ret, 1);
	return true;
}
/***
**   日期:2022-06-08 08:50:31
**   作者: leo.liu
**   函数作用：移动侦测开关
**   参数说明:
***/
bool tuya_api_auto_monitor_report(bool ok)
{
	if (tuya_api_online_check() == false)
	{
		printf("###### device off line ###### \n");
		return false;
	}
	BOOL_T ret = ok ? 1 : 0;
	tuya_ipc_dp_report(NULL, 240, PROP_BOOL, &ret, 1);
	return true;
}

/***
**   日期:2022-06-08 08:50:31
**   作者: leo.liu
**   函数作用：报警功能开关
**   参数说明:
***/
bool tuya_api_alam_report(bool ok)
{
	if (tuya_api_online_check() == false)
	{
		printf("###### device off line ###### \n");
		return false;
	}
	BOOL_T ret = ok ? 1 : 0;
	tuya_ipc_dp_report(NULL, 159, PROP_BOOL, &ret, 1);
	return true;
}
/***
**   日期:2022-06-08 08:50:31
**   作者: leo.liu
**   函数作用：上报SD卡状态
**   参数说明:
***/
bool tuya_api_sdcard_status_report(void)
{
	if (tuya_api_online_check() == false)
	{
		printf("###### device off line ###### \n");
		return false;
	}
	BOOL_T ret = tuya_ipc_sd_get_status() ? 1 : 5;
	tuya_ipc_dp_report(NULL, 110, PROP_BOOL, &ret, 1);
	return true;
}
/*************************************************************************
 * @brief  居家模式返回数据到云端
 * @date   2022-10-08 17:24
 * @author xiaoele
 **************************************************************************/
bool tuya_api_home_mode_report(int mode)
{
	if (tuya_api_online_check() == false)
	{
		LOG_RED("###### device off line ###### \n");
		return false;
	}

	// BOOL_T param_1 = 1;
	// BOOL_T param_0 = 0;

	// switch (mode)
	// {
	// case 2:
	// 	/* at home */
	// 	LOG_BLUE("REPORT AT MODE DATA TO CLOUD\n");
	// 	tuya_ipc_dp_report(NULL, 232, PROP_BOOL, &param_0, 1);
	// 	tuya_ipc_dp_report(NULL, 236, PROP_BOOL, &param_1, 1);
	// 	tuya_ipc_dp_report(NULL, 237, PROP_BOOL, &param_0, 1);
	// 	LOG_BLUE("TONIGHT I\n");
	// 	break;
	// case 0:
	// 	/* leave home */
	// 	LOG_BLUE("REPORT LEAVE MODE DATA TO CLOUD\n");
	// 	tuya_ipc_dp_report(NULL, 232, PROP_BOOL, &param_1, 1);
	// 	tuya_ipc_dp_report(NULL, 236, PROP_BOOL, &param_0, 1);
	// 	tuya_ipc_dp_report(NULL, 237, PROP_BOOL, &param_0, 1);
	// 	break;
	// case 1:
	// 	/* sleep */
	// 	LOG_BLUE("REPORT SLEEP MODE DATA TO CLOUD\n");
	// 	tuya_ipc_dp_report(NULL, 232, PROP_BOOL, &param_0, 1);
	// 	tuya_ipc_dp_report(NULL, 236, PROP_BOOL, &param_0, 1);
	// 	tuya_ipc_dp_report(NULL, 237, PROP_BOOL, &param_1, 1);
	// 	break;

	// default:
	// 	LOG_RED("ERR: unknow %d\n", mode);
	// 	break;
	// }

	// return true;
	char buf[4] = {0};
	switch (user_data_get()->home_mode)
	{
	case 2:
		strcat(buf, "0");
		break;

	case 0:
		strcat(buf, "1");
		break;

	case 1:
		strcat(buf, "2");
		break;

	default:
		strcat(buf, "0");
		break;
	}

	tuya_ipc_dp_report(NULL, 241, PROP_ENUM, buf, 1);
	return true;
}

/***
** 日期: 2022-05-19 10:37
** 作者: leo.liu
** 返回参数说明：记录call的时间戳，用于挂断与call之间的时间间隔
***/
static unsigned long long tuya_doorbell_timestamp = 0;
static bool tuya_doorbell_theread_run = false;
#define tuay_doorbell_handup_timeout_check(n) (((user_timestamp_get() - tuya_doorbell_timestamp) > (n)) ? true : false)
void tuya_doorbell_timestamp_set(unsigned long long value)
{
	tuya_doorbell_timestamp = value;
}
static void *tuya_doorbell_handup_task(void *arg)
{
	while (!tuay_doorbell_handup_timeout_check(4 * 1000))
	{
		usleep(1000);
	}
	tuya_ipc_door_bell_press_stop();
	tuya_doorbell_theread_run = false;
	return NULL;
}
/***
**   日期:2022-06-08 09:32:50
**   作者: leo.liu
**   函数作用：用于挂断app端处理
**   参数说明:
***/
void tuya_api_doorbell_handup(void)
{
	if (tuay_doorbell_handup_timeout_check(4 * 1000))
	{
		tuya_ipc_door_bell_press_stop();
	}
	else if (tuya_doorbell_theread_run == false)
	{
		tuya_doorbell_theread_run = true;
		pthread_t thread_id;
		pthread_create(&thread_id, user_pthread_atter_get(), tuya_doorbell_handup_task, NULL);
		pthread_detach(thread_id);
	}
}

/***
**   日期:2022-06-08 09:32:50
**   作者: leo.liu
**   函数作用：获取app连接设备的个数(不安全，手机app退出时无法更新状态)
**   参数说明:
***/
int tuya_api_client_num(void)
{
	if (tuya_api_online_check() == false)
	{
		return 0;
	}
	return tuya_ipc_get_client_online_num();
}
/***
**   日期:2022-06-08 15:34:00
**   作者: leo.liu
**   函数作用：tuya 循环buffer清除
**   参数说明:
***/
bool tuya_api_ringbuffer_clear(void)
{
#if 0
	if (tuya_api_online_check() == false)
	{
		return false;
	}
#endif

#if 0

	
	IPC_MEDIA_INFO_S ringbuffer;
	tuya_ringbuffer_init(&ringbuffer);
	tuya_ipc_ring_buffer_init(E_CHANNEL_VIDEO_MAIN, ringbuffer.video_bitrate[E_CHANNEL_VIDEO_MAIN], ringbuffer.video_fps[E_CHANNEL_VIDEO_MAIN], 0, NULL);
	tuya_ipc_ring_buffer_init(E_CHANNEL_AUDIO, ringbuffer.audio_sample[E_CHANNEL_AUDIO] * ringbuffer.audio_databits[E_CHANNEL_AUDIO] / 1024, ringbuffer.audio_fps[E_CHANNEL_AUDIO], 0, NULL);
#else
	// tuya_ipc_ring_buffer_clean_user_state_and_buffer(E_CHANNEL_VIDEO_MAIN,E_USER_P2P_USER);
	// tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_VIDEO_MAIN,E_USER_P2P_USER);
	// tuya_ipc_ring_buffer_clean_user_state_and_buffer(E_CHANNEL_AUDIO_2RD,E_USER_P2P_USER);
	// tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_AUDIO,E_USER_P2P_USER);

	extern void video_playback_ringbuffer_clear(void);
	extern void audio_playback_ringbuffer_clear(void);

	video_playback_ringbuffer_clear();
	audio_playback_ringbuffer_clear();
#if 0
	while(1){

		if((tuya_ipc_ring_buffer_get_video_frame(E_CHANNEL_VIDEO_MAIN,E_USER_P2P_USER,true) == NULL) &&
			(tuya_ipc_ring_buffer_get_video_frame(E_CHANNEL_VIDEO_SUB,E_USER_P2P_USER,true) == NULL) &&
			(tuya_ipc_ring_buffer_get_audio_frame(E_CHANNEL_AUDIO,E_USER_P2P_USER,true) == NULL) &&
			(tuya_ipc_ring_buffer_get_audio_frame(E_CHANNEL_AUDIO_2RD,E_USER_P2P_USER,true) == NULL)){


			break;
		}

		tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_VIDEO_MAIN, E_USER_P2P_USER);
		tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_VIDEO_SUB, E_USER_P2P_USER);

		tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_AUDIO, E_USER_P2P_USER);
		tuya_ipc_ring_buffer_clean_user_state(E_CHANNEL_AUDIO_2RD, E_USER_P2P_USER);
	
		printf("____%s:%d________\n",__func__,__LINE__);
		usleep(10000);
	}
#endif
#endif
	return true;
}
/***
**   日期:2022-06-08 11:16:24
**   作者: leo.liu
**   函数作用：上传call机照片
**   参数说明:
***/
bool tuya_api_call_event(int channel, const char *jpg, int size)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	} // DOORBELL_NORMAL

	tuya_ipc_notify_with_event(jpg, size, NOTIFICATION_CONTENT_JPEG, NOTIFICATION_NAME_DOORBELL);
	return true;
}

bool tuya_api_doorbell_event(void)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	tuya_ipc_door_bell_press(DOORBELL_AC, NULL, 0, NOTIFICATION_CONTENT_JPEG);
	return true;
}
/***
**   日期:2022-06-08 16:31:58
**   作者: leo.liu
**   函数作用：上传警报事件
**   参数说明:
***/
bool tuya_api_alarm_event(int channel, const char *jpg, int size)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	tuya_ipc_notify_with_event(jpg, size, NOTIFICATION_CONTENT_JPEG, channel == 3 ? NOTIFICATION_NAME_IO_ALARM : NOTIFICATION_NAME_USER_IO);
	return true;
}
/***
**   日期:2022-06-09 18:22:31
**   作者: leo.liu
**   函数作用：移动侦测事件触发
**   参数说明:
***/
bool tuya_api_motion_event(int channel, const char *jpg, int size)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	tuya_ipc_notify_with_event(jpg, size, NOTIFICATION_CONTENT_JPEG, NOTIFICATION_NAME_MOTION /*channel == 1 ? NOTIFICATION_NAME_MOTION : channel == 2 ? NOTIFICATION_NAME_LINGER
																											: channel == 3	? NOTIFICATION_NAME_HUMAN
																															 : NOTIFICATION_NAME_ABNORMAL_SOUND*/
	);
	return true;
}
/***
**   日期:2022-06-18 08:05:12
**   作者: leo.liu
**   函数作用：同步tuya时间
**   参数说明:
***/
bool tuya_api_time_sync(void)
{
	unsigned int time_t;
	int time_tone;
	if (tuya_api_online_check() == false)
	{
		return false;
	}

	if (tuya_ipc_get_service_time(&time_t, &time_tone) == 0)
	{
		struct tm new_tm, old_tm;
		tuya_ipc_get_local_time(time_t, &new_tm); // 获取涂鸦网络时间
		user_time_read(&old_tm);
		if ((new_tm.tm_year == old_tm.tm_year) && (new_tm.tm_mon == old_tm.tm_mon) && (new_tm.tm_mday == old_tm.tm_mday) && (new_tm.tm_hour == old_tm.tm_hour) && (new_tm.tm_min == old_tm.tm_min))
		{
			return false;
		}
		user_time_set(&new_tm);
		system("hwclock -s");
		extern bool intercom_user_time_sync_send(unsigned int receive_id, struct tm *tm);
		intercom_user_time_sync_send(CODE_ALL_ID, &new_tm);
		return true;
	}
	return false;
}
/***
**   日期:2022-06-18 08:05:12
**   作者: leo.liu
**   函数作用：获取当前天气
**   参数说明:
***/
bool tuya_api_weather_get(tuya_api_weather *weather)
{
	if (tuya_api_online_check() == false)
	{
		return false;
	}
	*weather = tuya_api_werather_const;
	return true;
}

int tuya_dp_238_response_access_lock_support(void)
{

	if (tuya_api_online_check() == false)
	{
		return false;
	}
	char *str = "148,239";
	int ret = 0;
	if ((ret = tuya_ipc_dp_report(NULL, 238, PROP_STR, str, 1)) != OPRT_OK)
	{
		printf("\n\n\ndp 238 report %s failed\n\n\n\n", str);
		usleep(1000 * 1000);
	}
	return true;
}

#if 1
OPERATE_RET tuya_iot_reg_wf_lock_chan_cb(FUNC_WIFI_LOCK_CHANNEL_CB func_wifi_lock_ch_cb)
{
	printf("================%s:%d =========== \n", __func__, __LINE__);
	return 1;
}

OPERATE_RET wf_nw_set_lock_chan_notify_cb(FUNC_WIFI_LOCK_CHANNEL_CB func_wifi_lock_ch_cb)
{
	printf("================%s:%d =========== \n", __func__, __LINE__);
	return 1;
}
#endif