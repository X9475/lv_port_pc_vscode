#ifndef LV_PAGE_SWITCH_H
#define LV_PAGE_SWITCH_H

#ifdef __cplusplus
extern "C" {
#endif

extern lv_subject_t switch_subject;

typedef enum
{
    TYPE_MENUSET = 0,     ///菜单设置页面一级
    TYPE_FUNC,            ///功能页面
    TYPE_MENU,            ///菜单页面
    TYPE_MAX,             ///最大页面类型数量
} LV_PAGE_TYPE_ENUM;

typedef struct
{
    lv_coord_t start_y;  //按下开始
    lv_coord_t end_y;    //释放结束
    uint32_t start_time; //开始时间
    bool is_pressed;     //是否按下
} lv_page_move_area_t;

//当前页面信息结构体
typedef struct
{
    uint32_t page_id; //页面ID
    void *current_page; //当前页面信息
} lv_page_info_current_t, *lv_page_info_current_pt;

/// @brief 初始化页面切换主题
void page_switch_subject_init();

void lv_dev_stage_set(uint8_t stage);
uint8_t lv_dev_stage_get(void);

void lv_page_type_set(int32_t type);
int32_t lv_page_type_get();

lv_page_info_current_pt lv_current_page_info_get();
void lv_current_page_info_set(uint32_t id, void *ptr);

#ifdef __cplusplus
}
#endif

#endif