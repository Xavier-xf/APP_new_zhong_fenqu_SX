#include "user_file.h"
#include "ak_mem.h"
#include <dirent.h>
#include <pthread.h>
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include "fcntl.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "user_common.h"
#include "user_time.h"
#include "lv_msg_event.h"
#include "info.h"

/***** sd文件最大数目 *****/
#define SD_MEDIA_NUM_MAX 10000

/***** flash的照片最大数目 *****/
#define FLASH_PHOTO_MAX 110 // 没插sd卡时照片最大数目

#define MEDIA_PATH_MAX 128

#define SD_MIXED_MAX 1000 // 插sd卡时照片和视频混合最大数目   （不使用）

#define SD_PHOTO_MAX 500 // 插sd卡时照片最大数目            （不使用）

#define SD_VIDEO_MAX 500 // 插sd卡时视频最大数目           （不使用）

#define SD_MUSIC_MAX 20 // sd卡中音乐最大数目

#define SD_PICTURE_MAX 30

/***** 文件最大路径 *****/
#define MEDIA_PATH_MAX 128

/***** 文件名最大数目 *****/
#define FILE_NAME_MAX 23

static int sd_media_total = 0;
static int sd_media_new_total = 0;
static file_info *p_media_sd = NULL;

static int flash_media_total = 0;
static int flash_media_new_total = 0;
static file_info *p_media_flash = NULL;

static int sd_music_total = 0;
static int sd_music_new_total = 0;
static file_info *p_sd_music = NULL;

static int sd_background_total = 0;
static int sd_background_new_total = 0;
static file_info *p_sd_background = NULL;

/***** 判定sd卡是否插入 *****/
static bool is_media_sd_insert = false;
static bool is_media_sd_full = false;
static pthread_mutex_t file_list_mutex;

static bool sd_format_flg = false;

static bool sd_format_status = false;
static bool file_del_status = false;
/***
** 日期: 2022-05-17 10:58
** 作者: leo.liu
** 函数作用：缓存文件路径
** 返回参数说明：
***/
#define CHACHE_NAME ".conf"
#define SD_MEDIA_CACHE_PATH SD_MEDIA_PATH CHACHE_NAME
#define FLASH_MEDIA_CACHE_PATH FLASH_PHOTO_PATH CHACHE_NAME
#define FLASH_MEDIA_CACHE_BCAKUP_PATH FLASH_MEDIA_BCAKUP_PATH CHACHE_NAME

/***
** 日期: 2022-05-17 11:48
** 作者: leo.liu
** 函数作用：文件信息同步
** 返回参数说明：
***/
static bool _media_file_sync(file_type type)
{
	if (type != FILE_TYPE_FLASH_PHOTO)
	{
		system("rm -rf " SD_MEDIA_CACHE_PATH);
		int fd = open(SD_MEDIA_CACHE_PATH, O_CREAT | O_WRONLY);
		if (fd < 0)
		{
			printf("open %s failed \n", SD_MEDIA_CACHE_PATH);
			return false;
		}
		write(fd, p_media_sd, sd_media_total * sizeof(file_info));
		close(fd);
	}
	else
	{
		system("rm -rf " FLASH_MEDIA_CACHE_PATH);
		int fd = open(FLASH_MEDIA_CACHE_PATH, O_CREAT | O_WRONLY);
		if (fd < 0)
		{
			printf("open %s failed \n", FLASH_MEDIA_CACHE_PATH);
			return false;
		}
		write(fd, p_media_flash, flash_media_total * sizeof(file_info));
		close(fd);
	}
	system("sync");
	return true;
}
/***
** 日期: 2022-05-17 10:53
** 作者: leo.liu
** 函数作用：根据.conf扫描其中的文件信息
** 返回参数说明：
***/
static bool _media_file_load(file_info *pinfo, const char *cache_dir, int *ptotal, int *pnew_total, int num_file_max, const char *file_path, file_type type)
{
	(*ptotal) = (*pnew_total) = 0;

	int fd = open(cache_dir, O_RDONLY);
	if (fd < 0)
	{
		printf("open %s failed \n", cache_dir);
		return false;
	}
	bool need_sync = false;
	char file[128];
	int read_len = 0;
	file_info info;
	int read_size = sizeof(file_info);
	while ((read_len = read(fd, &info, read_size)) == read_size)
	{
		sprintf(file, "%s%s", file_path, info.file_name);
		struct stat st;
		memset(&st, 0, sizeof(st));
		if (access(file, F_OK) == 0) //(stat(file, &st) == 0) //// if (access(file, F_OK) == 0)
		{
			*pinfo = info;
			(*ptotal)++;
			if (info.is_new == true)
			{
				(*pnew_total)++;
			}
			if ((*ptotal) >= num_file_max)
			{
				break;
			}
			pinfo++;
		}
		else if (need_sync == false)
		{
			printf("%s not exist \n", file);
			need_sync = true;
		}
	}
	close(fd);
	if (need_sync == true)
	{
		_media_file_sync(type);
	}
	printf("scanf %s :total:%d,new total:%d\n", cache_dir, *ptotal, *pnew_total);
	return true;
}

static int _video_bad_path_check(const char *file, char *damaged_file)
{
	char bad_file_path[MEDIA_PATH_MAX];
	sprintf(bad_file_path, "%stemp", file);
	if (access(bad_file_path, F_OK) == 0)
	{
		if (damaged_file != NULL)
		{

			strcpy(damaged_file, bad_file_path);
		}
		return 1;
	}
	return 0;
}
// static int _photo_bad_path_check(const char* file,char* badfile){

//     sprintf(badfile,"%s",file);
//     if(access(badfile,F_OK) == 0){
//         return 0;
//     }
//     return 1;
// }

