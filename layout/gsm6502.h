#ifndef _GSM_6502_H_
#define _GSM_6502_H_


#define IN_DOOR1        0x04
#define IN_DOOR2        0X03
#define IN_CCTV1        0X02
#define IN_CCTV2        0X01
#define IN_IN1_DOOR     0X00

#define OUT_BT601       0X05
#define OUT_INT_BT601   0X00


/***
** 日期: 2022-05-12 09:25
** 作者: leo.liu
** 函数作用：打开6502通道
** 返回参数说明：数组参数为打开通道8进6出
***/
void gsm6502_open(unsigned char data[6][8]);

/***
** 日期: 2022-05-12 09:34
** 作者: leo.liu
** 函数作用：关闭gsm6502所有通道
** 返回参数说明：
***/
void gsm6502_close(void);
#endif
