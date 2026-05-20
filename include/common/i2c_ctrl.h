#ifndef _I2C_CTRL_H_
#define _I2C_CTRL_H_
#include <stdbool.h>
#include "fcntl.h"

typedef struct
{
	unsigned char reg;
	unsigned char val;
}I2C_DATA;



/***
** 日期: 2022-05-12 09:05
** 作者: leo.liu
** 函数作用：写入i2c
** 返回参数说明：
***/
bool i2c_write(int i2c_dev, unsigned char dev_addr, I2C_DATA *data, int len);

/***
** 日期: 2022-05-12 09:05
** 作者: leo.liu
** 函数作用：读i2c
** 返回参数说明：
***/
bool i2c_read(int i2c_dev, unsigned char dev_addr, I2C_DATA* data,int len);

#endif