static bool scan_find_file(file_type type, const char *dir_path, file_info *p_info, int *p_total, int *p_total_new)
{
	char cmd_buffer[64] = {0};
	sprintf(cmd_buffer, "find %s -type f", dir_path);

	FILE *pf = popen(cmd_buffer, "r");
	char buffer[128] = {0};
	// printf("==========================================================================>>>>>  %d \n", *p_total);
	while (fgets(buffer, 128, pf))
	{
		printf("======================>>> : [%s] \n", buffer);
		/**********************
		判断文件是否为规范
		**********************/
		buffer[strlen(buffer) - 1] = '\0';
		char *p_file = strrchr(buffer, '/') + 1;
		if (strlen(p_file) > 24 || strlen(p_file) < 4)
		{
			printf("file name error: %s \n\r", p_file);
			goto fail_next;
		}
		/**************************
		判断是否为视频切换为坏文件
		***************************/
		if (((type != FILE_TYPE_FLASH_PHOTO) && (type != FILE_TYPE_SD_PICTURE)) && (_video_bad_path_check(buffer, NULL) != 0))
		{

			printf("unknown file type \n\r");
			goto fail_next;
		}

		// printf("--- %s %d ---\n\r",p_file,strlen(p_file));
		/******************
		获取该文件所属类型
		*******************/
		char *ptr = strrchr(buffer, '.');
		if ((type == FILE_TYPE_SD_MIXED_PHOTO) && (strcmp(ptr, PHOTO_DOT) == 0))
		{
			p_info->type = FILE_TYPE_SD_MIXED_PHOTO;
		}
		else if ((type == FILE_TYPE_SD_MIXED_VIDEO) && (strcmp(ptr, VIDEO_DOT) == 0))
		{
			p_info->type = FILE_TYPE_SD_MIXED_VIDEO;
		}
		else if ((type == FILE_TYPE_SD_PHOTO) && (strcmp(ptr, PHOTO_DOT) == 0))
		{
			p_info->type = FILE_TYPE_SD_PHOTO;
		}
		else if ((type == FILE_TYPE_FLASH_PHOTO) && (strcmp(ptr, PHOTO_DOT) == 0))
		{
			p_info->type = FILE_TYPE_FLASH_PHOTO;
		}
		else if ((type == FILE_TYPE_SD_VIDEO) && (strcmp(ptr, VIDEO_DOT) == 0))
		{
			p_info->type = FILE_TYPE_SD_VIDEO;
		}
		else if ((type == FILE_TYPE_SD_MUSIC) && (strcmp(ptr, MUSIC_DOT) == 0))
		{
			p_info->type = FILE_TYPE_SD_MUSIC;
		}
		else if ((type == FILE_TYPE_SD_PICTURE) && ((strcmp(ptr, PICTURE_E_DOT) == 0) || (strcmp(ptr, PICTURE_DOT) == 0) || (strcmp(ptr, PHOTO_DOT) == 0)))
		{
			p_info->type = FILE_TYPE_SD_PICTURE;
		}
		else
		{
			goto fail_next;
		}

		/***********
		获取该文件名
		************/
		strncpy(p_info->file_name, p_file, 22);

		p_info->ch = p_file[16] - 48;
		p_info->mode = p_file[17] - 48;

		p_info++;
		(*p_total)++;

		if ((type == FILE_TYPE_SD_MIXED_PHOTO || type == FILE_TYPE_SD_MIXED_VIDEO) && ((*p_total) > SD_MIXED_MAX))
		{
			break;
		}
		else if ((type == FILE_TYPE_SD_PHOTO) && ((*p_total) >= SD_PHOTO_MAX))
		{
			break;
		}
		if ((type == FILE_TYPE_SD_VIDEO) && ((*p_total) >= SD_VIDEO_MAX))
		{
			break;
		}
		else if ((type == FILE_TYPE_SD_MUSIC) && ((*p_total) > SD_MUSIC_MAX))
		{
			break;
		}
		else if ((type == FILE_TYPE_SD_PICTURE) && ((*p_total) > SD_PICTURE_MAX))
		{
			break;
		}
		else if ((type == FILE_TYPE_FLASH_PHOTO) && ((*p_total) > FLASH_PHOTO_MAX))
		{
			break;
		}
	fail_next:
		memset(buffer, 0, sizeof(buffer));
	}
	pclose(pf);
	// printf("==========================================================================>>>>>  %d \n", *p_total);
	return true;
}
/***
** 日期: 2022-05-17 10:48
** 作者: leo.liu
** 函数作用：媒体文件扫描
** 返回参数说明：
***/
static bool _scan_media_file(file_type type)
{
	char *cache_dir = NULL;
	char *file_path = NULL;
	file_info *info;
	int *p_total;
	int *p_new_total;
	int num_file_max = 0;
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		cache_dir = FLASH_MEDIA_CACHE_PATH;
		info = p_media_flash;
		flash_media_total = flash_media_new_total = 0;
		p_total = &flash_media_total;
		p_new_total = &flash_media_new_total;
		num_file_max = FLASH_PHOTO_MAX;
		file_path = FLASH_PHOTO_PATH;
		if (access(file_path, F_OK) != 0)
		{
			/***** 文件夹不存在 *****/
			system("mkdir " FLASH_PHOTO_PATH);
			printf("mkdir " FLASH_PHOTO_PATH "\n");
			return false;
		}
	}
	else if (type == FILE_TYPE_SD_MUSIC)
	{
		file_path = SD_MUSIC_PATH;
		sd_music_total = sd_music_new_total = 0;
		info = p_sd_music;
		p_total = &sd_music_total;
		p_new_total = &sd_music_new_total;
		if (access(file_path, F_OK) != 0)
		{
			/***** 文件夹不存在 *****/
			system("mkdir " SD_MUSIC_PATH);
			printf("mkdir " SD_MUSIC_PATH "\n");
			return false;
		}
		else
		{
			return scan_find_file(type, file_path, info, p_total, p_new_total);
		}
	}
	else if (type == FILE_TYPE_SD_PICTURE)
	{
		file_path = SD_PICTURE_PATH;
		sd_background_total = sd_background_new_total = 0;
		info = p_sd_background;
		p_total = &sd_background_total;
		p_new_total = &sd_background_new_total;
		if (access(file_path, F_OK) != 0)
		{
			/***** 文件夹不存在 *****/
			system("mkdir " SD_PICTURE_PATH);
			printf("mkdir " SD_PICTURE_PATH "\n");
			return false;
		}
		else
		{
			return scan_find_file(type, file_path, info, p_total, p_new_total);
		}
	}
	else if (type != FILE_TYPE_FLASH_PHOTO)
	{
		cache_dir = SD_MEDIA_CACHE_PATH;
		info = p_media_sd;
		sd_media_total = sd_media_new_total = 0;
		p_total = &sd_media_total;
		p_new_total = &sd_media_new_total;
		num_file_max = SD_MEDIA_NUM_MAX;
		file_path = SD_MEDIA_PATH;
		if (access(file_path, F_OK) != 0)
		{
			/***** 文件夹不存在 *****/
			system("mkdir " SD_MEDIA_PATH);
			printf("mkdir " SD_MEDIA_PATH "\n");
			return false;
		}
	}
	else
	{
		return false;
	}
	return _media_file_load(info, cache_dir, p_total, p_new_total, num_file_max, file_path, type);
}

/***
** 日期: 2022-05-17 11:07
** 作者: leo.liu
** 函数作用：判断sd卡是否已经插入
** 返回参数说明：
***/
static bool _media_sd_insert_check(void)
{
	return (access("/dev/mmcblk0", F_OK) == 0) ? true : false;
}

