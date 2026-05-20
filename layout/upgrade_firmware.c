#include <stdbool.h>
#include <linux/fb.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "string.h"
#include "tuya/tuya_uuid_and_key.h"
extern void *osal_fb_mmap_viraddr(int fb_len, int fb_fd);
/*
dev:    size   erasesize  name
mtd0: 00037000 00001000 "UBOOT"
mtd1: 00001000 00001000 "ENV"
mtd2: 00001000 00001000 "ENVBK"
mtd3: 00010000 00001000 "DTB"
mtd4: 00190000 00001000 "KERNEL"
mtd5: 00025000 00001000 "LOGO"
mtd6: 001b6000 00001000 "ROOTFS"
mtd7: 0097e000 00001000 "CONFIG"
mtd8: 002ce000 00001000 "APP"
*/

#define FB_PATH "/dev/fb0"
#define FIRMWARE_PATH "/mnt/.upgrade/tmp/"

static int upgrade_total = 0;
static int upgrade_count = 0;

static unsigned char *fb_adder = NULL;
static struct fb_var_screeninfo var_info;
static struct fb_fix_screeninfo fix_info;

static bool fb_init(void)
{
	int fd = open(FB_PATH, O_RDWR);
	if (fd < 0)
	{
		printf("open %s failed \n", FB_PATH);
		return false;
	}
	/***** 获取fb的相关的信息 *****/

	ioctl(fd, FBIOGET_VSCREENINFO, &var_info);

	var_info.activate |= FB_ACTIVATE_FORCE;
	var_info.activate |= FB_ACTIVATE_NOW;
	var_info.xres = var_info.xres_virtual;
	var_info.yres = var_info.yres_virtual;
	/***** 设置RGB888格式 *****/
	var_info.bits_per_pixel = 24; // 16;
	var_info.red.offset = 16;     // 11;
	var_info.red.length = 8;      // 5;
	var_info.green.offset = 8;    // 5;
	var_info.green.length = 8;    // 6;
	var_info.blue.offset = 0;     // 0;
	var_info.blue.length = 8;     // 5;
	ioctl(fd, FBIOPUT_VSCREENINFO, &var_info);

	/***** 获取不可变参数 *****/
	ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);

	/***** 初始化tde 需要的fb layer ******/
	fb_adder = (unsigned char *)osal_fb_mmap_viraddr(fix_info.smem_len, fd);
	memset((void *)fb_adder, 0x00, fix_info.smem_len);
	var_info.reserved[0] = 0;
	ioctl(fd, FBIOPUT_VSCREENINFO, &var_info);
	return true;
}

/***
** 日期: 2022-05-18 15:18
** 作者: leo.liu
** 函数作用：画矩形
** 返回参数说明：
***/
static void fb_rect_draw(int x, int y, int w, int h, char b, char g, char r)
{
	unsigned char *addr = fb_adder + y * 1024 * 3 + x * 3;
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			addr[i * 3 + 2] = r;
			addr[i * 3 + 1] = g;
			addr[i * 3] = b;
		}
		addr += 1024 * 3;
	}
}
/***
** 日期: 2022-05-18 15:18
** 作者: leo.liu
** 函数作用：初始化矩形框
** 返回参数说明：
***/
static void fb_rect_init(void)
{
	/***** x:394 y:288 w:13 h:704 *****/
	fb_rect_draw(210, 296, 604, 1, 0x26, 0x5c, 0xFF);
	fb_rect_draw(210 + 604 - 1, 296, 1, 12, 0x26, 0x5c, 0xFF);
	fb_rect_draw(210, 296, 1, 12, 0x26, 0x5c, 0xFF);
	fb_rect_draw(210, 296 + 12 - 1, 604, 1, 0x26, 0x5c, 0xFF);
}

/***
** 日期: 2022-05-18 16:06
** 作者: leo.liu
** 函数作用：进度条显示
** 返回参数说明：
***/
static void upgrade_fb_progress_display(void)
{
	fb_rect_draw(212, 298, upgrade_total ? (upgrade_count * 600 / upgrade_total) : 600, 8, 0x26, 0x5c, 0xFF);
}

