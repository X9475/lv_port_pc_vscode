#include "../lv_page_info.h"

//页面切换主题
lv_subject_t switch_subject;
static lv_switch_page_pt switch_menu;
extern lv_obj_t *top_screen;
extern lv_obj_t *act_screen;
extern lv_page_info_pt lv_page_menu_setting_info_get();
extern lv_page_info_pt lv_page_menu_info_get();

//不添加屏幕手势过滤列表
static uint32_t gesture_disable_list[] = {};

//设备当前状态
static LV_PAGE_STAGE_ENUM g_device_state = LV_PAGE_STAGE_ADDING;

/// @brief 页面切换观察者回调函数
static void page_switch_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
/// @brief 清楚旧的页面回调函数
static void page_switch_delete_page(void *param);
/// @brief 添加页面手势事件过滤
static bool page_add_gesture_event_filter(lv_page_info_pt newpage);
/// @brief 页面手势事件回调函数
static void page_gesture_event_cb(lv_event_t *e);
/// @brief 清除页面上的所有事件对象
static void lv_delete_all_event_obj(lv_obj_t *obj);

void lv_dev_stage_set(uint8_t stage)
{
    g_device_state = stage;
}

uint8_t lv_dev_stage_get(void)
{
    return g_device_state;
}

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

    LV_LOG_INFO("page switch begin");
    new_page->construct_cb();

    if (g_device_state == LV_PAGE_STAGE_RUNNING)
    {
        // lv_obj_add_event_cb(act_screen, page_gesture_event_cb, LV_EVENT_GESTURE, new_page);
        lv_obj_add_event_cb(top_screen, page_gesture_event_cb, LV_EVENT_GESTURE, new_page);
    }

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
        lv_delete_all_event_obj(old_page->page);
        lv_obj_del(old_page->page);
    }

    lv_free(switch_page);
    switch_page = NULL;
    return;
}

static bool page_add_gesture_event_filter(lv_page_info_pt newpage)
{
    bool result = false;

    uint8_t count = sizeof(gesture_disable_list) / sizeof(uint32_t);
    for (uint32_t i = 0; i < count; i++)
    {
        if (gesture_disable_list[i] == newpage->page_id) {
            result = true;
            break;
        }
    }

    return result;
}

static void page_gesture_event_cb(lv_event_t *e)
{
    static uint32_t menu = 0;
    static uint32_t setting_menu = 0;
    static uint32_t cur_page = PAGE_FUNCTIONAL_NONE;

    // lv_obj_t *target = lv_event_get_current_target(e);
    lv_page_info_pt page_info = lv_event_get_user_data(e);

    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
    lv_indev_wait_release(lv_indev_get_act());

    switch (dir)
    {
        case LV_DIR_BOTTOM:
            if (menu == 0)
            {
                menu = 1;
                cur_page = PAGE_FUNCTIONAL_MENU_SETTING;
                switch_menu = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                lv_memset(switch_menu, 0, sizeof(lv_switch_page_t));
                LV_ASSERT_MALLOC(switch_menu);
                switch_menu->new_page = lv_page_menu_setting_info_get();
                lv_subject_set_pointer(&switch_subject, switch_menu);

                //隐藏菜单页
                lv_obj_add_flag(lv_page_menu_info_get()->page, LV_OBJ_FLAG_HIDDEN);
            }
            else
            {
                lv_obj_add_flag(lv_page_menu_info_get()->page, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lv_page_menu_setting_info_get()->page, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        case LV_DIR_TOP:
            if (setting_menu == 0)
            {
                setting_menu = 1;
                cur_page = PAGE_FUNCTIONAL_MENU;
                switch_menu = (lv_switch_page_pt)lv_malloc(sizeof(lv_switch_page_t));
                lv_memset(switch_menu, 0, sizeof(lv_switch_page_t));
                LV_ASSERT_MALLOC(switch_menu);
                switch_menu->new_page = lv_page_menu_info_get();
                lv_subject_set_pointer(&switch_subject, switch_menu);

                //隐藏全局设置页
                lv_obj_add_flag(lv_page_menu_setting_info_get()->page, LV_OBJ_FLAG_HIDDEN);
            }
            else
            {
                lv_obj_add_flag(lv_page_menu_setting_info_get()->page, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(lv_page_menu_info_get()->page, LV_OBJ_FLAG_HIDDEN);
            }
            break;
        default:
            break;
    }
}

static void lv_delete_all_event_obj(lv_obj_t *obj)
{
    LV_ASSERT_NULL(obj);

    uint32_t event_cnt = lv_obj_get_event_count(obj);
    for (uint32_t i = 0; i < event_cnt; i++)
    {
        lv_obj_remove_event(obj, i);
    }

    return;
}