/***
** 日期: 2022-05-17 11:12
** 作者: leo.liu
** 函数作用：创建文件夹
** 返回参数说明：
***/
static void _media_dir_create(const char *path)
{
	char buf[128] = {0};
	sprintf(buf, "mkdir %s", path);
	system(buf);
}

#define SDMMC_DEVICE_PATH "/dev/"
#define SDMMC_DEVICE_NAME "mmcblk0"
#define SDMMC_PART_VDP_NAME "mmcblk0p1"
#define SDMMC_PART_TUYA_NAME "mmcblk0p2"
#define SD_TUYA_PATH "/mnt/tuya"
// SD卡重分区 true:成功 false:失败或已分区
// bool _sd_card_repartition(void)
// {
// 	/*-----------------------------------------------------------------------*/
// 	uint64_t block0_sector = 0, p1_sector = 0, p2_sector = 0, sector_size = 0, other_sector = 0;

// 	FILE *fp = popen("fdisk -l " SDMMC_DEVICE_PATH SDMMC_DEVICE_NAME, "r");
// 	// printf("=========================>>>> 开始分区1111\n");
// 	if (fp == NULL)
// 	{
// 		return false;
// 	}
// 	// printf("=========================>>>> 开始分区2222\n");
// 	char buffer[256] = {0};
// 	while (fgets(buffer, sizeof(buffer), fp))
// 	{
// 		// printf("%s\n", buffer);
// 		if (strstr(buffer, "/dev/mmcblk0:") != NULL)
// 		{
// 			char *p_sectors_str = strrchr(buffer, ',') + 1;
// 			sscanf(p_sectors_str, " %llu sectors", &block0_sector);
// 			// uint64_t a,b;
// 			// char uint_buffer[10] = {0};
// 			// sscanf(buffer, "Disk /dev/mmcblk0: %llu %s, %llu bytes, %llu sectors", &a,uint_buffer, &b, &block0_sector);
// 		}
// 		if (strstr(buffer, "Units: sectors of") != NULL) // 获取SD卡扇区大小
// 		{
// 			uint64_t a, b;
// 			sscanf(buffer, "Units: sectors of %llu * %llu = %llu bytes", &a, &sector_size, &b);
// 		}
// 		if (strstr(buffer, "/dev/mmcblk0p1") != NULL)
// 		{
// 			char str[8][32];
// 			for (int i = 0; i < 8; i++)
// 			{
// 				memset(str[i], 0, 32);
// 			}
// 			sscanf(buffer, "%s\t%s\t%s\t%s\t%s\t%llu\t%s\t%s", str[0], str[1], str[2], str[3], str[4], &p1_sector, str[6], str[7]);
// 		}
// 		else if (strstr(buffer, "/dev/mmcblk0p2") != NULL)
// 		{
// 			char str[8][32];
// 			for (int i = 0; i < 8; i++)
// 			{
// 				memset(str[i], 0, 32);
// 			}
// 			sscanf(buffer, "%s\t%s\t%s\t%s\t%s\t%llu\t%s\t%s", str[0], str[1], str[2], str[3], str[4], &p2_sector, str[6], str[7]);
// 		}
// 		else if (strstr(buffer, "/dev/mmcblk0p") != NULL)
// 		{
// 			// printf("============find other part===============\n");
// 			other_sector = 1;
// 		}
// 		memset(buffer, 0, sizeof(buffer));
// 		usleep(1 * 1000);
// 	}
// 	pclose(fp);
// 	printf("=================>>> block0_sector:[%llu], p1_sector:[%llu], p2_sector:[%llu], sector_size:[%llu]\n", block0_sector, p1_sector, p2_sector, sector_size);
// 	if (other_sector || p1_sector == 0 || p2_sector == 0 || abs(block0_sector / 3 - p1_sector) > 512 || abs(block0_sector * 2 / 3 - p2_sector) > 512)
// 	{
// 		system("umount " SD_BASE_PATH);
// 		system("umount " SD_TUYA_PATH);
// 		system("umount /mnt/.upgrade");

// 		printf("=========================>>>> 开始分区\n");
// 		// system("mkdosfs -F 32 -I /dev/mmcblk0 -n Panasonic0 ");
// 		FILE *fp1 = popen("fdisk " SDMMC_DEVICE_PATH SDMMC_DEVICE_NAME, "wr");
// 		if (other_sector)
// 		{
// 			for (int i = 0; i < 10; i++)
// 			{
// 				char c = i + 48;
// 				fputc('d', fp1);
// 				fputc('\n', fp1);

// 				fputc(c, fp1);
// 				fputc('\n', fp1);
// 				// printf("============find other part %c===============\n" ,c);
// 			}
// 		}
// 		else
// 		{
// 			if (p1_sector > 0)
// 			{
// 				fputc('d', fp1);
// 				fputc('\n', fp1);

// 				fputc('1', fp1);
// 				fputc('\n', fp1);
// 			}
// 			if (p2_sector > 0)
// 			{
// 				fputc('d', fp1);
// 				fputc('\n', fp1);

// 				fputc('2', fp1);
// 				fputc('\n', fp1);
// 			}
// 		}

// 		fputc('n', fp1);
// 		fputc('\n', fp1);

// 		fputc('p', fp1);
// 		fputc('\n', fp1);

// 		fputc('1', fp1);
// 		fputc('\n', fp1);

// 		fputc('\n', fp1);

// 		char str[128] = {0};
// 		sprintf(str, "%llu", block0_sector / 3);

// 		fputs(str, fp1);
// 		fputc('\n', fp1);

// 		fputc('n', fp1);
// 		fputc('\n', fp1);

// 		fputc('p', fp1);
// 		fputc('\n', fp1);

// 		fputc('2', fp1);
// 		fputc('\n', fp1);

// 		fputc('\n', fp1);

// 		fputc('\n', fp1);

// 		fputc('w', fp1);
// 		fputc('\n', fp1);
// 		pclose(fp1);
// 		usleep(500 * 1000);
// 		system("echo 0 > /sys/mmc_en/mmc2_card_pwr_en enable");
// 		usleep(500 * 1000);
// 		system("echo 1 > /sys/mmc_en/mmc2_card_pwr_en enable");
// 		usleep(2000 * 1000);
// 		system("mkdosfs -F 32 -I " SDMMC_DEVICE_PATH SDMMC_PART_VDP_NAME " -n Panasonic1 ");
// 		system("mkdosfs -F 32 -I " SDMMC_DEVICE_PATH SDMMC_PART_TUYA_NAME " -n Panasonic2 ");
// 		system("sync");
// 		system("mkdir " SD_BASE_PATH);
// 		system("mkdir " SD_TUYA_PATH);

