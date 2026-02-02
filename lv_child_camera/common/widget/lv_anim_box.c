#include "lv_anim_box.h"

//淡入淡出效果
static void lv_anim_box_fade_exec(lv_anim_t *a, int32_t v)
{
    lv_obj_set_style_opa((lv_obj_t *)a->var, v, 0);
}

//水平滑动效果
static void lv_anim_box_slide_x_exec(lv_anim_t *a, int32_t v)
{
    lv_obj_set_x((lv_obj_t *)a->var, v);
}

//垂直滑动效果
static void lv_anim_box_slide_y_exec(lv_anim_t *a, int32_t v)
{
    lv_obj_set_y((lv_obj_t *)a->var, v);
}

//缩放效果
static void lv_anim_box_scale_exec(lv_anim_t *a, int32_t v)
{
    lv_obj_set_style_transform_scale((lv_obj_t *)a->var, v, 0);
}

static lv_anim_t *create_fade_transition(lv_anim_params_t *param, bool is_new)
{
    lv_anim_init(&param->fade.anim);
    lv_anim_set_var(&param->fade.anim, *(param->fade.screen));
    if (!is_new)
    {
        //淡出
        lv_anim_set_values(&param->fade.anim, LV_OPA_COVER, LV_OPA_TRANSP);
        lv_anim_set_path_cb(&param->fade.anim, lv_anim_path_ease_out);
    }
    else
    {
        //淡入
        lv_anim_set_values(&param->fade.anim, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_anim_set_path_cb(&param->fade.anim, lv_anim_path_ease_in);
    }
    lv_anim_set_custom_exec_cb(&param->fade.anim, lv_anim_box_fade_exec);
    lv_anim_set_duration(&param->fade.anim, param->fade.duration);

    return &param->fade.anim;
}

static lv_anim_t *create_slide_transition(lv_anim_params_t *param, bool is_new)
{
    LV_UNUSED(is_new);
    int32_t start, end;
    lv_anim_custom_exec_cb_t anim_cb;

    //根据方向设置动画参数
    switch(param->slide.direction)
    {
        case LV_DIR_LEFT:
            start = lv_disp_get_hor_res(NULL); end = 0;
            anim_cb = lv_anim_box_slide_x_exec;
            break;
        case LV_DIR_RIGHT:
            start = -lv_disp_get_hor_res(NULL); end = 0;
            anim_cb = lv_anim_box_slide_x_exec;
            break;
        case LV_DIR_TOP:
            start = 0; end = -lv_disp_get_ver_res(NULL);
            anim_cb = lv_anim_box_slide_y_exec;
            break;
        case LV_DIR_BOTTOM:
            start = -lv_disp_get_ver_res(NULL); end = 0;
            anim_cb = lv_anim_box_slide_y_exec;
            break;
        default:
            return;
    }

    lv_anim_init(&param->slide.anim);
    lv_anim_set_var(&param->slide.anim, *(param->slide.screen));
    lv_anim_set_values(&param->slide.anim, start, end);
    lv_anim_set_custom_exec_cb(&param->slide.anim, anim_cb);
    lv_anim_set_path_cb(&param->slide.anim, lv_anim_path_linear);
    lv_anim_set_duration(&param->slide.anim, param->slide.duration);

    return &param->slide.anim;
}

static lv_anim_t *create_scale_transition(lv_anim_params_t *param, bool is_new)
{
    lv_anim_init(&param->scale.anim);
    lv_anim_set_var(&param->scale.anim, *(param->scale.screen));
    if (!is_new)
    {
        lv_anim_set_values(&param->scale.anim, 256, 0);
        lv_anim_set_path_cb(&param->scale.anim, lv_anim_path_ease_out);
    }
    else
    {
        lv_anim_set_values(&param->scale.anim, 0, 256);
        lv_anim_set_path_cb(&param->scale.anim, lv_anim_path_ease_in);
    }
    lv_anim_set_custom_exec_cb(&param->scale.anim, lv_anim_box_scale_exec);
    lv_anim_set_duration(&param->scale.anim, param->scale.duration);

    return &param->scale.anim;
}

static lv_anim_t *anim_param_allocation(int type, lv_anim_params_t *param, bool is_new)
{
    lv_anim_t *anim = NULL;

    switch (type)
    {
        case LV_ANIM_BOX_FADE:
            anim = create_fade_transition(param, is_new);
            break;
        case LV_ANIM_BOX_SLIDE:
            anim = create_slide_transition(param, is_new);
            break;
        case LV_ANIM_BOX_SCALE:
            anim = create_scale_transition(param, is_new);
            break;
        default:
            break;
    }

    return anim;
}

void lv_page_transition_anim_create(lv_anim_transt_data_t *transt_info, 
    lv_anim_completed_cb_t old_cb, void *param1, lv_anim_completed_cb_t new_cb, void *param2)
{
    static trans_manager_t trans_manager = {NULL, false};

    if (trans_manager.is_animating && trans_manager.timeline)
    {
        printf("###################\n");
        lv_anim_timeline_delete(trans_manager.timeline);
        trans_manager.is_animating = false;
    }

    trans_manager.timeline = lv_anim_timeline_create();

    //旧页面离开动画
    if (transt_info->old_type != LV_ANIM_BOX_NONE)
    {
        lv_anim_t *anim_old = anim_param_allocation(transt_info->old_type, &transt_info->old_params, false);
        lv_anim_set_completed_cb(anim_old, old_cb);
        lv_anim_set_user_data(anim_old, param1);
        lv_anim_timeline_add(trans_manager.timeline, 0, anim_old);
    }

    //新页面进入动画
    if (transt_info->new_type != LV_ANIM_BOX_NONE)
    {
        lv_anim_t *anim_new = anim_param_allocation(transt_info->new_type, &transt_info->new_params, true);
        lv_anim_set_completed_cb(anim_new, new_cb);
        lv_anim_set_user_data(anim_new, param2);
        lv_anim_timeline_add(trans_manager.timeline, 0, anim_new);
    }
    
    lv_anim_timeline_start(trans_manager.timeline);
    trans_manager.is_animating = true;

    return;
}

void lv_transition_anim_fade_param_set(lv_anim_params_pt params, lv_obj_t **screen, int32_t duration)
{
    params->fade.screen = screen;
    params->fade.duration = duration;
}

void lv_transition_anim_slide_param_set(lv_anim_params_pt params, lv_obj_t **screen, int32_t duration, int dir)
{
    params->slide.screen = screen;
    params->slide.duration = duration;
    params->slide.direction = dir;
}

void lv_transition_anim_scale_param_set(lv_anim_params_pt params, lv_obj_t **screen, int32_t duration)
{
    params->scale.screen = screen;
    params->scale.duration = duration;
}