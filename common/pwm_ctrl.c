#include "pwm_ctrl.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>




/***
** 日期: 2022-05-10 09:15 
** 作者: leo.liu
** 函数作用：初始化pwm设备
** 返回参数说明：pwmx:pwm号(比如pwmx = 2 ->pwm2)
**              CH:pwm 通道：一般默认0
***/
bool pwm_init(int pwmx,int ch)
{
    // system("insmod /usr/modules/ak_pwm_char.ko");

    /***** 探测pwm使能状态 *****/
    char path[128] = {0};
    sprintf(path,"/sys/class/pwm/pwmchip%d",pwmx);
    if(access(path,F_OK) != 0)
    {
        printf("No device was found \n");
        return false;
    }
    memset(path,0,sizeof(path));
    /***** 探测pwm下的通道是否导出 *****/

    sprintf(path,"/sys/class/pwm/pwmchip%d/pwm%d",pwmx,ch);
    if(access(path,F_OK) != 0)
    {
        /***** 导出此设备卡 *****/
        memset(path,0,sizeof(path));
        sprintf(path,"/sys/class/pwm/pwmchip%d/export",pwmx);
        int fd = open(path,O_WRONLY);
        if(fd < 0)
        {
            printf("open %s failed \n",path);
            return false;
        }
        memset(path,0,sizeof(path));
        int len = sprintf(path,"%d",ch);
        if(write(fd,path,len) < 0)
        {
            printf("write %s failed \n",path);
            close(fd);
            return false;
        }
        close(fd);
    }
    return true;
}

/***
** 日期: 2022-05-10 09:27 
** 作者: leo.liu
** 函数作用：使能/失能pwmd
** 返回参数说明：
***/
bool pwm_enable(int pwmx,int ch,bool en)
{
    char path[128] = {0};
    sprintf(path,"/sys/class/pwm/pwmchip%d/pwm%d/enable",pwmx,ch);
    int fd = open(path,O_WRONLY);
    if(fd < 0)
    {
        printf("open %s failed \n",path);
        return false;
    }

    memset(path,0,sizeof(path));
    int len = sprintf(path,"%d",en);
    if(write(fd,path,len) < 0)
    {
        printf("write %s  failed \n",path);
        close(fd);
        return false;
    }
    close(fd);
    return true;
}


/***
** 日期: 2022-05-10 09:30 
** 作者: leo.liu
** 函数作用：pwm 频率设置
** 返回参数说明：
***/
bool pwm_period_set(int pwmx,int ch,int period)
{
    char path[128] = {0};
    sprintf(path,"/sys/class/pwm/pwmchip%d/pwm%d/period",pwmx,ch);
    int fd = open(path,O_WRONLY);
    if(fd < 0)
    {
        printf("open %s failed \n",path);
        return false;
    }

    memset(path,0,sizeof(path));
    int len = sprintf(path,"%d",period);
    if(write(fd,path,len) < 0)
    {
        printf("write %s failed\n",path);
        close(fd);
        return false;
    }
    close(fd);
    return true;
}



/***
** 日期: 2022-05-10 09:30 
** 作者: leo.liu
** 函数作用：pwm 频率占空比设置
** 返回参数说明：
***/
bool pwm_duty_cycle_set(int pwmx,int ch,int duty_cycle)
{
    char path[128] = {0};
    sprintf(path,"/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle",pwmx,ch);
    int fd = open(path,O_WRONLY);
    if(fd < 0)
    {
        printf("open %s failed \n",path);
        return false;
    }

    memset(path,0,sizeof(path));
    int len = sprintf(path,"%d",duty_cycle);
    if(write(fd,path,len) < 0)
    {
        printf("write %s failed\n",path);
        close(fd);
        return false;
    }
    close(fd);
    return true;
}

