#ifndef LV_STACK_H
#define LV_STACK_H

#ifdef __cplusplus
extern "C" {
#endif

#define LV_STACK_DEPTH      5 //嵌套深度

/// @brief 初始化栈列表
void lv_stack_list_init();

/// @brief 压栈函数
/// @param page_ptr lv_page_info_pt
void lv_stack_push(void *page_ptr);

/// @brief 弹栈函数
/// @return lv_page_info_pt
void *lv_stack_pop();

#ifdef __cplusplus
}
#endif

#endif