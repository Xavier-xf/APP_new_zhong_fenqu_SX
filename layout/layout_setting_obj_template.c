// /*******************************************************************
//  * @Descripttion   : 
//  * @version        : 1.0.0
//  * @Author         : wxj
//  * @Date           : 2023-11-17 08:34
//  * @LastEditTime   : 2023-11-17 08:35
// *******************************************************************/
#include "layout_setting_obj_template.h"






// /*************************************************************************
//  * @brief  设置界面创建标题
//  * @date   2022-11-01 10:10
//  * @author xiaoele
//  **************************************************************************/
// lv_obj_t * layout_title_label_create(const char *string)
// {
// 	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
// 	lv_obj_set_size(cont, LV_HOR_RES_MAX, 72);
// 	lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
// 	lv_obj_set_auto_realign(cont, true);
// 	lv_obj_move_foreground(cont);						// 移动到前
	
// 	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
// 	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));
	

// 	lv_obj_t *sub_obj = lv_label_create(cont, NULL);
// 	lv_label_set_text(sub_obj, string);
// 	lv_label_set_long_mode(sub_obj, LV_LABEL_LONG_EXPAND);
// 	lv_label_set_align(sub_obj, LV_LABEL_ALIGN_CENTER);
// 	lv_obj_align(sub_obj, cont, LV_ALIGN_CENTER, 0, 0);
// 	lv_obj_set_auto_realign(sub_obj, true);

//     lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// 	lv_obj_set_click(cont, false);

// 	// lv_obj_set_style_local_bg_opa(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
// 	// lv_obj_set_style_local_bg_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));


// 	return cont;
// }


// /*************************************************************************
//  * @brief  设置界面的返回图标
//  * @date   2022-11-01 10:34
//  * @author xiaoele
//  **************************************************************************/
// // lv_obj_t *layout_back_btn_create(void (*obj_click_up)(lv_obj_t *))
// // {
// // 	static rom_bin_info img = rom_bin_info_get(ROM_UI_SETTING_BACK_PNG);
// // 	lv_obj_t *obj = lv_cont_create(lv_scr_act(), NULL);
// // 	lv_obj_set_size(obj, 60, 60);
// // 	lv_obj_align(obj, NULL, LV_ALIGN_IN_TOP_LEFT, 28, 14);
// // 	lv_obj_set_ext_click_area(obj, 40, 20, 40, 20);

// // 	lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);

// // 	static obj_click_data click_data;
// // 	click_data.down = NULL;
// // 	click_data.anything_func = NULL;
// // 	click_data.up = obj_click_up;
// // 	obj_click_event_listen(obj, &click_data);


// // 	return obj;
// // }


// lv_obj_t *setting_main_option_label_create(int y, bool select, const char *string, obj_click_data *click_cb)
// {
// 	lv_obj_t * obj = lv_obj_create(lv_scr_act(), NULL);
// 	lv_obj_set_pos(obj, 0, y);
// 	lv_obj_set_size(obj, 280, 75);
// 	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_COLOR_DEFAULT));

// 	lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// 	lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
// 	lv_obj_set_style_local_value_ofs_x(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 28);

// 	/* 是否选中 */
// 	if(select)
// 	{
// 		lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// 	}
// 	else
// 	{
// 		lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
// 	}

// 	/* 文字 */
// 	if(string != NULL)
// 	{
// 		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);
// 	}

// 	/* 点击回调 */
// 	if(click_cb != NULL)
// 	{
// 		obj_click_event_listen(obj, click_cb);
// 	}

// 	return obj;
// }

// /*************************************************************************
//  * @brief  按钮弹起
//  * @date   2022-11-01 11:23
//  * @author xiaoele
//  **************************************************************************/
// static void setting_rec_btn_click_fun(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	goto_layout(pLAYOUT(setting_record));

// }

// /*************************************************************************
//  * @brief  按钮弹起
//  * @date   2022-11-01 11:23
//  * @author xiaoele
//  **************************************************************************/
// static void setting_wif_btn_click_fun(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	LOG_WHITE("goto setting wifi\n");
// 	goto_layout(pLAYOUT(setting_wifi));

// }

// /*************************************************************************
//  * @brief  按钮弹起
//  * @date   2022-11-01 11:23
//  * @author xiaoele
//  **************************************************************************/
// static void setting_vol_btn_click_fun(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	goto_layout(pLAYOUT(setting_volume));

// }

// /*************************************************************************
//  * @brief  按钮弹起
//  * @date   2022-11-01 11:23
//  * @author xiaoele
//  **************************************************************************/
// static void setting_sto_btn_click_fun(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	goto_layout(pLAYOUT(setting_storage));

// }

// /*************************************************************************
//  * @brief  按钮弹起
//  * @date   2022-11-01 11:23
//  * @author xiaoele
//  **************************************************************************/
// static void setting_dat_btn_click_fun(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	goto_layout(pLAYOUT(setting_date));

// }

// /*************************************************************************
//  * @brief  按钮弹起
//  * @date   2022-11-01 11:23
//  * @author xiaoele
//  **************************************************************************/
// static void setting_dis_btn_click_fun(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	goto_layout(pLAYOUT(setting_display));

// }

// /*************************************************************************
//  * @brief  按钮弹起
//  * @date   2022-11-01 11:23
//  * @author xiaoele
//  **************************************************************************/
// static void setting_etc_btn_click_fun(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	goto_layout(pLAYOUT(setting_etc));

// }

// /*************************************************************************
//  * @brief  创建设置界面左侧的选项按钮
//  * @date   2022-11-01 10:59
//  * @author xiaoele
//  **************************************************************************/
// void setting_main_option_create(void)
// {

// 	static obj_click_data rec_click = obj_click_data_create(NULL, setting_rec_btn_click_fun);
// 	static obj_click_data wif_click = obj_click_data_create(NULL, setting_wif_btn_click_fun);
// 	static obj_click_data vol_click = obj_click_data_create(NULL, setting_vol_btn_click_fun);
// 	static obj_click_data sto_click = obj_click_data_create(NULL, setting_sto_btn_click_fun);
// 	// static obj_click_data dat_click = obj_click_data_create(NULL, setting_dat_btn_click_fun);
// 	static obj_click_data dis_click = obj_click_data_create(NULL, setting_dis_btn_click_fun);
// 	static obj_click_data etc_click = obj_click_data_create(NULL, setting_etc_btn_click_fun);

