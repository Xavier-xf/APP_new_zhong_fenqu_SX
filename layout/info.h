#ifndef _CUSTOM_FUNCTIONS_H
#define _CUSTOM_FUNCTIONS_H
/*************************************************************************
* 文件 : layout/info.h
* 功能 : 自定义函数
 **************************************************************************/

/********************************** <START> ***************************************
 * @brief  自定义打印输出
 * @date   2022-10-07 14:19
 * @author xiaoele
 **************************************************************************/
#define COLOR_BLUE_LOG      "\033[0;1;34m"
#define COLOR_YELLOW_LOG    "\033[0;1;33m"
#define COLOR_GREEN_LOG     "\033[0;1;32m"
#define COLOR_RED_LOG       "\033[0;1;31m"
#define COLOR_WHITE_LOG     "\033[0;1m"
#define COLOR_NONE_LOG      "\033[0m"

#define LOG_BLUE(format,...)   printf(COLOR_BLUE_LOG   "[%s:%04u]  " format COLOR_NONE_LOG, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define printf(format,...)  printf(COLOR_YELLOW_LOG "[%s:%04u]  " format COLOR_NONE_LOG, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_GREEN(format,...)   printf(COLOR_GREEN_LOG  "[%s:%04u]  " format COLOR_NONE_LOG, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_RED(format,...)      printf(COLOR_RED_LOG    "[%s:%04u]  " format COLOR_NONE_LOG, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WHITE(format,...)    printf(COLOR_WHITE_LOG  "[%s:%04u]  " format COLOR_NONE_LOG, __FUNCTION__, __LINE__, ##__VA_ARGS__)
/********************************** <END> ***************************************/




#endif /* _CUSTOM_FUNCTIONS_H */