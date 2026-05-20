#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "gpio_ctrl.h"

/***
** 日期: 2022-04-28 10:01 
** 作者: leo.liu
** 函数作用：检测gpio上是否已经导出
** 返回参数说明：
***/
static bool is_gpio_export(const int pin)
{
	char gpio_path[128] =
	{
		0
	};
	sprintf(gpio_path, "/sys/class/gpio/gpio%d", pin);

	if (access(gpio_path, F_OK) == 0)
	{
		return true;
	}

	return false;
}



/***
** 日期: 2022-04-28 10:01 
** 作者: leo.liu
** 函数作用：导出gpio口
** 返回参数说明：
***/
static bool gpio_export(const int pin)
{
	/***** 1.检测是否已经配置GPIO *****/
	if (is_gpio_export(pin) == true)
	{
		return true;
	}

	/***** 2.打开gpio export句柄 *****/
	int export_fd = open("/sys/class/gpio/export", O_WRONLY);

	if (export_fd < 0)
	{
		printf("export gpio%d failed \n", pin);
		return false;
	}

	/***** 3.写入pin到export句柄 *****/
	char export_buf[32] =
	{
		0
	};
	int len = sprintf(export_buf, "%d", pin);




	if (write(export_fd, export_buf, len) < 0)
	{
		close(export_fd);
		printf("write export gpio%d failed \n", pin);
		return false;
	}

	close(export_fd);
	return true;
}


/***
** 日期: 2022-04-28 10:02 
** 作者: leo.liu
** 函数作用：设置gpio输入/输出
** 返回参数说明：
***/
bool gpio_direction_set(const int pin, GPIO_DIR dir)
{
	if (gpio_export(pin) == false)
	{
		printf("export gpio%d faild \n", pin);
		return false;
	}

	char direction_buf[128] =
	{
		0
	};
	sprintf(direction_buf, "/sys/class/gpio/gpio%d/direction", pin);
	int direction_fd = open(direction_buf, O_WRONLY);

	if (direction_fd < 0)
	{
		printf("gpio%d direction setting failed \n", pin);
		return false;
	}

	memset(direction_buf, 0, sizeof(direction_buf));
	int len = sprintf(direction_buf, "%s", dir == GPIO_DIR_IN ? "in": "out");

	if (write(direction_fd, direction_buf, len) < 0)
	{
		printf("diection write gpio%d %s faild \n", pin, direction_buf);
		close(direction_fd);
		return false;
	}

	close(direction_fd);
	return true;
}




/***
** 日期: 2022-04-28 10:02 
** 作者: leo.liu
** 函数作用：使能/失能gpio内部上拉
** 返回参数说明：
***/
bool gpio_pull_enable(const int pin, bool enable)
{
	if (gpio_export(pin) == false)
	{
		printf("export gpio%d faild \n", pin);
		return false;
	}


	char gpio_pull_buf[128] =
	{
		0
	};
	sprintf(gpio_pull_buf, "/sys/class/gpio/gpio%d/pull_enable", pin);
	int gpio_pull_fd = open(gpio_pull_buf, O_WRONLY);

	if (gpio_pull_fd < 0)
	{
		printf("gpio%d pull_enable setting failed \n", pin);
		return false;
	}

	memset(gpio_pull_buf, 0, sizeof(gpio_pull_buf));
	int len = sprintf(gpio_pull_buf, "%s", enable? "1": "0");

	if (write(gpio_pull_fd, gpio_pull_buf, len) < 0)
	{
		printf("gpio pull write gpio%d %s faild \n", pin, gpio_pull_buf);
		close(gpio_pull_fd);
		return false;
	}

	close(gpio_pull_fd);
	return true;
}



/***
** 日期: 2022-04-28 10:02 
** 作者: leo.liu
** 函数作用：设置IO口电平
** 返回参数说明：在设置IO电平之前,需要调用 gpio_direction_set 
***/
bool gpio_level_set(const int pin, GPIO_LEVEL level)
{
	char gpio_buf[64] =
	{
		0
	};
	sprintf(gpio_buf, "/sys/class/gpio/gpio%d/value", pin);
	int gpio_fd = open(gpio_buf, O_WRONLY);

	if (gpio_fd < 0)
	{
		printf("gpio%d set value open failed \n", pin);
		return false;
	}

	if (write(gpio_fd, (level == GPIO_LEVEL_LOW) ? "0": "1", 1) < 0)
	{
		printf("gpio%d write %d failed \n", pin,level == GPIO_LEVEL_LOW?0:1);
		close(gpio_fd);
		return false;
	}

	close(gpio_fd);
	return true;
}