// 	const layout *page = cur_layout_get();

// 	int order = 1;
// 	setting_main_option_label_create(75*(order++), page == pLAYOUT(setting_record) 	? true : false, lan_str_get(LAN_SETTING_RECORD),    &rec_click);
// 	setting_main_option_label_create(75*(order++), page == pLAYOUT(setting_wifi) 	? true : false, lan_str_get(LAN_SETTING_WIFI),      &wif_click);
// 	setting_main_option_label_create(75*(order++), page == pLAYOUT(setting_volume) 	? true : false, lan_str_get(LAN_SETTING_VOLUME),    &vol_click);
// 	setting_main_option_label_create(75*(order++), page == pLAYOUT(setting_storage)	? true : false, lan_str_get(LAN_SETTING_STORAGE),   &sto_click);
// 	// setting_main_option_label_create(75*(order++), page == pLAYOUT(setting_date) 	? true : false, lan_str_get(LAN_SETTING_DATE_TIME), &dat_click);
// 	setting_main_option_label_create(75*(order++), page == pLAYOUT(setting_display)	? true : false, lan_str_get(LAN_SETTING_DISPLAY),   &dis_click);
// 	setting_main_option_label_create(75*(order++), page == pLAYOUT(setting_etc)		? true : false, lan_str_get(LAN_SETTING_ETC),       &etc_click);




// 	// lv_obj_set_click(obj_wifi, false);
// 	// lv_obj_set_style_local_value_color(obj_wifi, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));

// }





// /*************************************************************************
//  * @brief  设置界面,右边的控件
//  * @date   2022-11-01 14:00
//  * @author xiaoele
//  **************************************************************************/
// lv_obj_t * setting_sub_obj_template(lv_obj_t *parent, int x, int num, const char *main_string, int main_id, const char *sub_string, int sub_id, bool show_arr, SETTING_SUB_TYPE type, obj_click_data * click_cb)
// {	
// 	lv_obj_t *obj = lv_obj_create(parent, NULL);
// 	lv_obj_set_size(obj, 612, 70);
// 	lv_obj_set_pos(obj, x, 75*num);
// 	lv_page_glue_obj(obj, true);
// 	lv_obj_set_ext_click_area(obj, 16, 16, 0, 0);

// 	/* 控件ID */
// 	if(main_id != 0)
// 	{
// 		lv_obj_set_id(obj, main_id);
// 	}

// 	/* 点击回调 */
// 	if(click_cb != NULL)
// 	{
// 		obj_click_event_listen(obj, click_cb);
// 	}

// 	/* 右边的灰色箭头 */
// 	if(show_arr)
// 	{
// 		lv_obj_t *icon = lv_obj_create(obj, NULL);
// 		lv_obj_set_click(icon, false);
// 		lv_obj_set_size(icon, 60, 60);
// 		lv_obj_align(icon, icon->parent, LV_ALIGN_IN_RIGHT_MID, 0, 0);
// 		static rom_bin_info img = rom_bin_info_get(ROM_UI_SETTING_SUB_ARROW_RIGHT_DEF_PNG);
// 		lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
// 	}

// 	/* 底部边框 */
// 	lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
// 	lv_obj_set_style_local_border_side(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
// 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
// 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x404040));
// 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));

// 	/* 按钮的文字 */
// 	if(main_string != NULL)
// 	{
// 		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, main_string);
// 		lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
// 		lv_obj_set_style_local_value_ofs_x(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 4);
// 		lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// 	}

	
// 	if(type == SETTING_SUB_NONE)
// 	{
// 		return obj;
// 	}

// 	/* 以下控件只能选其一 */
// 	if((sub_string != NULL) && type == SETTING_SUB_SWITCH)
// 	{
// 		LOG_B_RED("SW控件不能显示附加文本\n");
// 	}
// 	lv_obj_t *sub_obj = NULL;

// 	/* 子控件 文本  */
// 	if(sub_string != NULL)
// 	{
// 		sub_obj = lv_label_create(obj, NULL);
// 		lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -55, 0);
// 		lv_obj_set_auto_realign(sub_obj, true);
// 		lv_label_set_align(sub_obj, LV_LABEL_ALIGN_RIGHT);
// 		lv_label_set_long_mode(sub_obj, LV_LABEL_LONG_EXPAND);
// 		lv_label_set_text(sub_obj, sub_string);

// 		lv_obj_set_style_local_text_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_SUB_FONT_COLOR));
// 		lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// 	}

// 	/* 子控件 开关  */
// 	if(type == SETTING_SUB_SWITCH)
// 	{
// 		sub_obj = lv_switch_create(obj, NULL);
// 		lv_obj_set_click(sub_obj, false);
// 		lv_obj_set_size(sub_obj, 51, 26);
// 		lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -22, 0);
// 		lv_obj_set_style_local_bg_color(sub_obj, LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, lv_color_hex(THEAME_COLOR_DEFAULT));

// 	}

// 	/* 子控件 ID */
// 	if(sub_id > 0)
// 	{
// 		lv_obj_set_id(sub_obj, sub_id);
// 	}

// 	return obj;
// }


// /*************************************************************************
//  * @brief  带按钮的弹窗，的取消按钮的默认回调，仅删除弹窗
//  * @date   2022-11-10 14:10
//  * @author xiaoele
//  **************************************************************************/
// static void msg_window_with_button_def_cancel(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	lv_obj_del(obj->parent->parent);
// }

// /*************************************************************************
//  * @brief  带按钮的弹窗，的确认按钮的默认回调，仅删除弹窗
//  * @date   2022-11-10 14:10
//  * @author xiaoele
//  **************************************************************************/
// static void msg_window_with_button_def_cofirm(lv_obj_t *obj)
// {
// 	LOG_WHITE("click \n");
// 	lv_obj_del(obj->parent->parent);
// }

// static void msg_window_cont_click(lv_obj_t *obj)
// {
// 	lv_obj_del(obj);
// }


