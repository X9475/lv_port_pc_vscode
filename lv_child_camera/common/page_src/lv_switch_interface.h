#ifndef LV_SWITCH_INTERFACE_H
#define LV_SWITCH_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_page_info.h"
#include "toast/toast.h"
#include "abnormal/abnormal.h"

extern lv_obj_t *top_screen;
extern lv_obj_t *act_screen;

//菜单
extern lv_subject_t menu_subject;
lv_page_info_pt lv_page_menu_info_get();

//设置菜单
extern lv_subject_t menu_setting_subject;
lv_page_info_pt lv_page_menu_setting_info_get();

//锁屏界面
extern lv_subject_t screenlock_subject;
lv_page_info_pt lv_page_screenlock_info_get();

//关机界面
extern lv_subject_t poweroff_subject;
lv_page_info_pt lv_page_poweroff_info_get();

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

/***************************音频效果*************************************/
//音频效果
extern lv_subject_t audio_effect_subject;
lv_page_info_pt lv_page_audio_effect_info_get();

extern lv_subject_t audio_option_subject;
lv_page_info_pt lv_page_audio_photo_info_get();
lv_page_info_pt lv_page_audio_ring_bell_info_get();
lv_page_info_pt lv_page_audio_answer_bell_info_get();
lv_page_info_pt lv_page_audio_alarm_clock_info_get();
lv_page_info_pt lv_page_audio_notify_sound_info_get();
lv_page_info_pt lv_page_audio_keypad_tone_info_get();
lv_page_info_pt lv_page_audio_beyond_area_info_get();

/***************************设备设置*************************************/
//更多设置
extern lv_subject_t settings_more_subject;
lv_page_info_pt lv_page_settings_more_info_get();

//拍摄设置
extern lv_subject_t shoot_setting_subject;
lv_page_info_pt lv_page_shoot_setting_info_get();

//个性化设置
extern lv_subject_t indvid_setting_subject;
lv_page_info_pt lv_page_indvid_setting_info_get();

//设备信息
extern lv_subject_t equip_setting_subject;
lv_page_info_pt lv_page_equip_setting_info_get();

//单次录像时长
extern lv_subject_t record_time_subject;
lv_page_info_pt lv_page_record_time_info_get();

//息屏时间设置
extern lv_subject_t hold_time_subject;
lv_page_info_pt lv_page_hold_time_info_get();

//时间展示格式
extern lv_subject_t time_display_subject;
lv_page_info_pt lv_page_time_display_info_get();

//存储管理
extern lv_subject_t storage_manage_subject;
lv_page_info_pt lv_page_storage_manage_info_get();

//滚动振动开关
extern lv_subject_t roller_vibrat_subject;
lv_page_info_pt lv_page_roller_vibrat_info_get();

//振动幅度
extern lv_subject_t vibrat_amplitude_subject;
lv_page_info_pt lv_page_vibrat_amplitude_info_get();

//远程预览开关
extern lv_subject_t remote_preview_subject;
lv_page_info_pt lv_page_remote_preview_info_get();

//关于相机
extern lv_subject_t about_camera_subject;
lv_page_info_pt lv_page_about_camera_info_get();

//恢复出厂设置
extern lv_subject_t factory_restore_subject;
lv_page_info_pt lv_page_factory_restore_info_get();

//认证标志
extern lv_subject_t cert_mask_subject;
lv_page_info_pt lv_page_cert_mask_info_get();

//设置页二维码
extern lv_subject_t settingQr_subject;
lv_page_info_pt lv_page_settingQr_info_get();

//格式化确认
extern lv_subject_t format_confirm_subject;
lv_page_info_pt lv_page_format_confirm_info_get();

//屏保样式
extern lv_subject_t screensaver_style_subject;
lv_page_info_pt lv_page_screensaver_style_info_get();

//自定义按键
extern lv_subject_t indvid_key_subject;
lv_page_info_pt lv_page_indvid_key_info_get();

//语音控制开关
extern lv_subject_t voice_control_subject;
lv_page_info_pt lv_page_voice_control_info_get();

//辅助线开关
extern lv_subject_t auxiliary_line_subject;
lv_page_info_pt lv_page_auxiliary_line_info_get();

//设备音量
extern lv_subject_t device_volume_subject;
lv_page_info_pt lv_page_device_volume_info_get();

//屏幕亮度
extern lv_subject_t device_bright_subject;
lv_page_info_pt lv_page_device_bright_info_get();
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

/***************************打印机功能*************************************/
extern lv_subject_t ptr_conn_subject;
lv_page_info_pt lv_page_printer_conn_get();

extern lv_subject_t ptr_pre_subject;
lv_page_info_pt lv_page_printer_preview_get();

/***************************拍摄功能*************************************/
extern lv_subject_t shooting_photo_subject;
lv_page_info_pt lv_page_shooting_photo_get();

extern lv_subject_t shooting_mode_subject;
lv_page_info_pt lv_page_shooting_mode_get();

extern lv_subject_t shooting_switch_wait_subject;
lv_page_info_pt lv_page_shooting_switch_wait_get();

extern lv_subject_t shooting_switch_video_subject;
lv_page_info_pt lv_page_shooting_switch_video_get();

extern lv_subject_t shooting_adj_focus_subject;
lv_page_info_pt lv_page_shooting_adj_focus_get();

lv_page_info_pt lv_page_shooting_multi_filter_get();
extern lv_subject_t shooting_multi_filter_subject;

lv_page_info_pt lv_page_shooting_adj_param_get();
extern lv_subject_t shooting_adj_param_subject;

lv_page_info_pt lv_page_shooting_video_param_get();
extern lv_subject_t shooting_video_param_subject;

/***************************Ai对话功能*************************************/

lv_page_info_pt lv_page_aidialog_info_get();
extern lv_subject_t aidialog_subject;

lv_page_info_pt lv_page_aiphone_get();
extern lv_subject_t aiphone_subject;

lv_page_info_pt lv_page_message_center_get();
extern lv_subject_t message_center_subject;

#ifdef __cplusplus
}
#endif

#endif