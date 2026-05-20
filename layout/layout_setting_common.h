#ifndef _LAYOUT_SETTING_H_
#define _LAYOUT_SETTING_H_
#include "layout_define.h"
/***
** 日期: 2022-04-26 17:09
** 作者: leo.liu
** 返回参数说明：
** 函数作用：创建设置页面的返回按钮
***/
bool setting_cancel_btn_create(void (*obj_click_up_callback)(lv_obj_t *));

/***
** 日期: 2022-04-28 10:40
** 作者: leo.liu
** 函数作用：创建setting的顶部文本
** 返回参数说明：
***/
bool setting_head_label_create(const char *string);

/***
** 日期: 2022-04-28 10:55
** 作者: leo.liu
** 函数作用：设置侧边按钮创建
** 返回参数说明:select_id:设置当前页面：
***/
bool setting_main_btn_create(int page_id);
bool setting_volume_btn_create(int page_id);
/***
** 日期: 2022-04-28 11:59
** 作者: leo.liu
** 函数作用：创建右边按钮基础函数
** 返回参数说明：
***/
lv_obj_t *setting_right_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, const char *sub_string, obj_click_data *click_data, unsigned int sub_obj_id);
/***
** 日期: 2022-04-29 15:56
** 作者: leo.liu
** 函数作用：消息框的背景创建
** 返回参数说明：
***/
lv_obj_t *setting_msgdialog_msg_bg_create(int id);

lv_obj_t *setting_mecurity_msgdialog_msg_bg_create(int id);
/***
**   日期:2022-07-04 17:00:03
**   作者: leo.liu
**   函数作用：销毁消息框
**   参数说明:
***/
bool setting_msgdialog_msg_bg_delete(int id);
/***
** 日期: 2022-05-04 09:10
** 作者: leo.liu
** 函数作用：消息框的确认按钮创建
** 返回参数说明：
***/
void setting_msgdialog_msg_confirm_and_cancel_btn_create(lv_obj_t *parent, void (*cancel_func)(lv_obj_t *obj), void (*confirm_func)(lv_obj_t *obj));
/***
** 日期: 2022-05-04 09:10
** 作者: leo.liu
** 函数作用：消息框的确认按钮创建
** 返回参数说明：
***/
void setting_msgdialog_msg_confirm_btn_create(lv_obj_t *parent, void (*confirm_func)(lv_obj_t *obj));
void setting_security_msgdialog_msg_confirm_btn_create(lv_obj_t *parent, void (*confirm_func)(lv_obj_t *obj));

/***
 ** 日期: 2022-04-28 15:32
 ** 作者: leo.liu
 ** 函数作用：创建按键矩阵
 ** 返回参数说明：
 ***/
lv_obj_t *setting_msgdialog_btnmatrix_create(lv_obj_t *prarent, int obj_id, const char *string_map[]);

/***
** 日期: 2022-05-04 09:18
** 作者: leo.liu
** 函数作用：消息框的消息内容显示控件创建
** 返回参数说明：
***/
void setting_msgdialog_msg_create(lv_obj_t *parent, const char *msg_string);
void setting_msgdialog_video_msg_create(lv_obj_t *parent, const char *msg_string);
/***
** 日期: 2022-04-28 11:59
** 作者: leo.liu
** 函数作用：创建设置图标
** 返回参数说明：focus:-1:不需要图标，0：不选中（图标），1：选中（需要图标）
***/
lv_obj_t *setting_sub_btn_base_create(lv_obj_t *parent, int x, int y, int w, int h, const char *main_string, obj_click_data *click_data, bool focus, int type);

/***
** 日期: 2022-05-06 10:22
** 作者: leo.liu
** 函数作用：滚动基础函数
** 返回参数说明：
***/
lv_obj_t *setting_time_rooler_base(lv_obj_t *parent, int x, int y, int w, int h, int min, int max, int cur, int obj_id);
lv_obj_t *setting_time_rooler_big_base(lv_obj_t *parent, int x, int y, int w, int h, int min, int max, int cur, int obj_id);
/***
** 日期: 2022-05-07 13:52
** 作者: leo.liu
** 函数作用：数字密码输入键盘
** 返回参数说明：
***/
lv_obj_t *setting_passowrd_num_keyboard_create(lv_obj_t *parent, int x, int y, int w, int h, obj_click_data *click_data);

/***
** 日期: 2022-05-07 16:58
** 作者: leo.liu
** 函数作用：设置密码输入框
** 返回参数说明
** 3x4个控件
** LABEL ：显示文本
** OBJ ：显示圆点
** OBJ :显示下划线
** 要获取控件:可以通过矩阵id获取，如第一个lable:0x11
***/
bool setting_password_input_label_create(lv_obj_t *parent, int x, int y, int row);

/***
 ** 日期: 2022-05-09 10:26
 ** 作者: leo.liu
 ** 函数作用：输入编辑的数值
 ** 返回参数说明：
 ***/
bool setting_password_input_string(lv_obj_t *parent, const char *string, int max_edit);

/***
** 日期: 2022-05-09 10:57
** 作者: leo.liu
** 函数作用：删除一个字符
** 返回参数说明：
***/
bool setting_password_del_string(lv_obj_t *parent, int max_edit);

/***
** 日期: 2022-05-09 11:22
** 作者: leo.liu
** 函数作用：获取输入字符串
** 返回参数说明：
***/
bool setting_password_get_string(lv_obj_t *parent, char *buffer);

/***
** 日期: 2022-05-09 11:27
** 作者: leo.liu
** 函数作用：获取当前输入的索引
** 返回参数说明：
***/
int setting_password_edit_index_get(lv_obj_t *parent);

/***
** 日期: 2022-05-09 11:31
** 作者: leo.liu
** 函数作用：复位输入框
** 返回参数说明：
***/
bool setting_password_input_reset(lv_obj_t *parent, int edit_max);
/***
** 日期: 2022-05-09 11:31
** 作者: fei.liu
** 函数作用：刷新图像进度条
** 返回参数说明：
***/
void adj_value_refresh_display(void);
#endif