// /*************************************************************************
//  * @brief  有确认和取消按钮的弹窗
//  * @date   2022-11-04 16:15
//  * @author xiaoele
//  * @param  cancel_func  取消按钮的回调
//  * @param  confirm_func 确认按钮的回调
//  **************************************************************************/
// lv_obj_t *msg_window_with_button(int w,int h ,void (*bg_func)(lv_obj_t *), void (*cancel_func)(lv_obj_t *), void (*confirm_func)(lv_obj_t *))
// {
// 	lv_obj_t *obj = lv_cont_create(lv_scr_act(), NULL);
// 	lv_obj_set_size(obj, LV_HOR_RES_MAX, LV_VER_RES_MAX);	// 一整个屏幕大小
// 	lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);		

// 	/* 有消息弹窗时, 除了弹窗外,其他区域变为深色, 有那个 聚焦的效果 */
// 	lv_obj_set_style_local_bg_opa(obj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_80);	
// 	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));

// 	static obj_click_data bg_click = {NULL, NULL, NULL};
// 	bg_click.up = bg_func;
// 	if(bg_func == NULL)
// 	{	
// 		/* 如果为空，则使用默认的回调 */
// 		bg_click.up = msg_window_cont_click;
// 	}
// 	obj_click_event_listen(obj, &bg_click);

// 	lv_obj_t *cont = lv_cont_create(obj, NULL);
// 	lv_obj_set_size(cont, w, h);
// 	lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
// 	lv_obj_set_auto_realign(cont, true);
// 	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// 	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x101010));
// 	lv_obj_set_style_local_value_font(cont,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// 	lv_obj_set_style_local_value_ofs_y(cont,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -72/2);


// 	/* 灰色边框 */
// 	lv_obj_set_style_local_border_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT|LV_STATE_FOCUSED, LV_OPA_100);
// 	lv_obj_set_style_local_border_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT|LV_STATE_FOCUSED, lv_color_hex(0x5C5C5C));
// 	lv_obj_set_style_local_border_width(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
// 	lv_obj_set_style_local_border_side(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);

// 	/* 取消按钮 */
// 	lv_obj_t *cancel_btn = lv_btn_create(cont, NULL);
// 	lv_obj_set_size(cancel_btn, lv_obj_get_width(cont)/2, 72);
	
// 	lv_obj_align(cancel_btn, cont, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
// 	lv_obj_set_auto_realign(cancel_btn, true);
// 	lv_obj_set_style_local_value_str(cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "×");
// 	lv_obj_set_style_local_value_font(cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(60));
// 	lv_obj_set_style_local_value_ofs_y(cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -6);
// 	lv_obj_set_style_local_bg_opa(cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// 	lv_obj_set_style_local_bg_color(cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x202020));
// 	lv_obj_set_style_local_bg_color(cancel_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x303030));

// 	static obj_click_data cancel_click = {NULL, NULL, NULL};
// 	cancel_click.up = cancel_func;
// 	if(cancel_func == NULL)
// 	{	
// 		/* 如果为空，则使用默认的回调 */
// 		cancel_click.up = msg_window_with_button_def_cancel;
// 	}
// 	obj_click_event_listen(cancel_btn, &cancel_click);


// 	/* 确认按钮 */
	
// 	lv_obj_t *confirm_btn = lv_btn_create(cont, NULL);
// 	lv_obj_set_size(confirm_btn, lv_obj_get_width(cont)/2, 72);
// 	lv_obj_align(confirm_btn, cont, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
// 	lv_obj_set_auto_realign(confirm_btn, true);
// 	lv_obj_set_style_local_value_str(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "√");
// 	lv_obj_set_style_local_value_font(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(60));
// 	lv_obj_set_style_local_value_ofs_y(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -8);
// 	lv_obj_set_style_local_bg_opa(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT|LV_STATE_PRESSED, LV_OPA_100);
// 	lv_obj_set_style_local_bg_color(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_COLOR_DEFAULT));
// 	lv_obj_set_style_local_bg_color(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(THEAME_COLOR_PRESS));


// 	static obj_click_data confirm_click = {NULL, NULL, NULL};
// 	confirm_click.up = confirm_func;
// 	if(confirm_func == NULL)
// 	{	
// 		/* 如果穿日的参数为空，则使用默认的回调 */
// 		confirm_click.up = msg_window_with_button_def_cofirm;
// 	}
// 	obj_click_event_listen(confirm_btn, &confirm_click);

// 	return cont;
// }


// /*************************************************************************
//  * @brief  创建单个按钮的弹窗
//  * @date   2023-01-31 09:25
//  * @author xiaole
//  * @param  bg_func
//  * @param  confirm_func
//  **************************************************************************/
// lv_obj_t *msg_window_with_button_one(int w, int h, void (*bg_func)(lv_obj_t *), void (*confirm_func)(lv_obj_t *))
// {
// 	lv_obj_t *obj = lv_cont_create(lv_scr_act(), NULL);
// 	lv_obj_set_size(obj, LV_HOR_RES_MAX, LV_VER_RES_MAX);	// 一整个屏幕大小
// 	lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);		

// 	/* 有消息弹窗时, 除了弹窗外,其他区域变为深色, 有那个 聚焦的效果 */
// 	lv_obj_set_style_local_bg_opa(obj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70);	
// 	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));

// 	static obj_click_data bg_click = {NULL, NULL, NULL};
// 	bg_click.up = bg_func;
// 	if(bg_func == NULL)
// 	{	
// 		/* 如果为空，则使用默认的回调 */
// 		bg_click.up = msg_window_cont_click;
// 	}
// 	obj_click_event_listen(obj, &bg_click);


// 	lv_obj_t *cont = lv_cont_create(obj, NULL);
// 	lv_obj_set_size(cont, w, h);
// 	lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);
// 	lv_obj_set_auto_realign(cont, true);
// 	lv_obj_set_style_local_bg_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// 	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1C1C1C));
// 	lv_obj_set_style_local_value_font(cont,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// 	lv_obj_set_style_local_value_ofs_y(cont,  LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -72/2);


// 	/* 灰色边框 */
// 	lv_obj_set_style_local_border_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// 	lv_obj_set_style_local_border_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x5C5C5C));
// 	lv_obj_set_style_local_border_width(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
// 	lv_obj_set_style_local_border_side(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);

// 	lv_obj_set_style_local_border_opa(cont, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, LV_OPA_100);
// 	lv_obj_set_style_local_border_color(cont, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x5C5C5C));

// 	/* 确认按钮 */
	