/***
** 日期: 2022-04-28 10:03 
** 作者: leo.liu
** 函数作用：读取IO电平
** 返回参数说明：在读取IO电平之前,需要调用 gpio_direction_set 
***/
bool gpio_level_read(const int pin, GPIO_LEVEL * level)
{
	char gpio_buf[64] =
	{
		0
	};
	sprintf(gpio_buf, "/sys/class/gpio/gpio%d/value", pin);
	int gpio_fd = open(gpio_buf, O_RDONLY);

	if (gpio_fd < 0)
	{
		printf("gpio%d  open failed \n", pin);
		return false;
	}

	memset(gpio_buf,0,sizeof(gpio_buf));
	if (read(gpio_fd, gpio_buf,1) < 0)
	{
		printf("gpio%d read failed \n", pin);
		close(gpio_fd);
		return false;
	}

	close(gpio_fd);


	*level = (gpio_buf[0] =='0')?GPIO_LEVEL_LOW:GPIO_LEVEL_HIGH;
	return true;
}

static	GPIO_LEVEL per_level = 0;
static	GPIO_LEVEL level = 0;
/***
** 日期: 2022-04-28 10:03 
** 作者: liu
** 函数作用：读取IO电平进行alarm的判定
** 返回参数说明：在读取IO电平之前,需要调用 gpio_direction_set 
***/
bool alarm_1_gpio_level_read(const int pin)
{
	char gpio_buf[64] =
	{
		0
	};
	sprintf(gpio_buf, "/sys/class/gpio/gpio%d/value", pin);
	int gpio_fd = open(gpio_buf, O_RDONLY);

	if (gpio_fd < 0)
	{
		printf("gpio%d  open failed \n", pin);
		return false;
	}

	memset(gpio_buf,0,sizeof(gpio_buf));
	if (read(gpio_fd, gpio_buf,1) < 0)
	{
		printf("gpio%d read failed \n", pin);
		close(gpio_fd);
		return false;
	}

	close(gpio_fd);
	

	level = (gpio_buf[0] =='0')?GPIO_LEVEL_LOW:GPIO_LEVEL_HIGH;
	if((level != per_level) /* && (level == GPIO_LEVEL_HIGH) */)
	{
		per_level = level;
		if(level == GPIO_LEVEL_HIGH)
		{
			return true;
		}
		
	}
	return false;
}

static	GPIO_LEVEL per_level_2 = 0;
static	GPIO_LEVEL level_2 = 0;

bool alarm_2_gpio_level_read(const int pin)
{
	char gpio_buf[64] =
	{
		0
	};
	sprintf(gpio_buf, "/sys/class/gpio/gpio%d/value", pin);
	int gpio_fd = open(gpio_buf, O_RDONLY);

	if (gpio_fd < 0)
	{
		printf("gpio%d  open failed \n", pin);
		return false;
	}

	memset(gpio_buf,0,sizeof(gpio_buf));
	if (read(gpio_fd, gpio_buf,1) < 0)
	{
		printf("gpio%d read failed \n", pin);
		close(gpio_fd);
		return false;
	}

	close(gpio_fd);
	

	level_2 = (gpio_buf[0] =='0')?GPIO_LEVEL_LOW:GPIO_LEVEL_HIGH;
	if((level_2 != per_level_2) /* && (level == GPIO_LEVEL_HIGH) */)
	{
		per_level_2 = level_2;
		if(level_2 == GPIO_LEVEL_HIGH)
		{
			return true;
		}
		
	}
	return false;
}


static int adc_value = 0;
int adc_value_get(void)
{
	return adc_value;
}


#define FILE_PATH_MAX 128
#define AK_SAR_ADC_PATCH "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

bool adc_level_read(GPIO_LEVEL * level)
{
	int fd = open(AK_SAR_ADC_PATCH,O_RDONLY);
	if(fd < 0)
	{
		printf("open ad ch failed\n");
	}
	char value[FILE_PATH_MAX] = {0};
	if (read(fd, value, FILE_PATH_MAX) < 0)    //读取为字符型
	{
		printf("adc read failed \n");
		close(fd);
		return false;
	}
	int ad_value = atoi(value); //把字符串转化成整形数
	adc_value = ad_value;
	// printf("===========>>>>>>>><<<<<<<<<<[%d]=======[%d]\n",adc_value,ad_value);
	close(fd);
	*level = (ad_value >= 2000)?GPIO_LEVEL_HIGH:GPIO_LEVEL_LOW;
	return true;
} 
