#include "i2c_ctrl.h"
#include "fcntl.h"
#include "linux/i2c-dev.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"

#include <linux/i2c.h>

/***
** 日期: 2022-05-12 09:05
** 作者: leo.liu
** 函数作用：写入i2c
** 返回参数说明：
***/
bool i2c_write(int i2c_dev, unsigned char dev_addr, I2C_DATA *data, int len)
{
    char path[64] = {0};

    sprintf(path, "/dev/i2c-%d", i2c_dev);
    int fd = open(path, O_RDWR);
    if (fd < 0)
    {
        return false;
    }

    if (ioctl(fd, I2C_SLAVE_FORCE, dev_addr) < 0)
    {
        printf("oictl:setslave address failed\n");
        close(fd);
        return false;
    }

    for (int i = 0; i < len; i++)
    {

        unsigned char i2c_data[2];
        i2c_data[0] = data[i].reg;
        i2c_data[1] = data[i].val;
        if (write(fd, (unsigned char *)i2c_data, 2) != 2)
        {
            printf("dev addr:%02x ,reg:%02x write failed \n", i2c_data[0], i2c_data[1]);
            close(fd);
            return false;
        }

    }

    close(fd);
    return true;
}

/***
** 日期: 2022-05-12 09:05
** 作者: leo.liu
** 函数作用：读i2c
** 返回参数说明：
***/
bool i2c_read(int i2c_dev, unsigned char dev_addr, I2C_DATA *data, int len)
{
    char path[64] = {0};
    sprintf(path, "/dev/i2c-%d", i2c_dev);
    int fd = open(path, O_RDWR);
    if (fd < 0)
    {
        printf("open %s failed \n", path);
        return false;
    }

    for (int i = 0; i < len; i++)
    {
        if (ioctl(fd, I2C_SLAVE_FORCE, dev_addr) < 0)
        {
            printf("oictl:setslave address failed\n");
            close(fd);
            return false;
        }

        if (write(fd, &data[i].reg, 1) <= 0)
        {
            printf("dev addr:%02x ,reg:%02x write failed \n", dev_addr, data[i].reg);
            close(fd);
            return false;
        }

        if (read(fd, &data[i].val, 1) <= 0)
        {
            printf("dev addr:%02x ,reg:%02x read failed \n", dev_addr, data[i].reg);
            close(fd);
            return false;
        }
    }
    close(fd);
    return true;
}