// 	lv_obj_t *confirm_btn = lv_btn_create(cont, NULL);
// 	lv_obj_set_size(confirm_btn, lv_obj_get_width(cont), 72);
// 	lv_obj_align(confirm_btn, cont, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
// 	lv_obj_set_auto_realign(confirm_btn, true);
// 	lv_obj_set_style_local_value_str(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, "√");
// 	lv_obj_set_style_local_value_font(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(60));
// 	lv_obj_set_style_local_value_ofs_y(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, -8);
// 	lv_obj_set_style_local_bg_opa(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// 	lv_obj_set_style_local_bg_color(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_COLOR_DEFAULT));
// 	lv_obj_set_style_local_bg_color(confirm_btn, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(THEAME_COLOR_PRESS));


// 	static obj_click_data confirm_click = {NULL, NULL, NULL};
// 	confirm_click.up = confirm_func;
// 	if(confirm_func == NULL)
// 	{	
// 		/* 如果穿日的参数为空，则使用默认的回调 */
// 		confirm_click.up = msg_window_with_button_def_cofirm;
// 	}
// 	obj_click_event_listen(confirm_btn, &confirm_click);

// 	return cont;
// }


// /*************************************************************************
//  * @brief  创建正在加载的弹窗
//  * @date   2022-11-09 09:45
//  * @author xiaoele
//  * @param  str  显示的文字
//  * @param  is_loading 加载的动画
//  * @note   600x360, 820x480
//  **************************************************************************/
// // lv_obj_t *msg_window_create(char *str, bool is_loading, int width, int height, void (*cancel_func)(lv_obj_t *))
// // {
// // 	lv_obj_t *cont = lv_cont_create(lv_scr_act(), NULL);
// // 	lv_obj_set_size(cont, LV_HOR_RES_MAX, LV_VER_RES_MAX);	// 一整个屏幕大小
// // 	lv_obj_align(cont, NULL, LV_ALIGN_CENTER, 0, 0);		

// // 	/* 有消息弹窗时, 除了弹窗外,其他区域变为深色, 有那个 聚焦的效果 */
// // 	lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70);	
// // 	lv_obj_set_style_local_bg_color(cont, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));	

// // 	lv_obj_t *obj = lv_cont_create(cont, NULL);
// // 	lv_obj_set_size(obj, width, height);
// // 	lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);
// // 	lv_obj_set_auto_realign(obj, true);
	
// // 	if(str != NULL)
// // 	{
// // 		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, str);
// // 	}
// // 	lv_obj_set_style_local_value_font(obj, LV_MSGBOX_PART_BG, LV_STATE_DEFAULT, THEAME_FONT_SIZE);	
// // 	lv_obj_set_style_local_value_align(obj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_CENTER);
// // 	lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// // 	lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1C1C1C));

// // 	/* 灰色边框 */
// // 	lv_obj_set_style_local_border_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x5C5C5C));
// // 	lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 1);
// // 	lv_obj_set_style_local_border_side(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_FULL);
// // 	lv_obj_set_style_local_border_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, LV_OPA_100);
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x5C5C5C));

// // 	if(is_loading)
// // 	{
// // 		lv_obj_t *preload = lv_spinner_create(obj, NULL);
// // 		lv_obj_set_size(preload, 120, 120);
// // 		lv_obj_align(preload, preload->parent, LV_ALIGN_CENTER, 0, 0);
// // 		lv_obj_set_style_local_line_color(preload, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, lv_color_hex(THEAME_COLOR_DEFAULT));
// // 		lv_obj_set_style_local_line_color(preload, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, lv_color_hex(0XFFFFFF));
// // 		lv_obj_set_style_local_line_width(preload, LV_SPINNER_PART_INDIC, LV_STATE_DEFAULT, 12);
// // 		lv_obj_set_style_local_line_width(preload, LV_SPINNER_PART_BG, LV_STATE_DEFAULT, 12);

// // 		/* 如果有加载框， 需要把文字向下偏移 */
// // 		lv_obj_set_style_local_value_ofs_y(obj, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, height/3);
// // 	}

// // 	if(cancel_func)
// // 	{
// // 		static rom_bin_info img = rom_bin_info_get(ROM_UI_MONITOR_X_PNG);
// // 		static obj_click_data click_cb = {NULL, NULL, NULL};
// // 		click_cb.up = cancel_func;


// // 		lv_obj_t *cancel = lv_obj_create(obj, NULL);
// // 		lv_obj_set_size(cancel, 30, 30);
// // 		lv_obj_align(cancel, NULL, LV_ALIGN_IN_TOP_RIGHT, -16, 16);
// // 		lv_obj_set_ext_click_area(cancel, 30, 30, 30, 30);
// // 		obj_click_event_listen(cancel, &click_cb);
// // 		lv_obj_set_style_local_pattern_image(cancel, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
// // 	}

// // 	return obj;
// // }




// /***
// ** 日期: 2022-04-28 17:20
// ** 作者: leo.liu
// ** 函数作用：消息框按键矩阵按下回调函数
// ** 返回参数说明：
// ***/
// static void msg_window_click(lv_obj_t *obj)
// {
// 	uint16_t btn_id = lv_btnmatrix_get_active_btn(obj);
// 	if (btn_id == LV_BTNMATRIX_BTN_NONE)
// 		return;

// 	if (lv_btnmatrix_get_btn_ctrl(obj, btn_id, LV_BTNMATRIX_CTRL_DISABLED))
// 		return;

// 	/* 不是单选模式，又被选中了 */
// 	if(lv_btnmatrix_get_btn_ctrl(obj, btn_id, LV_BTNMATRIX_CTRL_CHECK_STATE) && !lv_btnmatrix_get_one_check(obj))
// 	{
// 		lv_btnmatrix_clear_btn_ctrl(obj, btn_id, LV_BTNMATRIX_CTRL_CHECK_STATE);
// 	}
// 	else
// 	{
// 		lv_btnmatrix_set_btn_ctrl(obj, btn_id, LV_BTNMATRIX_CTRL_CHECK_STATE);
// 	}

// 	LOG_B_WHITE("check %d\n", btn_id);

// }