// 		printf("=========================>>>> 分区完成\n");
// 		return true;
// 	}
// 	return false;
// }
static uint64_t block0_sector = 0, p1_sector = 0, p2_sector = 0, sector_size = 0, other_sector = 0;

bool _sd_card_repartition_detection(void)
{
	// uint64_t block0_sector = 0, p1_sector = 0, p2_sector = 0, sector_size = 0, other_sector = 0;

	FILE *fp = popen("fdisk -l " SDMMC_DEVICE_PATH SDMMC_DEVICE_NAME, "r");
	if (fp == NULL)
	{
		return false;
	}
	char buffer[256] = {0};
	while (fgets(buffer, sizeof(buffer), fp))
	{
		if (strstr(buffer, "/dev/mmcblk0:") != NULL)
		{
			char *p_sectors_str = strrchr(buffer, ',') + 1;
			sscanf(p_sectors_str, " %llu sectors", &block0_sector);
		}
		if (strstr(buffer, "Units: sectors of") != NULL) // 获取SD卡扇区大小
		{
			uint64_t a, b;
			sscanf(buffer, "Units: sectors of %llu * %llu = %llu bytes", &a, &sector_size, &b);
		}
		if (strstr(buffer, "/dev/mmcblk0p1") != NULL)
		{
			char str[8][32];
			for (int i = 0; i < 8; i++)
			{
				memset(str[i], 0, 32);
			}
			sscanf(buffer, "%s\t%s\t%s\t%s\t%s\t%llu\t%s\t%s", str[0], str[1], str[2], str[3], str[4], &p1_sector, str[6], str[7]);
		}
		else if (strstr(buffer, "/dev/mmcblk0p2") != NULL)
		{
			char str[8][32];
			for (int i = 0; i < 8; i++)
			{
				memset(str[i], 0, 32);
			}
			sscanf(buffer, "%s\t%s\t%s\t%s\t%s\t%llu\t%s\t%s", str[0], str[1], str[2], str[3], str[4], &p2_sector, str[6], str[7]);
		}
		else if (strstr(buffer, "/dev/mmcblk0p") != NULL)
		{
			other_sector = 1;
		}
		memset(buffer, 0, sizeof(buffer));
		usleep(1 * 1000);
	}
	pclose(fp);
	printf("=================>>> block0_sector:[%llu], p1_sector:[%llu], p2_sector:[%llu], sector_size:[%llu], other_sector[%llu]\n", block0_sector, p1_sector, p2_sector, sector_size,other_sector);
	if (other_sector || p1_sector == 0 || p2_sector == 0 || abs(block0_sector / 3 - p1_sector) > 512 || abs(block0_sector * 2 / 3 - p2_sector) > 512)
	{
		return true;
	}
	return false;
}


void _sd_card_subzone(void)
{
	system("umount " SD_BASE_PATH);
	system("umount " SD_TUYA_PATH);
	system("umount /mnt/.upgrade");

	printf("=========================>>>> 开始分区\n");
	FILE *fp1 = popen("fdisk " SDMMC_DEVICE_PATH SDMMC_DEVICE_NAME, "wr");
	if (other_sector)
	{
		for (int i = 0; i < 10; i++)
		{
			char c = i + 48;
			fputc('d', fp1);
			fputc('\n', fp1);

			fputc(c, fp1);
			fputc('\n', fp1);
		}
	}
	else
	{
		if (p1_sector > 0)
		{
			fputc('d', fp1);
			fputc('\n', fp1);

			fputc('1', fp1);
			fputc('\n', fp1);
		}
		if (p2_sector > 0)
		{
			fputc('d', fp1);
			fputc('\n', fp1);

			fputc('2', fp1);
			fputc('\n', fp1);
		}
	}

	fputc('n', fp1);
	fputc('\n', fp1);

	fputc('p', fp1);
	fputc('\n', fp1);

	fputc('1', fp1);
	fputc('\n', fp1);

	fputc('\n', fp1);

	char str[128] = {0};
	sprintf(str, "%llu", block0_sector / 3);

	fputs(str, fp1);
	fputc('\n', fp1);

	fputc('n', fp1);
	fputc('\n', fp1);

	fputc('p', fp1);
	fputc('\n', fp1);

	fputc('2', fp1);
	fputc('\n', fp1);

	fputc('\n', fp1);

	fputc('\n', fp1);

	fputc('w', fp1);
	fputc('\n', fp1);
	pclose(fp1);
	usleep(500 * 1000);
	system("echo 0 > /sys/mmc_en/mmc2_card_pwr_en enable");
	usleep(500 * 1000);
	system("echo 1 > /sys/mmc_en/mmc2_card_pwr_en enable");
	usleep(2000 * 1000);
	system("mkdosfs -F 32 -I " SDMMC_DEVICE_PATH SDMMC_PART_VDP_NAME " -n Panasonic1 ");
	system("mkdosfs -F 32 -I " SDMMC_DEVICE_PATH SDMMC_PART_TUYA_NAME " -n Panasonic2 ");
	system("sync");
	system("mkdir " SD_BASE_PATH);
	system("mkdir " SD_TUYA_PATH);

	printf("=========================>>>> 分区完成\n");
	
}





/***
** 日期: 2022-05-17 11:24
** 作者: leo.liu
** 函数作用：格式化sd
** 返回参数说明：
***/
static void _media_sd_format(void)
{
	printf("##### start fomrat sdcard ##### \n");
	system("rm -rf " SD_MEDIA_PATH);
	system("umount " SD_BASE_PATH);
	system("umount " SD_TUYA_PATH);

	if (access(SDMMC_DEVICE_PATH SDMMC_PART_VDP_NAME, F_OK) == 0)
	{
		system("mkdosfs -F 32 -I " SDMMC_DEVICE_PATH SDMMC_PART_VDP_NAME " -n Panasonic1 ");
	}

	if (access(SDMMC_DEVICE_PATH SDMMC_PART_TUYA_NAME, F_OK) == 0)
	{
		system("mkdosfs -F 32 -I " SDMMC_DEVICE_PATH SDMMC_PART_TUYA_NAME " -n Panasonic2 ");
	}

	system("sync");
	printf("##### finish fomrat sdcard ##### \n");
}

/***
** 日期: 2022-05-17 11:34
** 作者: leo.liu
** 函数作用：判断sd卡是否已经满
** 返回参数说明：
***/
bool _media_sd_full_check(void)
{
	struct statfs sta;

	int total = 0;
	int free = 0;

	if (0 != statfs(SD_BASE_PATH, &sta))
	{
		return false;
	}

	if (sta.f_bsize >= 1024)
	{
		total = (int)((sta.f_bsize / 1024) * sta.f_blocks);
		free = (int)((sta.f_bsize / 1024) * sta.f_bavail);
	}
	else
	{
		total = (int)((sta.f_blocks / 1024) * sta.f_bsize);
		free = (int)((sta.f_bavail / 1024) * sta.f_bsize);
	}

	printf("============= sdcard disk:total:%0.2fM/ free:%0.2fM =============\n", total / 1024.0, free / 1024.0);
	return (free < 102400) ? true : false;
}

