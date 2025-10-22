#ifndef LV_SWITCH_INTERFACE_H
#define LV_SWITCH_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_page_info.h"

//菜单
extern lv_subject_t menu_subject;
lv_page_info_pt lv_page_menu_info_get();

/***************************添加流程开始*************************************/
//扫描二维码
extern lv_subject_t qrcode_subject;
lv_page_info_pt lv_page_qrcode_info_get();

//二维码添加成功
extern lv_subject_t signup_success_subject;
lv_page_info_pt *lv_page_signup_success_info_get();

//二维码添加失败
extern lv_subject_t signup_failed_subject;
lv_page_info_pt *lv_page_signup_failed_info_get();

//第一次开机启动
extern lv_subject_t inital_startup_subject;
lv_page_info_pt lv_page_inital_startup_info_get();

//开始探索世界
extern lv_subject_t agent_start_subject;
lv_page_info_pt lv_page_agent_start_info_get();

//网络异常
extern lv_subject_t network_error_subject;
lv_page_info_pt lv_page_network_error_info_get();

/***************************设备设置*************************************/
//更多设置
extern lv_subject_t settings_more_subject;
lv_page_info_pt lv_page_settings_more_info_get();

/***************************相册功能*************************************/

extern lv_subject_t album_subject;
lv_page_info_pt lv_page_album_get();

extern lv_subject_t  album_none_subject;
lv_page_info_pt lv_page_album_none_get();

extern lv_subject_t album_share_subject;
lv_page_info_pt lv_page_album_share_get();

extern lv_subject_t album_share_suc_subject;
lv_page_info_pt lv_page_album_share_suc_get();

extern lv_subject_t  album_share_retry_subject;
lv_page_info_pt lv_page_album_share_retry_get();

extern lv_subject_t album_info_subject;
lv_page_info_pt lv_page_album_info_get();

extern lv_subject_t album_ai_subject;
lv_page_info_pt lv_page_album_ai_get();

extern lv_subject_t album_four_grid_subject;
lv_page_info_pt lv_page_album_four_grid_get();

extern lv_subject_t  album_delete_subject;
lv_page_info_pt lv_page_album_delete_get();

/***************************拍摄功能*************************************/
extern lv_subject_t shooting_photo_subject;
lv_page_info_pt lv_page_shooting_photo_get();

extern lv_subject_t shooting_mode_subject;
lv_page_info_pt lv_page_shooting_mode_get();

extern lv_subject_t shooting_switch_wait_subject;
lv_page_info_pt lv_page_shooting_switch_wait_get();

extern lv_subject_t shooting_switch_video_subject;
lv_page_info_pt lv_page_shooting_switch_video_get();

#ifdef __cplusplus
}
#endif

#endif