// /*************************************************************************
//  * @brief  消息弹窗创建选项
//  * @date   2022-11-05 15:47
//  * @author xiaoele
//  * @param  single 是否为单选
//  **************************************************************************/
// // lv_obj_t *msg_window_btnmatrix_create(lv_obj_t *parent, const char *map[], int id, bool single)
// // {
// // 	lv_obj_t *btnmatrix = lv_btnmatrix_create(parent, NULL);
// // 	if (btnmatrix == NULL)
// // 	{
// // 		LOG_B_RED("btnmatrix create failed \n");
// // 		return NULL;
// // 	}

// // 	lv_obj_set_size(btnmatrix, 600, lv_obj_get_height(btnmatrix->parent) -72);
// // 	lv_obj_align(btnmatrix, parent, LV_ALIGN_IN_LEFT_MID, 80, -72/2);

// // 	if(id > 0)
// // 	{
// // 		lv_obj_set_id(btnmatrix, id);
// // 	}

// // 	if(map != NULL)
// // 	{
// // 		lv_btnmatrix_set_map(btnmatrix, map);
// // 	}
// // 	lv_btnmatrix_set_align(btnmatrix, LV_LABEL_ALIGN_LEFT, 84, 0);
// // 	lv_btnmatrix_set_recolor(btnmatrix, true);
// // 	lv_obj_set_style_local_text_font(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);

// // 	/* 清除所有选中的属性 */
// // 	lv_btnmatrix_clear_btn_ctrl_all(btnmatrix, LV_BTNMATRIX_CTRL_CHECK_STATE);
// // 	static obj_click_data click_data = obj_click_data_create(NULL, msg_window_click);
// // 	obj_click_event_listen(btnmatrix, &click_data);


// // 	if(single == true)
// // 	{	
// // 		/* 单选 */
// // 		static rom_bin_info image_un = rom_bin_info_get(ROM_UI_CIRCEL_OPTION_UN_PNG);
// // 		static rom_bin_info image_ck = rom_bin_info_get(ROM_UI_CIRCEL_OPTION_CK_PNG);
// // 		lv_obj_set_style_local_pattern_image(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, &image_un);
// // 		lv_obj_set_style_local_pattern_image(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_CHECKED, &image_ck);

// // 		// lv_btnmatrix_set_pattern_image(btnmatrix, 0, &image_un);
// // 		lv_btnmatrix_set_one_check(btnmatrix, true);
// // 	}
// // 	else
// // 	{
// // 		/* 多选 */
// // 		static rom_bin_info image_un = rom_bin_info_get(ROM_UI_BOX_OPTION_UN_PNG);
// // 		static rom_bin_info image_ck = rom_bin_info_get(ROM_UI_BOX_OPTION_CK_PNG);
// // 		lv_obj_set_style_local_pattern_image(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, &image_un);
// // 		lv_obj_set_style_local_pattern_image(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_CHECKED, &image_ck);
// // 		lv_btnmatrix_set_one_check(btnmatrix, false);
// // 	}
// // 	lv_obj_set_style_local_pattern_align(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
	
// // 	/* 按钮禁用时，不显示选项 */
// // 	lv_obj_set_style_local_pattern_image(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, NULL);
// // 	lv_obj_set_style_local_text_color(btnmatrix, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, lv_color_hex(0xFFFFFF));

// // 	return btnmatrix;
// // }


// /*************************************************************************
//  * @brief  设置界面 长条（占屏幕宽度大小）
//  * @date   2022-11-05 15:48
//  * @author xiaoele
//  * @param  num 第几个
//  * @param  main_string 主要控件显示的文本
//  * @param  main_id	主要空间的ID
//  * @param  sub_string
//  * @param  show_arr	是否显示右边的箭头
//  * @param  type	类型	SETTING_SUB_NONE
//  * @param  click_cb	点击的回调
//  **************************************************************************/
// // lv_obj_t * setting_long_obj_template(lv_obj_t *parent, int num, const char *main_string, int main_id, const char *sub_string, int sub_id, bool show_arr, SETTING_SUB_TYPE type, obj_click_data * click_cb)
// // {	
	
// // 	lv_obj_t *obj = lv_obj_create(parent == NULL ? lv_scr_act() : parent, NULL);
// // 	lv_obj_set_size(obj,LV_HOR_RES_MAX -56*2, 70);
// // 	lv_obj_align_x(obj, NULL, LV_ALIGN_IN_TOP_MID, 0);
// // 	lv_obj_set_y(obj, 75*num);
// // 	lv_page_glue_obj(obj, true);
// // 	lv_obj_set_ext_click_area(obj, 16, 16, 0, 0);
	
// // 	if(main_id > 0)	lv_obj_set_id(obj, main_id);

// // 	/* 点击回调 */
// // 	if(click_cb != NULL)
// // 	{
// // 		obj_click_event_listen(obj, click_cb);
// // 	}

// // 	/* 右边的灰色箭头 */
// // 	if(show_arr)
// // 	{	
// // 		lv_obj_t *icon = lv_obj_create(obj, NULL);
// // 		lv_obj_set_click(icon, false);
// // 		lv_obj_set_size(icon, 20, 40);
// // 		lv_obj_align(icon, icon->parent, LV_ALIGN_IN_RIGHT_MID, -22, 0);
// // 		lv_obj_set_auto_realign(icon, true);
// // 		static rom_bin_info img = rom_bin_info_get(ROM_UI_SETTING_SUB_ARROW_RIGHT_DEF_PNG);
// // 		lv_obj_set_style_local_pattern_image(icon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img);
// // 	}

// // 	/* 底部边框 */
// // 	lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
// // 	lv_obj_set_style_local_border_side(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x404040));
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));

// // 	/* 按钮的文字 */
// // 	if(main_string != NULL)
// // 	{
// // 		lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, main_string);
// // 		lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
// // 		lv_obj_set_style_local_value_ofs_x(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 4);
// // 		lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// // 	}

	
// // 	if(type == SETTING_SUB_NONE)
// // 	{
// // 		return obj;
// // 	}

// // 	/* 以下控件只能选其一 */
// // 	if((sub_string != NULL) && type == SETTING_SUB_SWITCH)
// // 	{
// // 		LOG_B_RED("SW控件不能显示附加文本\n");
// // 	}
// // 	lv_obj_t *sub_obj = NULL;