/***********************************************
** 作者: leo.liu
** 日期: 2022-11-5 17:10:35
** 说明: 获取sd卡使用信息
***********************************************/
bool media_sd_memory_query(unsigned int *p_total, unsigned int *p_user, unsigned int *p_free)
{
	struct statfs sta;
	static unsigned int total = 0;
	static unsigned int free = 0;

	pthread_mutex_lock(&file_list_mutex);
	if (media_format_sd_state() == true)
	{

		free = total;
		*p_user = (total) - (free);
		*p_total = total;
		*p_free = free;
		pthread_mutex_unlock(&file_list_mutex);
		return true;
	}

	if (media_sdcard_insert_check() == false)
	{
		*p_total = *p_user = *p_free = 0;
		pthread_mutex_unlock(&file_list_mutex);
		return false;
	}

	if (0 != statfs(SD_BASE_PATH, &sta))
	{
		pthread_mutex_unlock(&file_list_mutex);
		return false;
	}
	if (sta.f_bsize >= 1024)
	{
		total = (int)((sta.f_bsize / 1024) * sta.f_blocks);
		free = (int)((sta.f_bsize / 1024) * sta.f_bavail);
	}
	else
	{
		total = (int)((sta.f_blocks / 1024) * sta.f_bsize);
		free = (int)((sta.f_bavail / 1024) * sta.f_bsize);
	}
	*p_user = (total) - (free);
	*p_total = total;
	*p_free = free;
	pthread_mutex_unlock(&file_list_mutex);
	return true;
}

/***
** 日期: 2022-05-17 11:34
** 作者: leo.liu
** 函数作用：判断sd卡的容量
** 返回参数说明：
***/
void get_SD_space(unsigned long *bavail, unsigned long *disk_all_space)
{

	struct statfs diskinfo;
	statfs("/mnt/tf", &diskinfo);
	struct statfs diskinfo_p2;
	statfs("/mnt/tuya", &diskinfo_p2);
	unsigned long long total_blocks = diskinfo.f_bsize;
	unsigned long long total_blocks_p2 = diskinfo_p2.f_bsize;

	*disk_all_space = ((diskinfo.f_blocks * total_blocks) + (diskinfo_p2.f_blocks * total_blocks_p2)) >> 20; // MB
	*bavail = ((diskinfo.f_bavail * total_blocks) + (diskinfo_p2.f_bavail * total_blocks_p2)) >> 20;		 // MB

	return;
}
/***
** 日期: 2022-05-17 11:34
** 作者: leo.liu
** 函数作用：判断falsh的容量
** 返回参数说明：
***/
void get_falsh_space(unsigned long *bavail, unsigned long *disk_all_space)
{
	struct statfs diskinfo;
	statfs("/app/data", &diskinfo);
	unsigned long long total_blocks = diskinfo.f_bsize;

	*disk_all_space = (diskinfo.f_blocks * total_blocks) >> 20; // MB
	*bavail = (diskinfo.f_bavail * total_blocks) >> 20;			// MB

	return;
}
/***
** 日期: 2022-05-17 11:39
** 作者: leo.liu
** 函数作用：删除一个文件
** 返回参数说明：
***/
static bool _media_file_delete(file_type type, int index, bool sync)
{
	media_del_file_state_set(true);
	file_info *info = NULL;
	file_info *array = NULL;
	char file_path[MEDIA_PATH_MAX] = {0};
	int *ptotal = NULL;
	int *p_new_total = NULL;
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		if (index >= flash_media_total)
		{
			return false;
		}
		array = p_media_flash;
		info = &p_media_flash[index];
		strcpy(file_path, FLASH_PHOTO_PATH);
		strcat(file_path, info->file_name);

		ptotal = &flash_media_total;
		p_new_total = &flash_media_new_total;
	}
	else
	{
		if (index >= sd_media_total)
		{
			return false;
		}
		array = p_media_sd;
		info = &p_media_sd[index];
		strcpy(file_path, SD_MEDIA_PATH);
		strcat(file_path, info->file_name);

		ptotal = &sd_media_total;
		p_new_total = &sd_media_new_total;
	}

	if ((*ptotal) <= 0)
	{
		return false;
	}

	if ((info->is_new == true) && ((*p_new_total) > 0))
	{
		(*p_new_total)--;
	}
	if (access(file_path, F_OK) == 0)
	{
		remove(file_path);
	}
	printf("remove %d->%s \n", index, file_path);

	if (index < ((*ptotal) - 1))
	{
		memmove(&array[index], &array[index + 1], (((*ptotal) - 1) - index) * sizeof(file_info));
	}
	(*ptotal)--;
	if (sync == true)
	{
		_media_file_sync(type);
	}
	media_del_file_state_set(false);
	return true;
}





/***
** 日期: 2022-05-17 11:39
** 作者: leo.liu
** 函数作用：删除一个SD卡中的flash文件
** 返回参数说明：
***/
static bool _media_sd_flash_file_delete(file_type type, int index, bool sync)
{
	file_info *info = NULL;
	file_info *array = NULL;
	char file_path[MEDIA_PATH_MAX] = {0};
	int *ptotal = NULL;
	int *p_new_total = NULL;
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		if (index >= sd_media_total)
		{
			return false;
		}
		array = p_media_sd;
		info = &p_media_sd[index];
		strcpy(file_path, SD_MEDIA_PATH);
		strcat(file_path, info->file_name);

		ptotal = &sd_media_total;
		p_new_total = &sd_media_new_total;
	}

	if ((*ptotal) <= 0)
	{
		return false;
	}

	if ((info->is_new == true) && ((*p_new_total) > 0))
	{
		(*p_new_total)--;
	}
	if (access(file_path, F_OK) == 0)
	{
		remove(file_path);
	}
	printf("remove sd flash %d->%s \n", index, file_path);

	if (index < ((*ptotal) - 1))
	{
		memmove(&array[index], &array[index + 1], (((*ptotal) - 1) - index) * sizeof(file_info));
	}
	(*ptotal)--;
	if (sync == true)
	{
		_media_file_sync(type);
	}
	return true;
}





