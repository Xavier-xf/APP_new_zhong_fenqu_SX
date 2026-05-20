#include "user_data.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"

#define USER_DATA_PATH "/app/data/user_data.cfg"

static user_data_info user_data = {0};

static const user_data_info user_data_default = {

    .auto_record_mode = 0,
    .wifi_enable = true,
	.door1_always_enable = false,
	.door2_always_enable = false,
	.weather_enable = false,
	.home_mode = 2,
	.monitor_alarm = 1,
	.home_wifi_display = true,

	.ring_attr[0][0] = {//door1 ring1
		.timer_start = 0,
		.timer_end = 1200,
		.ring_mode = 0,
		.ring_time = 10,
		.ring_val = 3,
		.custom_ring = 0,
		.ring = 1,
	},
	.ring_attr[0][1] = {//door1 ring2
		.timer_start = 1200,
		.timer_end = 1800,
		.ring_mode = 0,
		.ring_time = 10,
		.ring_val = 3,
		.custom_ring = 0,
		.ring = 1,
	},
	.ring_attr[0][2] = {//door1 ring3
		.timer_start = 1800,
		.timer_end = 2359,
		.ring_mode = 0,
		.ring_time = 10,
		.ring_val = 3,
		.custom_ring = 0,
		.ring = 1,
	},
	.ring_attr[1][0] = {//door2 ring1
		.timer_start = 000,
		.timer_end = 1200,
		.ring_mode = 0,
		.ring_time = 10,
		.ring_val = 3,
		.custom_ring = 0,
		.ring = 1,
	},
	.ring_attr[1][1] = {//door2 ring2
		.timer_start = 1200,
		.timer_end = 1800,
		.ring_mode = 0,
		.ring_time = 10,
		.ring_val = 3,
		.custom_ring = 0,
		.ring = 1,
	},
	.ring_attr[1][2] = {//door2 ring3
		.timer_start = 1800,
		.timer_end = 2359,
		.ring_mode = 0,
		.ring_time = 10,
		.ring_val = 3,
		.custom_ring = 0,
		.ring = 1,
	},


	.wifi.wifi_open_flag = false,
	.wifi.wifi_connect_flag = false,
	.wifi.wifi_name = NULL,
	.wifi.wifi_pwd = NULL,

    .motion = {
	.enable = false,
	.select_camera = 0,
	.saving_fmt = 0,
	.sensivity = 1,
	.timer_en = false,
	.lcd_en = false,
	.start =
	    {
		.tm_year = 2023,
		.tm_mon = 1,
		.tm_mday = 1,
		.tm_hour = 0,
		.tm_min = 0,
		.tm_sec = 0},
	.end =
	    {
		.tm_year = 2023,
		.tm_mon = 1,
		.tm_mday = 1,
		.tm_hour = 0,
		.tm_min = 0,
		.tm_sec = 0}},
    .audio = {
	.key_sound = true,
	.ring_mute = false,
	.door1_tone = 1,
	.door2_tone = 2,
	.inter_tone = 3,
	.door_ring_volume = 2,
	.door_talk_volume = 2,

	.inter_ring_volume = 3,
	.inter_talk_volume = 2,
    },

    .display = {
	.standby_mode = 0,
	.lcd_brigtness = 100,
	.frame_time_en = 0,
	.frame_time_start = 0,
	.frame_time_end = 0,
	.night_mode = false,
	.night_time_start = 0,
	.night_time_end = 0,

	.frame_list = 0x00,
	.frame_background = 1,

	.door1 = {.bright = 10, .cont = 10, .color = 10}, 
	.door2 = {.bright = 10, .cont = 10, .color = 10},
	.cctv1 = {.bright = 10, .cont = 10, .color = 10},
	.cctv2 = {.bright = 10, .cont = 10, .color = 10},

    },
    .etc = {
	.language = 0,
	.deive_id = 0,
	.open_the_door = 0,
	.door1_open_door_mode = 0,
	.door2_lock_num = 1,
	.door_1_open_time = 1,
	.door_2_open_time = 1,
	.tuya_send_switch =true,
	.tuya_send_time =5,
	.password = {"1234"},
    },
    .alarm = {
	.auto_record = false,
	.alarm_1_enable = false,
	.alarm_1_trigger = false,
	.alarm_2_enable = false,
	.alarm_2_trigger = false,
	.alarm_enable = false,
    },
	.tuya_qrcode_info = {0},
	.get_record_flag = 0,
	.mac_qr_info = {0},
	.always_recode_mode = 0,
};

