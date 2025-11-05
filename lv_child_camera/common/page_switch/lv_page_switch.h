#ifndef LV_PAGE_SWITCH_H
#define LV_PAGE_SWITCH_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_subject_t switch_subject;

typedef enum
{
    TYPE_NONE = -1,
    TYPE_MENU_SETTING = 0,
    TYPE_FUNCTIONAL,
    TYPE_MENU,
} LV_PAGE_TYPE_ENUM;

/// @brief 初始化页面切换主题
void page_switch_subject_init();

void lv_dev_stage_set(uint8_t stage);
uint8_t lv_dev_stage_get(void);

void lv_page_type_set(int32_t type);
int32_t lv_page_type_get();

#ifdef __cplusplus
}
#endif

#endif