/***
** 日期: 2022-05-18 14:51
** 作者: leo.liu
** 函数作用：检测板级文件升级
** 返回参数说明：
***/
static unsigned int platform_mask = 0x00;
static void detect_platform_check(void)
{
	platform_mask = 0x00;
	/*  if (access(FIRMWARE_PATH "platform/u-boot.bin", F_OK) == 0)
	 {
	     upgrade_total++;
	     platform_mask |= 0x01;
	     printf("find:u-boot.bin \n");
	 } */
	if (access(FIRMWARE_PATH "platform/env_ak3761d_nor.img", F_OK) == 0)
	{
		upgrade_total += 2;
		platform_mask |= 0x02;
		printf("find:env_ak3761d_nor.img \n");
	}
	if (access(FIRMWARE_PATH "platform/c500_cbd_ak3761d_dsi_CDV_70QT_v1.0.0.dtb", F_OK) == 0)
	{
		upgrade_total++;
		platform_mask |= 0x04;
		printf("find:c500_cbd_ak3761d_dsi_CDV_70QT_v1.0.0.dtb \n");
	}
	if (access(FIRMWARE_PATH "platform/uImage", F_OK) == 0)
	{
		upgrade_total++;
		platform_mask |= 0x08;
		printf("find:uImage \n");
	}
	if (access(FIRMWARE_PATH "platform/cdv70qt_logo.rgb", F_OK) == 0)
	{
		upgrade_total++;
		platform_mask |= 0x10;
		printf("find:cdv1004qt_logo.rgb \n");
	}
	if (access(FIRMWARE_PATH "platform/root.sqsh4", F_OK) == 0)
	{
		upgrade_total++;
		platform_mask |= 0x20;
		printf("find:root.sqsh4 \n");
	}
	if (access(FIRMWARE_PATH "platform/usr.jffs2", F_OK) == 0)
	{
		upgrade_total++;
		platform_mask |= 0x40;
		printf("find:usr.jffs2 \n");
	}
	if (access(FIRMWARE_PATH "platform/usr.sqsh4", F_OK) == 0)
	{
		upgrade_total++;
		platform_mask |= 0x80;
		printf("find:usr.sqsh4 \n");
	}
}

/***
** 日期: 2022-05-18 15:52
** 作者: leo.liu
** 函数作用：获取文件大小
** 返回参数说明：
***/
static int upgrade_file_size_get(const char *path)
{
	int fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		return -1;
	}
	int size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	return size;
}

/***
** 日期: 2022-05-18 15:52
** 作者: leo.liu
** 函数作用：升级u-boot
** 返回参数说明：
***/
#if 0
 static bool upgrade_u_boot_firmware(void)
{
    int size = upgrade_file_size_get(FIRMWARE_PATH "platform/u-boot.bin");
    if (size < 0)
    {
        return false;
    }
    /* mtd0: 00037000 00001000 "UBOOT" */
    system("mtd_debug erase /dev/mtd0 0 0x00037000");

    char cmd[128] = {0};
    sprintf(cmd, "mtd_debug write /dev/mtd0 0 %d %s", size, FIRMWARE_PATH "platform/u-boot.bin");
    printf(" %s \n",cmd);
    system(cmd);
    upgrade_count++;
    return true;
}
#endif

static bool upgrade_nor_img_firmware(void)
{
	/*
    mtd1: 00001000 00001000 "ENV"
    mtd2: 00001000 00001000 "ENVBK"
     */
	int size = upgrade_file_size_get(FIRMWARE_PATH "platform/env_ak3761d_nor.img");
	if ((size < 0) || (size > 0x00001000))
	{
		return false;
	}

	system("mtd_debug erase /dev/mtd1 0 0x00001000");
	system("mtd_debug erase /dev/mtd2 0 0x00001000");

	char cmd[128] = {0};
	sprintf(cmd, "mtd_debug write /dev/mtd1 0 %d %s", size, FIRMWARE_PATH "platform/env_ak3761d_nor.img");
	printf(" %s \n", cmd);
	system(cmd);

	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "mtd_debug write /dev/mtd2 0 %d %s", size, FIRMWARE_PATH "platform/env_ak3761d_nor.img");
	printf(" %s \n", cmd);
	system(cmd);

	upgrade_count++;
	return true;
}