bool user_data_save(void)
{
	int fd = open(USER_DATA_PATH, O_WRONLY | O_CREAT);
	if (fd < 0)
	{
		printf("write open %s fail \n", USER_DATA_PATH);
		return false;
	}

	write(fd, &user_data, sizeof(user_data_info));

	close(fd);
	system("sync");
	return true;
}





#define user_data_check_range_out(cur, min, max)                                   \
	if ((user_##data.cur < min) || (user_##data.cur > max))                    \
	{                                                                          \
		printf("user data error %d(%d,%d) \n", user_##data.cur, min, max); \
		user_##data.cur = user_##data##_default.cur;                       \
	}

#define user_data_motion_check_range_out(x, min, max) user_data_check_range_out(motion.x, min, max)

#define user_data_audio_check_range_out(x, min, max) user_data_check_range_out(audio.x, min, max)

#define user_data_display_check_range_out(x, min, max) user_data_check_range_out(display.x, min, max)

#define user_data_etc_check_range_out(x, min, max) user_data_check_range_out(etc.x, min, max)

#define user_data_alarm_check_range_out(x, min, max) user_data_check_range_out(alarm.x, min, max)
/***
** 日期: 2022-05-05 08:47
** 作者: leo.liu
** 函数作用：检验数据是否合法
** 返回参数说明：
***/
static void user_data_check_valid(void)
{
	/***** 自动记录模式 *****/
	user_data_check_range_out(auto_record_mode, 0, 2);
	user_data_check_range_out(wifi_enable, 0, 1);
	user_data_check_range_out(weather_enable, 0, 1);
	user_data_check_range_out(door1_always_enable, 0, 1);
	user_data_check_range_out(door2_always_enable, 0, 1);
	user_data_check_range_out(home_mode, 0, 2);
	user_data_check_range_out(monitor_alarm, 0, 1);
	user_data_check_range_out(home_wifi_display, 0, 1);

	/***** 移动侦测 *****/
	user_data_motion_check_range_out(enable, 0, 1);
	user_data_motion_check_range_out(select_camera, 0, 3);
	user_data_motion_check_range_out(saving_fmt, 0, 1);
	user_data_motion_check_range_out(sensivity, 0, 2);
	user_data_motion_check_range_out(timer_en, 0, 1);
	user_data_motion_check_range_out(lcd_en, 0, 1);

	user_data_motion_check_range_out(start.tm_year, 2021, 2037);
	user_data_motion_check_range_out(start.tm_mon, 1, 12);
	user_data_motion_check_range_out(start.tm_mday, 1, 31);
	user_data_motion_check_range_out(start.tm_hour, 0, 23);
	user_data_motion_check_range_out(start.tm_min, 0, 59);
	user_data_motion_check_range_out(start.tm_sec, 0, 59);

	user_data_motion_check_range_out(end.tm_year, 2021, 2037);
	user_data_motion_check_range_out(end.tm_mon, 1, 12);
	user_data_motion_check_range_out(end.tm_mday, 1, 31);
	user_data_motion_check_range_out(end.tm_hour, 0, 23);
	user_data_motion_check_range_out(end.tm_min, 0, 59);
	user_data_motion_check_range_out(end.tm_sec, 0, 59);

	/***** audio *****/
	user_data_audio_check_range_out(key_sound, 0, 1);
	user_data_audio_check_range_out(ring_mute, 0, 1);
	user_data_audio_check_range_out(door1_tone, 1, 6);
	user_data_audio_check_range_out(door2_tone, 1, 6);
	user_data_audio_check_range_out(inter_tone, 1, 6);
 
	user_data_audio_check_range_out(door_ring_volume, 0, 3);
	user_data_audio_check_range_out(inter_ring_volume, 0, 4);

	user_data_audio_check_range_out(door_talk_volume, 1, 4);
	user_data_audio_check_range_out(inter_talk_volume, 1, 4);

	/***** display *****/
	user_data_display_check_range_out(standby_mode, 0, 1);
	user_data_display_check_range_out(lcd_brigtness, 0, 100);
	user_data_display_check_range_out(frame_time_en, 0, 1);
	user_data_display_check_range_out(frame_time_start, 0, 1440);
	user_data_display_check_range_out(frame_time_end, 0, 1440);
	user_data_display_check_range_out(night_mode, 0, 1);
	user_data_display_check_range_out(night_time_start, 0, 1440);
	user_data_display_check_range_out(night_time_end, 0, 1440);

	user_data_display_check_range_out(frame_list, 0, 0x1F);
	user_data_display_check_range_out(frame_background, 0, 2);

	user_data_display_check_range_out(door1.bright, 0, 20);
	user_data_display_check_range_out(door1.cont, 0, 20);
	user_data_display_check_range_out(door1.color, 0, 20);

	user_data_display_check_range_out(door2.bright, 0, 20);
	user_data_display_check_range_out(door2.cont, 0, 20);
	user_data_display_check_range_out(door2.color, 0, 20);

	user_data_display_check_range_out(cctv1.bright, 0, 20);
	user_data_display_check_range_out(cctv1.cont, 0, 20);
	user_data_display_check_range_out(cctv1.color, 0, 20);

	user_data_display_check_range_out(cctv2.bright, 0, 20);
	user_data_display_check_range_out(cctv2.cont, 0, 20);
	user_data_display_check_range_out(cctv2.color, 0, 20);

	/***** etc *****/ 
	user_data_etc_check_range_out(language, 0, 12);
	user_data_etc_check_range_out(deive_id, 0, 3);
	user_data_etc_check_range_out(open_the_door, 0, 1);
	user_data_etc_check_range_out(door1_open_door_mode, 0, 1);
	user_data_etc_check_range_out(door2_lock_num, 1, 2);
	user_data_etc_check_range_out(door_1_open_time, 1, 60);
	user_data_etc_check_range_out(door_2_open_time, 1, 60);
	user_data_etc_check_range_out(tuya_send_switch, 0, 1);
	user_data_etc_check_range_out(tuya_send_time, 0, 25);
	user_data_etc_check_range_out(password[0], '0', '9');
	user_data_etc_check_range_out(password[1], '0', '9');
	user_data_etc_check_range_out(password[2], '0', '9');
	user_data_etc_check_range_out(password[3], '0', '9');
	/*****  alarm *****/
	user_data_alarm_check_range_out(auto_record, 0, 1);
	user_data_alarm_check_range_out(alarm_1_enable, 0, 1);
	user_data_alarm_check_range_out(alarm_1_trigger, 0, 1);
	user_data_alarm_check_range_out(alarm_2_enable, 0, 1);
	user_data_alarm_check_range_out(alarm_2_trigger, 0, 1);
	user_data_alarm_check_range_out(alarm_enable, 0, 1);

	
	user_data_check_range_out(get_record_flag, 0, 2);
	user_data_check_range_out(always_recode_mode, 0, 2);
}

bool user_data_init(void)
{
	int fd = open(USER_DATA_PATH, O_RDONLY);
	if (fd < 0)
	{
		printf("read open %s fail \n", USER_DATA_PATH);
		user_data = user_data_default;
		return false;
	}
	read(fd, &user_data, sizeof(user_data_info));

	close(fd);
	user_data_check_valid();
	return true;
}

user_data_info *user_data_get(void)
{
	return &user_data;
}

void user_data_reset(void)
{
	char language = user_data.etc.language;
	user_data = user_data_default;
	user_data.etc.language = language;
	user_data_save();
	system("sync");
}
