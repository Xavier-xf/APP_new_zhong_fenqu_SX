#ifndef __USER_ONVIF_H__
#define __USER_ONVIF_H__

#include <stdbool.h>
#include <stdio.h>
#include "include/sat_ipcamera.h"


typedef struct 
{
    char ip[20];    
    char user[20];       /* 账户 */
    char pswd[20];       /* 密码 */
    // char byname[20];    /* 别名 */

    char rtsp_url[2][128];  /* 地址/链接 */
    int  port;
    int  type;          /* [0]-br0  [1]-wlan0 */
    

}ONVIF_INFO_STRUCT;

ONVIF_INFO_STRUCT onvif_discover_info[8];  /* 源码好像最大值是8 */


typedef struct 
{
    char ip[20];    
    char user[20];       /* 账户 */
    char pswd[20];       /* 密码 */
    // char byname[20];    /* 别名 */

    char rtsp_url[2][256];  /* 地址/链接 */
    int  port;
    int  type;          /* [0]-br0  [1]-wlan0 */
    

}ONVIF_ADD_INFO_STRUCT;

ONVIF_ADD_INFO_STRUCT onvif_add_discover_info[8];



#endif /* __USER_ONVIF_H__ */