// // 	/* 子控件 文本  */
// // 	if(type == SETTING_SUB_LABEL)
// // 	{
// // 		sub_obj = lv_label_create(obj, NULL);
// // 		lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -55, 0);
// // 		lv_obj_set_auto_realign(sub_obj, true);
// // 		lv_obj_set_click(sub_obj, false);
		
		

// // 		lv_label_set_align(sub_obj, LV_LABEL_ALIGN_RIGHT);
// // 		lv_label_set_long_mode(sub_obj, LV_LABEL_LONG_EXPAND);
// // 		lv_label_set_text(sub_obj, sub_string);
// // 		lv_obj_set_style_local_text_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_SUB_FONT_COLOR));
// // 		lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// // 	}else if(type == SETTING_SUB_SWITCH)
// // 	{
// // 		sub_obj = lv_switch_create(obj, NULL);
// // 		lv_obj_set_click(sub_obj, false);
// // 		lv_obj_set_size(sub_obj, 51, 26);
// // 		lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -22, 0);
// // 		lv_obj_set_style_local_bg_color(sub_obj, LV_SWITCH_PART_INDIC, LV_STATE_CHECKED, lv_color_hex(THEAME_COLOR_DEFAULT));

// // 	}else if(type == SETTING_SUB_TEXTAREA){
// // 		sub_obj = lv_textarea_create(obj, NULL);
// // 		lv_obj_set_size(sub_obj, 260, 60);
// // 		lv_obj_align(sub_obj, sub_obj->parent, LV_ALIGN_IN_RIGHT_MID, -55, 0);
// // 		lv_obj_set_auto_realign(sub_obj, true);

// // 		if(click_cb != NULL)
// // 		{
// // 			obj_click_event_listen(sub_obj, click_cb);
// // 		}
		


// // 		lv_textarea_set_text(sub_obj, sub_string);
// // 		lv_textarea_set_text_align(sub_obj, LV_LABEL_ALIGN_RIGHT);
// // 		lv_textarea_set_cursor_click_pos(sub_obj, false);
// // 		lv_textarea_set_cursor_hidden(sub_obj, true);
// // 		lv_textarea_set_one_line(sub_obj, true);
// // 		lv_textarea_set_max_length(sub_obj, 16);
// // 		lv_textarea_set_cursor_pos(sub_obj, strlen(sub_string));




// // 		lv_obj_set_style_local_text_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_SUB_FONT_COLOR));
// // 		lv_obj_set_style_local_text_font(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// // 		lv_obj_set_style_local_bg_opa(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_80);
// // 		lv_obj_set_style_local_bg_color(sub_obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x000000));

// // 	}

// // 	/* 子控件 ID */
// // 	if(sub_id > 0)	lv_obj_set_id(sub_obj, sub_id);

// // 	return obj;
// // }


// /*************************************************************************
//  * @brief  设置界面 左右箭头创建
//  * @date   2022-11-09 15:22
//  * @author xiaoele
//  * @param  arrow  是否创建箭头
//  * @param  left_func  左键回调
//  * @param  right_func 右键回调
//  **************************************************************************/
// // lv_obj_t *setting_left_right_arrow_create(lv_obj_t *parent, int num, bool arrow, obj_click_data *click_left, obj_click_data *click_right)
// // {
// // 	static rom_bin_info img_left_def = rom_bin_info_get(ROM_UI_SETTING_SUB_ARROW_LEFT_DEF_PNG);
// // 	static rom_bin_info img_left_press = rom_bin_info_get(ROM_UI_SETTING_SUB_ARROW_LEFT_PRESS_PNG);
// // 	static rom_bin_info img_right_def = rom_bin_info_get(ROM_UI_SETTING_SUB_ARROW_RIGHT_DEF_PNG);
// // 	static rom_bin_info img_right_press = rom_bin_info_get(ROM_UI_SETTING_SUB_ARROW_RIGHT_PRESS_PNG);
	
// // 	if(arrow)
// // 	{
// // 		lv_obj_t *left = lv_obj_create(parent, NULL);
// // 		lv_obj_set_ext_click_area(left, 15, 15, 15, 15);
// // 		lv_obj_set_size(left, 60, 60);
// // 		lv_obj_align(left, left->parent, LV_ALIGN_IN_RIGHT_MID, -278, 0);
// // 		lv_obj_set_style_local_pattern_image(left, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_left_def);
// // 		lv_obj_set_style_local_pattern_image(left, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, &img_left_press);
// // 		obj_click_event_listen(left, click_left);

// // 		// lv_obj_set_style_local_bg_opa(left, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
// // 		// lv_obj_set_style_local_bg_color(left, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

		
// // 		lv_obj_t *right = lv_obj_create(parent, left);
// // 		lv_obj_set_ext_click_area(right, 15, 15, 15, 15);
// // 		lv_obj_set_size(right, 60, 60);
// // 		lv_obj_align(right, right->parent, LV_ALIGN_IN_RIGHT_MID, 0, 0);
// // 		lv_obj_set_style_local_pattern_image(right, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &img_right_def);
// // 		lv_obj_set_style_local_pattern_image(right, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, &img_right_press);
// // 		obj_click_event_listen(right, click_right);

// // 	}


// // 	lv_obj_t *label = lv_label_create(parent, NULL);
// // 	lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL);
// // 	lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
// // 	lv_label_set_text_fmt(label, "%d", num);
// // 	lv_obj_set_size(label, 220, 50);
// // 	lv_obj_align(label, label->parent, LV_ALIGN_IN_RIGHT_MID, -54, 8);
// // 	lv_obj_set_style_local_text_font(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// // 	lv_obj_set_style_local_text_color(label, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(THEAME_SUB_FONT_COLOR));



// // 	return label;

// // }



// /*************************************************************************
//  * @brief  滚筒创建的模板
//  * @date   2022-11-08 08:24
//  * @author xiaoele
//  * @param  
//  **************************************************************************/
// lv_obj_t * setting_time_roller_template(lv_obj_t *parent, lv_obj_t *icon, int y, int width, int height, int min, int max, int cur, int id, obj_click_data *click_data)
// {
// 	lv_obj_t *roller = lv_roller_create(parent, NULL);
// 	lv_obj_set_id(roller, id);
// 	lv_obj_set_size(roller, width, height);
// 	lv_obj_set_y(roller, y);
// 	lv_obj_align_x(roller, icon, LV_ALIGN_CENTER, 0);			
// 	lv_obj_set_style_local_text_line_space(roller, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, 30);
// 	// lv_obj_set_style_local_text_font(roller, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);


