#ifndef LV_PAGE_SWITCH_H
#define LV_PAGE_SWITCH_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_subject_t switch_subject;

/// @brief 初始化页面切换主题
void page_switch_subject_init();

/// @brief 设置设备当前状态
/// @param stage LV_PAGE_STAGE_ENUM
void lv_dev_stage_set(uint8_t stage);

/// @brief 获取设备当前状态
/// @return LV_PAGE_STAGE_ENUM
uint8_t lv_dev_stage_get(void);


#ifdef __cplusplus
}
#endif

#endif