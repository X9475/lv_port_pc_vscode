#include "../lv_page_info.h"

//页面切换主题
lv_subject_t switch_subject;

//设备当前状态
static LV_PAGE_STAGE_ENUM g_device_state = LV_PAGE_STAGE_ADDING;

/// @brief 清楚旧的页面回调函数
static void page_switch_delete_page(void *param);
/// @brief 页面切换观察者回调函数
static void page_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

void page_switch_subject_init()
{
    lv_subject_init_pointer(&switch_subject, NULL);
    lv_subject_add_observer(&switch_subject, page_switch_observer_cb, NULL);
    return;
}

static void page_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
    LV_UNUSED(observer);
    lv_switch_page_pt switch_page = lv_subject_get_pointer(subject);
    if (NULL == switch_page) return;

    // lv_page_info_pt old_page = switch_page->old_page;
    lv_page_info_pt new_page = switch_page->new_page;
    if (NULL == new_page) {
        lv_free(switch_page);
        switch_page = NULL;
        return;
    }

    LV_LOG_INFO("page switch begin");
    new_page->construct_cb();
    lv_scr_load_anim(new_page->page, LV_SCREEN_LOAD_ANIM_NONE, 0, 0, false);

    lv_refr_now(NULL);
    lv_async_call(page_switch_delete_page, switch_page);
    return;
}

static void page_switch_delete_page(void *param)
{
    lv_switch_page_pt switch_page = (lv_switch_page_pt)param;
    lv_page_info_pt old_page = switch_page->old_page;

    if (NULL != old_page) {
        old_page->destruct_cb();
        lv_obj_del(old_page->page);
    }

    lv_free(switch_page);
    switch_page = NULL;
    return;
}

void lv_dev_stage_set(uint8_t stage)
{
    g_device_state = stage;
    //TODO: 写配置
}

uint8_t lv_dev_stage_get(void)
{
    //TODO: 读配置
    return g_device_state;
}