// 	char opt[512] = {0};
// 	for (int i = min; i <= max; i++)
// 	{
// 		char buf[8] = {0};
// 		snprintf(buf, 8,"%02d%s", i, i == max ? "" : "\n");
// 		strcat(opt, buf);

// 	}
// 	lv_roller_set_options(roller, opt, LV_ROLLER_MODE_INFINITE);

// 	lv_obj_set_style_local_bg_opa(roller, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, LV_OPA_0);
// 	// lv_obj_set_style_local_bg_color(roller, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, lv_color_hex(0x0000FF));
	
// 	lv_obj_set_ext_click_area(roller, 0, 0, 60, 60);
// 	lv_roller_set_visible_row_count(roller, 1);
// 	lv_roller_set_selected(roller, cur - min, false);		// 因为roller选项的特性。。。需要计算
// 	obj_click_event_listen(roller, click_data);
// 	roller->user_data = parent->user_data;



// 	return roller;
// }

// /*************************************************************************
//  * @brief  选项创建模板
//  * @date   2022-11-12 11:17
//  * @author xiaoele
//  * @param  id 控件的ID
//  * @param  string 文本
//  * @param  click_cb 点击回调
//  **************************************************************************/
// // lv_obj_t *setting_option_create_by_id_num_template(lv_obj_t *parent, int id, const char *string, obj_click_data *click_cb, bool single)
// // {
// //     lv_obj_t *obj = lv_obj_create(parent, NULL);
// //     lv_obj_set_id(obj, id);
// //     lv_obj_set_size(obj, LV_HOR_RES_MAX-56*2, 72);
// //     lv_obj_align(obj, parent, LV_ALIGN_IN_TOP_MID, 0, 75*id);
// // 	lv_page_glue_obj(obj, true);
	
    
// //     lv_obj_set_style_local_value_ofs_x(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 100);
// //     lv_obj_set_style_local_value_str(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, string);
// //     lv_obj_set_style_local_value_font(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, THEAME_FONT_SIZE);
// //     lv_obj_set_style_local_value_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);
// // 	lv_obj_set_style_local_value_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));

// // 	// lv_obj_set_style_local_bg_opa(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_20);
// // 	// lv_obj_set_style_local_bg_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));



// // 	static rom_bin_info def = rom_bin_info_get(ROM_UI_CIRCEL_OPTION_UN_PNG);
// // 	static rom_bin_info ck  = rom_bin_info_get(ROM_UI_CIRCEL_OPTION_CK_PNG);
// // 	if (!single){
// // 		def.offset = ROM_UI_BOX_OPTION_UN_PNG;
// // 		def.size   = ROM_UI_BOX_OPTION_UN_PNG_SIZE;

// // 		ck.offset = ROM_UI_BOX_OPTION_CK_PNG;
// // 		ck.size   = ROM_UI_BOX_OPTION_CK_PNG_SIZE;
// // 	}

// //     lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, &def);
// //     lv_obj_set_style_local_pattern_image(obj, LV_OBJ_PART_MAIN, LV_STATE_CHECKED, &ck);
// //     lv_obj_set_style_local_pattern_align(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_ALIGN_IN_LEFT_MID);



// //     /* 底部边框 */
// // 	lv_obj_set_style_local_border_width(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 2);
// // 	lv_obj_set_style_local_border_side (obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x404040));
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_FOCUSED, lv_color_hex(0x404040));
// // 	lv_obj_set_style_local_border_color(obj, LV_OBJ_PART_MAIN, LV_STATE_PRESSED, lv_color_hex(0x202020));

// //     obj_click_event_listen(obj, click_cb);

// //     return obj;
// // }




// // lv_obj_t *layout_kbd_all_create(void)
// // {	
// // 	lv_obj_t *kbd = lv_keyboard_create(lv_scr_act(), NULL);
// // 	lv_keyboard_set_mode(kbd, LV_KEYBOARD_MODE_TEXT_LOWER);
// // 	lv_keyboard_set_cursor_manage(kbd,false);
// // 	lv_obj_align(kbd, kbd, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

// // 	lv_obj_set_style_local_text_font(kbd,LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, FONT_SIZE(42));
// // 	lv_obj_set_style_local_pad_top(kbd, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT,4);
// // 	lv_obj_set_style_local_pad_left(kbd, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT,4);
// // 	lv_obj_set_style_local_pad_right(kbd, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT,4);
// // 	lv_obj_set_style_local_pad_inner(kbd, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT,4);
// // 	lv_obj_set_style_local_pad_bottom(kbd, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT,4);

// // 	lv_obj_set_style_local_bg_opa(kbd, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT,LV_OPA_100);
// // 	lv_obj_set_style_local_bg_opa(kbd, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED,LV_OPA_50);
// // 	lv_obj_set_style_local_bg_color(kbd, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT,  lv_color_hex(0x2A2A2A));
// // 	lv_obj_set_style_local_bg_color(kbd, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED,  lv_color_hex(0x3AC392));

// // 	lv_obj_set_style_local_bg_opa(kbd, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
// // 	lv_obj_set_style_local_bg_color(kbd, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT,  lv_color_hex(0x000000));


// // 	{
// // 		/*************************************************************************
// // 		 * @brief  修改键盘的按钮显示
// // 		 * @date   2022-11-08 16:26
// // 		 * @author xiaoele
// // 		 **************************************************************************/
// // 		static rom_bin_info del   = rom_bin_info_get(ROM_UI_KBD_DEL_PNG);
// // 		static rom_bin_info enter = rom_bin_info_get(ROM_UI_KBD_ENTER_PNG);
// // 		static rom_bin_info clear = rom_bin_info_get(ROM_UI_KBD_X_PNG);
// // 		static rom_bin_info left  = rom_bin_info_get(ROM_UI_KBD_LEFT_PNG);
// // 		static rom_bin_info space = rom_bin_info_get(ROM_UI_KBD_SPACE_PNG);
// // 		static rom_bin_info right = rom_bin_info_get(ROM_UI_KBD_RIGHT_PNG);
// // 		static rom_bin_info apply = rom_bin_info_get(ROM_UI_KBD_APPLY_PNG);

