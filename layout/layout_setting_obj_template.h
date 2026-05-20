#ifndef __UI_SETTING_OBJ_TEMPLATE_H_
#define __UI_SETTING_OBJ_TEMPLATE_H_

#include "layout_define.h"

#define THEAME_FONT_SIZE        FONT_SIZE(28)	    // 标题字体大小

#define THEAME_SUB_FONT_COLOR   (0x404040)		    // 右边子控件 文字的颜色
#define THEAME_COLOR_DEFAULT    (0x3AC392)			// default 颜色 (绿)
#define THEAME_COLOR_PRESS      (0x4ED7A6)			// press   颜色 (绿)
#define THEAME_COLOR_DISABLE    (0x4ED7A6)			// DISABLE 颜色 (绿)

#define MONITOR_COLOR_GREEN     (0x0DA56E) // 绿色
#define MONITOR_COLOR_GREY      (0x323232) // 灰色
#define MONITOR_COLOR_RED       (0xD43A3A) // 红色




/*************************************************************************
 * @brief  设置子对象样式
 * @date   2022-11-01 13:36
 * @author xiaoele
 **************************************************************************/
typedef enum 
{
    SETTING_SUB_NONE = 0x01,    // 不显示
    SETTING_SUB_LABEL,          // 字符串
    SETTING_SUB_SWITCH,         // 开关, 点击仅切换
    SETTING_SUB_BUTTON,         // 按钮, 点击会跳转界面, 有箭头 
    SETTING_SUB_TEXTAREA,


    SETTING_SUB_TYPE_TOTAL
}SETTING_SUB_TYPE;

lv_obj_t *layout_back_btn_create(void (*obj_click_up)(lv_obj_t *));
void setting_main_option_create(void);
lv_obj_t *setting_main_option_label_create(int y, bool select, const char *string, obj_click_data *click_cb);

lv_obj_t *layout_title_label_create(const char *string);
lv_obj_t *setting_sub_obj_template(lv_obj_t *parent, int x, int num, const char *main_string, int main_id, const char *sub_string, int sub_id, bool show_arr, SETTING_SUB_TYPE type, obj_click_data * click_cb);
lv_obj_t *setting_long_obj_template(lv_obj_t *parent, int num, const char *main_string, int main_id, const char *sub_string, int sub_id, bool show_arr, int type, obj_click_data * click_cb);

lv_obj_t *msg_window_with_button(int w,int h ,void (*bg_func)(lv_obj_t *), void (*cancel_func)(lv_obj_t *), void (*confirm_func)(lv_obj_t *));
lv_obj_t *msg_window_with_button_one(int w, int h, void (*bg_func)(lv_obj_t *), void (*confirm_func)(lv_obj_t *));
lv_obj_t *msg_window_btnmatrix_create(lv_obj_t *parent, const char *map[], int id, bool single);

lv_obj_t *msg_window_create(char *str, bool is_loading, int width, int height, void (*cancel_func)(lv_obj_t *));
lv_obj_t *setting_left_right_arrow_create(lv_obj_t *parent, int num, bool arrow, obj_click_data *click_left, obj_click_data *click_right);
lv_obj_t *setting_time_roller_template(lv_obj_t *parent, lv_obj_t *icon, int y, int width, int height, int min, int max, int cur, int id, obj_click_data *click_data);
lv_obj_t *setting_option_create_by_id_num_template(lv_obj_t *parent, int id, const char *string, obj_click_data *click_cb, bool single);
lv_obj_t *layout_kbd_all_create(void);
lv_obj_t *layout_kbd_number_create(lv_obj_t *parent, obj_click_data *click_data, int id);
lv_obj_t *layout_kbd_number_textarea_create(lv_obj_t *obj, int y, int id, const char *str, bool edit);
lv_obj_t* layout_page_create(int objid);


#endif /* __UI_SETTING_OBJ_TEMPLATE_H_ */