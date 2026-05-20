#include "layout_define.h"
#include "ak_thread.h"

static int standby_display_picture_item = 0;

extern bool lv_jpg_decode_data(const char *file, rom_bin_info *info, int dst_w, int dst_h);

static rom_bin_info bg_img = rom_bin_raw_get();
static unsigned char *picture_data = NULL;

void stangby_picture_open()
{
	char picture_name[64] = {0};	
	if (picture_data == NULL)
	{
		picture_data = (unsigned char *)ak_mem_dma_alloc(MODULE_ID_GUI, 1024 * 600 * 4);
		rom_bin_raw_init(bg_img, picture_data, 1024, 600);
	}
	
	// printf("=========%s======%d====dma_alloc size is %d\n",__func__,__LINE__, 1024 * 600 * 4);

	

	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_video_mode_enable(false);

	const file_info *info =  media_file_info_get(FILE_TYPE_SD_PICTURE, standby_display_picture_item);
	printf("--------------[%d]=====[%s]=====[%s]\n",__LINE__,__func__,info->file_name);
	sprintf(picture_name,"%s%s",SD_PICTURE_PATH,info->file_name);
	lv_jpg_decode_data(picture_name, &bg_img, 1024, 600);
	lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
	lv_disp_set_bg_image(lv_disp_get_default(), &bg_img);

}





static void layout_bg_settinh_fail_task(lv_task_t * task)
{
	char picture_name[64] = {0};
	int total;
	media_file_total_get(FILE_TYPE_SD_PICTURE, &total, NULL);



    standby_display_picture_item++;
	if(standby_display_picture_item >= total)
	{
		standby_display_picture_item = 0;
	}
	const file_info *info = media_file_info_get(FILE_TYPE_SD_PICTURE,standby_display_picture_item);
	bzero(picture_name,sizeof(picture_name));
    sprintf(picture_name,"%s%s",SD_PICTURE_PATH,info->file_name);

	lv_jpg_decode_data(picture_name, &bg_img, 1024, 600);
	lv_disp_set_bg_opa(lv_disp_get_default(), LV_OPA_COVER);
	lv_disp_set_bg_image(lv_disp_get_default(), &bg_img);
}


static void standby_display_picture_btn_up(lv_obj_t* obj)
{

	goto_layout(pLAYOUT(home));
}


static void layout_tuya_monitor_photo()
{
	lv_obj_t *obj = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_pos(obj, 0, 0);
	lv_obj_set_size(obj, 1024, 600);

	lv_obj_set_style_local_bg_opa(obj,LV_CONT_PART_MAIN,LV_STATE_DEFAULT,LV_OPA_TRANSP);
	static obj_click_data click_data = obj_click_data_up_create(standby_display_picture_btn_up);
	obj_click_event_listen(obj,&click_data);

}


static void playback_sd_detection_task(void)
{
	// if(media_sdcard_insert_check() == true)
	// {
	// 	copy_flash_photo_to_sd();
	// }
	if(media_sdcard_insert_check() == false)
	{
		goto_layout(pLAYOUT(home));
	}
}


static void LAYOUT_ENTER_FUNC(standby_picture)
{
	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_disp_set_bg_opa(lv_disp_get_default(),LV_OPA_COVER );
	standby_timer_close();
	stangby_picture_open();
	layout_tuya_monitor_photo();
	lv_layout_task_create(layout_bg_settinh_fail_task, 4000, LV_TASK_PRIO_MID, NULL);
	layout_sd_state_callback_register(playback_sd_detection_task);
}

static void LAYOUT_QUIT_FUNC(standby_picture)
{
	standby_timer_restart(true);
	lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_disp_set_bg_opa(lv_disp_get_default(),LV_OPA_TRANSP );
	lv_disp_set_bg_image(lv_disp_get_default(), NULL);
	if (picture_data != NULL)
	{
		ak_mem_dma_free(picture_data);
		picture_data = NULL;
	}
	layout_sd_state_callback_register(NULL);
}

CREATE_LAYOUT(standby_picture);