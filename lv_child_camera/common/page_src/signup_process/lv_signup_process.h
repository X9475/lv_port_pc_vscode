#ifndef LV_SIGNUP_PROCESS_H
#define LV_SIGNUP_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_page_info.h"

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

#ifdef __cplusplus
}
#endif

#endif