/*
根据sd卡的分区， 挂载SD卡， 优先挂载 mmcblk0p1
*/
void mount_sdcard_dev(void)
{
	if (access(SD_BASE_PATH, F_OK) != 0)
	{
		_media_dir_create(SD_BASE_PATH);
	}
	if (access(SD_TUYA_PATH, F_OK) != 0)
	{
		_media_dir_create(SD_TUYA_PATH);
	}

	char cmd[128] = {0};
	memset(cmd, 0, sizeof(cmd));
	if (access(SDMMC_DEVICE_PATH SDMMC_PART_VDP_NAME, F_OK) == 0) /* 主分区 */
	{
		sprintf(cmd, "mount -t vfat " SDMMC_DEVICE_PATH SDMMC_PART_VDP_NAME " " SD_BASE_PATH " -o rw,errors=continue");
		printf("===================>>> %s <<<================\n", cmd);
		system(cmd);
	}

	memset(cmd, 0, sizeof(cmd));
	if (access(SDMMC_DEVICE_PATH SDMMC_PART_TUYA_NAME, F_OK) == 0) /* 主分区 */
	{
		sprintf(cmd, "mount -t vfat " SDMMC_DEVICE_PATH SDMMC_PART_TUYA_NAME " " SD_TUYA_PATH " -o rw,errors=continue");
		printf("===================>>> %s <<<================\n", cmd);
		system(cmd);
	}
}

/***
** 日期: 2022-05-17 10:46
** 作者: leo.liu
** 函数作用：文件系统检测任务
** 返回参数说明： 
***/
static void *media_filelist_task(void *arg)
{
	bool insert = false;
	printf("***** media file task create sccess ! *****\n");
	system("echo 0 > /sys/mmc_en/mmc2_card_pwr_en enable");
	usleep(500 * 1000);
	system("echo 1 > /sys/mmc_en/mmc2_card_pwr_en enable");
	usleep(2000 * 1000);

	while (1)
	{

		pthread_mutex_lock(&file_list_mutex);
		insert = _media_sd_insert_check();
		if (insert != is_media_sd_insert)
		{
			is_media_sd_insert = insert;
			if (is_media_sd_insert == true)
			{
				if (access(SD_BASE_PATH, F_OK) != 0)
				{
					_media_dir_create(SD_BASE_PATH);
				}
				if (access(SD_TUYA_PATH, F_OK) != 0)
				{
					_media_dir_create(SD_TUYA_PATH);
				}

				// _sd_card_repartition();
				static bool ret = false;
				for(int i = 0;i < 2; i++)
				{
					block0_sector = 0, p1_sector = 0, p2_sector = 0, sector_size = 0, other_sector = 0;
					ret = _sd_card_repartition_detection();
					usleep(1 * 1000);
				}
				if(ret == true)
				{
					_sd_card_subzone();
				}
				

				mount_sdcard_dev();

				// /***** 挂载媒体 *****/
				// system("mount ""/dev/mmcblk0 " SD_BASE_PATH);

				if (access(SD_MEDIA_PATH, F_OK) != 0)
				{
					_media_dir_create(SD_MEDIA_PATH);
				}
				_scan_media_file(FILE_TYPE_VIDEO);

				if (access(SD_MUSIC_PATH, F_OK) != 0)
				{
					_media_dir_create(SD_MUSIC_PATH);
				}
				_scan_media_file(FILE_TYPE_SD_MUSIC);

				if (access(SD_PICTURE_PATH, F_OK) != 0)
				{
					_media_dir_create(SD_PICTURE_PATH);
				}
				_scan_media_file(FILE_TYPE_SD_PICTURE);

				is_media_sd_full = false;
				if (_media_sd_full_check() == true)
				{
					printf("sd space full \n");
					is_media_sd_full = true;
				}
				if (sd_format_status == true)
				{

					sd_format_status = false;
				}
			}
			else
			{
				is_media_sd_full = false;
				system("umount " SD_BASE_PATH);
				system("umount " SD_TUYA_PATH);
				printf("========================>>> 卸载SD卡\n");
			}

			lv_msg_send_cmd(MSG_EVENT_CMD_SD_STATE, 0, 0);
		}
		pthread_mutex_unlock(&file_list_mutex);

		if (sd_format_flg == true)
		{
			printf("###################start format sd \n");
			sd_format_status = true;
			if (is_media_sd_insert == false)
			{
				sd_format_flg = false;
			}
			else
			{ 
				pthread_mutex_lock(&file_list_mutex);
				is_media_sd_insert = false;
				is_media_sd_full = false;
				pthread_mutex_unlock(&file_list_mutex);
				_media_sd_format();
				sd_format_flg = false;
			}
		}
		usleep(50 * 1000);
	}
	return NULL;
}

/***
** 日期: 2022-05-17 10:40
** 作者: leo.liu
** 函数作用：媒体文件初始化
** 返回参数说明：
***/
void media_file_list_init(void)
{
	p_media_sd = (file_info *)ak_mem_alloc(MODULE_ID_APP, sizeof(file_info) * SD_MEDIA_NUM_MAX);
	p_media_flash = (file_info *)ak_mem_alloc(MODULE_ID_APP, sizeof(file_info) * FLASH_PHOTO_MAX);
	p_sd_music = (file_info *)ak_mem_alloc(MODULE_ID_APP, sizeof(file_info) * SD_MUSIC_MAX);
	p_sd_background = (file_info *)ak_mem_alloc(MODULE_ID_APP, sizeof(file_info) * SD_PICTURE_MAX);
	pthread_mutex_init(&file_list_mutex, NULL);

	_scan_media_file(FILE_TYPE_FLASH_PHOTO);
	pthread_t thread_t;
	pthread_create(&thread_t, user_pthread_atter_get(), media_filelist_task, NULL);
}

