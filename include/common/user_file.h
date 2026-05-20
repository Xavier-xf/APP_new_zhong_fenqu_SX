#ifndef _USER_FILE_H_
#define _USER_FILE_H_
#include <stdbool.h>

#define SD_BASE_PATH "/mnt/tf"
#define SD_TUYA_PATH "/mnt/tuya"
#define SD_MEDIA_PATH SD_BASE_PATH "/media/"
#define SD_MUSIC_PATH SD_BASE_PATH "/music/"
#define SD_DCIM_PATH SD_BASE_PATH "/DCIM/"
#define FLASH_MEDIA_BCAKUP_PATH SD_BASE_PATH "/backup/"
#define SD_PICTURE_PATH SD_BASE_PATH "/picture/"

#define FLASH_PHOTO_PATH "/app/data/photo/"

#define VIDEO_DOT ".AVI"
#define PHOTO_DOT ".JPG"
#define PICTURE_DOT   ".jpg"
#define PICTURE_E_DOT   ".jpeg"
#define MUSIC_DOT ".mp3"

typedef enum
{
	FILE_TYPE_PHOTO,
	FILE_TYPE_VIDEO,
	FILE_TYPE_FLASH_PHOTO,
	FILE_TYPE_SD_MIXED,
    FILE_TYPE_SD_MIXED_PHOTO,
    FILE_TYPE_SD_MIXED_VIDEO,
    FILE_TYPE_SD_PHOTO,
    FILE_TYPE_SD_VIDEO,
    FILE_TYPE_SD_PICTURE,
    FILE_TYPE_SD_MUSIC,

    FILE_TYPE_SD_DIMC,

	FILE_TYPE_NONE
} file_type;

typedef struct
{
	char file_name[23];
	char mode;
	char ch;
	bool is_new;
	file_type type;
} file_info;

/***
** 日期: 2022-05-17 10:40
** 作者: leo.liu
** 函数作用：媒体文件初始化
** 返回参数说明：
***/
void media_file_list_init(void);

/***
** 日期: 2022-05-17 10:40
** 作者: leo.liu
** 函数作用：sd卡容量計算
** 返回参数说明：
***/
void get_SD_space(unsigned long  *bavail,unsigned long  *disk_all_space);
/***
** 日期: 2022-05-17 10:40
** 作者: leo.liu
** 函数作用：falsh容量計算
** 返回参数说明：
***/
void get_falsh_space(unsigned long  *bavail,unsigned long  *disk_all_space);
/***
** 日期: 2022-05-17 11:26
** 作者: leo.liu
** 函数作用：判断sd插入
** 返回参数说明：
***/
bool media_sdcard_insert_check(void);
/***
** 日期: 2022-05-17 14:40
** 作者: leo.liu
** 函数作用：判断sd文件是否已经满
** 返回参数说明：
***/
bool media_sdcard_full_check(void);
/***
** 日期: 2022-05-17 11:28
** 作者: leo.liu
** 函数作用：创建一个文件
** 返回参数说明：
***/
bool media_file_create(file_type type, char ch, char mode, char *file_path);
/***
** 日期: 2022-05-17 13:36
** 作者: leo.liu
** 函数作用：文件总数获取
** 返回参数说明：
***/
bool media_file_total_get(file_type type, int *total, int *new_total);
/***
** 日期: 2022-05-17 13:44
** 作者: leo.liu
** 函数作用：清除新文件标志
** 返回参数说明：
***/
bool media_file_new_clear(file_type type, int index);
/***
** 日期: 2022-05-17 13:53
** 作者: leo.liu
** 函数作用：删除一个文件
** 返回参数说明：
***/
bool media_file_delete(file_type type, int index);
/***
** 日期: 2022-05-17 13:54
** 作者: leo.liu
** 函数作用：判断该文件是否存在
** 返回参数说明：
***/
bool media_file_bad_check(const char *file);
/***
**   日期:2022-05-23 09:51:21
**   作者: leo.liu
**   函数作用：获取格式化sd卡状态
**   参数说明:
***/
bool media_format_sd_state(void);
/***
**   日期:2022-05-23 09:58:49
**   作者: leo.liu
**   函数作用：格式话sd卡
**   参数说明:
***/
void media_format_sd(void);
/***
**   日期:2022-05-23 14:34:43
**   作者: leo.liu
**   函数作用：获取文件信息
**   参数说明:
***/
const file_info *media_file_info_get(file_type type, int index);
/***
**   日期:2022-06-15 11:14:00
**   作者: leo.liu
**   函数作用：删除所有文件
**   参数说明:
 all,true:删除所有文件包括视频和照片
     false:根据type删除指定的文件照片或这视频
***/
bool media_file_delete_all(file_type type, bool all);



/***********************************************
** 作者: leo.liu
** 日期: 2022-11-5 17:10:35
** 说明: 获取sd卡使用信息 
***********************************************/
bool media_sd_memory_query(unsigned int * p_total, unsigned int * p_user, unsigned int * p_free);


/***********************************************
** 作者: leo.liu
** 日期: 2022-11-5 17:10:35
** 说明: 备份flash的照片到sd卡的backup目录
***********************************************/
bool flash_backup_to_sd();

void copy_flash_photo_to_sd();

bool media_del_file_state(void);
bool file_format_sd_state(void);
void media_del_file_state_set(bool en);
#endif