static bool upgrade_dtb_firmware(void)
{
	/* mtd3: 00010000 00001000 "DTB"" */
	int size = upgrade_file_size_get(FIRMWARE_PATH "platform/c500_cbd_ak3761d_dsi_CDV_70QT_v1.0.0.dtb");
	if ((size < 0) || (size > 0x00010000))
	{
		return false;
	}

	system("mtd_debug erase /dev/mtd3 0 0x00010000");

	char cmd[128] = {0};
	sprintf(cmd, "mtd_debug write /dev/mtd3 0 %d %s", size, FIRMWARE_PATH "platform/c500_cbd_ak3761d_dsi_CDV_70QT_v1.0.0.dtb");
	printf(" %s \n", cmd);
	system(cmd);
	upgrade_count++;
	return true;
}

static bool upgrade_uimage_firmware(void)
{
	int size = upgrade_file_size_get(FIRMWARE_PATH "platform/uImage");
	if ((size < 0) || (size > 0x00190000))
	{
		return false;
	}
	/* mtd4: 00190000 00001000 "KERNEL" */
	system("mtd_debug erase /dev/mtd4 0 0x00190000");

	char cmd[128] = {0};
	sprintf(cmd, "mtd_debug write /dev/mtd4 0 %d %s", size, FIRMWARE_PATH "platform/uImage");
	printf(" %s \n", cmd);
	system(cmd);
	upgrade_count++;
	return true;
}

static bool upgrade_logo_firmware(void)
{
	int size = upgrade_file_size_get(FIRMWARE_PATH "platform/cdv1004qt_logo.rgb");
	if ((size < 0) || (size > 0x00001000))
	{
		return false;
	}
	/* mtd5: 00025000 00001000 "LOGO" */
	system("mtd_debug erase /dev/mtd5 0 0x00025000");

	char cmd[128] = {0};
	sprintf(cmd, "mtd_debug write /dev/mtd5 0 %d %s", size, FIRMWARE_PATH "platform/cdv1004qt_logo.rgb");
	printf(" %s \n", cmd);
	system(cmd);
	upgrade_count++;
	return true;
}

static bool upgrade_rootfs_firmware(void)
{
	int size = upgrade_file_size_get(FIRMWARE_PATH "platform/root.sqsh4");
	if ((size < 0) || (size > 0x001b6000))
	{
		return false;
	}
	/* mtd6: 001b6000 00001000 "ROOTFS" */
	system("mtd_debug erase /dev/mtd6 0 0x001b6000");

	char cmd[128] = {0};
	sprintf(cmd, "mtd_debug write /dev/mtd6 0 %d %s", size, FIRMWARE_PATH "platform/root.sqsh4");
	printf(" %s \n", cmd);
	system(cmd);
	upgrade_count++;
	return true;
}
static bool upgrade_config_firmware(void)
{
	int size = upgrade_file_size_get(FIRMWARE_PATH "platform/usr.jffs2");
	if ((size < 0) || (size > 0x0097e000))
	{
		return false;
	}
	/*mtd7: 0097e000 00001000 "CONFIG" */
	system("mtd_debug erase /dev/mtd7 0 0x0097e000");

	char cmd[128] = {0};
	sprintf(cmd, "mtd_debug write /dev/mtd7 0 %d %s", size, FIRMWARE_PATH "platform/usr.jffs2");
	printf(" %s \n", cmd);
	system(cmd);
	upgrade_count++;
	return true;
}