// // 		lv_keyboard_img_set(kbd,11,&del);
// // 		lv_keyboard_img_set(kbd,22,&enter);
// // 		lv_keyboard_img_set(kbd,35,&clear);
// // 		lv_keyboard_img_set(kbd,36,&left);
// // 		lv_keyboard_img_set(kbd,37,&space);
// // 		lv_keyboard_img_set(kbd,38,&right);
// // 		lv_keyboard_img_set(kbd,39,&apply);
// // 	}


// // 	return kbd;
// // }




// /*************************************************************************
//  * @brief  创建数字键盘
//  * @date   2022-11-12 13:49
//  * @author xiaoele
//  **************************************************************************/
// // lv_obj_t *layout_kbd_number_create(lv_obj_t *parent, obj_click_data *click_data, int id)
// // {
// //     lv_obj_t *keyboard = lv_keyboard_create(parent, NULL);
// // 	lv_obj_set_size(keyboard, 302, 400);
// //     lv_obj_align(keyboard, NULL, LV_ALIGN_IN_LEFT_MID, 150, 0);
// //     lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_NUM);
// // 	if(id > 0)
// // 	{
// // 		lv_obj_set_id(keyboard, id);
// // 	}
    
// //     /* 按钮 */
// // 	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, LV_OPA_10);
// // 	lv_obj_set_style_local_radius(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, 60);
// // 	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, lv_color_hex(0xFFFFFF));
// // 	lv_obj_set_style_local_bg_color(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_PRESSED, lv_color_hex(0x0096ff));
// //     lv_obj_set_style_local_text_font(keyboard, LV_KEYBOARD_PART_BTN, LV_STATE_DEFAULT, FONT_SIZE(42));

// //     /* 背景 */
// // 	lv_obj_set_style_local_bg_opa(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, LV_OPA_0);
// // 	lv_obj_set_style_local_pad_inner(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, 24);
// // 	lv_obj_set_style_local_border_opa(keyboard, LV_KEYBOARD_PART_BG, LV_STATE_DEFAULT, LV_OPA_0);


// //     {
        
// // 		static rom_bin_info clear = rom_bin_info_get(ROM_UI_KBD_X_PNG);
// //         static rom_bin_info del   = rom_bin_info_get(ROM_UI_KBD_DEL_PNG);
// //         lv_keyboard_img_set(keyboard, 9 ,&clear);
// //         lv_keyboard_img_set(keyboard, 11,&del);
// //     }

// //     obj_click_event_listen(keyboard, click_data);

// //     return keyboard;
// // }

// /*************************************************************************
//  * @brief  输入框创建
//  * @date   2022-11-15 07:55
//  * @author xiaoele
//  **************************************************************************/
// lv_obj_t *layout_kbd_number_textarea_create(lv_obj_t *obj, int y, int id, const char *str, bool edit)
// {
//     lv_obj_t *textarea = lv_textarea_create(obj, NULL);
//     lv_obj_set_id(textarea, id);
//     lv_obj_set_size(textarea, 320, 50);
//     lv_obj_align(textarea, textarea->parent, LV_ALIGN_IN_TOP_MID, 0, y);

// 	lv_textarea_set_text(textarea, "");
// 	lv_textarea_set_text_align(textarea, LV_LABEL_ALIGN_LEFT);
//     lv_textarea_set_pwd_show_time(textarea, 800);
//     lv_textarea_set_pwd_mode(textarea, true);
// 	lv_textarea_set_one_line(textarea, true);
// 	lv_textarea_set_max_length(textarea, 4);
// 	lv_textarea_set_cursor_pos(textarea, 0);
//     lv_textarea_set_cursor_click_pos(textarea, false);
//     lv_textarea_set_cursor_hidden(textarea, true);


//     lv_obj_set_style_local_border_width(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, 2);
// 	lv_obj_set_style_local_border_side(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, LV_BORDER_SIDE_BOTTOM);
// 	lv_obj_set_style_local_border_color(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, lv_color_hex(0x303030));
// 	lv_obj_set_style_local_border_color(textarea, LV_TEXTAREA_PART_BG, LV_STATE_FOCUSED, lv_color_hex(0x303030));

//     lv_obj_set_style_local_bg_opa(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, LV_OPA_0);
//     lv_obj_set_style_local_bg_opa(textarea, LV_TEXTAREA_PART_BG, LV_STATE_EDITED, LV_OPA_20);
//     lv_obj_set_style_local_bg_color(textarea, LV_TEXTAREA_PART_BG, LV_STATE_EDITED, lv_color_hex(THEAME_COLOR_DEFAULT));
    
//     // lv_obj_set_state(textarea, edit ? LV_STATE_EDITED : LV_STATE_DEFAULT);


//     int pos = lv_textarea_get_cursor_pos(textarea);
//     if (pos == 0)
// 	{
//         lv_obj_set_style_local_value_str(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, str);
//     }
//     else if (pos == 1)
// 	{
// 		lv_obj_set_style_local_value_str(textarea, LV_TEXTAREA_PART_BG, LV_STATE_DEFAULT, NULL);
// 	}
    
    
//     lv_obj_set_style_local_value_color(textarea, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x929292));
//     lv_obj_set_style_local_value_align(textarea, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_LABEL_ALIGN_LEFT);
//     lv_obj_set_style_local_value_font(textarea, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, FONT_SIZE(14));

//     return textarea;
// }






// /**
//  * @brief  创建一个通用page
//  * @date   2023-11-13 09:04:15
//  * @author xiaole
//  * @param  objid
//  * @return lv_obj_t*
//  * @note
//  */
// lv_obj_t* layout_page_create(int objid) {
// 	lv_obj_t* page = lv_page_create(lv_scr_act(), NULL);
// 	if (page == NULL) {
// 		printf("create page failed \n");
// 		return NULL;
// 	}

// 	lv_obj_set_id(page, objid);
// 	lv_obj_set_pos(page, 0, 75);
// 	lv_obj_set_size(page, 1024, 75*7);

// 	lv_page_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_DRAG);
// 	lv_obj_set_style_local_pad_top(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
// 	lv_obj_set_style_local_pad_left(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
// 	lv_obj_set_style_local_pad_right(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
// 	lv_obj_set_style_local_pad_bottom(page, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);

// 	return page;
// }