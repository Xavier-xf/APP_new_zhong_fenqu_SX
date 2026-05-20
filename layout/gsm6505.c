#include "gsm6502.h"
#include "i2c_ctrl.h"
#include "fcntl.h"
#include "stdio.h"
#include "user_intercom.h"
#define GSM6502_I2C_PATH 0
#define GSM6502_CLAMP 0X03
#define GSM6502_GAIN 0X04
#define GSM6502_ADDR (0X06 >> 1)

/***
** 日期: 2022-05-12 09:27
** 作者: leo.liu
** 函数作用：依据数组获取进的寄存器和输出的寄存器
** 返回参数说明：
***/
static void gsm6502_addinfo_get(int in, unsigned char in_addr[6][8], unsigned char *out_addr, unsigned char *clamp, unsigned char *agin)
{
    for (int j = in; j < in + 2; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (in_addr[j][i] == 1)
            {
                if (j == in)
                {

                    *out_addr = i + 1;
                }
                else
                {
                    *out_addr |= ((i + 1) << 4);
                }
                *clamp |= (0x01 << i);

                *agin = 0x00;
                
                // if (OUT_BT601 == j)
                // {
                //     *agin &= ((~(0x01 << j)) & 0xff);
                // }
                // if(OwnID != 1)
                // {
                //     *agin &= ((~(0x01 << j)) & 0xff);
                // }
                // else
                // {
                //     if (OUT_BT601 != j)
                //     {
                //         *agin &= ((~(0x01 << j)) & 0xff);
                //     }
                //     else
                //     {
                //         *agin = 0x00;
                //     }
                // }
                break;
            }
        }
    }
}

/***
** 日期: 2022-05-12 09:25
** 作者: leo.liu
** 函数作用：打开6502通道
** 返回参数说明：数组参数为打开通道8进6出
***/
void gsm6502_open(unsigned char data[6][8])
{
    unsigned char clamp = 0x00;
    unsigned char agin = 0xFF;

    unsigned char out12_addr = 0x00;
    gsm6502_addinfo_get(0, data, &out12_addr, &clamp, &agin);

    unsigned char out34_addr = 0x00;
    gsm6502_addinfo_get(2, data, &out34_addr, &clamp, &agin);

    unsigned char out56_addr = 0x00;
    gsm6502_addinfo_get(4, data, &out56_addr, &clamp, &agin);

    I2C_DATA reg;
    reg.reg = 0x00;
    reg.val = out12_addr;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);

    reg.reg = 0x01;
    reg.val = out34_addr;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);

    reg.reg = 0x02;
    reg.val = out56_addr;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);

    reg.reg = GSM6502_CLAMP;
    reg.val = clamp;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);

    reg.reg = GSM6502_GAIN;
    if(OwnID == 2)
    {
        reg.val = 0xde;//agin;
    }
    else
    {
        reg.val = 0xde;//agin;
    }
    // reg.val = agin;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);
}

/***
** 日期: 2022-05-12 09:34
** 作者: leo.liu
** 函数作用：关闭gsm6502所有通道
** 返回参数说明：
***/
void gsm6502_close(void)
{
    I2C_DATA reg;
    reg.reg = 0x00;
    reg.val = 0x00;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);
    reg.reg = 0x01;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);
    reg.reg = 0x02;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);
    reg.reg = 0x03;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);
    reg.reg = 0x04;
    i2c_write(GSM6502_I2C_PATH, GSM6502_ADDR, &reg, 1);
}