static bool upgrade_usr_firmware(void)
{
	int size = upgrade_file_size_get(FIRMWARE_PATH "platform/usr.sqsh4");
	if ((size < 0) || (size > 0x002ce000))
	{
		return false;
	}
	/*mtd8: 002ce000 00001000 "APP"*/
	system("mtd_debug erase /dev/mtd8 0 0x002ce000");

	char cmd[128] = {0};
	sprintf(cmd, "mtd_debug write /dev/mtd8 0 %d %s", size, FIRMWARE_PATH "platform/usr.sqsh4");
	printf(" %s \n", cmd);
	system(cmd);
	upgrade_count++;
	return true;
}
/***
** 日期: 2022-05-18 15:04
** 作者: leo.liu
** 函数作用：更新板机文件
** 返回参数说明：
***/
static void upgrade_platform_firmware(void)
{
	/*  if ((platform_mask & 0x01) && (access(FIRMWARE_PATH "platform/u-boot.bin", F_OK)==0))
	 {
	     upgrade_u_boot_firmware();
	     upgrade_fb_progress_display();
	 } */
	if ((platform_mask & 0x02) && (access(FIRMWARE_PATH "platform/env_ak3761d_nor.img", F_OK) == 0))
	{
		upgrade_nor_img_firmware();
		upgrade_fb_progress_display();
	}
	if ((platform_mask & 0x04) && (access(FIRMWARE_PATH "platform/c500_cbd_ak3761d_dsi_CDV_70QT_v1.0.0.dtb", F_OK) == 0))
	{
		upgrade_dtb_firmware();
		upgrade_fb_progress_display();
	}
	if ((platform_mask & 0x08) && (access(FIRMWARE_PATH "platform/uImage", F_OK) == 0))
	{
		upgrade_uimage_firmware();
		upgrade_fb_progress_display();
	}
	if ((platform_mask & 0x10) && (access(FIRMWARE_PATH "platform/cdv1004qt_logo.rgb", F_OK) == 0))
	{
		upgrade_logo_firmware();
		upgrade_fb_progress_display();
	}
	if ((platform_mask & 0x20) && (access(FIRMWARE_PATH "platform/root.sqsh4", F_OK) == 0))
	{
		upgrade_rootfs_firmware();
		upgrade_fb_progress_display();
	}
	if ((platform_mask & 0x40) && (access(FIRMWARE_PATH "platform/usr.jffs2", F_OK) == 0))
	{
		upgrade_config_firmware();
		upgrade_fb_progress_display();
	}
	if ((platform_mask & 0x80) && (access(FIRMWARE_PATH "platform/usr.sqsh4", F_OK) == 0))
	{
		upgrade_usr_firmware();
		upgrade_fb_progress_display();
	}
}