/***
** 日期: 2022-05-17 11:26
** 作者: leo.liu
** 函数作用：判断sd插入
** 返回参数说明：
***/
bool media_sdcard_insert_check(void)
{
	// pthread_mutex_lock(&file_list_mutex);
	bool insert = is_media_sd_insert;
	// pthread_mutex_unlock(&file_list_mutex);
	return insert;
}
/***
** 日期: 2022-05-17 14:40
** 作者: leo.liu
** 函数作用：判断sd文件是否已经满
** 返回参数说明：
***/
bool media_sdcard_full_check(void)
{
	pthread_mutex_lock(&file_list_mutex);
	bool full = is_media_sd_full;
	pthread_mutex_unlock(&file_list_mutex);
	return full;
}
/***
** 日期: 2022-05-17 11:28
** 作者: leo.liu
** 函数作用：创建一个文件
** 返回参数说明：
***/
bool media_file_create(file_type type, char ch, char mode, char *file_path)
{
	pthread_mutex_lock(&file_list_mutex);
	if ((type != FILE_TYPE_FLASH_PHOTO) && (is_media_sd_insert == false))
	{
		pthread_mutex_unlock(&file_list_mutex);
		return false;
	}

	file_info *p_array = NULL;
	char *path = NULL;
	if (type != FILE_TYPE_FLASH_PHOTO)
	{
		/***** 判断sd容量 *****/
		while ((sd_media_total >= SD_MEDIA_NUM_MAX) || (_media_sd_full_check() == true))
		{
			_media_file_delete(type, 0, true);

			if (sd_media_total < 1)
			{
				break;
			}
		}
		path = SD_MEDIA_PATH;
		p_array = &p_media_sd[sd_media_total];
		sd_media_total++;
		sd_media_new_total++;
	}
	else
	{
		if (flash_media_total >= FLASH_PHOTO_MAX)
		{
			_media_file_delete(type, 0, true);
		}
		path = FLASH_PHOTO_PATH;
		p_array = &p_media_flash[flash_media_total];
		flash_media_total++;
		flash_media_new_total++;
	}

	p_array->ch = ch;
	p_array->is_new = true;
	p_array->mode = mode;
	p_array->type = type;

	struct tm date;
	user_time_read(&date);

	int count = 0;
	do
	{

		snprintf(p_array->file_name, MEDIA_PATH_MAX, "%02d%02d%02d-%02d%02d%02d-%d%d%s",
				 date.tm_year % 100,
				 date.tm_mon,
				 date.tm_mday,
				 date.tm_hour,
				 date.tm_min,
				 date.tm_sec,
				 ch,
				 mode,
				 type == FILE_TYPE_VIDEO ? VIDEO_DOT : PHOTO_DOT);

		strcpy(file_path, path);

		strcat(file_path, p_array->file_name);

		date.tm_sec++;
		date.tm_sec %= 60;
		count++;

	} while (access(file_path, F_OK) == 0);

	if (count > 60)
	{
		pthread_mutex_unlock(&file_list_mutex);
		return false;
	}
	_media_file_sync(type);
	pthread_mutex_unlock(&file_list_mutex);

	return true;
}

/***
** 日期: 2022-05-17 13:36
** 作者: leo.liu
** 函数作用：文件总数获取
** 返回参数说明：
***/
bool media_file_total_get(file_type type, int *total, int *new_total)
{
	bool reslut = true;
	pthread_mutex_lock(&file_list_mutex);
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		if (total != NULL)
		{
			*total = flash_media_total;
		}
		if (new_total != NULL)
		{
			*new_total = flash_media_new_total;
		}
	}
	else if (type == FILE_TYPE_SD_MUSIC)
	{
		if (is_media_sd_insert == false)
		{
			reslut = false;
			goto finish;
		}
		if (total != NULL)
		{
			*total = sd_music_total;
		}
		if (new_total != NULL)
		{
			*new_total = sd_music_new_total;
		}
	}
	else if (type == FILE_TYPE_SD_PICTURE)
	{
		if (is_media_sd_insert == false)
		{
			reslut = false;
			goto finish;
		}
		if (total != NULL)
		{
			*total = sd_background_total;
		}
		if (new_total != NULL)
		{
			*new_total = sd_background_new_total;
		}
	}
	else
	{
		if (is_media_sd_insert == false)
		{
			reslut = false;
			goto finish;
		}

		if (total != NULL)
		{
			*total = sd_media_total;
		}
		if (new_total != NULL)
		{
			*new_total = sd_media_new_total;
		}
	}
finish:
	pthread_mutex_unlock(&file_list_mutex);
	return reslut;
}
/***
** 日期: 2022-05-17 13:44
** 作者: leo.liu
** 函数作用：清除新文件标志
** 返回参数说明：
***/
bool media_file_new_clear(file_type type, int index)
{
	pthread_mutex_lock(&file_list_mutex);
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		if ((is_media_sd_insert == true) && (index <= sd_media_total) && (p_media_sd[index].is_new != false)&& (sd_media_new_total > 0))
		{
			sd_media_new_total--;
			p_media_sd[index].is_new = false;
			_media_file_sync(FILE_TYPE_SD_PHOTO);
		}
		else
		{
			if (index >= flash_media_total)
			{
				pthread_mutex_unlock(&file_list_mutex);
				return false;
			}
			if ((p_media_flash[index].is_new != false) && (flash_media_new_total > 0))
			{
				flash_media_new_total--;
				p_media_flash[index].is_new = false;
				_media_file_sync(type);
			}
		}
	}
	else
	{
		if ((is_media_sd_insert == false) || (index >= sd_media_total))
		{
			pthread_mutex_unlock(&file_list_mutex);
			return false;
		}
		if ((p_media_sd[index].is_new != false) && (sd_media_new_total > 0))
		{
			sd_media_new_total--;
			p_media_sd[index].is_new = false;
			_media_file_sync(type);
		}
	}
	pthread_mutex_unlock(&file_list_mutex);
	return true;
}

/***
** 日期: 2022-05-17 13:53
** 作者: leo.liu
** 函数作用：删除一个文件
** 返回参数说明：
***/
bool media_file_delete(file_type type, int index)
{
	pthread_mutex_lock(&file_list_mutex);
	bool reslut = _media_file_delete(type, index, true);
	pthread_mutex_unlock(&file_list_mutex);
	return reslut;
}
/***
** 日期: 2022-05-17 13:54
** 作者: leo.liu
** 函数作用：判断该文件是否存在
** 返回参数说明：
***/
bool media_file_bad_check(const char *file)
{
	pthread_mutex_lock(&file_list_mutex);
	if (access(file, F_OK) != 0)
	{
		if (strstr(file, SD_MEDIA_PATH) != NULL)
		{
			for (int i = (sd_media_total - 1); i >= 0; i++)
			{
				if (strstr(file, p_media_sd[i].file_name) != NULL)
				{
					_media_file_delete(FILE_TYPE_VIDEO, i, true);
					break;
				}
			}
		}
		else if (strstr(file, FLASH_PHOTO_PATH) != NULL)
		{
			for (int i = (flash_media_total - 1); i >= 0; i++)
			{
				if (strstr(file, p_media_flash[i].file_name) != NULL)
				{
					_media_file_delete(FILE_TYPE_FLASH_PHOTO, i, true);
					break;
				}
			}
		}
	}
	pthread_mutex_unlock(&file_list_mutex);
	return true;
}
/***
**   日期:2022-05-23 09:51:21
**   作者: leo.liu
**   函数作用：获取格式化sd卡状态
**   参数说明:
***/
bool media_format_sd_state(void)
{
	bool state = (sd_format_status | sd_format_flg) ? true : false; // sd_format_flg;
	return state;
}
bool file_format_sd_state(void)
{
	bool state = (sd_format_flg) ? true : false; // sd_format_flg;
	return state;
}
/***
**   日期:2022-05-23 09:51:21
**   作者: leo.liu
**   函数作用：获取格式化sd卡状态
**   参数说明:
***/
bool media_del_file_state(void)
{
	bool state = file_del_status ? true : false;
	return state;
}

