#include "tuya_api.h"
#include "wifi_hwl.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include "tuya_iot_config.h"
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "base_hwl.h"
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "base_hwl.h"
#define WLAN_DEV "wlan0"
OPERATE_RET
hwl_bnw_get_ip(OUT NW_IP_S *ip)
{
	// printf("[%s:%d]---------\n",__func__,__LINE__);
	int sock;
	// char ipaddr[50];

	struct sockaddr_in *sin;
	struct ifreq ifr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket create failse...GetLocalIp!\n");
		return OPRT_COM_ERROR;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, WLAN_DEV, sizeof(ifr.ifr_name) - 1);

	if (ioctl(sock, SIOCGIFADDR, &ifr) < 0)
	{
		printf("ioctl error\n");
		close(sock);
		return OPRT_COM_ERROR;
	}

	sin = (struct sockaddr_in *)&ifr.ifr_addr;
	strcpy(ip->ip, inet_ntoa(sin->sin_addr));
	close(sock);

	return OPRT_OK;
}

BOOL_T hwl_bnw_station_conn(VOID)
{
	//printf("[%s:%d]---------\n",__func__,__LINE__);
	int sock;
	//   struct   sockaddr_in *sin;
	struct ifreq ifr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket create failse...GetLocalIp!\n");
		return OPRT_COM_ERROR;
	}

	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, WLAN_DEV, sizeof(ifr.ifr_name) - 1);

	if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0)
	{
		printf("ioctl error\n");
		close(sock);
		return FALSE;
	}
	close(sock);

	if (0 == (ifr.ifr_flags & IFF_UP))
	{
		printf("0 == (ifr.ifr_flags & IFF_UP\n");
		return FALSE;
	}

	return TRUE;
}
OPERATE_RET hwl_bnw_set_station_connect(IN CONST CHAR_T *ssid, IN CONST CHAR_T *passwd)
{
	// printf("[%s:%d]---------\n",__func__,__LINE__);
	return OPRT_OK;//OPRT_COM_ERROR;
}
BOOL_T hwl_bnw_need_wifi_cfg(VOID)
{
	// printf("[%s:%d]---------\n",__func__,__LINE__);
	return OPRT_OK;//FALSE;
}

OPERATE_RET hwl_bnw_station_get_conn_ap_rssi(OUT SCHAR_T *rssi)
{
	// *rssi = 99;
	// // printf("[%s:%d]---------\n",__func__,__LINE__);
	// return OPRT_OK;

	if(NULL == rssi)
    {
        return OPRT_INVALID_PARM;
    }
    *rssi = 0;

#ifdef __HuaweiLite__
    //todo 
    //liteos system
    //Implementation of RSSI acquisition
#else
    FILE *pp = popen("iwconfig "WLAN_DEV, "r");
    if(pp == NULL)
    {
       return OPRT_COM_ERROR;
    }

    char tmp[128] = {0};
    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        /* find signal  */
        char *pSIGNALStart = strstr(tmp, "Quality=");
        if(pSIGNALStart != NULL)
        {
            int x = 0;
            int y = 0;
            sscanf(pSIGNALStart + strlen("Quality="), "%d/%d",&x,&y);
            *rssi = x * 100/ (y+1);
            break;
        }
    }
    pclose(pp);
#endif
    printf("Get Conn AP RSSI:%d\r\n", *rssi);

    return OPRT_OK;
}

OPERATE_RET hwl_wf_get_mac(IN CONST WF_IF_E wf, INOUT NW_MAC_S *mac)
{
	// printf("[%s:%d]---------\n",__func__,__LINE__);
	if (NULL == mac)
	{
		// printf("[%s:%d]---------\n",__func__,__LINE__);
		return OPRT_INVALID_PARM;
	}

#ifdef __HuaweiLite__
	// todo
	// Implementing MAC acquisition of liteos system
#else
	FILE *pp = popen("ifconfig " WLAN_DEV, "r");
	if (pp == NULL)
	{
		// printf("[%s:%d]---------\n",__func__,__LINE__);
		return OPRT_COM_ERROR;
	}

	char tmp[256];
	memset(tmp, 0, sizeof(tmp));
	while (fgets(tmp, sizeof(tmp), pp) != NULL)
	{
		char *pMACStart = strstr(tmp, "ether ");
		if (pMACStart != NULL)
		{
			int x1, x2, x3, x4, x5, x6;
			sscanf(pMACStart + strlen("ether "), "%x:%x:%x:%x:%x:%x", &x1, &x2, &x3, &x4, &x5, &x6);
			mac->mac[0] = x1 & 0xFF;
			mac->mac[1] = x2 & 0xFF;
			mac->mac[2] = x3 & 0xFF;
			mac->mac[3] = x4 & 0xFF;
			mac->mac[4] = x5 & 0xFF;
			mac->mac[5] = x6 & 0xFF;

			break;
		}
	}
	pclose(pp);
#endif

	printf("WIFI Get MAC %02X-%02X-%02X-%02X-%02X-%02X\r\n",
	       mac->mac[0], mac->mac[1], mac->mac[2], mac->mac[3], mac->mac[4], mac->mac[5]);

	return OPRT_OK;
}
/***********************************************************
 *  Function: hwl_bnw_get_mac
 *  Desc:     get wired ethernet mac info
 *  Output:   mac: the mac info.
 *  Return:   OPRT_OK: success  Other: fail
 ***********************************************************/
OPERATE_RET hwl_bnw_get_mac(OUT NW_MAC_S *mac)
{
	printf("[%s:%d]---------\n",__func__,__LINE__);
	return OPRT_OK;
}

/***********************************************************
 *  Function: hwl_bnw_set_mac
 *  Desc:     set wired ethernet mac info
 *  Input:    mac: the mac info.
 *  Return:   OPRT_OK: success  Other: fail
 ***********************************************************/
OPERATE_RET hwl_bnw_set_mac(IN CONST NW_MAC_S *mac)
{
	printf("[%s:%d]---------\n",__func__,__LINE__);
	return OPRT_OK;
}