/***
** 日期: 2022-05-18 14:54
** 作者: leo.liu
** 函数作用：铃声文件检测
** 返回参数说明：
***/
static unsigned int rings_mask = 0x00;
static void detect_rings_check(void)
{
	rings_mask = 0x00;
	if (access(FIRMWARE_PATH "rings/", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x01;
		printf("find:rings \n");
	}

#if 0
	rings_mask = 0x00;
	if (access(FIRMWARE_PATH "rings/1.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x01;
		printf("find:1.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/2.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x02;
		printf("find:2.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/3.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x04;
		printf("find:3.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/4.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x08;
		printf("find:4.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/5.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x10;
		printf("find:5.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/6.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x20;
		printf("find:6.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/7.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x40;
		printf("find:7.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/8.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x80;
		printf("find:8.mp3 \n");
	}
	if (access(FIRMWARE_PATH "rings/10.mp3", F_OK) == 0)
	{
		upgrade_total++;
		rings_mask |= 0x100;
		printf("find:10.mp3 \n");
	}
#endif
}

static void upgrade_rings_firmware(void)
{
	if (access("/app/app", F_OK) != 0)
	{
		system("mkdir /app/app");
	}
	if (access("/app/app/rings", F_OK) != 0)
	{
		system("mkdir /app/app/rings");
	}


	if ((rings_mask & 0x01) && (access(FIRMWARE_PATH "rings/", F_OK) == 0))
	{

		system("cp -v " FIRMWARE_PATH "rings/* /app/app/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}

#if 0	

	if ((rings_mask & 0x01) && (access(FIRMWARE_PATH "rings/1.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/1.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/1.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/1.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}


	if ((rings_mask & 0x02) && (access(FIRMWARE_PATH "rings/2.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/2.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/2.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/2.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((rings_mask & 0x04) && (access(FIRMWARE_PATH "rings/3.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/3.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/3.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/3.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((rings_mask & 0x08) && (access(FIRMWARE_PATH "rings/4.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/4.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/4.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/4.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((rings_mask & 0x10) && (access(FIRMWARE_PATH "rings/5.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/5.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/5.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/5.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((rings_mask & 0x10) && (access(FIRMWARE_PATH "rings/6.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/6.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/6.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/6.mp3  /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((rings_mask & 0x20) && (access(FIRMWARE_PATH "rings/7.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/7.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/7.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/7.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((rings_mask & 0x40) && (access(FIRMWARE_PATH "rings/8.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/8.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/8.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/8.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((rings_mask & 0x80) && (access(FIRMWARE_PATH "rings/10.mp3", F_OK) == 0))
	{
		if (access("/etc/config/run/rings/10.mp3", F_OK) == 0)
		{
			system("rm -f /etc/config/run/rings/10.mp3");
		}
		system("cp " FIRMWARE_PATH "rings/10.mp3 /etc/config/run/rings/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
#endif
}

/***
** 日期: 2022-05-18 14:57
** 作者: leo.liu
** 函数作用：更新壁纸检测
** 返回参数说明：
***/
static unsigned int wallpaper_mask = 0x00;
static void detect_wallpaper_check(void)
{
	if (access(FIRMWARE_PATH "wallpaper/frame_1.jpg", F_OK) == 0)
	{
		upgrade_total++;
		wallpaper_mask |= 0x01;
		printf("find:frame_1.jpg \n");
	}
	if (access(FIRMWARE_PATH "wallpaper/frame_2.jpg", F_OK) == 0)
	{
		upgrade_total++;
		wallpaper_mask |= 0x02;
		printf("find:frame_2.jpg \n");
	}
}
static void upgrade_wallpaper_firmware(void)
{
	if (access("/app/app", F_OK) != 0)
	{
		system("mkdir /app/app");
	}
	if (access("/app/app/wallpaper", F_OK) != 0)
	{
		system("mkdir /app/app/wallpaper");
	}
	if ((wallpaper_mask & 0x01) && (access(FIRMWARE_PATH "wallpaper/frame_1.jpg", F_OK) == 0))
	{
		if (access("/app/app/wallpaper/frame_1.jpg", F_OK) == 0)
		{
			system("rm -f /app/app/wallpaper/frame_1.jpg");
		}
		system("cp " FIRMWARE_PATH "wallpaper/frame_1.jpg /app/app/wallpaper/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
	if ((wallpaper_mask & 0x02) && (access(FIRMWARE_PATH "wallpaper/frame_2.jpg", F_OK) == 0))
	{
		if (access("/app/app/wallpaper/frame_2.jpg", F_OK) == 0)
		{
			system("rm -f /app/app/wallpaper/frame_2.jpg");
		}
		system("cp " FIRMWARE_PATH "wallpaper/frame_2.jpg /app/app/wallpaper/");
		upgrade_count++;
		upgrade_fb_progress_display();
	}
}

/***
** 日期: 2022-05-18 14:59
** 作者: leo.liu
** 函数作用：检测app更新
** 返回参数说明：
***/
static unsigned int app_mask = 0x00;
static void detect_app_check(void)
{
	if (access(FIRMWARE_PATH "CDV1004QT.BIN", F_OK) == 0)
	{
		upgrade_total++;
		app_mask |= 0x01;
		printf("find:CDV1004QT.BIN \n");
	}
}
static void upgrade_app_firmware(void)
{
	if ((app_mask & 0x01) && (access(FIRMWARE_PATH "CDV1004QT.BIN", F_OK) == 0))
	{
		if (access("/app/app/CDV1004QT.BIN", F_OK) == 0)
		{
			system("rm -f /app/app/CDV1004QT.BIN");
		}
		system("cp " FIRMWARE_PATH "CDV1004QT.BIN /app/app/");

		upgrade_count++;
		upgrade_fb_progress_display();
	}
}
/***
** 日期: 2022-05-18 15:01
** 作者: leo.liu
** 函数作用：探测ui资源
** 返回参数说明：
***/
static unsigned int rom_bin_mask = 0x00;
static void detect_rom_check(void)
{
	if (access(FIRMWARE_PATH "rom.bin", F_OK) == 0)
	{
		upgrade_total++;
		rom_bin_mask |= 0x01;
		printf("find:rom.bin \n");
	}
}
static void upgrade_rom_bin_firmware(void)
{
	if ((rom_bin_mask & 0x01) && (access(FIRMWARE_PATH "rom.bin", F_OK) == 0))
	{
		if (access("/app/app/rom.bin", F_OK) == 0)
		{
			system("rm -f /app/app/rom.bin");
		}
		system("cp " FIRMWARE_PATH "rom.bin /app/app/");

		upgrade_count++;
		upgrade_fb_progress_display();
	}
}

/***
** 日期: 2022-05-18 15:02
** 作者: leo.liu
** 函数作用：检测字库更新
** 返回参数说明：
***/
static unsigned int font_mask = 0x00;
static void detect_font_check(void)
{
	if (access(FIRMWARE_PATH "sat_leo.ttf", F_OK) == 0)
	{
		upgrade_total++;
		font_mask |= 0x01;
		printf("find:sat_leo.ttf \n");
	}
}

static void upgrade_font_firmware(void)
{
	if ((font_mask & 0x01) && (access(FIRMWARE_PATH "sat_leo.ttf", F_OK) == 0))
	{
		if (access("/app/app/sat_leo.ttf", F_OK) == 0)
		{
			system("rm -f /app/app/sat_leo.ttf");
		}
		system("cp " FIRMWARE_PATH "sat_leo.ttf /app/app/");

		upgrade_count++;
		upgrade_fb_progress_display();
	}
}

/***
** 日期: 2022-05-18 15:03
** 作者: leo.liu
** 函数作用：更新固件
** 返回参数说明：
***/
static void upgrade_firmware(void)
{
	if (platform_mask != 0)
	{
		system("rm -rf /mnt/.upgrade/kernel_backup");
		system("mkdir /mnt/.upgrade/kernel_backup");
		system("tar -xvf /mnt/.upgrade/CDV1004QTOS -C /mnt/.upgrade/kernel_backup");
		system("rm -rf /mnt/.upgrade/CDV1004QTOS");
		system("rm -rf /mnt/.upgrade/kernel_backup/platform");
		chdir("/mnt/.upgrade/kernel_backup/");
		system("tar -cvf CDV1004QTOS *");
		system("mv /mnt/.upgrade/kernel_backup/CDV1004QTOS /mnt/.upgrade/");
		system("rm /mnt/.upgrade/kernel_backup -rf");

		upgrade_platform_firmware();
		system("sync");
		system("reboot ");
		while (1)
			;
	}
	if (rings_mask != 0)
	{
		upgrade_rings_firmware();
	}
	if (wallpaper_mask != 0)
	{
		upgrade_wallpaper_firmware();
	}
	if (app_mask != 0)
	{
		upgrade_app_firmware();
	}
	if (rom_bin_mask != 0)
	{
		upgrade_rom_bin_firmware();
	}
	if (font_mask != 0)
	{
		upgrade_font_firmware();
	}
	system("rm -rf " FIRMWARE_PATH);
	system("sync");
}
#if 0
/***
**   日期:2022-06-10 15:27:35
**   作者: leo.liu
**   函数作用：清空tuya数据
**   参数说明:
***/
static void tuya_user_data_clear(void)
{
	system("rm -rf " TUYA_UUID_AND_KEY_CONF_PATH);
	system("mkdir " TUYA_UUID_AND_KEY_CONF_PATH);
	system("cp " FIRMWARE_PATH "2022-05-27.conf " TUYA_UUID_AND_KEY_CONF_PATH);
	system("sync");
}
#endif
void upgrade_check_firmware(void)
{
	printf("find frimware start fenqu \n");
	
	fb_init();
	// fb_rect_init();

	/***** 检测板级文件更新 *****/
	detect_platform_check();
	if (platform_mask)
	{
		goto platform;
	}
	/***** 检测铃声升级*****/
	detect_rings_check();

	/***** 检测壁纸更新 *****/
	detect_wallpaper_check();

	/***** 检测执行文件 *****/
	detect_app_check();

	/***** 检测ui资源 *****/
	detect_rom_check();

	/***** 探测字库 *****/
	detect_font_check();
platform:
	if (upgrade_total > 0)
	{
		// fb_init();
		fb_rect_init();
		upgrade_firmware();
		printf("upgrade finish \n");
		while (1)
		{
			fb_rect_draw(0, 0, 1024, 600, 0xFF, 0, 0);
			usleep(1000 * 1000);
			fb_rect_draw(0, 0, 1024, 600, 0xFF, 0xFF, 0xFF);
			usleep(1000 * 1000);
		}
	}
}