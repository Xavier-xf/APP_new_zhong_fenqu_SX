#ifndef _MEDIA_THUMB_H_
#define _MEDIA_THUMB_H_
/***
**   日期:2022-05-23 11:04:28
**   作者: leo.liu
**   函数作用：初始化缩略图设备
**   参数说明:
***/
bool thumb_media_open(void);
/***
**   日期:2022-05-23 11:16:52
**   作者: leo.liu
**   函数作用：关闭缩略图
**   参数说明:
***/
bool thumb_media_close(void);
/***
**   日期:2022-05-23 11:18:45
**   作者: leo.liu
**   函数作用：缩略图显示
**   参数说明:
***/
bool thumb_media_load(int x, int y, int w, int h, const char *file);
/***
**   日期:2022-05-23 13:49:23
**   作者: leo.liu
**   函数作用：清除缩略图buffer
**   参数说明:
***/
bool thumb_media_buffer_clear(void);
#endif