void media_del_file_state_set(bool en)
{
	file_del_status = en;
}

/***
**   日期:2022-05-23 09:58:49
**   作者: leo.liu
**   函数作用：格式话sd卡
**   参数说明:
***/
void media_format_sd(void)
{
	sd_format_flg = true;
}

/***
**   日期:2022-05-23 14:34:43
**   作者: leo.liu
**   函数作用：获取文件信息
**   参数说明:
***/
const file_info *media_file_info_get(file_type type, int index)
{
	const file_info *pinfo = NULL;
	pthread_mutex_lock(&file_list_mutex);
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		if (index < flash_media_total)
		{
			pinfo = &p_media_flash[index];
		}
		else
		{
			printf("find flash media index:%d failed \n", index);
		}
	}
	else if (type == FILE_TYPE_SD_MUSIC)
	{
		if (index < sd_music_total)
		{
			pinfo = &p_sd_music[index];
		}
		else
		{
			printf("find sd card music index:%d failed \n", index);
		}
	}
	else if (type == FILE_TYPE_SD_PICTURE)
	{
		if (index < sd_background_total)
		{
			pinfo = &p_sd_background[index];
		}
		else
		{
			printf("find sd card background index:%d failed \n", index);
		}
	}
	else
	{
		if ((is_media_sd_insert == true) && (index < sd_media_total))
		{
			pinfo = &p_media_sd[index];
		}
		else
		{
			printf("find sd media index:%d failed \n", index);
		}
	}
	pthread_mutex_unlock(&file_list_mutex);
	return pinfo;
}
/***
**   日期:2022-06-15 11:14:00
**   作者: leo.liu
**   函数作用：删除所有文件
**   参数说明:
***/
bool media_file_delete_all(file_type type, bool all)
{
	pthread_mutex_lock(&file_list_mutex);
	if (type == FILE_TYPE_FLASH_PHOTO)
	{
		system("rm -rf " FLASH_PHOTO_PATH);
		_scan_media_file(type);
	}
	else if (all == true)
	{
		system("rm -rf " SD_MEDIA_PATH);
		_scan_media_file(type);
	}
	else if (is_media_sd_insert == true)
	{
		int total = sd_media_total;
		printf("\n\n==========total[%d]\n\n",total);
		for (int i = 0; i < total; i++)
		{
			if (type == p_media_sd[i].type)
			{
				_media_file_delete(type, i, false);
				total = sd_media_total;
				i = -1;
			}
			else if(type == FILE_TYPE_PHOTO) 							/*应对删除SD卡中的flash文件*/
			{
				if (p_media_sd[i].type == FILE_TYPE_FLASH_PHOTO)
				{
					_media_sd_flash_file_delete(FILE_TYPE_FLASH_PHOTO,i,false);
					total = sd_media_total;
					i = -1;
				}
			}
		}
		_media_file_sync(type);
		system("sync");
	}
	system("sync");
	pthread_mutex_unlock(&file_list_mutex);
	return true;
}

bool flash_backup_to_sd()
{
	if (!_media_sd_insert_check())
	{
		return false;
	}

	if (access(SD_BASE_PATH, F_OK) != 0)
	{
		_media_dir_create(SD_BASE_PATH);
	}
	if (access("/dev/mmcblk0p1", F_OK) == 0)
	{
		system("mount /dev/mmcblk0p1 " SD_BASE_PATH);
	}
	else
	{
		system("mount /dev/mmcblk0 " SD_BASE_PATH);
	}
	if (access(FLASH_MEDIA_BCAKUP_PATH, F_OK) != 0)
	{
		/***** 文件夹不存在 *****/
		system("mkdir " FLASH_MEDIA_BCAKUP_PATH);
		printf("mkdir " FLASH_MEDIA_BCAKUP_PATH "\n");
	}
	_scan_media_file(FILE_TYPE_FLASH_PHOTO);
	int fd_flash = open(FLASH_MEDIA_CACHE_PATH, O_CREAT | O_WRONLY);

	int fd_sd_backup_cache = open(FLASH_MEDIA_CACHE_BCAKUP_PATH, O_CREAT | O_WRONLY | O_APPEND);

	if (fd_flash < 0)
	{
		printf("open %s failed \n", FLASH_MEDIA_CACHE_PATH);
		return false;
	}
	if (fd_sd_backup_cache < 0)
	{
		printf("open %s failed \n", SD_MEDIA_CACHE_PATH);
		return false;
	}
	write(fd_sd_backup_cache, p_media_flash, flash_media_total * sizeof(file_info));

	system("cp -r " FLASH_PHOTO_PATH "* " FLASH_MEDIA_BCAKUP_PATH);

	close(fd_flash);
	close(fd_sd_backup_cache);

	return true;
}




void copy_flash_photo_to_sd()
{
	
	int total = 0;
	media_file_total_get(FILE_TYPE_FLASH_PHOTO,&total,NULL);
	if(total <= 0)
	{
		return;
	}

	for(int i = 0; i < total; i++)
	{
		const file_info *p = media_file_info_get(FILE_TYPE_FLASH_PHOTO,i);
		char cmd[128] = {0};
		char cmd_cp[128] = {0};
		snprintf(cmd , sizeof(cmd)-1,"cat %s | fgrep %s ",SD_MEDIA_CACHE_PATH,p->file_name);
		FILE *fp = popen(cmd,"r");
		if(!fp)
		{
			printf("popen fail for r...\n");
			return;
		}
		char buffer[64] = {0};
		fgets(buffer,sizeof(buffer)-1,fp);
		pclose(fp);

		if(buffer[0] != '\0')
		{
			continue;
		}

		if(sd_media_total >= SD_MEDIA_NUM_MAX)
		{
			printf("----->>cp file num > sdcard file num\n");
			goto finish;
		}

		if(_media_sd_full_check() == true)
		{
			goto finish;
		}

		snprintf(cmd_cp,128,"cp -rf %s/%s %s ",FLASH_PHOTO_PATH,p->file_name,SD_MEDIA_PATH);
		system(cmd_cp);

		file_info* p_sd = &p_media_sd[sd_media_total];
		memcpy(p_sd,p,sizeof(file_info));
		p_sd->is_new = 1;
		sd_media_total++;
		sd_media_new_total++;

	}
	

finish:
	_media_file_sync(FILE_TYPE_VIDEO);
	_scan_media_file(FILE_TYPE_VIDEO);
	_scan_media_file(FILE_TYPE_FLASH_PHOTO);


}


