#ifndef LV_ANIM_BOX_H
#define LV_ANIM_BOX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lvgl/lvgl.h"
#include "../page_switch/lv_page_switch.h"

//动画切换效果
typedef enum
{
    LV_ANIM_BOX_NONE,
    LV_ANIM_BOX_FADE,   //淡入淡出
    LV_ANIM_BOX_SLIDE,  //滑动
    LV_ANIM_BOX_SCALE,  //缩放
} LV_ANIM_TRANS_TYPE;

typedef union
{
    //淡入淡出参数
    struct {
        lv_anim_t anim;
        lv_obj_t **screen;
        int32_t duration;
    } fade;
    //滑动动画参数
    struct {
        lv_anim_t anim;
        lv_obj_t **screen;
        int direction;
        int32_t duration;
    } slide;
    //缩放动画参数
    struct {
        lv_anim_t anim;
        lv_obj_t **screen;
        int32_t duration;
    } scale;
} lv_anim_params_t, *lv_anim_params_pt;

typedef struct
{
    LV_ANIM_TRANS_TYPE old_type;
    lv_anim_params_t old_params;
    LV_ANIM_TRANS_TYPE new_type;
    lv_anim_params_t new_params;
    bool anim_support;//支持动画切换
} lv_anim_transt_data_t, *lv_anim_transt_data_pt;

typedef struct
{
    lv_anim_timeline_t *timeline;
    bool is_animating;
} trans_manager_t, *trans_manager_pt;

void lv_transition_anim_fade_param_set(lv_anim_params_pt params, lv_obj_t **screen, int32_t duration);
void lv_transition_anim_slide_param_set(lv_anim_params_pt params, lv_obj_t **screen, int32_t duration, int dir);
void lv_transition_anim_scale_param_set(lv_anim_params_pt params, lv_obj_t **screen, int32_t duration);

//动画时间轴创建执行
void lv_page_transition_anim_create(lv_anim_transt_data_t *transt_info, 
    lv_anim_completed_cb_t old_cb, void *param1, lv_anim_completed_cb_t new_cb, void *param2);

#ifdef __cplusplus
}
#endif

#endif