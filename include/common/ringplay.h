#ifndef _RING_PLAY_H_
#define _RING_PLAY_H_
#include <stdbool.h>
#include "lvgl/lv_obj.h"
typedef void (*ringplay_callback)(int index);
/***
** 日期: 2022-04-27 12:00
** 作者: leo.liu
** 函数作用：铃声播放任务初始化
** 返回参数说明：
***/
bool ringplay_init(void);
/***
** 日期: 2022-04-27 13:32
** 作者: leo.liu
** 函数作用：播放按键音
** 返回参数说明：
***/
bool touch_sound_play(ringplay_callback start, ringplay_callback finish);
/***
**   日期:2022-06-02 14:08:49
**   作者: leo.liu
**   函数作用：设置按键音信息
**   参数说明:info,必须是8k的pcm,为rom.bin中的文件
***/
bool touch_sound_rom_info_register(rom_bin_info *info);
/***
** 日期: 2022-04-27 13:51
** 作者: leo.liu
** 函数作用：强制铃声静止输出设置
** 返回参数说明：
***/
void ringplay_force_mute_set(bool en);
/***
** 日期: 2022-04-27 14:00
** 作者: leo.liu
** 函数作用：设置是否播放按键音
** 返回参数说明：
***/
void ringplay_touchsound_mute_set(bool en);
/***
** 日期: 2022-04-27 14:02
** 作者: leo.liu
** 函数作用：设置按键音的音量大小
** 返回参数说明：
***/
void ringplay_touchsound_volume_set(int volume);
/***
** 日期: 2022-04-27 14:11
** 作者: leo.liu
** 函数作用：停止播放铃声
** 返回参数说明：
***/
void ringplay_play_stop(void);
/***
** 日期: 2022-04-27 14:32
** 作者: leo.liu
** 函数作用：播放铃声
** 返回参数说明：
***/
void ringplay_play_form_index(int index, int volume, ringplay_callback start, ringplay_callback end, bool loop);
/***
** 日期: 2022-04-27 14:32
** 作者: leo.liu
** 函数作用：播放mp3音乐
** 返回参数说明：
***/
void custom_music_play(int index, int volume, ringplay_callback start, ringplay_callback finish, bool loop);
/***
**   日期:2022-05-28 14:44:09
**   作者: leo.liu
**   函数作用：判断铃声是否播放
**   参数说明:
***/
bool ringplay_ing_check(void);

/*****  说明 ;*****/
/***
**   日期:2022-06-02 14:35:57
**   作者: leo.liu
**   函数作用：外部需要实现  void power_amplifier_enable(bool);
**   参数说明:用于控制功放输出
***/
#endif